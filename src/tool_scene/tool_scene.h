// tool_scene.h : tool_scene 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// CSceneEditorApp:
// 有关此类的实现，请参阅 tool_scene.cpp
//

class CSceneEditorApp : public CWinApp
{
public:
	CSceneEditorApp();
	~CSceneEditorApp();


// 重写
public:
	virtual BOOL InitInstance();
	CString WorkingDirectory();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    afx_msg void OnHelpUse();

private:
	CString m_strWorkingDirectory;
public:
    virtual BOOL OnIdle(LONG lCount);
};

//extern RtgDevice *g_render;
//inline RtgDevice *RtGetRender()
//{
//	return g_render;
//}
extern CSceneEditorApp theApp;
