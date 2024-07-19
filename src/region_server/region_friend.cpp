
#include "region.h"
#include "game_string_assistant.h"
#include "game_mail_shared_const.h"
#include "region_friend.h"
#include "region_mail.h"


#include "region_object.h"
#include "scene/rts_sv_scene.h"
#include "region_creature.h"
#include "region_scene.h"
#include "cmd_channel.h"
#include "region_server.h"



//#define GET_PARAMS() CmdGiver = g_region->m_cmdGiver;
#define SAFE_READ(p) if(!(p)) return CMD_ERROR_READ_DATA;
const int STRING_LENGTH=1024;



inline CServerFriend* CServerFriend::FindByDBID(DWORD vDBID)
{
	CRegionUser* pUser=g_region->FindUser(vDBID);
	if(pUser==NULL)
	{//该物体找不到
		//SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_FIND);
	}
	else
	{//找到
		return pUser->m_dummy->m_pFriend;
	}
	return NULL;
}

inline CRegionCreature* CServerFriend::FindCreByDBID(DWORD vRegionID)
{	
	if (mpMaster->m_scene)
	{
		return mpMaster->m_scene->FindCreature(vRegionID);
	}
	return NULL;
}



//好友名单

inline CServerFriend* CServerFriend::FindByRegionID(DWORD vRegionID)
{
	//这里是用RegionID
	CRegionCreature* pCreature =mpMaster->m_scene->FindCreature(vRegionID);

	if(pCreature==NULL)
	{//该物体找不到
		SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_IN_REGION);
	}
	else
	{//找到该物体
		if(pCreature->m_pMail==NULL)
		{//不是玩家没有好友列表
			SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_PLAYER);
		}
		else
		{
			return pCreature->m_pFriend;
		}
	}

	return NULL;
}


//-------自己---------
//自己邀请别人加入好友
void CServerFriend::InviteOther(DWORD vOtherRegionID,BYTE vOperation)
{
	if(vOtherRegionID==mSelfRegionID)
	{//如果是自己邀请自己，不处理
		return ;
	}
	if(mConnection.Size(SConnection::FRIEND)>=MAX_FRIEND_COUNT)
	{//人数满不邀请
		SendToClient(R2C_FRIEND_MESSAGE,FRIEND_LIST_OVERFLOW);
		return;
	}

	//这里是用RegionID
	if(mConnection.Exist(SConnection::FRIEND,vOtherRegionID))
	{//该好友已经在自己的好友列表中
		SendToClient(R2C_FRIEND_MESSAGE,FRIEND_ALREADY_IN_LIST);
		return;
	}
	//人数不满，调用别人邀请自己的服务端
	SendToServer(vOperation,vOtherRegionID);
	return;
}



//自己驱逐好友
void CServerFriend::BanishOther(DWORD vOtherDBID)
{
	if(vOtherDBID==mSelf.mDBID)
	{//如果是自己邀请自己，不处理
		return ;
	}

	if(mConnection.Exist(SConnection::FRIEND,vOtherDBID))
	{//该好友存在，调用别人驱逐自己的服务器端，同时改变当前数据
		SendToServer(R2R_SELF_BANISH_FRIEND,vOtherDBID);
		//mFriendMap.erase(vOtherDBID);
		mConnection.Erase(SConnection::FRIEND,vOtherDBID);
	}
	else
	{//该好友不存在
		//通知用户该好友不存在
		SendToClient(R2C_FRIEND_MESSAGE,FRIEND_NOT_IN_LIST);
		return;
	}
	return;
}


//自己同意别人的邀请
void CServerFriend::AgreeOtherInvite(DWORD vOtherDBID)
{
	//是否到达人数上限
	if(mConnection.Size(SConnection::FRIEND)<MAX_FRIEND_COUNT)
	{//人数不满//调用别人的服务器端处理函数
		SendToServer(R2R_SELF_AGREE_FRIEND_INVITE,vOtherDBID);
	}
	else
	{//人数满，自动拒绝
		RefuseOtherInvite(vOtherDBID);
		SendToServer(R2R_FRIEND_MESSAGE,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW);
	}
}


//自己拒绝别人的邀请
void CServerFriend::RefuseOtherInvite(DWORD vOtherDBID)
{
	//调用别人的服务器端处理函数
	SendToServer(R2R_SELF_REFUSE_FRIEND_INVITE,vOtherDBID);
}



//-----服务器特有-----
//自己断线
/*
void CServerFriend::OnDisconnect()
{
	OnOffline();
}*/

//自己下线
void CServerFriend::OnOffline()
{
	//改变数据
	//mSelf.mOnline=false;
	//SelfDataChangeAffectOther();


	//清空自己的临时好友列表
	//mConnection.DeleteConnection(SConnection::WHENFRIEND);
}

//自己上线
void CServerFriend::OnOnline()
{
	//改变数据
	mSelf.mOnline=true;

	//将所有的数据重新刷一遍
	SendToClient(R2C_UPDATE_ALL_PLAYER,0);

	map<DWORD,SConnection>::iterator It;
	for (It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
	{
		if(It->second.Is(SConnection::FRIEND))
		{
			SR2rFriendData R2rFriendSendCmd;
			SendToServer(R2R_UPDATE_SELF_TO_FRIEND,(*It).first,0,NULL,0,1);//将自己更新给别人
			SendToServer(R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF,(*It).first);//要求别人更新给自己
		}

		/*修改过，已经不再有仇人的上下线消息了
		if(It->second.Is(SConnection::ENEMY))
		{
			SendToServer(R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF,(*It).first);//要求别人更新给自己
		}
		*/

		/*修改过，已经没有受害者
		if(It->second.Is(SConnection::CASUALTY))
		{
			SendToServer(R2R_UPDATE_SELF_TO_CASUALTY,(*It).first);//自己更新给受害者
		}
		*/
	}
}

//自己对好友列表有影响的变化
void CServerFriend::SelfDataChangeAffectOther()
{
	//发送消息给所有的好友的服务器端
	map<DWORD,SConnection>::iterator It;
	for (It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
	{
		if(It->second.Is(SConnection::FRIEND))
		{
			SendToServer(R2R_UPDATE_SELF_TO_FRIEND,(*It).first);
		}

		/*已经修改过，没有受害者了
		if(It->second.Is(SConnection::CASUALTY))
		{
			SendToServer(R2R_UPDATE_SELF_TO_CASUALTY,It->first);
		}
		*/
	}
	return;
}


//别人将你加入成功
void CServerFriend::OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mConnection.Size(SConnection::FRIEND) <MAX_FRIEND_COUNT)
	{
		//改变自身数据
		SConnection Temp(vOtherDBID,true,vrOtherName,SConnection::FRIEND,0,0,0,0,"",0);
		mConnection.Insert(vOtherDBID,Temp);
		SendToClient(R2C_ADD_PLAYER,vOtherDBID,&vrOtherName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vOtherDBID);
		SendToClient(R2C_FRIEND_MESSAGE_WITH_ID,OTHER_ADD_SELF,NULL,vOtherDBID);
	} 
	else
	{
		SendToServer(R2R_DELETE_FRIEND,vOtherDBID,mSelf.mDBID);
		SendToServer(R2R_FRIEND_MESSAGE_WITH_ID,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW,NULL,mSelf.mDBID);
	}
}

//删除黑名单
void CServerFriend::DeleteBlackList(DWORD vOtherDBID)
{
	mConnection.Erase(SConnection::BLACKLIST,vOtherDBID);
	g_region->m_gws.SendPakeBlackListName(vOtherDBID,mpMaster->m_userInfo->m_userId,0);
}


void CServerFriend::ReceivePakeBlackList(long SenderID,short type)
{
	if (type == 1)
	{
		if(mConnection.Exist(SConnection::FAKEBLACKLIST,SenderID))
		{
			return;
		}
		SConnection Temp(SenderID,true,"",SConnection::FAKEBLACKLIST,0,0,0,0,"",0);
		mConnection.Insert(SenderID,Temp);
	}
	else if (type == 0)
	{
		mConnection.Erase(SConnection::FAKEBLACKLIST,SenderID);
	}
}

void CServerFriend::MsgSetRefusefriend()
{
	rt2_sprintf(g_strStaticBuffer, R(MSG_FRIEND_OTHERSETREFUSE));
	mpMaster->SendSystemMessage(g_strStaticBuffer);
}


//Tianh 添加黑名单
void CServerFriend::AddBlackList(const char* vOtherName)
{
	
	g_region->m_gws.SendBlackListName(vOtherName,mpMaster->m_userInfo->m_userId);

}


void CServerFriend::ReceiveBlackList(long SenderID,const char* Name)
{

	string StrName = Name;

	//如果在好友里面
	if(mConnection.Exist(SConnection::FRIEND,SenderID))
	{
		map<DWORD,SConnection>::iterator It=mConnection.mMap.find(SenderID);
		string Name = It->second.mName;
		if (mConnection.Size(SConnection::BLACKLIST) < MAX_BLACKLIST_COUNT)
		{
			SConnection Temp(SenderID,true,StrName,SConnection::BLACKLIST,0,0,0,0,"",0);
			mConnection.Insert(SenderID,Temp);
		}
		//偷偷的通知对方删除好友
		BanishOther(SenderID);
	}
	else
	{
		if (mConnection.Size(SConnection::BLACKLIST) < MAX_BLACKLIST_COUNT)
		{
			SConnection Temp(SenderID,true,StrName,SConnection::BLACKLIST,0,0,0,0,"",0);
			mConnection.Insert(SenderID,Temp);
		}
	}

	//告诉对方把自己加到假的黑名单中   不显示给对方
	g_region->m_gws.SendPakeBlackListName(SenderID,mpMaster->m_userInfo->m_userId,1);//1代表添加

	//将所有的数据重新刷一遍
	SendToClient(R2C_UPDATE_ALL_PLAYER,0);
}

//Tianh 因为临时好友的加入有可能是远距离组队  所以拿名字还要发往GW
void CServerFriend::WhenFriend(DWORD vOtherDBID)
{
	//如果在自己的好友列表  不添加
	if(mConnection.Exist(SConnection::FRIEND,vOtherDBID))
	{
		return;
	}
	//在黑名单 也不添加
	if(mConnection.Exist(SConnection::BLACKLIST,vOtherDBID))
	{
		return;
	}
	//如果是自己  不添加
	if (mpMaster->m_userInfo)
	{
		if (mpMaster->m_userInfo->m_userId == vOtherDBID)
		{
			return;
		}
	}


	CRegionUser* pUser = g_region->FindUser(vOtherDBID);
	if (pUser)
	{
		string vrOtherName = "";
		short metier = 0;

		if (pUser->m_dummy)
		{
		   vrOtherName= pUser->m_dummy->m_core.Name;
		   metier= pUser->m_dummy->m_core.Metier;
		}	

		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_subsystem_mail);
		g_sendCmd->WriteByte(R2C_ADD_WHEN_FRIEND);
		g_sendCmd->WriteString((char*)vrOtherName.c_str());
		g_sendCmd->WriteLong(vOtherDBID);
		g_sendCmd->WriteShort(metier);
		mpMaster->RecvCmd(g_sendCmd);
	}
	else
	{
		if (mpMaster->m_userInfo)
		{
			g_region->m_gws.SendUserWhenFriendName(vOtherDBID,mpMaster->m_userInfo->m_userId);
		}	
	}

}

//接收GW传来的临时好友的名字
void CServerFriend::ReceiveFromServerwhenfriend(const char* Name,long SendID,short metier)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_mail);
	g_sendCmd->WriteByte(R2C_ADD_WHEN_FRIEND);
	g_sendCmd->WriteString(Name);
	g_sendCmd->WriteLong(SendID);
	g_sendCmd->WriteShort(metier);
	mpMaster->RecvCmd(g_sendCmd);
}



//------别人-------
//别人同意加入好友
void CServerFriend::OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mConnection.Size(SConnection::FRIEND)<MAX_FRIEND_COUNT)
	{//改变数据，告诉对方，告诉自己的客户端
		SConnection Temp(vOtherDBID,true,vrOtherName,SConnection::FRIEND,0,0,0,0,"",0);
		mConnection.Insert(vOtherDBID,Temp);

		SendToServer(R2R_SELF_ADD_OTHER_FRIEND_SUCCEED,vOtherDBID);
		SendToClient(R2C_ADD_PLAYER,vOtherDBID,&vrOtherName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vOtherDBID);
		SendToClient(R2C_FRIEND_MESSAGE_WITH_ID,OTHER_ADD_SELF,NULL,vOtherDBID);
	}
	else
	{//队伍已经满，无法加入
		SendToServer(R2R_FRIEND_MESSAGE,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW);
		SendToClient(R2C_FRIEND_MESSAGE,SELF_FRIEND_LIST_OVERFLOW_OTHER_CANNOT_ADD);
	}
	return;
}

//别人拒绝加入好友
void CServerFriend::OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	//告诉自己被拒绝
	SendToClient(R2C_FRIEND_MESSAGE,OTHER_FRIEND_REFUSE_INVITE);
	return;
}


//别人对好友列表有影响的变化
void CServerFriend::OtherDateChangeAffectSelf(DWORD vOtherDBID)
{
	SendToClient(R2C_UPDATE_ONE_PLAYER,vOtherDBID);
	return;
}


//别人邀请自己加入好友列表
void CServerFriend::OtherInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mConnection.Size(SConnection::FRIEND) <MAX_FRIEND_COUNT)
	{//发送给自己消息
		SendToClient(R2C_OTHER_FRIEND_INVITE,vOtherDBID,&vrOtherName);
	}
	else
	{//超过人数//调用别人的服务器端函数[别人拒绝加入]
		//根据当前的邀请互斥情况，决定是否自动拒绝
		SendToServer(R2R_SELF_REFUSE_FRIEND_INVITE,vOtherDBID);
		SendToServer(R2R_FRIEND_MESSAGE,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW);
	}
}

//别人驱逐自己从好友列表
void CServerFriend::OtherBanish(DWORD vOtherDBID)
{
	//发送消息到自己，同时改变数据	
	mConnection.Erase(SConnection::FRIEND,vOtherDBID);
	SendToClient(R2C_FRIEND_MESSAGE_WITH_ID,OTHER_BANISH_SELF,NULL,vOtherDBID);

	if(mConnection.Exist(vOtherDBID))
	{
		SendToClient(R2C_UPDATE_ONE_PLAYER,vOtherDBID);
	}
	else
	{
		SendToClient(R2C_DELETE_FRIEND,vOtherDBID);
	}
}


//------对客户端--------
//接到上传的消息
int CServerFriend::ReceiveFromClient(BYTE vOperation,CG_CmdPacket *cmd)
{
	long OtherID;
	SAFE_READ(cmd->ReadLong(&OtherID));

	//解析消息，并处理
	switch(vOperation)
	{
	case C2R_INVITE_OTHER_FRIEND_BYNAME:
		{
			char *username;
			SAFE_READ(cmd->ReadString(&username));
			CRegionUser* pRegionUser = g_region->FindUser(username,true);
			if (pRegionUser)
			{
				InviteOther(pRegionUser->m_userId,R2R_SELF_INVITE_FRIEND);
			}
			else
			{
				if (mpMaster->m_userInfo)
				{
					g_region->m_gws.SendUserIdbyName(username,mpMaster->m_userInfo->m_userId);
				}			
			}
		}
		break;
	case C2R_INVITE_OTHER_FRIEND:			//自己邀请别人加入好友
		InviteOther(OtherID,R2R_SELF_INVITE_FRIEND_NREA);
		break;
	case C2R_ADD_BLACKLIST:
		{
			char *username;
			SAFE_READ(cmd->ReadString(&username));
			AddBlackList(username);
		}
		break;
	case C2R_DELETE_BLACKLIST:
		DeleteBlackList(OtherID);
		break;
	case C2R_BANISH_OTHER_FRIEND:			//自己删除别人从好友中
		BanishOther(OtherID);
		break;
	case C2R_AGREE_OTHER_FRIEND_INVITE:		//自己同意别人的邀请
		AgreeOtherInvite(OtherID);
		break;
	case C2R_REFUSE_OTHER_FRIEND_INVITE:	//自己拒绝别人的邀请
		RefuseOtherInvite(OtherID);
		break;
	case C2R_DELETE_OTHER_ENEMY://删除一个仇人
		{
			mConnection.Erase(SConnection::ENEMY,OtherID);
			//SendToServer(R2R_DELETE_CASUALTY,OtherID);//修改过已经没有受害者了
		}
		break;
	default:
		return CMD_ERROR_UNKNOWN;
		break;
	}
	return CMD_ERROR_NONE;
}

void CServerFriend::ReceiveFromServerIDreturn()
{
	//邀请的好友不在线  或者不存在
	if (mpMaster)
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_FRIEND_NOTONLINEORNOTEXIST));
		mpMaster->SendSystemMessage(g_strStaticBuffer);
	}
}

void CServerFriend::ReceiveFromServerID(long UserID)
{
	InviteOther(UserID,R2R_SELF_INVITE_FRIEND);
}

//发送消息到客户端
void CServerFriend::SendToClient(BYTE vOperation,DWORD vParameter,const std::string* vpName,DWORD vDBIDInMessage)
{
	if (vOperation != R2C_UPDATE_ALL_PLAYER)
	{
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(r2c_subsystem_mail);
		g_sendCmd->WriteByte(vOperation);
	}

	switch(vOperation)
	{
	case R2C_ADD_PLAYER:			//增加一个玩家
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_DELETE_FRIEND:		//减少一个好友
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_TEACHER:	//减少一个老师
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_MASTER:		//减少一个师傅
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_STUDENT:	//减少一个学生
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_PRENTICE:	//减少一个徒弟
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_UPDATE_ONE_PLAYER:	//刷新某个好友的数据
		{
			g_sendCmd->WriteLong(vParameter);
			map<DWORD,SConnection>::iterator It=mConnection.mMap.find(vParameter);
			
			if(It==mConnection.mMap.end())
			{//没有找到
				string strone = "";
				g_sendCmd->WriteByte(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteString("");//
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteString((char*)strone.c_str());
				g_sendCmd->WriteLong(0);
			}
			else
			{//找到
				if (mpMaster->m_scene)
				{
					CRegionCreature* pCreature = mpMaster->m_scene->FindUserByName(It->second.mName.c_str());
					if (pCreature)
					{
						RtsSceneBlockMap* pMap = pCreature->m_scene->FindBlockByPos(pCreature->m_pos);
						/*RtsSceneBlockMap* FindBlockByFileName(const char* filename);*/
						string strone = pMap->m_szBlockName;
						g_sendCmd->WriteByte((char)It->second.mOnline);
						g_sendCmd->WriteLong((long)It->second.mConnection);
						g_sendCmd->WriteLong((long)It->second.mKilledCount);
						g_sendCmd->WriteString( (char*) It->second.mName.c_str());//
						g_sendCmd->WriteLong((long)pCreature->m_core.Metier);
						g_sendCmd->WriteLong((long)pCreature->m_core.Lev);
						g_sendCmd->WriteString((char*)strone.c_str());
						g_sendCmd->WriteLong(0); //亲密度
					}
					else
					{
						if (mpMaster->m_userInfo)
						{
							g_region->m_gws.SendUserIdbyNameToGw(vParameter,mpMaster->m_userInfo->m_userId,1);
							return;
						}					
					}
				}
			}
		}
		break;
	case R2C_UPDATE_ALL_PLAYER:		//刷新所有好友的数据
		{
			/*g_sendCmd->WriteLong((long)mConnection.mMap.size());*/
			map<DWORD,SConnection>::iterator It;
			for(It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
			{
				CRegionCreature* pCreature = mpMaster->m_scene->FindUserByName(It->second.mName.c_str());
				if (pCreature)
				{
					RtsSceneBlockMap* pMap = pCreature->m_scene->FindBlockByPos(pCreature->m_pos);
					/*RtsSceneBlockMap* FindBlockByFileName(const char* filename);*/
					string strone = pMap->m_szBlockName;
					g_sendCmd->BeginWrite();
					g_sendCmd->WriteShort(r2c_subsystem_mail);
					g_sendCmd->WriteByte(vOperation);
					g_sendCmd->WriteLong(It->second.mDBID);
					g_sendCmd->WriteByte(It->second.mOnline);
					g_sendCmd->WriteString((char*)It->second.mName.c_str());
					g_sendCmd->WriteLong((long)It->second.mConnection);
					g_sendCmd->WriteLong((long)It->second.mKilledCount);
					g_sendCmd->WriteLong((long)pCreature->m_core.Metier);
					g_sendCmd->WriteLong((long)pCreature->m_core.Lev);
					g_sendCmd->WriteString((char*)strone.c_str());
					g_sendCmd->WriteLong(0);
					mpMaster->RecvCmd(g_sendCmd);
				}
				else if (It->second.mOnline == 0)
				{
					ReceiveNotallFindFriend(It->second.mDBID);
				}
				else
				{
					g_region->m_gws.SendUserIdbyNameToGw(It->second.mDBID,mpMaster->m_userInfo->m_userId,2);
					return;
				}
			}
			return;
			
		}
		break;
	case R2C_FRIEND_MESSAGE:			//刷一个消息
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_FRIEND_MESSAGE_WITH_ID:
		g_sendCmd->WriteLong(vDBIDInMessage);
		g_sendCmd->WriteLong(vParameter);
		break;
		//-------别人-----------
	case R2C_OTHER_FRIEND_INVITE:				//别人邀请自己加入好友
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*)(vpName->c_str()));
		break;
	default:
		break;
	}
	mpMaster->RecvCmd(g_sendCmd);
}

void CServerFriend::ReceiveNotallFindFriend(long UserID)
{
	map<DWORD,SConnection>::iterator It= mConnection.mMap.find(UserID);
	string strone = "未知";
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_mail);
	g_sendCmd->WriteByte(R2C_UPDATE_ALL_PLAYER);
	g_sendCmd->WriteLong(It->second.mDBID);
	g_sendCmd->WriteByte(It->second.mOnline);
	g_sendCmd->WriteString((char*)It->second.mName.c_str());
	g_sendCmd->WriteLong((long)It->second.mConnection);
	g_sendCmd->WriteLong((long)It->second.mKilledCount);
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteString((char*)strone.c_str());
	g_sendCmd->WriteLong(0);

	mpMaster->RecvCmd(g_sendCmd);
}

void CServerFriend::ReceiveNotFindFriend(long UserID)
{
	map<DWORD,SConnection>::iterator It= mConnection.mMap.find(UserID);
	string strone = "未知";
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_mail);
	g_sendCmd->WriteByte(R2C_UPDATE_ONE_PLAYER);
	g_sendCmd->WriteLong(UserID);
	g_sendCmd->WriteByte((char)It->second.mOnline);
	g_sendCmd->WriteLong((long)It->second.mConnection);
	g_sendCmd->WriteLong((long)It->second.mKilledCount);
	g_sendCmd->WriteString( (char*) It->second.mName.c_str());//
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteString((char*)strone.c_str());
	g_sendCmd->WriteLong(0);
	mpMaster->RecvCmd(g_sendCmd);
}

void CServerFriend::ReceiveOneFromServerReturn(long UserID,const char *SceneName, long Lev,char Metier)
{
	map<DWORD,SConnection>::iterator It= mConnection.mMap.find(UserID);
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_mail);
	g_sendCmd->WriteByte(R2C_UPDATE_ONE_PLAYER);
	g_sendCmd->WriteLong(UserID);
	g_sendCmd->WriteByte((char)It->second.mOnline);
	g_sendCmd->WriteLong((long)It->second.mConnection);
	g_sendCmd->WriteLong((long)It->second.mKilledCount);
	g_sendCmd->WriteString( (char*) It->second.mName.c_str());//
	g_sendCmd->WriteLong((long)Metier);
	g_sendCmd->WriteLong(Lev);
	g_sendCmd->WriteString((char*)SceneName);
	g_sendCmd->WriteLong(0); //亲密度
	mpMaster->RecvCmd(g_sendCmd);
}

void CServerFriend::ReceiveAllFromServerReturn(long UserID,const char *SceneName, long Lev,char Metier)
{

	map<DWORD,SConnection>::iterator It= mConnection.mMap.find(UserID);

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_subsystem_mail);
	g_sendCmd->WriteByte(R2C_UPDATE_ALL_PLAYER);
	g_sendCmd->WriteLong(It->second.mDBID);
	g_sendCmd->WriteByte(It->second.mOnline);
	g_sendCmd->WriteString((char*)It->second.mName.c_str());
	g_sendCmd->WriteLong((long)It->second.mConnection);
	g_sendCmd->WriteLong((long)It->second.mKilledCount);
	g_sendCmd->WriteLong((long)Metier);
	g_sendCmd->WriteLong((long)Lev);
	g_sendCmd->WriteString((char*)SceneName);
	g_sendCmd->WriteLong(0);

	mpMaster->RecvCmd(g_sendCmd);

}


/*
//------对服务端--------
struct SR2rFriendData
{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;mParameter=0;mOnline=false;mpName=NULL;}
	BYTE	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	bool	mOnline;
	char*	mpName;
};
*/


//发送消息到服务端
void CServerFriend::SendToServer(BYTE vOperation,DWORD vSendToID,DWORD vParameter,std::string* vpName,DWORD vDBIDInMessage,char IsFristOnline)
{
	SR2rFriendData R2rFriendSendCmd;


	R2rFriendSendCmd.BeginSend();
	R2rFriendSendCmd.mOperation=vOperation;
	R2rFriendSendCmd.mFromDBID=mSelf.mDBID;

	switch(vOperation)
	{
	case R2R_DELETE_FRIEND://减少一个好友
		R2rFriendSendCmd.mParameter=vParameter;
		break;
	case R2R_UPDATE_SELF_TO_FRIEND://刷新自己的数据到好友
		{//
			R2rFriendSendCmd.IsFristOnline=IsFristOnline;
			R2rFriendSendCmd.mOnline=mSelf.mOnline;
			R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
			
		}
		break;
	case R2R_FRIEND_MESSAGE:
		R2rFriendSendCmd.mParameter=vParameter;
		break;
	case R2R_FRIEND_MESSAGE_WITH_ID:
		R2rFriendSendCmd.mDBIDInMessage=vDBIDInMessage;
		R2rFriendSendCmd.mParameter=vParameter;
		break;
	case R2R_SELF_ADD_OTHER_FRIEND_SUCCEED:
		R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
		break;
	case R2R_SELF_AGREE_FRIEND_INVITE:
		R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
		break;
	case R2R_SELF_REFUSE_FRIEND_INVITE:
		R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
		break;
	case R2R_SELF_INVITE_FRIEND:
		{
    		CServerFriend* pFriend= FindByDBID(vSendToID);
			if(pFriend!=NULL)
			{
				R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
				pFriend->ReceiveFromServer(R2rFriendSendCmd);
				return;
			}
			else
			{
				R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
			}
		}
		break;
	case R2R_SELF_INVITE_FRIEND_NREA:
		{
			CRegionCreature* pCre = FindCreByDBID(vSendToID);
			if (pCre)
			{
				if (pCre->m_bRefuseFriend)
				{
					rt2_sprintf(g_strStaticBuffer, R(MSG_FRIEND_OTHERSETREFUSE));
					mpMaster->SendSystemMessage(g_strStaticBuffer);
					return;
				}
			}

			CServerFriend* pFriend= FindByRegionID(vSendToID);
			if(pFriend!=NULL)
			{
				//这里是用RegionID
				if(mConnection.Exist(SConnection::FRIEND,pFriend->mpMaster->m_userInfo->m_userId))
				{//该好友已经在自己的好友列表中
					SendToClient(R2C_FRIEND_MESSAGE,FRIEND_ALREADY_IN_LIST);
					return;
				}
				R2rFriendSendCmd.mOperation = R2R_SELF_INVITE_FRIEND;
				R2rFriendSendCmd.mpFromName=(char*)mSelf.mName.c_str();
				pFriend->ReceiveFromServer(R2rFriendSendCmd);
			}
			return;
		}
		break;
	case R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF:
		break;
	//case R2R_SELF_ADD_OTHER_CASUALTY://已经取消这个设定
	//	break;
	//case R2R_DELETE_CASUALTY://已经取消这个设定
	//	break;
	//case R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF://已经取消这个设定
	//	break;
	//case R2R_UPDATE_SELF_TO_CASUALTY://已经取消这个设定
	//	{
			//修改过，已经没有受害者了
			//R2rFriendSendCmd.mOnline=mSelf.mOnline;//刷新自己的数据到受害者
	//	}
	//	break;
	default:
		break;
	}


	//send to game word
	g_region->m_gws.SendFriend(vSendToID,R2rFriendSendCmd);

	return;
}

void CServerFriend::ReceiveOneRefreshServerID(long SenderID,byte Type)
{
	long UserID = 0; 
	string SceneName = "";
	long   Lev = 0;
	char Metier  = 0;
	if (mpMaster->m_userInfo)
	{
		UserID = mpMaster->m_userInfo->m_userId;
	}
	if (mpMaster->m_scene)
	{
		RtsSceneBlockMap* pMap = mpMaster->m_scene->FindBlockByPos(mpMaster->m_pos);
		if (pMap)
		{
			SceneName = pMap->m_szBlockName;
		}	
	}
	if (mpMaster)
	{	
		Lev = mpMaster->m_core.Lev;
		Metier = mpMaster->m_core.Metier;
	}
	g_region->m_gws.SendOneFriendReturn(SenderID,UserID,SceneName.c_str(),Lev,Type,Metier);
}


//接到服务端的消息
void CServerFriend::ReceiveFromServer(SR2rFriendData& vrR2rFriendSendCmd)
{

	BYTE	Operation;
	DWORD	FromDBID;

	DWORD	OtherDBID;
	bool	Online;
	string  Name;

	std::string TempString;

	Operation=vrR2rFriendSendCmd.mOperation;
	FromDBID=vrR2rFriendSendCmd.mFromDBID;
	switch(Operation)
	{
	case R2R_DELETE_FRIEND://减少一个好友
		OtherDBID=vrR2rFriendSendCmd.mParameter;
		mConnection.Erase(SConnection::FRIEND,OtherDBID);
		break;
	case R2R_UPDATE_OTHER_FRIEND_TO_SELF://别人刷新数据到自己
		{
			Online=vrR2rFriendSendCmd.mOnline;
			Name=vrR2rFriendSendCmd.mpFromName;

			if (vrR2rFriendSendCmd.IsFristOnline == 1)
			{
				rt2_sprintf(g_strStaticBuffer, R(MSG_FRIEND_ONLINE), Name.c_str());
				mpMaster->SendSystemMessage(g_strStaticBuffer);
			}
			if (vrR2rFriendSendCmd.IsLeave == 1)
			{
				rt2_sprintf(g_strStaticBuffer, R(MSG_FRIEND_OFFLINE), Name.c_str());
				mpMaster->SendSystemMessage(g_strStaticBuffer);
			}

			if(mConnection.Exist(SConnection::FRIEND,FromDBID))
			{//是自己的好友
				SConnection Temp;
				Temp.mConnection=SConnection::FRIEND;
				Temp.mOnline=Online;
				Temp.mName=Name;
				mConnection.Insert(FromDBID,Temp);
			}
			else
			{//这个人不是自己的好友//通知对方将自己删除//好友
				SendToServer(R2R_DELETE_FRIEND,FromDBID,mSelf.mDBID);//通知对方将自己删除//好友
			}

			OtherDateChangeAffectSelf(FromDBID);
		}
		break;
	case R2R_FRIEND_MESSAGE:////刷一个消息
		{
			DWORD Parameter=vrR2rFriendSendCmd.mParameter;
			SendToClient(R2C_FRIEND_MESSAGE,Parameter);
		}
		break;
	case R2R_FRIEND_MESSAGE_WITH_ID:
		{
			DWORD DBIDInMessage=vrR2rFriendSendCmd.mDBIDInMessage;
			DWORD Parameter=vrR2rFriendSendCmd.mParameter;
			SendToClient(R2C_FRIEND_MESSAGE_WITH_ID,Parameter,NULL,DBIDInMessage);
		}
		break;
	case R2R_OTHER_FRIEND_ADD_SELF_SUCCEED://别人将你成功加入好友
		{
			char*	pFromName;
			pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherAddSelfSucceed(FromDBID,TempString);
		}
		
		break;
	case R2R_OTHER_FRIEND_AGREE_INVITE://别人同意加入好友
		{
			char*	pFromName;
			char    pMetier;
			pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherAgreeInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_REFUSE_FRIEND_INVITE://别人拒绝加入好友
		{
			char* pFromName;
			pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherRefuseInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_FRIEND_INVITE://别人邀请自己加入好友
		{
			char* pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_FRIEND_BANISH://别人驱逐自己出好友
		OtherBanish(FromDBID);
		break;
	case R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF:
		{
			SendToServer(R2R_UPDATE_SELF_TO_FRIEND,FromDBID);
		}
		break;
	//case R2R_OTHER_ADD_SELF_CASUALTY://别人加入自己的受害者//已经取消这个设定
	//	{
			/*修改过，已经没有受害者了
			SConnection Temp;
			Temp.mDBID=FromDBID;
			Temp.mConnection=SConnection::CASUALTY;
			mConnection.Insert(FromDBID,Temp);
			*/
	//	}
	//	break;
	//case R2R_DELETE_CASUALTY://已经取消这个设定

		//mConnection.Erase(SConnection::CASUALTY,FromDBID);//修改过，已经取消受害者
		//break;
	//case R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF://已经取消这个设定
	//	{
			//修改过，已经取消受害者了
			/*
			if(!mConnection.Find(SConnection::CASUALTY,FromDBID))
			{//不是受害者//将他添加进自己的受害者中
				SConnection Temp;
				Temp.mDBID=FromDBID;
				Temp.mConnection=SConnection::CASUALTY;
				mConnection.Insert(FromDBID,Temp);
			}

			SendToServer(R2R_UPDATE_SELF_TO_CASUALTY,FromDBID);
			*/
		//}
		//break;
	//case R2R_UPDATE_OTHER_ENEMY_TO_SELF://仇人的数据更新到自己//已经取消这个设定
	//	{
			//修改过，已经取消了仇人数据变化对自己的影响
			/*
			Online=vrR2rFriendSendCmd.mOnline;
			if(mConnection.Find(SConnection::ENEMY,FromDBID))
			{//是自己的仇人
				SConnection Temp;
				Temp.mConnection=SConnection::ENEMY;
				Temp.mOnline=Online;
				mConnection.Insert(FromDBID,Temp);
			}
			else
			{//这个人不是自己的仇人//通知他将自己从受害者中去掉
				SendToServer(R2R_DELETE_CASUALTY,FromDBID,mSelf.mDBID);
			}

			OtherDateChangeAffectSelf(FromDBID);
			*/

	//	}
	//	break;
	default:
		break;
	}
}

bool CServerFriend::Exist(unsigned short int vConnection,DWORD vDBID)//某种关系的玩家是否存在
{
	return mConnection.Exist(vConnection,vDBID);
}

void CServerFriend::AddEnmity(DWORD vEnemyDBID,std::string& vrName,int vAddKilled)
{
	int Count=MAX_ENEMY_COUNT;
	if(mConnection.Exist(SConnection::ENEMY,vEnemyDBID))
	{//已经有该玩家
		Count++;
	}

	if(mConnection.Size(SConnection::ENEMY)<Count)
	{//名单未满
		SConnection Temp(vEnemyDBID,true,vrName,SConnection::ENEMY,vAddKilled,0,0,0,"",0);
		mConnection.Insert(vEnemyDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vEnemyDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vEnemyDBID);
	}
}





int SConnection::mVersion=0;
void CServerFriend::Save(std::string& vrConnectionList)
{
	static string Temp;
	Temp="";

	Temp<< LAST_FRIEND_VERSION;//版本
    Temp<< (( unsigned int) mConnection.mMap.size());
	// region_server::Debug <<"Connection size["<< (( int) mConnection.mMap.size()) <<"]\n";


	map<DWORD,SConnection>::iterator It;
    for(It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
    {
		// region_server::Debug << It->second;
        Temp<<It->second;
    }

	// region_server::Debug<<"connection string[" << Temp <<"]\n";
    
	if(Temp.size()<=FRIEND_DB_MAX_LENGTH)
	{//正常情况
		vrConnectionList+=Temp;
	}
	else
	{//超出长度
		LOG("friend data is too larger!\n");
	}

	return;
}



void CServerFriend::Load(std::string& vrConnectionList)
{
	// region_server::Debug<<"friend string[" << vrConnectionList <<"]\n";
	int ConnectionCount=0;
	
	vrConnectionList>>SConnection::mVersion;

	switch(SConnection::mVersion)
	{
	case 100://不支持
	case 101://不支持
	case 102://有错误//不支持
	case 103://调试用了下//不支持了
	case 104://调试用了下//不支持了
	case 105://调试用了下//不支持了
		return;
		break;
	case 106://支持
		break;
	case 107://调试用了下//不支持
		return;
		break;
	case 108:
		break;
	default://不支持
		return;
		break;
	}


	vrConnectionList>>ConnectionCount;
	// region_server::Debug <<"connection size["<< ConnectionCount<<"]\n";

	mConnection.mMap.clear();
	for(int i=0;i!=ConnectionCount;++i)
	{
		SConnection Data;
		vrConnectionList>>Data;//数据读入临时变量
		
		//把一些错误的数据排除
		if(Data.NoConnection())
		{//没有关系
		}
		else
		{//有某种关系
			if(Data.mDBID!=0 && Data.mName!="")
			{//保证数据正确
				// region_server::Debug<<Data;
				mConnection.mMap[Data.mDBID]=Data;//从临时变量中得到数值
			}
		}
	}

	mConnection.CountPrentice();

	CheckTeacherAndStudentInfo();
	return;
}


//新增加师徒关系

void CServerFriend::CheckTeacherAndStudentInfo()
{
	SConnection* pTeacher=NULL;
	std::list<SConnection*> StudentList;

	std::map<DWORD,SConnection>::iterator It;
	for(It=mConnection.mMap.begin();It!=mConnection.mMap.end();++It)
	{
		if(It->second.Is(SConnection::TEACHER))
		{
			pTeacher=&(It->second);
		}

		if(It->second.Is(SConnection::STUDENT))
		{
			StudentList.push_back(&(It->second));
		}
	}


	//送要检查的信息到服务器端
	CG_CmdPacket& cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_master_prentice_check);		//要求检查关系的宏
	cmd.WriteLong(mSelf.mDBID);
	if(pTeacher==NULL)
	{
		cmd.WriteLong(0);							//老师ID
		cmd.WriteLong(0);							//老师索引ID
	}
	else
	{
		cmd.WriteLong(pTeacher->mDBID);				//老师ID
		cmd.WriteLong(pTeacher->mConnectionIndex);	//老师索引ID
	}

	cmd.WriteShort((unsigned short int )StudentList.size());		//学生总数
	std::list<SConnection*>::iterator It2;
	for(It2=StudentList.begin();It2!=StudentList.end();++It2)
	{
		cmd.WriteLong((*It2)->mDBID);//学生ID
		cmd.WriteLong((*It2)->mConnectionIndex);//学生索引ID
	}

	g_region->m_gws.EndSend();
}
bool CServerFriend::MendTeacherAndStudentInfo(CG_CmdPacket *pPacket)//矫正老师学生师傅徒弟关系
{
	std::vector<DWORD> arrAddTeacherIndexes;
	std::vector<DWORD> arrAddTeacherIDs;
	std::vector<std::string> arrAddTeacherNames;
	std::vector<DWORD> arrRemoveTeacherIDs;
	std::vector<DWORD> arrAddStudentIndexes;
	std::vector<DWORD> arrAddStudentIDs;
	std::vector<std::string> arrAddStudentNames;
	std::vector<DWORD> arrRemoveStudentIDs;

	// 下载更新的数据
	int i;
	short count;
	long prenIndex;
	long id;
	char* name;
	SAFE_READ(pPacket->ReadShort(&count)); // 添加Teacher
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&prenIndex));
		SAFE_READ(pPacket->ReadLong(&id));
		SAFE_READ(pPacket->ReadString(&name));
		arrAddTeacherIndexes.push_back(prenIndex);
		arrAddTeacherIDs.push_back(id);
		arrAddTeacherNames.push_back(name);
	}
	SAFE_READ(pPacket->ReadShort(&count)); // 删除Teacher
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&id));
		arrRemoveTeacherIDs.push_back(id);
	}
	SAFE_READ(pPacket->ReadShort(&count)); // 添加Student
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&prenIndex));
		SAFE_READ(pPacket->ReadLong(&id));
		SAFE_READ(pPacket->ReadString(&name));
		arrAddStudentIndexes.push_back(prenIndex);
		arrAddStudentIDs.push_back(id);
		arrAddStudentNames.push_back(name);
	}
	SAFE_READ(pPacket->ReadShort(&count)); // 删除Student
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&id));
		arrRemoveStudentIDs.push_back(id);
	}



	// 更新region的师徒数据
	
	//先删除老师
	int Index=0;
	for(Index=0;Index!=arrRemoveTeacherIDs.size();++Index)
	{
		Remove(SConnection::TEACHER,arrRemoveTeacherIDs[Index]);
	}

	//再增加老师
	for(Index=0;Index!=arrAddTeacherIDs.size();++Index)
	{
		AddTeacher(arrAddTeacherIndexes[Index],arrAddTeacherIDs[Index],arrAddTeacherNames[Index]);
	}

	//先删除学生
	for(Index=0;Index!=arrRemoveStudentIDs.size();++Index)
	{
		Remove(SConnection::STUDENT,arrRemoveStudentIDs[Index]);
	}
	//再增加学生
	for(Index=0;Index!=arrAddStudentIDs.size();++Index)
	{
		AddStudent(arrAddStudentIndexes[Index],arrAddStudentIDs[Index],arrAddStudentNames[Index]);
	}
	return true;
}



void CServerFriend::AddTeacher(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName)	//增加老师[未出师]
{
	if(mConnection.Size(SConnection::TEACHER)<MAX_TEACHER_COUNT)
	{//名单未满
		SConnection Temp(vDBID,true,vrName,SConnection::TEACHER,0,vMasterApperenticeIndexID,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}

void CServerFriend::AddStudent(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName)	//增加学生[未出师]
{
	if(mConnection.Size(SConnection::STUDENT)<MAX_STUDENT_COUNT)
	{//名单未满
		SConnection Temp(vDBID,true,vrName,SConnection::STUDENT,0,vMasterApperenticeIndexID,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}

void CServerFriend::AddMaster(DWORD vDBID,const std::string& vrName)									//增加师傅[已出师]
{
	if(mConnection.Size(SConnection::MASTER)<MAX_MASTER_COUNT)
	{
		SConnection Temp(vDBID,true,vrName,SConnection::MASTER,0,0,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}

void CServerFriend::AddPrentice(DWORD vDBID,const std::string& vrName)								//增加徒弟[已出师]
{
	std::vector<SConnection*> PrenticeVector;
	mConnection._Find(SConnection::PRENTICE,PrenticeVector);

	if(PrenticeVector.size()>=MAX_PRENTICE_COUNT)
	{//去掉大弟子

		DWORD OldPrenticeDBID=0;
			for(int Index=0;Index!=PrenticeVector.size();++Index)
			{
				if(PrenticeVector[Index]->mJoinIndex==0)
				{
					OldPrenticeDBID=PrenticeVector[Index]->mDBID;
					break;
				}
			}

			Remove(SConnection::PRENTICE,OldPrenticeDBID);//要用这个//会更新到客户端
	}


	if(mConnection.Size(SConnection::PRENTICE)<MAX_PRENTICE_COUNT)
	{
		SConnection Temp(vDBID,true,vrName,SConnection::PRENTICE,0,0,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}




