//********************************************************************
//	created:	2010.05.31
//	filename: 	netcommand_union.cpp
//	author:		ldr123
//	purpose:	游戏中,帮派系统网络操作
//  other:		version 1.0
//********************************************************************

#include "gc_include.h"
#include "region_client.h"
#include "game_script_interface.h"
#include "game_assistant.h"
#include "gc_track_bomb_missile.h"
#include "union_net_define.h"
#include "net_command_union.h"
#include "ui_form_union.h"
#include "ui_form_union_create.h"
#include "ui_form_msg.h"
#include "ui_form_feod_tax.h"
#include "ui_form_feod_bank.h"
#include "ui_form_feod_content.h"
#include "ui_form_bank.h"
#include "ui_form_playeritems.h"
#include "ui_form_castlewar.h"

bool UnionCmdProcess_client(CG_CmdPacket *cmd)
{
	short union_cmd_type;
	BOOL_ENSURE( cmd->ReadShort(&union_cmd_type) )

		switch (union_cmd_type)
	{
		case r2c_union_create_result:
			return net_r2c_union_create_result(cmd);
		case r2c_union_dismiss_other_result:
			return net_r2c_union_dismiss_other_result(cmd);
		case r2c_union_dismissed_by_other:
			return net_r2c_union_dismissed_by_other(cmd);
		case r2c_union_change_nickname_result:
			return net_r2c_union_change_nickname_result(cmd);
		case r2c_union_other_join_apply:
			return net_r2c_union_other_join_apply(cmd);
		case r2c_union_join_response:
			return net_r2c_union_join_response(cmd);
		case r2c_union_appoint_result:
			return net_r2c_union_appoint_result(cmd);
		case r2c_union_invitewar_result:
			return net_r2c_union_invitewar_result(cmd);
		case r2c_union_surrender_other_response:
			return net_r2c_union_surrender_other_response(cmd);
		case r2c_union_leave_result:
			return net_r2c_union_leave_result(cmd);
		case r2c_feod_content:
			return net_r2c_feod_content(cmd);
		case r2c_feod_taxrate_result:
			return net_r2c_feod_taxrate_result(cmd);
		case r2c_feod_feodbank_withdraw_result:
			return net_r2c_feod_feodbank_withdraw_result(cmd);
		case r2c_union_data_refresh:
			return net_r2c_union_data_refresh(cmd);
		case r2c_union_citizen_join:
			return net_r2c_union_citizen_join(cmd);
		case r2c_union_citizen_leave:
			return net_r2c_union_citizen_leave(cmd);
		case r2c_union_citizen_online:
			return net_r2c_union_citizen_online(cmd);
		case r2c_union_citizen_offline:
			return net_r2c_union_citizen_offline(cmd);
		case r2c_union_citizen_official_change:
			return net_r2c_union_citizen_official_change(cmd);
		case r2c_union_disband:
			return net_r2c_union_disband(cmd);
		case r2c_union_new_bulletin:
			return net_r2c_union_new_bulletin(cmd);
		case r2c_union_citizen_nickname_change:
			return net_r2c_union_citizen_nickname_change(cmd);
		case r2c_union_logo_result:
			return net_r2c_union_logo_result(cmd);
		case r2c_union_logo_data:
			return net_r2c_union_logo_data(cmd);
		case r2c_union_icon_change:
			return net_r2c_union_icon_change(cmd);
		case r2c_union_score_change:
			return net_r2c_union_score_change(cmd);
		case r2c_union_update_level_result:
			return net_r2c_union_update_level_result(cmd);
		case r2c_union_levelup:
			return net_r2c_union_levelup(cmd);
		case r2c_feod_data:
			return net_r2c_feod_data(cmd);
		case r2c_union_player_levelup:
			return net_r2c_union_player_levelup(cmd);
		case r2c_union_new_log:
			return net_r2c_union_new_log(cmd);
		case r2c_union_log_query_result:
			return net_r2c_union_log_query_result(cmd);
		case r2c_union_contribution:
			return net_r2c_union_contribution_result(cmd);
		case r2c_union_update_Contribution:
			return net_r2c_union_update_Contribution(cmd);
		case r2c_WarScores_Change:
			return net_r2c_WarScores_Change(cmd);
		case r2c_union_set_icon:
			return net_r2c_union_set_icon(cmd);
		default:
			return false;
	}

	return true;
}

bool FeodCmdProcess_client(CG_CmdPacket *cmd)
{
	short feod_cmd_type;
	BOOL_ENSURE( cmd->ReadShort(&feod_cmd_type) )

		switch (feod_cmd_type)
	{
		case r2c_feod_content:
			return net_r2c_feod_content(cmd);
		case r2c_feod_taxrate_result:
			return net_r2c_feod_taxrate_result(cmd);
		case r2c_feod_feodbank_withdraw_result:
			return net_r2c_feod_feodbank_withdraw_result(cmd);

		default:
			return false;
	}

	return true;
}

//创建回复
bool net_r2c_union_create_result(CG_CmdPacket *pPacket)
{
	guard;

	char errorCode;
	long unionID;
	char* unionName;
	READb_char(errorCode);
	READb_long(unionID);
	READb_string(unionName);

	g_layerMain->m_fromPlayerItems->LockOperate(false);

	GcActor *pPlayer = GetWorld()->m_pPlayer;
	if (errorCode==UNION_SUCCESS)
	{
		//CHECK(pPlayer->mItem.RemoveMoney(UNION_CREATE_USE_MONEY));

		//int count, page, gridI, gridJ;
		//if (pPlayer->mItem.m_Bag.FindType(UNION_CREATE_USE_ITEMID, ItemColor_All, &count, &page, &gridI, &gridJ))
		//{
		//    pPlayer->mItem.m_Bag.RemoveItem(page, gridI, gridJ);
		//}
		//else if (pPlayer->mItem.m_MissionBag.FindType(UNION_CREATE_USE_ITEMID, ItemColor_All, &count, &page, &gridI, &gridJ))
		//{
		//    pPlayer->mItem.m_MissionBag.RemoveItem(page, gridI, gridJ);
		//}
		g_layerMain->m_fromPlayerItems->Refresh(true);

		ShowSystemMessage(R(MSG_UNION_CREATE_SUCCESS));
		g_layerMain->m_formUnionCreate->Hide();
		pPlayer->SetUnionID(unionID);
		pPlayer->m_unionName = unionName;
		pPlayer->m_unionNickName = "";
		pPlayer->m_unionIcon = UNION_DEFAULT_ICON_NAME;
		g_layerMain->m_formUnion->OnRecvActorUnionData(pPlayer);
	}
	else if (errorCode==UNION_NAME_INVALID)
	{
		PlayerFailOperate(R(MSG_UNION_CREATE_FAIL_NAME));
	}
	else if (errorCode==UNION_CAPTION_LOCK_TIME)
	{
		PlayerFailOperate(R(MSG_UNION_CREATE_FAIL_LIMITTIME));
	}
	else if (errorCode==UNION_NOT_IN_SAME_FACTION)
	{
		PlayerFailOperate(R(MSG_UNION_CREATE_FAIL_FACTION));
	}
	else
	{
		PlayerFailOperate(R(MSG_UNION_CREATE_FAIL));
	}

	return true;
	unguard;
}

//禅让给别人
bool net_r2c_union_dismiss_other_result(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	char errorCode;
	long playerID;
	READb_char(errorCode);
	READb_long(playerID);

	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
	SUnionCitizen *pCitizen = pUnionData->Find(playerID);
	if (!pCitizen)		return false;

	if (errorCode==UNION_SUCCESS)
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_DISMISS_SUCCESS), pCitizen->m_strPlayerName.c_str() );
		//tmp128[127] = 0;
		ShowSystemMessage(tmp128);

		pUnionData->RemoveCitizen(playerID);
		pCitizen = NULL;
		g_layerMain->m_formUnion->Refresh();
	}
	else
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_DISMISS_FAIL), pCitizen->m_strPlayerName.c_str() );
		//tmp128[127] = 0;
		ShowSystemMessage(tmp128);
	}

	return true;
	unguard;
}

//别人禅让给自己
bool net_r2c_union_dismissed_by_other(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	long playerID;
	char* unionName;
	READb_long(playerID);
	READb_string(unionName);

	rt2_snprintf(tmp128, 128, R(MSG_UNION_BE_DISMISSED), unionName );
	//tmp128[127] = 0;
	ShowSystemMessage(tmp128);
	GetWorld()->m_pPlayer->SetUnionID(0);
	g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);
	g_layerMain->m_formUnion->Hide();
	return true;

	unguard;
}

//更新nickname
bool net_r2c_union_change_nickname_result(CG_CmdPacket *pPacket)
{
	guard;

	char errorCode;
	long playerID;
	char* newNickname;
	READb_char(errorCode);
	READb_long(playerID);
	READb_string(newNickname);

	if (errorCode==UNION_SUCCESS)
	{
		ShowSystemMessage(R(MSG_UNION_CHANGENICK_SUCCESS));
	}
	else
	{
		ShowSystemMessage(R(MSG_UNION_CHANGENICK_FAIL));
	}

	return true;
	unguard;
}

//邀请别人加入
bool net_r2c_union_other_join_apply(CG_CmdPacket *pPacket)
{
	guard;

	long playerID;
	char* playerName;
	char* unionName;

	READb_long(playerID);
	READb_string(playerName);
	READb_string(unionName);

	//if(g_layerMain && g_layerMain->m_formUnion->GetRefuseState())
	//{
	//	CUnionNetHelper::NetSend_c2r_union_other_join_response(GetWorld()->m_pPlayer->GetUnionID(), playerID, 0);
	//	return true;
	//}
	g_layerMain->m_formUnion->SetUnionButtonAnimation();


	char tmp[64]; 
	rt2_snprintf(tmp, 64, R(MSG_UNION_OTHER_INVITE), playerName, unionName );
	tmp[63] = 0;
	UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(g_layerMain->m_formUnion, UIFormUnion, ResponseOther_Agree, pFrm);
	pFrm->EvCancel = RTW_CALLBACK_1(g_layerMain->m_formUnion, UIFormUnion, ResponseOther_Disagree, pFrm);
	DWORD *pUserData = RT_NEW DWORD;
	*pUserData = playerID;
	pFrm->SetUserData(pUserData);

	DWORD dwTimeID = GetTimer()->AddTimer(g_layerMain->m_formUnion, 30);
	if (dwTimeID > 0)
	{
		g_layerMain->m_formUnion->m_pFrmMsg = pFrm;
	}

	return true;
	unguard;
}

//别人对你的邀请的反馈
bool net_r2c_union_join_response(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	char errorCode;
	long unionID;
	long playerID;
	char* unionName;
	char* playerName;
	READb_char(errorCode);
	READb_long(playerID);
	READb_long(unionID);
	READb_string(unionName);
	READb_string(playerName);

	if (errorCode==UNION_SUCCESS)
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_OTHER_AGREE), playerName, unionName);
		//tmp128[127] = 0;
		ShowSystemMessage(tmp128);
	}
	else
	{
		if (g_layerMain->m_formUnion && g_layerMain->m_formUnion->m_unionData.m_nID!=0)
		{
			rt2_snprintf(tmp128, 128, R(MSG_UNION_OTHER_DISAGREE), playerName, g_layerMain->m_formUnion->m_unionData.m_strName.c_str());
			//tmp128[127] = 0;
			ShowSystemMessage(tmp128);
		}
	}

	return true;
	unguard;
}

//更改别人的职位
bool net_r2c_union_appoint_result(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	char errorCode;
	READb_char(errorCode);

	if (errorCode==UNION_SUCCESS)
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_APPOINT_SUCCESS));
		//tmp128[127] = 0;
		ShowSystemMessage(tmp128);
	}
	else
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_APPOINT_FAIL));
		//tmp128[127] = 0;
		ShowSystemMessage(tmp128);
	}

	return true;
	unguard;
}

bool net_r2c_union_invitewar_result(CG_CmdPacket *pPacket)
{
	guard;

	return true;
	unguard;
}

bool net_r2c_union_surrender_other_response(CG_CmdPacket *pPacket)
{
	guard;

	return true;
	unguard;
}

//脱离帮派
bool net_r2c_union_leave_result(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	char errorCode;
	READb_char(errorCode);

	if (errorCode==UNION_SUCCESS)
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_YOU_LEAVE), (char *)g_layerMain->m_formUnion->m_unionData.m_strName.c_str());
		//tmp128[127] = 0;
		ShowSystemMessage(tmp128);
		GetWorld()->m_pPlayer->SetUnionID(0);
		g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);
		g_layerMain->m_formUnion->Hide();
	}

	return true;
	unguard;
}

bool net_r2c_union_data_refresh(CG_CmdPacket *pPacket)
{
	guard;

 	if ( g_layerMain->m_formUnion->m_unionData.Serialize(pPacket) )
 	{
 		g_layerMain->m_formUnion->Refresh();
		CUnionNetHelper::NetSend_c2r_union_query_log(g_layerMain->m_formUnion->m_unionData.m_nID);
 	}
 
 	if (GetWorld()->m_pPlayer->GetUnionID()==0)
 	{
 		SUnionCitizen *pCitizen = g_layerMain->m_formUnion->Find(GetWorld()->m_pPlayer->DBID());
 		BOOL_ENSURE(pCitizen);
		GetWorld()->m_pPlayer->SetUnionID(g_layerMain->m_formUnion->m_unionData.m_nID);
 		GetWorld()->m_pPlayer->m_unionName = g_layerMain->m_formUnion->m_unionData.m_strName.c_str();
 		GetWorld()->m_pPlayer->m_unionNickName = pCitizen->m_strNickName.c_str();
 		GetWorld()->m_pPlayer->m_unionIcon = g_layerMain->m_formUnion->m_unionData.m_strName.c_str();
 	}
	g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);

	return true;
	unguard;
}

//有人加入了帮派
bool net_r2c_union_citizen_join(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	SUnionCitizen citizen;
	BOOL_ENSURE( citizen.Serialize(pPacket) );
	
	if ( citizen.m_nPlayerID == GetWorld()->m_pPlayer->DBID() )
		return true;

	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
	SUnionCitizen *pCitizen = pUnionData->Find(citizen.m_nPlayerID);
	if (pCitizen)	
		return true;

	pUnionData->AddCitizen(citizen);
	g_layerMain->m_formUnion->Refresh();
	rt2_snprintf(tmp128, 128, R(MSG_UNION_PLAYER_JOIN), citizen.m_strPlayerName.c_str(), pUnionData->m_strName.c_str());
	//tmp128[127] = 0;
	ShowSystemMessage(tmp128);

	return true;
	unguard;
}

//有人离开了帮派
bool net_r2c_union_citizen_leave(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	long playerID;
	READb_long(playerID);

	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
	SUnionCitizen *pCitizen = pUnionData->Find(playerID);
	if (!pCitizen)			return true;

	rt2_snprintf(tmp128, 128, R(MSG_UNION_PLAYER_LEAVE), pCitizen->m_strPlayerName.c_str(), pUnionData->m_strName.c_str());
	//tmp128[127] = 0;
	ShowSystemMessage(tmp128);
	pUnionData->RemoveCitizen(playerID);
	pCitizen = NULL;
	g_layerMain->m_formUnion->Refresh();

	return true;
	unguard;
}

//有人上线
bool net_r2c_union_citizen_online(CG_CmdPacket *pPacket)
{
	guard;

	long playerID;
	READb_long(playerID);

	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
	SUnionCitizen *pCitizen = pUnionData->Find(playerID);
	if (!pCitizen)			return true;

	pCitizen->m_bOnLine = true;
	g_layerMain->m_formUnion->Refresh();

	return true;
	unguard;
}

//有人下线
bool net_r2c_union_citizen_offline(CG_CmdPacket *pPacket)
{
	guard;

	long playerID;
	READb_long(playerID);

	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
	SUnionCitizen *pCitizen = pUnionData->Find(playerID);
	if (!pCitizen)			return true;

	pCitizen->m_bOnLine = false;
	g_layerMain->m_formUnion->Refresh();

	return true;
	unguard;
}

//有人职位更改
bool net_r2c_union_citizen_official_change(CG_CmdPacket *pPacket)
{
 	guard;
 
 	char tmp128[128];
 	long playerID;
 	char official;
 	READb_long(playerID);
 	READb_byte(official);
 
 	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);

	static char *levelUpDown[] = {"升职", "降职"};
 
	SUnionCitizen *pCitizen = pUnionData->Find(playerID);
	if (!pCitizen)			return true;

  	if (official!=UO_King) // 诸侯离职，选择新任诸侯
 	{

		rt2_snprintf(
						tmp128, 128, "%s 官职变动，%s 由%s %s为 %s", 
						pUnionData->m_strName.c_str(), pCitizen->m_strPlayerName.c_str(), 
						GetUOString((EUnionOfficial)pCitizen->m_eUnionOfficial), 
						levelUpDown[pCitizen->m_eUnionOfficial < official?1:0],
						GetUOString((EUnionOfficial)official)
					);
		ShowSystemMessage(tmp128);
 	}
	pCitizen->m_eUnionOfficial = (EUnionOfficial)official;
	g_layerMain->m_formUnion->m_unionData.m_bGetChangeEnable = true;

	g_layerMain->m_formUnion->Refresh();

	return true;
	unguard;
}

//帮派解散
bool net_r2c_union_disband(CG_CmdPacket *pPacket)
{
	guard;

	char tmp128[128];
	short reason;
	char* unionName;
	READb_short(reason);
	READb_string(unionName);

	GetWorld()->m_pPlayer->SetUnionID(0);
	if (reason==UNION_MSG_CAPTAIN_DISBAND)
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_DISBAND_BY_CAPTAIN), unionName );
	}
	else if (reason==UNION_MSG_FEWCITIZEN_DISBAND)
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_DISBAND_FEWCITIZEN), unionName );
	}
	else 
	{
		rt2_snprintf(tmp128, 128, R(MSG_UNION_DISBAND), unionName );
	}
	//tmp128[127] = 0;
	ShowSystemMessage(tmp128);

	GetWorld()->m_pPlayer->SetUnionID(0);
	g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);
	g_layerMain->m_formUnion->Hide();

	return true;
	unguard;
}

//新公告
bool net_r2c_union_new_bulletin(CG_CmdPacket *pPacket)
{
	char* cNewBulletin;
	READb_string(cNewBulletin);

	g_layerMain->m_formUnion->OnRecvNewBulletin(cNewBulletin);
	return true;
}

//nickname更改,估计无用
bool net_r2c_union_citizen_nickname_change(CG_CmdPacket *pPacket)
{
// 	guard;
// 
// 	long playerID;
// 	char* nickname;
// 	READb_long(playerID);
// 	READb_string(nickname);
// 
// 	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
// 	SUnionCitizen *pCitizen = pUnionData->Find(playerID);
// 	if (pCitizen)
// 	{
// 		pCitizen->m_strNickName = nickname;
// 		//		g_layerMain->m_formUnion->Refresh();
// 	}
// 
// 	if (playerID == GetWorld()->m_pPlayer->DBID())
// 	{
// 		GetWorld()->m_pPlayer->m_unionNickName = nickname;
// 		g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);
// 	}

	return true;
	unguard;
}

bool net_r2c_union_logo_result(CG_CmdPacket *pPacket)
{
	//guard;

	//char errorCode;
	//char* logoNewName;
	//READb_char(errorCode);
	//READb_string(logoNewName);

	//if (errorCode==UNION_SUCCESS)
	//{
	//	g_layerMain->m_formUnion->OnRecvChangeLogoSuccess(logoNewName);
	//}
	//else
	//{
	//	g_layerMain->m_formUnion->OnRecvChangeLogoFail();
	//}

	return true;
	unguard;
}

bool net_r2c_union_logo_data(CG_CmdPacket *pPacket)
{
 	guard;
// 
// 	char* FileName;
// 	char* data;
// 	int dataLen;
// 	READb_string(FileName);
// 	if (!pPacket->ReadBinary(&data, &dataLen))	 {NET_READ_ERROR;return false;}
// 
// 	int iImageWidth, iImageHeight;
// 	bool res = RtgImage::Check24bBmpFormat(data, dataLen, iImageWidth, iImageHeight);
// 	if ( !res || iImageHeight!=16 || iImageWidth!=16 )
// 	{
// 		CUnionIconMgr::SUnionIconData IconData;
// 		IconData.dataLen = dataLen;
// 		IconData.fileName = FileName;
// 		IconData.data = new char[dataLen];
// 		memset(IconData.data, 0, dataLen);
// 		g_UnionIconMgr.m_arrIcon.push_back(IconData);
// 	}
// 
// 	CUnionIconMgr::SUnionIconData IconData;
// 	IconData.dataLen = dataLen;
// 	IconData.fileName = FileName;
// 	IconData.data = new char[dataLen];
// 	memcpy(IconData.data, data, dataLen);
// 	g_UnionIconMgr.m_arrIcon.push_back(IconData);
// 
// 	g_layerMain->m_formUnion->OnRecvLogoData(FileName);
// 	if (g_layerMain->m_formCastleWarReg->IsVisible())
// 		g_layerMain->m_formCastleWarReg->Refresh();
// 
 	return true;
	unguard;
}

bool net_r2c_union_icon_change(CG_CmdPacket *pPacket)
{
	guard;

// 	char* FileName;
// 	READb_string(FileName);
// 
// 	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
// 	if (pUnionData)
// 	{
// 		pUnionData->icon = FileName;
// 	}
// 
// 	GetWorld()->m_pPlayer->m_unionIcon = FileName;
// 	g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);

	return true;
	unguard;
}

bool net_r2c_union_score_change(CG_CmdPacket *pPacket)
{
	guard;

// 	long score;
// 	READb_long(score);
// 
// 	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
// 	if (pUnionData)
// 	{
// 		pUnionData->score = score;
// 
// 		char cTmp128[128];
// 		sprintf(cTmp128, R(MSG_UNION_SCORE_CHANGE), score);
// 		ShowSystemMessage(cTmp128);
// 		g_layerMain->m_formUnion->Refresh();
// 	}

	return true;
	unguard;
}

bool net_r2c_union_update_level_result(CG_CmdPacket *pPacket)
{
	guard;

	char errCode;
	READb_char(errCode);

	if (errCode==UNION_SUCCESS)
	{
		ShowSystemMessage(R(MSG_UNION_LEVELUP_SUCCESS));
	}
	else 
	{
		ShowSystemMessage(R(MSG_UNION_LEVELUP_FAIL));
	}

	return true;
	unguard;
}

bool net_r2c_union_levelup(CG_CmdPacket *pPacket)
{
	guard;
// 
// 	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
// 	if (pUnionData)
// 	{
// 		pUnionData->m_sLevel++;
// 
// 		ShowSystemMessage(R(MSG_UNION_LEVELUP));
// 		g_layerMain->m_formUnion->Refresh();
// 	}
// 
	return true;

	unguard;
}

//玩家升级
bool net_r2c_union_player_levelup(CG_CmdPacket *pPacket)
{
	guard;

	long playerID;
	short playerLevel;
	READb_long(playerID);
	READb_short(playerLevel);

	SUnionData *pUnionData = &(g_layerMain->m_formUnion->m_unionData);
	if (pUnionData)
	{
		SUnionCitizen *pCitizen = pUnionData->Find(playerID);
		if (pCitizen)
		{
			pCitizen->m_sLevel = playerLevel;
			g_layerMain->m_formUnion->Refresh();
		}
	}

	return true;

	unguard;
}
 
//新日志
bool net_r2c_union_new_log(CG_CmdPacket *pPacket)
{
	guard;
	long nType;
	long nTime;
	char *pChar;

	READb_long(nType);
	READb_long(nTime);
	READb_string(pChar);
	g_layerMain->m_formUnion->AddLog(nType, nTime, pChar);
	return true;

	unguard;
}
//更新日志
bool net_r2c_union_log_query_result(CG_CmdPacket *pPacket)
{
 	guard;

	g_layerMain->m_formUnion->ClearHistory();
  	long nLogNum;
  	READb_long(nLogNum);
  
  	for (int i=0; i<nLogNum; ++i)
  	{
  		long nType, nTime;
  		char *pTmp;
  
  		READb_long(nType);
  		READb_long(nTime);
  		READb_string(pTmp);
  
  		g_layerMain->m_formUnion->AddLog(nType, nTime, pTmp);
  	}

	return true;
	unguard;
}

//刷新帮贡
bool net_r2c_union_update_Contribution(CG_CmdPacket *pPacket)
{
	guard;
	long playerID;
	long contribution;

	READb_long(playerID);
	READb_long(contribution);

	g_layerMain->m_formUnion->UpdateContribution(playerID, contribution);
	return true;
	unguard;
}

//帮贡返回
bool net_r2c_union_contribution_result(CG_CmdPacket *pPacket)
{
	guard;
	long money;
	long item1;
	long item2;
	long item3;
	long item4;

	READb_long(money);
	READb_long(item1);
	READb_long(item2);
	READb_long(item3);
	READb_long(item4);

	g_layerMain->m_formUnion->UpdateMoneyItem(money, item1, item2, item3, item4);
	return true;
	unguard;

}


//帮派的战力评分改变
bool net_r2c_WarScores_Change(CG_CmdPacket *pPacket)
{
	guard;
	short nFighting;	//是不是太小了?
	READb_short(nFighting);

	g_layerMain->m_formUnion->UpdateWarScores(nFighting);
	return true;
	unguard;
}

//图腾图标已被修改
bool net_r2c_union_set_icon(CG_CmdPacket *pPacket)
{
	guard;
	long icon;
	long nMoney;
// 	long playerID;
// 	READb_long(playerID);
	READb_long(icon);
	READb_long(nMoney);

	g_layerMain->m_formUnion->UpdateIcon(icon, nMoney);
	g_layerMain->m_formUnion->OnRecvActorUnionData(GetWorld()->m_pPlayer);
	return true;
	unguard;
}

bool CUnionNetHelper::NetSend_c2r_union_create(DWORD npcID, char* name, char* enounce)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_create);
	pPacket->WriteLong(npcID);
	pPacket->WriteString(name);
	pPacket->WriteString(enounce);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_dismiss_other(DWORD unionID, DWORD playerID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_dismiss_other);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(playerID);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_change_nickname(DWORD unionID, DWORD playerID, char* newNickname)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_change_nickname);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(playerID);
	pPacket->WriteString(newNickname);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_join(DWORD unionID, DWORD playerApplyTo)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_join);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(playerApplyTo);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_join(DWORD unionID, char *pName)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_join_By_Name);
	pPacket->WriteLong(unionID);
	pPacket->WriteString(pName);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_other_join_response(DWORD unionID, DWORD playerApply, BYTE bAgree)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_other_join_response);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(playerApply);
	pPacket->WriteByte(bAgree);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_bulletin(DWORD unionID, char* bulletin)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_bulletin);
	pPacket->WriteLong(unionID);
	pPacket->WriteString(bulletin);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_appoint(DWORD unionID, DWORD playerID, BYTE official)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_appoint);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(playerID);
	pPacket->WriteByte(official);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_disband(DWORD unionID, DWORD playerID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_give_union);
	pPacket->WriteLong(playerID);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_invitewar(DWORD unionInvite, DWORD unionInvited)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_invitewar);
	pPacket->WriteLong(unionInvite);
	pPacket->WriteLong(unionInvited);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_surrender(DWORD unionSurrender, DWORD unionSurrendered)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_surrender);
	pPacket->WriteLong(unionSurrender);
	pPacket->WriteLong(unionSurrendered);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_leave(DWORD unionID, DWORD playerID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_leave);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(playerID);
	return NetSend(pPacket);
}

//更改图腾
bool CUnionNetHelper::NetSend_c2r_union_totem(DWORD unionID, DWORD picID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_set_icon);
	pPacket->WriteLong(picID);
	return NetSend(pPacket);
}


bool CUnionNetHelper::NetSend_c2r_union_data_query(DWORD unionID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_data_query);
	pPacket->WriteLong(unionID);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_feod_content_query(DWORD npcID, char* feodName)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_feod);
	pPacket->WriteShort(c2r_feod_content_query);
	pPacket->WriteLong(npcID);
	pPacket->WriteString(feodName);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_feod_taxrate(DWORD npcID, char* feodName, short nRate)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_feod);
	pPacket->WriteShort(c2r_feod_taxrate);
	pPacket->WriteLong(npcID);
	pPacket->WriteString(feodName);
	pPacket->WriteShort(nRate);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_feod_feodbank_withdraw(DWORD npcID, char* feodName, long money)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_feod);
	pPacket->WriteShort(c2r_feod_feodbank_withdraw);
	pPacket->WriteLong(npcID);
	pPacket->WriteString(feodName);
	pPacket->WriteLong(money);
	return NetSend(pPacket);
}


bool CUnionNetHelper::NetSend_c2r_union_logo(char* data, int dataSize)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_logo);
	pPacket->WriteBinary(data, dataSize);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_query_logo_data(char *FileName)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_query_logo_data);
	pPacket->WriteString(FileName);
	return NetSend(pPacket);
}

bool CUnionNetHelper::NetSend_c2r_union_query_log(DWORD unionID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_log_query);
//  	pPacket->WriteLong(unionID);
	return NetSend(pPacket);
}

//帮派贡献
bool CUnionNetHelper::NetSend_c2r_union_contribution(DWORD unionID, DWORD nMoney, DWORD nItem1, DWORD nItem2, DWORD nItem3, DWORD nItem4)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_union);
	pPacket->WriteShort(c2r_union_contribution);
	pPacket->WriteLong(unionID);
	pPacket->WriteLong(nMoney);
	pPacket->WriteLong(nItem1);
	pPacket->WriteLong(nItem2);
	pPacket->WriteLong(nItem3);
	pPacket->WriteLong(nItem4);
	return NetSend(pPacket);
}

string GetFeodName(const string& NameInDB)
{
	static string FeodNameInClient[FEOD_COUNT] = { "FEOD_XIQI", "FEOD_ZHAOGE", "FEOD_CHUANYUN" };

	for (int i=0; i<FEOD_COUNT; i++)
	{
		if (NameInDB == FeodNameInDB[i])
			return Rs(FeodNameInClient[i].c_str());
	}
	return "";
}


bool net_r2c_feod_content(CG_CmdPacket *pPacket)
{
	guard;

	char* feodName;
	long incomeToday, bankMoney;
	short nTaxRate;
	READb_string(feodName);
	READb_long(incomeToday);
	READb_short(nTaxRate);
	READb_long(bankMoney);

	g_layerMain->m_formFeodBank->m_iFeodBank = bankMoney;
	g_layerMain->m_formFeodBank->Refresh();
	g_layerMain->m_formFeodTax->m_nTaxRate = nTaxRate;
	g_layerMain->m_formFeodTax->m_iIncomeToday = incomeToday;
	g_layerMain->m_formFeodTax->Refresh();
	g_layerMain->m_formFeodContent->m_FeodBank = bankMoney;
	g_layerMain->m_formFeodContent->m_nTaxRate = nTaxRate;
	g_layerMain->m_formFeodContent->m_IncomeToday = incomeToday;
	g_layerMain->m_formFeodContent->Refresh();

	return true;
	unguard;
}

bool net_r2c_feod_taxrate_result(CG_CmdPacket *pPacket)
{
	guard;

	char* feodName;
	char errorCode;
	short newTaxRate;
	READb_string(feodName);
	READb_char(errorCode);
	READb_short(newTaxRate);

	if (errorCode==FEOD_SUCCESS)
	{
		g_layerMain->m_formFeodTax->OnModifySuccess(newTaxRate);
	}
	else
	{
		ShowSystemMessage(R(MSG_UNION_TAX_FAIL));
	}

	return true;
	unguard;
}

bool net_r2c_feod_feodbank_withdraw_result(CG_CmdPacket *pPacket)
{
	guard;

	char* feodName;
	char errorCode;
	long newBank;
	READb_string(feodName);
	READb_char(errorCode);
	READb_long(newBank);

	if (errorCode==FEOD_SUCCESS)
	{
		g_layerMain->m_formFeodBank->OnWithdrawSuccess(newBank);
	}
	else
	{
		ShowSystemMessage(R(MSG_UNION_UNIONBANK_WITHDRAW_FAIL));
	}

	return true;
	unguard;
}

bool net_r2c_feod_data(CG_CmdPacket *pPacket)
{
	guard;

	short feodCount;
	SFeod feod;
	READb_short(feodCount);
	g_GcFeodMgr.Clear();
	for (short i=0; i<feodCount; i++)
	{
		if (!feod.Serialize(pPacket))
			return false;
		if (!g_GcFeodMgr.AddFeodInfo(feod))
			return false;
	}

	return true;

	unguard;
}
