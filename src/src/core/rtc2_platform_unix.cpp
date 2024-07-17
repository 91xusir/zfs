#include "core/rt2_core.h"

namespace rt2_core {

#if defined(__GNUC__)

timeval* rtPlatformGetStartTime()
{
    static timeval startTime;
    return &startTime;
}

void rtciPlatformInit()
{
    timeval* pTimeVal = rtPlatformGetStartTime();
    pTimeVal->tv_sec  = 0;
    pTimeVal->tv_usec = 0;
    gettimeofday(pTimeVal, NULL);
	// printf("linux: start.tv_sec = %d,start.tv_usec = %d\n",start.tv_sec,start.tv_usec);
}

void rtciPlatformClear()
{
}

void RtCore::PlatformInitInfo()
{
}

RT2_CORE_API void rtSystemTime( int& iYear, int& iMonth, int& iDayOfWeek, int& iDay, int& iHour, int& iMin, int& iSec, int& iMSec )
{
    time_t t = time(NULL);
    struct tm * pst = localtime(&t);
    iYear = pst->tm_year + 1900;
    iMonth = pst->tm_mon + 1;
    iDayOfWeek = pst->tm_wday;
    iDay = pst->tm_mday;
    iHour = pst->tm_hour;
    iMin = pst->tm_min;
    iSec = pst->tm_sec;
    iMSec = 0;
}

RT2_CORE_API DWORD rtMilliseconds()
{
    timeval* pStart = rtPlatformGetStartTime();
	static timeval lasttv = {0};
	timeval tv;
	gettimeofday(&tv, NULL);
	
	////在2.6.21内核以下并且在多核CPU上，gettimeofday会有极小几率时光倒流，这里是为了防止这种情况影响上层逻辑
	//if (tv.tv_sec < lasttv.tv_sec)
	//{
	//	tv.tv_sec = lasttv.tv_sec;
	//	tv.tv_usec = lasttv.tv_usec;

	//}else if (tv.tv_usec < lasttv.tv_usec && tv.tv_sec == lasttv.tv_sec)
	//{
	//	tv.tv_usec = lasttv.tv_usec;
	//}else
	//{
	//	lasttv.tv_sec = tv.tv_sec;
	//	lasttv.tv_usec = tv.tv_usec;
	//}

	tv.tv_sec  -= pStart->tv_sec;
	tv.tv_usec -= pStart->tv_usec;
	

	return (tv.tv_sec*1000+tv.tv_usec/1000);
}

RT2_CORE_API void rtSleep( unsigned long dwMilliseconds )
{
	usleep(dwMilliseconds*1000);
}

RT2_CORE_API void rtSleepSecond( float fSeconds )
{
	usleep(fSeconds*1000*1000);
}

#endif // defined(__GNUC__)

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
