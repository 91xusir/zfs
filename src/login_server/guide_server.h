#ifndef _INC_GUIDE_SERVER_H_
#define _INC_GUIDE_SERVER_H_

#define GS_CLIENT_TIMEOUT 1000

////////////////////////////////////////////////////////////////////////////////////////////

#include <list>

struct GSSession
{
	bool			valid;
	time_t			connectTime;
	CG_TCPSession*	session;
    int             iIdx;
};

////////////////////////////////////////////////////////////////////////////////////////////

struct GSUpdateSession : public GSSession
{
    std::string		        host;
    short			        port;
    long			        lUserCnt;
    char			        cEvaluation;
    SStatUpdateServer       m_stat;

    bool                    bChecked;
    std::string             strExeMD5;
    std::string             strDataMD5;
    std::string             strOS;
    std::list<std::string>  listFiles;

    GSUpdateSession()
    {
        bChecked = false;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////

struct GSClientSession : public GSSession
{
    GSClientSession()
    {
		CM_MEMPROTECTOR(szUsername, 40)
        iState = 0;
        lPrivileges = 0;
        lastRequestTime = 0;
    }
	GSClientSession(const GSClientSession &object)
	{
		memcpy(this, &object, sizeof(GSClientSession));
		CM_MEMPROTECTOR(szUsername, 40)
	}
	~GSClientSession()
	{
		CM_MEMUNPROTECTOR(szUsername)
	}
    void Log2(const char* szMsg)
    {
        if ((lPrivileges&0x80000000)==0) LOG2(const_cast<char*>(szMsg), szUsername, rtTimestamp());
    }

    //char    szUsername[40];
	CM_MEMDEF(szUsername, 40)
    time_t  lastRequestTime;
    int     iState;
    long    lPrivileges;
};

////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class GSSessionManager
{
public:
    typedef std::list<T>	SessionList;

	SessionList			m_sessions;
protected:
	virtual bool OnCmd(short cmdID, CG_CmdPacket* packet) = 0;

	virtual bool OnAccept(CG_TCPSession* session) { return true; }	

public:
	bool SendCmd(int iIdx=0)
	{
        if (iIdx==0)
        {
            return m_activateSession->session->SendPacket(&m_sendPacket);
        }else
        {
            stlforeach_template(std::list<T>, se, m_sessions)
            {
                if (se->iIdx==iIdx)
                {
                    return se->session->SendPacket(&m_sendPacket);
                }
            }
        }
        return false;
	}

	CG_CmdPacket& BeginCmd(short cmd)
	{
		m_sendPacket.BeginWrite();
		m_sendPacket.WriteShort(cmd);

		return m_sendPacket;
	}


	T* GetSession()
	{
		return &*m_activateSession;
	}

	bool Create(const char* ip, int port, int capacity, int timeout)
	{
		if (!m_listener.StartListen(ip, port, false))
			return false;

		m_capacity = capacity;
		m_timeout = timeout;

		return true;
	}

	void Process()
	{
		m_now = time(NULL);

		int c = m_capacity - (int)m_sessions.size();

        int bError;

		CG_TCPSession* psession = NULL;

		if (c > 0)
		{
			do 
			{

				psession = m_listener.AcceptNewSession();

				if (psession == NULL)
					break;

				if (!OnAccept(psession))
				{
					delete psession;

					continue;
				}
				// client upload packet don't use compress
				psession->EnableRemoteCompress(false);

				m_sessions.push_back(T());
				m_sessions.back().iIdx = m_iCurIdx++;
				m_sessions.back().session = psession;
				m_sessions.back().connectTime = time(NULL);
				m_sessions.back().valid = true;

				char ip[1024];

				psession->GetSocket().GetRemoteAddr(ip, NULL);

				//MSG1("Accept new connection from %s\n", ip);


			} while(psession);
		}

	//	stlforeach(std::list<T>, se, m_sessions)
		typename std::list<T>::iterator se = m_sessions.begin();
		for (/*se = m_sessions.begin()*/; se!= m_sessions.end(); )
		{
			se->session->DefProc();

			if (se->session->IsDead() || 
				(m_timeout != 0 && m_now - se->connectTime >= m_timeout))
			{
				delete se->session;

				se = m_sessions.erase(se);

				continue;
			}

            bError = FALSE;
			while(se->session->GetPacket(&m_recvPacket))
			{
				short cmdID;

				if (m_recvPacket.ReadShort(&cmdID))
				{
					m_activateSession = &*se;

					if (!OnCmd(cmdID, &m_recvPacket))
                    {
                        bError = TRUE;
                        break;
                    }
				}
				else
				{
                    bError = TRUE;
					break;
				}
			}

            if (bError) // 发生错误，就直接断开连接
            {
                delete se->session;

                se = m_sessions.erase(se);

                continue;
            }

			++se;
		}
	}

public:
	GSSessionManager()
		: m_activateSession(NULL)
		, m_capacity(100)
		, m_timeout(0)
        , m_now(0)
        , m_iCurIdx(1)
	{
	}
    virtual ~GSSessionManager()
    {
    }

public:
	T*					m_activateSession;
	CG_TCPListener		m_listener;
	CG_CmdPacket		m_recvPacket;
	CG_CmdPacket		m_sendPacket;
	int					m_capacity;
	int					m_timeout;
    time_t              m_now;
    int                 m_iCurIdx;
};

////////////////////////////////////////////////////////////////////////////////////////////

class GSUpdateSessionManager : public GSSessionManager<GSUpdateSession>
{
	int OnRegisterUpdateServer(CG_CmdPacket* packet);

	int OnReceivedUpdateInfo(CG_CmdPacket* packet);

    void UpdateEvaluationByUserCnt();

	virtual bool OnAccept(CG_TCPSession* session);

	virtual bool OnCmd(short cmdID, CG_CmdPacket* packet);

public:
    virtual ~GSUpdateSessionManager() {}
};

////////////////////////////////////////////////////////////////////////////////////////////

class GSClientSessionManager : public GSSessionManager<GSClientSession>
{
	int OnRequestUpdateServerList(CG_CmdPacket*);
	int OnRequestGWServerList(CG_CmdPacket* pPacket);

    bool OnMoniterBase(CG_CmdPacket*);
    bool OnMoniterQueryInfo(CG_CmdPacket*);
    bool OnMoniterReboot(CG_CmdPacket*);
    bool OnMoniterShutdown(CG_CmdPacket*);
    bool OnMoniterBulletin(CG_CmdPacket*);
    bool OnMoniterSetStatus(CG_CmdPacket*);

	virtual bool OnCmd(short cmdID, CG_CmdPacket* packet);
    virtual bool OnAccept(CG_TCPSession* session);

public:
    bool        m_bAllowAllIP;
    SAllowIP    m_allowIP;
    SAllowUser  m_allowUser;

public:
    GSClientSessionManager();
    virtual ~GSClientSessionManager() {}
    void AllowAllIP()                 { m_bAllowAllIP = true; }
};


#endif // _INC_GUIDE_SERVER_H_
