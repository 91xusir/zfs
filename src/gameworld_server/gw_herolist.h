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
//	virtual void OnTimerEvent(DWORD dwID);		// ��ʱ�������а�

	bool Init();
	bool Refresh();
	void OnRefresh();

	void OnAddNewRegion(int regionID);

	void BroadcastToAllRegion();

	time_t m_LastUpdateTime;
	SHeroList m_data;

	// Ϊ�˱���һ���Ե�ִ�����в�ѯ��ʱ�ܶ࣬�����������±������꿪��ѯ
	bool m_bUpdated_Warrior;
	bool m_bUpdated_Hunter;
	bool m_bUpdated_Taolist;
	bool m_bUpdated_Wizard;
	bool m_bUpdated_Union;
};

extern "C" CHeroList g_HeroList;

CHeroList* GetHeroList();

#endif//_GW_HEROLIST_H

