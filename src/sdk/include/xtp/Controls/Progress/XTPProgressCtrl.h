// XTPProgressCtrl.h interface for the CXTPProgressCtrl class.
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
#if !defined(__XTPPROGRESSCTRL_H__)
#define __XTPPROGRESSCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifndef PBS_MARQUEE
#define PBS_MARQUEE             0x08
#endif

#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE          (WM_USER+10)
#endif

class CXTPProgressCtrlPaintManager;

//===========================================================================
// Summary:
//     The ProgressBar control shows the progress of a lengthy operation by
//     filling a rectangle with chunks from left to right.
// Remarks:
//      A ProgressBar control has a range and a current position.  The range
//      represents the entire duration of the operation.  The current position
//      represents the progress the application has made toward completing the
//      operation.  The Max and Min properties set the limits of the range.
//      The Value property specifies the current position within that range.
//      Because chunks are used to fill in the control, the amount filled in
//      only approximates the Value property's current setting.  Based on the
//      control's size, the Value property determines when to display the next chunk.
//===========================================================================
class _XTP_EXT_CLASS CXTPProgressCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CXTPProgressCtrl)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPProgressCtrl object
	//-----------------------------------------------------------------------
	CXTPProgressCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Handles cleanup, destroys object
	//-----------------------------------------------------------------------
	virtual ~CXTPProgressCtrl();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function returns a pointer to the associated paint
	//     manager.
	// Remarks:
	//     Call this member function to obtain a pointer to the paint manager
	//     object. The paint manager object is used for drawing the date
	//     picker window.
	// Returns:
	//     Pointer to the paint manager object.
	// See Also:
	//     SetTheme, SetPaintManager
	// ----------------------------------------------------------------------
	CXTPProgressCtrlPaintManager* GetPaintManager() const;

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function sets the new control drawing theme.
	// Parameters:
	//     pPaintManager :  Pointer of the new paint manager object or derived
	//                      class. If NULL - the default paint manager object is set.
	// Remarks:
	//     Call this member function to set the paint manager object that is
	//     used for drawing a date picker window.
	// See Also:
	//     GetPaintManager
	// ---------------------------------------------------------------------
	void SetTheme(CXTPProgressCtrlPaintManager* pPaintManager = NULL);

	// -----------------------------------------------------------------
	// Summary:
	//    This member function sets the border color for the progress bar.
	// Parameters:
	//    clrNew  - COLORREF value that specifies the new border color.
	// Returns:
	//     COLORREF value used for the bar.
	// -----------------------------------------------------------------
	COLORREF SetBorderColor(COLORREF clrNew);

	// -----------------------------------------------------------------
	// Summary:
	//    This member function sets the bar color for the progress bar.
	// Parameters:
	//    clrNew  - COLORREF value that specifies the new bar color.
	// Returns:
	//     COLORREF value used for the bar.
	// -----------------------------------------------------------------
	COLORREF SetBarColor(COLORREF clrNew);

	// -----------------------------------------------------------------
	// Summary:
	//    This member function sets the background color for the progress bar.
	// Parameters:
	//    clrNew  - COLORREF value that specifies the new background color.
	// Returns:
	//     COLORREF value used for the background.
	// -----------------------------------------------------------------
	COLORREF SetBkColor(COLORREF clrNew);

	// -----------------------------------------------------------------
	// Summary:
	//    This member function sets the text color for the progress bar.
	// Parameters:
	//    clrNew  - COLORREF value that specifies the new text color.
	// Returns:
	//     COLORREF value used for text.
	// -----------------------------------------------------------------
	COLORREF SetTextColor(COLORREF clrNew);

	// -----------------------------------------------------------------
	// Summary:
	//     Turns marquee mode on or off for the current progress bar
	//     control.
	// Parameters:
	//     fMarqueeMode - TRUE to turn marquee mode on, or FALSE to turn
	//                    marquee mode off.
	//     nInterval    - Time in milliseconds between updates of the
	//                    marquee animation.
	// Example:
	//    The following code example starts and stops the marquee
	//    scrolling animation.
	// <code>
	// int piAlpha[5] = { 25, 50, 75, 100, 100 };
	// m_wndProgress.SetMarquee(TRUE, 50);
	// </code>
	// -----------------------------------------------------------------
	BOOL SetMarquee(BOOL fMarqueeMode, int nInterval);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the visual theme of the control.
	// Parameters:
	//     nTheme - New visual theme. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     nStyle can be one of the following:
	//     * <b>xtpControlThemeDefault</b> Standard appearance style.
	//     * <b>xtpControlThemeFlat</b> Flat appearance style.
	//     * <b>xtpControlThemeUltraFlat</b> Ultra flat appearance style.
	//     * <b>xtpControlThemeOffice2000</b> Office 2000 appearance style.
	//     * <b>xtpControlThemeOfficeXP</b> Office XP appearance style.
	//     * <b>xtpControlThemeOffice2003</b> Office 2003 appearance style.
	//     * <b>xtpControlThemeResource</b> Office 2007 appearance style.
	//-----------------------------------------------------------------------
	void SetTheme(XTPControlTheme nTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the flat style appearance of the button control.
	// Remarks:
	//     The control will appear flat until the mouse pointer moves over it,
	//     at which point it appears three-dimensional.
	// Returns:
	//     TRUE if the flat style is used, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetFlatStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the flat style appearance of the button control.
	// Parameters:
	//     bFlatStyle - TRUE to use the flat style, FALSE otherwise.
	// Remarks:
	//     The control will appear flat until the mouse pointer moves over it,
	//     at which point it appears three-dimensional.
	//-----------------------------------------------------------------------
	void SetFlatStyle(BOOL bFlatStyle = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the control is drawn using visual styles\SkinFramework, if supported.
	// Remarks:
	//     This method enables Windows XP visual styles for the control. This
	//     Control will draw with visual styles if the control and the operating
	//     system supports it.
	//     Visual styles and SkinFramwork skins are specifications for the
	//     appearance of controls.  GetUseVisualStyle determines whether to
	//     use the currently set Windows XP visual style.  If using SkinFramwork,
	//     it determines whether to use the currently set style of the SkinFramework.
	// Returns:
	//     TRUE if the currently set Windows XP visual style will be used to
	//     theme the control.  If using SkinFramwork, the currently set style
	//     of the SkinFramework will be used to skin the control.
	//
	//     FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	BOOL GetUseVisualStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables Windows XP visual styles for the control.
	// Remarks:
	//     This method enables Windows XP visual styles for the control. This
	//     Control will draw with visual styles if the control and the operating
	//     system supports it.
	//     Visual styles and SkinFramwork skins are specifications for the
	//     appearance of controls.  GetUseVisualStyle determines whether to
	//     use the currently set Windows XP visual style.  If using SkinFramwork,
	//     it determines whether to use the currently set style of the SkinFramework.
	// Parameters:
	//     bUseVisualStyle - TRUE if the currently set Windows XP visual style will be used to
	//                       theme the control.  If using SkinFramwork, the currently set style
	//                       of the SkinFramework will be used to skin the control.
	//
	//                       FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	void SetUseVisualStyle(BOOL bUseVisualStyle = TRUE);

protected:

	// -----------------------------------------------------------------
	// Summary:
	//     This member is called to update color, text and other visual 
	//     elements of the control.
	// -----------------------------------------------------------------
	void RefreshMetrics();

	// -----------------------------------------------------------------
	// Summary:
	//     This member function is called by the progress bar to draw
	//     non client area borders.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc  - Size of the area to draw.
	// -----------------------------------------------------------------
	void DrawNcBorders(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     A helper for rendering the control appearance.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//-----------------------------------------------------------------------
	void DoPaint(CDC* pDC);

	// -----------------------------------------------------------------
	// Summary:
	//    This member function gets the border color for the progress bar.
	// Returns:
	//     COLORREF value used for the border.
	// -----------------------------------------------------------------
	COLORREF GetBorderColor();

	// -----------------------------------------------------------------
	// Summary:
	//    This member function gets the bar color for the progress bar.
	// Returns:
	//     COLORREF value used for the bar.
	// -----------------------------------------------------------------
	COLORREF GetBarColor();

	// -----------------------------------------------------------------
	// Summary:
	//    This member function gets the background color for the progress bar.
	// Returns:
	//     COLORREF value used for the background.
	// -----------------------------------------------------------------
	COLORREF GetBackColor();

	// -----------------------------------------------------------------
	// Summary:
	//    This member function gets the text color for the progress bar.
	// Returns:
	//     COLORREF value used for the text.
	// -----------------------------------------------------------------
	COLORREF GetTextColor();

//{{AFX_CODEJOCK_PRIVATE

	void Init();
	void DoStep(int nStep);

	//{{AFX_VIRTUAL(CXTPProgressCtrl)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPProgressCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	afx_msg LRESULT OnSetPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStepIt(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartMarquee(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTheme(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

//}}AFX_CODEJOCK_PRIVATE

protected:
	
	int  m_nMarqueePos;      // Position of Marquee progress bar.
	int  m_nMarqueeDelay;    // TRUE if delay is enabled.
	BOOL m_bPreSubclassInit; // TRUE if the progress bar has been subsclassed.
	BOOL m_bFlatStyle;       // TRUE if the progress bar is drawn flat (deprecated).
	CXTPProgressCtrlPaintManager* m_pPaintManager; // Pointer to the progress bar paint manager.
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPProgressCtrlPaintManager* CXTPProgressCtrl::GetPaintManager() const {
	return m_pPaintManager;
}
AFX_INLINE BOOL CXTPProgressCtrl::SetMarquee(BOOL fMarqueeMode, int nInterval) {
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, PBM_SETMARQUEE, (WPARAM)fMarqueeMode, (LPARAM)nInterval);
}
AFX_INLINE BOOL CXTPProgressCtrl::GetFlatStyle() const{
	return m_bFlatStyle;
}
AFX_INLINE void CXTPProgressCtrl::SetFlatStyle(BOOL bFlatStyle/* = TRUE*/) {
	m_bFlatStyle = bFlatStyle;
	if (m_hWnd) Invalidate(FALSE);
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPPROGRESSCTRL_H__)
