#include "gw_server.h"
#include "lgs_gws.h"

#define SAFE_READ(p) if(!(p)) return eOkay;

GW_BEGIN_CMD(cmd_gm2gw_chat)
{
	guard;

	long playerID;
	char* content;
	SAFE_READ(packet->ReadLong(&playerID))
	SAFE_READ(packet->ReadString(&content))

	g_server.m_chatTransmittal.ChatGM(playerID, content);

	return eOkay;

	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_tickuser)
{
	guard;

	long playerID;
	long gmID;
	SAFE_READ(packet->ReadLong(&playerID))
	SAFE_READ(packet->ReadLong(&gmID))

	GWLoginClient *cl = g_server.FindLoginClientByActorId(playerID);
	if(!cl) return eOkay;
    
	g_server.KickAccount(cl->info.accountId,KICK_USER_GM);
	
	CG_CmdPacket& packet = g_server.m_gm.BeginCmd(gw2gm_tickuser_result);
	packet.WriteLong(playerID);
	packet.WriteLong(gmID);
	packet.WriteByte(1);
	g_server.m_gm.SendCmd();

	return eOkay;

	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_save_itemid)
{
	guard;

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

	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_query_itemid)
{
	guard;

	long regionID;
	if ( !packet->ReadLong(&regionID) )			return eOkay;

	g_GWItemIDMgr.SendIDData(regionID);

	return eOkay;

	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_gm_response)
{
	guard;

	long userID;
	long GmID;
	char* GmName;
	short QuestionCount;
	short Order;

	if (!packet->ReadLong(&userID))
		return eOkay;
	if (!packet->ReadLong(&GmID))
		return eOkay;
	if (!packet->ReadString(&GmName))
		return eOkay;
	if (!packet->ReadShort(&QuestionCount))
		return eOkay;
	if (!packet->ReadShort(&Order))
		return eOkay;

	GWUser *pUser = g_server.FindActorById(userID);
	if (pUser)
	{
		GWRegion& rg = g_server.m_regionTable[pUser->regionID];
		if (rg.sessionIndex==-1)		return eError;
		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_gm_response);
		cmd.WriteLong(userID);
		cmd.WriteLong(GmID);
		cmd.WriteString(GmName);
		cmd.WriteShort(QuestionCount);
		cmd.WriteShort(Order);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
	return eOkay;

	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_gm_bulletin)
{
	guard;

	long GmID;
	char* content;
	long color;
	char style;

	if (!packet->ReadLong(&GmID))
		return eOkay;
	if (!packet->ReadString(&content))
		return eOkay;
	if (!packet->ReadLong(&color))
		return eOkay;
	if (!packet->ReadByte(&style))
		return eOkay;

    g_server.BroadcastBulletin(content, style, color, GmID);

	return eOkay;

	unguard;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_get_init_config)
{
	CG_CmdPacket& packet = g_server.m_gm.BeginCmd(gw2gm_update_config);
	packet.WriteString((char*)g_cfgGW.connectDatabase.ip.c_str());
	packet.WriteLong(g_cfgGW.connectDatabase.port);
	packet.WriteString((char*)g_cfgGW.connectDatabase.username.c_str());
	packet.WriteString((char*)g_cfgGW.connectDatabase.password.c_str());
	packet.WriteString((char*)g_cfgGW.connectDatabase.database.c_str());
	packet.WriteString((char*)g_cfgGW.connectLogDb.ip.c_str());
	packet.WriteLong(g_cfgGW.connectLogDb.port);
	packet.WriteString((char*)g_cfgGW.connectLogDb.username.c_str());
	packet.WriteString((char*)g_cfgGW.connectLogDb.password.c_str());
	packet.WriteString((char*)g_cfgGW.connectLogDb.database.c_str());
	g_server.m_gm.SendCmd();
	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_query_herolist)
{
    long listVersion;
    if ( !packet->ReadLong(&listVersion) )			return eOkay;

    if (GetHeroList()->m_data.m_version!=listVersion)
    {
        CG_CmdPacket& cmd = g_server.m_gm.GetSendCmd();
        cmd.BeginWrite();
        cmd.WriteShort(gw2gm_herolist);
        cmd.WriteLong(GetHeroList()->m_data.m_version);
        GetHeroList()->m_data.Serialize(&cmd);
        g_server.m_gm.SendCmd();
    }
    else
    {
        GetHeroList()->OnRefresh();
    }

    return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_send_mail)
{
	SMailEx mailEx;
	if (!mailEx.Serialize(packet))			return eOkay;

	int i;
	for (i=0; i<(int)mailEx.arrReceiverID.size(); i++)
	{
		string StrItemID = "";
		g_server.m_mail.AddMail(mailEx.arrReceiverID[i], mailEx.senderID, mailEx.title, mailEx.content, false,StrItemID,0);
	}

	return eOkay;
}
GW_END_CMD;

GW_BEGIN_CMD(cmd_gm2gw_query_userstate)
{
	long userID;
	if ( !packet->ReadLong(&userID))		return eOkay;

	char bOnline = -1;
	if (g_server.FindActorById(userID))
		bOnline = 1;
	else 
		bOnline = 2;

	CG_CmdPacket& cmd = g_server.m_gm.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(gw2gm_userstate);
	cmd.WriteLong(userID);
	cmd.WriteByte(bOnline);
	g_server.m_gm.SendCmd();

	return eOkay;
}
GW_END_CMD;


GWCommandChannel* GWServer::CreateGMChannel()
{
	GWCommandChannel* channel = RT_NEW GWCommandChannel;

#define REGISTER_COMMAND(c) channel->RegisterCommand(c, cmd_##c, #c);	

	REGISTER_COMMAND(gm2gw_chat);
	REGISTER_COMMAND(gm2gw_tickuser);
	REGISTER_COMMAND(gm2gw_save_itemid);
	REGISTER_COMMAND(gm2gw_query_itemid);
	REGISTER_COMMAND(gm2gw_gm_response);
	REGISTER_COMMAND(gm2gw_gm_bulletin);
	REGISTER_COMMAND(gm2gw_get_init_config);
    REGISTER_COMMAND(gm2gw_query_herolist);
	REGISTER_COMMAND(gm2gw_send_mail);
	REGISTER_COMMAND(gm2gw_query_userstate);

#undef REGISTER_COMMAND

	return channel;
}
