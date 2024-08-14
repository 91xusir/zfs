#pragma once
#include <list>
#include <ctime>

#define GS_CLIENT_TIMEOUT 1000

////////////////////////////////////////////////////////////////////////////////////////////
// �����Ự�ṹ��
struct GSSession {
    bool           valid;        // �Ự�Ƿ���Ч
    time_t         connectTime;  // ����ʱ��
    CG_TCPSession* session;      // ָ�� TCP �Ự��ָ��
    int            iIdx;         // �Ự����
};

////////////////////////////////////////////////////////////////////////////////////////////
// ���»Ự�ṹ�壬�̳��� GSSession
struct GSUpdateSession : public GSSession {
    std::string       host;         // ������
    short             port;         // �˿ں�
    long              lUserCnt;     // �û�����
    char              cEvaluation;  // ������־
    SStatUpdateServer m_stat;       // ���·�����״̬

    bool                   bChecked;    // �Ƿ��Ѽ��
    std::string            strExeMD5;   // ִ���ļ��� MD5 ֵ
    std::string            strDataMD5;  // �����ļ��� MD5 ֵ
    std::string            strOS;       // ����ϵͳ��Ϣ
    std::list<std::string> listFiles;   // �ļ��б�

    // ���캯������ʼ�� bChecked Ϊ false
    GSUpdateSession() { bChecked = false; }
};

////////////////////////////////////////////////////////////////////////////////////////////
// �ͻ��˻Ự�ṹ�壬�̳��� GSSession
struct GSClientSession : public GSSession {
    // ���캯��
    GSClientSession() {
        CM_MEMPROTECTOR(szUsername, 40)  // �����û����ڴ�
        iState          = 0;             // ��ʼ��״̬Ϊ 0
        lPrivileges     = 0;             // ��ʼ��Ȩ��Ϊ 0
        lastRequestTime = 0;             // ��ʼ���������ʱ��Ϊ 0
    }

    GSClientSession(const GSClientSession& object) {
        memcpy(this, &object, sizeof(GSClientSession));  // ʹ���ڴ濽��������������
        CM_MEMPROTECTOR(szUsername, 40)                  // �����û����ڴ�
    }

    ~GSClientSession() {
        CM_MEMUNPROTECTOR(szUsername)  // ȡ�����û����ڴ�ı���
    }

    // ��¼��־�ĺ���
    void Log2(const char* szMsg) {
        // ���Ȩ�ޱ�־�е����λΪ 0����¼��־
        if ((lPrivileges & 0x80000000) == 0)
            LOG2(const_cast<char*>(szMsg), szUsername, rtTimestamp());
    }

    // �ַ��������ڴ洢�û���
    CM_MEMDEF(szUsername, 40)  // �����СΪ 40 ���û�������

    time_t lastRequestTime;  // �ϴ�����ʱ��
    int    iState;           // ״̬
    long   lPrivileges;      // Ȩ��
};

////////////////////////////////////////////////////////////////////////////////////////////
// ģ���� GSSessionManager������Ự
template <class T>
class GSSessionManager {
   public:
    GSSessionManager()
        : m_activateSession(nullptr), m_capacity(100), m_timeout(0), m_now(0), m_iCurIdx(1) {
    }  // ���캯��

    virtual ~GSSessionManager() {}  // ��������

   public:
    T*             m_activateSession;  // ��ǰ����ĻỰ
    CG_TCPListener m_listener;         // TCP ������
    CG_CmdPacket   m_recvPacket;       // �������ݰ�
    CG_CmdPacket   m_sendPacket;       // �������ݰ�
    int            m_capacity;         // ���Ự����
    int            m_timeout;          // �Ự��ʱʱ��
    time_t         m_now;              // ��ǰʱ��
    int            m_iCurIdx;          // ��ǰ����

   public:
    typedef std::list<T> SessionList;  // �Ự�б����Ͷ���
    SessionList          m_sessions;   // �Ự�б�

   protected:
    // ��������Ĵ��麯����������Ҫʵ��
    virtual bool OnCmd(short cmdID, CG_CmdPacket* packet) = 0;

    // ��������»Ự���麯����Ĭ�Ϸ��� true
    virtual bool OnAccept(CG_TCPSession* session) { return true; }

   public:
    // �������ָ�������ĻỰ
    bool SendCmd(int iIdx = 0) {
        if (iIdx == 0) {
            return m_activateSession->session->SendPacket(&m_sendPacket);
        } else {
            // �����Ự�б��ҵ�ָ�������ĻỰ���������ݰ�
            for (auto se = m_sessions.begin(); se != m_sessions.end(); ++se) {
                if (se->iIdx == iIdx) {
                    return se->session->SendPacket(&m_sendPacket);
                }
            }
        }
        return false;  // û���ҵ��Ự����ʧ��
    }

    // ��ʼ��д�������ݰ�
    CG_CmdPacket& BeginCmd(short cmd) {
        m_sendPacket.BeginWrite();
        m_sendPacket.WriteShort(cmd);
        return m_sendPacket;
    }

    // ��ȡ��ǰ����ĻỰ
    T* GetSession() { return &*m_activateSession; }

    // ��������������ʼ���Ự������
    bool Create(const char* ip, int port, int capacity, int timeout) {
        if (!m_listener.StartListen(ip, port, false))
            return false;

        m_capacity = capacity;  // �����������
        m_timeout  = timeout;   // ���ó�ʱʱ��

        return true;
    }

    // ����Ự�����������»Ự���������ݰ���������Ч�Ự
    void Process() {
        m_now            = time(nullptr);                        // ��ȡ��ǰʱ��
        int            c = m_capacity - (int)m_sessions.size();  // ����ʣ������
        int            bError;                                   // �����־
        CG_TCPSession* psession = nullptr;                       // ��ʱ�Ựָ��
        if (c > 0) {
            do {
                // �����»Ự
                psession = m_listener.AcceptNewSession();

                if (psession == nullptr)
                    break;  // û�и���Ự

                if (!OnAccept(psession)) {
                    delete psession;  // �������ܵĻỰ
                    continue;
                }
                psession->EnableRemoteCompress(false);  // ����Զ��ѹ��
                // ����»Ự���Ự�б�
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

        // �����Ự�б�
        auto se = m_sessions.begin();
        while (se != m_sessions.end()) {

            se->session->DefProc();  // ����Ự

            // ���Ự�Ƿ�������ʱ
            if (se->session->IsDead() || (m_timeout != 0 && m_now - se->connectTime >= m_timeout)) {
                delete se->session;         // ɾ����Ч�Ự
                se = m_sessions.erase(se);  // ���б����Ƴ��Ự
                continue;
            }

            bError = 0;
            while (se->session->GetPacket(&m_recvPacket)) {
                short cmdID;
                if (m_recvPacket.ReadShort(&cmdID)) {
                    m_activateSession = &*se;  // ���ü���Ự

                    if (!OnCmd(cmdID, &m_recvPacket)) {
                        bError = 1;
                        break;  // ��������ʧ��
                    }
                } else {
                    bError = 1;
                    break;  // ��ȡ����ʧ��
                }
            }

            if (bError)  // �������󣬾�ֱ�ӶϿ�����
            {
                delete se->session;
                se = m_sessions.erase(se);  // ���б����Ƴ��Ự
            } else {
                ++se;  // �ƶ�����һ���Ự
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////
// GSUpdateSessionManager �࣬������·������Ự���̳��� GSSessionManager<GSUpdateSession>
class GSUpdateSessionManager : public GSSessionManager<GSUpdateSession> {
    // ����ע����·�����������
    //int OnRegisterUpdateServer(CG_CmdPacket* packet);

    // ������յ��ĸ�����Ϣ
    int OnReceivedUpdateInfo(CG_CmdPacket* packet);

    // �����û�������������
    void UpdateEvaluationByUserCnt();

    // �����µĻỰ����ʱ�Ĵ����߼�
    virtual bool OnAccept(CG_TCPSession* session);

    // ������յ�������
    virtual bool OnCmd(short cmdID, CG_CmdPacket* packet);

   public:
    // �������������� GSUpdateSessionManager ����
    virtual ~GSUpdateSessionManager() {}
};

////////////////////////////////////////////////////////////////////////////////////////////

// GSClientSessionManager �࣬����ͻ��˻Ự���̳��� GSSessionManager<GSClientSession>
class GSClientSessionManager : public GSSessionManager<GSClientSession> {
   public:
    GSClientSessionManager();

    virtual ~GSClientSessionManager() {}

    // ����������·������б������
    int OnRequestUpdateServerList(CG_CmdPacket*);

    // �����������ط������б������
    int OnRequestGWServerList(CG_CmdPacket* pPacket);

    // ��ػ�������
    bool OnMoniterBase(CG_CmdPacket*);

    // ��ز�ѯ��Ϣ
    bool OnMoniterQueryInfo(CG_CmdPacket*);

    // �����������
    bool OnMoniterReboot(CG_CmdPacket*);

    // ��عػ�����
    bool OnMoniterShutdown(CG_CmdPacket*);

    // ��ع���
    bool OnMoniterBulletin(CG_CmdPacket*);

    // �������״̬����
    bool OnMoniterSetStatus(CG_CmdPacket*);

    // ������յ�������
    virtual bool OnCmd(short cmdID, CG_CmdPacket* packet);

    // �����µĻỰ����ʱ�Ĵ����߼�
    virtual bool OnAccept(CG_TCPSession* session);

   public:
    // �������� IP ���ӵı�־
    bool m_bAllowAllIP;

    // ����� IP ����
    SAllowIP m_allowIP;

    // ������û�����
    SAllowUser m_allowUser;

    // �������� IP ����
    void AllowAllIP() { m_bAllowAllIP = true; }
};
