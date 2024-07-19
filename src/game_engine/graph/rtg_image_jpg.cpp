
#include "rtg_graph_inter.h"
#include "rtg_image_jpg.h"

extern "C" {
#define XMD_H
#undef FAR
#include "jpeglib.h"
#include "setjmp.h"
};

namespace rt_graph {

#if JPEG_LIB_VERSION < 62
#warning RtGraph was designed with libjpeg 6b or higher in mind.
#endif


static RtArchive* pCurrentArc;
jmp_buf	JpegJumpBuffer;
static bool jpgErrorOccured = false;

static void iJpegErrorExit( j_common_ptr cinfo )
{
    jpeg_destroy( cinfo );
    longjmp( JpegJumpBuffer, 1 );
}

void OutputMsg(struct jpeg_common_struct *JpegInfo)
{
    return;
}


//////////////////////////////////////////////////////////////////////////


// Internal function used to check if the HEADER is a valid .Jpg header.
bool CheckJpg(unsigned char Header[2])
{
    if (Header[0] != 0xFF || Header[1] != 0xD8)
        return false;
    return true;
}

// Overrides libjpeg's stupid error/warning handlers. =P
void ExitErrorHandle (struct jpeg_common_struct *JpegInfo)
{
    jpgErrorOccured = true;
    return;
}


typedef struct {
    struct jpeg_source_mgr pub;	/* public fields */

    JOCTET * buffer;		/* start of buffer */
    boolean start_of_file;	/* have we gotten any data yet? */
} iread_mgr;

typedef iread_mgr * iread_ptr;

#define INPUT_BUF_SIZE  4096  // choose an efficiently iread'able size


METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
    iread_ptr src = (iread_ptr) cinfo->src;
    src->start_of_file = TRUE;
}


METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
    iread_ptr src = (iread_ptr) cinfo->src;
    int nbytes;

    nbytes = pCurrentArc->Serialize(src->buffer, INPUT_BUF_SIZE);

    if (nbytes <= 0) {
        if (src->start_of_file) {  // Treat empty input file as fatal error
            //ERREXIT(cinfo, JERR_INPUT_EMPTY);
            jpgErrorOccured = true;
        }
        //WARNMS(cinfo, JWRN_JPEG_EOF);
        // Insert a fake EOI marker
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
        return FALSE;
    }
    if (nbytes < INPUT_BUF_SIZE) {
    }

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;

    return TRUE;
}


METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    iread_ptr src = (iread_ptr) cinfo->src;

    if (num_bytes > 0) {
        while (num_bytes > (long) src->pub.bytes_in_buffer) {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) fill_input_buffer(cinfo);
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}


METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
    // no work necessary here
}


GLOBAL(void)
devil_jpeg_read_init (j_decompress_ptr cinfo)
{
    iread_ptr src;

    if ( cinfo->src == NULL ) {  // first time for this JPEG object?
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small)( (j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(iread_mgr) );
        src = (iread_ptr) cinfo->src;
        src->buffer = (JOCTET *)
            (*cinfo->mem->alloc_small)( (j_common_ptr)cinfo, JPOOL_PERMANENT,
            INPUT_BUF_SIZE * sizeof(JOCTET) );
    }

    src = (iread_ptr) cinfo->src;
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;  // use default method
    src->pub.term_source = term_source;
    src->pub.bytes_in_buffer = 0;  // forces fill_input_buffer on first read
    src->pub.next_input_byte = NULL;  // until buffer loaded
}


typedef struct
{
    struct jpeg_destination_mgr		pub;
    JOCTET							*buffer;
    BOOL						    bah;
} iwrite_mgr;

typedef iwrite_mgr *iwrite_ptr;

#define OUTPUT_BUF_SIZE 4096


METHODDEF(void)
init_destination(j_compress_ptr cinfo)
{
    iwrite_ptr dest = (iwrite_ptr)cinfo->dest;
    dest->buffer = (JOCTET *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
        OUTPUT_BUF_SIZE * sizeof(JOCTET));

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    return;
}

METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
    iwrite_ptr dest = (iwrite_ptr)cinfo->dest;
    pCurrentArc->Serialize(dest->buffer, OUTPUT_BUF_SIZE);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    return TRUE;
}

METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
    iwrite_ptr dest = (iwrite_ptr)cinfo->dest;
    pCurrentArc->Serialize(dest->buffer, OUTPUT_BUF_SIZE - (int)dest->pub.free_in_buffer);
    return;
}


GLOBAL(void)
devil_jpeg_write_init(j_compress_ptr cinfo)
{
    iwrite_ptr dest;

    if (cinfo->dest == NULL) {	// first time for this JPEG object?
        cinfo->dest = (struct jpeg_destination_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(struct jpeg_destination_mgr));
        dest = (iwrite_ptr)cinfo->dest;
    }

    dest = (iwrite_ptr)cinfo->dest;
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;

    return;
}

static int s_rtgiJpegQuality = 80; // default = 80
void rtgiSetJpegQuality(int iQuality)
{
    s_rtgiJpegQuality = iQuality;
}

bool rtgiSaveImageJPG(RtArchive* pArc, RtgImage* pImage)
{
    struct		    jpeg_compress_struct JpegInfo;
    struct		    jpeg_error_mgr Error;
    JSAMPROW	    row_pointer[1];
    unsigned char*  pTempData;

    pCurrentArc = pArc;

    JpegInfo.err = jpeg_std_error(&Error);
    // Now we can initialize the JPEG compression object.
    jpeg_create_compress(&JpegInfo);

    //jpeg_stdio_dest(&JpegInfo, JpegFile);
    devil_jpeg_write_init(&JpegInfo);

    JpegInfo.image_width = pImage->info.iWidth;  // image width and height, in pixels
    JpegInfo.image_height = pImage->info.iHeight;
    JpegInfo.input_components = 3;  // # of color components per pixel
    int Bps = pImage->info.iWidth * 3;

    JpegInfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&JpegInfo);

    JpegInfo.write_JFIF_header = TRUE;

    jpeg_set_quality(&JpegInfo, s_rtgiJpegQuality, TRUE);

    jpeg_start_compress(&JpegInfo, TRUE);

    pTempData = (unsigned char*)RtCoreMem().Alloc(pImage->info.iWidth*pImage->info.iHeight*3, __FILE__, __LINE__);
    int x, y;
    unsigned char *pS, *pD;
    pS = pImage->data.pData;
    for (y=0; y<pImage->info.iHeight; y++)
    {
        pD = pTempData + pImage->info.iWidth*3*(pImage->info.iHeight-y-1);
        for (x=0; x<pImage->info.iWidth; x++)
        {
            pD[0] = pS[0];
            pD[1] = pS[1];
            pD[2] = pS[2];
            pD += 3;
            pS += 4;
        }
    }

    //row_stride = image_width * 3;	// JSAMPLEs per row in image_buffer

    pD = pTempData;
    while (JpegInfo.next_scanline < JpegInfo.image_height)
    {
        // jpeg_write_scanlines expects an array of pointers to scanlines.
        // Here the array is only one element long, but you could pass
        // more than one scanline at a time if that's more convenient.
        row_pointer[0] = pD+(JpegInfo.next_scanline*Bps);
        (void) jpeg_write_scanlines(&JpegInfo, row_pointer, 1);
    }

    RtCoreMem().Free(pTempData);

    // Step 6: Finish compression
    jpeg_finish_compress(&JpegInfo);

    // Step 7: release JPEG compression object

    // This is an important step since it will release a good deal of memory.
    jpeg_destroy_compress(&JpegInfo);

    return TRUE;
}



bool rtgiLoadFromJpegStruct(RtgImage* pImage, void *_JpegInfo)
{
    // sam. void (*errorHandler)(j_common_ptr);
    unsigned char	*TempPtr[1];
    unsigned int	Returned;
    j_decompress_ptr JpegInfo = (j_decompress_ptr)_JpegInfo;

    //added on 2003-08-31 as explained in sf bug 596793
    jpgErrorOccured = false;

    // sam. errorHandler = JpegInfo->err->error_exit;
    // sam. JpegInfo->err->error_exit = ExitErrorHandle;
    jpeg_start_decompress((j_decompress_ptr)JpegInfo);

    int Bpp = (unsigned char)JpegInfo->output_components; // Convert to bytes per pixel
    if (Bpp!=3)
    {
        jpgErrorOccured = true;
        return false;
    }
    pImage->info.bAlpha      = Bpp==4 ? true : false;
    pImage->info.bDDS        = false;
    pImage->info.iWidth      = JpegInfo->output_width;
    pImage->info.iHeight     = JpegInfo->output_height;
    pImage->info.iSizeOfData = pImage->info.iWidth * pImage->info.iHeight * Bpp;
    pImage->info.iDxtcFormat = 0;

    int Bps = Bpp*pImage->info.iWidth;
    int iDataSize = pImage->info.iWidth * pImage->info.iHeight * 4; // 读到内存中的贴图必须为32位
    pImage->data.pData = (unsigned char*)RtCoreMem().Alloc(iDataSize, __FILE__, __LINE__);

    if (pImage->data.pData==0)
    {
        RtgImage::m_eLastError = RtgImage::ERI_MEMORY_ERR;
        return false;
    }

    TempPtr[0] = pImage->data.pData;
    while (JpegInfo->output_scanline < JpegInfo->output_height)
    {
        Returned = jpeg_read_scanlines(JpegInfo, TempPtr, 1);  // anyway to make it read all at once?
        TempPtr[0] += Bps;
        if (Returned == 0)
            break;
    }

    // sam. JpegInfo->err->error_exit = errorHandler;
    unsigned char* pSrc = pImage->data.pData + pImage->info.iSizeOfData - 3;
    unsigned char* pDest = pImage->data.pData + iDataSize - 4;
    for (; pSrc>pImage->data.pData; pSrc-=3,pDest-=4)
    {
        pDest[0] = pSrc[0];
        pDest[1] = pSrc[1];
        pDest[2] = pSrc[2];
        pDest[3] = 0;
    }
    pImage->info.iSizeOfData = iDataSize;

    if (jpgErrorOccured)
        return false;

    return true;
}

bool rtgiLoadImageJPG(RtArchive* pArc, RtgImage* pImage)
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

    pCurrentArc = pArc;

    struct jpeg_error_mgr			Error;
    struct jpeg_decompress_struct	JpegInfo;
    bool                            bResult;

    JpegInfo.err = jpeg_std_error( &Error );		// init standard error handlers
    Error.error_exit = iJpegErrorExit;				// add our exit handler
    Error.output_message = OutputMsg;

    if ( (bResult = setjmp( JpegJumpBuffer ) == 0) != FALSE )
    {
        jpeg_create_decompress( &JpegInfo );
        JpegInfo.do_block_smoothing = TRUE;
        JpegInfo.do_fancy_upsampling = TRUE;

        //jpeg_stdio_src(&JpegInfo, iGetFile());
        devil_jpeg_read_init( &JpegInfo );
        jpeg_read_header( &JpegInfo, TRUE );

        bResult = rtgiLoadFromJpegStruct(pImage, &JpegInfo);

        jpeg_finish_decompress( &JpegInfo );
        jpeg_destroy_decompress( &JpegInfo );
    }
    return bResult;
}

bool rtgiLoadImageJPGFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiLoadImageJPG(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}


bool rtgiSaveImageJPGFile(const char* szFilename, RtgImage* pImage)
{
    RtArchive* pArc = RtCoreFile().CreateFileWriter(NULL, szFilename, 0);
    if (pArc==NULL)
    {
        RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
        return false;
    }
    bool bResult = rtgiSaveImageJPG(pArc, pImage);
    RtCoreFile().CloseFile(pArc);
    return bResult;
}

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
