#pragma once
#include "ui/rtw_ui.h"

//lyymark 没有声明变量时忽略“HINT_WIDGET”的左侧
enum HINT_WIDGET;

class CTimer_Hint : public GcTimerListener
{
public:
	CTimer_Hint(void);
	~CTimer_Hint(void);
	void OnTimerEvent(DWORD vID);

	//hint相关
	RtwWidget*			m_wdtHintFor;
	HINT_WIDGET			m_eNowHintFor;
	GcActorSkill*		m_pActorSkill;
};
