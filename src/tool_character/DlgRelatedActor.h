#pragma once
#include "afxwin.h"


// CDlgRelatedActor 对话框

class CDlgRelatedActor : public CDialog
{
	DECLARE_DYNAMIC(CDlgRelatedActor)

public:
	CDlgRelatedActor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRelatedActor();

// 对话框数据
	enum { IDD = IDD_DIALOG_RELATED_ACTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReadd();
	CListBox m_listBox;
	virtual BOOL OnInitDialog();
};
