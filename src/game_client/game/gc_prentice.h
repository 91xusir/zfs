#ifndef GC_PRENTICE_H
#define GC_PRENTICE_H

class GcPrenticeMgr
{
public:
	GcPrenticeMgr();
	~GcPrenticeMgr();
	bool Judge_AddMasterPrentice(GcActor* pMaster);
	bool Judge_RemoveMasterPrentice(GcActor* pApplyActor, const char* masterName, const char* prenticeName);
	bool Judge_FinishMasterPrentice(GcActor* pMaster);
	bool Judge_WithdrawMPGift(long lev);

	void cbPrenticeAdd_Agree(RtwWidget* sender, RtwEventDelegate* e);
	void cbPrenticeAdd_Disagree(RtwWidget* sender, RtwEventDelegate* e);
	void cbPrenticeFinish_Agree(RtwWidget* sender, RtwEventDelegate* e);
	void cbPrenticeFinish_Disagree(RtwWidget* sender, RtwEventDelegate* e);
	void cbOnMasterRemove(RtwWidget* sender, RtwEventDelegate* e);
	void cbOnPrenticeRemove(RtwWidget* sender, RtwEventDelegate* e);

	UIFormTextMsg* m_pFrmRemove;

private:

};
extern GcPrenticeMgr g_GcPrenticeMgr;

class GcPrenticeCmd
{
public:
	static bool NetSend_c2r_prentice_add_apply(DWORD playerDBID);
	static bool NetSend_c2r_prentice_add_response(char bAgree, DWORD playerOID);
	static bool NetSend_c2r_prentice_remove(const char* masterName, const char* prenticeName);
	static bool NetSend_c2r_prentice_finish_apply(DWORD playerDBID);
	static bool NetSend_c2r_prentice_finish_response(char bAgree, DWORD playerOID);
	static bool NetSend_c2r_prentice_gift(long lev);
};

bool PrenticeCmdProcess_client(CG_CmdPacket *cmd);

bool net_r2c_prentice_add_apply(CG_CmdPacket *pPacket);
bool net_r2c_prentice_finish_apply(CG_CmdPacket *pPacket);
bool net_r2c_prentice_add_result(CG_CmdPacket *pPacket);
bool net_r2c_prentice_remove_result(CG_CmdPacket *pPacket);
bool net_r2c_prentice_finish_result(CG_CmdPacket *pPacket);
bool net_r2c_prentice_onlinetime(CG_CmdPacket *pPacket);
bool net_r2c_prentice_gift_result(CG_CmdPacket *pPacket);


#endif//GC_PRENTICE_H
