
#include "rtg_graph_inter.h"

namespace rt_graph {

int RtgGetVertexFormatSize(RTGVertexFormat eFormat)
{
    static RTGVertexFormat eLastFormat=RTGVF_NULL;
    static int iLastSize=0;
    if (eLastFormat==eFormat) return iLastSize;
    iLastSize = 0;
    if (eFormat&RTGVF_V)   iLastSize += sizeof(float)*3;
    if (eFormat&RTGVF_TV)  iLastSize += sizeof(float)*4;
    if (eFormat&RTGVF_N)   iLastSize += sizeof(float)*3;
    if (eFormat&RTGVF_C)   iLastSize += sizeof(DWORD);

    if (eFormat&RTGVF_T1)      iLastSize += sizeof(float)*2*1;
    else if (eFormat&RTGVF_T2) iLastSize += sizeof(float)*2*2;
    else if (eFormat&RTGVF_T3) iLastSize += sizeof(float)*2*3;
    else if (eFormat&RTGVF_T4) iLastSize += sizeof(float)*2*4;
    return iLastSize;
}

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
