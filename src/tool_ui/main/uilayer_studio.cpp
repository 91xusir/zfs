#include "studio.h"

UiLayerStudio::UiLayerStudio()
{
    m_pLay_This = NULL;
    m_pLbl_OperateState = NULL;
    m_pLbl_MouseX = NULL;
    m_pLbl_MouseY = NULL;
    m_pLbl_Fps = NULL;
}

UiLayerStudio::~UiLayerStudio()
{
    DROP_RTUI_OBJECT(m_pLay_This);
    DROP_RTUI_OBJECT(m_pLbl_OperateState);
    DROP_RTUI_OBJECT(m_pLbl_MouseX);
    DROP_RTUI_OBJECT(m_pLbl_MouseY);
    DROP_RTUI_OBJECT(m_pLbl_Fps);
}

bool UiLayerStudio::Init()
{
	// Modified by Wayne Wong 2010-11-17
	//CHECK(g_workspace.FindWidget("layStudio", (RtwWidget**)&m_pLay_This));
	//CHECK(g_workspace.FindWidget("layStudio.frmDebugInfo.labFactoryNo", (RtwWidget**)&m_pLbl_Fps));
	//CHECK(g_workspace.FindWidget("layStudio.frmStatebar.State", (RtwWidget**)&m_pLbl_OperateState));
	//CHECK(g_workspace.FindWidget("layStudio.frmDebugInfo.MousePosX", (RtwWidget**)&m_pLbl_MouseX));
	//CHECK(g_workspace.FindWidget("layStudio.frmDebugInfo.MousePosY", (RtwWidget**)&m_pLbl_MouseY));

	CHECK(g_workspace.FindWidget("layStudio", (RtwWidget**)&m_pLay_This));
	CHECK(g_workspace.FindWidget("layStudio.frmStatebar.FPS", (RtwWidget**)&m_pLbl_Fps));
	CHECK(g_workspace.FindWidget("layStudio.frmStatebar.State", (RtwWidget**)&m_pLbl_OperateState));
	CHECK(g_workspace.FindWidget("layStudio.frmStatebar.MousePosX", (RtwWidget**)&m_pLbl_MouseX));
	CHECK(g_workspace.FindWidget("layStudio.frmStatebar.MousePosY", (RtwWidget**)&m_pLbl_MouseY));

    m_pLay_This->EvMouseMove += RTW_CALLBACK(this, UiLayerStudio, OnMouseMove_This);
    m_pLay_This->EvKeyChar += RTW_CALLBACK(this, UiLayerStudio, OnKeyChar_This);

    return true;
}

void UiLayerStudio::Refresh()
{
    g_pUiMain->m_pFrm_MainMenu->Refresh();
    g_pUiMain->m_pFrm_WidgetTree->Refresh();
    g_pUiMain->m_pFrm_Attributes->Refresh();
}

void UiLayerStudio::Show()
{
    m_pLay_This->Show();
}

void UiLayerStudio::Hide()
{
    m_pLay_This->Hide();
}

void UiLayerStudio::Run(DWORD dwDelta)
{
//     static DWORD dwFrameCount = 0;
//     static DWORD dwLastTick = rtMilliseconds();
// 
     char cTmp2048[2048];
// 
//     DWORD dwNowTick = rtMilliseconds();
//     dwFrameCount ++;
// 
//     if (dwNowTick - dwLastTick >= 1000)
//     {
        float nFps = 1000.0f / dwDelta;
        if (m_pLbl_Fps)
        {
            sprintf(cTmp2048, "FPS:%.2f", nFps);
            m_pLbl_Fps->SetText(cTmp2048);
        }
// 
//         dwFrameCount = 0;
//         dwLastTick = dwNowTick;
//     }
}

bool UiLayerStudio::IsVisible()
{
    return m_pLay_This->getVisible();
}

void UiLayerStudio::OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
    static char cTmp256[256];
    if (m_pLbl_MouseX)
    {
        sprintf(cTmp256, "MouseX: %d", pEvent->mouse.x);
        m_pLbl_MouseX->SetText(cTmp256);
    }
    if (m_pLbl_MouseY)
    {
        sprintf(cTmp256, "MouseY: %d", pEvent->mouse.y);
        m_pLbl_MouseY->SetText(cTmp256);
    }
}

void UiLayerStudio::OnKeyChar_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
    char character = pEvent->key.code;

    if (character == vkEscape)
    {
        if (g_pUiMain->m_pFrm_MainMenu->IsVisible())
        {
            g_pUiMain->m_pFrm_MainMenu->Hide();
        }
        else 
        {
            g_pUiMain->m_pFrm_MainMenu->Show();
        }
    }
}

