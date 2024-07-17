// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "ListCtrlEx.h"
#include "windowsx.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
    VERIFY(m_Font.CreateFont(
        15,                        // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        FW_NORMAL,                 // nWeight
        FALSE,                     // bItalic
        FALSE,                     // bUnderline
        0,                         // cStrikeOut
        ANSI_CHARSET,              // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        DEFAULT_QUALITY,           // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        "Arial"));                 // lpszFacename

}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
//    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlEx::OnNMCustomdraw)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
//ON_NOTIFY_REFLECT(NM_CLICK, &CListCtrlEx::OnNMClick)
//ON_NOTIFY(HDN_ITEMCLICKA, 0, &CListCtrlEx::OnHdnItemclick)
//ON_NOTIFY(HDN_ITEMCLICKW, 0, &CListCtrlEx::OnHdnItemclick)
//ON_NOTIFY_REFLECT(LVN_KEYDOWN, &CListCtrlEx::OnLvnKeydown)
//ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, &CListCtrlEx::OnLvnItemActivate)
//ON_NOTIFY_REFLECT(LVN_HOTTRACK, &CListCtrlEx::OnLvnHotTrack)
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CListCtrlEx message handlers



//void CListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

void CListCtrlEx::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    CDC bufDc;

    CRect rc;
    GetClientRect(&rc);

    if(!bufDc.CreateCompatibleDC( &dc ))
        return;

    CBitmap bufBm;
    bufBm.CreateCompatibleBitmap(&dc, rc.right - rc.left, rc.bottom - rc.top);

    CBitmap* OldBufBm = bufDc.SelectObject(&bufBm);


    bufDc.FillRect(&rc, CBrush::FromHandle(GetStockBrush(WHITE_BRUSH)));
   
    int n = GetItemCount();
    
    for (int i = 0; i < n; ++i)
    {
        LVITEM it;
        char text[128];
        CRect rcItem;
        CRect rcImage;

        ZeroMemory(&it, sizeof(it));
        it.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;
        it.cchTextMax = 128;
        it.pszText = text;
        it.iItem = i;
        it.iSubItem = 0;
        it.stateMask = LVIS_SELECTED;
        GetItem(&it);
        GetItemRect(i, &rcItem, LVIR_BOUNDS);
        GetItemRect(i, &rcImage, LVIR_ICON);

        CDC memDC;

        if(!memDC.CreateCompatibleDC( &dc ))
            return;

        CBitmap* OldBitmap = memDC.SelectObject(CBitmap::FromHandle(m_vBitmap[i]));
        BITMAP bm;
        ::GetObject(m_vBitmap[i], sizeof(BITMAP), &bm);
        bufDc.StretchBlt(rcImage.left, rcImage.top, rcImage.right - rcImage.left, rcImage.bottom - rcImage.top, &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        memDC.SelectObject(OldBitmap);

        CFont* OldFont = bufDc.SelectObject(&m_Font);
        CRect rcText;
        GetItemRect(i, &rcText, LVIR_LABEL);

        bufDc.SetTextColor(RGB(255,255,255));
        if (it.state & LVIS_SELECTED)
        {
            bufDc.SetTextColor(RGB(0,128,255));
            CPen pen;
            pen.CreatePen(PS_SOLID, 2, RGB(0,128,255));
            bufDc.SelectObject(&pen);
            CRect rcFrame;
            rcFrame.left = rcItem.left - 2;
            rcFrame.top = rcItem.top - 2;
            rcFrame.right = rcItem.right + 2;
            rcFrame.bottom = rcItem.bottom + 2;
            bufDc.MoveTo(rcFrame.left, rcFrame.top);
            bufDc.LineTo(rcFrame.right, rcFrame.top);
            bufDc.LineTo(rcFrame.right, rcFrame.bottom);
            bufDc.LineTo(rcFrame.left, rcFrame.bottom);
            bufDc.LineTo(rcFrame.left, rcFrame.top - 1);
        }
        else
        {
            bufDc.SetTextColor(RGB(100,100,100));
        }
        bufDc.DrawText(text, -1, &rcText, DT_CENTER | DT_SINGLELINE);

        bufDc.SelectObject(OldFont);
    }

    dc.BitBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, &bufDc, 0, 0, SRCCOPY);
    bufDc.SelectObject(OldBufBm);
}


BOOL CListCtrlEx::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

//void CListCtrlEx::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//
//
//    *pResult = 0;
//}

//void CListCtrlEx::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

//void CListCtrlEx::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

//void CListCtrlEx::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

//void CListCtrlEx::OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
    POSITION pos = GetFirstSelectedItemPosition();

    if (pos)
    {
        int it = GetNextSelectedItem(pos);
        CRect rc;

        GetItemRect(it, &rc, LVIR_BOUNDS);

        rc.left -= 4;
        rc.top -= 4;
        rc.right += 4;
        rc.bottom += 4;

        InvalidateRect(&rc);


    }

    CListCtrl::OnLButtonDown(nFlags, point);

    pos = GetFirstSelectedItemPosition();

    if (pos)
    {
        int it = GetNextSelectedItem(pos);
        CRect rc;

        GetItemRect(it, &rc, LVIR_BOUNDS);

        rc.left -= 4;
        rc.top -= 4;
        rc.right += 4;
        rc.bottom += 4;

        InvalidateRect(&rc);


    }
}
