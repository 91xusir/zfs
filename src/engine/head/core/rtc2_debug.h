/**
* @file rtc2_debug.h 和Debug相关函数和宏.
* @author janhail
*/

#include "assert.h"

namespace rt2_core {

///////////////////////////////////////////////////////////////////////////////
// Function.
///////////////////////////////////////////////////////////////////////////////

typedef void (*RTCBGuardError)(const char* szMsg);

RT2_CORE_API void rtFailAssert( const char* pExpr, const char* pFile, int iLine );
RT2_CORE_API const char* rtGetSystemErrorMessage( int iError=0 );
RT2_CORE_API const void rtDebugMessagef( const char* pFmt, ... );
RT2_CORE_API void rtUnwindf( const char* pFmt, ... );
RT2_CORE_API const void rtGuardEnd( const char* pAppname );
RT2_CORE_API void rtSetGuardErrorCallback(RTCBGuardError cbGuardError);

#ifdef WIN32
inline void rtDebugBreak()
{    
	__asm
    {
        int 3
    }
}

#else
inline void rtDebugBreak()
{
	// __asm
	// {
		// assert(0);
	// }
}
#endif

/*
#ifdef _DEBUG 检查是否定义了 _DEBUG 宏。这个宏通常在调试构建中定义，但在发布构建中未定义。
如果 _DEBUG 被定义，RTASSERT(exp) 会被定义为 assert(exp)。assert 是 C++ 标准库中的一个宏，用于在调试时验证表达式 exp 的正确性。如果表达式为 false，程序会中断并打印错误信息。
如果 _DEBUG 没有定义，则 RTASSERT(exp) 为空宏，这意味着断言不会在发布版本中检查，从而避免了运行时开销。
*/
#ifdef _DEBUG
#define RTASSERT(exp) assert(exp)
#else
#define RTASSERT(exp)
#endif

//
// For throwing string-exceptions which safely propagate through guard/unguard.
//
RT2_CORE_API void rtThrowf( const char* pFmt, ... );

#if DO_CHECK
#   define CHECK(exp)       (void)( (exp) || (rtFailAssert(#exp, __FILE__, __LINE__), 0) )
#   define CHECKEX(exp)     rtFailAssert(exp, __FILE__, __LINE__)
//#   define RTASSERT(exp)    (void)( (exp) || (rtDebugBreak(), 0) )
#   define VERIFYEXP(exp)  (void)( (exp) || (rtFailAssert(#exp, __FILE__, __LINE__), 0) )
#else
#   define CHECK(exp)       ((void)0)
#   define CHECKEX(exp)     ((void)0)
//#   define RTASSERT(exp)    ((void)0)
#   define VERIFYEXP(exp)  (exp)
#endif

#ifdef DO_DEBUG
#   define FORDEBUG(exp)    exp
#else
#   define FORDEBUG(exp)    ((void)0)
#endif

#if DO_STAT
#   define STAT(exp)    exp
#else
#   define STAT(exp)    ((void)0)
#endif

#define funguard           // {static const char __FUNC_NAME__[]=TEXT(__FUNCTION__); try{
#define funguard_name(func)// {static const char __FUNC_NAME__[]=TEXT(#func); try{
#define fununguard         // }catch(const int *Err){throw Err;}catch(...){rtUnwindf(TEXT("%s"),__FUNC_NAME__); throw;}}
#define fununguardf(msg)   // }catch(const int *Err){throw Err;}catch(...){rtUnwindf(TEXT("%s"),__FUNC_NAME__); rtUnwindf msg; throw;}}
#define funguard_begin(app)// {static const char __FUNC_NAME__[]=TEXT(#app); try{
#define funguard_end       // }catch(...){try{rtGuardEnd(__FUNC_NAME__);}catch( ...){}}}
#define funguard_end_ex   //  }catch(...){rtGuardEnd(__FUNC_NAME__);throw;}}
#define finalfununguard	//	}catch(...){try{rtUnwindf(TEXT("%s"),__FUNC_NAME__);rtGuardEnd(__FUNC_NAME__);}catch( ...){}}}
} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
