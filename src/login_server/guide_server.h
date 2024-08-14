#pragma once
#include <list>
#include <ctime>

#define GS_CLIENT_TIMEOUT 1000

////////////////////////////////////////////////////////////////////////////////////////////
// 基础会话结构体
struct GSSession {
    bool           valid;        // 会话是否有效
    time_t         connectTime;  // 连接时间
    CG_TCPSession* session;      // 指向 TCP 会话的指针
    int            iIdx;         // 会话索引
};

////////////////////////////////////////////////////////////////////////////////////////////
// 更新会话结构体，继承自 GSSession
struct GSUpdateSession : public GSSession {
    std::string       host;         // 主机名
    short             port;         // 端口号
    long              lUserCnt;     // 用户数量
    char              cEvaluation;  // 评估标志
    SStatUpdateServer m_stat;       // 更新服务器状态

    bool                   bChecked;    // 是否已检查
    std::string            strExeMD5;   // 执行文件的 MD5 值
    std::string            strDataMD5;  // 数据文件的 MD5 值
    std::string            strOS;       // 操作系统信息
    std::list<std::string> listFiles;   // 文件列表

    // 构造函数，初始化 bChecked 为 false
    GSUpdateSession() { bChecked = false; }
};

////////////////////////////////////////////////////////////////////////////////////////////
// 客户端会话结构体，继承自 GSSession
struct GSClientSession : public GSSession {
    // 构造函数
    GSClientSession() {
        CM_MEMPROTECTOR(szUsername, 40)  // 保护用户名内存
        iState          = 0;             // 初始化状态为 0
        lPrivileges     = 0;             // 初始化权限为 0
        lastRequestTime = 0;             // 初始化最后请求时间为 0
    }

    GSClientSession(const GSClientSession& object) {
        memcpy(this, &object, sizeof(GSClientSession));  // 使用内存拷贝拷贝对象内容
        CM_MEMPROTECTOR(szUsername, 40)                  // 保护用户名内存
    }

    ~GSClientSession() {
        CM_MEMUNPROTECTOR(szUsername)  // 取消对用户名内存的保护
    }

    // 记录日志的函数
    void Log2(const char* szMsg) {
        // 如果权限标志中的最高位为 0，记录日志
        if ((lPrivileges & 0x80000000) == 0)
            LOG2(const_cast<char*>(szMsg), szUsername, rtTimestamp());
    }

    // 字符数组用于存储用户名
    CM_MEMDEF(szUsername, 40)  // 定义大小为 40 的用户名数组

    time_t lastRequestTime;  // 上次请求时间
    int    iState;           // 状态
    long   lPrivileges;      // 权限
};

////////////////////////////////////////////////////////////////////////////////////////////
// 模板类 GSSessionManager，管理会话
template <class T>
class GSSessionManager {
   public:
    GSSessionManager()
        : m_activateSession(nullptr), m_capacity(100), m_timeout(0), m_now(0), m_iCurIdx(1) {
    }  // 构造函数

    virtual ~GSSessionManager() {}  // 析构函数

   public:
    T*             m_activateSession;  // 当前激活的会话
    CG_TCPListener m_listener;         // TCP 监听器
    CG_CmdPacket   m_recvPacket;       // 接收数据包
    CG_CmdPacket   m_sendPacket;       // 发送数据包
    int            m_capacity;         // 最大会话容量
    int            m_timeout;          // 会话超时时间
    time_t         m_now;              // 当前时间
    int            m_iCurIdx;          // 当前索引

   public:
    typedef std::list<T> SessionList;  // 会话列表类型定义
    SessionList          m_sessions;   // 会话列表

   protected:
    // 处理命令的纯虚函数，子类需要实现
    virtual bool OnCmd(short cmdID, CG_CmdPacket* packet) = 0;

    // 处理接受新会话的虚函数，默认返回 true
    virtual bool OnAccept(CG_TCPSession* session) { return true; }

   public:
    // 发送命令到指定索引的会话
    bool SendCmd(int iIdx = 0) {
        if (iIdx == 0) {
            return m_activateSession->session->SendPacket(&m_sendPacket);
        } else {
            // 遍历会话列表，找到指定索引的会话并发送数据包
            for (auto se = m_sessions.begin(); se != m_sessions.end(); ++se) {
                if (se->iIdx == iIdx) {
                    return se->session->SendPacket(&m_sendPacket);
                }
            }
        }
        return false;  // 没有找到会话或发送失败
    }

    // 开始编写命令数据包
    CG_CmdPacket& BeginCmd(short cmd) {
        m_sendPacket.BeginWrite();
        m_sendPacket.WriteShort(cmd);
        return m_sendPacket;
    }

    // 获取当前激活的会话
    T* GetSession() { return &*m_activateSession; }

    // 创建监听器并初始化会话管理器
    bool Create(const char* ip, int port, int capacity, int timeout) {
        if (!m_listener.StartListen(ip, port, false))
            return false;

        m_capacity = capacity;  // 设置最大容量
        m_timeout  = timeout;   // 设置超时时间

        return true;
    }

    // 处理会话，包括接受新会话、处理数据包、清理无效会话
    void Process() {
        m_now            = time(nullptr);                        // 获取当前时间
        int            c = m_capacity - (int)m_sessions.size();  // 计算剩余容量
        int            bError;                                   // 错误标志
        CG_TCPSession* psession = nullptr;                       // 临时会话指针
        if (c > 0) {
            do {
                // 接受新会话
                psession = m_listener.AcceptNewSession();

                if (psession == nullptr)
                    break;  // 没有更多会话

                if (!OnAccept(psession)) {
                    delete psession;  // 处理不接受的会话
                    continue;
                }
                psession->EnableRemoteCompress(false);  // 禁用远程压缩
                // 添加新会话到会话列表
                m_sessions.push_back(T());
                m_sessions.back().iIdx        = m_iCurIdx++;
                m_sessions.back().session     = psession;
                m_sessions.back().connectTime = time(NULL);
                m_sessions.back().valid       = true;
                char ip[1024];
                psession->GetSocket().GetRemoteAddr(ip, NULL);
                RtCoreLog().Info("Accept new connection from %s \n", ip);
            } while (psession);
        }

        // 遍历会话列表
        auto se = m_sessions.begin();
        while (se != m_sessions.end()) {

            se->session->DefProc();  // 处理会话

            // 检查会话是否死亡或超时
            if (se->session->IsDead() || (m_timeout != 0 && m_now - se->connectTime >= m_timeout)) {
                delete se->session;         // 删除无效会话
                se = m_sessions.erase(se);  // 从列表中移除会话
                continue;
            }

            bError = 0;
            while (se->session->GetPacket(&m_recvPacket)) {
                short cmdID;
                if (m_recvPacket.ReadShort(&cmdID)) {
                    m_activateSession = &*se;  // 设置激活会话

                    if (!OnCmd(cmdID, &m_recvPacket)) {
                        bError = 1;
                        break;  // 处理命令失败
                    }
                } else {
                    bError = 1;
                    break;  // 读取命令失败
                }
            }

            if (bError)  // 发生错误，就直接断开连接
            {
                delete se->session;
                se = m_sessions.erase(se);  // 从列表中移除会话
            } else {
                ++se;  // 移动到下一个会话
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////
// GSUpdateSessionManager 类，管理更新服务器会话，继承自 GSSessionManager<GSUpdateSession>
class GSUpdateSessionManager : public GSSessionManager<GSUpdateSession> {
    // 处理注册更新服务器的命令
    //int OnRegisterUpdateServer(CG_CmdPacket* packet);

    // 处理接收到的更新信息
    int OnReceivedUpdateInfo(CG_CmdPacket* packet);

    // 根据用户数量更新评价
    void UpdateEvaluationByUserCnt();

    // 当有新的会话连接时的处理逻辑
    virtual bool OnAccept(CG_TCPSession* session);

    // 处理接收到的命令
    virtual bool OnCmd(short cmdID, CG_CmdPacket* packet);

   public:
    // 析构函数，销毁 GSUpdateSessionManager 对象
    virtual ~GSUpdateSessionManager() {}
};

////////////////////////////////////////////////////////////////////////////////////////////

// GSClientSessionManager 类，管理客户端会话，继承自 GSSessionManager<GSClientSession>
class GSClientSessionManager : public GSSessionManager<GSClientSession> {
   public:
    GSClientSessionManager();

    virtual ~GSClientSessionManager() {}

    // 处理请求更新服务器列表的命令
    int OnRequestUpdateServerList(CG_CmdPacket*);

    // 处理请求网关服务器列表的命令
    int OnRequestGWServerList(CG_CmdPacket* pPacket);

    // 监控基础功能
    bool OnMoniterBase(CG_CmdPacket*);

    // 监控查询信息
    bool OnMoniterQueryInfo(CG_CmdPacket*);

    // 监控重启命令
    bool OnMoniterReboot(CG_CmdPacket*);

    // 监控关机命令
    bool OnMoniterShutdown(CG_CmdPacket*);

    // 监控公告
    bool OnMoniterBulletin(CG_CmdPacket*);

    // 监控设置状态命令
    bool OnMoniterSetStatus(CG_CmdPacket*);

    // 处理接收到的命令
    virtual bool OnCmd(short cmdID, CG_CmdPacket* packet);

    // 当有新的会话连接时的处理逻辑
    virtual bool OnAccept(CG_TCPSession* session);

   public:
    // 允许所有 IP 连接的标志
    bool m_bAllowAllIP;

    // 允许的 IP 配置
    SAllowIP m_allowIP;

    // 允许的用户配置
    SAllowUser m_allowUser;

    // 允许所有 IP 连接
    void AllowAllIP() { m_bAllowAllIP = true; }
};
