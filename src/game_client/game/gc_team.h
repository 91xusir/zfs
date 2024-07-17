
#ifndef GC_TEAM_H
#define GC_TEAM_H

#include "team_data.h"
#include "game_listener.h"
#include "UI_form_Team.h"
#include <string>
typedef unsigned char BYTE;
class CG_CmdPacket;
class CUI_form_Team;

class CClientTeam
{
public:
	CClientTeam();
	virtual ~CClientTeam();

	void SetID(DWORD vSelfRegionID,DWORD vSelfDBID,std::string& vrSelfName,char vSelfHead);

	//------自己----------
	//自己邀请别人加入队伍
	 void InviteOther(DWORD vOtherRegionID);
	 void InviteOther(std::string& vrOtherName);
	//自己驱逐别人从队伍中
	void BanishOtherByDBID(DWORD vOtherDBID);
	void BanishOtherByRegionID(DWORD vOtherRegionID);

	//自己离开队伍
	void SelfLeave();
	//自己解散队伍
	void Dismiss();

	//自己同意别人的邀请
	void AgreeOtherInvite(DWORD vOtherDBID);
	//自己拒绝别人的邀请
	void RefuseOtherInvite(DWORD vOtherDBID);

	//自己申请加入别人的队伍
	void ApplyOtherTeam(DWORD vOtherRegionID);
	void ApplyOtherTeam(std::string& vrOtherName);
	//自己同意别人的申请
	void AgreeOtherApply(DWORD vOtherRegionID);
	//自己拒绝别人的申请
	void RefuseOtherApply(DWORD vOtherRegionID);


	//-------别人-----------

	//别人同意加入队伍
	void OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName,char vrOtherhead,byte Metier,long BornTag,long actorID);
	//别人拒绝加入队伍
	void OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName);

	//别人邀请自己加入队伍
	void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);

	//别人驱逐自己出队伍
	void OtherBanish(DWORD vOtherDBID);
	//别人离开队伍
	void OtherLeave(DWORD vOtherDBID);
	//别人解散队伍
	void OtherDismiss(DWORD vOtherDBID);

	//别人申请加入自己的队伍
	void OtherApplyTeam(DWORD vOtherDBID,std::string& vrOtherName,DWORD vLevel,short metier/*,DWORD vUnionDBID,std::string& vrUnionName*/,int vrAni,int vrPow);
	//别人同意自己的申请
	void OtherAgreeApply(DWORD vOtherDBID);
	//别人拒绝自己的申请
	void OtherRefuseApply(DWORD vOtherDBID);
	//转让队长
	virtual void TransferCaptain(DWORD vOtherDBID);

	//-----客户端特有的-----
	//队伍数值变化
	virtual void TeamDataChange()=0;
	virtual void TeamMemberDataChange(DWORD vMemberDBID)=0;


	//------对服务器--------
	//上传消息
	void SendToServer(byte vOperation,DWORD vOtherID);
	//接到消息
	void ReceiveFromServer(CG_CmdPacket *pPacket);

	//-------对用户---------
	void Message(DWORD vDBIDInMessage,const std::string& vrMessage);
	void MessageWithChoice(DWORD vFromDBID,DWORD vDBIDInMessage,const std::string& vrMessage);
	virtual void Message(const std::string& vrMessage)=0;
	virtual void MessageWithChoice(DWORD vDBID,const std::string& vrMessage)=0;
	bool GetRefuseState(){return mRefuse;};
	void SetRefuseState(bool vRefuse){mRefuse=vRefuse;}
	
	CTeamData	mData;
protected:
	
	DWORD		mSelfRegionID;
	DWORD		mSelfDBID;
	std::string mSelfName;
	char		mSelfHead;

	bool		mRefuse;
	long		mlTime;
	map<long,time_t> m_LimitTime;
	map<std::string,time_t> m_LimitTimeByName;
	int m_iInviteTimes;
};


#endif//GC_TEAM_H