#include "login.h"
#include "license_check.h"

CGameWorldStub::CGameWorldStub()
{
    m_lStubIdx          = 0;
	m_netLink           = NULL;
	m_stubState         = STUB_STATE_NONE;
    m_userCount         = 0;
    m_evaluation        = 0;
    m_gwState           = gwssNone;
    m_bVersionChecked   = false;
    m_bRegistered       = false;
    m_szName            = "unregister";
    m_cTestState        = 0;

    m_packet            = new CG_CmdPacket;
    m_packet->SetSize(MAX_CMD_PACKET_SIZE);
}

CGameWorldStub::~CGameWorldStub()
{

}

void CGameWorldStub::SetNetLink(CG_TCPSession *link)
{
	delete m_netLink;
	m_netLink = link;
	m_stubState = STUB_STATE_ALIVE;
    if (m_netLink) m_netLink->SetDebugStr("GWStubSession");
}

void CGameWorldStub::SendCmd(CG_CmdPacket *cmd)
{
	if(m_netLink)
		m_netLink->SendPacket(cmd);
}

void CGameWorldStub::SendAcceptLoginCmd(bool bAccept)
{
    if (m_stubState==STUB_STATE_ALIVE)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_accept_login);
        g_sendCmd->WriteByte(bAccept);
        g_sendCmd->WriteString((char*)g_lgs->m_strLoginName.c_str());
        SendCmd(g_sendCmd);
    }
}

void CGameWorldStub::SendShutdownCmd(long lDelayTime)
{
    if (m_stubState==STUB_STATE_ALIVE)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_shutdown_gws);
        g_sendCmd->WriteLong(lDelayTime*1000);
        SendCmd(g_sendCmd);
        m_stubState = STUB_STATE_SERVER_SHUTDOWNING;
    }
}

void CGameWorldStub::SendKickAllUser(long lDelayTime)
{
    if (m_stubState==STUB_STATE_ALIVE)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_kick_all_user);
        g_sendCmd->WriteLong(lDelayTime*1000);
        SendCmd(g_sendCmd);
    }
}

void CGameWorldStub::Test()
{
    if (m_stubState==STUB_STATE_ALIVE)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_tester);
        SendCmd(g_sendCmd);
        m_cTestState = 1;
    }
}

bool CGameWorldStub::GetTestResult(std::string& resultStr)
{
    if (m_cTestState==2)
    {
        if (!m_strTestResult.empty())
        {
            sprintf(g_strStaticBuffer, "%s [%s]\n", m_szName.c_str(), m_szGameworldIP.c_str());
            resultStr += g_strStaticBuffer;
            resultStr += m_strTestResult;
        }else
        {
            sprintf(g_strStaticBuffer, "%s [%s] 正常\n", m_szName.c_str(), m_szGameworldIP.c_str());
            resultStr += g_strStaticBuffer;
        }
        return true;
    }else
    {
        sprintf(g_strStaticBuffer, "%s [%s] timeout\n", m_szName.c_str(), m_szGameworldIP.c_str());
        resultStr += g_strStaticBuffer;
    }
    return false;
}

bool CGameWorldStub::Tick(long delta)
{
	int cost = rtGetMilliseconds();
	bool ret = true;

	switch(m_stubState)
	{
	case STUB_STATE_ALIVE:
    case STUB_STATE_SERVER_SHUTDOWNING:
		if(!m_netLink) 
		{
			ERR("CGameWorldStub::Tick() stub alive but net link = NULL");
			break;
		}
		m_netLink->DefProc();
		if(m_netLink->GetState() == NET_STATE_DEAD)
		{
			OnNetDead();
		}else
		{
			while(m_netLink->GetPacket(m_packet))
			{
				if (!ParseCmd(m_packet))
                {
                    ERR1("CGameWorldStub::Tick() ParseCmd ERROR at %d\n", rtTimestamp());
                    m_netLink->Close();
                    OnNetDead();
                }
			}
		}
		break;

	case STUB_STATE_NET_DEAD:
		//if(rtGetMilliseconds() - m_netDeadTime > 180 * 1000)
		{
			// time out remove all user
			g_lgs->RemoveAllUser(this);
		}
        ret = false;
		break;
    case STUB_STATE_SERVER_SHUTDOWNED:
        // 关闭连接
        if(m_netLink && m_netLink->GetState()!=NET_STATE_DEAD)
        {
            m_netLink->Close();
            OnNetDead();
        }
		ret = false;
		break;
    default:
        ret = false;
    }
    
	cost = rtGetMilliseconds() - cost;
	if(cost > 5000)
		ERR2("tick stat = %d,cost = %d\n",m_stubState,cost);
	
	return ret;
}

bool CGameWorldStub::ParseCmd(CG_CmdPacket *pPacket)
{
	short cmdId;
	SUserInfo *user = NULL;
	if(!pPacket->ReadShort(&cmdId)) return false;

	int cost = rtGetMilliseconds();
	switch(cmdId)
	{
	case gws2lgs_user_login:
        {
            char *str;
            long lRetUser;
            short sRetCode = 0;
            SListCmd newCmd(this, LIST_CMD_USER_LOGIN);
            newCmd.userLogin.t.iStep = 0;
            newCmd.userLogin.dwLoginOrderID = g_dwLoginOrderID++; // 创建登陆流水号

            READb_long(newCmd.userLogin.lSeed)

            if (g_lgs->m_tShutdownTime)
            {
                lRetUser = LOGIN_RET_FAILED_SERVER_STOP;
                sRetCode = LOGIN_RET_CODE_ShutdownTime;
            }else if ((g_lgs->m_lStatus&(lssLoginNormal|lssLoginGM))==0)
            {
                lRetUser = LOGIN_RET_FAILED_SERVER_STOP;
                sRetCode = LOGIN_RET_CODE_LoginPadlock;
            }else
            {
                lRetUser = LOGIN_RET_FAILED_UNKNOWN;
                sRetCode = LOGIN_RET_CODE_DataError;
	            while (1)
                {
                    if(pPacket->IsError()) break;

                    READb_string(str)
                    if(!(strlen(str)<=MAX_USER_NAME_SIZE)) break;
                    strcpy(newCmd.userLogin.szUsername, str);
                    strlwr(newCmd.userLogin.szUsername);

					READb_string(str)
					if(!(strlen(str)<=MAX_USER_NAME_SIZE + 30)) break;
					strcpy(newCmd.userLogin.szPubWinUsername, str);

					READb_string(str)
					if(!(strlen(str)<=MAX_USER_NAME_SIZE + 30)) break;
					strcpy(newCmd.userLogin.pubwinKey, str);

                    READb_string(str)
					if(g_pWSDL->m_wsdl_is_pubwin == 0)
					{
						if(strlen(str)!=MAX_USER_PWD_SIZE) break;
						strncpy(newCmd.userLogin.szPassword,str, 32);
						newCmd.userLogin.szPassword[32] = 0;
					}
					else
					{
						if(strlen(str)!=MAX_USER_PWD_SIZE) break;//MD5加密后的密码
						strncpy(newCmd.userLogin.glomPubwinPWD,str, 32);
						newCmd.userLogin.glomPubwinPWD[32] = 0;
						READb_string(str)
						strcpy(newCmd.userLogin.szPassword, str);
					}


                    newCmd.userLogin.szUserIP[0] = '\0';
                    if(pPacket->ReadString(&str))
                    {
                        strncpy(newCmd.userLogin.szUserIP, str, 15);
                        newCmd.userLogin.szUserIP[15] = 0;
                    }
					//PZH
					char cPower = 0;
					bool bPower;
					pPacket->ReadByte(&cPower);
                    bPower = 0 != cPower;
					newCmd.userLogin.bPowerLogin = bPower;
					//
   				    if (!g_recvCmdList.AddCmd(&newCmd))
                    {
                        ERR("ERROR: Add Login cmd failed\n");
                    }
				    lRetUser = LOGIN_RET_SUCCESS;
				    /*
                    std::string szUsername;
				    if (g_lgs->m_userLogin.find(szUsername)==g_lgs->m_userLogin.end())
                    {
                        g_lgs->m_userLogin[szUsername] = 0;
                        if (!g_recvCmdList.AddCmd(&newCmd))
                        {
                            ERR("ERROR: Add Login cmd failed\n");
                        }
                        return false;
                    }else
                    {
                        MSG1("Received multi login command by user [%s]\n", newCmd.userLogin.username);
                    }
				    */
                    break;
                }
            }
		    if(lRetUser != LOGIN_RET_SUCCESS)
		    {
                switch (lRetUser)
                {
                case LOGIN_RET_FAILED_SERVER_STOP:
                    MSG("user login failed, server stoped\n");
                    break;
                case LOGIN_RET_FAILED_UNKNOWN:
                default:
                    ERR("user login failed, unknown\n");
                    break;
                }
			    g_sendCmd->BeginWrite();
			    g_sendCmd->WriteShort(lgs2gws_user_login_ret);
                g_sendCmd->WriteLong(newCmd.userLogin.lSeed);
			    g_sendCmd->WriteLong(lRetUser);
			    g_sendCmd->WriteShort(sRetCode);
                SendCmd(g_sendCmd);
		    }
        }
        break;

	case gws2lgs_user_quit:						// use account id instead of name
        {
		    DWORD accountId;
		    READb_ulong(accountId);
		    g_lgs->UserQuit(accountId);
        }
		break;

    case gws2lgs_user_enter_game:
        {
            long iLevel;
		    DWORD accountId;
		    READb_ulong(accountId);
            READb_long(iLevel);
            g_lgs->OnUserEnterGame(accountId, iLevel);
        }
        break;

    case gws2lgs_shutdown_ok:
        m_stubState = STUB_STATE_SERVER_SHUTDOWNED;
        break;

    case gws2lgs_update_state:
        READb_char(m_gwState)
        break;

    /*
    case gws2lgs_get_config:
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_update_config);
        m_pConfig->Serialize(g_sendCmd);
        SendCmd(g_sendCmd);
        break;
    */

    case gws2lgs_query_state:
        SendAcceptLoginCmd(true);
        break;

    case gws2lgs_update_gws_state:
        {
            m_gwStat.Serialize(pPacket, SERVER_STAT_VERSION);
            long i, lCnt;
            pPacket->ReadLong(&lCnt);
            if (m_rsStat.size()!=lCnt)
            {
                m_rsStat.resize(lCnt);
            }
            short sID, sState;
            for (i=0; i<lCnt; i++)
            {
                pPacket->ReadShort(&sState);
                pPacket->ReadShort(&sID);
                m_rsStat[i].sState = sState;
                m_rsStat[i].sID = sID;
                if (sState)
                {
                    m_rsStat[i].SerializeAll(pPacket, SERVER_STAT_VERSION);
                }else
                {
                    m_rsStat[i].Reset();
                }
            }
            m_gmStat.Serialize(pPacket, SERVER_STAT_VERSION);
        }
        break;

    case gws2lgs_check_version:
        {
            bool bSame = g_lgs->CheckGameworldVersion(this, pPacket);
            if (bSame)
            {
                NOTICE("Check GWS [%s] %s:%d version SUCCEEDED.\n",
                    m_szName.c_str(), m_szGameworldIP.c_str(), m_lGameworldPort);
            }else
            {
                ERR3("Check GWS [%s] %s:%d version FAILED.\n",
                    m_szName.c_str(), m_szGameworldIP.c_str(), m_lGameworldPort);
            }
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_check_version_ret);
            g_sendCmd->WriteByte(bSame);
            SendCmd(g_sendCmd);
        }
        break;

    case gws2lgs_check_sub_version:
        {
            long lCheckID;
            pPacket->ReadLong(&lCheckID);
            bool bSame = g_lgs->CheckServerVersion(pPacket);
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_check_sub_version_ret);
            g_sendCmd->WriteLong(lCheckID);
            g_sendCmd->WriteByte(bSame);
            SendCmd(g_sendCmd);
        }
        break;

    case gws2lgs_register: // 注册GWS, 包含GWS的名字和类型(登陆限制)
        {
            char ip[20];
            short port;
            char *szGwsName, *szListenClientIP;

            // 注册前从列表删除自己
            m_listAllowIP.Clear();

            // 读取注册信息
            pPacket->ReadString(&szGwsName);
            pPacket->ReadString(&szListenClientIP);
            pPacket->ReadLong(&m_lListenClientPort);
            pPacket->ReadLong(&m_lListType);
            pPacket->ReadLong(&m_lGameworldID);
            m_listAllowIP.Serialize(pPacket);

            if(m_netLink->GetSocket().GetRemoteAddr(ip,&port))
            {
                if (szListenClientIP=="localhost" || szListenClientIP=="127.0.0.1" )
                {
                    if (strcmp(ip, "localhost")==0 || strcmp(ip, "127.0.0.1")==0)
                    {
                        szListenClientIP = gethostbyname("localhost")->h_name;
                    }else
                    {
                        szListenClientIP = ip;
                    }
                }
            }

            // 检查正确性(不允许同名的Gameworld存在)
            if (g_lgs->FindGwsByName(szGwsName)!=NULL)
            {
                ERR3("Register GWS name overlap [%s] %s:%d\n", szGwsName, m_szGameworldIP.c_str(), m_lGameworldPort);
                m_bRegistered = false;
            }else
            {
                // 开始注册
                m_szName = szGwsName;
                m_szListenClientIP = szListenClientIP;
                m_lListenClientPort = m_lListenClientPort;
                g_lgs->RegisterGws(m_lStubIdx, szGwsName, szListenClientIP, m_lListenClientPort, m_lGameworldID, m_listAllowIP);

                m_bRegistered = true;
                NOTICE("Register GWS [%s](%d) %s:%d\n", szGwsName, m_lStubIdx, m_szGameworldIP.c_str(), m_lGameworldPort);
            }
            g_sendCmd->BeginWrite();
            g_sendCmd->WriteShort(lgs2gws_register_ret);
            g_sendCmd->WriteByte(m_bRegistered);
            SendCmd(g_sendCmd);
        }
        break;

    case gws2lgs_tester_ret:
        {
            char cResult;
            char *szResult;
            pPacket->ReadByte(&cResult);
            if (cResult==0)
            {
                pPacket->ReadString(&szResult);
                m_strTestResult = szResult;
            }else
            {
                m_strTestResult = "";
            }
            if (m_cTestState==1)
            {
                m_cTestState = 2;
            }
        }
        break;

    case gws2lgs_gm_command:
        {
            long lAccountID;
            char* szCmd;
	        pPacket->ReadLong(&lAccountID);
	        if (!pPacket->ReadString(&szCmd)) return true;
            g_lgs->DoGameMasterCommand(this, lAccountID, szCmd);
        }
        break;

    case gws2lgs_broadcast_to_all_region:
        {
            char* szCmd;
	        if (!pPacket->ReadString(&szCmd)) return true;
            g_lgs->BroadcastCmdToAllRegion(szCmd);
        }
        break;

    case gws2lgs_broadcast_to_gameworld:
        {
            char* szCmd;
	        if (!pPacket->ReadString(&szCmd)) return true;
            g_lgs->BroadcastCmdToGameworld(szCmd);
        }
        break;

    case gws2lgs_charge_card:
        {
            long lBuyerAccountID;
            char* szBuyerAccountName;
            long lSalesmanAccountID;
            char* szSalesmanAccountName;
            char* szUsername;
            long lUserID;
            short sLevel;
            char cMetier;
            char cMetierLev;
            char cCardType;
            char* szCardID;
            char* szCardPwd;
            long lUserParam;

            pPacket->ReadLong(&lBuyerAccountID);
            pPacket->ReadString(&szBuyerAccountName);
            pPacket->ReadLong(&lSalesmanAccountID);			//0
            pPacket->ReadString(&szSalesmanAccountName);	//NULL
            pPacket->ReadLong(&lUserID);
            pPacket->ReadString(&szUsername);
            pPacket->ReadShort(&sLevel);
            pPacket->ReadByte(&cMetier);
            pPacket->ReadByte(&cMetierLev);
            pPacket->ReadByte(&cCardType);
            if (!pPacket->ReadString(&szCardID)) return true;
            if (!pPacket->ReadString(&szCardPwd)) return true;
            pPacket->ReadLong(&lUserParam);					//0

//             g_lgs->ChargeCard(lBuyerAccountID, szBuyerAccountName,
//                 lSalesmanAccountID, szSalesmanAccountName,
//                 (short)m_lGameworldID, lUserID, szUsername,
//                 sLevel, cMetier, cMetierLev, cCardType, szCardID, szCardPwd, lUserParam);
			SUserInfo* pUserInfo =g_lgs->FindUserByName(szBuyerAccountName);

			SListCmd newCmd(this, LIST_CMD_CHARGE_CARD);
			newCmd.chargePoint.lBuyerAccountID	  = lBuyerAccountID;
			strcpy(newCmd.chargePoint.szBuyerAccountName,szBuyerAccountName);
			newCmd.chargePoint.lAccountID		  = lSalesmanAccountID;
			strcpy(newCmd.chargePoint.szAccountName,szSalesmanAccountName);
			newCmd.chargePoint.sServer			  = (short)m_lGameworldID;
			newCmd.chargePoint.lUserID			  = lUserID;
			strcpy(newCmd.chargePoint.szUserName,szUsername);
			newCmd.chargePoint.sUserLev			  = sLevel;
			newCmd.chargePoint.cMetier			  = cMetier;
			newCmd.chargePoint.cMetierLev		  = cMetierLev;
			newCmd.chargePoint.cCardType		  = cCardType;
			strcpy(newCmd.chargePoint.szCardID,szCardID);
			strcpy(newCmd.chargePoint.szCardPassWord,szCardPwd);
			newCmd.chargePoint.param			  = lUserParam;
			if (pUserInfo)
			{
				strcpy(newCmd.chargePoint.cUserIp,pUserInfo->info.ip.c_str());
			}
			else
				newCmd.chargePoint.cUserIp[0]		= '\0';
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add chargePoint cmd failed\n");
			}

        }
        break;
	case gws2lgs_add_account_time://tim.yang   VIP卡
		{
			long lBuyerAccountID;
			long time;
			long lUserId;
			pPacket->ReadLong(&lBuyerAccountID);
			pPacket->ReadLong(&time);
			pPacket->ReadLong(&lUserId);
			SListCmd newCmd(this, LIST_CMD_ADD_ACCOUNT_TIME);
			newCmd.accountInfo.accountId = lBuyerAccountID;
			newCmd.GWSId	= (short)m_lGameworldID;
			newCmd.VIPRegionTime = time;
			newCmd.userId = lUserId;
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add chargePoint cmd failed\n");
			}
		}
		break;
    case gws2lgs_card_can_trade:
        {
            long lSalesmanAccountID;
            char* szSalesmanAccountName;
            long lUserID;
            char cCardType;
            char* szCardID;
            char* szCardPwd;
            long lUserParam;
            short sTime;

            pPacket->ReadLong(&lSalesmanAccountID);
            pPacket->ReadString(&szSalesmanAccountName);
            pPacket->ReadLong(&lUserID);
            pPacket->ReadByte(&cCardType);
            if (!pPacket->ReadString(&szCardID)) return true;
            if (!pPacket->ReadString(&szCardPwd)) return true;
            pPacket->ReadLong(&lUserParam);
            pPacket->ReadShort(&sTime);

            g_lgs->CardCanTrade(lSalesmanAccountID, szSalesmanAccountName, (short)m_lGameworldID, lUserID, cCardType,
                szCardID, szCardPwd, lUserParam, sTime);

			SListCmd newCmd(this, LIST_CMD_CARD_TRADE);
			newCmd.chargePoint.lAccountID		= lSalesmanAccountID;
			strcpy(newCmd.chargePoint.szAccountName, szSalesmanAccountName);
			newCmd.chargePoint.lUserID			= lUserID;
			newCmd.chargePoint.cCardType		= cCardType;
			strcpy(newCmd.chargePoint.szCardID,szCardID);
			strcpy(newCmd.chargePoint.szCardPassWord,szCardPwd);
			newCmd.chargePoint.param			= lUserParam;//lSn
			newCmd.chargePoint.tTime			= sTime;
			newCmd.chargePoint.sServer			= (short)m_lGameworldID;
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add card_trade cmd failed\n");
			}

       }
        break;

    case gws2lgs_query_point:
        {
            long lAccountID;
            char cNeedShowHtml;
            pPacket->ReadLong(&lAccountID);
            pPacket->ReadByte(&cNeedShowHtml);

            SUserInfo* pUserInfo = g_lgs->FindUser(lAccountID);
            if (pUserInfo)
            {
                g_sendCmd->BeginWrite();
                g_sendCmd->WriteShort(lgs2gws_query_point_ret);
                g_sendCmd->WriteLong(lAccountID);
                if (cNeedShowHtml==1 && pUserInfo->bReceivePointInfo)
                {
                    g_sendCmd->WriteByte(2);
                }else
                {
                    g_sendCmd->WriteByte(pUserInfo->bReceivePointInfo);
                }
                g_sendCmd->WriteLong(pUserInfo->iChargePoint);
                g_sendCmd->WriteLong(pUserInfo->tChargeExpire);
                g_sendCmd->WriteString(pUserInfo->szChargeMsg);
                g_sendCmd->WriteByte(pUserInfo->cVIP);
                SendCmd(g_sendCmd);
            }
        }
        break;

    case gws2lgs_keep_online:
        {
            long lAccountID;
            long lLevel;
            pPacket->ReadLong(&lAccountID);
            pPacket->ReadLong(&lLevel);
            SUserInfo* pUserInfo = g_lgs->FindUser(lAccountID);
            if (pUserInfo)
            {
                pUserInfo->tLastKeepOnlineMsg = time(NULL);
                pUserInfo->iKickCount = 0;
                pUserInfo->sActorLevel = lLevel;
            }
        }
        break;

    case gws2lgs_deduct:
        {
             long lAccountID;
             long lUserID;
             long lDeductOrderID;
             short sPoint;
             pPacket->ReadLong(&lAccountID);
             pPacket->ReadLong(&lUserID);
             pPacket->ReadLong(&lDeductOrderID);
             pPacket->ReadShort(&sPoint);
             //g_lgs->Deduct((short)m_lGameworldID, lAccountID, lUserID, sPoint, lDeductOrderID);

			SListCmd newCmd(this, LIST_CMD_USER_POINT);
			newCmd.chargePoint.sServer = (short)m_lGameworldID;
			newCmd.chargePoint.lAccountID = lAccountID;
			newCmd.chargePoint.lUserID = lUserID;
			newCmd.chargePoint.lDeductOrderID = lDeductOrderID;
			newCmd.chargePoint.sPoint = sPoint;
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add chargePoint cmd failed\n");
			}
        }
        break;

    case gws2lgs_prededuct:
        {
            long lAccountID;
            long lUserID;
            long lDeductOrderID;
			long lAuctionId;
            short sPoint;
			long event;
			pPacket->ReadLong(&lAccountID);
            pPacket->ReadLong(&lUserID);
            pPacket->ReadLong(&lDeductOrderID);
			pPacket->ReadLong(&event);
            pPacket->ReadShort(&sPoint);
			pPacket->ReadLong(&lAuctionId);

            //g_lgs->PreDeduct((short)m_lGameworldID, lAccountID, lUserID, sPoint, lDeductOrderID);
			SListCmd newCmd(this, LIST_CMD_PRE_USER_POINT);
			newCmd.chargePoint.sServer = (short)m_lGameworldID;
			newCmd.chargePoint.lAccountID = lAccountID;
			newCmd.chargePoint.lUserID = lUserID;
			newCmd.chargePoint.lDeductOrderID = lDeductOrderID;
			newCmd.chargePoint.sPoint = sPoint;
			newCmd.chargePoint.event  = event;
			newCmd.chargePoint.lAuctionId  = lAuctionId;
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add chargePoint cmd failed\n");
			}
       }
        break;

    case gws2lgs_lock_account:
        {
            long lAccountID;
            short sLockFlag;
            long lLockSecond;
            unsigned long lNow = (unsigned long)time(NULL);

            pPacket->ReadLong(&lAccountID);
            pPacket->ReadShort(&sLockFlag);
            pPacket->ReadLong(&lLockSecond);

            g_lgs->LockAccountByID(this, 0, lAccountID, sLockFlag, lNow, lNow+lLockSecond);
        }
        break;

	//--------add start by tony 05.10.08-------//
	case gws2lgs_license_check:
		{
			char*	pstrip;
			char*	pstrmac;
			char*	pstrMd5;
			long	nType;
			bool	bValid = false;

			pPacket->ReadString(&pstrip);
			pPacket->ReadString(&pstrmac);
			pPacket->ReadString(&pstrMd5);
			pPacket->ReadLong(&nType);

			if(!LicenseCheck(pstrip, pstrmac, pstrMd5, nType))
			{
				LOG("License From GW Check Failed\n");
				bValid = false;
			}else
			{
				LOG("License From GW Check Success\n");
				bValid = true;
			}

			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(lgs2gws_license_check_ret);
			g_sendCmd->WriteByte(bValid);
			SendCmd(g_sendCmd);

		}
		break;
	case gws2lgs_license_check_rs:
		{
			char*	pstrip;
			char*	pstrmac;
			char*	pstrMd5;
			long	nType;
			bool	bValid = false;

			pPacket->ReadString(&pstrip);
			pPacket->ReadString(&pstrmac);
			pPacket->ReadString(&pstrMd5);
			pPacket->ReadLong(&nType);

			if(!LicenseCheck(pstrip, pstrmac, pstrMd5, nType))
			{
				LOG("License From GW RSCheck Failed\n");
				bValid = false;
			}else
			{
				LOG("License From GW RSCheck Success\n");
				bValid = true;
			}

			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(lgs2gws_license_check_rs_ret);
			g_sendCmd->WriteByte(bValid);
			SendCmd(g_sendCmd);

		}
		break;
	//--------add end   by tony 05.10.08-------//
	case gws2lgs_user_change_block:
		{
			long lAccountID, lUserId;
			char* szAccountName;
			char* oldBlockFileName;
			char* newBlockFileName;

			READb_string(szAccountName);
			READb_ulong(lAccountID);
			READb_ulong(lUserId);
			READb_string(oldBlockFileName);
			READb_string(newBlockFileName);

            g_lgs->OnUserBlockChanged(szAccountName, lAccountID, lUserId, oldBlockFileName, newBlockFileName);
		}
		break;

	case gws2lgs_card_failed:
		{
			char* szCardIdMd5;
			pPacket->ReadString(&szCardIdMd5);
			SListCmd newCmd(NULL, LIST_CMD_CARD_TRADE_FAILD);
			strcpy(newCmd.chargePoint.szCardID,szCardIdMd5);
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add cardtrade failed cmd failed\n");
			}
		}
		break;
	case gws2lgs_add_point:
		{
			long lAccountID;
			long lUserID;
			short sPoint;
			pPacket->ReadLong(&lAccountID);
			pPacket->ReadLong(&lUserID);
			pPacket->ReadShort(&sPoint);
			//g_lgs->Deduct((short)m_lGameworldID, lAccountID, lUserID, sPoint, lDeductOrderID);

			SListCmd newCmd(this, LIST_CMD_ADD_POINT);
			newCmd.chargePoint.sServer = (short)m_lGameworldID;
			newCmd.chargePoint.lAccountID = lAccountID;
			newCmd.chargePoint.lUserID = lUserID;
			newCmd.chargePoint.sPoint = sPoint;
			if (!g_recvCmdList.AddCmd(&newCmd))
			{
				ERR("ERROR: Add chargePoint cmd failed\n");
			}

		}
		break;
    default:
        ERR1("GameWorldStub: Invalid command %d\n", cmdId);
        break;
	}

	cost = rtGetMilliseconds() - cost;
	if(cost > 5000)
        ERR2("cmd = %d,cost = %d\n",cmdId,cost);
    return true;
}

void CGameWorldStub::OnNetDead()
{
    if (m_stubState==STUB_STATE_SERVER_SHUTDOWNING)
    {
        m_stubState = STUB_STATE_SERVER_SHUTDOWNED;
    }else
    {
        m_stubState = STUB_STATE_NET_DEAD;
    }
    ERR1("GameWorldStub: [%s] Net Dead\n", m_szName.c_str());
	ULONG timeNow = rtGetMilliseconds();
	delete m_netLink;
	m_netLink = NULL;
	m_netDeadTime = timeNow;
	g_lgs->GameWorldNetDead(this);
    // 这句用来取消Gameworld列表。之所以注释掉是因为目前没有这种需求。有这种需求的时候需要打开这个功能
    //if (m_lListType) // 这个判断没有确定，要证实
    //{
        g_lgs->UnregisterGws(m_lStubIdx);
    //}
}

void CGameWorldStub::SendMessageToUser(DWORD dwAccountID, char cMsgType, const char* szMessage)
{
    if (m_stubState==STUB_STATE_ALIVE)
    {
        g_sendCmd->BeginWrite();
        g_sendCmd->WriteShort(lgs2gws_user_message);
        g_sendCmd->WriteLong(dwAccountID);
        g_sendCmd->WriteByte(cMsgType);
        g_sendCmd->WriteString((char*)szMessage);
        SendCmd(g_sendCmd);
    }
}
