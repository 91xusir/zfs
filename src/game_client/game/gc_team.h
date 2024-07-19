
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

	//------�Լ�----------
	//�Լ�������˼������
	 void InviteOther(DWORD vOtherRegionID);
	 void InviteOther(std::string& vrOtherName);
	//�Լ�������˴Ӷ�����
	void BanishOtherByDBID(DWORD vOtherDBID);
	void BanishOtherByRegionID(DWORD vOtherRegionID);

	//�Լ��뿪����
	void SelfLeave();
	//�Լ���ɢ����
	void Dismiss();

	//�Լ�ͬ����˵�����
	void AgreeOtherInvite(DWORD vOtherDBID);
	//�Լ��ܾ����˵�����
	void RefuseOtherInvite(DWORD vOtherDBID);

	//�Լ����������˵Ķ���
	void ApplyOtherTeam(DWORD vOtherRegionID);
	void ApplyOtherTeam(std::string& vrOtherName);
	//�Լ�ͬ����˵�����
	void AgreeOtherApply(DWORD vOtherRegionID);
	//�Լ��ܾ����˵�����
	void RefuseOtherApply(DWORD vOtherRegionID);


	//-------����-----------

	//����ͬ��������
	void OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName,char vrOtherhead,byte Metier,long BornTag,long actorID);
	//���˾ܾ��������
	void OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName);

	//���������Լ��������
	void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);

	//���������Լ�������
	void OtherBanish(DWORD vOtherDBID);
	//�����뿪����
	void OtherLeave(DWORD vOtherDBID);
	//���˽�ɢ����
	void OtherDismiss(DWORD vOtherDBID);

	//������������Լ��Ķ���
	void OtherApplyTeam(DWORD vOtherDBID,std::string& vrOtherName,DWORD vLevel,short metier/*,DWORD vUnionDBID,std::string& vrUnionName*/,int vrAni,int vrPow);
	//����ͬ���Լ�������
	void OtherAgreeApply(DWORD vOtherDBID);
	//���˾ܾ��Լ�������
	void OtherRefuseApply(DWORD vOtherDBID);
	//ת�öӳ�
	virtual void TransferCaptain(DWORD vOtherDBID);

	//-----�ͻ������е�-----
	//������ֵ�仯
	virtual void TeamDataChange()=0;
	virtual void TeamMemberDataChange(DWORD vMemberDBID)=0;


	//------�Է�����--------
	//�ϴ���Ϣ
	void SendToServer(byte vOperation,DWORD vOtherID);
	//�ӵ���Ϣ
	void ReceiveFromServer(CG_CmdPacket *pPacket);

	//-------���û�---------
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