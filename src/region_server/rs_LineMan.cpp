/////////////////////////////////
//Filename:   rs_LineMan.cpp
//Author:     rendh
//CreateTime: 2009-12-29
//Desc:		  分线系统RS管理器
////////////////////////////////
#include "region.h"
#include "rs_LineMan.h"
#include "rs_cmd_LineSys.h"
//#include "rs_cmd_LineSys.h"

rs_LineMan::rs_LineMan(void):
m_dwUserCount(0)
{

}

rs_LineMan::~rs_LineMan(void)
{

}

bool rs_LineMan::Startup()
{
	return true;
}

void rs_LineMan::Shutdown()
{
	m_listLineGroup.clear();
	m_dwUserCount = 0;
}

bool rs_LineMan::Run()
{
	return true;
}

bool rs_LineMan::CreateLine(int iRegionCount)
{
	//激活该服务器上的线
	int iRegionID = 0;

	SmartPtr<rs_LineGroup> pGroup = NULL;

	for (int i = 1; i <= g_TableLineSys.m_dwGroupCount; i++)
	{
		pGroup = RT_NEW rs_LineGroup;
		SLineSys* pstLineGroup = g_TableLineSys.Find(i);

		if (pGroup && pstLineGroup)
		{
			pGroup->SetParameter(pstLineGroup);

			for (int x=1; x <= pstLineGroup->iLineCount; x++)
			{
				if (iRegionID == g_cfgRs.lRegionID)
				{
					pGroup->AddLine(x, iRegionID);
				}

				iRegionID++;
				iRegionID %= iRegionCount;
			}
		}else
		{
			return false;
		}

		m_listLineGroup.push_back(pGroup);
	}

	return true;
}

bool rs_LineMan::EnterLine(DWORD dwGroupID, DWORD dwLineID, int iRegionID, CRegionUser* pUser, long* ppos)
{
	SmartPtr<rs_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->EnterLine(dwLineID, iRegionID, pUser, ppos);
	}

	return false;
}


bool rs_LineMan::LeaveLine(DWORD dwGroupID, DWORD dwLineID, CRegionUser* pUser, char* pstrMap, long* ppos)
{
	SmartPtr<rs_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		if (pGroup->LeaveLine(dwLineID, pUser, ppos))
		{
			//先发出离线确认
			CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineRes(dwGroupID, dwLineID, pUser->m_userId, pUser);

			if (strstr(pstrMap, "LineSys:Group"))
			{
				DWORD dwNewGroupID = atoi(pstrMap + 13);
				//发出新的入线请求
				CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(dwNewGroupID, pUser->m_userId, ppos);

				return true;

			}else if (strstr(pstrMap, "DefMap"))
			{
				//进入大地图
				if(!g_region->m_defScene->AddCreature(pUser->m_dummy, ppos, 0))
				{
					return false;
				}

				pUser->m_dummy->ForceSetPosition(ppos[0], ppos[1], ppos[2]);

				return true;
			}
		}
	}

	return false;
}

bool rs_LineMan::SwitchLine(DWORD dwGroupID, DWORD dwOldLineID, DWORD dwNewLineID, int iRegionID, CRegionUser* pUser)
{
	SmartPtr<rs_LineGroup> pGroup = FindGroup(dwGroupID);

	if (pGroup)
	{
		return pGroup->SwitchLine(dwOldLineID, dwNewLineID, iRegionID, pUser);
	}

	return false;
}

bool rs_LineMan::FindLine()
{
	return true;
}

SmartPtr<rs_LineGroup> rs_LineMan::FindGroup(DWORD dwGroupID)
{
	list<SmartPtr <rs_LineGroup> >::iterator Groupit;


	for (Groupit = m_listLineGroup.begin(); Groupit != m_listLineGroup.end(); Groupit++)
	{
		if ((*Groupit)->m_dwGroupID == dwGroupID)
		{
			return *Groupit;
		}

	}

	return NULL;
}

void rs_LineMan::OnTimer()
{

}


void rs_LineMan::Spawn()
{
	list<SmartPtr <rs_LineGroup> >::iterator Groupit;


	for (Groupit = m_listLineGroup.begin(); Groupit != m_listLineGroup.end(); Groupit++)
	{
		(*Groupit)->Spawn();
	}
}
