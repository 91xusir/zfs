#include "region.h"
#include "rs_national_war.h"
#include "region_spawn.h"
//PZH
#include "PlayerLogic.h"
//
#define NUM(array)			(sizeof(array) / sizeof((array[0])))
#define EMAX(lev)			((lev)/10 * 5)

CRsNwarMgr	g_WarfieldMgr;

int CRsNwarMgr::s_iMaxLastQuitTime = 300;

const short iHeroID[2][3] = 
{
	{1254, 1255, 1256},
	{1257, 1258, 1259}
};

const short iArmsNpcID[2][3] = 
{
	{1218, 1217, 1219},
	{1223, 1222, 1224}
};
 
bool SRsNWar::Serialize(CG_CmdPacket *cmd)
{
	return true;
}

CRsNwarMgr::CRsNwarMgr()
{
	CM_MEMPROTECTOR(m_szMsg, 256)

	m_pWarfield = RT_NEW SRsNWar;
	m_bFlagDestroyedZhou = false;
	m_bFlagDestroyedShang = false;
	m_bIsWaitingStop = false;
	m_bIsOpen = false;
	m_bCanTrans = false;
	m_pSpawnMgr = NULL;
}

CRsNwarMgr::~CRsNwarMgr()
{
	CM_MEMUNPROTECTOR(m_szMsg)

	if(m_pWarfield)
	{
		DEL_ONE(m_pWarfield);
	}
}

EArmsKind CRsNwarMgr::IsArmsNpc(short npcID)
{
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 3; j++)
			if(iArmsNpcID[i][j] == npcID) return (EArmsKind)j;

	return eNone;
}

bool CRsNwarMgr::Load(const char* file)
{
	SWarfieldInfo tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j < 0) return false;

	tmp.html        = csv[j+1]["描述"].Str();
	tmp.script      = csv[j+1]["脚本"].Str();
	tmp.sceneFile   = csv[j+1]["场景文件"].Str();
	tmp.lev			= atol(csv[j+1]["等级限制"].Str());
	
	char* pos = (char*)csv[j+1]["周出生点"].Str();
	char  delim[] = ";";
	char* k;

	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.bornPosZhou[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.bornPosZhou[2] = 0;
	}

	pos = (char*)csv[j+1]["周退出点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.exitPosZhou[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.exitPosZhou[2] = 0;
	}

	pos = (char*)csv[j+1]["商出生点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.bornPosShang[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.bornPosShang[2] = 0;
	}

	pos = (char*)csv[j+1]["商退出点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.exitPosShang[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.exitPosShang[2] = 0;
	}

	tmp.exitSceneBlockZhou = csv[j+1]["周退出场景"].Str();
	tmp.exitSceneBlockShang = csv[j+1]["商退出场景"].Str();

	pos = (char*)csv[j+1]["周最终复活点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.posZhou.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.posZhou.pos[2] = 0;
		tmp.posZhou.faction = FACTION_ZHOU;
	}

	pos = (char*)csv[j+1]["商最终复活点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.posShang.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.posShang.pos[2] = 0;
		tmp.posShang.faction = FACTION_SHANG;
	}

	pos = (char*)csv[j+1]["周控制复活点"].Str();
	if(pos)
	{
		for(k = strtok(pos, delim); k && strlen(k)>0; )
		{
			SRebirthPos tmpPos;
			tmpPos.npcID = atol(k);

			k = strtok(NULL, delim);

			//读取可选npc
			for(int i = 0; k && (i < 2); ++i)
			{
				for(int j = 0; k && (j < 2); ++j)
				{
					tmpPos.optionalNpcID[i][j] = atol(k);
					k = strtok(NULL, delim);
				}
			}

			for(int i = 0; k && (i < 2); i++)
			{
				tmpPos.pos[i] = atol(k);
				//tmp.pos1.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}

			tmpPos.pos[2] = 0;
			tmpPos.faction = FACTION_ZHOU;
			tmpPos.old_faction = FACTION_ZHOU;

			tmp.posTmp.push_back(tmpPos);
			
			//k = strtok(NULL, delim);
		}		
	}

	pos = (char*)csv[j+1]["商控制复活点"].Str();
	if(pos)
	{
		for(k = strtok(pos, delim); k && strlen(k)>0; )
		{
			SRebirthPos tmpPos;
			tmpPos.npcID = atol(k);

			k = strtok(NULL, delim);

			//读取可选npc
			for(int i = 0; k && (i < 2); ++i)
			{
				for(int j = 0; k && (j < 2); ++j)
				{
					tmpPos.optionalNpcID[i][j] = atol(k);
					k = strtok(NULL, delim);
				}
			}

			for(int i = 0; k && (i < 2); i++)
			{
				tmpPos.pos[i] = atol(k);
				//tmp.pos1.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}

			tmpPos.pos[2] = 0;
			tmpPos.faction = FACTION_SHANG;
			tmpPos.old_faction = FACTION_SHANG;

			tmp.posTmp.push_back(tmpPos);

			//k = strtok(NULL, delim);
		}
	}

	m_pWarfield->stInfo = tmp;
	return true;
}

bool CRsNwarMgr::Exit()
{
	return true;
}

void CRsNwarMgr::Run()
{
#ifndef _WARFIELD_
	return;
#endif
	if(m_bIsOpen || m_bIsWaitingStop)
	{
		if(m_bIsWaitingStop && m_bCanTrans)
		{
			time_t tNow = time(NULL);
			static time_t tLeave = tNow;
			
			if(tLeave + 10 < tNow)
			{
				TransUser();				
				DestroyWarfield();
				tLeave = tNow;
			}
		}

		//时间到了，关闭战场
		//if(m_pWarfield->tEndTime!=0 && m_pWarfield->tEndTime<=time(NULL) && m_pWarfield->pScene)
		if(m_bIsWaitingStop)
		{
			//DestroyWarfield();
		}else// if(m_bIsOpen)
		{	
			m_pWarfield->tRemainTime--;
			std::list<SRebirthPos>::iterator it = m_pWarfield->stInfo.posTmp.begin();
			for(; it != m_pWarfield->stInfo.posTmp.end(); ++it)
			{
				if(it->bKilled == true)
				{
					if(CreateRebirthNpc((SRebirthPos*)&(*it)))
					{
						it->bKilled = false;
					}
				}else
				{
					if(it->tBeAttacked != 0 && it->tBeAttacked + 3 * 60 <= time(NULL))
					{
						it->faction = it->old_faction;
						it->npcID = it->optionalNpcID[it->faction][0];
						RemoveNpc(it->objID);
						CreateRebirthNpc((SRebirthPos*)&(*it));
					}
				}
			}
		}
	}
}

bool CRsNwarMgr::IsRebirthNpc(short npcID)
{
	std::list<SRebirthPos>::iterator it = m_pWarfield->stInfo.posTmp.begin();
	for(; it != m_pWarfield->stInfo.posTmp.end(); ++it)
	{
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 2; j++)
			{
				if(it->optionalNpcID[i][j] == npcID)
					return true;
			}
		}
	}

	return false;
}

bool CRsNwarMgr::IsFlagNpc(short npcID)
{
	return (npcID == WARFLAG_ZHOU || npcID == WARFLAG_SHANG);
}

void CRsNwarMgr::AddScore(char faction, float score)
{
	if(score != WF_SCORE_MAX)
	{
		//如果分数达到极大,表示令旗被击毁
		if(faction == FACTION_ZHOU)
		{
			m_pWarfield->lScoreZhou += score;
		}else
		{
			m_pWarfield->lScoreShang += score;
		}
	}

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_addscore);
	cmd.WriteByte(faction);
	cmd.WriteFloat(score);
	
	g_region->m_gws.EndSend();
}

bool CRsNwarMgr::CreateWarfield()
{
	bool bCreate = true;

	m_pWarfield->pScene = (CRsSceneWarfield*)g_factory->CreateObject(OBJECT_TYPE_SCENE_WARFIELD);
	
	if(!m_pWarfield->pScene)
	{
		bCreate = false;
		return false;
	}
	
	m_pWarfield->pScene->SetTimer(SCENE_TIMER_WARFIELD_SPAWN,15*1000);
	if(!m_pWarfield->pScene->LoadSceneFile(m_pWarfield->stInfo.sceneFile.c_str()))
	{
		LOG1("Warfield Load Scene File [%s] Error\n", (char*)m_pWarfield->stInfo.sceneFile.c_str());
		bCreate = false;
		return false;
	}

	if(!m_pWarfield->pScene->LoadScript(m_pWarfield->stInfo.script.c_str()))
	{
		LOG1("Warfield Load Script [%s] Error\n", (char*)m_pWarfield->stInfo.script.c_str());
		bCreate = false;
		return false;
	}

	m_pWarfield->pScene->OnCreate();

	if(!bCreate)
	{
		CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
		cmd.WriteShort(r2g_warfield_start_rep);
		cmd.WriteLong(0);
		g_region->m_gws.EndSend();
		return false;
	}

	m_bFlagDestroyedZhou = false;
	m_bFlagDestroyedShang = false;
	m_bIsOpen = true;
	m_pWarfield->tRemainTime = 3 * 60 * 60;

	if(!m_pSpawnMgr) m_pSpawnMgr = RT_NEW CRegionSpawnMgr;
	else LOG("Unknown Error: SpawnMgr is not null\n");

	m_pSpawnMgr->SetScene(m_pWarfield->pScene);
	if(!m_pSpawnMgr->LoadSpawnTable(R(RES_TABLE_GZSPAWN)))
	{
		LOG("Load gz_spawn.csv error\n");
	}

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_start_rep);
	cmd.WriteLong(1);
	g_region->m_gws.EndSend();

	return true;
}

void CRsNwarMgr::FindRebirthPos(long* pos, CRegionCreature* cre)
{
	if(!pos || !cre) return;

	long tmpPos[3] = {0, 0, 0};
	std::list<SRebirthPos>::iterator it = m_pWarfield->stInfo.posTmp.begin();
	for(; it != m_pWarfield->stInfo.posTmp.end(); ++it)
	{
		if(it->faction != cre->m_core.Faction) continue;

		if(tmpPos[0] == 0 && tmpPos[1] == 0)
		{
			memcpy((void*)tmpPos, (void*)it->pos, sizeof(tmpPos));
		}
		if(Distance(tmpPos, cre) > Distance(it->pos, cre))
		{
			memcpy((void*)tmpPos, (void*)it->pos, sizeof(tmpPos));
		}
	}

	pos[0] = tmpPos[0];
	pos[1] = tmpPos[1];
	pos[2] = tmpPos[2];

	if(cre->m_core.Faction == FACTION_ZHOU)
	{
		if(Distance(tmpPos, cre) > Distance(m_pWarfield->stInfo.posZhou.pos, cre))
		{
			pos[0] = m_pWarfield->stInfo.posZhou.pos[0];
			pos[1] = m_pWarfield->stInfo.posZhou.pos[1];
			pos[2] = m_pWarfield->stInfo.posZhou.pos[2];
		}
	}else
	{
		if(Distance(tmpPos, cre) > Distance(m_pWarfield->stInfo.posShang.pos, cre))
		{
			pos[0] = m_pWarfield->stInfo.posShang.pos[0];
			pos[1] = m_pWarfield->stInfo.posShang.pos[1];
			pos[2] = m_pWarfield->stInfo.posShang.pos[2];
		}
	}
}

long CRsNwarMgr::Distance(long *pos, CRegionCreature* cre)
{
	float fX2, fY2;
	fX2 = cre->m_pos[0]-pos[0];
	fY2 = cre->m_pos[1]-pos[1];
	float fD = sqrt(fX2*fX2+fY2*fY2);
	return (long)fD;
}

void CRsNwarMgr::SendReport()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_warfield_report);

	if(m_bFlagDestroyedZhou)
	{	//击毁令旗获胜
		g_sendCmd->WriteLong(0);
		g_sendCmd->WriteByte(FACTION_SHANG);
	}else if(m_bFlagDestroyedShang)
	{	//击毁令旗获胜
		g_sendCmd->WriteLong(0);
		g_sendCmd->WriteByte(FACTION_ZHOU);
	}else
	{	//积分获胜
		g_sendCmd->WriteLong(1);
		if(m_pWarfield->lScoreZhou > m_pWarfield->lScoreShang)
		{
			g_sendCmd->WriteByte(FACTION_ZHOU);
		}else if(m_pWarfield->lScoreZhou < m_pWarfield->lScoreShang)
		{
			g_sendCmd->WriteByte(FACTION_SHANG);
		}else if(m_pWarfield->lScoreZhou == m_pWarfield->lScoreShang)
		{
			g_sendCmd->WriteByte(FACTION_UNKNOWN);
		}
	}
	g_sendCmd->WriteLong((long)m_pWarfield->listUserZhou.size());
	g_sendCmd->WriteFloat(m_pWarfield->lScoreZhou);
	g_sendCmd->WriteFloat(m_pWarfield->fRZhou);
	g_sendCmd->WriteLong((long)m_pWarfield->listUserShang.size());
	g_sendCmd->WriteFloat(m_pWarfield->lScoreShang);
	g_sendCmd->WriteFloat(m_pWarfield->fRShang);

	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[2]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[2]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[2]);

	g_sendCmd->WriteLong((long)m_pWarfield->listExploit.size());
	std::list<SExploit>::iterator it = m_pWarfield->listExploit.begin();
	for(; it != m_pWarfield->listExploit.end(); ++it)
	{
		it->SerializeToGC(g_sendCmd);
	}

	//CmdGiver->RecvCmd(g_sendCmd);
	std::list<DWORD>::iterator itUser = m_pWarfield->listUserZhou.begin();
	for(; itUser != m_pWarfield->listUserZhou.end(); ++itUser)
	{
		CRegionUser* pUser = g_region->FindUser(*itUser);
		if(!pUser || !pUser->m_dummy) continue;

		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	itUser = m_pWarfield->listUserShang.begin();
	for(; itUser != m_pWarfield->listUserShang.end(); ++itUser)
	{
		CRegionUser* pUser = g_region->FindUser(*itUser);
		if(!pUser || !pUser->m_dummy) continue;

		pUser->m_dummy->RecvCmd(g_sendCmd);
	}
}

void CRsNwarMgr::SendUpdateData()
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_warfield_view_rep);

	g_sendCmd->WriteLong((long)m_pWarfield->tRemainTime);
	g_sendCmd->WriteLong((long)m_pWarfield->listUserZhou.size());
	g_sendCmd->WriteFloat(m_pWarfield->lScoreZhou);
	g_sendCmd->WriteFloat(m_pWarfield->fRZhou);
	g_sendCmd->WriteLong((long)m_pWarfield->listUserShang.size());
	g_sendCmd->WriteFloat(m_pWarfield->lScoreShang);
	g_sendCmd->WriteFloat(m_pWarfield->fRShang);

	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[2]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[2]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[2]);

	g_sendCmd->WriteLong((long)m_pWarfield->listExploit.size());
	std::list<SExploit>::iterator it = m_pWarfield->listExploit.begin();
	for(; it != m_pWarfield->listExploit.end(); ++it)
	{
		it->SerializeToGC(g_sendCmd);
	}

	//CmdGiver->RecvCmd(g_sendCmd);
	std::list<DWORD>::iterator itUser = m_pWarfield->listUserZhou.begin();
	for(; itUser != m_pWarfield->listUserZhou.end(); ++itUser)
	{
		CRegionUser* pUser = g_region->FindUser(*itUser);
		if(!pUser || !pUser->m_dummy) continue;

		pUser->m_dummy->RecvCmd(g_sendCmd);
	}

	itUser = m_pWarfield->listUserShang.begin();
	for(; itUser != m_pWarfield->listUserShang.end(); ++itUser)
	{
		CRegionUser* pUser = g_region->FindUser(*itUser);
		if(!pUser || !pUser->m_dummy) continue;

		pUser->m_dummy->RecvCmd(g_sendCmd);
	}
}

void CRsNwarMgr::UpdateRate(float fRZhou, float fRShang)
{
	m_pWarfield->fRZhou = fRZhou;
	m_pWarfield->fRShang = fRShang;
}

void CRsNwarMgr::UpdateClientData(CRegionCreature* CmdGiver)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_warfield_view_rep);
	
	g_sendCmd->WriteLong((long)m_pWarfield->tRemainTime);
	g_sendCmd->WriteLong((long)m_pWarfield->listUserZhou.size());
	g_sendCmd->WriteFloat(m_pWarfield->lScoreZhou);
	g_sendCmd->WriteFloat(m_pWarfield->fRZhou);
	g_sendCmd->WriteLong((long)m_pWarfield->listUserShang.size());
	g_sendCmd->WriteFloat(m_pWarfield->lScoreShang);
	g_sendCmd->WriteFloat(m_pWarfield->fRShang);

	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lPriHero[2]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lInterHero[2]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[0]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[1]);
	g_sendCmd->WriteFloat(m_pWarfield->lSeniorHero[2]);

	//if((long)m_pWarfield->listExploit.size() > 5)
	//{
	//	g_sendCmd->WriteLong();
	//}

	g_sendCmd->WriteLong((long)m_pWarfield->listExploit.size());
	//m_pWarfield->listExploit.sort(std::greater<SExploit>());
	std::list<SExploit>::iterator it = m_pWarfield->listExploit.begin();
	for(; it != m_pWarfield->listExploit.end(); ++it)
	{
		//it->Serialize(&cmd);
		it->SerializeToGC(g_sendCmd);
	}

	CmdGiver->RecvCmd(g_sendCmd);
}

bool CRsNwarMgr::RemoveNpc(long lObjID)
{
	CRegionCreature* cre = m_pWarfield->pScene->FindCreature(lObjID);
	if(!cre) return false;

	cre->m_scene->RemoveCreature(lObjID);
	g_factory->DestroyObject(cre);
	//cre->Die(NULL);

	return true;
}

bool CRsNwarMgr::RemoveNpc(CRegionCreature* cre)
{
	if(!cre) return false;
	cre->m_scene->RemoveCreature(cre->m_oId);
	g_factory->DestroyObject(cre);
	//cre->Die(NULL);
	return true;
}

bool CRsNwarMgr::AddNpc(SRebirthPos* pos)
{
	if(!pos) return false;

	CRegionCreature *cre = g_region->CreateNpc(pos->npcID);
	if(!cre) 
	{
		LOG1("Cannot create rebirth npc [%d]\n", pos->npcID);
		return false;
	}

	if(m_pWarfield->pScene->AddCreature(cre, pos->pos, (char)255))
	{
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	else
	{
		g_factory->DestroyObject(cre);
		cre = NULL;
	}

	//cre->m_core.Faction = pos->faction;
	pos->bKilled = false;
	pos->objID	 = cre->m_oId;
	pos->tBeAttacked = 0;
	return true;
}

bool CRsNwarMgr::CreateRebirthNpc(SRebirthPos* pos)
{
	if(!pos) return false;

	CRegionCreature *cre = g_region->CreateNpc(pos->npcID);
	if(!cre) 
	{
		LOG1("Cannot create rebirth npc [%d]\n", pos->npcID);
		return false;
	}

	if(m_pWarfield->pScene->AddCreature(cre, pos->pos, (char)255))
	{
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	else
	{
		g_factory->DestroyObject(cre);
		cre = NULL;
	}

	//cre->m_core.Faction = pos->faction;
	pos->bKilled = false;
	pos->tBeAttacked = 0;

	return true;
}

bool CRsNwarMgr::CanConjureHero(char faction, int lev)
{
	if(faction == FACTION_UNKNOWN) return false;

	float lHero[3];

	if(lev == 1) memcpy((void*)lHero, (void*)m_pWarfield->lPriHero, sizeof(lHero));
	else if(lev == 2) memcpy((void*)lHero, (void*)m_pWarfield->lInterHero, sizeof(lHero));
	else if(lev == 3) memcpy((void*)lHero, (void*)m_pWarfield->lSeniorHero, sizeof(lHero));
	else return false;

	if(lHero[(int)faction] >= lHero[2])
	{
		return false;
	}

	return true;
}

EConjureRet CRsNwarMgr::ConjureHero(long dwUserID, long dwResource, int lev)
{
	if(!m_bIsOpen || m_bIsWaitingStop) return eFailed;

	CRegionUser* pUser = g_region->FindUser(dwUserID);
	if(!pUser || !pUser->m_dummy) return eFailed;
    
	//long lHero[3];

	//if(lev == 1) memcpy((void*)lHero, (void*)m_pWarfield->lPriHero, sizeof(lHero));
	//else if(lev == 2) memcpy((void*)lHero, (void*)m_pWarfield->lInterHero, sizeof(lHero));
	//else if(lev == 3) memcpy((void*)lHero, (void*)m_pWarfield->lSeniorHero, sizeof(lHero));
	//else return eFailed;

	float* lHero = NULL;

	if(lev == 1) lHero = (float*)m_pWarfield->lPriHero;
	else if(lev == 2) lHero = (float*)m_pWarfield->lInterHero;
	else if(lev == 3) lHero = (float*)m_pWarfield->lSeniorHero;
	else return eFailed;

	char faction = pUser->m_dummy->m_core.Faction;
	if(FACTION_UNKNOWN == faction) return eFailed;

	if(lHero[(int)faction] >= lHero[2])
	{
		return eHaveConjured;
	}

	float f = 1.0;
	if(FACTION_ZHOU == faction) f = m_pWarfield->fRZhou;
	else if(FACTION_SHANG == faction) f = m_pWarfield->fRShang;
	
	float fHScore = dwResource * f;
	lHero[(int)faction] += fHScore;

	if(lHero[(int)faction] >= lHero[2])
	{
		lHero[(int)faction] = lHero[2];
		OnConjureHero(faction, lev);
	}

	AddPerTmpExploit(dwUserID, dwResource);
	AddExploit(pUser->m_dummy->m_unionID, faction, dwResource);

	// 刷新到GW
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_conjure_progress);
	cmd.WriteByte(faction);
	cmd.WriteFloat(lHero[(int)faction]);
	cmd.WriteLong((long)lev);
	g_region->m_gws.EndSend();

	//为了实时更新客户端
	SendUpdateData();

	return eSuccess;
}

EConjureRet CRsNwarMgr::ConjureHero(char faction, long dwResource, int lev)
{
	if(!m_bIsOpen || m_bIsWaitingStop) return eFailed;

	if(FACTION_UNKNOWN == faction) return eFailed;

	float* lHero = NULL;

	if(lev == 1) lHero = (float*)m_pWarfield->lPriHero;
	else if(lev == 2) lHero = (float*)m_pWarfield->lInterHero;
	else if(lev == 3) lHero = (float*)m_pWarfield->lSeniorHero;
	else return eFailed;

	if(lHero[(int)faction] >= lHero[2])
	{
		return eHaveConjured;
	}

	lHero[(int)faction] += dwResource;

	if(lHero[(int)faction] >= lHero[2])
	{
		lHero[(int)faction] = lHero[2];
		OnConjureHero(faction, lev);
	}

	// 刷新到GW
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_conjure_progress);
	cmd.WriteByte(faction);
	cmd.WriteFloat(lHero[(int)faction]);
	cmd.WriteLong((long)lev);
	g_region->m_gws.EndSend();

	//为了实时更新客户端
	SendUpdateData();

	return eSuccess;
}

EConjureRet CRsNwarMgr::ConjureArms(long dwUserID, EArmsKind kind)
{
	CRegionUser* pUser = g_region->FindUser(dwUserID);
	if(!pUser || !pUser->m_dummy) return eFailed;

	if(pUser->m_dummy->GetGold() < iArmsPrice[kind])
	{
		return eNoMoney;
	}

	char faction = pUser->m_dummy->m_core.Faction;
	time_t now = time(NULL);

	std::list<SArms>::iterator it = m_pWarfield->listArms.begin();
	for(; it != m_pWarfield->listArms.end(); ++it)
	{
		if(it->faction != faction) continue;
		if(it->kind != kind) continue;

		//一小时之内
		if(it->tStartTime + WF_ONEHOUR > now)
		{
			if(it->lCurrNum >= iArmsRate[kind])
				return eOverFlow;
		}else
		{
			it->tStartTime = now;
			it->lCurrNum = 1;
			break;
		}
	}

	if(it == m_pWarfield->listArms.end())
	{
		SArms tmp;

		tmp.faction = faction;
		tmp.kind = kind;
		tmp.lCurrNum = 1;
		tmp.tStartTime = now;

		m_pWarfield->listArms.push_back(tmp);
	}

	OnConjureArms(pUser->m_dummy, faction, kind);

	return eSuccess;
}

void CRsNwarMgr::SaveDataToGW()
{
}

void CRsNwarMgr::OnConjureHero(char faction, int lev)
{
	if(FACTION_UNKNOWN == faction) return;
	if(lev <= 0 || lev > 3) return;

	CRegionCreature *cre = g_region->CreateNpc(iHeroID[(int)faction][lev-1]);
	if(!cre) 
	{
		LOG1("Cannot create hero npc [%d]\n", iHeroID[(int)faction][lev-1]);
		return;
	}

	long pos[3];

	if(FACTION_ZHOU == faction)
	{
		pos[0] = m_pWarfield->stInfo.posZhou.pos[0];
		pos[1] = m_pWarfield->stInfo.posZhou.pos[1];
		pos[2] = m_pWarfield->stInfo.posZhou.pos[2];
	}else
	{
		pos[0] = m_pWarfield->stInfo.posShang.pos[0];
		pos[1] = m_pWarfield->stInfo.posShang.pos[1];
		pos[2] = m_pWarfield->stInfo.posShang.pos[2];
	}

	if(m_pWarfield->pScene->AddCreature(cre, pos, (char)255))
	{
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	else
	{
		g_factory->DestroyObject(cre);
		cre = NULL;
	}

	cre->m_ai->SetLvFlag(AI_PATROL);
}

void CRsNwarMgr::OnConjureArms(CRegionCreature* pUser, char faction, EArmsKind kind)
{
	if(FACTION_UNKNOWN == faction) return;
	if(!pUser) return;
	if(!m_bIsOpen || m_bIsWaitingStop) return;

	CRegionCreature *cre = g_region->CreateNpc(iArmsNpcID[(int)faction][(int)kind]);
	if(!cre) 
	{
		LOG1("Cannot create arms npc [%d]\n", iArmsNpcID[(int)faction][(int)kind]);
		return;
	}

	if(m_pWarfield->pScene->AddCreature(cre, pUser->m_pos, (char)255))
	{
		if(cre->IsScriptLoaded())
			cre->CallScript("OnCreate", false);
	}
	else
	{
		g_factory->DestroyObject(cre);
		cre = NULL;
	}
}

void CRsNwarMgr::OnRebirthPosKilled(CRegionCreature* cre, CRegionCreature* killer)
{
	if(!cre || !cre->m_npcInfo || !killer) return;

	std::list<SRebirthPos>::iterator it = m_pWarfield->stInfo.posTmp.begin();
	for(; it != m_pWarfield->stInfo.posTmp.end(); ++it)
	{
		if(it->npcID == cre->m_npcInfo->Id)
		{
			if(killer->m_core.Faction == FACTION_UNKNOWN)
			{
				if(killer->m_userInfo)
					LOG1("Unknown error: [%d] faction unknown", killer->m_userInfo->m_userId);
				return;
			}

			LOG1("RebirthPos changed old: [%d]\n", (int)it->old_faction);

			it->faction = killer->m_core.Faction;
			it->old_faction = it->faction;
			it->bKilled = true;
			it->npcID = it->optionalNpcID[it->faction][0];

			LOG1("RebirthPos changed new : [%d]\n", (int)it->faction);

			cre->m_scene->RemoveCreature(cre->m_oId);
			g_factory->DestroyObject(cre);

			break;
		}
	}
}

void CRsNwarMgr::OnRebirthPosBeAttacked(CRegionCreature* cre)
{
	if(!cre || !cre->m_npcInfo) return;

	if(cre->m_core.Faction != FACTION_UNKNOWN)
	{
		std::list<SRebirthPos>::iterator it = m_pWarfield->stInfo.posTmp.begin();
		for(; it != m_pWarfield->stInfo.posTmp.end(); ++it)
		{
			if(it->npcID == cre->m_npcInfo->Id)
				break;
		}

		if(it == m_pWarfield->stInfo.posTmp.end())
		{
			//LOG1("Rebirth pos id error: [%d]", cre->m_npcInfo->Id);
			return;
		}

		it->tBeAttacked = time(NULL);

		if(it->faction != FACTION_UNKNOWN)
		{			
			std::string msg = cre->m_npcInfo->Name + R(MSG_NATIONAL_WAR_UNDER_ATTACK);
			BroadcastSystemMessage((char*)msg.c_str(), it->faction);
			
			//如果是第一次遭到攻击，变幻模型
			it->npcID = it->optionalNpcID[it->faction][1];
			//然后转成无效状态
			it->faction = FACTION_UNKNOWN;

			RemoveNpc(cre);						
			AddNpc((SRebirthPos*)&(*it));
			it->tBeAttacked = time(NULL);
		}
	}
}

void CRsNwarMgr::OnUserDead(CRegionCreature* cre)
{
	if(!cre) return;
	if(!cre->m_userInfo) return;
	if(/*PZH*//*cre->m_bDead == false*/(cre->GetPlayerLogic()->IsLive())/**/) return;

	//PZH
	//cre->m_bDead = false;
	cre->GetPlayerLogic()->ReLive();
	//
	long pos[3];

	FindRebirthPos(pos, cre);
	
	int ix, iy;
	if (!cre->m_scene->m_pTerrain->GetTerrainTile(pos[0], pos[1], ix, iy))
	{
		return;
	}

	ix += rand() % 40 - 20;
	iy += rand() % 40 - 20;

	int rx, ry;
	// R [11/22/2008 Louis.Huang]
	//if (!cre->m_scene->m_pPathFind->FindSpace(ix, iy, rx, ry, true))
	if (!cre->m_scene->m_pPathFind->CanAdd(ix, iy, rx, ry, true))
	{
		LOG2("CRsNwarMgr::OnUserDead: Can not find free space %d, %d\n", pos[0], pos[1]);
		return;
	}

	float fx, fy;
	cre->m_scene->m_pTerrain->GetPosByTerrainXY(rx, ry, fx, fy);
	cre->ForceSetPosition(fx, fy, 0);
}

const char* CRsNwarMgr::RequestEnter(CRegionUser *user,char Faction)
{
	char nFaction = Faction;
	
	user->m_dummy->m_core.Faction = nFaction;

	if(!user)
	{
		return "";
	}

	if(m_bIsWaitingStop)
	{
		return R(MSG_NATIONAL_WAR_FIELD_CLOSE);
	}

	if(!user->m_dummy)
	{
		return R(MSG_NATIONAL_WAR_USER_INFO_ERR);
	}

	if(user->m_dummy->m_core.Lev < m_pWarfield->stInfo.lev)
	{
		return R(MSG_NATIONAL_WAR_LEVEL_MUST);
	}

	if(user->m_dummy->GetPKState() == PK_STATE_RED_NAME)
	{
		return R(MSG_NATIONAL_WAR_RED_NAME);
	}

	time_t tNow = time(NULL);
	if (tNow<user->m_dummy->m_lWarfieldLastQuit +s_iMaxLastQuitTime)
	{
	}



	// send request to gameworld server
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_enter);
	cmd.WriteLong(user->m_userId);
	cmd.WriteByte(nFaction);
	g_region->m_gws.EndSend();

	return NULL;
}

bool CRsNwarMgr::RequestLeave(CRegionUser *user)
{
	if (user==NULL || user->m_dummy==NULL)
	{
		return false;
	}
	user->m_dummy->m_core.Faction = FACTION_NEUTRAL;
	// send request to gameworld server
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_leave);
	cmd.WriteLong(user->m_userId);
	g_region->m_gws.EndSend();
	return true;
}

bool CRsNwarMgr::OnNWarUserLeave(CRegionUser *user)
{
	if (user==NULL || user->m_dummy==NULL)
	{
		// 如果玩家由于某种原因下线有可能导致这边返回。这样的话玩家就没有退出
		// 由于无法判断玩家阵营因此在两大阵营列表中都删除一次此玩家
		//m_pWarfield->listUserZhou.remove(user->m_userId);
		//m_pWarfield->listUserShang.remove(user->m_userId);
		return false; 
	}

	if(m_pWarfield && m_pWarfield->pScene)
	{
		m_pWarfield->pScene->RemoveCreature(user->m_dummy->m_oId);
	}

	long pos[3];
	
	if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
	{
		memcpy((void*)pos, (void*)m_pWarfield->stInfo.exitPosZhou, sizeof(long) * 3);
		//m_pWarfield->listUserZhou.remove(user->m_userId);
		
	}else
	{
		memcpy((void*)pos, (void*)m_pWarfield->stInfo.exitPosShang, sizeof(long) * 3);
		//m_pWarfield->listUserShang.remove(user->m_userId);
	}

	if(!g_region->m_defScene->AddCreature(user->m_dummy, pos, 0))
	{
		LOG2("Leave Warfield Failed: Cannot Add Creature to DefScene pos[%d][%d]\n", pos[0], pos[1]);
		return false;
	}

	int ix, iy;
	if (!user->m_dummy->m_scene->m_pTerrain->GetTerrainTile(pos[0], pos[1], ix, iy))
		return false;

	ix += rand() % 40 - 20;
	iy += rand() % 40 - 20;

	int rx, ry;
	if (!user->m_dummy->m_scene->m_pPathFind->FindSpace(user->m_dummy->GetObjectId(), (short)user->m_dummy->m_type, ix, iy, rx, ry, true))
	{
		LOG2("CRsNwarMgr::OnNWarUserLeave: Can not find free space %d, %d\n", pos[0], pos[1]);
		return false;
	}

	float fx, fy;
	user->m_dummy->m_scene->m_pTerrain->GetPosByTerrainXY(rx, ry, fx, fy);
	user->m_dummy->ForceSetPosition(fx, fy, 0);

	user->m_dummy->m_bIsInWarfield = false;
	user->m_dummy->m_lWarfieldLastQuit = (long)time(NULL);

	if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
	{
		m_pWarfield->listUserZhou.remove(user->m_userId);
	}else
	{
		m_pWarfield->listUserShang.remove(user->m_userId);
	}
	return true;
}

void CRsNwarMgr::Reward()
{
	if(m_bFlagDestroyedZhou)
	{
		Reward(FACTION_SHANG);
	}else if(m_bFlagDestroyedShang)
	{
		Reward(FACTION_ZHOU);
	}else if(m_pWarfield->lScoreShang - m_pWarfield->lScoreZhou > 0.01)
	{
		Reward(FACTION_SHANG);
	}else if(m_pWarfield->lScoreZhou - m_pWarfield->lScoreShang > 0.01)
	{
		Reward(FACTION_ZHOU);
	}

	stlforeach(std::list<SPerExploit>, it, m_pWarfield->listPersonExploit)
	{
		AddPersonExploit(it->dwUserID, it->dwExploit);
	}
}

void CRsNwarMgr::Reward(char faction)
{
	if(faction == FACTION_ZHOU)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserZhou.begin();
		for(; it != m_pWarfield->listUserZhou.end(); ++it)
		{
			AddExp(*it, FACTION_ZHOU);
			LOG1("Warfield AddExp: [%d][zhou]\n", *it);
			CRegionUser* pUser = g_region->FindUser(*it);
			if(!pUser || !pUser->m_dummy) continue;

			AddPerTmpExploit((*it), pUser->m_dummy->m_core.Lev / 10 * 10, true);
		}

		std::list<DWORD>::iterator it2 = m_pWarfield->listUserShang.begin();
		for(; it2 != m_pWarfield->listUserShang.end(); ++it2)
		{
			AddExp((*it2), FACTION_SHANG);
			LOG1("Warfield AddExp: [%d][shang]\n", *it2);
		}
	}else
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserShang.begin();
		for(; it != m_pWarfield->listUserShang.end(); ++it)
		{
			AddExp((*it), FACTION_SHANG);
			LOG1("Warfield AddExp: [%d][shang]\n", *it);

			CRegionUser* pUser = g_region->FindUser(*it);
			if(!pUser || !pUser->m_dummy) continue;
			AddPerTmpExploit((*it), pUser->m_dummy->m_core.Lev / 10 * 10, true);
		}

		std::list<DWORD>::iterator it2 = m_pWarfield->listUserZhou.begin();
		for(; it2 != m_pWarfield->listUserZhou.end(); ++it2)
		{
			AddExp(*it2, FACTION_ZHOU);
			LOG1("Warfield AddExp: [%d][zhou]\n", *it2);
		}
	}
}

void CRsNwarMgr::AddPerTmpExploit(DWORD dwUserID, DWORD dwExploit, bool IsWinnerReward)
{
	CRegionUser* pUser = g_region->FindUser(dwUserID);
	if(!pUser || !pUser->m_dummy) return;

	DWORD dwSendExploit = 0;

	stlforeach(std::list<SPerExploit>, it, m_pWarfield->listPersonExploit)
	{
		if(it->dwUserID == dwUserID)
		{
			it->dwExploit += dwExploit;

			if(!IsWinnerReward)
			{
				int iLev = (int)pUser->m_dummy->m_core.Lev;
				it->dwExploit = (it->dwExploit > (DWORD)EMAX(iLev)) ? EMAX(iLev) : it->dwExploit;
			}

			dwSendExploit = it->dwExploit;
			break;
		}
	}

	if(it == m_pWarfield->listPersonExploit.end())
	{
		SPerExploit tmp;
		tmp.dwUserID = dwUserID;
		tmp.dwExploit = dwExploit;

		if(!IsWinnerReward)
		{
			int iLev = (int)pUser->m_dummy->m_core.Lev;
			tmp.dwExploit = (tmp.dwExploit > (DWORD)EMAX(iLev)) ? EMAX(iLev) : tmp.dwExploit;
		}

		dwSendExploit = tmp.dwExploit;

		m_pWarfield->listPersonExploit.push_back(tmp);
	}

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_tmp_per_exploit);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong(dwSendExploit);
	g_region->m_gws.EndSend();
}

void CRsNwarMgr::AddPersonExploit(DWORD dwUserID, DWORD dwExploit)
{
	//CRegionUser* pUser = g_region->FindUser(dwUserID);
	//if(!pUser || !pUser->m_dummy) return;

	//int iLev = (int)pUser->m_dummy->m_core.Lev;

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_add_per_exploit);
	cmd.WriteLong(dwUserID);
	cmd.WriteLong(dwExploit);
	g_region->m_gws.EndSend();
}

void CRsNwarMgr::AddExp(DWORD dwUserID, char faction)
{
	//CRegionUser* pUser = g_region->FindUser(dwUserID);
	//if(!pUser || !pUser->m_dummy) return;

	//const int iExp[4] = {40, 30, 20, 10};
	//int idx = 0;
	//int iLev = (int)pUser->m_dummy->m_core.Lev;

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_add_exp);
	cmd.WriteLong(dwUserID);
	cmd.WriteByte(faction);
	g_region->m_gws.EndSend();
}

void CRsNwarMgr::AddTmpExploit(DWORD dwUnionID, char faction, DWORD lExploit)
{
	if(dwUnionID == 0)
	{
		//LOG("Invalid UnionID\n");
		return;
	}
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(dwUnionID);
	if(iter_union == g_region->m_unionMap.end())
	{
		LOG1("AddExploit: Cannot Find Union [%d]\n", dwUnionID);
		return;
	}

	std::list<SExploit>::iterator it = m_pWarfield->listExploit.begin();
	for(; it != m_pWarfield->listExploit.end(); ++it)
	{
		if(it->dwUnionID != dwUnionID) continue;

		it->lExploit += lExploit;
		break;
	}

	if(it == m_pWarfield->listExploit.end())
	{
		SExploit tmp;

		tmp.dwUnionID = dwUnionID;
		tmp.faction = faction;
		tmp.lScore = 0;
		tmp.lExploit = lExploit;

		m_pWarfield->listExploit.push_back(tmp);
	}
}

void CRsNwarMgr::AddExploit(DWORD dwUnionID, char faction, long lExploit)
{
	if(!m_bIsOpen || m_bIsWaitingStop) return;

	if(dwUnionID == 0)
	{
		//LOG("Invalid UnionID\n");
		return;
	}
	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
	iter_union = g_region->m_unionMap.find(dwUnionID);
	if(iter_union == g_region->m_unionMap.end())
	{
		LOG1("AddExploit: Cannot Find Union [%d]\n", dwUnionID);
		return;
	}

	long lSendExploit = 0;
	
	std::list<SExploit>::iterator it = m_pWarfield->listExploit.begin();
	for(; it != m_pWarfield->listExploit.end(); ++it)
	{
		if(it->dwUnionID != dwUnionID) continue;

		it->lExploit += lExploit;
		lSendExploit = it->lExploit;
		break;
	}

	if(it == m_pWarfield->listExploit.end())
	{
		SExploit tmp;

		tmp.dwUnionID = dwUnionID;
		tmp.faction = faction;
		tmp.lScore = 0;
		tmp.lExploit = lExploit;
		lSendExploit = lExploit;

		m_pWarfield->listExploit.push_back(tmp);
	}

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_warfield_update_exploit);	
	cmd.WriteByte(faction);
	cmd.WriteLong((long)dwUnionID);
	cmd.WriteLong(lSendExploit);
	g_region->m_gws.EndSend();

	//为了实时更新
	//SendUpdateData();
}

void CRsNwarMgr::OnRecvUpdateData(long lRemain, long numZhou, std::list<DWORD>* pUserZhou, float lScoreZhou, long numShang, std::list<DWORD>* pUserShang, float lScoreSHang,
								  float* lPriHero, float* lInterHero, float* lSeniorHero,
								  std::list<SExploit>* pExploit, std::list<SPerExploit>* pPerExploit)
{	
	m_pWarfield->tRemainTime = lRemain;
	m_iNumZhou = numZhou;
	m_iNumShang = numShang;
	m_pWarfield->lScoreZhou = lScoreZhou;
	m_pWarfield->lScoreShang = lScoreSHang;

	memcpy((void*)m_pWarfield->lPriHero, (void*)lPriHero, sizeof(float) * 3);
	memcpy((void*)m_pWarfield->lInterHero, (void*)lInterHero, sizeof(float) * 3);
	memcpy((void*)m_pWarfield->lSeniorHero, (void*)lSeniorHero, sizeof(float) * 3);

	m_pWarfield->listExploit.clear();
	m_pWarfield->listExploit = *pExploit;

	m_pWarfield->listUserZhou.clear();
	m_pWarfield->listUserZhou = *pUserZhou;

	m_pWarfield->listUserShang.clear();
	m_pWarfield->listUserShang = *pUserShang;

	m_pWarfield->listPersonExploit.clear();
	m_pWarfield->listPersonExploit = *pPerExploit;
}

//void CRsNwarMgr::AddUnionScore(DWORD dwUnionID, char faction, long score)
//{
//	if(dwUnionID == 0 || faction == FACTION_UNKNOWN)
//	{
//		LOG("Invalid UnionID or faction\n");
//		return;
//	}
//	EXT_SPACE::hash_map<ULONG, SUnionData>::iterator iter_union;
//	iter_union = g_region->m_unionMap.find(dwUnionID);
//	if(iter_union == g_region->m_unionMap.end())
//	{
//		LOG1("AddUnionScore: Cannot Find Union [%d]\n", dwUnionID);
//		return;
//	}
//
//	std::list<SExploit>::iterator it = m_pWarfield->listExploit.begin();
//	for(; it != m_pWarfield->listExploit.end(); ++it)
//	{
//		if(it->dwUnionID != dwUnionID) continue;
//		if(it->faction != faction)
//		{
//			LOG("Fatal Error: Same UnionID, Differ Faction\n");
//			return;
//		}
//
//		it->lScore += score;
//	}
//
//	if(it == m_pWarfield->listExploit.end())
//	{
//		SExploit tmp;
//
//		tmp.dwUnionID = dwUnionID;
//		tmp.faction = faction;
//		tmp.lScore = score;
//		tmp.lExploit = 0;
//
//		m_pWarfield->listExploit.push_back(tmp);
//	}
//
//	AddScore(faction, score);
//}

void CRsNwarMgr::AddUserScore(DWORD dwUserID, long res)
/*
 * res : 资源数量
 */
{
	if(!m_bIsOpen || m_bIsWaitingStop) return;

	CRegionUser* pUser = g_region->FindUser(dwUserID);
	if(!pUser || !pUser->m_dummy) return;

	float fR = 1.0;
	float fScore = 1.0;

	if(pUser->m_dummy->m_core.Faction == FACTION_ZHOU) fR = m_pWarfield->fRZhou;
	else if(pUser->m_dummy->m_core.Faction == FACTION_SHANG) fR = m_pWarfield->fRShang;
	
	fScore = fR * res;
	AddScore(pUser->m_dummy->m_core.Faction, fScore);
	
	AddPerTmpExploit(dwUserID, res);
	AddExploit(pUser->m_dummy->m_unionID, pUser->m_dummy->m_core.Faction, res);

	//为了实时更新
	SendUpdateData();
}

void CRsNwarMgr::Stop()
{
	BroadcastSystemMessage((char*)R(MSG_NATIONAL_WAR_WILL_CLOSE));
	
	m_bIsOpen				= false;
	m_bIsWaitingStop		= true;
	m_bCanTrans				= true;
	m_pWarfield->tEndTime	= time(NULL) + 5 * 60; // 300秒后关闭

	LOG("Received msg: Stop Warfield\n");
	LOG1("Warfield will stop at %s\n", ctime(&m_pWarfield->tEndTime));
	
	//战场结束，奖励
	SendReport();
	Reward();

	//TransUser();	
}

void CRsNwarMgr::TransUser()
{
	// 传送玩家离开战场
	// 每次各阵营传送10个
	CRegionUser *user;
	std::list<DWORD>::iterator it = m_pWarfield->listUserZhou.begin();
	for(int i = 0; it!=m_pWarfield->listUserZhou.end() && i < 10; ++it, ++i)
	{
		user = g_region->FindUser(*it);
		if (user)
		{
			if(user->m_dummy && (OB_TYPE(user->m_dummy->m_scene) == OBJECT_TYPE_SCENE_WARFIELD))
			{
				RequestLeave(user);
			}
		}else
		{
		}
	}

	it = m_pWarfield->listUserShang.begin();
	for(int i = 0; it!=m_pWarfield->listUserShang.end() && i < 10; ++it, ++i)
	{
		user = g_region->FindUser(*it);
		if (user)
		{
			if(user->m_dummy && (OB_TYPE(user->m_dummy->m_scene) == OBJECT_TYPE_SCENE_WARFIELD))
			{
				RequestLeave(user);
			}
		}else
		{
		}
	}
}

void CRsNwarMgr::OnFlagBeAttacked(long flag_id, CRegionCreature* cre)
{
	if(m_bIsWaitingStop) return;
	if(!cre || !cre->m_npcInfo) return;

	time_t tNow = time(NULL);
	static time_t tZhou = tNow;
	static time_t tShang = tNow;

	char faction = FACTION_UNKNOWN;

	if(flag_id == WARFLAG_ZHOU)
	{		
		faction = FACTION_ZHOU;		
	}else
	{
		faction = FACTION_SHANG;
	}

	if(FACTION_ZHOU == faction)
	{
		if(tZhou + 5 < tNow)
		{
			std::string msg = cre->m_npcInfo->Name + R(MSG_NATIONAL_WAR_UNDER_ATTACK);
			BroadcastSystemMessage((char*)msg.c_str(), faction);
			tZhou = tNow;
		}
	}else if(FACTION_SHANG == faction)
	{
		if(tShang + 5 < tNow)
		{
			std::string msg = cre->m_npcInfo->Name + R(MSG_NATIONAL_WAR_UNDER_ATTACK);
			BroadcastSystemMessage((char*)msg.c_str(), faction);
			tShang = tNow;
		}
	}
}

void CRsNwarMgr::DestroyWarfield()
{
	if(m_pWarfield->pScene == NULL)
	{
		m_pWarfield->Reset();
		m_bCanTrans = false;
		return;
	}
	
	if(m_pWarfield->pScene->m_userHash.size()==0)
	{
		if(m_pSpawnMgr)
		{
			DEL_ONE(m_pSpawnMgr);
			m_pSpawnMgr = NULL;
		}
		
		m_pWarfield->pScene->Exit();
		g_factory->DestroyObject(m_pWarfield->pScene);
		m_pWarfield->pScene = NULL;

		CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
		cmd.WriteShort(r2g_warfield_stop_rep);
		cmd.WriteLong(1);
		g_region->m_gws.EndSend();

		m_bIsOpen				= false;
		m_bIsWaitingStop		= false;
		m_bCanTrans				= false;
		m_bFlagDestroyedZhou	= false;
		m_bFlagDestroyedShang	= false;
		m_pWarfield->Reset();
		Load(R(RES_WARFIELD));
	}else
	{
		LOG("There are users in Warfield Scene, Waiting for stop\n");
	}
}

void CRsNwarMgr::TransmitMsg(long dwUserID, long iMsgID)
{
	CRegionUser* pUser =  g_region->FindUser(dwUserID);
	if(!pUser)
	{
		LOG1("Cannot find user [%d]\n", dwUserID);
		return;
	}

	if(!pUser->m_dummy)
	{
		LOG("User info error\n");
		return;
	}

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_warfield_msg);
	g_sendCmd->WriteLong(iMsgID);
	pUser->m_dummy->RecvCmd(g_sendCmd);
}

void CRsNwarMgr::OnRecvAddScore(float lScoreZhou, float lScoreShang)
{
	m_pWarfield->lScoreZhou += lScoreZhou;
	m_pWarfield->lScoreShang += lScoreShang;
}

void CRsNwarMgr::OnFlagIsDestroy(long flag_id)
{
	if(m_bIsWaitingStop) return;

	m_bIsWaitingStop = true;

	if(flag_id == WARFLAG_ZHOU)
	{		
		m_bFlagDestroyedZhou = true;
		AddScore(FACTION_SHANG, WF_SCORE_MAX);		
	}else
	{
		m_bFlagDestroyedShang = true;
		AddScore(FACTION_ZHOU, WF_SCORE_MAX);
	}
}

bool CRsNwarMgr::IsInWarfield(CRegionCreature* p_cre)
{
	if(p_cre == NULL) return false;
	return (OB_TYPE(p_cre->m_scene) == OBJECT_TYPE_SCENE_WARFIELD);
}

void CRsNwarMgr::BroadcastSystemMessage(char* szMessage, char faction)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_chat);
	g_sendCmd->WriteByte(CHAT_CHANNEL_SYSTEM);
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteString((char*)R(MSG_NATIONAL_WAR_FIELD));
	g_sendCmd->WriteString((char*)szMessage);

	CRegionUser *user;

	if(faction == FACTION_ALL || faction == FACTION_ZHOU)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserZhou.begin();
		for (; it!=m_pWarfield->listUserZhou.end(); it++)
		{
			user = g_region->FindUser(*it);
			if (user)
			{
				if (user->m_dummy)
				{
					user->m_dummy->RecvCmd(g_sendCmd);
				}
			}
		}
	}

	if(faction == FACTION_ALL || faction == FACTION_SHANG)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserShang.begin();
		for (; it!=m_pWarfield->listUserShang.end(); it++)
		{
			user = g_region->FindUser(*it);
			if (user)
			{
				if (user->m_dummy)
				{
					user->m_dummy->RecvCmd(g_sendCmd);
				}
			}
		}
	}
}

void CRsNwarMgr::BroadcastMessage(char* szMessage, BYTE channel, long from_id, const char* from_name, char faction)
{
	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_chat);
	g_sendCmd->WriteByte(channel);
	g_sendCmd->WriteLong(from_id);
	g_sendCmd->WriteString((char*)from_name);
	g_sendCmd->WriteString((char*)szMessage);

	CRegionUser *user;

	if(faction == FACTION_ALL || faction == FACTION_ZHOU)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserZhou.begin();
		for (; it!=m_pWarfield->listUserZhou.end(); it++)
		{
			user = g_region->FindUser(*it);
			if (user)
			{
				if (user->m_dummy)
				{
					user->m_dummy->RecvCmd(g_sendCmd);
				}
			}
		}
	}

	if(faction == FACTION_ALL || faction == FACTION_SHANG)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserShang.begin();
		for (; it!=m_pWarfield->listUserShang.end(); it++)
		{
			user = g_region->FindUser(*it);
			if (user)
			{
				if (user->m_dummy)
				{
					user->m_dummy->RecvCmd(g_sendCmd);
				}
			}
		}
	}
}

bool CRsNwarMgr::AddUserToNWar(DWORD userId, char cBorn, long x, long y)
{
	//
	return true;
}

bool CRsNwarMgr::_AddUserToNWar(CRegionUser *user, char cBorn, long x, long y)
{
	//return false;
	if (user==NULL || user->m_dummy==NULL)
	{
		return false;
	}

	LOG2("_AddUserToNWar: [%d][%d]\n", x, y);

	long pos[3];
	if (cBorn)
	{
		if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
		{
			pos[0] = m_pWarfield->stInfo.bornPosZhou[0];
			pos[1] = m_pWarfield->stInfo.bornPosZhou[1];
			pos[2] = m_pWarfield->stInfo.bornPosZhou[2];
		}else
		{
			pos[0] = m_pWarfield->stInfo.bornPosShang[0];
			pos[1] = m_pWarfield->stInfo.bornPosShang[1];
			pos[2] = m_pWarfield->stInfo.bornPosShang[2];
		}
	}else
	{
		pos[0] = x;
		pos[1] = y;
		pos[2] = 0;
	}

	LOG2("_AddUserToNWar Result: [%d][%d]\n", pos[0], pos[1]);

	if(user->m_dummy->m_scene)
	{
		user->m_dummy->m_scene->RemoveCreature(user->m_dummy->m_oId);
	}

	if(!m_pWarfield->pScene->AddCreature(user->m_dummy,pos,0))
	{
		LOG2("Region: Add user to warfield error! pos [%d][%d]\n",pos[0],pos[1]);
		return false;
	}

	// notify client
	user->m_dummy->ForceSetPosition(pos[0],pos[1],pos[2]); 

	if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserZhou.begin();
		for(; it != m_pWarfield->listUserZhou.end(); ++it)
		{
			if(*it == user->m_userId) break;
		}

		if(it == m_pWarfield->listUserZhou.end())
		{
			m_pWarfield->listUserZhou.push_back(user->m_userId);
		}
	}else if(user->m_dummy->m_core.Faction == FACTION_SHANG)
	{
		std::list<DWORD>::iterator it = m_pWarfield->listUserShang.begin();
		for(; it != m_pWarfield->listUserShang.end(); ++it)
		{
			if(*it == user->m_userId) break;
		}

		if(it == m_pWarfield->listUserShang.end())
		{
			m_pWarfield->listUserShang.push_back(user->m_userId);
		}
	}

	user->m_dummy->m_bIsInWarfield = true;

	return true;
}

CRegionUser *CRsNwarMgr::RemoveUser(CRegionUser *user)
{
	return NULL;
}
