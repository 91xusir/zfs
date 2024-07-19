// tool_scene.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "tool_scene.h"
#include "BrushMapView.h"
#include "BrushVertexView.h"
#include "BrushColorView.h"
#include "area_list_view.h"
//tim.yang ·���б�ͷ�ļ�  //������MainFrame.h�а���CPathPointListView
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_COMMAND(ID_HELP_USE, OnHelpUse)
END_MESSAGE_MAP()


// CSceneEditorApp ����

CSceneEditorApp::CSceneEditorApp()
{

}

CSceneEditorApp::~CSceneEditorApp()
{
}

RtgDevice *g_render=NULL;
// Ψһ��һ�� CSceneEditorApp ����

CSceneEditorApp theApp;

// CSceneEditorApp ��ʼ��

BOOL CSceneEditorApp::InitInstance()
{
	LPTSTR buffer = m_strWorkingDirectory.GetBuffer(1024);
	GetCurrentDirectory(1023, buffer);
	m_strWorkingDirectory.ReleaseBuffer();

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	//LoadStdProfileSettings(8);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSceneEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CSceneEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ���á�DDE ִ�С�
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������ں�׺ʱ�ŵ��� DragAcceptFiles��
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����
	// ������/��
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

CString CSceneEditorApp::WorkingDirectory()
{
	return m_strWorkingDirectory;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CSceneEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CSceneEditorApp ��Ϣ�������

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
