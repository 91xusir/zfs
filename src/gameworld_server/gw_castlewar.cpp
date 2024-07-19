//-----------------------------------------------------------------------------
//CGWCastleWarMgr.cpp
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "gw_server.h"
#include "gw_castlewar.h"
#include "gw_cmd_castlewar.h"
#include "castlewar_net_define.h"

class CGWCastleWarMgr;

CGWCastleWarMgr g_CastlewarMgr;

CGWCastleWarMgr* GetCastleWarMgr()
{
	return &g_CastlewarMgr;
}

bool CGWCastleWarMgr::Init()
{
	//从数据库中读取
	//...

	/*m_default_weekday	= DEFAULT_WEEKDAY;
	m_default_hour		= DEFAULT_HOUR;
	m_default_min		= DEFAULT_MIN;
	m_default_duration	= DEFAULT_DURATION;*/

	GWDBCommand cmd;
	MYSQL_RES* res;
	
	int			   i = 0;
	DWORD	union_id = 0;
	
	for(i = 0; i < FEOD_COUNT; i++)
	{
		STWarInfo	war_info;
		DWORD		start_time	= 0;

		rt2_sprintf(cmd.command, "SELECT DefWeekDay, DefHour, DefMinute, DefDuration, LatestTime FROM T_castlewar_time_set WHERE `FeodName` LIKE CONVERT( _utf8 '%s' USING gb2312 )",(char*)FeodNameInDB[i].c_str());

		if (!g_server.m_database.ExecCommand(cmd.command, &res))
		{
			//LOG1("SQL Command String : [%s]\n", cmd.command);
			return false;
		}

		int num = mysql_num_rows(res);

		if(num != 1)
		{
			return false;
		}

		MYSQL_ROW row = mysql_fetch_row(res);

		//---------------修改可从表中读取时间---郑泽华09.11.02-----//
		RtCsv csv;
		if (!csv.LoadTableFile("sv_table/castlewar_info.csv"))
		{
			m_default_weekday	= atol(row[0]);
			m_default_hour		= atol(row[1]);
			m_default_min		= atol(row[2]);
			m_default_duration	= atol(row[3]);
		}
		else
		{
			int j = csv.FindLineByString("ID");
			j++;
			m_default_weekday	= csv[j]["星期"].Int();
			m_default_hour		= csv[j]["时"].Int();
			m_default_min		= csv[j]["分"].Int();
			m_default_duration	= csv[j]["持续时间(小时)"].Int();
		}
		//---------------修改可从表中读取时间---郑泽华09.11.02-----//

		war_info.feod_name = FeodNameInDB[i];
		
		GetCastleWarDefaultStartTime(war_info.start_time, atol(row[4]));

		war_info.end_time			= war_info.start_time + m_default_duration * 60 * 60;
		war_info.last_end_time		= 0;
		war_info.master_union_id	= 0;
		war_info.master_union_name	= "";
		war_info.master_union_logo	= "";
		war_info.vassal_name		= "";
		war_info.flag_id			= 0;
		war_info.state				= NPC_OCCUPY_STATE;
		war_info.guard_animal_is_dead = false;

		war_info.v_attacker_union_info.clear();
		war_info.v_defender_union_info.clear();

		m_war_list.push_back(war_info);
	}
	
	rt2_sprintf(cmd.command, "SELECT Name, CurrentUnion FROM T_feod");

	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	int num = mysql_num_rows(res);

	int index = 0;
	
	for(int i = 0; i < num; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);

		for(index = 0; index < FEOD_COUNT; index++)
		{
			if(strncmp((char*)m_war_list[index].feod_name.c_str(), row[0], strlen(row[0])) == 0)
				break;
		}

		if(index >= FEOD_COUNT) continue;

		if(atol(row[1]) == 0) continue;

		CUnion* p_union = GetUnionMgr()->FindUnion(atol(row[1]));

		if(p_union != NULL)
		{
			SUnionData* p_union_data = p_union->GetData();

			if(p_union_data != NULL)
			{
//ldr123
				m_war_list[index].master_union_id	= p_union_data->m_nID;
				m_war_list[index].master_union_name	= p_union_data->m_strName;
// 				m_war_list[index].master_union_logo	= p_union_data->icon;
				
				SUnionCitizen* p_user = p_union->Find(p_union_data->m_nCaptainID);
				if(p_user != NULL)
				{
					m_war_list[index].vassal_name = p_user->m_strPlayerName;
				}
			}
		}
	}

	//GWDBCommand cmd;
	//MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT FeodName, UnionID, UnionName, VassalName, FlagID, IsAttacker, IsAllowed, AnimalDead FROM T_castlewar_info");
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	int count = mysql_num_rows(res);

	for(int i = 0; i < count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		
		for(index = 0; index < FEOD_COUNT; index++)
		{
			if(strncmp((char*)m_war_list[index].feod_name.c_str(), row[0], strlen(row[0])) == 0)
				break;
		}

		if(index >= FEOD_COUNT) continue;

		//m_war_list[index].start_time = atol(row[2]);
		//m_war_list[index].end_time = m_war_list[index].start_time + m_default_duration * 60 * 60;
		//m_war_list[index].guard_animal_is_dead = (atol(row[8]) == 1) ? true : false;
		m_war_list[index].guard_animal_is_dead = false;

		STAttackerInfo attacker_info;
		STDefenderInfo defender_info;

		if(atol(row[5]) == 1)	//攻城方
		{
			attacker_info.union_id = atol(row[1]);
			attacker_info.union_name = row[2];
			attacker_info.vassal_name = row[3];
			attacker_info.flag_id = atol(row[4]);

			m_war_list[index].v_attacker_union_info.push_back(attacker_info);
		}else	//守城方
		{
			defender_info.union_id = atol(row[1]);
			defender_info.union_name = row[2];
			defender_info.vassal_name = row[3];
			defender_info.flag_id = atol(row[4]);
			defender_info.is_allowed = (atol(row[6]) == 1) ? true : false;

			m_war_list[index].v_defender_union_info.push_back(defender_info);
		}
	}
	
	LOG("----------------------------------------------------------\n");
	LOG("-CastleWar Init Sucess\n");
	LOG("----------------------------------------------------------\n");
	for(int i = 0; i < FEOD_COUNT; i++)
	{
		//tim.yang  
		RtLocateTime rtLocateTime(m_war_list[i].start_time);
		//ST_TIME* p_time = localtime((TIME*)&m_war_list[i].start_time);
		//LOG5("-FeodName : [%s], MasterUnionID : [%d], StartTime : [%d] [%d] [%d]\n", (char*)m_war_list[i].feod_name.c_str(), m_war_list[i].master_union_id, p_time->tm_wday, p_time->tm_hour, p_time->tm_min);
		// LOG5("-FeodName : MasterUnionID : [%d], StartTime : [%d] [%d] [%d]\n", m_war_list[i].master_union_id, p_time->tm_wday, p_time->tm_hour, p_time->tm_min);
		LOG5("-FeodName : [%s], MasterUnionID : [%d], StartTime : [%d] [%d] [%d]\n",
			(char*)m_war_list[i].feod_name.c_str(),
			m_war_list[i].master_union_id,
			rtLocateTime.iDayOfWeek,
			rtLocateTime.iHour,
			rtLocateTime.iMin);
	}
	LOG("----------------------------------------------------------\n");

	return true;
}

bool CGWCastleWarMgr::ReloadTime(std::string feod_name)
{
	int i = 0;

	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(strncmp((char*)feod_name.c_str(), (char*)m_war_list[i].feod_name.c_str(), strlen((char*)feod_name.c_str())) == 0)
		{
			break;
		}
	}

	if(i >= FEOD_COUNT)
	{
		return false;
	}
	
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT DefWeekDay, DefHour, DefMinute, DefDuration, LatestTime FROM T_castlewar_time_set WHERE `FeodName` LIKE CONVERT( _utf8 '%s' USING gb2312 )",(char*)feod_name.c_str());

	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	int num = mysql_num_rows(res);

	if(num != 1)
	{
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	m_default_weekday = atol(row[0]);

	m_default_hour = atol(row[1]);

	m_default_min = atol(row[2]);

	m_default_duration = atol(row[3]);

	GetCastleWarDefaultStartTime(m_war_list[i].start_time, atol(row[4]));

	string time_str = fox::TimeConvert(m_war_list[i].start_time);
	LOG2("[%s] Reload CastleWarTime is [%s]\n", m_war_list[i].feod_name.c_str(), time_str.c_str());

	m_war_list[i].end_time = m_war_list[i].start_time + atol(row[3]) * 60 * 60;

	CCastleWarCmdHelper_GW::Broadcast_g2r_castlewar_reload_time_rep((char*)feod_name.c_str(), m_war_list[i].start_time, atol(row[3]));

    return true;
}

bool CGWCastleWarMgr::SetLatestWarTime(string feod_name, DWORD latest_time)
{
	int feod_cnt = 0;

	for(feod_cnt = 0; feod_cnt < (int)m_war_list.size(); feod_cnt++)
	{
		if(feod_name == m_war_list[feod_cnt].feod_name)
		{
			break;
		}
	}

	if(feod_cnt >= (int)m_war_list.size())
	{
		return false;
	}

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_castlewar_time_set SET LatestTime=%d WHERE `FeodName` LIKE CONVERT( _utf8 '%s' USING gb2312 )",
						latest_time,
						(char*)feod_name.c_str());

	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	return true;
}

bool CGWCastleWarMgr::SetDefWarTime(string feod_name, int def_weekday, int def_hour, int def_minute, int def_duration)
{
	int feod_cnt = 0;

	for(feod_cnt = 0; feod_cnt < (int)m_war_list.size(); feod_cnt++)
	{
		if(feod_name == m_war_list[feod_cnt].feod_name)
		{
			break;
		}
	}

	if(feod_cnt >= (int)m_war_list.size())
	{
		return false;
	}

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_castlewar_time_set SET DefWeekDay=%d, DefHour=%d, DefMinute=%d, DefDuration=%d WHERE `FeodName` LIKE CONVERT( _utf8 '%s' USING gb2312 )",
						 def_weekday,
						 def_hour,
						 def_minute,
						 def_duration,
						 (char*)feod_name.c_str());

	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	return true;
}

bool CGWCastleWarMgr::SaveToDB(string feod_name)
{
	int feod_cnt = 0;

	for(feod_cnt = 0; feod_cnt < (int)m_war_list.size(); feod_cnt++)
	{
		if(feod_name == m_war_list[feod_cnt].feod_name)
		{
			break;
		}
	}

	if(feod_cnt >= (int)m_war_list.size())
	{
		return false;
	}
	
	//清空数据表
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "DELETE FROM T_castlewar_info WHERE FeodName = '%s'", feod_name.c_str());
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		LOG1("SQL Command Error : [%s]\n", cmd.command);
		return false;
	}

	//LOG1("SQL Command String : [%s]\n", cmd.command);

	//所有城战攻守双方信息添加至数据库
	
	//攻城方
	for(int i = 0; i < (int)m_war_list[feod_cnt].v_attacker_union_info.size(); i++)
	{				
		rt2_sprintf(cmd.command, "INSERT INTO T_castlewar_info(FeodName, UnionID, StartTime, UnionName, VassalName, FlagID, IsAttacker, IsAllowed, AnimalDead) VALUES("
							"'%s', "
							"%d, "
							"%d, "
							"'%s', "
							"'%s', "
							"%d, "
							"%d, "
							"%d, "
							"%d)",
							(char*)m_war_list[feod_cnt].feod_name.c_str(),
							(int)m_war_list[feod_cnt].v_attacker_union_info[i].union_id,
							(int)m_war_list[feod_cnt].start_time,
							(char*)m_war_list[feod_cnt].v_attacker_union_info[i].union_name.c_str(),
							(char*)m_war_list[feod_cnt].v_attacker_union_info[i].vassal_name.c_str(),
							(int)m_war_list[feod_cnt].v_attacker_union_info[i].flag_id,
							1,
							0,
							m_war_list[feod_cnt].guard_animal_is_dead ? 1 : 0);

		if (!g_server.m_database.ExecCommand(cmd.command, &res))
		{
			LOG1("SQL Command Error : [%s]\n", cmd.command);
			return false;
		}
	}
	//防守方
	for(int i = 0; i < (int)m_war_list[feod_cnt].v_defender_union_info.size(); i++)
	{
		rt2_sprintf(cmd.command, "INSERT INTO T_castlewar_info(FeodName, UnionID, StartTime, UnionName, VassalName, FlagID, IsAttacker, IsAllowed, AnimalDead) VALUES("
							"'%s', "
							"%d, "
							"%d, "
							"'%s', "
							"'%s', "
							"%d, "
							"%d, "
							"%d, "
							"%d)",
							(char*)m_war_list[feod_cnt].feod_name.c_str(),
							(int)m_war_list[feod_cnt].v_defender_union_info[i].union_id,
							(int)m_war_list[feod_cnt].start_time,
							(char*)m_war_list[feod_cnt].v_defender_union_info[i].union_name.c_str(),
							(char*)m_war_list[feod_cnt].v_defender_union_info[i].vassal_name.c_str(),
							(int)m_war_list[feod_cnt].v_defender_union_info[i].flag_id,
							0,
							m_war_list[feod_cnt].v_defender_union_info[i].is_allowed ? 1 : 0,
							m_war_list[feod_cnt].guard_animal_is_dead ? 1 : 0);

		if (!g_server.m_database.ExecCommand(cmd.command, &res))
		{
			LOG1("SQL Command Error : [%s]", cmd.command);
			return false;
		}
	}
	
	return true;
}

bool CGWCastleWarMgr::IsInSameWeek(DWORD tm1, DWORD tm2)
{
	//ST_TIME*	p_time1;	//当前时间
	//ST_TIME*	p_time2;	//上次时间
	//ST_TIME		s_t1;
	//ST_TIME		s_t2;

	//p_time1 = localtime((time_t*)&tm1);
	//s_t1	= *p_time1;
	//p_time2 = localtime((time_t*)&tm2);
	//s_t2	= *p_time2;

	//s_t1.tm_wday = (s_t1.tm_wday == 0) ? 7 : s_t1.tm_wday;
	//s_t2.tm_wday = (s_t2.tm_wday == 0) ? 7 : s_t2.tm_wday;

	//DWORD firstday1 = tm1 - (s_t1.tm_wday-1) * 24 * 60 * 60
	//					- s_t1.tm_hour * 60 * 60
	//					- s_t1.tm_min * 60
	//					- s_t1.tm_sec;

	//DWORD firstday2 = tm2 - (s_t2.tm_wday-1) * 24 * 60 * 60
	//					- s_t2.tm_hour * 60 * 60
	//					- s_t2.tm_min * 60
	//					- s_t2.tm_sec;

	//tim.yang  此处调用localtime转时间函数时会出现空指针，所以用RtLocateTime
	RtLocateTime p_time1(tm1);
	RtLocateTime p_time2(tm2);
	//p_time1 = localtime((TIME*)&tm1);
	//s_t1	= *p_time1;
	//p_time2 = localtime((TIME*)&tm2);
	//s_t2	= *p_time2;

	p_time1.iDayOfWeek = (p_time1.iDayOfWeek == 0) ? 7 : p_time1.iDayOfWeek;
	p_time2.iDayOfWeek = (p_time2.iDayOfWeek == 0) ? 7 : p_time2.iDayOfWeek;

	DWORD firstday1 = tm1 - (p_time1.iDayOfWeek-1) * 24 * 60 * 60
		- p_time1.iHour * 60 * 60
		- p_time1.iMin * 60
		- p_time1.iSec;

	DWORD firstday2 = tm2 - (p_time2.iDayOfWeek-1) * 24 * 60 * 60
		- p_time2.iHour * 60 * 60
		- p_time2.iMin * 60
		- p_time2.iSec;
	//end
	if(firstday1 == firstday2)
	{
		return true;
	}

	return false;
}

void CGWCastleWarMgr::GetCastleWarDefaultStartTime(DWORD& ret, DWORD latest_time)
{
	//---------------------------
	//取得下次默认攻城时间
	//---------------------------
	ST_TIME*	p_local_time;	//当前时间
	DWORD		interval;		//时间间隔(second)

	//取得当前时间：年月日分秒
	TIME tmp = 0;	
	time(&tmp);
	p_local_time = localtime(&tmp);
	
	//攻城一周一次
	if((p_local_time->tm_wday < m_default_weekday)  ||
		((p_local_time->tm_wday == m_default_weekday) && (p_local_time->tm_hour < m_default_hour)) ||
		((p_local_time->tm_wday == m_default_weekday) && (p_local_time->tm_hour == m_default_hour) && 
		(p_local_time->tm_min < m_default_min)))
	{
		interval = ((m_default_weekday - p_local_time->tm_wday) * 24 * 60 * 60) +   //天数差
					((m_default_hour - p_local_time->tm_hour) * 60 * 60) +			//小时差
					((m_default_min - p_local_time->tm_min) * 60) -					//分钟差
					(p_local_time->tm_sec);											//秒差
		
		if(latest_time != 0)
		{
			if(IsInSameWeek(tmp, latest_time))
			{
				//interval += 7 * 24 * 60 * 60;		// 暂时去除,便于测试...郑泽华09.11.02
			}
		}
	
	}else
	{
		interval = ((7 - (p_local_time->tm_wday - m_default_weekday)) * 24 * 60 * 60) -    //天数差
					((p_local_time->tm_hour - m_default_hour) * 60 * 60) -				   //小时差
					((p_local_time->tm_min - m_default_min) * 60) -						   //分钟差
					(p_local_time->tm_sec);												   //秒差
	}

	ret = tmp + interval;

	return;
}

void CGWCastleWarMgr::OnUserOnline(long region_id, DWORD user_db_id)
{
	CCastleWarCmdHelper_GW::Send_g2r_castlewar_user_online(region_id, user_db_id);

	return;
}

void CGWCastleWarMgr::OnRegionRegister(long region_id)
{
	for(int i = 0; i < FEOD_COUNT; i++)
	{	
		CCastleWarCmdHelper_GW::Send_g2r_castlewar_data_init(region_id, &m_war_list[i]);
	}

	return;
}

void CGWCastleWarMgr::UpdateData(STWarInfo* p_war_info)
{
	if(p_war_info == NULL)
	{
		return;
	}
	
	int i = 0;
	for(i = 0; i < (int)m_war_list.size(); i++)
	{
		if(p_war_info->feod_name == m_war_list[i].feod_name)
		{
			break;
		}
	}
	
	if(i >= (int)m_war_list.size())
	{
		return;
	}
	
	m_war_list[i] = (*p_war_info);

#ifdef _DEBUG
	LOG ("-----------------CastleWar Save Data STR-------------------------\n");
	LOG2("-FeodName: [%s]       MasterUnionName: [%s]\n", m_war_list[i].feod_name.c_str(), m_war_list[i].master_union_name.c_str());
	LOG2("-Attacker Number: [%d]     Defender Number: [%d]\n",m_war_list[i].v_attacker_union_info.size(), m_war_list[i].v_defender_union_info.size());
	LOG ("-----------------CastleWar Save Data END-------------------------\n");
#endif //_DEBUG

	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;
		
		CCastleWarCmdHelper_GW::Send_g2r_castlewar_data_update(rgID, p_war_info);
	}

	
	//存档至数据库
	SaveToDB(m_war_list[i].feod_name);	
}
