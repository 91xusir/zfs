
#include "core/rt2_core.h"
#include "net/rt_net.h"
//#include "rt_system_info.h"
#include "license_check.h"
#include "ls_protocol.h"
#include "rt1tort2.h"
//#include "rt_system_info.h"

#ifdef LINUX
#include "../login_server/login.h"
#include "linux_util.h"
#else
#include "../login_server/login.h"
#endif //LINUX

#define SAFE_READ(p)	{ if(!(p)) return false; }

CNetSession g_ChkSession;

CNetSession::CNetSession()
{
}

CNetSession::~CNetSession()
{
	Close();
}

bool CNetSession::ReadMD5()
{
	FILE*	fp = NULL;
	char	strmd5[33];

	fp = fopen("license.dat", "r");

	if(!fp)
	{
		RtCoreLog().Info("File license.txt open error\n");
		return false;
	}

	for(int i = 0; i < 100; i++)
	{
		fgetc(fp);
	}

	fread(strmd5, 1, 32, fp);

	fclose(fp);

	strmd5[32] = '\0';

	m_strMd5 = strmd5;

	return true;
}

bool CNetSession::ReadMac()
{
	char	sMacAddr[6];
	m_strMac = "";

#ifdef LINUX
	char	sIP[16];
	if(GetIPMacAddr((unsigned char*)sMacAddr, sIP) != 0)
	{
		RtCoreLog().Info("LINUX Get IP/Mac Error\n");
		return false;
	}
#else
	// C [11/22/2008 Louis.Huang]
/*
	RtSystemInfo sysInfo;
	if (sysInfo.Init())
	{
		SSystemInfo* info = sysInfo.GetSystemInfo();
		memcpy(sMacAddr, info->sMacAddr, sizeof(sMacAddr));		// sMacAddr为8个字节（最后2个为0），而m_sMacAddress为6个字节
	}
	else
	{
		memset(sMacAddr, 0, 6);
	}
*/
#endif //LINUX

	for(int i = 0; i < 6; i++)
	{
		m_strMac += rtFormatNumber((unsigned char)sMacAddr[i]);
	}

	return true;
}

bool CNetSession::Init()
{
	RtIni ini;
	RtString szName, szValue;
	if(!ini.OpenFile("license_client.ini"))
	{
		return false;
	}
	ini.CloseFile();

	m_HostIP		= (const char*)ini["connect"]["HostIP"];
	m_HostPort		= (int)        ini["connect"]["HostPort"];
	m_ListenIP		= (const char*)ini["connect"]["ListenIP"];
	m_ListenPort	= (int)        ini["connect"]["ListenPort"];
	
	m_ToAddr.SetRemote(m_HostIP.c_str());
	m_ToAddr.SetPort(m_HostPort);

	m_ListenAddr.SetRemote(m_ListenIP.c_str());
	m_ListenAddr.SetPort(m_ListenPort);

	if (!m_ToAddr.BuildAddr())
	{
		RtCoreLog().Info("[CNetSession] Build Address Error\n");
		return false;
	}

	if (!m_ListenAddr.BuildAddr())
	{
		RtCoreLog().Info("[CNetSession] Build Address Error\n");
		return false;
	}

	if(!Create(m_ListenIP.c_str(), m_ListenPort))
	{
		RtCoreLog().Info("[CNetSession] Create Session Error\n");
		return false;
	}

	if(!ReadMD5())
	{
		RtCoreLog().Info("[CNetSession] ReadMD5 Error\n");
		return false;
	}

	if(!ReadMac())
	{
		RtCoreLog().Info("[CNetSession] ReadMac Error\n");
		return false;
	}

	m_SendPacket.SetSize(MAX_CMD_PACKET_SIZE);
	m_RecvPacket.SetSize(MAX_CMD_PACKET_SIZE);

	m_bLoop = true;
	m_bIsValid = false;

	BindCmd(ls2c_lc_check_once_license_rep, cmd_ls2c_lc_check_once_license_rep);
	BindCmd(ls2c_shutdown_login_license, cmd_ls2c_shutdown_login_license);

	return true;
}

void CNetSession::Process()
{
	if(RecvFrom(&m_RecvPacket, &m_FromAddr))
	{
		//RtCoreLog().Info("Command received form %s:%d\n", m_FromAddr.GetIP(), m_FromAddr.GetPort());
		OnReceive(&m_RecvPacket);
	}
}

void CNetSession::OnReceive(CG_CmdPacket *packet)
{
	guard;
	short CommandID;
	if(!packet->ReadShort(&CommandID)) return;
	this->DoCmd(CommandID, packet);
	return;
	unguard;
}

CG_CmdPacket* CNetSession::BeginWrite()
{
	guard;
	m_SendPacket.BeginWrite();
	return &m_SendPacket;
	unguard;
}

bool CNetSession::Send(CG_CmdPacket* pPacket)
{
	guard;
	return SendTo(pPacket, &m_ToAddr);
	unguard;
}

bool CNetSession::DoCmd(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    EXT_SPACE::unordered_map<short, NetCmdCB>::iterator it;
	it = m_Bind.find(cmdID);
	if (it!=m_Bind.end())
	{
		return ((*it).second)(cmdID, pPacket);
	}
	return false;
	unguard;
}

bool CNetSession::IsBind(short cmdID)
{
	return (m_Bind.find(cmdID)!=m_Bind.end());
}

void CNetSession::BindCmd(short cmdID, NetCmdCB pFun)
{
	guard;
	m_Bind[cmdID] = pFun;
	unguard;
}

void CNetSession::UnbindCmd(short cmdID)
{
	guard;
	EXT_SPACE::unordered_map<short, NetCmdCB>::iterator it;
	it = m_Bind.find(cmdID);
	if (it!=m_Bind.end())
	{
		m_Bind.erase(it);
	}
	unguard;
}

bool NetSendLicenseCheck(char* strmac, char* strmd5, int nType)
{
	if(!strmd5) return false;
	if(!strmac) return false;

	CG_CmdPacket* pPacket = g_ChkSession.BeginWrite();
	pPacket->WriteShort(c2ls_lc_check_once_license);
	pPacket->WriteString("none");
	pPacket->WriteString(strmac);
	pPacket->WriteString(strmd5);
	pPacket->WriteLong((long)nType);

	return g_ChkSession.Send(pPacket);
}

bool NetSendLicenseCheck(char* strIP, char* strmac, char* strmd5, int nType)
{
	if(!strmd5) return false;
	if(!strmac) return false;
	if(!strIP)	return false;

	CG_CmdPacket* pPacket = g_ChkSession.BeginWrite();
	pPacket->WriteShort(c2ls_lc_check_once_license);
	pPacket->WriteString(strIP);
	pPacket->WriteString(strmac);
	pPacket->WriteString(strmd5);
	pPacket->WriteLong((long)nType);

	return g_ChkSession.Send(pPacket);
}

bool cmd_ls2c_lc_check_once_license_rep(short cmdID, CG_CmdPacket *pPacket)
{
	char*	str;

	SAFE_READ(pPacket->ReadString(&str))

	if(strcmp(str, "yes") != 0)
	{
		g_ChkSession.SetValid(false);
	}else
	{
		g_ChkSession.SetValid(true);
	}

	g_ChkSession.SetLoop(false);

	return true;	
}

bool cmd_ls2c_shutdown_login_license(short cmdID, CG_CmdPacket *pPacket)
{
	RtCoreLog().Info("msg received for ShutDown\n");
	g_lgs->Shutdown();
	return true;
}

bool LicenseCheck(int nType)
{
	RtCoreLog().Info("Waiting for license check...\n");
	g_ChkSession.Init();
	NetSendLicenseCheck((char*)g_ChkSession.GetStrMac().c_str(), (char*)g_ChkSession.GetStrMd5().c_str(), nType);
	while(g_ChkSession.GetLoop())
	//while(1)
	{
		g_ChkSession.Process();
		rtSleep(10);
	}

	return g_ChkSession.GetValid();
}

bool LicenseCheck(char* strIP, char* strMac, char* strMd5, int nType)
{
	RtCoreLog().Info("Waiting for license check... [type]:[%d]\n", nType);
	//g_ChkSession.Init();
	NetSendLicenseCheck(strIP, strMac, strMd5, nType);
	while(g_ChkSession.GetLoop())
	//while(1)
	{
		g_ChkSession.Process();
		rtSleep(10);
	}

	return g_ChkSession.GetValid();
}
