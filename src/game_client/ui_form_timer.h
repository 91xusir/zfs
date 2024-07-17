//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef _INC_UI_FORM_TIMER_H_
#define _INC_UI_FORM_TIMER_H_

#define TOTAL_TIME	5

class UIFormTimer : public RtTimerListener
{
public:
	UIFormTimer();
	~UIFormTimer(){};
public:
	void Refresh();
	void Show(DWORD npc_id);
	void Hide();
	bool IsVisible();

private:
	
};

#endif //_INC_UI_FORM_TIMER_H_