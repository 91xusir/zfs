
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
	{//�������Ҳ���
		//SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_FIND);
	}
	else
	{//�ҵ�
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



//��������

inline CServerFriend* CServerFriend::FindByRegionID(DWORD vRegionID)
{
	//��������RegionID
	CRegionCreature* pCreature =mpMaster->m_scene->FindCreature(vRegionID);

	if(pCreature==NULL)
	{//�������Ҳ���
		SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_IN_REGION);
	}
	else
	{//�ҵ�������
		if(pCreature->m_pMail==NULL)
		{//�������û�к����б�
			SendToClient(R2C_FRIEND_MESSAGE,OBJECT_NOT_PLAYER);
		}
		else
		{
			return pCreature->m_pFriend;
		}
	}

	return NULL;
}


//-------�Լ�---------
//�Լ�������˼������
void CServerFriend::InviteOther(DWORD vOtherRegionID,BYTE vOperation)
{
	if(vOtherRegionID==mSelfRegionID)
	{//������Լ������Լ���������
		return ;
	}
	if(mConnection.Size(SConnection::FRIEND)>=MAX_FRIEND_COUNT)
	{//������������
		SendToClient(R2C_FRIEND_MESSAGE,FRIEND_LIST_OVERFLOW);
		return;
	}

	//��������RegionID
	if(mConnection.Exist(SConnection::FRIEND,vOtherRegionID))
	{//�ú����Ѿ����Լ��ĺ����б���
		SendToClient(R2C_FRIEND_MESSAGE,FRIEND_ALREADY_IN_LIST);
		return;
	}
	//�������������ñ��������Լ��ķ����
	SendToServer(vOperation,vOtherRegionID);
	return;
}



//�Լ��������
void CServerFriend::BanishOther(DWORD vOtherDBID)
{
	if(vOtherDBID==mSelf.mDBID)
	{//������Լ������Լ���������
		return ;
	}

	if(mConnection.Exist(SConnection::FRIEND,vOtherDBID))
	{//�ú��Ѵ��ڣ����ñ��������Լ��ķ������ˣ�ͬʱ�ı䵱ǰ����
		SendToServer(R2R_SELF_BANISH_FRIEND,vOtherDBID);
		//mFriendMap.erase(vOtherDBID);
		mConnection.Erase(SConnection::FRIEND,vOtherDBID);
	}
	else
	{//�ú��Ѳ�����
		//֪ͨ�û��ú��Ѳ�����
		SendToClient(R2C_FRIEND_MESSAGE,FRIEND_NOT_IN_LIST);
		return;
	}
	return;
}


//�Լ�ͬ����˵�����
void CServerFriend::AgreeOtherInvite(DWORD vOtherDBID)
{
	//�Ƿ񵽴���������
	if(mConnection.Size(SConnection::FRIEND)<MAX_FRIEND_COUNT)
	{//��������//���ñ��˵ķ������˴�����
		SendToServer(R2R_SELF_AGREE_FRIEND_INVITE,vOtherDBID);
	}
	else
	{//���������Զ��ܾ�
		RefuseOtherInvite(vOtherDBID);
		SendToServer(R2R_FRIEND_MESSAGE,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW);
	}
}


//�Լ��ܾ����˵�����
void CServerFriend::RefuseOtherInvite(DWORD vOtherDBID)
{
	//���ñ��˵ķ������˴�����
	SendToServer(R2R_SELF_REFUSE_FRIEND_INVITE,vOtherDBID);
}



//-----����������-----
//�Լ�����
/*
void CServerFriend::OnDisconnect()
{
	OnOffline();
}*/

//�Լ�����
void CServerFriend::OnOffline()
{
	//�ı�����
	//mSelf.mOnline=false;
	//SelfDataChangeAffectOther();


	//����Լ�����ʱ�����б�
	//mConnection.DeleteConnection(SConnection::WHENFRIEND);
}

//�Լ�����
void CServerFriend::OnOnline()
{
	//�ı�����
	mSelf.mOnline=true;

	//�����е���������ˢһ��
	SendToClient(R2C_UPDATE_ALL_PLAYER,0);

	map<DWORD,SConnection>::iterator It;
	for (It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
	{
		if(It->second.Is(SConnection::FRIEND))
		{
			SR2rFriendData R2rFriendSendCmd;
			SendToServer(R2R_UPDATE_SELF_TO_FRIEND,(*It).first,0,NULL,0,1);//���Լ����¸�����
			SendToServer(R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF,(*It).first);//Ҫ����˸��¸��Լ�
		}

		/*�޸Ĺ����Ѿ������г��˵���������Ϣ��
		if(It->second.Is(SConnection::ENEMY))
		{
			SendToServer(R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF,(*It).first);//Ҫ����˸��¸��Լ�
		}
		*/

		/*�޸Ĺ����Ѿ�û���ܺ���
		if(It->second.Is(SConnection::CASUALTY))
		{
			SendToServer(R2R_UPDATE_SELF_TO_CASUALTY,(*It).first);//�Լ����¸��ܺ���
		}
		*/
	}
}

//�Լ��Ժ����б���Ӱ��ı仯
void CServerFriend::SelfDataChangeAffectOther()
{
	//������Ϣ�����еĺ��ѵķ�������
	map<DWORD,SConnection>::iterator It;
	for (It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
	{
		if(It->second.Is(SConnection::FRIEND))
		{
			SendToServer(R2R_UPDATE_SELF_TO_FRIEND,(*It).first);
		}

		/*�Ѿ��޸Ĺ���û���ܺ�����
		if(It->second.Is(SConnection::CASUALTY))
		{
			SendToServer(R2R_UPDATE_SELF_TO_CASUALTY,It->first);
		}
		*/
	}
	return;
}


//���˽������ɹ�
void CServerFriend::OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mConnection.Size(SConnection::FRIEND) <MAX_FRIEND_COUNT)
	{
		//�ı���������
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

//ɾ��������
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


//Tianh ��Ӻ�����
void CServerFriend::AddBlackList(const char* vOtherName)
{
	
	g_region->m_gws.SendBlackListName(vOtherName,mpMaster->m_userInfo->m_userId);

}


void CServerFriend::ReceiveBlackList(long SenderID,const char* Name)
{

	string StrName = Name;

	//����ں�������
	if(mConnection.Exist(SConnection::FRIEND,SenderID))
	{
		map<DWORD,SConnection>::iterator It=mConnection.mMap.find(SenderID);
		string Name = It->second.mName;
		if (mConnection.Size(SConnection::BLACKLIST) < MAX_BLACKLIST_COUNT)
		{
			SConnection Temp(SenderID,true,StrName,SConnection::BLACKLIST,0,0,0,0,"",0);
			mConnection.Insert(SenderID,Temp);
		}
		//͵͵��֪ͨ�Է�ɾ������
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

	//���߶Է����Լ��ӵ��ٵĺ�������   ����ʾ���Է�
	g_region->m_gws.SendPakeBlackListName(SenderID,mpMaster->m_userInfo->m_userId,1);//1�������

	//�����е���������ˢһ��
	SendToClient(R2C_UPDATE_ALL_PLAYER,0);
}

//Tianh ��Ϊ��ʱ���ѵļ����п�����Զ�������  ���������ֻ�Ҫ����GW
void CServerFriend::WhenFriend(DWORD vOtherDBID)
{
	//������Լ��ĺ����б�  �����
	if(mConnection.Exist(SConnection::FRIEND,vOtherDBID))
	{
		return;
	}
	//�ں����� Ҳ�����
	if(mConnection.Exist(SConnection::BLACKLIST,vOtherDBID))
	{
		return;
	}
	//������Լ�  �����
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

//����GW��������ʱ���ѵ�����
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



//------����-------
//����ͬ��������
void CServerFriend::OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mConnection.Size(SConnection::FRIEND)<MAX_FRIEND_COUNT)
	{//�ı����ݣ����߶Է��������Լ��Ŀͻ���
		SConnection Temp(vOtherDBID,true,vrOtherName,SConnection::FRIEND,0,0,0,0,"",0);
		mConnection.Insert(vOtherDBID,Temp);

		SendToServer(R2R_SELF_ADD_OTHER_FRIEND_SUCCEED,vOtherDBID);
		SendToClient(R2C_ADD_PLAYER,vOtherDBID,&vrOtherName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vOtherDBID);
		SendToClient(R2C_FRIEND_MESSAGE_WITH_ID,OTHER_ADD_SELF,NULL,vOtherDBID);
	}
	else
	{//�����Ѿ������޷�����
		SendToServer(R2R_FRIEND_MESSAGE,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW);
		SendToClient(R2C_FRIEND_MESSAGE,SELF_FRIEND_LIST_OVERFLOW_OTHER_CANNOT_ADD);
	}
	return;
}

//���˾ܾ��������
void CServerFriend::OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	//�����Լ����ܾ�
	SendToClient(R2C_FRIEND_MESSAGE,OTHER_FRIEND_REFUSE_INVITE);
	return;
}


//���˶Ժ����б���Ӱ��ı仯
void CServerFriend::OtherDateChangeAffectSelf(DWORD vOtherDBID)
{
	SendToClient(R2C_UPDATE_ONE_PLAYER,vOtherDBID);
	return;
}


//���������Լ���������б�
void CServerFriend::OtherInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mConnection.Size(SConnection::FRIEND) <MAX_FRIEND_COUNT)
	{//���͸��Լ���Ϣ
		SendToClient(R2C_OTHER_FRIEND_INVITE,vOtherDBID,&vrOtherName);
	}
	else
	{//��������//���ñ��˵ķ������˺���[���˾ܾ�����]
		//���ݵ�ǰ�����뻥������������Ƿ��Զ��ܾ�
		SendToServer(R2R_SELF_REFUSE_FRIEND_INVITE,vOtherDBID);
		SendToServer(R2R_FRIEND_MESSAGE,vOtherDBID,OTHER_FRIEND_LIST_OVERFLOW);
	}
}

//���������Լ��Ӻ����б�
void CServerFriend::OtherBanish(DWORD vOtherDBID)
{
	//������Ϣ���Լ���ͬʱ�ı�����	
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


//------�Կͻ���--------
//�ӵ��ϴ�����Ϣ
int CServerFriend::ReceiveFromClient(BYTE vOperation,CG_CmdPacket *cmd)
{
	long OtherID;
	SAFE_READ(cmd->ReadLong(&OtherID));

	//������Ϣ��������
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
	case C2R_INVITE_OTHER_FRIEND:			//�Լ�������˼������
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
	case C2R_BANISH_OTHER_FRIEND:			//�Լ�ɾ�����˴Ӻ�����
		BanishOther(OtherID);
		break;
	case C2R_AGREE_OTHER_FRIEND_INVITE:		//�Լ�ͬ����˵�����
		AgreeOtherInvite(OtherID);
		break;
	case C2R_REFUSE_OTHER_FRIEND_INVITE:	//�Լ��ܾ����˵�����
		RefuseOtherInvite(OtherID);
		break;
	case C2R_DELETE_OTHER_ENEMY://ɾ��һ������
		{
			mConnection.Erase(SConnection::ENEMY,OtherID);
			//SendToServer(R2R_DELETE_CASUALTY,OtherID);//�޸Ĺ��Ѿ�û���ܺ�����
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
	//����ĺ��Ѳ�����  ���߲�����
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

//������Ϣ���ͻ���
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
	case R2C_ADD_PLAYER:			//����һ�����
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_DELETE_FRIEND:		//����һ������
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_TEACHER:	//����һ����ʦ
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_MASTER:		//����һ��ʦ��
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_STUDENT:	//����һ��ѧ��
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_DELETE_PRENTICE:	//����һ��ͽ��
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_UPDATE_ONE_PLAYER:	//ˢ��ĳ�����ѵ�����
		{
			g_sendCmd->WriteLong(vParameter);
			map<DWORD,SConnection>::iterator It=mConnection.mMap.find(vParameter);
			
			if(It==mConnection.mMap.end())
			{//û���ҵ�
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
			{//�ҵ�
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
						g_sendCmd->WriteLong(0); //���ܶ�
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
	case R2C_UPDATE_ALL_PLAYER:		//ˢ�����к��ѵ�����
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
	case R2C_FRIEND_MESSAGE:			//ˢһ����Ϣ
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_FRIEND_MESSAGE_WITH_ID:
		g_sendCmd->WriteLong(vDBIDInMessage);
		g_sendCmd->WriteLong(vParameter);
		break;
		//-------����-----------
	case R2C_OTHER_FRIEND_INVITE:				//���������Լ��������
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
	string strone = "δ֪";
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
	string strone = "δ֪";
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
	g_sendCmd->WriteLong(0); //���ܶ�
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
//------�Է����--------
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


//������Ϣ�������
void CServerFriend::SendToServer(BYTE vOperation,DWORD vSendToID,DWORD vParameter,std::string* vpName,DWORD vDBIDInMessage,char IsFristOnline)
{
	SR2rFriendData R2rFriendSendCmd;


	R2rFriendSendCmd.BeginSend();
	R2rFriendSendCmd.mOperation=vOperation;
	R2rFriendSendCmd.mFromDBID=mSelf.mDBID;

	switch(vOperation)
	{
	case R2R_DELETE_FRIEND://����һ������
		R2rFriendSendCmd.mParameter=vParameter;
		break;
	case R2R_UPDATE_SELF_TO_FRIEND://ˢ���Լ������ݵ�����
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
				//��������RegionID
				if(mConnection.Exist(SConnection::FRIEND,pFriend->mpMaster->m_userInfo->m_userId))
				{//�ú����Ѿ����Լ��ĺ����б���
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
	//case R2R_SELF_ADD_OTHER_CASUALTY://�Ѿ�ȡ������趨
	//	break;
	//case R2R_DELETE_CASUALTY://�Ѿ�ȡ������趨
	//	break;
	//case R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF://�Ѿ�ȡ������趨
	//	break;
	//case R2R_UPDATE_SELF_TO_CASUALTY://�Ѿ�ȡ������趨
	//	{
			//�޸Ĺ����Ѿ�û���ܺ�����
			//R2rFriendSendCmd.mOnline=mSelf.mOnline;//ˢ���Լ������ݵ��ܺ���
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


//�ӵ�����˵���Ϣ
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
	case R2R_DELETE_FRIEND://����һ������
		OtherDBID=vrR2rFriendSendCmd.mParameter;
		mConnection.Erase(SConnection::FRIEND,OtherDBID);
		break;
	case R2R_UPDATE_OTHER_FRIEND_TO_SELF://����ˢ�����ݵ��Լ�
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
			{//���Լ��ĺ���
				SConnection Temp;
				Temp.mConnection=SConnection::FRIEND;
				Temp.mOnline=Online;
				Temp.mName=Name;
				mConnection.Insert(FromDBID,Temp);
			}
			else
			{//����˲����Լ��ĺ���//֪ͨ�Է����Լ�ɾ��//����
				SendToServer(R2R_DELETE_FRIEND,FromDBID,mSelf.mDBID);//֪ͨ�Է����Լ�ɾ��//����
			}

			OtherDateChangeAffectSelf(FromDBID);
		}
		break;
	case R2R_FRIEND_MESSAGE:////ˢһ����Ϣ
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
	case R2R_OTHER_FRIEND_ADD_SELF_SUCCEED://���˽���ɹ��������
		{
			char*	pFromName;
			pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherAddSelfSucceed(FromDBID,TempString);
		}
		
		break;
	case R2R_OTHER_FRIEND_AGREE_INVITE://����ͬ��������
		{
			char*	pFromName;
			char    pMetier;
			pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherAgreeInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_REFUSE_FRIEND_INVITE://���˾ܾ��������
		{
			char* pFromName;
			pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherRefuseInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_FRIEND_INVITE://���������Լ��������
		{
			char* pFromName=vrR2rFriendSendCmd.mpFromName;
			TempString=(pFromName);
			OtherInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_FRIEND_BANISH://���������Լ�������
		OtherBanish(FromDBID);
		break;
	case R2R_FORCE_OTHER_FRIEND_UPDATE_TO_SELF:
		{
			SendToServer(R2R_UPDATE_SELF_TO_FRIEND,FromDBID);
		}
		break;
	//case R2R_OTHER_ADD_SELF_CASUALTY://���˼����Լ����ܺ���//�Ѿ�ȡ������趨
	//	{
			/*�޸Ĺ����Ѿ�û���ܺ�����
			SConnection Temp;
			Temp.mDBID=FromDBID;
			Temp.mConnection=SConnection::CASUALTY;
			mConnection.Insert(FromDBID,Temp);
			*/
	//	}
	//	break;
	//case R2R_DELETE_CASUALTY://�Ѿ�ȡ������趨

		//mConnection.Erase(SConnection::CASUALTY,FromDBID);//�޸Ĺ����Ѿ�ȡ���ܺ���
		//break;
	//case R2R_FORCE_OTHER_ENEMY_UPDATE_TO_SELF://�Ѿ�ȡ������趨
	//	{
			//�޸Ĺ����Ѿ�ȡ���ܺ�����
			/*
			if(!mConnection.Find(SConnection::CASUALTY,FromDBID))
			{//�����ܺ���//������ӽ��Լ����ܺ�����
				SConnection Temp;
				Temp.mDBID=FromDBID;
				Temp.mConnection=SConnection::CASUALTY;
				mConnection.Insert(FromDBID,Temp);
			}

			SendToServer(R2R_UPDATE_SELF_TO_CASUALTY,FromDBID);
			*/
		//}
		//break;
	//case R2R_UPDATE_OTHER_ENEMY_TO_SELF://���˵����ݸ��µ��Լ�//�Ѿ�ȡ������趨
	//	{
			//�޸Ĺ����Ѿ�ȡ���˳������ݱ仯���Լ���Ӱ��
			/*
			Online=vrR2rFriendSendCmd.mOnline;
			if(mConnection.Find(SConnection::ENEMY,FromDBID))
			{//���Լ��ĳ���
				SConnection Temp;
				Temp.mConnection=SConnection::ENEMY;
				Temp.mOnline=Online;
				mConnection.Insert(FromDBID,Temp);
			}
			else
			{//����˲����Լ��ĳ���//֪ͨ�����Լ����ܺ�����ȥ��
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

bool CServerFriend::Exist(unsigned short int vConnection,DWORD vDBID)//ĳ�ֹ�ϵ������Ƿ����
{
	return mConnection.Exist(vConnection,vDBID);
}

void CServerFriend::AddEnmity(DWORD vEnemyDBID,std::string& vrName,int vAddKilled)
{
	int Count=MAX_ENEMY_COUNT;
	if(mConnection.Exist(SConnection::ENEMY,vEnemyDBID))
	{//�Ѿ��и����
		Count++;
	}

	if(mConnection.Size(SConnection::ENEMY)<Count)
	{//����δ��
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

	Temp<< LAST_FRIEND_VERSION;//�汾
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
	{//�������
		vrConnectionList+=Temp;
	}
	else
	{//��������
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
	case 100://��֧��
	case 101://��֧��
	case 102://�д���//��֧��
	case 103://����������//��֧����
	case 104://����������//��֧����
	case 105://����������//��֧����
		return;
		break;
	case 106://֧��
		break;
	case 107://����������//��֧��
		return;
		break;
	case 108:
		break;
	default://��֧��
		return;
		break;
	}


	vrConnectionList>>ConnectionCount;
	// region_server::Debug <<"connection size["<< ConnectionCount<<"]\n";

	mConnection.mMap.clear();
	for(int i=0;i!=ConnectionCount;++i)
	{
		SConnection Data;
		vrConnectionList>>Data;//���ݶ�����ʱ����
		
		//��һЩ����������ų�
		if(Data.NoConnection())
		{//û�й�ϵ
		}
		else
		{//��ĳ�ֹ�ϵ
			if(Data.mDBID!=0 && Data.mName!="")
			{//��֤������ȷ
				// region_server::Debug<<Data;
				mConnection.mMap[Data.mDBID]=Data;//����ʱ�����еõ���ֵ
			}
		}
	}

	mConnection.CountPrentice();

	CheckTeacherAndStudentInfo();
	return;
}


//������ʦͽ��ϵ

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


	//��Ҫ������Ϣ����������
	CG_CmdPacket& cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_master_prentice_check);		//Ҫ�����ϵ�ĺ�
	cmd.WriteLong(mSelf.mDBID);
	if(pTeacher==NULL)
	{
		cmd.WriteLong(0);							//��ʦID
		cmd.WriteLong(0);							//��ʦ����ID
	}
	else
	{
		cmd.WriteLong(pTeacher->mDBID);				//��ʦID
		cmd.WriteLong(pTeacher->mConnectionIndex);	//��ʦ����ID
	}

	cmd.WriteShort((unsigned short int )StudentList.size());		//ѧ������
	std::list<SConnection*>::iterator It2;
	for(It2=StudentList.begin();It2!=StudentList.end();++It2)
	{
		cmd.WriteLong((*It2)->mDBID);//ѧ��ID
		cmd.WriteLong((*It2)->mConnectionIndex);//ѧ������ID
	}

	g_region->m_gws.EndSend();
}
bool CServerFriend::MendTeacherAndStudentInfo(CG_CmdPacket *pPacket)//������ʦѧ��ʦ��ͽ�ܹ�ϵ
{
	std::vector<DWORD> arrAddTeacherIndexes;
	std::vector<DWORD> arrAddTeacherIDs;
	std::vector<std::string> arrAddTeacherNames;
	std::vector<DWORD> arrRemoveTeacherIDs;
	std::vector<DWORD> arrAddStudentIndexes;
	std::vector<DWORD> arrAddStudentIDs;
	std::vector<std::string> arrAddStudentNames;
	std::vector<DWORD> arrRemoveStudentIDs;

	// ���ظ��µ�����
	int i;
	short count;
	long prenIndex;
	long id;
	char* name;
	SAFE_READ(pPacket->ReadShort(&count)); // ���Teacher
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&prenIndex));
		SAFE_READ(pPacket->ReadLong(&id));
		SAFE_READ(pPacket->ReadString(&name));
		arrAddTeacherIndexes.push_back(prenIndex);
		arrAddTeacherIDs.push_back(id);
		arrAddTeacherNames.push_back(name);
	}
	SAFE_READ(pPacket->ReadShort(&count)); // ɾ��Teacher
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&id));
		arrRemoveTeacherIDs.push_back(id);
	}
	SAFE_READ(pPacket->ReadShort(&count)); // ���Student
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&prenIndex));
		SAFE_READ(pPacket->ReadLong(&id));
		SAFE_READ(pPacket->ReadString(&name));
		arrAddStudentIndexes.push_back(prenIndex);
		arrAddStudentIDs.push_back(id);
		arrAddStudentNames.push_back(name);
	}
	SAFE_READ(pPacket->ReadShort(&count)); // ɾ��Student
	for (i=0; i<count; i++)
	{
		SAFE_READ(pPacket->ReadLong(&id));
		arrRemoveStudentIDs.push_back(id);
	}



	// ����region��ʦͽ����
	
	//��ɾ����ʦ
	int Index=0;
	for(Index=0;Index!=arrRemoveTeacherIDs.size();++Index)
	{
		Remove(SConnection::TEACHER,arrRemoveTeacherIDs[Index]);
	}

	//��������ʦ
	for(Index=0;Index!=arrAddTeacherIDs.size();++Index)
	{
		AddTeacher(arrAddTeacherIndexes[Index],arrAddTeacherIDs[Index],arrAddTeacherNames[Index]);
	}

	//��ɾ��ѧ��
	for(Index=0;Index!=arrRemoveStudentIDs.size();++Index)
	{
		Remove(SConnection::STUDENT,arrRemoveStudentIDs[Index]);
	}
	//������ѧ��
	for(Index=0;Index!=arrAddStudentIDs.size();++Index)
	{
		AddStudent(arrAddStudentIndexes[Index],arrAddStudentIDs[Index],arrAddStudentNames[Index]);
	}
	return true;
}



void CServerFriend::AddTeacher(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName)	//������ʦ[δ��ʦ]
{
	if(mConnection.Size(SConnection::TEACHER)<MAX_TEACHER_COUNT)
	{//����δ��
		SConnection Temp(vDBID,true,vrName,SConnection::TEACHER,0,vMasterApperenticeIndexID,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}

void CServerFriend::AddStudent(DWORD vMasterApperenticeIndexID,DWORD vDBID,const std::string& vrName)	//����ѧ��[δ��ʦ]
{
	if(mConnection.Size(SConnection::STUDENT)<MAX_STUDENT_COUNT)
	{//����δ��
		SConnection Temp(vDBID,true,vrName,SConnection::STUDENT,0,vMasterApperenticeIndexID,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}

void CServerFriend::AddMaster(DWORD vDBID,const std::string& vrName)									//����ʦ��[�ѳ�ʦ]
{
	if(mConnection.Size(SConnection::MASTER)<MAX_MASTER_COUNT)
	{
		SConnection Temp(vDBID,true,vrName,SConnection::MASTER,0,0,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}

void CServerFriend::AddPrentice(DWORD vDBID,const std::string& vrName)								//����ͽ��[�ѳ�ʦ]
{
	std::vector<SConnection*> PrenticeVector;
	mConnection._Find(SConnection::PRENTICE,PrenticeVector);

	if(PrenticeVector.size()>=MAX_PRENTICE_COUNT)
	{//ȥ�������

		DWORD OldPrenticeDBID=0;
			for(int Index=0;Index!=PrenticeVector.size();++Index)
			{
				if(PrenticeVector[Index]->mJoinIndex==0)
				{
					OldPrenticeDBID=PrenticeVector[Index]->mDBID;
					break;
				}
			}

			Remove(SConnection::PRENTICE,OldPrenticeDBID);//Ҫ�����//����µ��ͻ���
	}


	if(mConnection.Size(SConnection::PRENTICE)<MAX_PRENTICE_COUNT)
	{
		SConnection Temp(vDBID,true,vrName,SConnection::PRENTICE,0,0,0,0,"",0);
		mConnection.Insert(vDBID,Temp);

		SendToClient(R2C_ADD_PLAYER,vDBID,&vrName);
		SendToClient(R2C_UPDATE_ONE_PLAYER,vDBID);
	}
}




