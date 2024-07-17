//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	tool_files_packet.cpp
//	author:		ldr123
//	purpose:	
//********************************************************************

#include "stdafx.h"
#include "console_main.h"
#include "tool_files_packet.h"
#include "MainFrm.h"

#include "tool_files_packetDoc.h"
#include "tool_files_packetView.h"

BEGIN_MESSAGE_MAP(Ctool_files_packetApp, CWinApp)
END_MESSAGE_MAP()

Ctool_files_packetApp::Ctool_files_packetApp(){}

Ctool_files_packetApp theApp;

BOOL Ctool_files_packetApp::InitInstance()
{
	//得到控制台
	LPSTR CommandLine=GetCommandLine();
	string Arguments=CommandLine;
	string Temp;
	Temp=Arguments.substr(Arguments.find_last_of('"')+1);
	Arguments=Temp.substr(Temp.find_first_of(' ')+1);

	rtCoreInit("clt_engine.ini");

	if(Arguments.find("console")!=-1 || Arguments.find("command")!=-1)
	{

		AllocConsole();
		string Temp("files packet ");
		Temp+=Arguments;
		SetConsoleTitle(Temp.c_str());
		HANDLE s_hWin32StdOutConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD co = {640 , 480};
		SetConsoleScreenBufferSize(s_hWin32StdOutConsole, co);
		freopen("CONOUT$","w+t",stdout);
		freopen("CONOUT$","w+t",stderr);
		if(Arguments.find("console")!=-1)
		{
			freopen("CONIN$","r+t",stdin);
		}
		
		ConsoleMain();
		FreeConsole();
		return FALSE;
	}

	InitCommonControls();

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ctool_files_packetDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(Ctool_files_packetView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	CCommandLineInfo cmdInfo;
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}