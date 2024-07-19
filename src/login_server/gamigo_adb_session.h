#ifndef _GAMIGO_ACCOUNT_DB_SESSION_H_
#define _GAMIGO_ACCOUNT_DB_SESSION_H_


class CGamigoADBSession : protected CG_TCPSession
{
public:
    CGamigoADBSession();
    virtual ~CGamigoADBSession();

protected:
    virtual void    OnConnect(bool ret);
    virtual void    OnDisconnect();
    virtual void    OnReceive(CG_CmdPacket* packet);

public:
    bool ConnectServer(const char* szRemoteIP, int iRemotePort, const char* szBindIP=0, int iBindPort=0);
    void ProcessNet();

    bool IsConnected()  { return GetState()==NET_STATE_CONNECTED; }
    bool IsClosed()     { return GetState()==NET_STATE_ZERO || GetState()==NET_STATE_DEAD; }

    void                SendCmdLogin(const char* szUsername, const char* szPassword, long dwOrderID);

private:
    CG_CmdPacket&   BeginSend() { m_sendCmd.BeginWrite(); m_sendCmd.WriteShort(1001); return m_sendCmd; }
    bool            EndSend() { return SendPacket(&m_sendCmd); }
    void            SendCmd(const char* szCmd);

    CG_CmdPacket    m_sendCmd;

    std::string     m_szRemoteIP;
    int             m_iRemotePort;
    std::string     m_szBindIP;
    int             m_iBindPort;

    time_t          m_tDisconnect;

    char*           m_pCmdBuf;
    char**          m_ppCmdParameter;
    int             m_iCmdBufSize;
    int             m_iCmdParameterSize;
    int             m_iCmdParameterCnt;

};


#endif // _GAMIGO_ACCOUNT_DB_SESSION_H_
