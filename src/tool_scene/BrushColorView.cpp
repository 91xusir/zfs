
#include "stdafx.h"
#include "tool_scene.h"
#include "BrushColorView.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrushColorForm

IMPLEMENT_DYNCREATE(CBrushColorForm, CDialog)

CBrushColorForm::CBrushColorForm()
	: CDialog(CBrushColorForm::IDD)
{
	//{{AFX_DATA_INIT(CBrushColorForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBrushColorForm::~CBrushColorForm()
{
}

void CBrushColorForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushColorForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushColorForm, CDialog)
	//{{AFX_MSG_MAP(CBrushColorForm)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrushColorForm diagnostics

#ifdef _DEBUG
void CBrushColorForm::AssertValid() const
{
	CDialog::AssertValid();
}

void CBrushColorForm::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrushColorForm message handlers

void CBrushColorForm::OnOK()
{
}

void CBrushColorForm::OnCancel()
{
}

BOOL CBrushColorForm::PreCreateWindow(CREATESTRUCT& cs) 
{	
	
	return CDialog::PreCreateWindow(cs);
}

void CBrushColorForm::OnPaint() 
{
	CPaintDC dc(this);
	CRect rectClient;
	GetClientRect(&rectClient);
	CXTPBufferDC memDC(dc, rectClient);
	memDC.FillSolidRect(rectClient, GetSysColor(COLOR_3DFACE));
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
}

BOOL CBrushColorForm::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
