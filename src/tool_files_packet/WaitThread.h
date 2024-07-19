//andy
#pragma once


#include "WaitDlg.h"
// CWaitThread

class CWaitThread : public CWinThread
{
	DECLARE_DYNCREATE(CWaitThread)

protected:
	CWaitThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CWaitThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CWaitDlg m_wWaitDlg;
};


