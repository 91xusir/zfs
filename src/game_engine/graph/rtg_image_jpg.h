
#ifndef _RTG_IMAGE_JPG_H_
#define _RTG_IMAGE_JPG_H_
namespace rt_graph {

bool rtgiLoadImageJPGFile(const char* szFilename, RtgImage* pImage);
bool rtgiLoadImageJPG(RtArchive* pArc, RtgImage* pImage);

bool rtgiSaveImageJPGFile(const char* szFilename, RtgImage* pImage);
bool rtgiSaveImageJPG(RtArchive* pArc, RtgImage* pImage);

void rtgiSetJpegQuality(int iQuality); // 1-100

} // namespace rt_graph
#endif // _RTG_IMAGE_JPG_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
