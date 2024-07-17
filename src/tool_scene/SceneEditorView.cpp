// SceneEditorView.cpp : CSceneEditorView 类的实现
//

//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/xtime.hpp>

#include "stdafx.h"
#include "tool_scene.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "audio/rt_audio.h"

#include "brush_tool.h"
#include "EditorFrame.h"
#include ".\sceneeditorview.h"
#include "brush_tool_object.h"
#include "DlgNewScene.h"

#include "character/cha_basic.h"
#include "character/cha_anim.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif

//CEditorFrame* g_pAppCase = 0;
CRtgAppFrame* g_pAppCase = 0;
CSceneMusicApp* g_pMusicThread = 0;
//boost::thread* g_pSceneThread = 0;
//boost::mutex g_mutex ;
//RtsSceneThread* g_pSceneThread = NULL;

LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// CSceneEditorView

IMPLEMENT_DYNCREATE(CSceneEditorView, CView)

BEGIN_MESSAGE_MAP(CSceneEditorView, CView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
    ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
    //ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    //ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    //ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    //ON_COMMAND(ID_FILE_NEW, OnFileNew)

    ON_WM_CLOSE()
    ON_COMMAND_RANGE(IDS_MULTIOBJECT_NEAR, IDS_MULTIOBJECT_NEAR+20, OnMultiobjectSelect)
END_MESSAGE_MAP()

// CSceneEditorView 构造/析构

CSceneMusicApp::CSceneMusicApp()
{
	if (!rtMusicInit())
	{
		CHECKEX("初始化背景音乐失败");
	}
	g_pBackMusic->SetMusicPath("music");
}
void CSceneMusicApp::Play(const char* filename,bool loop)
{
	//LockThread();
	sprintf(m_szMusicName,filename);
	m_bloop = loop;
	m_bChangeMusic = true;
	//UnlockThread();
}
int CSceneMusicApp::OnRun()
{
	while (true)
	{
		if (m_bChangeMusic) 
		{
			g_pBackMusic->Stop();
			g_pBackMusic->Play(m_szMusicName,m_bloop);
			m_bChangeMusic = false;
		}
		Sleep(1);
	}
	

	return 0;
}

//void RtsSceneThread::Moveto(RtgVertex3 pos)
//{
//	//LockThread();
//	m_pos = pos;
//	m_bMove = true;
//	//UnlockThread();
//}
//int RtsSceneThread::OnRun()
//{
//	while(true)
//	{
//		if(m_bMove)
//		{
//			LockThread();
//			g_pScene->OnMoveTo(m_pos);
//            m_bMove = false;
//			UnlockThread();
//		}
//		Sleep(1);
//	}
//
//	
//	return 0;
//}

CSceneEditorView::CSceneEditorView()
{
    funguard;
    m_hWndRenderWindow = NULL;
    m_hWndRenderFullScreen = NULL;
    fununguard;
}

CSceneEditorView::~CSceneEditorView()
{
}
//CSceneLoader   载入线程
//void OnMoveTo(int)
//{
//	boost::mutex::scoped_lock lock(g_mutex, false);
//	while(1)
//	{
//		/*if (!lock.locked())
//			lock.lock();
//
//		g_pScene->OnMoveTo();
//
//		if(lock.locked())
//			lock.unlock();*/
//		Sleep(1);
//	}
//}
// CSceneEditorView 绘制

void CSceneEditorView::OnDraw(CDC* )
{
	CSceneEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

// CSceneEditorView 诊断

#ifdef _DEBUG
void CSceneEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSceneEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSceneEditorDoc* CSceneEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSceneEditorDoc)));
	return (CSceneEditorDoc*)m_pDocument;
}
#endif //_DEBUG


void CSceneEditorView::InitRtEngine()
{
    if (g_pAppCase) 
        return;

    g_pAppCase = new CEditorFrame;
    m_hWndRenderWindow = m_hWnd;

	if (!rtGraphInit())
    {
        RtCoreLog().Error("rtGraphInit fail :(\n");
        return;
    }

	RtGetRender()->SetCustomWndHandle(m_hWnd);
	if (!RtGetRender()->Init(NULL, RT_RUNTIME_CLASS(CEditorFrame), RT_RUNTIME_CLASS(RtgCameraEditor), "clt_engine.ini", "user.ini"))
	{
        RtCoreLog().Error("Device initialise fail :(\n");
		return;
	}
	g_pAppCase = (CEditorFrame*)(RtGetRender()->GetEvent());
	((CEditorFrame*)g_pAppCase)->m_pView = this;
    RtGetRender()->EnableDrawPref(true);

    if (!rtSoundInit("RT3D-Audio", m_hWndRenderWindow))
    {
        RtCoreLog().Error("rtSoundInit fail :(\n");
        return;
    }

    if (!ActorInit())
    {
        RtCoreLog().Error("ActorInit fail :(\n");
        return;
    }

    if (!rtSceneInit("RT3D-Scene"))
    {
        RtCoreLog().Error("rtSceneInit fail :(\n");
        return;
    }

    RtcGetActorManager()->AddSearchPath("scene/actor/");
    RtcGetActorManager()->AddSearchPath("scene/material/");
    RtcGetActorManager()->AddSearchPath("scene/effect/");


    g_pSoundMgr->SetAudioPath("audio");
	g_pMusicThread = new CSceneMusicApp();
	g_pMusicThread->Play("bg.ogg");
	g_pMusicThread->Start();

    if (!(CEditorFrame*)g_pAppCase->Init(RtGetRender()))
    {
        return;
    }

    ActorSetScene(g_pScene);
    Run();
}

void CSceneEditorView::ClearRtEngine()
{
    Pause();
    g_pAppCase->Close();
    rtAudioPreExit();
    rtAudioExit();
    rtScenePreExit();
    rtSceneExit();
    ActorExit();
    extern RtSceneTool* g_pSceneTool;
    g_pSceneTool = NULL;
    rtGraphExit();
}


// CSceneEditorView 消息处理程序
//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
//LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//{
//   // guard;
//    static POINT ptMouse, ptCurPos;
//    static bool bMouseCapture=false;
//    if (msg>=WM_MOUSEFIRST && msg<=WM_MOUSELAST)
//    {
//        ptCurPos.x = (short)LOWORD(lParam);
//        ptCurPos.y = (short)HIWORD(lParam);
//    }
//    switch (msg) 
//    {
//    case WM_CHAR:
//        g_pAppCase->KeyChar(lParam, wParam);
//        break;
//    case WM_KEYDOWN:				//按钮按下 
//        g_pAppCase->KeyDown(lParam, wParam);
//        break; 
//    case WM_KEYUP:					//按钮弹起
//        if (wParam==VK_ESCAPE) RtGetRender()->SetViewMode(RTGVWM_WINDOWED);
//        g_pAppCase->KeyUp(lParam, wParam);
//        break;
//    case WM_LBUTTONDOWN:			//鼠标左键被按下
//        ptMouse = ptCurPos;
//        g_pAppCase->MouseLDown(wParam, ptCurPos.x, ptCurPos.y);
//        break;
//    case WM_MBUTTONDOWN:
//        ptMouse = ptCurPos;
//        g_pAppCase->MouseMDown(wParam, ptCurPos.x, ptCurPos.y);
//        break;
//    case WM_RBUTTONDOWN:			//鼠标右键被按下
//        ptMouse = ptCurPos;
//        g_pAppCase->MouseRDown(wParam, ptCurPos.x, ptCurPos.y);
//        break;
//    case WM_LBUTTONUP:				//鼠标左键弹起
//        g_pAppCase->MouseLUp(wParam, ptCurPos.x, ptCurPos.y);
//        if (GetCapture()) ReleaseCapture();
//        break;
//    case WM_MBUTTONUP:
//        g_pAppCase->MouseMUp(wParam, ptCurPos.x, ptCurPos.y);
//        if (GetCapture()) ReleaseCapture();
//        break;
//    case WM_RBUTTONUP:				//鼠标右键被弹起
//        g_pAppCase->MouseRUp(wParam, ptCurPos.x, ptCurPos.y);
//        if (GetCapture())
//        {
//            RECT rt;
//            GetClientRect( hWnd, &rt );
//            ReleaseCapture();
//            if (!PtInRect(&rt, ptCurPos))
//                return 0;
//        }
//        break;
//    case WM_LBUTTONDBLCLK:			//鼠标左键双击
//        g_pAppCase->MouseLDClick(wParam, ptCurPos.x, ptCurPos.y);
//        break;
//    case WM_MBUTTONDBLCLK:
//        g_pAppCase->MouseMDClick(wParam, ptCurPos.x, ptCurPos.y);
//        break;
//    case WM_RBUTTONDBLCLK:			//鼠标右键双击
//        g_pAppCase->MouseRDClick(wParam, ptCurPos.x, ptCurPos.y);
//        break;
//    case WM_MOUSEWHEEL:
//		//g_pAppCase->MouseWheel((short)LOWORD(wParam), ((short)HIWORD(wParam))/WHEEL_DELTA);
//        g_pAppCase->MouseWheel((short)LOWORD(wParam), ((short)HIWORD(wParam))/WHEEL_DELTA,0,0);
//        break;
//    case WM_MOUSEMOVE:		//鼠标移动消息
//        if(wParam&MK_LBUTTON)
//        {
//            g_pAppCase->MouseLDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
//            SetCapture(hWnd);
//        }else if(wParam&MK_MBUTTON)
//        {
//            g_pAppCase->MouseMDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
//            SetCapture(hWnd);
//        }else if(wParam&MK_RBUTTON)
//        {
//            g_pAppCase->MouseRDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
//            SetCapture(hWnd);
//        }else
//        {
//            g_pAppCase->MouseMove(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
//        }
//        ptMouse = ptCurPos;
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//    //case WM_CLOSE:
//    //case WM_SETCURSOR:
//    }
//
//    if (RtGetRender()) return RtGetRender()->DeviceMsgProc(hWnd, msg, wParam, lParam);
//
//    /*
//    if( msg == WM_CLOSE )
//    {
//        // User wants to exit, so go back to windowed mode and exit for real
//        g_AppFormView->OnToggleFullScreen();
//        g_App.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
//    }else if( msg == WM_SETCURSOR )
//    {
//        SetCursor( NULL );
//    }
//    */
//    return DefWindowProc( hWnd, msg, wParam, lParam );
//   // guard_end_ex;
//}

int CSceneEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    int iResult = CView::OnCreate(lpCreateStruct);
    //InitRtEngine();
    return iResult;
}

void CSceneEditorView::OnDestroy()
{
	
    CView::OnDestroy();
    ClearRtEngine();
}

void CSceneEditorView::OnTimer(UINT nIDEvent) 
{
    //guard;
//    if( AfxGetApp()->m_pMainWnd->IsIconic() )
//        return;
//
//    //MSG msg;
//    //if (PeekMessage(&msg, m_hWnd, 0, 0, PM_NOREMOVE))
//    //{
//    //    return;
//    //}
//
////    Pause();
//    //if (g_pAppCase) g_pAppCase->Render();
//	if(RtGetRender())RtGetRender()->RenderScene();
	CView::OnTimer(nIDEvent);
  //  Run();
    //guard_end_ex;
}

void CSceneEditorView::Pause()
{
    KillTimer(0);
}

void CSceneEditorView::Run()
{
    //SetTimer(0, 0, NULL);
}

void CSceneEditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    //guard;
	//g_pAppCase->OnKeyChar( nChar,nRepCnt);
	CView::OnChar(nChar, nRepCnt, nFlags);
   // guard_end_ex;
}

void CSceneEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   // guard;
   ((CEditorFrame*)g_pAppCase)->OnKeyDown(nFlags, nChar);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
   // guard_end_ex;
}

void CSceneEditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   // guard;
    //g_pAppCase->OnKeyUp( nChar,nRepCnt);
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
   // guard_end_ex;
}

void CSceneEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
   // guard;
    m_ptMousePos = m_ptMouseCurPos = point;
    ((CEditorFrame*)g_pAppCase)->OnMouseLDown(nFlags, point.x, point.y);
	CView::OnLButtonDown(nFlags, point);
   // guard_end_ex;
}

void CSceneEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   // guard;
    m_ptMouseCurPos = point;
    ((CEditorFrame*)g_pAppCase)->OnMouseLUp(nFlags,point.x, point.y);
    if (GetCapture()) ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
   // guard_end_ex;
}

void CSceneEditorView::OnMButtonDown(UINT nFlags, CPoint point) 
{
   // guard;
    m_ptMousePos = m_ptMouseCurPos = point;
    ((CEditorFrame*)g_pAppCase)->OnMouseMDown(nFlags, point.x, point.y);
	CView::OnMButtonDown(nFlags, point);
   // guard_end_ex;
}

void CSceneEditorView::OnMButtonUp(UINT nFlags, CPoint point) 
{
  //  guard;
    m_ptMouseCurPos = point;
    ((CEditorFrame*)g_pAppCase)->OnMouseMUp(nFlags,point.x, point.y);
    if (GetCapture()) ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
  //  guard_end_ex;
}

void CSceneEditorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
  //  guard;
    m_ptMousePos = m_ptMouseCurPos = point;
    ((CEditorFrame*)g_pAppCase)->OnMouseRDown(nFlags,point.x, point.y);
	CView::OnRButtonDown(nFlags, point);
   // guard_end_ex;
}

void CSceneEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
   // guard;
    m_ptMouseCurPos = point;
    ((CEditorFrame*)g_pAppCase)->OnMouseRUp(nFlags,point.x, point.y);
    if (GetCapture())
    {
        RECT rt;
        this->GetClientRect(&rt);
        ReleaseCapture();
        if (!PtInRect(&rt, m_ptMouseCurPos))
        {
            return;
        }
    }
	CView::OnRButtonUp(nFlags, point);
   // guard_end_ex;
}

void CSceneEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
   // guard;
    m_ptMouseCurPos = point;
    if(nFlags&MK_LBUTTON)
    {
        ((CEditorFrame*)g_pAppCase)->OnMouseLDrag(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
        SetCapture();
    }else if(nFlags&MK_MBUTTON)
    {
        ((CEditorFrame*)g_pAppCase)->OnMouseMDrag(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
        SetCapture();
    }else if(nFlags&MK_RBUTTON)
    {
        ((CEditorFrame*)g_pAppCase)->OnMouseRDrag(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
        SetCapture();
    }else
    {
        g_pAppCase->OnMouseMove( point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
    }
    m_ptMousePos = m_ptMouseCurPos;

	CView::OnMouseMove(nFlags, point);
   // guard_end_ex;
}

BOOL CSceneEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  //  guard;
	//g_pAppCase->MouseWheel(nFlags, zDelta*0.01f);
    ((CEditorFrame*)g_pAppCase)->OnMouseWheel(nFlags,zDelta*0.01f);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
  //  guard_end_ex;
}

LRESULT CSceneEditorView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
   // guard;
    if (RtGetRender()) return RtGetRender()->DeviceMsgProc(m_hWnd, message, wParam, lParam);
	return CView::DefWindowProc(message, wParam, lParam);
   // guard_end_ex;
}

void CSceneEditorView::OnSize(UINT nType, int cx, int cy) 
{
   // guard;
    CView::OnSize(nType, cx, cy);
    if (RtGetRender()) RtGetRender()->OnWindowSizeChanged();
   // guard_end_ex;
}

void CSceneEditorView::OnFileNew()
{
   // guard;
    CDlgNewScene newDlg;
    if (IDOK==newDlg.DoModal())
    {
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        g_pScene->Init();
        if (!((RtSceneTool*)g_pScene)->ToolNewScene(newDlg.GetWidth()-1, newDlg.GetHeight()-1))
        {
            return;
        }
    }
   // guard_end_ex;
}
void CSceneEditorView::OnFileBack()
{
    return;
	RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
	CHAR tempPath[1024] = {0};
	char* tempName;
	tempName = strcat(tempPath,"\\scene\\temp\\temp.rts");

	g_pScene->Init();
	if(((RtSceneTool*)g_pScene)->ToolLoadScene(tempName))
	{
		RtGetRender()->m_pCamera->SetViewMatrix(tmp);
		RtGetRender()->m_pCamera->SetPosition(t_eye);
	}
    OnKeyDown(VK_ESCAPE,1,0);
}
void CSceneEditorView::OnFileOpen()
{
   // guard;
    static char BASED_CODE szFilter[] = "RT Scene Files (*.rts)|*.rts|All Files (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "rts", ".rts", OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        g_pScene->Init();
        if (!((RtSceneTool*)g_pScene)->ToolLoadScene(pathName))
        {
            //ERR1("Can't load height \"%s\"\n", pathName);
        }
    }
   // guard_end_ex;
}

void CSceneEditorView::OnFileSave()
{
   // guard;
    static char BASED_CODE szFilter[] = "RT Scene Files (*.rts)|*.rts|All Files (*.*)|*.*||";
    CFileDialog fileDlg(FALSE, "rts", ".rts", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        if (!((RtSceneTool*)g_pScene)->ToolSaveScene(pathName))
        {
            //ERR1("Can't load height \"%s\"\n", pathName);
        }
    }
   // guard_end_ex;
}

void CSceneEditorView::OnFileSaveAs()
{
   // guard;
    static char BASED_CODE szFilter[] = "RT Scene Files (*.rts)|*.rts|All Files (*.*)|*.*||";
    CFileDialog fileDlg(FALSE, "rts", ".rts", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        if (!((RtSceneTool*)g_pScene)->ToolSaveScene(pathName))
        {
            //ERR1("Can't load height \"%s\"\n", pathName);
        }
    }
   // guard_end_ex;
}

extern RtsObject* g_pMenuSelObject[20];

void CSceneEditorView::OnMultiobjectSelect(UINT uCmdID)
{
    if (uCmdID!=IDS_MULTIOBJECT_NEAR)
    {
        int iIdx = uCmdID - IDS_MULTIOBJECT_NEAR;
        CBrusherObject* pBrush = (CBrusherObject*)(((CEditorFrame*)g_pAppCase)->m_BrushTool.m_pBrusherList[CBrushTool::BT_OBJECT_ADJUST]);
        int i = 0;
        for (; iIdx>1 && g_pMenuSelObject[i]; iIdx--)
        {
            i ++;
        }
        pBrush->m_pLastSelectObject = g_pMenuSelObject[i];
    }
}

void CSceneEditorView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
    InitRtEngine();
}

