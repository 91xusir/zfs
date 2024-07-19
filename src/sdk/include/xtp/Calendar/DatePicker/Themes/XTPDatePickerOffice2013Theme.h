// XTPDatePickerOffice2013Theme.h: interface for the CXTPDatePickerOffice2013Theme class.
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
#if !defined(_XTPDATEPICKEROFFICE2013THEME_H__)
#define _XTPDATEPICKEROFFICE2013THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//---------------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerThemeOffice2013 is derived from CXTPDatePickerPaintManager
//     for rendering an Office 2013 style for DatePicker control.
// Example:
// <code>
//      if (m_bThemeOffice2013)
//      {
//          CXTPDatePickerThemeOffice2013* pTheme2013 = new CXTPDatePickerThemeOffice2013;
//          m_wndDatePicker.SetTheme(pTheme2013);
//      }
//      else
//      {
//          m_wndDatePicker.SetTheme(NULL);
//      }
// </code>
// See Also:
//     CXTPDatePickerThemeOffice2013
//---------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerThemeOffice2013 : public CXTPDatePickerPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPDatePickerThemeOffice2013();

	// -------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member item deallocation.
	// -------------------------------------
	virtual ~CXTPDatePickerThemeOffice2013();

	// -------------------------------------------------------------------
	// Summary:
	//     This member function performs initialization for the Paint
	//     Manager.
	// Remarks:
	//     Initializes all drawing defaults (fonts, colors, etc.). Most of
	//     defaults are system defaults.
	// -------------------------------------------------------------------
	virtual void RefreshMetrics();

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function is called to fill the control's background.
	// Parameters:
	//     pDC :   Pointer to a valid device context.
	// rcClient :  A CRect that contains the control's client area rectangle
	//                 coordinates.
	// ---------------------------------------------------------------------
	virtual void DrawBackground(CDC* pDC, CRect rcClient);

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw the week numbers in the month
	//     area.
	// Parameters:
	//     pDC      : Pointer to a valid device context.
	//     pMonth   : Pointer to month object.
	// Remarks:
	//     It is called from CXTPDatePickerItemMonth implementation.
	//     You can override it to customize drawing.
	// --------------------------------------------------------------------
	virtual void DrawWeekNumbers(CDC* pDC, CXTPDatePickerItemMonth* pMonth);

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw the days of the week in the
	//     month header.
	// Parameters:
	//     pDC      : Pointer to a valid device context.
	//     pMonth   : Pointer to month object.
	// Remarks:
	//     It is called from CXTPDatePickerItemMonth implementation.
	//     You can override it to customize drawing.
	// --------------------------------------------------------------------
	virtual void DrawDaysOfWeek(CDC* pDC, CXTPDatePickerItemMonth* pMonth);

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function draws a border around the control.
	// Parameters:
	//      pDC :       Pointer to a valid device context.
	//  pControl :  Pointer to a CXTPDatePickerControl object.
	//      rcClient :  A CRect that contains the control's client area rectangle
	//                 coordinates.
	//      bDraw    -  TRUE to draw else Adjust client rectangle.
	// ---------------------------------------------------------------------
	virtual void DrawBorder(CDC* pDC, const CXTPDatePickerControl* pControl, CRect& rcClient, BOOL bDraw);

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a single month day in the month
	//     area.
	// Parameters:
	//     pDC      : Pointer to a valid device context.
	//     pDay     : Pointer to day object.
	// Returns:
	//     TRUE if the drawing is successful, FALSE else.
	// Remarks:
	//     It is called from CXTPDatePickerItemDay implementation.
	//     You can override it to customize drawing.
	// --------------------------------------------------------------------
	virtual BOOL DrawDay(CDC* pDC, CXTPDatePickerItemDay* pDay);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the control's buttons.
	// Parameters:
	//     pDC :           Pointer to a valid device context.
	//     rcButton :      A CRect that contains the button area bounding
	//                     rectangle coordinates.
	//     strButton :     A CString that contains the string of the button's
	//                     text.
	//     bIsDown :       A BOOL that determines if the button is down or up.
	//     bIsHighLight :  A BOOL that determines if the button is highlighted.
	// Remarks:
	//     Use this member function to draw buttons in the appropriate state.
	//     The state depends on the bIsDown parameter, the button may be
	//     drawn pressed or released. Furthermore, depending on the
	//     bIsHighLight, the button may be drawn with highlighted borders.
	// Example:
	// <code>
	// for (int i = 0; i \< GetButtonCount(); i++)
	// {
	//     CXTPDatePickerButton* pButton = GetButton(i);
	//     if (pButton-\>m_bVisible)
	//     DrawButton(pDC, pButton-\>m_rcButton, pButton-\>m_strCaption,
	//     pButton-\>m_bPressed, pButton-\>m_bHighlight);
	// }
	// </code>
	// ----------------------------------------------------------------------
	virtual void DrawButton(CDC* pDC, const CRect& rcButton, const CString& strButton,
							BOOL bIsDown, BOOL bIsHighLight);

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the background color for the
	//     highlight today item.
	// Returns:
	//     A COLORREF object that contains the background color of the
	//     "Today" item.
	// ----------------------------------------------------------------------
	virtual COLORREF GetHighlightTodayBackColor();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the foreground (text) color for the
	//     highlight today item.
	// Returns:
	//     A COLORREF object that contains the foreground (text) color of the
	//     "Today" item.
	// ----------------------------------------------------------------------
	virtual COLORREF GetHighlightTodayTextColor();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the Border color for the
	//     highlight today item.
	// Returns:
	//     A COLORREF object that contains the Border color of the
	//     "Today" item.
	// ----------------------------------------------------------------------
	virtual COLORREF GetHighlightTodayBorderColor();

protected:

	COLORREF m_clrButtonBorder;
	COLORREF m_clrButtonPressed;
	COLORREF m_clrButtonPressedText;
	COLORREF m_clrButtonPressedBorder;
	COLORREF m_clrButtonHighlight;
	COLORREF m_clrButtonHighlightText;
	COLORREF m_clrButtonHighlightBorder;
	COLORREF m_clrHighlightTodayText;
	COLORREF m_clrHighlightTodayBorder;

	CXTPPaintManagerColorGradient m_clrBackground; // -------------------------------------------------------------
                                                   // This member variable represents two COLORREF objects that is 
	                                               // used as the dark and light background gradient color of the control.
                                                   // -------------------------------------------------------------
};

AFX_INLINE COLORREF CXTPDatePickerThemeOffice2013::GetHighlightTodayBackColor() {
	return m_clrHighlightToday;
}
AFX_INLINE COLORREF CXTPDatePickerThemeOffice2013::GetHighlightTodayTextColor() {
	return m_clrHighlightTodayText;
}
AFX_INLINE COLORREF CXTPDatePickerThemeOffice2013::GetHighlightTodayBorderColor() {
	return m_clrHighlightTodayBorder;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPDATEPICKEROFFICE2013THEME_H__)
