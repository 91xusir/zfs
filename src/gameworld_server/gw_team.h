
#ifndef GW_TEAM_H
#define	GW_TEAM_H

//#include <hash_map>
#include <vector>

#include "team_data.h"


class CG_CmdPacket;
struct GWUser;

class GwTeamManager
{
public:
	GwTeamManager(){};
	~GwTeamManager(){};

	void OnOffLine(GWUser* vpUser){SelfLeave(vpUser);};//某人下线了

	void ReceiveFromRegion(CG_CmdPacket* vpPacket);

	//------自己----------
	//自己邀请别人加入队伍
	void InviteOther(DWORD vSelfDBID,DWORD vOtherDBID);

	void InviteOtherbyName(GWUser *pGWUser,DWORD vOtherDBID);
	//自己驱逐别人从队伍中
	void BanishOther(DWORD vSelfDBID,DWORD vOtherDBID);
	//自己离开队伍
	void SelfLeave(DWORD vSelfDBID);
	//自己离开队伍
	void SelfLeave(GWUser* vpSelf);
	//自己解散队伍
	void Dismiss(DWORD vSelfDBID);

	//自己同意别人的邀请
	void AgreeOtherInvite(DWORD vSelfDBID,DWORD vOtherDBID);
	//自己拒绝别人的邀请
	void RefuseOtherInvite(DWORD vSelfDBID,DWORD vOtherDBID);

	void SendDateToWhenFriend(DWORD SelfDBID,DWORD OtherDBID);

	//自己的对队伍有影响的数值变化
	void DateChangeAffectTeam(DWORD vSelfDBID,int vHp,int vMaxHp,int vMp,int vMaxMp,char vLevel,long X,long Y,const char* MapName,long mCombatPower);




	//-------别人-----------
	//别人同意加入队伍
	void OtherAgreeInvite(GWUser* vpOther,GWUser* vpSelf);
	//别人邀请自己加入队伍
	void OtherInvite(GWUser* vpOther,GWUser* vpSelf);
	//别人离开队伍
	//void OtherLeave(DWORD vOtherDBID,GWUser* vpSelf);


	void SwitchRegion(GWUser* vpSelf);
	
	bool SetPickType(unsigned long vSelfDBID, char cType);
private:
	void ApplyAddTeam(unsigned long vSelfDBID, unsigned long vOtherID);
	void FreeOrLockTeam(DWORD vSelfDBID, char cFree);
	void AgreeApply(unsigned long vSelfDBID, unsigned long vOtherID);
	void RefuseTeam(unsigned long vSelfDBID, unsigned long vOtherID);
	void TransferCaptain(unsigned long vSelfDBID, unsigned long vOtherID);
	
	void SendToRegionAddTeamMember(GWUser* vpSendToUser,DWORD vMemberDBID,const char* vpMemberName,int headImageID,char Metier,long BornTag,long actorID);
	void SendToRegionDeleteTeamMember(GWUser* vpSendToUser,DWORD vMemberDBID);
	void SendToRegionUpdateOneMemberData(GWUser* vpSendToUser, DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp,char vLevel,long X,long Y,const char* MapName,long mCombatPower);
	void SendToRegionUpdateAllMemberData(GWUser* vpSendToUser);
	void SendToRegionRequestMemberData(GWUser* vpSendToUser);
	void SendToRegionTeamMessage(GWUser* vpSendToUser,int vMessageID);
	void SendToRegionTeamMessageWithID(GWUser* vpSendToUser,DWORD vDBIDInMessage,int vMessageID);
	void SendToRegionOtherAgreeInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName,char vOtherHead,char metier, long BornTag,long actorID);
	void SendToRegionOtherRefuseInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName);
	void SendToRegionOtherInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName);
	void SendToRegionOtherBanish(GWUser* vpSendToUser,DWORD vOtherDBID);
	void SendToRegionOtherLeave(GWUser* vpSendToUser,DWORD vOtherDBID);
	void SendToRegionOtherDismiss(GWUser* vpSendToUser,DWORD vOtherDBID);


	CG_CmdPacket& RegionPack(DWORD vSendToDBID);
	void SendToRegion(GWUser* vpSendToUser);

	GWUser* FindUser(DWORD vUserDBID);
	void SendTeamPicker(DWORD vUserDBID, const char* pMsg);
};

//---------------------------------------------------------------------------

#endif//GW_TEAM_H
