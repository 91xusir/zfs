// XTPTabThemePropertyPageSelected.h
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
#if !defined(__XTPTABTHEMEPROPERTYPAGESELECTED_H__)
#define __XTPTABTHEMEPROPERTYPAGESELECTED_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-------------------------------------------------------------------------
// Summary:
//     CAppearanceSetPropertyPageSelected is a CXTPTabThemeOffice2000 derived
//     class that represents the "Office 2000 Selected" tab appearance.
// Remarks:
//     To use the Office 2000 Selected appearance, SetAppearance is used to apply
//     the xtpTabAppearancePropertyPageSelected XTPTabAppearanceStyle.
//
//     CAppearanceSetPropertyPageSelected supports the following XTPTabColorStyle:
//     * <b>xtpTabColorDefault</b>    Default color style.
//     * <b>xtpTabColorOffice2003</b> Office 2003 color style.
//     * <b>xtpTabColorWinNative</b>      Windows XP color style.
//
// See Also: XTPTabColorStyle, XTPTabAppearanceStyle, SetAppearance, GetAppearance, GetAppearanceSet,
//           SetColor, GetColor, GetColorSet, SetColorSet, SetAppearanceSet
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CAppearanceSetPropertyPageSelected : public CXTPTabThemeOffice2000
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a single tab button in the tab
	//     client header area.
	// Parameters:
	//     pDC   - Pointer to a valid device context.
	//     pItem - Tab button to draw.
	// Remarks:
	//     This method will draw a single tab button.  This method only
	//     draws the button, no text or icon is added.
	//
	//     DrawSingleButton calls CXTPTabPaintManager::DrawSingleButtonIconAndText
	//     after the button is drawn.  This is called to draw the button's
	//     icon and text.
	//
	//     If the tab button is selected or highlighted then
	//     CXTPTabPaintManager::CAppearanceSetPropertyPageSelected::DrawSingleButton
	//     is called instead of CXTPTabPaintManager::DrawSingleButtonIconAndText.
	//
	// See Also:: CXTPTabPaintManager::DrawTabControlEx, CXTPTabPaintManager::DrawSingleButtonIconAndText
	//-----------------------------------------------------------------------
	virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABTHEMEPROPERTYPAGESELECTED_H__)
