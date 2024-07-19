#pragma once


// CHelpDialogUse dialog

class CHelpDialogUse : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CHelpDialogUse)

public:
	CHelpDialogUse(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHelpDialogUse();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_HELP_DIALOG_USE, IDH = IDR_HTML_HELPDIALOGUSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
