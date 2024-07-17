#ifndef _JHWG_SESSION_H_
#define _JHWG_SESSION_H_

class CJhWgSession : protected CG_TCPSession
{
public:
    CJhWgSession();
    virtual ~CJhWgSession();

protected:
    virtual void    OnConnect(bool ret);
    virtual void    OnDisconnect();
    virtual void    OnReceive(CG_CmdPacket* packet);

public:
    bool SetConnectServer(const char* szRemoteIP, int iRemotePort,
        const char* szLoginName, const char* szGameworldName, short sRegionIdx,
        const char* szBindIP=0, int iBindPort=0);
    void ProcessNet();

    bool IsConnected()  { return GetState()==NET_STATE_CONNECTED; }
    bool IsClosed()     { return GetState()==NET_STATE_ZERO || GetState()==NET_STATE_DEAD; }

    void LoginToServer(const char* szLoginName, const char* szGameworldName, short sRegionIdx);
    void CheckedUser(const char* szAccountName, const char* szActorName, long lActorID, long lCheckCnt);
    void UpdateStatus(long lTotalUser, long lCheckedUser);

    // 处理用户
    void OnUserProgram(long lActorID, short sProgramID, long lSecond, const char* szMsg);

private:
    CG_CmdPacket&   BeginSend(char cCmdID) { m_sendCmd.BeginWrite(); m_sendCmd.WriteShort(cCmdID); return m_sendCmd; }
    bool            EndSend();

    CG_CmdPacket    m_sendCmd;

    std::string     m_szRemoteIP;
    int             m_iRemotePort;
    std::string     m_szBindIP;
    int             m_iBindPort;

    std::string     m_szLoginName;
    std::string     m_szGameworldName;
    short           m_sRegionIdx;

    bool            m_bLogined;
    time_t          m_tDisconnect;
    int             m_iCurSn;

};


#endif // _JHWG_SESSION_H_
