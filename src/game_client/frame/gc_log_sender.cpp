#include "gc_log_msg.h"
#include "gc_log_sender.h"

//#pragma comment(lib,"rtlib_core.lib")

#define LMSG_WRITE_ERROR			  CHECKEX("LogMsg 写入错误")
#define LMSG_WRITE_CMD(d,cmd)		  if (!msg.cmd(d)) {LMSG_WRITE_ERROR;return;}
#define LMSG_WRITE_CMDb(d,cmd)		  if (!msg.cmd(d)) {LMSG_WRITE_ERROR;return false;}

#define LMSG_WRITEb_char(d)           LMSG_WRITE_CMDb(d, WriteByte)
#define LMSG_WRITEb_byte(d)           LMSG_WRITE_CMDb(d, WriteByte)
#define LMSG_WRITEb_string(d)         LMSG_WRITE_CMDb(d, WriteString)
#define LMSG_WRITEb_short(d)          LMSG_WRITE_CMDb(d, WriteShort)
#define LMSG_WRITEb_ulong(d)          LMSG_WRITE_CMDb(d, WriteLong)
#define LMSG_WRITEb_long(d)           LMSG_WRITE_CMDb(d, WriteLong)


CLogSender::CLogSender(void)
{
	m_bValid = false;
	CM_MEMPROTECTOR(m_sMacAddress, 6)
}

CLogSender::~CLogSender(void)
{
	CM_MEMUNPROTECTOR(m_sMacAddress)
}

bool CLogSender::Init(char* logServerIP, int logServerPort)
{
	guard;

	if (!logServerIP)
		return false;

	LOG2("Init LogSender: Ip=%s, Port=%d\n", logServerIP, logServerPort);

//	m_remoteIP = inet_addr(str);
	m_remoteIP = logServerIP;
	m_remotePort = logServerPort;

	m_sUserName = rtUserName();
    if (m_sUserName.size()==0) m_sUserName = "[Unknown]";
	// begin gc_log_sender.cpp [3/15/2009 tooth.shi]
	/*
	RtSystemInfo sysInfo;
	if ( sysInfo.Init() )
	{
		SSystemInfo* info = sysInfo.GetSystemInfo();
		m_sIP = info->sIpAddr;
		memcpy(m_sMacAddress, info->sMacAddr, 6);		// sMacAddr为8个字节（最后2个为0），而m_sMacAddress为6个字节
	}
	else
	{
		m_sIP = "defaultIP";
		memset(m_sMacAddress, 0, 6);
	}

	if ( !m_udpSession.Create(NULL, 0) )
	{
		return false;
	}
	*/
	// end gc_log_sender.cpp [3/15/2009 tooth.shi]

	m_bValid = true;
	return true;

	unguard;
}

bool CLogSender::Destroy()
{
	guard;

	m_udpSession.Close();
	return true;

	unguard;
}

bool CLogSender::Send(CLogMsg *pMsg)
{
	guard;

	if (!m_bValid)
		return false;

	ELogMsgType type = pMsg->GetType();
	if ( !pMsg || type<=LMT_NULL || type>=LMT_COUNT )
		return false;

	CG_CmdPacket cmd;
	cmd.BeginWrite();
	cmd.WriteLong( pMsg->GetType() );							// Msy Type
	cmd.WriteLong( pMsg->GetDataSize() );						// DataSize(不包括以下3个变量)
	cmd.WriteString( (char*)m_sUserName.c_str() );				// 用户名
	cmd.WriteString( (char*)m_sIP.c_str() );					// 用户IP
	cmd.WriteBinary( (char*)m_sMacAddress, 6);					// 用户Mac地址
	cmd.WriteBinary(pMsg->GetData(), pMsg->GetDataSize() );

	//SOCKADDR_IN addrRemote;
	//memset(&addrRemote, 0, sizeof(addrRemote));
	//addrRemote.sin_family = AF_INET;
	//addrRemote.sin_port = htons(LOGSERVER_DEFORT_PORT);
	//addrRemote.sin_addr.s_addr = m_remoteIP;

	CG_NetAddress toAddress;
	toAddress.SetRemote( m_remoteIP.c_str() );
	toAddress.SetPort(m_remotePort);
	if ( !toAddress.BuildAddr() )
	{
		ERR("[LogSender] CG_NetAddress Build Address Error\n");
		return false;
	}
	if ( !m_udpSession.SendTo(&cmd, &toAddress) )
	{
		ERR("[LogSender] SendTo error\n");
		return false;
	}
    return true;

	unguard;
}

bool CLogSender::SendSysInfo(SSystemInfo *pInfo)
{
	guard;

	if (!pInfo)
		return false;

	char cTmp1024[1024];

	std::string str = "";

	str += "PcName=\"";
	str += pInfo->sComputerName;
	str += "\n";
	str += "CpuName=\"";
	str += pInfo->sCpuName;
	str += "\n";
	str += "CpuSpeed=\"";
	str += rtFormatNumber(pInfo->nCpuSpeed);
	str += "\n";
	str += "DisplayParams=\"";
	str += pInfo->sDisplayParams;
	str += "\n";
	str += "IpAddr=\"";
	str += pInfo->sIpAddr;
	str += "\n";
	str += "IpMask=\"";
	str += pInfo->sIpMask;
	str += "\n";
	str += "Language=\"";
	str += pInfo->sLanguage;
	str += "\n";
	str += "LoginUser=\"";
	str += pInfo->sLoginUserName;
	str += "\n";
	str += "MacAddr=\"";
	rt2_sprintf(cTmp1024, "%02X-%02X-%02X-%02X-%02X-%02X", (UCHAR)pInfo->sMacAddr[0], (UCHAR)pInfo->sMacAddr[1], (UCHAR)pInfo->sMacAddr[2], (UCHAR)pInfo->sMacAddr[3], (UCHAR)pInfo->sMacAddr[4], (UCHAR)pInfo->sMacAddr[5]);
	str += cTmp1024;
	str += "\n";
	str += "OsType=\"";
	str += pInfo->sOSType;
	str += "\n";
	str += "VideoChip=\"";
	str += pInfo->sVideoChip;
	str += "\n";
	str += "VideoDriver=\"";
	str += pInfo->sVideoDriver;
	str += "\n";
	str += "DriveCount=\"";
	str += rtFormatNumber(pInfo->nDriveCount);
	str += "\n";
	str += "MemTotal=\"";
	str += rtFormatNumber(pInfo->nMemTotal);
	str += "\n";
	str += "MemFree=\"";
	str += rtFormatNumber(pInfo->nMemAvailable);
	str += "\n";
	str += "VideoMemTotal=\"";
	str += rtFormatNumber(pInfo->nVideoMemTotal);
	str += "\n";
	str += "VideoMemFree=\"";
	str += rtFormatNumber(pInfo->nVideoMemFree);
	str += "\n";

	return SendString("SysInfo", (char*)str.c_str());

	//CLogMsg msg;
	//msg.SetType(LMT_SYSINFO);
	//LMSG_WRITEb_string(pInfo->sComputerName);
	//LMSG_WRITEb_string(pInfo->sCpuName);
	//LMSG_WRITEb_string(pInfo->sDisplayParams);
	//LMSG_WRITEb_string(pInfo->sIpAddr);
	//LMSG_WRITEb_string(pInfo->sIpMask);
	//LMSG_WRITEb_string(pInfo->sLanguage);
	//LMSG_WRITEb_string(pInfo->sLoginUserName);
	//for (UINT i=0; i<8; i++)
	//	LMSG_WRITEb_char(pInfo->sMacAddr[i]);
	//LMSG_WRITEb_string(pInfo->sOSType);
	//LMSG_WRITEb_string(pInfo->sVideoChip);
	//LMSG_WRITEb_string(pInfo->sVideoDriver);
	//LMSG_WRITEb_ulong(pInfo->nCpuSpeed);
	//LMSG_WRITEb_ulong(pInfo->nDriveCount);
	//for (UINT i=0; i<pInfo->nDriveCount; i++)
	//{
	//	LMSG_WRITEb_string(pInfo->sDriveName[i]);
	//	LMSG_WRITEb_ulong(pInfo->nDriveFreeSpace[i]);
	//	LMSG_WRITEb_ulong(pInfo->nDriveTotalSpace[i]);
	//	LMSG_WRITEb_ulong(pInfo->nDriveType[i]);
	//}
	//LMSG_WRITEb_ulong(pInfo->nMemAvailable);
	//LMSG_WRITEb_ulong(pInfo->nMemTotal);
	//LMSG_WRITEb_ulong(pInfo->nVideoMemFree);
	//LMSG_WRITEb_ulong(pInfo->nVideoMemTotal);

	//return Send(&msg);

	unguard;
}

bool CLogSender::SendString(char* title, char* content)
{
	guard;

	CLogMsg msg;
	msg.SetType(LMT_STRING);
	LMSG_WRITEb_string(title);
	LMSG_WRITEb_string(content);
	return Send(&msg);

	unguard;
}


CGameRegistry::CGameRegistry()
{
	m_bFirstRun = true;
}

CGameRegistry::~CGameRegistry()
{
}

bool CGameRegistry::Init()
{
	guard;

	m_bFirstRun = true;

	HKEY hKEY;
	LPCTSTR data_Set="Software\\Gamigo\\";
	LPCTSTR data_Set1="Software\\Gamigo\\ZFS\\";
	long ret = (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_ALL_ACCESS, &hKEY));
	if(ret != ERROR_SUCCESS)
	{
		ret = ::RegCreateKey(HKEY_LOCAL_MACHINE, data_Set, &hKEY);
		if (ret != ERROR_SUCCESS)
			return false;
	}
	::RegCloseKey(hKEY);

	ret=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set1, 0, KEY_ALL_ACCESS, &hKEY));
	if(ret != ERROR_SUCCESS)
	{
		ret = ::RegCreateKey(HKEY_LOCAL_MACHINE, data_Set1, &hKEY);
		if (ret != ERROR_SUCCESS)
			return false;
	}

	LPBYTE strData = RT_NEW BYTE[512];
	DWORD type_1 = REG_SZ;		//定义数据类型
	DWORD cbData_1 = 512;		//定义数据长度

	long ret1 = ::RegQueryValueEx(hKEY, "bbs", NULL, &type_1, strData, &cbData_1);
	if(ret1!=ERROR_SUCCESS)
	{
		m_bFirstRun = true;

		rt2_strncpy((char*)strData, "http://zfsbbs.gamigo.com.cn", 512);
		type_1 = REG_SZ;							//定义数据类型
		cbData_1 = (DWORD)strlen((char*)strData) + 1;			//定义数据长度
		ret1=::RegSetValueEx(hKEY, "bbs", NULL, type_1, strData, cbData_1);
		if(ret1!=ERROR_SUCCESS)
		{
			::RegCloseKey(hKEY);
			return false;
		}

		rt2_strncpy((char*)strData, "http://zfs.gamigo.com.cn", 512);
		type_1 = REG_SZ;							//定义数据类型
		cbData_1 = (DWORD)strlen((char*)strData) + 1;		//定义数据长度
		ret1=::RegSetValueEx(hKEY, "homepage", NULL, type_1, strData, cbData_1);
		if(ret1!=ERROR_SUCCESS)
		{
			::RegCloseKey(hKEY);
			return false;
		}
	}
	else 
	{
		m_bFirstRun =false;
	}

    DWORD type_2 = REG_SZ;		//定义数据类型
    DWORD cbData_2 = 512;		//定义数据长度
    ret1 = ::RegQueryValueEx(hKEY, "InstallPath", NULL, &type_2, strData, &cbData_2);
    if(ret1!=ERROR_SUCCESS)
    {
        ::GetCurrentDirectory(511, (LPSTR)strData);
        type_2 = REG_SZ;							    //定义数据类型
        cbData_2 = (DWORD)strlen((char*)strData) + 1;			//定义数据长度
        ret1=::RegSetValueEx(hKEY, "InstallPath", NULL, type_2, strData, cbData_2);
        if(ret1!=ERROR_SUCCESS)
        {
            ::RegCloseKey(hKEY);
            return false;
        }
    }

	::RegCloseKey(hKEY);

	DEL_ARRAY(strData)

	return true;

	unguard;
}

