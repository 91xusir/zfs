#ifndef _GAME_LOG_SENDER_H
#define _GAME_LOG_SENDER_H

#include "core/rt2_core.h"
#include "net/rt_net.h"
//#include "rt_system_info.h"
#include "rt1tort2.h"
//using namespace std;

#define LOGSERVER_DEFORT_PORT		9110


class CLogSender
{
public:
	CLogSender(void);
	~CLogSender(void);
	bool Init(char* logServerIP, int logServerPort = LOGSERVER_DEFORT_PORT);
	bool Destroy();
	bool SendSysInfo(SSystemInfo *pInfo);
	//bool SendString(char* str);
	//bool SendErrorReport(char *errorString);
	bool SendString(char* title, char* content);

private:
	bool Send(CLogMsg *pMsg);

	string			m_sUserName;		// User name
	string			m_sIP;				// Client's IP
	// Client's Mac Address
	CM_MEMDEF(m_sMacAddress, 6)

	CG_UDPSession	m_udpSession;
	string			m_remoteIP;
	int				m_remotePort;

	bool			m_bValid;
};

class CGameRegistry
{
public:
	CGameRegistry();
	~CGameRegistry();
	bool Init();
	bool IsGameFirstRun() { return m_bFirstRun; }

private:
	bool m_bFirstRun;
};

#endif
