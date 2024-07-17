// XTPDatePickerResourceTheme.h: interface for the CXTPDatePickerResourceTheme class.
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
#if !defined(_XTPDATEPICKERRESOURCETHEME_H__)
#define _XTPDATEPICKERRESOURCETHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//---------------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerThemeResource is derived from CXTPDatePickerPaintManager
//     for rendering a Resource DLL style for DatePicker control.
// Example:
// <code>
//      if (m_bThemeResource)
//      {
//          CXTPDatePickerThemeResource* pThemeResource = new CXTPDatePickerThemeResource;
//          m_wndDatePicker.SetTheme(pThemeResource);
//      }
//      else
//      {
//          m_wndDatePicker.SetTheme(NULL);
//      }
// </code>
// See Also:
//     CXTPDatePickerThemeResource
//---------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerThemeResource : public CXTPDatePickerPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPDatePickerThemeResource();

	// -------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member item deallocation.
	// -------------------------------------
	virtual ~CXTPDatePickerThemeResource();

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
	virtual void DrawButton(CDC* pDC, const CRect& rcButton, const CString& strButton, BOOL bIsDown, BOOL bIsHighLight);

protected:
	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the COLORREF value for the specified
	//     color name located in the Resource DLL INI file.
	// Parameters:
	//     pcszColorName : A text string representing the color name specified in the Resource DLL INI file.
	//     clrDefault    : Default RGB value to be returned if the color was not found in the Resource DLL.
	// Returns:
	//     An RGB COLORREF value representing the color name if located in the Resource
	//     DLL, otherwise returns the specified default color.
	// ----------------------------------------------------------------------
	COLORREF GetResourceColor(LPCTSTR pcszColorName, COLORREF clrDefault);
};

//{{AFX_CODEJOCK_PRIVATE
#define CXTPDatePickerThemeOffice2007 CXTPDatePickerThemeResource
//}}AFX_CODEJOCK_PRIVATE


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPDATEPICKERRESOURCETHEME_H__)
