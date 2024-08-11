//
// tool_characterView.cpp : Ctool_characterView 类的实现
//
#include "stdafx.h"
#include "tool_character.h"
#include "tool_characterDoc.h"
#include "tool_characterView.h"
#include "EditorFrame.h"
#include "MainFrm.h"
#include "DlgNewShader.h"
#include "DlgMergeActor.h"
#include "DlgCreateEffect.h"
#include "DlgVerFix.h"
#include "DlgPoseConn.h"
#include "DlgRelatedActor.h"
#include ".\tool_characterview.h"
#include "MergeActorDlg.h"

// Ctool_characterView
CEditorFrame* g_pAppCase = NULL;
bool g_bsave = false;

IMPLEMENT_DYNCREATE(Ctool_characterView, CView)
BEGIN_MESSAGE_MAP(Ctool_characterView, CView)
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
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_NEW_SHADER, OnNewShader)
	ON_COMMAND(ID_LINK, OnLink)
	ON_COMMAND(ID_OPEN_ACTOR, OnOpenActor)
	ON_COMMAND(ID_SAVE_AS_ACTOR, OnSaveAsActor)
	ON_COMMAND(ID_SAVE_AS_SKIN, OnSaveAsSkin)
	ON_COMMAND(ID_SAVE_ALL, OnSaveAll)
	ON_COMMAND(ID_MERGE_ACTOR, OnMergeActor)
	ON_COMMAND(ID_NEW_EFFECT, OnNewEffect)
	ON_COMMAND(ID_VER_FIX, OnVerFix)
	ON_COMMAND(IDC_ANIM_PLAY, OnBnClickedAnimPlay)
	ON_COMMAND(IDC_ANIM_STOP, OnBnClickedAnimStop)
	ON_COMMAND(IDC_ANIM_LOOP, OnBnClickedAnimLoop)
	ON_COMMAND(IDC_POSE_CONN, OnBnClickedPoseConn)
    ON_COMMAND(IDC_SHOWLINK, OnBnClickedShowLink)
	ON_COMMAND(IDC_POSE_LIST, OnBnClickedPoseList)
	ON_COMMAND(IDC_MESH_VIEW, OnBnClickedMeshView)
	ON_COMMAND(IDC_PARAMETER, OnBnClickedParameter)
    ON_WM_DESTROY()
	ON_COMMAND(ID_SAVE_AS_EFFECT, OnSaveAsEffect)
	ON_COMMAND(ID_SAVE_AS_MTLLIB, OnSaveAsMtllib)
	ON_COMMAND(ID_OPEN_RELATED, OnOpenRelated)
	ON_COMMAND(ID_MODIFY_RELATED_ACTOR, OnModifyRelatedActor)
	ON_COMMAND(ID_GET_RELATED_RES, OnGetRelatedRes)
	ON_COMMAND(ID_COPY_EFFECT, OnCopyEffect)
END_MESSAGE_MAP()


// Ctool_characterView 构造/析构
Ctool_characterView::Ctool_characterView()
    : m_bShowBone(false)
{
	// TODO: 在此处添加构造代码
}

Ctool_characterView::~Ctool_characterView()
{

}
void Ctool_characterView::OnBnClickedMeshView()
{
	g_pAppCase->ChangeFillMode();
}

void Ctool_characterView::OnBnClickedAnimPlay()
{
	if(g_activeActor)
	{
		CMainFrame *main;
		main = (CMainFrame*)AfxGetMainWnd();
		string pose = main->GetListSelectedPose();
		g_activeActor->PlayPose(pose);
		/*if(g_activeActor->m_weapon)
		   g_activeActor->m_weapon->PlayPose(pose.c_str());*/
        for (size_t i = 0; i < g_activeActor->m_vSubActor.size(); ++i)
            g_activeActor->m_vSubActor[i]->PlayPose(pose.c_str());
	}
}

void Ctool_characterView::OnBnClickedAnimLoop()
{
	if(g_activeActor)
	{
		CMainFrame *main;
		main = (CMainFrame*)AfxGetMainWnd();
		string pose = main->GetListSelectedPose();
		g_activeActor->PlayPose(pose,true);
		//if(g_activeActor->m_weapon)
		//   g_activeActor->m_weapon->PlayPose(pose.c_str(),true);
        for (size_t i = 0; i < g_activeActor->m_vSubActor.size(); ++i)
            g_activeActor->m_vSubActor[i]->PlayPose(pose.c_str(), true);
	}
}

void Ctool_characterView::OnBnClickedShowLink()
{
    if (!g_activeActor)
        return;

    g_activeActor->m_inst->ShowBone(m_bShowBone = !m_bShowBone);

}

void Ctool_characterView::OnBnClickedAnimStop()
{
	if(g_activeActor)
		g_activeActor->StopPose();
	//if(g_activeActor->m_weapon)
	//	g_activeActor->m_weapon->StopPose();
    for (size_t i = 0; i < g_activeActor->m_vSubActor.size(); ++i)
        g_activeActor->m_vSubActor[i]->StopPose();
}

void Ctool_characterView::OnBnClickedPoseConn()
{
	CDlgPoseConn dlg;
	dlg.DoModal();
}

void Ctool_characterView::OnBnClickedPoseList()
{

}

BOOL Ctool_characterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式
	return CView::PreCreateWindow(cs);
}

// Ctool_characterView 绘制
void Ctool_characterView::OnDraw(CDC* /*pDC*/)
{
	Ctool_characterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

// Ctool_characterView 诊断
#ifdef _DEBUG
void Ctool_characterView::AssertValid() const
{
	CView::AssertValid();
}

void Ctool_characterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ctool_characterDoc* Ctool_characterView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ctool_characterDoc)));
	return (Ctool_characterDoc*)m_pDocument;
}
#endif //_DEBUG

//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
/*
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    static POINT ptMouse, ptCurPos;
    static bool bMouseCapture=false;
    if (msg>=WM_MOUSEFIRST && msg<=WM_MOUSELAST)
    {
        ptCurPos.x = (short)LOWORD(lParam);
        ptCurPos.y = (short)HIWORD(lParam);
    }
    switch (msg) 
    {
		case WM_CHAR:
			g_pAppCase->KeyChar(lParam, wParam);
			break;
		case WM_KEYDOWN:				//按钮按下 
			g_pAppCase->KeyDown(lParam, wParam);
			break; 
		case WM_KEYUP:					//按钮弹起
			if (wParam==VK_ESCAPE) RtGetRender()->SetViewMode(RTGVWM_WINDOWED);
			g_pAppCase->KeyUp(lParam, wParam);
			break;
		case WM_LBUTTONDOWN:			//鼠标左键被按下
			ptMouse = ptCurPos;
			g_pAppCase->MouseLDown(wParam, ptCurPos.x, ptCurPos.y);
			break;
		case WM_MBUTTONDOWN:
			ptMouse = ptCurPos;
			g_pAppCase->MouseMDown(wParam, ptCurPos.x, ptCurPos.y);
			break;
		case WM_RBUTTONDOWN:			//鼠标右键被按下
			ptMouse = ptCurPos;
			g_pAppCase->MouseRDown(wParam, ptCurPos.x, ptCurPos.y);
			break;
		case WM_LBUTTONUP:				//鼠标左键弹起
			g_pAppCase->MouseLUp(wParam, ptCurPos.x, ptCurPos.y);
			if (GetCapture()) ReleaseCapture();
			break;
		case WM_MBUTTONUP:
			g_pAppCase->MouseMUp(wParam, ptCurPos.x, ptCurPos.y);
			if (GetCapture()) ReleaseCapture();
			break;
		case WM_RBUTTONUP:				//鼠标右键被弹起
			g_pAppCase->MouseRUp(wParam, ptCurPos.x, ptCurPos.y);
			if (GetCapture())
			{
				RECT rt;
				GetClientRect( hWnd, &rt );
				ReleaseCapture();
				if (!PtInRect(&rt, ptCurPos))
					return 0;
			}
			break;
		case WM_LBUTTONDBLCLK:			//鼠标左键双击
			g_pAppCase->MouseLDClick(wParam, ptCurPos.x, ptCurPos.y);
			break;
		case WM_MBUTTONDBLCLK:
			g_pAppCase->MouseMDClick(wParam, ptCurPos.x, ptCurPos.y);
			break;
		case WM_RBUTTONDBLCLK:			//鼠标右键双击
			g_pAppCase->MouseRDClick(wParam, ptCurPos.x, ptCurPos.y);
			break;
		case WM_MOUSEWHEEL:
			g_pAppCase->MouseWheel((short)LOWORD(wParam), ((short)HIWORD(wParam))/WHEEL_DELTA,ptMouse.x, ptMouse.y);
			break;
		case WM_MOUSEMOVE:		//鼠标移动消息
			if(wParam&MK_LBUTTON)
			{
				g_pAppCase->MouseLDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
				SetCapture(hWnd);
			}else if(wParam&MK_MBUTTON)
			{
				g_pAppCase->MouseMDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
				SetCapture(hWnd);
			}else if(wParam&MK_RBUTTON)
			{
				g_pAppCase->MouseRDrag(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
				SetCapture(hWnd);
			}else
			{
				g_pAppCase->MouseMove(wParam, ptCurPos.x, ptCurPos.y, ptCurPos.x-ptMouse.x, ptCurPos.y-ptMouse.y);
			}
			ptMouse = ptCurPos;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			//case WM_CLOSE:
			//case WM_SETCURSOR:
    }

    if(RtGetRender()) return RtGetRender()->DeviceMsgProc(hWnd, msg, wParam, lParam);
    if(msg == WM_CLOSE)
    {
		// User wants to exit, so go back to windowed mode and exit for real
		g_AppFormView->OnToggleFullScreen();
		g_App.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }
	else if( msg == WM_SETCURSOR )
    {
		SetCursor( NULL );
    }
    // return DefWindowProc( hWnd, msg, wParam, lParam );
}
*/

// Ctool_characterView 消息处理程序
void Ctool_characterView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    g_pAppCase = new CEditorFrame;

    m_hWndRenderWindow = m_hWnd;

    if (!rtGraphInit())
    {
        CHECKEX("初始化图形引擎失败");
    }



	RtGetRender()->SetCustomWndHandle(m_hWnd);
	if(!RtGetRender()->Init(NULL, RT_RUNTIME_CLASS(CEditorFrame), RT_RUNTIME_CLASS(RtgCameraGame)))
	{
		return;
	}
	g_pAppCase = (CEditorFrame*)(RtGetRender()->GetEvent());
    g_pAppCase->m_pView = this;
	RtGetRender()->LockFps(1);
    
    // RtRegistrationClass::DumpObjectClass(g_pLog);
    Run();
}

bool bDestory = false;
void Ctool_characterView::OnDestroy()
{
	if(bDestory) return;
	bDestory = true;

	CView::OnDestroy();
    Pause();

	 ////Delete Test Case
	 //g_pAppCase->Close();
	 //rtGraphExit();
	 //DEL_ONE(g_pAppCase); // Event 必须在 rtGraphExit 之后Delete
}

void Ctool_characterView::OnTimer(UINT nIDEvent)
{
    if(AfxGetApp()->m_pMainWnd->IsIconic())
        return;

    // Pause();
	int beg = rtMilliseconds();
    // if (g_pAppCase) g_pAppCase->Render();
	if(RtGetRender())
		RtGetRender()->RenderScene();
    // LOG1("render time = %d\n",rtGetMilliseconds() - beg);
	
	CView::OnTimer(nIDEvent);
	// Run();
	MSG msg;
	while(PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));
}

void Ctool_characterView::Pause()
{
    KillTimer(0);
}

void Ctool_characterView::Run()
{
#ifdef _DEBUG 
	SetTimer(0, 10, NULL);
#else
	SetTimer(0, 10, NULL);
#endif
}

void Ctool_characterView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    g_pAppCase->OnKeyChar( nChar,nFlags);
    CView::OnChar(nChar, nRepCnt, nFlags);
}

void Ctool_characterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    g_pAppCase->OnKeyDown( nChar,nFlags);
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Ctool_characterView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    g_pAppCase->OnKeyUp( nChar,nFlags);
    CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void Ctool_characterView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    m_ptMousePos = m_ptMouseCurPos = point;
    g_pAppCase->OnMouseLDown(nFlags, point.x, point.y);
    CView::OnLButtonDown(nFlags, point);
}

void Ctool_characterView::OnLButtonUp(UINT nFlags, CPoint point) 
{
    m_ptMouseCurPos = point;
    g_pAppCase->OnMouseLUp(nFlags, point.x, point.y);
    if (GetCapture()) ReleaseCapture();
    CView::OnLButtonUp(nFlags, point);
}

void Ctool_characterView::OnMButtonDown(UINT nFlags, CPoint point) 
{
    m_ptMousePos = m_ptMouseCurPos = point;
    g_pAppCase->OnMouseMDown(nFlags, point.x, point.y);
    CView::OnMButtonDown(nFlags, point);
}

void Ctool_characterView::OnMButtonUp(UINT nFlags, CPoint point) 
{
    m_ptMouseCurPos = point;
    g_pAppCase->OnMouseMUp(nFlags, point.x, point.y);
    if (GetCapture()) ReleaseCapture();
    CView::OnMButtonUp(nFlags, point);
}

void Ctool_characterView::OnRButtonDown(UINT nFlags, CPoint point) 
{
    m_ptMousePos = m_ptMouseCurPos = point;
    g_pAppCase->OnMouseRDown(nFlags, point.x, point.y);
    CView::OnRButtonDown(nFlags, point);
}

void Ctool_characterView::OnRButtonUp(UINT nFlags, CPoint point) 
{
    m_ptMouseCurPos = point;
    g_pAppCase->OnMouseRUp(nFlags, point.x, point.y);
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
}

void Ctool_characterView::OnMouseMove(UINT nFlags, CPoint point) 
{
    m_ptMouseCurPos = point;
    if(nFlags&MK_LBUTTON)
    {
        g_pAppCase->OnMouseLDrag(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
        SetCapture();
    }else if(nFlags&MK_MBUTTON)
    {
        g_pAppCase->OnMouseMDrag(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
        SetCapture();
    }else if(nFlags&MK_RBUTTON)
    {
        g_pAppCase->OnMouseRDrag(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
        SetCapture();
    }else
    {
        g_pAppCase->OnMouseMove(nFlags, point.x, point.y, point.x-m_ptMousePos.x, point.y-m_ptMousePos.y);
    }
    m_ptMousePos = m_ptMouseCurPos;

    CView::OnMouseMove(nFlags, point);
}

BOOL Ctool_characterView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    g_pAppCase->OnMouseWheel(nFlags, zDelta*0.01f,pt.x,pt.y);
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT Ctool_characterView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (RtGetRender()) return RtGetRender()->DeviceMsgProc(m_hWnd, message, wParam, lParam);
    return CView::DefWindowProc(message, wParam, lParam);
}

void Ctool_characterView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);
    if (RtGetRender()) RtGetRender()->OnWindowSizeChanged();
}

void Ctool_characterView::OnFileOpen()
{

}

void Ctool_characterView::OnNewShader()
{
	CDlgNewShader dlg;
	dlg.DoModal();
}

// #define PER_TEST
void Ctool_characterView::OnOpenActor()
{
	OpenActor(false);
}

void Ctool_characterView::OpenActor(bool related)
{
	// TODO: Add your command handler code here
	char filter[] = "Actor Files (*.act)|*.act||";
	char bak[255];
	char path[255];

	// save path
	GetCurrentDirectory(255,bak); 
	strcpy(path,bak);
	strcat(path,"\\creature\\actor\\_floor.act");
	CFileDialog dlg(TRUE,NULL,path,NULL,filter,NULL);
	if(dlg.DoModal() != IDOK) return;
	CString file = dlg.GetFileTitle();

	// restore path
	SetCurrentDirectory(bak);
	if(file.GetLength() <=0 ) return;
	
#ifdef PER_TEST	
	for(int i=0; i<10; i++)
	{
#endif
//		CRT_ActorInstance *cha = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT, file);
		// Modified by Wayne Wong 2010-11-09
		//CRT_ActorInstance *cha = LoadActorInstance(file);
		CRT_ActorInstance *cha = RtcGetActorManager()->CreateActor(file, true);
		if(!cha)
		{
			MessageBox("load file failed");
			//delete cha;
		}
		else
		{
			DelActiveActor();
/*
			if(related)
                cha->m_actor->LoadRelatedActorFile();*/

			cha->Create();
#ifdef PER_TEST
			cha->PlayPose("pp",true);
			cha->GetWorldMatrix()->_30 = i*5; 
			cha->GetWorldMatrix()->_31 = i*5;
#else
			
#endif
			CMainFrame *main;
			main = (CMainFrame*)AfxGetMainWnd();
			CToolActor *actor = CreateToolActor();
			SetActiveActor(actor);
			actor->m_inst = cha;
			actor->m_data = cha->GetCore();
			cha->RegisterNotify(actor);
			main->RebuildPoseList();
			// main->m_viewSlider.SetAnimRange(0,actor->GetFrameNum()-1,0);
			main->CreateAllGrid();
			g_pAppCase->OnKeyDown(0,VK_F9);
			// file 
			g_openFile = dlg.GetPathName();
		}
#ifdef PER_TEST	
	}
#endif
	g_bRelatedMode = related;
}

void Ctool_characterView::OnLink()
{
	if(g_activeActor)
		g_activeActor->ModifyWeapon();
}

void Ctool_characterView::OnSaveAsShaderBase()
{
	CMainFrame *main;
	main = (CMainFrame*)AfxGetMainWnd();
	// main->SaveAsShaderBase();
}

void Ctool_characterView::OnSaveAsEffectShader()
{
	CMainFrame *main;
	main = (CMainFrame*)AfxGetMainWnd();
	// main->SaveAsShaderEffect();
}

void Ctool_characterView::OnSaveAsActor()
{
	CMainFrame *main;
	main = (CMainFrame*)AfxGetMainWnd();
	main->SaveAsActor();
}

void Ctool_characterView::OnSaveAsSkin()
{
	CMainFrame *main;
	main = (CMainFrame*)AfxGetMainWnd();
	main->SaveAsSkin();
}

void Ctool_characterView::OnSaveAll()
{
	if(g_activeActor)
	{
		// R [12/23/2008 Louis.Huang]
/*
		RtArchive *ar = RtCoreFile().CreateFileWriter(NULL,g_activeActor->m_data->m_diskFile.c_str());
		if(!ar) 
		{
			MessageBox("保存失败");
			return;
		}

		ar->WriteObject(g_activeActor->m_data);
		if(!ar->IsError())
			MessageBox("保存成功");
		else
			MessageBox("保存失败");

		ar->Close();
		delete ar;
*/
		RtArchive *ar = NULL;
		if (g_activeActor->m_inst->Save(ar))
		{
			MessageBox("保存成功");
			return;
		}
		else
			MessageBox("保存失败");
	}
}

void Ctool_characterView::OnMergeActor()
{
    CMergeActorDlg dlg;
    dlg.DoModal();

    
	//CDlgMergeActor dlg;
	//dlg.DoModal();
}

void Ctool_characterView::OnNewEffect()
{
	CDlgCreateEffect dlg;
	dlg.DoModal();
}

void Ctool_characterView::OnVerFix()
{
#ifdef VER_FIX
	CDlgVerFix dlg;
	dlg.DoModal();
#endif
}

void Ctool_characterView::OnSaveAsEffect()
{
	CMainFrame *main;
	main = (CMainFrame*)AfxGetMainWnd();

	CRT_Effect *lib = main->GetSelectedEffect();
	if(!lib) return;

	char filter[] = "Effect Files (*.eft)|*.eft||";
	char bak[255];

	// save path
	GetCurrentDirectory(255,bak); 
	CFileDialog dlg(FALSE,NULL,NULL,NULL,filter,NULL);
	if(dlg.DoModal() != IDOK)
	{
		// restore path
		SetCurrentDirectory(bak);
		return;
	}	
	CString file = dlg.GetPathName();
	if( file[file.GetLength()-1] != 't' ||
		file[file.GetLength()-2] != 'f' ||
		file[file.GetLength()-3] != 'e' ||
		file[file.GetLength()-4] != '.' )
	{
		file += ".eft";
	}

	// restore path
	SetCurrentDirectory(bak);
	if(file.GetLength() <=0 ) return;

	RtArchive *ar = RtCoreFile().CreateFileWriter(NULL,file);
	if(!ar) return;
	ar->WriteObject(lib);
	ar->Close();
	delete ar;
}

void Ctool_characterView::OnSaveAsMtllib()
{
	CMainFrame *main;
	main = (CMainFrame*)AfxGetMainWnd();

	// C [12/24/2008 Louis.Huang]
/*
	CRT_MaterialLib *lib = main->GetSelectedMtlLib();
	if(!lib) return;
*/

	char filter[] = "MtlLib Files (*.mtl)|*.mtl||";
	char bak[255];

	// save path
	GetCurrentDirectory(255,bak); 
	CFileDialog dlg(FALSE,NULL,NULL,NULL,filter,NULL);
	if(dlg.DoModal() != IDOK)
	{
		// restore path
		SetCurrentDirectory(bak);
		return;
	}	
	CString file = dlg.GetPathName();

	// restore path
	SetCurrentDirectory(bak);
	if(file.GetLength() <=0 ) return;

	if( file[file.GetLength()-1] != 'l' ||
		file[file.GetLength()-2] != 't' ||
		file[file.GetLength()-3] != 'm' ||
		file[file.GetLength()-4] != '.' )
	{
		file += ".mtl";
	}

	RtArchive *ar = RtCoreFile().CreateFileWriter(NULL,file);
	if(!ar) return;

	CRT_SkinInstance* pSkin = main->GetSelectedSkinInstance();
	if (!pSkin) return;
	CRT_MaterialLib * lib = pSkin->GetMaterialLib();
	if (ar->WriteObject(lib))
		MessageBox("保存成功！");
	else
		MessageBox("保存失败！");
	ar->Close();
	delete ar;
}

void Ctool_characterView::OnOpenRelated()
{
	OpenActor(true);
}

void Ctool_characterView::OnModifyRelatedActor()
{
	CDlgRelatedActor dlg;
	dlg.DoModal();
}

void Ctool_characterView::OnGetRelatedRes()
{
	// TODO: Add your command handler code here
/*
	vector<string> ext;
	ext.push_back(".bmp");
	ext.push_back(".tga");
	if(g_activeActor)
	{
		vector<SRelatedRes> res;
		g_activeActor->m_data->GetRelatedRes(res);

		RtCoreLog().Info("related res = %d\n", res.size());
		for(int i=0; i<res.size(); i++)
		{
			RtCoreLog().Info("\t%d %s\n",res[i].type,res[i].res.c_str());
		}
		
		string path;
		path = "temp";
		char bak[255];
		GetCurrentDirectory(255,bak); 
		RtCoreFile().MakeDirectory("temp", FALSE);
		GetCurrentDirectory(255,bak); 
			
		int num = 0;
		for(int i=0; i<res.size(); i++)
		{
			for(int j=0; j<ext.size(); j++)
			{
				string src = "creature\\texture\\";
				src += res[i].res;
				src += ext[j];
				string dest = "temp\\";
				dest += res[i].res;
				dest += ext[j]; 
				if(CopyFile(src.c_str(),dest.c_str(),TRUE))
					num++;
			}
		}

		char msg[255];
		sprintf(msg, "复制了%d张贴图", num);
		MessageBox(msg);
	}
*/
}

void Ctool_characterView::OnCopyEffect()
{
/*
	if(!g_activeActor) return;
	CMainFrame *main = (CMainFrame*)AfxGetMainWnd();
	CRT_Effect *effect = main->GetSelectedEffect();
	if(!effect) return;

	CRT_Effect *tmp;
	tmp = (CRT_Effect*)ACTOR()->CopyFrom(effect);
	if(tmp)
	{
		CRT_Actor *actor;
		actor = g_activeActor->m_data;
		actor->m_eftVec.push_back(tmp);
		actor->DataChange(ON_ACTOR_EFFECT_ADD, actor->m_eftVec.size()-1);
		main->m_bRebuildGridActor = true;
	}*/

}
void Ctool_characterView::OnBnClickedParameter()
{
	g_bsave = !g_bsave;
}
