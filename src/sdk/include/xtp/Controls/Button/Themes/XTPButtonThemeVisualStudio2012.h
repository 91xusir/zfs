// XTPButtonThemeVisualStudio2012.h
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
#if !defined(__XTPBUTTONTHEMEVISUALSTUDIO2012_H__)
#define __XTPBUTTONTHEMEVISUALSTUDIO2012_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: CXTPButtonThemeVisualStudio2012 is a class used to perform Visual 
//          Studio 2012 theme drawing tasks.
//===========================================================================
class _XTP_EXT_CLASS CXTPButtonThemeVisualStudio2012 : public CXTPButtonThemeOffice2013
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPButtonThemeVisualStudio2012 object
	//-----------------------------------------------------------------------
	CXTPButtonThemeVisualStudio2012(BOOL bLight);

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to refresh
	//          the visual styles used by each components theme.
	// Input:   pButton - Points to a CXTPButton object.
	//-----------------------------------------------------------------------
	void RefreshMetrics(CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the color used to draw
	//          the button text.
	// Input:   pButton - Points to a CXTPButton object.
	// Returns: An RGB value that represents the button text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: This member funciton is called to draw the check box mark for
	//          the button control.
	// Input:   pDC     - Points to a valid device context.
	//          pButton - Points to the CXTPButton to draw the check box for.
	//-----------------------------------------------------------------------
	virtual void DrawCheckBoxMark(CDC* pDC, CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: This member funciton is called to draw the radio mark for
	//          the button control.
	// Input:   pDC     - Points to a valid device context.
	//          pButton - Points to the CXTPButton to draw the radio mark for.
	//-----------------------------------------------------------------------
	virtual void DrawRadioButtonMark(CDC* pDC, CXTPButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: This member funciton is called to draw the background for
	//          the button control.
	// Input:   pDC     - Points to a valid device context.
	//          pButton - Points to the CXTPButton to draw the background for.
	//-----------------------------------------------------------------------
	virtual void DrawButtonBackground(CDC* pDC, CXTPButton* pButton);

protected:

	BOOL m_bLight; // TRUE when drawing light theme.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPBUTTONTHEMEVISUALSTUDIO2012_H__)
