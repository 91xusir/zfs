
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
	case R2GW_INVITE_OTHER://�Լ�������˼������[DWORD vOtherDBID]
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
			LOG("R2GW_INVITE_OTHER_BY_NAME\n");//�Լ�������˼������[Name]
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
	case R2GW_BANISH_OTHER://�Լ�������˴Ӷ�����[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_BANISH_OTHER\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			BanishOther(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_SELF_LEAVE://�Լ��뿪����
		{
#ifdef _DEBUG
			LOG("R2GW_SELF_LEAVE\n");
#endif
			SelfLeave(SelfDBID);
		}
		break;
	case R2GW_DISMISS://�Լ���ɢ����
		{
#ifdef _DEBUG
			LOG("R2GW_DISMISS\n");
#endif
			Dismiss(SelfDBID);
		}
		break;
	case R2GW_AGREE_OTHER_INVITE://�Լ�ͬ����˵�����[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_AGREE_OTHER_INVITE\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			AgreeOtherInvite(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_REFUSE_OTHER_INVITE://�Լ��ܾ����˵�����[DWORD vOtherDBID]
		{
#ifdef _DEBUG
			LOG("R2GW_REFUSE_OTHER_INVITE\n");
#endif
			vpPacket->ReadLong(&OtherDBID);
			RefuseOtherInvite(SelfDBID,OtherDBID);
		}
		break;
	case R2GW_DATE_CHANGE://�Լ������ݱ仯Ӱ�쵽����������Ա[int vHp,int vMaxHp,int vMp,int vMaxMp]
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
			char* mRegion; //��ͼ����
            long mCombatPower;   //ս������

			vpPacket->ReadLong(&Hp);
			vpPacket->ReadLong(&MaxHp);
			vpPacket->ReadLong(&Mp);
			vpPacket->ReadLong(&MaxMp);
			vpPacket->ReadByte(&Level);
			vpPacket->ReadLong(&X);
			vpPacket->ReadLong(&Y);
			vpPacket->ReadString(&mRegion);
			vpPacket->ReadLong(&mCombatPower);

			//�Լ��ĶԶ�����Ӱ�����ֵ�仯
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
			const char* szBuf[3] = {"����", "�ӳ�", "���"};
			if (0 <= nType && nType <3)
			{
				bool bOK = SetPickType(SelfDBID, nType);
				if (bOK)
				{
					os<<"�ӳ�����ʰȡģʽΪ��"<<szBuf[nType]<<"ʰȡģʽ";
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

			rPacket.WriteByte(GW2R_SETREFUSE_TEAM);//���������˾ܾ����

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

	rPacket.WriteByte(GW2R_INVITE_OTHER_BY_NAME);//���˽�ɢ����[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);
	
	int Session= g_server.GetRegionSession(pGWUser->regionID);
	if(Session==-1)
	{
		return;
	}

	g_server.m_region.SendToPeer(Session);
}

//------�Լ�----------
//�Լ�������˼������
void GwTeamManager::InviteOther(DWORD vSelfDBID,DWORD vOtherDBID)
{
	if(vSelfDBID==vOtherDBID)
	{//������Լ������Լ���������
		return ;
	}

	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//�Լ������ڣ�������
		return;
	}

	if(pSelf->team==NULL)
	{//�����ǰû�ж��飬���Է�������//���ñ��������Լ�
		GWUser* pOther=FindUser(vOtherDBID);
		if(pOther==NULL)
		{//���˲�����//����Ϣ���Լ�
			SendToRegionTeamMessage(pSelf,THAT_PLAYER_NOT_INLINE);
			return;
		}

		OtherInvite(pSelf,pOther);
	}
	else
	{//�����ǰ�Ѿ��ж�����
		if(pSelf->team->GetCaptain()==vSelfDBID)
		{//�Ƕӳ�
			if(pSelf->team->Size()<MAX_TEAM_MENBER_COUNT)
			{//����������//���ñ��������Լ�
				GWUser* pOther=FindUser(vOtherDBID);
				if(pOther==NULL)
				{//���˲�����
					SendToRegionTeamMessage(pSelf,THAT_PLAYER_NOT_INLINE);
				}
				else
				{
					OtherInvite(pSelf,pOther);
				}
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

//�Լ�������˴Ӷ�����
void GwTeamManager::BanishOther(DWORD vSelfDBID,DWORD vOtherDBID)
{
	if(vSelfDBID==vOtherDBID)
	{//������Լ������Լ��뿪����
		SelfLeave(vSelfDBID);
		return;
	}

	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//�Լ������ڣ�������
		return;
	}

	if(pSelf->team==NULL)
	{//û�ж���//������
		return;
	}

	CTeamData& rTeam=*(pSelf->team);

	if(rTeam.GetCaptain()!=vSelfDBID)
	{//���Ƕӳ�//������
		return;
	}

	if(rTeam.Exist(vOtherDBID))
	{//�иö�Ա�����ñ��������Լ��ķ������ˣ�ͬʱ�ı䵱ǰ����
		
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			if(rTeam[i].mDBID==vOtherDBID)
			{//�Ǳ�������Ǹ��ˣ���������Ϣ
				GWUser* pOther=FindUser(vOtherDBID);
				if(pOther!=NULL)
				{//�ҵ�����������
					//������Ϣ����������//�������
					SendToRegionOtherBanish(pOther,vSelfDBID);
					pOther->team=NULL;//������Ҷ������
				}
			}
			else if(rTeam[i].mDBID==vSelfDBID)
			{//���Լ�
				SendToRegionTeamMessageWithID(pSelf,vOtherDBID,BANISH_MEMBER);
				SendToRegionDeleteTeamMember(pSelf,vOtherDBID);
			}
			else
			{//���������ѣ�������һ���˵���Ϣ�������������Ϣ

				GWUser* pMember=FindUser(rTeam[i].mDBID);
				if(pMember!=NULL)
				{//�ҵ�������Ա
					SendToRegionTeamMessageWithID(pMember,vOtherDBID,BANISH_MEMBER);
					SendToRegionDeleteTeamMember(pMember,vOtherDBID);
				}
			}
		}

		rTeam.DeleteMember(vOtherDBID);
		if(rTeam.Size()==1)
		{//�������㣬�Զ���ɢ����
			SendToRegionTeamMessage(pSelf,NOT_MEMBER_TEAM_DISMISS);
			SendToRegionDeleteTeamMember(pSelf,vSelfDBID);
			DEL_ONE(pSelf->team);
			//delete pSelf->team;
			//pSelf->team=NULL;
		}
	}
	else
	{//û�иö�Ա����֮�ͻ���
		SendToRegionTeamMessage(pSelf,MEMBER_NOT_EXIST);
	}
	return;
}

void GwTeamManager::SelfLeave(GWUser* vpSelf)
{
	if(vpSelf->team==NULL)
	{//���û�У�������
		return;
	}

	CTeamData& rTeam=*(vpSelf->team);
	//���ݱ䶯
	rTeam.DeleteMember(vpSelf->userID);
	vpSelf->team=NULL;

	if(rTeam.Size()==1)
	{//�����������Զ���ɢ
		GWUser* pMember=FindUser(rTeam.GetCaptain());
		if(pMember!=NULL)
		{
			if(pMember->team!=NULL)
			{
				SendToRegionOtherLeave(pMember,vpSelf->userID);
				SendToRegionDeleteTeamMember(pMember,pMember->userID);
				SendToRegionTeamMessage(pMember,NOT_MEMBER_TEAM_DISMISS);

				//�������
				DEL_ONE(pMember->team);
				//delete pMember->team;
				//pMember->team=NULL;
			}
		}
	}
	else
	{
		//ʣ�µĶ���
		for(int i=rTeam.Begin();i!=rTeam.End();++i)
		{
			GWUser* pMember=FindUser(rTeam[i].mDBID);
			if(pMember!=NULL)
			{//�ҵ�����
				if(pMember->team!=NULL)
				{
					SendToRegionOtherLeave(pMember,vpSelf->userID);
				}
			}
		}
	}
}

//�Լ��뿪����
void GwTeamManager::SelfLeave(DWORD vSelfDBID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//�Լ�������//������
		return;
	}
	SelfLeave(pSelf);
}

//�Լ���ɢ����
void GwTeamManager::Dismiss(DWORD vSelfDBID)
{
	GWUser* pSelf=FindUser(vSelfDBID);
	if(pSelf==NULL)
	{//�Լ�������//������
		return;
	}

	if(pSelf->team==NULL)
	{//û�ж���//������
		return;
	}

	CTeamData& rTeam=*(pSelf->team);

	if(rTeam.GetCaptain()==vSelfDBID)
	{//�Ƕӳ�
		//���ö����������˵ı��˽�ɢ����[�����Լ�]
		for(int i=rTeam.Begin(); i!=rTeam.End();++i)
		{
			if(rTeam[i].mDBID !=vSelfDBID)
			{//�����Լ�//����Ϣ������//�����ɢ//�ı�����
				GWUser* pMember=FindUser(rTeam[i].mDBID);
				if(pMember!=NULL)
				{
					SendToRegionOtherDismiss(pMember,vSelfDBID);
					pMember->team=NULL;
				}
			}
		}

		//�ı�����
		DEL_ONE(pSelf->team);
		//delete pSelf->team;
		//pSelf->team=NULL;
	}
	else
	{//���Ƕӳ�//������
	}
	return;
}


//�Լ�ͬ����˵�����
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
	{//û�ж���//���ñ��˵ĺ���
		OtherAgreeInvite(pSelf,pOther);
		SendDateToWhenFriend(vSelfDBID,vOtherDBID);
	}
	else
	{//�Ѿ��ж�����//�Զ��ܾ����˵�����
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
	rPacket.WriteByte(GW2R_TO_WHENFRIEND);//Ҫ��regionˢ���Լ������ݵ�GameWorld[��]
	SendToRegion(pSelf);
}


//�Լ��ܾ����˵�����
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



//�Լ��ĶԶ�����Ӱ�����ֵ�仯
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
		{//�Լ�
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
		{//��������
			GWUser* pMember=FindUser(rTeam[i].mDBID);
			if(pMember!=NULL)
			{
				SendToRegionUpdateOneMemberData(pMember,vSelfDBID,vHp,vMaxHp,vMp,vMaxMp,vLevel,X,Y,MapName,mCombatPower);
			}
		}
	}
	return;
}





//-------����-----------

//����ͬ��������
void GwTeamManager::OtherAgreeInvite(GWUser* vpOther,GWUser* vpSelf)
{
	if(vpSelf->team==NULL)
	{//��ǰ����Ϊ��//����һ������//�����Լ�
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
	{//���Ƕӳ�//������
		return;
	}

	if(rTeam.Size()>=MAX_TEAM_MENBER_COUNT)
	{//�����Ƿ��Ѿ��������򣬷�����Ϣ������[����]
		SendToRegionTeamMessage(vpOther,TEAM_COUNT_OVER);
		return;
	}

	if(rTeam.Exist(vpOther->userID))
	{//�Ѿ������������
		SendToRegionTeamMessage(vpOther,ALREADY_IN_TEAM);
		return;
	}

	//�������ݣ�������Ϣ����Ա[�Լ�����������]
	
	//��������
	rTeam.AddMember(vpOther->userID,vpOther->name,vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);
	vpOther->team=&rTeam;

	//������Ա
	for(int i= rTeam.Begin();i!=rTeam.End();++i)
	{
		if(rTeam[i].mDBID==vpSelf->userID)
		{//�Լ�
			SendToRegionAddTeamMember(vpOther,vpSelf->userID,vpSelf->name.c_str(),vpSelf->attr.HeadImageID,vpSelf->attr.metier,vpSelf->attr.BornTag,vpSelf->attr.actorID);//�����¶�Ա�Ӷӳ�

			SendToRegionOtherAgreeInvite(vpSelf,vpOther->userID,vpOther->name.c_str(),vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);
			SendToRegionRequestMemberData(vpSelf);//Ҫ���������
		}
		else if(rTeam[i].mDBID==vpOther->userID)
		{//�¶���
			SendToRegionAddTeamMember(vpOther,vpOther->userID,vpOther->name.c_str(),vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);//�����¶�Ա�����Լ�
			SendToRegionRequestMemberData(vpOther);//Ҫ���������
			SendToRegionTeamMessageWithID(vpOther,vpSelf->userID,ADD_TEAM_SUCCEED);//������Ϣ
		}
		else
		{//�϶���
			GWUser* pMember=FindUser(rTeam[i].mDBID);
			if(pMember!=NULL)
			{
				SendToRegionAddTeamMember(pMember,vpOther->userID,vpOther->name.c_str(),vpOther->attr.HeadImageID,vpOther->attr.metier,vpOther->attr.BornTag,vpOther->attr.actorID);//���϶�Ա���¶�Ա������
				SendToRegionTeamMessageWithID(pMember,vpOther->userID,MEMBER_ADD_TEAM);//�����϶��ѣ����˼���

				SendToRegionAddTeamMember(vpOther,pMember->userID,pMember->name.c_str(),pMember->attr.HeadImageID,pMember->attr.metier,pMember->attr.BornTag,pMember->attr.actorID);//���¶��Ѽ��϶���
				SendToRegionRequestMemberData(pMember);//Ҫ���������
			}
		}
	}
	return;
}




//���������Լ��������
void GwTeamManager::OtherInvite(GWUser* vpOther,GWUser* vpSelf)
{
	if(vpSelf->team==NULL)
	{//û�ж���//������Ϣ���Լ�
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
	{//�Ѿ��ж���
		
		if(vpSelf->team->Exist(vpOther->userID))
		{//��ͬһ������//������Ϣ��������
			SendToRegionTeamMessage(vpOther,ALREADY_IN_TEAM);
		}
		else
		{//�Ѿ������Ķ���//������Ϣ��������
			SendToRegionTeamMessage(vpOther,THAT_PLAY_ALREADY_ADD_OTHER_TEAM);
		}

		//����Ϣ��������//���˾ܾ�����
		SendToRegionOtherRefuseInvite(vpOther,vpSelf->userID,vpSelf->name.c_str());
	}
}


void GwTeamManager::SendToRegionAddTeamMember(GWUser* vpSendToUser,DWORD vMemberDBID,const char* vpMemberName,int headImageID,char Metier,long BornTag,long actorID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_ADD_TEAM_MEMBER);//����һ������[DWORD vMemberDBID,char* vpMemberName]
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
	rPacket.WriteByte(GW2R_DELETE_TEAM_MEMBER);//����һ������[DWORD vMemberDBID]
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
		//������Ա
		for(int i= rTeam.Begin();i!=rTeam.End();++i)
		{
			if(rTeam[i].mDBID==pSelf->userID)
			{//�Լ�
				SendToRegionAddTeamMember(pOther, pSelf->userID, pSelf->name.c_str(),pSelf->attr.HeadImageID,pSelf->attr.metier,pSelf->attr.BornTag,pSelf->attr.actorID);//�����¶�Ա�Ӷӳ�

				SendToRegionOtherAgreeInvite(pSelf,pOther->userID,pOther->name.c_str(),pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);
				SendToRegionRequestMemberData(pSelf);//Ҫ���������
			}
			else if(rTeam[i].mDBID==pOther->userID)
			{//�¶���
				SendToRegionAddTeamMember(pOther,pOther->userID,pOther->name.c_str(),pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);//�����¶�Ա�����Լ�
				SendToRegionRequestMemberData(pOther);//Ҫ���������
				SendToRegionTeamMessageWithID(pOther,pSelf->userID,ADD_TEAM_SUCCEED);//������Ϣ
			}
			else
			{//�϶���
				GWUser* pMember=FindUser(rTeam[i].mDBID);
				if(pMember!=NULL)
				{
					SendToRegionAddTeamMember(pMember,pOther->userID,pOther->name.c_str(),pOther->attr.HeadImageID,pOther->attr.metier,pOther->attr.BornTag,pOther->attr.actorID);//���϶�Ա���¶�Ա������
					SendToRegionTeamMessageWithID(pMember,pOther->userID,MEMBER_ADD_TEAM);//�����϶��ѣ����˼���

					SendToRegionAddTeamMember(pOther,pMember->userID,pMember->name.c_str(),pMember->attr.HeadImageID,pMember->attr.metier,pMember->attr.BornTag,pMember->attr.actorID);//���¶��Ѽ��϶���
					SendToRegionRequestMemberData(pMember);//Ҫ���������
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
	rPacket.WriteByte(GW2R_UPDATE_ONE_MEMBER_DATA);//ˢ��ĳ����Ա������[DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp]
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
	{//����
		return ;
	}

	if(vpSelf->team==NULL)
	{//û�ж���
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
	rPacket.WriteByte(GW2R_UPDATE_ALL_MEMBER_DATA);//ˢ���г�Ա������
	rPacket.WriteString((char*)TeamString.c_str());
	SendToRegion(vpSendToUser);

	return;
}

void GwTeamManager::SendToRegionRequestMemberData(GWUser* vpSendToUser)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_REQUEST_MEMBER_DATA);//Ҫ��regionˢ���Լ������ݵ�GameWorld[��]
	SendToRegion(vpSendToUser);
}

void GwTeamManager::SendToRegionTeamMessage(GWUser* vpSendToUser,int vMessageID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_TEAM_MESSAGE);//ˢһ����Ϣ[int vMessageID]
	rPacket.WriteLong((long)vMessageID);

	SendToRegion(vpSendToUser);
}

void GwTeamManager::SendToRegionTeamMessageWithID(GWUser* vpSendToUser,DWORD vDBIDInMessage,int vMessageID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_TEAM_MESSAGE_WITH_ID);//ˢһ����Ϣ[��ID][DWORD vDBIDInMessage,int vMessageID]
	rPacket.WriteLong((long)vDBIDInMessage);
	rPacket.WriteLong((long)vMessageID);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherAgreeInvite(GWUser* vpSendToUser,DWORD vOtherDBID,const char* vpOtherName,char vOtherHead,char metier, long BornTag,long actorID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_AGREE_INVITE);//����ͬ��������[DWORD vOtherDBID,char* vpOtherName]
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
	rPacket.WriteByte(GW2R_OTHER_REFUSE_INVITE);//���˾ܾ��������[DWORD vOtherDBID,char* vpOtherName]
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
	rPacket.WriteByte(GW2R_OTHER_BANISH);//���������Լ�������[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);

	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherLeave(GWUser* vpSendToUser,DWORD vOtherDBID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);
	rPacket.WriteByte(GW2R_OTHER_LEAVE);//�����뿪����[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);


	SendToRegion(vpSendToUser);

}

void GwTeamManager::SendToRegionOtherDismiss(GWUser* vpSendToUser,DWORD vOtherDBID)
{
	CG_CmdPacket& rPacket=RegionPack(vpSendToUser->userID);

	rPacket.WriteByte(GW2R_OTHER_DISMISS);//���˽�ɢ����[DWORD vOtherDBID]
	rPacket.WriteLong((long)vOtherDBID);

	SendToRegion(vpSendToUser);
}





/*
//�����뿪����
void GwTeamManager::OtherLeave( DWORD vOtherDBID,GWUser* vpSelf)
{
if(vpSelf->team==NULL)
{//û�ж���//������
return;
}
CTeamData& rTeam = *(vpSelf->team);

if(rTeam.Exist(vOtherDBID))
{//�ڶ�����
//������Ϣ���Լ�//�ı�����
SendToRegionOtherLeave(vpSelf,vOtherDBID);
rTeam.DeleteMember(vOtherDBID);

if(rTeam.Size()==1)
{//�����������Զ���ɢ
SendToRegionDeleteTeamMember(vpSelf,vpSelf->userID);
SendToRegionTeamMessage(vpSelf,NOT_MEMBER_TEAM_DISMISS);

delete vpSelf->team;
vpSelf->team=NULL;
}
}
}
*/













