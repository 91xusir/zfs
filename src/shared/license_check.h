
#ifndef _LICENSE_CHECK_
#define _LICENSE_CHECK_

#include "net/g_udpsession.h"

//#define LICENSE_CHECK
class CG_UDPSession;
class CNetSession : public CG_UDPSession
{
	typedef bool (*NetCmdCB)(short cmdID, CG_CmdPacket *pPacket);

public:
	CNetSession();
	virtual ~CNetSession();

public:
	bool			Init();
	CG_CmdPacket*	BeginWrite();
	bool			Send(CG_CmdPacket* pPacket);
	void			BindCmd(short cmdID, NetCmdCB pFun);
	bool			IsBind(short cmdID);
	void			UnbindCmd(short cmdID);
	void			Process();
	void			SetLoop(bool bloop) { m_bLoop = bloop; }
	bool			GetLoop() { return m_bLoop; }
	void			SetValid(bool bv) { m_bIsValid = bv; }
	bool			GetValid() { return m_bIsValid; }
	std::string		GetStrMd5() { return m_strMd5; }
	std::string		GetStrMac() { return m_strMac; }

protected:
	void			OnReceive(CG_CmdPacket *packet);
	bool			DoCmd(short cmdID, CG_CmdPacket *pPacket);
	bool			ReadMD5();
	bool			ReadMac();

protected:
	CG_CmdPacket					m_SendPacket;   // 要发送的消息
	CG_CmdPacket					m_RecvPacket;   // 要接受的消息
	EXT_SPACE::hash_map<short, NetCmdCB>  m_Bind;         // 绑定命令
	CG_NetAddress					m_ToAddr;
	CG_NetAddress					m_ListenAddr;
	CG_NetAddress					m_FromAddr;

	std::string			m_HostIP;
	int					m_HostPort;
	std::string			m_ListenIP;
	int					m_ListenPort;
	bool				m_bLoop;
	bool				m_bIsValid;
	std::string			m_strMac;
	std::string			m_strMd5;
};

extern "C" CNetSession	g_ChkSession;
extern bool NetSendLicenseCheck(char* strmac, char* strmd5, int nType);
extern bool cmd_ls2c_lc_check_once_license_rep(short cmdID, CG_CmdPacket *pPacket);
extern bool cmd_ls2c_shutdown_login_license(short cmdID, CG_CmdPacket *pPacket);
extern bool LicenseCheck(int nType);
extern bool LicenseCheck(char* strIP, char* strMac, char* strMd5, int nType);

#endif //_LICENSE_CHECK_

