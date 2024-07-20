/**
* @file rtc2_platform_win32.h ��Win32ƽ̨��صĶ���
* @author janhail
*/

#define __WIN32__   1

#include <windows.h>

/*-----------------------------------------------------------------------------
-   ���Ͷ���
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
-   ������ת�� (��һЩ��ͬƽ̨����ͬ���ܣ��������ֲ�ͬ�ĺ���������ת��)
-----------------------------------------------------------------------------*/
#define     snprintf                _snprintf
#define     snscanf                 _snscanf
#define     vsnprintf               _vsnprintf

//#if (_MSC_VER >= 1400)	//1000 VC5.0 1200 VC6.0 1201 EVC4.2 1300 VC2002 1310 VC2003 1400 VC2005
//#define     EXT_SPACE               stdext
//#else
//#define     EXT_SPACE               std
//#endif

// �Ƴ��Ծɰ�vs��������֧�֣�ֱ��ʹ���ִ�C++��׼���е�std�����ռ�
#define EXT_SPACE std

/*-----------------------------------------------------------------------------
-   �������Ͷ���
-----------------------------------------------------------------------------*/
#define DLL_IMPORT          __declspec(dllimport)   ///< ��DLL��������
#define DLL_EXPORT          __declspec(dllexport)   ///< ����������DLL
#define DLL_EXPORT_CLASS    __declspec(dllexport)   ///< �����ൽDLL
#define CDECL               __cdecl                 ///< ��׼C������������������ѹ���ջ��������Ҫ�����������ջ��Ϣ��add esp xxx��
#define STDCALL             __stdcall               ///< ��׼C++������������������ѹ���ջ�����Ǻ��������������ջ��ret xxx��
#define FASTCALL            __fastcall              ///< ����������ʹ�üĴ�������
#define FORCEINLINE         __forceinline           ///< ǿ��inline
#define APICALL             _far _pascal            ///< Windows API �������÷�ʽ
#define NOVTABLE            __declspec(novtable)    ///< ǿ�Ʋ��������
#define ASM_NAKED           __declspec(naked)       ///< ǿ����Ƕ��಻����Ĵ�����Ϣ

/** HASH_STR �ǹ�ϣһ���ַ����ļ�д��ʽ.
*/
#define HASH_STR(key, val) std::unordered_map<key, val>

/** HASH_MULTISTR �ǹ�ϣ����ַ����ļ�д��ʽ.
*/
#define HASH_MULTISTR(key, val) std::unordered_multimap<key, val>

#define HASH_QWORD(key, val) std::unordered_map<key, val>

#define HASH_MULTIQWORD(key, val) std::unordered_multimap<key, val>

namespace rt2_core {

	/*-----------------------------------------------------------------------------
	-   ǿ�ƹر�һЩVC++��Lv4�ľ���
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
#error "VC������ѡ�����: C++�쳣����(C++ exception handling)�����"
#endif

#ifdef _CHAR_UNSIGNED
#error "VC������ѡ�����: �ַ�(Characters)�������з��ŵ�"
#endif

								   /*-----------------------------------------------------------------------------
								   -   ����ϵͳ����ַ�������
								   -----------------------------------------------------------------------------*/
#ifdef _DEBUG
#define RT_COMPILER "������ Visual C++ Debug ģʽ"
#else
#define RT_COMPILER "������ Visual C++ Release ģʽ"
#endif
#define OS_TYPE_STR             "Windows"           ///< ����ϵͳ����
#define LINE_TERMINATOR         "\r\n"              ///< ���з�
#define PATH_SEPARATOR          "\\/"               ///< Ŀ¼�ָ��������п��ܣ�
#define PATH_SEPARATOR_S        "\\"                ///< Ŀ¼�ָ������ַ�����
#define PATH_SEPARATOR_C        '\\'                ///< Ŀ¼�ָ������ַ���

								   /*-----------------------------------------------------------------------------
								   -   �̣߳��ź�����ز���
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
#define RtResLockDestroy(p)                 /* �����κ��� */
#define RtResLockLock(p)                    while(::InterlockedExchange(p, 1)) { rtSleep(0.f); }
#define RtResLockTryLock(p)                 (!::InterlockedExchange(p, 1))
#define RtResLockUnlock(p)                  ::InterlockedExchange(p, 0)

#define RtInterlockedIncrement              ::InterlockedIncrement
#define RtInterlockedDecrement              ::InterlockedDecrement

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
