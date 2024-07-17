#include "gw_server.h"
#include "filepack/memory_file.h"
#include <Windows.h>
#include <signal.h>

CG_CmdPacket *g_sendCmd, *g_recvCmd;
//char g_szExeMD5[40], g_szDataMD5[40];
G_MEMDEF(g_szExeMD5, 40)
G_MEMDEF(g_szDataMD5, 40)

void OnExitProcess(int sig)
{
	LOG("\n--------------------------------------------\n");
	LOG("\nWaiting for server shutdown...[Ctrl+C]\n");
	g_server.m_state = ssExit;
}

bool LoadConfiguration(const char *szFilename)
{
    RtIni ini(true);
    RtString szName, szValue;
    if(!ini.OpenFile(szFilename))
    {
        ERR1("Gameworld: open configuration file [%s] error\n",szFilename);
        return false;
    }
    ini.CloseFile();

    // 侦听客户端
    g_cfgGW.listenClient.listenIP   = (const char*)ini["listenClient"]["ListenIP"];
    g_cfgGW.listenClient.listenPort = (int)        ini["listenClient"]["ListenPort"];
    g_cfgGW.listenClient.capacity   = (int)        ini["listenClient"]["capacity"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfgGW.listenClient.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    // 侦听Region
    g_cfgGW.listenRegion.listenIP   = (const char*)ini["listenRegion"]["ListenIP"];
    g_cfgGW.listenRegion.listenPort = (int)        ini["listenRegion"]["ListenPort"];
    g_cfgGW.listenRegion.capacity   = (int)        ini["listenRegion"]["capacity"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfgGW.listenRegion.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }
    const char* szDungeonRsIDList   = (const char*)ini["listenRegion"]["dungeon"];
    char szCopyList[256];
    rt2_strncpy(szCopyList, szDungeonRsIDList, 255);
    //szCopyList[255] = 0;
    if (szDungeonRsIDList)
    {
        char* tok = strtok(szCopyList, ", ");
        if (tok)
        {
            do
            {
                g_cfgGW.listenRegion.mapDungeonRsID.insert(atoi(tok));
                tok = strtok(NULL, ", ");
            }while (tok);
        }
    }

    // 侦听GM
    g_cfgGW.listenGM.listenIP   = (const char*)ini["listenGMServer"]["ListenIP"];
    g_cfgGW.listenGM.listenPort = (int)        ini["listenGMServer"]["ListenPort"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfgGW.listenGM.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    // 连接Database
    g_cfgGW.connectDatabase.ip                = (const char*)ini["connectDatabase"]["ConnectIP"];
    g_cfgGW.connectDatabase.port              = (int)        ini["connectDatabase"]["ConnectPort"];
    g_cfgGW.connectDatabase.username          = (const char*)ini["connectDatabase"]["username"];
    g_cfgGW.connectDatabase.password          = (const char*)ini["connectDatabase"]["password"];
    g_cfgGW.connectDatabase.database          = (const char*)ini["connectDatabase"]["database"];
    g_cfgGW.connectDatabase.sendQueueCapacity = (int)        ini["connectDatabase"]["sendQueueCapacity"];
    g_cfgGW.connectDatabase.recvQueueCapacity = (int)        ini["connectDatabase"]["recvQueueCapacity"];

	if (g_cfgGW.connectDatabase.database.length()>20 && g_cfgGW.connectDatabase.username.length()==0)
	{
		rt2_strcpy(g_strStaticBuffer, g_cfgGW.connectDatabase.database.c_str());
		DecodeDatabase(g_strStaticBuffer,
			g_cfgGW.connectDatabase.username,
			g_cfgGW.connectDatabase.password,
			g_cfgGW.connectDatabase.database);
	}

	// Log Database
	g_cfgGW.connectLogDb.ip					= (const char*)ini["connectLogDatabase"]["ConnectIP"];
	g_cfgGW.connectLogDb.port				= (int)        ini["connectLogDatabase"]["ConnectPort"];
	g_cfgGW.connectLogDb.username			= (const char*)ini["connectLogDatabase"]["username"];
	g_cfgGW.connectLogDb.password			= (const char*)ini["connectLogDatabase"]["password"];
	g_cfgGW.connectLogDb.database			= (const char*)ini["connectLogDatabase"]["database"];
	g_cfgGW.connectLogDb.sendQueueCapacity	= (int)        ini["connectLogDatabase"]["sendQueueCapacity"];
	g_cfgGW.connectLogDb.recvQueueCapacity	= (int)        ini["connectLogDatabase"]["recvQueueCapacity"];
	if (ini.FindEntry("connectLogDatabase", "LogTableLimit"))
	{
		g_cfgGW.LogTableLimit				= (int)		   ini["connectLogDatabase"]["LogTableLimit"];
		g_cfgGW.LogTableLimit *= 10000;
	}
	else 
	{
		g_cfgGW.LogTableLimit = (1000*10000);
	}

	if (g_cfgGW.connectLogDb.database.length()>20 && g_cfgGW.connectLogDb.username.length()==0)
	{
		rt2_strcpy(g_strStaticBuffer, g_cfgGW.connectLogDb.database.c_str());
		DecodeDatabase(g_strStaticBuffer,
			g_cfgGW.connectLogDb.username,
			g_cfgGW.connectLogDb.password,
			g_cfgGW.connectLogDb.database);
	}

	//Activity Database
	g_cfgGW.connectActDB.ip					= (const char*)ini["connectActDatabase"]["ConnectIP"];
	g_cfgGW.connectActDB.port				= (int)        ini["connectActDatabase"]["ConnectPort"];
	g_cfgGW.connectActDB.username			= (const char*)ini["connectActDatabase"]["username"];
	g_cfgGW.connectActDB.password			= (const char*)ini["connectActDatabase"]["password"];
	g_cfgGW.connectActDB.database			= (const char*)ini["connectActDatabase"]["database"];
	g_cfgGW.connectActDB.sendQueueCapacity	= (int)        ini["connectActDatabase"]["sendQueueCapacity"];
	g_cfgGW.connectActDB.recvQueueCapacity	= (int)        ini["connectActDatabase"]["recvQueueCapacity"];

	if (g_cfgGW.connectActDB.database.length()>20 && g_cfgGW.connectActDB.username.length()==0)
	{
		rt2_strcpy(g_strStaticBuffer, g_cfgGW.connectActDB.database.c_str());
		DecodeDatabase(g_strStaticBuffer,
			g_cfgGW.connectActDB.username,
			g_cfgGW.connectActDB.password,
			g_cfgGW.connectActDB.database);
	}

    // 连接Login
    g_cfgGW.connectLogin.ip       = (const char*)ini["connectLogin"]["ConnectIP"];
    g_cfgGW.connectLogin.port     = (int)        ini["connectLogin"]["ConnectPort"];
    g_cfgGW.connectLogin.bindIP   = (const char*)ini["connectLogin"]["BindIP"];
    g_cfgGW.connectLogin.bindPort = (int)        ini["connectLogin"]["BindPort"];
    g_cfgGW.lGameworldID          = (short)      ini["connectLogin"]["gameworld_id"];

	// 连接LogServer
	if (ini.FindSection("connectLogServer"))
	{
		g_cfgGW.connectLogServer.ip       = (const char*)ini["connectLogServer"]["ConnectIP"];
		g_cfgGW.connectLogServer.port     = (int)        ini["connectLogServer"]["ConnectPort"];
		g_cfgGW.connectLogServer.bindIP   = (const char*)ini["connectLogServer"]["BindIP"];
		g_cfgGW.connectLogServer.bindPort = (int)        ini["connectLogServer"]["BindPort"];
	}

    // 游戏设置
	g_cfgGW.game.szGameworldName  = (const char*)ini["game"]["DisplayName"];
    g_cfgGW.game.maxUser          = (int)        ini["game"]["maxUser"];
    g_cfgGW.game.maxRegionUser    = (int)        ini["game"]["maxRegionUser"];
    g_cfgGW.game.listType         = (int)        ini["game"]["listType"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfgGW.game.listAllowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }
	//分线系统
	g_cfgGW.LineSYS.dwRegionCount = (DWORD)		ini["LineSys"]["RegionCount"];

    return true;
}

#ifdef LINUX
#include <signal.h>
void sig_pipe(int signal)
{

}
#endif

extern void RegisterMemoryBlock(void);
extern void RegisterSharedMemProtector(void);

int main(int argc, char* argv[])
{
	system("title 这是修改的新名字");

#ifdef ENABLE_OPERATOR_NEW
	RegisterMemoryBlock();
	RegisterSharedMemProtector();
	rt2_ActiveSafestr();
#endif //ENABLE_OPERATOR_NEW

#ifdef LINUX
    signal(SIGPIPE,sig_pipe);
#endif

	signal(SIGTERM, OnExitProcess);
	signal(SIGINT, OnExitProcess);

#ifdef LINUX
    const char* szPIDFileName = "gameworld_server.pid";
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

	// R [11/22/2008 Louis.Huang]
    //rtCoreInit("gameworld_server", 0, 0, 0, 0);
	rtCoreInit("gameworld_server.ini");//tim.yang 

    {
#ifdef WIN32
        const char* szExeFileName = "gameworld_server.exe";
#else
        const char* szExeFileName = "gameworld_server";
#endif
        if (!rtMD5FileString((char*)szExeFileName, g_szExeMD5))
        {
            ERR1("ERROR: Cannot get [%s]'s MD5\n", szExeFileName);
            rt2_strcpy(g_szExeMD5, "NotFound");
        }
        if (!rtMD5FileString("gameworld_server.pak", g_szDataMD5))
        {
            ERR1("ERROR: Cannot get [%s]'s MD5\n", "gameworld_server.pak");
            rt2_strcpy(g_szDataMD5, "NotFound");
        }
    }

	g_sendCmd = RT_NEW CG_CmdPacket;
	g_recvCmd = RT_NEW CG_CmdPacket;
	g_sendCmd->SetSize(MAX_CMD_PACKET_SIZE);
	g_recvCmd->SetSize(MAX_CMD_PACKET_SIZE);

	// C [11/22/2008 Louis.Huang]
/*
    CRtPackManager* pPackManager=RT_NEW CRtPackManager;
    pPackManager->OpenPack("gameworld_server.pak",false);
    CRtPackAndAnsiManager* pAllManager=RT_NEW CRtPackAndAnsiManager(pPackManager,g_pFileManager);
    g_pFileManager=pAllManager;
*/

	//LoadConfiguration("gameworld_server.config");
    LoadConfiguration("gameworld_server.ini");

	// 字符替换程序
	std::string strErr;
	if(!LoadConfig(strErr))
	{
		ERR1("Load config error: %s\n", (char*)strErr.c_str());
		return 0;
	}
	
	InitMapString(GetConfigBoot()->GetEntry("ConfigInfo", "StrmapPath"));
	InitBannedString(R(RES_TABLE_CZSTRBANNED));

    if (g_server.Startup())
    {
        g_server.Run();
    }else
    {
        ERR1("\nStartup game world server FAILED. at %d\n", rtTimestamp());//timn.yang  %s->%d
    }

	g_server.Exit();

#ifdef LINUX
    RtCoreFile().Delete(szPIDFileName);
#endif
	//rtCorePreExit();
	rtCoreExit();	

	DEL_ONE(g_sendCmd) ;//add by fox 
	DEL_ONE(g_recvCmd) ;//add by fox


	int exitCode;

	switch(g_server.m_state)
	{
	case ssShutdown:
        exitCode = 104;
		break;

	case ssReboot:
		exitCode = 101;
		break;

    case ssExit:
        exitCode = 105;
        break;

	default:
		exitCode = -1;
		break;
	}

	return exitCode;
}
