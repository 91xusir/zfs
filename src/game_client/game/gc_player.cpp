
#include "gc_include.h"
#include "ui_form_minimap.h"
#include "ui_form_playeritems.h"
#include "ui_form_trade.h"
#include "ui_form_union.h"
#include "ui_form_msg.h"
#include "ui_form_feod_bank.h"
#include "ui_form_feod_content.h"
#include "game_assistant.h"
#include "ui_form_cre_info.h"
#include "ui_form_trump.h"
#include "UIForm_Pickup.h"
#include "ui_form_char.h"
#include "UIForm_Char.h"

GcPlayer::GcPlayer()
{
	m_posFlag = 0;
    m_iCheckSeed = (int)(GetWorld());

	//add for packet		ac.ma

	m_dwRedBtlCoolDown = 0;
	m_dwBlueBtlCoolDown= 0;
	m_dwMoveCoolDown= 0;
	m_dwPickItemCoolDown = 0;

	m_bAutoState = false;
	m_bAutoBattle = false;
	//m_bAutoEatRed = true;  //heten
	m_bAutoEatRed = false;   //change by yz :�����ѡ��
	//m_bAutoEatBlue = true; //heten
	m_bAutoEatBlue = false;  //change by yz :�����ѡ��
	m_bAutoPick = false;

	m_bChooseAutoHp30 = false;
	m_bChooseAutoHp60 = false;
	m_bChooseAutoHp90 = false;

	m_bWhiteAutoSell = false;
	m_bGreenAutoSell = false;
	m_bBlueAutoSell = false;
	m_bGoldAutoSell = false;

	m_iAutoHp = 60;			//�ٷֱ�
	m_iAutoMp = 60;			//�ٷֱ�
	//ac.ma
	m_iAddSettleTime = 0;
	m_isGoBack = false;
	m_iLastX = 0;
	m_iLastY = 0;
	m_curMonster = 0;
	/* gao 2010.2.1
	������ر�����ֵ
	*/
	m_dwGroupID = 0;
	m_dwLineID = 0;
	//add end;
	m_bShowID = false;
	m_dwLatestTarget = 0;
	m_listTempPkInfo.clear();
	m_dwGetDropTime = 0;
}

GcPlayer::~GcPlayer()
{
}

void GcPlayer::OnInit()
{
    GetWorld()->m_Team.SetID(ID(),DBID(),std::string(GetName()),0);
	GetWorld()->m_Friend.SetID(ID(),DBID());
    DisplayActor(true);

	// reset pos flag
	m_posFlag = 0;

	//Heten
	const char* strAutoMp = GetGameRule()->GetEntry("GameLevelRule","AutoMp");
	if(strAutoMp == NULL)
	{
		RtCoreLog().Info("[CItemManager::Init] Invalid GameRule Init File\n");
		m_iAutoMp = 20;
	}
	else
		m_iAutoMp = atoi(strAutoMp);	
	//end
}

void GcPlayer::OnNetUploadAttack()
{
	//mpAI->OnNetUploadAttack();
}

void GcPlayer::OnCreateGraph()
{
	return;
	CreateTrump();
}

void GcPlayer::SetTeamOff(long teamoff)
{
	/*if (teamoff == 0)
	{
		m_bTeamOnOff = false;
		LOAD_UI("btnteamoff")->SetText(string("��"));
	}else{
		m_bTeamOnOff = true;
		LOAD_UI("btnteamoff")->SetText(string("��"));
	}*/
}
bool GcPlayer::ActorInSkillTarget(GcActor* pActor, SSkill* pSkill)
{
    if (pActor==NULL)
    {
        return (pSkill->dwRTarget & SKILL_TARGET_POSITION)!=0;
    }
    if (pActor==this)
    {
        return (pSkill->dwRTarget & SKILL_TARGET_PLAYER)!=0;
    }
    if (pActor==GetWorld()->m_pPet)
    {
        return (pSkill->dwRTarget & SKILL_TARGET_PET)!=0;
    }
    if (pActor->NpcType()==GcActor::ENT_MONSTER)
    {
        if (pActor->m_pNpc->Type==4) // ҩ��
        {
            return (pSkill->dwRTarget & SKILL_TARGET_HERB)!=0;
        }else if (pActor->m_pNpc->Type==5) // ����
        {
            return (pSkill->dwRTarget & SKILL_TARGET_MINE)!=0;
        }else
        {
            return (pSkill->dwRTarget & SKILL_TARGET_ENEMY)!=0;
        }
    }
    if (pActor->NpcType()==GcActor::ENT_NPC_SERVICE)
    {
        return (pSkill->dwRTarget & (SKILL_TARGET_NEUTRAL_ACTOR|SKILL_TARGET_NEUTRAL_PET))!=0;
    }
	if (pActor->NpcType()==GcActor::ENT_NPC_COMBATIVE) // ��ս����npc
	{
		if (!pActor->m_pNpc)	return false;
		char npcFaction = pActor->m_pNpc->Faction;
		if (npcFaction==FACTION_UNKNOWN)
		{
			// ������Ӫ
			return (pSkill->dwRTarget & (SKILL_TARGET_NEUTRAL_ACTOR|SKILL_TARGET_NEUTRAL_PET))!=0;
		}
		else if (npcFaction==m_core.Faction)
		{
			// ͬһ��Ӫ
			//---------add start by tony 05.06.21----------------------------//
			if(!GetWorld()->IsCastleWarTime(this))
			{
			//---------add end   by tony 05.06.21----------------------------//
				return (pSkill->dwRTarget & SKILL_TARGET_FRIEND)!=0;
			}else
			{
				return true;
			}
		}
		else 
		{
			// ��ͬ��Ӫ
			return (pSkill->dwRTarget & (SKILL_TARGET_ENEMY|SKILL_TARGET_FACTION_x))!=0;
		}
	}
    if (pActor->NpcType()==GcActor::ENT_USER)
    {
		if (pActor->m_cIsDeadNet > 0)
		{
			if (pSkill->dwRTarget != SKILL_TARGET_CORPSE)
			{
				return false;
			}	
		}
        if (pSkill->dwRTarget&SKILL_TARGET_UNION)
        {
            if (pActor->GetUnionID()==GetUnionID())
            {
                return true;
            }
        }else if (pSkill->dwRTarget&SKILL_TARGET_UNION_x)
        {
            if (pActor->GetUnionID()!=GetUnionID())
            {
                return true;
            }
        }
        if (pSkill->dwRTarget&SKILL_TARGET_FACTION)
        {
            if (pActor->m_core.Faction==m_core.Faction)
            {
                return true;
            }
        }else if (pSkill->dwRTarget&SKILL_TARGET_FACTION_x)
        {
            if (pActor->m_core.Faction!=m_core.Faction)
            {
                return true;
            }
        }else if (pSkill->dwRTarget&SKILL_TARGET_CORPSE)
        {
			if (pActor->m_cIsDeadNet > 0)
			{
				if (pActor->m_core.Faction==m_core.Faction)
				{
					return true;
				}			
			}
        }
        return (pSkill->dwRTarget & (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_FRIEND_ACTOR))!=0;
    }
    if (pActor->NpcType()==GcActor::ENT_PET || pActor->NpcType()==GcActor::ENT_CALLNPC)
    {
		if (pSkill->dwRTarget&SKILL_TARGET_PETSELF)
		{
			if (pActor->GetMaster()->ID() == GetMaster()->ID())
			{
				return true;
			}
		}
        if (pSkill->dwRTarget&SKILL_TARGET_UNION)
        {
            if (pActor->GetMaster()->GetUnionID()==GetUnionID())
            {
                return true;
            }
        }else if (pSkill->dwRTarget&SKILL_TARGET_UNION_x)
        {
            if (pActor->GetMaster()->GetUnionID()!=GetUnionID())
            {
                return true;
            }
        }
        if (pSkill->dwRTarget&SKILL_TARGET_FACTION)
        {
            if (pActor->GetMaster()->m_core.Faction==m_core.Faction)
            {
                return true;
            }
        }else if (pSkill->dwRTarget&SKILL_TARGET_FACTION_x)
        {
            if (pActor->GetMaster()->m_core.Faction!=m_core.Faction)
            {
                return true;
            }
        }
        return (pSkill->dwRTarget & (SKILL_TARGET_ENEMY_PET|SKILL_TARGET_FRIEND_PET))!=0;
    }
    //case SKILL_TARGET_CORPSE       : return true;
    return false;
}

void GcPlayer::OnPlayFootfall()
{
    static const int iFileCnt = 33;
    static const char* pFileList[iFileCnt] = 
    {
        "footstep_01.wav",		// 1 ʯ��
        "footstep_02.wav",		// 2 ʯ�壨���ڻ�����
        "footstep_03.wav",		// 3 ����ʯ
        "footstep_04.wav",		// 4 С��ʯ
        "footstep_05.wav",		// 5 ��ש��
        "footstep_06.wav",		// 6 ɳ��
        "footstep_07.wav",		// 7 ʪ����
		"footstep_08.wav",		// 8 ������
		"footstep_09.wav",		// 9 ����
		"footstep_10.wav",		// 10 ���������ڻ���Ч����
		"footstep_11.wav",		// 11 �ɲݵ�
		"footstep_12.wav",		// 12 �����
		"footstep_13.wav",		// 13 ��������
		"footstep_14.wav",		// 14 ���ݵ�
		"footstep_15.wav",		// 15 ��֦��Ҷ��
		"footstep_16.wav",		// 16 ��ѩ��
		"footstep_17.wav",		// 17 ��ѩ�أ����ڻ���Ч����
		"footstep_18.wav",		// 18 ǳѩ��
		"footstep_19.wav",		// 19 ǳѩ�أ����ڻ���Ч����
		"footstep_20.wav",		// 20 ����
		"footstep_21.wav",		// 21 ���飨���ڻ���Ч����
		"footstep_22.wav",		// 22 ��ɽ�һ�����
		"footstep_23.wav",		// 23 ǳˮ��
		"footstep_24.wav",		// 24 ǳˮ�������ڻ���Ч����
		"footstep_25.wav",		// 25 ��ˮ
		"footstep_26.wav",		// 26 ����
		"footstep_27.wav",		// 27 �п�ľ��
		"footstep_28.wav",		// 28 ��ľ��
		"footstep_29.wav",		// 29 ʵľ�ذ�
		"footstep_30.wav",		// 30 ����ľ�壨���ڻ���Ч����
		"footstep_31.wav",		// 31 �п���Ƥ
		"footstep_32.wav",		// 32 ʵ������
		"footstep_33.wav",		// 33 ����
    };
    if (GetPlayer()==this)
    {
        RtgVertex3 vPos;
        vPos = this->GetMatrix()->Position();
        int iIdx = GetWorld()->GetScene()->GetTerrainMapType(vPos.x, vPos.y);
        if (iIdx>0 && iIdx<=iFileCnt)
        {
            g_pSoundMgr->PlayOnce(pFileList[iIdx-1], false, 0.5f, vPos);
        }
	}
}

void GcPlayer::AddPkTempUser(DWORD id,DWORD time /* = 0 */)
{
	std::list<SPkTempInfo>::iterator it = m_listTempPkInfo.begin();
	for (;it!=m_listTempPkInfo.end();)
	{
		if ((*it).dwTempPkID == id)
		{
			it = m_listTempPkInfo.erase(it);
		}else
		{
			++it;
		}
	}
	m_listTempPkInfo.push_back(SPkTempInfo(id,time));
}

void GcPlayer::OnPositionChanged(float fX, float fY)
{
	static int prevX = 2147483647;
	static int pervY = 2147483647;

    int currX = (int)fX;
    int currY = (int)fY;

    if (prevX == currX && pervY == currY)
        return;

    prevX = currX;
    pervY = currY;

    g_pScene->RequestBlockReady(RtgVertex3(fX, fY, 0.f));

    if (g_layerMain && g_layerMain->m_fromMiniMap)
        g_layerMain->m_fromMiniMap->SetPosition(/*GetMatrix()->_30*/fX, /*GetMatrix()->_31*/fY);
    
	GcActor::OnPositionChanged(fX, fY);

	if (m_listTempPkInfo.size())
	{
		std::list<SPkTempInfo>::iterator it = m_listTempPkInfo.begin();
		DWORD dwNowTime = rtGetMilliseconds();
		for (;it!=m_listTempPkInfo.end();)
		{
			if ((*it).dwTempPkTime < dwNowTime)
			{
				it = m_listTempPkInfo.erase(it);
			}else
			{
				++it;
			}
		}
	}

	if ( m_dwGetDropTime && m_dwGetDropTime < rtGetMilliseconds() )
	{
		m_dwGetDropTime = 0;
		if (g_layerMain->m_formPickItem->IsVisible())
			g_layerMain->m_formPickItem->Hide();
	}


    if (m_bIsCollect)
    {
        m_bIsCollect = false;
        SetIsTaskCollect(false);
        ChangeCollectTool(false);
        if (this == GetPlayer())
        {
            g_layerMain->m_formCollectBar->Hide();
            CG_CmdPacket* pPacket = NetBeginWrite();
            pPacket->WriteShort(c2r_stop_getskin);
            NetSend(pPacket);
        }
    }

    if (g_layerMain->m_formTrump->IsRifining())
        g_layerMain->m_formTrump->StartRifine();
    
    if (m_bInCallPet)
        m_bInCallPet = false;
    
    if (m_bIntonate)
    {
        m_bIntonate = false;
        ClearCommand();
        GetWorld()->OnNetUploadBreakIntonate();
    }

}

void GcPlayer::OnRemoveCreature()
{
    GcActor::OnRemoveCreature();
}

void GcPlayer::OnBlockChanged(RtsSceneBlockRtb* pNewBlock, RtsSceneBlockRtb* pOldBlock)
{
    GcActor::OnBlockChanged(pNewBlock, pOldBlock);
    if (pNewBlock==NULL)
    {
        g_pBackMusic->Stop();
    }else
    {
        if (pOldBlock==NULL || strcmp(pNewBlock->GetBackMusicFileName(),pOldBlock->GetBackMusicFileName())!=0)
        {
            //g_pBackMusic->Stop();
            if (pNewBlock->GetBackMusicFileName() && pNewBlock->GetBackMusicFileName()[0])
            {
                //g_pBackMusic->Play(pNewBlock->GetBackMusicFileName(), true);
				g_pMusicThread->Play(pNewBlock->GetBackMusicFileName(), false, 30);
            }
        }
    }
    if (g_pScene->IsShowBlockFlag())
    {
        if (pNewBlock==NULL) return;
        LOG1("GcPlayer::OnBlockChanged �����µĿ� [%s]\n", pNewBlock->GetBlockName());
        if (pNewBlock->GetFlagTexture()[0])
        {
            g_pScene->ShowSceneFlagTexture(pNewBlock->GetFlagTexture());
        }
    }
    if (pNewBlock && g_layerMain && g_layerMain->m_fromMiniMap)
    {
        if (pNewBlock->m_szBlockName[0])
        {
            g_layerMain->m_fromMiniMap->SetSceneName(pNewBlock->m_szBlockName);
        }
    }
}

const char* s_szKeyShowFlag = "ShowFlag:";
const char* s_szKeyName = "Name:";

void GcPlayer::OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock)
{
    guard;
    GcActor::OnAreaEnter(pArea, pBlock);
    int iLen;
    char *pStr, *pValue;
    char szValue[40];
    if (pArea==NULL) return;
    LOG2("GcPlayer::OnAreaEnter [%s] [%s]\n", pArea->szName, pArea->szData);
    if (g_pScene->IsShowBlockFlag())
    {
        pStr = strstr(const_cast<char*>(pArea->szData), const_cast<char*>(s_szKeyShowFlag));
        if (pStr)
        {
            pStr += strlen(s_szKeyShowFlag);
            pValue = strchr(pStr, ';');
            iLen = pValue-pStr;
            if (pValue && iLen>0)
            {
                rt2_strncpy2(szValue, pStr, iLen);
                szValue[iLen] = 0;
                if (pBlock==NULL || strcmp(szValue, pBlock->GetFlagTexture())!=0)
                {
                    g_pScene->ShowSceneFlagTexture(szValue);
                }
            }
        }
    } // if (g_pScene->IsShowBlockFlag())
    unguard;
}

void GcPlayer::OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock)
{
    guard;
    GcActor::OnAreaExit(pArea, pBlock);
    LOG2("GcPlayer::OnAreaExit [%s] [%s]\n", pArea->szName, pArea->szData);
    if (g_pScene->IsShowBlockFlag())
    {
        if (pBlock && pBlock->GetFlagTexture()[0])
        {
            g_pScene->ShowSceneFlagTexture(pBlock->GetFlagTexture());
        }
    }
    unguard;
}

void GcPlayer::UpdateAreaPKAttr()
{
    EPKAttr eOld = m_eCurPosPKAttr;
    GcActor::UpdateAreaPKAttr();
    if (eOld!=m_eCurPosPKAttr)
    {
        switch (eOld)
        {
        case PKA_SAFEST:
            LOG("�뿪������\n");
            break;
        case PKA_SAFE:
            LOG("�뿪��ȫ��\n");
            break;
        case PKA_UNSAFE:
            LOG("�뿪ս����\n");
            break;
        case PKA_NORMAL:
            LOG("�뿪��ͨ��\n");
            break;
        case PKA_PKUNABLE:
            LOG("�뿪��ֹPK��\n");
            break;
        }
        switch (m_eCurPosPKAttr)
        {
        case PKA_SAFEST:
            LOG("����������\n");
            break;
        case PKA_SAFE:
            LOG("���밲ȫ��\n");
            break;
        case PKA_UNSAFE:
            LOG("����ս����\n");
            break;
        case PKA_NORMAL:
            LOG("������ͨ��\n");
            break;
        case PKA_PKUNABLE:
            LOG("�����ֹPK��\n");
            break;
        }
        if (g_layerMain)
        {
            g_layerMain->m_fromMiniMap->SetPKState(m_eCurPosPKAttr);
        }
    }
}

void GcPlayer::OnSetTarget(DWORD vID)
{
	guard
    if (m_dwLatestTarget==vID)
        return;
    DWORD dwBak = m_dwLatestTarget;
    GcActor* pTargetActor = FindActor(vID);
    if (pTargetActor==NULL)
    {
        m_dwLatestTarget = 0;
		LOG("SELECTED ACTOR FAILED!");
    }
	else if (pTargetActor == GetPlayerPet()->GetPetActor() && GetPlayerPet()->IsLinkUpPet())
	{
		return;
	}
	else
    {
		/*if (pTargetActor==this)
		{
			m_dwLatestTarget = 0;
			pTargetActor = NULL;
		}else*/
		{
			m_dwLatestTarget = vID;
			// ����HUD��Ѫ����ʾ

			if (pTargetActor->m_pNpc && pTargetActor->m_pNpc->Boss!=0)
			{
				g_layerMain->m_formCreInfo->m_nActorID = pTargetActor->ID();
				g_layerMain->m_formCreInfo->m_nHpTotal = pTargetActor->m_lBossMaxHp;
				g_layerMain->m_formCreInfo->m_nHpRemain = pTargetActor->m_lBossHp;
				g_layerMain->m_formCreInfo->m_strName = pTargetActor->GetName();
				g_layerMain->m_formCreInfo->m_strFaceFile = pTargetActor->m_pNpc->HeadPic;
				g_layerMain->m_formCreInfo->m_iLevel = pTargetActor->m_lLev;
				/***************************gao 2009.11.9*****************************
				���ǰ���Ѿ���ʾ����ͨ�����Ѫ������ô���������أ����Բ�������Ѫ���ص���һ��
				*/
				g_layerMain->m_formCreInfo->SetMonTextGolden();			//��boss��Ѫ�Խ�����ʾ
				if(g_layerMain->m_formMonInfo->IsVisible())
					g_layerMain->m_formMonInfo->Hide();
				g_layerMain->m_formCreInfo->Show();
				/*end*/
			}
			else 
			{
				//lmk ��Ѫ
				/*********************** gao 2009.11.5 ��Ѫ��ʽ�޸� ��ΪUI��Ѫ��ʽ*****************************/
				//if (!pTargetActor->m_pHUD.IsEmpty())
				//{
				//	//pTargetActor->m_pHUD->ShowHPBar(true);// ��Ϊԭ������ʾ��ʽ
				//}
				//SNpc* pNpc = pTargetActor->m_pNpc;
				g_layerMain->m_formMonInfo->m_nActorID = pTargetActor->ID();
				g_layerMain->m_formMonInfo->m_nHpTotal = 100;
				g_layerMain->m_formMonInfo->m_nHpRemain = pTargetActor->m_cHpRate;
				g_layerMain->m_formMonInfo->m_strName = pTargetActor->GetName();
				if ( pTargetActor->NpcType() == ENT_USER )
				{
					//��һ�ú��ѵ�������ʾ
					GetWorld()->ShowFriendUI();
					vector<SCharImage> images;
					if(GetPlayer() == pTargetActor)
					{
						if(!g_TableCharImage.GetCharImage(pTargetActor->m_core.actorID, images))
							return;
					}else
					{
						if (!g_TableCharImage.GetCharImage(pTargetActor->m_actorID, images))
							return;
					}
					std::string strImage = "ui/ui_texture/";
					if ( pTargetActor->m_core.HeadImageID < 0 || pTargetActor->m_core.HeadImageID > images.size())
					{
						pTargetActor->m_core.HeadImageID = 0;
					}
					strImage += images[pTargetActor->m_core.HeadImageID].imageGame;
					strImage += ".dds";
					g_layerMain->m_formMonInfo->m_strFaceFile = strImage;
					g_layerMain->m_formMonInfo->m_iLevel = pTargetActor->m_lLev;
					if(GetPlayer() == pTargetActor)
					{
						g_layerMain->m_formMonInfo->m_iLevel = pTargetActor->m_core.Lev;
					}else
					{
						g_layerMain->m_formMonInfo->m_iLevel = pTargetActor->m_lLev;
					}
				}else if(pTargetActor->NpcType() == ENT_PET )
				{
					g_layerMain->m_formMonInfo->m_strFaceFile = pTargetActor->m_pNpc->HeadPic;
					g_layerMain->m_formMonInfo->m_iLevel = pTargetActor->m_lLev;
				}else
				{
					g_layerMain->m_formMonInfo->m_strFaceFile = pTargetActor->m_pNpc->HeadPic;
					//g_layerMain->m_formMonInfo->m_iLevel = pTargetActor->m_pNpc->Level;
					g_layerMain->m_formMonInfo->m_iLevel = pTargetActor->m_lLev;
				}
				g_layerMain->m_formMonInfo->SetMonTextWhite();
				if(g_layerMain->m_formCreInfo->IsVisible())
					g_layerMain->m_formCreInfo->Hide();
				g_layerMain->m_formMonInfo->Show();
				/*end*/
			}
		}
    }
	if(pTargetActor)
	{
		if(pTargetActor->ID() == g_layerMain->m_formMonInfo->GetActorID())
			pTargetActor->m_Skill.UpdateEffectUIForActor(0);
		else if(pTargetActor->ID() == g_layerMain->m_formCreInfo->GetActorID())
			pTargetActor->m_Skill.UpdateEffectUIForActor(1);
	}
	//else if(GetWorld()->m_pPet && pActor->ID() == GetWorld()->m_pPet->ID())
	//	m_Skill.UpdateEffectUIForActor(2);

	// gao ͬ�� HUD����ʾ 2010.3.5
   // if (m_dwLatestTarget!=dwBak)
   // {
   //     GcActor* pBakActor = FindActor(dwBak);
   //     if (pBakActor)
   //     {
   //         // ȥ��HUDѪ������ʾ
			//if (pBakActor->m_pNpc && pBakActor->m_pNpc->Boss!=0)
			//{
			//	if (!(pTargetActor && pTargetActor->m_pNpc && pTargetActor->m_pNpc->Boss!=0))
			//	{
			//		g_layerMain->m_formCreInfo->m_nActorID = 0;
			//		g_layerMain->m_formCreInfo->Hide();
			//	}
			//}
			//else 
			//{
   //             if (!pBakActor->m_pHUD.IsEmpty() && pBakActor->ID()!=GetWorld()->m_pPlayer->ID())
   //             {
			//	    pBakActor->m_pHUD->ShowHPBar(false);
   //             }
			//}
   //     }
   // }
	unguard
}


void GcPlayer::ReadMatchFromNet(CG_CmdPacket *cmd)
{
    m_matchList.clear();

    short i, sID, sCnt;
    cmd->ReadShort(&sCnt);
    for (i=0; i<sCnt; i++)
    {
        cmd->ReadShort(&sID);
        m_matchList.push_back(sID);
    }
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/


// just for auto			ac.ma


float GcPlayer::_distant(GcActor* sMonster)
{
	guard
	return _distant(sMonster->GetMatrix()->_30, sMonster->GetMatrix()->_31);
	unguard
}

float GcPlayer::_distant(long x, long y)
{
	guard
	return (GetMatrix()->_30 - x) * (GetMatrix()->_30 - x)
		+  (GetMatrix()->_31 - y) * (GetMatrix()->_31 - y);
	unguard
}

//add by yz
bool GcPlayer::_ableUseItem(SItemID& rItemID)
{
	guard
	SItem* pItem = ((CGameClientFrame*)GetApp())->m_pItemManager->GetItem(rItemID); 
	if (NULL == pItem)
		return false;
	if (NULL == pItem->pItemBase)
		return false;

	SCreature tempCore;
	if(GetRideState() == ERS_NONE)
	{
		tempCore = m_core;
	}
	else if(GetRideState() == ERS_ACTOR)
	{
		tempCore = GetWorld()->m_pPet->m_core;
	}
	return (tempCore.Lev >= pItem->pItemBase->requirement[ItemRequirement_Level]);
	unguard
}

//end

void GcPlayer::_auto_eat_red()
{
	guard
	if(!m_bAutoEatRed) return;

// 	DWORD dwNow = rtMilliseconds();
// 	if ((dwNow - m_dwRedBtlCoolDown) >= 3000)
// 		m_dwRedBtlCoolDown = dwNow;
// 	else
// 		return;

	int lackHp;
	bool bLowThenThirty;
	float fRatio;

	fRatio = m_iAutoHp / 100.f;

	SCreature tempCore;
	if(GetRideState() == ERS_NONE)
	{
		lackHp = m_core.GetMaxHp() - m_core.GetHp();
		bLowThenThirty = m_core.GetHp() <= (m_core.GetMaxHp() * fRatio);
		tempCore = m_core;
	}
	else if(GetRideState() == ERS_ACTOR)
	{
		lackHp = GetWorld()->m_pPet->m_core.GetMaxHp() - GetWorld()->m_pPet->m_core.GetHp();
		bLowThenThirty = GetWorld()->m_pPet->m_core.GetHp() <= (GetWorld()->m_pPet->m_core.GetMaxHp() * fRatio);
		tempCore = GetWorld()->m_pPet->m_core;
	}

	SItemID lItem;
	if (bLowThenThirty)
	{
		for (int nPage = 0; nPage < MAX_BAG_PAGE_COUNT; nPage++)
		{
			for (int nI = 0; nI < MAX_BAG_LINE_COUNT; nI++)
			{
				for (int nJ = 0; nJ < MAX_BAG_COLUMN_COUNT; nJ++)
				{
					lItem = mItem.m_Bag.GetItem(nPage, nI, nJ);
					if (GetRideState() == ERS_NONE)
					{
						if (ItemIsHPBottle(lItem))
						{	
							if (_ableUseItem(lItem))
							{
								UseItem(lItem);							
								return;
							}							
						}
					}
					else if (GetRideState() == ERS_ACTOR)
					{
						if (ItemIsPetHpBottle(lItem))
						{
							if (_ableUseItem(lItem))
							{
								UseItem(lItem);							
								return;
							}	
						}
					}					
				}
			}
		}
	}


	/*
	SItemID lItem;
	bool bChooseAuto = m_bChooseAutoHp60 || m_bChooseAutoHp90 || m_bChooseAutoHp30;
	if( (m_bChooseAutoHp60 && ( tempCore.GetHp() < 0.6f * tempCore.GetMaxHp() ))
		|| (m_bChooseAutoHp90 && ( tempCore.GetHp() < 0.9f * tempCore.GetMaxHp() )) 
		|| (m_bChooseAutoHp30 && ( tempCore.GetHp() < 0.3f * tempCore.GetMaxHp() )))
	{
		lItem = ChooseRedInBag(lackHp,bLowThenThirty);
	}else
		if(tempCore.GetHp()< fRatio * tempCore.GetMaxHp() && !bChooseAuto)
		{
			lItem = ChooseRedInBag(lackHp,bLowThenThirty);
		}else
			return;

	if (lItem.id == 0)
		return;
	*/

	//RtCoreLog().Info("Before use red bottle \nlackhp:%d, itemID:%d, curHP:%d, MaxHP:%d\n",
	//	lackHp, lItemID, m_pPlayer->m_core.GetHp(), m_pPlayer->m_core.GetMaxHp());
	//UseItem(lItem);
	unguard
}

//change by yz : �����жϽ�ɫ�ȼ��ܷ�ʹ��ĳһҩƿ
SItemID GcPlayer::ChooseRedInBag(int lackHp,bool bLowThenThirty)
{	
	guard
	SItemID lItem;

	/*if (lackHp >= 300)
	{
		lItem = FindLargeRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindMiddleRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindSmallRedbottleInBag();
	}
	else if (lackHp >=200)
	{
		lItem = FindLargeRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindMiddleRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindSmallRedbottleInBag();
	}
	else 
	{
		lItem = FindSmallRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindMiddleRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindLargeRedbottleInBag();
	}

	if((lItem.id == 0) && bLowThenThirty)
	{
		lItem = FindLargeRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindMiddleRedbottleInBag();
		if (lItem.id == 0)
			lItem = FindSmallRedbottleInBag();
	}*/

	if (bLowThenThirty)
	{
		if (lackHp >= 300)
		{
			lItem = FindLargeRedbottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindMiddleRedbottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindSmallRedbottleInBag();
		}
		else if (lackHp >=200)
		{
			lItem = FindLargeRedbottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindMiddleRedbottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindSmallRedbottleInBag();
		}
		else 
		{
			lItem = FindSmallRedbottleInBag();
			if (lItem.id == 0 )
				lItem = FindMiddleRedbottleInBag();
			if (lItem.id == 0 )
				lItem = FindLargeRedbottleInBag();
			if (!_ableUseItem(lItem))
				lItem.id = 0;
		}
	}

	return lItem;
	unguard
}

//change by yz : �����жϽ�ɫ�ȼ��ܷ�ʹ��ĳһҩƿ
void GcPlayer::_auto_eat_blue()
{
	guard
	if(!m_bAutoEatBlue) return;
// 	DWORD dwNow = rtMilliseconds();
// 	if ((dwNow - m_dwBlueBtlCoolDown) >= 3000)
// 		m_dwBlueBtlCoolDown = dwNow;
// 	else
// 		return;

	float fRatio = m_iAutoMp / 100.f;

	//int lackMp = m_core.GetMaxMp() - m_core.GetMp();
	bool bLowThenThirty;
	if(GetRideState() == ERS_NONE)
	{
		bLowThenThirty = m_core.GetMp() <= (m_core.GetMaxMp() * fRatio);
	}
	else if(GetRideState() == ERS_ACTOR)
	{
		bLowThenThirty = GetWorld()->m_pPet->m_core.GetMp() <= (GetWorld()->m_pPet->m_core.GetMaxMp() * fRatio);
	}
	SItemID lItem;
	if (bLowThenThirty)
	{
		for (int nPage = 0; nPage < MAX_BAG_PAGE_COUNT; nPage++)
		{
			for (int nI = 0; nI < MAX_BAG_LINE_COUNT; nI++)
			{
				for (int nJ = 0; nJ < MAX_BAG_COLUMN_COUNT; nJ++)
				{
					lItem = mItem.m_Bag.GetItem(nPage, nI, nJ);
					if (GetRideState() == ERS_NONE)
					{
						if (ItemIsMPBottle(lItem))
						{	
							if (_ableUseItem(lItem))
							{
								UseItem(lItem);							
								return;
							}	
						}
					}
					else if (GetRideState() == ERS_ACTOR)
					{
						if (ItemIsPetMpBottle(lItem))
						{
							if (_ableUseItem(lItem))
							{
								UseItem(lItem);							
								return;
							}	
						}
					}	
				}
			}
		}
	}

	/*if (lackMp >= 350)
	{
		lItem = FindLargeBluebottleInBag();
		if (lItem.id == 0)
			lItem = FindMiddleBluebottleInBag();
		if (lItem.id == 0)
			lItem = FindSmallBluebottleInBag();
	}
	else if (lackMp >=250)
	{
		lItem = FindMiddleBluebottleInBag();
		if (lItem.id == 0)
			lItem = FindSmallBluebottleInBag();
	}
	else if (lackMp >= 150)
	{
		lItem = FindSmallBluebottleInBag();
	}

	if((lItem.id == 0) && bLowThenThirty)
	{
		lItem = FindLargeBluebottleInBag();
		if (lItem.id == 0)
			lItem = FindMiddleBluebottleInBag();
		if (lItem.id == 0)
			lItem = FindSmallBluebottleInBag();
	}*/	

	/*if(bLowThenThirty)
	{
		if (lackMp >= 350)
		{
			lItem = FindLargeBluebottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindMiddleBluebottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindSmallBluebottleInBag();
		}
		else if (lackMp >=250)
		{
			lItem = FindMiddleBluebottleInBag();
			if (lItem.id == 0 || (!_ableUseItem(lItem)))
				lItem = FindSmallBluebottleInBag();
		}
		else if (lackMp >= 150)
		{
			lItem = FindSmallBluebottleInBag();
		}
	}*/

	/*if (lItem.id == 0 || (!_ableUseItem(lItem)))
		return;*/

	//RtCoreLog().Info("Before use Blue bottle \nlackMp:%d, itemID:%d, curMp:%d, MaxMp:%d\n",
	//	lackMp, lItemID, m_pPlayer->m_core.GetMp(), m_pPlayer->m_core.GetMaxMp());
	/*if (bLowThenThirty)  //�����ҩ�ж�  heten
	{
		UseItem(lItem);
	}*/
	
	unguard
}

void GcPlayer::SetStickyPosition()
{
	guard
	m_iStickyX = GetMatrix()->_30;
	m_iStickyY = GetMatrix()->_31;
	unguard
}
 
//change by yz 2010-7-2:�Զ��������������ļ�Ϊ20%���һ�ʱ�����ѡ��
void GcPlayer::SetAutoEatBlue(bool autoeatblue)
{
	m_bAutoEatBlue = autoeatblue;
	if (autoeatblue)
	{
		if (g_layerMain->m_btnAutoHp30->GetChecked())
			m_iAutoMp = 30;
		else if (g_layerMain->m_btnAutoHp60->GetChecked())
			m_iAutoMp = 60;
		else if (g_layerMain->m_btnAutoHp90->GetChecked())
			m_iAutoMp = 90;
	}
	else
	{
		const char* strAutoMp = GetGameRule()->GetEntry("GameLevelRule","AutoMp");
		if(strAutoMp == NULL)
		{
			RtCoreLog().Info("[CItemManager::Init] Invalid GameRule Init File\n");
			m_iAutoMp = 20;
		}
		else
			m_iAutoMp = atoi(strAutoMp);
	}
}
//end yz


void GcPlayer::_auto_battle()			//		ac.ma
{
	guard
	if (!m_bAutoBattle) 
		return;

	if (1 == m_cShapeshift)  //����ʱ��ͣ�Զ�ս��
	{
		return;
	}

	m_iLastX = GetMatrix()->_30;
	m_iLastY = GetMatrix()->_31;
	GcSynchroActor *lastmonster = GetWorld()->m_ActorManager.FindActive(m_pLastTargetID);
	
	if ((m_pCurCmd && (m_pCurCmd->eCmd==ACMD_MOVETO || m_pCurCmd->eCmd==ACMD_FOLLOW))
		|| !m_pCurCmd)
	{
		if (_distant(m_iLastX,m_iLastY)<=2)
		{
			++m_iAddSettleTime;
		}
		if (m_iAddSettleTime>=10)
		{
			if (m_isGoBack)
			{
				PushCallStack(csn_GcActor_AddCommandMoveTo);
				GetWorld()->m_pPlayer->AddCommandMoveTo(m_iStickyX + 3,m_iStickyY + 3,0,-1.0f,0);
				PopCallStack();
				m_iAddSettleTime = 0;
				return;
			}
			m_curMonster = lastmonster;
			m_pLastTargetID = 0;
			m_iAddSettleTime = 0;
			lastmonster = 0;
			ClearCommand();
		}
	}
	else
	{
		m_iAddSettleTime = 0;
	}
	
	//���������Զ�ս���ľ��볬��2����Ļ
	if(_distant(m_iStickyX,m_iStickyY)>800000)	
	{
		PushCallStack(csn_GcActor_AddCommandMoveTo);
		GetWorld()->m_pPlayer->AddCommandMoveTo(m_iStickyX,m_iStickyY,0,-1.0f,0);
		PopCallStack();
		RtCoreLog().Info("�����Զ�ս���һ��ķ�Χ�����عһ��㣺%d,%d\n",m_iStickyX/15, m_iStickyY/15);
		m_isGoBack = true;
		return;
	}
	else
	{
		if (m_isGoBack)//���عһ���
		{
			if (_distant(m_iStickyX,m_iStickyY)<=200)
			{
				m_pLastTargetID = 0;
				m_isGoBack = false;
			}
		}
		else//�Զ�Ѱ��
		{
			if (lastmonster && _distant(lastmonster) > 700000)
			{
				m_pLastTargetID = 0;
				return;
			}

			//change by yz  2010-6-8
			
			static SSkill* spSkill = NULL;  //��¼���һ���ͷŵļ���
			static DWORD sdwSkillRTime = 0; //��¼���һ�μ����ͷ�ʱ��

			static float fDMin = 999999999;  //��������Ĺֵľ���

			if (NULL == lastmonster)  //û�й֣�ѡ�����Լ������Monster��Ϊ��������
			{		
				fDMin = 999999999;

				GcSynchroActor* pIMon = NULL;

				std::unordered_map<DWORD, GcSynchroActor *>::iterator iter;
				for(iter = GetWorld()->m_ActorManager.m_ActorMap.begin(); iter != GetWorld()->m_ActorManager.m_ActorMap.end() ; ++iter)
				{
					pIMon = iter->second;

					if (!pIMon)
						continue;
					if (!pIMon->m_pNpc)
						continue;
					if (pIMon->m_pNpc->Type != 1)
						continue;
					if (!pIMon->IsActive())
						continue;
#ifdef KICK_CHEAT
					if (pIMon->ID() % 21 == 0)
					{
						continue;
					}
#endif
					if (m_curMonster && m_curMonster == pIMon)
					{
						m_curMonster = 0;
						continue;
					}

					if(_distant(pIMon)<fDMin && pIMon->NpcType() == ENT_MONSTER) 
					{
						fDMin = _distant(pIMon);
						lastmonster = pIMon;
					}						
				}
				if (NULL == lastmonster)
					return;
				m_pLastTargetID = lastmonster->ID();
			}
			else 
			{
				if (lastmonster->m_cIsDeadNet)
					return;
			}

			//�й֣������������ж��ͷż���			
			SSkill* pAttr = GetWorld()->GetActiveSkill();
			if (NULL != pAttr) 
			{
				if (!pAttr->bABad || pAttr->IsAddHpSkill)  //���漼�ܲ��ܶԹ��ͷ�
					pAttr = NULL;
			}
			bool notEnoughMp = (NULL == pAttr) ? false : (m_core.GetMp() < pAttr->iMP);
			
			if (notEnoughMp || NULL == pAttr)  //����������û�м��ܣ���ͨ����
			{
				spSkill = NULL; 
				sdwSkillRTime = 0;

				_commonAttack(lastmonster->ID());
			}
			else if (!notEnoughMp && NULL != pAttr)	//����
			{
				if (NULL != spSkill)
				{
					if ((rtGetMilliseconds() - sdwSkillRTime) > (spSkill->iRRTime + 250))  //�����ͷ���ϣ������ͷż���
					{
						spSkill = pAttr;
						sdwSkillRTime = rtGetMilliseconds();
						_skillAttack(lastmonster, pAttr);
					}
					else   //����û�лָ�
					{
						_commonAttack(lastmonster->ID());
					}			
				}
				else
				{
					spSkill = pAttr;
					sdwSkillRTime = rtGetMilliseconds();
					_skillAttack(lastmonster, pAttr);
				}
			}

			//end yz 2010-6-8
		}
	}
	unguard
}

//add by yz 2010-6-30
void GcPlayer::_commonAttack(DWORD dwTargetID)
{
	int iRand = rtRandom() & 0x7fff;
	int iValue = (((iRand * 8561849L + 586468622L) >> 16) & 0x7fff);
	int iSum = (((iRand^dwTargetID)<<16)&0xFFFF0000) | iValue;

	GetWorld()->m_pPlayer->OnSetTarget(dwTargetID);
	PushCallStack(csn_GcActor_AttackSomebody);
	GetWorld()->m_pPlayer->AttackSomebody(0, dwTargetID, iSum, -1.f);
	PopCallStack();
}

void GcPlayer::_skillAttack(GcSynchroActor *pTarget, const SSkill* pSkill)
{
	if (NULL == pTarget || NULL == pSkill)
		return;
	
	if (pSkill->dwRTarget & SKILL_TARGET_POSITION)
	{
		int iSum = rtRandom();
		float fPos[3];
		pTarget->GetMatrix()->GetRow(3, fPos);
		GetWorld()->m_pPlayer->UsePositionSkill(0, pSkill->wID, fPos[0], fPos[1], fPos[2], iSum);
	}
	else
	{
		PushCallStack(csn_GcActor_UseTargetSkill);
		GetWorld()->m_pPlayer->OnSetTarget(pTarget->ID());
		GetWorld()->m_pPlayer->UseTargetSkill(0, pSkill->wID, pTarget->ID(), 1401);
		PopCallStack();
	}
	
}
//end yz


void GcPlayer::_auto_pick_item()
{
	guard
	if (!m_bAutoPick) return;

// 	DWORD dwNow = rtMilliseconds();
// 	if ((dwNow - m_dwPickItemCoolDown) >= 500)
// 		m_dwPickItemCoolDown = dwNow;
// 	else
// 		return;

	//RtCoreLog().Info("��ʼʰȡ���ߡ�\n");

	//if(m_bWhitePick) RtCoreLog().Info("ʰȡ��ɫ, ");
	//if(m_bGreenPick) RtCoreLog().Info("��ɫ, ");
	//if(m_bBluePick) RtCoreLog().Info("��ɫ, ");
	//if(m_bGoldPick) RtCoreLog().Info("��ɫ\n");

	GetWorld()->m_synchroItems.FindMapItems();
	unguard
}

void GcPlayer::_auto_sell_item()
{
	guard
	//�ж��Ƿ��Ա
	if(!mItem.m_Bag.CheckIsHuiyuan(10)) return;

	bool bl = m_bWhiteAutoSell || m_bGreenAutoSell || m_bBlueAutoSell || m_bGoldAutoSell;
	if(!bl) return;

	//���ɳ��۵�Array
	SDWORDArray idArray;
	idArray.array.clear();

	int iPage = mItem.m_Bag.GetPageCount();
	int iLine = mItem.m_Bag.GetLineCount();
	int iColumn = mItem.m_Bag.GetColCount();

	int iItemID;
	for (int p=0; p<iPage; p++)
		for (int i=0; i<iLine; i++)
			for (int j=0; j<iColumn; j++)
			{
				SItemID item = mItem.m_Bag.GetItem(p, i, j);
				if (!ItemID_IsValid(item)) continue;
				if (!ItemCanNpcTrade(item.type)) continue;
				if (!ItemIsWeapon(item) && !ItemIsArmor(item) && !ItemIsShipin(item)) continue;

				//				RtCoreLog().Info("Item.CLock=%d\n",item.cLock);			//û��.cLock����	ac.ma
				if(item.cLock != 0) continue; //�Ƿ����

				iItemID = (item.type&ITEM_MASK_INDEX) >> 8;
				if(iItemID == 430 || iItemID == 431 || iItemID == 432 || iItemID == 433 || iItemID == 422 || iItemID == 423) continue;
				if(iItemID == 795 || iItemID == 796 || iItemID == 4401) continue;

				if(item.color == ItemColor_White && m_bWhiteAutoSell)
					idArray.array.push_back(item.id);
				if(item.color == ItemColor_Green && m_bGreenAutoSell)
					idArray.array.push_back(item.id);
				if(item.color == ItemColor_Blue  && m_bBlueAutoSell)
					idArray.array.push_back(item.id);
				if(item.color == ItemColor_Golden&& m_bGoldAutoSell)
					idArray.array.push_back(item.id);
			}
			//��ʼ���۵���
			if(!idArray.array.empty() && mItem.m_Bag.IsFull())    //���������ж�     Heten
				CItemNetHelper::NetSend_c2r_item_autosell(idArray);
	unguard
}


void GcPlayer::UseItem(SItemID lItem, int iBagLine, int iBagColumn, int iPage)
{
	guard
	if (lItem.id <= 0) return;
	
	if (iBagLine == 0 || iBagColumn == 0)
	{
		DWORD dwNowTime = rtGetMilliseconds();

		SYaoPing *pYaoPingClass = (SYaoPing *)(GetItemMgr()->GetItem(lItem.type));
		if (!pYaoPingClass)		return;
		if ( ItemIsHPBottle(lItem) &&  (dwNowTime - m_dwHPOperateTime < pYaoPingClass->nTime + 250))
		{
			return;
		}
		else if ( ItemIsMPBottle(lItem) && (dwNowTime - m_dwMPOperateTime < pYaoPingClass->nTime + 250))
		{
			return;
		}
		//change by yz 2010-6-30:������ҩƿ��ʹ��
		else if ( ItemIsPetHpBottle(lItem) && (dwNowTime - m_dwHPPetOperateTime < pYaoPingClass->nTime + 250) )
			return;
		else if ( ItemIsPetMpBottle(lItem) && (dwNowTime - m_dwMPPetOperateTime < pYaoPingClass->nTime + 250) )
			return;
		//end yz

		if (mItem.m_Bag.Find(lItem, &iPage, &iBagLine, &iBagColumn))
		{
			if ( ItemIsHPBottle(lItem) )
			{
				m_dwHPOperateTime = rtGetMilliseconds();
			}
			else if ( ItemIsMPBottle(lItem) )
			{
				m_dwMPOperateTime = rtGetMilliseconds();
			}
			//change by yz 2010-6-30:������ҩƿ��ʹ��
			else if (ItemIsPetHpBottle(lItem))
			{
				m_dwHPPetOperateTime = rtGetMilliseconds();
			}
			else if (ItemIsPetMpBottle(lItem))
			{
				m_dwMPPetOperateTime = rtGetMilliseconds();
			}
			//end yz
			else if ( ItemIsWeapon(lItem) || ItemIsArmor(lItem) || ItemIsShipin(lItem) )
			{
				if (lItem.curWear <= 0)
				{
					PlayerFailOperate(R(MSG_ITEM_ITEMQUALITYLOW));
					return;
				}
			}
			//change by yz: ��ӦCD�¼�
			/*CItemNetHelper::NetSend_c2r_player_use_item(lItem.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, iPage, iBagLine, iBagColumn);*/
			if (g_layerMain)
			{
				if (g_layerMain->m_fromPlayerItems)
				{
					g_layerMain->m_fromPlayerItems->UseBagItem(iPage, iBagLine, iBagColumn);
				}
			}
			//end yz
		}
	}
	unguard
}

SItemID GcPlayer::_find_item_in_bag_by_types(std::vector<DWORD>& typeArray)
{
	guard
	SItemID pItemId;
	int nCount = 0, nPage = 0, nI = 0, nJ = 0;
	for (std::vector<DWORD>::iterator it = typeArray.begin();
		it != typeArray.end(); ++it)
	{
		if (mItem.m_Bag.FindType(*it, ItemColor_All, &nCount, &nPage, &nI, &nJ))
		{
			pItemId = mItem.m_Bag.GetItem(nPage, nI, nJ);
			if (pItemId.id != 0)
			{
				return pItemId;
			}
		}
	}
	return pItemId;
	unguard
}

void GcPlayer::AddCommandMoveTo(float fX, float fY, float fZ, float fTime, float fDistance) {
    // �������Ƿ�����ץȡ����
    if (m_dwCatchPet) {
        // �������ץȡ�������ץȡ��־������ֹͣץȡ��������Ϣ
        m_dwCatchPet = 0;
        CPetNetHelper::NetSend_c2r_catch_pet_stop();
        // �Ƴ���ǰ����
        RemoveCurCommand();
    }

    // ����Ƿ����ڽ��о�������
    if (g_layerMain->m_formChar->m_bIsRifine) {
        // ������������
        g_layerMain->m_formChar->OnFinishRifineTrump(false);
        // ���;�����������Ϣ
        SendTrumpRifine(0, true);
        // �Ƴ���ǰ����
        RemoveCurCommand();
    }

    // ��ȡĿ��λ���ڵ����е�����
    const RtSceneBlockTerrain::STileAttr* pAttr = g_pScene->GetTerrainAttr(fX / 20, fY / 20);

    // ����������Դ������������ߣ��ҵ�ǰ����״̬�����ƶ�
    if (pAttr && pAttr->bWalk && !GetWorld()->CanMove()) {
        // ��������ƶ�������ʾ����ʧ����Ϣ
        PlayerFailOperate(R(MSG_CANNOT_MOVE));
        return;
    }

    // ���û�����ƶ�������Ӻ���
    GcActor::AddCommandMoveTo(fX, fY, fZ, fTime, fDistance);
}


bool GcPlayer::OnGetPlayerIsMoving()
{
	guard;
	if(m_pCurCmd && m_pCurCmd->eCmd == ACMD_MOVETO)
		return true;
	else if(m_pCurCmd)
	{
		std::list<SCmd>::iterator it = m_listCmd.begin();
		for(it = m_listCmd.begin(); it != m_listCmd.end();)
		{
			if((*it).eCmd == ACMD_MOVETO)
			{
				it = m_listCmd.erase(it);
			}else
			{
				it++;
			}
		}
		return false;
	}else
		return false;
	unguard;
}

void GcPlayer::Run(float fSecond)
{

	// ս�������л�����
	if ( g_layerMain->m_formRebirth->IsVisible() && m_bIsDead == 0 )
	{
		g_layerMain->m_formRebirth->Hide();
	}
	static bool playCombatMusic = false;
	static long	switchMusicTime = 0;
	if ( m_bIsCombat && !playCombatMusic )
	{
		playCombatMusic = true;
		switchMusicTime = 0;
		RtsSceneBlockRtb* pBlockMap = (m_iCurrentBlockX<0)?(NULL):(g_pScene->GetBlockMap(m_iCurrentBlockX-g_pScene->GetCenterX(), m_iCurrentBlockY-g_pScene->GetCenterY()));
		// ����ڵ�ͼ�У�����ս������
		if (pBlockMap)
		{
			std::string szBlockName(pBlockMap->m_szFileName);
			SSceneInfo* pSceneInfo = g_TableScene.FindScene(szBlockName);
			if (pSceneInfo)
			{
				//g_pBackMusic->Stop();
				if (pSceneInfo->szSceneCombatMusic.c_str() && pSceneInfo->szSceneCombatMusic.c_str()[0])
				{
					if (strcmp(g_pMusicThread->m_szMusicName,pSceneInfo->szSceneCombatMusic.c_str()) != 0)
					{
						g_pMusicThread->Play(pSceneInfo->szSceneCombatMusic.c_str(), true);
					}
				}
			}
		}
		g_pScene->ShowSceneFlagTexture("enterAttack.dds",false);	// ����ս����־
	}else if ( !m_bIsCombat && playCombatMusic )
	{
		switchMusicTime = rtGetMilliseconds() + 20 * 1000;
		playCombatMusic = false;
		g_pScene->ShowSceneFlagTexture("leaveAttack.dds",false);	// ����ս����־
		g_pMusicThread->Stop();
		// gaoy ���������ս��ʱȥ��Ŀ��UI�ϵ���ʾ
		if(OnGetTarget())
		{
			GetPlayer()->OnSetTarget(NULL);
			if (g_layerMain->m_formGuide2->IsVisible())
			{
				g_layerMain->m_formGuide2->Hide();
			}
			g_layerMain->m_formMonInfo->Hide();
			g_layerMain->m_formCreInfo->Hide();
		}
	}
	if ( switchMusicTime != 0 )
	{
		if ( switchMusicTime < rtGetMilliseconds() )
		{
			RtsSceneBlockRtb* pBlockMap = (m_iCurrentBlockX<0)?(NULL):(g_pScene->GetBlockMap(m_iCurrentBlockX-g_pScene->GetCenterX(), m_iCurrentBlockY-g_pScene->GetCenterY()));
			// ����ڵ�ͼ�У����ű�������
			if (pBlockMap)
			{
				//g_pBackMusic->Stop();
				if (pBlockMap->GetBackMusicFileName() && pBlockMap->GetBackMusicFileName()[0])
				{
					if (strcmp(g_pMusicThread->m_szMusicName,pBlockMap->GetBackMusicFileName()) != 0)
					{
						g_pMusicThread->Play(pBlockMap->GetBackMusicFileName(), false, 30);
					}
				}
			}
			switchMusicTime = 0;
		}
	}

	if ( m_bIsCollect )
	{
		if ( m_lCollectTime )
		{
			g_layerMain->m_formCollectBar->SetCollectTime(m_lCollectTime);
			m_lCollectTime = 0;
			g_layerMain->m_formCollectBar->Show();
			//g_layerMain->m_formChant->Show();
		}
		if (g_layerMain->m_formCollectBar->IsVisible())
		{
			g_layerMain->m_formCollectBar->OnRun();
		}
	}

	if (g_layerMain && g_layerMain->m_formTrump && g_layerMain->m_formTrump->IsRifining())
	{
		if (ItemID_IsValid(m_RifineTrump) || g_layerMain->m_formTrump->m_iRifine == PNEUMA_TRUMP)
		{
			g_layerMain->m_formTrump->Tick();
		}
	}


	if (m_bIntonate)
	{
		float ftmp = float( rtGetMilliseconds() - m_dwIntonateTime );
		if (ftmp >= m_iNeedIntonateTime)
		{
			m_bIntonate = false;
			m_dwIntonateTime = 0;
			CommandFinished();
			/* gao 2009.12.21
			������ɣ�������������
			*/
			g_layerMain->m_formChant->Hide();
			/* end */
			AddCommand(m_SCmd);
		}else
		{
			mBaseActor.PlayPose(GcBaseActor::POSE_CHANT, true);
			/* gao 2009.12.21
			���������ĸ���
			*/
			float fPercent = ftmp /(float)m_iNeedIntonateTime;
			g_layerMain->m_formChant->Refresh(fPercent);
		}
		if (GetPlayer() == this)
		{
		}else{
			mBaseActor.PlayPose(GcBaseActor::POSE_CHANT, true);
		}
	}//else if(g_layerMain && g_layerMain->m_formChant->IsVisible())
	//g_layerMain->m_formChant->Hide();
	GcActor::Run(fSecond);
}

void GcPlayer::DoCommand(SCmd* pCmd)
{
	if (m_dwCatchPet)
	{
		GcActor *pPet = FindAllActor(m_dwCatchPet);
		if (pPet)
			pPet->mBaseActor.StopEffect("catchpetEffect");

		m_dwCatchPet = 0;
		CPetNetHelper::NetSend_c2r_catch_pet_stop();
		RemoveCurCommand();
	}

	if (g_layerMain->m_formChar->m_bIsRifine)
	{
		g_layerMain->m_formChar->OnFinishRifineTrump(false);
		SendTrumpRifine(0,true);
		RemoveCurCommand();
	}

	GcActor::DoCommand(pCmd);
}

int GcPlayer::GetMoveCmdNum()
{
	int cmdNum  = 0;
	std::list<SCmd>::iterator it;
	for (it=m_listCmd.begin(); it!=m_listCmd.end(); it++)
	{
		if ((*it).eCmd == ACMD_MOVETO)
		{
			cmdNum ++;
		}
	}
	return cmdNum;
}

bool GcPlayer::ProcessKey(STaskKey &key)
{
	char FailStr[128];
	if(key.key == "fbprocess")
	{
		int process = atoi(key.param[0].c_str());
		if(process > m_nFBprocess)
			return false;
		return true;
	}

	if(key.key == "item")
	{
		long itemId;
		itemId = atol(key.param[0].c_str());
		if(itemId == 0) return true;

		long itemNum;
		itemNum = atol(key.param[1].c_str());
		if(itemNum == 0) return true;

		EItemColor color = ItemColor_White;
		if(key.param[2] == "all")
			color = ItemColor_All;
		else if(key.param[2] == "white")
			color = ItemColor_White;
		else if(key.param[2] == "green")
			color = ItemColor_Green;
		else if(key.param[2] == "blue")
			color = ItemColor_Blue;

		int count;
		mItem.m_Bag.FindType(itemId, color, &count);

		if (count < itemNum)
		{
			SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(itemId);
			if (NULL != pItemClass)
			{
				rt2_snprintf(FailStr,128,R(MSG_NEED_KEY_ITEM),itemNum,pItemClass->name);
				PlayerFailOperate(FailStr);
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	if(key.key == "pet")
	{
		long petId;
		petId = atol(key.param[0].c_str());
		if(petId == 0) return true;

		long petNum;
		petNum = atol(key.param[1].c_str());
		if(petNum == 0) return true;

		int count;
		mItem.m_Bag.FindType(petId, ItemColor_All, &count);

		if (count < petNum)
		{
			return false;
		}
	}
	if(key.key == "sreward")
	{
		return true;
	}
	else if(key.key == "beginmove")
	{
		return true;
	}
	else if(key.key == "killself")
	{
		return true;
	}
	else if(key.key == "allcredit")
	{
		return true;
	}
	else if(key.key == "summon")
	{
		return true;
	}
	else if(key.key == "gold")
	{
		int num;
		num = atol(key.param[0].c_str());

		if (mItem.GetMoney()< num)
			return false;
		else
			return true;
	}else if(key.key == "idle")
	{
		return true;
	}
	else if(key.key == "team")
	{
		if(GetWorld()->m_Team.mData.Empty())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if(key.key == "setlev")
	{
		return true;
	}
	else if(key.key == "exp")
	{
		return true;
	}
	else if(key.key == "task")
	{
		int num;
		num = atol(key.param[0].c_str());

		int AwardClass = 0;
		if(!key.param[1].empty())
			AwardClass = atol(key.param[1].c_str());

		return m_task.IsTaskAccept(num);
	}
	else if(key.key == "lvlexp")
	{
		return true;
	}
	else if(key.key == "lvlexpsm")
	{
		return true;
	}
	else if(key.key == "ritem")
	{
		int id[3];
		for(int i=0; i<3; i++)
			id[i] = atol(key.param[2+i].c_str());

		int num;
		num = atol(key.param[0].c_str());

		EItemColor color = ItemColor_All;
		if(key.param[1] == "all")
			color = ItemColor_All;
		else if(key.param[1] == "white")
			color = ItemColor_White;
		else if(key.param[1] == "green")
			color = ItemColor_Green;
		else if(key.param[1] == "blue")
			color = ItemColor_Blue;

		for(int i=0; i<3; i++)
		{
			int count;
			mItem.m_Bag.FindType(id[i], color, &count);

			if (count >= num)
			{
				return true;
			}
		}
		return false;
	}
	else if(key.key == "mlev")
	{
		int lev;
		lev = atol(key.param[0].c_str());
		if(lev == -1) return true;
		if(m_core.MetierLev == lev) return true;
	}
	else if (key.key == "unionscore") // ��������
	{
		return true;
	}
	else if (key.key == "unionlev") // �����ȼ�
	{
	}
	else if(key.key == "ifree")
	{
		long num;
		num = atol(key.param[0].c_str());
		if(num == 0) return true;

		return (mItem.m_Bag.FreeSpace() >= num);
	}
	else if(key.key == "fbitem")
	{
		return true;
	}
	else if (key.key == "itemex")
	{
		return true;
	}
	else if(key.key == "skill")
	{
		long skillID;
		skillID = atol(key.param[0].c_str());
		if(skillID==0) return false;

		return (Skill()->FindSkill(skillID)!=NULL);
	}
	else if(key.key == "effect")
	{
		long effectID;
		effectID = atol(key.param[0].c_str());
		if(effectID==0) return false;

		return (Skill()->FindEffect(effectID)!=NULL);
	}
	else if(key.key == "wenreward")
	{
		return true;
	}
	else if(key.key == "gzaddzg")
	{
		return true;
	}
	else if(key.key == "gzaddjf")
	{
		return true;
	}
	else if(key.key == "gzaddgod")
	{
		return true;
	}
	else if(key.key == "circle_item")
	{
		int id = atol(key.param[0].c_str());
		int num = atol(key.param[1].c_str());
		STaskInfo* info = m_task.FindTask(id);

		if(info && info->Desc && info->Desc->circleTask!=0)
		{
			int count;
			mItem.m_Bag.FindType(id, ItemColor_White, &count);

			if(count < num)
				return false;

			return (m_task.m_isActive==1);
		}else
		{
			return false;
		}
	}
	else if(key.key == "circle_hold")
	{
		return true;
	}
	else if(key.key == "circle_monster")
	{
		int id=0, num=0;
		id = atol(key.param[0].c_str());
		num = atol(key.param[1].c_str());

		STaskInfo* info = m_task.FindTask(id);
		if(info && info->Desc && info->Desc->circleTask!=0)
		{
			if( (info->m_currMonsterNum >= num) )
			{
				return (m_task.m_isActive==1);
			}else
			{
				return false;
			}
		}else
		{
			return false;
		}
	}
	else if(key.key == "circle")
	{
		int id=0;
		id = atol(key.param[0].c_str());		

		return (m_task.m_currCircle==0);
	}
	else if(key.key == "circle_exp")
	{
		return true;
	}
	else if(key.key == "circle_task")
	{
		return true;
	}
	else if(key.key == "on_arrive")
	{
		return true;
	}
	PlayerFailOperate("����δ����");
	return false;
}

bool GcPlayer::CheckCanStartTrigger( DWORD triggerID, bool checkRemove/* = false*/ )
{
	if (this != GetPlayer()) return true;

	if (!GetWorld()->GetDungeon())
	{
		ERR("�ͻ��˸�����Ϣ������\n");
		return false;
	}

	STrigger *pTrigger = GetWorld()->GetDungeon()->GetTrigger(triggerID);

	if (!pTrigger)
	{
		ERR("������Ϣ������\n");
		return false;
	}

	bool failed = false;

	if (checkRemove)
	{
		for(size_t i=0; i<pTrigger->startKeyRemove.size(); i++)
		{
			if(!ProcessKey(pTrigger->startKeyRemove[i]))
				failed = true;
			if(failed)
				return false;
		}
	}
	else
	{
		for(size_t i=0; i<pTrigger->startKey.size(); i++)
		{
			if(!ProcessKey(pTrigger->startKey[i]))
				failed = true;
			if(failed)
				return false;
		}
	}
	return !failed;
}

bool GcPlayer::SetModel(short Id, short vHeadModelID)
{
	bool bSetModel = GcActor::SetModel(Id,vHeadModelID);

	if (g_layerMain && g_layerMain->m_formChar->IsVisible() && g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::EQUIP)
	{
		g_layerMain->m_formChar->ReCreate3DViewGraph();
	}
	return bSetModel;
}
