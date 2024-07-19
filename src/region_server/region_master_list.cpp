
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

int CRegionMasterListManager::ReceiveFromClient(CRegionCreature* CmdGiver,CG_CmdPacket *cmd)//从Client得到一个命令
{
	//
	char Operation;
	SAFE_READ(cmd->ReadByte(&Operation))

	switch(Operation)
	{
		case C2R_APPLY_ADD_LIST://要求加入
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

int CRegionMasterListManager::ReceiveFromGameWorld(DWORD vUserDBID,CG_CmdPacket* cmd)	//从GW得到一个命令
{
	CRegionUser* pUser=g_region->FindUser(vUserDBID);

	char Operation;
	SAFE_READ(cmd->ReadByte(&Operation))

		switch(Operation)
	{
		case GW2R_APPLY_SUCCESS://加入列表成功
			{
				if(pUser==NULL)
				{
					return CMD_ERROR_UNKNOWN;
				}

				//到客户端//加入成功
				CG_CmdPacket& rPacket=ClientPacket();
				rPacket.WriteByte(R2C_APPLY_SUCCESS);
				SendToClient(pUser->m_dummy);
			}
			break;
		case GW2R_APPLY_FAIL://加入列表失败
			{
				if(pUser==NULL)
				{
					return CMD_ERROR_UNKNOWN;
				}

				char FailCode; 
				cmd->ReadByte(&FailCode);

				//到客户端//加入失败
				CG_CmdPacket& rPacket=ClientPacket();
				rPacket.WriteByte(R2C_APPLY_FAIL);
				rPacket.WriteByte(FailCode);
				SendToClient(pUser->m_dummy);
			}
			break;
		case GW2R_LIST_ADD://增加一个师傅
			{

				SMasterInfo Temp;
				(*cmd) >> Temp;

				if(Temp.mCamp==FACTION_SHANG)
				{//如果是商
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
		case GW2R_LIST_DEL://减少一个师傅
			{
				long DBID;
				char Camp;

				cmd->ReadByte(&Camp);				//阵营
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
		case GW2R_LIST_INFO://师徒列表信息//暂时不需要
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
	{//不是玩家
		return;
	}

	//等级不到或超过能收徒弟的等级//失败//发消息到客户端//并返回
	if(vpCmdGiver->m_core.Lev < PRESYS_MASTER_MIN_LEV)
	{
		CG_CmdPacket& rPacket=ClientPacket();
		rPacket.WriteByte(R2C_APPLY_FAIL);//增加到列表失败
		rPacket.WriteByte(FAIL_LEVEL_LOW);//等级太低
		SendToClient(vpCmdGiver);
		return;
	}

	//当前徒弟人数已经到最大//失败//发消息到客户端//并返回
	if(vpCmdGiver->m_pFriend->Size(SConnection::STUDENT) >= MAX_STUDENT_COUNT)
	{
		CG_CmdPacket& rPacket=ClientPacket();
		rPacket.WriteByte(R2C_APPLY_FAIL);//增加到列表失败
		rPacket.WriteByte(FAIL_STUDENT_OVERFLOW);//当前学生太多
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

	//继续向gw提交
	CG_CmdPacket& rPacket=GameWorldPacket();
	rPacket.WriteByte(R2GW_APPLY_ADD_LIST);
	rPacket.WriteLong(vpCmdGiver->m_userInfo->m_userId);//师傅的DBID
	rPacket.WriteString((char*)Metier.c_str());				//师傅的职业
	rPacket.WriteByte(vpCmdGiver->m_core.Lev);	//师傅的等级
	rPacket.WriteShort((short int) vpCmdGiver->m_pFriend->Size(SConnection::PRENTICE));//已经出师的徒弟数量
	SendToGameWorld();

}

void CRegionMasterListManager::GetList(CRegionCreature* vpCmdGiver)
{
	CG_CmdPacket& rPacket=ClientPacket();
	rPacket.WriteByte(R2C_LIST_INFO);////师徒列表信息

	if(vpCmdGiver->m_core.Faction == FACTION_SHANG)
	{
		rPacket << mShangList;
	}
	else if(vpCmdGiver->m_core.Faction == FACTION_ZHOU)
	{
		rPacket << mZhouList;
	}
	/**add by Tianh  09.12.21*师徒阵营*/
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

	//继续向gw提交
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


void CRegionMasterListManager::SendToGameWorld()			//送一个包到GameWorld
{
	g_region->m_gws.SendPacket(&mSendPacket);
}


void CRegionMasterListManager::SendToClient(CRegionCreature* vpCreature)//送一个包到Client
{
	vpCreature->RecvCmd(&mSendPacket);
}						







