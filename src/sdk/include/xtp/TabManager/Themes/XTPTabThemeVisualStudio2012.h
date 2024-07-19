// XTPTabThemeVisualStudio2012.h
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
#if !defined(__XTPTABTHEMEVISUALSTUDIO2012_H__)
#define __XTPTABTHEMEVISUALSTUDIO2012_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPTabThemeVisualStudio2012 : public CXTPTabThemeFlat
{
public:

	CXTPTabThemeVisualStudio2012();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to refresh the appearance metrics.
	// Remarks:
	//     This member is used to refresh the appearance metrics such
	//     as font, button height, margin size, etc.  Override this
	//     member in derived classes to change the appearance metrics.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

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
	virtual void DrawSingleButton(CDC *pDC, CXTPTabManagerItem *pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to draw the different parts that make up
	//     the TabMDIClient TabWorkSpace.
	// Parameters:
	//     pDC    - Pointer to a valid device context.
	//     lpRect - Size of splitter.
	//     workspacePart  - XTPTabWorkspacePart to draw.
	// Remarks:
	//     This is used when drawing the TabWorkspace of MDI tabs when
	//     tab grouping is enabled.
	//
	//     lpRect is the size of the splitter that is placed between the
	//     vertical or horizontal tab groups if workspacePart is
	//     XTPTabWorkspacePartHSplitter or XTPTabWorkspacePartVSplitter.  And
	//     the splitter will be drawn with lpRect.
	//
	//     If workspacePart is XTPTabWorkspacePartWidth, then this is the size
	//     of the splitter.
	// See Also: XTPTabWorkspacePart
	//-----------------------------------------------------------------------
	virtual void DrawWorkspacePart(CDC* pDC, LPRECT lpRect, XTPTabWorkspacePart workspacePart);

	virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorVisualStudio2012; }
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABTHEMEVISUALSTUDIO2012_H__)
