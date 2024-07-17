//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef NET_COMMAND_CASTLEWAR_H
#define NET_COMMAND_CASTLEWAR_H

#include "castlewar_base.h"
#include "castlewar_net_define.h"

typedef bool (*CASTLEWAR_CMD_PFUNC_GC)(CG_CmdPacket *cmd);

typedef struct{
	short						cmd_type;
	CASTLEWAR_CMD_PFUNC_GC		cmd_func;
}STCASTLEWAR_GC;

//----------------------------------------------------------
//错误信息（暂时）
//----------------------------------------------------------
#define STR_ERR_NONE_CWAR				R(MSG_ERR_NONE_CWAR)//"注册已成功"
#define STR_ERR_NOT_VASSAL_CWAR			R(MSG_ERR_NOT_VASSAL_CWAR)//"只有诸侯有权注册"
#define STR_ERR_DIFF_FACTION_CWAR		R(MSG_ERR_DIFF_FACTION_CWAR)//"您不属于这个国家"
#define STR_ERR_WAR_PREPARE_STATE_CWAR	R(MSG_ERR_WAR_PREPARE_STATE_CWAR)//"该城正准备开战，拒绝操作"
#define STR_ERR_WARFARE_STATE_CWAR		R(MSG_ERR_WARFARE_STATE_CWAR)//"该城正在交战，拒绝操作"
#define STR_ERR_REPEAT_REG_CWAR			R(MSG_ERR_REPEAT_REG_CWAR)//"您不能重复注册"
#define STR_ERR_NPC_OCCUPY_STATE_CWAR	R(MSG_ERR_NPC_OCCUPY_STATE_CWAR)//"该城不需要您协助防守"
#define STR_ERR_MASTER_CWAR				R(MSG_ERR_MASTER_CWAR)//"您是城主，不能注册"
#define STR_ERR_FULL_CWAR				R(MSG_ERR_FULL_CWAR)//"人数已满"
#define STR_ERR_LEVEL_LOWER_CWAR		R(MSG_ERR_LEVEL_LOWER_CWAR)//"诸侯国级别太低，至少要2级"

#define STR_ERR_NONE_CANCEL_CWAR				R(MSG_ERR_NONE_CANCEL_CWAR)//"取消注册已成功"
#define STR_ERR_NOT_VASSAL_CANCEL_CWAR			R(MSG_ERR_NOT_VASSAL_CANCEL_CWAR)//"只有诸侯有权取消注册"
#define STR_ERR_ERR_NO_REG_CANCEL_CWAR			R(MSG_ERR_ERR_NO_REG_CANCEL_CWAR)//"您未注册过"
#define STR_ERR_WAR_PREPARE_STATE_CANCEL_CWAR	R(MSG_ERR_WAR_PREPARE_STATE_CANCEL_CWAR)//"该城正准备开战，无法取消注册"
#define STR_ERR_WARFARE_STATE_CANCEL_CWAR		R(MSG_ERR_WARFARE_STATE_CANCEL_CWAR)//"该城正在交战，无法取消注册"
#define STR_ERR_MASTER_CANCEL_CWAR				R(MSG_ERR_MASTER_CANCEL_CWAR)//"城主不能取消注册"

#define STR_ERR_NONE_ALLOW_CWAR					R(MSG_ERR_NONE_ALLOW_CWAR)//"批准已成功"
#define STR_ERR_NUM_OUTRANGE_CWAR				R(MSG_ERR_NUM_OUTRANGE_CWAR)//"同盟数量不能超过8个"
#define STR_SET_TIME_SUCCESS_CWAR				R(MSG_SET_TIME_SUCCESS_CWAR)//"设定时间成功"
#define STR_ERR_TIME_OUTRANGE_CWAR				R(MSG_ERR_TIME_OUTRANGE_CWAR)//"您必须在一小时之内设定时间"
//----------------------------------------------------------

//----------------------------------------------------------
//系统信息（暂时）
//----------------------------------------------------------
#define SYSMSG_WAR_PREPARE		R(MSG_SYSMSG_WAR_PREPARE)//"城战即将开始，非守城人员请速退出，攻守双方做好准备"
#define SYSMSG_WAR_START		R(MSG_SYSMSG_WAR_START)//"已进入战争状态"
#define SYSMSG_WAR_PAUSE		R(MSG_SYSMSG_WAR_PAUSE)//"战争告一段落，"
#define SYSMSG_WAR_END			R(MSG_SYSMSG_WAR_END)//"战争已结束，"
#define SYSMSG_WAR_WINNER		R(MSG_SYSMSG_WAR_WINNER)//"赢得了战争"
#define SYSMSG_WAR_PAUSE_WIN	R(MSG_SYSMSG_WAR_PAUSE_WIN)//"暂时占领了城市"
#define SYSMSG_IS_TRANSPORTED	R(MSG_SYSMSG_IS_TRANSPORTED)//"您登录的区域正发生战争，为了您的安全您被传送到安全区"
//----------------------------------------------------------

//----------------------------------------------------------
//插令旗
//----------------------------------------------------------
#define STR_ERR_UNKNOWN_FLAG		R(MSG_ERR_UNKNOWN_FLAG)//"您无法插令旗"
#define STR_ERR_HAVE_FLAG_ROUND		R(MSG_ERR_HAVE_FLAG_ROUND)//"与周围令旗距离太近，请保持距离"
#define STR_ERR_TIME_WRONG			R(MSG_ERR_TIME_WRONG)//"城战开始时才可以插旗"
#define STR_ERR_PLACE_WRONG			R(MSG_ERR_PLACE_WRONG)//"您不能将令旗插在此处，"
#define STR_ERR_MULTI_FLAG			R(MSG_ERR_MULTI_FLAG)//"您已经有了令旗"
#define STR_ERR_NO_REG_INFO			R(MSG_ERR_NO_REG_INFO)//"您没有注册过"
//----------------------------------------------------------

//------------------------
//匹配处理函数
//------------------------
bool CastleWarCmdProcess_Client(CG_CmdPacket *cmd);

//--------------------------------------
//处理REGION消息函数声明
//消息定义文件：castlewar_net_define.h
//--------------------------------------
bool	net_r2c_castlewar_war_prepare			(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_war_start				(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_war_pause				(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_war_end				(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_have_apply			(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_have_attacker			(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_reg_attack_rep		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_reg_attack_cancel_rep	(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_reg_defend_rep		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_reg_defend_cancel_rep	(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_allow_bund_rep		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_select_time_rep		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_view_info_rep			(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_insert_flag_rep		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_refresh				(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_user_data_chg			(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_is_transported		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_apply_flag_rep		(CG_CmdPacket *cmd);
bool	net_r2c_castlewar_flagnpc_created		(CG_CmdPacket *cmd);

class CCastleWarNetHelper
{
public:
	static bool NetSend_c2r_castlewar_reg_attack(DWORD npc_id);
	static bool NetSend_c2r_castlewar_reg_defend(DWORD npc_id);
	static bool NetSend_c2r_castlewar_reg_attack_cancel(DWORD npc_id);
	static bool NetSend_c2r_castlewar_reg_defend_cancel(DWORD npc_id);
	static bool NetSend_c2r_castlewar_select_time(DWORD npc_id, short week_day);
	static bool NetSend_c2r_castlewar_allow_bund(DWORD npc_id, DWORD union_id, BYTE op);
	static bool NetSend_c2r_castlewar_view_info(DWORD npc_id);
	static bool NetSend_c2r_castlewar_apply_flag(DWORD npc_id);
	static bool NetSend_c2r_castlewar_flagnpc_show(std::string union_name, DWORD o_id);
};

#endif //NET_COMMAND_CASTLEWAR_H