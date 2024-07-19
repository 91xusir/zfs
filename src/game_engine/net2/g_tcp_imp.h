//
// g_tcpsession.h: interface for the CG_NetClientTcp class.
//
#ifndef __CG_TCP_IMP_H__
#define __CG_TCP_IMP_H__

const int MAX_BUFFER_SIZE		= MAX_NET_PACKET_SIZE;
const int MAX_NET_SEED_SIZE		= 16;

enum ENetState
{
	NET_STATE_ZERO			= 0,
	NET_STATE_CONNECTING	= 1,
	NET_STATE_WAITING_SEED	= 2,
	NET_STATE_CONNECTED		= 3,
	NET_STATE_DEAD			= 4,
	NET_STATE_RECONNECT     = 5,
};

const int NET_STAT_INTERVAL         =  1*1000;
const int NET_CONNECT_TIMEOUT		= 30*1000;
const int NET_KEEP_ALIVE_INTERVAL	=  3*1000;
#ifdef _DEBUG
	const int NET_TIMEOUT			= 900*1000;
#else
	const int NET_TIMEOUT			= 90*1000;
#endif

class CG_NetClientTcp : public CG_NetClientImp  
{
public:
	CG_NetClientTcp();
	virtual ~CG_NetClientTcp();
	
	virtual bool Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify);
	virtual bool Connect(const char *remoteIp, int remotePort);
	virtual void ReConnect(DWORD delay);
	virtual void Process();
	virtual void Close(DWORD delay);
	virtual bool SendPacket(CG_CmdPacket *packet);
	virtual void EnableDebugInfo(bool debug, const char *title);
	virtual void GetNetInfo(SNetInfo &info);
	virtual bool GetRemoteAddr(SNetAddr &addr);

	virtual void Destroy();
	virtual bool CanClose();
	virtual bool IsClosing();
	virtual void SetBufferSize(int session, int net);
	CG_NetDeviceImp *GetDevice() { return m_dev; }
	void SetDevice(CG_NetDeviceImp *dev) { m_dev = dev; }
	CG_NetDeviceImp *m_dev;

	bool m_bDebugInfo;
	std::string m_remoteIp, m_bindIp;
	int m_remotePort, m_bindPort;
	DWORD m_closeDelay;

	// used in client mode
	DWORD m_reConnTime;
	CG_NetClientNotify *m_notify;

	// private 
	void Attach(SOCKET socket);
	bool GetPacket(CG_CmdPacket *packet);
	bool IsDead();
	bool SendPacket(CG_CmdPacket *packet,bool flush,bool sys);
	
	int  GetRtt();
	int  GetState();
	int  GetTotleBytesOut();
	int  GetTotleBytesIn();
	int  GetAvgFluxOut();
	int  GetAvgFluxIn();
	
	float GetRecvCompressRate()
	{
		if(m_rowRecvBytes == 0) return 0;
		return (m_rowRecvBytes - GetTotleBytesIn()) / ((float)m_rowRecvBytes);
	}
	float GetSendCompressRate()
	{
		if(m_rowSendBytes == 0) return 0;
		return (m_rowSendBytes - GetTotleBytesOut()) / ((float)m_rowSendBytes);
	}
	
	void DefProc();
	void Reset();
	void SendConnectInfo(long seed);
	void KeepAlive(long now);
	void SetDebugStr(const char *str);

private:
	int  HandleSysPacket(CG_CmdPacket *packet);
	bool RecvPacket(CG_NetPacket *packet);
	bool ReadData();
	bool PushData(char *buf,int len);
	bool ResizeBuf(int min);
	void SetSeed(long seed);
	void FlushData();

public:
	CG_NetSocket  m_socket;
	ENetState	  m_state;
	unsigned long m_startTime;
	unsigned long m_lastRecvTime;
	unsigned long m_lastSendTime;
	unsigned long m_connTime;
	unsigned long m_waitSeedTime;
	
	char *m_readBuf;
	int	  m_readBufSize;
	int   m_readPos;
	int   m_readLen;
		
	char *m_writeBuf;
	int   m_writeBufSize;
	int	  m_writeLen;
	int   m_writeBeg;
	int	  m_writeEnd;
	
	//char m_seed[MAX_NET_SEED_SIZE];
	CM_MEMDEF(m_seed, MAX_NET_SEED_SIZE)
	int  m_seedLen;
	int  m_maxBufSize;
	
	bool m_bEncrypt;
	unsigned short m_sendSeq;
	unsigned short m_recvSeq;

	// net performance 
	unsigned long m_lastStatTime;
	unsigned long m_sendPktNum;
	unsigned long m_recvPktNum;
	unsigned long m_rowSendBytes;
	unsigned long m_rowRecvBytes;
	unsigned long m_sendBytes;
	unsigned long m_recvBytes;
	unsigned long m_rtt;
	unsigned long m_lastSecSendBytes;
	unsigned long m_lastSecRecvBytes;
	unsigned long m_statSendBytes;
	unsigned long m_statRecvBytes;

	//char  m_debugStr[MAX_DEBUG_STR_LEN];
	CM_MEMDEF(m_debugStr, MAX_DEBUG_STR_LEN)
	int   m_comDelay;
};

class CG_TCPListener  
{
public:
	CG_TCPListener();
	virtual ~CG_TCPListener();

	bool StartListen(const char *ip,int port,bool encrypt=true);
	void Close();
	
	CG_NetClientTcp *AcceptNewClient();
	CG_NetDeviceImp *m_dev;

private:
	long ComputeSeed();
	bool m_bEncrypt;
	CG_NetSocket m_socket;
};

class CG_NetServerTcp : public CG_NetServerImp
{
public:
	CG_NetServerTcp();
	virtual ~CG_NetServerTcp();

	virtual bool Init(const char *ip, int port, CG_NetServerNotify *notify);
	virtual void Process();
	virtual void Close(DWORD delay);

	virtual void Destroy();
	virtual bool CanClose();
	virtual bool IsClosing();

	CG_NetDeviceImp *GetDevice() { return m_dev; }
	void SetDevice(CG_NetDeviceImp *dev) { m_dev = dev; }
	CG_NetDeviceImp *m_dev;

	DWORD m_closeDelay;
	CG_NetServerNotify *m_notify;
	CG_TCPListener *m_listener;
	std::list<CG_NetClientTcp*> m_clientList;
	CG_CmdPacket m_packet;
};

/*
class CG_TCPBroadcastArchive : public RtArchive
{
protected:
	bool OnCmd(short cmdID, CG_CmdPacket* packet)
    {
        return false;
    }

public:
	CG_CmdPacket& BeginCmd(short cmd)   { m_sendCmd.BeginWrite(); m_sendCmd.WriteShort(cmd); return m_sendCmd; }
	bool SendCmd()
	{
        SSNode* pNext = m_pNodes;
        while (pNext)
        {
            pNext->pSession->SendPacket(&m_sendCmd);
            pNext = pNext->pNext;
        }
        return true;
	}
    void SendString(const char* szString)
    {
        if (m_iSessionCnt)
        {
            BeginCmd(1001);
            m_sendCmd.WriteBinary((char*)szString, (int)strlen(szString));
            SendCmd();
        }
    }
    virtual long Serialize( void* V, int iLength )
    {
        if (m_iSessionCnt)
        {
            BeginCmd(1001);
            if (!m_sendCmd.WriteBinary((char*)V, iLength))
                return 0;
            SendCmd();
        }
        return iLength;
    }
	bool Start(const char* ip, int port=6999, int capacity=5)
	{
        int i;
        for (i=0; i<10; i++)
        {
		    if (m_listener.StartListen(ip, port, false))
            {
                printf("Startup log system at %s:%d\n", ip, port);
			    break;
            }
            port ++;
        }
        if (i==10)
        {
            return false;
        }
		m_capacity = capacity;
		return true;
	}
	void Process()
	{
        CG_TCPSession* se;
		while (1)
		{
			se = m_listener.AcceptNewSession();
			if (se == NULL)
				break;
            if (m_iSessionCnt>=m_capacity)
            {
				delete se;
				continue;
            }

			se->EnableRemoteCompress(false);

            SSNode* pNew = RT_NEW SSNode;
            pNew->pSession = se;
            pNew->valid = true;
            pNew->pNext = m_pNodes;
            m_pNodes = pNew;
            m_iSessionCnt ++;

#ifdef _DEBUG
            printf("Log client connected.\n");
#endif
            SendString("Welcome net log system.\n");
		}

		short cmdID;
        int bError;
        SSNode* pNode = m_pNodes, *pDel, *pPrev = NULL;

        while (pNode)
        {
            bError = FALSE;
            pNode->pSession->DefProc();

            if (pNode->pSession->IsDead())
			{
                bError = true;
			}else
            {
			    while(pNode->pSession->GetPacket(&m_recvCmd))
			    {
				    if (m_recvCmd.ReadShort(&cmdID))
				    {
					    if (!OnCmd(cmdID, &m_recvCmd))
                        {
                            bError = TRUE;
                            break;
                        }
				    }else
				    {
                        bError = TRUE;
					    break;
				    }
			    }
            }

            if (bError) // 发生错误，就直接断开连接
            {
                pDel = pNode;
                pNode = pNode->pNext;
                if (pPrev)
                {
                    pPrev->pNext = pNode;
                }else
                {
                    m_pNodes = pNode;
                }

#ifdef _DEBUG
                printf("Log client disconnected.\n");
#endif
                delete pDel->pSession;
                delete pDel;
                m_iSessionCnt --;
            }else
            {
                pPrev = pNode;
                pNode = pNode->pNext;
            }
        }
	}

public:
	CG_TCPBroadcastArchive()
	{
        m_iSessionCnt = 0;
        m_capacity = 5;
        m_pNodes = NULL;
	}
    virtual ~CG_TCPBroadcastArchive()
    {
    }

public:
    struct SSNode
    {
	    CG_TCPSession*  pSession;
        SSNode*         pNext;
        bool            valid;
    };
	SSNode*             m_pNodes;
	CG_TCPListener		m_listener;
	CG_CmdPacket		m_recvCmd;
	CG_CmdPacket		m_sendCmd;
    int                 m_iSessionCnt;
    int                 m_capacity;
};
*/
#endif
