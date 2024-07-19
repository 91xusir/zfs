/////////////////////////////////
//Filename:   LogicExt.cpp
//Author:     PZH
//CreateTime: 2009-12-22
//Desc:		  一些逻辑功能的扩展，一般是新加的功能
////////////////////////////////
#include "LogicExt.h"
//#include "rtc2_time.h"
#include "gw_server.h"
#include "gw_commandchannel.h"

bool CLogicExt::IsLockName(const char* szUserName)
{
	NextLoginTimeMap& nm = GetNextLoginTimeMap();
	NextLoginTimeMap::iterator iter = nm.find(szUserName);
	return nm.end() != iter;
}

void CLogicExt::OnRun(unsigned long dwTime)
{
	NextLoginTimeMap& nm = GetNextLoginTimeMap();
	NextLoginTimeMap::iterator iter;
	try
	{
		for(iter = nm.begin(); nm.end() != iter; )
		{
			NextLoginTimeMap::iterator nextIt = iter;
			++ nextIt;
			if(dwTime >= (*iter).second)
				nm.erase(iter);

			iter = nextIt;
		}
	}
	catch (...)
	{
		return;
	}
}

void CLogicExt::AddLock(const char* szUserName)
{
	NextLoginTimeMap& nm = GetNextLoginTimeMap();
	unsigned long dwTime = rtMilliseconds();
	const unsigned long c_Setp = 10 * 60 * 1000;
	dwTime += c_Setp;
	nm.insert(std::make_pair(szUserName, dwTime));
}

int CLogicExt::GetErrNum(const char* szUserName)
{
	int nRet = 0;
	NextLoginTimeMap& NM = GetNumMap();
	NextLoginTimeMap::iterator iter = NM.find(szUserName);
	if(NM.end() != iter)
	{
		nRet = (int)((*iter).second);
	}

	return nRet;
}

void CLogicExt::DelInNumMap(const char* szUserName)
{
	NextLoginTimeMap& NM = GetNumMap();
	NM.erase(szUserName);
}

void CLogicExt::SetErrNum(const char* szUserName, int nNum)
{
	NextLoginTimeMap& NM = GetNumMap();
	NM[szUserName] = nNum;
}

NextLoginTimeMap& CLogicExt::GetNextLoginTimeMap()
{
	return m_NextLoginTimeMap;
}

NextLoginTimeMap& CLogicExt::GetNumMap()
{
	return m_NumMap;
}

int CLogicExt::DeleteOldLogin(unsigned long nID, GWUnloginClient *ulc, long lPoint, char cPointType, char cVIP)
{
	int nRet = 1;
	do
	{
		if (NULL == ulc)
		{
			break;
		}
		GWLoginClient *lc = g_server.FindLoginClientByAccountId((int)nID);
		if(NULL == lc)
		{
			break;
		}
		
		//g_server.UserQuit(lc);
		lc->SetPower(ulc->seed/*ulc*/);
		lc->lPoint = lPoint;
		lc->cPointType = cPointType;
		lc->cVIP = cVIP;
		nRet = 0;
		g_server.KickAccount(nID, KICK_USER_MULTI_LOGIN, "DeleteOldLogin");
	}while(false);
	return nRet;
}

CLogicExt& CLogicExt::GetCLogicExt()
{
	static CLogicExt s_Logic;
	return s_Logic;
}
