#include "gw_server.h"
#include "gw_feod.h"
#include "gw_cmd_feod.h"

CFeodMgr g_FeodMgr;

CFeodMgr* GetFeodMgr()
{
	return &g_FeodMgr;
}


bool CFeod::ChangUnion(DWORD id)
{
	DWORD origID = m_data.unionID;

	m_data.unionID = id;

    GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_feod SET CurrentUnion=%d WHERE CONVERT( `Name` USING utf8 ) = '%s'", id, (char*)m_data.name.c_str());
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		m_data.unionID = origID;
		return false;
	}
	mysql_free_result(res);

	OnCurrentUnionChanged(origID, id);

	return true;
}

bool CFeod::SetTaxRate(int rate)
{
	if (rate>UNION_TAXRATE_MAX || rate<UNION_TAXRATE_MIN)
		return false;

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_feod SET TaxRate=%d WHERE CONVERT( `Name` USING utf8 )='%s'", rate, (char*)m_data.name.c_str());
	if (g_server.m_database.ExecCommand(cmd.command, &res))
	{
		mysql_free_result(res);

		m_data.taxRate = rate;
		return true;
	}
	else 
	{
		return false;
	}
}

bool CFeod::WithdrawBank(int withdraw)
{
	int bankOrig = m_data.bank;
	if (!MoneyIsValid(withdraw))
		return false;
	if (m_data.bank < withdraw)
		return false;

	m_data.bank -= withdraw;
	if (!MoneyIsValid(m_data.bank))
	{
		m_data.bank = bankOrig;
		return false;
	}

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_feod SET Bank=%d WHERE CONVERT( `Name` USING utf8 )='%s'", m_data.bank, (char*)m_data.name.c_str());
	if (g_server.m_database.ExecCommand(cmd.command, &res))
	{
		mysql_free_result(res);
		return true;
	}
	else 
	{
		m_data.bank = bankOrig;
		ERR("Update Bank Table Error!\n");
		return false;
	}
}

bool CFeod::AddIncome(int income)
{
	static DWORD dwLastUpdateTick = 0;

	int bankOrig = m_data.bank;
	if (!MoneyIsValid(income))
	{
		return true;
	}

	m_data.bank += income;
	if (!MoneyIsValid(m_data.bank))
	{
		m_data.bank = bankOrig;
		return false;
	}

	// 更新昨日税收
	time_t ltime;
	time(&ltime);
	struct tm *today = localtime(&ltime);
	if (today->tm_mday==m_lastIncomeDay)
	{
		m_data.incomeToday += income;
	}
	else 
	{
		m_data.incomeToday = 0;
		m_lastIncomeDay = today->tm_mday;
	}

	return true;
}

bool CFeod::SaveToDB()
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_feod SET Bank=%d, LastDayIncome=%d, LastDay=%d WHERE CONVERT( `Name` USING utf8 )='%s'", m_data.bank, m_data.incomeToday, m_lastIncomeDay, (char*)m_data.name.c_str());
	if (g_server.m_database.ExecCommand(cmd.command, &res))
	{
		mysql_free_result(res);
		//封地银行统计
		g_server.LogUserEvent(PLT_FeodBank, 0, "", m_data.bank, m_data.name.c_str(), 0, "", 0, "");
		return true;
	}
	else 
	{
		ERR("Update Bank Table Error!\n");
		return false;
	}
}

void CFeod::OnCurrentUnionChanged(DWORD oldUnion, DWORD newUnion)
{
	// 税率调到默认值
	// 采邑银行清空
	// 昨日税收清零
	m_data.bank = 0;
	m_data.incomeToday = 0;
	m_data.taxRate = UNION_TAXRATE_MIN;

	// 存盘
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "UPDATE T_feod SET Bank=%d, LastDayIncome=%d, LastDay=%d, TaxRate=%d WHERE CONVERT( `Name` USING utf8 )='%s'", m_data.bank, m_data.incomeToday, m_lastIncomeDay, m_data.taxRate, (char*)m_data.name.c_str());
	if (g_server.m_database.ExecCommand(cmd.command, &res))
	{
		mysql_free_result(res);
	}
	else 
	{
		ERR("Update Bank Table Error!\n");
	}

	LOG3("The master of Feod(%s) is changed, old is %d, new is %d\n", m_data.name.c_str(), oldUnion, newUnion);
}


CFeodMgr::CFeodMgr()
{
}

CFeodMgr::~CFeodMgr()
{
}

bool CFeodMgr::Init()
{
	// 确保诸侯国已经初始化！
	CHECK(g_UnionMgr.IsInited());

	time_t ltime;
	time(&ltime);
	struct tm *today = localtime(&ltime);

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_sprintf(cmd.command, "SELECT Name,CurrentUnion,TaxRate,Bank,LastDayIncome,LastDay FROM T_feod");
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	int count = mysql_num_rows(res);

	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		CFeod feod;
		feod.m_data.name = row[0];
		feod.m_data.unionID			= atol(row[1]);
		feod.m_data.taxRate			= atol(row[2]);
		feod.m_data.bank			= atol(row[3]);
		int lastIncome				= atol(row[4]);
		int lastDay					= atol(row[5]);
		if (lastDay==today->tm_mday)
		{
			feod.m_data.incomeToday = lastIncome;
		}
		else 
		{
			feod.m_data.incomeToday = 0;
		}
		feod.m_lastIncomeDay = today->tm_mday;

		if (feod.m_data.unionID!=0)
		{
			if (feod.m_data.taxRate<UNION_TAXRATE_MIN)
			{
				feod.m_data.taxRate = UNION_TAXRATE_MIN;
			}
			else if (feod.m_data.taxRate>UNION_TAXRATE_MAX)
			{
				feod.m_data.taxRate = UNION_TAXRATE_MAX;
			}

			if (!MoneyIsValid(feod.m_data.bank))
			{
				ERR2("[CFeodMgr::Init] FeodBank is INVALID!(feod=%s,feodbank=%d)\n", feod.m_data.name.c_str(), feod.m_data.bank);
				return false;
			}
		}
		m_feods.push_back(feod);
	}
	mysql_free_result(res);

	// 察看占领诸侯是否存在
	for (int j=0; j<(int)m_feods.size(); j++)
	{
		CFeod& feod = m_feods[j];
		if (feod.m_data.unionID==0)		continue;

		CUnion* pUnion = g_UnionMgr.FindUnion(feod.m_data.unionID);
		if (!pUnion)
		{
			feod.ChangUnion(0);
		}
	}

	return true;
}

CFeod* CFeodMgr::FindFeod(string name)
{
	int i;
	for (i=0; i<(int)m_feods.size(); i++)
	{
		if (m_feods[i].m_data.name == name)
			break;
	}

	if (i<(int)m_feods.size())
	{
		return &(m_feods[i]);
	}
	else
		return NULL;
}

void CFeodMgr::OnRegionRegister(int regionID)
{
	// 发送采邑信息
	GWRegion& rg = g_server.m_regionTable[regionID];
	if (rg.sessionIndex == -1)		return;

	CFeodCmdHelper_GW::UpdateRegionFeodData_r2c_feod_data(rg.sessionIndex);
}

void CFeodMgr::OnUnionDeleted(DWORD unionID)
{
	int i;
	for (i=0; i<(int)m_feods.size(); i++)
	{
		if (m_feods[i].m_data.unionID == unionID)
			break;
	}

	if (i<(int)m_feods.size())
	{
		CFeod& feod = m_feods[i];
		feod.ChangUnion(0);
	}
}

void CFeodMgr::Tick()
{
	// 定时存盘
	static DWORD s_dwLastTick = 0;
	DWORD now = rtGetMilliseconds();
	if (now-s_dwLastTick > 1800000)
	{
		s_dwLastTick = now;

		for (int i=0; i<(int)m_feods.size(); i++)
			m_feods[i].SaveToDB();
	}
}
