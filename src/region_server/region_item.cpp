#include "region.h"
#include "rs_national_war.h"
#include "PlayerLogic.h"

CRegionItemSaleStat g_ItemSaleStat;


static float s_fTwoHandWeapon_Wea1Scale = 1.00001; // 拿双手武器时1武器的倍率
//static float s_fTwoHandWeapon_Wea2Scale = 0.20001; // 拿双手武器时2武器的倍率

CRegionItemContainer::CRegionItemContainer(CItemManager *pManager, CRegionCreature* pMaster)
:CItemContainerBase(pManager, &(pMaster->m_core))
{
	m_pMaster = pMaster;
    m_lastTwoBianRate_Damage_Wea1 = 0.0f;
    m_lastTwoBianRate_Damage_Wea2 = 0.0f;
    m_lastTwoBianRate_Attack_Wea1 = 0.0f;
    m_lastTwoBianRate_Attack_Wea2 = 0.0f;

	m_bLocked = false;
	m_dwLastLockTick = 0;
}

CRegionItemContainer::~CRegionItemContainer()
{
}

void CRegionItemContainer::LockOperate(bool bLock)
{
	m_bLocked = bLock;

	if (bLock)
	{
		m_dwLastLockTick = rtGetMilliseconds();
	}
	else 
	{
		m_dwLastLockTick = 0;
	}
}

ETradableResult CRegionItemContainer::CanItemTrade(char bCommonBag, int page, int i, int j)
{
	SItemID item;
	if (bCommonBag == 1)
	{
		item = m_Bag.GetItem(page, i, j);
	}
	else if (bCommonBag == 2)
	{
		item = m_PetBag.GetItem(page, i, j);
	}
	else
	{
		item = m_MissionBag.GetItem(page, i, j);
	}

	if (!ItemID_IsValid(item))
		return TR_CanNot_Unkown;

	if (!ItemCanTrade(item))
		return TR_CanNot_Essence;

	if (ItemIsPet(item.type))
	{
		if (m_pMaster->m_pet && m_pMaster->m_pet->m_dwPetItemOID==item.id)
			return TR_CanNot_Pet;
	}
    if (ItemIsOther_Hero(item.type))
    {
        if (m_pMaster->m_hero)
            return TR_CanNot_Pet;
    }

	return TR_Can;
}

ETradableResult CRegionItemContainer::CanItemTrade(DWORD itemID)
{
	int page, i, j;
	if (m_Bag.Find(itemID, &page, &i, &j))
	{
		return CanItemTrade(1, page, i, j);
	}
	else if (m_MissionBag.Find(itemID, &page, &i, &j))
	{
		return CanItemTrade(0, page, i, j);
	}
	else if (m_PetBag.Find(itemID, &page, &i, &j))
	{
		return CanItemTrade(2, page, i, j);
	}
	else 
	{
		return TR_CanNot_Unkown;
	}
}

bool CRegionItemContainer::IsOperateLocked()
{
	if (!m_bLocked || m_dwLastLockTick<=0)
	{
		return false;
	}
	else 
	{
		DWORD now = rtGetMilliseconds();
		if (now-m_dwLastLockTick > 5000)
		{
			m_dwLastLockTick = 0;
			m_bLocked = false;
			return false;
		}
		else 
		{
			return true;
		}
	}
}

bool CRegionItemContainer::CheckEquip(EEquipParts vPart, SItemID& item, SCreature &cre)
{
	if (!m_pMaster)
		return false;
	if (!CItemContainerBase::CheckEquip(vPart, item, cre))
		return false;

    // 是否过期
    if (item.TimeLimit)
    {
        DWORD now = time(NULL);
        if (now > item.TimeLimit)
        {
            return false;
        }
    }

	// 装备双鞭时检查是否学习了技能
	if (vPart==WEAPON_1 && ItemIsWeapon_Pestle(item))
	{
		SItemID item2 = GetEquipItem(WEAPON_2, true);
		if (ItemIsWeapon_Pestle(item2))
		{
			if ( !(m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_DAMAGE) || m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_ATTACK)) )
				return false;
		}
	}
	else if (vPart==WEAPON_2 && ItemIsWeapon_Pestle(item))
	{
		SItemID item1 = GetEquipItem(WEAPON_1, true);
		if (ItemIsWeapon_Pestle(item1))
		{
            if ( !(m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_DAMAGE) || m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_ATTACK)) )
                return false;
		}
	}
    else if (vPart == CHARM && item.params[ITEMSHIPIN_PARAM_LIFE] != 0)
    {
        DWORD now = time(NULL);
        DWORD tLife = item.params[ITEMSHIPIN_PARAM_LIFE];
        if (now >= tLife)
            return false;
    }

	return true;
}

bool CRegionItemContainer::CheckUse(SItemID& item, SCreature &cre) const
{
    if (!CItemContainerBase::CheckUse(item, cre))
        return false;

    // 是否过期
    if (item.TimeLimit)
    {
        DWORD now = time(NULL);
        if (now > item.TimeLimit)
        {
            return false;
        }
    }

    SItemBase* pItemBase = g_region->m_pItemManager->GetItem(item.type);
    BOOL_ENSURE(pItemBase);

    switch (pItemBase->timeLimit)
    {
    case IUL_Free:
        break;
    case IUL_FeodWar:
        {
            // 城战时间内，并且在城战场境内才可以使用
            if (!g_RegionCastleWarMgr.IsCastleWarTime(m_pMaster))
            {
                return false;
            }
            else
            {
                if (!g_RegionCastleWarMgr.InCastleWarArea(m_pMaster))
                    return false;
            }
        }
        break;
    case IUL_NationalWar:
        {
            if (!g_WarfieldMgr.IsInWarfield(m_pMaster))
                return false;
        }
        break;
    default:
        break;
    }

    if (ItemIsOther_Spawn(item.type))
    {
        // 使用期限
        DWORD now = time(NULL);
        DWORD lLife = item.params[ITEMSPAWN_PARAM_LIFE];
        if (lLife != 0 && now >= lLife)
        {
            return false;
        }
    }
	if (ItemIsEXPBottle(item))
	{
		SItemID neckItem = GetEquipItem(CItemContainerBase::CHARM, true);
		int neckId = g_region->m_pItemManager->GetIndexFromType(neckItem.type);
		if(neckId == 4306 || neckId == 4516)
		{
			m_pMaster->SendSystemMessage(R(MSG_USE_EXPITEM_FAILD));
			return false;
		}
		if (!m_pMaster->CheckUseExpItem())
		{
			m_pMaster->SendSystemMessage(R(MSG_USE_EXPITEM_FAILD1));
			return false;
		}
	}
    return true;
}

bool CRegionItemContainer::ItemCanChangeAttrib(SItemID &item)
{
	if (ItemIsShipin(item) && (item.color>=ItemColor_Golden))
		return true;
	else 
		return false;
}

bool CRegionItemContainer::ItemHaveToRecomputeAttrib(SItemID &item)
{
	if (item.color != ItemColor_White)
		return true;
	else 
		return false;
}

void CRegionItemContainer::OnCreatureAttribChanged()
{
	for (int part=0; part<MAX_EQUIP_PARTS; part++)
	{
		OnCreatureAttribChanged((EEquipParts)part);
	}
}

void CRegionItemContainer::OnCreatureAttribChanged(EEquipParts part)
{
	SItemID itemClone = GetEquipItem(part, true);
	if (ItemID_IsValid(itemClone))
	{
		if (!CheckEquip(part, itemClone, m_pMaster->m_core))
		{
			if (m_bEquipValid[part])
			{
				CItemCmdBuilder_Svr::Build_r2c_equip_item_state(part, 0);
				m_pMaster->RecvCmd(g_sendCmd);
				//                LOG1("The item (part=%d) become INVALID\n", part);
				m_bEquipValid[part] = false;
				if (m_pSuit != NULL)
				{
					if (!m_bEquipValid[GLOVE] || !m_bEquipValid[BODY] || !m_bEquipValid[FOOT])
						m_pSuit = NULL;
				}
				_UpdateCreature(false, itemClone, part);
			}
		}
		else 
		{
			if (!m_bEquipValid[part])
			{
				CItemCmdBuilder_Svr::Build_r2c_equip_item_state(part, 1);
				m_pMaster->RecvCmd(g_sendCmd);

				//                LOG1("The item (part=%d) become VALID\n", part);
				m_bEquipValid[part] = true;
				_UpdateCreature(true, itemClone, part);
			}
		}
	}
}

void CRegionItemContainer::OnAfterEquip(SItemID& item, EEquipParts vPart)
{
	CItemCmdBuilder_Svr::Build_r2c_player_equipitem(item, vPart);
	m_pMaster->RecvCmd(g_sendCmd);
	m_pMaster->m_modelRef ++;

	// 如果只有左手有武器，而且不是盾，就把它自动换到右手
	SItemID w1 = GetEquipItem(WEAPON_1, true);
	SItemID w2 = GetEquipItem(WEAPON_2, true);
	if ( !ItemID_IsValid(w1) && ItemID_IsValid(w2) && !ItemIsShield(w2) )
	{
		ExchangeHandWeapon();
	}

	//OnCreatureAttribChanged();

	//m_pMaster->m_Skill.OnAttributeChanged();
}

void CRegionItemContainer::OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld)
{
	CItemCmdBuilder_Svr::Build_r2c_player_unequipitem(vPart);
	m_pMaster->RecvCmd(g_sendCmd);
	m_pMaster->m_modelRef ++;

	// 如果只有左手有武器，而且不是盾，就把它自动换到右手
	SItemID w1 = GetEquipItem(WEAPON_1, true);
	SItemID w2 = GetEquipItem(WEAPON_2, true);
	if ( !ItemID_IsValid(w1) && ItemID_IsValid(w2) && !ItemIsShield(w2) )
	{
		ExchangeHandWeapon();
	}

	//OnCreatureAttribChanged();
	//m_pMaster->m_Skill.OnAttributeChanged();
}

void CRegionItemContainer::ExchangeHandWeapon()
{
	SItemID itemBak = GetEquipItem(WEAPON_1, true);
	SItemID w2 = GetEquipItem(WEAPON_2, true);
	bool b1 = m_bEquipValid[WEAPON_1];
	bool b2 = m_bEquipValid[WEAPON_2];

	m_nItemID[WEAPON_1] = w2;
	m_nItemID[WEAPON_2] = itemBak;
	m_bEquipValid[WEAPON_1] = b2;
	m_bEquipValid[WEAPON_2] = b1;

	//SetEquipItem(WEAPON_1, w2);
	//SetEquipItem(WEAPON_2, itemBak);

	m_pMaster->m_modelRef ++;

	CItemCmdBuilder_Svr::Build_r2c_exchange_hand_weapon();
	m_pMaster->RecvCmd(g_sendCmd);
}

void CRegionItemContainer::UpdateItemToClient(EItemContainer container, DWORD itemID)
{
	int page, gridI, gridJ;
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
	{
		if (m_Bag.Find(itemID, &page, &gridI, &gridJ))
			UpdateItemToClient(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ);
	}
	else if (container==ITEM_CONTAINER_PLAYER_MISSION_BAG)
	{
		if (m_MissionBag.Find(itemID, &page, &gridI, &gridJ))
			UpdateItemToClient(ITEM_CONTAINER_PLAYER_MISSION_BAG, page, gridI, gridJ);
	}
	else if (container==ITEM_CONTAINER_PLAYER_PET_BAG)
	{
		if (m_PetBag.Find(itemID, &page, &gridI, &gridJ))
			UpdateItemToClient(ITEM_CONTAINER_PLAYER_PET_BAG, page, gridI, gridJ);
	}
	else 
	{
		ERR1("[CRegionItemContainer::UpdateItemToClient] invalid itemID(%d)\n", itemID);
	}
}

void CRegionItemContainer::UpdateItemToClient(CItemContainerBase::EEquipParts part)
{
	if (!_IsValidPart(part))
		return;

	SItemID item = GetEquipItem(part, true);
	CItemCmdBuilder_Svr::Build_r2c_refresh_equip_item(part, item);
	m_pMaster->RecvCmd(g_sendCmd);
}

void CRegionItemContainer::UpdateItemToClient(EItemContainer container, int page, int gridI, int gridJ)
{
	CBag *pBag = NULL;
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
		pBag = &(m_pMaster->m_pItemContainer->m_Bag);
	else if (container==ITEM_CONTAINER_PLAYER_MISSION_BAG)
		pBag = &(m_pMaster->m_pItemContainer->m_MissionBag);
	else if (container==ITEM_CONTAINER_PLAYER_PET_BAG)
		pBag = &(m_pMaster->m_pItemContainer->m_PetBag);

	if (pBag)
	{
		SItemID item = pBag->GetItem(page, gridI, gridJ);
		CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(container, page, gridI, gridJ, item);
		m_pMaster->RecvCmd(g_sendCmd);
	}
}

void CRegionItemContainer::_UpdateCreature(bool bEquip, SItemID& item, EEquipParts part)
{
	guard;

	// 双手鞭装备规则:
	// 1. 需要学习对应技能
	// 2. 攻击力为 右手武器 * 100% + 左手武器*x%(目前定为20%)
	// 3. 其他属性都为右手武器			(2005/6/16)原规则为：其他属性都为右手武器，左手的金色、蓝色属性无效

	CHECK(m_pCre && m_pItemManager);
	SItem *pItem;

	SItemID itemWea1 = GetEquipItem(WEAPON_1);

	int itemLevel = item.level;
	float fLev_Damage=0.0f, fLev_Attack=0.0f, fLev_Dodge=0.0f, fLev_Armor=0.0f, fLev_ElementArmor=0.0f;
	BYTE attribTemp = 0;;
	ItemUpdate_GetExtraAddtion(item, g_region->m_pItemManager, fLev_Damage, fLev_Attack, fLev_Dodge, fLev_Armor, fLev_ElementArmor);

	if ( part==WEAPON_1)
	{
		SWeapon *pWeaClass1 = (SWeapon *)m_pItemManager->GetItem(item.type);

		if (bEquip) // 把1武器装备起来
		{
			if (itemWea1.curWear > 0 )
			{
				// () -> (1)
				m_pMaster->UnEquipCuff();

				if (m_pCre->Metier == METIER_WARRIOR)//明王  伤害+(装备主伤害+装备附加伤害)*(1+力道/200)+技能附加伤害
				{
					float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
					float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
					m_pCre->DamageMax.Temp += rtRound(MaxD);
					m_pCre->DamageMin.Temp += rtRound(MinD);
				}
				else if (m_pCre->Metier == METIER_HUNTER)//花间  伤害+(装备主伤害+装备附加伤害)*(1+敏捷/200)+技能附加伤害值
				{
					float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
					float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
					m_pCre->DamageMax.Temp += rtRound(MaxD);
					m_pCre->DamageMin.Temp += rtRound(MinD);

				}
				else if (m_pCre->Metier == METIER_WIZARD)//蜀山  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
				{
					float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
					float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
					m_pCre->DamageMax.Temp += rtRound(MaxD);
					m_pCre->DamageMin.Temp += rtRound(MinD);
				}
				else if (m_pCre->Metier == METIER_TAOIST)//圣巫  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
				{
					float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
					float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
					m_pCre->DamageMax.Temp += rtRound(MaxD);
					m_pCre->DamageMin.Temp += rtRound(MinD);
				}

				m_pCre->Attack.Temp += pWeaClass1->nAttack + rtRound(fLev_Attack);
				m_pCre->Dodge.Temp += pWeaClass1->nDodge;	

				m_pCre->AttSpeed.Temp += rtRound(pWeaClass1->fAttSpeed * ATT_SPEED_SCALE);
				m_pCre->AttackRange.Temp += pWeaClass1->nAttRange;
				m_pCre->CriticalHit.Temp += pWeaClass1->nCriticalRate;
			}
		}
		else
		{								
			m_pMaster->EquipCuff();

			if (m_pCre->Metier == METIER_WARRIOR)//明王  伤害+(装备主伤害+装备附加伤害)*(1+力道/200)+技能附加伤害
			{
				float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
				float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);
			}
			else if (m_pCre->Metier == METIER_HUNTER)//花间  伤害+(装备主伤害+装备附加伤害)*(1+敏捷/200)+技能附加伤害值
			{
				float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
				float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);

			}
			else if (m_pCre->Metier == METIER_WIZARD)//蜀山  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
			{
				float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);
			}
			else if (m_pCre->Metier == METIER_TAOIST)//圣巫  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
			{
				float MaxD =  (pWeaClass1->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				float MinD =  (pWeaClass1->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);
			}

			m_pCre->Attack.Temp -= pWeaClass1->nAttack + rtRound(fLev_Attack);
			m_pCre->Dodge.Temp -= pWeaClass1->nDodge;
			m_pCre->AttSpeed.Temp -= rtRound(pWeaClass1->fAttSpeed * ATT_SPEED_SCALE);
			m_pCre->AttackRange.Temp -= pWeaClass1->nAttRange;
			m_pCre->CriticalHit.Temp -= pWeaClass1->nCriticalRate;


		}

	}
	else if (part == TRUMP)
	{
		STrump *pTrump = (STrump *)m_pItemManager->GetItem(item.type);

		if (bEquip) // 把1武器装备起来
		{
			// 伤害
			if (m_pCre->Metier == METIER_WARRIOR)//明王  伤害+(装备主伤害+装备附加伤害)*(1+力道/200)+技能附加伤害
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
				m_pCre->DamageMax.Temp += rtRound(MaxD);
				m_pCre->DamageMin.Temp += rtRound(MinD);
			}
			else if (m_pCre->Metier == METIER_HUNTER)//花间  伤害+(装备主伤害+装备附加伤害)*(1+敏捷/200)+技能附加伤害值
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
				m_pCre->DamageMax.Temp += rtRound(MaxD);
				m_pCre->DamageMin.Temp += rtRound(MinD);

			}
			else if (m_pCre->Metier == METIER_WIZARD)//蜀山  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				m_pCre->DamageMax.Temp += rtRound(MaxD);
				m_pCre->DamageMin.Temp += rtRound(MinD);
			}
			else if (m_pCre->Metier == METIER_TAOIST)//圣巫  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				m_pCre->DamageMax.Temp += rtRound(MaxD);
				m_pCre->DamageMin.Temp += rtRound(MinD);
			}
			// 命中
			m_pCre->Attack.Temp += pTrump->nAttack + rtRound(fLev_Attack);
			// 防御
			m_pCre->Armor.Temp +=rtRound((pTrump->nArmor + (fLev_Armor))*(1.0+(m_pCre->Con.GetValue())/200.0));


			// 法宝加元素伤害
			if (ItemIsTrump(item.type))
			{
				int damage[MAX_ELEMENT_ATTR],armor[MAX_ELEMENT_ATTR];
				GetTrumpElement(item,damage,armor);
				m_pCre->ElementDamage[ELEMENT_WATER].Temp += damage[ELEMENT_WATER];
				m_pCre->ElementDamage[ELEMENT_FIRE].Temp += damage[ELEMENT_FIRE];
				m_pCre->ElementDamage[ELEMENT_POISON].Temp += damage[ELEMENT_POISON];
				m_pCre->ElementArmor[ELEMENT_WATER].Temp += armor[ELEMENT_WATER];
				m_pCre->ElementArmor[ELEMENT_FIRE].Temp += armor[ELEMENT_FIRE];
				m_pCre->ElementArmor[ELEMENT_POISON].Temp += armor[ELEMENT_POISON];
			}

			float iTemp = (float)item.params[TRUMP_LEV] / 20.f;
			float fInfo = 0.02f * (float)iTemp;
			attribTemp = fInfo * 100.f;
		}
		else
		{
			// 伤害
			if (m_pCre->Metier == METIER_WARRIOR)//明王  伤害+(装备主伤害+装备附加伤害)*(1+力道/200)+技能附加伤害
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Str.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);
			}
			else if (m_pCre->Metier == METIER_HUNTER)//花间  伤害+(装备主伤害+装备附加伤害)*(1+敏捷/200)+技能附加伤害值
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Dex.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);

			}
			else if (m_pCre->Metier == METIER_WIZARD)//蜀山  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);
			}
			else if (m_pCre->Metier == METIER_TAOIST)//圣巫  伤害+(装备主伤害+装备附加伤害)*(1+慧根/200)+技能附加伤害
			{
				float MaxD =  (pTrump->nDamageMax + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				float MinD =  (pTrump->nDamageMin + rtRound(fLev_Damage))*(1.0+(m_pCre->Int.Base)/200.0);
				m_pCre->DamageMax.Temp -= rtRound(MaxD);
				m_pCre->DamageMin.Temp -= rtRound(MinD);
			}
			// 命中
			m_pCre->Attack.Temp -= pTrump->nAttack + rtRound(fLev_Attack);
			// 防御
			m_pCre->Armor.Temp -=rtRound((pTrump->nArmor + (fLev_Armor))*(1.0+(m_pCre->Con.GetValue())/200.0));

			// 法宝加元素伤害
			if (ItemIsTrump(item.type))
			{
				int damage[MAX_ELEMENT_ATTR],armor[MAX_ELEMENT_ATTR];
				GetTrumpElement(item,damage,armor);
				m_pCre->ElementDamage[ELEMENT_WATER].Temp -= damage[ELEMENT_WATER];
				m_pCre->ElementDamage[ELEMENT_FIRE].Temp -= damage[ELEMENT_FIRE];
				m_pCre->ElementDamage[ELEMENT_POISON].Temp -= damage[ELEMENT_POISON];
				m_pCre->ElementArmor[ELEMENT_WATER].Temp -= armor[ELEMENT_WATER];
				m_pCre->ElementArmor[ELEMENT_FIRE].Temp -= armor[ELEMENT_FIRE];
				m_pCre->ElementArmor[ELEMENT_POISON].Temp -= armor[ELEMENT_POISON];
			}
		}
	}
	else if (part==BODY || part==FOOT || part==GLOVE || /*PZH*/part == CAESTUS/**/)
	{
		pItem = m_pItemManager->GetItem(item);
		if (!pItem)			return;
		SArmor *pArmor = pItem->pArmor;

		if (bEquip)
		{
			m_pCre->Dodge.Temp +=  rtRound(pArmor->nDodge +(fLev_Dodge));
			m_pCre->Armor.Temp +=rtRound((pArmor->nArmor + (fLev_Armor))*(1.0+(m_pCre->Con.GetValue())/200.0));
			m_pCre->Attack.Temp += rtRound(pArmor->nAttack + (fLev_Attack));
			m_pCre->AttSpeed.Temp += rtRound(pArmor->fAttSpeed * ATT_SPEED_SCALE);
			m_pCre->MovSpeed.Temp += pArmor->nMoveSpeed;
			/*Tianh 腰带加入了血量*/
			m_pCre->MaxHp.Temp += pArmor->nMaxHP;
		}
		else
		{
			m_pCre->Dodge.Temp -=  rtRound(pArmor->nDodge +(fLev_Dodge));
			m_pCre->Armor.Temp -=rtRound((pArmor->nArmor + (fLev_Armor))*(1.0+(m_pCre->Con.GetValue())/200.0));
			m_pCre->Attack.Temp -= rtRound(pArmor->nAttack + (fLev_Attack));
			m_pCre->AttSpeed.Temp -= rtRound(pArmor->fAttSpeed * ATT_SPEED_SCALE);
			m_pCre->MovSpeed.Temp -= pArmor->nMoveSpeed;
			m_pCre->MaxHp.Temp -= pArmor->nMaxHP;
		}
	}
	else if (part==NECKLACE || part==RING 
		/*PZH*/ || CHARM == part || RING2 == part || KITS == part || TRUMP == part
		|| FASHION == part || SEALEDBOOK == part || AEROCRAFT == part/**/)
	{
		pItem = m_pItemManager->GetItem(item);
		if (!pItem)			return;
		SShiPin *pShipin = pItem->pShiPin;
		if (bEquip)
		{
			m_pCre->ElementArmor[ELEMENT_FIRE].Temp +=rtRound (pShipin->nFireDefence + (fLev_ElementArmor));
			m_pCre->ElementArmor[ELEMENT_WATER].Temp += rtRound(pShipin->nWaterDefence + (fLev_ElementArmor));
			m_pCre->ElementArmor[ELEMENT_POISON].Temp += rtRound(pShipin->nPoisonDefence + (fLev_ElementArmor));
			m_pCre->MaxHp.Temp += pShipin->nMaxHP;
			m_pCre->MaxMp.Temp += pShipin->nMaxMP;
			
		}
		else
		{
			m_pCre->ElementArmor[ELEMENT_FIRE].Temp -= rtRound(pShipin->nFireDefence + (fLev_ElementArmor));
			m_pCre->ElementArmor[ELEMENT_WATER].Temp -= rtRound(pShipin->nWaterDefence + (fLev_ElementArmor));
			m_pCre->ElementArmor[ELEMENT_POISON].Temp -= rtRound(pShipin->nPoisonDefence + (fLev_ElementArmor));
			m_pCre->MaxHp.Temp -= pShipin->nMaxHP;
			m_pCre->MaxMp.Temp -= pShipin->nMaxMP;
		}
	}

	if (ItemIsGreen(item))
	{
		AttribToCreature(item, item.attribID1, item.attribRate1 + attribTemp, bEquip);
	}
	else if (ItemIsBlue(item))
	{
		AttribToCreature(item, item.attribID1, item.attribRate1 + attribTemp, bEquip);
		AttribToCreature(item, item.attribID2, item.attribRate2 + attribTemp, bEquip);
	}
	else if (ItemIsGolden(item))
	{
		AttribToCreature(item, item.attribID1, item.attribRate1 + attribTemp, bEquip);
		AttribToCreature(item, item.attribID2, item.attribRate2 + attribTemp, bEquip);
	}

	if (item.enchantAttrib != 0)
	{
		AttribToCreature(item, item.enchantAttrib, 100, bEquip, false);
	}

	// 套装属性
	if (m_pSuit!=m_pLastSuit)
	{
		SItemID footItem;
		if (m_pLastSuit)
		{
			if(part != FOOT) 
				footItem = GetEquipItem(FOOT,true);
			else
				footItem = item;
			AttribToCreature(footItem, m_pLastSuit->attribID, SUIT_DEFAULT_ATTRIB_RATE, false);
		}
		if (m_pSuit)
		{
			if(part != FOOT) 
				footItem = GetEquipItem(FOOT,true);
			else
				footItem = item;
			AttribToCreature(footItem, m_pSuit->attribID, SUIT_DEFAULT_ATTRIB_RATE, true);
		}

		m_pLastSuit = m_pSuit;
	}

	unguard;
}

void CRegionItemContainer::DropEquipItem(CRegionCreature *pCre, int count, vector<SItemID>& itemsDrop)
{
	guard;

	if (count <= 0 || !pCre)	return;
	itemsDrop.clear();
	vector<DWORD> arrDropItems;

	EXT_SPACE::unordered_map<DWORD, int> hmIter;
	vector<SItemID> arrItems;
	for (int i=0; i<MAX_EQUIP_PARTS; i++)
	{
		SItemID item = GetEquipItem((EEquipParts)i, true);
		if (ItemID_IsValid(item) && ItemCanDrop(item) && ItemCanTrade(item) && ItemCanDrop(item))
		{
			arrItems.push_back(item);
			hmIter[item.id] = i;
		}
	}

	int dropCount = (int)arrItems.size();
	if (count >= dropCount)
	{
		for (int i=0; i<dropCount; i++)
		{
			int part = hmIter[arrItems[i].id];

			if (arrItems[i].cBind!=IB_Binded && ItemCanDelete(arrItems[i]) && ItemCanTrade(arrItems[i]) && ItemCanDrop(arrItems[i]))
			{
				UnEquip((EEquipParts)part);
				//CRegionItem *rgItem = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
				//rgItem->m_item = arrItems[i];
				//rgItem->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
				//rgItem->m_masterId = 0;
				//if(!pCre->m_scene->AddItem(rgItem,pCre->m_pos,0))
				//{
				//	g_factory->DestroyObject(rgItem);
				//	return;
				//}
				itemsDrop.push_back(arrItems[i]);
			}
		}
	}
	else
	{
		float fRate = count/(float)dropCount;
		for (int i=0; i<dropCount; i++)
		{
			if (GetProb(fRate))
			{
				arrDropItems.push_back(arrItems[i].id);
				if ((int)arrDropItems.size() >= count)
					break;
			}
		}

		if (arrDropItems.size()>0)
		{
			for (int i=0; i<(int)arrDropItems.size(); i++)
			{
				int part = hmIter[arrDropItems[i]];
				SItemID tmpItem = GetEquipItem((EEquipParts)part, true);

				if (tmpItem.cBind != IB_Binded && ItemCanDelete(tmpItem) && ItemCanTrade(tmpItem) && ItemCanDrop(tmpItem))
				{
					UnEquip((EEquipParts)part);
					//CRegionItem *rgItem = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
					//rgItem->m_item = tmpItem;
					//rgItem->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
					//rgItem->m_masterId = 0;
					//if(!pCre->m_scene->AddItem(rgItem,pCre->m_pos,0))
					//{
					//	g_factory->DestroyObject(rgItem);
					//	return;
					//}
					itemsDrop.push_back(tmpItem);
				}
			}
		}
	}

	// 如果只有左手有武器，而且不是盾，就把它自动换到右手
    SItemID w1 = GetEquipItem(WEAPON_1, true);
	SItemID w2 = GetEquipItem(WEAPON_2, true);
	if ( !ItemID_IsValid(w1) && ItemID_IsValid(w2) && !ItemIsShield(w2) )
	{
		ExchangeHandWeapon();
	}

	unguard;
}

void CRegionItemContainer::DropBagItem(CRegionCreature *pCre, int count, vector<SItemID>& itemsDrop)
{
	guard;

	if (count <= 0 || !pCre)	return;
	vector<DWORD> arrDropItems;

	EXT_SPACE::unordered_map<DWORD, CBag::BagIter> hmIter;
	vector<SItemID> arrItems;
	for (int p=0; p<m_Bag.GetPageCount(); p++)
		for (int i=0; i<m_Bag.GetLineCount(); i++)
			for (int j=0; j<m_Bag.GetColCount(); j++)
			{
				SItemID item = m_Bag.GetItem(p, i, j);
				if ( ItemID_IsValid(item) && ItemCanDrop(item) && ItemCanTrade(item) && ItemCanDelete(item))
				{
					arrItems.push_back(item);
					CBag::BagIter bagIter;
					bagIter.p = p;
					bagIter.i = i;
					bagIter.j = j;
					hmIter[item.id] = bagIter;
				}
			}

	int dropCount = (int)arrItems.size();
	if (count >= dropCount)
	{
		for (int i=0; i<dropCount; i++)
		{
			CBag::BagIter bagIter = hmIter[arrItems[i].id];

			if (arrItems[i].cBind != IB_Binded && ItemCanDelete(arrItems[i]) && ItemCanTrade(arrItems[i]) && ItemCanDrop(arrItems[i]))
			{
				m_Bag.RemoveItem(bagIter.p, bagIter.i, bagIter.j);

				//CRegionItem *rgItem = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
				//rgItem->m_item = arrItems[i];
				//rgItem->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
				//rgItem->m_masterId = 0;
				//if(!pCre->m_scene->AddItem(rgItem,pCre->m_pos,0))
				//{
				//	g_factory->DestroyObject(rgItem);
				//	return;
				//}
				itemsDrop.push_back(arrItems[i]);
			}
		}
	}
	else
	{
		float fRate = count/(float)dropCount;
		for (int i=0; i<dropCount; i++)
		{
			if (GetProb(fRate))
			{
				arrDropItems.push_back(arrItems[i].id);
				if ((int)arrDropItems.size() >= count)
					break;
			}
		}
		if (arrDropItems.size()>0)
		{
			for (int i=0; i<(int)arrDropItems.size(); i++)
			{
				CBag::BagIter bagIter = hmIter[arrDropItems[i]];
				SItemID tmpItem = m_Bag.GetItem(bagIter.p, bagIter.i, bagIter.j);
				m_Bag.RemoveItem(bagIter.p, bagIter.i, bagIter.j);

				if (tmpItem.cBind != IB_Binded && ItemCanDelete(tmpItem) && ItemCanTrade(tmpItem) && ItemCanDrop(tmpItem))
				{
//                    if (tmpItem.cBind != IB_Binded)
					//CRegionItem *rgItem = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
					//rgItem->m_item = tmpItem;
					//rgItem->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
					//rgItem->m_masterId = 0;
					//if(!pCre->m_scene->AddItem(rgItem,pCre->m_pos,0))
					//{
					//	g_factory->DestroyObject(rgItem);
					//	return;
					//}
					itemsDrop.push_back(tmpItem);
				}
			}
		}
	}

	unguard;
}

int CRegionItemContainer::DropMoney(CRegionCreature *pCre, float fRate)
{
	guard;

	if (fRate<0.0f || fRate>1.0f)
		return 0;

	int money = m_Bag.GetMoney() * fRate;
	if (money>0)
	{
		m_Bag.RemoveMoney(money);
		return money;
	}
	else 
		return 0;

	unguard;
}

void CRegionItemContainer::GetSkillAddtionRate(float& fDamageWea1, float& fDamageWea2, float& fAttackWea1, float& fAttackWea2)
{
    guard;

    SSkill* pSkillDamage = m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_DAMAGE);
    SSkill* pSkillAttack = m_pMaster->m_Skill.FindSkillBySubID(ITEM_TWOBIAN_SKILLID_ATTACK);
    int iSkillLevel_Damage = 0;
    int iSkillLevel_Attack = 0;
    if (pSkillDamage)
    {
        iSkillLevel_Damage = pSkillDamage->iLevel;
    }
    if (pSkillAttack)
    {
        iSkillLevel_Attack = pSkillAttack->iLevel;
    }
    fDamageWea1 = s_fTwoHandWeapon_Wea1Scale;
    fDamageWea2 = iSkillLevel_Damage * 0.10;
    fAttackWea1 = s_fTwoHandWeapon_Wea1Scale;
    fAttackWea2 = iSkillLevel_Attack * 0.20;

    unguard;
}

void CRegionItemContainer::AttribToCreature(SItemID& item, DWORD attribID, BYTE attribRate, bool bAdd, bool bItemRelated/* = true*/)
{
	guard;

	CHECK(m_pItemManager && m_pCre);

	if (attribID==0)
		return;

	vector<SAttribAddtion> arrAttrib;
	if (m_pItemManager->m_pAttribMgr->GetAttrib(attribID, arrAttrib))
	{
		for (int i=0; i<(int)arrAttrib.size(); i++)
		{
			AttribToCreature(item, arrAttrib[i], attribRate, bAdd, bItemRelated);
		}
	}

	unguard;
}

void CRegionItemContainer::AttribToCreature(SItemID& item, SAttribAddtion& att, BYTE attribRate, bool bAdd, bool bItemRelated/* = true*/)
{
	guard;

	if (!m_pCre)			return;
	SItemBase* pItemClass = m_pItemManager->GetItem(item.type);
	if (!pItemClass)		return;
	EEquipParts	ItemE = GetDefaultEquipPart(item);

	//SItemID	iItemID1  = 	m_nItemID[WEAPON_1];
	//SItemID	iItemID3  = 	m_nItemID[WEAPON_3];

	//if (bAdd == false)
	//{
	//	if(iItemID3 == item)
	//	{
	//		return;
	//	}
	//}
	//else
	//{
	//	if (IsEquip(WEAPON_1) && pItemClass->type == 1)
	//	{
	//		return;
	//	}
	//}

	int Num = 0;
	double fRate = attribRate/100.0f;

	double fTmp = 0.0;
	long lTmp = 0;

	switch (att.attrib)
	{
	case ITEM_ATTRIB_CON:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)
		{
			m_pCre->Con.Temp += lTmp;
			m_pCre->Con.ColorfulValue += lTmp;
		}
		else
		{
			m_pCre->Con.Temp -= lTmp;
			m_pCre->Con.ColorfulValue -= lTmp;
		}
		break;
	case ITEM_ATTRIB_STR:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)
		{
			m_pCre->Str.Temp += lTmp;
			m_pCre->Str.ColorfulValue += lTmp;
		}
		else
		{
			m_pCre->Str.Temp -= lTmp;
			m_pCre->Str.ColorfulValue -= lTmp;
		}
		break;
	case ITEM_ATTRIB_DEX:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)
		{
			m_pCre->Dex.Temp += lTmp;
			m_pCre->Dex.ColorfulValue += lTmp;
		}
		else	
		{
			m_pCre->Dex.Temp -= lTmp;
			m_pCre->Dex.ColorfulValue -= lTmp;
		}
		break;
	case ITEM_ATTRIB_INT:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)
		{
			m_pCre->Int.Temp += lTmp;
			m_pCre->Int.ColorfulValue += lTmp;
		}
		else	
		{ 
			m_pCre->Int.Temp -= lTmp;
			m_pCre->Int.ColorfulValue -= lTmp;
		}
		break;
	case ITEM_ATTRIB_HIT:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)	
		{
			m_pCre->Hit.Temp += lTmp;
			m_pCre->Hit.ColorfulValue += lTmp;
		}
		else		
		{
			m_pCre->Hit.Temp -= lTmp;
			m_pCre->Hit.ColorfulValue -= lTmp;
		}
		break;
	case ITEM_ATTRIB_CRITICALHIT:// 必杀率
		if (bItemRelated)		lTmp = rtRound(fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count/100);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->CriticalHit.Rate += lTmp;
		else			m_pCre->CriticalHit.Rate -= lTmp;
		break;
	case ITEM_ATTRIB_RATE_ATTACKSPEED://攻击速度
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->AttSpeed.Rate += fTmp;
		else 
			m_pCre->AttSpeed.Rate -= fTmp;
		break;
	case ITEM_ATTRIB_ATTACKRANGE: //攻击距离
		if (bItemRelated)		lTmp = rtRound(fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->AttackRange.Temp += lTmp;
		else			m_pCre->AttackRange.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_RATE_DAMAGE:// 伤害百分比
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
		{
			m_pCre->DamageMin.Rate += fTmp;
			m_pCre->DamageMax.Rate += fTmp;
		}
		else 
		{
			m_pCre->DamageMin.Rate -= fTmp;
			m_pCre->DamageMax.Rate -= fTmp;
		}
		break;
	case ITEM_ATTRIB_RATE_ATTACK:// 命中百分比
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->Attack.Rate += fTmp;
		else 
			m_pCre->Attack.Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_ARMOR:// 防御百分比
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->Armor.Rate += fTmp;
		else 
			m_pCre->Armor.Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_DODGE://闪躲百分比
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->Dodge.Rate += fTmp;
		else 
			m_pCre->Dodge.Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_FIREARMOR:
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->ElementArmor[ELEMENT_FIRE].Rate += fTmp;
		else 
			m_pCre->ElementArmor[ELEMENT_FIRE].Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_FIREDAMAGE:
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->ElementDamage[ELEMENT_FIRE].Rate += fTmp;
		else 
			m_pCre->ElementDamage[ELEMENT_FIRE].Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_WATERARMOR:
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->ElementArmor[ELEMENT_WATER].Rate += fTmp;
		else 
			m_pCre->ElementArmor[ELEMENT_WATER].Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_WATERDAMAGE:
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0;
		if (bAdd)
			m_pCre->ElementDamage[ELEMENT_WATER].Rate += fTmp;
		else 
			m_pCre->ElementDamage[ELEMENT_WATER].Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_POISONARMOR:
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->ElementArmor[ELEMENT_POISON].Rate += fTmp;
		else 
			m_pCre->ElementArmor[ELEMENT_POISON].Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_POISONDAMAGE:
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->ElementDamage[ELEMENT_POISON].Rate += fTmp;
		else 
			m_pCre->ElementDamage[ELEMENT_POISON].Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_MAXHP:// 最大HP百分比
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->MaxHp.Rate += fTmp;
		else 
			m_pCre->MaxHp.Rate -= fTmp;
		break;
	case ITEM_ATTRIB_RATE_MAXMP:// 最大MP百分比
		if (bItemRelated)		fTmp = fRate * (1.0f + pItemClass->requirement[ItemRequirement_Level]/100.0f) * att.count / 100.0f;
		else					fTmp = fRate * att.count / 100.0f;
		if (bAdd)
			m_pCre->MaxMp.Rate += fTmp;
		else 
			m_pCre->MaxMp.Rate -= fTmp;
		break;
	case ITEM_ATTRIB_CON_POWER:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)		m_pCre->ConPower.Temp += lTmp;
		else			m_pCre->ConPower.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_STR_POWER:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)		m_pCre->StrPower.Temp += lTmp;
		else			m_pCre->StrPower.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_DEX_POWER:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)		m_pCre->DexPower.Temp += lTmp;
		else			m_pCre->DexPower.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_INT_POWER:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)		m_pCre->IntPower.Temp += lTmp;
		else			m_pCre->IntPower.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_HIT_POWER:
		if (bItemRelated)		lTmp = rtRound(fRate * att.count * pItemClass->requirement[ItemRequirement_Level] / 100.0f);
		else					lTmp = rtRound(fRate * att.count / 100.0f);
		if (bAdd)		m_pCre->HitPower.Temp += lTmp;
		else			m_pCre->HitPower.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_APPENDSKILL:
		if (bAdd)
		{
			WORD skillID = att.count;
			bool bIsAdded = false;
			if (!m_pMaster->m_Skill.FindAllSkill(skillID))
			{
				bIsAdded = true;
				m_pMaster->m_Skill.AddSkill(skillID, 0, true);
			}
			else 
			{
				ERR("[CRegionItemContainer::AttribToCreature] ITEM_ATTRIB_APPENDSKILL error, Multi Add!\n");
				return;
			}
		}
		else 
		{
			WORD skillID = att.count;
			if (!m_pMaster->m_Skill.FindAllSkill(skillID))
			{
				ERR("[CRegionItemContainer::AttribToCreature] ITEM_ATTRIB_APPENDSKILL error!\n");
				return;
			}
			m_pMaster->m_Skill.DeleteSkill(skillID, true);
		}
		break;
	case ITEM_ATTRIB_RELEASEEFFECT_SELF:
		if (bAdd)
		{
			DWORD effectID = att.count;
			BYTE effectRate = att.skillRate;
			EXT_SPACE::unordered_map<DWORD, BYTE>::iterator iter = m_ReleaseEffectsToSelf.find(effectID);
			if (iter==m_ReleaseEffectsToSelf.end())
			{
				m_ReleaseEffectsToSelf[effectID] = effectRate;
			}
			else 
			{
				ERR("[CRegionItemContainer::AttribToCreature] ITEM_ATTRIB_RELEASEEFFECT_SELF add error!\n");
				return;
			}
		}
		else
		{
			DWORD effectID = att.count;
			EXT_SPACE::unordered_map<DWORD, BYTE>::iterator iter = m_ReleaseEffectsToSelf.find(effectID);
			if (iter==m_ReleaseEffectsToSelf.end())
			{
				ERR("[CRegionItemContainer::AttribToCreature] ITEM_ATTRIB_RELEASEEFFECT_SELF remove error!\n");
				return;
			}
			else 
			{
				m_ReleaseEffectsToSelf.erase(iter);
			}
		}
		break;
	case ITEM_ATTRIB_RELEASEEFFECT_ENEMY:
		if (bAdd)
		{
			DWORD effectID = att.count;
			BYTE effectRate = att.skillRate;
			EXT_SPACE::unordered_map<DWORD, BYTE>::iterator iter = m_ReleaseEffectsToEnemy.find(effectID);
			if (iter==m_ReleaseEffectsToEnemy.end())
			{
				m_ReleaseEffectsToEnemy[effectID] = effectRate;
			}
			else 
			{
				ERR("[CRegionItemContainer::AttribToCreature] ITEM_ATTRIB_RELEASEEFFECT_ENEMY add error!\n");
				return;
			}
		}
		else
		{
			DWORD effectID = att.count;
			EXT_SPACE::unordered_map<DWORD, BYTE>::iterator iter = m_ReleaseEffectsToEnemy.find(effectID);
			if (iter==m_ReleaseEffectsToEnemy.end())
			{
				ERR("[CRegionItemContainer::AttribToCreature] ITEM_ATTRIB_RELEASEEFFECT_ENEMY remove error!\n");
				return;
			}
			else 
			{
				m_ReleaseEffectsToEnemy.erase(iter);
			}
		}
		break;
	case ITEM_ATTRIB_SKILLCRITICAL_RATE: //技能爆击率
		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->SkillCriRate += lTmp;
		else			m_pCre->SkillCriRate -= lTmp;
		break;
	case ITEM_ATTRIB_SKILLCRITICAL_DAM: //技能爆击伤害
		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->SkillCriDamageRate += lTmp;
		else			m_pCre->SkillCriDamageRate -= lTmp;
		break;
    case ITEM_ATTRIB_DAMAGE_ABSORB: //伤害吸收
        if (bItemRelated)       lTmp = rtRound(fRate * att.count);
        else                    lTmp = rtRound(fRate * att.count);
        if (bAdd)       m_pCre->PhyDmgAbsorbRate += lTmp;
        else            m_pCre->PhyDmgAbsorbRate -= lTmp;
        break;
    case ITEM_ATTRIB_ABSORBHP: //吸血
        if (bItemRelated)       lTmp = rtRound(fRate * att.count);
        else                    lTmp = rtRound(fRate * att.count);
        if (bAdd)       m_pCre->AbsorbHpByRate.Temp += lTmp;
        else            m_pCre->AbsorbHpByRate.Temp -= lTmp;
        break;
    case ITEM_ATTRIB_ABSORBMP: //吸魔
        if (bItemRelated)       lTmp = rtRound(fRate * att.count);
        else                    lTmp = rtRound(fRate * att.count);
        if (bAdd)       m_pCre->AbsorbMp.Temp += lTmp;
        else            m_pCre->AbsorbMp.Temp -= lTmp;
        break;
    case ITEM_ATTRIB_RATE_ABSORBMP: //比率吸魔
        if (bItemRelated)       lTmp = rtRound(fRate * att.count);
        else                    lTmp = rtRound(fRate * att.count);
        if (bAdd)       m_pCre->AbsorbMpRate.Temp += lTmp;
        else            m_pCre->AbsorbMpRate.Temp -= lTmp;
        break;
    case ITEM_ATTRIB_RATE_EXP: //获得经验
        if (bItemRelated)       lTmp = rtRound(fRate * att.count);
        else                    lTmp = rtRound(fRate * att.count);
        if (bAdd)       m_pCre->RateExp.Temp += lTmp;
        else            m_pCre->RateExp.Temp -= lTmp;
        break;
	case ITEM_ATTRIB_RATE_GOLD://杀怪金钱获得率
		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->RateGold.Temp += lTmp;
		else			m_pCre->RateGold.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_RECRITICALHIT://Tianh 抗必杀
		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->ReCriticalHit.Temp += lTmp;
		else			m_pCre->ReCriticalHit.Temp -= lTmp;
		break;	
 	case ITEM_ATTRIB_RESKILLCRITICAL://新增抗暴击属性fansom
 		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
 		else					lTmp = rtRound(fRate * att.count);
 		if (bAdd)		m_pCre->ReSkillCritical.Temp += lTmp;
 		else			m_pCre->ReSkillCritical.Temp -= lTmp;
 		break;
	case ITEM_ATTRIB_REFAINT://新增抗晕击属性fansom
		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->ReFaint.Temp += lTmp;
		else			m_pCre->ReFaint.Temp -= lTmp;
		break;
	case ITEM_ATTRIB_REPARRY://新增格挡属性fansom
		if (bItemRelated)		lTmp = rtRound(fRate * att.count);
		else					lTmp = rtRound(fRate * att.count);
		if (bAdd)		m_pCre->ReParry.Temp += lTmp;
		else	       m_pCre->ReParry.Temp -= lTmp;				
		break;

//------------------------------
// 套装的效果
//------------------------------
	case ITEM_ATTRIB_SKILLONE:   
		{
			if (bAdd)
			{	
				if (att.count == 0)
				{
					break;
				}
				bool IsAddSkill = true;
				std::vector<int>::iterator it = m_pMaster->vSuitSkill.begin();
				if (it == m_pMaster->vSuitSkill.end())
				{
					m_pMaster->vSuitSkill.push_back(att.count);
					m_pMaster->UseSkill(att.count);
				}
				else
				{
					for (;it!=m_pMaster->vSuitSkill.end();it++)
					{
						if (*(it) == att.count)
						{
							IsAddSkill = false;
							break;
						}
					}

					if (IsAddSkill)
					{
						m_pMaster->UseSkill(att.count);
					}
				}

			}
			else
			{	
				if (att.count == 0)
				{
					break;
				}
				bool IsDeleteSkill = true;
		     	std::vector<int>::iterator it = m_pMaster->vSuitSkill.begin(); 
				for (;it!=m_pMaster->vSuitSkill.end();it++)
				{
					if ((*it) == att.count)
					{
						m_pMaster->vSuitSkill.erase(it);
						m_pMaster->DeleteSkill(att.count);
						IsDeleteSkill = false;
						break;
					}
				}

				if (IsDeleteSkill)
				{
					m_pMaster->DeleteSkill(att.count); //如果因为某种原因  未能在列表中找到  那么也尝试再删除
				}
			}
			
		}
		break;
	case ITEM_ATTRIB_SKILLTWO:   
		{
			if (bAdd)
			{	
				if (att.count == 0)
				{
					break;
				}
				bool IsAddSkill = true;
				std::vector<int>::iterator it = m_pMaster->vSuitSkill.begin();
				if (it == m_pMaster->vSuitSkill.end())
				{
					m_pMaster->vSuitSkill.push_back(att.count);
					m_pMaster->UseSkill(att.count);
				}
				else
				{
					for (;it!=m_pMaster->vSuitSkill.end();it++)
					{
						if (*(it) == att.count)
						{
							IsAddSkill = false;
							break;
						}
					}

					if (IsAddSkill)
					{
						m_pMaster->UseSkill(att.count);
					}
				}

			}
			else
			{	
				if (att.count == 0)
				{
					break;
				}
				bool IsDeleteSkill = true;
				std::vector<int>::iterator it = m_pMaster->vSuitSkill.begin(); 
				for (;it!=m_pMaster->vSuitSkill.end();it++)
				{
					if ((*it) == att.count)
					{
						m_pMaster->vSuitSkill.erase(it);
						m_pMaster->DeleteSkill(att.count);
						IsDeleteSkill = false;
						break;
					}
				}

				if (IsDeleteSkill)
				{
					m_pMaster->DeleteSkill(att.count); //如果因为某种原因  未能在列表中找到  那么也尝试再删除
				}
			}

		}
		break;
	default:
		ERR1("[CItemContainerBase::AttribToCreature] INVALID Item Attrib(%d)\n", att.attrib);
		break;
	}

	unguard;
}


CRegionItemCompose::CRegionItemCompose()
{
	Clear();
}

CRegionItemCompose::~CRegionItemCompose()
{
}

void CRegionItemCompose::Clear()
{
	SetType(CRE_STATE_COMPOSE_ITEM);
	m_Host = NULL;
	m_ComposeID = 0;
	m_EndTime = 0;
}

bool CRegionItemCompose::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if (!CCreatureState::Begin(host, param))
		return false;

	if (!param)
		return false;
	m_ComposeID = param->p1;

	if (!m_ComposeID)
		return false;

	if (!g_region->m_pItemManager->m_pComposeMgr->CheckCompose(m_ComposeID, host->m_pItemContainer))
		return false;

	DWORD dwNow = rtGetMilliseconds();
	m_EndTime = dwNow + ITEMCOMPOSE_USE_TIME;// - HEARTBEAT_INTERVAL/2;
	m_Host = host;
	return true;
}

bool CRegionItemCompose::Tick(unsigned long delta)
{
	if (!CCreatureState::Tick(delta))
		return false;

	if (!m_ComposeID || !m_Host)
		return true;

	DWORD dwNow = rtGetMilliseconds();
	if (dwNow > m_EndTime)
	{
		// Item Class
		CItemComposeMgr* pComposeMgr = g_region->m_pItemManager->m_pComposeMgr;
		SItemCompose* pComposeClass = pComposeMgr->FindComposing(m_ComposeID);
		if (!pComposeClass)
			return true;

		// Skill Class
		WORD SkillSubID = 0;
		switch (pComposeClass->ComposeType)
		{
		case ICT_Casting:
			SkillSubID = ITEMCOMPOSE_EQUIP_SKILL;
			break;
		case ICT_Equipment:
			SkillSubID = ITEMCOMPOSE_WEAPON_SKILL;
			break;
		case ICT_Yaoping:
			SkillSubID = ITEMCOMPOSE_YAOPING_SKILL;
			break;
		case ICT_EnchantScroll:
			SkillSubID = ITEMCOMPOSE_ENCHANT_SKILL;
			break;
		default:
			break;
		}

		SSkill* pSkill;
		if (pComposeClass->ComposeType != ICT_Gem)
		{
			pSkill = m_Host->m_Skill.FindSkillBySubID(SkillSubID);
			if (!pSkill)
				return true;
		}

		// 检查材料是否齐全
		if (!pComposeMgr->CheckCompose(m_ComposeID, m_Host->m_pItemContainer))
		{
			// 道具检查没有通过，操作失败
			CItemCmdBuilder_Svr::Build_r2c_compose_item_end(m_ComposeID, itemmsg_unknown_error);
			m_Host->RecvCmd(g_sendCmd);
			Clear();
			return false;
		}
		// 检查背包是否满
		if (m_Host->m_pItemContainer->m_Bag.IsFull())
		{
			CItemCmdBuilder_Svr::Build_r2c_compose_item_end(m_ComposeID, itemmsg_compose_bagfull);
			m_Host->RecvCmd(g_sendCmd);
			Clear();
			return false;
		}
		// 检查技能等级是否满足
		if (pComposeClass->ComposeType != ICT_Gem)
			if (pSkill->iLevel < pComposeClass->SkillLevel)
			{
				Clear();
				return false;
			}
			if (m_Host->m_pItemContainer->GetMoney() < pComposeClass->m_dwNeetMoney)
			{
				m_Host->SendSystemMessage(R(MSG_AUCTION_BID_NO_MONEY));
				Clear();
				return false;
			}
				

			
		// 删除掉合成材料
		bool bError = false;
		for (int i=0; i<(int)pComposeClass->MaterialTypeCount; i++)
		{
			DWORD ItemIdx = pComposeClass->Materials[i];
			DWORD ItemNeedNum = pComposeClass->MaterialNums[i];
			if (!m_Host->m_pItemContainer->m_Bag.RemoveItem(ItemIdx, ItemColor_All, ItemNeedNum))
			{
				ERR4("[CRegionItemCompose::Tick] RemoveItem Error!(User=%s,ComposeID=%d,ItemIdx=%d,Need=%d)\n", m_Host->m_userInfo->m_username.c_str(), pComposeClass->Idx, ItemIdx, ItemNeedNum);
				bError = true;
				break;
			}
		}
		if (m_Host->m_pItemContainer)
		{
			CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &(m_Host->m_pItemContainer->m_Bag));
			m_Host->RecvCmd(g_sendCmd);
		}

		// 增加合成后的成品
		SItemID itemCreate;
		if (!bError)
		{
			//m_Host->m_pItemContainer->RemoveMoney(pComposeClass->m_dwNeetMoney);
			m_Host->RemoveGold(pComposeClass->m_dwNeetMoney);
			SItemBase *pItemBase = g_region->m_pItemManager->GetItemFromIndex(pComposeClass->Product);
			if(NULL == pItemBase)
			{
				return false;
			}
			int nNeetLevel = 1;
			nNeetLevel = pComposeClass->SkillLevel;//pItemBase->requirement[ItemRequirement_Level] / 10 + 1;
			if (pComposeClass->ComposeType == ICT_Equipment || pComposeClass->ComposeType == ICT_Casting || pComposeClass->ComposeType == ICT_EnchantScroll
				/*|| pComposeClass->ComposeType == ICT_EnchantScroll*/)
			{

				EItemColor color = ItemColor_White;
				if (GetProb(pComposeClass->GreenRate))
				{
					color = ItemColor_Green;
					if (GetProb(pComposeClass->BlueRate))
					{
						color = ItemColor_Blue;
						if (GetProb(pComposeClass->GoldenRate))
						{
							color = ItemColor_Golden;
						}
					}
				}

				unsigned char nLev = 0;
				int nRe = GetRand(10000, 0);
				int i;
				for (i = 0; i < MAX_UPDATE_ITEM_LEVEL; i++)
				{
					if (nRe < pComposeClass->m_UpdateRate[i])
					{
						break;
					}
					nLev++;
				}
				if (nLev >= MAX_UPDATE_ITEM_LEVEL)
				{
					nLev = MAX_UPDATE_ITEM_LEVEL - 1;
				}

				if (pComposeClass->ComposeType == ICT_EnchantScroll && GetProb(pComposeClass->FailRate))
				{
					itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->FaidItems);
				}
				else
				{
					itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->Product, color, nLev);
					SItemBase* pItemBase = g_region->m_pItemManager->GetItem(itemCreate.type);
					if (!pItemBase)
					{
						return false;
					}

					if (itemCreate.color == ItemColor_Golden)
					{
						if(pComposeClass->ComposeType == ICT_Equipment)
						{
							string strone =  pItemBase->name;//武器名
							string str = m_Host->m_core.Name.c_str();
							const char* playername = str.c_str();
							const char *Name = "";
							const char *BossName = "";
							const char *SceneName = "";
							const char *ItemName = strone.c_str();

							char Cstr[10240];
							long lcolor = itemCreate.color;
							string AllStr;
							string StrItem = itemCreate.SaveToString(g_region->m_pItemManager);
							AllStr += "item[";
							rt2_sprintf(Cstr, "%d^)!%s^)!%s",lcolor,strone.c_str(),StrItem.c_str());
							AllStr += Cstr;
							AllStr += "]meti";
							m_Host->Systembroadcast(playername,Name,BossName,SceneName,SMITHINGWEAPONS,AllStr.c_str());

							/*[玩家名]两手一戳，衣袖一卷，抡起大铁锤乒乒乓乓一阵乱敲乱打。不多时，便见一件兵器在手中成形，
							直看众人惊声长叹：“偶的乖乖，这快便打出一件人间凶器[兵器名]，不亏是凶器量产师。”*/
						}
						else if (pComposeClass->ComposeType == ICT_Casting)
						{
							string strone =  pItemBase->name;//武器名字

							string str = m_Host->m_core.Name.c_str();
							const char* playername = str.c_str();
							const char *Name = "";
							const char *BossName = "";
							const char *SceneName = "";
							const char *ItemName = strone.c_str();

							char Cstr[10240];
							long lcolor = itemCreate.color;
							string AllStr;
							string StrItem = itemCreate.SaveToString(g_region->m_pItemManager);
							AllStr += "item[";
							rt2_sprintf(Cstr, "%d^)!%s^)!%s",lcolor,strone.c_str(),StrItem.c_str());
							AllStr += Cstr;
							AllStr += "]meti";
							m_Host->Systembroadcast(playername,Name,BossName,SceneName,SMITHINGARMOR,AllStr.c_str());

							/*	[玩家名]对着熔炉边锤边唱：“我屋子破，炉子破，我手上的锤儿破，你笑我，他笑我，可我出的都是超级货。
							”歌声停，锤子停，只见霞光冲天，一件超绝古今的[盔甲名]就此出世*/
						}
					}

					if (itemCreate.level >= 4)
					{
						if (pComposeClass->ComposeType == ICT_Equipment)
						{

							string strone =  pItemBase->name;//武器名字
							string str = m_Host->m_core.Name.c_str();
							const char* playername = str.c_str();
							const char *Name = "";
							const char *BossName = "";
							const char *SceneName = "";
							const char *ItemName = strone.c_str();

							char Cstr[10240];
							long lcolor = itemCreate.color;
							string AllStr;
							string StrItem = itemCreate.SaveToString(g_region->m_pItemManager);
							AllStr += "item[";
							rt2_sprintf(Cstr, "%d^)!%s^)!%s",lcolor,strone.c_str(),StrItem.c_str());
							AllStr += Cstr;
							AllStr += "]meti";
							m_Host->Systembroadcast(playername,Name,BossName,SceneName,SMITHINGWEAPONS4,AllStr.c_str());

							/*一滴汗，两滴汗，小汗，大汗，汗如雨下……沉默地[玩家名]突然右手甩锤，
							蹬脚上桌，仰天大笑：“通过我成吉思汗的努力，终于打造出了威力绝伦的[兵器名]。*/
						}
						else if (pComposeClass->ComposeType == ICT_Casting)
						{

							string strone =  pItemBase->name;//武器名字
							string str = m_Host->m_core.Name.c_str();
							const char* playername = str.c_str();
							const char *Name = "";
							const char *BossName = "";
							const char *SceneName = "";
							const char *ItemName = strone.c_str();

							char Cstr[10240];
							long lcolor = itemCreate.color;
							string AllStr;
							string StrItem = itemCreate.SaveToString(g_region->m_pItemManager);
							AllStr += "item[";
							rt2_sprintf(Cstr, "%d^)!%s^)!%s",lcolor,strone.c_str(),StrItem.c_str());
							AllStr += Cstr;
							AllStr += "]meti";
							m_Host->Systembroadcast(playername,Name,BossName,SceneName,SMITHINGARMOR4,AllStr.c_str());

							/*	[玩家名]盯着炉火愣愣发呆，恍恍惚惚做起梦来。梦中仙宫美景无数，星君仙蛾穿梭，
							好不惬意。忽然一阵剧痛，[红色]左手被自个锤儿砸出鲜血，落在锻材上，就此造出一件天下无双[盔甲]来。*/
						}
					}
				}

			}
			else if (pComposeClass->ComposeType == ICT_Yaoping)
			{
				itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->Product);
			}
			/*else if (pComposeClass->ComposeType == ICT_EnchantScroll)
			{
			if(GetProb(pComposeClass->FailRate)) //增加失败几率，Cheney, 2009.3.18
			itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->FaidItems);
			else
			itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->Product);
			}*/
			else if (pComposeClass->ComposeType == ICT_Gem)
			{
				if(GetProb(pComposeClass->FailRate)) //增加失败几率，Cheney, 2009.3.18
					itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->FaidItems);
				else
					itemCreate = g_region->m_pItemManager->CreateItem(pComposeClass->Product);
			}
			else 
			{
				return false;
			}

			if (!m_Host->AddItem(itemCreate))
			{
				bError = true;
				ERR1("[CRegionItemCompose::Tick] AddItem Error!(User=%s)\n", m_Host->m_userInfo->m_username.c_str());
			}
			unsigned short wSkillID = SkillSubID;

			if(0xffff != wSkillID)
			{
				int nMySkillLevel = m_Host->GetPlayerLogic()->GetSkillLevel(wSkillID);
				int nNeetSkillLevel = nNeetLevel;//this->GetLifeItemLevel();
				int nOneExp = (2+2*(nNeetSkillLevel - 1) * (1 + 0.05 * (nNeetSkillLevel -1 )));
				float fS = 1 + (nNeetSkillLevel - nMySkillLevel) / 3.0f;
				if(fS < 0)
				{
					fS = 0;
				}
				int nExp = (int)(nOneExp * fS);
				m_Host->GetPlayerLogic()->AddLifeSkillExp(nExp, wSkillID);
			}
		}
		// 技能成长
		//if (!bError)
		//{
			//CSkillInstance* pSkillInst = m_Host->m_Skill.FindActiveSkillBySubID(SkillSubID);
			//if (pSkillInst) pSkillInst->OnUseAfter(&(m_Host->m_Skill));
			
		//}

		// Log To DB
		if (!bError)
		{
			std::string strItem;
			strItem += rtFormatNumber(GetItemSerializeVersion());
			strItem += ",";
			strItem += itemCreate.SaveToString(g_region->m_pItemManager);
			g_region->SendUserLogEvent(PLT_ComposeItem, m_Host->m_userInfo->m_userId, "", pComposeClass->ComposeType, pComposeClass->Name, 0, strItem.c_str(), 0, "");
		}

		// 通知Client
		CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &m_Host->m_pItemContainer->m_Bag);
		m_Host->RecvCmd(g_sendCmd);
		if (bError)
		{
			CItemCmdBuilder_Svr::Build_r2c_compose_item_end(m_ComposeID, itemmsg_unknown_error);
			m_Host->RecvCmd(g_sendCmd);
		}
		else 
		{
			CItemCmdBuilder_Svr::Build_r2c_compose_item_end(m_ComposeID, itemmsg_success);
			m_Host->RecvCmd(g_sendCmd);
		}

		Clear();
		return false;
	}

	return true;
}

void CRegionItemCompose::End()
{
	Clear();

	CCreatureState::End();
}


CRegionItemSaleStat::CRegionItemSaleStat()
{
}

CRegionItemSaleStat::~CRegionItemSaleStat()
{
}

void CRegionItemSaleStat::Run()
{
	static DWORD dwLastProcessTick = 0;
	DWORD dwNow = rtGetMilliseconds();
	if (dwNow - dwLastProcessTick < ITEMSALESTAT_RUN_INTERVAL)
		return;

	dwLastProcessTick = dwNow;

	// 找出需要log的项，并发送log
	std::vector<DWORD> ToRemoveList;
	EXT_SPACE::unordered_map<DWORD, SItemSaleStat>::iterator iter = m_Stat.begin();
	for (; iter!=m_Stat.end(); iter++)
	{
		SItemSaleStat& stat = iter->second;
		if (dwNow - stat.StartTick > ITEMSALESTAT_LOG_INTERVAL)
		{
			LogToDb(iter->first, stat.ItemName, stat.Count, ITEMSALESTAT_LOG_INTERVAL);
			ToRemoveList.push_back(iter->first);
		}
	}
	
	// 删除已经log的项
	int i = 0;
	for (i=0; i<(int)ToRemoveList.size(); i++)
	{
		m_Stat.erase(ToRemoveList[i]);
	}
}

void CRegionItemSaleStat::Add(DWORD ItemIdx, int Count)
{
	if (m_Stat.size() > 10000)
	{
		if (GetProb(0.01))
			ERR("[CRegionItemSaleStat::Add] Too Much Item Type!\n");
		return;
	}

	EXT_SPACE::unordered_map<DWORD, SItemSaleStat>::iterator iter = m_Stat.find(ItemIdx);
	if (iter == m_Stat.end())
	{
		SItemSaleStat stat;
		stat.Count = Count;
		SItemBase* pItemClass = g_region->m_pItemManager->GetItemFromIndex(ItemIdx);
		if (pItemClass)
			stat.ItemName = pItemClass->name;
		stat.StartTick = rtGetMilliseconds();

		m_Stat[ItemIdx] = stat;
	}
	else 
	{
		SItemSaleStat& stat = iter->second;
		stat.Count += Count;
	}
}

void CRegionItemSaleStat::LogToDb(DWORD ItemIdx, const std::string& ItemName, int Count, DWORD IntervalInMinites)
{
	g_region->SendUserLogEvent(PLT_ItemSaleStat, 0, "", ItemIdx, ItemName.c_str(), Count, "", IntervalInMinites, "");
}
