#pragma once
#include "afxwin.h"


// CDlgGetBone �Ի���

class CDlgGetBone : public CDialog
{
	DECLARE_DYNAMIC(CDlgGetBone)

public:
	CDlgGetBone(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgGetBone();

// �Ի�������
	enum { IDD = IDD_DLG_GET_BONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CString m_selectedBone;
	afx_msg void OnBnClickedOk();
	CListBox m_boneList;
};
