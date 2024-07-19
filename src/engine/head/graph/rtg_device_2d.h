
namespace rt_graph {

class RtgDevice2D
{
public:
    enum EAlphaType
    {
        AT_COPY,    // 0 ֱ�Ӹ���             color = srgb
        AT_ANICOPY, // 1 ��ɫ����             color = 1-srgb
        AT_ANIDEST, // 2 Ŀ�귴ɫ             color = 1-drgb
        AT_ALPHA,   // 3 ����ԴAlpha��ɫ      color = srgb*sa + drgb*(1-sa)
        AT_ALPHAD,  // 4 ����Ŀ��Alpha��ɫ    color = srgb*da + drgb*(1-da)
        AT_ADD,     // 5 ��ɫ                 color = drgb + srgb
        AT_SUB,     // 6 ��ɫ                 color = drgb - srgb
        AT_ALPHAC,  // 7 ����ָ��ֵ��ɫ       color = srgb*a + drgb*(1-a)
    };

public:
    RtgDevice2D();
    virtual ~RtgDevice2D();

public:
    // ���������
    bool Create(HDC hCompatibleDC, const char *pszFile);
    bool Create(HDC hCompatibleDC, int iWidth, int iHeight, DWORD dwEraseColor=0xFFFFFFFF);
    void ClearMemDC();
    void UpdateMemDC(HDC hCompatibleDC);

    // ����DC��
    void BltToDC(HDC hDestDC, int x=0, int y=0, int w=0, int h=0);
    void BltToDC(HDC hDestDC, int iDx, int iDy, int iSx, int iSy, int iSw, int iSh);
    void GetChangedRect(RECT& rt);

    // ��ʾWindows������Ϣ
    void ShowLastError(LPSTR lpszFunction, bool bExitApp=false);

    // ��ͼ
    void EraseBackground(DWORD dwEraseColor=0xFFFFFFFF);

    void FillRectangle(DWORD dwColor, int x, int y, int w, int h,
        EAlphaType eAlphaType=AT_COPY,float fAlpha=1.f);

    void FillImage(long lDesX,long lDesY, long lCopyW,long lCopyH,
        DWORD* lpSrc, long lSrcW,long lSrcH,
        long lSrcX=0, long lSrcY=0, EAlphaType eAlphaType=AT_COPY,float fAlpha=1.f);

    void FillImage(long lDesX,long lDesY, long lCopyW,long lCopyH,
        RtgDevice2D* pSrc, long lSrcX=0, long lSrcY=0, EAlphaType eAlphaType=AT_COPY,float fAlpha=1.f);

    void Painting(long x, long y, long radiusMin, long radiusMax);

    // ��Pest
    void FeatherPest(long lDesX,long lDesY, long lCopyW,long lCopyH,
        RtgDevice2D* pSrcImage, long lFeatherRadius, DWORD dwDColor, long lSrcX=0, long lSrcY=0, EAlphaType eAlphaType=AT_COPY,float fAlpha=1.f);

    // ������ɻҶ�ͼ��iType������4��;����0 ������ɫ��ϵĻҶ�ͼ, 1/2/3��ȡr/g/b
    void Gray(int iType);

    DWORD* GetRawData();

protected:
    static void mem32BlitBitmap(long lSrcW,long lSrcH,long lDesW,long lDesH,DWORD *lpSrc,DWORD *lpDes,
        long lSrcX,long lSrcY,long lDesX,long lDesY,long lCopyW,long lCopyH,EAlphaType eAlphaType,float fAlpha=1.f);
    static void mem32BlitFill(long lDesW,long lDesH,DWORD *lpDes,
        long lX,long lY,long lW,long lH, DWORD dwColor, EAlphaType eAlphaType, float fAlpha=1.f);

    void ChangeRect(int x, int y, int w, int h);

protected:
    bool        m_bImageChanged;
    RECT        m_rtChanged;

    // ��ͼ���ڱ���
    int         m_bMemCreated;
    int         m_iWidth;
    int         m_iHeight;
    int         m_iDCWidth;
    int         m_iDCHeight;
    HDC         m_hMemDC;
    HBITMAP     m_hMemBitmap;
    HBITMAP     m_hOldBitmap;
#ifdef	USER_DRAW_DIB
    HDRAWDIB    m_hDrawDib;
#endif

    char*               m_pFeatherPestBuffer;
    BITMAPINFO          m_bitmapInfo;
    LPVOID              m_lpvBits;

    float*              m_pPen;
    long                m_lPenRadiusMem;
    long                m_lPenRadiusMin;
    long                m_lPenRadiusMax;

};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
