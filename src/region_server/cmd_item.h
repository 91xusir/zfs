#ifndef CMD_ITEM_H
#define CMD_ITEM_H


const int ItemID_UpdateDB_Circle		= 1000;			// ID每次消耗掉多少就更新数据库
class CRegionItemIDFactory
{
public:
	CRegionItemIDFactory();
	~CRegionItemIDFactory();
	void OnRecvItemIDFromDB(DWORD nowID, DWORD maxID);
	DWORD GetNextItemID();
	bool HaveGotIDFromDB() { return m_bGotIDFromDB; }

private:
	DWORD m_dwNextID;			// 下一个ID
	DWORD m_dwIDInDB;			// 数据库中的ID
	DWORD m_dwIDMaxInDB;
	bool m_bGotIDFromDB;		// 是否接受到从gws发来的ItemID初始化

	void UpdateDB();
};
extern CRegionItemIDFactory g_ItemIDFactory;
DWORD GetNextItemID();






class CRegionCreature;

int ItemCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);

#define ITEM_CMD_FUNCTION(func) int func(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)

// 物品操作
ITEM_CMD_FUNCTION(cmd_c2r_player_pickitem);
ITEM_CMD_FUNCTION(cmd_c2r_player_throwitem);
ITEM_CMD_FUNCTION(cmd_c2r_player_deleteitem);
ITEM_CMD_FUNCTION(cmd_c2r_player_moveitem);
ITEM_CMD_FUNCTION(cmd_c2r_player_equipitem);
ITEM_CMD_FUNCTION(cmd_c2r_player_switchweapon);
ITEM_CMD_FUNCTION(cmd_c2r_player_unequipitem);
ITEM_CMD_FUNCTION(cmd_c2r_player_use_item);
ITEM_CMD_FUNCTION(cmd_c2r_player_itemupdate);
ITEM_CMD_FUNCTION(cmd_c2r_player_itemlock);
ITEM_CMD_FUNCTION(cmd_c2r_player_itemunlock);
ITEM_CMD_FUNCTION(cmd_c2r_player_peteat);
ITEM_CMD_FUNCTION(cmd_c2r_player_dispart); //物品分离
ITEM_CMD_FUNCTION(cmd_c2r_player_repair);
ITEM_CMD_FUNCTION(cmd_c2r_player_updateBag); //整理背包

// 与另一玩家交易
ITEM_CMD_FUNCTION(cmd_c2r_trade_invite);
ITEM_CMD_FUNCTION(cmd_c2r_trade_invite_response);
ITEM_CMD_FUNCTION(cmd_c2r_trade_lock);
ITEM_CMD_FUNCTION(cmd_c2r_trade_unlock);
ITEM_CMD_FUNCTION(cmd_c2r_trade_deal);
ITEM_CMD_FUNCTION(cmd_c2r_trade_cancel);

// 与NPC交易
ITEM_CMD_FUNCTION(cmd_c2r_shop_query);
ITEM_CMD_FUNCTION(cmd_c2r_shop_buy);
ITEM_CMD_FUNCTION(cmd_c2r_shop_sell);
ITEM_CMD_FUNCTION(cmd_c2r_shop_cancel);
ITEM_CMD_FUNCTION(cmd_c2r_shop_autosell);//tim.yang  自动出售
ITEM_CMD_FUNCTION(cmd_c2r_requstID);
// 摆摊
ITEM_CMD_FUNCTION(cmd_c2r_pshop_start);
ITEM_CMD_FUNCTION(cmd_c2r_pshop_stop);
ITEM_CMD_FUNCTION(cmd_c2r_pshop_query_other);
ITEM_CMD_FUNCTION(cmd_c2r_pshop_buy);
ITEM_CMD_FUNCTION(cmd_c2r_pshop_sell);

// 钱庄
ITEM_CMD_FUNCTION(cmd_c2r_bank_query);
ITEM_CMD_FUNCTION(cmd_c2r_bank_store);
ITEM_CMD_FUNCTION(cmd_c2r_bank_withdraw);
ITEM_CMD_FUNCTION(cmd_c2r_bank_addpass);
ITEM_CMD_FUNCTION(cmd_c2r_bank_removepass);
ITEM_CMD_FUNCTION(cmd_c2r_bank_changepass);
ITEM_CMD_FUNCTION(cmd_c2r_bank_store_pet);
ITEM_CMD_FUNCTION(cmd_c2r_bank_withdraw_pet);
ITEM_CMD_FUNCTION(cmd_c2r_bank_query_pet);

// 生活技能
ITEM_CMD_FUNCTION(cmd_c2r_compose_item);
ITEM_CMD_FUNCTION(cmd_c2r_compose_item_stop);
ITEM_CMD_FUNCTION(cmd_c2r_decompose_item);
ITEM_CMD_FUNCTION(cmd_c2r_enchant_item);
ITEM_CMD_FUNCTION(cmd_c2r_fuse_item);
ITEM_CMD_FUNCTION(cmd_c2r_fuse_gem);

ITEM_CMD_FUNCTION(cmd_c2r_check_item);


class CItemCmdBuilder_Svr
{
public:
	static void Build_r2c_item_errorcode(short msg);
	static void Build_r2c_check_item(CItemContainerBase *pCon);
	static void Build_r2c_bag_additem(SItemID& item, EItemContainer container, int page, int gridI, int gridJ, int reason);
	static void Build_r2c_bag_removeitem(EItemContainer container, int page, int gridI, int gridJ, int reason);
	static void Build_r2c_bag_addmoney(long money, EItemContainer container, int reason);
	static void Build_r2c_player_usepileitem(DWORD itemID, int page, int gridI, int gridJ);
	static void Build_r2c_player_move_item(EItemContainer container, int pageFrom, int iFrom, int jFrom, int pageTo, int iTo, int jTo);
	static void Build_r2c_bag_refresh(EItemContainer container, CBag *pBag);
	static void Build_r2c_player_use_pet(DWORD actorID, SItemID& item, EItemContainer container, int page, int gridI, int gridJ);
	static void Build_r2c_player_itemupdate_result(SItemID &itemOrig, SItemID &scrollOrig, SItemID &luckOrig, SItemID &itemNew);
	static void Build_r2c_refresh_single_item(EItemContainer container, int page, int gridI, int gridJ, SItemID &item);
	static void Build_r2c_refresh_equip_item(CItemContainerBase::EEquipParts part, SItemID &item);
	static void Build_r2c_exchange_hand_weapon();
	static void Build_r2c_pickitem_fail();
	static void Build_r2c_equip_item_state(CItemContainerBase::EEquipParts part, char bValid);
    static void Build_r2c_use_magicitem_result(DWORD magicItemID, EMagicItemResult magicType, int money, SItemID& item, int exp);
	static void Build_r2c_other_additem(const char* actorName, SItemID &item, char reason);
    static void Build_r2c_peteat_result(DWORD petFrom, DWORD petTo, SItemID& itemNew);
	static void Build_r2c_player_dispart_item(EItemContainer container, int pageFrom, int iFrom, int jFrom, int pageTo, int iTo, int jTo,int num);

	static void Build_r2c_player_equipitem(SItemID& item, CItemContainerBase::EEquipParts part);
	static void Build_r2c_player_unequipitem(CItemContainerBase::EEquipParts part);

	static void Build_r2c_trade_other_invite(DWORD actorID);
	static void Build_r2c_trade_other_invite_response(DWORD actorID, BYTE bAgree);
	static void Build_r2c_trade_lock(DWORD actorID, BYTE errorCode);
	static void Build_r2c_trade_begin(DWORD actorID);
	static void Build_r2c_trade_other_agree(DWORD actorID);
	static void Build_r2c_trade_unlock(DWORD actorID);
	static void Build_r2c_trade_deal(DWORD actorID);
	static void Build_r2c_trade_cancel(DWORD actorID);
	static void Build_r2c_requstID();

    static void Build_r2c_shop_buy_query_response(DWORD npcID, CBag *pBag, std::vector<SNpcSellRareItem>& arrRare, short nPriceRate);
	static void Build_r2c_shop_buy_result(BYTE bSuccess);
	static void Build_r2c_shop_sell_result(BYTE bSuccess);
	static void Build_r2c_point_result(short spoint);	//heten

	static void Build_r2c_pshop_start_response(BYTE bSuccess);
	static void Build_r2c_pshop_stop_response(BYTE bSuccess);
	static void Build_r2c_pshop_content(CPrivateShop *pPShop);
	static void Build_r2c_pshop_buy_result(DWORD from, BYTE bSuccess, BYTE bReason,long ItemID,short amount);
	static void Build_r2c_pshop_other_buy(DWORD customer, long itemID,long money,short amount);

	static void Build_r2c_bank_query_response(CBag *pBag);
	static void Build_r2c_bank_store_response(BYTE bSuccess, BYTE reason = ITEM_REASON_NULL);
	static void Build_r2c_bank_withdraw_response(BYTE bSuccess, BYTE reason = ITEM_REASON_NULL);
	static void Build_r2c_bank_addpass_result(short errorCode);
	static void Build_r2c_bank_removepass_result(short errorCode);
	static void Build_r2c_bank_changepass_result(short errorCode);
	static void Build_r2c_bank_pass_query(DWORD npcID, short opID);
	static void Build_r2c_bank_query_response_pet(CBag *pBag);
	static void Build_r2c_bank_store_response_pet(BYTE bSuccess, BYTE reason = ITEM_REASON_NULL);
	static void Build_r2c_bank_withdraw_response_pet(BYTE bSuccess, BYTE reason = ITEM_REASON_NULL);

	static void Build_r2c_compose_item_begin(DWORD ComposeID, short ErrorCode);
	static void Build_r2c_compose_item_end(DWORD ComposeID, short ErrorCode);
	static void Build_r2c_decompose_item_result(DWORD itemID, short ErrorCode);
	static void Build_r2c_enchant_item_result(DWORD equipItemID, DWORD scrollItemID, short ErrorCode);
	static void Build_r2c_fuse_item_result(DWORD ErrorCode, DWORD Item1, DWORD Item2);
	static void Build_r2c_fuse_gem_result(DWORD ErrorCode, DWORD ItemNew);

	static void NetSend_r2g_bankpass(DWORD userID, short operateID, const char* pass);

	static void Build_r2c_pshop_sell_buy(DWORD customer,long FakeItemID ,long itemID,long money,short amount);
	static void Build_r2c_pshop_sell_result(DWORD from, BYTE bSuccess, BYTE bReason,long FakeItemID,long ItemID,short amount);
};


#endif //CMD_ITEM_H
