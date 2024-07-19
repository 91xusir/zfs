#include "core/rt2_core.h"

namespace rt2_core {

#if defined(__GNUC__)

BOOL   RtCPU::bTimestamp = false;
double RtCPU::fSecondsPerCycle = 0.0;

RtCPU::RtCPU()
{
    bIsMMX = FALSE;
    bIsPentiumPro = FALSE;
    bIsKatmai = FALSE;
    bIsK6 = FALSE;
    bIs3DNow = FALSE;
    bIsSSE = FALSE;
    bIsSSE2 = FALSE;
    dwCPUSpeed = 0;
}

void RtCPU::Init()
{
}

#endif // defined(__GNUC__)
} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
