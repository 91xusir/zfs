
#ifndef _RTG_IMAGE_BMP_H_
#define _RTG_IMAGE_BMP_H_
namespace rt_graph {

bool rtgiLoadImageBMPFile(const char* szFilename, RtgImage* pImage);
bool rtgiLoadImageBMP(RtArchive* pArc, RtgImage* pImage,bool bflip=false);

// ���溯��ֻ����24b(����Alpha)�� �����Ҫ��Alpha�뱣���Tga

bool rtgiSaveImageBMPFile(const char* szFilename, RtgImage* pImage);
bool rtgiSaveImageBMP(RtArchive* pArc, RtgImage* pImage);

} // namespace rt_graph
#endif // _RTG_IMAGE_BMP_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
