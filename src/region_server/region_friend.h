


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

	//-------自己---------
	//自己邀请别人加入好友
	void InviteOther(DWORD vOtherRegionID,BYTE vOperation);
	//自己驱逐好友
	void BanishOther(DWORD vOtherDBID);
	void BanishOtherByRegionID(DWORD vOtherRegionID);

	//自己同意别人的邀请
	void AgreeOtherInvite(DWORD vOtherDBID);
	//自己拒绝别人的邀请
	void RefuseOtherInvite(DWORD vOtherDBID);

	//-----服务器特有-----
	//下线由game_world处理

	//自己断线
	//void OnDisconnect();
	//自己下线
	void OnOffline();
	//自己上线
	void OnOnline();
	//自己对好友列表有影响的变化
	void SelfDataChangeAffectOther();

	//别人将你加入成功
	void OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName);
	//别人将你加入失败
	void OtherAddSelfFault(DWORD vOtherDBID);
	//别人不存在
	void OtherNotExist(DWORD vOtherDBID);

	//Tianh 增加临时好友
	void WhenFriend(DWORD vOtherDBID);
	void AddBlackList(const char* vOtherName);

	void DeleteBlackList(DWORD vOtherDBID);
	//------别人-------
	//别人同意加入好友
	void OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName);
	//别人拒绝加入好友
	void OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName);
	//别人对好友列表有影响的变化
	void OtherDateChangeAffectSelf(DWORD vOtherDBID);

	//别人邀请自己加入好友列表
	void OtherInvite(DWORD vOtherDBID,std::string& vrOtherName);
	//别人驱逐自己从好友列表
	void OtherBanish(DWORD vOtherDBID);

	//------对客户端--------
	//接到上传的消息
	int ReceiveFromClient(BYTE vOperation,CG_CmdPacket *cmd);
	//发送消息到客户端
	void SendToClient(BYTE vOperation,DWORD vParameter,const std::string* vpName=NULL,DWORD vDBIDInMessage=0);

	//------对服务端--------
	void ReceiveOneFromServerReturn(long UserID,const char *SceneName, long Lev,char Metier);
	void ReceiveAllFromServerReturn(long UserID,const char *SceneName, long Lev,char Metier);
	//接到服务端的消息
	void ReceiveFromServer(SR2rFriendData& vrR2rFriendSendCmd);
	//发送消息到服务端
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
	


	//新增加师徒关系
	void CheckTeacherAndStudentInfo();
	bool MendTeacherAndStudentInfo(CG_CmdPacket *pPacket);
	
	void AddTeacher(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName);	//增加老师[未出师]
	void AddStudent(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName);	//增加学生[未出师]
	void AddMaster(DWORD vDBID,const std::string& vrName);									//增加师傅[已出师]
	void AddPrentice(DWORD vDBID,const std::string& vrName);								//增加徒弟[已出师]


	//移除都统一用mConnection.Erase()
	void Remove(unsigned short int vConnection,DWORD vDBID)//将某人的某种关系清除
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
	
	SConnection* Find(unsigned short int vConnection,DWORD vDBID)//没有找到返回NULL
	{
		return mConnection._Find(vConnection,vDBID);
	}

	SConnection* Find(unsigned short int vConnection,const string& vrName)//没有找到返回NULL
	{
		return mConnection._Find(vConnection,vrName);
	}

	int Find(unsigned short int vConnection,std::vector<SConnection*>& vrVector)//总共有多少个人
	{
		return mConnection._Find(vConnection,vrVector);
	}


	
	SConnection						mSelf;
	//关系名单
	CConnectionMap					mConnection;
	
private:
	CServerFriend* FindByRegionID(DWORD vRegionID);
	CServerFriend* FindByDBID(DWORD vDBID);
	CRegionCreature* FindCreByDBID(DWORD vRegionID);
	


	CRegionCreature*				mpMaster;
	DWORD							mSelfRegionID;

	int						        FirendCount;//刷新所有好友的数量
};


#endif//REGION_FRIEND_H
