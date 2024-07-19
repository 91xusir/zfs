#include "core/rt2_core.h"

namespace rt2_core {

RT2_CORE_API const char* rtCmdLine()
{
    return RtCore::Instance().strCmdLine.c_str();
}

RT2_CORE_API const char* rtBaseDir()
{
    return RtCore::Instance().strBaseDir.c_str();
}

RT2_CORE_API const char* rtComputerName()
{
    return RtCore::Instance().strComputerName.c_str();
}

RT2_CORE_API const char* rtUserName()
{
    return RtCore::Instance().strUserName.c_str();
}

RT2_CORE_API const char* rtExePathName()
{
    return RtCore::Instance().strExePathName.c_str();
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
