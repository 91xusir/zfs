
#ifndef _RTG_IMAGE_TGA_H_
#define _RTG_IMAGE_TGA_H_
namespace rt_graph {

bool rtgiLoadImageTGAFile(const char* szFilename, RtgImage* pImage);
bool rtgiLoadImageTGA(RtArchive* pArc, RtgImage* pImage);

bool rtgiSaveImageTGAFile(const char* szFilename, RtgImage* pImage);
bool rtgiSaveImageTGA(RtArchive* pArc, RtgImage* pImage);

} // namespace rt_graph
#endif // _RTG_IMAGE_TGA_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
