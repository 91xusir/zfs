
#include "region.h"

#include "region_object.h"
#include "scene/rts_sv_scene.h"
#include "region_creature.h"
#include "region_scene.h"
#include "cmd_channel.h"
#include "region_server.h"

#include "master_prentice_base.h"
#include "region_master_list.h"

#define SAFE_READ(p) if(!(p)) return CMD_ERROR_READ_DATA;

int CRegionMasterListManager::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd)//��Client�õ�һ������
{
	//
	char Operation;
	SAFE_READ(cmd->ReadByte(&Operation))

	switch(Operation)
	{
		case C2R_APPLY_ADD_LIST://Ҫ�����
			Add(CmdGiver);
			break;
		case C2R_APPLY_VIEW_LIST:
			GetList(CmdGiver);
			break;
		default:
			return CMD_ERROR_UNKNOWN;
			break;
	}
	return CMD_ERROR_NONE;
}

int CRegionMasterListManager::ReceiveFromGameWorld(DWORD vUserDBID,CG_CmdPacket* cmd)	//��GW�õ�һ������
{
	CRegionUser* pUser=g_region->FindUser(vUserDBID);

	char Operation;
	SAFE_READ(cmd->ReadByte(&Operation))

		switch(Operation)
	{
		case GW2R_APPLY_SUCCESS://�����б�ɹ�
			{
				if(pUser==NULL)
				{
					return CMD_ERROR_UNKNOWN;
				}

				//���ͻ���//����ɹ�
				CG_CmdPacket& rPacket=ClientPacket();
				rPacket.WriteByte(R2C_APPLY_SUCCESS);
				SendToClient(pUser->m_dummy);
			}
			break;
		case GW2R_APPLY_FAIL://�����б�ʧ��
			{
				if(pUser==NULL)
				{
					return CMD_ERROR_UNKNOWN;
				}

				char FailCode; 
				cmd->ReadByte(&FailCode);

				//���ͻ���//����ʧ��
				CG_CmdPacket& rPacket=ClientPacket();
				rPacket.WriteByte(R2C_APPLY_FAIL);
				rPacket.WriteByte(FailCode);
				SendToClient(pUser->m_dummy);
			}
			break;
		case GW2R_LIST_ADD://����һ��ʦ��
			{

				SMasterInfo Temp;
				(*cmd) >> Temp;

				if(Temp.mCamp==FACTION_SHANG)
				{//�������
					if(!mShangList.Exist(Temp.mDBID))
					{
						mShangList += Temp;
					}
				}
				else if(Temp.mCamp==FACTION_ZHOU)
				{
					if(!mZhouList.Exist(Temp.mDBID))
					{
						mZhouList += Temp;
					}
				}
				else if (Temp.mCamp==FACTION_NEUTRAL)
				{
					if (!mNeutralList.Exist(Temp.mDBID))
					{
						mNeutralList += Temp;
					}
				}
				else 
				{
					return CMD_ERROR_UNKNOWN;
				}
			}
			break;
		case GW2R_LIST_DEL://����һ��ʦ��
			{
				long DBID;
				char Camp;

				cmd->ReadByte(&Camp);				//��Ӫ
				cmd->ReadLong(&DBID);				//

				if(Camp==FACTION_SHANG)
				{
					if(mShangList.Exist(DBID))
					{
						mShangList -= DBID;
					}
				}
				else if(Camp==FACTION_ZHOU)
				{
					if(mZhouList.Exist(DBID))
					{
						mZhouList -= DBID;
					}
				}
				else if (Camp==FACTION_NEUTRAL)
				{
					if (mNeutralList.Exist(DBID))
					{
						mNeutralList -= DBID;
					}
				}
				else
				{
					return CMD_ERROR_UNKNOWN;
				}
			}
			break;
		case GW2R_LIST_INFO://ʦͽ�б���Ϣ//��ʱ����Ҫ
			break;
		default:
			return CMD_ERROR_UNKNOWN;
			break;
	}

	return CMD_ERROR_NONE;
}

void CRegionMasterListManager::Add(CRegionCreature* vpCmdGiver)
{
	if(vpCmdGiver->m_userInfo==NULL)
	{//�������
		return;
	}

	//�ȼ������򳬹�����ͽ�ܵĵȼ�//ʧ��//����Ϣ���ͻ���//������
	if(vpCmdGiver->m_core.Lev < PRESYS_MASTER_MIN_LEV)
	{
		CG_CmdPacket& rPacket=ClientPacket();
		rPacket.WriteByte(R2C_APPLY_FAIL);//���ӵ��б�ʧ��
		rPacket.WriteByte(FAIL_LEVEL_LOW);//�ȼ�̫��
		SendToClient(vpCmdGiver);
		return;
	}

	//��ǰͽ�������Ѿ������//ʧ��//����Ϣ���ͻ���//������
	if(vpCmdGiver->m_pFriend->Size(SConnection::STUDENT) >= MAX_STUDENT_COUNT)
	{
		CG_CmdPacket& rPacket=ClientPacket();
		rPacket.WriteByte(R2C_APPLY_FAIL);//���ӵ��б�ʧ��
		rPacket.WriteByte(FAIL_STUDENT_OVERFLOW);//��ǰѧ��̫��
		SendToClient(vpCmdGiver);
		return;
	}


	string Metier;
	SUserActor* pActor = g_TableUserActor.FindUserActor
		(vpCmdGiver->m_actorInfo->Metier, vpCmdGiver->m_actorInfo->MetierLev);
	if (pActor==NULL)
	{
		return;
	}

	Metier = pActor->MetierName;

	//������gw�ύ
	CG_CmdPacket& rPacket=GameWorldPacket();
	rPacket.WriteByte(R2GW_APPLY_ADD_LIST);
	rPacket.WriteLong(vpCmdGiver->m_userInfo->m_userId);//ʦ����DBID
	rPacket.WriteString((char*)Metier.c_str());				//ʦ����ְҵ
	rPacket.WriteByte(vpCmdGiver->m_core.Lev);	//ʦ���ĵȼ�
	rPacket.WriteShort((short int) vpCmdGiver->m_pFriend->Size(SConnection::PRENTICE));//�Ѿ���ʦ��ͽ������
	SendToGameWorld();

}

void CRegionMasterListManager::GetList(CRegionCreature* vpCmdGiver)
{
	CG_CmdPacket& rPacket=ClientPacket();
	rPacket.WriteByte(R2C_LIST_INFO);////ʦͽ�б���Ϣ

	if(vpCmdGiver->m_core.Faction == FACTION_SHANG)
	{
		rPacket << mShangList;
	}
	else if(vpCmdGiver->m_core.Faction == FACTION_ZHOU)
	{
		rPacket << mZhouList;
	}
	/**add by Tianh  09.12.21*ʦͽ��Ӫ*/
	else if (vpCmdGiver->m_core.Faction == FACTION_NEUTRAL)
	{
		rPacket << mNeutralList;
	}
	else
	{
		return;
	}

	SendToClient(vpCmdGiver);
	return;
}

void CRegionMasterListManager::SetMaxCount(int vMax)
{
	mShangList.SetMaxCount(vMax);
	mZhouList.SetMaxCount(vMax);
	//Tianh  09.12.11
	mNeutralList.SetMaxCount(vMax);

	//������gw�ύ
	CG_CmdPacket& rPacket=GameWorldPacket();
	rPacket.WriteByte(R2GW_SET_MAX_COUNT);
	rPacket.WriteLong(vMax);
	SendToGameWorld();

}

CG_CmdPacket& CRegionMasterListManager::GameWorldPacket()
{
	mSendPacket.BeginWrite();
	mSendPacket.WriteShort(r2g_master_list);
	return mSendPacket;
}

CG_CmdPacket& CRegionMasterListManager::ClientPacket()
{
	mSendPacket.BeginWrite();
	mSendPacket.WriteShort(r2c_master_list);
	return mSendPacket;
}


void CRegionMasterListManager::SendToGameWorld()			//��һ������GameWorld
{
	g_region->m_gws.SendPacket(&mSendPacket);
}


void CRegionMasterListManager::SendToClient(CRegionCreature* vpCreature)//��һ������Client
{
	vpCreature->RecvCmd(&mSendPacket);
}						







