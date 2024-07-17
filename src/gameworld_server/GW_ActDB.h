#ifndef _INC_GW_ACTDB_H_
#define _INC_GW_ACTDB_H_


struct GWDBResult;
struct GWUser;

class GW_ActDB : public RtThread 
{
public:
	GW_ActDB(void);
	~GW_ActDB(void);

	bool			Initalize(void);
	void			Close(void);
	bool			IsValid(void) { RtThreadLock::AutoLock lock(m_cmdLock); return m_mysql != NULL && m_connectTime != 0; }
	void			Stop(void);

	bool			ExecCommand(const char* command, MYSQL_RES** res, bool bLogErr=true);
	void			PushCommand(GWDBCommand& cmd) { m_commandQueue.Push(cmd); }

protected:
	virtual int		OnRun(void);

public:
	void			ProcessQueryResult(void);
	void			EscapeString(string& result, const string& str);
	void			EscapeString(char* result, const char* str);
	std::string		EscapeString(const char *str); 

	bool				Connect(void);

private:
	MYSQL*					m_mysql;
	RtThreadLock			m_cmdLock;
	GWDBCommandQueue		m_commandQueue;
	GWDBResultQueue			m_resultQueue;

	bool					m_shutdown;
	int						m_connectTime;
};

#endif // _INC_GW_ACTDB_H_
