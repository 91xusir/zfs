//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------
#include "gw_server.h"
#include "gw_LineGroup.h"

gw_Line::gw_Line(void):
m_dwLineID(0),
m_iRegionID(0),
m_dwLineUserCount(0),
m_bAcitve(false)
{
}

gw_Line::~gw_Line(void)
{
	m_bAcitve = false;
	m_mapLineUser.clear();
	m_dwLineUserCount = 0;
	m_dwLineID = 0;
	m_iRegionID = 0;
}

bool gw_Line::SetParameter(DWORD dwLineID, int iRegionID)
{
	m_dwLineID = dwLineID;
	m_iRegionID = iRegionID;
	return true;
}

bool gw_Line::AddUser(DWORD dwUserID)
{
	if (FindLineUser(dwUserID))
	{
		return false;
	}

	SLineUser stLineUser;
	stLineUser.dwUID = dwUserID;
	m_mapLineUser[dwUserID] = stLineUser;
	m_dwLineUserCount++;

	return true;
}

bool gw_Line::DelUser(DWORD dwUserID)
{
	SLineUser* pLineUser = FindLineUser(dwUserID);

	if (!pLineUser)
	{
		return false;
	}

	m_mapLineUser.erase(dwUserID);
	m_dwLineUserCount--;

	return true;
}


gw_Line::SLineUser* gw_Line::FindLineUser(DWORD dwUID)
{
	std::map<DWORD, SLineUser>::iterator it = m_mapLineUser.find(dwUID);

	if (it == m_mapLineUser.end())
	{
		return NULL;
	}

	return &it->second;
}

void gw_Line::OnRegionDisconnected(int irgID)
{
	if (irgID == m_iRegionID)
	{
		m_mapLineUser.clear();
		m_dwLineUserCount = 0;
		m_bAcitve = false;
	}
}

void gw_Line::OnCreateLineRes(int irgID)
{
	if (irgID == m_iRegionID)
	{
		m_bAcitve = true;
	}
}


gw_LineGroup::gw_LineGroup(void):
m_dwGroupID(0),
m_dwGroupUserCount(0),
m_dwMaxUserPerLine(0),
m_dwSwitchLineCount(0)
{
}

gw_LineGroup::~gw_LineGroup(void)
{
	m_mapPendingUser.clear();
	m_mapLine.clear();
	m_dwGroupUserCount = 0;
	m_dwGroupID = 0;
	m_dwSwitchLineCount = 0;
	m_dwMaxUserPerLine = 0;
}

bool gw_LineGroup::SetParameter(SLineSys* pstLineGroup)
{
	if (!pstLineGroup)
	{
		return false;
	}

	m_stLineGroup = *pstLineGroup;
	m_dwGroupID = m_stLineGroup.id;
	m_dwMaxUserPerLine = m_stLineGroup.maxChr;
	m_dwSwitchLineCount = ((float)m_stLineGroup.maxChr) / 100 * m_stLineGroup.iPercnet;

	return true;
}

bool gw_LineGroup::AddLine(DWORD dwLineID, int iRegionID)
{
	if (FindLine(dwLineID))
	{
		return false;
	}

	SmartPtr<gw_Line> pLine(RT_NEW gw_Line);

	if (!pLine)
	{
		return false;
	}

	pLine->SetParameter(dwLineID, iRegionID);
	m_mapLine[dwLineID] = pLine;
	return true;
}

bool gw_LineGroup::DelLine(DWORD dwLineID)
{
	SmartPtr<gw_Line> pLine = FindLine(dwLineID);

	if (!pLine)
	{
		return false;
	}

	m_mapLine.erase(dwLineID);

	return true;
}

bool gw_LineGroup::AddUser(DWORD dwLineID, DWORD dwUserID)
{
	SmartPtr<gw_Line> pLine = FindLine(dwLineID);

	if (!pLine)
	{
		return false;
	}

	pLine->AddUser(dwUserID);
	m_dwGroupUserCount++;

	return true;
}

bool gw_LineGroup::DelUser(DWORD dwLineID, DWORD dwUserID)
{
	SmartPtr<gw_Line> pLine = FindLine(dwLineID);

	if (!pLine)
	{
		return false;
	}

	pLine->DelUser(dwUserID);
	m_dwGroupUserCount--;

	return true;
}

SmartPtr<gw_Line> gw_LineGroup::FindLine(DWORD dwLineID)
{
	std::map<DWORD, SmartPtr <gw_Line> >::iterator it = m_mapLine.find(dwLineID);

	if (it == m_mapLine.end())
	{
		return NULL;
	}

	return it->second;
}

void gw_LineGroup::OnRegionDisconnected(int irgID)
{
	std::map<DWORD, SmartPtr <gw_Line> >::iterator it;
	
	for (it = m_mapLine.begin(); it != m_mapLine.end(); it++)
	{
		it->second->OnRegionDisconnected(irgID);
	}
}

void gw_LineGroup::OnCreateLineRes(int irgID)
{
	std::map<DWORD, SmartPtr <gw_Line> >::iterator it;

	for (it = m_mapLine.begin(); it != m_mapLine.end(); it++)
	{
		it->second->OnCreateLineRes(irgID);
	}
}


bool gw_LineGroup::EnterLineReq(DWORD dwUserID, DWORD& dwLineID, int& iRegionID)
{
	DWORD dwMinUser = 0;
	DWORD dwMinLineID = 0;
	int iMinRegionID = -1;
	std::map<DWORD, SmartPtr <gw_Line> >::iterator it;

	for (it = m_mapLine.begin(); it != m_mapLine.end(); it++)
	{
		if (it->second->m_bAcitve)
		{
			//printf("%d", it->second->m_dwLineUserCount);
			NOTICE("RegionID:%d, LineID:%d, LineUserCount:%d \n",it->second->m_iRegionID,it->second->m_dwLineID,it->second->m_dwLineUserCount);
			if(it->second->m_dwLineUserCount < m_dwSwitchLineCount)
			{
				dwMinUser = it->second->m_dwLineUserCount;
				dwMinLineID = it->second->m_dwLineID;
				iMinRegionID = it->second->m_iRegionID;
				break;
			}else if(it->second->m_dwLineUserCount < m_dwMaxUserPerLine)
			{
				if (!dwMinUser || dwMinUser > it->second->m_dwLineUserCount)
				{
					dwMinUser = it->second->m_dwLineUserCount;
					dwMinLineID = it->second->m_dwLineID;
					iMinRegionID = it->second->m_iRegionID;
				}
			}
		}

	}

	if (dwMinLineID)
	{
		SPendingUser stPendingUser;
		stPendingUser.dwLineID = dwMinLineID;
		stPendingUser.dwTimer = 0;
		m_mapPendingUser[dwUserID] = stPendingUser;
		AddUser(dwMinLineID, dwUserID);

		dwLineID = dwMinLineID;
		iRegionID = iMinRegionID;
		return true;
	}

	return false;

}

bool gw_LineGroup::OnEnterLineRes(DWORD dwUserID)
{
	std::map<DWORD, SPendingUser>::iterator it = m_mapPendingUser.find(dwUserID);

	if (it == m_mapPendingUser.end())
	{
		return false;
	}

	m_mapPendingUser.erase(it);

	return true;
}

void gw_LineGroup::OnTimer()
{
	std::map<DWORD, SPendingUser>::iterator it;

	for (it = m_mapPendingUser.begin(); it != m_mapPendingUser.end(); it++)
	{
		if (!it->second.dwTimer)
		{
			it->second.dwTimer++;
		}else if(it->second.dwTimer > 1)
		{
			DelUser(it->second.dwLineID, it->first);
			m_mapPendingUser.erase(it);
		}

	}
}

bool gw_LineGroup::SwitchLineReq(DWORD dwOldLineID, DWORD dwNewLineID, DWORD dwUserID, int& iRegionID)
{
	SmartPtr<gw_Line> pOldLine = FindLine(dwOldLineID);
	SmartPtr<gw_Line> pNewLine = FindLine(dwNewLineID);

	if (pOldLine && pNewLine)
	{
		gw_Line::SLineUser* pOldLineUser = pOldLine->FindLineUser(dwUserID);

		if (pOldLineUser && pNewLine->m_dwLineUserCount < m_dwMaxUserPerLine)
		{
			if(pOldLine->DelUser(dwUserID) && pNewLine->AddUser(dwUserID))
			{
				SPendingUser stPendingUser;
				stPendingUser.dwLineID = dwNewLineID;
				stPendingUser.dwTimer = 0;
				m_mapPendingUser[dwUserID] = stPendingUser;

				iRegionID = pNewLine->m_iRegionID;
				return true;
			}
		}
	}

	return false;
}

bool gw_LineGroup::LeaveLineReq(DWORD dwLineID, DWORD dwUserID)
{
	return DelUser(dwLineID, dwUserID);
}

bool gw_LineGroup::LineInfoReq(std::list<SLineInfo>& listLineInfo)
{
	SLineInfo stLineInfo;

	stLineInfo.dwLineID = 0;
	stLineInfo.dwUserCount = 0;
	stLineInfo.dwMaxUserCount = m_dwMaxUserPerLine;


	std::map<DWORD, SmartPtr <gw_Line> >::iterator it;

	for (it = m_mapLine.begin(); it != m_mapLine.end(); it++)
	{
		if (it->second->m_bAcitve && it->second->m_dwLineUserCount)
		{
			stLineInfo.dwLineID = it->second->m_dwLineID;
			stLineInfo.dwUserCount = it->second->m_dwLineUserCount;
			listLineInfo.push_back(stLineInfo);
		}
	}

	return true;

}

bool gw_LineGroup::GetUserRegion(DWORD dwLineID, DWORD dwUserID, int& iRegionID)
{
	SmartPtr<gw_Line> pLine = FindLine(dwLineID);

	if (pLine)
	{
		gw_Line::SLineUser* pLineUser = pLine->FindLineUser(dwUserID);

		if (pLineUser)
		{	
			iRegionID = pLine->m_iRegionID;
			return true;
		}
	}

	return false;
}