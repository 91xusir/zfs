
#include "rtg_graph_inter.h"
#include "rtg_image_bmp.h"
namespace rt_graph {

#pragma pack(push, bmp_struct, 1)
struct SBMP_HEAD
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
#pragma pack(pop, bmp_struct)

void GetShiftFromMask(int Mask, DWORD *ShiftLeft, DWORD *ShiftRight)
{
    int Temp, i;

    if (Mask == 0)
    {
        *ShiftLeft = *ShiftRight = 0;
        return;
    }

    Temp = Mask;
    for (i = 0; i < 32; i++, Temp >>= 1)
    {
        if (Temp & 1)
            break;
    }
    *ShiftRight = i;

    // Temp is preserved, so use it again:
    for (i = 0; i < 8; i++, Temp >>= 1)
    {
        if (!(Temp & 1))
            break;
    }
    *ShiftLeft = 8 - i;

    return;
}

// Reads an uncompressed .bmp
//	One of the absolute ugliest functions I've ever written!
bool rtgiBmpReadUncomp(SBMP_HEAD *pHeader, RtArchive* pArc, RtgImage* pImage, unsigned char* pPalBuf)
{
    int i;
    DWORD Read;
    DWORD aMask, rMask, gMask, bMask; //required for bitfields packing
    DWORD aShiftR, rShiftR, gShiftR, bShiftR; //required for bitfields packing
    DWORD aShiftL, rShiftL, gShiftL, bShiftL; //required for bitfields packing

    if (pHeader->biBitCount==8)
    {
        int iPalSize = 256 * 4;
        if (pHeader->biBitCount==4)
        {
            iPalSize = 16 * 4;
        }

        pArc->Seek(sizeof(SBMP_HEAD), RtArchive::SeekBegin);

        if (pArc->Serialize(pPalBuf, iPalSize)!=iPalSize)
        {
            RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
            return false;
        }
    }

    // We have to handle 1 and 4-bit images separately, because we have to expand them.
    switch (pHeader->biBitCount)
    {
    case 8: // è°ƒè‰²æ?
    case 24:
        {
            //for 8 and 24 bit, Bps is equal to the bmps bps
            pArc->Seek(pHeader->bfDataOff, RtArchive::SeekBegin);
            int Bps = (pHeader->biWidth*pHeader->biBitCount)>>3;

            int PadSize = (4 - (Bps % 4)) % 4;
            if (PadSize == 0)
            {
                if (pArc->Serialize(pImage->data.pData, pImage->info.iSizeOfData)!=pImage->info.iSizeOfData)
                {
                    RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
                    return false;
                }
            }else
            {
                for (i = 0; i<pImage->info.iSizeOfData; i += Bps)
                {
                    if (pArc->Serialize(pImage->data.pData+i, Bps)!=Bps)
                    {
                        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
                        return false;
                    }
                    pArc->Seek(PadSize, RtArchive::SeekCurrent);
                }
            }
        }
        break;

    case 32: 
        {
            pArc->Seek(pHeader->bfDataOff, RtArchive::SeekBegin);

            //32bit files are always padded to 4 byte...
            //check if bitfield compression is used
            aMask = 0xFF000000;
            rMask = 0x00FF0000;
            gMask = 0x0000FF00;
            bMask = 0x000000FF;
            aShiftR = 24;
            rShiftR = 16;
            gShiftR = 8;
            bShiftR = 0;
            aShiftL = 0;
            rShiftL = 0;
            gShiftL = 0;
            bShiftL = 0;
            if(pHeader->biCompression == 3) //bitfields
            {
                pArc->Seek(pHeader->bfDataOff-12, RtArchive::SeekBegin); //seek to bitfield data
                pArc->Serialize(&rMask, 4);
                pArc->Serialize(&gMask, 4);
                pArc->Serialize(&bMask, 4);
                GetShiftFromMask(rMask, &rShiftL, &rShiftR);
                GetShiftFromMask(gMask, &gShiftL, &gShiftR);
                GetShiftFromMask(bMask, &bShiftL, &bShiftR);
            }

            for(i = 0; i < pImage->info.iSizeOfData; i += 4)
            {
                if (pArc->Serialize(&Read, 4) != 4)
                {
                    return false;
                }
                pImage->data.pData[i    ] = ((Read & rMask) >> rShiftR) << rShiftL;
                pImage->data.pData[i + 1] = ((Read & gMask) >> gShiftR) << gShiftL;
                pImage->data.pData[i + 2] = ((Read & bMask) >> bShiftR) << bShiftL;
                pImage->data.pData[i + 3] = ((Read & aMask) >> aShiftR) << aShiftL;
            }
        }
        break;

    default:
        return false; //shouldn't happen, we checked that before
    }

    return true;
}

bool rtgiLoadImageBMP(RtArchive* pArc, RtgImage* pImage,bool bflip)
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

    SBMP_HEAD bmpHead;
    if (pArc->Serialize(&bmpHead, sizeof(SBMP_HEAD))!=sizeof(SBMP_HEAD))
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
        return false;
    }

    if ((bmpHead.bfType != ('B'|('M'<<8))) || (bmpHead.biSize != 0x28))
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
        return false;
    }

    if (bmpHead.biHeight == 0 || bmpHead.biWidth < 1)
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_UNSUPPORT;
        return false;
    }
    if ((bmpHead.biHeight&0x03)!=0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_WIDTH;
        return false;
    }
    if (bmpHead.biPlanes != 1)
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
        return false;
    }
    // BI_RGB 0, BI_RLE8 1, BI_RLE4 2, BI_BITFIELDS 3, BI_JPEG 4, BI_PNG 5
    if(bmpHead.biCompression != 0 && bmpHead.biCompression != 1 && bmpHead.biCompression != 2 && bmpHead.biCompression != 3)
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_UNSUPPORT;
        return false;
    }
    if(bmpHead.biCompression == 3 && bmpHead.biBitCount != 32)
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_UNSUPPORT;
        return false;
    }
    if (bmpHead.biBitCount != 8 && bmpHead.biBitCount != 24 && bmpHead.biBitCount != 32)
    {
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_UNSUPPORT;
        return false;
    }

    int Bpp = bmpHead.biBitCount >> 3; // Convert to bytes per pixel

    pImage->info.bAlpha      = Bpp==4 ? true : false;
    pImage->info.bDDS        = false;
    pImage->info.iWidth      = bmpHead.biWidth;
    pImage->info.iHeight     = abs(bmpHead.biHeight);
    pImage->info.iSizeOfData = pImage->info.iWidth * Bpp * pImage->info.iHeight;
    pImage->info.iDxtcFormat = 0;

    static unsigned char pPalBuf[256*4]; // only for 256 color

    int iDataSize = pImage->info.iWidth * pImage->info.iHeight * 4; // è¯»åˆ°å†…å­˜ä¸­çš„è´´å›¾å¿…é¡»ä¸?2ä½?
    pImage->data.pData = (unsigned char*)RtCoreMem().Alloc(iDataSize, __FILE__, __LINE__);

    if (pImage->data.pData==0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_MEMORY_ERR;
        return false;
    }

    switch (bmpHead.biCompression)
    {
    case 0:	// No compression
    case 3:	// BITFIELDS compression is handled in 16 bit
        // and 32 bit code in rtgiBmpReadUncomp()
        if (!rtgiBmpReadUncomp(&bmpHead, pArc, pImage, pPalBuf))
        {
            RtCoreMem().Free(pImage->data.pData);
            pImage->data.pData = NULL;
            RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_ERR;
            return false;
        }
        break;
    case 1:  //	RLE 8-bit / pixel (BI_RLE4)
    case 2:  // RLE 4-bit / pixel (BI_RLE8)
    default:
        RtCoreMem().Free(pImage->data.pData);
        pImage->data.pData = NULL;
        RtgImage::m_eLastError = RtgImage::ERI_BMP_FILE_FORMAT_UNSUPPORT;
        return false;
    }

    // é¢œè‰²ç¿»è½¬ï¼Œé»˜è®¤é¢œè‰²ä¸ºBGRï¼Œè¦ç¿»è½¬æˆRGB
    unsigned char *pSrc, *pDest;
    if (Bpp==1) // 8 bite, 256 color
    {
        unsigned char* pP;
        pSrc  = pImage->data.pData + pImage->info.iWidth * pImage->info.iHeight - 1;
        pDest = pImage->data.pData + iDataSize - 4;
        for (; pSrc>=pImage->data.pData; pSrc--,pDest-=4)
        {
            pP = pPalBuf + (*pSrc)*4;
            pDest[0] = pP[2];
            pDest[1] = pP[1];
            pDest[2] = pP[0];
            pDest[3] = pP[3];
        }
        pImage->info.iSizeOfData = iDataSize;
    }else if (Bpp==3)
    {
        pSrc = pImage->data.pData + pImage->info.iSizeOfData - 3;
        pDest = pImage->data.pData + iDataSize - 4;
        for (; pSrc>=pImage->data.pData; pSrc-=3,pDest-=4)
        {
            pDest[0] = pSrc[2];
            pDest[1] = pSrc[1];
            pDest[2] = pSrc[0];
            pDest[3] = 0xFF;
        }
        pImage->info.iSizeOfData = iDataSize;
    }

    // bmpHead.biHeight<0 Í¼ÏñµÄÎ»ÖÃÎª ×óÉÏ£¬·ñÔòÍ¼ÏñµÄÎ»ÖÃÎª ×óÏÂ
    // ×óÏÂÐèÒª·­×ª
    if (bmpHead.biHeight>0 && !bflip)
    {
        pImage->Flip(true);
    }

    RtgImage::m_eLastError = RtgImage::ERI_OK;
    return true;
}

bool rtgiSaveImageBMP(RtArchive* pArc, RtgImage* pImage)
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

    SBMP_HEAD bmpHead;

    bmpHead.bfType = ('B'|('M'<<8));
    bmpHead.bfSize = 0x0E;
    bmpHead.bfReserved = ('J'|('H'<<8)|('.'<<16)| (0<<24));
    bmpHead.bfDataOff = sizeof(SBMP_HEAD);

    bmpHead.biSize = 0x28;
    bmpHead.biWidth  = pImage->info.iWidth;
    bmpHead.biHeight = pImage->info.iHeight;
    bmpHead.biPlanes = 1;
    bmpHead.biBitCount = 24;
    bmpHead.biCompression = 0;
    bmpHead.biSizeImage = 0;
    bmpHead.biXPelsPerMeter = 0;
    bmpHead.biYPelsPerMeter = 0;
    bmpHead.biClrUsed = 0;
    bmpHead.biClrImportant = 0;

    if (pArc->Serialize(&bmpHead, sizeof(SBMP_HEAD))!=sizeof(SBMP_HEAD))
    {
        return false;
    }

    int iDataSize = pImage->info.iWidth * pImage->info.iHeight * 3;
    unsigned char* pData = (unsigned char*)RtCoreMem().Alloc(iDataSize, __FILE__, __LINE__);

    if (pData==0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_MEMORY_ERR;
        return false;
    }

    unsigned char *pSrc, *pDest;
    pDest = pData + iDataSize - 3;
    pSrc  = pImage->data.pData + pImage->info.iSizeOfData - 4;
    for (; pSrc>=pImage->data.pData; pSrc-=4,pDest-=3)
    {
        pDest[0] = pSrc[2];
        pDest[1] = pSrc[1];
        pDest[2] = pSrc[0];
    }

    if (pArc->Serialize(pData, iDataSize)!=iDataSize)
    {
        return false;
    }

    RtCoreMem().Free(pData);
    pData = NULL;

    RtgImage::m_eLastError = RtgImage::ERI_OK;
    return true;
}

bool rtgiLoadImageBMPFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiLoadImageBMP(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}


bool rtgiSaveImageBMPFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileWriter(NULL, szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiSaveImageBMP(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}


} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
