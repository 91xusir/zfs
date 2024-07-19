
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

void CServerTeam::SendToGameWorld()			//��һ������GameWorld
{
	g_region->m_gws.SendPacket(&mSendPacket);
}

CRegionCreature* CServerTeam::FindByRegionID(DWORD vRegionID)
{
	//��������RegionID
	CRegionCreature* pCreature = mpMaster->m_scene->FindCreature(vRegionID);

	if(pCreature==NULL)
	{
		//��һ����Ϣ���ͻ���
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
			//������Ϣ��GameWorld
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
	{//�����ڸ����
		return 0;
	}

	if(pCreature->m_userInfo==NULL)
	{//��������޷����
		//������Ϣ���ͻ���
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

//------�Լ�----------
//�Լ�������˼������
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
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_INVITE_OTHER);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();
	mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
	return;
}

//�Լ�ͨ�������������
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
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_INVITE_OTHER);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();
	mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
	return;
}


//�Լ�������˴Ӷ�����[��RegionID]
void CServerTeam::BanishOtherByRegionID(DWORD vOtherRegionID)
{
	DWORD OtherDBID=RegionID2DBID(vOtherRegionID);
	if(OtherDBID==0){return;}
	BanishOther(OtherDBID);
	return;
}

//�Լ�������˴Ӷ�����
void CServerTeam::BanishOther(DWORD vOtherDBID)
{

	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_BANISH_OTHER);
	pPacket->WriteLong(vOtherDBID);
	SendToGameWorld();

	return;
}

//�Լ��뿪����
void CServerTeam::SelfLeave()
{
	mData.Clear();//�������
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_SELF_LEAVE);
	SendToGameWorld();
	return;
}


//�Լ���ɢ����
void CServerTeam::Dismiss()
{
	mData.Clear();//�������
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_DISMISS);
	SendToGameWorld();
	return;
}


//�Լ�ͬ����˵�����
void CServerTeam::AgreeOtherInvite(DWORD vOtherDBID)
{
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_AGREE_OTHER_INVITE);
	pPacket->WriteLong(vOtherDBID);
	SendToGameWorld();
	return;
}

//�Լ��ܾ����˵�����
void CServerTeam::RefuseOtherInvite(DWORD vOtherDBID)
{
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_REFUSE_OTHER_INVITE);
	pPacket->WriteLong(vOtherDBID);
	SendToGameWorld();
}




//�Լ��ĶԶ�����Ӱ�����ֵ�仯
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
	//mData[Index].mName = m_core.Name; //�������
	//mData[Index].mSchool = m_core.Metier; //ְҵ
	mData[Index].mRegion = MapName; //��ͼ����
	mData[Index].mCombatPower = mpMaster->m_core.WarScores; //ս������
	//mData[Index].mAnimal = m_core.BornTag; //��Ф
	//mData[Index].mActorID = m_core.actorID;//ģ��ID


	//������Ϣ��GameWorld
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
	//mData[Index].mName = m_core.Name; //�������
	//mData[Index].mSchool = m_core.Metier; //ְҵ
	//mData[Index].mAnimal = m_core.BornTag; //��Ф
	//mData[Index].mActorID = m_core.actorID;//ģ��ID
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
	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_APPLY_ADDTEAM);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();

}

void CServerTeam::AgreeApply(unsigned long vOtherID)
{
	//DWORD OtherDBID=RegionID2DBID(vOtherID);
	//if(OtherDBID==0){return;}

	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_OTHER_AGREE_APPLY);
	pPacket->WriteLong(vOtherID);
	SendToGameWorld();
}

void CServerTeam::RefuseTeam(unsigned long vOtherID)
{
	DWORD OtherDBID=RegionID2DBID(vOtherID);
	if(OtherDBID==0){return;}

	//������Ϣ��GameWorld
	CG_CmdPacket* pPacket= GameWorldPacket();
	pPacket->WriteByte(R2GW_REFUSE_TEAM);
	pPacket->WriteLong(OtherDBID);
	SendToGameWorld();
}

void CServerTeam::TransferCaptain(unsigned long vOtherID)
{
	//DWORD OtherDBID=RegionID2DBID(vOtherID);
	//if(OtherDBID==0){return;}

	//������Ϣ��GameWorld
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
//------�Կͻ���--------
//�ӵ��ϴ�����Ϣ
int CServerTeam::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket* cmd)
{
	char Operation;
	long OtherID;
	char cFree = 1;
	char* name;

	SAFE_READ(cmd->ReadByte(&Operation))

	//������Ϣ��������
	switch(Operation)
	{
	    case C2R_INVITE_OTHER_BY_NAME:
			SAFE_READ(cmd->ReadString(&name))
			InviteOtherbyName(name);
		    break;
		case C2R_INVITE_OTHER://�Լ�������˼������
			//LOG("C2R_INVITE_OTHER\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			FindInviteOther(OtherID); //������յ���RegionID
			break;
		case C2R_BANISH_OTHER://�Լ�������˴Ӷ�����
			//LOG("C2R_BANISH_OTHER\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			BanishOther(OtherID);
			break;
		case C2R_SELF_LEAVE://�Լ��뿪����
			//LOG("C2R_SELF_LEAVE\n");
			SelfLeave();
			break;
		case C2R_DISMISS://�Լ���ɢ����
			//LOG("C2R_DISMISS\n");
			Dismiss();
			break;
		case C2R_AGREE_OTHER_INVITE://�Լ�ͬ����˵�����
			//LOG("C2R_AGREE_OTHER_INVITE\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			AgreeOtherInvite(OtherID);
			break;
		case C2R_REFUSE_OTHER_INVITE://�Լ��ܾ����˵�����
			//LOG("C2R_REFUSE_OTHER_INVITE\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			RefuseOtherInvite(OtherID);
			break;
		case C2R_BANISH_OTHER_BY_REGION_ID://���������RegionID[DWORD vOtherRegionID]
			//LOG("C2R_BANISH_OTHER_BY_REGION_ID\n");
			SAFE_READ(cmd->ReadLong(&OtherID))
			BanishOtherByRegionID(OtherID);
			break;
		case C2R_TEAM_ONOFF: //����������� 0�ǹ�,�ص�ʱ����˲��ܼ��Լ��Ķ�,�Լ�Ҳ���ܱ�����.1�ǿ�.�෴.
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
		case C2R_TRANSFER_CAPTAIN: //ת�öӳ�
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
	case GW2R_ADD_TEAM_MEMBER:			//����һ������
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

			//���¶������ӵ���ʱ������
			if (mpMaster->m_pFriend)
			{
				mpMaster->m_pFriend->WhenFriend(MemberDBID);
			}
			
			
			string MemberName(pMemberName);
			mData.AddMember(MemberDBID,MemberName,MemberHead,Metier,BornTag,actorID); //region���ݸ���
			

			//if (mpMaster->m_pFriend)
			//{
			//	mpMaster->m_pFriend->WhenFriend(MemberDBID);
			//}

			////�������ж�Ա  ������ʱ����
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
	case GW2R_DELETE_TEAM_MEMBER:		//����һ������
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
				mData.DeleteMember(MemberDBID); //region���ݸ���
			}
			

			pPacket->WriteByte(R2C_DELETE_TEAM_MEMBER);
			pPacket->WriteLong(MemberDBID);
		}
		break;
	case GW2R_UPDATE_ONE_MEMBER_DATA:	//ˢ��ĳ����Ա������
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



			//region���ݸ���
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
			pPacket->WriteString(MapName); //��ͼ����
			pPacket->WriteLong(mCombatPower); //ս������
		}
		break;
	case GW2R_REQUEST_MEMBER_DATA:		//Ҫ���Լ������ݵ�GameWorld
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
			//������Ϣ��GameWorld
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
	case GW2R_UPDATE_ALL_MEMBER_DATA:	//ˢ�����г�Ա������
		{
			char* pTeamData;
			cmd->ReadString(&pTeamData);
			string TeamData=pTeamData;
			mData.Clear();
			TeamData>>mData;

			//��ͻ��˷�����Ϣ
			pPacket->WriteByte(R2C_UPDATE_ALL_MEMBER_DATA);
			pPacket->WriteString(pTeamData);
		}
		break;

	case GW2R_TEAM_MESSAGE:				//ˢһ����Ϣ
		//LOG("GW2R_TEAM_MESSAGE\n");
		{
			long MessageID;
			cmd->ReadLong(&MessageID);

			pPacket->WriteByte(R2C_TEAM_MESSAGE);
			pPacket->WriteLong(MessageID);
		}
		break;
	case GW2R_TEAM_MESSAGE_WITH_ID:		//ˢһ����Ϣ[��ID].
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
	case GW2R_OTHER_AGREE_INVITE:		//����ͬ��������
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

			//region���ݸ���
			if(mData.Empty())
			{//�����
				mData.AddMember(mSelfDBID,mSelfName,mSelfHead,metier,BornTag,actorID);//���Լ�
			}
			string OtherName(pOtherName);
			mData.AddMember(OtherDBID,OtherName,OtherHead,metier,BornTag,actorID);//���¶���



			pPacket->WriteByte(R2C_OTHER_AGREE_INVITE);
			pPacket->WriteLong(OtherDBID);
			pPacket->WriteString(pOtherName);
			pPacket->WriteByte(OtherHead);
			pPacket->WriteByte(metier); //ְҵ
			pPacket->WriteLong(BornTag); //��Ф
			pPacket->WriteLong(actorID); //actorID
			mpMaster->GetPlayerLogic()->RemoveFromInviteMap(OtherDBID);
			//mpMaster->GetPlayerLogic()->AddtoInviteMap(OtherDBID);
		}
		break;
	case GW2R_OTHER_REFUSE_INVITE:		//���˾ܾ��������
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
	case GW2R_TO_WHENFRIEND://�Լ�����ʱ����
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
	case GW2R_OTHER_INVITE:				//���������Լ��������
		//LOG("GW2R_OTHER_INVITE\n");
		{


			long OtherDBID;
			char* pOtherName;

			cmd->ReadLong(&OtherDBID);
			cmd->ReadString(&pOtherName);

			if (mpMaster->m_bRefuseTeam)
			{
				//������Ϣ�������Լ�����  �Լ������˾ܾ����
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
	case GW2R_OTHER_BANISH:				//���������Լ�������
		//LOG("GW2R_OTHER_BANISH\n");
		{
			long OtherDBID;
			cmd->ReadLong(&OtherDBID);

			mData.Clear();//region���ݸ���

			pPacket->WriteByte(R2C_OTHER_BANISH);
			pPacket->WriteLong(OtherDBID);
			mpMaster->GetPlayerLogic()->RemoveFromInviteMap(OtherDBID);
		}
		break;
	case GW2R_OTHER_LEAVE:				//�����뿪����
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
	case GW2R_OTHER_DISMISS:			//���˽�ɢ����
		//LOG("GW2R_OTHER_DISMISS\n");
		{
			long OtherDBID;
			cmd->ReadLong(&OtherDBID);

			mData.Clear();//region���ݸ���

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
			pPacket->WriteLong(BornTag);  //��Ф
			pPacket->WriteLong(WarScores); //ս������

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
//------�Լ�----------
//�Լ�������˼������
void CServerTeam::InviteOther(DWORD vOtherRegionID)
{
	if(vOtherRegionID==mSelfRegionID)
	{//������Լ������Լ���������
		return ;
	}

		
	if(mData.Empty())
	{//�����ǰû�ж��飬���Է�������//���ñ��������Լ��ķ�������
		SendToServer(R2R_SELF_INVITE,vOtherRegionID);
	}
	else
	{//�����ǰ�Ѿ��ж�����
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			if(mData.Size()<MAX_TEAM_MENBER_COUNT)
			{//����������//���ñ��������Լ��ķ�������
				SendToServer(R2R_SELF_INVITE,vOtherRegionID);
			}
			else
			{//��������������
			}
		}
		else
		{//�Լ����Ƕӳ���������
		}
	}
	return;
}
*/

/*
//�Լ�������˴Ӷ�����[��RegionID]
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
	{//������Լ������Լ��뿪����
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//���û�У�������
	}
	else
	{//����ж���
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			if(mData.Exist(vOtherDBID))
			{//�иö�Ա�����ñ��������Լ��ķ������ˣ�ͬʱ�ı䵱ǰ����
				for(int i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==vOtherDBID)
					{//�Ǳ�������Ǹ��ˣ���������Ϣ
						SendToServer(R2R_SELF_BANISH,vOtherDBID);
					}
					else if(mData[i].mDBID==mSelfDBID)
					{//���Լ�
						SendToClient(R2C_TEAM_MESSAGE_WITH_ID,BANISH_MEMBER,NULL,vOtherDBID);
						SendToClient(R2C_DELETE_TEAM_MEMBER,vOtherDBID);
					}
					else
					{//���������ѣ�������һ���˵���Ϣ�������������Ϣ
						SendToServer(R2R_TEAM_MESSAGE_WITH_ID,mData[i].mDBID,BANISH_MEMBER,NULL,vOtherDBID);
						SendToServer(R2R_DELETE_TEAM_MEMBER,mData[i].mDBID,vOtherDBID);
					}
				}
				mData.DeleteMember(vOtherDBID);

				if(mData.Size()==1)
				{//�������㣬�Զ���ɢ����
					SendToClient(R2C_TEAM_MESSAGE,NOT_MEMBER_TEAM_DISMISS);
					SendToClient(R2C_DELETE_TEAM_MEMBER,mSelfDBID);
					mData.Clear();
				}
			}
			else
			{//û�иö�Ա����֮�ͻ���
				SendToClient(R2C_TEAM_MESSAGE,MEMBER_NOT_EXIST);
			}
		}
		else
		{//���Ƕӳ���������
		}
	}
	return;
}
*/



/*
//�Լ�������˴Ӷ�����
void CServerTeam::BanishOther(DWORD vOtherDBID)
{
	if(vOtherDBID==mSelfDBID)
	{//������Լ������Լ��뿪����
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//���û�У�������
	}
	else
	{//����ж���
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			if(mData.Exist(vOtherDBID))
			{//�иö�Ա�����ñ��������Լ��ķ������ˣ�ͬʱ�ı䵱ǰ����
				for(int i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==vOtherDBID)
					{//�Ǳ�������Ǹ��ˣ���������Ϣ
						SendToServer(R2R_SELF_BANISH,vOtherDBID);
					}
					else if(mData[i].mDBID==mSelfDBID)
					{//���Լ�������
					}
					else
					{//���������ѣ�������һ���˵���Ϣ�������������Ϣ
						SendToServer(R2R_TEAM_MESSAGE_WITH_ID,mData[i].mDBID,BANISH_MEMBER,NULL,vOtherDBID);
						SendToServer(R2R_DELETE_TEAM_MEMBER,mData[i].mDBID,vOtherDBID);
					}
				}
				mData.DeleteMember(vOtherDBID);

				if(mData.Size()==1)
				{//�������㣬�Զ���ɢ����
					mData.Clear();
				}
			}
			else
			{//û�иö�Ա��������
			}
		}
		else
		{//���Ƕӳ���������
		}
	}
	return;
}
*/




/*
//�Լ��뿪����
void CServerTeam::SelfLeave()
{
	if(mData.Empty())
	{//���û�У�������
	}
	else
	{//����У�

		//���ö����������˵ı����뿪����[�����Լ�]
		for(int i=mData.Begin(); i!=mData.End();i++)
		{
			if(mData[i].mDBID !=mSelfDBID)
			{//�����Լ�
				SendToServer(R2R_SELF_LEAVE,mData[i].mDBID);
			}
		}

		//�ı䵱ǰ����
		mData.Clear();
	}
	return;
}
*/




/*
//�Լ���ɢ����
void CServerTeam::Dismiss()
{
	if(mData.Empty())
	{//���û�ж��飬������
	}
	else
	{//�����
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			//���ö����������˵ı��˽�ɢ����[�����Լ�]
			for(int i=mData.Begin(); i!=mData.End();i++)
			{
				if(mData[i].mDBID !=mSelfDBID)
				{//�����Լ�
					SendToServer(R2R_OTHER_DISMISS,mData[i].mDBID);
				}
			}

			//ͬʱ�ı䵱ǰ����
			mData.Clear();
		}
		else
		{//���Ƕӳ���������
		}
	}
	return;
}
*/



/*
//�Լ�ͬ����˵�����
void CServerTeam::AgreeOtherInvite(DWORD vOtherDBID)
{
	if(mServerWaitOtherAddSelf)
	{//�Ѿ�ͬ������һ�����飬���ڵȴ���״̬�������Զ��ܾ��������
		RefuseOtherInvite(vOtherDBID);
		return ;
	}

	if(mData.Empty())
	{//û�ж��飬���ñ��˵ķ������˴�������ͬʱ���õȴ���ʶ
		mServerWaitOtherAddSelf=true;
		SendToServer(R2R_SELF_AGREE_INVITE,vOtherDBID);
	}
	else
	{//�Ѿ��ж����ˣ��Զ��ܾ����˵�����
		RefuseOtherInvite(vOtherDBID);
	}
	return;
}
*/


/*
//�Լ��ܾ����˵�����
void CServerTeam::RefuseOtherInvite(DWORD vOtherDBID)
{
	//���ñ��˵ķ������˴�����
	SendToServer(R2R_SELF_REFUSE_INVITE,vOtherDBID);
}
*/


/*
//------������������------
//�Լ�����
void CServerTeam::OnDisconnect()
{
	//���÷��������뿪����
	SelfLeave();
}
//�Լ�����
void CServerTeam::OnOffline()
{
	//���÷��������뿪����
	SelfLeave();
}

//�Լ��ĶԶ�����Ӱ�����ֵ�仯
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


	//������Ϣ�����еĶ�Ա�ķ�������[�����Լ�]
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



//���˽���ɹ��������
void CServerTeam::OtherAddSelfSucceed(DWORD vOtherDBID,std::string& vrOtherName)
{
	//�ı�ȴ���ʶ
	mServerWaitOtherAddSelf=false;
	//�ı���������
	mData.AddMember(vOtherDBID,vrOtherName);
	mData.AddMember(mSelfDBID,mSelfName);
	GetDataFromMaster();
	SendToClient(R2C_ADD_TEAM_MEMBER,vOtherDBID,&vrOtherName);
	SendToClient(R2C_ADD_TEAM_MEMBER,mSelfDBID,&mSelfName);
	SendToClient(R2C_TEAM_MESSAGE_WITH_ID,ADD_TEAM_SUCCEED,NULL,vOtherDBID);
}

//���˽������ʧ��
void CServerTeam::OtherAddSelfFault(DWORD vOtherDBID)
{
	//�ı�ȴ���ʶ
	mServerWaitOtherAddSelf=false;
}

//���˲�����
void CServerTeam::OtherNotExist(DWORD vOtherDBID)
{
	//�ı�ȴ���ʶ
	mServerWaitOtherAddSelf=false;
}



//-------����-----------
//����ͬ��������
void CServerTeam::OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mServerWaitOtherAddSelf)
	{//�Ѿ�ͬ������һ�����飬���ڵȴ���״̬//����ʧ�ܣ�������Ϣ[�Ѿ������˱��˵Ķ���]
		SendToServer(R2R_SELF_ADD_OTHER_FAULT,vOtherDBID);
		SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
		return;
	}

	if(mData.Empty())
	{//��ǰ����Ϊ�գ������Լ�
		mData.AddMember(mSelfDBID,mSelfName);
		GetDataFromMaster();
	}

	if(mData.GetCaptain()==mSelfDBID) 
	{//�Լ��Ƕӳ�

		if(mData.Size()>=MAX_TEAM_MENBER_COUNT)
		{//�����Ƿ��Ѿ��������򣬷�����Ϣ�����˵ķ���˺Ϳͻ���[����]
			SendToServer(R2R_SELF_ADD_OTHER_FAULT,vOtherDBID);
			SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,TEAM_COUNT_OVER);
			return;
		}
		else
		{
			if(mData.Exist(vOtherDBID))
			{//�Ѿ������������
				SendToServer(R2R_SELF_ADD_OTHER_FAULT,vOtherDBID);
				SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,ALREADY_IN_TEAM);
				return;
			}
			else
			{//�����������
				//�������ݣ�������Ϣ����Ա[�Լ�����������]
				
				mData.AddMember(vOtherDBID,vrOtherName);
				SendToServer(R2R_SELF_ADD_OTHER_SUCCEED,vOtherDBID);//����Ϣ���Ǹ��������

				int i;
				for(i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==mSelfDBID)
					{//�Լ�
						SendToClient(R2C_OTHER_AGREE_INVITE,vOtherDBID,&vrOtherName);
					}
					else if(mData[i].mDBID==vOtherDBID)
					{//�¶���
					}
					else
					{//�϶���
						SendToServer(R2R_ADD_TEAM_MEMBER,mData[i].mDBID,vOtherDBID,&vrOtherName);//���϶�Ա���¶�Ա������
						SendToServer(R2R_TEAM_MESSAGE_WITH_ID,mData[i].mDBID,MEMBER_ADD_TEAM,NULL,vOtherDBID);//�����϶��ѣ����˼���

						SendToServer(R2R_ADD_TEAM_MEMBER,vOtherDBID,mData[i].mDBID,&(mData[i].mName));//���¶��Ѽ��϶��ѵ�����
					}
				}

				for(i=mData.Begin();i!=mData.End();i++)
				{
					if(mData[i].mDBID==mSelfDBID)
					{//�Լ�
						SendToServer(R2R_UPDATE_ONE_MEMBER_DATA,vOtherDBID,mSelfDBID);//�Լ������ݸ�����
						SendToServer(R2R_FORCE_UPDATE_DATA,vOtherDBID,mSelfDBID);//Ҫ�����˽����ݸ��Լ�
					}
					else if(mData[i].mDBID==vOtherDBID)
					{//����
					}
					else
					{//�϶���
						SendToServer(R2R_UPDATE_ONE_MEMBER_DATA,vOtherDBID,mData[i].mDBID);//���϶��ѵ����ݷ�������
						SendToServer(R2R_FORCE_UPDATE_DATA,vOtherDBID,mData[i].mDBID);//Ҫ�����˽����ݷ����϶���
					}
				}
			}
		}
	}
	
	return;
}

//���˾ܾ��������
void CServerTeam::OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	//�����Լ����ܾ�
	SendToClient(R2C_OTHER_REFUSE_INVITE,vOtherDBID,&vrOtherName);
}


//���˶Զ�����Ӱ�����ֵ�仯
void CServerTeam::OtherDateChangeAffectTeam(DWORD vOtherDBID)
{
	//�����Լ��Ŀͻ���
	SendToClient(R2C_UPDATE_ONE_MEMBER_DATA,vOtherDBID);
}


//���������Լ��������
void CServerTeam::OtherInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	if(mData.Empty())
	{//���͸��Լ���Ϣ
		SendToClient(R2C_OTHER_INVITE,vOtherDBID,&vrOtherName);
	}
	else
	{//�Ѿ��ж���//���ñ��˵ķ������˺���[���˾ܾ�����]
		//���ݵ�ǰ�����뻥������������Ƿ��Զ��ܾ�
		SendToServer(R2R_TEAM_MESSAGE,vOtherDBID,THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
		SendToServer(R2R_SELF_REFUSE_INVITE,vOtherDBID);
	}
}

//���������Լ�������
void CServerTeam::OtherBanish(DWORD vOtherDBID)
{
	if(mData.Empty())
	{//û�ж��飬������
		return;
	}

	if(mData.GetCaptain()==vOtherDBID)
	{//�����Ƕӳ�
		//������Ϣ���Լ���ͬʱ�ı�����	
		SendToClient(R2C_OTHER_BANISH,vOtherDBID);
		mData.Clear();
	}
	else
	{//���˲��Ƕӳ�
		return;
	}
}

//�����뿪����
void CServerTeam::OtherLeave(DWORD vOtherDBID)
{
	if(mData.Exist(vOtherDBID))
	{
		//������Ϣ���Լ���ͬʱ�ı�����
		SendToClient(R2C_OTHER_LEAVE,vOtherDBID);
		mData.DeleteMember(vOtherDBID);

		if(mData.Size()==1 )
		{//�����������Զ���ɢ
			SendToClient(R2C_DELETE_TEAM_MEMBER,mSelfDBID);
			SendToClient(R2C_TEAM_MESSAGE,NOT_MEMBER_TEAM_DISMISS);
			mData.Clear();
			return;
		}
	}
}





//���˽�ɢ����
void CServerTeam::OtherDismiss(DWORD vOtherDBID)
{
	if(mData.GetCaptain()==vOtherDBID)
	{//�����Ƕӳ�
		//�����û���ͬʱ�ı�����
		SendToClient(R2C_OTHER_DISMISS,vOtherDBID);
		mData.Clear();
	}
}

//------�Կͻ���--------
//�ӵ��ϴ�����Ϣ
int CServerTeam::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket* cmd)
{
	char Operation;
	long OtherID;
	SAFE_READ(cmd->ReadByte(&Operation))

	//������Ϣ��������
	switch(Operation)
	{
	case C2R_INVITE_OTHER://�Լ�������˼������
		SAFE_READ(cmd->ReadLong(&OtherID))
		InviteOther(OtherID);
		break;
	case C2R_BANISH_OTHER://�Լ�������˴Ӷ�����
		SAFE_READ(cmd->ReadLong(&OtherID))
		BanishOther(OtherID);
		break;
	case C2R_SELF_LEAVE://�Լ��뿪����
		SelfLeave();
		break;
	case C2R_DISMISS://�Լ���ɢ����
		Dismiss();
		break;
	case C2R_AGREE_OTHER_INVITE://�Լ�ͬ����˵�����
		SAFE_READ(cmd->ReadLong(&OtherID))
		AgreeOtherInvite(OtherID);
		break;
	case C2R_REFUSE_OTHER_INVITE://�Լ��ܾ����˵�����
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



//������Ϣ���ͻ���
void CServerTeam::SendToClient(byte vOperation,DWORD vParameter,std::string* vpName,DWORD vDBIDInMessage)
{
	g_sendCmd->BeginWrite();
	
	g_sendCmd->WriteShort(r2c_subsystem_team);
	g_sendCmd->WriteByte(vOperation);
	switch(vOperation)
	{
	case R2C_ADD_TEAM_MEMBER:			//����һ������
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_DELETE_TEAM_MEMBER:		//����һ������
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_UPDATE_ONE_MEMBER_DATA:	//ˢ��ĳ����Ա������
		{
			g_sendCmd->WriteLong(vParameter);
			
			int Index=mData.Find(vParameter);

			if(Index==mData.End())
			{//û���ҵ�
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
				g_sendCmd->WriteLong(0);
			}
			else
			{//�ҵ�
				g_sendCmd->WriteLong(mData[Index].mHp);
				g_sendCmd->WriteLong(mData[Index].mMaxHp);
				g_sendCmd->WriteLong(mData[Index].mMp);
				g_sendCmd->WriteLong(mData[Index].mMaxMp);
			}
		}
		break;
	case R2C_UPDATE_ALL_MEMBER_DATA:		//ˢ�����г�Ա������
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
	case R2C_TEAM_MESSAGE:				//ˢһ����Ϣ
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_TEAM_MESSAGE_WITH_ID:
		g_sendCmd->WriteLong(vDBIDInMessage);
		g_sendCmd->WriteLong(vParameter);
		break;
		//-------����-----------
	case R2C_OTHER_AGREE_INVITE:		//����ͬ��������
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_OTHER_REFUSE_INVITE:		//���˾ܾ��������
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*) (vpName->c_str()));
		break;
	case R2C_OTHER_INVITE:				//���������Լ��������
		g_sendCmd->WriteLong(vParameter);
		g_sendCmd->WriteString((char*)(vpName->c_str()));
		break;
	case R2C_OTHER_BANISH:				//���������Լ�������
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_OTHER_LEAVE:				//�����뿪����
		g_sendCmd->WriteLong(vParameter);
		break;
	case R2C_OTHER_DISMISS:				//���˽�ɢ����
		g_sendCmd->WriteLong(vParameter);
		break;
	default:
		break;
	}
	mpMaster->RecvCmd(g_sendCmd);
}







//-----------��������---------------

/*
//------�Է����--------
//������Ϣ�������
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
	case R2R_ADD_TEAM_MEMBER:			//����һ������
		R2rSendCmd.mParameter=vParameter;
		R2rSendCmd.mpName=(char*)vpName->c_str();
		break;
	case R2R_DELETE_TEAM_MEMBER:		//����һ������
		R2rSendCmd.mParameter=vParameter;
		break;
	case R2R_UPDATE_ONE_MEMBER_DATA:	//ˢ��ĳ����Ա������
		{
			R2rSendCmd.mParameter=vParameter;
			int Index=mData.Find(vParameter);
			if(Index==mData.End())
			{//û���ҵ�
				R2rSendCmd.mHp=0;
				R2rSendCmd.mMaxHp=0;
				R2rSendCmd.mMp=0;
				R2rSendCmd.mMaxMp=0;
			}
			else
			{//�ҵ�
				R2rSendCmd.mHp=mData[Index].mHp;
				R2rSendCmd.mMaxHp=mData[Index].mMaxHp;
				R2rSendCmd.mMp=mData[Index].mMp;
				R2rSendCmd.mMaxMp=mData[Index].mMaxMp;
			}
		}
		break;
	case R2R_TEAM_MESSAGE:				//ˢһ����Ϣ
		R2rSendCmd.mParameter=vParameter;
		break;
	case R2R_TEAM_MESSAGE_WITH_ID:
		R2rSendCmd.mDBIDInMessage=vDBIDInMessage;
		R2rSendCmd.mParameter=vParameter;
		break;
	case R2R_SELF_ADD_OTHER_SUCCEED:	//�Լ������˼��뵽�Լ�����ɹ�
		R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
		break;
	case R2R_SELF_ADD_OTHER_FAULT:		//�Լ������˼��뵽�Լ�����ʧ��
		break;
	case R2R_SELF_AGREE_INVITE:			//�Լ�ͬ�������˵Ķ���
		R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
		break;
	case R2R_SELF_REFUSE_INVITE:		//�Լ��ܾ�������˵Ķ���
		R2rSendCmd.mpFromName=(char*)mSelfName.c_str();
		break;
	case R2R_SELF_INVITE:				//�Լ�������˼������
		{
			//��������RegionID
			CRegionCreature* pCreature =mpMaster->m_scene->FindCreature(SendToID);
			if(pCreature==NULL)
			{//�������Ҳ���
				R2rSendCmd.BeginSend();
				R2rSendCmd.mOperation=R2R_OTHER_REGION_ID_NOT_EXIST;
				R2rSendCmd.mFromDBID=0;
				R2rSendCmd.mParameter=SendToID;
				ReceiveFromServer(R2rSendCmd);
			}
			else
			{//�ҵ�������
				if(pCreature->m_pTeam==NULL)
				{//��������޷����
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
	case R2R_SELF_BANISH:				//�Լ�������˳�����
		break;
	case R2R_SELF_LEAVE:
		break;
	case R2R_SELF_DISMISS:				//�Լ���ɢ����
		break;
	case R2R_FORCE_UPDATE_DATA:
		R2rSendCmd.mParameter=vParameter;
		break;
	default:
		break;
	}


	//���͵�game_word
	g_region->m_gws.SendTeam(SendToID,R2rSendCmd);


	//����޷��ҵ�
	//����Ϣ��
	
	*//*
	R2rSendCmd.BeginSend();
	R2rSendCmd.mOperation=R2R_OTHER_DB_ID_NOT_EXIST;
	R2rSendCmd.mFromDBID=SendToID;//�Ҳ������Ǹ�DBID
	R2rSendCmd.mParameter=SendToID;//�Ҳ������Ǹ�DBID
	ReceiveFromServer();
	*/




	/*
	CRegionUser* pUser=g_region-> FindUser(SendToID);
	if(pUser==NULL)
	{//���û��Ҳ���
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

//�ӵ�����˵���Ϣ
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
	case R2R_ADD_TEAM_MEMBER://����һ������
		{
			char*	pName;

			OtherDBID=vrR2rSendCmd.mParameter;
			pName=vrR2rSendCmd.mpName;
			string TempString=string(pName);
			mData.AddMember(OtherDBID,TempString);
			//���߿ͻ���
			SendToClient(R2C_ADD_TEAM_MEMBER,OtherDBID,&string(pName));
		}
		break;
	case R2R_DELETE_TEAM_MEMBER://����һ������
		OtherDBID=vrR2rSendCmd.mParameter;
		mData.DeleteMember(OtherDBID);
		//���߿ͻ���
		SendToClient(R2C_DELETE_TEAM_MEMBER,OtherDBID);
		break;
	case R2R_UPDATE_ONE_MEMBER_DATA://ˢ��ĳ����Ա������
		{
			OtherDBID=vrR2rSendCmd.mParameter;
			int Index=mData.Find(OtherDBID);
			if(Index==mData.End())
			{//û���ҵ�
				Hp=vrR2rSendCmd.mHp;
				MaxHp=vrR2rSendCmd.mMaxHp;
				Mp=vrR2rSendCmd.mMp;
				MaxMp=vrR2rSendCmd.mMaxMp;

			}
			else
			{//�ҵ�
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
	case R2R_TEAM_MESSAGE://ˢһ����Ϣ
		{
			DWORD Parameter=vrR2rSendCmd.mParameter;
			SendToClient(R2C_TEAM_MESSAGE,Parameter);
		}
		break;
	case R2R_TEAM_MESSAGE_WITH_ID://ˢһ����Ϣ
		{
			DWORD DBIDInMessage=vrR2rSendCmd.mDBIDInMessage;
			DWORD Parameter=vrR2rSendCmd.mParameter;
			SendToClient(R2C_TEAM_MESSAGE_WITH_ID,Parameter,NULL,DBIDInMessage);
		}
		break;


	case R2R_OTHER_ADD_SELF_SUCCEED:	//���˽���ɹ��������
		{
			char*	pFromName;
			pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherAddSelfSucceed(FromDBID,TempString);
		}
		
		break;
	case R2R_OTHER_ADD_SELF_FAULT:		//���˽������ʧ��
		OtherAddSelfFault(FromDBID);
		break;
	case R2R_OTHER_REGION_ID_NOT_EXIST:			//���˲�����
		OtherNotExist(FromDBID);
		SendToClient(R2C_TEAM_MESSAGE,THAT_PLAYER_NOT_INLINE);
		break;
	case R2R_OTHER_DB_ID_NOT_EXIST:				//���˲�����
		OtherNotExist(FromDBID);
		SendToClient(R2C_TEAM_MESSAGE,THAT_PLAYER_NOT_INLINE);
		break;
	case R2R_OTHER_AGREE_INVITE:		//����ͬ��������
		{
			char*	pFromName;
			pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherAgreeInvite(FromDBID,TempString);
		}
		break;
	case R2R_OTHER_REFUSE_INVITE:		//���˾ܾ��������
		{
			char* pFromName;
			pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherRefuseInvite(FromDBID,TempString);
		}

		break;
	case R2R_OTHER_INVITE:				//���������Լ��������
		{
			char* pFromName=vrR2rSendCmd.mpFromName;
			TempString=string(pFromName);
			OtherInvite(FromDBID,TempString);
		}

		break;
	case R2R_OTHER_BANISH:				//���������Լ�������
		OtherBanish(FromDBID);
		break;
	case R2R_OTHER_LEAVE:
		OtherLeave(FromDBID);
		break;
	case R2R_OTHER_DISMISS:				//���˽�ɢ����
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
