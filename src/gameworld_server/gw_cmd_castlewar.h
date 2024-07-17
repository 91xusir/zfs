#ifndef GW_CMD_CASTLEWAR_H
#define GW_CMD_CASTLEWAR_H


typedef int (*GW_CASTLEWAR_CMD_PFUNC)(CG_CmdPacket *cmd);
typedef struct {
	short					cmd_type;
	GW_CASTLEWAR_CMD_PFUNC	cmd_func;
}STGWCALSTLEWAR;

#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define SAFE_READ(p)	{ if(!(p)) return eInvalidData; }
#define SAFE_ASSERT(p)  { if(!(p)) return eError; }

#define WAR_PREPARE 0
#define WAR_START	1
#define WAR_PAUSE	2
#define WAR_END		3

//------------------------------
//region消息匹配函数
//------------------------------
int GW_CastleWarCmdProcess(CG_CmdPacket *cmd);

//-------------------------------
//region消息处理函数
//-------------------------------
int	cmd_r2g_castlewar_update_data		(CG_CmdPacket *cmd);
int	cmd_r2g_castlewar_war_prepare		(CG_CmdPacket *cmd);
int	cmd_r2g_castlewar_war_start			(CG_CmdPacket *cmd);
int	cmd_r2g_castlewar_war_pause			(CG_CmdPacket *cmd);
int	cmd_r2g_castlewar_war_end			(CG_CmdPacket *cmd);
int	cmd_r2g_castlewar_time_set			(CG_CmdPacket *cmd);
int	cmd_r2g_castlewar_reload_time		(CG_CmdPacket *cmd);

class CCastleWarCmdHelper_GW
{
public:
	static void Send_g2r_castlewar_data_init(long region_id, STWarInfo* p_war_info);
	static void Send_g2r_castlewar_data_update(long region_id, STWarInfo* p_war_info);
	static void Send_g2r_castlewar_user_online(long region_id, DWORD user_db_id);
	
	static void Broadcast_g2r_castlewar_war_prepare_rep(char* p_feod_name, char* p_def_union_name, DWORD total_time);
	static void Broadcast_g2r_castlewar_war_start_rep(char* p_feod_name);
	static void Broadcast_g2r_castlewar_war_pause_rep(char* p_feod_name, char* p_def_union_name);
	static void Broadcast_g2r_castlewar_war_end_rep(char* p_feod_name, char* p_win_union_name);

	static void Broadcast_g2r_castlewar_war_bulletin(char* p_feod_name, char* p_union_name, int op);
	static void Broadcast_g2r_castlewar_reload_time_rep(char* p_feod_name, DWORD start_time, int def_duration);
	/*static void Broadcast_g2r_castlewar_war_start_bulletin(char* p_feod_name);
	static void Broadcast_g2r_castlewar_war_pause_bulletin(char* p_feod_name);
	static void Broadcast_g2r_castlewar_war_end_bulletin(char* p_feod_name);*/
};


#endif //GW_CMD_CASTLEWAR_H

