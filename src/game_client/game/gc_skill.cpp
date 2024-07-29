
#include "gc_include.h"
#include "ui_form_playeritems.h"
#include "ui_form_item_update.h"
#include "ui_form_trade.h"
#include "game_assistant.h"
#include "ui_form_skill.h"
#include "ui_form_skill_study.h"
#include "string_mapping.h"
#include "UIForm_Gem.h"
#include "Timer_Hint.h"
#include "UIForm_Char.h"
CSkillInstance* CSkillInstance::GenerateOne(SSkill* pAttr)
{
    return RT_NEW GcSkill(pAttr->wID);
}

CActorSkill* FindActorSkill(void* pActor, DWORD dwCreatureOID)
{
    GcActor* pTarget = FindActor(dwCreatureOID);
    if (pTarget)
    {
        return &(pTarget->m_Skill);
    }
    return NULL;
}

/*----------------------------------------------------------------------------
- GcSkill.
----------------------------------------------------------------------------*/

GcSkill::GcSkill(unsigned short wSkillID) : CSkillInstance(wSkillID)
{
    m_bLastState = false;
    m_pButton = 0;
	m_curMaskValue = 0.f;
	m_bCanUse= true;
}

GcSkill::~GcSkill()
{
}

void GcSkill::OnSkillRun(unsigned long dwMillisecond,CActorSkill* pReleaser)
{
    bool bState = (m_dwResumeTime!=0);
    if (bState!=m_bLastState)
    {
        m_bLastState = bState;
        if (m_pButton)
        {
            m_pButton->EnableAlphaEffect(bState);
        }
    }
    if (bState)
    {
        if (m_pButton)
        {
			m_pButton->SetMaskColor(RtwPixel(0xb5000000));    
			m_pButton->SetMaskValue(GetResumeRate()*100 * 0.01);  //技能CD蒙板
        }
    }else
	{
		if (m_pButton)
		{
			m_curMaskValue = pReleaser->GetPublicCDRate(m_pAttr)*100 * 0.01;

			if(m_curMaskValue > 0)
			{
				m_pButton->SetMaskColor(RtwPixel(0xb5000000));   
				m_pButton->SetMaskValue(m_curMaskValue);
			}else if((long)m_pAttr->iMP > ((GcActorSkill*)pReleaser)->GetParentEx()->m_core.GetMp())
			{
				m_pButton->SetMaskColor(RtwPixel(0xb0ff0000));   
				m_pButton->SetMaskValue(1.f);
			}else if(m_pAttr->ConsumeAnger && (m_pAttr->ConsumeAnger > ((GcActorSkill*)pReleaser)->GetParentEx()->m_core.GetEnP() || (m_pAttr->wClass == 13 && ((GcActorSkill*)pReleaser)->GetParentEx()->IsShapeshifted())))
			{
				m_pButton->SetMaskColor(RtwPixel(0xb0ff0000));   
				m_pButton->SetMaskValue(1.f);
			}else if(!m_bCanUse)
			{
				m_pButton->SetMaskColor(RtwPixel(0xb5000000));   
				m_pButton->SetMaskValue(1.f);
			}else
			{
				m_pButton->SetMaskColor(RtwPixel(0xb5000000));   
				m_pButton->SetMaskValue(m_curMaskValue);
			}
		}
	}
	
}

bool GcSkill::OnCanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget, const float* pPos, bool bIgnoreDistance)
{

	// 技能需要MP
	if (pReleaser->GetMP()<m_pAttr->iMP)
	{
		m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
		return false;
	}

	//Tianh  10.03.12
	if (m_pAttr->dwType == 1) //技能类型1是法宝技能
	{
		// 技能需要怒气（能量）值
		if (pReleaser->GetEnP()<m_pAttr->ConsumeAnger)
		{
			m_pErrInfo = R(SMSG_EP_NOT_ENOUGH);
			return false;
		}
	}
	
    if (m_pAttr->bAlwaysUsable==0 && !((GcActorSkill*)pReleaser)->GetParentEx()->IsAbiUseSkill())
    {
        m_pErrInfo = R(SMSG_SKILL_CANNOTUSE);
        return false;
    }

    // 技能是否还没有恢复
    if (m_dwResumeTime)
    {
        m_pErrInfo = R(SMSG_SKILL_RESUMEING);
        return false;
    }

    if (m_dwUserTime)
    {
        m_pErrInfo = R(SMSG_SKILL_RESUMEING); // 还没有使用结束
        return false;
    }

	//Tianh 如果公共CD未冷却  不可以使用技能
	if (m_pAttr->wClass == 1 || m_pAttr->wClass == 2 || m_pAttr->wClass == 3 || m_pAttr->wClass == 4 || m_pAttr->wClass == 5
		||m_pAttr->wClass == 6 || m_pAttr->wClass == 7 || m_pAttr->wClass == 8 ||m_pAttr->wClass == 9
		||m_pAttr->wClass == 10||m_pAttr->wClass == 11 ||m_pAttr->wClass == 12)
	{

		if (pReleaser->m_SchoolPublicCDTime) // 允许半秒的buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}
	else if (m_pAttr->wClass == 13)
	{
		if (pReleaser->m_WapPublicCDTime) // 允许半秒的buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}
	//else if (m_pAttr->wClass == 0)
	//{
	//	if (pReleaser->m_PropPublicCDTime>500) // 允许半秒的buffer
	//	{
	//		m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
	//		return false;
	//	}
	//}
	else if (m_pAttr->wClass == 21)
	{
		if (pReleaser->m_beastPublicCDTime) // 允许半秒的buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}

    if (m_pAttr->wClass!=17 && ((GcActorSkill*)pReleaser)->GetParentEx()->GetRideState()!=GcActor::ERS_NONE)
    {
        //m_pErrInfo = "骑兽状态不能使用技能，除了骑兽术"; // 骑兽状态不能使用技能
		m_pErrInfo = R(MSG_SKILL_CAN_NOT_USED_ON_PET); // 骑兽状态不能使用技能
		return false;
    }

	if (pReleaser->GetParent() == GetWorld()->m_pPlayer)
		return AutoEatMPBottle(pReleaser);
	else 
		return true;
}

void GcSkill::SetUIButton(RtwButton* pButton)
{
    m_pButton = pButton;
}

bool GcSkill::AutoEatMPBottle(CActorSkill* pReleaser)
{
	guard;

	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	// MP不够时自动喝药瓶
	if (pReleaser->GetMP() < m_pAttr->iMP)
	{
		DWORD itemID = 0, itemType = 0;
		if (g_layerMain)
		{
			for (int p=0; p<=1; p++)
			{
				for (int i=0; i<UI_HOT_KEY_COUNT; i++)
				{
					if ( g_layerMain->m_fromPlayerItems->GetQuickStartItemData(p, i, itemID, itemType) )
					{
						if (ItemIsMPBottle(itemType))
							break;
						else 
							itemID = 0;
					}
				}
				if (itemID!=0)
					break;
			}
		}

		SItemID item;
		int page, i, j;
		if (itemID!=0)
		{
			if ( pCon->m_Bag.FindID(itemID, &page, &i, &j) )
			{
				item = pCon->m_Bag.GetItem(page, i, j);
			}
		}

		if (ItemID_IsValid(item))
		{
			if (g_layerMain && g_layerMain->m_formItemUpdate->IsVisible())
			{
				m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
				return false;
			}
			else if (g_layerMain && g_layerMain->m_formGem->IsVisible())
			{
				m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
				return false;
			}
			else if (g_layerMain && g_layerMain->m_formTrade->IsShowing())
			{
				m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
				return false;
			}
			else
			{
				if ( !pCon->OperateItem_CheckUIContent(item, IO_Eat))
					return false;
				else 
				{
					CItemNetHelper::NetSend_c2r_player_use_item(item.id, ITEM_CONTAINER_PLAYER_COMMON_BAG, page, i, j);
					return true;
				}
			}
		}
		else 
		{
			m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
			return false;
		}
	}
	else 
	{
		return true;
	}

	unguard;
}

/*----------------------------------------------------------------------------
- GcActorSkill.
----------------------------------------------------------------------------*/

GcActorSkill::GcActorSkill()
{
	InitializeCriticalSection(&m_cs);
}

GcActorSkill::~GcActorSkill()
{
	DeleteCriticalSection(&m_cs);
}

void GcActorSkill::UpdateUI()
{
    if(g_layerMain->m_fromSkill->IsShowing())
    {
        g_layerMain->m_fromSkill->Refresh();
    }
    if(g_layerMain->m_fromSkillStudy->IsShowing())
    {
        g_layerMain->m_fromSkillStudy->Refresh();
	}
}

void GcActorSkill::UpdateEffectUI()
{
    // Update UI
    //MSG("---------------------------------------------------------\n");
    int cCnt = 0;
    SSkillEffect* pEffect;
    RtwButton* pButton;
    std::list<SSkillEffect*>::iterator it;
    for (it=m_listSkillEffect.begin(); it!=m_listSkillEffect.end() && cCnt<10; it++)
    {
		//<add by Tianh>//作用目标为1的时候是针对周围的 
		//不针对自己  所以不给自己加载技能图标和作用
		if (1 == (*it)->cTarget)
		{
			continue;
		}
		
        pEffect = *it;
        if (pEffect->szIcon && pEffect->szIcon[0])
        {
            rt2_sprintf(g_strStaticBuffer, "fmBuff.btnbuff%d", cCnt+1);
            pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
            pButton->Show();
            RtwImage* pimg = g_workspace.getImageFactory()->createImage(pEffect->szIcon);
            pButton->SetBackgroundImage(pimg);
            //MSG1("%s\n", pEffect->szIcon);
            pButton->EvHint = RTW_CALLBACK(this, GcActorSkill, OnGetEffectHint);
            pButton->SetUserData(pEffect);
			pButton->SetParam2(this);
			cCnt++;
        }
    }
    for (; cCnt<10; cCnt++)
    {
        rt2_sprintf(g_strStaticBuffer, "fmBuff.btnbuff%d", cCnt+1);
        pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
        pButton->Hide();
    }
}
// 说明 当type 为0时，是普通怪物的BUFF，1为BOSS的BUFF，2为宠物的BUFF
void GcActorSkill::UpdateEffectUIForActor(int type)
{
	//<gao.2010.5.22>
	if(type == 0)// monster buff info
	{
		int cCnt = 0;
		SSkillEffect* pEffect;
		RtwButton* pButton;
		std::list<SSkillEffect*>::iterator it;
		for (it=m_listSkillEffect.begin(); it!=m_listSkillEffect.end() && cCnt<10; it++)
		{
			//<add by Tianh>//作用目标为1的时候是针对周围的 
			//不针对自己  所以不给自己加载技能图标和作用
			if (1 == (*it)->cTarget)
			{
				continue;
			}
			
			pEffect = *it;
			if (pEffect->szIcon && pEffect->szIcon[0])
			{
				rt2_snprintf(g_strStaticBuffer, 2048, "fmmonhp.fmmBuff.btnbuff%d", cCnt+1);
				pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
				pButton->Show();
				RtwImage* pimg = g_workspace.getImageFactory()->createImage(pEffect->szIcon);
				pButton->SetBackgroundImage(pimg);
				//MSG1("%s\n", pEffect->szIcon);
				pButton->EvHint = RTW_CALLBACK(this, GcActorSkill, OnGetEffectHint);
				pButton->SetUserData(pEffect);
				pButton->SetParam2(this);
				cCnt++;
			}
		}
		for (; cCnt<10; cCnt++)
		{
			rt2_snprintf(g_strStaticBuffer, 2048, "fmmonhp.fmmBuff.btnbuff%d", cCnt+1);
			pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
			pButton->Hide();
		}
	}else if(type == 1)// boss buff info
	{
		int cCnt = 0;
		SSkillEffect* pEffect;
		RtwButton* pButton;
		std::list<SSkillEffect*>::iterator it;
		for (it=m_listSkillEffect.begin(); it!=m_listSkillEffect.end() && cCnt<10; it++)
		{
			//<add by Tianh>//作用目标为1的时候是针对周围的 
			//不针对自己  所以不给自己加载技能图标和作用
			if (1 == (*it)->cTarget)
			{
				continue;
			}

			pEffect = *it;
			if (pEffect->szIcon && pEffect->szIcon[0])
			{
				rt2_snprintf(g_strStaticBuffer, 2048, "fmhpcreature.fmbBuff.btnbuff%d", cCnt+1);
				pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
				pButton->Show();
				RtwImage* pimg = g_workspace.getImageFactory()->createImage(pEffect->szIcon);
				pButton->SetBackgroundImage(pimg);
				//MSG1("%s\n", pEffect->szIcon);
				pButton->EvHint = RTW_CALLBACK(this, GcActorSkill, OnGetEffectHint);
				pButton->SetUserData(pEffect);
				pButton->SetParam2(this);
				cCnt++;
			}
		}
		for (; cCnt<10; cCnt++)
		{
			rt2_snprintf(g_strStaticBuffer, 2048, "fmhpcreature.fmbBuff.btnbuff%d", cCnt+1);
			pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
			pButton->Hide();
		}
	}else if(type == 2)// pet buff info
	{
		int cCnt = 0;
		SSkillEffect* pEffect;
		RtwButton* pButton;
		std::list<SSkillEffect*>::iterator it;
		for (it=m_listSkillEffect.begin(); it!=m_listSkillEffect.end() && cCnt<10; it++)
		{
			//<add by Tianh>//作用目标为1的时候是针对周围的 
			//不针对自己  所以不给自己加载技能图标和作用
			if (1 == (*it)->cTarget)
			{
				continue;
			}

			pEffect = *it;
			if (pEffect->szIcon && pEffect->szIcon[0])
			{
				rt2_snprintf(g_strStaticBuffer, 2048, "fmhpc.fmcBuff.btnbuff%d", cCnt+1);
				pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
				pButton->Show();
				RtwImage* pimg = g_workspace.getImageFactory()->createImage(pEffect->szIcon);
				pButton->SetBackgroundImage(pimg);
				//MSG1("%s\n", pEffect->szIcon);
				pButton->EvHint = RTW_CALLBACK(this, GcActorSkill, OnGetEffectHint);
				pButton->SetUserData(pEffect);
				pButton->SetParam2(this);
				cCnt++;
			}
		}
		for (; cCnt<10; cCnt++)
		{
			rt2_snprintf(g_strStaticBuffer, 2048, "fmhpc.fmcBuff.btnbuff%d", cCnt+1);
			pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
			pButton->Hide();
		}
	}
}

void GcActorSkill::ClearEffectUIForActor(int type)
{
	guard;
	if(type == 0)// monster buff info
	{
		int cCnt = 0;
		RtwButton* pButton;
		for (; cCnt<10; cCnt++)
		{
			rt2_snprintf(g_strStaticBuffer, 2048, "fmmonhp.fmmBuff.btnbuff%d", cCnt+1);
			pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
			pButton->Hide();
		}
	}else if(type == 1)// boss buff info
	{
		int cCnt = 0;
		RtwButton* pButton;
		for (; cCnt<10; cCnt++)
		{
			rt2_snprintf(g_strStaticBuffer, 2048, "fmhpcreature.fmbBuff.btnbuff%d", cCnt+1);
			pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
			pButton->Hide();
		}
	}else if(type == 2)// pet buff info
	{
		int cCnt = 0;
		RtwButton* pButton;
		for (; cCnt<10; cCnt++)
		{
			rt2_snprintf(g_strStaticBuffer, 2048, "fmhpc.fmcBuff.btnbuff%d", cCnt+1);
			pButton = LOAD_UI_T(RtwButton, g_strStaticBuffer);
			pButton->Hide();
		}
	}
	unguard;
}

void GcActorSkill::OnAddSkill(CSkillInstance* pSkill)
{
    UpdateUI();
}

void GcActorSkill::OnDeleteSkill(CSkillInstance* pSkill)
{
    UpdateUI();
}

void GcActorSkill::OnAddEffect(CSkillInstance* pEffect)
{
}

void GcActorSkill::OnDeleteEffect(CSkillInstance* pEffect)
{
}

void GcActorSkill::OnStudySkill(SSkill* pSkillAttr, bool bRefresh)
{
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_study_skill);
    pPacket->WriteShort(pSkillAttr->wID);
	char skill[128];
	rt2_sprintf(skill,R(MSG_SKILL_LEARNNEWSKILL),pSkillAttr->szName);
	PlayerFailOperate(skill);
    NetSend(pPacket);
}

/*----------------------------------------------------------------------------
-   class GcSkillCreature.
----------------------------------------------------------------------------*/

short GcActorSkill::GetMaxHP()
{
    return GetParentEx()->m_core.GetMaxHp();
}

float GcActorSkill::GetEnP()
{
	return GetParentEx()->m_core.GetEnP();
}


int GcActorSkill::GetLevel()
{
    return GetParentEx()->m_core.Lev;
}

int GcActorSkill::GetHP()
{
    return GetParentEx()->m_core.GetHp();
}

int GcActorSkill::GetMP()
{
    return GetParentEx()->m_core.GetMp();
}

// 0x0001 单刀  1
// 0x0002 双刀  2
// 0x0004 斧    4
// 0x0008 弓    8
// 0x0010 软鞭  16
// 0x0020 飞轮  32
// 0x0040 法杖  64
// 0x0080 飞剑  128
// 0x0100 宝珠  256
// 0x0200 盾牌  512
// 0x0400 采药  1024
// 0x0800 挖矿  2048
int GcActorSkill::GetWeaponType() // 1鞭 2斧 3弓 4软鞭 5双鞭 6飞轮 7 法杖 8 飞剑 9 宝珠
{
    SItemID id1 = GetParentEx()->mItem.GetEquipItem(CItemContainerBase::WEAPON_1);
    int iType = 0;

    if (ItemIsWeapon_Pestle(id1.type))        iType |= 0x0001;
    else if (ItemIsWeapon_Axe(id1.type))    iType |= 0x0004;
    else if (ItemIsWeapon_Riband(id1.type))    iType |= 0x0008;
    else if (ItemIsWeapon_Hoop(id1.type))   iType |= 0x0010;
    else if (ItemIsWeapon_Wheel(id1.type))    iType |= 0x0020;
    else if (ItemIsWeapon_Wand(id1.type))    iType |= 0x0040;
    else if (ItemIsWeapon_Sword(id1.type))    iType |= 0x0080;
    else if (ItemIsWeapon_Ball(id1.type))    iType |= 0x0100;
    else if (ItemIsHerbalEquip(id1.type))   iType |= 0x0400;
    else if (ItemIsMineEquip(id1.type))     iType |= 0x0800;

    SItemID id2 = GetParentEx()->mItem.GetEquipItem(CItemContainerBase::WEAPON_2);

    if (ItemIsWeapon_Pestle(id2.type))        iType |= 0x0002;
    else if (ItemIsShield(id2.type))        iType |= 0x0200;

    return iType;
}

int GcActorSkill::GetWeaponEquiped()
{
    SItemID id = GetParentEx()->mItem.GetEquipItem(CItemContainerBase::WEAPON_1);
    if (ItemID_IsValid(id))
    {
        return 1;
    }
    return 0;
}

int GcActorSkill::GetTrumpEquiped()
{
	SItemID id = GetParentEx()->mItem.GetEquipItem(CItemContainerBase::TRUMP);
	if (ItemID_IsValid(id))
	{
		return 1;
	}
		return 0;
}

float* GcActorSkill::GetSelfPos()
{
    return (float*)&(GetParentEx()->GetMatrix()->_30);
}

DWORD GcActorSkill::GetSelfID()
{
    return GetParentEx()->ID();
}

short GcActorSkill::GetModelID()
{
    return GetParentEx()->m_core.ModelId;
}

short GcActorSkill::GetCloth()
{
    return GetParentEx()->m_core.Cloth;
}

char  GcActorSkill::GetClothLev()
{
    return GetParentEx()->m_core.ClothLev;
}

short GcActorSkill::GetArmGuard()
{
    return GetParentEx()->m_core.ArmGuard;
}

char  GcActorSkill::GetArmGuardLev()
{
    return GetParentEx()->m_core.ArmGuardLev;
}

short GcActorSkill::GetShoes()
{
    return GetParentEx()->m_core.Shoes;
}

char  GcActorSkill::GetShoesLev()
{
    return GetParentEx()->m_core.ShoesLev;
}

short GcActorSkill::GetWeapon()
{
    return GetParentEx()->m_core.Weapon;
}

char  GcActorSkill::GetWeaponLev()
{
    return GetParentEx()->m_core.WeaponLev;
}

short GcActorSkill::GetMaxMP()
{
    return GetParentEx()->m_core.MaxMp.GetValue();
}

int   GcActorSkill::GetAttack()
{
    return GetParentEx()->m_core.Attack.GetValue();
}

int   GcActorSkill::GetDodge()
{
    return GetParentEx()->m_core.Dodge.GetValue();
}

int   GcActorSkill::GetDamageMin()
{
    return GetParentEx()->m_core.DamageMin.GetValue();
}

int   GcActorSkill::GetDamageMax()
{
    return GetParentEx()->m_core.DamageMax.GetValue();
}

int   GcActorSkill::GetArmor()
{
    return GetParentEx()->m_core.Armor.GetValue();
}

float GcActorSkill::GetDamageRate()
{
    return GetParentEx()->m_core.DamageRate;
}

short GcActorSkill::GetCon()
{
    return GetParentEx()->m_core.Con.GetValue();
}

short GcActorSkill::GetStr()
{
    return GetParentEx()->m_core.Str.GetValue();
}

short GcActorSkill::GetDex()
{
    return GetParentEx()->m_core.Dex.GetValue();
}

short GcActorSkill::GetInt()
{
    return GetParentEx()->m_core.Int.GetValue();
}

short GcActorSkill::GetHit()
{
    return GetParentEx()->m_core.Hit.GetValue();
}

short GcActorSkill::GetAttSpeed()
{
    return GetParentEx()->m_core.AttSpeed.GetValue();
}

char  GcActorSkill::GetMetier()
{
    return GetParentEx()->m_core.Metier;
}

char  GcActorSkill::GetMetierLev()
{
    return GetParentEx()->m_core.MetierLev;
}

char  GcActorSkill::GetFaction()
{
    return GetParentEx()->m_core.Faction;
}

DWORD GcActorSkill::GetExp()
{
    return GetParentEx()->m_core.Exp;
}

long  GcActorSkill::GetAttrPoint()
{
    return GetParentEx()->m_core.AttrPoint;
}

long  GcActorSkill::GetSkillPoint()
{
    return GetParentEx()->m_core.SkillPoint;
}

long  GcActorSkill::GetSkillSoul()
{
	return GetParentEx()->m_core.Soul;
}

short GcActorSkill::GetAbsorbHp()
{
    return GetParentEx()->m_core.AbsorbHp.GetValue();
}

short GcActorSkill::GetAbsorbMp()
{
    return GetParentEx()->m_core.AbsorbMp.GetValue();
}

short GcActorSkill::GetRateGold()
{
    return GetParentEx()->m_core.RateGold.GetValue();
}

short GcActorSkill::GetRateExp()
{
    return GetParentEx()->m_core.RateExp.GetValue();
}

short GcActorSkill::GetCritical()
{
    return GetParentEx()->m_core.CriticalHit.GetValue();
}

// 0自由，1骑兽，2被骑
int GcActorSkill::IsRide()
{
    switch (GetParentEx()->GetRideState())
    {
    case GcActor::ERS_ACTOR: return 1;
    case GcActor::ERS_PET:   return 2;
    }
    return 0;
}

int GcActorSkill::GetSkillSeries()
{
	return GetParentEx()->m_SkillSeries;
}

int GcActorSkill::IsHaveCallNpc()
{
	if (GetParentEx()->m_CallNpcCount)
	{
		return 1;
	}
	
	return 0;
}

int GcActorSkill::IsChangeBody()
{
	if (GetParentEx()->IsShapeshifted()) return 1;
	return 0;
}

/*
void GcActorSkill::FbXiulian()
{
	//GetWorld()->m_pPlayer->MagicWeaponBeginAddEle();
    CHECK(0);
}

void GcActorSkill::FbZhuru()
{
	//GetWorld()->m_pPlayer->MagicWeaponBeginAddMp();
    CHECK(0);
}

void GcActorSkill::UIProduce(short v)
{
	//GetWorld()->m_pPlayer->LivingSkillProduce(v);
    CHECK(0);
}

void GcActorSkill::UISyncretize()
{
	//GetWorld()->m_pPlayer->LivingSkillSyncretize();
    CHECK(0);
}

void GcActorSkill::ItemDecompound()
{
	//GetWorld()->m_pPlayer->LivingSkillItemDecompound();
    CHECK(0);
}
*/

void GcActorSkill::UpdateSkillExp(CG_CmdPacket* pPacket)
{
    short sID, sExp;
    pPacket->ReadShort(&sID);
    pPacket->ReadShort(&sExp);
    CSkillInstance* pSkill = this->FindActiveSkill(sID);
    if (pSkill)
    {
        pSkill->m_wExp = sExp;
        g_layerMain->m_fromSkill->UpdateSkillExp(sID, sExp, pSkill->m_pAttr->iLevelUpExp);
    }
}

void GcActorSkill::NetReadSkill(CG_CmdPacket* pPacket)
{
    ClearSkill(false);

    char i, cSize;
    short wID, wExp;
    READ_char(cSize);
	long dwUserTime,dwResumeTime = 0;
    for (i=0; i<cSize; i++)
    {
        READ_short(wID);
        READ_short(wExp);
		READ_ulong(dwResumeTime);
		READ_ulong(dwUserTime);
		SSkill* pAttr = Skill()->FindSkill(wID);
		SSkillContinueMask ss;
		ss.wSkillClass = pAttr->wClass;
		ss.resumeTime = dwResumeTime;
		ss.useTime = dwUserTime;
		m_listMask.push_back(ss);
        AddSkill(wID, wExp, false);
    }

    READ_char(cSize);
    for (i=0; i<cSize; i++)
    {
        READ_short(wID);
        AddSkill(wID, 0, false);
    }
}

bool GcActorSkill::NetReadEffect(CG_CmdPacket* pPacket)
{
	/* gao 2009.12.31
	读取buff，以及buff的剩余时间
	此处，由set改为map
	*/
	guard;
    map<unsigned short,DWORD> setEffect;
    char c, cCnt;
    short wID;
	DWORD wTime;					// gao buff 时间
    READb_char(cCnt);
    for (c=0; c<cCnt; c++)
    {
        READb_short(wID);
		READb_ulong(wTime);			// gao buff 时间读取
        setEffect.insert(make_pair(wID,wTime));
    }

    return NetReadEffect(setEffect);
	unguard;
	/* end */
}

bool GcActorSkill::NetReadEffect(std::map<unsigned short,DWORD>& setEffect)
{
	/* gao 2009.12.31
	buff重新整理，并将值保存到成员map中
	*/
	guard;
	EnterCriticalSection(&m_cs);
    bool bModify = false;
	std::list<SSkillEffect*> skillEffect;
    SSkillEffect* pEffect;
    GcActor* pActor = GetParentEx();
    map<unsigned short,DWORD>::iterator it;
	// 数据保存
	m_mapEffect = setEffect;
    while (m_listSkillEffect.size())
    {
        it = setEffect.find(m_listSkillEffect.front()->wID);
        if (it!=setEffect.end())
        {
            skillEffect.push_back(m_listSkillEffect.front());
			if (m_listSkillEffect.front() && pActor && 
				m_listSkillEffect.front()->szEffect && 
				strlen(m_listSkillEffect.front()->szEffect) > 0 &&
				!pActor->mBaseActor.FindEffect(m_listSkillEffect.front()->szEffect))
			{
			}
			else
			{
				setEffect.erase(it);
			}
        }else
        {
            bModify = true; // 删除
            pEffect = m_listSkillEffect.front();
            if (pEffect->szEffect && pEffect->szEffect[0])
            {
                pActor->mBaseActor.StopEffect(pEffect->szEffect);
            }
        }
        m_listSkillEffect.pop_front();
    }
    m_listSkillEffect = skillEffect;
    for (it=setEffect.begin(); it!=setEffect.end(); it++)
    {
        pEffect = Skill()->FindEffect((*it).first);
        if (pEffect)
        {
            bModify = true;
			std::list<SSkillEffect*>::iterator it2 = m_listSkillEffect.begin();
			for (;it2!=m_listSkillEffect.end();it2++)
			{
				if ((*it2)->wID == pEffect->wID)
				{
					break;
				}
			}
			if (it2 == m_listSkillEffect.end())
				m_listSkillEffect.push_back(pEffect); // 添加
            if (pEffect->szEffect && pEffect->szEffect[0])
            {
                if (pEffect->szEffLink==0 || pEffect->szEffLink[0]==0)
                {
                    pActor->mBaseActor.PlayEffect(pEffect->szEffect, false, NULL, RtgVertex3(0,0,0), true, 1.0f, "statloop");
                }else
                {
                    pActor->mBaseActor.PlayEffect(pEffect->szEffect, true, pEffect->szEffLink, RtgVertex3(0,0,0), true, 1.0f, "statloop");
                }
            }
        }
    }
	LeaveCriticalSection(&m_cs);
    return bModify;
	unguard;
	/* end */
}

void GcActorSkill::OnGetEffectHint(RtwWidget* sender, void*)
{
	/* gao 2010.1.1
	添加buff的hint提示的剩余时间
	*/
    guard;
    int iLine = 0;
	SSkillEffect* pSEffect = (SSkillEffect*)sender->GetUserData();
	RtwHtmlView* hintWidget= (RtwHtmlView*)GetWorld()->GetHintWidget();
	GcActorSkill* pSkill= (GcActorSkill*)sender->getParam2();
	if(pSEffect&& pSkill)
	{
		std::map<unsigned short,DWORD>::iterator it;
		it = pSkill->m_mapEffect.find(pSEffect->wID);
		if(it != pSkill->m_mapEffect.end())
		{
			char tmpStr[20];
			if((*it).second== (DWORD)-1)
				rt2_snprintf(tmpStr, 20, "无限制");
			else
			{
				DWORD time = (*it).second / 1000;
				rt2_sprintf(tmpStr,"%d天%d小时%d分%d秒", time/86400, (time%86400)/3600, (time%3600)/60, time%60);
			}

			char* pBuf = RT_NEW char[1024];
			rt2_sprintf(pBuf,
				"<html><body>"
				"<p><FONT color=\"#808080\">%s</FONT></p>"
				"<p><FONT color=\"#ff0000\">剩余时间：%s</FONT></p>"
				"<P><FONT color=\"#ffffff\">%s</FONT></P></body></html>",
				pSEffect->szName, tmpStr, pSEffect->szDescription);
			iLine = 1;
			int iLen = strlen(pSEffect->szDescription);
			iLine += (iLen/29)+1;

			g_layerMain->m_TimerHint->m_eNowHintFor = WIDGET_SKILL_BUFF;
			g_layerMain->m_TimerHint->m_wdtHintFor = sender;
			g_layerMain->m_TimerHint->m_pActorSkill = this;
			hintWidget->SetHtmlText(pBuf);
			sender->SetHint(hintWidget);
			AutoResize(hintWidget,1);
		}else
			hintWidget->Hide();
	}
    unguard;
	/* end */
}

/*----------------------------------------------------------------------------
- End.
----------------------------------------------------------------------------*/
