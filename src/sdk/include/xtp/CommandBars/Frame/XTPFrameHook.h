// XTPFrameHook.h: interface for the CXTPCommandBarsFrameHook class.
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
#if !defined(__XTPCOMMANDBARSFRAMEHOOK_H__)
#define __XTPCOMMANDBARSFRAMEHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPRibbonBar;
class CXTPResourceTheme;
class CXTPControls;
class CXTPFramePaintManager;
typedef DWORD XTP_NOTIFY_CODE;
class CXTPNotifySink;
class CXTPFrameShadow;

//===========================================================================
// Summary:
//     CXTPCommandBarsFrameHook is CXTPHookManagerHookAble derived class is helper
//     for Office 2007 window skinning
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarsFrameHook 
	: public CXTPHookManagerHookAble
	, IXTPPaintManagerEvents
	, IXTPFrameShadowManagerEvents
{
	class CControlCaptionButton;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarsFrameHook object
	//-----------------------------------------------------------------------
	CXTPCommandBarsFrameHook();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBarsFrameHook object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPCommandBarsFrameHook();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable frame theming
	// Parameters:
	//     pSite         - Window to theme
	//     pPaintManager - Paint manager used to theme window
	//     pCommandBars  - Parent CommandBars object
	//-----------------------------------------------------------------------
	void EnableFrameTheme(CXTPCommandBars *pCommandBars);
	void EnableFrameTheme(CWnd *pSite, CXTPPaintManager *pPaintManager);
	void EnableFrameTheme(CWnd *pSite, CXTPCommandBars *pCommandBars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to disable frame theming
	//-----------------------------------------------------------------------
	void DisableFrameTheme();

//{{AFX_CODEJOCK_PRIVATE
    _XTP_DEPRECATE("This function is no longer available. Please use 'EnableFrameTheme' instead")
	void EnableOffice2007Frame(CXTPCommandBars *pCommandBars);
    _XTP_DEPRECATE("This function is no longer available. Please use 'EnableFrameTheme' instead")
	void EnableOffice2007Frame(CWnd *pSite, CXTPPaintManager *pPaintManager);
    _XTP_DEPRECATE("This function is no longer available. Please use 'EnableFrameTheme' instead")
	void EnableOffice2007Frame(CWnd *pSite, CXTPCommandBars *pCommandBars);
    _XTP_DEPRECATE("This function is no longer available. Please use 'DisableFrameTheme' instead")
	void DisableOffice2007Frame();
//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Skinned window
	//-----------------------------------------------------------------------
	CWnd* GetSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Paint Manager used to skin window
	//-----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary: Call this method to retrieve the paint manager of the frame.
	// Returns: A pointer to a CXTPPaintManager object.
	//-----------------------------------------------------------------------
	CXTPFramePaintManager* GetFramePaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if caption of skinned window is visible
	//-----------------------------------------------------------------------
	BOOL IsCaptionVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Skinned window is active
	//-----------------------------------------------------------------------
	BOOL IsFrameActive() const;

	//-------------------------------------------------------------------------
	// Summary:
	//      Recalculates borders and region of frame
	//-------------------------------------------------------------------------
	void RecalcFrameLayout();

	//-------------------------------------------------------------------------
	// Summary:
	//      Recalculates borders and region of frame with small delay
	//-------------------------------------------------------------------------
	void DelayRecalcFrameLayout();

	//-------------------------------------------------------------------------
	// Summary:
	//      Redraw ribbon bar if found
	// See Also: RedrawFrame
	//-------------------------------------------------------------------------
	void RedrawRibbonBar();

	//-------------------------------------------------------------------------
	// Summary:
	//     Redraws frame of skinned window
	// See Also: RedrawRibbonBar
	//-------------------------------------------------------------------------
	void RedrawFrame();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame has skinned scrollbar to draw rounded rects.
	// Parameters:
	//      pnStatusHeight - Height of status bar to return.
	// Returns: TRUE if frame has skinned status bar.
	// See Also: IsMDIMaximized
	//-----------------------------------------------------------------------
	BOOL IsFrameHasStatusBar(int* pnStatusHeight = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame is maximized child window.
	// Returns: TRUE if frame is maximized child window.
	//-----------------------------------------------------------------------
	BOOL IsMDIMaximized() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns collection of caption buttons.
	// Returns: Collection of caption buttons.
	//-----------------------------------------------------------------------
	CXTPControls* GetCaptionButtons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates height of caption
	// Returns:
	//     Height of caption in pixels
	// See Also: GetFrameBorder
	//-----------------------------------------------------------------------
	int GetCaptionHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates size of frame borders
	// Returns: Size of frame borders  in pixels
	// See Also: GetCaptionHeight
	//-----------------------------------------------------------------------
	CRect CalcFrameBorder() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Vista AERO theme is enabled
	// Returns:
	//     TRUE if Vista AERO theme is enabled
	//-----------------------------------------------------------------------
	BOOL IsDwmEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Disables Vista AERO theme.
	//-----------------------------------------------------------------------
	void DisableDwm();

	//-----------------------------------------------------------------------
	// Summary: Returns style of site window
	// Returns: Style of site window.
	//-----------------------------------------------------------------------
	DWORD GetSiteStyle() const;

	//-----------------------------------------------------------------------
	// Summary: Returns extended style of site window
	// Returns: Extended style of site window.
	//-----------------------------------------------------------------------
	DWORD GetSiteExStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the ribbon frame if EnableFrameTheme was not called.
	// Parameters:
	//     pDC -  Pointer to a valid device context
	//     pRibbonBar - Ribbon Bar to draw
	//-----------------------------------------------------------------------
	void DrawRibbonFramePart(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the ribbon frame if EnableFrameTheme was not called.
	// Parameters:
	//     pDC -  Pointer to a valid device context
	//     pRibbonBar - Ribbon Bar to draw
	//-----------------------------------------------------------------------
	void UpdateDwmClientArea();

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables the previously disabled frame shadow.
	// Returns:
	//     TRUE if a shadow has been successfully enabled, otherwise FALSE.
	// See Also: DisableShadow, GetShadow
	//-----------------------------------------------------------------------
	BOOL EnableShadow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Disables frame shadow if enabled.
	// See Also: EnableShadow, GetShadow
	//-----------------------------------------------------------------------
	void DisableShadow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains frame shadow pointer.
	// Returns:
	//     A valid frame shadow pointer if frame shadow is supported and enabled,
	//     otherwise the return value is NULL.
	// See Also: EnableShadow, DisableShadow
	//-----------------------------------------------------------------------
	CXTPFrameShadow* GetShadow() const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by WindowProc, or is called during message reflection.
	// Parameters:
	//     hWnd - Window handle that the message belongs to.
	//     nMessage - Specifies the message to be sent.
	//     wParam - Specifies additional message-dependent information.
	//     lParam - Specifies additional message-dependent information.
	//     lResult - The return value of WindowProc. Depends on the message; may be NULL.
	// Returns:
	//     TRUE if message was processed.
	//-----------------------------------------------------------------------
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets invoked for MDI children windows as a result to call to 
	//     CXTPMDIFrameWnd::UpdateMDIChildrenTheme.
	//-----------------------------------------------------------------------
	virtual void OnUpdateMDIChildrenTheme();

protected:

//{{AFX_CODEJOCK_PRIVATE
	// IXTPPaintManagerEvents overrides
	virtual void OnPaintManagerDestroyed(CXTPPaintManager* pPaintManager);

	// IXTPFrameShadowManagerEvents overrides
	virtual void OnFrameShadowDisabled(
		CXTPFrameShadowManager* pManager,
		HWND hwndOwner,
		CXTPFrameShadow* pShadow);

	void TrackCaptionButton(CXTPControl* pSelected);
	void UpdateFrameRegion();
	void UpdateFrameRegion(CSize szFrameRegion, BOOL bUpdate = FALSE);
	void RepositionCaptionButtons();
	void AddCaptionButton(int nID, int nHit);
	void ScreenToFrame(LPPOINT lpPoint);
	CXTPControl* HitTestCaptionButton(CPoint point);
	void RefreshFrameStyle();
	void SetWinHook();
	BOOL GetAutoHideBar() const;
	void OnResourceImagesChanged(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
//}}AFX_CODEJOCK_PRIVATE

public:
	static UINT m_nMsgQueryFrameHook;   // Helper message to determine if Hook was attached.
	BOOL m_bLockNCPaint;                // TRUE to disable repaint of non client area

	static BOOL m_bAllowDwm;

	//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATE("Value ignored. Use frame shadows in combination with sizeable frame instead.")
	static BOOL m_bAllowBorderMouseTracking;
	//}}AFX_CODEJOCK_PRIVATE

protected:
	HWND m_hwndSite;                    // Skinned window handle
	CXTPRibbonBar* m_pRibbonBar;        // Child Ribbonbar object
	CXTPCommandBars* m_pCommandBars;    // Attached commandbars object
	CXTPPaintManager* m_pPaintManager;  // PaintManager used to draw frame
	BOOL m_bUseGlobalPaintManager;      // If TRUE, a global paint manager has to be used to draw frame
	BOOL m_bActive;                     // TRUE if frame is active
	CSize m_szFrameRegion;              // Previous size of window
	BOOL m_bDelayReclalcLayout;         // TRUE to recalculate layout after small delay
	CXTPControls* m_pCaptionButtons;    // Caption buttons
	CXTPControl* m_pSelected;           // Selected caption button
	BOOL m_bButtonPressed;              // Pressed caption button
	BOOL m_bThemeFrameEnabled;          // TRUE if skinning is enabled
	BOOL m_nSkipNCPaint;                // TRUE to skip frame drawing
	BOOL m_bInUpdateFrame;              // TRUE if UpdateFrame method is executed
	BOOL m_bDwmEnabled;                 // TRUE if Vista AERO theme is enabled
	BOOL m_bBeingDestructed;            // TRUE if class instance is being destructructed
	BOOL m_bToolBarComposition;
	mutable CXTPFrameShadow* m_pShadow; // Optional. Frame shadow pointer.

private:
	UINT m_nMsgUpdateSkinState;
	UINT m_nMsgQuerySkinState;

	CXTPNotifySink* m_pSink;

	static void CALLBACK WinEventProc(CXTPWinEventHook::HWINEVENTHOOK hWinEventHook,    DWORD event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

	void SetSiteWindow(HWND hWnd = NULL);

private:
	class CWinEventHook;
	friend class CXTPRibbonBar;
	friend class CControlCaptionButton;
	friend class CXTPCommandBars;
};

// Obsolete name
#define CXTPOffice2007FrameHook CXTPCommandBarsFrameHook

AFX_INLINE CWnd* CXTPCommandBarsFrameHook::GetSite() const
{
	return CWnd::FromHandle(m_hwndSite);
}

AFX_INLINE BOOL CXTPCommandBarsFrameHook::IsFrameActive() const
{
	return m_bActive;
}

AFX_INLINE CXTPControls* CXTPCommandBarsFrameHook::GetCaptionButtons() const
{
	return m_pCaptionButtons;
}

AFX_INLINE BOOL CXTPCommandBarsFrameHook::IsDwmEnabled() const
{
	return m_bDwmEnabled;
}

AFX_INLINE void CXTPCommandBarsFrameHook::DisableDwm()
{
	m_bDwmEnabled = FALSE;
}

AFX_INLINE void CXTPCommandBarsFrameHook::SetSiteWindow(HWND hWnd /*= NULL*/)
{
	m_hwndSite = hWnd;
}

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSFRAMEHOOK_H__)
