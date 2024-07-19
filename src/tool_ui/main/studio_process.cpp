#include "studio.h"

CUiStudioProcess::CUiStudioProcess(void)
:m_bCtrl(false),m_bStretching(false),m_pOperateWidget(NULL),m_pDragWidget(NULL),m_bShowWidgetsLine(true)
{	//LOG("");
	m_SelectedWidgetList.clear();
}

CUiStudioProcess::~CUiStudioProcess(void)
{	//LOG("");
	if (m_pSound)
	{
		delete m_pSound;
	}
}

bool CUiStudioProcess::Create()
{	//LOG("");
    g_pUiMain->m_pLay_Studio->m_pLay_This->grab();
    m_ExcludeLayers.push_back(g_pUiMain->m_pLay_Studio->m_pLay_This);
	CUiLayer *pGWorkSpaceLayer = NULL;
	pGWorkSpaceLayer = g_workspace.getSystemLayer();

    if (NULL != pGWorkSpaceLayer)
    {	 
        pGWorkSpaceLayer->grab();
        m_ExcludeLayers.push_back(pGWorkSpaceLayer);
    }
	m_pSound = new CUiSoundImpl_Rt;
	g_workspace.SetSound(m_pSound);
    return true;
}

bool CUiStudioProcess::Destroy()
{	//LOG("");
    DROP_RTUI_OBJECT(m_pOperateWidget);
    DROP_RTUI_OBJECT(m_pDragWidget);

    foreach (UiLayerList, iterLayer, m_ExcludeLayers)
    {	 
        CUiLayer* pLayer = *iterLayer;
        DROP_RTUI_OBJECT(pLayer);
    }
    m_ExcludeLayers.clear();

    return true;
}

void CUiStudioProcess::Run(DWORD dwDelta)
{	//LOG("");
    g_workspace.Run(dwDelta);
}

void CUiStudioProcess::Render()
{//LOG("");
	g_workspace.getRenderDevice()->BeginRender();

	g_workspace.Render();
	if (m_bShowWidgetsLine)
    {	 
        // 给可以操作的Widgets加一个边框
        RtwTree::Item* pTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getFirstItem();
        while (pTreeItem)
        {	 
            RtwWidget* pWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItem);
            if (pWidget)
            {	 
                bool bRenderBorder = true;
                RtwWidget* pTmp = pWidget;
                while (pTmp)
                {	 
                    if (!pTmp->getVisible())
                    {	 
                        bRenderBorder = false;
                        break;
                    }
                    pTmp = pTmp->getParent();
                }
              
				// Add by Peter  2010.12.31
				if (pWidget->IsVisible())
				{
				    pWidget->Render();
				}

//				if (bRenderBorder)					
// 				   g_workspace.getRenderDevice()->DrawRectangleBorder(pWidget->getFrameRect(), 
// 				   RtwPixel(0xff886633),&pWidget->getFrameRect());
			}
            pTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getNextItem(pTreeItem);
        }

        if (m_pOperateWidget && m_pOperateWidget->getVisible())
        {	 
            RtwRect rc = m_pOperateWidget->getFrameRect();
            g_workspace.getRenderDevice()->DrawRectangleBorder(rc, RtwPixel(0xff3dffa3),&rc);
        }

		if (!m_SelectedWidgetList.empty())
		{
			m_lri = m_SelectedWidgetList.begin();
			RtwRect rc;
			while (m_lri != m_SelectedWidgetList.end())
			{
				rc = (*m_lri)->getFrameRect();
				g_workspace.getRenderDevice()->DrawRectangleBorder(rc, RtwPixel(0xff3dffa3),&rc);
				m_lri++;
			}
		}

        if (g_workspace.getFocusWidget())
        {	 
            RtwWidget* pTmp = g_workspace.getFocusWidget();

            bool bRender = true;
            while (pTmp)
            {	 
                if (!pTmp->getVisible())
                { 
                    bRender = false;
                    break;
                }
                pTmp = pTmp->getParent();
            }
            if (bRender)
            { 
                RtwRect rc = g_workspace.getFocusWidget()->getFrameRect();
            }
        }
    }

    g_workspace.getRenderDevice()->EndRender();
}

bool CUiStudioProcess::MouseDown(int button)
{	//LOG("");
    g_workspace.MouseDown(button);

    // 如果焦点是编辑器窗口,就返回
    if (g_workspace.getMouseCapture())
    {	 
        foreach (UiLayerList, iterLayer, m_ExcludeLayers)
        {	 
            CUiLayer* pLayer = *iterLayer;
            if (g_workspace.getMouseCapture()->getLayer() == pLayer)
                return true;
        }
    }

    RtwWidget* pPickWidget = NULL;
    if (g_workspace.PickWidget(&pPickWidget, NULL, 0x00000000, &m_ExcludeLayers))
    {	
		if (m_bCtrl)
		{
			if (g_workspace.getFocusWidget() != NULL)
			{
				if (find(m_SelectedWidgetList.begin(), m_SelectedWidgetList.end(), pPickWidget) != m_SelectedWidgetList.end())
				{
					m_SelectedWidgetList.remove(pPickWidget);
				}
				else
				{
					m_SelectedWidgetList.push_back(pPickWidget);
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			SetOperateWidget(pPickWidget);
			m_SelectedWidgetList.clear();
		}

        if (button == buttonLeft)
        {	 
			/*
			int posX, posY;
			ui::SPoint mousePoint = g_workspace.getMousePos();
			posX = mousePoint.x;
			posY = mousePoint.y;
			
			RtwRect widgetRc = pPickWidget->getFrameRect();
			if (widgetRc.IsInBorderline(2, posX, posY))
			{
				m_bStretching = true;
			}
			*/
            if (m_pDragWidget != pPickWidget)
            {	 
                m_pDragWidget = pPickWidget;
                m_pDragWidget->grab();
            }
        }

        DROP_RTUI_OBJECT(pPickWidget);
    }

    return true;
}

bool CUiStudioProcess::MouseUp(int button)
{	//LOG("");
    bool bRes = g_workspace.MouseUp(button);

    if (button == buttonLeft)
    { 
        if (m_pOperateWidget && m_pOperateWidget == m_pDragWidget)
        { 
            if (m_pOperateWidget->getAnchorsCount() == 0)
            { 
                RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
                if (pSelectTreeItem && g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem) == m_pOperateWidget)
                { 
					SetMoveWidgetAttrib(m_pOperateWidget, pSelectTreeItem);
                }
            }
        }

        DROP_RTUI_OBJECT(m_pDragWidget);
    }

    return bRes;
}

bool CUiStudioProcess::MouseDClick(int button)
{	//LOG("");
    return g_workspace.MouseDClick(button);
}

bool CUiStudioProcess::MouseMove(int x, int y)
{	//LOG("");
    RtwRect rcOrig;
    if (m_pOperateWidget)
    { 
        rcOrig = m_pOperateWidget->getFrameRect();
    }

    bool bRes = g_workspace.MouseMove(x, y);

    if (m_pOperateWidget && getMainFrame()->m_bMouseLDown)
    {	 
        RtwRect rcNow = m_pOperateWidget->getFrameRect();
        if (rcOrig == rcNow)
        { 
			if (
				(rcNow.IsContain(x, y))
				||  (rcNow.IsContain(x+2, y+2))
				||  (rcNow.IsContain(x-2, y-2))
				||  (rcNow.IsContain(x+2, y-2))
				||  (rcNow.IsContain(x-2, y+2))			
				||  (rcNow.IsContain(x+1, y+1))
				||  (rcNow.IsContain(x-1, y-1))
				||  (rcNow.IsContain(x+1, y-1))
				||  (rcNow.IsContain(x-1, y+1))			
				)
            if (rcNow.IsContain(x, y))
            m_pOperateWidget->Offset(g_workspace.getMouseDelta());
        }
    }

    return bRes;
}

bool CUiStudioProcess::MouseWheel(int delta, int x, int y)
{	//LOG("");
    return g_workspace.MouseWheel(delta, x, y);
}

bool CUiStudioProcess::KeyChar(int ch)
{	//LOG("");
    bool bRes = g_workspace.KeyChar(ch);

    return bRes;
}

bool CUiStudioProcess::KeyDown(int key)
{	//LOG("");
    bool bRes = g_workspace.KeyDown(key);

    // 如果焦点是编辑器窗口,就返回
    if (g_workspace.getFocusWidget())
    { 
        foreach (UiLayerList, iterLayer, m_ExcludeLayers)
        { 
            CUiLayer* pLayer = *iterLayer;
            if (g_workspace.getFocusWidget()->getLayer() == pLayer)
                return true;
        }
    }

    if (key == vkLeft)
    { 
        if (m_pOperateWidget)
        { 
            SSize offset(-1, 0);
            RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
            if (pSelectTreeItem && g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem) == m_pOperateWidget)
            { 
				SetMoveWidgetAttrib(m_pOperateWidget,pSelectTreeItem,offset);
            }
        }
        return true;
    }
    else if (key == vkRight)
    { 
        if (m_pOperateWidget)
        { 
            SSize offset(1, 0);
            RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
            if (pSelectTreeItem && g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem) == m_pOperateWidget)
            { 
				SetMoveWidgetAttrib(m_pOperateWidget,pSelectTreeItem,offset);
            }
        }
        return true;
    }
    else if (key == vkUp)
    { 
        if (m_pOperateWidget)
        { 
            SSize offset(0, -1);
            RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
            if (pSelectTreeItem && g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem) == m_pOperateWidget)
            { 
				SetMoveWidgetAttrib(m_pOperateWidget,pSelectTreeItem,offset);
            }
        }
        return true;
    }
    else if (key == vkDown)
    { 
        if (m_pOperateWidget)
        { 
            SSize offset(0, 1);
            RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
            if (pSelectTreeItem && g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem) == m_pOperateWidget)
            { 
				SetMoveWidgetAttrib(m_pOperateWidget,pSelectTreeItem,offset);
            }
        }
        return true;
    }
	else if (key == vkCtrl)
	{
		m_bCtrl = true;
	}

    return bRes;
}

bool CUiStudioProcess::KeyUp(int key)
{	//LOG("");
	if (key == vkCtrl)
	{
		m_bCtrl = false;
	}
    return g_workspace.KeyUp(key);
}

void CUiStudioProcess::SetOperateWidget(RtwWidget* pWidget)
{	//LOG("");
    DROP_RTUI_OBJECT(m_pOperateWidget);

    if (pWidget)
    { 
        m_pOperateWidget = pWidget;
        m_pOperateWidget->grab();

        if (!g_pUiMain->m_pFrm_WidgetTree->SelectItemByWidget(pWidget))
        { 
            DROP_RTUI_OBJECT(m_pOperateWidget);
        }
    }

    g_pUiMain->m_pFrm_Attributes->Refresh();
}

void CUiStudioProcess::SetMoveWidgetAttrib(RtwWidget* pWidget, RtwTree::Item* pSelectTreeItem, SSize offset)
{	//LOG("");
	RtwRect rcFrame = pWidget->getFrameRect();
	rcFrame.Offset(offset);
	if (m_pOperateWidget->getParent())
	{ 
		rcFrame.Offset(-pWidget->getParent()->getFrameRect().getLeftTop());
		if (pWidget->getParent()->getWidgetType() == wtForm && ((RtwForm*)pWidget->getParent())->getTitleBarHeight() > 0)
		{ 
			rcFrame.top -= ((RtwForm*)pWidget->getParent())->getTitleBarHeight();
			rcFrame.bottom -= ((RtwForm*)pWidget->getParent())->getTitleBarHeight();
		}
	}
	g_pUiMain->m_pLay_Studio->m_UiDocument.SetAttribute("Location", rcFrame.ToString(), pSelectTreeItem);
	if (g_pUiMain->m_pFrm_WidgetTree->GetSelectItemWidget() != m_pOperateWidget)
	{	 
		SetOperateWidget(g_pUiMain->m_pFrm_WidgetTree->GetSelectItemWidget());
	}

}