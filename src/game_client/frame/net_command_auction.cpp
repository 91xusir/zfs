
#include "gc_include.h"
#include "region_client.h"
#include "game_assistant.h"
//#include "auction_net_base.h"
//#include "auction_base.h"
#include "net_command_auction.h"
#include "ui_form_vendor.h"
#include "ui_form_bid.h"
#include "ui_form_sale.h"

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define CHECK_NULL(p)		if(!(p)) return false;

const STAUCTION_GC AuctionCmdProcTbl_GC[] = 
{
	{r2c_auction_sale_rep,			net_r2c_auction_sale_rep},
	{r2c_auction_buy_rep,			net_r2c_auction_buy_rep},
	{r2c_auction_cancel_buy_rep,	net_r2c_auction_cancel_buy_rep},
	{r2c_auction_view_rep,			net_r2c_auction_view_rep},
	{r2c_auction_get_item_rep,		net_r2c_auction_get_item_rep},
	{r2c_auction_update,			net_r2c_auction_update}
};

bool AuctionCmdProcess_Client(CG_CmdPacket *cmd)
{
	//参数检查
	short cmd_type;

	CHECK_NULL(cmd)
	CHECK_NULL(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(; i < NUM(AuctionCmdProcTbl_GC); i++)
	{
		if(cmd_type == AuctionCmdProcTbl_GC[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(AuctionCmdProcTbl_GC))
	{
		return false;
	}

	return AuctionCmdProcTbl_GC[i].cmd_func(cmd);
}

bool net_r2c_auction_sale_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case AUCTION_SALE_SUCCESS:
		ShowSystemMessage(R(MSG_AUCTION_SALE_SUCCESS));
		g_SalesroomGC.SerializeDealing(pPacket);		
		break;
	case AUCTION_SALE_ITEM_EXIST:
		ShowSystemMessage(R(MSG_AUCTION_SALE_ITEM_EXIST));
		break;
	case AUCTION_SALE_ITEM_CANNOT_TRADE:
		ShowSystemMessage(R(MSG_AUCTION_SALE_ITEM_CANNOT_TRADE));
		break;
	case AUCTION_SALE_NO_MONEY:
		ShowSystemMessage(R(MSG_AUCTION_SALE_NO_MONEY));
		break;
	case AUCTION_SALE_NUM_OVERFLOW:
		ShowSystemMessage(R(MSG_AUCTION_SALE_NUM_OVERFLOW));
		break;
	case AUCTION_SALE_ABORT:
		ShowSystemMessage(R(MSG_AUCTION_SALE_ABORT));
		break;
	default:
		return false;
	}

	g_layerMain->m_formSale->ClearView();

	return true;

	unguard;
}

bool net_r2c_auction_buy_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case AUCTION_BID_ACCEPT:
		ShowSystemMessage(R(MSG_AUCTION_BID_ACCEPT));
		g_SalesroomGC.SerializeDealing(pPacket);
		break;
	case AUCTION_BID_NO_MONEY:
		ShowSystemMessage(R(MSG_AUCTION_BID_NO_MONEY));
		break;
	case AUCTION_BID_PRICE_INVALID:
		ShowSystemMessage(R(MSG_AUCTION_BID_PRICE_INVALID));
		break;
	case AUCTION_BID_ITEM_OVERFLOW:
		ShowSystemMessage(R(MSG_AUCTION_BID_ITEM_OVERFLOW));
		break;
	case AUCTION_BUY_ERROR_SAMEACC:
		ShowSystemMessage(R(MSG_AUCTION_BUY_ERROR_SAMEACC));
		break;
	case AUCTION_BUY_SUCCESS:
		ShowSystemMessage(R(MSG_AUCTION_BUY_SUCCESS));
		g_SalesroomGC.SerializeDealing(pPacket);
		break;
	default:
		return false;
	}

	g_layerMain->m_formBid->ClearView();

	return true;

	unguard;
}

bool net_r2c_auction_cancel_buy_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case AUCTION_CANCEL_BUY_SUCCESS:
		ShowSystemMessage(R(MSG_AUCTION_CANCEL_BUY_SUCCESS));
		g_SalesroomGC.SerializeDealing(pPacket);
		break;
	case AUCTION_CANCEL_BUY_NOITEM:
		ShowSystemMessage(R(MSG_AUCTION_CANCEL_BUY_NOITEM));
		break;
	default:
		return false;
	}

	g_layerMain->m_formBid->ClearView();

	return true;

	unguard;
}

bool net_r2c_auction_view_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	short sPart = 0;
	pPacket->ReadShort(&sPart);

	if(sPart==1)
	{
		g_SalesroomGCPart1.SerializeDealing(pPacket);
	}else if(sPart==2)
	{
		g_SalesroomGCPart2.SerializeDealing(pPacket);
		g_SalesroomGC.m_DealingItemList = g_SalesroomGCPart1.m_DealingItemList;
		//g_SalesroomGC.m_DealingItemList.merge(g_SalesroomGCPart2.m_DealingItemList);
		//stlforeach(std::list<CAuctionItem>, it1, g_SalesroomGCPart1.m_DealingItemList)
		//{
		//	g_SalesroomGC.m_DealingItemList.push_back(*it1);
		//}
		stlforeach(std::list<CAuctionItem>, it2, g_SalesroomGCPart2.m_DealingItemList)
		{
			g_SalesroomGC.m_DealingItemList.push_back(*it2);
		}

		g_SalesroomGCPart1.Clear();
		g_SalesroomGCPart2.Clear();
		g_layerMain->m_formVendor->Refresh();
		g_layerMain->m_formBid->ClearView();
		g_layerMain->m_formBid->Refresh();
		g_layerMain->m_formSale->ClearView();
		g_layerMain->m_formSale->Refresh();		
	}
	
	if(sPart==0)
	{
		g_SalesroomGC.SerializeDealing(pPacket);

		g_layerMain->m_formVendor->Refresh();
		g_layerMain->m_formBid->ClearView();
		g_layerMain->m_formBid->Refresh();
		g_layerMain->m_formSale->ClearView();
		g_layerMain->m_formSale->Refresh();
	}

	return true;

	unguard;
}

bool net_r2c_auction_update(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	short sPart = 0;
	pPacket->ReadShort(&sPart);

	if(sPart==1)
	{
		g_SalesroomGCPart1.SerializeDealing(pPacket);
	}else if(sPart==2)
	{
		g_SalesroomGCPart2.SerializeDealing(pPacket);
		g_SalesroomGC.m_DealingItemList = g_SalesroomGCPart1.m_DealingItemList;
		//g_SalesroomGC.m_DealingItemList.merge(g_SalesroomGCPart2.m_DealingItemList);
		stlforeach(std::list<CAuctionItem>, it, g_SalesroomGCPart2.m_DealingItemList)
		{
			g_SalesroomGC.m_DealingItemList.push_back(*it);
		}

		g_SalesroomGCPart1.Clear();
		g_SalesroomGCPart2.Clear();

		g_layerMain->m_formVendor->Refresh();
		g_layerMain->m_formBid->ClearView();
		g_layerMain->m_formBid->Refresh();
		g_layerMain->m_formSale->ClearView();
		g_layerMain->m_formSale->Refresh();	
	}

	if(sPart==0)
	{
		g_SalesroomGC.SerializeDealing(pPacket);

		g_layerMain->m_formVendor->Refresh();

		g_layerMain->m_formBid->ClearView();
		g_layerMain->m_formBid->Refresh();

		g_layerMain->m_formSale->ClearView();
		g_layerMain->m_formSale->Refresh();
	}

	return true;

	unguard;
}

bool net_r2c_auction_get_item_rep(CG_CmdPacket *pPacket)
{
	guard;

	CHECK_NULL(pPacket)

	BYTE	err_no;
	READb_byte(err_no);

	switch(err_no)
	{
	case AUCTION_GET_SALE_SUCCESS_ITEM:
		//ShowSystemMessage(R(MSG_AUCTION_GET_SALE_SUCCESS_ITEM));
		break;
	case AUCTION_GET_SALE_FAILED_ITEM:
		//ShowSystemMessage(R(MSG_AUCTION_GET_SALE_FAILED_ITEM));
		break;
	case AUCTION_GET_BUY_SUCCESS_ITEM:
		//ShowSystemMessage(R(MSG_AUCTION_GET_BUY_SUCCESS_ITEM));
		break;
	case AUCTION_GET_BUY_FAILED_ITEM:
		//ShowSystemMessage(R(MSG_AUCTION_GET_BUY_FAILED_ITEM));
		break;
	default:
		return false;
	}

	return true;

	unguard;
}

bool CAuctionNetHelper::NetSend_c2r_auction_sale(SItemID& id, long base_price, long unique_price, long duration)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_system_auction);
	pPacket->WriteShort(c2r_auction_sale);
	id.Serialize(pPacket);
	pPacket->WriteLong(base_price);
	pPacket->WriteLong(unique_price);
	pPacket->WriteLong(duration);
	return NetSend(pPacket);
}

bool CAuctionNetHelper::NetSend_c2r_auction_buy(long lAuctionId, long bid_price)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_system_auction);
	pPacket->WriteShort(c2r_auction_buy);
	//id.Serialize(pPacket);
	pPacket->WriteLong(lAuctionId);
	pPacket->WriteLong(bid_price);
	return NetSend(pPacket);
}

bool CAuctionNetHelper::NetSend_c2r_auction_cancel_buy(long lAuctionId)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_system_auction);
	pPacket->WriteShort(c2r_auction_cancel_buy);
	//id.Serialize(pPacket);
	pPacket->WriteLong(lAuctionId);
	return NetSend(pPacket);
}

bool CAuctionNetHelper::NetSend_c2r_auction_get_item()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_system_auction);
	pPacket->WriteShort(c2r_auction_get_item);
	return NetSend(pPacket);
}

bool CAuctionNetHelper::NetSend_c2r_auction_view()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_system_auction);
	pPacket->WriteShort(c2r_auction_view);
	return NetSend(pPacket);
}

bool CAuctionNetHelper::NetSend_c2r_auction_update()
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_system_auction);
	pPacket->WriteShort(c2r_auction_update);
	return NetSend(pPacket);
    return true;
}