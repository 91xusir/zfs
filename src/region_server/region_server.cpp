//
// s_regionserver.cpp: implementation of the CRegionServer class.
//
#include "region.h"
#include "rt_randomimage.h"
#include "rs_national_war.h"
//#include "rt_system_info.h"
#include "jhwg_session.h"
#include "RegionActManager.h"

#include "PlayerLogic.h"
#include "region_client.h"
#include <strstream>
#ifdef LINUX
#include "linux_util.h"
#endif //LINUX

const int REGION_TIMER_PERFORMANCE_LOG		 = OBJECT_C_TIMER_BASE + 1;
const int REGION_TIMER_STATE_REPORT          = OBJECT_C_TIMER_BASE + 2; 
const int REGION_TIMER_UPDATE_EUDEMON        = OBJECT_C_TIMER_BASE + 3;
const int REGION_TIMER_UPDATE_FUN			 = OBJECT_C_TIMER_BASE + 4;
const int REGION_TIMER_CHECK_CITY_WAR        = OBJECT_C_TIMER_BASE + 5;
const int REGION_TIMER_CHECK_DUNGEON         = OBJECT_C_TIMER_BASE + 6;
const int REGION_TIMER_END_FUN				 = OBJECT_C_TIMER_BASE + 7;
const int REGION_TIMER_CHECK_CHEATER         = OBJECT_C_TIMER_BASE + 8;
const int REGION_TIMER_LINESYS_SPAWN         = OBJECT_C_TIMER_BASE + 9;
const int REGION_TIMER_MEM_CHECK	         = OBJECT_C_TIMER_BASE + 10;

std::string	g_strConnectLicenseIP;
long			g_strConnectLicensePort;

// 取得检查的MD5，下面这段代码在服务器和工具中也需要使用

void CRegionServer::GetCheckCode(const char* szCode, char* szCheckCode)
{

	char szMD5[40];
	char szMD5Dest[40];
	RtString str;

	int iRand = szCode[6] - 'F';
	szMD5[16] = 0;
	szMD5[15] = szCode[ 1] - iRand;
	szMD5[14] = szCode[ 2] - iRand;
	szMD5[13] = szCode[ 3] - iRand;
	szMD5[12] = szCode[ 4] - iRand;
	szMD5[11] = szCode[ 7] - iRand;
	szMD5[10] = szCode[ 8] - iRand;
	szMD5[ 9] = szCode[ 9] - iRand;
	szMD5[ 8] = szCode[10] - iRand;
	szMD5[ 7] = szCode[13] - iRand;
	szMD5[ 6] = szCode[14] - iRand;
	szMD5[ 5] = szCode[15] - iRand;
	szMD5[ 4] = szCode[16] - iRand;
	szMD5[ 3] = szCode[19] - iRand;
	szMD5[ 2] = szCode[20] - iRand;
	szMD5[ 1] = szCode[21] - iRand;
	szMD5[ 0] = szCode[22] - iRand;

	rtMD5StringString(szMD5, szMD5Dest);
	str = szMD5Dest;
	str.Replace('1', 'L');
	str.Replace('2', 'I');
	str.Replace('3', 'H');
	str.Replace('4', 'P');
	str.Replace('5', 'K');
	str.Replace('6', 'O');
	str.Replace('7', 'M');
	str.Replace('8', 'R');
	str.Replace('9', 'N');
	str.Replace('0', 'Q');
	str.Replace('.', 'J');

	int md5Start = 11;
	szCheckCode[ 0] = szCode[0];
	szCheckCode[ 1] = str[md5Start++];
	szCheckCode[ 2] = str[md5Start++];
	szCheckCode[ 3] = str[md5Start++];
	szCheckCode[ 4] = str[md5Start++];
	szCheckCode[ 5] = '-';
	szCheckCode[ 6] = szCode[1];
	szCheckCode[ 7] = str[md5Start++];
	szCheckCode[ 8] = str[md5Start++];
	szCheckCode[ 9] = str[md5Start++];
	szCheckCode[10] = str[md5Start++];
	szCheckCode[11] = '-';
	szCheckCode[12] = szCode[2];
	szCheckCode[13] = str[md5Start++];
	szCheckCode[14] = str[md5Start++];
	szCheckCode[15] = str[md5Start++];
	szCheckCode[16] = str[md5Start++];
	szCheckCode[17] = '-';
	szCheckCode[18] = szCode[3];
	szCheckCode[19] = str[md5Start++];
	szCheckCode[20] = str[md5Start++];
	szCheckCode[21] = str[md5Start++];
	szCheckCode[22] = str[md5Start++];
	szCheckCode[23] = 0;

	strupr(szCheckCode); //变大写
}

bool CRegionServer::AuthorizationCheck(const char* szListenIP, const char* szConnectIP, int iConnectPort) // szListenIP为监听地址
{

	return true;

	char	szMac[6];

#ifdef LINUX
	char	sIP[16];
	if(GetIPMacAddr((unsigned char*)szMac, sIP) != 0)
	{
		//LOG("LINUX Get IP/Mac Error\n");
		return false;
	}
#else
	// C [11/22/2008 Louis.Huang]
/*
	RtSystemInfo sysInfo;
	if (sysInfo.Init())
	{
		SSystemInfo* info = sysInfo.GetSystemInfo();
		memcpy(szMac, info->sMacAddr, sizeof(szMac));		// sMacAddr为8个字节（最后2个为0），而m_sMacAddress为6个字节
	}
	else
	{
		memset(szMac, 0, 6);
	}
*/
#endif //LINUX

	/*
	RtString str = szMac;// + szListenIP; // 连接mac地址和ip
	str += szListenIP;

	// 把数字都替换成字母，以免被猜测
	str.Replace('1', 'H');
	str.Replace('2', 'I');
	str.Replace('3', 'J');
	str.Replace('4', 'K');
	str.Replace('5', 'L');
	str.Replace('6', 'M');
	str.Replace('7', 'N');
	str.Replace('8', 'O');
	str.Replace('9', 'P');
	str.Replace('0', 'Q');
	str.Replace('.', 'R');


	// 取得MD5编码

	char szMD5[40];
	memcpy(szMD5, str.GetBuffer(40), sizeof(szMD5));
	char szMD5Dest[40];

	rtMD5StringString(szMD5, szMD5Dest);

	str = szMD5Dest;


	// 把MD5编码中的数字转成字母
	str.Replace('1', 'N');
	str.Replace('2', 'M');
	str.Replace('3', 'R');
	str.Replace('4', 'O');
	str.Replace('5', 'L');
	str.Replace('6', 'I');
	str.Replace('7', 'H');
	str.Replace('8', 'P');
	str.Replace('9', 'K');
	str.Replace('0', 'Q');
	str.Replace('.', 'J');


	// 下面的代码是是生成一串号码

	// 生成号码
	char szDate[4];
	time_t now = time(NULL);
	struct tm* ptm = localtime(&now); 

	int iRand = rand()%8;
	szDate[0] = 'A' + (ptm->tm_year - 90) % ('Z'-'A');							// 年
	szDate[1] = 'F' + iRand;													// 随机
	szDate[2] = 'G' + ptm->tm_mon + 1;											// 月
	szDate[3] = (ptm->tm_mday>25)?('0'+ptm->tm_mday-25):('B'+ptm->tm_mday);		// 日

	int md5Start = 11;
	char szCode[24]; // 号码，格式为 XXXXX-XXXXX-XXXXX-XXXXX
	szCode[ 0] = szDate[0];
	szCode[ 1] = str[md5Start++] + iRand;
	szCode[ 2] = str[md5Start++] + iRand;
	szCode[ 3] = str[md5Start++] + iRand;
	szCode[ 4] = str[md5Start++] + iRand;
	szCode[ 5] = '-';
	szCode[ 6] = szDate[1];
	szCode[ 7] = str[md5Start++] + iRand;
	szCode[ 8] = str[md5Start++] + iRand;
	szCode[ 9] = str[md5Start++] + iRand;
	szCode[10] = str[md5Start++] + iRand;
	szCode[11] = '-';
	szCode[12] = szDate[2];
	szCode[13] = str[md5Start++] + iRand;
	szCode[14] = str[md5Start++] + iRand;
	szCode[15] = str[md5Start++] + iRand;
	szCode[16] = str[md5Start++] + iRand;
	szCode[17] = '-';
	szCode[18] = szDate[3];
	szCode[19] = str[md5Start++] + iRand;
	szCode[20] = str[md5Start++] + iRand;
	szCode[21] = str[md5Start++] + iRand;
	szCode[22] = str[md5Start++] + iRand;
	szCode[23] = 0;

	char szCheckCode[24]; // 序列号，格式为 XXXXX-XXXXX-XXXXX-XXXXX，就是szCode中MD5的部分倒序再来一次MD5
	char szcpCheckCode[24];
	char szcpCheckCodeSV[24];

	strupr(szCode); //变大写

	GetCheckCode(szCode, szCheckCode);

	memcpy(szcpCheckCode, szCheckCode, sizeof(szCheckCode));
	UnDisturb(szcpCheckCode);

	if (g_pFileManager->FileExist("region_server.gamigo.auth")) // （如：region_server.gamigo.auth）
	{
		char szFileRead[24];
		memset(szFileRead, 0, sizeof(szFileRead));

		RtArchive* pFile = g_pFileManager->CreateFileReader("region_server.gamigo.auth");
		if (pFile==NULL)
		{
			ERR("ReadAuthFile Error: Loading region_server.gamigo.auth Failed.\n");
			return false;
		}

		pFile->Serialize(szFileRead, sizeof(szFileRead));
		// 清除日期（因为每天日期都会不同）
		UnDisturb(szFileRead);
        
		// 比较
		if (stricmp(szFileRead, szcpCheckCode)==0)
		{
			return true;
		}
	}


	szCheckCode[ 0] = szCode[0];
	szCheckCode[ 6] = szCode[6];
	szCheckCode[12] = szCode[12];
	szCheckCode[18] = szCode[18];


	// 如果有服务器存在就发送给服务器，否则显示在屏幕上等待输入序列号

	//用TCP连接服务器;// 在端口 iConnectPort 连接 szConnectIP

	bool bSend = false;
	int iTimeout = 70; // 20秒
	MSG(" >");
	m_AuthSession.m_strHostIP = szConnectIP;
	m_AuthSession.m_iHostPort = iConnectPort;
	do{
		m_AuthSession.Process();
		iTimeout --;
		rtSleep(300);
		if(m_AuthSession.m_bConnected)
		{
			if(!bSend)
			{
				CG_CmdPacket& cmd=m_AuthSession.BeginSend();
				cmd.WriteShort(1000);
				cmd.WriteString((char*)szCode);
				m_AuthSession.EndSend();
				bSend = true;
			}
			if (m_AuthSession.m_bRecvData)
			{
				break;
			}
		}else
		{
			bSend = false;
		}
		MSG(">");
	}while (iTimeout>0);

	if (m_AuthSession.m_bRecvData)
	{
		MSG("|\n");
		//断开网络连接;
		m_AuthSession.Close();

		rt2_strncpy(szcpCheckCodeSV, m_AuthSession.m_strCheckCode.c_str(), sizeof(szcpCheckCodeSV));
		szcpCheckCodeSV[23] = 0;

		UnDisturb(szcpCheckCodeSV);
		//把返回数据写入文件;
		if (stricmp(szcpCheckCode, szcpCheckCodeSV)==0)
		{
			//把输入数据写入文件

			FILE*	fp = NULL;
			if((fp = fopen("region_server.gamigo.auth", "w")) == NULL)
			{
				return false;
			}
			fputs((char*)szCheckCode, fp);
			fclose(fp);
			return true;
		}else
		{
			return false;
		}
	}else
	{
		MSG("\n");
	}

	//断开网络连接;
	m_AuthSession.Close();
/*
	/*
	int iTimeout = 100; // 10秒
	bool bConnected = false;
	do{
		if(m_AuthSession.Connect(szConnectIP, iConnectPort))
		{
			bConnected = true;
			break;
		}
		rtSleep(100);
		iTimeout --;

	}while (!bConnected && iTimeout>0);

	iTimeout = 100; // 10秒

	do{
		m_AuthSession.Process();
		iTimeout--;
		rtSleep(100);

		if(m_AuthSession.m_bConnected)
		{
			static bool bSend = false;
			if(!bSend)
			{
				CG_CmdPacket& cmd=m_AuthSession.BeginSend();
				cmd.WriteShort(1000);
				cmd.WriteString((char*)szCode);
				m_AuthSession.EndSend();

				bSend = true;
			}
		}
	}while(!m_AuthSession.m_bRecvData && iTimeout>0);
	
	if (m_AuthSession.m_bRecvData)
	{
		//断开网络连接;
		m_AuthSession.Close();

		memcpy(szcpCheckCodeSV, m_AuthSession.m_strCheckCode.c_str(), sizeof(szcpCheckCodeSV));
		UnDisturb(szcpCheckCodeSV);
		//把返回数据写入文件;
		if (strcmp(szcpCheckCode, szcpCheckCodeSV)==0)
		{
			//把输入数据写入文件

			FILE*	fp = NULL;
			if((fp = fopen("region_server.gamigo.auth", "w")) == NULL)
			{
				return false;
			}
			fputs((char*)szCheckCode, fp);
			fclose(fp);
			return true;
		}else
		{
			return false;
		}
	}

	//断开网络连接;
	m_AuthSession.Close();
	*/

	// 网络方法失败，使用用户输入的方法

/*
	char szInput[1024];
	printf("请根据下面注册号码取得序列号，并且输入。\n注册号码为：%s\n", szCode);
	do{
		printf("请输入序列号：");
		gets(szInput); // 等待输入
		strupr(szInput); // 变大写
		strupr(szcpCheckCode);

		if ((strcmp(szInput, "EXIT")==0) || (strcmp(szInput, "QUIT")==0))
		{
			g_region->Shutdown();
			return false;
		}
		
		szInput[ 0] = ' ';
		szInput[ 6] = ' ';
		szInput[12] = ' ';
		szInput[18] = ' ';
		szInput[25] = 0; // 写成25是为了防止截断一些确实是多输入的

		if (strcmp(szInput, szcpCheckCode)==0)
		{
			//把输入数据写入文件
			FILE*	fp = NULL;
			if((fp = fopen("region_server.gamigo.auth", "w")) == NULL)
			{
				return false;
			}
			fputs((char*)szCheckCode, fp);
			fclose(fp);
			return true;
		}
		
		if (strlen(szInput)!=23)
		{
			printf("序列号格式错误，格式应该为 XXXXX-XXXXX-XXXXX-XXXXX\n输入Exit或者Quit退出本程序\n");
		}
	}while (1);
	return false;
*/
}

void CRegionServer::Disturb(char* data, char a, char r, char g, char b)
{
	data[ 0] = a;
	data[ 6] = r;
	data[12] = g;
	data[18] = b;
}

void CRegionServer::UnDisturb(char* data)
{
	data[ 0] = ' ';
	data[ 6] = ' ';
	data[12] = ' ';
	data[18] = ' ';
}

// action = 1 不作处理
//          2 封闭游戏动作 [时间(秒)]
//          3 断线
//          4 断线并封号 [时间(秒)]
void GMPunishWGCallback(unsigned int userID, int action, int value)
{
	LOG3("[WGCheck]: User [%d] used wg[%d,%d], punished\n", userID, action, value);
	CRegionUser* pUser = g_region->FindUser(userID);
	if(!pUser) return;
	if (!pUser->m_dummy) return;
	char szMsg[200];

	switch (action)
	{
	case 1: // 解释成警告
		pUser->m_dummy->SendSystemMessage("系统检查到您正在使用外挂，请马上停止使用，否则后果自负");
		break;
	case 2: // 解释成禁止移动
		rt2_sprintf(szMsg, "系统第二次检查到您正在使用外挂，停止您移动%d秒", value);
		pUser->m_dummy->SendSystemMessage(szMsg);
        pUser->ForbidUser(true, CreState_CanMove, value);
		break;
	case 3: // 解释成断线
		//pUser->m_dummy->SendSystemMessage("系统第三次检查到您正在使用外挂，强制断线");
		g_region->KickUser(userID, 0, true, true,"break line");
		break;
	case 4: // 解释成断线并封号, -1为永久封号
		//pUser->m_dummy->SendSystemMessage("系统第三次检查到您正在使用外挂，强制断线");
		g_region->KickUser(userID, 0, true, true,"lock user");
        if (value==-1)
        {
            pUser->LockAccount(true, 365*24*60*60); // 一年
        }else
        {
            pUser->LockAccount(true, value);
        }
		break;
	}
}

CRegionServer::CRegionServer():m_HeroList(HERO_LIST_COUNT, UNION_LIST_COUNT)
, m_canAcceptNewLink(false)
{
	m_activeCmd = RT_NEW CG_CmdPacket;
	m_activeCmd->SetSize(MAX_CMD_PACKET_SIZE);
	m_linkId = 1;
	InitChannel();

	m_gwt.Initialize(0, 1);
    m_eState = ssStartup;
    m_pNetLog = NULL;
    m_lCurRegionID = -1;

	m_funBegTime = 0;
	m_funInterval = 0;
	m_bFixFunActive = false;
	m_bTmpFunActive = false;

	m_funMap.push_back("scene01_081.rtb");
	m_funMap.push_back("scene01_082.rtb");
	m_funMap.push_back("scene01_083.rtb");
	m_funMap.push_back("scene01_084.rtb");

	m_funMap.push_back("scene01_085.rtb");
	m_funMap.push_back("scene01_086.rtb");
	m_funMap.push_back("scene01_087.rtb");
	m_funMap.push_back("scene01_088.rtb");

	m_funMap.push_back("scene01_089.rtb");
	m_funMap.push_back("scene01_090.rtb");
	m_funMap.push_back("scene01_091.rtb");
	m_funMap.push_back("scene01_092.rtb");

	m_funMap.push_back("scene01_093.rtb");
	m_funMap.push_back("scene01_094.rtb");
	m_funMap.push_back("scene01_095.rtb");
	m_funMap.push_back("scene01_096.rtb");

    m_randomImageData.szStrBuf[0] = 0;
    m_randomImageData.iDataSize = 0;

	m_expRate = 1.0f;
	m_bKickCheater = false;
    m_iCheaterValve = 1;
    m_iCheaterCnt = 0;
    m_lKickAllUserTime = 0;
    m_dwKickAllUserLastTickTime = 0;

    m_bAutoProcessCheck = true;
    m_iMinCheckTime = (int)(0.2*30*60);   // 0.2 小时
    m_iMaxCheckTime = (int)(0.5*60*60);   // 0.5 小时
    m_listWgProcesses.push_back("ZfsSkin.dll"); // 无级
    m_listWgProcesses.push_back("zfs1.dll");    // 龙眼1
    m_listWgProcesses.push_back("zfs2.dll");    // 龙眼2

    m_iWgCheckedTotal = 0;
	// m_maxCheatNpc = 0;
	// m_curCheatNpc = 0;

    m_bJhWgCheck = false;
    m_bJhWgServer = false;
    m_iClientCheckCodeCnt = 0;
    m_pJhWgServer = RT_NEW CJhWgSession;

    m_pRandomImgFactory = RT_NEW RtRandomImage;

	m_mc = CMonCastle::GetMCInstance();//tim.yang  MC
	m_useCardMinuteBottleMaxNum = 0;
	ReadUseCardMinBottleMaxNum();
}

CRegionServer::~CRegionServer()
{
	if (m_mc)
		DEL_ONE(m_mc);
	m_defScene->Exit();
	DEL_ONE(m_pItemManager);
	DEL_ONE(m_defScene);
    DEL_ONE(m_pJhWgServer);
	DEL_ONE(m_activeCmd);
    DEL_ONE(m_pRandomImgFactory);
    m_pRandomImgFactory = NULL;
}

// -----------------------------------------------------------------------
// setup command channel infomation 
// -----------------------------------------------------------------------
void CRegionServer::InitChannel()
{
	m_channelUnlogin.m_channelName = "UnloginC";
	m_channelLogin.m_channelName   = "LoginC";

	// cmd used when unlogin
	m_channelUnlogin.AddCmd(cmd_c2r_login,c2r_login,"login","");
    m_channelUnlogin.AddCmd(cmd_c2r_start,c2r_start,"start","");
	m_channelUnlogin.AddCmd(cmd_c2r_select_char,c2r_select_char,"","");

	// cmd valid when login finish 
	m_channelLogin.AddCmd(cmd_c2r_chat,c2r_chat,"chat","");
	m_channelLogin.AddCmd(cmd_c2r_update_position,c2r_update_position,"update pos","");

	m_channelLogin.AddCmd(cmd_c2r_ride,c2r_ride,"ride on/off","");
	m_channelLogin.AddCmd(cmd_c2r_attack,c2r_attack,"attack","");
	m_channelLogin.AddCmd(cmd_c2r_update_model,c2r_update_model,"update model info","");

	m_channelLogin.AddCmd(cmd_c2r_p_change_model,c2r_p_change_model,"","");
	m_channelLogin.AddCmd(cmd_c2r_get_model_info,c2r_get_model_info,"","");
	m_channelLogin.AddCmd(cmd_c2r_p_change_item,c2r_p_change_item,"","");
	m_channelLogin.AddCmd(cmd_c2r_add_attr_point,c2r_add_attr_point,"","");

	m_channelLogin.AddCmd(cmd_c2r_subsystem_mail,c2r_subsystem_mail,"","");
	m_channelLogin.AddCmd(cmd_c2r_subsystem_team,c2r_subsystem_team,"","");
	m_channelLogin.AddCmd(cmd_c2r_subsystem_item,c2r_subsystem_item,"","");
	m_channelLogin.AddCmd(cmd_c2r_subsystem_union,c2r_subsystem_union,"","");
	m_channelLogin.AddCmd(cmd_c2r_subsystem_feod,c2r_subsystem_feod,"","");
	m_channelLogin.AddCmd(cmd_c2r_interactive_npc,c2r_interactive_npc,"","");
	m_channelLogin.AddCmd(cmd_c2r_message_dialog_sure,c2r_message_dialog_sure,"","");
	m_channelLogin.AddCmd(cmd_c2r_herolist_query,c2r_herolist_query,"","");

	m_channelLogin.AddCmd(cmd_c2r_cancel_task,c2r_cancel_task,"","");
	m_channelLogin.AddCmd(cmd_c2r_query_task_overhead, c2r_query_task_overhead, "", "");

    m_channelLogin.AddCmd(cmd_c2r_study_skill,c2r_study_skill,"","");
	m_channelLogin.AddCmd(cmd_c2r_shortcut, c2r_shortcut, "", "");
	m_channelLogin.AddCmd(cmd_c2r_save_config, c2r_save_config, "", "");

	m_channelLogin.AddCmd(cmd_c2r_mw_add_mp, c2r_mw_add_mp, "", "");
	m_channelLogin.AddCmd(cmd_c2r_mw_add_ele, c2r_mw_add_ele, "", "");

	m_channelLogin.AddCmd(cmd_c2r_print_my_data, c2r_print_my_data, "", "");
	m_channelLogin.AddCmd(cmd_c2r_recover_pet, c2r_recover_pet, "", "");
	m_channelLogin.AddCmd(cmd_c2r_play_pose, c2r_play_pose, "", "");
	m_channelLogin.AddCmd(cmd_c2r_teleport, c2r_teleport, "", "");
//-------------add start by tony 05.06.06-----------------------------------------//
	m_channelLogin.AddCmd(cmd_c2r_subsystem_castlewar,c2r_subsystem_castlewar,"","");
	m_channelLogin.AddCmd(cmd_c2r_subsystem_auction,c2r_system_auction,"","");
	//-------------add end   by tony 05.06.06-----------------------------------------//
//rendh 2010.01.21
	m_channelLogin.AddCmd(cmd_c2r_subsystem_linesys,c2r_subsystem_linesys,"","");
//rendh 2010.01.21
	m_channelLogin.AddCmd(cmd_c2r_quit_game, c2r_quit_game,"","");
	m_channelLogin.AddCmd(cmd_c2r_confirm_switch_region, c2r_confirm_switch_region,"","");
	m_channelLogin.AddCmd(cmd_c2r_request_clear_skill, c2r_request_clear_skill,"","");
	m_channelLogin.AddCmd(cmd_c2r_confirm_clear_skill, c2r_confirm_clear_skill,"","");
	m_channelLogin.AddCmd(cmd_c2r_set_pet_info, c2r_set_pet_info ,"","");
	m_channelLogin.AddCmd(cmd_c2r_get_next_gift, c2r_get_next_gift, "", "");
	m_channelLogin.AddCmd(cmd_c2r_query_player_info, c2r_query_player_info, "", "");
	m_channelLogin.AddCmd(cmd_c2r_finish_fb_task, c2r_finish_fb_task, "", "");
	m_channelLogin.AddCmd(cmd_c2r_leave_fb, c2r_leave_fb, "", "");
	m_channelLogin.AddCmd(cmd_c2r_enter_fb, c2r_enter_fb, "", "");
	m_channelLogin.AddCmd(cmd_c2r_report_info, c2r_report_info, "", "");
	m_channelLogin.AddCmd(cmd_c2r_charge_card, c2r_charge_card, "", "");
	m_channelLogin.AddCmd(cmd_c2r_check_answer, c2r_check_answer, "", "");
	m_channelLogin.AddCmd(cmd_c2r_master_prentice, c2r_master_prentice, "", "");
	m_channelLogin.AddCmd(cmd_c2r_request_card_list, c2r_request_card_list, "", "");
	m_channelLogin.AddCmd(cmd_c2r_buy_card, c2r_buy_card, "", "");
	m_channelLogin.AddCmd(cmd_c2r_consign_card, c2r_consign_card, "", "");
	m_channelLogin.AddCmd(cmd_c2r_exp_card, c2r_exp_card, "", "");
	m_channelLogin.AddCmd(cmd_c2r_master_list,c2r_master_list,"","");
	m_channelLogin.AddCmd(cmd_c2r_warfield_enter,c2r_warfield_enter,"","");
	m_channelLogin.AddCmd(cmd_c2r_warfield_leave,c2r_warfield_leave,"","");
	m_channelLogin.AddCmd(cmd_c2r_warfield_view, c2r_warfield_view, "", "");
	m_channelLogin.AddCmd(cmd_c2r_save_circle, c2r_save_circle, "", "");
	m_channelLogin.AddCmd(cmd_c2r_active_circle, c2r_active_circle, "", "");
	m_channelLogin.AddCmd(cmd_c2r_rage_burst,c2r_rage_burst,"","");
	m_channelLogin.AddCmd(cmd_c2r_send_message,c2r_send_message,"","");

	//PZH
	m_channelLogin.AddCmd(cmd_c2r_shared_task, c2r_shared_task, "", "");
	m_channelLogin.AddCmd(cmd_c2r_accept_shared_task, c2r_accept_shared_task, "", "");
	//
	m_channelLogin.AddCmd(cmd_c2r_user_rebirth, c2r_user_rebirth, "", "");
	m_channelLogin.AddCmd(cmd_c2r_pickitem_type, c2r_pickitem_type, "", "");
	m_channelLogin.AddCmd(cmd_c2r_pickitem_id, c2r_pickitem_id, "", "");
	m_channelLogin.AddCmd(cmd_c2r_close_pickwindow, c2r_close_pickwindow, "", "");
	m_channelLogin.AddCmd(cmd_c2r_stop_getskin, c2r_stop_getskin, "", "");
	m_channelLogin.AddCmd(cmd_c2r_upload_taskid, c2r_upload_taskid, "", "");
	m_channelLogin.AddCmd(cmd_c2r_upload_finish_taskid, c2r_upload_finish_taskid, "", "");
	m_channelLogin.AddCmd(cmd_c2r_task_track, c2r_task_track, "", "");
	m_channelLogin.AddCmd(cmd_c2r_task_canecl_track, c2r_task_canecl_track, "", "");
	m_channelLogin.AddCmd(cmd_c2r_study_life_skill, c2r_study_life_skill, "", "");
	m_channelLogin.AddCmd(cmd_c2r_delete_life_skill,c2r_delete_life_skill,"","");
	m_channelLogin.AddCmd(cmd_c2r_pk_pick_item,c2r_pk_pick_item,"","");
	m_channelLogin.AddCmd(cmd_c2r_Guide, c2r_Guide,"","");
	m_channelLogin.AddCmd(cmd_c2r_change_pkmode, c2r_change_pkmode,"","");
	m_channelLogin.AddCmd(cmd_c2r_SynOnlineTime, c2r_SynOnlineTime,"","");
	m_channelLogin.AddCmd(cmd_c2r_ctrl_pet, c2r_ctrl_pet,"","");
	m_channelLogin.AddCmd(cmd_c2r_fuse_pet, c2r_fuse_pet,"","");
	m_channelLogin.AddCmd(cmd_c2r_catch_pet, c2r_catch_pet,"","");
	m_channelLogin.AddCmd(cmd_c2r_rebirth_user, c2r_rebirth_user,"","");
	m_channelLogin.AddCmd(cmd_c2r_execute_script, c2r_execute_script,"","");
	m_channelLogin.AddCmd(cmd_c2r_get_alterable, c2r_get_alterable,"","");
	m_channelLogin.AddCmd(cmd_c2r_set_refusesetting, c2r_set_refusesetting,"","");
}

bool CRegionServer::UpdateEudemonData()
{
    static short iTick = 0;
    m_cmdEudemon.BeginWrite();
    m_cmdEudemon.WriteShort(iTick++);
    if (!m_udpEudemonSession.SendTo(&m_cmdEudemon, &m_eudemonToAddress))
    {
        ERR("Can't send eudemon update cmd\n");
        m_udpEudemonSession.Close();
        return false;
    }
    return true;
}

bool CRegionServer::RegionWaittingForGWS()
{
    /*
    m_gws.Connect();
    m_gws.InitChannel();

    if (g_cfgRs.eudemon.enable)
    {
        m_udpEudemonSession.Create(NULL, 0);
        m_eudemonToAddress.SetRemote(g_cfgRs.eudemon.ip.c_str());
        m_eudemonToAddress.SetPort(g_cfgRs.eudemon.port);
        if (!m_eudemonToAddress.BuildAddr())
        {
            m_udpEudemonSession.Close();
            ERR2("Can't build eudemon address, IP=%s, Port=%d\n", g_cfgRs.eudemon.ip.c_str(), g_cfgRs.eudemon.port);
            g_cfgRs.eudemon.enable = false;
        }
    }

    ULONG dtime, eudemonTime=0;
    while(m_eState==ssWaittingConfig)
    {
        dtime = rtGetMilliseconds();
        if (g_cfgRs.eudemon.enable && (dtime-eudemonTime > (ULONG)g_cfgRs.eudemon.update_interval))
        {
            UpdateEudemonData();
            eudemonTime = dtime;
        }
        m_gws.Process();
        rtSleep(100);
    }
    if (m_eState==ssStartup)
    {
        g_cfgNet.Dump(g_pLog, "    ");
        return true;
    }
    */
    return true;
}

// -----------------------------------------------------------------------
// region server main loop,only exit loop when server shutdown 
// -----------------------------------------------------------------------
void CRegionServer::Run()
{
	if(!StartUp()) return;
    m_eState = ssRun;

    ULONG dwLastUpdateStatInfo = 0;
	ULONG dwLastUpdateTimer = 0;
	ULONG dtime, eudemonTime=0;
	ULONG dwPlayerTime = 0;
	static ULONG syncTime = 0,frame = 0;
	LOG1("fd_setsize = %d\n",FD_SETSIZE);
/*
#ifdef LINUX	
	LOG1("max = %d\n",OPEN_MAX);
#endif
*/
	// ULONG endTime = rtGetMilliseconds() + 30 * 1000;
	while(m_eState==ssRun)
	{
		// if(rtGetMilliseconds() > endTime) break;
		//const unsigned long c_nSetp = 20000;
		//static unsigned long s_NextTime = rtGetMilliseconds() + c_nSetp;
		//static int s_nNum = 0;
		
        if (m_pNetLog)
        {
            m_pNetLog->Process();
        }

        if (m_bJhWgServer && m_pJhWgServer)
        {
            m_pJhWgServer->ProcessNet();
        }

		// set region time
		m_time = rtGetMilliseconds();
		m_sec  = time(NULL);
 		m_gwt.Update();

		// check if time to sync with client
		int syncUsed = rtGetMilliseconds();
		if(m_time-syncTime >= (ULONG)g_cfg.listenClient.r2c_interval)
		{
            SyncToClient();
			syncTime = m_time;
			// LOG1("sync time = %d\n",rtGetMilliseconds() - syncUsed);
			// LOG1("id = %d\n",g_factory->PeekNextObjectId());
		}
		syncUsed = rtGetMilliseconds() - syncUsed;
		
		int procUsed = rtGetMilliseconds();
		Process();
		procUsed = rtGetMilliseconds() - procUsed;

		g_AntiRobot.Run();
		g_ItemSaleStat.Run();
		if(m_mc)
			m_mc->Run();//tim.yang  MC
		int timerUsed = 0;
		if(m_time - dwLastUpdateTimer > 100)
		{
			timerUsed = rtGetMilliseconds();
			CRegionObject::UpdateTimerObject();
			timerUsed = rtGetMilliseconds() - timerUsed;
			dwLastUpdateTimer = m_time;

		}

		//if(m_time - dwPlayerTime > 30000)
		//{
  //          m_cmdGiver->PlayTime();

		//	dwPlayerTime = m_time;
		//}


		g_factory->RealDestroyObject();
		dtime = rtGetMilliseconds() - m_time;
	
		if(dtime > 100) m_stat.busyFrameNum++;
		m_stat.frameNum++;
		m_stat.totalTime   += dtime;
		m_stat.syncTime    += syncUsed;
		m_stat.processTime += procUsed;
		m_stat.timerTime   += timerUsed;

		time_t tNow = time(NULL);
		struct tm* pTm = localtime(&tNow);
		if(m_yDay != pTm->tm_yday)
		{
			OnDayChange();
			m_yDay = pTm->tm_yday;
		}

       	// give cpu time to other app
		if(dtime < 50)
		{
			rtSleep(20);
		}
		else if(dtime > 100)
		{
			if(dtime > 30*1000)
			{
				NOTICE("RegionServer: frame time = %d,timer = %d,object = %d\n",
					dtime,CRegionObject::m_timerCalled,g_factory->GetObjectNum());
				NOTICE("RegionServer: sync = %d, process = %d, timer = %d\n",
					syncUsed,procUsed,timerUsed);
			}
			rtSleep(10);
		}
		else
		{
			rtSleep(10);
		}

		// 如果运行后3秒钟内没有接收到ItemID，就退出
		static DWORD dwRuningTick = rtGetMilliseconds();
		DWORD nowTick = rtGetMilliseconds();
		if ( !g_ItemIDFactory.HaveGotIDFromDB() && (nowTick-dwRuningTick > 30000))
		{
			ERR("Region Server Can NOT get ItemID init data from GWServer! Now Exit!\n");
			Reboot();
		}
		//PZH
		CRSLogicExt::GetRSLogicExt().OnRun(nowTick);
		//
		//if (rtGetMilliseconds() >= s_NextTime)
		//{
			
			/*std::ostringstream os;
			int nTemp = 0;
			if (0 != s_nNum)
			{
				nTemp = c_nSetp / s_nNum;
			}*/
			//os<<"c_nSetp: "<<s_nNum<<"秒: "<<s_nNum / (c_nSetp / 1000)<<"帧: "<<nTemp<<endl;
			//ERR(os.str().c_str());
			//s_NextTime = rtGetMilliseconds() + c_nSetp;
			//s_nNum = 0;
		//}
		//else
		//{
			 //s_nNum++;
		//}
	}

    // 提交未存盘的数据, 最多提交10次
    int i;
    for (i=0; i<10 && g_region->m_gws.CommitPendingData(); i++)
    {
        rtSleep(10);
    }

	// 保存所有的玩家数据
    LOG("CRegionServer::Run(): Save all user data (max 15 sec)\n");
    time_t tExitTime = time(NULL);
    EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator user = m_userMap.begin();
    for (; user != m_userMap.end();)
    {
        // 15秒内才需要处理存盘，超过15秒就不存盘直接退出
        if ((time(NULL)-tExitTime)<=15)
        {
            if (user->second->m_dummy)
            {
				if (user->second->m_dummy->m_pet)
					user->second->m_dummy->CallbackPet(true,false);
				user->second->m_dummy->RemoveWardNPC();
                user->second->m_dummy->m_Skill.ShutdownSkill();
                if (!m_gws.SaveUserData(user->first, user->second->m_dummy))
                {
                    rtSleep(10);
                    m_gws.DefProc();
                    continue;
                }
            }
        }
        m_gws.Logout(user->first, false);
        DEL_ONE(user->second);
        user ++;
    }
    m_userMap.clear();
    LOG1("                    time = %d seconds\n", (int)time(NULL)-tExitTime);

    LOG("CRegionServer::Run(): Send quited command to gameworld\n");
    CG_CmdPacket& cmd = m_gws.BeginSend();
    cmd.WriteShort(r2g_safe_quited);
    m_gws.EndSend();

    // 等待GameWorld的可以关闭命令, 20秒后强制关机
    dtime = rtGetMilliseconds() + 20*1000;
    while(m_gws.GetState()==NET_STATE_CONNECTED && rtGetMilliseconds()<dtime && !m_gws.m_bSafeQuited)
    {
        m_time = rtGetMilliseconds();
        m_gwt.Update();
        m_gws.Process();
        if (m_canAcceptNewLink)
            ProcessNewLink(); // 这里执行这个是为了拒绝新的网络连接
        rtSleep(10);
    }
    LOG1("CRegionServer::Run(): Region safe shutdown [%s]\n", m_gws.m_bSafeQuited?"SAFE":"FORCE");

	// region exit 
	Exit();
    if (g_cfgRs.eudemon.enable)
    {
        m_udpEudemonSession.Close();
    }

}

// -----------------------------------------------------------------------
// try startup region server 
// -----------------------------------------------------------------------
bool CRegionServer::StartUp()
{
	m_AuthSession.InitChannel();
	
	if(!AuthorizationCheck( g_cfgRs.connectGameWorld.bindIP.c_str(), 
							g_strConnectLicenseIP.c_str(),
							g_strConnectLicensePort))
	{
		LOG("RegionServer StartUp Failed: Cannot Get AuthorizationData\n");
		return false;
	}
	
	m_defScene = (CRegionScene*)g_factory->CreateObject(OBJECT_TYPE_SCENE);

	// g_region->IsHashError(1000,0,0);
#ifdef WIN32
    m_stat.sv.UpdateMD51("region_server.exe");
#else
    m_stat.sv.UpdateMD51("region_server");
#endif
    m_stat.sv.UpdateMD52("region_server.pak");

    // net log system
    m_pNetLog = RT_NEW CG_TCPBroadcastArchive;
    if (m_pNetLog)
    {
        m_pNetLog->Start(g_cfgRs.connectGameWorld.bindIP.c_str(), g_cfgRs.listenClient.listenPort+5);
		// C [11/22/2008 Louis.Huang]
/*
        if (g_pMsg) ((RtOutputManager*)g_pMsg)->AddArchiveNode(m_pNetLog, false);
        if (g_pLog) ((RtOutputManager*)g_pLog)->AddArchiveNode(m_pNetLog, false);
        if (g_pError) ((RtOutputManager*)g_pError)->AddArchiveNode(m_pNetLog, false);
*/
    }

    m_gws.InitChannel();
    m_gws.Connect();

    if (g_cfgRs.eudemon.enable)
    {
        m_udpEudemonSession.Create(NULL, 0);
        m_eudemonToAddress.SetRemote(g_cfgRs.eudemon.ip.c_str());
        m_eudemonToAddress.SetPort(g_cfgRs.eudemon.port);
        if (!m_eudemonToAddress.BuildAddr())
        {
            m_udpEudemonSession.Close();
            ERR2("Can't build eudemon address, IP=%s, Port=%d\n", g_cfgRs.eudemon.ip.c_str(), g_cfgRs.eudemon.port);
            g_cfgRs.eudemon.enable = false;
        }
    }

	// load all warfield(Zhulu)
	m_pWarZhulu = RT_NEW CWarZhulu();
	m_pWarZhulu->StartUp();

	// load all resource
	if (!RS_Load()) return false;
	if (!SkillInit(R(RES_TABLE_SKILL), R(RES_TABLE_PASSIVE), R(RES_TABLE_EFFECT), R(RES_TABLE_MUTEX))) return false;
	LoadPKConfig(R(INI_PK));
	m_mc->LoadMCConfig(R(INI_MC));//tim.yang MC
	if (!m_mc->m_isRightStartMC)
	{
		ERR("Region: MonCastle.ini LoadFailed! ... \n");
		Exit();
	}
    LOG("Region: Initialize item manager ... \n");
	m_pItemManager = RT_NEW CItemManager();
	if ( !m_pItemManager->Init() )
	{
		ERR("道具初始化出错");
		return false;
	}
	m_defScene->SetTimer(SCENE_TIMER_ITEM, 30*1000);
    LOG("        Done\n");

	LOG("Region: Load scene npc list file... \n");
	if (!CRegionScene::LoadNpcList(R(RES_TABLE_SCENENPCLIST)))
	{
		ERR("Load region npc list failed....\n");
		Exit();
	}
	LOG("        Done\n");

	// load scene file
    LOG("Region: Load scene file ... \n");
	m_defScene->LoadSceneFile("scene01");
    LOG("        Done\n");

    LOG("Region: Load script ... \n");
 	m_defScene->LoadScript("scene01");
	LOG("        Done\n");

    m_defScene->SetTimer(SCENE_TIMER_SPAWN_MONSTER,15*1000);
//---------------add start by tony 05.05.20----------------------------------//
	g_GuarderMgr.Init();
	g_GuarderMgr.LoadCastleWarGuarderTbl(R(RES_CASTLEWAR_GUARDER));
	m_defScene->SetTimer(SCENE_TIMER_GUARDER,15*1000);
	m_defScene->SetTimer(SCENE_TIMER_CASTLEWAR, 1*1000);
	m_defScene->SetTimer(SCENE_TIMER_WARFIELD, 1*1000);
	//g_RegionCastleWarMgr.LoadCastleWarInfo("sv_table/castlewar_info.csv");
	g_RegionCastleWarMgr.Init();
	if(!g_WarfieldMgr.Load(R(RES_WARFIELD)))
	{
		LOG("Load warfield.csv error\n");
	}
	//---------------add end   by tony 05.05.20----------------------------------//
    LOG("        Done\n");

    LOG("Region: OnCreate ... \n");
	m_defScene->OnCreate();
    LOG("        Done\n");

	// g_stateSnapshot.BuildSpawnMap("spawn_map.bmp");
	// g_stateSnapshot.BuildTerrainMap("terrain_map.bmp");
	
	// setup all region timer
	SetTimer(REGION_TIMER_PERFORMANCE_LOG,60*60*1000);
	SetTimer(REGION_TIMER_STATE_REPORT,10*1000);
	SetTimer(REGION_TIMER_UPDATE_FUN,5*1000);
	SetTimer(REGION_TIMER_CHECK_CITY_WAR,5*1000);
	SetTimer(REGION_TIMER_CHECK_DUNGEON,10*1000);
	SetTimer(REGION_TIMER_CHECK_CHEATER,10*1000);
	SetTimer(REGION_TIMER_LINESYS_SPAWN, 15*1000);
	SetTimer(REGION_TIMER_MEM_CHECK, 60*1000);
	//SetTimer(REGION_TIMER_UPDATE_ZHULU,5*1000);

	if(g_cfgRs.eudemon.enable)
		SetTimer(REGION_TIMER_UPDATE_EUDEMON, g_cfgRs.eudemon.update_interval);

	// fb system startup
	m_fbMgr = RT_NEW CRsDungeonMgr;
	if(!m_fbMgr->Init())
	{
		LOG("Region: fb system create error\n");
		return false;
	}

	if(!m_rsLineMan.Startup())
	{
		LOG("Region: Line system create error\n");
		return false;
	}

	if (!LogServerSender_Init())
	{
		ERR("LogServerSender Init FAILED, Ignored!\n");
	}

    long bEnableSend = 1;
    if (GetGameRule()->GetEntry("GameCheck", "EnableSendPicture", &bEnableSend))
    {
        if (bEnableSend == 1)
        {
            g_AntiRobot.EnableSendPicture();
            LOG("SendPicture Is Enabled.\n");
        }
        else if (bEnableSend == 0)
        {
            g_AntiRobot.DisableSendPicture();
            g_AntiRobot.Disable();
            LOG("SendPicture Is Disabled.\n");
        }
    }

    static int iRandomImageCharInfo[3][36] = 
    {
    //  { A, B, C, D, E, F, G, H, I, J, K, L, M,  N,  O,  P,  Q,  R,  S,  T,  U,  V,  W,  X,  Y,  Z,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9 }
        { 9, 8,10,10, 7, 7,11,10, 4, 6, 9, 6,11, 10, 13,  8, 11,  9,  6,  9,  9, 10, 12,  8,  9,  8,  8,  6,  8,  8,  8,  8,  8,  7,  8,  8 },
        { 0, 9,17,27,37,44,51,62,72,76,82,91,97,108,118,131,139,150,159,165,174,183,193,205,213,222,230,238,244,252,260,268,276,284,291,299 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1,  1,  0,  1,  0,  1,  0,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  1,  0,  1,  1,  0,  1 }
    };
    static char szRandomImageCharList[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	//S_MEMDEF(szRandomImageCharList, 37)
	//S_MEMPROTECTOR(szRandomImageCharList, 37, bMP)
	rt2_snprintf(szRandomImageCharList, 37, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
   if (!m_pRandomImgFactory->Init(R(RES_IMG_ALPHABET), 36, iRandomImageCharInfo[1], iRandomImageCharInfo[0], szRandomImageCharList, iRandomImageCharInfo[2]))
    {
		LOG("Region: init random string failed\n");
		g_AntiRobot.Disable();
		//return false;
    }

	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	m_yDay = pTm->tm_yday;
	CRSLogicExt::GetRSLogicExt().InitTaskDesc();
	LOG("Region server startup ok.\n");
	return true;
}

void CRegionServer::Exit()
{
	if(g_bUnWg)
	{
		NES_Finalize_S();
	}
	
	g_factory->DestroyObject(m_defScene);
	SkillClean();
    RS_Clear();
    LogServerSender_Close();
    if (m_pNetLog)
    {
		// C [11/22/2008 Louis.Huang]
/*
        if (g_pMsg) ((RtOutputManager*)g_pMsg)->RemoveArchiveNode(m_pNetLog);
        if (g_pLog) ((RtOutputManager*)g_pLog)->RemoveArchiveNode(m_pNetLog);
        if (g_pError) ((RtOutputManager*)g_pError)->RemoveArchiveNode(m_pNetLog);
*/
        DEL_ONE(m_pNetLog);
        m_pNetLog = NULL;
    }
	if (g_pkConfig)
	{
		DEL_ONE(g_pkConfig);
	}
	m_fbMgr->Exit();
	DEL_ONE(m_fbMgr);
	m_rsLineMan.Shutdown();
	CRegionActManager::GetSingle()->Shutdown();
}

void CRegionServer::OnTimer(ULONG id, DWORD dwParam[])
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	switch(id)
	{
    case REGION_TIMER_CHECK_DUNGEON:
        if (m_fbMgr)
        {
            m_fbMgr->Run();
        }
        break;

	case REGION_TIMER_CHECK_CITY_WAR:
		m_expRate = 1.0f;
		if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi"))
			m_expRate = 1.0f;
		if(g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
			m_expRate = 1.0f;
		break;

	case REGION_TIMER_PERFORMANCE_LOG:	
		// log it
		ERR3("frame = %u, busyFrame=%u, totalTime=%u, sync=%u, process=%u, timer=%u\n",
			m_stat.frameNum,m_stat.busyFrameNum,m_stat.totalTime,m_stat.syncTime,
			m_stat.processTime,m_stat.timerTime);
		ERR1("cheater = %d\n",m_cheaterMap.size());
#ifndef _WIN32
        if (m_bJhWgServer && m_pJhWgServer)
        {
            int iCnt = 0;
		    CRegionUser *pUser;
            EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator itUserMap;
		    for(itUserMap=m_userMap.begin(); itUserMap!=m_userMap.end(); itUserMap++)
		    {
			    pUser = (*itUserMap).second;
                if (pUser->m_iJhWgCheckedCnt)
                {
                    iCnt ++;
                }
		    }
            m_pJhWgServer->UpdateStatus(GetCurrentUserCnt(), iCnt);
        }
#endif
		break;

	case REGION_TIMER_STATE_REPORT: 
		m_stat.lUserCount   = GetCurrentUserCnt();
		m_stat.lObjectCount = (long)g_factory->GetObjectNum();
		//m_stat.sv.UpdateInfo();
		m_gws.UpdateRegionState();
#ifdef _WIN32
        if (m_bJhWgServer && m_pJhWgServer)
        {
            int iCnt = 0;
		    CRegionUser *pUser;
            EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator itUserMap;
		    for(itUserMap=m_userMap.begin(); itUserMap!=m_userMap.end(); itUserMap++)
		    {
			    pUser = (*itUserMap).second;
                if (pUser->m_iJhWgCheckedCnt)
                {
                    iCnt ++;
                }
		    }
            m_pJhWgServer->UpdateStatus(GetCurrentUserCnt(), iCnt);
        }
#endif
		break;

	case REGION_TIMER_UPDATE_EUDEMON:
		UpdateEudemonData();
		break;

	case REGION_TIMER_UPDATE_FUN:
		if(IsFunActive(true))
		{
			if(!IsFunTime(true)) EndFun();
		}
		else
		{
			if(IsFunTime(true)) StartFun(true);
		}
		/*
		if(IsFunActive(false))
		{
			if(!IsFunTime(false)) EndFun();
		}
		else
		{
			// if(IsFunTime(false)) StartFun(false);
		}
		*/
		break;

	case REGION_TIMER_END_FUN:
		{
		CRegionUser *user;
		bool kick;
		int cnt;
		kick = false;
		cnt = 0;
		for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
		{
			user = (*it).second;
			if(IsInFunMap(user))
			{
				KickFunUser(user);
				kick = true;
				if(cnt++ >= 30) break;
			}
		}
		if(!kick) 
			StopTimer(REGION_TIMER_END_FUN);
		break;
		}

	case REGION_TIMER_CHECK_CHEATER:
#ifdef KICK_CHEAT
		if((int)m_cheaterMap.size()>=m_iCheaterValve)
        {
			m_bKickCheater = true;
        }else
        {
			m_bKickCheater = false;
        }
#endif
		break;
	case REGION_TIMER_LINESYS_SPAWN:
		m_rsLineMan.Spawn();
		break;

#ifdef	ENABLE_OPERATOR_NEW
	case REGION_TIMER_MEM_CHECK:
		int syncUsed = rtGetMilliseconds();
		RtCoreMem().CheckAll();
//		RtCoreLog().Debug("MEM_CHECK use %d ms\n", rtGetMilliseconds() - syncUsed);
		break;
#endif
	}
}

// -----------------------------------------------------------------------
// process a server frame
// -----------------------------------------------------------------------
void CRegionServer::Process() 
{	
	m_gws.Process();

	if (m_canAcceptNewLink)
		ProcessNewLink();

	ProcessNetLink();
	ProcessUserLink();
}

// ----------------------------------------------------------------------- 
// try accept new net connection 
// -----------------------------------------------------------------------
void CRegionServer::ProcessNewLink() 
{	
	SNetLink *link;
	CG_TCPSession *session;
	
	// avoid accept too many link in one cycle
	do 
	{
		session = m_clientListen.AcceptNewSession();
		if(!session) break;
		session->SetDebugStr("ClientSession");
		session->EnableRemoteCompress(false);

		if (m_eState!=ssRun)
		{
			DEL_ONE(session);
		}else
		{
			link = RT_NEW SNetLink;
			link->ConnectTime = rtGetMilliseconds();
			link->Session = session;
			link->Id = m_linkId++;
			// LOG1("linkId = %d\n",m_linkId);

			link->Valid = true;
			link->UserId = 0;
			m_unloginLink.push_back(link);

#ifdef _DEBUG
			char ip[128];
			short port;
			session->GetSocket().GetRemoteAddr(ip, &port);
			LOG2("Accept new connection from %s:%d\n", ip, port);
#endif
		}
	} while(session);

}

// ----------------------------------------------------------------------- 
// process unlogin net connection 
// -----------------------------------------------------------------------
void CRegionServer::ProcessNetLink()
{
	short cmdId;
	SNetLink *link;
	CG_TCPSession *session;
	list<SNetLink*>::iterator it;

	DWORD cost = rtGetMilliseconds();	
	for(it=m_unloginLink.begin(); it!=m_unloginLink.end(); )
	{
		link = *it;
		g_region->m_activeLink = link;
		session = link->Session;

		if(!link->Valid || (rtGetMilliseconds()-link->ConnectTime) > (ULONG)g_cfg.listenClient.unlogin_timeout*1000)
		{
			if(link->Valid)
			{
				// session maybe null
				// char ip[128]; short port;
				// session->GetSocket().GetRemoteAddr(ip, &port);
				// LOG2("Remove timeout unlogin %s:%d\n", ip, port);
			}
			else
			{
				MSG("Remove unlogin link\n");
			}		

			if(link->Session)
				DEL_ONE(link->Session);

			DEL_ONE(link);
			it = m_unloginLink.erase(it);
			continue;
		}

		session->DefProc();
		if(session->IsDead())
		{
			link->Valid = false;
			it++;
			continue;
		}

		if(!ProcessLogin(link))
		{
			while(link->Valid && session->GetPacket(m_activeCmd))
			{				
				char* pCmdBuf = m_activeCmd->GetByteData();
				short cmdID;
				memcpy(&cmdID, pCmdBuf, 2);

				if(cmdID==c2r_wg_data)
				{
					if(g_bUnWg)
					{
						if(!m_activeCmd->ReadShort(&cmdId))
							break;

						int size = 1024*20;
						unsigned char buf[1024*20];
						char *pBuf;

						m_activeCmd->ReadBinary(&pBuf, &size);
						if (size>(1024*20-1))
						{
							LOG1("m_activeCmd->ReadBinary(&pBuf, &size) :size[%d]\n", size);
							break;
							size = 1024*20-1;
						}
						memcpy(buf, pBuf, size);

						if(NES_Decrypt_S(-1, buf, size))
						{
							LOG1("NES_Decrypt_S Failed(size[%d])\n", size);
							break;
						}

						m_activeCmd->LoadFromBuffer((char*)buf, size);
					}else
					{
                        if (m_bJhWgCheck)
                        {
						    if(!m_activeCmd->ReadShort(&cmdId))
							    break;

						    int size = 1024*20;
						    unsigned char buf[1024*20];
						    char *pBuf;

						    m_activeCmd->ReadBinary(&pBuf, &size);
						    if (size>(1024*20-1))
						    {
							    LOG1("m_activeCmd->ReadBinary(&pBuf, &size) :size[%d]\n", size);
							    break;
						    }
                            size -= 4;
						    memcpy(buf, pBuf+4, size);

						    m_activeCmd->LoadFromBuffer((char*)buf, size);
                        }else
                        {
						    LOG("[ProcessNetLink]WG UnValid, But Receive c2r_wg_data Packet\n");
                            link->Valid = false;
						    break;
                        }
					}
				} // if(cmdID==c2r_wg_data)

				if(!m_activeCmd->ReadShort(&cmdId)) 
					break;

				if (!m_channelUnlogin.RecvCmd(cmdId,m_activeCmd))
                {
                    link->Valid = false;
                }
			}
		}
		it++;
	}

	cost = rtGetMilliseconds() - cost;
	// LOG1("cost = %d\n",cost);
}

// -----------------------------------------------------------------------
// process user net connection 
// -----------------------------------------------------------------------
int step = 0;
void CRegionServer::ProcessUserLink()
{
	short cmdId;
	CG_TCPSession *session;
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionUser *user;

	vector<ULONG> delList;
	step = 1;
	// delList.reserve(100);
	step = 2;

    time_t tNow = time(NULL);
	ULONG now = rtGetMilliseconds();
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		user = (*it).second;
		m_cmdGiver = user->m_dummy;
		session = user->GetSession();
	
		step = 3;
		if(session)
		{
			session->DefProc();
			if(user->m_netDeadTime == 0)
			{
				if(session->IsDead())
				{
					user->m_netDeadTime = rtGetMilliseconds() + RS_REMOVE_USER_DELAY; // 设置超时
					user->ClearSession();
					continue;
				}
				else if (session->GetState() == NET_STATE_ZERO)
				{
					delList.push_back(user->m_userId);
					continue;
				}
			}
			else 
			{
				if (now > user->m_netDeadTime)
				{
					delList.push_back(user->m_userId);
					continue;
				}
				else if (session->IsDead())
				{
					continue;
				}
			}

			while(session->GetPacket(m_activeCmd)) 
			{
				char* pCmdBuf = m_activeCmd->GetByteData();
				short cmdID;
				memcpy(&cmdID, pCmdBuf, 2);
								
				if(cmdID==c2r_wg_data)
				{
					if(g_bUnWg)
					{
						if(!m_activeCmd->ReadShort(&cmdId))
							break;

						int size = 1024*20;
						unsigned char buf[1024*20];
						char* pBuf;

						m_activeCmd->ReadBinary(&pBuf, &size);
						if (size>(1024*20-1))
						{
							LOG1("m_activeCmd->ReadBinary(&pBuf, &size) :size[%d]\n", size);
							break;
							size = 1024*20-1;
						}
						memcpy(buf, pBuf, size);

						if(NES_Decrypt_S(user->m_userId, buf, size))
						{
							LOG1("NES_Decrypt_S Failed(size[%d])\n", size);
							break;
						}

						m_activeCmd->LoadFromBuffer((char*)buf, size);
					}else
					{
                        if (m_bJhWgCheck)
                        {
						    if(!m_activeCmd->ReadShort(&cmdId))
							    break;

						    int size = 1024*20;
						    unsigned char buf[1024*20];
						    char *pBuf;

						    m_activeCmd->ReadBinary(&pBuf, &size);
						    if (size>(1024*20-1))
						    {
							    LOG1("m_activeCmd->ReadBinary(&pBuf, &size) :size[%d]\n", size);
							    break;
						    }
						    memcpy(buf, pBuf, size);

						    if(!user->wgCheckServer(buf, size, 1024*20))
						    {
							    LOG1("wgCheckServer Failed(size[%d])\n", size);
							    break;
						    }

						    m_activeCmd->LoadFromBuffer((char*)buf, size);
                        }else
                        {
						    LOG("[ProcessUserLink]WG UnValid, But Receive c2r_wg_data Packet\n");
						    break;
                        }
					}
				}

				if(!m_activeCmd->ReadShort(&cmdId)) 
					break;
				
                if(user->m_active)
				{
                    // 如果命令错误，就踢掉玩家
                    if (!m_channelLogin.RecvCmd(cmdId,m_activeCmd))
                    {
                        KickUser(user->m_userId, 0, true, true,WG_INFO_CHECK_COMMAND);
                        break;
					}

#ifdef KICK_CHEAT
                    if (user->m_dummy)
                    {
                        if (user->m_dummy->m_wgChecked) // 如果有检查到使用外挂程序
                        {
                            if (tNow>user->m_dummy->m_wgCheckedTime) // 到了踢除时间
                            {
                                if (user->m_dummy->m_wgChecked==1)
                                {
                                    // 先发送踢出命令
                                    MSG1("WG Send checked to user %s\n", user->m_username.c_str());
                                    user->m_dummy->SendWgCheckedToClient();
                                    user->m_dummy->m_wgCheckedTime = tNow + 10; // 10秒后强制踢除
                                    user->m_dummy->m_wgChecked = 2;
                                }else if (user->m_dummy->m_wgChecked==2)
                                {
                                    MSG1("WG Kick user %s\n", user->m_username.c_str());
                                    user->m_dummy->m_wgCheckedTime = tNow + 10;
                                    KickUser(user->m_userId, 0, true, true,"WG Kick user");
                                }
                            }
                            break;
                        }else if (user->m_dummy->m_wgMaybeChecked>=5)
                        {
                            if (tNow>user->m_dummy->m_wgMaybeCheckedTime) // 到了发送图片时间
                            {
                                // 发送图片
                                if (user->m_ArStatus==ARS_Normal)
                                {
                                    if (!g_AntiRobot.CanIgnoreUser(user))
                                    {
                                        g_AntiRobot.SendRequestToUser(user);
                                    }
#ifdef WIN32
                                    LOG("g_AntiRobot.SendRequestToUser\n");
#endif
                                }
                                user->m_dummy->m_wgMaybeChecked = 0;
                            }
                            break;
                        }
                        if (m_bAutoProcessCheck) // 如果需要自动检查进程
                        {
                            if (user->m_dummy->m_lLastCheckProcessTime)
                            {
                                // 没有收到回应
                                if (tNow>user->m_dummy->m_lLastCheckProcessTime)
                                {
                                    user->m_dummy->m_lLastCheckProcessTime = 0; // 目前先做重新设置处理

                                    // 这里可以认为玩家使用外挂，因为发出检查命令100秒没有返回
                                    user->m_dummy->PossiblyUsingWG(5);
                                }
                            }else
                            {
                                if (user->m_dummy->m_tNextCheckProcess==0)
                                {
                                    user->m_dummy->m_tNextCheckProcess = tNow+rtRandom(m_iMaxCheckTime, m_iMinCheckTime);
                                }else
                                {
                                    // 到了检查时间
                                    if (tNow>user->m_dummy->m_tNextCheckProcess)
                                    {
                                        user->m_dummy->m_tNextCheckProcess = 0;

                                        g_sendCmd->BeginWrite();
	                                    g_sendCmd->WriteShort(r2c_event);
	                                    g_sendCmd->WriteLong(g_region->m_time);
	                                    g_sendCmd->WriteShort(EVENT_CHECK_PROCESS);
	                                    g_sendCmd->WriteLong(1);
	                                    g_sendCmd->WriteString((char*)(m_listWgProcesses[rand()%(int)m_listWgProcesses.size()].c_str()));
                                        user->m_dummy->m_lLastCheckProcessTime = (long)tNow+100;
		                                user->RecvCmd(g_sendCmd);
#ifdef WIN32
                                        LOG("Send EVENT_CHECK_PROCESS\n");
#endif
                                    }
                                }
                            }
                        }
                    }
#endif // KICK_CHEAT
                }
			}
		}
		else if (now > user->m_netDeadTime)
		{
			step = 4;
			delList.push_back(user->m_userId);
		}
	}

	step = 5;

	// real erase user
	for(size_t i=0; i<delList.size(); i++)
	{
		EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator iter = m_userMap.find(delList[i]);
		if (iter == m_userMap.end())
			continue;

		MSG2("Remove User [ Name=%s UserID=%d ]\n",user->m_username.c_str(), user->m_userId);
		CRegionUser* user = iter->second;
		OnUserQuit(user);
		user->ClearSession();

		if(user->m_dummy)
		{
			if (user->m_dummy->vSuitSkill.size() != 0)
			{
				std::vector<int>::iterator it = user->m_dummy->vSuitSkill.begin(); 
				for (;it!=user->m_dummy->vSuitSkill.end();it++)
				{
					user->m_dummy->DeleteSkill((*it));
				}
			}
			if (user->m_dummy->GetPlayerLogic() && user->m_dummy->GetPlayerLogic()->GetLiveType() != CPlayerLogic::eLive)
				user->m_dummy->m_IsDead = 1;
				/*user->m_dummy->Rebirth(3);*/
			if(user->m_dummy->m_pet)
				user->m_dummy->CallbackPet(true,false);
			user->m_dummy->RemoveWardNPC();
			if(user->m_dummy->m_hero)
				user->m_dummy->CallbackHero();
            user->m_dummy->m_Skill.ShutdownSkill();
            if(user->m_bSaveData)
				m_gws.SaveUserData(iter->first, user->m_dummy);
    		if(user->m_bLogout)
				m_gws.Logout(iter->first, true);
			user->m_dummy->Quit();
			user->m_dummy = NULL;
		}

		DEL_ONE(user);
		m_userMap.erase(iter);
	}

    if (m_lKickAllUserTime)
    {
        DWORD dwNowM = rtGetMilliseconds();
        DWORD dwDiffM = dwNowM-m_dwKickAllUserLastTickTime;
        if (dwDiffM>500)
        {
            if (m_dwKickAllUserLastTickTime!=0)
            {
                if ((DWORD)m_lKickAllUserTime<dwDiffM)
                {
                    m_lKickAllUserTime = 0;
                }else
                {
                    m_lKickAllUserTime -= dwDiffM;
                }
            }
            m_dwKickAllUserLastTickTime = dwNowM;

            int iUserCount = (int)m_userMap.size();
            int iTickCount = m_lKickAllUserTime/500; // 每500秒一次
            int i, iKickUser;
            if (m_lKickAllUserTime==0 || iTickCount==0)
            {
                iKickUser = iUserCount;
            }else
            {
                iKickUser = (iUserCount / iTickCount)+1;
            }

            i = 0;
	        for(it=m_userMap.begin(); it!=m_userMap.end() && i<iKickUser; it++)
	        {
		        user = (*it).second;
				if (KickUser(user->m_userId, 4, true, true,(char*) R(MSG_PROCESS_USER_LINK_SERVER_SERVICE)))
                {
                    i ++;
                }
            }

            if (i)
            {
                ERR2("    KillAllUser: Kick %d user at %d\n", i, rtTimestamp());
            }
        }
    }
}

// -----------------------------------------------------------------------
// create a new game user,use current net link
// -----------------------------------------------------------------------
bool CRegionServer::CreateUser(SNetLink *link, SUserData &data, CG_CmdPacket *cmd)
{
    MSG3("Create %s user [ name=%s, uid = %d]\n", (data.cSwitchRegion?"SwitchRegion":"New"),data.name.c_str(), data.userID);

	// create new user
	CRegionCreature *cre;
	cre = (CRegionCreature*)g_factory->CreateObject(OBJECT_TYPE_CREATURE);
	if(!cre)
		return false;
	cre->GetPlayerLogic()->SetTeamFree(data.bTeamFree);
	EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator oldUser = m_userMap.find(link->UserId);
	if (oldUser != m_userMap.end())
	{
		LOG1("New user [%s] comming, but there is old user exist in this server\n", oldUser->second->m_username.c_str());
		if (oldUser->second->m_dummy)
			oldUser->second->m_dummy->Quit();

		oldUser->second->ClearSession();

		DEL_ONE(oldUser->second);
		m_userMap.erase(oldUser);
	}

	CRegionUser *user   = RT_NEW CRegionUser;
	user->LinkDummy(cre);
	cre->m_core.SetIHpMpChange(cre->GetPlayerLogic()->GetCNewPlayerGuide());
	
	
	user->m_connectTime = link->ConnectTime;
    user->m_accountName = link->Username;
	user->SetSession(link->Session);
	user->m_lanCode		= link->Code;
	user->m_userId      = link->UserId;
	user->m_netDeadTime = 0;
	user->m_updateTimeStamp = time(NULL);
    user->m_active      = true;
	user->m_bSaveData   = true;

	//
	// load creature data
	//
    cre->Load(data);
	cre->m_pItemContainer->m_Bag.SetIOnItemChange(cre->GetPlayerLogic());
	cre->m_pItemContainer->m_MissionBag.SetIOnItemChange(cre->GetPlayerLogic());
	cre->m_pFriend->mConnection.SetIOnConnectionChange(cre->GetPlayerLogic());
	short peerPort;
	char  peerAddr[256];
	if(link->Session->GetSocket().GetRemoteAddr(peerAddr, &peerPort, NULL))
	{
		user->m_userIpAddress = peerAddr;
	}

	// server init msg

	char	addr[16];
	short	port;
	unsigned long	ip;
	int		length;
	char	binBuf[1024 * 20];

	if(g_bUnWg)
	{
		link->Session->m_socket.GetRemoteAddr(addr, &port, &ip);
		if(NES_UserLogin_S(user->m_userId, 
						(char*)data.account.c_str(),
						ip,
						(unsigned char*)binBuf, 
						length))
		{
			g_factory->DestroyObject(cre);
			m_gws.Logout(data.userID, true);
			LOG("NES_UserLogin_S Failed\n");
			return false;
			//CHECK(0);
		}else
		{
#ifdef WIN32
			LOG("NES_UserLogin_S Success\n");
#endif
		}
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_init);
	g_sendCmd->WriteShort(g_cfg.listenClient.r2c_interval);
	g_sendCmd->WriteShort(g_cfg.listenClient.c2r_interval);
	g_sendCmd->WriteShort(g_cfg.listenClient.cl_pre_buffer);
	g_sendCmd->WriteShort(g_cfg.listenClient.cl_max_buffer);
	g_sendCmd->WriteShort((short)g_bUnWg);
	if(g_bUnWg)
	{
		g_sendCmd->WriteBinary(binBuf, length);
	}

	g_sendCmd->WriteByte(m_bJhWgCheck);
    if (m_bJhWgCheck)
    {
        user->m_lClientLUASeed = rand()%300;
        g_sendCmd->WriteLong(user->m_lClientLUASeed);
    }

	user->RecvCmd(g_sendCmd);

	// check in world map or fb map
	if(data.IsInWarfield) //检查是否在国战场景
	{
        if (((time(NULL)-data.lastUpdateTime)<5*60) && g_WarfieldMgr.m_bIsOpen)
        {
		    if(!g_WarfieldMgr._AddUserToNWar(cre->m_userInfo, 0, cre->m_pos[0], cre->m_pos[1]))
		    {
			    LOG("Cannot into warfield\n");
			    g_factory->DestroyObject(cre);
			    m_gws.Logout(data.userID, true);
			    return false;
		    }
        }else
        {
			long defpos[3];
			if(cre->m_core.Faction == FACTION_ZHOU)
			{
				defpos[0] = 28006;
				defpos[1] = -7100;

			}
			else
			{
				defpos[0] = 27400;
				defpos[1] = -7100;
			}
			if(!g_region->m_defScene->AddCreature(cre,defpos,0))
			{
				LOG("Region: Add user to scene error!\n");
				// cre->Rebirth();
				g_factory->DestroyObject(cre);
				m_gws.Logout(data.userID, true);
				return false;
			}
        }
	}else if(data.LineSYSGroupID)//rendh 2010.01.21
	{
		if (!g_region->m_rsLineMan.EnterLine(data.LineSYSGroupID, data.LineSYSLineID, g_cfgRs.lRegionID, cre->m_userInfo, cre->m_pos))
		{
			ERR("Region: Add user to line error!\n");
			// cre->Rebirth();
			g_factory->DestroyObject(cre);
			m_gws.Logout(data.userID, true);
			return false;
		}

		CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineRes(data.LineSYSGroupID, data.LineSYSLineID, data.userID, user);

		CLineSysCmdBuilder_R2C::Build_r2c_linesys_EnterLineNotify(data.LineSYSGroupID, data.LineSYSLineID, data.userID, 0);
		cre->RecvCmd(g_sendCmd);

		//rendh 2010.01.21
	}else if(data.dungeonUID==0)
    {
        cre->m_dungeonLastQuit = data.dungeonLastQuit;
        cre->m_dungeonUID = 0;
	    if(!g_region->m_defScene->AddCreature(cre,cre->m_pos,0))
	    {
			long defpos[3];
			//SFuBen* PFuBen = g_TableFB.FindPos(g_region->m_defScene->m_sceneName);

			//if (PFuBen)
			//{
			//	defpos[0] = PFuBen->exitPos[0];
			//	defpos[1] = PFuBen->exitPos[1];
			//	defpos[2] = PFuBen->exitPos[2];
			//}
			//else
			//{
			//	if(cre->m_core.Faction == FACTION_NEUTRAL)
			//	{
			//		defpos[0] = 3090;
			//		defpos[1] = 2690;
			//	}
			//}

			{
				//放到蜀郡中间去
				defpos[0] = 4000;
				defpos[1] = 12000;
			}
			

			if(!g_region->m_defScene->AddCreature(cre,defpos,0))
			{
				LOG("Region: Add user to scene error!\n");
				// cre->Rebirth();
				g_factory->DestroyObject(cre);
				m_gws.Logout(data.userID, true);
				return false;
			}
	    }
    }else
	{
        if (!m_fbMgr->_AddUserToDungeon(data.dungeonUID, cre->m_userInfo, 0, cre->m_pos[0], cre->m_pos[1]))
        {
            // cre->Rebirth();
		    g_factory->DestroyObject(cre);
            m_gws.Logout(data.userID, true);
		    return false;
        }
    }

    m_userMap[user->m_userId] = user;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_login_ret);
    g_sendCmd->WriteLong(0);
    cre->RecvCmd(g_sendCmd);

 	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_create_char);
	g_sendCmd->WriteLong(user->m_userId);
	g_sendCmd->WriteByte(cre->m_core.Metier);
	g_sendCmd->WriteByte(cre->m_posFlag);
	g_sendCmd->WriteString((char*)user->m_username.c_str());
	g_sendCmd->WriteShort(cre->m_core.HeadModelId);
    g_sendCmd->WriteByte(data.cSwitchRegion);
	cre->WriteCreateInfo(g_sendCmd);
	
	cre->RecvCmd(g_sendCmd);
	// cre->SetTimer(CRE_TIMER_UPDATE_SHOW_INFO,1000);
	cre->SetTimer(CRE_TIMER_HEARTBEAT,HEARTBEAT_INTERVAL);
	cre->SetTimer(CRE_TIMER_MONITOR, PLAYER_MONITOR_CYCLE * 1000);
	cre->SetTimer(CRE_TIMER_PRENTICE, 1800*1000);
	cre->SetTimer(CRE_TIMER_CHECK_CARD, CHECK_CARD_INTERVAL);
    cre->SetTimer(CRE_TIMER_CHECK_ITEM_VALID, 60*1000);
	cre->SetTimer(CRE_TIMER_HPANDMP, 3*1000);
	cre->SetTimer(CRE_TIMER_ENP, 10*1000);
	cre->SetTimer(CRE_TIMER_TIME, 5*60*1000); //每5分钟更新一次
	cre->SetTimer(CRE_TIMER_STGTTIME, 1000); //每1秒更新一次
	if (cre->m_bTime)

	{
		cre->SetTimer(CRE_TIMER_TIME2, 1000);
	}
	// check fb task
	cre->FbCheckAllTask(false);

	//新创建人物加载技能
	if (cre->m_IsFirstLogin == 0)
	{
		CActorSkill* pActorSkill =  FindActorSkill(cre,cre->m_oId);
		if (pActorSkill)
		{
			char Str[8] = {111,112,113,161,106,107,76,114};

			for (int i = 0; i<8; i++)
			{
				pActorSkill->AddSkill(Str[i],0,true);
			}
			cre->m_IsFirstLogin = 1;
		}	
	}


	// update all info to client
    cre->UpdateMatchToClient();
	cre->UpdateToClient(NET_UPDATE_ALL);
	cre->UpdateToClient(NET_CRE_UPDATE_TASK);
	cre->UpdateToClient(NET_CRE_UPDATE_FB_TASK);

	if (!data.cSwitchRegion)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_shortcut);
		g_sendCmd->WriteString((char*)user->m_dummy->m_shortcut.c_str());
		cre->RecvCmd(g_sendCmd);

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_load_config);
		g_sendCmd->WriteString((char*)user->m_dummy->m_configData.c_str());
		cre->RecvCmd(g_sendCmd);

		// 发送采邑数据
		CFeodCmdBuilder_Svr::Build_r2c_feod_data();
		cre->RecvCmd(g_sendCmd);

		cre->NetSend_State();

		cre->m_pFriend->OnOnline();
	}

	// show wellcome message
    if (!data.cSwitchRegion)
    {
        //g_AntiRobot.InitAntiRobotData(cre->m_userInfo);
		g_AntiRobot.SendRequestToUser(cre->m_userInfo, false, ARS_Asking_2);

    }

	//-------------------add start by tony 05.07.21--------------------//

	if(data.cSwitchRegion)
	{
		g_RegionCastleWarMgr.OnUserOnline(cre);
	}

	//-------------------add end   by tony 05.07.21--------------------//

	//上线	
	OnUserEnter(user);

	//3.30, LeiJun添加创建用户时加载用户脚本并"OnCreate"
	cre->LoadScript("User");
	if(cre->IsScriptLoaded())
		cre->CallScript("OnCreate", false);

	/*
    // 如果国战已经关闭，就要到默认退出点
	if (data.IsInWarfield)
    {
        if (!g_WarfieldMgr.m_bIsOpen)
        {
			long pos[2];
            int iRx, iRy;
            if(FACTION_ZHOU == data.attr.faction)
			{
                pos[0] = g_WarfieldMgr.m_pWarfield->stInfo.exitPosZhou[0];
				pos[1] = g_WarfieldMgr.m_pWarfield->stInfo.exitPosZhou[1];
			}else
			{
				pos[0] = g_WarfieldMgr.m_pWarfield->stInfo.exitPosShang[0];
				pos[1] = g_WarfieldMgr.m_pWarfield->stInfo.exitPosShang[1];
			}

            cre->m_bIsInWarfield = false;
		    CG_CmdPacket& packet = m_gws.BeginSend();
		    packet.WriteShort(r2g_move_to);
		    packet.WriteLong(cre->m_userInfo->m_userId);
		    g_region->m_defScene->m_pTerrain->GetTerrainByPosFast(pos[0], pos[1], iRx, iRy);
		    iRx += rand() % 40 - 20;
		    iRy += rand() % 40 - 20;
		    packet.WriteLong((long)iRx);
		    packet.WriteLong((long)iRy);
		    m_gws.EndSend();
        }
    }
	*/


	cre->GetPlayerLogic()->SetOnLineTime(data.onlineTime);
	cre->GetPlayerLogic()->GetCNewPlayerGuide()->HitMove();

	if (cre->IsChengBody())
	{
		//SCreModel *model = g_TableCreModel.FindCreModel(cre->m_ChangeModelID);
		//if(!model) return false;
		//cre->m_dwLastModelID = cre->m_core.ModelId;
		//cre->m_model = model;
		//cre->m_core.ModelId = model->Id;
		//cre->m_core.ComplexModel = model->bComplex;
		//cre->SendChangeBosyToC(cre->m_ChangeModelID);
		if (cre->SetModel(cre->m_ChangeModelID))
		{
			cre->SendChangeBosyToC(cre->m_ChangeModelID);
		}
	}

	return true;
}

// not used now
CRegionCreature *CRegionServer::CreatePet(short Id,int lev)
{
	return NULL;
}

CRegionCreature *CRegionServer::CreateSpirit(short Id,bool cheat/*=false*/)
{
	int i;
	SNpc *info = g_TableNpc.FindNpc(Id);
	if(!info)
	{
		ERR1("CRegionServer::CreateNpc: unknown npc id %d\n", Id);
		return NULL;
	}
	char obType = OBJECT_TYPE_SPIRIT;

	CRegionCreature *spirit;
	spirit = (CRegionCreature*)g_factory->CreateObject(obType,cheat);
	if(!spirit)
		return NULL;

	if(!spirit->Init(info->TypeParam.c_str()))
	{
		return NULL;
	}

	if(!spirit->SetModel(info->ModelId))
	{
		g_factory->DestroyObject(spirit);
		return NULL;
	}

	// set spirit data
	spirit->m_npcId = Id;
	spirit->m_core.Faction = info->Faction;
	spirit->m_core.Name = info->Name;
	spirit->m_core.Lev = info->Level;
	spirit->m_core.MaxHp.Base = info->Hp;
	spirit->m_core.MaxMp.Base = info->Mp;
	spirit->m_core.SetMp(info->Mp);
	spirit->m_core.SetHp(info->Hp);
	spirit->m_core.Attack.Base = info->Attack;
	spirit->m_core.DamageMax.Base = 0;
	spirit->m_core.DamageMin.Base = 0;
	spirit->m_core.Dodge.Base = info->Dodge;
	spirit->m_core.Armor.Base = info->Armor;
	spirit->m_core.AttSpeed.Base = info->AttackSpeed * ATT_SPEED_SCALE;
	spirit->m_patrolRange = info->PatrolRange;	
	spirit->m_traceRange  = info->ValidRange;
	spirit->m_viewRange   = 0;
	spirit->m_attackRange = 0;
	spirit->m_assistRange = 0;
	spirit->m_moveSpeed   = info->MoveSpeed;
	spirit->m_core.MovSpeed.Base = info->MoveSpeed;
	spirit->m_core.AttackRange.Base = 0;

	for(i=0; i<MAX_ELEMENT_ATTR; i++)
		spirit->m_core.ElementArmor[i].Base = info->ElementArmor[i];

	spirit->m_npcInfo = info;
	spirit->m_killReward.Exp = info->Exp;
	spirit->m_killReward.lSP = info->lSP;
	spirit->m_killReward.Gold = info->Gold;
	spirit->m_killReward.GoldRate = info->GoldRate;
	if (!spirit->m_npcInfo->AIType.empty())
	{
		spirit->SetAI(spirit->m_npcInfo->AIType.c_str());
	}
	spirit->SetTimer(CRE_TIMER_HEARTBEAT,HEARTBEAT_INTERVAL);

	return spirit;
}

CRegionCreature *CRegionServer::CreateNpc(short Id,bool cheat)
{
    int i;
	SNpc *info = g_TableNpc.FindNpc(Id);
	if(!info)
    {
        ERR1("CRegionServer::CreateNpc: unknown npc id %d\n", Id);
        return NULL;
    }
	char obType = 0;

	if(info->Type==1 || info->Type==4 || info->Type==5 || info->Type==9) // 怪物，草药，矿物
		obType = OBJECT_TYPE_CREATURE;	 
	else if(info->Type == 2)
		obType = OBJECT_TYPE_NPC_SERVICE; 
//--------------------------add start by tony 05.05.18-----------------------//
	else if(info->Type == 3)
		obType = OBJECT_TYPE_NPC_COMBATIVE; 
	//--------------------------add end   by tony 05.05.18-----------------------//
	else if(info->Type==6 || info->Type==7 || info->Type==8)
		obType = OBJECT_TYPE_NPC_SERVICE;
	else
		return NULL;

	CRegionCreature *npc;
	npc = (CRegionCreature*)g_factory->CreateObject(obType,cheat);
	if(!npc)
		return NULL;
	//tim.yang
	if (722 == Id || 723 == Id)
	{
		m_mc->RestoreTargetObjId(npc->m_oId,Id);
	}
	//end
	if(!npc->Init(info->TypeParam.c_str()))
	{
		return false;
	}

	if(!npc->SetModel(info->ModelId))
	{
		g_factory->DestroyObject(npc);
		return NULL;
	}
    if (obType==OBJECT_TYPE_NPC_SERVICE)
    {
        m_serviceNpcList.push_back(npc->m_oId);
    }

	// set npc data
	npc->m_npcId = Id;
	npc->m_core.Faction = info->Faction;
	npc->m_core.Name = info->Name;
	npc->m_core.Lev = info->Level;
	npc->m_core.MaxHp.Base = info->Hp;
	npc->m_core.MaxMp.Base = info->Mp;
	npc->m_core.SetMp(info->Mp);
	npc->m_core.SetHp(info->Hp);
	npc->m_core.Attack.Base = info->Attack;
	npc->m_core.DamageMax.Base = info->Damage*1.1;
	npc->m_core.DamageMin.Base = info->Damage*0.9;
	npc->m_core.Dodge.Base = info->Dodge;
	npc->m_core.Armor.Base = info->Armor;
	npc->m_core.AttSpeed.Base = info->AttackSpeed * ATT_SPEED_SCALE;
	npc->m_patrolRange = info->PatrolRange;	
	npc->m_traceRange  = info->ValidRange;
	npc->m_viewRange   = info->ViewRange;
	npc->m_attackRange = info->AttackRange;
	npc->m_assistRange = info->AssistRange;
	npc->m_moveSpeed   = info->MoveSpeed;
	npc->m_core.MovSpeed.Base = info->MoveSpeed;
	npc->m_core.AttackRange.Base = info->AttackRange;

	for(i=0; i<MAX_ELEMENT_ATTR; i++)
        npc->m_core.ElementArmor[i].Base = info->ElementArmor[i];

	npc->m_npcInfo = info;
	npc->m_npcInfo->AddEventCommand("delself", "killself", "1000");
	npc->m_killReward.Exp = info->Exp;
    npc->m_killReward.lSP = info->lSP;
	npc->m_killReward.Gold = info->Gold;
	npc->m_killReward.GoldRate = info->GoldRate;
	for(int i=0; i<NPC_MAX_SKILL; ++i)
		npc->m_skillAttackRate[i] = info->SkillAttackRate[i];
	bool bHaveSkill = false;
    for (i=0; i<NPC_MAX_SKILL; i++)
    {
        if(info->sSkills[i])
        {
            npc->m_Skill.AddSkill(info->sSkills[i], 0, false);
			bHaveSkill = true;
        }else
        {
            break;
        }
    }
	if (bHaveSkill)
		npc->m_Skill.StartupSkill(); // 如果有添加到技能就启动技能

	// if sell items, add items
	if (!npc->m_npcInfo->SellItems.empty())
	{
		char *pStrItems = RT_NEW char[npc->m_npcInfo->SellItems.size() +1];
		rt2_strcpy(pStrItems, npc->m_npcInfo->SellItems.c_str());

		npc->m_pSellBag = RT_NEW CBag(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, m_pItemManager);
		SItemID item;
		const char sep[] = ",;";	// 分隔符
		char *k = strtok(pStrItems, sep);
		while (k)
		{
			int n = atoi(k);
			item = g_region->m_pItemManager->CreateItem(n);
			//if (ItemCanPile(item))
			//{
			//	SItemBase* pItemClass = g_region->m_pItemManager->GetItem(item.type);
			//	CHECK(pItemClass);
			//	item.count = pItemClass->maxCount;
			//}
			npc->m_pSellBag->AddItem(item);

			k = strtok(NULL, sep);
		}
		delete[] pStrItems;
	}

    if (!npc->m_npcInfo->SellRareItems.empty())
    {
        char *pStrRareItems = RT_NEW char[npc->m_npcInfo->SellRareItems.size() +1];
        rt2_strcpy(pStrRareItems, npc->m_npcInfo->SellRareItems.c_str());

        const char sep1[] = ";";
        std::vector<std::string> arrStringRare;
        char* k = strtok(pStrRareItems, sep1);
        while (k)
        {
            arrStringRare.push_back(k);
            k = strtok(NULL ,sep1);
        }
        delete[] pStrRareItems;
        int tmpIdx = 0;
        for (tmpIdx=0; tmpIdx<(int)arrStringRare.size(); tmpIdx++)
        {
            SNpcSellRareItem sri;
            if (!sri.Load(arrStringRare[tmpIdx]))
            {
                ERR("[CRegionServer::CreateNpc] 出售稀有物品 出错 (Load)\n");
                return false;
            }
            if (!sri.Create(g_region->m_pItemManager))
            {
                ERR("[CRegionServer::CreateNpc] 出售稀有物品 出错 (Create)\n");
                return false;
            }
            npc->m_NpcSellRareItem.push_back(sri);
        }
    }
    if (!npc->m_NpcSellRareItem.empty())
    {
        npc->StartRareItemSell();
    }

	// if have AI, add AI
	if (!npc->m_npcInfo->AIType.empty())
	{
		npc->SetAI(npc->m_npcInfo->AIType.c_str());
	}

	// load task
	for(i=0; i<(int)info->GetTaskId.size(); i++)
	{
		STaskDesc *desc = g_TableTask.FindTask(info->GetTaskId[i]);
		if(desc)
			npc->m_acceptTask.push_back(desc);
		else
			LOG2("npc = %d,can't find task [%d]\n",Id,info->GetTaskId[i]);
	}

	for(i=0; i<(int)info->FinishTaskId.size(); i++)
	{
		STaskDesc *desc = g_TableTask.FindTask(info->FinishTaskId[i]);
		if(desc)
			npc->m_finishTask.push_back(desc);
		else
			LOG2("npc = %d,can't find task [%d]\n",Id,info->FinishTaskId[i]);
	}

	// load script
	if(info->Script.size() != 0)
	{
		npc->LoadScript(info->Script.c_str());
	}
	npc->SetTimer(CRE_TIMER_HEARTBEAT,HEARTBEAT_INTERVAL);

	// npc->SetAI("normal");
	if(cheat)
	{
		npc->m_cheatDummy = true;
		npc->m_viewRange = 0;
		for(int i=0; i<NPC_MAX_SKILL; ++i)
			npc->m_skillAttackRate[i] = 0;
		npc->m_core.AttackRange.Base = 0;
		npc->m_core.AttSpeed.Base = 1;
		npc->m_core.AddNormalAttack();
	}
	
	//PZH
	npc->GetPlayerLogic()->SetForTaskType((CPlayerLogic::ForTaskType)(info->cTaskType));
	//

	return npc;
}

CRegionUser *CRegionServer::FindUser(ULONG UserId, bool forceAll)
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	it = m_userMap.find(UserId);
	if(it==m_userMap.end())
		return NULL;
	
	CRegionUser *user = it->second;
	if(!user->m_active && !forceAll)
		return NULL;
	return user;
}

CRegionUser *CRegionServer::FindUser(const char *Name, bool forceAll)
{
	EXT_SPACE::unordered_map<DWORD,CRegionUser*>::iterator it;
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		CRegionUser *user = (*it).second;
		if(user->m_dummy->m_core.Name == Name)
		{
			if(!user->m_active && !forceAll)
				return NULL;
			return user;
		}
	}
	return NULL;
}

bool CRegionServer::KickUser(DWORD dbId, char cCause, bool saveData, bool logout, char const* szCause)
{
	CRegionUser* user = FindUser(dbId,true);
	if(!user)
	{
		// user already offline
        ERR("CRegionServer::KickUser: Cannot found user DBID=%d\n", dbId);
		m_gws.Logout(dbId);
		return true;
	}

	if(!user->m_active)
	{
		return false;
	}

	ERR("Kick user [ Name=%s UserID=%d Reason=%s ]\n", user->m_username.c_str(), dbId, szCause);	

	if(user->Session() && cCause)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_kick);
		g_sendCmd->WriteByte(cCause);
        if (cCause==4) // KICK_USER_USERDEFINE
        {
            if (szCause==NULL)
				g_sendCmd->WriteString("");
			else
			    g_sendCmd->WriteString(szCause);
        }
		user->RecvCmd(g_sendCmd);
	}

    if (cCause)
    {
        user->m_netDeadTime = rtGetMilliseconds() + 500; // 500毫秒以后踢除
    }else
    {
        user->m_netDeadTime = rtGetMilliseconds() - 2000; // 立刻踢除
    }
    user->m_active = false;
	user->m_bSaveData = saveData;
	user->m_bLogout = logout;

	return true;
}

// -----------------------------------------------------------------------
// send sync msg to client 
// -----------------------------------------------------------------------
void CRegionServer::SyncToClient()
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionCreature  *pCreature;
	CRegionUser		 *pUser;
    time_t tNow = time(NULL);
	
	unsigned long dwTime = rtGetMilliseconds();
    time_t tKeepOnlineNow = tNow-(g_cfg.gws.update_user_interval*2); // 存盘的2倍时间
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		pUser = (*it).second;
		if(!pUser->Session()) continue;
		
		pCreature = pUser->m_dummy;
		if(!pCreature) 
		{
			LOG1("region: user no dummy,name = %s\n",pUser->m_username.c_str());
			continue;
		}

		if(!pCreature->m_scene)
		{
			LOG1("region: user no scene,name = %s\n",pUser->m_username.c_str());
			continue;
		}

		pCreature->GetPlayerLogic()->OnRun(dwTime);
        // 每隔一段时间就进行一次保持在线的通知
        if (tKeepOnlineNow>pUser->m_lastKeepOnlineTime)
        {
            pUser->m_lastKeepOnlineTime = tNow;

            CG_CmdPacket& cmd = m_gws.BeginSend();
            cmd.WriteShort(r2g_keep_online);
            cmd.WriteLong(pUser->m_userId);
            m_gws.EndSend();
        }
	
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_snapshot);
		g_sendCmd->WriteLong(m_sec);
		g_sendCmd->WriteLong(m_time);
		if(!pCreature->m_scene->BuildSnapshot(g_sendCmd, pCreature))
		{
			LOG1("region: build snapshot failed,name = %s",pUser->m_username.c_str());
			continue;
		}
		pUser->RecvCmd(g_sendCmd);
	}
}

void CRegionServer::ShowPingTime()
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	
	LOG("region: ping time list\n");
	for(it=m_userMap.begin(); it!=m_userMap.end();it++)
	{
		CRegionUser *user = (*it).second;
		user->KeepAlive(rtGetMilliseconds());
		//LOG2("region: name = %s, ping = %d",user->m_username.c_str(),session->GetRtt());
	}
}

void CRegionServer::RepCreatureToClient(CRegionCreature *cre,char type)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_update_object_info);
	g_sendCmd->WriteLong(cre->m_oId);
	g_sendCmd->WriteByte(type);
	cre->RepUpdate(type,g_sendCmd);
	cre->RecvCmd(g_sendCmd);
}

bool CRegionServer::BuildEventDead(CG_CmdPacket *cmd,CRegionCreature *cre)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_event);
	cmd->WriteLong(g_region->m_time);
	cmd->WriteShort(EVENT_DEAD);
	cmd->WriteLong(cre->m_oId);
	long lDead = cre->GetPlayerLogic()->GetLiveType();
	cmd->SerializeBit(lDead,3);
	cre->m_scene->BroadcastCmd(cre->m_pos,DEFAULT_RANGE,cmd,NULL);
    return true;
}

bool CRegionServer::BuildEventCulLevelUp(CG_CmdPacket *cmd,CRegionCreature *cre)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_event);
	cmd->WriteLong(g_region->m_time);
	cmd->WriteShort(EVENT_CULLEVEL_UP);
	cmd->WriteLong(cre->m_oId);
	return true;
}

bool CRegionServer::BuildEventLevelUp(CG_CmdPacket *cmd,CRegionCreature *cre)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_event);
	cmd->WriteLong(g_region->m_time);
	cmd->WriteShort(EVENT_LEVEL_UP);
	cmd->WriteLong(cre->m_oId);
	return true;
}

bool CRegionServer::BuildEventPlayPose(CG_CmdPacket *cmd, CRegionCreature *cre,short poseId, char dir)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_event);
	cmd->WriteLong(g_region->m_time);
	cmd->WriteShort(EVENT_PLAY_POSE);
	cmd->WriteLong(cre->m_oId);
	cmd->WriteShort(poseId);
	cmd->WriteByte(dir);
	return true;
}

bool CRegionServer::BuildEventBorn(CG_CmdPacket *cmd,CRegionCreature *cre)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_event);
	cmd->WriteLong(g_region->m_time);
	cmd->WriteShort(EVENT_BORN);
	cmd->WriteLong(cre->m_oId);
	return true;
}

bool CRegionServer::BuildEventStaticDialog(CG_CmdPacket *cmd,char *name)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_event);
	cmd->WriteLong(g_region->m_time);
	cmd->WriteShort(EVENT_STATIC_DIALOG);
	cmd->WriteString(name);
	return true;
}

bool CRegionServer::BuildEventSystemMsg(CG_CmdPacket *cmd, long msgID)
{
	cmd->BeginWrite();
	cmd->WriteShort(r2c_system_message);
	cmd->WriteLong(msgID);
	return true;
}

void CRegionServer::ShowMessageBox(CRegionCreature *pUser, const char *pMessage, const char *pCommandStr, long iButton)
{
	if(!pUser || !pMessage || !pCommandStr)
		return;
	if(iButton <= 0 || iButton >= 3)
		return;

	//char pHtmlMsg[512] = {0};
	//rt2_sprintf(pHtmlMsg, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\"/></head><body><p>    %s</p></body></html>", pMessage);
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_message_dialog);
	g_sendCmd->WriteShort((short)iButton);
	g_sendCmd->WriteString((char*)pMessage);
	g_sendCmd->WriteString((char*)pCommandStr);
	pUser->RecvCmd(g_sendCmd);
}

void CRegionServer::ShowHtmlDialog(CRegionCreature *src,CRegionCreature *tar,const char *link,
								   const char* szName, const char* szValue/*PZH*/, char cListType/**/,
								   short nTaskID, char cPangeType, long lShowTime)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_dialog);
	if(src)
    {
		g_sendCmd->WriteLong(src->GetObjectId());
    }else
    {
		g_sendCmd->WriteLong(0);
    }
	g_sendCmd->WriteShort(DIALOG_HTML);
	g_sendCmd->WriteString((char*)link);
	g_sendCmd->WriteLong(nTaskID);//PZH
	g_sendCmd->WriteByte(cPangeType);
	
    if (szName)
    {
	    g_sendCmd->WriteString((char*)szName);
	    g_sendCmd->WriteString((char*)szValue);
    }
	else
	{
		g_sendCmd->WriteString("");
		g_sendCmd->WriteString("");
	}
	g_sendCmd->WriteLong(lShowTime);//LeiJun
	tar->RecvCmd(g_sendCmd);
}

/**
	处理用户登录Region服务器，只有收到从gws来的add_user命令后
	并且用户也发送了login命令才会创建角色，创建之前，先比较种
	子，只有种子相等的情况下连接才可用。
	创建角色之前，先向gws发送一条acceptUser指令，是为了避免异
	常的现象，因为如果这时gws和region之间出现了网络抖动，可能
	会导致角色存在于region而gws因为add_user指令的timeout而将
	用户logout，如果角色创建失败，region再向gws发送一条logout
	指令（logout指令在与gws连接断开时，是缓存在队列中的）。
*/
bool CRegionServer::ProcessLogin(SNetLink* Link)
{
	// 当userId为0时，说明没有收到login命令
	if (Link->UserId == 0)
		return false;
	SUserData *user = g_region->m_gws.LookupUser(Link->UserId);
	CRegionUser* pRegionUser = FindUser(Link->UserId,false);

	// 当user为空时，说明没有收到来自gws的add_user命令
	if(user == NULL)
		return false;

	bool succeeded = false;
	if(user->seed == Link->Seed)
	{
        if(!pRegionUser)
        {
			if(g_region->CreateUser(Link, *user, NULL))
			{
				succeeded = true;
				g_region->m_gws.AcceptUser(Link->UserId);
			}
			else
			{
				m_gws.Logout(user->userID, true);
			}
        }
		else
        {
            succeeded = true;
			pRegionUser->ClearSession();
			pRegionUser->SetSession(Link->Session); // 由于网络终端，客户端重新连接
            if(g_cfg.game.bDebugMsg)
            {
                LOG1("由于网络中断，客户端重新连接 [%s]\n", pRegionUser->m_username.c_str());
            }
		}
		g_region->m_gws.RemoveUser(Link->UserId);
	}	


	pRegionUser = FindUser(Link->UserId,false);
	if(pRegionUser)
	{
	}else
	{
		LOG("CreateUser Error\n");
	}

	if (!succeeded)
    {
        DEL_ONE(Link->Session);
    }

	Link->Session = NULL;
	Link->Valid = false;
	return true;
}

/**
	处理Login的用户，这里用于收到来自gws的add_user命令后，检查
	unlogin队列中是否存在用户
*/
void CRegionServer::ProcessLoginUser(int userID)
{
	for(list<SNetLink*>::iterator Link = m_unloginLink.begin(); Link != m_unloginLink.end(); Link++)
	{
		if ((*Link)->UserId == userID)
		{
			ProcessLogin(*Link);
			break;
		}
	}
}

void CRegionServer::Shutdown()
{
	ERR("Shutdown the region server, all data will be discard\n");
    m_eState = ssShutdown;
}

void CRegionServer::Reboot()
{
	LOG("Reboot the region server, all data will be discard\n");
    m_eState = ssReboot;
}

void CRegionServer::KickAllUser(long lDelayTime)
{
    ERR1("CRegionServer::KillAllUser() at %d\n", rtTimestamp());
    m_lKickAllUserTime = lDelayTime - rand()%500;
    m_dwKickAllUserLastTickTime = 0;
}

void CRegionServer::StartListen()
{
	// Initialize network listen
	if(!m_clientListen.StartListen(
        g_cfgRs.listenClient.listenIP.c_str(),
		g_cfgRs.listenClient.listenPort, true))
	{
		ERR2("Startup: start listen error ip = %s,port = %d\n",
			g_cfgRs.listenClient.listenIP.c_str(), g_cfgRs.listenClient.listenPort);
        m_eState = ssExit;
		return;
	}
	m_canAcceptNewLink = true;
}

SUnionData* CRegionServer::FindUnion(DWORD unionID)
{
	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iterUnion = m_unionMap.find(unionID);
	if (iterUnion==g_region->m_unionMap.end())
		return NULL;
	else 
        return &(iterUnion->second);
}

SUnionData* CRegionServer::FindUnion(const char* unionName)
{
	string name = unionName;

	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iterUnion = m_unionMap.begin();
	for (; iterUnion!=m_unionMap.end(); iterUnion++)
	{
		if ((iterUnion->second).m_strName == name)
			return &(iterUnion->second);
	}

	return NULL;
}

void CRegionServer::BroadcastShowTimerByScene(CRegionScene *pScene, const char *pText, long lTime)
{
	if(!pText || pText[0] == 0 || lTime < 0)
		return;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_timer);
	g_sendCmd->WriteString(pText);
	g_sendCmd->WriteLong(lTime);
	//BroadcastCmd(g_sendCmd, 0);

	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		CRegionUser *user = (*it).second;
		if(user->m_dummy && user->m_dummy->m_scene == pScene)
			user->RecvCmd(g_sendCmd);
	}
}

void CRegionServer::BroadcastSceneDataByScene(CRegionScene *pScene, int iCount, const char **pInfoName, long *plInfovalue)
{
	if(!pScene || iCount < 0)
		return;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_npc_info);
	g_sendCmd->WriteLong(iCount);
	if(pInfoName && plInfovalue)
	{
		for(int ii=0; ii<iCount; ii++)
		{
			g_sendCmd->WriteString(pInfoName[ii]);
			char sValue[20] = {0};
			itoa(plInfovalue[ii], sValue, 10);
			g_sendCmd->WriteString(sValue);
		}
	}
	//BroadcastCmd(g_sendCmd, 0);

	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		CRegionUser *user = (*it).second;
		if(user->m_dummy && user->m_dummy->m_scene == pScene)
			user->RecvCmd(g_sendCmd);
	}
}

void CRegionServer::StartSchedule(CRegionCreature *pCreature, long lSchedule, const char *pRetScript)
{
	if(!pCreature || lSchedule <= 0 || !pRetScript)
		return;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_schedule);
	g_sendCmd->WriteLong(pCreature->GetObjectId());
	g_sendCmd->WriteLong(lSchedule);
	g_sendCmd->WriteString(pRetScript);
	pCreature->RecvCmd(g_sendCmd);
}

void CRegionServer::BroadcastCmd(CG_CmdPacket *cmd,CRegionCreature *exclude)
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionUser *user;
	
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		user = (*it).second;
		if(user->m_dummy == exclude) continue;
		user->RecvCmd(cmd);
	}
}

void CRegionServer::BroadcastCmd(CG_CmdPacket *cmd,CRegionCreature *exclude,char faction)
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionUser *user;

	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		user = (*it).second;
		if(user->m_dummy == exclude) continue;

		if(user->m_dummy->m_core.Faction == faction || faction == FACTION_ALL)
		{
			user->RecvCmd(cmd);
		}
	}
}

// ------------------------------------------------------------------------------
// 活动函数
// ------------------------------------------------------------------------------
bool CRegionServer::IsFunActive(bool fix)
{ 
	if(fix)
        return m_bFixFunActive;
	else
		return m_bTmpFunActive;
}

void CRegionServer::StartFun(bool fix) 
{ 
	if(fix)
	{
		m_bFixFunActive = true;
	}
	else
	{
		m_bTmpFunActive = true;
		m_funBegTime = time(NULL); 
		m_funInterval = 24*3600; 
	}
}

//
// 单次踢除所有玩家，经常会造成服务器异常，可能与数据量过大有一定关系
// 先改成在一段时间内踢除
//
void CRegionServer::EndFun()
{
	m_bFixFunActive = false;
	m_bTmpFunActive = false;
	SetTimer(REGION_TIMER_END_FUN,10*1000);

	/*
	// 传送活动场景中的玩家
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionUser *user;
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		user = (*it).second;
		if(IsInFunMap(user))
		{
			KickFunUser(user);
		}
	}
	*/
}

bool CRegionServer::IsInFunMap(CRegionUser *user)
{
	std::string map;
	RtsSceneBlockMap* bmap = g_region->m_defScene->FindBlockByPos(user->m_dummy->m_pos);
	if(bmap)
    {
		map = bmap->m_szFileName;
    }else
    {
        map = "scene01_001.rtb";
    }

	for(int i=0; i<(int)m_funMap.size(); i++)
	{			
		if(map == m_funMap[i]) return true;
	}
	return false;
}

void CRegionServer::KickFunUser(CRegionUser *user)
{
	int x,y;
	if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
	{
		if(GetProb(0.3))
		{
			x = 3090;
			y = 2690;
		}
		else if(GetProb(0.3))
		{
			x = 4740;
			y = -3165;
		}
		else
		{
			x = 6080;
			y = -2540;
		}
	}
	else
	{
		if(GetProb(0.3))
		{
			x = 48648;
			y = -16648;
		}
		else if(GetProb(0.3))
		{
			x = 53945;
			y = -18640;
		}
		else
		{
			x = 49425;
			y = -13980;
		}
	}

	user->m_dummy->ForceSetPosition(x,y,0);
}

bool CRegionServer::IsFunTime(bool fix)
{
	if(fix)
	{
		time_t tNow = time(NULL);
		struct tm* pTm = localtime(&tNow);
		if((pTm->tm_wday==5 && pTm->tm_hour >= 18) || pTm->tm_wday==6 ||
			(pTm->tm_wday==0 && pTm->tm_hour<19) 
			|| (pTm->tm_hour >=19 && pTm->tm_hour<=20))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
		// return time(NULL) < m_funBegTime + m_funInterval;
	}
}
//--------------------//
//逐鹿函数          //
//--------------------//
// bool CRegionServer::IsZhuluActive(bool fix)
// {
// 	if(fix)
// 		return m_bFixZhuluActive;
// 	else
// 		return m_bTmpZhuluActive;
// }
// 
// void CRegionServer::StartZhulu(bool fix)
// {
// 	if(fix)
// 	{
// 		m_bFixZhuluActive = true;
// 	}
// 	else
// 	{
// 		m_bTmpZhuluActive = true;
// 		m_ZhuluBegTime = time(NULL);
// 		m_ZhuluInterval = 24*3600;
// 	}
// 
// 	m_ZhuluActiveUser.clear();
// 	m_ZhuluWinFaction = 2;
// }
// 
// void CRegionServer::OnEnterZhulu(CRegionUser *user)
// {
// 	std::list<ULONG>::iterator it = find(m_ZhuluActiveUser.begin(),m_ZhuluActiveUser.end(),user->m_userId);
// 	if(it == m_ZhuluActiveUser.end())
// 	{
// 		m_ZhuluActiveUser.push_back(user->m_userId);
// 	}
// }
// 
// void CRegionServer::OnZhuluWin(char faction)
// {
// 	if(m_ZhuluWinFaction!=2) 
// 		return;
// 	m_ZhuluWinFaction = faction;
// 
// 	CRegionUser *pUser;
// 	std::list<ULONG>::iterator it = m_ZhuluActiveUser.begin();
// 
// 	for(; it!=m_ZhuluActiveUser.end();++it)
// 	{
// 		ULONG id = *it;
// 		if(m_userMap.find(id) != m_userMap.end())
// 		{
// 			pUser = m_userMap[id];
// 			if(pUser->m_dummy->m_core.Faction == faction)
// 			{
// 				SItemID item = m_pItemManager->CreateItem(1311);
// 				if(ItemID_IsValid(item))
// 				{
// 					pUser->m_dummy->AddItem(item);
// 					pUser->m_dummy->SendSystemMessage("恭喜您获得逐鹿的奖励！");
// 				}
// 			}
// 		}
// 	}
// } 
// 
// void CRegionServer::EndZhulu()
// {
// 	m_bFixZhuluActive = false;
// 	m_bTmpZhuluActive = false;
// 	SetTimer(REGION_TIMER_END_ZHULU,10*1000);
// }
// 
// bool CRegionServer::IsInZhuluMap(CRegionUser *user)
// {
// 	std::string map;
// 	RtsSceneBlockMap* bmap = g_region->m_defScene->FindBlockByPos(user->m_dummy->m_pos);
// 	if(bmap)
// 	{
// 		map = bmap->m_szFileName;
// 	}else
// 	{
// 		map = "scene01_001.rtb";
// 	}
// 
// 	for(int i=0; i<(int)m_ZhuluMap.size(); i++)
// 	{
// 		if(map == m_ZhuluMap[i]) return true;
// 	}
// 	return false;
// }
// 
// void CRegionServer::KickZhuluUser(CRegionUser *user)
// {
// 	int x,y;
// 	if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
// 	{
// 		if(GetProb(0.3))
// 		{
// 			x = 3090;
// 			y = 2690;
// 		}
// 		else if(GetProb(0.3))
// 		{
// 			x = 4740;
// 			y = -3165;
// 		}
// 		else
// 		{
// 			x = 6080;
// 			y = -2540;
// 		}
// 	}
// 	else
// 	{
// 		if(GetProb(0.3))
// 		{
// 			x = 48648;
// 			y = -16648;
// 		}
// 		else if(GetProb(0.3))
// 		{
// 			x = 53945;
// 			y = -18640;
// 		}
// 		else
// 		{
// 			x = 49425;
// 			y = -13980;
// 		}
// 	}
// 
// 	user->m_dummy->ForceSetPosition(x,y,0);
// }
// 
// bool CRegionServer::IsZhuluTime(bool fix)
// {
// 	if(fix)
// 	{
// 		time_t tNow = time(NULL);
// 		struct tm* pTm = localtime(&tNow);
// 		if((pTm->tm_wday==2 || pTm->tm_wday == 4) && (pTm->tm_hour>=19 && pTm->tm_hour<21))
// 		{
// 			return true;
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 	}
// 	else
// 	{
// 		return true;
// 	}
// }

void CRegionServer::AddCheater(CRegionUser *user)
{
	LOG1("add cheater %s\n",user->m_dummy->m_core.Name.c_str());
    if (m_cheaterMap.find(user->m_userId)==m_cheaterMap.end())
    {
        m_iCheaterCnt ++;
        if (m_iCheaterCnt<20)
        {
            // 发送到Log Server
            char szBuf[512];
            rt2_sprintf(szBuf, "'%s',%d,%s", user->m_username.c_str(), user->m_userId, g_cfgRs.connectGameWorld.bindIP.c_str());
            LogServerSender_Send(user, "Cheater", szBuf);
        }
    }
	m_cheaterMap[user->m_userId] = 1;
	if(m_bKickCheater && user->m_dummy->m_cheatKick == 0)
    {
		user->m_dummy->m_cheatKick = rtGetMilliseconds() + GetRand(90,30)*100;
    }
}

void CRegionServer::OnUserEnter(CRegionUser *user)
{
	// user->m_dummy->m_cheatKick = rtGetMilliseconds() + GetRand(90,30)*1000;
	if (1 == user->m_dummy->m_IsDead)
	{
		g_region->BuildEventDead(g_sendCmd, user->m_dummy);
	}
	if(user->m_dummy->m_usingCard)
	{
		user->m_dummy->m_usingCard = false;
		user->m_dummy->StartUseCard();
    }
	if (IsInFunMap(user) && !IsFunTime(true))
	{
		KickFunUser(user);
	}

    //if (user->m_bankPass.empty())
    //    user->m_dummy->SendSystemMessage("系统提示：您的仓库密码尚未设置，请尽快设置，确保您的账号安全");

	RtCoreLog().Debug("user name: %s, ip: %s, LoginRegion. Online count: %d\n",
		user->m_username.c_str(),user->m_userIpAddress.c_str(),(int)m_userMap.size());
}

void CRegionServer::OnUserQuit(CRegionUser *user)
{
	m_cheaterMap.erase(user->m_userId);

	if(g_bUnWg)
	{
		if(!NES_UserLogout_S(user->m_userId))
		{
#ifdef WIN32
			LOG("NES_UserLogout_S Success\n");
#endif
		}
	}
	RtCoreLog().Debug("user name: %s, ip: %s, LeaveRegion. Online count: %d\n",
		user->m_username.c_str(),user->m_userIpAddress.c_str(),(int)m_userMap.size());
}

bool CRegionServer::IsCheatId(DWORD id)
{
#ifdef KICK_CHEAT
	return (id%21 == 0);
#else
	return false;
#endif
}

bool CRegionServer::UserUploadString_IsValid(const char* str)
{
	const char* p = str;
	const char* stop = p + strlen(str);

	const char* Premission = "~!@#$%^&*()_+|:;<,>.?";

	while(p < stop)
	{
		if (*p & 0x80)
		{
			p += 2;
		}
		else
		{
			if (!(*p >= '0' && *p <= '9' ||
				*p >= 'A' && *p <= 'Z' ||
				*p >= 'a' && *p <= 'z' ||
				*p == '_' || strchr(Premission, *p)!=NULL) )
			{
				return false;
			}

			p++;
		}
	}

	return true;
}

int CRegionServer::GetCurrentCheckedWgUser()
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionUser *user;
    int iTotal = 0;
	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		user = (*it).second;
        if (user->m_dummy)
        {
            if (user->m_dummy->m_wgChecked)
            {
                iTotal ++;
            }
        }
	}
    return iTotal;
}

// 返回false表示找到外挂
bool CRegionServer::JhWgCheck(int iCheckData, long lClientSeed, short cmdID)
{
    int i, iData;
    iData = iCheckData - lClientSeed;
    for (i=0; i<m_iClientCheckCodeCnt; i++)
    {
        if (iData==m_dwClientCheckCode[i])
        {
            return true;
        }
    }
    if (m_bJhWgCollect)
    {
        if (m_iClientCheckCodeCnt<100)
        {
            m_dwClientCheckCode[m_iClientCheckCodeCnt] = iData;
            FILE* fp;
            if (m_iClientCheckCodeCnt==0)
                fp = fopen("check_collect.txt", "wt");
            else
                fp = fopen("check_collect.txt", "at+");
            if (cmdID==c2r_attack)
            {
                fprintf(fp, "atk = \"0x%08X\"\n", iData);
            }else
            {
                fprintf(fp, "mov = \"0x%08X\"\n", iData);
            }
            fclose(fp);
            m_iClientCheckCodeCnt ++;
        }
        return true;
    }
    return false;
}

void CRegionServer::ConnectJhWgServer()
{
    if (m_bJhWgServer && m_pJhWgServer)
    {
        m_pJhWgServer->SetConnectServer(
            m_strJhWgServerHost.c_str(), 9001,
            m_strLoginName.c_str(),
            m_strGameworldName.c_str(),
            m_lCurRegionID,
            m_strJhWgBindIP.c_str());
    }
}

void CRegionServer::ReadJhWgCheckConfig(const char* szFilename)
{
    m_bJhWgCheck = false;
    m_bJhWgCollect = false;
    m_bJhWgServer = false;
    m_iJhWgPunish = 60;
    m_iJhWgInterval = 300;
    m_iClientCheckCodeCnt = 0;
    m_strJhWgServerHost.clear();

    RtIni ini(true);
    if(!ini.OpenFile(szFilename))
    {
        return;
    }
    ini.CloseFile();

    RtString szName, szValue;

    const char* pStr;
    pStr = ini.GetEntry("check", "enable");   if (pStr) m_bJhWgCheck    = atoi(pStr)!=0;
    pStr = ini.GetEntry("check", "collect");  if (pStr) m_bJhWgCollect  = atoi(pStr)!=0;
    pStr = ini.GetEntry("check", "server");   if (pStr) m_bJhWgServer   = atoi(pStr)!=0;
    pStr = ini.GetEntry("check", "punish");   if (pStr) m_iJhWgPunish   = (int) atoi(pStr);
    pStr = ini.GetEntry("check", "interval"); if (pStr) m_iJhWgInterval = (int) atoi(pStr);

    if (m_bJhWgServer)
    {
        pStr = ini.GetEntry("check", "bind"); if (pStr) m_strJhWgBindIP = pStr;

        pStr = ini.GetEntry("check", "host");
        if (pStr)
        {
            m_strJhWgServerHost = pStr;
        }
    }

    ini.FindSection("data");
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szValue.GetLength()>3)
            {
                if (szValue[0]=='0' && (szValue[1]=='x'||szValue[1]=='X'))
                {
                    pStr = szValue;
                    pStr += 2;
                    m_dwClientCheckCode[m_iClientCheckCodeCnt] = 0;
                    while (*pStr)
                    {
                        m_dwClientCheckCode[m_iClientCheckCodeCnt] <<= 4;
                        if (*pStr>='0' && *pStr<='9')
                        {
                            m_dwClientCheckCode[m_iClientCheckCodeCnt] |= (*pStr-'0');
                        }else if (*pStr>='A' && *pStr<='F')
                        {
                            m_dwClientCheckCode[m_iClientCheckCodeCnt] |= (*pStr-'A')+10;
                        }else if (*pStr>='a' && *pStr<='f')
                        {
                            m_dwClientCheckCode[m_iClientCheckCodeCnt] |= (*pStr-'a')+10;
                        }
                        pStr ++;
                    }
                    m_iClientCheckCodeCnt ++;
                }
            }
        }while (ini.NextEntry(&szName, &szValue));
    }

    if (m_bJhWgCollect)
    {
        m_iClientCheckCodeCnt = 0;
    }
}

void CRegionServer::OnJhWgServerUserProgram(long lActorID, short sProgramID, long lSecond, const char* szMsg)
{
	CRegionUser* pUser = g_region->FindUser(lActorID);
	if(!pUser) return;
	if (!pUser->m_dummy) return;

	switch (sProgramID)
	{
	case 1: // 信息
		pUser->m_dummy->SendSystemMessage(szMsg);
		break;
	case 2: // 禁止移动
		pUser->m_dummy->SendSystemMessage(szMsg);
        pUser->ForbidUser(true, CreState_CanMove, lSecond);
		break;
	case 3: // 断线
		g_region->KickUser(lActorID, 0, true, true,"break line");
		break;
	case 4: // 断线并封号, -1为永久封号
		g_region->KickUser(lActorID, 0, true, true, "lock user");
        if (lSecond==-1)
        {
            pUser->LockAccount(true, 365*24*60*60); // 一年
        }else
        {
            pUser->LockAccount(true, lSecond);
        }
		break;
	}
}

CRegionServer::SRandomImage* CRegionServer::CreateRandomImage()
{
    int iRetry = 3;
    while (!m_pRandomImgFactory->RandomString(m_randomImageData.szStrBuf, 3))
    {
        iRetry --;
        if (iRetry<=0)
        {
            return &m_randomImageData;
        }
    }
    DWORD* pText = m_pRandomImgFactory->CreateRandomImage32b(m_randomImageData.szStrBuf, 32, 32, NULL, 1);
    m_pRandomImgFactory->AddNoise32b(pText, 32, 32, 0x00FFFFFF, 0x00000000, 0x00EEEEEE);
    m_randomImageData.iDataSize = m_pRandomImgFactory->RLE32bData(pText, 32*32*4, 0x00000000, (unsigned char*)m_randomImageData.szDataBuf, 32*32*4);
    return &m_randomImageData;
}

bool CRegionServer::IsHashError(int id,CRegionUser *user,int hash)
{
	// if hash is 1401 pass it, louis
	if (hash == 1401)	return false;
	int ret;
	string tmp;
	char buf[128];

	id += 123456789;
	rt2_sprintf(buf,"zql%dunknow",id);
	// tmp = user->m_luaPrefix;
	// tmp += user->m_luaEndfix;
	tmp = buf;

	if(tmp.length()%3 == 0)
	{
		ret = ha_P3(tmp.c_str(),(int)tmp.length(),65500)+ha_P2("53965977",8);
	}
	else
	{
		ret = ha_P2(tmp.c_str(),(int)tmp.length())+ha_P3("53965977",8,65500);
	}
	// LOG2("ret = %d,hash = %d\n",ret,hash);
	return ret != hash;
}

void CRegionServer::OnDayChange()
{
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	CRegionUser *user;
	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);

	for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
	{
		user = (*it).second;
		if(!user->m_dummy) continue;
		user->m_dummy->m_mtFinish[0] = 0;
		user->m_dummy->m_mtFinish[1] = 0;
		user->m_dummy->m_mtFinish[2] = 0;
		user->m_dummy->m_mtClearDay  = pTm->tm_yday;

		user->m_dummy->m_wenClearDay = pTm->tm_yday;
		user->m_dummy->m_wenTaskFinish = 0;
		user->m_dummy->m_wenTaskAccept = 0;

		user->m_dummy->m_gongClearDay = pTm->tm_yday;
		user->m_dummy->m_gongTaskFinish = 0;
		user->m_dummy->m_gongTaskAccept = 0;
	}
}

bool CRegionServer::SendUserLogEvent(char type, long userID, const char* ip, long longParam1, const char* strParam1, long longParam2, const char* strParam2, long longParam3, const char* strParam3)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_log_event);
	packet.WriteByte(type);
	packet.WriteLong(userID);
	packet.WriteString((char*)ip);
	packet.WriteLong(longParam1);
	packet.WriteString((char*)strParam1);
	packet.WriteLong(longParam2);
	packet.WriteString((char*)strParam2);
	packet.WriteLong(longParam3);
	packet.WriteString((char*)strParam3);
	return g_region->m_gws.EndSend();
}


bool CRegionServer::LogServerSender_Init()
{
	if ( !m_LogServerSession.Create(NULL, 0) )
	{
		return false;
	}
	return true;
}

bool CRegionServer::LogServerSender_Close()
{
	m_LogServerSession.Close();
	return true;
}

bool CRegionServer::LogServerSender_Send(CRegionUser* pUser, const char* szTitle, const char* szContent)
{
	// LMT_STRING = 2
	long type = 2;
    char macAddress[16] = {0, 0, 0, 0, 0, 0};
	memset(macAddress, 0, 16);

	CG_CmdPacket tmpDataPacket;
	tmpDataPacket.WriteString((char*)szTitle);
	tmpDataPacket.WriteString((char*)szContent);

	CG_CmdPacket cmd;
	cmd.BeginWrite();
	cmd.WriteLong( type );								    // Msy Type
	cmd.WriteLong( tmpDataPacket.GetByteDataSize() );	    // DataSize(不包括以下3个变量)
    cmd.WriteString((char*)pUser->m_accountName.c_str() );  // 用户名 [玩家帐号名]
    cmd.WriteString((char*)pUser->m_userIpAddress.c_str()); // 用户IP [玩家IP地址]
	cmd.WriteBinary(macAddress, 6);					        // 用户Mac地址
	cmd.WriteBinary(tmpDataPacket.GetByteData(), tmpDataPacket.GetByteDataSize() );

	CG_NetAddress toAddress;
	toAddress.SetRemote("zfslog01.gamigo.com.cn"); // Log Server IP
	toAddress.SetPort(6670);

    static bool bLog = true;
	if ( !toAddress.BuildAddr() )
	{
        if (bLog)
        {
		    ERR("[GWServer::LogServerSender_Send] CG_NetAddress Build Address Error\n");
            bLog = false;
        }
		return false;
	}
	if ( !m_LogServerSession.SendTo(&cmd, &toAddress) )
	{
        if (bLog)
        {
		    ERR("[GWServer::LogServerSender_Send] SendTo error\n");
            bLog = false;
        }
		return false;
	}
	return true;
}

// [8/20/2009 tim.yang] 双倍经验瓶使用限制
void CRegionServer::ReadUseCardMinBottleMaxNum()
{
	short maxnum = 5;
	RtIni regionini;
	if (regionini.OpenFile(R(INI_REGION)))
	{
		const char* strMaxNum = regionini.GetEntry("game","CardMinBottleMaxNum");
		if (strMaxNum)
		{
			maxnum = atoi(strMaxNum);
		}
		regionini.CloseFile();
		SetUseCardMinBottleMaxNum(maxnum);
	}
	else
	{
		ERR("load card minute bottle max num failed!\n");
		Exit();
	}
}

bool CRegionServer::WriteUseCardMinBottleMaxNum(int num)
{
	RtIni regionini;
	if (regionini.OpenFile(R(INI_REGION),true))
	{
		const char* strRule = "game";
		if (!regionini.FindSection(strRule)) 
		{
			if (!regionini.AddSection(strRule))
			{
				regionini.CloseFile();
				return false;
			}
		}
		if (regionini.SetEntry(strRule,"CardMinBottleMaxNum",(long)num))
		{
			regionini.CloseFile();
			return true;
		}
		regionini.CloseFile();
		return false;
	}
	return false;
}

void CRegionServer::SetUseCardMinBottleMaxNum(int num)
{
	m_useCardMinuteBottleMaxNum = num;
}

int CRegionServer::GetUseCardMinBottleMaxNum()
{
	return m_useCardMinuteBottleMaxNum;
}
