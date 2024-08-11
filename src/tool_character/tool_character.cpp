// tool_character.cpp : 定义应用程序的类行为。
//
#include "stdafx.h"
#include "tool_character.h"
#include "MainFrm.h"

#include "tool_characterDoc.h"
#include "tool_characterView.h"
#include "EditorFrame.h"
//#include <CrashReport/Export.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Ctool_characterApp
BEGIN_MESSAGE_MAP(Ctool_characterApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// Ctool_characterApp 构造
Ctool_characterApp::Ctool_characterApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

RtgDevice *g_render=NULL;
// 唯一的一个 Ctool_characterApp 对象
Ctool_characterApp theApp;
// Ctool_characterApp 初始化
BOOL Ctool_characterApp::InitInstance()
{
   // CrashRP::Start();

	RT_STATIC_REGISTRATION_CLASS(CEditorFrame);
	if(!rtCoreInit("tool_character.ini"))
	{
		return false;
	}



    //if(!RtGetRender()->Init(NULL, RT_RUNTIME_CLASS(CEditorFrame), RT_RUNTIME_CLASS(RtgCameraEditor), "clt_engine.ini", "user.ini"))
    //{
    //    CHECKEX("初始化图形设备失败");
    //    return FALSE;
    //}

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	//LoadStdProfileSettings(8);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ctool_characterDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(Ctool_characterView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
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


	return TRUE;
}

int Ctool_characterApp::ExitInstance()
{
    //SafeDelete(g_activeActor);
    
	//Delete Test Case
	g_pAppCase->Close();
	rtGraphExit();

    ActorExit();
	rtGraphExit();
	rtCoreExit();
  //  CrashRP::Stop();
	DEL_ONE(g_pAppCase); // Event 必须在 rtGraphExit 之后Delete

	return 0;
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
void Ctool_characterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ctool_characterApp 消息处理程序


BOOL Ctool_characterApp::OnIdle(LONG lCount)
{
    CWinApp::OnIdle(lCount);

    if( AfxGetApp()->m_pMainWnd->IsIconic() )
        return TRUE;
    if(RtGetRender())
        RtGetRender()->RenderScene();

    return TRUE;
}
