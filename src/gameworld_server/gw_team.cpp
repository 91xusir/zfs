
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "gw_team.h"
#include "gw_server.h"
#include "game_team_share_const.h"
#include <sstream>

extern GWServer                g_server;

CG_CmdPacket& GwTeamManager::RegionPack(DWORD vSendToDBID)
{
	CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_team);
	rPacket.WriteLong(vSendToDBID);
	return rPacket;
}
 
void GwTeamManager::SendToRegion(GWUser* vpSendToUser)
{
	int Session= g_server.GetRegionSession(vpSendToUser->regionID);
	if(Session==-1)
	{
		return;
	}

	g_server.m_region.SendToPeer(Session);
	return ;
}

GWUser* GwTeamManager::FindUser(DWORD vUserDBID)
{
	//return g_server.FindActorByIdWithoutSwitchRegion(vUserDBID);
	return g_server.FindActorById(vUserDBID);
}

void GwTeamManager::ReceiveFromRegion(CG_CmdPacket* vpPacket)
{
	BYTE	Operation=0;
	long	SelfDBID=0;
	long	OtherDBID=0;
	char*    Name;

	vpPacket->ReadLong(&SelfDBID);
	vpPacket->ReadByte((char*)&Operation);
	
	switch(Operation)
	{
	case R2GW_INVITE_OTHER://自己邀请别人加入队伍[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_INVITE_OTHER\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			InviteOther(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_INVITE_OTHER_BY_NAME:
		{
#ifdef _DEBUG
			LOG("R2GW_INVITE_OTHER_BY_NAME\n");//自己邀请别人加入队伍[Name]
#endif
			long UesrDBID = 0;
			vpPacket->ReadString(&Name);
			GWUser *pOtherGWUser= g_server.FindActorByName(Name);
			if (pOtherGWUser)
			{
				UesrDBID = pOtherGWUser->userID;
			}
			GWUser *pGWUser= g_server.FindActorById(SelfDBID);
			if (pGWUser)
			{
				InviteOtherbyName(pGWUser,UesrDBID);
			}
			
		}
		break;
	case R2GW_BANISH_OTHER://自己驱逐别人从队伍中[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_BANISH_OTHER\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			BanishOther(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_SELF_LEAVE://自己离开队伍
		{
#ifdef _DEBUG
			LOG("R2GW_SELF_LEAVE\n");
#endif
			SelfLeave(SelfDBID);
		}
		break;
	case R2GW_DISMISS://自己解散队伍
		{
#ifdef _DEBUG
			LOG("R2GW_DISMISS\n");
#endif
			Dismiss(SelfDBID);
		}
		break;
	case R2GW_AGREE_OTHER_INVITE://自己同意别人的邀请[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_AGREE_OTHER_INVITE\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			AgreeOtherInvite(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_REFUSE_OTHER_INVITE://自己拒绝别人的邀请[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_REFUSE_OTHER_INVITE\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			RefuseOtherInvite(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_DATE_CHANGE://自己的数据变化影响到队伍其他成员[int vHp,int vMaxHp,int vMp,int vMaxMp]
		{
#ifdef _DEBUG
			LOG("R2GW_DATE_CHANGE\n");
#endif
			long Hp;
			long MaxHp;
			long Mp;
			long MaxMp;
			char Level;
			long X;
			long Y;
			char* mRegion; //地图名字
            long mCombatPower;   //战力评估

			vpPacket->ReadLong(&Hp);
			vpPacket->ReadLong(&MaxHp);
			vpPacket->ReadLong(&Mp);
			vpPacket->ReadLong(&MaxMp);
			vpPacket->ReadByte(&Level);
			vpPacket->ReadLong(&X);
			vpPacket->ReadLong(&Y);
			vpPacket->ReadString(&mRegion);
			vpPacket->ReadLong(&mCombatPower);

			//自己的对队伍有影响的数值变化
			DateChangeAffectTeam(SelfDBID,Hp,MaxHp,Mp,MaxMp,Level,X,Y,mRegion,mCombatPower);
		}
		break;
	case R2GW_TEAM_ONOFF:
		{
			char cFree;
			vpPacket->ReadByte(&cFree);
			FreeOrLockTeam(SelfDBID, cFree);
		}
		break;
	case R2GW_APPLY_ADDTEAM:
		vpPacket->ReadLong(&OtherDBID);
		ApplyAddTeam(SelfDBID, OtherDBID);
		break;
	case R2GW_OTHER_AGREE_APPLY:
		vpPacket->ReadLong(&OtherDBID);
		AgreeApply(SelfDBID, OtherDBID);
		break;
	case R2GW_REFUSE_TEAM:
		vpPacket->ReadLong(&OtherDBID);
		RefuseTeam(SelfDBID, OtherDBID);
		break;
	case R2GW_TRANSFER_CAPTAIN:
		vpPacket->ReadLong(&OtherDBID);
		TransferCaptain(SelfDBID, OtherDBID);
		break;
	case R2GW_SET_PICK_TYPE:
		{
			char nType;
			vpPacket->ReadByte(&nType);
			std::ostringstream os;
			const char* szBuf[3] = {"自由", "队长", "随机"};
			if (0 <= nType && nType <3)
			{
				bool bOK = SetPickType(SelfDBID, nType);
				if (bOK)
				{
					os<<"队长设置拾取模式为："<<szBuf[nType]<<"拾取模式";
					SendTeamPicker(SelfDBID, os.str().c_str());
				}	
			}
		}
		break;
	case R2GW_TEAM_PICKER:
		{
			char *pMsg = NULL;
			vpPacket->ReadString(&pMsg);
			SendTeamPicker(SelfDBID, pMsg);
		}
		break;
	case R2GW_SETREFUSE_TEAM:
		{
			long OtherID;
			vpPacket->ReadLong(&OtherID);

			GWUser *pGWUser= g_server.FindActorById(OtherID);
			if (!pGWUser)
			{
				return;
			}

			CG_CmdPacket& rPacket=RegionPack(pGWUser->userID);

			rPacket.WriteByte(GW2R_SETREFUSE_TEAM);//别人设置了拒绝组队

			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				return;
			}

			g_server.m_region.SendToPeer(Session);

		}
		break;
	default:
		break;
	}
	return ;
}

void GwTeamManager::InviteOtherbyName(GWUser *pGWUser,DWORD vOtherDBID)
{
	CG_CmdPacket& rPacket=RegionPack(pGWUser->userID);

	rPacket.WriteByte(GW2R_INVITE_OTHER_BY_NAME);//别人解散队伍[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);
	
	int Session= g_server.GetRegionSession(pGWUser->regionID);
	if(Session==-1)
	{
		return;
	}

	g_server.m_region.SendToPeer(Session);
}

//------自己----------
//自己邀请别人加入队伍
void GwTeamManager::InviteOther(DWORD vSelfDBID,DWORD vOtherDBID)
{
	if(vSelfDBID==vOtherDBID)
	{//如果是自己邀请自己，不处理
		return ;
	}

	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//自己不存在，不处理
		return;
	}

	if(pSelf->team==NULL)
	{//如果当前没有队伍，可以发出邀请//调用别人邀请自己
		GWUser* pOther=FindUser(vOtherDBID);
		if(pOther==NULL)
		{//别人不存在//发消息给自己
			SendToRegionTeamMessage(pSelf,THAT_PLAYER_NOT_INLINE);
			return;
		}

		OtherInvite(pSelf,pOther);
	}
	else
	{//如果当前已经有队伍了
		if(pSelf->team->GetCaptain()==vSelfDBID)
		{//是队长
			if(pSelf->team->Size()<MAX_TEAM_MENBER_COUNT)
			{//人数不满，//调用别人邀请自己
				GWUser* pOther=FindUser(vOtherDBID);
				if(pOther==NULL)
				{//别人不存在
					SendToRegionTeamMessage(pSelf,THAT_PLAYER_NOT_INLINE);
				}
				else
				{
					OtherInvite(pSelf,pOther);
				}
			}
			else
			{//人数满，不邀请
			}
		}
		else
		{//自己不是队长，不邀请
		}
	}
	return;
}

//自己驱逐别人从队伍中
void GwTeamManager::BanishOther(DWORD vSelfDBID,DWORD vOtherDBID)
{
	if(vSelfDBID==vOtherDBID)
	{//如果是自己调用自己离开队伍
		SelfLeave(vSelfDBID);
		return;
	}

	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//自己不存在，不处理
		return;
	}

	if(pSelf->team==NULL)
	{//没有队伍//不处理
		return;
	}

	CTeamData& rTeam=*(pSelf->team);

	if(rTeam.GetCaptain()!=vSelfDBID)
	{//不是队长//不处理
		return;
	}

	if(rTeam.Exist(vOtherDBID))
	{//有该队员，调用别人驱逐自己的服务器端，同时改变当前数据
		
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			if(rTeam[i].mDBID==vOtherDBID)
			{//是被驱逐的那个人，发驱逐消息
				GWUser* pOther=FindUser(vOtherDBID);
				if(pOther!=NULL)
				{//找到被驱逐的玩家
					//发送消息被驱逐的玩家//清空数据
					SendToRegionOtherBanish(pOther,vSelfDBID);
					pOther->team=NULL;//将该玩家队伍清空
				}
			}
			else if(rTeam[i].mDBID==vSelfDBID)
			{//是自己
				SendToRegionTeamMessageWithID(pSelf,vOtherDBID,BANISH_MEMBER);
				SendToRegionDeleteTeamMember(pSelf,vOtherDBID);
			}
			else
			{//是其它队友，发减少一个人的消息，并发送相关信息

				GWUser* pMember=FindUser(rTeam[i].mDBID);
				if(pMember!=NULL)
				{//找到其他成员
					SendToRegionTeamMessageWithID(pMember,vOtherDBID,BANISH_MEMBER);
					SendToRegionDeleteTeamMember(pMember,vOtherDBID);
				}
			}
		}

		rTeam.DeleteMember(vOtherDBID);
		if(rTeam.Size()==1)
		{//人数不足，自动解散队伍
			SendToRegionTeamMessage(pSelf,NOT_MEMBER_TEAM_DISMISS);
			SendToRegionDeleteTeamMember(pSelf,vSelfDBID);
			DEL_ONE(pSelf->team);
			//delete pSelf->team;
			//pSelf->team=NULL;
		}
	}
	else
	{//没有该队员，告之客户端
		SendToRegionTeamMessage(pSelf,MEMBER_NOT_EXIST);
	}
	return;
}

void GwTeamManager::SelfLeave(GWUser* vpSelf)
{
	if(vpSelf->team==NULL)
	{//如果没有，不处理
		return;
	}

	CTeamData& rTeam=*(vpSelf->team);
	//数据变动
	rTeam.DeleteMember(vpSelf->userID);
	vpSelf->team=NULL;

	if(rTeam.Size()==1)
	{//人数不够，自动解散
		GWUser* pMember=FindUser(rTeam.GetCaptain());
		if(pMember!=NULL)
		{
			if(pMember->team!=NULL)
			{
				SendToRegionOtherLeave(pMember,vpSelf->userID);
				SendToRegionDeleteTeamMember(pMember,pMember->userID);
				SendToRegionTeamMessage(pMember,NOT_MEMBER_TEAM_DISMISS);

				//清除队伍
				DEL_ONE(pMember->team);
				//delete pMember->team;
				//pMember->team=NULL;
			}
		}
	}
	else
	{
		//剩下的队友
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			GWUser* pMember=FindUser(rTeam[i].mDBID);
			if(pMember!=NULL)
			{//找到队友
				if(pMember->team!=NULL)
				{
					SendToRegionOtherLeave(pMember,vpSelf->userID);
				}
			}
		}
	}
}

//自己离开队伍
void GwTeamManager::SelfLeave(DWORD vSelfDBID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//自己不存在//不处理
		return;
	}
	SelfLeave(pSelf);
}

//自己解散队伍
void GwTeamManager::Dismiss(DWORD vSelfDBID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//自己不存在//不处理
		return;
	}

	if(pSelf->team==NULL)
	{//没有队伍//不处理
		return;
	}

	CTeamData& rTeam=*(pSelf->team);

	if(rTeam.GetCaptain()==vSelfDBID)
	{//是队长
		//调用队伍中其它人的别人解散队伍[除了自己]
		for(int i=rTeam.Begin(); i!=rTeam.End();++i)
		{
			if(rTeam[i].mDBID !=vSelfDBID)
			{//除了自己//发消息给队友//队伍解散//改变数据
				GWUser* pMember=FindUser(rTeam[i].mDBID);
				if(pMember!=NULL)
				{
					SendToRegionOtherDismiss(pMember,vSelfDBID);
					pMember->team=NULL;
				}
			}
		}

		//改变数据
		DEL_ONE(pSelf->team);
		//delete pSelf->team;
		//pSelf->team=NULL;
	}
	else
	{//不是队长//不处理
	}
	return;
}


//自己同意别人的邀请
void GwTeamManager::AgreeOtherInvite(DWORD vSelfDBID,DWORD vOtherDBID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{
		return;
	}

	GWUser* pOther=FindUser(vOtherDBID);
	if(pOther==NULL)
	{
		return;
	}

	if(pSelf->team==NULL)
	{//没有队伍//调用别人的函数
		OtherAgreeInvite(pSelf,pOther);
		SendDateToWhenFriend(vSelfDBID,vOtherDBID);
	}
	else
	{//已经有队伍了//自动拒绝别人的邀请
		SendToRegionOtherRefuseInvite(pOther,vSelfDBID,pSelf->name.c_str());
	}
	return;
}

void GwTeamManager::SendDateToWhenFriend(DWORD SelfDBID,DWORD OtherDBID)
{
	GWUser* pSelf=FindUser(SelfDBID);
	if(pSelf==NULL)
	{
		return;
	}

	CG_CmdPacket& rPacket=RegionPack(OtherDBID);
	rPacket.WriteByte(GW2R_TO_WHENFRIEND);//要求region刷新自己的数据到GameWorld[无]
	SendToRegion(pSelf);
}


//自己拒绝别人的邀请
void GwTeamManager::RefuseOtherInvite(DWORD vSelfDBID,DWORD vOtherDBID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{
		return;
	}
	
	GWUser* pOther=FindUser(vOtherDBID);
	if(pOther==NULL)
	{
		return;
	}


	SendToRegionOtherRefuseInvite(pOther,vSelfDBID,pSelf->name.c_str());
}



//自己的对队伍有影响的数值变化
void GwTeamManager::DateChangeAffectTeam(DWORD vSelfDBID,int vHp,int vMaxHp,int vMp,int vMaxMp,char vLevel,long X,long Y,const char* MapName,long mCombatPower)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{
		return;
	}

	if(pSelf->team==NULL)
	{
		return;
	}

	CTeamData& rTeam=*(pSelf->team);

	for(int i=rTeam.Begin();i!=rTeam.End();++i)
	{
		if(rTeam[i].mDBID==vSelfDBID)
		{//自己
			rTeam[i].mHp=vHp;
			rTeam[i].mMaxHp=vMaxHp;
			rTeam[i].mMp=vMp;
			rTeam[i].mMaxMp=vMaxMp;
			rTeam[i].mLevel=vLevel;
			rTeam[i].X=X;
			rTeam[i].Y=Y;
			rTeam[i].mRegion=MapName;
			rTeam[i].mCombatPower=mCombatPower;
		}
		else
		{//其他队友
			GWUser* pMember=FindUser(rTeam[i].mDBID);
			if(pMember!=NULL)
			{
				SendToRegionUpdateOneMemberData(pMember,vSelfDBID,vHp,vMaxHp,vMp,vMaxMp,vLevel,X,Y,MapName,mCombatPower);
			}
		}
	}
	return;
}





//-------别人-----------

//别人同意加入队伍
void GwTeamManager::OtherAgreeInvite(GWUser* vpOther,GWUser* vpSelf)
{
	if(vpSelf->team==NULL)
	{//当前队伍为空//创建一个队伍//加入自己
		vpSelf->team=RT_NEW CTeamData;
		if(vpSelf->team==NULL)
		{
			return;
		}
		vpSelf->team->AddMember(vpSelf->userID,vpSelf->name,vpSelf->attr.HeadImageID,vpSelf->attr.metier,vpSelf->attr.BornTag,vpSelf->attr.actorID);
		//GetDataFromMaster();

	}

	CTeamData& rTeam=*(vpSelf->team);

	if(rTeam.GetCaptain()!=vpSelf->userID)
	{//不是队长//不处理
		return;
	}

	if(rTeam.Size()>=MAX_TEAM_MENBER_COUNT)
	{//队伍是否已经满，满则，发送消息给别人[人满]
		SendToRegionTeamMessage(vpOther,TEAM_COUNT_OVER);
		return;
	}

	if(rTeam.Exist(vpOther->userID))
	{//已经在这个队伍中
		SendToRegionTeamMessage(vpOther,ALREADY_IN_TEAM);
		return;
	}

	//更改数据，发送消息给队员[自己，其它队友]
	
	//更改数据
	rTeam.AddMember(vpOther->userID,vpOther->name,vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);
	vpOther->team=&rTeam;

	//其他队员
	for(int i= rTeam.Begin();i!=rTeam.End();++i)
	{
		if(rTeam[i].mDBID==vpSelf->userID)
		{//自己
			SendToRegionAddTeamMember(vpOther,vpSelf->userID,vpSelf->name.c_str(),vpSelf->attr.HeadImageID,vpSelf->attr.metier,vpSelf->attr.BornTag,vpSelf->attr.actorID);//告诉新队员加队长

			SendToRegionOtherAgreeInvite(vpSelf,vpOther->userID,vpOther->name.c_str(),vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);
			SendToRegionRequestMemberData(vpSelf);//要求更新数据
		}
		else if(rTeam[i].mDBID==vpOther->userID)
		{//新队友
			SendToRegionAddTeamMember(vpOther,vpOther->userID,vpOther->name.c_str(),vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);//告诉新队员加他自己
			SendToRegionRequestMemberData(vpOther);//要求更新数据
			SendToRegionTeamMessageWithID(vpOther,vpSelf->userID,ADD_TEAM_SUCCEED);//发送消息
		}
		else
		{//老队友
			GWUser* pMember=FindUser(rTeam[i].mDBID);
			if(pMember!=NULL)
			{
				SendToRegionAddTeamMember(pMember,vpOther->userID,vpOther->name.c_str(),vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);//给老队员加新队员的名单
				SendToRegionTeamMessageWithID(pMember,vpOther->userID,MEMBER_ADD_TEAM);//告诉老队友，新人加入

				SendToRegionAddTeamMember(vpOther,pMember->userID,pMember->name.c_str(),pMember->attr.HeadImageID,pMember->attr.metier,pMember->attr.BornTag,pMember->attr.actorID);//给新队友加老队友
				SendToRegionRequestMemberData(pMember);//要求更新数据
			}
		}
	}
	return;
}




//别人邀请自己加入队伍
void GwTeamManager::OtherInvite(GWUser* vpOther,GWUser* vpSelf)
{
	if(vpSelf->team==NULL)
	{//没有队伍//发送消息给自己
		/*if(!vpSelf->bTeamFree)
		{
			SendToRegionTeamMessage(vpOther,THAT_PLAYER_TEAM_OFF);
		}
		else */
		{
			SendToRegionOtherInvite(vpSelf,vpOther->userID,vpOther->name.c_str());
		}
	}
	else
	{//已经有队伍
		
		if(vpSelf->team->Exist(vpOther->userID))
		{//在同一个队伍//发送消息给邀请者
			SendToRegionTeamMessage(vpOther,ALREADY_IN_TEAM);
		}
		else
		{//已经加入别的队伍//发送消息给邀请者
			SendToRegionTeamMessage(vpOther,THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
		}

		//发消息给邀请者//别人拒绝加入
		SendToRegionOtherRefuseInvite(vpOther,vpSelf->userID,vpSelf->name.c_str());
	}
}


void GwTeamManager::SendToRegionAddTeamMember(GWUser* vpSendToUser,DWORD vMemberDBID,const char* vpMemberName,int headImageID,char Metier,long BornTag,long actorID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_ADD_TEAM_MEMBER);//增加一个队友[DWORD vMemberDBID,char* vpMemberName]
	rPacket.WriteLong((long)vMemberDBID);
	rPacket.WriteString((char*)vpMemberName);
	rPacket.WriteByte((char)headImageID);
	rPacket.WriteByte(Metier);
	rPacket.WriteLong(BornTag);
	rPacket.WriteLong(actorID);

	SendToRegion(vpSendToUser);
}

void GwTeamManager::SendToRegionDeleteTeamMember(GWUser* vpSendToUser,DWORD vMemberDBID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_DELETE_TEAM_MEMBER);//减少一个队友[DWORD vMemberDBID]
	rPacket.WriteLong((long)vMemberDBID);

	SendToRegion(vpSendToUser);
}

void GwTeamManager::FreeOrLockTeam(DWORD vSelfDBID, char cFree)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{
		return;
	}
	pSelf->bTeamFree = (0 != cFree);
}

void GwTeamManager::ApplyAddTeam(unsigned long vSelfDBID, unsigned long vOtherID)
{
	GWUser* pSelf = FindUser(vSelfDBID);
	GWUser* pOther = FindUser(vOtherID);
	do
	{
		if(NULL == pSelf || NULL == pOther)
		{
			break;
		}
		if(NULL == pOther->team)
		{
			SendToRegionTeamMessage(pSelf, TEAM_NOT_EXIST);
			break;
		}
		if(pOther->team->Size() >= MAX_TEAM_MENBER_COUNT)
		{
			SendToRegionTeamMessage(pSelf, TEAM_COUNT_OVER);
			break;
		}
		unsigned long dwHeadID = pOther->team->GetCaptain();
		if(dwHeadID != vOtherID)
		{
			pOther = FindUser(dwHeadID);
			if(NULL == pOther)
			{
				break;
			}
			
		}
		/*if(!pOther->bTeamFree)
		{
			SendToRegionTeamMessage(pSelf, THAT_PLAYER_TEAM_OFF);
			break;
		}*/
		CG_CmdPacket& rPacket=RegionPack(pOther->userID);
		rPacket.WriteByte(GW2R_APPLY_ADDTEAM);
		rPacket.WriteLong((long)vSelfDBID);
		rPacket.WriteString((char*)(pSelf->name.c_str()));
		rPacket.WriteLong(pSelf->attr.level);
		rPacket.WriteShort(pSelf->attr.metier);
		rPacket.WriteLong(pSelf->attr.BornTag);
		rPacket.WriteShort(pSelf->attr.actorID);
		//unsigned long lUn = 0xffffffff;
		//if(0 != pSelf->unionID)
		//{
		//	lUn = pSelf->unionID;
		//	rPacket.WriteLong(lUn);
		//	rPacket.WriteString((char*)(pSelf->unionName.c_str()));
		//}
		//else
		//{
		//	rPacket.WriteLong(lUn);
		//}
		SendToRegion(pOther);
	}while(false);
}

void GwTeamManager::AgreeApply(unsigned long vSelfDBID, unsigned long vOtherID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	GWUser* pOther = FindUser(vOtherID);

	do
	{
		if(NULL == pSelf)
		{
			break;
		}
		if(NULL == pSelf->team)
		{
			break;
		}
		if(pSelf->team->GetCaptain() != vSelfDBID)
		{
			break;
		}
		if(NULL == pOther)
		{
			SendToRegionTeamMessage(pSelf, THAT_PLAYER_NOT_INLINE);
			break;
		}

		if(pSelf->team->Exist(vOtherID))
		{
			SendToRegionTeamMessage(pSelf,ALREADY_IN_TEAM);
			break;
		}

		if(NULL != pOther->team)
		{
			SendToRegionTeamMessage(pSelf, THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
			break;
		}
		if(pSelf->team->Size() >= MAX_TEAM_MENBER_COUNT)
		{
			SendToRegionTeamMessage(pSelf, TEAM_COUNT_OVER);
			break;
		}
	
		pSelf->team->AddMember(pOther->userID, pOther->name, pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);
		pOther->team = pSelf->team;
	
		CTeamData& rTeam=*(pSelf->team);
		//其他队员
		for(int i= rTeam.Begin();i!=rTeam.End();++i)
		{
			if(rTeam[i].mDBID==pSelf->userID)
			{//自己
				SendToRegionAddTeamMember(pOther, pSelf->userID, pSelf->name.c_str(),pSelf->attr.HeadImageID,pSelf->attr.metier,pSelf->attr.BornTag,pSelf->attr.actorID);//告诉新队员加队长

				SendToRegionOtherAgreeInvite(pSelf,pOther->userID,pOther->name.c_str(),pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);
				SendToRegionRequestMemberData(pSelf);//要求更新数据
			}
			else if(rTeam[i].mDBID==pOther->userID)
			{//新队友
				SendToRegionAddTeamMember(pOther,pOther->userID,pOther->name.c_str(),pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);//告诉新队员加他自己
				SendToRegionRequestMemberData(pOther);//要求更新数据
				SendToRegionTeamMessageWithID(pOther,pSelf->userID,ADD_TEAM_SUCCEED);//发送消息
			}
			else
			{//老队友
				GWUser* pMember=FindUser(rTeam[i].mDBID);
				if(pMember!=NULL)
				{
					SendToRegionAddTeamMember(pMember,pOther->userID,pOther->name.c_str(),pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);//给老队员加新队员的名单
					SendToRegionTeamMessageWithID(pMember,pOther->userID,MEMBER_ADD_TEAM);//告诉老队友，新人加入

					SendToRegionAddTeamMember(pOther,pMember->userID,pMember->name.c_str(),pMember->attr.HeadImageID,pMember->attr.metier,pMember->attr.BornTag,pMember->attr.actorID);//给新队友加老队友
					SendToRegionRequestMemberData(pMember);//要求更新数据
				}
			}
		}
	}while(false);
}

void GwTeamManager::RefuseTeam(unsigned long vSelfDBID, unsigned long vOtherID)
{
	do
	{
		GWUser* pSelf=FindUser(vSelfDBID);
		GWUser* pOther=FindUser(vOtherID);
		if(pSelf==NULL || pOther==NULL)
		{
			break;
		}
		if(NULL == pSelf->team)
		{
			break;
		}
		if(pSelf->team->GetCaptain() != vSelfDBID)
		{
			break;
		}
		CG_CmdPacket& rPacket=RegionPack(pOther->userID);
		rPacket.WriteByte(GW2R_REFUSE_TEAM);
		rPacket.WriteLong((long)vSelfDBID);
		rPacket.WriteString((char*)(pSelf->name.c_str()));

		SendToRegion(pOther);
	}while(false);
}

void GwTeamManager::TransferCaptain(unsigned long vSelfDBID, unsigned long vOtherID)
{
	do
	{
		GWUser* pSelf=FindUser(vSelfDBID);
		GWUser* pOther=FindUser(vOtherID);
		if(pSelf==NULL || pOther==NULL)
		{
			break;
		}
		if(NULL == pSelf->team)
		{
			break;
		}
		if(pSelf->team->GetCaptain() != vSelfDBID)
		{
			break;
		}
		CTeamData& rTeam=*(pSelf->team);

		//bool bFind = false;
		int nNew = -1;
		STeamData Temp;
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			if (rTeam[i].mDBID == vOtherID)
			{
				Temp = rTeam[i];
				rTeam[i] = rTeam[0];
				rTeam[0] = Temp;
				nNew = i;
				break;
			}
		}
		if (-1 == nNew)
		{
			break;
		}
		
		
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			GWUser* pO=FindUser(rTeam[i].mDBID);
			if (NULL == pO)
			{
				continue;
			}
			CG_CmdPacket& rPacket=RegionPack(pO->userID);
			rPacket.WriteByte(GW2R_TRANSFER_CAPTAIN);
			rPacket.WriteLong((long)vOtherID);
			rPacket.WriteLong((long)nNew);
			SendToRegion(pO);
		}
	}while(false);
}

void GwTeamManager::SendTeamPicker(unsigned long vSelfDBID, const char* pMsg)
{
	do
	{
		GWUser* pSelf = FindUser(vSelfDBID);
		if(pSelf==NULL )
		{
			break;
		}
		if(NULL == pSelf->team)
		{
			break;
		}
		
		CTeamData& rTeam=*(pSelf->team);
		
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			GWUser* pO=FindUser(rTeam[i].mDBID);
			if (NULL == pO)
			{
				continue;
			}
			CG_CmdPacket& rPacket=RegionPack(pO->userID);
			rPacket.WriteByte(GW2R_TEAM_PICKER);
			rPacket.WriteString(pMsg);

			SendToRegion(pO);
		}
	}while(false);
}

bool GwTeamManager::SetPickType(unsigned long vSelfDBID, char cType)
{
	bool bRet = false;
	do 
	{
		GWUser* pSelf=FindUser(vSelfDBID);
		if(pSelf==NULL)
		{
			break;
		}
		if(NULL == pSelf->team)
		{
			break;
		}
		if(pSelf->team->GetCaptain() != vSelfDBID)
		{
			break;
		}
		CTeamData& rTeam=*(pSelf->team);
		bool bOK = rTeam.SetPickType((TeamPickType)(cType));
		if (!bOK)
		{
			break;
		}
		
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			GWUser* pO=FindUser(rTeam[i].mDBID);
			if (NULL == pO)
			{
				continue;
			}
			CG_CmdPacket& rPacket=RegionPack(pO->userID);
			rPacket.WriteByte(GW2R_SET_PICK_TYPE);
			rPacket.WriteByte(cType);

			SendToRegion(pO);
		}
		
		bRet = true;
	} while (false);
	return bRet;
}

void GwTeamManager::SendToRegionUpdateOneMemberData(GWUser* vpSendToUser, DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp,char vLevel,long X,long Y,const char* MapName,long mCombatPower)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_UPDATE_ONE_MEMBER_DATA);//刷新某个成员的数据[DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp]
	rPacket.WriteLong((long)vMemberDBID);
	rPacket.WriteLong((long)vHp);
	rPacket.WriteLong((long)vMaxHp);
	rPacket.WriteLong((long)vMp);
	rPacket.WriteLong((long)vMaxMp);
	rPacket.WriteByte(vLevel);
	rPacket.WriteLong(X);
	rPacket.WriteLong(Y);
	rPacket.WriteString((char*)MapName);
	rPacket.WriteLong(mCombatPower);

	SendToRegion(vpSendToUser);

}



void GwTeamManager::SwitchRegion(GWUser* vpSelf)
{
	if(vpSelf==NULL)
	{//存在
		return ;
	}

	if(vpSelf->team==NULL)
	{//没有队伍
		return;
	}


	SendToRegionUpdateAllMemberData(vpSelf);
	return;
}


void GwTeamManager::SendToRegionUpdateAllMemberData(GWUser* vpSendToUser)
{
	
	string TeamString;
	CTeamData& rTeam=*(vpSendToUser->team);
	TeamString<<rTeam;



	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_UPDATE_ALL_MEMBER_DATA);//刷所有成员的数据
	rPacket.WriteString((char*)TeamString.c_str());
	SendToRegion(vpSendToUser);

	return;
}

void GwTeamManager::SendToRegionRequestMemberData(GWUser* vpSendToUser)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_REQUEST_MEMBER_DATA);//要求region刷新自己的数据到GameWorld[无]
	SendToRegion(vpSendToUser);
}

void GwTeamManager::SendToRegionTeamMessage(GWUser* vpSendToUser,int vMessageID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_TEAM_MESSAGE);//刷一个消息[int vMessageID]
	rPacket.WriteLong((long)vMessageID);

	SendToRegion(vpSendToUser);
}

void GwTeamManager::SendToRegionTeamMessageWithID(GWUser* vpSendToUser,DWORD vDBIDInMessage,int vMessageID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_TEAM_MESSAGE_WITH_ID);//刷一个消息[带ID][DWORD vDBIDInMessage,int vMessageID]
	rPacket.WriteLong((long)vDBIDInMessage);
	rPacket.WriteLong((long)vMessageID);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherAgreeInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName,char vOtherHead,char metier, long BornTag,long actorID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_AGREE_INVITE);//别人同意加入队伍[DWORD vOtherDBID,char* vpOtherName]
	rPacket.WriteLong((long)vOtherDBID);
	rPacket.WriteString((char*)vpOtherName);
	rPacket.WriteByte(vOtherHead);
	rPacket.WriteByte(metier);
	rPacket.WriteLong(BornTag);
	rPacket.WriteLong(actorID);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherRefuseInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_REFUSE_INVITE);//别人拒绝加入队伍[DWORD vOtherDBID,char* vpOtherName]
	rPacket.WriteLong((long)vOtherDBID);
	rPacket.WriteString((char*)vpOtherName);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_INVITE);
	rPacket.WriteLong((long)vOtherDBID);
	rPacket.WriteString((char*) vpOtherName);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherBanish(GWUser* vpSendToUser,DWORD vOtherDBID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_BANISH);//别人驱逐自己出队伍[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherLeave(GWUser* vpSendToUser,DWORD vOtherDBID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_LEAVE);//别人离开队伍[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);


	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherDismiss(GWUser* vpSendToUser,DWORD vOtherDBID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);

	rPacket.WriteByte(GW2R_OTHER_DISMISS);//别人解散队伍[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);

	SendToRegion(vpSendToUser);
}





/*
//别人离开队伍
void GwTeamManager::OtherLeave( DWORD vOtherDBID,GWUser* vpSelf)
{
if(vpSelf->team==NULL)
{//没有队伍//不处理
return;
}
CTeamData& rTeam = *(vpSelf->team);

if(rTeam.Exist(vOtherDBID))
{//在队伍中
//发送消息到自己//改变数据
SendToRegionOtherLeave(vpSelf,vOtherDBID);
rTeam.DeleteMember(vOtherDBID);

if(rTeam.Size()==1)
{//人数不够，自动解散
SendToRegionDeleteTeamMember(vpSelf,vpSelf->userID);
SendToRegionTeamMessage(vpSelf,NOT_MEMBER_TEAM_DISMISS);

delete vpSelf->team;
vpSelf->team=NULL;
}
}
}
*/













