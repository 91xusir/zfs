#include "gc_include.h"
#include "ui_form_help.h"

UIFormHelp::UIFormHelp()
{
    m_pFrmThis  = RTW_WIDGET("fmhelpsys");
    m_pHtmlView = RTW_WIDGET_T(RtwHtmlView,"fmhelpsys.fmhelpright.viewright");
    m_pTree     = RTW_WIDGET_T(RtwTree,"fmhelpsys.treelist");

    m_pTree->EvSelect += RTW_CALLBACK(this, UIFormHelp, OnTreeNodeSelect);
 	m_pHtmlView->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_pHtmlView->SetEnableInput(false);
	m_pHtmlView->SetScrollBarV(true);
	m_pHtmlView->SetAutoScrollV(true);
}

UIFormHelp::~UIFormHelp()
{
}

bool UIFormHelp::IsVisible()
{
    guard;
    if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
    else	return false;
    unguard;
}

void UIFormHelp::OnShow(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	m_pFrmThis->Show();
	unguard;
}

void UIFormHelp::OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
    RtwTree::Item* pLastItem = m_pTree->GetLastSelect();
    if (pLastItem && pLastItem->link!="")
    {
        m_strURL = pLastItem->link;
    }else
    {
        m_strURL = "help/bank.html";
    }
    m_pHtmlView->LoadFromFile(m_strURL.c_str());
}

void UIFormHelp::OnUIHyperlink(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
    if (!IsVisible())
    {
        m_pFrmThis->Show();
    }
    m_strURL = ((RtwHyperlink*)pSender)->GetURL();
    if (m_strURL=="")
    {
        m_strURL = "help/bank.html";
    }
    m_pHtmlView->LoadFromFile(m_strURL.c_str());
}

void UIFormHelp::SetURL(const char* szURL)
{
    m_strURL = szURL?szURL:"";
    if (m_strURL=="")
    {
        m_strURL = "help/bank.html";
    }
    m_pHtmlView->LoadFromFile(m_strURL.c_str());
}

const char* UIFormHelp::GetURL()
{
    return m_strURL.c_str();
}
