#ifndef __ACTIVITY_INFO_H__
#define __ACTIVITY_INFO_H__


enum CYCLE_EVENT
{

};
enum ACTIVITY_MESSAGE
{
	ACTMSG_CREATE,
	ACTMSG_DESTORY,
	ACTMSG_START,
	ACTMSG_OVER,
	ACTMSG_CYCLE,
	ACTMSG_BROADCAST,
	ACTMSG_GETUSERINFO,
	ACTMSG_SAVEUSERINFO,
	ACTMSG_UPDATAUSERJOIN,
	ACT_MESSAGE_OVER
};
enum ACTIVITY_TYPE
{
	ACTIVITY_BASE,
	ACTIVITY_TELPHER,
	ACTIVITY_TREASURE,
	ACTIVITY_CACHOT,
	ACTIVITY_TYPE_OVER
};
enum ACTIVE_STATE
{
	AS_NONE,
	AS_START,
	AS_OVER,
	AS_CAN,
	AS_STATE_OVER
};

struct Activity
{
	const static int ciActTimeCount = 5;

	ACTIVITY_TYPE	eActivityType;
	ACTIVE_STATE	eState;
	short			iUpdataTime[ciActTimeCount];

	Activity(void) : eActivityType(ACTIVITY_BASE), eState(AS_NONE)
	{
		for(int ii=0; ii<ciActTimeCount; ii++)
			iUpdataTime[ii] = 0;
	}

	virtual bool	TimeLessEqual(const short iTime[])
	{
		bool bLess = false;
		for(int ii=0; ii<ciActTimeCount; ii++)
		{
			if(iUpdataTime[ii] == 0 || iTime[ii] == 0)
			{
				if(ii >= ciActTimeCount - 1)
					return true;
				continue;
			}
			if(iUpdataTime[ii] < iTime[ii])
				return true;
			if(iUpdataTime[ii] > iTime[ii])
				return false;
		}
		return bLess;
	}

	virtual void	UpdataInfo(Activity *pInfo)
	{
		if(pInfo)
		{
			eActivityType = pInfo->eActivityType;
			eState = pInfo->eState;
			for(int ii=0; ii<ciActTimeCount; ii++)
				iUpdataTime[ii] = pInfo->iUpdataTime[ii];
		}
	}

	virtual void	SerializeToString(std::string *psBuffer)
	{
		if(!psBuffer)
			return;
		char sTemp[40] = {0};
		rt2_snprintf(sTemp, 40, "%d ", eActivityType);
		(*psBuffer) += sTemp;
		rt2_snprintf(sTemp, 40, "%d ", eState);
		(*psBuffer) += sTemp;
		rt2_snprintf(sTemp, 40, "%d.%d.%d.%d.%d ", iUpdataTime[0], iUpdataTime[1], iUpdataTime[2], iUpdataTime[3], iUpdataTime[4]);
		(*psBuffer) += sTemp;
	}
	virtual std::string	SerializeToThis(std::string *psBuffer)
	{
		if(!psBuffer)
			return std::string();

		const char *sep = " ";
		const char *timeSep = ".";
		char *sString = (char*)psBuffer->c_str();
		char *sigStr = strtok(sString, sep);
		if (sigStr && strlen(sigStr)>0)
		{
			eActivityType = (ACTIVITY_TYPE)atoi(sigStr);
			if(eActivityType < 0 || eActivityType >= ACTIVITY_TYPE_OVER)
				eActivityType = ACTIVITY_BASE;
			sigStr = strtok(NULL, sep);
		}
		if (sigStr && strlen(sigStr)>0)
		{
			eState = (ACTIVE_STATE)atoi(sigStr);
			if(eState < 0 || eState >= AS_STATE_OVER)
				eState = AS_STATE_OVER;
			sigStr = strtok(NULL, sep);
		}
		if (sigStr && strlen(sigStr)>0)
		{
			char *sRetStr = strtok(NULL, sep);
			sigStr = strtok(sigStr, timeSep);
			for(int kk=0; kk<ciActTimeCount && sigStr && sigStr[0] != 0; kk++)
			{
				iUpdataTime[kk] = atoi(sigStr);
				sigStr = strtok(NULL, timeSep);
			}

			return sRetStr ? sRetStr : "";
		}

		return "";
	}
};

struct UserActivityInfo : public Activity
{
	UserActivityInfo(void) : Activity()
	{

	}
};

struct UserCachotInfo : public UserActivityInfo
{
	int iDegree;

	UserCachotInfo(void) : UserActivityInfo(), iDegree(0)
	{
	}

	void	UpdataInfo(Activity *pInfo)
	{
		UserCachotInfo *info = dynamic_cast<UserCachotInfo*> (pInfo);
		if(info)
		{
			UserActivityInfo::UpdataInfo(pInfo);
			iDegree = info->iDegree;
		}
	}

	void	SerializeToString(std::string *psBuffer)
	{
		if(!psBuffer)
			return;
		UserActivityInfo::SerializeToString(psBuffer);
		char sTemp[40] = {0};
		rt2_snprintf(sTemp, 40, "%d ", iDegree);
		(*psBuffer) += sTemp;
	}

	std::string	SerializeToThis(std::string *psBuffer)
	{
		if(!psBuffer)
			return std::string();
		std::string newBuffer = UserActivityInfo::SerializeToThis(psBuffer);
		const char *sep = " ";
		char *sString = (char*)(newBuffer.c_str());
		char *sigStr = strtok(sString, sep);
		if (sigStr && strlen(sigStr)>0)
		{
			iDegree = atoi(sigStr);
			sigStr = strtok(NULL, sep);
			return sigStr ? sigStr : "";
		}
		return "";
	}
};

struct ActivityInfo : public Activity
{
	int		iLevel;

	ActivityInfo(int iLV) : Activity(), iLevel(iLV)
	{
	}

	void	UpdataInfo(Activity *pInfo)
	{
		ActivityInfo *info = dynamic_cast<ActivityInfo*> (pInfo);
		if(info)
		{
			Activity::UpdataInfo(pInfo);
			iLevel = info->iLevel;
		}
	}

	virtual bool	CanJoinActivity(int iUserLV, UserActivityInfo *pInfo)
	{
		if(iUserLV < iLevel)
			return false;
		return true;
	}

	void	SerializeToString(std::string *psBuffer)
	{
		if(!psBuffer)
			return;
		Activity::SerializeToString(psBuffer);
		char sTemp[40] = {0};
		rt2_snprintf(sTemp, 40, "%d ", iLevel);
		(*psBuffer) += sTemp;
	}

	std::string	SerializeToThis(std::string *psBuffer)
	{
		if(!psBuffer)
			return std::string();
		std::string newBuffer = Activity::SerializeToThis(psBuffer);
		const char *sep = " ";
		char *sString = (char*)(newBuffer.c_str());
		char *sigStr = strtok(sString, sep);
		if (sigStr && strlen(sigStr)>0)
		{
			iLevel = atoi(sigStr);
			sigStr = strtok(NULL, sep);
			return sigStr ? sigStr : "";
		}
		return "";
	}
};

struct ActivityCachot : public ActivityInfo
{
	int iDegree;

	ActivityCachot(int iLv, int iDeg) : ActivityInfo(iLv), iDegree(iDeg)
	{
	}

	void	UpdataInfo(Activity *pInfo)
	{
		ActivityCachot *info = dynamic_cast<ActivityCachot*> (pInfo);
		if(info)
		{
			ActivityInfo::UpdataInfo(pInfo);
			iDegree = info->iDegree;
		}
	}

	bool	CanJoinActivity(int iUserLV, UserActivityInfo *pInfo)
	{
		UserCachotInfo *pAInfo = dynamic_cast<UserCachotInfo*>(pInfo);
		int iDeg = pAInfo ? pAInfo->iDegree : iDegree;
		if(ActivityInfo::CanJoinActivity(iUserLV, pInfo) && iDeg < iDegree)
			return true;
		return false;
	}

	void	SerializeToString(std::string *psBuffer)
	{
		if(!psBuffer)
			return;
		ActivityInfo::SerializeToString(psBuffer);
		char sTemp[40] = {0};
		rt2_snprintf(sTemp, 40, "%d ", iDegree);
		(*psBuffer) += sTemp;
	}

	std::string	SerializeToThis(std::string *psBuffer)
	{
		if(!psBuffer)
			return std::string();
		std::string newBuffer = ActivityInfo::SerializeToThis(psBuffer);
		const char *sep = " ";
		char *sString = (char*)(newBuffer.c_str());
		char *sigStr = strtok(sString, sep);
		if (sigStr && strlen(sigStr)>0)
		{
			iDegree = atoi(sigStr);
			sigStr = strtok(NULL, sep);
			return sigStr ? sigStr : "";
		}
		return "";
	}
};

struct ActivityRes
{
	static const int	iMaxEventCount = 5;

	ACTIVITY_TYPE	eActivityType;
	std::string		sActName;
	int				iLevel;
	int				iDegree;
	short			iForeTime[iMaxEventCount][iMaxEventCount][5];
	std::string		sForeInfo[iMaxEventCount];
	short			iStartTime[iMaxEventCount][5];	//月、周、日、时、分  (为-1表示每个时段)
	short			iOverTime[iMaxEventCount][5];	//月、周、日、时、分  (为-1表示每个时段)
	int				iEventCycle;
	std::string		sScript;
	std::string		sStartInfo[iMaxEventCount];
	std::string		sOverInfo[iMaxEventCount];
	std::string		sCycleInfo[iMaxEventCount];

	ActivityRes(void) : eActivityType(), iLevel(0), iDegree(0), iEventCycle(0)
	{
		memset(iForeTime, -1, sizeof(short) * iMaxEventCount * iMaxEventCount * 5);
		for(int ii=0; ii<iMaxEventCount; ii++)
		{
			for(int jj=0; jj<5; jj++)
			{
				iStartTime[ii][jj] = -1;
				iOverTime[ii][jj] = -1;
			}
		}
	}
};
class CRS_Activity
{
	typedef	std::map<int, ActivityRes*>	ACT_RES;

public:
	CRS_Activity(void)	{}
	~CRS_Activity(void)
	{
		for(ACT_RES::iterator it = mapActRes.begin();
			it != mapActRes.end();
			it++)
			DEL_ONE(it->second)
		mapActRes.clear();
	}

	bool	Load(const char *File)
	{
		const char *sep = ";";
		const char *sep2 = ",";
		const char *timeSep = ".";
		RtCsv csv;
		if(!csv.LoadTableFile(File))
			return false;

		int j = csv.FindLineByString("ID");
		if(j<0) return false;
		csv.SetTitleLine(j);

		int iTotal = csv.GetLineCount();
		for(j++; j<iTotal; j++)
		{
			ActivityRes *pActRes = RT_NEW ActivityRes();
			int iType = csv[j]["Type"].Int();
			if(iType < 0 || iType >= ACTIVITY_TYPE_OVER)
				continue;
			pActRes->eActivityType = (ACTIVITY_TYPE)iType;
			pActRes->sActName = csv[j]["Name"].Str();
			pActRes->iLevel = csv[j]["Level"].Int();
			pActRes->iDegree = csv[j]["Degree"].Int();

			//ForeTime
			char *sForeString = (char*)csv[j]["ForeTime"].Str();
			std::string sForeTime[ActivityRes::iMaxEventCount];
			char *sigStr = strtok(sForeString, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					sForeTime[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}
			for(int ll=0; ll<ActivityRes::iMaxEventCount; ll++)
			{
				int kk=0;
				sigStr = strtok((char*)sForeTime[ll].c_str(), sep2);
				if(!sigStr || sigStr[0] == 0)
					break;
				std::string sSubString[ActivityRes::iMaxEventCount];
				for(kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					sSubString[kk] = sigStr;
					sigStr = strtok(NULL, sep2);
				}
				for(kk=0; kk<ActivityRes::iMaxEventCount; kk++)
				{
					sigStr = strtok((char*)sSubString[kk].c_str(), timeSep);
					if(!sigStr || sigStr[0] == 0)
						break;
					for(int nn=0; nn<5 && sigStr && sigStr[0] != 0; nn++)
					{
						pActRes->iForeTime[ll][kk][nn] = (short)atoi(sigStr);
						sigStr = strtok(NULL, timeSep);
					}
				}
			}

			//ForeInfo
			sForeString = (char*)csv[j]["ForeInfo"].Str();
			sigStr = strtok(sForeString, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					pActRes->sForeInfo[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}

			//StartTime
			char *sStartStr = (char*)csv[j]["StartTime"].Str();
			std::string sTempTime[ActivityRes::iMaxEventCount];
			sigStr = strtok(sStartStr, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					sTempTime[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}
			for(int ll=0; ll<ActivityRes::iMaxEventCount; ll++)
			{
				sigStr = strtok((char*)sTempTime[ll].c_str(), timeSep);
				if(!sigStr || sigStr[0] == 0)
					break;
				for(int nn=0; nn<5 && sigStr && sigStr[0] != 0; nn++)
				{
					pActRes->iStartTime[ll][nn] = (short)atoi(sigStr);
					pActRes->iOverTime[ll][nn] = pActRes->iStartTime[ll][nn] != 0 ? pActRes->iStartTime[ll][nn] + 1 : 0;
					sigStr = strtok(NULL, timeSep);
				}
			}

			//EndTime
			char *sEndStr = (char*)csv[j]["EndTime"].Str();
			for(int x=0; x<ActivityRes::iMaxEventCount; x++)
				sTempTime[x].clear();
			sigStr = strtok(sEndStr, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					sTempTime[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}
			for(int ll=0; ll<ActivityRes::iMaxEventCount; ll++)
			{
				sigStr = strtok((char*)sTempTime[ll].c_str(), timeSep);
				if(!sigStr || sigStr[0] == 0)
					break;
				for(int nn=0; nn<5 && sigStr && sigStr[0] != 0; nn++)
				{
					pActRes->iOverTime[ll][nn] = (short)atoi(sigStr);
					sigStr = strtok(NULL, timeSep);
				}
			}

			pActRes->iEventCycle = csv[j]["CycleTime"].Int();
			pActRes->sScript = csv[j]["Script"].Str();

			char *sString = (char*)csv[j]["StartInfo"].Str();
			sigStr = strtok(sString, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					pActRes->sStartInfo[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}
			sString = (char*)csv[j]["EndInfo"].Str();
			sigStr = strtok(sString, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					pActRes->sOverInfo[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}
			sString = (char*)csv[j]["EventInfo"].Str();
			sigStr = strtok(sString, sep);
			if (sigStr && strlen(sigStr)>0)
			{
				for(int kk=0; kk<ActivityRes::iMaxEventCount && sigStr && sigStr[0] != 0; kk++)
				{
					pActRes->sCycleInfo[kk] = sigStr;
					sigStr = strtok(NULL, sep);
				}
			}

			mapActRes[pActRes->eActivityType] = pActRes;
			//mapActRes[csv[j]["ID"].Int()] = pActRes;
		}
		return true;
	}

	ActivityRes*	FindActivity(int iID)
	{
		if(iID < 0)
			return NULL;
		ACT_RES::iterator itAct = mapActRes.find(iID);
		if(itAct != mapActRes.end())
			return itAct->second;
		return NULL;
	}

	int	GetFirstActivity(ActivityRes **pRes)
	{
		*pRes = NULL;
		m_itCurRes = mapActRes.begin();
		if(m_itCurRes == mapActRes.end())
			return 0;
		*pRes = m_itCurRes->second;
		return m_itCurRes->first;
	}
	int	GetNextActivity(ActivityRes **pRes)
	{
		*pRes = NULL;
		if(m_itCurRes == mapActRes.end())
			return 0;
		m_itCurRes++;
		if(m_itCurRes == mapActRes.end())
			return 0;
		*pRes = m_itCurRes->second;
		return m_itCurRes->first;
	}

private:
	ACT_RES	mapActRes;

	ACT_RES::iterator	m_itCurRes;
};


#endif//__ACTIVITY_INFO_H__
