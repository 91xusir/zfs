#pragma once
#include "afxwin.h"


// CDlgPoseConn 对话框

class CDlgPoseConn : public CDialog
{
	DECLARE_DYNAMIC(CDlgPoseConn)

public:
	CDlgPoseConn(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPoseConn();

// 对话框数据
	enum { IDD = IDD_DIALOG_POSE_CONN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_pose1;
	CListBox m_pose2;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
