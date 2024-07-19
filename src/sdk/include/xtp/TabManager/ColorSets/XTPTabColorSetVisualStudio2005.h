// XTPTabColorSetVisualStudio2005.h
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
#if !defined(__XTPTABCOLORSETVISUALSTUDIO2005_H__)
#define __XTPTABCOLORSETVISUALSTUDIO2005_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-------------------------------------------------------------------------
// Summary:
//     CColorSetVisualStudio2005 is a CXTPTabColorSetOffice2003 derived class that represents the
//     Visual Studio 2005 tab color set.
// Remarks:
//     To use the Visual Studio 2005 color set, SetColor is used to apply
//     the xtpTabColorVisualStudio2005 XTPTabColorStyle.
//
//     The following appearances (XTPTabAppearanceStyle) support CColorSetVisualStudio2005:
//     * <b>xtpTabAppearancePropertyPage2003</b> Office 2003 appearance.
//     * <b>xtpTabAppearanceStateButtons</b>     State Button appearance.
//
// See Also: XTPTabColorStyle, XTPTabAppearanceStyle, SetAppearance, GetAppearance, GetAppearanceSet,
//           SetColor, GetColor, GetColorSet, SetColorSet, SetAppearanceSet
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CColorSetVisualStudio2005 : public CXTPTabColorSetOffice2003
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CColorSetVisualStudio2005 object.
	//-----------------------------------------------------------------------
	CColorSetVisualStudio2005();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to refresh the visual metrics of the tabs.
	// Remarks:
	//     All of the color members are refreshed when this is called.
	//     This member can be override this member to change the colors of
	//     the color members.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to fill a tab button.
	// Parameters:
	//     pDC    - Pointer to a valid device context.
	//     rcItem - Bounding rectangle of the tab button to fill.
	//     pItem  - Tab button to draw.
	// Returns:
	//     COLORREF returned from CXTPTabManager::GetItemColor(pItem).
	// Remarks:
	//     This member is used to fill all tab buttons except when the XTPTabAppearanceStyle is
	//     xtpTabAppearanceStateButtons.  xtpTabAppearanceStateButtons require
	//     the FillStateButton member.
	//
	//     If m_bGradientButton is TRUE, then CXTPTabColorSetOffice2003::FillPropertyButton
	//     is used to fill the tab button with a gradient fill. See
	//     CXTPTabColorSetOffice2003::FillPropertyButton for details.
	//
	//     If m_bGradientButton is FALSE then CXTPTabPaintManagerColorSet::FillPropertyButton is used.
	//
	// See Also: m_bGradientButton
	//-----------------------------------------------------------------------
	COLORREF FillPropertyButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);

protected:
	BOOL m_bGradientButton;         // TRUE if luna colors are enabled.  This will be TRUE when using Windows XP and the current system theme is xtpSystemThemeSilver, xtpSystemThemeOlive, or xtpSystemThemeOlive, FALSE otherwise.
};

//{{AFX_CODEJOCK_PRIVATE
typedef CColorSetVisualStudio2005 CColorSetWhidbey; // obsolete, for backward compatibility only.
//}}AFX_CODEJOCK_PRIVATE



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABCOLORSETVISUALSTUDIO2005_H__)
