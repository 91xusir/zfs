// XTPDatePickerOffice2000Theme.h: interface for the CXTPDatePickerOffice2000Theme class.
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
#if !defined(_XTPDATEPICKEROFFICE2000THEME_H__)
#define _XTPDATEPICKEROFFICE2000THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//---------------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerThemeOffice2000 is derived from CXTPDatePickerPaintManager
//     for rendering an Office 2000 style for DatePicker control.
// Example:
// <code>
//      if (m_bThemeOffice2000)
//      {
//          CXTPDatePickerThemeOffice2000* pTheme2000 = new CXTPDatePickerThemeOffice2000;
//          m_wndDatePicker.SetTheme(pTheme2000);
//      }
//      else
//      {
//          m_wndDatePicker.SetTheme(NULL);
//      }
// </code>
// See Also:
//     CXTPDatePickerThemeOffice2000
//---------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerThemeOffice2000 : public CXTPDatePickerPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPDatePickerThemeOffice2000();

	// -------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member item deallocation.
	// -------------------------------------
	virtual ~CXTPDatePickerThemeOffice2000();

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw the month header in the month
	//     area.
	// Parameters:
	//     pDC      : Pointer to a valid device context.
	//     pMonth   : Pointer to month object.
	// Remarks:
	//     It is called from CXTPDatePickerItemMonth implementation.
	//     You can override it to customize drawing.
	// --------------------------------------------------------------------
	virtual void DrawMonthHeader(CDC* pDC, CXTPDatePickerItemMonth* pMonth);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPDATEPICKEROFFICE2000THEME_H__)
