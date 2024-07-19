#include "gw_server.h"
#include "GW_ActDB.h"

GW_ActDB::GW_ActDB(void)
: m_mysql(NULL)
, m_shutdown(false)
, m_connectTime(0)
{
}

GW_ActDB::~GW_ActDB(void)
{
	Close();
}

bool	GW_ActDB::Initalize(void)
{
	m_commandQueue.Create(5000, "DBCommand");
	m_resultQueue.Create(5000, "DBResult");
	m_mysql = mysql_init(NULL);
	if(m_mysql == NULL)
	{
		ERR("[GWActDb::Initalize] Initialize MySQL failed\n");
		return false;
	}

	if (!Connect())
	{
		Close();
		return false;
	}

	LOG("Start activity database thread.\n");
	Start();

	return true;
}

void	GW_ActDB::Close(void)
{
	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
}

void	GW_ActDB::Stop(void)
{
	m_cmdLock.Enter();
	m_shutdown = true;
	m_cmdLock.Leave();

	LOG("Stop the activity database command queue\n");
	m_commandQueue.Stop();
	Join();
	LOG("Stop the activity database result queue\n");
	m_resultQueue.Stop();

	int succeeded = 0;
	int failed = 0;
}

bool	GW_ActDB::ExecCommand(const char* command, MYSQL_RES** res, bool bLogErr/* =true */)
{
	RtThreadLock::AutoLock lock(m_cmdLock);

	if (m_connectTime)
		return false;
	if (!m_mysql)
		return false;

	DWORD tBefore = rtGetMilliseconds();
	if (mysql_query(m_mysql, command))
	{
		if (bLogErr)
		{
			LOG3("[GWActDb::ExecCommand] Exec [%s] failed (%d)\nMYSQL: %s\n", command, mysql_errno(m_mysql), mysql_error(m_mysql));
		}

		if (mysql_errno(m_mysql) == CR_CONN_HOST_ERROR)
			m_connectTime = time(NULL);

		return false;
	}
	else
	{
		if (res)
			*res = mysql_store_result(m_mysql);
	}
	DWORD tAfter = rtGetMilliseconds();
	if ((tAfter-tBefore)>1000)
	{
		ERR2("[GWActDb::ExecCommand]SQL time = %d, SQL = %s\n", tAfter-tBefore, command);
	}

	return true;
}

int		GW_ActDB::OnRun(void)
{
	LOG1("Activity database thread started at %d", rtTimestamp());//timn.yang  %s->%d

	GWDBCommand cmd;
	GWDBResult res;

	for(;;)
	{
		m_cmdLock.Enter();
		if(m_shutdown)
		{
			m_cmdLock.Leave();
			return 0;
		}
		m_cmdLock.Leave();

		if(!m_commandQueue.Pop(cmd))
		{
			rtSleep(0.01 * 1000);
			continue;
		}

		m_cmdLock.Enter();
		for(;;)
		{
			if (m_shutdown)
			{
				m_cmdLock.Leave();
				return 0;
			}

			if (m_connectTime)
			{
				m_cmdLock.Leave();
				Sleep(1 * 1000);
				m_cmdLock.Enter();
				continue;
			}

			if (mysql_query(m_mysql, cmd.command))
			{
				if (mysql_errno(m_mysql) == CR_CONN_HOST_ERROR)
				{
					m_connectTime = time(NULL);
					continue;
				}
				res.okay = false;
				res.res = NULL;
			}
			else
			{
				res.okay = true;		
				if (cmd.type == dbCmdQuery)
					res.res = mysql_store_result(m_mysql);
				else
					res.res = NULL;
				res.affectRows = mysql_affected_rows(m_mysql);
			}
			break;
		}

		m_cmdLock.Leave();
		res.succeeded = cmd.succeeded;
		res.failed = cmd.failed;
		res.data = cmd.data;
		m_resultQueue.Push(res);
	}
	return 0;
}

void	GW_ActDB::ProcessQueryResult(void)
{
	time_t now = time(NULL);

	if (m_connectTime && now - m_connectTime > GW_CONNECT_DB_INTERVAL)
	{
		m_connectTime = now;

		Connect();
	}

	GWDBResult res;

	int count = m_resultQueue.GetSize();

	while(count)
	{
		if (!m_resultQueue.Pop(res))
			return;

		if (res.okay)
		{
			if (res.succeeded)
				res.succeeded(&res);

			if (res.res)
				mysql_free_result(res.res);
		}
		else
		{
			if (res.failed)
				res.failed(&res);
		}

		count--;
	}
}

void	GW_ActDB::EscapeString(string& result, const string& str)
{
	result.resize((long)str.size() * 2);
	result.resize(mysql_real_escape_string(m_mysql, &result[0], str.c_str(), (unsigned long)str.length()));
}

void	GW_ActDB::EscapeString(char *result, const char *str)
{
	mysql_real_escape_string(m_mysql, result, str, (int)strlen(str));
}

const int MAX_ESCAPE_STRING = 1024*16;
std::string	GW_ActDB::EscapeString(const char *str)
{
	//static char buf[MAX_ESCAPE_STRING];
	S_MEMDEF(buf, MAX_ESCAPE_STRING)
	S_MEMPROTECTOR(buf, MAX_ESCAPE_STRING, bMP)
	unsigned long len = mysql_real_escape_string(m_mysql,buf,str,(unsigned long)strlen(str));
	if(len >= MAX_ESCAPE_STRING) len = 0;
	buf[len] = '\0';
	return buf;
}

bool	GW_ActDB::Connect(void)
{
	m_mysql = mysql_init(m_mysql);

	LOG2("Connecting to activity db server %s on port %d ... \n", 
		g_cfgGW.connectActDB.ip.c_str(), 
		g_cfgGW.connectActDB.port);

	if (!mysql_real_connect(m_mysql, 
		g_cfgGW.connectActDB.ip.c_str(), 
		g_cfgGW.connectActDB.username.c_str(), 
		g_cfgGW.connectActDB.password.c_str(),
		NULL,
		g_cfgGW.connectActDB.port,
		NULL, 
		0))
	{
		LOG3("Can't connect to activity db server %s on port %d\n    MSG= %s\n", 
			g_cfgGW.connectActDB.ip.c_str(), 
			g_cfgGW.connectActDB.port,
			mysql_error(m_mysql));

		return false;
	}

	if (mysql_select_db(m_mysql, g_cfgGW.connectActDB.database.c_str()) < 0)
	{
		LOG2("Can't select activity db %s\n    MSG= %s\n", g_cfgGW.connectActDB.database.c_str(), mysql_error(m_mysql));

		return false;
	}

	m_connectTime = 0;

	return true;
}
