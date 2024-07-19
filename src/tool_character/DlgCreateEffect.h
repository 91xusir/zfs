#pragma once
#include "afxwin.h"


// CDlgCreateEffect dialog

class CDlgCreateEffect : public CDialog
{
	DECLARE_DYNAMIC(CDlgCreateEffect)

public:
	CDlgCreateEffect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCreateEffect();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_EFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCreate();
	CComboBox m_comboCon;
	CString m_combo;
	bool m_firstActive;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	CString m_eftName;
};
