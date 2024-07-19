
#ifndef _RTG_IMAGE_DDS_H_
#define _RTG_IMAGE_DDS_H_
namespace rt_graph {

bool rtgiLoadImageDDSFile(const char* szFilename, RtgImage* pImage);
bool rtgiLoadImageDDS(RtArchive* pArc, RtgImage* pImage);

} // namespace rt_graph
#endif // _RTG_IMAGE_DDS_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
