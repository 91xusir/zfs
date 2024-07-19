#include "core/rt2_core.h"

namespace rt2_core {
#ifdef _WIN32

BOOL  RtCPU::bTimestamp = false;
double RtCPU::fSecondsPerCycle = 0.0;

DWORD rtciCpuSpeedTest()
{
    return 0;
}

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
    // CPU特性
    bIsMMX       = IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE);;
    bIsPentiumPro= FALSE;;
    bIsKatmai    = FALSE;;
    bIsK6        = FALSE;;
    bIs3DNow     = IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE);;
    bIsSSE       = IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE);;
    bIsSSE2      = IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE);;
    bTimestamp   = IsProcessorFeaturePresent(PF_RDTSC_INSTRUCTION_AVAILABLE);

    // CPU速度
	LONG	lresult;
	HKEY	NewKey;
    lresult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_EXECUTE, &NewKey);
	if (lresult==ERROR_SUCCESS)
    {
	    DWORD dwData=0, dwType=REG_DWORD, dwSize=sizeof(dwData);
	    lresult = RegQueryValueEx(NewKey, "~MHz", NULL, &dwType, (LPBYTE)(&dwData), &dwSize);
	    if ((lresult==ERROR_SUCCESS) && (dwSize>0))
	    {
            dwCPUSpeed = dwData;
        }
    }
    if (dwCPUSpeed==0)
    {
        dwCPUSpeed = rtciCpuSpeedTest();
    }
    fSecondsPerCycle = (1.0/dwCPUSpeed) * 0.000001;
}

#endif // WIN32
} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
