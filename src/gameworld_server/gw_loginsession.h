#ifndef _INC_GW_LOGIN_SESSION_H_
#define _INC_GW_LOGIN_SESSION_H_

//
// 与login server的联系通道
//
class GWLoginSession : public CG_TCPSession
{
    friend class GWServer;

public:
    bool            CanLogin() { return (GetState() == NET_STATE_CONNECTED && m_acceptLogin); }
    CG_CmdPacket&   BeginCmd(short cmd) { m_sendCmd.BeginWrite(); m_sendCmd.WriteShort(cmd); return m_sendCmd; }
    bool            SendCmd() { return SendPacket(&m_sendCmd); }

    bool            Initalize();
    void            Process();
    void            InitChannel();
    void            SendShutdownOK(bool bForce); // 是否是强制关闭
    void            UpdateGameWorldStat();

    bool            LoginAccount(int seed,const char* account,const char* pwaccount,const char* pubwinKey, 
		const char* glompwd, const char* password,const char* ip/*PZH*/, bool bPowerLogin/**/);
    void            LogoutAccount(DWORD accountId);
    void            UpdateState(char cUpdate);
	//--------add start by tony 05.10.08-------//
	bool			LicenseCheck(int nType);
	bool			ReadMd5();
	//--------add end   by tony 05.10.08-------//

    long            CheckSubServerVersion(const char* szServerName, const char* szExeMD5, const char* szDataMD5, const char* szOS);
    void            OnCheckSubServerVersion(long dwCheckID, bool bSame, const char* szServerName);

private:
    virtual void    OnConnect(bool ret);
    virtual void    OnDisconnect();
    virtual void    OnReceive(CG_CmdPacket* packet);

public:
    GWLoginSession() : m_lastConnectTime(0) {this->SetDebugStr("GWLoginSession");}

private:
    CG_CmdPacket        m_recvCmd;
    CG_CmdPacket        m_sendCmd;
    GWCommandChannel    m_channel;
    time_t              m_lastConnectTime;
	//--------add start by tony 05.10.08-------//
	std::string			m_strMd5;
	//--------add end   by tony 05.10.08-------//

public:
    bool                m_acceptLogin;
};

#endif // _INC_GW_LOGIN_SESSION_H_
