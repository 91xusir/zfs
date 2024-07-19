//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------
#pragma once
#include "region_spawn.h"

class rs_Line
{
public:
	rs_Line(void);
	~rs_Line(void);
	bool SetParameter(SLineSys* pstLineGroup, DWORD dwLineID, int iRegionID);
	bool AddUser(CRegionUser* pUser, long* ppos);
	bool DelUser(CRegionUser* pUser);
	bool SwitchLine(CRegionUser* pUser, rs_Line* pNewLine, long* ppos);
	void Spawn();
	
	DWORD m_dwLineID;
	int m_iRegionID;
	DWORD m_dwLineUserCount;
private:
	CRegionScene*	m_pscene;
	CRegionSpawnMgr	m_SpawnMgr;
};

class rs_LineGroup
{
public:
	rs_LineGroup(void);
	~rs_LineGroup(void);
	bool SetParameter(SLineSys* pstLineGroup);
	bool AddLine(DWORD dwLineID, int iRegionID);
	bool DelLine(DWORD dwLineID);
	bool EnterLine(DWORD dwLineID, int iRegionID, CRegionUser* pUser, long* ppos);
	void OnTimer();
	bool SwitchLine(DWORD dwOldLineID, DWORD dwNewLineID, int iRegionID, CRegionUser* pUser);
	bool LeaveLine(DWORD dwLineID, CRegionUser* pUser, long* ppos);
	void Spawn();
	
	DWORD m_dwGroupID;
private:
	bool AddUser(DWORD dwLineID, CRegionUser* pUser, long* ppos);
	bool DelUser(DWORD dwLineID, CRegionUser* pUser);
	SmartPtr<rs_Line> FindLine(DWORD dwLineID);

	DWORD m_dwGroupUserCount;

	std::map<DWORD, SmartPtr <rs_Line> > m_mapLine;
	SLineSys	m_stLineGroup;
};


