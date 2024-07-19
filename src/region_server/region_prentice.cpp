#include "region.h"
#include "master_prentice_base.h"
#include "region_prentice.h"

CRegionPrenticeMgr g_PrenticeMgr;

#define SAFE_READ(p)	{ if(!(p)) return CMD_ERROR_READ_DATA; }
#define SAFE_ASSERT(p)	{ if ( !(p) ) return CMD_ERROR_UNKNOWN; }

CRegionPrenticeMgr::CRegionPrenticeMgr()
{
}

CRegionPrenticeMgr::~CRegionPrenticeMgr()
{
}

bool CRegionPrenticeMgr::OnRecvGW_AddMasterPrentice(CG_CmdPacket *cmd)
{
	short errorCode;
	long index;
	long masterID;
	long prenticeID;
	char* masterName;
	char* prenticeName;
	SAFE_READ(cmd->ReadShort(&errorCode))
	SAFE_READ(cmd->ReadLong(&index))
	SAFE_READ(cmd->ReadLong(&masterID))
	SAFE_READ(cmd->ReadLong(&prenticeID))
	SAFE_READ(cmd->ReadString(&masterName))
	SAFE_READ(cmd->ReadString(&prenticeName))

	CRegionUser *pMaster = g_region->FindUser(masterID);
	CRegionUser *pPrentice = g_region->FindUser(prenticeID);
	if (!pMaster || !pPrentice)
	{
		ERR("[CRegionPrenticeMgr::OnRecvGW_AddMasterPrentice] Master or Prentice is OFFLINE!\n");
		return false;
	}

	g_PrenticeMgr.UnlockUserPrenSys(pMaster);
	g_PrenticeMgr.UnlockUserPrenSys(pPrentice);

	switch (errorCode)
	{
	case prentice_code_success:
		{
			if (Judge_AddMasterPrentice(pMaster->m_dummy, pPrentice->m_dummy))
			{
				pMaster->m_dummy->m_pFriend->AddStudent(index, pPrentice->m_userId, pPrentice->m_username);
				pPrentice->m_dummy->m_pFriend->AddTeacher(index, pMaster->m_userId, pMaster->m_username);

				// 拜师奖励：（15级绿色武器一把）拜师成功之后就增加到徒弟的道具栏，该道具绑定玩家角色若道具栏位已满，就不给了
				static int bonusWeapon[] = {0, 350, 352, 353, 351};
				if (!pPrentice->m_dummy->m_pItemContainer->m_Bag.IsFull())
				{
					int bonusItemIndex = bonusWeapon[pPrentice->m_dummy->m_core.Metier];
					SItemID itemBonus = g_region->m_pItemManager->CreateItem(bonusItemIndex, ItemColor_Green, 0, IB_Binded);
					if (ItemID_IsValid(itemBonus))
					{
						if (!pPrentice->m_dummy->AddItem(itemBonus))
						{
							ERR1("[CRegionPrenticeMgr::OnRecvGW_AddMasterPrentice] Warning: Player %s Add Item Error, Prentice No bonus\n", pPrentice->m_username.c_str());
						}
					}
				}
				else 
				{
					// 不奖励
					ERR1("[CRegionPrenticeMgr::OnRecvGW_AddMasterPrentice] Warning: Player %s Bag Full, Prentice No bonus\n", pPrentice->m_username.c_str());
				}
			}
			else 
			{
				ERR("[CRegionPrenticeMgr::OnRecvGW_AddMasterPrentice] Judge_AddMasterPrentice NOT Pass!\n");
				errorCode = prentice_code_error_unknown;
				break;
			}
		}
		break;
    case prentice_code_time_limit:
        break;
	case prentice_code_error_unknown:
		{
		}
		break;
	default:
		break;
	}
	CRegionPrenticeCmd::Build_r2c_prentice_add_result(errorCode, pMaster->m_username.c_str(), pPrentice->m_username.c_str());
	pMaster->RecvCmd(g_sendCmd);
	pPrentice->RecvCmd(g_sendCmd);

	return true;
}

bool CRegionPrenticeMgr::OnRecvGW_RemoveMasterPrentice(CG_CmdPacket *cmd)
{
	short errorCode;
	long index;
	long masterID;
	long prenticeID;
	char* masterName;
	char* prenticeName;
	SAFE_READ(cmd->ReadShort(&errorCode))
	SAFE_READ(cmd->ReadLong(&index))
	SAFE_READ(cmd->ReadLong(&masterID))
	SAFE_READ(cmd->ReadLong(&prenticeID))
	SAFE_READ(cmd->ReadString(&masterName))
	SAFE_READ(cmd->ReadString(&prenticeName))

	CRegionUser *pMaster = g_region->FindUser(masterID);
	CRegionUser *pPrentice = g_region->FindUser(prenticeID);

	if (pMaster)
		g_PrenticeMgr.UnlockUserPrenSys(pMaster);
	if (pPrentice)
        g_PrenticeMgr.UnlockUserPrenSys(pPrentice);

	switch (errorCode)
	{
	case prentice_code_success:
		{
			if (pMaster && pMaster->m_dummy)
				pMaster->m_dummy->m_pFriend->Remove(SConnection::STUDENT, prenticeID);
			if (pPrentice && pPrentice->m_dummy)
				pPrentice->m_dummy->m_pFriend->Remove(SConnection::TEACHER, masterID);
		}
		break;
	case prentice_code_error_unknown:
		{
		}
		break;
	default:
		break;
	}
	CRegionPrenticeCmd::Build_r2c_prentice_remove_result(errorCode, masterName, prenticeName);
	if (pMaster)
		pMaster->RecvCmd(g_sendCmd);
	if (pPrentice)
		pPrentice->RecvCmd(g_sendCmd);

	return true;
}

bool CRegionPrenticeMgr::OnRecvGW_FinishMasterPrentice(CG_CmdPacket *cmd)
{
	short errorCode;
	long index;
	long masterID;
	long prenticeID;
	char* masterName;
	char* prenticeName;
	long onlineTime;
	SAFE_READ(cmd->ReadShort(&errorCode))
	SAFE_READ(cmd->ReadLong(&index))
	SAFE_READ(cmd->ReadLong(&masterID))
	SAFE_READ(cmd->ReadLong(&prenticeID))
	SAFE_READ(cmd->ReadString(&masterName))
	SAFE_READ(cmd->ReadString(&prenticeName))
	SAFE_READ(cmd->ReadLong(&onlineTime))

	CRegionUser *pMaster = g_region->FindUser(masterID);
	CRegionUser *pPrentice = g_region->FindUser(prenticeID);
	if (!pMaster || !pPrentice)
	{
		ERR("[CRegionPrenticeMgr::OnRecvGW_AddMasterPrentice] Master or Prentice is OFFLINE!\n");
		return false;
	}

	g_PrenticeMgr.UnlockUserPrenSys(pMaster);
	g_PrenticeMgr.UnlockUserPrenSys(pPrentice);

	switch (errorCode)
	{
	case prentice_code_success:
		{
			// 出师奖励
			int prenticeUseTime = onlineTime;
			if (prenticeUseTime > 45*3600)
			{
				static int bonusWeapons[] = {0, 355, 356, 357, 354 };
				pMaster->m_dummy->AddExp(3000000, 0);
				pMaster->m_dummy->AddCredit(16);
				pPrentice->m_dummy->AddExp(1000000, 0);
				int bonusItemIndex = bonusWeapons[pPrentice->m_dummy->m_core.Metier];
				SItemID bonusItem = g_region->m_pItemManager->CreateItem(bonusItemIndex, ItemColor_Blue, 0, IB_Binded);
				if (ItemID_IsValid(bonusItem))
				{
					if (!pPrentice->m_dummy->m_pItemContainer->m_Bag.IsFull())
					{
						if (!pPrentice->m_dummy->AddItem(bonusItem))
						{
						}
					}
					else
					{
						// 不奖励道具
					}
				}
			}
			else 
			{
				pMaster->m_dummy->AddExp(1000000, 0);
				pPrentice->m_dummy->AddExp(1000000, 0);
			}
			// 强制双方存盘
			g_region->m_gws.SaveUserData(pMaster->m_userId, pMaster->m_dummy);
			g_region->m_gws.SaveUserData(pPrentice->m_userId, pPrentice->m_dummy);
		}
		break;
	case prentice_code_error_unknown:
		{
		}
		break;
	default:
		break;
	}
	CRegionPrenticeCmd::Build_r2c_prentice_finish_result(errorCode, pMaster->m_username.c_str(), pPrentice->m_username.c_str());
	pMaster->RecvCmd(g_sendCmd);
	pPrentice->RecvCmd(g_sendCmd);

	return true;
}

bool CRegionPrenticeMgr::Judge_AddMasterPrentice(CRegionCreature* pMaster, CRegionCreature* pPrentice)
{
	if (!pMaster || !pMaster->IsUser() || !pPrentice || !pPrentice->IsUser())
		return false;

	if (!pMaster->m_pTeam)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
        return false;
    }

	if (pMaster->m_pTeam->mData.Size() != 2)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
        return false;
    }

	if (pMaster->m_pTeam->mData.GetCaptain() != pMaster->m_userInfo->m_userId)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
        return false;
    }

	if (pMaster->m_core.Faction != pPrentice->m_core.Faction)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_SAME_FACTION));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_SAME_FACTION));
        return false;
    }

	if ((pPrentice->m_core.Lev<PRESYS_ADD_START_LEV) || (pPrentice->m_core.Lev>PRESYS_ADD_END_LEV))
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_PRENTICE_LEVEL));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_PRENTICE_LEVEL));
		return false;
	}

	if (pMaster->m_core.Lev<PRESYS_MASTER_MIN_LEV)
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_MASTER_LEVEL));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_MASTER_LEVEL));
		return false;
	}

	//师徒关系判断
	DWORD TeacherID=pMaster->m_userInfo->m_userId;
	DWORD StudentID=pPrentice->m_userInfo->m_userId;
	CServerFriend& Teacher=*(pMaster->m_pFriend);
	CServerFriend& Student=*(pPrentice->m_pFriend);

	//老师有这个徒弟，或者，学生数满//返回失败
	if(Teacher.Exist(SConnection::STUDENT,StudentID) || Teacher.Size(SConnection::STUDENT) >=MAX_STUDENT_COUNT)
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION1));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION1));
		return false;
	}
	//老师有这个出师的徒弟
	if (Teacher.Exist(SConnection::PRENTICE,StudentID))
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION2));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION2));
		return false;
	}
	//徒弟已经有老师//返回失败
	if(Student.Size(SConnection::TEACHER)>=MAX_TEACHER_COUNT)
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION3));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION3));
		return false;
	}

	return true;
}


bool CRegionPrenticeMgr::Judge_RemoveMasterPrentice(CRegionCreature* pApplyCre, const char* masterName, const char* prenticeName, long& outIndex, long& outMasterID, long& outPrenticeID)
{
	if (!pApplyCre || !pApplyCre->IsUser())
		return false;

	CRegionUser* pMaster=NULL;
	CRegionUser* pPrentice=NULL;
	if (strcmp(pApplyCre->m_core.Name.c_str(), masterName)==0)
	{
		pMaster= pApplyCre->m_userInfo;
		pPrentice=g_region->FindUser(prenticeName);
	}
	else if (strcmp(pApplyCre->m_core.Name.c_str(), prenticeName)==0)
	{
		pMaster=g_region->FindUser(masterName);
		pPrentice= pApplyCre->m_userInfo;
	}
	else 
	{
		return false;
	}

	if (pMaster==pApplyCre->m_userInfo)
	{
		//老师没有学生//返回失败
		CServerFriend& Teacher=*(pMaster->m_dummy->m_pFriend);
		SConnection* pConnection=Teacher.Find(SConnection::STUDENT, prenticeName);
		if(pConnection==NULL)
		{
            pMaster->m_dummy->SendSystemMessage(R(MSG_PRENSYS_REMOVE_CONDITION1));
			return false;
		}
		outIndex = pConnection->mConnectionIndex;
		outMasterID = pMaster->m_userId;
		outPrenticeID = pConnection->mDBID;
	}
	else if (pPrentice==pApplyCre->m_userInfo)
	{
		//学生没有这个老师//返回失败
		CServerFriend& Student=*(pPrentice->m_dummy->m_pFriend);
		SConnection* pConnection = Student.Find(SConnection::TEACHER, masterName);
		if (pConnection==NULL)
		{
            pPrentice->m_dummy->SendSystemMessage(R(MSG_PRENSYS_REMOVE_CONDITION2));
			return false;
		}
		outIndex = pConnection->mConnectionIndex;
		outMasterID = pConnection->mDBID;
		outPrenticeID = pPrentice->m_userId;
	}

	return true;
}

bool CRegionPrenticeMgr::Judge_FinishMasterPrentice(CRegionCreature* pMaster, CRegionCreature* pPrentice, long& outIndex)
{
	if (!pMaster || !pMaster->IsUser() || !pPrentice || !pPrentice->IsUser())
		return false;

	if (!pMaster->m_pTeam)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION1));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION1));
        return false;
    }

	if (pMaster->m_pTeam->mData.Size() != 2)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION1));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION1));
        return false;
    }

	if (pMaster->m_pTeam->mData.GetCaptain() != pMaster->m_userInfo->m_userId)
    {
        pMaster->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION1));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION1));
        return false;
    }

	if ((pPrentice->m_core.Lev>PRESYS_FINISH_END_LEV) || (pPrentice->m_core.Lev<PRESYS_FINISH_START_LEV))
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION2));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION2));
		return false;
	}

    if (pPrentice->m_pItemContainer->m_Bag.IsFull())
    {
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_ITEMBAG_FULL));
        return false;
    }

	//师徒关系判断
	DWORD TeacherID= pMaster->m_userInfo->m_userId;
	DWORD StudentID= pPrentice->m_userInfo->m_userId;
	CServerFriend& Teacher=*(pMaster->m_pFriend);
	CServerFriend& Student=*(pPrentice->m_pFriend);

	//老师没有这个学生//返回失败
	SConnection* pConnection=Teacher.Find(SConnection::STUDENT,StudentID);
	if(pConnection==NULL)
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION3));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION3));
		return false;
	}

	//学生没有这个老师//返回失败
	if(!Student.Exist(SConnection::TEACHER,TeacherID))
	{
        pMaster->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION4));
        pPrentice->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION4));
		return false;
	}

	outIndex =  pConnection->mConnectionIndex;

	return true;
}

bool CRegionPrenticeMgr::Judge_WithdrawMPGift(CRegionCreature* pMaster, int lev, short& errorCode)
{
	errorCode = prentice_code_error_unknown;

	if (!PrenSysGiftLevel_IsValid(lev))
		return false;

	int nNeedPrenticeCount = PrenSysGiftNeedCount[lev];
	int nPrenticeCount = pMaster->m_pFriend->Size(SConnection::PRENTICE);
	if (nPrenticeCount < nNeedPrenticeCount)
		return false;
	int nBagFree = pMaster->m_pItemContainer->m_Bag.FreeSpace();
	if (nBagFree<PrenSysGiftNeedGrids[lev])
		return false;

	errorCode = prentice_code_success;
	return true;
}

bool CRegionPrenticeMgr::FinishMasterPrentice(CRegionCreature* pMaster, CRegionCreature* pPrentice)
{
	if (!pMaster || !pPrentice || !pMaster->IsUser() || !pPrentice->IsUser())
		return false;

	pMaster->m_pFriend->Remove(SConnection::STUDENT, pPrentice->m_userInfo->m_userId);
	pPrentice->m_pFriend->Remove(SConnection::TEACHER, pMaster->m_userInfo->m_userId);
	pMaster->m_pFriend->AddPrentice(pPrentice->m_userInfo->m_userId, pPrentice->m_userInfo->m_username);
	pPrentice->m_pFriend->AddMaster(pMaster->m_userInfo->m_userId, pMaster->m_userInfo->m_username);

	return true;
}

bool CRegionPrenticeMgr::WithdrawMPGift(CRegionCreature* pMaster, int lev)
{
	if (!pMaster)
		return false;
	if (!PrenSysGiftLevel_IsValid(lev))
		return false;

	switch (lev)
	{
	case 0:
		{
			SItemID item1 = g_region->m_pItemManager->CreateItem(305, ItemColor_Blue, 0, IB_NotBind);
			if (!ItemID_IsValid(item1))
			{
				return false;
			}
			if (!pMaster->AddItem(item1))
			{
				return false;
			}
		}
		break;
	case 1:
		{
			SItemID item1 = g_region->m_pItemManager->CreateItem(305, ItemColor_Golden, 0, IB_BindWhenEquip);
			if (!ItemID_IsValid(item1))
			{
				return false;
			}
			if (!pMaster->AddItem(item1))
			{
				return false;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

void CRegionPrenticeMgr::OnAfterUserLevelup(CRegionCreature* pCre, int LevelupCount)
{
	if (!pCre || !pCre->m_userInfo)
		return;
	int nLevelOrig = pCre->m_core.Lev - LevelupCount;
	if (nLevelOrig<=0)
		return;
	if ((nLevelOrig<PRESYS_ADD_START_LEV) || (nLevelOrig>PRESYS_FINISH_END_LEV))
		return;

	long prenIndex = 0;
	long nCredit = LevelupCount;
	DWORD masterID = 0;

	// 每个人最多只能有一个师父
	std::vector<SConnection*> arrConn;
	int nTeacherCount = pCre->m_pFriend->Find(SConnection::TEACHER, arrConn);
	if (nTeacherCount == 0) // 没有师父
		return;

	SConnection* pConn = arrConn[0];
	prenIndex = pConn->mConnectionIndex;
	masterID = pConn->mDBID;

	// 通知gws师父加声望值
	if (prenIndex>0 && masterID!=0 && nCredit>0)
	{
		CRegionPrenticeCmd::NetSend_r2g_master_prentice_credit(prenIndex, masterID, nCredit);
	}
}

void CRegionPrenticeMgr::OnAfterUserLevelDown(CRegionCreature* pCre,int LevelDownCount)
{
	//待处理
}

bool CRegionPrenticeMgr::IsUserPrenSysLocked(CRegionUser* pUser)
{
	if (!pUser)
		return true;

	DWORD dwNowTick = rtGetMilliseconds();

	if (pUser->m_dwPrenSysLockTick==0)
	{
		return false;
	}
	else 
	{
		if (dwNowTick - pUser->m_dwPrenSysLockTick < 8000) // 8 Seconds Timeout
			return true;
		else 
			return false;
	}
}

void CRegionPrenticeMgr::LockUserPrenSys(CRegionUser* pUser)
{
	if (!pUser)
		return;

	DWORD dwNowTick = rtGetMilliseconds();
	pUser->m_dwPrenSysLockTick = dwNowTick;
}

void CRegionPrenticeMgr::UnlockUserPrenSys(CRegionUser* pUser)
{
	if (!pUser)
		return;

	pUser->m_dwPrenSysLockTick = 0;
}

void CRegionPrenticeMgr::QueryPrenticesOnlineTime(CRegionUser* pUser)
{
	if (!pUser)
		return;

	std::vector<SConnection*> arrConn;
	int nStudentCount = pUser->m_dummy->m_pFriend->Find(SConnection::STUDENT, arrConn);
	if (nStudentCount == 0) // 没有徒弟
		return;
	DWORD prenticeID = 0;
	for (int i=0; i<(int)arrConn.size(); i++)
	{
		SConnection* pConn = arrConn[i];
		prenticeID = pConn->mDBID;
		CRegionPrenticeCmd::NetSend_r2g_prentice_onlinetime(pUser->m_userId, prenticeID);
	}
}


int PrenticeCmdProcess_Svr(CRegionCreature* CmdGiver, CG_CmdPacket *cmd)
{
	short SubCmd;
	if ( !cmd->ReadShort(&SubCmd) )		return CMD_ERROR_UNKNOWN;

	int iResult = CMD_ERROR_NONE; 
	switch(SubCmd)
	{
	case c2r_prentice_add_apply:
		iResult = cmd_c2r_prentice_add_apply(CmdGiver, cmd);
		break;
	case c2r_prentice_add_response:
		iResult = cmd_c2r_prentice_add_response(CmdGiver, cmd);
		break;
	case c2r_prentice_remove:
		iResult = cmd_c2r_prentice_remove(CmdGiver, cmd);
		break;
	case c2r_prentice_finish_apply:
		iResult = cmd_c2r_prentice_finish_apply(CmdGiver, cmd);
		break;
	case c2r_prentice_finish_response:
		iResult = cmd_c2r_prentice_finish_response(CmdGiver, cmd);
		break;
	case c2r_prentice_gift:
		iResult = cmd_c2r_prentice_gift(CmdGiver, cmd);
	default:
		iResult = CMD_ERROR_READ_DATA;
		break;
	}

	return iResult;
}

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_add_apply)
{
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&playerDBID))

	// CHECK OUT 
	CRegionCreature *pMaster = CmdGiver;
	CRegionUser *pPrenticeUser = g_region->FindUser(playerDBID);
	if (!pPrenticeUser)
		return CMD_ERROR_NONE;
	CRegionCreature *pPrentice = pPrenticeUser->m_dummy;
	if (!g_PrenticeMgr.Judge_AddMasterPrentice(pMaster, pPrentice))
	{
		CmdGiver->SendSystemMessage(R(MSG_PRENSYS_ADD_CONDITION));
		return CMD_ERROR_NONE;
	}

	if (g_PrenticeMgr.IsUserPrenSysLocked(pMaster->m_userInfo))
		return CMD_ERROR_NONE;

	CRegionPrenticeCmd::Build_r2c_prentice_add_apply(pMaster->m_oId);
	pPrenticeUser->RecvCmd(g_sendCmd);
	return CMD_ERROR_NONE;
}

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_add_response)
{
	char bAgree;
	long playerOID;
	SAFE_READ(cmd->ReadByte(&bAgree))
	SAFE_READ(cmd->ReadLong(&playerOID))

	// CHECK OUT 
	CRegionCreature *pMaster = CmdGiver->m_scene->FindCreature(playerOID);
	CRegionCreature *pPrentice = CmdGiver;
	if (!pMaster || !pPrentice || !pMaster->m_userInfo ||!pPrentice->m_userInfo)
		return CMD_ERROR_NONE;
	if (bAgree)
	{
		if (!g_PrenticeMgr.Judge_AddMasterPrentice(pMaster, pPrentice))
			return CMD_ERROR_NONE;

		if (g_PrenticeMgr.IsUserPrenSysLocked(pMaster->m_userInfo))
			return CMD_ERROR_NONE;
		if (g_PrenticeMgr.IsUserPrenSysLocked(pPrentice->m_userInfo))
			return CMD_ERROR_NONE;
		g_PrenticeMgr.LockUserPrenSys(pMaster->m_userInfo);
		g_PrenticeMgr.LockUserPrenSys(pPrentice->m_userInfo);

		CRegionPrenticeCmd::NetSend_r2g_add_master_prentice(pMaster->m_userInfo->m_userId, pPrentice->m_userInfo->m_userId, pMaster->m_userInfo->m_username.c_str(), pPrentice->m_userInfo->m_username.c_str());
	}
	else 
	{
		CRegionPrenticeCmd::Build_r2c_prentice_add_result(prentice_code_disagree, pMaster->m_userInfo->m_username.c_str(), pPrentice->m_userInfo->m_username.c_str());
		pMaster->RecvCmd(g_sendCmd);
	}

	return CMD_ERROR_NONE;
}

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_remove)
{
	char* masterName;
	char* prenticeName;
	SAFE_READ(cmd->ReadString(&masterName))
	SAFE_READ(cmd->ReadString(&prenticeName))

	// CHECK OUT 
	long index;
	long masterID, prenticeID;
	if (!g_PrenticeMgr.Judge_RemoveMasterPrentice(CmdGiver, masterName, prenticeName, index, masterID, prenticeID))
	{
		CmdGiver->SendSystemMessage(R(MSG_PRENSYS_REMOVE_CONDITION));
		return CMD_ERROR_NONE;
	}

	if (g_PrenticeMgr.IsUserPrenSysLocked(CmdGiver->m_userInfo))
		return CMD_ERROR_NONE;
	g_PrenticeMgr.LockUserPrenSys(CmdGiver->m_userInfo);

	long applyUserID = CmdGiver->m_userInfo->m_userId;

	CRegionPrenticeCmd::NetSend_r2g_remove_master_prentice(applyUserID, index, masterID, prenticeID, masterName, prenticeName);
	return CMD_ERROR_NONE;
}

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_finish_apply)
{
	long playerDBID;
	SAFE_READ(cmd->ReadLong(&playerDBID))

	// CHECK OUT 
	CRegionCreature* pMaster = CmdGiver;
	CRegionUser *pPrenticeUser = g_region->FindUser(playerDBID);
	if (!pPrenticeUser)
		return CMD_ERROR_NONE;
	CRegionCreature *pPrentice = pPrenticeUser->m_dummy;
	long index;
	if (!g_PrenticeMgr.Judge_FinishMasterPrentice(pMaster, pPrentice, index))
	{
		CmdGiver->SendSystemMessage(R(MSG_PRENSYS_FINISH_CONDITION));
		return CMD_ERROR_NONE;
	}

	CRegionPrenticeCmd::Build_r2c_prentice_finish_apply(pMaster->m_oId);
	pPrenticeUser->RecvCmd(g_sendCmd);
	return CMD_ERROR_NONE;
}

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_finish_response)
{
	char bAgree;
	long playerOID;
	SAFE_READ(cmd->ReadByte(&bAgree))
	SAFE_READ(cmd->ReadLong(&playerOID))

	CRegionCreature* pMaster = CmdGiver->m_scene->FindCreature(playerOID);
	CRegionCreature* pPrentice = CmdGiver;
	if (!pMaster || !pPrentice || !pMaster->IsUser() || !pPrentice->IsUser())
		return CMD_ERROR_NONE;
	if (bAgree)
	{
		long index;
		if (!g_PrenticeMgr.Judge_FinishMasterPrentice(pMaster, pPrentice, index))
			return CMD_ERROR_NONE;

		if (g_PrenticeMgr.IsUserPrenSysLocked(CmdGiver->m_userInfo))
			return CMD_ERROR_NONE;

		if (!g_PrenticeMgr.FinishMasterPrentice(pMaster, pPrentice))
		{
			return CMD_ERROR_NONE;
		}

		g_PrenticeMgr.LockUserPrenSys(CmdGiver->m_userInfo);

		CRegionPrenticeCmd::NetSend_r2g_finish_master_prentice(index, pMaster->m_userInfo->m_userId, pPrentice->m_userInfo->m_userId, pMaster->m_userInfo->m_username.c_str(), pPrentice->m_userInfo->m_username.c_str());
	}
	else 
	{
		CRegionPrenticeCmd::Build_r2c_prentice_finish_result(prentice_code_disagree, pMaster->m_userInfo->m_username.c_str(), pPrentice->m_userInfo->m_username.c_str());
		pMaster->RecvCmd(g_sendCmd);
	}
	return CMD_ERROR_NONE;
}

PRENTICE_CMD_FUNCTION(cmd_c2r_prentice_gift)
{
	long lev;
	SAFE_READ(cmd->ReadLong(&lev))

	short errorCode;
	if (!g_PrenticeMgr.Judge_WithdrawMPGift(CmdGiver, lev, errorCode))
	{
		CmdGiver->SendSystemMessage(R(MSG_PRENSYS_GIFT_CONDITION));
		return CMD_ERROR_NONE;
	}

	if (g_PrenticeMgr.IsUserPrenSysLocked(CmdGiver->m_userInfo))
		return CMD_ERROR_NONE;

	g_PrenticeMgr.LockUserPrenSys(CmdGiver->m_userInfo);

	CRegionPrenticeCmd::NetSend_r2g_master_prentice_gift(CmdGiver->m_userInfo->m_userId, lev);

	return CMD_ERROR_NONE;
}

void CRegionPrenticeCmd::Build_r2c_prentice_add_apply(DWORD playerOID)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_add_apply);
	g_sendCmd->WriteLong(playerOID);
}

void CRegionPrenticeCmd::Build_r2c_prentice_finish_apply(DWORD playerOID)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_finish_apply);
	g_sendCmd->WriteLong(playerOID);
}

void CRegionPrenticeCmd::Build_r2c_prentice_add_result(short errorCode, const char* MasterName, const char* PrenticeName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_add_result);
	g_sendCmd->WriteShort(errorCode);
	g_sendCmd->WriteString((char*)MasterName);
	g_sendCmd->WriteString((char*)PrenticeName);
}

void CRegionPrenticeCmd::Build_r2c_prentice_remove_result(short errorCode, const char* MasterName, const char* PrenticeName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_remove_result);
	g_sendCmd->WriteShort(errorCode);
	g_sendCmd->WriteString((char*)MasterName);
	g_sendCmd->WriteString((char*)PrenticeName);
}

void CRegionPrenticeCmd::Build_r2c_prentice_finish_result(short errorCode, const char* MasterName, const char* PrenticeName)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_finish_result);
	g_sendCmd->WriteShort(errorCode);
	g_sendCmd->WriteString((char*)MasterName);
	g_sendCmd->WriteString((char*)PrenticeName);
}

void CRegionPrenticeCmd::Build_r2c_prentice_onlinetime(long prenticeID, long onlineTime)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_onlinetime);
	g_sendCmd->WriteLong(prenticeID);
	g_sendCmd->WriteLong(onlineTime);
}

void CRegionPrenticeCmd::Build_r2c_prentice_gift_result(short errorCode, long lev)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_master_prentice);
	g_sendCmd->WriteShort(r2c_prentice_gift_result);
	g_sendCmd->WriteShort(errorCode);
	g_sendCmd->WriteLong(lev);
}


void CRegionPrenticeCmd::NetSend_r2g_add_master_prentice(DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_add_master_prentice);
	packet.WriteLong(masterID);
	packet.WriteLong(prenticeID);
	packet.WriteString((char*)masterName);
	packet.WriteString((char*)prenticeName);
	g_region->m_gws.EndSend();
}

void CRegionPrenticeCmd::NetSend_r2g_remove_master_prentice(DWORD applyUserID, long index, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_remove_master_prentice);
	packet.WriteLong(applyUserID);
	packet.WriteLong(index);
	packet.WriteLong(masterID);
	packet.WriteLong(prenticeID);
	packet.WriteString((char*)masterName);
	packet.WriteString((char*)prenticeName);
	g_region->m_gws.EndSend();
}

void CRegionPrenticeCmd::NetSend_r2g_finish_master_prentice(long index, DWORD masterID, DWORD prenticeID, const char* masterName, const char* prenticeName)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_finish_master_prentice);
	packet.WriteLong(index);
	packet.WriteLong(masterID);
	packet.WriteLong(prenticeID);
	packet.WriteString((char*)masterName);
	packet.WriteString((char*)prenticeName);
	g_region->m_gws.EndSend();
}

void CRegionPrenticeCmd::NetSend_r2g_master_prentice_credit(long index, DWORD userID, short count)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_master_prentice_credit);
	packet.WriteLong(index);
	packet.WriteLong(userID);
	packet.WriteShort(count);
	g_region->m_gws.EndSend();
}

void CRegionPrenticeCmd::NetSend_r2g_prentice_onlinetime(long userID, long prenticeID)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_prentice_onlinetime);
	packet.WriteLong(userID);
	packet.WriteLong(prenticeID);
	g_region->m_gws.EndSend();
}

void CRegionPrenticeCmd::NetSend_r2g_master_prentice_gift(long userID, long lev)
{
	CG_CmdPacket& packet = g_region->m_gws.BeginSend();
	packet.WriteShort(r2g_master_prentice_gift);
	packet.WriteLong(userID);
	packet.WriteLong(lev);
	g_region->m_gws.EndSend();
}
