
#ifndef GW_CMD_AUCTION_H
#define GW_CMD_AUCTION_H

class CAuctionItem;

typedef int (*GW_AUCTION_CMD_PFUNC)(CG_CmdPacket *cmd);
typedef struct {
	short					cmd_type;
	GW_AUCTION_CMD_PFUNC	cmd_func;
}STGWAUCTION;

//------------------------------
//region消息匹配函数
//------------------------------
int GW_AuctionCmdProcess(CG_CmdPacket *cmd);

//-------------------------------
//region消息处理函数
//-------------------------------
int	cmd_r2g_auction_sale		(CG_CmdPacket *cmd);
int	cmd_r2g_auction_buy			(CG_CmdPacket *cmd);
int	cmd_r2g_auction_cancel_buy	(CG_CmdPacket *cmd);
int	cmd_r2g_auction_view		(CG_CmdPacket *cmd);
int	cmd_r2g_auction_get_item	(CG_CmdPacket *cmd);

class CAuctionCmdHelper_GW
{
public:
	static void Broadcast_g2r_auction_data_init();
	static void Broadcast_g2r_auction_data_all_update();
	static void Broadcast_g2r_auction_data_update(BYTE err_no, CAuctionItem* item);
	static void Broadcast_g2r_auction_sale_rep(BYTE err_no, CAuctionItem* item);
	static void Broadcast_g2r_auction_buy_rep(BYTE err_no, CAuctionItem* item);
	static void Broadcast_g2r_auction_get_item_rep(BYTE err_no, CAuctionItem* item);
	static void Broadcast_g2r_auction_cancel_buy_rep(char* name, CAuctionItem* item);

	static void Send_g2r_auction_data_init(long region_id);
};

#endif //GW_CMD_AUCTION_H

