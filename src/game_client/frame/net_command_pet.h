#ifndef NET_COMMAND_PET_H
#define NET_COMMAND_PET_H

bool net_r2c_ctrl_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_fuse_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_catch_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_query_response_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_store_response_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_bank_withdraw_response_pet(short cmdID, CG_CmdPacket *pPacket);
bool net_r2c_pet_linkup(short cmdID, CG_CmdPacket *pPacket);

class CPetNetHelper
{
public:
	static bool NetSend_c2r_ctrl_pet(char ctrlType,int param);
	static bool NetSend_c2r_fuse_pet_start(char fuseType,SItemID item1,SItemID item2,SItemID item3);
	static bool NetSend_c2r_fuse_pet_stop(char fuseType);
	static bool NetSend_c2r_catch_pet_start(DWORD petID);
	static bool NetSend_c2r_catch_pet_stop();

	static bool NetSend_c2r_bank_query_pet(DWORD npcID, const char* pass);
	static bool NetSend_c2r_bank_store_pet(long petItemID, DWORD npcID);
	static bool NetSend_c2r_bank_withdraw_pet(long petItemID, DWORD npcID);
};
#endif