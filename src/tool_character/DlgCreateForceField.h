#pragma once
#include "afxwin.h"


// CDlgCreateForceField dialog

class CDlgCreateForceField : public CDialog
{
	DECLARE_DYNAMIC(CDlgCreateForceField)

public:
	CDlgCreateForceField(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCreateForceField();

// Dialog Data
	enum { IDD = IDD_DIALOG_CREATE_FF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreate();
	CString m_name;
	CComboBox m_comboCon;
	CString m_combo;
	bool m_firstActive;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	/*CRT_ForceField *m_result;*/
};
