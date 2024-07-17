


#ifndef REGION_FRIEND_H
#define REGION_FRIEND_H
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "game_mail_shared_const.h"

class CRegionCreature;
class CG_CmdPacket;
class CServerMail;
struct SR2rFriendData;



class CServerFriend
{
public:
	CServerFriend(CRegionCreature* vpMaster,DWORD vSelfRegionID,DWORD vSelfDBID,std::string& vrSelfName)
		:mSelf(vSelfDBID,true,vrSelfName,0,0,0,0,0,"",0),mpMaster(vpMaster),mSelfRegionID(vSelfRegionID),FirendCount(0)
	{
		
	}

	~CServerFriend(){};

	//-------�Լ�---------
	//�Լ�������˼������
	void InviteOther(DWORD vOtherRegionID,BYTE vOperation);
	//�Լ��������
	void BanishOther(DWORD vOtherDBID);
	void BanishOtherByRegionID(DWORD vOtherRegionID);

	//�Լ�ͬ����˵�����
	void AgreeOtherInvite(DWORD vOtherDBID);
	//�Լ��ܾ����˵�����
	void RefuseOtherInvite(DWORD vOtherDBID);

	//-----����������-----
	//������game_world����

	//�Լ�����
	//void OnDisconnect();
	//�Լ�����
	void OnOffline();
	//�Լ�����
	void OnOnline();
	//�Լ��Ժ����б���Ӱ��ı仯
	void SelfDataChangeAffectOther();

	//���˽������ɹ�
	void OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName);
	//���˽������ʧ��
	void OtherAddSelfFault(DWORD vOtherDBID);
	//���˲�����
	void OtherNotExist(DWORD vOtherDBID);

	//Tianh ������ʱ����
	void WhenFriend(DWORD vOtherDBID);
	void AddBlackList(const char* vOtherName);

	void DeleteBlackList(DWORD vOtherDBID);
	//------����-------
	//����ͬ��������
	void OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName);
	//���˾ܾ��������
	void OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName);
	//���˶Ժ����б���Ӱ��ı仯
	void OtherDateChangeAffectSelf(DWORD vOtherDBID);

	//���������Լ���������б�
	void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);
	//���������Լ��Ӻ����б�
	void OtherBanish(DWORD vOtherDBID);

	//------�Կͻ���--------
	//�ӵ��ϴ�����Ϣ
	int ReceiveFromClient(BYTE vOperation,CG_CmdPacket *cmd);
	//������Ϣ���ͻ���
	void SendToClient(BYTE vOperation,DWORD vParameter,const std::string* vpName=NULL,DWORD vDBIDInMessage=0);

	//------�Է����--------
	void ReceiveOneFromServerReturn(long UserID,const char *SceneName, long Lev,char Metier);
	void ReceiveAllFromServerReturn(long UserID,const char *SceneName, long Lev,char Metier);
	//�ӵ�����˵���Ϣ
	void ReceiveFromServer(SR2rFriendData& vrR2rFriendSendCmd);
	//������Ϣ�������
	void SendToServer(BYTE vOperation,DWORD vSendToID,DWORD vParameter=0,std::string* vpName=NULL,DWORD vDBIDInMessage=0,char IsFristOnline=0);

	void Save(std::string& vrFriendList);
	void Load(std::string& vrFriendList);

	void ReceiveFromServerID(long UserID);

	void ReceiveFromServerIDreturn();

	void ReceiveFromServerwhenfriend(const char* Name,long SendID,short metier);
	
	void AddEnmity(DWORD vDBID,std::string& vrName,int vAddKilled);

	void ReceiveOneRefreshServerID(long SenderID,byte Type);

	void ReceiveNotFindFriend(long UserID);

	void ReceiveNotallFindFriend(long UserID);

	void ReceiveBlackList(long SenderID,const char* Name);

	void ReceivePakeBlackList(long SenderID,short type);

	void MsgSetRefusefriend();
	


	//������ʦͽ��ϵ
	void CheckTeacherAndStudentInfo();
	bool MendTeacherAndStudentInfo(CG_CmdPacket *pPacket);
	
	void AddTeacher(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName);	//������ʦ[δ��ʦ]
	void AddStudent(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName);	//����ѧ��[δ��ʦ]
	void AddMaster(DWORD vDBID,const std::string& vrName);									//����ʦ��[�ѳ�ʦ]
	void AddPrentice(DWORD vDBID,const std::string& vrName);								//����ͽ��[�ѳ�ʦ]


	//�Ƴ���ͳһ��mConnection.Erase()
	void Remove(unsigned short int vConnection,DWORD vDBID)//��ĳ�˵�ĳ�ֹ�ϵ���
	{
		mConnection.Erase(vConnection,vDBID);

		if(mConnection.Exist(vDBID))
		{
			SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
		}
		else
		{
			switch(vConnection)
			{
			case SConnection::FRIEND:
				SendToClient(R2C_DELETE_FRIEND,vDBID);
				break;
			case SConnection::TEACHER:
				SendToClient(R2C_DELETE_TEACHER,vDBID);
				break;
			case SConnection::STUDENT:
				SendToClient(R2C_DELETE_STUDENT,vDBID);
				break;
			case SConnection::MASTER:
				SendToClient(R2C_DELETE_MASTER,vDBID);
				break;
			case SConnection::PRENTICE:
				SendToClient(R2C_DELETE_PRENTICE,vDBID);
				break;
			default:
				break;
			}
		}
	}


	bool Exist(unsigned short int vConnection,DWORD vDBID);
	int Size(unsigned short int vConnection)
	{
		return mConnection.Size(vConnection);
	}
	
	SConnection* Find(unsigned short int vConnection,DWORD vDBID)//û���ҵ�����NULL
	{
		return mConnection._Find(vConnection,vDBID);
	}

	SConnection* Find(unsigned short int vConnection,const string& vrName)//û���ҵ�����NULL
	{
		return mConnection._Find(vConnection,vrName);
	}

	int Find(unsigned short int vConnection,std::vector<SConnection*>& vrVector)//�ܹ��ж��ٸ���
	{
		return mConnection._Find(vConnection,vrVector);
	}


	
	SConnection						mSelf;
	//��ϵ����
	CConnectionMap					mConnection;
	
private:
	CServerFriend* FindByRegionID(DWORD vRegionID);
	CServerFriend* FindByDBID(DWORD vDBID);
	CRegionCreature* FindCreByDBID(DWORD vRegionID);
	


	CRegionCreature*				mpMaster;
	DWORD							mSelfRegionID;

	int						        FirendCount;//ˢ�����к��ѵ�����
};


#endif//REGION_FRIEND_H
