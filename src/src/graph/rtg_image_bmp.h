
#ifndef _RTG_IMAGE_BMP_H_
#define _RTG_IMAGE_BMP_H_
namespace rt_graph {

bool rtgiLoadImageBMPFile(const char* szFilename, RtgImage* pImage);
bool rtgiLoadImageBMP(RtArchive* pArc, RtgImage* pImage,bool bflip=false);

// 保存函数只保存24b(不带Alpha)， 如果需要带Alpha请保存成Tga

bool rtgiSaveImageBMPFile(const char* szFilename, RtgImage* pImage);
bool rtgiSaveImageBMP(RtArchive* pArc, RtgImage* pImage);

} // namespace rt_graph
#endif // _RTG_IMAGE_BMP_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
