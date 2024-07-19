// XTPDatePickerOffice2003Theme.h: interface for the CXTPDatePickerOffice2003Theme class.
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
#if !defined(_XTPDATEPICKEROFFICE2003THEME_H__)
#define _XTPDATEPICKEROFFICE2003THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//---------------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerThemeOffice2003 is derived from CXTPDatePickerPaintManager
//     for rendering an Office 2003 style for DatePicker control.
// Example:
// <code>
//      if (m_bThemeOffice2003)
//      {
//          CXTPDatePickerThemeOffice2003* pTheme2003 = new CXTPDatePickerThemeOffice2003;
//          m_wndDatePicker.SetTheme(pTheme2003);
//      }
//      else
//      {
//          m_wndDatePicker.SetTheme(NULL);
//      }
// </code>
// See Also:
//     CXTPDatePickerThemeOffice2003
//---------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerThemeOffice2003 : public CXTPDatePickerPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPDatePickerThemeOffice2003();

	// -------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member item deallocation.
	// -------------------------------------
	virtual ~CXTPDatePickerThemeOffice2003();

public:
	// -------------------------------------------------------------------
	// Summary:
	//     This member function performs initialization for the Paint
	//     Manager.
	// Remarks:
	//     Initializes all drawing defaults (fonts, colors, etc.). Most of
	//     defaults are system defaults.
	// -------------------------------------------------------------------
	virtual void RefreshMetrics();

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
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPDATEPICKEROFFICE2003THEME_H__)
