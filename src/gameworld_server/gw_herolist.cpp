#include "gw_server.h"
#include "gw_herolist.h"

CHeroList g_HeroList(HERO_LIST_COUNT, UNION_LIST_COUNT);
CHeroList* GetHeroList()
{
	return &g_HeroList;
}

CHeroList::CHeroList(int HeroCount, int UnionCount)
:m_data(HeroCount, UnionCount)
{
	m_LastUpdateTime = 0;

	m_bUpdated_Warrior = false;
	m_bUpdated_Hunter = false;
	m_bUpdated_Taolist = false;
	m_bUpdated_Wizard = false;
	m_bUpdated_Union = false;
}

CHeroList::~CHeroList()
{
}

bool CHeroList::Init()
{
	Refresh();

	return true;
}

void CHeroList::OnRefresh()
{
	time_t now = time(NULL);
	if (m_data.m_version==0)
	{
		Refresh();
	}
	else
	{
		if (m_LastUpdateTime + HEROLIST_UPDATE_PERIOD < now)
		{
			Refresh();
		}
	}
}

bool CHeroList::Refresh()
{
	static int iVersion = 0;
	static time_t tLastUpdate = 0;
	time_t now = time(NULL);

	if (now - tLastUpdate < HEROLIST_STAGGER_PERIOD)
	{
		//LOG2("now=%d, tLastUpdate=%d\n", now, tLastUpdate);
		return true;
	}
	else 
	{
		tLastUpdate = now;
	}

	char str256[256];
	MYSQL_RES *pDBResult;
	MYSQL_ROW row;
	int nRowCount = 0;
	int nUpdateCount = 0;

	if (!m_bUpdated_Warrior && nUpdateCount==0)
	{
		rt2_sprintf(str256, "SELECT Name,Level FROM `user` WHERE `Metier`=%d AND `usertype`=0 AND `deleted`=0 ORDER BY `Level` DESC, `EXP` DESC LIMIT 0,%d", METIER_WARRIOR, m_data.m_nHeroCount);
		if ( !g_server.m_database.ExecCommand(str256, &pDBResult) )	
			return false;	

		m_data.m_warriorList.clear();
		nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);
			SHeroListItem listItem;
			listItem.name = row[0];
			listItem.level = atol(row[1]);
			m_data.m_warriorList.push_back(listItem);
		}
		mysql_free_result(pDBResult);

		//LOG("Update Warrior List\n");
		m_bUpdated_Warrior = true;
		nUpdateCount ++;
	}

	if (!m_bUpdated_Hunter && nUpdateCount==0)
	{
		rt2_sprintf(str256, "SELECT Name,Level FROM `user` WHERE `Metier`=%d AND `usertype`=0 AND `deleted`=0 ORDER BY `Level` DESC, `EXP` DESC LIMIT 0,%d", METIER_HUNTER, m_data.m_nHeroCount);
		if ( !g_server.m_database.ExecCommand(str256, &pDBResult) )	
			return false;	

		m_data.m_hunterList.clear();
		nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);
			SHeroListItem listItem;
			listItem.name = row[0];
			listItem.level = atol(row[1]);
			m_data.m_hunterList.push_back(listItem);
		}
		mysql_free_result(pDBResult);

		//LOG("Update Hunter List\n");
		m_bUpdated_Hunter = true;
		nUpdateCount ++;
	}

	if (!m_bUpdated_Taolist && nUpdateCount==0)
	{
		rt2_sprintf(str256, "SELECT Name,Level FROM `user` WHERE `Metier`=%d AND `usertype`=0 AND `deleted`=0 ORDER BY `Level` DESC, `EXP` DESC LIMIT 0,%d", METIER_TAOIST, m_data.m_nHeroCount);
		if ( !g_server.m_database.ExecCommand(str256, &pDBResult) )	
			return false;	

		m_data.m_taolistList.clear();
		nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);
			SHeroListItem listItem;
			listItem.name = row[0];
			listItem.level = atol(row[1]);
			m_data.m_taolistList.push_back(listItem);
		}
		mysql_free_result(pDBResult);

		//LOG("Update Taolist List\n");
		m_bUpdated_Taolist = true;
		nUpdateCount ++;
	}

	if (!m_bUpdated_Wizard && nUpdateCount==0)
	{
		rt2_sprintf(str256, "SELECT Name,Level FROM `user` WHERE `Metier`=%d AND `usertype`=0 AND `deleted`=0 ORDER BY `Level` DESC, `EXP` DESC LIMIT 0,%d", METIER_WIZARD, m_data.m_nHeroCount);
		if ( !g_server.m_database.ExecCommand(str256, &pDBResult) )	
			return false;	

		m_data.m_wizardList.clear();
		nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);
			SHeroListItem listItem;
			listItem.name = row[0];
			listItem.level = atol(row[1]);
			m_data.m_wizardList.push_back(listItem);
		}
		mysql_free_result(pDBResult);

		//LOG("Update Wizard List\n");
		m_bUpdated_Wizard = true;
		nUpdateCount ++;
	}

	if (!m_bUpdated_Union && nUpdateCount==0)
	{
		rt2_sprintf(str256, "SELECT Name,Level,Score FROM `T_union` ORDER BY `Level` DESC, `Score` DESC LIMIT 0,%d", m_data.m_nUnionCount);
		if ( !g_server.m_database.ExecCommand(str256, &pDBResult) )	
			return false;	

		m_data.m_unionList.clear();
		nRowCount = mysql_num_rows(pDBResult);
		for (int i=0; i<nRowCount; i++)
		{
			row = mysql_fetch_row(pDBResult);
			SUnionListItem listItem;
			listItem.name = row[0];
			listItem.level = atol(row[1]);
			listItem.score = atol(row[2]);
			m_data.m_unionList.push_back(listItem);
		}
		mysql_free_result(pDBResult);

		//LOG("Update Union List\n");
		m_bUpdated_Union = true;
		nUpdateCount ++;
	}

	if (m_bUpdated_Warrior && m_bUpdated_Hunter && m_bUpdated_Taolist && m_bUpdated_Wizard && m_bUpdated_Union)
	{
		iVersion ++;
		m_data.m_version = iVersion;
		BroadcastToAllRegion();

		m_LastUpdateTime = now;

		m_bUpdated_Warrior = false;
		m_bUpdated_Hunter = false;
		m_bUpdated_Taolist = false;
		m_bUpdated_Wizard = false;
		m_bUpdated_Union = false;

		LOG1("Herolists are Updated, Now version=%d\n", m_data.m_version);
	}

	return true;
}

void CHeroList::OnAddNewRegion(int regionID)
{
	GWRegion& rg = g_server.m_regionTable[regionID];
	if (rg.sessionIndex == -1)		return;

	CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
	cmd.BeginWrite();
	cmd.WriteShort(g2r_herolist);
	cmd.WriteLong(m_data.m_version);
	m_data.Serialize(&cmd);
	g_server.m_region.SendToPeer(rg.sessionIndex);
}

void CHeroList::BroadcastToAllRegion()
{
	for (long rgID = 0; rgID < (long)g_server.m_regionTable.size(); rgID++)
	{
		GWRegion& rg = g_server.m_regionTable[rgID];
		if (rg.sessionIndex == -1)
			continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_herolist);
		cmd.WriteLong(m_data.m_version);
		m_data.Serialize(&cmd);
		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}
