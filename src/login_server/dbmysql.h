// DBMySql.h: interface for the CDBMySql class.
#ifndef __CDB_MYSQL__
#define __CDB_MYSQL__

#include <mysql/mysql.h>
class CDBMySql  
{
public:
	CDBMySql();
	virtual ~CDBMySql();

	long GetAffectedRows();
	unsigned long GetInsertId();
	bool GetField(int idx,char *field,int *type,char **value);
	int  GetFieldInfo(int nIdx, char *pszName);
    bool GetFieldContent(int nIdx , int *pnType , char **ppValue);
    char *GetField(char *fname,int *len = 0);
    bool GetRow();
	void ShowError(char const* sql);
	int  GetRowCount();
	int  GetFieldCount();
	bool Query(const char *sql,int len=0);
    int LastInsertID();
	void Close();
	bool Connect(const char *host,int port,const char *name,const char *pwd,const char *db);
	void EnableLog(bool enable);
	bool IsActive();
	std::string EscapeString(const char *str); 

private:
	MYSQL		*m_handle;
	MYSQL_RES	*m_res;
	MYSQL_ROW	 m_row;
	MYSQL_FIELD *m_fields;
	int          m_fieldCnt;
	bool		 m_log;
};

#endif 
