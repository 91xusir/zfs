
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

	//自己邀请别人加入好友
	void InviteOther(DWORD vOtherRegionID);
	void InviteOther(std::string& vrOtherName);
	//删除好友
	void BanishOther(DWORD vOtherDBID);
	//同意别人的邀请
	void AgreeOtherInvite(DWORD vOtherDBID);
	//拒绝别人的邀请
	void RefuseOtherInvite(DWORD vOtherDBID);

	//别人邀请你
	void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);
    //添加黑名单
	void AddOtherToBlackList(std::string &vrOtherName);
	//黑名单删除
	void DeleteOtherFromBlackList(DWORD vOtherDBID);
	//接到消息
	void ReceiveFromServer(byte vOperation,CG_CmdPacket *pPacket);

	bool Exist(long vConnection,DWORD vDBID);
	DWORD GetDBID(long vConnection,std::string& vrName);


	//删除仇人
	//void DeleteEnemy(DWORD vOtherDBID);
	/*void DeleteTemporaryFriend(DWORD vOtherDBID);*/
	bool GetRefuseState(){return mRefuse;};
	void SetRefuseState(bool vRefuse){mRefuse=vRefuse;}


	//师徒弟关系
	bool Entrance()	//拜师
	{
		/*
		//是否已经有了两个学生
		if(mConnection.Size(SConnection::STUDENT)>=MAX_STUDENT_COUNT)
		{
			return false;
		}
		*/
		return true;
	}

	bool BanishPrentice()	//逐出师门
	{
		return true;
	}

	bool BanishMaster()	//叛逃师门
	{
		return true;
	}

	bool Graduate()//出师
	{
		return true;
	}

	SConnection					mSelf;
protected:

	//------对服务器--------
	//上传消息
	void SendToServer(byte vOperation,DWORD vOtherID);

	//-----客户端特有的-----
	//好友数值变化
	virtual void DataChange()=0;
	virtual void MemberDataChange(DWORD vMemberDBID)=0;
	virtual void InsertWhenFriend(string vrName,long vrDBID,short vrMetier)=0;

	//-------对用户---------
	void Message(DWORD vDBIDInMessage,const std::string& vrMessage);
	void MessageWithChoice(DWORD vFromDBID,DWORD vDBIDInMessage,const std::string& vrMessage);
	virtual void Message(const std::string& vrMessage)=0;
	virtual void MessageWithChoice(DWORD vFromDBID,const std::string& vrMessage)=0;


	//好友名单
	CConnectionMap				mConnection;

	
	DWORD						mSelfRegionID;
	bool						mRefuse;
};

#endif//GC_FRIEND_H