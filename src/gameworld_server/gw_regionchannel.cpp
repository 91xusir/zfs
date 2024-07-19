#include "gw_server.h"
#include "gw_cmd_union.h"
#include "gw_cmd_feod.h"
#include "gw_herolist.h"
#include "gw_unionlist.h"
//-----------add start by tony 05.06.20----------------------//
#include "gw_cmd_castlewar.h"
//-----------add end   by tony 05.06.20----------------------//
//-----------add start by rendh 2010.01.23----------------------//
#include "gw_cmd_LineSys.h"
//-----------add end   by rendh 2010.01.23----------------------//
#include "gw_prentice.h"
#include "gw_cmd_auction.h"
#include "gw_nwar.h"
#include "GW_ActivityManager.h"

GW_BEGIN_CMD(cmd_r2g_move_user)
{
	long userID;
	long x;
	long y;

	if (!packet->ReadLong(&userID))
		return eInvalidData;

	if (!packet->ReadLong(&x))
		return eInvalidData;

	if (!packet->ReadLong(&y))
		return eInvalidData;

	g_server.MoveUser(userID, x, y);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_move_useraround)
{
	long userID;
	long x;
	long y;
	short nRange;

	if (!packet->ReadLong(&userID))
		return eInvalidData;

	if (!packet->ReadLong(&x))
		return eInvalidData;

	if (!packet->ReadLong(&y))
		return eInvalidData;

	if (!packet->ReadShort(&nRange))
		return eInvalidData;

	g_server.MoveUserAround(userID, x, y, nRange);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_check_version)
{
    char *szExeMD5, *szDataMD5, *szOSType;
    packet->ReadString(&szExeMD5);
    packet->ReadString(&szDataMD5);
    packet->ReadString(&szOSType);

    bool bSame = true;
    long rgID;
    GWRegion *pRs;
    for (rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
    {
        pRs = &(g_server.m_regionTable[rgID]);
        if (pRs->bChecked)
        {
#ifndef _DEBUG
            if (strcmp(szOSType, pRs->szOsType.c_str())==0)
            {
                if (strcmp(szExeMD5, pRs->szExeMD5.c_str())!=0)
                {
                    bSame = false;
                }
            }
            if (strcmp(szDataMD5, pRs->szDataMD5.c_str())!=0)
            {
                bSame = false;
            }
#endif
        }
    }
    GWSession& se = g_server.m_region.GetSession();
    for (rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
    {
        if (g_server.m_regionTable[rgID].sessionIndex == se.index)
        {
            pRs = &(g_server.m_regionTable[rgID]);
            break;
        }
    }
    if (pRs==NULL)
    {
        bSame = false;
    }
    if (bSame)
    {
        pRs->szExeMD5 = szExeMD5;
        pRs->szDataMD5 = szDataMD5;
        pRs->szOsType = szOSType;
        pRs->bChecked = true;
    }else
    {
        ERR("Check RegionServer version FAILED.\n");
    }
    g_server.m_login.CheckSubServerVersion("Region", szExeMD5, szDataMD5, szOSType);
    CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_check_version_ret);
    cmd.WriteByte(bSame);
    g_server.m_region.SendToPeer(se.index);

	//LeiJun Region登陆，向其更新全部活动实例
	GW_ActivityManager::GetSingle()->UpdataActivityToRegion(se.index);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_update_region_state)
{
    GWSession& se = g_server.m_region.GetSession();
    long rgID;
    for (rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
    {
        if (g_server.m_regionTable[rgID].sessionIndex == se.index)
        {
            g_server.m_regionTable[rgID].m_stat.Serialize(packet, SERVER_STAT_VERSION);
            break;
        }
    }
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_start_fun)
{
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_start_fun);
	g_server.m_region.BroadcastCmd();
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_card_trade_switch)
{
    char cOnoff;
    packet->ReadByte(&cOnoff);

    // 公告开始/停止信息
    const char* szMsg;
    if (cOnoff)
    {
		szMsg = R(MSG_CARD_TRADE_BEGIN);
    }else
    {
		szMsg = R(MSG_CARD_TRADE_END);
    }
    g_server.BroadcastBulletin(szMsg);

    CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_card_trade_switch);
    cmd.WriteByte(cOnoff);
	g_server.m_region.BroadcastCmd();
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_end_fun)
{
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_end_fun);
	g_server.m_region.BroadcastCmd();
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_safe_quited)
{
    CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_safe_quited);
    g_server.m_region.SendCmd();
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_register)
{
	long  seed;
    long  rgID;
    char* pListenIP;

	if (!packet->ReadLong(&seed))
    {
        ERR("Region session error. command error\n");
        return eInvalidData;
    }

    if (!packet->ReadLong(&rgID))
    {
        ERR("Region session error. command error\n");
        return eInvalidData;
    }

    if (rgID<-1 || rgID>=(long)g_server.m_regionTable.size())
    {
        ERR2("Region session error. (region id %d out of range[-1, %d])\n", rgID, (int)g_server.m_regionTable.size());
        return eInvalidData;
    }

    GWSession& se = g_server.m_region.GetSession();

    if (rgID>=0)
    {
        if (g_server.m_regionTable[rgID].sessionIndex!=-1)
        {
            ERR1("Region session error. (region %d already exist)\n", rgID);
            return eInvalidData;
        }
        g_server.m_regionTable[rgID].sessionIndex = se.index;
    }else
    {
        for (rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
        {
            if (g_server.m_regionTable[rgID].sessionIndex == -1)
            {
                g_server.m_regionTable[rgID].sessionIndex = rgID;
                break;
            }
        }
        if (rgID==(long)g_server.m_regionTable.size())
        {
            ERR1("Region session error (region full[%d]).\n", rgID);
            return eInvalidData;
        }
    }

    // 临时把所有的Region都添加成为副本服务器，实际使用的时候需要修改Region分配的表格，根据表格决定
    if (g_cfgGW.listenRegion.mapDungeonRsID.size()==0)
    {
        g_server.m_dungeonMgr.AddDungeonRegion(rgID);
    }else
    {
        if (g_cfgGW.listenRegion.mapDungeonRsID.find(rgID)!=g_cfgGW.listenRegion.mapDungeonRsID.end())
        {
            g_server.m_dungeonMgr.AddDungeonRegion(rgID);
        }
    }

	//添加到LineSys
	g_server.m_LineMan.AddRegion(rgID);

    GWRegion& rg = g_server.m_regionTable[rgID];

    rg.lRsID = rgID;

    if (!packet->ReadString(&pListenIP))
    {
        ERR("Region session error. command error\n");
        return eInvalidData;
    }

    if (!packet->ReadLong(&rg.lListenClientPort))
    {
        ERR("Region session error. command error\n");
        return eInvalidData;
    }

    char ip[40];
    short port;
    if(se.session->GetSocket().GetRemoteAddr(ip, &port))
    {
        rg.szConnectGameworldIP = ip;
        rg.lConnectGameworldPort = port;
    }else
    {
        rg.szConnectGameworldIP = "unknown";
        rg.lConnectGameworldPort = 0;
    }
    if (strcmp(pListenIP, "localhost")==0 || strcmp(pListenIP, "127.0.0.1")==0)
    {
        rg.szListenClientIP = rg.szConnectGameworldIP;
    }else
    {
        rg.szListenClientIP = pListenIP;
    }

	const char* szIP = rg.szListenClientIP.c_str();
	NOTICE("Register region server [ id=%d host=%s port=%d ]\n", rgID, szIP, rg.lListenClientPort);

	bool resetRegion = false;

	if (rg.seed == seed)
	{
		if (rg.seed == 0)
		{
			rg.seed = time(NULL);
		}else
		{
			LOG("The region is running on resume mode\n");
		}
	}else
	{
		if (rg.seed)
		{
			LOG("Reset all users in this region \n");
			g_server.LogoutUserByRegion(rgID);
			rg.seed = 0;
		}

		if (seed)
		{
			LOG("The region is gone away, reboot the old region\n");
			resetRegion = true;
		}else
		{
			rg.seed = time(NULL);
		}
	}

	if (resetRegion)
	{
		g_server.m_region.NotifyCmd(g2r_reboot_region);
	}else
	{
		g_server.SendRegionInit(rgID);
		g_server.SendRegionList(rg.sessionIndex);
		g_server.BroadcastAddRegionList(rgID);

		g_server.OnRegionRegister(rgID);
	}
}
GW_END_CMD;

//
// region接受用户后发送，用户可能是第一次进入游戏或是切换region
//
GW_BEGIN_CMD(cmd_r2g_accept_user)
{
	long userID;

	if (!packet->ReadLong(&userID))
		return eOkay;

	GWLoginClient *lc = g_server.FindLoginClientByActorId(userID);
	if(!lc) return eOkay;

	lc->SetState(LCS_INGAME);
	if(lc->IsFirstSwitchRegion())
		g_server.OnUserEnterWorld(lc);		
	else
        g_server.OnUserSwitchRegionSucceeded(lc);

	//LeiJun 更新用户活动加入状态
	GW_ActivityManager::GetSingle()->GetUserEnterActivity(g_server.m_region.GetSession().index, userID);
}
GW_END_CMD;

//
// region 发现用户在限定时间内没有进行连接
//
GW_BEGIN_CMD(cmd_r2g_repatriate_user)
{
	long userID;

	if (!packet->ReadLong(&userID))
		return eOkay;	

	GWLoginClient *lc = g_server.FindLoginClientByActorId(userID);
	if(lc)
	{
        g_server.UserQuit(lc);
	}
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_update_user)
{
	long userID;
	if (!packet->ReadLong(&userID))
		return eOkay;
	
	GWUser* user = g_server.FindActorById(userID);
	if (user == NULL)
		return eOkay;

	user->Serialize(*packet,false);
	user->isModified = true;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_Is_Open)
{
	short IsOpen;
	long MailID;
	if (!packet->ReadShort(&IsOpen))
		return eOkay;
	if (!packet->ReadLong(&MailID))
		return eOkay;
	g_server.m_mail.Update(IsOpen,MailID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_MailItem)
{
	long MailID;

	if (!packet->ReadLong(&MailID))
		return eOkay;

	g_server.m_mail.MoveItem(MailID);

}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_add_mail)
{
	long senderID;
	long receiverID;
	char* senderName;
	char* title;
	char* content;
    char* StrItem;
	long Money; 

	if (!packet->ReadLong(&senderID))
		return eOkay;

	if (!packet->ReadLong(&receiverID))
		return eOkay;

	if (!packet->ReadString(&senderName))
		return eOkay;

	if (!packet->ReadString(&title))
		return eOkay;

	if (!packet->ReadString(&content))
		return eOkay;

	if (!packet->ReadString(&StrItem))
		return eOkay;

	if (!packet->ReadLong(&Money))
		return eOkay;

	/*SItemID *piItemID = (*piItemID)(ItemID)*/

	g_server.m_mail.AddMail(receiverID, senderID, title, content, true,StrItem,Money);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_remove_mail)
{
	long MailID;

	if (!packet->ReadLong(&MailID))
		return eOkay;

	g_server.m_mail.RemoveMail(MailID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_SystemNews)
{
	char* Playername;
	char* Name;
	char* BossName;
	char* SceneName;
	long  type;
	char  channel;
	char* ItemName;
		
	if (!packet->ReadString(&Playername))
		return eOkay;
	if (!packet->ReadString(&Name))
		return eOkay;
	if (!packet->ReadString(&BossName))
		return eOkay;
	if (!packet->ReadString(&SceneName))
		return eOkay;
	if (!packet->ReadLong(&type))
		return eOkay;
	if (!packet->ReadByte(&channel))
		return eOkay;
	if (!packet->ReadString(&ItemName))
		return eOkay;

	g_server.m_chatTransmittal.ChatNewS(Playername,Name,BossName,SceneName,type,channel,ItemName);
	
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_chat) //Tian GW接收聊天消息
{
	long toID;
	char* toName;	
	char* fromName;
	char channel;
	char* content;
	char* title;
	long  oldId = 0;

	if (!packet->ReadLong(&toID))
		return eOkay;

	if (!packet->ReadString(&toName))
		return eOkay;

	if (!packet->ReadString(&fromName))
		return eOkay;

	if (!packet->ReadByte(&channel))
		return eOkay;

	if (!packet->ReadString(&content))
		return eOkay;

	if (!packet->ReadString(&title))
		return eOkay;

	if (!packet->ReadLong(&oldId))
		return eOkay;

	switch(channel)
	{
	case CHAT_CHANNEL_NORMAL:
	case CHAT_CHANNEL_SYSTEM:
		CHECK(false);
		break;

	case CHAT_CHANNEL_PUBLIC:
		g_server.m_chatTransmittal.ChatPublic(toID, toName, fromName, channel, content,oldId);
		break;
		//heten
	case CHAT_CHANNEL_WORLD:
		g_server.m_chatTransmittal.ChatWorld(toID, toName, fromName, channel, content);
		break;
		//end
	case CHAT_CHANNEL_TEAM:
		g_server.m_chatTransmittal.ChatTeam(toID, toName, fromName, channel, content);
		break;
	case CHAT_CHANNEL_PRIVATE:
		g_server.m_chatTransmittal.ChatPrivate(toID, toName, fromName, channel, content);
		break;

	case CHAT_CHANNEL_PARTY:
		g_server.m_chatTransmittal.ChatParty(toID, fromName, content);
		break;

	case CHAT_CHANNEL_COUNTRY:
		// @@@@
//		g_server.m_chatTransmittal.ChatCountry(toID, fromName, content);
//		break;

	case CHAT_CHANNEL_GM:
		{
			GWUser *pUser = g_server.FindActorByName(fromName);
			if (pUser)
			{
				CG_CmdPacket& cmd = g_server.m_gm.BeginCmd(gw2gm_chat);
				cmd.WriteString("Unknown Server");
				cmd.WriteString(fromName);
				cmd.WriteLong(toID);
				cmd.WriteString(content);
				if (!title || title[0]=='\0')
					cmd.WriteByte(0);
				else 
				{
					cmd.WriteByte(1);
					cmd.WriteString(title);
				}
				g_server.m_gm.SendCmd();
			}
		}
		break;
	}
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_union)
{
	GW_UnionCmdProcess(packet);
}
GW_END_CMD;
//----------------add start by tony 05.06.16-----------------------//
GW_BEGIN_CMD(cmd_r2g_castlewar)
{
	GW_CastleWarCmdProcess(packet);
}
GW_END_CMD;
//----------------add end   by tony 05.06.16-----------------------//
//----------------add start by rendh 2010.01.23-----------------------//
GW_BEGIN_CMD(cmd_r2g_linesys)
{
	GW_LineSysCmdProcess(packet);
}
GW_END_CMD;
//----------------add end   by rendh 2010.01.23-----------------------//


GW_BEGIN_CMD(cmd_r2g_auction)
{
	GW_AuctionCmdProcess(packet);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_feod)
{
	GW_FeodCmdProcess(packet);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_logout)
{
	long userID;
	packet->ReadLong(&userID);
	GWLoginClient *lc = g_server.FindLoginClientByActorId(userID);
	if(lc)
	{
		g_server.UserQuit(lc);
	}else
    {
        ERR1("cmd_r2g_logout error: Cannot found user ID %d\n", userID);
    }
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_switch_region)
{
	long userID;
	long seed;

	if (!packet->ReadLong(&userID)) return eOkay;
	if (!packet->ReadLong(&seed)) return eOkay;

	GWLoginClient *lc = g_server.FindLoginClientByActorId(userID);
	if(!lc)
		return eOkay;

	if(!g_server.DistributeUser(lc,seed,false))
	{
		g_server.UserQuit(lc);
	}
	g_server.OnUserSwitchRegion(userID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_herolist_query)
{
	long listVersion;
	if ( !packet->ReadLong(&listVersion) )			return eOkay;

	if (GetHeroList()->m_data.m_version!=listVersion)
	{
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_herolist);
		cmd.WriteLong(GetHeroList()->m_data.m_version);
		GetHeroList()->m_data.Serialize(&cmd);
		g_server.m_region.SendCmd();
	}
	else
	{
		GetHeroList()->OnRefresh();
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_log_event)
{
	// @@@@

	char BlankString[] = "";
	char logType;
	long userID;
	char *userIpAddress;
	long longParam1 = 0, longParam2 = 0, longParam3 = 0;
	char *strParam1 = BlankString, *strParam2 = BlankString, *strParam3 = BlankString;
	if ( !packet->ReadByte(&logType) )			return eOkay;
	if ( !packet->ReadLong(&userID) )			return eOkay;
	if ( !packet->ReadString(&userIpAddress))	return eOkay;
	switch (logType)
	{
	case PLT_Trade:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
			if ( !packet->ReadLong(&longParam3) )		return eOkay;
			if ( !packet->ReadString(&strParam3) )		return eOkay;
		}
		break;

	case PLT_MoveTo:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
		}
		break;

	case PLT_FollowPlayer:
		{
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;

	case PLT_MovePlayer:
		{
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;

	case PLT_AddItem:
		{
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;

	case PLT_AddSkill:
		{
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;

	case PLT_Monitor:
		{
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam3) )		return eOkay;
		}
		break;

	case PLT_GmCommand:
		{
			if ( !packet->ReadString(&strParam1))		return eOkay;
			if ( !packet->ReadString(&strParam2))		return eOkay;
		}
		break;

	case PLT_HackOperate:
		{
			if ( !packet->ReadString(&strParam1))		return eOkay;
		}
		break;

	case PLT_DeleteItem:
		{
			if ( !packet->ReadString(&strParam1))		return eOkay;
		}
		break;

	case PLT_ThrowItem:
		{
			if ( !packet->ReadString(&strParam1))		return eOkay;
		}
		break;

	case PLT_PickItem:
		{
			if ( !packet->ReadString(&strParam1))		return eOkay;
			if ( !packet->ReadLong(&longParam1))		return eOkay;
			if ( !packet->ReadLong(&longParam2))		return eOkay;
		}
		break;

	case PLT_BuyFromPShop:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadLong(&longParam3) )		return eOkay;
			if ( !packet->ReadString(&strParam3) )		return eOkay;
		}
		break;

	case PLT_PShopSell:
		// 不会收到此类型的Log，直接利用PLT_BuyFromPShop的数据来存储PLT_PShopSell
		ERR("Recv a invalid log type:PLT_PShopSell\n");
		break;

	case PLT_UserState:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
		}
		break;

	case PLT_Bank:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
		}
		break;

	case PLT_BuyFromNpc:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadLong(&longParam3) )		return eOkay;
		}
		break;

	case PLT_SellToNpc:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
		}
		break;

	case PLT_ItemUpdate:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )       return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
			if ( !packet->ReadString(&strParam3) )		return eOkay;
		}
		break;

	case PLT_WithdrawFeodBank:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
		}
		break;

	case PLT_UnionOperate:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;

	case PLT_PKDrop:
		{
			if ( !packet->ReadString(&strParam1) )		return eOkay;
		}
		break;

	case PLT_RemoveIllegalItem:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
		}
		break;
		
	case PLT_MagicEgg:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;
	case PLT_UseItem:
		{
			if (!packet->ReadLong(&longParam1))
			{
				return eOkay;
			}
		}
		break;
	case PLT_PointCast:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
		}
		break;
	case PLT_PointGet:
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
		}
		break;
	case PLT_LevelChanges:
		{
			if (!packet->ReadLong(&longParam1))         return eOkay;
			if (!packet->ReadLong(&longParam2))         return eOkay;
			if (!packet->ReadLong(&longParam3))         return eOkay;
		}
		break;
	//case PLT_SystemMessages:
	//	{
	//		if (!packet->ReadLong(&strParam1))         return eOkay;
	//		if (!packet->ReadLong(&strParam2))         return eOkay;
	//	}
	//	break;
	default:
		//ERR1("Recv a invalid log type:%d\n", (int)logType);
		//return eOkay;
		{
			if ( !packet->ReadLong(&longParam1) )		return eOkay;
			if ( !packet->ReadString(&strParam1) )		return eOkay;
			if ( !packet->ReadLong(&longParam2) )		return eOkay;
			if ( !packet->ReadString(&strParam2) )		return eOkay;
			if ( !packet->ReadLong(&longParam3) )		return eOkay;
			if ( !packet->ReadString(&strParam3) )		return eOkay;
		}
		break;
	}

	if (!g_server.LogUserEvent(logType, userID, userIpAddress, longParam1, strParam1, longParam2, strParam2, longParam3, strParam3))
	{
		return eError;
	}

	// 一些特殊类型记录的处理
	if (logType==PLT_Trade)
	{
		// 交易记录还要再记录一条
		if (!g_server.LogUserEvent(logType, longParam2, userIpAddress, longParam1, strParam1, longParam2, strParam2, longParam3, strParam3))
		{
			return eError;
		}
	}
	else if (logType==PLT_BuyFromPShop)
	{
		// 直接利用PLT_BuyFromPShop的数据来存储PLT_PShopSell
		if (!g_server.LogUserEvent(PLT_PShopSell, longParam2, userIpAddress, longParam1, strParam1, longParam2, strParam2, longParam3, strParam3))
		{
			return eError;
		}
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_gm_additem_event)
{
	return eError;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_gm_move)
{
	guard;

	long GmID;
	char moveType;
	if ( !packet->ReadLong(&GmID) )				return eOkay;
	if ( !packet->ReadByte(&moveType) )			return eOkay;
	switch (moveType)
	{
	case 1:	// MoveToUser
		{
			char* username;
			if ( !packet->ReadString(&username)) return eOkay;
			GWUser* pUser = g_server.FindActorByName(username);
			if (!pUser)
				return eOkay;
			g_server.PullUser(pUser->userID, GmID);
		}
		break;
	case 2: // MoveToPlace
		{
			long x, y;
			if (!packet->ReadLong(&x))			return eOkay;
			if (!packet->ReadLong(&y))			return eOkay;
			g_server.MoveUser(GmID, x, y);
		}
		break;
	case 3: // MoveUserToHere
		{
			char* username;
			if ( !packet->ReadString(&username)) return eOkay;
			GWUser* pUser = g_server.FindActorByName(username);
			if (!pUser)
				return eOkay;
			g_server.PullUser(GmID, pUser->userID);
		}
		break;
	case 4: // MoveUserAroundToHere
		{
			char* username;
			short nRange;
			if ( !packet->ReadString(&username))	return eOkay;
			if ( !packet->ReadShort(&nRange))		return eOkay;
			GWUser* pUser = g_server.FindActorByName(username);
			if (!pUser)
				return eOkay;
			g_server.PullUserAround(GmID, pUser->userID, nRange);
		}
		break;
	case 5: // MoveUserToPlace
		{
			char* username;
			long x, y;
			if (!packet->ReadString(&username)) return eOkay;
			if (!packet->ReadLong(&x))			return eOkay;
			if (!packet->ReadLong(&y))			return eOkay;
			GWUser* pUser = g_server.FindActorByName(username);
			if (!pUser)
				return eOkay;
			g_server.MoveUser(pUser->userID, x, y);
		}
		break;
	default:
		return eOkay;
	}

	return eOkay;
	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_move_to)
{
	guard;
	long MoveUserID;
    long x, y;

    if (!packet->ReadLong(&MoveUserID) )    return eOkay;
    if (!packet->ReadLong(&x))			    return eOkay;
    if (!packet->ReadLong(&y))			    return eOkay;
    g_server.MoveUser(MoveUserID, x, y);
	return eOkay;
	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_save_itemid)
{
	long regionID;
	long itemID;
	if ( !packet->ReadLong(&regionID) )			return eOkay;
	if ( !packet->ReadLong(&itemID) )			return eOkay;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "UPDATE T_ItemID SET ItemID=%d WHERE RegionID=%d", itemID, regionID);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return eError;
	}
	else
	{
		mysql_free_result(pDBResult);
		return eOkay;
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_query_itemid)
{
	long regionID;
	if ( !packet->ReadLong(&regionID) )			return eOkay;

	g_GWItemIDMgr.SendIDData(regionID);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_PakeBlack_List)
{
    long SenderID = 0;
	long UserID = 0;
	short Type = 0;

	if (!packet->ReadLong(&SenderID))		    return eInvalidData;
	if (!packet->ReadLong(&UserID))		        return eInvalidData;
	if (!packet->ReadShort(&Type))		        return eInvalidData;

	GWUser* user = g_server.FindActorById(UserID);
	if (!user)
		return eOkay;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_PakeBlack_List);

	int se = g_server.GetRegionSession(user->regionID);
	if (se == -1)
		return eOkay;

	cmd.WriteLong(UserID);
	cmd.WriteLong(SenderID);
	cmd.WriteShort(Type);

	g_server.m_region.SendToPeer(se);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_Black_List)
{
	char* Name;
	long UserID;
	long SenderID;
	if (!packet->ReadString(&Name))	        return eInvalidData;
	if (!packet->ReadLong(&UserID))		    return eInvalidData;

	GWUser* Senduser = g_server.FindActorByName(Name);
	if (Senduser)
	{
		SenderID = Senduser->userID;
	}

	GWUser* user = g_server.FindActorById(UserID);
	if (!user)
		return eOkay;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_Black_List);

	int se = g_server.GetRegionSession(user->regionID);
	if (se == -1)
		return eOkay;

	cmd.WriteLong(UserID);
	cmd.WriteLong(SenderID);
	cmd.WriteString(Name);


	g_server.m_region.SendToPeer(se);

	return eOkay;



}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_one_refresh)
{
	long userID;
	long SenderID;
	char type;
	if (!packet->ReadLong(&userID))	        return eInvalidData;
	if (!packet->ReadLong(&SenderID))		return eInvalidData;
	if (!packet->ReadByte(&type))		    return eInvalidData;

	GWUser* user = g_server.FindActorById(userID);
	//Tianh 如果找不到玩家  玩家处于下线的状态
	if (user == NULL)
	{
		if (type == 2)
		{
			GWUser* Senduser = g_server.FindActorById(SenderID);

			if (!Senduser)
			{
				return eOkay;
			}

			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_notall_found);

			int se = g_server.GetRegionSession(Senduser->regionID);
			if (se == -1)
				return eOkay;

			cmd.WriteLong(SenderID);
			cmd.WriteLong(userID);

			g_server.m_region.SendToPeer(se);

			return eOkay;
		}


		GWUser* Senduser = g_server.FindActorById(SenderID);

		if (!Senduser)
		{
			return eOkay;
		}

		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_not_found);

		int se = g_server.GetRegionSession(Senduser->regionID);
		if (se == -1)
			return eOkay;

		cmd.WriteLong(SenderID);
		cmd.WriteLong(userID);

		g_server.m_region.SendToPeer(se);

		return eOkay;
	}	
	/*long UserID = user->userID;*/

	int se = g_server.GetRegionSession(user->regionID);
	if (se == -1)
		return eOkay;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_one_refresh);

	cmd.WriteLong(userID);
	cmd.WriteLong(SenderID);
	cmd.WriteByte(type);

	g_server.m_region.SendToPeer(se);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_when_fiend)
{
	long SenderID;
	long userID;

	if (!packet->ReadLong(&SenderID))	        return eInvalidData;
	if (!packet->ReadLong(&userID))		        return eInvalidData;

	GWUser* Senduser = g_server.FindActorById(SenderID);

	if (!Senduser)
	{
		return eOkay;
	}

	string Name = "";
	short metier = 0;
	if (Senduser)
	{
		Name = Senduser->unionName;
		metier = Senduser->attr.metier;
	}
	else
	{
		return eOkay;
	}
	GWUser* user = g_server.FindActorById(userID);
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_when_fiend);

	int se = g_server.GetRegionSession(user->regionID);
	if (se == -1)
		return eOkay;

	cmd.WriteLong(userID);
	cmd.WriteLong(SenderID);
	cmd.WriteString((char*)Name.c_str());
	cmd.WriteShort(metier);
	

	g_server.m_region.SendToPeer(se);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_fiend_name_ID)
{
	char* name;
	long SenderID;
	if (!packet->ReadString(&name))	        return eInvalidData;
	if (!packet->ReadLong(&SenderID))		return eInvalidData;

	GWUser* user = g_server.FindActorByName(name);
	if (user == NULL)
	{
		//邀请的好友不在线  或者不存在
		GWUser* SendUser = g_server.FindActorById(SenderID);
		if (SendUser == NULL)
			return eOkay;

		int se = g_server.GetRegionSession(SendUser->regionID);
		if (se == -1)
			return eOkay;

		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_UserID_return);

		/*cmd.WriteLong(UserID);*/
		cmd.WriteLong(SenderID);			

		g_server.m_region.SendToPeer(se);

		return eOkay;
	}


		
	long UserID = user->userID;


	GWUser* SendUser = g_server.FindActorById(SenderID);
	if (SendUser == NULL)
		return eOkay;

	int se = g_server.GetRegionSession(SendUser->regionID);
	if (se == -1)
		return eOkay;

	if (user->m_bRefuseFriend)  //对方设置了拒绝加入好友
	{
		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_SetRefuse_friend);

		cmd.WriteLong(SenderID);

		g_server.m_region.SendToPeer(se);

		return eOkay;
	}

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_friend_UserID);

	cmd.WriteLong(SenderID);
	cmd.WriteLong(UserID);
				
	g_server.m_region.SendToPeer(se);

}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_OneFriend_Return)
{
	long SenderID;	
	long UserID;	
	char* SceneName;
	long Lev;
	char Metier;
	char Type;

	if (!packet->ReadLong(&SenderID))		return eInvalidData;
	if (!packet->ReadLong(&UserID))	        return eInvalidData;
	if (!packet->ReadString(&SceneName))	return eInvalidData;
	if (!packet->ReadLong(&Lev))		    return eInvalidData;
	if (!packet->ReadByte(&Metier))		    return eInvalidData;
	if (!packet->ReadByte(&Type))		    return eInvalidData;


	GWUser* user = g_server.FindActorById(SenderID);

	if (user == NULL)
		return eOkay;

	int se = g_server.GetRegionSession(user->regionID);

	if (se == -1)
		return eOkay;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_onefriend_return);

	cmd.WriteLong(SenderID);	
	cmd.WriteLong(UserID);	
	cmd.WriteString((char*)SceneName);
	cmd.WriteLong(Lev);	
	cmd.WriteByte(Metier);
	cmd.WriteByte(Type);	

	g_server.m_region.SendToPeer(se);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_friend)
{
	long UserDBID;
	char Operation;
	long DBIDInMessage;
	long FromDBID;
	char* pFromName;
	long Parameter;
	char Online;
	char* pName;
	char IsFristOnline;

	if (!packet->ReadLong(&UserDBID))		return eInvalidData;
	if (!packet->ReadByte(&Operation))		return eInvalidData;
	if (!packet->ReadLong(&DBIDInMessage))	return eInvalidData;
	if (!packet->ReadLong(&FromDBID))		return eInvalidData;
	if (!packet->ReadString(&pFromName))	return eInvalidData;
	if (!packet->ReadLong(&Parameter))		return eInvalidData;
	if (!packet->ReadByte(&Online))			return eInvalidData;
	if (!packet->ReadString(&pName))		return eInvalidData;
	if (!packet->ReadByte(&IsFristOnline))		return eInvalidData;

	GWUser* user = g_server.FindActorById(UserDBID);

	if (user == NULL)
		return eOkay;

	int se = g_server.GetRegionSession(user->regionID);

	if (se == -1)
		return eOkay;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_friend);

	cmd.WriteLong(UserDBID);
	cmd.WriteByte(Operation);		
	cmd.WriteLong(DBIDInMessage);	
	cmd.WriteLong(FromDBID);		
	cmd.WriteString(pFromName);		
	cmd.WriteLong(Parameter);		
	cmd.WriteByte(Online);			
	cmd.WriteString(pName);	
	cmd.WriteByte(IsFristOnline);
	cmd.WriteByte(0);

	g_server.m_region.SendToPeer(se);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_team)
{
	g_server.m_team.ReceiveFromRegion(packet);
}
GW_END_CMD;


GW_BEGIN_CMD(cmd_r2g_master_list)
{
	g_server.m_master_list_manager.ReceiveFromRegion(packet);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_enter)
{
	long dwUserID;
	char nFaction;
	if (!packet->ReadLong(&dwUserID))     
		return eOkay;
	/*add by Tianh*09.12.22*/
	if (!packet->ReadByte(&nFaction))
	{
		return eOkay;
	}
	GWUser*	pUser = g_server.FindActorById(dwUserID);
	pUser->attr.faction = nFaction;
	if (!g_GWWarfieldMgr.EnterWarfield(dwUserID))
	{
		g_server.m_chatTransmittal.Chat(g_server.m_region.GetSession().index, dwUserID, 
			R(MSG_NATIONAL_WAR_FIELD), CHAT_CHANNEL_SYSTEM, g_GWWarfieldMgr.m_szErrorInfo);
	}else
	{
		g_GWWarfieldMgr.RemoveUserInApp(dwUserID);
	}
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_create_arms)
{
	long dwUserID;
	long dwLev;
	long dwType;

	if (!packet->ReadLong(&dwUserID))      return eOkay;
	if (!packet->ReadLong(&dwLev))	       return eOkay;
	if (!packet->ReadLong(&dwType))		   return eOkay;

	//g_GWWarfieldMgr.OnCreateArms(dwUserID, dwLev, dwType);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_addscore)
{
	char faction;
	float score;

	if (!packet->ReadByte(&faction))	return eOkay;
	if (!packet->ReadFloat(&score))		return eOkay;
	
	g_GWWarfieldMgr.OnAddScore(faction, score);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_update_exploit)
{
	char faction;
	long lUnionID;
	long lExploit;

	if (!packet->ReadByte(&faction))	return eOkay;
	if (!packet->ReadLong(&lUnionID))	return eOkay;
	if (!packet->ReadLong(&lExploit))	return eOkay;

	g_GWWarfieldMgr.OnRecvExploit(faction, lUnionID, lExploit);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_add_exp)
{
	long lUserID;
	char faction;

	if (!packet->ReadLong(&lUserID))	return eOkay;
	if (!packet->ReadByte(&faction))	return eOkay;

	g_GWWarfieldMgr.OnAddExp(lUserID, faction);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_user_change_block)
{
	long lUserID;
	char* oldBlockFileName;
	char* newBlockFileName;

	if (!packet->ReadLong(&lUserID))	return eOkay;
	if (!packet->ReadString(&oldBlockFileName))	return eOkay;
	if (!packet->ReadString(&newBlockFileName))	return eOkay;

	GWUser *pUser = g_server.FindActorById(lUserID);
	if (!pUser) return eOkay;

	CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_user_change_block);
    packet.WriteString((char*)pUser->accountName.c_str());
    packet.WriteLong(pUser->accountId);
	packet.WriteLong(lUserID);
	packet.WriteString(oldBlockFileName);
	packet.WriteString(newBlockFileName);
	g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_add_per_exploit)
{
	long lUserID;
	long lExploit;

	if (!packet->ReadLong(&lUserID))	return eOkay;
	if (!packet->ReadLong(&lExploit))		return eOkay;

	g_GWWarfieldMgr.OnAddPersonExploit(lUserID, lExploit);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_tmp_per_exploit)
{
	long lUserID;
	long lExploit;

	if (!packet->ReadLong(&lUserID))	return eOkay;
	if (!packet->ReadLong(&lExploit))		return eOkay;

	g_GWWarfieldMgr.OnRecvPerExploit(lUserID, lExploit);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_conjure_progress)
{
	char faction;
	float progress;
	long lev;

	if (!packet->ReadByte(&faction))	return eOkay;
	if (!packet->ReadFloat(&progress))	return eOkay;
	if (!packet->ReadLong(&lev))		return eOkay;

	g_GWWarfieldMgr.OnRecvConjureProgress(faction, progress, lev);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_leave)
{
	long dwUserID;
	if (!packet->ReadLong(&dwUserID))       return eOkay;

	g_GWWarfieldMgr.OnUserLeave(dwUserID);
	
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_start_rep)
{
	long bResult = 0;
	if (!packet->ReadLong(&bResult))    return eOkay;
	g_GWWarfieldMgr.OnRsStartRep(bResult!=0);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_stop_rep)
{
	long bResult = 0;
	if (!packet->ReadLong(&bResult))    return eOkay;
	g_GWWarfieldMgr.OnRsStopRep(bResult!=0);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_open)
{
	g_GWWarfieldMgr.OnGMStart();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_warfield_stop)
{
	g_GWWarfieldMgr.OnGMStop();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_auction_auction)
{
    long lUserID, dwDuration, lBasePrice, lUniquePrice;
    SItemID id;
    packet->ReadLong(&lUserID);
    id.Serialize(packet);
    packet->ReadLong(&dwDuration);
    packet->ReadLong(&lBasePrice);
    packet->ReadLong(&lUniquePrice);
    g_server.m_gwAuction.Auction(lUserID, id, dwDuration, lBasePrice, lUniquePrice);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_auction_cancel_auction)
{
    long lUserID, itemID;
    packet->ReadLong(&lUserID);
    packet->ReadLong(&itemID);
    g_server.m_gwAuction.CancelAuction(lUserID, itemID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_auction_cancel_bid)
{
    long lUserID, itemID;
    packet->ReadLong(&lUserID);
    packet->ReadLong(&itemID);
    g_server.m_gwAuction.CancelBid(lUserID, itemID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_auction_get_items)
{
    long lUserID;
    long lFreeSpace;
    packet->ReadLong(&lUserID);
    packet->ReadLong(&lFreeSpace);
    g_server.m_gwAuction.GetItems(lUserID, lFreeSpace);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_auction_get_items_ret)
{
    long lUserID, lMoney = 0;
    char cMoney, cItem;
    SItemID itemID;

    packet->ReadLong(&lUserID);
    packet->ReadByte(&cMoney);
    if (cMoney)
    {
        packet->ReadLong(&lMoney);
    }else
    {
        lMoney = 0;
    }
    packet->ReadByte(&cItem);
    if (cItem==1)
    {
        itemID.Serialize(packet);
    }else
    {
        cItem = 0;
        itemID.Clear();
    }

    g_server.m_gwAuction.OnGetItemRet(lUserID, cMoney, lMoney, cItem, itemID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_auction_bid)
{
    long lUserID, lBidPrice, itemID;
    packet->ReadLong(&lUserID);
    packet->ReadLong(&lBidPrice);
    packet->ReadLong(&itemID);
    g_server.m_gwAuction.Bid(lUserID, lBidPrice, itemID);
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_kick)
{
	long gmID;
	char* guyName;
	if (!packet->ReadLong(&gmID))			return eOkay;
	if (!packet->ReadString(&guyName))		return eOkay;

	GWUser *pUser = g_server.FindActorByName(guyName);
	if (!pUser)		return eOkay;

	g_server.KickAccount(pUser->accountId, KICK_USER_GM);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_tester_ret)
{
    long rgID;
    GWRegion *pRs;
    GWSession& se = g_server.m_region.GetSession();
    for (rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
    {
        if (g_server.m_regionTable[rgID].sessionIndex == se.index)
        {
            pRs = &(g_server.m_regionTable[rgID]);
            pRs->m_cTestState = 2;
            break;
        }
    }
    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_query_user_info)
{
	long gmID;
	char* guyName;
	if (!packet->ReadLong(&gmID))			return eOkay;
	if (!packet->ReadString(&guyName))		return eOkay;

	GWUser *pUser = g_server.FindActorByName(guyName);
	if (!pUser)		return eOkay;

	int se = g_server.GetRegionSession(pUser->regionID);
	if (se == -1)
		return false;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_query_user_info);
	cmd.WriteLong(gmID);
	cmd.WriteLong(pUser->userID);
	g_server.m_region.SendToPeer(se);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_user_info)
{
	long gmID;
	SCreatureInfo info;
	if (!packet->ReadLong(&gmID))			return eOkay;
	if (!info.Serialize(packet))			return eOkay;

	GWUser *pUser = g_server.FindActorById(gmID);
	if (!pUser)		return eOkay;

	int se = g_server.GetRegionSession(pUser->regionID);
	if (se == -1)
		return false;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_user_info);
	cmd.WriteLong(gmID);
	info.Serialize(&cmd);
	g_server.m_region.SendToPeer(se);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_forbid_user)
{
	char* username;
	char bForbid;
	long forbidType;
	long forbidTime;
	if (!packet->ReadByte(&bForbid))		return eOkay;
	if (!packet->ReadString(&username))		return eOkay;
	if (!packet->ReadLong(&forbidType))		return eOkay;
	if (!packet->ReadLong(&forbidTime))		return eOkay;

	GWUser *pUser = g_server.FindActorByName(username);
	if (!pUser)		return eOkay;

	if (bForbid)
		g_server.ForbidUser(pUser->userID, (ECreatureState)forbidType, forbidTime);
	else 
		g_server.UnForbidUser(pUser->userID, (ECreatureState)forbidType);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_query_gifts)
{
	long userID;
	long level;
	if (!packet->ReadLong(&userID))		return eOkay;
	if (!packet->ReadLong(&level))		return eOkay;

	DWORD dwTick = rtGetMilliseconds();
	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)		return eOkay;

	// 在锁定时间内禁止取礼品
	if (pUser->dwWithdrawGiftLockTick != 0)
	{
		if (dwTick - pUser->dwWithdrawGiftLockTick < 8000) // 8秒钟超时
			return eOkay;
		else 
			pUser->dwWithdrawGiftLockTick = 0;
	}

	std::vector<SGift> giftsCanWithdraw;
	std::vector<SGift> giftsCanNotWithdraw;
	GetGiftMgr()->GetGifts(userID, level, giftsCanWithdraw, giftsCanNotWithdraw);

	if (!giftsCanWithdraw.empty())
		pUser->dwWithdrawGiftLockTick = dwTick;

	GetGiftMgr()->NetSend_g2r_query_gifts_response(userID, giftsCanWithdraw, giftsCanNotWithdraw);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_withdraw_gift_success)
{
	long userID;
	SGift gift;
	if (!packet->ReadLong(&userID))		return eOkay;
	if (!gift.Serialize(packet))		return eOkay;

	GWUser *pUser = g_server.FindActorById(userID);
	if (!pUser)		return eOkay;

	pUser->dwWithdrawGiftLockTick = 0;

	GetGiftMgr()->OnRecvWithdrawSuccess(userID, gift);

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_create_dungeon_ret)
{
    long lID;
    long bResult = 0;
	if (!packet->ReadLong(&lID))        return eOkay;
	if (!packet->ReadLong(&bResult))    return eOkay;
    g_server.m_dungeonMgr.OnCreateDungeonRet(lID, bResult!=0);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_enter_dungeon)
{
    long dwUserID, lDungeonType, lDungeonLevel, lDungeonUserLevel, lExitX, lExitY;
	if (!packet->ReadLong(&dwUserID))       return eOkay;
	if (!packet->ReadLong(&lDungeonType))   return eOkay;
	if (!packet->ReadLong(&lDungeonLevel))  return eOkay;
	if (!packet->ReadLong(&lDungeonUserLevel))  return eOkay;
	if (!packet->ReadLong(&lExitX))  return eOkay;
	if (!packet->ReadLong(&lExitY))  return eOkay;
    if (g_server.m_dungeonMgr.EnterDungeon(dwUserID, lDungeonType, lDungeonLevel, lDungeonUserLevel, lExitX, lExitY)==NULL)
    {
		g_server.m_chatTransmittal.Chat(g_server.m_region.GetSession().index, dwUserID,
			R(MSG_DUNGEON), CHAT_CHANNEL_SYSTEM, g_server.m_dungeonMgr.m_szErrorInfo);
    }
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_modify_maxdungeon)
{
	long userId = 0;
	long maxdungeon = 0;
	int listDungeonSize = 0;
	if (!packet->ReadLong(&userId))         return eOkay;
	if (!packet->ReadLong(&maxdungeon))       return eOkay;
	listDungeonSize = g_server.m_dungeonMgr.GetListDungeonSize();
	g_server.m_dungeonMgr.SetMaxDungeon(maxdungeon);
	char buf[512];
	memset(buf,0,sizeof(buf));
	rt2_sprintf(buf,"您设置的最大副本数是%d, 当前的副本数量是%d",maxdungeon,listDungeonSize);
	g_server.SendSystemMessageToUser(userId,buf);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_leave_dungeon)
{
    long dwUserID;
	if (!packet->ReadLong(&dwUserID)) return eOkay;
    if (!g_server.m_dungeonMgr.LeaveDungeon(dwUserID))
    {
        CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_leave_dungeon_ret);
        cmd.WriteLong(dwUserID);
        g_server.m_region.SendToPeer(g_server.m_region.GetSession().index, &cmd);
    }
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_destroy_dungeon)
{
    long dwDungeonUID, dwExitSecond;
	if (!packet->ReadLong(&dwDungeonUID))   return eOkay;
	if (!packet->ReadLong(&dwExitSecond))   return eOkay;
    g_server.m_dungeonMgr.DestroyDungeon(dwDungeonUID, dwExitSecond);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_broadcast_to_region)
{
    char* szCmd;
	if (!packet->ReadString(&szCmd)) return eOkay;
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_broadcast_to_region);
	cmd.WriteString(szCmd);
	g_server.m_region.BroadcastCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_login_server_command)
{
    long dwUserID;
    char* szCmd;
	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadString(&szCmd)) return eOkay;

	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;

    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_gm_command);
    packet.WriteLong(pUser->accountId);
    packet.WriteString(szCmd);
    g_server.m_login.SendCmd();

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_broadcast_to_all_region)
{
    char* szCmd;
	if (!packet->ReadString(&szCmd)) return eOkay;
    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_broadcast_to_all_region);
    packet.WriteString(szCmd);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_broadcast_to_gameworld)
{
    char* szCmd;
	if (!packet->ReadString(&szCmd)) return eOkay;
    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_broadcast_to_gameworld);
    packet.WriteString(szCmd);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_charge_card)
{
    long dwUserID;
    short sLevel;
    char cMetier, cMetierLev;
    char cCardType;
    char* szUsername;
    char* szCardID;
    char* szCardPwd;
    long dwUserParam = 0;

	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadString(&szUsername)) return eOkay;
	if (!packet->ReadShort(&sLevel)) return eOkay;
	if (!packet->ReadByte(&cMetier)) return eOkay;
	if (!packet->ReadByte(&cMetierLev)) return eOkay;
	if (!packet->ReadByte(&cCardType)) return eOkay;
	if (!packet->ReadString(&szCardID)) return eOkay;
	if (!packet->ReadString(&szCardPwd)) return eOkay;
	//if (!packet->ReadLong(&dwUserParam)) return eOkay; // 目前默认Region传上来的指令这个参数都是0

    g_server.ChargeCard(dwUserParam, dwUserID, 0, NULL, cCardType, szCardID, szCardPwd, szUsername, sLevel, cMetier, cMetierLev);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_query_poing)
{
    long dwUserID;
    char cNeedShowHtml;
	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadByte(&cNeedShowHtml)) return eOkay;

	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;

    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_query_point);
    packet.WriteLong(pUser->accountId);
    packet.WriteByte(cNeedShowHtml);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_keep_online)
{
    long dwUserID;
	if (!packet->ReadLong(&dwUserID)) return eOkay;

	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;

    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_keep_online);
    packet.WriteLong(pUser->accountId);
    packet.WriteLong(pUser->attr.level);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_prededuct)
{
    long dwUserID;
    long lDeductOrderID;
	long lAuctionId;
    short sPoint;
	long event;

	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadLong(&lDeductOrderID)) return eOkay;
	if (!packet->ReadLong(&event)) return eOkay;
	if (!packet->ReadShort(&sPoint)) return eOkay;
	if (!packet->ReadLong(&lAuctionId)) return eOkay;

	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;

    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_prededuct);
    packet.WriteLong(pUser->accountId);
    packet.WriteLong(dwUserID);
    packet.WriteLong(lDeductOrderID);
	packet.WriteLong(event);
    packet.WriteShort(sPoint);
	packet.WriteLong(lAuctionId);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_deduct)
{
    long dwUserID;
    long lDeductOrderID;
    short sPoint;
	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadLong(&lDeductOrderID)) return eOkay;
	if (!packet->ReadShort(&sPoint)) return eOkay;

	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;

    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_deduct);
    packet.WriteLong(pUser->accountId);
    packet.WriteLong(dwUserID);
    packet.WriteLong(lDeductOrderID);
    packet.WriteShort(sPoint);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_add_point)
{
	long dwUserID;
	short sPoint;
	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadShort(&sPoint)) return eOkay;
	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;
	CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_add_point);
	packet.WriteLong(pUser->accountId);
	packet.WriteLong(dwUserID);
	packet.WriteShort(sPoint);
	g_server.m_login.SendCmd();
	return eOkay;

}
GW_END_CMD

GW_BEGIN_CMD(cmd_r2g_lock_account)
{
    long dwUserID;
    short sLockFlag;
    long lLockSecond;
	if (!packet->ReadLong(&dwUserID)) return eOkay;
	if (!packet->ReadShort(&sLockFlag)) return eOkay;
	if (!packet->ReadLong(&lLockSecond)) return eOkay;

	GWUser *pUser = g_server.FindActorById(dwUserID);
	if (!pUser) return eOkay;

    CG_CmdPacket& packet = g_server.m_login.BeginCmd(gws2lgs_lock_account);
    packet.WriteLong(pUser->accountId);
    packet.WriteShort(sLockFlag);
    packet.WriteLong(lLockSecond);
    g_server.m_login.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_license_check)
{
	char*	pstrip;
	char*	pstrmac;
	char*	pstrMd5;
	long	nType;
	bool	bValid = false;

	packet->ReadString(&pstrip);
	packet->ReadString(&pstrmac);
	packet->ReadString(&pstrMd5);
	packet->ReadLong(&nType);

	CG_CmdPacket& cmd = g_server.m_login.BeginCmd(gws2lgs_license_check_rs);
	cmd.WriteString(pstrip);
	cmd.WriteString(pstrmac);
	cmd.WriteString(pstrMd5);
	cmd.WriteLong((long)nType);
	g_server.m_login.SendCmd();

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_card_consign)
{
    long lUserID, lPrice;
    char *szCardID, *szCardPwd;
    char cCardType;
	packet->ReadLong(&lUserID);
	packet->ReadByte(&cCardType);
	packet->ReadString(&szCardID);
	packet->ReadString(&szCardPwd);
	packet->ReadLong(&lPrice);
    g_server.m_cardTrade.Consign(lUserID, cCardType, szCardID, szCardPwd, lPrice);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_card_buy)
{
    long lUserID, lConsignationID, lPrice;
	packet->ReadLong(&lUserID);
	packet->ReadLong(&lConsignationID);
	packet->ReadLong(&lPrice);
    g_server.m_cardTrade.BuyCard(lUserID, lPrice, lConsignationID);
	return eOkay;
}
GW_END_CMD;

//tim.yang  VIP卡
GW_BEGIN_CMD(cmd_r2g_add_account_time)
{
	long lUserID, time;
	packet->ReadLong(&lUserID);
	packet->ReadLong(&time);
	g_server.AddAccountTime(lUserID,time);
	return eOkay;
}
GW_END_CMD;
//end

GW_BEGIN_CMD(cmd_r2g_card_cancel)
{
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_card_request_update)
{
    g_server.m_cardTrade.UpdateCardTradeList();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_add_master_prentice)
{
	long masterID, prenticeID;
	char* masterName, *prenticeName;
	if (!packet->ReadLong(&masterID))		return eInvalidData;
	if (!packet->ReadLong(&prenticeID))		return eInvalidData;
	if (!packet->ReadString(&masterName))	return eInvalidData;
	if (!packet->ReadString(&prenticeName))	return eInvalidData;

	long index = 0;
	if (!g_MasterPrenticeMgr.AddMasterPrentice(masterID, prenticeID, index, masterName, prenticeName))
	{
	}
	else 
	{
		int onlineTime = g_server.GetUserOnlineTime(prenticeID);
		CMasterPrenticeCmd::Send_g2r_prentice_onlinetime(masterID, prenticeID, onlineTime);
	}
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_remove_master_prentice)
{
	long applyUserID;
	long index;
	long masterID, prenticeID;
	char* masterName, *prenticeName;
	if (!packet->ReadLong(&applyUserID))	return eInvalidData;
	if (!packet->ReadLong(&index))			return eInvalidData;
	if (!packet->ReadLong(&masterID))		return eInvalidData;
	if (!packet->ReadLong(&prenticeID))		return eInvalidData;
	if (!packet->ReadString(&masterName))	return eInvalidData;
	if (!packet->ReadString(&prenticeName))	return eInvalidData;

	if (!g_MasterPrenticeMgr.RemoveMasterPrentice(index, applyUserID, masterID, prenticeID, masterName, prenticeName))
	{
	}
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_finish_master_prentice)
{
	long index;
	long masterID, prenticeID;
	char* masterName, *prenticeName;
	if (!packet->ReadLong(&index))			return eInvalidData;
	if (!packet->ReadLong(&masterID))		return eInvalidData;
	if (!packet->ReadLong(&prenticeID))		return eInvalidData;
	if (!packet->ReadString(&masterName))	return eInvalidData;
	if (!packet->ReadString(&prenticeName))	return eInvalidData;

	if (!g_MasterPrenticeMgr.FinishMasterPrentice(index, masterID, prenticeID, masterName, prenticeName))
	{
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_master_prentice_check)
{
	g_MasterPrenticeMgr.DoCheckDataFromRegion(packet);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_master_prentice_credit)
{
	long index;
	long userID;
	short count;
	if (!packet->ReadLong(&index))			return eInvalidData;
	if (!packet->ReadLong(&userID))			return eInvalidData;
	if (!packet->ReadShort(&count))			return eInvalidData;

	GWUser* pUser = g_server.FindActorById(userID);
	if (pUser)
	{
		CMasterPrenticeCmd::Send_g2r_master_prentice_credit(userID, count);
	}
	else 
	{
		// 如果玩家没有在线，就加到数据表T_MasterPrentice中
		if (!g_MasterPrenticeMgr.Add_CreditToAdd(index, userID, count))
		{
		}
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_master_prentice_gift)
{
	long userID;
	long lev;
	if (!packet->ReadLong(&userID))			return eInvalidData;
	if (!packet->ReadLong(&lev))			return eInvalidData;

	if (!g_MasterPrenticeMgr.WithdrawMPGift(userID, lev))
	{
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_bankpass)
{
	long userID;
	short operateID;
	char* pass;
	if (!packet->ReadLong(&userID))			return eInvalidData;
	if (!packet->ReadShort(&operateID))		return eInvalidData;
	if (!packet->ReadString(&pass))			return eInvalidData;

	if (!CBankPass::UpdatePass(userID, operateID, pass))
	{
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_broadcast_bulletin)
{
	long GmID;
	char* content;
	long color;
	char style;
	if (!packet->ReadLong(&GmID))       return eInvalidData;
	if (!packet->ReadString(&content))  return eInvalidData;
	if (!packet->ReadLong(&color))      return eInvalidData;
	if (!packet->ReadByte(&style))      return eInvalidData;
    g_server.BroadcastBulletin(content, style, color, GmID);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_prentice_onlinetime)
{
	long userID;
	long prenticeID;
	if (!packet->ReadLong(&userID))			return eInvalidData;
	if (!packet->ReadLong(&prenticeID))		return eInvalidData;
	int onlineTime = g_server.GetUserOnlineTime(prenticeID);
	CMasterPrenticeCmd::Send_g2r_prentice_onlinetime(userID, prenticeID, onlineTime);
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_r2g_activity_system)
{
	long lRegionID = g_server.m_region.GetSession().index;
	GW_ActivityManager::GetSingle()->Dispose(lRegionID, packet);
	return eOkay;
}
GW_END_CMD;


GW_BEGIN_CMD(cmd_r2g_Set_Refuse)
{
	long ID = 0;
	char bRefuseTeam = 0;
	char bRefuseAction = 0;
	char bRefuseMail = 0;
	char bRefuseDuel = 0;
	char bRefuseFriend = 0;
	char bRefuseMonster = 0;
	if (!packet->ReadLong(&ID))			    return eInvalidData;
	if (!packet->ReadByte(&bRefuseTeam))	return eInvalidData;
	if (!packet->ReadByte(&bRefuseAction))	return eInvalidData;
	if (!packet->ReadByte(&bRefuseMail))	return eInvalidData;
	if (!packet->ReadByte(&bRefuseDuel))	return eInvalidData;
	if (!packet->ReadByte(&bRefuseFriend))	return eInvalidData;
	if (!packet->ReadByte(&bRefuseMonster))	return eInvalidData;

	GWUser* pUser = g_server.FindActorById(ID);
	if (!pUser)
	{
		return eOkay;
	}

	pUser->m_bRefuseTeam = (bool)bRefuseTeam;       //设置是否拒绝组队
	pUser->m_bRefusetrade = (bool)bRefuseAction;       //设置是否拒绝交易
	pUser->m_bRefuseMail = (bool)bRefuseMail;        //设置是否拒绝邮件
	pUser->m_bRefuseDuel = (bool)bRefuseDuel;        //设置是否拒绝决斗
	pUser->m_bRefuseFriend = (bool)bRefuseFriend;     //设置是否拒绝好友
	pUser->m_bRefuseMonster = (bool)bRefuseMonster;     //设置是否拒绝师徒

	return eOkay;
}
GW_END_CMD;
/*
GW_BEGIN_CMD(cmd_r2g_team)
{
	long	UserDBID;
	char	Operation;
	long	DBIDInMessage;
	long	FromDBID;
	char*	pFromName;

	long	Parameter;
	long	Hp;
	long	MaxHp;
	long	Mp;
	long	MaxMp;
	char*	pName;

	if (!packet->ReadLong(&UserDBID))		return eInvalidData;
	if (!packet->ReadByte(&Operation))		return eInvalidData;
	if (!packet->ReadLong(&DBIDInMessage))	return eInvalidData;
	if (!packet->ReadLong(&FromDBID))		return eInvalidData;
	if (!packet->ReadString(&pFromName))	return eInvalidData;
	if (!packet->ReadLong(&Parameter))		return eInvalidData;
	if (!packet->ReadLong(&Hp))				return eInvalidData;
	if (!packet->ReadLong(&MaxHp))			return eInvalidData;	
	if (!packet->ReadLong(&Mp))				return eInvalidData;
	if (!packet->ReadLong(&MaxMp))			return eInvalidData;
	if (!packet->ReadString(&pName))		return eInvalidData;	

	GWUser* user = g_server.m_database.LookupUser(UserDBID);

	if (user == NULL)
		return eOkay;
	
	int se = g_server.GetRegionSession(user->regionID);

	if (se == -1)
		return eOkay;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_team);

	cmd.WriteLong(UserDBID);
	cmd.WriteByte(Operation);
	cmd.WriteLong(DBIDInMessage);
	cmd.WriteLong(FromDBID);
	cmd.WriteString(pFromName);
	cmd.WriteLong(Parameter);
	cmd.WriteLong(Hp);
	cmd.WriteLong(MaxHp);
	cmd.WriteLong(Mp);
	cmd.WriteLong(MaxMp);
	cmd.WriteString(pName);

	g_server.m_region.SendToPeer(se);
}
GW_END_CMD;
*/

GWCommandChannel* GWServer::CreateRegionChannel()
{
	GWCommandChannel* channel = RT_NEW GWCommandChannel;
	
#define REGISTER_COMMAND(c) channel->RegisterCommand(c, cmd_##c, #c);

	REGISTER_COMMAND(r2g_update_user);
	REGISTER_COMMAND(r2g_register);
	REGISTER_COMMAND(r2g_accept_user);
	REGISTER_COMMAND(r2g_repatriate_user);
	REGISTER_COMMAND(r2g_union);
	REGISTER_COMMAND(r2g_feod);
	REGISTER_COMMAND(r2g_logout);
	REGISTER_COMMAND(r2g_add_mail);
	REGISTER_COMMAND(r2g_remove_mail);
	REGISTER_COMMAND(r2g_MailItem);
	REGISTER_COMMAND(r2g_Is_Open);
	REGISTER_COMMAND(r2g_chat);
	REGISTER_COMMAND(r2g_SystemNews);
	REGISTER_COMMAND(r2g_switch_region);
	REGISTER_COMMAND(r2g_herolist_query);
	REGISTER_COMMAND(r2g_log_event);
	REGISTER_COMMAND(r2g_gm_additem_event);
	REGISTER_COMMAND(r2g_gm_move);
    REGISTER_COMMAND(r2g_move_to);
	REGISTER_COMMAND(r2g_save_itemid);
	REGISTER_COMMAND(r2g_query_itemid);
	REGISTER_COMMAND(r2g_move_user);
	REGISTER_COMMAND(r2g_move_useraround);
	REGISTER_COMMAND(r2g_team);
	REGISTER_COMMAND(r2g_friend);
	REGISTER_COMMAND(r2g_fiend_name_ID);
	REGISTER_COMMAND(r2g_when_fiend);
	REGISTER_COMMAND(r2g_OneFriend_Return);
	REGISTER_COMMAND(r2g_one_refresh);
	REGISTER_COMMAND(r2g_Black_List);
	REGISTER_COMMAND(r2g_PakeBlack_List);
    REGISTER_COMMAND(r2g_update_region_state);
    REGISTER_COMMAND(r2g_safe_quited);
	//---------------add start by tony 05.06.16---------------------------//
	REGISTER_COMMAND(r2g_castlewar);
	//---------------add end   by tony 05.06.16---------------------------//
	//---------------add start by rendh 2010.01.23---------------------------//
	REGISTER_COMMAND(r2g_linesys);
	//---------------add end   by rendh 2010.01.23---------------------------//
	REGISTER_COMMAND(r2g_check_version);
    REGISTER_COMMAND(r2g_check_version);
	REGISTER_COMMAND(r2g_kick);
    REGISTER_COMMAND(r2g_tester_ret);
	REGISTER_COMMAND(r2g_query_user_info);
	REGISTER_COMMAND(r2g_user_info);
	REGISTER_COMMAND(r2g_forbid_user);
	REGISTER_COMMAND(r2g_query_gifts);
	REGISTER_COMMAND(r2g_withdraw_gift_success);

	REGISTER_COMMAND(r2g_start_fun);
	REGISTER_COMMAND(r2g_end_fun);

	REGISTER_COMMAND(r2g_create_dungeon_ret);
	REGISTER_COMMAND(r2g_enter_dungeon);
	REGISTER_COMMAND(r2g_leave_dungeon);
	REGISTER_COMMAND(r2g_destroy_dungeon);
	REGISTER_COMMAND(r2g_modify_maxdungeon);

	REGISTER_COMMAND(r2g_broadcast_to_region);
	REGISTER_COMMAND(r2g_login_server_command);
	REGISTER_COMMAND(r2g_broadcast_to_all_region);
	REGISTER_COMMAND(r2g_broadcast_to_gameworld);
	REGISTER_COMMAND(r2g_charge_card);
	REGISTER_COMMAND(r2g_query_poing);
	REGISTER_COMMAND(r2g_keep_online);
	REGISTER_COMMAND(r2g_prededuct);
	REGISTER_COMMAND(r2g_deduct);
	REGISTER_COMMAND(r2g_lock_account);
	REGISTER_COMMAND(r2g_license_check);
	REGISTER_COMMAND(r2g_add_point);
	REGISTER_COMMAND(r2g_add_account_time);

	REGISTER_COMMAND(r2g_card_consign);
	REGISTER_COMMAND(r2g_card_buy);
	REGISTER_COMMAND(r2g_card_cancel);
	REGISTER_COMMAND(r2g_card_request_update);

	REGISTER_COMMAND(r2g_add_master_prentice);
	REGISTER_COMMAND(r2g_remove_master_prentice);
	REGISTER_COMMAND(r2g_finish_master_prentice);
	REGISTER_COMMAND(r2g_master_prentice_check);
	REGISTER_COMMAND(r2g_master_prentice_credit);
	REGISTER_COMMAND(r2g_master_prentice_gift);

	REGISTER_COMMAND(r2g_bankpass);
	REGISTER_COMMAND(r2g_broadcast_bulletin);
	REGISTER_COMMAND(r2g_prentice_onlinetime);
	REGISTER_COMMAND(r2g_card_trade_switch);
	REGISTER_COMMAND(r2g_auction);
	REGISTER_COMMAND(r2g_master_list);
	REGISTER_COMMAND(r2g_auction_auction);
	REGISTER_COMMAND(r2g_auction_cancel_auction);
	REGISTER_COMMAND(r2g_auction_cancel_bid);
	REGISTER_COMMAND(r2g_auction_bid);
	REGISTER_COMMAND(r2g_auction_get_items);
	REGISTER_COMMAND(r2g_auction_get_items_ret);
	REGISTER_COMMAND(r2g_warfield_enter);
	REGISTER_COMMAND(r2g_warfield_start_rep);
	REGISTER_COMMAND(r2g_warfield_leave);
	REGISTER_COMMAND(r2g_warfield_stop_rep);
	REGISTER_COMMAND(r2g_warfield_addscore);
	REGISTER_COMMAND(r2g_warfield_open);
	REGISTER_COMMAND(r2g_warfield_conjure_progress);
	REGISTER_COMMAND(r2g_warfield_update_exploit);
	REGISTER_COMMAND(r2g_warfield_stop);
	REGISTER_COMMAND(r2g_warfield_add_exp);
	REGISTER_COMMAND(r2g_warfield_add_per_exploit);
	REGISTER_COMMAND(r2g_warfield_create_arms);
	REGISTER_COMMAND(r2g_warfield_tmp_per_exploit);
	REGISTER_COMMAND(r2g_user_change_block);

	REGISTER_COMMAND(r2g_activity_system);
	REGISTER_COMMAND(r2g_Set_Refuse);

#undef REGISTER_COMMAND

	return channel;
}
