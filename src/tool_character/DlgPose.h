#pragma once
#include "afxwin.h"

// CDlgPose 对话框
struct SRT_Pose;
class CRT_ActorInstance;
class CDlgPose : public CDialog
{
	DECLARE_DYNAMIC(CDlgPose)

public:
	CDlgPose(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPose();

// 对话框数据
	enum { IDD = IDD_DIALOG_POSE };

	void RebuildEventList();
	SRT_Pose *m_pose;
    CRT_ActorInstance* m_act;
    map<string, string> m_typeMap;
    map<string, string> m_typeMap2;
    void UpdateLayout();
    void OnUpdateParam1();
    void OnUpdateParam2();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAddEvent();
	afx_msg void OnBnClickedButtonDelEvent();
	virtual BOOL OnInitDialog();
	CListBox m_list;
	afx_msg void OnLbnSelchangeListEvent();
	CString m_action;
	CString m_param;
	afx_msg void OnBnClickedButtonUpdate();
	long m_keyFrame;
    CString m_param1;
    CString m_param2;
    afx_msg void OnBnClickedButtonF0();
    afx_msg void OnCbnSetfocusEditEventParam1();
    afx_msg void OnCbnSelchangeEditEventAction();
    afx_msg void OnBnClickedOk();
};
