#include "studio.h"
#include "windows.h"
#include <commdlg.h>

UiForm_MainMenu::UiForm_MainMenu()
{/*LOG("");*/ 
    m_pFrm_This		= NULL;
    m_pBtn_Load		= NULL; 
    m_pBtn_Save		= NULL;
    m_pBtn_SaveAs	= NULL;
	m_pBtn_CtrlUIShow = NULL;
	m_pBtn_Config	= NULL;
	m_pBtn_Exit		= NULL;
	m_pBtn_LoadAndAdd = NULL;
}

UiForm_MainMenu::~UiForm_MainMenu()
{/*LOG("");*/
    DROP_RTUI_OBJECT(m_pFrm_This);
    DROP_RTUI_OBJECT(m_pBtn_Load);
    DROP_RTUI_OBJECT(m_pBtn_LoadAndAdd);
    DROP_RTUI_OBJECT(m_pBtn_Save);
    DROP_RTUI_OBJECT(m_pBtn_SaveAs);
	DROP_RTUI_OBJECT(m_pBtn_CtrlUIShow);
	DROP_RTUI_OBJECT(m_pBtn_Config);
	DROP_RTUI_OBJECT(m_pBtn_Exit);
}

bool UiForm_MainMenu::Init()
{/*LOG("");*/
    CHECK(g_workspace.FindWidget("layStudio.frmMenu",				(RtwWidget**)&m_pFrm_This));
    CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnLoad",		(RtwWidget**)&m_pBtn_Load));
    CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnLoadAndAdd", (RtwWidget**)&m_pBtn_LoadAndAdd));
    CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnSave",		(RtwWidget**)&m_pBtn_Save));
    CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnSaveAs",		(RtwWidget**)&m_pBtn_SaveAs));
	CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnCtrlUIShow", (RtwWidget**)&m_pBtn_CtrlUIShow));
	CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnConfig",		(RtwWidget**)&m_pBtn_Config));
	CHECK(g_workspace.FindWidget("layStudio.frmMenu.btnQuit",		(RtwWidget**)&m_pBtn_Exit));

    m_pBtn_Load->EvLClick		+= RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_Load);
    m_pBtn_LoadAndAdd->EvLClick += RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_LoadAndAdd);
    m_pBtn_Save->EvLClick		+= RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_Save);
    m_pBtn_SaveAs->EvLClick		+= RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_SaveAs);
	m_pBtn_CtrlUIShow->EvLClick += RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_CtrlUIShow);
	m_pBtn_Config->EvLClick		+= RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_Config);
	m_pBtn_Exit->EvLClick		+= RTW_CALLBACK(this, UiForm_MainMenu, OnLClick_Exit);

// 	m_pFrm_This->SetWidgetEffect(esTrans|esZoom, esZoom);

	m_pBtn_CtrlUIShow->Disable();   // Add by Peter 2010.12.31
	
	return true;
}

void UiForm_MainMenu::Refresh()
{/*LOG("");*/
}

void UiForm_MainMenu::Show()
{/*LOG("");*/
//      m_pFrm_This->ShowModal();
	m_pFrm_This->ShowModal();
}

void UiForm_MainMenu::Hide()
{/*LOG("");*/
    m_pFrm_This->Hide();
}

void UiForm_MainMenu::Run(DWORD dwDelta)
{/*LOG("");*/
}

bool UiForm_MainMenu::IsVisible()
{/*LOG("");*/
    return m_pFrm_This->getVisible();
}

void UiForm_MainMenu::OnLClick_Load(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/	
   //m_pFrm_This->Hide();         // Del by Peter 2010.12.31
	m_pBtn_CtrlUIShow->Enable();  // Add by Peter 2010.12.31
 
    char filename[_MAX_PATH];
    filename[0] = 0;
    OPENFILENAME ofn;

    memset(&ofn, 0, sizeof(ofn));

	std::string strCurrentPathWithSlash = rtCurrentDir();
	strCurrentPathWithSlash = strCurrentPathWithSlash + "\\";

	static char szInitDir[MAX_PATH];
	memset(szInitDir,	0, MAX_PATH);
	strcpy(szInitDir,	rtCurrentDir());
    ofn.lStructSize		= sizeof(ofn);
	ofn.lpstrInitialDir = szInitDir;
    ofn.lpstrFile		= filename;
    ofn.nMaxFile		 = _MAX_PATH;

    if (GetOpenFileName(&ofn))
    {
		RtCoreFile().SetDefaultDirectory(strCurrentPathWithSlash.c_str());
		if (!g_pUiMain->m_pLay_Studio->m_UiDocument.IsXmlFile(filename))
		{
			RtwMailBox *msg = g_workspace.getDefaultMessageBox();
			msg->Show("您选择的不是合法xml文件,请重新添加", "错误", true, false);
			return;
		}

		g_pUiMain->m_pLay_Studio->m_UiDocument.RemoveAllElements();
        if (!g_workspace.getXmlLoader()->LoadFromFile(filename))
        {
            RtwMailBox* pMB = g_workspace.getDefaultMessageBox();
            pMB->Show("打开文件失败", "错误", true, false);
            return;
        }
        else if (!g_pUiMain->m_pLay_Studio->m_UiDocument.LoadFromFile(filename))
        {
            RtwMailBox* pMB = g_workspace.getDefaultMessageBox();
            pMB->Show("解析文件失败", "错误", true, false);
            return;
        }
		g_workspace.KeyChar(0);
        g_pUiMain->m_pLay_Studio->m_FileName = filename; 
        g_pUiMain->m_pLay_Studio->Refresh();
    }
}

void UiForm_MainMenu::OnLClick_LoadAndAdd(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    m_pFrm_This->Hide();

	std::string strCurrentPathWithSlash = rtCurrentDir();
	strCurrentPathWithSlash = strCurrentPathWithSlash + "\\";

    char filename[_MAX_PATH];
    filename[0] = 0;
    OPENFILENAME ofn;

    memset(&ofn, 0, sizeof(ofn));

    ofn.lStructSize	= sizeof(ofn);
    ofn.lpstrFile	= filename;
    ofn.nMaxFile	= _MAX_PATH;

    if (GetOpenFileName(&ofn))
    {
		RtCoreFile().SetDefaultDirectory(strCurrentPathWithSlash.c_str());
		if (!g_pUiMain->m_pLay_Studio->m_UiDocument.IsXmlFile(filename))
		{
			if (!g_pUiMain->m_pLay_Studio->m_UiDocument.IsXmlFile(filename))
			{
				RtwMailBox *msg = g_workspace.getDefaultMessageBox();
				msg->Show("您选择的不是合法xml文件,请重新添加", "错误", true, false);
				return;
			}
			return;
		}

        if (!g_workspace.getXmlLoader()->LoadFromFile(filename))
        {
            RtwMailBox* pMB = g_workspace.getDefaultMessageBox();
            pMB->Show("打开文件失败", "错误", true, false);
            return;
        }
        if (!g_pUiMain->m_pLay_Studio->m_UiDocument.AddFromFile(filename))
        {
            RtwMailBox* pMB = g_workspace.getDefaultMessageBox();
            pMB->Show("解析文件失败", "错误", true, false);
            return;
        }
        g_pUiMain->m_pLay_Studio->m_FileName = ""; 
        g_pUiMain->m_pLay_Studio->Refresh();
    }
}

void UiForm_MainMenu::OnLClick_Save(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    m_pFrm_This->Hide();

    if (g_pUiMain->m_pLay_Studio->m_FileName.empty())
    {
        RtwChatInputBox* pInputBox = g_workspace.getDefaultInputBox();
		pInputBox->SetInputTitle("保存文件");
        pInputBox->Show("输入保存文件的名字:", "保存");
		RtwWidget* pInputEdit = NULL; 
		if (g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_Edit], &pInputEdit))
			pInputEdit->SetFocus();
        pInputBox->EvOk += RTW_CALLBACK(this, UiForm_MainMenu, OnSave);
    }
    else 
    {
        if (g_pUiMain->m_pLay_Studio->m_UiDocument.SaveToFile(g_pUiMain->m_pLay_Studio->m_FileName))
        {
            RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
            pMsgBox->Show("保存成功", "保存", true, false);
        }
        else
        {
            RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
            pMsgBox->Show("保存失败,请确认文件名合法并且可写", "保存", true, false);
        }
    }
}

void UiForm_MainMenu::OnLClick_SaveAs(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    m_pFrm_This->Hide();

    RtwChatInputBox* pInputBox = g_workspace.getDefaultInputBox();
	pInputBox->SetInputTitle("保存文件");
    pInputBox->Show("输入保存文件的名字:", "保存");
	RtwWidget* pInputEdit = NULL; 
	if (g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_Edit], &pInputEdit))
		pInputEdit->SetFocus();
    pInputBox->EvOk += RTW_CALLBACK(this, UiForm_MainMenu, OnSave);
}

void UiForm_MainMenu::OnLClick_CtrlUIShow(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	bool bUIShow = UIIsVisible();
    OnCtrlUIShow(!bUIShow);
}

bool UiForm_MainMenu::UIIsVisible()
{
	RtwTree::Item* pTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getFirstItem();
	while (pTreeItem)
	{	 
		RtwWidget* pWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItem);
		if (pWidget)
		{	 
			RtwWidget* pTmp = pWidget;
			while(pTmp)
			{
				if(pTmp->IsVisible())
				{
					return true;
				}
				pTmp = pTmp->getParent();
			}
		}
		pTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getNextItem(pTreeItem);
	}

	return false;  
}

void UiForm_MainMenu::OnCtrlUIShow(bool bShouUI/* = true*/)
{
	RtwTree::Item* pTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getFirstItem();
	while (pTreeItem)
	{	 
		RtwWidget* pWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItem);
		if (pWidget)
		{	 
			RtwWidget* pTmp = pWidget;
			if (bShouUI)
			{
			    pWidget->ModifyFlag(wfVisible, 0);	
			}
			else
			{
                pWidget->ModifyFlag(0, wfVisible);
			}
		}
		pTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getNextItem(pTreeItem);
	}
}

void UiForm_MainMenu::OnSave(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    m_pFrm_This->Hide();

    std::string filename = g_workspace.getDefaultInputBox()->getInputString();
    if (filename.empty())
    {
        RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
        pMsgBox->Show("不合法的文件名", "错误", true, false);
        return;
    }
    filename.insert(0, "ui/");

    if (g_pUiMain->m_pLay_Studio->m_UiDocument.SaveToFile(filename))
    {
        RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
        pMsgBox->Show("保存成功", "保存", true, false);
    }
    else
    {
        RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
        pMsgBox->Show("保存失败,请确认文件名合法并且可写", "保存", true, false);
    }
}

void UiForm_MainMenu::OnLClick_Config(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
	pMsgBox->Show("配置", "错还是对？？？？？？？", true, false);
	return;
}

void UiForm_MainMenu::OnLClick_Exit(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{ 
	PostQuitMessage(WM_QUIT);
	return;
}

