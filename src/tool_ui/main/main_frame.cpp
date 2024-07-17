#include "studio.h"
#include "main_frame.h"
#include "driver_system.h"

RT_IMPLEMENT_DYNCREATE(CEventFrame, CRtgAppFrame, NULL, "")

CUiStudioProcess* getStudioProcess()
{
	/*LOG("");*/
    return getMainFrame()->m_pStudioProcess;
}
UiMain* getUiMain()
{
	/*LOG("");*/
    return g_pUiMain;
}


CEventFrame::CEventFrame(void)
{
	/*LOG("");*/
    m_pStudioProcess = NULL;
    m_StudioState = ss_Edit;

    m_bMouseLDown = false;
    m_bMouseRDown = false;
}

CEventFrame::~CEventFrame(void)
{
	/*LOG("");*/
}

bool CEventFrame::OnDeviceInit()
{
	/*LOG("");*/
    m_pDevice->m_pCamera->SetProjParams(DegreeToRadian(60.0f), 4.f/3.f, 10.0f, 1000.0f);
	m_pDevice->m_pCamera->SetViewParams(RtgVertex3(0.0f, -280.0f, 0.0f), RtgVertex3(0.0f, 0.0f, 0.0f), RtgVertex3(0.0f, 0.0f, 1.0f));
	m_pDevice->m_pCamera->UpdateMatrix();
	return true;
}

bool CEventFrame::OnRestoreDevice()
{
	/*LOG("");*/
	g_workspace.OnRestoreDevice();
	return true;
}

bool CEventFrame::OnInvalidateDevice()
{
	/*LOG("");*/
	g_workspace.OnInvalidateDevice();
	return true;
}

bool CEventFrame::OnFrameInit()
{
	/*LOG("");*/
	RtIni UserIni;
	UserIni.OpenFile("uistudio_user.ini");
	UserIni.CloseFile();
	if(!mDriverSystem.Init((string)UserIni["Driver"]["Type"],m_pDevice) )
	{
		return false;
	}
    // Init UiSystem
	if (!g_workspace.Create(
								mDriverSystem.GetRenderDriver(), 
								mDriverSystem.GetImageFactory(), 
								mDriverSystem.GetFontManager()
							)
	)
    {
        RtCoreLog().Error("创建UI失败\n");
        return false;
    }

    g_workspace.SetClipboard(mDriverSystem.GetClipboard());

    g_pUiMain = new UiMain();
    if (!g_pUiMain->Init())
    {
        RtCoreLog().Error("初始化Ui失败\n");
        return false;
    }

	m_pStudioProcess = new CUiStudioProcess();
    if (!m_pStudioProcess->Create())
    {
        RtCoreLog().Error("创建Studio失败");
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    HCURSOR hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ARROW));
    SetCursor(hCursor);

	return true;
}

void CEventFrame::OnFrameClose()
{
	/*LOG("");*/
    m_pStudioProcess->Destroy();
    DEL_ONE(m_pStudioProcess);
    DEL_ONE(g_pUiMain);
    g_workspace.Destroy();
}

void CEventFrame::OnRender()
{
	/*LOG("");*/
	mDriverSystem.RenderClear();

    if (m_StudioState == ss_Edit)
        m_pStudioProcess->Render();
    else 
        g_workspace.Render();

	mDriverSystem.RenderFilp();
}

void CEventFrame::OnFrameMove(float fDifTime)
{/*LOG("");*/
   if (m_StudioState == ss_Edit)
        m_pStudioProcess->Run(fDifTime*1000);
    else 
        g_workspace.Run(fDifTime*1000);

    g_pUiMain->Run(fDifTime*1000);
}

bool CEventFrame::OnErrorMessage(const char* szMsg, int iType)
{
	/*LOG("");*/
	return false;
}

void CEventFrame::OnMouseLDown(int x, int y)
{
	/*LOG("");*/
    m_bMouseLDown = true;

    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseDown(buttonLeft);
    }
    else 
    {
	    if (!g_workspace.MouseDown(buttonLeft))
	    {
		    //m_pStudio->OnMouseLDown(x, y);
	    }
    }
}

void CEventFrame::OnMouseLDrag(int x, int y, int increaseX, int increaseY)
{
	/*LOG("");*/
    m_bMouseLDown = true;

    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseMove(x, y);
    }
    else 
    {
        if (!g_workspace.MouseMove(x, y))
        {
            //m_pWorld->OnMouseLDrag(x, y, increaseX, increaseY);
        }
    }
}

void CEventFrame::OnMouseLUp(int x, int y)
{
	/*LOG("");*/
    m_bMouseLDown = false;

    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseUp(buttonLeft);
    }
    else 
    {
	    if (!g_workspace.MouseUp(buttonLeft))
	    {
		    //m_pWorld->OnMouseLUp(x, y);
	    }
    }
}

void CEventFrame::OnMouseRDown(int x, int y)
{
	/*LOG("");*/
    m_bMouseRDown = true;

    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseDown(buttonRight);
    }
    else 
    {
        if (!g_workspace.MouseDown(buttonRight))
        {
            //m_pWorld->OnMouseRDown(x, y);
        }
    }
}

void CEventFrame::OnMouseRDrag(int x, int y, int increaseX, int increaseY)
{
	/*LOG("");*/
    m_bMouseRDown = true;

    //m_pWorld->OnMouseRDrag(x, y, increaseX, increaseY);
}

void CEventFrame::OnMouseRUp(int x, int y)
{
	/*LOG("");*/
    m_bMouseRDown = false;

    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseUp(buttonRight);
    }
    else 
    {
        if (!g_workspace.MouseUp(buttonRight))
        {
            //m_pWorld->OnMouseRUp(x, y);
        }
    }
}

void CEventFrame::OnMouseMDown(int x, int y)
{
	/*LOG("");*/
    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseDown(buttonMiddle);
    }
    else 
    {
        if (!g_workspace.MouseDown(buttonMiddle))
        {
            //m_pWorld->OnMouseMDown(x, y);
        }
    }
}

void CEventFrame::OnMouseMDrag(int x, int y, int increaseX, int increaseY)
{
	/*LOG("");*/
    //m_pWorld->OnMouseMDrag(x, y, increaseX, increaseY);
}

void CEventFrame::OnMouseMUp(int x, int y)
{
	/*LOG("");*/
    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseUp(buttonMiddle);
    }
    else 
    {
        if (!g_workspace.MouseUp(buttonMiddle))
        {
            //m_pWorld->OnMouseMUp(x, y);
        }
    }
}

void CEventFrame::OnMouseMove(int x, int y, int increaseX, int increaseY)
{/*LOG("");*/
	if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseMove(x, y);
    }
    else 
    {
        if (!g_workspace.MouseMove(x, y))
        {
            //m_pWorld->OnMouseMove(x, y, increaseX, increaseY);
        }
    }
}

void CEventFrame::OnMouseWheel(long vDelta, int x, int y)
{
	/*LOG("");*/
    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->MouseWheel(vDelta, x, y);
    }
    else 
    {
        if (!g_workspace.MouseWheel(vDelta, x, y))
        {
            //m_pWorld->OnMouseWheel(vDelta);
        }
    }
}

void CEventFrame::OnKeyDown(int iChar, bool bAltDown)
{
	/*LOG("");*/
    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->KeyDown(iChar);
    }
    else 
    {
        if (g_workspace.KeyDown(iChar))
        {
            //m_pWorld->OnKeyDown(iChar);
        }
    }
}

void CEventFrame::OnKeyUp(int iChar, bool bAltDown)
{
	/*LOG("");*/
    if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->KeyUp(iChar);
    }
    else 
    {
        if (g_workspace.KeyUp(iChar))
		{

		}
	}
}

void CEventFrame::OnKeyChar(int iChar, bool bAltDown)
{
	if (iChar == 25) //ctrl + y
	{
		if (!g_pUiMain->m_pLay_Studio->m_UiDocument.Redo())
		{
			RtwMailBox *msg = g_workspace.getDefaultMessageBox();
			msg->Show("无法重做", "错误", true, false);
		}
		return;
	}
	else if (iChar == 26) //ctrl + z
	{
		if (!g_pUiMain->m_pLay_Studio->m_UiDocument.Undo())
		{
			RtwMailBox *msg = g_workspace.getDefaultMessageBox();
			msg->Show("无法撤销", "错误", true, false);
		}
		return;
	}
	/*LOG("");*/
	if (m_StudioState == ss_Edit)
    {
        m_pStudioProcess->KeyChar(iChar);
    }
    else 
    {
		g_workspace.KeyChar(iChar);
    }
}

void CEventFrame::ClearPreviewWidgets()
{
	/*LOG("");*/
    for (std::list<DWORD>::iterator iter = m_PreviewWidgets.begin(); iter != m_PreviewWidgets.end(); iter++)
    {
        DWORD WidgetId = *iter;
        g_workspace.RemoveWidget(WidgetId);
    }
    m_PreviewWidgets.clear();
}

#if DO_CONSOLE

bool CEventFrame::OnConsoleCommand(const char* szCommand)
{
	/*LOG("");*/
    char seps[] = " ,\t\n";
    char *token;

    token = strtok((char*)szCommand, seps);
    UI_ENSURE_B(token);

    if (stricmp(token, "print") == 0)
    {
        token = strtok(NULL, seps);
        if (token)
        {
            if (stricmp(token, "tree")==0)
            {
                g_workspace.PrintWidgetTree();
                return true;
            }
            else 
            {
                g_workspace.PrintWidgetTree(token);
                return true;
            }
        }
        return true;
    }
    else if (stricmp(token, "render")==0)
    {}  else {}
    return false;
}

#endif


RT_IMPLEMENT_DYNCREATE(CUiStudioCamera, RtgCameraGame, NULL, "")

CUiStudioCamera::CUiStudioCamera()
{
	/*LOG("");*/
    SetUpOffset(15.f);
}

CUiStudioCamera::~CUiStudioCamera()
{
	/*LOG("");*/
}
