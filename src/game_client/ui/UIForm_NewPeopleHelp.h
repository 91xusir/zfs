//********************************************************************
//	created:	2010.04.22 18:00
//	filename: 	UIForm_NewPeopleHelp.h
//	author:		ldr123
//	purpose:	������������ָ��
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
	//���ֵȼ�����
	//�ؼ���Ϣ
	//begin
	RtwWidget*		m_fmNewPeopleHelp;	//�������洰��
	RtwHtmlView*	m_htmlHelpInfo;		//htmlview��ʾ
	RtwButton*		m_btnLeft;			//��ť
	RtwButton*		m_btnRight;			//�Ұ�ť
	RtwLabel*		m_lblPages;			//ҳ����ʾ

	RtwWidget*		m_fmNewPeopleTips;
	//end
private:
	int						m_nCurrentPage;		//��ǰҳ��	(��ǰ��Ҳ鿴��ҳ��)
	map<int, std::string>	m_mapLevHtml;		//���еȼ���html��Ϣ
	vector<int>				m_vecLev;

	//ҳ����ʱ�ַ���
	CM_MEMDEF(m_szTmp, 8)
	bool					m_bVisableEnable;	//�Ƿ����ʾ
};
