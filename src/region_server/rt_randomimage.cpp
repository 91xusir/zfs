
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "rt_randomimage.h"

//////////////////////////////////////////////////////////////////////////


#pragma pack(push, tga_struct, 1)

struct SRandomImageTargaHead
{
    unsigned char   IDLen;
    unsigned char   ColMapPresent;
    unsigned char   ImageType;
    short           FirstEntry;
    short           ColMapLen;
    unsigned char   ColMapEntSize;

    short           OriginX;
    short           OriginY;
    unsigned short  Width;
    unsigned short  Height;
    unsigned char   Bpp;
    unsigned char   ImageDesc;
};

struct SRandomImageTargaFooter
{
    unsigned int    ExtOff;             // Extension Area Offset
    unsigned int    DevDirOff;          // Developer Directory Offset
    //char            Signature[16];      // TRUEVISION-XFILE
	CM_MEMDEF(Signature, 16)
    char            Reserved;           // ASCII period '.'
    char            NullChar;           // NULL

	SRandomImageTargaFooter(void)
	{
		CM_MEMPROTECTOR(Signature, 16)
	}
	~SRandomImageTargaFooter(void)
	{
		CM_MEMUNPROTECTOR(Signature)
	}
};

struct SRandomImageBmpHead
{
    unsigned short  bfType;
    int             bfSize;
    unsigned int    bfReserved;
    int             bfDataOff;
    int             biSize;
    int             biWidth;
    int             biHeight;
    short           biPlanes;
    short           biBitCount;
    int             biCompression;
    int             biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    int             biClrUsed;
    int             biClrImportant;
};

#pragma pack(pop, tga_struct)

#define TGA_RANDOM_IMAGE_EXT_LEN    495
struct SRandomImageTargaExt
{
    // Dev Directory
    //	We don't mess with this

    // Extension Area
    short	Size;				// should be TGA_RANDOM_IMAGE_EXT_LEN
    //char	AuthName[41];		// the image author's name
	CM_MEMDEF(AuthName, 41)
    //char	AuthComments[324];	// author's comments
	CM_MEMDEF(AuthComments, 324)
    short	Month, Day, Year, Hour, Minute, Second;	// internal date of file
    //char	JobID[41];			// the job description (if any)
	CM_MEMDEF(JobID, 41)
    short	JobHour, JobMin, JobSecs;	// the job's time
    //char	SoftwareID[41];		// the software that created this
	CM_MEMDEF(SoftwareID, 41)
    short	SoftwareVer;		// the software version number * 100
    char	SoftwareVerByte;	// the software version letter
    int	KeyColor;			// the transparent colour

	SRandomImageTargaExt(void)
	{
		CM_MEMPROTECTOR(AuthName, 41)
		CM_MEMPROTECTOR(AuthComments, 324)
		CM_MEMPROTECTOR(JobID, 41)
		CM_MEMPROTECTOR(SoftwareID, 41)
	}
	~SRandomImageTargaExt(void)
	{
		CM_MEMUNPROTECTOR(AuthName)
		CM_MEMUNPROTECTOR(AuthComments)
		CM_MEMUNPROTECTOR(JobID)
		CM_MEMUNPROTECTOR(SoftwareID)
	}
};

// Different Targa formats
#define TGA_RANDOM_IMAGE_NO_DATA				0
#define TGA_RANDOM_IMAGE_COLMAP_UNCOMP		1
#define TGA_RANDOM_IMAGE_UNMAP_UNCOMP		2
#define TGA_RANDOM_IMAGE_BW_UNCOMP			3
#define TGA_RANDOM_IMAGE_COLMAP_COMP			9
#define TGA_RANDOM_IMAGE_UNMAP_COMP			10
#define TGA_BW_COMP				11


// Targa origins
#define RANDOM_IMAGE_IMAGEDESC_ORIGIN_MASK	0x30
#define RANDOM_IMAGE_IMAGEDESC_TOPLEFT		0x20
#define RANDOM_IMAGE_IMAGEDESC_BOTLEFT		0x00
#define RANDOM_IMAGE_IMAGEDESC_BOTRIGHT		0x10
#define RANDOM_IMAGE_IMAGEDESC_TOPRIGHT		0x30


bool rtgiRandomImageTgaUncompressData(SRandomImageTargaHead *Header, RtArchive* pArchive, unsigned char* pData, int iSizeOfData)
{
    int BytesRead = 0;
    unsigned int    RunLen, i;
    unsigned char   RunLineHeader, Color[4];
    int             c;
    int             Bpp = Header->Bpp >> 3;

    while (BytesRead < iSizeOfData)
    {
        pArchive->Serialize(&RunLineHeader, 1);
        if (RunLineHeader & 0x80)
        {
            RunLineHeader &= ~(0x80);
            pArchive->Serialize(&RunLineHeader, Bpp);
            RunLen = (RunLineHeader+1) * Bpp;
            for (i = 0; i < RunLen; i += Bpp)
            {
                for (c = 0; c < Bpp; c++)
                {
                    pData[BytesRead+i+c] = Color[c];
                }
            }
            BytesRead += RunLen;
        }else
        {
            RunLen = (RunLineHeader+1) * Bpp;
            pArchive->Serialize(pData + BytesRead, RunLen);
            BytesRead += RunLen;
        }
    }
    return true;
}


bool rtgiRandomImageTgaReadRunmap(SRandomImageTargaHead *Header, RtArchive* pArchive, unsigned char* pData, int iSizeOfData)
{
    unsigned char Bpp = Header->Bpp >> 3; // Convert to bytes per pixel
    switch (Bpp)
    {
    case 3:
        //iCurImage->Format = IL_BGR;
        break;
    case 4:
        //iCurImage->Format = IL_BGRA;
        break;
    case 1:
    case 2: // 16-bit is not supported directly!
    default:
        return false;
    }

    if (Header->ImageType == TGA_RANDOM_IMAGE_UNMAP_COMP)
    {
        if (!rtgiRandomImageTgaUncompressData(Header, pArchive, pData, iSizeOfData))
        {
            return false;
        }
    }else
    {
        pArchive->Serialize(pData, iSizeOfData);
    }

    return true;
}

bool rtgiRandomImageTgaReadBw(SRandomImageTargaHead *Header, RtArchive* pArchive, unsigned char* pData, int iSizeOfData)
{
    if (Header->ImageType == TGA_BW_COMP)
    {
        if (!rtgiRandomImageTgaUncompressData(Header, pArchive, pData, iSizeOfData))
        {
            return false;
        }
    }else
    {
        pArchive->Serialize(pData, iSizeOfData);
    }

    return true;
}

// returen改成unsigned char
unsigned char* rtgiRandomImageLoadImageTGA(RtArchive* pArchive, int& iWidth, int& iHeight)
{
    if (pArchive==NULL)
    {
        return NULL;
    }

    SRandomImageTargaHead tgaHead;
    char ID[255];

    pArchive->Serialize(&tgaHead, sizeof(SRandomImageTargaHead));
    if (tgaHead.Width == 0 || tgaHead.Height == 0)
    {
        return NULL;
    }
    if (tgaHead.Bpp != 24 && tgaHead.Bpp != 32)
    {
        return NULL;
    }
    if (tgaHead.ImageDesc & 0x10)  // Supposed to be set to 0
    {
        return NULL;
    }

    if(tgaHead.ImageType != TGA_RANDOM_IMAGE_NO_DATA
        && tgaHead.ImageType != TGA_RANDOM_IMAGE_COLMAP_UNCOMP
        && tgaHead.ImageType != TGA_RANDOM_IMAGE_UNMAP_UNCOMP
        && tgaHead.ImageType != TGA_RANDOM_IMAGE_BW_UNCOMP
        && tgaHead.ImageType != TGA_RANDOM_IMAGE_COLMAP_COMP
        && tgaHead.ImageType != TGA_RANDOM_IMAGE_UNMAP_COMP
        && tgaHead.ImageType != TGA_RANDOM_IMAGE_BW_UNCOMP)
    {
        return NULL;
    }

    pArchive->Serialize(ID, tgaHead.IDLen);

    int Bpp = tgaHead.Bpp >> 3; // Convert to bytes per pixel
    if (Bpp!=4)
    {
        return NULL;
    }

    iWidth      = tgaHead.Width;
    iHeight     = abs(tgaHead.Height);
    int iSizeOfData = iWidth * iHeight * Bpp;
    int iDataSize = iWidth * iHeight * 4; // 读到内存中的贴图必须为32位
    unsigned char* pData = RT_NEW unsigned char[iDataSize];

    if (pData==0)
    {
        return NULL;
    }

    switch (tgaHead.ImageType)
    {
    case TGA_RANDOM_IMAGE_UNMAP_UNCOMP:
    case TGA_RANDOM_IMAGE_UNMAP_COMP:
        if (!rtgiRandomImageTgaReadRunmap(&tgaHead, pArchive, pData, iSizeOfData))
        {
            delete[](pData);
            return NULL;
        }
        break;
    case TGA_RANDOM_IMAGE_BW_UNCOMP:
    case TGA_BW_COMP:
        if (!rtgiRandomImageTgaReadBw(&tgaHead, pArchive, pData, iSizeOfData))
        {
            delete[](pData);
            return NULL;
        }
        break;
    case TGA_RANDOM_IMAGE_COLMAP_UNCOMP:
    case TGA_RANDOM_IMAGE_COLMAP_COMP:
    case TGA_RANDOM_IMAGE_NO_DATA:
    default:
        delete[](pData);
        return NULL;
    }

    unsigned char iOrigin = tgaHead.ImageDesc & RANDOM_IMAGE_IMAGEDESC_ORIGIN_MASK;

    switch (iOrigin)
    {
    case RANDOM_IMAGE_IMAGEDESC_TOPLEFT:
        //iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
        break;

    case RANDOM_IMAGE_IMAGEDESC_TOPRIGHT:
        //iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
        //ilMirrorImage();
        break;

    case RANDOM_IMAGE_IMAGEDESC_BOTLEFT:
        //iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;
        break;

    case RANDOM_IMAGE_IMAGEDESC_BOTRIGHT:
        //iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;
        //ilMirrorImage();
        break;
    }

    if (Bpp==3)
    {
        unsigned char* pSrc = pData + iSizeOfData - 3;
        unsigned char* pDest = pData + iDataSize - 4;
        for (; pSrc>pData; pSrc-=3,pDest-=4)
        {
            pDest[0] = pSrc[2];
            pDest[1] = pSrc[1];
            pDest[2] = pSrc[0];
            pDest[3] = 0xFF;
        }
        iSizeOfData = iDataSize;
    }else if (Bpp==4)
    {
        unsigned char cTmp;
        unsigned char* pSrc = pData + iSizeOfData - 4;
        for (; pSrc>pData; pSrc-=4)
        {
            cTmp = pSrc[2];
            pSrc[2] = pSrc[0];
            pSrc[0] = cTmp;
            //pSrc[3] = 0xFF;
        }
        iSizeOfData = iDataSize;
    }

    return pData;
}


//////////////////////////////////////////////////////////////////////////

RtRandomImage::RtRandomImage()
{
    m_iCharCnt = 0;
    m_szCharList = NULL;
    m_iCharPos = NULL;
    m_iCharWidth = NULL;
    m_pAlphabet = NULL;
    m_iAlphabetImgWidth = 0;
    m_iAlphabetImgHeight = 0;
    m_pTempBmp = NULL;
    m_iTempWidth = 0;
    m_iTempHeight = 0;
    m_b32bit = false;
    m_dwBackgroundColor = 0x00FFFFFF;
    m_dwTextColor = 0x00000000;
}

RtRandomImage::~RtRandomImage()
{
    m_iCharCnt = 0;
    delete[] m_szCharList;
    delete[] m_iCharPos;
    delete[] m_iCharWidth;
    delete[] m_pAlphabet;
    delete[] m_pTempBmp;
}

bool RtRandomImage::Init(const char* szTgaAlphabet, int iCharCnt, int* iCharPos, int* iCharWidth, char* szCharList, int* iVaildFlag)
{
    if (iCharCnt<=0 || szTgaAlphabet==NULL)
    {
        return false;
    }
    int i;

    m_iCharCnt = iCharCnt;

    m_iCharWidth = RT_NEW int[m_iCharCnt];
    m_iCharPos = RT_NEW int[m_iCharCnt];
    m_szCharList = RT_NEW char[m_iCharCnt+1];
    memcpy(m_iCharWidth, iCharWidth, sizeof(int)*m_iCharCnt);
    memcpy(m_iCharPos, iCharPos, sizeof(int)*m_iCharCnt);
    memcpy(m_szCharList, szCharList, sizeof(char)*m_iCharCnt);
    m_szCharList[m_iCharCnt] = 0;

    for (i=0; i<256; i++)
    {
        m_pMap[i] = -1;
    }
    for (i=0; i<m_iCharCnt; i++)
    {
        if (iVaildFlag)
        {
            if (iVaildFlag[i]!=0)
            {
                m_pMap[m_szCharList[i]] = i;
            }
        }else
        {
            m_pMap[m_szCharList[i]] = i;
        }
    }

    RtArchive* pArchive = RtCoreFile().CreateFileReader(szTgaAlphabet);
    if (pArchive==NULL)
    {
        return false;
    }
    m_pAlphabet = rtgiRandomImageLoadImageTGA(pArchive, m_iAlphabetImgWidth, m_iAlphabetImgHeight);
    if (m_pAlphabet==NULL)
    {
        RtCoreFile().CloseFile(pArchive);
        return false;
    }
    RtCoreFile().CloseFile(pArchive);
    return true;
}

unsigned char* RtRandomImage::CreateImageBMP(const char* szStr, int iWidth, int iHeight, int* pDataSize)
{
    CreateImage24b(szStr, iWidth, iHeight);

    SRandomImageBmpHead* pBmpHead = (SRandomImageBmpHead*)m_pTempBmp;

    pBmpHead->bfType = ('B'|('M'<<8));
    pBmpHead->bfSize = 0x0E;
    pBmpHead->bfReserved = ('J'|('H'<<8)|('.'<<16)| (0<<24));
    pBmpHead->bfDataOff = sizeof(SRandomImageBmpHead);

    pBmpHead->biSize = 0x28;
    pBmpHead->biWidth  = iWidth;
    pBmpHead->biHeight = iHeight;
    pBmpHead->biPlanes = 1;
    pBmpHead->biBitCount = 24;
    pBmpHead->biCompression = 0;
    pBmpHead->biSizeImage = 0;
    pBmpHead->biXPelsPerMeter = 0;
    pBmpHead->biYPelsPerMeter = 0;
    pBmpHead->biClrUsed = 0;
    pBmpHead->biClrImportant = 0;

    if (pDataSize)
    {
        *pDataSize = sizeof(SRandomImageBmpHead)+iWidth*iHeight*3;
    }

    return m_pTempBmp;
}

bool RtRandomImage::CreateImageBMPToFile(const char* szFilename, const char* szStr, int iWidth, int iHeight)
{
    FILE* fp = fopen(szFilename, "wb");
    if (fp==NULL)
    {
        return false;
    }
    int iDataSize;
    unsigned char* pImgData = CreateImageBMP(szStr, iWidth, iHeight, &iDataSize);
    fwrite(pImgData, 1, iDataSize, fp);
    fclose(fp);
    return true;
}

void RtRandomImage::ClearBackground(unsigned long dwColor)
{
    int i, j;
    if (m_b32bit)
    {
        unsigned long* pData = (unsigned long*)m_pTempData;
        j = m_iCreateWidth * m_iCreateHeight;
        for (i=0; i<j; i++)
        {
            *pData = dwColor;
            pData ++;
        }
    }else
    {
        unsigned char* pData = m_pTempData;
        unsigned char r, g, b;
        r = (unsigned char)(dwColor>>16)&0x00FF;
        g = (unsigned char)(dwColor>>8)&0x00FF;
        b = (unsigned char)(dwColor)&0x00FF;
        j = m_iCreateWidth * m_iCreateHeight;
        for (i=0; i<j; i++)
        {
            pData[0] = r;
            pData[1] = g;
            pData[2] = b;
            pData += 3;
        }
    }
}

void RtRandomImage::DrawTextToImg(int x, int y, char c, unsigned long dwColor)
{
    if (m_pMap[c]<0)
    {
        return;
    }
    int idx = m_pMap[c];
    int iSx, iSy, iSMinX, iSMinY, iSMaxX, iSMaxY, iDMinX, iDMinY, iDMaxX, iDMaxY, iSL, iDL;
    unsigned char *pS, *pD;

    iSMinX = m_iCharPos[idx];
    iSMinY = 0;
    iSMaxX = m_iCharPos[idx]+m_iCharWidth[idx];
    iSMaxY = m_iAlphabetImgHeight;

    iDMinX = x;
    iDMinY = y;
    iDMaxX = x+m_iCharWidth[idx];
    iDMaxY = y+m_iAlphabetImgHeight;

    // 调整
    if (iDMinX<0)
    {
        iSMinX += -iDMinX;
        iDMinX = 0;
    }
    if (iDMinY<0)
    {
        iSMinY += -iDMinY;
        iDMinY = 0;
    }
    if (iDMaxX>m_iCreateWidth)
    {
        iSMaxX -= iDMaxX-m_iCreateWidth;
        iDMaxX = m_iCreateWidth;
    }
    if (iDMaxY>m_iCreateHeight)
    {
        iSMaxY -= iDMaxY-m_iCreateHeight;
        iDMaxY = m_iCreateHeight;
    }

    if (iSMinX>=iSMaxX || iSMinY>=iSMaxY)
    {
        return;
    }

    pS = m_pAlphabet + (iSMinY*m_iAlphabetImgWidth + iSMinX)*4;
    iSL = (m_iAlphabetImgWidth-(iSMaxX-iSMinX))*4;

    float fa;
    int r, g, b, dr, dg, db;
    r = (dwColor>>16)&0x00FF;
    g = (dwColor>>8)&0x00FF;
    b = (dwColor)&0x00FF;
    if (m_b32bit)
    {
        pD = m_pTempData + (iDMinY*m_iCreateWidth + iDMinX)*4;
        iDL = (m_iCreateWidth-(iSMaxX-iSMinX))*4;
        for (iSy=iSMinY; iSy<iSMaxY; iSy++)
        {
            for (iSx=iSMinX; iSx<iSMaxX; iSx++)
            {
                fa = pS[3]/255.f;
                dr = (int)(fa*r + (1.f-fa)*pD[2]);
                dg = (int)(fa*g + (1.f-fa)*pD[1]);
                db = (int)(fa*b + (1.f-fa)*pD[0]);
                if (dr>255) dr = 255;
                if (dg>255) dg = 255;
                if (db>255) db = 255;
                pD[3] = pS[3];
                pD[2] = dr;
                pD[1] = dg;
                pD[0] = db;
                pD += 4;
                pS += 4;
            }
            pS += iSL;
            pD += iDL;
        }
    }else
    {
        pD = m_pTempData + (iDMinY*m_iCreateWidth + iDMinX)*3;
        iDL = (m_iCreateWidth-(iSMaxX-iSMinX))*3;
        for (iSy=iSMinY; iSy<iSMaxY; iSy++)
        {
            for (iSx=iSMinX; iSx<iSMaxX; iSx++)
            {
                fa = pS[3]/255.f;
                dr = (int)(fa*r + (1.f-fa)*pD[0]);
                dg = (int)(fa*g + (1.f-fa)*pD[1]);
                db = (int)(fa*b + (1.f-fa)*pD[2]);
                if (dr>255) dr = 255;
                if (dg>255) dg = 255;
                if (db>255) db = 255;
                pD[0] = dr;
                pD[1] = dg;
                pD[2] = db;
                pD += 3;
                pS += 4;
            }
            pS += iSL;
            pD += iDL;
        }
    }
}

unsigned long* RtRandomImage::_CreateImage(const char* szStr, int iWidth, int iHeight, int* pDataSize, bool b32b)
{
    if (m_iTempWidth<iWidth || m_iTempHeight<iHeight)
    {
        m_iTempWidth = iWidth;
        m_iTempHeight = iHeight;
        delete[] m_pTempBmp;
        m_pTempBmp = RT_NEW unsigned char[m_iTempWidth*m_iTempHeight*4+sizeof(SRandomImageBmpHead)];
        m_pTempData = m_pTempBmp+sizeof(SRandomImageBmpHead);
    }

    m_b32bit = b32b;
    m_iCreateWidth = iWidth;
    m_iCreateHeight = iHeight;

    ClearBackground(m_dwBackgroundColor);

    static int sRand = 57829;
    int x, y, iTextWidth = 0;
    int i, iLen = (int)strlen(szStr);
    for (i=0; i<iLen; i++)
    {
        if (m_pMap[szStr[i]]!=-1)
        {
            iTextWidth += m_iCharWidth[m_pMap[szStr[i]]];
        }
    }

    x = (iWidth - iTextWidth)/2;
    if (x<0) x = 0;
    y = iHeight-m_iAlphabetImgHeight-8;
    if (y<1) y = 1;

    for (i=0; i<iLen; i++)
    {
        if (m_pMap[szStr[i]]!=-1)
        {
            sRand = ((sRand * 28586 + 85219835) / 8131)&0x7FFFFFFF;
            DrawTextToImg(x, (sRand%y)+4, szStr[i], m_dwTextColor);
            x += m_iCharWidth[m_pMap[szStr[i]]];
        }
    }

    if (pDataSize)
    {
        *pDataSize = m_iCreateWidth*m_iCreateHeight*(m_b32bit?4:3);
    }
    return (unsigned long*)m_pTempData;
}

unsigned char* RtRandomImage::CreateImage24b(const char* szStr, int iWidth, int iHeight, int* pDataSize)
{
    return (unsigned char*)_CreateImage(szStr, iWidth, iHeight, pDataSize, false);
}

unsigned long* RtRandomImage::CreateImage32b(const char* szStr, int iWidth, int iHeight, int* pDataSize)
{
    return _CreateImage(szStr, iWidth, iHeight, pDataSize, true);
}

unsigned long* RtRandomImage::CreateRandomImage32b(const char* szStr, int iWidth, int iHeight, int* pDataSize, int iTwistLev)
{
    unsigned long* pImage = CreateImage32b(szStr, iWidth, iHeight, pDataSize);

    if (iTwistLev)
    {
        int i, j, iLen, iHalf, iXChange[32], iYChange[32];

        switch (iTwistLev)
        {
        case 1:
            iLen = 5;
            break;
        default:
        case 2:
            iLen = 10;
            break;
        case 3:
            iLen = 16;
            break;
        case 4:
            iLen = 32;
            break;
        }
        iHalf = iLen/2;

        iXChange[0] = 0;
        iYChange[0] = 0;
        for (i=1; i<iHalf; i++)
        {
            iXChange[i] = iXChange[i-1]+(rand()%3)-1;
            iYChange[i] = iYChange[i-1]+(rand()%3)-1;
        }
        for (; i<iLen; i++)
        {
            if (abs(iXChange[i-1])>=(iLen-i))
            {
                if (iXChange[i-1]>0)
                {
                    iXChange[i] = iXChange[i-1]-1;
                }else
                {
                    iXChange[i] = iXChange[i-1]+1;
                }
            }else
            {
                j = (rand()%4)-1;
                if (j>1)
                {
                    if (iXChange[i-1]>0)
                    {
                        j = -1;
                    }else
                    {
                        j = 1;
                    }
                }
                iXChange[i] = iXChange[i-1]+j;
            }
            if (abs(iYChange[i-1])>=(iLen-i))
            {
                if (iYChange[i-1]>0)
                {
                    iYChange[i] = iYChange[i-1]-1;
                }else
                {
                    iYChange[i] = iYChange[i-1]+1;
                }
            }else
            {
                j = (rand()%4)-1;
                if (j>1)
                {
                    if (iYChange[i-1]>0)
                    {
                        j = -1;
                    }else
                    {
                        j = 1;
                    }
                }
                iYChange[i] = iYChange[i-1]+j;
            }
        }

        float fR = ((float)32) / iLen;
        for (i=31; i>0; i--)
        {
            iXChange[i] = iXChange[(int)(i/fR)];
            iYChange[i] = iYChange[(int)(i/fR)];
        }

        // X 变形
        for (i=0; i<32; i++)
        {
            if (iXChange[i]>0)
            {
                for (j=31; j>=iXChange[i]; j--)
                {
                    pImage[i*32+j] = pImage[i*32+j-iXChange[i]];
                }
                for (; j>=0; j--)
                {
                    pImage[i*32+j] = m_dwBackgroundColor;
                }
            }else if (iXChange[i]<0)
            {
                for (j=0; j<32+iXChange[i]; j++)
                {
                    pImage[i*32+j] = pImage[i*32+j-iXChange[i]];
                }
                for (; j<32; j++)
                {
                    pImage[i*32+j] = m_dwBackgroundColor;
                }
            }
        }

        // Y 变形
        for (i=0; i<32; i++)
        {
            if (iYChange[i]>0)
            {
                for (j=31; j>=iYChange[i]; j--)
                {
                    pImage[i+j*32] = pImage[i+(j-iYChange[i])*32];
                }
                for (; j>=0; j--)
                {
                    pImage[i+j*32] = m_dwBackgroundColor;
                }
            }else if (iYChange[i]<0)
            {
                for (j=0; j<32+iYChange[i]; j++)
                {
                    pImage[i+j*32] = pImage[i+(j-iYChange[i])*32];
                }
                for (; j<32; j++)
                {
                    pImage[i+j*32] = m_dwBackgroundColor;
                }
            }
        }
    }

    return pImage;
}

#ifdef _WIN32
#   include <assert.h>
#endif

unsigned long* RtRandomImage::AddNoise32b(unsigned long* pData, int iWidth, int iHeight, unsigned long dwColorKey, unsigned long dwColorText, unsigned long dwColorBG)
{
    int x, y;
    static int sRand = 854656;
    if (dwColorKey==dwColorText || dwColorKey==dwColorBG)
    {
#ifdef _WIN32
        assert(("错误", 0));
#endif
        return pData;
    }

    // 单色化，使颜色变成只有 dwColorBG 和 dwColorText (=dwColorKey的变dwColorBG，其他dwColorText)
    for (y=0; y<32; y++)
    {
        for (x=0; x<32; x++)
        {
            if (pData[y*32+x]==dwColorKey)
            {
                pData[y*32+x] = dwColorBG;
            }else
            {
                pData[y*32+x] = dwColorText;
            }
        }
    }


    /*
    // 添加杂色
    int c, d, e, f;
    for (c=0; c<20; c++)
    {
        sRand = ((sRand * 28586 + 85219835) / 8131)&0x7FFFFFFF;
        x = sRand%32;
        sRand = ((sRand * 28586 + 85219835) / 8131)&0x7FFFFFFF;
        y = sRand%32;
        if (y<0) y = -y;
        if (   pData[(y<<5)+x]==dwColorBG
            && pData[((y+1)<<5)+x]==dwColorBG
            && pData[((y-1)<<5)+x]==dwColorBG
            && pData[(y<<5)+x+1]==dwColorBG
            && pData[(y<<5)+x-1]==dwColorBG
            && pData[((y+2)<<5)+x]==dwColorBG
            && pData[((y-2)<<5)+x]==dwColorBG
            && pData[(y<<5)+x+2]==dwColorBG
            && pData[(y<<5)+x-2]==dwColorBG
            )
        {
            for (c=0; c<30; c++)
            {
                pData[(y<<5)+x] = dwColorText;

                sRand = ((sRand * 28586 + 85219835) / 8131)&0x7FFFFFFF;
                d = sRand%6;
                if (d>4) d -= 4;
                f = 0;
                for (e=0; e<4; e++)
                {
                    switch (d%4)
                    {
                    case 0:
                        if (pData[((y+2)<<5)+x]==dwColorBG)
                        {
                            y = y+1;
                            f = 1;
                        }
                        break;
                    case 1:
                        if (pData[((y-2)<<5)+x]==dwColorBG)
                        {
                            y = y-1;
                            f = 1;
                        }
                        break;
                    case 2:
                        if (pData[(y<<5)+x+2]==dwColorBG)
                        {
                            x = x+1;
                            f = 1;
                        }
                        break;
                    case 3:
                        if (pData[(y<<5)+x-2]==dwColorBG)
                        {
                            x = x-1;
                            f = 1;
                        }
                        break;
                    }
                    d ++;
                }
                if (f==0)
                {
                    break;
                }
            }
            break;
        }
    }
    */

    // 填充边缘，使dwColorText的周围有一圈dwColorKey的颜色
    unsigned long* pBuf;
    for (y=1; y<31; y++)
    {
        for (x=1; x<31; x++)
        {
            if (pData[y*32+x]==dwColorText)
            {
                pBuf = pData+(((y-1)<<5)+(x-1));
                if (*pBuf!=dwColorText)
                {
                    *pBuf = dwColorKey;
                }
                pBuf = pData+(((y-1)<<5)+(x+1));
                if (*pBuf!=dwColorText)
                {
                    *pBuf = dwColorKey;
                }
                pBuf = pData+(((y+1)<<5)+(x-1));
                if (*pBuf!=dwColorText)
                {
                    *pBuf = dwColorKey;
                }
                pBuf = pData+(((y+1)<<5)+(x+1));
                if (*pBuf!=dwColorText)
                {
                    *pBuf = dwColorKey;
                }
            }
        }
    }

    // 填充颜色，使dwColorBG的颜色变成dwColorText
    for (y=0; y<32; y++)
    {
        for (x=0; x<32; x++)
        {
            if (pData[y*32+x]==dwColorBG)
            {
                pData[y*32+x] = dwColorText;
            }else if (pData[y*32+x]==dwColorKey)
            {
                if ((sRand = ((sRand * 28586 + 85219835) / 8131)&0x7FFFFFFF)%20) // 1/20的机会去掉点
                {
                    pData[y*32+x] = dwColorBG;
                }else
                {
                    pData[y*32+x] = dwColorText;
                }
            }
        }
    }
    /*
    */
    return pData;
}

bool RtRandomImage::RandomString(char* szStrBuf, int iStrLen)
{
    int i = 0, iFind=100; // 最多只有100次的随机机会
    while (i<iStrLen)
    {
        szStrBuf[i] = rand()%('Z'-'A'+11)+'A';
        if (szStrBuf[i]>'Z') szStrBuf[i] = szStrBuf[i]-1-'Z'+'0';
        if (m_pMap[szStrBuf[i]]!=-1)
        {
            i++;
        }
        iFind --;
        if (iFind<=0)
        {
            szStrBuf[i] = 0;
            return false;
        }
    }
    szStrBuf[i] = 0;
    return true;
}

int RtRandomImage::RLE32bData(unsigned long* pRawData, int iRawDataSize, unsigned long dwColorKey, unsigned char* pRLEData, int iRLEBufSize)
{
    int iPos, iLen, iBufPos;
    unsigned long dwColor = dwColorKey;
    unsigned char cLine;

    cLine = 0;
    iLen = 0;
    iBufPos = 0;
    iRawDataSize /= 4;
    for (iPos=0; iPos<=iRawDataSize; iPos++)
    {
        if (iPos==iRawDataSize || dwColor!=pRawData[iPos] || iPos==0 || iLen>62)
        {
            if (iPos!=0)
            {
                if (iLen>7)
                {
                    pRLEData[iBufPos] = 0x80|(unsigned char)iLen;
                    if (dwColor!=dwColorKey)
                    {
                        pRLEData[iBufPos] |= 0x40;
                    }
                    iBufPos++;
                    if (iBufPos>=iRLEBufSize)
                    {
                        return -1;
                    }
                }else
                {
                    for (; iPos<iRawDataSize && iLen<7; iPos++, iLen++)
                    {
                        if (pRawData[iPos]!=dwColorKey)
                        {
                            cLine |= (0x01 << iLen);
                        }
                    }
                    pRLEData[iBufPos++] = cLine;
                    if (iBufPos>=iRLEBufSize)
                    {
                        return -1;
                    }
                }
            }
            if (iPos==iRawDataSize)
            {
                return iBufPos;
            }
            dwColor = pRawData[iPos];
            cLine = 0;
            iLen = 0;
        }
        if (iLen<7)
        {
            if (pRawData[iPos]!=dwColorKey)
            {
                cLine |= (0x01 << iLen);
            }
        }
        iLen ++;
    }
    return iBufPos;
}

bool RtRandomImage::DeRLE32bData(unsigned char* pRLEData, int iRLEDataSize, unsigned long dwBGColor, unsigned long dwTextColor, unsigned long* pRawData, int iRawDataSize)
{
    int i, j, iRawPos=0;
    iRawDataSize /= 4;

    for (i=0; i<iRLEDataSize; i++)
    {
        if (pRLEData[i]&0x80)
        {
            j = pRLEData[i]&0x3F;
            if (pRLEData[i]&0x40)
            {
                while (j>0)
                {
                    pRawData[iRawPos++] = dwTextColor;
                    if (iRawPos>=iRawDataSize) return false;
                    j --;
                }
            }else
            {
                while (j>0)
                {
                    pRawData[iRawPos++] = dwBGColor;
                    if (iRawPos>=iRawDataSize) return false;
                    j --;
                }
            }
        }else
        {
            for (j=0; j<7; j++)
            {
                if (pRLEData[i]& (0x01<<j))
                {
                    pRawData[iRawPos++] = dwTextColor;
                    if (iRawPos>=iRawDataSize) return true;
                }else
                {
                    pRawData[iRawPos++] = dwBGColor;
                    if (iRawPos>=iRawDataSize) return true;
                }
            }
        }
    }
    return true;
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
