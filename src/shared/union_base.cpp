//********************************************************************
//	created:	2010.05.31
//	filename: 	union_base.cpp
//	author:		ldr123
//	purpose:	游戏中,帮派系统基本操作重做
//  other:		version 1.0
//********************************************************************

#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "net/rt_net.h"
#include "union_base.h" 

bool SUnionLog::Serialize(CG_CmdPacket *cmd)
{
	if (!cmd)
	{
		return false;
	}

	*cmd << m_nLogType;
	*cmd << m_nLogTime;
	*cmd << m_strLog;

	if (cmd->IsError())
	{
		return false;
	}

	return true;
}

std::string SUnionLog::GetLogString()
{
	string strResult = "";
	if (m_strLog.empty() || m_strLog=="")
	{
		return strResult;
	}

	strResult = "<p>";
	return strResult;
}

bool SUnionCitizen::Serialize(CG_CmdPacket *cmd)
{
	guard;

	int nUnionOfficial = (int)m_eUnionOfficial;

	*cmd << m_nPlayerID;
	*cmd << m_nRegionID;
	*cmd << m_bOnLine;
	*cmd << m_cMetier;
	*cmd << m_nContribution;
	*cmd << m_sLevel;
	*cmd << nUnionOfficial;
	*cmd << m_strPlayerName;

	m_eUnionOfficial = (EUnionOfficial)nUnionOfficial;

	if (cmd->IsError())
	{
		return false;
	}

	return true;
	unguard;
}

bool SUnionData::Serialize(CG_CmdPacket *cmd)
{
	guard;
	*cmd << m_nID;
	*cmd << m_nCaptainID;
	*cmd << m_nPic;
	*cmd << m_nStrength;
	//*cmd << m_nFacMoney;
	*cmd << m_nPeopleNum;
	*cmd << m_strName;
	*cmd << m_strChatPost;
	*cmd << m_nFighting;
	*cmd << dwMoney;
	int i;
	for (i = 0; i < eItemTypeNum; i++)
	{
		*cmd << m_Item[i];
	}

	if (cmd->IsWritting())
	{
		//写
		//用户信息
		int size = (int)vecUnionCitizen.size();
		*cmd << size;
		for (int i=0; i<size; ++i)
		{
			vecUnionCitizen[i].Serialize(cmd);
		}

		//日志信息
		size = (int)vecLogList.size();
		*cmd << size;
		for (int i=0; i<size; ++i)
		{
			vecLogList[i].Serialize(cmd);
		}

		////本版不开放
		////同盟信息
		//size = 0;
		//for (int i=0; i<UNION_MAX_ALLIANCE; ++i)
		//{
		//	if (m_nAlliance[i] == 0)
		//	{
		//		size = i+1;
		//		break;
		//	}
		//}
		//*cmd << size;
		//for (int i=0; i<size; ++i)
		//{
		//	*cmd << m_nAlliance[i];
		//}	
	}
	else
	{
		//读
		//用户信息
		vecUnionCitizen.clear();
		int size = 0;
		*cmd << size;
		for (int i=0; i<size; ++i)
		{
			SUnionCitizen tmp;
			tmp.Serialize(cmd);
			vecUnionCitizen.push_back(tmp);
		}

		//日志信息
		size = 0;
		*cmd << size;
		for (int i=0; i<size; ++i)
		{
			SUnionLog tmp;
			tmp.Serialize(cmd);
			vecLogList.push_back(tmp);
		}	

	//	//本版不开放
	//	//同盟信息
	//	size = 0;
	//	*cmd << size;

	//	if (size > UNION_MAX_ALLIANCE)
	//	{
	//		size = UNION_MAX_ALLIANCE;
	//	}
	//	for (int i=0; i<size; ++i)
	//	{
	//		*cmd << m_nAlliance[i];
	//	}	
	}

	m_bGetChangeEnable = true;

	if (cmd->IsError())
	{
		return false;
	}

	return true;

	unguard;
}

//统计是否可以更改职位
bool SUnionData::Union_ChangeOfficialEnable(const EUnionOfficial &eOfficial)
{
	if (m_bGetChangeEnable)
	{
		m_nKingNum		= 0;
		m_nSafeLeftNum	= 0;
		m_nSafeRightNum	= 0;
		m_nElderNum		= 0;
		m_nSubleaderNum	= 0;
		m_nMassesNum	= 0;

		for (std::vector<SUnionCitizen>::iterator svsi=vecUnionCitizen.begin(); svsi!=vecUnionCitizen.end(); ++svsi)
		{
			SUnionCitizen &pCitizen = (*svsi);
			switch (svsi->m_eUnionOfficial)
			{
			case UO_King:
				m_nKingNum++;
				break;
			case UO_safeguardLeft:
				m_nSafeLeftNum++;
				break;
			case UO_safeguardRight:
				m_nSafeRightNum++;
				break;
			case UO_Elder:
				m_nElderNum++;
				break;
			case UO_SubLeader:
				m_nSubleaderNum++;
				break;
			case UO_Masses:
				m_nMassesNum++;
				break;
			}
		}

		m_bGetChangeEnable = false;
	}

	int nPlayerNum = (m_nSafeLeftNum+m_nKingNum+m_nSafeRightNum+m_nSubleaderNum+m_nElderNum+m_nMassesNum);
	switch (eOfficial)
	{
	case UO_King:
		return false;
	case UO_safeguardLeft:
		if (UNION_MAX_SAFELEFT > m_nSafeLeftNum)
		{
			return true;
		}
		return false;
	case UO_safeguardRight:
		if (UNION_MAX_SAFERIGHT > m_nSafeRightNum)
		{
			return true;
		}
		return false;
	case UO_Elder:
		if (UNION_MAX_ELDER > m_nElderNum)
		{
			return true;
		}
		return false;
	case UO_SubLeader:
	case UO_Masses:
		if (UNION_MAX_CITIZEN > nPlayerNum)
		{
			return true;
		}
		break;
	}
	
	return false;
}

SUnionCitizen* SUnionData::Find(DWORD playerID)
{
	guard;

	if (playerID==0)	return NULL;

	int size = (int)vecUnionCitizen.size();
	for (int i=0; i<size; i++)
	{
		if ( vecUnionCitizen[i].m_nPlayerID == playerID)
		{
			return (SUnionCitizen*)(&(vecUnionCitizen[i]));
		}
	}

	return NULL;

	unguard;
}

SUnionCitizen* SUnionData::Find(std::string playerName)
{
	guard;

	if (playerName.empty())	return NULL;

	int size = (int)vecUnionCitizen.size();
	for (int i=0; i<size; i++)
	{
		if ( vecUnionCitizen[i].m_strPlayerName == playerName)
		{
			return (SUnionCitizen*)(&(vecUnionCitizen[i]));
		}
	}

	return NULL;

	unguard;
}

bool SUnionData::AddCitizen(SUnionCitizen &citizen)
{
	guard;
	vecUnionCitizen.push_back(citizen);
	m_bGetChangeEnable = true;

	return true;
	unguard;
}

bool SUnionData::RemoveCitizen(DWORD playerID)
{
	guard;

	vector<SUnionCitizen>::iterator iter = vecUnionCitizen.begin();
	for (; iter!=vecUnionCitizen.end(); iter++)
	{
		if ( (*iter).m_nPlayerID == playerID )
		{
			vecUnionCitizen.erase(iter);
			m_bGetChangeEnable = true;
			return true;
		}
	}

	return false;

	unguard;
}

bool SFeod::Serialize(CG_CmdPacket *cmd)
{
	guard;

	*cmd << name;
	*cmd << unionID;
	*cmd << taxRate;
	*cmd << bank;
	*cmd << incomeToday;

	if (cmd->IsError())
		return false;
	else 
		return true;

	unguard;
}

bool Union_GetEnableFromAuction(const EUnionAction& eUnionAction, SUnionCitizen* user1, SUnionCitizen *user2)
{
	if (user1->m_eUnionOfficial == UO_King)
	{
		return true; //帮主有执行任何操作的权限
	}

	EUnionOfficial eUser1;
	EUnionOfficial eUser2;

	if (user1)
	{
		eUser1 = user1->m_eUnionOfficial;
	}

	if (user2)
	{
		eUser2 = user2->m_eUnionOfficial;
	}

	switch (eUnionAction)
	{
		case UA_USECONTRIBUTION:
		case UA_ADDMEMBER:
			//左右护法可用
			return (eUser1==UO_safeguardLeft || eUser1==UO_safeguardRight);

		case UA_BANISHMENT:
		case UA_DISABLECHANNEL:
		case UA_LEVELCHANGE:
			return ((eUser1<eUser2) && (eUser1!=UO_SubLeader && eUser1!=UO_Masses));

		case UA_MONDIFYPOST://制作帮派公告无需user2
			//堂主和群众无权使用
			return (eUser1!=UO_SubLeader && eUser1!=UO_Masses);

		case UA_VIEWCONTRIBUTION:
		case UA_INVITEMEMBER:
			//只有群众没权限
			return (eUser1!=UO_Masses);
			
		case UA_LEAVEUNION:
			return (eUser1!=UO_King);
		case UA_CONTRIBUTION:
		case UA_UNIONTASK:
			return true;

		case UA_DEMISEUNION:
		case UA_CHANGEUNIONPIC:
		case UA_UNIONDISBAND:
			//上面已经校验是否是帮主了
		default:
			return false;
	}

	return false;
}