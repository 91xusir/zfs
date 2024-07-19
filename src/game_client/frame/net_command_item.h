#ifndef NET_COMMAND_ITEM_H
#define NET_COMMAND_ITEM_H

class CPrivateShop;

bool ItemCmdProcess_client(CG_CmdPacket *cmd);

bool net_r2c_item_errorcode(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bag_addmoney(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bag_additem(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bag_removeitem(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bag_refresh(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_equipitem(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_unequipitem(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_usepileitem(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_move_item(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_dispart_item(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_use_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_itemupdate_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_gem_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_player_scrollhome(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_refresh_single_item(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_refresh_equip_item(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_exchange_hand_weapon(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pickitem_fail(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_equip_item_state(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_use_magicitem_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_other_additem(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_peteat_result(short cmdID, CG_CmdPacket *pPacket);

bool net_r2c_trade_other_invite(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_other_invite_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_lock(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_begin(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_other_agree(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_unlock(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_deal(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_trade_cancel(short cmdID, CG_CmdPacket *pPacket);

bool net_r2c_shop_buy_query_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_shop_buy_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_shop_sell_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_point_result(short cmdID, CG_CmdPacket *pPacket);	//heten
bool net_r2c_requstID(short cmdID, CG_CmdPacket *pPacket);

bool net_r2c_pshop_start_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pshop_stop_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pshop_content(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pshop_buy_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pshop_other_buy(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pshop_sell_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pshop_other_sell(short cmdID, CG_CmdPacket *pPacket);

bool net_r2c_bank_query_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_store_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_withdraw_response(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_addpass_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_removepass_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_changepass_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_pass_query(short cmdID, CG_CmdPacket *pPacket);

bool net_r2c_compose_item_begin(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_compose_item_end(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_decompose_item_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_enchant_item_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_fuse_item_result(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_fuse_gem_result(short cmdID, CG_CmdPacket *pPacket);

bool net_r2c_check_item(short cmdID, CG_CmdPacket *pPacket);




class CItemNetHelper
{
public:
	static bool NetSend_c2r_check_item();		// 测试Client与Server的道具栏是否一致

	static bool NetSend_PickItem(DWORD itemId);
	static bool NetSend_ThrowItem(DWORD itemId, EItemContainer container, int page, int gridI, int gridJ);
	static bool NetSend_DeleteItem(DWORD itemId, EItemContainer container, int page, int gridI, int gridJ);
	static bool NetSend_MoveItem(EItemContainer container, int pageFrom, int iFrom, int jFrom, int pageTo, int iTo, int jTo);
	static bool NetSend_DispartItem(EItemContainer container, int pageFrom, int iFrom, int jFrom, int pageTo, int iTo, int jTo,long num);
	static bool NetSend_EquipItem(DWORD itemId, CItemContainerBase::EEquipParts part, EItemContainer container, int page, int gridI, int gridJ);
	static bool NetSend_UnEquipItem(DWORD itemId, CItemContainerBase::EEquipParts part);
	static bool NetSend_c2r_player_use_item(DWORD itemId, EItemContainer container, int page, int gridI, int gridJ);
	static bool NetSend_c2r_player_itemupdate(int pageItem, int iItem, int jItem, int pageScroll, int iScroll, int jScroll, int pageLuck, int iLuck, int jLuck);
    static bool NetSend_c2r_player_itemlock(DWORD itemId, int page, int i, int j);
    static bool NetSend_c2r_player_itemunlock(DWORD itemId, int page, int i, int j, const char* pass);
    static bool NetSend_c2r_player_peteat(DWORD petFrom, DWORD petTo);
	static bool NetSend_c2r_player_switchweapon();
	static bool NetSend_c2r_player_UpDateItemBag();

	static bool NetSend_c2r_trade_invite(DWORD actorID);
	static bool NetSend_c2r_trade_invite_response(DWORD actorID, BYTE bAgree);
	static bool NetSend_c2r_trade_lock(SDWORDArray &idArray, SDWORDArray &idArrayPet, long money, const char* strBankPass);
	static bool NetSend_c2r_trade_unlock();
	static bool NetSend_c2r_trade_deal();
	static bool NetSend_c2r_trade_cancel();

	static bool NetSend_c2r_shop_query(DWORD npcID);
	static bool NetSend_c2r_requstID();
	static bool NetSend_c2r_shop_buy(DWORD npcID, SDWORDArray &idArray, long moneyClientCalc);
	static bool NetSend_c2r_shop_sell(DWORD npcID, SDWORDArray &idArray, long moneyClientCalc, const char* strBankPass);
	static bool NetSend_c2r_item_autosell(SDWORDArray &idArray)	;			//ac.ma
	static bool NetSend_c2r_item_repair(DWORD oneOrAll, DWORD ItemID);

	static bool NetSend_c2r_pshop_start(CPrivateShop *pPShop, const char* bankPass);
	static bool NetSend_c2r_pshop_stop();
	static bool NetSend_c2r_pshop_query_other(DWORD actorID);
	static bool NetSend_c2r_pshop_buy(DWORD actorID, SDWORDArray &idArray, SDWORDArray &moneyArray);

	static bool NetSend_c2r_bank_query(DWORD npcID, const char* pass);
	static bool NetSend_c2r_bank_store(SDWORDArray &idArray, long money, DWORD npcID);
	static bool NetSend_c2r_bank_withdraw(SDWORDArray &idArray, long money, DWORD npcID);
	static bool NetSend_c2r_bank_addpass(const char* pass);
	static bool NetSend_c2r_bank_removepass(const char* pass);
	static bool NetSend_c2r_bank_changepass(const char* oldPass, const char* newPass);

	static bool NetSend_c2r_compose_item(DWORD ComposeID);
	static bool NetSend_c2r_compose_item_stop(DWORD ComposeID);
	static bool NetSend_c2r_decompose_item(DWORD itemID);
	static bool NetSend_c2r_enchant_item(DWORD EquipmentItemID, DWORD ScrollItemID);
	static bool NetSend_c2r_fuse_item(DWORD itemID1, DWORD itemID2);
	static bool NetSend_c2r_rifine_trump( DWORD rifine );
	static bool NetSend_c2r_rifine_stop( DWORD rifine );
	static bool NetSend_c2r_charm_item( DWORD ComposeID );
	static bool NetSend_c2r_fuse_gem(DWORD itemID1);
};

#endif // NET_COMMAND_ITEM_H
