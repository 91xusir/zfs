#ifndef __GW_REGION_ACTIVITY_H__
#define __GW_REGION_ACTIVITY_H__

#include <map>
#include "region_client.h"
#include "gw_server.h"
#include "gw_database.h"

//最大SQL命令字符串长度GW_MAX_DB_COMMAND_LENGTH
class GW_ActivityBase;

class GW_ActivityManager
{
	typedef	std::map<int, GW_ActivityBase*>	GW_ACTIVITYS;

public:
	~GW_ActivityManager(void);
	static GW_ActivityManager*	GetSingle(void)
	{
		static GW_ActivityManager gwActMgr;
		return &gwActMgr;
	}

	bool	StartUp(void);
	void	Shutdown(void);
	void	OnEvent(void);
	void	UpdataActivityToRegion(int iRegionID);
	void	Dispose(long lRegionID, CG_CmdPacket *pCommand);
	void	SetUserInfo(int iSession, long lActID, long lUserID, long eState, std::string sActivity, long *plParam);
	void	UpdataActivity(int iID, const char *sActivity);
	void	GetUserEnterActivity(int iSession, long lUserID);
	void	DeleteAllInfoByUser(std::string &sUserName);

public:	//数据库查询返回处理
	static void	ReturnUserInfo(GWDBResult* pDBResult);
	static void	ReturnActivityInfo(GWDBResult* pDBResult);
	static void	ReturnUserEnterActivity(GWDBResult* pDBResult);

private:
	GW_ActivityManager(void);

	GW_ActivityBase*	CreateActivity(int iID, ActivityRes *pRes);
	bool				RemoveActivity(int iID);
	void				GetDatabaseActivity(GW_ActivityBase *pActivity);
	void				SaveUserInfo(CG_CmdPacket *pCommand);
	GW_ActivityBase*	GetActivity(int iID);

private:
	GW_ACTIVITYS	mapActivitys;

	const long		c_CycleTime;
	long			m_lLastUpdataTime;

public:
	static const char*	c_sUserInfoTable;
	static const char*	c_sUserEnterTable;
};


class GW_ActivityBase
{
public:
	GW_ActivityBase(int iID, ActivityRes *pRes);
	~GW_ActivityBase(void);

	virtual void	UpdataActivity(const char *sActivity);
	virtual void	OnEvent(long lCurTime);
	virtual void	GetUserInfo(int iRegionID, CG_CmdPacket *pCommand) = 0;
	virtual void	GetDatabaseActivity(void);
	virtual void	SetDatabaseActivity(void);
	virtual void	SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer) = 0;
	virtual bool	GetSerializeString(std::string &sBuffer);
	//void	Dispose(CG_CmdPacket *pCommand);

	const char*		GetTableName(void)	{return m_DBTableName.c_str();}
	int				GetID(void)		{return m_iID;}
	ACTIVITY_TYPE	GetType(void)	{return m_eType;}
	const char*		GetActName(void){return m_sActName.c_str();}

private:
	void			Startup(int iStartIndex);
	void			Shutdown(int iOverIndex);
	void			CycleTime(int iCycleIndex);
	void			ForeTime(int iActIndex, int iForeIndex);

protected:
	int				m_iID;
	ACTIVITY_TYPE	m_eType;
	std::string		m_sActName;
	bool			m_bCanFore[ActivityRes::iMaxEventCount][ActivityRes::iMaxEventCount];	//一次性预告标志
	short			m_iForeTime[ActivityRes::iMaxEventCount][ActivityRes::iMaxEventCount][5];	//月、周、日、时、分  (为0表示每个时段)
	short			m_iStartTime[ActivityRes::iMaxEventCount][5];	//月、周、日、时、分  (为0表示每个时段)
	short			m_iOverTime[ActivityRes::iMaxEventCount][5];	//月、周、日、时、分  (为0表示每个时段)
	int				m_iEventCycle;
	std::string		m_sStartInfo[ActivityRes::iMaxEventCount];
	std::string		m_sOverInfo[ActivityRes::iMaxEventCount];
	std::string		m_sCycleInfo[ActivityRes::iMaxEventCount];
	std::string		m_sForeInfo[ActivityRes::iMaxEventCount];

	std::string		m_DBTableName;
	ActivityInfo	*m_pActivityInfo;
	long			m_lLastSpring;
};

class GW_ActivityTelpher : public GW_ActivityBase
{
public:
	GW_ActivityTelpher(int iID, ActivityRes *pRes);
	~GW_ActivityTelpher(void);

	void	GetUserInfo(int iRegionID, CG_CmdPacket *pCommand);
	void	SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer);
};

class GW_ActivityTreasure : public GW_ActivityBase
{
public:
	GW_ActivityTreasure(int iID, ActivityRes *pRes);
	~GW_ActivityTreasure(void);

	void	GetUserInfo(int iRegionID, CG_CmdPacket *pCommand);
	void	SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer);

private:	//数据库查询返回处理
	static void	ReturnUserInfo(GWDBResult* pDBResult);

protected:
	std::string	m_UserStateTable;
};

class GW_ActivityCachot : public GW_ActivityBase
{
public:
	GW_ActivityCachot(int iID, ActivityRes *pRes);
	~GW_ActivityCachot(void);

	void	GetUserInfo(int iRegionID, CG_CmdPacket *pCommand);
	void	SaveUserInfo(std::string &sUserName, int iEnterActive, std::string &sBuffer);

private:	//数据库查询返回处理
	static void	ReturnUserInfo(GWDBResult* pDBResult);

protected:
	std::string	m_UserStateTable;
};

#endif//__GW_REGION_ACTIVITY_H__
