// TerrainBrushView.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"

#include "view_MapListCtrl.h"
#include "BrushMapView.h"

#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrushMapForm

IMPLEMENT_DYNCREATE(CBrushMapForm, CWnd)

CBrushMapForm::CBrushMapForm()
{
	m_nFilter = 0;
}

CBrushMapForm::~CBrushMapForm()
{
}


BEGIN_MESSAGE_MAP(CBrushMapForm, CWnd)
	//{{AFX_MSG_MAP(CBrushMapForm)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
    ON_WM_HSCROLL()
	//ON_COMMAND_RANGE(ID_BUTTON_BLUE, ID_FAMILY_HAWKS, OnFilterColor)
	//ON_UPDATE_COMMAND_UI_RANGE(ID_BUTTON_BLUE, ID_FAMILY_HAWKS, OnUpdateFilterColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrushMapForm drawing

void CBrushMapForm::OnDraw(CDC* /*pDC*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CBrushMapForm diagnostics

#ifdef _DEBUG
void CBrushMapForm::AssertValid() const
{
	CWnd::AssertValid();
}

void CBrushMapForm::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

#include "atlimage.h"

/////////////////////////////////////////////////////////////////////////////
// CBrushMapForm message handlers

#define MAPLIST_ICON_SIZE   48
void CBrushMapForm::OnInitialUpdate() 
{
    //guard;
	VERIFY(m_wndToolBar.CreateToolBar(WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));
    VERIFY(m_wndToolBar.LoadToolBar(IDR_TOOLBAR_MAP));

    m_wndToolBar.GetControls()->Remove(m_wndToolBar.GetControl(0));
    if (!m_wndSlider.Create(TBS_HORZ | WS_CHILD | WS_VISIBLE, CRect(0, 0, 180, 16), this, ID_BTN_MAP_SLIDER)) return;
    m_wndSlider.SetRangeMax(0xFF);
    CXTPControlCustom* pControlSlider = CXTPControlCustom::CreateControlCustom(&m_wndSlider);
	//CXTPControlCustom* pControlSlider = &(CXTPControlCustom());
    pControlSlider->SetBorders(3, 1, 3, 1);
    pControlSlider->SetVerticalPositionOption(xtpVerticalButton);
	CXTPControl* pControl = m_wndToolBar.GetControls()->Add(pControlSlider, ID_BTN_MAP_SLIDER);
	pControl->SetFlags(xtpFlagManualUpdate);

    // ID_BTN_EDIT_TERRAIN_SHOW
    // ID_BTN_EDIT_TERRAIN_WALK

    CRect rc;
	GetClientRect(&rc);

	/*if (!m_listCtrl.CWnd::CreateEx(WS_EX_STATICEDGE, WC_LISTVIEW, NULL,
		WS_CHILD|WS_VISIBLE|LVS_AUTOARRANGE,
		rc, this, 0x0))
	{
		TRACE0("Failed to create tree control.\n");
		return;
	}*/

    if (!m_listCtrl.Create(WS_CHILD | WS_VISIBLE | LVS_AUTOARRANGE , rc, this, 0))
    {
        TRACE0("Failed to create tree control.\n");
        return;
    }

	if (!m_imageList.Create (MAPLIST_ICON_SIZE, MAPLIST_ICON_SIZE, ILC_COLOR24 | ILC_MASK, 1, 1))
	{
		TRACE0 ("Failed to create image list.\n");
		return;
	}

	CWinApp* pWinApp = AfxGetApp();
	ASSERT_VALID(pWinApp);

    CImage imgAtl;
    CBitmap* pBitmap;
    HBITMAP hBitmap;
    RtgImage imgMapFile;
    RtString szMapFile;
    int i, iItem, iItemCnt=0, iImgIdx;
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    std::vector<int> m_IconIdx;

    bool bsdxt = RtgImage::m_bSupportDXT;
    RtgImage::m_bSupportDXT = false;

    m_listCtrl.SetImageList (&m_imageList, LVSIL_NORMAL);

    iImgIdx = 0;
    for (i=0; i<RtScene::MAX_MAP_COUNT; i++)
    {
        if (!g_pScene->m_texMaps[i].szName.IsEmpty())
        {
            szMapFile = "scene/texture/"+g_pScene->m_texMaps[i].szFile;
            
            if (!imgMapFile.Load(szMapFile))
                imgMapFile.Load("scene/texture/terrain/terrain_unknow.tga");
            
            imgMapFile.ResizeCanvas(64, 64);
            imgMapFile.Resize(MAPLIST_ICON_SIZE, MAPLIST_ICON_SIZE);

            hBitmap = (HBITMAP)imgMapFile.CreateBitmap(m_listCtrl.GetDC()->m_hDC);
            int it = m_listCtrl.InsertItem(i, g_pScene->m_texMaps[i].szName, hBitmap);
            m_listCtrl.SetItemData(it, i);

          //  m_imageList.Add(CBitmap::FromHandle(hBitmap), RGB(0, 0, 0));
         //   m_IconIdx.push_back(iImgIdx++);
        }
    }

    RtgImage::m_bSupportDXT = bsdxt;


   /* iImgIdx = 0;
    for (i=0; i<RtScene::MAX_MAP_COUNT; i++)
    {
        if (!g_pScene->m_Maps[i].szName.IsEmpty())
        {
            iItem = m_listCtrl.InsertItem(iItemCnt, g_pScene->m_Maps[i].szName, i);
            m_listCtrl.SetItemData(iItem, i);
            iItemCnt ++;
        }
    }*/

    m_wndSlider.SetRange(1, 10);
    //guard_end_ex;
}

void CBrushMapForm::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	CSize sz(0);
	if (m_wndToolBar.GetSafeHwnd())
	{
		
		 sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, 0, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
        m_wndSlider.MoveWindow(0, 0, cx, 16);
        m_wndSlider.Invalidate(FALSE);
	}
	if (m_listCtrl.GetSafeHwnd())
	{	
		m_listCtrl.MoveWindow(0, sz.cy, cx, cy - sz.cy);
	}
}

BOOL CBrushMapForm::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS| WS_CLIPCHILDREN;
	
	return CWnd::PreCreateWindow(cs);
}

BOOL CBrushMapForm::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CBrushMapForm::OnSetFocus(CWnd* /*pOldWnd*/) 
{
	if (m_listCtrl.GetSafeHwnd())
	{	
		m_listCtrl.SetFocus();
	}		
}

/*
void CBrushMapForm::OnFilterColor(UINT nID)
{
	m_listCtrl.DeleteAllItems();
	if (m_nFilter == (int)nID)
	{
		m_nFilter = 0;
		for (int i = 0; i < 11; i++) InsertItem(i);
		return;
	}
	m_nFilter = nID;
	switch (m_nFilter)
	{
		case ID_BUTTON_RED:
			InsertItem(0);
			InsertItem(3);
			InsertItem(4);
			break;
		case ID_BUTTON_BLUE:
			InsertItem(1);
			InsertItem(2);
			InsertItem(10);
			break;
		case ID_BUTTON_GREEN:
			InsertItem(5);
			InsertItem(6);
			break;
		case ID_FAMILY_HAWKS:
			InsertItem(7);
			InsertItem(8);
			break;
	}
}

void CBrushMapForm::OnUpdateFilterColor(CCmdUI* pCmd)
{
	pCmd->SetCheck(pCmd->m_nID == (UINT)m_nFilter);

}
*/
BOOL CBrushMapForm::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    //guard;
    LPNMHDR pNmhder = (LPNMHDR)lParam;
    if (pNmhder->code==NM_CLICK)
    {
        POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();

        if (pos)
        {
            int nItem = m_listCtrl.GetNextSelectedItem(pos);
            int iIdx = m_listCtrl.GetItemData(nItem);

            CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
            pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_TILE);
            pFrame->m_BrushTool.SetBrushData(iIdx);
        }



        //char szBuf[100];
        //sprintf(szBuf, "%d", nItem);
        //MessageBox(szBuf);
    }else if (pNmhder->code==NM_CUSTOMDRAW)
    {
        LPNMLVCUSTOMDRAW lpNMCustomDraw = (LPNMLVCUSTOMDRAW)lParam;
        //m_listCtrl.GetDC()->Rectangle(&(lpNMCustomDraw->nmcd.rc));
    }
    return CWnd::OnNotify(wParam, lParam, pResult);
    //guard_end_ex;
}

void CBrushMapForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    //guard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;
    if (pSliderCtrl!=&m_wndSlider)
    {
        return;
    }
    pEditor->m_BrushTool.m_Drawer.SetSize1(m_wndSlider.GetPos());
    CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
    //guard_end_ex;
}
