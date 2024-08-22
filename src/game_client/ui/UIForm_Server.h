#pragma once

class CUIForm_Server
{
public:
	CUIForm_Server(void);
	~CUIForm_Server(void);
	void Show();
	void Hide();
	void Refresh();
	void IsVisible();
	void OnSelectServer(RtwWidget *sender,void *e);
	void OnInsertNewServer(const char* servername,int ping,char* serverstate, const RtwRect &imgRc);
	void OnBtnOKDown(RtwWidget* sender, RtwEventDelegate* e);
	void SetSelectItemLast(int iLastSelect);
	RtwWidget*	GetServerWidget();
	int GetMaxLineNum();
	int GetSelectItem();
private:
	RtwWidget*			m_pfrmServer;
	RtwListBox*			m_plstServerContent;
	RtwWidget*			m_plbSelectServer;
	RtwButton*			m_pbtnOK;

	int					m_iSelect;
};
