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
//������Ϣ����ʱ��
//----------------------------------------------------------
#define STR_ERR_NONE_CWAR				R(MSG_ERR_NONE_CWAR)//"ע���ѳɹ�"
#define STR_ERR_NOT_VASSAL_CWAR			R(MSG_ERR_NOT_VASSAL_CWAR)//"ֻ�������Ȩע��"
#define STR_ERR_DIFF_FACTION_CWAR		R(MSG_ERR_DIFF_FACTION_CWAR)//"���������������"
#define STR_ERR_WAR_PREPARE_STATE_CWAR	R(MSG_ERR_WAR_PREPARE_STATE_CWAR)//"�ó���׼����ս���ܾ�����"
#define STR_ERR_WARFARE_STATE_CWAR		R(MSG_ERR_WARFARE_STATE_CWAR)//"�ó����ڽ�ս���ܾ�����"
#define STR_ERR_REPEAT_REG_CWAR			R(MSG_ERR_REPEAT_REG_CWAR)//"�������ظ�ע��"
#define STR_ERR_NPC_OCCUPY_STATE_CWAR	R(MSG_ERR_NPC_OCCUPY_STATE_CWAR)//"�óǲ���Ҫ��Э������"
#define STR_ERR_MASTER_CWAR				R(MSG_ERR_MASTER_CWAR)//"���ǳ���������ע��"
#define STR_ERR_FULL_CWAR				R(MSG_ERR_FULL_CWAR)//"��������"
#define STR_ERR_LEVEL_LOWER_CWAR		R(MSG_ERR_LEVEL_LOWER_CWAR)//"��������̫�ͣ�����Ҫ2��"

#define STR_ERR_NONE_CANCEL_CWAR				R(MSG_ERR_NONE_CANCEL_CWAR)//"ȡ��ע���ѳɹ�"
#define STR_ERR_NOT_VASSAL_CANCEL_CWAR			R(MSG_ERR_NOT_VASSAL_CANCEL_CWAR)//"ֻ�������Ȩȡ��ע��"
#define STR_ERR_ERR_NO_REG_CANCEL_CWAR			R(MSG_ERR_ERR_NO_REG_CANCEL_CWAR)//"��δע���"
#define STR_ERR_WAR_PREPARE_STATE_CANCEL_CWAR	R(MSG_ERR_WAR_PREPARE_STATE_CANCEL_CWAR)//"�ó���׼����ս���޷�ȡ��ע��"
#define STR_ERR_WARFARE_STATE_CANCEL_CWAR		R(MSG_ERR_WARFARE_STATE_CANCEL_CWAR)//"�ó����ڽ�ս���޷�ȡ��ע��"
#define STR_ERR_MASTER_CANCEL_CWAR				R(MSG_ERR_MASTER_CANCEL_CWAR)//"��������ȡ��ע��"

#define STR_ERR_NONE_ALLOW_CWAR					R(MSG_ERR_NONE_ALLOW_CWAR)//"��׼�ѳɹ�"
#define STR_ERR_NUM_OUTRANGE_CWAR				R(MSG_ERR_NUM_OUTRANGE_CWAR)//"ͬ���������ܳ���8��"
#define STR_SET_TIME_SUCCESS_CWAR				R(MSG_SET_TIME_SUCCESS_CWAR)//"�趨ʱ��ɹ�"
#define STR_ERR_TIME_OUTRANGE_CWAR				R(MSG_ERR_TIME_OUTRANGE_CWAR)//"��������һСʱ֮���趨ʱ��"
//----------------------------------------------------------

//----------------------------------------------------------
//ϵͳ��Ϣ����ʱ��
//----------------------------------------------------------
#define SYSMSG_WAR_PREPARE		R(MSG_SYSMSG_WAR_PREPARE)//"��ս������ʼ�����س���Ա�����˳�������˫������׼��"
#define SYSMSG_WAR_START		R(MSG_SYSMSG_WAR_START)//"�ѽ���ս��״̬"
#define SYSMSG_WAR_PAUSE		R(MSG_SYSMSG_WAR_PAUSE)//"ս����һ���䣬"
#define SYSMSG_WAR_END			R(MSG_SYSMSG_WAR_END)//"ս���ѽ�����"
#define SYSMSG_WAR_WINNER		R(MSG_SYSMSG_WAR_WINNER)//"Ӯ����ս��"
#define SYSMSG_WAR_PAUSE_WIN	R(MSG_SYSMSG_WAR_PAUSE_WIN)//"��ʱռ���˳���"
#define SYSMSG_IS_TRANSPORTED	R(MSG_SYSMSG_IS_TRANSPORTED)//"����¼������������ս����Ϊ�����İ�ȫ�������͵���ȫ��"
//----------------------------------------------------------

//----------------------------------------------------------
//������
//----------------------------------------------------------
#define STR_ERR_UNKNOWN_FLAG		R(MSG_ERR_UNKNOWN_FLAG)//"���޷�������"
#define STR_ERR_HAVE_FLAG_ROUND		R(MSG_ERR_HAVE_FLAG_ROUND)//"����Χ�������̫�����뱣�־���"
#define STR_ERR_TIME_WRONG			R(MSG_ERR_TIME_WRONG)//"��ս��ʼʱ�ſ��Բ���"
#define STR_ERR_PLACE_WRONG			R(MSG_ERR_PLACE_WRONG)//"�����ܽ�������ڴ˴���"
#define STR_ERR_MULTI_FLAG			R(MSG_ERR_MULTI_FLAG)//"���Ѿ���������"
#define STR_ERR_NO_REG_INFO			R(MSG_ERR_NO_REG_INFO)//"��û��ע���"
//----------------------------------------------------------

//------------------------
//ƥ�䴦����
//------------------------
bool CastleWarCmdProcess_Client(CG_CmdPacket *cmd);

//--------------------------------------
//����REGION��Ϣ��������
//��Ϣ�����ļ���castlewar_net_define.h
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