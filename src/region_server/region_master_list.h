
#ifndef REGION_MASTER_LIST_H 
#define REGION_MASTER_LIST_H

#include "master_list_const.h"

class CRegionCreature;

class CRegionMasterListManager
{
public:
	int ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd);//��Client�õ�һ������
	int ReceiveFromGameWorld(DWORD vUserDBID,CG_CmdPacket* cmd);	//��GW�õ�һ������

	void SetMaxCount(int vMax);

private:
	void Add(CRegionCreature* vpCmdGiver);
	void GetList(CRegionCreature* vpCmdGiver);
	
	CG_CmdPacket& GameWorldPacket();
	CG_CmdPacket& ClientPacket();
	

	void SendToGameWorld();			//��һ������GameWorld
	void SendToClient(CRegionCreature* vpCreature);//��һ������Client
private:
	CG_CmdPacket	mSendPacket;
	CMasterList		mShangList;
	CMasterList		mZhouList;
	//add by Tianh  09.12.11
	CMasterList     mNeutralList;
};





#endif//REGION_MASTER_LIST_H


