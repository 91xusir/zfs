// XTPCaptionButtonThemeOfficeXP.h
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
#if !defined(__XTPCAPTIONBUTTONTHEMEOFFICEXP_H__)
#define __XTPCAPTIONBUTTONTHEMEOFFICEXP_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Class CXTPCaptionButtonThemeOfficeXP is a CXTPButtonOfficeXPTheme
//          derived class.  This class is used to create a button caption that
//          resembles an Office XP button caption.
//===========================================================================
class _XTP_EXT_CLASS CXTPCaptionButtonThemeOfficeXP : public CXTPButtonThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default class constructor.  Call this member function to
	//          construct a CXTPCaptionButtonThemeOfficeXP object.
	//-----------------------------------------------------------------------
	CXTPCaptionButtonThemeOfficeXP();

protected:
	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the button caption text.
	// Parameters:
	//       pDC     - A pointer to a valid device context.
	//       pButton - A pointer to a valid CXTPButton object.
	//-----------------------------------------------------------------------
	virtual void DrawButtonText(CDC* pDC, CXTPButton* pButton);

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
#endif // !defined(__XTPCAPTIONBUTTONTHEMEOFFICEXP_H__)
