
#ifndef _RtgImage_H_
#define _RtgImage_H_


namespace rt_graph {

class RtgImage
{
public:
    enum ERtgiError
    {
        ERI_OK              = 0,
        ERI_FILENOTFOUND,
        ERI_FUNC_UNSUPPORT,
        ERI_MEMORY_ERR,
        ERI_IMAGE_IS_NULL,

        ERI_BMP_FILE_FORMAT_ERR,
        ERI_BMP_FILE_FORMAT_WIDTH,
        ERI_BMP_FILE_FORMAT_UNSUPPORT,

        ERI_TGA_FILE_FORMAT_ERR,
        ERI_TGA_FILE_FORMAT_WIDTH,
        ERI_TGA_FILE_FORMAT_UNSUPPORT,

        ERI_DDS_FILE_FORMAT_ERR,
        ERI_DDS_FILE_FORMAT_WIDTH,
        ERI_DDS_FILE_FORMAT_UNSUPPORT,
    };
    enum EImgType
    {
        EIT_UNKNOWE,
        EIT_DXT,
        EIT_BMP,
        EIT_TGA,
        EIT_JPG,
		EIT_RLE,
    };
    enum EPixFormat
    {
        EPF_ARGB,
        EPF_RGB,
        EPF_DXT1,
        EPF_DXT2,
        EPF_DXT3,
        EPF_DXT4,
        EPF_DXT5,
        EPF_L8,
        EPF_UNKNOWN
    };
    enum EScaleType
    {
        EST_NEAREST,
        EST_LINEAR,
        EST_BILINEAR,
    };
    struct SInfo
    {
        int iWidth, iHeight;
        int iSizeOfData;
        int iDxtcFormat;
        bool bAlpha;
        bool bDDS;
    }info;
    struct SData
    {
        unsigned char* pData;
    }data;

protected:
 //   char m_szFile[64];
	//char m_szAlpha[64];
	CM_MEMDEF(m_szFile, 64)
	CM_MEMDEF(m_szAlpha, 64)
    bool m_bLoad;
    bool m_bUseColorKey;
    int m_iColorKeyR, m_iColorKeyG, m_iColorKeyB;
    EImgType m_eImageType;

public:
    static ERtgiError m_eLastError;
    static int m_bSupportDXT;
    static int m_bSquare;
    static int m_iMaxWidth, m_iMaxHeight;
    static int m_iTextureQuality;

    static void StaticInit();
    static bool Check24bBmpFormat(const char* szFilename, int& iWidth, int& iHeight);
    static bool Check24bBmpFormat(void* pBits, int iDataSize, int& iWidth, int& iHeight);
    static void SetSaveJpegQuality(int iQuality);

protected:
    static unsigned int m_bLoadTmpImg, m_dwTmpImage, m_dwTmpAlpha; 

public:
	RtgImage();
	RtgImage(const RtgImage &object);
    ~RtgImage();
    void SetColorKey(int use, int r, int g, int b);
    void ColorKeyInImage(void);

    bool Create(int iWidth, int iHeight, bool bAlpha);

    bool Load(const char *szImage, const char *szAlpha=NULL);
    bool LoadRawdata(void *buffer, int width, int height);
    bool LoadFromMemory(void *bits, int size, void *abits=0, int asize=0);

    void FixTextureQuality(bool bQualityManage);

    int  GetDataSize()                  { return info.iSizeOfData; }
    int  GetMipMapCount(void);
    void *GetRawData(int level=0);
    void ResizeCanvas(int width, int height); // 缩放画布，不会缩放图片，如果撑大就用白色填充，如果缩小就截取
    void Resize(int width, int height); // 图像缩放
    void *CreateBitmap(void* hDC);

    void Flip(bool bForce=false);    // 上下翻转, bForce为true的时候表示强制进行，不管m_bLoad的标记。注意：使用这个参数可能引发访问非法内存
    void Mirror(bool bForce=false);  // 左右翻转, bForce为true的时候表示强制进行，不管m_bLoad的标记。注意：使用这个参数可能引发访问非法内存

    bool SaveToBMP(const char* szFilename);
    bool SaveToTGA(const char* szFilename);
    bool SaveToJPG(const char* szFilename);

    bool SaveToArchiveBMP(RtArchive* pArc);
    bool SaveToArchiveTGA(RtArchive* pArc);
    bool SaveToArchiveJPG(RtArchive* pArc);

protected:
    EImgType GetImageType(const char* szFilename);

};

void Rtgimg_Fill_A8R8G8B8(int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_X8R8G8B8(int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_R8G8B8  (int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_A4R4G4B4(int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_A8R3G3B2(int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_A1R5G5B5(int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_R5G6B5  (int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_X1R5G5B5(int width, int height, void *dest, int destpitch, void *src, int srcpitch);
void Rtgimg_Fill_X4R4G4B4(int width, int height, void *dest, int destpitch, void *src, int srcpitch);

} // namespace rt_graph


#endif
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
