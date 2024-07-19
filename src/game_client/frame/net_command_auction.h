
#include "auction_net_base.h"
#include "auction_base.h"

typedef bool (*AUCTION_CMD_PFUNC_GC)(CG_CmdPacket *pPacket);

typedef struct{
	short						cmd_type;
	AUCTION_CMD_PFUNC_GC		cmd_func;
}STAUCTION_GC;

//------------------------
//匹配处理函数
//------------------------
bool AuctionCmdProcess_Client(CG_CmdPacket *cmd);

//--------------------------------------
//处理REGION消息函数声明
//消息定义文件：castlewar_net_define.h
//--------------------------------------
bool	net_r2c_auction_sale_rep			(CG_CmdPacket *pPacket);
bool	net_r2c_auction_buy_rep				(CG_CmdPacket *pPacket);
bool	net_r2c_auction_cancel_buy_rep		(CG_CmdPacket *pPacket);
bool	net_r2c_auction_view_rep			(CG_CmdPacket *pPacket);
bool	net_r2c_auction_get_item_rep		(CG_CmdPacket *pPacket);
bool	net_r2c_auction_update				(CG_CmdPacket *pPacket);

class CAuctionNetHelper
{
public:
	static bool NetSend_c2r_auction_sale(SItemID& id, long base_price, long unique_price, long duration);
	static bool NetSend_c2r_auction_buy(long lAuctionId, long bid_price);
	static bool NetSend_c2r_auction_cancel_buy(long lAuctionId);
	static bool NetSend_c2r_auction_get_item();
	static bool NetSend_c2r_auction_view();
	static bool NetSend_c2r_auction_update();
};
