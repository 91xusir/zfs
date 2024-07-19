#include "region.h"

// 诸侯国操作的锁定
EXT_SPACE::hash_map<DWORD, DWORD> g_UnionLockTick;

void LockUnionOperate(DWORD unionID, bool bLock)
{
	if (g_UnionLockTick.size() > 100000)
		CHECK(0);

	EXT_SPACE::hash_map<DWORD, DWORD>::iterator iter = g_UnionLockTick.find(unionID);
	if (bLock)
	{
		DWORD tick = rtGetMilliseconds();
		if (iter==g_UnionLockTick.end())
		{
			g_UnionLockTick[unionID] = tick;
		}
		else 
		{
			iter->second = tick;
		}
	}
	else 
	{
		if (iter==g_UnionLockTick.end())
		{
			g_UnionLockTick[unionID] = 0;
		}
		else 
		{
			iter->second = 0;
		}
	}
}

bool IsUnionOperateLocked(DWORD unionID)
{
	if (g_UnionLockTick.size() > 100000)
		CHECK(0);

	EXT_SPACE::hash_map<DWORD, DWORD>::iterator iter = g_UnionLockTick.find(unionID);
	if (iter==g_UnionLockTick.end())
	{
		return false;
	}
	else 
	{
		DWORD now = rtGetMilliseconds();
		if (now - iter->second > 5000)
		{
			iter->second = 0;
			return false;
		}
		else 
		{
			return true;
		}
	}

	return false;
}


#define SAFE_READ(p)	{ if(!(p)) return CMD_ERROR_READ_DATA; }
#define SAFE_ASSERT(p)	{ if ( !(p) ) return CMD_ERROR_UNKNOWN; }

int UnionCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	short union_cmd_type;
	if ( !cmd->ReadShort(&union_cmd_type) )		return CMD_ERROR_UNKNOWN;

//	LOG2("Union command[%d] Received From Client[UserDBID=%d]\n", union_cmd_type, CmdGiver->m_userInfo->m_userId);

	switch(union_cmd_type)
	{
	case c2r_union_create:
		return cmd_c2r_union_create(CmdGiver, cmd);
	case c2r_union_dismiss_other:
		return cmd_c2r_union_dismiss_other(CmdGiver, cmd);
	case c2r_union_change_nickname:
		return cmd_c2r_union_change_nickname(CmdGiver, cmd);
	case c2r_union_join:
		return cmd_c2r_union_join(CmdGiver, cmd);
	case c2r_union_other_join_response:
		return cmd_c2r_union_other_join_response(CmdGiver, cmd);
	case c2r_union_bulletin:
		return cmd_c2r_union_bulletin(CmdGiver, cmd);
	case c2r_union_appoint:
		return cmd_c2r_union_appoint(CmdGiver, cmd);
	case c2r_union_invitewar:
		return cmd_c2r_union_invitewar(CmdGiver, cmd);
	case c2r_union_surrender:
		return cmd_c2r_union_surrender(CmdGiver, cmd);
	case c2r_union_leave:
		return cmd_c2r_union_leave(CmdGiver, cmd);
	case c2r_union_data_query:
		return cmd_c2r_union_data_query(CmdGiver, cmd);
	case c2r_union_logo:
		return cmd_c2r_union_logo(CmdGiver, cmd);
	case c2r_union_query_logo_data:
		return cmd_c2r_union_query_logo_data(CmdGiver, cmd);
	case c2r_union_update_level:
		return cmd_c2r_union_update_level(CmdGiver, cmd);
	case c2r_union_do_vote:
		return cmd_c2r_union_do_vote(CmdGiver, cmd);
	case c2r_union_log_query:
		return cmd_c2r_union_log_query(CmdGiver, cmd);
	case c2r_union_give_union:
		return cmd_c2r_union_give_union(CmdGiver, cmd);
	case c2r_union_join_By_Name:
		return cmd_c2r_union_join_By_Name(CmdGiver, cmd);
	case c2r_union_contribution:
		return cmd_c2r_union_contribution(CmdGiver, cmd);
	case c2r_union_set_icon:
		return cmd_c2r_union_set_icon(CmdGiver, cmd);
	default:
		return CMD_ERROR_UNKNOWN;
	}
}

int UnionCmdProcess_FromGW(CG_CmdPacket *cmd)
{
	short union_cmd_type;
	if ( !cmd->ReadShort(&union_cmd_type) )		return CMD_ERROR_UNKNOWN;

	switch(union_cmd_type)
	{
	case r2c_union_create_result:
		return cmd_r2c_union_create_result(cmd);
	case r2c_union_dismiss_other_result:
		return cmd_r2c_union_dismiss_other_result(cmd);
	case r2c_union_dismissed_by_other:
		return cmd_r2c_union_dismissed_by_other(cmd);
	case r2c_union_change_nickname_result:
		return cmd_r2c_union_change_nickname_result(cmd);
	case r2c_union_other_join_apply:
		return cmd_r2c_union_other_join_apply(cmd);
	case r2c_union_join_response:
		return cmd_r2c_union_join_response(cmd);
	case r2c_union_appoint_result:
		return cmd_r2c_union_appoint_result(cmd);
	case r2c_union_invitewar_result:
		return cmd_r2c_union_invitewar_result(cmd);
	case r2c_union_surrender_other_response:
		return cmd_r2c_union_surrender_other_response(cmd);
	case r2c_union_leave_result:
		return cmd_r2c_union_leave_result(cmd);
	case r2c_union_data_refresh:
		return cmd_r2c_union_data_refresh(cmd);
	case r2c_union_citizen_join:
		return cmd_r2c_union_citizen_join(cmd);
	case r2c_union_citizen_leave:
		return cmd_r2c_union_citizen_leave(cmd);
	case r2c_union_citizen_online:
		return cmd_r2c_union_citizen_online(cmd);
	case r2c_union_citizen_offline:
		return cmd_r2c_union_citizen_offline(cmd);
	case r2c_union_citizen_official_change:
		return cmd_r2c_union_citizen_official_change(cmd);
	case r2c_union_disband:
		return cmd_r2c_union_disband(cmd);
	case r2c_union_new_bulletin:
		return cmd_r2c_union_new_bulletin(cmd);
	case r2c_union_citizen_nickname_change:
		return cmd_r2c_union_citizen_nickname_change(cmd);
	case r2c_union_logo_data:
		return cmd_r2c_union_logo_data(cmd);
	case r2c_union_score_change:
		return cmd_r2c_union_score_change(cmd);
	case r2c_union_update_level_result:
		return cmd_r2c_union_update_level_result(cmd);
	case r2c_union_levelup:
		return cmd_r2c_union_levelup(cmd);
	case r2c_union_player_levelup:
		return cmd_r2c_union_player_levelup(cmd);
	case r2c_union_new_vote:
		return cmd_r2c_union_new_vote(cmd);
	case c2r_union_log_query:
		return cmd_c2r_union_log_query(cmd);
	case r2c_union_new_log:
		return cmd_r2c_union_new_log(cmd);
	case r2c_WarScores_Change:
		return cmd_r2c_WarScores_Change(cmd);
	case r2c_union_update_Contribution:
		return cmd_r2c_union_update_Contribution(cmd);
	case r2c_union_contribution:
		return cmd_r2c_union_contribution(cmd);
	case r2c_union_set_icon:
		return cmd_r2c_union_set_icon(cmd);
	case g2r_set_union_icon_result:
		return cmd_g2r_set_union_icon_result(cmd);
	default:
		return CMD_ERROR_UNKNOWN;
	}
}

UNION_CMD_FUNCTION(cmd_c2r_union_create)
{
	char *name, *enounce;
	long npcID;
	SAFE_READ(cmd->ReadLong(&npcID))
	SAFE_READ(cmd->ReadString(&name) )
	SAFE_READ(cmd->ReadString(&enounce) )

	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	if (strlen(name) > UNION_NAME_STRING_MAX)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_create_result(UNION_ERROR_UNKNOWN, 0, "");
		cre->RecvCmd(g_sendCmd);
		return CMD_ERROR_NONE;
	}
	if (!g_region->UserUploadString_IsValid(name))
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_create_result(UNION_ERROR_UNKNOWN, 0, "");
		cre->RecvCmd(g_sendCmd);
		return CMD_ERROR_NONE;
	}
	if (strlen(enounce) > UNION_ENOUNCE_STRING_MAX)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_create_result(UNION_ERROR_UNKNOWN, 0, "");
		cre->RecvCmd(g_sendCmd);
		return CMD_ERROR_NONE;
	}
	if (!g_region->UserUploadString_IsValid(enounce))
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_create_result(UNION_ERROR_UNKNOWN, 0, "");
		cre->RecvCmd(g_sendCmd);
		return CMD_ERROR_NONE;
	}

	// CHECK 
	// 玩家不属于任何诸侯国(Client,GWServer)
	// 玩家有足够的等级和金钱(Client,RegionSvr)
	// 诸侯国名字和宣言合法(Client,GWServer)
	// 诸侯国名字不可重复(GWServer)
	// 玩家摆摊状态下不能创建
	if ( cre->m_core.Lev < UNION_CREATE_USE_LEVEL )						return CMD_ERROR_UNKNOWN;
	if ( cre->m_pItemContainer->GetMoney() < UNION_CREATE_USE_MONEY )	return CMD_ERROR_UNKNOWN;
	/*if ( !cre->m_pItemContainer->m_Bag.FindType(UNION_CREATE_USE_ITEMID) && !cre->m_pItemContainer->m_MissionBag.FindType(UNION_CREATE_USE_ITEMID))
        return CMD_ERROR_UNKNOWN;*/
	if ( cre->m_pTrade || cre->m_pPShop)
		return CMD_ERROR_UNKNOWN;
	if ( cre->m_pItemContainer->IsOperateLocked())
		return CMD_ERROR_NONE;

		if (npcID != 65535)
		{
			CRegionCreature *pNpc = cre->m_scene->FindCreature(npcID);
			SAFE_ASSERT(pNpc)
				if (cre->Distance(pNpc->m_pos) > 240)
				{
					return CMD_ERROR_UNKNOWN;
				}
				if (pNpc->m_npcInfo)
				{
					//ldr123 重点分析是否有用
					////if (pNpc->m_npcInfo->Faction != cre->m_core.Faction)
					//{
					//	CUnionCmdBuilder_Svr::Build_r2c_union_create_result(UNION_NOT_IN_SAME_FACTION, 0, "");
					//	cre->RecvCmd(g_sendCmd);
					//	return CMD_ERROR_NONE;
					//}
					////if (!NpcHaveFunction(pNpc->m_npcInfo, NF_CreateUnion))
					////{
					////	CUnionCmdBuilder_Svr::Build_r2c_union_create_result(UNION_ERROR_UNKNOWN, 0, "");
					////	cre->RecvCmd(g_sendCmd);
					////	return CMD_ERROR_NONE;
					////}
				}

		}

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_create);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteString(name);
	packet.WriteString(enounce);
	g_region->m_gws.EndSend();

	cre->m_pItemContainer->LockOperate(true);

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_dismiss_other)
{
	long unionID;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )

	// CHECK UP
	// 双方均在同一诸侯国(GWServer)
	// 必须有权限(Client,GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_dismiss_other);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteLong(playerDBID);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_change_nickname)
{
	long unionID;
	long playerDBID;
	char *nickname;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadString(&nickname) )

	if (strlen(nickname) > 20)
		return CMD_ERROR_NONE;
	if (!g_region->UserUploadString_IsValid(nickname))
		return CMD_ERROR_NONE;

	// CHECK UP
	// 名字长度合法(GWServer)
	// 双方均在同一诸侯国(GWServer)
	// 授予者必须有权限(GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_change_nickname);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteLong(playerDBID);
	packet.WriteString(nickname);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_join)
{
	long unionID;
	long playerID;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerID) )

	// CHECK UP
	// 申请人和被申请人必须是玩家(RegionServer)
	// 被申请人不属于任何诸侯国(Client,GWServer)
	// 申请人属于一个诸侯国(Client,GWServer)
	// 申请人必须有权限(GWServer)
	// 双方必须在同一阵营
	// 被申请人级别不低于10级
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())
	CRegionCreature *pPlayer = cre->m_scene->FindCreature(playerID);
	SAFE_ASSERT( pPlayer && pPlayer->IsUser() )
// 	SAFE_ASSERT( pPlayer->m_core.Faction == cre->m_core.Faction )
// 
// 	if(pPlayer->m_core.Lev < 10)
// 	{
// 		CmdGiver->SendSystemMessage(R(TYRO_LIMIT_UNION));
// 		return CMD_ERROR_NONE;
// 	}

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_join);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteLong(pPlayer->m_userInfo->m_userId);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_other_join_response)
{
	long unionID;
	long playerDBID;
	char bAgree;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadByte(&bAgree) )
	
	// CHECK UP
	// 申请人和被申请人必须是玩家(RegionServer)
	// 被申请人不属于任何诸侯国(GWServer)
	// 申请人属于一个诸侯国(GWServer)
	// 申请人必须有权限(GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())
	CRegionUser* pPlayer = g_region->FindUser(playerDBID);
	SAFE_ASSERT( pPlayer && pPlayer->m_dummy )

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_other_join_response);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteLong(pPlayer->m_userId);
	packet.WriteByte(bAgree);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_bulletin)
{
	long unionID;
	char* bulletin;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadString(&bulletin) )

	if (strlen(bulletin) > UNION_BULLETIN_STRING_MAX)
		return CMD_ERROR_NONE;
// 	if (!g_region->UserUploadString_IsValid(bulletin))
// 		return CMD_ERROR_NONE;

	// CHECK UP
	// 必须是玩家(RegionServer)
	// 必须有权限(GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_bulletin);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteString(bulletin);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_appoint)
{
	long unionID;
	long playerDBID;
	char official;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadByte(&official) )

	// CHECK UP
	// 官职有效(Client, GWServer)
	// 双方都属于一个诸侯国，并且在同一诸侯国(Client, GWServer)
	// 必须有权限(Client, GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_appoint);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteLong(playerDBID);
	packet.WriteByte(official);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_invitewar)
{
	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_surrender)
{
	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_leave)
{
	long unionID;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )		// 接班人

	// CHECK
	// 玩家必须属于一个诸侯国(Client, GWServer)
	// 若有接班人，离开者必须是诸侯(GWServer)
	// 若有接班人，接班人必须是本诸侯人(GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_leave);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteLong(playerDBID);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_data_query)
{
	long unionID;
	SAFE_READ(cmd->ReadLong(&unionID) )

	// CHECK
	// 玩家必须属于这个诸侯国(Client, GWServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_data_query);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_logo)
{
	int dataLen;
	char* data;
	SAFE_READ(cmd->ReadBinary(&data, &dataLen))

	// CHECK
	// 玩家是诸侯(GWServer)
	// 数据格式合法(RegionServer)
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser() && cre->m_userInfo && cre->m_unionID!=0)

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_logo);
	packet.WriteBinary(data, dataLen);
	packet.WriteLong(cre->m_userInfo->m_userId);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_query_logo_data)
{
	char* fileName;
	SAFE_READ(cmd->ReadString(&fileName))

	if (strlen(fileName) > 10)
		return CMD_ERROR_NONE;
	if (!g_region->UserUploadString_IsValid(fileName))
		return CMD_ERROR_NONE;

	CUnionIconMgr::SUnionIconData *pIconData = NULL;
	pIconData = g_UnionIconMgr.Find(fileName);
	if (pIconData)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_logo_data(fileName, pIconData->data, pIconData->dataLen);
		CmdGiver->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
}

UNION_CMD_FUNCTION(cmd_c2r_union_update_level)
{
	long unionID;
	SAFE_READ(cmd->ReadLong(&unionID) )

	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())
	if ( cre->m_bPShop || cre->m_pTrade)
		return CMD_ERROR_UNKNOWN;

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_update_level);
	packet.WriteLong(cre->m_userInfo->m_userId);
	g_region->m_gws.EndSend();

	cre->m_pItemContainer->LockOperate(true);
	
	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION(cmd_c2r_union_do_vote)
{
	long voteID;
	char cS = 0;
	SAFE_READ(cmd->ReadLong(&voteID) )
	SAFE_READ(cmd->ReadByte(&cS) )
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_do_vote);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteByte(cS);
	
	g_region->m_gws.EndSend();

	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION(cmd_c2r_union_log_query)
{
	long voteID;
	char cS = 0;
	
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_log_query);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(cre->m_unionID);

	g_region->m_gws.EndSend();

	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION(cmd_c2r_union_give_union)
{
	long toID;
	
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	SAFE_READ(cmd->ReadLong(&toID) )
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_give_union);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(toID);
	packet.WriteLong(cre->m_unionID);

	g_region->m_gws.EndSend();

	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION(cmd_c2r_union_join_By_Name)
{
	char* pName = NULL;
	long unionID;
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadString(&pName) )
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_join_By_Name);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(unionID);
	packet.WriteString(pName);

	g_region->m_gws.EndSend();

	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION(cmd_c2r_union_contribution)
{
	char* pName = NULL;
	long unionID;
	long dwMoney;
	long dwItem1;
	long dwItem2;
	long dwItem3;
	long dwItem4;
	
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&dwMoney) )
	SAFE_READ(cmd->ReadLong(&dwItem1) )
	SAFE_READ(cmd->ReadLong(&dwItem2) )
	SAFE_READ(cmd->ReadLong(&dwItem3) )
	SAFE_READ(cmd->ReadLong(&dwItem4) )

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_contribution);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(dwMoney);
	packet.WriteLong(dwItem1);
	packet.WriteLong(dwItem2);
	packet.WriteLong(dwItem3);
	packet.WriteLong(dwItem4);

	g_region->m_gws.EndSend();

	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION(cmd_c2r_union_set_icon)
{
	char* pName = NULL;
	long dwIconID;
	
	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())
	SAFE_READ(cmd->ReadLong(&dwIconID) )
	
	/*SUnionIcon* pIcon = g_TableUnionIcon.FindUnionIcon(dwIconID);
	BOOL_ENSURE(NULL != pIcon)
	if (cre->GetGold() < pIcon->nMoney)
	{
		const int c_nLen = 256;
		char szBuff[c_nLen];
		rt2_snprintf(szBuff, c_nLen, R(MSG_CARD_BUY_MONEY_NOT_ENOUGH), pIcon->nMoney, cre->GetGold());
		cre->SendSystemMessage(szBuff);
		return CMD_ERROR_UNKNOWN;
	}
	cre->m_pItemContainer->RemoveMoney(pIcon->nMoney);*/

	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_union);
	packet.WriteShort(c2r_union_set_icon);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteLong(dwIconID);
	
	g_region->m_gws.EndSend();

	return CMD_ERROR_UNKNOWN;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_create_result)
{
	guard;

	long playerSendToDBID;
	char errorCode;
	long unionID;
	char* unionName;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadString(&unionName) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
        CRegionCreature *pCre = pUser->m_dummy;
        SAFE_ASSERT(pCre)
		pCre->m_pItemContainer->LockOperate(false);
        if (errorCode==UNION_SUCCESS)
        {
            // 检查并处理条件不符合的情况
            if (pCre->m_pItemContainer->GetMoney() < UNION_CREATE_USE_MONEY)
            {
                ERR1("Create Union Fail1(Username=%s)\n", pCre->m_core.Name.c_str());
            }
           /* if ( !pCre->m_pItemContainer->m_Bag.FindType(UNION_CREATE_USE_ITEMID) && !pCre->m_pItemContainer->m_MissionBag.FindType(UNION_CREATE_USE_ITEMID))
            {
                ERR1("Create Union Fail2(Username=%s)\n", pCre->m_core.Name.c_str());
            }*/
            if ( pCre->m_core.Lev < UNION_CREATE_USE_LEVEL )
            {
                ERR1("Create Union Fail3(Username=%s)\n", pCre->m_core.Name.c_str());
            }
			pCre->RemoveGold(UNION_CREATE_USE_MONEY);
            //pCre->m_pItemContainer->RemoveMoney(UNION_CREATE_USE_MONEY);

            //int count, page, gridI, gridJ;
            //if (pCre->m_pItemContainer->m_Bag.FindType(UNION_CREATE_USE_ITEMID, ItemColor_All, &count, &page, &gridI, &gridJ))
            //{
            //    pCre->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
            //}
            //else if (pCre->m_pItemContainer->m_MissionBag.FindType(UNION_CREATE_USE_ITEMID, ItemColor_All, &count, &page, &gridI, &gridJ))
            //{
            //    pCre->m_pItemContainer->m_MissionBag.RemoveItem(page, gridI, gridJ);
            //}
			//pCre->RemoveItem(UNION_CREATE_USE_ITEMID, 1); //PZH

            pCre->m_unionID = unionID;
            pCre->m_unionName = unionName;
            pCre->m_unionNickName = "";
            pCre->m_unionIcon = UNION_DEFAULT_ICON_NAME;
            pCre->m_modelRef++;

			// log到数据库
			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_log_event);
			packet.WriteByte(PLT_UnionOperate);
			packet.WriteLong(pCre->m_userInfo->m_userId);
			packet.WriteString((char*)pCre->m_userInfo->m_userIpAddress.c_str()); // userIP
			packet.WriteLong(1);
			packet.WriteString(unionName);
			packet.WriteLong(1);
			packet.WriteString((char*)pCre->m_core.Name.c_str());;
			g_region->m_gws.EndSend();
        }

		CUnionCmdBuilder_Svr::Build_r2c_union_create_result(errorCode, unionID, unionName);
		pCre->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_update_level_result)
{
	guard;

	long playerSendToDBID;
	char errorCode;
	long unionID;
	char unionLevel;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadByte(&unionLevel) )

	LockUnionOperate(unionID, false);

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CRegionCreature *pCre = pUser->m_dummy;
		SAFE_ASSERT(pCre)
		pCre->m_pItemContainer->LockOperate(false);

		if (errorCode==UNION_SUCCESS)
		{
			SUnionData* pUnionData = g_region->FindUnion(unionID);
			if (pUnionData)
			{
				// log到数据库
				CG_CmdPacket& packet = g_region->m_gws.BeginSend();
				packet.WriteShort(r2g_log_event);
				packet.WriteByte(PLT_UnionOperate);
				packet.WriteLong(pCre->m_userInfo->m_userId);
				packet.WriteString((char*)pCre->m_userInfo->m_userIpAddress.c_str()); // userIP
				packet.WriteLong(2);
				packet.WriteString((char*)pUnionData->m_strName.c_str());
// 				packet.WriteLong(pUnionData->level);
				packet.WriteString((char*)pCre->m_core.Name.c_str());;
				g_region->m_gws.EndSend();
			}
		}

		CUnionCmdBuilder_Svr::Build_r2c_union_update_level_result(errorCode);
		pCre->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_score_change)
{
	guard;

	long unionID;
	long score;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&score) )

	// 更新Region数据
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iterUnion = g_region->m_unionMap.find(unionID);
	if (iterUnion==g_region->m_unionMap.end())
		return CMD_ERROR_UNKNOWN;
 	iterUnion->second.m_nFighting = score;

	// 更新Client数据
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
	for (; iterUser!=g_region->m_userMap.end(); iterUser++)
	{
		CRegionUser* user = (*iterUser).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_score_change(score);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_levelup)
{
 	guard;
// 	long unionID;
// 	long unionLev;
// 	SAFE_READ(cmd->ReadLong(&unionID) )
// 	SAFE_READ(cmd->ReadLong(&unionLev) )
// 
// 	// 更新Region
// 	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iterUnion = g_region->m_unionMap.find(unionID);
// 	if (iterUnion==g_region->m_unionMap.end())
// 	{
// 		ERR("Recv cmd_r2c_union_levelup, But region have no this union!\n");
// 		return CMD_ERROR_UNKNOWN;
// 	}
// 	else 
// 	{
// // 		SUnionData &u = iterUnion->second;
// // 		u.level = unionLev;
// 	}
// 
// 	// 更新Client
// 	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
// 	for (; iterUser!=g_region->m_userMap.end(); iterUser++)
// 	{
// 		CRegionUser* user = (*iterUser).second;
// 		CRegionCreature* tmp;
// 		if(!user->Session())	continue;
// 		tmp = user->m_dummy;
// 		if(!tmp)				continue;
// 		if(!tmp->m_scene)		continue;
// 		if(tmp->m_unionID!=unionID) continue; 
// 
// 		CUnionCmdBuilder_Svr::Build_r2c_union_levelup();
// 		user->RecvCmd(g_sendCmd);
// 	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_player_levelup)
{
	guard;
	long unionID;
	long playerID;
	short playerLevel;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerID) )
	SAFE_READ(cmd->ReadShort(&playerLevel) )

	// 更新Region
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iterUnion = g_region->m_unionMap.find(unionID);
	if (iterUnion==g_region->m_unionMap.end())
	{
		ERR1("Recv cmd_r2c_union_player_levelup, But region have no this union!(UnionID=%d)\n", unionID);
		return CMD_ERROR_UNKNOWN;
	}
	else 
	{
		SUnionData &u = iterUnion->second;
		SUnionCitizen *pCitizen = u.Find(playerID);
		if (!pCitizen)
		{
			ERR1("[cmd_r2c_union_player_levelup] can NOT find player (id=%d) \n", playerID);
			return CMD_ERROR_UNKNOWN;
		}
// 		pCitizen->level = playerLevel;
	}

	// 更新Client
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
	for (; iterUser!=g_region->m_userMap.end(); iterUser++)
	{
		CRegionUser* user = (*iterUser).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_player_levelup(playerID, playerLevel);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_dismiss_other_result)
{
	guard;

	long playerSendToDBID;
	char errorCode;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_dismiss_other_result(errorCode, playerDBID);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	if (errorCode==UNION_SUCCESS)
	{
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_dismissed_by_other)
{
	guard;

	long playerSendToDBID;
	char *unionName;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadString(&unionName) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_dismissed_by_other(playerDBID, unionName);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	if (pUser && pUser->m_dummy)
	{
		pUser->m_dummy->m_unionID = 0;
		pUser->m_dummy->m_unionName = "";
		pUser->m_dummy->m_unionNickName = "";
		pUser->m_dummy->m_unionIcon = "";
		pUser->m_dummy->m_modelRef++;
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_change_nickname_result)
{
	guard;

	long playerSendToDBID;
	long playerDBID;
	char errorCode;
	char* newNickname;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadString(&newNickname) )
	
	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_change_nickname_result(errorCode, playerDBID, newNickname);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_other_join_apply)
{
	guard;

	long playerSendToDBID;
	long playerDBID;
	char* playerName;
	char* szUnionName;

	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadString(&playerName) )
	SAFE_READ(cmd->ReadString(&szUnionName) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_other_join_apply(playerDBID, playerName, szUnionName);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_join_response)
{
	guard;

	long playerSendToDBID;
	long playerDBID;
	char errorCode;
	char *unionName, *playerName;
	long unionID;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadString(&unionName) )
	SAFE_READ(cmd->ReadString(&playerName) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_join_response(errorCode, playerDBID, unionID, unionName, playerName);
		pUser->m_dummy->RecvCmd(g_sendCmd);

		if (errorCode==UNION_SUCCESS)
		{
			CRegionUser *pUser1 = g_region->FindUser(playerDBID);
			if (pUser1 && pUser1->m_dummy)
			{
				pUser1->m_dummy->m_unionID = unionID;
				pUser1->m_dummy->m_unionName = unionName;
				pUser1->m_dummy->m_unionNickName = "";
				pUser1->m_dummy->m_modelRef++;
			}
		}
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_appoint_result)
{
	guard;

	long playerSendToDBID;
	char errorCode;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_appoint_result(errorCode);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_invitewar_result)
{
	guard;

	long playerSendToDBID;
	char errorCode;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_invitewar_result(errorCode);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_surrender_other_response)
{
	guard;

	long playerSendToDBID;
	char bAgree;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&bAgree) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_surrender_other_response(bAgree);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_leave_result)
{
	guard;

	long playerSendToDBID;
	char errorCode;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadByte(&errorCode) )

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_leave_result(errorCode);
		pUser->m_dummy->RecvCmd(g_sendCmd);

		if (errorCode==UNION_SUCCESS)
		{
			g_RegionCastleWarMgr.OnUnionLeave(pUser->m_dummy->m_unionID);
			pUser->m_dummy->m_unionID = 0;
			pUser->m_dummy->m_unionName = "";
			pUser->m_dummy->m_unionNickName = "";
			pUser->m_dummy->m_unionIcon = "";
			pUser->m_dummy->m_modelRef++;
		}
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_data_refresh)
{
	guard;
	long playerSendToDBID;
	SUnionData unionData;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_ASSERT( unionData.Serialize(cmd) )

	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter = g_region->m_unionMap.find(unionData.m_nID);
	if (iter!=g_region->m_unionMap.end())
	{
		SUnionData &u = iter->second;
		u = unionData;
	}
	else 
	{
		g_region->m_unionMap[unionData.m_nID] = unionData;
	}

	CRegionUser *pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_data_refresh(&unionData);
		pUser->m_dummy->RecvCmd(g_sendCmd);

		pUser->m_dummy->m_unionID = unionData.m_nID;
		pUser->m_dummy->m_unionName = unionData.m_strName;
// 		pUser->m_dummy->m_unionIcon = unionData.icon;
		SUnionCitizen *pCitizen = unionData.Find(playerSendToDBID);
		if (pCitizen)
		{
            pUser->m_dummy->m_unionNickName = pCitizen->m_strNickName;
		}
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_join)
{
	guard;

	long unionID;
	SUnionCitizen citizen;
	SAFE_READ(cmd->ReadLong(&unionID))
	SAFE_ASSERT(citizen.Serialize(cmd) )

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_citizen_join(&citizen);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_leave)
{
	guard;

	long unionID;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_citizen_leave(playerDBID);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_online)
{
	guard;

	long unionID;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )

//-------------add start by tony 05.06.15---------------------------------//

	/*@@g_RegionCastleWarMgr.OnUserOnline(unionID, playerDBID);@@*/

//-------------add start by tony 05.06.15---------------------------------//

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_citizen_online(playerDBID);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_offline)
{
	guard;

	long unionID;
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_citizen_offline(playerDBID);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_official_change)
{
	guard;

	long unionID;
	long playerDBID;
	char official;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadByte(&official) )

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_citizen_official_change(playerDBID, official);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_disband)
{
	guard;

	char reason;
	long unionID;
	char *unionName;
	SAFE_READ(cmd->ReadLong(&unionID))
	SAFE_READ(cmd->ReadString(&unionName) )
	SAFE_READ(cmd->ReadByte(&reason) )

	g_RegionCastleWarMgr.OnUnionDisband(unionID);

	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter = g_region->m_unionMap.find(unionID);
	if (iter!=g_region->m_unionMap.end())
	{
		g_region->m_unionMap.erase(iter);
	}

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_disband(reason, unionName);
		user->RecvCmd(g_sendCmd);

		tmp->m_unionID = 0;
		tmp->m_unionName = "";
		tmp->m_unionNickName = "";
		tmp->m_unionIcon = "";
		tmp->m_modelRef ++;
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_new_bulletin)
{
	guard;

	long unionID;
	char* newBulletin;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadString(&newBulletin) )

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_new_bulletin(newBulletin);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_citizen_nickname_change)
{
	guard;

	long unionID;
	long playerDBID;
	char* nickname;
	SAFE_READ(cmd->ReadLong(&unionID) )
	SAFE_READ(cmd->ReadLong(&playerDBID) )
	SAFE_READ(cmd->ReadString(&nickname) )

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		CUnionCmdBuilder_Svr::Build_r2c_union_citizen_nickname_change(playerDBID, nickname);
		user->RecvCmd(g_sendCmd);
	}

	CRegionUser *pCitizen = g_region->FindUser(playerDBID);
	if (pCitizen && pCitizen->m_dummy)
	{
		pCitizen->m_dummy->m_unionNickName = nickname;
		pCitizen->m_dummy->m_modelRef ++;
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_logo_data)
{
	guard;

	long unionID;
	char* fileName;
	long userDBID;
	int dataLen;
	char* data;
	SAFE_READ(cmd->ReadLong(&unionID))
	SAFE_READ(cmd->ReadString(&fileName))
	SAFE_READ(cmd->ReadBinary(&data, &dataLen))
	SAFE_READ(cmd->ReadLong(&userDBID))

	// Save To Memory
	CUnionIconMgr::SUnionIconData IconData;
	IconData.dataLen = dataLen;
	IconData.fileName = fileName;
	IconData.data = RT_NEW char[dataLen];
	memcpy(IconData.data, data, dataLen);
	g_UnionIconMgr.m_arrIcon.push_back(IconData);

	// Response To Client
	CRegionUser *pUser = g_region->FindUser(userDBID);		// userDBID有可能为0, region初始化时此值就为0
	if (pUser && pUser->m_dummy)
	{
		CUnionCmdBuilder_Svr::Build_r2c_union_logo_result(UNION_SUCCESS, fileName);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	// 发送给所有诸侯国成员
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)				continue;
		if(!tmp->m_scene)		continue;
		if(tmp->m_unionID!=unionID) continue; 

		tmp->m_unionIcon = fileName;
		tmp->m_modelRef++;
		CUnionCmdBuilder_Svr::Build_r2c_union_icon_change(fileName);
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_new_vote)
{
	guard;

	long dwMyID;
	long unionID;
	long CreaterDBID;
	long RecallDBID;
	long VoteID;

	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&unionID))
	SAFE_READ(cmd->ReadLong(&CreaterDBID))
	SAFE_READ(cmd->ReadLong(&RecallDBID))
	SAFE_READ(cmd->ReadLong(&VoteID))

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_new_vote);

	g_sendCmd->WriteLong(CreaterDBID);
	g_sendCmd->WriteLong(RecallDBID);
	g_sendCmd->WriteLong(VoteID);

	pCitizen->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_c2r_union_log_query)
{
	guard;

	long dwMyID;
	long dwNum;
	

	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&dwNum))
	

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_log_query_result);

	g_sendCmd->WriteLong(dwNum);
	long i = 0;
	for (i = 0; i < dwNum; i++)
	{
		long LogType, dwTime;
		char* pStr = NULL;

		SAFE_READ(cmd->ReadLong(&LogType))
		SAFE_READ(cmd->ReadLong(&dwTime))
		SAFE_READ(cmd->ReadString(&pStr))

		g_sendCmd->WriteLong(LogType);
		g_sendCmd->WriteLong(dwTime);
		g_sendCmd->WriteString(pStr);
	}

	pCitizen->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_new_log)
{
	guard;

	long dwMyID;
	long type;
	long dwTime;
	char *pChar = NULL;
	

	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&type))
	SAFE_READ(cmd->ReadLong(&dwTime))
	SAFE_READ(cmd->ReadString(&pChar))
		

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_new_log);

	g_sendCmd->WriteLong(type);
	g_sendCmd->WriteLong(dwTime);
	g_sendCmd->WriteString(pChar);

	pCitizen->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_WarScores_Change)
{
	guard;

	long dwMyID;
	long nFighting;


	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&nFighting))
		

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_WarScores_Change);

	g_sendCmd->WriteLong(nFighting);
	pCitizen->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_update_Contribution)
{
	guard;

	long dwMyID;
	long dwWhoID;
	long dwContribution;


	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&dwWhoID))
	SAFE_READ(cmd->ReadLong(&dwContribution))

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}
	if (dwMyID == dwWhoID)
	{
		pCitizen->m_dummy->m_core.UnionRecord = dwContribution;
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_update_Contribution);

	g_sendCmd->WriteLong(dwWhoID);
	g_sendCmd->WriteLong(dwContribution);

	pCitizen->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_contribution)
{
	guard;

	long dwMyID;
	long dwMoney;
	long nItem1;
	long nItem2;
	long nItem3;
	long nItem4;

	long nWho;
	long nGiveMoney;


	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&dwMoney))
	SAFE_READ(cmd->ReadLong(&nItem1))
	SAFE_READ(cmd->ReadLong(&nItem2))
	SAFE_READ(cmd->ReadLong(&nItem3))
	SAFE_READ(cmd->ReadLong(&nItem4))
	SAFE_READ(cmd->ReadLong(&nWho))
	SAFE_READ(cmd->ReadLong(&nGiveMoney))

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}
	
	if (nWho == dwMyID)
	{
		if (NULL != pCitizen->m_dummy)
		{
			pCitizen->m_dummy->AddGold(-nGiveMoney);
		}
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_contribution);

	g_sendCmd->WriteLong(dwMoney);
	g_sendCmd->WriteLong(nItem1);
	g_sendCmd->WriteLong(nItem2);
	g_sendCmd->WriteLong(nItem3);
	g_sendCmd->WriteLong(nItem4);

	pCitizen->RecvCmd(g_sendCmd);

	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_r2c_union_set_icon)
{
	guard;

	long dwMyID;
	long dwID;
	long dwMoney;


	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&dwID))
	SAFE_READ(cmd->ReadLong(&dwMoney))
		

	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}

	if (NULL != pCitizen->m_dummy)
	{
		std::ostringstream os;
		os<<dwID;
		pCitizen->m_dummy->m_unionIcon = os.str();
	}
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_set_icon);

	g_sendCmd->WriteLong(dwID);
	g_sendCmd->WriteLong(dwMoney);

	pCitizen->RecvCmd(g_sendCmd);
	
	return CMD_ERROR_NONE;
	unguard;
}

UNION_CMD_FUNCTION_FROM_GW(cmd_g2r_set_union_icon_result)
{
	long dwMyID;
	long dwID;
	long dwRet;



	SAFE_READ(cmd->ReadLong(&dwMyID))
	SAFE_READ(cmd->ReadLong(&dwID))
	SAFE_READ(cmd->ReadLong(&dwRet))


	CRegionUser *pCitizen = g_region->FindUser(dwMyID);
	if(pCitizen==NULL )
	{
		return CMD_ERROR_NONE;
	}
	if (NULL == pCitizen->m_dummy)
	{
		return CMD_ERROR_NONE;
	}
	SUnionIcon* pIcon = g_TableUnionIcon.FindUnionIcon(dwID);
	BOOL_ENSURE(NULL != pIcon)
	
	if (0 == dwRet)
	{
		CItemCmdBuilder_Svr::Build_r2c_bag_addmoney(pIcon->nMoney, ITEM_CONTAINER_PLAYER_COMMON_BAG, ITEM_REASON_PICK);
		pCitizen->m_dummy->RecvCmd(g_sendCmd);
	}
	else
	{
		pCitizen->m_dummy->m_pItemContainer->AddMoney(pIcon->nMoney);
	}
}

void CUnionCmdBuilder_Svr::Build_r2c_union_create_result(BYTE errorCode, DWORD unionID, char const* unionName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_create_result);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteLong(unionID);
	g_sendCmd->WriteString(unionName);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_update_level_result(BYTE errorCode)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_update_level_result);
	g_sendCmd->WriteByte(errorCode);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_dismiss_other_result(BYTE errorCode, DWORD playerID)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_dismiss_other_result);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteLong(playerID);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_dismissed_by_other(DWORD playerID, char const* unionName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_dismissed_by_other);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteString(unionName);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_change_nickname_result(BYTE errorCode, DWORD playerID, char const* newNickname)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_change_nickname_result);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteString(newNickname);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_other_join_apply(DWORD playerID, char const* playerName, const char* szUnionName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_other_join_apply);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteString(playerName);
	g_sendCmd->WriteString(szUnionName);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_join_response(BYTE errorCode, DWORD playerID, DWORD unionID, char const* unionName, char const* playerName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_join_response);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteLong(unionID);
	g_sendCmd->WriteString(unionName);
	g_sendCmd->WriteString(playerName);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_appoint_result(BYTE errorCode)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_appoint_result);
	g_sendCmd->WriteByte(errorCode);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_invitewar_result(BYTE errorCode)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_invitewar_result);
	g_sendCmd->WriteByte(errorCode);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_surrender_other_response(BYTE bAgree)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_surrender_other_response);
	g_sendCmd->WriteByte(bAgree);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_leave_result(BYTE errorCode)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_leave_result);
	g_sendCmd->WriteByte(errorCode);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_data_refresh(SUnionData *pUnionData)
{
	CHECK(pUnionData);
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_data_refresh);
	pUnionData->Serialize(g_sendCmd);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_citizen_join(SUnionCitizen *pCitizen)
{
	CHECK(pCitizen);
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_citizen_join);
	pCitizen->Serialize(g_sendCmd);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_citizen_leave(DWORD playerID)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_citizen_leave);
	g_sendCmd->WriteLong(playerID);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_citizen_online(DWORD playerID)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_citizen_online);
	g_sendCmd->WriteLong(playerID);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_citizen_offline(DWORD playerID)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_citizen_offline);
	g_sendCmd->WriteLong(playerID);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_citizen_official_change(DWORD playerID, BYTE official)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_citizen_official_change);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteByte(official);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_disband(short reason, char const* unionName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_disband);
	g_sendCmd->WriteShort(reason);
	g_sendCmd->WriteString(unionName);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_new_bulletin(char const* NewBulletin)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_new_bulletin);
	g_sendCmd->WriteString(NewBulletin);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_citizen_nickname_change(DWORD playerID, char const* nickname)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_citizen_nickname_change);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteString(nickname);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_score_change(long score)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_score_change);
	g_sendCmd->WriteLong(score);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_levelup()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_levelup);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_player_levelup(long playerID, short playerLevel)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_player_levelup);
	g_sendCmd->WriteLong(playerID);
	g_sendCmd->WriteShort(playerLevel);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_logo_result(char errorCode, char const* fileNewName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_logo_result);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteString(fileNewName);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_logo_data(char const* fileName, char const* data, int dataSize)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_logo_data);
	g_sendCmd->WriteString(fileName);
	g_sendCmd->WriteBinary(data, dataSize);
}

void CUnionCmdBuilder_Svr::Build_r2c_union_icon_change(char const* UnionIcon)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_union_icon_change);
	g_sendCmd->WriteString(UnionIcon);
}
