/////////////////////////////////
//Filename:   gw_LineMan.h
//Author:     rendh
//CreateTime: 2009-12-29
//Desc:		  分线系统GWS管理器
////////////////////////////////
#pragma once
#include "gw_LineGroup.h"


class gw_LineMan
{
public:
	gw_LineMan(void);
	~gw_LineMan(void);
	bool Startup();
	void Shutdown();
	bool AddRegion(int irgID);
	bool OnRegionDisconnected(int irgID);
	bool Run();
	void OnCreateLineRes(int irgID);
	bool EnterLineReq(DWORD dwGroupID, DWORD dwUserID, DWORD& dwLineID, int& iRegionID);
	bool OnEnterLineRes(DWORD dwGroupID, DWORD dwUserID);
	bool LineInfoReq(DWORD dwGroupID, std::list<gw_LineGroup::SLineInfo>& listLineInfo);
	bool LeaveLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID);
	bool OnLeaveLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID);
	bool SwitchLineReq(DWORD dwGroupID, DWORD dwOldLineID, DWORD dwNewLineID, DWORD dwUserID, int& iRegionID);
	bool GetUserRegion(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, int& iRegionID);
	void OnTimer();
private:
	bool FindLine();
	SmartPtr<gw_LineGroup> FindGroup(DWORD dwGroupID);
	DWORD m_dwUserCount;
	std::list<SmartPtr <gw_LineGroup> > m_listLineGroup;
};
