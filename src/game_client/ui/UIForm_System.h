#pragma once
#include "ui/rtw_ui.h"
#include <string>
#include <map>

class CUIForm_System
{
public:
	CUIForm_System(void);
	~CUIForm_System(void);
	void Show();
	void Hide();
	void Show_Close(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void RunAnimation();
	void DoFrame();
private:
	ui::RtwWidget*	m_pfrmSystem;
	ui::RtwButton* m_pbtnShow_CloseButton;

	bool m_bIsRunning;
	bool m_bShow;
	float m_fStep;
};
