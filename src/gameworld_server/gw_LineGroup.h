//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------
#pragma once
class gw_Line
{
public:
	struct  SLineUser
	{
		DWORD dwUID;
	};

public:
	gw_Line(void);
	~gw_Line(void);
	bool SetParameter(DWORD dwLineID, int iRegionID);
	bool AddUser(DWORD dwUserID);
	bool DelUser(DWORD dwUserID);
	SLineUser* FindLineUser(DWORD dwUID);
	void OnRegionDisconnected(int irgID);
	void OnCreateLineRes(int irgID);
	
	DWORD m_dwLineID;
	int m_iRegionID;
	DWORD m_dwLineUserCount;
	bool m_bAcitve;
private:
	map<DWORD, SLineUser> m_mapLineUser;
};

class gw_LineGroup
{
public:

	struct SLineInfo
	{
		DWORD dwLineID;
		DWORD dwUserCount;
		DWORD dwMaxUserCount;
	};

public:
	gw_LineGroup(void);
	~gw_LineGroup(void);
	bool SetParameter(SLineSys* pstLineGroup);
	bool AddLine(DWORD dwLineID, int iRegionID);
	bool DelLine(DWORD dwLineID);
	bool AddUser(DWORD dwLineID, DWORD dwUserID);
	bool DelUser(DWORD dwLineID, DWORD dwUserID);
	SmartPtr<gw_Line> FindLine(DWORD dwLineID);
	void OnRegionDisconnected(int irgID);
	void OnCreateLineRes(int irgID);
	bool EnterLineReq(DWORD dwUserID, DWORD& dwLineID, int& iRegionID);
	bool OnEnterLineRes(DWORD dwUserID);
	void OnTimer();
	bool SwitchLineReq(DWORD dwOldLineID, DWORD dwNewLineID, DWORD dwUserID, int& iRegionID);
	bool LeaveLineReq(DWORD dwLineID, DWORD dwUserID);
	bool LineInfoReq(std::list<SLineInfo>& listLineInfo);
	bool GetUserRegion(DWORD dwLineID, DWORD dwUserID, int& iRegionID);
	
	DWORD m_dwGroupID;
private:
	DWORD m_dwGroupUserCount;
	DWORD m_dwMaxUserPerLine;
	DWORD m_dwSwitchLineCount;

	struct  SPendingUser
	{
		DWORD dwLineID;
		DWORD dwTimer;//被Timer扫描一次加1
	};

	std::map<DWORD, SmartPtr <gw_Line> > m_mapLine;
	std::map<DWORD, SPendingUser> m_mapPendingUser;
	SLineSys	m_stLineGroup;
};


