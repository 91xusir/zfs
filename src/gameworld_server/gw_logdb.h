#ifndef _INC_GW_LOGDB_H_
#define _INC_GW_LOGDB_H_

struct GWDBResult;
struct GWUser;

//enum GWDBCommandType
//{
//	dbCmdQuery,
//	dbCmdUpdate,
//	dbCmdInsert,
//	dbCmdDelete
//};
//
//typedef void (*GWDBCallback)(GWDBResult* data);
//struct GWDBCommand
//{
//	GWDBCommandType	type;
//	GWDBCallback	succeeded;
//	GWDBCallback	failed;
//	void*			data;
//	char			command[GW_MAX_DB_COMMAND_LENGTH];
//};
//
//struct GWDBResult
//{
//	bool			okay;
//	GWDBCallback	succeeded;
//	GWDBCallback	failed;
//	void*			data;
//	MYSQL_RES*		res;
//	int				affectRows;
//};
//
//typedef GWDBAsyncQueue<GWDBCommand>	GWDBCommandQueue;
//typedef GWDBAsyncQueue<GWDBResult>	GWDBResultQueue;

//class GWLogDb : public GWThread
class GWLogDb : public RtThread 
{
public:
	bool			Initalize();
	void			Close();
	bool			IsValid() { RtThreadLock::AutoLock lock(m_cmdLock); return m_mysql != NULL && m_connectTime != 0; }
	void			Stop();
	GWUser*			LoadUserData(DWORD accountID, int userID);
	GWActor*		LookupActor(int actorID);
	
	bool			ExecCommand(const char* command, MYSQL_RES** res, bool bLogErr=true);
	void			PushCommand(GWDBCommand& cmd) { m_commandQueue.Push(cmd); }

protected:
	virtual int		OnRun();

public:
	void				ProcessQueryResult();
	bool				QueryActorList(GWLoginClient& client);
	void				EscapeString(string& result, const string& str);
	void				EscapeString(char* result, const char* str);
	std::string			EscapeString(const char *str); 

	bool				Connect();

	std::string			GetTableName() { return m_TableName; }

public:
	GWLogDb();
	virtual ~GWLogDb();

private:
	MYSQL*					m_mysql;	
	RtThreadLock			m_cmdLock;
	GWDBCommandQueue		m_commandQueue;
	GWDBResultQueue			m_resultQueue;	

	bool					m_shutdown;
	int						m_connectTime;

	std::string				m_TableName;
};

#endif // _INC_GW_LOGDB_H_
