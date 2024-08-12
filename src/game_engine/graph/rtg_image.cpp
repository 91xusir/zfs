
#include "rtg_graph_inter.h"
//#include "rt_extend.h"
#include "rtg_image_bmp.h"
#include "rtg_image_tga.h"
#include "rtg_image_dds.h"
#include "rtg_image_jpg.h"
#include "TextureRLE.h"

namespace rt_graph {

int RtgImage::m_bSupportDXT = 0;
int RtgImage::m_bSquare = 0;
int RtgImage::m_iMaxWidth = 512;
int RtgImage::m_iMaxHeight = 512;
int RtgImage::m_iTextureQuality = 0;

RtgImage::ERtgiError RtgImage::m_eLastError = RtgImage::ERI_OK;
unsigned int RtgImage::m_bLoadTmpImg = 0;
unsigned int RtgImage::m_dwTmpImage = 0;
unsigned int RtgImage::m_dwTmpAlpha = 0;

static char *GetFileExt(const char *filename)
{
    //static char ext[10];
	S_MEMDEF(ext, 10)
	S_MEMPROTECTOR(ext, 10, bMP)
    _splitpath(filename, NULL, NULL, NULL, ext);
    strupr(ext);
    return &ext[1];
}

static int GetMipMapCount(int width, int height)
{
    int l = width > height ? width : height;
    int count=0;
    while(l)
    {
        l/=2;
        count++;
    }
    return count;
}

static bool IsPow2(int value)
{
    if ( value==0 )
        return 0;

    int mask=1;
    int count=0;

    for ( int i=0; i<32; i++ )
    {
        if ( value & mask )
        {
            count++;
            if ( count>=2 )
                return false;
        }
        mask<<=1;
    }
    return true;
}

static int GetPow2Less(int value)
{
    if ( value==0 )
        return 0;

    int mask = 0x10000000;
    for ( int i=0; i<32; i++ )
    {
        if ( mask & value )
        {
            break;
        }
        mask>>=1;
    }
    return mask;
}

//////////////////////////////////////////////////////////////////////////

void RtgImage::StaticInit()
{
}

bool RtgImage::Check24bBmpFormat(const char* szFilename, int& iWidth, int& iHeight)
{
    RtArchive* pFile = RtCoreFile().CreateFileReader(szFilename, 0);
    if (pFile==NULL) return false;
    int iSize = pFile->TotalSize();
    char* pBits = (char*)RtCoreMem().Alloc(iSize+1, __FILE__, __LINE__);
    if (pBits==NULL)
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }
    pFile->Serialize(pBits, iSize);
    RtCoreFile().CloseFile(pFile);
    bool bResult = Check24bBmpFormat(pBits, iSize, iWidth, iHeight);
    RtCoreMem().Free(pBits);
    return bResult;
}

void RtgImage::SetSaveJpegQuality(int iQuality)
{
    rtgiSetJpegQuality(iQuality);
}

#pragma pack(push)
#pragma pack(1)
struct SRtgBitmapFileHeader
{ 
    WORD    bfType; 
    DWORD   bfSize; 
    WORD    bfReserved1; 
    WORD    bfReserved2; 
    DWORD   bfOffBits; 
};

struct SRtgBitmapInfoHeader
{
    DWORD  biSize; 
    LONG   biWidth; 
    LONG   biHeight; 
    WORD   biPlanes; 
    WORD   biBitCount; 
    DWORD  biCompression; 
    DWORD  biSizeImage; 
    LONG   biXPelsPerMeter; 
    LONG   biYPelsPerMeter; 
    DWORD  biClrUsed; 
    DWORD  biClrImportant; 
};
#pragma pack(pop)

bool RtgImage::Check24bBmpFormat(void* pBits, int iDataSize, int& iWidth, int& iHeight)
{
    SRtgBitmapFileHeader* pFileHeader = (SRtgBitmapFileHeader*)pBits;
    SRtgBitmapInfoHeader* pInfoHeader = (SRtgBitmapInfoHeader*)((char*)pBits+sizeof(SRtgBitmapFileHeader));
    if (pFileHeader->bfType!=0x4d42)                        return false;
    if (pFileHeader->bfSize!=iDataSize)                     return false;
    if (pInfoHeader->biSize!=sizeof(SRtgBitmapInfoHeader))  return false;
    if (pInfoHeader->biPlanes!=1)                           return false;
    if (pInfoHeader->biBitCount!=24)                        return false;
    if (pInfoHeader->biCompression!=0)                      return false;
    if (pFileHeader->bfOffBits!=(sizeof(SRtgBitmapInfoHeader)+sizeof(SRtgBitmapFileHeader))) return false;
    iWidth  = pInfoHeader->biWidth;
    iHeight = pInfoHeader->biHeight;
    return true;
}

RtgImage::RtgImage()
{
	CM_MEMPROTECTOR(m_szFile, 64)
	CM_MEMPROTECTOR(m_szAlpha, 64)

	m_szFile[0] = '\0';
    m_szAlpha[0] = '\0';
    m_bLoad = false;
    m_bUseColorKey = 0;
    m_iColorKeyR = m_iColorKeyG = m_iColorKeyB = 0;
    m_eImageType = EIT_UNKNOWE;

    info.iWidth = info.iHeight = 0;
    info.iSizeOfData = 0;
    info.iDxtcFormat = 0;
    info.bAlpha = false;
    info.bDDS = false;

    data.pData = NULL;
}

RtgImage::RtgImage(const RtgImage &object)
{
	memcpy(this, &object, sizeof(RtgImage));
	CM_MEMPROTECTOR(m_szFile, 64)
	CM_MEMPROTECTOR(m_szAlpha, 64)
}

RtgImage::~RtgImage()
{
	CM_MEMUNPROTECTOR(m_szFile)
	CM_MEMUNPROTECTOR(m_szAlpha)

    if (data.pData)
    {
        RtCoreMem().Free(data.pData);
        data.pData = NULL;
    }
}

void RtgImage::SetColorKey(int use, int r, int g, int b)
{
    m_bUseColorKey = use!=0;
    m_iColorKeyR = r;
    m_iColorKeyG = g;
    m_iColorKeyB = b;
}

void *RtgImage::GetRawData(int level)
{
    return data.pData;
}

void RtgImage::Flip(bool bForce)
{
    if (!m_bLoad && !bForce) return;
    if (info.bDDS) return;
	enum
	{
        MAX_ONE_LINE_WIDTH = 1024 * 4
	};
    static unsigned char pFlipBuffer[ MAX_ONE_LINE_WIDTH ];
    static int iFlipBufferSize = 0;
	memset( pFlipBuffer , NULL , MAX_ONE_LINE_WIDTH );
    int y, ym, Bps, Bpp;
    Bpp = 4;
    Bps = info.iWidth * Bpp;
    ym = info.iHeight/2;
	if( MAX_ONE_LINE_WIDTH < Bps )
	{
		CHECK( false );
	}
    //if (iFlipBufferSize<Bps)
    //{
    //    RtCoreMem().Free(pFlipBuffer);
    //    iFlipBufferSize = Bps;
    //    //pFlipBuffer = (unsigned char*)RtCoreMem().Alloc(iFlipBufferSize, __FILE__, __LINE__);
    //}

    unsigned char *pS, *pD;
    for (y=0; y<ym; y++)
    {
        pS = data.pData+y*Bps;
        pD = data.pData+(info.iHeight-y-1)*Bps;

        memcpy(pFlipBuffer, pS, Bps);
        memcpy(pS, pD, Bps);
        memcpy(pD, pFlipBuffer, Bps);
    }
}

void RtgImage::Mirror(bool bForce)
{
}

int RtgImage::GetMipMapCount(void)
{
    return rt_graph::GetMipMapCount(info.iWidth, info.iHeight);
}

void RtgImage::ColorKeyInImage(void)
{
    if (!m_bLoad)
        return;

    unsigned char *buffer = data.pData + (info.iHeight-1) * info.iWidth * 4;

    m_bUseColorKey = 1;
    m_iColorKeyR = buffer[0];
    m_iColorKeyG = buffer[1];
    m_iColorKeyB = buffer[2];
}

bool RtgImage::Create(int iWidth, int iHeight, bool bAlpha)
{
    info.bAlpha = bAlpha;
    info.bDDS = false;
    info.iDxtcFormat = 0;
    info.iHeight = iHeight;
    info.iWidth = iWidth;
    info.iSizeOfData = iWidth*iHeight*4;
    data.pData = (unsigned char*)RtCoreMem().Alloc(info.iSizeOfData, __FILE__, __LINE__);
    if (data.pData==NULL)
    {
        m_eLastError = ERI_MEMORY_ERR;
        return false;
    }
    return true;
}

RtgImage::EImgType RtgImage::GetImageType(const char *szFilename)
{
	EImgType type = EIT_BMP;
	char *ext = GetFileExt(szFilename);
	if ( !strcmp(ext,"DDS") )
	{
		type = EIT_DXT;
	}else if ( !strcmp(ext,"RLE") )
	{
		type = EIT_RLE;
	}else if ( !strcmp(ext,"BMP") )
	{
		type = EIT_BMP;
	}else if ( !strcmp(ext,"TGA") )
	{
		type = EIT_TGA;
	}else if ( !strcmp(ext,"JPG") )
	{
		type = EIT_JPG;
	}
	return type;
}

bool RtgImage::Load(const char *szImage, const char *szAlpha)
{
	m_bUseColorKey = 0;
	m_eImageType = GetImageType(szImage);
	switch (m_eImageType)
	{
	case EIT_RLE:
		if (!rtgiLoadImageRLEFile(szImage, this))
		{
//#ifdef _DEBUG
//			RtCoreLog().Warn("Load RLE [%s] failed.\n", szImage);
//			CHECKEX("ÔØÈëRLEÊ§°Ü");
//#endif
			return false;
		}
		break;
	case EIT_BMP:
		if (!rtgiLoadImageBMPFile(szImage, this))
		{
//#ifdef _DEBUG
//			RtCoreLog().Warn("Load bmp [%s] failed.\n", szImage);
//			CHECKEX("ÔØÈëBMPÊ§°Ü");
//#endif
			return false;
		}
		break;
	case EIT_TGA:
		if (!rtgiLoadImageTGAFile(szImage, this))
		{
//#ifdef _DEBUG
//			RtCoreLog().Warn("Load tga [%s] failed.\n", szImage);
//			CHECKEX("ÔØÈëTGAÊ§°Ü");
//#endif
			return false;
		}
		break;
	case EIT_DXT:
		if (!rtgiLoadImageDDSFile(szImage, this))
		{
//#ifdef _DEBUG
//			RtCoreLog().Warn("Load dds [%s] failed.\n", szImage);
//			CHECKEX("ÔØÈëDDSÊ§°Ü");
//#endif
			return false;
		}
		break;
	case EIT_JPG:
		if (!rtgiLoadImageJPGFile(szImage, this))
		{
//#ifdef _DEBUG
//			RtCoreLog().Warn("Load jpg [%s] failed.\n", szImage);
//			CHECKEX("ÔØÈëJPGÊ§°Ü");
//#endif
			return false;
		}
		break;
	default:
		return false;
	}

	m_bLoad = true;

	if ( szAlpha )
	{
	}
	return true;
}

bool RtgImage::LoadRawdata(void *buffer, int width, int height)
{
    m_eLastError = ERI_FUNC_UNSUPPORT;
    return false;
}

bool RtgImage::LoadFromMemory(void *ibits, int isize, void *abits, int asize)
{
    RtMemoryArchive memImg((char*)ibits, isize);
    if (!rtgiLoadImageBMP(&memImg, this))
    {
#ifdef _DEBUG
        RtCoreLog().Warn("Load bmp [memory] failed.\n");
#endif
        return false;
    }

    m_bLoad = true;
    return true;
}

void rtgiScale2DNear(unsigned char* pSrc, unsigned int dwSrcWidth, unsigned int dwSrcHeight,
                     unsigned char* pDest,unsigned int dwDestWidth, unsigned int dwDestHeight,
                     RtgImage::EScaleType eScaleType, float fScaleX, float fScaleY)
{
    int x, y, SrcY, DestY, SrcX, DestX;
    unsigned int* pS = (unsigned int*)pSrc;
    unsigned int* pD = (unsigned int*)pDest;
    for (y = 0; y < (int)dwDestHeight; y++)
    {
        DestY = y * dwDestWidth;
        SrcY  = (unsigned int)((y / fScaleY) * dwSrcWidth);
        for (x = 0; x < (int)dwDestWidth; x++)
        {
            DestX = x;
            SrcX  = (unsigned int)(x / fScaleX);
            pD[DestY + DestX] = pS[SrcY + SrcX];
        }
    }
}

void rtgiScale2DLinear(unsigned char* pSrc, unsigned int dwSrcWidth, unsigned int dwSrcHeight,
                     unsigned char* pDest,unsigned int dwDestWidth, unsigned int dwDestHeight,
                     RtgImage::EScaleType eScaleType, float fScaleX, float fScaleY)
{
    float t1, t2, t4;
    int x, y, SrcY, SrcX1, SrcX2, Offset, c;
    unsigned char x1, x2;
    float f, ft;

    unsigned char* pS = pSrc;
    unsigned char* pD = pDest;
    for (y = 0; y < (int)dwDestHeight; y++)
    {
        SrcY = ((unsigned int)(y / fScaleY))*dwSrcWidth;
        for (x = 0; x < (int)dwDestWidth; x++)
        {
            t1 = x / (float)dwSrcWidth;
            t4 = t1 * dwSrcWidth;
            t2 = t4 - (unsigned int)(t4);
            ft = t2 * PI;
            f = (1.0f - fcos(ft)) * 0.5f;
            SrcX1 = ((unsigned int)(t4 / fScaleX));
            SrcX2 = ((unsigned int)(t4 / fScaleX) + 1);

            Offset = (y * dwDestWidth + x)<<2;
            for (c = 0; c < 4; c++)
            {
                x1 = pS[((SrcY + SrcX1)<<2) + c];
                x2 = pS[((SrcY + SrcX2)<<2) + c];
                pD[Offset + c] = (unsigned int)((1.0 - f) * x1 + f * x2);
            }
        }
    }
}

/*
bool rtgiScale2DBilinear(unsigned char* pSrc, unsigned int dwSrcWidth, unsigned int dwSrcHeight,
                       unsigned char* pDest,unsigned int dwDestWidth, unsigned int dwDestHeight,
                       RtgImage::EScaleType eScaleType, float fScaleX, float fScaleY)
{
    dwSrcWidth = Image->Bps / Image->Bpc;
    dwDestWidth = Scaled->Bps / Scaled->Bpc;

    switch (Image->Bpc)
    {
    case 1:
        Height--;  // Only use regular Height once in the following loop.
        for (y = 0; y < Height; y++) {
            NewY1 = (unsigned int)(y / ScaleY) * dwSrcWidth;
            NewY2 = (unsigned int)((y+1) / ScaleY) * dwSrcWidth;
            for (x = 0; x < Width; x++) {
                NewX = Width / fScaleX;
                t1 = x / (double)Width;
                t4 = t1 * Width;
                t2 = t4 - (unsigned int)(t4);
                t3 = (1.0 - t2);
                t4 = t1 * NewX;
                NewX1 = (unsigned int)(t4) * Image->Bpp;
                NewX2 = (unsigned int)(t4 + 1) * Image->Bpp;

                for (c = 0; c < Scaled->Bpp; c++) {
                    Table[0][c] = t3 * Image->Data[NewY1 + NewX1 + c] +
                        t2 * Image->Data[NewY1 + NewX2 + c];

                    Table[1][c] = t3 * Image->Data[NewY2 + NewX1 + c] +
                        t2 * Image->Data[NewY2 + NewX2 + c];
                }

                // Linearly interpolate between the table values.
                t1 = y / (double)(Height + 1);  // Height+1 is the real height now.
                t3 = (1.0 - t1);
                Size = y * dwDestWidth + x * Scaled->Bpp;
                for (c = 0; c < Scaled->Bpp; c++) {
                    Scaled->Data[Size + c] =
                        (ILubyte)(t3 * Table[0][c] + t1 * Table[1][c]);
                }
            }
        }

        // Calculate the last row.
        NewY1 = (unsigned int)(Height / ScaleY) * dwSrcWidth;
        for (x = 0; x < Width; x++) {
            NewX = Width / fScaleX;
            t1 = x / (double)Width;
            t4 = t1 * Width;
            ft = (t4 - (unsigned int)(t4)) * IL_PI;
            f = (1.0 - cos(ft)) * .5;  // Cosine interpolation
            NewX1 = (unsigned int)(t1 * NewX) * Image->Bpp;
            NewX2 = (unsigned int)(t1 * NewX + 1) * Image->Bpp;

            Size = Height * dwDestWidth + x * Image->Bpp;
            for (c = 0; c < Scaled->Bpp; c++) {
                Scaled->Data[Size + c] = (ILubyte)((1.0 - f) * Image->Data[NewY1 + NewX1 + c] +
                    f * Image->Data[NewY1 + NewX2 + c]);
            }
        }
        break;

    case 2:
        ShortPtr = (unsigned short*)Image->Data;
        SShortPtr = (unsigned short*)Scaled->Data;
        Height--;  // Only use regular Height once in the following loop.
        for (y = 0; y < Height; y++) {
            NewY1 = (unsigned int)(y / ScaleY) * dwSrcWidth;
            NewY2 = (unsigned int)((y+1) / ScaleY) * dwSrcWidth;
            for (x = 0; x < Width; x++) {
                NewX = Width / fScaleX;
                t1 = x / (double)Width;
                t4 = t1 * Width;
                t2 = t4 - (unsigned int)(t4);
                t3 = (1.0 - t2);
                t4 = t1 * NewX;
                NewX1 = (unsigned int)(t4) * Image->Bpp;
                NewX2 = (unsigned int)(t4 + 1) * Image->Bpp;

                for (c = 0; c < Scaled->Bpp; c++) {
                    Table[0][c] = t3 * ShortPtr[NewY1 + NewX1 + c] +
                        t2 * ShortPtr[NewY1 + NewX2 + c];

                    Table[1][c] = t3 * ShortPtr[NewY2 + NewX1 + c] +
                        t2 * ShortPtr[NewY2 + NewX2 + c];
                }

                // Linearly interpolate between the table values.
                t1 = y / (double)(Height + 1);  // Height+1 is the real height now.
                t3 = (1.0 - t1);
                Size = y * dwDestWidth + x * Scaled->Bpp;
                for (c = 0; c < Scaled->Bpp; c++) {
                    SShortPtr[Size + c] =
                        (unsigned short)(t3 * Table[0][c] + t1 * Table[1][c]);
                }
            }
        }

        // Calculate the last row.
        NewY1 = (unsigned int)(Height / ScaleY) * dwSrcWidth;
        for (x = 0; x < Width; x++) {
            NewX = Width / fScaleX;
            t1 = x / (double)Width;
            t4 = t1 * Width;
            ft = (t4 - (unsigned int)(t4)) * IL_PI;
            f = (1.0 - cos(ft)) * .5;  // Cosine interpolation
            NewX1 = (unsigned int)(t1 * NewX) * Image->Bpp;
            NewX2 = (unsigned int)(t1 * NewX + 1) * Image->Bpp;

            Size = Height * dwDestWidth + x * Image->Bpp;
            for (c = 0; c < Scaled->Bpp; c++) {
                SShortPtr[Size + c] = (unsigned short)((1.0 - f) * ShortPtr[NewY1 + NewX1 + c] +
                    f * ShortPtr[NewY1 + NewX2 + c]);
            }
        }
        break;

    case 4:
        if (Image->Type != IL_FLOAT) {
            IntPtr = (unsigned int*)Image->Data;
            SIntPtr = (unsigned int*)Scaled->Data;
            Height--;  // Only use regular Height once in the following loop.
            for (y = 0; y < Height; y++) {
                NewY1 = (unsigned int)(y / ScaleY) * dwSrcWidth;
                NewY2 = (unsigned int)((y+1) / ScaleY) * dwSrcWidth;
                for (x = 0; x < Width; x++) {
                    NewX = Width / fScaleX;
                    t1 = x / (double)Width;
                    t4 = t1 * Width;
                    t2 = t4 - (unsigned int)(t4);
                    t3 = (1.0 - t2);
                    t4 = t1 * NewX;
                    NewX1 = (unsigned int)(t4) * Image->Bpp;
                    NewX2 = (unsigned int)(t4 + 1) * Image->Bpp;

                    for (c = 0; c < Scaled->Bpp; c++) {
                        Table[0][c] = t3 * IntPtr[NewY1 + NewX1 + c] +
                            t2 * IntPtr[NewY1 + NewX2 + c];

                        Table[1][c] = t3 * IntPtr[NewY2 + NewX1 + c] +
                            t2 * IntPtr[NewY2 + NewX2 + c];
                    }

                    // Linearly interpolate between the table values.
                    t1 = y / (double)(Height + 1);  // Height+1 is the real height now.
                    t3 = (1.0 - t1);
                    Size = y * dwDestWidth + x * Scaled->Bpp;
                    for (c = 0; c < Scaled->Bpp; c++) {
                        SIntPtr[Size + c] =
                            (unsigned int)(t3 * Table[0][c] + t1 * Table[1][c]);
                    }
                }
            }

            // Calculate the last row.
            NewY1 = (unsigned int)(Height / ScaleY) * dwSrcWidth;
            for (x = 0; x < Width; x++) {
                NewX = Width / fScaleX;
                t1 = x / (double)Width;
                t4 = t1 * Width;
                ft = (t4 - (unsigned int)(t4)) * IL_PI;
                f = (1.0 - cos(ft)) * .5;  // Cosine interpolation
                NewX1 = (unsigned int)(t1 * NewX) * Image->Bpp;
                NewX2 = (unsigned int)(t1 * NewX + 1) * Image->Bpp;

                Size = Height * dwDestWidth + x * Image->Bpp;
                for (c = 0; c < Scaled->Bpp; c++) {
                    SIntPtr[Size + c] = (unsigned int)((1.0 - f) * IntPtr[NewY1 + NewX1 + c] +
                        f * IntPtr[NewY1 + NewX2 + c]);
                }
            }
        }
        else {  // IL_FLOAT
            FloatPtr = (ILfloat*)Image->Data;
            SFloatPtr = (ILfloat*)Scaled->Data;
            Height--;  // Only use regular Height once in the following loop.
            for (y = 0; y < Height; y++) {
                NewY1 = (unsigned int)(y / ScaleY) * dwSrcWidth;
                NewY2 = (unsigned int)((y+1) / ScaleY) * dwSrcWidth;
                for (x = 0; x < Width; x++) {
                    NewX = Width / fScaleX;
                    t1 = x / (double)Width;
                    t4 = t1 * Width;
                    t2 = t4 - (unsigned int)(t4);
                    t3 = (1.0 - t2);
                    t4 = t1 * NewX;
                    NewX1 = (unsigned int)(t4) * Image->Bpp;
                    NewX2 = (unsigned int)(t4 + 1) * Image->Bpp;

                    for (c = 0; c < Scaled->Bpp; c++) {
                        Table[0][c] = t3 * FloatPtr[NewY1 + NewX1 + c] +
                            t2 * FloatPtr[NewY1 + NewX2 + c];

                        Table[1][c] = t3 * FloatPtr[NewY2 + NewX1 + c] +
                            t2 * FloatPtr[NewY2 + NewX2 + c];
                    }

                    // Linearly interpolate between the table values.
                    t1 = y / (double)(Height + 1);  // Height+1 is the real height now.
                    t3 = (1.0 - t1);
                    Size = y * dwDestWidth + x * Scaled->Bpp;
                    for (c = 0; c < Scaled->Bpp; c++) {
                        SFloatPtr[Size + c] =
                            (ILfloat)(t3 * Table[0][c] + t1 * Table[1][c]);
                    }
                }
            }

            // Calculate the last row.
            NewY1 = (unsigned int)(Height / ScaleY) * dwSrcWidth;
            for (x = 0; x < Width; x++) {
                NewX = Width / fScaleX;
                t1 = x / (double)Width;
                t4 = t1 * Width;
                ft = (t4 - (unsigned int)(t4)) * IL_PI;
                f = (1.0 - cos(ft)) * .5;  // Cosine interpolation
                NewX1 = (unsigned int)(t1 * NewX) * Image->Bpp;
                NewX2 = (unsigned int)(t1 * NewX + 1) * Image->Bpp;

                Size = Height * dwDestWidth + x * Image->Bpp;
                for (c = 0; c < Scaled->Bpp; c++) {
                    SFloatPtr[Size + c] = (ILfloat)((1.0 - f) * FloatPtr[NewY1 + NewX1 + c] +
                        f * FloatPtr[NewY1 + NewX2 + c]);
                }
            }
        }
        break;
    }

    return Scaled;
}
*/

bool rtgiScale2D(unsigned char* pSrc, unsigned int dwSrcWidth, unsigned int dwSrcHeight,
                 unsigned char* pDest,unsigned int dwDestWidth, unsigned int dwDestHeight, RtgImage::EScaleType eScaleType)
{
    if (pSrc==NULL || pDest==NULL)
    {
        return false;
    }
    float fScaleX = (float)dwDestWidth / dwSrcWidth;
    float fScaleY = (float)dwDestHeight / dwSrcHeight;
    if (eScaleType==RtgImage::EST_NEAREST)
        rtgiScale2DNear(pSrc, dwSrcWidth, dwSrcHeight, pDest, dwDestWidth, dwDestHeight, eScaleType, fScaleX, fScaleY);
    else if (eScaleType==RtgImage::EST_LINEAR)
        rtgiScale2DLinear(pSrc, dwSrcWidth, dwSrcHeight, pDest, dwDestWidth, dwDestHeight, eScaleType, fScaleX, fScaleY);
    rtgiScale2DLinear(pSrc, dwSrcWidth, dwSrcHeight, pDest, dwDestWidth, dwDestHeight, eScaleType, fScaleX, fScaleY);
    //rtgiScale2DBilinear(pSrc, dwSrcWidth, dwSrcHeight, pDest, dwDestWidth, dwDestHeight, eScaleType, fScaleX, fScaleY);
    return true;
}

void RtgImage::ResizeCanvas(int width, int height)
{
    if (m_bLoad && !info.bDDS)
    {
        if (info.iWidth!=width || info.iHeight!=height)
        {
            int y, yCopy, ySkip, wCopy, wFill;

            unsigned char* pDest = (unsigned char*)RtCoreMem().Alloc(width*height*4, __FILE__, __LINE__);
            unsigned char *pS, *pD;
            pD = pDest;
            pS = data.pData;

            yCopy = (info.iHeight>height)?height:info.iHeight;
            ySkip = height-info.iHeight;
            if (ySkip<0) ySkip = 0;
            wCopy = (info.iWidth>width)?width:info.iWidth;
            wFill = height-wCopy;
            if (wFill<0) wFill = 0;
            wCopy <<= 2;
            wFill <<= 2;
            for (y=0; y<yCopy; y++)
            {
                memcpy(pD, pS, wCopy);
                if (wFill)
                {
                    memset(pD+wCopy, 0, wFill);
                }
                pS += (info.iWidth*4);
                pD += (width*4);
            }
            for (y=0; y<ySkip; y++)
            {
                memset(pD, 0, width*4);
                pD += (width*4);
            }
            RtCoreMem().Free(data.pData);
            data.pData = pDest;
            info.iWidth = width;
            info.iHeight = height;
        }
    }
}

void RtgImage::Resize(int width, int height)
{
    if (m_bLoad && !info.bDDS)
    {
        if (info.iWidth!=width || info.iHeight!=height)
        {
            int iSize = width*height*4;
            unsigned char* pDest = (unsigned char*)RtCoreMem().Alloc(iSize, __FILE__, __LINE__);
            rtgiScale2D(data.pData, info.iWidth, info.iHeight, pDest, width, height, RtgImage::EST_LINEAR);
            RtCoreMem().Free(data.pData);
            data.pData = pDest;
            info.iWidth = width;
            info.iHeight = height;
            info.iSizeOfData = iSize;
        }
    }
}

void RtgImage::FixTextureQuality(bool bQualityManage)
{
    if (!m_bLoad)
    {
        return;
    }

    int iTexWidth, iTexHeight;

    iTexWidth = info.iWidth;
    if ( !IsPow2(iTexWidth) )
    {
        iTexWidth = GetPow2Less(iTexWidth);
    }
    if ( iTexWidth > m_iMaxWidth )
        iTexWidth = m_iMaxWidth;

    iTexHeight = info.iHeight;
    if ( !IsPow2(iTexHeight) )
    {
        iTexHeight = GetPow2Less(iTexHeight);
    }
    if ( iTexHeight > m_iMaxHeight )
        iTexHeight = m_iMaxHeight;

    if (bQualityManage)
    {
        switch(m_iTextureQuality)
        {
        case 0:
            // do nothing
            break;
        case 1:
            if ( iTexHeight >= iTexWidth && iTexHeight > 32 )
                iTexHeight/=2;
            else if ( iTexWidth > 32 )
                iTexWidth/=2;
            break;
        case 2:
            if ( iTexWidth > 32 )
                iTexWidth/=2;
            if ( iTexHeight > 32 )
                iTexHeight/=2;
            break;
        case 3:
            if ( iTexHeight >= iTexWidth && iTexHeight > 32 )
                iTexHeight/=4;
            else if ( iTexWidth > 32 )
                iTexWidth/=4;
            break;
        case 4:
            if ( iTexWidth > 32 )
                iTexWidth/=4;
            if ( iTexHeight > 32 )
                iTexHeight/=4;
            break;
        case 5:
            if ( iTexHeight >= iTexWidth && iTexHeight > 32 )
                iTexHeight/=8;
            else if ( iTexWidth > 32 )
                iTexWidth/=8;
            break;
        case 6:
            if ( iTexWidth > 32 )
                iTexWidth/=8;
            if ( iTexHeight > 32 )
                iTexHeight/=8;
            break;
        }
    }

    if ( m_bSquare )
    {
        if ( iTexWidth < iTexHeight )
            iTexHeight = iTexWidth;
        else
            iTexWidth = iTexHeight;
    }
    // resize image if necessary
    Resize(iTexWidth, iTexHeight);
}

void *RtgImage::CreateBitmap(void* hDC)
{
    unsigned char   *Data;
    HBITMAP         hBitmap;

    unsigned char   *buff[sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD)];
    BITMAPINFO      *pBmpInfo = (BITMAPINFO*)buff;

    hBitmap = CreateCompatibleBitmap((HDC)hDC, info.iWidth, info.iHeight);
    if (hBitmap==NULL)
    {
        return NULL;
    }

    pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pBmpInfo->bmiHeader.biWidth  = info.iWidth;
    pBmpInfo->bmiHeader.biHeight = info.iHeight;
    pBmpInfo->bmiHeader.biPlanes = 1;
    pBmpInfo->bmiHeader.biCompression = 0;
    pBmpInfo->bmiHeader.biSizeImage = 0;
    pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
    pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
    pBmpInfo->bmiHeader.biClrUsed = 0;
    pBmpInfo->bmiHeader.biClrImportant = 0;

    Data = data.pData;
    pBmpInfo->bmiHeader.biBitCount = 24;
    //pBmpInfo->bmiHeader.biBitCount = 32;

    unsigned char* pDest = (unsigned char*)RtCoreMem().Alloc(info.iHeight*info.iWidth*3, __FILE__, __LINE__);
    Data = pDest;
    Rtgimg_Fill_R8G8B8(info.iWidth, info.iHeight, pDest, info.iWidth*3, data.pData, info.iWidth*4);

    SetDIBits((HDC)hDC, hBitmap, 0, info.iHeight, Data, pBmpInfo, DIB_RGB_COLORS);
    RtCoreMem().Free(pDest);

    return hBitmap;
}

bool RtgImage::SaveToBMP(const char* szFilename)
{
    return rtgiSaveImageBMPFile(szFilename, this);
}

bool RtgImage::SaveToTGA(const char* szFilename)
{
    return rtgiSaveImageTGAFile(szFilename, this);
}

bool RtgImage::SaveToJPG(const char* szFilename)
{
    return rtgiSaveImageJPGFile(szFilename, this);
}

bool RtgImage::SaveToArchiveBMP(RtArchive* pArc)
{
    return rtgiSaveImageBMP(pArc, this);
}

bool RtgImage::SaveToArchiveTGA(RtArchive* pArc)
{
    return rtgiSaveImageTGA(pArc, this);
}

bool RtgImage::SaveToArchiveJPG(RtArchive* pArc)
{
    return rtgiSaveImageJPG(pArc, this);
}

void Rtgimg_Fill_A8R8G8B8(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*4;
	int add1 = srcpitch - width*4;

	//float maxlen = sqrt(height*height + width*width);
	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			bdest[0] = bsrc[2];
			bdest[1] = bsrc[1];
			bdest[2] = bsrc[0];
			bdest[3] = bsrc[3];
			bdest += 4;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_X8R8G8B8(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*4;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			bdest[0] = bsrc[2]; // b
			bdest[1] = bsrc[1]; // g
			bdest[2] = bsrc[0]; // r
			bdest[3] = bsrc[3]; // a
			bdest += 4;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_R8G8B8(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
    if (!dest || !src)
        return;

	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*3;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			bdest[0] = bsrc[2];
			bdest[1] = bsrc[1];
			bdest[2] = bsrc[0];
			bdest += 3;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_A4R4G4B4(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
    int a, r, g, b, x, y;
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*2;
	int add1 = srcpitch - width*4;

	for ( y=0; y<height; y++ )
	{
		for ( x=0; x<width; x++ )
		{
			a = bsrc[3] >> 4;
			r = bsrc[0] >> 4;
			g = bsrc[1] >> 4;
			b = bsrc[2] >> 4;

			*(unsigned short *) bdest = a << 12 | r << 8 | g << 4 | b;
			bdest += 2;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_A8R3G3B2(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*2;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			int a = bsrc[3];
			int r = bsrc[0] >> 5;
			int g = bsrc[1] >> 5;
			int b = bsrc[2] >> 6;
			*(unsigned short *) bdest = a << 8 | r << 5 | g << 2 | b;
			bdest += 2;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_A1R5G5B5(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*2;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			int a = bsrc[3] >> 7;
			int r = bsrc[0] >> 3;
			int g = bsrc[1] >> 3;
			int b = bsrc[2] >> 3;
			*(unsigned short *) bdest = a <<15 | r << 10 | g << 5 | b;
			bdest += 2;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_R5G6B5(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*2;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			int r = bsrc[0] >> 3;
			int g = bsrc[1] >> 2;
			int b = bsrc[2] >> 3;
			*(unsigned short *) bdest = r << 11 | g << 5 | b;
			bdest += 2;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_X1R5G5B5(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*2;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			int r = bsrc[0] >> 3;
			int g = bsrc[1] >> 3;
			int b = bsrc[2] >> 3;
			*(unsigned short *)bdest = r << 10 | g << 5 | b;
			bdest += 2;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

void Rtgimg_Fill_X4R4G4B4(int width, int height, void *dest, int destpitch, void *src, int srcpitch)
{
	unsigned char *bdest = (unsigned char *) dest;
	unsigned char *bsrc = (unsigned char *) src;

	int add0 = destpitch - width*2;
	int add1 = srcpitch - width*4;

	for ( int y=0; y<height; y++ )
	{
		for ( int x=0; x<width; x++ )
		{
			int r = bsrc[0] >> 4;
			int g = bsrc[1] >> 4;
			int b = bsrc[2] >> 4;
			*(unsigned short *)bdest = r << 8 | g << 4 | b;
			bdest += 2;
			bsrc += 4;
		}
		bdest += add0;
		bsrc += add1;
	}
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
