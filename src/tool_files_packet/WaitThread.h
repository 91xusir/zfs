//andy
#pragma once


#include "WaitDlg.h"
// CWaitThread

class CWaitThread : public CWinThread
{
	DECLARE_DYNCREATE(CWaitThread)

protected:
	CWaitThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWaitThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CWaitDlg m_wWaitDlg;
};


