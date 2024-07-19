#include "gw_server.h"

void GWChatTransmittal::ChatTeam(int toID, const char* toName, const char* fromName, char channel, const char* content)
{
	GWUser *toUser;
	if (toID == 0)
		toUser = g_server.FindActorByName(toName);
	else
		toUser = g_server.FindActorById(toID);

	GWUser *fromUser = g_server.FindActorByName(fromName);
	if(fromUser == NULL)
		return;
	if(toUser == NULL)
	{
		if(toName == NULL)
			return;

		char buf[GW_MAX_NAME_LENGTH + 8];
		rt2_snprintf(buf, GW_MAX_NAME_LENGTH + 7, "%s不在线", toName);
		buf[GW_MAX_NAME_LENGTH + 7] = 0;

		Chat(g_server.m_region.GetSession().index, fromUser->userID, "", CHAT_CHANNEL_SYSTEM, buf);
	}
	else
	{
		int se = g_server.GetRegionSession(toUser->regionID);
		if (se == -1)
			return;

		Chat(se, toUser->userID, fromName, channel, content);//发给要收的人
		//Chat(g_server.m_region.GetSession().index, fromUser->userID, fromName, channel, content);//发给自己
	}
}

//heten
void GWChatTransmittal::ChatWorld(int faction, const char* toName, const char* fromName, char channel, const char* content)
{
	InitChatGroup();

	GWChatGroup* groupHeader = NULL;
	GWChatGroup* group = NULL;
	int iLen = m_group.size();
	for (int i = 0; i < iLen; ++i)
	{
		GWChatGroup* g = &m_group[i];
		if (g->session < 0)
			continue;

		g->users.clear();
		g->users.push_back(faction);

		if (groupHeader == NULL)
		{
			groupHeader = g;
			group = g;
		}
		else
		{
			group->next = g;
			group = g;
		}
	}

	BroadcastChat(groupHeader, fromName, CHAT_CHANNEL_WORLD, content);
}
//end

void GWChatTransmittal::ChatNewS(const char* fromName,const char* Name,const char* BossName,const char* SceneName,long type,char channel,const char *ItemName)
{
	InitChatGroup();

	GWChatGroup* groupHeader = NULL;
	GWChatGroup* group = NULL;
	int iLen = m_group.size();
	for (int i = 0; i < iLen; ++i)
	{
		GWChatGroup* g = &m_group[i];
		if (g->session < 0)
			continue;

		g->users.clear();
		g->users.push_back(FACTION_ALL);

		if (groupHeader == NULL)
		{
			groupHeader = g;
			group = g;
		}
		else
		{
			group->next = g;
			group = g;
		}
	}
	BroadcastChatNewS(groupHeader,fromName,Name,BossName,SceneName,type,CHAT_CHANNEL_NEWS,ItemName);
}

void GWChatTransmittal::ChatPublic(int faction, const char* toName, const char* fromName, char channel, const char* content,long oldId)
{
	//InitChatGroup();

	//GWChatGroup* groupHeader = NULL;
	//GWChatGroup* group = NULL;
	//int iLen = m_group.size();
	//for (int i = 0; i < iLen; ++i)
	//{
	//	GWChatGroup* g = &m_group[i];
	//	if (g->session < 0)
	//		continue;

	//	g->users.clear();
	//	g->users.push_back(faction);
	//	
	//	if (groupHeader == NULL)
	//	{
	//		groupHeader = g;
	//		group = g;
	//	}
	//	else
	//	{
	//		group->next = g;
	//		group = g;
	//	}
	//}

	//BroadcastChat(groupHeader, fromName, CHAT_CHANNEL_PUBLIC, content);

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_public_chat);
		cmd.WriteByte(CHAT_CHANNEL_PUBLIC);
		cmd.WriteString((char*)fromName);
		cmd.WriteString((char*)content);
		cmd.WriteLong(oldId);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

void GWChatTransmittal::ChatPrivate(int toID, const char* toName, const char* fromName, char channel, const char* content)
{
	GWUser *toUser;
	if (toID == 0)
		toUser = g_server.FindActorByName(toName);
	else
		toUser = g_server.FindActorById(toID);

	GWUser *fromUser = g_server.FindActorByName(fromName);
	if(fromUser == NULL)
		return;
	if(toUser == NULL)
	{
		if(toName == NULL)
			return;

		char buf[GW_MAX_NAME_LENGTH + 8];
		rt2_snprintf(buf, GW_MAX_NAME_LENGTH + 7, "%s不在线", toName);
        buf[GW_MAX_NAME_LENGTH + 7] = 0;

		Chat(g_server.m_region.GetSession().index, fromUser->userID, "", CHAT_CHANNEL_SYSTEM, buf);
	}
	else
	{
		int se = g_server.GetRegionSession(toUser->regionID);
		if (se == -1)
			return;

		Chat(se, toUser->userID, fromName, channel, content);//发给要收的人
		Chat(g_server.m_region.GetSession().index, fromUser->userID, fromName, channel, content);//发给自己
	}
}

void GWChatTransmittal::ChatParty(int toID, const char* fromName, const char* content)
{
	InitChatGroup();

	CUnionMgr* unionMgr = GetUnionMgr();

	CUnion* u = unionMgr->FindUnion(toID);

	if (u == NULL)
		return;

	GWChatGroup* groupHeader = NULL;
	
	for (int n = 0, c = u->CitizenCount(); n < c; n++)
	{
		SUnionCitizen* citizen = &(*u)[n];		
//ldr123
		if (citizen->m_bOnLine)
		{
			GWUser* user = g_server.FindActorById(citizen->m_nPlayerID);

			if (user == NULL)
				continue;

			GWChatGroup* group = &m_group[user->regionID];

			if (!group->pending)
			{
				group->next = groupHeader;				
				group->pending = true;
				group->users.resize(0);
				groupHeader = group;
			}

			group->users.push_back(citizen->m_nPlayerID);
		}
	}

	BroadcastChat(groupHeader, fromName, CHAT_CHANNEL_PARTY, content);
}

void GWChatTransmittal::BroadcastChat(GWChatGroup* groups, const char* fromName, char channel, const char* content)
{	
	for(GWChatGroup* g = groups; g; g = g->next)
	{
		if (g->session == -1)
			continue;

		if (g->users.empty())
			continue;

		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_chat_list);

		cmd.WriteBinary((char*)&g->users[0], (int)g->users.size() * sizeof(long));
		cmd.WriteString((char*)fromName);
		cmd.WriteByte(channel);
		cmd.WriteString((char*)content);		

		g_server.m_region.SendToPeer(g->session);
	}
}

void GWChatTransmittal::BroadcastChatNewS(GWChatGroup* groups,const char* fromName,const char* Name,const char* BossName,const char* SceneName,long type,char channel,const char *ItemName)
{
	for(GWChatGroup* g = groups; g; g = g->next)
	{
		if (g->session == -1)
			continue;

		if (g->users.empty())
			continue;

		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_chatNew_list);

		cmd.WriteBinary((char*)&g->users[0], (int)g->users.size() * sizeof(long));
		cmd.WriteString((char*)fromName);
		cmd.WriteString((char*)Name);
		cmd.WriteString((char*)BossName);
		cmd.WriteString((char*)SceneName);
		cmd.WriteLong(type);
		cmd.WriteByte(channel);	
		cmd.WriteString((char*)ItemName);

		g_server.m_region.SendToPeer(g->session);
	}
}

void GWChatTransmittal::ChatCountry(int toID, const char* fromName, const char* content)
{
}

void GWChatTransmittal::ChatGM(int toID, const char* content)
{
	GWUser *pUser = g_server.FindActorById(toID);
	if (pUser)
	{
		int session = g_server.m_regionTable[pUser->regionID].sessionIndex;
		if (session<0)			return;

		Chat(session, toID, "GM", CHAT_CHANNEL_GM, content);
	}
}

void GWChatTransmittal::InitChatGroup()
{
	m_group.resize(g_server.m_regionTable.size());

	for (int n = 0; n < (int)m_group.size(); n++)
	{
		GWChatGroup& group = m_group[n];

		group.pending = false;
		group.session = g_server.m_regionTable[n].sessionIndex;
		group.next = NULL;
	}
}

void GWChatTransmittal::Chat(int session, int toID, const char* fromName, char channel, const char* content)
{
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_chat);
	
	cmd.WriteLong(toID);
	cmd.WriteString((char*)fromName);
	cmd.WriteByte(channel);
	cmd.WriteString((char*)content);

	g_server.m_region.SendToPeer(session);
}
