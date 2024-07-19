#include "gw_server.h"
#include "gw_union.h"
#include "gw_cmd_union.h"
#include "union_net_define.h"
#include "game_team_share_const.h"
#include <sstream>

class CUnion;
class CUnionMgr;
extern void BroadcastUnion(CUnion *pUnion);

#define SAFE_READ(p)		if(!(p)) return eOkay;

CUnionMgr g_UnionMgr;

CUnionMgr* GetUnionMgr()
{
	return &g_UnionMgr;
}

CUnion::CUnion(CUnionMgr *pUnionMgr, const SUnionData &unionData)
{
	CHECK(pUnionMgr);
	CHECK(unionData.m_nCaptainID!=0);

	m_pUnionMgr = pUnionMgr;
	m_core		= unionData;
}

CUnion::~CUnion()
{
}

SUnionCitizen* CUnion::Find(DWORD playerID)
{
	return m_core.Find(playerID);
}

bool CUnion::AddCitizen(DWORD playerID, short level, short metier, BYTE bOnline /*= true*/, EUnionOfficial official /*= UO_Guarder*/)
{
	bool bRet = false;
	do 
	{
		GWUser* pUser = g_server.FindActorById(playerID);
		if (NULL == pUser)
		{
			break;
		}

		if ((int)m_core.vecUnionCitizen.size()>=UNION_MAX_CITIZEN/*[m_core.level]*/)
		{
			break;
		}
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
	
		rt2_snprintf(
			cmd.command, GW_MAX_DB_COMMAND_LENGTH, 
			"INSERT INTO T_union_citizen_back(userID, official, UnionID, Contribution)  "
			"VALUES(%d, %d, %d, %d)",
			playerID, (int)official, GetID(), 0, 0
			);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			break;
		}
		mysql_free_result(pDBResult);

		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "UPDATE community SET "
			"GuildID = %d "
			"WHERE UserID=%d",
			GetID(),
			playerID);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			break;
		}
		
		mysql_free_result(pDBResult);

		SUnionCitizen newCitizen;
		newCitizen.m_nPlayerID		= playerID;
		newCitizen.m_nRegionID		= pUser->regionID;
		newCitizen.m_bOnLine		= bOnline;
		newCitizen.m_eUnionOfficial = official;
		newCitizen.m_strPlayerName = pUser->name;
		newCitizen.m_sLevel			= level;
		newCitizen.m_cMetier		= metier;
		newCitizen.m_nContribution	= 0;

		m_core.AddCitizen(newCitizen);

		pUser->unionID = m_core.m_nID;

		// 判断人数是否多于最少要求
		if (CitizenCount() >= UNION_MIN_CITIZEN)
		{
			rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "UPDATE T_union SET "
				"ToBeDeletedBegin = %s "
				"WHERE ID=%d",
				"'0000-00-00 00:00:00'",
				GetID());
			if ( g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
			{
				mysql_free_result(pDBResult);
			}
		}
		AddWarScores(pUser->WarScores);	
		bRet = true;
	} while (false);
	return bRet;
//	GWUser* pUser = g_server.FindActorById(playerID);
//	BOOL_ENSURE(pUser)
//
//	if ((int)m_core.vecUnionCitizen.size()>=UNION_MAX_CITIZEN/*[m_core.level]*/)
//	{
//		return false;
//	}
//
//	GWDBCommand cmd;
//	MYSQL_RES *pDBResult;
//// 	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "INSERT INTO T_union_citizen(userID, official, name, nickname, UnionID) VALUES("
//// 		"%d,"
//// 		"%d,"
//// 		"'%s',"
//// 		"'%s',"
//// 		"%d )",
//// 		playerID,
//// 		(int)official,
//// 		(char*)pUser->name.c_str(),
//// 		"",
//// 		GetID());
//	rt2_snprintf(
//				cmd.command, GW_MAX_DB_COMMAND_LENGTH, 
//				"INSERT INTO T_union_citizen_back(userID, official, UnionID, Contribution)  "
//				"VALUES(%d, %d, %d, %d)",
//				playerID, (int)official, GetID(), 0, 0
//			);
//	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
//	{
//		return false;
//	}
//	else
//	{
//		mysql_free_result(pDBResult);
//
//		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "UPDATE community SET "
//			"GuildID = %d "
//			"WHERE UserID=%d",
//			GetID(),
//			playerID);
//		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
//		{
//			return false;
//		}
//		else
//		{
//			mysql_free_result(pDBResult);
//
//			SUnionCitizen newCitizen;
//			newCitizen.m_nPlayerID		= playerID;
//			newCitizen.m_nRegionID		= pUser->regionID;
//			newCitizen.m_bOnLine		= bOnline;
//			newCitizen.m_eUnionOfficial = official;
//			newCitizen.m_strPlayerName = pUser->name;
// 			newCitizen.m_sLevel			= level;
//			newCitizen.m_cMetier		= metier;
//			newCitizen.m_nContribution	= 0;
//
//			m_core.AddCitizen(newCitizen);
//
//			pUser->unionID = m_core.m_nID;
//
//			// 判断人数是否多于最少要求
//			if (CitizenCount() >= UNION_MIN_CITIZEN)
//			{
//				rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "UPDATE T_union SET "
//					"ToBeDeletedBegin = %s "
//					"WHERE ID=%d",
//					"'0000-00-00 00:00:00'",
//					GetID());
//				if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
//				{
//				}
//				else
//				{
//				}
//			}
//			return true;
//		}
//	}
}

bool CUnion::RemoveCitizen(DWORD playerID)
{
	bool bRet = false;
	do 
	{
		GWUser* pUser = g_server.FindActorById(playerID);
		if (NULL == pUser)
		{
			break;
		}
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "DELETE FROM T_union_citizen_back WHERE userID=%d", playerID);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			break;
		}
		
		mysql_free_result(pDBResult);

		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "UPDATE community SET "
			"GuildID = 0 "
			"WHERE UserID=%d",
			playerID);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			break;
		}
		
		mysql_free_result(pDBResult);

		m_core.RemoveCitizen(playerID);
		if (pUser)
			pUser->unionID = 0;

		// 判断人数是否多于最少要求
		if (CitizenCount() < UNION_MIN_CITIZEN)
		{
			rt2_sprintf(cmd.command, "UPDATE T_union SET "
				"ToBeDeletedBegin = FROM_UNIXTIME(%u) "
				"WHERE ID=%d",
				time(NULL),
				GetID());
			if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
			{
				mysql_free_result(pDBResult);
			}
		}
		AddWarScores(-pUser->WarScores);	
		bRet = true;
	} while (false);
	return bRet;
//	GWUser* pUser = g_server.FindActorById(playerID);
//	//BOOL_ENSURE(pUser)
//
//	GWDBCommand cmd;
//	MYSQL_RES *pDBResult;
//// 	rt2_sprintf(cmd.command, "DELETE FROM T_union_citizen WHERE userID = "
//// 		"%d",
//// 		playerID);
//	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "DELETE FROM T_union_citizen_back WHERE userID=%d", playerID);
//	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
//	{
//		return false;
//	}
//	else
//	{
//		rt2_sprintf(cmd.command, "UPDATE community SET "
//			"GuildID = 0 "
//			"WHERE UserID=%d",
//			playerID);
//		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
//		{
//			return false;
//		}
//		else
//		{
//			mysql_free_result(pDBResult);
//
//			m_core.RemoveCitizen(playerID);
//			if (pUser)
//				pUser->unionID = 0;
//
//			// 判断人数是否多于最少要求
//			if (CitizenCount() < UNION_MIN_CITIZEN)
//			{
//				rt2_sprintf(cmd.command, "UPDATE T_union SET "
//					"ToBeDeletedBegin = FROM_UNIXTIME(%u) "
//					"WHERE ID=%d",
//					time(NULL),
//					GetID());
//				if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
//				{
//					
//				}
//				else
//				{
//				}
//			}
//			return true;
//		}
//	}
}

bool CUnion::SetCitizenNickName(DWORD playerID, char* NickName)
{
// 	SUnionCitizen *pCitizen = Find(playerID);
// 	if (pCitizen)
// 	{
// 		string tNickName;
// 		string _NickName = NickName;
// 		g_server.m_database.EscapeString(tNickName, _NickName);
// 
// 		GWDBCommand cmd;
// 		MYSQL_RES *pDBResult;
// 		rt2_sprintf(cmd.command, "UPDATE T_union_citizen SET "		
// 			"nickname = '%s' "
// 			"WHERE userID = %d LIMIT 1",
// 			(char*)tNickName.c_str(),
// 			playerID);
// 		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
// 		{
// 			return false;
// 		}
// 		else
// 		{
// 			mysql_free_result(pDBResult);
// 
// 			pCitizen->m_strNickName = NickName;
// 			return true;
// 		}
// 	}
// 	else 
		return false;
}

bool CUnion::UpdateLevel()
{
	//if (GetData()->level == UNION_MAX_LEVEL)
	//	return false;

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "UPDATE T_union SET "		
		"Level = %d "
		"WHERE ID = %d LIMIT 1",
		/*GetData()->level+*/1,
		GetID());
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return false;
	}
	else
	{
		mysql_free_result(pDBResult);

// 		GetData()->level++;
		return true;
	}
}

bool CUnion::AddScore(int count)
{
	//int scoreOrig = m_core.score;

	//if (abs(count) > UNION_MAX_SCORE)
		return false;
// 	if (count>=0)
// 	{
// 		m_core.score += count;
// 		if (m_core.score > UNION_MAX_SCORE)
// 			m_core.score = UNION_MAX_SCORE;
// 	}
// 	else 
// 	{
// 		m_core.score -= count;
// 		if (m_core.score < 0)
// 			m_core.score = 0;
// 	}
// 	
// 	GWDBCommand cmd;
// 	MYSQL_RES *pDBResult;
// 	rt2_sprintf(cmd.command, "UPDATE T_union SET "		
// 		"Score = %d "
// 		"WHERE ID = %d LIMIT 1",
// 		GetData()->score,
// 		GetID());
// 	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
// 	{
// 		m_core.score = scoreOrig;
// 		return false;
// 	}
// 	else
// 	{
// 		mysql_free_result(pDBResult);
// 		return true;
// 	}
}

bool CUnion::SetCaptain(DWORD playerID)
{
	SUnionCitizen *pCitizen = Find(playerID);
	if (pCitizen)
	{
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_sprintf(cmd.command, "UPDATE T_union SET "		
			"CaptainID = %d "
			"WHERE ID = %d LIMIT 1",
			playerID,
			GetID());
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			return false;
		}
		else
		{
			mysql_free_result(pDBResult);

			m_core.m_nCaptainID = playerID;
			Appoint(playerID, UO_King);
			return true;
		}
	}
	else 
        return false;
}

bool CUnion::SetName(string name)
{
	m_core.m_strName = name;
	return true;
}

bool CUnion::SetIcon(char* data, int dataLen, DWORD &outIconID)
{
	static DWORD s_lastSetIconTime = 0;
	DWORD now = time(NULL);
	if (now == s_lastSetIconTime)
	{
		return false;
	}
	s_lastSetIconTime = now;

	char *escapeData = RT_NEW char[dataLen*2+1];
	int iLen = mysql_escape_string(escapeData, data, dataLen);
	escapeData[iLen] = 0;

	GWDBCommand cmd;
	MYSQL_RES* res;
	DWORD iNewID = now;
	//rt2_sprintf(cmd.command, "SELECT MAX(Icon) FROM T_union");
	//if (g_server.m_database.ExecCommand(cmd.command, &res) )
	//{
	//	int count = mysql_num_rows(res);
	//	if (count==0)	return false;
	//	MYSQL_ROW row = mysql_fetch_row(res);
	//	iNewID = atoi(row[0]) + 1;
	//	mysql_free_result(res);
	//}
	//else
	//{
	//	return false;
	//}

	rt2_sprintf(cmd.command, "UPDATE T_union SET "
		"Icon=%d, "
		"IconData='%s' "
		"WHERE ID=%d",
		iNewID,
		escapeData,
		GetID());
	if ( !g_server.m_database.ExecCommand(cmd.command, &res) )
	{
		return false;
	}
	else
	{
		mysql_free_result(res);

		char iconName[16];
		rt2_snprintf(iconName, 16, "%d", iNewID);
// 		m_core.icon = iconName;
		outIconID = iNewID;
		//delete escapeData;
		DEL_ONE(escapeData);
		
		return true;
	}
}

unsigned long CUnion::GetIcon()
{
	return m_core.m_nPic;
}

bool CUnion::SetBulletin(string bulletin)
{
	BOOL_ENSURE(bulletin.size() <= UNION_BULLETIN_STRING_MAX)

	string tBulletin;
	g_server.m_database.EscapeString(tBulletin, bulletin);

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_sprintf(cmd.command, "UPDATE T_union SET "		
		"Bulletin = '%s' "
		"WHERE ID = %d LIMIT 1",
		tBulletin.c_str(),
		GetID());
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return false;
	}
	else
	{
		mysql_free_result(pDBResult);

		m_core.m_strChatPost = bulletin;
		return true;
	}
}

bool CUnion::Appoint(DWORD playerID, EUnionOfficial official)
{
	BOOL_ENSURE( UnionOfficial_IsValid(official) )
	
	SUnionCitizen *pCitizen =  Find(playerID);
	BOOL_ENSURE(pCitizen)

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
// 	rt2_sprintf(cmd.command, "UPDATE T_union_citizen SET "		
// 		"official = %d "
// 		"WHERE userID = %d LIMIT 1",
// 		(int)official,
// 		playerID);
	rt2_sprintf(cmd.command, "UPDATE T_union_citizen_back SET official=%d WHERE userID=%d LIMIT 1", (int)official, playerID);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return false;
	}
	else
	{
		mysql_free_result(pDBResult);

		pCitizen->m_eUnionOfficial = official;
		return true;
	}
}

bool CUnion::InviteWar(DWORD unionInvited)
{
	// CHECK
	// 被邀请诸侯国存在
	// 邀请诸侯国必须3级或3级以上
	// 现在是攻城时间
	// 包括诸侯在内，所有诸侯国民不得在攻城区域
	CUnion* pUnion = m_pUnionMgr->FindUnion(unionInvited);
	if (!pUnion)			return false;
// 	if (m_core.level < 3)	return false;
	if ( 0)					return false;	// 如果非攻城时间
	if (0)					return false;	// 包括诸侯在内，所有诸侯国民不得在攻城区域
	return false;
}


void CUnion::OnCitizenComming(DWORD playerID)
{
	SUnionCitizen* pCitizen = Find(playerID);
	if (pCitizen)
	{
		pCitizen->m_bOnLine = 1;

		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_online(this, playerID);
		
		const  int c_nBufNum = 16;
		CVote* pBuff[c_nBufNum];
		int nNum = CVoteMgr::GetMgr().GetUnionAllVote(GetID(), pBuff, c_nBufNum);
		int i;
		for (i = 0; i < nNum; i++)
		{
			CVote* pVote = pBuff[i];
			if (NULL == pVote)
			{
				continue;
			}
			GWUser *pGWUser= g_server.FindActorById(pCitizen->m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}

			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}
			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_union_new_vote);

			rPacket.WriteLong(pCitizen->m_nPlayerID);
			rPacket.WriteLong(GetID());
			rPacket.WriteLong(pVote->GetCreater());
			rPacket.WriteLong(pVote->GetRecall());
			rPacket.WriteLong(pVote->GetVoteID());
			g_server.m_region.SendToPeer(Session);
		}
	}
}

void CUnion::OnCitizenGoing(DWORD playerID)
{
	SUnionCitizen* pCitizen = Find(playerID);
	if (pCitizen)
	{
		pCitizen->m_bOnLine = 0;

		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_offline(this, playerID);
	}
}

void CUnion::QueryLog(unsigned long dwPlayerID)
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	do 
	{
		GWUser *pGWUser= g_server.FindActorById(dwPlayerID);
		if (NULL == pGWUser)
		{
			break;
		}

		int Session= g_server.GetRegionSession(pGWUser->regionID);
		if(Session==-1)
		{
			break;
		}
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT LogType, LogData, UNIX_TIMESTAMP(LogTime) FROM t_VoteLog Where UnionID = %d ORDER BY LogTime LIMIT 300", GetID());
		if (!g_server.m_database.ExecCommand(cmd.command, &res))
		{
			break;
		}
		int count = mysql_num_rows(res);

		CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
		rPacket.WriteShort(c2r_union_log_query);

		rPacket.WriteLong(dwPlayerID);
		rPacket.WriteLong(count);
		int i;
		for (i = 0; i < count; i++)
		{
			MYSQL_ROW row = mysql_fetch_row(res);
			unsigned long LogType =  atol(row[0]);
			std::string str = row[1];
			unsigned long dwTime = atol(row[2]);
			rPacket.WriteLong(LogType);
			rPacket.WriteLong(dwTime);
			rPacket.WriteString(str.c_str());
		}
		g_server.m_region.SendToPeer(Session);
	} while (false);
	mysql_free_result(res);
}

int CUnion:: AddLog(UnionLogType type, const char* pData)
{
	int nRet = 1;
	do 
	{
		if (NULL == pData)
		{
			break;
		}
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "INSERT INTO t_VoteLog(LogType, LogData, LogTime, UnionID) VALUES("
			"%d,"
			"'%s',"
			"NOW(),"
			"%d)",
			type,
			pData, GetID());
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 1;//数据库出错
			break;
		}

		mysql_free_result(pDBResult);
		OnNewLog(type, pData);
		nRet = 0;
	} while (false);
	return nRet;
}

void CUnion::OnNewVote(CVote* pVote)
{
	do 
	{
		if (NULL == pVote)
		{
			break;
		}
		unsigned long dwRe = pVote->GetRecall();
		SUnionCitizen* pRe = Find(dwRe);
		if (NULL == pRe)
		{
			break;
		}

		unsigned long dwCre = pVote->GetCreater();

		EUnionOfficial	reOfficial = pRe->m_eUnionOfficial;


		int size = (int)(m_core.vecUnionCitizen.size());
		for (int i=0; i<size; i++)
		{
			SUnionCitizen& uc = m_core.vecUnionCitizen[i];
			if (uc.m_eUnionOfficial > reOfficial)
			{
				continue;
			}
			
			if (uc.m_nPlayerID == dwCre)
			{
				continue;
			}
			GWUser *pGWUser= g_server.FindActorById(uc.m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}
			
			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_union_new_vote);

			rPacket.WriteLong(uc.m_nPlayerID);
			rPacket.WriteLong(GetID());
			rPacket.WriteLong(dwCre);
			rPacket.WriteLong(dwRe);
			rPacket.WriteLong(pVote->GetVoteID());
			g_server.m_region.SendToPeer(Session);
		}
	} while (false);
}

int  CUnion::OnNewLog(UnionLogType type, const char* pData)
{
	int nRet = 0;
	do 
	{
		if (NULL == pData)
		{
			break;
		}
		

		int size = (int)(m_core.vecUnionCitizen.size());
		for (int i=0; i<size; i++)
		{
			SUnionCitizen& uc = m_core.vecUnionCitizen[i];
			
			GWUser *pGWUser= g_server.FindActorById(uc.m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}

			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_union_new_log);
			rPacket.WriteLong(uc.m_nPlayerID);
			rPacket.WriteLong(type);
			rPacket.WriteLong(time(NULL));
			rPacket.WriteString(pData);
			g_server.m_region.SendToPeer(Session);
		}
	} while (false);
	return nRet;
}

int  CUnion::GetNum(EUnionOfficial enO)
{
	int nRet = 0;
	do 
	{
		int size = (int)m_core.vecUnionCitizen.size();
		for (int i=0; i<size; i++)
		{
			if ( m_core.vecUnionCitizen[i].m_eUnionOfficial < enO)
			{
				continue;
			}
			nRet++;
		}
	} while (false);
	return nRet;
}

int CUnion::GiveUnion(unsigned long dwPlayerID, unsigned long dwTo)
{
	int nRet = 1;
	do
	{
		SUnionCitizen* pFrom = Find(dwPlayerID);
		SUnionCitizen* pTo = Find(dwTo);
		if (NULL == pFrom || NULL == pTo)
		{
			nRet = 1;
			break;
		}
		EUnionOfficial	enFrom = pFrom->m_eUnionOfficial;
		EUnionOfficial	enTo = pTo->m_eUnionOfficial;
		if(UO_King != enFrom)
		{
			nRet = 2;
			break;
		}
		pTo->m_eUnionOfficial = enFrom;
		pFrom->m_eUnionOfficial = enTo;
		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_official_change(this, dwPlayerID, pFrom->m_eUnionOfficial);
		CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_official_change(this, dwTo, pTo->m_eUnionOfficial);
		std::ostringstream os;
		os<<pFrom->m_strPlayerName<<" "<<pTo->m_strPlayerName;
		AddLog(eGiveCaption, os.str().c_str());
		nRet = 0;
	}while(false);
	return nRet;
}

int CUnion::AddWarScores(long nAdd)
{
	int nRet = 1;
	do 
	{
		nAdd /= 5;
		SetWarScores(GetWarScores() + nAdd);
		nRet = 0;
	} while (false);
	return nRet;
}

unsigned long CUnion::GetWarScores()
{
	return m_core.m_nFighting;
}

void CUnion::SetWarScores(unsigned long dwWS)
{
	do 
	{
		if (dwWS == m_core.m_nFighting)
		{
			break;
		}
		m_core.m_nFighting = dwWS;
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_sprintf(cmd.command, "UPDATE T_union SET "		
			"Score = %d "
			"WHERE ID = %d LIMIT 1",
			m_core.m_nFighting, GetID());
		if (!g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			break;	
		}
		mysql_free_result(pDBResult);

		int size = (int)(m_core.vecUnionCitizen.size());
		for (int i=0; i<size; i++)
		{
			SUnionCitizen& uc = m_core.vecUnionCitizen[i];

			GWUser *pGWUser= g_server.FindActorById(uc.m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}

			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_WarScores_Change);
			rPacket.WriteLong(uc.m_nPlayerID);
			rPacket.WriteLong(m_core.m_nFighting);

			g_server.m_region.SendToPeer(Session);
		}

	} while (false);
}

int CUnion::UpdateContribution(unsigned long dwPlayerID, long dwC)
{
	int nRet = 1;
	do 
	{
		SUnionCitizen* pCitizen = Find(dwPlayerID);	
		if (NULL == pCitizen)
		{
			nRet = 3;
			break;
		}
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;

		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "UPDATE T_union_citizen_back SET Contribution=%d WHERE userID=%d LIMIT 1", dwC, dwPlayerID);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 4;
			break;
		}
		mysql_free_result(pDBResult);

		pCitizen->m_nContribution = dwC;
		
		int size = (int)(m_core.vecUnionCitizen.size());
		for (int i=0; i<size; i++)
		{
			SUnionCitizen& uc = m_core.vecUnionCitizen[i];

			GWUser *pGWUser= g_server.FindActorById(uc.m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}

			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_union_update_Contribution);
			rPacket.WriteLong(uc.m_nPlayerID);
			rPacket.WriteLong(dwPlayerID);
			rPacket.WriteLong(dwC);
		
			g_server.m_region.SendToPeer(Session);
		}

		nRet = 0;
	} while (false);
	return nRet;
}

int CUnion::DoContribution(long playerID, long nMoney, long nItem1, long nItem2, long nItem3, long nItem4)
{
	int nRet = 1;
	do 
	{

		SUnionCitizen* pCitizen = Find(playerID);	
		if (NULL == pCitizen)
		{
			nRet = 3;
			break;
		}
		long OldC = pCitizen->m_nContribution;
		long nAdd = 0;
		bool bWrite = false;
		if (0 != nMoney)
		{
			nAdd +=nMoney / 200;
			m_core.dwMoney += nMoney;
			 bWrite = true;
		}
		if (0 != nItem1)
		{
			m_core.m_Item[0] += nItem1;
			bWrite = true;
		}
		if (0 != nItem2)
		{
			m_core.m_Item[1] += nItem1;
			bWrite = true;
		}
		if (0 != nItem3)
		{
			m_core.m_Item[2] += nItem1;
			bWrite = true;
		}
		if (0 != nItem4)
		{
			m_core.m_Item[3] += nItem1;
			bWrite = true;
		}
		if (0 != nAdd)
		{
			UpdateContribution(playerID, pCitizen->m_nContribution + nAdd);
		}
		if (!bWrite)
		{
			break;
		}
		
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_sprintf(cmd.command, "UPDATE T_union SET "		
			"Money = %d"
			",Item1 = %d"
			",Item2 = %d"
			",Item3 = %d"
			",Item4 = %d "
			"WHERE ID = %d LIMIT 1",
			m_core.dwMoney, m_core.m_Item[0], m_core.m_Item[1], m_core.m_Item[2], m_core.m_Item[3], GetID());
		if (!g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 4;
			break;
		}
		mysql_free_result(pDBResult);
		
		
		int size = (int)(m_core.vecUnionCitizen.size());
		for (int i=0; i<size; i++)
		{
			SUnionCitizen& uc = m_core.vecUnionCitizen[i];

			GWUser *pGWUser= g_server.FindActorById(uc.m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}

			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_union_contribution);
			rPacket.WriteLong(uc.m_nPlayerID);
			rPacket.WriteLong(m_core.dwMoney);
			rPacket.WriteLong(m_core.m_Item[0]);
			rPacket.WriteLong(m_core.m_Item[1]);
			rPacket.WriteLong(m_core.m_Item[2]);
			rPacket.WriteLong(m_core.m_Item[3]);

			rPacket.WriteLong(playerID);
			rPacket.WriteLong(nMoney);

			g_server.m_region.SendToPeer(Session);
		}
		
		std::ostringstream os;
		os<<pCitizen->m_strPlayerName<<" "<<nMoney<<" "<<nItem1<<" "<<nItem2<<" "<<nItem3<<" "<<nItem4;
		AddLog(eDoContribution, os.str().c_str());
		nRet = 0;
	} while (false);
	return nRet;
}

int CUnion::SetIcon(unsigned long dwPlayerID, long dwIconID)
{

	int nRet = 1;
	do 
	{
		SUnionCitizen *pPlayer = m_core.Find(dwPlayerID);

		if(!pPlayer || pPlayer->m_eUnionOfficial!=UO_King)
		{
			nRet = 2;
			break;
		}
		SUnionIcon* pIcon = g_TableUnionIcon.FindUnionIcon(dwIconID);
		if(NULL == pIcon)
		{
			nRet = 2;
			break;
		}

		if (m_core.dwMoney < pIcon->nMoney)
		{
			const int c_nLen = 256;
			char szBuff[c_nLen];
			rt2_snprintf(szBuff, c_nLen, R(MSG_CARD_BUY_MONEY_NOT_ENOUGH), pIcon->nMoney, m_core.dwMoney);

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_team);
			rPacket.WriteLong(dwPlayerID);
			rPacket.WriteByte(GW2R_TEAM_PICKER);
			rPacket.WriteString(szBuff);

			int Session= g_server.GetRegionSession(pPlayer->m_nRegionID);
			if(Session !=-1)
			{
				g_server.m_region.SendToPeer(Session);
			}
			nRet = 3;
			break;
		}
		m_core.dwMoney -= pIcon->nMoney;
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_sprintf(cmd.command, "UPDATE T_union SET "		
			"Icon = %d, "
			"Money = %d  "
			"WHERE ID = %d LIMIT 1",
			dwIconID, m_core.dwMoney, GetID());
		if (!g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 4;
			break;
		}
		mysql_free_result(pDBResult);
		m_core.m_nPic		= (int)(dwIconID);
		std::ostringstream os;
		os<<dwIconID;
		int size = (int)(m_core.vecUnionCitizen.size());
		for (int i=0; i<size; i++)
		{
			SUnionCitizen& uc = m_core.vecUnionCitizen[i];

			GWUser *pGWUser= g_server.FindActorById(uc.m_nPlayerID);
			if (NULL == pGWUser)
			{
				continue;
			}
			pGWUser->unionIcon = os.str();
			int Session= g_server.GetRegionSession(pGWUser->regionID);
			if(Session==-1)
			{
				continue;
			}

			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(r2c_union_set_icon);
			rPacket.WriteLong(uc.m_nPlayerID);
			rPacket.WriteLong(dwIconID);
			rPacket.WriteLong(m_core.dwMoney);
			g_server.m_region.SendToPeer(Session);

		}
		nRet = 0;
	} while (false);
	return nRet;
}

int CUnion::OnDelKind()
{
	int nRet = 1;
	do 
	{
		SUnionCitizen* pInheritor = NULL;
		unsigned long dwOldKind = GetCaption();
		SUnionCitizen* pOldKind = NULL;
		int size = (int)(m_core.vecUnionCitizen.size());

		for (int i=0; i < size; i++)
		{
			SUnionCitizen* pT = &(m_core.vecUnionCitizen[i]);

			if (pT->m_nPlayerID == dwOldKind)
			{
				pOldKind = pT;
				continue;
			}
			if (NULL == pInheritor)
			{
				pInheritor = pT;
				continue;
			}
			if (pT->m_eUnionOfficial < pInheritor->m_eUnionOfficial)
			{
				pInheritor = pT;
				continue;
			}
			if (pT->m_eUnionOfficial == pInheritor->m_eUnionOfficial)
			{
				if (pT->m_nContribution > pInheritor->m_nContribution)
				{
					pInheritor = pT;
				}
			}
		}

		// 是否有继承人
		if (pInheritor)
		{
			CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_official_change(this, pInheritor->m_nPlayerID, UO_King);
			
			SetCaptain(pInheritor->m_nPlayerID);
			RemoveCitizen(dwOldKind);
			if (NULL != pOldKind)
			{
				AddLog(CUnion::eLeave, pOldKind->m_strPlayerName.c_str());
			}

			CUnionCmdHelper_GW::Broadcast_r2c_union_citizen_leave(this, dwOldKind);

			CUnionCmdHelper_GW::Broadcast_r2c_union_data_refresh(0, GetData());
		}
		else
		{
			string unionName = GetName();
			string playerName = pOldKind->m_strPlayerName;
			AddLog(CUnion::eLeave, pOldKind->m_strPlayerName.c_str());
			CUnionCmdHelper_GW::Broadcast_r2c_union_disband(this, UNION_MSG_CAPTAIN_DISBAND, (char*)GetName().c_str());
			GetUnionMgr()->DeleteUnion(GetID());
			LOG2("A guild's caption leave, so the guild is deleted!(guild=%s,caption=%s)\n", unionName.c_str(), playerName.c_str());
		}
	} while (false);
	return nRet;
}

CUnionMgr::CUnionMgr()
{
	m_bInited = false;
}

CUnionMgr::~CUnionMgr()
{
	for (int i=0; i<(int)m_arrUnions.size(); i++)
	{
		CUnion* pUnion = m_arrUnions[i];
		DEL_ONE(pUnion);
	}
}

bool CUnionMgr::Init()
{
	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT ID,Name,Bulletin,Icon,CaptainID,Money,Item1,Item2,Item3,Item4 FROM T_union");
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	int count = mysql_num_rows(res);
	SUnionData unionData;
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);

		unionData.m_nID			= atol(row[0]);
		unionData.m_strName		= row[1];
		unionData.m_strChatPost = row[2];
		unionData.m_nPic		= atol(row[3]);
		unionData.m_nCaptainID	= atol(row[4]);
		unionData.dwMoney		= atol(row[5]);
		unionData.m_Item[0]		= atol(row[6]);
		unionData.m_Item[1]		= atol(row[7]);
		unionData.m_Item[2]		= atol(row[8]);
		unionData.m_Item[3]		= atol(row[9]);

		CUnion *pUnion = RT_NEW CUnion(this, unionData);
		pUnion->m_core = unionData;
		m_arrUnions.push_back(pUnion);
	}
	mysql_free_result(res);

// 	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT T_union_citizen.userID, T_union_citizen.official, T_union_citizen.name,"
// 		"T_union_citizen.nickname, T_union_citizen.UnionID, user.Level FROM T_union_citizen,user"
// 		" WHERE user.ID=T_union_citizen.userID");
// 
	rt2_snprintf(
				cmd.command, GW_MAX_DB_COMMAND_LENGTH,
				"SELECT T_union_citizen_back.userID, T_union_citizen_back.official, user.name, T_union_citizen_back.UnionID, user.Level, "
				"T_union_citizen_back.Contribution, user.Metier "
				"FROM T_union_citizen_back, user WHERE user.ID=T_union_citizen_back.userID"
			);
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return false;
	}

	count = mysql_num_rows(res);
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		SUnionCitizen citizen;
		citizen.m_bOnLine = false;
		citizen.m_nPlayerID = atol(row[0]);
		citizen.m_eUnionOfficial = (EUnionOfficial)atol(row[1]);
		if (row[2])
			citizen.m_strPlayerName = row[2];
// 		if (row[3])
// 			citizen.m_strNickName = row[3];
		DWORD unionID = atol(row[3]);
 		citizen.m_sLevel = atol(row[4]);
		citizen.m_nContribution = atol(row[5]);
		citizen.m_cMetier = (char)(atol(row[6]));
	
		CUnion *pUnion = FindUnion(unionID);
		if (pUnion)
		{
			pUnion->m_core.vecUnionCitizen.push_back(citizen);
			GWUser *pGWUser= g_server.FindActorById(citizen.m_nPlayerID);
			if (NULL != pGWUser)
			{
				std::ostringstream os;
				os<<pUnion->GetIcon();
				pGWUser->unionIcon = os.str();
			}

			//GWUser* pUser = g_server.FindActorById(citizen.m_nPlayerID);
			//if (pUser)
			//{
			//	pUser->unionID = unionID;
			//	m_mapUserToUnion[pUser->userID] = unionID;
			//}
		}
	}
	mysql_free_result(res);

	// 检查一下每个诸侯国的诸侯是否已经不存在了，如果不存在，那么诸侯国将解散
	//vector<DWORD> arrToBeDeleted;
	//for (int i=0; i<(int)m_arrUnions.size(); i++)
	//{
	//	CUnion *pUnion = m_arrUnions[i];

	//	GWDBCommand cmd;
	//	MYSQL_RES *pDBResult;
	//	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT ID FROM `user` WHERE ID=%d AND deleted=0", pUnion->GetCaption());
	//	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	//		continue;
	//	int count = mysql_num_rows(pDBResult);
	//	if (count==0)
	//	{
	//		arrToBeDeleted.push_back(pUnion->GetID());
	//	}
	//	mysql_free_result(pDBResult);
	//}

	//for (int i=0; i<(int)arrToBeDeleted.size(); i++)
	//{
	//	CUnion *pUnion = FindUnion(arrToBeDeleted[i]);
	//	string unionName;
	//	if (pUnion)			unionName = pUnion->GetName();
	//	// 解散诸侯国
	//	if (!DeleteUnion(arrToBeDeleted[i]))
	//		ERR1("DeleteUnion Error!(union=%d)\n", arrToBeDeleted[i]);
	//	LOG1("A guild is deleted because can NOT find its caption! (guild=%s)\n", unionName.c_str());
	//}

	m_bInited = true;
	return true;
}

void CUnionMgr::Tick()
{
	static DWORD s_dwLastTick = 0;
	DWORD now = rtGetMilliseconds();
	if (now-s_dwLastTick > 600000)
	{
		s_dwLastTick = now;

		RemoveInvalidUnion();
	}
}

bool CUnionMgr::RemoveInvalidUnion()
{
	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	MYSQL_ROW row;
	DWORD dwNow = 0;
	DWORD dwTmp = 0;
	DWORD dwInDB = 0;
	int count;

	dwNow = time(NULL);
	dwTmp = dwNow - 48*3600;

	// Check Time and Delete
	for (int i=0; i<(int)m_arrUnions.size(); i++)
	{
		rt2_snprintf(cmd.command,  GW_MAX_DB_COMMAND_LENGTH, "SELECT UNIX_TIMESTAMP(ToBeDeletedBegin) FROM T_union WHERE ID='%d'", m_arrUnions[i]->GetID());
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )	
			return false;	
		count = mysql_num_rows(pDBResult);
		if (count==0)		return false;

		row = mysql_fetch_row(pDBResult);
		if (row[0])
			dwInDB = atol(row[0]);
		else 
			dwInDB = 0;
		
		if (dwInDB!=0)
		{
			if (dwInDB<dwTmp && m_arrUnions[i]->CitizenCount() < UNION_MIN_CITIZEN)
			{
				string unionName = m_arrUnions[i]->GetName();
				CUnionCmdHelper_GW::Broadcast_r2c_union_disband(m_arrUnions[i], UNION_MSG_FEWCITIZEN_DISBAND, (char*)(m_arrUnions[i]->GetName().c_str()));
				DeleteUnion(m_arrUnions[i]->GetID());
				LOG3("A guild is deleted! (Guild=%s,ToBeDeletedBegin=%d,Now=%d)\n", unionName.c_str(), dwInDB, dwNow);
			}
		}
		mysql_free_result(pDBResult);
	}

	return true;
}

DWORD CUnionMgr::CreateUnion(DWORD playerID, short playerLevel, std::string name, string enounce)
{
	guard;

	// CHECK
	// 诸侯国名字不为空，而且不重复
	if ( name.length()==0 )		return 0;
	
	if(!NAME_IS_VALID(name.c_str()))
	{
		CUnionCmdHelper_GW::Send_r2c_msg_text(playerID, R(MSG_UNION_BADNAME));
		return 0;
	}
	//char cmd[GW_MAX_DB_COMMAND_LENGTH];

	//rt2_sprintf(cmd, "SELECT COUNT(*) FROM user WHERE AccountID = '%d' and deleted=0", data->accountId);
	
	std::ostringstream os;
	os<<"SELECT COUNT(*) FROM T_union WHERE Name = '"<<name.c_str()<<"'";
	MYSQL_RES* res;

	if (!g_server.m_database.ExecCommand(os.str().c_str(), &res))
	{
		return 0;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	int count = atol(row[0]);
	mysql_free_result(res);

	if (count > 0)
	{
		CUnionCmdHelper_GW::Send_r2c_msg_text(playerID, R(MSG_UNION_NAMEHASBEENUSED));
		return 0;
	}

	string tName;
	string tEnounce;
	g_server.m_database.EscapeString(tName, name);
	g_server.m_database.EscapeString(tEnounce, enounce);

	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_snprintf(cmd.command,  GW_MAX_DB_COMMAND_LENGTH, "INSERT INTO T_union(Name, Level, Enounce, Bulletin, Icon, CaptainID, ToBeDeletedBegin, Score) VALUES("
		/*"'',"*/
		"'%s',"
		"%d,"
		"'%s',"
		"'%s',"
		"'%s',"
		"%d,"
		"FROM_UNIXTIME(%u),"
		"%d)",
		tName.c_str(),
		1,
		tEnounce.c_str(),
		"",
		UNION_DEFAULT_ICON_NAME,
		playerID,
		time(NULL),
		0);
	if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
	{
		return 0;
	}else
	{
		MYSQL_ROW row;
		rt2_snprintf(cmd.command,  GW_MAX_DB_COMMAND_LENGTH, "SELECT `ID` FROM `T_union` WHERE `Name` = '%s'", tName.c_str());
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
        {
		    rt2_snprintf(cmd.command,  GW_MAX_DB_COMMAND_LENGTH, "SELECT `ID` FROM `T_union` WHERE `Name` LIKE CONVERT( _utf8 '%s' USING gb2312 )", tName.c_str());
            if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
            {
			    return false;
            }
        }
		int count = mysql_num_rows(pDBResult);
		if (count==0)		return false;

		row = mysql_fetch_row(pDBResult);
		int unionID = atol(row[0]);
		mysql_free_result(pDBResult);

		// 建立新诸侯国，并把诸侯加为国民
		SUnionData tmp;
		tmp.m_nCaptainID	= playerID;
		tmp.m_nID			= unionID;
		tmp.m_strName		= name;
		CUnion *pUnion = RT_NEW CUnion(this, tmp);
		GWUser *pUser = g_server.FindActorById(playerID);

		pUnion->AddCitizen(playerID, playerLevel, pUser->attr.metier, true, UO_King);
		pUnion->SetBulletin(enounce);

		m_arrUnions.push_back(pUnion);
		return unionID;
	}

	unguard;
}

bool CUnionMgr::DeleteUnion(DWORD unionID)
{
	//	CHECK

	// 查找Union
	vector<CUnion*>::iterator iter = m_arrUnions.begin();
	CUnion *pUnion = NULL;
	for (; iter!=m_arrUnions.end(); iter++)
	{
		if ( (*iter)->GetID() == unionID )
		{
			pUnion = *iter;
			break;
		}
	}

	if (pUnion)
	{
		SUnionCitizen *pCaption = pUnion->Find(pUnion->GetCaption());
		if (pCaption)
		{
			g_server.LogUserEvent(PLT_UnionOperate, pCaption->m_nPlayerID, "", 4, pUnion->GetName().c_str(), /*pUnion->GetData()->level*/0, pCaption->m_strPlayerName.c_str(), 0, "");
		}

		GWDBCommand cmd;
		MYSQL_RES *pDBResult;

		// 从DB和内存中删除Citizen
		SUnionData *pUnionData = pUnion->GetData();
		int size = (int)pUnionData->vecUnionCitizen.size();
		vector<DWORD> arrID;
		std::string strC = R(MSG_UNION_TAKEDOWNUNION);
		std::string strT = R(MSG_UNION_UNIONHASBEENTAKENDOWN);
		for (int i=0; i<size; i++)
		{
			DWORD id = pUnionData->vecUnionCitizen[i].m_nPlayerID;
			arrID.push_back(pUnionData->vecUnionCitizen[i].m_nPlayerID);
			if (!pUnionData->vecUnionCitizen[i].m_bOnLine)
			{
				g_server.m_mail.AddMail(pUnionData->vecUnionCitizen[i].m_nPlayerID, 0, 
					strT.c_str(), strT.c_str(), false, "", 0);
			}
		}
		for (int i=0; i<size; i++)
		{
			pUnion->RemoveCitizen(arrID[i]);
		}

		// 从DB和内存中删除Union
		rt2_snprintf(cmd.command,  GW_MAX_DB_COMMAND_LENGTH, "DELETE FROM T_union WHERE ID = "
			"%d",
			unionID);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			return false;
		}
		mysql_free_result(pDBResult);

		m_arrUnions.erase(iter);
		DEL_ONE(pUnion);
		GetFeodMgr()->OnUnionDeleted(unionID);
		return true;
	}
	else
	{
		return false;
	}
}

CUnion* CUnionMgr::FindUnion(DWORD unionID)
{
	if (unionID==0)	return NULL;

	int size = (int)m_arrUnions.size();
	for (int i=0; i<size; i++)
	{
		if ( m_arrUnions[i]->GetID() == unionID)
		{
			return m_arrUnions[i];
		}
	}

	return NULL;
}

//DWORD CUnionMgr::GetUnionID(DWORD userID)
//{
//	hash_map<DWORD, DWORD>::iterator iter = m_mapUserToUnion.find(userID);
//
//	if (iter == m_mapUserToUnion.end())
//		return NULL;
//
//	return iter->second;
//}

void CUnionMgr::OnCitizenComming(DWORD playerID, int& unionID, DWORD regionID)
{
	if (unionID==0)			return;

	CUnion* pUnion = FindUnion(unionID);
	if (pUnion)
	{
		SUnionCitizen *pCitizen = pUnion->Find(playerID);
		if (pCitizen)
		{
			pCitizen->m_nRegionID = regionID;
			pUnion->OnCitizenComming(playerID);
		}
		else 
		{
			unionID = 0;
		}
	}
	else 
	{
		unionID = 0;
	}
}

void CUnionMgr::OnCitizenGoing(DWORD playerID, DWORD unionID)
{
	if (unionID==0)			return;

	CUnion* pUnion = FindUnion(unionID);
	if (pUnion)
	{
		SUnionCitizen *pCitizen = pUnion->Find(playerID);
		if (pCitizen)
		{
			pCitizen->m_nRegionID = 0;
			pUnion->OnCitizenGoing(playerID);
		}
	}
}

void CUnionMgr::OnDeleteUser(DWORD userDBID)
{
	for (int i=0; i<(int)m_arrUnions.size(); i++)
	{
		DWORD CaptionID = m_arrUnions[i]->m_core.m_nCaptainID;
		if (userDBID==CaptionID)
		{
			/*string unionName = m_arrUnions[i]->GetName();
			CUnionCmdHelper_GW::Broadcast_r2c_union_disband(m_arrUnions[i], UNION_MSG_CAPTAIN_DISBAND, (char*)m_arrUnions[i]->GetName().c_str());
			GetUnionMgr()->DeleteUnion(m_arrUnions[i]->GetID());
			LOG1("The caption of a union is been DELETED, and the union is disband now(union=%s)\n", unionName.c_str());*/
			CUnion* pUnion = FindUnion(m_arrUnions[i]->GetID());
			if (pUnion)
			{
				pUnion->OnDelKind();
			}
			return;
		}
	}
}

void CUnionMgr::OnCreateUser(DWORD userDBID)
{
}

void CUnionMgr::OnAddNewRegion(int regionID)
{
	GWRegion& rg = g_server.m_regionTable[regionID];
	if (rg.sessionIndex == -1)		return;

	CUnionCmdHelper_GW::UpdateRegionUnionData_r2c_union_data_refresh(rg.sessionIndex);
	CUnionCmdHelper_GW::UpdateRegionUnionData_r2c_union_logo_data(rg.sessionIndex);
}

void CUnionMgr::QueryLog(unsigned long dwUnion, unsigned long dwPlayerID)
{
	do 
	{
		CUnion* pUnion = GetUnionMgr()->FindUnion(dwUnion);
		if (NULL == pUnion)
		{
			break;
		}
		pUnion->QueryLog(dwPlayerID);
	} while (false);
}

void CUnionMgr::GiveUnion(unsigned long dwUnion, unsigned long dwPlayerID, unsigned long dwTo)
{
	CUnion* pUnion = GetUnionMgr()->FindUnion(dwUnion);
	do
	{
		if (NULL == pUnion)
		{
			break;
		}
		pUnion->GiveUnion(dwPlayerID, dwTo);
	}while(false);

}

int CUnionMgr::UpdateContribution(GWUser* pUser, long dwDate)
{
	int nRet = 0;
	do 
	{
		if (NULL == pUser)
		{
			nRet = 1;
			break;
		}
		
		CUnion* pUnion = FindUnion(pUser->unionID);
		if (NULL == pUnion)
		{
			nRet = 2;
			break;
		}

		nRet = pUnion->UpdateContribution(pUser->userID, dwDate);
		if (0 == nRet)
		{
			pUser->attr.UnionRecord = dwDate;
		}
	} while (false);
	return nRet;
}

int CUnionMgr::DoContribution(DWORD dwUnion, long playerID, long nMoney, long nItem1, long nItem2, long nItem3, long nItem4)
{
	int nRet = 1;
	do 
	{
		CUnion* pUnion = FindUnion(dwUnion);
		if (NULL == pUnion)
		{
			nRet = 2;
			break;
		}
		pUnion->DoContribution(playerID, nMoney, nItem1, nItem2, nItem3, nItem4);
		nRet = 0;
	} while (false);
	return nRet;
}

int CUnionMgr::AddWarScores(unsigned long dwUnion, long nAdd)
{
	int nRet = 1;
	do 
	{
		CUnion* pUnion = FindUnion(dwUnion);
		if (NULL == pUnion)
		{
			nRet = 2;
			break;
		}
		nRet = pUnion->AddWarScores(nAdd);
	} while (false);
	return nRet;
}

int CUnionMgr::SetIcon(GWUser* pUser, long dwIconID)
{
	int nRet = 1;
	do 
	{
		CUnion* pUnion = FindUnion(pUser->unionID);
		if (NULL == pUnion)
		{
			nRet = 2;
			break;
		}
		nRet = pUnion->SetIcon(pUser->userID, dwIconID);
		
		
		/*int Session= g_server.GetRegionSession(pUser->regionID);
		if(Session !=-1)
		{
			CG_CmdPacket& rPacket = g_server.m_region.BeginCmd(g2r_union);
			rPacket.WriteShort(g2r_set_union_icon_result);
			rPacket.WriteLong(pUser->userID);
			rPacket.WriteLong(dwIconID);
			rPacket.WriteLong(nRet);
			g_server.m_region.SendToPeer(Session);
		}*/
	} while (false);
	return nRet;
}

CVote::CVote(unsigned long dwVoteID, unsigned long dwCreater, unsigned long dwRecall, unsigned long StopTime):
m_dwVoteID(dwVoteID),
m_dwUnionID(0),
m_dwCreater(dwCreater), 
m_dwRecall(dwRecall), 
m_dwStopTime(StopTime)
{
	GWUser *pCreater = g_server.FindActorById(dwCreater);
	do 
	{
		if (NULL == pCreater)
		{
			break;
		}
		m_dwUnionID = pCreater->unionID;
	} while (false);
}

unsigned long CVote::GetVoteID()
{
	return m_dwVoteID;
}

unsigned long CVote::GetCreater()
{
	return m_dwCreater;
}

unsigned long CVote::GetRecall()
{
	return m_dwRecall;
}

unsigned long CVote::GetUnionID()
{
	return m_dwUnionID;
}

bool CVote::OnRun(unsigned long dwTime)
{
	bool bRet = false;
	do 
	{
		if (dwTime < m_dwStopTime)
		{
			break;
		}
		End();
		bRet = true;
	} while (false);
	return bRet;
}

int CVote::GetAgreeNum()
{
	CVote::VoteSet& as = GetAgreeSet();
	return (int)(as.size());
}

int CVote::GetOpposeNum()
{
	CVote::VoteSet& os = GetOpposeSet();
	return (int)(os.size());
}

int CVote::GetRecallNum()
{
	CVote::VoteSet& fs = GetForfeitSet();
	return (int)(fs.size());
}

int CVote::GetDoneNum()
{
	return GetAgreeNum() + GetOpposeNum() + GetRecallNum();
}

int CVote::GetAllNum()
{
	int nRet = 0;
	do 
	{
		CUnion* pUnion = GetUnionMgr()->FindUnion(GetUnionID());
		if (NULL == pUnion)
		{
			nRet = 1;
			break;
		}
		SUnionCitizen* pCitizen = pUnion->Find(GetRecall());
		if (NULL == pCitizen)
		{
			nRet = 1;
			break;
		}
		nRet = pUnion->GetNum(pCitizen->m_eUnionOfficial);
	} while (false);
	return nRet;
}

CVote::VoteSet& CVote::GetAgreeSet()
{
	return m_setAgree;
}

CVote::VoteSet& CVote::GetOpposeSet()
{
	return m_setOppose;
}

CVote::VoteSet& CVote::GetForfeitSet()
{
	return m_setForfeit;
}

int CVote::GetPlayerVoteState(unsigned long dwPlayerID)
{
	int nRet = 0;
	CVote::VoteSet& as = GetAgreeSet();
	CVote::VoteSet& os = GetOpposeSet();
	CVote::VoteSet& fs = GetForfeitSet();
	VoteSet::iterator iter;
	do 
	{
		iter = as.find(dwPlayerID);
		if (as.end() != iter)
		{
			nRet = 1;
			break;
		}
		iter = os.find(dwPlayerID);
		if (os.end() != iter)
		{
			nRet = 2;
			break;
		}
		iter = fs.find(dwPlayerID);
		if (fs.end() != iter)
		{
			nRet = 3;
			break;
		}
	} while (false);
	return nRet;
}

int CVote::Agree(unsigned long dwPlayerID)
{
	CVote::VoteSet& as = GetAgreeSet();
	int nRet = 0;
	do 
	{
		nRet = GetPlayerVoteState(dwPlayerID);
		if (0 != nRet)
		{
			break;
		}
		int nT = DoVote(dwPlayerID, 0);
		if (0 != nT)
		{
			nRet = 8;
			break;
		}
		as.insert(dwPlayerID);
	} while (false);
	switch (nRet)
	{
	case 0:
		{
			int nTem = ProcessNum();
			if (0 == nTem)
			{
				nRet = -1;
			}
		}
		break;
	default:
		break;
	}
	return nRet;
}

int CVote::Oppose(unsigned long dwPlayerID)
{
	CVote::VoteSet& os = GetOpposeSet();
	int nRet = 0;
	do 
	{
		nRet = GetPlayerVoteState(dwPlayerID);
		if (0 != nRet)
		{
			break;
		}
		int nT = DoVote(dwPlayerID, 1);
		if (0 != nT)
		{
			nRet = 8;
			break;
		}
		os.insert(dwPlayerID);
	} while (false);
	switch (nRet)
	{
	case 0:
		if(0 == ProcessNum())
		{
			nRet = 4;
		}
		break;
	default:
		break;
	}
	return nRet;
}

int CVote::Forfeit(unsigned long dwPlayerID)
{
	CVote::VoteSet& fs = GetForfeitSet();
	int nRet = 0;
	do 
	{
		nRet = GetPlayerVoteState(dwPlayerID);
		if (0 != nRet)
		{
			break;
		}
		int nT = DoVote(dwPlayerID, 2);
		if (0 != nT)
		{
			nRet = 8;
			break;
		}
		fs.insert(dwPlayerID);
	} while (false);
	switch (nRet)
	{
	case 0:
		ProcessNum();
		break;
	default:
		break;
	}
	return nRet;
}


int CVote::DoVote(unsigned long dwPlayerID, int nState)
{
	int nRet = 0;
	do 
	{
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "INSERT INTO t_Vote(VoteID, VoterID, VState) VALUES("
			"%d,"
			"%d,"
			"%d)",
			GetVoteID(),
			dwPlayerID,
			nState);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 1;//数据库出错
			break;
		}

		mysql_free_result(pDBResult);
		nRet = 0;
	} while (false);
	return nRet;
}

int CVote::ProcessNum()
{
	int nRet = 1;
	do 
	{
		int nDone = GetDoneNum();
		if (nDone < GetAllNum())
		{
			break;
		}
		End();
		nRet = 0;
	} while (false);
	return nRet;
}

void CVote::End()
{
	int nA = GetAgreeNum();
	int nO = GetOpposeNum();
	do 
	{
		CUnion* pUnion = GetUnionMgr()->FindUnion(GetUnionID());
		if (NULL == pUnion)
		{
			break;
		}
		unsigned long dwR = GetRecall();
		if (nA > nO)
		{
			pUnion->RemoveCitizen(dwR);
		}
	} while (false);
}

void CVote::Load()
{
	CVote::VoteSet& as = GetAgreeSet();
	CVote::VoteSet& os = GetOpposeSet();
	CVote::VoteSet& fs = GetForfeitSet();

	as.clear();
	os.clear();
	fs.clear();

	GWDBCommand cmd;
	MYSQL_RES* res;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT VoterID, VState FROM t_Vote where VoteID = %d", GetVoteID());
	if (!g_server.m_database.ExecCommand(cmd.command, &res))
	{
		return;
	}

	int count = mysql_num_rows(res);
	SUnionData unionData;
	for (int i=0; i<count; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		unsigned long VoterID = atol(row[0]);
		unsigned long VState = atol(row[1]);
		switch (VState)
		{
		case 0:
			as.insert(VoterID);
			break;
		case 1:
			os.insert(VoterID);
			break;
		case 2:
			fs.insert(VoterID);
			break;
		}
	}
	mysql_free_result(res);

}

void CVote::Save()
{
	
}

void CVote::Delete()
{
	GWDBCommand cmd;
	MYSQL_RES *pDBResult;
	rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "DELETE From t_Vote where VoteID = %d", GetVoteID());
	g_server.m_database.ExecCommand(cmd.command, &pDBResult);
	mysql_free_result(pDBResult);
}

int CVoteMgr::CretaeVote(unsigned long dwCreater, unsigned long dwRecall)
{
	int nRet = 1;
	do 
	{
		GWUser *pCreater = g_server.FindActorById(dwCreater);
		GWUser *pRecall = g_server.FindActorById(dwRecall);
		if (NULL == pCreater || NULL == pRecall)
		{
			nRet = 1;
			break;
		}
		if (pCreater->unionID != pRecall->unionID)
		{
			nRet = 2;
			break;
		}
		CUnion* pUnion = GetUnionMgr()->FindUnion(pCreater->unionID);
		if (NULL == pUnion)
		{
			nRet = 1;
			break;
		}
		RecallKeySet& rs = GetRecallKeySet();
		CVote temp;
		temp.m_dwRecall = dwRecall;
		RecallKeySet::iterator iter = rs.find(&temp);
		if (rs.end() != iter)
		{
			nRet = 3;//此君正在被罢免
			break;
		}
		time_t tTime = time(NULL) + 86400;
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "INSERT INTO t_CreateVote(c_Creater, Recall, EndTime) VALUES("
			"%d,"
			"%d,"
			"FROM_UNIXTIME(%u))",
			dwCreater,
			dwRecall,
			tTime);
		if ( !g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 4;//数据库出错
			break;
		}
		unsigned long dwVoteID = g_server.m_database.GetInsertId();
		mysql_free_result(pDBResult);
		CVote* pVote = RT_NEW CVote(dwVoteID, dwCreater, dwRecall, tTime);
		AddVote(pVote);
		DoVote(dwCreater, dwVoteID, 0);
		pUnion->OnNewVote(pVote);
		nRet = 0;
	} while (false);
	return nRet;
}

int CVoteMgr::Load()
{
	int nRet = 1;
	CVoteMgr::IDKeySet& is = GetIDKeySet();
	CVoteMgr::RecallKeySet& rs = GetRecallKeySet();
	CVoteMgr::UnionIDKeySet& us = GetUnionIDKeySet();

	is.clear();
	rs.clear();
	us.clear();

	do 
	{
		GWDBCommand cmd;
		MYSQL_RES* res;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "SELECT VoteID, c_Creater, Recall, UNIX_TIMESTAMP(EndTime) FROM t_CreateVote");
		if (!g_server.m_database.ExecCommand(cmd.command, &res))
		{
			nRet = 1;
			break;
		}

		int count = mysql_num_rows(res);
		SUnionData unionData;
		for (int i=0; i<count; i++)
		{
			MYSQL_ROW row = mysql_fetch_row(res);
			unsigned long dwVoteID = atol(row[0]);
			unsigned long dwCreater = atol(row[1]);
			unsigned long dwRecall = atol(row[2]);
			unsigned long tTime = atol(row[3]);
			
			CVote* pVote = RT_NEW CVote(dwVoteID, dwCreater, dwRecall, tTime);
			AddVote(pVote);
			pVote->Load();
		}
		mysql_free_result(res);
		nRet = 0;
	} while (false);
	return nRet;
}

void CVoteMgr::Save()
{

}

bool CVoteMgr::OnRun(unsigned long dwTime)
{
	bool bRet = false;
	do 
	{
		RecallKeySet& rs = GetRecallKeySet();
		RecallKeySet::iterator iterCur;
		for(iterCur = rs.begin(); rs.end() != iterCur; )
		{
			CVote* pV = *iterCur;
			if (NULL != pV)
			{
				bool bD = pV->OnRun(dwTime);
				if (bD)
				{
					iterCur++;
					DelVote(pV);
					continue;
				}
			}
			iterCur++;
		}
	} while (false);
	return bRet;
}

CVoteMgr::IDKeySet& CVoteMgr::GetIDKeySet()
{
	return m_IDKeySet;
}

CVoteMgr::RecallKeySet& CVoteMgr::GetRecallKeySet()
{
	return m_RecallKeySet;
}


CVoteMgr::UnionIDKeySet& CVoteMgr::GetUnionIDKeySet()
{
	return m_UnionIDKeySet;
}

int CVoteMgr::DoVote(unsigned long dwPlayer, unsigned long dwVoteID, int nState)
{
	int nRet = 1;
	do 
	{
		IDKeySet& is = GetIDKeySet();
		IDKeySet::iterator iter;
		CVote cT;
		cT.m_dwVoteID = dwVoteID;
		iter = is.find(&cT);
		if (is.end() == iter)
		{
			nRet = 4;
			break;
		}
		CVote* pVote = *iter;
		if (NULL == pVote)
		{
			nRet = 4;
			break;
		}

		switch (nState)
		{
		case 0:
			nRet = pVote->Agree(dwPlayer);
			break;
		case 1:
			nRet = pVote->Oppose(dwPlayer);
			break;
		case 2:
			nRet = pVote->Forfeit(dwPlayer);
			break;
		default:
			nRet = 4;
			break;
		}
		if (-1 == nRet)
		{
			DelVote(pVote);
		}
	} while (false);
	return nRet;
}

int CVoteMgr::AddVote(CVote* pVote)
{
	int nRet = 1;
	do 
	{
		if (NULL == pVote)
		{
			break;
		}
		IDKeySet& is = GetIDKeySet();
		RecallKeySet& rs = GetRecallKeySet();
		UnionIDKeySet& us = GetUnionIDKeySet();

		is.insert(pVote);
		rs.insert(pVote);
		us.insert(pVote);
		nRet = 0;
	} while (false);
	return nRet;
}

int CVoteMgr::DelVote(CVote* pVote)
{
	int nRet = 1;
	do 
	{
		if (NULL == pVote)
		{
			break;
		}
		GWDBCommand cmd;
		MYSQL_RES *pDBResult;
		rt2_snprintf(cmd.command, GW_MAX_DB_COMMAND_LENGTH, "DELETE FROM t_CreateVote WHERE VoteID = %d", pVote->GetVoteID());
		if (!g_server.m_database.ExecCommand(cmd.command, &pDBResult) )
		{
			nRet = 4;//数据库出错
			break;
		}

		mysql_free_result(pDBResult);

		IDKeySet& is = GetIDKeySet();
		RecallKeySet& rs = GetRecallKeySet();
		UnionIDKeySet& us = GetUnionIDKeySet();
		
		pVote->Delete();

		is.erase(pVote);
		rs.erase(pVote);
		us.erase(pVote);
		DEL_ONE(pVote)
		nRet = 0;
	} while (false);
	return nRet;
}

int CVoteMgr::GetUnionAllVote(unsigned long dwUnionID, CVote** pBuf, int nBuffNum)
{
	int nRet = 0;
	do 
	{
		if (NULL == pBuf)
		{
			break;
		}
		UnionIDKeySet& us = GetUnionIDKeySet();
		CVote temp;
		temp.m_dwUnionID = dwUnionID;
		std::pair<UnionIDKeySet::iterator, UnionIDKeySet::iterator> pairFind = us.equal_range(&temp);
		UnionIDKeySet::iterator iter;
		for (iter = pairFind.first; iter != pairFind.second; iter++)
		{
			if (nRet < nBuffNum)
			{
				pBuf[nRet++] = *iter;
			}
		}
	} while (false);
	return nRet;
}


CVoteMgr& CVoteMgr::GetMgr()
{
	static CVoteMgr s_Mgr;
	return s_Mgr;
}

bool IDKey::operator () (CVote *p1, CVote *p2) const
{
	bool bRet = false;
	do 
	{
		if (NULL == p1 || NULL == p2)
		{
			break;
		}
		bRet = p1->GetVoteID() < p2->GetVoteID();
	} while (false);
	return bRet;
}

bool RecallKey::operator () (CVote *p1, CVote *p2) const
{
	bool bRet = false;
	do 
	{
		if (NULL == p1 || NULL == p2)
		{
			break;
		}
		bRet = p1->GetRecall() < p2->GetRecall();
	} while (false);
	return bRet;
}

bool UnionIDKey::operator () (CVote *p1, CVote *p2) const
{
	bool bRet = false;
	do 
	{
		if (NULL == p1 || NULL == p2)
		{
			break;
		}
		CUnion* pUnion1 = g_UnionMgr.FindUnion(p1->GetUnionID());
		CUnion* pUnion2 = g_UnionMgr.FindUnion(p2->GetUnionID());
		bRet = pUnion1 < pUnion2;
	} while (false);
	return bRet;
}

