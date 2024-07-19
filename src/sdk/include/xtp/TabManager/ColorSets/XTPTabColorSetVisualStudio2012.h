// XTPTabColorSetVisualStudio2012.h
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
#if !defined(__XTPTABCOLORSETVISUALSTUDIO2012_H__)
#define __XTPTABCOLORSETVISUALSTUDIO2012_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPTabColorSetVisualStudio2012 : public CXTPTabPaintManagerColorSet
{
public:
	CXTPTabColorSetVisualStudio2012();

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called to refresh the visual metrics of the tabs.
	// Remarks:
	//     All of the color members are refreshed when this is called.
	//     This member can be override this member to change the colors of
	//     the color members.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the color of a tab.
	// Parameters:
	//     pItem - Get color of this Tab.
	// Returns:
	//     COLORREF of pItem.
	// Remarks:
	//     This is used in many of the CXTPTabPaintManagerColorSet "fill" functions.
	// See Also: CXTPTabManager::GetItemColor, XTPTabColorStyle, SetColor, GetColorSet, SetColorSet
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemColor(CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to fill the tab navigation buttons.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     pButton - Tab navigation button to fill.
	//     rc      - Bounding rectangle of the tab navigation button.
	// Remarks:
	//     This member must be overridden and takes care of filling the
	//     tab navigation buttons that are in the header of the TabClient.
	//     The XTPTabColorStyle CXTPTabPaintManagerColorSet classes override this to perform
	//     actions such as painting the highlighting, pressed, and normal
	//     versions of the tab navigation buttons.
	//
	// See Also: CXTPTabManager::GetNavigateButton, CXTPTabManagerNavigateButton
	//-----------------------------------------------------------------------
	virtual void FillNavigateButton(CDC *pDC, CXTPTabManagerNavigateButton *pButton, CRect &rc);

protected:
	CXTPPaintManagerColor m_clrNonActiveButtonSelected;
	CXTPPaintManagerColor m_clrNonActiveButtonSelectedPressed;
	CXTPPaintManagerColor m_clrNonActiveButtonSelectedHighlighted;
	CXTPPaintManagerColor m_clrNonActiveTextSelected;
	CXTPPaintManagerColor m_clrNonActiveTextSelectedPressed;
	CXTPPaintManagerColor m_clrNonActiveTextSelectedHighlighted;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABCOLORSETVISUALSTUDIO2012_H__)
