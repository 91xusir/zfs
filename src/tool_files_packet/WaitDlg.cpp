// WaitDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "tool_files_packet.h"
#include "WaitDlg.h"
#include ".\waitdlg.h"


// CWaitDlg 对话框

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


// CWaitDlg 消息处理程序


//andy
BOOL CWaitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_pProgress.SetRange(1,100);
	pos = 1;
	m_pProgress.SetPos(pos);
	SetTimer(1, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//andy
void CWaitDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
