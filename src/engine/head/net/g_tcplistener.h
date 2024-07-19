//
// g_tcplistener.h: interface for the CG_TCPListener class.
//
#ifndef __CG_TCP_LISTENER_H__
#define __CG_TCP_LISTENER_H__

class CG_TCPListener  
{
public:
	CG_TCPListener();
	virtual ~CG_TCPListener();

	bool StartListen(const char *ip,int port,bool encrypt=true);
	void Close();
	
	CG_TCPSession *AcceptNewSession();
	bool AcceptNewSession(CG_TCPSession *session);

private:
	long ComputeSeed();
	bool m_bEncrypt;
	CG_NetSocket m_socket;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

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
				DEL_ONE(se);
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
                DEL_ONE(pDel->pSession);
                DEL_ONE(pDel);
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

#endif
