#include "studio.h"

#define MAX_LOADSTRING 100

#ifndef WM_MOUSEWHEEL
#   define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#   define WHEEL_DELTA 120
#endif

HINSTANCE hInst;

BOOL				InitInstance(HINSTANCE, int);
BOOL				ExitInstance(HINSTANCE, int);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

std::string strWindowClass = "WindowClass";
CEventFrame* g_pEventFrame = NULL;
HWND g_hWndRenderWindow = 0;

CEventFrame* getMainFrame()
{/*LOG("");*/
	return g_pEventFrame;
}

class CCcxyCoreFactory : public RtCoreFactory
{
public:
	RT_DECLARE_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")
		virtual void InitLogRoot(RtLogType rootLog)
	{/*LOG("");*/
	}
};
RT_IMPLEMENT_DYNCREATE(CCcxyCoreFactory, RtCoreFactory, 0, "RtCore")

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	RtGetRender()->m_dwClearColor = 0x3030A030;
	//RtGetRender()->m_dwClearColor = 0xff000000;

	//rtDevice主循环
	RtGetRender()->Run();

	ExitInstance(hInstance, 0);
	return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	/*LOG("");*/
	hInst = hInstance;
	// #ifdef _DEBUG
	if (!rtCoreInit("uistudio_core.ini"))
	{
		MessageBox(NULL, "uistudio_core.ini not exsit!", NULL, NULL);
		return false;
	}
	// #else
	//    // Core初始化
	//    if (!rtCoreInit("uistudio_core.ini", RT_RUNTIME_CLASS(CCcxyCoreFactory)))
	//    {
	// 	   return false;
	//    }
	// #endif
		//图形引擎初始化
	if (!rtGraphInit())
	{
		RtCoreLog().Error("rtGraphInit fail :(\n");
		return 0;
	}

	// 
	// //	//使用D3D9初始化device
	// 	if (!rtCreateDeviceD3D9())
	// 	{
	// 		return false;
	// 	}
	// 

	if (!RtGetRender()->Init(hInstance, RT_RUNTIME_CLASS(CEventFrame), RT_RUNTIME_CLASS(CUiStudioCamera),"uistudio_graph.ini","uistudio_user.ini"))
	{
		return false;
	}
	//    pDevice->LockFps(30);
	if (!rtSoundInit("RT3D-Audio", (HWND)RtGetRender()->GetWndHandle()))
	{
		return FALSE;
	}
	//g_pSoundMgr->SetAudioPath("audio");

	g_pEventFrame = ((CEventFrame*)RtGetRender()->GetEvent());
	g_pEventFrame->m_bNotifyOnMouseMove = true;
	g_pEventFrame->EnableConsole(true);

	return TRUE;
}

BOOL ExitInstance(HINSTANCE hInstance, int nCmdShow)
{
	/*LOG("");*/
	rtDestroyDevice();
	rtGraphExit();
	rtAudioExit();
	rtCoreExit();

	return TRUE;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*LOG("");*/
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
