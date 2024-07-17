// XTPCaptionButtonThemeOffice2003.h
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
#if !defined(__XTPCAPTIONBUTTONTHEMEOFFICE2003_H__)
#define __XTPCAPTIONBUTTONTHEMEOFFICE2003_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Class CXTPCaptionButtonThemeOffice2003 is a CXTPButtonOffice2003Theme
// derived class.  This class is used to create a button caption that resembles
// an Office 2003 button caption.
//===========================================================================
class _XTP_EXT_CLASS CXTPCaptionButtonThemeOffice2003 : public CXTPButtonThemeOffice2003
{
protected:
	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the caption.
	// Parameters:
	//     lpDIS   - An LPDRAWITEMSTRUCT struct that contains the information the
	//               owner window must have to determine how to paint an owner-drawn
	//               control.
	//     pButton - A pointer to valid CXTPButton object.
	// Returns:
	//     A TRUE if the background was successfully drawn.  FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual void DrawButtonBackground(CDC* pDC, CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the text color of the button caption.
	// Parameters:
	//      pButton - A pointer to a valid CXTPButton object.
	// Returns:
	//      An RGB value specifying the text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(CXTPButton* pButton);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCAPTIONBUTTONTHEMEOFFICE2003_H__)
