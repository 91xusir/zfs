// XTPApplication.h
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
#if !defined(__XTPAPPLICATION_H__)
#define __XTPAPPLICATION_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPApplication;

//-----------------------------------------------------------------------
// Summary:
//      Defines a callback interface for XTP application event sink.
//-----------------------------------------------------------------------
struct IXTPApplicationEvents
{
	//-----------------------------------------------------------------------
	// Summary:
	//      Is called by framework when application before application shutting down.
	// Parameters:
	//      pApplication - Event sending application pointer.
	//-----------------------------------------------------------------------
	virtual void OnBeforeApplicationShutdown(CXTPApplication* pApplication)
	{
		UNREFERENCED_PARAMETER(pApplication);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Is called by framework when application is being shutdown before
	//      the actual shutting down.
	// Parameters:
	//      pApplication - Event sending application pointer.
	//-----------------------------------------------------------------------
	virtual void OnApplicationShutdown(CXTPApplication* pApplication)
	{
		UNREFERENCED_PARAMETER(pApplication);
	}
};

//-----------------------------------------------------------------------
// Summary:
//      Defines a global XTP application object interface. The object is 
//      kept alive until a call to XTPShutdown. At any moment there is
//      one and only one XTP application instance per loaded dynamic or
//      executable module which links XTP framework.
// See also:
//      XTPGetApplication, XTPInitialize, XTPShutdown, CXTPWinApp
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPApplication
	: public CObject
	, public CXTPObservable<CXTPApplication, IXTPApplicationEvents>
	, public CXTPSynchronized
{
	friend class CXTPSingleton<CXTPApplication>;

	friend _XTP_EXT_CLASS CXTPApplication* AFX_CDECL XTPGetApplication();
	friend _XTP_EXT_CLASS BOOL AFX_CDECL XTPInitialize();
	friend _XTP_EXT_CLASS void AFX_CDECL XTPShutdown();

	DECLARE_DYNAMIC(CXTPApplication);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Returns module state pointer obtained at the point where XTPInitialize 
	//      or XTPGetApplication is first called.
	// Returns:
	//      Module state pointer.
	// See also:
	//      XTPInitialize, XTPGetApplication
	//-----------------------------------------------------------------------
	AFX_MODULE_STATE* GetModuleState();
	
	//{{AFX_CODEJOCK_PRIVATE
	~CXTPApplication();
	//}}AFX_CODEJOCK_PRIVATE
private:
	CXTPApplication();
	static CXTPApplication& AFX_CDECL GetInstance();

	void Shutdown();

	AFX_MODULE_STATE* m_pModuleState; // Module state
	BOOL m_bShutdown; // FALSE until XTPShutdown is called
};

//-----------------------------------------------------------------------
// Summary:
//      Initializes XTP framework. Must not be called after XTPShutdown call. 
//      For backward compatibility the call is optional, however future versions
//      may require it to be called before any other call to XTP framework.
//      CXTPWinApp calls it automatically in InitInstance.
// Returns:
//      TRUE if XTP framework is initialized successfully.
// See also:
//      XTPGetApplication, XTPShutdown, CXTPWinApp
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPInitialize();

//-----------------------------------------------------------------------
// Summary:
//      Obtains the only XTP application instance pointer. Must be called
//      between XTPInitialize and XTPShutdown calls.
// Returns:
//      The only XTP application instance pointer.
// See also:
//      XTPInitialize, XTPShutdown
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPApplication* AFX_CDECL XTPGetApplication();

//-----------------------------------------------------------------------
// Summary:
//      Performs XTP application shutting down with releasing all internal 
//      XTP pointers and resources but not terminating the calling thread.
//      The call to XTPShutdown must happen as late as possible at runtime
//      but no later than returning from WinMain or DllMain.
//      For backward compatibility it still is called by framework automatically
//      on static storage destruction but the order of this operation 
//      is not guaranteed.
//      CXTPWinApp calls it automatically in ExitInstance.
// See also:
//      XTPInitialize, XTPGetApplication, CXTPWinApp
//-----------------------------------------------------------------------
_XTP_EXT_CLASS void AFX_CDECL XTPShutdown();

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPAPPLICATION_H__)
