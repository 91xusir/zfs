// DlgNewScene.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "DlgNewScene.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif


// CDlgNewScene dialog

IMPLEMENT_DYNCREATE(CDlgNewScene, CDHtmlDialog)

CDlgNewScene::CDlgNewScene(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CDlgNewScene::IDD, CDlgNewScene::IDH, pParent)
{
    this->m_bUseHtmlTitle = TRUE;
    m_iWidth = 100;
    m_iHeight = 100;
}

CDlgNewScene::~CDlgNewScene()
{
}

void CDlgNewScene::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
    DDX_DHtml_ElementValue(pDX, _T("TextWidth"), m_iWidth);
    DDX_DHtml_ElementValue(pDX, _T("TextHeight"), m_iHeight);
}

BOOL CDlgNewScene::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDlgNewScene, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDlgNewScene)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CDlgNewScene message handlers

HRESULT CDlgNewScene::OnButtonOK(IHTMLElement* /*pElement*/)
{
    this->UpdateData(TRUE);
    if (m_iWidth>256) m_iWidth = 256;
    else if (m_iWidth<1) m_iWidth = 1;
    if (m_iHeight>256) m_iHeight = 256;
    if (m_iHeight<1) m_iHeight = 1;
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT CDlgNewScene::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
