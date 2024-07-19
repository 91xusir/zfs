#ifndef CMD_UNION_H
#define CMD_UNION_H

class CRegionCreature;
class CUnion;

// 诸侯国操作的锁定
extern EXT_SPACE::hash_map<DWORD, DWORD> g_UnionLockTick;
void LockUnionOperate(DWORD unionID, bool bLock);
bool IsUnionOperateLocked(DWORD unionID);


int UnionCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);
int UnionCmdProcess_FromGW(CG_CmdPacket *cmd);

#define UNION_CMD_FUNCTION(func)				int func(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
#define UNION_CMD_FUNCTION_FROM_GW(func)		int func(CG_CmdPacket *cmd)


UNION_CMD_FUNCTION(cmd_c2r_union_create);
UNION_CMD_FUNCTION(cmd_c2r_union_dismiss_other);
UNION_CMD_FUNCTION(cmd_c2r_union_change_nickname);
UNION_CMD_FUNCTION(cmd_c2r_union_join);
UNION_CMD_FUNCTION(cmd_c2r_union_other_join_response);
UNION_CMD_FUNCTION(cmd_c2r_union_bulletin);
UNION_CMD_FUNCTION(cmd_c2r_union_appoint);
UNION_CMD_FUNCTION(cmd_c2r_union_invitewar);
UNION_CMD_FUNCTION(cmd_c2r_union_surrender);
UNION_CMD_FUNCTION(cmd_c2r_union_leave);
UNION_CMD_FUNCTION(cmd_c2r_union_data_query);
UNION_CMD_FUNCTION(cmd_c2r_union_logo);
UNION_CMD_FUNCTION(cmd_c2r_union_query_logo_data);
UNION_CMD_FUNCTION(cmd_c2r_union_update_level);
UNION_CMD_FUNCTION(cmd_c2r_union_do_vote);
UNION_CMD_FUNCTION(cmd_c2r_union_log_query);
UNION_CMD_FUNCTION(cmd_c2r_union_give_union);
UNION_CMD_FUNCTION(cmd_c2r_union_join_By_Name);
UNION_CMD_FUNCTION(cmd_c2r_union_contribution);
UNION_CMD_FUNCTION(cmd_c2r_union_set_icon);


UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_create_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_dismiss_other_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_dismissed_by_other);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_change_nickname_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_other_join_apply);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_join_response);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_appoint_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_invitewar_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_surrender_other_response);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_leave_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_data_refresh);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_join);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_leave);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_online);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_offline);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_official_change);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_disband);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_new_bulletin);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_nickname_change);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_logo_data);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_update_level_result);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_score_change);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_levelup);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_player_levelup);


UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_new_vote);
UNION_CMD_FUNCTION_FROM_GW(cmd_c2r_union_log_query);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_new_log);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_WarScores_Change);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_update_Contribution);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_contribution);
UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_set_icon);
UNION_CMD_FUNCTION_FROM_GW(cmd_g2r_set_union_icon_result);


class CUnionCmdBuilder_Svr
{
public:
	static void Build_r2c_union_create_result(BYTE errorCode, DWORD unionID, char const* unionName);
	static void Build_r2c_union_update_level_result(BYTE errorCode);
	static void Build_r2c_union_dismiss_other_result(BYTE errorCode, DWORD playerID);
	static void Build_r2c_union_dismissed_by_other(DWORD playerID, char const* unionName);
	static void Build_r2c_union_change_nickname_result(BYTE errorCode, DWORD playerID, char const* newNickname);
	static void Build_r2c_union_other_join_apply(DWORD playerID, char const* playerName, const char* szUnionName);
	static void Build_r2c_union_join_response(BYTE errorCode, DWORD playerID, DWORD unionID, char const* unionName, char const* playerName);
	static void Build_r2c_union_appoint_result(BYTE errorCode);
	static void Build_r2c_union_invitewar_result(BYTE errorCode);
	static void Build_r2c_union_surrender_other_response(BYTE bAgree);
	static void Build_r2c_union_leave_result(BYTE errorCode);
	static void Build_r2c_union_data_refresh(SUnionData *pUnionData);
	static void Build_r2c_union_citizen_join(SUnionCitizen *pCitizen);
	static void Build_r2c_union_citizen_leave(DWORD playerID);
	static void Build_r2c_union_citizen_online(DWORD playerID);
	static void Build_r2c_union_citizen_offline(DWORD playerID);
	static void Build_r2c_union_citizen_official_change(DWORD playerID, BYTE official);
	static void Build_r2c_union_disband(short reason, char const* unionName);
	static void Build_r2c_union_new_bulletin(char const* NewBulletin);
	static void Build_r2c_union_citizen_nickname_change(DWORD playerID, char const* nickname);
	static void Build_r2c_union_logo_result(char errorCode, char const* fileNewName);
	static void Build_r2c_union_logo_data(char const* fileName, char const* data, int dataSize);
	static void Build_r2c_union_icon_change(char const* UnionIcon);
	static void Build_r2c_union_score_change(long score);
	static void Build_r2c_union_levelup();
	static void Build_r2c_union_player_levelup(long playerID, short playerLevel);
};

#endif //CMD_UNION_H
