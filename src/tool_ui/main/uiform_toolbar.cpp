#include "studio.h"



UiForm_ToolBar::UiForm_ToolBar()
{/*LOG("");*/
    m_pFrm_This			= NULL;
    m_pBtn_Cut			= NULL;
    m_pBtn_Copy			= NULL;
    m_pBtn_Paste		= NULL;
    m_pBtn_MoveUp		= NULL;
    m_pBtn_MoveDown		= NULL;
	m_pBtn_AlignTop		= NULL;
	m_pBtn_AlignBottom	= NULL;
	m_pBtn_AlignLeft	= NULL;
	m_pBtn_AlignRight	= NULL;
	m_pBtn_SameAll		= NULL;
	m_pBtn_SameWidth	= NULL;
	m_pBtn_SameHeight	= NULL;
}

UiForm_ToolBar::~UiForm_ToolBar()
{/*LOG("");*/
    DROP_RTUI_OBJECT(m_pFrm_This);
    DROP_RTUI_OBJECT(m_pBtn_ShowWidgetsLine);
    DROP_RTUI_OBJECT(m_pBtn_Cut);
    DROP_RTUI_OBJECT(m_pBtn_Copy);
    DROP_RTUI_OBJECT(m_pBtn_Paste);
    DROP_RTUI_OBJECT(m_pBtn_MoveUp);
	DROP_RTUI_OBJECT(m_pBtn_MoveDown);
	DROP_RTUI_OBJECT(m_pBtn_AlignTop);
	DROP_RTUI_OBJECT(m_pBtn_AlignBottom);
	DROP_RTUI_OBJECT(m_pBtn_AlignLeft);
	DROP_RTUI_OBJECT(m_pBtn_AlignRight);
	DROP_RTUI_OBJECT(m_pBtn_SameAll);
	DROP_RTUI_OBJECT(m_pBtn_SameWidth);
	DROP_RTUI_OBJECT(m_pBtn_SameHeight);
}

bool UiForm_ToolBar::Init()
{/*LOG("");*/
	// Modified by Wayne Wong 2010-11-17
	//CHECK(g_workspace.FindWidget("layStudio.frmDebugInfo",			(RtwWidget**)&m_pFrm_This));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar",			(RtwWidget**)&m_pFrm_This));

    CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnCut",		(RtwWidget**)&m_pBtn_Cut));
    CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnCopy",	(RtwWidget**)&m_pBtn_Copy));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnPaste",	(RtwWidget**)&m_pBtn_Paste));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnTop",		(RtwWidget**)&m_pBtn_AlignTop));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnLeft",	(RtwWidget**)&m_pBtn_AlignLeft));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnBottom",	(RtwWidget**)&m_pBtn_AlignBottom));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnRight",	(RtwWidget**)&m_pBtn_AlignRight));
    CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnMoveUp",	(RtwWidget**)&m_pBtn_MoveUp));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnMoveDown",(RtwWidget**)&m_pBtn_MoveDown));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnWidthSame", (RtwWidget**)&m_pBtn_SameWidth));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnHeightSame", (RtwWidget**)&m_pBtn_SameHeight));
	CHECK(g_workspace.FindWidget("layStudio.frmToolbar.btnAllSame", (RtwWidget**)&m_pBtn_SameAll));

    m_pBtn_Cut->EvLClick			+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Cut);
    m_pBtn_Copy->EvLClick			+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Copy);
	m_pBtn_Paste->EvLClick			+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Paste);
	m_pBtn_AlignTop->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Top);
	m_pBtn_AlignBottom->EvLClick	+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Bottom);
	m_pBtn_AlignLeft->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Left);
	m_pBtn_AlignRight->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_Right);
    m_pBtn_MoveUp->EvLClick			+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_MoveUp);
	m_pBtn_MoveDown->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_MoveDown);
	m_pBtn_SameAll->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_AllSame);
	m_pBtn_SameHeight->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_SameHeight);
	m_pBtn_SameWidth->EvLClick		+= RTW_CALLBACK(this,	UiForm_ToolBar, OnLClick_SameWidth);
// 	m_HtmlView->LoadFromFile("E:\\sxnew\\bin\\task\\npc\\zhoufb04_n001.html");

    return true;
}

void UiForm_ToolBar::Refresh()
{/*LOG("");*/
    
}

void UiForm_ToolBar::Show()
{/*LOG("");*/
    m_pFrm_This->Show();
}

void UiForm_ToolBar::Hide()
{/*LOG("");*/
    m_pFrm_This->Hide();
}

void UiForm_ToolBar::Run(DWORD dwDelta)
{/*LOG("");*/
}

bool UiForm_ToolBar::IsVisible()
{/*LOG("");*/
    return m_pFrm_This->getVisible();
}

void UiForm_ToolBar::OnLClick_Cut(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    RtwTree::Item *pSelectTree = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    if (pSelectTree)
    {
        g_pUiMain->m_pLay_Studio->m_UiDocument.Cut(pSelectTree);
    }
}

void UiForm_ToolBar::OnLClick_Copy(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    RtwTree::Item *pSelectTree = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    if (pSelectTree)
    {
        g_pUiMain->m_pLay_Studio->m_UiDocument.Copy(pSelectTree);
    }
}

void UiForm_ToolBar::OnLClick_Paste(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    RtwTree::Item *pSelectTree = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    if (pSelectTree)
    {
        g_pUiMain->m_pLay_Studio->m_UiDocument.Paste(pSelectTree);
    }
}

void UiForm_ToolBar::OnLClick_Left(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	AlignWidget(LEFT);
}

void UiForm_ToolBar::OnLClick_Top(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	AlignWidget(TOP);
}

void UiForm_ToolBar::OnLClick_Bottom(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	AlignWidget(BOTTOM);
}

void UiForm_ToolBar::OnLClick_Right(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	AlignWidget(RIGHT);
}

void UiForm_ToolBar::OnLClick_MoveUp(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	RtwTree::Item *pSelectTree = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
	if (pSelectTree)
	{
		g_pUiMain->m_pLay_Studio->m_UiDocument.MoveUp(pSelectTree);
	}
}

void UiForm_ToolBar::OnLClick_MoveDown(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	RtwTree::Item *pSelectTree = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
	if (pSelectTree)
	{
		g_pUiMain->m_pLay_Studio->m_UiDocument.MoveDown(pSelectTree);
	}
}

void UiForm_ToolBar::OnLClick_AllSame(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	OnLClick_SameHeight(pWidget, pEvent);
	OnLClick_SameWidth(pWidget, pEvent);
}

void UiForm_ToolBar::OnLClick_SameWidth(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	AlignWidget(SAMEWIDTH);
}

void UiForm_ToolBar::OnLClick_SameHeight(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	AlignWidget(SAMEHEIGHT);
}

void UiForm_ToolBar::AlignWidget(eAlignType eType)
{
	list<RtwWidget*> pWidgetList = getMainFrame()->m_pStudioProcess->m_SelectedWidgetList;
	RtwWidget * pOperatorWidget =  getMainFrame()->m_pStudioProcess->m_pOperateWidget;
	list<RtwWidget*>::iterator lri = pWidgetList.begin();

	if (pWidgetList.empty())
	{
		return;
	}

	RtwMailBox *msg = g_workspace.getDefaultMessageBox();
	int top, left, bottom, right;
	int samewidth, sameheight;
	RtwRect rc, tmpRc;
	rc = pOperatorWidget->getClientRect();
	samewidth = rc.getWidth();
	sameheight = rc.getHeight();

	int width, height;
	switch (eType)
	{
	case LEFT:
		left = rc.left;	
		while (lri != pWidgetList.end())
		{
			tmpRc = (*lri)->getClientRect();
			width = tmpRc.getWidth();
			tmpRc.left = left;
			tmpRc.right = left + width;
			(*lri)->Move(tmpRc);

			lri++;
		}
		break;
	case RIGHT:
		right = rc.right;
		while (lri != pWidgetList.end())
		{
			tmpRc = (*lri)->getClientRect();
			width = tmpRc.getWidth();
			tmpRc.right = right;
			tmpRc.left = right - width;
			(*lri)->Move(tmpRc);

			lri++;
		}
		break;
	case TOP:
		top = rc.top;
		while (lri != pWidgetList.end())
		{
			tmpRc = (*lri)->getClientRect();
			height = tmpRc.getHeight();
			tmpRc.top = top;
			tmpRc.bottom = top + height;
			(*lri)->Move(tmpRc);

			lri++;
		}
		break;
	case BOTTOM:
		bottom = rc.bottom;
		while (lri != pWidgetList.end())
		{
			tmpRc = (*lri)->getClientRect();
			height = tmpRc.getHeight();
			tmpRc.bottom = bottom;
			tmpRc.top = bottom - height;
			(*lri)->Move(tmpRc);

			lri++;
		}
		break;
	case SAMEHEIGHT:
		while (lri != pWidgetList.end())
		{
			tmpRc = (*lri)->getClientRect();
			tmpRc.bottom = tmpRc.top + sameheight;
			(*lri)->MoveResize(tmpRc);

			lri++;
		}
		break;
	case SAMEWIDTH:
		while (lri != pWidgetList.end())
		{
			tmpRc = (*lri)->getClientRect();
			tmpRc.right = tmpRc.left + samewidth;
			(*lri)->MoveResize(tmpRc);

			lri++;
		}
		break;
	default:
		return;
	}
}


