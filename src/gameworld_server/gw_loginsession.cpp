#include "gw_server.h"
#include "lgs_gws.h"
//#include "rt_system_info.h"

//#define LICENSE_CHECK

//PZH
#include "LogicExt.h"
//

#ifdef LINUX
#include "linux_util.h"
#endif //LINUX

GW_BEGIN_CMD(cmd_lgs2gws_accept_login)
{
    char cRet;
    if (!packet->ReadByte(&cRet))
    {
        cRet = 0;
    }
    char* pName;
    if (!packet->ReadString(&pName))
		g_server.m_strLoginName = "";
	else
		g_server.m_strLoginName = pName;

	g_server.OnAcceptLogin(cRet!=0);
}
GW_END_CMD;

long GetMaxRegionUser()
{
    long lMaxUser = 0;
    GWRegion* pRs;
    for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
    {
        pRs = &(g_server.m_regionTable[rgID]);
        if (pRs->sessionIndex != -1)
        {
            if (lMaxUser<pRs->m_stat.lUserCount)
            {
                lMaxUser = pRs->m_stat.lUserCount;
            }
        }
    }
    return lMaxUser;
}

GW_BEGIN_CMD(cmd_lgs2gws_user_login_ret)
{
	long seed,ret,lPoint;
    char cPointType; // 1 点数, 2 时间
    char cVIP;
    SAccountInfo   info;

	if (!packet->ReadLong(&seed))
		return eInvalidData;

	if (!packet->ReadLong(&ret))
		return eInvalidData;

	if(ret == LOGIN_RET_SUCCESS)
	{
		*packet << info;
        if (!packet->ReadByte(&cPointType))
            return eInvalidData;
        if (!packet->ReadLong(&lPoint))
            return eInvalidData;
        if (!packet->ReadByte(&cVIP))
            return eInvalidData;
	}

	if(packet->IsError())
	{
		return eInvalidData;
	}

	GWUnloginClient *ulc = g_server.FindUnloginClientBySeed(seed);
	if(!ulc)
	{
		// client disconnected
		if(ret == LOGIN_RET_SUCCESS)
        {
            g_server.m_login.LogoutAccount(info.accountId);
        }
		return eInvalidData;
	}

	
	if(ret == LOGIN_RET_SUCCESS)
	{
		// 人数限制
		if(!(info.privileges&EUP_LoginGM)
            && ((g_server.GetCurrentUserCnt() >= g_cfgGW.game.maxUser) || (GetMaxRegionUser()>=g_cfgGW.game.maxRegionUser)))
		{
			ret = LOGIN_RET_FAILED_SERVER_FULL;
		}
		//PZH
		bool bPower = ulc->IsPowerLogin();
		if(bPower)
		{
			int nOld = CLogicExt::GetCLogicExt().DeleteOldLogin(info.accountId, ulc, lPoint,  cPointType, cVIP);
			if(0 == nOld)
			{
				return eOkay; 
			}
		}
		//
	}

	CG_TCPSession* pUlcSession = ulc->netLink;
	int nNum = 0;
	if(ret == LOGIN_RET_SUCCESS)
	{
		if(!g_server.UserLogin(ulc,&info))
		{
			ret = LOGIN_RET_FAILED_UNKNOWN;
		}
	}
	//PZH
	else
	{
		ulc->loginFailedCnt++;
		nNum = CLogicExt::GetCLogicExt().GetErrNum(ulc->GetAccountName()) + 1;
		if(nNum > MAX_LOGIN_FAILED_CNT)
		{
			ret = LOGIN_RET_FAILED_ACCOUNT_WRONG5;
			CLogicExt::GetCLogicExt().AddLock(ulc->GetAccountName());//开始锁定10分钟
			CLogicExt::GetCLogicExt().DelInNumMap(ulc->GetAccountName());
		}
		else
		{
			CLogicExt::GetCLogicExt().SetErrNum(ulc->GetAccountName(), nNum);
		}
	}
	//

    if (pUlcSession)
    {
		g_sendCmd->BeginWrite();
		g_sendCmd->WriteShort(g2c_login_ret);
	    g_sendCmd->WriteLong(ret);
        if (ret==LOGIN_RET_FAILED_USERDEFINE)
        {
            char* szRetStr;
	        if (!packet->ReadString(&szRetStr))
				g_sendCmd->WriteString("");
			else
				g_sendCmd->WriteString(szRetStr);
		}else
        {
            short sRetCode = 0;
            if (ret!=LOGIN_RET_SUCCESS)
            {
	            packet->ReadShort(&sRetCode);
            }
            g_sendCmd->WriteShort(sRetCode);
        }
        if (ret==LOGIN_RET_SUCCESS)
        {
            g_sendCmd->WriteByte(cPointType);
            g_sendCmd->WriteLong(lPoint);
            g_sendCmd->WriteByte(cVIP);

			CLogicExt::GetCLogicExt().DelInNumMap(ulc->GetAccountName());//PZH
        }
        pUlcSession->SendPacket(g_sendCmd);
    }

	if(ret != LOGIN_RET_SUCCESS)
	{
		//ulc->loginFailedCnt++; //PZH 放到前面一点
		if(/*ulc->loginFailedCnt*/nNum > MAX_LOGIN_FAILED_CNT) 
		{
			ulc->valid = false;
		}
	}
}

GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_kick_user)
{
	DWORD accountId;
	char cCause;
    char* szCauseInfo;

	if (!packet->ReadLong((long*)&accountId))
		return eInvalidData;

	if (!packet->ReadByte(&cCause))
		return eInvalidData;

    if (cCause==KICK_USER_USERDEFINE)
    {
	    if (!packet->ReadString(&szCauseInfo))
		    return eInvalidData;
    }else
    {
        szCauseInfo = "cmd_lgs2gws_kick_user";
    }

	g_server.KickAccount(accountId, cCause, szCauseInfo);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_shutdown_gws)
{
	long delay;

	if (!packet->ReadLong(&delay))
		return eInvalidData;

	g_server.m_state = ssShutdown;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_kick_all_user)
{
	long lDelayTime;
	if (!packet->ReadLong(&lDelayTime))
		return eInvalidData;
    g_server.SendKickAllUser(lDelayTime);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_bulletin)
{
    char* content;
    long color;
    char style;

    if (!packet->ReadString(&content))
        return eOkay;
    if (!packet->ReadLong(&color))
        return eOkay;
    if (!packet->ReadByte(&style))
        return eOkay;

    g_server.BroadcastBulletin(content, style, color);

    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_bulletin_shutdown)
{
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_check_version_ret)
{
    char cSame;
    if (!packet->ReadByte(&cSame))
        return eInvalidData;
    if (cSame==0)
    {
        ERR("ERROR: check gameworld version FAILED.\n");
        g_server.m_state = ssExit;
        return eInvalidData;
    }else
    {
        NOTICE("Check gameworld version SUCCEEDED.\n");
    }

    g_server.m_cSubState = gwssWaitRegion;

    // 注册
    CG_CmdPacket& cmd = g_server.m_login.BeginCmd(gws2lgs_register);
    cmd.WriteString((char*)g_cfgGW.game.szGameworldName.c_str());
    cmd.WriteString((char*)g_cfgGW.listenClient.listenIP.c_str());
    cmd.WriteLong(g_cfgGW.listenClient.listenPort);
    cmd.WriteLong(g_cfgGW.game.listType);
    cmd.WriteLong(g_cfgGW.lGameworldID);
    g_cfgGW.game.listAllowIP.Serialize(&cmd);
    g_server.m_login.SendCmd();
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_register_ret)
{
    char cSucceed;
    if (!packet->ReadByte(&cSucceed))
        return eInvalidData;
    if (cSucceed==0)
    {
        ERR("ERROR: register gameworld FAILED.\n");
        g_server.m_state = ssExit;
        return eInvalidData;
    }else
    {
        NOTICE("Register gameworld SUCCEEDED.\n");
    }

    g_server.m_login.BeginCmd(gws2lgs_query_state);
    g_server.m_login.SendCmd();
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_tester)
{
    g_server.TestServer();
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_check_sub_version_ret)
{
    long lCheckID;
    char cSame;
    char* pServerName;
    if (!packet->ReadLong(&lCheckID))
        return eInvalidData;
    if (!packet->ReadByte(&cSame))
        return eInvalidData;
    if (!packet->ReadString(&pServerName))
        return eInvalidData;
    g_server.m_login.OnCheckSubServerVersion(lCheckID, cSame!=0, pServerName);
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_user_message)
{
    long lAccountID;
    char cMsgType;
    char* pMessage;
    if (!packet->ReadLong(&lAccountID))
        return eInvalidData;
    if (!packet->ReadByte(&cMsgType))
        return eInvalidData;
    if (!packet->ReadString(&pMessage))
        return eInvalidData;
    g_server.SendMessageToAccount((DWORD)lAccountID, cMsgType, pMessage);
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_add_gift)
{
    long lUserID;
    short sLevel;
    short sType;
    char* szGiftString;
    long tLimitTime;
    char cFromCardID;
    char* szCardID;

    if (!packet->ReadLong(&lUserID))
        return eInvalidData;
    if (!packet->ReadShort(&sLevel))
        return eInvalidData;
    if (!packet->ReadShort(&sType))
        return eInvalidData;
    if (!packet->ReadString(&szGiftString))
        return eInvalidData;
    if (!packet->ReadLong(&tLimitTime))
        return eInvalidData;
    if (!packet->ReadByte(&cFromCardID))
        return eInvalidData;
    if (cFromCardID)
    {
        if (!packet->ReadString(&szCardID))
            return eInvalidData;
		rt2_sprintf(g_strStaticBuffer, R(MSG_GIFT_DRAW_CARD_FROM_NPC), szCardID);
    }else
    {
        szCardID = NULL;
		rt2_sprintf(g_strStaticBuffer, R(MSG_GIFT_DRAW_ITEM_FROM_NPC));
    }
    if (g_GiftMgr.AddGift(lUserID, sLevel, sType, szGiftString, (DWORD)time(NULL), tLimitTime))
    {
        g_server.SendSystemMessageToUser(lUserID, g_strStaticBuffer);
		string StrItemID = "";
		g_server.m_mail.AddMail(lUserID, 0, R(MSG_GIFT_RECEIVE_ITEM), g_strStaticBuffer, true,StrItemID,0);
    }
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_clear_gift)
{
    long lUserID;
    if (!packet->ReadLong(&lUserID))
        return eInvalidData;
    g_GiftMgr.ClearGift(lUserID);
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_charge_card_ret)
{
    char cSucceed;
    long lUserID, lUserParam;
    char* szCardID, *szMsg;

    if (!packet->ReadLong(&lUserID))
        return eInvalidData;
    if (!packet->ReadByte(&cSucceed))
        return eInvalidData;
    if (!packet->ReadString(&szCardID))
        return eInvalidData;
    if (!packet->ReadString(&szMsg))
        return eInvalidData;
    if (!packet->ReadLong(&lUserParam))
        return eInvalidData;

    g_server.OnChargeCardResult(lUserParam, lUserID, szCardID, cSucceed, szMsg);
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_card_can_trade_ret)
{
    char cCan;
    long lUserID, lUserParam, lCardGroupType;
    char* szCardID, *szMsg;

    if (!packet->ReadLong(&lUserID))
        return eInvalidData;
    if (!packet->ReadByte(&cCan))
        return eInvalidData;
    if (!packet->ReadString(&szCardID))
        return eInvalidData;
    if (!packet->ReadString(&szMsg))
        return eInvalidData;
    if (!packet->ReadLong(&lCardGroupType))
        return eInvalidData;
    if (!packet->ReadLong(&lUserParam))
        return eInvalidData;

    g_server.OnCheckCardCanTradeResult(lUserID, szCardID, cCan, szMsg, lCardGroupType, lUserParam);
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_add_point_ret)
{
	long lUserID;
	char* cResult;
	short sPoint;
	char cSucceed;

	if (!packet->ReadLong(&lUserID)) return eInvalidData;
	if (!packet->ReadByte(&cSucceed)) return eInvalidData;
	if (!packet->ReadString(&cResult)) return eInvalidData;
	if (!packet->ReadShort(&sPoint)) return eInvalidData;

	GWUser* pUser = g_server.FindActorById(lUserID);
	if (pUser)
	{
		int seIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
		if (seIdx!=-1)
		{
			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_add_point_ret);
			cmd.WriteLong(lUserID);
			cmd.WriteByte(cSucceed);
			cmd.WriteString(cResult);
			cmd.WriteShort(sPoint);
			g_server.m_region.SendToPeer(seIdx);
		}
	}
	return eOkay;

}
GW_END_CMD

GW_BEGIN_CMD(cmd_lgs2gws_deduct_ret)
{
    long lUserID;
    long lDeductOrderID;
    char* cResult;
    short sPoint;
	short sCurPoint;

    if (!packet->ReadLong(&lUserID)) return eInvalidData;
    if (!packet->ReadLong(&lDeductOrderID)) return eInvalidData;
    if (!packet->ReadString(&cResult)) return eInvalidData;
    if (!packet->ReadShort(&sPoint)) return eInvalidData;
	if (!packet->ReadShort(&sCurPoint)) return eInvalidData;

    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser)
    {
		int seIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
		if (seIdx!=-1)
		{
		    CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_deduct_ret);
		    cmd.WriteLong(lUserID);
		    cmd.WriteLong(lDeductOrderID);
            cmd.WriteString(cResult);
            cmd.WriteShort(sPoint);
			cmd.WriteShort(sCurPoint);
		    g_server.m_region.SendToPeer(seIdx);
		}
    }
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_prededuct_ret)
{
    long lUserID;
    long lDeductOrderID;
    char cSucceed;
	short sPoint;
	short sCurPoint;
	char* cResult;
	long event;
	long lAuctionId;

    if (!packet->ReadLong(&lUserID)) return eInvalidData;
    if (!packet->ReadLong(&lDeductOrderID)) return eInvalidData;
    if (!packet->ReadByte(&cSucceed)) return eInvalidData;
    if (!packet->ReadShort(&sPoint)) return eInvalidData;
	if (!packet->ReadShort(&sCurPoint)) return eInvalidData;
	if (!packet->ReadString(&cResult))	return eInvalidData;
	if (!packet->ReadLong(&event))	return eInvalidData;
	if (!packet->ReadLong(&lAuctionId)) return eInvalidData;

    GWUser* pUser = g_server.FindActorById(lUserID);
    if (pUser)
    {
		int seIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
		if (seIdx!=-1)
		{
		    CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_prededuct_ret);
		    cmd.WriteLong(lUserID);
		    cmd.WriteLong(lDeductOrderID);
            cmd.WriteByte(cSucceed);
            cmd.WriteShort(sPoint);
			cmd.WriteShort(sCurPoint);
			cmd.WriteString(cResult);
			cmd.WriteLong(event);
			cmd.WriteLong(lAuctionId);
		    g_server.m_region.SendToPeer(seIdx);
		}
    }
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_query_point_ret)
{
    char cReceivePointInfo, cVIP;
    long lAccountID, lPoint, tExpire;
    char *szMsg;

    if (!packet->ReadLong(&lAccountID))
        return eInvalidData;
    if (!packet->ReadByte(&cReceivePointInfo))
        return eInvalidData;
    if (!packet->ReadLong(&lPoint))
        return eInvalidData;
    if (!packet->ReadLong(&tExpire))
        return eInvalidData;
    if (!packet->ReadString(&szMsg))
        return eInvalidData;
    if (!packet->ReadByte(&cVIP))
        return eInvalidData;

	GWLoginClient* pClient = g_server.FindLoginClientByAccountId(lAccountID);
    if (pClient)
    {
		GWUser *pUser = pClient->actorData;
        if (pUser)
        {
			int seIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
			if (seIdx!=-1)
			{
				CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_query_poing_ret);
				cmd.WriteLong(pUser->userID);
				cmd.WriteByte(cReceivePointInfo);
                cmd.WriteLong(lPoint);
                cmd.WriteLong(tExpire);
                cmd.WriteString(szMsg);
				cmd.WriteByte(cVIP);
				g_server.m_region.SendToPeer(seIdx);
			}
        }
    }
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_update_point_ret)
{
    char cType, cVIP;
    long lAccountID, lPoint;

    if (!packet->ReadLong(&lAccountID))
        return eInvalidData;
    if (!packet->ReadByte(&cType))
        return eInvalidData;
    if (!packet->ReadLong(&lPoint))
        return eInvalidData;
    if (!packet->ReadByte(&cVIP))
        return eInvalidData;

	GWLoginClient* pClient = g_server.FindLoginClientByAccountId(lAccountID);
    if (pClient)
    {
		GWUser *pUser = pClient->actorData;
        if (pUser)
        {
			int seIdx = g_server.m_regionTable[pUser->regionID].sessionIndex;
			if (seIdx!=-1)
			{
				CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_update_poing_ret);
				cmd.WriteLong(pUser->userID);
				cmd.WriteByte(cType);
                cmd.WriteLong(lPoint);
				cmd.WriteByte(cVIP);
				g_server.m_region.SendToPeer(seIdx);
			}
        }
    }
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_broadcast_to_all_region)
{
    char *szCommand;
    if (!packet->ReadString(&szCommand))
        return eInvalidData;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_broadcast_to_region);
	cmd.WriteString(szCommand);
	g_server.m_region.BroadcastCmd();
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_broadcast_to_gameworld)
{
    char *szCommand;
    if (!packet->ReadString(&szCommand))
        return eInvalidData;
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_license_check_ret)
{
	bool bValid;
	if (!packet->ReadByte((char*)&bValid))
		return eInvalidData;

	if(bValid)
	{
		LOG("License Check OK\n");
	}else
	{
		LOG("License Check Failed\n");
		g_server.Exit();
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_license_check_rs_ret)
{
	char bValid;
	if (!packet->ReadByte(&bValid))
		return eInvalidData;

	LOG1("RSCheck ret received: %d\n", bValid);

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_license_check_ret);
	cmd.WriteByte(bValid);
	g_server.m_region.BroadcastCmd();

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_lgs2gws_card_charged_event)
{
    long lCardGroupType;    // 卡组的类型，比如说 中秋月圆卡，优惠200点卡 等等
    char* szCardID;

    packet->ReadString(&szCardID);
    packet->ReadLong(&lCardGroupType);

    g_server.m_cardTrade.CancelConsign(0, szCardID);
	return eOkay;
}
GW_END_CMD;

//tim.yang   
GW_BEGIN_CMD(cmd_lgs2gws_add_account_time_ret)
{
	long lUserId;
	char isRight;
	char* time;
	packet->ReadLong(&lUserId);
	packet->ReadByte(&isRight);
	packet->ReadString(&time);
	g_server.AddAccountTimeRet(lUserId,isRight,time);
	return eOkay;
}
GW_END_CMD;
//end

void GWLoginSession::InitChannel()
{
	m_channel.RegisterCommand(lgs2gws_user_login_ret, cmd_lgs2gws_user_login_ret, "");
	m_channel.RegisterCommand(lgs2gws_accept_login, cmd_lgs2gws_accept_login, "");
	m_channel.RegisterCommand(lgs2gws_shutdown_gws, cmd_lgs2gws_shutdown_gws, "");	
	m_channel.RegisterCommand(lgs2gws_kick_user, cmd_lgs2gws_kick_user, "");
    m_channel.RegisterCommand(lgs2gws_bulletin, cmd_lgs2gws_bulletin, "");
    m_channel.RegisterCommand(lgs2gws_bulletin_shutdown, cmd_lgs2gws_bulletin_shutdown, "");
    m_channel.RegisterCommand(lgs2gws_check_version_ret, cmd_lgs2gws_check_version_ret, "");
    m_channel.RegisterCommand(lgs2gws_check_sub_version_ret, cmd_lgs2gws_check_sub_version_ret, "");
    m_channel.RegisterCommand(lgs2gws_register_ret, cmd_lgs2gws_register_ret, "");
    m_channel.RegisterCommand(lgs2gws_tester, cmd_lgs2gws_tester, "");
    m_channel.RegisterCommand(lgs2gws_user_message, cmd_lgs2gws_user_message, "");
    m_channel.RegisterCommand(lgs2gws_add_gift, cmd_lgs2gws_add_gift, "");
    m_channel.RegisterCommand(lgs2gws_clear_gift, cmd_lgs2gws_clear_gift, "");
    m_channel.RegisterCommand(lgs2gws_charge_card_ret, cmd_lgs2gws_charge_card_ret, "");
    m_channel.RegisterCommand(lgs2gws_query_point_ret, cmd_lgs2gws_query_point_ret, "");
    m_channel.RegisterCommand(lgs2gws_broadcast_to_all_region, cmd_lgs2gws_broadcast_to_all_region, "");
    m_channel.RegisterCommand(lgs2gws_broadcast_to_gameworld, cmd_lgs2gws_broadcast_to_gameworld, "");
    m_channel.RegisterCommand(lgs2gws_card_can_trade_ret, cmd_lgs2gws_card_can_trade_ret, "");
    m_channel.RegisterCommand(lgs2gws_deduct_ret, cmd_lgs2gws_deduct_ret, "");
    m_channel.RegisterCommand(lgs2gws_prededuct_ret, cmd_lgs2gws_prededuct_ret, "");
	m_channel.RegisterCommand(lgs2gws_license_check_ret, cmd_lgs2gws_license_check_ret, "");
	m_channel.RegisterCommand(lgs2gws_license_check_rs_ret, cmd_lgs2gws_license_check_rs_ret, "");
	m_channel.RegisterCommand(lgs2gws_card_charged_event, cmd_lgs2gws_card_charged_event, "");
	m_channel.RegisterCommand(lgs2gws_kick_all_user, cmd_lgs2gws_kick_all_user, "");	
    m_channel.RegisterCommand(lgs2gws_update_point_ret, cmd_lgs2gws_update_point_ret, "");
	m_channel.RegisterCommand(lgs2gws_add_point_ret,cmd_lgs2gws_add_point_ret,"");
	m_channel.RegisterCommand(lgs2gws_add_account_time_ret,cmd_lgs2gws_add_account_time_ret,"");//tim.yang  VIP卡
}

bool GWLoginSession::Initalize()
{
	SetBufSize(1024);
	return true;
}

void GWLoginSession::Process()
{
	CG_TCPSession::Process();

	switch(GetState())
	{
	default:
		{
			time_t now = time(NULL);

			if (now - m_lastConnectTime >= GW_CONNECT_LOGIN_INTERVAL)
			{
				Close();

                NOTICE("Connect to login server. IP=%s Port=%d, BindIP=%s BindPort=%d\n",
                    (char*)g_cfgGW.connectLogin.ip.c_str(),
                    g_cfgGW.connectLogin.port,
                    (char*)g_cfgGW.connectLogin.bindIP.c_str(),
                    g_cfgGW.connectLogin.bindPort);

				CG_TCPSession::Connect(
                    (char*)g_cfgGW.connectLogin.ip.c_str(),
					g_cfgGW.connectLogin.port,
					(char*)g_cfgGW.connectLogin.bindIP.c_str(),
					g_cfgGW.connectLogin.bindPort);

				m_lastConnectTime = now;
			}
		}
		break;

	case NET_STATE_CONNECTING:
	case NET_STATE_WAITING_SEED:
		break;

	case NET_STATE_CONNECTED:
		break;
	}
}

bool GWLoginSession::LoginAccount(int seed,const char* account,const char* pwaccount,
								  const char* pubwinKey, const char* glompwd, 
								  const char* password,const char* ip/*PZH*/, bool bPowerLogin/**/)
{
	//PZH 移到前面一点处理
    //string szAccount = account;
    //strlwr((char*)szAccount.c_str());
	//
    string szAccount = account;
    strlwr((char*)szAccount.c_str());

	CG_CmdPacket& cmd = BeginCmd(gws2lgs_user_login);
	cmd.WriteLong(seed);
	cmd.WriteString((char*)szAccount.c_str());
	cmd.WriteString((char*)pwaccount);//tim.yang  secret username
	cmd.WriteString((char*)pubwinKey);
	cmd.WriteString((char*)glompwd);
	cmd.WriteString((char*)password);
	cmd.WriteString((char*)ip);
	
	//PZH
	char cPower;
	if(bPowerLogin)
	{
		cPower = 1;
	}
	else
	{
		cPower = 0;
	}
	cmd.WriteByte(cPower);
	//
	if (!SendCmd())
		return false;

    return true;
}

void GWLoginSession::UpdateState(char cUpdate)
{
    CG_CmdPacket& cmd = BeginCmd(gws2lgs_update_state);
    cmd.WriteByte(cUpdate);
    SendCmd();
}

bool GWLoginSession::LicenseCheck(int nType)
{
	//std::string		strIP;
	char			sIP[16];
	char			sMacAddr[6];
	std::string		strmac;

	if(!ReadMd5())
	{
		return false;
	}

#ifdef LINUX
	if(GetIPMacAddr((unsigned char*)sMacAddr, sIP) != 0)
	{
		LOG("LINUX Get IP/Mac Error\n");
		return false;
	}
#else
	// C [11/22/2008 Louis.Huang]
/*
	RtSystemInfo	sysInfo;
	if (sysInfo.Init())
	{
		SSystemInfo* info = sysInfo.GetSystemInfo();
		memcpy((void*)sIP, info->sIpAddr, sizeof(info->sIpAddr));
		//strIP = info->sIpAddr;
		memcpy(sMacAddr, info->sMacAddr, sizeof(sMacAddr));		// sMacAddr为8个字节（最后2个为0），而m_sMacAddress为6个字节
	}
	else
	{
		memcpy((void*)sIP, "defaultIP", strlen("defaultIP"));
		memset(sMacAddr, 0, 6);
	}
*/
#endif //LINUX

	std::string ipAddr = sIP;

	for(int i = 0; i < 6; i++)
	{
		strmac += rtFormatNumber((unsigned char)sMacAddr[i]);
	}

	CG_CmdPacket& cmd = BeginCmd(gws2lgs_license_check);
	cmd.WriteString((char*)ipAddr.c_str());
	cmd.WriteString((char*)strmac.c_str());
	cmd.WriteString((char*)m_strMd5.c_str());
	cmd.WriteLong((long)nType);
	SendCmd();

	return true;
}

bool GWLoginSession::ReadMd5()
{
	FILE*	fp = NULL;
	char	strmd5[33];

	fp = fopen("license.dat", "r");

	if(!fp)
	{
		LOG("File license.txt open error\n");
		return false;
	}

	for(int i = 0; i < 100; i++)
	{
		fgetc(fp);
	}

	fread(strmd5, 1, 32, fp);

	fclose(fp);

	strmd5[32] = '\0';

	m_strMd5 = strmd5;

	return true;
}

void GWLoginSession::LogoutAccount(DWORD accountId)
{	
	CG_CmdPacket& cmd = BeginCmd(gws2lgs_user_quit);
	cmd.WriteLong(accountId);
	SendCmd();
}

//extern char g_szExeMD5[40], g_szDataMD5[40];
extern G_MEMDEF(g_szExeMD5, 40)
extern G_MEMDEF(g_szDataMD5, 40)

void GWLoginSession::OnConnect(bool ret)
{
    if (ret)
    {
        CG_CmdPacket& cmd = BeginCmd(gws2lgs_check_version);
        cmd.WriteString(g_szExeMD5); // szExeMD5
        cmd.WriteString(g_szDataMD5); // szDataMD5
        cmd.WriteString(OS_TYPE_STR); // szOS
        SendCmd();

		//-------add start by tony 05.10.08------------//
#ifdef LICENSE_CHECK
		if(!g_server.m_login.LicenseCheck(1))
		{
			ERR("GW License Check Failed\n");
			g_server.Exit();
		}
#endif //LICENSE_CHECK
		//-------add end   by tony 05.10.08------------//
        NOTICE("Connect login server. SUCCEEDED at %d.\n", rtTimestamp());//timn.yang  %s->%d
    }else
    {
        MSG1("Connect login server. FAILEDED at %d.\n", rtTimestamp());//timn.yang  %s->%d
    }
}

void GWLoginSession::OnDisconnect()
{
    ERR1("Disconnect from login server at %d", rtTimestamp());//timn.yang  %s->%d
    g_server.OnAcceptLogin(false);
}

void GWLoginSession::OnReceive(CG_CmdPacket* packet)
{
	short cmdID;
	if(packet->ReadShort(&cmdID))
	{
		m_channel.Invoke(cmdID, packet);
	}
}

void GWLoginSession::SendShutdownOK(bool bForce)
{
	CG_CmdPacket& cmd = BeginCmd(gws2lgs_shutdown_ok);
    cmd.WriteByte(bForce);
	SendCmd();
}

void GWLoginSession::UpdateGameWorldStat()
{
    CG_CmdPacket& cmd = BeginCmd(gws2lgs_update_gws_state);
    g_server.m_stat.Serialize(&cmd, SERVER_STAT_VERSION);
    long i, lCnt = (long)g_server.m_regionTable.size();
    cmd.WriteLong(lCnt);
    for (i=0; i<lCnt; i++)
    {
        GWRegion* pRs = &(g_server.m_regionTable[i]);
        pRs->m_stat.sState = (pRs->sessionIndex!=-1)?1:0;
        cmd.WriteShort(pRs->m_stat.sState);
        if (pRs->m_stat.sState)
        {
            pRs->m_stat.sID = pRs->lRsID;
            pRs->m_stat.strHost = pRs->szListenClientIP;
            pRs->m_stat.sPort = pRs->lListenClientPort;
            cmd.WriteShort(pRs->m_stat.sID);
            pRs->m_stat.SerializeAll(&cmd, SERVER_STAT_VERSION);
        }else
        {
            pRs->m_stat.sID = (short)i;
            cmd.WriteShort(pRs->m_stat.sID);
        }
    }
    g_server.m_gmStat.sState = g_server.m_gm.GetValidSessionCount();
    g_server.m_gmStat.Serialize(&cmd, SERVER_STAT_VERSION);
    SendCmd();
}

long GWLoginSession::CheckSubServerVersion(const char* szServerName, const char* szExeMD5, const char* szDataMD5, const char* szOS)
{
    static long lCheckID = 0;
    lCheckID ++;

    CG_CmdPacket& cmd = BeginCmd(gws2lgs_check_sub_version);
    cmd.WriteLong(lCheckID);
    cmd.WriteString((char*)szServerName);
    cmd.WriteString((char*)szExeMD5); // szExeMD5
    cmd.WriteString((char*)szDataMD5); // szDataMD5
    cmd.WriteString((char*)szOS); // szOS
    SendCmd();
    return lCheckID;
}

void GWLoginSession::OnCheckSubServerVersion(long dwCheckID, bool bSame, const char* szServerName)
{
    ERR1("ERROR: check server [%s] version different.\n", szServerName);
    g_server.Exit();
}
