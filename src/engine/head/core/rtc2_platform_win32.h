/**
* @file rtc2_platform_win32.h 和Win32平台相关的定义
* @author janhail
*/

#define __WIN32__   1

#include <windows.h>

/*-----------------------------------------------------------------------------
-   类型定义
-----------------------------------------------------------------------------*/

// Undo any Windows defines.
#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef UINT
#undef FLOAT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT
#undef CDECL

// Make sure HANDLE is defined.
#ifndef _WINDOWS_
#define HANDLE void*
#define HINSTANCE void*
#endif

// Unsigned base types.
typedef unsigned char       BYTE;           ///< 8-bit  unsigned.
typedef unsigned short      WORD;           ///< 16-bit unsigned.
typedef unsigned int        UINT;           ///< 32-bit unsigned.
typedef unsigned long       DWORD;          ///< 32-bit unsigned.
typedef unsigned __int64    QWORD;          ///< 64-bit unsigned.

// Signed base types.
typedef signed char         SBYTE;          ///< 8-bit  signed.
typedef signed short        SWORD;          ///< 16-bit signed.
typedef signed int          INT;            ///< 32-bit signed.
typedef signed __int64      SQWORD;         ///< 64-bit signed.

// Other base types.
typedef signed int          BOOL;           ///< Boolean 0 (false) or 1 (true).
typedef float               FLOAT;          ///< 32-bit IEEE floating point.
typedef double              DOUBLE;         ///< 64-bit IEEE double.
typedef unsigned long       SIZE_T;         ///< Corresponds to C SIZE_T.

typedef void* RTHINSTANCE;    ///< HINSTANCE
typedef void* RTHWND;         ///< HWND

typedef __int64             RTTIME64;
typedef HANDLE              RtThreadHandle;


#ifndef NULL
#   define NULL     0
#endif

#ifndef TRUE
#   define TRUE     1
#endif

#ifndef FALSE
#   define FALSE    0
#endif

/*-----------------------------------------------------------------------------
-   函数名转换 (把一些不同平台有相同功能，但是名字不同的函数名进行转换)
-----------------------------------------------------------------------------*/
#define     snprintf                _snprintf
#define     snscanf                 _snscanf
#define     vsnprintf               _vsnprintf

//#if (_MSC_VER >= 1400)	//1000 VC5.0 1200 VC6.0 1201 EVC4.2 1300 VC2002 1310 VC2003 1400 VC2005
//#define     EXT_SPACE               stdext
//#else
//#define     EXT_SPACE               std
//#endif

// 移除对旧版vs编译器的支持，直接使用现代C++标准库中的std命名空间
#define EXT_SPACE std

/*-----------------------------------------------------------------------------
-   函数类型定义
-----------------------------------------------------------------------------*/
#define DLL_IMPORT          __declspec(dllimport)   ///< 从DLL导出函数
#define DLL_EXPORT          __declspec(dllexport)   ///< 导出函数到DLL
#define DLL_EXPORT_CLASS    __declspec(dllexport)   ///< 导出类到DLL
#define CDECL               __cdecl                 ///< 标准C函数，参数从右向左压入堆栈，并且需要调用者清除堆栈信息（add esp xxx）
#define STDCALL             __stdcall               ///< 标准C++函数，参数从右向左压入堆栈，但是函数体自身清除堆栈（ret xxx）
#define FASTCALL            __fastcall              ///< 函数参数会使用寄存器传递
#define FORCEINLINE         __forceinline           ///< 强制inline
#define APICALL             _far _pascal            ///< Windows API 函数调用方式
#define NOVTABLE            __declspec(novtable)    ///< 强制不包含虚表
#define ASM_NAKED           __declspec(naked)       ///< 强制内嵌汇编不保存寄存器信息

/** HASH_STR 是哈希一个字符串的简写方式.
*/
#define HASH_STR(key, val) std::unordered_map<key, val>

/** HASH_MULTISTR 是哈希多个字符串的简写方式.
*/
#define HASH_MULTISTR(key, val) std::unordered_multimap<key, val>

#define HASH_QWORD(key, val) std::unordered_map<key, val>

#define HASH_MULTIQWORD(key, val) std::unordered_multimap<key, val>

namespace rt2_core {

	/*-----------------------------------------------------------------------------
	-   强制关闭一些VC++中Lv4的警告
	-----------------------------------------------------------------------------*/
#pragma warning(disable : 4305) /* truncation from 'const double' to 'float'                            */
#pragma warning(disable : 4244) /* conversion to float, possible loss of data                           */
#pragma warning(disable : 4699) /* creating precompiled header                                          */
#pragma warning(disable : 4200) /* Zero-length array item at end of structure, a VC-specific extension  */
#pragma warning(disable : 4100) /* unreferenced formal parameter                                        */
#pragma warning(disable : 4514) /* unreferenced inline function has been removed                        */
#pragma warning(disable : 4201) /* nonstandard extension used : nameless struct/union                   */
#pragma warning(disable : 4710) /* inline function not expanded                                         */
#pragma warning(disable : 4702) /* unreachable code in inline expanded function                         */
#pragma warning(disable : 4711) /* function selected for autmatic inlining                              */
#pragma warning(disable : 4725) /* Pentium fdiv bug                                                     */
#pragma warning(disable : 4127) /* Conditional expression is constant                                   */
#pragma warning(disable : 4512) /* assignment operator could not be generated                           */
#pragma warning(disable : 4530) /* C++ exception handler used, but unwind semantics are not enabled     */
#pragma warning(disable : 4245) /* conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch */
#pragma warning(disable : 4238) /* nonstandard extension used : class rvalue used as lvalue             */
#pragma warning(disable : 4251) /* needs to have dll-interface to be used by clients of class 'ULinker' */
#pragma warning(disable : 4275) /* non dll-interface class used as base for dll-interface class         */
#pragma warning(disable : 4511) /* copy constructor could not be generated                              */
#pragma warning(disable : 4284) /* return type is not a UDT or reference to a UDT                       */
#pragma warning(disable : 4355) /* this used in base initializer list                                   */
#pragma warning(disable : 4097) /* typedef-name '' used as synonym for class-name ''                    */
#pragma warning(disable : 4291) /* typedef-name '' used as synonym for class-name ''                    */
#pragma warning(disable : 4996) /* 'function': was declared deprecated                                  */
#pragma warning(disable : 4407) /* cast between different pointer to member representations, 
								   compiler may generate incorrect code */

#ifndef _CPPUNWIND
#error "VC编译器选项错误: C++异常处理(C++ exception handling)必须打开"
#endif

#ifdef _CHAR_UNSIGNED
#error "VC编译器选项错误: 字符(Characters)必须是有符号的"
#endif

								   /*-----------------------------------------------------------------------------
								   -   操作系统相关字符串常量
								   -----------------------------------------------------------------------------*/
#ifdef _DEBUG
#define RT_COMPILER "编译在 Visual C++ Debug 模式"
#else
#define RT_COMPILER "编译在 Visual C++ Release 模式"
#endif
#define OS_TYPE_STR             "Windows"           ///< 操作系统类型
#define LINE_TERMINATOR         "\r\n"              ///< 换行符
#define PATH_SEPARATOR          "\\/"               ///< 目录分隔符（所有可能）
#define PATH_SEPARATOR_S        "\\"                ///< 目录分隔符（字符串）
#define PATH_SEPARATOR_C        '\\'                ///< 目录分隔符（字符）

								   /*-----------------------------------------------------------------------------
								   -   线程，信号量相关操作
								   -----------------------------------------------------------------------------*/
#define RT_CREATE_THREAD(func,arg)          CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)func,(void*)arg,NULL,NULL)
#define RT_CREATE_THREAD_RET(ret)           ((ret)==0)
#define RT_TERMINATE_THREAD(vHandle, vCode) TerminateThread(vHandle, vCode)

#define RT_TYPE_CS                          CRITICAL_SECTION
#define RT_LOCK_CS(p)                       EnterCriticalSection(p)
#define RT_UNLOCK_CS(p)                     LeaveCriticalSection(p)
#define RT_INIT_CS(p)                       InitializeCriticalSection(p)
#define RT_DELETE_CS(p)                     DeleteCriticalSection(p)

#define RtThreadMutexType                   CRITICAL_SECTION
#define RtThreadMutexInit(p)                InitializeCriticalSection(p)
#define RtThreadMutexDestroy(p)             DeleteCriticalSection(p)
#define RtThreadMutexLock(p)                EnterCriticalSection(p)
#define RtThreadMutexUnlock(p)              LeaveCriticalSection(p)

#define RtProcessMutexType                  HANDLE
#define RtProcessMutexInit(p)               (*(p) = CreateMutex(NULL, false, NULL))
#define RtProcessMutexDestroy(p)            CloseHandle(*(p))
#define RtProcessMutexLock(p)               WaitForSingleObject(*(p), INFINITE)
#define RtProcessMutexUnlock(p)             ReleaseMutex(*(p))

#define RtResLockType                       long
#define RtResLockInit(p)                    (*(p) = 0)
#define RtResLockDestroy(p)                 /* 不做任何事 */
#define RtResLockLock(p)                    while(::InterlockedExchange(p, 1)) { rtSleep(0.f); }
#define RtResLockTryLock(p)                 (!::InterlockedExchange(p, 1))
#define RtResLockUnlock(p)                  ::InterlockedExchange(p, 0)

#define RtInterlockedIncrement              ::InterlockedIncrement
#define RtInterlockedDecrement              ::InterlockedDecrement

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
