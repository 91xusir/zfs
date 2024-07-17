//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef CMD_CASTLEWAR_H
#define CMD_CASTLEWAR_H

//#include "castlewar_base.h"

typedef int (*CASTLEWAR_CMD_PFUNC)(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
typedef int (*CASTLEWAR_CMD_PFUNC_GW)(CG_CmdPacket *cmd);
typedef struct{
	short						cmd_type;
	CASTLEWAR_CMD_PFUNC			cmd_func;
}STCASTLEWAR;
typedef struct{
	short						cmd_type;
	CASTLEWAR_CMD_PFUNC_GW		cmd_func;
}STCASTLEWAR_GW;

//------------------------
//小函数
//------------------------
bool check_is_vassal(CRegionCreature* cre);
CRegionCreature* find_vassal_by_union_id(DWORD union_id);

//------------------------
//匹配处理函数
//------------------------
int CastleWarCmdProcess_FromGC(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
int CastleWarCmdProcess_FromGW(CG_CmdPacket *cmd);

//--------------------------------------
//处理客户端消息函数声明
//消息定义文件：castlewar_net_define.h
//--------------------------------------
//----------------------------------------------------------------------------------------
int	cmd_c2r_castlewar_reg_attack		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_reg_attack_cancel   (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_reg_defend		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_reg_defend_cancel   (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_allow_bund		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_select_time		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_view_info			  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_insert_flag		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_animal_killed		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_apply_flag		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
int	cmd_c2r_castlewar_flagnpc_show		  (CRegionCreature* CmdGiver,  CG_CmdPacket *cmd);
//----------------------------------------------------------------------------------------

//--------------------------------------
//处理GW消息函数声明/生成反馈消息
//消息定义文件：castlewar_net_define.h
//--------------------------------------
//------------------------------------------------------------------------------
int cmd_g2r_castlewar_data_init				(CG_CmdPacket *cmd);	//通知型
int cmd_g2r_castlewar_data_update			(CG_CmdPacket *cmd);	//通知型
int cmd_g2r_castlewar_user_online			(CG_CmdPacket *cmd);	//通知型
int cmd_g2r_castlewar_war_prepare_rep		(CG_CmdPacket *cmd);	
int cmd_g2r_castlewar_war_start_rep			(CG_CmdPacket *cmd);	
int cmd_g2r_castlewar_war_pause_rep			(CG_CmdPacket *cmd);	
int cmd_g2r_castlewar_war_end_rep			(CG_CmdPacket *cmd);	
int cmd_g2r_castlewar_reload_time_rep		(CG_CmdPacket *cmd);
//------------------------------------------------------------------------------


class CCastleWarCmdBuilder_R2C
{
public:
	static void Build_r2c_castlewar_reg_attack_rep(BYTE err_no);
	static void Build_r2c_castlewar_reg_attack_cancel_rep(BYTE err_no);
	static void Build_r2c_castlewar_reg_defend_rep(BYTE err_no);
	static void Build_r2c_castlewar_reg_defend_cancel_rep(BYTE err_no);
	static void Build_r2c_castlewar_allow_bund_rep(BYTE err_no);
	static void Build_r2c_castlewar_select_time_rep(BYTE err_no);
	static void Build_r2c_castlewar_view_info_rep(STWarInfo* p_war_info);
	static void Build_r2c_castlewar_insert_flag_rep(BYTE err_no);
	static void Build_r2c_castlewar_apply_flag_rep(BYTE err_no);

	static void Build_r2c_castlewar_is_transported();
	static void Build_r2c_castlewar_refresh	(STWarInfo* p_war_info);
	static void Build_r2c_castlewar_user_data_chg(string feod_name, BYTE is_attacker);
	static void Build_r2c_castlewar_war_prepare(string feod_name, string def_union_name, DWORD total_time);
	static void Build_r2c_castlewar_war_start(string feod_name);
	static void Build_r2c_castlewar_war_pause(string feod_name, string win_union_name, DWORD total_time);
	static void Build_r2c_castlewar_war_end(string feod_name, string win_union_name);
	static void Build_r2c_castlewar_flagnpc_created(string union_name, DWORD o_id);
	
private:
	static void Build_R2C_Cmd_Begin();
};

class CCastleWarCmdSend_R2G
{
public:
	static void Send_r2g_castlewar_update_data(STWarInfo* p_war_info);
	static void Send_r2g_castlewar_war_prepare(string feod_name, string def_union_name);
	static void Send_r2g_castlewar_war_start(string feod_name);
	static void Send_r2g_castlewar_war_pause(string feod_name, string win_union_name);
	static void Send_r2g_castlewar_war_end(string feod_name, string win_union_name);
	static void Send_r2g_castlewar_time_set(string feod_name, DWORD time_set);
	static void Send_r2g_castlewar_reload_time(string feod_name);
};


#endif //CMD_CASTLEWAR_H

