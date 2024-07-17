#include "gw_server.h"
#include "master_prentice_base.h"
#include "gw_prentice.h"


CMasterPrenticeMgr g_MasterPrenticeMgr;

void CMasterPrenticeMgr::GetMasterPrentice(DWORD userID, std::vector<SMasterPrentice>& arrMPs)
{
	arrMPs.clear();
	SMasterPrentice mp;

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT Idx,MasterID,MasterName,PrenticeID,PrenticeName,UNIX_TIMESTAMP(FinishTime),CreditToAdd,FinishOnlineTime FROM T_MasterPrentice"
		" WHERE MasterID=%d OR PrenticeID=%d", userID, userID);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return;
	}

	int count = mysql_num_rows(res);
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		mp.Clear();
		mp.index = atoi(row[0]);
		mp.masterID = atoi(row[1]);
		mp.masterName = row[2];
		mp.prenticeID = atoi(row[3]);
		mp.prenticeName = atoi(row[4]);
		mp.finishTime = atoi(row[5]);
		mp.creditToAdd = atoi(row[6]);
		mp.finishOnlineTime = atoi(row[7]);
		arrMPs.push_back(mp);
	}
	mysql_free_result(res);
}

void CMasterPrenticeMgr::GetMasterPrentice(DWORD userID, std::vector<DWORD>& MastersID, std::vector<std::string>& MastersName, std::vector<DWORD>& PrenticesID, std::vector<std::string>& PrenticesName, std::vector<DWORD>& FinishedID, std::vector<std::string>& FinishedName)
{
	MastersID.clear();
	PrenticesID.clear();
	MastersName.clear();
	PrenticesName.clear();

	std::vector<SMasterPrentice> arrMPs;
	GetMasterPrentice(userID, arrMPs);

	for (int i=0; i<(int)arrMPs.size(); i++)
	{
		SMasterPrentice& mp = arrMPs[i];
		if (mp.masterID == userID)
		{
			if (mp.finishTime!=0)
			{
			}
			else
			{
			}
		}
		else if (mp.prenticeID == userID)
		{
		}
		else 
		{
			ERR("[CMasterPrenticeMgr::GetMasterPrentice] Unknown Error!!!\n");
		}
	}
}

bool CMasterPrenticeMgr::AddMasterPrentice(DWORD masterID, DWORD prenticeID, long &outIndex, const char* masterName, const char* prenticeName)
{
	short errorCode = prentice_code_success;

	GWUser *pMaster = g_server.FindActorById(masterID);
	GWUser *pPrentice = g_server.FindActorById(prenticeID);
	if (!pMaster || !pPrentice)
		return false;
	DWORD dwNow = time(NULL);
	if ( pMaster->dwLastRemoveMPTime!=0 && (dwNow - pMaster->dwLastRemoveMPTime < PRESYS_COLDOWN_TIME) )
	{
		errorCode = prentice_code_time_limit;
		CMasterPrenticeCmd::Send_g2r_add_master_prentice_result(errorCode, outIndex, masterID, (char*)masterName, prenticeID, (char*)prenticeName);
		return false;
	}
	if ( pPrentice->dwLastRemoveMPTime!=0 && (dwNow - pPrentice->dwLastRemoveMPTime < PRESYS_COLDOWN_TIME) )
	{
		errorCode = prentice_code_time_limit;
		CMasterPrenticeCmd::Send_g2r_add_master_prentice_result(errorCode, outIndex, masterID, (char*)masterName, prenticeID, (char*)prenticeName);
		return false;
	}

	SMasterPrentice mp;
	if (DB_GetMasterPrentice(masterID, prenticeID, mp))
	{
		// 如果已经有这种师徒关系了,并且是未出师关系，就删掉它重新建立（容错）
		if (mp.finishTime != 0)
			return false;
		else 
		{
			DB_RemoveMasterPrentice(mp.index);
		}
	}
	if (!DB_AddMasterPrentice(masterID, prenticeID, outIndex, masterName, prenticeName))
	{
		errorCode = prentice_code_error_unknown;
	}

	if (errorCode==prentice_code_success)
	{
		GWLoginClient* pLcMaster = g_server.FindLoginClientByAccountId(pMaster->accountId);
		if (pLcMaster)
		{
			g_server.LogUserEvent(PLT_PrenticeSystem, masterID, pLcMaster->info.ip.c_str(), 1, rtFormatNumber(outIndex), masterID, masterName, prenticeID, prenticeName);
		}
        GWLoginClient* pLcPrentice = g_server.FindLoginClientByAccountId(pPrentice->accountId);
        if (pLcPrentice)
        {
            g_server.LogUserEvent(PLT_PrenticeSystem, prenticeID, pLcPrentice->info.ip.c_str(), 1, rtFormatNumber(outIndex), masterID, masterName, prenticeID, prenticeName);
        }
	}

	CMasterPrenticeCmd::Send_g2r_add_master_prentice_result(errorCode, outIndex, masterID, (char*)masterName, prenticeID, (char*)prenticeName);
	return true;
}

bool CMasterPrenticeMgr::RemoveMasterPrentice(long index, DWORD applyUserID, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName)
{
	short errorCode = prentice_code_success;

	GWUser *pUser = g_server.FindActorById(applyUserID);
	if (!pUser)
		return false;

	if (!DB_RemoveMasterPrentice(index))
	{
		errorCode = prentice_code_error_unknown;
	}

	//if (!DB_Set_LastRemoveMPTime(applyUserID, time(NULL)))
	//{
	//}
	//pUser->dwLastRemoveMPTime = (long)time(NULL);

	GWUser *pMaster = g_server.FindActorById(masterID);
	if (pMaster)
	{
		if (!DB_Set_LastRemoveMPTime(pMaster->userID, time(NULL)))
		{
		}
		pMaster->dwLastRemoveMPTime = (long)time(NULL);
	}
	GWUser *pPrentice = g_server.FindActorById(prenticeID);
	if (pPrentice)
	{
		if (!DB_Set_LastRemoveMPTime(pPrentice->userID, time(NULL)))
		{
		}
		pPrentice->dwLastRemoveMPTime = (long)time(NULL);
	}

	if (errorCode==prentice_code_success)
	{
		GWLoginClient* pLc = g_server.FindLoginClientByAccountId(pUser->accountId);
		if (pLc)
		{
			g_server.LogUserEvent(PLT_PrenticeSystem, masterID, pLc->info.ip.c_str(), 2, rtFormatNumber(index), masterID, masterName, prenticeID, prenticeName);
		}
	}

	CMasterPrenticeCmd::Send_g2r_remove_master_prentice_result(errorCode, index, masterID, (char*)masterName, prenticeID, (char*)prenticeName);
	return true;
}

bool CMasterPrenticeMgr::FinishMasterPrentice(long index, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName)
{
	GWUser *pMaster = g_server.FindActorById(masterID);
	GWUser *pPrentice = g_server.FindActorById(prenticeID);
	if (!pMaster || !pPrentice)
		return false;

	long onlineTime = g_server.GetUserOnlineTime(prenticeID);

	short errorCode = prentice_code_success;
	if (!DB_FinishMasterPrentice(index, onlineTime/3600))
	{
		errorCode = prentice_code_error_unknown;
	}

	if (errorCode==prentice_code_success)
	{
		GWLoginClient* pLcMaster = g_server.FindLoginClientByAccountId(pMaster->accountId);
		if (pLcMaster)
		{
			g_server.LogUserEvent(PLT_PrenticeSystem, masterID, pLcMaster->info.ip.c_str(), 3, rtFormatNumber(index), masterID, masterName, prenticeID, prenticeName);
		}
        GWLoginClient* pLcPrentice = g_server.FindLoginClientByAccountId(pPrentice->accountId);
        if (pLcPrentice)
        {
            g_server.LogUserEvent(PLT_PrenticeSystem, masterID, pLcPrentice->info.ip.c_str(), 3, rtFormatNumber(index), masterID, masterName, prenticeID, prenticeName);
        }
	}

	CMasterPrenticeCmd::Send_g2r_finish_master_prentice_result(errorCode, index, masterID, (char*)masterName, prenticeID, (char*)prenticeName, onlineTime);
	return true;
}

bool CMasterPrenticeMgr::Add_CreditToAdd(long index, DWORD masterID, short count)
{
	if (!DB_Add_CreditToAdd(index, masterID, count))
		return false;

	return true;
}

bool CMasterPrenticeMgr::DB_AddMasterPrentice(DWORD masterID, DWORD prenticeID, long &outIndex, const char* masterName, const char* prenticeName)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "INSERT INTO T_MasterPrentice (Idx,MasterID,MasterName,PrenticeID,PrenticeName,FinishTime,CreditToAdd,FinishOnlineTime) VALUES('',%d,'%s',%d,'%s','0000-00-00 00:00:00',%d,%d)", masterID, masterName, prenticeID, prenticeName, 0, 0);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_AddMasterPrentice] Can NOT Execute DB!\n");
		return false;
	}
	mysql_free_result(res);

	SMasterPrentice tmpMP;
	if (!DB_GetMasterPrentice(masterID, prenticeID, tmpMP))
	{
		return false;
	}
	outIndex = tmpMP.index;

	return true;
}

bool CMasterPrenticeMgr::DB_RemoveMasterPrentice(long index)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "DELETE FROM T_MasterPrentice WHERE Idx=%d", index);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_RemoveMasterPrentice] Can NOT Push to DB!\n");
		return false;
	}
	mysql_free_result(res);

	return true;
}

bool CMasterPrenticeMgr::DB_FinishMasterPrentice(long index, int onlineTime)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_MasterPrentice SET FinishTime=FROM_UNIXTIME(%u),FinishOnlineTime=%d WHERE Idx=%d", time(NULL), onlineTime, index);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_RemoveMasterPrentice] Can NOT Push to DB!\n");
		return false;
	}
	mysql_free_result(res);

	return true;
}

bool CMasterPrenticeMgr::DB_GetMasterPrentice(long index, SMasterPrentice& outMP)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT Idx,MasterID,MasterName,PrenticeID,PrenticeName,UNIX_TIMESTAMP(FinishTime),CreditToAdd,FinishOnlineTime FROM T_MasterPrentice WHERE Idx=%d", index);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_RemoveMasterPrentice] Can NOT Push to DB!\n");
		return false;
	}

	int count = mysql_num_rows(res);
	if (count!=1)
		return false;
	outMP.Clear();
	MYSQL_ROW row = mysql_fetch_row(res);
	outMP.index = atoi(row[0]);
	outMP.masterID = atoi(row[1]);
	if (row[2])
		outMP.masterName = row[2];
	outMP.prenticeID = atoi(row[3]);
	if (row[4])
		outMP.prenticeName = row[4];
	outMP.finishTime = atoi(row[5]);
	outMP.creditToAdd = atoi(row[6]);
	outMP.finishOnlineTime = atoi(row[7]);

	mysql_free_result(res);

	return true;
}

bool CMasterPrenticeMgr::DB_Set_CreditToAdd(DWORD index, int count)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_MasterPrentice SET CreditToAdd=%d WHERE Idx=%d", count, index);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_SetCreditToAdd] Can NOT Push to DB!\n");
		return false;
	}
	mysql_free_result(res);

	return true;
}

bool CMasterPrenticeMgr::DB_Add_CreditToAdd(long index, DWORD masterID, short count)
{
	SMasterPrentice mp;
	if (!DB_GetMasterPrentice(index, mp))
		return false;
	if (mp.masterID != masterID)
	{
		ERR2("[CMasterPrenticeMgr::DB_Add_CreditToAdd] Error! index=%d,masterID=%d\n", index, masterID);
		return false;
	}
	int nSetCount = mp.creditToAdd + count;

	if (!DB_Set_CreditToAdd(index, nSetCount))
		return false;

	return true;
}

bool CMasterPrenticeMgr::DB_Set_LastRemoveMPTime(DWORD userID, long lTime)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE `user` SET `LastRemoveMPTime`=FROM_UNIXTIME(%u) WHERE `ID`=%d", lTime, userID);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_Set_LastRemoveMPTime] Can NOT Push to DB!\n");
		return false;
	}
	mysql_free_result(res);

	return true;
}

bool CMasterPrenticeMgr::DB_GetMasterPrentice(DWORD masterID, DWORD prenticeID, SMasterPrentice& outMP)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT Idx,MasterID,MasterName,PrenticeID,PrenticeName,UNIX_TIMESTAMP(FinishTime),CreditToAdd,FinishOnlineTime FROM T_MasterPrentice WHERE MasterID=%d AND PrenticeID=%d LIMIT 1", masterID, prenticeID);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		ERR("[CMasterPrenticeMgr::DB_RemoveMasterPrentice] Can NOT Push to DB!\n");
		return false;
	}

	int count = mysql_num_rows(res);
	if (count!=1)
		return false;
	outMP.Clear();
	MYSQL_ROW row = mysql_fetch_row(res);
	outMP.index = atoi(row[0]);
	outMP.masterID = atoi(row[1]);
	if (row[2])
		outMP.masterName = row[2];
	outMP.prenticeID = atoi(row[3]);
	if (row[4])
		outMP.prenticeName = row[4];
	outMP.finishTime = atoi(row[5]);
	outMP.creditToAdd = atoi(row[6]);
	outMP.finishOnlineTime = atoi(row[7]);

	mysql_free_result(res);

	return true;
}

int CMasterPrenticeMgr::FindMaster(DWORD userID, std::vector<SMasterPrentice>& arrMPs)
{
	for (int i=0; i<(int)arrMPs.size(); i++)
	{
		if (arrMPs[i].masterID == userID)
			return i;
	}
	return -1;
}

int CMasterPrenticeMgr::FindPrentice(DWORD userID, std::vector<SMasterPrentice>& arrMPs)
{
	for (int i=0; i<(int)arrMPs.size(); i++)
	{
		if (arrMPs[i].prenticeID == userID)
			return i;
	}
	return -1;
}

void CMasterPrenticeMgr::DoCheckDataFromRegion(CG_CmdPacket* pPacket)
{
	long userID = 0;
	int i = 0;

	DWORD checkStartTick = rtGetMilliseconds();

	// Serialize Net Data
	DWORD region_masterID = 0;
	DWORD region_prenticeID = 0;
	std::vector<DWORD> region_arrPrentices;
	
	long nTmp1,nTmp2;
	short nStudentsCount;
	if (!pPacket->ReadLong(&userID))			return;
	if (!pPacket->ReadLong(&nTmp1))				return;
	if (!pPacket->ReadLong(&nTmp2))				return;
	region_masterID = nTmp1;
	if (!pPacket->ReadShort(&nStudentsCount))	return;
	for (i=0; i<nStudentsCount; i++)
	{
		if (!pPacket->ReadLong(&nTmp1))			return;
		if (!pPacket->ReadLong(&nTmp2))			return;
		region_arrPrentices.push_back(nTmp1);
	}

	GWUser* pUser = g_server.FindActorById(userID);
	if (!pUser)
		return;

	// Get Table Data
	std::vector<SMasterPrentice> table_Reconds;
	GetMasterPrentice(userID, table_Reconds);

	// Check Data
	std::vector<DWORD> arrAddMasterIndexes;
	std::vector<DWORD> arrAddMasterIDs;
	std::vector<std::string> arrAddMasterNames;
	std::vector<DWORD> arrRemoveMasterIDs;
	std::vector<DWORD> arrAddPrenticeIndexes;
	std::vector<DWORD> arrAddPrenticeIDs;
	std::vector<std::string> arrAddPrenticeNames;
	std::vector<DWORD> arrRemovePrenticeIDs;
	int nAddCredits = 0;

	// 师父解除关系，徒弟不在线，对徒弟的纠正
	if (region_masterID!=0)
	{
		int nTmp = FindMaster(region_masterID, table_Reconds);
		if (nTmp>=0)
		{
			SMasterPrentice& mp = table_Reconds[nTmp];
			if (mp.finishTime==0)
			{ // 同步！
			}
			else 
			{ // ## 出师了，但回档
				arrRemoveMasterIDs.push_back(region_masterID);
			}
		}
		else
		{ // ** 我的师父主动解除了关系
			//LOG2("师父主动解除了关系, master=%d,prentice=%d\n", region_masterID, userID);
			DB_Set_LastRemoveMPTime(userID, time(NULL));
			pUser->dwLastRemoveMPTime = (long)time(NULL);

			arrRemoveMasterIDs.push_back(region_masterID);
			string StrItemID = "";
			g_server.m_mail.AddMail(userID, 0, R(MSG_PRENSYS_REMOVE_MAIL_TITLE),
				R(MSG_PRENSYS_MASTER_REMOVE_MAIL_CONTENT) , false,StrItemID,0);
		}
	}

	for (i=0; i<(int)region_arrPrentices.size(); i++)
	{
		int nTmpCredit = 0;
		region_prenticeID = region_arrPrentices[i];
		int nTmp = FindPrentice(region_prenticeID, table_Reconds);
		if (nTmp>=0)
		{
			SMasterPrentice& mp = table_Reconds[nTmp];
			if (mp.finishTime==0)
			{ // 同步！
				nTmpCredit = mp.creditToAdd;
				if (nTmpCredit != 0)
					DB_Set_CreditToAdd(mp.index, 0);
			}
			else 
			{ // ## 出师了，但回档
				arrRemovePrenticeIDs.push_back(region_prenticeID);
			}
		}
		else 
		{ // ** 我的徒弟主动解除了关系
			//LOG2("师父主动解除了关系, master=%d,prentice=%d\n", region_masterID, userID);
			DB_Set_LastRemoveMPTime(userID, time(NULL));
			pUser->dwLastRemoveMPTime = (long)time(NULL);

			arrRemovePrenticeIDs.push_back(region_prenticeID);
			string StrItemID = "";
			g_server.m_mail.AddMail(userID, 0, R(MSG_PRENSYS_REMOVE_MAIL_TITLE), 
				R(MSG_PRENSYS_PRENTICE_REMOVE_MAIL_CONTENT), false,StrItemID,0);
		}
		nAddCredits += nTmpCredit;
	}

	//@@@@@ 没有处理过的纪录 (暂时不作处理，已经有相应的容错)

	// 如果需要，就更新region师徒数据
	if (!arrAddMasterIDs.empty() || !arrRemoveMasterIDs.empty() || !arrAddPrenticeIDs.empty() || !arrRemovePrenticeIDs.empty())
	{
		CMasterPrenticeCmd::Send_g2r_master_prentice_update(userID, arrAddMasterIndexes, arrAddMasterIDs, arrAddMasterNames, arrRemoveMasterIDs, arrAddPrenticeIndexes, arrAddPrenticeIDs, arrAddPrenticeNames, arrRemovePrenticeIDs);
	}
	if (nAddCredits != 0)
	{
		CMasterPrenticeCmd::Send_g2r_master_prentice_credit(userID, nAddCredits);
		//LOG2("[CMasterPrenticeMgr::DoCheckDataFromRegion] User(id=%d) add credit %d\n", userID, nAddCredits);
	}

	// 发送徒弟在现时间信息
	for (i=0; i<(int)region_arrPrentices.size(); i++)
	{
		DWORD prenticeID = region_arrPrentices[i];
		long onlineTime = g_server.GetUserOnlineTime(prenticeID);
		CMasterPrenticeCmd::Send_g2r_prentice_onlinetime(userID, prenticeID, onlineTime);
	}
	for (i=0; i<(int)arrAddPrenticeIDs.size(); i++)
	{
		DWORD prenticeID = arrAddPrenticeIDs[i];
		long onlineTime = g_server.GetUserOnlineTime(prenticeID);
		CMasterPrenticeCmd::Send_g2r_prentice_onlinetime(userID, prenticeID, onlineTime);
	}

	//LOG1("[CMasterPrenticeMgr::DoCheckDataFromRegion] Use time = %d\n", rtGetMilliseconds()-checkStartTick);
}

bool CMasterPrenticeMgr::WithdrawMPGift(DWORD masterID, int lev)
{
	GWUser *pUser = g_server.FindActorById(masterID);
	if (!pUser)
		return false;
	if (!PrenSysGiftLevel_IsValid(lev))
		return false;

	if (IsPrenSysGiftWithdrawed(pUser->dwWithdrawMPGift, lev))
	{
		CMasterPrenticeCmd::Send_g2r_master_prentice_gift(pUser->userID, lev, prentice_code_gift_withdrawed);
		return false;
	}

	SetPrenSysGiftSign(pUser->dwWithdrawMPGift, lev);
	CMasterPrenticeCmd::Send_g2r_master_prentice_gift(pUser->userID, lev, prentice_code_success);

	return true;
}

void CMasterPrenticeMgr::SetPrenSysGiftSign(long& dwValue, int lev)
{
	if (!PrenSysGiftLevel_IsValid(lev))
	{
		CHECK(0);
		return;
	}

	dwValue |= (1 << lev);
}

bool CMasterPrenticeMgr::IsPrenSysGiftWithdrawed(long dwValue, int lev)
{
	if (!PrenSysGiftLevel_IsValid(lev))
		return false;

	return ( (dwValue&(1<<lev)) != 0);
}


void CMasterPrenticeCmd::Send_g2r_add_master_prentice_result(short errorCode, long index, DWORD masterID, char* masterName, DWORD prenticeID, char* prenticeName)
{
	int sendRegionID = -1;
	GWUser *pUser = g_server.FindActorById(masterID);
	if (pUser)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_add_master_prentice_result);
		cmd.WriteShort(errorCode);
		cmd.WriteLong(index);
		cmd.WriteLong(masterID);
		cmd.WriteLong(prenticeID);
		cmd.WriteString(masterName);
		cmd.WriteString(prenticeName);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));

		sendRegionID = pUser->regionID;
	}
	else 
	{
	}

	pUser = g_server.FindActorById(prenticeID);
	if (pUser)
	{
		if (pUser->regionID != sendRegionID)
		{
			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_add_master_prentice_result);
			cmd.WriteShort(errorCode);
			cmd.WriteLong(index);
			cmd.WriteLong(masterID);
			cmd.WriteLong(prenticeID);
			cmd.WriteString(masterName);
			cmd.WriteString(prenticeName);
			g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
		}
	}
	else 
	{
	}
}

void CMasterPrenticeCmd::Send_g2r_remove_master_prentice_result(short errorCode, long index, DWORD masterID, char* masterName, DWORD prenticeID, char* prenticeName)
{
	int sendRegionID = -1;
	GWUser *pUser = g_server.FindActorById(masterID);
	if (pUser)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_remove_master_prentice_result);
		cmd.WriteShort(errorCode);
		cmd.WriteLong(index);
		cmd.WriteLong(masterID);
		cmd.WriteLong(prenticeID);
		cmd.WriteString(masterName);
		cmd.WriteString(prenticeName);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));

		sendRegionID = pUser->regionID;
	}
	else 
	{
	}

	pUser = g_server.FindActorById(prenticeID);
	if (pUser)
	{
		if (pUser->regionID != sendRegionID)
		{
			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_remove_master_prentice_result);
			cmd.WriteShort(errorCode);
			cmd.WriteLong(index);
			cmd.WriteLong(masterID);
			cmd.WriteLong(prenticeID);
			cmd.WriteString(masterName);
			cmd.WriteString(prenticeName);
			g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
		}
	}
	else 
	{
	}
}

void CMasterPrenticeCmd::Send_g2r_finish_master_prentice_result(short errorCode, long index, DWORD masterID, char* masterName, DWORD prenticeID, char* prenticeName, long onlineTime)
{
	int sendRegionID = -1;
	GWUser *pUser = g_server.FindActorById(masterID);
	if (pUser)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_finish_master_prentice_result);
		cmd.WriteShort(errorCode);
		cmd.WriteLong(index);
		cmd.WriteLong(masterID);
		cmd.WriteLong(prenticeID);
		cmd.WriteString(masterName);
		cmd.WriteString(prenticeName);
		cmd.WriteLong(onlineTime);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));

		sendRegionID = pUser->regionID;
	}
	else 
	{
	}

	pUser = g_server.FindActorById(prenticeID);
	if (pUser)
	{
		if (pUser->regionID != sendRegionID)
		{
			CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
			cmd.BeginWrite();
			cmd.WriteShort(g2r_finish_master_prentice_result);
			cmd.WriteShort(errorCode);
			cmd.WriteLong(index);
			cmd.WriteLong(masterID);
			cmd.WriteLong(prenticeID);
			cmd.WriteString(masterName);
			cmd.WriteString(prenticeName);
			cmd.WriteLong(onlineTime);
			g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
		}
	}
	else 
	{
	}
}

void CMasterPrenticeCmd::Send_g2r_master_prentice_update(DWORD userID, std::vector<DWORD>& arrAddMasterIndexes, std::vector<DWORD>& arrAddMasterIDs, std::vector<std::string>& arrAddMasterNames, std::vector<DWORD>& arrRemoveMasterIDs, std::vector<DWORD>& arrAddPrenticeIndexes, std::vector<DWORD>& arrAddPrenticeIDs, std::vector<std::string>& arrAddPrenticeNames, std::vector<DWORD>& arrRemovePrenticeIDs)
{
	GWUser *pUser = g_server.FindActorById(userID);
	if (pUser)
	{
		int i = 0;
		short nAddMasterCount = (short)arrAddMasterIndexes.size();
		short nRemoveMasterCount = (short)arrRemoveMasterIDs.size();
		short nAddPrenticeCount = (short)arrAddPrenticeIndexes.size();
		short nRemovePrenticeCount = (short)arrRemovePrenticeIDs.size();
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_master_prentice_update);
		cmd.WriteLong(userID);
		cmd.WriteShort(nAddMasterCount);
		for (i=0; i<nAddMasterCount; i++)
		{
			cmd.WriteLong(arrAddMasterIndexes[i]);
			cmd.WriteLong(arrAddMasterIDs[i]);
			cmd.WriteString((char*)arrAddMasterNames[i].c_str());
		}
		cmd.WriteShort(nRemoveMasterCount);
		for (i=0; i<nRemoveMasterCount; i++)
		{
			cmd.WriteLong(arrRemoveMasterIDs[i]);
		}
		cmd.WriteShort(nAddPrenticeCount);
		for (i=0; i<nAddPrenticeCount; i++)
		{
			cmd.WriteLong(arrAddPrenticeIndexes[i]);
			cmd.WriteLong(arrAddPrenticeIDs[i]);
			cmd.WriteString((char*)arrAddPrenticeNames[i].c_str());
		}
		cmd.WriteShort(nRemovePrenticeCount);
		for (i=0; i<nRemovePrenticeCount; i++)
		{
			cmd.WriteLong(arrRemovePrenticeIDs[i]);
		}
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
	}
}

void CMasterPrenticeCmd::Send_g2r_master_prentice_credit(int playerSendTo, short credit)
{
	GWUser *pUser = g_server.FindActorById(playerSendTo);
	if (pUser)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_master_prentice_credit);
		cmd.WriteLong(playerSendTo);
		cmd.WriteShort(credit);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
	}
}

void CMasterPrenticeCmd::Send_g2r_prentice_onlinetime(int playerSendTo, int prenticeID, long onlineTime)
{
	GWUser *pUser = g_server.FindActorById(playerSendTo);
	if (pUser)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_prentice_onlinetime);
		cmd.WriteLong(playerSendTo);
		cmd.WriteLong(prenticeID);
		cmd.WriteLong(onlineTime);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
	}
}

void CMasterPrenticeCmd::Send_g2r_master_prentice_gift(int playerSendTo, long lev, short errorCode)
{
	GWUser *pUser = g_server.FindActorById(playerSendTo);
	if (pUser)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_master_prentice_gift);
		cmd.WriteLong(playerSendTo);
		cmd.WriteLong(lev);
		cmd.WriteShort(errorCode);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(pUser->regionID));
	}
}
