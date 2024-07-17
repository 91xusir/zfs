#include "region.h"
#include "rs_dungeon.h"

int CRsDungeonMgr::s_iMaxLastQuitTime = 300;

CRsDungeonMgr::CRsDungeonMgr()
{
	CM_MEMPROTECTOR(m_szMsg, 256)
    m_iMaxDungeon = 50;
}

CRsDungeonMgr::~CRsDungeonMgr()
{
	CM_MEMUNPROTECTOR(m_szMsg)
	if (m_listDungeon.size() > 0)
	{
		SRsDungeon* pDungeon;
		list<SRsDungeon*>::iterator itDungeon = m_listDungeon.begin();
		for (; itDungeon!=m_listDungeon.end(); itDungeon++)
		{
			pDungeon = *itDungeon;
			DEL_ONE(pDungeon);
		}
	}
	m_listDungeon.clear();
	m_listRunTrigger.clear();
}

bool CRsDungeonMgr::Init()
{
	return true;
}

bool CRsDungeonMgr::Exit()
{
	return true;
}

void CRsDungeonMgr::Run()
{
    time_t tNow = time(NULL);
    static time_t tLastTime = 0;
    if (tLastTime==tNow) return;
    tLastTime = tNow;

    SRsDungeon* pDungeon;
    list<SRsDungeon*>::iterator itDungeon = m_listDungeon.begin();
    for (; itDungeon!=m_listDungeon.end(); itDungeon++)
    {
        pDungeon = *itDungeon;
        if (pDungeon->tLimitTime!=0 && tNow>pDungeon->tLimitTime)
        {
            // 强制退出
#ifdef _DEBUG
            LOG1("检查到副本[%d]的极限时间已经到达，准备退出副本\n", pDungeon->lDungeonUID);
#endif
	        CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	        cmd.WriteShort(r2g_destroy_dungeon);
            cmd.WriteLong(pDungeon->lDungeonUID);
            cmd.WriteLong(60); // 设置玩家退出，并且60秒后关闭副本
	        g_region->m_gws.EndSend();
            pDungeon->tLimitTime = 0;
        }
        if (pDungeon->tEndTime!=0 && tNow>pDungeon->tEndTime)
        {
#ifdef _DEBUG
            LOG1("检查到副本[%d]的退出时间已经到达，强制退出副本\n", pDungeon->lDungeonUID);
#endif
            // 全部的用户退出一段时间，现在超时退出
	        CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	        cmd.WriteShort(r2g_destroy_dungeon);
	        cmd.WriteLong(pDungeon->lDungeonUID);
            cmd.WriteLong(0); // 关闭副本
	        g_region->m_gws.EndSend();
        }
    }
	StartTriggerRun();
}

bool CRsDungeonMgr::CreateDungeon(int tableId,int uId,int fbDif,int fbUserLV, int iExitPosX, int iExitPosY)
{
	if(fbDif < 0 || fbDif >= MAX_FB_LEVEL)
    {
        return false;
    }

	SFuBen *info = g_TableFB.Find(tableId);
	if(!info)
    {
		return false;
    }

    // 信息
	SRsDungeon *fb = RT_NEW SRsDungeon;
    fb->lDungeonTID = tableId;
    fb->lDungeonUID = uId;
	fb->info = info;
	fb->process = 0;

    // 时间
    fb->tCreateTime = time(NULL);
    fb->tLimitTime = fb->tCreateTime+info->timeLimitSecond; // 5小时(应改为由表读入)
    fb->tEndTime = 0;

    // 场景
	fb->scene = (CRegionSceneFB*)g_factory->CreateObject(OBJECT_TYPE_SCENE_FB);
	fb->scene->SetTimer(SCENE_TIMER_ITEM, 30*1000);
	fb->scene->SetSceneID(info->id);					// Set fb scene id louis
	//FB等级值被策划要求==FB里用户平均等级
	fb->scene->m_curLev = fbUserLV;//info->reqLev[fbDif];
	fb->scene->m_curDif = fbDif;
	fb->scene->m_curUserLV = fbUserLV;
	fb->scene->m_lExitPos[0] = iExitPosX;
	fb->scene->m_lExitPos[1] = iExitPosY;
	fb->scene->m_lExitPos[2] = 0;
	fb->scene->m_info = info;
	fb->scene->LoadSceneFile(info->sceneFile.c_str());
	// Still load script to create npc [8/27/2009 Louis.Huang]
	fb->scene->LoadScript(info->script.c_str());
	fb->scene->OnCreate();
	fb->scene->m_defPos[0] = fb->info->bornPos[0];
	fb->scene->m_defPos[1] = fb->info->bornPos[1];

	// 加入机关
	int id;
	long pos[2];
	int dir;
	for (int i=1;i<=g_TableFB.m_trigger.size();i++)
	{
		if (g_TableFB.m_trigger[i].fbID == tableId)
		{
			STrigger trigger = g_TableFB.m_trigger[i];

			if (trigger.active == 0)
				continue;

			trigger.curUseNum = 0;
			if (trigger.close)
				trigger.triggerCurNum = 0;
			else
				trigger.triggerCurNum = trigger.triggerNum;
			if (trigger.UseTime < 5000)
				trigger.UseTime = 5000;

			// 添加触发物件
			for (int j=0;j<trigger.triggerNum;j++)
			{
				id = trigger.trigger[j];
				pos[0] = trigger.triggerPos[j].x;
				pos[1] = trigger.triggerPos[j].y;
				dir = trigger.triggerDir[j];
				if ( id )
				{
					CRegionCreature *cre = g_region->CreateNpc(id);
					if( cre )
					{
						trigger.triggerUID[j] = cre->GetObjectId();
						cre->triggerID = trigger.id;
						cre->triggerIsClose = trigger.close;
						cre->triggerOpen = !cre->triggerIsClose;
						fb->scene->AddCreature(cre, pos, dir);
					}
				}
			}
			// 添加表现物件
			for (int j=0;j<trigger.opendoorNum;j++)
			{
				id = trigger.opendoor[j];
				pos[0] = trigger.opendoorPos[j].x;
				pos[1] = trigger.opendoorPos[j].y;
				dir = trigger.opendoorDir[j];
				if ( id )
				{
					CRegionCreature *cre = g_region->CreateNpc(id);
					if( cre )
					{
						if (trigger.opendoorMain == j)
						{
							cre->triggerMain = 1;
							trigger.opendoorMainUID = cre->GetObjectId();
						}
						trigger.opendoorUID[j] = cre->GetObjectId();
						cre->triggerID = trigger.id;
						cre->triggerIsClose = trigger.close;
						cre->triggerOpen = !cre->triggerIsClose;
						fb->scene->AddCreature(cre, pos, dir);
					}
				}
			}
			// 添加阻挡物件
			if (trigger.close && trigger.type == 3)
			{
				for (int j=0;j<trigger.blockCount;j++)
				{
					id = trigger.block[j];
					pos[0] = trigger.blockPos[j].x;
					pos[1] = trigger.blockPos[j].y;
					dir = trigger.blockDir[j];
					if ( id )
					{
						CRegionCreature *cre = g_region->CreateNpc(id);
						if( cre )
						{
							trigger.blockUID[j] = cre->GetObjectId();
							cre->triggerID = trigger.id;
							cre->triggerIsClose = trigger.close;
							cre->triggerOpen = !cre->triggerIsClose;
							fb->scene->AddCreature(cre, pos, dir);
						}
					}
				}
			}
			else if (trigger.type == 1 || trigger.type == 2)
			{
				SetTriggerBlock(fb,trigger);
			}
			fb->trigger.push_back(trigger);
		}
	}
	fb->scene->m_info = info;
	fb->info = info;

    // 加入列表
	m_listDungeon.push_back(fb);
	ERR("Dungeon: Total dungeon number is: %d\n", m_listDungeon.size());

    // 通知
	// notify gameworld server
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_create_dungeon_ret);
	cmd.WriteLong(uId);
	cmd.WriteLong(1);
	g_region->m_gws.EndSend();

	return true;
}

void CRsDungeonMgr::SetTriggerBlock(SRsDungeon *fb, STrigger trigger)
{
	if (fb->scene && fb->scene->m_pTerrain)
	{
		RtsSceneDoc* pTerrain = fb->scene->m_pTerrain;
		if (trigger.close)
		{
			int x,y;
			for (int i=0;i<trigger.blockCount;++i)
			{
				x = trigger.blockPos[i].x / 20;
				y = trigger.blockPos[i].y / 20;
				if ( !pTerrain->SetTerrainAttr(x, y, 1) )
				{
					ERR2("trigger block add error; triggerID : %d,blockPos: %d,%d\n",trigger.id,trigger.blockPos[i].x,trigger.blockPos[i].y);
				}
			}
		}else
		{
			int x,y;
			for (int i=0;i<trigger.blockCount;++i)
			{
				x = trigger.blockPos[i].x / 20;
				y = trigger.blockPos[i].y / 20;
				if ( !pTerrain->RemoveTerrainAttr(x, y, 1) )
				{
					ERR2("trigger block remove error; triggerID : %d,blockPos: %d,%d\n",trigger.id,trigger.blockPos[i].x,trigger.blockPos[i].y);
				}
			}
		}
	}
}

CRsDungeonMgr::SRsDungeon *CRsDungeonMgr::FindDungeon(int uId)
{
	list<SRsDungeon*>::iterator it;
	for(it=m_listDungeon.begin(); it!=m_listDungeon.end(); it++)
	{
		SRsDungeon *tmp = *it;
        if(tmp->lDungeonUID == uId)
			return tmp;
	}
	return NULL;
}

const char* CRsDungeonMgr::RequestEnter(CRegionUser *user,int fbId,int fbDif,int iUserLV,int iExitX, int iExitY)
{
	if(g_RegionCastleWarMgr.IsCastleWarTime("XiQi")//tim.yang  城战期间不允许进入副本
	|| g_RegionCastleWarMgr.IsCastleWarTime("ZhaoGe"))
	{
		return R(MSG_TASK_CANNOT_TELEPORT_INCASTLEWARTIME);
	}
	SFuBen *pDungeonInfo = g_TableFB.Find(fbId);
    if (user==NULL)
    {
        return "";
    }
    if (user->m_dummy==NULL)
    {
		return R(MSG_DUNGEON_CANNOT_ENTER);
    }
	if(pDungeonInfo==NULL)
    {
		return R(MSG_DUNGEON_NOT_EXIST);
    }

  //  if (user->privileges&EUP_Developer)
  //  {
		//rt2_sprintf(m_szMsg, R(MSG_DUNGEON_WELCOME), user->m_dummy->m_core.Name.c_str());
  //      user->m_dummy->SendSystemMessage(m_szMsg);
  //  }else
    {
   //     if (user->m_dummy->m_pTeam->mData.Empty())
   //     {
   //         // 必须组成队伍才能进入副本
			//return R(MSG_DUNGEON_TEAM_MUST);
   //     }else
        {
    //        if (user->m_userId!=user->m_dummy->m_pTeam->mData.GetCaptain())
    //        {
				//return R(MSG_DUNGEON_CAPTAIN_MUST);
    //        }

			// Captain item test
			if (!user->m_dummy->m_pItemContainer->m_Bag.FindType(pDungeonInfo->reqLeaderItem) &&
					!user->m_dummy->m_pItemContainer->m_MissionBag.FindType(pDungeonInfo->reqLeaderItem))
			{
				rt2_sprintf(m_szMsg, R(MSG_DUNGEON_ITEM_MUST), user->m_dummy->m_core.Name.c_str());
				return m_szMsg;
			}

            long tNow = (long)time(NULL);
            CRegionUser *pTeamMember;
            int num = user->m_dummy->m_pTeam->mData.Size();
	        for(int i=0; i<num; i++)
	        {
		        pTeamMember = g_region->FindUser(user->m_dummy->m_pTeam->mData[i].mDBID);
		        if(pTeamMember==NULL || pTeamMember->m_dummy==NULL)
                {
					continue;
     //               // 在另外一个Region中，这里认为他距离过远
					//rt2_sprintf(m_szMsg, R(MSG_DUNGEON_FAR), user->m_dummy->m_pTeam->mData[i].mName.c_str());
     //               return m_szMsg;
                }

				// Team member item test
				if (!pTeamMember->m_dummy->m_pItemContainer->m_Bag.FindType(pDungeonInfo->reqMemberItem) &&
						!pTeamMember->m_dummy->m_pItemContainer->m_MissionBag.FindType(pDungeonInfo->reqMemberItem))
				{
					rt2_sprintf(m_szMsg, R(MSG_DUNGEON_ITEM_MUST), pTeamMember->m_dummy->m_core.Name.c_str());
					return m_szMsg;
				}

		   //     // 距离 限制
		   //     if(pTeamMember->m_dummy->Distance(user->m_dummy->m_pos) > 800)
     //           {
     //               // 距离过远不能进入副本
					//rt2_sprintf(m_szMsg, R(MSG_DUNGEON_FAR), pTeamMember->m_dummy->m_core.Name.c_str());
     //               return m_szMsg;
     //           }

                // 等级 限制
				// LeiJun 不在根据难度限制进入等级，直接根据拥护等级调整怪物实力
     //           if (pTeamMember->m_dummy->m_core.Lev<pDungeonInfo->reqLev[fbDif])
     //           {
					//rt2_sprintf(m_szMsg, R(MSG_DUNGEON_LEVEL_MUST), pTeamMember->m_dummy->m_core.Name.c_str(), pDungeonInfo->reqLev[fbDif]);
     //               return m_szMsg;
     //           }

                // 红名 限制
                if (pTeamMember->m_dummy->GetPKState() == PK_STATE_RED_NAME)
                {
					rt2_sprintf(m_szMsg, R(MSG_DUNGEON_RED_NAME), pTeamMember->m_dummy->m_core.Name.c_str());
                    return m_szMsg;
                }

    //            // 阵营 限制
    //            switch (pDungeonInfo->faction)
    //            {
    //            case 0:
    //                if (pTeamMember->m_dummy->m_core.Faction!=FACTION_ZHOU)
    //                {
				//		rt2_sprintf(m_szMsg, R(MSG_DUNGEON_FACTION_ZHOU_MUST), pTeamMember->m_dummy->m_core.Name.c_str());
    //                    return m_szMsg;
    //                }
    //                break;
    //            case 1:
    //                if (pTeamMember->m_dummy->m_core.Faction!=FACTION_SHANG)
    //                {
				//		rt2_sprintf(m_szMsg, R(MSG_DUNGEON_FACTION_SHANG_MUST), pTeamMember->m_dummy->m_core.Name.c_str());
    //                    return m_szMsg;
    //                }
    //                break;
    //            default:
    //            case 2:
    //                break;
				//}

                // 可进入的副本难度 限制
				/*
                SFbTaskInfo *pFbTaskInfo = pTeamMember->m_dummy->m_task.GetFbTaskInfo(fbId);
                if (pFbTaskInfo)
                {
                    if(pFbTaskInfo->curDif < fbDif)
                    {
                        rt2_sprintf(m_szMsg, "不能进入副本，因为 [%s] 没有完成上一难度的任务", user->m_dummy->m_pTeam->mData[i].mName.c_str());
                        return m_szMsg;
                    }
                }else
                {
                    rt2_sprintf(m_szMsg, "不能进入副本，因为 [%s] 没有完成上一难度的任务", user->m_dummy->m_pTeam->mData[i].mName.c_str());
                    return m_szMsg;
                }
				*/

                if (pTeamMember->m_dummy->m_dungeonLastQuit > 0)
                {
                    if (tNow<pTeamMember->m_dummy->m_dungeonLastQuit+s_iMaxLastQuitTime)
                    {
						rt2_sprintf(m_szMsg, R(MSG_DUNGEON_TIME_MUST), user->m_dummy->m_pTeam->mData[i].mName.c_str(), s_iMaxLastQuitTime/60);
                        int iDiffTime = pTeamMember->m_dummy->m_dungeonLastQuit+s_iMaxLastQuitTime-tNow;
                        int nLen = strlen(m_szMsg);
						if (iDiffTime > 60)
                        {
							rt2_snprintf(m_szMsg + nLen, 256 - nLen, R(MSG_DUNGEON_TIME_MORE), iDiffTime/60, iDiffTime%60);
                        }else
                        {
							rt2_snprintf(m_szMsg + nLen, 256 - nLen, R(MSG_DUNGEON_TIME_LESS), iDiffTime);
                        }
                        return m_szMsg;
                    }
                }
	        }
        }
    }

	//if (user->m_dummy->m_dwGroupID || user->m_dummy->m_dwLineID)
	//{
	//	long pos[2] = { 0, 0 };
	//	CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(user->m_dummy->m_dwGroupID, user->m_dummy->m_dwLineID, user->m_dummy->m_userInfo->m_userId, "", pos);
	//}

	// send request to gameworld server
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_enter_dungeon);
	cmd.WriteLong(user->m_userId);
	cmd.WriteLong(fbId);
	cmd.WriteLong(fbDif);
	cmd.WriteLong(iUserLV);
	cmd.WriteLong(iExitX);
	cmd.WriteLong(iExitY);
	g_region->m_gws.EndSend();
	return NULL;
}

bool CRsDungeonMgr::RequestLeave(CRegionUser *user)
{
    if (user==NULL || user->m_dummy==NULL)
    {
        return false;
    }
	// send request to gameworld server
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_leave_dungeon);
	cmd.WriteLong(user->m_userId);
	g_region->m_gws.EndSend();
    return true;
}

bool CRsDungeonMgr::OnDungeonUserLeave(CRegionUser *user)
{
    if (user==NULL || user->m_dummy==NULL)
    {
        return false; // 如果玩家由于某种原因下线有可能导致这边返回。这样的话玩家就没有退出
    }

	// 避免重复调用本函数
	// FB的UID==0是正常的，所以用小于0来判断
	if(user->m_dummy->m_dungeonUID<0)
	{
		ERR("leave dungeon but dungeon id = null\n");
		return false;
	}

	SRsDungeon *pDungeon = FindDungeon(user->m_dummy->m_dungeonUID);
	SFuBen *pDungeonInfo = g_TableFB.Find(user->m_dummy->m_dungeonTID);

	if (!pDungeon)
	{
		return false;
	}

	SFuBen *info = pDungeon->info;

    user->m_dummy->m_dungeonUID = 0;
	if(user->m_dummy->m_scene == pDungeon->scene)
		user->m_dummy->m_dungeonLastQuit = (long)time(NULL);

    if (pDungeonInfo==NULL)
    {
        return false;
    }

    if (pDungeon)
    {
        if(pDungeon->scene)
        {
			if(pDungeon->scene->IsScriptLoaded())
				pDungeon->scene->CallScript("OnUserLeave", false, user->m_dummy);
	        pDungeon->scene->RemoveCreature(user->m_dummy->m_oId);
        }
    }

	if(user->m_dummy->m_scene)
	{
		ERR("leave dungeon scene not null\n");
		user->m_dummy->m_scene->RemoveCreature(user->m_dummy->m_oId);
	}

	if(pDungeon)
	{
		if(!g_region->m_defScene->AddCreature(user->m_dummy, pDungeon->scene->m_lExitPos, 0))
			return false;
		user->m_dummy->ForceSetPosition(pDungeon->scene->m_lExitPos[0], pDungeon->scene->m_lExitPos[1], pDungeon->scene->m_lExitPos[2]);
	}
	else
	{
		if(!g_region->m_defScene->AddCreature(user->m_dummy, pDungeon ? pDungeon->scene->m_lExitPos : (long*)pDungeonInfo->exitPos, 0))
		{
			return false;
		}
		user->m_dummy->ForceSetPosition(pDungeonInfo->exitPos[0], pDungeonInfo->exitPos[1], pDungeonInfo->exitPos[2]);
	}

    if (pDungeon && pDungeon->scene)
    {
        pDungeon->listUser.remove(user->m_userId);
        if (pDungeon->listUser.size()==0)
        {
            //pDungeon->tEndTime = pDungeon->tCreateTime+pDungeon->info->timeOutSecond;
        }
		//if((int)pDungeon->listUser.size()<pDungeon->info->minChr)
		//	pDungeon->tEndTime = pDungeon->tCreateTime+pDungeon->info->timeOutSecond;
    }

	if(user->m_dummy->m_bIsFBReborn)//timyang 神灯  玩家登陆前在副本中死亡，而且此时使用了神灯召唤monster，从副本出来后，新添加两个怪物
	{
		user->m_dummy->GodLightCreateMonster_AddMonster(user->m_dummy,user->m_dummy->m_monsterCount);
		user->m_dummy->m_bIsFBReborn = 0;
	}
	user->m_dummy->LeaveDungeon();
    return true;
}

void CRsDungeonMgr::DestroyDungeon(int uId, int iExitSecond)
{
	SRsDungeon *pDungeon = FindDungeon(uId);
	if(!pDungeon)
		return;
	SFuBen *info = pDungeon->info;

#ifdef _DEBUG
    LOG2("收到副本[%d]退出命令，退出时间为 %d\n", uId, iExitSecond);
#endif

    if (iExitSecond==0)
    {
		BroadcastSystemMessage(pDungeon, R(MSG_DUNGEON_TIME_FOR_QUIT));
    }else
    {
		BroadcastSystemMessage(pDungeon, R(MSG_DUNGEON_MAX_TIME));
    }

    // 传送玩家离开副本
	CRegionUser *user;
    std::list<DWORD>::iterator it = pDungeon->listUser.begin();
    for (; it!=pDungeon->listUser.end(); it++)
    {
        user = g_region->FindUser(*it);
        if (user)
        {
			RequestLeave(user);
			user->m_dummy->LeaveDungeon();
        }
    }

	// 删除机关
	for (int i=0;i<pDungeon->trigger.size();i++)
	{
		STrigger &trigger = pDungeon->trigger[i];

		for (int j=0;j<trigger.triggerNum;j++)
		{
			if(pDungeon->scene)
			{
				CRegionCreature *cre = pDungeon->scene->FindCreature(trigger.triggerUID[j]);
				pDungeon->scene->RemoveCreature(trigger.triggerUID[j]);
				g_factory->DestroyObject(cre);
			}
		}
		for (int j=0;j<trigger.opendoorNum;j++)
		{
			if(pDungeon->scene)
			{
				CRegionCreature *cre = pDungeon->scene->FindCreature(trigger.opendoorUID[j]);
				pDungeon->scene->RemoveCreature(trigger.opendoorUID[j]);
				g_factory->DestroyObject(cre);
			}
		}
		if (trigger.type == 3)
		{
			for (int j=0;j<trigger.blockCount;j++)
			{
				if(pDungeon->scene)
				{
					CRegionCreature *cre = pDungeon->scene->FindCreature(trigger.blockUID[j]);
					pDungeon->scene->RemoveCreature(trigger.blockUID[j]);
					g_factory->DestroyObject(cre);
				}
			}
		}
	}
	pDungeon->trigger.clear();

    if (iExitSecond==0 && pDungeon->scene->m_userHash.size()==0)
    {
		pDungeon->scene->Exit();
	    m_listDungeon.remove(pDungeon);
	    g_factory->DestroyObject(pDungeon->scene);
    }else
    {
        if (iExitSecond==0) iExitSecond = 60;
		long thisTime = time(NULL);
        pDungeon->tEndTime = thisTime + iExitSecond;
    }

	std::list<RunTrigger>::iterator iter = m_listRunTrigger.begin();
	for (;iter!=m_listRunTrigger.end();)
	{
		if ((*iter).pDungeon == pDungeon)
		{
			iter = m_listRunTrigger.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CRsDungeonMgr::BroadcastSystemMessage(SRsDungeon *pDungeon, const char* szMessage)
{
    if (pDungeon==NULL || szMessage==NULL)
    {
        return;
    }

    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_chat);
    g_sendCmd->WriteByte(CHAT_CHANNEL_SYSTEM);
    g_sendCmd->WriteLong(0);
	g_sendCmd->WriteString((char*)R(MSG_DUNGEON));
    g_sendCmd->WriteString((char*)szMessage);

    CRegionUser *user;
    std::list<DWORD>::iterator it = pDungeon->listUser.begin();
    for (; it!=pDungeon->listUser.end(); it++)
    {
        user = g_region->FindUser(*it);
        if (user)
        {
            if (user->m_dummy)
            {
                user->m_dummy->RecvCmd(g_sendCmd);
            }
        }
    }
}

long CRsDungeonMgr::GetDungeonIDByUserID(DWORD dwID)
{
	for(list<SRsDungeon*>::iterator itDun=m_listDungeon.begin();
		itDun!=m_listDungeon.end(); itDun++)
	{
		SRsDungeon *pDungeon = *itDun;
		for(std::list<DWORD>::iterator it = pDungeon->listUser.begin();
			it!=pDungeon->listUser.end(); it++)
		{
			if(dwID == *it)
				return pDungeon->lDungeonUID;
		}
	}

	return -1;
}

long CRsDungeonMgr::GetDungeonIDBySceneID(long lSceneID)
{
	for(list<SRsDungeon*>::iterator itDun=m_listDungeon.begin();
		itDun!=m_listDungeon.end(); itDun++)
	{
		SRsDungeon *pDungeon = *itDun;
		if(pDungeon->scene->GetObjectId() == lSceneID)
			return pDungeon->lDungeonUID;
	}

	return -1;
}

bool CRsDungeonMgr::AddUserToDungeon(int uId, DWORD userId, char cBorn, long x, long y)
{
	CRegionUser *user = g_region->FindUser(userId);
	if(!user) return false;

	long pos[3];
	pos[0] = user->m_dummy->m_pos[0];
	pos[1] = user->m_dummy->m_pos[1];
	pos[2] = user->m_dummy->m_pos[2];

	_AddUserToDungeon(uId,user,cBorn,x,y);
	return true;
}

bool CRsDungeonMgr::_AddUserToDungeon(int uId, CRegionUser *user, char cBorn, long x, long y)
{
	SRsDungeon *pDungeon = FindDungeon(uId);
	if(!pDungeon)
    {
		LOG("Region: Add user to dungeon error! find dungeon error\n");
        return false;
    }
    if (user==NULL || user->m_dummy==NULL)
    {
        return false;
    }

	if (user->m_dummy->m_dungeonUID != uId)
	{
		user->m_dummy->m_nFBprocess = 0;
	}
    user->m_dummy->m_dungeonUID = uId;
    user->m_dummy->m_dungeonTID = pDungeon->lDungeonTID;
    user->m_dummy->m_dungeonLastQuit = 0;

	// Remove the dungeon require item here Louis @ Thu 30 Apr 2009
	if (user->m_dummy->m_pTeam)
	{
		if (user->m_userId == user->m_dummy->m_pTeam->mData.GetCaptain())
			user->m_dummy->RemoveItem(pDungeon->info->reqLeaderItem, 1);
		else
			user->m_dummy->RemoveItem(pDungeon->info->reqMemberItem, 1);
	}

	long pos[3];
    if (cBorn)
    {
	    pos[0] = pDungeon->info->bornPos[0];
	    pos[1] = pDungeon->info->bornPos[1];
		pos[2] = pDungeon->info->bornPos[2];
    }else
    {
	    pos[0] = x;
	    pos[1] = y;
	    pos[2] = pDungeon->info->bornPos[2];
    }

	if(user->m_dummy->m_scene)
    {
		user->m_dummy->m_scene->RemoveCreature(user->m_dummy->m_oId);
    }
	
	if(!pDungeon->scene->AddCreature(user->m_dummy,pos,0))
    {
		ERR2("Region: Add user to dungeon error! pos error[%d][%d]\n",pos[0],pos[1]);
		return false;
    }

	// notify client
	user->m_dummy->ForceSetPosition(pos[0],pos[1],pos[2]); 

	std::list<DWORD>::iterator it = pDungeon->listUser.begin();
	for (; it!=pDungeon->listUser.end(); )
	{
		if (*it == user->m_userId)
		{
			it = pDungeon->listUser.erase(it);
		}
		else
		{
			++it;
		}
	}
	pDungeon->listUser.push_back(user->m_userId);
	//用户进入FB场景脚本事件
	if(pDungeon->scene->IsScriptLoaded())
	{
		pDungeon->scene->CallScript("OnUserEnter", false, user->m_dummy);
	}

    if (pDungeon->info->minChr!=0)
    {
        if ((int)pDungeon->listUser.size()>=pDungeon->info->minChr)
        {
            pDungeon->tEndTime = 0;
        }
    }else
    {
        if ((int)pDungeon->listUser.size()>0)
        {
            pDungeon->tEndTime = 0;
        }
	}
	user->m_dummy->EnterDungeon();

	return true;
}

void CRsDungeonMgr::DungeonLeaveByItem(CRegionUser *user)
{
	SRsDungeon *pDungeon = FindDungeon(user->m_dummy->m_dungeonUID);
	SFuBen *pDungeonInfo = g_TableFB.Find(user->m_dummy->m_dungeonTID);
    user->m_dummy->ForceSetPosition(pDungeon->info->bornPos[0],pDungeon->info->bornPos[1],pDungeon->info->bornPos[2]);
}


CRegionUser *CRsDungeonMgr::RemoveUser(CRegionUser *user)
{
	return NULL;
}


void CRsDungeonMgr::StartTrigger(SRsDungeon *fb, DWORD triggerId, bool open/* = true*/)
{
	if (!fb || !fb->info)
		return;
	STrigger *pTrigger = fb->GetTrigger(triggerId);

	if (!pTrigger)
		return;

	if (fb && triggerId)
	{
		long time;
		if (pTrigger->close)
		{
			time = 1;
		}
		else
		{
			time = pTrigger->StartTime;
		}
		m_listRunTrigger.push_back(RunTrigger(fb,triggerId,time));
	}
}

void CRsDungeonMgr::StartTrigger(DWORD fbId, DWORD triggerId, bool open /* = true */)
{
	SRsDungeon *fb = FindDungeon(fbId);
	STrigger *pTrigger = fb->GetTrigger(triggerId);

	if (!pTrigger)
		return;

	if (fb && triggerId)
	{
		long time;
		if (pTrigger->close)
		{
			time = 1;
		}
		else
		{
			time = pTrigger->StartTime;
		}
		m_listRunTrigger.push_back(RunTrigger(fb,triggerId,time));
	}
}

void CRsDungeonMgr::StartTriggerRun()
{
	if (m_listRunTrigger.empty())
		return;
	std::list<RunTrigger>::iterator it = m_listRunTrigger.begin();
	for (;it!=m_listRunTrigger.end();)
	{
		if ((*it).pDungeon)
		{
			if ((*it).dwTime < rtGetMilliseconds())
			{
				CRegionUser *user;
				std::list<DWORD>::iterator iter = (*it).pDungeon->listUser.begin();
				long changeBlock = 1;
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_start_trigger);
				g_sendCmd->WriteShort((*it).dwTrigger);
				g_sendCmd->SerializeBit(changeBlock,1);
				for (; iter!=(*it).pDungeon->listUser.end(); iter++)
				{
					user = g_region->FindUser(*iter);
					if (user && user->m_dummy)
					{
						user->m_dummy->RecvCmd(g_sendCmd);
					}
				}
				if ((*it).pDungeon)
				{
					STrigger *pTrigger = (*it).pDungeon->GetTrigger((*it).dwTrigger);
					if (pTrigger)
					{
						SetTriggerBlock((*it).pDungeon,*pTrigger);
					}
				}
				it = m_listRunTrigger.erase(it);
			}
			else
			{
				++it;
			}
		}
		else
		{
			it = m_listRunTrigger.erase(it);
		}
	}
}
