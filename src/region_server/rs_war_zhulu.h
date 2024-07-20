//-----add start by Cheney 09.04.10 -------//
//涿鹿战场，每周二、四晚上19:00~21:00的活动
#ifndef _WAR_ZHULU_
#define _WAR_ZHULU_
#include "region.h"
#define MAX(a,b) (a)>(b)?(a):(b);
#define MIN(a,b) (a)<(b)?(a):(b);
typedef unsigned long ULONG;

class CWarZhulu : public CRegionObject
{
public:
	CWarZhulu();
	virtual ~CWarZhulu();
	
	struct  SZhuluUserInfo
	{
		SZhuluUserInfo(ULONG id = 0)
		{
			userId = id;
			killNum = 0;
			stayTime = 0;
		}
		~SZhuluUserInfo(){}
		ULONG userId;
		int killNum;
		int stayTime;
	};

	virtual void OnTimer(ULONG id, DWORD dwParam[]);
	
	void StartUp();
	bool IsZhuluActive(bool fix);
	bool IsZhuluTime(bool fix);
	void StartZhulu(bool fix);
	bool IsInZhuluMap(CRegionUser *user);
	void EndZhulu();
	void KickZhuluUser(CRegionUser *user);
	void OnEnterZhulu(CRegionUser *user);
	void OnZhuluWin(char faction);
	void OnZhuluPKOther(CRegionUser *user);

	bool m_bFixZhuluActive;
	bool m_bTmpZhuluActive;
	long m_ZhuluBegTime;
	long m_ZhuluInterval;
	char m_ZhuluWinFaction;
	vector<string> m_ZhuluMap;
	//std::list<ULONG> m_ZhuluActiveUser;
	EXT_SPACE::unordered_map<ULONG,SZhuluUserInfo> m_ZhuluActiveUser;


};

#endif

