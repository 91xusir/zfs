
#include "rtg_graph_inter.h"
#include "rtg_image_tga.h"
namespace rt_graph {

#pragma pack(push, tga_struct, 1)

struct STargaHead
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

struct STargaFooter
{
    unsigned int    ExtOff;             // Extension Area Offset
    unsigned int    DevDirOff;          // Developer Directory Offset
    //char            Signature[16];      // TRUEVISION-XFILE
	CM_MEMDEF(Signature, 16)
    char            Reserved;           // ASCII period '.'
    char            NullChar;           // NULL

	STargaFooter()
	{
		CM_MEMPROTECTOR(Signature, 16)
	}
	STargaFooter(const STargaFooter &object)
	{
		memcpy(this, &object, sizeof(STargaFooter));
		CM_MEMPROTECTOR(Signature, 16)
	}
	~STargaFooter()
	{
		CM_MEMUNPROTECTOR(Signature)
	}
};

#pragma pack(pop, tga_struct)

#define TGA_EXT_LEN		495
struct STargaExt
{
    // Dev Directory
    //	We don't mess with this

    // Extension Area
    short	Size;				// should be TGA_EXT_LEN
    //char	AuthName[41];		// the image author's name
    //char	AuthComments[324];	// author's comments
	CM_MEMDEF(AuthName, 41)
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

	STargaExt()
	{
		CM_MEMPROTECTOR(AuthName, 41)
		CM_MEMPROTECTOR(AuthComments, 324)
		CM_MEMPROTECTOR(JobID, 41)
		CM_MEMPROTECTOR(SoftwareID, 41)
	}
	STargaExt(const STargaExt &object)
	{
		memcpy(this, &object, sizeof(STargaExt));
		CM_MEMPROTECTOR(AuthName, 41)
		CM_MEMPROTECTOR(AuthComments, 324)
		CM_MEMPROTECTOR(JobID, 41)
		CM_MEMPROTECTOR(SoftwareID, 41)
	}
	~STargaExt()
	{
		CM_MEMUNPROTECTOR(AuthName)
		CM_MEMUNPROTECTOR(AuthComments)
		CM_MEMUNPROTECTOR(JobID)
		CM_MEMUNPROTECTOR(SoftwareID)
	}
};

// Different Targa formats
#define TGA_NO_DATA				0
#define TGA_COLMAP_UNCOMP		1
#define TGA_UNMAP_UNCOMP		2
#define TGA_BW_UNCOMP			3
#define TGA_COLMAP_COMP			9
#define TGA_UNMAP_COMP			10
#define TGA_BW_COMP				11


// Targa origins
#define IMAGEDESC_ORIGIN_MASK	0x30
#define IMAGEDESC_TOPLEFT		0x20
#define IMAGEDESC_BOTLEFT		0x00
#define IMAGEDESC_BOTRIGHT		0x10
#define IMAGEDESC_TOPRIGHT		0x30


bool rtgiTgaUncompressData(STargaHead *Header, RtArchive* pArc, RtgImage* pImage)
{
    int BytesRead = 0;
    unsigned int    RunLen, i;
    unsigned char   RunLineHeader, Color[4];
    int             c;
    int             Bpp = Header->Bpp >> 3;

    while (BytesRead < pImage->info.iSizeOfData)
    {
        if (pArc->Serialize(&RunLineHeader, 1)!=1)
        {
            return false;
        }
        if (RunLineHeader & 0x80)
        {
            RunLineHeader &= ~(0x80);
            if (pArc->Serialize(&Color, Bpp)!=Bpp)
            {
                return false;
            }
            RunLen = (RunLineHeader+1) * Bpp;
            for (i = 0; i < RunLen; i += Bpp)
            {
                for (c = 0; c < Bpp; c++)
                {
                    pImage->data.pData[BytesRead+i+c] = Color[c];
                }
            }
            BytesRead += RunLen;
        }else
        {
            RunLen = (RunLineHeader+1) * Bpp;
            if (pArc->Serialize(pImage->data.pData + BytesRead, RunLen)!=RunLen)
            {
                return false;
            }
            BytesRead += RunLen;
        }
    }
    return true;
}


bool rtgiTgaReadRunmap(STargaHead *Header, RtArchive* pArc, RtgImage* pImage)
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
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
        return false;
    }

    if (Header->ImageType == TGA_UNMAP_COMP)
    {
        if (!rtgiTgaUncompressData(Header, pArc, pImage))
        {
            RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
            return false;
        }
    }else
    {
        if (pArc->Serialize(pImage->data.pData, pImage->info.iSizeOfData)!=pImage->info.iSizeOfData)
        {
            RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
            return false;
        }
    }

    return true;
}

bool rtgiTgaReadBw(STargaHead *Header, RtArchive* pArc, RtgImage* pImage)
{
    if (Header->ImageType == TGA_BW_COMP)
    {
        if (!rtgiTgaUncompressData(Header, pArc, pImage))
        {
            RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
            return false;
        }
    }else
    {
        if (pArc->Serialize(pImage->data.pData, pImage->info.iSizeOfData)!=pImage->info.iSizeOfData)
        {
            RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
            return false;
        }
    }

    return true;
}

bool rtgiLoadImageTGA(RtArchive* pArc, RtgImage* pImage)
{
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    if (pImage==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_IMAGE_IS_NULL;
        return false;
    }

    STargaHead tgaHead;
    char ID[255];
    if (pArc->Serialize(&tgaHead, sizeof(STargaHead))!=sizeof(STargaHead))
    {
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
        return false;
    }

    if (tgaHead.Width == 0 || tgaHead.Height == 0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
        return false;
    }
    if (tgaHead.Bpp != 24 && tgaHead.Bpp != 32)
    {
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_UNSUPPORT;
        return false;
    }
    if (tgaHead.ImageDesc & 0x10)  // Supposed to be set to 0
    {
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_UNSUPPORT;
        return false;
    }

    if(tgaHead.ImageType != TGA_NO_DATA
        && tgaHead.ImageType != TGA_COLMAP_UNCOMP
        && tgaHead.ImageType != TGA_UNMAP_UNCOMP
        && tgaHead.ImageType != TGA_BW_UNCOMP
        && tgaHead.ImageType != TGA_COLMAP_COMP
        && tgaHead.ImageType != TGA_UNMAP_COMP
        && tgaHead.ImageType != TGA_BW_UNCOMP)
    {
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_UNSUPPORT;
        return false;
    }

    if (pArc->Serialize(ID, tgaHead.IDLen)!=tgaHead.IDLen)
    {
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
        return false;
    }

    int Bpp = tgaHead.Bpp >> 3; // Convert to bytes per pixel
    pImage->info.bAlpha      = Bpp==4 ? true : false;
    pImage->info.bDDS        = false;
    pImage->info.iWidth      = tgaHead.Width;
    pImage->info.iHeight     = abs(tgaHead.Height);
    pImage->info.iSizeOfData = pImage->info.iWidth * pImage->info.iHeight * Bpp;
    pImage->info.iDxtcFormat = 0;

    int iDataSize = pImage->info.iWidth * pImage->info.iHeight * 4; // 读到内存中的贴图必须为32位
    pImage->data.pData = (unsigned char*)RtCoreMem().Alloc(iDataSize, __FILE__, __LINE__);

    if (pImage->data.pData==0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_MEMORY_ERR;
        return false;
    }

    switch (tgaHead.ImageType)
    {
    case TGA_UNMAP_UNCOMP:
    case TGA_UNMAP_COMP:
        if (!rtgiTgaReadRunmap(&tgaHead, pArc, pImage))
        {
            RtCoreMem().Free(pImage->data.pData);
            pImage->data.pData = NULL;
            RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
            return false;
        }
        break;
    case TGA_BW_UNCOMP:
    case TGA_BW_COMP:
        if (!rtgiTgaReadBw(&tgaHead, pArc, pImage))
        {
            RtCoreMem().Free(pImage->data.pData);
            pImage->data.pData = NULL;
            RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_ERR;
            return false;
        }
        break;
    case TGA_COLMAP_UNCOMP:
    case TGA_COLMAP_COMP:
    case TGA_NO_DATA:
    default:
        RtCoreMem().Free(pImage->data.pData);
        pImage->data.pData = NULL;
        RtgImage::m_eLastError = RtgImage::ERI_TGA_FILE_FORMAT_UNSUPPORT;
        return false;
    }

    // 颜色翻转, TGA的颜色模式为BGR或者BGRA，这里要转成RGB或者RGBA
    if (Bpp==3)
    {
        unsigned char* pSrc = pImage->data.pData + pImage->info.iSizeOfData - 3;
        unsigned char* pDest = pImage->data.pData + iDataSize - 4;
        for (; pSrc>=pImage->data.pData; pSrc-=3,pDest-=4)
        {
            pDest[0] = pSrc[2];
            pDest[1] = pSrc[1];
            pDest[2] = pSrc[0];
            pDest[3] = 0xFF;
        }
        pImage->info.iSizeOfData = iDataSize;
    }else if (Bpp==4)
    {
        unsigned char cTmp;
        unsigned char* pSrc = pImage->data.pData + pImage->info.iSizeOfData - 4;
        for (; pSrc>=pImage->data.pData; pSrc-=4)
        {
            cTmp = pSrc[2];
            pSrc[2] = pSrc[0];
            pSrc[0] = cTmp;
            //pSrc[3] = 0xFF;
        }
        pImage->info.iSizeOfData = iDataSize;
    }

    // 翻转，这里要把相应的类型翻转成左上的类型
    unsigned char iOrigin = tgaHead.ImageDesc & IMAGEDESC_ORIGIN_MASK;

    switch (iOrigin)
    {
    case IMAGEDESC_TOPLEFT: // 无需翻转
        break;

    case IMAGEDESC_TOPRIGHT:
        pImage->Mirror(true);
        break;

    case IMAGEDESC_BOTLEFT:
        pImage->Flip(true);
        break;

    case IMAGEDESC_BOTRIGHT:
        pImage->Flip(true);
        pImage->Mirror(true);
        break;
    }

    RtgImage::m_eLastError = RtgImage::ERI_OK;
    return true;
}

bool rtgiSaveImageTGA(RtArchive* pArc, RtgImage* pImage)
{
    return false;
}

bool rtgiLoadImageTGAFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiLoadImageTGA(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}


bool rtgiSaveImageTGAFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileWriter(NULL, szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiSaveImageTGA(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
