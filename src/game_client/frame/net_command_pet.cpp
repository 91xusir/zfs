#include "gc_include.h"
#include "region_client.h"
#include "ui_form_pet_include.h"
#include "ui_form_timebar.h"
#include "game_assistant.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "item_net_define.h"
#include "ui_form_pet_bank.h"

bool net_r2c_ctrl_pet(short cmdID, CG_CmdPacket *pPacket)
{
	char ctrlType = 0;
	long lPetID = 0;
	READb_byte(ctrlType);
	switch (ctrlType)
	{
	case PET_CTRL_CALLBACK:
		{
			READb_long(lPetID);
			GetWorld()->PlayActorEffect(lPetID,"callbackpet");
			
			GcSynchroActor *pSActor = (GcSynchroActor*)FindAllActor(lPetID);
			if (pSActor)
			{
				pSActor->m_ePetType = PET_TYPE_NULL;
				pSActor->SetDownloadWaitTime(1.f);
			}
		}
		break;
	case PET_CTRL_CALLOUT:
		{
			READb_long(lPetID);
			GetWorld()->PlayActorEffect(lPetID,"calloutpet");
		}
		break;
	case PET_CTRL_ATTACK:
		{
		}
		break;
	case PET_CTRL_SYNERGY:
		{
		}
		break;
	case PET_CTRL_PASSIVE:
		{
		}
		break;
	case PET_CTRL_DEF:
		{
		}
		break;
	case PET_CTRL_USE_SKILL:
		{
		}
		break;
	case PET_CTRL_SET_DEFSKILL:
		{
			long skillid = 0;
			READb_long(skillid);
			g_layerMain->m_formPetCtrl->SetDefSkillUI(skillid);
		}
		break;
	default:
		break;
	}
	return true;
}

bool net_r2c_fuse_pet(short cmdID, CG_CmdPacket *pPacket)
{
	char fuseType = 0;
	long lSucceed = 0;
	char finishParam = 0;
	READb_byte(fuseType);
	pPacket->SerializeBit(lSucceed,1);
	READb_byte(finishParam);

	g_layerMain->m_formTimeBar->Hide(true);
	switch (fuseType)
	{
	case PET_FUSE_TYPE_MERGE:
		if (lSucceed)
			PlayerFailOperate(R(MSG_MERGE_PET_SUCCEED));
		else
			PlayerFailOperate(R(MSG_MERGE_PET_FAIL));
		g_layerMain->m_formPetMerge->OnFinishMerge(lSucceed != 0);
		break;
	case PET_FUSE_TYPE_ABILITY:
		if (lSucceed)
			PlayerFailOperate(R(MSG_ABILITY_PET_SUCCEED));
		else if (finishParam == PET_FUSE_FINISH_DELETE)
			PlayerFailOperate(R(MSG_ABILITY_PET_FAIL));
		else
			PlayerFailOperate(R(MSG_ABILITY_PET_FAIL_NODEL));
		g_layerMain->m_formPetAbility->OnFinishAbility(lSucceed != 0);
		break;
	case PET_FUSE_TYPE_TOBEADS:
		if (lSucceed)
			PlayerFailOperate(R(MSG_TOBEADS_PET_SUCCEED));
		else
			PlayerFailOperate(R(MSG_TOBEADS_PET_FAIL));
		g_layerMain->m_formPetToBeads->OnFinishToBeads(lSucceed != 0);
		break;
	case PET_FUSE_TYPE_MERGEBEADS:
		if (lSucceed)
			PlayerFailOperate(R(MSG_MERGEBEADS_PET_SUCCEED));
		else
			PlayerFailOperate(R(MSG_MERGEBEADS_PET_FAIL));
		g_layerMain->m_formPetMergeBeads->OnFinishMergeBeads(lSucceed != 0);
		break;
	}
	return true;
}

bool net_r2c_catch_pet(short cmdID, CG_CmdPacket *pPacket)
{
	char cError;
	long Actor;
	READb_byte(cError);
	READb_long(Actor);
	GcActor *pActor = FindAllActor(Actor);
	if (!pActor) return false;
	switch (cError)
	{
	case PET_CATCH_ERROR_START:
		{
			long Pet;
			READb_long(Pet);
			GcActor *pPet = FindAllActor(Pet);
			if (pActor && pPet && pActor != GetPlayer())
			{
				pActor->CatchPetAdd(pPet);
			}
			else if (pActor == GetPlayer() && pPet)
			{
				SPet *pSPet = g_TablePet.FindPet(pPet->m_pNpc->Id);
				if (!pSPet) return false;
				DWORD dwTime = pSPet->dwCatchMillseconds;
				g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbus_catch_pet,dwTime);
			}
			if (pPet)
				pPet->mBaseActor.PlayEffect("catchpetEffect",true,"root",RtgVertex3(0.f,0.f,0.f),true);
		}
		break;
	case PET_CATCH_ERROR_LIFETIME:
	case PET_CATCH_ERROR_BECATCHED:
	case PET_CATCH_ERROR_BEATTACKED:
	case PET_CATCH_ERROR_DIE:
	case PET_CATCH_ERROR_SUCCEED:
	default:
		{
			if (pActor->m_dwCatchPet)
			{
				GcActor *pPet = FindAllActor(pActor->m_dwCatchPet);
				if (pPet)
					pPet->mBaseActor.StopEffect("catchpetEffect");
			}
			pActor->StopCatchPet(cError);
		}
		break;
	}
	return true;
}

bool net_r2c_bank_query_response_pet(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
	BOOL_ENSURE( g_layerMain->m_formPetBank->SerializeBankBag(pPacket));
	g_layerMain->m_formPetBank->Refresh();

	return true;
	unguard;
}

bool net_r2c_bank_store_response_pet(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	char bSuccess;
	READb_char(bSuccess);
	if (bSuccess)
	{
		ShowSystemMessage(R(MSG_STORE_SUCCESS_PET));

		g_layerMain->m_formPetMain->OnPreDraw();
		g_layerMain->m_formPetBank->ToBankResult(true);
	}
	else
	{
		char reason;
		READb_char(reason);
		switch (reason)
		{
		case ITEM_REASON_CONTAINER_FULL:
			ShowSystemMessage(R(MSG_STORE_FAIL_REACHMAX_PET));
			break;
		case ITEM_REASON_TOO_FAR:
		default:
			ShowSystemMessage(R(MSG_STORE_FAIL_PET));
			break;
		}
		g_layerMain->m_formPetBank->ToBankResult(false);
	}

	return true;

	unguard;
}

bool net_r2c_bank_withdraw_response_pet(short cmdID, CG_CmdPacket *pPacket)
{
	guard;

	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	char bSuccess;
	READb_char(bSuccess);
	if (bSuccess)
	{
		ShowSystemMessage(R(MSG_WITHDRAW_SUCCESS_PET));

		g_layerMain->m_formPetMain->OnPreDraw();
		g_layerMain->m_formPetBank->ToMineResult(true);
	}
	else
	{
		char reason;
		READb_char(reason);
		switch (reason)
		{
		case ITEM_REASON_TOO_FAR:
		default:
			ShowSystemMessage(R(MSG_WITHDRAW_FAIL_PET));
			break;
		}
		g_layerMain->m_formPetBank->ToMineResult(false);
	}

	return true;

	unguard;
}

bool net_r2c_pet_linkup(short cmdID, CG_CmdPacket *pPacket)
{
	long link = 0;
	READb_long(link);
	GetPlayerPet()->LinkUpWithPet(link);
	return true;
}


bool CPetNetHelper::NetSend_c2r_ctrl_pet(char ctrlType,int param)
{
	return true;
}

bool CPetNetHelper::NetSend_c2r_fuse_pet_start(char fuseType,SItemID item1,SItemID item2,SItemID item3)
{
	if ( !CanFusePet(GetWorld()->m_pItemManager,fuseType,item1,item2,item3) )
		return false;

	DWORD dwTime = GetFusePetMillseconds(fuseType);
	g_layerMain->m_formTimeBar->Begin(UIFormTimeBar::tbus_fuse_pet,dwTime);

	long start = 1;
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_fuse_pet);
	cmd->WriteByte(fuseType);
	cmd->SerializeBit(start,1);
	cmd->WriteLong(item1.id);
	cmd->WriteLong(item2.id);
	cmd->WriteLong(item3.id);
	NetSend(cmd);

	return true;
}

bool CPetNetHelper::NetSend_c2r_fuse_pet_stop(char fuseType)
{
	g_layerMain->m_formTimeBar->Hide(true);

	long start = 0;
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_fuse_pet);
	cmd->WriteByte(fuseType);
	cmd->SerializeBit(start,1);
	NetSend(cmd);
	return true;
}

bool CPetNetHelper::NetSend_c2r_catch_pet_start(DWORD petID)
{
	long start = 1;
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_catch_pet);
	cmd->SerializeBit(start,1);
	cmd->WriteLong(petID);
	NetSend(cmd);
	return true;
}

bool CPetNetHelper::NetSend_c2r_catch_pet_stop()
{
	long start = 0;
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_catch_pet);
	cmd->SerializeBit(start,1);
	NetSend(cmd);
	return true;
}

bool CPetNetHelper::NetSend_c2r_bank_query_pet(DWORD npcID, const char* pass)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_query_pet);
	pPacket->WriteLong(npcID);
	pPacket->WriteString((char*)pass);
	return NetSend(pPacket);
}

bool CPetNetHelper::NetSend_c2r_bank_store_pet(long petItemID, DWORD npcID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_store_pet);
	pPacket->WriteLong(petItemID);
	pPacket->WriteLong(npcID);
	return NetSend(pPacket);
}

bool CPetNetHelper::NetSend_c2r_bank_withdraw_pet(long petItemID, DWORD npcID)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_item);
	pPacket->WriteShort(c2r_bank_withdraw_pet);
	pPacket->WriteLong(petItemID);
	pPacket->WriteLong(npcID);
	return NetSend(pPacket);
}