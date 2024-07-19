// XTPTabThemeExcel2003.h
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
#if !defined(__XTPTABTHEMEEXCEL2003_H__)
#define __XTPTABTHEMEEXCEL2003_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-------------------------------------------------------------------------
// Summary:
//     CXTPTabThemeExcel2003 is a CXTPTabThemeFlat derived
//     class that represents the Excel tab appearance.
// Remarks:
//     To use the Excel appearance, SetAppearance is used to apply
//     the xtpTabAppearanceExcel2003 XTPTabAppearanceStyle.
//
//     CXTPTabThemeExcel2003 supports the following XTPTabColorStyle:
//     * <b>xtpTabColorDefault</b>      Default color style.
//     * <b>xtpTabColorVisualStudio2003</b> Visual Studio color style.
//     * <b>xtpTabColorOffice2003</b>   Office 2003 color style.
//
// See Also: XTPTabColorStyle, XTPTabAppearanceStyle, SetAppearance, GetAppearance, GetAppearanceSet,
//           SetColor, GetColor, GetColorSet, SetColorSet, SetAppearanceSet
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPTabThemeExcel2003 : public CXTPTabThemeFlat
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTabThemeExcel2003 object.
	//-------------------------------------------------------------------------
	CXTPTabThemeExcel2003()
	{
		m_rcHeaderMargin.SetRect(2, 3, 2, 0);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get the top, left, right, and bottom
	//     tab header margins.
	// Returns:
	//     CRect struct containing the left, top, right, and bottom margins
	//     of the tab header.  This is the margin (space) placed around the
	//     tab buttons inside the tab header.
	// Remarks:
	//     The CRect structure will contain the margins of the tab header.
	//     CXTPTabPaintManagerTheme::m_rcHeaderMargin contains
	//     these margins.
	//
	// See Also: CXTPTabPaintManagerTheme::m_rcHeaderMargin, CXTPTabPaintManager::GetHeaderRect
	//-----------------------------------------------------------------------
	virtual CRect GetHeaderMargin()
	{
		return CRect(m_rcHeaderMargin.left + m_nButtonHeight / 2, m_rcHeaderMargin.top, m_rcHeaderMargin.right + m_nButtonHeight / 2, m_rcHeaderMargin.bottom);
	}

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
	virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to get bounding rectangle of item that was drawn
	// Parameters:
	//     pItem - Item to test
	// Returns: Bounding rectangle of drawing item
	// See Also: CXTPTabManagerItem::GetRect
	//-----------------------------------------------------------------------
	virtual CRect GetButtonDrawRect(const CXTPTabManagerItem* pItem);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABTHEMEEXCEL2003_H__)
