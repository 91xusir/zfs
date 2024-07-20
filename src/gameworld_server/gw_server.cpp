#include "gw_server.h"
#include "lgs_gws.h"
#include "game_mail_shared_const.h"
#include "gw_nwar.h"

//PZH
#include "LogicExt.h"
//

#include "GW_ActivityManager.h"

CGwNationalWarMgr g_GWWarfieldMgr;
SConfigGameworldNet g_cfgGW;
GWServer            g_server;
CItemManager        g_ItemMgr;

void GWServer::Run()
{
    DWORD frameTime, tLogin, tGM, tRegion, tClient, tDatabase, tLogDB, tActDB, dwnow;

	m_state = ssRun;

	LOG("\nServer is running, waiting for all connections\n\n");	
	LOG("You can stop the server by Ctrl+C\n\n");

	time_t tLastUpdateStatInfo = 0;
	while(m_state == ssRun)
	{
		frameTime = rtGetMilliseconds();
		m_gwt.Update();

        // net log system
        if (m_pNetLog)
        {
            m_pNetLog->Process();
        }

		//m_reboot.Accept();
		//m_reboot.Process();
		m_login.Process();
        tLogin = rtGetMilliseconds();

        m_gm.Accept();
        m_gm.Process();
        tGM = rtGetMilliseconds();

        m_region.Accept();
        m_region.Process();
        tRegion = rtGetMilliseconds();

        RunClient();
        tClient = rtGetMilliseconds();

        m_database.ProcessQueryResult();
        tDatabase = rtGetMilliseconds();

		m_LogDb.ProcessQueryResult();
        tLogDB = rtGetMilliseconds();

		m_ActDB.ProcessQueryResult();
		tActDB = rtGetMilliseconds();

		time_t now = time(NULL);
        if(m_lastUpdateRegionListTimeStamp + GW_SEND_REGION_LIST_INTERVAL < now)
        {
            BroadcastUpdateRegionList();
            m_lastUpdateRegionListTimeStamp = now;
        }

        if(m_lastUpdateUserStateTimeStamp + GW_SAVE_USER_DATA_INTERVAL < now)
        {
            UpdateUserState();
			SaveUserData();
            m_lastUpdateUserStateTimeStamp = now;
        }
//by rendh 10.01.07
		if(m_lastLineCheckTimeStamp + GW_LINESYS_PENDING_CHECK_INTERVAL < now)
		{
			m_LineMan.OnTimer();
			m_lastLineCheckTimeStamp = now;
		}
//by rendh 10.01.07

//by rendh 10.05.07
#ifdef ENABLE_OPERATOR_NEW
		if(m_lastMemCheckTimeStamp + GW_MEM_CHECK_INTERVAL < now)
		{
			RtCoreMem().CheckAll();
			m_lastMemCheckTimeStamp = now;
		}
#endif
//by rendh 10.05.07
        if (m_lastTestServerTime)
        {
            if (now>m_lastTestServerTime+3) // 3 秒后回收测试信息
            {
                SendTestResult();
                m_lastTestServerTime = 0;
            }
        }

		if (now>m_lastOnlinetime+60) //Tianh 60秒LOG一次在线人数
		{
			if (!logOnline())
			{
				ERR("logOnline Error!\n");
			}
			m_lastOnlinetime = now;
		}

        if (now>m_lastUpdateBusiness+10) // 10 秒更新一次信息
        {
            if (m_iAllUpdateBusiness<=0)
            {
                m_cardTrade.RegionModify();
                m_gwAuction.RegionModify();
                m_iAllUpdateBusiness = 6; // 每 60 秒强制更新一次
            }
            m_cardTrade.Run();
            m_gwAuction.Run();
            m_lastUpdateBusiness = now;
            m_iAllUpdateBusiness --;
        }

        if(m_lostRegionCount)
        {
            for(int n = 0; n < (int)m_regionTable.size(); n++)
            {
                GWRegion& rg =m_regionTable[n];
                if(rg.lostTime) // && now - rg.lostTime > GW_REGION_LOST_TIMEOUT)
                {
					// 立刻处理
                    LOG2("Region server %s:%d disconnect timeout, logout all users in this region\n",
                        rg.szConnectGameworldIP.c_str(),
                        rg.lConnectGameworldPort);
                    this->m_dungeonMgr.OnRegionDisconnected(n);
					g_GWWarfieldMgr.OnRegionDisconnected(n);
					this->m_LineMan.OnRegionDisconnected(n);
                    LogoutUserByRegion(n);
                    rg.lostTime = 0;
                    rg.seed = 0;
                    m_lostRegionCount--;
                }
            }
        }

        if (now-tLastUpdateStatInfo>10) // 每10秒更新一次
        {
            m_stat.lUserCount = GetCurrentUserCnt();
            //m_stat.sv.UpdateInfo();
            m_login.UpdateGameWorldStat();
            tLastUpdateStatInfo = now;

			CVoteMgr::GetMgr().OnRun(now);
        }
        CHECK(m_lostRegionCount >= 0);

        GetUnionMgr()->Tick();
		GetFeodMgr()->Tick();

		//网站GM命令获得与执行
		GetGMOP();
		DisPatchPyGMOP();

		static time_t tHeroList = 0;
		if (now - tHeroList > HEROLIST_STAGGER_PERIOD+1)
		{
			GetHeroList()->OnRefresh();
			tHeroList = now;
		}

		static time_t tWarfield = 0;
		if (now - tWarfield > 5)
		{
			g_GWWarfieldMgr.Run();
			tWarfield = now;
		}

		GW_ActivityManager::GetSingle()->OnEvent();

		//static time_t tAuction = 0;
		//if (now-tAuction>2) // 每2秒更新一次
		//{
		//	g_SalesroomMgr.OnTimer();
		//	tAuction = now;
		//}
  //      

		ProcessUserForbidden();
		dwnow = rtGetMilliseconds();
		if ((dwnow-frameTime) > 5000)
        {
            ERR1("frame time = %d, dwnow = %d, frameTime = %d\n", dwnow - frameTime, dwnow, frameTime);
            if (tLogin-frameTime ) ERR1("      login    time = %d\n", tLogin-frameTime );
            if (tGM-tLogin       ) ERR1("      GM       time = %d\n", tGM-tLogin       );
            if (tRegion-tGM      ) ERR1("      Region   time = %d\n", tRegion-tGM      );
            if (tClient-tRegion  ) ERR3("      Client   time = %d [%d,%d]\n", tClient-tRegion, (int)m_unloginClient.size(), (int)m_loginClient.size() );
            if (tDatabase-tClient) ERR1("      Database time = %d\n", tDatabase-tClient);
			if (tLogDB-tDatabase ) ERR1("      LogDB    time = %d\n", tLogDB-tDatabase );
			if (tActDB-tLogDB ) ERR1("      ActDB    time = %d\n", tActDB-tLogDB );
        }else
        {
		    rtSleep(0.001 * 1000);
        }
	}
}

bool GWServer::MoveUserToWarfield(int userID, int x, int y, bool bBron,char faction)
{
	GWUser* user = FindActorById(userID);
	if (user == NULL)
		return false;

	int se = GetRegionSession(user->regionID);
	if (se == -1)
		return false;

	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_move_user); // 移动到国战场景中
	cmd.WriteLong(userID);
	cmd.WriteLong(x);
	cmd.WriteLong(y);
	cmd.WriteLong(0xffff);
	cmd.WriteLong(g_GWWarfieldMgr.m_stWarField.lRsidx);
	cmd.WriteByte(bBron?1:0);
	cmd.WriteByte(faction);

	return m_region.SendToPeer(se);
}

void GWServer::SaveUserData()
{
	int now = time(NULL);
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	for(it=m_tableByActorId.begin(); it!=m_tableByActorId.end(); it++)
	{
		GWLoginClient *lc = it->second;
		if(!lc->actorData) continue;
		if(now < lc->m_lastSaveTime + 300) continue;

		m_database.SaveUserData(lc->actorData,false,0,0);
		lc->m_lastSaveTime = now;
	}
}

void GWServer::SendKickAllUser(long lDelayTime)
{
    // 发送踢除命令给所有的Region
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_kick_all_user);
    cmd.WriteLong(lDelayTime);
    int n, seIdx;
	for (n=0; n < (int)m_regionTable.size(); n++)
	{
		seIdx = m_regionTable[n].sessionIndex;
		if (seIdx == -1)
			continue;
        m_region.SendToPeer(seIdx, &cmd);
	}

    // 断开所有已经连接但是没有登陆的连接
    GWUnloginClient* uc;
	list<GWUnloginClient*>::iterator itUC;
	for(itUC=m_unloginClient.begin(); itUC!=m_unloginClient.end();)
	{
		uc = *itUC;
		itUC = m_unloginClient.erase(itUC);
		DEL_ONE(uc->netLink);
		//if(uc->netLink) delete uc->netLink;
		DEL_ONE(uc);
		//delete uc;
    }

    // 断开所有在选人界面的玩家
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator itLC;
	for(itLC=m_loginClient.begin(); itLC!=m_loginClient.end(); itLC++)
	{
		GWLoginClient *lc = itLC->second;
        if (lc->state==LCS_SELECT_CHAR)
        {
            UserQuit(lc);
        }
	}
}

//
// 接受新的region的连接
//    如果region id为指定的id(0 base)的话就到指定id位置，否则随机位置
//    这边只检查是否允许连接
//  
bool GWServer::OnRegionAccept(GWSessionManager* pseManager, GWSession& se)
{
    // 查找Region ID
    short peerPort;
    char  peerAddr[256];
    unsigned long peerIP;

    if (!se.session->GetSocket().GetRemoteAddr(peerAddr, &peerPort, &peerIP))
        return false;

    peerIP = InvertIPOrder(peerIP);
    if (!g_cfgGW.listenRegion.allowIP.CheckAllow(peerIP, true))
    {
        LOG2("Access denied for session [%s:%u]\n", peerAddr, peerPort);
        return false;
    }
    return true;
}

bool GWServer::KickAccount(DWORD accountId, char cCause, char const* szCause) 
{ 
	GWLoginClient *lc = FindLoginClientByAccountId(accountId);
	if(lc == NULL)
	{
		m_login.LogoutAccount(accountId);
		return false;
	}

	// LOG2("Kick login account [ %s ][ %d ]\n", lc->info.accountName.c_str(),lc->state);
	switch(lc->state)
	{
		case LCS_INGAME:
			GWUser *user;
			user = lc->actorData;
			int seIdx;
			seIdx = m_regionTable[user->regionID].sessionIndex;
			if(seIdx == -1)
			{
				// 如果Region已经断线，则按regio断线流程处理，暂时无法踢帐号
				if (m_regionTable[user->regionID].lostTime)
				{
                    lc->m_bKickInSwitchRegion = true;
                    lc->m_cKickCause = cCause;
                    if (cCause==KICK_USER_USERDEFINE)
                    {
                        lc->m_szKickInfo[0] = 0;
                        if (szCause)
                        {
                            rt2_strncpy(lc->m_szKickInfo, szCause, 49);
                            //lc->m_szKickInfo[49] = 0;
                        }
                    }
					LOG1("The region %d is lost, can't kick account\n", user->regionID);
					return true;
				}
			}
			else // if (lc->kickCnt < GW_KICK_COUNT_CAP)
			{		
				// LOG3("Send the g2r_kick_user cmd to region %d for kick user %s (%d)\n", 
				//	user->regionID, user->uname.c_str(), lc->kickCnt);
				CG_CmdPacket& cmd = m_region.BeginCmd(g2r_kick_user);
				cmd.WriteLong(user->userID);
				cmd.WriteByte(cCause);
                if (cCause==KICK_USER_USERDEFINE)
                {
                    if (szCause==0) szCause = "";
				    cmd.WriteString(szCause);
                }
				g_server.m_region.SendToPeer(seIdx);
				lc->kickCnt++;
				ERR("GW kick user %s\n",szCause);
			}
			/*
			else if (lc->kickCnt >= GW_KICK_COUNT_CAP)
			{
				LOG2("Kick count is %d, kick the user %d by force\n", lc->kickCnt, user->userID);
				UserQuit(lc);
			}
			else
			{
				lc->kickCnt++;
			}
			*/
			break;
		
		case LCS_SELECT_CHAR:
			if (KICK_USER_MULTI_LOGIN == cCause)
			{
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(g2c_kick);
				g_sendCmd->WriteByte(cCause);
				lc->SendPacket(g_sendCmd);	
			}
			UserQuit(lc);
			break;

		case LCS_SWITCH_REGION:
			if (lc->IsPower())
			{
				UserQuit(lc);
			}
			else
			{
				lc->m_bKickInSwitchRegion = true;
				lc->m_cKickCause = cCause;
				if (cCause==KICK_USER_USERDEFINE)
				{
					lc->m_szKickInfo[0] = 0;
					if (szCause)
					{
						rt2_strncpy(lc->m_szKickInfo, szCause, 49);
						//lc->m_szKickInfo[49] = 0;
					}
				}
			}
			break;
	}

	return true;	
}

//
// distribute user to region,两种可能，第一次进游戏与切换region
//
bool GWServer::DistributeUser(GWLoginClient *lc,int seed,bool bLogin)
{
	if(!lc->actorData)
	{
		ERR1("DistributeUser failed, account = %s, data = NULL", lc->info.accountName.c_str());
		return false;
	}

	GWUser *user = lc->actorData;

    time_t tNow = time(NULL);
    GWSceneTable::iterator itSceneTable;

	if(user->IsInWarfield) //如果在国战场景内
	{
        // 如果是登陆状态就可以重新分配国战的场景ID (如果最后存盘时间超过5分钟，或者当前状态不是国战开启)
        // 如果只是在游戏中切景的状态就不可以，因为会导致玩家连接错误的Region而不能进入游戏
		//if(bLogin && ((time(NULL)-user->lastUpdateTime > 5*60) || (g_GWWarfieldMgr.eStarting!=g_GWWarfieldMgr.m_stWarField.eState)))
		if(bLogin && (g_GWWarfieldMgr.eStarting!=g_GWWarfieldMgr.m_stWarField.eState))
		{
			std::string block;
			long pos[3];
			if(FACTION_ZHOU == user->attr.faction)
			{
				block = g_GWWarfieldMgr.m_stWarField.stInfo.exitSceneBlockZhou;
				pos[0] = g_GWWarfieldMgr.m_stWarField.stInfo.exitPosZhou[0];
				pos[1] = g_GWWarfieldMgr.m_stWarField.stInfo.exitPosZhou[1];
				pos[2] = g_GWWarfieldMgr.m_stWarField.stInfo.exitPosZhou[2];
			}else
			{
				block = g_GWWarfieldMgr.m_stWarField.stInfo.exitSceneBlockShang;
				pos[0] = g_GWWarfieldMgr.m_stWarField.stInfo.exitPosShang[0];
				pos[1] = g_GWWarfieldMgr.m_stWarField.stInfo.exitPosShang[1];
				pos[2] = g_GWWarfieldMgr.m_stWarField.stInfo.exitPosShang[2];
			}
			itSceneTable = m_sceneTable.find(block.c_str());
			if (itSceneTable==m_sceneTable.end())
			{
				ERR1("DistributeUser failed, invalid bornSceneBlock %s[2]\n", block.c_str());
				return false;
			}
			lc->m_regionId   = (*itSceneTable).second;
			user->regionID   = (*itSceneTable).second;
			//MSG2("DistributeUser 找到退出点位置 rgID = %d [%s]\n", user->regionID, pInfo->bornSceneBlock.c_str());
			//user->IsInWarfield = false;
			user->attr.x =pos[0];
			user->attr.y = pos[1];
			user->scene = block;
		}else
		{
			lc->m_regionId   = g_GWWarfieldMgr.m_stWarField.lRsidx;
			user->regionID   = g_GWWarfieldMgr.m_stWarField.lRsidx;
			user->lWarfieldLastQuit = 0;
		}
	}
	else if (user->dungeonUID!=0)
    {
        CGwDungeonMgr::SGwDungeon* pDungeon = m_dungeonMgr.FindGwDungeon(user->dungeonUID);
 		if (pDungeon) // 超过三百秒就会退出副本
        {
	        lc->m_regionId   = pDungeon->iRegionIdx;
            user->regionID   = pDungeon->iRegionIdx;
            user->dungeonLastQuit = 0;
            //MSG2("DistributeUser 找到副本并且未超时 = %d [%d]\n", user->regionID, user->dungeonUID);
        }
		else
        {
            if (!bLogin)
            {
		        ERR("DistributeUser error\n");
            }
            // 如果副本已经不存在，就找到这个副本的退出信息，放到退出点。
            SFuBen* pInfo = g_TableFB.Find(user->dungeonTID);
            if (pInfo==NULL)
            {
		        ERR1("DistributeUser failed, invalid dungeonTID %d[3]\n", user->dungeonTID);
		        return false;
            }

            itSceneTable = m_sceneTable.find(pInfo->bornSceneBlock.c_str());
	        if (itSceneTable==m_sceneTable.end())
	        {
		        ERR1("DistributeUser failed, invalid bornSceneBlock %s[2]\n", pInfo->bornSceneBlock.c_str());
		        return false;
	        }
	        lc->m_regionId   = (*itSceneTable).second;
            user->regionID   = (*itSceneTable).second;
            //MSG2("DistributeUser 找到退出点位置 rgID = %d [%s]\n", user->regionID, pInfo->bornSceneBlock.c_str());
            user->dungeonUID = 0;
            user->dungeonTID = 0;
            user->attr.x = pInfo->exitPos[0];
            user->attr.y = pInfo->exitPos[1];
            user->scene = pInfo->bornSceneBlock;
            if (pDungeon==NULL)
            {
                user->dungeonLastQuit = (long)time(NULL);
            }else
            {
                m_dungeonMgr.LeaveDungeon(user->userID);
            }
        }
    }else if (user->LineSYSGroupID)//rendh 2010.1.9
    {
		int iregionID = -1;

		if (lc->m_switchRegionCnt == 0)
		{
			if(!m_LineMan.EnterLineReq(user->LineSYSGroupID, user->userID, user->LineSYSLineID, iregionID))
			{
				ERR1("DistributeUser failed, EnterLineReq\n");
				return false;
			}
		}else
		{
			if (!m_LineMan.GetUserRegion(user->LineSYSGroupID, user->LineSYSLineID, user->userID, iregionID))
			{
				ERR1("DistributeUser failed, EnterLineReq\n");
				return false;
			}
		}

		if (iregionID != -1)
		{
			lc->m_regionId = iregionID;
			user->regionID = iregionID;
		}

    }else//rendh 2010.1.9
    {
        itSceneTable = m_sceneTable.find(user->scene);
	    if (itSceneTable==m_sceneTable.end())
	    {
		    ERR1("DistributeUser failed, invalid regionID %d\n", user->regionID);
		    return false;
	    }
	    lc->m_regionId   = (*itSceneTable).second;
        user->regionID   = (*itSceneTable).second;
        //user->dungeonLastQuit = 0;
        //MSG2("DistributeUser 找到场景 rgID = %d [%s]\n", user->regionID, user->scene.c_str());
    }

    if (user->regionID>=(int)m_regionTable.size())
    {
		LOG("DistributeUser failed, scene table error\n");
		return false;
    }

	int rgIdx = m_regionTable[user->regionID].sessionIndex;
	if(rgIdx == -1)
	{
		LOG1("DistributeUser failed, region server %d is offline\n", user->regionID);
		return false;
	}

    //MSG2("DistributeUser to rgID = %d, DungeonUID=%d\n", user->regionID, user->dungeonUID);

	GWSession& se = m_region.GetSession(rgIdx);
	if(!se.isValid)
	{
		LOG1("DistributeUser failed, region session %d is invalid\n", se.index);
		return false;
	}

#ifdef _DEBUG
	LOG4("DistributeUser [ account=%s, userID=%d ] to %s on port %d\n", 
		lc->info.accountName.c_str(), 
		user->userID,
        m_regionTable[user->regionID].szListenClientIP.c_str(),
		m_regionTable[user->regionID].lListenClientPort);
#endif

	lc->m_switchRegionTime = time(NULL);
	lc->m_switchRegionSeed = seed; 
	lc->m_switchRegionCnt++;
	lc->SetState(LCS_SWITCH_REGION);

	if(lc->IsFirstSwitchRegion())
	{
		m_tableByActorName[user->name] = lc;
		m_tableByActorId[user->userID] = lc;
	}

	CUnion *pUnion = GetUnionMgr()->FindUnion(user->unionID);
	SUnionCitizen *pCitizen = NULL;
	if (pUnion)
		pCitizen = pUnion->Find(user->userID);
	if (pCitizen)
		pCitizen->m_nRegionID = user->regionID;

	user->seed = seed;
	if(lc->IsFirstSwitchRegion())
		user->cSwitchRegion = 0;
	else 
		user->cSwitchRegion = 1;

	user->privileges = lc->info.privileges;
	if(pCitizen)
	{
		user->unionName = pUnion->GetName();
		user->unionNickName = pCitizen->m_strNickName;
		user->attr.UnionRecord = pCitizen->m_nContribution;
		std::ostringstream os;
		os<<pUnion->GetIcon();
		user->unionIcon = os.str();
 		//user->unionIcon = pUnion->GetData()->icon;
	}else
	{
		user->unionName = "";
		user->unionNickName = "";
		user->unionIcon = "";
		user->attr.UnionRecord = 0;
	}
	
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_add_user);
	cmd.WriteLong(user->userID);
	user->Serialize(cmd,true);

	if (!m_region.SendToPeer(rgIdx))
	{
		LOG("DistributeUser failed, Network error\n");
		return false;
	}
	return true;
}

bool GWServer::Startup()
{
    LOG1("Startup Server ... at %d\n", rtTimestamp());//timn.yang  %s->%d

#ifdef WIN32
    m_stat.sv.UpdateMD51("gameworld_server.exe");
#else
    m_stat.sv.UpdateMD51("gameworld_server");
#endif
    m_stat.sv.UpdateMD52("gameworld_server.pak");

    // net log system
    m_pNetLog = RT_NEW CG_TCPBroadcastArchive;
    if (m_pNetLog)
    {
        m_pNetLog->Start(g_cfgGW.listenRegion.listenIP.c_str(), g_cfgGW.listenRegion.listenPort+5);
		// C [11/22/2008 Louis.Huang]
/*
        if (g_pMsg) ((RtOutputManager*)g_pMsg)->AddArchiveNode(m_pNetLog, false);
        if (g_pLog) ((RtOutputManager*)g_pLog)->AddArchiveNode(m_pNetLog, false);
        if (g_pError) ((RtOutputManager*)g_pError)->AddArchiveNode(m_pNetLog, false);
*/
    }

	m_cSubState = gwssWaitRegion;

    m_regionTable.resize(g_cfgGW.listenRegion.capacity);

	stlforeach(GWRegionTable, r, m_regionTable)
	{
		r->sessionIndex = -1;
		r->lostTime = 0;
		r->seed = 0;
	}

    if (!RS_Load())
    {
		ERR("Resource load err!!!\n");
        return false;
    }

	LoadSceneTable();

    // 这里要检查Region的个数是否和他匹配

    m_login.InitChannel();

    //m_reboot.Create("reboot", CreateRebootChannel(), "localhost", 6300, 2, NULL, 0, false, true);

    if (!m_login.Initalize())
	{
		ERR("Login channel initalize err!!!\n");
        return false;
	}

    if (!m_region.Create( "region", CreateRegionChannel(), 
        g_cfgGW.listenRegion.listenIP,
        g_cfgGW.listenRegion.listenPort,
        (int)g_cfgGW.listenRegion.capacity,
        &(g_cfgGW.listenRegion.allowIP),
        0, true, true))
    {
		ERR("Region channel initalize err!!!\n");
        return false;
    }
    m_region.SessionLost = OnRegionLost;
    m_region.SessionAccept = OnRegionAccept;
    m_lastUpdateRegionListTimeStamp = time(NULL);
	m_lastUpdateUserStateTimeStamp = time(NULL);
	m_lastLineCheckTimeStamp = time(NULL);
#ifdef ENABLE_OPERATOR_NEW
		m_lastMemCheckTimeStamp = time(NULL);
#endif
	m_region.SetBufferSize(1024);
	m_region.SetRowBufferSize(256*1024);

    if (!m_gm.Create( "gm", CreateGMChannel(),
        g_cfgGW.listenGM.listenIP,
        g_cfgGW.listenGM.listenPort,
        1, NULL, 0, true, true))
    {
		ERR("GM channel initalize err!!!\n");
        return false;
    }

	if (!m_database.Initalize())
	{
		ERR("Database initalize err!!!\n");
		return false;
	}
	RtIni*pRule = GetGameRule();
	if (NULL != pRule)
	{
		long nLev = 20;
		bool bOK = false;
		bOK = pRule->GetEntry("GameLevelRule", "DeleteLevel", &nLev);
		if (bOK)
		{
			//nLev = atol(szLev);
			m_database.SetDeleteLevel(nLev);
		}
	}
	if (!m_LogDb.Initalize())
	{
		ERR("Log database initalize err!!!\n");
		return false;
	}

	if (!m_ActDB.Initalize())
	{
		ERR("Activity database initalize err!!!\n");
		return false;
	}

	// 诸侯国，依赖于DB
	if (!g_UnionMgr.Init())
	{
		ERR("Union startup FAILED\n");
		return false;
	}
	
    // 采邑，依赖于DB
	if (!g_FeodMgr.Init())
	{
		ERR("Feod startup FAILED\n");
		return false;
	}
	
	// 投票系统，依赖于DB
	if (0 != CVoteMgr::GetMgr().Load())
	{
		ERR("Vote startup FAILED\n");
		return false;
	} 
	//-------------add start by tony 05.06.17---------------------//
	if(!g_CastlewarMgr.Init())
	{
		ERR("Castlewar startup FAILED\n");
		return false;
	}
	//-------------add end   by tony 05.06.17---------------------//

	if(!g_GWWarfieldMgr.Init())
	{
		ERR("Warfield load error\n");
		return false;
	}
    /*
	//-------------add start by tony 05.06.17---------------------//
	if(!g_SalesroomMgr.Init())
	{
		ERR("Salesroom startup FAILED\n");
		return false;
	}
	//-------------add end   by tony 05.06.17---------------------//
    */
	
	if (!GetHeroList()->Init())
	{
		ERR("HeroList startup FAILED\n");
		return false;
	}

	if (!g_GWItemIDFactory.Init())
	{
		ERR("Item ID Factory startup FAILED\n");
		return false;
	}

    if (!m_dungeonMgr.Startup())
    {
		ERR("Dungeon manager startup FAILED\n");
		return false;
    }

    if (!m_cardTrade.Startup())
    {
		ERR("Card trade manager startup FAILED\n");
		return false;
    }

    if (!m_gwAuction.Startup())
    {
		ERR("Auction manager startup FAILED\n");
		return false;
    }

	if (!m_LineMan.Startup())
	{
		ERR("Line manager startup FAILED\n");
		return false;
	}

	if(!GW_ActivityManager::GetSingle()->StartUp())
	{
		ERR("Activity manager startup FAILED!\n");
		return false;
	}

	if(!StartClientListen())
    {
		ERR("Client channel initalize err!!!\n");
        return false;
    }

	if (!LogServerSender_Init())
	{
		ERR("LogServerSender Init FAILED, Ignored!\n");
	}

    m_lastUpdateBusiness = 0;
	m_lastOnlinetime = 0;
    m_iAllUpdateBusiness = 5;

	UpdateUserIsOnLine(0);
	return true;
}

void GWServer::Exit()
{
	LOG("\n\n");
	LOG("Broadcast the shutdown message\n");

	UpdateUserIsOnLine(0);


    m_gwAuction.Shutdown();

    m_dungeonMgr.Shutdown();

	m_LineMan.Shutdown();

	GW_ActivityManager::GetSingle()->Shutdown();

    // 关闭客户端的连接，理由是服务器维护
    // m_client.Stop(true, 0);

    // 发送关机命令到各个Region
	for (int n = 0; n < (int)m_regionTable.size(); n++)
	{
		int seIdx = m_regionTable[n].sessionIndex;

		if (seIdx == -1)
			continue;

		m_region.NotifyCmd(seIdx, g2r_shutdown);
	}

	LOG1("Warting for %d regions shutdown\n", m_region.GetValidSessionCount());

    bool bForce = false;
    DWORD dwCloseRegionTimeout = rtGetMilliseconds() + 30*1000; // 30秒后超时，强制关闭所有的连接
    while(m_region.GetValidSessionCount() > 0)
	{
		m_region.Process();
		m_database.ProcessQueryResult();
		m_LogDb.ProcessQueryResult();
		m_ActDB.ProcessQueryResult();
        if (rtGetMilliseconds()>dwCloseRegionTimeout)
        {
            bForce = true;
            break; // 强制关闭连接
        }
		rtSleep(0.001 * 1000);
	}

	// 强行保存内存中的所有玩家资料，包括正在异步存盘的玩家
	m_database.Stop();
	LOG("Save all user data\n");
	int succeeded = 0;
	int failed = 0;
	
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	for(it=m_loginClient.begin(); it!=m_loginClient.end(); it++)
	{
		GWLoginClient *lc = it->second;
		if(!lc->actorData) continue;
		if(m_database.SaveUserData(lc->actorData, true, NULL, NULL))
			succeeded++;
		else
			failed++;
	}
	LOG3("There are %d succeeded, %d failed of %d users be stored\n", succeeded, failed, m_loginClient.size());

	LOG("Close log db\n");
	m_LogDb.Stop();

	LOG("Close activity db\n");
	m_ActDB.Stop();

	LOG("All connections had been closed\n");	
    if (bForce)
    {
        LOG("Notify the login server, 'Shutdown FORCE'\n");
    }
	else
    {
        LOG("Notify the login server, 'Shutdown OK'\n");
    }

	m_login.SendShutdownOK(bForce);
    rtSleep(0.01 * 1000); // 等待10毫秒，用于网络传输
    
    if (m_pNetLog)
    {
		// C [11/22/2008 Louis.Huang]
/*
        if (g_pMsg) ((RtOutputManager*)g_pMsg)->RemoveArchiveNode(m_pNetLog);
        if (g_pLog) ((RtOutputManager*)g_pLog)->RemoveArchiveNode(m_pNetLog);
        if (g_pError) ((RtOutputManager*)g_pError)->RemoveArchiveNode(m_pNetLog);
*/
        DEL_ONE(m_pNetLog);
        //m_pNetLog = NULL;
    }

	LogServerSender_Close();
}

void GWServer::OnAcceptLogin(bool bCanLogin)
{
    m_login.m_acceptLogin = bCanLogin;
}

void GWServer::SendRegionList(long session)
{
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_update_region_list);

	for (long rgID = 0; rgID < (long)m_regionTable.size(); rgID++)
	{
		GWRegion& rg = m_regionTable[rgID];

		if (rg.sessionIndex == -1)
			continue;

		if (!m_region.m_sessions[rg.sessionIndex].isValid)
			continue;

		cmd.WriteLong(rgID);
		cmd.WriteString((char*)g_server.m_regionTable[rgID].szListenClientIP.c_str());
		cmd.WriteShort(g_server.m_regionTable[rgID].lListenClientPort);
	}

	cmd.WriteLong(-1l);

	m_region.SendToPeer(session);
}

void GWServer::BroadcastUpdateRegionList()
{
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_update_region_list);

	for (long rgID = 0; rgID < (long)m_regionTable.size(); rgID++)
	{
		GWRegion& rg = m_regionTable[rgID];

		if (rg.sessionIndex == -1)
			continue;
				
		if (!m_region.m_sessions[rg.sessionIndex].isValid)
			continue;

		cmd.WriteLong(rgID);
		cmd.WriteString((char*)m_regionTable[rgID].szListenClientIP.c_str());
		cmd.WriteShort(m_regionTable[rgID].lListenClientPort);
	}

	cmd.WriteLong(-1l);

	m_region.BroadcastCmd();
}

void GWServer::SendRegionInit(long regionID)
{
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_init_region);

	GWRegion& rg = m_regionTable[regionID];

	cmd.WriteLong(m_gwt.Ticks());
	cmd.WriteFloat(m_gwt.Scaling());
	cmd.WriteLong(regionID);
	//cmd.WriteLong(rg.port);
	cmd.WriteLong(rg.seed);

    cmd.WriteString((char*)m_strLoginName.c_str());
    cmd.WriteString((char*)g_cfgGW.game.szGameworldName.c_str());

	cmd.WriteLong((long)m_sceneTable.size());	

	stlforeach(GWSceneTable, iter, m_sceneTable)
	{
		cmd.WriteString((char*)iter->first.c_str());
		cmd.WriteLong(iter->second);
	}

	m_region.SendToPeer(rg.sessionIndex);
}

void GWServer::BroadcastAddRegionList(long rgID)
{
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_add_region);
	
	cmd.WriteLong(rgID);
	cmd.WriteString((char*)m_regionTable[rgID].szListenClientIP.c_str());
	cmd.WriteShort(m_regionTable[rgID].lListenClientPort);

	m_region.BroadcastCmd();
}

void GWServer::BroadcastRemoveRegionList(long rgID)
{
	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_remove_region);

	cmd.WriteLong(rgID);
	
	m_region.BroadcastCmd();
}

bool GWServer::OnRegionLost(GWSessionManager* pseManager, GWSession& se)
{
	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];

		if (rg.sessionIndex == se.index)
		{
			rg.sessionIndex = -1;
			rg.lostTime = time(NULL);

			g_server.m_lostRegionCount++;
            g_server.OnRegionSessionChange();
			g_server.BroadcastRemoveRegionList(rgID);
			return true;
		}
	}
    return true;
}

void GWServer::UpdateUserState()
{
	int now = time(NULL);
	
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	for(it=m_loginClient.begin(); it!=m_loginClient.end(); it++)
	{
		GWLoginClient *lc = it->second;
		if(lc->GetState() == LCS_SWITCH_REGION)
		{
			if(now > lc->m_switchRegionTime + GW_USER_SWITCH_REGION_TIMEOUT)
			{
				UserQuit(lc);
			}
		}
	}
}

void GWServer::OnUserEnterWorld(GWLoginClient *lc)
{
	GWUser *user = lc->actorData;

	if(user->unionID != 0)
		GetUnionMgr()->OnCitizenComming(user->userID, user->unionID, user->regionID);

	//-------------add start by tony 05.06.16-------------------------//
	GetCastleWarMgr()->OnUserOnline(user->regionID, user->userID);
	//-------------add end   by tony 05.06.16-------------------------//
	g_GWWarfieldMgr.OnUserOnline(user->userID);

	user->enterTime = time(NULL);
	m_mail.CheckMail(user->userID);
	// m_tableByActorName[user->name] = lc;
	// m_tableByActorId[user->userID] = lc;

	// 通知GMServer
	CG_CmdPacket& cmd = g_server.m_gm.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(gw2gm_userstate);
	cmd.WriteLong(user->userID);
	cmd.WriteByte(1);
	g_server.m_gm.SendCmd();

	// Log the Event
	if (!LogUserEvent(PLT_UserState, user->userID, lc->info.ip.c_str(), 1, "", user->money, "", user->bankMoney, ""))
	{
		ERR("LogUserEvent Error!\n");
	}

    // 发送到Login
    CG_CmdPacket& loginCmd = g_server.m_login.BeginCmd(gws2lgs_user_enter_game);
	loginCmd.WriteLong(user->accountId);
    loginCmd.WriteLong(user->attr.level);
    g_server.m_login.SendCmd();
}

int SConnection::mVersion=0;
void FriendLeave(GWUser* vpUser)
{
	string FriendString=vpUser->friends;

	//将friends的信息读出，并检查版本
	int ConnectionCount=0;
	

	FriendString>>SConnection::mVersion;

	//版本变化时候要记的改
	if(SConnection::mVersion != LAST_FRIEND_VERSION)
	{//不是当前的//LAST_FRIEND_VERSION版本
		ERR("friend data version error!\n");
		return ;
	}


	FriendString>>ConnectionCount;
	for(int i=0;i!=ConnectionCount;++i)
	{
		SConnection Data;
		FriendString>>Data;//数据读入临时变量

		if(Data.Is(SConnection::FRIEND) || Data.Is(SConnection::FAKEBLACKLIST))
		{//是朋友关系或者临时好友
			//将离开的消息发给每个好友

			GWUser* pUser = g_server.FindActorById(Data.mDBID);

			if (pUser == NULL)
			{
				continue;
			}

			int RegionID = g_server.GetRegionSession(pUser->regionID);

			if (RegionID == -1)
			{
				continue;
			}

			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_friend);
			cmd.WriteLong(Data.mDBID);
			cmd.WriteByte(R2R_UPDATE_OTHER_FRIEND_TO_SELF);		
			cmd.WriteLong(0);	
			cmd.WriteLong(vpUser->userID);		
			cmd.WriteString((char*)vpUser->name.c_str());		
			cmd.WriteLong(0);		
			cmd.WriteByte(FALSE);			
			cmd.WriteString("");
			cmd.WriteByte(0);
			if (Data.Is(SConnection::FAKEBLACKLIST))
			{
				cmd.WriteByte(0);
			}
			else
			{
				cmd.WriteByte(1);
			}
			
			g_server.m_region.SendToPeer(RegionID);
		}
	}
}


void GWServer::OnUserLeaveWorld(GWLoginClient *lc)
{
	GWUser *user = lc->actorData;

	if(user->enterTime && (time(NULL) - user->enterTime) > 0)
		user->onlineTime += (time(NULL) - user->enterTime);

	//队伍
	m_team.OnOffLine(user);

	//好友
	FriendLeave(user);

	//师傅列表
	m_master_list_manager.Remove(user);

	//国战
	if(user)
	{
		g_GWWarfieldMgr.OnUserOffline(user->userID);
	}

	if(user->unionID != 0)
	{
		GetUnionMgr()->OnCitizenGoing(user->userID, user->unionID);
	}

	//分线
	if (user->LineSYSGroupID && user->LineSYSLineID)
	{
		m_LineMan.LeaveLineReq(user->LineSYSGroupID, user->LineSYSLineID, user->userID);
	}

	// 通知GMServer
	CG_CmdPacket& cmd = g_server.m_gm.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(gw2gm_userstate);
	cmd.WriteLong(user->userID);
	cmd.WriteByte(2);
	g_server.m_gm.SendCmd();

	// Log the event
	if (!LogUserEvent(PLT_UserState, user->userID, lc->info.ip.c_str(), 2, "", user->money, "", user->bankMoney, ""))
	{
		ERR("LogUserEvent Error!\n");
	}

	user->IsOnline = false;
	UpdateUserIsOnLine(0,user->userID);
}

void GWServer::OnUserSwitchRegion(int userID)
{
	
}

void GWServer::OnUserSwitchRegionSucceeded(GWLoginClient *lc)
{
	if(lc==NULL)
	{
		return;
	}

	if(lc->actorData==NULL)
	{
		return;
	}

	//切换region后，重建region的team数据
	m_team.SwitchRegion(lc->actorData);

    if (lc->m_bKickInSwitchRegion)
    {
        KickAccount(lc->info.accountId, lc->m_cKickCause, lc->m_szKickInfo);
    }
	return;
}

void GWServer::OnUserDelete(int userID)
{
	g_UnionMgr.OnDeleteUser(userID);
}

bool GWServer::LoadSceneTable()
{
	RtCsv csv;

	if(!csv.LoadTableFile(R(RES_TABLE_SCENE)))
	{
		LOG1("Load scene table [%s] failed\n", "sv_table/scene.csv");

		return false;
	}

	int numLines = csv.GetLineCount();

	if (numLines <= 1)
	{
		LOG("Invalid scene table formation\n");
		return false;
	}

	csv.SetTitleLine(1);
    int regionID;

	for (int lineID = 2; lineID < numLines; lineID++)
	{
		const char* sceneFileName = csv[lineID]["场景文件"].Str();
		regionID = csv[lineID]["区域ID"].Int();

//  [12/20/2009 rendh] 无用的东西
//		isDungeon = csv[lineID]["副本"].Int();
//  [12/20/2009 rendh]

		m_sceneTable[sceneFileName] = regionID;
	}

	return true;
}

void GWServer::LogoutUserByRegion(int regionID)
{
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	for(it=m_tableByActorId.begin(); it!=m_tableByActorId.end(); it++)
	{
		GWLoginClient *lc = it->second;
		if(lc->m_regionId == regionID)
		{
			UserQuit(lc);
		}
	}
}

void GWServer::OnRegionRegister(int regionID)
{
	GetUnionMgr()->OnAddNewRegion(regionID);
	GetHeroList()->OnAddNewRegion(regionID);
	g_GWItemIDMgr.OnRegionRegister(regionID);
	g_FeodMgr.OnRegionRegister(regionID);
	//--------------add start by tony 05.06.20----------------------------//
	g_CastlewarMgr.OnRegionRegister(regionID);
	//--------------add start by tony 05.06.20----------------------------//
	//g_SalesroomMgr.OnRegionRegister(regionID);
	g_GWWarfieldMgr.OnRegionRegister(regionID);
    m_cardTrade.RegionModify();
    m_gwAuction.RegionModify();
    OnRegionSessionChange();
}

void GWServer::OnRegionSessionChange()
{
    long rgID;
    for (rgID=0; rgID<(long)m_regionTable.size(); rgID++)
    {
        GWRegion& rg = m_regionTable[rgID];
        if (rg.sessionIndex == -1)
            break;
    }
    char cNewState;
    if (rgID==(long)m_regionTable.size())
    {
        cNewState = gwssAcceptLogin;
    }else
    {
        cNewState = gwssWaitRegion;
    }
    if (m_cSubState!=cNewState)
    {
        m_cSubState = cNewState;
        m_login.UpdateState(m_cSubState);
        switch (m_cSubState)
        {
        case gwssWaitRegion:
            LOG("Change Game World State: Waitting region server\n");
            break;
        case gwssAcceptLogin:
            LOG("Change Game World State: Accept login\n");
            break;
        }
    }
}

void GWServer::OnFinalSaveFinish(GWDBResult* result)
{
	int id = (int)result->data;
	GWLoginClient *lc = g_server.FindLoginClientByAccountId(id);
	if(lc)
	{
		//-------add start by tony 06.04.07--------------------//
		//-reason: 此处有可能在玩家退出之前角色数据已经被清空
		if(lc->actorData)
		{
		//-------add end  by tony 06.04.07---------------------//
			g_server.m_tableByActorId.erase(lc->actorData->userID);
			g_server.m_tableByActorName.erase(lc->actorData->uname);
		}
		g_server.OnRealQuit(lc);
	}
}


void GWServer::OnRealQuit(GWLoginClient *lc)
{
	lc->valid = false;
}

bool GWServer::PullUser(int target, int source)
{
	GWUser* tu = FindActorById(target);

	if (tu == NULL)
		return false;

	int se = GetRegionSession(tu->regionID);

	if (se == -1)
		return false;

	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_pull_user);

	cmd.WriteLong(target);
	cmd.WriteLong(source);

	return m_region.SendToPeer(se);	
}

bool GWServer::PullUserAround(int target, int source, short nRange)
{
	GWUser* tu = FindActorById(target);

	if (tu == NULL)
		return false;

	int se = GetRegionSession(tu->regionID);

	if (se == -1)
		return false;

	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_pull_useraround);

	cmd.WriteLong(target);
	cmd.WriteLong(source);
	cmd.WriteShort(nRange);

	return m_region.SendToPeer(se);	
}

bool GWServer::MoveUser(int userID, int x, int y)
{
	GWUser* user = FindActorById(userID);
	if (user == NULL)
		return false;

	int se = GetRegionSession(user->regionID);
	if (se == -1)
		return false;

	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_move_user); // 移动到普通场景中
	cmd.WriteLong(userID);
	cmd.WriteLong(x);
	cmd.WriteLong(y);
	cmd.WriteLong(0);

	return m_region.SendToPeer(se);
}

// userID 用户的ID
// iDungeonID 副本的ID
bool GWServer::MoveUserToDungeon(int userID, int iDungeonID, int x, int y, bool bBron)
{
	GWUser* user = FindActorById(userID);
	if (user == NULL)
		return false;

	int se = GetRegionSession(user->regionID);
	if (se == -1)
		return false;

    CGwDungeonMgr::SGwDungeon* pDungeon = m_dungeonMgr.FindGwDungeon(iDungeonID);
    if (pDungeon==NULL)
    {
        ERR("Can't found dungeon.\n");
        return false;
    }

	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_move_user); // 移动到副本场景中
	cmd.WriteLong(userID);
	cmd.WriteLong(x);
	cmd.WriteLong(y);
	cmd.WriteLong(iDungeonID);
    cmd.WriteLong(pDungeon->lDungeonType);
    cmd.WriteLong(pDungeon->iRegionIdx);
    cmd.WriteByte(bBron?1:0);

	return m_region.SendToPeer(se);
}

bool GWServer::MoveUserAround(int userID, int x, int y, short nRange)
{
	GWUser* user = FindActorById(userID);
	if (user == NULL)
		return false;

	int se = GetRegionSession(user->regionID);
	if (se == -1)
		return false;

	CG_CmdPacket& cmd = m_region.BeginCmd(g2r_move_useraround);
	cmd.WriteLong(userID);
	cmd.WriteLong(x);
	cmd.WriteLong(y);
	cmd.WriteShort(nRange);

	return m_region.SendToPeer(se);
}

int GWServer::GetFreeRegion()
{
	for (int n = 0; n < (int)m_regionTable.size(); n++)
		if (m_regionTable[n].sessionIndex == -1)
			return n;

	return -1;
}

#define reboot 12

GW_BEGIN_CMD(cmd_reboot)
{
	g_server.m_state = ssReboot;
}
GW_END_CMD;

/*
GWCommandChannel* GWServer::CreateRebootChannel()
{
	GWCommandChannel* channel = RT_NEW GWCommandChannel;

#define REGISTER_COMMAND(c) channel->RegisterCommand(c, cmd_##c, #c);

	REGISTER_COMMAND(reboot);

#undef REGISTER_COMMAND

	return channel;
}
*/

/*Tianh 把在线人数写入数据库 2010.01.08*/
bool GWServer::logOnline()
{
	bool bSync = false;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command,
		"insert into t_log_user_onlineamount (OnlineAmount,LogOnlineTime) values (%d,FROM_UNIXTIME(%u))",
		GetCurrentUserCnt(),
		time(NULL));

	if (bSync)
	{
		if ( !g_server.m_LogDb.ExecCommand(cmd.command, &pDBResult) )
		{
			return false;
		}
		mysql_free_result(pDBResult);
	}
	else 
	{
		g_server.m_LogDb.PushCommand(cmd);
	}

	return true;
}

bool GWServer::DeleteAllMail(long userID) 
{
	bool bSync = false;

	GWDBCommand cmd;

	MYSQL_RES *pDBResult;

	rt2_sprintf(cmd.command, "DELETE FROM mail WHERE ReceiverID=%d",userID);

	if (bSync)
	{
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			return false;
		}
		mysql_free_result(pDBResult);
	}
	else 
	{
		g_server.m_database.PushCommand(cmd);
	}

	return true;
}

bool GWServer::LogUserEvent(char type, long userID, const char* ip, long longParam1, const char* strParam1, long longParam2, const char* strParam2, long longParam3, const char* strParam3)
{
	bool bSync = false;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "INSERT INTO %s(OperateID, type, userID, longParam1, strParam1, longParam2, strParam2, longParam3, strParam3, LogTime, userIP) VALUES("
		"'',"
		"%d,"
		"%d,"
		"%d,"
		"'%s',"
		"%d,"
		"'%s',"
		"%d,"
		"'%s',"
		"FROM_UNIXTIME(%u),"
		"'%s')",
		m_LogDb.GetTableName().c_str(),
		type,
		userID,
		longParam1,
		strParam1,
		longParam2,
		strParam2,
		longParam3,
		strParam3,
		time(NULL),
		ip);

	if (bSync)
	{
		if ( !g_server.m_LogDb.ExecCommand(cmd.command, &pDBResult) )
		{
			return false;
		}
		mysql_free_result(pDBResult);
	}
	else 
	{
		g_server.m_LogDb.PushCommand(cmd);
	}

	return true;
}

// ----------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------
bool GWServer::StartClientListen()
{
	CreateClientChannel();
	if(!m_clientListen.StartListen(g_cfgGW.listenClient.listenIP.c_str(),
		g_cfgGW.listenClient.listenPort,true))
    {
        LOG2("Listen client failed. IP= %s:%d\n", g_cfgGW.listenClient.listenIP.c_str(), g_cfgGW.listenClient.listenPort);
		return false;
    }

    LOG2("Listen client succeeded. IP= %s:%d\n", g_cfgGW.listenClient.listenIP.c_str(), g_cfgGW.listenClient.listenPort);
	return true;
}


GWUnloginClient *GWServer::FindUnloginClientBySeed(int seed)
{
	list<GWUnloginClient*>::iterator it;
	for(it=m_unloginClient.begin(); it!=m_unloginClient.end(); it++)
	{
		if((*it)->seed == seed) return *it;
	}
	return NULL;
}


GWLoginClient *GWServer::FindLoginClientByAccountId(int accountId)
{
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	it = m_loginClient.find(accountId);
	if(it != m_loginClient.end())
	{
		return it->second;
	}
	return NULL;
}

GWLoginClient *GWServer::FindLoginClientByActorId(int actorId)
{
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	it = m_tableByActorId.find(actorId);
	if(it != m_tableByActorId.end())
	{
		return it->second;
	}
	return NULL;
}
/*
GWUser* GWServer::FindActorByIdWithoutSwitchRegion(int actorId)
{
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	it = m_tableByActorId.find(actorId);
	if(it != m_tableByActorId.end())
	{
		GWLoginClient *cl = it->second;
		if(!cl->actorData || cl->GetState() == LCS_QUITING || cl->GetState()== LCS_SWITCH_REGION)
		{
			return NULL;
		}
		return cl->actorData;
	}
	return NULL;
}
*/
GWUser *GWServer::FindActorById(int actorId)
{
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator it;
	it = m_tableByActorId.find(actorId);
	if(it != m_tableByActorId.end())
	{
		GWLoginClient *cl = it->second;
		if(!cl->actorData || cl->GetState() == LCS_QUITING)
		{
			return NULL;
		}
		return cl->actorData;
	}
	return NULL;
}

GWUser *GWServer::FindActorByName(const char *name)
{
	HASH_STR(std::string, GWLoginClient*)::iterator it;
	it = m_tableByActorName.find(name);
	if(it != m_tableByActorName.end())
	{
		GWLoginClient *cl = it->second;
		if(!cl->actorData || cl->GetState() == LCS_QUITING)
		{
			return NULL;
		}
		return cl->actorData;
	}
	return NULL;
}

GWUnloginClient *GWServer::CreateUnloginClient(CG_TCPSession *netLink)
{
	GWUnloginClient *uc;
	uc = RT_NEW GWUnloginClient;
	uc->netLink = netLink;
	uc->state   = UCS_IDLE;
	uc->seed	= m_linkSeed++;
	uc->connectTime = rtGetMilliseconds();
    uc->valid = true;
    uc->netLink->SetDebugStr("ClientSession");

	return uc;
}

void GWServer::RunClient() 
{
	//
	// try accept new net link
	//
	GWUnloginClient *uc;
	GWLoginClient *lc;
	CG_TCPSession *session;

	static int linkSeed = 1;
	bool accept = (m_unloginClient.size() < 100) && m_login.CanLogin();

	// avoid accept too many link in one cycle
	do 
	{
		session = m_clientListen.AcceptNewSession();
		if(!session) break;
		session->EnableRemoteCompress(false);

		if(!accept)
		{
			DEL_ONE(session);
		}
		else
		{
			uc = CreateUnloginClient(session);
			m_unloginClient.push_back(uc);
		}

	} while(session);

	//
	// process unlogin client
	//
	list<GWUnloginClient*>::iterator it;
	for(it=m_unloginClient.begin(); it!=m_unloginClient.end();)
	{
		uc = *it;

		if(!uc->valid || ((rtGetMilliseconds() - uc->connectTime) > 40*1000))
		{
			it = m_unloginClient.erase(it);
			DEL_ONE(uc->netLink);
			DEL_ONE(uc);
			//if(uc->netLink) delete uc->netLink;
			//delete uc;
			continue;
		}

		uc->netLink->DefProc();
		if(uc->netLink->IsDead())
		{
			uc->valid = false;
			continue;
		}

		m_curUnloginClient = uc;
		short cmdId;
		while(uc->netLink->GetPacket(g_recvCmd))
		{
			if(!g_recvCmd->ReadShort(&cmdId)) 
				break;
			m_channelUnlogin.Invoke(cmdId,g_recvCmd);
		}
		it++;
    }

	// 
	// process login client
	//
	vector<GWLoginClient*> delList;
	EXT_SPACE::unordered_map<DWORD, GWLoginClient*>::iterator itl;
	for(itl=m_loginClient.begin(); itl!=m_loginClient.end(); itl++)
	{
		lc = itl->second;

		if(!lc->valid)
		{
			delList.push_back(lc);
			continue;
		}

		if(!lc->netLink) continue;

		lc->netLink->DefProc();
		if(lc->netLink->IsDead())
		{
			UserQuit(lc);
			continue;
		}

		m_curLoginClient = lc;
		short cmdId;
		while(lc->netLink && lc->netLink->GetPacket(g_recvCmd))
		{
			if(!g_recvCmd->ReadShort(&cmdId)) 
				break;
			m_channelLogin.Invoke(cmdId,g_recvCmd);
		}
	}

	for(int i=0; i<(int)delList.size(); i++)
	{
		GWLoginClient *lc = delList[i];
		m_loginClient.erase(lc->info.accountId);
		//PZH
		bool bPower = lc->IsPower();
		if(!bPower)
		{
			g_server.m_login.LogoutAccount(lc->info.accountId);
		}
		else
		{
			lc->PowerLogin();
		}
		//
		DEL_ONE(lc->netLink);
		DEL_ONE(lc->actorData);
		DEL_ONE(lc);
		//if(lc->netLink) delete lc->netLink;
		//if(lc->actorData) delete lc->actorData;
		//delete lc;
	}
	//PZH
	static unsigned long s_dwNext = 0;
	const unsigned long c_cwSetp = 500;
	unsigned long dwTime = rtGetMilliseconds();
	if(dwTime > s_dwNext)
	{
		CLogicExt::GetCLogicExt().OnRun(dwTime);
		s_dwNext = dwTime + c_cwSetp;
	}
	//
}

bool GWServer::UserLogin(GWUnloginClient *ulc,SAccountInfo *info)
{
	GWLoginClient *lc = FindLoginClientByAccountId(info->accountId);
	if(lc)
	{
		KickAccount(info->accountId, KICK_USER_MULTI_LOGIN);
		ERR("login server check ok,but game world mulit login find!!!\n");
		return false;
	}

	lc = RT_NEW GWLoginClient;
	lc->netLink = ulc->netLink;
	lc->info = *info;
	lc->valid = true;
	lc->state = LCS_SELECT_CHAR;
	//PZH
	lc->SetPower(0);
	//
	m_loginClient[info->accountId] = lc;
    
	ulc->valid = false;
	ulc->netLink = NULL;

	return true;
}

void GWServer::UserQuit(GWLoginClient *lc)
{
	if(lc->GetState() == LCS_QUITING)
		return;
	lc->SetState(LCS_QUITING);

	if(lc->actorData)
	{
		OnUserLeaveWorld(lc);
		
		if(!m_database.SaveUserData(lc->actorData, false, lc->info.accountId,OnFinalSaveFinish))
		{
			ERR("UserQuit save data error!");
			OnRealQuit(lc);
		}
	}
	else
	{
		OnRealQuit(lc);
	}
}

void GWServer::UserSelectActor(GWLoginClient *lc)
{

}

void GWServer::TestServer()
{
    m_lastTestServerTime = time(NULL);
    m_region.BeginCmd(g2r_tester);
    for (long rgID = 0; rgID < (long)m_regionTable.size(); rgID++)
    {
        m_regionTable[rgID].m_cTestState = 1;
    }
    m_region.BroadcastCmd();
}

void GWServer::SendTestResult()
{
    char cResult = 1;
    std::string resultStr;
    for (long rgID = 0; rgID < (long)m_regionTable.size(); rgID++)
    {
        if (m_regionTable[rgID].m_cTestState!=2)
        {
            rt2_sprintf(g_strStaticBuffer, "    Region %d timeout\n", rgID);
            resultStr += g_strStaticBuffer;
            cResult = 0;
        }
    }
    CG_CmdPacket& cmd = m_login.BeginCmd(gws2lgs_tester_ret);
    cmd.WriteByte(cResult);
    if (cResult==0)
    {
        cmd.WriteString((char*)(const char*)resultStr.c_str());
    }
    m_login.SendCmd();
}

bool GWServer::ForbidUser(DWORD userID, ECreatureState forbidType, DWORD forbidTime)
{
	GWUser* pUser = g_server.FindActorById(userID);
	if (!pUser)
		return false;

	// 修改玩家状态
	DWORD dwState = pUser->dwState;
    switch (forbidType)
	{
	case CreState_CanSpeak:
		dwState &= ~CreState_CanSpeak;
		break;
	case CreState_CanPShop:
		dwState &= ~CreState_CanPShop;
		break;
	case CreState_CanMove:
		dwState &= ~CreState_CanMove;
		break;
	default:
		return false;
	}
	SetUserState(userID, dwState);

	DWORD now = rtGetMilliseconds();

	// 查找有没有已经存在的此类型的惩罚
	SUserForbid *pTmp = FindUserForbidden(userID, forbidType);
	if (pTmp)
	{
		pTmp->ForbidTime = forbidTime;
		pTmp->EndTime = now + forbidTime;
	}
	else 
	{
		// 添加到处理列表中
		SUserForbid forbid;
		forbid.userID = userID;
		forbid.Type = forbidType;
		forbid.ForbidTime = forbidTime;
		forbid.EndTime = now + forbidTime;
		m_arrUserForidden.push_back(forbid);
	}

	return true;
}

bool GWServer::UnForbidUser(DWORD userID, ECreatureState forbidType)
{
	SUserForbid *pTmp = FindUserForbidden(userID, forbidType);
	if (pTmp)
	{
		RemoveUserForbidden(userID, forbidType);
	}
	return true;
}

void GWServer::SetUserState(DWORD userID, DWORD state)
{
	GWUser* pUser = g_server.FindActorById(userID);
	if (!pUser)			return;

	pUser->dwState = state;

	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_user_state);
	cmd.WriteLong(userID);
	cmd.WriteLong(state);
	g_server.m_region.SendToPeer(rgIdx);
}

GWServer::SUserForbid* GWServer::FindUserForbidden(DWORD userID, ECreatureState forbidType)
{
	std::list<SUserForbid>::iterator iter = m_arrUserForidden.begin();
	for (; iter!=m_arrUserForidden.end(); iter++)
	{
		SUserForbid tmp = *iter;
		if (tmp.userID==userID && tmp.Type==forbidType)
		{
			return &(*iter);
		}
	}

	return NULL;
}

void GWServer::RemoveUserForbidden(DWORD userID, ECreatureState forbidType)
{
	// 设置玩家状态
	GWUser* pUser = g_server.FindActorById(userID);
	if (!pUser)			return;
	DWORD dwNewState = pUser->dwState;
	switch (forbidType)
	{
	case CreState_CanSpeak:
		dwNewState |= CreState_CanSpeak;
		break;
	case CreState_CanPShop:
		dwNewState |= CreState_CanPShop;
		break;
	case CreState_CanMove:
		dwNewState |= CreState_CanMove;
		break;
	default:
		break;
	}
	SetUserState(userID, dwNewState);

	// 从列表中删除
	std::list<SUserForbid>::iterator iter = m_arrUserForidden.begin();
	for (; iter!=m_arrUserForidden.end(); iter++)
	{
		SUserForbid tmp = *iter;
		if (tmp.userID==userID && tmp.Type==forbidType)
		{
			m_arrUserForidden.erase(iter);
			return;
		}
	}
}

void GWServer::ProcessUserForbidden()
{
#define USER_FORBIDDEN_PROCESS_INTERVAL		60000

	DWORD now = rtGetMilliseconds();

	static DWORD dwLastProcessTick = 0;
	if (now - dwLastProcessTick < USER_FORBIDDEN_PROCESS_INTERVAL)
		return;

	dwLastProcessTick = now;

	if (m_arrUserForidden.empty())	return;

	std::vector<SUserForbid> arrRemoveList;

	std::list<SUserForbid>::iterator iter = m_arrUserForidden.begin();
	for (; iter!=m_arrUserForidden.end(); iter++)
	{
		SUserForbid tmp = *iter;
		if (now >= tmp.EndTime)
		{
			arrRemoveList.push_back(tmp);
		}
	}

	for (int i=0; i<(int)arrRemoveList.size(); i++)
	{
		RemoveUserForbidden(arrRemoveList[i].userID, arrRemoveList[i].Type);
	}
}

bool GWServer::IsValidScene(const char *scene)
{
	GWSceneTable::iterator it;
	for(it=m_sceneTable.begin(); it!=m_sceneTable.end(); it++)
	{
		if(it->first == scene)
			return true;
	}
	return false;
}

void GWServer::GetDefaultPosByFaction(int faction,string &scene,long &x,long &y)
{
	if(faction == FACTION_SHANG)
	{
		scene = "scene01_032.rtb";
		x = 2455;
		y = -860;
	}
	else if(faction == FACTION_ZHOU)
	{
		scene = "scene01_060.rtb";
		x = 94;
		y = 117;
	}
	else
	{
		scene = "scene01_060.rtb";
		x = 94;
		y = 117;
	}
}

bool GWServer::LogServerSender_Init()
{
	if ( !m_LogServerSession.Create(NULL, 0) )
	{
		return false;
	}

	return true;
}

bool GWServer::LogServerSender_Close()
{
	m_LogServerSession.Close();
	return true;
}

bool GWServer::LogServerSender_Send(const char* title, const char* content)
{
	if (g_cfgGW.connectLogServer.ip.empty())
		return false;

	// LMT_STRING = 2
	long type = 2;
	string username = "";
	string ip = "";
	char macAddress[16];
	memset(macAddress, 0, 16);

	CG_CmdPacket tmpDataPacket;
	tmpDataPacket.WriteString((char*)title);
	tmpDataPacket.WriteString((char*)content);

	CG_CmdPacket cmd;
	cmd.BeginWrite();
	cmd.WriteLong( type );								// Msy Type
	cmd.WriteLong( tmpDataPacket.GetByteDataSize() );	// DataSize(不包括以下3个变量)
	cmd.WriteString( (char*)username.c_str() );				// 用户名
	cmd.WriteString( (char*)ip.c_str() );						// 用户IP
	cmd.WriteBinary( macAddress, 6);					// 用户Mac地址
	cmd.WriteBinary(tmpDataPacket.GetByteData(), tmpDataPacket.GetByteDataSize() );

	CG_NetAddress toAddress;
	toAddress.SetRemote(g_cfgGW.connectLogServer.ip.c_str());
	toAddress.SetPort(g_cfgGW.connectLogServer.port);
	if ( !toAddress.BuildAddr() )
	{
		ERR("[GWServer::LogServerSender_Send] CG_NetAddress Build Address Error\n");
		return false;
	}
	if ( !m_LogServerSession.SendTo(&cmd, &toAddress) )
	{
		ERR("[GWServer::LogServerSender_Send] SendTo error\n");
		return false;
	}
	return true;
}

void GWServer::SendMessageToAccount(DWORD dwAccountID, char cMsgType, const char* szMessage)
{
    if (szMessage==NULL)
    {
        return;
    }

    GWLoginClient* pLoginClient = FindLoginClientByAccountId(dwAccountID);
    if (pLoginClient==NULL)
    {
        return;
    }
	if(!pLoginClient->actorData || pLoginClient->GetState() == LCS_QUITING)
	{
		return;
	}
	if (pLoginClient->actorData==NULL)
    {
        return;
    }
    SendSystemMessageToUser(pLoginClient->actorData, szMessage);
}

void GWServer::SendSystemMessageToUser(DWORD dwUserDBID, const char* szMessage, const char* szTitle)
{
    GWUser* pUser = g_server.FindActorById(dwUserDBID);
    if (pUser)
    {
        g_server.SendSystemMessageToUser(pUser, szMessage, szTitle);
    }
}

void GWServer::SendSystemMessageToUser(GWUser* pUser, const char* szMessage, const char* szTitle)
{
    int se = g_server.GetRegionSession(pUser->regionID);
    if (se == -1)
    {
        return;
    }
    m_chatTransmittal.Chat(se, pUser->userID, szTitle?szTitle:"", CHAT_CHANNEL_SYSTEM, szMessage);
}

void GWServer::BroadcastBulletin(const char* szMessage, char bSysMsg, DWORD dwColor, DWORD dwGMID)
{
    for (long rgID = 0; rgID < (long)m_regionTable.size(); rgID++)
    {
        GWRegion& rg = g_server.m_regionTable[rgID];
        if (rg.sessionIndex == -1)
            continue;

        CG_CmdPacket& cmd = m_region.GetSendCmd();
        cmd.BeginWrite();
        cmd.WriteShort(g2r_gm_bulletin);
        cmd.WriteLong(dwGMID);
        cmd.WriteString((char*)szMessage);
        cmd.WriteLong(dwColor);
        cmd.WriteByte(bSysMsg);
        m_region.SendToPeer(rg.sessionIndex);
    }
}

void GWServer::ChargeCard(long dwUserParam, long dwUserID,
                long lSalesmanAccountID, const char* szSalesmanAccountName,
                char cCardType, const char* szCardID, const char* szCardPwd,
                const char* szUsername, short sLevel, char cMetier, char cMetierLev)
{
	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return;

    CG_CmdPacket& packet = m_login.BeginCmd(gws2lgs_charge_card);
    packet.WriteLong(pUser->accountId);
    packet.WriteString((char*)pUser->accountName.c_str());
    packet.WriteLong(lSalesmanAccountID);
    if (szSalesmanAccountName==NULL)
    {
        packet.WriteString((char*)"");
    }else
    {
        packet.WriteString((char*)szSalesmanAccountName);
    }
    packet.WriteLong(dwUserID);
    if (szUsername)
    {
        packet.WriteString((char*)szUsername);
        packet.WriteShort(sLevel);
        packet.WriteByte(cMetier);
        packet.WriteByte(cMetierLev);
    }else
    {
        packet.WriteString((char*)pUser->name.c_str());
        packet.WriteShort(pUser->attr.level);
        packet.WriteByte(pUser->attr.metier);
        packet.WriteByte(pUser->attr.metierLevel);
    }
    packet.WriteByte(cCardType);
    packet.WriteString((char*)szCardID);
    packet.WriteString((char*)szCardPwd);
    packet.WriteLong(dwUserParam);
    m_login.SendCmd();
}

void GWServer::OnChargeCardResult(long dwUserParam, long lUserID, const char* szCardID, char cSucceed, const char* szMsg)
{
    if (dwUserParam) // 只有Gameworld的 点卡寄售 才会有这个参数，其他的都是0
    {
        m_cardTrade.OnChargeResult(dwUserParam, lUserID, szCardID, cSucceed, szMsg);
    }else
    {
        const char* szSucceed;
        if (cSucceed)
        {
			szSucceed = R(MSG_CARD_TRADE_SUCCEED);
        }else
        {
			szSucceed = R(MSG_CARD_TRADE_FAIL);
        }
		rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_TRADE_ACTIVE), szSucceed);
		rt2_sprintf(g_strStaticBuffer+100, R(MSG_CARD_TRADE_ACTIVE_NUMBER), szCardID, szMsg);
        SendSystemMessageToUser(lUserID, g_strStaticBuffer+100);
		string StrItemID = "";
	    m_mail.AddMail(lUserID, 0, g_strStaticBuffer, g_strStaticBuffer+100, true,StrItemID,0);
    }
}

void GWServer::CheckCardCanTrade(long lSalesmanAccountID, const char* szSalesmanAccountName,
                                 long lUserID, char cCardType, const char* szCardID,
                                 const char* szCardPwd, long lUserParam, short sTimeOut)
{
    CG_CmdPacket& packet = m_login.BeginCmd(gws2lgs_card_can_trade);
    packet.WriteLong(lSalesmanAccountID);
    packet.WriteString((char*)szSalesmanAccountName);
    packet.WriteLong(lUserID);
    packet.WriteByte(cCardType);
    packet.WriteString((char*)szCardID);
    packet.WriteString((char*)szCardPwd);
    packet.WriteLong(lUserParam);
    packet.WriteShort(sTimeOut);
    m_login.SendCmd();
}

void GWServer::OnCheckCardCanTradeResult(long lUserID, const char* szCardID, char cCan, const char* szMsg, long lCardGroupType, long lUserParam)
{
    m_cardTrade.OnCheckCardCanTrade(lUserParam, lUserID, szCardID, lCardGroupType, cCan, szMsg);
}

bool GWServer::AddMoneyToGift(long dwUserID, int iMoney, const char* szInfo)
{
    char szMoney[50];
    rt2_snprintf(szMoney, 49, "%d", iMoney);
    szMoney[49] = 0;
    return g_GiftMgr.AddGift(dwUserID, 0, GT_Money, szMoney, (DWORD)time(NULL), 0);
}

bool GWServer::AddItemToGift(long dwUserID, SItemID& itemID, const char* szInfo)
{
	//SCreateItem ci;
	//ci.index = g_ItemMgr.GetIndexFromType(itemID.type);
	//ci.level = itemID.
	//g_GiftMgr.AddGift(dwUserID, 0, , , )
	return true;
}

int	GWServer::GetUserOnlineTime(DWORD userID)
{
	long onlineTime = 0;
	GWUser *pUser = g_server.FindActorById(userID);
	if (pUser)
	{
		onlineTime = pUser->onlineTime;
		long stayTime = time(NULL) - pUser->enterTime;
		if (stayTime > 0)
			onlineTime += stayTime;
	}
	else 
	{
		GWDBCommand cmd;
		MYSQL_RES* res;
		rt2_sprintf(cmd.command, "SELECT `OnlineTime` FROM `user` WHERE `ID` = %d", userID);
		if (!g_server.m_database.ExecCommand(cmd.command, &res))
			return 0;

		int count = mysql_num_rows(res);
		if (count == 0)
			return 0;

		MYSQL_ROW row = mysql_fetch_row(res);
		onlineTime = atol(row[0]);

		mysql_free_result(res);
	}

	if (onlineTime < 0)
		onlineTime = 0;

	return onlineTime;
}

void GWServer::AddPyGMOP(long cmdid, std::string gmcommand, std::string toUser, long GMAccountid)
{
	PyGmOP mp;
	mp.cmdIDinDB  = cmdid;
	mp.gm_account = GMAccountid;
	mp.gmcmd	  = gmcommand;
	mp.toUserName = toUser;
	m_pygmop.push_back(mp);
}

void GWServer::GetGMOP()
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT `cmdid`,`cmd` ,`gm_account`,`toUserName`  FROM `py_game_op` WHERE `result` = %d",0);
	if (!g_server.m_database.ExecCommand(cmd.command,&res))
		return ;
	int count = mysql_num_rows(res);
	m_pygmop.clear();
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		AddPyGMOP(atol(row[0]),row[1],row[3],atol(row[2]));
	}
	mysql_free_result(res);
}

void GWServer::DisPatchPyGMOP()
{
	std::list<PyGmOP>::iterator it = m_pygmop.begin();
	for(; it != m_pygmop.end();)
	{
		GWUser* pUser = FindActorByName((*it).toUserName.c_str());
		if (!pUser)
			return ;
		int se = GetRegionSession(pUser->regionID);
		if (se == -1)
			return ;

		CG_CmdPacket& cmd = m_region.BeginCmd(g2r_execute_gmcmd); 
		cmd.WriteLong(pUser->userID);
		cmd.WriteLong((*it).gm_account);
		cmd.WriteString((char*)(*it).gmcmd.c_str());
		//cmd.WriteString((char*)(*it).toUserName.c_str());
		m_region.SendToPeer(se);

		GWDBCommand dbcmd;
		rt2_sprintf(dbcmd.command, "update `py_game_op` set `result`=%d  WHERE `cmdid` = %d",1,(*it).cmdIDinDB);
		it = m_pygmop.erase(it);
		if (!g_server.m_database.ExecCommand(dbcmd.command,NULL))
			return ;
	}
}

void GWServer::UpdateUserIsOnLine(int bOnLine,long lUserID/* =-1 */)
{
	GWDBCommand usercmd;
	if (lUserID == -1)
		rt2_sprintf(usercmd.command, "update `user` set `IsOnline` = %d ", bOnLine);
	else
		rt2_sprintf(usercmd.command, "update `user` set `IsOnline` = %d where `ID` = %d", bOnLine, lUserID);
	if (!g_server.m_database.ExecCommand(usercmd.command, NULL))
	{
		LOG("set user isOnLine error\n");
		return ;
	}

}

//tim.yang  VIP卡
void GWServer::AddAccountTime(long lUserID,long time)
{
	GWUser *pUser = FindActorById(lUserID);
	if (!pUser)
	{
		AddAccountTimeRet(lUserID,0);
		return;
	}
	CG_CmdPacket& packet = m_login.BeginCmd(gws2lgs_add_account_time);
	packet.WriteLong(pUser->accountId);
	packet.WriteLong(time);
	packet.WriteLong(lUserID);
	m_login.SendCmd();
}

void GWServer::AddAccountTimeRet(long lUserID,char isRight,std::string time)
{
	GWUser* pUser = FindActorById(lUserID);
	if (pUser)
	{
		int se = GetRegionSession(pUser->regionID);
		CG_CmdPacket& cmd = m_region.BeginCmd(g2r_add_account_time_ret);
		cmd.WriteLong(lUserID);
		cmd.WriteByte(isRight);
		cmd.WriteString((char*)time.c_str());
		g_server.m_region.SendToPeer(se);
	}
}
//end
