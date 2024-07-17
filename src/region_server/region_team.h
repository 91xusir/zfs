


#ifndef REGION_TEAM_H
#define REGION_TEAM_H

#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "team_data.h"
typedef unsigned char byte;
class CRegionCreature;
class CG_CmdPacket;
struct SR2rData;


class CServerTeam 
{
public:
	CServerTeam(CRegionCreature* vpMaster,DWORD vSelfRegionID,DWORD vSelfDBID,std::string& vrSelfName,char vSelfHead);
	~CServerTeam();


	//------自己----------
	//自己通过名字邀请别人加入队伍
	void InviteOtherbyName(const char* name);
	//自己邀请别人加入队伍
	void FindInviteOther(DWORD vOtherRegionID);
	void InviteOther(DWORD OtherDBID);
	//自己驱逐别人从队伍中
	void BanishOther(DWORD vOtherDBID);
	void BanishOtherByRegionID(DWORD vOtherRegionID);
	//自己离开队伍
	void SelfLeave();
	//自己解散队伍
	void Dismiss();

	//自己同意别人的邀请
	void AgreeOtherInvite(DWORD vOtherDBID);
	//自己拒绝别人的邀请
	void RefuseOtherInvite(DWORD vOtherDBID);

	//------服务器端特有------
	//自己断线
	//void OnDisconnect();
	//自己下线
	//void OnOffline();
	//自己的对队伍有影响的数值变化
	void DateChangeAffectTeam();
	
	//------对客户端--------
	//接到上传的消息
	int ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd);
	//发送消息到客户端
	//void SendToClient(byte vOperation,DWORD vParameter,std::string* vpName=NULL,DWORD vDBIDInMessage=0);

	//------对服务端--------
	//接到服务端的消息
	int ReceiveFromGameWorld(CG_CmdPacket *cmd);
	//void SendToGameWorld();

	//void ReceiveFromServer(SR2rData&  vrR2rSendCmd);
	//发送消息到服务端
	//void SendToServer(byte vOperation,DWORD vSendToID,DWORD vParameter=0,std::string* vpName=NULL,DWORD vDBIDInMessage=0);

	//void Load(string& vrString){ vrString>>mData;};
	//void Save(string& vrString){ vrString<<mData;};
	CTeamData			mData;
	void StartOrderPick();
	void NextPicker();
	unsigned long GetOrderPickDBID();
	void SendTeamPickerMsgToGWS(const char* pMsg);
private:
	void FreeOrLockTeam(char cFree);
	void ApplyAddTeam( unsigned long vOtherID);
	void AgreeApply(unsigned long vOtherID);
	void RefuseTeam(unsigned long vOtherID);
	void TransferCaptain(unsigned long vOtherID);
	void SetPickType(char nType);
	void SendTeamPicker(char nType);
	
private:
	CG_CmdPacket* GameWorldPacket()
	{mSendPacket.BeginWrite();mSendPacket.WriteShort(r2g_team);mSendPacket.WriteLong(mSelfDBID);return &mSendPacket;};	//得到包
	
	CG_CmdPacket* ClientPacket()
	{mSendPacket.BeginWrite();mSendPacket.WriteShort(r2c_subsystem_team);return &mSendPacket;};
	
	void SendToGameWorld();			//送一个包到GameWorld
	void SendToClient(){mpMaster->RecvCmd(&mSendPacket);};						//送一个包到Client		

	CRegionCreature*	FindByRegionID(DWORD vRegionID);
	DWORD				RegionID2DBID(DWORD vOtherRegionID);
	DWORD               RegionIDbyName(const char* name);
	
	static CG_CmdPacket mSendPacket;

	DWORD				mSelfRegionID;
	DWORD				mSelfDBID;
	std::string			mSelfName;
	char				mSelfHead;


	CRegionCreature*	mpMaster;

	int m_nPickerIncex;

	//void				GetDataFromMaster();
	//bool				mServerWaitOtherAddSelf;//等待其它人服务端加入自己
};

#endif//REGION_TEAM_H



//别人将你成功加入队伍
//void OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName);
//别人将你加入失败
//void OtherAddSelfFault(DWORD vOtherDBID);
//别人不存在
//void OtherNotExist(DWORD vOtherDBID);

//-------别人-----------
//别人同意加入队伍
//void OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName);
//别人拒绝加入队伍
//void OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName);
//别人的对队伍有影响的数值变化
//void OtherDateChangeAffectTeam(DWORD vOtherDBID);


//别人邀请自己加入队伍
//void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);

//别人驱逐自己出队伍
//void OtherBanish(DWORD vOtherDBID);
//别人离开队伍
//void OtherLeave(DWORD vOtherDBID);
//别人解散队伍
//void OtherDismiss(DWORD vOtherDBID);

