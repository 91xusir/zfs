#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMergeActorDlg dialog

class CMergeActorDlg : public CDialog
{
	DECLARE_DYNAMIC(CMergeActorDlg)

public:
	CMergeActorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMergeActorDlg();

// Dialog Data
	enum { IDD = IDD_MERGE_ACTOR_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAdd();
    CListBox m_ListActor;
    afx_msg void OnBnClickedMerge();
    afx_msg void OnBnClickedButtonClr();
    CProgressCtrl m_Progress;
    CString m_StcActorName;
    afx_msg void OnBnClickedActorDst();
};
