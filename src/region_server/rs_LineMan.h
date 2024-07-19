/////////////////////////////////
//Filename:   rs_LineMan.h
//Author:     rendh
//CreateTime: 2009-12-29
//Desc:		  分线系统rs管理器
////////////////////////////////
#pragma once
#include "rs_LineGroup.h"


class rs_LineMan
{
public:
	rs_LineMan(void);
	~rs_LineMan(void);
	bool Startup();
	void Shutdown();
	void Spawn();
	bool Run();
	bool CreateLine(int iRegionCount);
	bool EnterLine(DWORD dwGroupID, DWORD dwLineID, int iRegionID, CRegionUser* pUser, long* ppos);
	bool LeaveLine(DWORD dwGroupID, DWORD dwLineID, CRegionUser* pUser, char* pstrMap, long* ppos);
	bool SwitchLine(DWORD dwGroupID, DWORD dwOldLineID, DWORD dwNewLineID, int iRegionID, CRegionUser* pUser);
	void OnTimer();
private:
	bool FindLine();
	SmartPtr<rs_LineGroup> FindGroup(DWORD dwGroupID);
	DWORD m_dwUserCount;
	std::list<SmartPtr <rs_LineGroup> > m_listLineGroup;
};
