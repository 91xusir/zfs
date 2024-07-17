// DlgHeightMap.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "DlgHeightMap.h"
#include ".\dlgheightmap.h"


// CDlgHeightMap dialog

IMPLEMENT_DYNAMIC(CDlgHeightMap, CDialog)
CDlgHeightMap::CDlgHeightMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeightMap::IDD, pParent)
{
    m_pImage = NULL;
}

CDlgHeightMap::~CDlgHeightMap()
{
}

void CDlgHeightMap::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_HEIGHTMAP_VIEW_IMG, m_imgHeightmap);
    DDX_Control(pDX, IDC_HEIGHTMAP_IMG_RECT, m_imgRectWnd);
}


BEGIN_MESSAGE_MAP(CDlgHeightMap, CDialog)
END_MESSAGE_MAP()


// CDlgHeightMap message handlers

BOOL CDlgHeightMap::OnInitDialog()
{
    CDialog::OnInitDialog();
    if (m_pImage!=NULL) return TRUE;

    CRect rt, wrt;
    m_imgRectWnd.GetClientRect(&rt);
    m_imgRectWnd.ClientToScreen(&rt);
    GetClientRect(&wrt);
    ClientToScreen(&wrt);
    rt.right -= rt.left;
    rt.bottom -= rt.top;
    rt.left = rt.left-wrt.left;
    rt.top = rt.top-wrt.top;
    rt.right += rt.left;
    rt.bottom += rt.top;

    HDC hDC = ::GetDC(this->m_hWnd);
    HBITMAP hBITMAP = (HBITMAP)(((RtgImage*)m_pImage)->CreateBitmap(hDC));
    m_imgHeightmap.Create(_T("my static"),
        WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE,
        rt, this);
    m_imgHeightmap.SetBitmap(hBITMAP);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
