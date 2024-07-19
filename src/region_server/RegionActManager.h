#ifndef __REGION_ACTIVITY_H__
#define __REGION_ACTIVITY_H__

#include <map>
#include <list>
#include "region.h"
#include "ActivityInfo.h"

class CRegionActivityBase;

class CRegionActManager
{
	typedef	std::map<int, CRegionActivityBase*>	ACTIVITYS;
	typedef	std::list<UserActivityInfo*>		DELETE_LIST;

public:
	~CRegionActManager(void);
	static CRegionActManager*	GetSingle(void)
	{
		static CRegionActManager oActMgr;
		return &oActMgr;
	}

	bool CanExecute(int iID, CRegionUser *pUser);
	bool Execute(int iID, CRegionUser *pUser, long *plParam);
	bool Complate(int iID, CRegionUser *pUser);
	void OnEvent(CG_CmdPacket *pCommand);
	void SaveUserInfo(int iID, CRegionUser *pUser, bool bEnterActive, UserActivityInfo *pInfo);
	void Shutdown(void);

	static UserActivityInfo*	GetNewUserActivityInfo(ACTIVITY_TYPE eType);

private:
	CRegionActManager(void);
	void CreateActivity(CG_CmdPacket *pCommand);
	void RemoveActivity(int iID);
	void SetUserEnterActivity(CG_CmdPacket *pCommand);
	CRegionActivityBase*	GetActivity(int iID);

private:
	ACTIVITYS		mapActivitys;

public:
	DELETE_LIST	listDelInfo;
	const int ciDelListmaxCount;
};


class CRegionActivityBase : public CRegionObject
{
	typedef	std::list<int>	LINK_NPC;
	typedef std::list<long>	USER_GROUP;
	typedef std::list<USER_GROUP>	JOIN_EVENTS;

public:
	CRegionActivityBase(int iID);
	virtual ~CRegionActivityBase(void);

	virtual bool Execute(CRegionUser *pUser, long *plParam);
	virtual bool Complate(CRegionUser *pUser);
	virtual bool CanExecute(CRegionUser *pUser);
	virtual void OnEvent(ACTIVITY_MESSAGE eMsgType, CG_CmdPacket *pCommand);
	virtual void UpdataInfo(std::string sString, bool bRealTime);

	void	AddLinkNPC(int iNPC_ID);
	CRegionCreature*	GetLinkNPCByIndex(int iIndex);
	void	GetAllLinkNPC(std::list<CRegionCreature*> &listCreature);
	void	ClearLinkNPCByIndex(int iIndex);
	void	ClearLinkNPCByID(int iID);

protected:
	virtual void StartUp(long lIndex);
	virtual void Shutdown(long lIndex);
	virtual bool Compera(UserActivityInfo *pInfo, CRegionUser *pUser);
	virtual void GetUserInfo(CRegionUser *pUser, ACTIVE_STATE eState, long *plParam);
	virtual void SetUserInfo(ACTIVE_STATE eState, CRegionUser *pUser, UserActivityInfo *pInfo, bool bIsNewInfo, long *plParam);
	virtual void ExecuteSuccessful(CRegionUser *pUser, long *plParam);
	virtual void ExecuteFailed(CRegionUser *pUser);

public:
	static CRegionUser*	GetCreatureByID(long lID);
	static CRegionUser*	GetCreatureByName(const char *sName);

protected:
	int				m_iID;
	ACTIVITY_TYPE	m_eType;
	std::string		m_sActName;
	ActivityInfo	*m_pActivityInfo;
	LINK_NPC		m_listNPC;
	JOIN_EVENTS		listJoins;
};

class ActivityTelpher : public CRegionActivityBase
{
public:
	ActivityTelpher(int iID);
	~ActivityTelpher(void);
};

class ActivityTreasure : public CRegionActivityBase
{
public:
	ActivityTreasure(int iID);
	~ActivityTreasure(void);
};

class ActivityCachotClass : public CRegionActivityBase
{
public:
	ActivityCachotClass(int iID);
	~ActivityCachotClass(void);

	bool Execute(CRegionUser *pUser, long *plParam);
	bool Compera(UserActivityInfo *pInfo, CRegionUser *pUser);

protected:
	void SetUserInfo(ACTIVE_STATE eState, CRegionUser *pUser, UserActivityInfo *pInfo, bool bIsNewInfo, long *plParam);
};

#endif//__REGION_ACTIVITY_H__
