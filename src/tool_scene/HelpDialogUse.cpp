// HelpDialogUse.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "HelpDialogUse.h"


#ifdef RT_DEBUG_NEW
#   undef new
#endif

// CHelpDialogUse dialog

IMPLEMENT_DYNCREATE(CHelpDialogUse, CDHtmlDialog)

CHelpDialogUse::CHelpDialogUse(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CHelpDialogUse::IDD, CHelpDialogUse::IDH, pParent)
{
}

CHelpDialogUse::~CHelpDialogUse()
{
}

void CHelpDialogUse::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CHelpDialogUse::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CHelpDialogUse, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CHelpDialogUse)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CHelpDialogUse message handlers

HRESULT CHelpDialogUse::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT CHelpDialogUse::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
