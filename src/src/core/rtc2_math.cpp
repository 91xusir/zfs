#include "core/rt2_core.h"

namespace rt2_core {

/*------------------------------------------------------------------------
-   Math.
------------------------------------------------------------------------*/

static long s_lHoldRand = 19790213L;
void rtRandomSeed(unsigned int seed)
{
    s_lHoldRand = (long)seed;
}

int rtRandom()
{
    return(((s_lHoldRand = s_lHoldRand * 214013L + 2531011L) >> 16) & 0x7fff);
}

int rtRandom(int iMax, int iMin)
{
    if(iMax==iMin) return iMin;
    return rtRandom()%(iMax-iMin) + iMin;
}

RtRandom::RtRandom()
{
    m_lSeed = 19790213L;
}

RtRandom::RtRandom(unsigned int iSeed)
{
    m_lSeed = iSeed;
}

void RtRandom::SetSeed(unsigned int iSeed)
{
    m_lSeed = iSeed;
}

int RtRandom::Get()
{
    return(((s_lHoldRand = s_lHoldRand * 214013L + 2531011L) >> 16) & 0x7fff);
}

int RtRandom::Get(int iMax)
{
    return Get()%iMax;
}

int RtRandom::Get(int iMin, int iMax)
{
    if(iMax==iMin) return iMin;
    return Get()%(iMax-iMin) + iMin;
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
