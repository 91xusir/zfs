#ifndef _REGION_PRETICE_H
#define _REGION_PRETICE_H


class CRegionPrenticeMgr
{
public:
	CRegionPrenticeMgr();
	~CRegionPrenticeMgr();
	bool OnRecvGW_AddMasterPrentice(CG_CmdPacket *cmd);
	bool OnRecvGW_RemoveMasterPrentice(CG_CmdPacket *cmd);
	bool OnRecvGW_FinishMasterPrentice(CG_CmdPacket *cmd);
	bool Judge_AddMasterPrentice(CRegionCreature* pMaster, CRegionCreature* pPrentice);
	bool Judge_RemoveMasterPrentice(CRegionCreature* pApplyCre, const char* masterName, const char* prenticeName, long& outIndex, long& outMasterID, long& outPrenticeID);
	bool Judge_FinishMasterPrentice(CRegionCreature* pMaster, CRegionCreature* pPrentice, long& outIndex);
	bool Judge_WithdrawMPGift(CRegionCreature* pMaster, int lev, short& errorCode);

	bool FinishMasterPrentice(CRegionCreature* pMaster, CRegionCreature* pPrentice);
	bool WithdrawMPGift(CRegionCreature* pMaster, int lev);

	void OnAfterUserLevelup(CRegionCreature* pCre, int LevelupCount);
	void OnAfterUserLevelDown(CRegionCreature* pCre,int LevelDownCount);
	bool IsUserPrenSysLocked(CRegionUser* pUser);
	void LockUserPrenSys(CRegionUser* pUser);
	void UnlockUserPrenSys(CRegionUser* pUser);

	void QueryPrenticesOnlineTime(CRegionUser* pUser);

private:

};


int PrenticeCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd);

#define PRENTICE_CMD_FUNCTION(func)				int func(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_add_apply);
PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_add_response);
PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_remove);
PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_finish_apply);
PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_finish_response);
PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_gift);


class CRegionPrenticeCmd
{
public:
	static void Build_r2c_prentice_add_apply(DWORD playerOID);
	static void Build_r2c_prentice_finish_apply(DWORD playerOID);
	static void Build_r2c_prentice_add_result(short errorCode, const char* MasterName, const char* PrenticeName);
	static void Build_r2c_prentice_remove_result(short errorCode, const char* MasterName, const char* PrenticeName);
	static void Build_r2c_prentice_finish_result(short errorCode, const char* MasterName, const char* PrenticeName);
	static void Build_r2c_prentice_onlinetime(long prenticeID, long onlineTime);
	static void Build_r2c_prentice_gift_result(short errorCode, long lev);

	static void NetSend_r2g_add_master_prentice(DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName);
	static void NetSend_r2g_remove_master_prentice(DWORD applyUserID, long index, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName);
	static void NetSend_r2g_finish_master_prentice(long index, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName);
	static void NetSend_r2g_master_prentice_credit(long index, DWORD userID, short count);
	static void NetSend_r2g_prentice_onlinetime(long userID, long prenticeID);
	static void NetSend_r2g_master_prentice_gift(long userID, long lev);
};

extern CRegionPrenticeMgr g_PrenticeMgr;

#endif // _REGION_PRETICE_H
