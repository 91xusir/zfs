#include "rtg_graph_inter.h"
#include "graph/rtg_device_2d.h"

namespace rt_graph {

#define EL2DGETA(c)                 ((((c)&0xFF000000)>>24)&0x00FF)
#define EL2DGETR(c)                 ((((c)&0x00FF0000)>>16)&0x00FF)
#define EL2DGETG(c)                 ((((c)&0x0000FF00)>>8)&0x00FF)
#define EL2DGETB(c)                 ((c)&0x000000FF)
#define EL2DGETC(r, g, b)           (((((r)>255?255:((r)<0?0:(r)))&0x00FF)<<16)|((((g)>255?255:((g)<0?0:(g)))&0x00FF)<<8) | (((b)>255?255:((b)<0?0:(b)))&0x00FF))
#define EL2DGETCA(a, r, g, b)       (((((a)>255?255:((a)<0?0:(a)))&0xFF)<<24)|((((r)>255?255:((r)<0?0:(r)))&0x00FF)<<16)|((((g)>255?255:((g)<0?0:(g)))&0x00FF)<<8) | (((b)>255?255:((b)<0?0:(b)))&0x00FF))
#define EL2DGETRGB(c, r, g, b)      {(r)=EL2DGETR(c);(g)=EL2DGETG(c);(b)=EL2DGETB(c);}
#define EL2DGETARGB(c, a, r, g, b)  {(a)=EL2DGETA(c);(r)=EL2DGETR(c);(g)=EL2DGETG(c);(b)=EL2DGETB(c);}
#define EL2DALPHA(s, d, a)          (((s)*(a) + (d)*(255-(a)))>>8)

RtgDevice2D::RtgDevice2D()
{
    m_bImageChanged = false;

    m_bMemCreated = FALSE;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iDCWidth = 0;
    m_iDCHeight = 0;
    m_hMemDC = NULL;
    m_hMemBitmap = NULL;
    m_hOldBitmap = NULL;
#ifdef	USER_DRAW_DIB
    m_hDrawDib = NULL;
#endif
    m_lpvBits = NULL;
    m_pFeatherPestBuffer = NULL;

    m_pPen = NULL;
    m_lPenRadiusMem = 0;
    m_lPenRadiusMin = 0;
    m_lPenRadiusMax = 0;
}

RtgDevice2D::~RtgDevice2D()
{
    ClearMemDC();
#ifdef	USER_DRAW_DIB
    if(m_hDrawDib)
    {
        DrawDibClose(m_hDrawDib);
        m_hDrawDib = FALSE;
    }
#endif
    if (m_pFeatherPestBuffer)
    {
        RtCoreMem().Free(m_pFeatherPestBuffer);
        m_pFeatherPestBuffer = NULL;
    }
    m_iWidth = 0;
    m_iHeight = 0;
}

void RtgDevice2D::ClearMemDC()
{
    if (m_bMemCreated)
    {
        DeleteObject(m_hMemBitmap);
        DeleteDC(m_hMemDC);
        m_hMemDC = NULL;
        m_hMemBitmap = NULL;
        m_hOldBitmap = NULL;
        m_bMemCreated = FALSE;
    }
}

void RtgDevice2D::UpdateMemDC(HDC hCompatibleDC)
{
    if (m_iDCWidth!=m_iWidth || m_iDCHeight!=m_iHeight)
    {
        ClearMemDC();

        m_iDCWidth = m_iWidth;
        m_iDCHeight = m_iHeight;
        m_hMemDC = CreateCompatibleDC(hCompatibleDC);
        m_hMemBitmap = CreateCompatibleBitmap(hCompatibleDC, m_iWidth, m_iHeight);
    }
    m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hMemBitmap);

#ifndef	USER_DRAW_DIB
    int iResult = StretchDIBits( m_hMemDC,
        0, 0, m_iWidth, m_iHeight,
        0, 0, m_iWidth, m_iHeight,
        m_lpvBits,
        (LPBITMAPINFO)&m_bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
        );
    CHECK(iResult!=GDI_ERROR);
#else
    /*
    if( !m_Turn )	// 如果不是翻转的图像
    {
        m_Turn = !m_Turn ;

        WORD	tempcolor, *pData;
        int		offset1,offset2;
        long	hImage, wImage ;

        hImage = ((LPBITMAPINFOHEADER)m_pImage)->biHeight ;
        wImage = ((LPBITMAPINFOHEADER)m_pImage)->biWidth ;
        pData  = (WORD*)(m_pImage + sizeof(BITMAPINFOHEADER)) ;

        for(long i=0;i<hImage/2;i++)
        {
            offset1=i*wImage;
            offset2=(hImage-i-1)*wImage;
            for(long j=0;j<wImage;j++)
            {
                tempcolor=pData[offset1+j];
                pData[offset1+j]=pData[offset2+j];
                pData[offset2+j]=tempcolor;
            }
        }
    }
    */
    DrawDibDraw(m_hDrawDib, m_hMemDC,
        0, 0, m_iWidth, m_iHeight,
        (LPBITMAPINFOHEADER)&m_bitmapInfo,
        m_lpvBits,
        0, 0, m_iWidth, m_iHeight,
        DDF_HALFTONE
        );
#endif
    SelectObject(m_hMemDC, m_hOldBitmap);
    m_hOldBitmap = NULL;

    m_bMemCreated = TRUE;
    m_bImageChanged = false;
}

void RtgDevice2D::BltToDC(HDC hDestDC, int x, int y, int lCopyW, int lCopyH)
{
    if (hDestDC==NULL) return;

    if (m_bImageChanged)
    {
        UpdateMemDC(hDestDC);
    }

    if(lCopyW==0) lCopyW = m_iWidth;
    if(lCopyH==0) lCopyH = m_iHeight;

    m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hMemBitmap);
    if (!BitBlt(hDestDC, x, y, lCopyW, lCopyH, m_hMemDC, x, y,SRCCOPY))
    {
        MessageBeep(0);
    }
    SelectObject(m_hMemDC, m_hOldBitmap);
    m_hOldBitmap = NULL;
}

void RtgDevice2D::BltToDC(HDC hDestDC, int iDx, int iDy, int iSx, int iSy, int iSw, int iSh)
{
    if (hDestDC==NULL) return;

    if (m_bImageChanged)
    {
        UpdateMemDC(hDestDC);
    }

    if(iSw==0) iSw = m_iWidth;
    if(iSh==0) iSh = m_iHeight;
    m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hMemBitmap);
    if (!BitBlt(hDestDC, iDx, iDy, iSw, iSh, m_hMemDC, iSx, iSy,SRCCOPY))
    {
        MessageBeep(0);
    }
    SelectObject(m_hMemDC, m_hOldBitmap);
    m_hOldBitmap = NULL;
}

void RtgDevice2D::ChangeRect(int x, int y, int w, int h)
{
    if (!m_bImageChanged)
    {
        m_rtChanged.left = x;
        m_rtChanged.top = y;
        m_rtChanged.right = x+w;
        m_rtChanged.bottom = y+h;
    }else
    {
        if (m_rtChanged.left>x)  m_rtChanged.left = x;
        if (m_rtChanged.top>y)   m_rtChanged.top = y;
        if (m_rtChanged.right<x+w) m_rtChanged.right = x+w;
        if (m_rtChanged.bottom<y+h) m_rtChanged.bottom = y+h;
    }
    m_bImageChanged = true;
}

void RtgDevice2D::GetChangedRect(RECT& rt)
{
    rt = m_rtChanged;
    if (rt.left<0)              rt.left = 0;
    if (rt.top<0)               rt.top = 0;
    if (rt.right>m_iWidth)      rt.right = m_iWidth;
    if (rt.bottom>m_iHeight)    rt.bottom = m_iHeight;
}

void RtgDevice2D::EraseBackground(DWORD dwEraseColor)
{
    if (m_lpvBits==NULL) return;

    int i, iSize = m_iWidth*m_iHeight;
    unsigned long* p = (unsigned long*)m_lpvBits;
    for (i=0; i<iSize; i++)
    {
        *(p++) = dwEraseColor;
    }
    ChangeRect(0, 0, m_iWidth, m_iHeight);
}

void RtgDevice2D::FillRectangle(DWORD dwColor, int x, int y, int w, int h, EAlphaType eAlphaType,float fAlpha)
{
    mem32BlitFill(m_iWidth, m_iHeight, (DWORD*)m_lpvBits, x, y, w, h, dwColor, eAlphaType, fAlpha);
    ChangeRect(x, y, w, h);
}

void RtgDevice2D::FillImage(long lDesX,long lDesY, long lCopyW,long lCopyH,DWORD* lpSrc, long lSrcW,long lSrcH, long lSrcX,long lSrcY,EAlphaType eAlphaType,float fAlpha)
{
    mem32BlitBitmap(lSrcW, lSrcH, m_iWidth, m_iHeight, lpSrc, (DWORD*)m_lpvBits, lSrcX, lSrcY, lDesX, lDesY, lCopyW, lCopyH, eAlphaType, fAlpha);
    ChangeRect(lDesX, lDesY, lCopyW, lCopyH);
}

void RtgDevice2D::FillImage(long lDesX,long lDesY, long lCopyW,long lCopyH, RtgDevice2D* pSrc, long lSrcX,long lSrcY,EAlphaType eAlphaType,float fAlpha)
{
    FillImage(lDesX,lDesY,lCopyW,lCopyH, (DWORD*)(pSrc->m_lpvBits),pSrc->m_iWidth,pSrc->m_iHeight,lSrcX,lSrcY,eAlphaType,fAlpha);
}

// 要求Src的图里面表示的是Alpha
// lFeatherRadius 为羽化半径
// dwDColor 为目标颜色
//  请注意：这个函数没有对数据进行严格的检查，如果两个图的大小不一样，会导致出错(不检查是因为目前懒得写)
void RtgDevice2D::FeatherPest(long lDesX,long lDesY, long lCopyW,long lCopyH, RtgDevice2D* pSrcImage, long lFeatherRadius, DWORD dwDColor, long lSrcX,long lSrcY,EAlphaType eAlphaType,float fAlpha)
{
    DWORD *pSrc = (DWORD*)pSrcImage->m_lpvBits;
    DWORD *pDest = (DWORD*)m_lpvBits;

    if (m_pFeatherPestBuffer==0)
    {
        m_pFeatherPestBuffer = (char*)RtCoreMem().Alloc(m_iWidth*m_iHeight, __FILE__, __LINE__);
        memset(m_pFeatherPestBuffer, 0, m_iWidth*m_iHeight);
    }

    long x, y;
    long xSS, ySS, xSE, ySE;
    long xDS, yDS, xDE, yDE;
    DWORD cDR, cDG, cDB, cSR, cSG, cSB;
    float fC, fAC;
    DWORD *pS, *pD;
    char* pF;

    xSS = lSrcX;
    ySS = lSrcY;
    xSE = lSrcX+lCopyW;
    ySE = lSrcY+lCopyH;
    xDS = lDesX;
    yDS = lDesY;
    xDE = lDesX+lCopyW;
    yDE = lDesY+lCopyH;

    EL2DGETRGB(dwDColor, cSR, cSG, cSB);

    // 先复制符合颜色的部分
    pF = m_pFeatherPestBuffer;
    for (y=ySS; y<ySE; y++)
    {
        pD = pDest + y*m_iWidth;
        pS = pSrc + y*pSrcImage->m_iWidth;
        for (x=xSS; x<xSE; x++)
        {
            EL2DGETRGB(*pD, cDR, cDG, cDB);
            fC = (float)(*pS)/0xFF;
            fAC = 1.0f-fC;
            *pD = EL2DGETC((DWORD)(fC*cSR+fAC*cDR), (DWORD)(fC*cSG+fAC*cDG), (DWORD)(fC*cSB+fAC*cDB));
            *pS ++;
            *pD ++;
        }
    }
}

void RtgDevice2D::Painting(long x, long y, long radiusMin, long radiusMax)
{
    int xi, xm, yi, ym, xt, yt;
    int xss, yss, xs, ys;

    if (m_pPen==0 || m_lPenRadiusMem<radiusMax<<1)
    {
        RtCoreMem().Free(m_pPen);
        m_lPenRadiusMem = (radiusMax<<1)+1;
        m_pPen = (float*)RtCoreMem().Alloc(sizeof(float)*m_lPenRadiusMem*m_lPenRadiusMem, __FILE__, __LINE__);
    }
    if (m_lPenRadiusMax!=radiusMax || m_lPenRadiusMin!=radiusMin)
    {
        m_lPenRadiusMax = radiusMax;
        m_lPenRadiusMin = radiusMin;

        // 对模版进行赋值
        float fR;
        for (yt=0; yt<=radiusMax; yt++)
        {
            for (xt=0; xt<=radiusMax; xt++)
            {
                xi = m_lPenRadiusMax;
                yi = m_lPenRadiusMax;
                fR = sqrt((float)(yt*yt+xt*xt));
                if (fR<radiusMin)
                {
                    fR = 1;
                }else if (fR<radiusMax)
                {
                    fR = 1.f - ((fR-radiusMin)/(radiusMax-radiusMin));
                }else
                {
                    fR = 0.f;
                }
                m_pPen[(yi-yt)*m_lPenRadiusMem+(xi-xt)] = fR;
                m_pPen[(yi-yt)*m_lPenRadiusMem+(xi+xt)] = fR;
                m_pPen[(yi+yt)*m_lPenRadiusMem+(xi-xt)] = fR;
                m_pPen[(yi+yt)*m_lPenRadiusMem+(xi+xt)] = fR;
            }
        }
    }

    xi = x - radiusMax;
    xm = x + radiusMax;
    yi = y - radiusMax;
    ym = y + radiusMax;
    yss = xss = 0;
    if (xi<0) { xss = -xi; xi = 0; }
    if (yi<0) { yss = -yi; yi = 0; }
    if (xm>=m_iWidth ) { xm = m_iWidth -1; }
    if (ym>=m_iHeight) { ym = m_iHeight-1; }

    DWORD *pDest, d;
    float *pPen;
    
    for (yt=yi,ys=yss; yt<ym; yt++,ys++)
    {
        pDest = ((DWORD*)m_lpvBits)+yt*m_iWidth+xi;
        pPen = m_pPen+ys*m_lPenRadiusMem+xss;
        for (xt=xi,xs=xss; xt<xm; xt++,xs++)
        {
            d = 0xFF*(*pPen);
            if (*pDest<d)
            {
                *pDest = d;
            }
            pDest ++;
            pPen ++;
        }
    }
}

void RtgDevice2D::ShowLastError(LPSTR lpszFunction, bool bExitApp)
{
    CHAR szBuf[512];
    DWORD dw = GetLastError();

    LPVOID lpMsgBuf;
    if (!FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL ))
    {
        rt2_sprintf(szBuf, "%s failed: GetLastError returned %u\n",
            lpszFunction, dw);
    }else
    {
        rt2_sprintf(szBuf, "%s failed: GetLastError returned %u\n%s", lpszFunction, dw, lpMsgBuf);

        // Free the buffer.
        LocalFree( lpMsgBuf );
    }

    MessageBox(NULL, szBuf, "Error", MB_OK | MB_ICONINFORMATION);

    if (bExitApp)
    {
        ExitProcess(dw);
    }
}

bool RtgDevice2D::Create(HDC hCompatibleDC, const char *pszFile)
{
    BITMAP bmpData;
    HBITMAP hBitmap =(HBITMAP)::LoadImage(NULL, pszFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    GetObject(hBitmap, sizeof(BITMAP), &bmpData);

    memset(&m_bitmapInfo, 0, sizeof(BITMAPINFO));
    m_bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bitmapInfo.bmiHeader.biWidth = bmpData.bmWidth;
    m_bitmapInfo.bmiHeader.biHeight = -bmpData.bmHeight;
    m_bitmapInfo.bmiHeader.biPlanes = 1;
    m_bitmapInfo.bmiHeader.biBitCount = 32;
    m_bitmapInfo.bmiHeader.biSizeImage = bmpData.bmWidth*bmpData.bmHeight*4;
    m_bitmapInfo.bmiHeader.biXPelsPerMeter = 256;
    m_bitmapInfo.bmiHeader.biYPelsPerMeter = 256;

    m_iWidth = bmpData.bmWidth;
    m_iHeight = bmpData.bmHeight;

    RtCoreMem().Free(m_lpvBits);
    m_lpvBits = (unsigned char*)RtCoreMem().Alloc(m_iWidth*m_iHeight*4, __FILE__, __LINE__);
    int iResult2 = GetDIBits(hCompatibleDC, hBitmap, 0, m_iHeight, m_lpvBits, &m_bitmapInfo, DIB_RGB_COLORS);

    if (iResult2==0)
    {
        ShowLastError("GetDIBits", true);
    }
    ChangeRect(0, 0, m_iWidth, m_iHeight);
    return true;
}

bool RtgDevice2D::Create(HDC hCompatibleDC, int iWidth, int iHeight, DWORD dwEraseColor)
{
    if (iWidth!=m_iWidth || iHeight!=m_iHeight)
    {
        memset(&m_bitmapInfo, 0, sizeof(BITMAPINFO));
        m_bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        m_bitmapInfo.bmiHeader.biWidth = iWidth;
        m_bitmapInfo.bmiHeader.biHeight = -iHeight;
        m_bitmapInfo.bmiHeader.biPlanes = 1;
        m_bitmapInfo.bmiHeader.biBitCount = 32;
        m_bitmapInfo.bmiHeader.biSizeImage = iWidth*iHeight*4;
        m_bitmapInfo.bmiHeader.biXPelsPerMeter = 256;
        m_bitmapInfo.bmiHeader.biYPelsPerMeter = 256;

        m_iWidth = iWidth;
        m_iHeight = iHeight;

        RtCoreMem().Free(m_lpvBits);
        m_lpvBits = (unsigned char*)RtCoreMem().Alloc(m_iWidth*m_iHeight*4, __FILE__, __LINE__);
        EraseBackground(dwEraseColor);
    }
    return true;
}


//========================================================================================
//		名称：mem32BlitBitmap
//		功能：从源图块搬一块图块到目的图块[32位的图]
//		入口：lSrcW,lSrcH   lpSrc的长宽
//			  lDesW,lSrcH   lpDes的长宽
//			  lSrcX,lSrcY   取lpSrc的起始相对坐标
//			  lDesX,lDesY   贴lpDes的起始相对坐标
//			  lCopyW,lCopyH 拷贝图块的长宽
//			  lpSrc         源图块
//			  lpDes         目的图块
//            eAlphaType    混色模式
//                    0 直接复制             color = srgb
//                    1 反色复制             color = 1-srgb
//                    2 目标反色             color = 1-drgb
//                    3 根据源Alpha混色      color = srgb*sa + drgb*(1-sa)
//                    4 根据目的Alpha混色    color = srgb*da + drgb*(1-da)
//                    5 加色                 color = drgb + srgb
//                    6 减色                 color = drgb - srgb
//                    7 根据指定值混色       color = srgb*a + drgb*(1-a)
//========================================================================================
void RtgDevice2D::mem32BlitBitmap(
        long lSrcW,long lSrcH,long lDesW,long lDesH,
        DWORD *lpSrc,DWORD *lpDes,
        long lSrcX,long lSrcY,long lDesX,long lDesY,
        long lCopyW,long lCopyH, EAlphaType eAlphaType, float fAlpha)
{
    if (lSrcX<0)
    {
        lDesX -= lSrcX;
        lSrcX = 0;
    }
    if (lSrcY<0)
    {
        lDesY -= lSrcY;
        lSrcY = 0;
    }

    if (lDesX<0)
    {
        lSrcX -= lDesX;
        lDesX = 0;
    }

    if (lDesY<0)
    {
        lSrcY -= lDesY;
        lDesY = 0;
    }

    if (lSrcX+lCopyW > lSrcW)
        lCopyW = lSrcW-lSrcX;
    if (lCopyW<=0) return;

    if (lSrcY+lCopyH > lSrcH)
        lCopyH = lSrcH-lSrcY;
    if (lCopyH<=0) return;

    if (lDesX+lCopyW > lDesW)
        lCopyW = lDesW-lDesX;
    if (lCopyW<=0) return;

    if (lDesY+lCopyH > lDesH)
        lCopyH = lDesH-lDesY;
    if (lCopyH<=0) return;

    int sa, sr, sg, sb, da, dr, dg, db, cr, cg, cb;
    DWORD *pDest, *pSour, *ppDest, *ppSour;
    ppSour = pSour = lpSrc + lSrcY*lSrcW + lSrcX;
    ppDest = pDest = lpDes + lDesY*lDesW + lDesX;

    long lLineCount;
    switch (eAlphaType)
    {
    case AT_COPY: // 直接复制 color = srgb
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                *pDest = *pSour;
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ANICOPY: // 反色复制 color = 1-srgb
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                *pDest = ~(*pSour);
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ANIDEST: // 目标反色 color = 1-drgb
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                *pDest ^= 0xFFFFFFFF;
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ALPHA: // 根据源Alpha混色 color = srgb*sa + drgb*(1-sa)
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                EL2DGETARGB(*pSour, sa, sr, sg, sb);
                cr = EL2DALPHA(sr, dr, sa);
                cg = EL2DALPHA(sg, dg, sa);
                cb = EL2DALPHA(sb, db, sa);
                *pDest = EL2DGETC(cr, cg, cb);
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ALPHAD: // 根据目的Alpha混色 color = srgb*da + drgb*(1-da)
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                EL2DGETARGB(*pDest, da, dr, dg, db);
                EL2DGETRGB(*pSour, sr, sg, sb);
                cr = EL2DALPHA(sr, dr, da);
                cg = EL2DALPHA(sg, dg, da);
                cb = EL2DALPHA(sb, db, da);
                *pDest = EL2DGETC(cr, cg, cb);
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ADD: // 加色 color = drgb + srgb
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                EL2DGETRGB(*pSour, sr, sg, sb);
                *pDest = EL2DGETC(dr+sr, dg+sg, db+sb);
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_SUB: // 减色 color = drgb - srgb
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                EL2DGETRGB(*pSour, sr, sg, sb);
                *pDest = EL2DGETC(dr-sr, dg-sg, db-sb);
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ALPHAC: // 根据指定值混色 color = srgb*a + drgb*(1-a)
        sa = (int)(fAlpha * 255);
        while (lCopyH--)
        {
            lLineCount = lCopyW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                EL2DGETRGB(*pSour, sr, sg, sb);
                cr = EL2DALPHA(sr, dr, sa);
                cg = EL2DALPHA(sg, dg, sa);
                cb = EL2DALPHA(sb, db, sa);
                *pDest = EL2DGETC(cr, cg, cb);
                pDest ++ ;
                pSour ++ ;
            }
            ppSour = pSour = ppSour+lSrcW ;
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    }
}

void RtgDevice2D::mem32BlitFill(long lDesW,long lDesH,DWORD *lpDes,
        long lX,long lY,long lW,long lH, DWORD dwColor, EAlphaType eAlphaType, float fAlpha)
{
    if (lX<0) lX = 0;
    if (lY<0) lY = 0;
    if (lX+lW>lDesW) lW = lDesW - lX;
    if (lY+lH>lDesH) lH = lDesH - lY;
    if (lW<=0) return;
    if (lH<=0) return;

    int sa, sr, sg, sb, da, dr, dg, db, cr, cg, cb;
    DWORD *pDest, *ppDest;
    ppDest = pDest = lpDes + lY*lDesW + lX;
    EL2DGETARGB(dwColor, sa, sr, sg, sb);

    long lLineCount;
    switch (eAlphaType)
    {
    case AT_COPY: // 直接复制 color = srgb
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                *pDest = dwColor;
                pDest ++ ;
            }
            pDest = ppDest+lDesW;
            ppDest = pDest;
        }
        break;
    case AT_ANICOPY: // 反色复制 color = 1-srgb
        dwColor = ~dwColor;
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                *pDest = dwColor;
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ANIDEST: // 目标反色 color = 1-drgb
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                *pDest ^= 0xFFFFFFFF;
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ALPHA: // 根据源Alpha混色 color = srgb*sa + drgb*(1-sa)
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                cr = EL2DALPHA(sr, dr, sa);
                cg = EL2DALPHA(sg, dg, sa);
                cb = EL2DALPHA(sb, db, sa);
                *pDest = EL2DGETC(cr, cg, cb);
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ALPHAD: // 根据目的Alpha混色 color = srgb*da + drgb*(1-da)
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                EL2DGETARGB(*pDest, da, dr, dg, db);
                cr = EL2DALPHA(sr, dr, da);
                cg = EL2DALPHA(sg, dg, da);
                cb = EL2DALPHA(sb, db, da);
                *pDest = EL2DGETC(cr, cg, cb);
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ADD: // 加色 color = drgb + srgb
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                *pDest = EL2DGETC(dr+sr, dg+sg, db+sb);
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_SUB: // 减色 color = drgb - srgb
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                *pDest = EL2DGETC(dr-sr, dg-sg, db-sb);
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    case AT_ALPHAC: // 根据指定值混色 color = srgb*a + drgb*(1-a)
        sa = (int)(fAlpha * 255);
        while (lH--)
        {
            lLineCount = lW ;
            while (lLineCount--)
            {
                EL2DGETRGB(*pDest, dr, dg, db);
                cr = EL2DALPHA(sr, dr, sa);
                cg = EL2DALPHA(sg, dg, sa);
                cb = EL2DALPHA(sb, db, sa);
                *pDest = EL2DGETC(cr, cg, cb);
                pDest ++ ;
            }
            ppDest = pDest = ppDest+lDesW ;
        }
        break;
    }
}

void RtgDevice2D::Gray(int iType)
{
    DWORD* p = (DWORD*)m_lpvBits;
    int i, j = m_iWidth*m_iHeight;
    int r, g, b, gray;
    switch (iType)
    {
    case 0:
        for (i=0; i<j; i++, p++)
        {
            EL2DGETRGB(*p, r, g, b);
            gray = r*0.3f + g*0.6f + b*0.1f;
            *p = EL2DGETC(gray, gray, gray);
        }
        break;
    case 3:
        for (i=0; i<j; i++, p++)
        {
            EL2DGETRGB(*p, r, g, b);
            *p = EL2DGETC(r, r, r);
        }
        break;
    case 2:
        for (i=0; i<j; i++, p++)
        {
            EL2DGETRGB(*p, r, g, b);
            *p = EL2DGETC(g, g, g);
        }
        break;
    case 1:
        for (i=0; i<j; i++, p++)
        {
            EL2DGETRGB(*p, r, g, b);
            *p = EL2DGETC(b, b, b);
        }
        break;
    }
}

DWORD* RtgDevice2D::GetRawData()
{
    return (DWORD*)m_lpvBits;
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
