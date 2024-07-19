#include "login.h"
#include <signal.h>
#include "license_check.h"

//--------------------------------------------------------------------
// connect db
// get cmd from recv_cmd_list
// parse cmd
// add cmd to send_cmd_list
//--------------------------------------------------------------------
void *db_thread(void *arg)
{
	unsigned long btime = rtGetMilliseconds();
	CLgsThread tlgs;
	
	tlgs.SetThreadId((int)arg);
	tlgs.Run();
	return (void*)0;
}


void *wsdl_thread(void *arg)
{
	g_pWSDL->Run();

	delete g_pWSDL;
	return (void*)0;
}

CLoginServer   *g_lgs;
TCmdList	    g_sendCmdList,g_recvCmdList;
SConfigLoginNet g_cfg;
CG_CmdPacket   *g_sendCmd;
long            g_dwLoginOrderID = 1;


bool StartupServer()
{
    g_sendCmd = new CG_CmdPacket;
    if (g_sendCmd==NULL)
        return false;
    g_sendCmd->SetSize(MAX_CMD_PACKET_SIZE);
    LOG1("cmd size = %d\n",sizeof(SListCmd));

    g_lgs = new CLoginServer;
    if (g_lgs==NULL)
        return false;

    if (!g_lgs->Startup())
    {
        ERR("StartupServer: Startup login server failed.\n");
        return false;
    }

    LOG("Login server startup ok.\n");
    return true;
}

void ShutdownServer()
{
    g_lgs->Exit();
    delete g_lgs;
    delete g_sendCmd;
}

bool RunServer()
{
    return g_lgs->Run();
}

void OnExitProcess(int sig)
{
    g_lgs->m_bNeedSendShutdown = 2;
}

#ifdef LINUX
#include <signal.h>
void sig_pipe(int signal)
{

}
#endif

int main()
{
#ifdef ENABLE_OPERATOR_NEW
	rt2_ActiveSafestr();
#endif //ENABLE_OPERATOR_NEW

#ifdef LINUX
    signal(SIGPIPE,sig_pipe);
#endif
    signal(SIGTERM, OnExitProcess);
    signal(SIGINT, OnExitProcess);

#ifdef LINUX
    const char* szPIDFileName = "login_server.pid";
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
	//rtCoreInit("login_server",NULL,NULL,NULL,NULL);
	if (!rtCoreInit("login_server.ini"))
	{
		LOG("Rt core init failed!");
		return false;
	}

#ifdef LICENSE_CHECK
	if(!LicenseCheck(0))
	{
		LOG("License Check Failed\n");
		return 101;
	}else
	{
		LOG("License Check Success\n");
	}
#endif //LICENSE_CHECK

    if (!StartupServer())
    {
        return 101; // 16 sec later reboot
    }

    RunServer();

    int iReturnResult = 101; // reboot
    if (g_lgs->m_tShutdownTime!=0)
    {
        iReturnResult = 104; // shutdown
    }

    ShutdownServer();

#ifdef LINUX
    RtCoreFile().Delete(szPIDFileName);
#endif
	// C [11/22/2008 Louis.Huang]
    //rtCorePreExit();
	rtCoreExit();

    return iReturnResult;
}
