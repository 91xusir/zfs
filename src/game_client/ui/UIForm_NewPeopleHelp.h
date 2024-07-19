//********************************************************************
//	created:	2010.04.22 18:00
//	filename: 	UIForm_NewPeopleHelp.h
//	author:		ldr123
//	purpose:	新手升级帮助指引
//  other:		
//********************************************************************

#pragma once
#include "ui/rtw_ui.h"

class CUIForm_NewPeopleHelp
{
public:
	CUIForm_NewPeopleHelp(void);
	~CUIForm_NewPeopleHelp(void);

	void OnRun(float fSecond);

	void Show();
	void Hide();
	void Init();

	void ShowUserLev();
	void SetLev(int nLev, bool bIsLevup = false);

	void MoveToPage(int nPage);

	void OnBtnLeft(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnBtnRight(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnTips(ui::RtwWidget* sender,RtwEventDelegate* e);

	void SetVisable(bool bVisableEnable = false);
private:
	//新手等级帮助
	//控件信息
	//begin
	RtwWidget*		m_fmNewPeopleHelp;	//帮助界面窗体
	RtwHtmlView*	m_htmlHelpInfo;		//htmlview显示
	RtwButton*		m_btnLeft;			//左按钮
	RtwButton*		m_btnRight;			//右按钮
	RtwLabel*		m_lblPages;			//页数显示

	RtwWidget*		m_fmNewPeopleTips;
	//end
private:
	int						m_nCurrentPage;		//当前页数	(当前玩家查看的页面)
	map<int, std::string>	m_mapLevHtml;		//所有等级的html信息
	vector<int>				m_vecLev;

	//页码临时字符串
	CM_MEMDEF(m_szTmp, 8)
	bool					m_bVisableEnable;	//是否可显示
};
