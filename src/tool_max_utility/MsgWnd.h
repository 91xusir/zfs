// MsgWnd.h: interface for the CMsgWnd class.
#ifndef __MSG_WND_H__
#define __MSG_WND_H__

#include <windows.h>
class CMsgWnd  
{
public:
	bool Create(HINSTANCE h,int res);
	void SetMsg(const char *msg);
	void SetTotalProgress(int p);
	void SetCurProgress(int p);
	void Refresh();

	CMsgWnd();
	virtual ~CMsgWnd();

private:
	HWND m_hwnd;  
};

#endif