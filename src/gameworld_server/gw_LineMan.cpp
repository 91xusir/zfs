/////////////////////////////////
//Filename:   gw_LineMan.cpp
//Author:     rendh
//CreateTime: 2009-12-29
//Desc:		  分线系统GWS管理器
////////////////////////////////
#include "gw_server.h"
#include "gw_LineMan.h"
#include "gw_cmd_LineSys.h"

gw_LineMan::gw_LineMan(void):
m_dwUserCount(0)
{

}

gw_LineMan::~gw_LineMan(void)
{

}

bool gw_LineMan::Startup()
{

	int iRegionID = 0;
	SmartPtr<gw_LineGroup> pGroup = NULL;
	
	for (int i = 1; i <= g_TableLineSys.m_dwGroupCount; i++)
	{
		pGroup = RT_NEW gw_LineGroup;
		SLineSys* pstLineGroup = g_TableLineSys.Find(i);
		
		if (pGroup && pstLineGroup)
		{
			pGroup->SetParameter(pstLineGroup);

			for (int x=1; x <= pstLineGroup->iLineCount; x++)
			{
				pGroup->AddLine(x, iRegionID);

				iRegionID++;
				iRegionID %= g_cfgGW.LineSYS.dwRegionCount;
			}

		}else
		{
			return false;
		}

		m_listLineGroup.push_back(pGroup);

	}

	return true;
}

void gw_LineMan::Shutdown()
{
	m_listLineGroup.clear();
	m_dwUserCount = 0;
}

bool gw_LineMan::AddRegion(int irgID)
{

	CLineSysCmdHelper_GW::Send_g2r_LineSys_CreateLineReq(g_cfgGW.LineSYS.dwRegionCount);

	return true;
}

bool gw_LineMan::OnRegionDisconnected(int irgID)
{
	//下线所有该服务器上用户
	list<SmartPtr <gw_LineGroup> >::iterator Groupit;

	
	for (Groupit = m_listLineGroup.begin(); Groupit != m_listLineGroup.end(); Groupit++)
	{
		(*Groupit)->OnRegionDisconnected(irgID);
	}

	return true;
}

bool gw_LineMan::Run()
{
	return true;
}

void gw_LineMan::OnCreateLineRes(int irgID)
{
	//激活该服务器上的线
	list< SmartPtr <gw_LineGroup> >::iterator Groupit;


	for (Groupit = m_listLineGroup.begin(); Groupit != m_listLineGroup.end(); Groupit++)
	{
		(*Groupit)->OnCreateLineRes(irgID);
	}
}

bool gw_LineMan::EnterLineReq(DWORD dwGroupID, DWORD dwUserID, DWORD& dwLineID, int& iRegionID)
{
	SmartPtr<gw_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->EnterLineReq(dwUserID, dwLineID, iRegionID);
	}

	return false;
}

bool gw_LineMan::OnEnterLineRes(DWORD dwGroupID, DWORD dwUserID)
{

	SmartPtr<gw_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->OnEnterLineRes(dwUserID);
		m_dwUserCount++;
	}

	return false;
}

bool gw_LineMan::LineInfoReq(DWORD dwGroupID, std::list<gw_LineGroup::SLineInfo>& listLineInfo)
{
	SmartPtr<gw_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->LineInfoReq(listLineInfo);
	}

	return false;
}

bool gw_LineMan::LeaveLineReq(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID)
{
	SmartPtr<gw_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->LeaveLineReq(dwLineID, dwUserID);
		m_dwUserCount--;
	}

	return false;
}

bool gw_LineMan::OnLeaveLineRes(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID)
{
	return true;
}

bool gw_LineMan::SwitchLineReq(DWORD dwGroupID, DWORD dwOldLineID, DWORD dwNewLineID, DWORD dwUserID, int& iRegionID)
{
	SmartPtr<gw_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		if(pGroup->SwitchLineReq(dwOldLineID, dwNewLineID, dwUserID, iRegionID))
		{
			//发送成功消息
			return true;
		}
		//发送人满消息
		return false;
	}

	//发送失败消息
	return false;
}

bool gw_LineMan::FindLine()
{
	return true;
}

SmartPtr<gw_LineGroup> gw_LineMan::FindGroup(DWORD dwGroupID)
{
	list<SmartPtr <gw_LineGroup> >::iterator Groupit;


	for (Groupit = m_listLineGroup.begin(); Groupit != m_listLineGroup.end(); Groupit++)
	{
		if ((*Groupit)->m_dwGroupID == dwGroupID)
		{
			return *Groupit;
		}

	}

	return NULL;
}

void gw_LineMan::OnTimer()
{
	list<SmartPtr <gw_LineGroup> >::iterator Groupit;


	for (Groupit = m_listLineGroup.begin(); Groupit != m_listLineGroup.end(); Groupit++)
	{
		(*Groupit)->OnTimer();
	}
}

bool gw_LineMan::GetUserRegion(DWORD dwGroupID, DWORD dwLineID, DWORD dwUserID, int& iRegionID)
{
	SmartPtr<gw_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->GetUserRegion(dwLineID, dwUserID, iRegionID);
	}

	return false;
}
