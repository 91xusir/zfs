#pragma once
#include "afxwin.h"


// CDlgNewShader dialog

class CDlgNewShader : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewShader)

public:
	CDlgNewShader(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewShader();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_SHADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCreate();
	CString m_combo;
	CComboBox m_comboCon;
	CString m_mtlName;
	bool    m_firstActive;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnCbnSelchangeCombo1();
};
