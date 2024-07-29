#include "gc_include.h"
#include "region_client.h"
#include "game_script_interface.h"
#include "game_assistant.h"
#include "gc_track_bomb_missile.h"
#include "item_net_define.h"
#include "item_pshop.h"
#include "gc_synchro_item.h"
#include "ui_form_playeritems.h"
#include "ui_form_shop.h"
#include "ui_form_playershop.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_msg.h"
#include "ui_form_union.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_char.h"
#include "ui_form_item_update.h"
#include "ui_form_dupe.h"
#include "ui_form_bankpass.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "ui_form_peteat.h"
#include "ui_form_decompound.h"
#include "ui_form_task.h"
#include "UIForm_Gem.h"
#include "ui_form_pet_include.h"
#include "UIForm_Char.h"

bool ItemCmdProcess_client(CG_CmdPacket *cmd)
{
	short item_cmd_type;
	BOOL_ENSURE( cmd->ReadShort(&item_cmd_type) )

	if (g_layerMain)
		g_layerMain->m_fromPlayerItems->LockOperate(false);

	// 道具操作
	if (item_cmd_type==r2c_item_errorcode)
		return net_r2c_item_errorcode(0, cmd);
	else if (item_cmd_type==r2c_bag_additem)
		return net_r2c_bag_additem(0, cmd);
	else if (item_cmd_type==r2c_bag_addmoney)
		return net_r2c_bag_addmoney(0, cmd);
	else if (item_cmd_type==r2c_bag_removeitem)
		return net_r2c_bag_removeitem(0, cmd);
	else if (item_cmd_type==r2c_bag_refresh)
		return net_r2c_bag_refresh(0, cmd);
	else if (item_cmd_type==r2c_player_usepileitem)
		return net_r2c_player_usepileitem(0, cmd);
	else if (item_cmd_type==r2c_player_use_pet)
		return net_r2c_player_use_pet(0, cmd);
	else if (item_cmd_type==r2c_player_move_item)
		return net_r2c_player_move_item(0, cmd);
	else if (item_cmd_type==r2c_player_dispart_item)
		return net_r2c_player_dispart_item(0,cmd);
	else if (item_cmd_type==r2c_player_equipitem)
		return net_r2c_player_equipitem(0, cmd);
	else if (item_cmd_type==r2c_player_unequipitem)
		return net_r2c_player_unequipitem(0, cmd);
	else if (item_cmd_type==r2c_player_itemupdate_result)
		return net_r2c_player_itemupdate_result(0, cmd);
	else if (item_cmd_type==r2c_player_gem_result)
		return net_r2c_player_gem_result(0, cmd);
	else if (item_cmd_type==r2c_refresh_single_item)
		return net_r2c_refresh_single_item(0, cmd);
	else if (item_cmd_type==r2c_refresh_equip_item)
		return net_r2c_refresh_equip_item(0, cmd);
	else if (item_cmd_type==r2c_exchange_hand_weapon)
		return net_r2c_exchange_hand_weapon(0, cmd);
	else if (item_cmd_type==r2c_pickitem_fail)
		return net_r2c_pickitem_fail(0, cmd);
	else if (item_cmd_type==r2c_equip_item_state)
		return net_r2c_equip_item_state(0, cmd);
    else if (item_cmd_type==r2c_use_magicitem_result)
        return net_r2c_use_magicitem_result(0, cmd);
	else if (item_cmd_type==r2c_other_additem)
		return net_r2c_other_additem(0, cmd);
    else if (item_cmd_type==r2c_peteat_result)
        return net_r2c_peteat_result(0, cmd);
	// 与玩家交易
	else if (item_cmd_type==r2c_trade_other_invite)
		return net_r2c_trade_other_invite(0, cmd);
	else if (item_cmd_type==r2c_trade_other_invite_response)
		return net_r2c_trade_other_invite_response(0, cmd);
	else if (item_cmd_type==r2c_trade_begin)
		return net_r2c_trade_begin(0, cmd);
	else if (item_cmd_type==r2c_trade_lock)
		return net_r2c_trade_lock(0, cmd);
	else if (item_cmd_type==r2c_trade_other_agree)
		return net_r2c_trade_other_agree(0, cmd);
	else if (item_cmd_type==r2c_trade_deal)
		return net_r2c_trade_deal(0, cmd);
	else if (item_cmd_type==r2c_trade_cancel)
		return net_r2c_trade_cancel(0, cmd);
	else if (item_cmd_type==r2c_trade_unlock)
		return net_r2c_trade_unlock(0, cmd);
	// 系统买卖
	else if (item_cmd_type==r2c_shop_buy_query_response)
		return net_r2c_shop_buy_query_response(0, cmd);
	else if (item_cmd_type==r2c_requstID)
		return net_r2c_requstID(0,cmd);
	else if (item_cmd_type==r2c_shop_buy_result)
		return net_r2c_shop_buy_result(0, cmd);
	else if (item_cmd_type==r2c_shop_sell_result)
		return net_r2c_shop_sell_result(0, cmd);
	else if (item_cmd_type==r2c_point_result)	//heten
		return net_r2c_point_result(0,cmd);
	// 摆摊
	else if (item_cmd_type==r2c_pshop_start_response)
		return net_r2c_pshop_start_response(0, cmd);
	else if (item_cmd_type==r2c_pshop_stop_response)
		return net_r2c_pshop_stop_response(0, cmd);
	else if (item_cmd_type==r2c_pshop_content)
		return net_r2c_pshop_content(0, cmd);
	else if (item_cmd_type==r2c_pshop_buy_result)
		return net_r2c_pshop_buy_result(0, cmd);
	else if (item_cmd_type==r2c_pshop_other_buy)
		return net_r2c_pshop_other_buy(0, cmd);
	else if (item_cmd_type==r2c_pshop_sell_result)
		return net_r2c_pshop_sell_result(0, cmd);
	else if (item_cmd_type==r2c_pshop_other_sell)
		return net_r2c_pshop_other_sell(0, cmd);
	// 钱庄
	else if (item_cmd_type==r2c_bank_query_response)
		return net_r2c_bank_query_response(0, cmd);
	else if (item_cmd_type==r2c_bank_store_response)
		return net_r2c_bank_store_response(0, cmd);
	else if (item_cmd_type==r2c_bank_withdraw_response)
		return net_r2c_bank_withdraw_response(0, cmd);
	else if (item_cmd_type==r2c_bank_addpass_result)
		return net_r2c_bank_addpass_result(0, cmd);
	else if (item_cmd_type==r2c_bank_removepass_result)
		return net_r2c_bank_removepass_result(0, cmd);
	else if (item_cmd_type==r2c_bank_changepass_result)
		return net_r2c_bank_changepass_result(0, cmd);
	else if (item_cmd_type==r2c_bank_pass_query)
		return net_r2c_bank_pass_query(0, cmd);
	else if (item_cmd_type==r2c_bank_query_response_pet)
		return net_r2c_bank_query_response_pet(0, cmd);
	else if (item_cmd_type==r2c_bank_store_response_pet)
		return net_r2c_bank_store_response_pet(0, cmd);
	else if (item_cmd_type==r2c_bank_withdraw_response_pet)
		return net_r2c_bank_withdraw_response_pet(0, cmd);
	else if (item_cmd_type==r2c_pet_linkup)
		return net_r2c_pet_linkup(0, cmd);
	// 生活技能
	else if (item_cmd_type==r2c_compose_item_begin)
		return net_r2c_compose_item_begin(0, cmd);
	else if (item_cmd_type==r2c_compose_item_end)
		return net_r2c_compose_item_end(0, cmd);
	else if (item_cmd_type==r2c_decompose_item_result)
		return net_r2c_decompose_item_result(0, cmd);
	else if (item_cmd_type==r2c_enchant_item_result)
		return net_r2c_enchant_item_result(0, cmd);
	else if (item_cmd_type==r2c_fuse_item_result)
		return net_r2c_fuse_item_result(0, cmd);
	else if (item_cmd_type==r2c_fuse_gem_result)
		return net_r2c_fuse_gem_result(0, cmd);

	// 测试Client与Server的道具栏是否一致
	else if (item_cmd_type==r2c_check_item)
		return net_r2c_check_item(0, cmd);

	else
		return false;
}


bool net_r2c_item_errorcode(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	short msgCode;
	READb_short(msgCode);

	switch (msgCode)
	{
	case itemmsg_deal_bagfull:
		PlayerFailOperate(R(MSG_ITEM_FULL));
		break;
	case itemmsg_money_error:
		PlayerFailOperate(R(MSG_MONEY_FULL));
		break;
	case itemmsg_time_non_arrival:
		PlayerFailOperate(R(MSG_TIME_NON_ARRIVAL));
		break;
	case itemmsg_compose_learned:
		PlayerFailOperate(R(MSG_SKILL_HASLEARNED));
		break;
	case itemmsg_time_far:
		PlayerFailOperate(R(MSG_ITEM_FAR));
		break;
	default:
		ERR1("未处理的Item消息：%d", msgCode);
		break;
	}
	return true;

	unguard;
}

bool net_r2c_bag_addmoney(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	long nMoney;
	char container;
	char reason;
	READb_long(nMoney);
	READb_char(container);
	READb_char(reason);
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
	{
		if (nMoney>0)
			pPlayer->mItem.AddMoney(nMoney);
		else 
			pPlayer->mItem.RemoveMoney(abs(nMoney));

		g_layerMain->m_fromPlayerItems->Refresh(true);
	}
	else
		return false;

	if (reason == ITEM_REASON_PICK)
	{
		char cTmp[256];
		if (nMoney >= 0)		rt2_snprintf(cTmp, 256, R(MSG_GET_MONEY), nMoney);
		else				rt2_snprintf(cTmp, 256, R(MSG_LOST_MONEY), -nMoney);
		ShowBattleInfoMessage(cTmp);
	}

	return true;

	unguard;
}

bool net_r2c_bag_additem(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CHECK(pPlayer);
	SItemBase* pItemClass = NULL;
	CBag *pBag = NULL;
	char container;
	char page, i, j;
	char reason;
	SItemID item;
	item.Serialize(pPacket);
	READb_char(container);
	READb_char(page);
	READb_char(i);
	READb_char(j);
	READb_char(reason);
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
		pBag = &(pPlayer->mItem.m_Bag);
	else if (container == ITEM_CONTAINER_PLAYER_MISSION_BAG)
		pBag = &(pPlayer->mItem.m_MissionBag);
	else if (container == ITEM_CONTAINER_PLAYER_PET_BAG)
		pBag = &(pPlayer->mItem.m_PetBag);
	else
		return false;

	SItemID itemOrig = pBag->GetItem(page, i, j);
	if (!ItemID_IsValid(itemOrig))
	{
		pBag->SetItem(page, i, j, item);
	}
	else 
	{
		if (ItemCanPile(item) && item.type==itemOrig.type)
		{
			itemOrig.count += item.count;
			pBag->SetItem(page, i, j, itemOrig);
		}
		else
		{
			CHECK(0);
			return false;
		}
	}
	//if ( !pBag->AddItem(page, i, j, item) )
	//	return false;
	g_workspace.ResetHintText();
	if (ItemIsPet(item))
		g_layerMain->m_formPetMain->OnPreDraw();
	else
		g_layerMain->m_fromPlayerItems->Refresh(true);

	if (ItemIsOther_Dungeon(item.type))
	{
		if (g_layerMain->m_formDupe->IsVisible())
			g_layerMain->m_formDupe->Refresh();
	}
	g_layerMain->m_formItemCompose->Refresh();

	if (reason == ITEM_REASON_PICK)
	{
		char cTmp[128];
		if (ItemIsPet(item))
		{
			rt2_snprintf(cTmp, 128, R(MSG_GET_PET), item.name.c_str());
			//cTmp[127] = 0;
			ShowSystemMessage(cTmp);
		}
		else
		{
			pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
			CHECK(pItemClass);
			string itemName = pItemClass->name;
			if (item.color!=ItemColor_White)
			{
				itemName += "(";
				switch (item.color)
				{
				case ItemColor_Green:
					itemName += R(MSG_ITEM_GREEN);
					break;
				case ItemColor_Blue:
					itemName += R(MSG_ITEM_BLUE);
					break;
				case ItemColor_Golden:
					itemName += R(MSG_ITEM_GOLDEN);
					break;
				case ItemColor_Purple:
					itemName += R(MSG_ITEM_PURPLE);
					break;
				default:
					break;
				}
				itemName += ")";
			}
			rt2_snprintf(cTmp, 128, R(MSG_GET_ITEM), itemName.c_str());
			//cTmp[127] = 0;
			ShowBattleInfoMessage(cTmp);

			g_layerMain->m_fromPlayerItems->RefreshQuickStart();
		}
	}
	else if (reason == ITEM_REASON_PICK)
	{
	}

	return true;

	unguard;
}

bool net_r2c_bag_refresh(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	if(!GetPlayer()->m_bPShop)
	{
		g_layerMain->m_formPlayerShop->OnDeleteAllItem();
	}

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CHECK(pPlayer);
	char container;
	READb_char(container);

	if (container==ITEM_CONTAINER_TRADE_OTHER_PET)
	{
		CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);
		CBag *pBag = pTrade->GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) );
		CHECK(pBag);
		pBag->Serialize(0, pPacket);
		g_layerMain->m_formTrade->RefreshPetTrade();
	}
	else if (container==ITEM_CONTAINER_TRADE_OTHER_ITEMS)
	{
		CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);
		CBag *pBag = pTrade->GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) );
		CHECK(pBag);
		pBag->Serialize(0, pPacket);

		g_layerMain->m_formTrade->Refresh();
	}
	else if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
	{
		int pageIndex;
		bool bSavePage = !g_layerMain->m_fromPlayerItems->IsInMissionPage();
		if (bSavePage)
			pageIndex = pPlayer->mItem.m_Bag.GetPageIndex();
		pPlayer->mItem.m_Bag.Serialize(0, pPacket);
		if (bSavePage)
		{
			int page = pPlayer->mItem.m_Bag.GetPageIndex();
			while (page!=pageIndex)
			{
				pPlayer->mItem.m_Bag.MoveToNextPage();
				page = pPlayer->mItem.m_Bag.GetPageIndex();
			}
		}
		g_workspace.ResetHintText();
		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}
	else if (container==ITEM_CONTAINER_PLAYER_MISSION_BAG)
	{
		pPlayer->mItem.m_MissionBag.Serialize(0, pPacket);
		g_workspace.ResetHintText();
		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}
	else if (container==ITEM_CONTAINER_PLAYER_PET_BAG)
	{
		pPlayer->mItem.m_PetBag.Serialize(0, pPacket);
		g_workspace.ResetHintText();
		if (g_layerMain->m_formPetMain->IsVisible())
		{
			g_layerMain->m_formPetMain->OnPreDraw();
		}
		if (g_layerMain->m_formPetAptitude->IsVisible())
		{
			g_layerMain->m_formPetAptitude->OnPreDraw();
		}
	}

	return true;
	unguard;
}

bool net_r2c_bag_removeitem(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CHECK(pPlayer);
	char container;
	char page, i, j;
	char reason;
	READb_char(container);
	READb_char(page);
	READb_char(i);
	READb_char(j);
	READb_char(reason);
	SItemID item;
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
	{
		item = pPlayer->mItem.m_Bag.GetItem(page, i, j);
		if ( !pPlayer->mItem.m_Bag.RemoveItem(page, i, j) )
			return false;

		// 先判断一下商店是否有这个物品
		g_layerMain->m_formPlayerShop->OnDeleteItem(item);
	}
	else if (container == ITEM_CONTAINER_PLAYER_MISSION_BAG)
	{
		item = pPlayer->mItem.m_MissionBag.GetItem(page, i, j);
		if ( !pPlayer->mItem.m_MissionBag.RemoveItem(page, i, j) )
			return false;
	}
	else if (container == ITEM_CONTAINER_PLAYER_PET_BAG)
	{
		item = pPlayer->mItem.m_PetBag.GetItem(page, i, j);
		if ( !pPlayer->mItem.m_PetBag.RemoveItem(page, i, j) )
			return false;
	}
	else
		return false;

	if (reason == ITEM_REASON_THROW)
	{
		char cTmp[128];
		if (ItemIsPet(item))
		{
			rt2_snprintf(cTmp, 128, R(MSG_LOST_PET), item.name.c_str());
			//cTmp[63] = 0;
			ShowSystemMessage(cTmp);
		}
		else
		{
			SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
			//if (pItemClass->szSound)
			//{
			//	RtgVertex3 pos;
			//	g_pSoundMgr->PlayOnce("it001.wav", false, 0, pos);
			//}
			rt2_snprintf(cTmp, 64, R(MSG_LOST_ITEM), pItemClass->name);
			//cTmp[63] = 0;
			ShowBattleInfoMessage(cTmp);

			g_workspace.ResetHintText();
			g_layerMain->m_fromPlayerItems->RefreshQuickStart();
		}
	}
	else if (reason == ITEM_REASON_DELETE)
	{
		g_workspace.ResetHintText();
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}
	else if (reason == ITEM_REASON_EQUIP)
	{
	}
	else 
	{
		g_workspace.ResetHintText();
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}

	if (ItemIsPet(item))
		g_layerMain->m_formPetMain->OnPreDraw();
	else
		g_layerMain->m_fromPlayerItems->Refresh(true);
	return true;

	unguard;
}

bool net_r2c_player_equipitem(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	DWORD dwTime = rtGetMilliseconds();

	BOOL_ENSURE( GetWorld() && GetWorld()->m_pPlayer )
	char cTmp;
	long itemID;
	CItemContainerBase::EEquipParts part;
	READb_long(itemID);
	READb_char(cTmp);
	part = (CItemContainerBase::EEquipParts)cTmp;
	int page, gridI, gridJ;

	SItemID item;
	if ( !GetWorld()->m_pPlayer->mItem.m_Bag.Find(itemID, &page, &gridI, &gridJ) )
	{
		if (part == CItemContainerBase::WEAPON_1)
		{
			item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1,true);
		}
		else if (part == CItemContainerBase::WEAPON_3)
		{
			item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_3,true);
		}
		else
		{
			return false;
		}
	}
	else
	{
		item = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(page, gridI, gridJ);
	}

	if (item.cBind == IB_BindWhenEquip || item.cBind == IB_BindWhenPick)
		item.cBind = IB_Binded;
	BOOL_ENSURE( GetWorld()->m_pPlayer->mItem.Equip(item, part) )

	const SItemBase* pItemClass = GetItemMgr()->GetItem(item.type);
	if ( pItemClass && pItemClass->szSound)
	{
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce(pItemClass->szSound, false, 0, pos);
	}

	g_workspace.ResetHintText();
	if (ItemIsPet(item))
		g_layerMain->m_formPetMain->OnPreDraw();
	else
	{
		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_formChar->Refresh(true);
	}

	LOG1("net_r2c_player_equipitem use time: %d\n", rtGetMilliseconds()-dwTime);

	return true;
	unguard;
}

bool net_r2c_player_unequipitem(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	BOOL_ENSURE( GetWorld() && GetWorld()->m_pPlayer )
	char cTmp;
	CItemContainerBase::EEquipParts part;
	READb_char(cTmp);
	part = (CItemContainerBase::EEquipParts)cTmp;

	BOOL_ENSURE( GetWorld()->m_pPlayer->mItem.UnEquip(part) )

	g_workspace.ResetHintText();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_formChar->Refresh(true);
	g_layerMain->m_formCharProp->Update();

	return true;
	unguard;
}

bool net_r2c_player_usepileitem(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = GetWorld()->m_pPlayer;
	long itemID;
	char page, i, j;
	READb_long(itemID);
	READb_char(page);
	READb_char(i);
	READb_char(j);

	SItemID item = pActor->mItem.m_Bag.GetItem(page, i, j);

	if ( !pActor->mItem.UsePileItem(page, i, j) )
		return false;

	// 更新快捷栏血瓶的个数
	g_layerMain->m_fromPlayerItems->RefreshQuickStart();

	SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
	if (pItemClass->szSound)
	{
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce(pItemClass->szSound, false, 0, pos);
	}

	g_workspace.ResetHintText();
	g_layerMain->m_fromPlayerItems->Refresh(true);

	return true;
	unguard;
}

bool net_r2c_refresh_single_item(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	SItemID item;
	char container;
	char page, i, j;
	READb_char(container);
	READb_char(page);
	READb_char(i);
	READb_char(j);
	item.Serialize(pPacket);

	CBag *pBag = NULL;
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
	{
		pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
	}
	else if (container==ITEM_CONTAINER_PLAYER_MISSION_BAG)
	{
		pBag = &(GetWorld()->m_pPlayer->mItem.m_MissionBag);
	}
	else if (container==ITEM_CONTAINER_PLAYER_PET_BAG)
	{
		pBag = &(GetWorld()->m_pPlayer->mItem.m_PetBag);
	}
	else 
	{
	}

	if (pBag)
	{
		if (!pBag->SetItem(page, i, j, item))
		{
			ERR("[net_r2c_refresh_single_item] SetItem Error\n");
			return false;
		}
	}

	g_workspace.ResetHintText();
	if (ItemIsPet(item))
	{
		if (GetPlayerPet()->GetPetItem().id == item.id)
		{
			GetPlayerPet()->SetPetItem(item);
		}
		g_layerMain->m_formPetMain->OnPreDraw();
	}
	else
	{
		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}

	return true;
	unguard;
}

bool net_r2c_refresh_equip_item(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	SItemID item;
	char part;
	READb_char(part);
	item.Serialize(pPacket);

	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	if (pCon)
	{
		SItemID itemEquip = pCon->GetEquipItem((CItemContainerBase::EEquipParts)part, true);

        if (item.level==itemEquip.level)
        {
			pCon->SetEquipData(item, (CItemContainerBase::EEquipParts)part);
			GetPlayer()->mItem.OnAfterEquip(item,(CItemContainerBase::EEquipParts)part);
        }
        else 
        {
            pCon->SetEquipData(item, (CItemContainerBase::EEquipParts)part);
            pCon->ChangeModel(itemEquip, item, (CItemContainerBase::EEquipParts)part);
            //if (!pCon->SetEquipItem((CItemContainerBase::EEquipParts)part, item))
            //{
            //    ERR("[net_r2c_refresh_single_item] SetItem Error\n");
            //    return false;
			//}
			GetPlayer()->mItem.OnAfterEquip(item,(CItemContainerBase::EEquipParts)part);
        }
		//pCon->_Equip(item,(CItemContainerBase::EEquipParts)part);
	}

	g_workspace.ResetHintText();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_formChar->Refresh(true);
	g_layerMain->m_fromPlayerItems->RefreshQuickStart();

	if (g_layerMain->m_formChar->IsVisible() && g_layerMain->m_formChar->GetNowShowType() == CUIForm_Char::TRUMP)
	{
		g_layerMain->m_formChar->OnShowTrump();
	}

	return true;
	unguard;
}

bool net_r2c_exchange_hand_weapon(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GetWorld()->m_pPlayer->mItem.ExchangeHandWeapon();
	g_workspace.ResetHintText();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_formChar->Refresh(true);
	return true;

	unguard;
}

bool net_r2c_pickitem_fail(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	PlayerFailOperate(R(MSG_ITEM_CANNOT_PICKUP));
	return true;

	unguard;
}

bool net_r2c_equip_item_state(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	char part;
	char bValid;
	READb_char(part);
	READb_char(bValid);

	if (GetWorld() && GetWorld()->m_pPlayer)
	{
		GetWorld()->m_pPlayer->mItem.SetEquipItemValid((CItemContainerBase::EEquipParts)part, bValid!=0);
		if (bValid)
		{
			ShowSystemMessage(R(MSG_ITEM_STATE_VALID));
		}
		else 
		{
			ShowSystemMessage(R(MSG_ITEM_STATE_INVALID));
		}

		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_formChar->Refresh(true);

		return true;
	}
	else 
	{
		return false;
	}

	unguard;
}

bool net_r2c_use_magicitem_result(short cmdID, CG_CmdPacket *pPacket)
{
    guard;

 //   char cTmp;
 //   long magicItemID;
 //   READb_long(magicItemID);
 //   READb_char(cTmp);
 //   EMagicItemResult magicType = (EMagicItemResult)cTmp;

 //   //CItemContainer& con = GetWorld()->m_pPlayer->mItem;
 //   //int page, gridI, gridJ;
 //   //if ( !con.m_Bag.Find(magicItemID, &page, &gridI, &gridJ) )
 //   //    return false;
 //   //con.m_Bag.RemoveItem(page, gridI, gridJ);

 //   switch(magicType)
 //   {
 //   case MI_Null:
 //       {
 //           ShowSystemMessage(R(MSG_ITEM_MAGICITEM_IS_NULL));
 //       }
 //       break;
 //   case MI_Money:
 //       {
 //           char str64[64];
 //           long money;
 //           READb_long(money);

 //           CHECK(con.AddMoney(money));

 //           snprintf(str64, 63, R(MSG_GET_MONEY), money);
 //           str64[63] = 0;
 //           ShowSystemMessage(str64);
 //       }
 //       break;
 //   case MI_Item:
 //       {
 //           char str64[64];
 //           SItemID item;
 //           item.Serialize(pPacket);

 //           con.AddItem(item);

 //           SItemBase *pItemBase = GetItemMgr()->GetItem(item.type);
 //           BOOL_ENSURE(pItemBase)

	//		string itemName = pItemBase->name;
	//		if (item.color!=ItemColor_White)
	//		{
	//			itemName += "(";
	//			switch (item.color)
	//			{
	//			case ItemColor_Green:
	//				itemName += R(MSG_ITEM_GREEN);
	//				break;
	//			case ItemColor_Blue:
	//				itemName += R(MSG_ITEM_BLUE);
	//				break;
	//			case ItemColor_Golden:
	//				itemName += R(MSG_ITEM_GOLDEN);
	//				break;
	//			case ItemColor_Purple:
	//				itemName += R(MSG_ITEM_PURPLE);
	//				break;
	//			default:
	//				break;
	//			}
	//			itemName += ")";
	//		}
	//		snprintf(str64, 63, R(MSG_GET_ITEM), itemName.c_str());
	//		str64[63] = 0;
	//		ShowSystemMessage(str64);
 //       }
 //       break;
 //   case MI_Exp:
 //       {
 //           long exp;
 //           READb_long(exp);
 //           //snprintf(str64, 63, R(MSG_ADD_EXP), exp);
 //           //str64[63] = 0;
 //           //ShowSystemMessage(str64);
 //       }
 //       break;
 //   default:
 //       break;
 //   }

 //   g_layerMain->m_fromPlayerItems->Refresh(true);
	//g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	return true;

    unguard;
}

bool net_r2c_other_additem(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	char* username;
	SItemID item;
	char reason;
	READb_string(username);
	item.Serialize(pPacket);
	READb_char(reason);

	if (reason == ITEM_REASON_PICK)
	{
		char cTmp[128];
		SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItem(item.type);
		CHECK(pItemClass);
		string itemName = pItemClass->name;
		if (item.color != ItemColor_White)
		{
			itemName += "(";
			switch (item.color)
			{
			case ItemColor_Green:
				itemName += R(MSG_ITEM_GREEN);
				break;
			case ItemColor_Blue:
				itemName += R(MSG_ITEM_BLUE);
				break;
			case ItemColor_Golden:
				itemName += R(MSG_ITEM_GOLDEN);
				break;
			case ItemColor_Purple:
				itemName += R(MSG_ITEM_PURPLE);
				break;
			default:
				break;
			}
			itemName += ")";
		}
		rt2_snprintf(cTmp, 128, R(MSG_OTHER_GET_ITEM), username, itemName.c_str());
		//cTmp[127] = 0;
		ShowBattleInfoMessage(cTmp);
	}
	else if (reason == ITEM_REASON_PICK)
	{
	}

	return true;

	unguard;
}

bool net_r2c_peteat_result(short cmdID, CG_CmdPacket *pPacket)
{
    guard;

    SItemID itemNew;
    long petFrom, petTo;
    READb_long(petFrom);
    READb_long(petTo);
    CHECK(itemNew.Serialize(pPacket));

    GcPlayer *pPlayer = GetWorld()->m_pPlayer;
    int page, i, j;
    SItemID itemOrig;
    if ( pPlayer->mItem.m_Bag.Find(petTo, &page, &i, &j) )
    {
        pPlayer->mItem.m_Bag.SetItem(page, i, j, itemNew);
    }
    if ( pPlayer->mItem.m_Bag.Find(petFrom, &page, &i, &j) )
    {
        pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid());
    }

    ShowSystemMessage(R(MSG_PET_ENGULFPET));

    // 因为符咒(甚至是道具)被删除了，所以一些地方要处理
    if (g_layerMain->m_formPlayerShop->IsVisible())
    {
        g_layerMain->m_formPlayerShop->OnCommonItemDeleted(petFrom);
        g_layerMain->m_formPlayerShop->OnCommonItemDeleted(petTo);
    }

    g_layerMain->m_fromPlayerItems->Refresh(true);
    g_layerMain->m_formPetEat->Hide();

    return true;

    unguard;
}

bool net_r2c_player_use_pet(short cmdID, CG_CmdPacket *pPacket)
{
	return false;
}

bool net_r2c_player_itemupdate_result(short cmdID, CG_CmdPacket *pPacket)
{
    SItemID itemNew;
	long itemIDOrig, scrollIDOrig, luckIDOrig;
	READb_long(itemIDOrig);
	READb_long(scrollIDOrig);
	READb_long(luckIDOrig);
	CHECK(itemNew.Serialize(pPacket));

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	int page, i, j;
    SItemID itemOrig;
	if ( pPlayer->mItem.m_Bag.Find(itemIDOrig, &page, &i, &j) )
	{
        itemOrig = pPlayer->mItem.m_Bag.GetItem(page, i, j);
		pPlayer->mItem.m_Bag.SetItem(page, i, j, itemNew);
	}
	if ( pPlayer->mItem.m_Bag.Find(scrollIDOrig, &page, &i, &j) )
	{
		pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid() );
	}
	if ( luckIDOrig && pPlayer->mItem.m_Bag.Find(luckIDOrig, &page, &i, &j))
	{
		pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid() );
	}
    CHECK(itemOrig.level<ITEM_MAX_ITEM_LEVEL && itemOrig.level>=0);
    int moneyRequired = 0;
    ItemUpdate_GetMoneyRequirement(itemOrig, moneyRequired, GetWorld()->m_pItemManager);
    pPlayer->mItem.RemoveMoney(moneyRequired);

	if (!ItemID_IsValid(itemNew))
	{
		ShowHelperMessage(R(MSG_ITEMUPDATE_FAIL), R(G_FAIL));
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("item_fail.wav", false, 0, pos);
	}
	else if (itemNew.level > 0)
	{
		ShowHelperMessage(R(MSG_ITEMUPDATE_SUCCEED), R(G_SUCCEED));
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("task_success.wav", false, 0, pos);
	}
	else
	{
		ShowHelperMessage(R(MSG_ITEMUPDATE_FAIL), R(G_FAIL));
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("item_fail.wav", false, 0, pos);
	}

	// 因为符咒(甚至是道具)被删除了，所以一些地方要处理
	if (g_layerMain->m_formPlayerShop->IsVisible())
	{
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(scrollIDOrig);
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(luckIDOrig);
		if (ItemID_IsValid(itemNew))
			g_layerMain->m_formPlayerShop->OnCommonItemDeleted(itemIDOrig);
	}

	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_formItemUpdate->Refresh();//Hide();

	return true;
}

bool net_r2c_player_gem_result(short cmdID, CG_CmdPacket *pPacket)
{
	SItemID itemNew;
	long gem1IDOrig, gem2IDOrig, gem3IDOrig, gem4IDOrig, gem5IDOrig;
	READb_long(gem1IDOrig);
	READb_long(gem2IDOrig);
	READb_long(gem3IDOrig);
	READb_long(gem4IDOrig);
	READb_long(gem5IDOrig);
	CHECK(itemNew.Serialize(pPacket));

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	int page, i, j;
	SItemID itemOrig;
	if ( gem1IDOrig &&pPlayer->mItem.m_Bag.Find(gem1IDOrig, &page, &i, &j) )
	{
		itemOrig = pPlayer->mItem.m_Bag.GetItem(page, i, j);
		pPlayer->mItem.m_Bag.SetItem(page, i, j, itemNew);
	}
	if ( gem2IDOrig &&pPlayer->mItem.m_Bag.Find(gem2IDOrig, &page, &i, &j) )
	{
		pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid() );
	}
	if ( gem3IDOrig && pPlayer->mItem.m_Bag.Find(gem3IDOrig, &page, &i, &j))
	{
		pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid() );
	}
	if ( gem4IDOrig && pPlayer->mItem.m_Bag.Find(gem4IDOrig, &page, &i, &j))
	{
		pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid() );
	}
	if ( gem5IDOrig && pPlayer->mItem.m_Bag.Find(gem5IDOrig, &page, &i, &j))
	{
		pPlayer->mItem.m_Bag.SetItem(page, i, j, ItemID_CreateInvalid() );
	}
	CHECK(itemOrig.level<ITEM_MAX_ITEM_LEVEL && itemOrig.level>=0);
	int moneyRequired = 0;
	//ItemUpdate_GetMoneyRequirement(itemOrig, moneyRequired, GetWorld()->m_pItemManager);合成需要的银币数
	pPlayer->mItem.RemoveMoney(moneyRequired);

	if (!ItemID_IsValid(itemNew))
	{
		ShowHelperMessage(R(MSG_ITEMUPDATE_FAIL), R(G_FAIL));
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("item_fail.wav", false, 0, pos);
	}
	else if (itemNew.level > 0)
	{
		ShowHelperMessage(R(MSG_ITEMUPDATE_SUCCEED), R(G_SUCCEED));
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("task_success.wav", false, 0, pos);
	}
	else
	{
		ShowHelperMessage(R(MSG_ITEMUPDATE_FAIL_DOWNTOZERO), R(G_FAIL));
		RtgVertex3 pos;
		g_pSoundMgr->PlayOnce("item_fail.wav", false, 0, pos);
	}

	// 因为符咒(甚至是道具)被删除了，所以一些地方要处理
	if (g_layerMain->m_formPlayerShop->IsVisible())
	{
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(gem2IDOrig);
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(gem3IDOrig);
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(gem4IDOrig);
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(gem5IDOrig);
		if (ItemID_IsValid(itemNew))
			g_layerMain->m_formPlayerShop->OnCommonItemDeleted(gem1IDOrig);
	}

	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_formGem->Refresh();//Hide();

	return true;
}

bool net_r2c_player_move_item(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	char pageFrom, iFrom, jFrom;
	char pageTo, iTo, jTo;
    char container;
    READb_char(container);
	READb_char(pageFrom);
	READb_char(iFrom);
	READb_char(jFrom);
	READb_char(pageTo);
	READb_char(iTo);
	READb_char(jTo);

    CBag *pBag = NULL;
    if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
        pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
    else if (container==ITEM_CONTAINER_PLAYER_MISSION_BAG)
		pBag = &(GetWorld()->m_pPlayer->mItem.m_MissionBag);
	else if (container==ITEM_CONTAINER_PLAYER_PET_BAG)
		pBag = &(GetWorld()->m_pPlayer->mItem.m_PetBag);
    else 
        return false;
	BOOL_ENSURE( pBag->MoveItem(pageFrom, iFrom, jFrom, pageTo, iTo, jTo) );

    bool bVisibleFrom = pBag->GetVisible(pageFrom, iFrom, jFrom);
    bool bVisibleTo = pBag->GetVisible(pageTo, iTo, jTo);
    pBag->SetVisible(pageFrom, iFrom, jFrom, bVisibleTo);
    pBag->SetVisible(pageTo, iTo, jTo, bVisibleFrom);

	g_workspace.ResetHintText();
	g_layerMain->m_fromPlayerItems->Refresh(true);
    g_layerMain->m_fromPlayerItems->LockOperate(false);
	return true;

	unguard;
}

bool net_r2c_player_dispart_item(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	char pageFrom, iFrom, jFrom;
	char pageTo, iTo, jTo;
	char container;
	long num;
	READb_char(container);
	READb_char(pageFrom);
	READb_char(iFrom);
	READb_char(jFrom);
	READb_char(pageTo);
	READb_char(iTo);
	READb_char(jTo);
	READb_long(num);

	CBag *pBag = NULL;
	if (container==ITEM_CONTAINER_PLAYER_COMMON_BAG)
		pBag = &(GetWorld()->m_pPlayer->mItem.m_Bag);
	else if (container==ITEM_CONTAINER_PLAYER_MISSION_BAG)
		pBag = &(GetWorld()->m_pPlayer->mItem.m_MissionBag);
	else if (container==ITEM_CONTAINER_PLAYER_PET_BAG)
		pBag = &(GetWorld()->m_pPlayer->mItem.m_PetBag);
	else 
		return false;
	BOOL_ENSURE( pBag->ResolveItem(pageFrom, iFrom, jFrom, pageTo, iTo, jTo,num) );

	bool bVisibleFrom = pBag->GetVisible(pageFrom, iFrom, jFrom);
	bool bVisibleTo = pBag->GetVisible(pageTo, iTo, jTo);
	pBag->SetVisible(pageFrom, iFrom, jFrom, bVisibleFrom);
	pBag->SetVisible(pageTo, iTo, jTo, bVisibleTo);

	g_workspace.ResetHintText();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_fromPlayerItems->LockOperate(false);
	return true;
	unguard;
}


bool net_r2c_trade_other_invite(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = NULL;
	long actorID;
	READb_long(actorID);
	pActor = GetWorld()->m_ActorManager.FindActive(actorID);

	BOOL_ENSURE(pActor)	

	if(g_layerMain && g_layerMain->m_formTrade->GetRefuseState())
	{
		CItemNetHelper::NetSend_c2r_trade_invite_response(actorID, 0);
		return true;
	}

	if (g_layerMain->m_formTrade->m_actorInviteU != 0)
	{
		LOG("另一个玩家的邀请没有显示");
		return true;
	}
	g_layerMain->m_formTrade->m_actorInviteU = actorID;
	char tmp[64];
	rt2_snprintf(tmp, 64, R(MSG_TRADE_REQUEST), pActor->GetName() );
    //tmp[63] = 0;
	UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(g_layerMain->m_formTrade, UIFormTrade, ResponseOther_Agree,pFrm);
	pFrm->EvCancel = RTW_CALLBACK_1(g_layerMain->m_formTrade, UIFormTrade, ResponseOther_Disagree,pFrm);
	DWORD dwTimeID = GetTimer()->AddTimer( g_layerMain->m_formTrade, 15);
	if (dwTimeID > 0)
	{
		g_layerMain->m_formTrade->m_pFrmOtherInvite = pFrm;
	}

	return true;
	unguard;
}

bool net_r2c_trade_other_invite_response(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = NULL;
	long actorID;
	char bAgree;
	READb_long(actorID);
	READb_char(bAgree);
	pActor = GetWorld()->m_ActorManager.FindActive(actorID);
	BOOL_ENSURE(pActor)

	char tmp[128];
	if (bAgree==1)
	{
		rt2_sprintf(tmp, R(MSG_TRADE_INVITE_AGREE), pActor->GetName() );
		ShowSystemMessage(tmp);
	}
	else if (bAgree==2)
	{
		rt2_sprintf(tmp, R(MSG_TRADE_INVITE_FAIL_TRADING), pActor->GetName() );
		ShowSystemMessage(tmp);
	}
	else if (bAgree==0)
	{
		rt2_sprintf(tmp, R(MSG_TRADE_REJECT), pActor->GetName() );
		ShowSystemMessage(tmp);
	}

	return true;
	unguard;
}

bool net_r2c_trade_begin(short cmdID, CG_CmdPacket *pPacket)
{
	GcActor *pActor = NULL;
	long actorID;
	READb_long(actorID);
	pActor = GetWorld()->m_ActorManager.FindActive(actorID);
	BOOL_ENSURE(pActor)

	// 如果在战斗，战斗停止
	if (GetWorld() && GetWorld()->m_pPlayer)
		GetWorld()->m_pPlayer->WantStopAttack();

	g_layerMain->m_formTrade->SetPos(160, 140);
	g_layerMain->m_formTrade->SetPlayer(pActor, GetWorld()->m_pPlayer);
	g_layerMain->m_formTrade->Show();

	g_layerMain->m_fromPlayerItems->SetPos(460, 140);
	g_layerMain->m_fromPlayerItems->Show();

	return true;
}

bool net_r2c_trade_lock(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = NULL;
	long actorID;
    char errorCode;
	READb_long(actorID);
    READb_char(errorCode);
    if (actorID == GetWorld()->m_pPlayer->ID())
        pActor = GetWorld()->m_pPlayer;
    else 
        pActor = GetWorld()->m_ActorManager.FindActive(actorID);
	BOOL_ENSURE(pActor);
    BOOL_ENSURE(g_layerMain->m_formTrade->IsShowing());
	CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);

    switch (errorCode)
    {
    case itemmsg_success:
        pTrade->Lock( &(pActor->mItem) );
        if (actorID != GetWorld()->m_pPlayer->ID())
            ShowSystemMessage(R(MSG_TRADE_CONFIRM));
        break;
    case itemmsg_bankpass_error:
        PlayerFailOperate(R(MSG_NEED_BANKPASS_ERROR));
        break;
    case itemmsg_unknown_error:
    default:
        PlayerFailOperate(R(MSG_TRADE_LOCK_ERROR));
        break;
    }

	g_layerMain->m_formTrade->Refresh();
	return true;

	unguard;
}

bool net_r2c_trade_other_agree(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = NULL;
	long actorID;
	READb_long(actorID);
	pActor = GetWorld()->m_ActorManager.FindActive(actorID);
	BOOL_ENSURE(pActor)
	CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);

	pTrade->Agree( &(pActor->mItem) );

	ShowSystemMessage(R(MSG_TRADE_AGREE));
	g_layerMain->m_formTrade->Refresh();

	return true;
	unguard;
}

bool net_r2c_trade_unlock(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = NULL;
	long actorID;
	READb_long(actorID);
	CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);
	BOOL_ENSURE(pTrade);

	if (GetWorld()->m_pPlayer->ID() == actorID)
	{
		// 以下步骤已作过处理
//		pTrade->UnLock();
//		g_layerMain->m_formTrade->m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
	}
	else
	{
		pActor = GetWorld()->m_ActorManager.FindActive(actorID);
		BOOL_ENSURE(pActor)
		pTrade->UnLock();

		g_layerMain->m_formTrade->m_trade.GetTradeOtherBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		g_layerMain->m_formTrade->m_trade.GetTradeOtherPetBag( &(GetWorld()->m_pPlayer->mItem) )->Clear();
		ShowSystemMessage(R(MSG_TRADE_RECONFIRM));
	}
	g_layerMain->m_formTrade->Refresh();

	return true;
	unguard;
}

bool net_r2c_trade_cancel(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcActor *pActor = NULL;
	long actorID;
	READb_long(actorID);
	pActor = GetWorld()->m_ActorManager.FindActive(actorID);
	if (!pActor)		// 如果交易对象已经断线或离开了
	{
		ShowSystemMessage(R(MSG_TRADE_LEAVE_CANCEL));
	}
	CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);
	BOOL_ENSURE(pTrade)

	ShowSystemMessage(R(MSG_TRADE_CANCEL));

	g_layerMain->m_formTrade->SetPlayer(NULL, NULL);
	g_layerMain->m_formTrade->Hide();

	// 所有的道具都变为可见
	GetWorld()->m_pPlayer->mItem.m_Bag.SetAllVisible(true);
	GetWorld()->m_pPlayer->mItem.m_MissionBag.SetAllVisible(true);
	GetWorld()->m_pPlayer->mItem.m_PetBag.SetAllVisible(true);
	g_layerMain->m_fromPlayerItems->Refresh(true);

	return true;
	unguard;
}

bool net_r2c_trade_deal(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long actorID;
	READb_long(actorID);
	CTrade *pTrade = &(g_layerMain->m_formTrade->m_trade);
	BOOL_ENSURE(pTrade)

	ShowSystemMessage(R(MSG_TRADE_SUCCEED));

	g_layerMain->m_formTrade->SetPlayer(NULL, NULL);
	g_layerMain->m_formTrade->Hide();
	g_layerMain->m_fromPlayerItems->Refresh(true);

	return true;
	unguard;
}

bool net_r2c_requstID(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	long npcID;
	READb_long(npcID);
	if ( 0 != npcID)
	{
		g_layerMain->m_formShop->Show(true,npcID);
	}
	return true;
	unguard;
}

bool net_r2c_shop_buy_query_response(short cmdID, CG_CmdPacket *pPacket)
 {
	guard;

	g_layerMain->m_formShop->UnlockOperate();

    if (!g_layerMain->m_formShop->IsVisible())
        return true;

    g_layerMain->m_formShop->m_dwLastRecvData = rtGetMilliseconds();

    std::vector<SNpcSellRareItem>& arrRares = g_layerMain->m_formShop->m_RareItems;
    arrRares.clear();
    short nRareCount;
    long ServerTime;
	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	long npcID;
	short priceRate;
	READb_long(npcID);
	READb_short(priceRate);
	CBag *pBag = &(g_layerMain->m_formShop->m_bagUp);
	CHECK(pBag);

	BOOL_ENSURE(pBag->Serialize(0, pPacket));
    READb_short(nRareCount);
    for (short i=0; i<nRareCount; i++)
    {
        SNpcSellRareItem sri;
        BOOL_ENSURE(sri.Serialize(pPacket));
        arrRares.push_back(sri);
    }
    READb_long(ServerTime);
    g_layerMain->m_formShop->m_TimeDelta = ServerTime - (DWORD)time(NULL);
    g_layerMain->m_formShop->m_bagDown.Clear();
	//g_layerMain->m_formShop->m_bagDownSell.Clear();
    g_layerMain->m_formShop->m_RareItemsToBuy.clear();

	g_layerMain->m_formShop->m_fPriceRate *= priceRate/100.0f;
	/*g_layerMain->m_formShop->SetScrollBar();*/
	g_layerMain->m_formShop->Refresh();
	return true;

	unguard;
}

//heten
bool net_r2c_point_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;

	short sPoint;
	READb_short(sPoint);
	pPlayer->m_Point += sPoint;

	g_layerMain->m_formShop->Refresh();

	unguard;
}
//end

bool net_r2c_shop_buy_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formShop->UnlockOperate();

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CBag bag(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager);
	char result;
	READb_char(result);
	//BOOL_ENSURE(bag.Serialize(0, pPacket))
	if (result)
	{
		//bag.MoveToContainer(&(pPlayer->mItem));
		//CHECK(pPlayer->mItem.RemoveMoney(bag.GetMoney()));
		ShowSystemMessage(R(MSG_BUY_SUCCEED));
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}
	else
	{
		ShowSystemMessage(R(MSG_BUY_CANCEL));
	}
	g_layerMain->m_formShop->m_bagDown.Clear();
	g_layerMain->m_formShop->m_RareItemsToBuy.clear();
	g_layerMain->m_formShop->Refresh();
	
	g_layerMain->m_formPetMain->OnPreDraw();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	return true;

	unguard;
}

bool net_r2c_shop_sell_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formShop->UnlockOperate();

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CBag bag(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, GetWorld()->m_pItemManager);
	char errorCode;
	READb_char(errorCode);
    switch (errorCode)
    {
    case itemmsg_success:
        ShowSystemMessage(R(MSG_SELL_SUCCEED));
        g_layerMain->m_fromPlayerItems->RefreshQuickStart();
        break;
    case itemmsg_bankpass_error:
        PlayerFailOperate(R(MSG_NEED_BANKPASS_ERROR));
        break;
    case itemmsg_unknown_error:
    default:
        ShowSystemMessage(R(MSG_SELL_CANCEL));
        break;
	}
	g_layerMain->m_formPetMain->OnPreDraw();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_formShop->m_bagDownSell.Clear();
	g_layerMain->m_formShop->m_RareItems.clear();
	g_layerMain->m_formShop->Refresh();
	return true;

	unguard;
}


bool net_r2c_pshop_start_response(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formPlayerShop->UnlockOperate();

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	char errorCode;
	READb_char(errorCode);

    switch (errorCode)
    {
    case itemmsg_success:
        g_layerMain->m_formPlayerShop->OnRecvStart();
        break;
    case itemmsg_bankpass_error:
        PlayerFailOperate(R(MSG_NEED_BANKPASS_ERROR));
        break;
    case itemmsg_unknown_error:
    default:
        break;
    }

	return true;

	unguard;
}

bool net_r2c_pshop_stop_response(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formPlayerShop->UnlockOperate();

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	char bSuccess;
	READb_char(bSuccess);
	if (bSuccess)
		g_layerMain->m_formPlayerShop->OnRecvStop();

	return true;

	unguard;
}

bool net_r2c_pshop_content(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	//g_layerMain->m_formLookPlayerShop->UnlockOperate();

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	if(!g_layerMain->m_formLookPlayerShop->IsVisible())
		g_layerMain->m_formLookPlayerShop->Init();
	//char bSell;
	//READb_char(bSell);
	BOOL_ENSURE( g_layerMain->m_formLookPlayerShop->m_PShop.Serialize(pPacket) )
	//g_layerMain->m_formLookPlayerShop->OnPShopDataChanged();

	GcActor* pActor = GetWorld()->m_ActorManager.FindAll(g_layerMain->m_formLookPlayerShop->m_PShop.GetPlayerID());
	g_layerMain->m_formLookPlayerShop->SetShopSeller(pActor->GetName());
	RECT winRect;
	::GetClientRect((HWND)GetDevice()->GetWndHandle(),&winRect);
	RtwRect rect1 = g_layerMain->m_formLookPlayerShop->OnGetFormRect();
	RtwRect rect2 = g_layerMain->m_fromPlayerItems->OnGetFormRect();

	int x = (winRect.right - winRect.left - rect1.getWidth()-rect2.getWidth())*0.5 +winRect.left;
	int y = (winRect.bottom - winRect.top - rect1.getHeight())*0.5 +winRect.top;
	g_layerMain->m_formLookPlayerShop->SetPos(x, y);
	g_layerMain->m_fromPlayerItems->SetPos(x+rect1.getWidth(), y);
	g_layerMain->m_fromPlayerItems->Show();
	if (!pActor)
		return true;
	else 
	{
		if (pActor->Distance(GetWorld()->m_pPlayer) > g_layerMain->m_formLookPlayerShop->m_nMaxDistance)
			return true;
	}

	if (g_layerMain->m_formLookPlayerShop->IsVisible())
		g_layerMain->m_formLookPlayerShop->Refresh();
	else
	{
		g_layerMain->m_formLookPlayerShop->Show();
	}

	return true;

	unguard;
}

bool net_r2c_pshop_buy_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	//g_layerMain->m_formLookPlayerShop->UnlockOperate();

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CPrivateShop *pPShop = &(g_layerMain->m_formLookPlayerShop->m_PShop);

	long dwPlayerFrom;
	char bSuccess, bReason;
	short count;
	long ItemID_oldID;
	READb_long(dwPlayerFrom);
	READb_char(bSuccess);
	READb_char(bReason);
	READb_long(ItemID_oldID);
	READb_short(count);

	char msg256[256];
	GcActor *pPlayerFrom = GetWorld()->m_ActorManager.FindAll(dwPlayerFrom);
	int money = 0;
	if (bSuccess)
	{
		int page, gridI, gridJ;
		int itemNum = 0;
		SItemID item;
		if (!pPShop->GetSellItemBag()->Find(ItemID_oldID, &page, &gridI, &gridJ))
		{
			CHECK("[严重错误] net_r2c_pshop_buy_result");
		}
		item = pPShop->GetSellItemBag()->GetItem(page, gridI, gridJ);
		SItemBase *pItemB = GetWorld()->m_pItemManager->GetItem(item.type);
		if(ItemCanPile(item) && item.count >= count)
		{
			if(count)
			{
				if(item.count > count)
				{
					item.count -= count;
					// 处理玩家买卖背包里的东西
					pPShop->GetSellItemBag()->SetItem(page,gridI,gridJ,item);
					money = count*pPShop->GetPrice(item.id);
					itemNum = count;
					rt2_snprintf(msg256, 256, R(MSG_ITEM_BUYFROMOTHERS), pPShop->GetPrice(item.id), pPlayerFrom->GetName(),itemNum,pItemB->name,money);
				}else if(item.count == count)
				{
					itemNum = count;
					money = count*pPShop->GetPrice(item.id);
					rt2_snprintf(msg256, 256, R(MSG_ITEM_BUYFROMOTHERS), pPShop->GetPrice(item.id), pPlayerFrom->GetName(),itemNum,pItemB->name,money);
					pPShop->RemoveSellItem(item.id);
				}
			}
		}else// 当
		{
			itemNum = 1;
			money = pPShop->GetPrice(item.id);
			rt2_snprintf(msg256, 256, R(MSG_ITEM_BUYFROMOTHERS), money/itemNum, pPlayerFrom->GetName(),itemNum,pItemB->name,money);
			pPShop->RemoveSellItem(item.id);
		}
		ShowSystemMessage(msg256);
		//CHECK(pPlayer->mItem.RemoveMoney(money) );
		if(g_layerMain->m_formLookPlayerShop->IsVisible())
			g_layerMain->m_formLookPlayerShop->Refresh();
	}
	else
	{
		if (bReason == ITEM_REASON_PSHOP_NOITEM)
			PlayerFailOperate(R(MSG_BUY_FAIL_REASON_OTHERBUY));
		else if (bReason == ITEM_REASON_PSHOP_NOPLAYER)
		{
			PlayerFailOperate(R(MSG_BUY_FAIL_REASON_LEFT));
		}
		else if (bReason == ITEM_REASON_PSHOP_NOPSHOP)
		{
			PlayerFailOperate(R(MSG_BUY_FAIL_REASON_CLOSE));
		}
		else
		{
			PlayerFailOperate(R(MSG_BUY_FAIL));
		}
	}

	return true;

	unguard;
}

bool net_r2c_pshop_sell_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CPrivateShop *pPShop = &(g_layerMain->m_formLookPlayerShop->m_PShop);

	long dwPlayerFor;
	char bSuccess, bReason;
	short count;
	long ItemID_oldID,vFakeItemID;
	READb_long(dwPlayerFor);
	READb_char(bSuccess);
	READb_char(bReason);
	READb_long(vFakeItemID);
	READb_long(ItemID_oldID);
	READb_short(count);
	DWORD FakeItemID = (DWORD)vFakeItemID;

	char msg256[256];
	GcActor *pPlayerFor = GetWorld()->m_ActorManager.FindAll(dwPlayerFor);
	int money = 0;
	if (bSuccess)
	{
		int page, gridI, gridJ;
		SItemID item;
		int itemNum = 0;
		if (!pPShop->GetBuyItemBag()->Find(FakeItemID, &page, &gridI, &gridJ))
		{
			CHECK("[严重错误] net_r2c_pshop_buy_result");
		}
		item = pPShop->GetBuyItemBag()->GetItem(page, gridI, gridJ);
		SItemBase *pItemB = GetWorld()->m_pItemManager->GetItem(item.type);
		if(ItemCanPile(item) && item.count >= count)
		{
			if(count)
			{
				if(item.count > count)
				{
					item.count -= count;
					// 处理玩家买卖背包里的东西
					pPShop->GetBuyItemBag()->SetItem(page,gridI,gridJ,item);
					money = count*pPShop->GetPrice(item.id);
					itemNum = count;
				}else if(item.count == count)
				{
					itemNum = count;
					money = count*pPShop->GetPrice(item.id);
					pPShop->RemoveBuyItem(item.id);
				}
			}
		}else // 当
		{
			money = pPShop->GetPrice(item.id);
			pPShop->RemoveBuyItem(item.id);
			itemNum = 1;
		}
		rt2_snprintf(msg256,256,R(MSG_TRADE_SELLITEMSTOPLAYER),pPlayerFor->GetName(),itemNum,pItemB->name,money);
		ShowSystemMessage(msg256);
		//CHECK(pPlayer->mItem.AddMoney(money) );
		if(g_layerMain->m_formLookPlayerShop->IsVisible())
			g_layerMain->m_formLookPlayerShop->Refresh();
	}
	else
	{
		if (bReason == ITEM_REASON_PSHOP_NOITEM)
			PlayerFailOperate(R(MSG_TRADE_FAILCAUSEOTHERSBUY));
		else if (bReason == ITEM_REASON_PSHOP_NOPLAYER)
		{
			PlayerFailOperate(R(MSG_TRADE_FAILCAUSELEAVE));
		}
		else if (bReason == ITEM_REASON_PSHOP_NOPSHOP)
		{
			PlayerFailOperate(R(MSG_TRADE_FAILCAUSESHOPSHUT));
		}
		else
		{
			PlayerFailOperate(R(MSG_TRADE_SELL_FAIL ));
		}
	}

	return true;
	unguard;
};
bool net_r2c_pshop_other_buy(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CPrivateShop *pPShop = &(g_layerMain->m_formPlayerShop->m_PShop);

	g_layerMain->m_formPlayerShop->UnlockOperate();

	long dwCustomerID;
	long ItemID_id;
	long price,iPrice;
	short count;
	SDWORDArray idArray;
	READb_long(dwCustomerID);
	READb_long(ItemID_id);
	READb_long(price);
	READb_short(count);

	GcActor* pCustomer = GetWorld()->m_ActorManager.FindAll(dwCustomerID);
	char msg128[128];

	int page, gridI, gridJ;
	int itemNum = 0;
	SItemID item;
	if (!pPShop->GetSellItemBag()->Find(ItemID_id, &page, &gridI, &gridJ))
	{
		CHECK("[严重错误] net_r2c_pshop_other_buy");
	}
	item = pPShop->GetSellItemBag()->GetItem(page, gridI, gridJ);
	SItemBase *pItemB = GetWorld()->m_pItemManager->GetItem(item.type);
	iPrice= pPShop->GetPrice(item.id);
	if(ItemCanPile(item) && item.count >= count)
	{
		if(count)
		{
			if(item.count > count)
			{
				item.count -= count;
				// 处理玩家买卖背包里的东西
				pPShop->GetSellItemBag()->SetItem(page,gridI,gridJ,item);
				// 处理玩家背包的东西
				//CHECK(pPlayer->mItem.AddMoney(price*count));
				pPlayer->mItem.m_Bag.Find(ItemID_id, &page, &gridI, &gridJ);
				pPlayer->mItem.m_Bag.SetItem(page,gridI,gridJ,item);
				itemNum = count;
			}else if(item.count == count)
			{
				itemNum = count;
				pPShop->RemoveSellItem(item.id);
				//CHECK(pPlayer->mItem.AddMoney(price*count));
				pPlayer->mItem.RemoveItem(item);
			}
		}
	}else // 当
	{
		pPlayer->mItem.RemoveItem(item);
		//CHECK(pPlayer->mItem.AddMoney(price));
		pPShop->RemoveSellItem(item.id);
		itemNum = 1;
	}

	if (pCustomer)
	{
		if (pItemB)
		{
			rt2_snprintf(msg128, 128, R(MSG_PLAYER_BUY_ITEM), pCustomer->GetName(),iPrice,itemNum, pItemB->name,price);
			//msg128[127] = 0;
			ShowSystemMessage(msg128);
		}
	}

	//g_layerMain->m_formPShop->m_PShop.GetSellItemBag()->Pinch();

	g_layerMain->m_formPlayerShop->Refresh();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	return true;

	unguard;
}

bool net_r2c_pshop_other_sell(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
	CPrivateShop *pPShop = &(g_layerMain->m_formPlayerShop->m_PShop);

	g_layerMain->m_formPlayerShop->UnlockOperate();

	long dwCustomerID;
	long ItemID_id,FakeItemID;
	long price;
	short count;
	SDWORDArray idArray;
	READb_long(dwCustomerID);
	READb_long(FakeItemID);
	READb_long(ItemID_id);
	READb_long(price);
	READb_short(count);

	GcActor* pCustomer = GetWorld()->m_ActorManager.FindAll(dwCustomerID);
	char msg128[128];

	int page, gridI, gridJ;
	int itemNum = 0;
	SItemID item;
	if (!pPShop->GetBuyItemBag()->Find(FakeItemID, &page, &gridI, &gridJ))
	{
		CHECK("[严重错误] net_r2c_pshop_other_buy");
	}
	item = pPShop->GetBuyItemBag()->GetItem(page, gridI, gridJ);
	if(ItemCanPile(item) && item.count >= count)
	{
		if(count)
		{
			if(item.count > count)
			{
				item.count -= count;
				// 处理玩家买卖背包里的东西
				pPShop->GetBuyItemBag()->SetItem(page,gridI,gridJ,item);
				// 处理玩家背包的东西
				//CHECK(pPlayer->mItem.RemoveMoney(price*count));
				itemNum = count;
			}else if(item.count == count)
			{
				itemNum = count;
				pPShop->RemoveBuyItem(item.id);
				//CHECK(pPlayer->mItem.RemoveMoney(price*count));
			}
		}
	}else // 当
	{
		//CHECK(pPlayer->mItem.RemoveMoney(price));
		pPShop->RemoveBuyItem(item.id);
		itemNum = 1;
	}

	if (pCustomer)
	{
		SItemBase *pItemB = GetWorld()->m_pItemManager->GetItem(item.type);
		if (pItemB)
		{
			//msg128[127] = 0;
			rt2_snprintf(msg128, 128, R(MSG_TRADE_PLAYERSELLTOME), pCustomer->GetName(),pPShop->GetPrice(item.id),itemNum, pItemB->name,price);
			g_layerMain->m_formPlayerShop->OnSubBuyMoney(price);
			ShowSystemMessage(msg128);
		}
	}

	//g_layerMain->m_formPShop->m_PShop.GetSellItemBag()->Pinch();

	g_layerMain->m_formPlayerShop->Refresh();
	
	g_layerMain->m_formPetMain->OnPreDraw();
	g_layerMain->m_fromPlayerItems->Refresh(true);
	g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	return true;

	unguard;
}

bool net_r2c_bank_query_response(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formBank->UnlockOperate();
	g_layerMain->m_formBank->m_bagDown.Clear();
	BOOL_ENSURE( g_layerMain->m_formBank->m_bagUp.Serialize(0, pPacket) );
    CBag* pBag = &(g_layerMain->m_formBank->m_bagUp);
    g_layerMain->m_formBank->m_BankSize = pBag->GetPageCount() * pBag->GetLineCount() * pBag->GetColCount();
    g_layerMain->m_formBank->m_BankItemCount = pBag->Count();
	g_layerMain->m_formBank->SetScrollBar();
	g_layerMain->m_formBank->Refresh();
	return true;

	unguard;
}

bool net_r2c_bank_store_response(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formBank->UnlockOperate();

	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	char bSuccess;
	READb_char(bSuccess);
	if (bSuccess)
	{
		//pCon->RemoveFromBag( &(g_layerMain->m_formBank->m_bagDown) );
		//CHECK(pCon->RemoveMoney(g_layerMain->m_formBank->m_bagDown.GetMoney()));
		ShowSystemMessage(R(MSG_STORE_SUCCESS));
		
		g_layerMain->m_formPetMain->OnPreDraw();
		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_formBank->Hide();
		g_layerMain->m_fromPlayerItems->RefreshQuickStart();
	}
	else
	{
		char reason;
		READb_char(reason);
		switch (reason)
		{
		case ITEM_REASON_CONTAINER_FULL:
			ShowSystemMessage(R(MSG_STORE_FAIL_REACHMAX));
			break;
		case ITEM_REASON_TOO_FAR:
		default:
			ShowSystemMessage(R(MSG_STORE_FAIL));
			g_layerMain->m_formBank->Hide();
			break;
		}
	}

	return true;

	unguard;
}

bool net_r2c_bank_withdraw_response(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	g_layerMain->m_formBank->UnlockOperate();

	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	char bSuccess;
	READb_char(bSuccess);
	if (bSuccess)
	{
		//g_layerMain->m_formBank->m_bagDown.MoveToContainer(pCon);
		//CHECK(pCon->AddMoney(g_layerMain->m_formBank->m_bagDown.GetMoney()));
		ShowSystemMessage(R(MSG_WITHDRAW_SUCCESS));
		
		g_layerMain->m_formPetMain->OnPreDraw();
		g_layerMain->m_fromPlayerItems->Refresh(true);
		g_layerMain->m_formBank->Hide();
	}
	else
	{
		char reason;
		READb_char(reason);
		switch (reason)
		{
		case ITEM_REASON_TOO_FAR:
		default:
			ShowSystemMessage(R(MSG_WITHDRAW_FAIL));
			g_layerMain->m_formBank->Hide();
			break;
		}
	}

	return true;

	unguard;
}

bool net_r2c_bank_addpass_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	READb_short(errorCode);

	char cTmp256[256];
	switch (errorCode)
	{
	case 0:
		rt2_sprintf(cTmp256, R(MSG_BANK_ADDPASS), R(G_SUCCEED));
		ShowSystemMessage(cTmp256);
		break;
	case 1:
		rt2_sprintf(cTmp256, R(MSG_BANK_ADDPASS), R(G_FAIL));
		PlayerFailOperate(cTmp256);
		break;
	default:
		break;
	}
	return true;

	unguard;
}

bool net_r2c_bank_removepass_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	READb_short(errorCode);

	char cTmp256[256];
	switch (errorCode)
	{
	case 0:
		rt2_sprintf(cTmp256, R(MSG_BANK_REMOVEPASS), R(G_SUCCEED));
		ShowSystemMessage(cTmp256);
		break;
	case 1:
		rt2_sprintf(cTmp256, R(MSG_BANK_REMOVEPASS), R(G_FAIL));
		PlayerFailOperate(cTmp256);
		break;
	default:
		break;
	}
	return true;

	unguard;
}

bool net_r2c_bank_changepass_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	READb_short(errorCode);

	char cTmp256[256];
	switch (errorCode)
	{
	case 0:
		rt2_sprintf(cTmp256, R(MSG_BANK_CHANGEPASS), R(G_SUCCEED));
		ShowSystemMessage(cTmp256);
		break;
	case 1:
		rt2_sprintf(cTmp256, R(MSG_BANK_CHANGEPASS), R(G_FAIL));
		PlayerFailOperate(cTmp256);
		break;
	default:
		break;
	}
	return true;

	unguard;
}

bool net_r2c_bank_pass_query(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long npcID;
	short opID;
	READb_long(npcID);
	READb_short(opID);

	switch (opID)
	{
	case 0: // 询问
		g_layerMain->m_formBankPass->Show(npcID, UIFormBankPass::sCheck);
		break;
	case 1: // 密码错误
		PlayerFailOperate(R(MSG_BANK_PASS_ERROR));
		g_layerMain->m_formBankPass->Show(npcID, UIFormBankPass::sCheck);
		break;
	default:
		break;
	}
	return true;

	unguard;
}

bool net_r2c_compose_item_begin(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long ComposeID;
	short ErrorCode;
	READb_long(ComposeID);
	READb_short(ErrorCode);

	g_layerMain->m_fromPlayerItems->LockOperate(true);

	g_layerMain->m_formItemCompose->OnRecvComposeBegin(ComposeID, ErrorCode);

	return true;

	unguard;
}

bool net_r2c_compose_item_end(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long ComposeID;
	short ErrorCode;
	READb_long(ComposeID);
	READb_short(ErrorCode);

	g_layerMain->m_fromPlayerItems->LockOperate(false);
	g_layerMain->m_formItemCompose->OnRecvComposeEnd(ComposeID, ErrorCode);

	return true;

	unguard;
}

bool net_r2c_decompose_item_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long itemID;
	short ErrorCode;
	READb_long(itemID);
	READb_short(ErrorCode);

	g_layerMain->m_fromPlayerItems->LockOperate(false);
	g_layerMain->m_formItemDecompound->SetItemID(ItemID_CreateInvalid());

	((CGameClientFrame*)GetApp())->CancelMouseCapture();

	switch (ErrorCode)
	{
	case itemmsg_success:
		ShowSystemMessage(R(MSG_ITEM_FENJIEOK));
		break;
	case itemmsg_decompose_bagfull:
		PlayerFailOperate(R(MSG_ITEM_FENJIENEEDKONG));
		break;
	default:
		break;
	}

	return true;

	unguard;
}

bool net_r2c_enchant_item_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long EquipmentItemID;
	long ScrollItemID;
	short ErrorCode;
	READb_long(EquipmentItemID);
	READb_long(ScrollItemID);
	READb_short(ErrorCode);

	g_layerMain->m_fromPlayerItems->LockOperate(false);

	switch (ErrorCode)
	{
	case itemmsg_success:
		ShowSystemMessage(R(MSG_ITEM_USEINCANTATION));
		break;
	case itemmsg_unknown_error:
		PlayerFailOperate(R(MSG_ITEM_USEINCANTATIONFAIL));
	default:
		break;
	}
	((CGameClientFrame*)GetApp())->CancelMouseCapture();

	return true;

	unguard;
}
bool net_r2c_fuse_gem_result(short cmdID, CG_CmdPacket *pPacket)
{
	long ItemNew;
	short ErrorCode;
	READb_short(ErrorCode);
	READb_long(ItemNew);
	g_layerMain->m_fromPlayerItems->LockOperate(false);

	switch (ErrorCode)
	{
	case itemmsg_success:
		ShowSystemMessage(R(MSG_ITEM_COMPOSEJEWEL));
		break;
	case itemmsg_unknown_error:
		PlayerFailOperate(R(MSG_ITEM_COMPOSEJEWELFAIL));
		break;
	default:
		break;
	}

	//if (g_layerMain->m_formPShop->IsVisible())
	//{
	//	g_layerMain->m_formPShop->OnCommonItemDeleted(Item1);
	//	g_layerMain->m_formPShop->OnCommonItemDeleted(Item2);
	//}

	g_layerMain->m_formGem->Refresh();//Hide();
	g_layerMain->m_fromPlayerItems->Refresh(true);

	return true;
}
bool net_r2c_fuse_item_result(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	long Item1, Item2;
	short ErrorCode;
	READb_short(ErrorCode);
	READb_long(Item1);
	READb_long(Item2);

	g_layerMain->m_fromPlayerItems->LockOperate(false);

	switch (ErrorCode)
	{
	case itemmsg_success:
		ShowSystemMessage(R(MSG_ITEM_RONGHEOK));
		break;
	case itemmsg_unknown_error:
		PlayerFailOperate(R(MSG_ITEM_RONGHEFAIL));
		break;
	default:
		break;
	}

	if (g_layerMain->m_formPlayerShop->IsVisible())
	{
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(Item1);
		g_layerMain->m_formPlayerShop->OnCommonItemDeleted(Item2);
	}

	g_layerMain->m_formItemFuse->Hide();
	g_layerMain->m_fromPlayerItems->Refresh(true);
    return true;

	unguard;
}

bool net_r2c_check_item(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	CItemContainerBase container(GetWorld()->m_pItemManager, &(GetWorld()->m_pPlayer->m_core));
	BOOL_ENSURE( container.Serialize(0, pPacket) )

	if ( container != GetWorld()->m_pPlayer->mItem )
	{
		LOG("发现Client与RegionServer的道具不一致：\n");
		LOG1("Region: %s\n", container.SaveToString());
		LOG1("Client: %s\n", GetWorld()->m_pPlayer->mItem.SaveToString());
		PlayerFailOperate(R(IMSG_DATA_ERROR)); // "服务器端与客户端的道具栏不一致"
	}

	return true;

	unguard;
}


// 测试Client与Server的道具栏是否一致
bool CItemNetHelper::NetSend_c2r_check_item()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_check_item);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_EquipItem(DWORD itemId, CItemContainerBase::EEquipParts part, EItemContainer container, int page, int gridI, int gridJ)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_equipitem);
	pPacket->WriteLong(itemId);
	pPacket->WriteByte(part);				// Part
	pPacket->WriteByte(container);			// container
	pPacket->WriteByte(page);				// page
	pPacket->WriteByte(gridI);				// line
	pPacket->WriteByte(gridJ);				// column
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_UnEquipItem(DWORD itemId, CItemContainerBase::EEquipParts part)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_unequipitem);
	pPacket->WriteLong(itemId);
	pPacket->WriteByte(part);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_MoveItem(EItemContainer container, int pageFrom, int iFrom, int jFrom, int pageTo, int iTo, int jTo)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_moveitem);
	pPacket->WriteByte(container);
	pPacket->WriteByte(pageFrom);
	pPacket->WriteByte(iFrom);
	pPacket->WriteByte(jFrom);
	pPacket->WriteByte(pageTo);
	pPacket->WriteByte(iTo);
	pPacket->WriteByte(jTo);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_DispartItem(EItemContainer container, int pageFrom, int iFrom, int jFrom, int pageTo, int iTo, int jTo,long num)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_dispart);
	pPacket->WriteByte(container);
	pPacket->WriteByte(pageFrom);
	pPacket->WriteByte(iFrom);
	pPacket->WriteByte(jFrom);
	pPacket->WriteByte(pageTo);
	pPacket->WriteByte(iTo);
	pPacket->WriteByte(jTo);
	pPacket->WriteLong(num);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_DeleteItem(DWORD itemId, EItemContainer container, int page, int gridI, int gridJ)
{
	//if(LOAD_UI("fmplayershop")->IsVisible())
	//{
	SItemID item;
	int i,j,p;
	GetPlayer()->mItem.m_Bag.FindID(itemId,&p,&i,&j);
	item= GetPlayer()->mItem.m_Bag.GetItem(p,i,j);
	g_layerMain->m_formPlayerShop->OnDeleteItem(item);
	//}
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_deleteitem);
	pPacket->WriteLong(itemId);
	pPacket->WriteByte(container);			// container
	pPacket->WriteByte(page);				// page
	pPacket->WriteByte(gridI);				// line
	pPacket->WriteByte(gridJ);				// column
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_PickItem(DWORD itemId)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_pickitem);
	pPacket->WriteLong(itemId);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_ThrowItem(DWORD itemId, EItemContainer container, int page, int gridI, int gridJ)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_throwitem);
	pPacket->WriteLong(itemId);
	pPacket->WriteByte(container);			// container
	pPacket->WriteByte(page);				// page
	pPacket->WriteByte(gridI);				// line
	pPacket->WriteByte(gridJ);				// column
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_player_use_item(DWORD itemId, EItemContainer container, int page, int gridI, int gridJ)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_use_item);
	pPacket->WriteLong(itemId);
	pPacket->WriteByte(container);			// container
	pPacket->WriteByte(page);				// page
	pPacket->WriteByte(gridI);				// line
	pPacket->WriteByte(gridJ);				// column
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_player_itemupdate(int pageItem, int iItem, int jItem, int pageScroll, int iScroll, int jScroll, int pageLuck, int iLuck, int jLuck)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_itemupdate);
	pPacket->WriteByte(pageItem);
	pPacket->WriteByte(iItem);
	pPacket->WriteByte(jItem);	
	pPacket->WriteByte(pageScroll);
	pPacket->WriteByte(iScroll);
	pPacket->WriteByte(jScroll);
	pPacket->WriteByte(pageLuck);
	pPacket->WriteByte(iLuck);
	pPacket->WriteByte(jLuck);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_player_itemlock(DWORD itemId, int page, int i, int j)
{
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_subsystem_item);
    pPacket->WriteShort(c2r_player_itemlock);
    pPacket->WriteLong(itemId);
    pPacket->WriteByte(page);
    pPacket->WriteByte(i);
    pPacket->WriteByte(j);
    return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_player_itemunlock(DWORD itemId, int page, int i, int j, const char* pass)
{
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_subsystem_item);
    pPacket->WriteShort(c2r_player_itemunlock);
    pPacket->WriteLong(itemId);
    pPacket->WriteByte(page);
    pPacket->WriteByte(i);
    pPacket->WriteByte(j);
    pPacket->WriteString((char*)pass);
    return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_player_peteat(DWORD petFrom, DWORD petTo)
{
    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_subsystem_item);
    pPacket->WriteShort(c2r_player_peteat);
    pPacket->WriteLong(petFrom);
    pPacket->WriteLong(petTo);
    return NetSend(pPacket);
}
bool CItemNetHelper::NetSend_c2r_player_switchweapon()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_player_switchweapon);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_player_UpDateItemBag()
{	
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_update_item);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_trade_invite(DWORD actorID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_trade_invite);
	pPacket->WriteLong(actorID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_trade_invite_response(DWORD actorID, BYTE bAgree)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_trade_invite_response);
	pPacket->WriteLong(actorID);
	pPacket->WriteByte((char)bAgree);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_trade_lock(SDWORDArray &idArray, SDWORDArray &idArrayPet, long money, const char* strBankPass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_trade_lock);
	idArray.Serialize(pPacket);
	idArrayPet.Serialize(pPacket);
	pPacket->WriteLong(money);
    pPacket->WriteString((char*)strBankPass);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_trade_unlock()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_trade_unlock);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_trade_deal()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_trade_deal);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_trade_cancel()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_trade_cancel);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_requstID()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_requstID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_shop_query(DWORD npcID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_shop_query);
	pPacket->WriteLong(npcID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_shop_buy(DWORD npcID, SDWORDArray &idArray, long moneyClientCalc)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_shop_buy);
	pPacket->WriteLong(npcID);
	pPacket->WriteLong(moneyClientCalc);
	idArray.Serialize(pPacket);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_shop_sell(DWORD npcID, SDWORDArray &idArray, long moneyClientCalc, const char* strBankPass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_shop_sell);
	pPacket->WriteLong(npcID);
	pPacket->WriteLong(moneyClientCalc);
	idArray.Serialize(pPacket);
    pPacket->WriteString((char*)strBankPass);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_item_autosell(SDWORDArray &idArray)	//for autosell        ac.ma
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_shop_autosell);
	idArray.Serialize(pPacket);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_item_repair(DWORD oneOrAll, DWORD ItemID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_repair_item);
	pPacket->WriteLong(oneOrAll);
	pPacket->WriteLong(ItemID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_pshop_start(CPrivateShop *pPShop, const char* bankPass)
{
	CHECK(pPShop);
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_pshop_start);
	pPShop->Serialize(pPacket);
    pPacket->WriteString("");
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_pshop_stop()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_pshop_stop);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_pshop_query_other(DWORD actorID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_pshop_query_other);
	pPacket->WriteLong(actorID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_pshop_buy(DWORD actorID, SDWORDArray &idArray, SDWORDArray &moneyArray)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_pshop_buy);
	pPacket->WriteLong(actorID);
	idArray.Serialize(pPacket);
	moneyArray.Serialize(pPacket);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_bank_query(DWORD npcID, const char* pass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_query);
	pPacket->WriteLong(npcID);
	pPacket->WriteString((char*)pass);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_bank_store(SDWORDArray &idArray, long money, DWORD npcID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_store);
	idArray.Serialize(pPacket);
	pPacket->WriteLong(money);
	pPacket->WriteLong(npcID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_bank_withdraw(SDWORDArray &idArray, long money, DWORD npcID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_withdraw);
	idArray.Serialize(pPacket);
	pPacket->WriteLong(money);
	pPacket->WriteLong(npcID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_bank_addpass(const char* pass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_addpass);
	pPacket->WriteString((char*)pass);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_bank_removepass(const char* pass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_removepass);
	pPacket->WriteString((char*)pass);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_bank_changepass(const char* oldPass, const char* newPass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_changepass);
	pPacket->WriteString((char*)oldPass);
	pPacket->WriteString((char*)newPass);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_compose_item(DWORD ComposeID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_compose_item);
	pPacket->WriteLong(ComposeID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_compose_item_stop(DWORD ComposeID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_compose_item_stop);
	pPacket->WriteLong(ComposeID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_decompose_item(DWORD itemID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_decompose_item);
	pPacket->WriteLong(itemID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_enchant_item(DWORD EquipmentItemID, DWORD ScrollItemID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_enchant_item);
	pPacket->WriteLong(EquipmentItemID);
	pPacket->WriteLong(ScrollItemID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_fuse_item(DWORD itemID1, DWORD itemID2)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_fuse_item);
	pPacket->WriteLong(itemID1);
	pPacket->WriteLong(itemID2);
	return NetSend(pPacket);
}
bool CItemNetHelper::NetSend_c2r_charm_item(DWORD ComposeID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_compose_item);
	pPacket->WriteLong(ComposeID);
	return NetSend(pPacket);
}

bool CItemNetHelper::NetSend_c2r_fuse_gem(DWORD itemID1)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_fuse_gem);
	pPacket->WriteLong(itemID1);
	return NetSend(pPacket);
}