
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
    // ����ٶ�
    ((CSvSkillCreature*)pReleaser)->GetParentEx()->CheckAttackSpeed(m_pAttr->iRTime, false);
}

void CSvSkill::OnUseAfter(CActorSkill* pReleaser)
{
    // ��龭�飬����������ģ�������
    m_wExp ++;
#ifdef _DEBUG
	// MSG4("����[%s]Lv%d ����[%d/%d]\n", m_pAttr->szName, m_pAttr->iLevel, m_wExp, m_pAttr->iLevelUpExp);
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

	// ������ҪMP
	if (pReleaser->GetMP()<m_pAttr->iMP)
	{
		m_pErrInfo = R(SMSG_MP_NOT_ENOUGH);
		return false;
	}

	//Tianh  10.03.12
	if (m_pAttr->dwType == 1) //��������1�Ƿ�������
	{
		// ������Ҫŭ����������ֵ
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
			// ��Ŀ�겻�ܱ�����
			m_pErrInfo = R(SMSG_SKILL_CANNOTATTACK);
			return false;
		}
	}

    if (m_pAttr->bAlwaysUsable==0 && !((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.IsAbiUseSkill())
    {
        m_pErrInfo = R(SMSG_SKILL_CANNOTUSE);
        return false;
    }

    // �����Ƿ�û�лָ�
    if (m_dwResumeTime>500) // ��������buffer
    {
        m_pErrInfo = R(SMSG_SKILL_RESUMEING);
        return false;
    }

    if (m_dwUserTime>500) // ��������buffer
    {
        m_pErrInfo = R(SMSG_SKILL_RESUMEING); // ��û��ʹ�ý���
        return false;
    }

	//Tianh �������CDδ��ȴ  ������ʹ�ü���
	if (m_pAttr->wClass == 1 || m_pAttr->wClass == 2 || m_pAttr->wClass == 3 || m_pAttr->wClass == 4 || m_pAttr->wClass == 5
		||m_pAttr->wClass == 6 || m_pAttr->wClass == 7 || m_pAttr->wClass == 8 ||m_pAttr->wClass == 9
		||m_pAttr->wClass == 10||m_pAttr->wClass == 11 ||m_pAttr->wClass == 12)
	{

		if (pReleaser->m_SchoolPublicCDTime>500) // ��������buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}
	else if (m_pAttr->wClass == 13)
	{
		if (pReleaser->m_WapPublicCDTime>500) // ��������buffer
		{
			m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
			return false;
		}
	}
	//else if (m_pAttr->wClass == 0)
	//{
	//	if (pReleaser->m_PropPublicCDTime>500) // ��������buffer
	//	{
	//		m_pErrInfo = R(SMSG_SKILL_PUBLICCD);
	//		return false;
	//	}
	//}
	else if (m_pAttr->wClass == 21)
	{
		if (pReleaser->m_beastPublicCDTime>500) // ��������buffer
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

// 0x0001 ����  1
// 0x0002 ˫��  2
// 0x0004 ��    4
// 0x0008 ��    8
// 0x0010 ���  16
// 0x0020 ����  32
// 0x0040 ����  64
// 0x0080 �ɽ�  128
// 0x0100 ����  256
// 0x0200 ����  512
// 0x0400 ��ҩ  1024
// 0x0800 �ڿ�  2048
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

/*Tianh ��������*/
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

void CSvSkillCreature::AddPhyAr(short sV)         // 15 ����������
{
    GetParentEx()->m_core.Armor.Temp += sV;
}

void CSvSkillCreature::AddPhyAb(short sV)         // 16 �����������
{
    GetParentEx()->m_core.DamageAbsorb.Temp += sV;
}

void CSvSkillCreature::AddAbsorbHP(short sV)      // 17 �����Ѫ
{
    GetParentEx()->m_core.AbsorbHp.Temp += sV;
}

void CSvSkillCreature::AddAbsorbMP(short sV)      // 18 �����ħ
{
    GetParentEx()->m_core.AbsorbMp.Temp += sV;
}

void CSvSkillCreature::AddDamageMin(short sV,int _DamageMode)     // 19 �����С�˺�
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

void CSvSkillCreature::AddDamageMax(short sV,int _DamageMode)     // 20 �������˺�
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

void CSvSkillCreature::AddSpeedAttack(short sV)   // 21 ��ӹ����ٶ�
{
    GetParentEx()->m_core.AttSpeed.Temp += sV;
}

void CSvSkillCreature::AddSpeedMove(short sV)     // 22 ����ƶ��ٶ�
{
    GetParentEx()->m_core.MovSpeed.Temp += sV;
}

void CSvSkillCreature::AddRateGold(short sV)     // 23 ��ӻ�ý�Ǯ����
{
    GetParentEx()->m_core.RateGold.Temp += sV;
}

void CSvSkillCreature::AddRateExp(short sV)      // 24 ��ӻ�þ��鱶��
{
    GetParentEx()->m_core.RateExp.Temp += sV;
}


//Tianh �˺���ʽ2 
//���˺�=���ܻ��˺�*��1+װ�����ӻ��˺�%����
//ˮ�˺�=����ˮ�˺�*��1+װ������ˮ�˺�%����
//���˺�=���ܶ��˺�*��1+װ�����Ӷ��˺�%����

void CSvSkillCreature::AddWaterDm(short sV,int _DamageMode)     // 25 ���ˮ�˺�
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

void CSvSkillCreature::AddFireDm(short sV,int _DamageMode)      // 26 ��ӻ��˺�
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

void CSvSkillCreature::AddPoisonDm(short sV,int _DamageMode)    // 27 ��Ӷ��˺�
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

void CSvSkillCreature::AddWaterAr(short sV)       // 28 ���ˮ����
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_WATER].Temp += sV;
}

void CSvSkillCreature::AddFireAr(short sV)        // 29 ��ӻ����
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_FIRE].Temp += sV;
}

void CSvSkillCreature::AddPoisonAr(short sV)      // 30 ��Ӷ�����
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_POISON].Temp += sV;
}

void CSvSkillCreature::AddWaterAb(short sV)       // 31 ���ˮ����
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_WATER].Temp += sV;
}

void CSvSkillCreature::AddFireAb(short sV)        // 32 ��ӻ�����
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_FIRE].Temp += sV;
}

void CSvSkillCreature::AddPoisonAb(short sV)      // 33 ��Ӷ�����
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_POISON].Temp += sV;
}

void CSvSkillCreature::AddAttackRate(short sV)   // 34 �������
{
    GetParentEx()->m_core.Attack.Temp += sV;
}

void CSvSkillCreature::AddDodge(short sV)        // 35 ��Ӷ��
{
    GetParentEx()->m_core.Dodge.Temp += sV;
}

void CSvSkillCreature::AddAttackRange(short sV)  // 36 ��ӹ�������
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

void CSvSkillCreature::AddPhyArR(float fV)  // 15 ����������
{
    GetParentEx()->m_core.Armor.Rate += fV;
}

void CSvSkillCreature::AddPhyAbR(float fV)  // 16 �����������
{
    GetParentEx()->m_core.DamageAbsorb.Rate += fV;
}

void CSvSkillCreature::AddAbsorbHPR(float fV)  // 17 �����Ѫ
{
    GetParentEx()->m_core.AbsorbHp.Temp += fV;
}

void CSvSkillCreature::AddAbsorbMPR(float fV)  // 18 �����ħ
{
    GetParentEx()->m_core.AbsorbMp.Temp += fV;
}

void CSvSkillCreature::AddDamageMinR(float fV)  // 19 �����С�˺�
{
    GetParentEx()->m_core.DamageMin.Rate += fV;
}

void CSvSkillCreature::AddDamageMaxR(float fV)  // 20 �������˺�
{
    GetParentEx()->m_core.DamageMax.Rate += fV;
}

void CSvSkillCreature::AddSpeedAttackR(float fV)  // 21 ��ӹ����ٶ�
{
    GetParentEx()->m_core.AttSpeed.Rate += fV;
}

void CSvSkillCreature::AddSpeedMoveR(float fV)  // 22 ����ƶ��ٶ�
{
    GetParentEx()->m_core.MovSpeed.Rate += fV;
}

void CSvSkillCreature::AddRateGoldR(float fV)  // 23 ��ӻ�ý�Ǯ����
{
    GetParentEx()->m_core.RateGold.Rate += fV;
}

void CSvSkillCreature::AddRateExpR(float fV)  // 24 ��ӻ�þ��鱶��
{
    GetParentEx()->m_core.RateExp.Rate += fV;
}

void CSvSkillCreature::AddWaterDmR(float fV)  // 25 ���ˮ�˺�
{
    GetParentEx()->m_core.ElementDamage[ELEMENT_WATER].Rate += fV;
}

void CSvSkillCreature::AddFireDmR(float fV)  // 26 ��ӻ��˺�
{
    GetParentEx()->m_core.ElementDamage[ELEMENT_FIRE].Rate += fV;
}

void CSvSkillCreature::AddPoisonDmR(float fV)  // 27 ��Ӷ��˺�
{
    GetParentEx()->m_core.ElementDamage[ELEMENT_POISON].Rate += fV;
}

void CSvSkillCreature::AddWaterArR(float fV)  // 28 ���ˮ����
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_WATER].Rate += fV;
}

void CSvSkillCreature::AddFireArR(float fV)  // 29 ��ӻ����
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_FIRE].Rate += fV;
}

void CSvSkillCreature::AddPoisonArR(float fV)  // 30 ��Ӷ�����
{
    GetParentEx()->m_core.ElementArmor[ELEMENT_POISON].Rate += fV;
}

void CSvSkillCreature::AddWaterAbR(float fV)  // 31 ���ˮ����
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_WATER].Rate += fV;
}

void CSvSkillCreature::AddFireAbR(float fV)  // 32 ��ӻ�����
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_FIRE].Rate += fV;
}

void CSvSkillCreature::AddPoisonAbR(float fV)  // 33 ��Ӷ�����
{
    GetParentEx()->m_core.ElementAbsorb[ELEMENT_POISON].Rate += fV;
}

void CSvSkillCreature::AddAttackRateR(float fV) // 34 �������
{
    GetParentEx()->m_core.Attack.Rate += fV;
}

void CSvSkillCreature::AddDodgeR(float fV) // 35 ��Ӷ��
{
    GetParentEx()->m_core.Dodge.Rate += fV;
}

void CSvSkillCreature::AddAttackRangeR(float fV) // 36 ��ӹ�������
{
    GetParentEx()->m_core.AttackRange.Rate += fV;
}

void CSvSkillCreature::AddCriticalR(float fV) // 36 ��ӱ���
{
    GetParentEx()->m_core.CriticalHit.Rate += fV;
}

void CSvSkillCreature::AddResilience(float fV) //88 ����ɱ
{
	GetParentEx()->m_core.ReCriticalHit.Temp += fV;
}

void CSvSkillCreature::Rebirth(int iHPrate,int iMPrate) //91 ����
{
	/*GetParentEx()->Rebirth(4,iHPrate,iMPrate);*/

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_rebirth_user);
	g_sendCmd->WriteLong(GetParentEx()->GetObjectId());
	g_sendCmd->WriteLong(iHPrate);
	g_sendCmd->WriteLong(iMPrate);
	GetParentEx()->RecvCmd(g_sendCmd);
}

void CSvSkillCreature::ExtractHpR(int iHPrate,int maxHP) //�ٷֱȳ�ȡĿ��Ѫ��������಻�ܶ���һ���̶�ֵ
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

void CSvSkillCreature::SeriesDamage(int Series,int damageR)//���Ӽ���ϵ���˺�
{
	GetParentEx()->m_SkillSeries += Series;          //Ҫ�����˺��ļ���ϵ��
	GetParentEx()->m_SkilldamageR += damageR;        //Ҫ�����˺�
	GetParentEx()->AddSkillSeries();
}

//Tianh  
void CSvSkillCreature::AddAbsorptionMaxDamage(float sV) //�����˺����ֵ
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
//Tianh �����˺���
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

void CSvSkillCreature::AddReceiveDamageSkillR(float fV)//���ӽ��ռ����˺�����
{	
	GetParentEx()->m_ReceiveDamageSkillR += fV;
}

void CSvSkillCreature::StateMove(short sV) // 78 �Ƿ�����ƶ�
{
    if (sV>0)      GetParentEx()->m_core.AddMove();
    else if (sV<0) GetParentEx()->m_core.SubMove();
}

void CSvSkillCreature::StateDodge(short sV) // 79 �Ƿ��������
{
    if (sV>0)      GetParentEx()->m_core.AddDodge();
    else if (sV<0) GetParentEx()->m_core.SubDodge();
}

void CSvSkillCreature::StateUseSkill(short sV) // 80 �Ƿ����ʹ�ü���
{
    if (sV>0)      GetParentEx()->m_core.AddUseSkill();
    else if (sV<0) GetParentEx()->m_core.SubUseSkill();
}

void CSvSkillCreature::StateNAttack(short sV) // 81 �Ƿ������ͨ����
{
    if (sV>0)      GetParentEx()->m_core.AddNormalAttack();
    else if (sV<0) GetParentEx()->m_core.SubNormalAttack();
}

void CSvSkillCreature::StateResist(short sV) // 82 ����״̬�ֿ�
{
    if (sV>0)      GetParentEx()->m_core.AddAbiResist(sV);
    else if (sV<0) GetParentEx()->m_core.RemoveAbiResist(-sV);
}

void CSvSkillCreature::Hate(short sV) // 83 ����ĳ�޶�
{
    GetParentEx()->m_core.Hate += sV;
}

void CSvSkillCreature::HateRate(short sV) // 84 ��ޱ���
{
    GetParentEx()->m_core.HateRate += sV;
}

void CSvSkillCreature::AddHate(short sV, CDummyActorSkill *pReleaser) // 85 ���ӹ�����
{
    CRegionCreature* pCra = GetParentEx();
    if (pCra->m_ai)
    {
        pCra->m_ai->OnBeAttacked(((CSvSkillCreature*)pReleaser)->GetParentEx(), sV);
    }
}

void CSvSkillCreature::AddSkillCriRate(short sV, CDummyActorSkill *pReleaser) // 86 ���ܱ�����
{
    CRegionCreature* pCra = GetParentEx();
    pCra->m_core.SkillCriRate += sV;
}

void CSvSkillCreature::AddSkillCriDamageR(short sV, CDummyActorSkill *pReleaser) // 87 ���ܱ����˺�
{
    CRegionCreature* pCra = GetParentEx();
    pCra->m_core.SkillCriDamageRate += sV;
}

void CSvSkillCreature::ChangeWeapon()
{
	GetParentEx()->ChangeWeapon();
}

void CSvSkillCreature::MomentMove(int sV) //˲���ƶ�
{
	GetParentEx()->DoTeleportSkill(sV);
}

void CSvSkillCreature::ChangeBody(int ModelID) //����
{
	if (!GetParentEx()->IsChengBody()) //����ڷǱ���״̬
	{
		GetParentEx()->ChangeBoby(ModelID);
	}
	else
	{
		GetParentEx()->ChangeBoby(GetParentEx()->m_dwLastModelID);
	}
	
}

void CSvSkillCreature::BackHome()  //�س�
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

void CSvSkillCreature::AddResistZXC(int sV) //�ֿ����� 
{
	GetParentEx()->m_core.ReSkillCritical.Temp += sV;
}

void CSvSkillCreature::AddResistSwim(int sV) //�ֿ�ѣ�� 
{
	GetParentEx()->m_ResistSwim += sV;
}

void CSvSkillCreature::AddResistInertia(int sV) //�ֿ��ٶ� 
{
	GetParentEx()->m_ResistLethargy += sV;
}

void CSvSkillCreature::AddResistTrance(int sV) //�ֿ���˯
{
	GetParentEx()->m_ResistBleed += sV;
}

void CSvSkillCreature::AddResistBleed(int sV) //�ֿ���Ѫ
{
	GetParentEx()->m_ResistBluntness += sV;
}

void CSvSkillCreature::TeamShareDamage(int sV)//���ѷֵ��˺�
{
	GetParentEx()->m_TeamShareDamage += sV;
}

void CSvSkillCreature::Hprelay(int sV,CActorSkill *pTarget)//���Լ���ǰ��HP���ݸ�����
{
	int hp = GetParentEx()->m_core.GetHp() * sV/100;
	GetParentEx()->m_core.AddHp(-hp);
	if (pTarget)
	{
		((CSvSkillCreature*)pTarget)->GetParentEx()->m_core.AddHp(hp);
	}
}
void CSvSkillCreature::Mprelay(int sV,CActorSkill *pTarget)//���Լ���ǰ��MP���ݸ�����
{
	int mp = GetParentEx()->m_core.GetMp() * sV/100;
	GetParentEx()->m_core.AddMp(-mp);
	if (pTarget)
	{
		((CSvSkillCreature*)pTarget)->GetParentEx()->m_core.AddMp(mp);
	}
}

void CSvSkillCreature::SkillhpRecover(int sV)      //����ÿ��ָ�Ѫ��
{
	GetParentEx()->m_SkillhpRecover += sV;
} 

void CSvSkillCreature::SkillmpRecover(int sV)    //����ÿ��ָ�����
{
	GetParentEx()->m_SkillmpRecover += sV;
}

//void CSvSkillCreature::ConsumeNpc(int sV)  //�����ٻ�NPC�ĸ���
//{
//	GetParentEx()->DeleteCallNpc(sV);
//}

void CSvSkillCreature::UltimateChange(int sV)//�ռ�����
{
	GetParentEx()->UltimateChange(sV);
}

void CSvSkillCreature::MonomersCgroup(int sV)//�����Ⱥ���չ� ���뷶Χ
{
	GetParentEx()->m_ScopeoDinaryInjury += sV;
}

void CSvSkillCreature::PlayerHiding(int sV)//�����ܵ�ʵ��
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

void CSvSkillCreature::PlayerImmuno(int sV) //+1 �������� + 2�����к�  +3��������
{
	if (sV == 1)//�Ƿ���������BUF
	{
		(&(GetParentEx()->m_Skill))->ImmunoUseful = true;
	}
	else if (sV == -1)
	{
		(&(GetParentEx()->m_Skill))->ImmunoUseful = false;
	}
	else if (sV == 2) //�Ƿ������к�BUF
	{
		(&(GetParentEx()->m_Skill))->ImmunoHarmful = true;
	}
	else if (sV == -2)
	{
		(&(GetParentEx()->m_Skill))->ImmunoHarmful = false;
	}
	else if (sV == 3)//�Ƿ����߶���BUF
	{
		(&(GetParentEx()->m_Skill))->ImmunoAll = true;
	}
	else if (sV == -3)
	{
		(&(GetParentEx()->m_Skill))->ImmunoAll = false;
	}

}

void CSvSkillCreature::AddSkillCriDamage(int sV)// ���ܱ����˺��ӳ�
{
	GetParentEx()->m_core.SkillCriDamage += sV;
}

void CSvSkillCreature::AddCriticalDamage(int sV)// ��ͨ���������˺�
{
	GetParentEx()->m_core.CriticalDamage += sV;
}

void CSvSkillCreature::AddCriticalDamageR(int sV)// ��ͨ�����������ʣ����Ǳ����˺�����
{
	GetParentEx()->m_core.CriticalDamageR += sV;
}

void CSvSkillCreature::AddShieldMpR(int sV)
{
	//Tianh �置�ĺ����ܵ��ж���Ҫ  Ѫ�ܵ�֧��
	if (GetParentEx()->m_core.IsShieldMpR)
	{
		GetParentEx()->m_core.SetShieldMpR(0);
		GetParentEx()->m_core.SetShieldHp(0);

		GetParentEx()->m_core.IsShieldMpR = false;

		GetParentEx()->m_Skill.ClearEffByClass(18); //ȥ��ħ����Ч��(ʥ��)
	}
	else
	{
		GetParentEx()->m_core.SetShieldMpR(sV);
		GetParentEx()->m_core.SetShieldHp(100);

		GetParentEx()->m_core.IsShieldMpR = true;
	}
}

void CSvSkillCreature::AddAbsorptionHpR(int sV) //�ܵ����˺�ת��ΪHP��
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
		GetParentEx()->CreateAllUnderlingNpc(NpcId,PLAYER_CALL,Num);//����ٻ�
	} 
}

void CSvSkillCreature::ClearUseful()
{
    std::list<SActorEffect>::iterator it;
    for (it=m_listEffect.begin(); it!=m_listEffect.end();)
    {
        if ((*it).pAttr->cAType==1) // ����Ч��
        {
            OnEffectClose(&(*it)); // �رպ��һ��Ҫɾ��
			it = m_listEffect.erase(it);
			continue;
		}
		it ++;
	}
	OnEffectModify();
}

//<add by Tianh> 
//��������������ɾ��
void CSvSkillCreature::SpecialClearUseful()
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->cAType==1) // ����Ч��
		{
			if ((*it).pAttr->isCleared)
			{
				OnEffectClose(&(*it)); // �رպ��һ��Ҫɾ��
				it = m_listEffect.erase(it);
				continue;
			}
		}
		it ++;
	}
	OnEffectModify();
}

//<add by Tianh> 
//�ݵ�ʱ������ɾ��
void CSvSkillCreature::TopoClearUseful()
{
	std::list<SActorEffect>::iterator it;
	for (it=m_listEffect.begin(); it!=m_listEffect.end();)
	{
		if ((*it).pAttr->isCleared)
		{
			OnEffectClose(&(*it)); // �رպ��һ��Ҫɾ��
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
		if ((*it).pAttr->cAType==2) // �к�Ч��
		{
			OnEffectClose(&(*it)); // �رպ��һ��Ҫɾ��
			it = m_listEffect.erase(it);
			continue;
		}
		it ++;
	}
	OnEffectModify();
}

void CSvSkillCreature::MoveParent(CActorSkill *pReleaser)//��Ŀ��˲���ƶ����Լ��Ա�
{
	if (!GetParentEx()->GetPlayerLogic()->IsLive()) //����״̬�����ƶ�
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
		GetParentEx()->DeleteCallNpc(1); //ɾ��1���ٻ�NPC
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

	//Tianh ���ǿ��
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
		// ��鼼�ܵ�ʹ�ö���
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
	// �Ȳ��ҹ�����Χ�ڵ������ˣ��ٶ����е��˶�����һ�����溯��
	int i, iCnt;
	long lPos[3];
	lPos[0] = pPos[0];
	lPos[1] = pPos[1];
	lPos[2] = pPos[2];
	bool IsFlashMove = false;

	unsigned short wMyARange;
	unsigned char cMyAMaxObj;
	CActorSkill::SActorPassive* pStrengthenSkill = NULL;

	//Tianh ���ǿ��
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


   //Tianh  ���˲���ƶ�����
	if (pSkillAttr->isFlashMove)
	{
		IsFlashMove = true;
	}
	if (IsFlashMove)
	{
		///*˲���ƶ�*/
		//long mPos[3]; //�������ڵ�����
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
		//Tianh ����ͷ��߲����˻����˵��� �����ͷŵļ��������漼�ܣ��������ͷ�
		bool bCondition = (!((GetParentEx()->IsUser()) || (GetParentEx()->IsUserPet()) || (GetParentEx()->IsCallNpc()))) || (pSkillAttr->bABad==0);
		CRegionCreature* pTargetCreature;
		for (i=0; i<iCnt; i++)
		{
			if(pFindCreature[i]->m_type==OBJECT_TYPE_CREATURE || pFindCreature[i]->m_type==OBJECT_TYPE_NPC_COMBATIVE)
			{
				pTargetCreature = (CRegionCreature*)(pFindCreature[i]);
				// ����Ƿ�Χ���� ���� �Է����ˣ�����Ч��������Ϊ��û�д���PK��������ᵼ��Ⱥ����PK����

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
            return false; // �к�����Ҫ����Ƿ���ʹ��
        }
    }
	//else
	//{
	//	if (pTarget->ImmunoUseful || pTarget->ImmunoAll) //����Է���������BUF����ȫ������
	//	{
	//		return false;
	//	}
	//}

    // ��鼼�ܵ�ʹ�ö���
    DWORD dwTargetParty = pSource->GetTargetParty(pTarget);

	//4.24 LeiJun ���ѷ�ʩ�ż��ܵ����
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

//Tianh ��Ѫ��������
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
	pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode); // ��һ��������NULL����Ϊ��thisҲû���ã�û�м���������Լ���
	pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);

	delta = (((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMax()) - (((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMin());
	if(delta == 0)
		phyDmg = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMin();
	else
		phyDmg = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.GetDamageMin() + GetRand(delta);

	eleDmg[ELEMENT_WATER] = ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.ElementDamage[ELEMENT_WATER].GetValue();

	if(eleDmg[ELEMENT_WATER] >= 5)
		eleDmg[ELEMENT_WATER] = GetRand(eleDmg[ELEMENT_WATER]*1.1,eleDmg[ELEMENT_WATER]*0.9);

	//�Ƿ񱬻�
	bool bCriDamage = false;
	bCriDamage = ((CSvSkillCreature*)pReleaser)->GetParentEx()->IsSkillCritical(pSkillAttr->dwAHitRate, this->GetParentEx());
	if (bCriDamage)
	{
		((CSvSkillCreature*)pReleaser)->GetParentEx()->GetSkillCriDamage(icDmgPhy, icDmgElement, this->GetParentEx());
		phyDmg	+= icDmgPhy + phyDmg * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;
		eleDmg[ELEMENT_WATER]  += icDmgElement[ELEMENT_WATER] + eleDmg[ELEMENT_WATER] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;;
	}
	
	//���Է���������   ���������ж�
	int addHp = phyDmg+eleDmg[ELEMENT_WATER]/4; //���� ���Ƶ�ˮ�˺�/4
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
    pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode); // ��һ��������NULL����Ϊ��thisҲû���ã�û�м���������Լ���
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

	//Tianh  ����������������
	if (GetParentEx()->m_TeamShareDamage)
	{
		short ID = 0;
		std::vector<CRegionCreature*> ActorSkill;//�洢�����������ѵ�ָ������
		CActorSkill *pActorSkill = FindActorSkill(GetParentEx()->m_oId);
		if (pActorSkill)
		{
			ID = pActorSkill->FindEffectbyClass(LIFE_CHAINS);
		}
		//�ж϶ӵ����Ƿ���1�����ϵĶ��ѻ�����ͬ��BUF״̬�����򲻷ֵ��˺�
		//��������ֻ�Զ���������ͬBUF������Ч��
		 
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
							//������Լ�
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
			//û�ж�����BUF״̬   ���ֵ�
			OnSkillUseCalculating(pSkillAttr,pReleaser,iDmgPhy,iDmgElement,bCriDamage,bResult);
		}
		else
		{
			int viDmgElement[MAX_ELEMENT_ATTR] = {0,0,0};
			int kiDmgElement[MAX_ELEMENT_ATTR] = {0,0,0};
			//��ʼ�ֵ��˺�
			float viDmgPhyR = (GetParentEx()->m_TeamShareDamage)/100.0f;
		    int viDmgPhy = 	iDmgPhy * viDmgPhyR;
			viDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER] * viDmgPhyR;
			viDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE] * viDmgPhyR;
			viDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON] * viDmgPhyR;
			//��k-1���������ֵ��˺�
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
				pSkillAttr->actSelfTemp.Apply(NULL, pReleaser, 0,false,pSkillAttr->DamageMode); // ��һ��������NULL����Ϊ��thisҲû���ã�û�м���������Լ���
				pSkillAttr->actTargetTemp.Apply(pReleaser, this, 0,false,pSkillAttr->DamageMode);
			}
			SetParentEx(pCreature);

			//�������Լ�������˺�
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
		case SKILL_STATE_XUANYUN:     //ѣ��
			{
				if (GetParentEx()->m_ResistSwim)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistSwim);
				}
			}
			break;
		case SKILL_STATE_HUNSHUI:      //��˯      
			{
				if (GetParentEx()->m_ResistLethargy)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistLethargy);
				}
			}
			break;
		case SKILL_STATE_LIUXUE:       //��Ѫ
			{
				if (GetParentEx()->m_ResistBleed)
				{
					viStateRate = (pSkillAttr->iStateRate) - (GetParentEx()->m_ResistBleed);
				}
			}
			break;
		case SKILL_STATE_CHIDUN:      //�ٶ� 
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
	//Tianh ����˺�����ķ�ʽ
	if (pSkillAttr->bADamage) // ��������к��ļ��ܾ�ȫ�����У�����͵�����ȥ��������
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
				//Tianh ����˱����˺��ӳ�SkillCriDamage

				iDmgPhy						+= icDmgPhy + iDmgPhy * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;

				iDmgElement[ELEMENT_WATER]  += icDmgElement[ELEMENT_WATER] + icDmgElement[ELEMENT_WATER] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;
				iDmgElement[ELEMENT_FIRE]   += icDmgElement[ELEMENT_FIRE] + icDmgElement[ELEMENT_FIRE] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;
				iDmgElement[ELEMENT_POISON] += icDmgElement[ELEMENT_POISON] + icDmgElement[ELEMENT_POISON] * ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamageRate/100.0f + ((CSvSkillCreature*)pReleaser)->GetParentEx()->m_core.SkillCriDamage;

				//���������1�Ļ���ֻ����Ԫ���˺� //Tianh  ��������˺���ʽΪ1 ��ֱ���ò��μӼ����ֵ
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
					case SKILL_STATE_XUANYUN:     //ѣ��
						{
							if (GetParentEx()->m_ResistSwim)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistSwim);
							}
						}
						break;
					case SKILL_STATE_HUNSHUI:      //��˯      
						{
							if (GetParentEx()->m_ResistLethargy)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistLethargy);
							}
						}
						break;
					case SKILL_STATE_LIUXUE:       //��Ѫ
						{
							if (GetParentEx()->m_ResistBleed)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistBleed);
							}
						}
						break;
					case SKILL_STATE_CHIDUN:      //�ٶ� 
						{
							if (GetParentEx()->m_ResistBluntness)
							{
								viStateRate = (pStrengthenSkill->pAttr->iStateRate) - (GetParentEx()->m_ResistBluntness);
							}
						}
						break;
					}
				}

				// ȡ��״̬����		
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

		//�޵�
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
				|| pSkillAttr->bABad) // add by janhail at 20051207 ����������˺������ǻ����ܣ��ͼ����˺�(��Ӱ������)
			{
				bLive = GetParentEx()->GetPlayerLogic()->IsLive();


				//����Է��н��ռ����˺�����
				if (GetParentEx()->m_ReceiveDamageSkillR)
				{
					iDmgPhy = iDmgPhy * (GetParentEx()->m_ReceiveDamageSkillR);
					iDmgElement[ELEMENT_WATER] = iDmgElement[ELEMENT_WATER]*(GetParentEx()->m_ReceiveDamageSkillR);
					iDmgElement[ELEMENT_FIRE] = iDmgElement[ELEMENT_FIRE]*(GetParentEx()->m_ReceiveDamageSkillR);
					iDmgElement[ELEMENT_POISON] = iDmgElement[ELEMENT_POISON]*(GetParentEx()->m_ReceiveDamageSkillR);
				}

				////Tianh   �߻�������
				//if (((CSvSkillCreature*)pReleaser)->GetParentEx()->IsUser())
				//{
				//	RtCoreLog().Debug("���������ܡ�:'%s'�Ե�����������˺���: %d\n",pSkillAttr->szName,iDmgPhy);
				//	RtCoreLog().Debug("���������ܡ�:'%s'�Ե�����ɡ�ˮ���˺���: %d\n",pSkillAttr->szName,iDmgElement[0]);
				//	RtCoreLog().Debug("���������ܡ�:'%s'�Ե�����ɡ����˺���: %d\n",pSkillAttr->szName,iDmgElement[1]);
				//	RtCoreLog().Debug("���������ܡ�:'%s'�Ե�����ɡ������˺���: %d\n",pSkillAttr->szName,iDmgElement[2]);

				//}
				//else if (((CSvSkillCreature*)pReleaser)->GetParentEx()->IsMonster())
				//{
				//	RtCoreLog().Debug("���˵ĳ�������ܡ�:'%s'������������˺���: %d\n",pSkillAttr->szName,iDmgPhy);
				//	RtCoreLog().Debug("���˵ĳ�������ܡ�:'%s'������ɡ�ˮ���˺���: %d\n",pSkillAttr->szName,iDmgElement[0]);
				//	RtCoreLog().Debug("���˵ĳ�������ܡ�:'%s'������ɡ����˺���: %d\n",pSkillAttr->szName,iDmgElement[1]);
				//	RtCoreLog().Debug("���˵ĳ�������ܡ�:'%s'������ɡ������˺���: %d\n",pSkillAttr->szName,iDmgElement[2]);

				//}
				//else if (((CSvSkillCreature*)pReleaser)->GetParentEx()->IsUserPet())
				//{
				//                RtCoreLog().Debug("���ĳ�������ܡ�:'%s'�Ե�����������˺���: %d\n",pSkillAttr->szName,iDmgPhy);
				//	RtCoreLog().Debug("���ĳ�������ܡ�:'%s'�Ե�����ɡ�ˮ���˺���: %d\n",pSkillAttr->szName,iDmgElement[0]);
				//	RtCoreLog().Debug("���ĳ�������ܡ�:'%s'�Ե�����ɡ����˺���: %d\n",pSkillAttr->szName,iDmgElement[1]);
				//	RtCoreLog().Debug("���ĳ�������ܡ�:'%s'�Ե�����ɡ������˺���: %d\n",pSkillAttr->szName,iDmgElement[2]);
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
		if (pSkillAttr->bABad) // ��������к��ļ��ܾ�ȫ�����У�����͵�����ȥ��������
		{
		GetParentEx()->OnBeAttacked(((CSvSkillCreature*)pReleaser)->GetParentEx(), iDmgPhy);
		}
		}
		*/
	}else
	{
		// �������������û�����壬���д��
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
		
		//<ADD by Tianh>//���һλ��1�Ϳ���
        if (pEffect->pAttr->cTarget&0x01) 
        {
            // �Ȳ��ҹ�����Χ�ڵ������ˣ��ٶ����е��˶�����һ�����溯��
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
					int ret = ((CRegionCreature*)(pFindCreature[i]))->RecvDamage(NULL, iDmgPhy, iDmgElement,true); //����һ���Ƿ�Χ����  ����Ϊtrue
					
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

        if (pEffect->pAttr->cTarget!=1) // 0 �Լ���Ч��1 ��Χ��Ч��3 ��Χ���Լ�����Ч
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
