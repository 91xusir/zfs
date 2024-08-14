#include "login.h"
#include <signal.h>
#include "license_check.h"

//--------------------------------------------------------------------
// ���ݿ��̣߳��������ݿ⣬�ӽ��������б��ȡ��������������������ӵ������б�
//--------------------------------------------------------------------
void* db_thread(void* arg) {
    unsigned long btime = rtGetMilliseconds();  // ��ȡ��ǰ����ʱ��
    CLgsThread    tlgs;                         // �����̶߳���

    tlgs.SetThreadId((int)arg);  // �����߳�ID
    tlgs.Run();                  // �����߳�
    return (void*)0;             // ���ؿ�ָ��
}

//--------------------------------------------------------------------
// WSDL�̣߳�����WSDL�߼���������Web����Ľ���
//--------------------------------------------------------------------
void* wsdl_thread(void* arg) {
    g_pWSDL->Run();  // ����WSDL�߼�

    delete g_pWSDL;   // ɾ��WSDL�����ͷ��ڴ�
    return (void*)0;  // ���ؿ�ָ��
}

// ȫ�ֱ�������
CLoginServer* g_lgs;                    // ȫ��ָ�룬ָ���¼����������
TCmdList g_sendCmdList, g_recvCmdList;  // ȫ�������б����ڴ洢���ͺͽ��յ�����
SConfigLoginNet g_cfg;                  // ��¼��������
CG_CmdPacket*   g_sendCmd;              // ȫ��ָ�룬ָ������������ݰ�
long            g_dwLoginOrderID = 1;   // ��¼����ID����ʼֵΪ1

//--------------------------------------------------------------------
// ��������������ʼ����������¼������
//--------------------------------------------------------------------
static bool StartupServer() {
    g_sendCmd = new CG_CmdPacket;  // �����µ������
    if (g_sendCmd == NULL)
        return false;                           // �������ʧ�ܣ�����false
    g_sendCmd->SetSize(MAX_CMD_PACKET_SIZE);    // ����������Ĵ�С
    LOG1("cmd size = %d\n", sizeof(SListCmd));  // ��������С����־

    g_lgs = new CLoginServer;  // �����µĵ�¼����������
    if (g_lgs == NULL)
        return false;  // �������ʧ�ܣ�����false

    if (!g_lgs->Startup()) {
        ERR("StartupServer: Startup login server failed.\n");  // �������������ʧ�ܣ����������־
        return false;                                          // ����false
    }

    LOG("Login server startup ok.\n");  // �����������ɹ��������־
    return true;                        // ����true����ʾ�ɹ�
}

//--------------------------------------------------------------------
// �رշ��������رյ�¼���������ͷ���Դ
//--------------------------------------------------------------------
static void ShutdownServer() {
    g_lgs->Exit();     // �����˳�����
    delete g_lgs;      // ɾ����¼�����������ͷ��ڴ�
    delete g_sendCmd;  // ɾ������������ͷ��ڴ�
}

//--------------------------------------------------------------------
// ���з�������ִ�з������������߼�
//--------------------------------------------------------------------
static bool RunServer() {
    return g_lgs->Run();  // ���з���������������״̬
}

//--------------------------------------------------------------------
// �źŴ����������յ��˳��ź�ʱִ��
//--------------------------------------------------------------------
static void OnExitProcess(int sig) {
    g_lgs->m_bNeedSendShutdown = 2;  // ���÷������Ĺرձ�־
}

#ifdef LINUX
#include <signal.h>
#include <core/rt2_core.h>

//--------------------------------------------------------------------
// �źŴ�����������SIGPIPE�źţ�ͨ�����ڴ������ѹر�socket�������ݵ����
//--------------------------------------------------------------------
void sig_pipe(int signal) {}
#endif

//--------------------------------------------------------------------
// ����������������������
//--------------------------------------------------------------------
int main() {
#ifdef ENABLE_OPERATOR_NEW
    rt2_ActiveSafestr();  // �����̰߳�ȫ���
#endif                    // ENABLE_OPERATOR_NEW

#ifdef LINUX
    signal(SIGPIPE, sig_pipe);  // ע��SIGPIPE�źŴ�����
#endif
    signal(SIGTERM, OnExitProcess);  // ע��SIGTERM�źŴ�����
    signal(SIGINT, OnExitProcess);   // ע��SIGINT�źŴ�����

#ifdef LINUX
    const char* szPIDFileName = "login_server.pid";  // ����PID�ļ���
    FILE*       fpPID;                               // �����ļ�ָ��
#ifndef _DEBUG
    if (fpPID = fopen(szPIDFileName, "rt"))  // ���PID�ļ��Ƿ��Ѵ���
    {
        printf("ERROR: PID file [%s] already existed.\n", szPIDFileName);  // ���������Ϣ
        fclose(fpPID);                                                     // �ر��ļ�
        return 105;                                                        // ���ش�����105
    }
#endif
    if (fpPID = fopen(szPIDFileName, "wt"))  // ��PID�ļ�д�����ID
    {
        fprintf(fpPID, "%d", getpid());  // д�����ID
        fclose(fpPID);                   // �ر��ļ�
    }
#endif

    if (!rtCoreInit("login_server.ini")) {
        LOG("Rt core init failed!");  // ���ĳ�ʼ��ʧ�ܣ������־
        return false;                 // ����ʧ��
    }

#ifdef LICENSE_CHECK
    if (!LicenseCheck(0))  // ִ�����֤���
    {
        LOG("License Check Failed\n");  // ���ʧ�ܣ������־
        return 101;                     // ���ش�����101
    } else {
        LOG("License Check Success\n");  // ���ɹ��������־
    }
#endif  // LICENSE_CHECK

    if (!StartupServer())  // ����������
    {
        return 101;  // ����ʧ�ܣ�16�������
    }
    RunServer();  // ���з��������߼�
    int iReturnResult = 101;          // Ĭ�Ϸ���101����ʾ����
    if (g_lgs->m_tShutdownTime != 0)  // ����Ƿ������˹ر�ʱ��
    {
        iReturnResult = 104;  // ��������˹ر�ʱ�䣬����104����ʾ�ػ�
    }
    ShutdownServer();  // �رշ�����
#ifdef LINUX
    RtCoreFile().Delete(szPIDFileName);  // ɾ��PID�ļ�
#endif
    rtCoreExit();  // �˳�����ģ��
    return iReturnResult;  // �������ս��
}
