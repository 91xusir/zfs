#include "region.h"

#define SAFE_READ(p)	 { if(!(p)) return CMD_ERROR_READ_DATA; }
#define SAFE_ASSERT(p)	 { if ( !(p) ) return CMD_ERROR_UNKNOWN; }

int FeodCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	short feod_cmd_type;
	if ( !cmd->ReadShort(&feod_cmd_type) )		return CMD_ERROR_UNKNOWN;

	switch(feod_cmd_type)
	{
	case c2r_feod_content_query:
		return cmd_c2r_feod_content_query(CmdGiver, cmd);
	case c2r_feod_taxrate:
		return cmd_c2r_feod_taxrate(CmdGiver, cmd);
	case c2r_feod_feodbank_withdraw:
		return cmd_c2r_feod_feodbank_withdraw(CmdGiver, cmd);
	default:
		return CMD_ERROR_UNKNOWN;
	}
}

int FeodCmdProcess_FromGW(CG_CmdPacket *cmd)
{
	short feod_cmd_type;
	if ( !cmd->ReadShort(&feod_cmd_type) )		return CMD_ERROR_UNKNOWN;

	switch(feod_cmd_type)
	{
	case r2c_feod_content:
		return cmd_r2c_feod_content(cmd);
	case r2c_feod_taxrate_result:
		return cmd_r2c_feod_taxrate_result(cmd);
	case r2c_feod_feodbank_withdraw_result:
		return cmd_r2c_feod_feodbank_withdraw_result(cmd);
	case g2r_feod_data:
		return cmd_g2r_feod_data(cmd);

	default:
		return CMD_ERROR_UNKNOWN;
	}
}

FEOD_CMD_FUNCTION(cmd_c2r_feod_content_query)
{
	long npcID;
	char* feodName;
	SAFE_READ(cmd->ReadLong(&npcID))
	SAFE_READ(cmd->ReadString(&feodName))

	if (strlen(feodName) > FEODNAME_MAX)
		return CMD_ERROR_NONE;
	if (!g_region->UserUploadString_IsValid(feodName))
		return CMD_ERROR_NONE;

	if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
		return CMD_ERROR_NONE;

	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	//转发
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_feod);
	packet.WriteShort(c2r_feod_content_query);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteString(feodName);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

FEOD_CMD_FUNCTION(cmd_c2r_feod_taxrate)
{
	long npcID;
	char* feodName;
	short nRate;
	SAFE_READ(cmd->ReadLong(&npcID))
	SAFE_READ(cmd->ReadString(&feodName))
	SAFE_READ(cmd->ReadShort(&nRate) )

	if (strlen(feodName) > FEODNAME_MAX)
		return CMD_ERROR_NONE;
	if (!g_region->UserUploadString_IsValid(feodName))
		return CMD_ERROR_NONE;

	if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
		return CMD_ERROR_NONE;

	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	//转发
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_feod);
	packet.WriteShort(c2r_feod_taxrate);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteString(feodName);
	packet.WriteShort(nRate);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}

FEOD_CMD_FUNCTION(cmd_c2r_feod_feodbank_withdraw)
{
	long npcID;
	char* feodName;
	long money;
	SAFE_READ(cmd->ReadLong(&npcID))
	SAFE_READ(cmd->ReadString(&feodName))
	SAFE_READ(cmd->ReadLong(&money) )

	if (strlen(feodName) > FEODNAME_MAX)
		return CMD_ERROR_NONE;
	if (!g_region->UserUploadString_IsValid(feodName))
		return CMD_ERROR_NONE;

	if (CmdGiver->m_pTrade || CmdGiver->m_pPShop)
		return CMD_ERROR_NONE;

	CRegionCreature *cre = CmdGiver;
	BOOL_ENSURE(cre->IsUser())

	//转发
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_feod);
	packet.WriteShort(c2r_feod_feodbank_withdraw);
	packet.WriteLong(cre->m_userInfo->m_userId);
	packet.WriteString(feodName);
	packet.WriteLong(money);
	g_region->m_gws.EndSend();

	return CMD_ERROR_NONE;
}



FEOD_CMD_FUNCTION_FROM_GW(cmd_r2c_feod_content)
{
	guard;

	long playerSendToDBID;
	char* feodName;
	short nRate;
	long bankMoney, incomeToday;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadString(&feodName))
	SAFE_READ(cmd->ReadLong(&incomeToday) )
	SAFE_READ(cmd->ReadShort(&nRate) )
	SAFE_READ(cmd->ReadLong(&bankMoney) )

	CRegionUser* pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CFeodCmdBuilder_Svr::Build_r2c_feod_content(feodName, incomeToday, nRate, bankMoney);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

FEOD_CMD_FUNCTION_FROM_GW(cmd_r2c_feod_taxrate_result)
{
	guard;

	long playerSendToDBID;
	char* feodName;
	char errorCode;
	short newTaxRate;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadString(&feodName))
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadShort(&newTaxRate) )

	CRegionUser* pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		CFeodCmdBuilder_Svr::Build_r2c_feod_taxrate_result(feodName, errorCode, newTaxRate);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

FEOD_CMD_FUNCTION_FROM_GW(cmd_r2c_feod_feodbank_withdraw_result)
{
	guard;

	long playerSendToDBID;
	char* feodName;
	char errorCode;
	long newBankMoney;
	long withdrawMoney;
	SAFE_READ(cmd->ReadLong(&playerSendToDBID) )
	SAFE_READ(cmd->ReadString(&feodName))
	SAFE_READ(cmd->ReadByte(&errorCode) )
	SAFE_READ(cmd->ReadLong(&newBankMoney) )
	SAFE_READ(cmd->ReadLong(&withdrawMoney) )

	CRegionUser* pUser = g_region->FindUser(playerSendToDBID);
	if (pUser && pUser->m_dummy)
	{
		if (errorCode==FEOD_SUCCESS)
		{
			pUser->m_dummy->AddGold(withdrawMoney);

			// log到数据库
			CG_CmdPacket& packet = g_region->m_gws.BeginSend();
			packet.WriteShort(r2g_log_event);
			packet.WriteByte(PLT_WithdrawFeodBank);
			packet.WriteLong(pUser->m_userId);
			packet.WriteString((char*)pUser->m_userIpAddress.c_str()); // userIP
			packet.WriteLong(withdrawMoney);
			packet.WriteString(feodName);
			packet.WriteLong(newBankMoney);
			g_region->m_gws.EndSend();
		}

		CFeodCmdBuilder_Svr::Build_r2c_feod_feodbank_withdraw_result(feodName, errorCode, newBankMoney);
		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
	unguard;
}

FEOD_CMD_FUNCTION_FROM_GW(cmd_g2r_feod_data)
{
	guard;

	short feodCount;
	SFeod feod;
	if (!cmd->ReadShort(&feodCount))
		return CMD_ERROR_READ_DATA;
	g_RegionFeodMgr.Clear();
	for (short i=0; i<feodCount; i++)
	{
		if (!feod.Serialize(cmd))
			return CMD_ERROR_READ_DATA;
		if (!g_RegionFeodMgr.AddFeodInfo(feod))
			return CMD_ERROR_UNKNOWN;
		if (cmd->IsError())
			return CMD_ERROR_READ_DATA;
	}

	// 发送给Client
	EXT_SPACE::hash_map<ULONG, CRegionUser*>::iterator it;
	for(it=g_region->m_userMap.begin(); it!=g_region->m_userMap.end(); it++)
	{
		CRegionUser* user = (*it).second;
		CRegionCreature* tmp;
		if(!user->Session()) continue;
		tmp = user->m_dummy;
		if(!tmp) 
			continue;
		if(!tmp->m_scene)
			continue;

		CFeodCmdBuilder_Svr::Build_r2c_feod_data();
		user->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;

	unguard;
}


void CFeodCmdBuilder_Svr::Build_r2c_feod_content(char* feodName, int incomeToday, short nTaxRate, int feodBank)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_feod_content);
	g_sendCmd->WriteString(feodName);
	g_sendCmd->WriteLong(incomeToday);
	g_sendCmd->WriteShort(nTaxRate);
	g_sendCmd->WriteLong(feodBank);
}

void CFeodCmdBuilder_Svr::Build_r2c_feod_taxrate_result(char* feodName, BYTE errorCode, short newTaxRate)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_feod_taxrate_result);
	g_sendCmd->WriteString(feodName);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteShort(newTaxRate);
}

void CFeodCmdBuilder_Svr::Build_r2c_feod_feodbank_withdraw_result(char* feodName, BYTE errorCode, int newBankMoney)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_feod_feodbank_withdraw_result);
	g_sendCmd->WriteString(feodName);
	g_sendCmd->WriteByte(errorCode);
	g_sendCmd->WriteLong(newBankMoney);
}

void CFeodCmdBuilder_Svr::Build_r2c_feod_data()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_union);
	g_sendCmd->WriteShort(r2c_feod_data);
	g_sendCmd->WriteShort((short)g_RegionFeodMgr.m_arrFeods.size());
	for (int i=0; i<(int)g_RegionFeodMgr.m_arrFeods.size(); i++)
	{
		g_RegionFeodMgr.m_arrFeods[i].Serialize(g_sendCmd);
	}
}

void CFeodCmdBuilder_Svr::NetSend_r2g_feod_change_union(char* feodName, DWORD unionID)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_feod);
	packet.WriteShort(r2g_feod_change_union);
	packet.WriteString(feodName);
	packet.WriteLong(unionID);
	g_region->m_gws.EndSend();
}

