// TerrainBrushView.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "area_list_view.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"

/////////////////////////////////////////////////////////////////////////////
// CAreaListForm
const UINT AREA_LIST_VIEW_SPLITTER_HEIGHT = 3;
const UINT AREA_LIST_EDIT_NAME_HEIGHT = 16;
const UINT AREA_LIST_DATA_SPLITTER_HEIGHT = 3;

IMPLEMENT_DYNCREATE(CAreaListForm, CWnd)

CAreaListForm::CAreaListForm()
{
    m_hCursor           = AfxGetApp()->LoadCursor( XTP_IDC_VSPLITBAR);
    m_clrFace           = GetSysColor(COLOR_3DFACE);
    m_clrShadow         = GetSysColor(COLOR_3DSHADOW);
    m_clrHelpBack       = m_clrFace;
    m_clrHelpFore       = GetSysColor(COLOR_BTNTEXT);
    m_nHelpHeight       = 80;
}

CAreaListForm::~CAreaListForm()
{
}


BEGIN_MESSAGE_MAP(CAreaListForm, CWnd)
	//{{AFX_MSG_MAP(CAreaListForm)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
    ON_WM_LBUTTONDOWN() 
    ON_LBN_DBLCLK(IDC_LISTBOX_AREA, OnListBoxDoubleClick)
    ON_LBN_SELCHANGE(IDC_LISTBOX_AREA, OnListBoxSelectChange)
    ON_WM_VKEYTOITEM()
    ON_EN_KILLFOCUS(IDC_EDIT_AREA_NAME, OnEditNameKillFocus)
    ON_EN_KILLFOCUS(IDC_EDIT_AREA_DATA, OnEditDataKillFocus)
    ON_COMMAND(ID_TOOLBAR_AREA_ENABLE, OnToobarEnable)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_AREA_ENABLE, OnUpdateUIToobarEnable)
    ON_COMMAND(ID_TOOLBAR_AREA_OPEN_FILE, OnToobarLoadFile)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_AREA_OPEN_FILE, OnUpdateUIToobarAreaOperate)
    ON_COMMAND(ID_TOOLBAR_AREA_SAVE_FILE, OnToobarSaveFile)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_AREA_SAVE_FILE, OnUpdateUIToobarAreaOperate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaListForm drawing

void CAreaListForm::OnDraw(CDC* pDC)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    pDC->FillSolidRect(rcClient, m_clrFace);

    /*
    CRect rcHelp = rcClient;
    rcHelp.top = rcHelp.bottom - m_nHelpHeight;

    CXTPPenDC pen(*pDC, m_clrShadow);
    CXTPBrushDC brush(*pDC, m_clrHelpBack);

    pDC->Rectangle(rcHelp);


    //CXTPFontDC font(pDC, &pView->m_fontBold);
    CXTPFontDC font(pDC, XTPPaintManager()->GetRegularFont());

    pDC->SetTextColor(m_clrHelpFore);
    pDC->SetBkColor(m_clrHelpBack);
    CRect rcCaption(rcHelp);
    rcCaption.DeflateRect(6, 3);
    //pDC->DrawText(pItem->GetCaption(), rcCaption, DT_SINGLELINE);
    pDC->DrawText("Caption", rcCaption, DT_SINGLELINE);

    CString strDesc = "近期出现TO游戏帐号被盗的情况，已知被盗号的玩家约有数十位。"
        "经技术部查证：密码问题和密码问题答案设置为111111的约有两千多位玩家。"
        "为保护玩家帐号，游戏帐号的“取回密码”功能现已暂时被锁定。";
    if (!strDesc.IsEmpty())
    {
        //CXTPFontDC font(pDC, &pView->m_fontNormal);
        CXTPFontDC font(pDC, XTPPaintManager()->GetRegularFont());
        CXTPEmptyRect rcCaption;
        //pDC->DrawText(pItem->GetCaption(), rcCaption, DT_SINGLELINE|DT_CALCRECT);
        pDC->DrawText("Caption", rcCaption, DT_SINGLELINE|DT_CALCRECT);
        CRect rcDesc(rcHelp);
        rcDesc.DeflateRect(6, 3 + rcCaption.Height() + 3, 6, 3);
        pDC->DrawText(strDesc, rcDesc, DT_WORDBREAK);
    }
    */
}

void CAreaListForm::OnPaint()
{
    CPaintDC dcPaint(this); 
    CXTPClientRect rc(this);
    CXTPBufferDC dc(dcPaint, rc);
    OnDraw(&dc);
}

/////////////////////////////////////////////////////////////////////////////
// CAreaListForm diagnostics

#ifdef _DEBUG
void CAreaListForm::AssertValid() const
{
	CWnd::AssertValid();
}

void CAreaListForm::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAreaListForm message handlers

void CAreaListForm::OnInitialUpdate() 
{
}

BOOL CAreaListForm::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW));
    CXTPEmptyRect rc;

    if (!CreateEx(0, pszCreateClass, _T(""), WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_CHILD|WS_GROUP, rc, pParentWnd, nID))
        return FALSE;

	VERIFY(m_wndToolBar.CreateToolBar(WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));
    XTPImageManager()->SetMaskColor(RGB(192, 192, 192));
    VERIFY(m_wndToolBar.LoadToolBar(IDR_TOOLBAR_AREALIST));

    CRect rcList, rcName, rcData, rcBtnUpdate;
	GetClientRect(&rcList);
    rcName = rcList;
    rcData = rcList;
    rcBtnUpdate = rcName;

    /*
    rcList.bottom -= (AREA_LIST_VIEW_SPLITTER_HEIGHT + m_nHelpHeight);
    rcName.top = rcList.bottom + AREA_LIST_VIEW_SPLITTER_HEIGHT;
    rcName.bottom = rcName.top + AREA_LIST_EDIT_NAME_HEIGHT;
    rcBtnUpdate = rcName;
    rcName.right -= 30;
    rcBtnUpdate.left = rcName.right + 2;
    rcData.top = rcName.bottom + AREA_LIST_DATA_SPLITTER_HEIGHT;
    */

    m_listBox.Create(WS_VISIBLE|WS_VSCROLL|LBS_NOTIFY|LBS_SORT|LBS_WANTKEYBOARDINPUT, rcList, this, IDC_LISTBOX_AREA);
    m_listBox.ModifyStyleEx(0, WS_EX_STATICEDGE);
    m_listBox.SetFont(XTPPaintManager()->GetRegularFont());

    m_wndNameEdit.Create(WS_VISIBLE | ES_AUTOHSCROLL, rcName, this, IDC_EDIT_AREA_NAME);
    m_wndNameEdit.ModifyStyleEx(0, WS_EX_STATICEDGE);
    m_wndNameEdit.SetFont(XTPPaintManager()->GetRegularFont());

    m_wndDataEdit.Create(WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, rcData, this, IDC_EDIT_AREA_DATA);
    m_wndDataEdit.ModifyStyleEx(0, WS_EX_STATICEDGE);
    m_wndDataEdit.SetFont(XTPPaintManager()->GetRegularFont());

    m_btnUpdate.Create("Update", WS_VISIBLE|BS_CENTER|BS_VCENTER, rcBtnUpdate, this, 0xFF);
    //m_btnUpdate.ModifyStyleEx(0, WS_EX_STATICEDGE);
    m_btnUpdate.SetFont(XTPPaintManager()->GetRegularFont());

    return TRUE;
}

void CAreaListForm::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
    Reposition(cx, cy);
}

BOOL CAreaListForm::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS| WS_CLIPCHILDREN;
	return CWnd::PreCreateWindow(cs);
}

BOOL CAreaListForm::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CAreaListForm::OnSetFocus(CWnd* /*pOldWnd*/) 
{
	if (m_listBox.GetSafeHwnd())
	{	
		m_listBox.SetFocus();
	}
}

bool CAreaListForm::HitTestSplitter(CPoint pt)
{
    CXTPClientRect rc(this);
    CRect rcSplitter(CPoint(rc.left, rc.bottom - AREA_LIST_VIEW_SPLITTER_HEIGHT - m_nHelpHeight), CSize(rc.Width(), AREA_LIST_VIEW_SPLITTER_HEIGHT));
    if (rcSplitter.PtInRect(pt))
        return true;
    return false;
}

BOOL CAreaListForm::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT)
    {
        CPoint point;
        GetCursorPos(&point);
        ScreenToClient(&point);
        if (HitTestSplitter(point))
        {
            SetCursor( m_hCursor );
            return TRUE;
        }
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CAreaListForm::OnInvertTracker(CRect rect)
{
    ASSERT_VALID(this);
    ASSERT(!rect.IsRectEmpty());
    ASSERT((GetStyle() & WS_CLIPCHILDREN) == 0);

    // pat-blt without clip children on
    CDC* pDC = GetDC();

    CBrush brush(GetSysColor(COLOR_3DFACE));
    CBrush* pBrush = (CBrush*)pDC->SelectObject(&brush);
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
    pDC->SelectObject(pBrush);

    ReleaseDC(pDC);
}

void CAreaListForm::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (HitTestSplitter(point))
    {
        SetCapture();
        CXTPClientRect rc(this);

        ModifyStyle(WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, FALSE);

        m_rectTracker.SetRect(0, point.y, rc.Width(), point.y + 3);

        CSize sz(0);
        sz = m_wndToolBar.CalcDockingLayout(rc.Width(), /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

        OnInvertTracker(m_rectTracker);

        BOOL bAccept = FALSE;
        while (GetCapture() == this)
        {
            MSG msg;
            if (!GetMessage(&msg, NULL, 0, 0))
                break;

            if (msg.message == WM_MOUSEMOVE)
            {
                point = CPoint(msg.lParam);
                point.y = __min(point.y, rc.Height() - 20);
                point.y = __max(point.y, 20 + sz.cy);

                if (m_rectTracker.top != point.y)
                {
                    OnInvertTracker(m_rectTracker);
                    m_rectTracker.OffsetRect(0, point.y - m_rectTracker.top);
                    OnInvertTracker(m_rectTracker);
                }
            }
            else if (msg.message == WM_KEYDOWN &&  msg.wParam == VK_ESCAPE)	break;
            else if (msg.message ==WM_LBUTTONUP)
            {
                bAccept = TRUE;
                break;
            }
            else  ::DispatchMessage(&msg);
        }

        ReleaseCapture();

        if (bAccept)
        {
            m_nHelpHeight = rc.bottom - m_rectTracker.top -2;
            Reposition(rc.Width(), rc.Height());
        } 
        Invalidate(FALSE);
        ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS, FALSE);
        return;
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CAreaListForm::Reposition(int cx, int cy)
{
    CRect rcList(0, 0, cx, cy), rcName, rcData, rcBtnUpdate;
    rcName = rcList;
    rcData = rcList;

    rcList.bottom -= (AREA_LIST_VIEW_SPLITTER_HEIGHT + m_nHelpHeight);
    rcName.top = rcList.bottom + AREA_LIST_VIEW_SPLITTER_HEIGHT;
    rcName.bottom = rcName.top + AREA_LIST_EDIT_NAME_HEIGHT;
    rcBtnUpdate = rcName;
    rcName.right -= 45;
    rcBtnUpdate.left = rcName.right + 2;
    rcData.top = rcName.bottom + AREA_LIST_DATA_SPLITTER_HEIGHT;

    if (m_wndToolBar.GetSafeHwnd())
    {
        CSize sz(0);
        sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);
        CRect rcToolBar(0, 0, cx, sz.cy);
        m_wndToolBar.MoveWindow(rcToolBar);
        m_wndToolBar.Invalidate(FALSE);
        rcList.top += sz.cy;
    }
    if (m_listBox.GetSafeHwnd())
    {
        m_listBox.MoveWindow(rcList);
    }
    if (m_wndNameEdit.GetSafeHwnd())
    {
        m_wndNameEdit.MoveWindow(rcName);
    }
    if (m_wndDataEdit.GetSafeHwnd())
    {
        m_wndDataEdit.MoveWindow(rcData);
    }
    if (m_btnUpdate.GetSafeHwnd())
    {
        m_btnUpdate.MoveWindow(rcBtnUpdate);
    }
}

void CAreaListForm::UpdateAreaList()
{
    RtsAreaHelper* pArea;
    m_listBox.ResetContent();
    for (pArea=m_pSceneTool->m_Areas.GetFirst(); pArea; pArea=m_pSceneTool->m_Areas.GetNext())
    {
        AddArea(pArea);
    }
}

void CAreaListForm::AddArea(RtsAreaHelper* pArea)
{
    int iIndex;
    if (pArea->GetName().GetLength())
    {
        iIndex = m_listBox.AddString(pArea->GetName());
    }else
    {
        iIndex = m_listBox.AddString("[noname]");
    }
    if (iIndex!=LB_ERR && iIndex!=LB_ERRSPACE)
    {
        m_listBox.SetItemDataPtr(iIndex, pArea);
    }else
    {
        MessageBox("不能网AreaList添加内容，请和Janhail联系");
    }
}

void CAreaListForm::RemoveArea(RtsAreaHelper* pArea)
{
    if (pArea==NULL) return;
    if (MessageBox("真的要删除区域块吗？", "删除确认", MB_YESNO|MB_ICONQUESTION)!=IDYES)
    {
        return;
    }
    m_pSceneTool->DeleteArea(pArea);
    for (int i=0; i<m_listBox.GetCount(); i++)
    {
        if (pArea==(RtsAreaHelper*)m_listBox.GetItemDataPtr(i))
        {
            m_listBox.DeleteString(i);
            if (i>=m_listBox.GetCount())
            {
                m_listBox.SetCurSel(m_listBox.GetCount()-1);
            }else
            {
                m_listBox.SetCurSel(i);
            }
            return;
        }
    }
}

void CAreaListForm::SelectArea(RtsAreaHelper* pArea)
{
    m_pSceneTool->SelectArea(pArea);
    for (int i=0; i<m_listBox.GetCount(); i++)
    {
        if (pArea==(RtsAreaHelper*)m_listBox.GetItemDataPtr(i))
        {
            m_listBox.SetCurSel(i);
            return;
        }
    }
}

void CAreaListForm::OnListBoxDoubleClick()
{
    int iIdx = m_listBox.GetCurSel();
    if (iIdx<0) return;
    // 屏幕移动到区域块位置
    RtsAreaHelper* pArea = (RtsAreaHelper*)m_listBox.GetItemDataPtr(iIdx);
    if (pArea)
    {
        m_pSceneTool->MoveToArea(pArea);
    }
}

void CAreaListForm::OnListBoxSelectChange()
{
    int iIdx = m_listBox.GetCurSel();
    if (iIdx<0) return;
    // 选中该区域
    RtsAreaHelper* pArea = (RtsAreaHelper*)m_listBox.GetItemDataPtr(iIdx);
    if (pArea)
    {
        m_pSceneTool->SelectArea(pArea);

        m_wndNameEdit.SetWindowText(pArea->GetName());
        m_wndDataEdit.SetWindowText(pArea->szData);
    }
}

int CAreaListForm::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
    if (nKey==VK_DELETE)
    {
        CHECK(m_listBox.GetCurSel()==nIndex);
        RtsAreaHelper* pArea = (RtsAreaHelper*)m_listBox.GetItemDataPtr(nIndex);
        if (pArea)
        {
            if (MessageBox("真的要删除区域块吗？", "删除确认", MB_YESNO|MB_ICONQUESTION)!=IDYES)
            {
                return nIndex;
            }
            m_pSceneTool->DeleteArea(pArea);
            m_listBox.DeleteString(nIndex);
            if (nIndex>=m_listBox.GetCount())
            {
                return m_listBox.GetCount()-1;
            }
            m_listBox.SetFocus();
        }
    }
    return nIndex;
}

void CAreaListForm::OnEditNameKillFocus()
{
    int iIdx = m_listBox.GetCurSel();
    if (iIdx<0) return;
    RtsAreaHelper* pArea = (RtsAreaHelper*)m_listBox.GetItemDataPtr(iIdx);
    if (pArea)
    {
        if (m_wndNameEdit.GetModify())
        {
            CString szText;
            m_wndNameEdit.GetWindowText(szText);
            m_pSceneTool->m_Areas.SetName(pArea, szText);
            m_listBox.InsertString(iIdx, szText);
            m_listBox.SetItemDataPtr(iIdx, pArea);
            m_listBox.DeleteString(iIdx+1);
            m_listBox.SetCurSel(iIdx);
        }
    }
}

void CAreaListForm::OnEditDataKillFocus()
{
    int iIdx = m_listBox.GetCurSel();
    if (iIdx<0) return;
    RtsAreaHelper* pArea = (RtsAreaHelper*)m_listBox.GetItemDataPtr(iIdx);
    if (pArea)
    {
        if (m_wndDataEdit.GetModify())
        {
            CString szText;
            m_wndDataEdit.GetWindowText(szText);
            pArea->szData = szText;
        }
    }
}

void CAreaListForm::OnUpdateUIToobarEnable(CCmdUI* pCmdUI)
{
    pCmdUI->SetText(m_pSceneTool->m_bAreaTool?"关闭区域编辑工具":"打开区域编辑工具");
}

void CAreaListForm::OnUpdateUIToobarAreaOperate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_pSceneTool->m_bAreaTool);
}

void CAreaListForm::OnToobarEnable()
{
    m_pSceneTool->m_bAreaTool = !m_pSceneTool->m_bAreaTool;
    if (m_pSceneTool->m_bAreaTool)
    {
        UpdateAreaList();
    }
}

void CAreaListForm::OnToobarSaveFile()
{
    m_pSceneTool->m_bRender = FALSE;
    static char BASED_CODE szFilter[] = "区域信息文件 (*.rsr)|*.rsr|所有文件 (*.*)|*.*||";
    CFileDialog fileDlg(FALSE, "rsr", ".rsr", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        m_pSceneTool->m_Areas.SaveToFile(fileDlg.GetPathName());
    }
    m_pSceneTool->m_bRender = TRUE;
}

void CAreaListForm::OnToobarLoadFile()
{
    m_pSceneTool->m_bRender = FALSE;
    static char BASED_CODE szFilter[] = "区域信息文件 (*.rsr)|*.rsr|所有文件 (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "rsr", ".rsr", OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        m_pSceneTool->m_Areas.LoadFromFile(fileDlg.GetPathName());
        UpdateAreaList();
    }
    m_pSceneTool->m_bRender = TRUE;
}
