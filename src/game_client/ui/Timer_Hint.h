#pragma once
#include "ui/rtw_ui.h"

extern enum HINT_WIDGET;

class CTimer_Hint : public GcTimerListener
{
public:
	CTimer_Hint(void);
	~CTimer_Hint(void);
	void OnTimerEvent(DWORD vID);

	//hintœ‡πÿ
	RtwWidget*			m_wdtHintFor;
	HINT_WIDGET			m_eNowHintFor;
	GcActorSkill*		m_pActorSkill;
};
