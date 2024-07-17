//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef _INC_UI_FORM_TIMER_H_
#define _INC_UI_FORM_TIMER_H_

#define CWAR_START_TIME	R(MSG_CASTLEWAR_CWAR_START_TIME)//"城战开始时间: "

class UIFormTimer
{
public:
	UIFormTimer();
	~UIFormTimer(){};
public:
	void Refresh();
	void Show(string feod_name, DWORD total_time);
	void Hide();
	bool IsVisible();
	void SetTotalTime(DWORD num) { m_total_time = num; }
	DWORD GetTotalTime() { return m_total_time; }

private:
	RtwLabel*	m_pLabelTimer;
	DWORD		m_total_time;
};

#endif //_INC_UI_FORM_TIMER_H_