#pragma once
#include "afxwin.h"


// CDlgPoseConn �Ի���

class CDlgPoseConn : public CDialog
{
	DECLARE_DYNAMIC(CDlgPoseConn)

public:
	CDlgPoseConn(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPoseConn();

// �Ի�������
	enum { IDD = IDD_DIALOG_POSE_CONN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_pose1;
	CListBox m_pose2;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
