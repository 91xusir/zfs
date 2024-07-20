#include "region.h"
#include "gw_protocol.h"
//#include "rt_system_info.h"
#include "master_prentice_base.h"
#include "region_prentice.h"
#ifdef LINUX
#include "linux_util.h"
#endif //LINUX

#include "rs_national_war.h"

#include "RegionActManager.h"
//#define LICENSE_CHECK

#define SAFE_READ(p) if(!(p)) return CMD_ERROR_READ_DATA;
#define SAFE_READ2(fun, val) if (!cmd->fun(&val)) { ERR("Read '"#val"' from network error\n"); return CMD_ERROR_READ_DATA; }

CMD_FUNCTION(cmd_g2r_warfield_msg)
{
	long dwUserID, iMsgID;

	cmd->ReadLong(&dwUserID);
	cmd->ReadLong(&iMsgID);

	g_WarfieldMgr.TransmitMsg(dwUserID, iMsgID);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_update_rate)
{
	float fRZhou, fRShang;

	cmd->ReadFloat(&fRZhou);
	cmd->ReadFloat(&fRShang);

	g_WarfieldMgr.UpdateRate(fRZhou, fRShang);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_start)
{
	g_WarfieldMgr.CreateWarfield();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_stop)
{
	g_WarfieldMgr.Stop();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_update_data)
{
	long lRemain;
	long numZhou;
	long numShang;
	float lScoreZhou;
	float lScoreShang;

	float lPriHero[3];
	float lInterHero[3];
	float lSeniorHero[3];

	long lUnionNum;
	long lExploitNum;
	std::list<SExploit> listExploit;
	std::list<DWORD>	listUserZhou;
	std::list<DWORD>	listUserShang;
	std::list<SPerExploit> listPerExploit;

	cmd->ReadLong(&lRemain);
	cmd->ReadLong(&numZhou);
	for(int i=0; i < numZhou; i++)
	{
		long tmp;
		cmd->ReadLong(&tmp);
		listUserZhou.push_back(tmp);
	}
	cmd->ReadFloat(&lScoreZhou);
	cmd->ReadLong(&numShang);
	for(int i=0; i < numShang; i++)
	{
		long tmp;
		cmd->ReadLong(&tmp);
		listUserShang.push_back(tmp);
	}
	cmd->ReadFloat(&lScoreShang);

	cmd->ReadFloat(&lPriHero[0]);
	cmd->ReadFloat(&lPriHero[1]);
	cmd->ReadFloat(&lPriHero[2]);
	cmd->ReadFloat(&lInterHero[0]);
	cmd->ReadFloat(&lInterHero[1]);
	cmd->ReadFloat(&lInterHero[2]);
	cmd->ReadFloat(&lSeniorHero[0]);
	cmd->ReadFloat(&lSeniorHero[1]);
	cmd->ReadFloat(&lSeniorHero[2]);

	cmd->ReadLong(&lUnionNum);
	for(int i = 0; i < lUnionNum; i++)
	{
		SExploit tmp;
		tmp.Serialize(cmd);
		listExploit.push_back(tmp);
	}

	cmd->ReadLong(&lExploitNum);
	for(int i = 0; i < lExploitNum; i++)
	{
		SPerExploit tmp;
		cmd->ReadLong((long*)&(tmp.dwUserID));
		cmd->ReadLong((long*)&(tmp.dwExploit));

		listPerExploit.push_back(tmp);
	}

	g_WarfieldMgr.OnRecvUpdateData(lRemain, numZhou, &listUserZhou, lScoreZhou, numShang, &listUserShang, lScoreShang, 
								lPriHero, lInterHero, lSeniorHero, &listExploit, &listPerExploit);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_addscore_rep)
{
	long lScoreZhou = 0;
	long lScoreShang = 0;

	cmd->ReadLong(&lScoreZhou);
	cmd->ReadLong(&lScoreShang);

	if(cmd->IsError())
		return CMD_ERROR_READ_DATA;

	//

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_flag_destroyed_rep)
{
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_warfield_leave_rep)
{
	long dwUserID;

	cmd->ReadLong(&dwUserID);

	CRegionUser* user = g_region->FindUser(dwUserID);
	g_WarfieldMgr.OnNWarUserLeave(user);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_create_dungeon)
{
	long uId;
	long fbId;
	long fbDif;
	long fbUserLV;
	long lExitPos[2];

	cmd->ReadLong(&uId);
	cmd->ReadLong(&fbId);
	cmd->ReadLong(&fbDif);
	cmd->ReadLong(&fbUserLV);
	cmd->ReadLong(&lExitPos[0]);
	cmd->ReadLong(&lExitPos[1]);

	if(cmd->IsError())
		return CMD_ERROR_READ_DATA;

	g_region->m_fbMgr->CreateDungeon(fbId,uId,fbDif,fbUserLV, lExitPos[0], lExitPos[1]);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_enter_dungeon_ret)
{
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_leave_dungeon_ret)
{
	long uId;
	cmd->ReadLong(&uId);

	if(cmd->IsError())
		return CMD_ERROR_READ_DATA;

	CRegionUser* user = g_region->FindUser(uId);
    g_region->m_fbMgr->OnDungeonUserLeave(user);
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_destroy_dungeon_ret)
{
    long uId, iExitSecond;
    cmd->ReadLong(&uId);
    if(cmd->IsError())
    {
        return CMD_ERROR_READ_DATA;
    }
    cmd->ReadLong(&iExitSecond);
    if(cmd->IsError())
    {
        return CMD_ERROR_READ_DATA;
    }
    g_region->m_fbMgr->DestroyDungeon(uId, (int)iExitSecond);
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_broadcast_to_region)
{
    char* szCmd;
    char seps[]   = " \t";
    cmd->ReadString(&szCmd);

    //static char szCmdCopy[512];
	S_MEMDEF(szCmdCopy, 512)
	S_MEMPROTECTOR(szCmdCopy, 512, bMP)

    rt2_strncpy(szCmdCopy, szCmd, 511);// szCmdCopy[511] = 0;
    char* token = strtok( (char*)szCmdCopy, seps );

	if(stricmp(token, "set")==0)
	{
		token = strtok( NULL, seps );
		if(token)
		{
            int iCmdId;
			if(stricmp(token, "exp_rate")==0)
			{
				iCmdId = 1;
			}else if(stricmp(token, "gold_rate")==0)
			{
				iCmdId = 2;
			}else if(stricmp(token, "next_enter")==0)
			{
				iCmdId = 3;
			}else
            {
                iCmdId = 0;
            }

			token = strtok( NULL, seps );
			if(iCmdId!=0 && token)
            {
                float fValue = (float)atof(token);
                switch (iCmdId)
                {
                case 1:
                    g_cfg.game.exp_rate = fValue;
                    break;
                case 2:
                    g_cfg.game.gold_rate = fValue;
                    break;
                case 3:
					CRsDungeonMgr::s_iMaxLastQuitTime = (int)fValue;
                    break;
                }
            }
        }
	}
	else if (stricmp(token, "processcheck")==0)
	{
		token = strtok( NULL, seps );
		if (token!=NULL)
		{
            g_sendCmd->BeginWrite();
	        g_sendCmd->WriteShort(r2c_event);
	        g_sendCmd->WriteLong(g_region->m_time);
	        g_sendCmd->WriteShort(EVENT_CHECK_PROCESS);
	        g_sendCmd->WriteLong(1);
	        g_sendCmd->WriteString(token);

            long lNowTime = (long)time(NULL)+100; // 100秒超时
	        CRegionUser *user;
	        EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator it;
	        for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	        {
		        user = (*it).second;
                if (user && user->m_dummy)
                {
                    user->m_dummy->m_lLastCheckProcessTime = lNowTime;
		            user->RecvCmd(g_sendCmd);
                }
	        }
		}
	}
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_query_poing_ret)
{
    char cReceivePointInfo, cVIP;
    long lUserID, lPoint, tExpire;
    char *szMsg;

    SAFE_READ2(ReadLong, lUserID)
    SAFE_READ2(ReadByte, cReceivePointInfo)
    SAFE_READ2(ReadLong, lPoint)
    SAFE_READ2(ReadLong, tExpire)
    SAFE_READ2(ReadString, szMsg)
    SAFE_READ2(ReadByte, cVIP)

	CRegionUser* pUser = g_region->FindUser(lUserID);
	if (pUser!=NULL && pUser->m_dummy!=NULL)
	{
        pUser->m_lPoint = lPoint;
        pUser->m_tPointExpire = tExpire;
        pUser->m_cVIP = cVIP;
        rt2_strncpy(pUser->m_szPointMsg, szMsg, 99); //pUser->m_szPointMsg[99] = 0;

        if (cReceivePointInfo==2)
        {
            if (cReceivePointInfo)
            {
                if (tExpire!=0)
                {
                    struct tm* pTm = localtime((time_t*)&tExpire);
                    rt2_sprintf(g_strStaticBuffer, "%s 过期时间: %d/%d/%d %d:%d",
                        szMsg, pTm->tm_year+1970, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min);
                    /*
                    int iD, iH, iM;
                    iD = tExpire/(24*60*60);
                    iM = tExpire - iD*(24*60*60);
                    iH = iM/(60*60);
                    iM = (iM-iH*(60*60))/60;
                    rt2_sprintf(g_strStaticBuffer, "%s 剩余时间: %d天 %d:%d", szMsg, iD, iH, iM);
                    */
                }else
                {
					rt2_sprintf(g_strStaticBuffer, R(MSG_POINT_SPARE_POINT), szMsg, lPoint);
                }
                if (cVIP)
                {
                    rt2_strcat(g_strStaticBuffer, "(VIP)");
                }
            }else
            {
				rt2_sprintf(g_strStaticBuffer, R(MSG_QUERY_ACCOUNT_POINT_ERR));
            }
            g_region->ShowHtmlDialog(NULL, pUser->m_dummy, R(QUERY_POINT_HTML), "POINT", g_strStaticBuffer);
        }
	}
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_update_poing_ret)
{
    char cType, cVIP;
    long lUserID, lPoint;

    SAFE_READ2(ReadLong, lUserID)
    SAFE_READ2(ReadByte, cType)
    SAFE_READ2(ReadLong, lPoint)
    SAFE_READ2(ReadByte, cVIP)

	CRegionUser* pUser = g_region->FindUser(lUserID);
	if (pUser!=NULL && pUser->m_dummy!=NULL)
	{
        if (cType==1)
        {
            pUser->m_lPoint = lPoint;
        }else
        {
            pUser->m_tPointExpire = lPoint;
        }
        if (pUser->m_cVIP!=cVIP)
        {
            // VIP信息发生变化，这里可以视需要通知客户端VIP的变化
        }
        pUser->m_cVIP = cVIP;
	}
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_prededuct_ret) // 预扣点数的返回值, 用来做用点数向NPC买道具
{
    long lUserID;
    long lDeductOrderID;
    char cSucceed;
    short sPoint;
	short sCurPoint;
	char* cResult;
	long event;
	long lAuctionId;

    SAFE_READ2(ReadLong, lUserID)
    SAFE_READ2(ReadLong, lDeductOrderID)
    SAFE_READ2(ReadByte, cSucceed) // 1 成功, 0 失败,完全不能扣点, 2 失败,可以扣除部分点数,数量保存在sPoint中
    SAFE_READ2(ReadShort, sPoint)
	SAFE_READ2(ReadShort, sCurPoint)
	SAFE_READ2(ReadString, cResult)
	SAFE_READ2(ReadLong, event)
	SAFE_READ2(ReadLong, lAuctionId)


	CRegionUser* pUser = g_region->FindUser(lUserID);
	if (pUser!=NULL && pUser->m_dummy!=NULL)
	{
        // 调用对应函数
		if(cSucceed)
		{
			if (event == 1)//竞拍
			{
				CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
				cmd.WriteShort(r2g_auction_bid);
				cmd.WriteLong(pUser->m_userId);
				cmd.WriteLong(sPoint);
				cmd.WriteLong(lAuctionId);
				g_region->m_gws.EndSend();

				CG_CmdPacket& packet = g_region->m_gws.BeginSend();
				packet.WriteShort(r2g_log_event);
				packet.WriteByte(PLT_PointCast);
				packet.WriteLong(pUser->m_userId);
				packet.WriteString((char*)pUser->m_userIpAddress.c_str()); // userIP
				packet.WriteLong((long)sPoint);
				packet.WriteString("竞拍物品扣除");
				g_region->m_gws.EndSend();
			}
		}
		pUser->m_dummy->SendSystemMessage(cResult);
	}
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_execute_gmcmd)
{
	long toUserID;
	long GMAccountID;
	char* gmCmd;

	SAFE_READ2(ReadLong, toUserID)
	SAFE_READ2(ReadLong, GMAccountID)
	SAFE_READ2(ReadString, gmCmd) // 1 成功, 0 失败,没有扣任何点, 2 失败,扣除部分点数,数量保存在sPoint中
	CRegionUser* pUser = g_region->FindUser(toUserID);
	if (pUser!=NULL && pUser->m_dummy!=NULL)
	{
		DoClientCommand(pUser->m_dummy,gmCmd,true,GMAccountID);
	}
	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_add_point_ret)
{
	long lUserID;
	char cSucceed;
	char* cResult;
	short sPoint;

	SAFE_READ2(ReadLong, lUserID)
	SAFE_READ2(ReadByte, cSucceed)
	SAFE_READ2(ReadString, cResult) // 1 成功, 0 失败,没有扣任何点, 2 失败,扣除部分点数,数量保存在sPoint中
	SAFE_READ2(ReadShort, sPoint)
// 	if(!CDeduct::GetDeductInstance()->UseDeductData(lUserID,lDeductOrderID,cResult,sPoint,sCurPoint))
// 		ERR("use deduct data err!!!!!!");
	CRegionUser* pUser = g_region->FindUser(lUserID);
	if (pUser!=NULL && pUser->m_dummy!=NULL)
	{
		if (cSucceed)
		{
			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_log_event);
			packet.WriteByte(PLT_PointGet);
			packet.WriteLong(pUser->m_userId);
			packet.WriteString((char*)pUser->m_userIpAddress.c_str()); // userIP
			packet.WriteLong((long)sPoint);
			packet.WriteString("竞拍获得");
			g_region->m_gws.EndSend();
			pUser->m_dummy->SetVip(1);
			CItemCmdBuilder_Svr::Build_r2c_point_result(sPoint);	//heten
			pUser->m_dummy->RecvCmd(g_sendCmd);	//heten
		}
		pUser->m_dummy->SendSystemMessage(cResult);
	}
	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_deduct_ret) // 扣点数的返回值, 用来做用点数向NPC买道具
{
    long lUserID;
    long lDeductOrderID;
    char* cResult;
    short sPoint;
	short sCurPoint;

    SAFE_READ2(ReadLong, lUserID)
    SAFE_READ2(ReadLong, lDeductOrderID)
    SAFE_READ2(ReadString, cResult) // 1 成功, 0 失败,没有扣任何点, 2 失败,扣除部分点数,数量保存在sPoint中
    SAFE_READ2(ReadShort, sPoint)
	SAFE_READ2(ReadShort, sCurPoint)
	if(!CDeduct::GetDeductInstance()->UseDeductData(lUserID,lDeductOrderID,cResult,sPoint,sCurPoint))
		ERR("use deduct data err!!!!!!");
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_license_check_ret)
{
	char bret;

	SAFE_READ2(ReadByte, bret)

	if (bret!=0)
	{
		LOG("License Check OK\n");
	}else
	{
		LOG("License Check Failed\n");
        g_region->Shutdown();
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_card_consign_ret)
{
    long lUserID, lConsignationID=0, lPrice=0;
    char cSucceed;
    char *szCardID=0, *szMsg=0;
    SAFE_READ2(ReadLong, lUserID)
    SAFE_READ2(ReadByte, cSucceed) // 1 成功, 0 失败
    if (cSucceed==0)
    {
        SAFE_READ2(ReadString, szCardID)
        SAFE_READ2(ReadString, szMsg)
    }else
    {
        SAFE_READ2(ReadLong, lConsignationID)
        SAFE_READ2(ReadLong, lPrice)
    }
    g_region->m_rsCardTrade.OnConsignResult(lUserID, cSucceed, lConsignationID, lPrice, szCardID, szMsg);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_card_buy_ret)
{
    long lUserID, lConsignationID, lPrice;
    char cSucceed;
    char* szMsg;
    SAFE_READ2(ReadLong, lUserID)
    SAFE_READ2(ReadByte, cSucceed) // 1 成功, 0 失败
    SAFE_READ2(ReadLong, lConsignationID)
    SAFE_READ2(ReadLong, lPrice)
    SAFE_READ2(ReadString, szMsg)
    g_region->m_rsCardTrade.OnBuyCardResult(lUserID, cSucceed, lConsignationID, lPrice, szMsg);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_add_account_time_ret)
{
	long lUserID;
	char cSucceed;
	char* pAccountTime;
	SAFE_READ2(ReadLong, lUserID)
	SAFE_READ2(ReadByte, cSucceed) // 1 成功, 0 失败
	SAFE_READ2(ReadString, pAccountTime)

	CRegionUser *pUser = g_region->FindUser(lUserID);
	if (pUser==NULL || pUser->m_dummy==NULL)
	{
		return 0;
	}
	pUser->m_dummy->OnAddAccountTime(lUserID,pAccountTime,cSucceed);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_card_cancel_ret)
{
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_card_update)
{
    g_region->m_rsCardTrade.OnUpdateTradeList(cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_move_user)
{
	long userID;
	long x;
	long y;
    long iDungeonUID, iDungeonTID, iRegionID;
    char cBorn;
	char cfaction;

    SAFE_READ2(ReadLong, userID)
    SAFE_READ2(ReadLong, x)
    SAFE_READ2(ReadLong, y)
    SAFE_READ2(ReadLong, iDungeonUID)

	CRegionUser* user = g_region->FindUser(userID);
	if (user == NULL)
	{
        LOG1("cmd_g2r_move_user: Can not find user %d\n", userID);
		return CMD_ERROR_NONE;
	}

    if(iDungeonUID == 0xffff)//战场
	{
		SAFE_READ2(ReadLong, iRegionID)
		SAFE_READ2(ReadByte, cBorn)
		SAFE_READ2(ReadByte,cfaction)

		if(iRegionID==(long)g_cfgRs.lRegionID)
		{
			if(g_WarfieldMgr.m_pWarfield)
			{
				// 先收起宠物
				if(user->m_dummy->m_pet)
				{
					if(user->m_dummy->IsRidePet()) 
						user->m_dummy->RideOffPet();					
				}
				if(user->m_dummy->m_hero)
				{
					user->m_dummy->CallbackHero();
				}
				user->m_dummy->m_scene->RemoveCreature(user->m_dummy->m_oId);
				user->m_dummy->m_core.Faction = cfaction;
				if(!g_WarfieldMgr._AddUserToNWar(user, true, x, y))
				{
					ERR("cmd_g2r_move_user:move user to warfield error!!!\n");
					long defpos[3];
					defpos[0] = 27789;
					defpos[1] = -8100;
					if(!g_region->m_defScene->AddCreature(user->m_dummy,defpos,0))
					{
						defpos[0] = 26536;
						defpos[1] = -7755;
						g_region->m_defScene->AddCreature(user->m_dummy,defpos,0);
					}
					user->m_dummy->ForceSetPosition(defpos[0],defpos[1],0);
				}
			}else
			{
				LOG("Cannot find warfield\n");
			}
		}else
		{
            user->m_dummy->m_bIsInWarfield = true;
			user->m_dummy->m_pos[0] = x;
			user->m_dummy->m_pos[1] = y;

			user->m_regionId = iRegionID;
			g_region->m_gws.BeginSwitchRegion(user);
		}
	}else if (iDungeonUID != 0)
    {
        SAFE_READ2(ReadLong, iDungeonTID)
        SAFE_READ2(ReadLong, iRegionID)
        SAFE_READ2(ReadByte, cBorn)

        if (iRegionID==(long)g_cfgRs.lRegionID)
        {
            // 要查找副本是否在这个Region里面
            CRsDungeonMgr::SRsDungeon* pDungeon = g_region->m_fbMgr->FindDungeon(iDungeonUID);

            if (pDungeon)
            {
                // 副本在这个Region中
				if (user->m_dummy->m_scene)
					user->m_dummy->m_scene->RemoveCreature(user->m_dummy->m_oId);
                g_region->m_fbMgr->_AddUserToDungeon(iDungeonUID, user, cBorn, x, y);
            }else
            {
                // 副本没有找到
		        ERR("Dungeon not found.\n");
            }
        }else
        {
            // 如果在其他的Region上面，就切换到那个Region上
            user->m_dummy->m_dungeonUID = iDungeonUID;
            user->m_dummy->m_dungeonTID = iDungeonTID;
            user->m_dummy->m_pos[0] = x;
            user->m_dummy->m_pos[1] = y;

            user->m_regionId = iRegionID;
	        g_region->m_gws.BeginSwitchRegion(user);
        }

    }else
    {
	    int ix, iy;

	    CRegionCreature* cre = user->m_dummy;

	    if (!cre->m_scene->m_pPathFind->FindSpace(cre->GetObjectId(), (short)cre->m_type, x, y, ix, iy, true))
	    {
            LOG3("g2r_move_user: Can not find free space %d, %d at %s\n", x, y, cre->m_scene->m_sceneName.c_str());
		    return CMD_ERROR_NONE;
	    }

	    float fx, fy;
	    cre->m_scene->m_pTerrain->GetPosByTerrainXY(ix, iy, fx, fy);
		if (cre->m_bBroadcastTele)
		{
			cre->ForceSetPosition(fx, fy, 0,true);
		}
		else
		{
			cre->ForceSetPosition(fx, fy, 0);
		}
    }
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_move_useraround)
{
	long userID;
	long x;
	long y;
	short nRange;

    SAFE_READ2(ReadLong, userID)
    SAFE_READ2(ReadLong, x)
    SAFE_READ2(ReadLong, y)
    SAFE_READ2(ReadShort, nRange)

	CRegionUser* user = g_region->FindUser(userID);
	if (user == NULL)
	{
        LOG1("cmd_g2r_move_useraround: Can not find user %d\n", userID);
		return CMD_ERROR_NONE;
	}

	int ix, iy;

	CRegionCreature* cre = user->m_dummy;

	if (!cre->m_scene->m_pPathFind->FindSpace(cre->GetObjectId(), (short)cre->m_type, x, y, ix, iy, true))
	{
        LOG3("g2r_move_useraround: Can not find free space %d, %d at %s", x, y, cre->m_scene->m_sceneName.c_str());
		return CMD_ERROR_NONE;
	}

	float fx, fy;
	cre->m_scene->m_pTerrain->GetPosByTerrainXY(ix, iy, fx, fy);

	// Move Around User
	std::vector<CRegionCreature*> creList;
	int counter = 20;
	cre->m_scene->GetAroundCreatureByGrid(cre->m_pos, nRange, creList, cre);
	std::vector<CRegionCreature*>::iterator iter = creList.begin();
	for (; iter!=creList.end() && counter>=0; iter++, counter--)
	{
		CRegionCreature* pCre = (*iter);
		if (!pCre->IsUser() && !pCre->IsUserPet())
			continue;
		if (pCre == cre)
			continue;

		pCre->ForceSetPosition(fx, fy, 0);
	}

	// Move The User
	cre->ForceSetPosition(fx, fy, 0);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_pull_user)
{
	long moveTo;
	long moveFrom;

    SAFE_READ2(ReadLong, moveTo)
    SAFE_READ2(ReadLong, moveFrom)

	CRegionUser* ut = g_region->FindUser(moveTo);
	if (ut == NULL)
	{
		LOG1("Can not find moveTo user %d\n", moveTo);
		return CMD_ERROR_NONE;
	}

	int tmpX, tmpY;
	int x, y;
	if (!ut->m_dummy->m_scene->m_pTerrain->GetTerrainTile(ut->m_dummy->m_pos[0], ut->m_dummy->m_pos[1], tmpX, tmpY))
	{
		return CMD_ERROR_NONE;
	}
	if (!ut->m_dummy->m_scene->m_pPathFind->FindSpace(ut->m_dummy->GetObjectId(), (short)ut->m_dummy->m_type, tmpX, tmpY, x, y, true))
	{
        LOG("g2r_pull_user: Can not find free space\n");
		return CMD_ERROR_NONE;
	}

	CRegionUser* uf = g_region->FindUser(moveFrom);

	if (uf)
	{
		float fx, fy;
		ut->m_dummy->m_scene->m_pTerrain->GetPosByTerrainXY(x, y, fx, fy);

		uf->m_dummy->ForceSetPosition(fx, fy, 0);
	}
	else
	{
		CG_CmdPacket& c = g_region->m_gws.BeginSend();

		c.WriteShort(r2g_move_user);
		c.WriteLong(moveFrom);
		c.WriteLong(x);
		c.WriteLong(y);

		g_region->m_gws.EndSend();
	}	

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_pull_useraround)
{
	long moveTo;
	long moveFrom;
	short nRange;

    SAFE_READ2(ReadLong, moveTo)
    SAFE_READ2(ReadLong, moveFrom)
    SAFE_READ2(ReadShort, nRange)

	CRegionUser* ut = g_region->FindUser(moveTo);
	if (ut == NULL)
	{
		LOG1("Can not find moveTo user %d\n", moveTo);
		return CMD_ERROR_NONE;
	}

	int tmpX, tmpY;
	int x, y;
	if (!ut->m_dummy->m_scene->m_pTerrain->GetTerrainTile(ut->m_dummy->m_pos[0], ut->m_dummy->m_pos[1], tmpX, tmpY))
	{
		return CMD_ERROR_NONE;
	}
	if (!ut->m_dummy->m_scene->m_pPathFind->FindSpace(ut->m_dummy->GetObjectId(), (short)ut->m_dummy->m_type, tmpX, tmpY, x, y, true))
	{
        LOG("g2r_pull_useraround: Can not find free space\n");
		return CMD_ERROR_NONE;
	}

	CRegionUser* uf = g_region->FindUser(moveFrom);

	if (uf)
	{
		float fx, fy;
		ut->m_dummy->m_scene->m_pTerrain->GetPosByTerrainXY(x, y, fx, fy);

		// Move Around User
		std::vector<CRegionCreature*> creList;
		int counter = 20;
		uf->m_dummy->m_scene->GetAroundCreatureByGrid(uf->m_dummy->m_pos, nRange, creList, uf->m_dummy);
		ERR1("Pull useraround, around creature count=%d\n", creList.size());
		std::vector<CRegionCreature*>::iterator iter = creList.begin();
		for (; iter!=creList.end() && counter>=0; iter++, counter--)
		{
			CRegionCreature* pCre = (*iter);

			if (!pCre->IsUser() && !pCre->IsUserPet() && !pCre->IsCallNpc())
				continue;
			if (!pCre->m_userInfo || pCre->m_userInfo->m_userId==uf->m_userId || pCre->m_userInfo->m_userId==ut->m_userId)
				continue;

			pCre->ForceSetPosition(fx, fy, 0);
		}

		// Move The Useer
		uf->m_dummy->ForceSetPosition(fx, fy, 0);
	}
	else
	{
		CG_CmdPacket& c = g_region->m_gws.BeginSend();

		c.WriteShort(r2g_move_useraround);
		c.WriteLong(moveFrom);
		c.WriteLong(x);
		c.WriteLong(y);
		c.WriteShort(nRange);

		g_region->m_gws.EndSend();
	}	

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_shutdown)
{
	LOG("g2r_shutdown command from gws\n");
	g_region->Shutdown();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_kick_all_user)
{
	LOG("cmd_g2r_kick_all_user command from gws\n");
	long lDelayTime;
    SAFE_READ2(ReadLong, lDelayTime)
	g_region->KickAllUser(lDelayTime);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_reboot_region)
{
	LOG("g2r_reboot command from gws\n");

	for (EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator user = g_region->m_userMap.begin();
		user != g_region->m_userMap.end(); user++)
	{
		if (user->second->m_dummy)
        {
			if(user->second->m_dummy->m_pet)
				user->second->m_dummy->CallbackPet();
			if(user->second->m_dummy->m_hero)
				user->second->m_dummy->CallbackHero();
			if(user->second->m_dummy->m_WardNPC)
				user->second->m_dummy->RemoveWardNPC();
			user->second->m_dummy->m_Skill.ShutdownSkill();
            g_region->m_gws.SaveUserData(user->first, user->second->m_dummy);
        }

		g_region->m_gws.Logout(user->first, false);
	}

	g_region->Reboot();

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_safe_quited)
{
    g_region->m_gws.m_bSafeQuited = true;
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_check_version_ret)
{
    bool bContinue = false;
    char cSame;
    cmd->ReadByte(&cSame);

    if (cSame)
    {
        LOG("Check version SUCCEEDED.\n");
    }else
    {
        LOG("Check version FAILED.\n");
        if (bContinue)
        {
            LOG("    Continue ...\n");
        }
    }

    if (cSame || bContinue)
    {
        // 发送注册信息
        CG_CmdPacket& cmd = g_region->m_gws.BeginSend();
        cmd.WriteShort(r2g_register);
        cmd.WriteLong(g_region->m_gws.m_seed);
        cmd.WriteLong(g_cfgRs.lRegionID);
        cmd.WriteString((char*)g_cfgRs.listenClient.listenIP.c_str());
        cmd.WriteLong(g_cfgRs.listenClient.listenPort);
        g_region->m_gws.EndSend();
    }else
    {
        // 退出
        g_region->Shutdown();
    }
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_tester)
{
    CG_CmdPacket& sendCmd = g_region->m_gws.BeginSend();
    sendCmd.WriteShort(r2g_tester_ret);
    g_region->m_gws.EndSend();
    return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_init_region)
{
	LOG("g2r_init_region command from gws\n");

	long count;
	long seed;
	long gwtTicks;
	float gwtScaling;

	if (!cmd->ReadLong(&gwtTicks))
	{
		ERR("Read 'GWTTicks' error\n");
		return false;
	}

	if (!cmd->ReadFloat(&gwtScaling))
	{
		ERR("Read 'GWTFactor' error\n");
		return false;
	}

	g_region->m_gwt.Initialize(gwtTicks, gwtScaling);

	if (!cmd->ReadLong(&g_region->m_lCurRegionID))
	{
		ERR("Read 'id' error\n");
		return CMD_ERROR_READ_DATA;
	}

    /*
	g_cfgNet->gws_region_id = id;

	if (!cmd->ReadLong(&port))
	{
		ERR("Read 'port' error\n");
		return CMD_ERROR_READ_DATA;
	}

	g_cfgNet->client_listen_port = port;
    */

    SAFE_READ2(ReadLong, seed)

    char* pServerName;
    cmd->ReadString(&pServerName);
    g_region->m_strLoginName = pServerName;
    cmd->ReadString(&pServerName);
    g_region->m_strGameworldName = pServerName;

    g_region->ConnectJhWgServer();

	if(g_bUnWg)
	{
		unsigned long localIP = inet_addr(g_cfgRs.connectGameWorld.bindIP.c_str());
		localIP = InvertIPOrder(localIP);
		localIP &= 0x00FFFFFF;
		localIP <<= 8;
		localIP |= (g_region->m_lCurRegionID)&0x00FF;
		NES_Initialize_S(localIP, GMPunishWGCallback);
	}

	g_region->m_regionMap.clear();
    SAFE_READ2(ReadLong, count)

    int isc = 0;
    LOG1("This region ID = %d\n", g_region->m_lCurRegionID);
    MSG("Scene file list:");
	for (int n = 0; n < count; n++)
	{
		char* blockName;
		long regionID;

        SAFE_READ2(ReadString, blockName)
        SAFE_READ2(ReadLong, regionID)

		if(blockName[0] == '\0') continue;
		RtsSceneBlockMap* bmap = g_region->m_defScene->FindBlockByFileName(blockName);

		if (bmap == NULL)
		{
			LOG1("Can not find block map with name %s\n", blockName);
			continue;
		}

		g_region->m_regionMap[(ULONG)bmap] = regionID;
        g_region->m_regionMap.insert(EXT_SPACE::unordered_map<ULONG, long>::value_type((ULONG)bmap, regionID));

        // for 成战
        if (g_region->m_lCurRegionID==regionID) // 如果这个场景块是当前场景块
        {
            if ((isc%4)==0) MSG("\n");
            MSG1("  %s", blockName);
            isc ++;
            if (strcmp(blockName, "scene01_004.rtb")==0) // 如果文件名是指定的文件名
            {
                //西岐
				g_RegionCastleWarMgr.m_flag = 0;
            }
			
			if(strcmp(blockName, "scene01_030.rtb")==0)
			{
				//朝歌
				g_RegionCastleWarMgr.m_flag = 1;
			}
        }
	}
    MSG("\n");

	if (g_region->m_gws.m_seed != seed)
	{
		if (g_region->m_gws.m_seed)
		{
			LOG("The region seed is not valid, program will restart\n");

			g_region->Reboot();
		}

		g_region->m_gws.m_seed = seed;
	}

    // 载入怪物分配表
    LOG("Region: Load monster spawn table ... \n");
	g_spawnMgr.SetScene(g_region->m_defScene);
	g_spawnMgr.LoadSpawnTable(R(RES_TABLE_MONSTER_SPAWN));

    LOG("Region: Commit Pending Data ... \n");
	g_region->m_gws.CommitPendingData();

    LOG("Region: Start Listen ... \n");
	g_region->StartListen();

	//-------add start by tony 05.10.08------------//
#ifdef LICENSE_CHECK
	if(!g_region->m_gws.LicenseCheck(2))
	{
		ERR("License Check Error\n");
        g_region->Shutdown();
	}
#endif //LICENSE_CHECK
	//-------add end   by tony 05.10.08------------//

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_remove_region)
{
	long rgID;

    SAFE_READ2(ReadLong, rgID)

	if (rgID >= GW_MAX_REGION_COUNT)
	{
		LOG1("Invalid region ID %d\n", rgID);
		return CMD_ERROR_READ_DATA;
	}

	g_region->m_gws.m_regions[rgID].valid = 0;
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_add_region)
{
	long	rgID;
	char*	rgHost;
	short	rgPort;

    SAFE_READ2(ReadLong, rgID)

	if (rgID >= GW_MAX_REGION_COUNT)
	{
		LOG1("Invalid region ID %d\n", rgID);
		return CMD_ERROR_READ_DATA;
	}

    SAFE_READ2(ReadString, rgHost)
    SAFE_READ2(ReadShort, rgPort)

	CRegionGWSession::SRegionInfo& info = g_region->m_gws.m_regions[rgID];

	info.host = rgHost;
	info.port = rgPort;
	info.valid = 1;	
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_update_region_list)
{
	CRegionGWSession::SRegionInfo* rg = g_region->m_gws.m_regions;
	CRegionGWSession::SRegionInfo* stop = rg + GW_MAX_REGION_COUNT;

	for (; rg < stop; rg++)
		rg->valid = 0;
	
	long	rgID;
	char*	rgHost;
	short	rgPort;

	for (;;)
	{
        SAFE_READ2(ReadLong, rgID)

		if (rgID == -1)
			break;

		if ((unsigned long)rgID >= GW_MAX_REGION_COUNT)
		{
			LOG1("Invalid regionID %d\n", rgID);
			return CMD_ERROR_READ_DATA;
		}

        SAFE_READ2(ReadString, rgHost)
        SAFE_READ2(ReadShort, rgPort)

		CRegionGWSession::SRegionInfo& rgInfo = g_region->m_gws.m_regions[rgID];

		rgInfo.valid = 1;
		rgInfo.host = rgHost;
		rgInfo.port = rgPort;
	}
	
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_add_user)
{
	long userID;

	SAFE_READ2(ReadLong, userID)
	SUserData& user = g_region->m_gws.m_unloginUsers[userID];

	user.Serialize(*cmd,true);
	MSG1("GWS: Received a user [Name=%s]\n", user.name.c_str());
	g_region->ProcessLoginUser(userID);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_notify)
{
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_public_chat)
{
	char channel = 0;
	char* fromName;
	char* content;
	long  oldId = 0;

	if (!cmd->ReadByte(&channel))
	{
		ERR("Read 'channel' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadString(&fromName))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadString(&content))
	{
		ERR("Read 'content' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadLong(&oldId))
	{
		ERR("Read 'content' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser *pUser;
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_chat);
		g_sendCmd->WriteByte(channel);
		g_sendCmd->WriteLong(oldId);
		g_sendCmd->WriteString((char*)fromName);
		g_sendCmd->WriteString((char*)content);
        pUser->m_dummy->RecvCmd(g_sendCmd);

	}

	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_chatNew_list)
{

	char* fromName;
	char* Name;
	char* BossName;
	char* SceneName;
	long  type;
	char  channel;
	char* ItemName;

	long* toIDs;
	int toIDLength;

	if (!cmd->ReadBinary((char**)&toIDs, &toIDLength))
	{
		ERR("Read 'toIDs' error\n");
		return CMD_ERROR_READ_DATA;	
	}

	if (!cmd->ReadString(&fromName))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadString(&Name))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadString(&BossName))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadString(&SceneName))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadLong(&type))
	{
		ERR("Read 'content' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadByte(&channel))
	{
		ERR("Read 'channel' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (!cmd->ReadString(&ItemName))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if (channel == CHAT_CHANNEL_NEWS)
	{
		CRegionUser* pSender=g_region->FindUser(fromName);
		if (pSender)
		pSender->m_dummy->ChatNewS(fromName,Name,BossName,SceneName,type,ItemName);
	}
	
	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_chat)
{
	long receiverID;
	char* senderName;
	char channel;
	char* content;

	if (!cmd->ReadLong(&receiverID))
	{
		ERR("Read 'fromID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pReceiver=g_region->FindUser(receiverID);

	if(pReceiver==NULL)
	{
		//LOG1("Can not find from user %d\n", receiverID);
		return CMD_ERROR_NONE;
	}

	if (!cmd->ReadString(&senderName))
	{
		ERR("Read 'toName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadByte(&channel))
	{
		ERR("Read 'channel' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadString(&content))
	{
		ERR("Read 'content' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pReceiver->m_dummy->Chat(channel, content, pReceiver->m_dummy, 0, senderName);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_chat_list)
{
	char* fromName;
	char channel;
	char* content;

	long* toIDs;
	int toIDLength;

	if (!cmd->ReadBinary((char**)&toIDs, &toIDLength))
	{
		ERR("Read 'toIDs' error\n");
		return CMD_ERROR_READ_DATA;	
	}

	if (!cmd->ReadString(&fromName))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadByte(&channel))
	{
		ERR("Read 'channel' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadString(&content))
	{
		ERR("Read 'content' error\n");
		return CMD_ERROR_READ_DATA;
	}
	//heten
	if (channel == CHAT_CHANNEL_WORLD)
	{
		char faction = *toIDs;
		CRegionUser* pSender=g_region->FindUser(fromName);
		if (pSender)
			pSender->m_dummy->Chat(channel, content, 0 ,faction);
	}
	//end
	if (channel == CHAT_CHANNEL_PUBLIC)
	{
		char faction = *toIDs;
		CRegionUser* pSender=g_region->FindUser(fromName);
		if (pSender)
			pSender->m_dummy->Chat(channel, content, 0, faction);
	}
	else
	{
		long* stop = toIDs + toIDLength / sizeof(long);

		for (; toIDs < stop; toIDs++)
		{
			CRegionUser* pReceiver=g_region->FindUser(*toIDs);

			if(pReceiver==NULL)
				continue;

			pReceiver->m_dummy->Chat(channel, content, pReceiver->m_dummy, 0, fromName);
		}
	}
	return CMD_ERROR_NONE;
}


CMD_FUNCTION(cmd_g2r_team)
{
	long UserDBID;

	if(!cmd->ReadLong(&UserDBID))
	{
		ERR("Read 'UserDBID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserDBID);
	if(pUser==NULL)
	{
        //LOG1("cmd_g2r_team: Can not find user %d\n", UserDBID);
		return CMD_ERROR_NONE;
	}

	//收到
	pUser->m_dummy->m_pTeam->ReceiveFromGameWorld(cmd);
	return CMD_ERROR_NONE;
}
/*
CMD_FUNCTION(cmd_g2r_team)
{
	/*
	//------对服务端--------
	//发送消息到服务端
	struct SR2rData
	{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;
	mParameter=0;mHp=0;mMaxHp=0;mMp=0;mMaxMp=0;mpName=NULL;}
	
	byte	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	int		mHp;
	int		mMaxHp;
	int		mMp;
	int		mMaxMp;
	char*	mpName;
	};
	*//*
	long	UserDBID;


	char	Operation;
	long	DBIDInMessage;
	long	FromDBID;
	char*	pFromName;

	long	Parameter;
	long	Hp;
	long	MaxHp;
	long	Mp;
	long	MaxMp;
	char*	pName;

	if(!cmd->ReadLong(&UserDBID))
	{
		ERR("Read 'UserDBID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserDBID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_team: Can not find user %d\n", UserDBID);
		return CMD_ERROR_READ_DATA;
	}
	//////////////////////////////////////////////////////////

	if(!cmd->ReadByte(&Operation))
	{
		ERR("Read 'Operation' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&DBIDInMessage))
	{
		ERR("Read 'DBIDInMessage' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&FromDBID))
	{
		ERR("Read 'FromDBID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&pFromName))
	{
		ERR("Read 'pFromName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&Parameter))
	{
		ERR("Read 'Parameter' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&Hp))
	{
		ERR("Read 'Hp' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&MaxHp))
	{
		ERR("Read 'MaxHp' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&Mp))
	{
		ERR("Read 'Mp' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&MaxMp))
	{
		ERR("Read 'MaxMp' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&pName))
	{
		ERR("Read 'pName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	SR2rData R2rTeamData;
	R2rTeamData.BeginSend();
	R2rTeamData.mOperation=Operation;
	R2rTeamData.mDBIDInMessage=DBIDInMessage;
	R2rTeamData.mFromDBID=FromDBID;
	R2rTeamData.mpFromName=pFromName;
	R2rTeamData.mParameter=Parameter;
	R2rTeamData.mHp=Hp;
	R2rTeamData.mMaxHp=MaxHp;

	R2rTeamData.mMp=Mp;
	R2rTeamData.mMaxMp=MaxMp;
	R2rTeamData.mpName=pName;

	//收到
	pUser->m_dummy->m_pTeam->ReceiveFromServer(R2rTeamData);

	return CMD_ERROR_NONE;

}
*/

CMD_FUNCTION(cmd_g2r_PakeBlack_List)
{
	long UserID = 0;
	long SenderID =0;
	short type = 0;

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'UserID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", UserID);
		return CMD_ERROR_NONE;
	}


	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadShort(&type))
	{
		ERR("Read 'type' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceivePakeBlackList(SenderID,type);

	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_Black_List)
{
	long UserID = 0;
	long SenderID =0;
	char* Name;

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'UserID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", UserID);
		return CMD_ERROR_NONE;
	}


	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&Name))
	{
		ERR("Read 'Name' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceiveBlackList(SenderID,Name);

	return CMD_ERROR_NONE;


}

CMD_FUNCTION(cmd_g2r_notall_found)
{
	long SenderID =0;
	long UserID = 0;
	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(SenderID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceiveNotallFindFriend(UserID);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_not_found)
{
	long SenderID =0;
	long UserID = 0;
	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(SenderID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceiveNotFindFriend(UserID);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_one_refresh)
{
	long UserID = 0;
	long SenderID =0;
	char type = 0;
	

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", UserID);
		return CMD_ERROR_NONE;
	}

	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadByte(&type))
	{
		ERR("Read 'Type' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceiveOneRefreshServerID(SenderID,type);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_when_fiend)
{
	long userID;
	long SenderID;
	char* Name;
	short metier; 

	if(!cmd->ReadLong(&userID))
	{
		ERR("Read 'userID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(userID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}

	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&Name))
	{
		ERR("Read 'Name' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadShort(&metier))
	{
		ERR("Read 'Name' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceiveFromServerwhenfriend(Name,SenderID,metier);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_UserID_return)
{
	long SenderID;

	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(SenderID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}

	pUser->m_dummy->m_pFriend->ReceiveFromServerIDreturn();

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_SetRefuse_friend)
{
	long SenderID;
	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}
	CRegionUser* pUser=g_region->FindUser(SenderID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}

	pUser->m_dummy->m_pFriend->MsgSetRefusefriend();

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_friend_UserID)
{
	long SenderID;
	long UserID;
	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'SenderID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(SenderID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'UserID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	pUser->m_dummy->m_pFriend->ReceiveFromServerID(UserID);

	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_onefriend_return)
{
	long SenderID;	
	long UserID;	
	char* SceneName;
	long Lev;
	char Metier;
	char Type;

	if(!cmd->ReadLong(&SenderID))
	{
		ERR("Read 'UserDBID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(SenderID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_friend: Can not find user %d\n", SenderID);
		return CMD_ERROR_NONE;
	}
	//////////////////////////////////////////////////////////

	if(!cmd->ReadLong(&UserID))
	{
		ERR("Read 'DBIDInMessage' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&SceneName))
	{
		ERR("Read 'pFromName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&Lev))
	{
		ERR("Read 'Lev' error\n");
		return CMD_ERROR_READ_DATA;
	}
	if(!cmd->ReadByte(&Metier))
	{
		ERR("Read 'Metier' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadByte(&Type))
	{
		ERR("Read 'Type' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (Type == 1)
	{
		pUser->m_dummy->m_pFriend->ReceiveOneFromServerReturn(UserID,SceneName,Lev,Metier);
	}
	else if (Type == 2)
	{
		pUser->m_dummy->m_pFriend->ReceiveAllFromServerReturn(UserID,SceneName,Lev,Metier);
	}

	

	return CMD_ERROR_NONE;

}

CMD_FUNCTION(cmd_g2r_friend)
{
	/*
	//------对服务端--------
	struct SR2rFriendData
	{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;mParameter=0;mOnline=false;mpName=NULL;}
	byte	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	bool	mOnline;
	char*	mpName;
	};
	*/

	long	UserDBID;
	////////////////////////////////////////
	char	Operation;
	long	DBIDInMessage;
	long	FromDBID;
	char*	pFromName;

	long	Parameter;
	char	Online;
	char*	pName;
	char    IsFristOnline;
	char    IsLeave;

	if(!cmd->ReadLong(&UserDBID))
	{
		ERR("Read 'UserDBID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserDBID);
	if(pUser==NULL)
	{
        LOG1("cmd_g2r_friend: Can not find user %d\n", UserDBID);
		return CMD_ERROR_NONE;
	}
	//////////////////////////////////////////////////////////

	if(!cmd->ReadByte(&Operation))
	{
		ERR("Read 'Operation' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&DBIDInMessage))
	{
		ERR("Read 'DBIDInMessage' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&FromDBID))
	{
		ERR("Read 'FromDBID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&pFromName))
	{
		ERR("Read 'pFromName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadLong(&Parameter))
	{
		ERR("Read 'Parameter' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadByte(&Online))
	{
		ERR("Read 'Online' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadString(&pName))
	{
		ERR("Read 'pName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadByte(&IsFristOnline))
	{
		ERR("Read 'IsFristOnline' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if(!cmd->ReadByte(&IsLeave))
	{
		ERR("Read 'IsLeave' error\n");
		return CMD_ERROR_READ_DATA;
	}


	SR2rFriendData R2rFriendData;
	R2rFriendData.BeginSend();
	R2rFriendData.mOperation=Operation;
	R2rFriendData.mDBIDInMessage=DBIDInMessage;
	R2rFriendData.mFromDBID=FromDBID;
	R2rFriendData.mpFromName=pFromName;


	R2rFriendData.mParameter=Parameter;
	R2rFriendData.mOnline=(Online==1);
	R2rFriendData.mpName=pName;
	R2rFriendData.IsFristOnline=IsFristOnline;
	R2rFriendData.IsLeave=IsLeave;


	//收到
	pUser->m_dummy->m_pFriend->ReceiveFromServer(R2rFriendData);

	return CMD_ERROR_NONE;


}

CMD_FUNCTION(cmd_g2r_Item_Mail)
{
	long UserID;
	char* StrItem;
	long Money;

	if (!cmd->ReadLong(&UserID))
	{
		ERR("Read 'fromID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(UserID);
	if(pUser==NULL)
	{
		LOG1("cmd_g2r_mail: Can not find user %d\n", UserID);
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadString(&StrItem))
	{
		ERR("Read 'fromName' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadLong(&Money))
	{
		ERR("Read 'fromID' error\n");
		return CMD_ERROR_READ_DATA;
	}
	
	string StrItemID(StrItem);
	pUser->m_dummy->m_pMail->MoveItem(StrItemID,UserID,Money);

	return CMD_ERROR_NONE;
}


CMD_FUNCTION(cmd_g2r_mail)
{
	long MailID;
	long userID;
	long numMails;
	long senderID;
	char* date;
	char* senderName;
	char* receivedDate;
	char* title;
	char* content;
	char* StrItemID;
	long Money;
	long IsOpen;

	if (!cmd->ReadLong(&userID))
	{
		ERR("Read 'userID' error\n");
		return CMD_ERROR_READ_DATA;
	}

	CRegionUser* pUser=g_region->FindUser(userID);
	if(pUser==NULL)
	{
        LOG1("cmd_g2r_mail: Can not find user %d\n", userID);
		return CMD_ERROR_READ_DATA;
	}


	if (!cmd->ReadString(&date))
	{
		ERR("Read 'date' error\n");
		return CMD_ERROR_READ_DATA;
	}

	if (!cmd->ReadLong(&numMails))
	{
		ERR("Read 'numMails' error\n");
		return CMD_ERROR_READ_DATA;
	}

	for (long n = 0; n < numMails; n++)
	{
		if (!cmd->ReadLong(&MailID))
		{
			ERR("Read 'fromID' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadLong(&senderID))
		{
			ERR("Read 'fromID' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadString(&senderName))
		{
			ERR("Read 'fromName' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadString(&StrItemID))
		{
			ERR("Read 'MailItemID' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadLong(&Money))
		{
			ERR("Read 'MailMoney' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadString(&receivedDate))
		{
			ERR("Read 'receivedDate' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadString(&title))
		{
			ERR("Read 'title' error\n");
			return CMD_ERROR_READ_DATA;
		}

		if (!cmd->ReadString(&content))
		{
			ERR("Read 'content' error\n");
			return CMD_ERROR_READ_DATA;
		}
		if (!cmd->ReadLong(&IsOpen))
		{
			ERR("Read 'MailMoney' error\n");
			return CMD_ERROR_READ_DATA;
		}

		string SendName(senderName);
		string ReceivedDate(receivedDate);
		pUser->m_dummy->m_pMail->ReceiveFromGameWorld(MailID,senderID,ReceivedDate,SendName,title,content,StrItemID,Money,IsOpen);
	}

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_login)
{
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_kick_user)
{
	long userID;
	char cCause;
    char* szCause;

	if (!cmd->ReadLong(&userID))
	{
		ERR("Read 'userID' error\n");
		return CMD_ERROR_READ_DATA;
	}
	
	if (!cmd->ReadByte(&cCause))
	{
		ERR("Read 'cCause' error\n");
		return CMD_ERROR_READ_DATA;
	}

    if (cCause==4) // KICK_USER_USERDEFINE
    {
        cmd->ReadString(&szCause);
    }else
    {
        szCause = NULL;
	}
	szCause = "GW kick";

	g_region->m_gws.KickUser(userID);
	g_region->KickUser(userID, cCause, true, true, szCause);

	return CMD_ERROR_NONE;
}


CMD_FUNCTION(cmd_g2r_union)
{
	return UnionCmdProcess_FromGW(cmd);
}

//-------------add start by tony 05.06.16------------------------//
CMD_FUNCTION(cmd_g2r_castlewar)
{
	return CastleWarCmdProcess_FromGW(cmd);
}
//-------------add end   by tony 05.06.16------------------------//
//rendh 2010.01.21
CMD_FUNCTION(cmd_g2r_linesys)
{
	return LineSysCmdProcess_FromGW(cmd);
}
//rendh 2010.01.21
CMD_FUNCTION(cmd_g2r_auction)
{
	return AuctionCmdProcess_FromGW(cmd);
}

CMD_FUNCTION(cmd_g2r_feod)
{
	return FeodCmdProcess_FromGW(cmd);
}

CMD_FUNCTION(cmd_g2r_start_fun)
{
	g_region->StartFun(false);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_end_fun)
{
	g_region->EndFun();
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_card_trade_switch)
{
	char cOnoff;
	if (!cmd->ReadByte(&cOnoff))
	{
		return CMD_ERROR_READ_DATA;
	}
    if (cOnoff)
    {
        g_region->m_rsCardTrade.StartTrade();
    }else
    {
        g_region->m_rsCardTrade.StopTrade();
    }
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_herolist)
{
	guard;

	long listVersion;
	if (!cmd->ReadLong(&listVersion))
		return CMD_ERROR_READ_DATA;

	g_region->m_HeroList.m_version = listVersion;
	g_region->m_HeroList.Serialize(cmd);

	return CMD_ERROR_NONE;
	unguard;
}

CMD_FUNCTION(cmd_g2r_itemid)
{
	guard;

	long nowID;
	long maxID;
	if (!cmd->ReadLong(&nowID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&maxID))
		return CMD_ERROR_READ_DATA;

	g_ItemIDFactory.OnRecvItemIDFromDB(nowID, maxID);
	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_gm_response)
{
	guard;

	long userID;
	long GmID;
	char* GmName;
	short QuestionCount;
	short Order;

	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&GmID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadString(&GmName))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&QuestionCount))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&Order))
		return CMD_ERROR_READ_DATA;

	CRegionUser* pUser = g_region->FindUser(userID);
	if (pUser && pUser->m_dummy)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_gm_response);
		g_sendCmd->WriteLong(GmID);
		g_sendCmd->WriteString(GmName);
		g_sendCmd->WriteShort(QuestionCount);
		g_sendCmd->WriteShort(Order);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}
	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_gm_bulletin)
{
	guard;

	long GmID;
	char* content;
	long color;
	char style;

	if (!cmd->ReadLong(&GmID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadString(&content))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&color))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadByte(&style))
		return CMD_ERROR_READ_DATA;

	CRegionUser *pUser;
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;

        if (style==1)
        {
            pUser->m_dummy->SendSystemMessage(content);
        }else
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_gm_bulletin);
            g_sendCmd->WriteLong(GmID);
            g_sendCmd->WriteString(content);
            g_sendCmd->WriteLong(color);
            g_sendCmd->WriteByte(style);
            pUser->m_dummy->RecvCmd(g_sendCmd);
        }
	}

	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_user_info)
{
	guard;

	long GmID;
	SCreatureInfo info;

	if (!cmd->ReadLong(&GmID))
		return CMD_ERROR_READ_DATA;
	if (!info.Serialize(cmd))
		return CMD_ERROR_READ_DATA;

	CRegionUser *pUser = g_region->FindUser(GmID);
	if (pUser && pUser->m_dummy)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_user_data);
		info.Serialize(g_sendCmd);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_query_user_info)
{
	guard;

	long GmID;
	long GuyID;

	if (!cmd->ReadLong(&GmID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&GuyID))
		return CMD_ERROR_READ_DATA;

	CRegionUser* pUser = g_region->FindUser(GuyID);
	if (pUser && pUser->m_dummy)
	{
		SCreatureInfo info;
		pUser->m_dummy->ToCreatureInfo(&info);

		CG_CmdPacket& c = g_region->m_gws.BeginSend();
		c.WriteShort(r2g_user_info);
		c.WriteLong(GmID);
		info.Serialize(&c);
		g_region->m_gws.EndSend();
	}

	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_user_state)
{
	guard;

	long userID;
	long dwState;

	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&dwState))
		return CMD_ERROR_READ_DATA;

	CRegionUser* pUser = g_region->FindUser(userID);
	if (pUser && pUser->m_dummy)
	{
		long dwOldState = pUser->m_dummy->m_dwState;

		if ( (dwOldState&CreState_CanSpeak) != (dwState&CreState_CanSpeak) )
		{
			if (dwOldState&CreState_CanSpeak)
			{
				pUser->m_dummy->State_SetSpeakable(false);
			}
			else
			{
				pUser->m_dummy->State_SetSpeakable(true);
			}
		}
		if ( (dwOldState&CreState_CanPShop) != (dwState&CreState_CanPShop) )
		{
			if (dwOldState&CreState_CanPShop)
			{
				pUser->m_dummy->State_SetPShopable(false);
			}
			else
			{
				pUser->m_dummy->State_SetPShopable(true);
			}
		}
		if ( (dwOldState&CreState_CanMove) != (dwState&CreState_CanMove) )
		{
			if (dwOldState&CreState_CanMove)
			{
				pUser->m_dummy->State_SetMovable(false);
			}
			else
			{
				pUser->m_dummy->State_SetMovable(true);
			}
		}
	}

	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_query_gifts_response)
{
	guard;

	long userID;
	short i = 0;
	short countCanWithdraw = 0;
	short countCanNotWithdraw = 0;
	SGift gift;
	std::vector<SGift> giftsCanWithdraw;
	std::vector<SGift> giftsCanNotWithdraw;

	// 读取数据报
	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&countCanWithdraw))
		return CMD_ERROR_READ_DATA;
	for (i=0; i<countCanWithdraw; i++)
	{
		if (!gift.Serialize(cmd))
			return CMD_ERROR_READ_DATA;
		giftsCanWithdraw.push_back(gift);
	}
	if (!cmd->ReadShort(&countCanNotWithdraw))
		return CMD_ERROR_READ_DATA;
	for (i=0; i<countCanNotWithdraw; i++)
	{
		if (!gift.Serialize(cmd))
			return CMD_ERROR_READ_DATA;
		giftsCanNotWithdraw.push_back(gift);
	}

	// 确认玩家存在
	CRegionUser* pUser = g_region->FindUser(userID);
	if (!pUser || !pUser->m_dummy)
		return CMD_ERROR_NONE;

	// 取得下一个礼品
	if (giftsCanWithdraw.empty())
	{
		if (giftsCanNotWithdraw.empty())
		{
			CRegionGiftMgr::Build_r2c_get_gift_response(GIFT_NONE,"");
			pUser->m_dummy->RecvCmd(g_sendCmd);
		}
		else 
		{
			CRegionGiftMgr::Build_r2c_get_gift_response(GIFT_CAN_NOT_WITHDRAW,"");
			pUser->m_dummy->RecvCmd(g_sendCmd);
		}
	}
	else
	{
		SGift& gift = giftsCanWithdraw.back();
		char errorCode;
		if (CRegionGiftMgr::WithdrawGift(gift, errorCode))
		{
			CRegionGiftMgr::NetSend_r2g_withdraw_gift_success(userID, gift);
			CRegionGiftMgr::Build_r2c_get_gift_response(GIFT_SUCCESS, gift.message.c_str());
			pUser->m_dummy->RecvCmd(g_sendCmd);
			g_region->m_gws.SaveUserData(userID, pUser->m_dummy);
		}
		else 
		{
			CRegionGiftMgr::Build_r2c_get_gift_response(errorCode,"");
			pUser->m_dummy->RecvCmd(g_sendCmd);
		}
	}

	return CMD_ERROR_NONE;

	unguard;
}

CMD_FUNCTION(cmd_g2r_activity_system)
{
	guard;
	CRegionActManager::GetSingle()->OnEvent(cmd);
	return CMD_ERROR_NONE;
	unguard;
}


void CRegionGWSession::Process()
{	
	CG_TCPSession::Process();

    time_t now = time(NULL);
	switch(GetState())
	{
	default:
		/*
		if (m_disconnectTimeStamp)
		{
			LOG("Connect to GameWorld server timeout (30s), program will exit\n");
			g_region->Reboot();
		}
		GetSocket().Close();
		*/
        if (now-m_disconnectTimeStamp > 0) // 每秒连接一次
        {
            if (!CG_TCPSession::Connect(
                (char*)g_cfgRs.connectGameWorld.ip.c_str(), 
                g_cfgRs.connectGameWorld.port,
                (char*)g_cfgRs.connectGameWorld.bindIP.c_str(),
                g_cfgRs.connectGameWorld.bindPort))
            {
                ERR("Cannot connected GWS. reboot now.\n");
                g_region->Reboot();
            }
        }
		break;

	case NET_STATE_CONNECTING:
	case NET_STATE_WAITING_SEED:
		break;

	case NET_STATE_CONNECTED:
		if(time(NULL) - m_lastProcessTime > 5)
		{
			m_lastProcessTime = time(NULL);
			int gate = now - g_cfg.gws.update_user_interval;			
			for (EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator user = g_region->m_userMap.begin();
				user != g_region->m_userMap.end(); user++)
			{
				if (user->second->m_updateTimeStamp < gate)
				{
					if (!g_region->m_gws.SaveUserData(user->second->m_userId, user->second->m_dummy))
						return;
					user->second->m_updateTimeStamp = now;
				}
			}

			time_t repatriation = now - g_cfg.gws.accept_user_timeout;
            std::list<int> listRepatriateUsers;
            std::list<int>::iterator itListRep;
            CRegionUnloginUserTable::iterator iter;
			for (iter = m_unloginUsers.begin(); iter != m_unloginUsers.end(); iter++)
			{
				if (iter->second.timeStamp < repatriation)
				{
					MSG2("GWS: Repatriate a user [ name=%s, ID=%d ]\n", iter->second.name.c_str(), iter->first);
					if (!SendRepatriateEvent(iter->first))
						return;
                    listRepatriateUsers.push_back(iter->first);
				}
			}
            for (itListRep=listRepatriateUsers.begin(); itListRep!=listRepatriateUsers.end(); itListRep++)
            {
                iter = m_unloginUsers.find(*itListRep);
                m_unloginUsers.erase(iter);
            }
		}
		break;
	}
}

CMD_FUNCTION(cmd_g2r_add_master_prentice_result)
{
	g_PrenticeMgr.OnRecvGW_AddMasterPrentice(cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_remove_master_prentice_result)
{
	g_PrenticeMgr.OnRecvGW_RemoveMasterPrentice(cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_finish_master_prentice_result)
{
	g_PrenticeMgr.OnRecvGW_FinishMasterPrentice(cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_master_prentice_update)
{
	long userID;
	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;

	CRegionUser *pUser = g_region->FindUser(userID);
	if (pUser && pUser->m_dummy)
	{
		if (!pUser->m_dummy->m_pFriend->MendTeacherAndStudentInfo(cmd))
			return CMD_ERROR_UNKNOWN;
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_master_prentice_credit)
{
	long userID;
	short credit;
	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&credit))
		return CMD_ERROR_READ_DATA;

	CRegionUser* pUser = g_region->FindUser(userID);
	if (!pUser || !pUser->m_dummy)
	{
		ERR1("[cmd_g2r_master_prentice_credit] The User (id=%d) is NOT In This Region, But GWS Think It Is!\n", userID);
		return CMD_ERROR_NONE;
	}

	pUser->m_dummy->AddCredit(credit);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_prentice_onlinetime)
{
	long userID;
	long prenticeID;
	long onlineTime;
	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&prenticeID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&onlineTime))
		return CMD_ERROR_READ_DATA;

	CRegionUser* pUser = g_region->FindUser(userID);
	if (pUser)
	{
		CRegionPrenticeCmd::Build_r2c_prentice_onlinetime(prenticeID, onlineTime);
		pUser->RecvCmd(g_sendCmd);
	}
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_master_prentice_gift)
{
	long userID;
	long lev;
	short errorCode;
	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadLong(&lev))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&errorCode))
		return CMD_ERROR_READ_DATA;

	CRegionUser *pUser = g_region->FindUser(userID);
	if (pUser)
	{
		switch(errorCode)
		{
		case prentice_code_success:
			if (!g_PrenticeMgr.WithdrawMPGift(pUser->m_dummy, lev))
			{
				ERR1("[cmd_g2r_master_prentice_gift] WithdrawMPGift Error!(UserName=%s)\n", pUser->m_username.c_str());
			}
			// 避免回档，提交一次数据
			g_region->m_gws.SaveUserData(pUser->m_userId, pUser->m_dummy);
			break;
		case prentice_code_gift_withdrawed:
		case prentice_code_error_unknown:
			break;
		default:
			break;
		}
	}

	g_PrenticeMgr.UnlockUserPrenSys(pUser);
	CRegionPrenticeCmd::Build_r2c_prentice_gift_result(errorCode, lev);
	pUser->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_master_list)
{
	long UserDBID;
	if(!cmd->ReadLong(&UserDBID))
	{
		return CMD_ERROR_READ_DATA;
	}
	g_region->m_MasterList.ReceiveFromGameWorld(UserDBID,cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_auction_failed)
{
	SItemID id;
    long lUserID;
    short sType;

    cmd->ReadLong(&lUserID);
    id.Serialize(cmd);
	cmd->ReadShort(&sType);

    g_region->m_rsAuction.OnAuctionFailed(id, lUserID, sType);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_auction_bid_failed)
{
    long lUserID, lBidPrice;
    short sType;

    cmd->ReadLong(&lUserID);
    cmd->ReadLong(&lBidPrice);
	cmd->ReadShort(&sType);

    g_region->m_rsAuction.OnBidFailed(lUserID, lBidPrice, sType);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_auction_update)
{
    g_region->m_rsAuction.OnUpdateAuction(cmd);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_auction_event)
{
    short sOP;
    SAuctionItem item;

    cmd->ReadShort(&sOP);
    if (sOP==EAOP_UPDATE_TO_CLIENT)
    {
        g_region->m_rsAuction.OnUpdateToClient(cmd);
    }else
    {
        item.Serialize(cmd);
        g_region->m_rsAuction.OnEvent((EAuctionOP)sOP, &item);
    }
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_auction_get_result)
{
    long lUserID, lMoney = 0;
    char cMoney, cItem;
    SItemID itemID;

    cmd->ReadLong(&lUserID);
    cmd->ReadByte(&cMoney);
    if (cMoney) cmd->ReadLong(&lMoney);
    cmd->ReadByte(&cItem);
    if (cItem)  itemID.Serialize(cmd);

    g_region->m_rsAuction.OnGetItems(lUserID, cMoney, lMoney, cItem, itemID);
	return CMD_ERROR_NONE;
}

CMD_FUNCTION(cmd_g2r_bankpass_result)
{
	long userID;
	short operateID;
	short errorCode;
	char* newPass;
	if (!cmd->ReadLong(&userID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&operateID))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadShort(&errorCode))
		return CMD_ERROR_READ_DATA;
	if (!cmd->ReadString(&newPass))
		return CMD_ERROR_READ_DATA;

	CRegionUser* pUser = g_region->FindUser(userID);
	if (!pUser || !pUser->m_dummy)
	{
		return CMD_ERROR_NONE;
	}

	switch (errorCode)
	{
	case 0:
		pUser->m_bankPass = newPass;
		if (operateID==1)
			CItemCmdBuilder_Svr::Build_r2c_bank_addpass_result(errorCode);
		else if (operateID==2)
			CItemCmdBuilder_Svr::Build_r2c_bank_removepass_result(errorCode);
		else if (operateID==3)
			CItemCmdBuilder_Svr::Build_r2c_bank_changepass_result(errorCode);
		else 
			return CMD_ERROR_NONE;
		pUser->RecvCmd(g_sendCmd);
		break;
	case 1:
		if (operateID==1)
			CItemCmdBuilder_Svr::Build_r2c_bank_addpass_result(errorCode);
		else if (operateID==2)
			CItemCmdBuilder_Svr::Build_r2c_bank_removepass_result(errorCode);
		else if (operateID==3)
			CItemCmdBuilder_Svr::Build_r2c_bank_changepass_result(errorCode);
		else 
			return CMD_ERROR_NONE;
		pUser->RecvCmd(g_sendCmd);
		break;
	default:
		break;
	}
	return CMD_ERROR_NONE;
}

/*
CMD_FUNCTION(cmd_auth2r_license_data)
{
	char* pData;

	SAFE_READ2(ReadString, pData)

	g_region->m_AuthSession.m_bRecvData = true;

	g_region->m_AuthSession.SaveAuthData(pData);

	return CMD_ERROR_NONE;
}
*/

void CRegionGWSession::InitChannel()
{
    m_bSafeQuited = false;

	m_recvCmd.SetSize(16*1024);
	m_sendCmd.SetSize(16*1024);

	SetBufSize(1024);
    m_channel.m_channelName = "GWSession";

#define REG_CMD(c) \
	m_channel.AddCmd(cmd_##c, c, #c, "")
	
	REG_CMD(g2r_add_user);
	REG_CMD(g2r_chat);
	REG_CMD(g2r_chat_list);
	REG_CMD(g2r_chatNew_list);
	REG_CMD(g2r_public_chat);
	REG_CMD(g2r_login);	
	REG_CMD(g2r_mail);
	REG_CMD(g2r_Item_Mail);
	REG_CMD(g2r_kick_user);
	REG_CMD(g2r_shutdown);
	REG_CMD(g2r_update_region_list);
	REG_CMD(g2r_add_region);
	REG_CMD(g2r_remove_region);
	REG_CMD(g2r_union);
	REG_CMD(g2r_feod);
	REG_CMD(g2r_init_region);
   	REG_CMD(g2r_herolist);
	REG_CMD(g2r_reboot_region);	
	REG_CMD(g2r_itemid);
	REG_CMD(g2r_pull_user);
	REG_CMD(g2r_move_user);
	REG_CMD(g2r_team);
	REG_CMD(g2r_friend);
	REG_CMD(g2r_onefriend_return);
	REG_CMD(g2r_friend_UserID);
	REG_CMD(g2r_UserID_return);
	REG_CMD(g2r_when_fiend);
	REG_CMD(g2r_one_refresh);
	REG_CMD(g2r_not_found);
	REG_CMD(g2r_notall_found);
	REG_CMD(g2r_Black_List);
	REG_CMD(g2r_PakeBlack_List);
	REG_CMD(g2r_gm_response);
	REG_CMD(g2r_gm_bulletin);
    REG_CMD(g2r_safe_quited);
	REG_CMD(g2r_feod_data);
	//----------add start by tony 05.06.16---------------//
	REG_CMD(g2r_castlewar);
	//----------add end   by tony 05.06.16---------------//
	//rendh 2010.01.21
	REG_CMD(g2r_linesys);
	//rendh 2010.01.21
    REG_CMD(g2r_check_version_ret);
    REG_CMD(g2r_tester);
	REG_CMD(g2r_pull_useraround);
	REG_CMD(g2r_move_useraround);
	REG_CMD(g2r_query_user_info);
	REG_CMD(g2r_user_info);
	REG_CMD(g2r_user_state);
	REG_CMD(g2r_query_gifts_response);

	REG_CMD(g2r_start_fun);
	REG_CMD(g2r_end_fun);

	REG_CMD(g2r_create_dungeon);
	REG_CMD(g2r_enter_dungeon_ret);
	REG_CMD(g2r_leave_dungeon_ret);
	REG_CMD(g2r_destroy_dungeon_ret);

	REG_CMD(g2r_broadcast_to_region);
	REG_CMD(g2r_query_poing_ret);
	REG_CMD(g2r_prededuct_ret);
	REG_CMD(g2r_deduct_ret);
	REG_CMD(g2r_add_point_ret);
	REG_CMD(g2r_execute_gmcmd);
	REG_CMD(g2r_add_account_time_ret);		//ac.ma		增加会员时间

	REG_CMD(g2r_license_check_ret);

	REG_CMD(g2r_card_consign_ret);
	REG_CMD(g2r_card_buy_ret);
	REG_CMD(g2r_card_cancel_ret);
	REG_CMD(g2r_card_update);

	REG_CMD(g2r_add_master_prentice_result);
	REG_CMD(g2r_remove_master_prentice_result);
	REG_CMD(g2r_finish_master_prentice_result);
	REG_CMD(g2r_master_prentice_update);
	REG_CMD(g2r_master_prentice_credit);
	REG_CMD(g2r_prentice_onlinetime);
	REG_CMD(g2r_master_prentice_gift);

	REG_CMD(g2r_bankpass_result);

	REG_CMD(g2r_kick_all_user);
	REG_CMD(g2r_card_trade_switch);
	REG_CMD(g2r_auction);
	REG_CMD(g2r_master_list);
	REG_CMD(g2r_auction_failed);
	REG_CMD(g2r_auction_bid_failed);
	REG_CMD(g2r_auction_update);
	REG_CMD(g2r_auction_event);
	REG_CMD(g2r_auction_get_result);

	REG_CMD(g2r_warfield_start);
	REG_CMD(g2r_warfield_stop);
	REG_CMD(g2r_warfield_flag_destroyed_rep);
	REG_CMD(g2r_warfield_leave_rep);
	REG_CMD(g2r_warfield_msg);
	REG_CMD(g2r_warfield_addscore_rep);
	REG_CMD(g2r_warfield_update_data);
	REG_CMD(g2r_warfield_update_rate);
	REG_CMD(g2r_update_poing_ret);

	REG_CMD(g2r_activity_system);

	REG_CMD(g2r_SetRefuse_friend);

#undef REG_CMD
}


void CRegionLsSession::InitChannel()
{
	m_recvCmd.SetSize(16*1024);
	m_sendCmd.SetSize(16*1024);

	SetBufSize(1024);

	/*
	m_channel.m_channelName = "LsSession";

#define REG_CMD(c) \
	m_channel.AddCmd(cmd_##c, c, #c, "")

	REG_CMD(auth2r_license_data);

#undef REG_CMD
	*/
}

bool CRegionLsSession::Connect(const char* remote_ip, const int remote_port)
{
	if (!CG_TCPSession::Connect(
		(char*)remote_ip,
		remote_port,
		(char*)g_cfgRs.connectGameWorld.bindIP.c_str(),
		0))
	{
		//LOG("Cannot connected AuthSvr.\n");
		return false;
	}
	return true;
}

void CRegionLsSession::SaveAuthData(char* p_data)
{
	if(p_data)
	{
		LOG1("CheckCode: %s\n", p_data);
		m_strCheckCode = p_data;
	}
}

void CRegionLsSession::Close()
{
	CG_TCPSession::Close();
}

void CRegionLsSession::OnConnect(bool ret)
{
	if (ret)
	{
		ERR1("Connect AuthSvr succeeded. at %d\n", rtTimestamp());
		m_bConnected = true;
		SetBufSize(1024*5);
		GetSocket().SetSendBufferSize(256*1024);
	}
	else
	{
		m_bConnected = false;
		MSG1("Connect AuthSvr FAILED, %d\n", rtTimestamp());
		m_disconnectTimeStamp = time(NULL);
	}
}

void CRegionLsSession::OnDisconnect()
{
	MSG1("Disconnect from Auth server at %d\n", rtTimestamp());

	m_disconnectTimeStamp = time(NULL);
}

void CRegionLsSession::OnReceive(CG_CmdPacket* packet)
{
	short cmd;

	if (!packet->ReadShort(&cmd))
	{
		ERR("Read command error from Auth server\n");
		return;
	}

	if (cmd==auth2r_license_data)
	{
		char* pData;
		if (packet->ReadString(&pData))
		{
			m_bRecvData = true;
			SaveAuthData(pData);
		}
	}
}

void CRegionLsSession::Process()
{	
	CG_TCPSession::Process();

	time_t now = time(NULL);
	switch(GetState())
	{	
	case NET_STATE_CONNECTING:
		break;
	case NET_STATE_WAITING_SEED:
		break;
	case NET_STATE_CONNECTED:
		break;
	default:
		this->Connect(m_strHostIP.c_str(), m_iHostPort);
		break;
	}
}

bool CRegionGWSession::SaveUserData(SUserData &data)
{
	CG_CmdPacket& cmd = BeginSend();
	
	cmd.WriteShort(r2g_update_user);
	cmd.WriteLong(data.userID);
	data.Serialize(cmd,false);
	
	return EndSend();
}

bool CRegionGWSession::SaveUserData(long userID, CRegionCreature* cre)
{
    if (g_cfg.game.bDebugMsg)
    {
        ERR1("Save user data [%s]\n",cre->m_core.Name.c_str());
    }
	if(GetState() == NET_STATE_CONNECTED)
	{
		if(!cre->m_userInfo->m_bSaveData) return true;

		//LeiJun 保存用户活动相关数据
		cre->m_userInfo->SaveAllActivityInfo();

		SUserData data;
		cre->Save(data);

		CG_CmdPacket& cmd = BeginSend();
		cmd.WriteShort(r2g_update_user);
		cmd.WriteLong(data.userID);
		data.Serialize(cmd,false);
		return EndSend();
	}
	else
	{
		PushPendingSaveUser(userID, cre);

		//LeiJun 保存用户活动相关数据
		cre->m_userInfo->SaveAllActivityInfo(true);
	}

	return true;
}

///
bool CRegionGWSession::AcceptUser(long userID)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_accept_user);
	cmd.WriteLong(userID);

	return EndSend();
}

///
bool CRegionGWSession::RepatriateUser(long userID)
{
	SendRepatriateEvent(userID);

	m_unloginUsers.erase(userID);

	return true;
}

bool CRegionGWSession::SendRepatriateEvent(long userID)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_repatriate_user);
	cmd.WriteLong(userID);

	return EndSend();
}

/*
struct SR2rData
{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;
	mParameter=0;mHp=0;mMaxHp=0;mMp=0;mMaxMp=0;mpName=NULL;}
	
	byte	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	int		mHp;
	int		mMaxHp;
	int		mMp;
	int		mMaxMp;
	char*	mpName;
};
*/

/*
void CRegionGWSession::SendTeam(const DWORD receiverID,SR2rData& data)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_team);
	cmd.WriteLong(receiverID);		//接收者DBID

	cmd.WriteByte(data.mOperation);
	cmd.WriteLong(data.mDBIDInMessage);
	cmd.WriteLong(data.mFromDBID);
	
	data.mpFromName==NULL ?  cmd.WriteString("") : cmd.WriteString(data.mpFromName) ;
	
	cmd.WriteLong(data.mParameter);
	cmd.WriteLong(data.mHp);
	cmd.WriteLong(data.mMaxHp);
	cmd.WriteLong(data.mMp);
	cmd.WriteLong(data.mMaxMp);
	data.mpName==NULL ?  cmd.WriteString("") : cmd.WriteString(data.mpName) ;
	EndSend();
}
*/

/*
//------对服务端--------
struct SR2rFriendData
{
void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;mParameter=0;mOnline=false;mpName=NULL;}
byte	mOperation;
DWORD	mDBIDInMessage;
DWORD	mFromDBID;
char*	mpFromName;

DWORD	mParameter;
bool	mOnline;
char*	mpName;
};
*/

void CRegionGWSession::SendOneFriendReturn(long SenderID,long UserID,const char* SceneName,long Lev,byte Type,char Metier)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_OneFriend_Return);
	cmd.WriteLong(SenderID);	
	cmd.WriteLong(UserID);	
	cmd.WriteString((char*)SceneName);
	cmd.WriteLong(Lev);	
	cmd.WriteByte(Metier);
	cmd.WriteByte(Type);
	EndSend();
}

void CRegionGWSession::SendRefusesetting(long ID,char bRefuseTeam,char bRefuseAction,char bRefuseMail,char bRefuseDuel,char bRefuseFriend,char bRefuseMonster)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_Set_Refuse);
	cmd.WriteLong(ID);	
	cmd.WriteByte(bRefuseTeam);
	cmd.WriteByte(bRefuseAction);
	cmd.WriteByte(bRefuseMail);
	cmd.WriteByte(bRefuseDuel);
	cmd.WriteByte(bRefuseFriend);
	cmd.WriteByte(bRefuseMonster);
	EndSend();
}

void CRegionGWSession::SendFriend(const DWORD receiverID,SR2rFriendData& data)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_friend);
	cmd.WriteLong(receiverID);		//接收者DBID


	cmd.WriteByte(data.mOperation);
	cmd.WriteLong(data.mDBIDInMessage);
	cmd.WriteLong(data.mFromDBID);
	data.mpFromName==NULL ?  cmd.WriteString("") : cmd.WriteString(data.mpFromName) ;

	cmd.WriteLong(data.mParameter);
	cmd.WriteByte(data.mOnline);
	data.mpName==NULL ?  cmd.WriteString("") : cmd.WriteString(data.mpName) ;
	cmd.WriteByte(data.IsFristOnline);
	EndSend();
}


void CRegionGWSession::SendChat(const char* receiver, const char* senderName, char channel, const char* content, const char* title/*=""*/,long oldId)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_chat);
	cmd.WriteLong(0L);
	cmd.WriteString((char*)receiver);
	cmd.WriteString((char*)senderName);
	cmd.WriteByte(channel);
	cmd.WriteString((char*)content);
	cmd.WriteString((char*)title);
	cmd.WriteLong(oldId);
	

	EndSend();
}
void CRegionGWSession::SendSystemNews(const char *Playername,const char *Name,const char *BossName,const char *SceneName,long type,char channel,const char *ItemName)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_SystemNews);
	cmd.WriteString((char*)Playername);
	cmd.WriteString((char*)Name);
	cmd.WriteString((char*)BossName);
	cmd.WriteString((char*)SceneName);
	cmd.WriteLong(type);
	cmd.WriteByte(channel);
	cmd.WriteString((char*)ItemName);

	EndSend();

}
void CRegionGWSession::SendChat(int receiverID, const char* senderName, char channel, const char* content, const char* title/*=""*/,long oldId)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_chat);
	cmd.WriteLong(receiverID);
	cmd.WriteString("");
	cmd.WriteString((char*)senderName);
	cmd.WriteByte(channel);
	cmd.WriteString((char*)content);
	cmd.WriteString((char*)title);
	cmd.WriteLong(oldId);

	EndSend();
}

bool CRegionGWSession::SendMail(int receiverID, int senderID, const string& senderName, const string& title, const string& content,const string& StrItemID,long Money)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_add_mail);
	cmd.WriteLong(senderID);
	cmd.WriteLong(receiverID);
	cmd.WriteString((char*)senderName.c_str());
	cmd.WriteString((char*)title.c_str());
	cmd.WriteString((char*)content.c_str());
	cmd.WriteString((char*)StrItemID.c_str());
	cmd.WriteLong(Money);

	return EndSend();
}

void CRegionGWSession::SendUserWhenFriendName(long vOtherDBID,long userId)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_when_fiend);
	cmd.WriteLong(vOtherDBID);
	cmd.WriteLong(userId);
	EndSend();
}

void CRegionGWSession::SendUserIdbyName(const char* name,long senderID)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_fiend_name_ID);
	cmd.WriteString((char*)name);
	cmd.WriteLong(senderID);
	EndSend();
}

void CRegionGWSession::SendUserIdbyNameToGw(long UserID,long SenderID,byte Type)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_one_refresh);
	cmd.WriteLong(UserID);
	cmd.WriteLong(SenderID);
	cmd.WriteByte(Type);
	EndSend();
}

void CRegionGWSession::SendPakeBlackListName(long SenderID,long userId,short type)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_PakeBlack_List);
	cmd.WriteLong(SenderID);
	cmd.WriteLong(userId);
	cmd.WriteShort(type);
	EndSend();
}

void CRegionGWSession::SendBlackListName(const char* Name,long UserID)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_Black_List);
	cmd.WriteString((char*)Name);
	cmd.WriteLong(UserID);
	EndSend();
}

bool CRegionGWSession::SendMailItem(long MailID)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_MailItem);
	cmd.WriteLong(MailID);

	return EndSend();
}

bool  CRegionGWSession::SendIsOpenMail(short IsOpen,long MailID)
{
	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_Is_Open);
	cmd.WriteShort(IsOpen);
	cmd.WriteLong(MailID);

	return EndSend();
}

bool CRegionGWSession::DeleteMail(long MailID)
{
	CG_CmdPacket& cmd = BeginSend();

	cmd.WriteShort(r2g_remove_mail);
	cmd.WriteLong(MailID);

	return EndSend();
}

bool CRegionGWSession::Logout(long userID, bool pending, CRegionUser* user)
{
	m_unloginUsers.erase(userID);

    if (GetState() == NET_STATE_CONNECTED)
	{
		CG_CmdPacket& cmd  = BeginSend();

		cmd.WriteShort(r2g_logout);
		cmd.WriteLong(userID);

		if (!EndSend())
		{
			if (pending)
            {
				PushPendingLogoutUser(userID, user);
            }else
            {
                if (user && user->m_dummy)
                {
                    ERR1("CRegionGWSession::Logout Failed [%s]\n", user->m_dummy->m_core.Name.c_str());
                }else
                {
                    ERR("CRegionGWSession::Logout Failed [Unknown]\n");
                }
				return false;
            }
		}
	}
	else if (pending)
	{
		PushPendingLogoutUser(userID, user);
	}

	return true;
}


SUserData* CRegionGWSession::LookupUser(long userID)
{
	CRegionUnloginUserTable::iterator iter = m_unloginUsers.find(userID);

	if (iter == m_unloginUsers.end())
		return NULL;

	return &iter->second;
}

void CRegionGWSession::SwitchRegion(CRegionUser* user)
{
	if (user->m_dummy && user->m_dummy->m_pPShop)
	{
		user->m_dummy->StopPShop();
	}

	if (user->m_dummy && user->m_dummy->m_pet)
	{
		user->m_dummy->CallbackPet(true,false);		
		user->m_dummy->RemoveWardNPC();
	}
	if(user->m_dummy && user->m_dummy->m_hero)
	{
		user->m_dummy->CallbackHero();
	}
	g_region->m_gws.SaveUserData(user->m_userId, user->m_dummy);

	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_switch_region);
	cmd.WriteLong(user->m_userId);
	cmd.WriteLong(user->m_switchRegionSeed);
	EndSend();

	g_region->KickUser(user->m_userId, 0, false, false,"switch region");
}

void CRegionGWSession::BeginSwitchRegion(CRegionUser* user)
{
	SRegionInfo* info = &m_regions[user->m_regionId];

    user->m_bSwitchRegionState = true;
	user->m_switchRegionSeed = rtGetMilliseconds();
	if(g_cfg.game.bDebugMsg)
    {
        LOG3("Switch Region Command [UserID=%d, Host=%s, Port=%d]\n", user->m_userId, info->host.c_str(), info->port);
	    user->m_dummy->SendSystemMessage("调试信息 - 切换服务器...");
    }

	// Remove member item by louis
	int page = 0, i = 0, j = 0;
	if (user->m_dummy->m_pItemContainer->m_Bag.FindType(4401, ItemColor_All, NULL, &page, &i, &j))
	{
		user->m_dummy->m_pItemContainer->m_Bag.RemoveItem(page, i, j);
		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j, ITEM_REASON_NULL);
		user->RecvCmd(g_sendCmd);
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_switch_region);
	g_sendCmd->WriteLong(user->m_userId);
	g_sendCmd->WriteLong(user->m_switchRegionSeed);
	g_sendCmd->WriteString((char*)info->host.c_str());
	g_sendCmd->WriteShort(info->port);
	user->RecvCmd(g_sendCmd);
}

extern G_MEMDEF(g_szExeMD5, 40)
extern G_MEMDEF(g_szDataMD5, 40)
//extern char g_szExeMD5[40], g_szDataMD5[40];

bool CRegionGWSession::LicenseCheck(int nType)
{
	ReadMd5();

	//std::string	strIP;
	char	sIP[16];
	char	sMacAddr[6];
	std::string strmac;

#ifdef LINUX
	if(GetIPMacAddr((unsigned char*)sMacAddr, sIP) != 0)
	{
		LOG("LINUX Get IP/Mac Error\n");
		return false;
	}
#else
	// C [11/22/2008 Louis.Huang]
/*
	RtSystemInfo sysInfo;
	if (sysInfo.Init())
	{
		SSystemInfo* info = sysInfo.GetSystemInfo();
		memcpy((void*)sIP, info->sIpAddr, sizeof(info->sIpAddr));
		//strIP = info->sIpAddr;
		memcpy(sMacAddr, info->sMacAddr, sizeof(sMacAddr));		// sMacAddr为8个字节（最后2个为0），而m_sMacAddress为6个字节
	}
	else
	{
		memcpy((void*)sIP, "defaultIP", strlen("defaultIP"));
		memset(sMacAddr, 0, 6);
	}
*/
#endif //LINUX

	for(int i = 0; i < 6; i++)
	{
		strmac += rtFormatNumber((unsigned char)sMacAddr[i]);
	}

	std::string ipAddr = sIP;

	CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_license_check);
	cmd.WriteString((char*)ipAddr.c_str());
	cmd.WriteString((char*)strmac.c_str());
	cmd.WriteString((char*)m_strMd5.c_str());
	cmd.WriteLong((long)nType);
	EndSend();

	return true;
}

bool CRegionGWSession::ReadMd5()
{
	FILE*	fp = NULL;
	char	strmd5[33];

	fp = fopen("license.dat", "r");

	if(!fp)
	{
		LOG("File license.txt open error\n");
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

void CRegionGWSession::OnConnect(bool ret)
{
	if (ret)
	{
		LOG1("Connect GWS succeeded. at %d\n", rtTimestamp());
		SetBufSize(1024*5);
		GetSocket().SetSendBufferSize(256*1024);

		CG_CmdPacket& cmd = BeginSend();
		cmd.WriteShort(r2g_check_version);
        cmd.WriteString(g_szExeMD5);
        cmd.WriteString(g_szDataMD5);
        cmd.WriteString(OS_TYPE_STR);
		EndSend();
	}
	else
    {
        MSG1("Connect GWS FAILED, %d\n", rtTimestamp());
        m_disconnectTimeStamp = time(NULL);
    }
}

void CRegionGWSession::OnDisconnect()
{
	ERR1("Disconnect from GameWorld server,reboot now. %d\n", rtTimestamp());
	// LOG1("There are %d unlogin users have been clean\n", m_unloginUsers.size());
	m_unloginUsers.clear();
	m_disconnectTimeStamp = time(NULL);
	g_region->Reboot();
}

void CRegionGWSession::OnReceive(CG_CmdPacket* packet)
{
	short cmd;

	if (!packet->ReadShort(&cmd))
	{
		ERR("Read command error from GW server\n");
		return;
	}

	m_channel.RecvCmd(cmd, packet);
}

bool CRegionGWSession::Connect()
{
	return true;
}

CRegionGWSession::CRegionGWSession()
    : m_disconnectTimeStamp(0), m_seed(0)
{
    this->SetDebugStr("GWSession");
	m_lastProcessTime = 0;
}

void CRegionGWSession::PushPendingLogoutUser(int userID, CRegionUser* user)
{
	for (list<int>::iterator iter = m_pendingLogoutUsers.begin(); iter != m_pendingLogoutUsers.end(); iter++)
    {
		if (*iter == userID)
        {
			return;
        }
    }
    if (user && user->m_dummy)
    {
        LOG1("CRegionGWSession::PushPendingLogoutUser [%s]\n", user->m_dummy->m_core.Name.c_str());
    }else
    {
        LOG("CRegionGWSession::PushPendingLogoutUser [Unknown]\n");
    }
	m_pendingLogoutUsers.push_back(userID);
}

void CRegionGWSession::PushPendingSaveUser(int userID, CRegionCreature* cre)
{
	CRegionPendingUserList::iterator user = m_pendingSaveUsers.begin();
	for(; user != m_pendingSaveUsers.end(); user++)
	{
		if (user->userID == userID)
		{
			break;
		}
	}

	if (user == m_pendingSaveUsers.end())
	{
		m_pendingSaveUsers.push_front(SUserData());
		user = m_pendingSaveUsers.begin();
	}

	cre->Save(*user);
}

bool CRegionGWSession::CommitPendingData()
{
	LOG2("Commit pending %d save user data, %d logout data\n", 
		m_pendingSaveUsers.size(),
		m_pendingLogoutUsers.size());

	list<SUserData>::iterator pendingUser = m_pendingSaveUsers.begin();
	while(pendingUser != m_pendingSaveUsers.end())
	{
		if (!SaveUserData(*pendingUser))
			return false;

		pendingUser = m_pendingSaveUsers.erase(pendingUser);
	}

	list<int>::iterator pendingLogout = m_pendingLogoutUsers.begin();
	while(pendingLogout != m_pendingLogoutUsers.end())
	{
		if (!Logout(*pendingLogout, false))
			return false;

		pendingLogout = m_pendingLogoutUsers.erase(pendingLogout);
	}
    return true;
}

void CRegionGWSession::UpdateRegionState()
{
    CG_CmdPacket& cmd = BeginSend();
    cmd.WriteShort(r2g_update_region_state);
    g_region->m_stat.Serialize(&cmd, SERVER_STAT_VERSION);
    EndSend();
}

void CRegionGWSession::BroadcastCmdToAllRegion(const char* szCmd)
{
    CG_CmdPacket& cmd = BeginSend();
    cmd.WriteShort(r2g_broadcast_to_region);
    cmd.WriteString((char*)szCmd);
    EndSend();
}

void CRegionGWSession::BroadcastBulletin(const char* szMessage, char bSysMsg, DWORD dwColor, DWORD dwGMID)
{
    CG_CmdPacket& cmd = BeginSend();
    cmd.WriteShort(r2g_broadcast_bulletin);
    cmd.WriteLong((long)dwGMID);
    cmd.WriteString((char*)szMessage);
    cmd.WriteLong((long)dwColor);
    cmd.WriteByte(bSysMsg);
    EndSend();
}

void CRegionGWSession::LockAccount(long m_userId ,short sLockFlag, long lLockSecond)
{
	 CG_CmdPacket& cmd = BeginSend();
	cmd.WriteShort(r2g_lock_account);
	cmd.WriteLong(m_userId);
	cmd.WriteShort(sLockFlag);
	cmd.WriteLong(lLockSecond);
	EndSend();
}

void CRegionGWSession::MoveUserToPosition(long userId, char const* userName, int x, int y)
{
	CG_CmdPacket& packet = BeginSend();
	packet.WriteShort(r2g_gm_move);
	packet.WriteLong(userId);
	packet.WriteByte(userName ? 5 : 2); // moveuserto
	if(userName)
		packet.WriteString(userName);
	packet.WriteLong(x);
	packet.WriteLong(y);
	EndSend();
	
	//Tianh LOG到数据库
	CG_CmdPacket& packe = g_region->m_gws.BeginSend();
	packe = g_region->m_gws.BeginSend();
	packe.WriteShort(r2g_log_event);
	packe.WriteByte(PLT_MoveTo);
	packe.WriteLong(userId);
	packe.WriteString(""); // userIP 
	packe.WriteLong(userId);
	if(userName)
	{
	    packe.WriteString(userName);
	}
	else
	{
		packe.WriteString("");
	}
	packe.WriteLong(x);
	packe.WriteLong(y);
	g_region->m_gws.EndSend();
}
void CRegionGWSession::MoveUserAround(long userId, char const* userName, int range)
{
	CG_CmdPacket& packet = BeginSend();
	packet.WriteShort(r2g_gm_move);
	packet.WriteLong(userId);
	packet.WriteByte(4); // movearound
	packet.WriteString(userName);
	packet.WriteShort(range);
	EndSend();
}

void CRegionGWSession::MoveUser(char cmd, long userId, char const* userName)
{
	CG_CmdPacket& packet = BeginSend();
	packet.WriteShort(r2g_gm_move);
	packet.WriteLong(userId);
	packet.WriteByte(cmd); // moveuser
	packet.WriteString(userName);
	EndSend();
}
