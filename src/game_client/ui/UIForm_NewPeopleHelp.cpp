//********************************************************************
//	created:	2010.04.22 18:00
//	filename: 	UIForm_NewPeopleHelp.cpp
//	author:		ldr123
//	purpose:	新手升级帮助指引
//  other:		
//********************************************************************

#include "gc_include.h"
#include "UIForm_NewPeopleHelp.h"

CUIForm_NewPeopleHelp::CUIForm_NewPeopleHelp(void):m_nCurrentPage(0), m_bVisableEnable(false)
{
	m_fmNewPeopleHelp	= RTW_WIDGET("fmnew_people");
	m_htmlHelpInfo		= RTW_WIDGET_T(RtwHtmlView, "fmnew_people.htmlhelp");
	m_btnLeft			= RTW_WIDGET_T(RtwButton,	"fmnew_people.btnleft");
	m_btnRight			= RTW_WIDGET_T(RtwButton,	"fmnew_people.btnright");
	m_lblPages			= RTW_WIDGET_T(RtwLabel,	"fmnew_people.lbpages");

	m_fmNewPeopleTips	= RTW_WIDGET("fmNewpeopleTips");
	m_fmNewPeopleTips->Hide();
// 	m_fmNewPeopleTips->SetWidgetEffect(esTrans, esTrans, 1000);

	m_htmlHelpInfo->SetScrollBarV(true);
	m_htmlHelpInfo->SetEnableInput(false);
	m_htmlHelpInfo->SetAutoScrollV(true);
 	m_lblPages->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));

	m_btnLeft->EvLClick			+= RTW_CALLBACK(this,	CUIForm_NewPeopleHelp,	OnBtnLeft);
	m_btnRight->EvLClick		+= RTW_CALLBACK(this,	CUIForm_NewPeopleHelp,	OnBtnRight);
	m_htmlHelpInfo->EvLClick	+= RTW_CALLBACK(this,	UILayerMain,			OnHtmlAction_HtmlHelpView);

	m_fmNewPeopleTips->EvLClick	+= RTW_CALLBACK(this,	CUIForm_NewPeopleHelp,	OnTips);

	CM_MEMPROTECTOR(m_szTmp, 8)

// 	Init();
}

CUIForm_NewPeopleHelp::~CUIForm_NewPeopleHelp(void)
{ 
	m_mapLevHtml.clear();
	m_fmNewPeopleHelp->Hide();
	CM_MEMUNPROTECTOR(m_szTmp)
}

void CUIForm_NewPeopleHelp::Show()
{
	m_fmNewPeopleHelp->Show();
}

void CUIForm_NewPeopleHelp::Hide()
{
	m_fmNewPeopleHelp->Hide();
}

void CUIForm_NewPeopleHelp::Init()
{
	m_mapLevHtml.clear();

	RtCsv csv;
	std::string strKey, strValue;

	if(!csv.LoadTableFile("sv_table\\LevHelp.csv"))
	{
		return;
	}

	int nStart = 2; //从2开始
	int nCount = csv.GetLineCount();
	for(int i=nStart; i<nCount; ++i)
	{
		strKey		= csv[i][0].SafeStr();
		strValue	= csv[i][1].SafeStr();

		if (strKey.empty())
		{
			continue;
		}

		int nKey = atoi(strKey.c_str());

		if (m_mapLevHtml.find(nKey) == m_mapLevHtml.end())
		{
			m_vecLev.push_back(nKey);
			m_mapLevHtml[nKey] = strValue;
		}
	}

	SetVisable();
}

//设置等级
void CUIForm_NewPeopleHelp::SetLev(int nLev, bool bIsLevup)
{
// 	if (!m_bVisableEnable)
// 	{
// 		return;
// 	}
// 
// 	Hide();
// 
// 	if (nLev<0)
// 	{
// 		//越界退出
// 		return;
// 	}
// 
// 	if (find(m_vecLev.begin(), m_vecLev.end(), nLev) == m_vecLev.end())
// 	{
// 		return;
// 	}
// 
// 	if (m_mapLevHtml.find(nLev) == m_mapLevHtml.end())
// 	{
// 		//表里没有此等级内容
// 		return;
// 	}
// 
// 	for (int i=0; i<(int)m_vecLev.size(); ++i)
// 	{
// 		if (m_vecLev[i] == nLev)
// 		{
// 			m_nCurrentPage = i+1;
// 			break;
// 		}
// 	}
// 
// 	//从文件加载html
// 	std::string strFileName = "task/Levelup/";
//  	strFileName += m_mapLevHtml[nLev];
// 
// 	if (!m_htmlHelpInfo->LoadFromFile(strFileName))
// 	{
// 		return;
// 	}
// 
// 	if (!m_fmNewPeopleHelp->IsVisible())
// 	{
// 		//不可见,就显示
// 		m_fmNewPeopleHelp->Show();
// 	}
// 
// 	sprintf(m_szTmp, "%d", m_nCurrentPage);
// 	m_lblPages->SetText(m_szTmp);
}

void CUIForm_NewPeopleHelp::MoveToPage(int nPage)
{
	int nLev = GetWorld()->m_pPlayer->m_core.Lev;
	if (nPage<=0 || nPage>(int)m_vecLev.size() || m_vecLev[nPage-1]>nLev)
	{
		//越界
		return;
	}

	SetLev(m_vecLev[nPage-1]);
}

void CUIForm_NewPeopleHelp::OnBtnLeft(ui::RtwWidget* sender,RtwEventDelegate* e)
{
	if (m_nCurrentPage == 1)
	{
		return;
	}

	MoveToPage(--m_nCurrentPage);
}

void CUIForm_NewPeopleHelp::OnBtnRight(ui::RtwWidget* sender,RtwEventDelegate* e)
{
	if (m_nCurrentPage >= (int)m_mapLevHtml.size())
	{
		return;
	}

	MoveToPage(++m_nCurrentPage);
}

void CUIForm_NewPeopleHelp::ShowUserLev()
{
	Hide();
// 
// 	if (!m_bVisableEnable)
// 	{
// 		return;
// 	}
// 
// 	int nLev = GetWorld()->m_pPlayer->m_core.Lev;
// 	if (nLev<0)
// 	{
// 		//越界退出
// 		return;
// 	}
// 
// 	if (find(m_vecLev.begin(), m_vecLev.end(), nLev) == m_vecLev.end())
// 	{
// 		return;
// 	}
// 
// 	if (m_mapLevHtml.find(nLev) == m_mapLevHtml.end())
// 	{
// 		//表里没有此等级内容
// 		return;
// 	}
// 
// 	m_fmNewPeopleTips->Show();
}

void CUIForm_NewPeopleHelp::SetVisable(bool bVisableEnable /* = false */)
{
	m_bVisableEnable = bVisableEnable;
}

void CUIForm_NewPeopleHelp::OnTips(ui::RtwWidget *sender, ui::RtwEventDelegate *e)
{
// 	SetLev(GetWorld()->m_pPlayer->m_core.Lev, true);
// 	m_fmNewPeopleTips->Hide();

}