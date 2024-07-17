#include "gw_server.h"
#include "gw_feod.h"
#include "union_net_define.h"
#include "gw_cmd_feod.h"

#define SAFE_READ(p)	{ if(!(p)) return eInvalidData; }
#define SAFE_ASSERT(p)	{ if ( !(p) ) return eError; }

int GW_FeodCmdProcess(CG_CmdPacket *packet)
{
	short feod_cmd_type;
	if ( !packet->ReadShort(&feod_cmd_type) )		return eInvalidData;

	switch(feod_cmd_type)
	{
	case c2r_feod_content_query:
		return cmd_c2r_feod_content_query(packet);
	case c2r_feod_taxrate:
		return cmd_c2r_feod_taxrate(packet);
	case c2r_feod_feodbank_withdraw:
		return cmd_c2r_feod_feodbank_withdraw(packet);
	case r2g_feod_new_income:
		return cmd_r2g_feod_new_income(packet);
	case r2g_feod_change_union:
		return cmd_r2g_feod_change_union(packet);
	default:
		return eInvalidData;
	}
}

GW_FEOD_CMD_FUNCTION(cmd_c2r_feod_content_query)
{
	char* feodName;
	long playerSend;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadString(&feodName) )

	// CHECK
	// 
	// 
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserSend);
	CFeod *pFeod = GetFeodMgr()->FindFeod(feodName);
	SAFE_ASSERT(pFeod);
	CUnion* pUnion = GetUnionMgr()->FindUnion(pUserSend->unionID);
    SAFE_ASSERT(pUnion);
	SAFE_ASSERT(playerSend == pUnion->GetCaption());		// 必须是诸侯
	SAFE_ASSERT(pFeod->m_data.unionID==pUnion->GetID());	// 必须是他的采邑

	CFeodCmdHelper_GW::Send_r2c_feod_content(playerSend, feodName, pFeod->GetIncomeToday(), pFeod->GetTaxRate(), pFeod->GetBank());

	return eOkay;
}

GW_FEOD_CMD_FUNCTION(cmd_c2r_feod_taxrate)
{
	char* feodName;
	short nRate;
	long playerSend;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadString(&feodName) )
	SAFE_READ(packet->ReadShort(&nRate) )

	// CHECK
	// 
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserSend);
	CFeod *pFeod = GetFeodMgr()->FindFeod(feodName);
	SAFE_ASSERT(pFeod);
	CUnion* pUnion = GetUnionMgr()->FindUnion(pUserSend->unionID);
	SAFE_ASSERT(pUnion);
	SAFE_ASSERT(playerSend == pUnion->GetCaption())			// 必须是诸侯
	SAFE_ASSERT(pFeod->m_data.unionID==pUnion->GetID());	// 必须是他的采邑

	if (pFeod->SetTaxRate(nRate))
	{
		CFeodCmdHelper_GW::Send_r2c_feod_taxrate_result(playerSend, feodName, FEOD_SUCCESS, nRate);

		for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
		{
			GWRegion& rg = g_server.m_regionTable[rgID];
			if (rg.sessionIndex == -1)
				continue;

			CFeodCmdHelper_GW::UpdateRegionFeodData_r2c_feod_data(rg.sessionIndex);
		}
	}
	else 
	{
		CFeodCmdHelper_GW::Send_r2c_feod_taxrate_result(playerSend, feodName, FEOD_ERROR_UNKNOWN, 0);
	}

	return eOkay;
}

GW_FEOD_CMD_FUNCTION(cmd_c2r_feod_feodbank_withdraw)
{
	char* feodName;
	long money;
	long playerSend;
	SAFE_READ(packet->ReadLong(&playerSend) )
	SAFE_READ(packet->ReadString(&feodName) )
	SAFE_READ(packet->ReadLong(&money) )

	// CHECK
	// 
	GWUser* pUserSend = g_server.FindActorById(playerSend);
	SAFE_ASSERT(pUserSend);
	CFeod *pFeod = GetFeodMgr()->FindFeod(feodName);
	SAFE_ASSERT(pFeod);
	CUnion* pUnion = GetUnionMgr()->FindUnion(pUserSend->unionID);
	SAFE_ASSERT(pUnion);
	SAFE_ASSERT(playerSend == pUnion->GetCaption())			// 必须是诸侯
	SAFE_ASSERT(pFeod->m_data.unionID==pUnion->GetID());	// 必须是他的采邑

	if (pFeod->WithdrawBank(money))
	{
		CFeodCmdHelper_GW::Send_r2c_feod_feodbank_withdraw_result(playerSend, feodName, FEOD_SUCCESS, pFeod->GetBank(), money);
	}

	return eOkay;
}

GW_FEOD_CMD_FUNCTION(cmd_r2g_feod_new_income)
{
	char* feodName;
	long income;
	SAFE_READ(packet->ReadString(&feodName) )
	SAFE_READ(packet->ReadLong(&income) )

	CFeod *pFeod = GetFeodMgr()->FindFeod(feodName);
	SAFE_ASSERT(pFeod)

	if (!pFeod->AddIncome(income))
	{
		ERR2("采邑税收提交失败！（采邑:%s, 税收:%d）\n", feodName, income);
		return eError;
	}

	return eOkay;
}

GW_FEOD_CMD_FUNCTION(cmd_r2g_feod_change_union)
{
	char* feodName;
	long unionID;
	SAFE_READ(packet->ReadString(&feodName) )
	SAFE_READ(packet->ReadLong(&unionID) )

	CFeod *pFeod = GetFeodMgr()->FindFeod(feodName);
	SAFE_ASSERT(pFeod)

	if (!pFeod->ChangUnion(unionID))
	{
		ERR2("Change Union Error!(Feod:%s, UnionID:%d)\n", feodName, unionID);
		return eError;
	}

	// 更新region数据
	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CFeodCmdHelper_GW::UpdateRegionFeodData_r2c_feod_data(rg.sessionIndex);
	}

	return eOkay;
}


void CFeodCmdHelper_GW::Send_r2c_feod_content(DWORD playerSendTo, char* feodName, int incomeToday, short nTaxRate, int feodBank)
{
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_feod);
	cmd.WriteShort(r2c_feod_content);
	cmd.WriteLong(playerSendTo);
	cmd.WriteString(feodName);
	cmd.WriteLong(incomeToday);
	cmd.WriteShort(nTaxRate);
	cmd.WriteLong(feodBank);
	g_server.m_region.SendCmd();
}

void CFeodCmdHelper_GW::Send_r2c_feod_taxrate_result(DWORD playerSendTo, char* feodName, BYTE errorCode, short nRate)
{
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_feod);
	cmd.WriteShort(r2c_feod_taxrate_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteString(feodName);
	cmd.WriteByte(errorCode);
	cmd.WriteShort(nRate);
	g_server.m_region.SendCmd();
}

void CFeodCmdHelper_GW::Send_r2c_feod_feodbank_withdraw_result(DWORD playerSendTo, char* feodName, BYTE errorCode, int newBankMoney, int withdrawMoney)
{
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_feod);
	cmd.WriteShort(r2c_feod_feodbank_withdraw_result);
	cmd.WriteLong(playerSendTo);
	cmd.WriteString(feodName);
	cmd.WriteByte(errorCode);
	cmd.WriteLong(newBankMoney);
	cmd.WriteLong(withdrawMoney);
	g_server.m_region.SendCmd();
}

void CFeodCmdHelper_GW::UpdateRegionFeodData_r2c_feod_data(int iSessionIndex)
{
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_feod);
	cmd.WriteShort(g2r_feod_data);
	cmd.WriteShort((short)g_FeodMgr.m_feods.size());
	for (int i=0; i<(int)g_FeodMgr.m_feods.size(); i++)
	{
		g_FeodMgr.m_feods[i].m_data.Serialize(&cmd);
	}
	g_server.m_region.SendToPeer(iSessionIndex);
}

