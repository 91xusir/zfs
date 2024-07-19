
#include "region.h"
#include "PlayerLogic.h"

void CSvSkillCreature::SkillWriteDamage(ULONG dwTargetID, bool bMiss, int &phyDmg, int eleDmg[MAX_ELEMENT_ATTR],long dead, long bCriDamage)
{
    if (m_bSkillWriteDamage)
    {
		m_listSkillDamage.push_back(SSkillDamage());
		SSkillDamage* pSkillDamage = &(m_listSkillDamage.back());
        pSkillDamage->dwTargetID = dwTargetID;
        pSkillDamage->bMiss = bMiss;
        if (!bMiss)
        {
           // pSkillDamage->bDead = (dead!=0);
			pSkillDamage->cDead = (char)(dead);
            pSkillDamage->bCriDamage = (bCriDamage!=0);
            pSkillDamage->phyDmg = phyDmg;
            for(int i=0; i<MAX_ELEMENT_ATTR; i++)
            {
                pSkillDamage->eleDmg[i] = eleDmg[i];
            }
        }
		
    }
}

void CSvSkillCreature::SkillWriteDamageBuffer(CG_CmdPacket* pPacket)
{
    long i, lDead, lCriDamage;
    SSkillDamage* pSkillDamage;
    std::list<SSkillDamage>::iterator it;
    for (it=m_listSkillDamage.begin(); it!=m_listSkillDamage.end(); it++)
    {
        pSkillDamage = &(*it);
        pPacket->WriteLong(pSkillDamage->dwTargetID);
		char cIntonate = 0;
		CRegionCreature *pThis =  (CRegionCreature*)(GetParent());
		CRegionCreature *tmp = pThis->m_scene->FindCreature(pSkillDamage->dwTargetID);
        pPacket->WriteByte(pSkillDamage->bMiss?USE_SKILL_MISS:USE_SKILL_OK);
        if (!pSkillDamage->bMiss)
        {
			if(NULL != tmp && NULL != pThis)
			{
				if(tmp->GetPlayerLogic()->IsIntonating())
				{
					if(pThis->GetPlayerLogic()->CanStopSomeOnceIntonating(pThis->GetPlayerLogic()->GetCurUseSkill(),*tmp))
					{
						tmp->GetPlayerLogic()->StopIntonate();
						cIntonate = 1;
					}
					else
					{
						cIntonate = 2;
					}
				}
			}
			pPacket->WriteByte(cIntonate);
            lDead = pSkillDamage->cDead;
            lCriDamage = pSkillDamage->bCriDamage;
            pPacket->SerializeBit(lDead,3);
            pPacket->SerializeBit(lCriDamage,1);
            pPacket->WriteShort(pSkillDamage->phyDmg);
            for(i=0; i<MAX_ELEMENT_ATTR; i++)
            {
                pPacket->WriteShort(pSkillDamage->eleDmg[i]);
            }	
        }
		else
		{
			if(NULL != tmp)
			{
				if(tmp->GetPlayerLogic()->IsIntonating())
				{
					cIntonate = 2;
				}
				pPacket->WriteByte(cIntonate);
			}
		}
		
    }
}

CSkillInstance* CSkillInstance::GenerateOne(SSkill* pSkillAttr)
{
    return RT_NEW CSvSkill(pSkillAttr->wID);
}

CActorSkill* FindActorSkill(void* pActor, DWORD dwCreatureOID)
{
    CRegionCreature* pTarget = ((CRegionCreature*)pActor)->m_scene->FindCreature(dwCreatureOID);
    if (pTarget)
    {
        return &(pTarget->m_Skill);
    }
    return NULL;
}

/*----------------------------------------------------------------------------
-   class CSvSkill.
----------------------------------------------------------------------------*/

CSvSkill::CSvSkill(unsigned short wSkillID) : CSkillInstance(wSkillID)
{
    CHECK(SKILL_ELEMENT_FIRE==ELEMENT_FIRE);
    CHECK(SKILL_ELEMENT_WATER==ELEMENT_WATER);
    CHECK(SKILL_ELEMENT_POISON==ELEMENT_POISON);
    CHECK(SKILL_MAX_ELEMENT_ATTR==MAX_ELEMENT_ATTR);
}

CSvSkill::~CSvSkill()
{
}

void CSvSkill::OnUseBefore(CActorSkill* pReleaser)
{
    // 检查速度
    ((CSvSkillCreature*)pReleaser)->GetParentEx()->CheckAttackSpeed(m_pAttr->iRTime, false);
}

void CSvSkill::OnUseAfter(CActorSkill* pReleaser)
{
    // 检查经验，如果有升级的，就升级
    m_wExp ++;
#ifdef _DEBUG
	// MSG4("技能[%s]Lv%d 经验[%d/%d]\n", m_pAttr->szName, m_pAttr->iLevel, m_wExp, m_pAttr->iLevelUpExp);
#endif
    if (m_pAttr->iLevelUpExp)
    {
        if (m_wExp>=m_pAttr->iLevelUpExp)
        {
            m_wExp = m_pAttr->iLevelUpExp;
            SSkill* pSkill = Skill()->FindNextLevelSkill(m_pAttr->wID);
            if (pSkill)
            {
                if (pReleaser->StudySkill(pSkill->wID, true))
                {
					rt2_sprintf(g_strStaticBuffer, R(MSG_SKILL_UPDATE), pSkill->szName, pSkill->iLevel, pSkill->iMaxLevel);
                    ((CSvSkillCreature*)pReleaser)->GetParentEx()->SendSystemMessage(g_strStaticBuffer);
                }
            }
        }else
        {
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(r2c_update_skill_exp);
            g_sendCmd->WriteShort(m_pAttr->wID);
            g_sendCmd->WriteShort(m_wExp);
            ((CSvSkillCreature*)pReleaser)->GetParentEx()->RecvCmd(g_sendCmd);
        }
    }
}

bool CSvSkill::OnCanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget,const float* pPos, bool bIgnoreDistance)
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


	if (!m_pAttr->bAType && pTarget)
	{
		if (!((CSvSkillCreature*)pTarget)->CheckCanUse(m_pAttr, pReleaser))
		{
			// 该目标不能被攻击
			m_pErrInfo = R(SMSG_SKILL_CANNOTATTACK);
			return false;
		}
	}

    if (m_pAttr->bAlwaysUsable==0 && !((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.IsAbiUseSkill())
    {
        m_pErrInfo = R(SMSG_SKILL_CANNOTUSE);
        return false;
    }

    // 技能是否还没有恢复
    if (m_dwResumeTime>500) // 允许半秒的buffer
    {
        m_pErrInfo = R(SMSG_SKILL_RESUMEING);
        return false;
    }

    if (m_dwUserTime>500) // 允许半秒的buffer
    {
        m_pErrInfo = R(SMSG_SKILL_RESUMEING); // 还没有使用结束
        return false;
    }

	//Tianh 如果公共CD未冷却  不可以使用技能
	if (m_pAttr->wClass == 1 || m_pAttr->wClass == 2 || m_pAttr->wClass == 3 || m_pAttr->wClass == 4 || m_pAttr->wClass == 5
		||m_pAttr->wClass == 6 || m_pAttr->wClass == 7 || m_pAttr->wClass == 8 ||m_pAttr->wClass == 9
		||m_pAttr->wClass == 10||m_pAttr->wClass == 11 ||m_pAttr->wClass == 12)
	{

		if (pReleaser->m_SchoolPublicCDTime>500) // 允许半秒的buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}
	else if (m_pAttr->wClass == 13)
	{
		if (pReleaser->m_WapPublicCDTime>500) // 允许半秒的buffer
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
		if (pReleaser->m_beastPublicCDTime>500) // 允许半秒的buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}

    if (!((CSvSkillCreature*)pReleaser)->GetParentEx()->CheckAttackSpeed(m_pAttr->iRTime, true))
    {
        m_pErrInfo = R(MSG_ATTACK_SPEED_ERR);
        return false;
    }
	return true;
}

/*----------------------------------------------------------------------------
-   class CSvSkillCreature.
----------------------------------------------------------------------------*/

short CSvSkillCreature::GetMaxHP()
{
    return GetParentEx()->m_core.GetMaxHp();
}

float CSvSkillCreature::GetEnP()
{
	return GetParentEx()->m_core.GetEnP();
}

int CSvSkillCreature::GetLevel()
{
    return GetParentEx()->m_core.Lev;
}

int CSvSkillCreature::GetHP()
{
    return GetParentEx()->m_core.GetHp();
}

int CSvSkillCreature::GetMP()
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
int CSvSkillCreature::GetWeaponType()
{
    if (!GetParentEx()->m_pItemContainer)
        return 0;

    SItemID id1 = GetParentEx()->m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1);
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

	SItemID id2 = GetParentEx()->m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_2);

    if (ItemIsWeapon_Pestle(id2.type))        iType |= 0x0002;
    else if (ItemIsShield(id2.type))        iType |= 0x0200;

    return iType;
}

int CSvSkillCreature::GetWeaponEquiped()
{
    if (!GetParentEx()->m_pItemContainer)
        return 0;

    SItemID id = GetParentEx()->m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1);
    if (ItemID_IsValid(id))
    {
        return 1;
    }
    return 0;
}

int CSvSkillCreature::GetTrumpEquiped()
{
    if (!GetParentEx()->m_pItemContainer)
        return 0;

    SItemID id = GetParentEx()->m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP);
    if (ItemID_IsValid(id))
    {
        return 1;
    }
    return 0;
}

float* CSvSkillCreature::GetSelfPos()
{
    m_pPos[0] = GetParentEx()->m_pos[0];
    m_pPos[1] = GetParentEx()->m_pos[1];
    m_pPos[2] = GetParentEx()->m_pos[2];
    return m_pPos;
}

DWORD CSvSkillCreature::GetSelfID()
{
    return GetParentEx()->m_oId;
}
short CSvSkillCreature::GetModelID()
{
    return GetParentEx()->m_core.ModelId;
}

short CSvSkillCreature::GetCloth()
{
    return GetParentEx()->m_core.Cloth;
}

char  CSvSkillCreature::GetClothLev()
{
    return GetParentEx()->m_core.ClothLev;
}

short CSvSkillCreature::GetArmGuard()
{
    return GetParentEx()->m_core.ArmGuard;
}

char  CSvSkillCreature::GetArmGuardLev()
{
    return GetParentEx()->m_core.ArmGuardLev;
}

short CSvSkillCreature::GetShoes()
{
    return GetParentEx()->m_core.Shoes;
}

char  CSvSkillCreature::GetShoesLev()
{
    return GetParentEx()->m_core.ShoesLev;
}

short CSvSkillCreature::GetWeapon()
{
    return GetParentEx()->m_core.Weapon;
}

char  CSvSkillCreature::GetWeaponLev()
{
    return GetParentEx()->m_core.WeaponLev;
}

short CSvSkillCreature::GetMaxMP()
{
    return GetParentEx()->m_core.MaxMp.GetValue();
}

/*Tianh 技能命中*/
int   CSvSkillCreature::GetAttack()
{
    return GetParentEx()->m_core.Attack.GetValue();
}

int   CSvSkillCreature::GetDodge()
{
    return GetParentEx()->m_core.Dodge.GetValue();
}

int   CSvSkillCreature::GetDamageMin()
{
    return GetParentEx()->m_core.GetDamageMin();
}

int   CSvSkillCreature::GetDamageMax()
{
    return GetParentEx()->m_core.GetDamageMax();
}

int   CSvSkillCreature::GetArmor()
{
    return GetParentEx()->m_core.Armor.GetValue();
}

float CSvSkillCreature::GetDamageRate()
{
    return GetParentEx()->m_core.DamageRate;
}

short CSvSkillCreature::GetCon()
{
    return GetParentEx()->m_core.Con.GetValue();
}

short CSvSkillCreature::GetStr()
{
    return GetParentEx()->m_core.Str.GetValue();
}

short CSvSkillCreature::GetDex()
{
    return GetParentEx()->m_core.Dex.GetValue();
}

short CSvSkillCreature::GetInt()
{
    return GetParentEx()->m_core.Int.GetValue();
}

short CSvSkillCreature::GetHit()
{
    return GetParentEx()->m_core.Hit.GetValue();
}

short CSvSkillCreature::GetAttSpeed()
{
    return GetParentEx()->m_core.AttSpeed.GetValue();
}

char  CSvSkillCreature::GetMetier()
{
    return GetParentEx()->m_core.Metier;
}

char  CSvSkillCreature::GetMetierLev()
{
    return GetParentEx()->m_core.MetierLev;
}

char  CSvSkillCreature::GetFaction()
{
    return GetParentEx()->m_core.Faction;
}

DWORD CSvSkillCreature::GetExp()
{
    return GetParentEx()->m_core.Exp;
}

long  CSvSkillCreature::GetAttrPoint()
{
    return GetParentEx()->m_core.AttrPoint;
}

long  CSvSkillCreature::GetSkillPoint()
{
    return GetParentEx()->m_core.SkillPoint;
}

long  CSvSkillCreature::GetSkillSoul()
{
	return GetParentEx()->m_core.Soul;
}

short CSvSkillCreature::GetAbsorbHp()
{
    return GetParentEx()->m_core.AbsorbHp.GetValue();
}

short CSvSkillCreature::GetAbsorbMp()
{
    return GetParentEx()->m_core.AbsorbMp.GetValue();
}

short CSvSkillCreature::GetRateGold()
{
    return GetParentEx()->m_core.RateGold.GetValue();
}

short CSvSkillCreature::GetRateExp()
{
    return GetParentEx()->m_core.RateExp.GetValue();
}

short CSvSkillCreature::GetCritical()
{
    return GetParentEx()->m_core.CriticalHit.GetValue();
}

void CSvSkillCreature::AddHP(int iV,int IsDiS)
{
	GetParentEx()->AddHp(iV,IsDiS);
}

void CSvSkillCreature::AddMP(int iV)
{
    GetParentEx()->m_core.AddMp(iV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_SIMPLE);
}

void CSvSkillCreature::AddEnP(int iV)
{
    GetParentEx()->m_core.AddEnp(iV);
}

void CSvSkillCreature::AddMaxHP(int iV)
{
    int iHP = GetParentEx()->m_core.MaxHp.Temp + iV;
    if (iHP>30000)
    {
        iHP = 30000;
    }else if (iHP<0)
    {
        iHP = 0;
    }
    GetParentEx()->m_core.MaxHp.Temp = iHP;
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_SIMPLE);
}

void CSvSkillCreature::AddMaxMP(int iV)
{
    int iMP = GetParentEx()->m_core.MaxMp.Temp + iV;
    if (iMP>30000)
    {
        iMP = 30000;
    }else if (iMP<0)
    {
        iMP = 0;
    }
    GetParentEx()->m_core.MaxMp.Temp = iMP;
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_SIMPLE);
}

void CSvSkillCreature::AddAddedStr(short sV)
{
    GetParentEx()->m_core.AddAddedStr(sV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
}

void CSvSkillCreature::AddAddedCon(short sV)
{
    GetParentEx()->m_core.AddAddedCon(sV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
}

void CSvSkillCreature::AddAddedDex(short sV)
{
    GetParentEx()->m_core.AddAddedDex(sV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
}

void CSvSkillCreature::AddAddedInt(short sV)
{
    GetParentEx()->m_core.AddAddedInt(sV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
}

void CSvSkillCreature::AddAddedHit(short sV)
{
    GetParentEx()->m_core.AddAddedHit(sV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_ALL);
}

void CSvSkillCreature::AddResist(short sV)
{
    GetParentEx()->m_core.AddResist(sV);
    //GetParentEx()->UpdateToClient(NET_CRE_UPDATE_ATTR_ELEMENT_ARMOR);
}

void CSvSkillCreature::AddPhyAr(short sV)         // 15 添加物理防御
{
    GetParentEx()->m_core.Armor.Temp += sV;
}

void CSvSkillCreature::AddPhyAb(short sV)         // 16 添加物理吸收
{
    GetParentEx()->m_core.DamageAbsorb.Temp += sV;
}

void CSvSkillCreature::AddAbsorbHP(short sV)      // 17 添加吸血
{
    GetParentEx()->m_core.AbsorbHp.Temp += sV;
}

void CSvSkillCreature::AddAbsorbMP(short sV)      // 18 添加吸魔
{
    GetParentEx()->m_core.AbsorbMp.Temp += sV;
}

void CSvSkillCreature::AddDamageMin(short sV,int _DamageMode)     // 19 添加最小伤害
{
	int DamageMode = _DamageMode;
	if (DamageMode == SKILL_BADAMAGE_ONE)
	{
		 GetParentEx()->m_core.DamageMin.Temp  += sV;
	}
	else if(DamageMode == SKILL_BADAMAGE_TWO)
	{
		 GetParentEx()->m_core.DamageMin.Temp  = sV;
	}
}

void CSvSkillCreature::AddDamageMax(short sV,int _DamageMode)     // 20 添加最大伤害
{
	int DamageMode = _DamageMode;
	if (DamageMode == SKILL_BADAMAGE_ONE)
	{
		GetParentEx()->m_core.DamageMax.Temp += sV;
	}
	else if(DamageMode == SKILL_BADAMAGE_TWO)
	{
		GetParentEx()->m_core.DamageMax.Temp = sV;
	}
}

void CSvSkillCreature::AddSpeedAttack(short sV)   // 21 添加攻击速度
{
    GetParentEx()->m_core.AttSpeed.Temp += sV;
}

void CSvSkillCreature::AddSpeedMove(short sV)     // 22 添加移动速度
{
    GetParentEx()->m_core.MovSpeed.Temp += sV;
}

void CSvSkillCreature::AddRateGold(short sV)     // 23 添加获得金钱倍率
{
    GetParentEx()->m_core.RateGold.Temp += sV;
}

void CSvSkillCreature::AddRateExp(short sV)      // 24 添加获得经验倍率
{
    GetParentEx()->m_core.RateExp.Temp += sV;
}


//Tianh 伤害方式2 
//火伤害=技能火伤害*（1+装备附加火伤害%）。
//水伤害=技能水伤害*（1+装备附加水伤害%）。
//毒伤害=技能毒伤害*（1+装备附加毒伤害%）。

void CSvSkillCreature::AddWaterDm(short sV,int _DamageMode)     // 25 添加水伤害
{
	int DamageMode = _DamageMode;
	if (DamageMode == SKILL_BADAMAGE_ONE)
	{
		 GetParentEx()->m_core.ElementDamage[ELEMENT_WATER].Temp += sV;
	}
	else if(DamageMode == SKILL_BADAMAGE_TWO)
	{
		GetParentEx()->m_core.ElementDamage[ELEMENT_WATER].Temp = sV;
	}
}

void CSvSkillCreature::AddFireDm(short sV,int _DamageMode)      // 26 添加火伤害
{
	int DamageMode = _DamageMode;
	if (DamageMode == SKILL_BADAMAGE_ONE)
	{
		GetParentEx()->m_core.ElementDamage[ELEMENT_FIRE].Temp += sV;
	}
	else if(DamageMode == SKILL_BADAMAGE_TWO)
	{
		GetParentEx()->m_core.ElementDamage[ELEMENT_FIRE].Temp = sV;
	}
}

void CSvSkillCreature::AddPoisonDm(short sV,int _DamageMode)    // 27 添加毒伤害
{
	int DamageMode = _DamageMode;
	if (DamageMode == SKILL_BADAMAGE_ONE)
	{
		GetParentEx()->m_core.ElementDamage[ELEMENT_POISON].Temp += sV;
	}
	else if(DamageMode == SKILL_BADAMAGE_TWO)
	{
		GetParentEx()->m_core.ElementDamage[ELEMENT_POISON].Temp = sV;
	}
}

void CSvSkillCreature::AddWaterAr(short sV)       // 28 添加水防御
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_WATER].Temp += sV;
}

void CSvSkillCreature::AddFireAr(short sV)        // 29 添加火防御
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_FIRE].Temp += sV;
}

void CSvSkillCreature::AddPoisonAr(short sV)      // 30 添加毒防御
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_POISON].Temp += sV;
}

void CSvSkillCreature::AddWaterAb(short sV)       // 31 添加水吸收
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_WATER].Temp += sV;
}

void CSvSkillCreature::AddFireAb(short sV)        // 32 添加火吸收
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_FIRE].Temp += sV;
}

void CSvSkillCreature::AddPoisonAb(short sV)      // 33 添加毒吸收
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_POISON].Temp += sV;
}

void CSvSkillCreature::AddAttackRate(short sV)   // 34 添加命中
{
    GetParentEx()->m_core.Attack.Temp += sV;
}

void CSvSkillCreature::AddDodge(short sV)        // 35 添加躲避
{
    GetParentEx()->m_core.Dodge.Temp += sV;
}

void CSvSkillCreature::AddAttackRange(short sV)  // 36 添加攻击距离
{
    GetParentEx()->m_core.AttackRange.Temp += sV;
}

void CSvSkillCreature::AddCritical(short sV)
{
    GetParentEx()->m_core.CriticalHit.Temp += sV;
}

void CSvSkillCreature::AddHPR(float fV)
{
    GetParentEx()->AddHp(GetParentEx()->m_core.GetHp()*fV,true);
}

void CSvSkillCreature::AddMPR(float fV)
{
    GetParentEx()->m_core.AddMp(GetParentEx()->m_core.GetMp()*fV);
}

void CSvSkillCreature::HPMPConversion(float aV, float bV)
{
	float AV = 0;

	
	if (GetParentEx()->m_core.GetHp()*aV < 0)
	{
		AV = GetParentEx()->m_core.GetHp()*(-aV);
	}
	if (GetParentEx()->m_core.GetHp()*aV > 0)
	{
		AV = GetParentEx()->m_core.GetHp()*aV;
	}

	GetParentEx()->AddHp(GetParentEx()->m_core.GetHp()*aV);

	GetParentEx()->m_core.AddMp(AV * bV);
}

void CSvSkillCreature::AddAddedConR(float fV)
{
    GetParentEx()->m_core.AddAddedConR(fV);
}

void CSvSkillCreature::AddAddedStrR(float fV)
{
    GetParentEx()->m_core.AddAddedStrR(fV);
}

void CSvSkillCreature::AddAddedDexR(float fV)
{
    GetParentEx()->m_core.AddAddedDexR(fV);
}

void CSvSkillCreature::AddAddedIntR(float fV)
{
    GetParentEx()->m_core.AddAddedIntR(fV);
}

void CSvSkillCreature::AddAddedHitR(float fV)
{
    GetParentEx()->m_core.AddAddedHitR(fV);
}

void CSvSkillCreature::AddResistR(float fV)
{
    GetParentEx()->m_core.AddResistR(fV);
}

void CSvSkillCreature::AddMaxHPR(float fV)
{
    GetParentEx()->m_core.MaxHp.Rate += fV;
}

void CSvSkillCreature::AddMaxMPR(float fV)
{
    GetParentEx()->m_core.MaxMp.Rate += fV;
}

void CSvSkillCreature::AddPhyArR(float fV)  // 15 添加物理防御
{
    GetParentEx()->m_core.Armor.Rate += fV;
}

void CSvSkillCreature::AddPhyAbR(float fV)  // 16 添加物理吸收
{
    GetParentEx()->m_core.DamageAbsorb.Rate += fV;
}

void CSvSkillCreature::AddAbsorbHPR(float fV)  // 17 添加吸血
{
    GetParentEx()->m_core.AbsorbHp.Temp += fV;
}

void CSvSkillCreature::AddAbsorbMPR(float fV)  // 18 添加吸魔
{
    GetParentEx()->m_core.AbsorbMp.Temp += fV;
}

void CSvSkillCreature::AddDamageMinR(float fV)  // 19 添加最小伤害
{
    GetParentEx()->m_core.DamageMin.Rate += fV;
}

void CSvSkillCreature::AddDamageMaxR(float fV)  // 20 添加最大伤害
{
    GetParentEx()->m_core.DamageMax.Rate += fV;
}

void CSvSkillCreature::AddSpeedAttackR(float fV)  // 21 添加攻击速度
{
    GetParentEx()->m_core.AttSpeed.Rate += fV;
}

void CSvSkillCreature::AddSpeedMoveR(float fV)  // 22 添加移动速度
{
    GetParentEx()->m_core.MovSpeed.Rate += fV;
}

void CSvSkillCreature::AddRateGoldR(float fV)  // 23 添加获得金钱倍率
{
    GetParentEx()->m_core.RateGold.Rate += fV;
}

void CSvSkillCreature::AddRateExpR(float fV)  // 24 添加获得经验倍率
{
    GetParentEx()->m_core.RateExp.Rate += fV;
}

void CSvSkillCreature::AddWaterDmR(float fV)  // 25 添加水伤害
{
    GetParentEx()->m_core.ElementDamage[ELEMENT_WATER].Rate += fV;
}

void CSvSkillCreature::AddFireDmR(float fV)  // 26 添加火伤害
{
    GetParentEx()->m_core.ElementDamage[ELEMENT_FIRE].Rate += fV;
}

void CSvSkillCreature::AddPoisonDmR(float fV)  // 27 添加毒伤害
{
    GetParentEx()->m_core.ElementDamage[ELEMENT_POISON].Rate += fV;
}

void CSvSkillCreature::AddWaterArR(float fV)  // 28 添加水防御
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_WATER].Rate += fV;
}

void CSvSkillCreature::AddFireArR(float fV)  // 29 添加火防御
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_FIRE].Rate += fV;
}

void CSvSkillCreature::AddPoisonArR(float fV)  // 30 添加毒防御
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_POISON].Rate += fV;
}

void CSvSkillCreature::AddWaterAbR(float fV)  // 31 添加水吸收
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_WATER].Rate += fV;
}

void CSvSkillCreature::AddFireAbR(float fV)  // 32 添加火吸收
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_FIRE].Rate += fV;
}

void CSvSkillCreature::AddPoisonAbR(float fV)  // 33 添加毒吸收
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_POISON].Rate += fV;
}

void CSvSkillCreature::AddAttackRateR(float fV) // 34 添加命中
{
    GetParentEx()->m_core.Attack.Rate += fV;
}

void CSvSkillCreature::AddDodgeR(float fV) // 35 添加躲避
{
    GetParentEx()->m_core.Dodge.Rate += fV;
}

void CSvSkillCreature::AddAttackRangeR(float fV) // 36 添加攻击距离
{
    GetParentEx()->m_core.AttackRange.Rate += fV;
}

void CSvSkillCreature::AddCriticalR(float fV) // 36 添加爆击
{
    GetParentEx()->m_core.CriticalHit.Rate += fV;
}

void CSvSkillCreature::AddResilience(float fV) //88 抗必杀
{
	GetParentEx()->m_core.ReCriticalHit.Temp += fV;
}

void CSvSkillCreature::Rebirth(int iHPrate,int iMPrate) //91 复活
{
	/*GetParentEx()->Rebirth(4,iHPrate,iMPrate);*/

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_rebirth_user);
	g_sendCmd->WriteLong(GetParentEx()->GetObjectId());
	g_sendCmd->WriteLong(iHPrate);
	g_sendCmd->WriteLong(iMPrate);
	GetParentEx()->RecvCmd(g_sendCmd);
}

void CSvSkillCreature::ExtractHpR(int iHPrate,int maxHP) //百分比抽取目标血量，但最多不能多于一个固定值
{
	long Hp = GetParentEx()->m_core.GetHp() * iHPrate/100;
	if (Hp > maxHP)
	{
		Hp = maxHP;
	}
	GetParentEx()->AddHp(-Hp,1);
}

void CSvSkillCreature::OrdinaryAddEffect(int probability,int EffectID)
{
	GetParentEx()->m_EffectProbability += probability;
	GetParentEx()->m_PTEffectID += EffectID;
}

void CSvSkillCreature::BossConjure(int NpcId,int Num)
{
	GetParentEx()->CreateAllUnderlingNpc(NpcId,BOSS_CALL,Num);
}

void CSvSkillCreature::SeriesDamage(int Series,int damageR)//增加技能系列伤害
{
	GetParentEx()->m_SkillSeries += Series;          //要增加伤害的技能系列
	GetParentEx()->m_SkilldamageR += damageR;        //要增加伤害
	GetParentEx()->AddSkillSeries();
}

//Tianh  
void CSvSkillCreature::AddAbsorptionMaxDamage(float sV) //吸收伤害最大值
{
	if (sV > 0)
	{
		GetParentEx()->m_core.AddShieldHp(sV);
	}
	if (sV <= 0)
	{
		GetParentEx()->m_core.SetShieldHp(0);
	}
}
//Tianh 反弹伤害率
void CSvSkillCreature::AddReboundHurtR(float sV,int Type)
{
	GetParentEx()->ReboundHurtR += sV;
	GetParentEx()->ReboundHurtRType += Type;
}

void CSvSkillCreature::AddCutMaxHP(float sV)
{
	GetParentEx()->AddHp((GetParentEx()->m_core.GetMaxHp()*sV),true);
}

void CSvSkillCreature::AddCutMaxMP(float sV)
{
	GetParentEx()->m_core.AddMp((GetParentEx()->m_core.GetMaxMp()*sV));
}

void CSvSkillCreature::AddReceiveDamageSkillR(float fV)//增加接收技能伤害倍率
{	
	GetParentEx()->m_ReceiveDamageSkillR += fV;
}

void CSvSkillCreature::StateMove(short sV) // 78 是否可以移动
{
    if (sV>0)      GetParentEx()->m_core.AddMove();
    else if (sV<0) GetParentEx()->m_core.SubMove();
}

void CSvSkillCreature::StateDodge(short sV) // 79 是否可以闪避
{
    if (sV>0)      GetParentEx()->m_core.AddDodge();
    else if (sV<0) GetParentEx()->m_core.SubDodge();
}

void CSvSkillCreature::StateUseSkill(short sV) // 80 是否可以使用技能
{
    if (sV>0)      GetParentEx()->m_core.AddUseSkill();
    else if (sV<0) GetParentEx()->m_core.SubUseSkill();
}

void CSvSkillCreature::StateNAttack(short sV) // 81 是否可以普通攻击
{
    if (sV>0)      GetParentEx()->m_core.AddNormalAttack();
    else if (sV<0) GetParentEx()->m_core.SubNormalAttack();
}

void CSvSkillCreature::StateResist(short sV) // 82 增加状态抵抗
{
    if (sV>0)      GetParentEx()->m_core.AddAbiResist(sV);
    else if (sV<0) GetParentEx()->m_core.RemoveAbiResist(-sV);
}

void CSvSkillCreature::Hate(short sV) // 83 额外的仇恨度
{
    GetParentEx()->m_core.Hate += sV;
}

void CSvSkillCreature::HateRate(short sV) // 84 仇恨比率
{
    GetParentEx()->m_core.HateRate += sV;
}

void CSvSkillCreature::AddHate(short sV, CDummyActorSkill *pReleaser) // 85 增加怪物仇恨
{
    CRegionCreature* pCra = GetParentEx();
    if (pCra->m_ai)
    {
        pCra->m_ai->OnBeAttacked(((CSvSkillCreature*)pReleaser)->GetParentEx(), sV);
    }
}

void CSvSkillCreature::AddSkillCriRate(short sV, CDummyActorSkill *pReleaser) // 86 技能爆击率
{
    CRegionCreature* pCra = GetParentEx();
    pCra->m_core.SkillCriRate += sV;
}

void CSvSkillCreature::AddSkillCriDamageR(short sV, CDummyActorSkill *pReleaser) // 87 技能爆击伤害
{
    CRegionCreature* pCra = GetParentEx();
    pCra->m_core.SkillCriDamageRate += sV;
}

void CSvSkillCreature::ChangeWeapon()
{
	GetParentEx()->ChangeWeapon();
}

void CSvSkillCreature::MomentMove(int sV) //瞬间移动
{
	GetParentEx()->DoTeleportSkill(sV);
}

void CSvSkillCreature::ChangeBody(int ModelID) //变身
{
	if (!GetParentEx()->IsChengBody()) //如果在非变身状态
	{
		GetParentEx()->ChangeBoby(ModelID);
	}
	else
	{
		GetParentEx()->ChangeBoby(GetParentEx()->m_dwLastModelID);
	}
	
}

void CSvSkillCreature::BackHome()  //回程
{
	GetParentEx()->BackHome();
}

void CSvSkillCreature::Ride()
{
    if (GetParentEx()->IsRidePet())
    {
        GetParentEx()->RideOffPet();
    }else
    {
        GetParentEx()->RidePet();
    }
}

void CSvSkillCreature::AddResistZXC(int sV) //抵抗爆击 
{
	GetParentEx()->m_core.ReSkillCritical.Temp += sV;
}

void CSvSkillCreature::AddResistSwim(int sV) //抵抗眩晕 
{
	GetParentEx()->m_ResistSwim += sV;
}

void CSvSkillCreature::AddResistInertia(int sV) //抵抗迟钝 
{
	GetParentEx()->m_ResistLethargy += sV;
}

void CSvSkillCreature::AddResistTrance(int sV) //抵抗昏睡
{
	GetParentEx()->m_ResistBleed += sV;
}

void CSvSkillCreature::AddResistBleed(int sV) //抵抗流血
{
	GetParentEx()->m_ResistBluntness += sV;
}

void CSvSkillCreature::TeamShareDamage(int sV)//队友分担伤害
{
	GetParentEx()->m_TeamShareDamage += sV;
}

void CSvSkillCreature::Hprelay(int sV,CActorSkill *pTarget)//把自己当前的HP传递给队友
{
	int hp = GetParentEx()->m_core.GetHp() * sV/100;
	GetParentEx()->m_core.AddHp(-hp);
	if (pTarget)
	{
		((CSvSkillCreature*)pTarget)->GetParentEx()->m_core.AddHp(hp);
	}
}
void CSvSkillCreature::Mprelay(int sV,CActorSkill *pTarget)//把自己当前的MP传递给队友
{
	int mp = GetParentEx()->m_core.GetMp() * sV/100;
	GetParentEx()->m_core.AddMp(-mp);
	if (pTarget)
	{
		((CSvSkillCreature*)pTarget)->GetParentEx()->m_core.AddMp(mp);
	}
}

void CSvSkillCreature::SkillhpRecover(int sV)      //技能每秒恢复血量
{
	GetParentEx()->m_SkillhpRecover += sV;
} 

void CSvSkillCreature::SkillmpRecover(int sV)    //技能每秒恢复蓝量
{
	GetParentEx()->m_SkillmpRecover += sV;
}

//void CSvSkillCreature::ConsumeNpc(int sV)  //消耗召唤NPC的个数
//{
//	GetParentEx()->DeleteCallNpc(sV);
//}

void CSvSkillCreature::UltimateChange(int sV)//终极变身
{
	GetParentEx()->UltimateChange(sV);
}

void CSvSkillCreature::MonomersCgroup(int sV)//单体变群体普攻 传入范围
{
	GetParentEx()->m_ScopeoDinaryInjury += sV;
}

void CSvSkillCreature::PlayerHiding(int sV)//隐身技能的实现
{
	if (sV == 1||sV == -1 )
	{
		GetParentEx()->Hiding(sV);
	}
	else if (sV == 2||sV == -2)
	{
		GetParentEx()->PlayerLurk(sV);
	}
}

void CSvSkillCreature::PlayerImmuno(int sV) //+1 免疫有益 + 2免疫有害  +3免疫所有
{
	if (sV == 1)//是否免疫有益BUF
	{
		(&(GetParentEx()->m_Skill))->ImmunoUseful = true;
	}
	else if (sV == -1)
	{
		(&(GetParentEx()->m_Skill))->ImmunoUseful = false;
	}
	else if (sV == 2) //是否免疫有害BUF
	{
		(&(GetParentEx()->m_Skill))->ImmunoHarmful = true;
	}
	else if (sV == -2)
	{
		(&(GetParentEx()->m_Skill))->ImmunoHarmful = false;
	}
	else if (sV == 3)//是否免疫多有BUF
	{
		(&(GetParentEx()->m_Skill))->ImmunoAll = true;
	}
	else if (sV == -3)
	{
		(&(GetParentEx()->m_Skill))->ImmunoAll = false;
	}

}

void CSvSkillCreature::AddSkillCriDamage(int sV)// 技能爆击伤害加成
{
	GetParentEx()->m_core.SkillCriDamage += sV;
}

void CSvSkillCreature::AddCriticalDamage(int sV)// 普通攻击爆击伤害
{
	GetParentEx()->m_core.CriticalDamage += sV;
}

void CSvSkillCreature::AddCriticalDamageR(int sV)// 普通攻击爆击倍率，就是爆击伤害倍数
{
	GetParentEx()->m_core.CriticalDamageR += sV;
}

void CSvSkillCreature::AddShieldMpR(int sV)
{
	//Tianh 苗疆的耗蓝盾的判断需要  血盾的支持
	if (GetParentEx()->m_core.IsShieldMpR)
	{
		GetParentEx()->m_core.SetShieldMpR(0);
		GetParentEx()->m_core.SetShieldHp(0);

		GetParentEx()->m_core.IsShieldMpR = false;

		GetParentEx()->m_Skill.ClearEffByClass(18); //去掉魔法盾效果(圣巫)
	}
	else
	{
		GetParentEx()->m_core.SetShieldMpR(sV);
		GetParentEx()->m_core.SetShieldHp(100);

		GetParentEx()->m_core.IsShieldMpR = true;
	}
}

void CSvSkillCreature::AddAbsorptionHpR(int sV) //受到的伤害转化为HP率
{
	GetParentEx()->m_core.AbsorptionHpR += sV;
}



void CSvSkillCreature::Unride()
{
    GetParentEx()->RideOffPet();
}

void CSvSkillCreature::AddEff(short sEffectID, DWORD dwUID)
{
    AddEffect(sEffectID, dwUID, true);
}

void CSvSkillCreature::Conjure(int NpcId,int Num)
{
	if (Num == 0)
	{
		return;
	}
	if (NpcId<0 || Num<0)
	{
		GetParentEx()->DeleteAddUnderlingNpc();
	}
	else
	{
		GetParentEx()->CreateAllUnderlingNpc(NpcId,PLAYER_CALL,Num);//玩家召唤
	} 
}

void CSvSkillCreature::ClearUseful()
{
    std::list<SActorEffect>::iterator it;
    for (it=m_listEffect.begin(); it!=m_listEffect.end();)
    {
        if ((*it).pAttr->cAType==1) // 有益效果
        {
            OnEffectClose(&(*it)); // 关闭后就一定要删除
			it = m_listEffect.erase(it);
			continue;
		}
		it ++;
	}
	OnEffectModify();
}

//<add by Tianh> 
//更换武器的特殊删除
void CSvSkillCreature::SpecialClearUseful()
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->cAType==1) // 有益效果
		{
			if ((*it).pAttr->isCleared)
			{
				OnEffectClose(&(*it)); // 关闭后就一定要删除
				it = m_listEffect.erase(it);
				continue;
			}
		}
		it ++;
	}
	OnEffectModify();
}

//<add by Tianh> 
//遁地时的特殊删除
void CSvSkillCreature::TopoClearUseful()
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->isCleared)
		{
			OnEffectClose(&(*it)); // 关闭后就一定要删除
			it = m_listEffect.erase(it);
			continue;
		}
		it ++;
	}
	OnEffectModify();
}

void CSvSkillCreature::ClearBaleful()
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->cAType==2) // 有害效果
		{
			OnEffectClose(&(*it)); // 关闭后就一定要删除
			it = m_listEffect.erase(it);
			continue;
		}
		it ++;
	}
	OnEffectModify();
}

void CSvSkillCreature::MoveParent(CActorSkill *pReleaser)//把目标瞬间移动到自己旁边
{
	if (!GetParentEx()->GetPlayerLogic()->IsLive()) //死亡状态不能移动
	{
		return;
	}
	if (GetParentEx()->m_eCurPosPKAttr != PKA_SAFEST || GetParentEx()->IsMonster())
	{
		if (GetParentEx()->IsMonster())
		{
			GetParentEx()->SetTarget(((CSvSkillCreature*)pReleaser)->GetParentEx());
		}
		else if(!((CSvSkillCreature*)pReleaser)->GetParentEx()->IsTeamFriend(GetParentEx()))
		{
			((CSvSkillCreature*)pReleaser)->GetParentEx()->EnterCombat();
		}
		
	}
	float npos[3];
	npos[0] = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_pos[0];
	npos[1] = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_pos[1];
	npos[2] = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_pos[2];
	GetParentEx()->CMoveParent(npos);
}

void CSvSkillCreature::ClearAllEff()
{
	ClearEffect(true);
}

void CSvSkillCreature::ClearEff(short sEffectID)
{
	DeleteEffect(sEffectID, true);
}


void CSvSkillCreature::ClearEffByClass(BYTE cClass)
{
	SActorEffect* pActorEffect = FindEffectByClassID(cClass);
	if (pActorEffect)
	{
		if (pActorEffect->pAttr)
		{
			DWORD sEffectID = pActorEffect->pAttr->wID;
			DeleteEffect(sEffectID, true);
		}
	}
}

int CSvSkillCreature::IsHaveCallNpc()
{
	if (GetParentEx()->CallNpc.size())
	{
		GetParentEx()->DeleteCallNpc(1); //删除1个召唤NPC
		return 1;
	}

	return 0;
}

int CSvSkillCreature::IsChangeBody()
{
	if (GetParentEx()->IsChengBody()) return 1;
	return 0;
}

int CSvSkillCreature::GetSkillSeries()
{
	return GetParentEx()->m_SkillSeries;
}

int CSvSkillCreature::IsRide()
{
	if (GetParentEx()->IsRided()) return 2;
	else if (GetParentEx()->IsRidePet()) return 1;
	return 0;
}

void CSvSkillCreature::OnUseRangeSkillAddHp(SSkill* pSkillAttr, const float* pPos,CActorSkill* pReleaser)
{
	int i, iCnt;
	long lPos[3];
	lPos[0] = pPos[0];
	lPos[1] = pPos[1];
	lPos[2] = pPos[2];

	CRegionCreature* pSource = ((CSvSkillCreature*)pReleaser)->GetParentEx();

	unsigned short wMyARange;
	unsigned char cMyAMaxObj;
	CActorSkill::SActorPassive* pStrengthenSkill = NULL;

	//Tianh 多加强型
	wMyARange = pSkillAttr->wARange;
	cMyAMaxObj = pSkillAttr->cAMaxObj;
	if (!(pSkillAttr->wManyStrSkillSub.empty()))
	{
		for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
		{
			pStrengthenSkill = FindPassiveSkillBySubID(*(it));

			if (pStrengthenSkill)
			{
				if (pStrengthenSkill->pAttr->wARange)
				{
					wMyARange += pStrengthenSkill->pAttr->wARange;
				}
				if (pStrengthenSkill->pAttr->cAMaxObj)
				{
					cMyAMaxObj += pStrengthenSkill->pAttr->cAMaxObj;
				}
			}
		}
	}

	CRegionCreature* pTargetCreature;
	CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
	iCnt = GetParentEx()->m_scene->GetAroundCreatureByTile(pFindCreature, lPos, wMyARange, NULL, PT_PATH, false);
	if (cMyAMaxObj>0)
	{
		iCnt = (cMyAMaxObj<iCnt) ? cMyAMaxObj : iCnt;
	}

	for (i=0; i<iCnt; i++)
	{
		// 检查技能的使用对象
		DWORD dwTargetParty = pSource->GetTargetParty((CRegionCreature*)(pFindCreature[i]));
		if(pFindCreature[i]->m_type==OBJECT_TYPE_CREATURE || pFindCreature[i]->m_type==OBJECT_TYPE_NPC_COMBATIVE)
		{
			pTargetCreature = (CRegionCreature*)(pFindCreature[i]);

			if (/*pTargetCreature->m_unionID!=0*/pTargetCreature->IsUser())
			{
				if (GetParentEx()->IsTeamFriend(pTargetCreature) || pTargetCreature == pSource)
				{
					pTargetCreature->GetEntity()->m_Skill.OnSkillAddHP(pSkillAttr, this);
				}		
			}
		}

	}
}

void CSvSkillCreature::OnUseRangeSkill(SSkill* pSkillAttr,const float* pPos)
{
	//MSG1("CSvSkillCreature::OnUseRangeSkill %s\n", pSkillAttr->szName);
	// 先查找攻击范围内的所有人，再对所有的人都调用一次下面函数
	int i, iCnt;
	long lPos[3];
	lPos[0] = pPos[0];
	lPos[1] = pPos[1];
	lPos[2] = pPos[2];
	bool IsFlashMove = false;

	unsigned short wMyARange;
	unsigned char cMyAMaxObj;
	CActorSkill::SActorPassive* pStrengthenSkill = NULL;

	//Tianh 多加强型
	wMyARange = pSkillAttr->wARange;
	cMyAMaxObj = pSkillAttr->cAMaxObj;
	if (!(pSkillAttr->wManyStrSkillSub.empty()))
	{
		for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
		{
			pStrengthenSkill = FindPassiveSkillBySubID(*(it));

			if (pStrengthenSkill)
			{
				if (pStrengthenSkill->pAttr->wARange)
				{
					wMyARange += pStrengthenSkill->pAttr->wARange;
				}
				if (pStrengthenSkill->pAttr->cAMaxObj)
				{
					cMyAMaxObj += pStrengthenSkill->pAttr->cAMaxObj;
				}
			}
		}
	}


	//{
	//	pStrengthenSkill = FindPassiveSkillBySubID(pSkillAttr->wStrSkillSub);
	//}
	//if (pStrengthenSkill)
	//{
	//	if (pStrengthenSkill->pAttr->wARange)
	//	{
	//		wMyARange = pStrengthenSkill->pAttr->wARange;
	//	}else
	//	{
	//		wMyARange = pSkillAttr->wARange;
	//	}
	//	if (pStrengthenSkill->pAttr->cAMaxObj)
	//	{
	//		cMyAMaxObj = pStrengthenSkill->pAttr->cAMaxObj;
	//	}else
	//	{
	//		cMyAMaxObj = pSkillAttr->cAMaxObj;
	//	}
	//}else
	//{
	//	wMyARange = pSkillAttr->wARange;
	//	cMyAMaxObj = pSkillAttr->cAMaxObj;
	//}


   //Tianh  添加瞬间移动技能
	if (pSkillAttr->isFlashMove)
	{
		IsFlashMove = true;
	}
	if (IsFlashMove)
	{
		///*瞬间移动*/
		//long mPos[3]; //人物现在的坐标
		//mPos[0] = GetParentEx()->m_pos[0];
		//mPos[1] = GetParentEx()->m_pos[1];
		//mPos[2] = GetParentEx()->m_pos[2];
	}
	else
	{
		CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
		iCnt = GetParentEx()->m_scene->GetAroundCreatureByTile(pFindCreature, lPos, wMyARange, NULL, PT_PATH, false);
		if (cMyAMaxObj>0)
		{
			iCnt = (cMyAMaxObj<iCnt) ? cMyAMaxObj : iCnt;
		}
		//Tianh 如果释放者不是人或者人的兽 或者释放的技能是有益技能，就允许释放
		bool bCondition = (!((GetParentEx()->IsUser()) || (GetParentEx()->IsUserPet()) || (GetParentEx()->IsCallNpc()))) || (pSkillAttr->bABad==0);
		CRegionCreature* pTargetCreature;
		for (i=0; i<iCnt; i++)
		{
			if(pFindCreature[i]->m_type==OBJECT_TYPE_CREATURE || pFindCreature[i]->m_type==OBJECT_TYPE_NPC_COMBATIVE)
			{
				pTargetCreature = (CRegionCreature*)(pFindCreature[i]);
				// 如果是范围攻击 并且 对方是人，就无效，这是因为还没有处理PK的情况，会导致群攻会PK别人

				if ( (GetParentEx()->IsUser() || GetParentEx()->IsUserPet() || GetParentEx()->IsCallNpc()) && 
					((pTargetCreature->IsUser() || pTargetCreature->IsUserPet()) || pTargetCreature->IsCallNpc()) && pSkillAttr->bABad )
				{
					CRegionCreature *pSource = NULL;
					CRegionCreature *pTarget = NULL;
					if ( GetParentEx()->IsUser() )
					{
						pSource = GetParentEx();
					}
					else if ( GetParentEx()->IsUserPet() || GetParentEx()->IsCallNpc())
					{
						pSource = GetParentEx()->m_master;
					}

					if ( pTargetCreature->IsUser() )
					{
						pTarget = pTargetCreature;
					}
					else if ( pTargetCreature->IsUserPet() || GetParentEx()->IsCallNpc())
					{
						pTarget = pTargetCreature->m_master;
					}

					if (pSource)
					{
						bool useDefault = false;
						switch (pSource->m_eCurPosPKAttr)
						{
						case PKA_SAFEST:
							useDefault = true;
							continue;
							break;
						case PKA_NORMAL:
						case PKA_UNSAFE:
							if (pSource->IsSelfDef(pTarget))
							{
								if (pTargetCreature->GetEntity()->m_Skill.CheckCanUse(pSkillAttr, this))
								{
									pTargetCreature->GetEntity()->m_Skill.OnSkillUse(pSkillAttr, this);
								}
								break;
							}
							if (pSource->m_ePkMode == PK_Monster)
							{
								continue;
							}
							else if (pSource->m_ePkMode == PK_Goodevil)
							{
								if(pTargetCreature->GetPKState() == PK_STATE_RED_NAME)
								{
									if (pTargetCreature->GetEntity()->m_Skill.CheckCanUse(pSkillAttr, this))
									{
										pTargetCreature->GetEntity()->m_Skill.OnSkillUse(pSkillAttr, this);
									}
								}
							}
							else if (pSource->m_ePkMode == PK_Free)
							{
								if (pTargetCreature->GetEntity()->m_Skill.CheckCanUse(pSkillAttr, this))
								{
									pTargetCreature->GetEntity()->m_Skill.OnSkillUse(pSkillAttr, this);
								}
							}
							break;
						default:
							useDefault = true;
							continue;
							break;
						}
					}
				}
				else if( bCondition || !(pTargetCreature->IsUser() || pTargetCreature->IsUserPet() || pTargetCreature->IsCallNpc()))
				{
					if(pTargetCreature->m_cheatDummy) continue;
					if (pTargetCreature->GetEntity()->m_Skill.CheckCanUse(pSkillAttr, this))
					{
						pTargetCreature->GetEntity()->m_Skill.OnSkillUse(pSkillAttr, this);
					}
				}
			}else
			{
				ERR1("CSvSkillCreature::OnUseRangeSkill ob type error [%d]",pFindCreature[i]->m_type);
			}
		}
	}
}

CActorSkill* CSvSkillCreature::FindActorSkill(DWORD dwCreatureOID)
{
    if (dwCreatureOID==GetParentEx()->m_oId) return this;
    return ::FindActorSkill(GetParentEx(), dwCreatureOID);
}

bool CSvSkillCreature::CheckCanUse(SSkill* pSkillAttr, CActorSkill* pReleaser)
{
    CRegionCreature* pTarget = GetParentEx();
    CRegionCreature* pSource = ((CSvSkillCreature*)pReleaser)->GetParentEx();
	if (pSkillAttr->IsAddHpSkill)
	{
		return true;
	}
    if (pSkillAttr->bABad)
    {
        if (!pSource->CanAttack(pTarget, pSkillAttr))
        {
#ifdef _DEBUG
            MSG("CSvSkillCreature::OnSkillUse Cannot attack\n");
#endif
            return false; // 有害技能要检查是否能使用
        }
    }
	//else
	//{
	//	if (pTarget->ImmunoUseful || pTarget->ImmunoAll) //如果对方免疫有益BUF或者全部免疫
	//	{
	//		return false;
	//	}
	//}

    // 检查技能的使用对象
    DWORD dwTargetParty = pSource->GetTargetParty(pTarget);

	//4.24 LeiJun 对友方施放技能的情况
	if(dwTargetParty == SKILL_TARGET_FRIEND_ACTOR && !pSkillAttr->bABad)
		return true;

    if (pSkillAttr->dwATarget==0 && (dwTargetParty&4))
        return true;

    dwTargetParty |= SKILL_TARGET_POSITION;
    if ((dwTargetParty & pSkillAttr->dwATarget)==0)
    {
        //MSG("CSvSkillCreature::OnSkillUse Cannot attack 2\n");
        return false;
    }
    return true;
}

//Tianh 加血技能流程
void CSvSkillCreature::OnSkillAddHP(SSkill* pSkillAttr, CActorSkill* pReleaser)
{
	int dead = 0;
	int IsRidedMax,IsRidedMin = 0;
	int eleDmg[MAX_ELEMENT_ATTR] = {0, 0, 0};
	int delta = 0;
	int phyDmg = 0;
	int icDmgPhy = 0;
	int icDmgElement[MAX_ELEMENT_ATTR] = {0, 0, 0};

	SActorPassive* pStrengthenSkill;
	/*SActorPassive* pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(pSkillAttr->wStrSkillSub);*/
	for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
	{
		pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));
		if (pStrengthenSkill)
		{
			pStrengthenSkill->pAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);
			pStrengthenSkill->pAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);

			pStrengthenSkill->pAttr->actSelfUse.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);
			pStrengthenSkill->pAttr->actTargetUse.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
		}

	}
	//MSG1("CSvSkillCreature::OnSkillUse %s\n", pSkillAttr->szName);
	pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode); // 第一个参数添NULL是因为添this也没有用，没有技能是针对自己的
	pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);

	delta = (((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMax()) - (((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMin());
	if(delta == 0)
		phyDmg = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMin();
	else
		phyDmg = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMin() + GetRand(delta);

	eleDmg[ELEMENT_WATER] = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.ElementDamage[ELEMENT_WATER].GetValue();

	if(eleDmg[ELEMENT_WATER] >= 5)
		eleDmg[ELEMENT_WATER] = GetRand(eleDmg[ELEMENT_WATER]*1.1,eleDmg[ELEMENT_WATER]*0.9);

	//是否爆击
	bool bCriDamage = false;
	bCriDamage = ((CSvSkillCreature*)pReleaser)->GetParentEx()->IsSkillCritical(pSkillAttr->dwAHitRate, this->GetParentEx());
	if (bCriDamage)
	{
		((CSvSkillCreature*)pReleaser)->GetParentEx()->GetSkillCriDamage(icDmgPhy, icDmgElement, this->GetParentEx());
		phyDmg	+= icDmgPhy + phyDmg * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;
		eleDmg[ELEMENT_WATER]  += icDmgElement[ELEMENT_WATER] + eleDmg[ELEMENT_WATER] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;;
	}
	
	//给对方接收治疗   函数里有判断
	int addHp = phyDmg+eleDmg[ELEMENT_WATER]/4; //这里 治疗的水伤害/4
	GetParentEx()->AddHp(addHp,true,bCriDamage);
	


	for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
	{
		pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));
		if (pStrengthenSkill)
		{
			pStrengthenSkill->pAttr->actTargetTemp.Apply(pReleaser, this, 0, true,pSkillAttr->DamageMode);
			pStrengthenSkill->pAttr->actSelfTemp.Apply(NULL, pReleaser, 0, true,pSkillAttr->DamageMode);
		}
	}
	pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0, true);
	pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0, true);
}

void CSvSkillCreature::OnSkillUse(SSkill* pSkillAttr, CActorSkill* pReleaser)
{
	(&(GetParentEx()->m_Skill))->SetbRun(true);
	SActorPassive* pStrengthenSkill;
    /*SActorPassive* pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(pSkillAttr->wStrSkillSub);*/
	for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
	{
		 pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));
		 if (pStrengthenSkill)
		 {
			 pStrengthenSkill->pAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);
			 pStrengthenSkill->pAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
		 }

	}
    //MSG1("CSvSkillCreature::OnSkillUse %s\n", pSkillAttr->szName);
    pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode); // 第一个参数添NULL是因为添this也没有用，没有技能是针对自己的
    pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);

    int iDmgPhy = 0;
    int iDmgElement[MAX_ELEMENT_ATTR] = {0, 0, 0};

    long bCriDamage = 0;
    bool bResult = true;

	if (pSkillAttr->bADamage)
	{
		((CSvSkillCreature*)pReleaser)->GetParentEx()->ClearHidingAndTopo();

		((CSvSkillCreature*)pReleaser)->GetParentEx()->GetDamage(iDmgPhy, iDmgElement);
	}

	//Tianh  处理生命锁链技能
	if (GetParentEx()->m_TeamShareDamage)
	{
		short ID = 0;
		std::vector<CRegionCreature*> ActorSkill;//存储符合条件队友的指针容器
		CActorSkill *pActorSkill = FindActorSkill(GetParentEx()->m_oId);
		if (pActorSkill)
		{
			ID = pActorSkill->FindEffectbyClass(LIFE_CHAINS);
		}
		//判断队的人是否还有1个以上的队友还有相同的BUF状态，否则不分担伤害
		//生命锁链只对队伍里有相同BUF的人有效果
		 
		if (GetParentEx()->m_pTeam)
		{
			for (int i = 0 ;i<MAX_TEAM_MENBER_COUNT;i++)
			{

				CRegionUser *pRegionUser = g_region->FindUser(GetParentEx()->m_pTeam->mData.mMember[i].mDBID);
				if (pRegionUser)
				{
					if (pRegionUser->m_dummy)
					{	
						if (pRegionUser->m_dummy->m_oId == (GetParentEx()->m_oId))
						{
							//如果是自己
							continue;
						}

						CActorSkill *pActorSkill = FindActorSkill(pRegionUser->m_dummy->m_oId);
						if (pActorSkill)
						{
							if (pActorSkill->FindEffectbyId(ID))
							{
								ActorSkill.push_back(pRegionUser->m_dummy);
							}	
						}
					}			
				}
			}
		}
		
		if (ActorSkill.size()<1) 
		{
			//没有队友有BUF状态   不分担
			OnSkillUseCalculating(pSkillAttr,pReleaser,iDmgPhy,iDmgElement,bCriDamage,bResult);
		}
		else
		{
			int viDmgElement[MAX_ELEMENT_ATTR] = {0,0,0};
			int kiDmgElement[MAX_ELEMENT_ATTR] = {0,0,0};
			//开始分担伤害
			float viDmgPhyR = (GetParentEx()->m_TeamShareDamage)/100.0f;
		    int viDmgPhy = 	iDmgPhy * viDmgPhyR;
			viDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER] * viDmgPhyR;
			viDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE] * viDmgPhyR;
			viDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON] * viDmgPhyR;
			//有k-1个队友来分担伤害
			int conut = ActorSkill.size();
			int kiDmgPhy = viDmgPhy/conut;
			kiDmgElement[ELEMENT_WATER] = viDmgElement[ELEMENT_WATER]/conut;
			kiDmgElement[ELEMENT_FIRE] = viDmgElement[ELEMENT_FIRE]/conut;
			kiDmgElement[ELEMENT_POISON] = viDmgElement[ELEMENT_POISON]/conut;

			CRegionCreature* pCreature = GetParentEx();
			std::vector<CRegionCreature*> ::iterator itArea = ActorSkill.begin();
			for(; itArea != ActorSkill.end(); itArea++)
			{
				SetParentEx((*itArea));
				OnSkillUseCalculating(pSkillAttr,pReleaser,kiDmgPhy,kiDmgElement,bCriDamage,bResult);	
				SActorPassive* pStrengthenSkill;
				/*SActorPassive* pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(pSkillAttr->wStrSkillSub);*/
				for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
				{
					pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));
					if (pStrengthenSkill)
					{
						pStrengthenSkill->pAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);
						pStrengthenSkill->pAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
					}

				}
				//MSG1("CSvSkillCreature::OnSkillUse %s\n", pSkillAttr->szName);
				pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode); // 第一个参数添NULL是因为添this也没有用，没有技能是针对自己的
				pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
			}
			SetParentEx(pCreature);

			//还属于自己本身的伤害
			float iDmgPhyR = 1.0f - (GetParentEx()->m_TeamShareDamage)/100.0f;
			iDmgPhy = iDmgPhy * iDmgPhyR;
			iDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER] * iDmgPhyR;
			iDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE] * iDmgPhyR;
			iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON] * iDmgPhyR;
			
			OnSkillUseCalculating(pSkillAttr,pReleaser,iDmgPhy,iDmgElement,bCriDamage,bResult);


		}
	}
	else
	{
		OnSkillUseCalculating(pSkillAttr,pReleaser,iDmgPhy,iDmgElement,bCriDamage,bResult);
	}

}

void CSvSkillCreature::OnSkillUseCalculating(SSkill* pSkillAttr, CActorSkill* pReleaser,int iDmgPhy,int iDmgElement[MAX_ELEMENT_ATTR],long bCriDamage,bool bResult)
{

	int viStateRate = pSkillAttr->iStateRate;
	if (pSkillAttr->ResistState)
	{
		switch(pSkillAttr->ResistState)
		{
		case SKILL_STATE_XUANYUN:     //眩晕
			{
				if (GetParentEx()->m_ResistSwim)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistSwim);
				}
			}
			break;
		case SKILL_STATE_HUNSHUI:      //昏睡      
			{
				if (GetParentEx()->m_ResistLethargy)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistLethargy);
				}
			}
			break;
		case SKILL_STATE_LIUXUE:       //流血
			{
				if (GetParentEx()->m_ResistBleed)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistBleed);
				}
			}
			break;
		case SKILL_STATE_CHIDUN:      //迟钝 
			{
				if (GetParentEx()->m_ResistBluntness)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistBluntness);
				}
			}
			break;
		}
	}
	SActorPassive* pStrengthenSkill;
	//Tianh 添加伤害计算的方式
	if (pSkillAttr->bADamage) // 如果不是有害的技能就全部命中，否则就到里面去计算命中
	{

		bResult = ((CSvSkillCreature*)pReleaser)->GetParentEx()->TryHit(this->GetParentEx());
		if (bResult)
		{	

			/*((CSvSkillCreature*)pReleaser)->GetParentEx()->GetDamage(iDmgPhy, iDmgElement);*/

			GetParentEx()->ApplyDamage(((CSvSkillCreature*)pReleaser)->GetParentEx(), iDmgPhy, iDmgElement,true,pSkillAttr);
			bCriDamage = ((CSvSkillCreature*)pReleaser)->GetParentEx()->IsSkillCritical(pSkillAttr->dwAHitRate, this->GetParentEx());
			if (bCriDamage)
			{
				int icDmgPhy = 0;
				int icDmgElement[MAX_ELEMENT_ATTR] = {0, 0, 0};

				((CSvSkillCreature*)pReleaser)->GetParentEx()->GetSkillCriDamage(icDmgPhy, icDmgElement, this->GetParentEx());
				//iDmgPhy                     += icDmgPhy;
				//Tianh 添加了爆击伤害加成SkillCriDamage

				iDmgPhy						+= icDmgPhy + iDmgPhy * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;

				iDmgElement[ELEMENT_WATER]  += icDmgElement[ELEMENT_WATER] + icDmgElement[ELEMENT_WATER] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;
				iDmgElement[ELEMENT_FIRE]   += icDmgElement[ELEMENT_FIRE] + icDmgElement[ELEMENT_FIRE] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;
				iDmgElement[ELEMENT_POISON] += icDmgElement[ELEMENT_POISON] + icDmgElement[ELEMENT_POISON] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;

				//如果类型是1的话，只计算元素伤害 //Tianh  这里如果伤害方式为1 则直接拿不参加计算的值
				if (pSkillAttr->DamageMode == SKILL_BADAMAGE_TWO)
				{
					iDmgPhy = 0;
					iDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER]*0.25;
					iDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE]*0.25;
					iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]*0.25;
				}
			}		
		}
	}

	bool bLive = true;
	if (bResult)
	{
		pSkillAttr->actSelfUse.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);

		for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
		{
			pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));
			if (pStrengthenSkill)
			{
				pStrengthenSkill->pAttr->actSelfUse.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);

				pStrengthenSkill->pAttr->actTargetUse.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
				iDmgPhy += GetImmDamage();
				iDmgElement[ELEMENT_WATER]  += GetImmWaterDm();
				iDmgElement[ELEMENT_FIRE]   += GetImmFireDm();
				iDmgElement[ELEMENT_POISON] += GetImmPoisonDm();


				int viStateRate = pStrengthenSkill->pAttr->iStrStateRate;
				if (viStateRate == 0)
				{
					viStateRate = pStrengthenSkill->pAttr->iStateRate;
				}
				if (pSkillAttr->ResistState)
				{
					switch(pSkillAttr->ResistState)
					{
					case SKILL_STATE_XUANYUN:     //眩晕
						{
							if (GetParentEx()->m_ResistSwim)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistSwim);
							}
						}
						break;
					case SKILL_STATE_HUNSHUI:      //昏睡      
						{
							if (GetParentEx()->m_ResistLethargy)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistLethargy);
							}
						}
						break;
					case SKILL_STATE_LIUXUE:       //流血
						{
							if (GetParentEx()->m_ResistBleed)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistBleed);
							}
						}
						break;
					case SKILL_STATE_CHIDUN:      //迟钝 
						{
							if (GetParentEx()->m_ResistBluntness)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistBluntness);
							}
						}
						break;
					}
				}

				// 取得状态命中		
				if (((CSvSkillCreature*)pReleaser)->GetParentEx()->TryStateHit(viStateRate, GetParentEx(),pSkillAttr))
				{
					pStrengthenSkill->pAttr->actSelfState.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);
					pStrengthenSkill->pAttr->actTargetState.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
					iDmgPhy += GetImmDamage();
					iDmgElement[ELEMENT_WATER]  += GetImmWaterDm();
					iDmgElement[ELEMENT_FIRE]   += GetImmFireDm();
					iDmgElement[ELEMENT_POISON] += GetImmPoisonDm();
				}
			}
		}

		if (((CSvSkillCreature*)pReleaser)->GetParentEx()->TryStateHit(viStateRate, GetParentEx(),pSkillAttr))
		{
			pSkillAttr->actSelfState.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode);
			pSkillAttr->actTargetState.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
			iDmgPhy += GetImmDamage();
			iDmgElement[ELEMENT_WATER]  += GetImmWaterDm();
			iDmgElement[ELEMENT_FIRE]   += GetImmFireDm();
			iDmgElement[ELEMENT_POISON] += GetImmPoisonDm();
		}

		pSkillAttr->actTargetUse.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
		iDmgPhy += GetImmDamage();
		iDmgElement[ELEMENT_WATER]  += GetImmWaterDm();
		iDmgElement[ELEMENT_FIRE]   += GetImmFireDm();
		iDmgElement[ELEMENT_POISON] += GetImmPoisonDm();

		if (!bCriDamage)
		{
			if (pSkillAttr->DamageMode == SKILL_BADAMAGE_TWO)
			{
				iDmgPhy = 0;
				iDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER]*0.25;
				iDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE]*0.25;
				iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]*0.25;
			}
		}

		if (((CSvSkillCreature*)pReleaser)->GetParentEx()->m_SkillSeries)
		{
			if (pSkillAttr->wClass == ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_SkillSeries)
			{
				int SkilldamageR = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_SkilldamageR;
				iDmgPhy = iDmgPhy * (1 + SkilldamageR/100);
				iDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER]*(1 + SkilldamageR/100);
				iDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE]*(1 + SkilldamageR/100);
				iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]*(1 + SkilldamageR/100);
			}
		}

		//无敌
		if (GetParentEx()->m_IsInvincible)
		{
			iDmgPhy = 0;
			iDmgElement[ELEMENT_WATER] = 0;
			iDmgElement[ELEMENT_FIRE] = 0;
			iDmgElement[ELEMENT_POISON] = 0;
		}


		long dead = 0;
		bool IsAddhpSkill = false;

		if (pSkillAttr->bADamage==0 || iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
		{

			if (pSkillAttr->bADamage
				|| pSkillAttr->bABad) // add by janhail at 20051207 如果有物理伤害或者是坏技能，就计算伤害(会影响死亡)
			{
				bLive = GetParentEx()->GetPlayerLogic()->IsLive();


				//如果对方有接收技能伤害倍率
				if (GetParentEx()->m_ReceiveDamageSkillR)
				{
					iDmgPhy = iDmgPhy * (GetParentEx()->m_ReceiveDamageSkillR);
					iDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER]*(GetParentEx()->m_ReceiveDamageSkillR);
					iDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE]*(GetParentEx()->m_ReceiveDamageSkillR);
					iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]*(GetParentEx()->m_ReceiveDamageSkillR);
				}

				////Tianh   策划测试用
				//if (((CSvSkillCreature*)pReleaser)->GetParentEx()->IsUser())
				//{
				//	RtCoreLog().Debug("您：【技能】:'%s'对敌人造成物理伤害是: %d\n",pSkillAttr->szName,iDmgPhy);
				//	RtCoreLog().Debug("您：【技能】:'%s'对敌人造成【水】伤害是: %d\n",pSkillAttr->szName,iDmgElement[0]);
				//	RtCoreLog().Debug("您：【技能】:'%s'对敌人造成【火】伤害是: %d\n",pSkillAttr->szName,iDmgElement[1]);
				//	RtCoreLog().Debug("您：【技能】:'%s'对敌人造成【毒】伤害是: %d\n",pSkillAttr->szName,iDmgElement[2]);

				//}
				//else if (((CSvSkillCreature*)pReleaser)->GetParentEx()->IsMonster())
				//{
				//	RtCoreLog().Debug("敌人的宠物：【技能】:'%s'对您造成物理伤害是: %d\n",pSkillAttr->szName,iDmgPhy);
				//	RtCoreLog().Debug("敌人的宠物：【技能】:'%s'对您造成【水】伤害是: %d\n",pSkillAttr->szName,iDmgElement[0]);
				//	RtCoreLog().Debug("敌人的宠物：【技能】:'%s'对您造成【火】伤害是: %d\n",pSkillAttr->szName,iDmgElement[1]);
				//	RtCoreLog().Debug("敌人的宠物：【技能】:'%s'对您造成【毒】伤害是: %d\n",pSkillAttr->szName,iDmgElement[2]);

				//}
				//else if (((CSvSkillCreature*)pReleaser)->GetParentEx()->IsUserPet())
				//{
				//                RtCoreLog().Debug("您的宠物：【技能】:'%s'对敌人造成物理伤害是: %d\n",pSkillAttr->szName,iDmgPhy);
				//	RtCoreLog().Debug("您的宠物：【技能】:'%s'对敌人造成【水】伤害是: %d\n",pSkillAttr->szName,iDmgElement[0]);
				//	RtCoreLog().Debug("您的宠物：【技能】:'%s'对敌人造成【火】伤害是: %d\n",pSkillAttr->szName,iDmgElement[1]);
				//	RtCoreLog().Debug("您的宠物：【技能】:'%s'对敌人造成【毒】伤害是: %d\n",pSkillAttr->szName,iDmgElement[2]);
				//}



				dead = GetParentEx()->RecvDamage(((CSvSkillCreature*)pReleaser)->GetParentEx(), iDmgPhy, iDmgElement,pSkillAttr->bAType);
			}

			CRegionCreature *pThis =  (CRegionCreature*)(GetParent());

			if(bLive)
			{
				bool bIn = false;

				if(NULL != pThis)
				{
					if(pThis->GetPlayerLogic()->IsMiss())
					{
						bIn = true;
						pThis->GetPlayerLogic()->SetMiss(false);
					}
				}
				if (GetParentEx()->m_core.GetShieldHp() == 0)
				{
					((CSvSkillCreature*)pReleaser)->SkillWriteDamage(GetParentEx()->m_oId, bIn, iDmgPhy, iDmgElement,dead,bCriDamage);	
				}		
			}

		}
		/*
		if (this!=pReleaser)
		{
		if (pSkillAttr->bABad) // 如果不是有害的技能就全部命中，否则就到里面去计算命中
		{
		GetParentEx()->OnBeAttacked(((CSvSkillCreature*)pReleaser)->GetParentEx(), iDmgPhy);
		}
		}
		*/
	}else
	{
		// 后面的三个参数没有意义，随便写的
		bLive = GetParentEx()->GetPlayerLogic()->IsLive();
		if(bLive)
		{
			((CSvSkillCreature*)pReleaser)->SkillWriteDamage(GetParentEx()->m_oId, true, iDmgPhy, iDmgElement, 0,bCriDamage);
		}
	}

	for(vector <int>::iterator it = pSkillAttr->wManyStrSkillSub.begin() ;it!= pSkillAttr->wManyStrSkillSub.end() ; it++)
	{
		pStrengthenSkill = pReleaser->FindPassiveSkillBySubID(*(it));
		if (pStrengthenSkill)
		{
			pStrengthenSkill->pAttr->actTargetTemp.Apply(pReleaser, this, 0, true,pSkillAttr->DamageMode);
			pStrengthenSkill->pAttr->actSelfTemp.Apply(NULL, pReleaser, 0, true,pSkillAttr->DamageMode);
		}
	}
	pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0, true);
	pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0, true);

	GetParentEx()->UpdateToClientDelta();

}

void CSvSkillCreature::OnPassiveOpen(SActorPassive* pPassive)
{
    if (m_bRun)
    {
        //MSG1("CSvSkillCreature::OnPassiveOpen %s\n", pPassive->pAttr->szName);
        pPassive->pAttr->actTemp.Apply(this, this, pPassive->dwUID);
        pPassive->pAttr->actOpen.Apply(this, this, pPassive->dwUID);

        int iDmgPhy;
        int iDmgElement[MAX_ELEMENT_ATTR];

        iDmgPhy = GetImmDamage();
        iDmgElement[ELEMENT_WATER]  = GetImmWaterDm();
        iDmgElement[ELEMENT_FIRE]   = GetImmFireDm();
        iDmgElement[ELEMENT_POISON] = GetImmPoisonDm();

        if (iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
        {
			if (GetParentEx()->IsUser())
			{
				iDmgPhy = iDmgPhy/4;
				iDmgElement[ELEMENT_WATER]  = iDmgElement[ELEMENT_WATER]/4;
				iDmgElement[ELEMENT_FIRE]   = iDmgElement[ELEMENT_FIRE]/4;
				iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]/4;
			}

			int ret = GetParentEx()->RecvDamage(NULL, iDmgPhy, iDmgElement,true);

			if (CPlayerLogic::eDead== ret)
			{
				g_region->BuildEventDead(g_sendCmd,GetParentEx());
				return;
			} 
			else
			{	
				long jectId  = GetParentEx()->GetObjectId();
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_update_DeBuf);
				g_sendCmd->WriteLong(jectId);
				g_sendCmd->WriteShort((short)iDmgPhy);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_WATER]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_FIRE]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_POISON]);
				/*GetParentEx()->RecvCmd(g_sendCmd);*/
				GetParentEx()->m_scene->BroadcastCmd(GetParentEx()->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
			}
        }
    }
}

void CSvSkillCreature::OnPassiveClose(SActorPassive* pPassive)
{
    if (m_bRun)
    {
        //MSG1("CSvSkillCreature::OnPassiveClose %s\n", pPassive->pAttr->szName);
        pPassive->pAttr->actClose.Apply(this, this, pPassive->dwUID);
        pPassive->pAttr->actTemp.Apply(this, this, pPassive->dwUID, true);

        int iDmgPhy;
        int iDmgElement[MAX_ELEMENT_ATTR];

        iDmgPhy = GetImmDamage();
        iDmgElement[ELEMENT_WATER]  = GetImmWaterDm();
        iDmgElement[ELEMENT_FIRE]   = GetImmFireDm();
        iDmgElement[ELEMENT_POISON] = GetImmPoisonDm();

        if (iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
        {
			if (GetParentEx()->IsUser())
			{
				iDmgPhy = iDmgPhy/4;
				iDmgElement[ELEMENT_WATER]  = iDmgElement[ELEMENT_WATER]/4;
				iDmgElement[ELEMENT_FIRE]   = iDmgElement[ELEMENT_FIRE]/4;
				iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]/4;
			}

			int ret = GetParentEx()->RecvDamage(NULL, iDmgPhy, iDmgElement,true);

			if (CPlayerLogic::eDead== ret)
			{
				g_region->BuildEventDead(g_sendCmd,GetParentEx());
				return;
			} 
			else
			{	
				long jectId  = GetParentEx()->GetObjectId();
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_update_DeBuf);
				g_sendCmd->WriteLong(jectId);
				g_sendCmd->WriteShort((short)iDmgPhy);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_WATER]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_FIRE]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_POISON]);
				/*GetParentEx()->RecvCmd(g_sendCmd);*/
				GetParentEx()->m_scene->BroadcastCmd(GetParentEx()->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
			}
        }
    }
}

void CSvSkillCreature::OnEffectOpen(SActorEffect* pEffect)
{
    if (m_bRun)
    {
        //MSG1("CSvSkillCreature::OnEffectOpen %s\n", pEffect->pAttr->szName);
        pEffect->pAttr->actTemp.Apply(NULL, this, 0);
        pEffect->pAttr->actOpen.Apply(NULL, this, 0);

        int iDmgPhy;
        int iDmgElement[MAX_ELEMENT_ATTR];

        iDmgPhy = GetImmDamage();
        iDmgElement[ELEMENT_WATER]  = GetImmWaterDm();
        iDmgElement[ELEMENT_FIRE]   = GetImmFireDm();
        iDmgElement[ELEMENT_POISON] = GetImmPoisonDm();

        if (iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
        {
			if (GetParentEx()->IsUser())
			{
				iDmgPhy = iDmgPhy/4;
				iDmgElement[ELEMENT_WATER]  = iDmgElement[ELEMENT_WATER]/4;
				iDmgElement[ELEMENT_FIRE]   = iDmgElement[ELEMENT_FIRE]/4;
				iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]/4;
			}

			int ret = GetParentEx()->RecvDamage(NULL, iDmgPhy, iDmgElement,true);

			if (CPlayerLogic::eDead== ret)
			{
				g_region->BuildEventDead(g_sendCmd,GetParentEx());
				return;
			} 
			else
			{	
				long jectId  = GetParentEx()->GetObjectId();
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_update_DeBuf);
				g_sendCmd->WriteLong(jectId);
				g_sendCmd->WriteShort((short)iDmgPhy);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_WATER]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_FIRE]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_POISON]);
				/*GetParentEx()->RecvCmd(g_sendCmd);*/
				GetParentEx()->m_scene->BroadcastCmd(GetParentEx()->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
			}
        }
    }
}

void CSvSkillCreature::OnEffectClose(SActorEffect* pEffect)
{
    if (m_bRun)
    {
        //MSG1("CSvSkillCreature::OnEffectClose %s\n", pEffect->pAttr->szName);
        pEffect->pAttr->actClose.Apply(NULL, this, 0);
        pEffect->pAttr->actTemp.Apply(NULL, this, 0, true);

        int iDmgPhy;
        int iDmgElement[MAX_ELEMENT_ATTR];

        iDmgPhy = GetImmDamage();
        iDmgElement[ELEMENT_WATER]  = GetImmWaterDm();
        iDmgElement[ELEMENT_FIRE]   = GetImmFireDm();
        iDmgElement[ELEMENT_POISON] = GetImmPoisonDm();

        if (iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
        {
			if (GetParentEx()->IsUser())
			{
				iDmgPhy = iDmgPhy/4;
				iDmgElement[ELEMENT_WATER]  = iDmgElement[ELEMENT_WATER]/4;
				iDmgElement[ELEMENT_FIRE]   = iDmgElement[ELEMENT_FIRE]/4;
				iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]/4;
			}

			int ret = GetParentEx()->RecvDamage(NULL, iDmgPhy, iDmgElement,true);

			if (CPlayerLogic::eDead== ret)
			{
				g_region->BuildEventDead(g_sendCmd,GetParentEx());
				return;
			} 
			else
			{	
				long jectId  = GetParentEx()->GetObjectId();
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_update_DeBuf);
				g_sendCmd->WriteLong(jectId);
				g_sendCmd->WriteShort((short)iDmgPhy);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_WATER]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_FIRE]);
				g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_POISON]);
				/*GetParentEx()->RecvCmd(g_sendCmd);*/
				GetParentEx()->m_scene->BroadcastCmd(GetParentEx()->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
			}
        }
    }
}

bool CSvSkillCreature::OnEffectCycle(SActorEffect* pEffect)
{
    if (m_bRun)
    {
        //MSG1("CSvSkillCreature::OnEffectCycle %s\n", pEffect->pAttr->szName);

        int iDmgPhy;
        int iDmgElement[MAX_ELEMENT_ATTR];
		
		//<ADD by Tianh>//最低一位是1就可以
        if (pEffect->pAttr->cTarget&0x01) 
        {
            // 先查找攻击范围内的所有人，再对所有的人都调用一次下面函数
            int i, iCnt, iMax;
            long lPos[3];
            DWORD dwTargetParty;
            CActorSkill* pTarget;
            CRegionCreature* pRegionCreature;
            const float* fPos = GetSelfPos();
            lPos[0] = fPos[0];
            lPos[1] = fPos[1];
            lPos[2] = fPos[2];
            CRegionObject* pFindCreature[SCENE_MAX_FIND_OBJECT];
			// TODO: Maybe someone don't have scene? louis
            iCnt = GetParentEx()->m_scene->GetAroundCreatureByTile(pFindCreature, lPos, pEffect->pAttr->wRange, GetParentEx(), PT_PATH, false);
            iMax = iCnt;
            if (pEffect->pAttr->cMaxObj>0)
            {
                iMax = (pEffect->pAttr->cMaxObj<iMax) ? pEffect->pAttr->cMaxObj : iMax;
            }
            for (i=0; i<iCnt && iMax>=0; i++)
            {
                pRegionCreature = (CRegionCreature*)(pFindCreature[i]);
                dwTargetParty = GetParentEx()->GetTargetParty(pRegionCreature);
                if ((dwTargetParty & pEffect->pAttr->dwATarget)==0)
                {
                    continue;
                }

                iMax --;
                pTarget = &(pRegionCreature->m_Skill);

				if (pTarget == this)
				{
					continue;
				}
	
				pEffect->pAttr->actCycle.Apply(this, pTarget, 0);
				 
                iDmgPhy = GetImmDamage();
                iDmgElement[ELEMENT_WATER]  = GetImmWaterDm();
                iDmgElement[ELEMENT_FIRE]   = GetImmFireDm();
                iDmgElement[ELEMENT_POISON] = GetImmPoisonDm();

				if (iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
				{
					if (((CRegionCreature*)(pFindCreature[i]))->IsUser())
					{
						iDmgPhy = iDmgPhy/4;
						iDmgElement[ELEMENT_WATER]  = iDmgElement[ELEMENT_WATER]/4;
						iDmgElement[ELEMENT_FIRE]   = iDmgElement[ELEMENT_FIRE]/4;
						iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]/4;
					}
					int ret = ((CRegionCreature*)(pFindCreature[i]))->RecvDamage(NULL, iDmgPhy, iDmgElement,true); //这里一定是范围攻击  设置为true
					
					if (CPlayerLogic::eDead== ret)//
					{
						g_region->BuildEventDead(g_sendCmd,((CRegionCreature*)(pFindCreature[i])));
						return true;
					}
					else
					{
						long jectId  = ((CRegionCreature*)(pFindCreature[i]))->GetObjectId();
						g_sendCmd->BeginWrite();
						g_sendCmd->WriteShort(r2c_update_DeBuf);
						g_sendCmd->WriteLong(jectId);
						g_sendCmd->WriteShort((short)iDmgPhy);
						g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_WATER]);
						g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_FIRE]);
						g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_POISON]);
						/*GetParentEx()->RecvCmd(g_sendCmd);*/
						((CRegionCreature*)(pFindCreature[i]))->m_scene->BroadcastCmd(((CRegionCreature*)(pFindCreature[i]))->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
					}
				}
			}
		}

        if (pEffect->pAttr->cTarget!=1) // 0 自己有效，1 范围有效，3 范围与自己都有效
        {
            pEffect->pAttr->actCycle.Apply(this, this, 0);

            iDmgPhy = GetImmDamage();
            iDmgElement[ELEMENT_WATER]  = GetImmWaterDm();
            iDmgElement[ELEMENT_FIRE]   = GetImmFireDm();
            iDmgElement[ELEMENT_POISON] = GetImmPoisonDm();

            if (iDmgPhy || iDmgElement[ELEMENT_WATER] || iDmgElement[ELEMENT_FIRE] || iDmgElement[ELEMENT_POISON])
            {		
				if (GetParentEx()->IsUser())
				{
					iDmgPhy = iDmgPhy/4;
					iDmgElement[ELEMENT_WATER]  = iDmgElement[ELEMENT_WATER]/4;
					iDmgElement[ELEMENT_FIRE]   = iDmgElement[ELEMENT_FIRE]/4;
					iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]/4;
				}
				int ret = GetParentEx()->RecvDamage(NULL, iDmgPhy, iDmgElement,true);

				if (CPlayerLogic::eDead== ret)
				{
					g_region->BuildEventDead(g_sendCmd,GetParentEx());
					return true;
				} 
				else
				{	
					long jectId  = GetParentEx()->GetObjectId();
					g_sendCmd->BeginWrite();
					g_sendCmd->WriteShort(r2c_update_DeBuf);
					g_sendCmd->WriteLong(jectId);
					g_sendCmd->WriteShort((short)iDmgPhy);
					g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_WATER]);
					g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_FIRE]);
					g_sendCmd->WriteShort((short)iDmgElement[ELEMENT_POISON]);
					/*GetParentEx()->RecvCmd(g_sendCmd);*/
					GetParentEx()->m_scene->BroadcastCmd(GetParentEx()->m_pos,DEFAULT_RANGE,g_sendCmd,NULL);
				}
			}
		}
	}

	return false;
}

void CSvSkillCreature::OnStudySkill(SSkill* pSkillAttr, bool bRefresh)
{
    //MSG1("CSvSkillCreature::OnStudySkill %s\n", pSkillAttr->szName);
    if (AddSkill(pSkillAttr->wID, 0, bRefresh))
	{
		GetParentEx()->m_core.Soul -= pSkillAttr->iSoul;
		GetParentEx()->m_core.SkillPoint -= pSkillAttr->iSP;
		GetParentEx()->GetPlayerLogic()->OnSkillNumChange();
    }
}

void CSvSkillCreature::OnClearSkill(bool bFigthSkill, int iRemoveSP)
{
    // GetParentEx()->m_core.SkillPoint += iRemoveSP;
	GetParentEx()->m_core.SkillPoint = GetLevelSkillPoint(GetParentEx()->m_core.Lev); 
	//if(GetParentEx()->m_task.GetTaskFinishNum(2159))
		//GetParentEx()->m_core.SkillPoint += MAX_LEVEL - 4;
}

void CSvSkillCreature::OnSkillModify()
{
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_refresh_skill);
    this->NetWriteSkill(g_sendCmd);
    GetParentEx()->RecvCmd(g_sendCmd);
}

void CSvSkillCreature::OnEffectModify()
{
    CRegionCreature* pCreature = GetParentEx();
    if (pCreature->IsUser())
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(r2c_refresh_effect);
        this->NetWriteEffect(g_sendCmd);
        pCreature->RecvCmd(g_sendCmd);
    }
}

//void CSvSkillCreature::OnResumeTimeModify(WORD wID,  WORD wResumeTime)
//{
//	CRegionCreature* pCreature = GetParentEx();
//	if (pCreature->IsUser())
//	{
//		g_sendCmd->BeginWrite();
//		g_sendCmd->WriteShort(r2c_refresh_wResumeTime);
//		g_sendCmd->WriteLong((long)wID);
//		g_sendCmd->WriteLong((long)wResumeTime);
//		pCreature->RecvCmd(g_sendCmd);
//	}
//}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
