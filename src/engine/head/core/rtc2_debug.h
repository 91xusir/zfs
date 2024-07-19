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
