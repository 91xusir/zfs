// tool_scene.h : tool_scene Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������


// CSceneEditorApp:
// �йش����ʵ�֣������ tool_scene.cpp
//

class CSceneEditorApp : public CWinApp
{
public:
	CSceneEditorApp();
	~CSceneEditorApp();


// ��д
public:
	virtual BOOL InitInstance();
	CString WorkingDirectory();

// ʵ��
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
