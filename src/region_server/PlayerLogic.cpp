#include "PlayerLogic.h"
#include <stdlib.h>
#include <iostream> 
#include <strstream>
#include <sstream>
#include <fstream>
#include <time.h>
#include "region_scene.h"
#include <fstream>
#include <string>

bool ReadRegionConfigFile(const char *szFilename);
extern char* g_strStaticBuffer;
const int c_g_Min = 60000;
unsigned long CPlayerLogic::s_Time[c_nNewNum] = {5 * c_g_Min, 15 * c_g_Min, 30 * c_g_Min, 
50 * c_g_Min, 70 * c_g_Min, 95 * c_g_Min, 120 * c_g_Min};
CPlayerLogic::CPlayerLogic(CRegionCreature& rPlayer):m_rPlayer(rPlayer), m_dwFireTime(0),
m_pCurUseSkill(NULL), m_enForTaskType(eNo), m_LiveType(eLive), m_pMonsterLogicExt(NULL), m_dwGetingSkinID(eEmptyDBID)
,m_dwGetingMedicineID(eEmptyDBID), m_dwGetingMineID(eEmptyDBID), m_dwEndUsingTaskItemTime(0), 
m_pUsingItem(NULL), m_nCurMainStory(0), m_dwNextOverHeadTime(0), m_dwOnLineTime(0xffffffff), m_dwEnterTime(0), 
m_NewPlayerGuide(NULL), m_nNewW(c_nNewNum), m_dwCanUseYaoPingHpTime(0),m_dwCanUseYaoPingMpTime(0),
m_dwCanUseYaoPingPetHpTime(0),m_dwCanUseYaoPingPetMpTime(0),m_dwCanUseYaoPingEnergyTime(0)
{
	SetMiss(false);
	SetTeamFree(true);
	if(!m_rPlayer.IsUser())
	{
		if(GetMonsterLogicExt()->IsMineNPC() || GetMonsterLogicExt()->IsMedicineNPC())
		{
			m_LiveType = eHaveSkin;
			GetMonsterLogicExt()->DropMineMedicineS();
		}
	}
	else
	{
		m_rPlayer.m_core.SetIHpMpChange(GetCNewPlayerGuide());
	}
}

CPlayerLogic::~CPlayerLogic()
{
	DEL_ONE(m_pMonsterLogicExt)
		DEL_ONE(m_NewPlayerGuide)
}

CNewPlayerGuide* CPlayerLogic::GetCNewPlayerGuide()
{
	if(NULL == m_NewPlayerGuide)
	{
		m_NewPlayerGuide = RT_NEW CNewPlayerGuide(m_rPlayer);
	}
	return m_NewPlayerGuide;
}

SAttackParam& CPlayerLogic::GetSAttackParam()
{
	return m_SAttackParam;
}

unsigned long CPlayerLogic::GetFireTime()
{
	return m_dwFireTime;
}

bool CPlayerLogic::IsIntonating()
{
	return 0 != m_dwFireTime;
}

void  CPlayerLogic::StopIntonate()
{
	m_dwFireTime = 0;
}

int  CPlayerLogic::StartIntonate(const SAttackParam& Param)
{
	int nRet = 1;
	do
	{
		SSkill* pSkill = Skill()->FindSkill(Param.Skill);
		if(NULL == pSkill)
		{
			break;
		}
		if(IsIntonating())
		{
			break;
		}

		m_SAttackParam = Param;
		unsigned long dwTime = pSkill->iIntonateTime - 100;
		//dwTime = (unsigned long)(dwTime * 0.9f);
		m_dwFireTime = rtGetMilliseconds();
		m_dwFireTime += dwTime;
		if(IsMiss())
		{
			SetMiss(false);
		}
		nRet = 0;
	}while(false);
	return nRet;
}

bool CPlayerLogic::CanStopSomeOnceIntonating(SSkill* pSkill, CRegionCreature& rPlayer)
{
	bool bRet = false;
	do
	{
		if(NULL == pSkill)
		{
			break;
		}
		if(!rPlayer.GetPlayerLogic()->IsIntonating())
		{
			break;
		}
		SSkill* pOtherSkill = Skill()->FindSkill(rPlayer.GetPlayerLogic()->GetSAttackParam().Skill);
		if(NULL == pOtherSkill)
		{
			break;
		}
		int nR = pSkill->iBreakOdds + pOtherSkill->iBreakOdds;
		srand(rtGetMilliseconds());   ///< 随机种子
		int nNum =rand() % 100;    
		bRet = nNum < nR;
	}while(false);
	return bRet;
}

void CPlayerLogic::SetCurUseSkill(SSkill* pCurUseSkill)
{
	m_pCurUseSkill = pCurUseSkill;
}

SSkill* CPlayerLogic::GetCurUseSkill()
{
	return m_pCurUseSkill;
}

bool	CPlayerLogic::IsMiss()
{
	return m_bMiss;
}

void	CPlayerLogic::SetMiss(bool bMiss)
{
	m_bMiss = bMiss;
}

bool CPlayerLogic::IsTeamFree()
{
	return true;//m_bTeamFree;
}

void CPlayerLogic::SetTeamFree(bool bTeamFree)
{
	m_bTeamFree = bTeamFree;
}

CPlayerLogic::InviteMap& CPlayerLogic::GetInviteMap()
{
	return m_InviteMap;
}

void  CPlayerLogic::AddtoInviteMap(unsigned long dwDBID)
{
	InviteMap& im = CPlayerLogic::GetInviteMap();
	im.insert(make_pair(dwDBID, rtGetMilliseconds() + 60000));
}

void  CPlayerLogic::RemoveFromInviteMap(unsigned long dwDBID)
{
	InviteMap& im = CPlayerLogic::GetInviteMap();
	InviteMap::iterator iter = im.find(dwDBID);
	if(im.end() != iter)
	{
		im.erase(iter);
	}
}

bool  CPlayerLogic::IsInInviteMap(unsigned long dwDBID)
{
	InviteMap& im = CPlayerLogic::GetInviteMap();
	InviteMap::iterator iter = im.find(dwDBID);
	return im.end() != iter;
}

void CPlayerLogic::OnRun(unsigned long dwTime)
{
	InviteMap& im = CPlayerLogic::GetInviteMap();
	InviteMap::iterator iter;
	for(iter = im.begin(); im.end() != iter; iter++)
	{
		if(dwTime >= (*iter).second)
		{
			im.erase(iter);
			break;
		}
	}
	if(IsUsingTaskItem())
	{
		if(dwTime >= m_dwEndUsingTaskItemTime)
		{
			EndUsingTaskItem();
		}
	}
	OnRunNewPlayer(dwTime);
	/*if(m_rPlayer.IsMonster())
	{
	GetMonsterLogicExt()->OnRun(dwTime);
	}*/
}

void CPlayerLogic::OnGetItem(SItemID item, EItemContainer c, int page, int i, int j)
{
	CRegionCreature *cre = &m_rPlayer;
	cre->m_nGetItemCount ++;
	int DropUserID = 0;

	CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, c, page, i, j, ITEM_REASON_PICK);
	cre->RecvCmd(g_sendCmd);

	// 如果在队伍里面，通知其他的队伍成员
	if (cre->m_pTeam)
	{
		for(int index=cre->m_pTeam->mData.Begin(); index!=cre->m_pTeam->mData.End(); index++)
		{
			if (cre->m_pTeam->mData[index].mDBID == cre->m_userInfo->m_userId)
				continue;

			CRegionUser *pUser = g_region->FindUser(cre->m_pTeam->mData[index].mDBID);
			if (pUser && pUser->m_dummy)
			{
				CItemCmdBuilder_Svr::Build_r2c_other_additem(cre->m_core.Name.c_str(), item, ITEM_REASON_PICK);
				pUser->RecvCmd(g_sendCmd);
			}
		}
	}

	if (ItemIsRare(item, g_region->m_pItemManager))
	{
		// log到数据库
		string strItem = rtFormatNumber(GetItemSerializeVersion());
		strItem += ",";
		strItem += item.SaveToString(g_region->m_pItemManager);

		CG_CmdPacket& packet = g_region->m_gws.BeginSend();
		packet.WriteShort(r2g_log_event);
		packet.WriteByte(PLT_PickItem);
		packet.WriteLong(cre->m_userInfo->m_userId);
		packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str()); // userIP
		packet.WriteString((char*)strItem.c_str());	// strParam1
		packet.WriteLong(item.color);
		packet.WriteLong(DropUserID);
		g_region->m_gws.EndSend();
	}
}

long  CPlayerLogic::CanFinishState(STaskDesc *task, CRegionCreature* npc)
{
	// check if doing task
	if(m_rPlayer.IsDoingTask(task, npc))
	{
		// check if can auto finish task
		if(task->finishType == TASK_FINISH_TYPE_NORMAL)
		{
			if(m_rPlayer.CanFinishTask(task,0, npc))
			{
				//FinishTask(task,0,npc);
				return TASK_STATE_CAN_FINISH;
			}
		}
		return TASK_STATE_DOING; 
	}

	return TASK_STATE_NONE;
}


void  CPlayerLogic::SharedTask(long nTaskID)
{
	STaskInfo *info = m_rPlayer.m_task.FindTask(nTaskID);
	char cRet = 0;
	const int c_cTagNum = 10;
	char cTag[c_cTagNum];
	char cTagNum = 0;
	//int i = 0;
	const int c_nBuffSize = 256;
	char szBuff[c_nBuffSize];
	do
	{
		if(NULL == info)
		{
			cRet = 1;
			break;
		}
		STaskDesc *pDesc = info->GetDesc();
		if(NULL == pDesc)
		{
			cRet = 1;
			break;
		}
		if (!m_rPlayer.IsUser())
		{
			cRet = 1;
			break;
		}
		if(NULL == m_rPlayer.m_pTeam)
		{
			cRet = 2;
			break;
		}
		if(!pDesc->m_bShare)
		{
			cRet = 3;
			break;
		}
		CTeamData &td = m_rPlayer.m_pTeam->mData;
		int nNum = td.Size();
		if (0 == nNum)
		{
			cRet = 2;
			break;
		}
		STeamData* pData = td.GetTeamData();
		int i;
		
		for(i = 0; i < nNum; i++)
		{
			if (pData[i].mDBID == m_rPlayer.m_userInfo->m_userId)
			{
				continue;
			}
			CRegionUser* pUser = g_region->FindUser(pData[i].mDBID);
			if(NULL == pUser)
			{
				continue;
			}
			CRegionCreature *tmp = pUser->m_dummy;
			if(NULL == tmp)
			{
				continue;
			}
			bool bA = IsAroundMe(tmp);
			if (!bA)
			{
				if (cTagNum < c_cTagNum)
				{
					cTag[cTagNum++] = 1;//距离过远
				}
				rt2_snprintf(szBuff, c_nBuffSize, R(MSG_TASK_OUTRANGE), tmp->m_core.Name.c_str());
				m_rPlayer.SendSystemMessage(szBuff);
				continue;
			}
			long nRet = tmp->CheckTaskAcceptState(pDesc, NULL);
			if(TASK_STATE_CAN_ACCEPT != nRet)
			{
				if (cTagNum < c_cTagNum)
				{
					cTag[cTagNum++] = 2;//不符合条件
				}
				rt2_snprintf(szBuff, c_nBuffSize, R(MSG_TASK_MATECANNOTACCEPT), tmp->m_core.Name.c_str(), pDesc->name.c_str());
				m_rPlayer.SendSystemMessage(szBuff);
				continue;
			}
			if (cTagNum < c_cTagNum)
			{
				cTag[cTagNum++] = 0;//OK
			}
			CTaskInfo &cinfo = tmp->m_task;
			STaskInfo *task = cinfo.FindTask(nTaskID);
			if(NULL == task)
			{
				cinfo.AddTask(nTaskID);
			}
			task = cinfo.FindTask(nTaskID);
			if(NULL == task)
			{
				continue;
			}
			task->FSetShared();
			rt2_snprintf(szBuff, c_nBuffSize, R(MSG_TASK_SHARE), pDesc->name.c_str() , tmp->m_core.Name.c_str());
			m_rPlayer.SendSystemMessage(szBuff);
			
			rt2_snprintf(szBuff, c_nBuffSize, R(MSG_TASK_MATESHARE), tmp->m_core.Name.c_str(), pDesc->name.c_str());
			tmp->SendSystemMessage(szBuff);

			tmp->UpdateToClient(NET_CRE_UPDATE_TASK);
		}
	}while(false);
	switch (cRet)
	{
	/*case 0:
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_shared_task_ret);
		g_sendCmd->WriteByte(cTagNum);
		for (i = 0; i < cTagNum; i++)
		{
			g_sendCmd->WriteByte(cTag[i]);
		}
		m_rPlayer.RecvCmd(g_sendCmd);
		break;*/
	case 2:
		m_rPlayer.SendSystemMessage(R(MSG_TASK_NOTEAM));
		break;
	default:
		break;
	}
}

void CPlayerLogic::AcceptSharedTask(long nTaskID)
{
	char cRet = 0;
	do
	{
		STaskDesc *desc = g_TableTask.FindTask(nTaskID);
		if(NULL == desc)
		{
			cRet = 3;
			break;
		}
		CTaskInfo &info = m_rPlayer.m_task;
		STaskInfo *task = info.FindTask(nTaskID);
		if(NULL == task)
		{
			cRet = 4;
			break;
		}
		if(!task->FIsShared())
		{
			cRet = 4;
			break;
		}
		if(info.GetTaskAcceptNum() >= MAX_TASK_SELECT_NUM)
		{
			cRet = 1;
			m_rPlayer.SendSystemMessage(R(MSG_TASK_GETTASKFAIL));
			break;
		}
		long nRet = m_rPlayer.CheckTaskAcceptState(desc, NULL);
		if(TASK_STATE_CAN_ACCEPT != nRet)
		{
			cRet = 2;
			m_rPlayer.SendSystemMessage(R(MSG_TASK_TASKNOTMATCHCONDITION));
			break;
		}
		bool bAccepted = m_rPlayer.AcceptTask(desc, NULL);
		if(!bAccepted)
		{
			cRet = 2;
			break;
		}
		m_rPlayer.CanFinishTask(desc, 0, NULL);
		cRet = 0;
	}while(false);

	/*switch (cRet)
	{
	case 0:
	case 1:
	case 2:
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_accept_shared_task_ret);
		g_sendCmd->WriteByte(cRet);
		m_rPlayer.RecvCmd(g_sendCmd);
		break;
	default:
		break;
	}*/
}

int	CPlayerLogic::AllMyTeamAddTaskItem(CRegionCreature* pDieNPC, long kk)
{
	int nRet = 1;
	do
	{
		if(NULL == pDieNPC)
		{
			break;
		}
		if(NULL == pDieNPC->m_npcInfo)
		{
			break;
		}
		if(NULL == m_rPlayer.m_pTeam)
		{
			break;
		}

		if(m_rPlayer.m_pTeam->mData.Empty())
		{
			break;
		}

		CTeamData &td = m_rPlayer.m_pTeam->mData;
		int nNum = td.Size();
		int i;
		STeamData* pData = td.GetTeamData();
		for(i = 0; i < nNum; i++)
		{
			CRegionUser* pUser = g_region->FindUser(pData[i].mDBID);
			if(NULL == pUser)
			{
				continue;
			}
			CRegionCreature *tmp = pUser->m_dummy;
			if(NULL == tmp)
			{
				continue;
			}
			//CRegionCreature *tmp2 = m_rPlayer.m_scene->FindCreature(tmp->GetObjectId());
			if(NULL == tmp/* || tmp != tmp2*/)
			{
				continue;
			}
			if(tmp->Distance(pDieNPC->m_pos) > 800)
			{
				continue;
			}
			
			if(/*pDieNPC->m_npcInfo->FbTask==0 &&*/
				pDieNPC->m_npcInfo->TaskId[kk] && 
				tmp->m_task.IsTaskAccept(pDieNPC->m_npcInfo->TaskId[kk]))
			{
				if(tmp->GetItemNum(pDieNPC->m_npcInfo->TaskItemId[kk], ItemColor_White) < pDieNPC->m_npcInfo->TaskItemLimit[kk])
				{
					tmp->AddItem(pDieNPC->m_npcInfo->TaskItemId[kk], ItemColor_White);
					STaskInfo* info = tmp->m_task.FindTask(pDieNPC->m_npcInfo->TaskId[kk]);
					if (NULL != info)
					{
						STaskDesc *pDesc = info->GetDesc();
						if (NULL != pDesc)
						{
							tmp->CanFinishTask(pDesc, 0, NULL);
						}
					}

					//tmp->UpdateToClient(NET_UPDATE_ALL);
					tmp->UpdateToClient(NET_CRE_UPDATE_TASK);
				}
			}
		}
		nRet = 0;
	}while(false);
	return nRet;
}

CPlayerLogic::ForTaskType CPlayerLogic::GetForTaskType()
{
	return m_enForTaskType;
}

void CPlayerLogic::SetForTaskType(ForTaskType enType)
{
	m_enForTaskType = enType;
}

bool CPlayerLogic::IsTaskDone(int nID)
{
	bool bRet = false;
	do
	{
		/*STaskInfo *info = m_rPlayer.m_task.FindTask(nID);
		if(NULL == info)
		{
			break;
		}
		if(!info->FIsDone())
		{
			break;
		}
		bRet = true;*/
		bRet = m_rPlayer.m_task.IsDoneTask(nID);
	}while(false);
	return bRet;
}

int CPlayerLogic::ProcessTaskKey(STaskKey &key, int op, vector<long> *param, STaskDesc *pDesc)
{
	const int c_nBuffSize = 200;
	char szBuff[c_nBuffSize];

	int nRet = -1;
	do
	{
		if(key.key == "taskdone")
		{

			if(TYPE_CHECK != op)
			{
				nRet = 1;
				break;
			}
			int nTaskID = atoi(key.param[0].c_str());
			if(!IsTaskDone(nTaskID))
			{
				nRet = 2;
				break;
			}
		}
		else if (key.key == "JobRecord")
		{
			if (TYPE_ADD == op)
			{
				long num;
				num = atol(key.param[0].c_str());
				m_rPlayer.m_core.AddJobRecord(num);
				rt2_sprintf(szBuff, R(MSG_GET_JobRecord), num);
				m_rPlayer.SendSystemMessage(szBuff);
			}
			else
			{
				nRet = 1;
				break;
			}
		}
		else if(key.key == "useitem")
		{
			if(NULL == pDesc)
			{
				nRet = 2;
				break;
			}
			STaskInfo *info = m_rPlayer.m_task.FindTask(pDesc->id);
			if(NULL == info)
			{
				nRet = 2;
				break;
			}
			int i, n;
			n = (int)(pDesc->taskReq.size());
			bool bB = false;
			for(i = 0; i < n; i++)
			{
				if(!info->IsUseItem((char)(i)))
				{
					bB = true;
					break;
				}
			}
			if(bB)
			{
				nRet = 2;
				break;
			}
		}
		else if (key.key == "mfree")
		{
			long num;
			num = atol(key.param[0].c_str());
			if(num == 0)
			{
				nRet = 2;
				break;
			}

			if (NULL == m_rPlayer.m_pItemContainer)
			{
				nRet = 1;
				break;
			}
			if(m_rPlayer.m_pItemContainer->m_MissionBag.FreeSpace() < num)
			{
				nRet = 2;
				break;
			}
		}
		else if(key.key == "credit")
		{
			bool bB = true;
			int num;
			int nType = atol(key.param[0].c_str());
			switch (nType)
			{
			case 0://帮贡
				num = atol(key.param[1].c_str());
				if (TYPE_ADD == op)
				{
					AddUnionRecord(num);
					rt2_sprintf(szBuff, R(MSG_GET_UnionRecord), num);
					m_rPlayer.SendSystemMessage(szBuff);
					nRet = 0;
				}
				else
				{
					nRet = 2;
				}
				break;
			case 1://俗名
				num = atol(key.param[1].c_str());
				if (TYPE_ADD == op)
				{
					rt2_sprintf(szBuff, R(MSG_GET_Contribute), num);
					m_rPlayer.SendSystemMessage(szBuff);
				}

				nRet = 2;
				break;
			case 2://灵力
				if (TYPE_ADD == op)
				{
					num = atol(key.param[1].c_str());
					m_rPlayer.m_core.AddSoul(num);
					rt2_sprintf(szBuff, R(MSG_GET_Soul), num);
					m_rPlayer.SendBattleInfoMessage(szBuff);

					bB = false;
				}
				break;
			case 3://修为
				num = atol(key.param[1].c_str());

				if (TYPE_ADD == op)
				{
					m_rPlayer.AddCulExp(num);
					//rt2_sprintf(szBuff, R(MSG_GET_Cul), num);
					//m_rPlayer.SendSystemMessage(szBuff);
				}
				nRet = 2;
				break;
			case 4://声望
				{
					int nCreditType = atol(key.param[1].c_str());
					num = atol(key.param[2].c_str());
					switch (nCreditType)
					{
					case 0://仙誉
						rt2_sprintf(szBuff, R(MSG_GET_Honors), num);
						break;
					case 1://蜀郡官府声望
						rt2_sprintf(szBuff, R(MSG_GET_AreaHonors1), num);
						break;
					case 2://发羌蛮族声望
						rt2_sprintf(szBuff, R(MSG_GET_AreaHonors2), num);
						break;
					case 3://镇妖义士声望
						rt2_sprintf(szBuff, R(MSG_GET_AreaHonors3), num);
						break;
					case 4://反隋义军声望
						rt2_sprintf(szBuff, R(MSG_GET_AreaHonors4), num);
						break;
					case 5://仙魔联盟声望
						rt2_sprintf(szBuff, R(MSG_GET_AreaHonors5), num);
						break;
					default:
						break;
					}
					if (TYPE_ADD == op)
					{
						m_rPlayer.SendSystemMessage(szBuff);
					}
				}

				nRet = 2;
				break;
			default:
				nRet = 2;
				break;
			}
			if (bB)
			{
				break;
			}
		}
		else if(key.key == "have")
		{
			nRet = 2;
			if (TYPE_CHECK != op)
			{
				break;
			}

			int nType = atol(key.param[0].c_str());
			int nNum;

			bool bB = true;
			switch (nType)
			{
			case 0://宠物召唤
				{
					short nID = (short)(atol(key.param[1].c_str()));
					if (NULL == m_rPlayer.m_pet)
					{
						nRet = 1;
						break;
					}
					if (nID != m_rPlayer.m_pet->m_npcId)
					{
						break;
					}
					bB = false;
				}
				break;
			case 1://生活技能
				{
					nNum = atol(key.param[1].c_str());
					int nSkillType = atol(key.param[2].c_str());
					CSkillInstance* pS =  m_rPlayer.m_Skill.FirstActiveSkill();
					int nS = 0;
					while(NULL != pS)
					{
						if (NULL != pS->m_pAttr)
						{
							if (nSkillType == pS->m_pAttr->wClass)
							{
								nS++;
							}
						}
						pS = m_rPlayer.m_Skill.NextActiveSkill();
					}
					if (nS < nNum)
					{
						break;
					}
					bB = false;
				}
				break;
			case 2://好友
				if (NULL == m_rPlayer.m_pFriend)
				{
					nRet = 1;
					break;
				}
				nNum = atol(key.param[1].c_str());
				if (nNum > m_rPlayer.m_pFriend->Size(SConnection::FRIEND))
				{
					break;
				}
				bB = false;
				break;
			case 3://法宝炼化
				{
					SItemID	itemID;
					int nID = atol(key.param[1].c_str());
					nNum = atol(key.param[2].c_str());
					bool bFind = false;


					CItemContainerBase *pCon = m_rPlayer.m_pItemContainer;
					if (NULL == pCon)
					{
						nRet = 1;
						break;
					}
					CBag *pBag =NULL;
					pBag = &(pCon->m_Bag);
					if (NULL == pBag)
					{
						nRet = 1;
						break;
					}
					int		nPage	= pBag->GetPageCount();
					int		nLine	 = pBag->GetLineCount();
					int		nCol	 = pBag->GetColCount();
					//int nCun = 0;

					int i, j, k;
					for(i = 0; i < nPage && !bFind; i++)
					{
						for (j = 0; j < nLine && !bFind; j++)
						{
							for(k = 0; k < nCol && !bFind; k++)
							{
								itemID = pBag->GetItem(i, j, k);
								if (!ItemID_IsValid(itemID))
								{
									continue;
								}
								if (!ItemIsTrump(itemID))
								{
									continue;
								}
								if(CItemManager::GetIndexFromType(itemID.type) != nID)
								{
									continue;
								}
								if (itemID.params[TRUMP_LEV] < (unsigned long)(nNum))
								{
									continue;
								}
								bFind = true;
							}
						}
					}

					if (bFind)
					{
						nRet = 0;
						bB = true;
						break;
					}
					itemID = m_rPlayer.m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);
					bB = true;
					if (!ItemID_IsValid(itemID))
					{
						break;
					}
					if(CItemManager::GetIndexFromType(itemID.type) != nID)
					{
						break;
					}
					if (itemID.params[TRUMP_LEV] < (unsigned long)(nNum))
					{
						break;
					}
					bB = false;
				}
				break;
			}
			if (bB)
			{
				break;
			}
		}
		else if(key.key == "gongreward")
		{
			int GradeLev = m_rPlayer.m_core.GradeLev;
			float fLev = GradeLev / 20.0f + 1;
			int nR = 1;
			if (m_rPlayer.m_gongTaskFinish < 20)
			{
				nR = 3;
			}
			int nFinish = m_rPlayer.m_gongTaskFinish % 20 + 1;
			//奖励门贡=INT((玩家修为等级/20+1)*(1+0.05*((玩家修为等级/20+1)-1))*1*(MOD((当前任务个数-1),20)+1))*(IF(当前任务个数<=20,3,1))
			float fAdd = (fLev*(1 + 0.05f * (fLev - 1)) * nFinish)*nR;
			int nAdd = (int)(fAdd);
			AddJobRecord(nAdd);
			if (19 == m_rPlayer.m_gongTaskFinish)
			{
				if(GetProb(1)) m_rPlayer.AddItem(6222, ItemColor_White);
				if(GetProb(0.01)) m_rPlayer.AddItem(6221, ItemColor_Blue);
			}
			//GradeLev--;
			//奖励经验=INT((60*6*(20+5*(玩家修为等级-1)*(1+0.05*(玩家修为等级-1)))/210)*(MOD((当前任务个数-1),20)+1))
			//fAdd = (60 * 6 * (20 + 5 * GradeLev * (1 + 0.05f * GradeLev)) / 210) * nFinish;
			//nAdd = (int)(fAdd);
			//m_rPlayer.AddExp(nAdd, 0);
			//奖励金钱=INT((0.3*60*(20+5*(玩家修为等级-1)*(1+0.05*(玩家修为等级-1)))/210*(MOD((当前任务个数-1),20)+1)))
			//fAdd = 0.3 * 60 * (20 + 5 * GradeLev * (1 + 0.05 * GradeLev)) / 210 * nFinish;
			//nAdd = (int)(fAdd);
			//m_rPlayer.AddGold(nAdd);

			m_rPlayer.m_gongTaskFinish++;

			//int GradeLev = m_rPlayer.m_core.GradeLev;
			////int((玩家修为等级/20+1)*(1+0.05*((玩家修为等级/20+1)-1))*1*(mod((任务次数-1),20)+1)*3)
			//float fLev = GradeLev / 20.0f + 1;
			//int nAdd = (int)((fLev*(1 + 0.05f * (fLev - 1))*((m_rPlayer.m_wenTaskFinish - 1) % 20 + 1))*3);
			//AddJobRecord(nAdd);
		}
		else if(key.key == "moureward")
		{
			float fGradeLev = m_rPlayer.m_core.Lev / 20.0f;
			float fLev = fGradeLev + 1;
			//谋生任务奖励俗名值=int((玩家等级/20+1)*(1+0.05*((玩家等级/20+1)-1))*1*(任务次数)
			float fAdd = fLev * (1 + 0.5f * fGradeLev) * (m_rPlayer.m_MouTaskFinish + 1);
			int nAdd = (int)(fAdd);
			AddContribute(nAdd);
			fAdd *= 50;
			nAdd = (int)(fAdd);
			AddGold(nAdd);
			if (19 == m_rPlayer.m_MouTaskFinish)
			{
				if(GetProb(0.5))
				{
					m_rPlayer.AddItem(6500, ItemColor_White);
				}
				else
				{
					m_rPlayer.AddItem(6501, ItemColor_White);
				}
			}
			m_rPlayer.m_MouTaskFinish++;
		}
		else if (key.key == "killmonster")
		{
			if(NULL == pDesc)
			{
				nRet = 2;
				break;
			}
			STaskInfo *info = m_rPlayer.m_task.FindTask(pDesc->id);
			if(NULL == info)
			{
				nRet = 2;
				break;
			}
			unsigned long dwMaxKill = atol(key.param[0].c_str());
			if (info->GetCurKillMonsterNum() < dwMaxKill)
			{
				nRet = 2;
			}
			else
			{
				nRet = 0;
			}
			break;
		}
		else if (key.key == "day")
		{
			if(TYPE_CHECK != op)
			{
				nRet = 1;
				break;
			}
			unsigned long nType = atol(key.param[0].c_str());
			time_t tNow = time(NULL);
			struct tm* pTm = localtime(&tNow);
			int nDayType = 0;
			switch (pTm->tm_wday)
			{
			case 2:
			case 4:
			case 6:
				nDayType = 0;
				break;
			case 3:
			case 5:
			case 1:
				nDayType = 2;
				break;
			default:
				nDayType = 1;
				break;
			}
			if (nType == nDayType)
			{
				nRet = 0;
				break;
			}
			nRet = 2;
			break;
		}
		else if (key.key == "gradelev")
		{
			unsigned long nLev = atol(key.param[0].c_str());
			if(TYPE_CHECK != op)
			{
				nRet = 1;
				break;
			}

			if(m_rPlayer.m_core.GradeLev > nLev)
			{
				nRet = 0;
				break;
			}
			nRet = 2;
			break;
		}
		else if (key.key == "inunion")
		{
			if(TYPE_CHECK != op)
			{
				nRet = 1;
				break;
			}
			if(0 != m_rPlayer.m_unionID)
			{
				nRet = 0;
				break;
			}
			nRet = 2;
			break;
		}
		else
		{
			nRet = -1;
			break;
		}
		nRet = 0;
	}while(false);
	return nRet;
}

void CPlayerLogic::OnPetChange()
{
	bool bUpdate = false;
	do 
	{
		STaskInfo *info = m_rPlayer.m_task.FindTask(11001);
		if(NULL == info)
		{
			break;
		}
		if(!info->FIsDoing())
		{
			break;
		}

		if (NULL == m_rPlayer.m_pet)
		{
			if (info->FIsCanFinish())
			{
				info->FSetDoing();
				bUpdate = true;
			}
			break;
		}

		if (1800 == m_rPlayer.m_pet->m_npcId)
		{
			if (!info->FIsCanFinish())
			{
				info->FSetCanFinish();
				bUpdate = true;
			}
			break;
		}
	} while (false);
	if (bUpdate)
	{
		m_rPlayer.UpdateToClient(NET_CRE_UPDATE_TASK);
	}
}

void CPlayerLogic::OnTrumpLevUp(const SItemID &idS, int nNewLev)
{
	do 
	{
		STaskDesc *desc = g_TableTask.FindTask(11005);
		if(NULL == desc)
		{
			break;
		}
		m_rPlayer.CanFinishTask(desc, 0, NULL);

	} while (false);
}

void CPlayerLogic::OnSkillNumChange()
{
	do 
	{
		STaskDesc *desc = g_TableTask.FindTask(11006);
		if(NULL == desc)
		{
			break;
		}
		m_rPlayer.CanFinishTask(desc, 0, NULL);

	} while (false);
}

void CPlayerLogic::OnFriendNumChange()
{
	do 
	{

		STaskDesc *desc = g_TableTask.FindTask(11007);
		if(NULL == desc)
		{
			break;
		}
		m_rPlayer.CanFinishTask(desc, 0, NULL);

	} while (false);
}

void CPlayerLogic::OnInsert(unsigned short int vConnection, DWORD vDBID)
{
	do 
	{
		unsigned short int nT = vConnection & SConnection::FRIEND;
		if (0 == nT)
		{
			break;
		}
		OnFriendNumChange();
	} while (false);
}

void CPlayerLogic::OnErase(unsigned short int vConnection, DWORD vDBID)
{
	do 
	{
		unsigned short int nT = vConnection & SConnection::FRIEND;
		if (0 == nT)
		{
			break;
		}
		OnFriendNumChange();
	} while (false);
}

CPlayerLogic::LiveType CPlayerLogic::GetLiveType()
{
	return m_LiveType;
}

void CPlayerLogic::SetLiveType(LiveType type)
{
	m_LiveType = type;
}

bool CPlayerLogic::IsLive()
{
	return eLive == m_LiveType;
}

void CPlayerLogic::Die()
{
	m_LiveType = eHaveItem;
	if(IsGetingSkin())
	{
		CaneclGetingSkin();
		SendGetSkinError(3);
	}
	else if (IsGetingMine())
	{
		CaneclGetingMine();
		SendGetSkinError(3);
	}
	else if (IsGetingMedicine())
	{
		CaneclGetingMedicine();
		SendGetSkinError(3);
	}
}

void CPlayerLogic::SendGetSkinError(char cErr)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_pickitem_type);
	g_sendCmd->WriteByte(2);//扒皮
	g_sendCmd->WriteByte(cErr);

	SItemID item;
	CMonsterLogicExt* Teaget = NULL;
	if(this->IsGetingSkin())
	{
		Teaget = GetGetingSkin();
		if (NULL != Teaget)
		{
			item = Teaget->m_SkinItem;
		}
	}
	else 
	{

		if(this->IsGetingMine())
		{
			Teaget = GetGetingMine();
		}
		else if(this->IsGetingMedicine())
		{
			Teaget = GetGetingMedicine();
		}
		if(NULL != Teaget)
		{
			CMonsterLogicExt::ItemList& il = Teaget->GetItemList();
			if(!il.empty())
			{
				item = *(il.begin());
			}
		}
	}
	if (NULL != Teaget)
	{
		g_sendCmd->WriteLong(Teaget->m_rPlayer.GetObjectId());
		item.Serialize(g_sendCmd);
		m_rPlayer.RecvCmd(g_sendCmd);
	}
}

void CPlayerLogic::ReLive()
{
	m_LiveType = eLive;
}

long CPlayerLogic::GetMoveSpeed()
{
	long nRet = m_rPlayer.m_moveSpeed;
	do
	{
		if(NULL == m_rPlayer.m_ai)
		{
			break;
		}
		if(m_rPlayer.m_ai->m_bTrace)
		{
			nRet = (long)(nRet * 1.5);
		}else if ( CRegionAI::STATE_RETURN == m_rPlayer.m_ai->m_state )
		{
			nRet = (long)(nRet * 10);
		}
	}while(false);
	return nRet;
}

CMonsterLogicExt* CPlayerLogic::GetMonsterLogicExt()
{
	if(NULL == m_pMonsterLogicExt)
	{
		m_pMonsterLogicExt = RT_NEW CMonsterLogicExt(m_rPlayer);
	}
	return m_pMonsterLogicExt;
}

bool CPlayerLogic::IsGetingSkin()
{
	return NULL != GetGetingSkin();
}

bool CPlayerLogic::IsGetingMedicine()
{
	return NULL != GetGetingMedicine();
}

bool CPlayerLogic::IsGetingMine()
{
	return NULL != GetGetingMine();
}

void CPlayerLogic::SetGetingSkin(CMonsterLogicExt* pMonsterLogicExt)
{
	if (NULL != pMonsterLogicExt)
	{
		m_dwGetingSkinID = pMonsterLogicExt->m_rPlayer.GetObjectId();
	}
	else
	{
		m_dwGetingSkinID = eEmptyDBID;
	}
}

void CPlayerLogic::SetGetingMedicine(CMonsterLogicExt* pMonsterLogicExt)
{
	if (NULL != pMonsterLogicExt)
	{
		m_dwGetingMedicineID = pMonsterLogicExt->m_rPlayer.GetObjectId();
	}
	else
	{
		m_dwGetingMedicineID = eEmptyDBID;
	}
}

void CPlayerLogic::SetGetingMine(CMonsterLogicExt* pMonsterLogicExt)
{
	if (NULL != pMonsterLogicExt)
	{
		m_dwGetingMineID = pMonsterLogicExt->m_rPlayer.GetObjectId();
	}
	else
	{
		m_dwGetingMineID = eEmptyDBID;
	}
}

void CPlayerLogic::CaneclGetingSkin()
{
	CMonsterLogicExt* pEx = GetGetingSkin();
	if(NULL != pEx)
	{
		pEx->CaneclGetSkin();
		m_dwGetingSkinID = eEmptyDBID;
	}
}

void CPlayerLogic::CaneclGetingMedicine()
{
	CMonsterLogicExt* pEx = GetGetingMedicine();
	if(NULL != pEx)
	{
		pEx->CaneclGetMedicine();
		m_dwGetingMedicineID = eEmptyDBID;
	}
}

void CPlayerLogic::CaneclGetingMine()
{
	CMonsterLogicExt* pEx = GetGetingMine();
	if(NULL != pEx)
	{
		pEx->CaneclGetMine();
		m_dwGetingMineID = eEmptyDBID;
	}

}

bool CPlayerLogic::HaveGetSkinSkill()
{
	return NULL != m_rPlayer.m_Skill.FindActiveSkillBySubID(654);
}

bool CPlayerLogic::IsHaveItem(unsigned long dwID)
{
	return m_rPlayer.m_pItemContainer->m_Bag.FindType(dwID);
}

int  CPlayerLogic::GetSkillLevel(unsigned short SubID)
{
	int nRet = 0;
	CSkillInstance* pSkill = m_rPlayer.m_Skill.FindActiveSkillBySubID(SubID);
	do
	{
		if(NULL == pSkill)
		{
			break;
		}
		nRet = pSkill->m_pAttr->iLevel;
	}while(false);
	return nRet;
}

SSkill*  CPlayerLogic::GetSkill(unsigned short SubID)
{
	SSkill* pRetSkill = NULL;
	CSkillInstance* pSkill = m_rPlayer.m_Skill.FindActiveSkillBySubID(SubID);
	do
	{
		if(NULL == pSkill)
		{
			break;
		}
		pRetSkill = pSkill->m_pAttr;
	}while(false);
	return pRetSkill;
}

void CPlayerLogic::AddLifeSkillExp(int nExp, unsigned short wSkillID)
{
	CSkillInstance* pSkill = m_rPlayer.m_Skill.FindActiveSkillBySubID(wSkillID);
	do
	{
		if(NULL == pSkill)
		{
			break;
		}
		if(nExp <= 0)
		{
			break;
		}
		pSkill->m_wExp += (unsigned short)(nExp);

		if (pSkill->m_pAttr->iLevelUpExp)
		{
			if (pSkill->m_wExp >= pSkill->m_pAttr->iLevelUpExp)
			{
				pSkill->m_wExp = pSkill->m_pAttr->iLevelUpExp;
				int nCanLevel = m_rPlayer.m_core.Lev /5 + 1;
				if(nCanLevel > 10)
				{
					nCanLevel = 10;
				}
				if(pSkill->m_pAttr->iLevel >= nCanLevel)
				{
					m_rPlayer.SendSystemMessage(R(MSG_SKILL_NEEDCHARACTERLEVEL));
					break;
				}
				SSkill* pSkillNext = Skill()->FindNextLevelSkill(pSkill->m_pAttr->wID);
				if (pSkillNext)
				{
					if (m_rPlayer.m_Skill.StudySkill(pSkillNext->wID, true))
					{
						rt2_sprintf(g_strStaticBuffer, R(MSG_SKILL_UPDATE), pSkillNext->szName, pSkillNext->iLevel, pSkillNext->iMaxLevel);
						m_rPlayer.SendSystemMessage(g_strStaticBuffer);
					}
				}
			}
			else
			{
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_update_skill_exp);
				g_sendCmd->WriteShort(pSkill->m_pAttr->wID);
				g_sendCmd->WriteShort(pSkill->m_wExp);
				m_rPlayer.RecvCmd(g_sendCmd);
			}
		}  
	}while(false);
}
void CPlayerLogic::StopGetLifeItem()
{
	unsigned char cAni = 0xff;
	if(IsGetingSkin())
	{
		cAni = DO_COMMAND_GETSKIN;
		CaneclGetingSkin();
	}
	else if(IsGetingMine())
	{
		CMonsterLogicExt* pEx = GetGetingMine();
		if (NULL != pEx)
		{
			if (pEx->IsLifeSkillItemNPC())
			{
				cAni = DO_COMMAND_TASKCOLECT;
			}
			else
			{
				cAni = DO_COMMAND_GETMINE;
			}
			CaneclGetingMine();
		}
	}
	else if(IsGetingMedicine())
	{
		cAni = DO_COMMAND_GETMEDICINE;
		CaneclGetingMedicine();
	}
	if (0xff != cAni)
	{
		m_rPlayer.ClientDoCommand(cAni, 1);
	}

}
void CPlayerLogic::DonotGetLifeItem()
{
	CMonsterLogicExt* pT = NULL;
	if(IsGetingSkin())
	{
		pT = this->GetGetingSkin();
	}
	else if(IsGetingMine())
	{
		pT = this->GetGetingMine();
	}
	else if(IsGetingMedicine())
	{
		pT = this->GetGetingMedicine();
	}
	if(NULL != pT)
	{
		pT->OnClosePickwindow(&m_rPlayer);
	}
}

bool CPlayerLogic::IsDoingTask(int nTaskID)
{
	bool bRet = false;
	do
	{
		STaskInfo *info = m_rPlayer.m_task.FindTask(nTaskID);
		if(NULL == info)
		{
			break;
		}
		if(!info->FIsDoing())
		{
			break;
		}
		bRet = true;
	}while(false);
	return bRet;
}

int CPlayerLogic::UseTaskItem(int page, int gridI, int gridJ)
{
	int nRet = 1;
	SItemID item;
	do
	{
		item =  m_rPlayer.m_pItemContainer->m_Bag.GetItem(page, gridI, gridJ);
		if(!ItemID_IsValid(item))
		{
			break;
		}
		unsigned long dwIndex = CItemManager::GetIndexFromType(item.type);
		int nNum = 0;
		const int c_MaxTask = 16;
		ItemTaskData* ppData[c_MaxTask];
		nNum = CRSLogicExt::GetRSLogicExt().GetItemTaskData(dwIndex, ppData, c_MaxTask);
		if(0 == nNum)
		{
			break;
		}
		int i;
		m_pUsingItem = NULL;
		for (i = 0; i < nNum; i++)
		{
			ItemTaskData* pData = ppData[i];
			if (NULL == pData)
			{
				continue;
			}
			STaskInfo *info = m_rPlayer.m_task.FindTask(pData->pTask->id);
			if(NULL == info)
			{
				continue;
			}
			if(!info->FIsDoing())
			{
				continue;
			}

			RtsSceneBlockMap::SArea* pArea = m_rPlayer.m_scene->m_pTerrain->FindFirstArea(m_rPlayer.m_pos[0], m_rPlayer.m_pos[1]);
			int i;
			for (i=0; i<9 && pArea; i++)
			{
				if(NULL == pArea)
				{
					continue;
				}
				if (pData->strArea == pArea->szName)
				{
					m_pUsingItem = pData;
					break;
				}
				pArea = m_rPlayer.m_scene->m_pTerrain->FindNextArea();
			}
			if (NULL != m_pUsingItem)
			{
				break;
			}
		}
		if (NULL == m_pUsingItem)
		{
			break;
		}
		m_rPlayer.m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ, ITEM_REASON_DELETE);
		m_rPlayer.RecvCmd(g_sendCmd);

		StartUseTaskItem();

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_start_process_bar);

		m_rPlayer.RecvCmd(g_sendCmd);
		nRet = 0;
	}while(false);
	return nRet;
}

bool CPlayerLogic::IsUsingTaskItem()
{
	return NULL != m_pUsingItem;
}

void CPlayerLogic::StartUseTaskItem()
{
	m_dwEndUsingTaskItemTime = rtGetMilliseconds() + 3000;
}

void CPlayerLogic::EndUsingTaskItem()
{
	do 
	{
		if (NULL == m_pUsingItem)
		{
			break;
		}
		if (NULL == m_pUsingItem->pTask)
		{
			break;
		}
		STaskInfo *info = m_rPlayer.m_task.FindTask(m_pUsingItem->pTask->id);
		if(NULL == info)
		{
			break;	
		}
		STaskDesc *pDesc = info->GetDesc();
		if (NULL == pDesc)
		{
			break;	
		}
		info->SetUseItem(m_pUsingItem->cIndex);
		m_rPlayer.UpdateToClient(NET_CRE_UPDATE_TASK);
		/*bool b5124 = info->IsUseItemByID(5124);
		bool b5125 = info->IsUseItemByID(5125);
		bool b5126 = info->IsUseItemByID(5126);
		bool b5127 = info->IsUseItemByID(5127);*/
		m_rPlayer.CheckTaskFinishState(pDesc, NULL, false);
		//if (m_rPlayer.CheckTaskFinishState(info->Desc, NULL, false))
		//{
		//	if (info->FIsCanFinish())
		//	{
		//		info->FSetCanFinish();
		//		m_rPlayer.UpdateToClient(NET_CRE_UPDATE_TASK);
		//		/*g_sendCmd->BeginWrite();
		//		g_sendCmd->WriteShort(r2c_task_can_finish);
		//		g_sendCmd->WriteLong(info->Desc->id);
		//		m_rPlayer.RecvCmd(g_sendCmd);*/
		//	}
		//}
	} while (false);

	m_pUsingItem = NULL;
}

void CPlayerLogic::TrackTask(long nTaskID)
{
	int nTrackNum = 0;
	std::list<STaskInfo>::iterator iter;
	STaskInfo *pInfo = NULL;
	for(iter = m_rPlayer.m_task.m_taskList.begin(); iter != m_rPlayer.m_task.m_taskList.end(); iter++)
	{
		STaskInfo &info = *iter;
		
		if (info.IsTrack())
		{
			nTrackNum++;
		}
		if (info.Id == nTaskID)
		{
			pInfo = &info;
		}
	}
	if (nTrackNum >= 10)
	{
		m_rPlayer.SendSystemMessage(R(MSG_TASK_TRACKFULL));
	}
	else
	{
		if (NULL != pInfo)
		{
			pInfo->SetTrack();
			m_rPlayer.UpdateToClient(NET_CRE_UPDATE_TASK);
		}
	}
}

void CPlayerLogic::CaneclTrackTask(long nTaskID)
{
	STaskInfo *info = m_rPlayer.m_task.FindTask(nTaskID);
	if (NULL != info)
	{
		info->CaneclTrack();
		m_rPlayer.UpdateToClient(NET_CRE_UPDATE_TASK);
	}
}

CMonsterLogicExt* CPlayerLogic::GetGetingSkin()
{

	CMonsterLogicExt* pRet = NULL;
	do 
	{
		if (NULL == m_rPlayer.m_scene)
		{
			break;
		}
		if (eEmptyDBID == m_dwGetingSkinID)
		{
			break;
		}
		CRegionCreature *pFind = m_rPlayer.m_scene->FindCreature(m_dwGetingSkinID);
		if (NULL == pFind)
		{
			break;
		}
		if(pFind->IsUser())
		{
			break;
		}
		pRet = pFind->GetPlayerLogic()->GetMonsterLogicExt();
	} while (false);
	return pRet;
}

CMonsterLogicExt* CPlayerLogic::GetGetingMine()
{
	CMonsterLogicExt* pRet = NULL;
	do 
	{
		if (NULL == m_rPlayer.m_scene)
		{
			break;
		}
		if (eEmptyDBID == m_dwGetingMineID)
		{
			break;
		}
		CRegionCreature *pFind = m_rPlayer.m_scene->FindCreature(m_dwGetingMineID);
		if (NULL == pFind)
		{
			break;
		}
		if(pFind->IsUser())
		{
			break;
		}
		pRet = pFind->GetPlayerLogic()->GetMonsterLogicExt();
	} while (false);
	return pRet;
}

CMonsterLogicExt* CPlayerLogic::GetGetingMedicine()
{
	CMonsterLogicExt* pRet = NULL;
	do 
	{
		if (NULL == m_rPlayer.m_scene)
		{
			break;
		}
		if (eEmptyDBID == m_dwGetingMedicineID)
		{
			break;
		}
		CRegionCreature *pFind = m_rPlayer.m_scene->FindCreature(m_dwGetingMedicineID);
		if (NULL == pFind)
		{
			break;
		}
		if(pFind->IsUser())
		{
			break;
		}
		pRet = pFind->GetPlayerLogic()->GetMonsterLogicExt();
	} while (false);
	return pRet;
}

int  CPlayerLogic::GetCurMainStory()
{
	return m_nCurMainStory;
}

int  CPlayerLogic::GetCurMainStoryID()
{
	return m_rPlayer.m_task.m_nMainTaskData;
}

int  CPlayerLogic::SetCurMainStoryID(int nMainStoryDataID)
{
	int nRet = 0;
	do 
	{
		if (-1 == nMainStoryDataID)
		{
			m_nCurMainStory = 0;
			int nID = CMainStoryMgr::GetMgr().GetMainTaskIDByJobAndIndex(m_rPlayer.m_core.Metier, m_nCurMainStory);
			m_rPlayer.m_task.m_nMainTaskData = nID;
			break;
		}
		if (-2 == nMainStoryDataID)
		{
			m_nCurMainStory = CMainStoryMgr::GetMgr().GetMainStoryNum(m_rPlayer.m_core.Metier);
			m_rPlayer.m_task.m_nMainTaskData = -2;
			break;
		}

		MainStoryData* pData = CMainStoryMgr::GetMgr().GetMainStoryByID(nMainStoryDataID);
		if (NULL != pData)
		{
			int nIndex = CMainStoryMgr::GetMgr().GetIndexByMainTaskIDAndJob(m_rPlayer.m_core.Metier, nMainStoryDataID);
			if (nIndex >= 0)
			{
				m_nCurMainStory = nIndex;
				m_rPlayer.m_task.m_nMainTaskData = nMainStoryDataID;
				nRet = 0;
				break;
			}
		}
	} while (false);
	return nRet;
}

void CPlayerLogic::AfterFinishTask(STaskDesc *task)
{
	do 
	{
		if (NULL == task)
		{
			break;
		}
		if (task->m_TaskGroupID < 40)
		{
			MainStoryData* pData = CMainStoryMgr::GetMgr().GetMainStoryByTaskID(task->id);
			if (NULL == pData)
			{
				break;
			}
			int nMainID = GetCurMainStoryID();
			if (pData->nDataID != nMainID)
			{
				break;
			}
			int i;
			bool bB = false;
			for(i = 0; i < pData->nTaskNum; i++)
			{
				if (!IsTaskDone(pData->nTaskID[i]))
				{
					bB = true;
					break;
				}
			}
			if (bB)
			{
				break;
			}
			int nID = GetNextMainStoryID();
			this->SetCurMainStoryID(nID);
		}
		NPCTaskMap& npcMap = GetNPCTaskMap();
		NPCTaskMap::iterator iter;
		for(iter = npcMap.begin(); npcMap.end() != iter; iter++)
		{
			short wID = (*iter).second;
			if (wID == task->id)
			{
				npcMap.erase(iter);
				break;
			}
		}
	} while (false);
}

int  CPlayerLogic::GetNextMainStoryID()
{
	int nRet = -2;
	do
	{
		int nIndex = this->GetCurMainStory();
		if (nIndex == CMainStoryMgr::GetMgr().GetMainStoryNum(m_rPlayer.m_core.Metier))
		{
			//ERR3("nIndex == CMainStoryMgr::GetMgr().GetMainStoryNum %d", 2);
			break;
		}
		nRet = CMainStoryMgr::GetMgr().GetMainTaskIDByJobAndIndex(m_rPlayer.m_core.Metier, nIndex +1);
	}while (false);
	return nRet;
}

unsigned long CPlayerLogic::GetNextOverHeadTime()
{
	return m_dwNextOverHeadTime;
}

ETaskOverHead CPlayerLogic::GetTaskOverHead(DWORD npcOID)
{
	ETaskOverHead enRet = TOH_CanNotAccept;
	do 
	{
		bool bB = false;
		bool bDing = false;
		size_t nSize = 0;

		if (NULL == m_rPlayer.m_scene)
		{
			break;
		}

		CRegionCreature *pNPC = m_rPlayer.m_scene->FindCreature(npcOID);
		if (NULL == pNPC || NULL == pNPC->m_npcInfo)
		{
			break;
		}
		CRegionNpcTask *pReNPC = (CRegionNpcTask*)(pNPC);
		CPlayerLogic::ForTaskType enType = pReNPC->GetPlayerLogic()->GetForTaskType();
		nSize = pReNPC->m_finishTask.size();
		for(size_t i=0; i < nSize; i++)
		{
			STaskDesc* pDesc = pReNPC->m_finishTask[i];
			if (NULL == pDesc)
			{
				continue;
			}
			STaskInfo *info = m_rPlayer.m_task.FindTask(pDesc->id);
			if(NULL != info)
			{
				if (info->FIsCanFinish())
				{
					enRet = TOH_AcceptAndFinish;
					bB = true;
					break;
				}
				if (info->FIsDoing())
				{
					bDing = true;
					if (CPlayerLogic::eRand == enType)
					{
						enRet = TOH_AcceptNoFinish;
						bB = true;
						break;
					}
				}
			}
		}
		if (bB)
		{
			break;
		}
		nSize = pReNPC->m_acceptTask.size();
		for(size_t i=0; i < nSize; i++)
		{
			STaskDesc* pDesc = pReNPC->m_acceptTask[i];
			if (NULL == pDesc)
			{
				continue;
			}
			STaskInfo *info = m_rPlayer.m_task.FindTask(pDesc->id);
			if(NULL != info)
			{
				if (info->FIsDoing())
				{
					bDing = true;
					if (CPlayerLogic::eRand == enType)
					{
						enRet = TOH_AcceptNoFinish;
						bB = true;
						break;
					}
				}
			}
			else
			{
				long ret = m_rPlayer.CheckTaskAcceptState(pDesc, pReNPC);
				if(ret == TASK_STATE_CAN_ACCEPT)
				{
					enRet = TOH_CanAccept;
					if (CPlayerLogic::eRand != enType)
					{
						bB = true;
						break;
					}
				}
			}
		}
		if (bB)
		{
			break;
		}
		if (bDing)
		{
			enRet = TOH_AcceptNoFinish;
		}
	} while (false);
	return enRet;
}

void CPlayerLogic::SetNextOverHeadTime(unsigned long dwTime)
{
	m_dwNextOverHeadTime = dwTime;
}

unsigned long CPlayerLogic::GetNextGongTaskTime()
{
	return m_dwNextGongTaskTime;
}

void CPlayerLogic::SetNextGongTaskTime(unsigned long dwTime)
{
	m_dwNextGongTaskTime = dwTime;
}


void CPlayerLogic::AddJobRecord(long nAdd)
{
	const int c_nBuffSize = 256;
	char szBuff[c_nBuffSize];
	const char* pC = R(MSG_GET_JobRecord);
	
	do 
	{
		if (0 == nAdd)
		{
			break;
		}

		CRegionCreature* pAdd = &m_rPlayer;
		if(m_rPlayer.IsUserPet() || m_rPlayer.IsCallNpc())
		{
			pAdd = m_rPlayer.m_master;
		}

		if (NULL == pAdd)
		{
			break;
		}

		rt2_snprintf(szBuff, c_nBuffSize, pC, nAdd);
		pAdd->m_core.AddJobRecord(nAdd);
		pAdd->SendBattleInfoMessage(szBuff);
	} while (false);
}

void CPlayerLogic::AddGold(int nAdd)
{
	//const int c_nBuffSize = 256;
	//char szBuff[c_nBuffSize];
	//const char* pC = R(MSG_GET_MONEY);
	////int nLen = strlen(pC);
	//rt2_snprintf(szBuff, c_nBuffSize, pC, nAdd);
	//m_rPlayer.AddGold(nAdd);
	//m_rPlayer.SendBattleInfoMessage(szBuff);
	m_rPlayer.AddGold(nAdd);
}

void CPlayerLogic::AddContribute(long nAdd)
{
	const int c_nBuffSize = 256;
	char szBuff[c_nBuffSize];
	const char* pC = R(MSG_GET_Contribute);
	do 
	{
		if (0 == nAdd)
		{
			break;
		}

		CRegionCreature* pAdd = &m_rPlayer;
		if(m_rPlayer.IsUserPet() || m_rPlayer.IsCallNpc())
		{
			pAdd = m_rPlayer.m_master;
		}

		if (NULL == pAdd)
		{
			break;
		}

		rt2_snprintf(szBuff, c_nBuffSize, pC, nAdd);
		pAdd->m_core.AddContribute(nAdd);
		pAdd->SendSystemMessage(szBuff);
	} while (false);
}

void CPlayerLogic::AddUnionRecord(int add)
{
	do 
	{
		if (0 == add)
		{
			break;
		}
		const int c_nBuffSize = 256;
		char szBuff[c_nBuffSize];
		const char* pC = R(MSG_GET_UnionRecord);
		
		CRegionCreature* pAdd = &m_rPlayer;
		if(m_rPlayer.IsUserPet() || m_rPlayer.IsCallNpc())
		{
			pAdd = m_rPlayer.m_master;
		}

		if (NULL == pAdd)
		{
			break;
		}
		if (NULL == pAdd->m_userInfo)
		{
			break;
		}

		rt2_snprintf(szBuff, c_nBuffSize, pC, add);
		pAdd->m_core.AddUnionRecord(add);
		pAdd->SendSystemMessage(szBuff);
		CG_CmdPacket& packet = g_region->m_gws.BeginSend();
		packet.WriteShort(r2g_union);
		packet.WriteShort(r2g_union_update_Contribution);
		packet.WriteLong(pAdd->m_userInfo->m_userId);
		packet.WriteLong(pAdd->m_core.UnionRecord);

		g_region->m_gws.EndSend();
	} while (false);
}

void CPlayerLogic::DoGuide(GuideType type)
{
	bool bB = false;
	do 
	{
		switch (type)
		{
		case eKeyMoveFront:
			if (!m_rPlayer.m_task.IsDoneGuild(eHitMove))
			{
				bB = true;
			}
			break;
		case eKeyMoveBack:
			if (!m_rPlayer.m_task.IsDoneGuild(eHitMove)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveFront))
			{
				bB = true;
			}
			break;
		case eKeyMoveLeft:
			if (!m_rPlayer.m_task.IsDoneGuild(eHitMove)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveFront)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveBack))
			{
				bB = true;
			}
			break;
		case eKeyMoveRight:
			if (!m_rPlayer.m_task.IsDoneGuild(eHitMove)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveFront)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveBack)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveLeft))
			{
				bB = true;
			}
			break;
		case eViewChange:
			if (!m_rPlayer.m_task.IsDoneGuild(eHitMove)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveFront)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveBack)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveLeft)
				|| !m_rPlayer.m_task.IsDoneGuild(eKeyMoveRight))
			{
				bB = true;
			}
			break;
		case  eNPCAcceptTask:
			if (!m_rPlayer.m_task.IsDoneGuild(eNPCTaskTalk))
			{
				bB = true;
			}
			break;
		case  eMoveToNPC:
			if (!m_rPlayer.m_task.IsDoneGuild(eNPCTaskTalk)
				|| !m_rPlayer.m_task.IsDoneGuild(eNPCAcceptTask))
			{
				bB = true;
			}
			break;
		case  eNPCFinishTask:
			if (!m_rPlayer.m_task.IsDoneGuild(eNPCTaskTalk)
				|| !m_rPlayer.m_task.IsDoneGuild(eNPCAcceptTask)
				|| !m_rPlayer.m_task.IsDoneGuild(eMoveToNPC))
			{
				bB = true;
			}
			break;
		case  eAddAttr:
			if (!m_rPlayer.m_task.IsDoneGuild(eAttrButFlash))
			{
				bB = true;
			}
			break;
		case  eFinishAddAttr:
			if (!m_rPlayer.m_task.IsDoneGuild(eAttrButFlash)
				|| !m_rPlayer.m_task.IsDoneGuild(eAddAttr))
			{
				bB = true;
			}
			break;
		case  eSkillChangePane:
			if (!m_rPlayer.m_task.IsDoneGuild(eOpenSkillUI))
			{
				bB = true;
			}
			break;
		case  eUsePet2:
			if (!m_rPlayer.m_task.IsDoneGuild(eUsePet1))
			{
				bB = true;
			}
			break;
		case  eUsePet3:
			if (!m_rPlayer.m_task.IsDoneGuild(eUsePet1)
				|| !m_rPlayer.m_task.IsDoneGuild(eUsePet2))
			{
				bB = true;
			}
			break;
		case  eBuyItem2:
			if (!m_rPlayer.m_task.IsDoneGuild(eBuyItem1))
			{
				bB = true;
			}
			break;
		case  eBuyItem3:
			if (!m_rPlayer.m_task.IsDoneGuild(eBuyItem1)
				|| !m_rPlayer.m_task.IsDoneGuild(eBuyItem2))
			{
				bB = true;
			}
			break;
		case  eFaBao2:
			if (!m_rPlayer.m_task.IsDoneGuild(eFaBao1))
			{
				bB = true;
			}
			break;
		case  eFaBao3:
			if (!m_rPlayer.m_task.IsDoneGuild(eFaBao1)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao2))
			{
				bB = true;
			}
			break;
		case  eFaBao4:
			if (!m_rPlayer.m_task.IsDoneGuild(eFaBao1)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao2)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao3))
			{
				bB = true;
			}
			break;
		case  eFaBao5:
			if (!m_rPlayer.m_task.IsDoneGuild(eFaBao1)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao2)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao3)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao4))
			{
				bB = true;
			}
			break;
		case  eFaBao6:
			if (!m_rPlayer.m_task.IsDoneGuild(eFaBao1)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao2)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao3)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao4)
				|| !m_rPlayer.m_task.IsDoneGuild(eFaBao5))
			{
				bB = true;
			}
			break;
			/*case  eMail2:
			if (!m_rPlayer.m_task.IsDoneGuild(eMail1))
			{
			bB = true;
			}
			break;*/
		case  eMail3:
			if (!m_rPlayer.m_task.IsDoneGuild(eMail1)
				/*|| !m_rPlayer.m_task.IsDoneGuild(eMail2)*/)
			{
				bB = true;
			}
			break;
			//case  eMail4:
			//	if (!m_rPlayer.m_task.IsDoneGuild(eMail1)
			//		/*|| !m_rPlayer.m_task.IsDoneGuild(eMail2)*/
			//		|| !m_rPlayer.m_task.IsDoneGuild(eMail3))
			//	{
			//		bB = true;
			//	}
			//	break;
		case  eMail5:
			if (!m_rPlayer.m_task.IsDoneGuild(eMail1)
				/*|| !m_rPlayer.m_task.IsDoneGuild(eMail2)*/
				|| !m_rPlayer.m_task.IsDoneGuild(eMail3)
				/*|| !m_rPlayer.m_task.IsDoneGuild(eMail4)*/)
			{
				bB = true;
			}
			break;
		case  eLifeSkill2:
			if (!m_rPlayer.m_task.IsDoneGuild(eLifeSkill1))
			{
				bB = true;
			}
			break;
		case  eFriend2:
			if (!m_rPlayer.m_task.IsDoneGuild(eFriend1))
			{
				bB = true;
			}
			break;
		case  eMidMap2:
			if (!m_rPlayer.m_task.IsDoneGuild(eMidMap1))
			{
				bB = true;
			}
			break;
		}
		if (bB)
		{
			break;
		}
		m_rPlayer.m_task.SetDoneGuild(type, true);
		m_rPlayer.UpdateToClient(NET_CRE_UPDATE_TASK);
		//m_rPlayer.NetSend_r2c_task_event(1);
	} while (false);
}

void CPlayerLogic::OnAreaEnter(const char* szAreName)
{
	do 
	{
		if (NULL == szAreName)
		{
			break;
		}
		std::string str = "xinshouyindao";
		if (str == szAreName)
		{
			GetCNewPlayerGuide()->DoGuide(eAreaEnter);
			m_rPlayer.m_task.SetDoneGuild(eAreaEnter, true);
		}
	} while (false);
}

void CPlayerLogic::SetOnLineTime(unsigned long dwTime)
{
	m_dwOnLineTime = dwTime * 1000;
	m_dwEnterTime = rtGetMilliseconds();
	int i;
	for (i = 0; i < c_nNewNum; i++)
	{
		if (m_dwOnLineTime < s_Time[i])
		{
			break;
		}
		SendNewPlayerMail(i);
	}
	m_nNewW = i;
}

void CPlayerLogic::ClientGetOnLineTime()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_SynOnlineTime	);
	unsigned long dwTime = GetOnLineTime();
	g_sendCmd->WriteLong((long)(dwTime));
	m_rPlayer.RecvCmd(g_sendCmd);
}

unsigned long CPlayerLogic::GetOnLineTime()
{
	unsigned long long dwTime = rtGetMilliseconds() - m_dwEnterTime;
	dwTime +=  m_dwOnLineTime;
	dwTime /=  1000;
	return (unsigned long)(dwTime);
}

void CPlayerLogic::OnRunNewPlayer(unsigned long dwTime)
{
	//static int s_nItemID[c_nNewNum] = {5700, 5701, 5702, 5706, 5707, 5708, 5709};

	do 
	{
		if (m_nNewW >= c_nNewNum)
		{
			break;
		}
		unsigned long dwS = m_dwOnLineTime + dwTime - m_dwEnterTime;

		if (dwS < s_Time[m_nNewW])
		{
			break;
		}
		SendNewPlayerMail(m_nNewW);
		m_nNewW++;
	} while (false);
}

void CPlayerLogic::SendNewPlayerMail(int n)
{
	//static int s_nItemID[c_nNewNum] = {5700, 5701, 5702, 5706, 5707, 5708, 5709};
	////5702蜀山5703花间5704五台5705苗疆	
	//static int s_nItemID2[CMainStoryMgr::eJobNum] = {5704, 5703, 5702, 5705};
	int ItemID[CMainStoryMgr::eJobNum][c_nNewNum] = {{5702, 5706, 5710, 5714, 5718, 5722, 5726},
													 {5701, 5705, 5709, 5713, 5717, 5721, 5725},
													 {5700, 5704, 5708, 5712, 5716, 5720, 5724},
													 {5703, 5707, 5711, 5715, 5719, 5723, 5727}};
	do 
	{
		if (NULL == m_rPlayer.m_pMail)
		{
			break;
		}
		bool bSend = m_rPlayer.m_task.IsSendNewPlayerMail(n);
		if (bSend)
		{
			break;
		}
		/*if (0 < m_rPlayer.m_core.Metier && m_rPlayer.m_core.Metier <= CMainStoryMgr::eJobNum)
		{
			s_nItemID[2] = s_nItemID2[m_rPlayer.m_core.Metier - 1];
		}*/
		
		const int c_nTitleLen = 64;
		const int c_nContentLen = 256;
		char szTitle[c_nTitleLen];
		char szContent[c_nContentLen];
		rt2_sprintf(szTitle, "新手在线奖励%d", n + 1);
		if (c_nNewNum - 1 == n)
		{
			rt2_sprintf(szContent, R(MSG_SERVER_MAIL_001), n + 1);
		}
		else
		{
			rt2_sprintf(szContent, R(MSG_SERVER_MAIL_002), 
				m_nNewW + 1, (s_Time[n + 1] - s_Time[m_nNewW]) / 60000, n + 2);
		}

		CItemManager*  pItemManager = m_rPlayer.m_pItemContainer->GetItemManager();

		SItemID Item = g_region->m_pItemManager->CreateItem(ItemID[m_rPlayer.m_core.Metier - 1][m_nNewW], ItemColor_White, 0, IB_NotBind);
		std::string strItem = Item.SaveToString(pItemManager);

		g_region->m_gws.SendMail(m_rPlayer.m_userInfo->m_userId, 0, 
			"【系统】", szTitle, szContent, strItem, 0);
		if (0 == n)
		{
			GetCNewPlayerGuide()->Mail1();
		}
		m_rPlayer.m_task.SetSendNewPlayerMail(n, true);
	} while (false);
}

void CPlayerLogic::OnLevelUp()
{
	if (m_rPlayer.IsUser())
	{
		switch (m_rPlayer.m_core.Lev)
		{
		case 2:
			GetCNewPlayerGuide()->AttrButFlash();
			break;
		case 4:
			//GetCNewPlayerGuide()->Mail1();
			break;
		default:
			break;
		}
	}
}

void CPlayerLogic::OnUseItem(SItemID item)
{
	do 
	{
		if (ItemIsYaoPing(item))
		{
			if (ItemIsHPBottle(item))
			{
				unsigned long dwTime = rtGetMilliseconds();
				if (dwTime < GetCanUseYaoPingHpTime())
				{
					break;
				}
			}
			if (ItemIsMPBottle(item))
			{
				unsigned long dwTime = rtGetMilliseconds();
				if (dwTime < GetCanUseYaoPingMpTime())
				{
					break;
				}
			}
			if (ItemIsPetHpBottle(item))
			{
				unsigned long dwTime = rtGetMilliseconds();
				if (dwTime < GetCanUseYaoPingPetHpTime())
				{
					break;
				}
			}
			if (ItemIsPetMpBottle(item))
			{
				unsigned long dwTime = rtGetMilliseconds();
				if (dwTime < GetCanUseYaoPingPetMpTime())
				{
					break;
				}
			}
			if (ItemIsEnergyBottle(item))
			{
				unsigned long dwTime = rtGetMilliseconds();
				if (dwTime < GetCanUseYaoPingEnergyTime())
				{
					break;
				}
			}
		}
	} while (false);
}

unsigned long CPlayerLogic::GetCanUseYaoPingHpTime()
{
	return m_dwCanUseYaoPingHpTime;
}

unsigned long CPlayerLogic::GetCanUseYaoPingMpTime()
{
	return m_dwCanUseYaoPingMpTime;
}

unsigned long CPlayerLogic::GetCanUseYaoPingPetHpTime()
{
	return m_dwCanUseYaoPingPetHpTime;
}

unsigned long CPlayerLogic::GetCanUseYaoPingPetMpTime()
{
	return m_dwCanUseYaoPingPetMpTime;
}

unsigned long CPlayerLogic::GetCanUseYaoPingEnergyTime()
{
	return m_dwCanUseYaoPingEnergyTime;
}

void CPlayerLogic::SetCanUseYaoPingHpTime(unsigned long dwTime)
{
	m_dwCanUseYaoPingHpTime = dwTime;
}

void CPlayerLogic::SetCanUseYaoPingMpTime(unsigned long dwTime)
{
	m_dwCanUseYaoPingMpTime = dwTime;
}

void CPlayerLogic::SetCanUseYaoPingPetHpTime(unsigned long dwTime)
{
	m_dwCanUseYaoPingPetHpTime = dwTime;
}

void CPlayerLogic::SetCanUseYaoPingPetMpTime(unsigned long dwTime)
{
	m_dwCanUseYaoPingPetMpTime = dwTime;
}

void CPlayerLogic::SetCanUseYaoPingEnergyTime(unsigned long dwTime)
{
	m_dwCanUseYaoPingEnergyTime = dwTime;
}

bool CPlayerLogic::IsInPShop()
{
	return NULL != m_rPlayer.m_pPShop || 0 != m_rPlayer.m_bPShop;
}

 CPlayerLogic::NPCTaskMap& CPlayerLogic::GetNPCTaskMap()
{
	return m_NPCTaskMap;
}

short CPlayerLogic::FindInRandNPCMap(unsigned long dwNPCID)
{
	short wRet = 0x7fff;
	do 
	{
		NPCTaskMap& npcMap = GetNPCTaskMap();
		NPCTaskMap::iterator iter = npcMap.find(dwNPCID);
		if (npcMap.end() == iter)
		{
			break;
		}
		wRet = (*iter).second;
	} while (false);
	return wRet;
}

void CPlayerLogic::AddInRandNPCMap(unsigned long dwNPCID, short nTaskID)
{
	NPCTaskMap& npcMap = GetNPCTaskMap();
	npcMap[dwNPCID] = nTaskID;
}

bool CPlayerLogic::IsAroundMe(CRegionCreature* pCre)
{
	bool bRet = false;
	do 
	{
		if (NULL == pCre)
		{
			break;
		}
		if (NULL == m_rPlayer.m_scene)
		{
			break;
		}
		CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
		int iCnt = m_rPlayer.m_scene->GetAroundCreatureByTile(pFindCreature,
			m_rPlayer.m_pos, DEFAULT_RANGE, NULL, PT_PATH, false);

		int i;
		for(i = 0; i < iCnt; i++)
		{
			if(pCre == pFindCreature[i])
			{
				bRet = true;
				break;
			}
		}
	} while (false);
	return bRet;
}

void CPlayerLogic::OnSetItem(const SItemID &idS, const SItemID &idD)
{
	OnItemChangeForTask(idS);
	OnItemChangeForTask(idD);
}

void CPlayerLogic::OnRemoveItem(const SItemID &idS)
{
	OnItemChangeForTask(idS);
}

void CPlayerLogic::OnItemChangeForTask(const SItemID &idS)
{
	CRSLogicExt::ItemFinishTaskMap& finishMap = CRSLogicExt::GetRSLogicExt().GetItemFinishTaskMap();
	unsigned long dwIndex = CItemManager::GetIndexFromType(idS.type);
	do 
	{
		CRSLogicExt::ItemFinishTaskMap::iterator beg = finishMap.lower_bound(dwIndex);
		CRSLogicExt::ItemFinishTaskMap::iterator end = finishMap.upper_bound(dwIndex);

		CRSLogicExt::ItemFinishTaskMap::iterator iter;
		for(iter = beg; iter != end; iter++)
		{
			STaskDesc *desc = g_TableTask.FindTask((*iter).second);
			if(NULL == desc)
			{
				continue;
			}
			m_rPlayer.CanFinishTask(desc, 0, NULL);
		}
	} while (false);
}

////////////////////////
//class CNpcTaskLogicExt
///////////////////////
//char CNpcTaskLogicExt::s_m_DefTalkBuff[CNpcTaskLogicExt::eMaxTexSize] = {0};
int  CNpcTaskLogicExt::s_m_nCurIndex = 0;
//char CNpcTaskLogicExt::s_m_TempBuff[CNpcTaskLogicExt::eTempBuffSize] = {0};
G_MEMDEF(CNpcTaskLogicExt::s_m_DefTalkBuff, CNpcTaskLogicExt::eMaxTexSize)
G_MEMDEF(CNpcTaskLogicExt::s_m_TempBuff, CNpcTaskLogicExt::eTempBuffSize)
CNpcTaskLogicExt::CNpcTaskLogicExt(CRegionNpcTask& rNPC):m_rNPC(rNPC), m_pTalk(NULL), m_nTalkLen(0)
{
	S_MEMPROTECTOR(s_m_DefTalkBuff, eMaxTexSize, bMP1)
	S_MEMPROTECTOR(s_m_TempBuff, eTempBuffSize, bMP2)

	std::ifstream dataFile(m_rNPC.m_npcInfo->DefaultTalk.c_str(), std::ios::binary|std::ios::ate);
	if( !dataFile.fail())
	{
		std::streampos size = dataFile.tellg();
		dataFile.seekg (0, std::ios::beg);
		dataFile.read(s_m_DefTalkBuff + s_m_nCurIndex, size);
		s_m_DefTalkBuff[s_m_nCurIndex + size] = '\0';
		const char* pF = "</body>";
		char* pFind = strstr(s_m_DefTalkBuff + s_m_nCurIndex, pF);
		if(NULL != pFind)
		{
			m_pTalk = s_m_DefTalkBuff + s_m_nCurIndex;
			m_nTalkLen = pFind - m_pTalk;
			*pFind = '\0';
			s_m_nCurIndex += (m_nTalkLen + 1);
		}
		dataFile.close();
	}

}
int CNpcTaskLogicExt::ShowTaskList(CRegionCreature *target)
{
	int nRet = 1;
	CPlayerLogic::ForTaskType enType = m_rNPC.GetPlayerLogic()->GetForTaskType();
	switch(enType)
	{
	case CPlayerLogic::eRand:
		nRet = ProcessRandTask(target);
		break;
	case CPlayerLogic::eList:
		nRet = SendList(target);
		break;
	case CPlayerLogic::eCircle:
		break;
	default:
		break;
	}
	return nRet;
}

int CNpcTaskLogicExt::ProcessRandTask(CRegionCreature *target)
{
	int nRet = 0;
	const int c_nMaxTask = 20;
	STaskDesc* DescBuff[c_nMaxTask];
	int nNum = 0;
	srand( (unsigned)time( NULL ) );

	do
	{
		nNum = GetCanFinishShowTask(target, DescBuff, c_nMaxTask, false);
		if(nNum > 0)
		{
			int n = rand() % nNum;
			target->FinishTask(DescBuff[n], 0, &m_rNPC);
			if(!m_rNPC.IsCircleTaskNpc())
			{
				m_rNPC.ShowHtmlDialog(target, DescBuff[n]->npc2AfterFinishTalk.c_str());
			}
			break;
		}
		nNum = GetDoingShowTask(target, DescBuff, c_nMaxTask, false);
		if(nNum > 0)
		{
			int n = rand() % nNum;
			nRet = ProcessDingTask(target, DescBuff[n]);
			break;
		}
		nNum = GetAcceptShowTask(target, DescBuff, c_nMaxTask, false);
		if(nNum > 0)
		{
			int n = rand() % nNum;
			m_rNPC.ShowHtmlDialog(target, DescBuff[n]->npc1PreAcceptTalk.c_str());
			break;
		}
		g_region->ShowHtmlDialog(&m_rNPC, target, R(RES_DEFAULT_TALK));
	}while(false);
	return nRet;
}



int CNpcTaskLogicExt::DefTalkRand(std::ostrstream &os, CRegionCreature *target, int &nFinish, int &nDoing, int &nAccept, bool bWriteText)
{
	int nRet = 0;
	const int c_nMaxTask = 20;
	STaskDesc* DescBuff[c_nMaxTask];
	STaskDesc* pShow = NULL;

	int nNumCanFinish = 0;
	int nNumDoing = 0;
	int nNumCanAccept = 0;

	nFinish = 0;
	nDoing = 0;
	nAccept = 0;

	SDSet& sd = GetSDSet();
	sd.clear();

	do
	{
		if (NULL == target)
		{
			break;
		}
		nNumCanFinish = GetCanFinishShowTask(target, DescBuff, c_nMaxTask, false);
		if(nNumCanFinish > 0)
		{
			int n = rand() % nNumCanFinish;
			if(!m_rNPC.IsCircleTaskNpc())
			{
				pShow = DescBuff[n];
			}
		}
		else
		{
			nNumDoing = GetDoingShowTask(target, DescBuff, c_nMaxTask, false);
			if(nNumDoing > 0)
			{
				int n = rand() % nNumDoing;
				pShow = DescBuff[n];
			}
			else
			{
				nNumCanAccept = GetAcceptShowTask(target, DescBuff, c_nMaxTask, false);
				if(nNumCanAccept > 0)
				{
					short wID = target->GetPlayerLogic()->FindInRandNPCMap(m_rNPC.m_npcInfo->Id);
					bool bF = false;
					int i;
					for(i = 0; i < nNumCanAccept; i++)
					{
						if (wID == DescBuff[i]->id)
						{
							pShow = DescBuff[i];
							bF = true;
							break;
						}
					}
					if (!bF)
					{
						int n = rand() % nNumCanAccept;
						pShow = DescBuff[n];
						target->GetPlayerLogic()->AddInRandNPCMap(m_rNPC.m_npcInfo->Id, pShow->id);
					}
				}
			}
		}

		nFinish = nNumCanFinish;
		nDoing = nNumDoing;
		nAccept = nNumCanAccept;

		if(NULL != pShow)
		{
			if (!bWriteText)
			{
				nRet = 0;
				break;
			}
			const char* pChar = NULL;
			if (nNumCanFinish > 0)
			{
				pChar = R(MSG_TASK_WHICHCANFINISH);//"可完成的任务";
			}
			else if(nNumDoing > 0)
			{
				pChar = R(MSG_TASK_WHICHONPROGRESS);//"正在做的任务";
			}
			else
			{
				pChar = R(MSG_TASK_WHICHCANACCEPT);//"可接受的任务";
			}
			os<<"<p><font color=\"#7FAF33\">"<<pChar<<"</font></p>"<<endl;
			os<<"<p><a href=\"rs://_TA_"<<pShow->id<<"\">"<<pShow->name<<"</a></p>"<<endl;
			nRet = 0;
		}
		else
		{
			nRet = 1;
		}
	}while(false);
	return nRet;
}

int CNpcTaskLogicExt::DefTalk(CRegionCreature *target)
{
	int nRet = 1;
	//int nRetType = 0;
	int nFinish = 0, nDoing = 0, nAccept = 0;
	do
	{
		if(NULL == target)
		{
			break;
		}
		char* pTalk = GetDefTalk();
		int nLen = GetDefTalkLen();
		if(NULL == pTalk || 1 > nLen)
		{
			break;
		}
		rt2_strcpy(s_m_TempBuff, pTalk);
		int nT = 1;
		std::ostrstream os(s_m_TempBuff + nLen, eTempBuffSize - nLen);
		CPlayerLogic::ForTaskType enType = m_rNPC.GetPlayerLogic()->GetForTaskType();
		switch(enType)
		{
		case CPlayerLogic::eRand:
			nT = DefTalkRand(os, target, nFinish, nDoing, nAccept);
			break;
		case CPlayerLogic::eList:
			nT = GetTaskList(os, target, true, nFinish, nDoing, nAccept);
			nT = GetTaskList(os, target, false, nFinish, nDoing, nAccept);
			break;
		case CPlayerLogic::eCircle:
			break;
		default:
			break;
		}
		/*if(0 != nT)
		{
		nRetType = 1;
		break;
		}*/
		os<<"</body>"<<endl;
		os<<"</html>"<<'\0';
		/*std::ofstream of("OK.text");
		of<<s_m_TempBuff;*/
		nRet = 0;
	}while(false);
	/*switch(nRetType)
	{
	case 0:
	g_region->ShowHtmlDialog(&m_rNPC, target, s_m_TempBuff, NULL, NULL, 1);
	break;
	default:
	g_region->ShowHtmlDialog(&m_rNPC,target,R(RES_DEFAULT_TALK));
	break;
	}*/
	g_region->ShowHtmlDialog(&m_rNPC, target, s_m_TempBuff, NULL, NULL, 1);
	if (nAccept > 0)
	{
		target->GetPlayerLogic()->GetCNewPlayerGuide()->NPCTaskTalk();
	}

	switch (m_rNPC.m_npcInfo->Id)
	{
	case 5:
		if (target->GetPlayerLogic()->IsDoingTask(11004))
		{
			target->GetPlayerLogic()->GetCNewPlayerGuide()->BuyItem1();
		}
		break;
	case 29:
		target->GetPlayerLogic()->GetCNewPlayerGuide()->LifeSkill1();
		break;
	default:
		break;
	}
	return nRet;
}

int CNpcTaskLogicExt::SendList(CRegionCreature *target)
{
	int nRet = 1;
	//const int c_nBuffSize = 2048;
	//static char szBuff[c_nBuffSize];

	int nFinish, nDoing, nAccept;
	std::ostrstream os(s_m_TempBuff, eTempBuffSize);
	int nRetType = 0;
	do
	{
		os<<"<html>"<<endl;
		os<<"<head>"<<endl;
		os<<"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\"/>"<<endl;
		os<<"</head>"<<endl;
		os<<"<body>"<<endl;
		int nT = GetTaskList(os, target, true, nFinish, nDoing, nAccept);
		if(0 != nT)
		{
			nRetType = 1;
			break;
		}
		nT = GetTaskList(os, target, false, nFinish, nDoing, nAccept);
		if(0 != nT)
		{
			nRetType = 1;
			break;
		}

		nRet = 0;
	}while(false);
	switch(nRetType)
	{
	case 0:
		os<<"</body>"<<endl;
		os<<"</html>"<<'\0';
		g_region->ShowHtmlDialog(&m_rNPC, target, s_m_TempBuff, NULL, NULL, 1);
		break;
	default:
		g_region->ShowHtmlDialog(&m_rNPC,target,R(RES_DEFAULT_TALK));
		break;
	}
	return nRet;
}

int CNpcTaskLogicExt::GetTaskList(std::ostrstream &os, CRegionCreature *target, bool bMain, int &nFinish, int &nDoing, int &nAccept, bool bWriteText)
{
	int nRet = 1;
	const int c_nMaxTask = 20;
	int nAllNum = 0;
	int nCanFinish = 0;
	int nCanAccept = 0;

	STaskDesc* CanFinish[c_nMaxTask];
	STaskDesc* Ding[c_nMaxTask];
	STaskDesc* CanAccept[c_nMaxTask];

	SDSet& sd = GetSDSet();
	sd.clear();

	nCanFinish = GetCanFinishShowTask(target, CanFinish, c_nMaxTask, bMain);
	nAllNum += nCanFinish;
	nDoing = GetDoingShowTask(target, Ding, c_nMaxTask, bMain);
	nAllNum += nDoing;
	nCanAccept = GetAcceptShowTask(target, CanAccept, c_nMaxTask, bMain);
	nAllNum += nCanAccept;

	nFinish = nCanFinish;
	nAccept = nCanAccept;
	do
	{
		
		const char* szCanFinish = R(MSG_TASK_WHICHCANFINISH);//"可完成的任务";
		const char* szDoing = R(MSG_TASK_WHICHONPROGRESS);//"正在做的任务";
		const char* szCanAccept = R(MSG_TASK_WHICHCANACCEPT);//"可接受的任务";


		if (!bWriteText)
		{
			nRet = 0;
			break;
		}

		if(0 == nAllNum)
		{
			break;
		}

		int i;
		if(0 != nCanFinish)
		{
			os<<"<p><font color=\"#ABA615\">";
			os<<szCanFinish;
			if (bMain)
			{
				os<<"【主线】";
			}
			os<<"</font></p>"<<endl;
			for(i = 0; i < nCanFinish; i++)
			{
				os<<"<p><a href=\"rs://_TA_"<<CanFinish[i]->id<<"\">"<<CanFinish[i]->name<<"</a></p>"<<endl;
			}
			os<<"<p>&nbsp;</p>"<<endl<<"<p>&nbsp;</p>"<<endl;

		}

		if(0 != nCanAccept)
		{
			os<<"<p><font color=\"#ABA615\">";
			os<<szCanAccept;
			if (bMain)
			{
				os<<"【主线】";
			}
			os<<"</font></p>"<<endl;
			for(i = 0; i < nCanAccept; i++)
			{
				os<<"<p><a href=\"rs://_TA_"<<CanAccept[i]->id<<"\">"<<CanAccept[i]->name<<"</a></p>"<<endl;
			}
		}

		if(0 != nDoing)
		{
			os<<"<p><font color=\"#ABA615\">";
			os<<szDoing;
			if (bMain)
			{
				os<<"【主线】";
			}
			os<<"</font></p>"<<endl;
			for(i = 0; i < nDoing; i++)
			{
				os<<"<p><a href=\"rs://_TA_"<<Ding[i]->id<<"\">"<<Ding[i]->name<<"</a></p>"<<endl;
			}
			os<<"<p>&nbsp;</p>"<<endl<<"<p>&nbsp;</p>"<<endl;
		}

		nRet = 0;
	}while(false);
	return nRet;
}

int CNpcTaskLogicExt::ProcessTask(CRegionCreature *target, const char *str)
{
	int nRet = 1;
	do
	{
		if(NULL == str)
		{
			break;
		}
		if('_' != str[0] || 'T' != str[1] || 'A' != str[2] || '_' != str[3])
		{
			break;
		}
		int nID = atoi(str+4);
		STaskDesc* pDesc = FindTaskInFinish(nID);
		if(NULL != pDesc)
		{
			nRet = ProcessFinishTask(target, pDesc);
			if(0 == nRet)
			{
				break;
			}
		}

		pDesc = FindTaskInAccept(nID);
		if(NULL != pDesc)
		{
			nRet = ProcessAcceptTask(target, pDesc);
			break;
		}
	}while(false);
	return nRet;
}

int CNpcTaskLogicExt::ProcessFinishTask(CRegionCreature *target, STaskDesc* pDesc)
{
	int nRet = 1;
	do
	{
		if(NULL == pDesc)
		{
			break;
		}
		long ret = target->CheckTaskFinishState(pDesc, &m_rNPC, false);

		if(TASK_STATE_CAN_FINISH == ret)
		{
			if(!m_rNPC.IsCircleTaskNpc())
			{
				//m_rNPC.ShowHtmlDialog(target, pDesc->npc2AfterFinishTalk.c_str());
				g_region->ShowHtmlDialog(&m_rNPC, target, pDesc->npc2AfterFinishTalk.c_str(), NULL, NULL, 0, pDesc->id, 2);

				target->GetPlayerLogic()->GetCNewPlayerGuide()->NPCFinishTask();

			}
			nRet = 0;
			break;
		}else if(TASK_STATE_DOING == ret)
		{
			nRet = ProcessDingTask(target, pDesc);
			break;
		}
	}while(false);
	return nRet;
}

int CNpcTaskLogicExt::ProcessAcceptTask(CRegionCreature *target, STaskDesc* pDesc)
{
	int nRet = 1;
	do
	{
		long ret;

		ret = target->CheckTaskAcceptState(pDesc, &m_rNPC);
		switch(ret)
		{
		case TASK_STATE_DOING:
			nRet = ProcessDingTask(target, pDesc);
			break;

		case TASK_STATE_CAN_ACCEPT:
			//m_rNPC.ShowHtmlDialog(target, pDesc->npc1PreAcceptTalk.c_str());
			g_region->ShowHtmlDialog(&m_rNPC, target, pDesc->npc1PreAcceptTalk.c_str(), NULL, NULL, 0, pDesc->id, 1);
			//target->GetPlayerLogic()->GetCNewPlayerGuide()->NPCFinishTask();
			nRet = 0;
			break;
		default:
			break;
		}

	}while(false);
	return nRet;
}

bool CNpcTaskLogicExt::IsInFinishTask(short nID)
{
	bool bRet = false;
	do 
	{
		STaskDesc* pTemp = NULL;
		for(size_t i=0; i < m_rNPC.m_finishTask.size(); i++)
		{
			pTemp = m_rNPC.m_finishTask[i];
			if (NULL == pTemp)
			{
				continue;
			}
			if (pTemp->id == nID)
			{
				bRet = true;
				break;
			}
		}
	} while (false);
	return bRet;
}

bool CNpcTaskLogicExt::IsInAcceptTask(short nID)
{
	bool bRet = false;
	do 
	{
		STaskDesc* pTemp = NULL;
		for(size_t i=0; i < m_rNPC.m_acceptTask.size(); i++)
		{
			pTemp = m_rNPC.m_acceptTask[i];
			if (NULL == pTemp)
			{
				continue;
			}
			if (pTemp->id == nID)
			{
				bRet = true;
				break;
			}
		}
	} while (false);
	return bRet;
}

int CNpcTaskLogicExt::ProcessDingTask(CRegionCreature *target, STaskDesc* pDesc)
{
	int nRet = 1;
	do 
	{
		STaskInfo* info = target->m_task.FindTask(pDesc->id);
		if(NULL == info)
		{
			break;
		}
		if(m_rNPC.IsCircleTaskNpc())
		{
			switch(pDesc->circleTask)
			{
			case TASK_CIRCLE_ITEM:
				{
					SItemBase* pBase= g_region->m_pItemManager->GetItemFromIndex(info->m_finditemID);						
					g_region->ShowHtmlDialog(&m_rNPC, target, pDesc->npc1InTaskTalk.c_str(), "FIND_ITEM", (char*)pBase->name);
				}
				break;
			case TASK_CIRCLE_FINDNPC:
				{
					SNpc* pNpc = g_TableNpc.FindNpc(info->m_findnpcID);
					g_region->ShowHtmlDialog(&m_rNPC, target, pDesc->npc1InTaskTalk.c_str(), "FIND_NPC", (char*)pNpc->Name.c_str());
				}
				break;
			case TASK_CIRCLE_BATTLE:
				{
					SNpc* pNpc = g_TableNpc.FindNpc(info->m_killmonsterID);
					g_region->ShowHtmlDialog(&m_rNPC, target, pDesc->npc1InTaskTalk.c_str(), "KILL_MONSTER", (char*)pNpc->Name.c_str());
				}
				break;
			default:
				break;
			}
		}
		else
		{
			STaskDesc *pDesc = info->GetDesc();
			if (NULL == pDesc)
			{
				break;
			}
			char cType = 4;
			if(pDesc->IsMainTask())
			{
				cType = 3;
			}
			bool bIn = IsInFinishTask(pDesc->id);
			const char* pTalk = NULL;
			if (bIn)
			{
				pTalk = pDesc->npc2InTaskTalk.c_str();
			}
			else
			{
				pTalk = pDesc->npc1InTaskTalk.c_str();
			}
			g_region->ShowHtmlDialog(&m_rNPC, target, pTalk, NULL, NULL, 0, 0, cType);

		}
		nRet = 0;
	} while (false);
	return nRet;
}

STaskDesc* CNpcTaskLogicExt::FindTaskInAccept(int nID)
{
	STaskDesc* pRet = NULL;
	short sID = (short)(nID);

	for(size_t i=0; i<m_rNPC.m_acceptTask.size(); i++)
	{
		if(NULL == m_rNPC.m_acceptTask[i])
		{
			continue;
		}
		if(m_rNPC.m_acceptTask[i]->id == sID)
		{
			pRet = m_rNPC.m_acceptTask[i];
			return pRet;
		}
	}
	return pRet;
}


STaskDesc* CNpcTaskLogicExt::FindTaskInFinish(int nID)
{
	STaskDesc* pRet = NULL;
	short sID = (short)(nID);
	for(size_t i=0; i<m_rNPC.m_finishTask.size(); i++)
	{
		if(NULL == m_rNPC.m_finishTask[i])
		{
			continue;
		}
		if(m_rNPC.m_finishTask[i]->id == sID)
		{
			pRet = m_rNPC.m_finishTask[i];
			return pRet;
		}
	}

	return pRet;
}

CNpcTaskLogicExt::SDSet& CNpcTaskLogicExt::GetSDSet()
{
	return m_SDSet;
}

int CNpcTaskLogicExt::GetCanFinishShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum, bool bMain)
{
	int nRet = 0;
	long ret;

	SDSet& sd = GetSDSet();
	STaskDesc* pTemp = NULL;

	for(size_t i=0; i<m_rNPC.m_finishTask.size(); i++)
	{
		pTemp = m_rNPC.m_finishTask[i];
		if (NULL == pTemp)
		{
			continue;
		}
		bool bOK = true;
		if (bMain)
		{
			bOK = pTemp->m_TaskGroupID < 40;
		}
		else
		{
			bOK = pTemp->m_TaskGroupID >= 40;
		}

		if(!bOK)
		{
			continue;
		}

		ret = target->GetPlayerLogic()->CanFinishState(m_rNPC.m_finishTask[i], &m_rNPC);
		if(ret == TASK_STATE_CAN_FINISH)
		{
			/*if(!m_rNPC.IsCircleTaskNpc())
			{*/
				if(nRet < nBufNum)
				{
					ppBuff[nRet++] = pTemp;
					sd.insert(pTemp);
				}
			//}
		}
	}
	return nRet;
}

int CNpcTaskLogicExt::GetDoingShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum, bool bMain)
{
	int nRet = 0;
	long ret;

	SDSet& sd = GetSDSet();
	SDSet::iterator iter;
	STaskDesc* pTemp = NULL;

	for(size_t i=0; i < m_rNPC.m_finishTask.size(); i++)
	{
		pTemp = m_rNPC.m_finishTask[i];
		if (NULL == pTemp)
		{
			continue;
		}
		bool bOK;
		if (bMain)
		{
			bOK = pTemp->m_TaskGroupID < 40;
		}
		else
		{
			bOK = pTemp->m_TaskGroupID >= 40;
		}
		if(!bOK)
		{
			continue;
		}
		iter = sd.find(pTemp);
		if(sd.end() != iter)
		{
			continue;
		}
		ret = target->GetPlayerLogic()->CanFinishState(pTemp, &m_rNPC);
		if(ret != TASK_STATE_DOING)
		{
			continue;
		}
		if(nRet < nBufNum)
		{
			sd.insert(pTemp);
			ppBuff[nRet++] = pTemp;
		}
	}

	// accept task list 
	for(size_t i=0; i<m_rNPC.m_acceptTask.size(); i++)
	{
		pTemp = m_rNPC.m_acceptTask[i];
		if (NULL == pTemp)
		{
			continue;
		}
		bool bOK;
		if (bMain)
		{
			bOK = pTemp->m_TaskGroupID < 40;
		}
		else
		{
			bOK = pTemp->m_TaskGroupID >= 40;
		}
		if(!bOK)
		{
			continue;
		}

		iter = sd.find(pTemp);
		if(sd.end() != iter)
		{
			continue;
		}

		ret = target->CheckTaskAcceptState(pTemp, &m_rNPC);

		if(ret == TASK_STATE_DOING)
		{
			if(nRet < nBufNum)
			{
				ppBuff[nRet++] = pTemp;
			}
		}
	}

	return nRet;
}

int CNpcTaskLogicExt::GetAcceptShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum, bool bMain)
{
	int nRet = 0;
	long ret;
	STaskDesc* pTemp = NULL;
	SDSet& sd = GetSDSet();
	SDSet::iterator iter;
	do 
	{
		for(size_t i=0; i<m_rNPC.m_acceptTask.size(); i++)
		{
			pTemp = m_rNPC.m_acceptTask[i];
			if (NULL == pTemp)
			{
				continue;
			}
			bool bOK;
			if (bMain)
			{
				MainStoryData* pData = CMainStoryMgr::GetMgr().GetMainStoryByTaskID(pTemp->id);
				if (NULL == pData)
				{
					continue;
				}
				int nMainID = target->GetPlayerLogic()->GetCurMainStoryID();
				if (nMainID != pData->nDataID)
				{
					continue;
				}

				bOK = pTemp->m_TaskGroupID < 40;
			}
			else
			{
				bOK = pTemp->m_TaskGroupID >= 40;
			}
			if(!bOK)
			{
				continue;
			}
			pTemp = m_rNPC.m_acceptTask[i];
			if (NULL == pTemp)
			{
				continue;
			}
			iter = sd.find(pTemp);
			if(sd.end() != iter)
			{
				continue;
			}
			ret = target->CheckTaskAcceptState(m_rNPC.m_acceptTask[i], &m_rNPC);

			if(ret == TASK_STATE_CAN_ACCEPT)
			{
				if(nRet < nBufNum)
				{
					ppBuff[nRet++] = pTemp;
				}
			}
		}
	} while (false);

	return nRet;
}

int CNpcTaskLogicExt::GetShowTask(CRegionCreature *target, STaskDesc** ppBuff, int nBufNum)
{
	int nRet = 0;
	long ret;
	for(size_t i=0; i<m_rNPC.m_finishTask.size(); i++)
	{
		ret = target->GetPlayerLogic()->CanFinishState(m_rNPC.m_finishTask[i], &m_rNPC);
		if(ret == TASK_STATE_CAN_FINISH)
		{
			if(!m_rNPC.IsCircleTaskNpc())
			{
				if(nRet < nBufNum)
				{
					ppBuff[nRet++] = m_rNPC.m_finishTask[i];
				}
			}
		}
	}
	for(size_t i=0; i < m_rNPC.m_finishTask.size(); i++)
	{
		ret = target->GetPlayerLogic()->CanFinishState(m_rNPC.m_finishTask[i], &m_rNPC);
		if(ret == TASK_STATE_DOING)
		{
			if(nRet < nBufNum)
			{
				ppBuff[nRet++] = m_rNPC.m_finishTask[i];
			}
		}
	}

	// accept task list 
	for(size_t i=0; i<m_rNPC.m_acceptTask.size(); i++)
	{
		ret = target->CheckTaskAcceptState(m_rNPC.m_acceptTask[i], &m_rNPC);

		if(ret == TASK_STATE_DOING)
		{
			if(nRet < nBufNum)
			{
				ppBuff[nRet++] = m_rNPC.m_acceptTask[i];
			}
		}
	}

	for(size_t i=0; i<m_rNPC.m_acceptTask.size(); i++)
	{
		ret = target->CheckTaskAcceptState(m_rNPC.m_acceptTask[i], &m_rNPC);

		if(ret == TASK_STATE_CAN_ACCEPT)
		{
			if(nRet < nBufNum)
			{
				ppBuff[nRet++] = m_rNPC.m_acceptTask[i];
			}
		}
	}
	return nRet;
}

char* CNpcTaskLogicExt::GetDefTalk()
{
	return m_pTalk;
}

int	CNpcTaskLogicExt::GetDefTalkLen()
{
	return m_nTalkLen;
}

void CNpcTaskLogicExt::AcceptTaskByID(CRegionCreature *target, short nID)
{

	do
	{
		if(NULL == target)
		{
			break;
		}
		STaskDesc* pDesc = FindTaskInAccept(nID);
		if(NULL == pDesc)
		{
			break;
		}
		if(target->CanAcceptTask(pDesc, true, &m_rNPC))
		{
			//ERR1("AcceptTaskByID %s\n", target->m_core.Name.c_str());
			target->AcceptTask(pDesc, &m_rNPC);
			
			switch (pDesc->id)
			{
			case 11001:
				//if (target->GetPlayerLogic()->IsHaveItem(16000))
				{
					target->GetPlayerLogic()->GetCNewPlayerGuide()->UsePet1();
				}
				break;
			case 11005:
				target->GetPlayerLogic()->GetCNewPlayerGuide()->FaBao1 ();
				break;
			default:
				break;
			}

		}

		//if(target->TaskProcess(pDesc, pDesc->acceptFlag.c_str(), &link))
		//{
		//	//m_rNPC.ShowHtmlDialog(target,link);
		//}
	}while(false);
}

void CNpcTaskLogicExt::FinishTaskByID(CRegionCreature *target, short nID, long nIndex)
{
	do
	{
		if(NULL == target)
		{
			break;
		}
		STaskDesc* pDesc = FindTaskInFinish(nID);
		if(NULL == pDesc)
		{
			break;
		}
		target->CheckTaskFinishState(pDesc, &m_rNPC, true, nIndex);
	}while(false);
}

////////////////////////
//class CMonsterLogicExt
///////////////////////
CMonsterLogicExt::CMonsterLogicExt(CRegionCreature& rPlayer):m_rPlayer(rPlayer), m_dwGetSkinID(eEmptyDBID), m_dwKillerDBID(eEmptyDBID),
m_dwGetMineID(eEmptyDBID), m_dwMineTime(0), m_dwGetMedicineID(eEmptyDBID), m_dwMedicineTime(0), 
m_nLifeSkillExp(0), m_dwCloseTime(0), m_dwEraseTime(0), m_dwSkinTime(0)
{
}

CMonsterLogicExt::~CMonsterLogicExt()
{
}


EItemSelectionLevel GetSelLevel(int lev);


//Tianh  掉落
CPlayerLogic::LiveType CMonsterLogicExt::DropItemS(CRegionCreature *killer, CRegionSceneFB* pFB)
{
	CPlayerLogic::LiveType nRet = CPlayerLogic::eDead;
	ItemList& il = GetItemList();
	SItemID item;
	CRegionCreature *pRelKiller = NULL;
	int nRetType = 1;
	do
	{
		if(NULL == killer)
		{
			nRetType = 1;
			break;
		}

		if(killer->IsUserPet() || killer->IsCallNpc())
		{
			pRelKiller = killer->m_master;
			if (NULL == pRelKiller)
			{
				m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
				nRetType = 1;
				break;
			}
		}
		else
		{
			pRelKiller = killer;
		}

		il.clear();
		//bool bFirstDropItem = true;

		for(int i=0; i<m_rPlayer.m_npcInfo->ItemNum; i++)
		{
			if(m_rPlayer.m_npcInfo->ItemDropRate[i] < 1)
			{
				double dR = m_rPlayer.m_npcInfo->ItemDropRate[i] *
					g_cfg.game.item_drop_rate *
					ComputeGoldRate(pRelKiller->m_core.Lev, m_rPlayer.m_core.Lev);
				if (!GetProb(dR))
				{
					continue;
				}
			}
			EItemColor color = ItemColor_White;
			if(GetProb(m_rPlayer.m_npcInfo->ItemGreenRate*g_cfg.game.item_green_rate))
			{
				color = ItemColor_Green;
				if(GetProb(m_rPlayer.m_npcInfo->ItemBlueRate*g_cfg.game.item_blue_rate))	
				{
					color = ItemColor_Blue;
					// golden item
					if(GetProb(m_rPlayer.m_npcInfo->ItemGoldRate))	
					{
						color = ItemColor_Golden;
					}
				}
			}

			EItemBind bindType = IB_NotBind;
			switch(m_rPlayer.m_npcInfo->ItemBindType)
			{
			case 0: bindType = IB_NotBind;break;
			case 1: bindType = IB_BindWhenEquip;break;
			case 2: bindType = IB_BindWhenPick;break;
			case 3: //高级模式：金装，蓝装拾取绑定，其他不绑定
				{
					if(color == ItemColor_Golden)
					{
						bindType = IB_BindWhenPick;
					}
					else if(color == ItemColor_Blue)
					{
						if(GetProb(0.2)) bindType = IB_NotBind;
						else bindType = IB_BindWhenPick;

					}
					break;
				}
			default: bindType = IB_NotBind;
			}

			item = g_region->m_pItemManager->CreateItem(m_rPlayer.m_npcInfo->Item[i],color,0,bindType);
			//if(!ItemCanNpcTrade(item.type)) continue;
			if(ItemID_IsValid(item) && !ItemIsPet(item))
			{
				il.push_back(item);
			}
		}
		if(0 != m_rPlayer.m_npcInfo->dwSkinID)
		{
			m_SkinItem = g_region->m_pItemManager->CreateItem(m_rPlayer.m_npcInfo->dwSkinID, 
				ItemColor_White, 0, IB_NotBind);
			if (ItemIsPet(m_SkinItem)) ItemID_SetInvalid(m_SkinItem);
		}

		/*if (NULL != killer->m_pTeam)
		{
			int nNum = killer->m_pTeam->mData.Size();
			if (0 != nNum)
			{
				TeamPickType type = killer->m_pTeam->mData.GetPickType();
				if (eOrderPick == type)
				{
					killer->m_pTeam->StartOrderPick();
				}
			}
		}*/

		if(NULL == pFB)
		{
			nRetType = 0;
			break;
		}

		static int blueNum=0,greenNum=0,dropNum=0,goldNum=0;
		SNpc *info = m_rPlayer.m_npcInfo; 
		if(NULL == info) 
		{
			nRetType = 0;
			break;
		}

		for(int i=0; i<info->RandItemNum; i++)
		{
			/*if(!GetProb(info->RandItemDropRate[i])) continue;*/
			if(info->RandItemDropRate[i] < 1 &&
				!GetProb(info->RandItemDropRate[i]*g_cfg.game.item_drop_rate* 
				ComputeGoldRate(pRelKiller->m_core.Lev, m_rPlayer.m_core.Lev)))
			{
				continue;
			}

			//float blueRate = 0.0f;
			float goldRate = 0.0f;
			//switch(pFB->m_curDif)
			//{
			//	case 2: blueRate = 0.05; goldRate = 0; break;
			//	case 3: blueRate = 0.05; goldRate = 0; break;
			//	case 4: blueRate = 0.05; goldRate = 0; break;
			//}
			//

			if(pFB->m_curDif == 4)
			{
				goldRate = 0.02;
			}

			//添加金装掉落的等级衰减
			goldRate = goldRate * (pFB->m_curLev + 20) / pRelKiller->m_core.Lev;
			if(goldRate > 0.1) goldRate = 0.1;
			if(goldRate < 0) goldRate = 0;

			EItemColor color = ItemColor_White;

			if(info->Boss)
			{
				//blueRate *= 5;
				//goldRate = 0.1;

				/*EItemColor */color = ItemColor_Green; //这里设置BOSS的掉落最低是绿装

				if(GetProb(info->ItemBlueRate*g_cfg.game.item_blue_rate))
				{
					color = ItemColor_Blue;
					if (GetProb(info->ItemGoldRate))
					{
						color = ItemColor_Golden;
					}
				}

			}
			else
			{
				if(GetProb(info->ItemGreenRate*g_cfg.game.item_green_rate))
				{
					color = ItemColor_Green;
					if(GetProb(info->ItemBlueRate*g_cfg.game.item_blue_rate))	
					{
						color = ItemColor_Blue;
						// golden item
						if(GetProb(info->ItemGoldRate))	
						{
							color = ItemColor_Golden;
						}
					}
				}
			}

			int dropItemLev = pFB->m_curLev;
			//if(color == ItemColor_Blue)
			//{
			//	dropItemLev = pFB->m_info->reqLev[pFB->m_curDif-1];
			//	blueNum++;
			//}
			//else if(color == ItemColor_Golden)
			//{
			//	dropItemLev = pFB->m_info->reqLev[pFB->m_curDif-1];
			//	goldNum++;
			//}
			//else
			//{
			//	dropItemLev = pFB->m_info->reqLev[pFB->m_curDif];
			//	greenNum++;
			//}

			EItemBind bindType = IB_NotBind;
			switch(info->ItemBindType)
			{
			case 0: bindType = IB_NotBind;break;
			case 1: bindType = IB_BindWhenEquip;break;
			case 2: bindType = IB_BindWhenPick;break;
			case 3: //高级模式：金装，蓝装拾取绑定，其他不绑定
				{
					if(color == ItemColor_Golden)
					{
						bindType = IB_BindWhenPick;
					}
					else if(color == ItemColor_Blue)
					{
						if(GetProb(0.2)) bindType = IB_NotBind;
						else bindType = IB_BindWhenPick;

					}
					break;
				}
			default: bindType = IB_NotBind;
			}
			//

			SItemSelector sel;
			sel.level = GetSelLevel(dropItemLev);
			sel.type  = (EItemSelectionType)info->RandItem[i];
			vector<DWORD> id;
			g_region->m_pItemManager->m_pSelectionTable->FindSelection(sel,id);
			long nSize = (long)(id.size());
			if(nSize == 0) continue;
			long idx = GetRand(nSize-1,0);

			item = g_region->m_pItemManager->CreateItem(id[idx], color, 0, bindType);

			if(ItemID_IsValid(item) && !ItemIsPet(item))
			{
				if (ItemIsBlue(item))
				{
					std::string itemString;
					item.SaveToReadableString(g_region->m_pItemManager, itemString);
					ERR1("FB drop a BLUE item: item=%s\n", itemString.c_str());
					ERR4("FB item drop=%d,gold=%d,blue=%d,green=%d\n",dropNum,goldNum,blueNum,greenNum);
				}
				else if(ItemIsGolden(item))
				{
					std::string itemString;
					item.SaveToReadableString(g_region->m_pItemManager, itemString);
					ERR1("FB drop a GOLD item: item=%s\n", itemString.c_str());
					ERR4("FB item drop=%d,gold=%d,blue=%d,green=%d\n",dropNum,goldNum,blueNum,greenNum);
				}

				il.push_back(item);
			}
		}
		nRetType = 0;
	}while(false);
	switch(nRetType)
	{
	case 0:
		//m_pKiller = killer;
		if (pRelKiller->IsUser())
		{
			m_dwKillerDBID = pRelKiller->m_userInfo->m_userId;
		}
		else
		{
			m_dwKillerDBID = eEmptyDBID;
		}

		m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eHaveItem);
		SetEraseTime(rtGetMilliseconds() + m_rPlayer.m_npcInfo->dwDiwKeetTime * 1000);
		nRet = UpdateItemS();
		UpdateTaskKillMonsterNum(pRelKiller);
		break;
	default:
		m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
		break;
	}
	return nRet;
}

void CMonsterLogicExt::UpdateTaskKillMonsterNum(CRegionCreature *killer)
{
	const int c_BufNum = 128;
	
	STaskDesc *pBuff[c_BufNum];
	const int c_nMaxTeamNum = 10;
	//CRegionCreature *pCreBuff[c_nMaxTeamNum];

	do 
	{
		if (NULL == killer)
		{
			break;
		}
		int nNum = CRSLogicExt::GetRSLogicExt().GetNPCTaskData(m_rPlayer.m_npcInfo->Id, pBuff, c_BufNum);
		int i;
		for(i = 0; i < nNum; i++)
		{
			if (NULL == pBuff[i])
			{
				continue;
			}
			if(NULL == killer->m_pTeam)
			{
				UpdateTaskKillMonsterNumOnce(killer, pBuff[i]->id);
				continue;
			}

			CTeamData &td = killer->m_pTeam->mData;
			int nTeamNum = td.Size();
			if(0 == nTeamNum)
			{
				UpdateTaskKillMonsterNumOnce(killer, pBuff[i]->id);
				continue;
			}
			STeamData* pData = td.GetTeamData();
			for (int j = 0; j < nTeamNum; j++)
			{
				CRegionUser* pUser = g_region->FindUser(pData[j].mDBID);
				if(NULL == pUser)
				{
					continue;
				}
				CRegionCreature *tmp = pUser->m_dummy;
				if(NULL == tmp)
				{
					continue;
				}
				
				if(tmp->Distance(m_rPlayer.m_pos) > 800)
				{
					continue;
				}
				UpdateTaskKillMonsterNumOnce(tmp, pBuff[i]->id);
			}
		}
	} while (false);
}

void CMonsterLogicExt::UpdateTaskKillMonsterNumOnce(CRegionCreature *pWho, int nTaskID)
{
	do 
	{
		const unsigned long c_dwMaxKill = 63;
		STaskInfo *info = pWho->m_task.FindTask(nTaskID);
		if(NULL == info)
		{
			break;
		}

		if(!info->FIsDoing())
		{
			break;
		}
		unsigned long dwNum = info->GetCurKillMonsterNum();
		if (dwNum >= c_dwMaxKill)
		{
			break;
		}
		info->SetCurKillMonsterNum(dwNum + 1);
		STaskDesc *pDesc = info->GetDesc();
		if (NULL == pDesc)
		{
			break;
		}
		pWho->CheckTaskFinishState(pDesc, NULL, false);
		pWho->UpdateToClient(NET_CRE_UPDATE_TASK);
	} while (false);
}

CPlayerLogic::LiveType CMonsterLogicExt::DropMineMedicineS(/*CRegionCreature *killer*/)
{
	CPlayerLogic::LiveType nRet = CPlayerLogic::eHaveSkin;
	ItemList& il = GetItemList();
	SItemID item;
	//int nRetType = 1;
	do
	{
		il.clear();
		int i = 0;

		int nR[MAX_NPC_DROP_ITEM];
		int nS = 0;

		for(i = 0; i < m_rPlayer.m_npcInfo->ItemNum; i++)
		{
			int nTemp = (int)(m_rPlayer.m_npcInfo->ItemDropRate[i] * 100);
			nR[i] = nS + nTemp;
			nS = nR[i];
		}

		int nRand = ::rand() % 100;
		bool bFind = false;
		for(i = 0; i < m_rPlayer.m_npcInfo->ItemNum; i++)
		{
			if(nRand < nR[i])
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			break;
		}
		EItemColor color = ItemColor_White;
		if(GetProb(m_rPlayer.m_npcInfo->ItemGreenRate*g_cfg.game.item_green_rate))
		{
			color = ItemColor_Green;
			if(GetProb(m_rPlayer.m_npcInfo->ItemBlueRate*g_cfg.game.item_blue_rate))	
			{
				color = ItemColor_Blue;
				// golden item
				if(GetProb(m_rPlayer.m_npcInfo->ItemGoldRate))	
				{
					color = ItemColor_Golden;
				}
			}
		}

		EItemBind bindType = IB_NotBind;
		switch(m_rPlayer.m_npcInfo->ItemBindType)
		{
		case 0: bindType = IB_NotBind;break;
		case 1: bindType = IB_BindWhenEquip;break;
		case 2: bindType = IB_BindWhenPick;break;
		case 3: //高级模式：金装拾取绑定，蓝装20%概率装备绑定、80概率拾取绑定，其他不绑定
			{
				if(color == ItemColor_Golden)
				{
					bindType = IB_BindWhenPick;
				}
				else if(color == ItemColor_Blue)
				{
					if(GetProb(0.2)) bindType = IB_NotBind;
					else bindType = IB_BindWhenPick;
				}
				break;
			}
		default: bindType = IB_NotBind;
		}

		item = g_region->m_pItemManager->CreateItem(m_rPlayer.m_npcInfo->Item[i],color,0,bindType);
		if(ItemID_IsValid(item) && !ItemIsPet(item))
		{
			il.push_back(item);
		}

	}while(false);

	return nRet;
}

int CMonsterLogicExt::DropTaskLifeItem()
{
	int nRet = 1;
	ItemList& il = GetItemList();
	SItemID item;
	CRegionCreature* pGet = NULL;
	do
	{
		if(IsMineNPC())
		{
			pGet = GetGetingMine();
		}
		else if(IsMedicineNPC())
		{
			pGet = GetGetingMedicine();
		}
		if(NULL == pGet)
		{
			break;
		}
		il.clear();
		int i;
		for(i = 0; i < m_rPlayer.m_npcInfo->nTaskNum; i++)
		{
			if(m_rPlayer.m_npcInfo->TaskId[i] && 
				pGet->m_task.IsTaskAccept(m_rPlayer.m_npcInfo->TaskId[i]))
			{
				if(pGet->GetItemNum(m_rPlayer.m_npcInfo->TaskItemId[i], ItemColor_White) >= m_rPlayer.m_npcInfo->TaskItemLimit[i])
				{
					continue;
				}
				item = g_region->m_pItemManager->CreateItem(m_rPlayer.m_npcInfo->TaskItemId[i]);
				if(ItemID_IsValid(item) && !ItemIsPet(item))
				{
					il.push_back(item);
				}
			}
		}
		if(il.empty())
		{
			break;
		}
		nRet = 0;
	}while(false);
	return nRet;
}

int CMonsterLogicExt::InTeamCanPick(CRegionCreature *Picker)
{
	int nRet = 3;
	do 
	{
		if (NULL == Picker /*|| NULL == m_pKiller*/)
		{
			break;
		}
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		if (CPlayerLogic::eDropSkin == type)
		{
			nRet = 4;//采集到的不受限
			break;
		}

		CRegionCreature *pKiller = NULL;
		CRegionUser *user = g_region->FindUser(m_dwKillerDBID);
		if(NULL == user) 
		{
			break;
		}

		pKiller = user->m_dummy;
		if (NULL == pKiller)
		{
			break;
		}

		if (NULL == pKiller->m_pTeam)
		{
			nRet = 2;//没组队
			break;
		}

		int nNum = pKiller->m_pTeam->mData.Size();
		if (0 == nNum)
		{
			nRet = 2;//没组队
			break;
		}
		TeamPickType PickType = pKiller->m_pTeam->mData.GetPickType();
		if (eFreePick == PickType)
		{
			nRet = 0;//自由拾取
			break;
		}
		if (eOrderPick == PickType)
		{
			nRet = 5;//随机拾取
			break;
		}
		if (!pKiller->IsUser())
		{
			nRet = 3;//未知错误
			break;
		}
		
		if (eCaptainPick == PickType)
		{
			if (pKiller->m_pTeam->mData.GetCaptain() == Picker->m_userInfo->m_userId)
			{
				nRet = 0;
			}
			else
			{
				nRet = 1;//组队不能检
			}
			break;
		}
		else
		{
			if (pKiller->m_pTeam->GetOrderPickDBID() == Picker->m_userInfo->m_userId)
			{
				nRet = 0;
			}
			else
			{
				nRet = 1;//组队不能检
			}
			break;
		}

	} while (false);
	return nRet;
}

int CMonsterLogicExt::PickMyItem(CRegionCreature *Picker, unsigned long dwID)
{
	int nRet = 0;
	do
	{
		if(NULL == Picker)
		{
			nRet = 1;//未知错误
			break;
		}
		CRegionCreature *RePicker = Picker;
		int nT = InTeamCanPick(Picker);
		if(1 == nT || 3 == nT)
		{
			break;//组队不能检
		}
		else if(2 == nT)
		{
			if (!Picker->IsUser())
			{
				nRet = 1;//未知错误
				break;
			}
			if(Picker->m_userInfo->m_userId != m_dwKillerDBID)
			{
				nRet = 5;//该怪主要不是你打死的
				break;
			}
		}

		CRegionCreature *cre = Picker;
		CItemContainerBase *pCon = cre->m_pItemContainer;

		DWORD dwTick = rtGetMilliseconds();

		// 检查两次捡道具的时间间隔, 如果两次检道具的时间在200毫秒内就认为检查到外挂
		if (cre->m_dwLastPickItemTime && cre->m_dwLastPickItemTime+200>dwTick)
		{
			cre->PossiblyUsingWG();
			cre->m_dwLastPickItemTime = dwTick;
		}

		// CHECK
		// 道具必须存在，并且玩家与它的距离不超出范围
		// 玩家的背包必须有空间存放

		if (cre->Distance(m_rPlayer.m_pos) > 300)
		{
			nRet = 2;//距离过远
			break;
		}
		if (cre->m_pTrade || cre->m_pPShop)
		{
			nRet = 3;//正在交易或摆摊状态
			break;
		}

		bool bCanPick = false;
		ItemList& il = GetItemList();
		ItemList::iterator iter;
		SItemID item;
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		bool bBreak = false;
		switch(type)
		{
		case CPlayerLogic::eHaveItem:
			for(iter = il.begin(); il.end() != iter; )
			{
				if(dwID == (*iter).id)
				{
					item = (*iter);
					bCanPick = true;
					iter = il.erase(iter);
					if (5 != nT)
					{
						break;
					}
					if (NULL != Picker->m_pTeam)
					{
						int nNum = Picker->m_pTeam->mData.Size();
						CRegionCreature* pBuff[MAX_TEAM_MENBER_COUNT];
						int ReNum = 0;
						int i;
						for (i = 0; i < nNum; i++)
						{
							CRegionUser* pUser = g_region->FindUser(Picker->m_pTeam->mData[i].mDBID);
							if(NULL == pUser)
							{
								continue;
							}
							CRegionCreature *tmp = pUser->m_dummy;
							if(NULL == tmp)
							{
								continue;
							}
							pBuff[ReNum++] = tmp;
						}
						
						if (0 != ReNum)
						{
							int nR = rand() % ReNum;
							RePicker = pBuff[nR];
							pCon = RePicker->m_pItemContainer;
						}
					}
					break;
				}
				else
				{
					iter++;
				}
			}
			break;
		case CPlayerLogic::eDropSkin:
			if(this->IsMineNPC() || this->IsMedicineNPC())
			{
				if(il.empty())
				{
					nRet = 13;//状态不对
					bBreak = true;
					break;
				}
				item = *(il.begin());
				il.clear();
				bCanPick = true;
			}
			else
			{
				if(dwID == m_SkinItem.id)
				{
					item = m_SkinItem;
					bCanPick = true;
				}
			}
			break;
		default:
			nRet = 9;//状态不对
			bBreak = true;
			break;
		}
		if(bBreak)
		{
			break;
		}
		if (!bCanPick)
		{
			CItemCmdBuilder_Svr::Build_r2c_pickitem_fail();
			cre->RecvCmd(g_sendCmd);
			nRet = 4;//该东西已被其它人捡走
			break;
		}

		if (item.cBind==IB_BindWhenPick)
			item.cBind = IB_Binded;

		CBag *pBag;
		EItemContainer c;
		if(ItemIsMissionItem(item) )
		{
			pBag = &(pCon->m_MissionBag);
			c = ITEM_CONTAINER_PLAYER_MISSION_BAG;
		}
		else
		{
			pBag = &(pCon->m_Bag);
			c = ITEM_CONTAINER_PLAYER_COMMON_BAG;
		}

		int page, i, j;
		if(!pBag->AddItem(item, &page, &i, &j))
		{
			if(pBag->IsFull())
			{
				Picker->SendSystemMessage(R(MSG_CLEAR_BAG_ITEM));
			}
			il.push_back(item);
			OnClosePickwindow(NULL);
			nRet = 26;//背包已满
			break;
		}
		else
		{
			RePicker->GetPlayerLogic()->OnGetItem(item, c, page, i, j);
			UpdateItemS();
		}
		if (Picker != RePicker)
		{
			nRet = 11;
		}
	}while(false);

	return nRet;
}


int CMonsterLogicExt::PickAllMyItem(CRegionCreature *Picker)
{
	int nRet = 0;
	unsigned long dwIDS[20];
	int nNum = 0;
	int i;
	int nT = 0;
	CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
	switch(type)
	{
	case CPlayerLogic::eHaveItem:
		{
			ItemList& il = GetItemList();
			ItemList::iterator iter;
			for(iter = il.begin(); il.end() != iter; iter++)
			{
				dwIDS[nNum++] = (*iter).id;
			}
			int nPickNum = 0;

			for(i = 0; i < nNum; i++)
			{
				nT = PickMyItem(Picker, dwIDS[i]);
				if(0 == nT)
				{
					nPickNum++;
				}
			}
			if(0 == nPickNum)
			{
				nRet = nT;
			}
		}
		break;
	case CPlayerLogic::eDropSkin:
		nT = PickMyItem(Picker, m_SkinItem.id);
		if(0 != nT)
		{
			nRet = nT;
		}
		break;
	default:
		nRet = 1;
		break;
	}

	return nRet;
}

int CMonsterLogicExt::SendItemList(CRegionCreature *Picker, CG_CmdPacket* pPacket)
{
	int nRet = 0;
	do
	{
		if(NULL == Picker || NULL == pPacket)
		{
			nRet = 1;
			pPacket->WriteByte(9);//未知错误
			break;
		}
		
		TObjectHash& Oh = g_factory->GetTObjectHash();
		TObjectHash::iterator it;
		it = Oh.find(m_rPlayer.GetObjectId());
		if (Oh.end() == it)
		{
			OnErase();
			nRet = 1;
			pPacket->WriteByte(9);//未知错误
			break;
		}
		//else if(2 == nT)
		//{
		//	if(Picker != m_pKiller)
		//	{
		//		nRet = 5;//该怪主要不是你打死的
		//		break;
		//	}
		//}
		bool bB = false;
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		switch(type)
		{
		case CPlayerLogic::eHaveItem:
			{
				int nT = InTeamCanPick(Picker);
				if(1 == nT || 3 == nT)
				{
					nRet = 9;
					pPacket->WriteByte(10);//你不能捡
					bB = true;
					break;//组队不能检
				}
				else if (2 == nT)
				{
					if(Picker->m_userInfo->m_userId != m_dwKillerDBID)
					{
						nRet = 5;//该怪主要不是你打死的
						pPacket->WriteByte(5);//你不能捡
						bB = true;
						break;
					}
				}

				ItemList& il = GetItemList();
				char cNum = (char)(il.size());
				if(0 == cNum)
				{
					pPacket->WriteByte(2);//失败，东西被检光了
				}
				else
				{
					pPacket->WriteByte(0);//成功，下面是数组
					pPacket->WriteByte(cNum);
					ItemList::iterator iter;
					for(iter = il.begin(); il.end() != iter; iter++)
					{
						(*iter).Serialize(pPacket);
					}
				}
			}
			break;
		case CPlayerLogic::eDropSkin:
			pPacket->WriteByte(0);//成功，下面是数组
			pPacket->WriteByte(1);
			m_SkinItem.Serialize(pPacket);
			break;
		default:
			pPacket->WriteByte(2);//失败，东西被检光了
		}
		if (bB)
		{
			break;
		}
	}while(false);

	return nRet;
}

void CMonsterLogicExt::OnRun(unsigned long dwTime)
{
	if(CPlayerLogic::eDead == m_rPlayer.GetPlayerLogic()->GetLiveType())
	{
		OnErase();
	}
	else if(IsMineNPC())
	{
		GetMineRun(dwTime);
	}
	else
	{
		if(IsMedicineNPC())
		{
			GetMedicineRun(dwTime);
		}
		else
		{
			if(!m_rPlayer.GetPlayerLogic()->IsLive())
			{
				GetSkinRun(dwTime);
				if(dwTime > m_dwEraseTime)
				{
					//OnErase();
					m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
				}
				else if (m_dwEraseTime - dwTime > 600 * 1000)
				{
					RtCoreLog().Debug("\n");
					RtCoreLog().Debug("Monster erase time error : %d \n",m_dwEraseTime - dwTime);
					RtCoreLog().Debug("\n");
					m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
				}
			}
		}
	}
}

void CMonsterLogicExt::GetSkinRun(unsigned long dwTime)
{
	if(IsBeGetingSkin())
	{
		if(dwTime > m_dwSkinTime)
		{
			CRegionCreature*  pGetSkin = GetGetingSkin();
			if (NULL != pGetSkin)
			{
				int nMySkillLevel = pGetSkin->GetPlayerLogic()->GetSkillLevel(654);
				int nNeetSkillLevel = this->GetLifeItemLevel();
				float fR = 0.8f + (nMySkillLevel - nNeetSkillLevel) * 0.1f;
				int nR = (int)(fR * 100);
				//srand(rtGetMilliseconds());   ///< 随机种子
				//int nNum =rand() % 100;
				int nNum = GetRand(99, 0);

				if(nNum < nR)
				{
					int nOneExp = (2+2*(nMySkillLevel - 1) * (1 + 0.05 * (nMySkillLevel -1 )));
					float fS = 1 + (nNeetSkillLevel - nMySkillLevel) / 3.0f;
					if(fS < 0)
					{
						fS = 0;
					}
					//int nExp = (int)(nOneExp * fS);
					//OnBeGetingLifeItemOK(nExp, 654);
					m_nLifeSkillExp = (int)(nOneExp * fS);

					m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDropSkin);
					g_sendCmd->BeginWrite();
					g_sendCmd->WriteShort(r2c_pickitem_type);
					g_sendCmd->WriteByte(2);//扒皮
					g_sendCmd->WriteByte(0);//剥皮成功
					g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
					g_sendCmd->WriteLong(pGetSkin->GetObjectId());
					m_SkinItem.Serialize(g_sendCmd);

					pGetSkin->RecvCmd(g_sendCmd);
					/*if(NULL != pGetSkin->m_scene)
					{
					pGetSkin->m_scene->BroadcastCmd(pGetSkin->m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
					}*/

				}
				else
				{
					pGetSkin->GetPlayerLogic()->SendGetSkinError(13);
				}
				pGetSkin->GetPlayerLogic()->CaneclGetingSkin();
				pGetSkin->ClientDoCommand(DO_COMMAND_GETSKIN, 1);
			}
		}
	}
}

void CMonsterLogicExt::GetMineRun(unsigned long dwTime)
{
	CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
	switch (type)
	{
	case CPlayerLogic::eHaveSkin:
		if(IsBeGetingMine())
		{
			GettingMineRun(dwTime);
		}
		break;
	case CPlayerLogic::eDropSkin:
		PickMineRun(dwTime);
		break;
	}
}

void CMonsterLogicExt::GetMedicineRun(unsigned long dwTime)
{
	CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
	switch (type)
	{
	case CPlayerLogic::eHaveSkin:
		if(IsBeGetingMedicine())
		{
			GettingMedicineRun(dwTime);
		}
		break;
	case CPlayerLogic::eDropSkin:
		PickMedicineRun(dwTime);
		break;
	}
}

void CMonsterLogicExt::GettingMineRun(unsigned long dwTime)
{
	ItemList& il = GetItemList();
	if(dwTime >= m_dwMineTime)
	{
		CRegionCreature* pGetMine = GetGetingMine();
		char cAni = DO_COMMAND_GETMINE;
		char cErr = 0;
		do
		{
			if(m_rPlayer.m_npcInfo->nTaskNum > 0)
			{
				int nTemp = DropTaskLifeItem();
				if(0 != nTemp || il.empty())
				{
					cErr = 15;
				}
				cAni = DO_COMMAND_TASKCOLECT;
				break;
			}

			if (NULL != pGetMine)
			{
				int nMySkillLevel = pGetMine->GetPlayerLogic()->GetSkillLevel(581);
				int nNeetSkillLevel = this->GetLifeItemLevel();
				float fR = 0.8f + (nMySkillLevel - nNeetSkillLevel) * 0.1f;
				int nR = (int)(fR * 100);
				//srand(rtGetMilliseconds());   ///< 随机种子
				//int nNum =rand() % 100;
				int nNum = GetRand(99, 0);

				if(nNum >= nR)
				{
					if (IsLifeSkillItemNPC())
					{
						cErr = 24;
					}
					else
					{
						cErr = 13;
					}
					break;
				}

				if(il.empty())
				{
					cErr = 15;
					break;
				}

				int nOneExp = (2+2*(nMySkillLevel - 1) * (1 + 0.05 * (nMySkillLevel -1 )));
				float fS = 1 + (nNeetSkillLevel - nMySkillLevel) / 3.0f;
				if(fS < 0)
				{
					fS = 0;
				}

				m_nLifeSkillExp = (int)(nOneExp * fS);
			}
		}
		while(false);
		switch(cErr)
		{
		case 0:
			if (NULL != pGetMine)
			{
				m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDropSkin);

				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_pickitem_type);
				g_sendCmd->WriteByte(2);//扒皮
				g_sendCmd->WriteByte(0);//剥矿成功
				g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
				g_sendCmd->WriteLong(pGetMine->GetObjectId());

				SItemID&  Item = *(il.begin());//矿
				Item.Serialize(g_sendCmd);

				pGetMine->RecvCmd(g_sendCmd);
				/*if(NULL != pGetMine->m_scene)
				{
				pGetMine->m_scene->BroadcastCmd(pGetMine->m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
				}*/
				m_dwCloseTime = rtGetMilliseconds() + LifeItemNPCReLiveTime;
			}
			break;
		default:
			if (NULL != pGetMine)
			{
				pGetMine->GetPlayerLogic()->SendGetSkinError(cErr);
			}
			break;
		}
		if (NULL != pGetMine)
		{
			pGetMine->GetPlayerLogic()->CaneclGetingMine();
			pGetMine->ClientDoCommand(cAni, 1);
		}
	}
}

void CMonsterLogicExt::GettingMedicineRun(unsigned long dwTime)
{
	ItemList& il = GetItemList();
	if(dwTime >= m_dwMedicineTime)
	{
		CRegionCreature* pGetMedicine = GetGetingMedicine();
		char cAni = DO_COMMAND_GETMINE;
		char cErr = 0;
		do
		{
			if(m_rPlayer.m_npcInfo->nTaskNum > 0)
			{
				int nTemp = DropTaskLifeItem();
				if(0 != nTemp || il.empty())
				{
					cErr = 15;
				}
				cAni = DO_COMMAND_TASKCOLECT;
				break;
			}
			if (NULL != pGetMedicine)
			{
				int nMySkillLevel = pGetMedicine->GetPlayerLogic()->GetSkillLevel(582);
				int nNeetSkillLevel = this->GetLifeItemLevel();
				float fR = 0.8f + (nMySkillLevel - nNeetSkillLevel) * 0.1f;
				int nR = (int)(fR * 100);
				//srand(rtGetMilliseconds());   ///< 随机种子
				//int nNum =rand() % 100;
				int nNum = GetRand(99, 0);

				if(nNum >= nR)
				{
					cErr = 13;
					break;
				}
				if(il.empty())
				{
					cErr = 15;
					break;
				}

				int nOneExp = (2+2*(nMySkillLevel - 1) * (1 + 0.05 * (nMySkillLevel -1 )));
				float fS = 1 + (nNeetSkillLevel - nMySkillLevel) / 3.0f;
				if(fS < 0)
				{
					fS = 0;
				}
				//int nExp = (int)(nOneExp * fS);
				//OnBeGetingLifeItemOK(nExp, 582);
				m_nLifeSkillExp = (int)(nOneExp * fS);
			}

		}while(false);
		switch(cErr)
		{
		case 0:
			if (NULL != pGetMedicine)
			{
				m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDropSkin);

				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_pickitem_type);
				g_sendCmd->WriteByte(2);//扒皮
				g_sendCmd->WriteByte(0);//剥药成功
				g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
				g_sendCmd->WriteLong(pGetMedicine->GetObjectId());

				SItemID&  Item = *(il.begin());//药
				Item.Serialize(g_sendCmd);

				pGetMedicine->RecvCmd(g_sendCmd);
				/*if(NULL != pGetMedicine->m_scene)
				{
				pGetMedicine->m_scene->BroadcastCmd(pGetMedicine->m_pos, DEFAULT_RANGE, g_sendCmd, NULL);
				}*/
				m_dwCloseTime = rtGetMilliseconds() + LifeItemNPCReLiveTime;
			}
			break;
		default:
			if (NULL != pGetMedicine)
			{
				pGetMedicine->GetPlayerLogic()->SendGetSkinError(cErr);
			}
			break;
		}
		if (NULL != pGetMedicine)
		{
			pGetMedicine->GetPlayerLogic()->CaneclGetingMedicine();
			pGetMedicine->ClientDoCommand(cAni, 1);
		}
	}
}

void CMonsterLogicExt::PickMineRun(unsigned long dwTime)
{
	if (dwTime >= m_dwCloseTime)
	{
		OnClosePickwindow(NULL);
	}
}

void CMonsterLogicExt::PickMedicineRun(unsigned long dwTime)
{
	if (dwTime >= m_dwCloseTime)
	{
		OnClosePickwindow(NULL);
	}
}

void CMonsterLogicExt::OnBeGetingLifeItemOK(int nExp, unsigned short wSkillID)
{
	do
	{
		if (0 == nExp)
		{
			break;
		}
		CRegionUser *user = g_region->FindUser(m_dwKillerDBID);
		if(NULL == user) 
		{
			break;
		}

		CRegionCreature* pGet = user->m_dummy;

		if(NULL == pGet)
		{
			break;
		}
		pGet->GetPlayerLogic()->AddLifeSkillExp(nExp, wSkillID);
	}while(false);
}

void CMonsterLogicExt::SetEraseTime(unsigned long dwTime)
{
	m_dwEraseTime = dwTime;
}

void CMonsterLogicExt::SetGetSkinTime(unsigned long dwTime)
{
	m_dwSkinTime = dwTime;
}

CPlayerLogic::LiveType CMonsterLogicExt::UpdateItemS()
{
	CPlayerLogic::LiveType nRet = CPlayerLogic::eHaveItem;
	ItemList& il = GetItemList();
	if(0 == il.size())
	{
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		if(CPlayerLogic::eHaveItem == type)
		{
			if(0 != m_rPlayer.m_npcInfo->dwSkinID)
			{
				m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eHaveSkin);
				nRet = CPlayerLogic::eHaveSkin;
			}
			else
			{
				//OnErase();
				m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
				nRet = CPlayerLogic::eDead;
			}
		}
		else if(CPlayerLogic::eDropSkin == type)
		{
			//OnErase();
			m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
			nRet = CPlayerLogic::eDead;
		}
		else
		{
			//OnErase();
			m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
			nRet = CPlayerLogic::eDead;
		}
	}
	return nRet;
}

void CMonsterLogicExt::OnErase()
{
	CRegionCreature*  pGetSkin = GetGetingSkin();
	do 
	{
		if (NULL != pGetSkin)
		{
			if(IsBeGetingSkin())
			{
				pGetSkin->ClientDoCommand(DO_COMMAND_GETSKIN, 1);
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_pickitem_type);
				g_sendCmd->WriteByte(2);//扒皮
				g_sendCmd->WriteByte(2);//物品已经消失
				g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
				m_SkinItem.Serialize(g_sendCmd);
				pGetSkin->RecvCmd(g_sendCmd);
				pGetSkin->GetPlayerLogic()->CaneclGetingSkin();
			}
		}

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_remove_actor);
		g_sendCmd->WriteLong((long)(m_rPlayer.m_oId));
		CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
		// TODO: Maybe someone don't have scene? louis
		int iCnt = m_rPlayer.m_scene->GetAroundCreatureByTile(pFindCreature,
			m_rPlayer.m_pos, DEFAULT_RANGE, NULL, PT_PATH, false);

		int i;
		for(i = 0; i < iCnt; i++)
		{
			if(NULL == pFindCreature[i])
			{
				continue;
			}
			CRegionCreature *pC = (CRegionCreature*)(pFindCreature[i]);
			if(!pC->IsUser())
			{
				continue;
			}

			pC->RecvCmd(g_sendCmd);
		}

		if (NULL != pGetSkin)
		{
			pGetSkin->RecvCmd(g_sendCmd);
		}

		if (NULL != m_rPlayer.m_spawn)
		{
			m_rPlayer.m_spawn->KillNpc(&m_rPlayer);
		}

		if (eEmptyDBID != m_dwKillerDBID)
		{
			unsigned short wSkillID = 654;
			if (this->IsMineNPC())
			{
				wSkillID = 581;
			}
			else if (this->IsMedicineNPC())
			{
				wSkillID = 582;
			}
			OnBeGetingLifeItemOK(m_nLifeSkillExp, wSkillID);
		}
		if (NULL != m_rPlayer.m_scene)
		{
			m_rPlayer.m_scene->RemoveCreature((long)(m_rPlayer.m_oId));
		}

		g_factory->DestroyObject(&m_rPlayer);
	} while (false);

}

bool CMonsterLogicExt::IsBeGetingSkin()
{
	bool bRet = false;
	do
	{
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		if(CPlayerLogic::eHaveSkin != type && CPlayerLogic::eDead != type)
		{
			break;
		}
		if(NULL == GetGetingSkin())
		{
			break;
		}
		bRet = true;
	}while(false);
	return bRet;
}

bool CMonsterLogicExt::IsBeGetingMine()
{
	bool bRet = false;
	do
	{
		if(NULL == GetGetingMine())
		{
			break;
		}
		bRet = true;
	}while(false);
	return bRet;
}

bool CMonsterLogicExt::IsBeGetingMedicine()
{
	bool bRet = false;
	do
	{
		if(NULL == GetGetingMedicine())
		{
			break;
		}
		bRet = true;
	}while(false);
	return bRet;
}

void CMonsterLogicExt::CaneclGetSkin()
{
	m_dwGetSkinID = eEmptyDBID;
}

void CMonsterLogicExt::CaneclGetMine()
{
	m_dwGetMineID = eEmptyDBID;
}

void CMonsterLogicExt::CaneclGetMedicine()
{
	m_dwGetMedicineID = eEmptyDBID;
}


int  CMonsterLogicExt::GetSkin(CRegionCreature* pGetSkin)
{
	int nRet = 0;
	do
	{
		//const int c_n_BuffSize = 256;
		//char szBuff[c_n_BuffSize];
		TObjectHash& Oh = g_factory->GetTObjectHash();
		TObjectHash::iterator it;
		it = Oh.find(m_rPlayer.GetObjectId());
		if (Oh.end() == it)
		{
			OnErase();
			nRet = 19;
			break;
		}
		if(this->IsMineNPC())
		{
			nRet = this->GetMine(pGetSkin);
			break;
		}
		else if(this->IsMedicineNPC())
		{
			nRet = this->GetMedicine(pGetSkin);
			break;
		}
		if(NULL == pGetSkin)
		{
			nRet = 7;
			break;
		}

		if(IsBeGetingSkin()  && pGetSkin != GetGetingSkin())
		{
			nRet = 10;//有人在剥了
			break;
		}
		//if(pGetSkin != m_pKiller)
		//{
		//	nRet = 5;//该怪主要不是你打死的
		//	break;
		//}
		SSkill* pS = pGetSkin->GetPlayerLogic()->GetSkill(654);
		if (NULL == pS)
		{
			nRet = 27;
			break;
		}
		int nMySkillLevel = pS->iLevel;//pGetSkin->GetPlayerLogic()->GetSkillLevel(654);
		int nNeetSkillLevel = this->GetLifeItemLevel();

		if(nMySkillLevel < nNeetSkillLevel)
		{
			//rt2_snprintf(szBuff, c_n_BuffSize, R(MSG_SKILL_BOPILOW), m_rPlayer.m_core.Name.c_str(), nNeetSkillLevel, pS->szName);
			//pGetSkin->SendSystemMessage(szBuff);
			nRet = 9;//所需技能等级不符合
			break;
		}

		bool bHave = pGetSkin->GetPlayerLogic()->IsHaveItem(6103);

		if(!bHave)
		{
			nRet = 11;//没剥皮工具
			break;
		}

		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		bool bBreak = false;
		switch(type)
		{
		case CPlayerLogic::eHaveSkin:

			if (pGetSkin->IsUser())
			{
				m_dwKillerDBID = pGetSkin->m_userInfo->m_userId;
				m_dwGetSkinID = m_dwKillerDBID;
			}
			else
			{
				m_dwKillerDBID = eEmptyDBID;
			}
			m_dwSkinTime = rtGetMilliseconds() + m_rPlayer.m_npcInfo->dwGetSkinTime * 1000;
			pGetSkin->GetPlayerLogic()->SetGetingSkin(this);
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_pickitem_type);
			g_sendCmd->WriteByte(2);//扒皮
			g_sendCmd->WriteByte(4);//开始剥皮成功
			g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
			g_sendCmd->WriteLong(pGetSkin->GetObjectId());
			pGetSkin->RecvCmd(g_sendCmd);

			pGetSkin->ClientDoCommand(DO_COMMAND_GETSKIN, 0);
			/*if(NULL != pGetSkin->m_scene)
			{
			pGetSkin->m_scene->BroadcastCmd(pGetSkin->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
			}*/
			break;
		default:
			nRet = 14;
			bBreak = true;
			break;
		}
		if(bBreak)
		{
			break;
		}
		nRet = 0;
	}while(false);
	return nRet;
}

int  CMonsterLogicExt::GetMine(CRegionCreature* pGetMine)
{
	int nRet = 1;
	do
	{
		//const int c_n_BuffSize = 256;
		//char szBuff[c_n_BuffSize];
		char cAni = DO_COMMAND_GETMINE;
		if(NULL == pGetMine)
		{
			nRet = 7;
			break;
		}

		if(IsBeGetingMine() && pGetMine != GetGetingMine())
		{
			nRet = 10;//有人在剥了
			break;
		}

		if(0 == m_rPlayer.m_npcInfo->nTaskNum)
		{
			SSkill* pS = pGetMine->GetPlayerLogic()->GetSkill(581);
			if (NULL == pS)
			{
				nRet = 27;
				break;
			}

			int nMySkillLevel = pS->iLevel;//pGetMine->GetPlayerLogic()->GetSkillLevel(581);
			int nNeetSkillLevel = this->GetLifeItemLevel();

			if(nMySkillLevel < nNeetSkillLevel)
			{
				//rt2_snprintf(szBuff, c_n_BuffSize, R(MSG_SKILL_YEJINLOW), m_rPlayer.m_core.Name.c_str(), nNeetSkillLevel, pS->szName);
				//pGetMine->SendSystemMessage(szBuff);
				nRet = 8;//所需技能等级不符合
				break;
			}

			bool bHave = pGetMine->GetPlayerLogic()->IsHaveItem(6101);

			if(!bHave)
			{
				nRet = 11;//没挖矿工具
				break;
			}
		}
		else
		{
			int i;
			bool bB = true;
			for(i = 0; i < m_rPlayer.m_npcInfo->nTaskNum; i++)
			{
				if(pGetMine->GetPlayerLogic()->IsDoingTask(m_rPlayer.m_npcInfo->TaskId[i]))
				{
					if(pGetMine->GetItemNum(m_rPlayer.m_npcInfo->TaskItemId[i], ItemColor_White) >= m_rPlayer.m_npcInfo->TaskItemLimit[i])
					{
						nRet = 18;//任务采集时超过上限，不能再采集
						bB = true;
					}
					else
					{
						bB = false;
					}
					break;
				}
			}
			if(bB)
			{
				break;
			}
			cAni = DO_COMMAND_TASKCOLECT;
		}

		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		bool bBreak = false;
		switch(type)
		{
		case CPlayerLogic::eHaveSkin:
			if (pGetMine->IsUser())
			{
				m_dwKillerDBID = pGetMine->m_userInfo->m_userId;
			}
			else
			{
				m_dwKillerDBID = eEmptyDBID;
			}
			m_dwGetMineID = m_dwKillerDBID;

			m_dwMineTime = rtGetMilliseconds() + 5000;
			pGetMine->GetPlayerLogic()->SetGetingMine(this);

			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_pickitem_type);
			g_sendCmd->WriteByte(2);//扒皮
			g_sendCmd->WriteByte(4);//开始挖矿成功
			g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
			g_sendCmd->WriteLong(pGetMine->GetObjectId());
			pGetMine->RecvCmd(g_sendCmd);

			pGetMine->ClientDoCommand(cAni, 0);
			/*if(NULL != pGetMine->m_scene)
			{
			pGetMine->m_scene->BroadcastCmd(pGetMine->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
			}*/
			break;
		case CPlayerLogic::eDropSkin:
			nRet = 12;
			bBreak = true;
			break;
		default:
			nRet = 9;
			bBreak = true;
			break;
		}
		if(bBreak)
		{
			break;
		}
		nRet = 0;
	}while(false);
	return nRet;
}

int  CMonsterLogicExt::GetMedicine(CRegionCreature* pGetMedicine)
{
	int nRet = 1;
	do
	{
		//const int c_n_BuffSize = 256;
		//char szBuff[c_n_BuffSize];
		char cAni = DO_COMMAND_GETMINE;
		if(NULL == pGetMedicine)
		{
			nRet = 7;
			break;
		}

		if(IsBeGetingMedicine() && pGetMedicine != GetGetingMedicine())
		{
			nRet = 10;//有人在剥了
			break;
		}

		if(0 == m_rPlayer.m_npcInfo->nTaskNum)
		{
			SSkill* pS = pGetMedicine->GetPlayerLogic()->GetSkill(582);
			if (NULL == pS)
			{
				nRet = 27;
				break;
			}
			
			int nMySkillLevel = pS->iLevel;//pGetMedicine->GetPlayerLogic()->GetSkillLevel(582);
			int nNeetSkillLevel = this->GetLifeItemLevel();

			if(nMySkillLevel < nNeetSkillLevel)
			{
				//rt2_snprintf(szBuff, c_n_BuffSize, R(MSG_SKILL_BAICAOLOW), m_rPlayer.m_core.Name.c_str(), nNeetSkillLevel, pS->szName);
				//pGetMedicine->SendSystemMessage(szBuff);
				nRet = 8;//所需技能等级不符合
				break;
			}

			bool bHave = pGetMedicine->GetPlayerLogic()->IsHaveItem(6102);

			if(!bHave)
			{
				nRet = 11;//没挖矿工具
				break;
			}
		}
		else
		{
			bool bB = true;
			int i;
			for(i = 0; i < m_rPlayer.m_npcInfo->nTaskNum; i++)
			{
				if(pGetMedicine->GetPlayerLogic()->IsDoingTask(m_rPlayer.m_npcInfo->TaskId[i]))
				{
					if(pGetMedicine->GetItemNum(m_rPlayer.m_npcInfo->TaskItemId[i], ItemColor_White) >= m_rPlayer.m_npcInfo->TaskItemLimit[i])
					{
						nRet = 18;//任务采集时超过上限，不能再采集
						bB = true;
					}
					else
					{
						bB = false;
					}
					break;
				}
			}
			if(bB)
			{
				break;
			}
			cAni = DO_COMMAND_TASKCOLECT;
		}
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		bool bBreak = false;
		switch(type)
		{
		case CPlayerLogic::eHaveSkin:

			if (pGetMedicine->IsUser())
			{
				m_dwKillerDBID = pGetMedicine->m_userInfo->m_userId;
			}
			else
			{
				m_dwKillerDBID = eEmptyDBID;
			}
			m_dwGetMedicineID = m_dwKillerDBID;

			m_dwMedicineTime = rtGetMilliseconds() + 5000;
			pGetMedicine->GetPlayerLogic()->SetGetingMedicine(this);

			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_pickitem_type);
			g_sendCmd->WriteByte(2);//扒皮
			g_sendCmd->WriteByte(4);//开始采药成功
			g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
			g_sendCmd->WriteLong(pGetMedicine->GetObjectId());

			pGetMedicine->RecvCmd(g_sendCmd);

			pGetMedicine->ClientDoCommand(cAni, 0);
			break;
		case CPlayerLogic::eDropSkin:
			nRet = 12;
			bBreak = true;
			break;
		default:
			nRet = 9;
			bBreak = true;
			break;
		}
		if(bBreak)
		{
			break;
		}

		nRet = 0;
	}while(false);
	return nRet;
}

void CMonsterLogicExt::PickSkin(CRegionCreature* pGetSkin)
{
	SItemID item;
	char cRet = 0;
	do
	{
		if(NULL == pGetSkin)
		{
			cRet = 7;
			break;
		}

		//if(pGetSkin != m_pKiller)
		//{
		//	cRet = 5;//该怪主要不是你打死的
		//	break;
		//}
		if (!pGetSkin->IsUser())
		{
			cRet = 1;//未知错误
			break;
		}
		if(pGetSkin->m_userInfo->m_userId != m_dwKillerDBID)
		{
			cRet = 5;//该怪主要不是你打死的
			break;
		}
		CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
		if(CPlayerLogic::eDropSkin != type)
		{
			cRet = 1;//状态不对
			break;
		}
		if(this->IsMineNPC() || this->IsMedicineNPC())
		{
			ItemList& il = GetItemList();
			if(il.empty())
			{
				cRet = 13;//状态不对
				break;
			}
			else
			{
				item = *(il.begin());
			}
		}
		else
		{
			item = m_SkinItem;
		}
	}while(false);
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_pickitem_type);
	g_sendCmd->WriteByte(2);//扒皮
	g_sendCmd->WriteByte(cRet);
	g_sendCmd->WriteLong(m_rPlayer.GetObjectId());
	g_sendCmd->WriteLong(pGetSkin->GetObjectId());
	item.Serialize(g_sendCmd);
	pGetSkin->RecvCmd(g_sendCmd);
}

CMonsterLogicExt::ItemList& CMonsterLogicExt::GetItemList()
{
	return m_ItemList;
}

int CMonsterLogicExt::GetLifeItemLevel()
{
	int nRet = 1;
	if (IsMedicineNPC() || IsMineNPC())
	{
		nRet = m_rPlayer.m_core.Lev;
	}
	else
	{
		nRet = m_rPlayer.m_core.Lev / 10 + 1;
	}

	return nRet;
}

bool CMonsterLogicExt::IsMineNPC()
{
	bool bRet =false;
	do
	{
		if(NULL == m_rPlayer.m_npcInfo)
		{
			break;
		}
		bRet = (5 == m_rPlayer.m_npcInfo->Type || IsLifeSkillItemNPC());
	}while(false);
	return bRet;
}

bool CMonsterLogicExt::IsMedicineNPC()
{
	bool bRet =false;
	do
	{
		if(NULL == m_rPlayer.m_npcInfo)
		{
			break;
		}
		bRet = 4 == m_rPlayer.m_npcInfo->Type;
	}while(false);
	return bRet;
}

bool CMonsterLogicExt::IsLifeSkillItemNPC()
{
	bool bRet =false;
	do
	{
		if(NULL == m_rPlayer.m_npcInfo)
		{
			break;
		}
		bRet = 9 == m_rPlayer.m_npcInfo->Type;
	}while(false);
	return bRet;
}

void CMonsterLogicExt::OnClosePickwindow(CRegionCreature *pPlayer)
{
	CPlayerLogic::LiveType type = m_rPlayer.GetPlayerLogic()->GetLiveType();
	do
	{
		/*if (CPlayerLogic::eHaveItem == type)
		{
			if (NULL == pPlayer)
			{
				break;
			}
			if (!pPlayer->IsUser())
			{
				break;
			}
			if (NULL == pPlayer->m_pTeam)
			{
				break;
			}
			int nNum = pPlayer->m_pTeam->mData.Size();
			if (0 == nNum)
			{
				break;
			}
			TeamPickType type = pPlayer->m_pTeam->mData.GetPickType();
			if (eOrderPick != type)
			{
				break;
			}
			pPlayer->m_pTeam->NextPicker();
			
			break;
		}*/
		if(CPlayerLogic::eDropSkin != type)
		{
			break;
		}
		//this->m_pKiller = NULL;
		m_dwKillerDBID = eEmptyDBID;
		m_rPlayer.GetPlayerLogic()->SetLiveType(CPlayerLogic::eHaveSkin);
	}while(false);
}

CRegionCreature* CMonsterLogicExt::GetGetingSkin()
{
	return GetCreature(m_dwGetSkinID);
}

CRegionCreature* CMonsterLogicExt::GetGetingMine()
{
	return GetCreature(m_dwGetMineID);
}

CRegionCreature* CMonsterLogicExt::GetGetingMedicine()
{
	return GetCreature(m_dwGetMedicineID);
}

CRegionCreature* CMonsterLogicExt::GetCreature(unsigned long dwID)
{
	CRegionCreature* pGet = NULL;
	do 
	{
		if (eEmptyDBID == dwID)
		{
			break;
		}
		CRegionUser *user = g_region->FindUser(m_dwKillerDBID);
		if(NULL == user) 
		{
			break;
		}
		pGet = user->m_dummy;

	} while (false);
	return pGet;
}

////////////////////////
//class CNewPlayerGuide
///////////////////////

CNewPlayerGuide::CNewPlayerGuide(CRegionCreature& rPlayer):m_rPlayer(rPlayer)
{
}

void CNewPlayerGuide::OnHpChange(int nNewHp)
{
	if (m_rPlayer.IsUser())
	{
		EatHP();
	}

}

void CNewPlayerGuide::OnMpChange(int nNewHp)
{
	if (m_rPlayer.IsUser())
	{
		EatMP();
	}
}

void CNewPlayerGuide::OnWarScoresChange(float fOldWarScores, float fNewWarScores)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(r2g_WarScores_Change);
	packet.WriteLong(m_rPlayer.m_userInfo->m_userId);
	packet.WriteLong(fOldWarScores);
	packet.WriteLong(fNewWarScores);
	g_region->m_gws.EndSend();
}

bool CNewPlayerGuide::HitMove()
{
	return DoGuide(eHitMove);
}

bool CNewPlayerGuide::NPCTaskTalk()
{
	return DoGuide(eNPCTaskTalk);
}

bool CNewPlayerGuide::NPCFinishTask()
{
	return DoGuide(eNPCFinishTask);
}

bool CNewPlayerGuide::AttrButFlash()
{
	return DoGuide(eAttrButFlash);
}

bool CNewPlayerGuide::FirstAttackMonster()
{
	return DoGuide(eFirstAttackMonster);
}

bool CNewPlayerGuide::EatHP()
{
	int nMax = m_rPlayer.m_core.MaxHp.GetValue();
	float fHp = m_rPlayer.m_core.GetHp();
	float ft = fHp / nMax;
	bool bRet = false;
	do 
	{
		if (ft > 0.5f)
		{
			break;
		}
		bRet =  DoGuide(eEatHP);
	} while (false);
	return bRet;
}

bool CNewPlayerGuide::EatMP()
{
	int nMax = m_rPlayer.m_core.MaxMp.GetValue();
	float fMp = m_rPlayer.m_core.GetMp();
	float ft = fMp / nMax;
	bool bRet = false;
	do 
	{
		if (ft > 0.5f)
		{
			break;
		}
		bRet =  DoGuide(eEatMP);
	} while (false);
	return bRet;
}

bool CNewPlayerGuide::UsePet1()
{
	return DoGuide(eUsePet1);
}

bool CNewPlayerGuide::BuyItem1()
{
	return DoGuide(eBuyItem1);
}

bool CNewPlayerGuide::FaBao1()
{
	return DoGuide(eFaBao1);
}

bool CNewPlayerGuide::Mail1()
{
	return DoGuide(eMail1);
}

bool CNewPlayerGuide::LifeSkill1()
{
	return DoGuide(eLifeSkill1);
}


bool CNewPlayerGuide::DoGuide(GuideType type)
{
	bool bRet = false;
	do 
	{
		if (m_rPlayer.m_core.Lev > 8)
		{
			break;
		}

		bool bDone = m_rPlayer.m_task.IsDoneGuild(type);
		if (bDone)
		{
			break;
		}
		SendGuide(type);
		bRet = true;
	} while (false);
	return bRet;
}

void CNewPlayerGuide::SendGuide(GuideType type)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_Guide);
	g_sendCmd->WriteShort((short)type);
	m_rPlayer.RecvCmd(g_sendCmd);
}

////////////////////////
//class CMainStoryMgr
///////////////////////
int CMainStoryMgr::BuildMgr()
{
	int nRet = 0;
	CRS_MainStory::MainStoryDataV& v = g_TableMainStory.GetMainStoryDataV();
	MainStoryIDSet& IDSet = GetMainStoryIDSet();
	MainStoryTaskIDSet& TaskIDSet = GetMainStoryTaskIDSet();

	int i;
	int nSize = (int)(v.size());
	int j;
	int nNum;
	for (i = 0; i < eJobNum; i++)
	{
		m_MainTaskNum[i] = 0;
	}
	for(i = 0; i < nSize; i++)
	{
		MainStoryData* pData = &(v[i]);
		if (NULL == pData)
		{
			continue;
		}
		IDSet.insert(pData);
		nNum = pData->nTaskNum;
		for(j = 0; j < nNum; j++)
		{
			TaskIDSet[pData->nTaskID[j]] = i;
		}
		if (pData->nJob < 0 || pData->nJob > eJobNum)
		{
			continue;
		}
		if (0 == pData->nJob)
		{
			for (j = 0; j < eJobNum; j++)
			{
				if (m_MainTaskNum[j] >= eMaxMainTaskNum)
				{
					continue;
				}
				m_MainTask[j][m_MainTaskNum[j]++] = pData->nDataID;
			}
		}
		else
		{
			int nJob = pData->nJob - 1;
			if (m_MainTaskNum[nJob] >= eMaxMainTaskNum)
			{
				continue;
			}
			m_MainTask[nJob][m_MainTaskNum[nJob]++] = pData->nDataID;
		}
	}
	return nRet;
}

MainStoryData* CMainStoryMgr::GetMainStoryByID(int nID)
{
	MainStoryData* pRet = NULL;
	do
	{
		MainStoryIDSet& IDSet = GetMainStoryIDSet();

		MainStoryData data;
		data.nDataID = nID;
		MainStoryIDSet::iterator iter = IDSet.find(&data);
		if (IDSet.end() == iter)
		{
			break;
		}
		pRet = *iter;
	}while(false);
	return pRet;
}

MainStoryData* CMainStoryMgr::GetMainStoryByTaskID(int nTaskID)
{
	MainStoryData* pRet = NULL;
	CRS_MainStory::MainStoryDataV& v = g_TableMainStory.GetMainStoryDataV();
	MainStoryTaskIDSet& TaskIDSet = GetMainStoryTaskIDSet();
	do
	{
		MainStoryTaskIDSet::iterator iter = TaskIDSet.find(nTaskID);
		if (TaskIDSet.end() == iter)
		{
			break;
		}
		int nIndex = (*iter).second;
		if (nIndex < 0 || nIndex >= (int)(v.size()))
		{
			break;
		}
		pRet = &(v[nIndex]);
	}while(false);
	return pRet;
}

int CMainStoryMgr::GetMainTaskIDByJobAndIndex(int nJob, int nIndex)
{
	int nRet = -1;
	do 
	{
		nJob--;
		if (nJob < 0 || nJob >= eJobNum)
		{
			break;
		}
		if (nIndex < 0 )
		{
			break;
		}
		if (nIndex >= m_MainTaskNum[nJob])
		{
			nRet = -2;
			break;
		}
		nRet = m_MainTask[nJob][nIndex];
	} while (false);
	return nRet;
}

int CMainStoryMgr::GetIndexByMainTaskIDAndJob(int nJob, int nID)
{
	int nRet = -1;
	do 
	{
		nJob--;
		if (nJob < 0 || nJob >= eJobNum)
		{
			break;
		}
		int i;
		bool bB = false;
		for (i = 0; i < m_MainTaskNum[nJob]; i++)
		{
			if (m_MainTask[nJob][i] == nID)
			{
				nRet = i;
				bB = true;
				break;
			}
		}
		if (bB)
		{
			break;
		}
	} while (false);
	return nRet;
}

int CMainStoryMgr::GetMainStoryNum(int nJob)
{
	int nRet = 0;
	do 
	{
		if (nJob <= 0 || nJob > eJobNum)
		{
			break;
		}
		nJob--;
		nRet = m_MainTaskNum[nJob];
	} while (false);
	return nRet;
}

CMainStoryMgr& CMainStoryMgr::GetMgr()
{
	static CMainStoryMgr s_Mgr;
	return s_Mgr;
}


bool MainStoryIDCmp::operator () (MainStoryData* pData1, MainStoryData* pData2) const
{
	bool bRet = false;
	do 
	{
		if (NULL == pData1 || NULL == pData2)
		{
			break;
		}
		bRet = pData1->nDataID < pData2->nDataID;
	} while (false);
	return bRet;
}



CMainStoryMgr::MainStoryIDSet& CMainStoryMgr::GetMainStoryIDSet()
{
	return m_MainStoryIDSet;
}

CMainStoryMgr::MainStoryTaskIDSet& CMainStoryMgr::GetMainStoryTaskIDSet()
{
	return m_MainStoryTaskIDSet;
}
////////////////////////
//class CRSLogicExt
///////////////////////
extern const int c_g_TempBuffSize = 1024 * 512;
extern G_MEMDEF(g_TempBuff, c_g_TempBuffSize)
//char g_TempBuff[c_g_TempBuffSize];
void CRSLogicExt::OnRun(DWORD dwTime)
{
	static unsigned long s_NextTime = 0;
	const unsigned long c_Setp = 400;
	static bool s_bScence = false;

	do 
	{
		if(dwTime < s_NextTime)
		{
			break;
		}
		CRegionCreature** ppBuff = (CRegionCreature**)(g_TempBuff);
		int nNum = 0;
		TObjectHash::iterator it;
		TObjectHash* pOh = NULL;
		if (s_bScence)
		{
			pOh = &(g_region->m_defScene->m_objectHash);
		}
		else
		{
			pOh = &(g_factory->GetTObjectHash());
		}
		if (NULL == pOh)
		{
			break;
		}
		for(it = pOh->begin(); pOh->end() != it; it++)
		{
			CRegionObject *object = (*it).second;
			if(NULL == object)
			{
				continue;
			}
			if( OB_TYPE(object) != OBJECT_TYPE_CREATURE &&
				OB_TYPE(object) != OBJECT_TYPE_NPC_SERVICE &&
				OB_TYPE(object) != OBJECT_TYPE_NPC_COMBATIVE)
			{
				continue;
			}

			CRegionCreature *cre = (CRegionCreature*)object;
			if(!cre->IsMonster())
			{
				continue;
			}
			if (nNum >= c_g_TempBuffSize)
			{
				break;
			}
			ppBuff[nNum++] = cre;
		}
		int i;
		for(i = 0; i < nNum; i++)
		{
			ppBuff[i]->GetPlayerLogic()->GetMonsterLogicExt()->OnRun(dwTime);
		}
		s_NextTime = dwTime + c_Setp;
		s_bScence = !s_bScence;
	} while (false);
}

void CRSLogicExt::InitTaskDesc()
{
	std::vector<STaskDesc> &v = g_TableTask.GetTaskTable();
	std::vector<STaskDesc>::iterator iter;
	char cIndex = 0;
	for(iter = v.begin(); v.end() != iter; iter++)
	{
		STaskDesc &Desc = *iter;

		vector<STaskKey>::iterator itKey;
		cIndex = 0;
		for(itKey = Desc.taskReq.begin(); Desc.taskReq.end() != itKey; itKey++)
		{
			ProcessTaskKey(*itKey, &Desc, cIndex);
		}
	}
	CMainStoryMgr::GetMgr().BuildMgr();
}

bool CRSLogicExt::ProcessTaskKey(STaskKey &key, STaskDesc* pDesc, char &cIndex)
{
	bool bRet = false;
	ItemTaskData data;
	int i = 0;
	ItemTaskDataMap& mapData = GetItemTaskDataMap();
	NPCTaskMap& mapNPC = GetNPCTaskMap();
	do
	{
		if(key.key == "useitem")
		{
			int nItemID = atoi(key.param[0].c_str());
			data.pTask = pDesc;
			data.strArea = key.param[1];
			data.cIndex = cIndex++;
			data.nTaskID = pDesc->id;
			mapData.insert(std::make_pair(nItemID, data));
		}
		else if (key.key == "item")
		{
			unsigned long itemId;
			itemId = atol(key.param[0].c_str());
			CRSLogicExt::ItemFinishTaskMap& finishMap = GetItemFinishTaskMap();
			finishMap.insert(std::make_pair(itemId, pDesc->id));
		}
		else if (key.key == "killmonster")
		{
			for(i = 1; i < 5; i++)
			{
				unsigned long dwNPCID = atol(key.param[i].c_str());
				if (0 == dwNPCID)
				{
					continue;
				}
				mapNPC.insert(std::make_pair(dwNPCID, pDesc));
			}
		}
	}while(false);
	return bRet;
}


CRSLogicExt::ItemFinishTaskMap& CRSLogicExt::GetItemFinishTaskMap()
{
	return m_ItemFinishTaskMap;
}

const char* CRSLogicExt::DoClientCommand(CRegionCreature *cmdGiver,const char* szCommand, bool bPyGMOP, long pyGmAccount, bool& bProce)
{
	const char* pRet = "";
	bProce = true;
	do 
	{
		if (NULL == cmdGiver || NULL == szCommand)
		{
			break;
		}
		std::istringstream is(szCommand);
		std::string strKey;
		is>>strKey;
		if (strKey == "CreateUnion")
		{
			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_union);
			packet.WriteShort(c2r_union_create);
			packet.WriteLong(cmdGiver->m_userInfo->m_userId);
			std::string str;
			is>>str;
			packet.WriteString(str.c_str());
			is>>str;
			packet.WriteString(str.c_str());
			g_region->m_gws.EndSend();
			cmdGiver->m_pItemContainer->LockOperate(true);
		}
		else if (strKey == "AcceptTask")
		{
			short wID;
			is>>wID;
			cmdGiver->m_task.AddTask(wID);
			STaskInfo *pTask = cmdGiver->m_task.FindTask(wID);
			if (NULL != pTask)
			{
				pTask->FSetDoing();
				STaskDesc *desc = pTask->GetDesc();
				if (NULL != desc)
				{
					if (desc->IsMainTask())
					{
						int nIndex = CMainStoryMgr::GetMgr().GetIndexByMainTaskIDAndJob(cmdGiver->m_core.Metier, wID);
						cmdGiver->GetPlayerLogic()->SetCurMainStoryID(nIndex);
					}
					cmdGiver->CheckTaskFinishState(desc, NULL, false);
				}
				cmdGiver->GetPlayerLogic()->TrackTask(wID);
				cmdGiver->UpdateToClient(NET_CRE_UPDATE_TASK);
			}
		}
		else if (strKey == "FinishTask")
		{
			short wID;
			is>>wID;
			STaskDesc *desc = g_TableTask.FindTask(wID);
			if (NULL != desc)
			{
				cmdGiver->m_task.FinishTask(desc);
				cmdGiver->UpdateToClient(NET_CRE_UPDATE_TASK);
			}
		}
		else if (strKey == "RemoveTask")
		{
			short wID;
			is>>wID;
			bool bOK = cmdGiver->m_task.RemoveTask(wID);
			if (!bOK)
			{
				CTaskInfo::DoneTaskSet& ds = cmdGiver->m_task.GetDoneTaskSet();
				ds.erase(wID);
			}
			cmdGiver->UpdateToClient(NET_CRE_UPDATE_TASK);
		}
		else if (strKey == "MustKill")
		{
			long nPh = 1;
			is>>nPh;
			cmdGiver->m_core.DamageMin.Rate = nPh;
			cmdGiver->m_core.DamageMax.Rate = nPh;
			cmdGiver->m_IsInvincible = 1;
			(&(cmdGiver->m_Skill))->ImmunoHarmful = 1;
			cmdGiver->UpdateToClient(NET_CRE_UPDATE_TASK);
		}
		else if (strKey == "ReadConfig")
		{
			if(!ReadRegionConfigFile(R(INI_REGION)))
			{
				ERR("Startup: read region config file [sv_table/region.ini] failed");
				return false;
			}	
		}
		else
		{
			bProce = false;
		}
	} while (false);
	return pRet;
}

int CRSLogicExt::GetItemTaskData(unsigned long nIndex, ItemTaskData** ppBuff, int nBuffNum)
{
	int nRet = 0;
	do
	{
		if (NULL == ppBuff || nBuffNum < 1)
		{
			break;
		}
		ItemTaskDataMap& mapData = GetItemTaskDataMap();
		ItemTaskDataMap::iterator beg = mapData.lower_bound(nIndex);
		ItemTaskDataMap::iterator end = mapData.upper_bound(nIndex);

		ItemTaskDataMap::iterator iter;
		for(iter = beg; iter != end; iter++)
		{
			if (nRet >= nBuffNum)
			{
				break;
			}
			ppBuff[nRet++] = &((*iter).second);
		}
	}while(false);
	return nRet;
}

int CRSLogicExt::GetNPCTaskData(unsigned long nIndex, STaskDesc** ppBuff, int nBuffNum)
{
	int nRet = 0;
	do
	{
		if (NULL == ppBuff || nBuffNum < 1)
		{
			break;
		}
		NPCTaskMap& mapData = GetNPCTaskMap();
		NPCTaskMap::iterator beg = mapData.lower_bound(nIndex);
		NPCTaskMap::iterator end = mapData.upper_bound(nIndex);

		NPCTaskMap::iterator iter;
		for(iter = beg; iter != end; iter++)
		{
			if (nRet >= nBuffNum)
			{
				break;
			}
			ppBuff[nRet++] = (*iter).second;
		}
	}while(false);
	return nRet;
}

void  CRSLogicExt::SelectedByDBID(CRegionCreature* pFind, unsigned long dwDBID)
{
	char cType = 0;
	long ln = 0;
	long id = -1;
	do 
	{
		if (NULL == pFind)
		{
			cType = 1;
			break;
		}
		if(NULL == pFind->m_scene)
		{
			cType = 1;
			break;
		}
		CRegionUser *user = g_region->FindUser(dwDBID);
		if(NULL == user) 
		{
			break;
		}
		CRegionObject *ob;
		CRegionCreature *tmp;

		CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
		// int ObjectNum = GetAroundCreatureByTile(pos, range, exclude, PT_PATH, false, MAX_SNAPSHOT_OBJECT_SIZE);
		int ObjectNum = pFind->m_scene->GetAroundObjectByGrid(pFindCreature, pFind->m_pos, DEFAULT_RANGE, 50);
		for (int i=0; i<ObjectNum; i++)
		{
			ob = pFindCreature[i];

			if( (OB_TYPE(ob) == OBJECT_TYPE_CREATURE) ||
				(OB_TYPE(ob) == OBJECT_TYPE_NPC_COMBATIVE)
				)
			{
				tmp = (CRegionCreature*)ob;
				if (NULL == tmp)
				{
					continue;
				}
				if(!tmp->IsUser()) continue;
				if(tmp->Distance(pFind->m_pos) > DEFAULT_RANGE) continue;
				if (tmp->m_userInfo->m_userId == dwDBID)
				{
					ln = 1;
					id  = tmp->GetObjectId();
					break;
				}
			}
		}
	} while (false);
	switch (cType)
	{
	case 0:
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_subsystem_team);
		g_sendCmd->WriteByte(R2C_SELECTED_BY_DBID);
		g_sendCmd->SerializeBit(ln, 1);
		g_sendCmd->WriteLong(id);
		pFind->RecvCmd(g_sendCmd);
		break;
	default:
		break;
	}

}

CRSLogicExt::ItemTaskDataMap& CRSLogicExt::GetItemTaskDataMap()
{
	return m_ItemTaskDataMap;
}

CRSLogicExt::NPCTaskMap& CRSLogicExt::GetNPCTaskMap()
{
	return m_NPCTaskMap;
}

CRSLogicExt& CRSLogicExt::GetRSLogicExt()
{
	static CRSLogicExt s_s;
	return s_s;
}


