#pragma once
#include "afxwin.h"


// CDlgGetBone 对话框

class CDlgGetBone : public CDialog
{
	DECLARE_DYNAMIC(CDlgGetBone)

public:
	CDlgGetBone(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGetBone();

// 对话框数据
	enum { IDD = IDD_DLG_GET_BONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CString m_selectedBone;
	afx_msg void OnBnClickedOk();
	CListBox m_boneList;
};
