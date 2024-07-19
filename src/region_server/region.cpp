//
// region_server.cpp : Defines the entry point for the console application.
//
#include "region.h"
#include "filepack/memory_file.h"
#include <signal.h>

SConfigRegion       g_cfg;
SConfigRegionNet    g_cfgRs;
//SConfigEudemon      g_cfgEudemon;
//SConfigConnect      g_cfgConnectGameWorld;
CG_CmdPacket       *g_sendCmd;
CG_CmdPacket       *g_sendCmdWG;
CRegionFactory     *g_factory;
CRegionServer      *g_region;
bool				g_bUnWg;
bool				g_bClientCommand;

RtScene* g_pScene = NULL;

G_MEMDEF(g_szExeMD5, 40)
G_MEMDEF(g_szDataMD5, 40)
//char g_szExeMD5[40], g_szDataMD5[40];

static CRegionServer::ServerState   s_eExitState;

void TestRandom()
{
	LOG("Test random number\n");
	float prob[3] = {0.1,0.5,0.01};
	float ret[3];
	for(int i=0; i<3; i++)
	{
		int succeed = 0;
		for(int j=0; j<100000; j++)
			if(GetProb(prob[i])) succeed++;
		ret[i] = succeed / 100000.0f;
		LOG2("    prob = %f, ret = %f\n",prob[i],ret[i]);
	}
}

void TestPacket()
{
	CG_CmdPacket ttt;
	char tmp[1024];
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteLong(1001);
	g_sendCmd->WriteString("test");

	int size = 1024;
	g_sendCmd->SaveToBuffer(tmp,size);
	LOG1("save size = %d\n",size);

	ttt.LoadFromBuffer(tmp,size);
	long lg;char *str;
	ttt.ReadLong(&lg);
	ttt.ReadString(&str);

	LOG2("long = %d, str = %s",lg,str);
}

bool ReadConfigFile(const char *szFilename)
{
    RtIni ini(true);
    RtString szName, szValue;
    if(!ini.OpenFile(szFilename))
    {
        ERR1("Region: open configuration file [%s] error\n",szFilename);
        return false;
    }
    ini.CloseFile();

    // 侦听客户端
    g_cfgRs.listenClient.listenIP   = (const char*)ini["listenClient"]["ListenIP"];
    g_cfgRs.listenClient.listenPort = (int)        ini["listenClient"]["ListenPort"];
    g_cfgRs.listenClient.capacity   = (int)        ini["listenClient"]["capacity"];

	long lUnWgValid = 0;
	if (ini.GetEntry("unwg", "Valid", &lUnWgValid))
	{
		g_bUnWg = (lUnWgValid!=0);
	}
	long lDebugClient = 0;
	if (ini.GetEntry("unwg", "DebugClientCommand", &lDebugClient))
	{
		g_bClientCommand = (lDebugClient!=0);
	}
	else g_bClientCommand = false;
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfgRs.listenClient.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    // 连接Gameworld
    g_cfgRs.connectGameWorld.ip       = (const char*)ini["connectGameworld"]["ConnectIP"];
    g_cfgRs.connectGameWorld.port     = (int)        ini["connectGameworld"]["ConnectPort"];
    g_cfgRs.connectGameWorld.bindIP   = (const char*)ini["connectGameworld"]["BindIP"];
    g_cfgRs.connectGameWorld.bindPort = (int)        ini["connectGameworld"]["BindPort"];
    g_cfgRs.lRegionID                 = (short)      ini["connectGameworld"]["region_id"];

	const char* licenseIP = NULL;
	if(licenseIP = ini.GetEntry("connectLicense", "ConnectIP"))
	{
		g_strConnectLicenseIP = licenseIP;
	}else
	{
		g_strConnectLicenseIP = "licence.zfs.gamigo.com.cn";
	}

	if(!ini.GetEntry("connectLicense", "ConnectPort", &g_strConnectLicensePort))
	{
		g_strConnectLicensePort = 9000;
	}


    /*
    // eudemon
    const char* pStr;
    pStr = ini.GetEntry("eudemon", "enable");           if (pStr) g_cfgEudemon.enable = atoi(pStr);
    pStr = ini.GetEntry("eudemon", "host_ip");          if (pStr) g_cfgEudemon.ip = pStr;
    pStr = ini.GetEntry("eudemon", "host_port");        if (pStr) g_cfgEudemon.port = atol(pStr);
    pStr = ini.GetEntry("eudemon", "update_interval");  if (pStr) g_cfgEudemon.update_interval = atol(pStr);
    */

    return true;
}

bool ReadRegionConfigFile(const char *szFilename)
{
    RtIni ini;
    if(!ini.OpenFile(szFilename))
    {
        ERR1("Gameworld: open config [%s] error\n",szFilename);
        return false;
    }
    ini.CloseFile();

    // game play config var
    g_cfg.listenClient.unlogin_timeout  = atol(ini.GetEntry("listenClient","unlogin_timeout"));
    g_cfg.listenClient.r2c_interval     = atol(ini.GetEntry("listenClient","r2c_interval"));
    g_cfg.listenClient.c2r_interval     = atol(ini.GetEntry("listenClient","c2r_interval"));
    g_cfg.listenClient.cl_max_buffer    = atol(ini.GetEntry("listenClient","cl_max_buffer"));
    g_cfg.listenClient.cl_pre_buffer    = atol(ini.GetEntry("listenClient","cl_pre_buffer"));

    // game play config var
    g_cfg.game.scene_item_live_time     = atol(ini.GetEntry("game","scene_item_live_time"));
	g_cfg.game.exp_rate                 = atof(ini.GetEntry("game","exp_rate"));
	g_cfg.game.gold_rate                = atof(ini.GetEntry("game","gold_rate"));

	g_cfg.game.item_drop_rate           = atof(ini.GetEntry("game","item_drop_rate"));
	g_cfg.game.item_green_rate          = atof(ini.GetEntry("game","item_green_rate"));
	g_cfg.game.item_blue_rate           = atof(ini.GetEntry("game","item_blue_rate"));
	g_cfg.game.item_gold_rate           = atof(ini.GetEntry("game","item_gold_rate"));

    g_cfg.game.bDebugMsg                = ini.GetEntry("game","DebugMsg")?atoi(ini.GetEntry("game","DebugMsg")):0;

    // gws config var
    g_cfg.gws.update_user_interval      = atoi(ini.GetEntry("gws","update_user_interval"));
    g_cfg.gws.accept_user_timeout       = atoi(ini.GetEntry("gws","accept_user_timeout"));

    return true;
}

bool RegionInit()
{
	// LOG1("cre size = %d\n",sizeof(CRegionCreature));
	std::string strErr;
	if(!LoadConfig(strErr))
	{
		 ERR("Startup: Load config file failed\n");
		 return false;
	}
    if (!InitMapString(GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath")))
    {
        ERR("Startup: read [language/chinese/strmap.csv] file failed\n");
        return false;
    }
	InitBannedString(R(RES_TABLE_CZSTRBANNED));
	InitChatBannedString(R(RES_TABLE_CZCHATBANNED));

	g_sendCmd = RT_NEW CG_CmdPacket;
	g_sendCmd->SetSize(MAX_CMD_PACKET_SIZE);
	g_sendCmdWG = RT_NEW CG_CmdPacket;
	g_sendCmdWG->SetSize(MAX_CMD_PACKET_SIZE);
	g_factory = RT_NEW CRegionFactory;
	g_region = RT_NEW CRegionServer;

    // read config file
    LOG("Region: Load config ... \n");
    if(!ReadRegionConfigFile(R(INI_REGION)))
    {
        ERR("Startup: read region config file [sv_table/region.ini] failed");
        return false;
    }	
    if(!ReadConfigFile(R(INI_REGION_SERVER)))
    {
        ERR("Startup: read region net config file [region_server.ini] failed");
        return false;
    }
    g_region->ReadJhWgCheckConfig("region_check.ini");
    LOG("        Done\n");
	return true;
}

bool RegionWaittingForGWS()
{
    return g_region->RegionWaittingForGWS();
}

void RegionRun()
{
	g_region->Run();
}

void RegionExit()
{
    s_eExitState = g_region->m_eState;
	DEL_ONE(g_sendCmd);
	DEL_ONE(g_sendCmdWG);
	DEL_ONE(g_region);
    ClearMapString();
	DEL_ONE(g_factory);
}

const int TEST_NUM = 1000;
struct SBitTest
{
	long value;
	long size;
};

#ifdef LINUX
#include <signal.h>
void sig_pipe(int signal)
{
}
#endif

void OnExitProcess(int sig)
{
    g_region->Shutdown();
}

int SimClient()
{
	// test
	CG_TCPSession *t = RT_NEW CG_TCPSession[3500];
	for(int i=0; i<2000; i++)
	{
		t[i].Connect("192.168.0.14",6660);
	}
	int cnt = 0;
	int start = rtGetMilliseconds();

	while(rtGetMilliseconds() - start < 10000)
	{
		for(int i=0; i<3500; i++)
			t[i].Process();
	}

	for(int i=0; i<3500; i++)
	{
		if(t[i].GetState() == NET_STATE_CONNECTED)
		cnt++;
	}

	printf("cnt = %d\n",cnt);
	getchar();
	return 0;
}

int SimServer()
{
	CG_TCPListener lis;
	if(!lis.StartListen("localhost",6660))
		return 0;

	// vector<CG_TCPSession*> seList;
	list<CG_TCPSession*> seList;
	list<CG_TCPSession*>::iterator it;
	CG_TCPSession *se;

#ifdef LINUX
	pollfd *pfds = RT_NEW pollfd[10000];
#endif

	while(1)
	{
		while(1)
		{
			se = lis.AcceptNewSession();
			if(!se)
			break;
			seList.push_back(se);
		}

		DWORD cost = rtGetMilliseconds();
		//for(int i=0; i<seList.size(); i++)
		for(it=seList.begin();it!=seList.end();it++)
		{
			// se = seList[i];
			se = *it;
			se->DefProc();
		}
		cost = rtGetMilliseconds() - cost;
		LOG2("process = %d, link = %d\n",cost,seList.size());
		rtSleep(500);

#ifdef LINUX
		cost = rtGetMilliseconds();
		int num = 0;
		for(it=seList.begin();it!=seList.end();it++)
		{
			se = *it;
			pfds[num].fd = se->GetSocket().m_socket;
           	pfds[num].events = POLLIN;
			num++;
		}
		int ret = poll(pfds,num,0);
		cost = rtGetMilliseconds() - cost;
		LOG2("poll = %d, link = %d\n",cost,seList.size());
		
		/*
		// poll 
		num = 0;
		for(it=seList.begin();it!=seList.end();it++)
		{
			se = *it;
			if(pfds[num].fd&POLLIN)
			{
				// can read data
			}
			num++;
		}
		
		// epoll
		for(it=seList.begin();it!=seList.end();it++)
		{
			// add to ev
		}
		
		// if can read
		// way1,add point to ev
		// way2,use idx
		*/
#endif
	}
}

class CClientNotify : public CG_NetClientNotify
{
public:
	CClientNotify() {}
	virtual ~CClientNotify() {}

	virtual void OnConnect(CG_NetClient *client, bool ret) 
	{
		LOG("client on connect\n");

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteString("test123");
		client->SendPacket(g_sendCmd);
	}
	virtual void OnReceive(CG_NetClient *client, CG_CmdPacket *packet) 
	{
		LOG("client on receive\n");
		char *msg;
		if(packet->ReadString(&msg))
			LOG1("recv [%s]\n",msg);
	}
	virtual void OnDisconnect(CG_NetClient *client, int reason) 
	{
		LOG("client on disconnect\n");
	}
};

void NewSimClient()
{
	CG_NetClient *client = RT_NEW CG_NetClientTcp;
	client->Init(NULL,0,RT_NEW CClientNotify);
	client->Connect("localhost",5555);
	while(1)
	{
		client->Process();
		rtSleep(100);
	}
}

class CServerNotify : public CG_NetServerNotify
{
public:
	CServerNotify() {}
	virtual ~CServerNotify() {}

	virtual void OnAccept(CG_NetServer *server, CG_NetClient *client)
	{
		LOG("server on accept\n");
	}
	virtual void OnDisconnect(CG_NetServer *server, CG_NetClient *client, int reason) 
	{
		LOG("server on disconnect\n");

	}
	virtual void OnReceive(CG_NetServer *server, CG_NetClient *client, CG_CmdPacket *packet)
	{
		LOG("server on receive\n");
		char *msg;
		if(packet->ReadString(&msg))
			LOG1("recv [%s]\n",msg);

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteString("567890");
		client->SendPacket(g_sendCmd);
	}
};

void NewSimServer()
{
	CG_NetServer *server = RT_NEW CG_NetServerTcp;
	server->Init(NULL,5555,true,RT_NEW CServerNotify);
	while(1)
	{
		server->Process();
		rtSleep(100);
	}
}

extern void RegisterMemoryBlock(void);
extern void RegisterSharedMemProtector(void);

int main(int argc, char *argv[])
{
#ifdef	ENABLE_OPERATOR_NEW
	RegisterSharedMemProtector();
	RegisterMemoryBlock();
	rt2_ActiveSafestr();
#endif

#ifdef LINUX
	signal(SIGPIPE,sig_pipe);
#endif

	signal(SIGTERM, OnExitProcess);
	signal(SIGINT, OnExitProcess);

#ifdef LINUX
    const char* szPIDFileName = "region_server.pid";
    FILE* fpPID;
#ifndef _DEBUG
    if (fpPID = fopen(szPIDFileName, "rt"))
    {
        printf("ERROR: PID file [%s] already existed.\n", szPIDFileName);
        fclose(fpPID);
        return 105;
    }
#endif
    if (fpPID = fopen(szPIDFileName, "wt"))
    {
        fprintf(fpPID, "%d", getpid());
        fclose(fpPID);
    }
#endif

	Py_NoSiteFlag = 1;
	
	// R [11/22/2008 Louis.Huang]
	//rtCoreInit("region_server",NULL,NULL,NULL,NULL);
	if (!rtCoreInit("region_server.ini"))
	{
		ERR1("Init core failed!!");
		return false;
	}

	{
#ifdef WIN32
        const char* szExeFileName = "region_server.exe";
#else
        const char* szExeFileName = "region_server";
#endif
        if (!rtMD5FileString((char*)szExeFileName, g_szExeMD5))
        {
            ERR1("ERROR: Cannot get [%s]'s MD5\n", szExeFileName);
            rt2_strcpy(g_szExeMD5, "NotFound");
        }
        if (!rtMD5FileString("region_server.pak", g_szDataMD5))
        {
            ERR1("ERROR: Cannot get [%s]'s MD5\n", "region_server.pak");
            rt2_strcpy(g_szDataMD5, "NotFound");
        }
    }

	// C [11/22/2008 Louis.Huang]
/*
	CRtPackManager* pPackManager=RT_NEW CRtPackManager;
    pPackManager->OpenPack("region_server.pak",false);
    CRtPackAndAnsiManager* pAllManager=RT_NEW CRtPackAndAnsiManager(pPackManager,g_pFileManager);
    g_pFileManager=pAllManager;
*/

	if (!RegionInit())
    {
        return 101; // 16 sec later reboot
    }

	CScriptEngine::GetSingleton()->Start();

	// SimClient(); return 0;
	// SimServer(); return 0;
	// TestRandom();
	// TestPacket();
	// NewSimClient();
	// NewSimServer();

	RegionRun();
	RegionExit();

#ifdef LINUX
    RtCoreFile().Delete(szPIDFileName);
#endif
	rtCoreExit();
	CScriptEngine::GetSingleton()->Stop();

    int exitCode;
    switch(s_eExitState)
    {
		case CRegionServer::ssExit: exitCode = 105; break;
		case CRegionServer::ssReboot: exitCode = 101; break;
		case CRegionServer::ssShutdown: exitCode = 104; break;
		default: exitCode = -1; break;
	}
    
	return exitCode;
}
