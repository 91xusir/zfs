#include "region.h"
#include "region_object.h"
#include "cmd_channel.h"
#include "region_server.h"
#include "scene/rts_sv_scene.h"
#include "scene/rts_path_find.h"
#include "region_scene.h"
#include "region_creature.h"
#include "region_ai.h"

//PZH
#include "PlayerLogic.h"
//

CRegionAIMgr g_aiMgr;
extern CRS_NPCTalkText	g_TableNpcRandomTalk;

//HM_RecInfoArray	g_rec_info[2];

CRegionConjureAI::CRegionConjureAI(CRegionCreature* p_cre, DWORD skill_id, short npc_id)
{
	m_creature = p_cre;
	
	m_SkillID = skill_id;
	
	m_NpcID = npc_id;

	m_ConjuredNum = CONJURE_NUM;
}

bool CRegionConjureAI::CanUseSkill()
{
	if(m_ConjuredNum <= 0)
	{
		return false;
	}

	if(GetCreatureNum() > 0)
	{
		return false;
	}

	if(m_creature->m_master)
	{
		return false;
	}

	return true;
}

void CRegionConjureAI::UseSkill()
{
	if(!m_creature->m_Skill.CanUseSkill(m_SkillID, &(m_creature->m_Skill)))
	{
		SAttackParam param;
		param.Seed = rand()%60000;
		param.EnemyId = m_creature->m_oId;
		param.Skill = m_SkillID;
		param.TargetType = 1;

		if(!m_creature->UseSkill(param))
		{
			LOG("[CRegionConjureAI::UseSkill]: cannot use conjure skill\n");
		}

		DecNum();
	}else
	{
		LOG1("[CRegionConjureAI::UseSkill]: cannot use conjure skill [%d]\n", m_SkillID);
	}
	
	return;
}

void CRegionAI::OnCallPathNextSuccessed(int tileX, int tileY, float &fx, float &fy)
{
	if (m_bHaveNextNodePos)
	{
		m_currNodePos[0] = m_nextNodePos[0];
		m_currNodePos[1] = m_nextNodePos[1];
	}
	else 
	{
		m_currNodePos[0] = m_creature->m_pos[0];
		m_currNodePos[1] = m_creature->m_pos[1];
	}

	g_region->m_defScene->m_pTerrain->GetPosByTerrainXY(tileX, tileY, fx, fy);
	m_nextNodePos[0] = (long)fx;
	m_nextNodePos[1] = (long)fy;

	m_bHaveNextNodePos = true;
}


// 单腿走路问题原因：
// 1 客户端先完成了moveto，已经播idle动作
// 2 服务器端将要更新时，怪物移动了一个小小的距离，客户端不能实现此同步

void CRegionAI::SetStartNode(long* pos)
{
}

void CRegionAI::SetGuard(CRegionCreature* pGuard)
{
}

void CRegionAI::CancelGuard(DWORD iUserID)
{
}

void CRegionAI::SetToAnimationState(std::string* psAnimation, short iDurTime)
{
	if(!psAnimation || psAnimation->empty() || iDurTime < 500 || iDurTime > 5000)
		return;
	m_state = STATE_ANIMATION;
	m_AnimDurativeTime = iDurTime;
	m_PlayAnimName = *psAnimation;
	m_creature->UpdataAnimationToClient(m_PlayAnimName.c_str());
}

void CRegionAI::TalkSuccor(void)
{
	if(m_creature->IsUser() || !m_creature->m_npcInfo)
		return;
	if(!m_isBeAttacked)
		return;

	if(rtGetMilliseconds() - m_tTalkTime > c_tTalkCycleTime)
	{
		m_tTalkTime = rtGetMilliseconds();
		const char *pTalk = g_TableNpcRandomTalk.FindRandomSuccorTextByID(m_creature->m_npcInfo->Id);
		if(pTalk)
			m_creature->Chat(1, pTalk, m_creature->m_viewRange);
	}
}

void CRegionAI::TalkNormal(void)
{
	if(m_creature->IsUser() || !m_creature->m_npcInfo)
		return;
	if(m_isBeAttacked)
		return;

	if(rtGetMilliseconds() - m_tTalkTime > c_tTalkCycleTime)
	{
		if(rand() % 4 > 0)
			return;
		m_tTalkTime = rtGetMilliseconds();
		const char *pTalk = g_TableNpcRandomTalk.FindRandomTalkTextByID(m_creature->m_npcInfo->Id);
		if(pTalk)
			m_creature->Chat(1, pTalk, m_creature->m_viewRange);
	}
}

void CRegionAI::TalkOver(void)
{
	if(m_creature->IsUser() || !m_creature->m_npcInfo)
		return;
	m_tTalkTime = rtGetMilliseconds();
	const char *pTalk = g_TableNpcRandomTalk.FindRandomOverTextByID(m_creature->m_npcInfo->Id);
	if(pTalk)
		m_creature->Chat(1, pTalk, m_creature->m_viewRange);
}

bool CRegionAI::ApplyMove()
{
#ifdef _DEBUG
	if (m_creature->m_pathNode == NULL)
	{
		if (_need_debug(m_creature))
			RtCoreLog().Debug("#%d CRegionAI::ApplyMove return becaulse creature path node is null \n");
		return false;
	}
#else
	if (m_creature->m_pathNode == NULL) return false;
#endif
	int count = m_creature->m_pathNode->PathCount();

#ifdef _DEBUG
	if (_need_debug(m_creature))
	{
		RtCoreLog().Debug("#%d CRegionAI::ApplyMove::PathCount: %d \n",
				m_creature->m_oId,
				count);
	}
#endif
	if (count < 1)
	{
		m_bHaveNextNodePos = false;
		return false;
	}

	int x, y;
	float fx, fy;

	if (!m_bHaveNextNodePos)
	{
		if (!m_creature->m_pathNode->Next(x, y))
			return false;

#ifdef _DEBUG
		if (_need_debug(m_creature))
		{
			RtCoreLog().Debug("#%d CRegionAI::ApplyMove::PathNext1: x:%d y:%d \n",
				m_creature->m_oId,
				x, y);
		}
#endif
		OnCallPathNextSuccessed(x, y, fx, fy);

	}

	m_currNodePos[0] = m_creature->m_pos[0];
	m_currNodePos[1] = m_creature->m_pos[1];
	
	//PZH
	long MoveSpace = m_creature->GetPlayerLogic()->GetMoveSpeed();
	//

	int nMoveLength = (/*PZH*//*m_creature->m_moveSpeed*/MoveSpace * RS_AI_UPDATE_STATE_DURATION) / 1000;
	int nNextNodeLength = sqrt((float)(m_nextNodePos[0] - m_currNodePos[0]) * (float)(m_nextNodePos[0] - m_currNodePos[0]) + (float)(m_nextNodePos[1] - m_currNodePos[1]) * (float)(m_nextNodePos[1] - m_currNodePos[1]));
	int nDelta = nMoveLength - nNextNodeLength;

	int nCycleSafeCounter = 0;
	while (nDelta>=0 && nCycleSafeCounter<20)
	{
		if (!m_creature->m_pathNode->Next(x, y) )
		{
			nMoveLength = nNextNodeLength;
			break;
		}

		nMoveLength -= nNextNodeLength;

#ifdef _DEBUG
		if (_need_debug(m_creature))
		{
			RtCoreLog().Debug("#%d CRegionAI::ApplyMove::PathNext2: x:%d y:%d \n",
				m_creature->m_oId,
				x, y);
		}
#endif
		OnCallPathNextSuccessed(x, y, fx, fy);

		nNextNodeLength = sqrt((float)(m_nextNodePos[0] - m_currNodePos[0]) * (float)(m_nextNodePos[0] - m_currNodePos[0]) + (float)(m_nextNodePos[1] - m_currNodePos[1]) * (float)(m_nextNodePos[1] - m_currNodePos[1]));
		nDelta = nMoveLength - nNextNodeLength;

		nCycleSafeCounter++;
	}

	if (nCycleSafeCounter>=20)
	{
		ERR("CRegionAI:ApplyMove error! Now Exit!\n");
		CHECK(0);
		m_bHaveNextNodePos = false;
		return false;
	}

	if (nNextNodeLength==0)
	{
		m_bHaveNextNodePos = false;
		return false;
	}

	long toPos[3];
	if (m_nextNodePos[0] != m_currNodePos[0])
	{
		if (m_nextNodePos[1] != m_currNodePos[1]) // 45度
			toPos[0] = m_currNodePos[0] + nMoveLength *	0.7071 * (m_nextNodePos[0] >= m_currNodePos[0] ? 1:-1);
		else 
			toPos[0] = m_currNodePos[0] + nMoveLength * (m_nextNodePos[0] >= m_currNodePos[0] ? 1:-1);
	}
	else 
		toPos[0] = m_currNodePos[0];

	if (m_nextNodePos[1] != m_currNodePos[1])
	{
		if (m_nextNodePos[0] != m_currNodePos[0]) // 45度
			toPos[1] = m_currNodePos[1] + nMoveLength *	0.7071 * (m_nextNodePos[1] >= m_currNodePos[1] ? 1:-1);
		else 
			toPos[1] = m_currNodePos[1] + nMoveLength * (m_nextNodePos[1] >= m_currNodePos[1] ? 1:-1);
	}
	else
		toPos[1] = m_currNodePos[1];

	// 避免宠物川帮
	if (m_type==TYPE_PET || m_type==TYPE_CALLNPC)
	{
		if (m_creature && m_creature->m_master && m_creature->GetCurrentTarget()==m_creature->m_master)
		{
			if (TestCollision(toPos, m_creature->m_model, m_creature->m_master->m_pos, m_creature->m_master->m_model))
			{
#ifdef _DEBUG
				if (_need_debug(m_creature))
					ERR("Pet Collision\n");
#endif				
				m_creature->m_pathNode->ClearPath();
				SetState(STATE_FOLLOW);
				m_creature->RemoveAllTargets();
				m_bHaveNextNodePos = false;
				return false;
			}
		}
	}

	int xx,yy;
	m_creature->m_scene->m_pTerrain->GetTerrainTile(toPos[0], toPos[1], xx, yy);

	bool res = m_creature->SetPosition(toPos[0], toPos[1], 0);
	if (!res)
		m_bHaveNextNodePos = false;

	return res;
}

CRegionAI::CRegionAI(Type type)
: m_type(type)
, m_creature(NULL)
, m_state(STATE_MOVE)
, m_moveFur(0)
, m_freezeTimeStamp(0)
, m_freezeDuration(0)
, m_lastAttackTime(0)
, m_AnimDurativeTime(0)
, m_tTalkTime(0)
, c_tTalkCycleTime(150000)
{
	m_targetPos[0] = ~0;
	m_targetPos[1] = ~0;

	m_bHaveNextNodePos = false;

	m_pConjureAI = NULL;
	m_isBeAttacked = false;
	m_bTrace = false;
	m_bActive = true;
	m_dwForceReturnTime = 0;
}

bool CRegionAI::OnBorn(CRegionCreature* creature)
{ 
	m_creature = creature;

	m_freezeTimeStamp = rtGetMilliseconds() + m_freezeDuration;

	//m_creature->m_npcInfo->AddEventCommand("delself", "killself", "1000");

	return true;
}

bool CRegionAI::OnBeAttacked(CRegionCreature* target, int hate)
{
	TalkSuccor();
	return true; 
}

bool CRegionAI::OnAttack(CRegionCreature* target)
{
	return true;
}

bool CRegionAI::OnArrivePoint()
{
	return true;
}

bool CRegionAI::OnAssist(CRegionCreature* target)
{
	return true;
}

bool CRegionAI::OnSuccor(CRegionCreature* target)
{
	return true;
}

bool CRegionAI::OnFollow(CRegionCreature* target) 
{ 
	return true; 
}

bool CRegionAI::OnDead() 
{ 
	m_creature = NULL;

	return true;
}

void CRegionAI::SetActive(bool bActive)
{
	m_bActive = bActive;
	m_bHaveNextNodePos = false;
}

bool CRegionAI::OnThink()
{
	if (!m_bActive) return false;

	TalkNormal();

	//5.13 LeiJun添加原地播放动画状态
	if(m_state == STATE_ANIMATION)
	{
		m_AnimDurativeTime -= 500;
		if(!m_PlayAnimName.empty() && m_AnimDurativeTime <= 0)
		{
			m_state = STATE_MOVE;
			m_AnimDurativeTime = 0;
			m_PlayAnimName.clear();
			if(m_creature->IsScriptLoaded() && !m_creature->IsUser())
				m_creature->CallScript("EndOriginAnimation", false);
			return true;
		}
		return false;
	}
	return true;
}

CRegionMonsterAI::CRegionMonsterAI()
: CRegionAI(TYPE_MONSTER)
, m_patrolInertia(0)
, m_patrolSpeed(2)
{
	m_bHaveIdlePos = false;
	m_SelfRewardNum = SELF_REWARD_NUM;
	m_isBeAttacked = false;
}

bool CRegionBossAI::OnAssist(CRegionCreature* cre)
{
	return CRegionMonsterAI::OnAssist(cre);
}

bool CRegionMonsterAI::OnAssist(CRegionCreature* cre)
{
	if (!CRegionAI::OnAssist(cre))
		return false;

	if (m_state == STATE_COMBAT)
		return true;

	SetState(STATE_COMBAT);
	m_creature->SetTarget(cre);

	return true;
}

bool CRegionMonsterAI::OnSuccor(CRegionCreature* target)
{
	if (!CRegionAI::OnSuccor(target))
		return false;
	
	if(target == NULL)
	{
		return false;
	}

	if(m_state == STATE_SUCCOR)
	{
		return true;
	}

	m_proTargetID = target->m_oId;
	
	SetState(STATE_SUCCOR);

	return true;
}

void CRegionMonsterAI::CryForHelp()
{
	if(!m_isBeAttacked) return;

	if(m_creature->m_npcInfo != NULL)
	{
		if(m_creature->m_npcInfo->ProtecterID != 0)
		{
			m_creature->AddProtector();

			if(!m_creature->m_protectorList.empty())
			{
				std::list<CRegionCreature*>::iterator it;

				for(it = m_creature->m_protectorList.begin(); it != m_creature->m_protectorList.end(); it++)
				{
					if((*it)->m_ai)
					{
						(*it)->m_ai->OnSuccor(m_creature);
					}
				}
			}
		}
	}

	if(CONJURE(m_dwLevelFlag) == AI_CONJURE)
	{
		if(m_pConjureAI)
		{
			if(m_pConjureAI->CanUseSkill())
			{
				m_pConjureAI->UseSkill();
			}
		}
	}
}

void CRegionMonsterAI::SelfSuccor()
{
	if(m_SelfRewardNum <= 0) return;
	
	if(m_creature->m_core.GetHp() < m_creature->m_core.GetMaxHp() / 2)
	{
		//查找有益技能并施放
		CSkillInstance* p_instance = m_creature->m_Skill.FirstActiveSkill();

		while(p_instance && p_instance->m_pAttr && p_instance->m_pAttr->bABad)
		{
			p_instance = m_creature->m_Skill.NextActiveSkill();
		}

		if(p_instance && p_instance->m_pAttr)
		{
			int skill = p_instance->m_pAttr->wID;
			if(!m_creature->m_Skill.CanUseSkill(skill,&(m_creature->m_Skill)))
				skill = 0;

			if(skill != 0)
			{
				SAttackParam param;
				param.Seed = rand()%60000;
				param.EnemyId = m_creature->m_oId;
				param.Skill = skill;
				param.TargetType = 1;

				m_creature->UseSkill(param);

				m_SelfRewardNum--;
			}
		}
	}
}

void CRegionMonsterAI::SuccorMode(CRegionCreature* target)
{
	if(target == NULL)
	{
		return;
	}

	if(target->m_core.GetHp() < target->m_core.GetMaxHp() / 2)
	{
		m_SuccorMode = SUCCOR_MODE_REWARD;
	}else
	{
		m_SuccorMode = SUCCOR_MODE_HARM;
	}

	return;
}

bool CRegionMonsterAI::BeginReturn()
{
	SetState(STATE_RETURN);

	if(m_creature->IsScriptLoaded())
	{
		m_creature->CallScript("LeaveBattle", false);
	}

	//获得当前攻击目标

	CRegionCreature* target = m_creature->GetCurrentTarget();	

	if ( target )
	{
		target->LeaveCombat();
	}
	m_creature->RemoveAllTargets();
	m_creature->m_core.SetHp(m_creature->m_core.GetMaxHp());
	m_creature->m_core.SetMp(m_creature->m_core.GetMaxMp());
	m_creature->m_Skill.DeleteUIDEffect(0, true);

	bool result = UpdateMovePath(m_orignalPos[0], m_orignalPos[1]);
	if (!result)
		SetState(STATE_MOVE);
	else
		m_dwForceReturnTime = rtGetMilliseconds() + 5000;

	m_isBeAttacked = false;

	if (!result && m_creature->m_pos[0] == m_orignalPos[0] && m_creature->m_pos[1] == m_orignalPos[1])
	{
		result = true;
	}
	return true;
}
 
bool CRegionMonsterAI::OnThink()
{
	if (!CRegionAI::OnThink())
		return false;

	if (!m_creature->CanMove())
		return false;
	
	switch(m_state)
	{
	//闲散状态
	case STATE_MOVE:
		m_bTrace = false;
		if(!DoScanTarget())
		{
			DoMove();
		}
		else
		{
			SetState(STATE_COMBAT);

			if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
			{
				//保存当前位置
				SaveCurrPos();
			}
		}
		break;

	//返回状态
	case STATE_RETURN:
		m_bTrace = false;
		DoReturn();
		break;

	//战斗状态
	case STATE_COMBAT:

		m_bTrace = true;

		SelfSuccor();

		if (m_creature->IsOutOfRange(m_orignalPos, m_creature->m_traceRange))
		{
#ifdef _DEBUG
			if (_need_debug(m_creature))
				RtCoreLog().Debug("#%d out_of_range, return now\n", m_creature->m_oId);
#endif
			if (!BeginReturn())
			{
				m_creature->m_pos[0] = m_orignalPos[0];
				m_creature->m_pos[1] = m_orignalPos[1];
			}
		}		
		else
		{			
			CryForHelp();
			
			//获得当前攻击目标
			CRegionCreature* target = m_creature->GetCurrentTarget();
			if (target)
			{
				//Tianh 如果目标死亡  跟随玩家
				if (0 == target->m_core.GetHp())
				{
					target = m_creature->m_master;

					m_state = STATE_FOLLOW;
				}
			}
			if(! (m_creature->m_npcInfo->Boss))//如果是BOSS
			{
				//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
				if ((target != NULL) && (target->m_IsHiding != 0))
				{
					target->LeaveCombat();
					break;
				}
			}
	

			if(target != NULL && /*PZH*//*target->m_bDead == false*/(target->GetPlayerLogic()->IsLive())/**/)
			{
				target->EnterCombat();
				OnCombat(target);
			}else
			{
				m_SelfRewardNum = SELF_REWARD_NUM;

				SetState(STATE_MOVE);

				m_isBeAttacked = false;
				if (m_creature->m_targets.empty())
					BeginReturn();
			}
		}
		break;

	//救援状态
	case STATE_SUCCOR:
		{
			m_bTrace = true;
			if (m_creature->IsOutOfRange(m_orignalPos, m_creature->m_traceRange))
			{
				BeginReturn();
				break;
			}
			
			//获得当前救援目标
			CRegionCreature* target = m_creature->m_scene->FindCreature(m_proTargetID);

			if(target != NULL)
			{
				/*if (m_creature->Distance(target->m_pos) > 60)
				{
					DoTrace(target);
					break;
				}*/
				
				SuccorMode(target);

				if(m_SuccorMode == SUCCOR_MODE_REWARD)
				{
					//查找有益技能并施放
					CSkillInstance* p_instance = m_creature->m_Skill.FirstActiveSkill();

					while(p_instance && p_instance->m_pAttr && p_instance->m_pAttr->bABad)
					{
						p_instance = m_creature->m_Skill.NextActiveSkill();
					}

					if(p_instance && p_instance->m_pAttr)
					{
						int skill = p_instance->m_pAttr->wID;
						if(!m_creature->m_Skill.CanUseSkill(skill,&(target->m_Skill)))
							skill = 0;
						
						if(skill != 0)
						{
							SAttackParam param;
							param.Seed = rand()%60000;
							param.EnemyId = m_proTargetID;
							param.Skill = skill;
							param.TargetType = 1;

							m_creature->UseSkill(param);

							break;
						}
					}
				}

				//获得当前攻击目标
				CRegionCreature* enemy_target = m_creature->GetCurrentTarget();

				if(enemy_target != NULL)
				{
					OnCombat(enemy_target);
				}else
				{
					CRegionCreature* enemy = target->GetCurrentTarget();

					if(enemy != NULL)
					{
						m_creature->SetTarget(enemy);
					}else
					{
						SetState(STATE_MOVE);

						m_isBeAttacked = false;
					}
				}					
				
			}else
			{
				SetState(STATE_MOVE);

				m_isBeAttacked = false;
			}
			
			break;
		}

	//逃跑状态
	case STATE_RUNAWAY:
		m_bTrace = false;
		if (IsOutOfRange(m_creature->m_pos, m_orignalPos, 500))
		{
			LOG1("AI(oId=%d): 超出逃跑范围\n", m_creature->m_oId);
			BeginReturn();
		}
		else
		{
			DoRunaway();
		}
		break;

	//其他
	default:
		m_bTrace = false;
		SetState(STATE_MOVE);
		m_isBeAttacked = false;
		break;
	}

	if (m_creature->GetCurrentTarget() && m_creature->Distance(m_creature->GetCurrentTarget()->m_pos) > 300)
	{
		m_creature->GetCurrentTarget()->LeaveCombat();
	}

	return true;
}

CRegionCreature* CRegionMonsterAI::DoScanTarget()
{  
	if (m_creature->m_viewRange == 0)
		return NULL;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos, 
		m_creature->m_viewRange, creList, m_creature);
 
	if (creList.empty())
		return NULL;

	int viewRange = m_creature->m_viewRange * m_creature->m_viewRange;
	int minRange = viewRange + 1;

	std::vector<CRegionCreature*>::iterator nearest = creList.end();
	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		//LeiJun 是所属用户则不能是目标
		if(m_creature->m_master && m_creature->m_master == (*iter))
			continue;

		//不是玩家且不是旅游型NPC就不作为目标
		if ( !(*iter)->IsUser() && !(*iter)->IsUserPet() && !(*iter)->IsCallNpc() &&
			((*iter)->m_ai && (*iter)->m_ai->GetType() != TYPE_TOUR) )
			continue;

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_traceRange))
			continue;

		if ( !(*iter)->State_IsVisible() || /*PZH*//*(*iter)->m_bDead == true*/ !(*iter)->GetPlayerLogic()->IsLive())
			continue;

		int x = (*iter)->m_pos[0] - m_creature->m_pos[0];
		int y = (*iter)->m_pos[1] - m_creature->m_pos[1];

		int range = x * x + y  * y;

		if (range < minRange && range <= viewRange)
		{
			minRange = range;
			nearest = iter;
		}
	}

	if (nearest == creList.end())
		return NULL;

	if (*nearest)
	{
		m_creature->SetTarget(*nearest);
	}

	return *nearest;
}

bool CRegionMonsterAI::OnAttack(CRegionCreature* target)
{
	if (!CRegionAI::OnAttack(target))
		return false;

	if (m_state == STATE_COMBAT)
	{
		LOG1("AI(oId=%d): 正处于战斗状态 Attack 指令被忽略\n", m_creature->m_oId);
		return true;
	}

	char det = rand() % 100;
	char rate = (char)(m_creature->m_bravePoint * 0.5f + m_creature->m_loyalPoint * 0.5f);

#ifdef _DEBUG
	LOG4("AI(old=%d): 攻击指令接受测试 [%s] (决定值=%d, 概率=%d)\n", 
		m_creature->m_oId, rate >= det ? "成功" : "失败", det, rate);
#endif // _DEBUG

	if (rate < det)
		return true;

	m_creature->RemoveAllTargets();

	if (target)
	{
		m_creature->SetTarget(target);

		SetState(STATE_COMBAT);
	}

	return true;
}

void CRegionMonsterAI::SeekHelp()
{
	if (m_creature->m_faction == 0)
		return;

	if (m_creature->m_assistRange == 0)
		return;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos, m_creature->m_assistRange, creList,m_creature);

	if (creList.empty())
		return;

	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_faction != m_creature->m_faction)
			continue;

		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_assistRange))
			continue;

		if ((*iter)->m_ai)
		{
			if ((*iter)->m_ai->GetState() != STATE_COMBAT)
				(*iter)->m_ai->OnAssist(m_creature);
		}
	}
}

bool CRegionMonsterAI::OnBeAttacked(CRegionCreature* target, int hate)
{
	if (target == m_creature || target == m_creature->m_master)
		return false;
	
	if (m_state==STATE_RETURN || m_state==STATE_RUNAWAY)
		return false;
	
	if (!CRegionAI::OnBeAttacked(target, hate))
		return false;	
	
	if ((m_state != STATE_COMBAT) &&
		(m_state != STATE_SUCCOR))
		SetState(STATE_COMBAT);
	
	// modify later
	// ...
	m_creature->AddTarget(target, hate, hate);

	//SeekHelp();

	m_isBeAttacked = true;

	//if(CONJURE(m_dwLevelFlag) == AI_CONJURE)
	//{
	//	if(m_pConjureAI)
	//	{
	//		if(m_pConjureAI->CanUseSkill())
	//		{
	//			m_pConjureAI->UseSkill();
	//		}
	//	}
	//}

//	return OnThink();
	return true;
}

void CRegionMonsterAI::DoReturn()
{
	static long oldx = 0;
	static long oldy = 0;
	
	if (m_creature->Distance(m_TargetNode) < 30)
	{
		m_dwForceReturnTime = 0;
		SetState(STATE_MOVE);

		m_Arrived = true;
		m_Left	  = false;

		return;
	}

	if (m_dwForceReturnTime && rtGetMilliseconds() > m_dwForceReturnTime)
	{
		m_creature->m_pos[0] = m_orignalPos[0];
		m_creature->m_pos[1] = m_orignalPos[1];
		m_creature->m_pos[2] = m_orignalPos[2];
		m_TargetNode[0] = m_orignalPos[0];
		m_TargetNode[1] = m_orignalPos[1];
		m_TargetNode[2] = m_orignalPos[2];
		m_creature->m_pathNode->ClearPath();
		m_bHaveNextNodePos = false;
	}
	if (/*m_creature->CanMove() && */!ApplyMove())
	{
		if (UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
		{
			if (!ApplyMove())
			{
				m_dwForceReturnTime = 0;
				SetState(STATE_MOVE);
			}
		}else 
		{
			m_dwForceReturnTime = 0;
			SetState(STATE_MOVE);
		}
	}else
	{
		if( (oldx == m_creature->m_pos[0]) &&
			(oldy == m_creature->m_pos[1]))
		{
			CRegionCreature*	obstacle = DoScanTarget();
			
			if(obstacle)
			{
				m_dwForceReturnTime = 0;
				OnCombat(obstacle);
			}
		}else
		{
			oldx = m_creature->m_pos[0];
			oldy = m_creature->m_pos[1];
		}
	}
}

void CRegionMonsterAI::DoMove()
{
	switch(MOVETYPE(m_dwLevelFlag))
	{
	case AI_FIXED:
		DoIdle();
		break;
	case AI_PATROL:
		DoPatrol();
		break;
	case AI_IDLE:
		DoIdle();
		break;
	default:
		DoIdle();
		break;
	}
}

void CRegionMonsterAI::DoPatrol()
{
	CHECK(m_state == STATE_MOVE);

	if(MOVETYPE(m_dwLevelFlag) != AI_PATROL)
	{
		return;
	}

	/*if ((m_creature->m_patrolRange<=0) ||
		m_PatrolPath.empty())
	{
		return;
	}*/

	//m_Arrived：是否到达目标节点，此点不仅仅是巡逻路径上的节点
	if ( !m_Arrived && !ApplyMove())
	{
		//
	}

	if (m_creature->Distance(m_TargetNode) < 40)
	{
		m_Arrived = true;
		m_Left	  = false;
	}
	else
	{
		m_Arrived = false;
	}

	//已到达目标节点
	if(m_Arrived)
	{		
		//判断是否到达巡逻终点		
		if((++ m_IterNode) == m_PatrolPath.end())
		{
			//若到终点，返回
			m_PatrolPath.reverse();
			m_IterNode = m_PatrolPath.begin();
			m_IterNode ++;
		}
	}

	//重新设定巡逻目标
	m_TargetNode[0] = (*m_IterNode).x;
	m_TargetNode[1] = (*m_IterNode).y;

	if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	{
		//do nothing
	}	
}

void CRegionMonsterAI::DoIdle()
{
	CHECK(m_state == STATE_MOVE);

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		return;
	}

	if (m_creature->m_patrolRange<=0)
		return;

	if (m_bHaveIdlePos)
	{
		if (!ApplyMove())
			m_bHaveIdlePos = false;
	}
	else 
	{
		int nNewPlace = rand() % 4;
		if (nNewPlace!=0)
			return;

		// Find a random place to go
		int range;
		if (m_creature->m_patrolRange==0)
			range = 80;
		else 
			range = m_creature->m_patrolRange * 0.5;

		long ranX = rand() % (range*2);
		long ranY = rand() % (range*2);
		ranX -= range;
		ranY -= range;

		m_idleToPos[0] = m_orignalPos[0] + ranX;
		m_idleToPos[1] = m_orignalPos[1] + ranY;

		m_bHaveIdlePos = true;

		if (!UpdateMovePath(m_idleToPos[0], m_idleToPos[1]))
		{
		}
		else
		{
		}
	}
}

bool CRegionMonsterAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;
	
	//初始化
	m_orignalPos[0] = creature->m_pos[0];
	m_orignalPos[1] = creature->m_pos[1];
	m_orignalPos[2] = creature->m_pos[2];	

	m_creature->m_lockTarget = false;
	
	m_Arrived	   = false;
	m_Left		   = false;

	//巡逻目标点初始化
	m_PatrolPath = creature->m_npcInfo->PatrolPath;

	//有巡逻AI
	if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
	{
		if(m_PatrolPath.empty())
		{
			LOG("This is PartolAI but patrol_path is empty \n");
            m_dwLevelFlag &= (~AI_PATROL);
            m_dwLevelFlag |= AI_IDLE;
					
			m_TargetNode[0] = creature->m_pos[0];
			m_TargetNode[1] = creature->m_pos[1];		
		}else
		{
		    m_IterNode = m_PatrolPath.begin();
			m_TargetNode[0] = (*m_IterNode).x;
			m_TargetNode[1] = (*m_IterNode).y;

			m_orignalPos[0] = m_TargetNode[0];
			m_orignalPos[1] = m_TargetNode[1];
			m_orignalPos[2] = m_TargetNode[2];

			m_creature->SetPosition(m_orignalPos[0], m_orignalPos[1], 0);
		}
	}else
	{
		m_TargetNode[0] = creature->m_pos[0];
		m_TargetNode[1] = creature->m_pos[1];
	}

	//有召唤AI
	if(CONJURE(m_dwLevelFlag) == AI_CONJURE)
	{
		m_pConjureAI = RT_NEW CRegionConjureAI(creature, CONJURE_SKILL_ID, m_creature->m_npcInfo->ConjureTarID);
	}else
	{
		m_pConjureAI = NULL;
	}

	m_state = STATE_MOVE;

	return true;
}

CRegionPetAI::CRegionPetAI()
: CRegionAI(TYPE_PET)
{
	m_eAS = AS_SYNERGY;
	m_pDefSkill = NULL;
	m_pCurSkill = NULL;
}

CRegionPetAI::~CRegionPetAI()
{
	m_pDefSkill = NULL;
	m_pCurSkill = NULL;
}

void CRegionPetAI::BeginFollow(CRegionCreature* creature)
{
	SetState(STATE_FOLLOW);
	m_creature->SetTarget(creature);
	m_creature->m_lockTarget = true;
	UpdateMovePath(creature->m_pos[0], creature->m_pos[1]);
}

void CRegionPetAI::DoFollow(CRegionCreature* target)
{
	if (m_targetPos[0] != target->m_pos[0] || m_targetPos[1] != target->m_pos[1])
	{
		if (!UpdateMovePath(target->m_pos[0], target->m_pos[1]))
		{	
			return;
		}		

		m_targetPos[0] = target->m_pos[0];
		m_targetPos[1] = target->m_pos[1];
	}

	//RtCoreLog().Debug("CRegionPetAI::DoFollow target name %s.\n",
	//	target->m_userInfo->m_username.c_str());

	if (m_creature->CanMove() && !ApplyMove())
	{
		if (UpdateMovePath(target->m_pos[0], target->m_pos[1]))
			ApplyMove();
	}
}

void CRegionAI::DoAttack()
{	
	int seed = rand() % 100;
	int skill = 0;

	for(int ii=0; ii<NPC_MAX_SKILL; ii++)
	{
		if (m_creature->m_skillAttackRate[ii] > rand() % 100)
		{
			CSkillInstance* pInstance = m_creature->m_Skill.GetSkillByIndex(ii);
			if (pInstance)
			{
				skill = pInstance->m_pAttr->wID;
				if(!m_creature->m_Skill.CanUseSkill(skill,&(m_creature->GetCurrentTarget()->m_Skill)))
					skill = 0;
				else
				{
					break;
				}
			}
		}
	}

	m_targetPos[0] = m_creature->m_pos[0];
	m_targetPos[1] = m_creature->m_pos[1];
	
	int now = rtGetMilliseconds();
	int attackTime = m_lastAttackTime + m_creature->m_core.GetAttackInterval();

	if (now >= attackTime)
	{
		//NOTICE("AI: attackLastTime:%d, attackIntervalTime:%d\n",(int)now - attackTime,(int)m_creature->m_core.GetAttackInterval());

		if (m_creature->GetCurrentTarget() == m_creature->m_master)
		{
			return;
		}
		// modify by carl
		SAttackParam param;
		param.Seed = rand()%60000;
		param.EnemyId = m_creature->GetCurrentTarget()->GetObjectId();
		param.Skill = skill;
		param.TargetType = 1;
		
		// fix later
		// ...
		if(skill == 0)
            m_creature->DoAttack(param.EnemyId,param.Seed);
        else
            m_creature->UseSkill(param);

		m_lastAttackTime = now;
	}
}

void CRegionAI::DoTrace(CRegionCreature* target)
{
	if (m_state != STATE_COMBAT)
	{
		m_targetPos[0] = ~0;
		m_targetPos[1] = ~0;

		SetState(STATE_COMBAT);
	}

	//if (m_targetPos[0] != target->m_pos[0] || m_targetPos[1] != target->m_pos[1])
	//{
	//	int dx, dy;
	//	
	//	if (m_targetPos[0] != ~0)
	//		dx = target->m_pos[0] - m_targetPos[0];
	//	else
	//		dx = 0;

	//	if (m_targetPos[1] != ~0)
	//		dy = target->m_pos[1] - m_targetPos[1];
	//	else
	//		dy = 0;

	//	if (!UpdateMovePath(target->m_pos[0] + dx, target->m_pos[1] + dy))
	//	{	
	//		return;
	//	}

	//	m_targetPos[0] = target->m_pos[0] + dx;
	//	m_targetPos[1] = target->m_pos[1] + dy;
	//}

	if (m_targetPos[0] != target->m_pos[0] || m_targetPos[1] != target->m_pos[1])
	{
#ifdef _DEBUG
	if (_need_debug(m_creature))
		RtCoreLog().Debug("#%d CRegionAI::DoTrace::Update path to correct the target pos.\n",
				m_creature->m_oId);
#endif
		if (!UpdateMovePath(target->m_pos[0], target->m_pos[1]))
		{	
			return;
		}

		m_targetPos[0] = target->m_pos[0];
		m_targetPos[1] = target->m_pos[1];
	}

	if (m_creature->CanMove() && !ApplyMove())
	{
#ifdef _DEBUG
	if (_need_debug(m_creature))
		RtCoreLog().Debug("#%d CRegionAI::DoTrace::Update path for can't ApplyMove.\n",
				m_creature->m_oId);
#endif
		if (UpdateMovePath(target->m_pos[0], target->m_pos[1]))
			ApplyMove();
	}
}

CRegionCallNpc::CRegionCallNpc()
: CRegionAI(TYPE_CALLNPC)
{
	m_eAS = AS_SYNERGY;
}

CRegionCallNpc::~CRegionCallNpc()
{

}

bool CRegionCallNpc::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	//初始化
	//m_orignalPos[0] = creature->m_pos[0];
	//m_orignalPos[1] = creature->m_pos[1];
	//m_orignalPos[2] = creature->m_pos[2];	

	//m_creature->m_lockTarget = true;
	//m_pConjureAI = NULL;

	return true;
}

bool CRegionCallNpc::OnAttack(CRegionCreature* target)
{
	if (!CRegionAI::OnAttack(target))
		return false;

	m_creature->m_lockTarget = false;

	if (m_state != STATE_COMBAT)
	{
		SetState(STATE_COMBAT);
		m_creature->SetTarget(target);
	}
	else
	{
		m_creature->AddTarget(target, 1, 1);
	}

	return true;
}

bool CRegionCallNpc::OnFollow(CRegionCreature* target)
{
	if (!CRegionAI::OnFollow(target))
		return false;

	BeginFollow(target);

	return true;
}

void CRegionCallNpc::BeginFollow(CRegionCreature* creature)
{
	SetState(STATE_FOLLOW);
	m_creature->SetTarget(creature);
	m_creature->m_lockTarget = true;
	UpdateMovePath(creature->m_pos[0], creature->m_pos[1]);
}

void CRegionCallNpc::DoFollow(CRegionCreature* target)
{
	if (m_targetPos[0] != target->m_pos[0] || m_targetPos[1] != target->m_pos[1])
	{
		if (!UpdateMovePath(target->m_pos[0], target->m_pos[1]))
		{	
			return;
		}		

		m_targetPos[0] = target->m_pos[0];
		m_targetPos[1] = target->m_pos[1];
	}

	//RtCoreLog().Debug("CRegionPetAI::DoFollow target name %s.\n",
	//	target->m_userInfo->m_username.c_str());

	if (m_creature->CanMove() && !ApplyMove())
	{
		if (UpdateMovePath(target->m_pos[0], target->m_pos[1]))
			ApplyMove();
	}
}

bool CRegionCallNpc::OnThink()
{
	if (!CRegionAI::OnThink())
		return false;

	// 如果离主人距离过远，放弃当前状态，直接传送，进入follow状态
	if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, 500))
	{
		m_creature->ForceSetPosition(m_creature->m_master->m_pos[0],m_creature->m_master->m_pos[1],
			m_creature->m_master->m_pos[2]);


		BeginFollow(m_creature->m_master);

		m_bHaveNextNodePos = false;//当正在战斗时，有时候会残留上次的下一点坐标导致宠物瞬移后往回跑，这里强制转瞬后取新的路径点
		return true;
	}

	CRegionCreature* target = m_creature->GetCurrentTarget();

	if (target)
	{
		//Tianh 如果目标死亡  跟随玩家
		if (0 == target->m_core.GetHp())
		{
			target = m_creature->m_master;

			m_state = STATE_FOLLOW;
		}
	}


	//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
	if ((target != NULL) && (target->m_IsHiding != 0))
	{
		target->LeaveCombat();
		return false;
	}

	//if (target->IsUser())
	//{
	//	m_eAS = AS_SYNERGY;
	//}

	if (target == NULL)
	{
		if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, m_creature->m_traceRange))
		{	

		}
		else
		{
			if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, 20))
			{

				BeginFollow(m_creature->m_master);
			}
		}
	}
	else
	{
		if (m_state == STATE_COMBAT)
		{
			// if (m_creature->IsOutOfRange(target->m_pos, m_creature->m_traceRange / 2) ||
			//	m_creature->IsOutOfRange(m_creature->m_master->m_pos, m_creature->m_traceRange / 2))
			if (m_creature->IsOutOfRange(target->m_pos, 300) ||
				m_creature->IsOutOfRange(m_creature->m_master->m_pos, 400) ||
				m_eAS == AS_DEF )
			{
				BeginFollow(m_creature->m_master);
			}
			else if (CanAttack(target))
			{
				DoAttack();
			}
			else
			{
				DoTrace(target);
			}
		}
		else if (m_state == STATE_FOLLOW)
		{
			if (m_eAS == AS_ATTACK)
			{
				if(!DoScanTarget())
					DoFollow(target);
				else
					SetState(STATE_COMBAT);
			}
			else
			{
				DoFollow(target);
			}
		}
	}

	return true;
}

CRegionCreature* CRegionCallNpc::DoScanTarget()
{  
	if (m_creature->m_viewRange == 0)
		return NULL;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos, 
		m_creature->m_viewRange, creList, m_creature);

	if (creList.empty())
		return NULL;

	int viewRange = m_creature->m_viewRange * m_creature->m_viewRange;
	int minRange = viewRange + 1;

	std::vector<CRegionCreature*>::iterator nearest = creList.end();
	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		// 不是怪物,则忽略
		if ( !(*iter)->IsMonster())
			continue;

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_traceRange))
			continue;

		if ( !(*iter)->State_IsVisible() || /*PZH*//*(*iter)->m_bDead == true*/ !(*iter)->GetPlayerLogic()->IsLive())
			continue;

		int x = (*iter)->m_pos[0] - m_creature->m_pos[0];
		int y = (*iter)->m_pos[1] - m_creature->m_pos[1];

		int range = x * x + y  * y;

		if (range < minRange && range <= viewRange)
		{
			minRange = range;
			nearest = iter;
		}
	}

	if (nearest == creList.end())
		return NULL;

	if (*nearest)
	{
		m_creature->SetTarget(*nearest);
	}
	return *nearest;
}


bool CRegionPetAI::OnFollow(CRegionCreature* target)
{
	if (!CRegionAI::OnFollow(target))
		return false;

	BeginFollow(target);

	return true;
}

CRegionAI* CRegionAIMgr::CreateAI(const std::string& name, DWORD ai_level)
{	
	//static int count = 0;
	
	if (m_ai.empty())
		SetTimer(CRE_TIMER_AI, RS_AI_UPDATE_STATE_DURATION);

	CRegionAI* ai;

	if (name == "pet")
		ai = RT_NEW CRegionPetAI;
	else if (name == "callnpc")
		ai = RT_NEW CRegionCallNpc;
	else if (name == "monster")
		ai = RT_NEW CRegionBossAI;
	else if (name == "guarder")
	{
		ai = RT_NEW CRegionGuarderAI;
	}
	else if (name == "s_guarder")
	{
		ai = RT_NEW CRegionAssistantAI;
	}else if(name == "hero")
	{
		ai = RT_NEW CRegionHeroAI;
	}
	else if(name == "g_monster")//tim.yang
	{
		ai = RT_NEW CGodLightMonsterAI;
	}
	else if (name == "mc_monster")
	{
		ai = RT_NEW CMCAI;
	}
	else if (name == "tour")
	{
		ai = RT_NEW CTourAI;
	}
	else 
	{
		ERR1("Not supported AI type: %s (Create default monster AI)", name.c_str());
		ai = RT_NEW CRegionBossAI;
	}

	//count++;
	//LOG1("New AI Object Num : [%d]\n", count);

	ai->SetLvFlag(ai_level);

	m_ai[int(ai)] = 0;

	return ai;
}

void CRegionAIMgr::CloseAI(CRegionAI* ai)
{	
	//static int count = 0;

	//count++;
	//LOG1("Delete AI Object Num : [%d]\n", count);

	m_ai.erase(int(ai));

	if (m_ai.empty())
		StopTimer(CRE_TIMER_AI);

	DEL_ONE(ai);
}

void CRegionAIMgr::OnTimer(ULONG id, DWORD dwParam[])
{
//	long t1 = rtGetMilliseconds();
	for(EXT_SPACE::unordered_map<int, int>::iterator ai = m_ai.begin(); ai != m_ai.end(); ai++)
	{
		ULONG lTest = (ULONG)(ai->first);
		if( ((CRegionAI*)(ai->first))->IsValid() )
			((CRegionAI*)(ai->first))->OnThink();
	}
//	LOG1("ai used tiem = %d\n", rtGetMilliseconds()-t1);
}

bool CRegionPetAI::OnThink()
{
	if (!CRegionAI::OnThink())
		return false;

	// 如果离主人距离过远，放弃当前状态，直接传送，进入follow状态
	if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, 600))
	{
		m_creature->ForceSetPosition(m_creature->m_master->m_pos[0],m_creature->m_master->m_pos[1],
			m_creature->m_master->m_pos[2]);
		

		BeginFollow(m_creature->m_master);

		m_bHaveNextNodePos = false;//当宠物正在战斗时，有时候会残留上次的下一点坐标导致宠物瞬移后往回跑，这里强制转瞬后取新的路径点
		return true;
	}

	CRegionCreature* target = m_creature->GetCurrentTarget();
	
	if (target)
	{
		//Tianh 如果目标死亡  跟随玩家
		if (0 == target->m_core.GetHp())
		{
			target = m_creature->m_master;

			m_state = STATE_FOLLOW;
		}
	}


	//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
	if ((target != NULL) && (target->m_IsHiding != 0))
	{
		target->LeaveCombat();
		return false;
	}


	if (m_pItemPet.params[PET_PARAM_STATE] && ItemIsPetSpectre(m_pItemPet))
	{
		m_creature->m_pos[0] = m_creature->m_master->m_pos[0];
		m_creature->m_pos[1] = m_creature->m_master->m_pos[1];
		m_creature->m_pos[2] = m_creature->m_master->m_pos[2];
		return true;
	}

	if (target == NULL)
	{
		if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, m_creature->m_traceRange))
		{	

		}
		else
		{
			if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, 20))
			{

				BeginFollow(m_creature->m_master);
			}
		}
	}
	else
	{
		if (m_state == STATE_COMBAT)
		{
			// if (m_creature->IsOutOfRange(target->m_pos, m_creature->m_traceRange / 2) ||
			//	m_creature->IsOutOfRange(m_creature->m_master->m_pos, m_creature->m_traceRange / 2))
			if (m_creature->IsOutOfRange(target->m_pos, 400) ||
				m_creature->IsOutOfRange(m_creature->m_master->m_pos, 400) ||
				m_eAS == AS_DEF )
			{
				BeginFollow(m_creature->m_master);
			}
			else if (CanAttack(target))
			{
				DoAttack();
			}
			else
			{
				DoTrace(target);
			}
		}
		else if (m_state == STATE_FOLLOW)
		{
			if (m_eAS == AS_ATTACK)
			{
				if(!DoScanTarget())
					DoFollow(target);
				else
					SetState(STATE_COMBAT);
			}
			else
			{
				DoFollow(target);
			}
		}
	}

	return true;
}

bool CRegionPetAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	m_creature->m_Skill.StartupSkill();

	if (m_creature && m_creature->m_master && m_creature->m_master->m_pItemContainer && m_creature->m_dwPetItemOID)
	{
		m_pItemPet = m_creature->m_master->m_pItemContainer->m_PetBag.GetItemFromID(m_creature->m_dwPetItemOID);
		
		for (int i=PET_PARAM_PASSIVE_1;i<PET_PARAM_PASSIVE_6+1;i++)
		{
			if (m_pItemPet.params[i])
			{
				m_creature->m_Skill.AddSkill(m_pItemPet.params[i], 0, false);
			}
		}
		for (int i=PET_PARAM_SKILL_1;i<PET_PARAM_SKILL_6+1;i++)
		{
			if (m_pItemPet.params[i])
			{
				m_creature->m_Skill.AddSkill(m_pItemPet.params[i], 0, false);
			}
		}
		

		return true;
	}

	if (m_pItemPet.params[PET_PARAM_STATE] && ItemIsPetSpectre(m_pItemPet))
		m_creature->State_SetVisible(false);

	return false;
}

void CRegionPetAI::DoAttack()
{	
	int seed = rand() % 100;
	int skill = 0;

	SSkill* pSkill = NULL;

	if (m_pCurSkill)
	{
		pSkill = m_pCurSkill;
	}
	else
	{
		pSkill = m_pDefSkill;
	}

	SAttackParam param;

	bool ChangeTarget = false;

	if (pSkill)
	{
		if (pSkill->dwRTarget & SKILL_TARGET_MASTER)
		{
			if (m_creature->IsUserPet())
			{
				if (m_creature->m_master)
				{
					m_creature->SetTarget(m_creature->m_master);
					param.EnemyId = m_creature->m_master->GetObjectId();
				}
			}
		}
		else if (pSkill->dwRTarget & SKILL_TARGET_PETSELF)
		{
			if (m_creature->IsUserPet())
			{
				m_creature->SetTarget(m_creature);
				param.EnemyId = m_creature->GetObjectId();
			}
			ChangeTarget = true;
		}
		if (pSkill->dwRTarget==0)
		{
			if(pSkill && m_creature->m_Skill.CanUseSkill(pSkill->wID))
			{
				skill = pSkill->wID;
				param.EnemyId = m_creature->GetCurrentTarget()->GetObjectId();
				param.TargetType = 0;
			}
		}
		else if (pSkill->dwRTarget&SKILL_TARGET_POSITION) 
		{
			CRegionCreature *target = m_creature->GetCurrentTarget();
			if (!target)
				target = m_creature;

			float vPos[3] = {0,0,0};
			vPos[0] = m_creature->m_pos[0];
			vPos[1] = m_creature->m_pos[1];
			vPos[2] = m_creature->m_pos[2];
	
			if(pSkill && m_creature->m_Skill.CanUseSkill(pSkill->wID,vPos))
			{
				skill = pSkill->wID;
				param.Pos[0] = target->m_pos[0];
				param.Pos[1] = target->m_pos[1];
				param.Pos[2] = target->m_pos[2];
				param.TargetType = 2;
			}
		}
		else 
		{
			CRegionCreature *target = m_creature->GetCurrentTarget();
			if (!target)
				return;
			if(pSkill && m_creature->m_Skill.CanUseSkill(pSkill->wID,&(m_creature->GetCurrentTarget()->m_Skill)))
			{
				skill = pSkill->wID;
				param.EnemyId = m_creature->GetCurrentTarget()->GetObjectId();
				param.TargetType = 1;
			}
		}
	}

	m_targetPos[0] = m_creature->m_pos[0];
	m_targetPos[1] = m_creature->m_pos[1];

	int now = rtGetMilliseconds();
	int attackTime = m_lastAttackTime + m_creature->m_core.GetAttackInterval();

	if (now >= attackTime)
	{
		if (m_creature->GetCurrentTarget() == m_creature->m_master)
		{
			return;
		}

		param.Seed = rand()%60000;
		param.Skill = skill;
		if (skill == 0)
		{
			param.EnemyId = m_creature->GetCurrentTarget()->GetObjectId();
			param.TargetType = 1;
		}

		if(skill == 0)
			m_creature->DoAttack(param.EnemyId,param.Seed);
		else if (m_creature->UseSkill(param))
		{
			if (m_pCurSkill)
			{
				m_pCurSkill = NULL;
			}
			if (skill == PET_SKILL_DROPRIDE)
			{
				if (m_creature->GetCurrentTarget() && 
					m_creature->GetCurrentTarget()->m_master && 
					m_creature->GetCurrentTarget()->m_master->IsUser() && 
					m_creature->GetCurrentTarget()->m_master->IsRidePet())
					m_creature->GetCurrentTarget()->m_master->DropUnRide(0.5f);
			}
			else if (skill == PET_SKILL_RIDE)
			{
				if (m_creature->m_master->IsRidePet())
					m_creature->m_master->RideOffPet();
				else
					m_creature->m_master->RidePet();
			}
			else if (skill == PET_SKILL_LINKUP)
			{
				if(!m_creature->m_master->IsLinkUpWithPet())
					m_creature->m_master->LinkUpWithPet(0);
				else
					m_creature->m_master->LinkUpWithPet(m_pItemPet.id);
			}
		}

		if (ChangeTarget)
		{
			if (m_creature->m_master)
			{
				m_creature->SetTarget(m_creature->m_master);
			}	
		}

		m_lastAttackTime = now;
	}
}

list<DWORD>	CRegionPetAI::GetPassiveList()
{
	list<DWORD> listPassive;
	listPassive.clear();

	if ( !ItemIsPet(m_pItemPet) )
		return listPassive;

	for (int i=PET_PARAM_PASSIVE_1;i<PET_PARAM_PASSIVE_6+1;i++)
	{
		if (m_pItemPet.params[i] != 0)
		{
			listPassive.push_back(m_pItemPet.params[i]);
		}
	}

	return listPassive;
}

bool CRegionPetAI::FindPetSkill(DWORD skillID)
{
	if ( !ItemIsPet(m_pItemPet) )
		return false;

	for (int i=PET_PARAM_SKILL_1;i<PET_PARAM_SKILL_6+1;i++)
	{
		if (m_pItemPet.params[i] == skillID)
		{
			return true;
		}
	}
	return false;
}

bool CRegionPetAI::PetLevUp()
{
	if ( !m_creature || !m_creature->m_master || !m_creature->m_master->m_pItemContainer)
		return false;

	if ( m_pItemPet.params[PET_PARAM_LEV] == m_creature->m_core.Lev )
		return false;

	CBag *pPetBag = &m_creature->m_master->m_pItemContainer->m_PetBag;
	if (!pPetBag) return false;

	int page,i,j;
	if (!pPetBag->Find(m_pItemPet,&page,&i,&j))
		return false;

	SItemID item = m_pItemPet;

	SItemPet *pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
	if(!pItemPet) return false;
	SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
	if(!pPet) return false;

	item.params[PET_PARAM_EXP]	= m_creature->m_core.Exp;
	item.params[PET_PARAM_LEV]  = m_creature->m_core.Lev;
	item.params[PET_PARAM_AI]	= 0;

	if (ItemIsPetFairy(m_pItemPet))
	{
		// 仙兽进化

		int lev = item.params[PET_PARAM_EVOLUTION_LEV];
		for(int i=0; i<MAX_PET_MODEL_CHANGE; i++)
		{
			if(item.params[PET_PARAM_LEV] <= pPet->ModelLevel[i])
			{
				item.params[PET_PARAM_EVOLUTION_LEV] = i;
				break;
			}
		}
		if (lev != item.params[PET_PARAM_EVOLUTION_LEV])
		{
			SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(item.params[PET_PARAM_LEV]));
			if(!base) return false;

			//item.params[PET_PARAM_GROWTH_HP] = pPet->FHp * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_MP] = pPet->FMp * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_ATT] = pPet->FDamage * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_DEF] = pPet->FArmor * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_HIT] = pPet->FAttack * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_DODGE] = pPet->FDodge * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;

			//item.params[PET_PARAM_GROWTH_WATERDA] = pPet->ElementAttackRate[ELEMENT_WATER] * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_FIREDA] = pPet->ElementAttackRate[ELEMENT_FIRE] * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_POSIONDA] = pPet->ElementAttackRate[ELEMENT_POISON] * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_WATERDEF] = pPet->ElementArmorRate[ELEMENT_WATER] * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_FIREDEF] = pPet->ElementArmorRate[ELEMENT_FIRE] * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;
			//item.params[PET_PARAM_GROWTH_POSIONDEF] = pPet->ElementArmorRate[ELEMENT_POISON] * (1.f + 0.15f * (float)item.params[PET_PARAM_EVOLUTION_LEV]) * PET_PARAM_COMPUTE_GROWTH;

			item.params[PET_PARAM_HP]	= base->FHp * item.params[PET_PARAM_GROWTH_HP] / PET_PARAM_COMPUTE_GROWTH + pPet->AddedHp;
			item.params[PET_PARAM_MP]   = base->FMp * item.params[PET_PARAM_GROWTH_MP] / PET_PARAM_COMPUTE_GROWTH + pPet->AddedMp;
		}
	}
	else if (ItemIsPetSpectre(m_pItemPet))
	{
		item.params[PET_PARAM_LINKUP_LEV] = GetMergeBeadsLev(item,pPet);
		m_creature->m_master->SendPetInfo();
	}

	if (!pPetBag->SetItem(page, i, j,item))
		return false;

	m_pItemPet = item;

	CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, page, i, j, m_pItemPet);
	m_creature->m_master->RecvCmd(g_sendCmd);

	return true;
}

bool CRegionPetAI::DoUseSkill(DWORD skillID,DWORD dwTarget/* = 0*/)
{
	if (!FindPetSkill(skillID))
		return false;

	SSkill* pSkill = Skill()->FindSkill(skillID);

	if (!pSkill)
		return false;

	if (m_state == STATE_COMBAT)
	{
		m_pCurSkill = pSkill;
		if (pSkill->dwRTarget & SKILL_TARGET_TYPE_ACTOR && dwTarget)
		{
			if (m_creature->m_scene)
			{
				CRegionCreature *pTarget = m_creature->m_scene->FindCreature(dwTarget);
				if (pTarget)
				{
					m_creature->SetTarget(pTarget);
				}
			}
		}
		//else if ((pSkill->dwRTarget & SKILL_TARGET_POSITION) || pSkill->bAType)
		//{
		//}
	}
	else
	{
		int seed = rand() % 100;
		int skill = 0;
		SAttackParam param;

		bool ChangeTarget = false; //是否强制改变了宠物的目标  如果改变了 记得设置回来

		//add by Tianh  //合体和骑宠物将目标设置为主人
		if (pSkill->dwRTarget & SKILL_TARGET_MASTER)
		{
			if (m_creature->IsUserPet())
			{
				if (m_creature->m_master)
				{
					m_creature->SetTarget(m_creature->m_master);
					param.EnemyId = m_creature->m_master->GetObjectId();
				}
			}
		}
		else if (pSkill->dwRTarget & SKILL_TARGET_PETSELF)
		{
			if (m_creature->IsUserPet())
			{
				m_creature->SetTarget(m_creature);
				param.EnemyId = m_creature->GetObjectId();
			}
			ChangeTarget = true;
		}
		else if (pSkill->dwRTarget & SKILL_TARGET_TYPE_ACTOR && dwTarget)
		{
			if (m_creature->m_scene)
			{
				CRegionCreature *pTarget = m_creature->m_scene->FindCreature(dwTarget);
				if (pTarget)
				{
					m_creature->SetTarget(pTarget);
				}
			}
		}
		

		if (pSkill->dwRTarget==0)
		{
			if(pSkill && m_creature->m_Skill.CanUseSkill(pSkill->wID))
				skill = pSkill->wID;
			else 
				return false;
			if (m_creature->GetCurrentTarget())
			{
				param.EnemyId = m_creature->GetCurrentTarget()->GetObjectId();
			}
			param.TargetType = 0;
		}
		else if (pSkill->dwRTarget&SKILL_TARGET_POSITION) 
		{
			CRegionCreature *target = m_creature->GetCurrentTarget();
			if (!target)
				target = m_creature;

			float vpos[3];
			vpos[0] = target->m_pos[0];
			vpos[1] = target->m_pos[1];
			vpos[2] = target->m_pos[2];
			if(pSkill && m_creature->m_Skill.CanUseSkill(pSkill->wID,vpos))
				skill = pSkill->wID;
			else 
				return false;
			param.Pos[0] = target->m_pos[0];
			param.Pos[1] = target->m_pos[1];
			param.Pos[2] = target->m_pos[2];
			param.TargetType = 2;
		}
		else 
		{
			CRegionCreature *target = m_creature->GetCurrentTarget();
			if (!target)
				return false;
			if(pSkill && m_creature->m_Skill.CanUseSkill(pSkill->wID,&(m_creature->GetCurrentTarget()->m_Skill)))
				skill = pSkill->wID;
			else 
				return false;
			param.EnemyId = m_creature->GetCurrentTarget()->GetObjectId();
			param.TargetType = 1;
		}


		m_targetPos[0] = m_creature->m_pos[0];
		m_targetPos[1] = m_creature->m_pos[1];

		int now = rtGetMilliseconds();
		int attackTime = m_lastAttackTime + m_creature->m_core.GetAttackInterval();

		if (now >= attackTime)
		{
			if (m_creature->GetCurrentTarget() == m_creature->m_master && pSkill->bABad && param.TargetType == 1)
			{
				return false;
			}

			param.Seed = rand()%60000;
			param.Skill = skill;


			if(skill == 0)
				return false;
			else
			{
				if (m_creature->UseSkill(param))
				{
					if (skill == PET_SKILL_DROPRIDE)
					{
						if (m_creature->GetCurrentTarget() && 
							m_creature->GetCurrentTarget()->m_master && 
							m_creature->GetCurrentTarget()->m_master->IsUser() && 
							m_creature->GetCurrentTarget()->m_master->IsRidePet())
							m_creature->GetCurrentTarget()->m_master->DropUnRide(0.5f);
					}
					else if (skill == PET_SKILL_RIDE)
					{
						if (m_creature->m_master->IsRidePet())
							m_creature->m_master->RideOffPet();
						else
							m_creature->m_master->RidePet();
					}
					else if (skill == PET_SKILL_LINKUP)
					{
						if(m_creature->m_master->IsLinkUpWithPet())
							m_creature->m_master->LinkUpWithPet(0);
						else
							m_creature->m_master->LinkUpWithPet(m_pItemPet.id);
					}
				}
			}
			m_lastAttackTime = now;
		}

		if (ChangeTarget)
		{
			if (m_creature->m_master)
			{
				m_creature->SetTarget(m_creature->m_master);
			}	
		}
	}

	return true;
}

void CRegionPetAI::UserCtrlPet(char command,DWORD dwParam1/* = 0*/,DWORD dwParam2/* = 0*/)
{
	switch (command)
	{
	case PET_CTRL_ATTACK:
		{
			m_eAS = AS_ATTACK;
			if (m_creature && m_creature->m_scene)
			{
				CRegionCreature *cre = m_creature->m_scene->FindCreature(dwParam1);
				if (cre) m_creature->SetTarget(cre);
			}
		}
		break;
	case PET_CTRL_SYNERGY:
		{
			m_eAS = AS_SYNERGY;
		}
		break;
	case PET_CTRL_PASSIVE:
		{
			m_eAS = AS_PASSIVE;
		}
		break;
	case PET_CTRL_DEF:
		{
			m_eAS = AS_DEF;
		}
		break;
	case PET_CTRL_USE_SKILL:
		{
			DoUseSkill(dwParam1,dwParam2);
		}
		break;
	case PET_CTRL_SET_DEFSKILL:
		{
			SetDefSkill(dwParam1);
		}
		break;
	default:
		break;
	}
}

PetType CRegionPetAI::GetPetType()
{
	if ( !ItemIsPet(m_pItemPet) )
		return PET_TYPE_NULL;

	if (ItemIsPetNormal(m_pItemPet))
		return PET_TYPE_NORMAL;
	else if (ItemIsPetFairy(m_pItemPet))
		return PET_TYPE_FAIRY;
	else if (ItemIsPetSpectre(m_pItemPet))
		return PET_TYPE_SPECTRE;

	return PET_TYPE_NULL;
}

list<DWORD>	CRegionPetAI::GetSkillList()
{
	list<DWORD> listSkill;
	listSkill.clear();

	if ( !ItemIsPet(m_pItemPet) )
		return listSkill;

	for (int i=PET_PARAM_SKILL_1;i<PET_PARAM_SKILL_6+1;i++)
	{
		if (m_pItemPet.params[i] != 0)
		{
			listSkill.push_back(m_pItemPet.params[i]);
		}
	}

	return listSkill;
}

bool CRegionPetAI::SetDefSkill(DWORD defSkill/* = 0*/)
{
	if (!ItemIsPet(m_pItemPet) )
		return false;

	if (m_pDefSkill && m_pDefSkill->wID == defSkill)
	{
		m_pDefSkill = NULL;
		defSkill = 0;
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_ctrl_pet);
		g_sendCmd->WriteByte(PET_CTRL_SET_DEFSKILL);
		g_sendCmd->WriteLong(defSkill);
		m_creature->m_master->RecvCmd(g_sendCmd);
	}
	else if (defSkill == PET_SKILL_DROPRIDE || defSkill == PET_SKILL_RIDE || defSkill == PET_SKILL_LINKUP )
	{
		return false;
	}
	else
	{
		for (int i=PET_PARAM_SKILL_1;i<PET_PARAM_SKILL_6+1;i++)
		{
			if (m_pItemPet.params[i] == defSkill || defSkill == 0)
			{
				m_pDefSkill = Skill()->FindSkill(defSkill);
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_ctrl_pet);
				g_sendCmd->WriteByte(PET_CTRL_SET_DEFSKILL);
				g_sendCmd->WriteLong(defSkill);
				m_creature->m_master->RecvCmd(g_sendCmd);
				return true;
			}
		}
	}
	return false;
}

CRegionCreature* CRegionPetAI::DoScanTarget()
{  
	if (m_creature->GetCurrentTarget() && m_creature->m_master != m_creature->GetCurrentTarget())
		return m_creature->GetCurrentTarget();

	if (m_creature->m_viewRange == 0)
		return NULL;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos, 
		m_creature->m_viewRange, creList, m_creature);

	if (creList.empty())
		return NULL;

	int viewRange = m_creature->m_viewRange * m_creature->m_viewRange;
	int minRange = viewRange + 1;

	std::vector<CRegionCreature*>::iterator nearest = creList.end();
	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter) == NULL)
			continue;

		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		if (g_region->IsCheatId((*iter)->m_oId))
			continue;

		// 不是怪物,则忽略
		if ( !(*iter)->IsMonster())
			continue;

		if ((*iter)->m_npcInfo && (*iter)->m_npcInfo->Type != 1)
		{
			continue;
		}

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_traceRange))
			continue;

		if ( !(*iter)->State_IsVisible() || !(*iter)->GetPlayerLogic()->IsLive())
			continue;

		int x = (*iter)->m_pos[0] - m_creature->m_pos[0];
		int y = (*iter)->m_pos[1] - m_creature->m_pos[1];

		int range = x * x + y  * y;

		if (range < minRange && range <= viewRange)
		{
			minRange = range;
			nearest = iter;
		}
	}

	if (nearest == creList.end())
		return NULL;

	if (*nearest)
	{
		m_creature->SetTarget(*nearest);
	}
	return *nearest;
}

bool CRegionPetAI::OnAttack(CRegionCreature* target)
{
	if (!CRegionAI::OnAttack(target))
		return false;

	m_creature->m_lockTarget = false;

	if (m_state != STATE_COMBAT)
	{
		SetState(STATE_COMBAT);
		m_creature->SetTarget(target);
	}
	else
	{
        m_creature->AddTarget(target, 1, 1);
	}

	return true;
}
bool CRegionAI::UpdateMovePath(int x, int y)
{
	int tx, ty;
	g_region->m_defScene->m_pTerrain->GetTerrainTile(x, y, tx, ty);

	if (!m_creature->m_scene || !m_creature->m_pathNode) return false;

 	bool result = m_creature->m_scene->m_pPathFind->FindPathEx(m_creature->m_pathNode, 
 		tx, ty, true, true);
   	if (result)
   		result = m_creature->m_scene->m_pPathFind->SmoothPath(m_creature->m_pathNode, false);

#ifdef _DEBUG
	if (_need_debug(m_creature))
		RtCoreLog().Debug("#%d Update Move path success:%d \n",
				m_creature->m_oId,
				result?1:0);
	// Print all path position if success.
	if (result && _need_debug(m_creature))
	{
		std::stringstream ss;
		ss << "Monster path node:\n"
		   << "from " << m_creature->m_pos[0] << ':' << m_creature->m_pos[1]
		   << " to "  << tx << ':' << ty << '\n';
		int x = 0, y = 0;
		while (m_creature->m_pathNode->Next(x, y))
			ss << "x: " << x << "\ty: " << y << '\n';
		RtCoreLog().Debug("%s\n", ss.str().c_str());
		m_creature->m_scene->m_pPathFind->FindPathEx(m_creature->m_pathNode, 
				tx, ty, true, true);
		//m_creature->m_scene->m_pPathFind->SmoothPath(m_creature->m_pathNode, false);
	}
#endif

    if (!result)
    {
        return false;
    }

    if (m_creature->m_pathNode==NULL)
    {
        return false;
    }

//-------------------------chg start by tony 05.05.10------------------------//
 	if (m_creature->m_pathNode->PathCount() >= 3)
 //-------------------------chg end   by tony 05.05.10------------------------//
 	{
 		int u, v;
 		float fx, fy;
 		m_creature->m_pathNode->Next(u, v);		
 
 		float fx1stNode, fy1stNode;
 		g_region->m_defScene->m_pTerrain->GetPosByTerrainXY(u, v, fx1stNode, fy1stNode);
 		// 如果下一个点是往回走，就取下一个点
  		int tmpX1 = x - (int)fx1stNode;
  		int tmpX2 = (int)fx1stNode - m_creature->m_pos[0];
  		int tmpY1 = y - (int)fy1stNode;
  		int tmpY2 = (int)fy1stNode - m_creature->m_pos[1];
  		if ((tmpX1*tmpX2 < 0) && (tmpY1*tmpY2 < 0))
  		{
  			m_creature->m_pathNode->Next(u, v);
  		}
 
 		OnCallPathNextSuccessed(u, v, fx, fy);		
 	}

	return result;
}

bool CRegionAI::CanAttack(CRegionCreature* creature)
{
	//if (IsOutOfRange(m_creature->m_pos, creature->m_pos, m_creature->m_attackRange))
	//{
	//	// 如果在相邻的格子就返回true（因为不能再往前走了）
	//	int tile1_y, tile1_x, tile2_x, tile2_y;
	//	g_region->m_defScene->m_pTerrain->GetTerrainTile(m_creature->m_pos[0], m_creature->m_pos[1], tile1_x, tile1_y);
	//	g_region->m_defScene->m_pTerrain->GetTerrainTile(creature->m_pos[0], creature->m_pos[1], tile2_x, tile2_y);
	//	if (abs(tile1_x-tile2_x)<=1 && abs(tile1_y-tile2_y)<=1)
	//		return true;
	//	else 
	//           return false;
	//}

	//return true;

	//LeiJun 如果NPC所属用户是攻击目标则返回假
	if(m_creature->m_master == creature)
		return false;

	return TestAttackRange(m_creature->m_attackRange, m_creature->m_pos, m_creature->m_model, creature->m_pos, creature->m_model);
}

bool CRegionAI::IsOutOfRange(const long* p1, const long* p2, int range)
{
	int x = p2[0] - p1[0];
	int y = p2[1] - p1[1];

	int d = x * x + y * y;

	return d > range * range;
}

void CRegionMonsterAI::KeepDistance(CRegionCreature* target)
{
	float dx = m_creature->m_pos[0] - target->m_pos[0];
	float dy = m_creature->m_pos[1] - target->m_pos[1];
	float dist = dx * dx + dy * dy;

	int tx, ty;

	if (dist > 1e-6f)
	{
		dist = sqrtf(dist);

		dx /= dist; dy /= dist;

		tx = target->m_pos[0] + dx * m_creature->m_attackRange;
		ty = target->m_pos[1] + dy * m_creature->m_attackRange;
	}
	else
	{
		tx = m_creature->m_pos[0];
		ty = m_creature->m_pos[1];
	}		

	m_creature->SetPosition(tx, ty, 0);
}


bool CRegionMonsterAI::OnCombat(CRegionCreature* target)
{
#ifdef _DEBUG
	if (_need_debug(m_creature))
		RtCoreLog().Debug("#%d CRegionMonsterAI::OnCombat::CombatRule%d \n",
				m_creature->m_oId,
				m_creature->m_combatRule);
#endif
	switch(m_creature->m_combatRule)
	{
	case CRegionCreature::COMBAT_RULE_GUARD:
		if (CanAttack(target))
		{
			DoAttack();
		}
		break;

	case CRegionCreature::COMBAT_RULE_AGGRESSIVE:
		if (CanAttack(target))
		{
			DoAttack();
		}
		else
		{
#ifdef _DEBUG
	if (_need_debug(m_creature))
		RtCoreLog().Debug("#%d CRegionMonsterAI::OnCombat::Decide do trace now.\n",
				m_creature->m_oId);
#endif
			DoTrace(target);				

			//if (CanAttack(target))
			//	DoAttack();
		}
		break;

	case CRegionCreature::COMBAT_RULE_DISCREET:	
		if (CanAttack(target))
		{
			KeepDistance(target);
			//	DoAttack();
		}
		else
		{
			DoTrace(target);

			if (CanAttack(target))
				DoAttack();
		}
		break;
	}

	return true;
}


CRegionCreature* CRegionAI::GetLeader()
{
	if (m_creature->m_spawn == NULL || 
		m_creature->m_spawn->m_leader == NULL)
		return NULL;

	if (m_creature->m_spawn->m_leader->m_npc.front().npc)
		return m_creature->m_spawn->m_leader->m_npc.front().npc;
	else
		return NULL;
}

bool CRegionAI::OnUnderlingBeAttacked(CRegionCreature* underling, CRegionCreature* target, int hate)
{
	return true;
}

bool CRegionBossAI::OnUnderlingBeAttacked(CRegionCreature* underling, CRegionCreature* target, int hate)
{
	return CRegionMonsterAI::OnBeAttacked(target, hate);

	/*
	if (!CRegionAI::OnUnderlingBeAttacked(underling, target, hate))	
	return false;    

	stlforeach(TargetList, iter, m_targets)
	{
	if (iter->targetId == target->GetObjectId())
	{
	iter->hate += hate;
	break;
	}
	}

	if (iter == m_targets.end())
	{
	m_targets.push_back(Target());
	m_targets.back().targetId = target->GetObjectId();
	m_targets.back().hate = hate;
	}
	else if (iter->hate > m_targets.front().hate)
	{	
	m_targets.push_front(Target());
	m_targets.front().targetId = iter->targetId;
	m_targets.front().hate = iter->hate;
	m_targets.erase(iter);
	}

	if (m_creature->m_angryPointLevel2 && m_targets.front().hate >= m_creature->m_angryPointLevel2)
	{
	LOG2("AI(oId=%d): Boss 进入2级愤怒状态(最大仇恨值=%d)，开始命令所有的属下攻击目标\n", 
	m_creature->m_oId,
	m_targets.front().hate);

	SetState(STATE_COMBAT);

	CRegionCreature* t = NULL;

	for (int n = 0; n < (int)m_creature->m_spawn->m_underling->m_npc.size(); n++)
	{
	if (m_creature->m_spawn->m_underling->m_npc[n].npc && 
	m_creature->m_spawn->m_underling->m_npc[n].npc->m_ai)
	{
	CRegionCreature* cre = m_creature->m_spawn->m_underling->m_npc[n].npc->m_ai->GetCurrentTarget();

	if (!cre || cre->GetObjectId() != m_targets.front().targetId)
	{
	if (t == NULL)
	t = m_creature->m_scene->FindCreature(m_targets.front().targetId);

	m_creature->m_spawn->m_underling->m_npc[n].npc->m_ai->OnAttack(t);
	}
	}
	}

	int maxHate = m_targets.front().hate;

	stlforeach(TargetList, iter, m_targets)
	{
	iter->hate = 0;
	}

	return false;
	}
	else if (m_creature->m_angryPointLevel1 && m_targets.front().hate >= m_creature->m_angryPointLevel1)
	{
	LOG2("AI(oId=%d): Boss 进入1级愤怒状态(最大仇恨值=%d)，开始命令周围的属下攻击目标\n",
	m_creature->m_oId, 
	m_targets.front().hate);

	CRegionCreature* t = NULL;

	for (int n = 0; n < (int)m_creature->m_spawn->m_underling->m_npc.size(); n++)
	{
	if (m_creature->m_spawn->m_underling->m_npc[n].npc && 
	m_creature->m_spawn->m_underling->m_npc[n].npc->m_ai)
	{
	CRegionCreature* cre = m_creature->m_spawn->m_underling->m_npc[n].npc->m_ai->GetCurrentTarget();

	if (!cre || cre->GetObjectId() != m_targets.front().targetId)
	{
	if (IsOutOfRange(m_creature->m_spawn->m_underling->m_npc[n].npc->m_pos, target->m_pos, 200) &&
	IsOutOfRange(m_creature->m_spawn->m_underling->m_npc[n].npc->m_pos, m_creature->m_pos, 200))
	continue;

	if (t == NULL)
	t = m_creature->m_scene->FindCreature(m_targets.front().targetId);

	m_creature->m_spawn->m_underling->m_npc[n].npc->m_ai->OnAttack(t);
	}
	}
	}		

	stlforeach(TargetList, iter, m_targets)
	{
	iter->hate *= 2;
	}

	return false;
	}
	*/

	return true;
}

bool CRegionBossAI::OnDead()
{
	/*
	CRegionSpawn* underling = GetUnderling();

	if (underling)
	{
	for (int n = 0; n < (int)underling->m_npc.size(); n++)
	{
	if (underling->m_npc[n].npc &&
	underling->m_npc[n].npc->m_ai)
	{
	underling->m_npc[n].npc->m_ai->OnLostLeader(m_creature);
	}
	}
	}
	*/

	if (!CRegionMonsterAI::OnDead())
		return false;	

	return true;
}

CRegionSpawn* CRegionAI::GetUnderling()
{
	if (!m_creature->m_spawn)
		return NULL;

	return m_creature->m_spawn->GetUnderling();
}

bool CRegionAI::OnLostLeader(CRegionCreature* leader)
{
	return true;
}

bool CRegionAI::OnUnderlingBorn(CRegionCreature* underling)
{
	return true;
}

bool CRegionAI::OnUnderlingDead(CRegionCreature* underling)
{
	return true;
}

void CRegionMonsterAI::BeginRunaway()
{
	ComputeRandomFaceTo(1);

	SetState(STATE_RUNAWAY);

	DoRunaway();
}

void CRegionMonsterAI::DoRunaway()
{	
	int moveAmount = m_creature->m_moveSpeed + m_moveFur;
	int moveLength = (int)(g_fSceneTerrainTileWidth * m_moveLengthPerStep);
	int moveStep = moveAmount / moveLength;

	m_moveFur = moveAmount % moveLength;

	float x = m_creature->m_pos[0] / g_fSceneTerrainTileWidth;
	float y = m_creature->m_pos[1] / g_fSceneTerrainTileWidth; 

	for (int n = 0; n < moveStep; n++)
	{
		x += m_face[0]; 
		y += m_face[1];

		if (!m_creature->m_scene->m_pPathFind->CanAdd(m_creature->GetObjectId(), m_creature->m_type, (int)x, (int)y))
			break;		
	}

	m_creature->m_pos[0] = (int)(x * g_fSceneTerrainTileWidth);
	m_creature->m_pos[1] = (int)(y * g_fSceneTerrainTileWidth);

	//if (!m_targets.empty())
	//SeekHelp();
}

void CRegionMonsterAI::ComputeRandomFaceTo(int fix)
{
	int seek = rand() % (8 - fix) + (fix);

	m_face[0] = seek & 1;
	if ((seek >> 1) & 1)
		m_face[0] = -m_face[0];

	m_face[1] = (seek >> 2) & 1;
	if ((seek > 3) & 1)
		m_face[1] = -m_face[1];

	if (m_face[0] && m_face[1])
		m_moveLengthPerStep = 1;//sqrtf(2);
	else
		m_moveLengthPerStep = 1;
}

bool CRegionMonsterAI::OnLostLeader(CRegionCreature* leader)
{
	return false;

#if 0
	if (!CRegionAI::OnLostLeader(leader))
		return false;

	char rate = (char)((char)((m_creature->m_bravePoint * 0.5f) + (m_creature->m_loyalPoint * 0.5f)));
	char det = rand() % 100;

	char revengeRate = 80;
	char runawayRate = 25;

#ifdef _DEBUG
	LOG6("AI(oId=%d): 勇敢测试 [%s] (决定值=%d, 概率=%d, 胆量=%d,忠诚=%d)\n",
		m_creature->m_oId,
		det < rate ? "成功" : "失败",
		det,
		rate,
		m_creature->m_bravePoint,
		m_creature->m_loyalPoint);
#endif

	if (det < rate)
	{
		det = rand() % 100;

		LOG4("AI(oId=%d): 复仇测试 [%s] (决定值=%d, 复仇概率=%d)\n",
			m_creature->m_oId, 
			det < revengeRate ? "成功" : "失败",
			det,
			revengeRate);

		if (det < revengeRate)
		{
			CRegionCreature* cre = leader->m_ai->GetCurrentTarget();

			OnAttack(cre);
		}		
	}
	else
	{
		det = rand() % 100;

		LOG4("AI(oId=%d): 逃跑测试 [%s] (决定值=%d, 逃跑概率=%d)\n",
			m_creature->m_oId, 
			det < revengeRate ? "成功" : "失败",
			det,
			runawayRate);

		if (det < runawayRate)
		{
			if (m_targets.empty() && !leader->m_ai->m_targets.empty())
			{			
				m_targets.push_back(Target());
				m_targets.back().targetId = leader->m_ai->m_targets.front().targetId;
				m_targets.back().hate = 0;
			}

			BeginRunaway();
		}
	}

	return true;
#endif
}

bool CRegionMonsterAI::OnUnderlingBorn(CRegionCreature* underling)
{
	if(!CRegionAI::OnUnderlingBorn(underling))
	{
		return false;
	}

	if(!underling)
	{
		return false;
	}

	if(m_pConjureAI)
	{
		m_pConjureAI->AddCreatureID(underling->m_oId);
	}

	return true;
}

bool CRegionMonsterAI::OnUnderlingDead(CRegionCreature* underling)
{
	if(!CRegionAI::OnUnderlingDead(underling))
	{
		return false;
	}

	if(!underling)
	{
		return false;
	}
	
	if(m_pConjureAI)
	{
		m_pConjureAI->DelCreatureID(underling->m_oId);
	}

	return true;
}


bool CRegionBossAI::OnBeAttacked(CRegionCreature* target, int hate)
{

		CRegionMonsterAI::OnBeAttacked(target, hate);
		return true;
}

bool CRegionAI::OnLostTarget(CRegionCreature* target)
{	
	m_creature->RemoveTargetByCreature(target);
	return true;
}

//-----------------------add start by tony 05.05.08--------------------------//
CRegionGuarderAI::CRegionGuarderAI()
: CRegionAI(TYPE_GUARDER)
{
	m_state	  = STATE_MOVE;
	m_Arrived = false;
	m_Left	  = false;
	m_TargetNode[0] = 0;
	m_TargetNode[1] = 0;
	m_TargetNode[2] = 0;
	m_bHaveIdlePos = false;
}

bool CRegionGuarderAI::OnAssist(CRegionCreature* target)
{
	if (!CRegionAI::OnAssist(target))
		return false;

	if ((m_state == STATE_COMBAT) &&
		 (m_state == STATE_RETURN))
		return false;

	m_creature->SetTarget(target);

	//测试该目标是否有效
	CRegionCreature* p_cre = m_creature->GetCurrentTarget();
	
	if(p_cre != NULL)
	{
		SetState(STATE_COMBAT);

		//保存当前位置
		SaveCurrPos();

#ifdef _DEBUG
		LOG1("the guarder [%s] received help info\n", m_creature->m_npcInfo->Name.c_str());
		LOG2("It's current position is [%d][%d]\n", m_creature->m_pos[0], m_creature->m_pos[1]);
		LOG2("Saved position is [%d][%d]\n\n ", m_TargetNode[0], m_TargetNode[1]);
#endif //debug

		return true;
	}
		
	m_creature->RemoveTargetByCreature(target);
	
	return false;
}

bool CRegionGuarderAI::OnAttack(CRegionCreature* target)
{
	return true;
}

bool CRegionGuarderAI::OnBeAttacked(CRegionCreature* target, int hate)
{
	if (target == m_creature || target == m_creature->m_master)
		return false;

	if (m_state==STATE_RETURN || m_state==STATE_RUNAWAY)
		return false;

	if (!CRegionAI::OnBeAttacked(target, hate))
		return false;	

	if (m_state != STATE_COMBAT)
		SetState(STATE_COMBAT);

	//m_creature->AddTarget(target, hate, hate);
	m_creature->SetTarget(target);

	//SeekHelp(target);
	m_isBeAttacked = true; 

	return true;
}

bool CRegionGuarderAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	//初始化
	m_StartNode[0] = creature->m_pos[0];
	m_StartNode[1] = creature->m_pos[1];
	m_StartNode[2] = creature->m_pos[2];
	m_StartDir	   = creature->m_dir;
	m_Arrived	   = false;
	m_Left		   = false;

	//巡逻目标点初始化
	m_PatrolPath = creature->m_npcInfo->PatrolPath;

	if(m_PatrolPath.empty())
	{
        m_dwLevelFlag &= (~AI_PATROL);
        m_dwLevelFlag |= AI_IDLE;

		m_TargetNode[0] = creature->m_pos[0];
		m_TargetNode[1] = creature->m_pos[1];
		m_TargetNode[2] = 0;
	}else
	{		
		m_IterNode		= m_PatrolPath.begin();
		m_TargetNode[0] = (*m_IterNode).x;
		m_TargetNode[1] = (*m_IterNode).y;
		m_TargetNode[2] = 0;
	}

	m_state = STATE_MOVE;

	return true;
}

bool CRegionGuarderAI::OnCombat(CRegionCreature* target)
{
	if (CanAttack(target))
	{
		DoAttack();
	}
	else
	{
		DoTrace(target);				
	}
	
	return true;
}

bool CRegionGuarderAI::OnLostLeader(CRegionCreature* leader)
{
	return true;
}

bool CRegionGuarderAI::OnThink()
{
	
	if (!CRegionAI::OnThink())
		return false;

	if (!m_creature->CanMove())
		return false;

	switch(m_state) 
	{
	//运动状态
	case STATE_MOVE:
		if(!DoScanTarget())
		{
			DoMove();
		}
		else
		{
			//状态转换
			SetState(STATE_COMBAT);

			if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
			{
				//保存当前位置
				SaveCurrPos();
			}			
		}
		break;

	//返回状态
	case STATE_RETURN:
		DoReturn();
		break;

	//战斗状态
	case STATE_COMBAT:
		{
			//获得当前攻击目标
			CRegionCreature* target = m_creature->GetCurrentTarget();


			//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
			if ((target != NULL) && (target->m_IsHiding != 0))
			{
				target->LeaveCombat();
				break;
			}


			if(target != NULL)
			{
				if(CanAttack(target))
				{
					DoAttack();
				}
				else
				{		
					//越界？返回		
					if (m_creature->IsOutOfRange(m_TargetNode, m_creature->m_traceRange))
					{
						if(m_creature->m_traceRange == 0)
						{
							SetState(STATE_MOVE);
						}else
						{
							BeginReturn();
						}
					}		
					else
					{
						if(target != NULL && target->GetPlayerLogic()->IsLive())
						{
							OnCombat(target);
						}
						else
						{
							BeginReturn();
						}
					}
				}
			}else
			{
				BeginReturn();
			}
		}
		break;

	//其他情况
	default:
		SetState(STATE_MOVE);
		break;
	}

	return true;
}

void CRegionGuarderAI::DoMove()
{
	switch(MOVETYPE(m_dwLevelFlag))
	{
	case AI_FIXED:
		DoIdle();
		break;
	case AI_PATROL:
		DoPatrol();
		break;
	case AI_IDLE:
		DoIdle();
		break;
	default:
		DoIdle();
		break;
	}
}

void CRegionGuarderAI::DoIdle()
{
	CHECK(m_state == STATE_MOVE);

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		return;
	}
	
	if (m_creature->m_patrolRange<=0)
		return;

	if (m_bHaveIdlePos)
	{
		if (!ApplyMove())
			m_bHaveIdlePos = false;
	}
	else 
	{
		int nNewPlace = rand() % 4;
		if (nNewPlace!=0)
			return;

		// Find a random place to go

		int range = m_creature->m_patrolRange;

		long ranX = rand() % (range*2);
		long ranY = rand() % (range*2);
		ranX -= range;
		ranY -= range;

		m_idleToPos[0] = m_StartNode[0] + ranX;
		m_idleToPos[1] = m_StartNode[1] + ranY;

		m_bHaveIdlePos = true;

		if (!UpdateMovePath(m_idleToPos[0], m_idleToPos[1]))
		{
		}
		else
		{
			//LOG2("Create a new pos[x=%d,y=%d] to go for guarder\n", m_idleToPos[0], m_idleToPos[1]);
		}
	}
}

void CRegionGuarderAI::DoPatrol()
{
	CHECK(m_state == STATE_MOVE);

	// A{{{ [12/9/2008 Louis.Huang]
	// Not enter the region?
	if (m_creature->m_pathNode == NULL)
		return;
	// }}} [12/9/2008 Louis.Huang]

	if ((m_creature->m_patrolRange<=0) ||
		 m_PatrolPath.empty())
	{
		return;
	}

	//m_Arrived：是否到达目标节点，此点不仅仅是巡逻路径上的节点
	if ( !m_Arrived && !ApplyMove())
	{
		//
	}

	// RtCoreLog().Info("guard ai: %d, %d, %d, %d\n", m_creature, m_TargetNode[0], m_TargetNode[1], m_TargetNode[2]);
	if (m_creature->Distance(m_TargetNode) < 30)
	{
		m_Arrived = true;
		m_Left	  = false;
	}
	else
	{
		m_Arrived = false;
	}

	//已到达目标节点
	if(m_Arrived)
	{		
		//判断是否到达巡逻终点		
		if((++ m_IterNode) == m_PatrolPath.end())
		{
			//若到终点，返回
			m_PatrolPath.reverse();
			m_IterNode = m_PatrolPath.begin();
			m_IterNode ++;
		}
	}

	//重新设定巡逻目标
	m_TargetNode[0] = (*m_IterNode).x;
	m_TargetNode[1] = (*m_IterNode).y;

	if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	{
		//do nothing
	}		
}

bool CRegionGuarderAI::DoScanTarget()
{
	//视野为0
	if (m_creature->m_viewRange == 0)
	{
		return false;
	}

	//取得周围的生物
	std::vector<CRegionCreature*> creList;
	m_creature->m_scene->GetAroundCreatureByGrid( m_creature->m_pos, 
											m_creature->m_viewRange,
											creList,
											m_creature );

	if (creList.empty())
	{
		return false;
	}

	//最大视野范围
	int viewRange = m_creature->m_viewRange * m_creature->m_viewRange;

	//最小视野
	int minRange = viewRange + 1;

	std::vector<CRegionCreature*>::iterator nearest = creList.end();
	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	//遍历取得离自己最近的目标
	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		//守卫不攻击怪物
		if (!(*iter)->IsUser() && !(*iter)->IsUserPet() && !(*iter)->IsCallNpc())
			continue;

		// 不攻击隐身生物
		if ( !(*iter)->State_IsVisible() )
			continue;

		//中立守卫只攻击红名玩家
		if(m_creature->m_core.Faction == FACTION_UNKNOWN)
		{
			if ((*iter)->GetPKState()!= PK_STATE_RED_NAME)
				continue;
		}
		else
		{
			if( (m_creature->m_npcInfo != NULL) &&
				(g_RegionCastleWarMgr.IsCastleWarTime(m_creature->m_npcInfo->Home)))
			{
				//
			}	
			else 
			{
				//守卫攻击红名以及对立阵营玩家
				if((*iter)->IsUserPet() || (*iter)->IsCallNpc())
				{
					if(((*iter)->m_master->GetPKState() != PK_STATE_RED_NAME) &&
						((*iter)->m_master->m_core.Faction == m_creature->m_core.Faction))
					{
						continue;
					}
				}else
				{
					if (((*iter)->GetPKState()!= PK_STATE_RED_NAME) &&
						((*iter)->m_core.Faction == m_creature->m_core.Faction))
						continue;
				}
			}
		}

		int x = (*iter)->m_pos[0] - m_creature->m_pos[0];
		int y = (*iter)->m_pos[1] - m_creature->m_pos[1];

		//守卫与目标距离
		int range = x * x + y  * y;

		//优先攻击对立阵营玩家，其次红名
		if((m_creature->m_core.Faction != FACTION_UNKNOWN) &&
		   ((*iter)->m_core.Faction != m_creature->m_core.Faction))
		{
			range /= 2;
		}

		if (range < minRange && range <= viewRange)
		{
			minRange = range;
			nearest = iter;
		}
	}

	//搜寻到目标
	if (nearest != creList.end())
	{
		m_creature->SetTarget(*nearest);
		return true;
	}	

	return false;
}

bool CRegionGuarderAI::BeginReturn()
{
	SetState(STATE_RETURN);

	if(m_creature->IsScriptLoaded())
	{
		m_creature->CallScript("LeaveBattle", false);
	}

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		memcpy((void*)m_TargetNode, (void*)m_StartNode, sizeof(m_StartNode));
	}
	
	m_creature->RemoveAllTargets();

	if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	{
		//SetState(STATE_MOVE);
		m_creature->m_dir = m_StartDir;
	}
	return true;
}

void CRegionGuarderAI::DoReturn()
{
	if (m_creature->Distance(m_TargetNode) < 30)
	{
		SetState(STATE_MOVE);

		m_Arrived = true;
		m_Left	  = false;
		
		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
		m_creature->m_dir = m_StartDir;
		
		return;
	}

	if (m_creature->CanMove() && !ApplyMove())
	{
		if (UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
		{
			if (!ApplyMove())
			{
				SetState(STATE_MOVE);
			}
		}
		else 
		{
			SetState(STATE_MOVE);
		}
	}
}

void CRegionGuarderAI::SeekHelp(CRegionCreature* target)
{	
	if(target == NULL)
		return;
	if (m_creature->m_assistRange == 0)
		return;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos,
										   m_creature->m_assistRange,
										   creList,
										   m_creature);

	if (creList.empty())
		return;

	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_core.Faction != m_creature->m_core.Faction)
			continue;

		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_assistRange))
			continue;

		//找到目标
		if ((*iter)->m_ai)
		{
			if (((*iter)->m_ai->GetState() != STATE_COMBAT) &&
				((*iter)->m_ai->GetState() != STATE_RETURN))
			{
				(*iter)->m_ai->OnAssist(target);

#ifdef _DEBUG
				LOG2("[%s]ask help for[%s]\n", m_creature->m_npcInfo->Name.c_str(),
											 (*iter)->m_ai->m_creature->m_npcInfo->Name.c_str());
#endif //debug
			}
		}
	}
}
//-----------------------add end by tony 05.05.08----------------------------//


CRegionAssistantAI::CRegionAssistantAI()
: CRegionAI(TYPE_GUARDER)
{
	m_state	  = STATE_MOVE;
	m_Arrived = false;
	m_Left	  = false;
	m_bHaveIdlePos = false;
}

CRegionAssistantAI::CRegionAssistantAI(Type type)
: CRegionAI(type)
{
	m_state	  = STATE_MOVE;
	m_Arrived = false;
	m_Left	  = false;
	m_bHaveIdlePos = false;
}

bool CRegionAssistantAI::OnAssist(CRegionCreature* target)
{
	if (!CRegionAI::OnAssist(target))
		return false;

	if ((m_state == STATE_COMBAT) &&
		(m_state == STATE_RETURN))
		return false;

	m_creature->SetTarget(target);

	//测试该目标是否有效
	CRegionCreature* p_cre = m_creature->GetCurrentTarget();

	if(p_cre != NULL)
	{
		SetState(STATE_COMBAT);

		//保存当前位置
		SaveCurrPos();

#ifdef _DEBUG
		LOG1("the guarder [%s] received help info\n", m_creature->m_npcInfo->Name.c_str());
		LOG2("It's current position is [%d][%d]\n", m_creature->m_pos[0], m_creature->m_pos[1]);
		LOG2("Saved position is [%d][%d]\n\n ", m_TargetNode[0], m_TargetNode[1]);
#endif //debug

		return true;
	}

	m_creature->RemoveTargetByCreature(target);

	return false;
}

bool CRegionAssistantAI::OnAttack(CRegionCreature* target)
{
	return true;
}

bool CRegionAssistantAI::OnBeAttacked(CRegionCreature* target, int hate)
{
	if (target == m_creature || target == m_creature->m_master)
		return false;

	if (m_state==STATE_RETURN || m_state==STATE_RUNAWAY)
		return false;

	if (!CRegionAI::OnBeAttacked(target, hate))
		return false;	

	if (m_state != STATE_COMBAT)
		SetState(STATE_COMBAT);

	m_creature->AddTarget(target, hate, hate);
	//m_creature->SetTarget(target);

	//SeekHelp(target);
	m_isBeAttacked = true; 

	return true;
}

bool CRegionAssistantAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	//初始化
	m_StartNode[0] = creature->m_pos[0];
	m_StartNode[1] = creature->m_pos[1];
	m_StartNode[2] = creature->m_pos[2];
	m_StartDir	   = creature->m_dir;
	m_Arrived	   = false;
	m_Left		   = false;

	//巡逻目标点初始化
	m_PatrolPath = creature->m_npcInfo->PatrolPath;

	if(m_PatrolPath.empty())
	{
        m_dwLevelFlag &= (~AI_PATROL);
        m_dwLevelFlag |= AI_IDLE;

        m_TargetNode[0] = creature->m_pos[0];
		m_TargetNode[1] = creature->m_pos[1];
	}else
	{		
		m_IterNode		= m_PatrolPath.begin();
		m_TargetNode[0] = (*m_IterNode).x;
		m_TargetNode[1] = (*m_IterNode).y;		
	}

	m_state = STATE_MOVE;

	m_tBorn = (DWORD)time(NULL);

	return true;
}

bool CRegionAssistantAI::OnCombat(CRegionCreature* target)
{
	if (CanAttack(target))
	{
		DoAttack();
	}
	else
	{
		DoTrace(target);				
	}

	return true;
}

bool CRegionAssistantAI::OnThink()
{

	if (!CRegionAI::OnThink())
		return false;

	if (!m_creature->CanMove())
		return false;

	switch(m_state) 
	{
		//运动状态
	case STATE_MOVE:
		if(!DoScanTarget())
		{
			DoMove();
		}
		else
		{
			//状态转换
			SetState(STATE_COMBAT);

			if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
			{
				//保存当前位置
				SaveCurrPos();
				LOG3("[%s] Find Target: curr pos [%d][%d]\n", m_creature->m_npcInfo->Name.c_str(), m_creature->m_pos[0], m_creature->m_pos[1]);
				LOG3("[%s] Return Node: [%d][%d]\n", m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);
			}			
		}
		break;

		//返回状态
	case STATE_RETURN:
		DoReturn();
		break;

		//战斗状态
	case STATE_COMBAT:
		{
			//获得当前攻击目标
			CRegionCreature* target = m_creature->GetCurrentTarget();

			//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
			if ((target != NULL) && (target->m_IsHiding != 0))
			{
				target->LeaveCombat();
				break;
			}

			if(target != NULL)
			{
				if(CanAttack(target))
				{
					DoAttack();
				}
				else
				{		
					//越界？返回		
					if (m_creature->IsOutOfRange(m_TargetNode, m_creature->m_traceRange))
					{
						if(m_creature->m_traceRange == 0)
						{
							SetState(STATE_MOVE);
						}else
						{
							BeginReturn();
						}
					}		
					else
					{
						if(target != NULL && target->GetPlayerLogic()->IsLive())
						{
							OnCombat(target);
						}
						else
						{
							BeginReturn();
						}
					}
				}
			}else
			{
				BeginReturn();
			}
		}
		break;

		//其他情况
	default:
		SetState(STATE_MOVE);
		break;
	}

	return true;
}

void CRegionAssistantAI::DoMove()
{
	switch(MOVETYPE(m_dwLevelFlag))
	{
	case AI_FIXED:
		DoIdle();
		break;
	case AI_PATROL:
		DoPatrol();
		break;
	case AI_IDLE:
		DoIdle();
		break;
	default:
		DoIdle();
		break;
	}
}

void CRegionAssistantAI::DoIdle()
{
	CHECK(m_state == STATE_MOVE);

	//if(m_creature->m_npcInfo->Id == 1254)
	//	LOG1("[%s] Into StateMove\n", m_creature->m_npcInfo->Name.c_str());

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		return;
	}

	if (m_creature->m_patrolRange<=0)
		return;

	if (m_bHaveIdlePos)
	{
		//LOG1("[%s] Finded Idle Place\n", m_creature->m_npcInfo->Name.c_str());
		if (!ApplyMove())
			m_bHaveIdlePos = false;
	}
	else 
	{
		//LOG1("[%s] Finding Idle Place\n", m_creature->m_npcInfo->Name.c_str());
		int nNewPlace = rand() % 4;
		if (nNewPlace!=0)
			return;

		// Find a random place to go

		int range = m_creature->m_patrolRange;

		long ranX = rand() % (range*2);
		long ranY = rand() % (range*2);
		ranX -= range;
		ranY -= range;

		m_idleToPos[0] = m_StartNode[0] + ranX;
		m_idleToPos[1] = m_StartNode[1] + ranY;

		m_bHaveIdlePos = true;

		if (!UpdateMovePath(m_idleToPos[0], m_idleToPos[1]))
		{
		}
		else
		{
			//LOG2("Create a new pos[x=%d,y=%d] to go for guarder\n", m_idleToPos[0], m_idleToPos[1]);
		}
	}
}

void CRegionAssistantAI::DoPatrol()
{
	CHECK(m_state == STATE_MOVE);

	if ((m_creature->m_patrolRange<=0) ||
		m_PatrolPath.empty())
	{
		return;
	}

	//m_Arrived：是否到达目标节点，此点不仅仅是巡逻路径上的节点
	if ( !m_Arrived && !ApplyMove())
	{
		//
	}

	if (m_creature->Distance(m_TargetNode) < 30)
	{
		m_Arrived = true;
		m_Left	  = false;
	}
	else
	{
		m_Arrived = false;
	}

	//已到达目标节点
	if(m_Arrived)
	{		
		//判断是否到达巡逻终点		
		if((++ m_IterNode) == m_PatrolPath.end())
		{
			//若到终点，返回
			m_PatrolPath.reverse();
			m_IterNode = m_PatrolPath.begin();
			m_IterNode ++;
			if(m_creature->m_npcInfo)
				LOG3("OnArrive OK : [%s] at pos [%d][%d]\n", (char*)m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);
			if ( !OnArrivePoint() )
				return;
		}
	}

	//重新设定巡逻目标
	m_TargetNode[0] = (*m_IterNode).x;
	m_TargetNode[1] = (*m_IterNode).y;

	if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	{
		//do nothing
		if(m_creature->m_npcInfo)
			LOG3("UpdateMovePath Error [%s]   [%d][%d]\n", (char*)m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);

		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
	}		
}

bool CRegionAssistantAI::DoScanTarget()
{
	//视野为0
	if (m_creature->m_viewRange == 0)
	{
		return false;
	}

	//取得周围的生物
	std::vector<CRegionCreature*> creList;
	m_creature->m_scene->GetAroundCreatureByGrid( m_creature->m_pos, 
		m_creature->m_viewRange,
		creList,
		m_creature );

	if (creList.empty())
	{
		return false;
	}

	//最大视野范围
	int viewRange = m_creature->m_viewRange * m_creature->m_viewRange;

	//最小视野
	int minRange = viewRange + 1;

	std::vector<CRegionCreature*>::iterator nearest = creList.end();
	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	//遍历取得离自己最近的目标
	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		//攻击对立阵营npc
		if (!(*iter)->IsUser() && !(*iter)->IsUserPet() && !(*iter)->IsCallNpc())
		{		
			if( ((*iter)->m_core.Faction != FACTION_UNKNOWN) &&
				((*iter)->m_core.Faction == m_creature->m_core.Faction))
			{
				continue;
			}

			if ((*iter)->m_core.Faction == FACTION_UNKNOWN)
			{
				continue;
			}
		}

		// 不攻击隐身生物
		if ( !(*iter)->State_IsVisible() )
			continue;

		//攻击对立阵营
		if((*iter)->IsUserPet() || !(*iter)->IsCallNpc())
		{
			if(((*iter)->m_master->GetPKState() != PK_STATE_RED_NAME) &&
				((*iter)->m_master->m_core.Faction == m_creature->m_core.Faction))
			{
				continue;
			}
		}else if((*iter)->IsUser())
		{
			if (((*iter)->GetPKState()!= PK_STATE_RED_NAME) &&
				((*iter)->m_core.Faction == m_creature->m_core.Faction))
				continue;
		}

		int x = (*iter)->m_pos[0] - m_creature->m_pos[0];
		int y = (*iter)->m_pos[1] - m_creature->m_pos[1];

		//守卫与目标距离
		int range = x * x + y  * y;

		if (range < minRange && range <= viewRange)
		{
			minRange = range;
			nearest = iter;
		}
	}

	//搜寻到目标
	if (nearest != creList.end())
	{
		m_creature->SetTarget(*nearest);
		return true;
	}	

	return false;
}

bool CRegionAssistantAI::BeginReturn()
{
	SetState(STATE_RETURN);

	if(m_creature->IsScriptLoaded())
	{
		m_creature->CallScript("LeaveBattle", false);
	}

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		memcpy((void*)m_TargetNode, (void*)m_StartNode, sizeof(m_StartNode));
	}

	m_creature->RemoveAllTargets();

	if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	{
		if(m_creature->m_npcInfo)
			LOG3("BeginReturn: [%s] Cannot Find Path to [%d][%d]\n", (char*)m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);
		//m_creature->m_dir = m_StartDir;
		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
		m_Arrived = true;
		m_Left	  = false;
		SetState(STATE_MOVE);
	}
	return true;
}

void CRegionAssistantAI::DoReturn()
{
	if (m_creature->Distance(m_TargetNode) < 40)
	{
		SetState(STATE_MOVE);

		m_Arrived = true;
		m_Left	  = false;

		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
		m_creature->m_dir = m_StartDir;

		return;
	}

	if (m_creature->CanMove() && !ApplyMove())
	{
		if (UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
		{
			if (!ApplyMove())
			{
				SetState(STATE_MOVE);
			}
		}
		else 
		{
			SetState(STATE_MOVE);
		}
	}else
	{
		//LOG("DoReturn Unknown Error: cannot move or apply move failed\n");
		SetState(STATE_MOVE);

		m_Arrived = true;
		m_Left	  = false;

		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
	}
}

void CRegionAssistantAI::SetStartNode(long* pos)
{
	m_StartNode[0] = pos[0];
	m_StartNode[1] = pos[1];
}

void CRegionAssistantAI::SeekHelp(CRegionCreature* target)
{	
	if(target == NULL)
		return;
	if (m_creature->m_assistRange == 0)
		return;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos,
		m_creature->m_assistRange,
		creList,
		m_creature);

	if (creList.empty())
		return;

	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_core.Faction != m_creature->m_core.Faction)
			continue;

		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_assistRange))
			continue;

		//找到目标
		if ((*iter)->m_ai)
		{
			if (((*iter)->m_ai->GetState() != STATE_COMBAT) &&
				((*iter)->m_ai->GetState() != STATE_RETURN))
			{
				(*iter)->m_ai->OnAssist(target);

#ifdef _DEBUG
				LOG2("[%s]ask help for[%s]\n", m_creature->m_npcInfo->Name.c_str(),
					(*iter)->m_ai->m_creature->m_npcInfo->Name.c_str());
#endif //debug
			}
		}
	}
}

bool CRegionAssistantAI::OnArrivePoint()
{
	if(!CRegionAI::OnArrivePoint())
	{
		return false;
	}

	m_creature->InvokeEventCommand("on_arrive");

	return true;
}

CRegionHeroAI::CRegionHeroAI()
: CRegionAI(TYPE_HERO)
{
}

void CRegionHeroAI::BeginFollow(CRegionCreature* creature)
{
	SetState(STATE_FOLLOW);
	m_creature->SetTarget(creature);
	m_creature->m_lockTarget = true;
	UpdateMovePath(creature->m_pos[0], creature->m_pos[1]);
}

void CRegionHeroAI::DoFollow(CRegionCreature* target)
{
	if (m_targetPos[0] != target->m_pos[0] || m_targetPos[1] != target->m_pos[1])
	{
		if (!UpdateMovePath(target->m_pos[0], target->m_pos[1]))
		{	
			return;
		}		

		m_targetPos[0] = target->m_pos[0];
		m_targetPos[1] = target->m_pos[1];
	}

	if (m_creature->CanMove() && !ApplyMove())
	{
		if (UpdateMovePath(target->m_pos[0], target->m_pos[1]))
			ApplyMove();
	}
}

bool CRegionHeroAI::OnFollow(CRegionCreature* target)
{
	if (!CRegionAI::OnFollow(target))
		return false;

	BeginFollow(target);

	return true;
}

bool CRegionHeroAI::OnThink()
{
	if (!CRegionAI::OnThink())
		return false;

	// 如果离主人距离过远，放弃当前状态，直接传送，进入follow状态
	if (m_creature->IsOutOfRange(m_creature->m_master->m_pos, 600))
	{
		m_creature->ForceSetPosition(m_creature->m_master->m_pos[0],m_creature->m_master->m_pos[1],
			m_creature->m_master->m_pos[2]);
		BeginFollow(m_creature->m_master);
		return true;
	}

	DoFollow(m_creature->m_master);

	return true;
}
//tim.yang   神灯召唤怪物AI
bool CGodLightMonsterAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	//初始化
	m_orignalPos[0] = creature->m_pos[0];
	m_orignalPos[1] = creature->m_pos[1];
	m_orignalPos[2] = creature->m_pos[2];	

	m_creature->m_lockTarget = true;
	//m_pConjureAI = NULL;

	return true;
}

bool CGodLightMonsterAI::OnBeAttacked(CRegionCreature* target, int hate)
{
	if (!target || target == m_creature)
		return false;

	if (!CRegionAI::OnBeAttacked(target, hate))
		return false;	

	//if ((m_state != STATE_COMBAT) &&
	//	(m_state != STATE_SUCCOR))
	//	SetState(STATE_COMBAT);

	m_isBeAttacked = true;
	if(!m_TargetDBID)
	{
		//m_TargetObjID = target->GetObjectId();	
		m_TargetDBID = target->m_userInfo->m_userId;
	}

	m_creature->AddTarget(target, hate, hate);
	return true;
}

bool CGodLightMonsterAI::OnThink()
{
	//if (!CRegionAI::OnThink())
	//	return false;
	if(!m_creature)
		return false;
	CRegionCreature *curTarget = 0;
	CRegionUser *curTargetUser = 0;
	//if(m_creature->m_scene)
	//	curTarget = m_creature->m_scene->FindCreature(m_TargetObjID);
	//if(m_creature->m_scene)
	curTargetUser = g_region->FindUser(m_TargetDBID);
	if(!curTargetUser)
	{
		return false;
	}
	curTarget = curTargetUser->m_dummy;
	//获得当前攻击目标
	CRegionCreature* target = m_creature->GetCurrentTarget();

	//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
	if ((target != NULL) && (target->m_IsHiding != 0))
	{
		target->LeaveCombat();
		return false;
	}

	if(!target)
	{
		target = curTarget;
		OnBeAttacked(target,10000);
	}
	if(target->m_core.Name != curTarget->m_core.Name)
	{
		target = curTarget;
		OnBeAttacked(target,10000);
	}
	if(target && target->GetPlayerLogic()->IsLive())
	{
		OnCombat(target);
	}
	if (!m_creature->CanMove())
		m_creature->ForceSetPosition(target->m_pos[0],target->m_pos[1],
		target->m_pos[2]);
	// 如果离目标距离过远，放弃当前状态，直接传送
	if (m_creature && m_creature->IsOutOfRange(target->m_pos, 200))
	{
		m_creature->ForceSetPosition(target->m_pos[0],target->m_pos[1],
			target->m_pos[2]);
	}
	return true;
}
//end

//tim.yang  MCAI
std::vector<SPathPoint> CMCAI::_ChangeStringToSPointVector(const std::string str)
{
	std::vector<SPathPoint> temp;
	if (!str.empty())
	{
		int start = 0;
		int end = str.find(",",start);
		while(std::string::npos != end)
		{
			SPathPoint sp;
			sp.x = atoi(str.substr(start,end - start).c_str()) * 20;
			start = end + 1;
			end = str.find("/",start);
			sp.y = atoi(str.substr(start,end - start).c_str()) * 20;
			temp.push_back(sp);
			start = end + 1;
			end = str.find(",",start);
		}
	}
	return temp;
}

double CMCAI::_GetDisBetweenTwoPoint(SPathPoint sp1,SPathPoint sp2)
{
	return sqrt(pow((double)(sp1.x - sp2.x),2) + pow((double)(sp1.y - sp2.y),2));
}

CMCAI::CMCAI()
{
	m_mc = CMonCastle::GetMCInstance();
	memset(m_curTargetPos,0,sizeof(m_curTargetPos));
}

CMCAI::~CMCAI()
{

}

std::vector<SPathPoint> CMCAI::_GetPathPoint(byte country)
{
	std::string stemp = m_mc->GetPosVector(country);
	return _ChangeStringToSPointVector(stemp);
}

SPathPoint CMCAI::_GetNearestDis(long *orignalPos, std::vector<SPathPoint> pathPoint)
{
	std::vector<SPathPoint>::iterator it = pathPoint.begin();
	SPathPoint spNearest;
	SPathPoint curPos;
	curPos.x = orignalPos[0];
	curPos.y = orignalPos[1];
	double nearest = _GetDisBetweenTwoPoint(curPos,(*it));
	spNearest = (*it);
	++it;
	double temp = 0;
	for (;it != pathPoint.end(); ++it)
	{
		temp = _GetDisBetweenTwoPoint(curPos,(*it));
		if (temp < nearest)
		{
			nearest = temp;
			spNearest = (*it);
		}
	}
	return spNearest;
}

bool CMCAI::_ReCalCurTargetPos(long *curTargetPos)
{
	long disX = 0;
	long disY = 0;
	disX = (curTargetPos[0] - m_creature->m_pos[0]) / 2;
	disY = (curTargetPos[1] - m_creature->m_pos[1]) / 2;
	curTargetPos[0] -= disX;
	curTargetPos[1] -= disY;
	if (abs(disX) <= 10 && abs(disY) <= 10)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CMCAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	//自动寻路的路点初始化
	if (0 == creature->m_core.Faction || 1 == creature->m_core.Faction)
	{
		m_pathPoint = _GetPathPoint(creature->m_core.Faction);
		m_attackTargetObj = m_mc->GetTargetObjId(creature->m_core.Faction);
	}
	else
	{
		return false;
	}

	//初始化
	//m_orignalPos[0] = creature->m_pos[0];
	//m_orignalPos[1] = creature->m_pos[1];
	//m_orignalPos[2] = creature->m_pos[2];	

	m_creature->m_lockTarget = false;

	m_Arrived	   = false;
	m_Left		   = false;

	//巡逻目标点初始化
	//m_PatrolPath = creature->m_npcInfo->PatrolPath;

	//有巡逻AI
	//if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
	//{
		if(m_pathPoint.empty())
		{
			LOG("The pathPoint is empty \n");
			//m_dwLevelFlag &= (~AI_PATROL);
			//m_dwLevelFlag |= AI_IDLE;
			
			m_TargetNode[0] = creature->m_pos[0];
			m_TargetNode[1] = creature->m_pos[1];		
		}else
		{
			SPathPoint temp = _GetNearestDis(creature->m_pos, m_pathPoint);
			m_pathPointNext = find(m_pathPoint.begin(),m_pathPoint.end(),temp);
			m_TargetNode[0] = (*m_pathPointNext).x;
			m_TargetNode[1] = (*m_pathPointNext).y;
			//m_orignalPos[0] = m_TargetNode[0];
			//m_orignalPos[1] = m_TargetNode[1];
			//m_orignalPos[2] = m_TargetNode[2];

			//m_creature->SetPosition(m_orignalPos[0], m_orignalPos[1], 0);
		}
		m_curTargetPos[0] = m_TargetNode[0];
		m_curTargetPos[1] = m_TargetNode[1];
	//}else
	//{
	//	m_TargetNode[0] = creature->m_pos[0];
	//	m_TargetNode[1] = creature->m_pos[1];
	//}

	//有召唤AI
	//if(CONJURE(m_dwLevelFlag) == AI_CONJURE)
	//{
	//	m_pConjureAI = RT_NEW CRegionConjureAI(creature, CONJURE_SKILL_ID, m_creature->m_npcInfo->ConjureTarID);
	//}else
	//{
	//	m_pConjureAI = NULL;
	//}

	m_state = STATE_MOVE;

	return true;
}

void CMCAI::DoIdle()
{
	CHECK(m_state == STATE_MOVE);

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		return;
	}

	//if (m_creature->m_patrolRange<=0)
	//	return;

	//if (m_bHaveIdlePos)
	//{
	//	if (!ApplyMove())
	//		m_bHaveIdlePos = false;
	//}
	//else 
	//{
	//	int nNewPlace = rand() % 4;
	//	if (nNewPlace!=0)
	//		return;

	//	// Find a random place to go
	//	int range;
	//	if (m_creature->m_patrolRange==0)
	//		range = 80;
	//	else 
	//		range = m_creature->m_patrolRange * 0.5;

	//	long ranX = rand() % (range*2);
	//	long ranY = rand() % (range*2);
	//	ranX -= range;
	//	ranY -= range;

	//	m_idleToPos[0] = m_orignalPos[0] + ranX;
	//	m_idleToPos[1] = m_orignalPos[1] + ranY;

	//	m_bHaveIdlePos = true;

	//	if (!UpdateMovePath(m_idleToPos[0], m_idleToPos[1]))
	//	{
	//	}
	//	else
	//	{
	//	}
	//}

	CRegionCreature* attactTarget = m_creature->m_scene->FindCreature(m_attackTargetObj);
	if (!attactTarget)
	{
		return;
	}
	if (m_creature->IsOutOfRange(attactTarget->m_pos,100))
	{
		/*if ((m_creature->m_patrolRange<=0) ||
		m_PatrolPath.empty())
		{
		return;
		}*/
		//m_Arrived：是否到达目标节点，此点不仅仅是巡逻路径上的节点
		if (/* !m_Arrived && */!ApplyMove())
		{
			if (!_ReCalCurTargetPos(m_curTargetPos))
			{
				m_curTargetPos[0] = m_TargetNode[0];
				m_curTargetPos[1] = m_TargetNode[1];
			}
		}

		if (m_creature->Distance(m_TargetNode) < 50)
		{
			m_Arrived = true;
			m_Left	  = false;
		}
		else
		{
			m_Arrived = false;
		}

		//已到达目标节点
		if(m_Arrived)
		{		
			//判断是否到达巡逻终点		
			if((++ m_pathPointNext) == m_pathPoint.end())
			{
				//到达终点攻击NPC
				m_state = STATE_COMBAT;
			}
			else
			{
				//重新设定巡逻目标
				m_TargetNode[0] = (*m_pathPointNext).x;
				m_TargetNode[1] = (*m_pathPointNext).y;
				m_curTargetPos[0] = m_TargetNode[0];
				m_curTargetPos[1] = m_TargetNode[1];
			}
		}

		if (!UpdateMovePath(m_curTargetPos[0], m_curTargetPos[1]))
		{
			//do nothing
			/*	_ReCalCurTargetPos(m_curTargetPos);*/
		}	
	}
	else
	{
		OnBeAttacked(attactTarget, 100000);
		m_state = STATE_COMBAT;
	}
}

bool CMCAI::OnThink()
{
	if (!CRegionAI::OnThink())
		return false;

	if (!m_creature->CanMove())
		return false;

	switch(m_state)
	{
		//闲散状态
	case STATE_MOVE:
		if(!DoScanTarget())
		{
			DoMove();
		}
		else
		{
			SetState(STATE_COMBAT);

			//if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
			//{
				//保存当前位置
				SaveCurrPos();
			//}
		}
		break;

		//返回状态
	case STATE_RETURN:
		DoReturn();
		break;
//
//		//战斗状态
	case STATE_COMBAT:

		SelfSuccor();

/*		if (m_creature->IsOutOfRange(m_orignalPos, m_creature->m_traceRange))
		{
#ifdef _DEBUG
			if (_need_debug(m_creature))
				RtCoreLog().Debug("#%d out_of_range, return now\n", m_creature->m_oId);
#endif
			BeginReturn();
		}		
		else*/
		{			
			CryForHelp();

			//获得当前攻击目标
			CRegionCreature* target = m_creature->GetCurrentTarget();

			//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
			if ((target != NULL) && (target->m_IsHiding != 0))
			{
				target->LeaveCombat();
				break;
			}

			if(target != NULL && target->GetPlayerLogic()->IsLive())
			{
				OnCombat(target);
				if (m_creature->IsOutOfRange(target->m_pos, 50))
				{
					BeginReturn();
				}
			}
			else
			{
				BeginReturn();
			}/*else
			{	
				m_SelfRewardNum = SELF_REWARD_NUM;

				SetState(STATE_MOVE);
				m_isBeAttacked = false;
			}*/
		}
		break;
//
//		//救援状态
//	case STATE_SUCCOR:
//		{
//			if (m_creature->IsOutOfRange(m_orignalPos, m_creature->m_traceRange))
//			{
//				BeginReturn();
//				break;
//			}
//
//			//获得当前救援目标
//			CRegionCreature* target = m_creature->m_scene->FindCreature(m_proTargetID);
//
//			if(target != NULL)
//			{
//				/*if (m_creature->Distance(target->m_pos) > 60)
//				{
//				DoTrace(target);
//				break;
//				}*/
//
//				SuccorMode(target);
//
//				if(m_SuccorMode == SUCCOR_MODE_REWARD)
//				{
//					//查找有益技能并施放
//					CSkillInstance* p_instance = m_creature->m_Skill.FirstActiveSkill();
//
//					while(p_instance && p_instance->m_pAttr && p_instance->m_pAttr->bABad)
//					{
//						p_instance = m_creature->m_Skill.NextActiveSkill();
//					}
//
//					if(p_instance && p_instance->m_pAttr)
//					{
//						int skill = p_instance->m_pAttr->wID;
//						if(!m_creature->m_Skill.CanUseSkill(skill,&(target->m_Skill)))
//							skill = 0;
//
//						if(skill != 0)
//						{
//							SAttackParam param;
//							param.Seed = rand()%60000;
//							param.EnemyId = m_proTargetID;
//							param.Skill = skill;
//							param.TargetType = 1;
//
//							m_creature->UseSkill(param);
//
//							break;
//						}
//					}
//				}
//
//				//获得当前攻击目标
//				CRegionCreature* enemy_target = m_creature->GetCurrentTarget();
//
//				if(enemy_target != NULL)
//				{
//					OnCombat(enemy_target);
//				}else
//				{
//					CRegionCreature* enemy = target->GetCurrentTarget();
//
//					if(enemy != NULL)
//					{
//						m_creature->SetTarget(enemy);
//					}else
//					{
//						SetState(STATE_MOVE);
//
//						m_isBeAttacked = false;
//					}
//				}					
//
//			}else
//			{
//				SetState(STATE_MOVE);
//
//				m_isBeAttacked = false;
//			}
//
//			break;
//		}
//
//		//逃跑状态
//	case STATE_RUNAWAY:
//		if (IsOutOfRange(m_creature->m_pos, m_orignalPos, 500))
//		{
//			LOG1("AI(oId=%d): 超出逃跑范围\n", m_creature->m_oId);
//			BeginReturn();
//		}
//		else
//		{
//			DoRunaway();
//		}
//		break;

		//其他
	default:
		SetState(STATE_MOVE);
		m_isBeAttacked = false;
		break;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
//	TourAI
//////////////////////////////////////////////////////////////////////////

CTourAI::CTourAI()
: CRegionAI(TYPE_TOUR),
m_tBeAttTime(0),
c_tResumeTime(8000),
c_iGuardCount(8)
{
	m_state	  = STATE_MOVE;
	m_Arrived = false;
	m_Left	  = false;
	m_bHaveIdlePos = false;
}

CTourAI::CTourAI(Type type)
: CRegionAI(type),
m_tBeAttTime(0),
c_tResumeTime(8000),
c_iGuardCount(8)
{
	m_state	  = STATE_MOVE;
	m_Arrived = false;
	m_Left	  = false;
	m_bHaveIdlePos = false;
}

bool CTourAI::OnAssist(CRegionCreature* target)
{
	if (!CRegionAI::OnAssist(target))
		return false;
	return true;

	if ((m_state == STATE_COMBAT) &&
		(m_state == STATE_RETURN))
		return false;

	m_creature->SetTarget(target);

	//测试该目标是否有效
	CRegionCreature* p_cre = m_creature->GetCurrentTarget();

	if(p_cre != NULL)
	{
		SetState(STATE_COMBAT);

		//保存当前位置
		SaveCurrPos();

#ifdef _DEBUG
		LOG1("the guarder [%s] received help info\n", m_creature->m_npcInfo->Name.c_str());
		LOG2("It's current position is [%d][%d]\n", m_creature->m_pos[0], m_creature->m_pos[1]);
		LOG2("Saved position is [%d][%d]\n\n ", m_TargetNode[0], m_TargetNode[1]);
#endif //debug

		return true;
	}

	m_creature->RemoveTargetByCreature(target);

	return false;
}

bool CTourAI::OnAttack(CRegionCreature* target)
{
	return true;
}

bool CTourAI::OnBeAttacked(CRegionCreature* target, int hate)
{
	if (target == m_creature || target == m_creature->m_master)
		return false;

	if (m_state==STATE_RETURN || m_state==STATE_RUNAWAY)
		return false;

	if (!CRegionAI::OnBeAttacked(target, hate))
		return false;

	if(PROTECT(m_dwLevelFlag) == 0)
	{
		m_isBeAttacked = true;
		m_tBeAttTime = rtGetMilliseconds();
		SetState(STATE_SUCCOR);
		return true;
	}

	if (m_state != STATE_COMBAT)
		SetState(STATE_COMBAT);

	m_creature->AddTarget(target, hate, hate);
	//m_creature->SetTarget(target);

	//SeekHelp(target);
	m_isBeAttacked = true; 

	return true;
}

bool CTourAI::OnBorn(CRegionCreature* creature)
{
	if (!CRegionAI::OnBorn(creature))
		return false;

	//初始化
	m_StartNode[0] = creature->m_pos[0];
	m_StartNode[1] = creature->m_pos[1];
	m_StartNode[2] = creature->m_pos[2];
	m_StartDir	   = creature->m_dir;
	m_Arrived	   = false;
	m_Left		   = false;

	//巡逻目标点初始化
	m_PatrolPath = creature->m_npcInfo->PatrolPath;

	if(m_PatrolPath.empty())
	{
		m_dwLevelFlag &= (~AI_PATROL);
		m_dwLevelFlag |= AI_IDLE;

		m_TargetNode[0] = creature->m_pos[0];
		m_TargetNode[1] = creature->m_pos[1];
	}else
	{		
		m_IterNode		= m_PatrolPath.begin();
		m_TargetNode[0] = (*m_IterNode).x;
		m_TargetNode[1] = (*m_IterNode).y;
	}

	m_state = STATE_MOVE;

	m_tBorn = (DWORD)time(NULL);

	return true;
}

bool CTourAI::OnCombat(CRegionCreature* target)
{
	if (CanAttack(target))
	{
		DoAttack();
	}
	else
	{
		DoTrace(target);				
	}

	return true;
}

bool CTourAI::OnThink()
{
	if (!CRegionAI::OnThink())
		return false;

	if (!m_creature->CanMove())
		return false;

	switch(m_state) 
	{
		//运动状态
	case STATE_MOVE:
		//if(!DoScanTarget())
		//{
		//	DoMove();
		//}
		//else if(PROTECT(m_dwLevelFlag) != 0)
		//{
		//	//状态转换
		//	SetState(STATE_COMBAT);

		//	if(MOVETYPE(m_dwLevelFlag) == AI_PATROL)
		//	{
		//		//保存当前位置
		//		SaveCurrPos();
		//		LOG3("[%s] Find Target: curr pos [%d][%d]\n", m_creature->m_npcInfo->Name.c_str(), m_creature->m_pos[0], m_creature->m_pos[1]);
		//		LOG3("[%s] Return Node: [%d][%d]\n", m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);
		//	}			
		//}
		//else
		{
			DoMove();
		}
		break;

		//返回状态
	//case STATE_RETURN:
	//	DoReturn();
	//	break;

		//战斗状态
	case STATE_COMBAT:
		{
			//获得当前攻击目标
			CRegionCreature* target = m_creature->GetCurrentTarget();

			//Tianh  //目标是在隐身的状态就跳出函数 不进行下一步的操作了
			if ((target != NULL) && (target->m_IsHiding != 0))
			{
				target->LeaveCombat();
				break;
			}

			if(target != NULL)
			{
				if(CanAttack(target))
				{
					DoAttack();
				}
				else
				{		
					//越界？返回		
					if (m_creature->IsOutOfRange(m_TargetNode, m_creature->m_traceRange))
					{
						SetState(STATE_MOVE);
						//if(m_creature->m_traceRange == 0)
						//{
						//	SetState(STATE_MOVE);
						//}else
						//{
						//	BeginReturn();
						//}
					}		
					else
					{
						if(target != NULL && target->GetPlayerLogic()->IsLive())
						{
							OnCombat(target);
						}
						else
						{
							SetState(STATE_MOVE);
							//BeginReturn();
						}
					}
				}
			}else
			{
				SetState(STATE_MOVE);
				//BeginReturn();
			}
		}
		break;

		//被攻击时，如果没有反击能力，就在这里执行求助
	case STATE_SUCCOR:
		if(rtGetMilliseconds() - m_tBeAttTime > c_tResumeTime)
		{
			m_creature->LeaveCombat();
			m_isBeAttacked = false;
			SetState(STATE_MOVE);
		}
		if(PROTECT(m_dwLevelFlag) == 0)
		{

		}
		else
		{
			SetState(STATE_COMBAT);
		}
		break;

		//其他情况
	default:
		SetState(STATE_MOVE);
		break;
	}

	return true;
}

void CTourAI::DoMove()
{
	switch(MOVETYPE(m_dwLevelFlag))
	{
	case AI_PATROL:
		DoPatrol();
		break;
	default:
		//DoIdle();
		break;
	}
}

void CTourAI::DoIdle()
{
	CHECK(m_state == STATE_MOVE);

	//if(m_creature->m_npcInfo->Id == 1254)
	//	LOG1("[%s] Into StateMove\n", m_creature->m_npcInfo->Name.c_str());

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		return;
	}

	if (m_creature->m_patrolRange<=0)
		return;

	if (m_bHaveIdlePos)
	{
		//LOG1("[%s] Finded Idle Place\n", m_creature->m_npcInfo->Name.c_str());
		if (!ApplyMove())
			m_bHaveIdlePos = false;
	}
	else 
	{
		//LOG1("[%s] Finding Idle Place\n", m_creature->m_npcInfo->Name.c_str());
		int nNewPlace = rand() % 4;
		if (nNewPlace!=0)
			return;

		// Find a random place to go

		int range = m_creature->m_patrolRange;

		long ranX = rand() % (range*2);
		long ranY = rand() % (range*2);
		ranX -= range;
		ranY -= range;

		m_idleToPos[0] = m_StartNode[0] + ranX;
		m_idleToPos[1] = m_StartNode[1] + ranY;

		m_bHaveIdlePos = true;

		if (!UpdateMovePath(m_idleToPos[0], m_idleToPos[1]))
		{
		}
		else
		{
			//LOG2("Create a new pos[x=%d,y=%d] to go for guarder\n", m_idleToPos[0], m_idleToPos[1]);
		}
	}
}

void CTourAI::DoPatrol()
{
	CHECK(m_state == STATE_MOVE);

	if ((m_creature->m_patrolRange<=0) || m_PatrolPath.empty())
	{
		return;
	}

	//m_Arrived：是否到达目标节点，此点不仅仅是巡逻路径上的节点
	if ( !m_Arrived && !ApplyMove())
	{
		m_Arrived = true;
	}
	else
	{
		if (m_creature->Distance(m_TargetNode) < 40)
		{
			m_Arrived = true;
			m_Left	  = false;
		}
		else
		{
			m_Arrived = false;
		}
	}

	//已到达目标节点
	if(m_Arrived)
	{		
		RangeOutside(false);

		//判断是否到达巡逻终点		
		if((++ m_IterNode) == m_PatrolPath.end())
		{
			//若到终点，返回
			m_PatrolPath.clear();
			m_IterNode = m_PatrolPath.begin();
			if(m_creature->m_npcInfo)
				LOG3("OnArrive OK : [%s] at pos [%d][%d]\n", (char*)m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);
			OnArrivePoint();
			return;
		}
	}

	//重新设定巡逻目标
	m_TargetNode[0] = (*m_IterNode).x;
	m_TargetNode[1] = (*m_IterNode).y;

	UpdateMovePath(m_TargetNode[0], m_TargetNode[1]);
	//if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	//{
	//	if(m_creature->m_npcInfo)
	//		LOG3("UpdateMovePath Error [%s]   [%d][%d]\n", (char*)m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);

	//	m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
	//}		
}

bool CTourAI::DoScanTarget()
{
	//视野为0
	if (m_creature->m_viewRange == 0)
	{
		return false;
	}

	//取得周围的生物
	std::vector<CRegionCreature*> creList;
	m_creature->m_scene->GetAroundCreatureByGrid( m_creature->m_pos, 
		m_creature->m_viewRange,
		creList,
		m_creature );

	if (creList.empty())
	{
		return false;
	}

	//最大视野范围
	int viewRange = m_creature->m_viewRange * m_creature->m_viewRange;

	//最小视野
	int minRange = viewRange + 1;

	std::vector<CRegionCreature*>::iterator nearest = creList.end();
	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	//遍历取得离自己最近的目标
	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		//不攻击玩家
		if((*iter)->IsUser() || (*iter)->IsUserPet() || !(*iter)->IsCallNpc())
			continue;

		//攻击对立阵营npc
		if( ((*iter)->m_core.Faction != FACTION_UNKNOWN) &&
			((*iter)->m_core.Faction == m_creature->m_core.Faction))
			continue;
		if ((*iter)->m_core.Faction == FACTION_UNKNOWN)
			continue;

		// 不攻击隐身生物
		if ( !(*iter)->State_IsVisible() )
			continue;

		int x = (*iter)->m_pos[0] - m_creature->m_pos[0];
		int y = (*iter)->m_pos[1] - m_creature->m_pos[1];

		//守卫与目标距离
		int range = x * x + y  * y;

		if (range < minRange && range <= viewRange)
		{
			minRange = range;
			nearest = iter;
		}
	}

	//搜寻到目标
	if (nearest != creList.end())
	{
		m_creature->SetTarget(*nearest);
		return true;
	}	

	return false;
}

bool CTourAI::BeginReturn()
{
	m_creature->LeaveCombat();
	SetState(STATE_RETURN);

	if(m_creature->IsScriptLoaded())
	{
		m_creature->CallScript("LeaveBattle", false);
	}

	if(MOVETYPE(m_dwLevelFlag) == AI_FIXED)
	{
		memcpy((void*)m_TargetNode, (void*)m_StartNode, sizeof(m_StartNode));
	}

	m_creature->RemoveAllTargets();

	if (!UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
	{
		if(m_creature->m_npcInfo)
			LOG3("BeginReturn: [%s] Cannot Find Path to [%d][%d]\n", (char*)m_creature->m_npcInfo->Name.c_str(), m_TargetNode[0], m_TargetNode[1]);
		//m_creature->m_dir = m_StartDir;
		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
		m_Arrived = true;
		m_Left	  = false;
		SetState(STATE_MOVE);
	}
	return true;
}

void CTourAI::DoReturn()
{
	if (m_creature->Distance(m_TargetNode) < 40)
	{
		SetState(STATE_MOVE);

		m_Arrived = true;
		m_Left	  = false;

		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
		m_creature->m_dir = m_StartDir;

		return;
	}

	if (m_creature->CanMove() && !ApplyMove())
	{
		if (UpdateMovePath(m_TargetNode[0], m_TargetNode[1]))
		{
			if (!ApplyMove())
			{
				SetState(STATE_MOVE);
			}
		}
		else 
		{
			SetState(STATE_MOVE);
		}
	}else
	{
		//LOG("DoReturn Unknown Error: cannot move or apply move failed\n");
		SetState(STATE_MOVE);

		m_Arrived = true;
		m_Left	  = false;

		m_creature->SetPosition(m_TargetNode[0], m_TargetNode[1], 0);
	}
}

void CTourAI::SetStartNode(long* pos)
{
	m_StartNode[0] = pos[0];
	m_StartNode[1] = pos[1];
}

void CTourAI::SetGuard(CRegionCreature* pGuard)
{
	m_creature->ClearResideForNameColor();
	for(int ii=0; ii<c_iGuardCount; ii++)
		m_vGroudID[ii].Clear();
	if(pGuard && pGuard->IsUser())
	{
		if(pGuard->m_pTeam && pGuard->m_pTeam->mData.Size())
		{
			for(int ii=0; ii<pGuard->m_pTeam->mData.Size() && ii<c_iGuardCount; ++ii)
			{
				CRegionCreature *pUser = m_creature->m_scene->FindCreature(pGuard->m_pTeam->mData[ii].mDBID);
				if(pUser)
				{
					pUser->m_iGuardID = m_creature->GetObjectId();
					m_vGroudID[ii].SetUser(pGuard->m_pTeam->mData[ii].mDBID);
					m_creature->AddResideForNameColor(pGuard->m_pTeam->mData[ii].mDBID);
				}
			}
		}
		else
		{
			m_vGroudID[0].SetUser(pGuard->GetObjectId());
			pGuard->m_iGuardID = m_creature->GetObjectId();
			m_creature->AddResideForNameColor(pGuard->GetObjectId());
		}
	}
}

void CTourAI::CancelGuard(DWORD iUserID)
{
	for(int ii=0; ii<c_iGuardCount; ++ii)
	{
		if(m_vGroudID[ii] == iUserID)
		{
			CRegionCreature *pUser = m_creature->m_scene->FindCreature(iUserID);
			if(pUser)
				pUser->m_iGuardID = 0;
			m_creature->DeleteResideForNameColor(m_vGroudID[ii].miUserID);
			m_vGroudID[ii].Clear();
			break;
		}
	}

	bool bEmpty = true;
	for(int ii=0; ii<c_iGuardCount; ++ii)
	{
		if(!m_vGroudID[ii].Empty())
		{
			bEmpty = false;
			break;
		}
	}

	if(bEmpty)
		m_creature->InvokeEventCommand("delself");
}

void CTourAI::SeekHelp(CRegionCreature* target)
{	
	if(target == NULL)
		return;
	if(m_creature->m_assistRange == 0)
		return;

	std::vector<CRegionCreature*> creList;

	m_creature->m_scene->GetAroundCreatureByGrid(m_creature->m_pos,
		m_creature->m_assistRange,
		creList,
		m_creature);

	if (creList.empty())
		return;

	std::vector<CRegionCreature*>::iterator iter = creList.begin();

	for (; iter != creList.end(); iter++)
	{
		if ((*iter)->m_core.Faction != m_creature->m_core.Faction)
			continue;

		if ((*iter)->m_pos[0] == 0 && (*iter)->m_pos[1] == 0)
			continue;

		if (*iter == m_creature)
			continue;

		if (IsOutOfRange((*iter)->m_pos, m_creature->m_pos, m_creature->m_assistRange))
			continue;

		//找到目标
		if ((*iter)->m_ai)
		{
			if (((*iter)->m_ai->GetState() != STATE_COMBAT) &&
				((*iter)->m_ai->GetState() != STATE_RETURN))
			{
				(*iter)->m_ai->OnAssist(target);

#ifdef _DEBUG
				LOG2("[%s]ask help for[%s]\n", m_creature->m_npcInfo->Name.c_str(),
					(*iter)->m_ai->m_creature->m_npcInfo->Name.c_str());
#endif //debug
			}
		}
	}
}

void CTourAI::RangeOutside(bool bDelOutside)
{
	if(!m_creature || !m_creature->m_npcInfo)
		return;

	for(int ii=0; ii<c_iGuardCount; ++ii)
	{
		if(m_vGroudID[ii] == 0)
			continue;
		CRegionCreature *pUser = m_creature->m_scene->FindCreature(m_vGroudID[ii].miUserID);
		if(pUser && pUser->IsUser() && pUser->IsScriptLoaded())
		{
			long lDistance = m_creature->Distance(pUser->m_pos);
			if(lDistance > m_creature->m_npcInfo->ViewRange)
			{
				m_vGroudID[ii].miOutsideCount++;
				if(bDelOutside)
				{
					m_creature->DeleteResideForNameColor(m_vGroudID[ii].miUserID);
					m_vGroudID[ii].Clear();
				}
				else
					pUser->CallScript("RangeOutside", false, m_creature, m_vGroudID[ii].miOutsideCount);
			}
		}
		else
		{
			m_creature->DeleteResideForNameColor(m_vGroudID[ii].miUserID);
			m_vGroudID[ii].Clear();
		}
	}
}

bool CTourAI::OnArrivePoint(void)
{
	if(!CRegionAI::OnArrivePoint())
	{
		return false; 
	}

	if(m_creature->m_npcInfo)
	{
		if(IsScriptLoaded())
		{
			char sFuncName[50] = {0};
			rt2_snprintf(sFuncName, 50, "OnArrive%d", m_creature->m_npcInfo->Id);
			CallScript(sFuncName, false);
		}
		TalkOver();
		//RangeOutside(true);

		char sFuncName[50] = {0};
		rt2_snprintf(sFuncName, 50, "TourGuardEnd%d", m_creature->m_npcInfo->Id);
		for(int ii=0; ii<c_iGuardCount; ++ii)
		{
			if(!m_vGroudID[ii].Empty())
			{
				CRegionCreature *pUser = m_creature->m_scene->FindCreature(m_vGroudID[ii].miUserID);
				if(pUser && pUser->IsUser() && pUser->IsScriptLoaded())
					pUser->CallScript(sFuncName, false);
			}
		}

		m_creature->ClearResideForNameColor();
		for(int ii=0; ii<c_iGuardCount; ii++)
		{
			m_vGroudID[ii].Clear();
		}
		m_creature->InvokeEventCommand("delself");
	}
	return true;
}

//end
