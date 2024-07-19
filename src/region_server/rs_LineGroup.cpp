//-----------------------------------------------------------------------------
//created by rendh 2010.01.11
//-----------------------------------------------------------------------------
#include "region.h"
#include "rs_LineGroup.h"

rs_Line::rs_Line(void):
m_dwLineID(0),
m_iRegionID(0),
m_dwLineUserCount(0),
m_pscene(NULL)
{
}

rs_Line::~rs_Line(void)
{
	g_factory->DestroyObject(m_pscene);
	m_dwLineUserCount = 0;
	m_dwLineID = 0;
	m_iRegionID = 0;
}

bool rs_Line::SetParameter(SLineSys* pstLineGroup, DWORD dwLineID, int iRegionID)
{
	m_dwLineID = dwLineID;
	m_iRegionID = iRegionID;

	// 场景加载
	m_pscene = (CRegionScene*)g_factory->CreateObject(OBJECT_TYPE_SCENE);

	if (!m_pscene)
	{
		return false;
	}
	m_pscene->SetTimer(SCENE_TIMER_ITEM, 30*1000);
	m_pscene->SetSceneID(pstLineGroup->id);
	m_pscene->LoadSceneFile(pstLineGroup->sceneFile.c_str());
	// Still load script to create npc
	m_pscene->LoadScript(pstLineGroup->script.c_str());
	m_pscene->OnCreate();
	m_pscene->m_defPos[0] = pstLineGroup->bornPos[0];
	m_pscene->m_defPos[1] = pstLineGroup->bornPos[1];
	m_pscene->m_defPos[2] = pstLineGroup->bornPos[2];

	m_SpawnMgr.SetScene(m_pscene);
	m_SpawnMgr.SetLineSysSceneID(atoi(pstLineGroup->sceneFile.c_str() + 5));

	if(!m_SpawnMgr.LoadSpawnTable(R(RES_TABLE_LINE_SPAWN)))
	{
		LOG("Load line_spawn.csv error\n");
	}

	return true;
}

bool rs_Line::AddUser(CRegionUser* pUser, long* ppos)
{
	if (pUser==NULL || pUser->m_dummy==NULL)
	{
		return false;
	}


	if(pUser->m_dummy->m_scene)
	{
		pUser->m_dummy->m_scene->RemoveCreature(pUser->m_dummy->m_oId);
	}

	if(!m_pscene->AddCreature(pUser->m_dummy,ppos,0))
	{
		ERR2("Region: Add user to line error! pos error[%d][%d], add user to defPos\n",ppos[0],ppos[1]);

		if (!m_pscene->AddCreature(pUser->m_dummy, NULL,0))
		{
			ERR2("Region: Add user to line defPos error!\n");
			return false;
		}
		pUser->m_dummy->m_pos[0] = m_pscene->m_defPos[0];
		pUser->m_dummy->m_pos[1] = m_pscene->m_defPos[1];
		pUser->m_dummy->m_pos[2] = m_pscene->m_defPos[2];
		// notify client
		pUser->m_dummy->ForceSetPosition(m_pscene->m_defPos[0],m_pscene->m_defPos[1],m_pscene->m_defPos[2]);
	}else{

		pUser->m_dummy->m_pos[0] = ppos[0];
		pUser->m_dummy->m_pos[1] = ppos[1];
		pUser->m_dummy->m_pos[2] = ppos[2];

		// notify client
		pUser->m_dummy->ForceSetPosition(ppos[0],ppos[1],ppos[2]);
	}

	m_dwLineUserCount++;

	return true;
}

bool rs_Line::DelUser(CRegionUser* pUser)
{
	if (pUser==NULL || pUser->m_dummy==NULL)
	{
		return false; // 如果玩家由于某种原因下线有可能导致这边返回。这样的话玩家就没有退出
	}


	m_pscene->RemoveCreature(pUser->m_dummy->m_oId);



	if(pUser->m_dummy->m_scene)
	{
		ERR("rs_Line::leave scene not null!\n");
		pUser->m_dummy->m_scene->RemoveCreature(pUser->m_dummy->m_oId);
	}

	m_dwLineUserCount--;

	return true;
}


bool rs_Line::SwitchLine(CRegionUser* pUser, rs_Line* pNewLine, long* ppos)
{
	if (!pUser || !pNewLine)
	{
		return false;
	}	

	if (DelUser(pUser) && pNewLine->AddUser(pUser, ppos))
	{
		return true;
	}

	return false;
}

void rs_Line::Spawn()
{
	m_SpawnMgr.Spawn();
}

rs_LineGroup::rs_LineGroup(void):
m_dwGroupID(0),
m_dwGroupUserCount(0)
{
}

rs_LineGroup::~rs_LineGroup(void)
{
	m_mapLine.clear();
	m_dwGroupUserCount = 0;
	m_dwGroupID = 0;
}

bool rs_LineGroup::SetParameter(SLineSys* pstLineGroup)
{
	if (!pstLineGroup)
	{
		return false;
	}

	m_stLineGroup = *pstLineGroup;

	m_dwGroupID = m_stLineGroup.id;
	
	return true;
}

bool rs_LineGroup::AddLine(DWORD dwLineID, int iRegionID)
{
	if (FindLine(dwLineID))
	{
		return false;
	}

	SmartPtr<rs_Line> pLine(RT_NEW rs_Line);

	if (!pLine)
	{
		return false;
	}

	pLine->SetParameter(&m_stLineGroup, dwLineID, iRegionID);
	m_mapLine[dwLineID] = pLine;
	return true;
}

bool rs_LineGroup::DelLine(DWORD dwLineID)
{
	SmartPtr<rs_Line> pLine = FindLine(dwLineID);

	if (!pLine)
	{
		return false;
	}

	m_mapLine.erase(dwLineID);

	return true;
}

bool rs_LineGroup::AddUser(DWORD dwLineID, CRegionUser* pUser, long* ppos)
{
	SmartPtr<rs_Line> pLine = FindLine(dwLineID);

	if (!pLine)
	{
		return false;
	}

	if(!pLine->AddUser(pUser, ppos))
		return false;

	m_dwGroupUserCount++;

	return true;
}

bool rs_LineGroup::DelUser(DWORD dwLineID, CRegionUser* pUser)
{
	SmartPtr<rs_Line> pLine = FindLine(dwLineID);

	if (!pLine)
	{
		return false;
	}

	if(!pLine->DelUser(pUser))
		return false;

	m_dwGroupUserCount--;

	return true;
}

SmartPtr<rs_Line> rs_LineGroup::FindLine(DWORD dwLineID)
{
	std::map<DWORD, SmartPtr <rs_Line> >::iterator it = m_mapLine.find(dwLineID);

	if (it == m_mapLine.end())
	{
		return NULL;
	}

	return it->second;
}


bool rs_LineGroup::EnterLine(DWORD dwLineID, int iRegionID, CRegionUser* pUser, long* ppos)
{
	rs_Line* pLine = FindLine(dwLineID);

	if (!pLine && iRegionID != g_cfgRs.lRegionID)
	{
		//切服
		pUser->m_dummy->m_dwGroupID = m_dwGroupID;
		pUser->m_dummy->m_dwLineID = dwLineID;
		memcpy(pUser->m_dummy->m_pos, ppos, 12);

		pUser->m_regionId = iRegionID;
		g_region->m_gws.BeginSwitchRegion(pUser);
		
		return false;
	}
	
	return AddUser(dwLineID, pUser, ppos);
}

void rs_LineGroup::OnTimer()
{

}

bool rs_LineGroup::SwitchLine(DWORD dwOldLineID, DWORD dwNewLineID, int iRegionID, CRegionUser* pUser)
{
	rs_Line* pNewLine = FindLine(dwNewLineID);
	rs_Line* pOldLine = FindLine(dwOldLineID);

	if (!pNewLine && iRegionID != g_cfgRs.lRegionID)
	{
		if (pOldLine)
		{
			pOldLine->m_dwLineUserCount--;
			//切服
			pUser->m_dummy->m_dwLineID = dwNewLineID;
			pUser->m_regionId = iRegionID;
			g_region->m_gws.BeginSwitchRegion(pUser);
		}

	}
	else if (pOldLine && iRegionID == pOldLine->m_iRegionID)
	{
		return pOldLine->SwitchLine(pUser, pNewLine, pUser->m_dummy->m_pos);
	}

	return false;
}

bool rs_LineGroup::LeaveLine(DWORD dwLineID, CRegionUser* pUser, long* ppos)
{
	return DelUser(dwLineID, pUser);
}

void rs_LineGroup::Spawn()
{
	std::map<DWORD, SmartPtr <rs_Line> >::iterator it;

	for (it = m_mapLine.begin(); it != m_mapLine.end(); it++)
	{
		it->second->Spawn();
	}
}
