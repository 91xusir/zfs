
#include "region.h"
#include "region_team.h"
#include "game_team_share_const.h"

#include "region_object.h"
#include "scene/rts_sv_scene.h"
#include "region_creature.h"
#include "region_scene.h"
#include "cmd_channel.h"
#include "region_server.h"
#include "PlayerLogic.h"

#define GET_PARAMS() CmdGiver = g_region->m_cmdGiver;
#define SAFE_READ(p) if(!(p)) return CMD_ERROR_READ_DATA;

CG_CmdPacket CServerTeam::mSendPacket;

inline CRegionUser* FindByDBID(DWORD vDBID)
{
	return g_region->FindUser(vDBID);
}


CServerTeam::CServerTeam(CRegionCreature* vpMaster,DWORD vSelfRegionID,DWORD vSelfDBID,std::string& vrSelfName,char vSelfHead)
:mData(),
mSelfRegionID(vSelfRegionID),
mSelfDBID(vSelfDBID),
mSelfName(vrSelfName),
mSelfHead(vSelfHead),
mpMaster(vpMaster),
m_nPickerIncex(0)
{
}

CServerTeam::~CServerTeam()
{
}

void CServerTeam::SendToGameWorld()			//送一个包到GameWorld
{
	g_region->m_gws.SendPacket(&mSendPacket);
}

CRegionCreature* CServerTeam::FindByRegionID(DWORD vRegionID)
{
	//这里是用RegionID
	CRegionCreature* pCreature = mpMaster->m_scene->FindCreature(vRegionID);

	if(pCreature==NULL)
	{
		//送一个消息到客户端
		CG_CmdPacket* pPacket=ClientPacket();
		pPacket->WriteByte(R2C_TEAM_MESSAGE);
		pPacket->WriteLong(TEAM_OBJECT_NOT_IN_REGION);
		SendToClient();
	}

	return pCreature;
}

DWORD CServerTeam::RegionIDbyName(const char* name)
{
	if (mpMaster->m_scene)
	{
		CRegionCreature* pCreature = mpMaster->m_scene->FindUserByName(name);
		if (pCreature == NULL)
		{
			//发送消息到GameWorld
			CG_CmdPacket* pPacket= GameWorldPacket();
			pPacket->WriteByte(R2GW_INVITE_OTHER_BY_NAME);
			pPacket->WriteString((char*)name);
			SendToGameWorld();
			return 0;
		}
		if (!pCreature->m_userInfo)
		{
			return 0;
		}
		else
		{
			return pCreature->m_userInfo->m_userId;
		}
	}	
}

DWORD CServerTeam::RegionID2DBID(DWORD vOtherRegionID)
{
	CRegionCreature* pCreature = mpMaster->m_scene->FindCreature(vOtherRegionID);
	if(pCreature==NULL)
	{//不存在该物件
		return 0;
	}

	if(pCreature->m_userInfo==NULL)
	{//不是玩家无法组队
		//发送消息到客户端
		CG_CmdPacket* pPacket=ClientPacket();
		pPacket->WriteByte(R2C_TEAM_MESSAGE);
		pPacket->WriteLong(THAT_OBJECT_NOT_PLAYER);
		SendToClient();
		return 0;
	}
	else
	{
		return pCreature->m_userInfo->m_userId;
	}
}


//----------------------------------------------------------------------------------------------

//------自己----------
//自己邀请别人加入队伍
void CServerTeam::FindInviteOther(DWORD vOtherRegionID)
{
	DWORD OtherDBID=RegionID2DBID(vOtherRegionID);
	InviteOther(OtherDBID);
}
void CServerTeam::InviteOther(DWORD OtherDBID)
{
	/*DWORD OtherDBID=RegionID2DBID(vOtherRegionID);*/
	if(OtherDBID==0){return;}

	bool bIn = mpMaster->GetPlayerLogic()->IsInInviteMap(OtherDBID);
	if(bIn)
	{
		mpMaster->SendSystemMessage(R(MSG_TEAM_CANTREQUESTINTHIRTY));
		return;
	}
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_INVITE_OTHER);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();
	mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
	return;
}

//自己通过名字邀请组队
void CServerTeam::InviteOtherbyName(const char* name)
{
	DWORD OtherDBID=RegionIDbyName(name);
	if(OtherDBID==0){return;}

	bool bIn = mpMaster->GetPlayerLogic()->IsInInviteMap(OtherDBID);
	if(bIn)
	{
		mpMaster->SendSystemMessage(R(MSG_TEAM_CANTREQUESTINTHIRTY));
		return;
	}
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_INVITE_OTHER);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();
	mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
	return;
}


//自己驱逐别人从队伍中[用RegionID]
void CServerTeam::BanishOtherByRegionID(DWORD vOtherRegionID)
{
	DWORD OtherDBID=RegionID2DBID(vOtherRegionID);
	if(OtherDBID==0){return;}
	BanishOther(OtherDBID);
	return;
}

//自己驱逐别人从队伍中
void CServerTeam::BanishOther(DWORD vOtherDBID)
{

	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_BANISH_OTHER);
	pPacket->WriteLong(vOtherDBID);
	SendToGameWorld();

	return;
}

//自己离开队伍
void CServerTeam::SelfLeave()
{
	mData.Clear();//数据清空
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_SELF_LEAVE);
	SendToGameWorld();
	return;
}


//自己解散队伍
void CServerTeam::Dismiss()
{
	mData.Clear();//数据清空
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_DISMISS);
	SendToGameWorld();
	return;
}


//自己同意别人的邀请
void CServerTeam::AgreeOtherInvite(DWORD vOtherDBID)
{
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_AGREE_OTHER_INVITE);
	pPacket->WriteLong(vOtherDBID);
	SendToGameWorld();
	return;
}

//自己拒绝别人的邀请
void CServerTeam::RefuseOtherInvite(DWORD vOtherDBID)
{
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_REFUSE_OTHER_INVITE);
	pPacket->WriteLong(vOtherDBID);
	SendToGameWorld();
}




//自己的对队伍有影响的数值变化
void CServerTeam::DateChangeAffectTeam()
{
	if(mData.Empty())
	{
		return;
	}

	int Index=mData.Find(mSelfDBID);
	if(Index==mData.End())
	{
		return;
	}

	//long x , y = 0;
	//CRegionUser *pRegionUser = g_region->FindUser(mSelfDBID);
	//if (pRegionUser)
	//{
	//	if (pRegionUser->m_dummy)
	//	{
	//		x = pRegionUser->m_dummy->m_pos[0];
	//		y = pRegionUser->m_dummy->m_pos[1];
	//	}	
	//}
	//
	string MapName = "";
	if (mpMaster->m_scene)
	{
		RtsSceneBlockMap* pMap = mpMaster->m_scene->FindBlockByPos(mpMaster->m_pos);
		if (pMap)
		{
			MapName = pMap->m_szFileName;
		}
	}


	if(mData[Index].mHp==mpMaster->m_core.GetHp() 
		&& mData[Index].mMp==mpMaster->m_core.GetMp()
		&& mData[Index].mMaxHp==mpMaster->m_core.GetMaxHp()
		&& mData[Index].mMaxMp==mpMaster->m_core.GetMaxMp()
		&& mData[Index].mLevel==mpMaster->m_core.Lev 
		&& mData[Index].X==mpMaster->m_pos[0]
		&& mData[Index].Y==mpMaster->m_pos[1]
		&& mData[Index].mRegion==MapName
		&& mData[Index].mCombatPower==mpMaster->m_core.WarScores)
	{
		return;
	}

	mData[Index].mHp=mpMaster->m_core.GetHp();
	mData[Index].mMp=mpMaster->m_core.GetMp();
	mData[Index].mMaxHp=mpMaster->m_core.GetMaxHp();
	mData[Index].mMaxMp=mpMaster->m_core.GetMaxMp();
	mData[Index].mLevel=mpMaster->m_core.Lev;
	mData[Index].X=mpMaster->m_pos[0];
	mData[Index].Y=mpMaster->m_pos[1];
	//mData[Index].mName = m_core.Name; //玩家名字
	//mData[Index].mSchool = m_core.Metier; //职业
	mData[Index].mRegion = MapName; //地图名字
	mData[Index].mCombatPower = mpMaster->m_core.WarScores; //战力评估
	//mData[Index].mAnimal = m_core.BornTag; //生肖
	//mData[Index].mActorID = m_core.actorID;//模型ID


	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_DATE_CHANGE);
	pPacket->WriteLong(mData[Index].mHp);
	pPacket->WriteLong(mData[Index].mMaxHp);
	pPacket->WriteLong(mData[Index].mMp);
	pPacket->WriteLong(mData[Index].mMaxMp);
	pPacket->WriteByte(mData[Index].mLevel);
	pPacket->WriteLong(mData[Index].X);
	pPacket->WriteLong(mData[Index].Y);
	pPacket->WriteString((char*)mData[Index].mRegion.c_str());
	pPacket->WriteLong(mData[Index].mCombatPower);
	//mData[Index].mName = m_core.Name; //玩家名字
	//mData[Index].mSchool = m_core.Metier; //职业
	//mData[Index].mAnimal = m_core.BornTag; //生肖
	//mData[Index].mActorID = m_core.actorID;//模型ID
	SendToGameWorld();
}


void CServerTeam::FreeOrLockTeam(char cFree)
{
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_TEAM_ONOFF);
	pPacket->WriteByte(cFree);
	SendToGameWorld();
}

void CServerTeam::ApplyAddTeam( unsigned long vOtherID)
{
	DWORD OtherDBID=RegionID2DBID(vOtherID);
	if(OtherDBID==0){return;}

	bool bIn = mpMaster->GetPlayerLogic()->IsInInviteMap(OtherDBID);
	if(bIn)
	{
		mpMaster->SendSystemMessage(R(MSG_TEAM_CANTREQUESTINTHIRTY));
		return;
	}
	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_APPLY_ADDTEAM);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();

}

void CServerTeam::AgreeApply(unsigned long vOtherID)
{
	//DWORD OtherDBID=RegionID2DBID(vOtherID);
	//if(OtherDBID==0){return;}

	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_OTHER_AGREE_APPLY);
	pPacket->WriteLong(vOtherID);
	SendToGameWorld();
}

void CServerTeam::RefuseTeam(unsigned long vOtherID)
{
	DWORD OtherDBID=RegionID2DBID(vOtherID);
	if(OtherDBID==0){return;}

	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_REFUSE_TEAM);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();
}

void CServerTeam::TransferCaptain(unsigned long vOtherID)
{
	//DWORD OtherDBID=RegionID2DBID(vOtherID);
	//if(OtherDBID==0){return;}

	//发送消息到GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_TRANSFER_CAPTAIN);
	pPacket->WriteLong(vOtherID);
	SendToGameWorld();
}

void CServerTeam::SetPickType(char nType)
{
	do 
	{
		if (mData.GetCaptain() != mSelfDBID)
		{
			break;
		}
		CG_CmdPacket* pPacket= GameWorldPacket();
		pPacket->WriteByte(R2GW_SET_PICK_TYPE);
		pPacket->WriteByte(nType);
		SendToGameWorld();
	} while (false);
}
//------对客户端--------
//接到上传的消息
int CServerTeam::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket* cmd)
{
	char Operation;
	long OtherID;
	char cFree = 1;
	char* name;

	SAFE_READ(cmd->ReadByte(&Operation))

	//解析消息，并处理
	switch(Operation)
	{
	    case C2R_INVITE_OTHER_BY_NAME:
			SAFE_READ(cmd->ReadString(&name))
			InviteOtherbyName(name);
		    break;
		case C2R_INVITE_OTHER://自己邀请别人加入队伍
			//LOG("C2R_INVITE_OTHER\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			FindInviteOther(OtherID); //这里接收的是RegionID
			break;
		case C2R_BANISH_OTHER://自己驱逐别人从队伍中
			//LOG("C2R_BANISH_OTHER\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			BanishOther(OtherID);
			break;
		case C2R_SELF_LEAVE://自己离开队伍
			//LOG("C2R_SELF_LEAVE\n");
			SelfLeave();
			break;
		case C2R_DISMISS://自己解散队伍
			//LOG("C2R_DISMISS\n");
			Dismiss();
			break;
		case C2R_AGREE_OTHER_INVITE://自己同意别人的邀请
			//LOG("C2R_AGREE_OTHER_INVITE\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			AgreeOtherInvite(OtherID);
			break;
		case C2R_REFUSE_OTHER_INVITE://自己拒绝别人的邀请
			//LOG("C2R_REFUSE_OTHER_INVITE\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			RefuseOtherInvite(OtherID);
			break;
		case C2R_BANISH_OTHER_BY_REGION_ID://驱逐别人用RegionID[DWORD vOtherRegionID]
			//LOG("C2R_BANISH_OTHER_BY_REGION_ID\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			BanishOtherByRegionID(OtherID);
			break;
		case C2R_TEAM_ONOFF: //锁定或开启组队 0是关,关的时候别人不能加自己的队,自己也不能被邀请.1是开.相反.
			SAFE_READ(cmd->ReadByte(&cFree))
			if(NULL != CmdGiver)
			{
				FreeOrLockTeam(cFree);
				CmdGiver->GetPlayerLogic()->SetTeamFree( 0 != cFree);
			}
			break;
		case C2R_APPLY_TEAM:
			SAFE_READ(cmd->ReadLong(&OtherID))
			ApplyAddTeam(OtherID);
			break;
		case C2R_AGREE_APPLY:
			SAFE_READ(cmd->ReadLong(&OtherID))
			AgreeApply(OtherID);
		case C2R_REFUSE_APPLY:
			SAFE_READ(cmd->ReadLong(&OtherID))
			RefuseTeam(OtherID);
			break;
		case C2R_TRANSFER_CAPTAIN: //转让队长
			SAFE_READ(cmd->ReadLong(&OtherID))
				TransferCaptain(OtherID);
			break;
		case C2R_SELECTED_BY_DBID:
			SAFE_READ(cmd->ReadLong(&OtherID))
				CRSLogicExt::GetRSLogicExt().SelectedByDBID(CmdGiver, OtherID);
			break;
		case C2R_SET_PICK_TYPE:
			{
				char cType;
				SAFE_READ(cmd->ReadByte(&cType))
					SetPickType(cType);
			}
			break;
		default:
			return CMD_ERROR_UNKNOWN;
			break;
	}
	return CMD_ERROR_NONE;
} 

int CServerTeam::ReceiveFromGameWorld(CG_CmdPacket *cmd)
{
	char Operation;
	SAFE_READ(cmd->ReadByte(&Operation))
	CG_CmdPacket* pPacket=ClientPacket();
	
	switch(Operation)
	{
	case GW2R_INVITE_OTHER_BY_NAME:
		{
			long OtherDBID;
			cmd->ReadLong(&OtherDBID);
			InviteOther(OtherDBID);
			return CMD_ERROR_NONE;
		}
		break;
	case GW2R_ADD_TEAM_MEMBER:			//增加一个队友
		//LOG("GW2R_ADD_TEAM_MEMBER\n");
		{
			long MemberDBID;	
			char* pMemberName;
			char MemberHead;
			char Metier;
			long BornTag;
			long actorID;
			cmd->ReadLong(&MemberDBID);
			cmd->ReadString(&pMemberName);
			cmd->ReadByte(&MemberHead);
			cmd->ReadByte(&Metier);
			cmd->ReadLong(&BornTag);
			cmd->ReadLong(&actorID);

			//把新队友增加到临时好友里
			if (mpMaster->m_pFriend)
			{
				mpMaster->m_pFriend->WhenFriend(MemberDBID);
			}
			
			
			string MemberName(pMemberName);
			mData.AddMember(MemberDBID,MemberName,MemberHead,Metier,BornTag,actorID); //region数据更新
			

			//if (mpMaster->m_pFriend)
			//{
			//	mpMaster->m_pFriend->WhenFriend(MemberDBID);
			//}

			////遍历所有队员  加入临时好友
			//if (mpMaster->m_pFriend)
			//{
			//	for(int i=0;i<MAX_TEAM_MENBER_COUNT;i++)
			//	{
			//		if (mData.mMember[i].mDBID!=0)
			//		{
			//			mpMaster->m_pFriend->WhenFriend(mData.mMember[i].mDBID);

			//			mSendPacket.BeginWrite();
			//			mSendPacket.WriteShort(r2g_team_WhenFriend);
			//			mSendPacket.WriteLong(mSelfDBID);
			//			mSendPacket.WriteLong(mData.mMember[i].mDBID);
			//			SendToGameWorld();
			//		}		
			//	}
			//}

			pPacket->WriteByte(R2C_ADD_TEAM_MEMBER);
			pPacket->WriteLong(MemberDBID);
			pPacket->WriteString(pMemberName);
			pPacket->WriteByte(MemberHead);
			pPacket->WriteByte(Metier);
			pPacket->WriteLong(BornTag);
			pPacket->WriteLong(actorID);

		}
		break;
	case GW2R_DELETE_TEAM_MEMBER:		//减少一个队友
		//LOG("GW2R_DELETE_TEAM_MEMBER\n");
		{
			long MemberDBID;
			cmd->ReadLong(&MemberDBID);

			if(MemberDBID==mSelfDBID)
			{
				mData.Clear();
			}
			else
			{
				mData.DeleteMember(MemberDBID); //region数据更新
			}
			

			pPacket->WriteByte(R2C_DELETE_TEAM_MEMBER);
			pPacket->WriteLong(MemberDBID);
		}
		break;
	case GW2R_UPDATE_ONE_MEMBER_DATA:	//刷新某个成员的数据
		//LOG("GW2R_UPDATE_ONE_MEMBER_DATA\n");
		{
			long MemberDBID;
			long Hp;
			long MaxHp;
			long Mp;
			long MaxMp;
			char Level;
			long X;
			long Y;
			char* MapName;
			long mCombatPower;


			cmd->ReadLong(&MemberDBID);
			cmd->ReadLong(&Hp);
			cmd->ReadLong(&MaxHp);
			cmd->ReadLong(&Mp);
			cmd->ReadLong(&MaxMp);
			cmd->ReadByte(&Level);
			cmd->ReadLong(&X);
			cmd->ReadLong(&Y);
			cmd->ReadString(&MapName);
			cmd->ReadLong(&mCombatPower);



			//region数据更新
            int i;
			for(i=mData.Begin();i!=mData.End();++i)
			{
				if(mData[i].mDBID==MemberDBID)
				{
					break;
				}
			}
			if(i!=mData.End())
			{
				mData[i].mHp=Hp;
				mData[i].mMaxHp=MaxHp;
				mData[i].mMp=Mp;
				mData[i].mMaxMp=MaxMp;
				mData[i].mLevel=Level;
				mData[i].X=X;
				mData[i].Y=Y;
				mData[i].mRegion=MapName;
				mData[i].mCombatPower=mCombatPower;
			}

			pPacket->WriteByte(R2C_UPDATE_ONE_MEMBER_DATA);
			pPacket->WriteLong(MemberDBID);
			pPacket->WriteLong(Hp);
			pPacket->WriteLong(MaxHp);
			pPacket->WriteLong(Mp);
			pPacket->WriteLong(MaxMp);
			pPacket->WriteByte(Level);
			pPacket->WriteLong(X);
			pPacket->WriteLong(Y);
			pPacket->WriteString(MapName); //地图名字
			pPacket->WriteLong(mCombatPower); //战力评估
		}
		break;
	case GW2R_REQUEST_MEMBER_DATA:		//要求自己的数据到GameWorld
		//LOG("GW2R_REQUEST_MEMBER_DATA\n");
		{
			string MapName = "";
			if (mpMaster->m_scene)
			{
				RtsSceneBlockMap* pMap = mpMaster->m_scene->FindBlockByPos(mpMaster->m_pos);
				if (pMap)
				{
					MapName = pMap->m_szFileName;
				}
			}
			//发送消息到GameWorld
			CG_CmdPacket* pPacket= GameWorldPacket();
			pPacket->WriteByte(R2GW_DATE_CHANGE);
			pPacket->WriteLong(mpMaster->m_core.GetHp());
			pPacket->WriteLong(mpMaster->m_core.GetMaxHp());
			pPacket->WriteLong(mpMaster->m_core.GetMp());
			pPacket->WriteLong(mpMaster->m_core.GetMaxMp());
			pPacket->WriteByte(mpMaster->m_core.Lev);
			pPacket->WriteLong(mpMaster->m_pos[0]);
			pPacket->WriteLong(mpMaster->m_pos[1]);
			pPacket->WriteString((char*)MapName.c_str());
			pPacket->WriteLong(mpMaster->m_core.WarScores);
			SendToGameWorld();
		}
		break;
	case GW2R_UPDATE_ALL_MEMBER_DATA:	//刷新所有成员的数据
		{
			char* pTeamData;
			cmd->ReadString(&pTeamData);
			string TeamData=pTeamData;
			mData.Clear();
			TeamData>>mData;

			//向客户端发送消息
			pPacket->WriteByte(R2C_UPDATE_ALL_MEMBER_DATA);
			pPacket->WriteString(pTeamData);
		}
		break;

	case GW2R_TEAM_MESSAGE:				//刷一个消息
		//LOG("GW2R_TEAM_MESSAGE\n");
		{
			long MessageID;
			cmd->ReadLong(&MessageID);

			pPacket->WriteByte(R2C_TEAM_MESSAGE);
			pPacket->WriteLong(MessageID);
		}
		break;
	case GW2R_TEAM_MESSAGE_WITH_ID:		//刷一个消息[带ID].
		//LOG("GW2R_TEAM_MESSAGE_WITH_ID\n");
		{
			long DBIDInMessage;
			long MessageID;

			cmd->ReadLong(&DBIDInMessage);
			cmd->ReadLong(&MessageID);

			pPacket->WriteByte(R2C_TEAM_MESSAGE_WITH_ID);
			pPacket->WriteLong(DBIDInMessage);
			pPacket->WriteLong(MessageID);
		}
		break;
	case GW2R_OTHER_AGREE_INVITE:		//别人同意加入队伍
		//LOG("GW2R_OTHER_AGREE_INVITE\n");
		{
			long OtherDBID;
			char* pOtherName;
			char OtherHead;
			char metier;
			long BornTag;
			long actorID;

			cmd->ReadLong(&OtherDBID);
			cmd->ReadString(&pOtherName);
			cmd->ReadByte(&OtherHead);
			cmd->ReadByte(&metier);
			cmd->ReadLong(&BornTag);
			cmd->ReadLong(&actorID);

			//region数据更新
			if(mData.Empty())
			{//队伍空
				mData.AddMember(mSelfDBID,mSelfName,mSelfHead,metier,BornTag,actorID);//加自己
			}
			string OtherName(pOtherName);
			mData.AddMember(OtherDBID,OtherName,OtherHead,metier,BornTag,actorID);//加新队友



			pPacket->WriteByte(R2C_OTHER_AGREE_INVITE);
			pPacket->WriteLong(OtherDBID);
			pPacket->WriteString(pOtherName);
			pPacket->WriteByte(OtherHead);
			pPacket->WriteByte(metier); //职业
			pPacket->WriteLong(BornTag); //生肖
			pPacket->WriteLong(actorID); //actorID
			mpMaster->GetPlayerLogic()->RemoveFromInviteMap(OtherDBID);
			//mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
		}
		break;
	case GW2R_OTHER_REFUSE_INVITE:		//别人拒绝加入队伍
		//LOG("GW2R_OTHER_REFUSE_INVITE\n");
		{
			long OtherDBID;
			char* pOtherName;

			cmd->ReadLong(&OtherDBID);
			cmd->ReadString(&pOtherName);

			pPacket->WriteByte(R2C_OTHER_REFUSE_INVITE);
			pPacket->WriteLong(OtherDBID);
			pPacket->WriteString(pOtherName);
			//mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
	
		}
		break;
	case GW2R_TO_WHENFRIEND://自己加临时好友
		{
			if (!(mpMaster->m_pFriend))
			{
				return CMD_ERROR_UNKNOWN;
			}
			for (int i = 0; i <MAX_TEAM_MENBER_COUNT; i++)
			{
				if (mpMaster->m_userInfo)
				{
					if (mData.mMember[i].mDBID == mpMaster->m_userInfo->m_userId)
					{
						continue;
					}
				}

				mpMaster->m_pFriend->WhenFriend(mData.mMember[i].mDBID);
			}
			
			return CMD_ERROR_UNKNOWN;
		}
		break;
	case GW2R_OTHER_INVITE:				//别人邀请自己加入队伍
		//LOG("GW2R_OTHER_INVITE\n");
		{


			long OtherDBID;
			char* pOtherName;

			cmd->ReadLong(&OtherDBID);
			cmd->ReadString(&pOtherName);

			if (mpMaster->m_bRefuseTeam)
			{
				//发送消息给邀请自己的人  自己设置了拒绝组队
				CG_CmdPacket& rPacket = *(GameWorldPacket());
				rPacket.WriteByte(R2GW_SETREFUSE_TEAM);
				rPacket.WriteLong(OtherDBID);
				SendToGameWorld();

				return CMD_ERROR_NONE;
			}

			pPacket->WriteByte(R2C_OTHER_INVITE);
			pPacket->WriteLong(OtherDBID);
			pPacket->WriteString(pOtherName);

		}
		break;
	case GW2R_OTHER_BANISH:				//别人驱逐自己出队伍
		//LOG("GW2R_OTHER_BANISH\n");
		{
			long OtherDBID;
			cmd->ReadLong(&OtherDBID);

			mData.Clear();//region数据更新

			pPacket->WriteByte(R2C_OTHER_BANISH);
			pPacket->WriteLong(OtherDBID);
			mpMaster->GetPlayerLogic()->RemoveFromInviteMap(OtherDBID);
		}
		break;
	case GW2R_OTHER_LEAVE:				//别人离开队伍
		//LOG("GW2R_OTHER_LEAVE\n");
		{
			long OtherDBID;
			cmd->ReadLong(&OtherDBID);

			mData.DeleteMember(OtherDBID);

			pPacket->WriteByte(R2C_OTHER_LEAVE);
			pPacket->WriteLong(OtherDBID);

			SendToClient();
			

			mpMaster->GetPlayerLogic()->RemoveFromInviteMap(OtherDBID);
			
			if (mpMaster->m_userInfo)  
			{
				RefuseTeam(mpMaster->m_userInfo->m_userId);
			}


			return CMD_ERROR_NONE;
		
		}
		break;
	case GW2R_OTHER_DISMISS:			//别人解散队伍
		//LOG("GW2R_OTHER_DISMISS\n");
		{
			long OtherDBID;
			cmd->ReadLong(&OtherDBID);

			mData.Clear();//region数据更新

			pPacket->WriteByte(R2C_OTHER_DISMISS);
			pPacket->WriteLong(OtherDBID);
			mpMaster->GetPlayerLogic()->RemoveFromInviteMap(OtherDBID);
		}
		break;
	case GW2R_APPLY_ADDTEAM:
		{
			long dwOtherID;
			char* pOthername;
			long  dwLevel;
			short metier;
			long  dw;
			char* unName = NULL;
			long  BornTag;
			long  WarScores;
			cmd->ReadLong(&dwOtherID);
			cmd->ReadString(&pOthername);
			cmd->ReadLong(&dwLevel);
			cmd->ReadShort(&metier);
			cmd->ReadLong(&dw);
			cmd->ReadLong(&BornTag);
			cmd->ReadLong(&WarScores);

			pPacket->WriteByte(R2C_OTHER_APPLY);
			pPacket->WriteLong(dwOtherID);
			pPacket->WriteString(pOthername);
			pPacket->WriteLong(dwLevel);
			pPacket->WriteShort(metier);
			/*pPacket->WriteLong(dw);*/
			pPacket->WriteLong(BornTag);  //生肖
			pPacket->WriteLong(WarScores); //战力评估

			//if(0xffffffff != dw)
			//{
			//	cmd->ReadString(&unName);
			//	pPacket->WriteString(unName);
			//}
		}
		break;
	case GW2R_REFUSE_TEAM:
		{
			long dwOtherID;
			char* pOthername;
			
			cmd->ReadLong(&dwOtherID);
			cmd->ReadString(&pOthername);
			
			pPacket->WriteByte(R2C_OTHER_REFUSE_APPLY);
			pPacket->WriteLong(dwOtherID);
			pPacket->WriteString(pOthername);
			
		}
		break;
	case GW2R_TRANSFER_CAPTAIN:
		{
			long dwOtherID;
			long nNew;
			cmd->ReadLong(&dwOtherID);
			cmd->ReadLong(&nNew);
			pPacket->WriteByte(R2C_TRANSFER_CAPTAIN);
			pPacket->WriteLong(dwOtherID);
			if (-1 < nNew && nNew < mData.Size())
			{
				STeamData Temp;
				Temp = mData[nNew];
				mData[nNew] = mData[0];
				mData[0] = Temp;
			}
			
		}
		break;
	case GW2R_SET_PICK_TYPE:
		{
			char cType;
			cmd->ReadByte(&cType);
			mData.SetPickType((TeamPickType)(cType));
			pPacket->WriteByte(R2C_SET_PICK_TYPE);
			pPacket->WriteByte(cType);
		}
		break;
	case GW2R_TEAM_PICKER:
		{
			char* pMsg = NULL;
			cmd->ReadString(&pMsg);
			mpMaster->SendSystemMessage(pMsg);
			return CMD_ERROR_NONE;
		}
		break;
	case GW2R_SETREFUSE_TEAM:
		{
			rt2_sprintf(g_strStaticBuffer, R(MSG_TEAM_OTHERSETREFUSE));
			mpMaster->SendSystemMessage(g_strStaticBuffer);
			return CMD_ERROR_UNKNOWN;
		}
		break;
	default:
		return CMD_ERROR_UNKNOWN;
	}
	SendToClient();
	return CMD_ERROR_NONE;
}

void CServerTeam::SendTeamPickerMsgToGWS(const char* pMsg)
{
	CG_CmdPacket& rPacket = *(GameWorldPacket());
	rPacket.WriteByte(R2GW_TEAM_PICKER);
	rPacket.WriteString(pMsg);				
	SendToGameWorld();
}

void CServerTeam::StartOrderPick()
{
	m_nPickerIncex = 0;
}

void CServerTeam::NextPicker()
{
	int nNum = mData.Size();
	m_nPickerIncex = (m_nPickerIncex + 1) % nNum;
}

unsigned long CServerTeam::GetOrderPickDBID()
{
	unsigned long dwRet = 0xffffffff;
	do 
	{
		int nNum = mData.Size();
		if (m_nPickerIncex >= nNum)
		{
			m_nPickerIncex = nNum - 1;
		}
		dwRet = mData[m_nPickerIncex].mDBID;
	} while (false);
	return dwRet;
}














/*
//------自己----------
//自己邀请别人加入队伍
void CServerTeam::InviteOther(DWORD vOtherRegionID)
{
	if(vOtherRegionID==mSelfRegionID)
	{//如果是自己邀请自己，不处理
		return ;
	}

		
	if(mData.Empty())
	{//如果当前没有队伍，可以发出邀请//调用别人邀请自己的服务器端
		SendToServer(R2R_SELF_INVITE,vOtherRegionID);
	}
	else
	{//如果当前已经有队伍了
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			if(mData.Size()<MAX_TEAM_MENBER_COUNT)
			{//人数不满，//调用别人邀请自己的服务器端
				SendToServer(R2R_SELF_INVITE,vOtherRegionID);
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
*/

/*
//自己驱逐别人从队伍中[用RegionID]
void CServerTeam::BanishOtherByRegionID(DWORD vOtherRegionID)
{
	CRegionCreature* pCreature=FindByRegionID(vOtherRegionID);//!!!!
	
	if(pCreature==NULL)
	{
		return;
	}

	if(pCreature->m_userInfo==NULL)
	{
		SendToClient(R2C_TEAM_MESSAGE,THAT_OBJECT_NOT_PLAYER);
		return ;
	}


	DWORD vOtherDBID=pCreature->m_userInfo->m_userId;

	if(vOtherDBID==mSelfDBID)
	{//如果是自己调用自己离开队伍
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//如果没有，不处理
	}
	else
	{//如果有队伍
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			if(mData.Exist(vOtherDBID))
			{//有该队员，调用别人驱逐自己的服务器端，同时改变当前数据
				for(int i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==vOtherDBID)
					{//是被驱逐的那个人，发驱逐消息
						SendToServer(R2R_SELF_BANISH,vOtherDBID);
					}
					else if(mData[i].mDBID==mSelfDBID)
					{//是自己
						SendToClient(R2C_TEAM_MESSAGE_WITH_ID,BANISH_MEMBER,NULL,vOtherDBID);
						SendToClient(R2C_DELETE_TEAM_MEMBER,vOtherDBID);
					}
					else
					{//是其它队友，发减少一个人的消息，并发送相关信息
						SendToServer(R2R_TEAM_MESSAGE_WITH_ID,mData[i].mDBID,BANISH_MEMBER,NULL,vOtherDBID);
						SendToServer(R2R_DELETE_TEAM_MEMBER,mData[i].mDBID,vOtherDBID);
					}
				}
				mData.DeleteMember(vOtherDBID);

				if(mData.Size()==1)
				{//人数不足，自动解散队伍
					SendToClient(R2C_TEAM_MESSAGE,NOT_MEMBER_TEAM_DISMISS);
					SendToClient(R2C_DELETE_TEAM_MEMBER,mSelfDBID);
					mData.Clear();
				}
			}
			else
			{//没有该队员，告之客户端
				SendToClient(R2C_TEAM_MESSAGE,MEMBER_NOT_EXIST);
			}
		}
		else
		{//不是队长，不处理
		}
	}
	return;
}
*/



/*
//自己驱逐别人从队伍中
void CServerTeam::BanishOther(DWORD vOtherDBID)
{
	if(vOtherDBID==mSelfDBID)
	{//如果是自己调用自己离开队伍
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//如果没有，不处理
	}
	else
	{//如果有队伍
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			if(mData.Exist(vOtherDBID))
			{//有该队员，调用别人驱逐自己的服务器端，同时改变当前数据
				for(int i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==vOtherDBID)
					{//是被驱逐的那个人，发驱逐消息
						SendToServer(R2R_SELF_BANISH,vOtherDBID);
					}
					else if(mData[i].mDBID==mSelfDBID)
					{//是自己不处理
					}
					else
					{//是其它队友，发减少一个人的消息，并发送相关信息
						SendToServer(R2R_TEAM_MESSAGE_WITH_ID,mData[i].mDBID,BANISH_MEMBER,NULL,vOtherDBID);
						SendToServer(R2R_DELETE_TEAM_MEMBER,mData[i].mDBID,vOtherDBID);
					}
				}
				mData.DeleteMember(vOtherDBID);

				if(mData.Size()==1)
				{//人数不足，自动解散队伍
					mData.Clear();
				}
			}
			else
			{//没有该队员，不处理
			}
		}
		else
		{//不是队长，不处理
		}
	}
	return;
}
*/




/*
//自己离开队伍
void CServerTeam::SelfLeave()
{
	if(mData.Empty())
	{//如果没有，不处理
	}
	else
	{//如果有，

		//调用队伍中其它人的别人离开队伍[除了自己]
		for(int i=mData.Begin(); i!=mData.End();i++)
		{
			if(mData[i].mDBID !=mSelfDBID)
			{//除了自己
				SendToServer(R2R_SELF_LEAVE,mData[i].mDBID);
			}
		}

		//改变当前数据
		mData.Clear();
	}
	return;
}
*/




/*
//自己解散队伍
void CServerTeam::Dismiss()
{
	if(mData.Empty())
	{//如果没有队伍，不处理
	}
	else
	{//如果有
		if(mData.GetCaptain()==mSelfDBID)
		{//是队长
			//调用队伍中其它人的别人解散队伍[除了自己]
			for(int i=mData.Begin(); i!=mData.End();i++)
			{
				if(mData[i].mDBID !=mSelfDBID)
				{//除了自己
					SendToServer(R2R_OTHER_DISMISS,mData[i].mDBID);
				}
			}

			//同时改变当前数据
			mData.Clear();
		}
		else
		{//不是队长，不处理
		}
	}
	return;
}
*/



/*
//自己同意别人的邀请
void CServerTeam::AgreeOtherInvite(DWORD vOtherDBID)
{
	if(mServerWaitOtherAddSelf)
	{//已经同意了另一个队伍，正在等待的状态，所以自动拒绝这个队伍
		RefuseOtherInvite(vOtherDBID);
		return ;
	}

	if(mData.Empty())
	{//没有队伍，调用别人的服务器端处理函数，同时设置等待标识
		mServerWaitOtherAddSelf=true;
		SendToServer(R2R_SELF_AGREE_INVITE,vOtherDBID);
	}
	else
	{//已经有队伍了，自动拒绝别人的邀请
		RefuseOtherInvite(vOtherDBID);
	}
	return;
}
*/


/*
//自己拒绝别人的邀请
void CServerTeam::RefuseOtherInvite(DWORD vOtherDBID)
{
	//调用别人的服务器端处理函数
	SendToServer(R2R_SELF_REFUSE_INVITE,vOtherDBID);
}
*/


/*
//------服务器端特有------
//自己断线
void CServerTeam::OnDisconnect()
{
	//调用服务器端离开队伍
	SelfLeave();
}
//自己下线
void CServerTeam::OnOffline()
{
	//调用服务器端离开队伍
	SelfLeave();
}

//自己的对队伍有影响的数值变化
void CServerTeam::DateChangeAffectTeam()
{
	int Index=mData.Find(mSelfDBID);
	if(Index==mData.End())
	{
		return;
	}


	if(mData[Index].mHp==mpMaster->m_core.GetHp() 
		&& mData[Index].mMp==mpMaster->m_core.GetMp()
		&& mData[Index].mMaxHp==mpMaster->m_core.GetMaxHp()
		&& mData[Index].mMaxMp==mpMaster->m_core.GetMaxMp())
	{
		return;
	}

	mData[Index].mHp=mpMaster->m_core.GetHp();
	mData[Index].mMp=mpMaster->m_core.GetMp();
	mData[Index].mMaxHp=mpMaster->m_core.GetMaxHp();
	mData[Index].mMaxMp=mpMaster->m_core.GetMaxMp();


	//发送消息给所有的队员的服务器端[除了自己]
	for (int i=mData.Begin();i!=mData.End();i++)
	{
		if(mData[i].mDBID!=mSelfDBID)
		{
			SendToServer(R2R_UPDATE_ONE_MEMBER_DATA,mData[i].mDBID,mSelfDBID);
		}
	}
	return;
}

void CServerTeam::GetDataFromMaster()
{
	int Index=mData.Find(mSelfDBID);
	if(Index==mData.End())
	{
		return;
	}

	mData[Index].mHp=mpMaster->m_core.GetHp();
	mData[Index].mMp=mpMaster->m_core.GetMp();
	mData[Index].mMaxHp=mpMaster->m_core.GetMaxHp();
	mData[Index].mMaxMp=mpMaster->m_core.GetMaxMp();
}



//别人将你成功加入队伍
void CServerTeam::OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName)
{
	//改变等待标识
	mServerWaitOtherAddSelf=false;
	//改变自身数据
	mData.AddMember(vOtherDBID,vrOtherName);
	mData.AddMember(mSelfDBID,mSelfName);
	GetDataFromMaster();
	SendToClient(R2C_ADD_TEAM_MEMBER,vOtherDBID,&vrOtherName);
	SendToClient(R2C_ADD_TEAM_MEMBER,mSelfDBID,&mSelfName);
	SendToClient(R2C_TEAM_MESSAGE_WITH_ID,ADD_TEAM_SUCCEED,NULL,vOtherDBID);
}

//别人将你加入失败
void CServerTeam::OtherAddSelfFault(DWORD vOtherDBID)
{
	//改变等待标识
	mServerWaitOtherAddSelf=false;
}

//别人不存在
void CServerTeam::OtherNotExist(DWORD vOtherDBID)
{
	//改变等待标识
	mServerWaitOtherAddSelf=false;
}



//-------别人-----------
//别人同意加入队伍
void CServerTeam::OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mServerWaitOtherAddSelf)
	{//已经同意了另一个队伍，正在等待的状态//加入失败，并发信息[已经加入了别人的队伍]
		SendToServer(R2R_SELF_ADD_OTHER_FAULT,vOtherDBID);
		SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
		return;
	}

	if(mData.Empty())
	{//当前队伍为空，加入自己
		mData.AddMember(mSelfDBID,mSelfName);
		GetDataFromMaster();
	}

	if(mData.GetCaptain()==mSelfDBID) 
	{//自己是队长

		if(mData.Size()>=MAX_TEAM_MENBER_COUNT)
		{//队伍是否已经满，满则，发送消息给别人的服务端和客户端[人满]
			SendToServer(R2R_SELF_ADD_OTHER_FAULT,vOtherDBID);
			SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,TEAM_COUNT_OVER);
			return;
		}
		else
		{
			if(mData.Exist(vOtherDBID))
			{//已经在这个队伍中
				SendToServer(R2R_SELF_ADD_OTHER_FAULT,vOtherDBID);
				SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,ALREADY_IN_TEAM);
				return;
			}
			else
			{//不在这个队伍
				//更改数据，发送消息给队员[自己，其它队友]
				
				mData.AddMember(vOtherDBID,vrOtherName);
				SendToServer(R2R_SELF_ADD_OTHER_SUCCEED,vOtherDBID);//发消息给那个加入的人

				int i;
				for(i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==mSelfDBID)
					{//自己
						SendToClient(R2C_OTHER_AGREE_INVITE,vOtherDBID,&vrOtherName);
					}
					else if(mData[i].mDBID==vOtherDBID)
					{//新队友
					}
					else
					{//老队友
						SendToServer(R2R_ADD_TEAM_MEMBER,mData[i].mDBID,vOtherDBID,&vrOtherName);//给老队员加新队员的名单
						SendToServer(R2R_TEAM_MESSAGE_WITH_ID,mData[i].mDBID,MEMBER_ADD_TEAM,NULL,vOtherDBID);//告诉老队友，新人加入

						SendToServer(R2R_ADD_TEAM_MEMBER,vOtherDBID,mData[i].mDBID,&(mData[i].mName));//给新队友加老队友的名单
					}
				}

				for(i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==mSelfDBID)
					{//自己
						SendToServer(R2R_UPDATE_ONE_MEMBER_DATA,vOtherDBID,mSelfDBID);//自己的数据给新人
						SendToServer(R2R_FORCE_UPDATE_DATA,vOtherDBID,mSelfDBID);//要求新人将数据给自己
					}
					else if(mData[i].mDBID==vOtherDBID)
					{//新人
					}
					else
					{//老队友
						SendToServer(R2R_UPDATE_ONE_MEMBER_DATA,vOtherDBID,mData[i].mDBID);//将老队友的数据发给新人
						SendToServer(R2R_FORCE_UPDATE_DATA,vOtherDBID,mData[i].mDBID);//要求新人将数据发给老队友
					}
				}
			}
		}
	}
	
	return;
}

//别人拒绝加入队伍
void CServerTeam::OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	//告诉自己被拒绝
	SendToClient(R2C_OTHER_REFUSE_INVITE,vOtherDBID,&vrOtherName);
}


//别人对队伍有影响的数值变化
void CServerTeam::OtherDateChangeAffectTeam(DWORD vOtherDBID)
{
	//告诉自己的客户端
	SendToClient(R2C_UPDATE_ONE_MEMBER_DATA,vOtherDBID);
}


//别人邀请自己加入队伍
void CServerTeam::OtherInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mData.Empty())
	{//发送给自己消息
		SendToClient(R2C_OTHER_INVITE,vOtherDBID,&vrOtherName);
	}
	else
	{//已经有队伍//调用别人的服务器端函数[别人拒绝加入]
		//根据当前的邀请互斥情况，决定是否自动拒绝
		SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
		SendToServer(R2R_SELF_REFUSE_INVITE,vOtherDBID);
	}
}

//别人驱逐自己出队伍
void CServerTeam::OtherBanish(DWORD vOtherDBID)
{
	if(mData.Empty())
	{//没有队伍，不处理
		return;
	}

	if(mData.GetCaptain()==vOtherDBID)
	{//别人是队长
		//发送消息到自己，同时改变数据	
		SendToClient(R2C_OTHER_BANISH,vOtherDBID);
		mData.Clear();
	}
	else
	{//别人不是队长
		return;
	}
}

//别人离开队伍
void CServerTeam::OtherLeave(DWORD vOtherDBID)
{
	if(mData.Exist(vOtherDBID))
	{
		//发送消息到自己，同时改变数据
		SendToClient(R2C_OTHER_LEAVE,vOtherDBID);
		mData.DeleteMember(vOtherDBID);

		if(mData.Size()==1 )
		{//人数不够，自动解散
			SendToClient(R2C_DELETE_TEAM_MEMBER,mSelfDBID);
			SendToClient(R2C_TEAM_MESSAGE,NOT_MEMBER_TEAM_DISMISS);
			mData.Clear();
			return;
		}
	}
}





//别人解散队伍
void CServerTeam::OtherDismiss(DWORD vOtherDBID)
{
	if(mData.GetCaptain()==vOtherDBID)
	{//别人是队长
		//告诉用户，同时改变数据
		SendToClient(R2C_OTHER_DISMISS,vOtherDBID);
		mData.Clear();
	}
}

//------对客户端--------
//接到上传的消息
int CServerTeam::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket* cmd)
{
	char Operation;
	long OtherID;
	SAFE_READ(cmd->ReadByte(&Operation))

	//解析消息，并处理
	switch(Operation)
	{
	case C2R_INVITE_OTHER://自己邀请别人加入队伍
		SAFE_READ(cmd->ReadLong(&OtherID))
		InviteOther(OtherID);
		break;
	case C2R_BANISH_OTHER://自己驱逐别人从队伍中
		SAFE_READ(cmd->ReadLong(&OtherID))
		BanishOther(OtherID);
		break;
	case C2R_SELF_LEAVE://自己离开队伍
		SelfLeave();
		break;
	case C2R_DISMISS://自己解散队伍
		Dismiss();
		break;
	case C2R_AGREE_OTHER_INVITE://自己同意别人的邀请
		SAFE_READ(cmd->ReadLong(&OtherID))
		AgreeOtherInvite(OtherID);
		break;
	case C2R_REFUSE_OTHER_INVITE://自己拒绝别人的邀请
		SAFE_READ(cmd->ReadLong(&OtherID))
		RefuseOtherInvite(OtherID);
		break;
	case C2R_BANISH_OTHER_BY_REGION_ID:
		SAFE_READ(cmd->ReadLong(&OtherID))
		BanishOtherByRegionID(OtherID);
		break;
	default:
		return CMD_ERROR_UNKNOWN;
		break;
	}
	return CMD_ERROR_NONE;
} 



//发送消息到客户端
void CServerTeam::SendToClient(byte vOperation,DWORD vParameter,std::string* vpName,DWORD vDBIDInMessage)
{
	g_sendCmd->BeginWrite();
	
	g_sendCmd->WriteShort(r2c_subsystem_team);
	g_sendCmd->WriteByte(vOperation);
	switch(vOperation)
	{
	case R2C_ADD_TEAM_MEMBER:			//增加一个队友
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_DELETE_TEAM_MEMBER:		//减少一个队友
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_UPDATE_ONE_MEMBER_DATA:	//刷新某个成员的数据
		{
			g_sendCmd->WriteLong(vParameter);
			
			int Index=mData.Find(vParameter);

			if(Index==mData.End())
			{//没有找到
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
			}
			else
			{//找到
				g_sendCmd->WriteLong(mData[Index].mHp);
				g_sendCmd->WriteLong(mData[Index].mMaxHp);
				g_sendCmd->WriteLong(mData[Index].mMp);
				g_sendCmd->WriteLong(mData[Index].mMaxMp);
			}
		}
		break;
	case R2C_UPDATE_ALL_MEMBER_DATA:		//刷新所有成员的数据
		{
			g_sendCmd->WriteByte(mData.Size());
			for(int i=mData.Begin();i!=mData.End();i++)
			{
				g_sendCmd->WriteLong(mData[i].mDBID);	//mDBID;
				g_sendCmd->WriteLong(mData[i].mHp);		//mHp;
				g_sendCmd->WriteLong(mData[i].mMaxHp);	//mMaxHp;

				g_sendCmd->WriteLong(mData[i].mMp);		//mMp;
				g_sendCmd->WriteLong(mData[i].mMaxMp);	//mMaxMp;
				g_sendCmd->WriteString((char*)mData[i].mName.c_str());
				
			}
		}
		break;
	case R2C_TEAM_MESSAGE:				//刷一个消息
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_TEAM_MESSAGE_WITH_ID:
		g_sendCmd->WriteLong(vDBIDInMessage);
		g_sendCmd->WriteLong(vParameter);
		break;
		//-------别人-----------
	case R2C_OTHER_AGREE_INVITE:		//别人同意加入队伍
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_OTHER_REFUSE_INVITE:		//别人拒绝加入队伍
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_OTHER_INVITE:				//别人邀请自己加入队伍
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*)(vpName->c_str()));
		break;
	case R2C_OTHER_BANISH:				//别人驱逐自己出队伍
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_OTHER_LEAVE:				//别人离开队伍
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_OTHER_DISMISS:				//别人解散队伍
		g_sendCmd->WriteLong(vParameter);
		break;
	default:
		break;
	}
	mpMaster->RecvCmd(g_sendCmd);
}







//-----------服务器间---------------

/*
//------对服务端--------
//发送消息到服务端
struct SR2rData
{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;
									mParameter=0;mHp=0;mMaxHp=0;mMp=0;mMaxMp=0;mpName=NULL;}
	byte	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	int		mHp;
	int		mMaxHp;
	int		mMp;
	int		mMaxMp;
	char*	mpName;
};
*//*

void CServerTeam::SendToServer(byte vOperation,DWORD vSendToID,DWORD vParameter,std::string* vpName,DWORD vDBIDInMessage)
{
	SR2rData R2rSendCmd;
	R2rSendCmd.BeginSend();
	
	DWORD	SendToID;
	
	SendToID=vSendToID;


	R2rSendCmd.mOperation=vOperation;
	R2rSendCmd.mFromDBID=mSelfDBID;

	switch(vOperation)
	{
	case R2R_ADD_TEAM_MEMBER:			//增加一个队友
		R2rSendCmd.mParameter=vParameter;
		R2rSendCmd.mpName=(char*)vpName->c_str();
		break;
	case R2R_DELETE_TEAM_MEMBER:		//减少一个队友
		R2rSendCmd.mParameter=vParameter;
		break;
	case R2R_UPDATE_ONE_MEMBER_DATA:	//刷新某个成员的数据
		{
			R2rSendCmd.mParameter=vParameter;
			int Index=mData.Find(vParameter);
			if(Index==mData.End())
			{//没有找到
				R2rSendCmd.mHp=0;
				R2rSendCmd.mMaxHp=0;
				R2rSendCmd.mMp=0;
				R2rSendCmd.mMaxMp=0;
			}
			else
			{//找到
				R2rSendCmd.mHp=mData[Index].mHp;
				R2rSendCmd.mMaxHp=mData[Index].mMaxHp;
				R2rSendCmd.mMp=mData[Index].mMp;
				R2rSendCmd.mMaxMp=mData[Index].mMaxMp;
			}
		}
		break;
	case R2R_TEAM_MESSAGE:				//刷一个消息
		R2rSendCmd.mParameter=vParameter;
		break;
	case R2R_TEAM_MESSAGE_WITH_ID:
		R2rSendCmd.mDBIDInMessage=vDBIDInMessage;
		R2rSendCmd.mParameter=vParameter;
		break;
	case R2R_SELF_ADD_OTHER_SUCCEED:	//自己将别人加入到自己队伍成功
		R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
		break;
	case R2R_SELF_ADD_OTHER_FAULT:		//自己将别人加入到自己队伍失败
		break;
	case R2R_SELF_AGREE_INVITE:			//自己同意加入别人的队伍
		R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
		break;
	case R2R_SELF_REFUSE_INVITE:		//自己拒绝加入别人的队伍
		R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
		break;
	case R2R_SELF_INVITE:				//自己邀请别人加入队伍
		{
			//这里是用RegionID
			CRegionCreature* pCreature =mpMaster->m_scene->FindCreature(SendToID);
			if(pCreature==NULL)
			{//该物体找不到
				R2rSendCmd.BeginSend();
				R2rSendCmd.mOperation=R2R_OTHER_REGION_ID_NOT_EXIST;
				R2rSendCmd.mFromDBID=0;
				R2rSendCmd.mParameter=SendToID;
				ReceiveFromServer(R2rSendCmd);
			}
			else
			{//找到该物体
				if(pCreature->m_pTeam==NULL)
				{//不是玩家无法组队
					SendToClient(R2C_TEAM_MESSAGE,THAT_OBJECT_NOT_PLAYER);
				}
				else
				{
					R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
					pCreature->m_pTeam->ReceiveFromServer(R2rSendCmd);
				}
			}
			return;
		}
		break;
	case R2R_SELF_BANISH:				//自己驱逐别人出队伍
		break;
	case R2R_SELF_LEAVE:
		break;
	case R2R_SELF_DISMISS:				//自己解散队伍
		break;
	case R2R_FORCE_UPDATE_DATA:
		R2rSendCmd.mParameter=vParameter;
		break;
	default:
		break;
	}


	//发送到game_word
	g_region->m_gws.SendTeam(SendToID,R2rSendCmd);


	//如果无法找到
	//发消息：
	
	*//*
	R2rSendCmd.BeginSend();
	R2rSendCmd.mOperation=R2R_OTHER_DB_ID_NOT_EXIST;
	R2rSendCmd.mFromDBID=SendToID;//找不到的那个DBID
	R2rSendCmd.mParameter=SendToID;//找不到的那个DBID
	ReceiveFromServer();
	*/




	/*
	CRegionUser* pUser=g_region-> FindUser(SendToID);
	if(pUser==NULL)
	{//该用户找不到
		R2rSendCmd.BeginSend();
		R2rSendCmd.mOperation=R2R_OTHER_DB_ID_NOT_EXIST;
		R2rSendCmd.mFromDBID=SendToID;
		R2rSendCmd.mParameter=SendToID;
		ReceiveFromServer();
	}
	else
	{
		pUser->m_dummy->m_pTeam->ReceiveFromServer();
	}
	*//*
	return;
}
*/

/*

//接到服务端的消息
void CServerTeam::ReceiveFromServer(SR2rData&  vrR2rSendCmd)
{
	string TempString;
	
	byte	Operation;
	DWORD	FromDBID;

	DWORD	OtherDBID;
	int		Hp;
	int		MaxHp;
	int		Mp;
	int		MaxMp;

	Operation=vrR2rSendCmd.mOperation;
	FromDBID=vrR2rSendCmd.mFromDBID;
	
	

	switch(Operation)
	{
	case R2R_ADD_TEAM_MEMBER://增加一个队友
		{
			char*	pName;

			OtherDBID=vrR2rSendCmd.mParameter;
			pName=vrR2rSendCmd.mpName;
			string TempString=string(pName);
			mData.AddMember(OtherDBID,TempString);
			//告诉客户端
			SendToClient(R2C_ADD_TEAM_MEMBER,OtherDBID,&string(pName));
		}
		break;
	case R2R_DELETE_TEAM_MEMBER://减少一个队友
		OtherDBID=vrR2rSendCmd.mParameter;
		mData.DeleteMember(OtherDBID);
		//告诉客户端
		SendToClient(R2C_DELETE_TEAM_MEMBER,OtherDBID);
		break;
	case R2R_UPDATE_ONE_MEMBER_DATA://刷新某个成员的数据
		{
			OtherDBID=vrR2rSendCmd.mParameter;
			int Index=mData.Find(OtherDBID);
			if(Index==mData.End())
			{//没有找到
				Hp=vrR2rSendCmd.mHp;
				MaxHp=vrR2rSendCmd.mMaxHp;
				Mp=vrR2rSendCmd.mMp;
				MaxMp=vrR2rSendCmd.mMaxMp;

			}
			else
			{//找到
				Hp=vrR2rSendCmd.mHp;
				MaxHp=vrR2rSendCmd.mMaxHp;
				Mp=vrR2rSendCmd.mMp;
				MaxMp=vrR2rSendCmd.mMaxMp;
				
				mData[Index].mHp=Hp;
				mData[Index].mMaxHp=MaxHp;
				mData[Index].mMp=Mp;
				mData[Index].mMaxMp=Mp;
			}
			
			OtherDateChangeAffectTeam(OtherDBID);
		}
		break;
	case R2R_TEAM_MESSAGE://刷一个消息
		{
			DWORD Parameter=vrR2rSendCmd.mParameter;
			SendToClient(R2C_TEAM_MESSAGE,Parameter);
		}
		break;
	case R2R_TEAM_MESSAGE_WITH_ID://刷一个消息
		{
			DWORD DBIDInMessage=vrR2rSendCmd.mDBIDInMessage;
			DWORD Parameter=vrR2rSendCmd.mParameter;
			SendToClient(R2C_TEAM_MESSAGE_WITH_ID,Parameter,NULL,DBIDInMessage);
		}
		break;


	case R2R_OTHER_ADD_SELF_SUCCEED:	//别人将你成功加入队伍
		{
			char*	pFromName;
			pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherAddSelfSucceed(FromDBID,TempString);
		}
		
		break;
	case R2R_OTHER_ADD_SELF_FAULT:		//别人将你加入失败
		OtherAddSelfFault(FromDBID);
		break;
	case R2R_OTHER_REGION_ID_NOT_EXIST:			//别人不存在
		OtherNotExist(FromDBID);
		SendToClient(R2C_TEAM_MESSAGE,THAT_PLAYER_NOT_INLINE);
		break;
	case R2R_OTHER_DB_ID_NOT_EXIST:				//别人不存在
		OtherNotExist(FromDBID);
		SendToClient(R2C_TEAM_MESSAGE,THAT_PLAYER_NOT_INLINE);
		break;
	case R2R_OTHER_AGREE_INVITE:		//别人同意加入队伍
		{
			char*	pFromName;
			pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherAgreeInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_REFUSE_INVITE:		//别人拒绝加入队伍
		{
			char* pFromName;
			pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherRefuseInvite(FromDBID,TempString);
		}

		break;
	case R2R_OTHER_INVITE:				//别人邀请自己加入队伍
		{
			char* pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherInvite(FromDBID,TempString);
		}

		break;
	case R2R_OTHER_BANISH:				//别人驱逐自己出队伍
		OtherBanish(FromDBID);
		break;
	case R2R_OTHER_LEAVE:
		OtherLeave(FromDBID);
		break;
	case R2R_OTHER_DISMISS:				//别人解散队伍
		OtherDismiss(FromDBID);
		break;
	case R2R_FORCE_UPDATE_DATA:
		OtherDBID=vrR2rSendCmd.mParameter;
		SendToServer(R2R_UPDATE_ONE_MEMBER_DATA,OtherDBID,mSelfDBID);
		break;
	default:
		break;
	}
}
*/
