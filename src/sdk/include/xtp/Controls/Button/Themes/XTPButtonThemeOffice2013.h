// XTPButtonThemeOffice2013.h
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
#if !defined(__XTPBUTTONTHEMEOFFICE2013_H__)
#define __XTPBUTTONTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPButtonThemeOffice2013 is a class used to perform Office 2013 theme
//     drawing tasks.
//===========================================================================
class _XTP_EXT_CLASS CXTPButtonThemeOffice2013 : public CXTPButtonThemeUltraFlat
{
public:
	CXTPButtonThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the background for the
	//     button.
	// Parameters:
	//     pDC     - A CDC pointer that represents the current device
	//               context.
	//     pButton - Points to a CXTPButton object.
	// Returns:
	//     TRUE if the background was drawn successfully, otherwise returns
	//     FALSE.
	//-----------------------------------------------------------------------
	virtual void DrawButtonBackground(CDC* pDC, CXTPButton* pButton);

	virtual void DrawGroupBox(CDC* pDC, CXTPButton* pButton);

	virtual void DrawCheckBoxMark(CDC* pDC, CXTPButton* pButton);

	virtual void DrawRadioButtonMark(CDC* pDC, CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme manager to refresh
	//     the visual styles used by each components theme.
	// Parameters:
	//     pButton - Points to a CXTPButton object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPButton *pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the color used to draw
	//     the button text.
	// Parameters:
	//     pButton - Points to a CXTPButton object.
	// Returns:
	//     An RGB value that represents the button text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(CXTPButton* pButton);


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the text for the button.
	// Parameters:
	//     pDC     - A CDC pointer that represents the current device
	//               context.
	//     pButton - Points to a CXTPButton object.
	//-----------------------------------------------------------------------
	virtual void DrawPushButtonText(CDC* pDC, CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function draws the icon for the button, if one has
	//     been defined.
	// Parameters:
	//     pDC     - A CDC pointer that represents the current device
	//               context.
	//     pButton - Points to a CXTPButton object.
	//-----------------------------------------------------------------------
	virtual void DrawPushButtonIcon(CDC* pDC, CXTPButton* pButton);
	virtual void DrawPushButtonDropDown(CDC* pDC, CXTPButton* pButton);

protected:
	
	CXTPPaintManagerColor m_crBorder;
	CXTPPaintManagerColor m_crBorderDefault;
	CXTPPaintManagerColor m_crBorderDisabled;
	CXTPPaintManagerColor m_crBackDisabled;
	CXTPPaintManagerColor m_crMark;
	CXTPPaintManagerColor m_crMarkHilite;
	CXTPPaintManagerColor m_crMarkPushed;
	CXTPPaintManagerColor m_crMarkDisabled;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPBUTTONTHEMEOFFICE2013_H__)
