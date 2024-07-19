// GrassTextWnd.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "BrushGrassForm.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "tool_scene.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "GrassTextWnd.h"
#include "windowsx.h"


// GrassTextWnd

IMPLEMENT_DYNAMIC(GrassTextWnd, CWnd)

GrassTextWnd::GrassTextWnd()
    : m_hBitmap(0),
      m_subX(1),
      m_subY(1),
      m_curX(0),
      m_curY(0)
{

}

GrassTextWnd::~GrassTextWnd()
{
    if (m_hBitmap)
        DeleteObject(m_hBitmap);
}


BEGIN_MESSAGE_MAP(GrassTextWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

float GrassTextWnd::GetAspectW2H()
{
    if (!m_hBitmap)
        return 1.f;

    BITMAP bm;
    GetObject(m_hBitmap, sizeof(bm), &bm);

    return ((float)bm.bmWidth / m_subX) / ((float)bm.bmHeight / m_subY);

}

void GrassTextWnd::SetText(const char* fileName, int sx, int sy)
{
    RTASSERT(fileName);
    RtgImage _Img;

    char filePath[MAX_PATH];
	// Modified by Wayne Wong 2010-12-28
    //sprintf(filePath, "scene\\texture\\%s", fileName);
	sprintf(filePath, "%s", fileName);

    if (!_Img.Load(filePath))
        return;

    m_hBitmap = CreateBitmap(_Img.info.iWidth, _Img.info.iHeight, 1, 32, _Img.GetRawData());

    BITMAP bm;
    GetObject(m_hBitmap, sizeof(bm), &bm);
    void* s = malloc(bm.bmWidthBytes * bm.bmHeight);
    void* d = malloc(bm.bmWidthBytes * bm.bmHeight);
    GetBitmapBits(m_hBitmap, bm.bmWidthBytes * bm.bmHeight, s);
    if (bm.bmBitsPixel == 32)
    {
        for (int y = 0; y < bm.bmHeight; ++y)
        {
            RGBQUAD* dst = (RGBQUAD*)&((byte*)d)[bm.bmWidthBytes * y];
            RGBQUAD* src = (RGBQUAD*)&((byte*)s)[bm.bmWidthBytes * y];

            for (int x = 0; x < bm.bmWidth; ++x)
            {
                float a = (float)src[x].rgbReserved / 255.f;
                dst[x].rgbBlue = (int)((float)src[x].rgbRed * a) + (1.f - a) * 200.f;
                dst[x].rgbGreen = (int)((float)src[x].rgbGreen * a) + (1.f - a) * 200.f;
                dst[x].rgbRed = (int)((float)src[x].rgbBlue * a) + (1.f - a) * 200.f;
            }
        }
    }
    else
    {
        for (int i = 0; i < bm.bmHeight; ++i)
            memcpy(&((byte*)d)[bm.bmWidthBytes * i], &((byte*)s)[bm.bmWidthBytes * (bm.bmHeight - 1 - i)], bm.bmWidthBytes);
    }
    SetBitmapBits(m_hBitmap, bm.bmWidthBytes * bm.bmHeight, d);
    free(s);
    free(d);

    m_subX = sx;
    m_subY = sy;
    m_curX = 0;
    m_curY = 0;

    InvalidateRect(NULL, FALSE);
}



void GrassTextWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    RECT rc;
    GetClientRect(&rc);

    BITMAP bm;
    GetObject(m_hBitmap, sizeof(bm), &bm);

    int w = rc.right;
    int h = rc.bottom;

    if (bm.bmWidth > bm.bmHeight)
        h = (int)w * ((float)bm.bmHeight / (float)bm.bmWidth);
    else
        w = (int)h * ((float)bm.bmWidth / (float)bm.bmHeight);

    CRect rcImge = CRect(0, rc.bottom - h, w, rc.bottom);

    if (rcImge.PtInRect(point))
    {
        m_curX = point.x / (w / m_subX);
        m_curY = (point.y - (rc.bottom - h)) / (h / m_subY);

        InvalidateRect(NULL, FALSE);

        CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
        pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_GRASS);
        pFrame->m_BrushTool.SetBrushData((long)(LONG_PTR)this->GetParent());
        
    }


}

void GrassTextWnd::OnPaint()
{
    CPaintDC dc(this);



    RECT rc;
    GetClientRect(&rc);

    dc.FillSolidRect(rc.left, rc.top, rc.right, rc.bottom, RGB(200, 200, 200));

    if (!m_hBitmap)
        return;

    CDC memDc;
    memDc.CreateCompatibleDC(&dc);
    memDc.SelectObject(m_hBitmap);




    BITMAP bm;
    GetObject(m_hBitmap, sizeof(bm), &bm);

    int w = rc.right;
    int h = rc.bottom;

    if (bm.bmWidth > bm.bmHeight)
        h = (int)w * ((float)bm.bmHeight / (float)bm.bmWidth);
    else
        w = (int)h * ((float)bm.bmWidth / (float)bm.bmHeight);

    dc.StretchBlt(0, rc.bottom - h, w, h, &memDc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);


    CRect rcItem = CRect(
        m_curX * w / m_subX,
        rc.bottom - (m_subY - m_curY)* h / m_subY,
        (m_curX + 1) * w / m_subX,
        rc.bottom - (m_subY - m_curY - 1) * h / m_subY
        );

    CPen pen;
    pen.CreatePen(PS_SOLID, 2, RGB(0,128,255));
    dc.SelectObject(&pen);
    dc.MoveTo(rcItem.left + 1, rcItem.top + 1);
    dc.LineTo(rcItem.right - 4, rcItem.top + 1);
    dc.LineTo(rcItem.right - 4, rcItem.bottom - 1);
    dc.LineTo(rcItem.left + 1, rcItem.bottom - 1);
    dc.LineTo(rcItem.left + 1, rcItem.top);


}
// GrassTextWnd message handlers


