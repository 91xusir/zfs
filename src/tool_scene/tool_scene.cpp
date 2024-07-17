// tool_scene.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "tool_scene.h"
#include "BrushMapView.h"
#include "BrushVertexView.h"
#include "BrushColorView.h"
#include "area_list_view.h"
//tim.yang 路点列表头文件  //可以在MainFrame.h中包含CPathPointListView
#include "pathpoint_list_view.h"
#include "MainFrame.h"

#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include ".\tool_scene.h"
#include "HelpDialogUse.h"

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

// CSceneEditorApp

BEGIN_MESSAGE_MAP(CSceneEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_COMMAND(ID_HELP_USE, OnHelpUse)
END_MESSAGE_MAP()


// CSceneEditorApp 构造

CSceneEditorApp::CSceneEditorApp()
{

}

CSceneEditorApp::~CSceneEditorApp()
{
}

RtgDevice *g_render=NULL;
// 唯一的一个 CSceneEditorApp 对象

CSceneEditorApp theApp;

// CSceneEditorApp 初始化

BOOL CSceneEditorApp::InitInstance()
{
	LPTSTR buffer = m_strWorkingDirectory.GetBuffer(1024);
	GetCurrentDirectory(1023, buffer);
	m_strWorkingDirectory.ReleaseBuffer();

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	//LoadStdProfileSettings(8);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSceneEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CSceneEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当存在后缀时才调用 DragAcceptFiles，
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

CString CSceneEditorApp::WorkingDirectory()
{
	return m_strWorkingDirectory;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CSceneEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CSceneEditorApp 消息处理程序

void CSceneEditorApp::OnHelpUse()
{
    CHelpDialogUse helpDlg;
    helpDlg.DoModal();
}

BOOL CSceneEditorApp::OnIdle(LONG lCount)
{

    CWinApp::OnIdle(lCount);

    if( AfxGetApp()->m_pMainWnd->IsIconic() )
        return TRUE;
    if(RtGetRender())RtGetRender()->RenderScene();

    return TRUE;
}
