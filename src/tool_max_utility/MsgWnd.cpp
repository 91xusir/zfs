#include "MsgWnd.h"
#include "resource.h"
#include <Commctrl.h>

BOOL CALLBACK GoToProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch (message) 
    { 
        case WM_INITDIALOG: 
            return TRUE; 
	}
   	return false;
}

CMsgWnd::CMsgWnd()
{
	m_hwnd = NULL;
}

CMsgWnd::~CMsgWnd()
{
	if(m_hwnd)
		DestroyWindow(m_hwnd);
}

bool CMsgWnd::Create(HINSTANCE h,int res)
{
	m_hwnd = CreateDialog(h,MAKEINTRESOURCE(res),NULL,GoToProc); 
    if(!m_hwnd) 
	{
		int err;
		err = GetLastError();
		return false;
	}
	ShowWindow(m_hwnd, SW_SHOW); 
	HWND p;
	p = GetDlgItem(m_hwnd,IDC_TOTAL_PROGRESS);
	SendMessage(p,PBM_SETRANGE32,0,100);
	SendMessage(p,PBM_SETSTEP,1,0); 

	p = GetDlgItem(m_hwnd,IDC_CUR_PROGRESS);
	SendMessage(p,PBM_SETRANGE32,0,100);
	SendMessage(p,PBM_SETSTEP,1,0); 
    return true;            
}

void CMsgWnd::SetMsg(const char *msg)
{
	SetDlgItemText(m_hwnd,IDC_MESSAGE,msg);
	// RedrawWindow(m_hwnd,NULL,NULL,NULL);
}

void CMsgWnd::SetTotalProgress(int p)
{
	HWND h;
	h = GetDlgItem(m_hwnd,IDC_TOTAL_PROGRESS);
	SendMessage(h,PBM_SETPOS,p,0); 
}

void CMsgWnd::SetCurProgress(int p)
{
	HWND h;
	h = GetDlgItem(m_hwnd,IDC_CUR_PROGRESS);
	SendMessage(h,PBM_SETPOS,p,0); 
}

void CMsgWnd::Refresh()
{
	RedrawWindow(m_hwnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE); 
}
