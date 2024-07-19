
#include "gc_include.h"

#include "gc_team.h"
#include "gc_team_const.h"

fox::CStrMap					gTeamStr;//字符串索引

CClientTeam::CClientTeam()
:mData(),mSelfRegionID(0),mSelfDBID(0),m_iInviteTimes(0),mRefuse(false)
{
	mlTime = 0;
} 
 
void CClientTeam::SetID(DWORD vSelfRegionID,DWORD vSelfDBID,std::string& vrSelfName,char vSelfHead)
{
	mSelfRegionID=vSelfRegionID;
	mSelfDBID=vSelfDBID;
	mSelfName=vrSelfName;
	mSelfHead=vSelfHead;
	InitString();
}


CClientTeam::~CClientTeam()
{
}



//------自己----------
//自己邀请别人加入队伍
void CClientTeam::InviteOther(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID)
	{//别人是自己，不做任何处理
		return ;
	}

	if(mData.Empty())
	{//如果当前没有队伍，可以发出邀请//即调用向服务器上传消息
		
		SendToServer(C2R_INVITE_OTHER,vOtherRegionID);
		Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
	}
	else
	{//如果当前已经有队伍了
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			if(mData.Size()<MAX_TEAM_MENBER_COUNT)
			{//人数不满，即调用向服务器上传消息
				time_t curTime = time(NULL);
				if ((curTime-m_LimitTime[vOtherRegionID]) < 30)
				{
					Message(R(MSG_TEAM_CANTREQUESTINTHIRTY));
				}else{
					SendToServer(C2R_INVITE_OTHER,vOtherRegionID);
					m_LimitTime.erase(vOtherRegionID);
					m_LimitTime.insert(pair<long,time_t>(vOtherRegionID,time(NULL)));
					Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
				}
			}
			else
			{//人数满，告诉用户不能邀请
				Message(gTeamStr[TEAM_COUNT_OVER]);
			}
		}
		else
		{//不是队长，告诉用户不能邀请
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return ;
	unguard;
}

void CClientTeam::InviteOther(std::string& vrOtherName)
{
	guard;
	if (!strcmp(vrOtherName.c_str(),GetWorld()->m_pPlayer->GetName()))
	{
		return;
	} 
	if(mData.Empty())
	{//如果当前没有队伍，可以发出邀请//即调用向服务器上传消息

		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_subsystem_team);
		pPacket->WriteByte(C2R_INVITE_OTHER_BY_NAME);
		pPacket->WriteString(vrOtherName.c_str()); // 服务器上有检查长度
		NetSend(pPacket);
		Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
	}
	else
	{
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			if(mData.Size()<MAX_TEAM_MENBER_COUNT)
			{//人数不满，即调用向服务器上传消息
				time_t curTime = time(NULL);
				if ((curTime-m_LimitTimeByName[vrOtherName]) < 30)
				{
					Message(R(MSG_TEAM_CANTREQUESTINTHIRTY));
				}
				else
				{
					CG_CmdPacket* pPacket = NetBeginWrite();
					pPacket->WriteShort(c2r_subsystem_team);
					pPacket->WriteByte(C2R_INVITE_OTHER_BY_NAME);
					pPacket->WriteString(vrOtherName.c_str()); // 服务器上有检查长度
					NetSend(pPacket);
					m_LimitTimeByName.erase(vrOtherName);
					m_LimitTimeByName.insert(pair<std::string,time_t>(vrOtherName,time(NULL)));
					Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
				}
			}
			else
			{//人数满，告诉用户不能邀请
				Message(gTeamStr[TEAM_COUNT_OVER]);
			}
		}
		else
		{//不是队长，告诉用户不能邀请
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	unguard;
}

void CClientTeam::ApplyOtherTeam(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID || !GetPlayer()->m_bTeamOnOff)
	{//别人是自己，不做任何处理
		return ;
	}

	if(mData.Empty())
	{//如果当前没有队伍，可以发出申请//即调用向服务器上传消息
		time_t curTime = time(NULL);
		if ((curTime-m_LimitTime[vOtherRegionID]) < 30)
		{
			Message(R(MSG_TEAM_CANTREQUESTINTHIRTY));
		}else{
			SendToServer(C2R_APPLY_TEAM,vOtherRegionID);
			Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
			m_LimitTime.erase(vOtherRegionID);
			m_LimitTime.insert(pair<long,time_t>(vOtherRegionID,time(NULL)));
		}
	}
	else
	{//如果当前已经有队伍了
		// 告诉用户不能申请
		Message(gTeamStr[NOT_TEAM_CAPTAIN]);
	}
	unguard;
}

void CClientTeam::ApplyOtherTeam(std::string &vrOtherName)
{
	guard;
	if(!strcmp(vrOtherName.c_str(),GetWorld()->m_pPlayer->GetName()) || !GetPlayer()->m_bTeamOnOff)
	{//别人是自己，不做任何处理
		return ;
	}

	if(mData.Empty())
	{//如果当前没有队伍，可以发出申请//即调用向服务器上传消息
		time_t curTime = time(NULL);
		if ((curTime-m_LimitTimeByName[vrOtherName]) < 30)
		{
			Message(R(MSG_TEAM_CANTREQUESTINTHIRTY));
		}else{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_subsystem_team);
			pPacket->WriteByte(C2R_APPLY_TEAM_BY_NAME);
			pPacket->WriteString(vrOtherName.c_str()); // 服务器上有检查长度
			NetSend(pPacket);
			Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
			m_LimitTimeByName.erase(vrOtherName);
			m_LimitTimeByName.insert(pair<std::string,time_t>(vrOtherName,time(NULL)));
		}
	}
	else
	{//如果当前已经有队伍了
		// 告诉用户不能申请
		Message(gTeamStr[NOT_TEAM_CAPTAIN]);
	}
	unguard;
}

//自己同意别人的申请
void CClientTeam::AgreeOtherApply(DWORD vOtherRegionID)
{
	guard;

	if(vOtherRegionID==mSelfRegionID)
	{//别人是自己，不做任何处理
		return ;
	}

	SendToServer(C2R_AGREE_APPLY,vOtherRegionID);
	unguard;
}
//自己拒绝别人的申请
void CClientTeam::RefuseOtherApply(DWORD vOtherRegionID)
{
	guard;

	if(vOtherRegionID==mSelfRegionID)
	{//别人是自己，不做任何处理
		return ;
	}

	SendToServer(C2R_REFUSE_APPLY,vOtherRegionID);
	unguard;
}

void CClientTeam::BanishOtherByRegionID(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID)
	{//如果是自己调用自己离开队伍
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//如果没有队伍，告诉用户
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//如果有队伍
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			SendToServer(C2R_BANISH_OTHER_BY_REGION_ID,vOtherRegionID);
		}
		else
		{//自己不是队长，告诉用户
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return;
	unguard;
}

//自己驱逐别人从队伍中
void CClientTeam::BanishOtherByDBID(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{//如果是自己调用自己离开队伍
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//如果没有队伍，告诉用户
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//如果有队伍
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			SendToServer(C2R_BANISH_OTHER,vOtherDBID);
		}
		else
		{//自己不是队长，告诉用户
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return ;
	unguard;
}



//自己离开队伍
void CClientTeam::SelfLeave()
{
	guard;
	if(mData.Empty())
	{//如果没有队伍//告诉用户
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//如果有队伍
		//告诉服务器
		SendToServer(C2R_SELF_LEAVE,mSelfDBID);
		
		//显示信息
		Message(mData.GetCaptain(),gTeamStr[SELF_LEFT_TEAM]);

		//同时改变当前数据
		mData.Clear();
		TeamDataChange();
		g_layerMain->m_formTeam->OnRefresh();
	}
	unguard;
}



//自己解散队伍
void CClientTeam::Dismiss()
{
	guard;
	if(mData.Empty())
	{//如果没有队伍，告诉用户
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//如果有
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			//告诉服务器
			SendToServer(C2R_DISMISS,mSelfDBID);
			
			//显示信息
			Message(gTeamStr[TEAM_DISMISS]);

			//同时改变当前数据
			mData.Clear();
			TeamDataChange();
			if (RTW_WIDGET("lbrequest")->IsVisible())
				RTW_WIDGET("lbrequest")->Hide();
		}
		else
		{	//如果不是队长，告诉用户
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return;
	unguard;
}


//自己同意别人的邀请
void CClientTeam::AgreeOtherInvite(DWORD vOtherDBID)
{
	guard
	if(vOtherDBID==mSelfDBID)
	{
		return ;
	}

	//告诉服务器
	m_iInviteTimes = 0;
	SendToServer(C2R_AGREE_OTHER_INVITE,vOtherDBID);
	unguard;
}

//自己拒绝别人的邀请
void CClientTeam::RefuseOtherInvite(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}

	//告诉服务器
	m_iInviteTimes = 0;
	SendToServer(C2R_REFUSE_OTHER_INVITE,vOtherDBID);
	unguard;
}



//-------别人-----------
//别人同意加入队伍
void CClientTeam::OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName,char vrOtherhead,byte Metier,long BornTag,long actorID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{//自己同意自己，不处理
		return ;
	}

	if(mData.Empty())
	{//如果队伍不存在
		mData.AddMember(mSelfDBID,mSelfName,mSelfHead,Metier,BornTag,actorID);
		g_layerMain->m_formTeam->OnRefresh();
	}

	if(mData.GetCaptain()==mSelfDBID)
	{//自己是队长
		//改变当前的数据
		mData.AddMember(vOtherDBID,vrOtherName,vrOtherhead,Metier,BornTag,actorID);
		g_layerMain->m_formTeam->OnRefresh();
		TeamDataChange();


		//告诉用户
		Message(vOtherDBID,gTeamStr[MEMBER_ADD_TEAM]);
	}
	else
	{//不是队长
		return;

	}
	unguard;
}

//别人拒绝加入队伍
void CClientTeam::OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	//告诉用户
	std::string Temp(gTeamStr[REFUSE_INVITE]);

	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,vrOtherName);

	Message(Temp);
	unguard;
}

//别人邀请自己加入队伍
void CClientTeam::OtherInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	guard;
	//if(mRefuse)
	//{
	//	RefuseOtherInvite(vOtherDBID);
	//	return;
	//}

	//static int times = 0;
	if(m_iInviteTimes)
	{
		return;
	}
	else
	{
		++m_iInviteTimes;
	}
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}

	//告诉用户
	std::string Temp(gTeamStr[INVITE_ADD_TEAM]);

	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,vrOtherName);
	MessageWithChoice(vOtherDBID,Temp);
	unguard;
}

//别人驱逐自己出队伍
void CClientTeam::OtherBanish(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	if(mData.GetCaptain()==vOtherDBID)
	{
		//告诉用户，同时改变当前数据
		Message(vOtherDBID,gTeamStr[BANISH_FROM_TEAM]);
		mData.Clear();
		TeamDataChange();
		/* gao 2009.12.25
		如果自己被从队伍中踢出，并且自己的队伍列表是打开状态
		*/
		if(g_layerMain->m_formTeam->IsVisibale())
			g_layerMain->m_formTeam->Hide();
		/* end */
	}
	unguard;
}

//别人离开队伍
void CClientTeam::OtherLeave(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	if(mData.Exist(vOtherDBID))
	{
		//告诉用户，同时改变数据
		Message(vOtherDBID,gTeamStr[LEAVE_TEAM]);
		mData.DeleteMember(vOtherDBID);
		g_layerMain->m_formTeam->OnRefresh();
		TeamDataChange();
	}
	unguard;
}


//别人解散队伍
void CClientTeam::OtherDismiss(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	if(mData.GetCaptain()==vOtherDBID)
	{
		//告诉用户，同时改变数据
		Message(gTeamStr[TEAM_DISMISS]);
		mData.Clear();
		TeamDataChange();
	}
	unguard;
}


//别人申请加入自己的队伍
void CClientTeam::OtherApplyTeam(DWORD vOtherDBID,std::string& vrOtherName,DWORD vLevel,short metier/*,DWORD vUnionDBID,std::string& vrUnionName*/,int vrAni,int vrPow)
{
	guard;
	if (!GetPlayer()->m_bTeamOnOff)
	{
		RefuseOtherApply(vOtherDBID);
		return;
	}
	if (mData.Size()<MAX_TEAM_MENBER_COUNT)
	{
		g_layerMain->m_formTeam->SetTeamButtonAnimation();
		// 加入申请列表
		std::string met;
		switch(metier)
		{
		case 1:
			met = string("五台");
			break;
		case 2:
			met = string("花间");
			break;
		case 3:
			met = string("蜀山");
			break;
		case 4:
			met = string("苗疆");
			break;
		default:
			met = string("未知");
			break;
		}
		g_layerMain->m_formTeam->OnApplyListInsertNew(vOtherDBID,vrOtherName,vLevel,met,vrAni,vrPow);//由于组队变动暂时干掉

	}else{
		RefuseOtherApply(vOtherDBID);
	}
	unguard;
}

//别人同意自己的申请
void CClientTeam::OtherAgreeApply(DWORD vOtherDBID)
{
	guard;
	m_LimitTime.clear();
	unguard;
}

//别人拒绝自己的申请
void CClientTeam::OtherRefuseApply(DWORD vOtherDBID)
{
	guard;
	Message(vOtherDBID,R(MSG_TEAM_REFUSEAPPLICATION));
	unguard;
}

void CClientTeam::TransferCaptain(DWORD vOtherDBID)
{
	guard;
	return;
	unguard;
}


//------对服务器--------
//上传消息
void CClientTeam::SendToServer(byte vOperation,DWORD vOtherID)
{
	guard;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_team);
	pPacket->WriteByte(vOperation);

	switch(vOperation)
	{
	case C2R_INVITE_OTHER://自己邀请别人加入队伍
	case C2R_BANISH_OTHER://自己驱逐别人从队伍中
	case C2R_AGREE_OTHER_INVITE://自己同意别人的邀请
	case C2R_REFUSE_OTHER_INVITE://自己拒绝别人的邀请
	case C2R_BANISH_OTHER_BY_REGION_ID:
	case C2R_APPLY_TEAM://申请加入某个队伍
	case C2R_AGREE_APPLY://同意别人申请
	case C2R_REFUSE_APPLY://拒绝别人申请
	case C2R_TRANSFER_CAPTAIN:	// 队长转让
	case C2R_SELECTED_BY_DBID:
		pPacket->WriteLong(vOtherID);
		break;
	case C2R_TEAM_ONOFF:	//组队开关信息,此时发的不是角色ID.而是0和1,0为关,1为开.
	case C2R_SET_PICK_TYPE:
		pPacket->WriteByte(vOtherID);
		break;
	case C2R_SELF_LEAVE://自己离开队伍
	case C2R_DISMISS://自己解散队伍
	default:
		break;
	}
	NetSend(pPacket);
	unguard;
}

//接到消息
void CClientTeam::ReceiveFromServer(CG_CmdPacket *pPacket)
{
	guard;
	byte Operation;
	long OtherDBID;
	READ_byte(Operation);
	
	//解析消息，并处理
	switch(Operation)
	{
	case R2C_ADD_TEAM_MEMBER:			//增加一个队友
		{
			char* pOtherName;
			char OtherHead;
			byte OtherMetier;
			long OtherBornTag;
			long OtherActorID;
			READ_long(OtherDBID);
			READ_string(pOtherName);
			READ_byte(OtherHead);
			READ_byte(OtherMetier);
			READ_long(OtherBornTag);
			READ_long(OtherActorID);
			mData.AddMember(OtherDBID,string(pOtherName),OtherHead,OtherMetier,OtherBornTag,OtherActorID);
			g_layerMain->m_formTeam->OnRefresh();
			TeamDataChange();
		}

		break;
	case R2C_DELETE_TEAM_MEMBER:		//减少一个队友
		READ_long(OtherDBID);
		mData.DeleteMember(OtherDBID);
		g_layerMain->m_formTeam->OnRefresh();
		TeamDataChange();

		break;
	case R2C_UPDATE_ONE_MEMBER_DATA:	//刷新某个成员的数据
		{
			long Hp;
			long MaxHp;
			long Mp;
			long MaxMp;
			char Level;
			long OtherX;
			long OtherY;
			char* OtherRegion;
			long OtherPower;

			
			READ_long(OtherDBID);
			int Index=mData.Find(OtherDBID);
			if(Index==mData.End())
			{//没找到 
				READ_long(Hp);
				READ_long(MaxHp);
				READ_long(Mp);
				READ_long(MaxMp);
				READ_byte(Level);
				READ_long(OtherX);
				READ_long(OtherY);
				READ_string(OtherRegion);
				READ_long(OtherPower);
			}
			else
			{//找到
				READ_long(Hp);
				READ_long(MaxHp);
				READ_long(Mp);
				READ_long(MaxMp);
				READ_byte(Level);
				READ_long(OtherX);
				READ_long(OtherY);
				READ_string(OtherRegion);
				READ_long(OtherPower);
				
				mData[Index].mHp=Hp;
				mData[Index].mMaxHp=MaxHp;
				mData[Index].mMp=Mp;
				mData[Index].mMaxMp=MaxMp;
				mData[Index].mLevel=Level;
				mData[Index].X = OtherX;
				mData[Index].Y = OtherY;
				mData[Index].mRegion = OtherRegion;
				mData[Index].mCombatPower = OtherPower;
				
			}
			TeamMemberDataChange(OtherDBID);
		}
		break;
	case R2C_UPDATE_ALL_MEMBER_DATA:		//刷新所有成员的数据
		{
			char* pTeamData;
			READ_string(pTeamData);
            
			string TeamString;
			TeamString=pTeamData;

			mData.Clear();
			TeamString>>mData;

			TeamDataChange();
		}
		break;
	case R2C_TEAM_MESSAGE:				//刷一个消息
		{
			long Index;
			READ_long(Index);
			Message(gTeamStr[Index]);
		}
		break;
	case R2C_TEAM_MESSAGE_WITH_ID:
		{
			long DBID;
			long Index;
			READ_long(DBID);
			READ_long(Index);

			Message(DBID,gTeamStr[Index]);
		}
		break;
		//-------别人-----------
	case R2C_OTHER_AGREE_INVITE:		//别人同意加入队伍
		{
			char* pOtherName;
			char OtherHead;
			byte metier; //职业
			long BornTag; //生肖
			long actorID; //actorID

			READ_long(OtherDBID);
			READ_string(pOtherName);
			READ_byte(OtherHead);
			READ_byte(metier);
			READ_long(BornTag);
			READ_long(actorID);
			OtherAgreeInvite(OtherDBID,string(pOtherName),OtherHead,metier,BornTag,actorID);
		}

		break;
	case R2C_OTHER_REFUSE_INVITE:		//别人拒绝加入队伍
		{
			char* pOtherName;
			READ_long(OtherDBID);
			READ_string(pOtherName)
			OtherRefuseInvite(OtherDBID,string(pOtherName));
		}
		break;
	case R2C_OTHER_INVITE:				//别人邀请自己加入队伍
		{
			char* pOtherName;
			READ_long(OtherDBID);
			READ_string(pOtherName);
			OtherInvite(OtherDBID,string(pOtherName));
		}
		break;
	case R2C_OTHER_BANISH:				//别人驱逐自己出队伍
		READ_long(OtherDBID);
		OtherBanish(OtherDBID);
		break;
	case R2C_OTHER_LEAVE:				//别人离开队伍
		READ_long(OtherDBID);
		OtherLeave(OtherDBID);
		break;
	case R2C_OTHER_DISMISS:				//别人解散队伍
		READ_long(OtherDBID);
		OtherDismiss(OtherDBID);
		/* gao 2009.12.25
		如果自己被从队伍中踢出，并且自己的队伍列表是打开状态
		*/
		if(g_layerMain->m_formTeam->IsVisibale())
			g_layerMain->m_formTeam->Hide();
		/* end */
		break;
	case R2C_OTHER_APPLY:
		{
			char* pOtherName;
			//char* pUnionName;
			long level;
			short metier;
			//long UnionDBID;
			long OtherAnimal;
			long OtherPower;
			READ_long(OtherDBID);
			READ_string(pOtherName);
			READ_long(level);
			READ_short(metier);
			//READ_long(UnionDBID);
			READ_long(OtherAnimal);
			READ_long(OtherPower);
			/*if (UnionDBID != 0xffffffff)
			{
				READ_string(pUnionName);
				OtherApplyTeam(OtherDBID,string(pOtherName),level,metier,OtherAnimal,OtherPower);
				return;
			}else{
				pUnionName = new char[2];
				strcpy(pUnionName,"");
			}*/
			OtherApplyTeam(OtherDBID,string(pOtherName),level,metier,OtherAnimal,OtherPower);
			RTW_WIDGET("lbrequest")->Show();
		}
		break;
	case R2C_OTHER_AGREE_APPLY:
		READ_long(OtherDBID);
		OtherAgreeApply(OtherDBID);
		break;
	case R2C_OTHER_REFUSE_APPLY:
		READ_long(OtherDBID);
		OtherRefuseApply(OtherDBID);
		break;
	case R2C_TRANSFER_CAPTAIN://转让队长
		READ_long(OtherDBID);
		TransferCaptain(OtherDBID);
		if (mData.GetCaptain() == GetPlayer()->DBID())
		{
			RTW_WIDGET("fmteam.fmteaminfo.fmbtns.btn3")->Enable();
			RTW_WIDGET("fmteam.fmteaminfo.fmbtns.btn4")->Enable();
			RTW_WIDGET("fmteam.fmteaminfo.fmbtns.btn5")->Enable();
		}else
		{
			if (RTW_WIDGET("lbrequest")->IsVisible())
				RTW_WIDGET("lbrequest")->Hide();
		}
		g_layerMain->m_formTeam->OnRefresh();
		break;
	case R2C_SELECTED_BY_DBID:
		{
			long succeed = 0;
			pPacket->SerializeBit(succeed,1);
			if (succeed)
			{
				READ_long(OtherDBID);
				GetPlayer()->OnSetTarget(OtherDBID);
			}
			else
			{
				PlayerFailOperate(R(MSG_CANNOT_SELECT_MEMBER));
			}
		}
		//TransferCaptain(OtherDBID);
		break;
	case R2C_SET_PICK_TYPE:
		{
			if (mData.GetCaptain() != GetPlayer()->DBID())
			{
				byte type = 0;
				READ_byte(type);
				g_layerMain->m_formTeam->SetPickMode(type);
				g_layerMain->m_formTeam->OnRefresh();
			}
			
		}
		break;
	default:
		break;
	}
	unguard;
}


void CClientTeam::Message(DWORD vDBID,const std::string& vrMessage)
{
	guard;
	int Index=mData.Find(vDBID);
	if(Index==mData.End())
	{
		Message(gTeamStr[TEAM_LOCAL_NOT_THIS_DBID_NAME]);
		return;
	}

	std::string Temp(vrMessage);
	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,mData[Index].mName);

	Message(Temp);
	unguard;
}

void CClientTeam::MessageWithChoice(DWORD vFromDBID,DWORD vDBIDInMessage,const std::string& vrMessage)
{
	guard;
	int Index=mData.Find(vDBIDInMessage);
	if(Index==mData.End())
	{
		Message(gTeamStr[TEAM_LOCAL_NOT_THIS_DBID_NAME]);
		return;
	}

	std::string Temp(vrMessage);
	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,mData[Index].mName);

	MessageWithChoice(vFromDBID,Temp);
	unguard;
}




/*
if(mData.Exist(vOtherDBID))
{//有该队员//如果有，上传服务器，同时改变当前数据，显示信息
SendToServer(C2R_BANISH_OTHER,vOtherDBID);
Message(vOtherDBID,gTeamStr[BANISH_MEMBER]);
mData.DeleteMember(vOtherDBID);
if(mData.Size()==1)
{//人数不足，自动解散队伍
mData.Clear();
Message(gTeamStr[NOT_MEMBER_TEAM_DISMISS]);
}

TeamDataChange();
}
else
{//没有该队员//告诉用户
Message(gTeamStr[MEMBER_NOT_EXIST]);
}
*/


