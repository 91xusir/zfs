
#ifndef GC_FRIEND_H 
#define GC_FRIEND_H

#include <string>
#include <map>
#include "game_mail_shared_const.h"

typedef unsigned long       DWORD;
typedef unsigned char BYTE;
class CG_CmdPacket;

class CClientFriend
{
public:
	CClientFriend();
	virtual~CClientFriend();

	void SetID(DWORD vRegionID,DWORD vDBID);

	//�Լ�������˼������
	void InviteOther(DWORD vOtherRegionID);
	void InviteOther(std::string& vrOtherName);
	//ɾ������
	void BanishOther(DWORD vOtherDBID);
	//ͬ����˵�����
	void AgreeOtherInvite(DWORD vOtherDBID);
	//�ܾ����˵�����
	void RefuseOtherInvite(DWORD vOtherDBID);

	//����������
	void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);
    //��Ӻ�����
	void AddOtherToBlackList(std::string &vrOtherName);
	//������ɾ��
	void DeleteOtherFromBlackList(DWORD vOtherDBID);
	//�ӵ���Ϣ
	void ReceiveFromServer(byte vOperation,CG_CmdPacket *pPacket);

	bool Exist(long vConnection,DWORD vDBID);
	DWORD GetDBID(long vConnection,std::string& vrName);


	//ɾ������
	//void DeleteEnemy(DWORD vOtherDBID);
	/*void DeleteTemporaryFriend(DWORD vOtherDBID);*/
	bool GetRefuseState(){return mRefuse;};
	void SetRefuseState(bool vRefuse){mRefuse=vRefuse;}


	//ʦͽ�ܹ�ϵ
	bool Entrance()	//��ʦ
	{
		/*
		//�Ƿ��Ѿ���������ѧ��
		if(mConnection.Size(SConnection::STUDENT)>=MAX_STUDENT_COUNT)
		{
			return false;
		}
		*/
		return true;
	}

	bool BanishPrentice()	//���ʦ��
	{
		return true;
	}

	bool BanishMaster()	//����ʦ��
	{
		return true;
	}

	bool Graduate()//��ʦ
	{
		return true;
	}

	SConnection					mSelf;
protected:

	//------�Է�����--------
	//�ϴ���Ϣ
	void SendToServer(byte vOperation,DWORD vOtherID);

	//-----�ͻ������е�-----
	//������ֵ�仯
	virtual void DataChange()=0;
	virtual void MemberDataChange(DWORD vMemberDBID)=0;
	virtual void InsertWhenFriend(string vrName,long vrDBID,short vrMetier)=0;

	//-------���û�---------
	void Message(DWORD vDBIDInMessage,const std::string& vrMessage);
	void MessageWithChoice(DWORD vFromDBID,DWORD vDBIDInMessage,const std::string& vrMessage);
	virtual void Message(const std::string& vrMessage)=0;
	virtual void MessageWithChoice(DWORD vFromDBID,const std::string& vrMessage)=0;


	//��������
	CConnectionMap				mConnection;

	
	DWORD						mSelfRegionID;
	bool						mRefuse;
};

#endif//GC_FRIEND_H