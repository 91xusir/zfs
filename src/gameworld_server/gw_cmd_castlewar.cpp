//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------

#include "gw_server.h"
#include "castlewar_base.h"
#include "gw_cmd_castlewar.h"
#include "castlewar_net_define.h"
#include "gw_castlewar.h"

//----------------------------
//region消息与对应处理函数表
//----------------------------
const STGWCALSTLEWAR GWCastleWarCmdProcTbl[] = 
{
	{r2g_castlewar_update_data,		cmd_r2g_castlewar_update_data},
	
	{r2g_castlewar_war_prepare,		cmd_r2g_castlewar_war_prepare},
	
	{r2g_castlewar_war_start,		cmd_r2g_castlewar_war_start},
	
	{r2g_castlewar_war_pause,		cmd_r2g_castlewar_war_pause},
	
	{r2g_castlewar_war_end,			cmd_r2g_castlewar_war_end},

	{r2g_castlewar_time_set,		cmd_r2g_castlewar_time_set},

	{r2g_castlewar_reload_time,		cmd_r2g_castlewar_reload_time}
};

//---------------------------------------
//REGION消息匹配函数
//---------------------------------------
int GW_CastleWarCmdProcess(CG_CmdPacket *cmd)
{
	short cmd_type;
	
	SAFE_ASSERT(cmd->ReadShort(&cmd_type))

	//查找匹配函数
	int i = 0;
	for(i = 0; i < NUM(GWCastleWarCmdProcTbl); i++)
	{
		if(cmd_type == GWCastleWarCmdProcTbl[i].cmd_type)
		{
			break;
		}
	}

	if(i >= NUM(GWCastleWarCmdProcTbl))
	{
		return eError;
	}

	return GWCastleWarCmdProcTbl[i].cmd_func(cmd);
}

//----------------------------
//以下处理来自REGION消息
//----------------------------
int	cmd_r2g_castlewar_update_data(CG_CmdPacket *cmd)
{
	guard;
	
	SAFE_ASSERT(cmd)
		
	STWarInfo	temp;
	
	if(temp.Serialize(cmd) == false)
	{
		return eError;
	}
	
	g_CastlewarMgr.UpdateData(&temp);
	
	return eOkay;
	
	unguard;
}

int	cmd_r2g_castlewar_time_set(CG_CmdPacket *cmd)
{
	guard;

	SAFE_ASSERT(cmd)

	char*	 p_feod_name= NULL;	
	long	 start_time	= 0;
	ST_TIME* p_time		= NULL;
	
	SAFE_READ(cmd->ReadString(&p_feod_name))
	SAFE_READ(cmd->ReadLong(&start_time))

	std::string feod_name(p_feod_name);

	//开始时间格式变换
	p_time = localtime((TIME*)&start_time);

	g_CastlewarMgr.SetDefWarTime(feod_name, p_time->tm_wday, p_time->tm_hour, p_time->tm_min);

	LOG3("[%s] Next WarTime Set at: [%d]-[%d]-[%d]  [%d]:[%d]", 
				p_feod_name, 
				p_time->tm_year + 1900,
				p_time->tm_mon + 1,
				p_time->tm_mday,
				p_time->tm_hour,
				p_time->tm_min);

	return eOkay;

	unguard;
}

int	cmd_r2g_castlewar_reload_time(CG_CmdPacket *cmd)
{
	guard;

	SAFE_ASSERT(cmd)

	char* p_feod_name= NULL;

	SAFE_READ(cmd->ReadString(&p_feod_name))

	std::string feod_name(p_feod_name);
	
	g_CastlewarMgr.ReloadTime(feod_name);

	return eOkay;

	unguard;
}

int	cmd_r2g_castlewar_war_prepare(CG_CmdPacket *cmd)
{
	guard;	
	
	char* p_feod_name	= NULL;	
	char* p_union_name	= NULL;
	long total_time;
	
	SAFE_READ(cmd->ReadString(&p_feod_name))
	SAFE_READ(cmd->ReadString(&p_union_name))
	SAFE_READ(cmd->ReadLong(&total_time))
	
	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_prepare_rep(p_feod_name, p_union_name, total_time);
	
	return eOkay;
		
	unguard;
}

int	cmd_r2g_castlewar_war_start(CG_CmdPacket *cmd)
{
	guard;	
	
	char* p_feod_name	= NULL;	
	
	SAFE_READ(cmd->ReadString(&p_feod_name))
	
	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_start_rep(p_feod_name);

	//string feod_name = p_feod_name;

	//g_CastlewarMgr.SaveToDB(feod_name);

	//--------------log info start-----------------------//
	ST_TIME*	p_local_time;	//当前时间
	TIME		tmp;

	time(&tmp);
	p_local_time = localtime((TIME *)(&tmp));
	LOG3("[%s] CastleWar Started at  [%d]-[%d]-[%d]  [%d]:[%d]\n",
		p_feod_name,
		p_local_time->tm_year + 1900,
		p_local_time->tm_mon + 1,
		p_local_time->tm_mday,
		p_local_time->tm_hour,
		p_local_time->tm_min);
	//--------------log info end ------------------------//
	
	return eOkay;
		
	unguard;
}

int	cmd_r2g_castlewar_war_pause(CG_CmdPacket *cmd)
{
	guard;	
	
	char* p_feod_name	= NULL;	
	char* p_union_name	= NULL;
	
	SAFE_READ(cmd->ReadString(&p_feod_name))
	SAFE_READ(cmd->ReadString(&p_union_name))
	
	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_pause_rep(p_feod_name, p_union_name);

	//string feod_name = p_feod_name;

	//g_CastlewarMgr.SaveToDB(feod_name);
	
	return eOkay;
		
	unguard;
}

int	cmd_r2g_castlewar_war_end(CG_CmdPacket *cmd)
{
	guard;	
	
	char* p_feod_name	= NULL;	
	char* p_union_name	= NULL;
	TIME tmp = 0;	
	time(&tmp);
	
	SAFE_READ(cmd->ReadString(&p_feod_name))
	SAFE_READ(cmd->ReadString(&p_union_name))
	
	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_end_rep(p_feod_name, p_union_name);

	if(!g_CastlewarMgr.SetLatestWarTime(string(p_feod_name), (DWORD)tmp))
	{
		LOG1("Set LatestWarTime Failed: [&s]\n", p_feod_name);
	}
	
	//string feod_name = p_feod_name;

	//g_CastlewarMgr.SaveToDB(feod_name);

	//--------------log info start-----------------------//
	ST_TIME*	p_local_time;	//当前时间

	p_local_time = localtime((TIME *)(&tmp));
	LOG3("[%s] CastleWar End at  [%d]-[%d]-[%d]  [%d]:[%d]\n",
		p_feod_name,
		p_local_time->tm_year + 1900,
		p_local_time->tm_mon + 1,
		p_local_time->tm_mday,
		p_local_time->tm_hour,
		p_local_time->tm_min);
	//--------------log info end ------------------------//
	
	return eOkay;
		
	unguard;
}

void CCastleWarCmdHelper_GW::Send_g2r_castlewar_data_init(long region_id, STWarInfo* p_war_info)
{
	guard;
		
	GWRegion& rg = g_server.m_regionTable[region_id];
	
	if (rg.sessionIndex == -1) return;	
	
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_castlewar);
	cmd.WriteShort(g2r_castlewar_data_init);
	p_war_info->Serialize(&cmd);	
	g_server.m_region.SendToPeer(rg.sessionIndex);
	
	unguard;
}

void CCastleWarCmdHelper_GW::Send_g2r_castlewar_data_update(long region_id, STWarInfo* p_war_info)
{
	guard;

	GWRegion& rg = g_server.m_regionTable[region_id];

	if (rg.sessionIndex == -1) return;	

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_castlewar);
	cmd.WriteShort(g2r_castlewar_data_update);
	p_war_info->Serialize(&cmd);	
	g_server.m_region.SendToPeer(rg.sessionIndex);

	unguard;
}

void CCastleWarCmdHelper_GW::Send_g2r_castlewar_user_online(long region_id, DWORD user_db_id)
{
	guard;
	
	GWRegion& rg = g_server.m_regionTable[region_id];
	
	if (rg.sessionIndex == -1) return;	
	
	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_castlewar);
	cmd.WriteShort(g2r_castlewar_user_online);
	cmd.WriteLong(user_db_id);
	g_server.m_region.SendToPeer(rg.sessionIndex);
	
	unguard;
}

void CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_prepare_rep(char* p_feod_name, char* p_def_union_name, DWORD total_time)
{	
	guard;
	
	if((p_feod_name == NULL) || (p_def_union_name == NULL)) return;

	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];
		
		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_castlewar);
		cmd.WriteShort(g2r_castlewar_war_prepare_rep);
		cmd.WriteString(p_feod_name);
		cmd.WriteString(p_def_union_name);
		cmd.WriteLong(total_time);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
	
	unguard;
}

void CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_start_rep(char* p_feod_name)
{	
	guard;
	
	CHECK(p_feod_name);

	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];
		
		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_castlewar);
		cmd.WriteShort(g2r_castlewar_war_start_rep);
		cmd.WriteString(p_feod_name);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_bulletin(p_feod_name, NULL, WAR_START);
		
	unguard;
}

void CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_pause_rep(char* p_feod_name, char* p_def_union_name)
{	
	guard;
	
	CHECK(p_feod_name);
	CHECK(p_def_union_name);

	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];
		
		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_castlewar);
		cmd.WriteShort(g2r_castlewar_war_pause_rep);
		cmd.WriteString(p_feod_name);
		cmd.WriteString(p_def_union_name);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_bulletin(p_feod_name, p_def_union_name, WAR_PAUSE);
		
	unguard;
}

void CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_end_rep(char* p_feod_name, char* p_win_union_name)
{	
	guard;
	
	CHECK(p_feod_name);
	CHECK(p_win_union_name);

	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];
		
		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_castlewar);
		cmd.WriteShort(g2r_castlewar_war_end_rep);
		cmd.WriteString(p_feod_name);
		cmd.WriteString(p_win_union_name);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_bulletin(p_feod_name, p_win_union_name, WAR_END);

	unguard;
}

void CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_war_bulletin(char* p_feod_name, char* p_union_name, int op)
{
	guard;

#define SYSMSG_WAR_PREPARE		R(MSG_CASTLEWAR_WAR_READY_START)
#define SYSMSG_WAR_START		R(MSG_CASTLEWAR_WAR_START)
#define SYSMSG_WAR_PAUSE		R(MSG_CASTLEWAR_WAR_PAUSE)
#define SYSMSG_WAR_END			R(MSG_CASTLEWAR_WAR_END)
#define SYSMSG_WAR_WINNER		R(MSG_CASTLEWAR_WAR_WINNER)
#define SYSMSG_WAR_PAUSE_WIN	R(MSG_CASTLEWAR_WAR_PAUSE_WIN)

	std::string feod_name = "";
	std::string union_name = "";
	std::string msg = "";

	if((p_union_name == NULL) || strncmp(p_union_name, "npc", strlen("npc")) == 0)
	{
		union_name = R(MSG_CASTLEWAR_UNION_NAME);
	}else
	{
		union_name = p_union_name;
	}
	
	for(int cnt = 0; cnt < FEOD_COUNT; cnt++)
	{
		if(strncmp(p_feod_name, (char*)FeodNameInDB[cnt].c_str(), strlen(p_feod_name)) == 0)
		{
			feod_name = CastleWarAreaName[cnt];

			break;
		}
	}

	switch(op)
	{
	case WAR_PREPARE:
		msg = feod_name + SYSMSG_WAR_PREPARE;
		break;
		
	case WAR_START:
		msg = feod_name + SYSMSG_WAR_START;
		break;

	case WAR_PAUSE:
		msg = feod_name + SYSMSG_WAR_PAUSE + union_name + SYSMSG_WAR_PAUSE_WIN;
		break;

	case WAR_END:
		msg = feod_name + SYSMSG_WAR_END + union_name + SYSMSG_WAR_WINNER;
		break;

	default:
		break;
	}

    g_server.BroadcastBulletin(msg.c_str());

	unguard;
}

void CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_reload_time_rep(char* p_feod_name, DWORD start_time, int def_duration)
{	
	guard;

	CHECK(p_feod_name);

	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];

		if (rg.sessionIndex == -1)	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_castlewar);
		cmd.WriteShort(g2r_castlewar_reload_time_rep);
		cmd.WriteString(p_feod_name);
		cmd.WriteLong(start_time);
		cmd.WriteLong(def_duration);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}

	unguard;
}

