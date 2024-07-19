#include "login.h"

/******************************* GSUpdateSessionManager ************************************/

struct SUpdateMySort
{
    long             iUserCnt;
    GSUpdateSession* pSession;
};

int updateMySortCompare(const void * p1, const void * p2)
{
    return (((SUpdateMySort*)p1)->iUserCnt - ((SUpdateMySort*)p2)->iUserCnt);
}

void GSUpdateSessionManager::UpdateEvaluationByUserCnt()
{
    SUpdateMySort* pSort = new SUpdateMySort[m_sessions.size()];
    int i=0;
    stlforeach(std::list<GSUpdateSession>, se, m_sessions)
    {
        pSort[i].iUserCnt = se->lUserCnt;
        pSort[i].pSession = &(*se);
        i ++;
    }
    qsort(pSort, m_sessions.size(), sizeof(SUpdateMySort), updateMySortCompare);
    for (i=0; i<(int)m_sessions.size(); i++)
    {
        pSort[i].pSession->cEvaluation = i;
    }
}

int GSUpdateSessionManager::OnReceivedUpdateInfo(CG_CmdPacket* packet)
{
	// MSG("Execute command u2gs_update_info\n");

	char  peerAddr[128];
	short peerPort;
	char* host;
	long port;
	long evaluation;

	if (!packet->ReadString(&host))
		return 1;

	if (!packet->ReadLong(&port))
		return 1;	

	if (!packet->ReadLong(&evaluation))
		return 0;

	if (strcmp(host, "localhost") == 0 ||
		strcmp(host, "127.0.0.1") == 0)
	{
		g_lgs->m_update.GetSession()->session->GetSocket().GetRemoteAddr(peerAddr, &peerPort);

		host = peerAddr;
	}

	GetSession()->host = host;
	GetSession()->port = port;
	GetSession()->lUserCnt = evaluation;

    UpdateEvaluationByUserCnt();

	return 0;
}

bool GSUpdateSessionManager::OnCmd(short cmdID, CG_CmdPacket* packet)
{
	switch(cmdID)
	{
	case u2gs_update_info:
        if (!GetSession()->bChecked)
            return false;
        OnReceivedUpdateInfo(packet);
		break;

    case u2gs_check_version:
        {
            if (GetSession()->bChecked)
                return false;
            char *pExeMD5, *pDataMD5, *pOS, *pFile;
            if (!packet->ReadString(&pExeMD5))
                return false;

            long i, iFilesCnt;
            std::list<std::string> listFiles;
            packet->ReadLong(&iFilesCnt);
            for (i=0; i<iFilesCnt; i++)
            {
                if (!packet->ReadString(&pFile))
                    return false;
                listFiles.push_back(pFile);
            }

            if (!packet->ReadString(&pDataMD5))
                return false;
            if (!packet->ReadString(&pOS))
                return false;
            bool bSame = true;
            stlforeach(std::list<GSUpdateSession>, se, m_sessions)
            {
                if ((*se).bChecked)
                {
                    if (strcmp(pOS, (*se).strOS.c_str())==0)
                    {
                        if (strcmp(pExeMD5, (*se).strExeMD5.c_str())!=0)
                            bSame = false;
                        if (strcmp(pDataMD5, (*se).strDataMD5.c_str())!=0)
                            bSame = false;
                    }
                }
            }
            if (bSame)
            {
                GetSession()->strExeMD5 = pExeMD5;
                GetSession()->strDataMD5 = pDataMD5;
                GetSession()->listFiles = listFiles;
                GetSession()->strOS = pOS;
                GetSession()->bChecked = true;
            }
            CG_CmdPacket& cmd = g_lgs->m_update.BeginCmd(gs2u_check_version_ret);
            cmd.WriteByte(bSame);
            g_lgs->m_update.SendCmd();
            LOG1("Update server check version %s\n", (bSame?"SUCCEEDED":"FAILED"));
        }
        break;

	default:
        LOG1("Update session: Invalid command %d\n", cmdID);
        return false;
	}
    return true;
}

bool GSUpdateSessionManager::OnAccept(CG_TCPSession* session)
{
	char  peerAddr[128];
	unsigned long peerIP;
	short peerPort;

	session->GetSocket().GetRemoteAddr(peerAddr, &peerPort, &peerIP);
    session->SetDebugStr("UpdateSession");

	peerIP = InvertIPOrder(peerIP);
    if (!g_cfg.listenUpdateServer.allowIP.CheckAllow(peerIP))
    {
        LOG2("Access denied for session [%s:%u]\n", peerAddr, peerPort);
        return false;
    }
    MSG2("Access accept update session [%s:%u]\n", peerAddr, peerPort);
	return true;
}

/******************************** GSClientSessionManager *********************************/

GSClientSessionManager::GSClientSessionManager()
{
    m_sendPacket.SetSize(MAX_CMD_PACKET_SIZE);
    m_bAllowAllIP = false;
    m_allowUser.AddUser("administrator", "976d1f58a33acda99927503239473a4c", 0xFFFFFFFF);
}

bool GSClientSessionManager::OnAccept(CG_TCPSession* session)
{
    if (!m_bAllowAllIP)
    {
        char  peerAddr[128];
        unsigned long peerIP;
        short peerPort;
        session->GetSocket().GetRemoteAddr(peerAddr, &peerPort, &peerIP);
        session->SetDebugStr("ClientSession");
        peerIP = InvertIPOrder(peerIP);
        if (!m_allowIP.CheckAllow(peerIP))
        {
            LOG2("Access denied for session [%s:%u]\n", peerAddr, peerPort);
            return false;
        }
    }
    return true;
}

int GSClientSessionManager::OnRequestGWServerList(CG_CmdPacket* pPacket)
{
	// MSG("Execute command c2gs_request_gws_list\n");

    if (m_now - GetSession()->lastRequestTime > 1) // 最小请求单位为 1 秒
    {
		short iTmp = 0;
		if (pPacket->ReadShort(&iTmp) && iTmp == 1)	// Old server
			g_lgs->SendGWServerList(this, 1);
		else
			g_lgs->SendGWServerList(this);

        GetSession()->lastRequestTime = m_now;
    }
	return 0;
}

int GSClientSessionManager::OnRequestUpdateServerList(CG_CmdPacket*)
{
	// MSG("Execute command c2gs_request_us_list\n");
    if ((m_now - GetSession()->lastRequestTime) > 1) // 最小请求单位为 1 秒
    {
        GetSession()->lastRequestTime = m_now;
	    g_lgs->SendUpdateServerList(this);
    }
	return 0;
}

bool GSClientSessionManager::OnMoniterBase(CG_CmdPacket* packet)
{
    long lCmdID;
    char* pUsername;
    char* pPassword;
    char szUsername[100], szPassword[100];

    packet->ReadLong(&lCmdID);
    if (lCmdID!=790213)
    {
        LOG1("Access denied for monitor user [unknown] at %d\n", rtTimestamp());
        return false;
    }

    packet->ReadString(&pUsername);
    strncpy(szUsername, pUsername, 99);
    szUsername[99] = 0;

    packet->ReadString(&pPassword);
    strncpy(szPassword, pPassword, 99);
    szPassword[99] = 0;

    long lPrivileges = m_allowUser.CheckUser(szUsername, szPassword);
    if (lPrivileges==0)
    {
        LOG2("Access denied for monitor user [%s] at %d", szUsername, rtTimestamp());
        return false;
    }

    GSClientSession* pClentSession = GetSession();
    strncpy(pClentSession->szUsername, pUsername, 39);
    pClentSession->szUsername[39] = 0;
    pClentSession->iState = 1;
    pClentSession->lPrivileges = lPrivileges;
    pClentSession->Log2("Monitor user [%s] logined at %s");
    return true;
}

bool GSClientSessionManager::OnMoniterQueryInfo(CG_CmdPacket* packet)
{
    if ((m_now - GetSession()->lastRequestTime) > 5) // 最小请求单位为 5 秒
    {
        GetSession()->lastRequestTime = m_now;
        g_lgs->SendServerInfoToMoniter(this);
    }
    return true;
}

bool GSClientSessionManager::OnMoniterReboot(CG_CmdPacket* packet)
{
    return true;
}

bool GSClientSessionManager::OnMoniterShutdown(CG_CmdPacket* packet)
{
    long lDelay = 0;
    char* strCon;
    if (!packet->ReadLong(&lDelay))
        return false;
    if (!packet->ReadString(&strCon))
        return false;
    g_lgs->StartAutoBulletin(lDelay, strCon);
    return true;
}

bool GSClientSessionManager::OnMoniterBulletin(CG_CmdPacket* packet)
{
    char cStyle;
    char* strCon;

    if (!packet->ReadByte(&cStyle))
        return false;

    if (!packet->ReadString(&strCon))
        return false;

    g_lgs->SendBulletinToAllGWS(strCon, cStyle);

    return true;
}

bool GSClientSessionManager::OnMoniterSetStatus(CG_CmdPacket* packet)
{
    long lState;

    if (!packet->ReadLong(&lState))
        return false;

    // 设置状态
    //     0x0001 普通登陆
    //     0x0002 GM登陆
    //     0x0004 更新
    g_lgs->SetServerStatus(lState);

    return true;
}

bool GSClientSessionManager::OnCmd(short cmdID, CG_CmdPacket* packet)
{
	switch(cmdID)
	{
	case c2gs_query_gws_list:
		OnRequestGWServerList(packet);
		break;

	case c2gs_query_us_list:
		OnRequestUpdateServerList(packet);
		break;

    case c2gs_query:
        //OnRequestString(packet);
        break;

    case m2gs_base:
        if (!OnMoniterBase(packet))
            return false;
        break;

    default:
        if (GetSession()->iState!=1)
            return false;
        switch (cmdID)
        {
        case m2gs_query_info:
            if (!OnMoniterQueryInfo(packet))
                return false;
            break;

        case m2gs_reboot:
            GetSession()->Log2("Monitor user [%s] reboot at %s");
            if (!OnMoniterReboot(packet))
                return false;
            break;

        case m2gs_shutdown:
            GetSession()->Log2("Monitor user [%s] shutdown at %s");
            if (!OnMoniterShutdown(packet))
                return false;
            break;

        case m2gs_bulletin:
            GetSession()->Log2("Monitor user [%s] bulletin at %s");
            if (!OnMoniterBulletin(packet))
                return false;
            break;

        case m2gs_setstate:
            GetSession()->Log2("Monitor user [%s] set state at %s");
            if (!OnMoniterSetStatus(packet))
                return false;
            break;

        case m2gs_tester:
            g_lgs->TestServer(this, GetSession()->iIdx);
            break;

        default:
            LOG1("Client session: Invalid command %d\n", cmdID);
            return false;
        }
	}
    return true;
}
