// XTPTabThemeVisio.h
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
#if !defined(__XTPTABTHEMEVISIO_H__)
#define __XTPTABTHEMEVISIO_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-------------------------------------------------------------------------
// Summary:
//     CAppearanceSetVisio is a CXTPTabPaintManagerTheme derived
//     class that represents the Visio tab appearance.
// Remarks:
//     To use the Visio appearance, SetAppearance is used to apply
//     the xtpTabAppearanceVisio XTPTabAppearanceStyle.
//
//     CAppearanceSetVisio supports the following XTPTabColorStyle:
//     * <b>xtpTabColorDefault</b>     Default color style.
//     * <b>xtpTabColorOffice2003</b>  Office 2003 color style.
//
// See Also: XTPTabColorStyle, XTPTabAppearanceStyle, SetAppearance, GetAppearance, GetAppearanceSet,
//           SetColor, GetColor, GetColorSet, SetColorSet, SetAppearanceSet
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CAppearanceSetVisio : public CXTPTabPaintManagerTheme
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to adjust the tab client rectangle.  It will
	//     return the new tab client rectangle with margins included.
	// Parameters:
	//     pTabManager - Pointer to the tab manager to adjust the tab client rectangle.
	//     rcClient    - [out] CRect to store tab client rectangle.
	// Remarks:
	//     CXTPTabPaintManager::AdjustClientRect and CXTPTabPaintManager::RepositionTabControl
	//     are called when the window is resized.  When CXTPTabPaintManager::DrawTabControl
	//     is called the changes will be used when drawing the tab control.
	//
	//     This member will adjust the tab client rectangle based on the
	//     currently set XTPTabClientFrame.
	//
	//     Then CXTPTabPaintManager::AdjustClientRect calls this member
	//     if there is one or more tab in the tab manager.
	// See Also:
	//     CXTPTabPaintManager::AdjustClientRect
	//-----------------------------------------------------------------------
	void AdjustClientRect(CXTPTabManager* pTabManager, CRect& rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to move the tab buttons of the tab control to their
	//     correct positions.
	// Parameters:
	//     pTabManager - Pointer to the tab manager the tab button are on.
	//     pDC         - Pointer to a valid device context.
	//     rcClient    - Bounding rectangle of the tab client area.
	// Remarks:
	//     CXTPTabPaintManager::RepositionTabControl and CXTPTabPaintManager::AdjustClientRect
	//     are called when the window is resized.  When DrawTabControl is called
	//     the changes will be used when drawing the tab control.
	//
	//     This member is called in CXTPTabPaintManager::RepositionTabControl.
	//     This member calls CXTPTabPaintManager::RepositionTabControlEx.
	//-----------------------------------------------------------------------
	void RepositionTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the bounding rectangle to the tab
	//     header area.
	// Parameters:
	//     rcControl   - Bounding rectangle of the tab header.
	//                   See CXTPTabManager::GetControlRect.
	//     pTabManager - Pointer to a CXTPTabManager.
	// Returns:
	//     Bounding rectangle of tab header area.
	// See Also: GetClientRect, CXTPTabManager::GetControlRect, CXTPTabManager::CXTPTabPaintManagerTheme::GetHeaderMargin, CXTPTabManager::CXTPTabPaintManagerTheme::GetClientMargin
	//-----------------------------------------------------------------------
	CRect GetHeaderRect(CRect rcControl, CXTPTabManager* pTabManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the bounding rectangle to the tab
	//     client area.
	// Parameters:
	//     rcControl   - Bounding rectangle of the tab control.
	//     pTabManager - Pointer to a CXTPTabManager.
	// Returns:
	//     Bounding rectangle of tab client area.
	// See Also: GetClientRect, CXTPTabManager::GetControlRect
	//-----------------------------------------------------------------------
	CRect GetClientRect(CRect rcControl, CXTPTabManager* pTabManager);

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
	// See Also:: CXTPTabPaintManager::DrawTabControlEx, CXTPTabPaintManager::DrawSingleButtonIconAndText
	//-----------------------------------------------------------------------
	void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to draw the tab control.
	// Parameters:
	//     pTabManager - Pointer to the tab manager to draw.
	//     pDC         - Pointer to a valid device context.
	//     rcControl   - Bounding rectangle of the client area.
	// Remarks:
	//     This member is called in various Draw and Paint functions.
	//
	//     This member will be called by CXTPTabPaintManager::DrawTabControl
	//     when the tab control needs to be drawn.
	//-----------------------------------------------------------------------
	void DrawTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABTHEMEVISIO_H__)
