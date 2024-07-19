// DlgNewBlock.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "DlgNewBlock.h"
#include "DlgBlockInfo.h"
#include "scene/rt_scene.h"
#include "resource.h"
#include "rts_scene_tool.h"
#include ".\dlgnewblock.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"

extern RtScene* g_pScene1;

// CDlgNewBlock dialog
RtSceneTool* g_pSceneTool1;
bool g_bDrawTop = true;
int g_offsetX = 0;
int g_offsetY = 0;


IMPLEMENT_DYNAMIC(CDlgNewBlock, CDialog)
CDlgNewBlock::CDlgNewBlock(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewBlock::IDD, pParent),nCurHPos(0),nCurVPos(0)
{
    m_pView = NULL;
}

CDlgNewBlock::~CDlgNewBlock()
{
}

void CDlgNewBlock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// DDX_Control(pDX, IDC_BLOCK_DRAW, m_View);
	DDX_Control(pDX, IDC_NEW_BLOCK_ATTRIB, m_Attrib);
	DDX_Control(pDX, IDC_SCROLLBAR3, m_vscroll);
	DDX_Control(pDX, IDC_SCROLLBAR4, m_hscroll);
}


BEGIN_MESSAGE_MAP(CDlgNewBlock, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_GENERATE_ALL_MINIMAP, OnBnClickedButtonGenerateAllMinimap)
	ON_COMMAND(IDC_BUTTON1,OnFileOpen)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgNewBlock message handlers

BOOL CDlgNewBlock::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_pView = new CNewBlockView;
    CWnd* pWnd = GetDlgItem(IDC_BLOCK_DRAW);
    CRect rt;
    pWnd->GetClientRect(&rt);
    pWnd->ClientToScreen(&rt);
    this->ScreenToClient(&rt);
    m_pView->Create(NULL, "CNewBlockView", WS_VISIBLE|WS_CHILD, rt, this, 0);

    m_pView->m_pDlgNewBlock = this;

	m_vscroll.SetScrollRange(0,100);
	m_hscroll.SetScrollRange(0,100);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CNewBlockView, CWnd)
    ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_MENU_BO_MOVETO,   OnMenuBlockOpMoveTo)
    ON_COMMAND(ID_MENU_BO_MINIMAP,  OnMenuBlockOpMiniMap)
    ON_COMMAND(ID_MENU_BO_PROPERTY, OnMenuBlockOpProperty)
    ON_COMMAND(ID_MENU_BO_DELETE,   OnMenuBlockOpDelete)
	ON_COMMAND(ID_MENU_BO_COPY, OnMenuBlockOpCopy)
	ON_COMMAND(ID_MENU_BO_POST, OnMenuBlockOpPost)
//	ON_WM_VSCROLL()
//	ON_WM_HSCROLL()
END_MESSAGE_MAP()

CNewBlockView::CNewBlockView()
{
    m_iDrawStartX = m_iDrawStartY = 0;
    m_iGridStartX = m_iGridStartY = 0;
    m_iSelectGridX = m_iSelectGridY = m_iSelectGridIsDummy = 0;
    m_iGridWidth = 19;
    m_iGridSpaceBetween = 1;
	m_bDrag = FALSE;
	m_pBlockMap = NULL;
	m_iScene = 0;
}

CNewBlockView::~CNewBlockView()
{
}

bool CNewBlockView::GetPosition(int iX, int iY, int& iPosX, int& iPosY, bool& bDummy,int& iScene)
{
	if(g_bDrawTop)//g_pScene1==NULL || iY< m_iDrawStartY1+(m_iGridWidth+m_iGridSpaceBetween)+10)
	{
		iScene = 0;
		if (g_pScene==NULL) return false;
		if (((iX-m_iDrawStartX)%(m_iGridWidth+m_iGridSpaceBetween))>=m_iGridWidth) return false;
		if (((iY-m_iDrawStartY)%(m_iGridWidth+m_iGridSpaceBetween))>=m_iGridWidth) return false;
		if (iX<m_iDrawStartX-(m_iGridWidth+m_iGridSpaceBetween)) return false;
		if (iY>m_iDrawStartY+(m_iGridWidth+m_iGridSpaceBetween)) return false;
		int iPX = ((iX-m_iDrawStartX+(m_iGridWidth+m_iGridSpaceBetween))/(m_iGridWidth+m_iGridSpaceBetween));
		int iPY = ((m_iDrawStartY+(m_iGridWidth+m_iGridSpaceBetween)-iY)/(m_iGridWidth+m_iGridSpaceBetween));
		if (iPX<0 || iPY<0 || iPX>g_pScene->GetBlockCntX()+1 || iPY>g_pScene->GetBlockCntY()+1) return false;
		if (iPX==0 || iPY==0 || iPX==g_pScene->GetBlockCntX()+1 || iPY==g_pScene->GetBlockCntY()+1) bDummy = true;
		else bDummy = false;
		iPosX = iPX-1;
		iPosY = iPY-1;
		return true;
	}
	else
	{
		iScene = 1;
		if (g_pScene1==NULL) return false;
        if (((iX-m_iDrawStartX)%(m_iGridWidth+m_iGridSpaceBetween))>=m_iGridWidth) return false;
        if (((iY-m_iDrawStartY)%(m_iGridWidth+m_iGridSpaceBetween))>=m_iGridWidth) return false;
        if (iX<m_iDrawStartX-(m_iGridWidth+m_iGridSpaceBetween)) return false;
        if (iY>m_iDrawStartY+(m_iGridWidth+m_iGridSpaceBetween)) return false;
        int iPX = ((iX-m_iDrawStartX+(m_iGridWidth+m_iGridSpaceBetween))/(m_iGridWidth+m_iGridSpaceBetween));
		int iPY = ((m_iDrawStartY+(m_iGridWidth+m_iGridSpaceBetween)-iY)/(m_iGridWidth+m_iGridSpaceBetween));
		if (iPX<0 || iPY<0 || iPX>g_pScene1->GetBlockCntX()+1 || iPY>g_pScene1->GetBlockCntY()+1) return false;
		if (iPX==0 || iPY==0 || iPX==g_pScene1->GetBlockCntX()+1 || iPY==g_pScene1->GetBlockCntY()+1) bDummy = true;
		else bDummy = false;
		iPosX = iPX-1;
		iPosY = iPY-1;
		return true;
	}  
}

void CNewBlockView::GetRect(int iPosX, int iPosY, CRect& rect,int s)
{
    rect.left = m_iDrawStartX+iPosX*(m_iGridWidth+m_iGridSpaceBetween);
	rect.top  = (s?m_iDrawStartY:m_iDrawStartY1)-iPosY*(m_iGridWidth+m_iGridSpaceBetween);
    rect.right = rect.left + m_iGridWidth;
    rect.bottom = rect.top - m_iGridWidth;
}

void CNewBlockView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//int x,y,s;
	//bool bDummy;
	//RtsSceneBlockMap* pMap;
	//if(!GetPosition(point.x,point.y,x,y,bDummy,s))return;
	//if(!bDummy)
	//{
 //       if(!s)
 //           pMap = g_pScene->GetBlockMap(m_iSelectGridX-g_pScene->GetCenterX(), m_iSelectGridY-g_pScene->GetCenterY());
	//	else
	//	    pMap = g_pScene1->GetBlockMap(m_iSelectGridX-g_pScene1->GetCenterX(), m_iSelectGridY-g_pScene1->GetCenterY());
	//	if(pMap->m_szFileName[0])
	//	{
 //           m_bDrag = true;
	//	    //g_pMap = pMap;
	//		g_pScene->m_iOldBlockXCnt = m_iSelectGridX-g_pScene1->GetCenterX();
	//		g_pScene->m_iOldBlockYCnt = m_iSelectGridY-g_pScene1->GetCenterY();
	//		g_pScene->m_bMergeBlock = true;
	//	}
	//	
	//}
    
    CWnd::OnLButtonDown(nFlags, point);
}
void CNewBlockView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//if(!m_bDrag)return;
	//int x,y,s;
	//bool bDummy;
	//if(!GetPosition(point.x,point.y,x,y,bDummy,s))return;
	////g_pScene->AddMapBlock(x,y,g_pMap);
	//RedrawWindow();
	//m_bDrag = FALSE;
	CWnd::OnLButtonUp(nFlags,point);
}

void CNewBlockView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    int x, y,s;
    bool bDummy;
    if (!GetPosition(point.x, point.y, x, y, bDummy,s)) return;
	if(s)return;
    g_pScene->AddMapBlock(x, y);
    RedrawWindow();
    CWnd::OnLButtonDblClk(nFlags, point);
}

void CNewBlockView::OnRButtonDown(UINT nFlags, CPoint point)
{
    int x, y;
    bool bDummy;
    if (!GetPosition(point.x, point.y, x, y, bDummy, m_iScene)) return;
 
	if(!m_iScene)
	{
		m_iLastMenuX = x-g_pScene->GetCenterX();
		m_iLastMenuY = y-g_pScene->GetCenterY();
		
	}
	else
	{
		m_iLastMenuX = x-g_pScene1->GetCenterX();
		m_iLastMenuY = y-g_pScene1->GetCenterY();
		
	}

	CMenu menu;
	menu.LoadMenu(IDR_MENU_BLOCK_OP);
	ClientToScreen(&point);
	TrackPopupMenu(menu.GetSubMenu(0)->GetSafeHmenu(), TPM_LEFTALIGN, point.x, point.y, 0, (HWND)(this->GetSafeHwnd()), NULL);

    CWnd::OnRButtonDown(nFlags, point);
}

void CNewBlockView::OnMenuBlockOpMoveTo(void)
{
	if(m_iScene)return;
    m_pBlockMap = g_pScene->GetBlockMap(m_iLastMenuX, m_iLastMenuY);	
    if (m_pBlockMap)
    {
        RtgVertex3 vPos;
        vPos.x = (m_iLastMenuX+0.5f) * g_fSceneBlockWidth;
        vPos.y = (m_iLastMenuY+0.5f) * g_fSceneBlockHeight;
        vPos.z = 512;
		//g_pScene->GetTerrainHeight(vPos.x, vPos.y, vPos.z);
        RtGetRender()->m_pCamera->SetPosition(vPos);
    }
}

void CNewBlockView::OnMenuBlockOpMiniMap(void)
{
	if(m_iScene)return;
    char szCmd[40];
    strcpy(szCmd, "> stat alloff");
    ((CEditorFrame*)g_pAppCase)->DoConsoleCommand(szCmd);
    ((RtSceneTool*)g_pScene)->GenerateMiniMap(m_iLastMenuX, m_iLastMenuY);
    strcpy(szCmd, "> stat fps");
    ((CEditorFrame*)g_pAppCase)->DoConsoleCommand(szCmd);
}

void CNewBlockView::OnMenuBlockOpProperty(void)
{
	if(!m_iScene)
		m_pBlockMap = g_pScene->GetBlockMap(m_iLastMenuX, m_iLastMenuY);
	else
		m_pBlockMap = g_pScene1->GetBlockMap(m_iLastMenuX,m_iLastMenuY);
    
    if (m_pBlockMap && m_pBlockMap->m_szFileName[0])
    {
        CDlgBlockInfo dlgInfo;
        dlgInfo.Init(m_pBlockMap);
        dlgInfo.DoModal();
    }
}

void CNewBlockView::OnMenuBlockOpDelete(void)
{
	if(m_iScene)return;
	m_pBlockMap = g_pScene->GetBlockMap(m_iLastMenuX,m_iLastMenuY);

	if(m_pBlockMap)
	{
		//RtsSceneBlockMap* pMap = g_pScene->GetBlockMap(m_iLastMenuX, m_iLastMenuY);
        CString szMsg = "你确定要删除该场景块吗？";
		szMsg += "\n    块名字为：";
		szMsg += m_pBlockMap->m_szBlockName;
		szMsg += "\n    块文件名：";
		szMsg += m_pBlockMap->m_szFileName;
		szMsg += "\n  删除操作将删除该块的所有地表、物件、水面、灯光等等，并且无法恢复。";
		if (MessageBox(szMsg, "删除场景块", MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		{
			g_pScene->DeleteBlockMap(m_iLastMenuX, m_iLastMenuY);
		}
	}
    
}

void CNewBlockView::OnMenuBlockOpCopy(void)
{
	//RtsSceneBlockMap* pMap = g_pScene->GetBlockMap(m_iLastMenuX,m_iLastMenuY);
    if(!m_iScene)return;
	m_pBlockMap = g_pScene1->GetBlockMap(m_iLastMenuX,m_iLastMenuY);
	if(m_pBlockMap->m_szFileName[0])
	{
		m_bDrag = true;
		//g_pMap = pMap;
		g_pScene->m_iOldBlockXCnt = m_iLastMenuX;
		g_pScene->m_iOldBlockYCnt = m_iLastMenuY;
		g_pScene->m_bMergeBlock = true;
	}
}

void CNewBlockView::OnMenuBlockOpPost(void)
{
	if(m_iScene)return;
	if(!m_bDrag)return;
	g_pScene->AddMapBlock(m_iSelectGridX,m_iSelectGridY,m_pBlockMap);
	RedrawWindow();
	m_bDrag = FALSE;
	m_pBlockMap = NULL;
}

void CNewBlockView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (g_pScene==NULL) return;
    int x, y,s;
    bool bDummy;
    int iState;
    RtsSceneBlockMap* pMap;
    if (GetPosition(point.x, point.y, x, y, bDummy, s))
    {
        if (bDummy) iState = 1;
        else        iState = 2;
    }else
    {
        iState = 0;
    }
    if (m_iSelectGridX!=x || m_iSelectGridY!=y || m_iSelectGridIsDummy!=iState)
    {
        CDC* pDC = GetDC(); // device context for painting
        CRect rtDraw;
        CPen penBlock (PS_SOLID, 1, RGB(  0,   0, 255));
        CPen penLoaded(PS_SOLID, 1, RGB(  0,  64, 255));
        CPen penDummy (PS_DOT,   1, RGB(255,   0,   0));
        CBrush brushBlock (RGB(128, 128, 255));
        CBrush brushLoaded(RGB(128, 196, 255));
        CBrush brushDummy (RGB(255, 128, 128));
        RtsSceneBlockMap* pBlockMap;
        if (m_iSelectGridIsDummy!=0)
        {
            GetRect(m_iSelectGridX, m_iSelectGridY, rtDraw,m_iScene);
            if (m_iSelectGridIsDummy==1)
            {
                pDC->SelectObject(penDummy);
                pDC->SelectObject(brushDummy);
            }else
            {
				if(!m_iScene)
                    pMap = g_pScene->GetBlockMap(m_iSelectGridX-g_pScene->GetCenterX(), m_iSelectGridY-g_pScene->GetCenterY());
				else
					pMap = g_pScene1->GetBlockMap(m_iSelectGridX-g_pScene1->GetCenterX(), m_iSelectGridY-g_pScene1->GetCenterY());
                if (pMap->m_pBlock)
                {
                    pDC->SelectObject(penLoaded);
                    pDC->SelectObject(brushLoaded);
                }else if (pMap->m_szFileName[0])
                {
                    pDC->SelectObject(penBlock);
                    pDC->SelectObject(brushBlock);
                }else
                {
                    pDC->SelectObject(penDummy);
                    pDC->SelectObject(brushDummy);
                }
            }
            pDC->Rectangle(&rtDraw);
        }

        m_iSelectGridX = x;
        m_iSelectGridY = y;
        m_iSelectGridIsDummy = iState;
		m_iScene = s;
        switch (iState)
        {
        case 1:
            sprintf(RtCoreBuffer2048(), "[%d,%d] 双击该场景块创建新块", m_iSelectGridX, m_iSelectGridY);
            break;
        case 2:
            if(!m_iScene)
				pBlockMap = g_pScene->GetBlockMap(m_iSelectGridX-g_pScene->GetCenterX(), m_iSelectGridY-g_pScene->GetCenterY());
			else
				pBlockMap = g_pScene1->GetBlockMap(m_iSelectGridX-g_pScene1->GetCenterX(), m_iSelectGridY-g_pScene1->GetCenterY());
            sprintf(RtCoreBuffer2048(), "[%d,%d] 场景 [%s] 文件 [%s]", m_iSelectGridX, m_iSelectGridY, pBlockMap->GetBlockName(), pBlockMap->GetFileName() );
            break;
        case 0:
            sprintf(RtCoreBuffer2048(), "没有选中任何场景块");
            break;
        }
        m_pDlgNewBlock->m_Attrib.SetWindowText(RtCoreBuffer2048());

        if (m_iSelectGridIsDummy!=0)
        {
            GetRect(m_iSelectGridX, m_iSelectGridY, rtDraw,m_iScene);
            CBrush brushBlockSel(RGB(158, 158, 255));
            CBrush brushDummySel(RGB(255, 158, 158));
            CBrush brushLoadedSel(RGB(158, 225, 255));
            if (m_iSelectGridIsDummy==1)
            {
                pDC->SelectObject(penDummy);
                pDC->SelectObject(brushDummySel);
            }else
            {
				if(!m_iScene)
                    pMap = g_pScene->GetBlockMap(m_iSelectGridX-g_pScene->GetCenterX(), m_iSelectGridY-g_pScene->GetCenterY());
				else
					pMap = g_pScene1->GetBlockMap(m_iSelectGridX-g_pScene1->GetCenterX(), m_iSelectGridY-g_pScene1->GetCenterY());
                if (pMap->m_pBlock)
                {
                    pDC->SelectObject(penLoaded);
                    pDC->SelectObject(brushLoadedSel);
                }else if (pMap->m_szFileName[0])
                {
                    pDC->SelectObject(penBlock);
                    pDC->SelectObject(brushBlock);
                }else
                {
                    pDC->SelectObject(penDummy);
                    pDC->SelectObject(brushDummySel);
                }
            }
            pDC->Rectangle(&rtDraw);
            pDC->Draw3dRect(rtDraw.left+1, rtDraw.bottom+1, rtDraw.Width()-2, -rtDraw.Height()-2, RGB(200, 200, 200), RGB(64, 64, 64));
        }
        ReleaseDC(pDC);
    }
    CWnd::OnMouseMove(nFlags, point);
}

void CNewBlockView::OnPaint()
{
    if (g_pScene==NULL) return;
    CPaintDC dc(this); // device context for painting
    CRect rtClient;
    CPen penBlock (PS_SOLID, 1, RGB(  0,   0, 255));
    CPen penLoaded(PS_SOLID, 1, RGB(  0,  6+4, 255));
    CPen penDummy (PS_DOT,   1, RGB(255,   0,   0));
    CBrush brushBlock (RGB(128, 128, 255));
    CBrush brushLoaded(RGB(128, 196, 255));
    CBrush brushDummy (RGB(255, 128, 128));
    int i, j;
    CRect rtDraw;
    RtsSceneBlockMap* pMap;

    this->GetClientRect(&rtClient);
    dc.FillRect(&rtClient, NULL);

    
    
  //  m_iDrawStartY1 = m_iDrawStartY;*/
    if (g_bDrawTop)
	{
		m_iDrawStartX = rtClient.right/2 - (m_iGridWidth+m_iGridSpaceBetween) * g_pScene->GetBlockCntX()/2;
		m_iDrawStartX = (m_iDrawStartX>20?m_iDrawStartX:20) - g_offsetX;
		//-g_offsetX;
		m_iDrawStartY = rtClient.bottom/2 + (m_iGridWidth+m_iGridSpaceBetween) * g_pScene->GetBlockCntY()/2;
		m_iDrawStartY = (m_iDrawStartY< rtClient.bottom-20 ? m_iDrawStartY:rtClient.bottom- 20) + g_offsetY;

		for (j=0; j<g_pScene->GetBlockCntY(); j++)
		{
			for (i=0; i<g_pScene->GetBlockCntX(); i++)
			{
				pMap = g_pScene->GetBlockMap(i-g_pScene->GetCenterX(), j-g_pScene->GetCenterY());
				if (pMap->m_pBlock)
				{
					dc.SelectObject(penLoaded);
					dc.SelectObject(brushLoaded);
				}else if (pMap->m_szFileName[0])
				{
					dc.SelectObject(penBlock);
					dc.SelectObject(brushBlock);
				}else
				{
					dc.SelectObject(penDummy);
					dc.SelectObject(brushDummy);
				}
				GetRect(i, j, rtDraw,1);
				dc.Rectangle(&rtDraw);
			}
		}
		dc.SelectObject(penDummy);
		dc.SelectObject(brushDummy);
		for (i=0; i<g_pScene->GetBlockCntX()+2; i++)
		{
			GetRect(i-1, -1, rtDraw,1);
			dc.Rectangle(&rtDraw);
			GetRect(i-1, g_pScene->GetBlockCntY(), rtDraw,1);
			dc.Rectangle(&rtDraw);
		}
		for (i=0; i<g_pScene->GetBlockCntY(); i++)
		{
			GetRect(-1, i, rtDraw,1);
			dc.Rectangle(&rtDraw);
			GetRect(g_pScene->GetBlockCntX(), i, rtDraw,1);
			dc.Rectangle(&rtDraw);
		}

    }
    
    // Do not call CWnd::OnPaint() for painting messages
	if( !g_bDrawTop && g_pScene1 )
	{
		m_iDrawStartX = rtClient.right/2 - (m_iGridWidth+m_iGridSpaceBetween) * g_pScene1->GetBlockCntX()/2;
		m_iDrawStartX = (m_iDrawStartX>20?m_iDrawStartX:20) - g_offsetX;
		//-g_offsetX;
		m_iDrawStartY = rtClient.bottom/2 + (m_iGridWidth+m_iGridSpaceBetween) * g_pScene1->GetBlockCntY()/2;
		m_iDrawStartY = (m_iDrawStartY< rtClient.bottom-20 ? m_iDrawStartY:rtClient.bottom- 20) + g_offsetY;
		
		  for (j=0; j<g_pScene1->GetBlockCntY(); j++)
		{
			for (i=0; i<g_pScene1->GetBlockCntX(); i++)
			{
				pMap = g_pScene1->GetBlockMap(i-g_pScene1->GetCenterX(), j-g_pScene1->GetCenterY());
				if (pMap->m_pBlock)
				{
					dc.SelectObject(penLoaded);
					dc.SelectObject(brushLoaded);
				}else if (pMap->m_szFileName[0])
				{
					dc.SelectObject(penBlock);
					dc.SelectObject(brushBlock);
				}else
				{
					dc.SelectObject(penDummy);
					dc.SelectObject(brushDummy);
				}
				GetRect(i, j, rtDraw,1);
				dc.Rectangle(&rtDraw);
			}
		}
		dc.SelectObject(penDummy);
		dc.SelectObject(brushDummy);
		for (i=0; i<g_pScene1->GetBlockCntX()+2; i++)
		{
			GetRect(i-1, -1, rtDraw,1);
			dc.Rectangle(&rtDraw);
			GetRect(i-1, g_pScene1->GetBlockCntY(), rtDraw,1);
			dc.Rectangle(&rtDraw);
		}
		for (i=0; i<g_pScene1->GetBlockCntY(); i++)
		{
			GetRect(-1, i, rtDraw,1);
			dc.Rectangle(&rtDraw);
			GetRect(g_pScene1->GetBlockCntX(), i, rtDraw,1);
			dc.Rectangle(&rtDraw);
		}

	}
}

void CDlgNewBlock::OnBnClickedButtonGenerateAllMinimap()
{
    RtGetRender()->EnableDrawPref(false);

    for (int j = 0; j < g_pScene->GetBlockCntY(); ++j)
        for (int i = 0; i < g_pScene->GetBlockCntX(); ++i)
            ((RtSceneTool*)g_pScene)->GenerateMiniMap(i - g_pScene->GetCenterX(), j - g_pScene->GetCenterY());

    MessageBox("生成小地图完成", "Done");
    RtGetRender()->EnableDrawPref(true);
}

void CDlgNewBlock::OnFileOpen()
{
	static char BASED_CODE szFilter[] = "RT Scene Files (*.rts)|*.rts|All Files (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "rts", ".rts", OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        //CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
		g_pSceneTool1 = new RtSceneTool;
        g_pScene1 = g_pSceneTool1;
        g_pScene1->Init();
        if (!((RtSceneTool*)g_pScene1)->ToolLoadScene(pathName,1))
        {
            //ERR1("Can't load height \"%s\"\n", pathName);
        }
		//g_pScene->OnMoveTo();
    }
}

//void CNewBlockView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//     CRect rtClient;
//	 GetClientRect(rtClient);
//
//	if (nSBCode == SB_TOP)
//	{
//
//		 m_iDrawStartY = rtClient.bottom/2 - 
//			(m_iGridWidth+m_iGridSpaceBetween) * g_pScene->GetBlockCntY()/2;
//
//		 m_iDrawStartY1 = 0;
//		 
//	}
//
//	if (nSBCode == SB_BOTTOM)
//	{
//         m_iDrawStartY = rtClient.bottom/2 - 
//			(m_iGridWidth+m_iGridSpaceBetween) * g_pScene1->GetBlockCntY()/2;
//
//		 m_iDrawStartY1 = 1;
//	}
//
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
//}

//void CNewBlockView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
//}

void CDlgNewBlock::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nSBCode == SB_THUMBPOSITION) 
	{
		g_offsetY = nPos/100.0f *500;
		nCurVPos = nPos;
	}
	m_vscroll.SetScrollPos(nCurVPos);
	m_pView->Invalidate(true);
	m_pView->UpdateWindow();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDlgNewBlock::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode == SB_THUMBPOSITION) 
	{
		g_offsetX = nPos/100.0f *400;
		nCurHPos = nPos;
	}
	
    m_hscroll.SetScrollPos(nCurHPos);
	m_pView->Invalidate(true);
	m_pView->UpdateWindow();
	//Invalidate(true);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgNewBlock::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	g_bDrawTop = false;

	m_pView->Invalidate(true);
	m_pView->UpdateWindow();
}

void CDlgNewBlock::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	g_bDrawTop = true;

	m_pView->Invalidate(true);
	m_pView->UpdateWindow();

}
