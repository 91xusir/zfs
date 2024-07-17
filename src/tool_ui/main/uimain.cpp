#include "studio.h"

#define LOAD_UI_RESOURCE_B(clause)         { if (!(clause))    { RtCoreLog().Error("Ignored ui error - [ %s ]\n", #clause); return true; } }

UiMain *g_pUiMain = NULL;

UiMain::UiMain()
{/*LOG("");*/
    m_pLay_Studio = NULL;
    m_pFrm_Attributes = NULL;
    m_pFrm_WidgetTree = NULL;
    m_pFrm_MainMenu = NULL;
    m_pFrm_Toolbar = NULL;
}

UiMain::~UiMain()
{/*LOG("");*/
    DEL_ONE(m_pLay_Studio);
    DEL_ONE(m_pFrm_Attributes);
    DEL_ONE(m_pFrm_WidgetTree);
    DEL_ONE(m_pFrm_MainMenu);
//     DEL_ONE(m_pFrm_Toolbar);
}

bool UiMain::Init()
{/*LOG("");*/
    m_pLay_Studio = new UiLayerStudio();
    LOAD_UI_RESOURCE_B(m_pLay_Studio->Init());

    m_pFrm_Attributes = new UiForm_Attributes();
    LOAD_UI_RESOURCE_B(m_pFrm_Attributes->Init());

    m_pFrm_WidgetTree = new UiForm_WidgetTree();
    LOAD_UI_RESOURCE_B(m_pFrm_WidgetTree->Init());

    m_pFrm_MainMenu = new UiForm_MainMenu();
    LOAD_UI_RESOURCE_B(m_pFrm_MainMenu->Init());

    m_pFrm_Toolbar = new UiForm_ToolBar();
    LOAD_UI_RESOURCE_B(m_pFrm_Toolbar->Init());
    // At last, Init XmlWriter
    UI_ENSURE_B(m_pLay_Studio->m_UiDocument.Init());	

    return true;
}

void UiMain::Run(DWORD dwDelta)
{///*LOG("");*/
    m_pLay_Studio->Run(dwDelta);
}
