


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


	//------�Լ�----------
	//�Լ�ͨ������������˼������
	void InviteOtherbyName(const char* name);
	//�Լ�������˼������
	void FindInviteOther(DWORD vOtherRegionID);
	void InviteOther(DWORD OtherDBID);
	//�Լ�������˴Ӷ�����
	void BanishOther(DWORD vOtherDBID);
	void BanishOtherByRegionID(DWORD vOtherRegionID);
	//�Լ��뿪����
	void SelfLeave();
	//�Լ���ɢ����
	void Dismiss();

	//�Լ�ͬ����˵�����
	void AgreeOtherInvite(DWORD vOtherDBID);
	//�Լ��ܾ����˵�����
	void RefuseOtherInvite(DWORD vOtherDBID);

	//------������������------
	//�Լ�����
	//void OnDisconnect();
	//�Լ�����
	//void OnOffline();
	//�Լ��ĶԶ�����Ӱ�����ֵ�仯
	void DateChangeAffectTeam();
	
	//------�Կͻ���--------
	//�ӵ��ϴ�����Ϣ
	int ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd);
	//������Ϣ���ͻ���
	//void SendToClient(byte vOperation,DWORD vParameter,std::string* vpName=NULL,DWORD vDBIDInMessage=0);

	//------�Է����--------
	//�ӵ�����˵���Ϣ
	int ReceiveFromGameWorld(CG_CmdPacket *cmd);
	//void SendToGameWorld();

	//void ReceiveFromServer(SR2rData&  vrR2rSendCmd);
	//������Ϣ�������
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
	{mSendPacket.BeginWrite();mSendPacket.WriteShort(r2g_team);mSendPacket.WriteLong(mSelfDBID);return &mSendPacket;};	//�õ���
	
	CG_CmdPacket* ClientPacket()
	{mSendPacket.BeginWrite();mSendPacket.WriteShort(r2c_subsystem_team);return &mSendPacket;};
	
	void SendToGameWorld();			//��һ������GameWorld
	void SendToClient(){mpMaster->RecvCmd(&mSendPacket);};						//��һ������Client		

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
	//bool				mServerWaitOtherAddSelf;//�ȴ������˷���˼����Լ�
};

#endif//REGION_TEAM_H



//���˽���ɹ��������
//void OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName);
//���˽������ʧ��
//void OtherAddSelfFault(DWORD vOtherDBID);
//���˲�����
//void OtherNotExist(DWORD vOtherDBID);

//-------����-----------
//����ͬ��������
//void OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName);
//���˾ܾ��������
//void OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName);
//���˵ĶԶ�����Ӱ�����ֵ�仯
//void OtherDateChangeAffectTeam(DWORD vOtherDBID);


//���������Լ��������
//void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);

//���������Լ�������
//void OtherBanish(DWORD vOtherDBID);
//�����뿪����
//void OtherLeave(DWORD vOtherDBID);
//���˽�ɢ����
//void OtherDismiss(DWORD vOtherDBID);

