#include "rtg_graph_inter.h"
#include "rtg_console.h"

namespace rt_graph {

/*----------------------------------------------------------------------------
-   CConsoleDefault
----------------------------------------------------------------------------*/

CConsoleDefault::CConsoleDefault(CRtgAppFrame* pAppFrame) : CRtgAppFrame::CConsole(pAppFrame)
{
	CM_MEMPROTECTOR2(m_szConsoleString, CONSOLE_LINE, CONSOLE_LENGTH)
	CM_MEMPROTECTOR(m_szConsoleInput, CONSOLE_LENGTH)

	rt2_strcpy(m_szConsoleInput, "> ");
    m_iConsoleLineCur = 0;
    m_iConsoleFadeCur = 0;
    m_iConsoleFadeFrame = 5;
}

CConsoleDefault::CConsoleDefault(const CConsoleDefault &object) : CRtgAppFrame::CConsole(object.m_pAppFrame)
{
	memcpy(this, &object, sizeof(CConsoleDefault));
	CM_MEMPROTECTOR2(m_szConsoleString, CONSOLE_LINE, CONSOLE_LENGTH)
	CM_MEMPROTECTOR(m_szConsoleInput, CONSOLE_LENGTH)
}

CConsoleDefault::~CConsoleDefault()
{
	CM_MEMUNPROTECTOR2(m_szConsoleString, CONSOLE_LINE, CONSOLE_LENGTH)
	CM_MEMUNPROTECTOR(m_szConsoleInput)
}
void CConsoleDefault::Init()
{
    m_ConsoleShader.bEnableBlendFunc = true;
    m_ConsoleShader.iNumStage = 0;
    //m_ConsoleShader.iNumStage = 1;
    //m_ConsoleShader.Shaders[0].dwTextureID = g_pDevice->m_Texture.CreateTexture("console_bg", true, 1);
    //m_ConsoleShader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    //m_ConsoleShader.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
    //if (m_ConsoleShader.Shaders[0].dwTextureID==0)
    //{
    //}
}
void CConsoleDefault::Close(RtgDevice* pDevice)
{
    pDevice->DeleteShader(&m_ConsoleShader);
}

void CConsoleDefault::ShowConsole(bool bShow)
{
    if (bShow)
    {
        if (m_iConsoleFadeCur==0)
        {
            m_iConsoleFadeCur ++;
        }
    }
}

void CConsoleDefault::ShowConsole(RtgDevice* _pDevice, bool bShowConsole)
{
#if DO_CONSOLE
    if (m_iConsoleFadeCur==0)
        return;
    RtgDeviceD3D9* pDevice = RtGetRender();
    RECT rt;
    DWORD dwColor;
    DWORD dwColorText = 0xFF00FF00;
    float fRate;

    const int iEdgeLeft     = 10;
    const int iEdgeRight    = 10;
    const int iEdgeBottom   = 10;
    const int iEdgeVertical = 5;
    const int iLineHeight   = 12;
    const int iInputLineHeight = 15;

    if (bShowConsole)
    {
        m_iConsoleFadeCur ++;
    }else
    {
        m_iConsoleFadeCur --;
    }
    m_iConsoleFadeCur = Clamp(m_iConsoleFadeCur, 0, m_iConsoleFadeFrame);
    fRate = (float)m_iConsoleFadeCur/(float)m_iConsoleFadeFrame;
    dwColor = 0x00000000 | (((DWORD)(fRate*0xB0))<<24);

    rt.left   = iEdgeLeft;
    rt.bottom = pDevice->m_iWndHeight - iEdgeBottom;
    rt.right  = pDevice->m_iWndWidth - iEdgeRight;
    rt.top    = rt.bottom - (CONSOLE_LINE*iLineHeight + iEdgeVertical*2 + iInputLineHeight);

    BOOL bBakFog = pDevice->GetFogEnable();
    pDevice->SetFogEnable(FALSE);
    pDevice->SetRenderState(RTGRS_LIGHTING, FALSE);
    pDevice->SetShader(&m_ConsoleShader);
    if (m_ConsoleShader.Shaders[0].GetTexture())
    {
        //_TexItem* pTexItem = pDevice->m_Texture.GetTextureItem(m_ConsoleShader.Shaders[0].GetTexture());
        RtgTextItem* texItem = m_ConsoleShader.Shaders[0].GetTexture();
        float tl = 0;
        float tr = (float)(rt.right-rt.left) / texItem->width;
        float tb = 0;
        float tt = (float)(rt.bottom-rt.top) / texItem->height;

        RtgNVertexVCT p[4];
        p[0] = RtgNVertexVCT( rt.left, rt.bottom-(rt.bottom-rt.top)*fRate, dwColor, tl, tt);
        p[1] = RtgNVertexVCT( rt.left, rt.bottom, dwColor, tl, tb);
        p[2] = RtgNVertexVCT( rt.left+(rt.right-rt.left)*fRate, rt.bottom, dwColor, tr, tb);
        p[3] = RtgNVertexVCT( rt.left+(rt.right-rt.left)*fRate, rt.bottom-(rt.bottom-rt.top)*fRate, dwColor, tr, tt);

        pDevice->SetVertexFormat(RTG_TRANSFORMED_VCT);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVCT), 2);
    }else
    {
        RtgNVertexVC p[4];
        p[0] = RtgNVertexVC( rt.left, rt.bottom-(rt.bottom-rt.top)*fRate, dwColor);
        p[1] = RtgNVertexVC( rt.left, rt.bottom, dwColor);
        p[2] = RtgNVertexVC( rt.left+(rt.right-rt.left)*fRate, rt.bottom, dwColor);
        p[3] = RtgNVertexVC( rt.left+(rt.right-rt.left)*fRate, rt.bottom-(rt.bottom-rt.top)*fRate, dwColor);

        pDevice->SetVertexFormat(RTG_TRANSFORMED_VC);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVC), 2);

    }
    pDevice->RestoreShader(&m_ConsoleShader);
    pDevice->SetFogEnable(bBakFog);
    pDevice->SetRenderState(RTGRS_LIGHTING, TRUE);

    if (m_iConsoleFadeCur==m_iConsoleFadeFrame)
    {
        // Draw String
        int i, iHeight = rt.bottom - iEdgeVertical - iLineHeight;
        char szShowInput[CONSOLE_LENGTH+1];
        static bool sbShowCursor = true;
        static RtTime lastTime(true);
        RtTime curTime(true);

        float fTimeDiff = curTime-lastTime;
        if (fTimeDiff > 0.3f)
        {
            lastTime = curTime;
            sbShowCursor= !sbShowCursor;
        }

        rt2_strcpy(szShowInput, m_szConsoleInput);
        if (sbShowCursor)
        {
            rt2_strcat(szShowInput, "_");
        }
        pDevice->DrawString( iEdgeLeft+2,  iHeight, dwColorText, szShowInput );
        iHeight = rt.bottom - iEdgeVertical - iInputLineHeight;
        for (i=m_iConsoleLineCur-1; i>=0; i--)
        {
            iHeight -= iLineHeight;
            pDevice->DrawString( iEdgeLeft+2,  iHeight, dwColorText, m_szConsoleString[i] );
        }
    }
#endif
}
void CConsoleDefault::OnKeyChar(int iChar, bool bAltDown)
{
    if (iChar==13)
    {
        m_pAppFrame->DoConsoleCommand(m_szConsoleInput);
        rt2_strcpy(m_szConsoleInput, "> ");
    }else if (iChar==8)
    {
        if (strlen(m_szConsoleInput)>2)
        {
            m_szConsoleInput[strlen(m_szConsoleInput)-1] = 0;
        }
    }else
    {
        if (strlen(m_szConsoleInput)<(CONSOLE_LENGTH-1))
        {
            char szChar[2] = "c";
            szChar[0] = (char)iChar;
            rt2_strcat(m_szConsoleInput, szChar);
        }
    }
}

void CConsoleDefault::AddConsoleString(const char* szString)
{
    if (m_iConsoleLineCur>=CONSOLE_LINE)
    {
        m_iConsoleLineCur = CONSOLE_LINE-1;
        for (int i=0; i<m_iConsoleLineCur; i++)
        {
            rt2_strcpy(m_szConsoleString[i], m_szConsoleString[i+1]);
        }
    }
    m_szConsoleString[m_iConsoleLineCur][0] = 0;
    rt2_strncpy(m_szConsoleString[m_iConsoleLineCur], szString, CONSOLE_LENGTH-1);
    m_iConsoleLineCur ++;
}

void CConsoleDefault::ClearConsole()
{
    for (int i=0; i<m_iConsoleLineCur; i++)
    {
        m_szConsoleString[i][0] = 0;
    }
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
