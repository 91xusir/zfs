
#include "rtg_graph_inter.h"
#include "rtg_image_dds.h"
namespace rt_graph {

#define DDS_CAPS				0x00000001l
#define DDS_HEIGHT				0x00000002l
#define DDS_WIDTH				0x00000004l
#define DDS_PIXELFORMAT			0x00001000l

#define DDS_ALPHAPIXELS			0x00000001l
#define DDS_ALPHA				0x00000002l
#define DDS_FOURCC				0x00000004l
#define DDS_PITCH				0x00000008l
#define DDS_COMPLEX				0x00000008l
#define DDS_TEXTURE				0x00001000l
#define DDS_MIPMAPCOUNT			0x00020000l
#define DDS_LINEARSIZE			0x00080000l
#define DDS_VOLUME				0x00200000l
#define DDS_MIPMAP				0x00400000l
#define DDS_DEPTH				0x00800000l

#define DDS_CUBEMAP				0x00000200L
#define DDS_CUBEMAP_POSITIVEX	0x00000400L
#define DDS_CUBEMAP_NEGATIVEX	0x00000800L
#define DDS_CUBEMAP_POSITIVEY	0x00001000L
#define DDS_CUBEMAP_NEGATIVEY	0x00002000L
#define DDS_CUBEMAP_POSITIVEZ	0x00004000L
#define DDS_CUBEMAP_NEGATIVEZ	0x00008000L

int BlockSize = 0;


#define MY_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
    ((int)(char)(ch0) | ((int)(char)(ch1) << 8) |   \
    ((int)(char)(ch2) << 16) | ((int)(char)(ch3) << 24 ))

#pragma pack(push, dds_struct, 1)
struct SDDSHead
{
    char            Signature[4];

    unsigned int    Size1;				// size of the structure (minus MagicNum)
    unsigned int    Flags1; 			// determines what fields are valid
    unsigned int    Height; 			// height of surface to be created
    unsigned int    Width;				// width of input surface
    unsigned int    LinearSize; 		// Formless late-allocated optimized surface size
    unsigned int    Depth;				// Depth if a volume texture
    unsigned int    MipMapCount;		// number of mip-map levels requested
    unsigned int    AlphaBitDepth;		// depth of alpha buffer requested

    unsigned int    NotUsed[10];

    unsigned int    Size2;				// size of structure
    unsigned int    Flags2;				// pixel format flags
    unsigned int    FourCC;				// (FOURCC code)
    unsigned int    RGBBitCount;		// how many bits per pixel
    unsigned int    RBitMask;			// mask for red bit
    unsigned int    GBitMask;			// mask for green bits
    unsigned int    BBitMask;			// mask for blue bits
    unsigned int    RGBAlphaBitMask;	// mask for alpha channel

    unsigned int    ddsCaps1, ddsCaps2, ddsCaps3, ddsCaps4; // direct draw surface capabilities
    unsigned int    TextureStage;
};
#pragma pack(pop, dds_struct)

// use cast to struct instead of RGBA_MAKE as struct is
//  much
typedef struct Color8888
{
    unsigned char r;		// change the order of names to change the 
    unsigned char g;		//  order of the output ARGB or BGRA, etc...
    unsigned char b;		//  Last one is MSB, 1st is LSB.
    unsigned char a;
} Color8888;


typedef struct Color888
{
    unsigned char r;		// change the order of names to change the 
    unsigned char g;		//  order of the output ARGB or BGRA, etc...
    unsigned char b;		//  Last one is MSB, 1st is LSB.
} Color888;


typedef struct Color565
{
    unsigned nBlue  : 5;		// order of names changes
    unsigned nGreen : 6;		//	byte order of output to 32 bit
    unsigned nRed	: 5;
} Color565;


typedef struct DXTColBlock
{
    short col0;
    short col1;

    // no bit fields - use bytes
    //char row[4];
	CM_MEMDEF(row, 4)

	DXTColBlock()
	{
		CM_MEMPROTECTOR(row, 4)
	}
	DXTColBlock(const DXTColBlock &object)
	{
		memcpy(this, &object, sizeof(DXTColBlock));
		CM_MEMPROTECTOR(row, 4)
	}
	~DXTColBlock()
	{
		CM_MEMUNPROTECTOR(row)
	}
} DXTColBlock;

typedef struct DXTAlphaBlockExplicit
{
    short row[4];
} DXTAlphaBlockExplicit;

typedef struct DXTAlphaBlock3BitLinear
{
    char alpha0;
    char alpha1;

    //char stuff[6];
	CM_MEMDEF(stuff, 6)

	DXTAlphaBlock3BitLinear()
	{
		CM_MEMPROTECTOR(stuff, 6)
	}
	DXTAlphaBlock3BitLinear(const DXTAlphaBlock3BitLinear &object)
	{
		memcpy(this, &object, sizeof(DXTAlphaBlock3BitLinear));
		CM_MEMPROTECTOR(stuff, 6)
	}
	~DXTAlphaBlock3BitLinear()
	{
		CM_MEMUNPROTECTOR(stuff)
	}
} DXTAlphaBlock3BitLinear;

void rtgiDecodePixelFormat(SDDSHead* pHeader, RtgImage* pImage)
{
    if (pHeader->Flags2 & DDS_FOURCC)
    {
        BlockSize = ((pHeader->Width + 3)/4) * ((pHeader->Height + 3)/4) * ((pHeader->Depth + 3)/4);
        switch (pHeader->FourCC)
        {
        case MY_MAKEFOURCC('D','X','T','1'):
            pImage->info.iDxtcFormat = RtgImage::EPF_DXT1;
            BlockSize *= 8;
            break;

        case MY_MAKEFOURCC('D','X','T','2'):
            pImage->info.iDxtcFormat = RtgImage::EPF_DXT2;
            BlockSize *= 16;
            break;

        case MY_MAKEFOURCC('D','X','T','3'):
            pImage->info.iDxtcFormat = RtgImage::EPF_DXT3;
            BlockSize *= 16;
            break;

        case MY_MAKEFOURCC('D','X','T','4'):
            pImage->info.iDxtcFormat = RtgImage::EPF_DXT4;
            BlockSize *= 16;
            break;

        case MY_MAKEFOURCC('D','X','T','5'):
            pImage->info.iDxtcFormat = RtgImage::EPF_DXT5;
            BlockSize *= 16;
            break;

        default:
            pImage->info.iDxtcFormat = RtgImage::EPF_UNKNOWN;
            BlockSize *= 16;
            break;
        }
    }else
    {
        // This dds texture isn't compressed so write out ARGB format
        if (pHeader->RGBBitCount == 8)
        {
            pImage->info.iDxtcFormat = RtgImage::EPF_L8;
        }
        else if (pHeader->Flags2 & DDS_ALPHAPIXELS)
        {
            pImage->info.iDxtcFormat = RtgImage::EPF_ARGB;
        }else
        {
            pImage->info.iDxtcFormat = RtgImage::EPF_RGB;
        }
        BlockSize = (pHeader->Width * pHeader->Height * pHeader->Depth * (pHeader->RGBBitCount >> 3));
    }
    return;
}

#define MY_MAX(a,b) (((a) > (b)) ? (a) : (b))

// The few volume textures that I have don't have consistent LinearSize
//	entries, even thouh the DDS_LINEARSIZE flag is set.
void rtgiAdjustVolumeTexture(SDDSHead* pHeader, RtgImage* pImage)
{
    if (pHeader->Depth <= 1)
        return;

    // All volume textures I've seem so far didn't have the DDS_COMPLEX flag set,
    // even though this is normally required. But because noone does set it,
    // also read images without it (TODO: check file size for 3d texture?)
    if (/*!(pHeader->ddsCaps1 & DDS_COMPLEX) ||*/ !(pHeader->ddsCaps2 & DDS_VOLUME)) {
        pHeader->Depth = 1;
    }

    switch (pImage->info.iDxtcFormat)
    {
    case RtgImage::EPF_ARGB:
    case RtgImage::EPF_RGB:
        pHeader->LinearSize = MY_MAX(1,pHeader->Width) * MY_MAX(1,pHeader->Height) * (pHeader->RGBBitCount / 8);
        break;

    case RtgImage::EPF_DXT1:
        pHeader->LinearSize = MY_MAX(1,pHeader->Width/4) * MY_MAX(1,pHeader->Height/4) * 8;
        break;

    case RtgImage::EPF_DXT2:
    case RtgImage::EPF_DXT3:
    case RtgImage::EPF_DXT4:
    case RtgImage::EPF_DXT5:
        pHeader->LinearSize = MY_MAX(1,pHeader->Width/4) * MY_MAX(1,pHeader->Height/4) * 16;
        break;
    }

    pHeader->Flags1 |= DDS_LINEARSIZE;
    pHeader->LinearSize *= pHeader->Depth;

    return;
}

// Reads the compressed data
unsigned char* rtgiDxtReadData(RtArchive* pArc, SDDSHead* pHeader, RtgImage* pImage)
{
    unsigned int    Bps;
    int             y, z;
    unsigned char*  pTemp;
    unsigned int    Bpp;
    unsigned char*  pData;

    if (pImage->info.iDxtcFormat==RtgImage::EPF_RGB)
        Bpp = 3;
    else
        Bpp = 4;

    if (pHeader->Flags1 & DDS_LINEARSIZE)
    {
        pImage->info.iSizeOfData = pHeader->LinearSize;
        pData = (unsigned char*)RtCoreMem().Alloc(pImage->info.iSizeOfData, __FILE__, __LINE__);
        if (pData == NULL)
        {
            return NULL;
        }

        if (pArc->Serialize(pData, pHeader->LinearSize)!=(long)pHeader->LinearSize)
        {
            RtCoreMem().Free(pData);
            return NULL;
        }
    }else
    {
        Bps = pImage->info.iWidth * pHeader->RGBBitCount / 8;
        pImage->info.iSizeOfData = Bps * pImage->info.iHeight * pHeader->Depth;
        int CompLineSize = Bps;

        pData = (unsigned char*)RtCoreMem().Alloc(pImage->info.iSizeOfData, __FILE__, __LINE__);
        if (pData==NULL)
        {
            return NULL;
        }

        pTemp = pData;
        for (z = 0; z < (int)pHeader->Depth; z++)
        {
            for (y = 0; y < pImage->info.iHeight; y++)
            {
                if (pArc->Serialize(pTemp, Bps)!=(long)Bps)
                {
                    RtCoreMem().Free(pData);
                    return NULL;
                }
                pTemp += Bps;
            }
        }
    }
    return pData;
}

bool rtgiDecompressDXT1(SDDSHead* pHeader, RtgImage* pImage)
{
    int             x, y, z, i, j, k, Select;
    unsigned char   *Temp;
    Color565        *color_0, *color_1;
    Color8888       colours[4], *col;
    unsigned int    bitmask, Offset;

    int Bpp = 4;
    int Bps = pImage->info.iWidth * Bpp;
    int iSizeOfPlane = pImage->info.iHeight * Bps;

    Temp = pImage->data.pData;
    pImage->data.pData = (unsigned char*)RtCoreMem().Alloc(iSizeOfPlane*pHeader->Depth, __FILE__, __LINE__);

    for (z = 0; z < (int)pHeader->Depth; z++)
    {
        for (y = 0; y < pImage->info.iHeight; y += 4)
        {
            for (x = 0; x < pImage->info.iWidth; x += 4)
            {
                color_0 = ((Color565*)Temp);
                color_1 = ((Color565*)(Temp+2));
                bitmask = ((unsigned int*)Temp)[1];
                Temp += 8;

                colours[0].r = color_0->nRed << 3;
                colours[0].g = color_0->nGreen << 2;
                colours[0].b = color_0->nBlue << 3;
                colours[0].a = 0xFF;

                colours[1].r = color_1->nRed << 3;
                colours[1].g = color_1->nGreen << 2;
                colours[1].b = color_1->nBlue << 3;
                colours[1].a = 0xFF;

                if (*((unsigned short*)color_0) > *((unsigned short*)color_1))
                {
                    // Four-color block: derive the other two colors.
                    // 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
                    // These 2-bit codes correspond to the 2-bit fields 
                    // stored in the 64-bit block.
                    colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
                    colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
                    colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
                    colours[2].a = 0xFF;

                    colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
                    colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
                    colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
                    colours[3].a = 0xFF;
                }else
                { 
                    // Three-color block: derive the other color.
                    // 00 = color_0,  01 = color_1,  10 = color_2,
                    // 11 = transparent.
                    // These 2-bit codes correspond to the 2-bit fields 
                    // stored in the 64-bit block. 
                    colours[2].b = (colours[0].b + colours[1].b) / 2;
                    colours[2].g = (colours[0].g + colours[1].g) / 2;
                    colours[2].r = (colours[0].r + colours[1].r) / 2;
                    colours[2].a = 0xFF;

                    colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
                    colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
                    colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
                    colours[3].a = 0x00;
                }

                for (j = 0, k = 0; j < 4; j++)
                {
                    for (i = 0; i < 4; i++, k++)
                    {
                        Select = (bitmask & (0x03 << k*2)) >> k*2;
                        col = &colours[Select];

                        if (((x + i) < pImage->info.iWidth) && ((y + j) < pImage->info.iHeight))
                        {
                            Offset = z * iSizeOfPlane + (y + j) * Bps + (x + i) * Bpp;
                            pImage->data.pData[Offset + 0] = col->r;
                            pImage->data.pData[Offset + 1] = col->g;
                            pImage->data.pData[Offset + 2] = col->b;
                            pImage->data.pData[Offset + 3] = col->a;
                        }
                    }
                }
            } // for z
        } // for y
    } // for z

    RtCoreMem().Free(Temp);

    return true;
}

void CorrectPreMult(RtgImage* pImage)
{
    int i;
    for (i = 0; i < pImage->info.iSizeOfData; i += 4)
    {
        if (pImage->data.pData[i+3] != 0) // Cannot divide by 0.
        {
            pImage->data.pData[i]   = (unsigned char)(((unsigned int)pImage->data.pData[i]   << 8) / pImage->data.pData[i+3]);
            pImage->data.pData[i+1] = (unsigned char)(((unsigned int)pImage->data.pData[i+1] << 8) / pImage->data.pData[i+3]);
            pImage->data.pData[i+2] = (unsigned char)(((unsigned int)pImage->data.pData[i+2] << 8) / pImage->data.pData[i+3]);
        }
    }

    return;
}


bool rtgiDecompressDXT3(SDDSHead* pHeader, RtgImage* pImage)
{
    int                     x, y, z, i, j, k, Select;
    unsigned char           *Temp;
    Color565                *color_0, *color_1;
    Color8888               colours[4], *col;
    unsigned int            bitmask, Offset;
    unsigned short          word;
    DXTAlphaBlockExplicit   *alpha;

    int Bpp = 4;
    int Bps = pImage->info.iWidth * Bpp;
    int iSizeOfPlane = pImage->info.iHeight * Bps;

    Temp = pImage->data.pData;
    pImage->data.pData = (unsigned char*)RtCoreMem().Alloc(iSizeOfPlane*pHeader->Depth, __FILE__, __LINE__);

    for (z = 0; z < (int)pHeader->Depth; z++)
    {
        for (y = 0; y < pImage->info.iHeight; y += 4)
        {
            for (x = 0; x < pImage->info.iWidth; x += 4)
            {
                alpha = (DXTAlphaBlockExplicit*)Temp;
                Temp += 8;
                color_0 = ((Color565*)Temp);
                color_1 = ((Color565*)(Temp+2));
                bitmask = ((unsigned int*)Temp)[1];
                Temp += 8;

                colours[0].r = color_0->nRed << 3;
                colours[0].g = color_0->nGreen << 2;
                colours[0].b = color_0->nBlue << 3;
                colours[0].a = 0xFF;

                colours[1].r = color_1->nRed << 3;
                colours[1].g = color_1->nGreen << 2;
                colours[1].b = color_1->nBlue << 3;
                colours[1].a = 0xFF;

                // Four-color block: derive the other two colors.    
                // 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
                // These 2-bit codes correspond to the 2-bit fields 
                // stored in the 64-bit block.
                colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
                colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
                colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
                colours[2].a = 0xFF;

                colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
                colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
                colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
                colours[3].a = 0xFF;

                k = 0;
                for (j = 0; j < 4; j++)
                {
                    for (i = 0; i < 4; i++, k++)
                    {
                        Select = (bitmask & (0x03 << k*2)) >> k*2;
                        col = &colours[Select];

                        if (((x + i) < pImage->info.iWidth) && ((y + j) < pImage->info.iHeight))
                        {
                            Offset = z * iSizeOfPlane + (y + j) * Bps + (x + i) * Bpp;
                            pImage->data.pData[Offset + 0] = col->r;
                            pImage->data.pData[Offset + 1] = col->g;
                            pImage->data.pData[Offset + 2] = col->b;
                        }
                    }
                }

                for (j = 0; j < 4; j++)
                {
                    word = alpha->row[j];
                    for (i = 0; i < 4; i++)
                    {
                        if (((x + i) < pImage->info.iWidth) && ((y + j) < pImage->info.iHeight))
                        {
                            Offset = z * iSizeOfPlane + (y + j) * Bps + (x + i) * Bpp + 3;
                            pImage->data.pData[Offset] = word & 0x0F;
                            pImage->data.pData[Offset] = pImage->data.pData[Offset] | (pImage->data.pData[Offset] << 4);
                        }
                        word >>= 4;
                    }
                }
            } // for x
        } // for y
    } // for z

    RtCoreMem().Free(Temp);

    return true;
}

bool rtgiDecompressDXT2(SDDSHead* pHeader, RtgImage* pImage)
{
    // Can do color & alpha same as dxt3, but color is pre-multiplied 
    //   so the result will be wrong unless corrected. 
    if (!rtgiDecompressDXT3(pHeader, pImage))
        return false;
    CorrectPreMult(pImage);

    return true;
}

bool rtgiDecompressDXT5(SDDSHead* pHeader, RtgImage* pImage)
{
    int             x, y, z, i, j, k, Select;
    unsigned char   *Temp, *ppSource;
    Color565        *color_0, *color_1;
    Color8888       colours[4], *col;
    unsigned int    bitmask, Offset;
    unsigned char   alphas[8], *alphamask;
    unsigned int    bits;

    int Bpp = 4;
    int Bps = pImage->info.iWidth * Bpp;
    int iSizeOfPlane = pImage->info.iHeight * Bps;

    Temp = ppSource = pImage->data.pData;

    pImage->info.iSizeOfData = iSizeOfPlane*pHeader->Depth;
    pImage->data.pData = (unsigned char*)RtCoreMem().Alloc(pImage->info.iSizeOfData, __FILE__, __LINE__);

    for (z = 0; z < (int)pHeader->Depth; z++)
    {
        for (y = 0; y < pImage->info.iHeight; y += 4)
        {
            for (x = 0; x < pImage->info.iWidth; x += 4)
            {
                if (y >= pImage->info.iHeight || x >= pImage->info.iWidth)
                    break;
                alphas[0] = Temp[0];
                alphas[1] = Temp[1];
                alphamask = Temp + 2;
                Temp += 8;
                color_0 = ((Color565*)Temp);
                color_1 = ((Color565*)(Temp+2));
                bitmask = ((unsigned int*)Temp)[1];
                Temp += 8;

                colours[0].r = color_0->nRed << 3;
                colours[0].g = color_0->nGreen << 2;
                colours[0].b = color_0->nBlue << 3;
                colours[0].a = 0xFF;

                colours[1].r = color_1->nRed << 3;
                colours[1].g = color_1->nGreen << 2;
                colours[1].b = color_1->nBlue << 3;
                colours[1].a = 0xFF;

                // Four-color block: derive the other two colors.    
                // 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
                // These 2-bit codes correspond to the 2-bit fields 
                // stored in the 64-bit block.
                colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
                colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
                colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
                colours[2].a = 0xFF;

                colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
                colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
                colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
                colours[3].a = 0xFF;

                k = 0;
                for (j = 0; j < 4; j++)
                {
                    for (i = 0; i < 4; i++, k++)
                    {

                        Select = (bitmask & (0x03 << k*2)) >> k*2;
                        col = &colours[Select];

                        // only put pixels out < width or height
                        if (((x + i) < pImage->info.iWidth) && ((y + j) < pImage->info.iHeight))
                        {
                            Offset = z * iSizeOfPlane + (y + j) * Bps + (x + i) * Bpp;
                            pImage->data.pData[Offset + 0] = col->r;
                            pImage->data.pData[Offset + 1] = col->g;
                            pImage->data.pData[Offset + 2] = col->b;
                        }
                    }
                }

                // 8-alpha or 6-alpha block?    
                if (alphas[0] > alphas[1])
                {    
                    // 8-alpha block:  derive the other six alphas.    
                    // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
                    alphas[2] = (6 * alphas[0] + 1 * alphas[1] + 3) / 7;	// bit code 010
                    alphas[3] = (5 * alphas[0] + 2 * alphas[1] + 3) / 7;	// bit code 011
                    alphas[4] = (4 * alphas[0] + 3 * alphas[1] + 3) / 7;	// bit code 100
                    alphas[5] = (3 * alphas[0] + 4 * alphas[1] + 3) / 7;	// bit code 101
                    alphas[6] = (2 * alphas[0] + 5 * alphas[1] + 3) / 7;	// bit code 110
                    alphas[7] = (1 * alphas[0] + 6 * alphas[1] + 3) / 7;	// bit code 111  
                }else
                {  
                    // 6-alpha block.    
                    // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
                    alphas[2] = (4 * alphas[0] + 1 * alphas[1] + 2) / 5;	// Bit code 010
                    alphas[3] = (3 * alphas[0] + 2 * alphas[1] + 2) / 5;	// Bit code 011
                    alphas[4] = (2 * alphas[0] + 3 * alphas[1] + 2) / 5;	// Bit code 100
                    alphas[5] = (1 * alphas[0] + 4 * alphas[1] + 2) / 5;	// Bit code 101
                    alphas[6] = 0x00;										// Bit code 110
                    alphas[7] = 0xFF;										// Bit code 111
                }

                // Note: Have to separate the next two loops,
                //	it operates on a 6-byte system.

                // First three bytes
                bits = *((int*)alphamask);
                for (j = 0; j < 2; j++)
                {
                    for (i = 0; i < 4; i++)
                    {
                        // only put pixels out < width or height
                        if (((x + i) < pImage->info.iWidth) && ((y + j) < pImage->info.iHeight))
                        {
                            Offset = z * iSizeOfPlane + (y + j) * Bps + (x + i) * Bpp + 3;
                            pImage->data.pData[Offset] = alphas[bits & 0x07];
                        }
                        bits >>= 3;
                    }
                }

                // Last three bytes
                bits = *((int*)&alphamask[3]);
                for (j = 2; j < 4; j++)
                {
                    for (i = 0; i < 4; i++)
                    {
                        // only put pixels out < width or height
                        if (((x + i) < pImage->info.iWidth) && ((y + j) < pImage->info.iHeight))
                        {
                            Offset = z * iSizeOfPlane + (y + j) * Bps + (x + i) * Bpp + 3;
                            pImage->data.pData[Offset] = alphas[bits & 0x07];
                        }
                        bits >>= 3;
                    }
                }
            }
        }
    }

    RtCoreMem().Free(ppSource);

    return true;
}

bool rtgiDecompressDXT4(SDDSHead* pHeader, RtgImage* pImage)
{
    // Can do color & alpha same as dxt5, but color is pre-multiplied 
    //   so the result will be wrong unless corrected. 
    if (!rtgiDecompressDXT5(pHeader, pImage))
        return false;
    CorrectPreMult(pImage);

    return true;
}


bool rtgiLoadImageDDS(RtArchive* pArc, RtgImage* pImage)
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

    SDDSHead ddsHead;
    if (pArc->Serialize(&ddsHead, sizeof(SDDSHead))!=sizeof(SDDSHead))
    {
        RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_ERR;
        return false;
    }

    if (ddsHead.Depth == 0)
        ddsHead.Depth = 1;

    if (strnicmp(ddsHead.Signature, "DDS ", 4))
    {
        RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_ERR;
        return false;
    }
    if (ddsHead.Size1 != 124)
    {
        RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_ERR;
        return false;
    }
    if (ddsHead.Size2 != 32)
    {
        RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_ERR;
        return false;
    }
    if (ddsHead.Width == 0 || ddsHead.Height == 0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_ERR;
        return false;
    }

    rtgiDecodePixelFormat(&ddsHead, pImage);



    if (pImage->info.iDxtcFormat==RtgImage::EPF_UNKNOWN)
    {
        RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_ERR;
        return false;
    }

    // Microsoft bug, they're not following their own documentation.
    if (!(ddsHead.Flags1 & (DDS_LINEARSIZE | DDS_PITCH)))
    {
        ddsHead.Flags1 |= DDS_LINEARSIZE;
        ddsHead.LinearSize = BlockSize;
    }

    if (ddsHead.ddsCaps1 & DDS_COMPLEX)
    {
        if (ddsHead.ddsCaps2 & DDS_CUBEMAP)
        {
            // Load DDS Cubemap
            RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_UNSUPPORT;
            return false;
        }
    }

    pImage->info.bAlpha      = true;
    pImage->info.bDDS        = true;
    pImage->info.iWidth      = ddsHead.Width;
    pImage->info.iHeight     = ddsHead.Height;

    //if (!(pImage->info.iDxtcFormat>=RtgImage::EPF_DXT1 && pImage->info.iDxtcFormat<=RtgImage::EPF_DXT5))
    //{
    //    pImage->info.bDDS = false;
    //    //RtgImage::m_eLastError = RtgImage::ERI_DDS_FILE_FORMAT_UNSUPPORT;
    //    //return false;
    //}

    rtgiAdjustVolumeTexture(&ddsHead, pImage);

    pImage->data.pData = rtgiDxtReadData(pArc, &ddsHead, pImage);

    if (!RtgImage::m_bSupportDXT) // 如果不支持DDS
    {
        bool bDecompress = false;
        switch (pImage->info.iDxtcFormat)
        {
        case RtgImage::EPF_DXT1:
            bDecompress = rtgiDecompressDXT1(&ddsHead, pImage);
            break;

        case RtgImage::EPF_DXT2:
            bDecompress = rtgiDecompressDXT2(&ddsHead, pImage);
            break;

        case RtgImage::EPF_DXT3:
            bDecompress = rtgiDecompressDXT3(&ddsHead, pImage);	
            break;

        case RtgImage::EPF_DXT4:
            bDecompress = rtgiDecompressDXT4(&ddsHead, pImage);
            break;

        case RtgImage::EPF_DXT5:
            bDecompress = rtgiDecompressDXT5(&ddsHead, pImage);		
            break;

        case RtgImage::EPF_UNKNOWN:
            bDecompress = false;
            break;
        }
        if (!bDecompress)
        {
            RtCoreMem().Free(pImage->data.pData);
            pImage->data.pData = NULL;
            return false;
        }
        pImage->info.bDDS = false;
        pImage->Flip();
    }
    RtgImage::m_eLastError = RtgImage::ERI_OK;
    return true;
}

bool rtgiLoadImageDDSFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiLoadImageDDS(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}


} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
