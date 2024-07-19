#ifndef _GW_CMD_UNION_H
#define _GW_CMD_UNION_H

class CUnion;
struct SUnionData;
struct SUnionCitizen;
class CVote;

int GW_UnionCmdProcess(CG_CmdPacket *cmd);

#define GW_UNION_CMD_FUNCTION(func) int func(CG_CmdPacket *packet)


GW_UNION_CMD_FUNCTION(cmd_c2r_union_create);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_dismiss_other);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_change_nickname);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_join);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_other_join_response);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_bulletin);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_appoint);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_invitewar);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_surrender);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_leave);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_data_query);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_logo);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_update_level);
GW_UNION_CMD_FUNCTION(cmd_r2g_union_add_score);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_update_level);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_player_levelup);

GW_UNION_CMD_FUNCTION(cmd_c2r_union_join_By_Name);
GW_UNION_CMD_FUNCTION(cmd_r2g_union_update_Contribution);
GW_UNION_CMD_FUNCTION(cmd_r2g_WarScores_Change);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_contribution);
GW_UNION_CMD_FUNCTION(cmd_c2r_union_set_icon);


class CUnionCmdHelper_GW
{
public:
	static void Send_r2c_union_create_result(DWORD playerSendTo, BYTE errorCode, DWORD unionID, char const* unionName);
	static void Send_r2c_union_update_level_result(DWORD playerSendTo, BYTE errorCode, DWORD unionID, BYTE level);
	static void Send_r2c_union_dismiss_other_result(DWORD playerSendTo, BYTE errorCode, DWORD playerID);
	static void Send_r2c_union_dismissed_by_other(DWORD playerSendTo, DWORD playerID, char const* unionName);
	static void Send_r2c_union_change_nickname_result(DWORD playerSendTo, BYTE errorCode, DWORD playerID, char const* newNickname);
	static void Send_r2c_union_other_join_apply(DWORD playerSendTo, DWORD playerID, char const* playerName, const char* szUnionName);
	static void Send_r2c_union_join_response(DWORD playerSendTo, BYTE errorCode, DWORD playerID, DWORD unionID, char const* unionName, char const* playerName);
	static void Send_r2c_union_appoint_result(DWORD playerSendTo, BYTE errorCode);
	static void Send_r2c_union_invitewar_result(DWORD playerSendTo, BYTE errorCode);
	static void Send_r2c_union_surrender_other_response(DWORD playerSendTo, BYTE bAgree);
	static void Send_r2c_union_leave_result(DWORD playerSendTo, BYTE errorCode);
	static void Send_r2c_union_data_refresh(DWORD playerSendTo, SUnionData *pUnionData);
	static void Send_r2c_msg_text(DWORD playerSendTo, const char* pText);
	static void Broadcast_r2c_union_citizen_join(CUnion* pUnion, SUnionCitizen *pCitizen);
	static void Broadcast_r2c_union_citizen_leave(CUnion* pUnion, DWORD playerID);
	static void Broadcast_r2c_union_citizen_online(CUnion* pUnion, DWORD playerID);
	static void Broadcast_r2c_union_citizen_offline(CUnion* pUnion, DWORD playerID);
	static void Broadcast_r2c_union_citizen_official_change(CUnion* pUnion, DWORD playerID, BYTE official);
	static void Broadcast_r2c_union_disband(CUnion* pUnion, char reason, char const* unionName);
	static void Broadcast_r2c_union_new_bulletin(CUnion* pUnion, char const* NewBulletin);
	static void Broadcast_r2c_union_citizen_nickname_change(CUnion* pUnion, DWORD playerID, char const* nickname);
	static void Broadcast_r2c_union_score_change(CUnion* pUnion);
	static void Broadcast_r2c_union_levelup(CUnion* pUnion);
	static void Broadcast_r2c_union_player_levelup(CUnion* pUnion, long playerID, short level);
	static void Broadcast_r2c_union_data_refresh(DWORD playerSendTo, SUnionData *pUnionData);

	static void BroadcastRegion_r2c_union_logo_data(char const* fileName, char const* data, int dataSize, long userDBID, CUnion *pUnion);
	static void UpdateRegionUnionData_r2c_union_logo_data(int iSessionIndex);
	static void UpdateRegionUnionData_r2c_union_data_refresh(int iSessionIndex);
};


#endif //_GW_CMD_UNION_H
