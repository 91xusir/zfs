#ifndef _GW_PRENTICE_H
#define _GW_PRENTICE_H

#include "master_prentice_base.h"

class CMasterPrenticeMgr
{
public:
	CMasterPrenticeMgr() {}
	~CMasterPrenticeMgr() {}

	void GetMasterPrentice(DWORD userID, std::vector<SMasterPrentice>& arrMPs);
	void GetMasterPrentice(DWORD userID, std::vector<DWORD>& MastersID, std::vector<std::string>& MastersName, std::vector<DWORD>& PrenticesID, std::vector<std::string>& PrenticesName, std::vector<DWORD>& FinishedID, std::vector<std::string>& FinishedName);
	bool AddMasterPrentice(DWORD masterID, DWORD prenticeID, long &outIndex, const char* masterName, const char* prenticeName);	// 拜师
	bool RemoveMasterPrentice(long index, DWORD applyUserID, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName);	// 解除师徒关系
	bool FinishMasterPrentice(long index, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName);	// 出师
	bool Add_CreditToAdd(long index, DWORD masterID, short count);

	int FindMaster(DWORD userID, std::vector<SMasterPrentice>& arrMPs);
	int FindPrentice(DWORD userID, std::vector<SMasterPrentice>& arrMPs);
	void DoCheckDataFromRegion(CG_CmdPacket* pPacket);
	bool WithdrawMPGift(DWORD masterID, int lev);

private:
	bool DB_AddMasterPrentice(DWORD masterID, DWORD prenticeID, long &outIndex, const char* masterName, const char* prenticeName);
	bool DB_RemoveMasterPrentice(long index);
	bool DB_FinishMasterPrentice(long index, int onlineTime);
	bool DB_GetMasterPrentice(long index, SMasterPrentice& outMP);
	bool DB_GetMasterPrentice(DWORD masterID, DWORD prenticeID, SMasterPrentice& outMP);
	bool DB_Set_CreditToAdd(DWORD index, int count);
	bool DB_Add_CreditToAdd(long index, DWORD masterID, short count);
	bool DB_Set_LastRemoveMPTime(DWORD userID, long lTime);

	void SetPrenSysGiftSign(long& dwValue, int lev);
	bool IsPrenSysGiftWithdrawed(long dwValue, int lev);
};

class CMasterPrenticeCmd
{
public:
	static void Send_g2r_add_master_prentice_result(short errorCode, long index, DWORD masterID, char* masterName, DWORD prenticeID, char* prenticeName);
	static void Send_g2r_remove_master_prentice_result(short errorCode, long index, DWORD masterID, char* masterName, DWORD prenticeID, char* prenticeName);
	static void Send_g2r_finish_master_prentice_result(short errorCode, long index, DWORD masterID, char* masterName, DWORD prenticeID, char* prenticeName, long onlineTime);
	static void Send_g2r_master_prentice_update(DWORD userID, std::vector<DWORD>& arrAddMasterIndexes, std::vector<DWORD>& arrAddMasterIDs, std::vector<std::string>& arrAddMasterNames, std::vector<DWORD>& arrRemoveMasterIDs, std::vector<DWORD>& arrAddPrenticeIndexes, std::vector<DWORD>& arrAddPrenticeIDs, std::vector<std::string>& arrAddPrenticesName, std::vector<DWORD>& arrRemovePrenticeIDs);
	static void Send_g2r_master_prentice_credit(int playerSendTo, short credit);
	static void Send_g2r_prentice_onlinetime(int playerSendTo, int prenticeID, long onlineTime);
	static void Send_g2r_master_prentice_gift(int playerSendTo, long lev, short errorCode);
};

extern CMasterPrenticeMgr g_MasterPrenticeMgr;

#endif//_GW_PRENTICE_H

