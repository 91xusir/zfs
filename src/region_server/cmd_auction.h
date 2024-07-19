
#ifndef CMD_AUCTION_H
#define CMD_AUCTION_H

class CAuctionItem;

typedef int (*AUCTION_CMD_PFUNC)(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
typedef int (*AUCTION_CMD_PFUNC_GW)(CG_CmdPacket *cmd);
typedef struct{
	short						cmd_type;
	AUCTION_CMD_PFUNC			cmd_func;
}STAUCTION;
typedef struct{
	short						cmd_type;
	AUCTION_CMD_PFUNC_GW		cmd_func;
}STAUCTION_GW;

//------------------------
//匹配处理函数
//------------------------
int AuctionCmdProcess_FromGC(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
int AuctionCmdProcess_FromGW(CG_CmdPacket *cmd);

//--------------------------------------
//处理客户端消息函数声明
//消息定义文件：auction_net_define.h
//--------------------------------------
int	cmd_c2r_auction_sale		(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_auction_buy			(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_auction_cancel_buy	(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_auction_view		(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_auction_get_item	(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_auction_update		(CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);

int	cmd_g2r_auction_sale_rep		(CG_CmdPacket *cmd);
int	cmd_g2r_auction_buy_rep			(CG_CmdPacket *cmd);
int	cmd_g2r_auction_cancel_buy_rep	(CG_CmdPacket *cmd);
int	cmd_g2r_auction_view_rep		(CG_CmdPacket *cmd);
int	cmd_g2r_auction_get_item_rep	(CG_CmdPacket *cmd);
int	cmd_g2r_auction_data_update		(CG_CmdPacket *cmd);
int	cmd_g2r_auction_data_init		(CG_CmdPacket *cmd);

class CAuctionCmdBuilder_R2G
{
public:
	static void Build_r2g_auction_sale(CAuctionItem* pItem);
	static void Build_r2g_auction_buy(CAuctionItem* pItem);
	static void Build_r2g_auction_cancel_buy(CAuctionItem* pItem);
	static void Build_r2g_auction_get_item(DWORD chr_id);
};

class CAuctionCmdBuilder_R2C
{
public:
	static void Build_r2c_auction_sale_rep(BYTE	err_no);
	static void Build_r2c_auction_buy_rep(BYTE	err_no);
	static void Build_r2c_auction_cancel_buy_rep(BYTE	err_no);
	static void Build_r2c_auction_view_rep(CRegionCreature* CmdGiver);
	static void Build_r2c_auction_get_item_rep(BYTE	err_no, CAuctionItem* item);
	static void Build_r2c_auction_update(CRegionCreature* CmdGiver);

private:
	static void Build_R2C_Cmd_Begin();
};

#endif //CMD_AUCTION_H

