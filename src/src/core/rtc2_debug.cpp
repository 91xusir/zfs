
#include "core/rt2_core.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

namespace rt2_core {

static char g_szRunStackBuffer[2*1024] = "Application Crash\n";
static char g_szLogStaticBuffer[1024];

/*------------------------------------------------------------------------
	Error
------------------------------------------------------------------------*/

BOOL AssertFailedLine(const char* pExp, const char* pFileName, int nLine)
{
    static RtResLock lock;

    TCHAR szMessage[_MAX_PATH*3];
    sprintf(szMessage, "Assertion Failed: Exp [%s], File [%s], Line [%d]\n", pExp, pFileName, nLine);
    RtCoreLog().Error(szMessage);

    RtResLock::AutoLock autoLock(lock);
#ifdef __WIN32__
    int nCode = IDRETRY;
    HWND hWndParent = GetActiveWindow();
    if (hWndParent != NULL) hWndParent = GetLastActivePopup(hWndParent);

    // we remove WM_QUIT because if it is in the queue then the message box won't display
    MSG msg;
    BOOL bQuit = ::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);

    // display the assert
    sprintf(szMessage, "%s:\nExp [%s]\nFile [%s]\nLine [%d]", RtCore::Instance().strName.c_str(), pExp, pFileName, nLine);
    nCode = ::MessageBox(hWndParent, szMessage, "Assertion Failed!", MB_TASKMODAL|MB_ICONHAND|MB_ABORTRETRYIGNORE|MB_SETFOREGROUND);
    if (bQuit) PostQuitMessage(msg.wParam);
    if (nCode == IDIGNORE) return FALSE;   // ignore
    if (nCode == IDRETRY)
    {
        rtDebugBreak();
        return TRUE;    // will cause rtDebugBreak
    }
#endif

    rtDebugBreak();
    return TRUE;
}

RT2_CORE_API void rtFailAssert( const char* pExpr, const char* pFile, int iLine )
{
#ifdef _DEBUG
    AssertFailedLine(pExpr, pFile, iLine);
#else
    rtThrowf("File: \"%s\" Line: %d \nAssert: %s\n", pFile, iLine, pExpr);
#endif
}

RT2_CORE_API const char* rtGetSystemErrorMessage( int iError )
{
    return 0;
}

RT2_CORE_API const void rtDebugMessagef( const char* pFmt, ... )
{
}

RT2_CORE_API void rtUnwindf( const char* pFmt, ... )
{
	// Maybe memory overflow here. Louis @ Mon 31 Aug 2009
	int rsbLen = strlen(g_szRunStackBuffer);

	if ((rsbLen + 1) >= 2048)
		return;
	
    strcat(g_szRunStackBuffer, "\n");
    char* pBuf = g_szRunStackBuffer;
    pBuf += strlen(pBuf);
	va_list argList;
	va_start(argList, pFmt);
	vsnprintf(pBuf, 2047 - rsbLen, pFmt, argList);
	va_end(argList);
}

RTCBGuardError  g_cbGuardError = NULL;

RT2_CORE_API void rtSetGuardErrorCallback(RTCBGuardError cbGuardError)
{
    g_cbGuardError = cbGuardError;
}

RT2_CORE_API const void rtGuardEnd( const char* pAppname )
{
    strcat(g_szRunStackBuffer, "\n *** End ***\n");
    RtCoreLog().Info(g_szRunStackBuffer);
#ifdef WIN32
	MessageBox(0,g_szRunStackBuffer,"error",MB_OK);
#endif
    if (g_cbGuardError)
    {
        g_cbGuardError(g_szRunStackBuffer);
    }else
    {
    }
}

RT2_CORE_API void rtThrowf( const char* pFmt, ... )
{
	va_list argList;
	va_start(argList, pFmt);
	vsnprintf(g_szLogStaticBuffer, 1023, pFmt, argList);
	va_end(argList);
    rtUnwindf(g_szLogStaticBuffer);

    // 临时设置成只有Windows下才会当出，这是为了在正式服务器上运行的时候比较稳定
#ifdef __WIN32__
    throw g_szLogStaticBuffer;
#endif
}


} // namespace rt2_core

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
