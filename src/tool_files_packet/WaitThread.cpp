// WaitThread.cpp : 实现文件
//

#include "stdafx.h"
#include "tool_files_packet.h"
#include "WaitThread.h"


// CWaitThread

IMPLEMENT_DYNCREATE(CWaitThread, CWinThread)

CWaitThread::CWaitThread()
{
}

CWaitThread::~CWaitThread()
{
}

BOOL CWaitThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	m_pMainWnd = &m_wWaitDlg;
	m_wWaitDlg.Create(IDD_DIALOG_WAIT);
	m_wWaitDlg.ShowWindow(SW_SHOW);

	return TRUE;
}

int CWaitThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	m_wWaitDlg.DestroyWindow();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWaitThread, CWinThread)
END_MESSAGE_MAP()


// CWaitThread 消息处理程序
