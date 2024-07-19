// XTPButtonThemeFlat.h
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
#if !defined(__XTPBUTTONTHEMEFLAT_H__)
#define __XTPBUTTONTHEMEFLAT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPButtonThemeFlat is a class used to perform Flat Theme
//     drawing tasks.
//===========================================================================
class _XTP_EXT_CLASS CXTPButtonThemeFlat : public CXTPButtonTheme
{
public:

	// ----------------------------------------
	// Summary:
	//     Constructs a CXTPButtonThemeFlat object
	// ----------------------------------------
	CXTPButtonThemeFlat();

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
	void DrawButtonBackground(CDC* pDC, CXTPButton* pButton);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPBUTTONTHEMEFLAT_H__)
