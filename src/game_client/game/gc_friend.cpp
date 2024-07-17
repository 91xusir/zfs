
#include "gc_include.h"
#include "gc_friend.h"

#include "gc_mail_const.h"



CClientFriend::CClientFriend()
:mRefuse(false)
{
}

CClientFriend::~CClientFriend()
{
}

void CClientFriend::SetID(DWORD vRegionID,DWORD vDBID)
{
	guard;
	mSelf.mDBID=vDBID;
	mSelf.mOnline=true;
	mSelfRegionID=vRegionID;
	InitString(gMailStr);
	unguard;
}

void CClientFriend::InviteOther(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID)
	{//�������Լ��������κδ���
		Message(gMailStr[SELF_CAN_NOT_ADD_SELF]);
		return ;
	}
	GcActor* pOther = GetWorld()->m_ActorManager.FindActive(vOtherRegionID);
	if (mConnection._Find(SConnection::FRIEND,pOther->GetName()))
	{
		Message(gMailStr[FRIEND_ALREADY_IN_LIST]);
		return;
	}
	if (!vOtherRegionID)
	{
		PlayerFailOperate(R(OBJECT_NOT_IN_REGION));
		return;
	}
	if(mConnection.Size(SConnection::FRIEND) <MAX_FRIEND_COUNT)
	{//�����ǰ��������û�г������ޣ����Է�������//��������������ϴ���Ϣ
		SendToServer(C2R_INVITE_OTHER_FRIEND,vOtherRegionID);
		Message(gMailStr[FRIEND_WAIT_PLAYER_RESPOND]);
	}
	else
	{//�������������û���������
		Message(gMailStr[FRIEND_LIST_OVERFLOW]);
	}
	return ;
	unguard;
}

void CClientFriend::InviteOther(std::string &vrOtherName)
{
	guard;
	if(!strcmp(vrOtherName.c_str(),mSelf.mName.c_str()))
	{//�������Լ��������κδ���
		Message(gMailStr[SELF_CAN_NOT_ADD_SELF]);
		return ;
	}
	if (mConnection._Find(SConnection::FRIEND,vrOtherName))
	{
		Message(gMailStr[FRIEND_ALREADY_IN_LIST]);
		return;
	}
	if(mConnection.Size(SConnection::FRIEND) <MAX_FRIEND_COUNT)
	{//�����ǰ��������û�г������ޣ����Է�������//��������������ϴ���Ϣ
		CG_CmdPacket* pPacket = NetBeginWrite();
		if (!pPacket)
			return;
		pPacket->WriteShort(c2r_subsystem_mail);
		pPacket->WriteByte(C2R_INVITE_OTHER_FRIEND_BYNAME);
		pPacket->WriteLong(0);
		pPacket->WriteString(vrOtherName.c_str());
		NetSend(pPacket);
		char info[128];
		rt2_sprintf(info,R(MSG_FRIEND_INVITE),vrOtherName.c_str());
		PlayerFailOperate(info);
	}
	else
	{//�������������û���������
		Message(gMailStr[FRIEND_LIST_OVERFLOW]);
	}
	return ;
	unguard;
}

//void CClientFriend::DeleteEnemy(DWORD vOtherDBID)
//{
//	guard;
//	if(vOtherDBID==mSelf.mDBID)
//	{
//		return;
//	}
//
//	if(mConnection.Exist(SConnection::ENEMY,vOtherDBID))
//	{//�ó��˴��ڣ��ϴ���������ͬʱ�ı䵱ǰ���ݣ���ʾ��Ϣ
//		SendToServer(C2R_DELETE_OTHER_ENEMY,vOtherDBID);
//		Message(vOtherDBID,R(SELF_DELETE_ENEMY));
//		mConnection.Erase(SConnection::ENEMY,vOtherDBID);
//		DataChange();
//	}
//	else
//	{//�ó��˲�����
//		Message(R(ENEMY_NOT_IN_LIST));
//	}
//
//	unguard;
//}
void CClientFriend::AddOtherToBlackList(std::string &vrOtherName)
{
	guard;
	if (vrOtherName == mSelf.mName)
	{
		return;
	}
	if (!vrOtherName.c_str())
	{
		PlayerFailOperate("");
	}
	if (mConnection.Size(SConnection::BLACKLIST) <MAX_BLACKLIST_COUNT)
	{
		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_subsystem_mail);
		pPacket->WriteByte(C2R_ADD_BLACKLIST);
		pPacket->WriteLong(0);
		pPacket->WriteString((char*)vrOtherName.c_str());
		NetSend(pPacket);
		return;
		//Message(gMailStr[""]);
	} 
	else
	{
		//Message(gMailStr[BLACK_LIST_OVERFLOW]);
	}
	unguard;
}
void CClientFriend::DeleteOtherFromBlackList(DWORD vOtherDBID)
{
	guard;
	if (vOtherDBID == mSelf.mDBID)
	{
		return;
	}
	if (mConnection.Exist(SConnection::BLACKLIST,vOtherDBID))
	{
		SendToServer(C2R_DELETE_BLACKLIST,vOtherDBID);
		//Message(vOtherDBID,"  ");
		mConnection.Erase(SConnection::BLACKLIST,vOtherDBID);
		DataChange();
	} 
	else
	{
		//Message("");
	}
	
	unguard;
}
//void CClientFriend::DeleteTemporaryFriend(DWORD vOtherDBID)
//{
//	guard;
//	/*if(vOtherDBID == mSelf.mDBID)
//	{
//		return;
//	}
//	if(mConnection.Exist(SConnection::WHENFRIEND,vOtherDBID))
//	{
//		SendToServer(C2R_DELETE_OTHER_TEMPORARYFRIEND,vOtherDBID);
//		Message(vOtherDBID,R(SELF_DELETE_ENEMY));
//		mConnection.Erase(SConnection::WHENFRIEND,vOtherDBID);
//		DataChange();
//	}
//	else
//	{
//		Message(R(ENEMY_NOT_IN_LIST));
//	}*/
//	unguard;
//}

void CClientFriend::BanishOther(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelf.mDBID)
	{
		return;
	}

	if(mConnection.Exist(SConnection::FRIEND,vOtherDBID))
	{//�ú��Ѵ��ڣ��ϴ���������ͬʱ�ı䵱ǰ���ݣ���ʾ��Ϣ
		SendToServer(C2R_BANISH_OTHER_FRIEND,vOtherDBID);
		Message(vOtherDBID,gMailStr[SELF_BANISH_OTHER]);
		mConnection.Erase(SConnection::FRIEND,vOtherDBID);
		DataChange();
	}
	else
	{//�ú��Ѳ�����
		Message(gMailStr[FRIEND_NOT_IN_LIST]);
	}

	return;
	unguard;
}

void CClientFriend::AgreeOtherInvite(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelf.mDBID)
	{
		return ;
	}

	//���߷�����
	SendToServer(C2R_AGREE_OTHER_FRIEND_INVITE,vOtherDBID);
	return;
	unguard;
}

void CClientFriend::RefuseOtherInvite(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelf.mDBID)
	{
		return;
	}

	//���߷�����
	SendToServer(C2R_REFUSE_OTHER_FRIEND_INVITE,vOtherDBID);
	return;
	unguard;
}

void CClientFriend::OtherInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	guard;
	//if(mRefuse)
	//{
	//	RefuseOtherInvite(vOtherDBID);
	//	return;
	//}

	if(vOtherDBID==mSelf.mDBID)
	{
		return;
	}

	//�����û�
	std::string Temp(gMailStr[OTHER_INVITE_SELF]);

	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,vrOtherName);

	MessageWithChoice(vOtherDBID,Temp);
	unguard;
}


//�ӵ���Ϣ
void CClientFriend::ReceiveFromServer(byte vOperation,CG_CmdPacket *pPacket)
{
	guard;
	long OtherDBID;
	switch(vOperation)
	{
	case R2C_ADD_PLAYER:			//����һ�����
		{
			char* pOtherName;
			/*char* pSchool=NULL;
			long  Level;
			char* pAddress = NULL;
			long  Resolve;*/
			READ_long(OtherDBID);
			READ_string(pOtherName);
			//READ_l(pSchool);
			//READ_long(Level);
			//READ_string(pAddress);
			//READ_long(Resolve);

			SConnection Temp(OtherDBID,true,string(pOtherName),0,0,0,0,0,"",0);
			mConnection.Insert(OtherDBID,Temp);
			DataChange();
		}
		break;
	case R2C_DELETE_FRIEND:			//����һ������
		{
			READ_long(OtherDBID);
			mConnection.Erase(SConnection::FRIEND,OtherDBID);
			DataChange();
		}
		break;
	case R2C_DELETE_TEACHER:			//����һ����ʦ
		{
			READ_long(OtherDBID);
			mConnection.Erase(SConnection::TEACHER,OtherDBID);
			DataChange();
		}
		break;
	case R2C_DELETE_STUDENT:			//����һ��ѧ��
		{
			READ_long(OtherDBID);
			mConnection.Erase(SConnection::STUDENT,OtherDBID);
			DataChange();
		}
		break;
	case R2C_DELETE_MASTER:			//����һ��ʦ��
		{
			READ_long(OtherDBID);
			mConnection.Erase(SConnection::MASTER,OtherDBID);
			DataChange();
		}
		break;
	case R2C_DELETE_PRENTICE:			//����һ��ͽ��
		{
			READ_long(OtherDBID);
			mConnection.Erase(SConnection::PRENTICE,OtherDBID);
			DataChange();
		}
		break;

	case R2C_UPDATE_ONE_PLAYER:		//ˢ��ĳ����Ա������
		{
			byte Online;
			long Connection;
			long KilledCount;
			char* pPlayerName=NULL;
			long  School;
			long  Level;
			char* pAddress = NULL;
			long  Resolve;

			
			READ_long(OtherDBID);
			READ_byte(Online);
			READ_long(Connection);
			READ_long(KilledCount);
			READ_string(pPlayerName);//temptemptemp
			READ_long(School);
			READ_long(Level);
			READ_string(pAddress);
			READ_long(Resolve);

			map<DWORD,SConnection>::iterator It=mConnection.mMap.find(OtherDBID);

			if(It==mConnection.mMap.end())
			{//û�ҵ� 
			}
			else
			{//�ҵ�
				It->second.mDBID;
				It->second.mOnline=(Online!=0);
				It->second.mConnection=Connection;
				It->second.mKilledCount=KilledCount;
				It->second.mLevel = Level;
				It->second.mResolve = Resolve;
				It->second.mSchool=School;
				if(pPlayerName!=NULL)
				{
					It->second.mName=pPlayerName;
				}
				
				if (pAddress!=NULL)
				{
					It->second.mAddress=pAddress;
				}
			}
			MemberDataChange(OtherDBID);
		}
		break;
	case R2C_UPDATE_ALL_PLAYER:		//ˢ�����г�Ա������
		{
			long	Count;
			
			long	DBID;
			byte	Online;
			char*	pName;

			long Connection;
			long KilledCount;
			long School;
			long  Level;
			char* pAddress = NULL;
			long  Resolve;

			//READ_long(Count);
			//mConnection.mMap.clear();
			//for (int i = 0;i != Count; ++i)
			//{
			
				READ_long(DBID);
				READ_byte(Online);
				READ_string(pName);
				READ_long(Connection);
				READ_long(KilledCount);
				READ_long(School);
				READ_long(Level);
				READ_string(pAddress);
				READ_long(Resolve);
	
				mConnection.mMap[(DWORD)DBID]=SConnection((DWORD)DBID,(Online==1),string(pName),Connection,KilledCount,0,School,Level,string(pAddress),Resolve);
			//}
			DataChange();
		}
		break;
	case R2C_FRIEND_MESSAGE:		//ˢһ����Ϣ
		{
			long Index;
			READ_long(Index);
			Message(gMailStr[Index]);
		}
		break;
	case R2C_FRIEND_MESSAGE_WITH_ID:
		{
			long DBID;
			long Index;
			READ_long(DBID);
			READ_long(Index);

			Message(DBID,gMailStr[Index]);

		}
		break;
	
	case R2C_OTHER_FRIEND_INVITE:	//���������Լ�����
		{
			char* pOtherName;
			READ_long(OtherDBID);
			READ_string(pOtherName);
			OtherInvite(OtherDBID,string(pOtherName));
		}
		break;
	case R2C_ADD_WHEN_FRIEND:       //�����ʱ���� add by ���� 2010.05.27
		{
			char* Name;
			long  DBID;
			short Metier;
			READ_string(Name);
			READ_long(DBID);
			READ_short(Metier);
			InsertWhenFriend(Name,DBID,Metier);
		}
		break;
	default:
		break;
	}
	return;
	unguard;
}

void CClientFriend::SendToServer(byte vOperation,DWORD vOtherID)
{
	guard;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_mail);
	pPacket->WriteByte(vOperation);
	pPacket->WriteLong(vOtherID);
	/*
	switch(vOperation)
	{
	case C2R_INVITE_OTHER_FRIEND://�Լ�������˼������
	pPacket->WriteLong(vOtherID);
	break;
	case C2R_BANISH_OTHER_FRIEND://�Լ�ɾ�����˴Ӻ�����
	pPacket->WriteLong(vOtherID);
	break;
	case C2R_AGREE_OTHER_FRIEND_INVITE://�Լ�ͬ����˵�����
	pPacket->WriteLong(vOtherID);
	break;
	case C2R_REFUSE_OTHER_FRIEND_INVITE://�Լ��ܾ����˵�����
	pPacket->WriteLong(vOtherID);
	break;
	default:
	break;
	}
	*/
	NetSend(pPacket);
	return;
	unguard;
}


void CClientFriend::Message(DWORD vDBIDInMessage,const std::string& vrMessage)
{
	guard;
	map<DWORD,SConnection>::iterator It;
	It=mConnection.mMap.find(vDBIDInMessage);

	std::string Temp(vrMessage);

	if(It==mConnection.mMap.end())
	{
		Temp=gMailStr[MAIL_LOCAL_NOT_THIS_DBID_NAME];
	}
	else
	{
		std::string::size_type First=Temp.find("%s");
		Temp.replace(First,2,It->second.mName);
	}



	Message(Temp);
	unguard;
}

void CClientFriend::MessageWithChoice(DWORD vFromDBID,DWORD vDBIDInMessage,const std::string& vrMessage)
{
	guard;
	map<DWORD,SConnection>::iterator It;
	It=mConnection.mMap.find(vDBIDInMessage);

	std::string Temp(vrMessage);

	if(It==mConnection.mMap.end())
	{
		Temp=gMailStr[MAIL_LOCAL_NOT_THIS_DBID_NAME];
	}
	else
	{
		std::string::size_type First=Temp.find("%s");
		Temp.replace(First,2,It->second.mName);
	}


	MessageWithChoice(vFromDBID,Temp);
	unguard;
}

bool CClientFriend::Exist(long vConnection,DWORD vDBID)
{
	guard;
	return mConnection.Exist(vConnection,vDBID);
	unguard;
}

DWORD CClientFriend::GetDBID(long vConnection,std::string& vrName)
{
	guard;
	map<DWORD,SConnection>::iterator It;
	for(It=mConnection.mMap.begin();It!=mConnection.mMap.end();It++)
	{
		if(It->second.mName==vrName)
		{
			if(It->second.Is(vConnection))
			{
				return It->first;
			}
		}
	}

	return 0;
	unguard;
}






