//---------------------------------------------------------------------------//
//created by tony 05.05.20
//---------------------------------------------------------------------------//
#include "region.h"
#include "region_guarder.h"
#include "region_ai.h"
#include "region_state_snapshot.h"

CRegionGuarderMgr g_GuarderMgr;

CGuarderLair::CGuarderLair()
: m_npc_id(0)
, m_max_count(0)
, m_cur_count(0)
, m_dir(0)
, m_home("")
{
	memset((void*)m_area_begin, 0, sizeof(m_area_begin));
	memset((void*)m_area_end, 0, sizeof(m_area_end));
}

void CGuarderLair::Init(short id, short max_cnt, long* area_begin, long* area_end, char dir, char* home)
{
	m_npc_id	= id;
	m_max_count	= max_cnt;
	m_dir		= dir;
	m_home		= home;

	memcpy((void *)m_area_begin, (void *)area_begin, sizeof(m_area_begin));
	memcpy((void *)m_area_end, (void *)area_end, sizeof(m_area_end));	
}

CRegionGuarderMgr::CRegionGuarderMgr()
{
	//...
	m_interval_factor = 1;
	m_max_counter	  = 0;
	m_curr_counter	  = 0;
}

void CRegionGuarderMgr::Init()
{

	const std::vector<SNpc> npc_table = g_TableNpc.GetNpcTable();
	int						      max = (int)npc_table.size();

	//从g_TableNpc中读取所有的守卫信息至g_GuarderMgr
	for(int	counter = 0; counter < max; counter++)
	{		
		if((npc_table[counter].AIType == "guarder") ||
		   (npc_table[counter].AIType == "patrol_guarder"))
		{
//--------------add start by tony 05.06.14----------------------------//
			if(npc_table[counter].Home == "")
			{
//--------------add end   by tony 05.06.14----------------------------//			
				SGInfo		temp;
				
				temp.id				 = npc_table[counter].Id;
				temp.isdead			 = false;
				temp.last_time_stamp = 0;
				temp.guarder_info	 = npc_table[counter];

				//重置重生周期
				temp.guarder_info.Interval *= 1000;
				temp.guarder_info.Interval *= m_interval_factor;

				g_GuarderMgr.m_guarder_info.push_back(temp);

				m_max_counter++;
			}
		} //end of if	
	} //end of for

	m_curr_counter = m_max_counter;
}

bool CRegionGuarderMgr::LoadCastleWarGuarderTbl(const char* file_name)
{
	RtCsv file;

	if (!file.LoadTableFile(file_name))
	{
		ERR1("Can not open table file %s\n", file_name);

		return false;
	}

	int titleLine = file.FindLineByString("ID");

	if (titleLine < 0)
	{
		ERR("Invalid castlewar table format, missing ID column\n");
		return false;
	}

	int numLines = file.GetLineCount();

	if (numLines <= 1)
	{
		ERR1("Invalid castlewar table format (lines=%d)\n", numLines);
		return false;
	}	

	for (int lineId = titleLine + 1; lineId < numLines; lineId++)
	{
		STCWarGuarderInfo tmp;
						
		int id = file[lineId]["守卫ID"].Int();

		SNpc* snpc = g_TableNpc.FindNpc(id);

		if (snpc == NULL)
		{
			LOG1("Can not find npc %d\n", id);

			continue;
		}

		tmp.o_id = 0;
		tmp.guarder_info = (*snpc);
		tmp.isdead = true;

		char* pos = (char*)file[lineId]["出生地点"].Str();
		char  delim[] = ";";
		char* k;

		if(pos)
		{
			k = strtok(pos, delim);
			
			if(k && strlen(k)>0)
			{
				for(int i = 0; k && (i < 2); i++)
				{
					tmp.pos[i] = atol(k);
					k = strtok(NULL, delim);
				}
			}

			tmp.pos[2] = 0;
		}
				
		char dir = (char)file[lineId]["初始方向"].Int();

		tmp.dir = dir;

		m_castlewar_guarder.push_back(tmp);
	}	

	return true;
}

SNpc* CRegionGuarderMgr::GetCastleWarGuarderById(short id)
{
	int count = 0;
	int max   = (int)m_castlewar_guarder.size();

	for(; count < max; count++)
	{
		if(m_castlewar_guarder[count].guarder_info.Id == id)
		{
			return &m_castlewar_guarder[count].guarder_info;
		}
	}

	//未找到
	if(count == max)
	{
#ifdef _DEBUG
		LOG1("Can't find the specified guarder ID=[%d]", id);
#endif //_DEBUG
	}

	return NULL;
}

SNpc* CRegionGuarderMgr::GetGuarderById(short id)
{
	int count = 0;
	int max   = (int)m_guarder_info.size();
	
	for(; count < max; count++)
	{
		if(m_guarder_info[count].guarder_info.Id == id)
		{
			return &m_guarder_info[count].guarder_info;
		}
	}

	//未找到
	if(count == max)
	{
#ifdef _DEBUG
		LOG1("Can't find the specified guarder ID=[%d]", id);
#endif //_DEBUG
	}

	return NULL;
}

void CRegionGuarderMgr::KillGuarder(short id)
{
	int count = 0;
	int	max   = (int)m_guarder_info.size();

	for(; count < max; count++)
	{
		if(m_guarder_info[count].id == id)
		{
			//守卫已死
			if(m_guarder_info[count].isdead == true)
			{
#ifdef _DEBUG
				LOG1("The guarder [%d] is already dead\n", id);
#endif //_DEBUG

				return;
			}

			m_guarder_info[count].isdead = true;

			m_curr_counter--;
		}
	}
	
}

//void CRegionGuarderMgr::KillAllCastleWarGuarder(std::string feod_name)
//{
//	int count = 0;
//
//	//for(count = 0; count < (int)m_info_tbl.size(); count++)
//	stlforeach(std::vector<STCWarGuarderInfo>, info, m_info_tbl)
//	{
//		if(info->home != feod_name)
//		{
//			continue;
//		}
//
//		CRegionCreature* p_cre = g_region->m_defScene->FindCreature(info->o_id);
//
//		if(p_cre == NULL) continue;
//
//		p_cre->Die(NULL);
//
//		m_info_tbl.erase(info);
//	}
//}

void CRegionGuarderMgr::KillAllCastleWarGuarder(std::string feod_name)
{
	LOG1("KillAllCastleWarGuarder is called: [%s]\n", (char*)feod_name.c_str());
	
	int count = 0;
	SNpc*	p_npc	= NULL;

	for(count = 0; count < (int)m_castlewar_guarder.size(); count++)
	{
		//取得守卫信息
		p_npc = GetCastleWarGuarderById(m_castlewar_guarder[count].guarder_info.Id);

		if(p_npc == NULL) continue;

		if(m_castlewar_guarder[count].guarder_info.Home != feod_name)
		{
			continue;
		}
		
		m_castlewar_guarder[count].isdead = true;

		CRegionCreature* p_cre = g_region->m_defScene->FindCreature(m_castlewar_guarder[count].o_id);

		if(p_cre != NULL)
		{			
			p_cre->Die(NULL);
		}else
		{
			LOG1("Cannot find guarder by oId: [%d]\n", m_castlewar_guarder[count].o_id);
		}
	}
}

void CRegionGuarderMgr::CreateAllCastleWarGuarder(std::string feod_name)
{
	if(m_castlewar_guarder.empty())
	{
		return;
	}

	int		count	= 0;
	SNpc*	p_npc	= NULL;

	for(count = 0; count < (int)m_castlewar_guarder.size(); count++)
	{
		//取得守卫信息
		p_npc	   = GetCastleWarGuarderById(m_castlewar_guarder[count].guarder_info.Id);

		if(p_npc == NULL) continue;

		if(m_castlewar_guarder[count].guarder_info.Home != feod_name)
		{
			continue;
		}
		
		m_castlewar_guarder[count].isdead = false;
        
		//创建
		CRegionCreature *cre = g_region->CreateNpc(m_castlewar_guarder[count].guarder_info.Id);
		if(!cre) 
		{
#ifdef _DEBUG
			LOG1("Can't create NPC (ID=%d)\n", m_castlewar_guarder[count].guarder_info.Id);
#endif //_DEBUG
			return;
		}

		m_castlewar_guarder[count].o_id = cre->m_oId;

		//添加至场景
		long pos[3];
		char dir = m_castlewar_guarder[count].dir;

		pos[0] = m_castlewar_guarder[count].pos[0];
		pos[1] = m_castlewar_guarder[count].pos[1];
		pos[2] = m_castlewar_guarder[count].pos[2];
		
		if(g_region->m_defScene->AddCreature(cre, pos, dir))
		{
			if(cre->IsScriptLoaded())
				cre->CallScript("OnCreate", false);
		}
		else
		{
			g_factory->DestroyObject(cre);

			cre = NULL;
		}
	}	
}

//void CRegionGuarderMgr::CreateGuarder(CGuarderLair* p_lair)
//{
//	if(p_lair == NULL)
//	{
//		return;
//	}
//
//	for(int i = 0; i < p_lair->m_max_count; i++)
//	{
//		//创建
//		CRegionCreature *cre = g_region->CreateNpc(p_lair->m_npc_id);
//
//		if(!cre) 
//		{
//#ifdef _DEBUG
//			LOG1("Can't create NPC (ID=%d)\n", p_lair->m_npc_id);
//#endif //_DEBUG
//			return;
//		}
//
//		long rangex = (p_lair->m_area_end[0] - p_lair->m_area_begin[0]) / 20;
//		long rangey = (p_lair->m_area_end[1] - p_lair->m_area_begin[1]) / 20;		
//		long rx = (rand() % rangex) * 20; if (rangex < 0) rx = -rx;
//		long ry = (rand() % rangey) * 20; if (rangey < 0) ry = -ry;
//
//		int tx, ty;
//		long pos[3];
//
//		g_region->m_defScene->m_pTerrain->GetTerrainByPosFast(rx, ry, tx, ty);
//
//		pos[0] = p_lair->m_area_begin[0] + rx;
//		pos[1] = p_lair->m_area_begin[1] + ry;
//		pos[2] = 0;
//
//		if(g_region->m_defScene->AddCreature(cre, pos, p_lair->m_dir))
//		{		
//			if(cre->IsScriptLoaded())
//				cre->CallScript("OnCreate", false);	
//		}
//		else
//		{
//			g_factory->DestroyObject(cre);
//
//			cre = NULL;
//		}
//
//		STCWarGuarderInfo tmp;
//
//		tmp.o_id = cre->m_oId;
//		tmp.home = p_lair->m_home;
//
//		m_info_tbl.push_back(tmp);
//	}
//}

//void CRegionGuarderMgr::CreateAllCastleWarGuarder(std::string feod_name)
//{
//	if(m_guarder_lair.empty())
//	{
//		return;
//	}
//
//	for(int i = 0; i < (int)m_guarder_lair.size(); i++)
//	{
//		if(m_guarder_lair[i].m_home != feod_name)
//		{
//			continue;
//		}
//
//		CreateGuarder(&m_guarder_lair[i]);
//	}
//}

void CRegionGuarderMgr::CreateGuarder()
{
	
	int		now			= (int)rtGetMilliseconds();
	int		max			= (int)m_guarder_info.size();
	int		time_stamp  = 0;
	SNpc*	p_npc		= NULL;

	//全都活着
	if(m_curr_counter >= m_max_counter)
	{
		for(int count = 0; count < max; count++)
		{
			m_guarder_info[count].last_time_stamp = now;
		}
		return;
	}
	
	for(int count = 0; count < max; count++)
	{
		if(m_guarder_info[count].isdead == false)
		{
			m_guarder_info[count].last_time_stamp = now;
			continue;
		}

		//取得已死亡守卫信息
		p_npc	   = GetGuarderById(m_guarder_info[count].id);
		time_stamp = m_guarder_info[count].last_time_stamp +
					 m_guarder_info[count].guarder_info.Interval;
		
		//未到重生时间
		if((m_guarder_info[count].last_time_stamp != 0) && (now < time_stamp))
		{
			continue;
		}

		if(p_npc == NULL) continue;

		//重生
		m_guarder_info[count].last_time_stamp = now;
		m_guarder_info[count].isdead		  = false;
		m_curr_counter++;

		if(m_curr_counter > m_max_counter)
		{
#ifdef _DEBUG
			LOG("the number of guarder is out of range\n");
			return;
#endif //_DEBUG
		}

		//创建
		CRegionCreature *cre = g_region->CreateNpc(m_guarder_info[count].id);
		if(!cre) 
		{
#ifdef _DEBUG
			LOG1("Can't create NPC (ID=%d)\n", m_guarder_info[count].id);
#endif //_DEBUG
			return;
		}
		
		//添加至场景
		long pos[3];
		char dir = rand() % 255;

		if(!p_npc->PatrolPath.empty())
		{
			pos[0] = p_npc->PatrolPath.begin()->x;	//默认巡逻初始点为重生点
			pos[1] = p_npc->PatrolPath.begin()->y;
			pos[2] = 0;
		}else
		{
			pos[0] = 0;
			pos[1] = 0;
			pos[2] = 0;
		}

		if(g_region->m_defScene->AddCreature(cre, pos, dir))
		{		
			if(cre->IsScriptLoaded())
				cre->CallScript("OnCreate", false);	
		}
		else
		{
			g_factory->DestroyObject(cre);

			cre = NULL;
		}
	}

	//已复活所有
	//m_curr_counter = m_max_counter;

	return;
}
