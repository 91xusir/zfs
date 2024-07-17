     
#include "stdafx.h"
#include "tool_scene.h"
#include "BrushVertexView.h"

#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include ".\brushvertexview.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif
/////////////////////////////////////////////////////////////////////////////
// CBrushVertexForm

CBrushVertexForm*   gpkTerrainBrush = NULL;

IMPLEMENT_DYNCREATE(CBrushVertexForm, CDialog)

CBrushVertexForm::CBrushVertexForm()
	: CDialog(CBrushVertexForm::IDD)
    , m_iBrushShape(0)
    , m_bCheckSnapToGrid(FALSE)
    , m_bEditTile(TRUE)
    , m_bEditTerrain(TRUE)
    , m_iSelectWater(0)
	, m_bFadeInFadeOut( TRUE )	
	, m_pkObject( NULL )
{	
	//{{AFX_DATA_INIT(CBrushVertexForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBrushVertexForm::~CBrushVertexForm()
{
	gpkTerrainBrush = NULL;
}

void CBrushVertexForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushVertexForm)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SLIDER_MIN, m_minBrush);
	DDX_Control(pDX, IDC_SLIDER_MAX, m_maxBrush);
	DDX_Check(pDX, IDC_CHECK_SNAP_TO_GRID, m_bCheckSnapToGrid);
	DDX_Check(pDX, IDC_CHECK_TILE, m_bEditTile);
	DDX_Check(pDX, IDC_CHECK_TERRAIN, m_bEditTerrain);
	DDX_CBIndex(pDX, IDC_COMBO_WATER, m_iSelectWater);
	DDX_Control(pDX, IDC_COMBO_WATER, m_SelectWaterCtrl);
	DDX_Check(pDX, IDC_CHECK_FADEOUTIN, m_bFadeInFadeOut);
}


BEGIN_MESSAGE_MAP(CBrushVertexForm, CDialog)
	//{{AFX_MSG_MAP(CBrushVertexForm)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_RADIO_RECTANGLE, OnBnClickedRadioRectangle)
    ON_BN_CLICKED(IDC_RADIO_ROUNDED, OnBnClickedRadioRounded)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_VERTEX_DRAG, OnBnClickedRadioBrushVertexDrag)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_VERTEX_BRUSH, OnBnClickedRadioBrushVertexBrush)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_VERTEX_NOISE, OnBnClickedRadioBrushVertexNoise)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_VERTEX_SMOOTH, OnBnClickedRadioBrushVertexSmooth)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_TERRAIN_VERTEX, OnBnClickedRadioBrushTerrainVertex)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_TERRAIN_WALK, OnBnClickedRadioBrushTerrainWalk)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_TERRAIN_FLY, OnBnClickedRadioBrushTerrainFly)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_TERRAIN_DIG, OnBnClickedRadioBrushTerrainDig)
    ON_BN_CLICKED(IDC_CHECK_SNAP_TO_GRID, OnBnClickedCheckSnapToGrid)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_TERRAIN_WATER, OnBnClickedRadioBrushTerrainWater)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_POINT_TILE, OnBnClickedRadioBrushPointTile)
    ON_BN_CLICKED(IDC_CHECK_TILE, OnBnClickedCheckTile)
    ON_BN_CLICKED(IDC_CHECK_TERRAIN, OnBnClickedCheckTerrain)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_TILE_VERTEX, OnBnClickedRadioBrushTileVertex)
    ON_BN_CLICKED(IDC_RADIO_BRUSH_VERTEX_DRAG2, OnBnClickedRadioBrushVertexDrag2)
	ON_BN_CLICKED(IDC_CHECK_FADEOUTIN, OnBnClickedCheckFadeoutin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrushVertexForm diagnostics

#ifdef _DEBUG
void CBrushVertexForm::AssertValid() const
{
	CDialog::AssertValid();
}

void CBrushVertexForm::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrushVertexForm message handlers

void CBrushVertexForm::OnOK()
{
}

void CBrushVertexForm::OnCancel()
{
}

BOOL CBrushVertexForm::PreCreateWindow(CREATESTRUCT& cs) 
{	
	return CDialog::PreCreateWindow(cs);
}

void CBrushVertexForm::OnPaint() 
{
	CPaintDC dc(this);
	CRect rectClient;
	GetClientRect(&rectClient);
	CXTPBufferDC memDC(dc, rectClient);
	memDC.FillSolidRect(rectClient, GetSysColor(COLOR_3DFACE));
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
}

BOOL CBrushVertexForm::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CBrushVertexForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	funguard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;
    if (pSliderCtrl==&m_maxBrush)
    {
        if (m_maxBrush.GetPos()<m_minBrush.GetPos())
        {
            m_minBrush.SetPos(m_maxBrush.GetPos());
        }
    }else if (pSliderCtrl==&m_minBrush)
    {
        if (m_minBrush.GetPos()>m_maxBrush.GetPos())
        {
            m_maxBrush.SetPos(m_minBrush.GetPos());
        }
    }else
    {
        return;
    }
    pEditor->m_BrushTool.m_Drawer.SetSize2(m_maxBrush.GetPos(), m_minBrush.GetPos());
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioRectangle()
{
    funguard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    pEditor->m_BrushTool.m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioRounded()
{
    funguard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    pEditor->m_BrushTool.m_Drawer.SetType(CDrawerBrush::DB_ROUND);
    fununguard;
}

// 变成拉高拉低的地形刷子 [方]
void CBrushVertexForm::OnBnClickedRadioBrushVertexDrag()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_VERTEX);
    pFrame->m_BrushTool.SetBrushData(1, 0);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

// 变成拉高拉低的地形刷子 [圆]
void CBrushVertexForm::OnBnClickedRadioBrushVertexDrag2()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_VERTEX);
    pFrame->m_BrushTool.SetBrushData(1, 4);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

// 变成刷高(低)的地形刷子
void CBrushVertexForm::OnBnClickedRadioBrushVertexBrush()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_VERTEX);
    pFrame->m_BrushTool.SetBrushData(1, 1);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

// 使地形变崎岖
void CBrushVertexForm::OnBnClickedRadioBrushVertexNoise()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_VERTEX);
    pFrame->m_BrushTool.SetBrushData(1, 2);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

// 使地形变平滑
void CBrushVertexForm::OnBnClickedRadioBrushVertexSmooth()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_VERTEX);
    pFrame->m_BrushTool.SetBrushData(1, 3);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushTerrainVertex()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_ATTR_VERTEX);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushTerrainWalk()
{
    funguard;
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_ATTR_WALK);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushTerrainFly()
{
    funguard;
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_ATTR_FLY);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushTerrainDig()
{
    funguard;
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_ATTR_DIG);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushTerrainWater()
{
    funguard;
    this->UpdateData(TRUE);
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_WATER);
    pFrame->m_BrushTool.SetBrushData(m_iSelectWater+1);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushPointTile()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_POING);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

void CBrushVertexForm::OnBnClickedRadioBrushTileVertex()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_TILE_VERTEX);
    pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    fununguard;
}

void CBrushVertexForm::OnBnClickedCheckSnapToGrid()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    pFrame->m_bCheckSnapToGrid = m_bCheckSnapToGrid;
    fununguard;
}

void CBrushVertexForm::OnBnClickedCheckTile()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    CBrushTool::EBrushType eType = pFrame->m_BrushTool.GetBrushType();
    if (eType==CBrushTool::BT_VERTEX || eType==CBrushTool::BT_POING || eType==CBrushTool::BT_TILE_VERTEX)
    {
        pFrame->m_BrushTool.SetBrushData(2, m_bEditTile);
    }
    fununguard;
}

void CBrushVertexForm::OnBnClickedCheckTerrain()
{
    funguard;
    this->UpdateData();
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    CBrushTool::EBrushType eType = pFrame->m_BrushTool.GetBrushType();
    if (eType==CBrushTool::BT_VERTEX || eType==CBrushTool::BT_POING || eType==CBrushTool::BT_TILE_VERTEX)
    {
        pFrame->m_BrushTool.SetBrushData(3, m_bEditTerrain);
    }
    fununguard;
}

BOOL CBrushVertexForm::OnInitDialog()
{
    CDialog::OnInitDialog();

    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    for (int i=0; i<RtScene::MAX_WATER_COUNT; i++)
    {
        if (!g_pScene->m_Waters[i].szFile.IsEmpty())
        {
            if (g_pScene->m_Waters[i].szName.IsEmpty())
            {
                m_SelectWaterCtrl.InsertString(-1, "没有名字的");
            }else
            {
                m_SelectWaterCtrl.InsertString(-1, g_pScene->m_Waters[i].szName);
            }
            m_iSelectWater = 0;
        }
    }
    m_minBrush.SetRange(1, 10);
    m_maxBrush.SetRange(1, 10);
    UpdateData(FALSE);
	gpkTerrainBrush = this;
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CBrushVertexForm::OnBnClickedCheckFadeoutin()
{
	UpdateData( TRUE );
    // done
	/*if( m_pkObject )
	{
		m_pkObject->m_bFadeOutFadeIn = m_bFadeInFadeOut;
	}*/
	// TODO: 在此添加控件通知处理程序代码
}

void CBrushVertexForm::SetFadeOutFadeIn( bool bFadeOutIn )
{
    m_bFadeInFadeOut = bFadeOutIn;
	UpdateData( FALSE );
}
void CBrushVertexForm::SetOperatorObject( RtsObject* pkObject )
{
    m_pkObject = pkObject;
}
