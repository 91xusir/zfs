#include "gc_include.h"
#include "master_prentice_base.h"
#include "gc_prentice.h"
#include "ui_form_textMsg.h"
#include "ui_form_msg.h"


GcPrenticeMgr g_GcPrenticeMgr;

GcPrenticeMgr::GcPrenticeMgr()
{
	m_pFrmRemove = NULL;
}

GcPrenticeMgr::~GcPrenticeMgr()
{
}

bool GcPrenticeMgr::Judge_AddMasterPrentice(GcActor* pMaster)
{
	return true;
}

bool GcPrenticeMgr::Judge_RemoveMasterPrentice(GcActor* pApplyActor, const char* masterName, const char* prenticeName)
{
	return true;
}

bool GcPrenticeMgr::Judge_FinishMasterPrentice(GcActor* pMaster)
{
	return true;
}

bool GcPrenticeMgr::Judge_WithdrawMPGift(long lev)
{
	if (!PrenSysGiftLevel_IsValid(lev))
		return false;

	int nBagFree = GetWorld()->m_pPlayer->mItem.m_Bag.FreeSpace();
	if (nBagFree < PrenSysGiftNeedGrids[lev])
	{
		ShowSystemMessage(R(MSG_ITEM_CONTAINER_NEEDMORE), true);
		return false;
	}

	return true;
}

void GcPrenticeMgr::cbPrenticeAdd_Agree(RtwWidget* sender, RtwEventDelegate* e)
{
	UIFormMsg *pFrm = (UIFormMsg *)sender->GetUser1();
	DWORD *pUserData = (DWORD*)pFrm->GetUserData();

	GcPrenticeCmd::NetSend_c2r_prentice_add_response(1, *pUserData);

	DEL_ONE(pUserData);
}

void GcPrenticeMgr::cbPrenticeAdd_Disagree(RtwWidget* sender, RtwEventDelegate* e)
{
	UIFormMsg *pFrm = (UIFormMsg *)sender->GetUser1();
	DWORD *pUserData = (DWORD*)pFrm->GetUserData();

	GcPrenticeCmd::NetSend_c2r_prentice_add_response(0, *pUserData);

	DEL_ONE(pUserData);
}

void GcPrenticeMgr::cbPrenticeFinish_Agree(RtwWidget* sender, RtwEventDelegate* e)
{
	UIFormMsg *pFrm = (UIFormMsg *)sender->GetUser1();
	DWORD *pUserData = (DWORD*)pFrm->GetUserData();

	GcPrenticeCmd::NetSend_c2r_prentice_finish_response(1, *pUserData);

	DEL_ONE(pUserData);
}

void GcPrenticeMgr::cbPrenticeFinish_Disagree(RtwWidget* sender, RtwEventDelegate* e)
{
	UIFormMsg *pFrm = (UIFormMsg *)sender->GetUser1();
	DWORD *pUserData = (DWORD*)pFrm->GetUserData();

	GcPrenticeCmd::NetSend_c2r_prentice_finish_response(0, *pUserData);

	DEL_ONE(pUserData);
}

void GcPrenticeMgr::cbOnMasterRemove(RtwWidget* sender, RtwEventDelegate* e)
{
	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	RtString str = pFrm->GetString();

	DWORD dwMyUserID = GetWorld()->m_pPlayer->DBID();

	string masterName = GetWorld()->m_pPlayer->GetName();
	string prenticeName = str;

	if (!g_GcPrenticeMgr.Judge_RemoveMasterPrentice(GetWorld()->m_pPlayer, masterName.c_str(), prenticeName.c_str()))
	{
		PlayerFailOperate(R(MSG_PRENSYS_REMOVE_CONDITION));
		return;
	}
	GcPrenticeCmd::NetSend_c2r_prentice_remove(masterName.c_str(), prenticeName.c_str());
}

void GcPrenticeMgr::cbOnPrenticeRemove(RtwWidget* sender, RtwEventDelegate* e)
{
	UIFormTextMsg *pFrm = (UIFormTextMsg *)e->GetUser1();
	RtString str = pFrm->GetString();

	DWORD dwMyUserID = GetWorld()->m_pPlayer->DBID();

	string masterName = str;
	string prenticeName = GetWorld()->m_pPlayer->GetName();

	if (!g_GcPrenticeMgr.Judge_RemoveMasterPrentice(GetWorld()->m_pPlayer, masterName.c_str(), prenticeName.c_str()))
	{
		PlayerFailOperate(R(MSG_PRENSYS_REMOVE_CONDITION));
		return;
	}
	GcPrenticeCmd::NetSend_c2r_prentice_remove(masterName.c_str(), prenticeName.c_str());
}


bool GcPrenticeCmd::NetSend_c2r_prentice_add_apply(DWORD playerDBID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_prentice);
	pPacket->WriteShort(c2r_prentice_add_apply);
	pPacket->WriteLong(playerDBID);
	return NetSend(pPacket);
}

bool GcPrenticeCmd::NetSend_c2r_prentice_add_response(char bAgree, DWORD playerOID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_prentice);
	pPacket->WriteShort(c2r_prentice_add_response);
	pPacket->WriteByte(bAgree);
	if (bAgree)
		pPacket->WriteLong(playerOID);
	return NetSend(pPacket);
}

bool GcPrenticeCmd::NetSend_c2r_prentice_remove(const char* masterName, const char* prenticeName)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_prentice);
	pPacket->WriteShort(c2r_prentice_remove);
	pPacket->WriteString((char*)masterName);
	pPacket->WriteString((char*)prenticeName);
	return NetSend(pPacket);
}

bool GcPrenticeCmd::NetSend_c2r_prentice_finish_apply(DWORD playerDBID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_prentice);
	pPacket->WriteShort(c2r_prentice_finish_apply);
	pPacket->WriteLong(playerDBID);
	return NetSend(pPacket);
}

bool GcPrenticeCmd::NetSend_c2r_prentice_finish_response(char bAgree, DWORD playerOID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_prentice);
	pPacket->WriteShort(c2r_prentice_finish_response);
	pPacket->WriteByte(bAgree);
	if (bAgree)
		pPacket->WriteLong(playerOID);
	return NetSend(pPacket);
}

bool GcPrenticeCmd::NetSend_c2r_prentice_gift(long lev)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_prentice);
	pPacket->WriteShort(c2r_prentice_gift);
	pPacket->WriteLong(lev);
	return NetSend(pPacket);
}


bool PrenticeCmdProcess_client(CG_CmdPacket *cmd)
{
	short SubCmd;
	BOOL_ENSURE( cmd->ReadShort(&SubCmd) )

	bool bResult = true;
	switch (SubCmd)
	{
		case r2c_prentice_add_apply:
			bResult = net_r2c_prentice_add_apply(cmd);
			break;
		case r2c_prentice_finish_apply:
			bResult = net_r2c_prentice_finish_apply(cmd);
			break;
		case r2c_prentice_add_result:
			bResult = net_r2c_prentice_add_result(cmd);
			break;
		case r2c_prentice_remove_result:
			bResult = net_r2c_prentice_remove_result(cmd);
			break;
		case r2c_prentice_finish_result:
			bResult = net_r2c_prentice_finish_result(cmd);
			break;
		case r2c_prentice_onlinetime:
			bResult = net_r2c_prentice_onlinetime(cmd);
			break;
		case r2c_prentice_gift_result:
			bResult = net_r2c_prentice_gift_result(cmd);
			break;
		default:
			bResult = false;
			break;
	}

	return bResult;
}


bool net_r2c_prentice_add_apply(CG_CmdPacket *pPacket)
{
	long playerID;
	READb_long(playerID);

	GcActor* pActor = GetWorld()->m_ActorManager.FindActive(playerID);
	if (pActor)
	{
		char tmp[128];
		rt2_snprintf(tmp, 128, R(MSG_PRENSYS_ADD_INVITE), pActor->GetName() );
		//tmp[127] = 0;
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp, UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(&g_GcPrenticeMgr, GcPrenticeMgr, cbPrenticeAdd_Agree, pFrm);
		pFrm->EvCancel = RTW_CALLBACK_1(&g_GcPrenticeMgr, GcPrenticeMgr, cbPrenticeAdd_Disagree, pFrm);
		DWORD *pUserData = RT_NEW DWORD;
		*pUserData = playerID;
		pFrm->SetUserData(pUserData);
	}

	return true;
}

bool net_r2c_prentice_finish_apply(CG_CmdPacket *pPacket)
{
	long playerID;
	READb_long(playerID);

	GcActor* pActor = GetWorld()->m_ActorManager.FindActive(playerID);
	if (pActor)
	{
		char tmp[128];
		rt2_snprintf(tmp, 128, R(MSG_PRENSYS_FINISH_INVITE), pActor->GetName() );
		//tmp[127] = 0;
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(tmp, UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(&g_GcPrenticeMgr, GcPrenticeMgr, cbPrenticeFinish_Agree, pFrm);
		pFrm->EvCancel = RTW_CALLBACK_1(&g_GcPrenticeMgr, GcPrenticeMgr, cbPrenticeFinish_Disagree, pFrm);
		DWORD *pUserData = RT_NEW DWORD;
		*pUserData = playerID;
		pFrm->SetUserData(pUserData);
	}

	return true;
}

bool net_r2c_prentice_add_result(CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	char* MasterName;
	char* PrenticeName;
	READb_short(errorCode);
	READb_string(MasterName);
	READb_string(PrenticeName);

	char cTmp1024[1024];
	switch (errorCode)
	{
	case prentice_code_success:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_ADD_SUCCESS));
		break;
	case prentice_code_error_unknown:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_ADD_FAIL));
		break;
	case prentice_code_time_limit:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_ADD_TIME_LIMIT));
		break;
	default:
		break;
	}
	ShowSystemMessage(cTmp1024);

	return true;

	unguard;
}

bool net_r2c_prentice_remove_result(CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	char* MasterName;
	char* PrenticeName;
	READb_short(errorCode);
	READb_string(MasterName);
	READb_string(PrenticeName);

	char cTmp1024[1024];
	switch (errorCode)
	{
	case prentice_code_success:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_REMOVE_SUCCESS));
		break;
	case prentice_code_error_unknown:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_REMOVE_FAIL));
		break;
	default:
		break;
	}
	ShowSystemMessage(cTmp1024);

	return true;

	unguard;
}

bool net_r2c_prentice_finish_result(CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	char* MasterName;
	char* PrenticeName;
	READb_short(errorCode);
	READb_string(MasterName);
	READb_string(PrenticeName);

	char cTmp1024[1024];
	switch (errorCode)
	{
	case prentice_code_success:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_FINISH_SUCCESS));
		break;
	case prentice_code_error_unknown:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_FINISH_FAIL));
		break;
	default:
		break;
	}
	ShowSystemMessage(cTmp1024);

	return true;

	unguard;
}

bool net_r2c_prentice_onlinetime(CG_CmdPacket *pPacket)
{
	guard;

	long prenticeID;
	long onlineTime;
	READb_long(prenticeID);
	READb_long(onlineTime);

	GetWorld()->m_Friend.StudentOnlineTime(prenticeID, onlineTime);
	return true;

	unguard;
}

bool net_r2c_prentice_gift_result(CG_CmdPacket *pPacket)
{
	guard;

	short errorCode;
	long lev;
	READb_short(errorCode);
	READb_long(lev);

	char cTmp1024[1024];
	switch(errorCode)
	{
	case prentice_code_success:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_WITHDRAW_GIFT_SUCCESS));
		break;
	case prentice_code_error_unknown:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_WITHDRAW_GIFT_FAIL));
		break;
	case prentice_code_gift_withdrawed:
		rt2_sprintf(cTmp1024, "%s", R(MSG_PRENSYS_GIFT_WITHDRAWED));
		break;
	default:
		break;
	}
	ShowSystemMessage(cTmp1024);

	return true;

	unguard;
}

