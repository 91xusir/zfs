#include "login.h"
#include "license_check.h"
#include "LoginLogicExt.h" //PZH

static int s_iChargeTest = 0;
CWsdlWrapper* g_pWSDL = NULL;

// char* g_strStaticBuffer = new char[2048];

void SListCmd::SendCmd(CG_CmdPacket *cmd)
{
    if (pStub)
    {
        pStub->SendCmd(cmd);
    }
}

CLoginServer::CLoginServer()
    : m_dwBulletinInterval(60000) // 1*60*1000
{
	CM_MEMPROTECTOR(m_sql, MAX_SQL_SIZE)

    m_bNeedSendShutdown = 0;
    m_lShutdownForceTime = 0;
    m_tShutdownTime = 0;
    m_tShutdownDelay = 0;
    m_dwLogTime = 0;
    m_bListenGameWorld = false;
    m_bListenUpdateServer = false;
    m_bListenClientGuide = false;
    m_bListenMonitor = false;
    m_bConnectDatabase = false;
    m_bConnectChargeServer = false;
    m_bUseCharge = false;
    m_lastTestTime = 0;
    m_pNetLog = NULL;
	m_bLicenseCheck = false;
    m_lastLogoutOrder = 1;

    m_lStatus = 0xFFFFFFFF;

	m_binitdb = false;

    ResetBulletin();

    m_lLastGwsIdx = 1; // 默认从1开始

    for (int i=0; i<20; i++)
    {
        m_cmdFree.push_back(new SListCmd);
    }
	//Initdb();
}

CLoginServer::~CLoginServer()
{
    // SListCmd
    std::list<SListCmd*>::iterator it;
    for (it=m_cmdFree.begin(); it!=m_cmdFree.end(); it++)
    {
        delete (*it);
    }

    // Activing
    TNetCmdMap::iterator itP;
    for (itP=m_cmdAccountActiving.begin(); itP!=m_cmdAccountActiving.end(); itP++)
    {
        delete itP->second;
    }
    for (itP=m_cmdAccountCreating.begin(); itP!=m_cmdAccountCreating.end(); itP++)
    {
        delete itP->second;
    }

    CmdCardCanTradeDelete();
    CmdChargeCardDelete();

    // Logout
    std::list<SLogoutMsg*>::iterator itLogout;
    for (itLogout=m_listLogoutFree.begin(); itLogout!=m_listLogoutFree.end(); itLogout++)
    {
        delete *itLogout;
    }
    for (itLogout=m_listLogoutCmd.begin(); itLogout!=m_listLogoutCmd.end(); itLogout++)
    {
        delete *itLogout;
    }
}

bool CLoginServer::Run()
{
    LOG("\nLogin server is running, waiting for connections ...\n\n");

    DWORD frameTime, mNow, tickTime, upsTime, cgsTime, monTime, chkTime = 0;
    static DWORD lastTime = rtGetMilliseconds();
    bool bAllShutdowned;
    while(1)
    {
		while(!m_db.IsActive())
		{
			Initdb();
		}

        time_t tNow = time(NULL);
		mNow = rtGetMilliseconds();

		tickTime = rtGetMilliseconds();
		bAllShutdowned = Tick(mNow - lastTime);
		tickTime = rtGetMilliseconds() - tickTime;

        if (m_tShutdownTime)
        {
            if (m_tShutdownDelay && tNow>m_tShutdownDelay)
            {
                ShutdownNow();
            }

            if (tNow>m_tShutdownTime)
            {
                LOG1("\nShutdown timeout. FORCE SHUTDOWN!!! at %d\n", rtTimestamp());
                break;
            }

            // 全部退出，并且退出的命令经过计费系统的处理后
            if (bAllShutdowned && m_listLogoutCmd.size()==0)
            {
                LOG1("\nAll GWS shutdowned. Login server shutdown. at %d\n", rtTimestamp());
                break;
            }
        }
		

		upsTime = rtGetMilliseconds();
        if (m_bListenUpdateServer)
        {
            m_update.Process();
        }
		upsTime = rtGetMilliseconds() - upsTime;
        
		cgsTime = rtGetMilliseconds();
		if (m_bListenClientGuide)
        {
//            m_client80.Process();
            m_client6620.Process();
        }
		cgsTime = rtGetMilliseconds() - cgsTime;

		monTime = rtGetMilliseconds();
        if (m_bListenMonitor)
        {
            m_monitor.Process();
        }
		monTime = rtGetMilliseconds() - monTime;

#ifdef LICENSE_CHECK
		chkTime = rtGetMilliseconds();
		if(m_bLicenseCheck)
		{
			g_ChkSession.Process();
		}
		chkTime = rtGetMilliseconds() - chkTime;
#endif //LICENSE_CHECK

		if (g_pWSDL)
			g_pWSDL->tick();

        lastTime = mNow;
        frameTime = rtGetMilliseconds() - mNow;

        // give cpu time to other app
        if(frameTime < 10)
        {
            rtSleep(10);
        }
		else if(frameTime > 500)
        {
            ERR5("Run: frame time = %d,tick=%d,cgs=%d,mon=%d,chk=%d\n",frameTime,
				tickTime,cgsTime,monTime,chkTime);
        }
    }
    return true;
}

bool CLoginServer::Startup()
{
    if(!LoadConfigFile("login_server.ini"))
    {
        ERR("Startup: Read config failed\n");
        return false;
    }
    SetServerStatus(g_cfg.systemCfg.lDefaultStatus);

#ifdef WIN32
    m_lsStat.sv.UpdateMD5("login_server.exe");
#else
    m_lsStat.sv.UpdateMD5("login_server");
#endif

    // net log system
    m_pNetLog = new CG_TCPBroadcastArchive;
    if (m_pNetLog)
    {
        m_pNetLog->Start(g_cfg.listenMonitor.listenIP.c_str());
		// C [11/21/2008 Louis.Huang]
/*
        if (g_pMsg) ((RtOutputManager*)g_pMsg)->AddArchiveNode(m_pNetLog, false);
        if (g_pLog) ((RtOutputManager*)g_pLog)->AddArchiveNode(m_pNetLog, false);
        if (g_pError) ((RtOutputManager*)g_pError)->AddArchiveNode(m_pNetLog, false);
*/
    }

    /*
    stlforeach (list<SConfigGameworldNet*>, it, g_cfg.gwsList)
    {
        if ((*it)->connectLogin.bindIP=="localhost")
        {
            (*it)->connectLogin.bindIP = "127.0.0.1";
        }
        stlforeach (list<SConfigRegionNet*>, itW, (*it)->rsList)
        {
            if ((*itW)->connectGameWorld.bindIP=="localhost")
            {
                (*itW)->connectGameWorld.bindIP = "127.0.0.1";
            }
        }
    }

    g_cfg.Dump(g_pLog, "");
    */

    g_sendCmdList.SetMaxCmdNum(g_cfg.systemCfg.maxCmdNum);
    g_recvCmdList.SetMaxCmdNum(g_cfg.systemCfg.maxCmdNum);

    // Listen Game World Server
    if (g_cfg.listenGW.listenIP.length())
    {
        if(!m_gwsListen.StartListen(g_cfg.listenGW.listenIP.c_str(),g_cfg.listenGW.listenPort))
        {
            ERR2("Startup: GWS listen FAILED,ip: %s port: %d\n",
                g_cfg.listenGW.listenIP.c_str(),g_cfg.listenGW.listenPort);
            return false;
        }

        LOG2("Startup: GWS     session listen startup: ip: %s port: %d\n",
            g_cfg.listenGW.listenIP.c_str(), g_cfg.listenGW.listenPort);
        m_bListenGameWorld = true;
    }else
    {
        LOG("Startup: no game world server\n");
        m_bListenGameWorld = false;
    }

    // Listen Update Server
    if (g_cfg.listenUpdateServer.listenIP.length())
    {
        if (!m_update.Create(g_cfg.listenUpdateServer.listenIP.c_str(), 
            g_cfg.listenUpdateServer.listenPort,
            g_cfg.listenUpdateServer.capacity, 0))
        {
            ERR2("Startup: UPS session listen FAILED: ip: %s port: %d\n",
                g_cfg.listenUpdateServer.listenIP.c_str(), g_cfg.listenUpdateServer.listenPort);
            return false;
        }

        LOG2("Startup: UPS     session listen startup: ip: %s port: %d\n",
            g_cfg.listenUpdateServer.listenIP.c_str(), g_cfg.listenUpdateServer.listenPort);
        m_bListenUpdateServer = true;
    }else
    {
        LOG("Startup: no listen update server\n");
        m_bListenUpdateServer = false;
    }

    // Listen Game Client
    if (!g_cfg.listenClientGuide.listenIP.empty())
    {
        if (!m_client6620.Create(g_cfg.listenClientGuide.listenIP.c_str(), 
            g_cfg.listenClientGuide.listenPort,
            g_cfg.listenClientGuide.capacity, 
            GS_CLIENT_TIMEOUT))
        {
            ERR2("Startup: Client session listen FAILED: ip: %s port: %d\n",
                g_cfg.listenClientGuide.listenIP.c_str(), g_cfg.listenClientGuide.listenPort);
            return false;
        }
        LOG2("Startup: Client  session listen startup: ip: %s port: %d\n",
            g_cfg.listenClientGuide.listenIP.c_str(), g_cfg.listenClientGuide.listenPort);
        m_client6620.AllowAllIP();

//        if (g_cfg.listenClientGuide.listenPort!=80)
//        {
//            if (!m_client80.Create(g_cfg.listenClientGuide.listenIP.c_str(), 
//                80,
//                g_cfg.listenClientGuide.capacity, 
//                GS_CLIENT_TIMEOUT))
//            {
//                ERR2("Startup: Client session listen FAILED: ip: %s port: %d\n",
//                    g_cfg.listenClientGuide.listenIP.c_str(), 80);
//                return false;
//            }
//            LOG2("Startup: Client  session listen startup: ip: %s port: %d\n",
//                g_cfg.listenClientGuide.listenIP.c_str(), 80);
//            m_client80.AllowAllIP();
//        }
        m_bListenClientGuide = true;
    }else
    {
        LOG("Startup: no listen client guide\n");
        m_bListenClientGuide = false;
    }

    // Listen Monitor
    if (!g_cfg.listenMonitor.listenIP.empty())
    {
        if (!m_monitor.Create(g_cfg.listenMonitor.listenIP.c_str(), 
            g_cfg.listenMonitor.listenPort,
            g_cfg.listenMonitor.capacity, 0))
        {
            ERR2("Startup: Monitor session listen FAILED: ip: %s port: %d\n",
                g_cfg.listenMonitor.listenIP.c_str(), g_cfg.listenMonitor.listenPort);
            return false;
        }

        LOG2("Startup: Monitor session listen startup: ip: %s port: %d\n",
            g_cfg.listenMonitor.listenIP.c_str(), g_cfg.listenMonitor.listenPort);
        m_bListenMonitor = true;
        m_monitor.m_allowIP = g_cfg.listenMonitor.allowIP;
        m_monitor.m_allowUser.Unite(g_cfg.listenMonitor.allowUser);
    }else
    {
        LOG("Startup: no listen monitor guide\n");
        m_bListenMonitor = false;
    }

    // Connect gamigo account database agent
    if (!g_cfg.gamigoAdb.agentIP.empty())
    {
        if (m_gamigoADBSession.ConnectServer(g_cfg.gamigoAdb.agentIP.c_str(), g_cfg.gamigoAdb.agentPort,
            (g_cfg.gamigoAdb.bindIP.empty()?NULL:g_cfg.gamigoAdb.bindIP.c_str()), g_cfg.gamigoAdb.bindPort))
        {
            LOG("Startup: Gamigo account database agent server connected.\n");
        }else
        {
            ERR4("Startup: CANNOT connect Gamigo account database agent server:\n"
                "         connect ip: %s port: %d\n"
                "         bind    ip: %s port: %d\n",
                g_cfg.gamigoAdb.agentIP.c_str(), g_cfg.gamigoAdb.agentPort,
                (g_cfg.gamigoAdb.bindIP.empty()?"none":g_cfg.gamigoAdb.bindIP.c_str()), g_cfg.gamigoAdb.bindPort);
        }
    }

	// Startup libcurl
	curl_global_init(CURL_GLOBAL_ALL);

	// Start up WSDL Thread
	g_pWSDL = new CWsdlWrapper;
    RtThreadHandle _pthreadid;
	if (g_pWSDL->_init())
		RT_CREATE_THREAD(wsdl_thread,0);
	else
	{
		ERR("Start up wsdl error!\n");
	}

    // Connect billing server
    m_bUseCharge = false;
    m_bConnectChargeServer = false;
    if (g_cfg.charge.bEnable)
    {
        if (m_chargeSession.ConnectServer(g_cfg.charge.billingServerIP.c_str(), g_cfg.charge.billingServerPort,
            g_cfg.charge.strUsername.c_str(), g_cfg.charge.strPassword.c_str(),
            g_cfg.charge.bindIP.c_str(), g_cfg.charge.bindPort))
        {
            LOG("Startup: Billing server connected.\n");
            m_bUseCharge = true;
            m_bConnectChargeServer = true;
        }else
        {
            ERR4("Startup: CANNOT connect billing server:\n"
                "         connect ip: %s port: %d\n"
                "         bind    ip: %s port: %d\n",
                g_cfg.charge.billingServerIP.c_str(), g_cfg.charge.billingServerPort,
                (g_cfg.charge.bindIP.empty()?"none":g_cfg.charge.bindIP.c_str()), g_cfg.charge.bindPort);
        }
    }

    // Startup Database Thread
    if (!g_cfg.accountDB.ip.empty())
    {
        for(int i=0; i<g_cfg.systemCfg.threadNum; i++)
        {
            RT_CREATE_THREAD(db_thread,(void*)i);
            rtSleep(1000);
        }
        m_bConnectDatabase = true;
    }else
    {
        m_bConnectDatabase = false;
    }

	//LicenseCheck
	m_bLicenseCheck = true;
    return true;
}

bool CLoginServer::Shutdown(int iDelayTime, int iForceTime) // iForceTime是强制关机的时间
{
    // 设置时间
    time_t tNow = time(NULL);
    m_lShutdownForceTime = iForceTime;
    m_tShutdownDelay = tNow + iDelayTime;
    m_tShutdownTime = m_tShutdownDelay + iForceTime;

    // 拒绝用户连接，并且发送踢除所有玩家指令
    CGameWorldStub *gws;
    TGWSMap::iterator itGws;
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        gws = itGws->second;
        gws->SendAcceptLoginCmd(false);
        gws->SendKickAllUser(iDelayTime);
    }
    return true;
}

void CLoginServer::ShutdownNow()
{
    int iCount = 0;
    CGameWorldStub *gws;
    TGWSMap::iterator itGws;
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        gws = itGws->second;
        if (gws->m_stubState==CGameWorldStub::STUB_STATE_ALIVE)
        {
            iCount ++;
        }
    }
    LOG1("\nWaiting for [%d] GWS servers shutdown...\n", iCount);

    if (m_tShutdownTime==0)
    {
        m_tShutdownTime = time(NULL) + 60;
    }
    m_tShutdownDelay = 0;

    long lGameworldForceTime = m_lShutdownForceTime - 5;
    if (lGameworldForceTime<0)
    {
        lGameworldForceTime = m_lShutdownForceTime;
    }
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        gws = itGws->second;
        gws->SendAcceptLoginCmd(false);
        gws->SendShutdownCmd(lGameworldForceTime);
    }
}

void CLoginServer::Exit()
{
    LOG("\n--------------------------------------------\n");
    ERR1("Shutdown server at %d", rtTimestamp());

	// Cleanup the libcurl
	curl_global_cleanup();

    if (m_pNetLog)
    {
		// C [11/21/2008 Louis.Huang]
/*
        if (g_pMsg) ((RtOutputManager*)g_pMsg)->RemoveArchiveNode(m_pNetLog);
        if (g_pLog) ((RtOutputManager*)g_pLog)->RemoveArchiveNode(m_pNetLog);
        if (g_pError) ((RtOutputManager*)g_pError)->RemoveArchiveNode(m_pNetLog);
        delete m_pNetLog;
*/
        m_pNetLog = NULL;
    }
}

bool CLoginServer::Tick(long delta)
{
	DWORD tickTime = rtGetMilliseconds();

    if (!m_bListenGameWorld)
    {
        return true;
    }

    // net log system
    DWORD nlogTime = rtGetMilliseconds();
	if (m_pNetLog)
    {
        m_pNetLog->Process();
    }
	nlogTime = rtGetMilliseconds() - nlogTime;

	DWORD logcTime = rtGetMilliseconds();
    // log
    if ((DWORD)delta<m_dwLogTime)
    {
        m_dwLogTime -= delta;
    }else
    {
        m_dwLogTime = g_cfg.systemCfg.logCycleSecond * 1000;
        LogConcurrent();
    }
	logcTime = rtGetMilliseconds() - logcTime;

	DWORD testTime = rtGetMilliseconds();
    // Test
    if (m_lastTestTime)
    {
        time_t tt = time(NULL);
        if (tt>m_lastTestTime+5)
        {
            SendTestResult();
            m_lastTestTime = 0;
        }
    }
	testTime = rtGetMilliseconds() - testTime;

	DWORD gwsTime = rtGetMilliseconds();
    // game world server
    CG_TCPSession *link;
    link = m_gwsListen.AcceptNewSession();
    if(link)
    {
        if(!TryAddGameWorld(link))
            delete link;
    }

    std::list<long> delGws;
    CGameWorldStub* pGws;
    TGWSMap::iterator it;
    for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        if (!pGws->Tick(delta))
        {
            NOTICE("Remove GameworldStub [%s] at %d\n", pGws->m_szName.c_str(), rtTimestamp());
            delGws.push_back(it->first);
        }
    }
    while (delGws.size())
    {
        m_gwsMap.erase(delGws.front());
        delGws.pop_front();
    }
	gwsTime = rtGetMilliseconds() - gwsTime;

	DWORD bullTime = rtGetMilliseconds();
    RunBulletin();
	bullTime = rtGetMilliseconds() - bullTime;

	DWORD fmsgTime = rtGetMilliseconds();
    CmdCardCanTradeRun();
    CmdChargeCardRun();
    RunLogout();
    FlushMsg();
	fmsgTime = rtGetMilliseconds() - fmsgTime;

    time_t tNow = time(NULL);
	DWORD checkpointTime = rtGetMilliseconds();
    if (tNow-m_tLastCheckTime>10) // 10秒一次
    {
        SUserInfo *pUserInfo;
        TUserMap::iterator itUser;
        int iSn;
        bool bSendCheck;
        std::list<SUserInfo*> listTimeoutUser;
        std::list<SUserInfo*>::iterator itTimeoutUser;
        for(itUser=m_userMap.begin(); itUser!=m_userMap.end(); itUser++)
        {
            pUserInfo = (*itUser).second;
            bSendCheck = false;

            // 如果离最后一次保持在线信息时间为10分钟，就认为这个玩家已经离线，应该强制踢除
            if (pUserInfo->tLastKeepOnlineMsg && (tNow-pUserInfo->tLastKeepOnlineMsg>600))
            {
                pUserInfo->iKickCount ++;
                listTimeoutUser.push_back(pUserInfo);
            }

            // 过期时间
            /*
            if (pUserInfo->tChargeExpire)
            {
                if (tNow>pUserInfo->tChargeExpire)
                {
                    bSendCheck = true;
                }
            }
            */

            // 下一个检测时间
            if (pUserInfo->tChargeNextCheck)
            {
                if (tNow>pUserInfo->tChargeNextCheck)
                {
                    bSendCheck = true;
                }
            }

            // 发送一个检测时间
            if (m_bUseCharge && bSendCheck)
            {
                iSn = m_chargeSession.Check((short)(pUserInfo->gwsStub->m_lGameworldID), pUserInfo->info.accountName.c_str(), pUserInfo->sActorLevel);
                AddChargeCmd(iSn, pUserInfo->info.accountId, pUserInfo, tNow);
            }
        }
        // 进行踢除
        for(itTimeoutUser=listTimeoutUser.begin(); itTimeoutUser!=listTimeoutUser.end(); itTimeoutUser++)
        {
            pUserInfo = *itTimeoutUser;
            if (pUserInfo->iKickCount<5)
            {
                LOG1("KeepOnlineTimeOut: Kick account [%s]\n", pUserInfo->info.accountName.c_str());
                KickUser(pUserInfo, KICK_USER_UNKNOWN);
            }else
            {
                // 如果5次没有踢除成功，就强制从Login踢除
                LOG1("FORCE KICK: account [%s]\n", pUserInfo->info.accountName.c_str());
                UserQuit(pUserInfo->info.accountId);
            }
        }
    }
	checkpointTime = rtGetMilliseconds() - checkpointTime;

    if (m_bNeedSendShutdown)
    {
        switch (m_bNeedSendShutdown)
        {
        default:
        case 1:
            Shutdown();
            break;
        case 2:
            Shutdown(0, 30);
            break;
        }
        m_bNeedSendShutdown = 0;
    }

	DWORD chargeTime = rtGetMilliseconds();
    SListCmd* pCmd;
    if (m_bUseCharge)
    {
        tNow = time(NULL);
        m_chargeSession.ProcessNet();

        // 如果向计费服务器检查点数的命令没有在指定的时间内返回
        while (!m_listChargeCmd.empty())
        {
            if (tNow>m_listChargeCmd.front().tTimeout)
            {
                // 如果检查命令没有在指定时间内返回
                //      需要进行处理 ...

                m_listChargeCmd.pop_front();
            }else
            {
                break;
            }
        }

        // 如果向计费服务器登陆的命令没有在指定的时间内返回
        while (!m_listChargeLoginCmd.empty())
        {
            if (tNow>m_listChargeLoginCmd.front().tTimeout)
            {
                SListCmd &cmd = m_listChargeLoginCmd.front().cmd;
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_SERVER_STOP
                g_sendCmd->WriteLong(cmd.userloginRet.lSeed);
			    g_sendCmd->WriteLong(LOGIN_RET_FAILED_SERVER_STOP);
                g_sendCmd->WriteShort(LOGIN_RET_CODE_ChargeTimeout);
                cmd.SendCmd(g_sendCmd);

                m_listChargeLoginCmd.pop_front();
            }else
            {
                break;
            }
        }
    }
	chargeTime = rtGetMilliseconds() - chargeTime;

    std::list<long> listDel;
    TNetCmdMap::iterator itCmd;
	DWORD adbTime = rtGetMilliseconds();
    m_gamigoADBSession.ProcessNet();
    if (!m_cmdAccountActiving.empty())
    {
        tNow = time(NULL);
        for (itCmd=m_cmdAccountActiving.begin(); itCmd!=m_cmdAccountActiving.end(); itCmd++)
        {
            pCmd = itCmd->second;
            if (tNow-pCmd->userloginRet.t.tStepTime > g_cfg.gamigoAdb.timeoutSecond) // 指定时间还没有收到回复，就认为用户登录失败
            {
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_NOT_FOUND
                g_sendCmd->WriteLong(pCmd->userloginRet.lSeed);
                g_sendCmd->WriteLong(LOGIN_RET_FAILED_NOT_FOUND);
                g_sendCmd->WriteShort(LOGIN_RET_CODE_ActivingTimeout);
                pCmd->SendCmd(g_sendCmd);
                listDel.push_back(pCmd->userloginRet.dwLoginOrderID);
                m_cmdFree.push_back(itCmd->second);
            }
        }
        if (!listDel.empty())
        {
            std::list<long>::iterator itDel;
            for (itDel=listDel.begin(); itDel!=listDel.end(); itDel++)
            {
                m_cmdAccountActiving.erase(*itDel);
            }
        }
    }
	adbTime = rtGetMilliseconds() - adbTime;

	DWORD accTime = rtGetMilliseconds();
    // 如果创建角色的数据库语句执行超时，就认为创建不成功，退出登陆
    if (!m_cmdAccountCreating.empty())
    {
        tNow = time(NULL);
        listDel.clear();
        for (itCmd=m_cmdAccountCreating.begin(); itCmd!=m_cmdAccountCreating.end(); itCmd++)
        {
            pCmd = itCmd->second;
            if (tNow-pCmd->userloginRet.t.tStepTime > g_cfg.gamigoAdb.timeoutSecond) // 指定时间还没有收到回复，就认为用户登录失败
            {
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_NOT_FOUND
                g_sendCmd->WriteLong(pCmd->userloginRet.lSeed);
                g_sendCmd->WriteLong(LOGIN_RET_FAILED_NOT_FOUND);
                g_sendCmd->WriteShort(LOGIN_RET_CODE_CreatingTimeout);
                pCmd->SendCmd(g_sendCmd);
                listDel.push_back(pCmd->userloginRet.dwLoginOrderID);
                m_cmdFree.push_back(itCmd->second);
            }
        }
        if (!listDel.empty())
        {
            std::list<long>::iterator itDel;
            for (itDel=listDel.begin(); itDel!=listDel.end(); itDel++)
            {
                m_cmdAccountCreating.erase(*itDel);
            }
        }
    }
	accTime = rtGetMilliseconds() - accTime;

	tickTime = rtGetMilliseconds() - tickTime;
	if(tickTime > 5000)
	{
		ERR5("Tick=%d,nlog=%d,logc=%d,test=%d,gws=%d,bull=%d,fmsg=%d,abd=%d\n",
			tickTime,nlogTime,logcTime,testTime,gwsTime,bullTime,fmsgTime,adbTime);
		ERR3("cp=%d,charge=%d,acc=%d\n",checkpointTime,chargeTime,accTime);
	}

	return (m_gwsMap.size()==0);
}

void CLoginServer::ResetBulletin()
{
    m_dwBulletinTick = 0;
    m_dwBulletinLastTime = 0;
    m_dwNextUpdateBulletinCycle = 0;
    m_strBulletinContent = "";
}

void CLoginServer::StartAutoBulletin(long lDelay, const char* szContent)
{
    ResetBulletin();
    m_dwBulletinTick = lDelay*1000;
    m_strBulletinContent = szContent;
}

void CLoginServer::RunBulletin()
{
    DWORD mNow = rtGetMilliseconds();
    DWORD dwDiffTime;
    if (m_dwBulletinTick!=0)
    {
        // 如果最后一次更新的周期是0，就计算周期
        //     如果小于10秒，就每秒更新一次
        //     如果最后时间离现在还大于一次周期+10秒，就一个周期一次
        //     否则，这个周期就等于剩余时间减去10秒，这样是为了保证最后10秒有发出公告
        if (m_dwNextUpdateBulletinCycle==0)
        {
            if (m_dwBulletinTick<=10*1000) // 如果小于10秒，就每秒更新一次
            {
                m_dwNextUpdateBulletinCycle = 1000;
            }else
            {
                if (m_dwBulletinTick > m_dwBulletinInterval+10*1000)
                {
                    m_dwNextUpdateBulletinCycle = m_dwBulletinInterval;
                }else
                {
                    m_dwNextUpdateBulletinCycle = m_dwBulletinTick-10*1000;
                }
            }
        }

        // 离最后一次发出公告的时间
        if (m_dwBulletinLastTime)
        {
            dwDiffTime = mNow-m_dwBulletinLastTime;
        }else
        {
            dwDiffTime = 0;
        }

        // 如果剩余时间少于差异时间，就直接关闭
        if (m_dwBulletinTick<=dwDiffTime)
        {
            m_dwBulletinTick = 0;
            m_bNeedSendShutdown = 1;
        }else if (m_dwBulletinLastTime==0 || m_dwNextUpdateBulletinCycle<=dwDiffTime)
        {
            // 如果差异时间小于最后一次更新的周期，就发出公告
            m_dwNextUpdateBulletinCycle = 0;                // 需要重新计算周期
            m_dwBulletinLastTime = mNow;                    // 设置最后一次发出公告的时间
            m_dwBulletinTick -= dwDiffTime;                 // 设置剩余时间
            char cStyle = (m_dwBulletinTick<=10*1000)?1:0;  // 设置公告的类型

            string strTime = "  系统将于";
            long lDelay = m_dwBulletinTick;
            if (lDelay>=60000)
            {
                strTime += rtFormatNumber(lDelay/60000);
                strTime += "分钟之后关闭.";
            }else
            {
                strTime += rtFormatNumber(lDelay/1000);
                strTime += "秒之内关闭.";
            }

            string strTotal;
            switch (cStyle)
            {
            default:
            case 1:
                strTotal = strTime;
                break;
            case 0:
                strTotal = m_strBulletinContent + strTime;
                break;
            }

            ERR2("%s at %d", strTotal.c_str(), rtTimestamp());
            SendBulletinToAllGWS(strTotal.c_str(), cStyle);
        }
    }
}

bool CLoginServer::Initdb()
{
	int 	ret = m_db.Connect(
		g_cfg.accountDB.ip.c_str(),
		g_cfg.accountDB.port,
		g_cfg.accountDB.username.c_str(),
		g_cfg.accountDB.password.c_str(),
		g_cfg.accountDB.database.c_str()
		);

	if(!ret) 
	{
		ERR1("card_number connect login db failed\n");
		return false;
	}	
	else
	{
		m_binitdb = true;
		LOG1("card_number connect login db ok\n");
		return true;
	}
}

bool CLoginServer::LoadConfigFile(const char *szFilename)
{
    RtIni ini(true);
    RtString szName, szValue;
    if(!ini.OpenFile(szFilename))
    {
        ERR1("Login: open configuration file [%s] error\n", szFilename);
        return false;
    }
    ini.CloseFile();

    // 连接Account DB
    g_cfg.accountDB.ip       = (const char*)ini["connectDatabase"]["ConnectIP"];
    g_cfg.accountDB.port     = (short)      ini["connectDatabase"]["ConnectPort"];
    g_cfg.accountDB.database = (const char*)ini["connectDatabase"]["database"];
    g_cfg.accountDB.username = (const char*)ini["connectDatabase"]["username"];
    g_cfg.accountDB.password = (const char*)ini["connectDatabase"]["password"];

    if (g_cfg.accountDB.database.length()>20 && g_cfg.accountDB.username.length()==0)
    {
        strcpy(g_strStaticBuffer, g_cfg.accountDB.database.c_str());
        DecodeDatabase(g_strStaticBuffer, g_cfg.accountDB.username, g_cfg.accountDB.password, g_cfg.accountDB.database);
    }

    // 系统设置
    g_cfg.systemCfg.threadNum      = (int )ini["system"]["thread_num"];
    g_cfg.systemCfg.maxCmdNum      = (int )ini["system"]["max_cmd"];
    g_cfg.systemCfg.logCycleSecond = (int )ini["system"]["logCycleSecond"];
    g_cfg.systemCfg.lDefaultStatus = (long)ini["system"]["defaultstatus"];
    if (ini.GetEntry("system", "name"))
    {
        m_strLoginName = (const char*)ini["system"]["name"];
    }else
    {
        m_strLoginName = g_cfg.accountDB.ip;
    }

    // 连接自动激活服务器
    if (ini.GetEntry("connectActivation", "ConnectIP"))
    {
        g_cfg.gamigoAdb.agentIP       = (const char*)ini["connectActivation"]["ConnectIP"];
        g_cfg.gamigoAdb.agentPort     = (int        )ini["connectActivation"]["ConnectPort"];
        g_cfg.gamigoAdb.bindIP        = (const char*)ini["connectActivation"]["BindIP"];
        g_cfg.gamigoAdb.bindPort      = (int        )ini["connectActivation"]["BindPort"];
        g_cfg.gamigoAdb.timeoutSecond = (int        )ini["connectActivation"]["timeoutSecond"];
    }else
    {
        g_cfg.gamigoAdb.agentIP = "";
    }

    // 连接收费服务器
    if (ini.GetEntry("connectBilling", "Enable"))
    {
        g_cfg.charge.bEnable            = (int        )ini["connectBilling"]["Enable"];
        g_cfg.charge.billingServerIP    = (const char*)ini["connectBilling"]["ConnectIP"];
        g_cfg.charge.billingServerPort  = (int        )ini["connectBilling"]["ConnectPort"];
        g_cfg.charge.bindIP             = (const char*)ini["connectBilling"]["BindIP"];
        g_cfg.charge.bindPort           = (int        )ini["connectBilling"]["BindPort"];
        g_cfg.charge.timeoutSecond      = (int        )ini["connectBilling"]["timeoutSecond"];
        g_cfg.charge.strUsername        = (const char*)ini["connectBilling"]["Username"];
        g_cfg.charge.strPassword        = (const char*)ini["connectBilling"]["Password"];
    }else
    {
        g_cfg.charge.bEnable            = false;
    }

    if (ini.GetEntry("connectBilling", "Test"))
    {
        s_iChargeTest                   = (int        )ini["connectBilling"]["Test"];
    }else
    {
        s_iChargeTest                   = 0;
    }

    // 侦听Game World Server
    g_cfg.listenGW.listenIP   = (const char*)ini["listenGameworld"]["ListenIP"];
    g_cfg.listenGW.listenPort = (int        )ini["listenGameworld"]["ListenPort"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfg.listenGW.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    // 侦听 客户端的 Guide 连接
    g_cfg.listenClientGuide.listenIP        = (const char*)ini["listenGuide"]["ListenIP"];
    g_cfg.listenClientGuide.listenPort      = (int        )ini["listenGuide"]["ListenPort"];
    g_cfg.listenClientGuide.capacity        = (int        )ini["listenGuide"]["capacity"];
    g_cfg.listenClientGuide.refreshInterval = (float      )ini["listenGuide"]["refreshInterval"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfg.listenClientGuide.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    // 侦听 监控连接
    g_cfg.listenMonitor.listenIP        = (const char*)ini["listenMonitor"]["ListenIP"];
    g_cfg.listenMonitor.listenPort      = (int        )ini["listenMonitor"]["ListenPort"];
    g_cfg.listenMonitor.capacity        = (int        )ini["listenMonitor"]["capacity"];
    g_cfg.listenMonitor.refreshInterval = (float      )ini["listenMonitor"]["refreshInterval"];
	g_cfg.listenMonitor.peopleRate		= (float        )ini["listenMonitor"]["PeopleRate"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfg.listenMonitor.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="user")
            {
                g_cfg.listenMonitor.allowUser.AddUserFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    // 侦听 Update Server
    g_cfg.listenUpdateServer.listenIP   = (const char*)ini["listenUpdateserver"]["ListenIP"];
    g_cfg.listenUpdateServer.listenPort = (int        )ini["listenUpdateserver"]["ListenPort"];
    g_cfg.listenUpdateServer.capacity   = (int        )ini["listenUpdateserver"]["capacity"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfg.listenUpdateServer.allowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }

    ini["gmAllowIP"];
    if (ini.FirstEntry(&szName, &szValue))
    {
        do
        {
            if (szName=="allow")
            {
                g_cfg.gmAllowIP.AddIPFromStr(szValue);
            }
        } while(ini.NextEntry(&szName, &szValue));
    }
    LOG("Load configuration succeeded\n\n");
    return true;
}

bool CLoginServer::TryAddGameWorld(CG_TCPSession *link)
{
    char ip[40];
    short port;

    if(!link->GetSocket().GetRemoteAddr(ip, &port))
    {
        ERR2("add game world failed (cannot get remote address), ip=%s port=%d\n", ip, port);
        return false;
    }

    if (strcmp(ip, "localhost")==0 || strcmp(ip, "127.0.0.1")==0)
    {
        strcpy(ip, gethostbyname("localhost")->h_name);
    }

    if (g_cfg.listenGW.allowIP.CheckAllow(ip, true))
    {
        CGameWorldStub* pGws = new CGameWorldStub;
        pGws->m_lStubIdx = m_lLastGwsIdx++;
        m_gwsMap[pGws->m_lStubIdx] = pGws;

		link->SetBufSize(1024);
		link->GetSocket().SetSendBufferSize(256*1024);
		pGws->SetNetLink(link);
        pGws->m_szGameworldIP = ip;
        pGws->m_lGameworldPort = port;
        NOTICE("add game world succeeded, ip=%s port=%d\n",ip, port);
        return true;
    }
    ERR2("add game world failed (IP denied), ip=%s port=%d\n", ip, port);
    return false;
}

void CLoginServer::AddUser(SUserInfo *user,CGameWorldStub *stub)
{
    // LOG1("user [%s] add ok.\n",user->info.accountName.c_str());
    // m_userMap[user->info.accountName] = user;
	m_userMap[user->info.accountId] = user;
    user->gwsStub = stub;
    user->gwsStub->OnAddUser();
    user->info.beginTime = time(NULL);
    user->state   = USER_STATE_ALIVE;
}

SUserInfo *CLoginServer::FindUser(DWORD id)
{
    TUserMap::iterator it;
    it = m_userMap.find(id);
    if(it==m_userMap.end()) return NULL;
    return (*it).second;
}

SUserInfo* CLoginServer::FindUserByName(const char* szAccountName)
{
    SUserInfo *pUser;
    TUserMap::iterator it;
    for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
    {
        pUser = (*it).second;
        if (strcmp(pUser->info.accountName.c_str(), szAccountName)==0)
        {
            return pUser;
        }
    }
    return NULL;
}

void CLoginServer::UserQuit(DWORD dwAccountID)
{
    SUserInfo *pUser = FindUser(dwAccountID);
    if (pUser)
    {
        short sServerID = (short)(pUser->gwsStub->m_lGameworldID);

        pUser->gwsStub->OnRemoveUser();
        m_userMap.erase(dwAccountID);
        pUser->gwsStub = NULL;

        if (m_listLogoutFree.size()==0)
        {
            int i;
            for (i=0; i<20; i++)
            {
                m_listLogoutFree.push_back(new SLogoutMsg);
            }
        }
        SLogoutMsg* pLogoutMsg = m_listLogoutFree.front();
        m_listLogoutFree.pop_front();
        m_listLogoutCmd.push_back(pLogoutMsg);

        pLogoutMsg->lLogoutOrder    = m_lastLogoutOrder++;
        pLogoutMsg->dwAccountID     = pUser->info.accountId;
        pLogoutMsg->dwLoginIP       = inet_addr(pUser->info.ip.c_str());
        pLogoutMsg->dwEnterTime     = pUser->info.beginTime;
        pLogoutMsg->dwLeaveTime     = (DWORD)time(NULL);
        pLogoutMsg->fPoint          = 0.f;
        pLogoutMsg->sServer         = -1;

        // 发送给计费服务器说登出
        if (m_bUseCharge)
        {
            m_chargeSession.Logout(sServerID, pUser->info.accountName.c_str(),
                pUser->info.accountId, pLogoutMsg->lLogoutOrder );
        }

	    MSG2("user [%s:%d] quit\n",pUser->info.accountName.c_str(), pUser->info.accountId);

        delete pUser;
    }
}

void CLoginServer::LogLogoutInfo(short sServer, long lAccountID, float fPoint, long lLogoutOrderID)
{
    SLogoutMsg* pLogoutMsg;
    std::list<SLogoutMsg*>::iterator it;
    for (it=m_listLogoutCmd.begin(); it!=m_listLogoutCmd.end(); it++)
    {
        pLogoutMsg = *it;
        if (pLogoutMsg->lLogoutOrder==lLogoutOrderID)
        {
            pLogoutMsg->fPoint = fPoint;
            pLogoutMsg->sServer = sServer;
            break;
        }
    }
    if (it==m_listLogoutCmd.end())
    {
        SListCmd newCmd(NULL, LIST_CMD_USER_QUIT);
        newCmd.accountInfo.accountId        = lAccountID;
        newCmd.accountInfo.beginTime        = (long)time(NULL);
        newCmd.userLogout.tLogoutTime       = newCmd.accountInfo.beginTime;
        newCmd.userLogout.dwIP              = 0;
        newCmd.userLogout.fPoint            = fPoint;
        newCmd.userLogout.sServer           = sServer;
	    if (!g_recvCmdList.AddCmd(&newCmd))
        {
            ERR("ERROR: Add Quit cmd failed, [g_recvCmdList.AddCmd 1]\n");
        }
    }
}

void CLoginServer::RunLogout()
{
    DWORD tNow = (DWORD)time(NULL);
    DWORD tTimeout = tNow-6; // 6 秒内没有返回就认为失败，写入数据库
    SLogoutMsg* pLogoutMsg;
    SListCmd newCmd(NULL, LIST_CMD_USER_QUIT);
    std::list<SLogoutMsg*>::iterator it;
    for (it=m_listLogoutCmd.begin(); it!=m_listLogoutCmd.end();)
    {
        pLogoutMsg = *it;
        if (pLogoutMsg->dwLeaveTime < tTimeout)
        {
            newCmd.accountInfo.accountId        = pLogoutMsg->dwAccountID;
            newCmd.accountInfo.beginTime        = pLogoutMsg->dwEnterTime;
            newCmd.userLogout.tLogoutTime       = tNow;
            newCmd.userLogout.dwIP              = pLogoutMsg->dwLoginIP;
            newCmd.userLogout.fPoint            = pLogoutMsg->fPoint;
            newCmd.userLogout.sServer           = pLogoutMsg->sServer;
	        if (!g_recvCmdList.AddCmd(&newCmd))
            {
                ERR("ERROR: Add Quit cmd failed, [g_recvCmdList.AddCmd 2]\n");
            }

            m_listLogoutFree.push_back(pLogoutMsg);
            it = m_listLogoutCmd.erase(it);
        }
        break;
    }
}

void CLoginServer::FlushMsg()
{
    SListCmd cmd(NULL, 0);
    time_t tNow = time(NULL);
    while(g_sendCmdList.GetCmd(&cmd))
    {
        switch(cmd.cmdId)
        {
        ///////////////////////////////////////////////////////////////////////////////////////
        case LIST_CMD_USER_LOGIN_RET:
            if(cmd.pStub)
            {
                if(cmd.userloginRet.lRet == LOGIN_RET_SUCCESS)
                {
                    if (m_bUseCharge)
                    {
                        // 发送给计费服务器，说要登录
                        long lSn = m_chargeSession.Login((short)(cmd.pStub->m_lGameworldID), cmd.accountInfo.accountName.c_str(), cmd.accountInfo.ip.c_str());
                        m_listChargeLoginCmd.push_back(SChargeLoginCmd());
                        SChargeLoginCmd* pNewCmd = &(m_listChargeLoginCmd.back());
                        pNewCmd->lSn = lSn;
                        pNewCmd->cmd = cmd;
                        pNewCmd->tTimeout = tNow + 5;
                    }else
                    {
                        TryAddUser(&cmd, 1, cmd.accountInfo.totalPot, cmd.accountInfo.isVip);
                    }
                    continue;
                }else if ( (cmd.userloginRet.lRet==LOGIN_RET_FAILED_NOT_FOUND || cmd.userloginRet.lRet==LOGIN_RET_FAILED_PWD_WRONG)
                        && cmd.userloginRet.t.iStep==0)
                {
                    if (m_bUseCharge)
                    {
                        if (m_cmdFree.size()==0)
                        {
                            for (int i=0; i<100; i++)
                            {
                                m_cmdFree.push_back(new SListCmd);
                            }
                            LOG1("account activation cmd cnt = %d\n", (int)(m_cmdAccountActiving.size()+m_cmdFree.size()));
                        }
                        SListCmd* pNewCmd = m_cmdFree.front();
                        m_cmdFree.pop_front();
#ifdef _DEBUG
                        CHECK(m_cmdAccountActiving.find(cmd.userloginRet.dwLoginOrderID)==m_cmdAccountActiving.end());
#endif
                        m_cmdAccountActiving[cmd.userloginRet.dwLoginOrderID] = pNewCmd;
                        *pNewCmd = cmd;
                        pNewCmd->userloginRet.t.iStep = 1;
                        pNewCmd->userloginRet.t.tStepTime = tNow;

                        char cType;
                        if (cmd.userloginRet.lRet==LOGIN_RET_FAILED_PWD_WRONG)
                        {
                            cType = 1;
                        }else
                        {
                            cType = 0;
                        }
                        m_chargeSession.AccountActive(
                            pNewCmd->userloginRet.dwLoginOrderID,
                            pNewCmd->accountInfo.accountName.c_str(),
                            pNewCmd->accountInfo.password.c_str(),
                            cType);
                        continue;
                    }else
                    {
                        if (m_gamigoADBSession.IsConnected())
                        {
                            if (m_cmdFree.size()==0)
                            {
                                for (int i=0; i<100; i++)
                                {
                                    m_cmdFree.push_back(new SListCmd);
                                }
                                LOG1("Gamigo agent cmd cnt = %d\n", (int)(m_cmdAccountActiving.size()+m_cmdFree.size()));
                            }
                            SListCmd* pNewCmd = m_cmdFree.front();
                            m_cmdFree.pop_front();
#ifdef _DEBUG
                            CHECK(m_cmdAccountActiving.find(cmd.userloginRet.dwLoginOrderID)==m_cmdAccountActiving.end());
#endif
                            m_cmdAccountActiving[cmd.userloginRet.dwLoginOrderID] = pNewCmd;
                            *pNewCmd = cmd;
                            pNewCmd->userloginRet.t.iStep = 1;
                            pNewCmd->userloginRet.t.tStepTime = tNow;
                            m_gamigoADBSession.SendCmdLogin(pNewCmd->accountInfo.accountName.c_str(), pNewCmd->accountInfo.password.c_str(), pNewCmd->userloginRet.dwLoginOrderID);
                            continue;
                        }
                    }
                }
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(lgs2gws_user_login_ret); // 非 LOGIN_RET_SUCCESS
                g_sendCmd->WriteLong(cmd.userloginRet.lSeed);
			    g_sendCmd->WriteLong(cmd.userloginRet.lRet);
                g_sendCmd->WriteShort(cmd.userloginRet.sRetCode);
                cmd.SendCmd(g_sendCmd);
                switch (cmd.userloginRet.lRet)
                {
                case LOGIN_RET_FAILED_USER_ONLINE:      MSG1("user [%s] login failed, user online\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_ACCOUNT_STOP:     MSG1("user [%s] login failed, account stop\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_ACCOUNT_PAUSE:    MSG1("user [%s] login failed, account pause\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_POT_NOT_ENOUGH:   MSG1("user [%s] login failed, not enough\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_PWD_WRONG:        MSG1("user [%s] login failed, password wrong\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_NOT_FOUND:        MSG1("user [%s] login failed, not found\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_SERVER_FULL:      MSG1("user [%s] login failed, server full\n", cmd.accountInfo.accountName.c_str()); break;
                case LOGIN_RET_FAILED_SERVER_STOP:      MSG1("user [%s] login failed, server stop\n", cmd.accountInfo.accountName.c_str()); break;
				case LOGIN_RET_FAILED_PUBWIN_CENTER:    MSG1("user [%s] login failed, query pubwin center failed\n", cmd.accountInfo.accountName.c_str()); break;//  [8/25/2009 tim.yang] 同步pubwin失败
				case LOGIN_RET_ACCOUNT_IS_LOCKER:		MSG1("user [%s] login failed, user account is locked\n", cmd.accountInfo.accountName.c_str()); break;//  [8/25/2009 tim.yang] 玩家帐号被锁
                default:
                case LOGIN_RET_FAILED_UNKNOWN:          MSG1("user [%s] login failed, unknown\n", cmd.accountInfo.accountName.c_str()); break;
                }
            }
            break;

        ///////////////////////////////////////////////////////////////////////////////////////
        case LIST_CMD_ACCOUNT_CREATE_RET:
            OnCreateAccountResult(&cmd);
            break;

        case LIST_CMD_RET:
            if (cmd.cmdRet.bShowMessage)
            {
                if (cmd.cmdRet.lShowMessageAccount && cmd.pStub)
                {
                    cmd.pStub->SendMessageToUser(cmd.cmdRet.lShowMessageAccount, 0, cmd.cmdRet.szInfo);
                }
            }
            break;
		case LIST_CMD_USER_POINT_RET:
			OnDeduct(cmd.chargePoint.lAccountID,cmd.chargePoint.sServer, cmd.chargePoint.lAccountID, cmd.chargePoint.lUserID, cmd.chargePoint.sPoint,
						cmd.chargePoint.sCurPoint,cmd.chargePoint.cResult, cmd.chargePoint.lDeductOrderID);
			break;
		case LIST_CMD_PRE_USER_POINT_RET:
			OnPreDeduct(cmd.chargePoint.sServer,cmd.chargePoint.lAccountID,cmd.chargePoint.lUserID,cmd.chargePoint.sPoint,cmd.chargePoint.sCurPoint,
						cmd.chargePoint.cResult,cmd.chargePoint.bSucceed,cmd.chargePoint.lDeductOrderID,cmd.chargePoint.event,cmd.chargePoint.lAuctionId);
			break;
		case LIST_CMD_CHARGE_CARD_RET:
			GiftErrorRet(cmd.chargePoint.sServer,cmd.chargePoint.lUserID,cmd.chargePoint.szCardID,cmd.chargePoint.cResult,cmd.chargePoint.param,
						cmd.chargePoint.bSucceed);
			break;
		//tim.yang  VIP卡
		case LIST_CMD_ADD_ACCOUNT_TIME_RET:
			AddCardTimeRet(cmd.GWSId,cmd.userId,cmd.isVIPCardAddTimeRight,cmd.VIPTime);
			break;
		//end
		case LIST_CMD_ADD_GIFT_RET:
			AddGift(1,cmd.chargePoint.szUserName,cmd.chargePoint.sServer,cmd.chargePoint.lUserID,cmd.chargePoint.sUserLev,cmd.chargePoint.cCardType,
					cmd.chargePoint.cResult,cmd.chargePoint.tTime,cmd.chargePoint.lDeductOrderID,cmd.chargePoint.szCardID);
			break;
		case LIST_CMD_CARD_TRADE_RET:
			OnCardCanTradeResult(cmd.chargePoint.param,cmd.chargePoint.sServer,cmd.chargePoint.lUserID,cmd.chargePoint.szCardID,cmd.chargePoint.bSucceed,
					cmd.chargePoint.cResult,cmd.chargePoint.lCardGroupType);
			break;
		case LIST_CMD_ADD_POINT_RET:
			AddPointRet(cmd.chargePoint.sServer,cmd.chargePoint.lAccountID,cmd.chargePoint.lUserID,cmd.chargePoint.sPoint,cmd.chargePoint.cResult,
					cmd.chargePoint.bSucceed);
			break;
        }
    }
}

void CLoginServer::OnCreateAccountResult(SListCmd* pResultCmd)
{
    SListCmd* pCmd;
    TNetCmdMap::iterator it = m_cmdAccountCreating.find(pResultCmd->accountCreateRet.lLoginOrderID);
    if (it!=m_cmdAccountCreating.end())
    {
        pCmd = it->second;
        if (pResultCmd->accountCreateRet.bSucceed) // 成功，重新丢入队列中进行登陆
        {
            long lSeed = pCmd->userloginRet.lSeed;

            pCmd->cmdId = LIST_CMD_USER_LOGIN;
            pCmd->userLogin.t.iStep = 3; // 为了避免逻辑死循环，这个不能为0
            pCmd->userLogin.dwLoginOrderID = pResultCmd->accountCreateRet.lLoginOrderID;
            pCmd->userLogin.lSeed = lSeed;
            strcpy(pCmd->userLogin.szUsername, pCmd->accountInfo.accountName.c_str());
            strcpy(pCmd->userLogin.szPassword, pCmd->accountInfo.password.c_str());
            strcpy(pCmd->userLogin.szUserIP, pCmd->accountInfo.ip.c_str());
            if (!g_recvCmdList.AddCmd(pCmd))
            {
                ERR("ERROR: Add gamigo Login cmd ret failed\n");
            }
        }else // 失败
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_NOT_FOUND
            g_sendCmd->WriteLong(pCmd->userloginRet.lSeed);
            g_sendCmd->WriteLong(LOGIN_RET_FAILED_NOT_FOUND);
            g_sendCmd->WriteShort(LOGIN_RET_CODE_CreatingFailed);
            pCmd->SendCmd(g_sendCmd);
        }
        m_cmdFree.push_back(it->second);
        m_cmdAccountCreating.erase(it);
    }
}

void CLoginServer::OnGamigoAdbLoginCmd(int iParam, const char** pParam)
{
    if (iParam==2)
    {
        short sRetCode = 0;
        unsigned long dwOrderID = atol(pParam[1]);
        TNetCmdMap::iterator it = m_cmdAccountActiving.find(dwOrderID);
        if (it!=m_cmdAccountActiving.end())
        {
            long lRetUser = LOGIN_RET_SUCCESS;
            SListCmd* pCmd = it->second;
            if (strcmp(pParam[0], "Fail")==0) // 失败
            {
                lRetUser = LOGIN_RET_FAILED_NOT_FOUND;
                sRetCode = LOGIN_RET_CODE_ActivingFailed;
            }else if (strcmp(pParam[0], "Member")==0) // 成功，重新丢入队列中进行登陆
            {
                pCmd->cmdId = LIST_CMD_USER_LOGIN;
                if (!g_recvCmdList.AddCmd(pCmd))
                {
                    ERR("ERROR: Add gamigo Login cmd ret failed\n");
                }
            }else if (strcmp(pParam[0], "NewCard")==0) // 新手卡
            {
                lRetUser = LOGIN_RET_FAILED_NEW_CARD;
                sRetCode = LOGIN_RET_CODE_NewCard;
            }else // 未知失败
            {
                lRetUser = LOGIN_RET_FAILED_NOT_FOUND;
                sRetCode = LOGIN_RET_CODE_Unknown;
            }
            if (lRetUser!=LOGIN_RET_SUCCESS)
            {
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(lgs2gws_user_login_ret); // 非 LOGIN_RET_SUCCESS
                g_sendCmd->WriteLong(pCmd->userloginRet.lSeed);
                g_sendCmd->WriteLong(lRetUser);
                g_sendCmd->WriteShort(sRetCode);
                pCmd->SendCmd(g_sendCmd);
            }
            m_cmdFree.push_back(it->second);
            m_cmdAccountActiving.erase(it);
        }
    }
}

void CLoginServer::OnActiveResult(char cResult, long lOrderID)
{
    long lRetUser;
    short sRetCode = 0;
    SListCmd* pCmd;
    TNetCmdMap::iterator it = m_cmdAccountActiving.find(lOrderID);
    if (it!=m_cmdAccountActiving.end())
    {
        lRetUser = LOGIN_RET_SUCCESS;
        pCmd = it->second;
        if (cResult==0) // 失败
        {
            lRetUser = LOGIN_RET_FAILED_NOT_FOUND;
            sRetCode = LOGIN_RET_CODE_ActivingFailed;
        }else if (cResult==1 || cResult==3) // 1 会员, 3 密码错误
        {
            pCmd->userloginRet.t.tStepTime = time(NULL);
            m_cmdAccountCreating[lOrderID] = pCmd;
            m_cmdAccountActiving.erase(it);
            return;
        }else if (cResult==2) // 新手卡
        {
            lRetUser = LOGIN_RET_FAILED_NEW_CARD;
            sRetCode = LOGIN_RET_CODE_NewCard;
        }else // 未知失败
        {
            lRetUser = LOGIN_RET_FAILED_NOT_FOUND;
            sRetCode = LOGIN_RET_CODE_Unknown;
        }
        if (lRetUser!=LOGIN_RET_SUCCESS)
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_user_login_ret); // 非 LOGIN_RET_SUCCESS
            g_sendCmd->WriteLong(pCmd->userloginRet.lSeed);
            g_sendCmd->WriteLong(lRetUser);
            g_sendCmd->WriteShort(sRetCode);
            pCmd->SendCmd(g_sendCmd);
        }
        m_cmdFree.push_back(it->second);
        m_cmdAccountActiving.erase(it);
    }
}

SUserInfo *CLoginServer::TryAddUser(SListCmd *cmd, char cPointType, long lPoint, char cVIP)
{
    bool ret = true;
	/*PZH*/
	if(cmd->userLogin.bPowerLogin)//处理强制登陆
	{
		CLoginLogicExt::GetCLoginLogicExt().DeleteOldLogin(cmd->accountInfo.accountId);
	}
    /**/
    SUserInfo *old = NULL;
    old = g_lgs->FindUser(cmd->accountInfo.accountId);

    if(old)
    {
        if(old->gwsStub == NULL)
        {
            MSG1("old user svr stub = NULL,name = %s",old->info.accountName.c_str());
            //----------add start by tony 06.04.10-----------------------------//
			//-reason:卡号，因为总是提示“此账号已存在”，原因是在这个分支里发现
			//-       玩家在loginserver却找不到gws
			m_userMap.erase(old->info.accountId);
			//----------add end   by tony 06.04.10-----------------------------//
			//ret = false;
        }
		//else if(old->gwsStub->m_stubState == CGameWorldStub::STUB_STATE_ALIVE)
        //{
        //    // try kick current user
        //    KickUser(old, KICK_USER_MULTI_LOGIN);
        //    ret = false;
        //}else
        //{
        //    MSG1("old user svr stub dead, replace it,name = %s",old->info.accountName.c_str());

        //    old->gwsStub->OnRemoveUser();
        //    m_userMap.erase(old->info.accountId);
        //    delete old;
        //}
		ret = false;
    }

    if (ret)
    {
        SUserInfo *user = new SUserInfo;
        user->info = cmd->accountInfo;
        AddUser(user, cmd->pStub);
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_user_login_ret);// LOGIN_RET_SUCCESS
        g_sendCmd->WriteLong(cmd->userloginRet.lSeed);
        g_sendCmd->WriteLong(LOGIN_RET_SUCCESS);
        *g_sendCmd << user->info;
        g_sendCmd->WriteByte(cPointType);
        g_sendCmd->WriteLong(lPoint);
        g_sendCmd->WriteByte(cVIP);
        cmd->SendCmd(g_sendCmd);
#ifdef _DEBUG
		MSG2("user [%s:%d] login ok\n",cmd->accountInfo.accountName.c_str(), cmd->accountInfo.accountId);
#endif
        return user;
    }else
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_USER_ONLINE
        g_sendCmd->WriteLong(cmd->userloginRet.lSeed);
        g_sendCmd->WriteLong(LOGIN_RET_FAILED_USER_ONLINE);
        g_sendCmd->WriteShort(LOGIN_RET_CODE_UserOnlined);
        cmd->SendCmd(g_sendCmd);
        MSG1("user [%s] login failed,user online\n",cmd->accountInfo.accountName.c_str());
        return NULL;
    }
}

void CLoginServer::KickUser(SUserInfo *user, char cReason, char* szReason)
{
    if(!user->gwsStub) return;

    g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(lgs2gws_kick_user);
	g_sendCmd->WriteLong(user->info.accountId);
    g_sendCmd->WriteByte(cReason);
    if (cReason==KICK_USER_USERDEFINE)
    {
        g_sendCmd->WriteString(szReason);
    }
    user->gwsStub->SendCmd(g_sendCmd);
}

void CLoginServer::KickUserByName(const char* szAccountName, char cReason, char* szReason)
{
    TUserMap::iterator it;
    for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
    {
        SUserInfo *user = (*it).second;
        if (strcmp(user->info.accountName.c_str(), szAccountName)==0)
        {
            KickUser(user, cReason, szReason);
            return;
        }
    }
}

void CLoginServer::GameWorldNetDead(CGameWorldStub *stub)
{
    TUserMap::iterator it;
    for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
    {
        SUserInfo *user = (*it).second;
        if(user->gwsStub == stub)
        {
            user->state = USER_STATE_GWS_LOST;
        }
    }
}

void CLoginServer::RemoveAllUser(CGameWorldStub *stub)
{
    SUserInfo *user;
    std::list<DWORD> listRemoveUser;
    std::list<DWORD>::iterator itList;
    TUserMap::iterator it;

    for(it=m_userMap.begin(); it!=m_userMap.end(); it++)
    {
        user = (*it).second;
        if(user->gwsStub == stub)
        {
            listRemoveUser.push_back((*it).first);
        }
    }

    for (itList=listRemoveUser.begin(); itList!=listRemoveUser.end(); itList++)
    {
        UserQuit(*itList);
    }
}

void CLoginServer::LogConcurrent()
{
    int i;
	CGameWorldStub* pGws;
	TGWSMap::iterator it;
    SListCmd newCmd(NULL, LIST_CMD_LOG_CONCURRENT);
	long lSize = sizeof(SListCmd);

	for(i=0; i<8; i++)
	{
		newCmd.gwConcurrent[i] = 0;
	}
    for(it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        if (pGws->m_stubState==CGameWorldStub::STUB_STATE_ALIVE)
        {
            if (pGws->m_lGameworldID>=0 && pGws->m_lGameworldID<8)
            {
                newCmd.gwConcurrent[pGws->m_lGameworldID] = pGws->m_userCount;
            }
			//newCmd.gwConcurrent[i] = pGws->m_userCount;
        }
    }
    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::SendUpdateServerList(GSClientSessionManager* pClient)
{
    if (m_lStatus&lssUpdateList) // 如果允许更新就发送列表
    {
        CG_CmdPacket& cmd = pClient->BeginCmd(gs2c_us_list);

        cmd.WriteShort((short)m_update.m_sessions.size());

        GSUpdateSessionManager::SessionList::iterator se;
        for(se=m_update.m_sessions.begin(); se!=m_update.m_sessions.end(); se++)
        {
            cmd.WriteString((char*)se->host.c_str());
            cmd.WriteShort(se->port);
            cmd.WriteByte(se->cEvaluation);
        }

        /*
        cmd.WriteByte(1); // 数据版本1

        if (m_update.m_sessions.size())
        {
            cmd.WriteByte(1); // 个数
            // 发送第一个Update Server的MD5值给客户端
            se=m_update.m_sessions.begin();
            cmd.WriteString((char*)se->strDataMD5.c_str());
        }else
        {
            cmd.WriteByte(0); // 个数
        }
        */

        if (m_update.m_sessions.size())
        {
            cmd.WriteByte(2); // 数据版本2
            cmd.WriteByte(0); // count
            se=m_update.m_sessions.begin();

            cmd.WriteLong((long)(se->listFiles.size()));
            std::list<std::string>::iterator itFiles;
            for (itFiles=se->listFiles.begin(); itFiles!=se->listFiles.end(); itFiles++)
            {
                cmd.WriteString((char*)(*itFiles).c_str());
            }
            cmd.WriteString((char*)se->strDataMD5.c_str()); // 发送第一个Update Server的MD5值给客户端
        }else
        {
            cmd.WriteByte(0); // 没有服务器
            cmd.WriteByte(0); // count
        }
        pClient->SendCmd();
    }else
    {
        CG_CmdPacket& cmd = pClient->BeginCmd(gs2c_us_info);
		cmd.WriteByte(1); //不允许客户端连接 
        pClient->SendCmd();
    }
}

/*
void CLoginServer::SendGWServerList()
{
    if (m_lStatus&(lssLoginNormal|lssLoginGM)) // 如果允许登陆就发送列表
    {
        CG_CmdPacket& cmd = m_client.BeginCmd(gs2c_gws_list);

        cmd.WriteShort((short)g_cfg.listenClientGuide.refreshInterval);
        cmd.WriteShort((short)m_gwsList.size());

        CGameWorldStub* pGW;
        stlforeach(vector<CGameWorldStub*>, info, m_gwsList)
        {
            pGW = *info;
            cmd.WriteString((char*)pGW->m_pConfig->name.c_str());
            cmd.WriteString((char*)pGW->m_listenClientIP.c_str());
            cmd.WriteShort((short)(pGW->m_pConfig->listenClient.listenPort));
            if (pGW->m_stubState==CGameWorldStub::STUB_STATE_ALIVE && pGW->m_gwState==gwssAcceptLogin)
            {
                cmd.WriteByte(pGW->m_evaluation);
            }else
            {
                cmd.WriteByte(0);
            }
        }

        m_client.SendCmd();
    }else
    {
        CG_CmdPacket& cmd = m_client.BeginCmd(gs2c_gws_info);
        m_client.SendCmd();
    }
}
*/

void CLoginServer::SendServerInfoToMoniter(GSClientSessionManager* pClientSessionManager)
{
    CGameWorldStub* pGW;
    CG_CmdPacket& cmd = pClientSessionManager->BeginCmd(gs2m_update_info);

    short sVersion = SERVER_STAT_VERSION;

    cmd.WriteShort(sVersion);

    m_lsStat.lUserCount = (int)m_userMap.size();
    m_lsStat.lState = m_lStatus;
    //m_lsStat.sv.UpdateInfo();

    m_lsStat.Serialize(&cmd, SERVER_STAT_VERSION);

    // Update Server
    cmd.WriteShort((short)m_update.m_sessions.size());
    stlforeach(GSUpdateSessionManager::SessionList, seUpdate, m_update.m_sessions)
    {
        cmd.WriteString((char*)seUpdate->host.c_str());
        cmd.WriteShort(seUpdate->port);

        seUpdate->m_stat.sv.SetMD51(seUpdate->strExeMD5.c_str());
        seUpdate->m_stat.sv.SetMD52(seUpdate->strDataMD5.c_str());
        seUpdate->m_stat.lUserCount = seUpdate->lUserCnt;
        seUpdate->m_stat.Serialize(&cmd, SERVER_STAT_VERSION);
    }

    // Game World Server
    short i, sCnt;
    TGWSMap::iterator itGws;
    cmd.WriteShort((short)m_gwsMap.size());
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        pGW = itGws->second;
        cmd.WriteString((char*)pGW->m_szName.c_str());
        cmd.WriteString((char*)pGW->m_szListenClientIP.c_str());
        cmd.WriteShort((short)(pGW->m_lListenClientPort));
        if (pGW->m_stubState==CGameWorldStub::STUB_STATE_ALIVE && pGW->m_gwState==gwssAcceptLogin)
        {
            cmd.WriteShort(2); // 正常连接
        }else
        {
            if (pGW->m_stubState!=CGameWorldStub::STUB_STATE_ALIVE)
            {
                cmd.WriteShort(0); // 未连接上来
            }else
            {
                cmd.WriteShort(1); // 连接，不接受Login
            }
        }
        pGW->m_gwStat.Serialize(&cmd, SERVER_STAT_VERSION, g_cfg.listenMonitor.peopleRate);
        sCnt = (short)pGW->m_rsStat.size();
        cmd.WriteShort(sCnt);
        for (i=0; i<sCnt; i++)
        {
            cmd.WriteShort((short)pGW->m_rsStat[i].sID);
            cmd.WriteShort((short)pGW->m_rsStat[i].sState);
            pGW->m_rsStat[i].SerializeAll(&cmd, SERVER_STAT_VERSION);
        }
        pGW->m_gmStat.Serialize(&cmd, SERVER_STAT_VERSION, g_cfg.listenMonitor.peopleRate);
    }

    pClientSessionManager->SendCmd();
}

void CLoginServer::SendBulletinToGWServer(CGameWorldStub *pGW, std::string content, char cStyle)
{
    if(!pGW)    return;

    long color = 0;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(lgs2gws_bulletin);
    g_sendCmd->WriteString((char*)content.c_str());
    g_sendCmd->WriteLong(color);
    g_sendCmd->WriteByte(cStyle);
    pGW->SendCmd(g_sendCmd);
}

bool CLoginServer::CheckServerVersion(CG_CmdPacket* pPacket)
{
    char *szServerName, *szExeMD5, *szDataMD5, *szOSType;
    pPacket->ReadString(&szServerName);
    pPacket->ReadString(&szExeMD5);
    pPacket->ReadString(&szDataMD5);
    pPacket->ReadString(&szOSType);

    std::string strServerName(szServerName);
    strServerName += szOSType;
    if (m_mapVersion.find(strServerName)==m_mapVersion.end())
    {
        m_mapVersion[strServerName] = SVersionData();
        m_mapVersion[strServerName].szExeMD5 = szExeMD5;
        m_mapVersion[strServerName].szDataMD5 = szDataMD5;
        m_mapVersion[strServerName].szOSType = szOSType;
        return true;
    }else
    {
        bool bSame = true;
        SVersionData* pVersion = &(m_mapVersion[strServerName]);
        if (strcmp(szOSType, pVersion->szOSType.c_str())==0)
        {
            if (strcmp(szExeMD5, pVersion->szExeMD5.c_str())!=0)
            {
                bSame = false;
            }
        }
        if (strcmp(szDataMD5, pVersion->szDataMD5.c_str())!=0)
        {
            bSame = false;
        }
        return bSame;
    }
}

bool CLoginServer::CheckGameworldVersion(CGameWorldStub* pCheckGws, CG_CmdPacket* pPacket)
{
    char *szExeMD5, *szDataMD5, *szOSType;
    pPacket->ReadString(&szExeMD5);
    pPacket->ReadString(&szDataMD5);
    pPacket->ReadString(&szOSType);

    bool bSame = true;
    CGameWorldStub *pGws;
    TGWSMap::iterator itGws;
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        pGws = itGws->second;
        if (pGws->m_bVersionChecked)
        {
            if (strcmp(szOSType, pGws->m_strOsType.c_str())==0)
            {
                if (strcmp(szExeMD5, pGws->m_strExeMD5.c_str())!=0)
                {
                    bSame = false;
                }
            }
            if (strcmp(szDataMD5, pGws->m_strDataMD5.c_str())!=0)
            {
                bSame = false;
            }
        }
    }
    if (bSame)
    {
        pCheckGws->m_strExeMD5 = szExeMD5;
        pCheckGws->m_strDataMD5 = szDataMD5;
        pCheckGws->m_strOsType = szOSType;
        pCheckGws->m_bVersionChecked = true;
    }
    return bSame;
}

CGameWorldStub* CLoginServer::FindGws(long lIdx)
{
    TGWSMap::iterator it = m_gwsMap.find(lIdx);
    if (it==m_gwsMap.end())
    {
        return NULL;
    }
    return it->second;
}

CGameWorldStub* CLoginServer::FindGwsByGWSID(long lGameworldID)
{
    CGameWorldStub *pGws;
    TGWSMap::iterator itGws;
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        pGws = itGws->second;
        if (pGws->m_lGameworldID==lGameworldID)
        {
            return pGws;
        }
    }
    return NULL;
}

CGameWorldStub* CLoginServer::FindGwsByName(const char* szName)
{
    CGameWorldStub *pGws;
    TGWSMap::iterator itGws;
    for (itGws=m_gwsMap.begin(); itGws!=m_gwsMap.end(); itGws++)
    {
        pGws = itGws->second;
        if (strcmp(pGws->m_szName.c_str(), szName)==0)
        {
            return pGws;
        }
    }
    return NULL;
}

void CLoginServer::SendBulletinToAllGWS(const char* content, char cStyle)
{
    CGameWorldStub* pGws;
    TGWSMap::iterator it;
    for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        SendBulletinToGWServer(pGws, content, cStyle);
    }
}

void CLoginServer::TestServer(GSClientSessionManager* pClientSessionManager, int iSessionIdx)
{
    std::list<STester>::iterator itTester;
    for (itTester=m_listTest.begin(); itTester!=m_listTest.end(); itTester++)
    {
        if ((*itTester).pClientSessionManager==pClientSessionManager
            && (*itTester).iSessionIdx==iSessionIdx)
        {
            return;
        }
    }

    if (m_listTest.size()==0)
    {
        m_lastTestTime = time(NULL);

        CGameWorldStub* pGws;
        TGWSMap::iterator it;
        for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
        {
            pGws = it->second;
            pGws->Test();
        }
    }

    m_listTest.push_back(STester());
    m_listTest.back().pClientSessionManager = pClientSessionManager;
    m_listTest.back().iSessionIdx = iSessionIdx;
}

void CLoginServer::SendTestResult()
{
    m_lastTestTime = 0;
    if (m_listTest.size()==0)
    {
        return;
    }

    std::string resultStr;
    CGameWorldStub* pGws;
    TGWSMap::iterator it;
    for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        pGws->GetTestResult(resultStr);
    }

    CG_CmdPacket& cmd = m_listTest.back().pClientSessionManager->BeginCmd(gs2m_tester_ret);
    cmd.WriteString((char*)(const char*)resultStr.c_str());

    std::list<STester>::iterator itTester;
    for (itTester=m_listTest.begin(); itTester!=m_listTest.end(); itTester++)
    {
        (*itTester).pClientSessionManager->SendCmd((*itTester).iSessionIdx);
    }
    m_listTest.clear();
}

// 如果之前有，就刷新，如果没有就添加
void CLoginServer::RegisterGws(long lIdx, const char* szName, const char* szIP,
                               short sPort, long lGwID, SAllowIP& allowIP)
{
    // 查找位置，根据位置进行插入
    SGwsList* pGws = NULL;
    std::list<SGwsList>::iterator it;
    for (it=m_listGws.begin(); it!=m_listGws.end(); it++)
    {
        pGws = &(*it);
        if (strcmp(pGws->szName.c_str(), szName)==0)
        {
            break;
        }
    }
    if (it==m_listGws.end())
    {
        pGws = NULL;
    }

    if (pGws==NULL)
    {
		if (m_listGws.empty())
		{
			m_listGws.push_back(SGwsList());
			pGws = &(m_listGws.front());
		}else if (lGwID<m_listGws.front().lGameworldID)
        {
            m_listGws.push_front(SGwsList());
            pGws = &(m_listGws.front());
        }else if (lGwID>=m_listGws.back().lGameworldID)
        {
            m_listGws.push_back(SGwsList());
            pGws = &(m_listGws.back());
        }else
        {
            for (it=m_listGws.begin(); it!=m_listGws.end(); it++)
            {
                if (lGwID < (*it).lGameworldID)
                {
                    it = m_listGws.insert(it, SGwsList());
                    pGws = &(*it);
                    break;
                }
            }
        }
    }

    // 修改值
    pGws->lIdx = lIdx;
    pGws->lGameworldID = lGwID;
    pGws->szName = szName;
    pGws->szIP = szIP;
    pGws->sPort = sPort;
    pGws->bHaveAllow = allowIP.HaveIP();
    pGws->allowDisplayIP = allowIP;
}

void CLoginServer::UnregisterGws(long lIdx)
{
    SGwsList* pGws;
    std::list<SGwsList>::iterator it;
    for (it=m_listGws.begin(); it!=m_listGws.end(); it++)
    {
        pGws = &(*it);
        if (pGws->lIdx==lIdx)
        {
            break;
        }
    }
    if (it==m_listGws.end())
    {
        return;
    }
    m_listGws.erase(it);
}

void CLoginServer::SendGWServerList(GSClientSessionManager* pClient, int iCount/* = 0*/)
{
    // LOG1("SendGWServerList, state=%d\n", g_lgs->m_lStatus);
    if (g_lgs->m_lStatus&(lssLoginNormal|lssLoginGM)) // 如果允许登陆就发送列表
    {
        CG_CmdPacket& cmd = pClient->BeginCmd(gs2c_gws_list);

		if (iCount == 0 || iCount >= m_listGws.size())
			iCount = m_listGws.size();

        cmd.WriteShort((short)g_cfg.listenClientGuide.refreshInterval);
        cmd.WriteShort((short)iCount);

        SGwsList* pGws;
        CGameWorldStub* pGWStub;
        std::list<SGwsList>::iterator it = m_listGws.begin();
        for (int i = 0; i < iCount; ++i)
        {
            pGws = &(*it);
            cmd.WriteString((char*)pGws->szName.c_str());
            cmd.WriteString((char*)pGws->szIP.c_str());
            cmd.WriteShort((short)pGws->sPort);
            pGWStub = g_lgs->FindGws(pGws->lIdx);
            if (pGWStub)
            {
                if (pGWStub->m_stubState==CGameWorldStub::STUB_STATE_ALIVE
                    && pGWStub->m_gwState==gwssAcceptLogin)
                {
                    cmd.WriteByte(pGWStub->m_evaluation);
                }else
                {
                    cmd.WriteByte(0);
                }
            }else
            {
                cmd.WriteByte(0);
            }
			++it;
        }
        pClient->SendCmd();
    }else
    {
        CG_CmdPacket& cmd = pClient->BeginCmd(gs2c_gws_info);
        pClient->SendCmd();
    }
}

void CLoginServer::UpdatePointInfo(SUserInfo* pUserInfo, char cPointType, long lPoint, char cVIP)
{
    if (pUserInfo==NULL)
        return;
    if (pUserInfo->gwsStub==NULL)
        return;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(lgs2gws_update_point_ret);
    g_sendCmd->WriteLong(pUserInfo->info.accountId);
    g_sendCmd->WriteByte(cPointType);
    g_sendCmd->WriteLong(lPoint);
    g_sendCmd->WriteByte(cVIP);
    pUserInfo->gwsStub->SendCmd(g_sendCmd);
}

SUserInfo* CLoginServer::OnChargePass(long lSn, char cPointType, long lPoint, char cVIP)
{
    SUserInfo* pUserInfo;
    if (m_listChargeLoginCmd.front().lSn==lSn)
    {
        pUserInfo = TryAddUser(&(m_listChargeLoginCmd.front().cmd), cPointType, lPoint, cVIP);
        m_listChargeLoginCmd.pop_front();
        return pUserInfo;
    }

    if (m_listChargeCmd.front().lSn==lSn)
    {
        pUserInfo = FindUser(m_listChargeCmd.front().dwUserID);
        UpdatePointInfo(pUserInfo, cPointType, lPoint, cVIP);
        m_listChargeCmd.pop_front();
        return pUserInfo;
    }

    std::list<SChargeLoginCmd>::iterator itLogin;
    for (itLogin=m_listChargeLoginCmd.begin(); itLogin!=m_listChargeLoginCmd.end(); itLogin++)
    {
        if ((*itLogin).lSn==lSn)
        {
            pUserInfo = TryAddUser(&((*itLogin).cmd), cPointType, lPoint, cVIP);
            m_listChargeLoginCmd.erase(itLogin);
            return pUserInfo;
        }
    }

    std::list<SChargeCmd>::iterator itCheck;
    for (itCheck=m_listChargeCmd.begin(); itCheck!=m_listChargeCmd.end(); itCheck++)
    {
        if ((*itCheck).lSn==lSn)
        {
            pUserInfo = FindUser((*itCheck).dwUserID);
            UpdatePointInfo(pUserInfo, cPointType, lPoint, cVIP);
            m_listChargeCmd.erase(itCheck);
            return pUserInfo;
        }
    }
    return NULL;
}

void CLoginServer::OnChargeFail(long lSn, const char* pReason)
{
    SUserInfo* pUserInfo;

    // 登陆的返回, 第一个记录
    if (m_listChargeLoginCmd.front().lSn==lSn)
    {
        SListCmd &cmd = m_listChargeLoginCmd.front().cmd;
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_USERDEFINE or LOGIN_RET_FAILED_POT_NOT_ENOUGH
        g_sendCmd->WriteLong(cmd.userloginRet.lSeed);
        if (pReason)
        {
            g_sendCmd->WriteLong(LOGIN_RET_FAILED_USERDEFINE);
            g_sendCmd->WriteString((char*)pReason);
        }else
        {
		    g_sendCmd->WriteLong(LOGIN_RET_FAILED_POT_NOT_ENOUGH);
            g_sendCmd->WriteShort(LOGIN_RET_CODE_PotNotEnough);
        }
        cmd.SendCmd(g_sendCmd);

        m_listChargeLoginCmd.pop_front();
        return;
    }

    // 计费的返回, 第一个记录
    if (m_listChargeCmd.front().lSn==lSn)
    {
        pUserInfo = FindUser(m_listChargeCmd.front().dwUserID);
        if (pUserInfo)
        {
            KickUser(pUserInfo, KICK_USER_POT_NOT_ENOUGH);
        }
        m_listChargeCmd.pop_front();
        return;
    }

    // 登陆的返回, 其它记录
    std::list<SChargeLoginCmd>::iterator itLogin;
    for (itLogin=m_listChargeLoginCmd.begin(); itLogin!=m_listChargeLoginCmd.end(); itLogin++)
    {
        if ((*itLogin).lSn==lSn)
        {
            SListCmd &cmd = ((*itLogin).cmd);
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_user_login_ret); // LOGIN_RET_FAILED_USERDEFINE or LOGIN_RET_FAILED_POT_NOT_ENOUGH
            g_sendCmd->WriteLong(cmd.userloginRet.lSeed);
            if (pReason)
            {
                g_sendCmd->WriteLong(LOGIN_RET_FAILED_USERDEFINE);
                g_sendCmd->WriteString((char*)pReason);
            }else
            {
		        g_sendCmd->WriteLong(LOGIN_RET_FAILED_POT_NOT_ENOUGH);
                g_sendCmd->WriteShort(LOGIN_RET_CODE_PotNotEnough);
            }
            cmd.SendCmd(g_sendCmd);

            m_listChargeLoginCmd.erase(itLogin);
            return;
        }
    }

    // 计费的返回, 其它记录
    std::list<SChargeCmd>::iterator itCheck;
    for (itCheck=m_listChargeCmd.begin(); itCheck!=m_listChargeCmd.end(); itCheck++)
    {
        if ((*itCheck).lSn==lSn)
        {
            pUserInfo = FindUser((*itCheck).dwUserID);
            if (pUserInfo)
            {
                KickUser(pUserInfo, KICK_USER_POT_NOT_ENOUGH);
            }
            m_listChargeCmd.erase(itCheck);
            return;
        }
    }
}

void CLoginServer::AddChargeCmd(long lSn, DWORD dwAccountID, SUserInfo* pUserInfo, time_t tNow)
{
    m_listChargeCmd.push_back(SChargeCmd());
    SChargeCmd* pNewCmd = &(m_listChargeCmd.back());
    pNewCmd->lSn = lSn;
    pNewCmd->dwUserID = dwAccountID;
    pNewCmd->tTimeout = tNow + 60; // 60秒超时
    pUserInfo->tChargeExpire = 0;
    pUserInfo->tChargeNextCheck = pNewCmd->tTimeout; // 如果60秒内没有返回，就60秒后再发一次检查命令
}

void CLoginServer::OnUserEnterGame(DWORD dwAccountID, short sLevel)
{
    SUserInfo* pUserInfo = FindUser(dwAccountID);
    if (pUserInfo)
    {
        pUserInfo->sActorLevel = sLevel;
        if (m_bUseCharge)
        {
            int iSn = m_chargeSession.Enter((short)(pUserInfo->gwsStub->m_lGameworldID),
                pUserInfo->info.accountName.c_str(), pUserInfo->sActorLevel);
            AddChargeCmd(iSn, dwAccountID, pUserInfo, time(NULL));
        }
    }
}

void CLoginServer::SendMsgToUser(const char* szUsername, char cMsgType, const char* szMessage)
{
    if (szUsername==NULL)
    {
        return;
    }
    if (szMessage==NULL)
    {
        return;
    }
    if (szMessage[0]==0)
    {
        return;
    }
    SUserInfo* pUserInfo = FindUserByName(szUsername);
    if (pUserInfo)
    {
        if (pUserInfo->gwsStub)
        {
            pUserInfo->gwsStub->SendMessageToUser(pUserInfo->info.accountId, cMsgType, szMessage);
        }
    }
}

void CLoginServer::CreateAccount(long lOrderID, const char* szUsername,
                                 const char* szPassword, long lPriv, short sLocked,
                                 long tLockBegin, long tLockEnd)
{
    SListCmd newCmd(NULL, LIST_CMD_ACCOUNT_CREATE);

    strncpy(newCmd.accountCreate.szUsername, szUsername, MAX_USER_NAME_SIZE);
    newCmd.accountCreate.szUsername[MAX_USER_NAME_SIZE] = 0;

    strncpy(newCmd.accountCreate.szPassword, szPassword, MAX_USER_PWD_SIZE);
    newCmd.accountCreate.szPassword[MAX_USER_PWD_SIZE] = 0;

    newCmd.accountCreate.lLoginOrderID = lOrderID;
    newCmd.accountCreate.lPriv = lPriv;
    newCmd.accountCreate.sLocked = sLocked;
    newCmd.accountCreate.tLockBegin = tLockBegin;
    newCmd.accountCreate.tLockEnd = tLockEnd;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::LockAccount(CGameWorldStub* pStub, long lResultAccountID,
                               const char* szUsername, short sLocked, long tLockBegin, long tLockEnd)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_LOCK);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountLock.bUseUsername = true;
    strncpy(newCmd.accountLock.szUsername, szUsername, MAX_USER_NAME_SIZE);
    newCmd.accountLock.szUsername[MAX_USER_NAME_SIZE] = 0;
    newCmd.accountLock.sLocked = sLocked;
    newCmd.accountLock.tLockBegin = tLockBegin;
    newCmd.accountLock.tLockEnd = tLockEnd;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::LockAccountByID(CGameWorldStub* pStub, long lResultAccountID,
                                   long lAccountID, short sLocked, long tLockBegin, long tLockEnd)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_LOCK);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountLock.bUseUsername = false;
    newCmd.accountLock.lAccountID = lAccountID;
    newCmd.accountLock.sLocked = sLocked;
    newCmd.accountLock.tLockBegin = tLockBegin;
    newCmd.accountLock.tLockEnd = tLockEnd;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::SetAccountPriv(CGameWorldStub* pStub, long lResultAccountID,
                                  const char* szUsername, long lPriv)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_SETPRIV);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountSetPriv.bUseUsername = true;
    strncpy(newCmd.accountSetPriv.szUsername, szUsername, MAX_USER_NAME_SIZE);
    newCmd.accountSetPriv.szUsername[MAX_USER_NAME_SIZE] = 0;
    newCmd.accountSetPriv.lPriv = lPriv;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::SetAccountPrivByID(CGameWorldStub* pStub, long lResultAccountID,
                                      long lAccountID, long lPriv)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_SETPRIV);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountSetPriv.bUseUsername = false;
    newCmd.accountSetPriv.lAccountID = lAccountID;
    newCmd.accountSetPriv.lPriv = lPriv;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::SetAccountPassword(CGameWorldStub* pStub, long lResultAccountID,
                                      const char* szUsername, const char* szPassword,
                                      long lLoginOrderID)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_SETPWD);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountSetPwd.bUseUsername = true;
    strncpy(newCmd.accountSetPwd.szUsername, szUsername, MAX_USER_NAME_SIZE);
    newCmd.accountSetPwd.szUsername[MAX_USER_NAME_SIZE] = 0;
    strncpy(newCmd.accountSetPwd.szPassword, szPassword, MAX_USER_PWD_SIZE);
    newCmd.accountSetPwd.szPassword[MAX_USER_PWD_SIZE] = 0;
    newCmd.accountSetPwd.lAccountID = 0;
    newCmd.accountSetPwd.lLoginOrderID = lLoginOrderID;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::SetAccountPasswordByID(CGameWorldStub* pStub, long lResultAccountID,
                                          long lAccountID, const char* szPassword)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_SETPWD);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountSetPwd.bUseUsername = false;
    newCmd.accountSetPwd.lAccountID = lAccountID;
    strncpy(newCmd.accountSetPwd.szPassword, szPassword, MAX_USER_PWD_SIZE);
    newCmd.accountSetPwd.szPassword[MAX_USER_PWD_SIZE] = 0;
    newCmd.accountSetPwd.lLoginOrderID = 0;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::DeleteAccount(CGameWorldStub* pStub, long lResultAccountID, const char* szUsername)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_DELETE);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountDelete.bUseUsername = true;
    strncpy(newCmd.accountDelete.szUsername, szUsername, MAX_USER_NAME_SIZE);
    newCmd.accountDelete.szUsername[MAX_USER_NAME_SIZE] = 0;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

void CLoginServer::DeleteAccountByID(CGameWorldStub* pStub, long lResultAccountID, long lAccountID)
{
    SListCmd newCmd(pStub, LIST_CMD_ACCOUNT_DELETE);
    newCmd.accountInfo.accountId = lResultAccountID;

    newCmd.accountDelete.bUseUsername = false;
    newCmd.accountDelete.lAccountID = lAccountID;

    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add log concurrent cmd failed\n");
    }
}

bool CLoginServer::Deduct(short sServer, long lAccountID, long lUserID,
                          short sPoint, long lDeductOrderID)
{
    //m_chargeSession.Deduct(sServer, lAccountID, lUserID, sPoint, lDeductOrderID);
	int iType;
	char* Mysqlpoint;
	int curPoint = 0;
	sprintf(m_sql,
		//        0
		"select `point`"
		"from `user`"
		"where `idx`='%d'",lAccountID);
	if(!m_binitdb)
		Initdb();
	if (!m_db.Query(m_sql))
	{
		ERR1("user :query db failed,sql = %s",m_sql);
		return false;
	}
	if(!m_db.GetRow())
	{
		//OnDeduct(lAccountID,sServer,lAccountID,lUserID,sPoint,(char*)"没找到用户信息！",lDeductOrderID);
		return false;
	}
	if(!m_db.GetFieldContent(0,&iType,&Mysqlpoint)) return false;
	curPoint = atoi(Mysqlpoint) - sPoint;
	if(curPoint < 0)
	{
		//OnDeduct(lAccountID,sServer,lAccountID,lUserID,sPoint,(char*)"您当前的点数不够，请去充值！",lDeductOrderID);
		return false;
	}
	sprintf(m_sql,
		"update `user`"
		"set `point`= %d "
		"where `idx` = '%s'",
		curPoint,lAccountID);
	if (!m_db.Query(m_sql))
	{
		ERR1("user : query db failed, sql = %s",m_sql);
		return false;
	}
	LOG1("%d deduct %d\n", lAccountID,sPoint);
	return true;
}

void CLoginServer::PreDeduct(short sServer, long lAccountID, long lUserID,
                             short sPoint, long lDeductOrderID)
{
    m_chargeSession.PreDeduct(sServer, lAccountID, lUserID, sPoint, lDeductOrderID);
}

void CLoginServer::AddPointRet(short sServer, long lAccountID, long lUserID, short sPoint,char* cResult,char cSucceed)
{
	CGameWorldStub* pGws = FindGwsByGWSID(sServer);
	if (pGws)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(lgs2gws_add_point_ret);
		g_sendCmd->WriteLong(lUserID);
		g_sendCmd->WriteByte(cSucceed);
		g_sendCmd->WriteString(cResult);
		g_sendCmd->WriteShort(sPoint);
		pGws->SendCmd(g_sendCmd);
	}
}
void CLoginServer::OnDeduct(long lSn, short sServer, long lAccountID, long lUserID,
                            short sPoint, short curPoint,char* cResult, long lDeductOrderID)
{
    CGameWorldStub* pGws = FindGwsByGWSID(sServer);
    if (pGws)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_deduct_ret);
        g_sendCmd->WriteLong(lUserID);
        g_sendCmd->WriteLong(lDeductOrderID);
        g_sendCmd->WriteString(cResult);
        g_sendCmd->WriteShort(sPoint);
		g_sendCmd->WriteShort(curPoint);
        pGws->SendCmd(g_sendCmd);
    }
}

void CLoginServer::OnPreDeduct(short sServer, long lAccountID, long lUserID,
							   short sPoint,short curPoint, char* cResult,char cSucceed, long lDeductOrderID,long event,long lAuctionId)
{
    CGameWorldStub* pGws = FindGwsByGWSID(sServer);
    if (pGws)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_prededuct_ret);
        g_sendCmd->WriteLong(lUserID);
        g_sendCmd->WriteLong(lDeductOrderID);
		g_sendCmd->WriteByte(cSucceed);
		g_sendCmd->WriteShort(sPoint);
		g_sendCmd->WriteShort(curPoint);
		g_sendCmd->WriteString(cResult);
		g_sendCmd->WriteLong(event);
		g_sendCmd->WriteLong(lAuctionId);
        pGws->SendCmd(g_sendCmd);
    }else
    {
        LOG("OnPreDeduct\n");
    }
}

CLoginServer::SCmdChargeCard* CLoginServer::CmdChargeCardNew()
{
    if (m_cmdChargeCardFree.size()==0)
    {
        for (int i=0; i<20; i++)
        {
            m_cmdChargeCardFree.push_back(new SCmdChargeCard);
        }
    }
    SCmdChargeCard* pCmd = m_cmdChargeCardFree.back();
    m_cmdChargeCardFree.pop_back();
    m_cmdChargeCardList.push_back(pCmd);
    return pCmd;
}

bool CLoginServer::CmdChargeCardRemove(long lSn)
{
    std::list<SCmdChargeCard*>::iterator it;
    for (it=m_cmdChargeCardList.begin(); it!=m_cmdChargeCardList.end(); it++)
    {
        if ((*it)->lSn==lSn)
        {
            m_cmdChargeCardFree.push_back(*it);
            m_cmdChargeCardList.erase(it);
            return true;
        }
    }
    return false;
}

bool CLoginServer::CmdChargeCardExist(long lSn)
{
    std::list<SCmdChargeCard*>::iterator it;
    for (it=m_cmdChargeCardList.begin(); it!=m_cmdChargeCardList.end(); it++)
    {
        if ((*it)->lSn==lSn)
        {
            return true;
        }
    }
    return false;
}

void CLoginServer::CmdChargeCardRun()
{
    time_t tNow = time(NULL);
    SCmdChargeCard* pCmd;
    std::list<SCmdChargeCard*>::iterator it;
    for (it=m_cmdChargeCardList.begin(); it!=m_cmdChargeCardList.end();)
    {
        pCmd = *it;
        it ++;
        if (tNow > pCmd->tTimeout)
        {
            LogChargeCard(pCmd->lSn, SCmdCardChargeLog::CCOP_CHARGE_TIMEOUT,
                pCmd->cCardType, pCmd->lAccountID, pCmd->sServer, pCmd->lUserID, pCmd->szCardID);
            CmdChargeCardRemove(pCmd->lSn);
        }
    }
}

void CLoginServer::CmdChargeCardDelete()
{
    SCmdChargeCard* pCmd;
    std::list<SCmdChargeCard*>::iterator it, itNext;
    for (it=m_cmdChargeCardList.begin(); it!=m_cmdChargeCardList.end();)
    {
        pCmd = *it;
        it ++;
        LogChargeCard(pCmd->lSn, SCmdCardChargeLog::CCOP_CHARGE_CLOSED,
            pCmd->cCardType, pCmd->lAccountID, pCmd->sServer, pCmd->lUserID, pCmd->szCardID);
        CmdChargeCardRemove(pCmd->lSn);
    }
    for (it=m_cmdChargeCardList.begin(); it!=m_cmdChargeCardList.end(); it++)
    {
        delete (*it);
    }
    for (it=m_cmdChargeCardFree.begin(); it!=m_cmdChargeCardFree.end(); it++)
    {
        delete (*it);
    }
    m_cmdChargeCardList.clear();
    m_cmdChargeCardFree.clear();
}

void CLoginServer::OnUserBlockChanged(char* szAccountName, long lAccountId, long lUserId, char* oldBlockFileName, char* newBlockFileName)
{
    if (m_bUseCharge)
    {
        m_chargeSession.OnUserBlockChanged(szAccountName, lAccountId, lUserId, oldBlockFileName, newBlockFileName);
    }
}

void CLoginServer::ChargeCard(long lBuyerAccountID, const char* szBuyerAccountName,
        long lSalesmanAccountID, const char* szSalesmanAccountName,
        short sServer, long lUserID, const char* szUserName, short sLevel, char cMetier, char cMetierLev,
        char cCardType, const char* szCardID, const char* szCardPwd, long lUserParam)
{
    if (s_iChargeTest)
    {
        // 测试代码
        CGameWorldStub* pGws = FindGwsByGWSID(sServer);
        if (pGws)
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_charge_card_ret);
            g_sendCmd->WriteLong(lUserID);
            g_sendCmd->WriteByte(1); // 失败
            g_sendCmd->WriteString((char*)szCardID);
            g_sendCmd->WriteString((char*)"杂志免费试用卡充值成功");
            g_sendCmd->WriteLong(lUserParam);
            pGws->SendCmd(g_sendCmd);
        }else
        {
            LOG("ChargeCard failed. Not use charge server\n");
        }
        return;
    }

	//激活码。。。。。。。by tooth.shi 09/02/16
	char* type;
	char* attribute;
	char* userid;
	int iType;

	sprintf(  m_sql,
		//       0          1          2           3        4     5								6
		"select `type`,`attribute`,`userid` "
		"from `card_number` "
		"where `cardid`='%s'", //
		szCardID);

	//check info
	if(!m_binitdb)
		Initdb();
	if (!m_db.Query(m_sql))
	{
		ERR1("card_number :query db failed,sql = %s",m_sql);
		GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)"点数系统暂时维护，请稍后重试！",lUserParam);
		return;
	}
	if(!m_db.GetRow())
	{
		GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)"您输入的道具码不存在，请检查！！",lUserParam);
		return ;
	}
	if(!m_db.GetFieldContent(2,&iType,&userid)) return;
	if(strcmp(userid,"0"))
	{
		GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)"您输入的道具码已被使用，请重新申请！",lUserParam);
		return ;
	}

	if (!m_db.GetFieldContent(0, &iType, &type))
		return ;

	if (!m_db.GetFieldContent(1, &iType, &attribute))
		return ;
	int itype = atoi(type);
	char str[200];
	switch(itype)
	{
	case 5:	//point gift				
		{
			int iAttribute = atoi(attribute);
			sprintf(m_sql,
					"update `user`"
					"set `point`= `point` + %d , `isVip` = 1"
					"where `idx` = %d",
					iAttribute,lBuyerAccountID);
			if (!m_db.Query(m_sql))
			{
				ERR1("user : query db failed, sql = %s",m_sql);
				GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)"点数系统暂时维护，请稍后重试！",lUserParam);
				return ;
			}
			sprintf(str,"恭喜您获得 %d 的点数！",iAttribute);
			GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)str,lUserParam,1);
			break;
		}
	case 6:	// vip time gift
		{
			int iAttribute = atoi(attribute);
			sprintf(m_sql,
					"update `user`"
					"set `datetime` = DATE_ADD( datetime, INTERVAL %d DAY )"
					"where `idx` = %d",
					iAttribute,lBuyerAccountID);
			if (!m_db.Query(m_sql))
			{
				ERR1("user : query db failed, sql = %s",m_sql);
				GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)"点数系统暂时维护，请稍后重试！",lUserParam);
				return ;
			}
			sprintf(str,"恭喜您获得 %d 的天的VIP！",iAttribute);
			GiftErrorRet(sServer,lUserID,(char*)szCardID,(char*)str,lUserParam,1);
			break;
		}
	default:
		{
			AddGift(1,szUserName,sServer,lUserID,sLevel,atoi(type),attribute,0,lSalesmanAccountID,const_cast<char*>(szCardID));
			break;
		}
	}
	SUserInfo* pUserInfo = FindUserByName(szBuyerAccountName);
	if (pUserInfo == 0)
	{
		sprintf(m_sql,
			"update `card_number`"
			"set `userid`= %d ,`accountid` = %d ,`usetime` = NOW()"
			"where `cardid` = '%s'",
			lUserID,lBuyerAccountID,szCardID);
	}
	else
	{
		sprintf(m_sql,
			"update `card_number`"
			"set `userid`= %d ,`accountid` = %d ,`usetime` = NOW(),`userip` = '%s'"
			"where `cardid` = '%s'",
			lUserID,lBuyerAccountID,pUserInfo->info.ip.c_str(),szCardID);
	}

	//update card_number
	if (!m_db.Query(m_sql))
	{
		ERR1("card_number : query db failed, sql = %s",m_sql);
		return ;
	}

    // 正式代码
//     if (m_bUseCharge)
//     {
//         SCmdChargeCard *pCmd = CmdChargeCardNew();
//         pCmd->cCardType = cCardType;
//         pCmd->lAccountID = lBuyerAccountID;
//         pCmd->sServer = sServer;
//         pCmd->lUserID = lUserID;
//         strncpy(pCmd->szCardID, szCardID, 39);
//         pCmd->szCardID[39] = 0;
//         pCmd->tTimeout = time(NULL)+60; // 60秒超时
//         pCmd->lSn = lUserParam;
//         m_chargeSession.ChargeCard(lBuyerAccountID, szBuyerAccountName,
//             lSalesmanAccountID, szSalesmanAccountName, 
//             sServer, lUserID, szUserName, sLevel, cMetier, cMetierLev,
//             cCardType, szCardID, szCardPwd, lUserParam);
// 
//         LogChargeCard(pCmd->lSn, SCmdCardChargeLog::CCOP_CHARGE, pCmd->cCardType,
//             pCmd->lAccountID, pCmd->sServer, pCmd->lUserID, pCmd->szCardID);
//     }else
//     {
//         CGameWorldStub* pGws = FindGwsByGWSID(sServer);
//         if (pGws)
//         {
//             g_sendCmd->BeginWrite();
//             g_sendCmd->WriteShort(lgs2gws_charge_card_ret);
//             g_sendCmd->WriteLong(lUserID);
//             g_sendCmd->WriteByte(0); // 失败
//             g_sendCmd->WriteString((char*)szCardID);
//             g_sendCmd->WriteString((char*)"可能是服务器忙，请稍候重试(1000)");
//             g_sendCmd->WriteLong(lUserParam);
//             pGws->SendCmd(g_sendCmd);
//         }else
//         {
//             LOG("ChargeCard failed. Not use charge server\n");
//         }
// 	}
}

void CLoginServer::OnChargeCard(long lSn, short sServer, long lUserID,
                                const char* szCardID, char cSucceeded, const char* szMsg)
{
    CmdChargeCardRemove(lSn);
#ifdef _DEBUG
    MSG4("%d:%d 充值点卡 [%s] %s, %s\n", sServer, lUserID, szCardID, (cSucceeded?"成功":"失败"), szMsg);
#endif
    if (cSucceeded)
    {
        LogChargeCard(lSn, SCmdCardChargeLog::CCOP_CHARGE_SUCCEEDED, 0, 0, sServer, lUserID, szCardID);
    }else
    {
        LogChargeCard(lSn, SCmdCardChargeLog::CCOP_CHARGE_FAILED, 0, 0, sServer, lUserID, szCardID);
    }
    CGameWorldStub* pGws = FindGwsByGWSID(sServer);
    if (pGws)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_charge_card_ret);
        g_sendCmd->WriteLong(lUserID);
        g_sendCmd->WriteByte(cSucceeded); // 成功
        g_sendCmd->WriteString((char*)szCardID);
        g_sendCmd->WriteString((char*)szMsg);
        g_sendCmd->WriteLong(lSn);
        pGws->SendCmd(g_sendCmd);
    }else
    {
        LOG1("OnChargeCard %s\n", szMsg);
    }
}

CLoginServer::SCmdCardCanTrade* CLoginServer::CmdCardCanTradeNew()
{
    if (m_cmdCardCanTradeFree.size()==0)
    {
        for (int i=0; i<20; i++)
        {
            m_cmdCardCanTradeFree.push_back(new SCmdCardCanTrade);
        }
    }
    SCmdCardCanTrade* pCmd = m_cmdCardCanTradeFree.back();
    m_cmdCardCanTradeFree.pop_back();
    m_cmdCardCanTradeList.push_back(pCmd);
    return pCmd;
}

bool CLoginServer::CmdCardCanTradeRemove(long lSn)
{
    std::list<SCmdCardCanTrade*>::iterator it;
    for (it=m_cmdCardCanTradeList.begin(); it!=m_cmdCardCanTradeList.end(); it++)
    {
        if ((*it)->lSn==lSn)
        {
            m_cmdCardCanTradeFree.push_back(*it);
            m_cmdCardCanTradeList.erase(it);
            return true;
        }
    }
    return false;
}

bool CLoginServer::CmdCardCanTradeExist(long lSn)
{
    std::list<SCmdCardCanTrade*>::iterator it;
    for (it=m_cmdCardCanTradeList.begin(); it!=m_cmdCardCanTradeList.end(); it++)
    {
        if ((*it)->lSn==lSn)
        {
            return true;
        }
    }
    return false;
}

void CLoginServer::CmdCardCanTradeRun()
{
    time_t tNow = time(NULL);
    SCmdCardCanTrade* pCmd;
    std::list<SCmdCardCanTrade*>::iterator it;
    for (it=m_cmdCardCanTradeList.begin(); it!=m_cmdCardCanTradeList.end();)
    {
        pCmd = *it;
        it ++;
        if (tNow > pCmd->tTimeout)
        {
            OnCardCanTradeResult(pCmd->lSn, pCmd->sServer, pCmd->lUserID, pCmd->szCardID, 0, "超时", 0);
        }
    }
}

void CLoginServer::CmdCardCanTradeDelete()
{
    SCmdCardCanTrade* pCmd;
    std::list<SCmdCardCanTrade*>::iterator it, itNext;
    for (it=m_cmdCardCanTradeList.begin(); it!=m_cmdCardCanTradeList.end();)
    {
        pCmd = *it;
        it ++;
        OnCardCanTradeResult(pCmd->lSn, pCmd->sServer, pCmd->lUserID, pCmd->szCardID, 0, "关闭", 0);
    }
    for (it=m_cmdCardCanTradeList.begin(); it!=m_cmdCardCanTradeList.end(); it++)
    {
        delete (*it);
    }
    for (it=m_cmdCardCanTradeFree.begin(); it!=m_cmdCardCanTradeFree.end(); it++)
    {
        delete (*it);
    }
    m_cmdCardCanTradeList.clear();
    m_cmdCardCanTradeFree.clear();
}

void CLoginServer::CardCanTrade(long lSalesmanAccountID, const char* szSalesmanAccountName,
                                short sServer, long lCharID, char cCardType, const char* szCardID,
                                const char* szCardPwd, long lUserParam, short sTime)
{
    if (s_iChargeTest)
    {
        // 测试代码
        CGameWorldStub* pGws = FindGwsByGWSID(sServer);
        if (pGws)
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_card_can_trade_ret);
            g_sendCmd->WriteLong(lCharID);
            g_sendCmd->WriteByte(1); // 不能交易
            g_sendCmd->WriteString((char*)szCardID);
            g_sendCmd->WriteString((char*)"杂志免费试用卡");
            g_sendCmd->WriteLong(1);
            g_sendCmd->WriteLong(lUserParam);
            pGws->SendCmd(g_sendCmd);
        }else
        {
            LOG("CardCanTrade failed. Not use charge server\n");
        }
        return;
    }

    // 正式代码
    if (!m_bUseCharge)// change from if(m_bUseCharge) [3/18/2009 tooth.shi]加个！
    {
        SCmdCardCanTrade* pCmd = CmdCardCanTradeNew();
        pCmd->sServer = sServer;
        pCmd->lUserID = lCharID;
        strncpy(pCmd->szCardID, szCardID, 39);
        pCmd->szCardID[39] = 0;
        pCmd->tTimeout = time(NULL)+60; // 60秒超时
        pCmd->lSn = lUserParam;
//         m_chargeSession.CardCanTrade(lSalesmanAccountID, szSalesmanAccountName,
//             sServer, lCharID, cCardType, szCardID, szCardPwd, lUserParam, sTime);
    }else
    {
        CGameWorldStub* pGws = FindGwsByGWSID(sServer);
        if (pGws)
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_card_can_trade_ret);
            g_sendCmd->WriteLong(lCharID);
            g_sendCmd->WriteByte(0); // 不能交易
            g_sendCmd->WriteString((char*)szCardID);
            g_sendCmd->WriteString((char*)"可能是服务器忙，请稍候重试(1001)");
            g_sendCmd->WriteLong(0);
            g_sendCmd->WriteLong(lUserParam);
            pGws->SendCmd(g_sendCmd);
        }else
        {
            LOG("CardCanTrade failed. Not use charge server\n");
        }
    }
}

void CLoginServer::OnCardCanTradeResult(long lSn, short sServer, long lUserID,
                                        const char* szCardID, char cSucceeded,
                                        const char* szMsg, long lCardGroupType)
{
    if (CmdCardCanTradeRemove(lSn))
    {
#ifdef _DEBUG
        MSG4("%d:%d 检查点卡是否可以交易 [%s] %s, %s\n",
            sServer, lUserID, szCardID, (cSucceeded?"成功":"失败"), szMsg);
#endif
        CGameWorldStub* pGws = FindGwsByGWSID(sServer);
        if (pGws)
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_card_can_trade_ret);
            g_sendCmd->WriteLong(lUserID);
            g_sendCmd->WriteByte(cSucceeded);
            g_sendCmd->WriteString((char*)szCardID);
            g_sendCmd->WriteString((char*)szMsg);
            g_sendCmd->WriteLong(lCardGroupType);
            g_sendCmd->WriteLong(lSn);
            pGws->SendCmd(g_sendCmd);
        }else
        {
            LOG1("OnCardCanTradeResult %s\n", szMsg);
        }
		if (cSucceeded)
		{

		}
		else
		{
			SListCmd newCmd(NULL, LIST_CMD_CARD_TRADE_FAILD);
			newCmd.chargePoint.sServer		  = sServer;
			newCmd.chargePoint.lUserID		  = lUserID;
			strcpy(newCmd.chargePoint.szCardID,szCardID);
			strcpy(newCmd.chargePoint.cResult,szMsg);
			newCmd.chargePoint.lCardGroupType = lCardGroupType;
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add cardtrade failed cmd failed\n");
			}
		}
    }
}

void CLoginServer::OnCardCharged(const char* szCardID, long lCardGroupType)
{
    CGameWorldStub* pGws;
    TGWSMap::iterator it;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(lgs2gws_card_charged_event);
    g_sendCmd->WriteString((char*)szCardID);
    g_sendCmd->WriteLong(lCardGroupType);

    for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        pGws->SendCmd(g_sendCmd);
    }
}

void CLoginServer::GiftErrorRet(short sServer, long lUserID, char* szCardID, char* msg, long lUserParam,char szSucceed/* = 0*/)
{
	CGameWorldStub* pGws = FindGwsByGWSID(sServer);
	if (pGws)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(lgs2gws_charge_card_ret);
		g_sendCmd->WriteLong(lUserID);
		g_sendCmd->WriteByte(szSucceed); // 失败
		g_sendCmd->WriteString(szCardID);
		g_sendCmd->WriteString(msg);
		g_sendCmd->WriteLong(lUserParam);
		pGws->SendCmd(g_sendCmd);
	}else
	{
		LOG("error ret failed. Not use charge server\n");
	}
}

//tim.yang  VIP卡
void CLoginServer::AddCardTimeRet(short gwsid,long lUserid,char isRight,std::string time)
{
	CGameWorldStub* pGws = FindGwsByGWSID(gwsid);
	if (pGws)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(lgs2gws_add_account_time_ret);
		g_sendCmd->WriteLong(lUserid);
		g_sendCmd->WriteByte(isRight); // 失败
		g_sendCmd->WriteString((char*)time.c_str());
		pGws->SendCmd(g_sendCmd);
	}else
	{
		LOG("add vip cardtime error ret failed. Not use server\n");
	}
}
//end
// lSnCard 如果有值，就是指定卡的充值礼物，如果为0就是未知的充值礼物
void CLoginServer::AddGift(long lSnCard, const char* szUsername, short sServer,
                           long lUserID, short sLevel, short sType,
                           const char* szGiftString, long tLimitTime, char cFromCardID, char* szCardID)
{
    CGameWorldStub* pGws = FindGwsByGWSID(sServer);
    if (pGws)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_add_gift);
        g_sendCmd->WriteLong(lUserID);
        g_sendCmd->WriteShort(sLevel);
        g_sendCmd->WriteShort(sType);
        g_sendCmd->WriteString((char*)szGiftString);
        g_sendCmd->WriteLong(tLimitTime);
        if (cFromCardID)
        {
            g_sendCmd->WriteByte(1);
            g_sendCmd->WriteString(szCardID);
        }else
        {
            g_sendCmd->WriteByte(0);
        }
        pGws->SendCmd(g_sendCmd);
    }else
    {
        LOG4("Add [%s:%d:%d] gift [%s] failed\n", szUsername, sServer, lUserID, szGiftString);
    }
}

void CLoginServer::ClearGift(const char* szUsername, short sServer, long lUserID)
{
    CGameWorldStub* pGws = FindGwsByGWSID(sServer);
    if (pGws)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_clear_gift);
        g_sendCmd->WriteLong(lUserID);
        pGws->SendCmd(g_sendCmd);
    }else
    {
        LOG3("Clear [%s:%d:%d] gift failed\n", szUsername, sServer, lUserID);
    }
}

void CLoginServer::BroadcastCmdToGameworld(const char* szCommand)
{
    CGameWorldStub* pGws;
    TGWSMap::iterator it;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(lgs2gws_broadcast_to_gameworld);
    g_sendCmd->WriteString((char*)szCommand);

    for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        pGws->SendCmd(g_sendCmd);
    }
}

void CLoginServer::BroadcastCmdToAllRegion(const char* szCommand)
{
    CGameWorldStub* pGws;
    TGWSMap::iterator it;

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(lgs2gws_broadcast_to_all_region);
    g_sendCmd->WriteString((char*)szCommand);

    for (it=m_gwsMap.begin(); it!=m_gwsMap.end(); it++)
    {
        pGws = it->second;
        pGws->SendCmd(g_sendCmd);
    }
}

void CLoginServer::LogChargeCard(long lSn, SCmdCardChargeLog::ECCOP eOP,
                                 char cCardType, long lAccountID, short sServer,
                                 long lUserID, const char* szCardID)
{
    /*
    SListCmd newCmd(NULL, LIST_CMD_CHARGE_CARD_LOG);
    newCmd.logCardCharge.lSn = lSn;
    newCmd.logCardCharge.cOP = (char)eOP;
    newCmd.logCardCharge.cCardType = cCardType;
    newCmd.logCardCharge.sServer = sServer;
    newCmd.logCardCharge.lAccountID = lAccountID;
    newCmd.logCardCharge.lUserID = lUserID;
    strncpy(newCmd.logCardCharge.szCardID, szCardID, 39);
    newCmd.logCardCharge.szCardID[39] = 0;
    if (!g_recvCmdList.AddCmd(&newCmd))
    {
        ERR("ERROR: Add LogChargeCard cmd failed\n");
    }
    */
    static FILE* fp = NULL;
    if (fp==NULL)
    {
        fp = fopen("log_charge_card.txt", "wt+");
        if (fp)
        {
            fprintf(fp, "OP       : sn [account server user] [type:cardid]\n",
                lSn, lAccountID, sServer, lUserID, cCardType, szCardID);
            fflush(fp);
        }
    }
    if (fp)
    {
        switch (eOP)
        {
        case SCmdCardChargeLog::CCOP_CHARGE:
            fprintf(fp, "Charge   : %d [%d %d %d] [%d:%s]\n",
                lSn, lAccountID, sServer, lUserID, cCardType, szCardID);
            break;
        case SCmdCardChargeLog::CCOP_CHARGE_SUCCEEDED:
            fprintf(fp, "Succeeded: %d [%d %d %d] [%d:%s]\n",
                lSn, lAccountID, sServer, lUserID, cCardType, szCardID);
            break;
        case SCmdCardChargeLog::CCOP_CHARGE_FAILED:
            fprintf(fp, "Failed   : %d [%d %d %d] [%d:%s]\n",
                lSn, lAccountID, sServer, lUserID, cCardType, szCardID);
            break;
        case SCmdCardChargeLog::CCOP_CHARGE_TIMEOUT:
            fprintf(fp, "Timeout  : %d [%d %d %d] [%d:%s]\n",
                lSn, lAccountID, sServer, lUserID, cCardType, szCardID);
            break;
        case SCmdCardChargeLog::CCOP_CHARGE_CLOSED:
            fprintf(fp, "Closed   : %d [%d %d %d] [%d:%s]\n",
                lSn, lAccountID, sServer, lUserID, cCardType, szCardID);
            break;
        }
        fflush(fp);
    }
}

void CLoginServer::DoGameMasterCommand(CGameWorldStub* pStub, long lGMAccountID, const char* szCommand)
{
    //static char szCmdCopy[512];
	S_MEMDEF(szCmdCopy, 512)
	S_MEMPROTECTOR(szCmdCopy, 512, bMP)
    char seps[]   = " \t";
    char const* token = 0;
	char const* pUsername = 0;
	char const* p1=0;
	char const* p2=0;

    strncpy(szCmdCopy, szCommand, 511); szCmdCopy[511] = 0;
    token = strtok( (char*)szCmdCopy, seps );
    if (token)
    {
		if (stricmp(token, "Lock")==0) // AccountOP Lock 用户名 [0/1] 时间(分钟)
		{
			pUsername = strtok( NULL, seps );
			if (pUsername==NULL) return;
			p1 = strtok( NULL, seps );
			if (p1==NULL) return;
			p2 = strtok( NULL, seps );
			if (p2==NULL) p2 = "0";
            this->LockAccount(pStub, lGMAccountID, pUsername, (short)atoi(p1),
                (long)time(NULL), (long)time(NULL)+atol(p2)*60);
        }else if (stricmp(token, "SetPriv")==0) // AccountOP SetPriv 用户名 权限
		{
			pUsername = strtok( NULL, seps );
			if (pUsername==NULL) return;
			p1 = strtok( NULL, seps );
			if (p1==NULL) return;
            this->SetAccountPriv(pStub, lGMAccountID, pUsername, atol(p1));
        }else if (stricmp(token, "SetPwd")==0) // AccountOP SetPwd 用户名 密码
		{
			pUsername = strtok( NULL, seps );
			if (pUsername==NULL) return;
			p1 = strtok( NULL, seps );
			if (p1==NULL) return;
            this->SetAccountPassword(pStub, lGMAccountID, pUsername, p1);
        }else if (stricmp(token, "Charge")==0) // AccountOP Charge 1/0
		{
			p1 = strtok( NULL, seps );
			if (p1==NULL) return;
            if (atoi(p1)==1)
            {
                if (m_bConnectChargeServer)
                {
                    m_bUseCharge = true;
                }
            }else if (atoi(p1)==0)
            {
                m_bUseCharge = false;
            }
        }
    }
}
