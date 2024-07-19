// tool_character.cpp : ����Ӧ�ó��������Ϊ��
//
#include "stdafx.h"
#include "tool_character.h"
#include "MainFrm.h"

#include "tool_characterDoc.h"
#include "tool_characterView.h"
#include "EditorFrame.h"
#include <CrashReport/Export.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Ctool_characterApp
BEGIN_MESSAGE_MAP(Ctool_characterApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// Ctool_characterApp ����
Ctool_characterApp::Ctool_characterApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

RtgDevice *g_render=NULL;
// Ψһ��һ�� Ctool_characterApp ����
Ctool_characterApp theApp;
// Ctool_characterApp ��ʼ��
BOOL Ctool_characterApp::InitInstance()
{
    CrashRP::Start();

	RT_STATIC_REGISTRATION_CLASS(CEditorFrame);
	if(!rtCoreInit("tool_character.ini"))
	{
		return false;
	}



    //if(!RtGetRender()->Init(NULL, RT_RUNTIME_CLASS(CEditorFrame), RT_RUNTIME_CLASS(RtgCameraEditor), "clt_engine.ini", "user.ini"))
    //{
    //    CHECKEX("��ʼ��ͼ���豸ʧ��");
    //    return FALSE;
    //}

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	//LoadStdProfileSettings(8);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ctool_characterDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(Ctool_characterView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
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
    CrashRP::Stop();
	DEL_ONE(g_pAppCase); // Event ������ rtGraphExit ֮��Delete

	return 0;
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
void Ctool_characterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ctool_characterApp ��Ϣ�������


BOOL Ctool_characterApp::OnIdle(LONG lCount)
{
    CWinApp::OnIdle(lCount);

    if( AfxGetApp()->m_pMainWnd->IsIconic() )
        return TRUE;
    if(RtGetRender())
        RtGetRender()->RenderScene();

    return TRUE;
}
