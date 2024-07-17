#include "gw_server.h"
#include "item_base.h"
#include "gw_union.h"
#include "union_net_define.h"
#include "gw_cmd_union.h"
#include <sstream>
#include "game_team_share_const.h"

#define SAFE_READ(p)	{ if(!(p)) return eInvalidData; }
#define SAFE_ASSERT(p)  { if ( !(p) ) return eError; }

int GW_UnionCmdProcess(CG_CmdPacket *packet)
{
	short union_cmd_type;
	if ( !packet->ReadShort(&union_cmd_type) )		return eInvalidData;

	switch(union_cmd_type)
	{
	case c2r_union_create:
		return cmd_c2r_union_create(packet);
	case c2r_union_dismiss_other:
		return cmd_c2r_union_dismiss_other(packet);
	case c2r_union_change_nickname:
		return cmd_c2r_union_change_nickname(packet);
	case c2r_union_join:
		return cmd_c2r_union_join(packet);
	case c2r_union_other_join_response:
		return cmd_c2r_union_other_join_response(packet);
	case c2r_union_bulletin:
		return cmd_c2r_union_bulletin(packet);
	case c2r_union_appoint:
		return cmd_c2r_union_appoint(packet);
	case c2r_union_invitewar:
		return cmd_c2r_union_invitewar(packet);
	case c2r_union_surrender:
		return cmd_c2r_union_surrender(packet);
	case c2r_union_leave:
		return cmd_c2r_union_leave(packet);
	case c2r_union_data_query:
		return cmd_c2r_union_data_query(packet);
	case c2r_union_logo:
		return cmd_c2r_union_logo(packet);
	case c2r_union_update_level:
		return cmd_c2r_union_update_level(packet);
	case r2g_union_add_score:
		return cmd_r2g_union_add_score(packet);
	case c2r_union_player_levelup:
		return cmd_c2r_union_player_levelup(packet);
	case c2r_union_do_vote:
		{
			long playerSend;
			long dwVoteID;
			char cS;
			SAFE_READ(packet->ReadLong(&playerSend) )
			SAFE_READ(packet->ReadLong(&dwVoteID) )
			SAFE_READ(packet->ReadByte(&cS) )
			CVoteMgr::GetMgr().DoVote(playerSend, dwVoteID, cS);
		}
		return eOkay;
	case c2r_union_log_query:
		{
			long playerSend;
			long dwUnionID;
			
			SAFE_READ(packet->ReadLong(&playerSend) )
			SAFE_READ(packet->ReadLong(&dwUnionID) )
				
			GetUnionMgr()->QueryLog(dwUnionID, playerSend);
		}
		return eOkay;
	case c2r_union_give_union:
		{
			long playerSend;
			long dwTo;
			long dwUnionID;

			SAFE_READ(packet->ReadLong(&playerSend) )
			SAFE_READ(packet->ReadLong(&dwTo) )
			SAFE_READ(packet->ReadLong(&dwUnionID) )
			GetUnionMgr()->GiveUnion(dwUnionID, playerSend, dwTo);
		}
		return eOkay;
	case c2r_union_join_By_Name:
		return cmd_c2r_union_join_By_Name(packet);
	case r2g_union_update_Contribution:
		return cmd_r2g_union_update_Contribution(packet);
	case r2g_WarScores_Change:
		return cmd_r2g_WarScores_Change(packet);
	case c2r_union_contribution:
		return cmd_c2r_union_contribution(packet);
	case c2r_union_set_icon:
		return cmd_c2r_union_set_icon(packet);
	default:
		return eInvalidData;
	}
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_create)
{
	long playerSend;
	char *name, *enounce;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadString(&name) )
	SAFE_READ(packet->ReadString(&enounce) )

	// CHECK 
	// 玩家不属于任何诸侯国(Client,GWServer)
	// 玩家有足够的等级和金钱(Client,RegionSvr)
	// 诸侯国名字和宣言合法(Client,GWServer)
	// 诸侯国名字不可重复(GWServer)
	GWUser *pUser = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUser && pUser->unionID==0)
//	if (strlen(name)==0 || strlen(name)>UNION_NAME_STRING_MAX)
	if (!g_server.m_database.IsValidName(name))
	{
		CUnionCmdHelper_GW::Send_r2c_union_create_result(playerSend, UNION_NAME_INVALID, 0, "");
		return eError;
	}
	else if (strlen(enounce)>UNION_ENOUNCE_STRING_MAX)
	{
		CUnionCmdHelper_GW::Send_r2c_union_create_result(playerSend, UNION_ERROR_UNKNOWN, 0, "");
		return eError;
	}
	else if (pUser->dwLastDelUnion!=0)
	{
		DWORD dwNow = time(NULL);
		if (dwNow-pUser->dwLastDelUnion < 7*24*3600)
		{
			CUnionCmdHelper_GW::Send_r2c_union_create_result(playerSend, UNION_CAPTION_LOCK_TIME, 0, "");
			return eOkay;
		}
	}
	DWORD unionID = GetUnionMgr()->CreateUnion(playerSend, pUser->attr.level, name, enounce);
	if ( unionID==0 )
	{
		CUnionCmdHelper_GW::Send_r2c_union_create_result(playerSend, UNION_ERROR_UNKNOWN, 0, "");
		return eError;
	}
	
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion)
    SUnionData *pUnionData = pUnion->GetData();
    SAFE_ASSERT(pUnionData)

	CUnionCmdHelper_GW::Send_r2c_union_create_result(playerSend, UNION_SUCCESS, unionID, (char*)pUnion->GetName().c_str());
	LOG1("Create Union [%s] Success!\n", name);
    //CUnionCmdHelper_GW::Send_r2c_union_data_refresh(playerSend, pUnionData);
	CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(playerSend, pUnionData);
	std::ostringstream os;
	os<<pUser->name<<" "<<pUnion->GetName();
	pUnion->AddLog(CUnion::eCreate, os.str().c_str());
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_dismiss_other)
{
	long playerSend;
	long unionID;
	long playerDismissed;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadLong(&playerDismissed) )

	// CHECK UP
	// 双方均在同一诸侯国(GWServer)
	// 必须有权限(Client,GWServer)
	GWUser* pUserDismiss = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserDismiss)
	unionID = pUserDismiss->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion)
	SUnionCitizen *pDismiss = pUnion->Find(playerSend);
	SUnionCitizen *pDismissed = pUnion->Find(playerDismissed);
	SAFE_ASSERT( pDismissed && pDismiss );
//ldr123
	std::ostringstream os;
	os<<pDismiss->m_strPlayerName<<" "<<pDismissed->m_strPlayerName;
	SAFE_ASSERT(Union_GetEnableFromAuction(UA_BANISHMENT, pDismiss, pDismissed));
// 	SAFE_ASSERT( Union_CanDismiss(pDismiss, pDismissed) )
	//PZH
	/*if (pDismissed->m_eUnionOfficial < UO_SubLeader)
	{
		CVoteMgr::GetMgr().CretaeVote(playerSend, playerDismissed);
	}
	else
	{*/
		SAFE_ASSERT( pUnion->RemoveCitizen(playerDismissed) )
		pUnion->AddLog(CUnion::eBanish, os.str().c_str());
		pDismissed = NULL;
		CUnionCmdHelper_GW::Send_r2c_union_dismiss_other_result(playerSend, UNION_SUCCESS, playerDismissed);
		CUnionCmdHelper_GW::Send_r2c_union_dismissed_by_other(playerDismissed, playerSend, (char*)pUnion->GetName().c_str());
		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_leave(pUnion, playerDismissed);
		CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(0, pUnion->GetData());		
	//}
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_change_nickname)
{
//ldr123 未用
// 	long playerSend;
// 	long unionID;
// 	long playerID;
// 	char *nickname;
// 	SAFE_READ(packet->ReadLong(&playerSend) )
// 	SAFE_READ(packet->ReadLong(&unionID) )
// 	SAFE_READ(packet->ReadLong(&playerID) )
// 	SAFE_READ(packet->ReadString(&nickname) )
// 
// 	// CHECK UP
// 	// 名字长度合法(GWServer)
// 	// 双方均在同一诸侯国(GWServer)
// 	// 授予者必须有权限(GWServer)
// 	GWUser* pUserSend = g_server.FindActorById(playerSend);
// 	SAFE_ASSERT(pUserSend)
// 	int nNameLen = (int)strlen(nickname);
// 	SAFE_ASSERT( nNameLen>=0 && nNameLen<=UNION_NAME_STRING_MAX )
// 	unionID = pUserSend->unionID;
// 	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
// 	SAFE_ASSERT(pUnion)
// 	SUnionCitizen *pCitizen1 = pUnion->Find(playerSend);
// 	SUnionCitizen *pCitizen2 = pUnion->Find(playerID);
// 	SAFE_ASSERT( pCitizen1 && pCitizen2 )
// 	SAFE_ASSERT( Union_CanChangeNickname(pCitizen1, pCitizen2) )
// 
// 	pUnion->SetCitizenNickName(playerID, nickname);
// 	CUnionCmdHelper_GW::Send_r2c_union_change_nickname_result(playerSend, UNION_SUCCESS, playerID, nickname);
// 	CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_nickname_change(pUnion, playerID, (char*)pCitizen2->m_strNickName.c_str());

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_join_By_Name)
{
	char* pName = NULL;
	long playerSend;
	long unionID;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )

	SAFE_READ(packet->ReadString(&pName) )

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	GWUser* pUser = g_server.FindActorByName(pName);
	SAFE_ASSERT( pUserSend)

	if (NULL == pUser)
	{
		CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_team);
		rPacket.WriteLong(playerSend);
		rPacket.WriteByte(GW2R_TEAM_PICKER);
		rPacket.WriteString(R(OBJECT_NOT_IN_REGION));

		int Session= g_server.GetRegionSession(pUserSend->regionID);
		if(Session !=-1)
		{
			g_server.m_region.SendToPeer(Session);
		}
		return eOkay;
	}

	if (pUser->unionID!=0)		// 被申请人已经属于一个诸侯国
	{
		return eOkay;
	}
	if (pUserSend->unionID==0)	// 申请人不属于任何诸侯国
	{
		return eOkay;
	}

//ldr123
	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion);
	SUnionCitizen *pCitizen = pUnion->Find(playerSend);
	SAFE_ASSERT(Union_GetEnableFromAuction(UA_INVITEMEMBER, pCitizen));
// 
// 	SAFE_ASSERT(Union_CanInvite(pCitizen))
// 
	CUnionCmdHelper_GW::Send_r2c_union_other_join_apply(pUser->userID, playerSend, (char*)pUserSend->name.c_str(), pUnion->GetName().c_str());
	CUnionCmdHelper_GW::Send_r2c_msg_text(playerSend, R(MSG_UNION_SENDMESSAGESUCCESS));
	
// 	pUnion->AddLog(CUnion::eJobin, pCitizen->m_strPlayerName.c_str());
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_r2g_union_update_Contribution)
{
	char* pName = NULL;
	long playerSend;
	long dwContribution;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&dwContribution) )

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend)

	GetUnionMgr()->UpdateContribution(pUserSend, dwContribution);
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_r2g_WarScores_Change)
{
	long playerSend;
	long dwOldV;
	long dwV;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&dwOldV) )
	SAFE_READ(packet->ReadLong(&dwV) )

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend)
	pUserSend->WarScores = dwV;
	GetUnionMgr()->AddWarScores(pUserSend->unionID, (dwV - dwOldV));
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_contribution)
{
	long playerSend;
	long dwMoney;
	long dwItem1;
	long dwItem2;
	long dwItem3;
	long dwItem4;

	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&dwMoney) )
	SAFE_READ(packet->ReadLong(&dwItem1) )
	SAFE_READ(packet->ReadLong(&dwItem2) )
	SAFE_READ(packet->ReadLong(&dwItem3) )
	SAFE_READ(packet->ReadLong(&dwItem4) )

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend)
	
	GetUnionMgr()->DoContribution(pUserSend->unionID, playerSend, dwMoney, dwItem1, dwItem2, dwItem3, dwItem4);

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_set_icon)
{
	long playerSend;
	long dwIconID;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&dwIconID) )

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend)

	GetUnionMgr()->SetIcon(pUserSend, dwIconID);

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_join)
{
	long playerSend;
	long unionID;
	long playerID;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadLong(&playerID) )

	// CHECK UP
	// 申请人和被申请人必须是玩家(RegionServer)
	// 被申请人不属于任何诸侯国(Client,GWServer)
	// 申请人属于一个诸侯国(Client,GWServer)
	// 申请人必须有权限(GWServer)
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	GWUser* pUser = g_server.FindActorById(playerID);
	SAFE_ASSERT( pUserSend && pUser )
	if (pUser->unionID!=0)		// 被申请人已经属于一个诸侯国
	{
		return eOkay;
	}
	if (pUserSend->unionID==0)	// 申请人不属于任何诸侯国
	{
		return eOkay;
	}

	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion)
	SUnionCitizen *pCitizen = pUnion->Find(playerSend);
	SAFE_ASSERT(Union_GetEnableFromAuction(UA_INVITEMEMBER, pCitizen));
// 	SAFE_ASSERT(Union_CanInvite(pCitizen))

	CUnionCmdHelper_GW::Send_r2c_union_other_join_apply(playerID, playerSend, (char*)pUserSend->name.c_str(), pUnion->GetName().c_str());
	
// 	pUnion->AddLog(CUnion::eJobin, pCitizen->m_strPlayerName.c_str());
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_other_join_response)
{
	long playerSend;
	long unionID;
	long playerID;
	char bAgree;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadLong(&playerID) )
	SAFE_READ(packet->ReadByte(&bAgree) )
	
	// CHECK UP
	// 申请人和被申请人必须是玩家(RegionServer)
	// 被申请人不属于任何诸侯国(GWServer)
	// 申请人属于一个诸侯国(GWServer)
	// 申请人必须有权限(GWServer)
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	GWUser* pUser = g_server.FindActorById(playerID);
	SAFE_ASSERT( pUserSend && pUser )
	SAFE_ASSERT(pUser->unionID!=0)
	SAFE_ASSERT(pUserSend->unionID==0)
 
	unionID = pUser->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion)
	SUnionCitizen *pCitizenInvite = pUnion->Find(playerID);
	SAFE_ASSERT(Union_GetEnableFromAuction(UA_INVITEMEMBER, pCitizenInvite));
// 	SAFE_ASSERT(Union_CanInvite(pCitizenInvite))

	if (bAgree)
	{
		if ( !pUnion->AddCitizen(playerSend, pUserSend->attr.level, pUserSend->attr.metier) )
		{
			CUnionCmdHelper_GW::Send_r2c_union_join_response(pUser->userID, UNION_ERROR_UNKNOWN, 0, 0, "", "");
			return eOkay;
		}

		SUnionCitizen *pPlayerUnion = pUnion->Find(playerSend);
		if (!pPlayerUnion)
		{
			return eOkay;
		}
		pUnion->AddLog(CUnion::eJobin, pPlayerUnion->m_strPlayerName.c_str());


		CUnionCmdHelper_GW::Send_r2c_union_join_response(pUser->userID, UNION_SUCCESS, pUserSend->userID, pUnion->GetID(), (char *)pUnion->GetName().c_str(), (char *)pUserSend->name.c_str());
		//CUnionCmdHelper_GW::Send_r2c_union_data_refresh(pUserSend->userID, pUnion->GetData());
		CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(pUserSend->userID, pUnion->GetData());

		SUnionCitizen* pCitizenInvited = pUnion->Find(playerSend);
		SAFE_ASSERT(pCitizenInvited)
		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_join(pUnion, pCitizenInvited);
	}
	else
	{
		CUnionCmdHelper_GW::Send_r2c_union_join_response(pUser->userID, UNION_ERROR_UNKNOWN, 0, 0, "", "");
		return eOkay;
	}

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_bulletin)
{
	long playerSend;
	long unionID;
	char* bulletin;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadString(&bulletin) )

	// CHECK UP
	// 必须是玩家(RegionServer)
	// 必须有权限(GWServer)
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend )
	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SUnionCitizen *pUserData;
	SAFE_ASSERT(pUnion);
	pUserData = pUnion->Find(pUserSend->userID);
	SAFE_ASSERT(pUserData && pUserData->m_eUnionOfficial==UO_King);
	//SAFE_ASSERT( pUnion && pUnion->GetCaption()==pUserSend->userID )
	SAFE_ASSERT(pUnion);
	SUnionCitizen *pUserCitizen = pUnion->Find(pUserSend->userID);
	SAFE_ASSERT(pUserCitizen && pUserCitizen->m_eUnionOfficial==UO_King);

	SAFE_ASSERT( pUnion->SetBulletin(bulletin) )
	CUnionCmdHelper_GW::Broadcast_r2c_union_new_bulletin(pUnion, bulletin);
	
	pUnion->AddLog(CUnion::eChangeBulletin, pUserSend->name.c_str());
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_appoint)
{
	long playerSend;
	long unionID;
	long playerID;
	char official;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadLong(&playerID) )
	SAFE_READ(packet->ReadByte(&official) )

	// CHECK UP
	// 官职有效(Client, GWServer)
	// 双方都属于一个诸侯国，并且在同一诸侯国(Client, GWServer)
	// 必须有权限(Client, GWServer)
	SAFE_ASSERT(UnionOfficial_IsValid((EUnionOfficial)official) && official!=UO_King)
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend )
	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion)
	SUnionCitizen* pAppoint = pUnion->Find(playerSend);
	SUnionCitizen* pAppointed = pUnion->Find(playerID);
	SAFE_ASSERT( pAppointed && pAppoint )
//ldr123
	SAFE_ASSERT(Union_GetEnableFromAuction(UA_LEVELCHANGE, pAppoint, pAppointed));
// 	SAFE_ASSERT( Union_CanAppoint(pUnion->GetData(), pAppoint, pAppointed, (EUnionOfficial)official) )

	EUnionOfficial oFf = pAppointed->m_eUnionOfficial;
	if (pUnion->Appoint(playerID, (EUnionOfficial)official) )
	{
		CUnionCmdHelper_GW::Send_r2c_union_appoint_result(playerSend, UNION_SUCCESS);
		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_official_change(pUnion, playerID, official);

		CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(0, pUnion->GetData());
	}
	CUnion::UnionLogType enType = CUnion::eJobDown;
	EUnionOfficial oCur =  pAppointed->m_eUnionOfficial;
	if (oFf > oCur)
	{
		enType = CUnion::eJobUp;
	}
	std::ostringstream os;
	os<<pAppoint->m_strPlayerName<<" "<<pAppointed->m_strPlayerName<<" "<<oFf<<" "<<oCur;
	pUnion->AddLog(enType, os.str().c_str());
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_invitewar)
{
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_surrender)
{
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_leave)
{
	long playerSend;
	long unionID;
	long playerID;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadLong(&playerID) )		// 接班人

	// CHECK
	// 玩家必须属于一个诸侯国(Client, GWServer)
	// 若有接班人，离开者必须是诸侯(GWServer)
	// 若有接班人，接班人必须是本诸侯人，并且不是自己(GWServer)
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT( pUserSend )
	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT( pUnion )
	SUnionCitizen* pPlayerLeave = pUnion->Find(pUserSend->userID);
	SAFE_ASSERT( pPlayerLeave )
	string playerName = pPlayerLeave->m_strPlayerName;
	GWUser* pUser = NULL;
	SUnionCitizen* pInheritor = NULL;
	if (playerID!=0)
	{
		pInheritor = pUnion->Find(playerID);
		SAFE_ASSERT(pInheritor)
        SAFE_ASSERT(playerID != playerSend)
	}

	CUnionCmdHelper_GW::Send_r2c_union_leave_result(playerSend, UNION_SUCCESS);

// 	if (pUnion->GetCaption() == pPlayerLeave->m_nPlayerID)
	if (pPlayerLeave->m_eUnionOfficial == UO_King)
	{
		// 记录该玩家最后退隐诸侯国的时间
		GWDBCommand cmd;
		rt2_sprintf(cmd.command, "UPDATE `user` SET LastDelUnionTime=FROM_UNIXTIME(%u) WHERE ID=%d", time(NULL), pPlayerLeave->m_nPlayerID);
		if (!g_server.m_database.ExecCommand(cmd.command, NULL))
		{
			ERR("Can NOT update LastDelUnionTime\n");
		}
		pUserSend->dwLastDelUnion = time(NULL);

		// 是否有继承人
		if (pInheritor)
		{
			CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_official_change(pUnion, playerID, UO_King);
//ldr123 需要修改
// 			// 接任 log
// 			g_server.LogUserEvent(PLT_UnionOperate, pInheritor->m_nPlayerID, "", 3, pUnion->GetName().c_str(), pUnion->GetData()->level, pInheritor->m_strPlayerName.c_str(), 0, "");

			pUnion->SetCaptain(pInheritor->m_nPlayerID);
			pUnion->AddLog(CUnion::eLeave, playerName.c_str());
			pUnion->RemoveCitizen(pPlayerLeave->m_nPlayerID);
			pPlayerLeave = NULL;

			CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_leave(pUnion, playerSend);

			CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(0, pUnion->GetData());
		}
		else
		{
			string unionName = pUnion->GetName();
			pUnion->AddLog(CUnion::eLeave, playerName.c_str());
			CUnionCmdHelper_GW::Broadcast_r2c_union_disband(pUnion, UNION_MSG_CAPTAIN_DISBAND, (char*)pUnion->GetName().c_str());
			GetUnionMgr()->DeleteUnion(pUnion->GetID());
			LOG2("A guild's caption leave, so the guild is deleted!(guild=%s,caption=%s)\n", unionName.c_str(), playerName.c_str());
		}
	}
	else
	{
		pUnion->RemoveCitizen(pPlayerLeave->m_nPlayerID);
		pUnion->AddLog(CUnion::eLeave, playerName.c_str());
		pPlayerLeave = NULL;

		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_leave(pUnion, playerSend);

		CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(0, pUnion->GetData());
		
	}
	
	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_data_query)
{
	long playerSend;
	long unionID;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )

	// CHECK UP
	// 玩家必须属于这个诸侯国(Client, GWServer)
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserSend)
	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion)
	CUnionCmdHelper_GW::Send_r2c_union_data_refresh(playerSend, pUnion->GetData());

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_logo)
{
	guard;

	int dataLen;
	char* data;
	long userDBID;
	SAFE_READ(packet->ReadBinary(&data, &dataLen))
	SAFE_READ(packet->ReadLong(&userDBID))

	GWUser *pUser = g_server.FindActorById(userDBID);
	SAFE_ASSERT(pUser)
	CUnion *pUnion = GetUnionMgr()->FindUnion(pUser->unionID);
	SAFE_ASSERT(pUnion && pUnion->GetCaption()==pUser->userID)
// 	SAFE_ASSERT(pUnion->GetData()->level >= 3);

	DWORD iNewIconID;
	if (pUnion->SetIcon(data, dataLen, iNewIconID))
	{
		char szFileName[16];
		rt2_snprintf(szFileName, 16, "%d", iNewIconID);
		CUnionCmdHelper_GW::BroadcastRegion_r2c_union_logo_data(szFileName, data, dataLen, userDBID, pUnion);
		return eOkay;
	}
	else
	{
		return eError;
	}

	return eOkay;

	unguard;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_update_level)
{
//ldr123 帮派升级没用
	//long userDBID;
	//SAFE_READ(packet->ReadLong(&userDBID))

	//GWUser *pUser = g_server.FindActorById(userDBID);
	//SAFE_ASSERT(pUser)
	//CUnion *pUnion = GetUnionMgr()->FindUnion(pUser->unionID);
	//SAFE_ASSERT(pUnion && pUnion->GetCaption()==userDBID)

	//if (pUnion->UpdateLevel())
	//{
	//	CUnionCmdHelper_GW::Send_r2c_union_update_level_result(userDBID, UNION_SUCCESS, pUnion->GetID(), pUnion->GetData()->level);
	//	CUnionCmdHelper_GW::Broadcast_r2c_union_levelup(pUnion);
	//}
	//else
	//{
	//	CUnionCmdHelper_GW::Send_r2c_union_update_level_result(userDBID, UNION_ERROR_UNKNOWN, pUnion->GetID(), pUnion->GetData()->level);
	//	return eError;
	//}

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_r2g_union_add_score)
{
	long playerSend;
	long unionID;
	long scoreCount;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadLong(&unionID) )
	SAFE_READ(packet->ReadLong(&scoreCount))

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserSend)
	unionID = pUserSend->unionID;
	CUnion *pUnion = GetUnionMgr()->FindUnion(unionID);
	SAFE_ASSERT(pUnion && pUnion->GetCaption()==playerSend)

	if (pUnion->AddScore(scoreCount))
	{
		CUnionCmdHelper_GW::Broadcast_r2c_union_score_change(pUnion);
	}
	else 
	{
		return eError;
	}

	return eOkay;
}

GW_UNION_CMD_FUNCTION(cmd_c2r_union_player_levelup)
{
	long playerSend;
	short level;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadShort(&level) )

	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserSend)
	CUnion *pUnion = GetUnionMgr()->FindUnion(pUserSend->unionID);
	SAFE_ASSERT(pUnion)
	SUnionCitizen *pCitizen = pUnion->Find(playerSend);
	SAFE_ASSERT(pCitizen)

// 	pCitizen->level = level;
	CUnionCmdHelper_GW::Broadcast_r2c_union_player_levelup(pUnion, playerSend, level);

	return eOkay;
}




void CUnionCmdHelper_GW::Send_r2c_union_create_result(DWORD playerSendTo, BYTE errorCode, DWORD unionID, char const* unionName)
{
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_create_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	cmd.WriteLong(unionID);
	cmd.WriteString(unionName);
	g_server.m_region.SendCmd();
}

void CUnionCmdHelper_GW::Send_r2c_union_update_level_result(DWORD playerSendTo, BYTE errorCode, DWORD unionID, BYTE level)
{
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_update_level_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	cmd.WriteLong(unionID);
	cmd.WriteByte(level);
	g_server.m_region.SendCmd();
}

void CUnionCmdHelper_GW::Send_r2c_union_dismiss_other_result(DWORD playerSendTo, BYTE errorCode, DWORD playerID)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_dismiss_other_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	cmd.WriteLong(playerID);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_dismissed_by_other(DWORD playerSendTo, DWORD playerID, char const* unionName)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_dismissed_by_other);
	cmd.WriteLong(playerSendTo);
	cmd.WriteLong(playerID);
	cmd.WriteString(unionName);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_change_nickname_result(DWORD playerSendTo, BYTE errorCode, DWORD playerID, char const* newNickname)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_change_nickname_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	cmd.WriteLong(playerID);
	cmd.WriteString(newNickname);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_other_join_apply(DWORD playerSendTo, DWORD playerID, char const* playerName, const char* szUnionName)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_other_join_apply);
	cmd.WriteLong(playerSendTo);
	cmd.WriteLong(playerID);
	cmd.WriteString(playerName);
	cmd.WriteString(szUnionName);
	
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_join_response(DWORD playerSendTo, BYTE errorCode, DWORD playerID, DWORD unionID, char const* unionName, char const* playerName)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_join_response);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	cmd.WriteLong(playerID);
	cmd.WriteLong(unionID);
	cmd.WriteString(unionName);
	cmd.WriteString(playerName);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_appoint_result(DWORD playerSendTo, BYTE errorCode)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_appoint_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_invitewar_result(DWORD playerSendTo, BYTE errorCode)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_invitewar_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_surrender_other_response(DWORD playerSendTo, BYTE bAgree)
{
	//GWUser* pUserSendTo = g_server.m_database.LookupUser(playerSendTo);
	//if (!pUserSendTo)		return;
	//int rgIdx = g_server.m_regionTable[pUserSendTo->regionID].sessionIndex;
	//if (rgIdx<0)			return;

	//CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	//cmd.BeginWrite();
	//cmd.WriteShort(g2r_union);
	//cmd.WriteShort(r2c_union_surrender_other_response);
	//cmd.WriteLong(playerSendTo);
	//cmd.WriteByte(bAgree);
	//g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_leave_result(DWORD playerSendTo, BYTE errorCode)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_leave_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteByte(errorCode);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Send_r2c_union_data_refresh(DWORD playerSendTo, SUnionData *pUnionData)
{
	GWUser* pUser = g_server.FindActorById(playerSendTo);
	if (!pUser)			return;
	int rgIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
	if (rgIdx<0)		return;

	CHECK(pUnionData);
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_union);
	cmd.WriteShort(r2c_union_data_refresh);
	cmd.WriteLong(playerSendTo);
	pUnionData->Serialize(&cmd);
	g_server.m_region.SendToPeer(rgIdx);
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_join(CUnion* pUnion, SUnionCitizen *pCitizen)
{
	guard;

	CHECK(pUnion);
	CHECK(pCitizen);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_citizen_join);
		cmd.WriteLong(pUnion->GetID());
		pCitizen->Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_leave(CUnion* pUnion, DWORD playerID)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_citizen_leave);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteLong(playerID);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_online(CUnion* pUnion, DWORD playerID)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_citizen_online);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteLong(playerID);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_offline(CUnion* pUnion, DWORD playerID)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_citizen_offline);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteLong(playerID);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_official_change(CUnion* pUnion, DWORD playerID, BYTE official)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_citizen_official_change);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteLong(playerID);
		cmd.WriteByte(official);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_disband(CUnion* pUnion, char reason, char const* unionName)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_disband);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteString(unionName);
		cmd.WriteByte(reason);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_new_bulletin(CUnion* pUnion, char const* NewBulletin)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_new_bulletin);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteString(NewBulletin);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_nickname_change(CUnion* pUnion, DWORD playerID, char const* nickname)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_citizen_nickname_change);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteLong(playerID);
		cmd.WriteString(nickname);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::BroadcastRegion_r2c_union_logo_data(char const* fileName, char const* data, int dataSize, long userDBID, CUnion *pUnion)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_logo_data);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteString(fileName);
		cmd.WriteBinary(data, dataSize);
		cmd.WriteLong(userDBID);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::UpdateRegionUnionData_r2c_union_logo_data(int iSessionIndex)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT DISTINCT ID, Icon, IconData FROM T_union WHERE Icon!=0");
	if (g_server.m_database.ExecCommand(cmd.command, &res) )
	{
		int count = mysql_num_rows(res);
		if (count==0)	return;
		MYSQL_ROW row = mysql_fetch_row(res);
		unsigned long* rowLengths = mysql_fetch_lengths(res);
		DWORD unionID;
		char* fileName;
		char* data;
		int dataSize;
		for (int i=0; i<count; i++)
		{
			unionID = atoi(row[0]);
			fileName = row[1];
			data = row[2];
			dataSize = rowLengths[2];

			if (dataSize>0)
			{
				CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
				cmd.BeginWrite();
				cmd.WriteShort(g2r_union);
				cmd.WriteShort(r2c_union_logo_data);
				cmd.WriteLong(unionID);
				cmd.WriteString(fileName);
				cmd.WriteBinary(data, dataSize);
				cmd.WriteLong(0);	// userDBID
				g_server.m_region.SendToPeer(iSessionIndex);
			}
			row = mysql_fetch_row(res);
			rowLengths = mysql_fetch_lengths(res);
		}
		mysql_free_result(res);
	}
	else
	{
		return;
	}
}

void CUnionCmdHelper_GW::UpdateRegionUnionData_r2c_union_data_refresh(int iSessionIndex)
{
	for (int i=0; i<(int)g_UnionMgr.m_arrUnions.size(); i++)
	{
		CUnion *pUnion = g_UnionMgr.m_arrUnions[i];
		SUnionData *pUnionData = pUnion->GetData();

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_data_refresh);
		cmd.WriteLong(0);
		pUnionData->Serialize(&cmd);
		g_server.m_region.SendToPeer(iSessionIndex);
	}
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_score_change(CUnion* pUnion)
{
//暂未用,打算改成帮派金钱
	guard;

	CHECK(pUnion);

	SUnionData *pUnionData = pUnion->GetData();

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_score_change);
		cmd.WriteLong(pUnionData->m_nID);
 		cmd.WriteLong(pUnionData->m_nFighting);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_levelup(CUnion* pUnion)
{
	guard;

	CHECK(pUnion);

	SUnionData *pUnionData = pUnion->GetData();

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_levelup);
		cmd.WriteLong(pUnionData->m_nID);
// 		cmd.WriteLong(pUnionData->level);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_player_levelup(CUnion* pUnion, long playerID, short level)
{
	guard;

	CHECK(pUnion);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_player_levelup);
		cmd.WriteLong(pUnion->GetID());
		cmd.WriteLong(playerID);
		cmd.WriteShort(level);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(DWORD playerSendTo, SUnionData *pUnionData)
{
	guard;

	CHECK(pUnionData);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_union);
		cmd.WriteShort(r2c_union_data_refresh);
		cmd.WriteLong(playerSendTo);
		pUnionData->Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

void CUnionCmdHelper_GW::Send_r2c_msg_text(DWORD playerSendTo, const char* pText)
{
	
	do 
	{
		if (NULL == pText)
		{
			break;
		}
		GWUser* pUserSend = g_server.FindActorById(playerSendTo);
		if (NULL == pUserSend)
		{
			break;
		}
		int Session= g_server.GetRegionSession(pUserSend->regionID);
		if (-1 == Session)
		{
			break;
		}

		CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_team);
		rPacket.WriteLong(playerSendTo);
		rPacket.WriteByte(GW2R_TEAM_PICKER);
		rPacket.WriteString(pText);
		g_server.m_region.SendToPeer(Session);
		
	} while (false);
	
}
