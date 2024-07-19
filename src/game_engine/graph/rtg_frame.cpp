         
#include <windows.h>
#include "rtg_graph_inter.h"
#include "rtg_i_util.h"
#include "rtg_console.h"

namespace rt_graph {

/*----------------------------------------------------------------------------
-   RtgDeviceEvent
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(RtgDeviceEvent, RtObject, 0, "RtGraph")

/*----------------------------------------------------------------------------
-   CRtgAppFrame
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNCREATE(CRtgAppFrame, RtgDeviceEvent, 0, "RtGraph")

CRtgAppFrame::CRtgAppFrame()
{
    m_bEnableConsole = false;
    m_bShowConsole = false;
    m_pConsole = NULL;
    m_bUseDefaultWindowProc = false;

    for (int i=0; i<256; i++)
    {
        m_bKeys[i] = false;
    }
    m_bMouseButtons[0] = m_bMouseButtons[1] = m_bMouseButtons[2] = false;
    m_iLastMousePos[0] = m_iLastMousePos[1] = 0;
    m_bNotifyOnMouseMove = false;
    m_bCaptureMouse = false;
}

CRtgAppFrame::~CRtgAppFrame()
{
}

bool CRtgAppFrame::Init(RtgDeviceD3D9* pDevice)
{
    if (!CheckInit())
        return false;
    m_pDevice = pDevice;
    return true;
}

HINSTANCE CRtgAppFrame::GetInstance()
{
    return (HINSTANCE)m_pDevice->GetAppInstance();
}

void* CRtgAppFrame::GetWndHandle()
{
    return m_pDevice->GetWndHandle();
}

void CRtgAppFrame::Close()
{
    if (m_pConsole)
    {
        m_pConsole->Close(m_pDevice);
    }
    this->OnFrameClose();
}

bool CRtgAppFrame::CheckInit()
{
//     // 检查版本
//     if( !D3DXCheckVersion( D3D_SDK_VERSION, D3DX_SDK_VERSION ) )
//     {
//         return false; // 版本错误
//     }
    return true;
}

int CRtgAppFrame::Run()
{
    //return rtGraphRun();
    return 0;
}

void CRtgAppFrame::Render()
{
    //rtGraphRender();
}

void CRtgAppFrame::OnKeyDown(int iChar, bool bAltDown){}
void CRtgAppFrame::OnKeyUp(int iChar, bool bAltDown) {}
void CRtgAppFrame::OnKeyChar(int iChar, bool bAltDown) {}

void CRtgAppFrame::OnMouseMove(int x, int y, int increaseX, int increaseY){}
void CRtgAppFrame::OnMouseLUp(int x, int y){}
void CRtgAppFrame::OnMouseLDrag(int x, int y, int increaseX, int increaseY){}
void CRtgAppFrame::OnMouseLDown(int x, int y){}
void CRtgAppFrame::OnMouseLDClick(int x, int y){}
void CRtgAppFrame::OnMouseMUp(int x, int y){}
void CRtgAppFrame::OnMouseMDown(int x, int y){}
void CRtgAppFrame::OnMouseMDClick(int x, int y){}
void CRtgAppFrame::OnMouseRUp(int x, int y){}
void CRtgAppFrame::OnMouseRDown(int x, int y){}
void CRtgAppFrame::OnMouseRDClick(int x, int y){}
void CRtgAppFrame::OnMouseWheel(long vDelta,int x,int y){}
void CRtgAppFrame::MouseWheel(long vDelta,int x,int y)
{
    m_pDevice->m_pCamera->MoveUp(vDelta*10.0f);
}

void CRtgAppFrame::OnMouseMDrag(int x, int y, int increaseX, int increaseY)
{
    if (m_pDevice->m_pCamera)
    {
        m_pDevice->m_pCamera->MoveRight(-increaseX*0.4);
        m_pDevice->m_pCamera->MoveForward(-increaseY*0.4);
    }
}

void CRtgAppFrame::OnMouseRDrag(int x, int y, int increaseX, int increaseY)
{
    if (m_pDevice->m_pCamera)
    {
        if (GetKeyState(VK_MENU)&0xFF00) // 当有按下Alt键的时候为镜头的远近
        {
            m_pDevice->m_pCamera->AddRoll(DegreeToRadian(increaseY*0.2f));
            m_pDevice->m_pCamera->AddYaw(DegreeToRadian(-increaseX*0.2f));
        }else // 否则为旋转镜头
        {
            m_pDevice->m_pCamera->MoveRight(-increaseX*1.4);
            m_pDevice->m_pCamera->MoveForward(-increaseY*1.4);
        }
    }
}

bool CRtgAppFrame::OnPreCreate3DDevice()
{
    return true;
}

bool CRtgAppFrame::OnAfterCreate3DDevice()
{
    return true;
}

bool CRtgAppFrame::OnDeviceInit()
{
    return true;
}

void CRtgAppFrame::OnDeviceClose()
{
}

bool CRtgAppFrame::OnRestoreDevice()
{
    return true;
}

bool CRtgAppFrame::OnInvalidateDevice()
{
    return true;
}

void CRtgAppFrame::OnBeginRender()
{
}

void CRtgAppFrame::OnEndRender()
{
    if (m_bEnableConsole && m_pConsole)
    {
        if (m_bShowConsole)
        {
            m_pConsole->ShowConsole(m_pDevice, m_bShowConsole);
        }
    }
}

void CRtgAppFrame::ShowConsole(bool bShow)
{
    if (m_bEnableConsole)
    {
        m_bShowConsole = bShow;
        if (m_pConsole==NULL)
        {
            m_pConsole = RT_NEW CConsoleDefault(this);
        }
        m_pConsole->ShowConsole(bShow);
    }
}

#define CONSOLE_LENGTH 1024
bool CRtgAppFrame::DoConsoleCommand(const char* szCommand)
{
    char seps[]   = " ,\t\n";
    char *token;
    char szString[CONSOLE_LENGTH];
    
    if (!m_bEnableConsole && m_pConsole==NULL)
	
    {
        m_pConsole = RT_NEW CConsoleDefault(this);
    }

    m_pConsole->AddConsoleString(szCommand);
    rt2_strncpy(szString, szCommand+2, CONSOLE_LENGTH-1);

    token = strtok( szString, seps );
    if (token==NULL) return false;
    if (stricmp(token, "stat")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "fps")==0)
            {
                s_bShowStatFPS = !s_bShowStatFPS;
                return true;
            }else if (stricmp(token, "buffer")==0)
            {
                s_bShowStatBuffer = !s_bShowStatBuffer;
                return true;
            }else if (stricmp(token, "memory")==0)
            {
                s_bShowStatMem = !s_bShowStatMem;
                return true;
            }else if (stricmp(token, "texture")==0)
            {
                s_bShowStatTexture = !s_bShowStatTexture;
                return true;
            }else if (stricmp(token, "render")==0)
            {
                s_bShowStatRender = !s_bShowStatRender;
                return true;
            }else if (stricmp(token, "collision")==0)
            {
                s_bShowStatCollision = !s_bShowStatCollision;
                return true;
            }else if (stricmp(token, "allon")==0)
            {
                s_bShowStatFPS       = true;
                s_bShowStatBuffer    = true;
                s_bShowStatMem       = true;
                s_bShowStatTexture   = true;
                s_bShowStatRender    = true;
                s_bShowStatCollision = true;
            }else if (stricmp(token, "alloff")==0)
            {
                s_bShowStatFPS       = false;
                s_bShowStatBuffer    = false;
                s_bShowStatMem       = false;
                s_bShowStatTexture   = false;
                s_bShowStatRender    = false;
                s_bShowStatCollision = false;
            }
        }
    }else if (stricmp(token, "help")==0)
    {
        m_pConsole->AddConsoleString("   stat [fps/buffer/memory/texture/render/collision/allon/alloff/dump]");
        return true;
    }else if (stricmp(token, "clear")==0)
    {
        m_pConsole->ClearConsole();
        return true;
    }else if (stricmp(token, "camera")==0)
    {
        RtgMatrix16* pM16 = &(m_pDevice->m_pCamera->m_matView);
        char szStr[100];
        rt2_sprintf(szStr, "%4.2f, %4.2f, %4.2f, %4.2f", pM16->_00, pM16->_01, pM16->_02); m_pConsole->AddConsoleString(szStr);
        rt2_sprintf(szStr, "%4.2f, %4.2f, %4.2f, %4.2f", pM16->_10, pM16->_11, pM16->_12); m_pConsole->AddConsoleString(szStr);
        rt2_sprintf(szStr, "%4.2f, %4.2f, %4.2f, %4.2f", pM16->_20, pM16->_21, pM16->_22); m_pConsole->AddConsoleString(szStr);
        rt2_sprintf(szStr, "%4.2f, %4.2f, %4.2f, %4.2f", pM16->_30, pM16->_31, pM16->_32); m_pConsole->AddConsoleString(szStr);
        return true;
    }/*else if (stricmp(token, "effect")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "post")==0)
            {
                m_pDevice->UsePostProcessEffect(!m_pDevice->IsUsePostProcessEffect());
                return true;
            }else if (stricmp(token, "object")==0)
            {
                m_pDevice->UsePostProcessObjectHighLight(!m_pDevice->IsUsePostProcessObjectHighLight());
                return true;
            }
        }
        m_pConsole->AddConsoleString("   effect [post/object]");
        return true;
    }*/else if (stricmp(token, "dump")==0)
    {
        token = strtok( NULL, seps );
        if (token)
        {
            if (stricmp(token, "texture")==0)
            {
                RtGetRender()->GetTextMgr()->DumpTextInfo(NULL);
                //m_pDevice->m_Texture.DumpUsedTexture(NULL);
                return true;
            }
        }
        m_pConsole->AddConsoleString("   dump [texture]");
        return true;
    }

    bool bResult = OnConsoleCommand(szCommand+2);
    if (!bResult)
    {
        m_pConsole->AddConsoleString("  Unknow command");
    }
    return bResult;
}

bool CRtgAppFrame::OnConsoleCommand(const char* szCommand)
{
    return false;
}


} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
