// region state cpp file 
//
#include "region.h"
#include "PlayerLogic.h"

const int SKILL_MW_ADD_MP_T1	= 97;
const int SKILL_MW_ADD_MP_T2	= 98;
const int SKILL_MW_ADD_MP_T3	= 99;

const int SKILL_MW_ADD_MP_S1	= 163;
const int SKILL_MW_ADD_MP_S2	= 164;
const int SKILL_MW_ADD_MP_S3	= 165;

const int SKILL_MW_ADD_ELE_T1	= 226;
const int SKILL_MW_ADD_ELE_T2	= 227;
const int SKILL_MW_ADD_ELE_T3	= 228;

const int SKILL_MW_ADD_ELE_S1	= 229;
const int SKILL_MW_ADD_ELE_S2	= 230;
const int SKILL_MW_ADD_ELE_S3	= 231;

const int SKILL_GM_SPEED = 1246;
// #define MW_TEST 
// --------------------------------------------------------------------------------
// state for magic weapon add weapon
// --------------------------------------------------------------------------------
CCreState_MagWeaAddMp::CCreState_MagWeaAddMp()
{
	SetType(CRE_STATE_MAGIC_WEAPON_ADD_MP);
	m_host = NULL;
	m_lTime = 0;
}

CCreState_MagWeaAddMp::~CCreState_MagWeaAddMp() {}

bool CCreState_MagWeaAddMp::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if(!CCreatureState::Begin(host,param)) return false;

	m_host = host;
	return true;
}

bool CCreState_MagWeaAddMp::Tick(unsigned long delta)
{
//	if(!CCreatureState::Tick(delta)) return false;
//	
//	// 合法性判断
//	SItemID item = m_host->m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
//	if(!ItemID_IsValid(item)) return false;
//
//	if(!ItemIsWeapon_MagicWeapon(item.type)) return false;
//
//	int maxMp;
//	MW_GetInfo(item,maxMp);
//	if(item.params[MW_PARAM_MP] >= maxMp)
//	{
//		item.params[MW_PARAM_MP] = maxMp;
//		m_host->UpdateMagicWeapon(item);
//		return false;
//	}
//
//    int mp = 10;
//	float rate = 0.1;
//	
//	// 检查技能
//	if(m_host->m_Skill.FindAllSkill(1246))
//	{
//		mp = 10;
//		rate = 10;
//	}
//	else if(m_host->m_Skill.FindAllSkill(SKILL_MW_ADD_MP_T1) || m_host->m_Skill.FindAllSkill(SKILL_MW_ADD_MP_S1))
//	{
//		mp = 10;
//		rate = 0.3;
//	}
//	else if(m_host->m_Skill.FindAllSkill(SKILL_MW_ADD_MP_T2) || m_host->m_Skill.FindAllSkill(SKILL_MW_ADD_MP_S2))
//	{
//		mp = 10;
//		rate = 0.6;
//	}
//	else if(m_host->m_Skill.FindAllSkill(SKILL_MW_ADD_MP_T3) || m_host->m_Skill.FindAllSkill(SKILL_MW_ADD_MP_S3))
//	{
//		mp = 10;
//		rate = 1;
//	}
//	else
//	{
//		// return false;
//	}
//
//	if(m_host->m_Skill.FindAllSkill(SKILL_GM_SPEED))
//	{
//		rate = 100;
//	}
//
//#ifdef MW_TEST
//	rate = 100;
//#endif
//    int finial = mp * rate;
//
//	// 如果mp不够，自动喝mp
//	if(m_host->m_core.GetMp() < mp)
//	{
//		// notify client
//		g_sendCmd->BeginWrite();
//		g_sendCmd->WriteShort(r2c_mw_add_mp);
//		g_sendCmd->WriteShort(1);
//		m_host->RecvCmd(g_sendCmd);
//		return true;
//	}
//	else
//	{
//		m_host->m_core.AddMp(-mp);
//		item.params[MW_PARAM_MP] += finial;
//		if(item.params[MW_PARAM_MP] > maxMp)
//			item.params[MW_PARAM_MP] = maxMp;
//		m_host->UpdateMagicWeapon(item);
//		// update data to client
//		g_sendCmd->BeginWrite();
//		g_sendCmd->WriteShort(r2c_mw_add_mp);
//		g_sendCmd->WriteShort(0);
//		m_host->RecvCmd(g_sendCmd);
//	}
	return true;
}

void CCreState_MagWeaAddMp::End()
{
	CCreatureState::End();
}

// --------------------------------------------------------------------------------
// state for trump add element
// --------------------------------------------------------------------------------
CCreState_MagWeaAddEle::CCreState_MagWeaAddEle()
{
	SetType(CRE_STATE_MAGIC_WEAPON_ADD_ELE);
	m_host = NULL;
	m_bCanFull = false;
	m_noFullValue = 0;
	m_ActorSoulGap = 0;
	m_bStop = false;
	m_fSpeed = 0.f;
	m_dwNeedTime = 0;
	m_dwNeedSoul = 0;
	m_dwLastTime = 0;
}

CCreState_MagWeaAddEle::~CCreState_MagWeaAddEle() {}

bool CCreState_MagWeaAddEle::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if (NULL == host)
	{
		return false;
	}
	if (host->GetPlayerLogic()->IsInPShop())
	{
		return false;
	}
	if(!CCreatureState::Begin(host,param)) return false;

	if(!param) return false;
	if(param->p1<0 || param->p1>=MAX_TRUMP_RIFINE) return false;
	if(host->IsChengBody()) return false;

	m_host = host;
	m_itemID = m_host->m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);

	// 合法性判断
	if(!ItemID_IsValid(m_itemID)) return false;
	if(!ItemIsTrump(m_itemID.type)) return false;

	m_iRifine = param->p1;

	int iValue = 0;
	switch(m_iRifine)
	{
	case MP_TRUMP:
		if(m_itemID.params[TRUMP_LEV] >= host->m_core.GradeLev) return false;
		iValue = m_itemID.params[TRUMP_MP];
		break;
	default:
		iValue = m_itemID.params[RifineToTrumpParamExp(m_iRifine)];
		break;
	}

	float iSpeed = TRUMP_ActorSoulSec(m_itemID,m_iRifine);
	float rifinePointTime = TRUMP_CentiSoul(m_itemID,m_iRifine) / iSpeed;
	long rifineNeedSoul = TRUMP_CentiSoul(m_itemID,m_iRifine) * 100.f;
	float rifineLevupTime = rifinePointTime * float(rifineNeedSoul - iValue);
	float actorSoulOverTime = m_host->m_core.Soul / iSpeed;

	if ( actorSoulOverTime < 0.f && rifineLevupTime < 0.f )
	{
		return false;
	}

	m_dwNeedTime = rifineLevupTime * 1000.f;
	m_dwNeedSoul = TRUMP_CentiSoul(m_itemID,m_iRifine) * 100.f;

	m_host->m_bIsRining = true;
	m_bStop = false;
	m_iTrumpParam = RifineToTrumpParamExp(m_iRifine);
	m_dwLastTime = rtGetMilliseconds();

	m_host->ClientDoCommand(DO_COMMAND_RIFINE,0);
	SetCanMove(true);

	return true;
}

bool CCreState_MagWeaAddEle::Tick(unsigned long delta)
{
	if(!CCreatureState::Tick(delta)) return false;

	// 合法性判断
	if(!ItemIsTrump(m_itemID.type)) return false;

	if(!m_host->m_bIsRining) return false;

	if(m_iRifine < 0 || m_iRifine >= MAX_TRUMP_RIFINE) return false;

	DWORD dwNowTime = rtGetMilliseconds();


	SItemID *pTrump = m_host->m_pItemContainer->GetEquipItemPoint(CItemContainerBase::TRUMP, true);

	if (!pTrump)
		return false;

	DWORD addSoul = m_dwNeedSoul * float(dwNowTime - m_dwLastTime) / (float)m_dwNeedTime;

	if (pTrump->params[m_iTrumpParam] + addSoul  >= m_dwNeedSoul || dwNowTime - m_dwLastTime )
	{
		if (pTrump->params[m_iTrumpParam] + addSoul > m_dwNeedSoul)
		{
			addSoul = m_dwNeedSoul - pTrump->params[m_iTrumpParam];
		}

		if (m_host->m_core.Soul >= addSoul)
		{
			m_host->m_core.Soul -= addSoul;
			pTrump->params[m_iTrumpParam] += m_dwNeedSoul * float(float(dwNowTime - m_dwLastTime) / m_dwNeedTime);

			if (pTrump->params[m_iTrumpParam] >= m_dwNeedSoul)
			{
				// 结束
				long lSucceed = 1;
				if ( m_host->m_core.Soul < 0 )
				{
					m_host->m_core.Soul = 0;
				}

				m_itemID = m_host->m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);
				pTrump = &m_itemID;
				/************************************************************************/
				/* 这里原来调用ItemID_CreateInvalid() 在linux下编译不过去  by rendh     */
				/************************************************************************/			
				SItemID tempItemID;
				m_host->m_pItemContainer->SetEquipItem(CItemContainerBase::TRUMP,tempItemID);

				if (pTrump->params[TRUMP_ATTACH_RATE] < 0)
				{
					pTrump->params[TRUMP_ATTACH_RATE] = 0;
				}

				int nAddTemp = 0;
				switch (m_iRifine)
				{
				case MP_TRUMP:
					{
						nAddTemp = 1;
						if ( pTrump->params[TRUMP_LEV] <= 100)
						{
							pTrump->params[TRUMP_LEV] += nAddTemp;
							pTrump->params[TRUMP_ATTACH_RATE] += 5;

							if (pTrump->params[TRUMP_ATTACH_RATE] > 100)
							{
								pTrump->params[TRUMP_ATTACH_RATE] = 100;
							}

							if (pTrump->params[TRUMP_LEV] == 20)
							{
								for (int i = TRUMP_SKILL_1;i< TRUMP_SKILL_MAX;i++)
								{
									if (pTrump->params[i] == 0)
									{
										int skillid = 4440 + GetRand(3) * 10;
										pTrump->params[i] = skillid;
										break;
									}
								}
							}
						}
						pTrump->params[TRUMP_MP] = 0;
					}
					break;
				case PNEUMA_TRUMP:
					{
						nAddTemp = 1;
						if ( m_host->m_core.VigourPoint <= 100)
						{
							m_host->m_core.VigourPoint += nAddTemp;
						}
						m_host->m_core.Vigour = 0;
					}
					break;
				default:
					{
						if (pTrump->params[TRUMP_ATTACH_RATE] > 0)
						{
							pTrump->params[TRUMP_ATTACH_RATE] -= 1;
						}

						if (GetProb((float)pTrump->params[TRUMP_ATTACH_RATE] / 100.f))
						{
							// 注入成功
							if (m_iRifine % 2 == 0)
							{	// 攻
								nAddTemp = 2;
								pTrump->params[RifineToTrumpParam(m_iRifine)] += nAddTemp;
							}else
							{	// 防
								nAddTemp = 1;
								pTrump->params[RifineToTrumpParam(m_iRifine)] += nAddTemp;
							}
						}else
						{
							// 注入失败
							lSucceed = 0;
						}
						pTrump->params[m_iTrumpParam] = 0;
					}
					break;
				}

				SItemID trump = m_itemID;

				if ( ItemIsTrump(trump) )
				{
					m_host->m_pItemContainer->SetEquipItem(CItemContainerBase::TRUMP,trump);
					m_host->m_pItemContainer->UpdateItemToClient(CItemContainerBase::TRUMP);
					m_host->GetPlayerLogic()->OnTrumpLevUp(*pTrump, pTrump->params[TRUMP_LEV]);
				}

				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_mw_add_ele);
				g_sendCmd->WriteByte(m_iRifine);
				g_sendCmd->SerializeBit(lSucceed,2);
				g_sendCmd->WriteLong(nAddTemp);
				m_host->RecvCmd(g_sendCmd);

				End();
				return false;
			}
			else
			{
				// 炼化中

				SItemID trump = m_host->m_pItemContainer->GetEquipItem(CItemContainerBase::TRUMP, true);

				if ( ItemIsTrump(trump) )
				{
					m_host->m_pItemContainer->SetEquipItem(CItemContainerBase::TRUMP,trump);
					//m_host->m_pItemContainer->UpdateItemToClient(CItemContainerBase::TRUMP);
				}

				long lSucceed = 2;
				g_sendCmd->BeginWrite();
				g_sendCmd->WriteShort(r2c_mw_add_ele);
				g_sendCmd->WriteByte(m_iRifine);
				g_sendCmd->SerializeBit(lSucceed,2);
				g_sendCmd->WriteLong(pTrump->params[m_iTrumpParam]);
				m_host->RecvCmd(g_sendCmd);
			}
		}
		else
		{
			// 人物灵力不足
			long lSucceed = 3;
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_mw_add_ele);
			g_sendCmd->WriteByte(m_iRifine);
			g_sendCmd->SerializeBit(lSucceed,2);
			//g_sendCmd->WriteLong(nAddTemp);
			m_host->RecvCmd(g_sendCmd);

			End();
			return false;
		}
		m_dwLastTime = dwNowTime;
	}
	return true;
}

void CCreState_MagWeaAddEle::End()
{
	CCreatureState::End();
	m_host->ClientDoCommand(DO_COMMAND_RIFINE,1);
	ItemID_SetInvalid(m_host->m_RifineTrump);
	m_host->m_bIsRining = false;
	m_bStop = true;
	m_dwLastTime = 0;
}

const unsigned char PET_MERGE_MAIN		= 0;
const unsigned char PET_MERGE_SLAVE		= 1;
const unsigned char PET_MERGE_ITEM		= 2;

const unsigned char PET_ABILITY_PET		= 0;
const unsigned char PET_ABILITY_ITEM1	= 1;
const unsigned char PET_ABILITY_ITEM2	= 2;

const unsigned char PET_TOBEADS_PET		= 0;
const unsigned char PET_TOBEADS_ITEM1	= 1;
const unsigned char PET_TOBEADS_ITEM2	= 2;

const unsigned char PET_MERGEBEADS_PET	= 0;
const unsigned char PET_MERGEBEADS_ITEM	= 1;

CCreState_FusePet::CCreState_FusePet()
{
	SetType(CRE_STATE_FUSE_PET);
	m_dwEndTime = 0;
	m_host = NULL;
	m_cFuseType = 0;
	m_bIsFuse = false;
}

CCreState_FusePet::~CCreState_FusePet()
{

}

bool CCreState_FusePet::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if(!CCreatureState::Begin(host,param)) return false;
	if(!host) return false;

	m_host = host;
	return true;
}

bool CCreState_FusePet::Tick(unsigned long delta)
{
	if(!CCreatureState::Tick(delta)) return false;

	if( !m_bIsFuse || !m_host ) return false;

	if(rtGetMilliseconds() > m_dwEndTime)
	{
		CBag *pBag = &m_host->m_pItemContainer->m_Bag;
		CBag *pPetBag = &m_host->m_pItemContainer->m_PetBag;
		int bagI,bagJ,bagPage;
		bool succeed = false;
		unsigned char finishParam = PET_FUSE_FINISH_DELETE;
		switch(m_cFuseType)
		{
		case PET_FUSE_TYPE_MERGE:
			{
				if (!pBag->Find(m_item[PET_MERGE_ITEM],&bagPage,&bagI,&bagJ))
					return false;
				if (!pBag->RemoveItem(bagPage, bagI, bagJ))
					return false;

				CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
				m_host->RecvCmd(g_sendCmd);

				// Rand()%10000<10000*（int(（主宠物等级+副宠等级）/20)*0.1）
				succeed = FusePetRate(PET_FUSE_TYPE_MERGE,m_item[PET_MERGE_MAIN],m_item[PET_MERGE_SLAVE]);
				if (succeed)
				{
					// 成功

					if (!pPetBag->Find(m_item[PET_MERGE_SLAVE],&bagPage,&bagI,&bagJ))
						return false;
					if (!pPetBag->RemoveItem(bagPage, bagI, bagJ))
						return false;

					CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
					m_host->RecvCmd(g_sendCmd);

					float fToWowRate = 0.1f;
					if (GetRand(10000) < 50)
					{
						DWORD wowItemID = 0;
						// 成功合成魔兽

						SItemPet *pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(m_item[PET_MERGE_MAIN].type);
						if(!pItemPet) return false;
						SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
						if(!pPet) return false;

						if (pPet->vParams1.size() < 2) return false;

						wowItemID = GetRand(pPet->vParams1[1],pPet->vParams1[0]);

						if (!pPetBag->Find(m_item[PET_MERGE_MAIN],&bagPage,&bagI,&bagJ))
							return false;
						if (!pPetBag->RemoveItem(bagPage, bagI, bagJ))
							return false;

						CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
						m_host->RecvCmd(g_sendCmd);

						SItemID wowItem = g_region->m_pItemManager->CreateItem(wowItemID);

						SItemID &item = wowItem;

						pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
						if(!pItemPet) return false;
						pPet = g_TablePet.FindPet(pItemPet->petID);
						if(!pPet) return false;

						if (pPet->vParams1.size() < 2) return false;

						SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(item.params[PET_PARAM_LEV]));
						if(!base) return false;
						item.params[PET_PARAM_EXP]	= 0;
						item.params[PET_PARAM_AI]	= 0;
						item.params[PET_PARAM_LEV] = pPet->Lev;

						item.params[PET_PARAM_FUSE_LEV] = m_item[PET_MERGE_MAIN].params[PET_PARAM_FUSE_LEV] + 1;

						SetPetHpAndMp(item,pPet);

						if (!pPetBag->SetItem(bagPage, bagI, bagJ,item))
							return false;

						CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, item);
						m_host->RecvCmd(g_sendCmd);
					}
					else
					{
						// 主宠合成成功

						if (!pPetBag->Find(m_item[PET_MERGE_MAIN],&bagPage,&bagI,&bagJ))
							return false;

						SItemID &item = m_item[PET_MERGE_MAIN];

						SItemPet *pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
						if(!pItemPet) return false;
						SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
						if(!pPet) return false;

						SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(item.params[PET_PARAM_LEV]));
						if(!base) return false;
						item.params[PET_PARAM_EXP]	= 0;
						item.params[PET_PARAM_AI]	= 0;
						item.params[PET_PARAM_LEV] = 1;

						//int(当前生命成长*(1+0.15*融合等级) )
						item.params[PET_PARAM_FUSE_LEV] += 1;

						SetPetHpAndMp(item,pPet);

						if (GetRand(10000) < 5000)
						{
							int count = 0;
							std::vector<int> passive;
							int skillMin = 0;
							int skillMax = 0;
							if (GetProb(0.5f))
							{
								// 继承副宠被动技能
								skillMin = PET_PARAM_PASSIVE_2;
								skillMax = PET_PARAM_PASSIVE_6;
							}
							else
							{
								// 继承副宠主动技能
								skillMin = PET_PARAM_SKILL_2;
								skillMax = PET_PARAM_SKILL_6;
							}
							for (int i=skillMin;i<=skillMax;i++)
							{
								if (m_item[PET_MERGE_SLAVE].params[i])
								{
									passive.push_back(m_item[PET_MERGE_SLAVE].params[i]);
								}
							}
							if (!passive.empty())
							{
								count = GetRand(passive.size()-1,0);
								int i=skillMin;
								for (;i<=skillMax;i++)
								{
									if (!item.params[i])
									{
										continue;
									}
									SSkill *pSkillPet = Skill()->FindSkill(item.params[i]);
									SSkill *pSkillAdd = Skill()->FindSkill(passive[count]);
									if (pSkillPet && pSkillPet && pSkillPet->wSubID == pSkillAdd->wSubID)
									{
										item.params[i] = pSkillPet->wID > pSkillAdd->wID ? pSkillPet->wID : pSkillAdd->wID;
										break;
									}
								}

								if (i > skillMax)
								{
									for (i=skillMin;i<=skillMax;i++)
									{
										if (item.params[i] == 0)
										{
											item.params[i] = passive[count];
											break;
										}
									}
								}
							}
						}
						if (!pPetBag->SetItem(bagPage, bagI, bagJ,item))
							return false;

						CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, item);
						m_host->RecvCmd(g_sendCmd);
					}
				}
				else
				{
					// 融合失败

				}
			}
			break;
		case PET_FUSE_TYPE_ABILITY:
			{
				if (!pBag->Find(m_item[PET_ABILITY_ITEM1],&bagPage,&bagI,&bagJ))
					return false;
				if (!pBag->RemoveItem(bagPage, bagI, bagJ))
					return false;

				CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
				m_host->RecvCmd(g_sendCmd);

				if (pBag->Find(m_item[PET_ABILITY_ITEM2],&bagPage,&bagI,&bagJ))
					if (pBag->RemoveItem(bagPage, bagI, bagJ))
					{
						CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
						m_host->RecvCmd(g_sendCmd);
					}

				SItemID &item = m_item[PET_ABILITY_PET];

				//max(0.2,1-宠物当前锻体等级*0.2)

				succeed = FusePetRate(PET_FUSE_TYPE_ABILITY,item,ItemID_CreateInvalid());

				if (succeed)
				{
					// 锻体成功	
					item.params[PET_PARAM_ABILITY_LEV] += 1;
				}
				else
				{
					// 锻体失败
					if ( !ItemID_IsValid(m_item[PET_ABILITY_ITEM2]) )
					{
						if (item.params[PET_PARAM_ABILITY_LEV] > 0)
							item.params[PET_PARAM_ABILITY_LEV] -= 1;
					}
					else
						finishParam = PET_FUSE_FINISH_NONE;
				}

				if (succeed || !ItemID_IsValid(m_item[PET_ABILITY_ITEM2]) )
				{
					// 闪避=闪避*（1+锻体等级*（0.05+0.01（锻体等级-1）））
					SItemPet *pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
					if(!pItemPet) return false;
					SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
					if(!pPet) return false;

					SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(item.params[PET_PARAM_LEV]));
					if(!base) return false;

					int nAbiliLev = item.params[PET_PARAM_ABILITY_LEV];

					SetPetHpAndMp(item,pPet);

					//if (ItemIsPetSpectre(item))
					//{
					//	item.params[PET_PARAM_LINKUP_LEV] = GetMergeBeadsLev(item,pPet);
					//}

					if (!pPetBag->Find(m_item[PET_ABILITY_PET],&bagPage,&bagI,&bagJ))
						return false;
					if (!pPetBag->SetItem(bagPage, bagI, bagJ,item))
						return false;

					CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, item);
					m_host->RecvCmd(g_sendCmd);
				}
			}
			break;
		case PET_FUSE_TYPE_TOBEADS:
			{
				if (!pBag->Find(m_item[PET_TOBEADS_ITEM1],&bagPage,&bagI,&bagJ))
					return false;
				if (!pBag->RemoveItem(bagPage, bagI, bagJ))
					return false;

				CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
				m_host->RecvCmd(g_sendCmd);

				if (pBag->Find(m_item[PET_TOBEADS_ITEM2],&bagPage,&bagI,&bagJ))
					if (pBag->RemoveItem(bagPage, bagI, bagJ))
					{
						CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
						m_host->RecvCmd(g_sendCmd);
					}

				SItemID &item = m_item[PET_TOBEADS_PET];

				succeed = FusePetRate(PET_FUSE_TYPE_TOBEADS,item,ItemID_CreateInvalid());

				if (succeed)
				{
					if (!pPetBag->Find(m_item[PET_TOBEADS_PET],&bagPage,&bagI,&bagJ))
						return false;
					if (!pPetBag->RemoveItem(bagPage, bagI, bagJ))
						return false;

					CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
					m_host->RecvCmd(g_sendCmd);

					// 成功
					SItemPet *pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
					if(!pItemPet) return false;
					SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
					if(!pPet) return false;

					SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(item.params[PET_PARAM_LEV]));
					if(!base) return false;

					SItemID beads = g_region->m_pItemManager->CreateItem(PET_BEADS);
					// 预估品级=round((1+int(宠物等级/10)*0.02)*(生命成长/表格生命成长系数+法力成长/表格法力成长系数+)/2,2)
					// 品级=Round(预估品级*Rand(0.7,1.3),2)

					float grade = (GetToBeadsGrade(item,pPet) * ((float)GetRand(13,7) / 10.f));

					beads.params[0] = grade * PET_PARAM_COMPUTE_BEADS;
					if (pBag->AddItem(beads,&bagPage,&bagI,&bagJ) && pBag->Find(beads,&bagPage,&bagI,&bagJ))
					{
						CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, beads);
						m_host->RecvCmd(g_sendCmd);
					}
				}
				else
				{
					// 失败
					if (!ItemID_IsValid(m_item[PET_TOBEADS_ITEM2]))
					{
						if (!pPetBag->Find(m_item[PET_TOBEADS_PET],&bagPage,&bagI,&bagJ))
							return false;
						if (!pPetBag->RemoveItem(bagPage, bagI, bagJ))
							return false;

						CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
						m_host->RecvCmd(g_sendCmd);
					}
					else
					{
						finishParam = PET_FUSE_FINISH_NONE;
					}
				}
			}
			break;
		case PET_FUSE_TYPE_MERGEBEADS:
			{
				if (!pBag->Find(m_item[PET_MERGEBEADS_ITEM],&bagPage,&bagI,&bagJ))
					return false;
				if (!pBag->RemoveItem(bagPage, bagI, bagJ))
					return false;

				CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, bagPage, bagI, bagJ, ITEM_REASON_THROW);
				m_host->RecvCmd(g_sendCmd);

				SItemID &item = m_item[PET_MERGEBEADS_PET];

				SItemPet *pItemPet = (SItemPet*)g_region->m_pItemManager->GetItem(item.type);
				if(!pItemPet) return false;
				SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
				if(!pPet) return false;

				int grade = m_item[PET_MERGEBEADS_ITEM].params[0];
				std::vector<int> vToBeads;
				for (int cnt=PET_PARAM_BEADS_1;cnt<=PET_PARAM_BEADS_4;cnt++)
				{
					if (item.params[cnt])
					{
						vToBeads.push_back(cnt);
					}
				}
				if (vToBeads.empty())
				{
					item.params[PET_PARAM_BEADS_1] = grade;
				}
				else
				{
					int randNum = GetRand(vToBeads.size(),0);
					if (randNum > 3)
						randNum = 3;
					item.params[PET_PARAM_BEADS_1 + randNum] = grade;
				}

				item.params[PET_PARAM_LINKUP_LEV] = GetMergeBeadsLev(item,pPet);

				if (!pPetBag->Find(m_item[PET_MERGEBEADS_PET],&bagPage,&bagI,&bagJ))
					return false;
				if (!pPetBag->SetItem(bagPage, bagI, bagJ,item))
					return false;

				CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, bagPage, bagI, bagJ, item);
				m_host->RecvCmd(g_sendCmd);
			}
			break;
		default:
			return false;
		}

		long lSucceed = succeed;
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_fuse_pet);
		g_sendCmd->WriteByte(m_cFuseType);
		g_sendCmd->SerializeBit(lSucceed,1);
		g_sendCmd->WriteByte(finishParam);
		m_host->RecvCmd(g_sendCmd);
		End();
		return false;
	}
	return true;
}

void CCreState_FusePet::End()
{
	CCreatureState::End();
	m_cFuseType = 0;
	for (int i=0;i<PET_FUSE_MAX_ITEM;i++)
	{
		ItemID_SetInvalid(m_item[i]);
	}
	m_dwEndTime = 0;
	m_bIsFuse = false;
	m_host = NULL;
}

bool CCreState_FusePet::SetFuseItem(unsigned char fuseType, SItemID item[PET_FUSE_MAX_ITEM])
{
	m_cFuseType = fuseType;
	for (int i=0;i<PET_FUSE_MAX_ITEM;i++)
	{
		m_item[i] = item[i];
	}
	m_dwEndTime = rtGetMilliseconds() + GetFusePetMillseconds(m_cFuseType);
	m_bIsFuse = true;
	return true;
}


CCreState_CatchPet::CCreState_CatchPet()
{
	SetType(CRE_STATE_CATCH_PET);
	m_host = NULL;
	m_spirit = NULL;
	m_dwEndTime = 0;
}

CCreState_CatchPet::~CCreState_CatchPet()
{
	End();
}

bool CCreState_CatchPet::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if(!CCreatureState::Begin(host,param)) return false;
	if(!host) return false;
	m_host = host;
	return true;
}

bool CCreState_CatchPet::Tick(unsigned long delta)
{
	if(!CCreatureState::Tick(delta)) return false;

	if(!m_host || !m_host->m_dwCatchPet || !m_spirit) return false;

	if (m_dwEndTime < rtGetMilliseconds())
	{
		m_host->AddPetToBag(m_spirit->m_npcInfo->Id);

		m_host->StopCatchSpirit(PET_CATCH_ERROR_SUCCEED);

		m_spirit->GetPlayerLogic()->SetLiveType(CPlayerLogic::eDead);
		m_spirit->DeleteAI();
		if (m_spirit->m_scene)
		{
			g_region->BuildEventDead(g_sendCmd, m_spirit);
			m_spirit->m_scene->RemoveCreature(m_spirit->m_oId);
		}
		g_factory->DestroyObject(m_spirit);
		End();
		return false;
	}
	return true;
}

void CCreState_CatchPet::End()
{
	CCreatureState::End();
	m_dwEndTime = 0;
	if (m_host)
	{
		m_host->m_dwCatchPet = 0;
	}
	m_host = NULL;
	if (m_spirit)
	{
		m_spirit->m_dwCatchMe = 0;
		m_spirit = NULL;
	}
}

bool CCreState_CatchPet::SetSpirit(CRegionCreature *spirit)
{
	if ( !spirit || !spirit->IsSpirit())
	{
		return false;
	}
	SPet *pPet = g_TablePet.FindPet(spirit->m_npcId);
	if(!pPet) return false;
	m_dwEndTime = pPet->dwCatchMillseconds + rtGetMilliseconds();
	m_spirit = spirit;
	m_spirit->m_dwCatchMe = m_host->m_oId;
	m_host->m_dwCatchPet = m_spirit->m_oId;
	SetCanMove(true);
	return true;
}

// --------------------------------------------------------------------------------
// state for dead
// --------------------------------------------------------------------------------
CCreState_Dead::CCreState_Dead()
{
	SetType(CRE_STATE_DEAD);
	m_host = NULL;
}

CCreState_Dead::~CCreState_Dead() {}

bool CCreState_Dead::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if(!CCreatureState::Begin(host,param)) return false;

	m_deadTime = rtGetMilliseconds();
	m_host = host;
	return true;
}

bool CCreState_Dead::Tick(unsigned long delta)
{
	if(!CCreatureState::Tick(delta)) return false;

	if(rtGetMilliseconds() - m_deadTime > 15 * 1000)
	{
		m_host->Rebirth();
		return false;
	}
	return true;
}

void CCreState_Dead::End()
{
	CCreatureState::End();
}

// --------------------------------------------------------------------------------
// state for Battle
// --------------------------------------------------------------------------------
CCreState_Battle::CCreState_Battle() :
c_cycleTime(5 * 1000),
m_host(NULL),
m_startTime(0)
{
	SetType(CRE_STATE_PROCESS_BATTLE);
}

CCreState_Battle::~CCreState_Battle() {}

bool CCreState_Battle::Begin(CRegionCreature *host,SCreStateParam *param)
{
	if(!CCreatureState::Begin(host,param)) return false;

	m_startTime = rtGetMilliseconds();
	m_host = host;
	return true;
}

bool CCreState_Battle::Tick(unsigned long delta)
{
	if(!CCreatureState::Tick(delta)) return false;

	if(rtGetMilliseconds() - m_startTime > c_cycleTime)
	{
		if(m_host)
			m_host->EndBattleState();
		return false;
	}
	return true;
}

void CCreState_Battle::End()
{
	CCreatureState::End();
}
