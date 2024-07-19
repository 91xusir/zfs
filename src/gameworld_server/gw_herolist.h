#ifndef _GW_HEROLIST_H
#define _GW_HEROLIST_H

#include "herolist_base.h"

#define HEROLIST_UPDATE_PERIOD		1200		// seconds
#define HEROLIST_STAGGER_PERIOD		8			// seconds

class CHeroList// : public RtTimerListener
{
public:
	CHeroList(int HeroCount, int UnionCount);
	~CHeroList();
//	virtual void OnTimerEvent(DWORD dwID);		// 定时更新排行榜

	bool Init();
	bool Refresh();
	void OnRefresh();

	void OnAddNewRegion(int regionID);

	void BroadcastToAllRegion();

	time_t m_LastUpdateTime;
	SHeroList m_data;

	// 为了避免一次性的执行所有查询耗时很多，所以增加以下变量，搓开查询
	bool m_bUpdated_Warrior;
	bool m_bUpdated_Hunter;
	bool m_bUpdated_Taolist;
	bool m_bUpdated_Wizard;
	bool m_bUpdated_Union;
};

extern "C" CHeroList g_HeroList;

CHeroList* GetHeroList();

#endif//_GW_HEROLIST_H

