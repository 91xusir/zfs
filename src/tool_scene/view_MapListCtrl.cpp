// view_MapListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "view_MapListCtrl.h"


// CMapListCtrl

IMPLEMENT_DYNAMIC(CMapListCtrl, CListCtrl)
CMapListCtrl::CMapListCtrl()
{
}

CMapListCtrl::~CMapListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMapListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CMapListCtrl message handlers
LRESULT CMapListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            CPaintDC dc(this);
            CRect r;
            GetClientRect(&r);
            CXTPBufferDC memDC(dc, r);
            memDC.FillSolidRect(r, GetSysColor(COLOR_WINDOW));
            return CListCtrl::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
        }
    case WM_ERASEBKGND:
        {
            return TRUE;
        }
    }
    return CListCtrl::WindowProc(message, wParam, lParam);
}

void CMapListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    ::Rectangle(lpDrawItemStruct->hDC,
        lpDrawItemStruct->rcItem.left,
        lpDrawItemStruct->rcItem.top,
        lpDrawItemStruct->rcItem.right,
        lpDrawItemStruct->rcItem.bottom);
}
