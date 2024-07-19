
#ifndef REGION_MASTER_LIST_H 
#define REGION_MASTER_LIST_H

#include "master_list_const.h"

class CRegionCreature;

class CRegionMasterListManager
{
public:
	int ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd);//从Client得到一个命令
	int ReceiveFromGameWorld(DWORD vUserDBID,CG_CmdPacket* cmd);	//从GW得到一个命令

	void SetMaxCount(int vMax);

private:
	void Add(CRegionCreature* vpCmdGiver);
	void GetList(CRegionCreature* vpCmdGiver);
	
	CG_CmdPacket& GameWorldPacket();
	CG_CmdPacket& ClientPacket();
	

	void SendToGameWorld();			//送一个包到GameWorld
	void SendToClient(CRegionCreature* vpCreature);//送一个包到Client
private:
	CG_CmdPacket	mSendPacket;
	CMasterList		mShangList;
	CMasterList		mZhouList;
	//add by Tianh  09.12.11
	CMasterList     mNeutralList;
};





#endif//REGION_MASTER_LIST_H


