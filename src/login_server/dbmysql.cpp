// DBMySql.cpp: implementation of the CDBMySql class.
//
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "dbmysql.h"
//#include "errmsg.h"

CDBMySql::CDBMySql()
{
	m_handle = NULL;
	m_res = NULL;
	m_log = true;
}

CDBMySql::~CDBMySql()
{
	Close();
}

bool CDBMySql::IsActive()
{
	if(!m_handle) return false;

	int ret = mysql_ping(m_handle);
	switch(ret)
	{
		case 0:
			return true;

//        case CR_COMMANDS_OUT_OF_SYNC: 
//        case CR_SERVER_GONE_ERROR: 
//        case CR_UNKNOWN_ERROR: 
//            return false;

		default:
			return false;
	}
}

bool CDBMySql::Connect(const char *host, int port,const char *name,const char *pwd,const char *db)
{
	Close();
	m_handle = mysql_init(NULL);
	if (m_handle==NULL) return false;
	if (port == 0) port = MYSQL_PORT;
	mysql_options(m_handle, MYSQL_SET_CHARSET_NAME, "gb2312");
	if (mysql_real_connect(m_handle, host, name, pwd, NULL, port, NULL, 0) == NULL)
	{
		ShowError("mysql_real_connect");
		return false;
	}
	if (mysql_select_db(m_handle, db) != 0) 
	{
		ShowError("mysql_select_db");
		return false;
	}
	return true;
}

void CDBMySql::Close()
{
	if (m_handle)
	{
		mysql_close(m_handle);
		m_handle = NULL;
	}
	if (m_res)
	{
		mysql_free_result(m_res);
		m_res = NULL;
	}
}

bool CDBMySql::Query(const char *sql,int len)
{
	if (!m_handle) return false;
	if (len == 0) len = (int)strlen(sql);
	if (mysql_real_query(m_handle, sql, len) != 0) 
	{
		ShowError((char*)sql);
		return false;
	}

	if (m_res != NULL) mysql_free_result(m_res);
	m_res = mysql_store_result(m_handle);
	if (m_res != NULL)
	{
		m_fieldCnt = mysql_num_fields(m_res);
		m_fields = mysql_fetch_fields(m_res);
	}
	return true;
}

int CDBMySql::LastInsertID()
{
    return (int)mysql_insert_id(m_handle);
}

int CDBMySql::GetFieldCount()
{
	return m_fieldCnt;
}

int CDBMySql::GetRowCount()
{
	return (int)mysql_num_rows(m_res);
}

void CDBMySql::ShowError(char const* sql)
{
	/*
    int err = mysql_error(m_handle);
	switch (err)
	{
        case CR_UNKNOWN_ERROR:
		case CR_SOCKET_CREATE_ERROR:
		case CR_CONNECTION_ERROR:
		case CR_CONN_HOST_ERROR:
		case CR_IPSOCK_ERROR:
		case CR_UNKNOWN_HOST:
		case CR_SERVER_GONE_ERROR:
		case CR_VERSION_ERROR:
		case CR_OUT_OF_MEMORY:
		case CR_WRONG_HOST_INFO:
		case CR_LOCALHOST_CONNECTION:
		case CR_TCP_CONNECTION:	
		case CR_SERVER_HANDSHAKE_ERR: 
		case CR_SERVER_LOST:
        case CR_COMMANDS_OUT_OF_SYNC:
        case CR_NAMEDPIPE_CONNECTION: 
		case CR_NAMEDPIPEWAIT_ERROR: 
		case CR_NAMEDPIPEOPEN_ERROR: 
		case CR_NAMEDPIPESETSTATE_ERROR: 
		case CR_CANT_READ_CHARSET:	
		case CR_NET_PACKET_TOO_LARGE:
        
	*/
	if(m_log)
	{
        ERR2("ERROR: SQL = [%s]\n       ERR = [%s]\n", sql, mysql_error(m_handle));
	}
}

bool CDBMySql::GetRow()
{
	m_row = mysql_fetch_row(m_res);
	if (m_row == NULL) return false;
	return true;
}

char *CDBMySql::GetField(char *fname, int *pnLen)
{
	int i;
	unsigned long *lengths;
	lengths = mysql_fetch_lengths(m_res);
	for(i = 0; i < m_fieldCnt; i++)
	{
		if (strcmp(fname,m_fields[i].name)==0)
		{
			if(pnLen) *pnLen = lengths[i];
			return m_row[i];
		}
	}
	return NULL;
}

//-------------------------------------
// Get Field Name , Data Type And Value
//-------------------------------------
bool CDBMySql::GetField(int idx,char *field,int *type,char **value)
{
	if(idx<0 || idx>m_fieldCnt)
		return false;
	strcpy(field,m_fields[idx].name);
	*type = m_fields[idx].type;
	*value = m_row[idx];
	/*
	if(m_row[idx])
		strValue = m_row[idx];
	else
		strValue = "NULL";
	*/
	return true;
}

//------------------------
// Get Field Name And Type
//------------------------
int CDBMySql::GetFieldInfo(int nIdx, char *pszName)
{
	if(nIdx<0 || nIdx>=m_fieldCnt)
    {
        return 0;
    }
	if(pszName)
    {
        strcpy(pszName, m_fields[nIdx].name);
    }
	return m_fields[nIdx].type;
}

//------------------------------
// Get Field Data Type And Value
//------------------------------
bool CDBMySql::GetFieldContent(int nIdx , int *pnType , char **ppValue)
{
	if(nIdx<0 || nIdx>=m_fieldCnt)
    {
        return false;
    }
	*pnType  = m_fields[nIdx].type;
	*ppValue = m_row[nIdx];
    return true;
}
    
unsigned long CDBMySql::GetInsertId()
{
	return mysql_insert_id(m_handle);
}

long CDBMySql::GetAffectedRows()
{
	return mysql_affected_rows(m_handle);
}

void CDBMySql::EnableLog(bool enable)
{
	m_log = enable;
}

const int MAX_ESCAPE_STRING = 1024*16;
std::string CDBMySql::EscapeString(const char *str)
{
	static char buf[MAX_ESCAPE_STRING];
	
	unsigned long len = mysql_real_escape_string(m_handle,buf,str,(unsigned long)strlen(str));
    if(len >= MAX_ESCAPE_STRING) len = 0;
	buf[len] = '\0';

	return buf;
}
