// tool_character.h : tool_character Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������


// Ctool_characterApp:
// �йش����ʵ�֣������ tool_character.cpp
//

class Ctool_characterApp : public CWinApp
{
public:
	Ctool_characterApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance(); // return app exit code

	virtual BOOL OnIdle(LONG lCount);
// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Ctool_characterApp theApp;
