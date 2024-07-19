#include "core/rt2_core.h"
#include <time.h>

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   Timing functions.
-----------------------------------------------------------------------------*/

RT2_CORE_API DWORD rtTimestamp()
{
    return (DWORD)time(NULL);
}

RT2_CORE_API const char* rtTimestampStr()
{
    time_t ltime;
    time(&ltime);
    struct tm *today = localtime(&ltime);
    return asctime(today);
}

RT2_CORE_API DWORD rtSecond()
{
    return (DWORD)time(NULL);
}

/*-----------------------------------------------------------------------------
-   RtLocateTime.
-----------------------------------------------------------------------------*/

RtLocateTime::RtLocateTime()
{
    rtSystemTime(iYear, iMonth, iDayOfWeek, iDay, iHour, iMin, iSec, iMSec);
}

RtLocateTime::RtLocateTime(DWORD dwTime)
{
    Update(dwTime);
}

void RtLocateTime::Update()
{
    rtSystemTime(iYear, iMonth, iDayOfWeek, iDay, iHour, iMin, iSec, iMSec);
}

void RtLocateTime::Update(DWORD dwTime)
{
    time_t ltime = dwTime;
    struct tm *pst = localtime(&ltime);
    iYear      = pst->tm_year + 1900;
    iMonth     = pst->tm_mon + 1;
    iDayOfWeek = pst->tm_wday;
    iDay       = pst->tm_mday;
    iHour      = pst->tm_hour;
    iMin       = pst->tm_min;
    iSec       = pst->tm_sec;
    iMSec      = 0;
}

time_tick _tick;



_tick_log::_tick_log(const char* block)
{
    //strncpy(blockName, block, sizeof(blockName));
	rt2_strncpy(blockName, block, sizeof(blockName));
    _tick.begin();
}

_tick_log::~_tick_log()
{
    RtCoreLog().Info("%s : %fms\n", blockName, _tick.end());
}




/*
RT2_CORE_API RtTime rtSecondsSlow()
{
    return RtTime((rtMilliseconds()*1.f)/1000.f);
}
*/

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
