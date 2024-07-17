/**
* @file rtc2_platform.h 这个文件负责一些与平台相关的函数，目的是使引擎跨平台使用.
* @author janhail
*/

/*
#ifdef WIN32
#   include "rtc2_platform_win32.h"
#else
#   include "rtc2_platform_unix.h"
#endif
*/

#if (defined(_MSC_VER) || defined(__ICL))
#   include "rtc2_platform_win32.h"
#elif defined(__GNUC__)
#   include "rtc2_platform_unix.h"
#endif

/*-----------------------------------------------------------------------------
-     API 定义
-----------------------------------------------------------------------------*/

#ifdef RT_CORE_DLL  // 是否生成DLL
#   ifdef RT_CORE_EXPORTS
#       define RT2_CORE_API DLL_EXPORT
#   else
#       define RT2_CORE_API DLL_IMPORT
#   endif
#else
#   define  RT2_CORE_API
#endif

#ifdef __cplusplus
#   define BEGIN_EXTERN_C() extern "C" {
#   define END_EXTERN_C() }
#else
#   define BEGIN_EXTERN_C()
#   define END_EXTERN_C()
#endif


/*-----------------------------------------------------------------------------
-     STL字符串的哈希函数
-----------------------------------------------------------------------------*/
inline std::size_t rtHashString(const char* __s) // take from sgi stl
{
	unsigned long __h = 0;
	for ( ; *__s; ++__s)
		__h = 5*__h + *__s;
	return std::size_t(__h);
};

struct hash_str
{
	std::size_t operator()(const std::string &s) const { return rtHashString(s.c_str()); }
};

struct hash_qword
{
	std::size_t operator() (const QWORD &key) const
   	{
		QWORD myid = key;
		std::size_t t = (std::size_t)(myid & 0x00000000ffffffff);
		myid = key;
		t = t + (std::size_t)(myid > 32);
		return t;
	}
};


namespace rt2_core {

enum EGlobalConstants
{
    MAXBYTE       = 0xff,
    MAXWORD       = 0xffffU,
    MAXDWORD      = 0xffffffffU,
    MAXSBYTE      = 0x7f,
    MAXSWORD      = 0x7fff,
    MAXINT        = 0x7fffffff,
    INDEX_NONE    = -1,
    UNICODE_BOM   = 0xfeff,
    E_NoInit      = 0
};

/*-----------------------------------------------------------------------------
-     平台相关函数定义
-----------------------------------------------------------------------------*/

/** 让线程“睡眠”dwMilliseconds毫秒.
*/
RT2_CORE_API void rtSleep( unsigned long dwMilliseconds );

/** 让线程“睡眠”fSeconds秒.
*/
RT2_CORE_API void rtSleepSecond( float fSeconds );

/*-----------------------------------------------------------------------------
-   操作系统相关函数
-----------------------------------------------------------------------------*/

RT2_CORE_API const char* rtCmdLine();           // 执行命令行
RT2_CORE_API const char* rtBaseDir();           // 执行程序所在目录
RT2_CORE_API const char* rtCurrentDir();        // 当前的目录
RT2_CORE_API const char* rtComputerName();      // 计算机名字
RT2_CORE_API const char* rtUserName();          // 用户名字
RT2_CORE_API const char* rtExePathName();       // 执行程序的文件名(包含目录)

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
