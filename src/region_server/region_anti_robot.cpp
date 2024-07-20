#include "region.h"
#include "region_anti_robot.h"
#include "rs_national_war.h"

CAntiRobot g_AntiRobot;

CAntiRobot::CAntiRobot()
{
	m_bEnable = true;

	m_nSendInterval = ANTIROBOT_SENDPIC_INTERVAL;
	m_nSendIntervalRand = ANTIROBOT_SENDPIC_INTERVAL_RAND;
	m_nLockAccountSeconds = ANTIROBOT_KICK_ACCOUNT_TIME;
	m_nResponseMaxTime = ANTIROBOT_RESPONSE_MAX_TIME;

	m_nLockAccCounter = 0;
}

CAntiRobot::~CAntiRobot()
{
}


void CAntiRobot::Run()
{
	if (!m_bEnable)
		return;

	DWORD dwNowTick = rtGetMilliseconds();
	int deltaSend = 0;
	int deltaRecv = 0;
	int i = 0;

	static int dwLastRunTick = 0;
	if (dwNowTick - dwLastRunTick < 800)
	{
		return;
	}
	dwLastRunTick = dwNowTick;

	std::vector<DWORD> kickList;
	CRegionUser *pUser = NULL;
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;

		// 是否忽略该玩家
		if (!FindInSuspiciousList(pUser->m_userId) && CanIgnoreUser(pUser))
		{
			continue;
		}

		switch (pUser->m_ArStatus)
		{
		case ARS_Normal:
			{
				if (pUser->m_ArSendTick != 0)
				{
					if ((long)dwNowTick > pUser->m_ArSendTick)
					{
						SendRequestToUser(pUser);
					}
				}
				else 
				{
					SetNextSendTick(pUser);
				}
			}
			break;
		case ARS_Asking_1:
        case ARS_Asking_2:
			{
				if (pUser->m_ArSendTick==0)
				{
					ERR("[CAntiRobot::Run] User State Error1!\n");
					ChangeState(pUser, ARS_Normal);
				}

				int deltaTick = dwNowTick - pUser->m_ArSendTick;
				if (deltaTick > m_nResponseMaxTime*1000)
				{
					ChangeState(pUser, ARS_Lock);
				}
			}
			break;
		case ARS_Lock:
			{
			}
			break;
		default:
			{
				ERR2("[CAntiRobot::Run] User AntiRobot State Error!(user=%s,state=%d)\n", pUser->m_username.c_str(), pUser->m_ArStatus);
			}
			break;
		}
	}

	DWORD tmpTick = rtGetMilliseconds() - dwNowTick;
	if (tmpTick > 100)
		LOG1("AntiRobot Frame Use Time %d\n", tmpTick);
}

void CAntiRobot::KickRobotUser(DWORD userID)
{
	CRegionUser* pUser = g_region->FindUser(userID);
	if (!pUser)
		return;

	g_region->KickUser(userID, 0, true, true,WG_INFO_ROBOT_KICK);

	time_t now;
	time(&now);
	struct tm *en = localtime(&now);
	LOG5("Kick a RobotUser(user=%s,time=%d-%d %d:%d)\n", pUser->m_username.c_str(), en->tm_mon, en->tm_yday, en->tm_hour, en->tm_min);
}

void CAntiRobot::LockRobotUser(DWORD userID)
{
	CRegionUser* pUser = g_region->FindUser(userID);
	if (!pUser)
		return;

	pUser->m_dummy->AddInvincibility();
	if (pUser->m_dummy->m_pet)
		pUser->m_dummy->m_pet->AddInvincibility();

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_lock_user);
	g_sendCmd->WriteByte(1);
	pUser->RecvCmd(g_sendCmd);

	time_t now;
	time(&now);
	struct tm *en = localtime(&now);
	//LOG5("Lock a RobotUser(user=%s,time=%d-%d %d:%d)\n", pUser->m_username.c_str(), en->tm_mon, en->tm_yday, en->tm_hour, en->tm_min);
}

void CAntiRobot::UnlockRobotUser(DWORD userID)
{
	CRegionUser* pUser = g_region->FindUser(userID);
	if (!pUser)
		return;

	pUser->m_dummy->RemoveInvincibility();
	if (pUser->m_dummy->m_pet)
		pUser->m_dummy->m_pet->RemoveInvincibility();

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_lock_user);
	g_sendCmd->WriteByte(2);
	pUser->RecvCmd(g_sendCmd);

	time_t now;
	time(&now);
	struct tm *en = localtime(&now);
	//LOG5("Unlock a RobotUser(user=%s,time=%d-%d %d:%d)\n", pUser->m_username.c_str(), en->tm_mon, en->tm_yday, en->tm_hour, en->tm_min);
}

void CAntiRobot::DumpData()
{
	DWORD dwNowTick = rtGetMilliseconds();
	DWORD deltaSend = 0;
	DWORD deltaRecv = 0;

	LOG("----------------------------DUMP AntiRobot Data----------------------------\n");
	LOG1("State=%s    ", m_bEnable?"On":"Off");
	LOG1("NowTick=%d\n", dwNowTick);
	LOG("UserName\tStatus\tResendCount\tErrorCount\n");
	CRegionUser *pUser = NULL;
	EXT_SPACE::unordered_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;
		LOG4("%s\t\t%d\t\t%d\t\t%d\n", pUser->m_username.c_str(), pUser->m_ArStatus, pUser->m_ArResendCount, pUser->m_ArErrorCount);
	}
	LOG1("Users Ignored(count=%d):\n", (int)m_UsersIgnored.size());
	std::list<DWORD>::iterator iterList = m_UsersIgnored.begin();
	for (; iterList!=m_UsersIgnored.end(); iterList++)
	{
		DWORD usrID = *iterList;
		CRegionUser *pUser = g_region->FindUser(usrID);
		if (pUser)
		{
			LOG2("%d(%s)\n", usrID, pUser->m_username.c_str());
		}
		else
		{
			LOG1("%d\n", usrID);
		}
	}
	LOG1("Users Suspicious(count=%d):\n", (int)m_UsersSuspicious.size());
	iterList = m_UsersSuspicious.begin();
	for (; iterList!=m_UsersSuspicious.end(); iterList++)
	{
		DWORD usrID = *iterList;
		CRegionUser *pUser = g_region->FindUser(usrID);
		if (pUser)
		{
			LOG2("%d(%s)\n", usrID, pUser->m_username.c_str());
		}
		else
		{
			LOG1("%d\n", usrID);
		}
	}
	LOG("---------------------------------------------------------------------------\n");
}

std::string CAntiRobot::GetStatusString()
{
	char cTmp1024[1024];
	rt2_snprintf(cTmp1024, 1023, "status=%s,SendInterval=%d,SendIntervalRand=%d,LockAccTime=%d(minutes),LockAccCounter=%d", m_bEnable?"On":"Off", m_nSendInterval, m_nSendIntervalRand, m_nLockAccountSeconds/60, m_nLockAccCounter);
	cTmp1024[1023] = '\0';
	return cTmp1024;
}

void CAntiRobot::OnAfterAddUser(CRegionUser* pUser)
{
	if (!pUser)
		return;

	InitAntiRobotData(pUser);

	if (FindInSuspiciousList(pUser->m_userId))
	{
		SendRequestToUser(pUser, false, ARS_Asking_2);
	}
}

void CAntiRobot::OnBeforeRemoveUser(CRegionUser* pUser)
{
	if (!pUser)
		return;

	if (pUser->m_ArStatus==ARS_Lock || pUser->m_ArStatus==ARS_Asking_1)
		AddUserToSuspiciousList(pUser->m_userId);
}

void CAntiRobot::OnRecvUserReply(CRegionUser* pUser, CG_CmdPacket* pPacket)
{
	if (!pUser)
		return;

	char type;
	char* CharctersReply;

	if (!pPacket->ReadByte(&type))
		return;

    // 是选择题还是填空题
    EAntiRobotState RecvType = ARS_Asking_1;
    if (pUser->m_ArSendCharacters.size() == 3)
        RecvType = ARS_Asking_2;

	if (type==0) // 回答问题
	{
		if (!pPacket->ReadString(&CharctersReply))
			return;

		DWORD dwNowTick = rtGetMilliseconds();
		bool bReplyOK = false;

		if (strcmp(pUser->m_ArSendCharacters.c_str(), CharctersReply) == 0)
			bReplyOK = true;
		else 
			bReplyOK = false;

		if (bReplyOK)
		{
			SendResultToUser(pUser, 0);

			RemoveFromSuspiciousList(pUser->m_userId);

			int nExpBonus = 0;
			if (rand()%5000 == 0)
			{
				if (pUser->m_dummy->m_core.Lev > 60)
					nExpBonus = 1680000;
				else if (pUser->m_dummy->m_core.Lev > 40)
					nExpBonus = 880000;
				else if (pUser->m_dummy->m_core.Lev > 20)
					nExpBonus = 280000;
				else 
				{
					nExpBonus = g_TableExp.GetNextLevExp(pUser->m_dummy->m_core.Lev) - pUser->m_dummy->m_core.Exp + 1;
				}
				char cTmp512[512];
				rt2_sprintf(cTmp512, R(MSG_ANTI_HORTATION), pUser->m_username.c_str(), nExpBonus);
				g_region->m_gws.BroadcastBulletin(cTmp512);
			}
			else 
			{
				nExpBonus = 50;
			}
			pUser->m_dummy->AddExp(nExpBonus, 0);

			ChangeState(pUser, ARS_Normal);
		}
		else 
		{
			pUser->m_ArErrorCount++;
			if (pUser->m_ArErrorCount >= ANTIROBOT_ERROR_LOCK_ACCOUNT)
			{
				KickRobotUser(pUser->m_userId);
				if (m_nLockAccountSeconds > 0)
				{
					pUser->LockAccount(1, m_nLockAccountSeconds);
					m_nLockAccCounter++;
				}
			}
			else 
			{
				SendResultToUser(pUser, 1);
				SendRequestToUser(pUser, true, RecvType);
			}
		}
	}
	else if (type==1) // Ask for another string
	{
		if (pUser->m_ArResendCount >= ANTIROBOT_RESEND_MAX_COUNT)
		{
			return;
		}
        else if (pUser->m_ArStatus == ARS_Normal)
        {
            return;
        }
		else 
		{
			SendRequestToUser(pUser, true, RecvType);
		}
	}
	else // Invalid
	{
		return;
	}
}

void CAntiRobot::SendRequestToUser(CRegionUser* pUser, bool bResend/* = false*/, EAntiRobotState type/* = ARS_Asking_1*/)
{
    if (!m_bEnableSendPicture)
        return;
	if (!pUser)
		return;

	static char szRandomImageCharList[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static char szRandomImageCharMask[] = "0OIL1Q8S5";
	//S_MEMDEF(szRandomImageCharList, 37)
	//S_MEMDEF(szRandomImageCharMask, 10)
	//S_MEMPROTECTOR(szRandomImageCharList, 37, bMP1)
	//S_MEMPROTECTOR(szRandomImageCharMask, 10, bMP2)
	rt2_snprintf(szRandomImageCharList, 37, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	rt2_snprintf(szRandomImageCharMask, 10, "0OIL1Q8S5");

	DWORD nowTick = rtGetMilliseconds();

	CRegionServer::SRandomImage* pImg = g_region->CreateRandomImage();
	if (!pImg || strlen(pImg->szStrBuf)!=3 || pImg->iDataSize<=0)
	{
		ERR("[CAntiRobot::SendRequestToUser] Create Random Image Error! Now Disable AntiRobot!\n");
		Disable();
		return;
	}

	int i = 0;
	std::string strTmp = pImg->szStrBuf;
	int nSelectNum = rand() % 3;
	char cSendCharacter = strTmp[nSelectNum];
	int nTmp1 = rand() % 8;
	std::string strSelection;
	i = 0;
	while (i<nTmp1)
	{
		char cTmp = szRandomImageCharList[rand()%36];
		bool bAdd = true;
		if (cTmp==cSendCharacter)
			bAdd = false;
		else if (strSelection.find(cTmp) != string::npos)
			bAdd = false;
		else if (strchr(szRandomImageCharMask, cTmp) != NULL)
			bAdd = false;
		if (bAdd)
		{
			strSelection.push_back(cTmp);
			i++;
		}
	}
	strSelection.push_back(cSendCharacter);
	i = nTmp1 + 1;
	while (i<8)
	{
		char cTmp = szRandomImageCharList[rand()%36];
		bool bAdd = true;
		if (cTmp==cSendCharacter)
			bAdd = false;
		else if (strSelection.find(cTmp) != string::npos)
			bAdd = false;
		else if (strchr(szRandomImageCharMask, cTmp) != NULL)
			bAdd = false;
		if (bAdd)
		{
			strSelection.push_back(cTmp);
			i++;
		}
	}
	nSelectNum++;

    if (type == ARS_Asking_1)
	    pUser->m_ArSendCharacters = cSendCharacter;
    else if (type == ARS_Asking_2)
    {
        pUser->m_ArSendCharacters = pImg->szStrBuf;
        nSelectNum = -1;
    }

	if (bResend)
	{
		if (pUser->m_ArResendCount < ANTIROBOT_RESEND_MAX_COUNT)
			pUser->m_ArResendCount ++;
		//--------add start by tony 06.05.24-------------//
		pUser->m_ArSendTick = nowTick;
		//--------add end   by tony 06.05.24-------------//
	}
	else 
	{
		pUser->m_ArResendCount = 0;
		pUser->m_ArSendTick = nowTick;
	}

	if (pUser->m_ArStatus == ARS_Normal)
    {
        ChangeState(pUser, type);
    }

	int nSecondsLeft = (m_nResponseMaxTime*1000 - (nowTick - pUser->m_ArSendTick) - 800) / 1000;
	int nResendLeft = ANTIROBOT_RESEND_MAX_COUNT - pUser->m_ArResendCount;
	int nErrorLeft = ANTIROBOT_ERROR_LOCK_ACCOUNT - pUser->m_ArErrorCount;
	if (nSecondsLeft < 0)
		nSecondsLeft = 0;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_check_question);
	g_sendCmd->WriteByte(0);								// 0:Ask  1:Result
	g_sendCmd->WriteBinary((char*)(pImg->szDataBuf), pImg->iDataSize);	// 图片内容
	g_sendCmd->WriteString((char*)strSelection.c_str());	// 选择内容
	g_sendCmd->WriteShort(nSelectNum);						// 选择第几个字母(-1为填空题，3个数字都要填)
	g_sendCmd->WriteShort(nSecondsLeft);					// 剩余时间
	g_sendCmd->WriteShort(nResendLeft);						// 可重发次数
	g_sendCmd->WriteShort(nErrorLeft);						// 可识别错误次数
	pUser->RecvCmd(g_sendCmd);

	DWORD endTick = rtGetMilliseconds();
}

void CAntiRobot::SendResultToUser(CRegionUser* pUser, short nErrorCode)
{
	if (!pUser)
		return;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_check_question);
	g_sendCmd->WriteByte(1); // 0:Ask  1:Result
	g_sendCmd->WriteShort(nErrorCode);
	pUser->RecvCmd(g_sendCmd);
}

bool CAntiRobot::CanIgnoreUser(CRegionUser* pUser)
{
	if (!pUser)
		return true;

	if (FindInIgnoredList(pUser->m_userId))
		return true;

	if (pUser->m_dummy->m_pPShop)
		return true;

	if (pUser->m_dummy->m_eCurPosPKAttr==PKA_SAFEST || pUser->m_dummy->m_eCurPosPKAttr==PKA_SAFE)
		return true;

    if (g_WarfieldMgr.IsInWarfield(pUser->m_dummy))
        return true;
    
    if (pUser->m_dummy->m_dungeonUID)
        return true;

	return false;
}

void CAntiRobot::InitAntiRobotData(CRegionUser* pUser)
{
	if (!pUser)
		return;

	pUser->m_ArStatus = ARS_Normal;
	pUser->m_ArSendTick = 0;
	pUser->m_ArResendCount = 0;
	pUser->m_ArSendCharacters = "";
	pUser->m_ArErrorCount = 0;
}

bool CAntiRobot::ChangeState(CRegionUser *pUser, EAntiRobotState NewState)
{
	if (!pUser)
		return false;

	switch (NewState)
	{
	case ARS_Normal:
		pUser->m_ArResendCount = 0;
		pUser->m_ArSendCharacters = "";
		pUser->m_ArErrorCount = 0;
		if (pUser->m_ArStatus==ARS_Asking_1 || pUser->m_ArStatus==ARS_Asking_2)
		{
			SetNextSendTick(pUser);
		}
		else if (pUser->m_ArStatus==ARS_Lock)
		{
			SetNextSendTick(pUser);
			UnlockRobotUser(pUser->m_userId);
		}
		else if (pUser->m_ArStatus==ARS_Normal)
		{
		}
		else 
		{
			return false;
		}
		break;
	case ARS_Asking_1:
    case ARS_Asking_2:
		if (pUser->m_ArStatus==ARS_Normal)
		{
		}
		else if (pUser->m_ArStatus==ARS_Lock)
		{
			UnlockRobotUser(pUser->m_userId);
		}
		else 
		{
			return false;
		}
		break;
	case ARS_Lock:
		if (pUser->m_ArStatus==ARS_Asking_1 || pUser->m_ArStatus==ARS_Asking_2)
		{
			LockRobotUser(pUser->m_userId);
		}
		else if (pUser->m_ArStatus==ARS_Normal)
		{
			LockRobotUser(pUser->m_userId);
		}
		else if (pUser->m_ArStatus==ARS_Lock)
		{
		}
		else 
		{
			return false;
		}
		break;
	default:
		break;
	}

	pUser->m_ArStatus = NewState;
	return true;
}

void CAntiRobot::SetNextSendTick(CRegionUser *pUser)
{
	if (!pUser)
		return;

	if (pUser->m_ArSendTick==0)
	{
		DWORD dwNowTick = rtGetMilliseconds();
		DWORD nNextTick = dwNowTick + m_nSendInterval/4*1000;
		nNextTick += rand() % (m_nSendIntervalRand/6*1000*2+1) - m_nSendIntervalRand/6*1000;
		pUser->m_ArSendTick = nNextTick;
	}
	else
	{
		DWORD dwNowTick = rtGetMilliseconds();
		DWORD nNextTick = dwNowTick + m_nSendInterval*1000;
		nNextTick += rand() % (m_nSendIntervalRand*1000*2+1) - m_nSendIntervalRand*1000;
		pUser->m_ArSendTick = nNextTick;
	}
}

void CAntiRobot::AddUserToIgnoredList(DWORD userID)
{
	if (!FindInIgnoredList(userID))
	{
		m_UsersIgnored.push_back(userID);
	}
}

bool CAntiRobot::FindInIgnoredList(DWORD userID)
{
	std::list<DWORD>::iterator iter = m_UsersIgnored.begin();
	for (; iter!=m_UsersIgnored.end(); iter++)
	{
		if (userID == (*iter))
			break;
	}
	if (iter==m_UsersIgnored.end())
		return false;
	else 
		return true;
}

void CAntiRobot::RemoveFromIgnoredList(DWORD userID)
{
	std::list<DWORD>::iterator iter = m_UsersIgnored.begin();
	for (; iter!=m_UsersIgnored.end(); iter++)
	{
		if (userID == (*iter))
			break;
	}
	if (iter!=m_UsersIgnored.end())
		m_UsersIgnored.erase(iter);
}

void CAntiRobot::AddUserToSuspiciousList(DWORD userID)
{
	if (!FindInIgnoredList(userID))
	{
		m_UsersSuspicious.push_back(userID);
	}
}

bool CAntiRobot::FindInSuspiciousList(DWORD userID)
{
	std::list<DWORD>::iterator iter = m_UsersSuspicious.begin();
	for (; iter!=m_UsersSuspicious.end(); iter++)
	{
		if (userID == (*iter))
			break;
	}
	if (iter==m_UsersSuspicious.end())
		return false;
	else 
		return true;
}

void CAntiRobot::RemoveFromSuspiciousList(DWORD userID)
{
	std::list<DWORD>::iterator iter = m_UsersSuspicious.begin();
	for (; iter!=m_UsersSuspicious.end(); iter++)
	{
		if (userID == (*iter))
			break;
	}
	if (iter!=m_UsersSuspicious.end())
		m_UsersSuspicious.erase(iter);
}

