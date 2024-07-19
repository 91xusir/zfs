#include "core/rt2_core.h"

namespace rt2_core {

#ifdef _WIN32

RT2_CORE_API const char* rtCurrentDir()
{
    static char szPath[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, szPath);
    return szPath;
}

void rtciPlatformInit()
{
}

void rtciPlatformClear()
{
}

void RtCore::PlatformInitInfo()
{
    char buf[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    unsigned long len = MAX_COMPUTERNAME_LENGTH;

    // strCmdLine
    strCmdLine = GetCommandLine();

    // strBaseDir
    GetModuleFileName(0, buf, _MAX_PATH);
    _splitpath(buf, drive, dir, fname, ext);
    _makepath(buf, drive, dir, "", "");
    strBaseDir = buf;

    // strExePathName
    GetModuleFileName(0, buf, _MAX_PATH);
    strExePathName = buf;

    // strComputerName
    if (!GetComputerName(buf, &len))
    {
        buf[0] = 0;
    }
    strComputerName = buf;

    // strUserName
    if (!GetUserName(buf, &len))
    {
        buf[0] = 0;
    }
    strUserName = buf;
}

RT2_CORE_API void rtSystemTime( int& iYear, int& iMonth, int& iDayOfWeek, int& iDay, int& iHour, int& iMin, int& iSec, int& iMSec )
{
    SYSTEMTIME t;
    GetLocalTime(&t);
    iYear = t.wYear;
    iMonth = t.wMonth;
    iDayOfWeek = t.wDayOfWeek;
    iDay = t.wDay;
    iHour = t.wHour;
    iMin = t.wMinute;
    iSec = t.wSecond;
    iMSec = t.wMilliseconds;
}

RT2_CORE_API DWORD rtMilliseconds()
{
	return timeGetTime();
}

RT2_CORE_API void rtSleep( unsigned long dwMilliseconds )
{
	Sleep(dwMilliseconds);
}

RT2_CORE_API void rtSleepSecond( float fSeconds )
{
	Sleep((DWORD)(fSeconds*1000.f));
}

#endif // _WIN32

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
