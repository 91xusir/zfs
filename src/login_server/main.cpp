#include "login.h"
#include <signal.h>
#include "license_check.h"

//--------------------------------------------------------------------
// 数据库线程：连接数据库，从接收命令列表获取命令，解析命令，并将命令添加到发送列表
//--------------------------------------------------------------------
void* db_thread(void* arg) {
    unsigned long btime = rtGetMilliseconds();  // 获取当前毫秒时间
    CLgsThread    tlgs;                         // 创建线程对象

    tlgs.SetThreadId((int)arg);  // 设置线程ID
    tlgs.Run();                  // 运行线程
    return (void*)0;             // 返回空指针
}

//--------------------------------------------------------------------
// WSDL线程：运行WSDL逻辑，处理与Web服务的交互
//--------------------------------------------------------------------
void* wsdl_thread(void* arg) {
    g_pWSDL->Run();  // 运行WSDL逻辑

    delete g_pWSDL;   // 删除WSDL对象，释放内存
    return (void*)0;  // 返回空指针
}

// 全局变量声明
CLoginServer* g_lgs;                    // 全局指针，指向登录服务器对象
TCmdList g_sendCmdList, g_recvCmdList;  // 全局命令列表，用于存储发送和接收的命令
SConfigLoginNet g_cfg;                  // 登录网络配置
CG_CmdPacket*   g_sendCmd;              // 全局指针，指向发送命令的数据包
long            g_dwLoginOrderID = 1;   // 登录订单ID，初始值为1

//--------------------------------------------------------------------
// 启动服务器：初始化并启动登录服务器
//--------------------------------------------------------------------
static bool StartupServer() {
    g_sendCmd = new CG_CmdPacket;  // 分配新的命令包
    if (g_sendCmd == NULL)
        return false;                           // 如果分配失败，返回false
    g_sendCmd->SetSize(MAX_CMD_PACKET_SIZE);    // 设置命令包的大小
    LOG1("cmd size = %d\n", sizeof(SListCmd));  // 输出命令大小的日志

    g_lgs = new CLoginServer;  // 分配新的登录服务器对象
    if (g_lgs == NULL)
        return false;  // 如果分配失败，返回false

    if (!g_lgs->Startup()) {
        ERR("StartupServer: Startup login server failed.\n");  // 如果服务器启动失败，输出错误日志
        return false;                                          // 返回false
    }

    LOG("Login server startup ok.\n");  // 服务器启动成功，输出日志
    return true;                        // 返回true，表示成功
}

//--------------------------------------------------------------------
// 关闭服务器：关闭登录服务器并释放资源
//--------------------------------------------------------------------
static void ShutdownServer() {
    g_lgs->Exit();     // 调用退出方法
    delete g_lgs;      // 删除登录服务器对象，释放内存
    delete g_sendCmd;  // 删除命令包对象，释放内存
}

//--------------------------------------------------------------------
// 运行服务器：执行服务器的运行逻辑
//--------------------------------------------------------------------
static bool RunServer() {
    return g_lgs->Run();  // 运行服务器并返回运行状态
}

//--------------------------------------------------------------------
// 信号处理函数：当收到退出信号时执行
//--------------------------------------------------------------------
static void OnExitProcess(int sig) {
    g_lgs->m_bNeedSendShutdown = 2;  // 设置服务器的关闭标志
}

#ifdef LINUX
#include <signal.h>
#include <core/rt2_core.h>

//--------------------------------------------------------------------
// 信号处理函数：处理SIGPIPE信号，通常用于处理向已关闭socket发送数据的情况
//--------------------------------------------------------------------
void sig_pipe(int signal) {}
#endif

//--------------------------------------------------------------------
// 主函数：服务器程序的入口
//--------------------------------------------------------------------
int main() {
#ifdef ENABLE_OPERATOR_NEW
    rt2_ActiveSafestr();  // 启用线程安全检查
#endif                    // ENABLE_OPERATOR_NEW

#ifdef LINUX
    signal(SIGPIPE, sig_pipe);  // 注册SIGPIPE信号处理函数
#endif
    signal(SIGTERM, OnExitProcess);  // 注册SIGTERM信号处理函数
    signal(SIGINT, OnExitProcess);   // 注册SIGINT信号处理函数

#ifdef LINUX
    const char* szPIDFileName = "login_server.pid";  // 定义PID文件名
    FILE*       fpPID;                               // 定义文件指针
#ifndef _DEBUG
    if (fpPID = fopen(szPIDFileName, "rt"))  // 检查PID文件是否已存在
    {
        printf("ERROR: PID file [%s] already existed.\n", szPIDFileName);  // 输出错误信息
        fclose(fpPID);                                                     // 关闭文件
        return 105;                                                        // 返回错误码105
    }
#endif
    if (fpPID = fopen(szPIDFileName, "wt"))  // 打开PID文件写入进程ID
    {
        fprintf(fpPID, "%d", getpid());  // 写入进程ID
        fclose(fpPID);                   // 关闭文件
    }
#endif

    if (!rtCoreInit("login_server.ini")) {
        LOG("Rt core init failed!");  // 核心初始化失败，输出日志
        return false;                 // 返回失败
    }

#ifdef LICENSE_CHECK
    if (!LicenseCheck(0))  // 执行许可证检查
    {
        LOG("License Check Failed\n");  // 检查失败，输出日志
        return 101;                     // 返回错误码101
    } else {
        LOG("License Check Success\n");  // 检查成功，输出日志
    }
#endif  // LICENSE_CHECK

    if (!StartupServer())  // 启动服务器
    {
        return 101;  // 启动失败，16秒后重启
    }
    RunServer();  // 运行服务器主逻辑
    int iReturnResult = 101;          // 默认返回101，表示重启
    if (g_lgs->m_tShutdownTime != 0)  // 检查是否设置了关闭时间
    {
        iReturnResult = 104;  // 如果设置了关闭时间，返回104，表示关机
    }
    ShutdownServer();  // 关闭服务器
#ifdef LINUX
    RtCoreFile().Delete(szPIDFileName);  // 删除PID文件
#endif
    rtCoreExit();  // 退出核心模块
    return iReturnResult;  // 返回最终结果
}
