// WaitDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tool_files_packet.h"
#include "WaitDlg.h"
#include ".\waitdlg.h"


// CWaitDlg �Ի���

IMPLEMENT_DYNAMIC(CWaitDlg, CDialog)
CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
}

CWaitDlg::~CWaitDlg()
{
}

void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_pProgress);
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaitDlg ��Ϣ�������


//andy
BOOL CWaitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_pProgress.SetRange(1,100);
	pos = 1;
	m_pProgress.SetPos(pos);
	SetTimer(1, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
//andy
void CWaitDlg::OnTimer(UINT nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (pos<=100)
	{
		++pos;
	}
	else
	{
		pos = 1;
	}
	m_pProgress.SetPos(pos);
	CDialog::OnTimer(nIDEvent);
}
