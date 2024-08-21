// TerrainBrushView.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "pathpoint_list_view.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"

/////////////////////////////////////////////////////////////////////////////
// CPathPointListView
const UINT AREA_LIST_VIEW_SPLITTER_HEIGHT = 3;
const UINT AREA_LIST_EDIT_NAME_HEIGHT = 16;
const UINT AREA_LIST_DATA_SPLITTER_HEIGHT = 3;
extern RtSceneTool* g_pSceneTool;

extern RtScene*     g_pScene;

IMPLEMENT_DYNCREATE(CPathPointListView, CWnd)

CPathPointListView::CPathPointListView()
{
    m_hCursor           = AfxGetApp()->LoadCursor( XTP_IDC_VSPLITBAR);
    m_clrFace           = GetSysColor(COLOR_3DFACE);
    m_clrShadow         = GetSysColor(COLOR_3DSHADOW);
    m_clrHelpBack       = m_clrFace;
    m_clrHelpFore       = GetSysColor(COLOR_BTNTEXT);
    m_nHelpHeight       = 80;
	//jin.wang
	m_PathFind.InitOnce();

	m_bStartPoint = TRUE;
	m_nLastTileX = -1;
	m_nLastTileY = -1;
}

CPathPointListView::~CPathPointListView()
{
}


BEGIN_MESSAGE_MAP(CPathPointListView, CWnd)
	//{{AFX_MSG_MAP(CAreaListForm)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
    ON_WM_VKEYTOITEM()
    ON_COMMAND(ID_TOOLBAR_PATHPOINT_ENABLE, OnToobarEnable)
    //ON_UPDATE_COMMAND_UI(ID_TOOLBAR_PATHPOINT_ENABLE, OnUpdateUIToobarEnable)
    ON_COMMAND(ID_TOOLBAR_PATHPOINT_OPEN_FILE, OnToobarLoadFile)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_PATHPOINT_OPEN_FILE, OnUpdateUIToobarAreaOperate)
    ON_COMMAND(ID_TOOLBAR_PATHPOINT_SAVE_FILE, OnToobarSaveFile)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_PATHPOINT_SAVE_FILE, OnUpdateUIToobarAreaOperate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPathPointListView drawing

void CPathPointListView::OnDraw(CDC* pDC)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    pDC->FillSolidRect(rcClient, m_clrFace);
}

void CPathPointListView::OnPaint()
{
    CPaintDC dcPaint(this); 
    CXTPClientRect rc(this);
    CXTPBufferDC dc(dcPaint, rc);
    OnDraw(&dc);
}

/////////////////////////////////////////////////////////////////////////////
// CPathPointListView diagnostics

#ifdef _DEBUG
void CPathPointListView::AssertValid() const
{
	CWnd::AssertValid();
}

void CPathPointListView::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPathPointListView message handlers

BOOL CPathPointListView::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW));
    CXTPEmptyRect rc;

    if (!CreateEx(0, pszCreateClass, _T(""), WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_CHILD|WS_GROUP, rc, pParentWnd, nID))
        return FALSE;

	VERIFY(m_wndToolBar.CreateToolBar(WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));
    XTPImageManager()->SetMaskColor(RGB(192, 192, 192));
    VERIFY(m_wndToolBar.LoadToolBar(IDR_TOOLBAR_PATHPOINTLIST));

    CRect rcList, rcName, rcData, rcBtnUpdate;
	GetClientRect(&rcList);
    rcName = rcList;
    rcData = rcList;
    rcBtnUpdate = rcName;

    m_listBox.Create(WS_VISIBLE|WS_VSCROLL|LBS_NOTIFY|LBS_SORT|LBS_WANTKEYBOARDINPUT, rcList, this, IDC_LISTBOX_PATHPOINT);
    m_listBox.ModifyStyleEx(0, WS_EX_STATICEDGE);
    m_listBox.SetFont(XTPPaintManager()->GetRegularFont());

    m_wndDataEdit.Create(WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, rcData, this, IDC_EDIT_AREA_DATA);
    m_wndDataEdit.ModifyStyleEx(0, WS_EX_STATICEDGE);
    m_wndDataEdit.SetFont(XTPPaintManager()->GetRegularFont());

    m_btnUpdate.Create("Update", WS_VISIBLE|BS_CENTER|BS_VCENTER, rcBtnUpdate, this, 0xFF);
    m_btnUpdate.SetFont(XTPPaintManager()->GetRegularFont());

    return TRUE;
}

void CPathPointListView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
    Reposition(cx, cy);
}

BOOL CPathPointListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS| WS_CLIPCHILDREN;
	return CWnd::PreCreateWindow(cs);
}

BOOL CPathPointListView::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
//重置路点列表窗口
void CPathPointListView::Reposition(int cx, int cy)
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
		sz = m_wndToolBar.CalcDockingLayout(cx, LM_HORZDOCK|LM_HORZ | LM_COMMIT);
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

//切换隐藏和显示工具
void CPathPointListView::OnToobarEnable()
{
	m_pSceneTool->m_bPathPointTool = !m_pSceneTool->m_bPathPointTool;	
}

//切换是否使用路点工具，不使用时将保存和载入图标变暗
void CPathPointListView::OnUpdateUIToobarAreaOperate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pSceneTool->m_bPathPointTool);
}

//保存文件
void CPathPointListView::OnToobarSaveFile()
{   
	std::list<RtsPathFind::rtPOINT> pathList;
    char szFilter[] = "区域信息文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||";
	CFileDialog fileDlg(FALSE, "txt", ".txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);  

	BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
	if(!bOpen)
		return;

	SaveWaypointFile(fileDlg.GetPathName());
}

//载入文件
void CPathPointListView::OnToobarLoadFile()
{
	static char BASED_CODE szFilter[] = "区域信息文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "txt", ".txt", OFN_OVERWRITEPROMPT, szFilter, this);
    BOOL bOpen = fileDlg.DoModal()==IDOK;
	RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if(!bOpen)
		return;

	LoadWaypointFile(fileDlg.GetPathName());
}

//
// mouse down
//
void CPathPointListView::OnMouseLDown(int x, int y)
{
	//
	// get tilex, tiley
	//
	int tileX,tileY;
	((CEditorFrame*)g_pAppCase)->m_BrushTool.GetTerrainPos(x,y, tileX, tileY, false, false);
	RtSceneBlockTerrain::STileAttr* pAttr = g_pScene->RtsSceneDoc::GetTerrainPathAttr(tileX, tileY);
	if(!pAttr)
		return;

	//
	// link mode
	//
	if(GetKeyState(VK_CONTROL)&0x8000)
	{
		if(pAttr->bWayPoint)
		{
			if(m_bStartPoint)
			{
				m_bStartPoint = FALSE;
				RtCoreLog().Info("start from(%d,%d)\n", tileX, tileY);
				m_nLastTileX = tileX;
				m_nLastTileY = tileY;
			}
			else
			{
				m_bStartPoint = TRUE;
				RtCoreLog().Info("link from(%d,%d) to (%d,%d)\n", m_nLastTileX, m_nLastTileY, tileX, tileY);

				AddOrRemoveLink(tileX, tileY, m_nLastTileX, m_nLastTileY);
			}
		}
	}
	else
	{
		if(!pAttr->bWalk)
		{
			if(pAttr->bWayPoint)
			{
				//
				// remove point
				//
				g_pScene->RtsSceneDoc::RemoveTerrainAttr(tileX, tileY, 0x8);
				CString strText;
				strText.Format("%d,%d", tileX, tileY);
				for(INT i = 0; i < m_listBox.GetCount(); i ++)
				{
					CString strTemp;
					m_listBox.GetText(i, strTemp);

					if(strTemp != strText)
						continue;

					m_listBox.DeleteString(i);
					break;
				}

				RemoveLink(tileX, tileY);
			}
			else
			{
				AddWayPoint(tileX, tileY);
			}
		}
	}
}

void CPathPointListView::AddWayPoint(int x, int y)
{
	g_pScene->RtsSceneDoc::SetTerrainAttr(x, y, 0x8);

	CString strText;
	strText.Format("%d,%d", x, y);

	for(INT i = 0; i < m_listBox.GetCount(); i ++)
	{
		CString strTemp;
		m_listBox.GetText(i, strTemp);

		if(strTemp == strText)
			return;
	}

	m_listBox.AddString(strText);
}

void CPathPointListView::AddLink(int x0, int y0, int x1, int y1)
{
	vector<WAY_EDGE>::iterator it = m_vtEdges.begin();
	for(; it != m_vtEdges.end(); ++ it)
	{
		if(it->m_ptStartTile.x == x0 && it->m_ptStartTile.y == y0 && it->m_ptEndTile.x == x1 && it->m_ptEndTile.y == y1)
		{
			return;
		}
	}

	WAY_EDGE newEdge;
	newEdge.m_ptStartTile.x = x0;
	newEdge.m_ptStartTile.y = y0;

	newEdge.m_ptEndTile.x = x1;
	newEdge.m_ptEndTile.y = y1;

	m_vtEdges.push_back(newEdge);
}

void CPathPointListView::RemoveLink(int x, int y)
{
	vector<WAY_EDGE>::iterator it = m_vtEdges.begin();
	while(it != m_vtEdges.end())
	{
		if((it->m_ptStartTile.x == x && it->m_ptStartTile.y == y) || (it->m_ptEndTile.x == x && it->m_ptEndTile.y == y))
			it = m_vtEdges.erase(it);
		else
			 ++it;
	}
}

void CPathPointListView::AddOrRemoveLink(int x0, int y0, int x1, int y1)
{
	vector<WAY_EDGE>::iterator it = m_vtEdges.begin();
	for(; it != m_vtEdges.end(); ++ it)
	{
		if( (it->m_ptStartTile.x == x0 && it->m_ptStartTile.y == y0 && it->m_ptEndTile.x == x1 && it->m_ptEndTile.y == y1) ||
			(it->m_ptStartTile.x == x1 && it->m_ptStartTile.y == y1 && it->m_ptEndTile.x == x0 && it->m_ptEndTile.y == y0))
		{
			m_vtEdges.erase(it);
			return;
		}
	}

	WAY_EDGE newEdge;
	newEdge.m_ptStartTile.x = x0;
	newEdge.m_ptStartTile.y = y0;

	newEdge.m_ptEndTile.x = x1;
	newEdge.m_ptEndTile.y = y1;

	m_vtEdges.push_back(newEdge);
//加上反向边
	newEdge.m_ptStartTile.x = x1;
	newEdge.m_ptStartTile.y = y1;

	newEdge.m_ptEndTile.x = x0;
	newEdge.m_ptEndTile.y = y0;

	m_vtEdges.push_back(newEdge);
}

void CPathPointListView::RenderLinks(RtgCamera& inCamera, RtgDevice& inDevice)
{
	vector<WAY_EDGE>::iterator it = m_vtEdges.begin();
	for(; it != m_vtEdges.end(); ++ it)
	{
		RtgVertex3 worldPos0;
		g_pScene->GetPosByTerrain(it->m_ptStartTile.x, it->m_ptStartTile.y, worldPos0);
		worldPos0.z += 5;

		RtgVertex3 worldPos1;
		g_pScene->GetPosByTerrain(it->m_ptEndTile.x, it->m_ptEndTile.y, worldPos1);
		worldPos1.z += 5;

		DWORD color = 0xffff0000;
		inDevice.DrawLine(worldPos0, worldPos1, &color);
	}
}

void CPathPointListView::RenderTips(RtgDevice& inDevice)
{
	CString strOutput;
	if(!m_bStartPoint)
		strOutput.Format("start(%d, %d), ctrl+left to link against the other waypoint", m_nLastTileX, m_nLastTileY);
	else
		strOutput.Format("left button to add/remove waypoint, crtl+left to begin link waypoints");

	inDevice.DrawString((inDevice.m_iWndWidth - strOutput.GetLength()*8)/2, inDevice.m_iWndHeight*3/4, 0xFFFF0000, (LPCTSTR)strOutput);
}

void CPathPointListView::LoadWaypointFile(LPCTSTR szFileName)
{
    return;
    begin_log_tick(0);

	FILE *fp = fopen(szFileName, "r");
	if(!fp)
		return;

	vector<WAY_EDGE>::iterator it = m_vtEdges.begin();
	for(; it != m_vtEdges.end(); ++ it)
	{
		g_pScene->RtsSceneDoc::RemoveTerrainAttr(it->m_ptStartTile.x, it->m_ptStartTile.y, 0x8);
		g_pScene->RtsSceneDoc::RemoveTerrainAttr(it->m_ptEndTile.x, it->m_ptEndTile.y, 0x8);
	}

	m_vtEdges.clear();
	m_listBox.ResetContent();
	
    // done
    // g_pScene->LoadAllBlock();

	CHAR szLine[1024] = {0};
	while(fgets(szLine, 1024, fp))
	{
		if(!szLine[0])
			continue;

		char* npos = strtok(szLine, ",");
		if(npos == NULL)
			continue;

		int x0 = atol(npos);

		npos = strtok(NULL, ",");
		if(npos == NULL)
			continue;

		int y0 = atol(npos);

		AddWayPoint(x0, y0);

		while (npos != NULL)
		{
			npos = strtok(NULL, ",");
			if(npos == NULL)
				break;

			int x1 = atol(npos);

			npos = strtok(NULL, ",");
			if(npos == NULL)
				break;

			int y1 = atol(npos);

			AddLink(x0, y0, x1, y1);

			AddWayPoint(x1, y1);
		}
	}

	fclose(fp);

    end_log_tick2(0, "load way point", szFileName);
}

void CPathPointListView::SaveWaypointFile(LPCTSTR szFileName)
{
	FILE *fp = fopen(szFileName, "w");
	if(!fp)
		return;

	vector<vector<CPoint>> vtOutput;
	vector<WAY_EDGE>::iterator it = m_vtEdges.begin();
	for(; it != m_vtEdges.end(); ++ it)
	{
		size_t i;
		for(i = 0; i < vtOutput.size(); ++i)
		{
			CString strLine;
			vector<CPoint>& outputPoints = vtOutput[i];
			if(outputPoints[0].x == it->m_ptStartTile.x && outputPoints[0].y == it->m_ptStartTile.y)
				break;
		}

		if(i != vtOutput.size())
		{
			vtOutput[i].push_back(it->m_ptEndTile);
		}
		else
		{
			vector<CPoint> temp;
			temp.push_back(it->m_ptStartTile);
			temp.push_back(it->m_ptEndTile);
			vtOutput.push_back(temp);
		}
	}

	for(size_t i = 0; i < vtOutput.size(); ++i)
	{
		CString strLine;
		vector<CPoint>& outputPoints = vtOutput[i];
		for(size_t j = 0; j < outputPoints.size(); j ++)
			strLine.AppendFormat(j ? ", %d, %d" : "%d, %d", outputPoints[j].x, outputPoints[j].y);

		fprintf(fp, "%s\n", (LPCTSTR)strLine);
	}

	fclose(fp);
}
