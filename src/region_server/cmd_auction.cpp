
#include "region.h"
#include "item_base.h"
#include "auction_net_base.h"
#include "cmd_auction.h"
#include "region_auction.h"

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define CHECK_READ(p)		if(!(p)) return CMD_ERROR_READ_DATA;
#define CHECK_ASSERT(p)		if(!(p)) return CMD_ERROR_UNKNOWN;
#define CHECK_NULL(p)		if(!(p)) return CMD_ERROR_UNKNOWN;
 
//----------------------------
//�ͻ�����Ϣ���Ӧ��������
//----------------------------
const STAUCTION AuctionCmdProcTbl[] = 
{
	{c2r_auction_sale,			cmd_c2r_auction_sale},
	{c2r_auction_buy,			cmd_c2r_auction_buy},
	{c2r_auction_cancel_buy,	cmd_c2r_auction_cancel_buy},
	{c2r_auction_view,			cmd_c2r_auction_view},
	{c2r_auction_get_item,		cmd_c2r_auction_get_item},
	{c2r_auction_update,		cmd_c2r_auction_update}
};

const STAUCTION_GW AuctionCmdProcTbl_GW[] = 
{
	{g2r_auction_sale_rep,			cmd_g2r_auction_sale_rep},
	{g2r_auction_buy_rep,			cmd_g2r_auction_buy_rep},
	{g2r_auction_cancel_buy_rep,	cmd_g2r_auction_cancel_buy_rep},
	{g2r_auction_view_rep,			cmd_g2r_auction_view_rep},
	{g2r_auction_get_item_rep,		cmd_g2r_auction_get_item_rep},
	{g2r_auction_data_update,		cmd_g2r_auction_data_update},
	{g2r_auction_data_init,			cmd_g2r_auction_data_init}
};


int AuctionCmdProcess_FromGC(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	//�������
	short cmd_type;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(cmd)
	CHECK_ASSERT(cmd->ReadShort(&cmd_type))

	//����ƥ�亯��
	int i = 0;
	for(; i < NUM(AuctionCmdProcTbl); i++)
	{
		if(cmd_type == AuctionCmdProcTbl[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(AuctionCmdProcTbl))
	{
		return CMD_ERROR_UNKNOWN;
	}

	return AuctionCmdProcTbl[i].cmd_func(CmdGiver, cmd);
}

int AuctionCmdProcess_FromGW(CG_CmdPacket *cmd)
{
	//�������
	short cmd_type;

	CHECK_NULL(cmd)
	CHECK_ASSERT(cmd->ReadShort(&cmd_type))

	//����ƥ�亯��
	int i = 0;
	for(; i < NUM(AuctionCmdProcTbl_GW); i++)
	{
		if(cmd_type == AuctionCmdProcTbl_GW[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(AuctionCmdProcTbl_GW))
	{
		return CMD_ERROR_UNKNOWN;
	}

	return AuctionCmdProcTbl_GW[i].cmd_func(cmd);
}

int	cmd_c2r_auction_sale(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	SItemID	item_id;
	long	base_price;
	long	unique_price;
	long	duration;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(CmdGiver->m_userInfo)
	CHECK_NULL(cmd)
	//CHECK_READ(cmd->ReadLong(&item_id))
	
	item_id.Serialize(cmd);
	CHECK_READ(cmd->ReadLong(&base_price))
	CHECK_READ(cmd->ReadLong(&unique_price))
	CHECK_READ(cmd->ReadLong(&duration))
	
	if((base_price <= 0) || (unique_price < 0))
	{
#ifdef WIN32
		LOG1("Item [%d] price invalid\n", item_id.id);
#endif
		return CMD_ERROR_UNKNOWN;

	}

	if((unique_price != 0) && (base_price > unique_price))
	{
#ifdef WIN32
		LOG1("Item [%d] base price is higher than unique price\n", item_id.id);
#endif
		return CMD_ERROR_UNKNOWN;
	}

    /*
	if(CmdGiver->m_pItemContainer->GetMoney() < duration * AUCTION_CHARGE_BASE)
	{
		CAuctionCmdBuilder_R2C::Build_r2c_auction_sale_rep(AUCTION_SALE_NO_MONEY);
		CmdGiver->RecvCmd(g_sendCmd);
		return CMD_ERROR_UNKNOWN;
	}

	if(!ItemCanTrade(item_id))
	{
		CAuctionCmdBuilder_R2C::Build_r2c_auction_sale_rep(AUCTION_SALE_ITEM_CANNOT_TRADE);
		CmdGiver->RecvCmd(g_sendCmd);
		return CMD_ERROR_UNKNOWN;
	}
    */

	//-------------------------------------------------------------------------


    /*
	CAuctionItem tmp;

	tmp.BargainorChr((char*)CmdGiver->m_userInfo->m_username.c_str());
	tmp.ItemID(item_id);
	tmp.BasePrice(base_price);
	tmp.UniquePrice(unique_price);
	tmp.Duration(duration);

	CAuctionCmdBuilder_R2G::Build_r2g_auction_sale(&tmp);
    */
    g_region->m_rsAuction.Auction(CmdGiver, item_id, duration, base_price, unique_price);

	//--update
	CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(CmdGiver);
	//CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int	cmd_c2r_auction_buy(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	//SItemID	item_id;
	long	bid_price, lAuctionID;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(CmdGiver->m_userInfo)
	CHECK_NULL(cmd)
	
	//item_id.Serialize(cmd);
	CHECK_READ(cmd->ReadLong(&lAuctionID))
	CHECK_READ(cmd->ReadLong(&bid_price))

	if(bid_price <= 0)
	{
		LOG1("Auction [%d] bid price invalid\n", lAuctionID);
		return CMD_ERROR_UNKNOWN;

	}

    /*
	//�������Ƿ�Ϸ�
	CAuctionItem* p_item = g_SalesroomBackUp.FindDealingItemById(item_id);
	if(!p_item)
	{
		LOG1("Cannot find item [%d]\n", item_id.id);
		return CMD_ERROR_UNKNOWN;
	}

	if(p_item->BidPrice() * 1.05 * 1.05 > bid_price)
	{
		LOG1("user [%d] bid pirce invalid\n", CmdGiver->m_userInfo->m_userId);
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(AUCTION_BID_PRICE_INVALID);
		CmdGiver->RecvCmd(g_sendCmd);
		return CMD_ERROR_UNKNOWN;
	}

	//����Ǯ�Ƿ��㹻
	if(CmdGiver->m_pItemContainer->GetMoney() < bid_price)
	{
		LOG1("user [%d] Not enough money\n", CmdGiver->m_userInfo->m_userId);
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(AUCTION_BID_NO_MONEY);
		CmdGiver->RecvCmd(g_sendCmd);
		return CMD_ERROR_UNKNOWN;
	}
    */

	//-------------------------------------------------------------------------

    /*
	CAuctionItem tmp;

	tmp.PurchaserChr((char*)CmdGiver->m_userInfo->m_username.c_str());
	tmp.ItemID(item_id);
	tmp.BidPrice(bid_price);

	CAuctionCmdBuilder_R2G::Build_r2g_auction_buy(&tmp);
    */
    g_region->m_rsAuction.Bid(CmdGiver, bid_price, lAuctionID);

	//--update
	CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(CmdGiver);
	//CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int	cmd_c2r_auction_cancel_buy(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	//SItemID	item_id;
	long	lAuctionID;

	CHECK_NULL(CmdGiver)
	CHECK_NULL(CmdGiver->m_userInfo)
	CHECK_NULL(cmd)

	CHECK_READ(cmd->ReadLong(&lAuctionID))
	//item_id.Serialize(cmd);

    /*
	//����Ƿ񾺱��
	CAuctionItem* p_item = g_SalesroomBackUp.FindDealingItemById(item_id);
	if(!p_item)
	{
		LOG2("user [%d] Not bid item [%d]\n", CmdGiver->m_userInfo->m_userId, item_id.id);
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(AUCTION_CANCEL_BUY_NOITEM);
		CmdGiver->RecvCmd(g_sendCmd);
		return CMD_ERROR_UNKNOWN;
	}
    */

	//----------------------------------------------------------------

    /*
	CAuctionItem tmp;

	tmp.PurchaserChr((char*)CmdGiver->m_userInfo->m_username.c_str());
	tmp.ItemID(item_id);

	CAuctionCmdBuilder_R2G::Build_r2g_auction_cancel_buy(&tmp);

	CAuctionCmdBuilder_R2C::Build_r2c_auction_cancel_buy_rep(AUCTION_CANCEL_BUY_SUCCESS);
	CmdGiver->RecvCmd(g_sendCmd);

	//--update
	CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep();
	CmdGiver->RecvCmd(g_sendCmd);
    */
    g_region->m_rsAuction.CancelBid(CmdGiver, lAuctionID);

	//--update
	CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(CmdGiver);
	//CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int	cmd_c2r_auction_view(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{	
	CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(CmdGiver);
	//CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int	cmd_c2r_auction_update(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{	
	CAuctionCmdBuilder_R2C::Build_r2c_auction_update(CmdGiver);
	//CmdGiver->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int	cmd_c2r_auction_get_item(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd)
{
	CHECK_NULL(CmdGiver)
	CHECK_NULL(CmdGiver->m_userInfo)
	CHECK_NULL(cmd)

	g_region->m_rsAuction.GetItems(CmdGiver);
	CAuctionCmdBuilder_R2G::Build_r2g_auction_get_item(CmdGiver->m_userInfo->m_userId);
	
	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_sale_rep(CG_CmdPacket *cmd)
{
	char	err_no;

	CHECK_READ(cmd->ReadByte(&err_no))

	if(err_no == AUCTION_SALE_SUCCESS)
	{
		// ������������
		// 1 ����ұ�����ɾ�����߲���ӵ���������
		// 2 �۳������������
		CAuctionItem item;
		item.Serialize(cmd);
		
		// ��ӵ�������
		g_SalesroomBackUp.AddItem(item, AUCTION_ITEM_DEALING);

		// �۳����ܷ�
		RemoveStorageCharge(&item);

		// �ӱ�����ɾ������
		RemoveItemFromBag(&item);

		CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_sale_rep(AUCTION_SALE_SUCCESS);
		p_cre->RecvCmd(g_sendCmd);

		//--update
		CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(p_cre);
		//p_cre->RecvCmd(g_sendCmd);

	}else
	{
		//long	user_id;
		char*	user_name;

		//CHECK_READ(cmd->ReadLong(&user_id))
		CHECK_READ(cmd->ReadString(&user_name))

		CRegionUser* p_user = g_region->FindUser(user_name);

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_sale_rep(err_no);
		p_cre->RecvCmd(g_sendCmd);
	}
	
	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_buy_rep(CG_CmdPacket *cmd)
{
	char	err_no;

	CHECK_READ(cmd->ReadByte(&err_no))

	if(err_no == AUCTION_BID_ACCEPT)
	{
		// ���걻���ܴ���
		// 1 ����ұ����п۳���Ӧ��Ǯ�������������ã�
		CAuctionItem item;
		item.Serialize(cmd);

		// ���µ�������
		g_SalesroomBackUp.UpdateItem(item, AUCTION_ITEM_DEALING);

		// �۳����۰���������
		RemoveCharge(&item);

		CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(AUCTION_BID_ACCEPT);
		p_cre->RecvCmd(g_sendCmd);

		//--update
		CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(p_cre);
		//p_cre->RecvCmd(g_sendCmd);

	}else if(err_no == AUCTION_BUY_SUCCESS)
	{
		// һ�ڼ�
		// �۳���Ǯ�� ��Ʒ���ض�npc����ȡ�����ʼ�֪ͨ
		CAuctionItem item;
		item.Serialize(cmd);

		// ���µ�������
		g_SalesroomBackUp.DelItem(item, AUCTION_ITEM_DEALING);

		g_SalesroomBackUp.AddItem(item, AUCTION_ITEM_DEAL_SUCCESS);

		// �۳����۰���������
		RemoveCharge(&item);

		CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(AUCTION_BID_ACCEPT);
		p_cre->RecvCmd(g_sendCmd);

		//--update
		CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(p_cre);
		//p_cre->RecvCmd(g_sendCmd);
	}else if(err_no == AUCTION_BUY_PRICE_IS_EXCEEDED)
	{
		// �۳���Ǯ�� ��Ʒ���ض�npc����ȡ�����ʼ�֪ͨ
		CAuctionItem item;
		item.Serialize(cmd);

		// ���µ�������
		g_SalesroomBackUp.AddItem(item, AUCTION_ITEM_DEAL_FAILED);

		//���������߾�֪ͨ
		CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(AUCTION_BID_ACCEPT);
		p_cre->RecvCmd(g_sendCmd);
	}else
	{
		//long	user_id;

		//CHECK_READ(cmd->ReadLong(&user_id))

		char*	user_name;
		CHECK_READ(cmd->ReadString(&user_name))

		CRegionUser* p_user = g_region->FindUser(user_name);

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(err_no);
		p_cre->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_cancel_buy_rep(CG_CmdPacket *cmd)
{
	char	err_no;

	CHECK_READ(cmd->ReadByte(&err_no))

	CAuctionItem item;
	item.Serialize(cmd);

	g_SalesroomBackUp.UpdateItem(item, AUCTION_ITEM_DEALING);

	////���������߾�֪ͨ
	//CRegionUser* p_user = g_region->FindUser(item.BargainorChr());

	//if((p_user == NULL) || p_user->m_dummy == NULL)
	//{
	//	return -1;
	//}
	//CRegionCreature* p_cre = p_user->m_dummy;
	//CAuctionCmdBuilder_R2C::Build_r2c_auction_cancel_buy_rep(AUCTION_CANCEL_BUY_SUCCESS);
	//p_cre->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_view_rep(CG_CmdPacket *cmd)
{
	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_get_item_rep(CG_CmdPacket *cmd)
{
	char	err_no;

	CHECK_READ(cmd->ReadByte(&err_no))

	CAuctionItem item;
	item.Serialize(cmd);

	switch(err_no)
	{
	case AUCTION_GET_SALE_SUCCESS_ITEM:
		{
			CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

			if((p_user == NULL) || p_user->m_dummy == NULL)
			{
				return false;
			}
			CRegionCreature* p_cre = p_user->m_dummy;

			p_cre->AddGold(item.BidPrice());
			
			CAuctionCmdBuilder_R2C::Build_r2c_auction_get_item_rep(AUCTION_GET_SALE_SUCCESS_ITEM, &item);
			p_cre->RecvCmd(g_sendCmd);
		}
		break;
	case AUCTION_GET_SALE_FAILED_ITEM:
		{
			CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

			if((p_user == NULL) || p_user->m_dummy == NULL)
			{
				return false;
			}
			CRegionCreature* p_cre = p_user->m_dummy;

			if(!p_cre->AddItem(item.ItemID()))
			{
				return false;
			}

			CAuctionCmdBuilder_R2C::Build_r2c_auction_get_item_rep(AUCTION_GET_SALE_FAILED_ITEM, &item);
			p_cre->RecvCmd(g_sendCmd);
		}
		break;
	case AUCTION_GET_BUY_SUCCESS_ITEM:
		{
			CRegionUser* p_user = g_region->FindUser((char*)item.PurchaserChr().c_str());

			if((p_user == NULL) || p_user->m_dummy == NULL)
			{
				return false;
			}
			CRegionCreature* p_cre = p_user->m_dummy;

			if(!p_cre->AddItem(item.ItemID()))
			{
				return false;
			}

			CAuctionCmdBuilder_R2C::Build_r2c_auction_get_item_rep(AUCTION_GET_BUY_SUCCESS_ITEM, &item);
			p_cre->RecvCmd(g_sendCmd);
		}
		break;
	case AUCTION_GET_BUY_FAILED_ITEM:
		{
			CRegionUser* p_user = g_region->FindUser((char*)item.PurchaserChr().c_str());

			if((p_user == NULL) || p_user->m_dummy == NULL)
			{
				return false;
			}
			CRegionCreature* p_cre = p_user->m_dummy;

			p_cre->AddGold(item.BidPrice());
			
			CAuctionCmdBuilder_R2C::Build_r2c_auction_get_item_rep(AUCTION_GET_BUY_FAILED_ITEM, &item);
			p_cre->RecvCmd(g_sendCmd);
		}
		break;
	default:
		break;
	}
	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_data_update(CG_CmdPacket *cmd)
{
	char	err_no;

	CHECK_READ(cmd->ReadByte(&err_no))

	if(err_no == AUCTION_SALE_ABORT)
	{
		CAuctionItem item;
		item.Serialize(cmd);

		// ���µ�������
		g_SalesroomBackUp.DelItem(item, AUCTION_ITEM_DEALING);
		g_SalesroomBackUp.AddItem(item, AUCTION_ITEM_DEAL_FAILED);

		//���������߾�֪ͨ
		CRegionUser* p_user = g_region->FindUser((char*)item.BargainorChr().c_str());

		if((p_user == NULL) || p_user->m_dummy == NULL)
		{
			return -1;
		}
		CRegionCreature* p_cre = p_user->m_dummy;
		CAuctionCmdBuilder_R2C::Build_r2c_auction_sale_rep(AUCTION_SALE_ABORT);
		p_cre->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
}

int	cmd_g2r_auction_data_init(CG_CmdPacket *cmd)
{
	LOG("Region Received Init Data From GW\n");
	g_SalesroomBackUp.Serialize(cmd);
	return CMD_ERROR_NONE;
}

//-------------------------------------------------------------

void CAuctionCmdBuilder_R2G::Build_r2g_auction_sale(CAuctionItem* pItem)
{
	if(!pItem) return;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_auction);
	packet.WriteShort(r2g_auction_sale);
	pItem->Serialize(&packet);
	g_region->m_gws.EndSend();
}

void CAuctionCmdBuilder_R2G::Build_r2g_auction_buy(CAuctionItem* pItem)
{
	if(!pItem) return;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_auction);
	packet.WriteShort(r2g_auction_buy);
	pItem->Serialize(&packet);
	g_region->m_gws.EndSend();
}

void CAuctionCmdBuilder_R2G::Build_r2g_auction_cancel_buy(CAuctionItem* pItem)
{
	if(!pItem) return;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_auction);
	packet.WriteShort(r2g_auction_cancel_buy);
	pItem->Serialize(&packet);
	g_region->m_gws.EndSend();
}

void CAuctionCmdBuilder_R2G::Build_r2g_auction_get_item(DWORD chr_id)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_auction);
	packet.WriteShort(r2g_auction_get_item);
	packet.WriteLong(chr_id);
	g_region->m_gws.EndSend();
}

void CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_auction);
}

void CAuctionCmdBuilder_R2C::Build_r2c_auction_sale_rep(BYTE err_no)
{
	CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_auction_sale_rep);
	g_sendCmd->WriteByte(err_no);
}

void CAuctionCmdBuilder_R2C::Build_r2c_auction_buy_rep(BYTE err_no)
{
	CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_auction_buy_rep);
	g_sendCmd->WriteByte(err_no);
}

void CAuctionCmdBuilder_R2C::Build_r2c_auction_cancel_buy_rep(BYTE err_no)
{
	CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_auction_cancel_buy_rep);
	g_sendCmd->WriteByte(err_no);
}

void SplitPacket(std::list<SAuctionItem>* pSrc, std::list<SAuctionItem>& part1, std::list<SAuctionItem>& part2)
{
	if(!pSrc) return;
	int size = (int)pSrc->size();
	int cnt = (int)size/2;
	int i = 0;
	stlforeach(std::list<SAuctionItem>, it, *pSrc)
	{
		if(i<cnt)
		{
			part1.push_back(*it);
		}else
		{
			part2.push_back(*it);
		}
		i++;
	}
}

void SendAuctionToClient(CG_CmdPacket *cmd, std::list<SAuctionItem>* pListAuction)
{
	if(cmd->IsReading())
	{
		char size;
		pListAuction->clear();
		*cmd << size;
		if((int)size < 0)
		{
			return;
		}
		for(int i = 0; i < (int)size; i++)
		{
            pListAuction->push_back(SAuctionItem());
            pListAuction->back().Serialize(cmd);
		}
	}else
	{
        char size = (char)pListAuction->size();		
		*cmd << size;

#ifdef WIN32
        MSG("------ ������Ϣ Region -> Client ---------------------------------\n");
#endif		
		char c=0;
		stlforeach(std::list<SAuctionItem>, it, *pListAuction)
		{
			SAuctionItem* pItem = (SAuctionItem*)&(*it);
			pItem->Serialize(cmd);
#ifdef WIN32
            SItemBase* pItemBase = g_region->m_pItemManager->GetItem(pItem->itemID.type);
            char* szItemName = pItemBase?pItemBase->name:"δ֪����";
            MSG3("    ����: %s, ������: %s, ������: %s, �۸�: %d/%d ����: %d\n",
                szItemName, pItem->strBargainorChr.c_str(), pItem->strPurchaserChr.c_str(), pItem->lBasePrice, pItem->lUniquePrice, pItem->lBidPrice);
#endif
		}
	}
}
//--------------update start by tony 06.04.20---------------------------//
//- reason: ��Ϊ�����İ�̫���ˣ������ͻ��˷���r2c_auction_view_rep
//-			��r2c_auction_updateʱ����2��������
//--------------update end   by tony 06.04.20---------------------------//
void CAuctionCmdBuilder_R2C::Build_r2c_auction_view_rep(CRegionCreature* CmdGiver)
{
	short noPart = 0;
	short iPart1 = 1;
	short iPart2 = 2;
	bool  b = false;
	std::list<SAuctionItem>	part1;
	std::list<SAuctionItem> part2;

	if((int)g_region->m_rsAuction.m_listAuction.size()>100)
	{
		b = true;
	}

	if(b)
	{
		SplitPacket(&g_region->m_rsAuction.m_listAuction, part1, part2);
		LOG2("SplitPacket: part1 size[%d], part2 size[%d]\n", part1.size(), part2.size());

		CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();
		g_sendCmd->WriteShort(r2c_auction_view_rep);	
		g_sendCmd->WriteShort(iPart1);
		//g_SalesroomBackUp.SerializeDealing(g_sendCmd);
		SendAuctionToClient(g_sendCmd, &part1);
		CmdGiver->RecvCmd(g_sendCmd);

		CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();
		g_sendCmd->WriteShort(r2c_auction_view_rep);
		g_sendCmd->WriteShort(iPart2);
		//g_SalesroomBackUp.SerializeDealing(g_sendCmd);
		SendAuctionToClient(g_sendCmd, &part2);
		CmdGiver->RecvCmd(g_sendCmd);
	}else
	{
		CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();
		g_sendCmd->WriteShort(r2c_auction_view_rep);	
		g_sendCmd->WriteShort(noPart);
		//g_SalesroomBackUp.SerializeDealing(g_sendCmd);
		SendAuctionToClient(g_sendCmd, &g_region->m_rsAuction.m_listAuction);
		CmdGiver->RecvCmd(g_sendCmd);
	}
}

void CAuctionCmdBuilder_R2C::Build_r2c_auction_update(CRegionCreature* CmdGiver)
{
	short noPart = 0;
	short iPart1 = 1;
	short iPart2 = 2;
	bool  b = false;
	std::list<SAuctionItem>	part1;
	std::list<SAuctionItem> part2;

	if((int)g_region->m_rsAuction.m_listAuction.size()>100)
	{
		b = true;
	}

	if(b)
	{
		SplitPacket(&g_region->m_rsAuction.m_listAuction, part1, part2);

		CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();
		g_sendCmd->WriteShort(r2c_auction_update);
		g_sendCmd->WriteShort(iPart1);
		//g_SalesroomBackUp.SerializeDealing(g_sendCmd);
		SendAuctionToClient(g_sendCmd, &part1);
		CmdGiver->RecvCmd(g_sendCmd);

		CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();
		g_sendCmd->WriteShort(r2c_auction_update);
		g_sendCmd->WriteShort(iPart2);
		//g_SalesroomBackUp.SerializeDealing(g_sendCmd);
		SendAuctionToClient(g_sendCmd, &part2);
		CmdGiver->RecvCmd(g_sendCmd);
	}else
	{
		CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();
		g_sendCmd->WriteShort(r2c_auction_update);
		g_sendCmd->WriteShort(noPart);
		//g_SalesroomBackUp.SerializeDealing(g_sendCmd);
		SendAuctionToClient(g_sendCmd, &g_region->m_rsAuction.m_listAuction);
		CmdGiver->RecvCmd(g_sendCmd);
	}
}

void CAuctionCmdBuilder_R2C::Build_r2c_auction_get_item_rep(BYTE err_no, CAuctionItem* item)
{
	CAuctionCmdBuilder_R2C::Build_R2C_Cmd_Begin();

	g_sendCmd->WriteShort(r2c_auction_get_item_rep);
	g_sendCmd->WriteByte(err_no);
	item->Serialize(g_sendCmd);
}

