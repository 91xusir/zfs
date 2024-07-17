// XTPSkinManagerApiHook.h: interface for the CXTPSkinManagerApiHook class.
//
// (c)1998-2018 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSKINMANAGERAPIHOOK_H__)
#define __XTPSKINMANAGERAPIHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPWinThemeWrapper;

//{{AFX_CODEJOCK_PRIVATE

#define XTP_LDR_IS_DATAFILE(handle)      (((ULONG_PTR)(handle)) &  (ULONG_PTR)1)
#define XTP_LDR_IS_IMAGEMAPPING(handle)  (((ULONG_PTR)(handle)) & (ULONG_PTR)2)
#define XTP_LDR_IS_RESOURCE(handle)      (XTP_LDR_IS_IMAGEMAPPING(handle) || XTP_LDR_IS_DATAFILE(handle))


enum XTPSkinFrameworkApiFunctionIndex
{
	xtpSkinApiLoadLibraryA_KERNEL32,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiLoadLibraryW_KERNEL32,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiLoadLibraryExA_KERNEL32,
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiLoadLibraryExW_KERNEL32,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiFreeLibrary_KERNEL32,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiFreeLibraryAndExitThread_KERNEL32,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiGetProcAddress_KERNEL32,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiGetModuleHandleA_KERNEL32,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiGetModuleHandleW_KERNEL32,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiGetModuleHandleExA_KERNEL32,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiGetModuleHandleExW_KERNEL32,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinApiCreateThread_KERNEL32,
	xtpSkinApiCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1,
	xtpSkinApiCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2,
	xtpSkinApiRegisterClassA,
	xtpSkinApiRegisterClassW,
	xtpSkinApiGetSysColor,
	xtpSkinApiGetSysColorBrush,
	xtpSkinApiDrawEdge,
	xtpSkinApiDrawFrameControl,
	xtpSkinApiSetScrollInfo,
	xtpSkinApiSetScrollPos,
	xtpSkinApiGetScrollInfo,
	xtpSkinApiEnableScrollBar,
	xtpSkinApiOpenThemeData,
	xtpSkinApiDrawThemeBackground,
	xtpSkinApiCloseThemeData,
	xtpSkinApiGetThemeColor,
	xtpSkinApiGetThemeInt,
	xtpSkinApiIsAppThemed,
	xtpSkinApiIsThemeActive,
	xtpSkinApiGetCurrentThemeName,
	xtpSkinApiGetThemeSysBool,
	xtpSkinApiGetThemeSysColor,
	xtpSkinApiGetThemePartSize,
	xtpSkinApiSystemParametersInfoA,
	xtpSkinApiSystemParametersInfoW,
	xtpSkinApiDefWindowProcA,
	xtpSkinApiDefWindowProcW,
	xtpSkinApiDefMDIChildProcA,
	xtpSkinApiDefMDIChildProcW,
	xtpSkinApiDefFrameProcA,
	xtpSkinApiDefFrameProcW,
	xtpSkinApiDefDlgProcA,
	xtpSkinApiDefDlgProcW,
	xtpSkinApiCallWindowProcA,
	xtpSkinApiCallWindowProcW,
	xtpSkinApiFillRect,
	xtpSkinApiDeleteObject,
	xtpSkinApiAdjustWindowRectEx,
	xtpSkinApiAdjustWindowRect,
	xtpSkinApiGetSystemMetrics,
	xtpSkinApiTrackPopupMenu,
	xtpSkinApiTrackPopupMenuEx_USER32,
	xtpSkinApiNtUserTrackPopupMenuEx_WIN32U,
	xtpSkinApiDrawMenuBar,
	xtpSkinApiGetMenuItemRect,
	xtpSkinApiDwmExtendFrameIntoClientArea,
	XTP_SKIN_APIHOOKCOUNT
};


class CXTPSkinManagerApiHook;
class CXTPSkinManagerModuleList;

class _XTP_EXT_CLASS CXTPSkinManagerApiFunction : public CXTPSynchronized
{
public:
	CXTPSkinManagerApiFunction(CXTPSkinManagerApiHook* pApiHook,
		LPCSTR  pszCalleeModName, LPCSTR pszFuncName, PROC pfnOrig, PROC pfnHook, PROC pfnAltOrig = NULL);

	virtual ~CXTPSkinManagerApiFunction();

public:
	BOOL HookImport(CXTPSkinManagerModuleList* pModuleList);
	BOOL UnhookImport(CXTPSkinManagerModuleList* pModuleList);
	BOOL UnhookImport(HMODULE hmod);
	BOOL IsHooked() const;
	PROC GetOriginal() const;

private:
	BOOL ReplaceInOneModule(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller, PROC pfnAltCurrent = NULL);
	BOOL ReplaceInAllModules(LPCSTR pszCalleeModName, 
		PROC pfnCurrent, PROC pfnNew, CXTPSkinManagerModuleList* pModuleList, 
		PROC pfnAltCurrent = NULL);

private:
	CXTPSkinManagerApiHook* m_pApiHook;
	BOOL              m_bHooked;
	char              m_szCalleeModName[50];
	PROC              m_pfnOrig;
	PROC              m_pfnAltOrig;
	PROC              m_pfnHook;
	char              m_szFuncName[30];

	static PVOID          sm_pvMaxAppAddr;
	static PVOID          sm_pfnAfxWndProc;

	friend class CXTPSkinManagerApiHook;
	friend class CXTPSkinObject;
};

AFX_INLINE BOOL CXTPSkinManagerApiFunction::IsHooked() const {
	return m_bHooked;
}

AFX_INLINE PROC CXTPSkinManagerApiFunction::GetOriginal() const {
	return m_pfnOrig;
}


class _XTP_EXT_CLASS CXTPSkinManagerApiHook : public CXTPSynchronized
{
	struct EXCLUDEDMODULE
	{
		HMODULE hModule;
		CString strModule;
		BOOL bWin9x;
	};

	struct APIHOOKINFO
	{
		XTPSkinFrameworkApiFunctionIndex nIndex;
		LPCSTR lpModuleName;
		LPCSTR lpRoutineName;
		PROC pfnHook;
		int nApplyOptions;
		bool(CXTPSystemVersion::* pfnVersionCheck)() const;
	};

private:
	CXTPSkinManagerApiHook();

public:
	static CXTPSkinManagerApiHook* AFX_CDECL GetInstance();
	virtual ~CXTPSkinManagerApiHook();

public:
	void ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x);
	BOOL IsModuleExcluded(HMODULE hModule) const;

public:
	void InitializeHookManagement();
	void FinalizeHookManagement();

public:
	CXTPSkinManagerApiFunction* GetHookedFunction(LPCSTR pszCalleeModName, LPCSTR pszFuncName);
	CXTPSkinManagerApiFunction* GetHookedFunction(XTPSkinFrameworkApiFunctionIndex nIndex);
	static PROC AFX_CDECL GetOriginalProc(XTPSkinFrameworkApiFunctionIndex nIndex);

public:
	static FARPROC WINAPI GetProcAddressWindows(HMODULE hmod, LPCSTR pszProcName);
	static FARPROC WINAPI GetProcAddressManual(HMODULE hmod, LPCSTR pszProcName);
	static HMODULE WINAPI GetModuleHandleAWindows(LPCSTR pszModuleName);

	static BOOL WINAPI AdjustWindowRectExOrig(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);

	static LRESULT WINAPI CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	static BOOL AFX_CDECL IsSystemWindowModule(WNDPROC lpWndProc);

	static BOOL AFX_CDECL IsHookedByAppHelpDll(PROC lpProc);

	void ConnectWrapper(CXTPWinThemeWrapper* pThemeWrapper);

protected:
	void UnhookAllFunctions(BOOL bFinal);
	void UnhookAllFunctions(HMODULE hModule);
	CXTPSkinManagerApiFunction* HookImport(XTPSkinFrameworkApiFunctionIndex nIndex, 
		LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC  pfnHook,
		CXTPSkinManagerModuleList* pModuleList);

private:
	void ProcessExcludedModules();
	void IncrementAllModulesReferenceCounters(CXTPSkinManagerModuleList* pModuleList, BOOL bIncrement = TRUE);

	CXTPSkinManagerApiFunction* AddHook(XTPSkinFrameworkApiFunctionIndex nIndex, 
		LPCSTR  pszCalleeModName, LPCSTR  pszFuncName, PROC pfnOrig, PROC pfnHook,
		CXTPSkinManagerModuleList* pModuleList);

	BOOL RemoveHook(LPCSTR pszCalleeModName, LPCSTR pszFuncName);

	void HackModuleOnLoad(HMODULE hmod, DWORD dwFlags);

	static BOOL AFX_CDECL EnterHookedCall();
	static CXTPSkinManagerApiFunction* AFX_CDECL EnterHookedCall(LPCSTR pszCalleeModName, LPCSTR pszFuncName);
	static CXTPSkinManagerApiFunction* AFX_CDECL EnterHookedCall(XTPSkinFrameworkApiFunctionIndex nIndex);

	static HMODULE WINAPI OnHookLoadLibraryA_KERNEL32(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_KERNEL32(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryExA_KERNEL32(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_KERNEL32(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static BOOL WINAPI OnHookFreeLibrary_KERNEL32(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_KERNEL32(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(HMODULE hmod, DWORD dwExitCode);
	static FARPROC WINAPI OnHookGetProcAddress_KERNEL32(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(HMODULE hmod, PCSTR pszProcName);
	static HMODULE WINAPI OnHookGetModuleHandleA_KERNEL32(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_KERNEL32(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleExA_KERNEL32(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_KERNEL32(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HANDLE WINAPI OnHookCreateThread_KERNEL32(LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
	static HANDLE WINAPI OnHookCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1(LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
	static HANDLE WINAPI OnHookCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2(LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);

	static HMODULE WINAPI OnHookLoadLibraryA(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryExA(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static FARPROC WINAPI OnHookGetProcAddress(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, HMODULE hmod, PCSTR pszProcName);
	static BOOL WINAPI OnHookFreeLibrary(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, HMODULE hmod, DWORD dwExitCode);
	static HMODULE WINAPI OnHookGetModuleHandleA(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleExA(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
	static HANDLE STDAPICALLTYPE OnHookCreateThread(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, 
		LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, 
		LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);

	static ATOM WINAPI OnHookRegisterClassA( const WNDCLASSA *lpWndClass);
	static ATOM WINAPI OnHookRegisterClassW( const WNDCLASSW *lpWndClass);
	static int WINAPI OnHookSetScrollInfo(HWND hwnd, int fnBar, LPCSCROLLINFO lpsi, BOOL fRedraw);
	static int WINAPI OnHookSetScrollPos(HWND hwnd,  int nBar,   int nPos,  BOOL redraw);
	static BOOL WINAPI OnHookAdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
	static BOOL WINAPI OnHookAdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
	static int WINAPI OnHookGetSystemMetrics(int nIndex);
	static BOOL WINAPI OnHookEnableScrollBar(HWND hWnd, UINT wSBflags, UINT wArrows);
	static BOOL WINAPI OnHookGetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO lpsi);
	static DWORD WINAPI OnHookGetSysColor(int nIndex);
	static BOOL WINAPI OnHookDeleteObject(HGDIOBJ hObject);
	static BOOL WINAPI OnHookDrawEdge (HDC hdc, LPRECT qrc, UINT edge, UINT grfFlags);
	static BOOL WINAPI OnHookDrawFrameControl(HDC hDC, LPRECT rect, UINT, UINT);
	static HBRUSH WINAPI OnHookGetSysColorBrush(int nIndex);
	static BOOL WINAPI OnHookSystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
	static BOOL WINAPI OnHookSystemParametersInfoW(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
	static LRESULT WINAPI OnHookDefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefFrameProcA(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefFrameProcW(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefDlgProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefMDIChildProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefMDIChildProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookCallWindowProcA(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookCallWindowProcW(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static int WINAPI OnHookFillRect( HDC hDC, CONST RECT *lprc, HBRUSH hbr);
	static BOOL WINAPI OnHookTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, CONST RECT *prcRect);
	static BOOL WINAPI OnHookTrackPopupMenuEx(XTPSkinFrameworkApiFunctionIndex nFunctionIndex, HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookTrackPopupMenuEx_USER32(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookNtUserTrackPopupMenuEx_WIN32U(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookDrawMenuBar(HWND hWnd);
	static BOOL WINAPI OnHookGetMenuItemRect(HWND hWnd, HMENU hMenu, UINT uItem, LPRECT lprcItem);
	static HRESULT STDAPICALLTYPE OnHookGetCurrentThemeName(LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR /*pszSizeBuff*/, int /*cchMaxSizeChars*/);
	static BOOL STDAPICALLTYPE OnHookGetThemeSysBool(HTHEME /*hTheme*/, int iBoolId);
	static COLORREF STDAPICALLTYPE OnHookGetThemeSysColor(HTHEME /*hTheme*/, int iColorId);
	static HRESULT STDAPICALLTYPE OnHookGetThemePartSize(HTHEME hTheme, HDC, int iPartId, int iStateId, RECT *pRect, THEMESIZE eSize, SIZE* pSize);
	static BOOL STDAPICALLTYPE OnHookIsAppThemed();
	static BOOL STDAPICALLTYPE OnHookIsThemeActive();
	static HRESULT STDAPICALLTYPE OnHookGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropID, COLORREF *pColor);
	static HRESULT STDAPICALLTYPE OnHookGetThemeInt(HTHEME hTheme, int iPartId, int iStateId, int iPropID, int *piVal);
	static HRESULT STDAPICALLTYPE OnHookCloseThemeData(HTHEME);
	static HRESULT STDAPICALLTYPE OnHookDrawThemeBackground(HTHEME hTheme, HDC hDC, int iPartId, int iStateId, const RECT* pRect, const RECT*);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeData(HWND /*hWnd*/, LPCWSTR pszClassList);
	static DWORD WINAPI ThreadProcHook(LPVOID lpParameter);
	static HRESULT WINAPI OnHookDwmExtendFrameIntoClientArea(HWND hWnd, LPCVOID pMarInset);

	BOOL CallHookDefWindowProc(HWND hWnd, PROC pfnOrig, XTPSkinDefaultProc defProc, LPVOID lpPrev, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	LONG IncrementModuleReferenceCounter(HMODULE hModule);
	LONG DecrementModuleReferenceCounter(HMODULE hModule);

protected:
	CXTPSkinManagerApiFunction* m_arrFunctions[XTP_SKIN_APIHOOKCOUNT];
	static BOOL m_bInitialized;
	static BOOL m_bInitializationInProgress;
	CArray<EXCLUDEDMODULE, EXCLUDEDMODULE&> m_arrExcludedModules;
	BOOL m_bInAppCompatMode; // Win7+ only.
	long m_nLockCondition;
	CMap<HMODULE, HMODULE, LONG, LONG> m_mapModuleRefCount;
	CXTPThreadLocal<int> m_HackModuleOnLoadCounter;
	static const APIHOOKINFO m_apiHookInfos[];

	friend class CXTPSkinManagerApiFunction;
	friend class CXTPSkinManager;
};

//}}AFX_CODEJOCK_PRIVATE


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGERAPIHOOK_H__)
