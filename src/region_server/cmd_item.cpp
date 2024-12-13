#include "region.h"
#include "region_server.h"

//// Temp for log NPC's income
//struct forLogNPCIncome
//{
//	time_t StartTimeT;
//	struct tm StartTime;
//	int Income;
//};
//HASH_STR(std::string, forLogNPCIncome) s_forLogNPCIncome;

CRegionItemIDFactory g_ItemIDFactory;

DWORD GetNextItemID() {
    return g_ItemIDFactory.GetNextItemID();
}

CRegionItemIDFactory::CRegionItemIDFactory() {
    m_dwNextID     = 0;
    m_dwIDInDB     = 0;
    m_dwIDMaxInDB  = 0;
    m_bGotIDFromDB = false;
}

CRegionItemIDFactory::~CRegionItemIDFactory() {}

void CRegionItemIDFactory::OnRecvItemIDFromDB(DWORD nowID, DWORD maxID) {
    m_bGotIDFromDB = true;

    m_dwNextID = nowID + 1;
    m_dwIDInDB = nowID;
    UpdateDB();
}

DWORD CRegionItemIDFactory::GetNextItemID() {
    if (m_dwNextID == 0) {
        static DWORD tmpIDFactory = 0xffffffff;  // 只提供给npc商店等地方使用
        return tmpIDFactory--;
    }

    if (m_dwNextID + ItemID_UpdateDB_Circle >= m_dwIDInDB) {
        UpdateDB();
    }
    return m_dwNextID++;
}

void CRegionItemIDFactory::UpdateDB() {
    m_dwIDInDB += ItemID_UpdateDB_Circle;

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_save_itemid);
    packet.WriteLong(g_cfgRs.lRegionID);
    packet.WriteLong(m_dwIDInDB);
    g_region->m_gws.EndSend();
}

//#define SAFE_READ(p)		{ if(!(p)) return CMD_ERROR_READ_DATA; }
#define SAFE_READ(p)                                                                               \
    {                                                                                              \
        if (!(p))                                                                                  \
            return CMD_ERROR_NONE;                                                                 \
    }
#define SAFE_ASSERT(p)                                                                             \
    {                                                                                              \
        if (!(p))                                                                                  \
            return CMD_ERROR_NONE;                                                                 \
    }

int ItemCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket* cmd) {
    short item_cmd_type;
    int   cmdResult;
    if (!cmd->ReadShort(&item_cmd_type))
        return CMD_ERROR_READ_DATA;

    if (CmdGiver->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    // 道具操作
    if (item_cmd_type == c2r_player_deleteitem)
        cmdResult = cmd_c2r_player_deleteitem(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_use_item)
        cmdResult = cmd_c2r_player_use_item(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_equipitem)
        cmdResult = cmd_c2r_player_equipitem(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_moveitem)
        cmdResult = cmd_c2r_player_moveitem(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_pickitem)
        cmdResult = cmd_c2r_player_pickitem(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_throwitem)
        cmdResult = cmd_c2r_player_throwitem(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_switchweapon)
        cmdResult = cmd_c2r_player_switchweapon(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_unequipitem)
        cmdResult = cmd_c2r_player_unequipitem(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_itemupdate)
        cmdResult = cmd_c2r_player_itemupdate(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_itemlock)
        cmdResult = cmd_c2r_player_itemlock(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_itemunlock)
        cmdResult = cmd_c2r_player_itemunlock(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_peteat)
        cmdResult = cmd_c2r_player_peteat(CmdGiver, cmd);
    else if (item_cmd_type == c2r_player_dispart)
        cmdResult = cmd_c2r_player_dispart(CmdGiver, cmd);
    else if (item_cmd_type == c2r_repair_item)
        cmdResult = cmd_c2r_player_repair(CmdGiver, cmd);
    else if (item_cmd_type == c2r_update_item)
        cmdResult = cmd_c2r_player_updateBag(CmdGiver, cmd);

    // 与玩家交易
    else if (item_cmd_type == c2r_trade_invite)
        cmdResult = cmd_c2r_trade_invite(CmdGiver, cmd);
    else if (item_cmd_type == c2r_trade_invite_response)
        cmdResult = cmd_c2r_trade_invite_response(CmdGiver, cmd);
    else if (item_cmd_type == c2r_trade_lock)
        cmdResult = cmd_c2r_trade_lock(CmdGiver, cmd);
    else if (item_cmd_type == c2r_trade_deal)
        cmdResult = cmd_c2r_trade_deal(CmdGiver, cmd);
    else if (item_cmd_type == c2r_trade_cancel)
        cmdResult = cmd_c2r_trade_cancel(CmdGiver, cmd);
    else if (item_cmd_type == c2r_trade_unlock)
        cmdResult = cmd_c2r_trade_unlock(CmdGiver, cmd);
    // 系统买卖
    else if (item_cmd_type == c2r_shop_query)
        cmdResult = cmd_c2r_shop_query(CmdGiver, cmd);
    else if (item_cmd_type == c2r_requstID)
        cmdResult = cmd_c2r_requstID(CmdGiver, cmd);
    else if (item_cmd_type == c2r_shop_buy)
        cmdResult = cmd_c2r_shop_buy(CmdGiver, cmd);
    else if (item_cmd_type == c2r_shop_sell)
        cmdResult = cmd_c2r_shop_sell(CmdGiver, cmd);
    else if (item_cmd_type == c2r_shop_autosell)  //tim.yang 自动出售
        cmdResult = cmd_c2r_shop_autosell(CmdGiver, cmd);
    // 摆摊
    else if (item_cmd_type == c2r_pshop_stop)
        cmdResult = cmd_c2r_pshop_stop(CmdGiver, cmd);
    else if (item_cmd_type == c2r_pshop_start)
        cmdResult = cmd_c2r_pshop_start(CmdGiver, cmd);
    else if (item_cmd_type == c2r_pshop_query_other)
        cmdResult = cmd_c2r_pshop_query_other(CmdGiver, cmd);
    else if (item_cmd_type == c2r_pshop_buy)
        cmdResult = cmd_c2r_pshop_buy(CmdGiver, cmd);
    else if (item_cmd_type == c2r_pshop_sell)
        cmdResult = cmd_c2r_pshop_sell(CmdGiver, cmd);
    // 仓库
    else if (item_cmd_type == c2r_bank_query)
        cmdResult = cmd_c2r_bank_query(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_store)
        cmdResult = cmd_c2r_bank_store(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_withdraw)
        cmdResult = cmd_c2r_bank_withdraw(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_addpass)
        cmdResult = cmd_c2r_bank_addpass(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_removepass)
        cmdResult = cmd_c2r_bank_removepass(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_changepass)
        cmdResult = cmd_c2r_bank_changepass(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_store_pet)
        cmdResult = cmd_c2r_bank_store_pet(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_withdraw_pet)
        cmdResult = cmd_c2r_bank_withdraw_pet(CmdGiver, cmd);
    else if (item_cmd_type == c2r_bank_query_pet)
        cmdResult = cmd_c2r_bank_query_pet(CmdGiver, cmd);
    // 生活技能
    else if (item_cmd_type == c2r_compose_item)
        cmdResult = cmd_c2r_compose_item(CmdGiver, cmd);
    else if (item_cmd_type == c2r_compose_item_stop)
        cmdResult = cmd_c2r_compose_item_stop(CmdGiver, cmd);
    else if (item_cmd_type == c2r_decompose_item)
        cmdResult = cmd_c2r_decompose_item(CmdGiver, cmd);
    else if (item_cmd_type == c2r_enchant_item)
        cmdResult = cmd_c2r_enchant_item(CmdGiver, cmd);
    else if (item_cmd_type == c2r_fuse_item)
        cmdResult = cmd_c2r_fuse_item(CmdGiver, cmd);
    else if (item_cmd_type == c2r_fuse_gem)
        cmdResult = cmd_c2r_fuse_gem(CmdGiver, cmd);
    // 测试
    else if (item_cmd_type == c2r_check_item)
        cmdResult = cmd_c2r_check_item(CmdGiver, cmd);

    else
        return CMD_ERROR_READ_DATA;

    return cmdResult;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_pickitem) {
    long id;
    SAFE_READ(cmd->ReadLong(&id))

    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;

    DWORD dwTick = rtGetMilliseconds();

    // 检查两次捡道具的时间间隔, 如果两次检道具的时间在500毫秒内就认为检查到外挂
    if (cre->m_dwLastPickItemTime && cre->m_dwLastPickItemTime + 500 > dwTick) {
        cre->PossiblyUsingWG();
        cre->m_dwLastPickItemTime = dwTick;
    }

    // CHECK
    // 道具必须存在，并且玩家与它的距离不超出范围
    // 玩家的背包必须有空间存放
    CRegionItem* rgItem = CmdGiver->m_scene->FindItem(id);
    if (!rgItem)
        return CMD_ERROR_NONE;
    if (cre->Distance(rgItem->m_pos) > 300)
        return CMD_ERROR_NONE;
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    if ((DWORD)rgItem->m_masterKeepTime <= dwTick)
        rgItem->m_masterId = 0;

    bool bCanPick = false;

    if (rgItem->m_masterId == 0) {
        bCanPick = true;
    } else {
        if (CmdGiver->m_oId == (ULONG)rgItem->m_masterId)
            bCanPick = true;
        else {
            if (CmdGiver->m_pTeam) {
                int teamNum = CmdGiver->m_pTeam->mData.Size();
                if (teamNum != 0) {
                    SAFE_ASSERT(CmdGiver->m_userInfo);
                    CRegionCreature* pMaster = CmdGiver->m_scene->FindCreature(rgItem->m_masterId);
                    if (pMaster && pMaster->m_userInfo) {
                        int index = CmdGiver->m_pTeam->mData.Find(pMaster->m_userInfo->m_userId);
                        if (index != CmdGiver->m_pTeam->mData.End())
                            bCanPick = true;
                    }
                }
            }
        }
    }

    if (!bCanPick) {
        CItemCmdBuilder_Svr::Build_r2c_pickitem_fail();
        CmdGiver->RecvCmd(g_sendCmd);

        return CMD_ERROR_NONE;
    }

    SItemID item = rgItem->m_item;

    if (item.cBind == IB_BindWhenPick)
        item.cBind = IB_Binded;

    CBag*          pBag;
    EItemContainer c;
    if (ItemIsMissionItem(item)) {
        pBag = &(pCon->m_MissionBag);
        c    = ITEM_CONTAINER_PLAYER_MISSION_BAG;
    } else {
        pBag = &(pCon->m_Bag);
        c    = ITEM_CONTAINER_PLAYER_COMMON_BAG;
    }

    int page, i, j;
    if (ItemIsPet(item))
        return CMD_ERROR_NONE;
    if (!pBag->AddItem(item, &page, &i, &j)) {
        return CMD_ERROR_NONE;
    } else {
        CmdGiver->m_nGetItemCount++;
        int DropUserID = rgItem->m_dropUserID;

        CRegionItem* rgitem = CmdGiver->m_scene->RemoveItem(rgItem->m_oId);
        if (rgitem)
            g_factory->DestroyObject(rgitem);

        CItemCmdBuilder_Svr::Build_r2c_bag_additem(item, c, page, i, j, ITEM_REASON_PICK);
        CmdGiver->RecvCmd(g_sendCmd);

        // 如果在队伍里面，通知其他的队伍成员
        if (CmdGiver->m_pTeam) {
            for (int index = CmdGiver->m_pTeam->mData.Begin();
                 index != CmdGiver->m_pTeam->mData.End(); index++) {
                if (CmdGiver->m_pTeam->mData[index].mDBID == CmdGiver->m_userInfo->m_userId)
                    continue;

                CRegionUser* pUser = g_region->FindUser(CmdGiver->m_pTeam->mData[index].mDBID);
                if (pUser && pUser->m_dummy) {
                    CItemCmdBuilder_Svr::Build_r2c_other_additem(CmdGiver->m_core.Name.c_str(),
                                                                 item, ITEM_REASON_PICK);
                    pUser->RecvCmd(g_sendCmd);
                }
            }
        }

        if (ItemIsRare(item, g_region->m_pItemManager)) {
            // log到数据库
            string strItem = rtFormatNumber(GetItemSerializeVersion());
            strItem += ",";
            strItem += item.SaveToString(g_region->m_pItemManager);

            CG_CmdPacket& packet = g_region->m_gws.BeginSend();
            packet.WriteShort(r2g_log_event);
            packet.WriteByte(PLT_PickItem);
            packet.WriteLong(cre->m_userInfo->m_userId);
            packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
            packet.WriteString((char*)strItem.c_str());                           // strParam1
            packet.WriteLong(item.color);
            packet.WriteLong(DropUserID);
            g_region->m_gws.EndSend();
        }
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_throwitem) {
    CRegionCreature*      cre  = CmdGiver;
    CRegionItemContainer* pCon = cre->m_pItemContainer;
    SAFE_ASSERT(pCon)

    // CHECK
    // 在交易，买卖，摆摊时不准丢弃
    // 物品有可丢弃属性
    if (cre->m_pTrade || cre->m_pPShop)
        return CMD_ERROR_NONE;

    char container;
    char page, i, j;
    long id;
    SAFE_READ(cmd->ReadLong(&id))
    SAFE_READ(cmd->ReadByte(&container))
    SAFE_READ(cmd->ReadByte(&page))
    SAFE_READ(cmd->ReadByte(&i))
    SAFE_READ(cmd->ReadByte(&j))

    SItemID item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page, i, j);
    SAFE_ASSERT(item.id == id);
    SAFE_ASSERT(item.cLock == 0);

    CBag* pBag = NULL;
    if (container == ITEM_CONTAINER_PLAYER_COMMON_BAG) {
        pBag = &(pCon->m_Bag);
        if (pCon->CanItemTrade(1, page, i, j) != TR_Can)
            return CMD_ERROR_NONE;
    } else if (container == ITEM_CONTAINER_PLAYER_MISSION_BAG) {
        pBag = &(pCon->m_MissionBag);
        if (pCon->CanItemTrade(0, page, i, j) != TR_Can)
            return CMD_ERROR_NONE;
    } else if (container == ITEM_CONTAINER_PLAYER_PET_BAG) {
        pBag = &(pCon->m_PetBag);
        if (pCon->CanItemTrade(2, page, i, j) != TR_Can)
            return CMD_ERROR_NONE;
    } else
        return CMD_ERROR_NONE;

    SAFE_ASSERT(ItemCanDrop(item))
    SAFE_ASSERT(pBag->RemoveItem(page, i, j))

    if (item.cBind == IB_Binded)
        return CMD_ERROR_NONE;
    ////////////////lyytodo 物品丢弃在地上
    CRegionItem* rgItem      = (CRegionItem*)g_factory->CreateObject(OBJECT_TYPE_ITEM);
    rgItem->m_item           = item;
    rgItem->m_masterKeepTime = rtGetMilliseconds() + ITEM_MASTER_KEEP_TIME;
    rgItem->m_masterId       = 0;  //CmdGiver->m_oId;
    rgItem->m_dropUserID     = cre->m_userInfo->m_userId;
    if (!CmdGiver->m_scene->AddItem(rgItem, cre->m_pos, 0)) {
        g_factory->DestroyObject(rgItem);
        return CMD_ERROR_NONE;
    }
    ///////////////////
    // 通知Player，去掉此物品
    CItemCmdBuilder_Svr::Build_r2c_bag_removeitem((EItemContainer)container, page, i, j,
                                                  ITEM_REASON_THROW);
    CmdGiver->RecvCmd(g_sendCmd);

    if (ItemIsRare(item, g_region->m_pItemManager)) {
        // log到数据库
        string strItem = rtFormatNumber(GetItemSerializeVersion());
        strItem += ",";
        strItem += item.SaveToString(g_region->m_pItemManager);

        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_log_event);
        packet.WriteByte(PLT_ThrowItem);
        packet.WriteLong(cre->m_userInfo->m_userId);
        packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
        packet.WriteString((char*)strItem.c_str());                           // strParam1
        g_region->m_gws.EndSend();
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_deleteitem) {
    CRegionCreature*      cre  = CmdGiver;
    CRegionItemContainer* pCon = cre->m_pItemContainer;
    SAFE_ASSERT(pCon)
    SAFE_ASSERT(cre->m_userInfo)

    // CHECK
    // 在交易，买卖，摆摊时不准删除
    // 物品有可删除属性
    if (cre->m_pTrade || cre->m_pPShop)
        return CMD_ERROR_NONE;

    char container;
    char page, i, j;
    long id;
    SAFE_READ(cmd->ReadLong(&id))
    SAFE_READ(cmd->ReadByte(&container))
    SAFE_READ(cmd->ReadByte(&page))
    SAFE_READ(cmd->ReadByte(&i))
    SAFE_READ(cmd->ReadByte(&j))

    // 要把装备从container中拿掉
    SItemID item;
    if (container == ITEM_CONTAINER_PLAYER_COMMON_BAG) {
        item = pCon->m_Bag.GetItem(page, i, j);
        if (item.id != (DWORD)id) {
            // 发刷新客户端消息
            // @@@@
            return CMD_ERROR_NONE;
        }
        SAFE_ASSERT(ItemID_IsValid(item))
        //		SAFE_ASSERT( pCon->CanItemTrade(true, page, i, j)==TR_Can)
        SAFE_ASSERT(ItemCanDelete(item))
        SAFE_ASSERT(item.cLock == 0);
        if (CmdGiver->m_pet && ItemIsPet(item) && CmdGiver->m_pet->m_dwPetItemOID == item.id) {
            return CMD_ERROR_NONE;
        }
        SAFE_ASSERT(pCon->m_Bag.RemoveItem(page, i, j))
    } else if (container == ITEM_CONTAINER_PLAYER_MISSION_BAG) {
        item = pCon->m_MissionBag.GetItem(page, i, j);
        if (item.id != (DWORD)id) {
            // 发刷新客户端消息
            // @@@@
            return CMD_ERROR_NONE;
        }
        SAFE_ASSERT(ItemID_IsValid(item))
        //		SAFE_ASSERT( pCon->CanItemTrade(false, page, i, j)==TR_Can)
        SAFE_ASSERT(ItemCanDelete(item))
        SAFE_ASSERT(pCon->m_MissionBag.RemoveItem(page, i, j))
    } else if (container == ITEM_CONTAINER_PLAYER_PET_BAG) {
        item = pCon->m_PetBag.GetItem(page, i, j);
        if (item.id != (DWORD)id) {
            // 发刷新客户端消息
            // @@@@
            return CMD_ERROR_NONE;
        }
        SAFE_ASSERT(ItemID_IsValid(item))
        //		SAFE_ASSERT( pCon->CanItemTrade(false, page, i, j)==TR_Can)
        SAFE_ASSERT(ItemCanDelete(item))
        SAFE_ASSERT(pCon->m_PetBag.RemoveItem(page, i, j))
    }

    if (ItemIsRare(item, g_region->m_pItemManager)) {
        // log到数据库
        string strItem = rtFormatNumber(GetItemSerializeVersion());
        strItem += ",";
        strItem += item.SaveToString(g_region->m_pItemManager);

        CG_CmdPacket& packet = g_region->m_gws.BeginSend();
        packet.WriteShort(r2g_log_event);
        packet.WriteByte(PLT_DeleteItem);
        packet.WriteLong(cre->m_userInfo->m_userId);
        packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
        packet.WriteString((char*)strItem.c_str());                           // strParam1
        g_region->m_gws.EndSend();
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_moveitem) {
    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;
    SAFE_ASSERT(pCon)

    CBag*   pBag = NULL;
    SItemID itemFrom, itemTo;
    char    container;
    char    pageFrom, iFrom, jFrom, pageTo, iTo, jTo;
    SAFE_READ(cmd->ReadByte(&container))
    SAFE_READ(cmd->ReadByte(&pageFrom))
    SAFE_READ(cmd->ReadByte(&iFrom))
    SAFE_READ(cmd->ReadByte(&jFrom))
    SAFE_READ(cmd->ReadByte(&pageTo))
    SAFE_READ(cmd->ReadByte(&iTo))
    SAFE_READ(cmd->ReadByte(&jTo))

    // 道具背包
    if (container == ITEM_CONTAINER_PLAYER_COMMON_BAG) {
        pBag = &(pCon->m_Bag);

        itemFrom = pBag->GetItem(pageFrom, iFrom, jFrom);
        itemTo   = pBag->GetItem(pageTo, iTo, jTo);
        SAFE_ASSERT(ItemID_IsValid(itemFrom));
        SAFE_ASSERT(pBag->MoveItem(pageFrom, iFrom, jFrom, pageTo, iTo, jTo));
    }
    // 任务背包
    else if (container == ITEM_CONTAINER_PLAYER_MISSION_BAG) {
        pBag = &(pCon->m_MissionBag);

        itemFrom = pBag->GetItem(pageFrom, iFrom, jFrom);
        itemTo   = pBag->GetItem(pageTo, iTo, jTo);
        SAFE_ASSERT(ItemID_IsValid(itemFrom));
        SAFE_ASSERT(pBag->MoveItem(pageFrom, iFrom, jFrom, pageTo, iTo, jTo));
    } else {
        return CMD_ERROR_NONE;
    }

    CItemCmdBuilder_Svr::Build_r2c_player_move_item((EItemContainer)container, pageFrom, iFrom,
                                                    jFrom, pageTo, iTo, jTo);
    CmdGiver->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

//Tianh 整理背包
ITEM_CMD_FUNCTION(cmd_c2r_player_updateBag) {
    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;

    SAFE_ASSERT(pCon);

    pCon->m_Bag.TidyUpBag();

    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &(pCon->m_Bag));
    CmdGiver->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

//Tianh 10.04.13 修理装备
ITEM_CMD_FUNCTION(cmd_c2r_player_repair) {
    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;
    long                oneOrall;
    long                ItemID;

    SAFE_ASSERT(pCon)

    SAFE_READ(cmd->ReadLong(&oneOrall));
    SAFE_READ(cmd->ReadLong(&ItemID));

    CBag*         pBag         = NULL;
    SItemBase*    pItemBase    = NULL;
    CItemManager* pItemManager = NULL;

    if (oneOrall) {
        long endure     = 0;
        long Maxendure  = 0;
        long Needendure = 0;

        for (DWORD i = CItemContainerBase::WEAPON_1; i < CItemContainerBase::MAX_EQUIP_PARTS; i++) {
            SItemID ret = pCon->GetEquipItem((CItemContainerBase::EEquipParts)i, true);

            if (ret.id == 0) {
                continue;
            }

            endure = ret.curWear / 100;

            pItemManager = pCon->GetItemManager();
            if (pItemManager) {
                pItemBase = pItemManager->GetItem(ret.type);
            }
            if (pItemBase) {
                Maxendure = pItemBase->maxWear / 100;
            }

            Needendure += Maxendure - endure;
        }

        //一点耐久需要金钱一GB
        if (Needendure > pCon->GetMoney()) {
            return CMD_ERROR_NONE;
        } else {
            for (DWORD i = CItemContainerBase::WEAPON_1; i < CItemContainerBase::MAX_EQUIP_PARTS;
                 i++) {
                SItemID ret  = pCon->GetEquipItem((CItemContainerBase::EEquipParts)i, true);
                pItemManager = pCon->GetItemManager();
                if (pItemManager) {
                    pItemBase = pItemManager->GetItem(ret.type);
                }
                if (pItemBase) {
                    ret.curWear = pItemBase->maxWear;
                }
                if (ItemID_IsValid(ret)) {
                    pCon->SetEquipItem((CItemContainerBase::EEquipParts)i, ret);
                    cre->m_pItemContainer->UpdateItemToClient(((CItemContainerBase::EEquipParts)i));
                }
            }

            cre->m_pItemContainer->RemoveMoney(Needendure);
            CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(
                -Needendure, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_PICK);
            cre->RecvCmd(g_sendCmd);

            rt2_sprintf(g_strStaticBuffer, R(MSG_ITEM_ALLREPAIRED), Needendure);
            cre->SendSystemMessage(g_strStaticBuffer);
        }
    } else {
        long    endure     = 0;
        long    Maxendure  = 0;
        long    Needendure = 0;
        SItemID ret;
        bool    Type = false;
        int     c    = 0;

        for (DWORD i = CItemContainerBase::WEAPON_1; i < CItemContainerBase::MAX_EQUIP_PARTS; i++) {
            if (pCon->GetEquipItem((CItemContainerBase::EEquipParts)i, true).id == ItemID) {
                ret          = pCon->GetEquipItem((CItemContainerBase::EEquipParts)i, true);
                pItemManager = pCon->GetItemManager();
                if (pItemManager) {
                    pItemBase = pItemManager->GetItem(ret.type);
                }
                Type = true;
                c    = i;
            }
        }

        if (!pItemBase)  //如果在身上找不到就去背包找
        {
            pBag = &(pCon->m_Bag);

            ret = pBag->GetItemFromID(ItemID);

            pItemManager = pCon->GetItemManager();
            if (pItemManager) {
                pItemBase = pItemManager->GetItem(ret.type);
            }
        }

        endure = ret.curWear / 100;

        if (!pItemBase)  //还找不到就返回
        {
            return CMD_ERROR_NONE;
        }
        Maxendure = pItemBase->maxWear / 100;

        Needendure += Maxendure - endure;

        if (Needendure > pCon->GetMoney()) {
            return CMD_ERROR_NONE;
        } else {
            ret.curWear = pItemBase->maxWear;
            if (Type) {
                if (ItemID_IsValid(ret)) {
                    pCon->SetEquipItem((CItemContainerBase::EEquipParts)c, ret);
                    cre->m_pItemContainer->UpdateItemToClient(((CItemContainerBase::EEquipParts)c));
                }
            } else {
                SItemID Str = ret;
                cre->m_pItemContainer->RemoveItem(ret);  //删除这件装备
                cre->m_pItemContainer->AddItem(Str);

                cre->m_pItemContainer->UpdateItemToClient(ITEM_CONTAINER_PLAYER_COMMON_BAG, ret.id);
            }
            cre->m_pItemContainer->RemoveMoney(Needendure);
            CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(
                -Needendure, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_PICK);
            cre->RecvCmd(g_sendCmd);

            rt2_sprintf(g_strStaticBuffer, R(MSG_ITEM_REPAIRED), Needendure);
            cre->SendSystemMessage(g_strStaticBuffer);
        }
    }
}

//Tianh 10.03.13 //物品分离
ITEM_CMD_FUNCTION(cmd_c2r_player_dispart) {
    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;

    SAFE_ASSERT(pCon)

    if (cre->m_pPShop) {
        return CMD_ERROR_NONE;
    }

    CBag*   pBag = NULL;
    SItemID itemFrom, itemTo;
    char    container;
    char    pageFrom, iFrom, jFrom, pageTo, iTo, jTo;
    long    num;  //分离的数量
    SAFE_READ(cmd->ReadByte(&container))
    SAFE_READ(cmd->ReadByte(&pageFrom))
    SAFE_READ(cmd->ReadByte(&iFrom))
    SAFE_READ(cmd->ReadByte(&jFrom))
    SAFE_READ(cmd->ReadByte(&pageTo))
    SAFE_READ(cmd->ReadByte(&iTo))
    SAFE_READ(cmd->ReadByte(&jTo))
    SAFE_READ(cmd->ReadLong(&num))

    // 道具背包
    if (container == ITEM_CONTAINER_PLAYER_COMMON_BAG) {
        pBag     = &(pCon->m_Bag);
        itemFrom = pBag->GetItem(pageFrom, iFrom, jFrom);
        itemTo   = pBag->GetItem(pageTo, iTo, jTo);
        SAFE_ASSERT(ItemID_IsValid(itemFrom));
        SAFE_ASSERT(pBag->ResolveItem(pageFrom, iFrom, jFrom, pageTo, iTo, jTo, num));
    }
    // 任务背包
    else if (container == ITEM_CONTAINER_PLAYER_MISSION_BAG) {
        pBag = &(pCon->m_MissionBag);

        itemFrom = pBag->GetItem(pageFrom, iFrom, jFrom);
        itemTo   = pBag->GetItem(pageTo, iTo, jTo);
        SAFE_ASSERT(ItemID_IsValid(itemFrom));
        SAFE_ASSERT(pBag->ResolveItem(pageFrom, iFrom, jFrom, pageTo, iTo, jTo, num));
    } else {
        return CMD_ERROR_NONE;
    }
    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, pBag);
    CmdGiver->RecvCmd(g_sendCmd);
    //CItemCmdBuilder_Svr::Build_r2c_player_dispart_item((EItemContainer)container, pageFrom, iFrom, jFrom, pageTo, iTo, jTo,num);
    //CmdGiver->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_equipitem) {
    // CHECK
    // 交易，系统买卖，摆摊时不准装备卸装
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    CItemContainerBase::EEquipParts part;
    char                            cTmp;
    char                            container;
    char                            page, i, j;
    long                            id;
    SAFE_READ(cmd->ReadLong(&id))
    SAFE_READ(cmd->ReadByte((char*)&cTmp))
    SAFE_READ(cmd->ReadByte(&container))
    SAFE_READ(cmd->ReadByte(&page))
    SAFE_READ(cmd->ReadByte(&i))
    SAFE_READ(cmd->ReadByte(&j))
    part = (CItemContainerBase::EEquipParts)cTmp;

    SItemID item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page, i, j);
    SAFE_ASSERT(item.id == id)

    if (CmdGiver->UseItem(page, i, j, part, true)) {
        //---------add start by tony 06.05.16---------------//
        //-换装备1秒内不能攻击
        CmdGiver->m_lastEquipItemTime = time(NULL);
        //---------add end   by tony 06.05.16---------------//
        return CMD_ERROR_NONE;
    } else {
        return CMD_ERROR_NONE;
    }
}

ITEM_CMD_FUNCTION(cmd_c2r_player_switchweapon) {
    //CRegionCreature *cre = CmdGiver;
    // CHECK
    // 交易，系统买卖，摆摊时不准装备卸装
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    SItemID item1 = CmdGiver->m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_1, true);
    SItemID item3 = CmdGiver->m_pItemContainer->GetEquipItem(CItemContainerBase::WEAPON_3, true);
    CmdGiver->m_pItemContainer->UnEquip(CItemContainerBase::WEAPON_1);
    CmdGiver->m_pItemContainer->UnEquip(CItemContainerBase::WEAPON_3);
    CmdGiver->m_pItemContainer->Equip(item3, CItemContainerBase::WEAPON_1);
    CmdGiver->m_pItemContainer->Equip(item1, CItemContainerBase::WEAPON_3);

    CmdGiver->m_pItemContainer->UpdateItemToClient(CItemContainerBase::WEAPON_1);
    CmdGiver->m_pItemContainer->UpdateItemToClient(CItemContainerBase::WEAPON_3);

    /*CmdGiver->m_Skill.SpecialClearUseful();*/  //add by Tianh 换武器清除所有有益BUF 除了双倍等RMB道具

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_unequipitem) {
    // CHECK
    // 交易，系统买卖，摆摊时不准装备卸装
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    CItemContainerBase::EEquipParts part;
    char                            cTmp;
    long                            id;
    SAFE_READ(cmd->ReadLong(&id))
    SAFE_READ(cmd->ReadByte((char*)&cTmp))
    part = (CItemContainerBase::EEquipParts)cTmp;

    SItemID item = CmdGiver->m_pItemContainer->GetEquipItem(part, true);
    SAFE_ASSERT(item.id == id)

    if (CmdGiver->UseItem(-1, -1, -1, part, false))
        return CMD_ERROR_NONE;
    else
        return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_use_item) {
    char container;
    char page, i, j;
    long id;
    SAFE_READ(cmd->ReadLong(&id))
    SAFE_READ(cmd->ReadByte(&container))
    SAFE_READ(cmd->ReadByte(&page))
    SAFE_READ(cmd->ReadByte(&i))
    SAFE_READ(cmd->ReadByte(&j))

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    SItemID item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page, i, j);
    SAFE_ASSERT(item.id == id)

    if (CmdGiver->UseItem(page, i, j))
        return CMD_ERROR_NONE;
    else
        return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_itemupdate) {
    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;
    SAFE_ASSERT(pCon)
    DWORD NpcID = 0;

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    char pageItem, iItem, jItem;
    char pageScroll, iScroll, jScroll;
    char pageLuck, iLuck, jLuck;
    SAFE_READ(cmd->ReadByte(&pageItem))
    SAFE_READ(cmd->ReadByte(&iItem))
    SAFE_READ(cmd->ReadByte(&jItem))
    SAFE_READ(cmd->ReadByte(&pageScroll))
    SAFE_READ(cmd->ReadByte(&iScroll))
    SAFE_READ(cmd->ReadByte(&jScroll))
    SAFE_READ(cmd->ReadByte(&pageLuck))
    SAFE_READ(cmd->ReadByte(&iLuck))
    SAFE_READ(cmd->ReadByte(&jLuck))

    SItemID origItemUpdate, origItemScroll, origItemLuck;
    SItemID newItemUpdate;
    origItemUpdate = pCon->m_Bag.GetItem(pageItem, iItem, jItem);
    origItemScroll = pCon->m_Bag.GetItem(pageScroll, iScroll, jScroll);
    if (pageLuck >= 0 && iLuck >= 0 && jLuck >= 0) {
        origItemLuck = pCon->m_Bag.GetItem(pageLuck, iLuck, jLuck);
        SAFE_ASSERT(ItemIsGemLuck(origItemLuck) || ItemIsGemAntiBroken(origItemLuck.type))
    }

    // CHECKUP
    // 交易状态不能打造
    // 摆摊状态不能打造
    // 打造道具和符咒要匹配（盾牌使用防御符咒）
    // 法宝不能打造

    if (ItemIsTrump(origItemUpdate.type))
        return CMD_ERROR_NONE;

    bool bWeapon = true;
    if (ItemIsWeapon(origItemUpdate) && !ItemIsShield(origItemUpdate)) {
        if (origItemUpdate.level >= 0 && origItemUpdate.level <= 2)
            SAFE_ASSERT(ItemIsGemWeapon1(origItemScroll))
        else if (origItemUpdate.level >= 3 && origItemUpdate.level <= 5)
            SAFE_ASSERT(ItemIsGemWeapon2(origItemScroll))
        else if (origItemUpdate.level >= 6 && origItemUpdate.level <= 8)
            SAFE_ASSERT(ItemIsGemWeapon3(origItemScroll))
        else if (origItemUpdate.level == 10)
            SAFE_ASSERT(false)
        else
            return CMD_ERROR_UNKNOWN;
    } else if (ItemIsArmor(origItemUpdate) || ItemIsShield(origItemUpdate)) {
        bWeapon = false;

        if (origItemUpdate.level >= 0 && origItemUpdate.level <= 2)
            SAFE_ASSERT(ItemIsGemArmor1(origItemScroll))
        else if (origItemUpdate.level >= 3 && origItemUpdate.level <= 5)
            SAFE_ASSERT(ItemIsGemArmor2(origItemScroll))
        else if (origItemUpdate.level >= 6 && origItemUpdate.level <= 8)
            SAFE_ASSERT(ItemIsGemArmor3(origItemScroll))
        else if (origItemUpdate.level == 10)
            SAFE_ASSERT(false)
        else
            return CMD_ERROR_UNKNOWN;
    } else
        return CMD_ERROR_UNKNOWN;

    // 判断第二宝石的合法性
    if (ItemID_IsValid(origItemLuck)) {
        if (ItemIsGemLuck6Only(origItemLuck.type)) {
            SAFE_ASSERT(origItemUpdate.level == 5);
        } else if (ItemIsGemLuck7Only(origItemLuck.type)) {
            //SAFE_ASSERT(origItemUpdate.level == 6);
            SAFE_ASSERT(origItemUpdate.level >= 6 && origItemUpdate.level <= 8);
        } else {
            if (origItemUpdate.level >= 0 && origItemUpdate.level <= 2)
                return CMD_ERROR_NONE;
            else if (origItemUpdate.level >= 3 && origItemUpdate.level <= 5)
                SAFE_ASSERT(ItemIsGemLuck2(origItemLuck.type) ||
                            ItemIsGemAntiBroken(origItemLuck.type))
            else if (origItemUpdate.level >= 6 && origItemUpdate.level <= 8)
                SAFE_ASSERT(ItemIsGemLuck3(origItemLuck.type) ||
                            ItemIsGemAntiBroken(origItemLuck.type))
            else if (origItemUpdate.level == 10)
                SAFE_ASSERT(false)
            else
                return CMD_ERROR_UNKNOWN;
        }
    }

    newItemUpdate     = origItemUpdate;
    int moneyRequired = 0;
    if (newItemUpdate.level < ITEM_MAX_ITEM_LEVEL) {
        // deal money
        ItemUpdate_GetMoneyRequirement(origItemUpdate, moneyRequired, g_region->m_pItemManager);
        if (pCon->GetMoney() < moneyRequired)
            return CMD_ERROR_NONE;

        pCon->RemoveMoney(moneyRequired);
        /*
		if (newItemUpdate.level<ITEM_ITEM_LEVEL_STABLE)
		{
			newItemUpdate.level++;
			pCon->m_Bag.SetItem((int)pageItem, (int)iItem, (int)jItem, newItemUpdate);
			pCon->m_Bag.SetItem((int)pageScroll, (int)iScroll, (int)jScroll, ItemID_CreateInvalid());
			if (pageLuck>=0 && iLuck>=0 && jLuck>=0)
				pCon->m_Bag.SetItem((int)pageLuck, (int)iLuck, (int)jLuck, ItemID_CreateInvalid());
			CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_SUCCEED1));
			CItemCmdBuilder_Svr::Build_r2c_player_itemupdate_result(origItemUpdate, origItemScroll, origItemLuck, newItemUpdate);
			cre->RecvCmd(g_sendCmd);
		}
		else 
		{
			float fSuccessRate = g_itemupdate_successRate[newItemUpdate.level];
			float fBrokenRate = g_itemupdate_brokenRate[newItemUpdate.level];
			if (ItemID_IsValid(origItemLuck))
			{
				SGem *pLuckCharm = (SGem*)g_region->m_pItemManager->GetItem(origItemLuck.type);
				if (!pLuckCharm)
					return CMD_ERROR_UNKNOWN;
				if ( ItemIsGemLuck2(origItemLuck.type) && (origItemUpdate.level>=3 && origItemUpdate.level<=5) )
					fSuccessRate *= pLuckCharm->fRate;
				else if ( ItemIsGemLuck3(origItemLuck.type) && (origItemUpdate.level>=6 && origItemUpdate.level<=8) )
					fSuccessRate *= pLuckCharm->fRate;
				else if ( ItemIsGemAntiBroken(origItemLuck.type))
					fBrokenRate = 0.00000000;
                else if ( ItemIsGemLuck6Only(origItemLuck.type))
                {
                    fSuccessRate = 0.5;
                    fBrokenRate = 0.00000000;
                }
                else if ( ItemIsGemLuck7Only(origItemLuck.type))
                {
                    fSuccessRate = 0.7;//0.5;
                    fBrokenRate = 0.00000000;
                }
			}

			//绑定装备概率再加5%
			if(origItemUpdate.cBind == IB_Binded)
			{
				fSuccessRate = fSuccessRate + 0.01;
			}

			if (fSuccessRate>1.0f)
				fSuccessRate = 1.0000001f;
			else if (fSuccessRate<0.0f)
				fSuccessRate = 0.0f;
			if (fBrokenRate>1.0f)
				fBrokenRate = 1.0000001f;
			else if (fBrokenRate<0.0f)
				fBrokenRate = 0.0f;

			if (GetProb(fSuccessRate))
			{
				newItemUpdate.level++;
				pCon->m_Bag.SetItem((int)pageItem, (int)iItem, (int)jItem, newItemUpdate);
				pCon->m_Bag.SetItem((int)pageScroll, (int)iScroll, (int)jScroll, ItemID_CreateInvalid());
				if (pageLuck>=0 && iLuck>=0 && jLuck>=0)
					pCon->m_Bag.SetItem((int)pageLuck, (int)iLuck, (int)jLuck, ItemID_CreateInvalid());
				CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_SUCCEED1));
				CItemCmdBuilder_Svr::Build_r2c_player_itemupdate_result(origItemUpdate, origItemScroll, origItemLuck, newItemUpdate);
				cre->RecvCmd(g_sendCmd);
				if(newItemUpdate.level >= 7)
				{
					char cTmp512[512];
					rt2_sprintf(cTmp512, "玩家“%s”锻造+%d装备成功！",CmdGiver->m_core.Name.c_str(),newItemUpdate.level);
					g_region->m_gws.BroadcastBulletin(cTmp512);
				}

			}
			else
			{
				//if(newItemUpdate.level < 6)
			//	{
			//		newItemUpdate.level = 0;
			//	}
				if (CmdGiver->GetItemNum(REFINEGOAT_ITEM) > 0)
				{
					CmdGiver->RemoveItem(REFINEGOAT_ITEM, 1);
					newItemUpdate.level--;
					if(newItemUpdate.level < 0)
						newItemUpdate.level = 0;
					CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_FAIL1));
				}
				else if (CmdGiver->GetItemNum(REFINEGOAT_ITEM_SET) > 0)
				{
					CmdGiver->RemoveItem(REFINEGOAT_ITEM_SET,1);
					newItemUpdate.level--;
					if(newItemUpdate.level < 0)
						newItemUpdate.level = 0;
					CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_FAIL1));
				}
				else if (CmdGiver->GetItemNum(REFINEGOAT_ITEM_SET_BIND) > 0)
				{
					CmdGiver->RemoveItem(REFINEGOAT_ITEM_SET_BIND,1);
					newItemUpdate.level--;
					if(newItemUpdate.level < 0)
						newItemUpdate.level = 0;
					CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_FAIL1));
				}
				else
				{
					newItemUpdate.level = 0;
				}
				
				pCon->m_Bag.SetItem((int)pageItem, (int)iItem, (int)jItem, newItemUpdate);
				pCon->m_Bag.SetItem((int)pageScroll, (int)iScroll, (int)jScroll, ItemID_CreateInvalid());
				if (pageLuck>=0 && iLuck>=0 && jLuck>=0)
					pCon->m_Bag.SetItem((int)pageLuck, (int)iLuck, (int)jLuck, ItemID_CreateInvalid());
				CItemCmdBuilder_Svr::Build_r2c_player_itemupdate_result(origItemUpdate, origItemScroll, origItemLuck, newItemUpdate);
				cre->RecvCmd(g_sendCmd);
				
			}
		}
		*/

        static int nR[ITEM_MAX_ITEM_LEVEL][4] = {
            {100, 0, 0, 0},   {80, 0, 20, 0},  {70, 10, 20, 0},  {60, 16, 20, 4},
            {50, 24, 20, 6},  {40, 32, 20, 8}, {30, 40, 20, 10}, {20, 48, 20, 12},
            {20, 48, 20, 12}, {20, 48, 20, 12}};
        int i;
        //for(i = 0; i < ITEM_MAX_ITEM_LEVEL; i++)
        //srand(rtGetMilliseconds());   ///< 随机种子
        int nNum = rand() % 100;

        int nCurLev = newItemUpdate.level;
        if (nCurLev < 0) {
            nCurLev = 0;
        } else if (nCurLev > 9) {
            nCurLev = 9;
        }

        if (nNum < nR[nCurLev][0]) {
            //成功
            newItemUpdate.level++;
            CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_SUCCEED1));

            if (newItemUpdate.level >= 5) {
                //string str = "乌云滚滚，天雷阵阵，突然一道闪电落下，砸中正在镶嵌[";
                //str += cre->m_core.Name;
                //str += "]突然右手甩锤，蹬脚上桌，仰天大笑：“通过我成吉思汗的努力，终于打造出了威力绝伦的[";

                SItemBase*    pItemBase;
                CItemManager* pItemManager;
                pItemManager = pCon->GetItemManager();

                if (pItemManager) {
                    pItemBase = pItemManager->GetItem(newItemUpdate.type);
                }
                string str1 = "";
                if (pItemBase) {
                    str1 += pItemBase->name;
                } else {
                    str1 += "无法得到物品名字";
                }
                char   Cstr[10240];
                long   lcolor = newItemUpdate.color;
                string AllStr;
                string StrItem = newItemUpdate.SaveToString(pItemManager);
                AllStr += "item[";
                rt2_sprintf(Cstr, "%d^)!%s^)!%s", lcolor, str1.c_str(), StrItem.c_str());
                AllStr += Cstr;
                AllStr += "]meti";
                //str += "]";
                //const char* msg = str.c_str();
                /*SItemID ItemID;*/
                //cre->Systembroadcast(msg,STRENGTHENEQUIP,newItemUpdate, cre->m_core.Name.c_str());

                string      str        = cre->m_core.Name;
                const char* playername = str.c_str();
                const char* Name       = "";
                const char* BossName   = "";
                const char* SceneName  = "";
                const char* ItemName   = str1.c_str();
                cre->Systembroadcast(playername, Name, BossName, SceneName, STRENGTHENEQUIP,
                                     AllStr.c_str());

                /*乌云滚滚，天雷阵阵，突然一道闪电落下，砸中正在镶嵌[宝石]的[玩家名]，
				使得他头发曲卷，脸色焦黑，衣裳褴褛。虽然有些破相，机缘之下却炼成了[强化装备]。*/
            }
        } else {
            nNum -= nR[nCurLev][0];
            if (nNum < nR[nCurLev][1]) {
                //失败
                if (newItemUpdate.level > 0) {
                    if (newItemUpdate.level > 5) {
                        newItemUpdate.level = 0;
                    } else {
                        newItemUpdate.level--;
                    }
                    CmdGiver->SendSystemMessage(R(MSG_ITEM_ZHUANGBEIGUILING));
                }
            } else {
                nNum -= nR[nCurLev][1];
                if (nNum < nR[nCurLev][2]) {
                    //不变
                    CmdGiver->SendSystemMessage(R(MSG_ITEM_QIANGHUA_DOWN));
                } else {
                    //销毁
                    newItemUpdate = ItemID_CreateInvalid();
                    CmdGiver->SendSystemMessage(R(MSG_ITEM_QIANGHUA_DESTORY));
                }
            }
        }
        pCon->m_Bag.SetItem((int)pageItem, (int)iItem, (int)jItem, newItemUpdate);
        pCon->m_Bag.SetItem((int)pageScroll, (int)iScroll, (int)jScroll, ItemID_CreateInvalid());
        if (pageLuck >= 0 && iLuck >= 0 && jLuck >= 0)
            pCon->m_Bag.SetItem((int)pageLuck, (int)iLuck, (int)jLuck, ItemID_CreateInvalid());
        //CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_SUCCEED1));
        CItemCmdBuilder_Svr::Build_r2c_player_itemupdate_result(origItemUpdate, origItemScroll,
                                                                origItemLuck, newItemUpdate);
        cre->RecvCmd(g_sendCmd);

    } else {
        return CMD_ERROR_UNKNOWN;
    }

    // log到数据库
    std::string strOrig = rtFormatNumber(GetItemSerializeVersion());
    strOrig += ",";
    strOrig += origItemUpdate.SaveToString(g_region->m_pItemManager);
    std::string strNew = rtFormatNumber(GetItemSerializeVersion());
    strNew += ",";
    strNew += newItemUpdate.SaveToString(g_region->m_pItemManager);
    std::string strLuck;
    if (ItemID_IsValid(origItemLuck)) {
        strLuck += rtFormatNumber(GetItemSerializeVersion());
        strLuck += ",";
        strLuck += origItemLuck.SaveToString(g_region->m_pItemManager);
    }

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_ItemUpdate);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(NpcID);
    //packet.WriteLong(moneyRequired);
    packet.WriteString((char*)strOrig.c_str());
    packet.WriteString((char*)strNew.c_str());
    packet.WriteString((char*)strLuck.c_str());
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_itemlock) {
    char    page, gridI, gridJ;
    long    itemId;
    SItemID item;
    SAFE_READ(cmd->ReadLong(&itemId));
    SAFE_READ(cmd->ReadByte(&page));
    SAFE_READ(cmd->ReadByte(&gridI));
    SAFE_READ(cmd->ReadByte(&gridJ));

    item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page, gridI, gridJ);
    if (!ItemID_IsValid(item) || item.id != itemId || item.cLock != 0) {
        CmdGiver->SendSystemMessage(R(MSG_ITEM_LOCK_FAIL));
        return CMD_ERROR_NONE;
    }

    item.cLock = 1;
    CmdGiver->m_pItemContainer->m_Bag.SetItem(page, gridI, gridJ, item);

    CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                       gridI, gridJ, item);
    CmdGiver->RecvCmd(g_sendCmd);
    CmdGiver->SendSystemMessage(R(MSG_ITEM_LOCK_SUCCESS));

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_itemunlock) {
    char    page, gridI, gridJ;
    long    itemId;
    char*   strBankPass;
    SItemID item;
    SAFE_READ(cmd->ReadLong(&itemId));
    SAFE_READ(cmd->ReadByte(&page));
    SAFE_READ(cmd->ReadByte(&gridI));
    SAFE_READ(cmd->ReadByte(&gridJ));
    SAFE_READ(cmd->ReadString(&strBankPass));

    item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page, gridI, gridJ);
    if (!ItemID_IsValid(item) || item.id != itemId || item.cLock == 0) {
        CmdGiver->SendSystemMessage(R(MSG_ITEM_UNLOCK_FAIL));
        return CMD_ERROR_NONE;
    }

    if (!CmdGiver->m_userInfo->CheckBankPass(strBankPass)) {
        CmdGiver->SendSystemMessage(R(MSG_ITEM_UNLOCK_FAIL));
        return CMD_ERROR_NONE;
    }

    item.cLock = 0;
    CmdGiver->m_pItemContainer->m_Bag.SetItem(page, gridI, gridJ, item);

    CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                       gridI, gridJ, item);
    CmdGiver->RecvCmd(g_sendCmd);
    CmdGiver->SendSystemMessage(R(MSG_ITEM_UNLOCK_SUCCESS));

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_player_peteat) {
    long petFrom;
    long petTo;
    SAFE_READ(cmd->ReadLong(&petFrom));
    SAFE_READ(cmd->ReadLong(&petTo));

    CItemContainerBase* pCon = CmdGiver->m_pItemContainer;
    SAFE_ASSERT(pCon);

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;
    if (CmdGiver->m_pet) {
        CmdGiver->SendSystemMessage("请先收回所有的宠物");
        return CMD_ERROR_NONE;
    }

    SItemID PetFrom, PetTo;
    SItemID NewPetItem;

    int page_From, gridI_From, gridJ_From;
    int page_To, gridI_To, gridJ_To;
    SAFE_ASSERT(pCon->m_Bag.Find(petFrom, &page_From, &gridI_From, &gridJ_From));
    PetFrom = pCon->m_Bag.GetItem(page_From, gridI_From, gridJ_From);
    SAFE_ASSERT(pCon->m_Bag.Find(petTo, &page_To, &gridI_To, &gridJ_To));
    PetTo = pCon->m_Bag.GetItem(page_To, gridI_To, gridJ_To);

    SAFE_ASSERT(ItemID_IsValid(PetFrom) && ItemID_IsValid(PetTo));
    int ItemIndex_From = g_region->m_pItemManager->GetIndexFromType(PetFrom.type);
    int ItemIndex_To   = g_region->m_pItemManager->GetIndexFromType(PetTo.type);
    SAFE_ASSERT(ItemIndex_From == 476 || ItemIndex_From == 512 || ItemIndex_From == 513);
    SAFE_ASSERT(ItemIndex_To == 1108);

    NewPetItem                       = PetTo;
    NewPetItem.paramCount            = PetTo.paramCount;
    NewPetItem.params[PET_PARAM_LEV] = PetFrom.params[PET_PARAM_LEV];
    NewPetItem.params[PET_PARAM_EXP] = PetFrom.params[PET_PARAM_EXP];
    NewPetItem.params[PET_PARAM_HP]  = 1;
    NewPetItem.params[PET_PARAM_MP]  = 1;
    NewPetItem.params[PET_PARAM_AI]  = PetFrom.params[PET_PARAM_AI];

    pCon->m_Bag.SetItem(page_To, gridI_To, gridJ_To, NewPetItem);
    pCon->m_Bag.SetItem(page_From, gridI_From, gridJ_From, ItemID_CreateInvalid());
    CItemCmdBuilder_Svr::Build_r2c_peteat_result(petFrom, petTo, NewPetItem);
    CmdGiver->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_trade_invite) {
    CRegionCreature* cre = CmdGiver;

    long actorID;
    SAFE_READ(cmd->ReadLong(&actorID))
    CRegionCreature* pPlayerInvited = cre->m_scene->FindCreature(actorID);
    if (!pPlayerInvited || !pPlayerInvited->m_pItemContainer)
        return CMD_ERROR_NONE;

    if (pPlayerInvited->m_bRefusetrade)  //对方设置了
    {
        rt2_sprintf(g_strStaticBuffer, R(MSG_TRADE_OTHERREFUSETRADE));
        cre->SendSystemMessage(g_strStaticBuffer);
    }

    if (pPlayerInvited->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    /*交易距离*/
    if (cre->Distance(pPlayerInvited->m_pos) > 200) {
        CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_time_far);
        cre->RecvCmd(g_sendCmd);
        //CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_time_far);
        //pPlayerInvited->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    /*增加上线交易保护，为1分钟*Tianh*/
    if (cre->m_bTime || pPlayerInvited->m_bTime) {
        CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_time_non_arrival);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    //增加临时好友
    if (cre->m_pFriend) {
        cre->m_pFriend->WhenFriend(pPlayerInvited->m_userInfo->m_userId);
    }
    if (pPlayerInvited->m_pFriend) {
        pPlayerInvited->m_pFriend->WhenFriend(cre->m_userInfo->m_userId);
    }

    // CHECK
    // 邀请人不在交易状态和摆摊状态
    // 被邀请人不在交易状态
    if (cre->m_pTrade || cre->m_pPShop || cre->m_lIsCombat) {
        return CMD_ERROR_NONE;
    } else if (pPlayerInvited->m_pTrade) {
        CItemCmdBuilder_Svr::Build_r2c_trade_other_invite_response(pPlayerInvited->m_oId, 2);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    } else if (pPlayerInvited->m_pPShop) {
        return CMD_ERROR_NONE;
    } else if (pPlayerInvited->m_lIsCombat) {
        return CMD_ERROR_NONE;
    }

    CItemCmdBuilder_Svr::Build_r2c_trade_other_invite(cre->m_oId);
    pPlayerInvited->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_trade_invite_response) {
    // CHECK
    // 双方都不在交易状态
    // 双方都不在摆摊状态
    CRegionCreature* cre = CmdGiver;

    long actorID;
    char bAgree;
    SAFE_READ(cmd->ReadLong(&actorID))
    SAFE_READ(cmd->ReadByte(&bAgree))
    CRegionCreature* pPlayerTo = cre->m_scene->FindCreature(actorID);
    if (!pPlayerTo || !pPlayerTo->m_pItemContainer)
        return CMD_ERROR_NONE;
    if (pPlayerTo->m_pTrade || cre->m_pTrade)
        return CMD_ERROR_NONE;
    if (pPlayerTo->m_pPShop || cre->m_pPShop)
        return CMD_ERROR_NONE;
    /**Tianh  09.12.26  玩家在PK状态*/
    if (pPlayerTo->m_lIsCombat || cre->m_lIsCombat)
        return CMD_ERROR_NONE;

    /*增加上线交易保护，为1分钟*Tianh*/
    if (cre->m_bTime || pPlayerTo->m_bTime) {
        CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_time_non_arrival);
        pPlayerTo->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if (pPlayerTo->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    if (bAgree) {
        CTrade* pTrade = RT_NEW CTrade(CmdGiver->m_pItemContainer, pPlayerTo->m_pItemContainer,
                                       CmdGiver->m_oId, pPlayerTo->m_oId, g_region->m_pItemManager);
        pTrade->GetPetbag1().Resize(ITEM_PET_BAG_PAGE, ITEM_PET_BAG_LINE,
                                    3 + CmdGiver->m_core.GradeLev / 20);
        pTrade->GetPetbag2().Resize(ITEM_PET_BAG_PAGE, ITEM_PET_BAG_LINE,
                                    3 + pPlayerTo->m_core.GradeLev / 20);
        CmdGiver->m_pTrade  = pTrade;
        pPlayerTo->m_pTrade = pTrade;

        CItemCmdBuilder_Svr::Build_r2c_trade_begin(pPlayerTo->m_oId);
        cre->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_trade_begin(cre->m_oId);
        pPlayerTo->RecvCmd(g_sendCmd);
    } else {
        CItemCmdBuilder_Svr::Build_r2c_trade_other_invite_response(cre->m_oId, 0);
        pPlayerTo->RecvCmd(g_sendCmd);
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_trade_lock) {
    // CHECK
    // 交易对象在线，并且交易对象的交易对象是自己
    // 交易双方不在摆摊状态
    // 交易物品个数不超过最大值
    // 交易物品在玩家道具栏里必须都存在

    CRegionCreature*      cre    = CmdGiver;
    CRegionItemContainer* pCon   = cre->m_pItemContainer;
    CTrade*               pTrade = cre->m_pTrade;

    BOOL_ENSURE(pTrade)
    DWORD            dwTradeWith = pTrade->GetOtherPlayer(cre->m_oId);
    CRegionCreature* pTradeWith  = cre->m_scene->FindCreature(dwTradeWith);
    if (!pTradeWith || !pTradeWith->m_pItemContainer || pTradeWith->m_lIsCombat) {
        cre->StopPlayerTrade();
        return CMD_ERROR_NONE;
    }

    if (pTradeWith->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    SAFE_ASSERT(pTradeWith->m_pTrade == cre->m_pTrade)
    SAFE_ASSERT(!pTradeWith->m_pPShop && !cre->m_pPShop)

    long        money;
    SDWORDArray idArray;
    SDWORDArray idArrayPet;
    BOOL_ENSURE(idArray.Serialize(cmd))
    BOOL_ENSURE(idArrayPet.Serialize(cmd))
    SAFE_READ(cmd->ReadLong(&money))
    SAFE_ASSERT(money >= 0 && money <= MAX_MONEY_COUNT);
    char* strBankPass;
    SAFE_READ(cmd->ReadString(&strBankPass));
    CBag bag(TRADE_GRID_LINE_COUNT, TRADE_GRID_COLUMN_COUNT, 1, g_region->m_pItemManager);
    CBag Petbag(ITEM_PET_BAG_LINE, ITEM_PET_BAG_COLUMN, 1, g_region->m_pItemManager);
    int  counterCommon  = 0;
    int  counterMission = 0;
    int  counterPet     = 0;

    // clone check
    if (CloneCheck(idArray.array)) {
        ERR1("[cmd_c2r_trade_lock] Find Clone Item(User=%s)\n", cre->m_core.Name.c_str());
        return CMD_ERROR_UNKNOWN;
    }
    if (CloneCheck(idArrayPet.array)) {
        ERR1("[cmd_c2r_trade_lock] Find Clone Pet(User=%s)\n", cre->m_core.Name.c_str());
        return CMD_ERROR_UNKNOWN;
    }
    SItemID item;
    bool    bNeedBankPass = false;
    for (int i = 0; i < (int)idArray.array.size(); i++) {
        if (cre->m_pet && cre->m_pet->m_ai) {
            if (((CRegionPetAI*)cre->m_pet->m_ai)->m_pItemPet.id == idArray.array[i]) {
                return CMD_ERROR_NONE;
            }
        }

        int page, gridI, gridJ;

        if (pCon->m_Bag.Find(idArray.array[i], &page, &gridI, &gridJ)) {
            item = pCon->m_Bag.GetItem(page, gridI, gridJ);
            if (pCon->CanItemTrade(1, page, gridI, gridJ) != TR_Can) {
                cre->StopPlayerTrade();
                return CMD_ERROR_NONE;
            }
            if (item.cBind == IB_Binded) {
                cre->StopPlayerTrade();
                return CMD_ERROR_NONE;
            }
            if (item.cLock != 0) {
                bNeedBankPass = true;
            }
            int tmpPage, tmpI, tmpJ;
            BOOL_ENSURE(bag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
            counterCommon++;
        } else {
            return CMD_ERROR_UNKNOWN;
        }
    }

    for (int i = 0; i < (int)idArrayPet.array.size(); i++) {
        int page, gridI, gridJ;
        if (pCon->m_PetBag.Find(idArrayPet.array[i], &page, &gridI, &gridJ)) {
            item = pCon->m_PetBag.GetItem(page, gridI, gridJ);
            if (pCon->CanItemTrade(2, page, gridI, gridJ) != TR_Can) {
                cre->StopPlayerTrade();
                return CMD_ERROR_NONE;
            }
            if (item.cBind == IB_Binded) {
                cre->StopPlayerTrade();
                return CMD_ERROR_NONE;
            }
            if (item.cLock != 0) {
                bNeedBankPass = true;
            }
            int tmpPage, tmpI, tmpJ;
            BOOL_ENSURE(Petbag.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
            counterPet++;
        } else {
            return CMD_ERROR_UNKNOWN;
        }
    }

    pTrade->SetItemCount(pCon, counterCommon, counterMission, counterPet);
    if (!MoneyIsValid(money)) {
        return CMD_ERROR_UNKNOWN;
    }
    if (money > pCon->GetMoney()) {}
    /*Tianh 09.12.15*通知玩家客户端金钱超过最大上限*自动关闭对话框*/
    if ((money + (pTradeWith->m_pItemContainer->GetMoney())) > MAX_MONEY_COUNT) {
        /*CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_deal_bagfull);
		pTradeWith->RecvCmd(g_sendCmd);*/
        CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_money_error);
        cre->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_money_error);
        pTradeWith->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_trade_cancel(cre->m_oId);
        pTradeWith->RecvCmd(g_sendCmd);

        return CMD_ERROR_UNKNOWN;
    }

    if (bNeedBankPass) {
        if (!CmdGiver->m_userInfo->CheckBankPass(strBankPass)) {
            CItemCmdBuilder_Svr::Build_r2c_trade_lock(cre->m_oId, itemmsg_bankpass_error);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    }

    pTrade->GetTradeBag(cre->m_pItemContainer)->Clear();
    pTrade->GetTradePetBag(cre->m_pItemContainer)->Clear();
    BOOL_ENSURE(bag.MoveToBag(pTrade->GetTradeBag(cre->m_pItemContainer)))
    BOOL_ENSURE(Petbag.MoveToBag(pTrade->GetTradePetBag(cre->m_pItemContainer)))
    pTrade->GetTradeBag(cre->m_pItemContainer)->SetMoney(money);
    pTrade->Lock(cre->m_pItemContainer);

    // 通知双方，此人已经点击了“确定”
    CItemCmdBuilder_Svr::Build_r2c_trade_lock(cre->m_oId, itemmsg_success);
    pTradeWith->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_trade_lock(cre->m_oId, itemmsg_success);
    CmdGiver->RecvCmd(g_sendCmd);

    // 把交易栏的物品传过去
    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_TRADE_OTHER_ITEMS,
                                               pTrade->GetTradeBag(cre->m_pItemContainer));
    pTradeWith->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_TRADE_OTHER_PET,
                                               pTrade->GetTradePetBag(cre->m_pItemContainer));
    pTradeWith->RecvCmd(g_sendCmd);

    if (pTrade->IsLocked(pTradeWith->m_pItemContainer)) {
        // 对方早已点击“确定”了，双方可以交易了
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_trade_unlock) {
    CRegionCreature* cre    = CmdGiver;
    CTrade*          pTrade = cre->m_pTrade;
    BOOL_ENSURE(pTrade)
    DWORD            dwTradeWith = pTrade->GetOtherPlayer(cre->m_oId);
    CRegionCreature* pTradeWith  = cre->m_scene->FindCreature(dwTradeWith);
    if (!pTradeWith) {
        cre->StopPlayerTrade();
        return CMD_ERROR_NONE;
    }
    DWORD dwForCheck = pTrade->GetOtherPlayer(pTradeWith->m_oId);
    if (dwForCheck != cre->m_oId) {
        ERR("[cmd_c2r_trade_unlock] Error in CTrade!\n");
        return CMD_ERROR_UNKNOWN;
    }

    if (pTradeWith->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    if (pTrade->IsLocked(cre->m_pItemContainer)) {
        cre->m_pTrade->UnLock();
        cre->m_pTrade->GetTradeBag(cre->m_pItemContainer)->Clear();
        cre->m_pTrade->GetTradeOtherBag(cre->m_pItemContainer)->Clear();
        cre->m_pTrade->GetTradePetBag(cre->m_pItemContainer)->Clear();
        cre->m_pTrade->GetTradeOtherPetBag(cre->m_pItemContainer)->Clear();

        CItemCmdBuilder_Svr::Build_r2c_trade_unlock(cre->m_oId);
        pTradeWith->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_trade_unlock(cre->m_oId);
        cre->RecvCmd(g_sendCmd);
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_trade_deal) {
    CRegionCreature* cre    = CmdGiver;
    CTrade*          pTrade = cre->m_pTrade;
    BOOL_ENSURE(pTrade)
    DWORD            dwTradeWith = pTrade->GetOtherPlayer(cre->m_oId);
    CRegionCreature* pTradeWith  = cre->m_scene->FindCreature(dwTradeWith);
    if (!pTradeWith || !pTradeWith->m_pItemContainer) {
        cre->StopPlayerTrade();
        return CMD_ERROR_NONE;
    }
    DWORD dwForCheck = pTrade->GetOtherPlayer(pTradeWith->m_oId);
    if (dwForCheck != cre->m_oId) {
        ERR("[cmd_c2r_trade_deal] Error in CTrade!\n");
        return CMD_ERROR_UNKNOWN;
    }

    if (pTradeWith->m_pItemContainer->IsOperateLocked() ||
        cre->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    SAFE_ASSERT(pTradeWith->m_pTrade == cre->m_pTrade)
    SAFE_ASSERT(!pTradeWith->m_pPShop && !cre->m_pPShop)

    if (pTrade->IsAgreed(pTradeWith->m_pItemContainer))  // 对方已经同意交易了,双方交易
    {
        if (pTrade->Deal()) {
            // 通知交易双方
            CItemCmdBuilder_Svr::Build_r2c_trade_deal(cre->m_oId);
            pTradeWith->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_trade_deal(pTradeWith->m_oId);
            cre->RecvCmd(g_sendCmd);
            // 更新道具栏
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       &(pTradeWith->m_pItemContainer->m_Bag));
            pTradeWith->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(
                ITEM_CONTAINER_PLAYER_MISSION_BAG, &(pTradeWith->m_pItemContainer->m_MissionBag));
            pTradeWith->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                       &(pTradeWith->m_pItemContainer->m_PetBag));
            pTradeWith->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       &(cre->m_pItemContainer->m_Bag));
            cre->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_MISSION_BAG,
                                                       &(cre->m_pItemContainer->m_MissionBag));
            cre->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_PET_BAG,
                                                       &(cre->m_pItemContainer->m_PetBag));
            cre->RecvCmd(g_sendCmd);

            // 记录到数据库
            CBag *pTradeBag1, *pTradeBag2, *pTradePetBag1, *pTradePetBag2;
            pTradeBag1    = pTrade->GetTradeBag(cre->m_pItemContainer);
            pTradeBag2    = pTrade->GetTradeOtherBag(cre->m_pItemContainer);
            pTradePetBag1 = pTrade->GetTradePetBag(cre->m_pItemContainer);
            pTradePetBag2 = pTrade->GetTradeOtherPetBag(cre->m_pItemContainer);
            CHECK(pTradeBag1 && pTradeBag2);
            CHECK(pTradePetBag1 && pTradePetBag2);
            vector<SItemID> items1;
            vector<SItemID> items2;
            vector<SItemID> itemsPet1;
            vector<SItemID> itemsPet2;
            pTradeBag1->ToItemsArray(items1);
            pTradeBag2->ToItemsArray(items2);
            pTradePetBag1->ToItemsArray(itemsPet1);
            pTradePetBag2->ToItemsArray(itemsPet2);

            string strBag1, strBag2;
            strBag1 += rtFormatNumber(GetItemSerializeVersion());
            strBag1 += ",";
            strBag1 += rtFormatNumber((int)items1.size());
            strBag1 += ",";
            for (int i = 0; i < (int)items1.size(); i++) {
                strBag1 += items1[i].SaveToString(g_region->m_pItemManager);
            }
            for (int i = 0; i < (int)itemsPet1.size(); i++) {
                strBag1 += itemsPet1[i].SaveToString(g_region->m_pItemManager);
            }
            strBag1 += rtFormatNumber(pTradeBag1->GetMoney());

            strBag2 += rtFormatNumber(GetItemSerializeVersion());
            strBag2 += ",";
            strBag2 += rtFormatNumber((int)items2.size());
            strBag2 += ",";
            for (int i = 0; i < (int)items2.size(); i++) {
                strBag2 += items2[i].SaveToString(g_region->m_pItemManager);
            }
            for (int i = 0; i < (int)itemsPet2.size(); i++) {
                strBag2 += itemsPet2[i].SaveToString(g_region->m_pItemManager);
            }
            strBag2 += rtFormatNumber(pTradeBag2->GetMoney());

            char          cTmp128[128];
            CG_CmdPacket& packet = g_region->m_gws.BeginSend();
            packet.WriteShort(r2g_log_event);
            packet.WriteByte(PLT_Trade);
            packet.WriteLong(cre->m_userInfo->m_userId);                          // userID
            packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
            packet.WriteLong(cre->m_userInfo->m_userId);                          // longParam1
            packet.WriteString((char*)strBag1.c_str());                           // strParam1
            packet.WriteLong(pTradeWith->m_userInfo->m_userId);                   // longParam2
            packet.WriteString((char*)strBag2.c_str());                           // strParam2
            packet.WriteLong(0L);                                                 // longParam3
            rt2_snprintf(cTmp128, 127, "%s %s", cre->m_core.Name.c_str(),
                         pTradeWith->m_core.Name.c_str());
            cTmp128[127] = 0;
            packet.WriteString(cTmp128);  // strParam3 (name1 name2)
            g_region->m_gws.EndSend();

            // 删除交易
            DEL_ONE(pTrade);
            pTrade               = NULL;
            pTradeWith->m_pTrade = NULL;
            cre->m_pTrade        = NULL;
        } else {
            /*Tianh 09.12.15*通知客户端道具包已满，并且自动关闭交易栏*/
            CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_deal_bagfull);
            pTradeWith->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_item_errorcode(itemmsg_deal_bagfull);
            cre->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_trade_cancel(cre->m_oId);
            pTradeWith->RecvCmd(g_sendCmd);
        }
    } else  // 对方还没有同意交易
    {
        pTrade->Agree(cre->m_pItemContainer);

        CItemCmdBuilder_Svr::Build_r2c_trade_other_agree(cre->m_oId);
        pTradeWith->RecvCmd(g_sendCmd);
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_requstID) {
    CRegionCreature* cre = CmdGiver;
    CItemCmdBuilder_Svr::Build_r2c_requstID();
    cre->RecvCmd(g_sendCmd);
    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_trade_cancel) {
    CRegionCreature* cre = CmdGiver;

    if (cre->StopPlayerTrade())
        return CMD_ERROR_NONE;
    else
        return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_shop_query) {
    CRegionCreature* cre = CmdGiver;
    SAFE_ASSERT(cre->m_scene)

    long NpcID;
    SAFE_READ(cmd->ReadLong(&NpcID))

    // CHECK
    // NPC存在，并且是NPC可以交易
    // 要检查人物距离NPC有多远
    //

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    if (!pNpc && g_region && g_region->m_defScene) {
        pNpc = g_region->m_defScene->FindCreature(NpcID);
    }
    SAFE_ASSERT(pNpc && pNpc->m_npcInfo)
    if (pNpc->m_pSellBag || !pNpc->m_NpcSellRareItem.empty()) {
        short nPriceRate = 100;
        if (!pNpc->m_npcInfo->Home.empty()) {
            DWORD dwCurrUnion = 0;
            int   nTaxRate    = 0;
            g_RegionFeodMgr.GetFeodInfo(pNpc->m_npcInfo->Home, dwCurrUnion, nTaxRate);
            if (dwCurrUnion != 0) {
                nPriceRate = 90 + nTaxRate;
            }
        }

        CItemCmdBuilder_Svr::Build_r2c_shop_buy_query_response(NpcID, pNpc->m_pSellBag,
                                                               pNpc->m_NpcSellRareItem, nPriceRate);
        cre->RecvCmd(g_sendCmd);
    } else {
        return CMD_ERROR_NONE;
    }

    return CMD_ERROR_NONE;
}

//tim.yang  自动出售
ITEM_CMD_FUNCTION(cmd_c2r_shop_autosell) {
    // CHECK
    // 玩家不在摆摊和交易状态
    // NPC存在，并且是NPC可以交易
    // 要检查人物距离NPC有多远
    // 这些东西是不是都是玩家的
    CRegionCreature* cre = CmdGiver;
    if (!cre->IsHuiYuan())  //只有会员才可以使用
    {
        return CMD_ERROR_UNKNOWN;
    }
    CRegionItemContainer* pCon     = cre->m_pItemContainer;
    CItemManager*         pItemMgr = g_region->m_pItemManager;
    SAFE_ASSERT(!cre->m_pPShop && !cre->m_pTrade)

    //long NpcID;
    //long moneyClientCalc;
    //SAFE_READ(cmd->ReadLong(&NpcID) )
    //	SAFE_READ(cmd->ReadLong(&moneyClientCalc) )
    SDWORDArray idArray;
    SAFE_ASSERT(idArray.Serialize(cmd))
    //	char* strBankPass;
    //SAFE_READ(cmd->ReadString(&strBankPass));

    //CRegionCreature *pNpc = cre->m_scene->FindCreature(NpcID);
    //SAFE_ASSERT(pNpc && pNpc->m_pSellBag)

    CBag            bagSell(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, g_region->m_pItemManager);
    vector<SItemID> itemsArray;

    //if (cre->Distance(pNpc->m_pos) > 300)
    //{
    //	CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
    //	CmdGiver->RecvCmd(g_sendCmd);
    //	return CMD_ERROR_NONE;
    //}

    //tim.yang  会员功能
    //if(pNpc->m_npcId == 1958)
    //{
    //	if(NpcID != CmdGiver->m_curHuiYuanHoptoadOid)
    //	{
    //		CmdGiver->SendSystemMessage(R(MSG_HUIYUAN_CANTSHOP));
    //		return CMD_ERROR_NONE;
    //	}
    //}
    //end
    // clone check
    if (CloneCheck(idArray.array)) {
        ERR1("[cmd_c2r_shop_sell] Find Clone Item(User=%s)\n", cre->m_core.Name.c_str());
        CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_UNKNOWN;
    }

    // 计算金币并确认玩家拥有
    float                  fPriceRate = itemshop_sell_pricerate;
    int                    moneyTotal = 0;
    vector<SAttribAddtion> atts;
    //bool bNeedBankPass = false;
    for (int i = 0; i < (int)idArray.array.size(); i++) {
        SItemID item;
        int     page = 0, gridI = 0, gridJ = 0;
        if (pCon->m_Bag.Find(idArray.array[i], &page, &gridI, &gridJ)) {
            item = pCon->m_Bag.GetItem(page, gridI, gridJ);
            ////if (pCon->CanItemTrade(true, page, gridI, gridJ)!=TR_Can)
            //if (!ItemCanNpcTrade(item.type))//是否可以NPC交易
            //{
            //	CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
            //	CmdGiver->RecvCmd(g_sendCmd);
            //	return CMD_ERROR_NONE;
            //}
            //if (!ItemCanTrade(item.type))//是否可以交易
            //{
            //	CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
            //	CmdGiver->RecvCmd(g_sendCmd);
            //	return CMD_ERROR_NONE;
            //}
            //if (item.cLock != 0)
            //{
            //	bNeedBankPass = true;
            //}
        } else {
            CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }

        int moneyBase = pItemMgr->GetItemValue(item);
        moneyTotal += moneyBase * fPriceRate * item.count;

        if (MAX_MONEY_COUNT - (pCon->m_Bag.GetMoney()) < moneyTotal) {
            return CMD_ERROR_NONE;
        }

        int tmpPage, tmpI, tmpJ;
        SAFE_ASSERT(bagSell.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
        itemsArray.push_back(item);
    }
    bagSell.AddMoney(moneyTotal);

    //if (bNeedBankPass)
    //{
    //	if (!CmdGiver->m_userInfo->CheckBankPass(strBankPass))
    //	{
    //		CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_bankpass_error);
    //		cre->RecvCmd(g_sendCmd);
    //		return CMD_ERROR_NONE;
    //	}
    //}

    //if (abs(moneyClientCalc-moneyTotal) > 15) // 允许15块钱的误差（可能由于浮点数造成的）
    //	//	if (moneyTotal != moneyClientCalc)
    //{
    //	//ERR3("[cmd_c2r_shop_sell] 玩家客户端计算的金钱数量与服务器不一致 (user=%s,server=%d,client=%d)\n", cre->m_core.Name.c_str(), moneyTotal, moneyClientCalc);
    //	if (moneyClientCalc==0)
    //	{
    //		CmdGiver->PossiblyUsingWG();
    //	}
    //	CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
    //	return CMD_ERROR_NONE;
    //}

    // 交易
    if (!cre->m_pItemContainer->RemoveFromArray(idArray.array)) {
        ERR1("[cmd_c2r_shop_sell] 向NPC出售道具时失败(user=%s)\n", cre->m_core.Name.c_str());
        return CMD_ERROR_UNKNOWN;
    }
    pCon->AddMoney(moneyTotal);

    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(cre->m_pItemContainer->m_Bag));
    cre->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_success);
    cre->RecvCmd(g_sendCmd);

    // log到数据库
    string strBag;
    strBag += rtFormatNumber(GetItemSerializeVersion());
    strBag += ",";
    strBag += rtFormatNumber((int)itemsArray.size());
    strBag += ",";
    for (int i = 0; i < (int)itemsArray.size(); i++) {
        strBag += itemsArray[i].SaveToString(g_region->m_pItemManager);
    }

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_SellToNpc);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(/*NpcID*/ 0);
    packet.WriteString((char*)strBag.c_str());
    packet.WriteLong(moneyTotal);
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

//end

ITEM_CMD_FUNCTION(cmd_c2r_shop_sell) {
    // CHECK
    // 玩家不在摆摊和交易状态
    // NPC存在，并且是NPC可以交易
    // 要检查人物距离NPC有多远
    // 这些东西是不是都是玩家的
    CRegionCreature*      cre      = CmdGiver;
    CRegionItemContainer* pCon     = cre->m_pItemContainer;
    CItemManager*         pItemMgr = g_region->m_pItemManager;
    SAFE_ASSERT(!cre->m_pPShop && !cre->m_pTrade)

    long NpcID;
    long moneyClientCalc;
    SAFE_READ(cmd->ReadLong(&NpcID))
    SAFE_READ(cmd->ReadLong(&moneyClientCalc))
    SDWORDArray idArray;
    SAFE_ASSERT(idArray.Serialize(cmd))
    char* strBankPass;
    SAFE_READ(cmd->ReadString(&strBankPass));

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc && pNpc->m_pSellBag)

    CBag            bagSell(SHOP_MAX_LINE, SHOP_GRID_COLUMN_COUNT, 1, g_region->m_pItemManager);
    vector<SItemID> itemsArray;

    if (cre->Distance(pNpc->m_pos) > 300) {
        CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    //tim.yang  会员功能
    //if(pNpc->m_npcId == 1958)
    //{
    //	if(NpcID != CmdGiver->m_curHuiYuanHoptoadOid)
    //	{
    //		CmdGiver->SendSystemMessage(R(MSG_HUIYUAN_CANTSHOP));
    //		return CMD_ERROR_NONE;
    //	}
    //}
    //end
    // clone check
    if (CloneCheck(idArray.array)) {
        ERR1("[cmd_c2r_shop_sell] Find Clone Item(User=%s)\n", cre->m_core.Name.c_str());
        CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_UNKNOWN;
    }

    // 计算金币并确认玩家拥有
    float                  fPriceRate = itemshop_sell_pricerate;
    int                    moneyTotal = 0;
    vector<SAttribAddtion> atts;
    bool                   bNeedBankPass = false;
    for (int i = 0; i < (int)idArray.array.size(); i++) {
        SItemID item;
        int     page = 0, gridI = 0, gridJ = 0;
        if (pCon->m_Bag.Find(idArray.array[i], &page, &gridI, &gridJ)) {
            item = pCon->m_Bag.GetItem(page, gridI, gridJ);
            //if (pCon->CanItemTrade(true, page, gridI, gridJ)!=TR_Can)
            if (!ItemCanNpcTrade(item.type)) {
                CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
                CmdGiver->RecvCmd(g_sendCmd);
                return CMD_ERROR_NONE;
            }
            if (item.cLock != 0) {
                bNeedBankPass = true;
            }
        } else {
            CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
        int tmpPage, tmpI, tmpJ;
        SAFE_ASSERT(bagSell.AddItem(item, &tmpPage, &tmpI, &tmpJ, false))
        itemsArray.push_back(item);

        int moneyBase = pItemMgr->GetItemValue(item);
        moneyTotal += moneyBase * fPriceRate * item.count;
    }
    bagSell.AddMoney(moneyTotal);

    if (bNeedBankPass) {
        if (!CmdGiver->m_userInfo->CheckBankPass(strBankPass)) {
            CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_bankpass_error);
            cre->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    }

    // if (abs(moneyClientCalc-moneyTotal) > 15) // 允许15块钱的误差（可能由于浮点数造成的）
    //	if (moneyTotal != moneyClientCalc)
    // {
    //ERR3("[cmd_c2r_shop_sell] 玩家客户端计算的金钱数量与服务器不一致 (user=%s,server=%d,client=%d)\n", cre->m_core.Name.c_str(), moneyTotal, moneyClientCalc);
    // if (moneyClientCalc==0)
    // {
    // CmdGiver->PossiblyUsingWG();
    // }
    // CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_unknown_error);
    // return CMD_ERROR_NONE;
    // }

    // 交易
    if (!cre->m_pItemContainer->RemoveFromArray(idArray.array)) {
        ERR1("[cmd_c2r_shop_sell] 向NPC出售道具时失败(user=%s)\n", cre->m_core.Name.c_str());
        return CMD_ERROR_UNKNOWN;
    }
    pCon->AddMoney(moneyTotal);

    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(cre->m_pItemContainer->m_Bag));
    cre->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(itemmsg_success);
    cre->RecvCmd(g_sendCmd);

    // log到数据库
    string strBag;
    strBag += rtFormatNumber(GetItemSerializeVersion());
    strBag += ",";
    strBag += rtFormatNumber((int)itemsArray.size());
    strBag += ",";
    for (int i = 0; i < (int)itemsArray.size(); i++) {
        strBag += itemsArray[i].SaveToString(g_region->m_pItemManager);
    }

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_SellToNpc);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(NpcID);
    packet.WriteString((char*)strBag.c_str());
    packet.WriteLong(moneyTotal);
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_shop_buy) {
    // CHECK
    // NPC存在，并且是NPC可以交易
    // 要检查人物距离NPC有多远
    // NPC必须出售过购买的所有道具
    // 客户端发过来的计算过得金币数目要和服务器一致，否则认为出错
    // 金币足够
    // 玩家道具栏有足够的空间
    CRegionCreature*    cre  = CmdGiver;
    CItemContainerBase* pCon = cre->m_pItemContainer;
    if (cre->m_pTrade || cre->m_pPShop)
        return CMD_ERROR_NONE;
    long NpcID;
    long moneyClientCalc;
    SAFE_READ(cmd->ReadLong(&NpcID))
    SAFE_READ(cmd->ReadLong(&moneyClientCalc))
    SDWORDArray idArray;
    BOOL_ENSURE(idArray.Serialize(cmd))

    SItemBase*      pItemBase = NULL;
    vector<SItemID> arrItems;

    vector<SItemID>* point_arrItems;
    // 这个指针会在扣点返回后删除 tooth
    point_arrItems = RT_NEW vector<SItemID>;

    float fPriceRate = itemshop_buy_pricerate;

    // 用于统计卖掉多少道具
    EXT_SPACE::unordered_map<DWORD, int>           hmItemSaleStat;
    EXT_SPACE::unordered_map<DWORD, int>::iterator iterSaleStat;

    SItemID          item;
    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);

    if (!pNpc && g_region && g_region->m_defScene) {
        pNpc = g_region->m_defScene->FindCreature(NpcID);
    }
    SAFE_ASSERT(pNpc && pNpc->m_npcInfo)
    if (!pNpc->m_pSellBag) {
        return CMD_ERROR_NONE;
    }
    if (!pNpc->m_npcInfo) {
        return CMD_ERROR_NONE;
    }

    char                        ShopType = 0;
    std::vector<long>::iterator it       = pNpc->m_npcInfo->Function.begin();
    for (; it != pNpc->m_npcInfo->Function.end(); it++) {
        if ((*it) == NF_Buy) {
            ShopType = NF_Buy;
        } else if ((*it) == NF_UnionRecord)  // 帮贡商店
        {
            ShopType = NF_Buy;
        } else if ((*it) == NF_JobRecord)  // 门贡商店
        {
            ShopType = NF_JobRecord;
        } else if ((*it) == NF_Credit)  // 仙誉商店
        {
            ShopType = NF_Credit;
        } else if ((*it) == NF_Record)  // 军功商店
        {
            ShopType = NF_Record;
        } else if ((*it) == NF_Contribute)  // 俗名
        {
            ShopType = NF_Contribute;
        } else if ((*it) == NF_Point)  // 积分
        {
            ShopType = NF_Point;
        }
    }

    if (cre->Distance(pNpc->m_pos) > 200 && NpcID != g_region->m_shopnpcoid)  //heten
    {
        CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    //tim.yang  会员功能
    //if(pNpc->m_npcId == 1958)
    //{
    //	if(NpcID != CmdGiver->m_curHuiYuanHoptoadOid)
    //	{
    //		CmdGiver->SendSystemMessage(R(MSG_HUIYUAN_CANTSHOP));
    //		return CMD_ERROR_NONE;
    //	}
    //}
    //end

    // 得到采邑税率，计算价格倍率，国库倍率
    float fPriceRate_UnionBank = 0.0f;
    if (!pNpc->m_npcInfo->Home.empty()) {
        DWORD dwCurrUnion = 0;
        int   nTaxRate    = 0;
        g_RegionFeodMgr.GetFeodInfo(pNpc->m_npcInfo->Home, dwCurrUnion, nTaxRate);
        if (dwCurrUnion != 0) {
            fPriceRate *= (90 + nTaxRate) / 100.0f;
            fPriceRate_UnionBank = (nTaxRate - 10) / 100.0f;
            if (fPriceRate_UnionBank < 0.0f || fPriceRate_UnionBank > 1.0f)
                fPriceRate_UnionBank = 0.0f;
        }
    }

    std::vector<SNpcSellRareItem> cloneRareItems       = pNpc->m_NpcSellRareItem;
    int                           moneyTotal           = 0;
    int                           moneyTotal_UnionBank = 0;
    int                           nCommonItemCount     = 0;
    int                           nMissionItemCount    = 0;
    float                         point                = 0.0;
    for (int i = 0; i < (int)idArray.array.size(); i++) {
        int page = 0, gridI = 0, gridJ = 0;
        int iRareIdx = -1;

        if (pNpc->m_pSellBag->Find(idArray.array[i], &page, &gridI, &gridJ)) {
            item = pNpc->m_pSellBag->GetItem(page, gridI, gridJ);
        } else {
            iRareIdx = 0;
            stlforeach(std::vector<SNpcSellRareItem>, rareIter, cloneRareItems) {
                SNpcSellRareItem& sri = *rareIter;
                if (sri.item.id == idArray.array[i])
                    break;
                iRareIdx++;
            }
            if (iRareIdx >= 0 && iRareIdx < (int)cloneRareItems.size()) {
                SNpcSellRareItem& sri = cloneRareItems[iRareIdx];
                if (sri.count <= 0) {
                    CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
                    CmdGiver->RecvCmd(g_sendCmd);
                    return CMD_ERROR_NONE;
                } else {
                    sri.count--;
                    item = sri.item;
                }
            } else {
                CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
                CmdGiver->RecvCmd(g_sendCmd);
                return CMD_ERROR_NONE;
            }
        }

        if (ItemIsMissionItem(item.type))
            nMissionItemCount++;
        else
            nCommonItemCount++;
        // 生成一个新的item
        item = g_region->m_pItemManager->CreateItem(item);
        SAFE_ASSERT(ItemID_IsValid(item));

        // 统计系统出售数量
        int tmpItemIdx = g_region->m_pItemManager->GetIndexFromType(item.type);
        iterSaleStat   = hmItemSaleStat.find(tmpItemIdx);
        if (iterSaleStat == hmItemSaleStat.end())
            hmItemSaleStat[tmpItemIdx] = 1;
        else
            hmItemSaleStat[tmpItemIdx] += 1;

        //if (4635 == tmpItemIdx)//新的会员道具，使用期限31天
        //{
        //	item.TimeLimit = time(NULL) + 31 * 24 * 60 * 60;
        //}

        pItemBase = g_region->m_pItemManager->GetItem(item.type);
        BOOL_ENSURE(pItemBase)
        //if (pItemBase->point > 0)
        //{
        //	point += pItemBase->point * item.count;
        //	point_arrItems->push_back(item);
        //}
        if (ShopType == NF_Buy) {
            if (pItemBase->value > 0) {
                moneyTotal += rtRound(pItemBase->value * fPriceRate * item.count);
            }
        } else if (ShopType == NF_Credit) {
            if (pItemBase->itemCredit > 0)  // 仙誉
            {
                moneyTotal += rtRound(pItemBase->itemCredit * fPriceRate * item.count);
            }
        } else if (ShopType == NF_Contribute) {
            if (pItemBase->itemContribute > 0)  // 俗名
            {
                moneyTotal += rtRound(pItemBase->itemContribute * fPriceRate * item.count);
            }
        } else if (ShopType == NF_Point) {
            if (pItemBase->itemPoint > 0)  // 积分
            {
                moneyTotal += rtRound(pItemBase->itemPoint * fPriceRate * item.count);
            }
        } else if (ShopType == NF_Record) {
            if (pItemBase->itemRecord > 0)  // 军功
            {
                moneyTotal += rtRound(pItemBase->itemRecord * fPriceRate * item.count);
            }
        } else if (ShopType == NF_UnionRecord) {
            if (pItemBase->itemUnionRecord > 0)  // 帮贡
            {
                moneyTotal += rtRound(pItemBase->itemUnionRecord * fPriceRate * item.count);
            }
        } else if (ShopType == NF_UnionRecord) {
            if (pItemBase->itemJobRecord > 0)  // 门贡
            {
                moneyTotal += rtRound(pItemBase->itemJobRecord * fPriceRate * item.count);
            }
        }
        moneyTotal_UnionBank += rtRound(pItemBase->value * fPriceRate_UnionBank * item.count);
        arrItems.push_back(item);
    }
    BOOL_ENSURE(moneyTotal >= 0)

    if (pCon->m_Bag.FreeSpace() < nCommonItemCount ||
        pCon->m_MissionBag.FreeSpace() < nMissionItemCount) {
        //LOG("玩家的道具栏空位不够\n");
        CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    //判断钱数等够不够
    if (ShopType == NF_Credit)  // 仙誉
    {
        if (cre->m_core.Credit < moneyTotal) {
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    } else if (ShopType == NF_Contribute)  // 俗名
    {
        if (cre->m_core.Contribute < moneyTotal) {
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    } else if (ShopType == NF_Point)  // 积分
    {
        if (cre->m_core.Point < moneyTotal) {
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    } else if (ShopType == NF_Record)  // 军功
    {
        if (cre->m_core.Record < moneyTotal) {
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    } else if (ShopType == NF_UnionRecord)  // 帮贡
    {
        if (cre->m_core.UnionRecord < moneyTotal) {
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    } else if (ShopType == NF_JobRecord)  // 门贡
    {
        if (cre->m_core.JobRecord < moneyTotal) {
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    } else if (ShopType == NF_Buy)  // 金币
    {
        if (cre->m_pItemContainer->GetMoney() < moneyTotal) {
            //LOG("玩家钱币不够买这些东西\n");
            CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
            CmdGiver->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    }

    if (abs(moneyClientCalc - moneyTotal) > 50 &&
        point == 0)  // 允许50块钱的误差（可能由于浮点数造成的）
    {
        //ERR3("[cmd_c2r_shop_buy] 玩家客户端计算的金钱数量与服务器不一致 (user=%s,server=%d,client=%d)\n", cre->m_core.Name.c_str(), moneyTotal, moneyClientCalc);
        if (moneyClientCalc == 0) {
            CmdGiver->PossiblyUsingWG();
        }
        CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
        return CMD_ERROR_NONE;
    }

    // 交易
    if (point != 0 && cre->IsUser()) {
        cre->m_userInfo->Deduct(rtRound(point), CDeduct::GetDeductInstance()->GetDeductId(),
                                DEDUCT_TYPE_BUYITEM, point_arrItems);
    }

    if (ShopType == NF_UnionRecord) {
        cre->m_core.UnionRecord -= moneyTotal;
    } else if (ShopType == NF_JobRecord) {
        cre->m_core.JobRecord -= moneyTotal;
    } else if (ShopType == NF_Credit) {
        cre->m_core.Credit -= moneyTotal;
    } else if (ShopType == NF_Record) {
        cre->m_core.Record -= moneyTotal;
    } else if (ShopType == NF_Point) {
        cre->m_core.Point -= moneyTotal;
    } else if (ShopType == NF_Contribute) {
        cre->m_core.Contribute -= moneyTotal;
    } else if (ShopType == NF_Buy) {
        cre->m_pItemContainer->RemoveMoney(moneyTotal);
    }

    if (!cre->m_pItemContainer->AddItem(arrItems)) {
        ERR("从NPC处买东西时出错：[AddItem]");
        CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_UNKNOWN;
    }
    pNpc->m_NpcSellRareItem = cloneRareItems;

    // 添加到统计数据中
    iterSaleStat = hmItemSaleStat.begin();
    for (; iterSaleStat != hmItemSaleStat.end(); iterSaleStat++) {
        g_ItemSaleStat.Add(iterSaleStat->first, iterSaleStat->second);
    }

    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(cre->m_pItemContainer->m_Bag));
    cre->RecvCmd(g_sendCmd);
    if (point == 0) {
        CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(1);
        cre->RecvCmd(g_sendCmd);
    }

    // 税提交国库
    if (moneyTotal_UnionBank > 0) {
        g_RegionFeodMgr.AddIncome(pNpc->m_npcInfo->Home, moneyTotal_UnionBank);
    }

    //// Temp Log NPC's income
    //HASH_STR(std::string, forLogNPCIncome)::iterator iter = s_forLogNPCIncome.find(pNpc->m_npcInfo->Name);
    //if (iter==s_forLogNPCIncome.end())
    //{
    //	forLogNPCIncome tmp;
    //	tmp.Income = moneyTotal;
    //	time(&(tmp.StartTimeT));
    //	struct tm *st = localtime(&(tmp.StartTimeT));
    //	tmp.StartTime = *st;

    //	s_forLogNPCIncome[pNpc->m_npcInfo->Name] = tmp;
    //}
    //else
    //{
    //	forLogNPCIncome& tmp = iter->second;
    //	tmp.Income += moneyTotal;

    //	time_t now;
    //	time(&now);
    //	if (now - tmp.StartTimeT > 3600)
    //	{
    //		struct tm *st = &(tmp.StartTime);
    //		struct tm *en = localtime(&now);
    //		ERR8("NPC INCOME(name=%s, starttime=%d %d:%d, endtime=%d %d:%d, income=%d)\n", pNpc->m_npcInfo->Name.c_str(), st->tm_mday, st->tm_hour, st->tm_min, en->tm_mday, en->tm_hour, en->tm_min, tmp.Income);

    //		tmp.StartTimeT = now;
    //		tmp.StartTime = *en;
    //		tmp.Income = 0;
    //	}
    //}

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_pshop_start) {
    // CHECK
    // 玩家不在摆摊状态和交易状态和战斗状态
    // 商店里的道具必须在道具栏里有
    CRegionCreature*      cre  = CmdGiver;
    CRegionItemContainer* pCon = cre->m_pItemContainer;

    if (cre->m_eCurPosPKAttr != PKA_SAFEST) {
        rt2_sprintf(g_strStaticBuffer, R(MSG_BAITAN_CANNOTBAIHERE));
        cre->SendSystemMessage(g_strStaticBuffer);
        return CMD_ERROR_NONE;
    }
    if (cre->m_core.Lev < 10)  //限制摆摊10级
    {
        return CMD_ERROR_NONE;
    }
    if (cre->m_pTrade)
        return CMD_ERROR_NONE;
    if (cre->m_lIsCombat) {
        return CMD_ERROR_NONE;
    }
    if (!cre->State_CanPShop()) {
        return CMD_ERROR_NONE;
    }

    if (cre->m_pPShop) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_success);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;  // 已经在摆摊了
    }

    CPrivateShop* pPShop = RT_NEW CPrivateShop(g_region->m_pItemManager);
    //接收客户端发来的物品  卖和买的东西
    if (!pPShop->Serialize(cmd) || pPShop->GetName().size() > 512) {
        DEL_ONE(pPShop);

        CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_unknown_error);
        cre->RecvCmd(g_sendCmd);

        return CMD_ERROR_NONE;
    }
    char* strBankPass;
    SAFE_READ(cmd->ReadString(&strBankPass));

    // clone check
    vector<DWORD> idList;
    pPShop->GetSellItemBag()->ToItemIDArray(idList);

    //买的物品不需要检查 这里只检查卖出的物品
    /*pPShop->GetBuyItemBag()->ToItemIDArray();*/
    if (CloneCheck(idList)) {
        ERR1("[cmd_c2r_pshop_start] Find Clone Item(User=%s)\n", cre->m_core.Name.c_str());
        CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_unknown_error);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_UNKNOWN;
    }

    bool bNeedBankPass = false;
    for (int i = 0; i < (int)idList.size(); i++) {
        SItemID item;
        int     page, gridI, gridJ;

        if (pCon->m_Bag.Find(idList[i], &page, &gridI, &gridJ)) {
            item = pCon->m_Bag.GetItem(page, gridI, gridJ);
            if (pCon->CanItemTrade(1, page, gridI, gridJ) != TR_Can) {
                CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_unknown_error);
                cre->RecvCmd(g_sendCmd);
                return CMD_ERROR_NONE;
            }
            if (item.cBind == IB_Binded) {
                CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_unknown_error);
                cre->RecvCmd(g_sendCmd);
                return CMD_ERROR_NONE;
            }
            if (item.cLock != 0) {
                bNeedBankPass = true;
            }
        } else {
            return CMD_ERROR_NONE;
        }
    }

    if (bNeedBankPass) {
        if (!CmdGiver->m_userInfo->CheckBankPass(strBankPass)) {
            CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_bankpass_error);
            cre->RecvCmd(g_sendCmd);
            return CMD_ERROR_NONE;
        }
    }

    if (!cre->m_pItemContainer->IsContain(pPShop->GetSellItemBag())) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_unknown_error);
        cre->RecvCmd(g_sendCmd);

        return CMD_ERROR_UNKNOWN;
    }
    pPShop->SetPlayer(cre->m_oId, cre->m_pItemContainer);
    if (!pPShop->Start()) {
        DEL_ONE(pPShop);

        CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_unknown_error);
        cre->RecvCmd(g_sendCmd);

        return CMD_ERROR_NONE;
    }
    cre->m_pPShop = pPShop;
    cre->m_bPShop = 1;
    cre->m_modelRef++;
    CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(itemmsg_success);
    cre->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_pshop_stop) {
    CRegionCreature* cre = CmdGiver;

    // CHECK
    // 玩家必须在摆摊状态
    cre->StopPShop();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_pshop_query_other) {
    // CHECK
    // 被查询玩家必须在摆摊状态，并且不在交易状态
    CRegionCreature* cre = CmdGiver;

    long actorID;
    SAFE_READ(cmd->ReadLong(&actorID))
    CRegionCreature* pPlayer = cre->m_scene->FindCreature(actorID);
    if (!pPlayer || !pPlayer->IsUser() || !pPlayer->m_pPShop || pPlayer->m_pTrade)
        return CMD_ERROR_NONE;

    if (pPlayer->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    if (cre->Distance(pPlayer->m_pos) > 240) {
        return CMD_ERROR_NONE;
    }

    CItemCmdBuilder_Svr::Build_r2c_pshop_content(pPlayer->m_pPShop);
    cre->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_pshop_buy) {
    // CHECK
    // 出售玩家必须在摆摊状态
    // 购买列表必须包含于出售列表之中（如果不符合，可能是因为有物品已经被别人买走，要告知买方）
    // 出售列表必须包含于玩家背包中(这种情况也是可能的)
    // 购买玩家的金币必须足够
    // 玩家发回来的价格要与实际价格相同
    //

    CRegionCreature* cre = CmdGiver;
    SAFE_ASSERT(cre->m_userInfo)

    long actorID = 0;
    SAFE_READ(cmd->ReadLong(&actorID))
    long ItemID = 0;
    SAFE_READ(cmd->ReadLong(&ItemID))  //买到的物品
    long vMoney = 0;
    SAFE_READ(cmd->ReadLong(&vMoney))  //物品的单价
    short amount = 0;
    SAFE_READ(cmd->ReadShort(&amount))  //物品的数量---堆叠的 如果为不是堆叠的则接收为0

    CRegionCreature* pPlayer = cre->m_scene->FindCreature(actorID);
    if (!pPlayer || !pPlayer->IsUser()) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_PSHOP_NOPLAYER, ItemID,
                                                        amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (!pPlayer->m_pPShop) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(pPlayer->m_oId, 0,
                                                        ITEM_REASON_PSHOP_NOPSHOP, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (cre->m_pTrade || pPlayer->m_pTrade)
        return CMD_ERROR_NONE;
    CBag* pPShopBag = pPlayer->m_pPShop->GetSellItemBag();
    SAFE_ASSERT(pPShopBag)

    if (!pPlayer->m_pItemContainer) {
        return CMD_ERROR_NONE;
    }

    if (pPlayer->m_pItemContainer->IsOperateLocked() || cre->m_pItemContainer->IsOperateLocked()) {
        return CMD_ERROR_NONE;
    }

    //得到卖家背包里的这件物品
    SItemID rItem = pPShopBag->GetItemFromID(ItemID);
    if (!ItemID_IsValid(rItem)) {
        return CMD_ERROR_NONE;
    }
    SItemID rItemPlayer = pPlayer->m_pItemContainer->m_Bag.GetItemFromID(ItemID);
    if (!ItemID_IsValid(rItem)) {
        return CMD_ERROR_NONE;
    }
    if (rItemPlayer.count < amount) {
        return CMD_ERROR_NONE;
    }

    if (cre->Distance(pPlayer->m_pos) > 240) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_NULL, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (!pPlayer->m_pItemContainer) {
        return CMD_ERROR_NONE;
    }
    if (!pPlayer->m_pItemContainer->IsContain(pPShopBag)) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_NULL, ItemID, amount);
        cre->RecvCmd(g_sendCmd);

        pPlayer->m_pPShop->Stop();
        DEL_ONE(pPlayer->m_pPShop);
        pPlayer->m_bPShop = 0;
        pPlayer->m_modelRef++;
        CItemCmdBuilder_Svr::Build_r2c_pshop_stop_response(1);
        pPlayer->RecvCmd(g_sendCmd);

        return CMD_ERROR_UNKNOWN;
    }

    if (!pPShopBag->Find(rItem)) {
        // 可能是有物品已经被别人买走了,通知买家
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(pPlayer->m_oId, 0, ITEM_REASON_PSHOP_NOITEM,
                                                        ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_pshop_content(pPlayer->m_pPShop);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (rItem.cBind == IB_Binded) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(pPlayer->m_oId, 0, ITEM_REASON_PSHOP_NOITEM,
                                                        ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    int price             = 0;  //这件物品单价
    int money             = 0;
    int nCommonItemCount  = 0;
    int nMissionItemCount = 0;

    if (ItemIsMissionItem(rItem))
        nMissionItemCount++;
    else
        nCommonItemCount++;
    price = pPlayer->m_pPShop->GetPrice(rItem.id);

    if (vMoney != price) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_NULL, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (amount != 0)  //买到的如果是叠加物品
    {
        price = price * amount;
    }
    money += price;

    if (!MoneyIsValid(money)) {
        //ERR("[cmd_c2r_bank_store]金钱数量不合法.\n");
        return CMD_ERROR_UNKNOWN;
    }

    if (money > cre->m_pItemContainer->GetMoney()) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_NULL, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (cre->m_pItemContainer->m_Bag.FreeSpace() < nCommonItemCount ||
        cre->m_pItemContainer->m_MissionBag.FreeSpace() < nMissionItemCount) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_NULL, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    // 交易
    if (amount != 0) {
        if (amount > rItem.count) {
            return CMD_ERROR_NONE;
        }
        //设置卖方的背包
        int page, gridI, gridJ;
        if (!pPlayer->m_pItemContainer->m_Bag.Find(rItem, &page, &gridI, &gridJ)) {
            return CMD_ERROR_NONE;
        }

        int page1, gridI1, gridJ1;
        if (!pPShopBag->Find(rItem, &page1, &gridI1, &gridJ1)) {
            return CMD_ERROR_NONE;
        }
        //pPShopBag->SetItem(page1, gridI1, gridJ1,rItem);

        rItem.count -= amount;
        if (rItem.count == 0x0) {
            pPlayer->m_pItemContainer->m_Bag.RemoveItem(rItem);
            pPlayer->m_pPShop->RemoveSellItem(rItem.id);
        } else {
            pPlayer->m_pItemContainer->m_Bag.SetItem(page, gridI, gridJ, rItem);
            pPShopBag->SetItem(page1, gridI1, gridJ1, rItem);
        }

        CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                   &(pPlayer->m_pItemContainer->m_Bag));
        pPlayer->RecvCmd(g_sendCmd);

        //设置买家的背包(自己的)--生成一个新的物品给买家
        SItemID vItemID;
        if (!cre->m_pItemContainer->m_Bag.m_pItemMgr) {
            return CMD_ERROR_NONE;
        }
        vItemID       = cre->m_pItemContainer->m_Bag.m_pItemMgr->CreateItem(rItem);
        vItemID.count = amount;

        //给买家在背包里找合适的格子
        //for (int p=0; p<cre->m_pItemContainer->m_Bag.GetPageCount(); p++)
        //	for (int i=0; i<cre->m_pItemContainer->m_Bag.GetLineCount(); i++)
        //		for (int j=0; j<cre->m_pItemContainer->m_Bag.GetColCount(); j++)
        //		{
        //			if ( !ItemID_IsValid(cre->m_pItemContainer->m_Bag.GetItem(i, j)))
        //			{
        //				cre->m_pItemContainer->m_Bag.SetItem(p, i, j, vItemID);
        //				break;
        //			}
        //		}
        cre->m_pItemContainer->m_Bag.AddItem(vItemID);
        CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                   &(cre->m_pItemContainer->m_Bag));
        cre->RecvCmd(g_sendCmd);

    } else if (pPShopBag->RemoveItem(rItem)) {
        if (pPlayer->m_pItemContainer->RemoveItem(rItem)) {
            int page, gridI, gridJ;
            pPlayer->m_pItemContainer->m_Bag.Find(rItem, &page, &gridI, &gridJ);
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                          gridI, gridJ, ITEM_REASON_DELETE);
            pPlayer->RecvCmd(g_sendCmd);
            int tmpPage, tmpI, tmpJ;
            //这里不让买方买到后直接进行堆叠
            cre->m_pItemContainer->m_Bag.AddItem(rItem, &tmpPage, &tmpI, &tmpJ, false);
            CItemCmdBuilder_Svr::Build_r2c_bag_additem(rItem, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       tmpPage, tmpI, tmpJ, ITEM_REASON_NULL);
            cre->RecvCmd(g_sendCmd);
        }
    }

    cre->m_pItemContainer->RemoveMoney(money);
    CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(-money, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                ITEM_REASON_PICK);
    cre->RecvCmd(g_sendCmd);

    if (MAX_MONEY_COUNT - (pPlayer->m_pItemContainer->GetMoney()) >= money) {
        pPlayer->m_pItemContainer->AddMoney(money);
    } else {
        pPlayer->m_pItemContainer->AddMoney(0);
    }
    CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(money, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                ITEM_REASON_PICK);
    pPlayer->RecvCmd(g_sendCmd);

    /*pPShopBag->Pinch();*/

    CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(pPlayer->m_oId, 1, ITEM_REASON_NULL, ItemID,
                                                    amount);
    cre->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_pshop_other_buy(cre->m_oId, ItemID, money, amount);
    pPlayer->RecvCmd(g_sendCmd);

    // 如果贩卖物品都卖光了，就停止摆摊
    //if (pPShopBag->IsEmpty())
    //{
    //	pPlayer->m_pPShop->Stop();
    //	DEL_ONE( pPlayer->m_pPShop )
    //	pPlayer->m_bPShop = 0;
    //	pPlayer->m_modelRef ++;
    //	CItemCmdBuilder_Svr::Build_r2c_pshop_stop_response(1);
    //	pPlayer->RecvCmd(g_sendCmd);
    //}

    // log到数据库
    //string strBag;
    //int moneyDeal = 0;
    //string strNames;

    //strBag += rtFormatNumber(GetItemSerializeVersion());
    //strBag += ",";
    //strBag += rtFormatNumber((int)itemArray.size());
    //strBag += ",";
    //for (int i=0; i<(int)itemArray.size(); i++)
    //{
    //	strBag += itemArray[i].SaveToString(g_region->m_pItemManager);
    //	strBag += rtFormatNumber((int)priceArray.array[i]);
    //	strBag += ",";
    //	moneyDeal += priceArray.array[i];
    //}
    //strNames += cre->m_userInfo->m_username;
    //strNames += " ";
    //strNames += pPlayer->m_userInfo->m_username;

    //CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    //packet.WriteShort(r2g_log_event);
    //packet.WriteByte(PLT_BuyFromPShop);
    //packet.WriteLong(cre->m_userInfo->m_userId);
    //packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str()); // userIP
    //packet.WriteLong(cre->m_userInfo->m_userId);
    //packet.WriteString((char*)strBag.c_str());
    //packet.WriteLong(pPlayer->m_userInfo->m_userId);
    //packet.WriteLong(moneyDeal);
    //packet.WriteString((char*)strNames.c_str());
    //g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

//Tianh 通过个人摆摊的商店卖东西
//站在卖家的角度向摆摊的买家卖东西

//摆摊人的ID（买家）
//买家物品假ID
//卖家物品真ID
//卖家物品数量
//物品单价
ITEM_CMD_FUNCTION(cmd_c2r_pshop_sell) {
    CRegionCreature* cre = CmdGiver;

    SAFE_ASSERT(cre->m_userInfo);

    long actorID;
    SAFE_READ(cmd->ReadLong(&actorID));
    long ItemID = 0;
    SAFE_READ(cmd->ReadLong(&ItemID))  //买到的物品
    long FakeItem = 0;
    SAFE_READ(cmd->ReadLong(&FakeItem));
    DWORD FakeItemID = (DWORD)FakeItem;
    long  vMoney     = 0;
    SAFE_READ(cmd->ReadLong(&vMoney))  //物品的单价
    short amount = 0;
    SAFE_READ(cmd->ReadShort(&amount))  //物品的数量---堆叠的 如果为不是堆叠的则接收为0

    //得到买家的CRegionCreature类指针
    CRegionCreature* pPlayer = cre->m_scene->FindCreature(actorID);

    if (!pPlayer || !pPlayer->IsUser()) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(0, 0, ITEM_REASON_PSHOP_NOPLAYER,
                                                         FakeItemID, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    //如果买家不在商店状态
    if (!pPlayer->m_pPShop) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(
            pPlayer->m_oId, 0, ITEM_REASON_PSHOP_NOPSHOP, FakeItemID, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    //卖家和买家如果都在交易的状态
    if (cre->m_pTrade || pPlayer->m_pTrade)
        return CMD_ERROR_NONE;

    //得到买东西的买家背包
    CBag* pPShopBag = pPlayer->m_pPShop->GetBuyItemBag();
    SAFE_ASSERT(pPShopBag);

    SItemID vFakeItemID = pPShopBag->GetItemFromID(FakeItemID);

    //判断交易距离
    if (cre->Distance(pPlayer->m_pos) > 240) {
        return CMD_ERROR_NONE;
    }

    //得到卖家背包里的这件物品
    SItemID rItem = cre->m_pItemContainer->m_Bag.GetItemFromID(ItemID);

    if (!pPlayer->m_pItemContainer) {
        return CMD_ERROR_NONE;
    }

    //if (!pPlayer->m_pItemContainer->IsContain(pPShopBag))
    //{
    //	//CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(0, 0, ITEM_REASON_NULL);
    //	//cre->RecvCmd(g_sendCmd);

    //	pPlayer->m_pPShop->Stop();
    //	DEL_ONE( pPlayer->m_pPShop )
    //	pPlayer->m_bPShop = 0;
    //	pPlayer->m_modelRef ++;
    //	CItemCmdBuilder_Svr::Build_r2c_pshop_stop_response(1);
    //	pPlayer->RecvCmd(g_sendCmd);

    //	return CMD_ERROR_UNKNOWN;
    //}

    if (!pPShopBag->Find(FakeItemID))  //查找商店的东西用假ID来查找
    {
        //可能是有物品已经被别人买走了,通知买家
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(
            pPlayer->m_oId, 0, ITEM_REASON_PSHOP_NOITEM, FakeItemID, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_pshop_content(pPlayer->m_pPShop);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (rItem.cBind == IB_Binded) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(
            pPlayer->m_oId, 0, ITEM_REASON_PSHOP_NOITEM, FakeItemID, ItemID, amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    int price = 0;  //这件物品单价
    int money = 0;

    price = pPlayer->m_pPShop->GetPrice(FakeItemID);

    if (vMoney != price) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(0, 0, ITEM_REASON_NULL, FakeItemID, ItemID,
                                                         amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (amount != 0)  //买到的如果是叠加物品
    {
        price = price * amount;
    }
    money += price;

    if (!MoneyIsValid(money)) {
        //ERR("[cmd_c2r_bank_store]金钱数量不合法.\n");
        return CMD_ERROR_UNKNOWN;
    }

    if (money > pPlayer->m_pItemContainer->GetMoney()) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(0, 0, ITEM_REASON_NULL, FakeItemID, ItemID,
                                                         amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (pPlayer->m_pItemContainer->m_Bag.FreeSpace() < 1 ||
        pPlayer->m_pItemContainer->m_MissionBag.FreeSpace() < 1) {
        CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(0, 0, ITEM_REASON_NULL, FakeItemID, ItemID,
                                                         amount);
        cre->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    //满足条件开始交易
    if (amount != 0) {
        //卖家的背包里可能有同类型的 好几组物品
        //比如买家收99个   卖家包里有N组 20个一叠的同类型物品  要做特殊处理
        long NumBytype = cre->m_pItemContainer->m_Bag.GetNumBytype(rItem.type, rItem.cBind);
        if (NumBytype < amount) {
            return CMD_ERROR_NONE;
        }

        //设置商店
        int page1, gridI1, gridJ1;
        if (!pPShopBag->Find(vFakeItemID, &page1, &gridI1, &gridJ1)) {
            return CMD_ERROR_NONE;
        }
        vFakeItemID.count -= amount;
        if (vFakeItemID.count == 0x0) {
            pPShopBag->RemoveItem(vFakeItemID);
        } else {
            pPShopBag->SetItem(page1, gridI1, gridJ1, vFakeItemID);
        }

        //设置买家的背包(自己的)--生成一个新的物品给买家
        SItemID vItemID;
        if (!pPlayer->m_pItemContainer->m_Bag.m_pItemMgr) {
            return CMD_ERROR_NONE;
        }
        vItemID       = pPlayer->m_pItemContainer->m_Bag.m_pItemMgr->CreateItem(vFakeItemID);
        vItemID.count = amount;

        pPlayer->m_pItemContainer->m_Bag.AddItem(vItemID);

        CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                   &(pPlayer->m_pItemContainer->m_Bag));
        pPlayer->RecvCmd(g_sendCmd);

        //pPlayer->m_pItemContainer->m_Bag.TidyUpBag();
        //通过邮件给买方物品
        //SItemBase* pItemBase = g_region->m_pItemManager->GetItem(vItemID.type);
        //if (!pItemBase)
        //{
        //	return false;
        //}
        //string strone =  pItemBase->name;//名
        //string PlayerName = cre->m_core.Name; //卖家名字
        //char cStr[512] = {0};
        //rt2_snprintf(cStr,512, R(MSG_TRADE_SELLTOOTHERSINPRICE),PlayerName.c_str(),money,strone.c_str(),amount);
        //std::string strItem = vItemID.SaveToString(g_region->m_pItemManager);
        //if (pPlayer->m_userInfo)
        //{
        //	g_region->m_gws.SendMail(pPlayer->m_userInfo->m_userId, 0, "【系统】", "摆摊买卖系统", cStr, strItem, 0);
        //}

        //设置卖家的背包
        int page, gridI, gridJ;
        if (!cre->m_pItemContainer->m_Bag.Find(rItem, &page, &gridI, &gridJ)) {
            return CMD_ERROR_NONE;
        }

        long Num = 0;
        if (amount <= rItem.count) {
            if (rItem.count == amount) {
                cre->m_pItemContainer->m_Bag.RemoveItem(rItem);
            } else {
                rItem.count -= amount;
                cre->m_pItemContainer->m_Bag.SetItem(page, gridI, gridJ, rItem);
            }
        } else {
            Num         = amount - rItem.count;
            DWORD type  = rItem.type;
            DWORD cBind = rItem.cBind;
            cre->m_pItemContainer->RemoveItem(rItem);
            //开始遍历同类型且绑定属性一致的物品   进行操作
            cre->m_pItemContainer->m_Bag.SetSell(type, cBind, Num);
        }
    } else if (pPShopBag->RemoveItem(vFakeItemID)) {
        if (cre->m_pItemContainer->RemoveItem(rItem)) {
            int page, gridI, gridJ;
            cre->m_pItemContainer->m_Bag.Find(rItem, &page, &gridI, &gridJ);
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page,
                                                          gridI, gridJ, ITEM_REASON_DELETE);
            cre->RecvCmd(g_sendCmd);

            int tmpPage, tmpI, tmpJ;
            //这里不让买方买到后直接进行堆叠
            pPlayer->m_pItemContainer->m_Bag.AddItem(rItem, &tmpPage, &tmpI, &tmpJ, false);
            CItemCmdBuilder_Svr::Build_r2c_bag_additem(rItem, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       tmpPage, tmpI, tmpJ, ITEM_REASON_NULL);
            pPlayer->RecvCmd(g_sendCmd);
        }
    }

    pPlayer->m_pItemContainer->RemoveMoney(money);
    CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(-money, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                ITEM_REASON_PICK);
    pPlayer->RecvCmd(g_sendCmd);

    if (MAX_MONEY_COUNT - (cre->m_pItemContainer->GetMoney()) >= money) {
        cre->m_pItemContainer->AddMoney(money);
    } else {
        cre->m_pItemContainer->AddMoney(0);
    }
    CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(money, ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                ITEM_REASON_PICK);
    cre->RecvCmd(g_sendCmd);

    //反馈消息
    CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(pPlayer->m_oId, 1, ITEM_REASON_NULL,
                                                     FakeItemID, ItemID, amount);
    cre->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_pshop_sell_buy(cre->m_oId, FakeItemID, ItemID, money, amount);
    pPlayer->RecvCmd(g_sendCmd);

    //刷新卖家的背包
    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(cre->m_pItemContainer->m_Bag));
    cre->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_query_pet) {
    CRegionCreature* cre = CmdGiver;

    long  NpcID;
    char* pass;
    SAFE_READ(cmd->ReadLong(&NpcID))
    SAFE_READ(cmd->ReadString(&pass))

    SAFE_ASSERT(cre->IsUser())

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc)

    if (cre->Distance(pNpc->m_pos) > 240) {
        return CMD_ERROR_NONE;
    }

    if (!cre->m_pBankPet) {
        ERR1("A user have no bank!(user=%s)\n", cre->m_core.Name.c_str());
        return CMD_ERROR_UNKNOWN;
    }

    char strMD5Password[36];
    rtMD5StringString(pass, strMD5Password);
    std::string strMD5Check;
    for (int i = 0; i < 16; i++) {
        strMD5Check.push_back(strMD5Password[i]);
    }

    if (!cre->m_userInfo->m_bankPass.empty()) {
        if (strlen(pass) == 0) {
            CItemCmdBuilder_Svr::Build_r2c_bank_pass_query(NpcID, 0);
            cre->RecvCmd(g_sendCmd);
        } else {
            if (cre->m_userInfo->m_bankPass == strMD5Check) {
                CItemCmdBuilder_Svr::Build_r2c_bank_query_response_pet(cre->m_pBankPet);
                cre->RecvCmd(g_sendCmd);
            } else {
                CItemCmdBuilder_Svr::Build_r2c_bank_pass_query(NpcID, 1);
                cre->RecvCmd(g_sendCmd);
            }
        }
    } else {
        CItemCmdBuilder_Svr::Build_r2c_bank_query_response_pet(cre->m_pBankPet);
        cre->RecvCmd(g_sendCmd);
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_query) {
    CRegionCreature* cre = CmdGiver;

    // CHECK
    // 角色必须是玩家
    long  NpcID;
    char* pass;
    SAFE_READ(cmd->ReadLong(&NpcID))
    SAFE_READ(cmd->ReadString(&pass))

    SAFE_ASSERT(cre->IsUser())

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc)

    if (cre->Distance(pNpc->m_pos) > 240) {
        return CMD_ERROR_NONE;
    }

    if (!cre->m_pBank) {
        ERR1("A user have no bank!(user=%s)\n", cre->m_core.Name.c_str());
        return CMD_ERROR_UNKNOWN;
    }

    char strMD5Password[36];
    rtMD5StringString(pass, strMD5Password);
    std::string strMD5Check;
    for (int i = 0; i < 16; i++) {
        strMD5Check.push_back(strMD5Password[i]);
    }

    if (!cre->m_userInfo->m_bankPass.empty()) {
        if (strlen(pass) == 0) {
            CItemCmdBuilder_Svr::Build_r2c_bank_pass_query(NpcID, 0);
            cre->RecvCmd(g_sendCmd);
        } else {
            if (cre->m_userInfo->m_bankPass == strMD5Check) {
                CItemCmdBuilder_Svr::Build_r2c_bank_query_response(cre->m_pBank);
                cre->RecvCmd(g_sendCmd);
            } else {
                CItemCmdBuilder_Svr::Build_r2c_bank_pass_query(NpcID, 1);
                cre->RecvCmd(g_sendCmd);
            }
        }
    } else {
        CItemCmdBuilder_Svr::Build_r2c_bank_query_response(cre->m_pBank);
        cre->RecvCmd(g_sendCmd);
    }

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_store) {
    // CHECK
    // 玩家必须有bagStore中的所有物品(RegionServer)
    // 玩家的金币必须不小于bagStore中的金币(RegionServer)
    // bagStore中的所有物品必须是可以储存的(RegionServer)
    // 钱庄必须有足够的空格(RegionServer)

    CRegionCreature* cre = CmdGiver;
    SAFE_ASSERT(cre->m_userInfo)
    CRegionItemContainer* pCon = cre->m_pItemContainer;
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    SDWORDArray idArray;
    long        money;
    SAFE_ASSERT(idArray.Serialize(cmd))
    SAFE_READ(cmd->ReadLong(&money))
    long NpcID;
    SAFE_READ(cmd->ReadLong(&NpcID))

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc)
    if (cre->Distance(pNpc->m_pos) > 300) {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0, ITEM_REASON_TOO_FAR);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    // clone check
    if (CloneCheck(idArray.array)) {
        ERR1("[cmd_c2r_bank_store] Find Clone Item(User=%s)\n", cre->m_core.Name.c_str());
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_UNKNOWN;
    }

    vector<SItemID> itemsArray;
    for (int counter = 0; counter < (int)idArray.array.size(); counter++) {
        SItemID item;
        int     page, gridI, gridJ;
        if (pCon->m_Bag.Find(idArray.array[counter], &page, &gridI, &gridJ)) {
            if (pCon->CanItemTrade(1, page, gridI, gridJ) != TR_Can) {
                CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0);
                CmdGiver->RecvCmd(g_sendCmd);
                return CMD_ERROR_NONE;
            }
            item = pCon->m_Bag.GetItem(page, gridI, gridJ);
            if (NULL == item.GetSItemBase()) {
                continue;
            }
            if (!item.GetSItemBase()->bStore) {
                CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0);
                CmdGiver->RecvCmd(g_sendCmd);
                return CMD_ERROR_NONE;
            }
            SAFE_ASSERT(ItemID_IsValid(item))
            itemsArray.push_back(item);
        } else
            return CMD_ERROR_UNKNOWN;
    }

    if (!MoneyIsValid(money)) {
        //ERR("[cmd_c2r_bank_store]金钱数量不合法.\n");
        return CMD_ERROR_UNKNOWN;
    }
    if (money > pCon->GetMoney()) {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0);
        CmdGiver->RecvCmd(g_sendCmd);

        return CMD_ERROR_UNKNOWN;
    }

    if (cre->m_pBank->FreeSpace() < (int)idArray.array.size()) {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0, ITEM_REASON_CONTAINER_FULL);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    // Deal
    pCon->RemoveMoney(money);
    cre->m_pBank->AddMoney(money);
    if (!CItemContainerBase::ExchangeItems(&(pCon->m_Bag), cre->m_pBank, idArray.array)) {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    //for (int counter=0; counter<(int)idArray.array.size(); counter++)
    //{
    //	if (pCon->m_Bag.Find(idArray.array[counter], &page, &i, &j) )
    //	{
    //		item = pCon->m_Bag.GetItem(page, i, j);
    //		SAFE_ASSERT( pCon->m_Bag.RemoveItem(page, i, j) )
    //		SAFE_ASSERT( cre->m_pBank->AddItem(item) )
    //	}
    //	else
    //		return CMD_ERROR_UNKNOWN;
    //}

    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(cre->m_pItemContainer->m_Bag));
    cre->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_bank_store_response(1);
    cre->RecvCmd(g_sendCmd);

    // log到数据库
    string strBag;
    strBag += rtFormatNumber(GetItemSerializeVersion());
    strBag += ",";
    strBag += rtFormatNumber((int)itemsArray.size());
    strBag += ",";
    for (int i = 0; i < (int)itemsArray.size(); i++) {
        strBag += itemsArray[i].SaveToString(g_region->m_pItemManager);
    }
    strBag += rtFormatNumber(money);

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_Bank);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(1);
    packet.WriteString((char*)strBag.c_str());
    packet.WriteLong(NpcID);
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_store_pet) {
    CRegionCreature* cre = CmdGiver;
    SAFE_ASSERT(cre->m_userInfo)
    CRegionItemContainer* pCon = cre->m_pItemContainer;
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    long petItemID;
    long NpcID;
    SAFE_READ(cmd->ReadLong(&petItemID))
    SAFE_READ(cmd->ReadLong(&NpcID))

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc)
    if (cre->Distance(pNpc->m_pos) > 300) {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response_pet(0, ITEM_REASON_TOO_FAR);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if (CmdGiver->m_pet && CmdGiver->m_pet->m_ai) {
        if (((CRegionPetAI*)CmdGiver->m_pet->m_ai)->m_pItemPet.id == petItemID) {
            return CMD_ERROR_NONE;
        }
    }

    SItemID item;
    int     page, gridI, gridJ;
    if (pCon->m_PetBag.Find(petItemID, &page, &gridI, &gridJ)) {
        //if (pCon->CanItemTrade(2, page, gridI, gridJ)!=TR_Can)
        //{
        //	CItemCmdBuilder_Svr::Build_r2c_bank_store_response_pet(0);
        //	CmdGiver->RecvCmd(g_sendCmd);
        //	return CMD_ERROR_NONE;
        //}
        item = pCon->m_PetBag.GetItem(page, gridI, gridJ);
        SAFE_ASSERT(ItemID_IsValid(item))
    } else
        return CMD_ERROR_UNKNOWN;

    if (cre->m_pBankPet->IsFull()) {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response_pet(0, ITEM_REASON_CONTAINER_FULL);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if (pCon->m_PetBag.RemoveItem(page, gridI, gridJ) && cre->m_pBankPet->AddItem(item)) {
        CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_PET_BAG, page, gridI,
                                                      gridJ, ITEM_REASON_THROW);
        cre->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response_pet(1);
        cre->RecvCmd(g_sendCmd);
    } else {
        CItemCmdBuilder_Svr::Build_r2c_bank_store_response_pet(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    // log到数据库
    string strBag;
    strBag += rtFormatNumber(GetItemSerializeVersion());
    strBag += ",";

    strBag += item.SaveToString(g_region->m_pItemManager);

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_Bank);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(3);
    packet.WriteString((char*)strBag.c_str());
    packet.WriteLong(NpcID);
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_withdraw_pet) {
    CRegionCreature* cre = CmdGiver;
    SAFE_ASSERT(cre->m_userInfo && cre->m_pBank)
    CItemContainerBase* pCon = cre->m_pItemContainer;
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    long petItemID;
    long NpcID;
    SAFE_READ(cmd->ReadLong(&petItemID))
    SAFE_READ(cmd->ReadLong(&NpcID))

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc)
    if (cre->Distance(pNpc->m_pos) > 240) {
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response_pet(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if (pCon->m_PetBag.IsFull()) {
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response_pet(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    int     page, gridI, gridJ;
    SItemID item;
    if (cre->m_pBankPet->Find(petItemID, &page, &gridI, &gridJ)) {
        item = cre->m_pBankPet->GetItem(page, gridI, gridJ);
        SAFE_ASSERT(ItemIsPet(item))
    } else
        return CMD_ERROR_UNKNOWN;

    if (cre->AddPetByItem(item) && cre->m_pBankPet->RemoveItem(page, gridI, gridJ) &&
        pCon->m_PetBag.Find(item.id, &page, &gridI, &gridJ)) {
        CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_PET_BAG, page,
                                                           gridI, gridJ, item);
        cre->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response_pet(1);
        cre->RecvCmd(g_sendCmd);
    } else {
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response_pet(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    cre->m_pBankPet->Pinch();

    // log到数据库
    string strBag;
    strBag += rtFormatNumber(GetItemSerializeVersion());
    strBag += ",";

    strBag += item.SaveToString(g_region->m_pItemManager);

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_Bank);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(4);
    packet.WriteString((char*)strBag.c_str());
    packet.WriteLong(NpcID);
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_withdraw) {
    CRegionCreature* cre = CmdGiver;
    SAFE_ASSERT(cre->m_userInfo && cre->m_pBank)
    CItemContainerBase* pCon = cre->m_pItemContainer;
    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    SDWORDArray idArray;
    long        money;
    SAFE_ASSERT(idArray.Serialize(cmd))
    SAFE_READ(cmd->ReadLong(&money))
    long NpcID;
    SAFE_READ(cmd->ReadLong(&NpcID))

    // CHECK
    // 银行中必须有这些道具(GWServer)
    // 取金币必须小于银行金币(GWServer)
    // 玩家道具栏必须足够(RegionServer)

    CRegionCreature* pNpc = cre->m_scene->FindCreature(NpcID);
    SAFE_ASSERT(pNpc)
    if (cre->Distance(pNpc->m_pos) > 240) {
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if (!MoneyIsValid(money)) {
        return CMD_ERROR_UNKNOWN;
    }

    if (cre->m_pBank->GetMoney() < money) {
        return CMD_ERROR_NONE;
    }

    if (pCon->m_Bag.FreeSpace() < (int)idArray.array.size()) {
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if (CloneCheck(idArray.array)) {
        ERR1("[cmd_c2r_bank_withdraw] Find Clone Item(User=%s)\n", cre->m_core.Name.c_str());
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_UNKNOWN;
    }

    vector<SItemID> itemsArray;
    for (int counter = 0; counter < (int)idArray.array.size(); counter++) {
        int     page, gridI, gridJ;
        SItemID item;
        if (cre->m_pBank->Find(idArray.array[counter], &page, &gridI, &gridJ)) {
            item = cre->m_pBank->GetItem(page, gridI, gridJ);
            SAFE_ASSERT(ItemID_IsValid(item))
            itemsArray.push_back(item);
        } else
            return CMD_ERROR_UNKNOWN;
    }

    // Deal
    cre->m_pBank->RemoveMoney(money);
    cre->m_pItemContainer->AddMoney(money);
    if (!CItemContainerBase::ExchangeItems(cre->m_pBank, cre->m_pItemContainer, idArray.array)) {
        CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response(0);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    //for (int counter=0; counter<(int)idArray.array.size(); counter++)
    //{
    //	if (cre->m_pBank->FindID(idArray.array[counter], &page, &i, &j) )
    //	{
    //		item = cre->m_pBank->GetItem(page, i, j);
    //		SAFE_ASSERT( cre->m_pBank->RemoveItem(page, i, j) )
    //		SAFE_ASSERT( cre->m_pItemContainer->AddItem(item) )
    //	}
    //	else
    //		return CMD_ERROR_UNKNOWN;
    //}
    cre->m_pBank->Pinch();

    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(cre->m_pItemContainer->m_Bag));
    cre->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response(1);
    cre->RecvCmd(g_sendCmd);

    // log到数据库
    string strBag;
    strBag += rtFormatNumber(GetItemSerializeVersion());
    strBag += ",";
    strBag += rtFormatNumber((int)itemsArray.size());
    strBag += ",";
    for (int i = 0; i < (int)itemsArray.size(); i++) {
        strBag += itemsArray[i].SaveToString(g_region->m_pItemManager);
    }
    strBag += rtFormatNumber(money);

    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_log_event);
    packet.WriteByte(PLT_Bank);
    packet.WriteLong(cre->m_userInfo->m_userId);
    packet.WriteString((char*)cre->m_userInfo->m_userIpAddress.c_str());  // userIP
    packet.WriteLong(2);
    packet.WriteString((char*)strBag.c_str());
    packet.WriteLong(NpcID);
    g_region->m_gws.EndSend();

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_addpass) {
    char* pass;
    SAFE_READ(cmd->ReadString(&pass));

    if (!CmdGiver->m_userInfo->m_bankPass.empty()) {
        CItemCmdBuilder_Svr::Build_r2c_bank_addpass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if ((strlen(pass) < BANKPASS_MIN_COUNT) || (strlen(pass) > BANKPASS_MAX_COUNT)) {
        CItemCmdBuilder_Svr::Build_r2c_bank_addpass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (!g_region->UserUploadString_IsValid(pass)) {
        CItemCmdBuilder_Svr::Build_r2c_bank_addpass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    char strMD5Password[36];
    rtMD5StringString(pass, strMD5Password);
    std::string strMD5DB;
    for (int i = 0; i < 16; i++) {
        strMD5DB.push_back(strMD5Password[i]);
    }

    CItemCmdBuilder_Svr::NetSend_r2g_bankpass(CmdGiver->m_userInfo->m_userId, 1, strMD5DB.c_str());

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_removepass) {
    char* pass;
    SAFE_READ(cmd->ReadString(&pass));

    char strMD5Password[36];
    rtMD5StringString(pass, strMD5Password);
    std::string strMD5Check;
    for (int i = 0; i < 16; i++) {
        strMD5Check.push_back(strMD5Password[i]);
    }

    if (CmdGiver->m_userInfo->m_bankPass != strMD5Check) {
        CItemCmdBuilder_Svr::Build_r2c_bank_removepass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    CItemCmdBuilder_Svr::NetSend_r2g_bankpass(CmdGiver->m_userInfo->m_userId, 2, "");

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_bank_changepass) {
    char* OldPass;
    char* NewPass;
    SAFE_READ(cmd->ReadString(&OldPass));
    SAFE_READ(cmd->ReadString(&NewPass));

    char strMD5Password[36];
    rtMD5StringString(OldPass, strMD5Password);
    std::string strMD5Check;
    for (int i = 0; i < 16; i++) {
        strMD5Check.push_back(strMD5Password[i]);
    }

    if (CmdGiver->m_userInfo->m_bankPass != strMD5Check) {
        CItemCmdBuilder_Svr::Build_r2c_bank_changepass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    if ((strlen(NewPass) < BANKPASS_MIN_COUNT) || (strlen(NewPass) > BANKPASS_MAX_COUNT)) {
        CItemCmdBuilder_Svr::Build_r2c_bank_changepass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    if (!g_region->UserUploadString_IsValid(NewPass)) {
        CItemCmdBuilder_Svr::Build_r2c_bank_changepass_result(1);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    char strMD5PasswordNew[36];
    rtMD5StringString(NewPass, strMD5PasswordNew);
    std::string strMD5New;
    for (int i = 0; i < 16; i++) {
        strMD5New.push_back(strMD5PasswordNew[i]);
    }

    CItemCmdBuilder_Svr::NetSend_r2g_bankpass(CmdGiver->m_userInfo->m_userId, 3, strMD5New.c_str());

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_compose_item) {
    long ComposeID;
    SAFE_READ(cmd->ReadLong(&ComposeID));

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    CCreatureState* pState = CmdGiver->FindStateByType(CRE_STATE_COMPOSE_ITEM);
    if (pState) {
        pState->End();
        return CMD_ERROR_NONE;
    }

    // 判断道具栏是否满了（因为制造出来的道具只有一个，所以只要不满就能保证一定可以添加成品）
    if (CmdGiver->m_pItemContainer->m_Bag.IsFull()) {
        CItemCmdBuilder_Svr::Build_r2c_compose_item_begin(ComposeID, itemmsg_compose_bagfull);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }

    SCreStateParam params;
    params.p1 = ComposeID;
    pState    = RT_NEW CRegionItemCompose;
    if (!CmdGiver->AddState(pState, &params)) {
        DEL_ONE(pState);

        CItemCmdBuilder_Svr::Build_r2c_compose_item_begin(ComposeID, itemmsg_unknown_error);
        CmdGiver->RecvCmd(g_sendCmd);

        return CMD_ERROR_NONE;
    }

    CItemCmdBuilder_Svr::Build_r2c_compose_item_begin(ComposeID, itemmsg_success);
    CmdGiver->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_compose_item_stop) {
    long ComposeID;
    SAFE_READ(cmd->ReadLong(&ComposeID));

    CCreatureState* pState = CmdGiver->FindStateByType(CRE_STATE_COMPOSE_ITEM);
    if (pState) {
        pState->End();
    }

    CItemCmdBuilder_Svr::Build_r2c_compose_item_end(ComposeID, itemmsg_unknown_error);
    CmdGiver->RecvCmd(g_sendCmd);

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_decompose_item) {
    long itemID;
    SAFE_READ(cmd->ReadLong(&itemID));

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    // 道具资料
    int i = 0, j = 0;
    int page, gridI, gridJ;
    if (!CmdGiver->m_pItemContainer->m_Bag.Find(itemID, &page, &gridI, &gridJ))
        return CMD_ERROR_NONE;
    SItemID item = CmdGiver->m_pItemContainer->m_Bag.GetItem(page, gridI, gridJ);
    SAFE_ASSERT(item.color != ItemColor_White);
    SAFE_ASSERT(item.cLock == 0);
    // 取得分解结构
    SItemBase* pItemClass = g_region->m_pItemManager->GetItem(item.type);
    SAFE_ASSERT(pItemClass);
    SAFE_ASSERT(pItemClass->decomposeID != 0);
    SItemDecompose* pDecomposeClass =
        g_region->m_pItemManager->m_pDecomposeMgr->FindDecomposing(pItemClass->decomposeID);
    SAFE_ASSERT(pDecomposeClass);
    // 检查背包空位
    int  nFreeSpace = CmdGiver->m_pItemContainer->m_Bag.FreeSpace();
    int  RandomCount[ITEMDECOMPOSE_MAX_MATERIAL];
    CBag bagTmp(ITEM_PLAYER_INVENTORY_LINE, ITEM_PLAYER_INVENTORY_COLUMN,
                ITEM_PLAYER_INVENTORY_PAGE, g_region->m_pItemManager);
    for (i = 0; i < (int)pDecomposeClass->MaterialTypeCount; i++) {
        static float RangeMax = 1.2f;
        static float RangeMin = 0.8f;

        SItemID NewItem;
        float   fRange = GetRand(RangeMax * 100, RangeMin * 100) / 100.0f;
        if (item.color == ItemColor_Green) {
            RandomCount[i] = rtRound(pDecomposeClass->MaterialNums_Green[i] * fRange);
        } else if (item.color == ItemColor_Blue) {
            RandomCount[i] = rtRound(pDecomposeClass->MaterialNums_Blue[i] * fRange);
        } else if (item.color == ItemColor_Golden) {
            RandomCount[i] = rtRound(pDecomposeClass->MaterialNums_Golden[i] * fRange);
        } else {
            return CMD_ERROR_NONE;
        }

        for (j = 0; j < RandomCount[i]; j++) {
            NewItem = g_region->m_pItemManager->CreateItem(pDecomposeClass->Materials[i]);
            SAFE_ASSERT(bagTmp.AddItem(NewItem));
        }
    }
    int nGrids = bagTmp.Count();
    if (nFreeSpace + 1 < nGrids) {
        CItemCmdBuilder_Svr::Build_r2c_decompose_item_result(itemID, itemmsg_decompose_bagfull);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    // 分解
    SAFE_ASSERT(CmdGiver->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ));
    CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI,
                                                  gridJ, 1);
    CmdGiver->RecvCmd(g_sendCmd);

    for (i = 0; i < (int)pDecomposeClass->MaterialTypeCount; i++) {
        SItemID NewItem;
        for (j = 0; j < RandomCount[i]; j++) {
            NewItem = g_region->m_pItemManager->CreateItem(pDecomposeClass->Materials[i]);
            CmdGiver->AddItem(NewItem);
        }
    }
    // 通知Client
    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &(CmdGiver->m_pItemContainer->m_Bag));
    CmdGiver->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_decompose_item_result(itemID, itemmsg_success);
    CmdGiver->RecvCmd(g_sendCmd);
    // Log To DB
    std::string strLogItem;
    strLogItem += rtFormatNumber(GetItemSerializeVersion());
    strLogItem += ",";
    strLogItem += item.SaveToString(g_region->m_pItemManager);
    g_region->SendUserLogEvent(PLT_DecomposeItem, CmdGiver->m_userInfo->m_userId, "", 0,
                               strLogItem.c_str(), 0, "", 0, "");

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_enchant_item) {
    long equipItemID;
    long scrollItemID;
    SAFE_READ(cmd->ReadLong(&equipItemID));
    SAFE_READ(cmd->ReadLong(&scrollItemID));

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    int EquipmentPosPage, EquipmentPosI, EquipmentPosJ;
    int ScrollPosPage, ScrollPosI, ScrollPosJ;
    SAFE_ASSERT(CmdGiver->m_pItemContainer->m_Bag.Find(equipItemID, &EquipmentPosPage,
                                                       &EquipmentPosI, &EquipmentPosJ));
    SAFE_ASSERT(CmdGiver->m_pItemContainer->m_Bag.Find(scrollItemID, &ScrollPosPage, &ScrollPosI,
                                                       &ScrollPosJ));
    SItemID EquipmentItem =
        CmdGiver->m_pItemContainer->m_Bag.GetItem(EquipmentPosPage, EquipmentPosI, EquipmentPosJ);
    SItemID OrigEquipmentItem = EquipmentItem;
    SItemID ScrollItem =
        CmdGiver->m_pItemContainer->m_Bag.GetItem(ScrollPosPage, ScrollPosI, ScrollPosJ);
    SItemBase* pItemClassEquipment = g_region->m_pItemManager->GetItem(EquipmentItem.type);
    SItemBase* pItemClassScroll    = g_region->m_pItemManager->GetItem(ScrollItem.type);
    SAFE_ASSERT(pItemClassEquipment && pItemClassScroll);
    SAFE_ASSERT(ItemIsWeapon(EquipmentItem) || ItemIsArmor(EquipmentItem) ||
                ItemIsShipin(EquipmentItem));
    SAFE_ASSERT(ItemIsScrollEnchant(ScrollItem.type));
    SAFE_ASSERT(pItemClassEquipment->requirement[ItemRequirement_Level] >=
                pItemClassScroll->requirement[ItemRequirement_Level]);

    bool bSuccess1 = true;

    if (CItemManager::GetIndexFromType(ScrollItem.type) == REBUILD_ITEM ||
        CItemManager::GetIndexFromType(ScrollItem.type) == REBUILD_ITEM_BIND ||
        CItemManager::GetIndexFromType(ScrollItem.type) == REBUILD_ITEM_SET_BIND) {
        if (EquipmentItem.color == ItemColor_Green) {
            EquipmentItem.attribRate1 =
                g_region->m_pItemManager->m_pAttribMgr->CreateAttribRate(CAttribMgr::csGreen);
        } else if (EquipmentItem.color == ItemColor_Blue) {
            EquipmentItem.attribRate1 =
                g_region->m_pItemManager->m_pAttribMgr->CreateAttribRate(CAttribMgr::csBlue1);
            EquipmentItem.attribRate2 =
                g_region->m_pItemManager->m_pAttribMgr->CreateAttribRate(CAttribMgr::csBlue2);
        } else if (EquipmentItem.color == ItemColor_Golden) {
            EquipmentItem.attribRate1 =
                g_region->m_pItemManager->m_pAttribMgr->CreateAttribRate(CAttribMgr::csGolden1);
            EquipmentItem.attribRate2 =
                g_region->m_pItemManager->m_pAttribMgr->CreateAttribRate(CAttribMgr::csGolden2);
        } else {
            return CMD_ERROR_NONE;
        }
    } else if (CItemManager::GetIndexFromType(ScrollItem.type) == DOBIND_ITEM) {
        if (EquipmentItem.cBind != IB_NotBind)
            return CMD_ERROR_NONE;
        EquipmentItem.cBind = IB_Binded;
    } else if (CItemManager::GetIndexFromType(ScrollItem.type) == DISBINDE_ITEM) {
        if (EquipmentItem.cBind != IB_Binded)
            return CMD_ERROR_NONE;
        EquipmentItem.cBind = IB_BindWhenEquip;
    } else if (CItemManager::GetIndexFromType(ScrollItem.type) == SUPERPLUS10_ITEM) {
        if (EquipmentItem.level != 9)
            return CMD_ERROR_NONE;
        if (GetProb(0.05))
            EquipmentItem.level = 10;
        else {
            EquipmentItem.level = 8;
            bSuccess1           = false;
        }
    } else if (CItemManager::GetIndexFromType(ScrollItem.type) == PLUS10_ITEM) {
        if (EquipmentItem.level != 9)
            return CMD_ERROR_NONE;
        if (GetProb(0.05))
            EquipmentItem.level = 10;
        else {
            EquipmentItem.level = 0;
            bSuccess1           = false;
        }
    } else if (CItemManager::GetIndexFromType(ScrollItem.type) <= ELE_REFINE_ITEM_MAX &&
               CItemManager::GetIndexFromType(ScrollItem.type) >= ELE_REFINE_ITEM_MIN) {
        if (!ItemIsWeapon_MagicWeapon(EquipmentItem.type))
            return CMD_ERROR_NONE;
        float rate = 1;
        int   limit =
            EquipmentItem.params[MW_PARAM_LEV] * EquipmentItem.params[MW_PARAM_LEV] / 25 * 10;
        int eletype;

        int ele[MAX_ELEMENT_ATTR];
        MW_GetEleValue(EquipmentItem, ele);

        eletype = pItemClassScroll->enchantAttribID;

        if (ele[eletype] < limit * 0.4) {
            rate = 1;
        } else if (ele[eletype] < limit * 0.6) {
            rate = 0.55;
        } else if (ele[eletype] < limit * 0.8) {
            rate = 0.175;
        } else if (ele[eletype] < limit * 0.9) {
            rate = 0.05;
        } else {
            return CMD_ERROR_NONE;
        }

        if (GetProb(rate)) {
            ele[eletype] += limit * 0.1;
        } else {
            ele[eletype] = 0;
            bSuccess1    = false;
        }

        MW_SetEleValue(EquipmentItem, ele);
    } else if (CItemManager::GetIndexFromType(ScrollItem.type) <= EQUIP_LEVELUP_ITEM_MAX &&
               CItemManager::GetIndexFromType(ScrollItem.type) >= EQUIP_LEVELUP_ITEM_MIN) {
        int originId = pItemClassScroll->enchantAttribID % 10000;
        int destId   = pItemClassScroll->enchantAttribID / 10000;
        //int rate = 1;
        //int wanbi = 1;

        //这里需要加个保护

        if (CItemManager::GetIndexFromType(EquipmentItem.type) != originId) {
            return CMD_ERROR_NONE;
        }
        if (EquipmentItem.level < 5) {
            return CMD_ERROR_NONE;
        };

        double rate = 0.1;
        // 		if(CmdGiver->GetItemNum(2222) > 0)
        // 		{
        // 			CmdGiver->RemoveItem(2222,1);
        // 			rate = 0.2;
        // 		}

        if (GetProb(rate)) {  //成功，装备升级成DestId，Attrib继承，rate浮动范围(-15~5)，附魔取消
            if (EquipmentItem.level < 7)
                EquipmentItem.level = 0;
            else
                EquipmentItem.level--;

            SItemBase* pItemBase = g_region->m_pItemManager->GetItemFromIndex(destId);
            EquipmentItem.type   = pItemBase->type;

        } else {  //失败，装备等级不变
            bSuccess1 = false;
            if (EquipmentItem.level < 7)
                EquipmentItem.level = 0;
        }

        //根据原始Level,判断升级或者降级，并检测完毕娃娃

        // 		switch (EquipmentItem.level)
        // 		{
        // 		case 5:{ wanbi = 1;break;}
        // 		case 6:{ wanbi = 2;break;}
        // 		case 7:{ wanbi = 10;break;}
        // 		case 8:{ wanbi = 30;break;}
        // 		case 9:{ wanbi = 90;break;}
        // 		case 10:{ wanbi = 200;break;}
        // 		}
        //
        // 		if (CmdGiver->GetItemNum(REFINEGOAT_ITEM) >= wanbi)
        // 		{
        // 			CmdGiver->RemoveItem(REFINEGOAT_ITEM, wanbi);
        // 			CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_FAIL1));
        // 		}else if (CmdGiver->GetItemNum(REFINEGOAT_ITEM_SET) >= wanbi)
        // 		{
        // 			CmdGiver->RemoveItem(REFINEGOAT_ITEM_SET,wanbi);
        // 			CmdGiver->SendSystemMessage(R(MSG_ITEMUPDATE_FAIL1));
        // 		}else
        // 		{
        // 			EquipmentItem.level = 0;
        // 		}
    } else {
        WORD EnchantAttribID        = (WORD)pItemClassScroll->enchantAttribID;
        EquipmentItem.enchantAttrib = EnchantAttribID;
    }
    CmdGiver->m_pItemContainer->m_Bag.SetItem(EquipmentPosPage, EquipmentPosI, EquipmentPosJ,
                                              EquipmentItem);

    // 删除卷轴
    if (ItemCanPile(ScrollItem.type)) {
        if (!CmdGiver->m_pItemContainer->UsePileItem(ScrollPosPage, ScrollPosI, ScrollPosJ))
            return CMD_ERROR_NONE;
        CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(ScrollItem.id, ScrollPosPage, ScrollPosI,
                                                          ScrollPosJ);
        CmdGiver->RecvCmd(g_sendCmd);
    } else {
        CmdGiver->m_pItemContainer->m_Bag.RemoveItem(ScrollPosPage, ScrollPosI, ScrollPosJ);
        CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                      ScrollPosPage, ScrollPosI, ScrollPosJ,
                                                      ITEM_REASON_NULL);
        CmdGiver->RecvCmd(g_sendCmd);
    }
    // 通知Client
    CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                       EquipmentPosPage, EquipmentPosI,
                                                       EquipmentPosJ, EquipmentItem);
    CmdGiver->RecvCmd(g_sendCmd);

    if (bSuccess1) {
        CItemCmdBuilder_Svr::Build_r2c_enchant_item_result(EquipmentItem.id, ScrollItem.id,
                                                           itemmsg_success);
        CmdGiver->RecvCmd(g_sendCmd);
    } else {
        CItemCmdBuilder_Svr::Build_r2c_enchant_item_result(EquipmentItem.id, ScrollItem.id,
                                                           itemmsg_unknown_error);
        CmdGiver->RecvCmd(g_sendCmd);
    }

    // Log To DB
    std::string strLogItemOrig, strLogItemScroll, strLogItemNew;
    strLogItemOrig += rtFormatNumber(GetItemSerializeVersion());
    strLogItemOrig += ",";
    strLogItemOrig += OrigEquipmentItem.SaveToString(g_region->m_pItemManager);
    strLogItemScroll += rtFormatNumber(GetItemSerializeVersion());
    strLogItemScroll += ",";
    strLogItemScroll += ScrollItem.SaveToString(g_region->m_pItemManager);
    strLogItemNew += rtFormatNumber(GetItemSerializeVersion());
    strLogItemNew += ",";
    strLogItemNew += EquipmentItem.SaveToString(g_region->m_pItemManager);
    g_region->SendUserLogEvent(PLT_FuseItem, CmdGiver->m_userInfo->m_userId, "", 0,
                               strLogItemOrig.c_str(), 0, strLogItemScroll.c_str(), 0,
                               strLogItemNew.c_str());

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_fuse_gem) {
    long origItem1;
    //long origItem2;
    //long origItem3;
    //long origItem4;
    //long origItem5;
    SAFE_READ(cmd->ReadLong(&origItem1));
    //SAFE_READ(cmd->ReadLong(&origItem2));
    //SAFE_READ(cmd->ReadLong(&origItem3));
    //SAFE_READ(cmd->ReadLong(&origItem4));
    //SAFE_READ(cmd->ReadLong(&origItem5));

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    // 做检查
    int page1, gridI1, gridJ1;
    //int page2, gridI2, gridJ2;
    //int page3, gridI3, gridJ3;
    //int page4, gridI4, gridJ4;
    //int page5, gridI5, gridJ5;
    CBag* pBag = &(CmdGiver->m_pItemContainer->m_Bag);

    //SAFE_ASSERT(pBag->Find(origItem1, &page1, &gridI1, &gridJ1));
    //SAFE_ASSERT(pBag->Find(origItem2, &page2, &gridI2, &gridJ2));
    //SAFE_ASSERT(pBag->Find(origItem3, &page1, &gridI1, &gridJ1));
    //SAFE_ASSERT(pBag->Find(origItem4, &page2, &gridI2, &gridJ2));
    //SAFE_ASSERT(pBag->Find(origItem5, &page2, &gridI2, &gridJ2));

    //SItemID item1 = pBag->GetItem(page1, gridI1, gridJ1);
    //SItemID item2 = pBag->GetItem(page2, gridI2, gridJ2);
    //SItemID item3 = pBag->GetItem(page3, gridI3, gridJ3);
    //SItemID item4 = pBag->GetItem(page4, gridI4, gridJ4);
    //SItemID item5 = pBag->GetItem(page5, gridI5, gridJ5);

    SItemID item1 = pBag->GetItemFromID(origItem1);

    if (!ItemID_IsValid(item1) || !ItemIsGem(item1))
        return CMD_ERROR_NONE;

    int nHaveCount = 0;
    pBag->FindType(CItemManager::GetIndexFromType(item1.type), ItemColor_All, &nHaveCount);

    if (nHaveCount < 5)
        return CMD_ERROR_NONE;

    //SAFE_ASSERT( (item1.id != item2.id != item3.id != item4.id != item5.id ) &&
    //			 (item1.type == item2.type == item3.type == item4.type == item5.type) );

    SItemBase* pItemClass = g_region->m_pItemManager->GetItem(item1.type);
    SAFE_ASSERT(pItemClass);

    //if (!ItemIsGem(item1)/* || !ItemIsGem(item2) || !ItemIsGem(item3) || !ItemIsGem(item4) || !ItemIsGem(item5)*/)
    //{
    //	CmdGiver->SendSystemMessage("此类道具不可以合成");
    //	return CMD_ERROR_NONE;
    //}

    SGem* pCharm     = (SGem*)pItemClass;
    int   nNeetMoney = pCharm->lFuseMoney;
    if (!CmdGiver->RemoveGold(nNeetMoney)) {
        CmdGiver->SendSystemMessage("钱不够，操作失败");
        return CMD_ERROR_NONE;
    }

    //EItemBind BindType = IB_NotBind;
    //if (item1.cBind==IB_Binded || item2.cBind==IB_Binded || item3.cBind==IB_Binded || item4.cBind==IB_Binded || item5.cBind==IB_Binded)
    //	BindType = IB_Binded;
    //else if (item1.cBind==IB_BindWhenPick || item2.cBind==IB_BindWhenPick || item3.cBind==IB_BindWhenPick || item4.cBind==IB_BindWhenPick || item5.cBind==IB_BindWhenPick)
    //	BindType = IB_BindWhenPick;
    //else if (item1.cBind==IB_BindWhenEquip || item2.cBind==IB_BindWhenEquip || item3.cBind==IB_BindWhenEquip || item4.cBind==IB_BindWhenEquip || item5.cBind==IB_BindWhenEquip)
    //	BindType = IB_BindWhenEquip;
    //else
    //	BindType = IB_NotBind;

    SItemID NewItem = g_region->m_pItemManager->CreateItem(pCharm->lFuse, (EItemColor)item1.color,
                                                           0 /*, BindType*/);

    float fRate = (float)pCharm->cFuseRate / 100.f;

    if (!ItemID_IsValid(NewItem)) {
        return CMD_ERROR_NONE;
    }
    // 人物背包操作
    //if (pBag->RemoveItem(page1, gridI1, gridJ1))
    //{
    //	if (pBag->RemoveItem(page2, gridI2, gridJ2))
    //	{
    //		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page1, gridI1, gridJ1, ITEM_REASON_NULL);
    //		CmdGiver->RecvCmd(g_sendCmd);
    //		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page2, gridI2, gridJ2, ITEM_REASON_NULL);
    //		CmdGiver->RecvCmd(g_sendCmd);
    //		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page3, gridI3, gridJ3, ITEM_REASON_NULL);
    //		CmdGiver->RecvCmd(g_sendCmd);
    //		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page4, gridI4, gridJ4, ITEM_REASON_NULL);
    //		CmdGiver->RecvCmd(g_sendCmd);
    //		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page5, gridI5, gridJ5, ITEM_REASON_NULL);
    //		CmdGiver->RecvCmd(g_sendCmd);

    //		CmdGiver->AddItem(NewItem);
    //	}
    //}

    DWORD ItemIdx     = CItemManager::GetIndexFromType(item1.type);
    DWORD ItemNeedNum = 5;
    //修正宝石合成成功时客户端崩溃。
    //原因：判定成功时，删除物品后未更新客户端背包，导致新物品添加客户端判定错误。
    //修改：在删除物品和成功鉴定通过后，刷新客户端背包，之后再添加物品。
    //修改人：李泽华
    //修改日期：2010年6月26日
    //Begin
    if (!pBag->RemoveItem(ItemIdx, ItemColor_All, ItemNeedNum) ||
        !GetProb(fRate) /*|| !CmdGiver->AddItem(NewItem)*/) {
        CItemCmdBuilder_Svr::Build_r2c_fuse_gem_result(itemmsg_unknown_error, 0);
        CmdGiver->RecvCmd(g_sendCmd);
        CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                   &CmdGiver->m_pItemContainer->m_Bag);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    } else {
        CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                                   &CmdGiver->m_pItemContainer->m_Bag);
        CmdGiver->RecvCmd(g_sendCmd);
    }
    if (!CmdGiver->AddItem(NewItem)) {
        CItemCmdBuilder_Svr::Build_r2c_fuse_gem_result(itemmsg_unknown_error, 0);
        CmdGiver->RecvCmd(g_sendCmd);
        //CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &CmdGiver->m_pItemContainer->m_Bag);
        //CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    //End
    CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG,
                                               &CmdGiver->m_pItemContainer->m_Bag);
    CmdGiver->RecvCmd(g_sendCmd);
    CItemCmdBuilder_Svr::Build_r2c_fuse_gem_result(itemmsg_success, NewItem.id);
    CmdGiver->RecvCmd(g_sendCmd);

    // Log To DB
    std::string strLogItem1, strLogItem2, strLogItem3;
    strLogItem1 += rtFormatNumber(GetItemSerializeVersion());
    strLogItem1 += ",";
    strLogItem1 += item1.SaveToString(g_region->m_pItemManager);
    strLogItem2 += rtFormatNumber(GetItemSerializeVersion());
    strLogItem2 += ",";
    strLogItem2 += item1.SaveToString(g_region->m_pItemManager);
    strLogItem3 += rtFormatNumber(GetItemSerializeVersion());
    strLogItem3 += ",";
    strLogItem3 += NewItem.SaveToString(g_region->m_pItemManager);
    g_region->SendUserLogEvent(PLT_FuseGem, CmdGiver->m_userInfo->m_userId, "", 0,
                               strLogItem1.c_str(), 0, strLogItem2.c_str(), 0, strLogItem3.c_str());

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_fuse_item) {
    long origItem1;
    long origItem2;
    SAFE_READ(cmd->ReadLong(&origItem1));
    SAFE_READ(cmd->ReadLong(&origItem2));

    if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
        return CMD_ERROR_NONE;

    // 做检查
    int   page1, gridI1, gridJ1;
    int   page2, gridI2, gridJ2;
    CBag* pBag = &(CmdGiver->m_pItemContainer->m_Bag);
    SAFE_ASSERT(pBag->Find(origItem1, &page1, &gridI1, &gridJ1));
    SAFE_ASSERT(pBag->Find(origItem2, &page2, &gridI2, &gridJ2));
    SItemID item1 = pBag->GetItem(page1, gridI1, gridJ1);
    SItemID item2 = pBag->GetItem(page2, gridI2, gridJ2);
    SAFE_ASSERT((item1.id != item2.id) && (item1.color != ItemColor_White) &&
                (item1.color == item2.color) && (item1.type == item2.type));
    SItemBase* pItemClass = g_region->m_pItemManager->GetItem(item1.type);
    SAFE_ASSERT(pItemClass);
    if (!pItemClass->bCanFuse) {
        CmdGiver->SendSystemMessage("此类道具不可以融合");
        return CMD_ERROR_NONE;
    }

    if (!CmdGiver->RemoveGold(int((float)pItemClass->requirement[ItemRequirement_Level] / 10.f *
                                  (float)item1.color * 200.f))) {
        CmdGiver->SendSystemMessage("钱不够，操作失败");
        return CMD_ERROR_NONE;
    }

    /*int nNeetMoney = (pItemClass->requirement[ItemRequirement_Level] / 10) * item1.color * 200;
	if (CmdGiver->m_pItemContainer->GetMoney() < nNeetMoney)
	{
		CmdGiver->SendSystemMessage("钱不够，操作失败");
		return CMD_ERROR_NONE;
	}
	CmdGiver->m_pItemContainer->RemoveGold(nNeetMoney);*/
    SSkill* pSkill = CmdGiver->m_Skill.FindSkillBySubID(ITEMFUSE_SKILL);
    //SAFE_ASSERT(pSkill && (pItemClass->requirement[ItemRequirement_Level] <= ItemDecompose_MaxItemLevel(pSkill->iLevel)) );
    // 融合
    EItemBind BindType = IB_NotBind;
    if (item1.cBind == IB_Binded || item2.cBind == IB_Binded)
        BindType = IB_Binded;
    else if (item1.cBind == IB_BindWhenPick || item2.cBind == IB_BindWhenPick)
        BindType = IB_BindWhenPick;
    else if (item1.cBind == IB_BindWhenEquip || item2.cBind == IB_BindWhenEquip)
        BindType = IB_BindWhenEquip;
    else
        BindType = IB_NotBind;
    SItemID NewItem =
        g_region->m_pItemManager->CreateItem(g_region->m_pItemManager->GetIndexFromType(item1.type),
                                             (EItemColor)item1.color, 0, BindType);
    if (!ItemID_IsValid(NewItem)) {
        CItemCmdBuilder_Svr::Build_r2c_fuse_item_result(itemmsg_unknown_error, origItem1,
                                                        origItem2);
        CmdGiver->RecvCmd(g_sendCmd);
        return CMD_ERROR_NONE;
    }
    // 人物背包操作
    if (pBag->RemoveItem(page1, gridI1, gridJ1)) {
        if (pBag->RemoveItem(page2, gridI2, gridJ2)) {
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page1,
                                                          gridI1, gridJ1, ITEM_REASON_NULL);
            CmdGiver->RecvCmd(g_sendCmd);
            CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page2,
                                                          gridI2, gridJ2, ITEM_REASON_NULL);
            CmdGiver->RecvCmd(g_sendCmd);

            CmdGiver->AddItem(NewItem);
        }
    }
    CItemCmdBuilder_Svr::Build_r2c_fuse_item_result(itemmsg_success, origItem1, origItem2);
    CmdGiver->RecvCmd(g_sendCmd);
    // 技能成长
    CSkillInstance* pSkillInst = CmdGiver->m_Skill.FindActiveSkillBySubID(ITEMFUSE_SKILL);
    if (pSkillInst)
        pSkillInst->OnUseAfter(&(CmdGiver->m_Skill));
    // Log To DB
    std::string strLogItem1, strLogItem2, strLogItem3;
    strLogItem1 += rtFormatNumber(GetItemSerializeVersion());
    strLogItem1 += ",";
    strLogItem1 += item1.SaveToString(g_region->m_pItemManager);
    strLogItem2 += rtFormatNumber(GetItemSerializeVersion());
    strLogItem2 += ",";
    strLogItem2 += item2.SaveToString(g_region->m_pItemManager);
    strLogItem3 += rtFormatNumber(GetItemSerializeVersion());
    strLogItem3 += ",";
    strLogItem3 += NewItem.SaveToString(g_region->m_pItemManager);
    g_region->SendUserLogEvent(PLT_FuseItem, CmdGiver->m_userInfo->m_userId, "", 0,
                               strLogItem1.c_str(), 0, strLogItem2.c_str(), 0, strLogItem3.c_str());

    return CMD_ERROR_NONE;
}

ITEM_CMD_FUNCTION(cmd_c2r_check_item) {
    CRegionCreature* cre = CmdGiver;

    CItemCmdBuilder_Svr::Build_r2c_check_item(cre->m_pItemContainer);
    cre->RecvCmd(g_sendCmd);
    return CMD_ERROR_NONE;
}

void CItemCmdBuilder_Svr::Build_r2c_item_errorcode(short msg) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_item_errorcode);
    g_sendCmd->WriteShort(msg);
}

void CItemCmdBuilder_Svr::Build_r2c_check_item(CItemContainerBase* pCon) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_check_item);
    pCon->Serialize(0, g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_bag_additem(SItemID& item, EItemContainer container, int page,
                                                int gridI, int gridJ, int reason) {
    CG_CmdPacket* pPacket = g_sendCmd;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bag_additem);
    item.Serialize(g_sendCmd);
    g_sendCmd->WriteByte((char)container);
    g_sendCmd->WriteByte((char)page);
    g_sendCmd->WriteByte((char)gridI);
    g_sendCmd->WriteByte((char)gridJ);
    g_sendCmd->WriteByte((char)reason);
}

void CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(EItemContainer container, int page, int gridI,
                                                   int gridJ, int reason) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bag_removeitem);
    g_sendCmd->WriteByte((char)container);
    g_sendCmd->WriteByte((char)page);
    g_sendCmd->WriteByte((char)gridI);
    g_sendCmd->WriteByte((char)gridJ);
    g_sendCmd->WriteByte((char)reason);
}

void CItemCmdBuilder_Svr::Build_r2c_player_move_item(EItemContainer container, int pageFrom,
                                                     int iFrom, int jFrom, int pageTo, int iTo,
                                                     int jTo) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_move_item);
    g_sendCmd->WriteByte((char)container);
    g_sendCmd->WriteByte((char)pageFrom);
    g_sendCmd->WriteByte((char)iFrom);
    g_sendCmd->WriteByte((char)jFrom);
    g_sendCmd->WriteByte((char)pageTo);
    g_sendCmd->WriteByte((char)iTo);
    g_sendCmd->WriteByte((char)jTo);
}

void CItemCmdBuilder_Svr::Build_r2c_player_dispart_item(EItemContainer container, int pageFrom,
                                                        int iFrom, int jFrom, int pageTo, int iTo,
                                                        int jTo, int num) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_dispart_item);
    g_sendCmd->WriteByte((char)container);
    g_sendCmd->WriteByte((char)pageFrom);
    g_sendCmd->WriteByte((char)iFrom);
    g_sendCmd->WriteByte((char)jFrom);
    g_sendCmd->WriteByte((char)pageTo);
    g_sendCmd->WriteByte((char)iTo);
    g_sendCmd->WriteByte((char)jTo);
    g_sendCmd->WriteLong((long)num);
}

void CItemCmdBuilder_Svr::Build_r2c_bag_refresh(EItemContainer container, CBag* pBag) {
    if (!pBag)
        return;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bag_refresh);
    g_sendCmd->WriteByte(container);
    pBag->Serialize(0, g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(long money, EItemContainer container, int reason) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bag_addmoney);
    g_sendCmd->WriteLong(money);
    g_sendCmd->WriteByte(container);
    g_sendCmd->WriteByte((char)reason);
}

void CItemCmdBuilder_Svr::Build_r2c_player_usepileitem(DWORD itemID, int page, int gridI,
                                                       int gridJ) {
    CG_CmdPacket* pPacket = g_sendCmd;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_usepileitem);
    g_sendCmd->WriteLong(itemID);
    g_sendCmd->WriteByte(page);
    g_sendCmd->WriteByte(gridI);
    g_sendCmd->WriteByte(gridJ);
}

void CItemCmdBuilder_Svr::Build_r2c_refresh_single_item(EItemContainer container, int page,
                                                        int gridI, int gridJ, SItemID& item) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_refresh_single_item);
    g_sendCmd->WriteByte(container);
    g_sendCmd->WriteByte(page);
    g_sendCmd->WriteByte(gridI);
    g_sendCmd->WriteByte(gridJ);
    item.Serialize(g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_refresh_equip_item(CItemContainerBase::EEquipParts part,
                                                       SItemID&                        item) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_refresh_equip_item);
    g_sendCmd->WriteByte(part);
    item.Serialize(g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_exchange_hand_weapon() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_exchange_hand_weapon);
}

void CItemCmdBuilder_Svr::Build_r2c_pickitem_fail() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pickitem_fail);
}

void CItemCmdBuilder_Svr::Build_r2c_equip_item_state(CItemContainerBase::EEquipParts part,
                                                     char                            bValid) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_equip_item_state);
    g_sendCmd->WriteByte((char)part);
    g_sendCmd->WriteByte(bValid);
}

void CItemCmdBuilder_Svr::Build_r2c_use_magicitem_result(DWORD            magicItemID,
                                                         EMagicItemResult magicType, int money,
                                                         SItemID& item, int exp) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_use_magicitem_result);
    g_sendCmd->WriteLong(magicItemID);
    g_sendCmd->WriteByte((char)magicType);
    switch (magicType) {
        case MI_Item:
            item.Serialize(g_sendCmd);
            break;
        case MI_Money:
            g_sendCmd->WriteLong(money);
            break;
        case MI_Exp:
            g_sendCmd->WriteLong(exp);
            break;
        case MI_SOUL:
            break;
        case MI_Null:
            break;
        default:
            break;
    }
}

void CItemCmdBuilder_Svr::Build_r2c_other_additem(const char* actorName, SItemID& item,
                                                  char reason) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_other_additem);
    g_sendCmd->WriteString((char*)actorName);
    item.Serialize(g_sendCmd);
    g_sendCmd->WriteByte(reason);
}

void CItemCmdBuilder_Svr::Build_r2c_peteat_result(DWORD petFrom, DWORD petTo, SItemID& itemNew) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_peteat_result);
    g_sendCmd->WriteLong(petFrom);
    g_sendCmd->WriteLong(petTo);
    itemNew.Serialize(g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_player_use_pet(DWORD actorID, SItemID& item,
                                                   EItemContainer container, int page, int gridI,
                                                   int gridJ) {
    CG_CmdPacket* pPacket = g_sendCmd;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_use_pet);
    g_sendCmd->WriteLong(actorID);
    item.Serialize(g_sendCmd);
    g_sendCmd->WriteByte((char)container);
    g_sendCmd->WriteByte(page);
    g_sendCmd->WriteByte(gridI);
    g_sendCmd->WriteByte(gridJ);
}

void CItemCmdBuilder_Svr::Build_r2c_player_itemupdate_result(SItemID& itemOrig, SItemID& scrollOrig,
                                                             SItemID& luckOrig, SItemID& itemNew) {
    CG_CmdPacket* pPacket = g_sendCmd;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_itemupdate_result);
    g_sendCmd->WriteLong(itemOrig.id);
    g_sendCmd->WriteLong(scrollOrig.id);
    g_sendCmd->WriteLong(luckOrig.id);
    itemNew.Serialize(g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_player_equipitem(SItemID&                        item,
                                                     CItemContainerBase::EEquipParts part) {
    CG_CmdPacket* pPacket = g_sendCmd;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_equipitem);
    g_sendCmd->WriteLong(item.id);
    g_sendCmd->WriteByte((char)part);
}

void CItemCmdBuilder_Svr::Build_r2c_player_unequipitem(CItemContainerBase::EEquipParts part) {
    CG_CmdPacket* pPacket = g_sendCmd;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_player_unequipitem);
    g_sendCmd->WriteByte((char)part);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_other_invite(DWORD actorID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_other_invite);
    g_sendCmd->WriteLong(actorID);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_other_invite_response(DWORD actorID, BYTE bAgree) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_other_invite_response);
    g_sendCmd->WriteLong(actorID);
    g_sendCmd->WriteByte((char)bAgree);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_lock(DWORD actorID, BYTE errorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_lock);
    g_sendCmd->WriteLong(actorID);
    g_sendCmd->WriteByte(errorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_begin(DWORD actorID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_begin);
    g_sendCmd->WriteLong(actorID);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_other_agree(DWORD actorID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_other_agree);
    g_sendCmd->WriteLong(actorID);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_unlock(DWORD actorID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_unlock);
    g_sendCmd->WriteLong(actorID);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_deal(DWORD actorID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_deal);
    g_sendCmd->WriteLong(actorID);
}

void CItemCmdBuilder_Svr::Build_r2c_trade_cancel(DWORD actorID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_trade_cancel);
    g_sendCmd->WriteLong(actorID);
}

void CItemCmdBuilder_Svr::Build_r2c_requstID() {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_requstID);
    g_sendCmd->WriteLong(g_region->m_shopnpcoid);
}

void CItemCmdBuilder_Svr::Build_r2c_shop_buy_query_response(DWORD npcID, CBag* pBag,
                                                            std::vector<SNpcSellRareItem>& arrRare,
                                                            short nPriceRate) {
    short nRareCount = (short)arrRare.size();
    if (!pBag)
        return;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_shop_buy_query_response);
    g_sendCmd->WriteLong(npcID);
    g_sendCmd->WriteShort(nPriceRate);
    pBag->Serialize(0, g_sendCmd);
    g_sendCmd->WriteShort(nRareCount);
    for (short i = 0; i < nRareCount; i++) {
        arrRare[i].Serialize(g_sendCmd);
    }
    g_sendCmd->WriteLong((long)time(NULL));
}

void CItemCmdBuilder_Svr::Build_r2c_shop_buy_result(BYTE bSuccess) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_shop_buy_result);
    g_sendCmd->WriteByte(bSuccess);
}

void CItemCmdBuilder_Svr::Build_r2c_point_result(short spoint) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_point_result);
    g_sendCmd->WriteShort(spoint);
}

void CItemCmdBuilder_Svr::Build_r2c_shop_sell_result(BYTE bSuccess) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_shop_sell_result);
    g_sendCmd->WriteByte(bSuccess);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_start_response(BYTE bSuccess) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_start_response);
    g_sendCmd->WriteByte(bSuccess);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_stop_response(BYTE bSuccess) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_stop_response);
    g_sendCmd->WriteByte(bSuccess);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_content(CPrivateShop* pPShop) {
    if (!pPShop)
        return;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_content);
    pPShop->Serialize(g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_sell_result(DWORD from, BYTE bSuccess, BYTE bReason,
                                                      long FakeItemID, long ItemID, short amount) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_sell_result);
    g_sendCmd->WriteLong(from);
    g_sendCmd->WriteByte(bSuccess);
    g_sendCmd->WriteByte(bReason);
    g_sendCmd->WriteLong(FakeItemID);
    g_sendCmd->WriteLong(ItemID);
    g_sendCmd->WriteShort(amount);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_buy_result(DWORD from, BYTE bSuccess, BYTE bReason,
                                                     long ItemID, short amount) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_buy_result);
    g_sendCmd->WriteLong(from);
    g_sendCmd->WriteByte(bSuccess);
    g_sendCmd->WriteByte(bReason);
    g_sendCmd->WriteLong(ItemID);
    g_sendCmd->WriteShort(amount);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_sell_buy(DWORD customer, long FakeItemID, long itemID,
                                                   long money, short amount) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_other_sell);
    g_sendCmd->WriteLong(customer);
    g_sendCmd->WriteLong(FakeItemID);
    g_sendCmd->WriteLong(itemID);
    g_sendCmd->WriteLong(money);
    g_sendCmd->WriteShort(amount);
}

void CItemCmdBuilder_Svr::Build_r2c_pshop_other_buy(DWORD customer, long itemID, long money,
                                                    short amount) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_pshop_other_buy);
    g_sendCmd->WriteLong(customer);
    g_sendCmd->WriteLong(itemID);
    g_sendCmd->WriteLong(money);
    g_sendCmd->WriteShort(amount);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_query_response_pet(CBag* pBag) {
    if (!pBag)
        return;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_query_response_pet);
    pBag->Serialize(0, g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_query_response(CBag* pBag) {
    if (!pBag)
        return;
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_query_response);
    pBag->Serialize(0, g_sendCmd);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_store_response_pet(BYTE bSuccess,
                                                            BYTE reason /* = ITEM_REASON_NULL*/) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_store_response_pet);
    g_sendCmd->WriteByte(bSuccess);
    if (!bSuccess)
        g_sendCmd->WriteByte(reason);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response_pet(
    BYTE bSuccess, BYTE reason /* = ITEM_REASON_NULL*/) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_withdraw_response_pet);
    g_sendCmd->WriteByte(bSuccess);
    if (!bSuccess)
        g_sendCmd->WriteByte(reason);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_store_response(BYTE bSuccess,
                                                        BYTE reason /* = ITEM_REASON_NULL*/) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_store_response);
    g_sendCmd->WriteByte(bSuccess);
    if (!bSuccess)
        g_sendCmd->WriteByte(reason);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_withdraw_response(BYTE bSuccess,
                                                           BYTE reason /* = ITEM_REASON_NULL*/) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_withdraw_response);
    g_sendCmd->WriteByte(bSuccess);
    if (!bSuccess)
        g_sendCmd->WriteByte(reason);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_addpass_result(short errorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_addpass_result);
    g_sendCmd->WriteShort(errorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_removepass_result(short errorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_removepass_result);
    g_sendCmd->WriteShort(errorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_changepass_result(short errorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_changepass_result);
    g_sendCmd->WriteShort(errorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_bank_pass_query(DWORD npcID, short opID) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_bank_pass_query);
    g_sendCmd->WriteLong(npcID);
    g_sendCmd->WriteShort(opID);
}

void CItemCmdBuilder_Svr::Build_r2c_compose_item_begin(DWORD ComposeID, short ErrorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_compose_item_begin);
    g_sendCmd->WriteLong(ComposeID);
    g_sendCmd->WriteShort(ErrorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_compose_item_end(DWORD ComposeID, short ErrorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_compose_item_end);
    g_sendCmd->WriteLong(ComposeID);
    g_sendCmd->WriteShort(ErrorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_decompose_item_result(DWORD itemID, short ErrorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_decompose_item_result);
    g_sendCmd->WriteLong(itemID);
    g_sendCmd->WriteShort(ErrorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_enchant_item_result(DWORD equipItemID, DWORD scrollItemID,
                                                        short ErrorCode) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_enchant_item_result);
    g_sendCmd->WriteLong(equipItemID);
    g_sendCmd->WriteLong(scrollItemID);
    g_sendCmd->WriteShort(ErrorCode);
}

void CItemCmdBuilder_Svr::Build_r2c_fuse_item_result(DWORD ErrorCode, DWORD Item1, DWORD Item2) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_fuse_item_result);
    g_sendCmd->WriteShort(ErrorCode);
    g_sendCmd->WriteLong(Item1);
    g_sendCmd->WriteLong(Item2);
}

void CItemCmdBuilder_Svr::Build_r2c_fuse_gem_result(DWORD ErrorCode, DWORD ItemNew) {
    g_sendCmd->BeginWrite();
    g_sendCmd->WriteShort(r2c_subsystem_item);
    g_sendCmd->WriteShort(r2c_fuse_gem_result);
    g_sendCmd->WriteShort(ErrorCode);
    g_sendCmd->WriteLong(ItemNew);
}

void CItemCmdBuilder_Svr::NetSend_r2g_bankpass(DWORD userID, short operateID, const char* pass) {
    CG_CmdPacket& packet = g_region->m_gws.BeginSend();
    packet.WriteShort(r2g_bankpass);
    packet.WriteLong(userID);
    packet.WriteShort(operateID);
    packet.WriteString((char*)pass);
    g_region->m_gws.EndSend();
}
