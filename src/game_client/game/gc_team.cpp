
#include "gc_include.h"

#include "gc_team.h"
#include "gc_team_const.h"

fox::CStrMap					gTeamStr;//�ַ�������

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



//------�Լ�----------
//�Լ�������˼������
void CClientTeam::InviteOther(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID)
	{//�������Լ��������κδ���
		return ;
	}

	if(mData.Empty())
	{//�����ǰû�ж��飬���Է�������//��������������ϴ���Ϣ
		
		SendToServer(C2R_INVITE_OTHER,vOtherRegionID);
		Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
	}
	else
	{//�����ǰ�Ѿ��ж�����
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			if(mData.Size()<MAX_TEAM_MENBER_COUNT)
			{//������������������������ϴ���Ϣ
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
			{//�������������û���������
				Message(gTeamStr[TEAM_COUNT_OVER]);
			}
		}
		else
		{//���Ƕӳ��������û���������
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
	{//�����ǰû�ж��飬���Է�������//��������������ϴ���Ϣ

		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_subsystem_team);
		pPacket->WriteByte(C2R_INVITE_OTHER_BY_NAME);
		pPacket->WriteString(vrOtherName.c_str()); // ���������м�鳤��
		NetSend(pPacket);
		Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
	}
	else
	{
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			if(mData.Size()<MAX_TEAM_MENBER_COUNT)
			{//������������������������ϴ���Ϣ
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
					pPacket->WriteString(vrOtherName.c_str()); // ���������м�鳤��
					NetSend(pPacket);
					m_LimitTimeByName.erase(vrOtherName);
					m_LimitTimeByName.insert(pair<std::string,time_t>(vrOtherName,time(NULL)));
					Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
				}
			}
			else
			{//�������������û���������
				Message(gTeamStr[TEAM_COUNT_OVER]);
			}
		}
		else
		{//���Ƕӳ��������û���������
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	unguard;
}

void CClientTeam::ApplyOtherTeam(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID || !GetPlayer()->m_bTeamOnOff)
	{//�������Լ��������κδ���
		return ;
	}

	if(mData.Empty())
	{//�����ǰû�ж��飬���Է�������//��������������ϴ���Ϣ
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
	{//�����ǰ�Ѿ��ж�����
		// �����û���������
		Message(gTeamStr[NOT_TEAM_CAPTAIN]);
	}
	unguard;
}

void CClientTeam::ApplyOtherTeam(std::string &vrOtherName)
{
	guard;
	if(!strcmp(vrOtherName.c_str(),GetWorld()->m_pPlayer->GetName()) || !GetPlayer()->m_bTeamOnOff)
	{//�������Լ��������κδ���
		return ;
	}

	if(mData.Empty())
	{//�����ǰû�ж��飬���Է�������//��������������ϴ���Ϣ
		time_t curTime = time(NULL);
		if ((curTime-m_LimitTimeByName[vrOtherName]) < 30)
		{
			Message(R(MSG_TEAM_CANTREQUESTINTHIRTY));
		}else{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_subsystem_team);
			pPacket->WriteByte(C2R_APPLY_TEAM_BY_NAME);
			pPacket->WriteString(vrOtherName.c_str()); // ���������м�鳤��
			NetSend(pPacket);
			Message(gTeamStr[TEAM_WAIT_PLAYER_RESPOND]);
			m_LimitTimeByName.erase(vrOtherName);
			m_LimitTimeByName.insert(pair<std::string,time_t>(vrOtherName,time(NULL)));
		}
	}
	else
	{//�����ǰ�Ѿ��ж�����
		// �����û���������
		Message(gTeamStr[NOT_TEAM_CAPTAIN]);
	}
	unguard;
}

//�Լ�ͬ����˵�����
void CClientTeam::AgreeOtherApply(DWORD vOtherRegionID)
{
	guard;

	if(vOtherRegionID==mSelfRegionID)
	{//�������Լ��������κδ���
		return ;
	}

	SendToServer(C2R_AGREE_APPLY,vOtherRegionID);
	unguard;
}
//�Լ��ܾ����˵�����
void CClientTeam::RefuseOtherApply(DWORD vOtherRegionID)
{
	guard;

	if(vOtherRegionID==mSelfRegionID)
	{//�������Լ��������κδ���
		return ;
	}

	SendToServer(C2R_REFUSE_APPLY,vOtherRegionID);
	unguard;
}

void CClientTeam::BanishOtherByRegionID(DWORD vOtherRegionID)
{
	guard;
	if(vOtherRegionID==mSelfRegionID)
	{//������Լ������Լ��뿪����
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//���û�ж��飬�����û�
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//����ж���
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			SendToServer(C2R_BANISH_OTHER_BY_REGION_ID,vOtherRegionID);
		}
		else
		{//�Լ����Ƕӳ��������û�
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return;
	unguard;
}

//�Լ�������˴Ӷ�����
void CClientTeam::BanishOtherByDBID(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{//������Լ������Լ��뿪����
		SelfLeave();
		return;
	}

	if(mData.Empty())
	{//���û�ж��飬�����û�
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//����ж���
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			SendToServer(C2R_BANISH_OTHER,vOtherDBID);
		}
		else
		{//�Լ����Ƕӳ��������û�
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return ;
	unguard;
}



//�Լ��뿪����
void CClientTeam::SelfLeave()
{
	guard;
	if(mData.Empty())
	{//���û�ж���//�����û�
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//����ж���
		//���߷�����
		SendToServer(C2R_SELF_LEAVE,mSelfDBID);
		
		//��ʾ��Ϣ
		Message(mData.GetCaptain(),gTeamStr[SELF_LEFT_TEAM]);

		//ͬʱ�ı䵱ǰ����
		mData.Clear();
		TeamDataChange();
		g_layerMain->m_formTeam->OnRefresh();
	}
	unguard;
}



//�Լ���ɢ����
void CClientTeam::Dismiss()
{
	guard;
	if(mData.Empty())
	{//���û�ж��飬�����û�
		Message(gTeamStr[TEAM_NOT_EXIST]);
	}
	else
	{//�����
		if(mData.GetCaptain()==mSelfDBID)
		{//�Ƕӳ�
			//���߷�����
			SendToServer(C2R_DISMISS,mSelfDBID);
			
			//��ʾ��Ϣ
			Message(gTeamStr[TEAM_DISMISS]);

			//ͬʱ�ı䵱ǰ����
			mData.Clear();
			TeamDataChange();
			if (RTW_WIDGET("lbrequest")->IsVisible())
				RTW_WIDGET("lbrequest")->Hide();
		}
		else
		{	//������Ƕӳ��������û�
			Message(gTeamStr[NOT_TEAM_CAPTAIN]);
		}
	}
	return;
	unguard;
}


//�Լ�ͬ����˵�����
void CClientTeam::AgreeOtherInvite(DWORD vOtherDBID)
{
	guard
	if(vOtherDBID==mSelfDBID)
	{
		return ;
	}

	//���߷�����
	m_iInviteTimes = 0;
	SendToServer(C2R_AGREE_OTHER_INVITE,vOtherDBID);
	unguard;
}

//�Լ��ܾ����˵�����
void CClientTeam::RefuseOtherInvite(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}

	//���߷�����
	m_iInviteTimes = 0;
	SendToServer(C2R_REFUSE_OTHER_INVITE,vOtherDBID);
	unguard;
}



//-------����-----------
//����ͬ��������
void CClientTeam::OtherAgreeInvite(DWORD vOtherDBID,std::string& vrOtherName,char vrOtherhead,byte Metier,long BornTag,long actorID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{//�Լ�ͬ���Լ���������
		return ;
	}

	if(mData.Empty())
	{//������鲻����
		mData.AddMember(mSelfDBID,mSelfName,mSelfHead,Metier,BornTag,actorID);
		g_layerMain->m_formTeam->OnRefresh();
	}

	if(mData.GetCaptain()==mSelfDBID)
	{//�Լ��Ƕӳ�
		//�ı䵱ǰ������
		mData.AddMember(vOtherDBID,vrOtherName,vrOtherhead,Metier,BornTag,actorID);
		g_layerMain->m_formTeam->OnRefresh();
		TeamDataChange();


		//�����û�
		Message(vOtherDBID,gTeamStr[MEMBER_ADD_TEAM]);
	}
	else
	{//���Ƕӳ�
		return;

	}
	unguard;
}

//���˾ܾ��������
void CClientTeam::OtherRefuseInvite(DWORD vOtherDBID,std::string& vrOtherName)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	//�����û�
	std::string Temp(gTeamStr[REFUSE_INVITE]);

	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,vrOtherName);

	Message(Temp);
	unguard;
}

//���������Լ��������
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

	//�����û�
	std::string Temp(gTeamStr[INVITE_ADD_TEAM]);

	std::string::size_type First=Temp.find("%s");
	Temp.replace(First,2,vrOtherName);
	MessageWithChoice(vOtherDBID,Temp);
	unguard;
}

//���������Լ�������
void CClientTeam::OtherBanish(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	if(mData.GetCaptain()==vOtherDBID)
	{
		//�����û���ͬʱ�ı䵱ǰ����
		Message(vOtherDBID,gTeamStr[BANISH_FROM_TEAM]);
		mData.Clear();
		TeamDataChange();
		/* gao 2009.12.25
		����Լ����Ӷ������߳��������Լ��Ķ����б��Ǵ�״̬
		*/
		if(g_layerMain->m_formTeam->IsVisibale())
			g_layerMain->m_formTeam->Hide();
		/* end */
	}
	unguard;
}

//�����뿪����
void CClientTeam::OtherLeave(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	if(mData.Exist(vOtherDBID))
	{
		//�����û���ͬʱ�ı�����
		Message(vOtherDBID,gTeamStr[LEAVE_TEAM]);
		mData.DeleteMember(vOtherDBID);
		g_layerMain->m_formTeam->OnRefresh();
		TeamDataChange();
	}
	unguard;
}


//���˽�ɢ����
void CClientTeam::OtherDismiss(DWORD vOtherDBID)
{
	guard;
	if(vOtherDBID==mSelfDBID)
	{
		return;
	}
	if(mData.GetCaptain()==vOtherDBID)
	{
		//�����û���ͬʱ�ı�����
		Message(gTeamStr[TEAM_DISMISS]);
		mData.Clear();
		TeamDataChange();
	}
	unguard;
}


//������������Լ��Ķ���
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
		// ���������б�
		std::string met;
		switch(metier)
		{
		case 1:
			met = string("��̨");
			break;
		case 2:
			met = string("����");
			break;
		case 3:
			met = string("��ɽ");
			break;
		case 4:
			met = string("�置");
			break;
		default:
			met = string("δ֪");
			break;
		}
		g_layerMain->m_formTeam->OnApplyListInsertNew(vOtherDBID,vrOtherName,vLevel,met,vrAni,vrPow);//������ӱ䶯��ʱ�ɵ�

	}else{
		RefuseOtherApply(vOtherDBID);
	}
	unguard;
}

//����ͬ���Լ�������
void CClientTeam::OtherAgreeApply(DWORD vOtherDBID)
{
	guard;
	m_LimitTime.clear();
	unguard;
}

//���˾ܾ��Լ�������
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


//------�Է�����--------
//�ϴ���Ϣ
void CClientTeam::SendToServer(byte vOperation,DWORD vOtherID)
{
	guard;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_team);
	pPacket->WriteByte(vOperation);

	switch(vOperation)
	{
	case C2R_INVITE_OTHER://�Լ�������˼������
	case C2R_BANISH_OTHER://�Լ�������˴Ӷ�����
	case C2R_AGREE_OTHER_INVITE://�Լ�ͬ����˵�����
	case C2R_REFUSE_OTHER_INVITE://�Լ��ܾ����˵�����
	case C2R_BANISH_OTHER_BY_REGION_ID:
	case C2R_APPLY_TEAM://�������ĳ������
	case C2R_AGREE_APPLY://ͬ���������
	case C2R_REFUSE_APPLY://�ܾ���������
	case C2R_TRANSFER_CAPTAIN:	// �ӳ�ת��
	case C2R_SELECTED_BY_DBID:
		pPacket->WriteLong(vOtherID);
		break;
	case C2R_TEAM_ONOFF:	//��ӿ�����Ϣ,��ʱ���Ĳ��ǽ�ɫID.����0��1,0Ϊ��,1Ϊ��.
	case C2R_SET_PICK_TYPE:
		pPacket->WriteByte(vOtherID);
		break;
	case C2R_SELF_LEAVE://�Լ��뿪����
	case C2R_DISMISS://�Լ���ɢ����
	default:
		break;
	}
	NetSend(pPacket);
	unguard;
}

//�ӵ���Ϣ
void CClientTeam::ReceiveFromServer(CG_CmdPacket *pPacket)
{
	guard;
	byte Operation;
	long OtherDBID;
	READ_byte(Operation);
	
	//������Ϣ��������
	switch(Operation)
	{
	case R2C_ADD_TEAM_MEMBER:			//����һ������
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
	case R2C_DELETE_TEAM_MEMBER:		//����һ������
		READ_long(OtherDBID);
		mData.DeleteMember(OtherDBID);
		g_layerMain->m_formTeam->OnRefresh();
		TeamDataChange();

		break;
	case R2C_UPDATE_ONE_MEMBER_DATA:	//ˢ��ĳ����Ա������
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
			{//û�ҵ� 
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
			{//�ҵ�
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
	case R2C_UPDATE_ALL_MEMBER_DATA:		//ˢ�����г�Ա������
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
	case R2C_TEAM_MESSAGE:				//ˢһ����Ϣ
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
		//-------����-----------
	case R2C_OTHER_AGREE_INVITE:		//����ͬ��������
		{
			char* pOtherName;
			char OtherHead;
			byte metier; //ְҵ
			long BornTag; //��Ф
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
	case R2C_OTHER_REFUSE_INVITE:		//���˾ܾ��������
		{
			char* pOtherName;
			READ_long(OtherDBID);
			READ_string(pOtherName)
			OtherRefuseInvite(OtherDBID,string(pOtherName));
		}
		break;
	case R2C_OTHER_INVITE:				//���������Լ��������
		{
			char* pOtherName;
			READ_long(OtherDBID);
			READ_string(pOtherName);
			OtherInvite(OtherDBID,string(pOtherName));
		}
		break;
	case R2C_OTHER_BANISH:				//���������Լ�������
		READ_long(OtherDBID);
		OtherBanish(OtherDBID);
		break;
	case R2C_OTHER_LEAVE:				//�����뿪����
		READ_long(OtherDBID);
		OtherLeave(OtherDBID);
		break;
	case R2C_OTHER_DISMISS:				//���˽�ɢ����
		READ_long(OtherDBID);
		OtherDismiss(OtherDBID);
		/* gao 2009.12.25
		����Լ����Ӷ������߳��������Լ��Ķ����б��Ǵ�״̬
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
	case R2C_TRANSFER_CAPTAIN://ת�öӳ�
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
{//�иö�Ա//����У��ϴ���������ͬʱ�ı䵱ǰ���ݣ���ʾ��Ϣ
SendToServer(C2R_BANISH_OTHER,vOtherDBID);
Message(vOtherDBID,gTeamStr[BANISH_MEMBER]);
mData.DeleteMember(vOtherDBID);
if(mData.Size()==1)
{//�������㣬�Զ���ɢ����
mData.Clear();
Message(gTeamStr[NOT_MEMBER_TEAM_DISMISS]);
}

TeamDataChange();
}
else
{//û�иö�Ա//�����û�
Message(gTeamStr[MEMBER_NOT_EXIST]);
}
*/


