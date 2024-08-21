//-----------------------------------------------------------------------------
//CRegionCastleWarMgr.cpp
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "region.h"
//PZH
#include "PlayerLogic.h"
//
CRegionCastleWarMgr g_RegionCastleWarMgr;

/*
STCASTLEWAR_AREA_NAME CastleWarAreaName[FEOD_COUNT] = 
{
	{"XiQi",	"西岐城"}, 
	{"ZhaoGe",	"朝歌城"},
	{"ChuanYun","穿云关"}
};
*/

//#define WEEKDAY			"星期"
//#define HOUR			"时"
//#define MINUTE			"分"
//#define DURATION		"持续时间(小时)"

#define NUM(array)			(sizeof(array) / sizeof((array[0])))

const std::string SceneNameZhou[] = 
{
	"scene01_002.rtb",
	"scene01_003.rtb",
	"scene01_004.rtb",
	"scene01_005.rtb"
};

const std::string SceneNameShang[] = 
{
	"scene01_029.rtb",
	"scene01_030.rtb",
	"scene01_035.rtb",
	"scene01_036.rtb"
};

CRegionCastleWarMgr::CRegionCastleWarMgr()
{
	CM_MEMPROTECTOR(m_dir, 3)
	isStartWar = false;//tim.yang  城战
	//Init();
}

CRegionCastleWarMgr::~CRegionCastleWarMgr()
{
	CM_MEMUNPROTECTOR(m_dir)
}

//bool CRegionCastleWarMgr::LoadCastleWarInfo(const char* file_name)
//{
//	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
//#ifdef WAR_SWITCH
//	return true;
//#endif //WAR_SWITCH
//	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
//	
//	if(file_name == NULL) return false;
//
//	RtCsv file;
//
//	if (!file.LoadTableFile(file_name))
//	{
//		ERR1("Can not open castle war table file %s\n", file_name);
//
//		return false;
//	}
//
//	int titleLine = file.FindLineByString("ID");
//
//	if (titleLine < 0)
//	{
//		ERR("Invalid castle war table format, missing ID column\n");
//		return false;
//	}
//
//	int numLines = file.GetLineCount();
//
//	if (numLines <= 1)
//	{
//		ERR1("Invalid castle war table format (lines=%d)\n", numLines);
//		return false;
//	}
//
//	for(int i = 0; i < FEOD_COUNT; i++)
//	{
//		m_week_day[i]	= file[titleLine + 1][WEEKDAY].Int();
//		m_hour[i]		= file[titleLine + 1][HOUR].Int();
//		m_min[i]		= file[titleLine + 1][MINUTE].Int();
//		m_duration[i]	= file[titleLine + 1][DURATION].Int() * 60 * 60;
//	}
//
//	LOG4("CastleWar Info WeekDay[%d]  Hour[%d]  Min[%d]  Duration[%d]\n", m_week_day, m_hour, m_min, m_duration);
//
//	return true;	
//}

bool CRegionCastleWarMgr::CreateFlagNpc(CRegionCreature* p_sender_cre)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return true;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */

	short npc_id = 0;

	if(p_sender_cre->m_core.Faction == FACTION_ZHOU)
	{
		npc_id = FLAG_NPC_ID_ZHOU;
	}else
	{
		npc_id = FLAG_NPC_ID_SHANG;
	}
	
	CRegionCreature *cre = g_region->CreateNpc(npc_id);
	if(!cre) 
	{
		LOG1("Cannot create flag [%d]\n", npc_id);

		return false;
	}

	if(g_region->m_defScene->AddCreature(cre, p_sender_cre->m_pos, p_sender_cre->m_dir))
	{		
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);

		if(!AddFlag(p_sender_cre->m_unionID, cre->m_oId))
		{
			LOG2("Cannot add flag [%d] to union [%d]\n", npc_id, p_sender_cre->m_unionID);
			return false;
		}
	}
	else
	{
		g_factory->DestroyObject(cre);

		cre = NULL;
	}

	cre->m_unionName = p_sender_cre->m_unionName;
	cre->m_unionID	 = p_sender_cre->m_unionID;
	
	return true;
}

bool CRegionCastleWarMgr::CreateGuardAnimalNpc(string feod_name)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return true;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	STWarInfo* p_war_info = FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return false;
	}

	if(p_war_info->guard_animal_is_dead == false)
	{
		return false;
	}
	
	int index = 0;

	for(index = 0; index < FEOD_COUNT; index++)
	{
		if(feod_name == FeodNameInDB[index]) break;
	}

	if(index >= FEOD_COUNT)
	{
		return false;
	}
	
	p_war_info->guard_animal_is_dead = false;

	CRegionCreature *cre = g_region->CreateNpc(m_animal_id[index]);
	if(!cre) 
	{
		return false;
	}

	if(g_region->m_defScene->AddCreature(cre, m_animal_pos[index], m_dir[index]))
	{		
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	else
	{
		g_factory->DestroyObject(cre);

		cre = NULL;
	}
	
	return true;
}

bool CRegionCastleWarMgr::CreateCastleGuardNpc(std::string feod_name)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	g_GuarderMgr.CreateAllCastleWarGuarder(feod_name);

	return true;
}

bool CRegionCastleWarMgr::KillCastleGuardNpc(std::string feod_name)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	g_GuarderMgr.KillAllCastleWarGuarder(feod_name);

	return true;
}

bool CRegionCastleWarMgr::KillFlagNpc(CRegionCreature* p_flag_npc, bool b_is_killed)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_flag_npc == NULL) return false;

	if(p_flag_npc->m_oId == 0) return false;

	if(!DelFlag(p_flag_npc->m_oId, b_is_killed))
	{
		LOG("Remove Flag Npc Error\n");
	}

	p_flag_npc->m_scene->RemoveCreature(p_flag_npc->m_oId);

	g_factory->DestroyObject(p_flag_npc);

	return true;
}

bool CRegionCastleWarMgr::KillAnimalNpc(CRegionCreature* p_killer, CRegionCreature* p_self)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
		
	if((p_killer == NULL) || (p_self == NULL) || (p_self->m_npcInfo == NULL))
	{
		return false;
	}
	
	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(p_killer->m_unionID);
	if(iter_union == g_region->m_unionMap.end())
	{
		return false;
	}

	std::string feod_name = p_self->m_npcInfo->Home;
	short		animal_id = p_self->m_npcInfo->Id;
	char		dir		  = p_self->m_dir;
	long		pos[3];

	memcpy((void *)pos, (void *)p_self->m_pos, sizeof(pos));

	STWarInfo* p_war_info = FindByName(feod_name);

	if((p_war_info == NULL) || (p_war_info->guard_animal_is_dead == true))
	//if(p_war_info == NULL)
	{
		return false;
	}		
	
	p_self->m_scene->RemoveCreature(p_self->m_oId);

	g_factory->DestroyObject(p_self);

	//守护兽已死
	p_war_info->guard_animal_is_dead = true;

	//守护兽位置
	int index = 0;

	for(index = 0; index < FEOD_COUNT; index++)
	{
		if(feod_name == FeodNameInDB[index]) break;
	}

	if(index >= FEOD_COUNT)
	{
		return false;
	}

	m_animal_pos[index][0] = pos[0];
	m_animal_pos[index][1] = pos[1];
	m_animal_pos[index][2] = pos[2];
	m_dir[index] = dir;

	if(m_current_time + PREPARE_TIME * 60 >= p_war_info->end_time)
	{
		EndWar(feod_name, iter_union->second.m_nID);
	}else
	{
		PauseWar(feod_name, iter_union->second.m_nID);	//5分钟
	}

	
	//CreateGuardAnimalNpc(feod_name);

	return true;
}

bool CRegionCastleWarMgr::IsFlagNpc(short flag_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	return ((flag_id == FLAG_NPC_ID_SHANG) || (flag_id == FLAG_NPC_ID_ZHOU)) ? true : false;
	
}

bool CRegionCastleWarMgr::IsGuardAnimal(short animal_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if((animal_id == XIQI_ANIMAL_NPC_ID) ||
	   (animal_id == ZHAOGE_ANIMAL_NPC_ID) ||
	   (animal_id == CHUANYUN_ANIMAL_NPC_ID))
	{
		return true;
	}

	return false;
}

void CRegionCastleWarMgr::Init()
{
	int			   i = 0;
	DWORD	union_id = 0;
	TIME		 tmp = 0;

	//---------------修改可从表中读取时间---郑泽华09.11.02-----//
	RtCsv csv;
	if (!csv.LoadTableFile("sv_table/castlewar_info.csv"))
	{
		for(int cnt = 0; cnt < FEOD_COUNT; cnt++)
		{
			m_week_day[cnt] = 5;
			m_hour[cnt] = 20;
			m_min[cnt] = 0;
			m_duration[cnt] = 2 * 60 * 60;
		}
	}
	else
	{
		int j = csv.FindLineByString("ID");
		j++;
		for(int cnt = 0; cnt < FEOD_COUNT; cnt++)
		{
			m_week_day[cnt]	= csv[j]["星期"].Int();
			m_hour[cnt]		= csv[j]["时"].Int();
			m_min[cnt]		= csv[j]["分"].Int();
			m_duration[cnt]	= csv[j]["持续时间(小时)"].Int();
		}
	}
	//---------------修改可从表中读取时间---郑泽华09.11.02-----//

	time(&tmp);

	m_current_time = (DWORD)tmp;

	for(i = 0; i < FEOD_COUNT; i++)
	{
		STWarInfo	war_info;
		DWORD		start_time	= 0;
	
		//memset((void *)&war_info, 0, sizeof(war_info));

		war_info.feod_name = FeodNameInDB[i];

		GetCastleWarDefaultStartTime(war_info.start_time, i);

		war_info.end_time			= war_info.start_time + m_duration[i];
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

	memset((void*)m_animal_pos, 0, sizeof(m_animal_pos));

	memset((void*)m_dir, 0, sizeof(m_dir));

	m_animal_id[0] = XIQI_ANIMAL_NPC_ID;
	m_animal_id[1] = ZHAOGE_ANIMAL_NPC_ID;
	m_animal_id[2] = CHUANYUN_ANIMAL_NPC_ID;

	//西岐点
	m_rebirth_pos[0][0][0] = 1650;
	m_rebirth_pos[0][0][1] = -8035;
	m_rebirth_pos[0][0][2] = 0;

	m_rebirth_pos[0][1][0] = 2755;
	m_rebirth_pos[0][1][1] = -11615;
	m_rebirth_pos[0][1][2] = 0;

	m_rebirth_pos[0][2][0] = 2145;
	m_rebirth_pos[0][2][1] = -4825;
	m_rebirth_pos[0][2][2] = 0;

	//朝歌点
	m_rebirth_pos[1][0][0] = 48775;
	m_rebirth_pos[1][0][1] = -8500;
	m_rebirth_pos[1][0][2] = 0;

	m_rebirth_pos[1][1][0] = 49215;
	m_rebirth_pos[1][1][1] = -11205;
	m_rebirth_pos[1][1][2] = 0;

	m_rebirth_pos[1][2][0] = 49965;
	m_rebirth_pos[1][2][1] = -6690;
	m_rebirth_pos[1][2][2] = 0;

	//穿云关点
	m_rebirth_pos[2][0][0] = 0;
	m_rebirth_pos[2][0][1] = 0;
	m_rebirth_pos[2][0][2] = 0;

	m_rebirth_pos[2][1][0] = 0;
	m_rebirth_pos[2][1][1] = 0;
	m_rebirth_pos[2][1][2] = 0;

	m_rebirth_pos[2][2][0] = 0;
	m_rebirth_pos[2][2][1] = 0;
	m_rebirth_pos[2][2][2] = 0;

	m_flag = 3;
}

void CRegionCastleWarMgr::OnRecvInitData(STWarInfo* p_war_info)
{
	if(p_war_info == NULL)
	{
		return;
	}

	/* 初始化数据处理
	 * Region数据已经经过自身初始化，
	 * 现在接收gw传过来的每个采邑的具体存档信息
	 * index为采邑索引
	 */
	int index = 0;

	for(index = 0; index < FEOD_COUNT; index++)
	{
		if(p_war_info->feod_name == m_war_list[index].feod_name)
		{
			break;
		}
	}

	if(index >= FEOD_COUNT)
	{
		return;
	}

	m_war_list[index] = (*p_war_info);

	/* 初始化时间
	 * 根据索引初始化时间
	 */
	RtLocateTime rtLocateTime(m_war_list[index].start_time);
	//ST_TIME* p_time = localtime((TIME*)&m_war_list[index].start_time);

	m_week_day[index]	= rtLocateTime.iDayOfWeek;
	m_hour[index]		= rtLocateTime.iHour;
	m_min[index]		= rtLocateTime.iMin;
	m_duration[index]	= 2 * 60 * 60;
	
	if(m_war_list[index].master_union_id != 0)
	{
		m_war_list[index].state = UNION_OCCUPY_STATE;
	}

	TIME tmp = 0;

	time(&tmp);

	if(m_war_list[index].end_time < (DWORD)tmp)
	{
		GetCastleWarDefaultStartTime(m_war_list[index].start_time, index);

		m_war_list[index].end_time = m_war_list[index].start_time + m_duration[index];
	}

	///* 令旗特殊处理:
	// * 如果令旗id不为零，表示该诸侯插过令旗
	// * 重新初始化时将id置为零，并给诸侯添加
	// * 令旗道具
	// */
	if( (m_war_list[index].flag_id != 0) &&
		(m_war_list[index].flag_id != 0xffff))
	{
		m_war_list[index].flag_id = 0;
	}

	for(int i = 0; i < (int)m_war_list[index].v_attacker_union_info.size(); i++)
	{
		if( (m_war_list[index].v_attacker_union_info[i].flag_id != 0) &&
			(m_war_list[index].v_attacker_union_info[i].flag_id != 0xffff))
		{
			m_war_list[index].v_attacker_union_info[i].flag_id = 0;
		}
	}

	for(int i = 0; i < (int)m_war_list[index].v_defender_union_info.size(); i++)
	{
		if( (m_war_list[index].v_defender_union_info[i].flag_id != 0) &&
			(m_war_list[index].v_defender_union_info[i].flag_id != 0xffff))
		{
			m_war_list[index].v_defender_union_info[i].flag_id = 0;
		}
	}
#ifdef _DEBUG

	LOG("-------------Region Recevied Init Data--------------------------------------\n");
	for(int i = 0; i < FEOD_COUNT; i++)
	{
		LOG2("-FeodName: [%s]  MasterUnionName: [%s]\n", (char*)m_war_list[i].feod_name.c_str(), (char*)m_war_list[i].master_union_name.c_str());
		LOG1("-StartTime: [%d]\n", m_war_list[i].start_time);
	}
	LOG("----------------------------------------------------------------------------\n");
#endif //_DEBUG

	return;
}

void CRegionCastleWarMgr::OnRecvUpdateData(STWarInfo* p_war_info)
{
	if(p_war_info == NULL)
	{
		return;
	}

	int index = 0;

	for(index = 0; index < FEOD_COUNT; index++)
	{
		if(p_war_info->feod_name == m_war_list[index].feod_name)
		{
			break;
		}
	}

	if(index >= FEOD_COUNT)
	{
		return;
	}

	m_war_list[index] = (*p_war_info);	
	
	return;
}

//-------------------------------------------
//规则相关
//-------------------------------------------
void CRegionCastleWarMgr::GetCastleWarDefaultStartTime(DWORD& ret, int index)
{
	//---------------------------
	//取得下次默认攻城时间
	//---------------------------
	ST_TIME*	p_local_time;	//当前时间
	DWORD		interval;		//时间间隔(second)

	//取得当前时间：年月日分秒
	p_local_time = localtime((TIME *)(&m_current_time));

	
	//如果今天是周日、周一、周二或者周三20：00以前
	//那么下次默认攻城时间为指定默认时间
	if((p_local_time->tm_wday < m_week_day[index])  ||
	   ((p_local_time->tm_wday == m_week_day[index]) && (p_local_time->tm_hour < m_hour[index])) ||
	   ((p_local_time->tm_wday == m_week_day[index]) && (p_local_time->tm_hour == m_hour[index]) && (p_local_time->tm_min < m_min[index])))
	{
		interval = ((m_week_day[index] - p_local_time->tm_wday) * 24 * 60 * 60) +    //天数差
				   ((m_hour[index] - p_local_time->tm_hour) * 60 * 60) +			 //小时差
				   ((m_min[index] - p_local_time->tm_min) * 60) -					 //分钟差
				   (p_local_time->tm_sec);											 //秒差
	}else
	{
		interval = ((7 - (p_local_time->tm_wday - m_week_day[index])) * 24 * 60 * 60) -		//天数差
				   ((p_local_time->tm_hour - m_hour[index]) * 60 * 60) -				    //小时差
				   ((p_local_time->tm_min - m_min[index]) * 60) -							//分钟差
				   (p_local_time->tm_sec);													//秒差
	}
	
	ret = m_current_time + interval;

	return;
}

void CRegionCastleWarMgr::StartWar(string feod_name)
{
	isStartWar = true;//tim.yang 城战已经开始
	//遍历当前所有采邑
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(m_war_list[i].feod_name == feod_name) break;
	}

	if(i >= FEOD_COUNT) return;

	if(m_war_list[i].state == WARFARE_STATE) return;

	m_war_list[i].state = WARFARE_STATE;

	if(m_war_list[i].master_union_id != 0)
	{
		UpdateCreWarInfo(feod_name, m_war_list[i].master_union_id, (BYTE)0);
	}

	for(int cnt = 0; cnt < (int)m_war_list[i].v_attacker_union_info.size(); cnt++)
	{
		UpdateCreWarInfo(feod_name, m_war_list[i].v_attacker_union_info[cnt].union_id,(BYTE)1);
	}

	for(int cnt = 0; cnt < (int)m_war_list[i].v_defender_union_info.size(); cnt++)
	{
		if(m_war_list[i].v_defender_union_info[cnt].is_allowed == true)
		{
			UpdateCreWarInfo(feod_name, m_war_list[i].v_defender_union_info[cnt].union_id,(BYTE)0);
		}
	}
	
	if(m_war_list[i].master_union_id == 0)
	{
		//生成守城NPC
		if(!CreateCastleGuardNpc(feod_name))
		{
			//...
		}
	}

	CreateGuardAnimalNpc(feod_name);

	//传送不相关人员
	if(!TransUser(feod_name))
	{
		//...
	}
	
	//消息建构并发送
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(&m_war_list[i]);

	CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_start(feod_name);

	//--------------log info start-----------------------//
	ST_TIME*	p_local_time;	//当前时间
	TIME		tmp;

	time(&tmp);
	p_local_time = localtime((TIME *)(&tmp));
	LOG3("[%s] CastleWar Started at  [%d]-[%d]-[%d]  [%d]:[%d]\n",
										feod_name.c_str(),
										p_local_time->tm_year + 1900,
										p_local_time->tm_mon + 1,
										p_local_time->tm_mday,
										p_local_time->tm_hour,
										p_local_time->tm_min);
	//--------------log info end ------------------------//
}

void CRegionCastleWarMgr::ClearAllFlagNpc(int index)
{
	if(index >= FEOD_COUNT) return;

	//清除当前令旗
	//攻城方
	int			i = 0;
	DWORD flag_id = 0;

	if(m_war_list[index].master_union_id != 0)
	{
		flag_id = m_war_list[index].flag_id;

		if(flag_id != 0)
		{
			CRegionCreature* p_cre = g_region->m_defScene->FindCreature(flag_id);

			if(p_cre != NULL)
			{
				KillFlagNpc(p_cre, false);
			}
		}else
		{
			//删除令旗道具
			CRegionCreature* p_cre = find_vassal_by_union_id(m_war_list[index].master_union_id);

			if(p_cre != NULL && p_cre->m_pItemContainer != NULL)
			{
				int nItemCount, page, gridI, gridJ;
				bool bFindItem = p_cre->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
				if (bFindItem)
				{
					p_cre->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
					CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
					p_cre->RecvCmd(g_sendCmd);
				}
			}
		}
	}
	
	for(i = 0; i < (int)m_war_list[index].v_attacker_union_info.size(); i++)
	{
		flag_id = m_war_list[index].v_attacker_union_info[i].flag_id;

		if(flag_id != 0)
		{
			CRegionCreature* p_cre = g_region->m_defScene->FindCreature(flag_id);

			if(p_cre != NULL)
			{
				KillFlagNpc(p_cre, false);
			}
		}else
		{
			//删除令旗道具
			CRegionCreature* p_cre = find_vassal_by_union_id(m_war_list[index].v_attacker_union_info[i].union_id);

			if(p_cre != NULL && p_cre->m_pItemContainer != NULL)
			{
				int nItemCount, page, gridI, gridJ;
				bool bFindItem = p_cre->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
				if (bFindItem)
				{
					p_cre->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
					CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
					p_cre->RecvCmd(g_sendCmd);
				}
			}
		}
	}

	//守城方
	for(i = 0; i < (int)m_war_list[index].v_defender_union_info.size(); i++)
	{
		flag_id = m_war_list[index].v_defender_union_info[i].flag_id;

		if(flag_id != 0)
		{
			CRegionCreature* p_cre = g_region->m_defScene->FindCreature(flag_id);

			if(p_cre != NULL)
			{
				KillFlagNpc(p_cre, false);
			}
		}else
		{
			//删除令旗道具
			CRegionCreature* p_cre = find_vassal_by_union_id(m_war_list[index].v_defender_union_info[i].union_id);
			
			if(p_cre != NULL && p_cre->m_pItemContainer != NULL)
			{
				int nItemCount, page, gridI, gridJ;
				bool bFindItem = p_cre->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
				if (bFindItem)
				{
					p_cre->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
					CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
					p_cre->RecvCmd(g_sendCmd);
				}
			}
		}
	}	

	return;
}

void CRegionCastleWarMgr::EndWar(string feod_name, DWORD winner_id)
{
	isStartWar = false;//tim.yang  城战结束
	//遍历当前所有采邑
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(m_war_list[i].feod_name == feod_name) break;
	}

	if(i >= FEOD_COUNT) return;

	if( (m_war_list[i].state == NPC_OCCUPY_STATE) ||
		(m_war_list[i].state == UNION_OCCUPY_STATE))
	{
		return;
	}

	//删除令旗npc，同时删除令旗道具
	ClearAllFlagNpc(i);

	ReSetAllUserWarInfo(&m_war_list[i]);
	
	GetCastleWarDefaultStartTime(m_war_list[i].start_time, i);
	m_war_list[i].last_end_time = m_war_list[i].end_time;
	m_war_list[i].end_time = m_war_list[i].start_time + m_duration[i];
	m_war_list[i].v_attacker_union_info.clear();
	m_war_list[i].v_defender_union_info.clear();

	if(winner_id == 0)
	{				
		m_war_list[i].master_union_id	= 0;
		m_war_list[i].master_union_name	= "npc";
		m_war_list[i].master_union_logo	= "";
		m_war_list[i].vassal_name		= "";
		m_war_list[i].flag_id			= 0;
		m_war_list[i].state				= NPC_OCCUPY_STATE;	

		if(!KillCastleGuardNpc(feod_name))
		{
			//...
		}
		
		//消息建构并发送
		//CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_end(feod_name, m_war_list[i].master_union_name);		
		
	}else
	{
		UpdateCreWarInfo(feod_name, winner_id, (BYTE)0);

		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union;
		iter_union = g_region->m_unionMap.find(winner_id);
		if(iter_union == g_region->m_unionMap.end())
		{
			return;
		}

		SUnionData union_data = iter_union->second;

		SUnionCitizen* p_citizen = union_data.Find(union_data.m_nCaptainID);
		
		if(p_citizen != NULL)
		{
			m_war_list[i].master_union_id	= winner_id;
			m_war_list[i].master_union_name	= union_data.m_strName;
// 			m_war_list[i].master_union_logo	= union_data.icon;
			m_war_list[i].vassal_name		= p_citizen->m_strPlayerName;
			m_war_list[i].flag_id			= 0;
			m_war_list[i].state				= UNION_OCCUPY_STATE;
		}

		if(!KillCastleGuardNpc(feod_name))
		{
			//...
		}

		//消息建构并发送
		//CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_end(feod_name, m_war_list[i].master_union_name);
	}

	CreateGuardAnimalNpc(feod_name);

	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(&m_war_list[i]);

	CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_end(feod_name, m_war_list[i].master_union_name);
	
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(&m_war_list[i]);

	g_region->BroadcastCmd(g_sendCmd, NULL);

	CFeodCmdBuilder_Svr::NetSend_r2g_feod_change_union((char*)m_war_list[i].feod_name.c_str(), winner_id);

	//--------------log info start-----------------------//
	ST_TIME*	p_local_time;	//当前时间
	TIME		tmp;

	time(&tmp);
	p_local_time = localtime((TIME *)(&tmp));
	LOG3("[%s] CastleWar End at  [%d]-[%d]-[%d]  [%d]:[%d]\n",
									feod_name.c_str(),
									p_local_time->tm_year + 1900,
									p_local_time->tm_mon + 1,
									p_local_time->tm_mday,
									p_local_time->tm_hour,
									p_local_time->tm_min);

	string time_str = fox::TimeConvert(m_war_list[i].start_time);
	LOG2("[%s] Next CastleWarTime is [%s]\n", m_war_list[i].feod_name.c_str(), time_str.c_str());

	//--------------log info end ------------------------//
}

void CRegionCastleWarMgr::PauseWar(string feod_name, DWORD winner_id)
{
	//遍历当前所有采邑
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(m_war_list[i].feod_name == feod_name) break;
	}

	if(i >= FEOD_COUNT) return;

	//令旗相关
	ClearAllFlagNpc(i);

	if(m_war_list[i].master_union_id != 0)
	{
		UpdateCreWarInfo("", m_war_list[i].master_union_id, (BYTE)0);
	}

	for(int cnt = 0; cnt < (int)m_war_list[i].v_attacker_union_info.size(); cnt++)
	{
		UpdateCreWarInfo("", m_war_list[i].v_attacker_union_info[cnt].union_id,(BYTE)0);
	}

	for(int cnt = 0; cnt < (int)m_war_list[i].v_defender_union_info.size(); cnt++)
	{
		UpdateCreWarInfo("", m_war_list[i].v_defender_union_info[cnt].union_id,(BYTE)0);
	}

	m_war_list[i].v_attacker_union_info.clear();
	m_war_list[i].v_defender_union_info.clear();

	if(winner_id == 0)
	{				
		m_war_list[i].master_union_id	= 0;
		m_war_list[i].master_union_name	= "";
		m_war_list[i].master_union_logo	= "";
		m_war_list[i].vassal_name		= "";
		m_war_list[i].flag_id			= 0;
		m_war_list[i].state				= NPC_OCCUPY_STATE;	

		if(!KillCastleGuardNpc(feod_name))
		{
			//...
		}

		//消息建构并发送
		CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_pause(feod_name, m_war_list[i].master_union_name);
		
	}else
	{
		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union;
		iter_union = g_region->m_unionMap.find(winner_id);
		if(iter_union == g_region->m_unionMap.end())
		{
			return;
		}

		SUnionData union_data = iter_union->second;

		SUnionCitizen* p_citizen = union_data.Find(union_data.m_nCaptainID);

		UpdateCreWarInfo(feod_name, winner_id, (BYTE)0);

		if(p_citizen != NULL)
		{
			m_war_list[i].start_time		= m_current_time + PREPARE_TIME * 60;
			m_war_list[i].master_union_id	= winner_id;
			m_war_list[i].master_union_name	= union_data.m_strName;
// 			m_war_list[i].master_union_logo	= union_data.icon;
			m_war_list[i].vassal_name		= p_citizen->m_strPlayerName;
			m_war_list[i].flag_id			= 0;
			m_war_list[i].state				= WAR_FIVE_MIN_PAUSE_STATE;
		}

		if(!KillCastleGuardNpc(feod_name))
		{
			//...
		}

		//消息建构并发送
		CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_pause(feod_name, m_war_list[i].master_union_name);

	}

	//传送不相关人员
	if(!TransUser(m_war_list[i].feod_name))
	{
		//...
	}
	
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(&m_war_list[i]);

	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(&m_war_list[i]);

	g_region->BroadcastCmd(g_sendCmd, NULL);

	//--------------log info start-----------------------//
	ST_TIME*	p_local_time;	//当前时间
	TIME		tmp;

	time(&tmp);
	p_local_time = localtime((TIME *)(&tmp));
	LOG3("[%s] CastleWar Paused at  [%d]-[%d]-[%d]  [%d]:[%d]\n",
		feod_name.c_str(),
		p_local_time->tm_year + 1900,
		p_local_time->tm_mon + 1,
		p_local_time->tm_mday,
		p_local_time->tm_hour,
		p_local_time->tm_min);
	//--------------log info end ------------------------//

}

void CRegionCastleWarMgr::OnTimer()
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	CMonCastle *m_mc = CMonCastle::GetMCInstance();
	if (m_mc->GetState() != 0)
	{
		return;
	}
	//取得当前时间
	TIME tmp = 0;	
	time(&tmp);	

	m_current_time = (DWORD)tmp;

	//遍历当前所有采邑
	for(int i = 0; i < FEOD_COUNT; i++)
	{		
		//------------------------//
		//if(i != m_flag) continue;		// 郑泽华---09.11.2
		//------------------------//

		switch(m_war_list[i].state)
		{		
		//当前正交战准备中
		case WAR_PREPARE_STATE:
		case WAR_FIVE_MIN_PAUSE_STATE:	
			if(m_war_list[i].end_time <= m_current_time)
			{
				EndWar(m_war_list[i].feod_name, m_war_list[i].master_union_id);

				break;
			}
			
			if(m_war_list[i].start_time <= m_current_time)
			{
				if(m_war_list[i].v_attacker_union_info.empty())
				{
					m_war_list[i].start_time = m_current_time + PREPARE_TIME * 60;

					break;
				}
				
				StartWar(m_war_list[i].feod_name);

				break;
			}

			CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_prepare(m_war_list[i].feod_name, m_war_list[i].master_union_name);
			
			break;

		//当前正交战中
		case WARFARE_STATE:	
			if(m_war_list[i].end_time <= m_current_time)
			{
				EndWar(m_war_list[i].feod_name, m_war_list[i].master_union_id);
			}
			break;
		
		//当前NPC或者诸侯国占领
		case NPC_OCCUPY_STATE:
		case UNION_OCCUPY_STATE:
			//当前无人注册攻城
			if(m_war_list[i].v_attacker_union_info.empty())
			{
				//攻城时间已过
				if(m_war_list[i].end_time <= m_current_time)
				{					
					GetCastleWarDefaultStartTime(m_war_list[i].start_time, i);

					m_war_list[i].end_time = m_war_list[i].start_time + m_duration[i];

					m_war_list[i].last_end_time = m_war_list[i].end_time;
				}

				break;
			}

			if(m_war_list[i].end_time <= m_current_time)
			{
				EndWar(m_war_list[i].feod_name, m_war_list[i].master_union_id);

				break;
			}

			if(m_war_list[i].start_time <= m_current_time)
			{
				m_war_list[i].start_time = m_current_time;

				m_war_list[i].state = WAR_PREPARE_STATE;
			}else if((m_war_list[i].start_time - m_current_time) <= PREPARE_TIME * 60) 
			{
				m_war_list[i].state = WAR_PREPARE_STATE;					
				
				CCastleWarCmdSend_R2G::Send_r2g_castlewar_war_prepare(m_war_list[i].feod_name, m_war_list[i].master_union_name);
			}
			break;
		default:
			break;
		}		
	}
	
}

bool CRegionCastleWarMgr::IsCastleWarTime(string feod_name)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */

	STWarInfo* p_war_info = FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return false;
	}

	if(p_war_info->state != WARFARE_STATE)
	{
		return false;
	}

	return true;
}

bool CRegionCastleWarMgr::IsInTimeZone(std::string feod_name)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */

	STWarInfo* p_war_info = FindByName(feod_name);

	if(p_war_info == NULL)
	{
		return false;
	}

	if((p_war_info->state != WARFARE_STATE) && (p_war_info->state != WAR_FIVE_MIN_PAUSE_STATE))
	{
		return false;
	}

	return true;
}

bool CRegionCastleWarMgr::IsCastleWarTime(CRegionCreature* p_cre)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */

	if(p_cre == NULL) return false;
	
	std::list<RtsSceneBlockRtb::SArea*>::iterator it;

	//如果处在城战区域内
	for(it = p_cre->m_listArea.begin(); it != p_cre->m_listArea.end(); it++)
	{	
		for(int i = 0; i < FEOD_COUNT; i++)
		{	
			if(strncmp((*it)->szName, (char*)CastleWarAreaName[i].c_str(), strlen("西岐")) == 0)			
			{
				if(m_war_list[i].state == WARFARE_STATE)
				{
#ifdef _DEBUG
					LOG2("[In IsCastleWarTime] : This is CastleWar Area : [%d] - [%s]\n", i, CastleWarAreaName[i].c_str());
#endif //_DEBUG					
					return true;
				}
			}
		}
	}

	//如果不在城战区域内，但是属于攻守方
	if(p_cre->m_unionID != 0)
	{
		STWarInfo* p_war_info = FindByUnionID(p_cre->m_unionID);

		if(p_war_info != NULL)
		{
			if(p_war_info->state == WARFARE_STATE)
			{				
				return true;
			}
		}
	}

	return false;
}

bool CRegionCastleWarMgr::InCastleWarArea(CRegionCreature* p_cre)
{
	if(p_cre == NULL) return false;

	RtsSceneBlockRtb* bmap = g_region->m_defScene->FindBlockByPos(p_cre->m_pos);
	std::string map;
	if(bmap)
	{
		map = bmap->m_szFileName;
	}else
    {
	    map = "scene01_001.rtb";
    }

	std::list<RtsSceneBlockRtb::SArea*>::iterator it;

	if(p_cre->m_core.Faction == FACTION_ZHOU)
	{
		for(int i = 0; i < NUM(SceneNameZhou); i++)
		{			
			if(strncmp((char*)map.c_str(), (char*)SceneNameZhou[i].c_str(), strlen((char*)SceneNameZhou[i].c_str())) == 0)			
			{			
				return true;					
			}
		}
	}else
	{
		for(int i = 0; i < NUM(SceneNameShang); i++)
		{			
			if(strncmp((char*)map.c_str(), (char*)SceneNameShang[i].c_str(), strlen((char*)SceneNameShang[i].c_str())) == 0)			
			{			
				return true;					
			}
		}
	}

	return false;
}

STWarInfo* CRegionCastleWarMgr::FindByName(string name)
{	
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return NULL;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	for(int i = 0; i < (int)m_war_list.size(); i++)
	{
		if(m_war_list[i].feod_name == name)
		{
			return &m_war_list[i];
		}
	}

	return NULL;	
}

//------------------------------------------------------------------------
//-根据指定的诸侯国id查找返回其相关的采邑信息
//-b_side : 默认NULL，用于只查寻该诸侯国id是否与某采邑相关
//-			攻城方true  
//-			守城方false
//-			union_id为未被批准的守城方函数返回NULL（区别FindAllByUnionID）
//------------------------------------------------------------------------
STWarInfo* CRegionCastleWarMgr::FindByUnionID(DWORD union_id, bool* b_side)
{
	
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return NULL;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */

	if(union_id == 0) return NULL;

	for(int i = 0; i < (int)m_war_list.size(); i++)
	{
		if(m_war_list[i].master_union_id == union_id)
		{
			//该诸侯是守城方
			if(b_side != NULL)
			{
				(*b_side) = false;
			}
			return &m_war_list[i];
		}
		
		//在攻城方中查找
		vector<STAttackerInfo>::iterator iter_attack =  m_war_list[i].v_attacker_union_info.begin();

		for(; iter_attack !=  m_war_list[i].v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).union_id == union_id)
			{
				//该诸侯是攻城方
				if(b_side != NULL)
				{
					(*b_side) = true;
				}
				return &m_war_list[i];
			}
		}

		//在守城方中查找
		vector<STDefenderInfo>::iterator iter_defend =  m_war_list[i].v_defender_union_info.begin();

		for(; iter_defend !=  m_war_list[i].v_defender_union_info.end(); iter_defend++)
		{
			if(((*iter_defend).union_id == union_id) && ((*iter_defend).is_allowed == true))
			{
				//该诸侯是守城方
				if(b_side != NULL)
				{
					(*b_side) = false;
				}
				return &m_war_list[i];
			}
		}

	}

	if(b_side != NULL)
	{
		(*b_side) = false;
	}
	
	return NULL;
}

STWarInfo* CRegionCastleWarMgr::FindByFlagID(DWORD flag_id, DWORD* union_id, bool* b_side)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return NULL;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(flag_id == 0)
	{
		return NULL;
	}

	for(int i = 0; i < (int)m_war_list.size(); i++)
	{
		if(m_war_list[i].flag_id == flag_id)
		{
			if(union_id != NULL)	(*union_id) = m_war_list[i].master_union_id;
			if(b_side != NULL)		(*b_side)	= false;

			return &m_war_list[i];
		}

		//在攻城方中查找
		vector<STAttackerInfo>::iterator iter_attack =  m_war_list[i].v_attacker_union_info.begin();

		for(; iter_attack !=  m_war_list[i].v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).flag_id == flag_id)
			{
				if(union_id != NULL)	(*union_id) = (*iter_attack).union_id;
				if(b_side != NULL)		(*b_side)	= true;

				return &m_war_list[i];
			}
		}

		//在守城方中查找

		//城主
		if(m_war_list[i].flag_id == flag_id)
		{
			if(union_id != NULL)	(*union_id) = m_war_list[i].master_union_id;
			if(b_side != NULL)		(*b_side)	= false;

			return &m_war_list[i];
		}

		vector<STDefenderInfo>::iterator iter_defend =  m_war_list[i].v_defender_union_info.begin();

		for(; iter_defend !=  m_war_list[i].v_defender_union_info.end(); iter_defend++)
		{
			if((*iter_defend).flag_id == flag_id)
			{
				if(union_id != NULL)	(*union_id) = (*iter_defend).union_id;
				if(b_side != NULL)		(*b_side)	= false;

				return &m_war_list[i];
			}
		}

	}

	return NULL;

}

bool CRegionCastleWarMgr::UpdateMasterInfo(string name, DWORD union_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	STWarInfo*	p_war_info = FindByName(name);
	
	if(p_war_info == NULL)
	{
		return false;
	}

	if(union_id == 0)
	{
		p_war_info->master_union_id		= 0;
		p_war_info->master_union_name	= "";
		p_war_info->master_union_logo	= "";
		p_war_info->vassal_name			= "";
		p_war_info->state				= NPC_OCCUPY_STATE;
	}else
	{

		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
		iter_union = g_region->m_unionMap.find(union_id);

		if(iter_union == g_region->m_unionMap.end())
		{
			return false;
		}

		CRegionUser* pUser = g_region->FindUser(iter_union->second.m_nCaptainID);

		if(pUser == NULL || pUser->m_dummy==NULL)
		{
			return false;
		}
		CRegionCreature* p_cre = pUser->m_dummy;
		
		p_war_info->master_union_id		= union_id;
		p_war_info->master_union_name	= iter_union->second.m_strName;
// 		p_war_info->master_union_logo	= iter_union->second.icon;
		p_war_info->vassal_name			= p_cre->m_userInfo->m_username;
		p_war_info->state				= UNION_OCCUPY_STATE;
	}
	
	return true;
}

bool CRegionCastleWarMgr::UpdateState(string name, EState state)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	STWarInfo*	p_war_info = FindByName(name);

	if(p_war_info == NULL)
	{
		return false;
	}	

	p_war_info->state = state;

	return true;
}

bool CRegionCastleWarMgr::UpdateCreWarInfo(string name, DWORD union_id, BYTE is_attacker)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(union_id == 0) return false;

	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
	iter_union = g_region->m_unionMap.find(union_id);

	if(iter_union == g_region->m_unionMap.end())
	{
		return false;
	}

	EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
	for (; iterUser!=g_region->m_userMap.end(); iterUser++)
	{
		CRegionUser* user = (*iterUser).second;
		CRegionCreature* tmp;
		if(!user->Session())	continue;
		tmp = user->m_dummy;
		if(!tmp)						continue;
		if(!tmp->m_scene)				continue;
		if(tmp->m_unionID != union_id)	continue; 

		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_user_data_chg(name, is_attacker);

		tmp->RecvCmd(g_sendCmd);

		tmp->m_feod_name = name;
		tmp->m_is_attacker = is_attacker;
		tmp->m_modelRef++;
	}

	return true;
}

bool CRegionCastleWarMgr::UpdateAttackerInfo(string name, DWORD union_id, BYTE op)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	STWarInfo*	p_war_info = FindByName(name);

	if(p_war_info == NULL)
	{
		return false;
	}	

	vector<STAttackerInfo>::iterator iter_attack;

	//添加操作
	if(op == ADD)
	{		
		if(union_id == p_war_info->master_union_id)
		{
			return false;
		}
		
		//查找有无重复
		iter_attack =  p_war_info->v_attacker_union_info.begin();
		
		for(; iter_attack != p_war_info->v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).union_id == union_id)
			{
				return false;
			}
		}

		//查找有无在另一方注册
		vector<STDefenderInfo>::iterator iter_defend = p_war_info->v_defender_union_info.begin();
		
		for(; iter_defend != p_war_info->v_defender_union_info.end(); iter_defend++)
		{
			if((*iter_defend).union_id == union_id)
			{
				return false;
			}
		}

		//诸侯国相关信息查询
		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
		iter_union = g_region->m_unionMap.find(union_id);

		if(iter_union == g_region->m_unionMap.end())
		{
			return false;
		}

		CRegionUser* pUser = g_region->FindUser(iter_union->second.m_nCaptainID);

		if(pUser == NULL || pUser->m_dummy==NULL)
		{
			return false;
		}
		CRegionCreature* p_cre = pUser->m_dummy;

		//添加
		STAttackerInfo	attack_info;

		attack_info.union_id	= union_id;
		attack_info.union_name	= iter_union->second.m_strName;
		attack_info.vassal_name	= p_cre->m_userInfo->m_username;

		p_war_info->v_attacker_union_info.push_back(attack_info);

		//UpdateCreWarInfo(p_war_info->feod_name, union_id, (BYTE)1);

	}else if(op == MINUS) //删除操作
	{		
		//查找有无目标
		iter_attack =  p_war_info->v_attacker_union_info.begin();
		
		for(; iter_attack != p_war_info->v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).union_id == union_id)
			{
				p_war_info->v_attacker_union_info.erase(iter_attack);
				
				return true;
			}
		}

		if(iter_attack == p_war_info->v_attacker_union_info.end())
		{
			return false;
		}

	}else
	{
		return false;
	}

	return true;
}

void CRegionCastleWarMgr::ReSetAllUserWarInfo(STWarInfo* p_war_info)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_war_info == NULL) return;
	
	if(p_war_info->master_union_id != 0)
	{
		UpdateCreWarInfo("", p_war_info->master_union_id, (BYTE)0);
	}

	for(int cnt = 0; cnt < (int)p_war_info->v_attacker_union_info.size(); cnt++)
	{
		UpdateCreWarInfo("", p_war_info->v_attacker_union_info[cnt].union_id, (BYTE)0);
	}

	for(int cnt = 0; cnt < (int)p_war_info->v_defender_union_info.size(); cnt++)
	{
		UpdateCreWarInfo("", p_war_info->v_defender_union_info[cnt].union_id, (BYTE)0);
	}
}

bool CRegionCastleWarMgr::UpdateDefenderInfo(string name, DWORD union_id, BYTE op)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	STWarInfo*	p_war_info = FindByName(name);

	if(p_war_info == NULL)
	{
		return false;
	}

	vector<STDefenderInfo>::iterator iter_definfo;

	switch((int)op)
	{
	//增加守城方
	case ADD:
		{
			if(union_id == p_war_info->master_union_id)
			{
				return false;
			}
			
			iter_definfo = p_war_info->v_defender_union_info.begin();

			for(; iter_definfo != p_war_info->v_defender_union_info.end(); iter_definfo++)
			{
				if((*iter_definfo).union_id == union_id)
				{
					return false;
				}
			}

			//查找有无在另一方注册
			vector<STAttackerInfo>::iterator iter_attack = p_war_info->v_attacker_union_info.begin();
			for(; iter_attack != p_war_info->v_attacker_union_info.end(); iter_attack++)
			{
				if((*iter_attack).union_id == union_id)
				{
					return false;
				}
			}

			//诸侯国相关信息查询
			EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
			iter_union = g_region->m_unionMap.find(union_id);

			if(iter_union == g_region->m_unionMap.end())
			{
				return false;
			}

			CRegionUser* pUser = g_region->FindUser(iter_union->second.m_nCaptainID);

			if(pUser == NULL || pUser->m_dummy==NULL)
			{
				return false;
			}
			CRegionCreature* p_cre = pUser->m_dummy;

			STDefenderInfo	def_info;

			def_info.union_id		= union_id;
			def_info.union_name		= iter_union->second.m_strName;
			def_info.vassal_name	= p_cre->m_userInfo->m_username;
			def_info.is_allowed		= false;

			p_war_info->v_defender_union_info.push_back(def_info);

		}
		break;

	//减少守城方
	case MINUS:
		{
			iter_definfo = p_war_info->v_defender_union_info.begin();

			for(; iter_definfo != p_war_info->v_defender_union_info.end(); iter_definfo++)
			{
				if((*iter_definfo).union_id == union_id)
				{
					p_war_info->v_defender_union_info.erase(iter_definfo);
					return true;
				}
			}

			if(iter_definfo == p_war_info->v_defender_union_info.end())
			{
				return false;
			}
		}
		break;
	
	//守城诸侯批准同盟
	case ALLOWED:
		{
			int count	 = 0;	//守城同盟不超过8个
			iter_definfo = p_war_info->v_defender_union_info.begin();

			for(; iter_definfo != p_war_info->v_defender_union_info.end(); iter_definfo++)
			{
				if((*iter_definfo).is_allowed == true)
				{
					count++;
				}

				//如果已经有8个，不能再批准
				if(count > 7)
				{
					return false;
				}

				if((*iter_definfo).union_id == union_id)
				{
					if((*iter_definfo).is_allowed == true)
					{
						return false;
					}

					(*iter_definfo).is_allowed = true;
					
					break;
				}
			}

			if(iter_definfo == p_war_info->v_defender_union_info.end())
			{
				return false;
			}
		}
		break;
	
	//守城诸侯解除同盟
	case NOTALLOWED:
		{
			vector<STDefenderInfo>::iterator iter_definfo = p_war_info->v_defender_union_info.begin();

			for(; iter_definfo != p_war_info->v_defender_union_info.end(); iter_definfo++)
			{
				if((*iter_definfo).union_id == union_id)
				{
					if((*iter_definfo).is_allowed == false)
					{
						return false;
					}

					(*iter_definfo).is_allowed = false;

					return true;
				}
			}

			if(iter_definfo == p_war_info->v_defender_union_info.end())
			{
				return false;
			}
		}
		break;

	default:
		return false;
	}
	
	return true;
}

bool CRegionCastleWarMgr::CanInsertFlag(CRegionCreature* p_sender_cre)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_sender_cre == NULL) return false;
	
	DWORD dummy;
	if(FindFlagByUnionID(p_sender_cre->m_unionID, dummy))
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_insert_flag_rep(ERR_MULTI_FLAG);

		p_sender_cre->RecvCmd(g_sendCmd);

		LOG1("Union [%d] have had flag\n", p_sender_cre->m_unionID);

		return false;
	}
	
	if(FindHaveFlagAround(p_sender_cre)) 
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_insert_flag_rep(ERR_HAVE_FLAG_ROUND);

		p_sender_cre->RecvCmd(g_sendCmd);

		return false;
	}
	
	STWarInfo* p_war_info = g_RegionCastleWarMgr.FindByUnionID(p_sender_cre->m_unionID);

	if(p_war_info == NULL)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_insert_flag_rep(ERR_UNKNOWN_FLAG);

		p_sender_cre->RecvCmd(g_sendCmd);

		LOG1("Cannot find union [%d]\n", p_sender_cre->m_unionID);
		
		return false;
	}
	
	if(p_war_info->state != WARFARE_STATE)
	{
		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_insert_flag_rep(ERR_TIME_WRONG);

		p_sender_cre->RecvCmd(g_sendCmd);
		
		return false;
	}

	return true;
}

void CRegionCastleWarMgr::OnUnionLeave(DWORD union_id)
{
	if(union_id == 0)
	{
		return;
	}

	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
	iter_union = g_region->m_unionMap.find(union_id);

	if(iter_union == g_region->m_unionMap.end())
	{
		return;
	}

	CRegionUser* pUser = g_region->FindUser(iter_union->second.m_nCaptainID);
	if(pUser == NULL || pUser->m_dummy==NULL)
	{
		return;
	}
	CRegionCreature* p_cre = pUser->m_dummy;

	if(!p_cre) return;

	for(int i = 0; i < (int)m_war_list.size(); i++)
	{
		if(m_war_list[i].master_union_id == union_id)
		{
			//该诸侯是守城方
			m_war_list[i].vassal_name = p_cre->m_core.Name;
			return;
		}

		//在攻城方中查找
		vector<STAttackerInfo>::iterator iter_attack =  m_war_list[i].v_attacker_union_info.begin();

		for(; iter_attack !=  m_war_list[i].v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).union_id == union_id)
			{
				//该诸侯是攻城方
				(*iter_attack).vassal_name = p_cre->m_core.Name;
				return ;
			}
		}

		//在守城方中查找
		vector<STDefenderInfo>::iterator iter_defend =  m_war_list[i].v_defender_union_info.begin();

		for(; iter_defend !=  m_war_list[i].v_defender_union_info.end(); iter_defend++)
		{
			if(((*iter_defend).union_id == union_id) && ((*iter_defend).is_allowed == true))
			{
				//该诸侯是守城方
				(*iter_defend).vassal_name = p_cre->m_core.Name;
				return ;
			}
		}

	}
}

bool CRegionCastleWarMgr::FindHaveFlagAround(CRegionCreature* p_sender_cre)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_sender_cre->m_unionID == 0)
	{
		return false;
	}
	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(p_sender_cre->m_unionID);
	if(iter_union == g_region->m_unionMap.end())
	{
		return false;
	} 
	if(iter_union->second.m_nCaptainID != p_sender_cre->m_userInfo->m_userId)
	{
		return false;
	}

	//取得周围的npc，查找周围有无令旗
	std::vector<CRegionCreature*> creList;
	p_sender_cre->m_scene->GetAroundCreatureByGrid(p_sender_cre->m_pos, 
											 FLAG_RANGE,
											 creList,
										 	 p_sender_cre );

	if(!creList.empty())
	{
		std::vector<CRegionCreature*>::iterator iter = creList.begin();

		for(; iter != creList.end(); iter++)
		{
			if(*iter == p_sender_cre) continue;

			//周围一定范围内有令旗
			if (((*iter)->m_npcInfo != NULL) && (IsFlagNpc((*iter)->m_npcInfo->Id)))
			{
				CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_insert_flag_rep(ERR_HAVE_FLAG_ROUND);

				p_sender_cre->RecvCmd(g_sendCmd);

				return true;
			}
		}
	}

	return false;
}

bool CRegionCastleWarMgr::FindFlagByUnionID(DWORD union_id, DWORD& ret)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(union_id == 0)
	{
		ret = 0;
		return false;
	}

	STWarInfo* p_war_info = FindByUnionID(union_id);

	if(p_war_info == NULL)
	{
		ret = 0;
		return false;
	}

	if(p_war_info->master_union_id == union_id)
	{
		ret = p_war_info->flag_id;

		return (p_war_info->flag_id == 0) ? false : true;
	}
	
	vector<STAttackerInfo>::iterator iter_attack = p_war_info->v_attacker_union_info.begin();

	for(; iter_attack != p_war_info->v_attacker_union_info.end(); iter_attack++)
	{
		if(((*iter_attack).union_id == union_id) &&
			((*iter_attack).flag_id != 0))
		{
			ret = (*iter_attack).flag_id;
			return true;
		}
	}

	vector<STDefenderInfo>::iterator iter_defend = p_war_info->v_defender_union_info.begin();

	for(; iter_defend != p_war_info->v_defender_union_info.end(); iter_defend++)
	{
		if(((*iter_defend).union_id == union_id) &&
			((*iter_defend).flag_id != 0))
		{
			ret = (*iter_defend).flag_id;
			return true;
		}
	}

	ret = 0;
	return false;
}

STWarInfo* CRegionCastleWarMgr::FindAllByUnionID(DWORD union_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return NULL;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(union_id == 0) return NULL;

	int i = 0;
	for(i = 0; i < (int)m_war_list.size(); i++)
	{
		if(m_war_list[i].master_union_id == union_id)
		{			
			return &m_war_list[i];
		}

		//在攻城方中查找
		vector<STAttackerInfo>::iterator iter_attack =  m_war_list[i].v_attacker_union_info.begin();

		for(; iter_attack !=  m_war_list[i].v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).union_id == union_id)
			{				
				return &m_war_list[i];
			}
		}

		//在守城方中查找
		vector<STDefenderInfo>::iterator iter_defend =  m_war_list[i].v_defender_union_info.begin();

		for(; iter_defend !=  m_war_list[i].v_defender_union_info.end(); iter_defend++)
		{
			if((*iter_defend).union_id == union_id)
			{
				return &m_war_list[i];
			}
		}
	}

	return NULL;
}

void CRegionCastleWarMgr::OnUserOnline(CRegionCreature* p_cre)
{
	if(p_cre == NULL) return;

	if(p_cre->m_userInfo == NULL) return;

	OnUserOnline(p_cre->m_userInfo->m_userId, false);
}

void CRegionCastleWarMgr::OnUserOnline(DWORD user_db_id, bool b_flag)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	CRegionUser* p_user = g_region->FindUser(user_db_id);

	if((p_user == NULL) || p_user->m_dummy == NULL)
	{
		return;
	}
	
	CRegionCreature*	p_cre	 = p_user->m_dummy;
	DWORD				union_id = p_cre->m_unionID;	
	bool				b_side	 = false;
	bool				b_transported = false;

	if(b_flag)
	{
		//通知城战开始
		for(int i = 0; i < (int)m_war_list.size(); i++)
		{
			if(m_war_list[i].state == WARFARE_STATE)
			{
				CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_start(m_war_list[i].feod_name);

				p_cre->RecvCmd(g_sendCmd);
			}
		}

		//城战时期
		if(IsCastleWarTime(p_cre))
		{				
			std::list<RtsSceneBlockRtb::SArea*>::iterator it;

			for(it = p_cre->m_listArea.begin(); it != p_cre->m_listArea.end(); it++)
			{	
				for(int i = 0; i < FEOD_COUNT; i++)
				{
					if(strncmp((*it)->szName, (char*)CastleWarAreaName[i].c_str(), strlen("西岐")) == 0)
					{
						if(m_war_list[i].state == WARFARE_STATE)
						{														
							//传送玩家
							if(TransUser(p_cre))
							{
								//消息建构通知玩家
								//...
								CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_is_transported();

								p_cre->RecvCmd(g_sendCmd);

								b_transported = true;

							}
							
							break;
						}
					}
				}

				if(b_transported == true)
				{
					break;
				}
			}		
		}
	}

	std::string	name;
	BYTE		is_attacker;
	
	STWarInfo* p_war_info = FindByUnionID(union_id, &b_side);
	
	if(p_war_info == NULL)
	{
		name = "";
		is_attacker = (BYTE)0;
	}else
	{
		name = p_war_info->feod_name;
		
		if(b_side == true)
		{
			is_attacker = (BYTE)1;
		}else
		{
			is_attacker = (BYTE)0;
		}

		CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_refresh(p_war_info);

		p_cre->RecvCmd(g_sendCmd);
		
	}

	//通知别人
	p_cre->m_feod_name = name;

	p_cre->m_is_attacker = is_attacker;

	p_cre->m_modelRef++;

	//通知自己
	CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_user_data_chg(name, is_attacker);

	p_cre->RecvCmd(g_sendCmd);

}

void CRegionCastleWarMgr::OnUnionDisband(DWORD union_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(union_id == 0)
	{
		return;
	}

	STWarInfo* p_war_info = FindAllByUnionID(union_id);

	if(p_war_info == NULL)
	{
		return;
	}
	
	if(p_war_info->master_union_id == union_id)
	{
		p_war_info->master_union_id = 0;
		p_war_info->flag_id			= 0;
		p_war_info->master_union_logo	= "";
		p_war_info->master_union_name	= "";
		p_war_info->vassal_name			= "";		
	}

	bool b_side = false;

	FindByUnionID(union_id, &b_side);

	//攻城方
	if(b_side == true)
	{
		UpdateAttackerInfo(p_war_info->feod_name, union_id, MINUS);
	}else
	{
		UpdateDefenderInfo(p_war_info->feod_name, union_id, MINUS);
	}

	CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

	//删除令旗道具
	EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
	iter_union = g_region->m_unionMap.find(union_id);

	if(iter_union == g_region->m_unionMap.end())
	{
		return;
	}

	CRegionUser* pUser = g_region->FindUser(iter_union->second.m_nCaptainID);
	if(pUser == NULL || pUser->m_dummy==NULL)
	{
		return;
	}
	CRegionCreature* p_cre = pUser->m_dummy;

	int nItemCount, page, gridI, gridJ;
	bool bFindItem = p_cre->m_pItemContainer->m_Bag.FindType(ITEM_FLAG_ID, ItemColor_All, &nItemCount, &page, &gridI, &gridJ);
	if (bFindItem)
	{
		p_cre->m_pItemContainer->m_Bag.RemoveItem(page, gridI, gridJ);
		CItemCmdBuilder_Svr::Build_r2c_bag_removeitem(ITEM_CONTAINER_PLAYER_COMMON_BAG, page, gridI, gridJ,ITEM_REASON_DELETE);
		p_cre->RecvCmd(g_sendCmd);
	}
}

bool CRegionCastleWarMgr::AddFlag(DWORD union_id, DWORD flag_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(flag_id == 0) return false;

	STWarInfo* p_war_info = FindByUnionID(union_id);

	if(p_war_info == NULL)
	{
		return false;
	}

	//城主
	if((p_war_info->master_union_id == union_id) &&
	   (p_war_info->flag_id == 0))
	{
		p_war_info->flag_id = flag_id;

		CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

		return true;
	}

	vector<STAttackerInfo>::iterator iter_attack = p_war_info->v_attacker_union_info.begin();
		
	for(; iter_attack != p_war_info->v_attacker_union_info.end(); iter_attack++)
	{
		if(((*iter_attack).union_id == union_id) &&
			((*iter_attack).flag_id == 0))
		{
			(*iter_attack).flag_id = flag_id;

			CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

			return true;
		}
	}

	vector<STDefenderInfo>::iterator iter_defend = p_war_info->v_defender_union_info.begin();

	for(; iter_defend != p_war_info->v_defender_union_info.end(); iter_defend++)
	{
		if(((*iter_defend).union_id == union_id) &&
			((*iter_defend).flag_id == 0))
		{
			(*iter_defend).flag_id = flag_id;

			CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

			return true;
		}
	}

	return false;
}

bool CRegionCastleWarMgr::DelFlag(DWORD flag_id, bool b_is_killed)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(flag_id == 0)
	{
		return false;
	}
	
	DWORD	union_id	= 0;
	DWORD	def_flag_id = 0;
	bool	b_side		= false;
	
	STWarInfo* p_war_info = FindByFlagID(flag_id, &union_id, &b_side);

	if(p_war_info == NULL)
	{
		return false;
	}

	def_flag_id = (b_is_killed == true) ? 0xffff : 0; 

	if(b_side == true)	//攻城方
	{
		vector<STAttackerInfo>::iterator iter_attack = p_war_info->v_attacker_union_info.begin();

		for(; iter_attack != p_war_info->v_attacker_union_info.end(); iter_attack++)
		{
			if((*iter_attack).union_id == union_id)
			{
				(*iter_attack).flag_id = def_flag_id;

				CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

				return true;
			}
		}
	}else	//守城方
	{
		if(p_war_info->master_union_id == union_id)	//城主
		{
			p_war_info->flag_id = def_flag_id;

			CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

			return true;
		}
		
		vector<STDefenderInfo>::iterator iter_defend = p_war_info->v_defender_union_info.begin();

		for(; iter_defend != p_war_info->v_defender_union_info.end(); iter_defend++)
		{
			if((*iter_defend).union_id == union_id)
			{
				(*iter_defend).flag_id = def_flag_id;

				CCastleWarCmdSend_R2G::Send_r2g_castlewar_update_data(p_war_info);

				return true;
			}
		}
	}

	return false;
}

bool CRegionCastleWarMgr::SetStartTime(string name, short week_day)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	STWarInfo*	p_war_info = FindByName(name);

	if(p_war_info == NULL)
	{
		return false;
	}

	int index = 0;

	for(index = 0; index < (int)m_war_list.size(); index++)
	{
		if(m_war_list[index].feod_name == name)
		{
			break;
		}
	}

	if(index >= (int)m_war_list.size())
	{
		return false;
	}

	if((week_day > 6) || (week_day < 0))
	{
		return false;
	}

	if((m_current_time - p_war_info->last_end_time) > 1 * 60 * 60)
	{
		return false;
	}

	ST_TIME*	p_local_time;
	DWORD		interval;	//时间间隔(second)

	//取得当前时间：年月日分秒
	p_local_time = localtime((TIME *)(&m_current_time));

	week_day = (week_day == 0) ? 7 : week_day;

	p_local_time->tm_wday = (p_local_time->tm_wday == 0) ? 7 : p_local_time->tm_wday;
		
	interval = ((week_day - p_local_time->tm_wday + 7) * 24 * 60 * 60) +		  //天数差
				((m_hour[index] - p_local_time->tm_hour) * 60 * 60) +			  //小时差
				((m_min[index] - p_local_time->tm_min) * 60) -					  //分钟差
				(p_local_time->tm_sec);											  //秒差
	
	p_war_info->start_time = m_current_time + interval;
	p_war_info->end_time = p_war_info->start_time + m_duration[index];
	
	CCastleWarCmdSend_R2G::Send_r2g_castlewar_time_set(name, p_war_info->start_time);

	return true;
}

//--------------------------------------------
//规则相关
//--------------------------------------------

//void CRegionCastleWarMgr::InformAllRegUserPrepare(int index)
//{
//	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
//#ifdef WAR_SWITCH
//	return;
//#endif //WAR_SWITCH
//	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
//	
//	if(m_war_list[index].master_union_id != 0)
//	{
//		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
//		iter_union = g_region->m_unionMap.find(m_war_list[index].master_union_id);
//
//		if(iter_union == g_region->m_unionMap.end())
//		{
//			return;
//		}
//
//		EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
//		for (; iterUser!=g_region->m_userMap.end(); iterUser++)
//		{
//			CRegionUser* user = (*iterUser).second;
//			CRegionCreature* tmp;
//			if(!user->m_session)	continue;
//			tmp = user->m_dummy;
//			if(!tmp)				continue;
//			if(!tmp->m_scene)		continue;
//			if(tmp->m_unionID!=m_war_list[index].master_union_id) continue; 
//
//			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_prepare(m_war_list[index].feod_name, m_war_list[index].master_union_name);
//
//			user->RecvCmd(g_sendCmd);
//		}
//	}
//
//	for(int cnt = 0; cnt < (int)m_war_list[index].v_attacker_union_info.size(); cnt++)
//	{
//		DWORD	union_id = m_war_list[index].v_attacker_union_info[cnt].union_id;
//
//		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
//		iter_union = g_region->m_unionMap.find(union_id);
//
//		if(iter_union == g_region->m_unionMap.end())
//		{
//			return;
//		}
//
//		EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
//		for (; iterUser!=g_region->m_userMap.end(); iterUser++)
//		{
//			CRegionUser* user = (*iterUser).second;
//			CRegionCreature* tmp;
//			if(!user->m_session)	continue;
//			tmp = user->m_dummy;
//			if(!tmp)						continue;
//			if(!tmp->m_scene)				continue;
//			if(tmp->m_unionID != union_id)	continue; 
//
//			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_prepare(m_war_list[index].feod_name, m_war_list[index].master_union_name);
//
//			user->RecvCmd(g_sendCmd);
//		}
//	}
//
//	for(int cnt = 0; cnt < (int)m_war_list[index].v_defender_union_info.size(); cnt++)
//	{
//		DWORD	union_id = m_war_list[index].v_defender_union_info[cnt].union_id;
//
//		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union; 
//		iter_union = g_region->m_unionMap.find(union_id);
//
//		if(iter_union == g_region->m_unionMap.end())
//		{
//			return;
//		}
//
//		EXT_SPACE::unordered_map<ULONG, CRegionUser*>::iterator iterUser = g_region->m_userMap.begin();
//		for (; iterUser!=g_region->m_userMap.end(); iterUser++)
//		{
//			CRegionUser* user = (*iterUser).second;
//			CRegionCreature* tmp;
//			if(!user->m_session)	continue;
//			tmp = user->m_dummy;
//			if(!tmp)				continue;
//			if(!tmp->m_scene)		continue;
//			if(tmp->m_unionID!=union_id) continue; 
//
//			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_prepare(m_war_list[index].feod_name, m_war_list[index].master_union_name);
//
//			user->RecvCmd(g_sendCmd);
//		}
//	}
//}
//void CRegionCastleWarMgr::InformAreaUserPrepare(int index)
//{
//	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
//#ifdef WAR_SWITCH
//	return;
//#endif //WAR_SWITCH
//	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
//	
//	TObjectHash& ob_hash = g_region->m_defScene->m_objectHash;
//	TObjectHash::iterator iter = ob_hash.begin();
//
//	for(; iter!=ob_hash.end(); iter++)
//	{
//		CRegionObject *object = (*iter).second;
//
//		//如果是生物
//		if(OB_TYPE(object) == OBJECT_TYPE_CREATURE)
//		{
//			CRegionCreature* p_cre = (CRegionCreature *)object;
//
//			//如果不是玩家
//			if(!p_cre->IsUser()) continue;
//
//			//查找玩家所处的所有区域
//			std::list<RtsSceneBlockRtb::SArea*>::iterator it;
//
//			for(it = p_cre->m_listArea.begin(); it != p_cre->m_listArea.end(); it++)
//			{									
//				if(strncmp((*it)->szName, CastleWarAreaName[index].c_str(), strlen((*it)->szName)) == 0)
//				{
//					break;
//				}								
//			}
//
//			//该玩家的确处在城战区域中
//			if(it == p_cre->m_listArea.end())
//			{
//				continue;
//			}
//
//			CCastleWarCmdBuilder_R2C::Build_r2c_castlewar_war_prepare(m_war_list[index].feod_name, m_war_list[index].master_union_name);
//
//			p_cre->RecvCmd(g_sendCmd);							
//		}
//	}
//}


bool CRegionCastleWarMgr::CanAttack(DWORD target_type, CRegionCreature* p_attacker, DWORD target_id)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return true;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_attacker == NULL) return false;

	DWORD attacker_union_id = p_attacker->m_unionID;

	if(p_attacker->IsUserPet() || p_attacker->IsCallNpc())
	{
		attacker_union_id = p_attacker->m_master->m_unionID;
	}

	STWarInfo* p_war_info = FindByUnionID(attacker_union_id);

	if(p_war_info != NULL)
	{
		if(p_war_info->state != WARFARE_STATE)
		{
			return false;
		}
	}

	switch(target_type)
	{
	//令旗
	case FLAG_NPC_ID_ZHOU:
	case FLAG_NPC_ID_SHANG:
		{
			DWORD	union_id;
			bool	flag_side;	//true 攻城方
			bool	union_side; //true 攻城方

			if(FindByFlagID(target_id, &union_id, &flag_side) == NULL)
			{
				return false;
			}
			
			//中立
			if(FindByUnionID(attacker_union_id, &union_side) == NULL)
			{
				return true;
			}

			if(flag_side == union_side)
			{
				return false;
			}else
			{
				return true;
			}
		}
		break;

	//城市守护兽
	case XIQI_ANIMAL_NPC_ID:
	case ZHAOGE_ANIMAL_NPC_ID:
	case CHUANYUN_ANIMAL_NPC_ID:
		{
			bool	union_side;		//true 攻城方
			DWORD	flag_id;

			if(!check_is_vassal(p_attacker))
			{
				return false;
			}
			
			//中立
			if(FindByUnionID(attacker_union_id, &union_side) == NULL)
			{
				return false;
			}

			if(union_side == false)	//守城方
			{
				return false;
			}else if((FindFlagByUnionID(attacker_union_id, flag_id) == true) &&
					 (flag_id != 0xffff))
			{
				return true;
			}else //攻城方令旗被打掉
			{
				return false;
			}
		}
	default:
		break;
	}

	return true;
}

bool CRegionCastleWarMgr::TransUser(CRegionCreature* p_cre)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_cre == NULL)
	{
		return false;
	}	
		
	DWORD	flag_id;
	
	//判断是否守城方
	bool b_side;
	STWarInfo* p_war_info = FindByUnionID(p_cre->m_unionID, &b_side);

	if((p_war_info != NULL) && (b_side == false))
	{
		return false;
	}

	//如果是攻城方
	if((p_war_info != NULL) && (b_side == true))
	{
		//找到令旗
		if(FindFlagByUnionID(p_cre->m_unionID, flag_id) == true)
		{
			CRegionCreature* p_flag = g_region->m_defScene->FindCreature(flag_id);

			if(p_flag != NULL)
			{
				p_cre->ForceSetPosition(p_flag->m_pos[0], p_flag->m_pos[1], 0);

				return true;
			}		
		}
	}
	
	//默认抛出坐标
	float fX, fY;
	int	  index = rand() % 3;

	if (p_cre->m_core.Faction==FACTION_ZHOU) // 周
	{
		fX = (float)m_rebirth_pos[0][index][0];
		fY = (float)m_rebirth_pos[0][index][1];
	}else // if (m_core.Faction==FACTION_SHANG) // 商
	{
		fX = (float)m_rebirth_pos[1][index][0];
		fY = (float)m_rebirth_pos[1][index][1];
	}

	p_cre->ForceSetPosition(fX, fY, 0);

	return true;
}
bool CRegionCastleWarMgr::TransUser(string feod_name)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	int i = 0;
	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(FeodNameInDB[i] == feod_name)
		{
			break;
		}
	}

	if(i >= FEOD_COUNT) return false;

	//查找城战区域所有玩家
	TObjectHash& ob_hash = g_region->m_defScene->m_objectHash;
	TObjectHash::iterator iter = ob_hash.begin();

	for(; iter!=ob_hash.end(); iter++)
	{
		CRegionObject *object = (*iter).second;

		//如果是生物
		if(OB_TYPE(object) == OBJECT_TYPE_CREATURE)
		{
			CRegionCreature* p_cre = (CRegionCreature *)object;

			//如果不是玩家
			if(!p_cre->IsUser()) continue;

			//判断是否守城方
			bool b_side;
			STWarInfo* p_war_info = FindByUnionID(p_cre->m_unionID, &b_side);

			if((p_war_info != NULL) && (b_side == false)) continue;
			
			std::list<RtsSceneBlockRtb::SArea*>::iterator it;

			for(it = p_cre->m_listArea.begin(); it != p_cre->m_listArea.end(); it++)
			{	
				int i = 0;
				for(i = 0; i < FEOD_COUNT; i++)
				{
					if(strncmp((*it)->szName, CastleWarAreaName[i].c_str(), strlen("西岐")) == 0)
					{
						if(m_war_list[i].state == WARFARE_STATE)
						{							
							break;
						}
					}
				}

				if(i < FEOD_COUNT)
				{
					break;
				}
			}

			if(it == p_cre->m_listArea.end())
			{
				continue;
			}

			//默认抛出坐标
			float fX, fY;
			int	  index = rand() % 3;

			if (p_cre->m_core.Faction==FACTION_ZHOU) // 周
			{
				fX = (float)m_rebirth_pos[0][index][0];
				fY = (float)m_rebirth_pos[0][index][1];
			}else // if (m_core.Faction==FACTION_SHANG) // 商
			{
				fX = (float)m_rebirth_pos[1][index][0];
				fY = (float)m_rebirth_pos[1][index][1];
			}
						
			p_cre->ForceSetPosition(fX, fY, 0);
		}
	}
    
	return true;
}

bool CRegionCastleWarMgr::Rebirth(CRegionCreature* p_cre)
{
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
#ifdef WAR_SWITCH
	return false;
#endif //WAR_SWITCH
	/* @@@@@@@@@@@@@@@@@@@@@@@@ */
	
	if(p_cre == NULL) return false;
	//PZH
	//if(p_cre->m_bDead == false) return false;
	//p_cre->m_bDead = false;
	if(!(p_cre->GetPlayerLogic()->IsLive()))
	{
		return false;
	}
	p_cre->GetPlayerLogic()->ReLive();
	//
	//重生
	
	//寻找本方令旗位置
	STWarInfo*	p_war_info = NULL;
	bool		union_side = false;
	DWORD		flag_id	   = 0;

	//p_war_info = FindByUnionID(p_cre->m_unionID, &union_side);

	//中立者
	/*if(FindByUnionID(p_cre->m_unionID, &union_side)	== NULL)
	{
		return false;
	}*/

	//找到令旗
	if(FindFlagByUnionID(p_cre->m_unionID, flag_id) == true)
	{
		CRegionCreature* p_flag = g_region->m_defScene->FindCreature(flag_id);

		if(p_flag != NULL)
		{
			/*int ix, iy;

			if (!p_cre->m_scene->m_pTerrain->GetTerrainTile(p_flag->m_pos[0], p_flag->m_pos[1], ix, iy))
				return false;

			int rx, ry;

			if (!p_cre->m_scene->m_pPathFind->FindSpace(ix, iy, rx, ry))
			{
				LOG2("rebirth: Can not find free space %d, %d\n", p_flag->m_pos[0], p_flag->m_pos[1]);
				return false;
			}

			float fx, fy;

			p_cre->m_scene->m_pTerrain->GetPosByTerrainXY(rx, ry, fx, fy);
			
			p_cre->ForceSetPosition(fx, fy, 0);*/

			SetPos(p_cre, p_flag->m_pos[0], p_flag->m_pos[1]);
			
			return true;
		}		
	}

	float fRebirthX, fRebirthY;
	int	  index = rand() % 3;

	if (p_cre->m_core.Faction==FACTION_ZHOU) // 周
	{
		fRebirthX = (float)m_rebirth_pos[0][index][0];
		fRebirthY = (float)m_rebirth_pos[0][index][1];
	}else // if (m_core.Faction==FACTION_SHANG) // 商
	{
		fRebirthX = (float)m_rebirth_pos[1][index][0];
		fRebirthY = (float)m_rebirth_pos[1][index][1];
	}

	/*int ix, iy;

	if (!p_cre->m_scene->m_pTerrain->GetTerrainTile(fRebirthX, fRebirthY, ix, iy))
		return false;

	int rx, ry;

	if (!p_cre->m_scene->m_pPathFind->FindSpace(ix, iy, rx, ry))
	{
		LOG2("rebirth: Can not find free space %d, %d\n", fRebirthX, fRebirthY);
		return false;
	}

	float fx, fy;

	p_cre->m_scene->m_pTerrain->GetPosByTerrainXY(rx, ry, fx, fy);

	p_cre->ForceSetPosition(fx, fy, 0);*/

	//p_cre->ForceSetPosition(fRebirthX, fRebirthY, 0);

	SetPos(p_cre, fRebirthX, fRebirthY);
	
	return true;
}

bool CRegionCastleWarMgr::SetPos(CRegionCreature* p_cre, long posx, long posy)
{
	if(p_cre == NULL) return false;

	int ix, iy;
	if (!p_cre->m_scene->m_pTerrain->GetTerrainTile(posx, posy, ix, iy))
		return false;

	ix += rand() % 40 - 20;
	iy += rand() % 40 - 20;

	int rx, ry;
	if (!p_cre->m_scene->m_pPathFind->FindSpace(p_cre->GetObjectId(), (short)p_cre->m_type, ix, iy, rx, ry, true))
	{
		LOG2("CRegionCastleWarMgr::SetPos: Can not find free space %d, %d\n", posx, posy);
		return false;
	}

	float fx, fy;
	p_cre->m_scene->m_pTerrain->GetPosByTerrainXY(rx, ry, fx, fy);
	p_cre->ForceSetPosition(fx, fy, 0);
	return true;
}

/*
bool CRegionCastleWarMgr::GetFeodIndexByAreaName(char* p_name)
{
	if(p_name == NULL) return false;

	int i = 0;

	for(i = 0; i < FEOD_COUNT; i++)
	{
		if(strncmp(p_name, CastleWarAreaName[i], strlen(p_name)) == 0)
		{

		}
	}
}
*/
