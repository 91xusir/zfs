// XTPCalendarCaptionBarControl.h
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
#if !defined(__XTPCalendarCaptionBarControl_H__)
#define __XTPCalendarCaptionBarControl_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPNotifySink;
class CXTPWinThemeWrapper;
class CXTPCalendarCaptionBarTheme;

//////////////////////////////////////////////////////////////////////////
// Office2007

/*
	Switch View Bar:
		SwitchViewButtonPart
		RadioButtonPart
		// LabelPart

	Scroll Date Bar:
		ScrollDateButtonPart
		DateLabelPart

	==========================================
	ButtonViewDay   : SwitchViewButtonPart
	ButtonViewWeek  : SwitchViewButtonPart
	ButtonViewMonth : SwitchViewButtonPart

	ShowWorkWeek    : RadioButtonPart
	ShowFullWeek    : RadioButtonPart
	---------------------------------
	ScrollPrevDate  : ScrollDateButtonPart
	ScrollNextDate  : ScrollDateButtonPart
	DateLabel       : DateLabelPart

	---------------------------------
	SwitchViewBarBkColor
	ScrollDateBarBkColor

*/

//===========================================================================
// Summary:
//     Enumerates Calendar Caption Bar graphical objects .
//===========================================================================
enum XTPEnumCalendarCaptionBarThemeObject
{
	xtpCalendarCaptionBar_Unknown           = 0,            // Defines Unknown (NULL) value.
	xtpCalendarCaptionBar_ButtonViewDay     = 0x00000001,   // Defines SwitchViewBar DayViewButton.
	xtpCalendarCaptionBar_ButtonViewWeek    = 0x00000002,   // Defines SwitchViewBar WeekViewButton.
	xtpCalendarCaptionBar_ButtonViewMonth   = 0x00000004,   // Defines SwitchViewBar MonthViewButton.
	xtpCalendarCaptionBar_ShowWorkWeek      = 0x00000010,   // Defines SwitchViewBar ShowWorkWeek radio button.
	xtpCalendarCaptionBar_ShowFullWeek      = 0x00000020,   // Defines SwitchViewBar ShowFullWeek radio button.
	xtpCalendarCaptionBar_ShowTimeline      = 0x00000040,   // Defines SwitchViewBar ShowTimeline radio button.
	xtpCalendarCaptionBar_MultiColumnsWeek  = 0x00000080,   // Defines SwitchViewBar MultiColumnsWeek radio button.
	xtpCalendarCaptionBar_ScrollPrevDate    = 0x00000100,   // Defines ScrollDateBar PrevDateButton.
	xtpCalendarCaptionBar_ScrollNextDate    = 0x00000200,   // Defines ScrollDateBar NextDateButton.
	xtpCalendarCaptionBar_DateLabel         = 0x00000400,   // Defines ScrollDateBar DateLabel.
	xtpCalendarCaptionBar_ButtonExpand      = 0x00000800    // Defines ScrollDateBar Expand Buton.
};

//===========================================================================
// Summary:
//     CXTPCalendarCaptionBarControl represents CalendarCaptionBar control and
//     used as a part of CXTPCalendarControl object. They looks and works as
//     a single control but separated on 2 objects for customization
//     possibilities.
// Remarks:
//     This class is used as owner (host, events source) window for
//     CXTPCalendarCaptionBarTheme object which implements the most control logic.
//
// See Also: CXTPCalendarCaptionBarTheme, CXTPCalendarControl
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarControl : public CWnd
{
	using CWnd::Create;

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarCaptionBarControl)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also:
	//     ~CXTPCalendarCaptionBarControl
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also:
	//     CXTPCalendarCaptionBarControl
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the minimum size required to show a
	//     CaptionBar control.
	// Parameters:
	//     bExactSize - BOOL flag for calc mode
	// Remarks:
	//     This member function call CXTPCalendarCaptionBarTheme::CalcMinRect
	//     function.
	// Returns:
	//     A minimum rectangle required to show CaptionBar control.
	// See Also:
	//     CXTPCalendarCaptionBarTheme::CalcMinRect
	// --------------------------------------------------------------------------
	virtual CRect CalcMinRect(BOOL bExactSize = FALSE);

	// -----------------------------------------------------------------------
	// Summary:
	//     This member function is used to create the CaptionBar control window.
	// Parameters:
	//     dwStyle :     Specifies the window style attributes.
	//     rect :        A RECT object that contains the coordinates of the
	//                   window, in the client coordinates of pParentWnd.
	//     pParentWnd :  A CWnd pointer to the parent window.
	//     nID :         A UINT that contains the ID of the child window.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// -----------------------------------------------------------------------
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the object's layout
	//     depending on the provided bounding rectangle.
	// Parameters:
	//     pDC :     Pointer to a valid device context (may be NULL).
	//     prcWnd :  A CRect object containing the dimensions of the bounding
	//               rectangle for this part.
	//
	// ----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC = NULL, const CRect* prcWnd = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain pointer to
	//     CXTPCalendarCaptionBarTheme object.
	// Returns:
	//     A pointer to CXTPCalendarCaptionBarTheme object.
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarTheme* GetTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set theme object.
	// Parameters:
	//     pTheme - A pointer to CXTPCalendarCaptionBarTheme object.
	//-----------------------------------------------------------------------
	void SetTheme(CXTPCalendarCaptionBarTheme* pTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Register the window class if it has not already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     TRUE if the window class was successfully registered.  FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the object's layout depending
	//     on the provided bounding rectangle.
	// Returns:
	//     A CRect object containing the dimensions of the bounding rectangle
	//-----------------------------------------------------------------------
	CRect GetBorders() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the object's layout depending
	//     on the provided bounding rectangle.
	// Parameters:
	//  nLeft - rectange data
	//  nTop - rectange data
	//  nRight - rectange data
	//  nBottom - rectange data
	//-----------------------------------------------------------------------
	virtual void SetBorders(int nLeft, int nTop, int nRight, int nBottom);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the object's layout depending
	//     on the provided bounding rectangle.
	//-----------------------------------------------------------------------
	virtual void UpdateBorders();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to show or hide Timeline button.
	// Parameters:
	//     bEnable - BOOL flag to pass
	// see also m_bTimelineEnable
	//-----------------------------------------------------------------------
	void EnableTimeline(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get state of m_bTimelineEnable
	// Returns:
	//     TRUE if the time line is enabled, FALSE else.
	//-----------------------------------------------------------------------
	BOOL IsTimelineEnable();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to show or hide Multi-Columns mode button.
	// Parameters:
	//     bEnable - BOOL flag to pass
	// see also m_bMulticolumnsEnable
	//-----------------------------------------------------------------------
	void EnableMulticolumns(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get state of m_bMulticolumnsEnable
	// Returns:
	//     TRUE if the this button enable.
	//-----------------------------------------------------------------------
	BOOL IsMulticolumnsEnable();

	//-----------------------------------------------------------------------
	// Summary:
	//  This member function is used to get state of GetTheme()->m_bIsOneLine -
	// Parameters:
	//  bSet - BOOL flag to toggle CaptionMode 2-lines or 1-line modes
	//-----------------------------------------------------------------------
	void UseOneLine(BOOL bSet);

	//-----------------------------------------------------------------------
	// Summary:
	//  This member function is used to get state of GetTheme()->m_bIsOneLine -
	//  flag to toggle CaptionMode 2-lines or 1-line modes
	//
	// Returns:
	//     TRUE if the this One Line mode used
	//-----------------------------------------------------------------------
	BOOL IsUseOneLine(); // get state of m_bIsOneLine

	//-----------------------------------------------------------------------
	// Summary:
	//		This member function is used to Set Button Text if text not empty
	//		and hide button and adjust Caption Bar if text is empty
	// Parameters:
	//		iButtonNumber - ordinal number of button starting from 0
	//		sButtonText - CString of text passed
	// Remarks:
	//		Smart function which allow to set custom text on Caption Bar buttons and
	//		hide - if text EMPTY at least MonthView, WeekView and DayView buttons to prevent user switch mode
	//		(for restricted app)
	// Example:
	//		Example of using with button numbers linked to specific View type:
	//		GetCalendarCaptionBarCtrl().SetButtonText(0, _T("D"));   /* DayView */
	//		GetCalendarCaptionBarCtrl().SetButtonText(1, _T("W"));   /* WeekView */
	//		GetCalendarCaptionBarCtrl().SetButtonText(2, _T("M"));   /* MonthView */
	//		GetCalendarCaptionBarCtrl().SetButtonText(3, _T("TL"));  /* TimeLine */
	//		GetCalendarCaptionBarCtrl().SetButtonText(4, _T("WW"));  /* WorkWeek */
	//		GetCalendarCaptionBarCtrl().SetButtonText(5, _T("FW"));  /* FullWeek */
	//		GetCalendarCaptionBarCtrl().SetButtonText(6, _T("III")); /* Multi-Columns mode */
	//-----------------------------------------------------------------------
	void SetButtonText(int iButtonNumber, CString sButtonText);

protected:
	CXTPCalendarCaptionBarTheme* m_pTheme; // Store pointer to CXTPCalendarCaptionBarTheme object.
	CRect m_Borders;              // Store options to draw border lines;

	//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR uTimerID);
	afx_msg void OnDestroy();
//  afx_msg void OnEnable(BOOL bEnable);
//  afx_msg UINT OnGetDlgCode();
//  afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus (CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CRect CXTPCalendarCaptionBarControl::GetBorders() const {
	return m_Borders;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCalendarCaptionBarControl_H__)
