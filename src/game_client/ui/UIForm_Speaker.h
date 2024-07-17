#pragma once
#include "ui/rtw_ui.h"
class CEditChat;
class CChat;

class CUIForm_Speaker
{
public:
	CUIForm_Speaker(void);
	~CUIForm_Speaker(void);
	void Show();
	void Hide();
	void Refresh(){};
	void Send();
	void ButtonClick(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void FormHide(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
	void OnDLClickedSpeakerItem(RtwWidget* vpSender, RtwEventDelegate* vpEvent);
private:
	RtwWidget* m_pfrmSpeaker;
	RtwButton* m_pbtnOK;
	RtwButton* m_pbtnCancel;
	RtwTextBox* m_ptbTextBox;
};
