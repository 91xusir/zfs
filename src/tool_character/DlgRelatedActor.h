#pragma once
#include "afxwin.h"


// CDlgRelatedActor �Ի���

class CDlgRelatedActor : public CDialog
{
	DECLARE_DYNAMIC(CDlgRelatedActor)

public:
	CDlgRelatedActor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRelatedActor();

// �Ի�������
	enum { IDD = IDD_DIALOG_RELATED_ACTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReadd();
	CListBox m_listBox;
	virtual BOOL OnInitDialog();
};
