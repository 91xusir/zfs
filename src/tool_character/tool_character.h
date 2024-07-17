// tool_character.h : tool_character 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// Ctool_characterApp:
// 有关此类的实现，请参阅 tool_character.cpp
//

class Ctool_characterApp : public CWinApp
{
public:
	Ctool_characterApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance(); // return app exit code

	virtual BOOL OnIdle(LONG lCount);
// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Ctool_characterApp theApp;
