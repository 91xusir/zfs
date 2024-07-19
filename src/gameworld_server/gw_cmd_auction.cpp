#include "gw_server.h"
#include "auction_base.h"
#include "gw_cmd_auction.h"
#include "auction_net_base.h"
#include "auction.h"

#define NUM(array)		(sizeof(array) / sizeof((array[0])))
#define SAFE_READ(p)	{ if(!(p)) return eInvalidData; }
#define SAFE_ASSERT(p)  { if(!(p)) return eError; }

//----------------------------
//region消息与对应处理函数表
//----------------------------
const STGWAUCTION GWAuctionCmdProcTbl[] = 
{
	{r2g_auction_sale,			cmd_r2g_auction_sale},
	{r2g_auction_buy,			cmd_r2g_auction_buy},
	{r2g_auction_cancel_buy,	cmd_r2g_auction_cancel_buy},
	{r2g_auction_view,			cmd_r2g_auction_view},
	{r2g_auction_get_item,		cmd_r2g_auction_get_item},
};

int GW_AuctionCmdProcess(CG_CmdPacket *cmd)
{
	short cmd_type;

	SAFE_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(i = 0; i < NUM(GWAuctionCmdProcTbl); i++)
	{
		if(cmd_type == GWAuctionCmdProcTbl[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(GWAuctionCmdProcTbl))
	{
		return eError;
	}

	return GWAuctionCmdProcTbl[i].cmd_func(cmd);
}

int	cmd_r2g_auction_sale(CG_CmdPacket *cmd)
{
	CAuctionItem tmp;

	tmp.Serialize(cmd);

	//g_SalesroomMgr.OnSale(&tmp);

	return eOkay;
}

int	cmd_r2g_auction_buy(CG_CmdPacket *cmd)
{
	CAuctionItem tmp;

	tmp.Serialize(cmd);

	//g_SalesroomMgr.OnBidding(&tmp);

	return eOkay;
}

int	cmd_r2g_auction_cancel_buy(CG_CmdPacket *cmd)
{
	CAuctionItem tmp;

	tmp.Serialize(cmd);

	//g_SalesroomMgr.OnCancelBidding(&tmp);

	return eOkay;
}

int	cmd_r2g_auction_view(CG_CmdPacket *cmd)
{
	return eOkay;
}

int	cmd_r2g_auction_get_item(CG_CmdPacket *cmd)
{
	long chr_id;

	SAFE_READ(cmd->ReadLong(&chr_id))

	//g_SalesroomMgr.OnWithDrawItem(chr_id);

	return eOkay;
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_data_init()
{
	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];

		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_auction);
		cmd.WriteShort(g2r_auction_data_init);
		//GetSalesRoom()->Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_data_all_update()
{
	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];

		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_auction);
		cmd.WriteShort(g2r_auction_data_all_update);
		//GetSalesRoom()->Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_data_update(BYTE err_no, CAuctionItem* item)
{
	if(err_no == AUCTION_SALE_ABORT)
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_data_update);
			cmd.WriteByte(AUCTION_SALE_ABORT);
			item->Serialize(&cmd);
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_sale_rep(BYTE err_no, CAuctionItem* item)
{
	if(err_no == AUCTION_SALE_SUCCESS)
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_sale_rep);
			cmd.WriteByte(AUCTION_SALE_SUCCESS);
			item->Serialize(&cmd);
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}else
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_sale_rep);
			cmd.WriteByte(err_no);
			cmd.WriteString((char*)item->BargainorChr().c_str());
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_buy_rep(BYTE err_no, CAuctionItem* item)
{
	if(err_no == AUCTION_BID_ACCEPT)
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_buy_rep);
			cmd.WriteByte(err_no);
			item->Serialize(&cmd);
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}else if(err_no == AUCTION_BUY_SUCCESS)
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_buy_rep);
			cmd.WriteByte(err_no);
			item->Serialize(&cmd);
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}else if(err_no == AUCTION_BUY_PRICE_IS_EXCEEDED)
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_buy_rep);
			cmd.WriteByte(err_no);
			item->Serialize(&cmd);
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}else
	{
		for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
		{
			GWRegion& rg = g_server.m_regionTable[id_cnt];

			if (rg.sessionIndex == -1)	continue;

			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_auction);
			cmd.WriteShort(g2r_auction_sale_rep);
			cmd.WriteByte(err_no);
			cmd.WriteString((char*)item->PurchaserChr().c_str());
			g_server.m_region.SendToPeer(rg.sessionIndex);
		}
	}
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_cancel_buy_rep(char* name, CAuctionItem* item)
{
	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];

		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_auction);
		cmd.WriteShort(g2r_auction_cancel_buy_rep);
		cmd.WriteString(name);
		item->Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

void CAuctionCmdHelper_GW::Broadcast_g2r_auction_get_item_rep(BYTE err_no, CAuctionItem* item)
{
	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];

		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_auction);
		cmd.WriteShort(g2r_auction_get_item_rep);
		cmd.WriteByte(err_no);
		item->Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

void CAuctionCmdHelper_GW::Send_g2r_auction_data_init(long region_id)
{
	guard;

	GWRegion& rg = g_server.m_regionTable[region_id];

	if (rg.sessionIndex == -1) return;	

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_auction);
	cmd.WriteShort(g2r_auction_data_init);
	//g_SalesroomMgr.GetSalesroom()->Serialize(&cmd);
	g_server.m_region.SendToPeer(rg.sessionIndex);

	unguard;
}
