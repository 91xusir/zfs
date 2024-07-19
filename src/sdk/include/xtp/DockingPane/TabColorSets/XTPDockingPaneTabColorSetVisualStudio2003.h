// XTPDockingPaneTabColorSetVisualStudio2003.h
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
#if !defined(__XTPDOCKINGPANETABCOLORSETVISUALSTUDIO2003_H__)
#define __XTPDOCKINGPANETABCOLORSETVISUALSTUDIO2003_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Helper class used to initialize default colors for a theme from 
//          an INI resource and handle drawing routines.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneTabColorSetVisualStudio2003 : public CXTPTabColorSetOffice2003
{
public:
    //-----------------------------------------------------------------------
    // Summary:    Default class constructor.
	// Parameters: bHighlightSelected : TRUE if the tab is selected.
    //-----------------------------------------------------------------------
	CXTPDockingPaneTabColorSetVisualStudio2003(BOOL bHighlightSelected);

    //-----------------------------------------------------------------------
    // Summary: This member function is used to initialize default colors 
	//          for a theme from an INI resource.
    //-----------------------------------------------------------------------
	void RefreshMetrics();

public:
    //-----------------------------------------------------------------------
    // Summary:    This member is used to paint the specified frame portion
	//             for a dockign pane tab.
    // Parameters: pDC   : Pointer to a valid device context.
	//             rc    : Size of the area to draw.
	//             pWnd  : Pointer to a valid CWnd object.
	//             pSite : Pointer to a valid CWnd object.
    //-----------------------------------------------------------------------
	void FillFramePartRect(CDC* pDC, CRect rc, CWnd* pWnd, CWnd* pSite);

    //-----------------------------------------------------------------------
    // Summary:    This member is used to paint the specified header portion
	//             for a dockign pane tab.
    // Parameters: pDC         : Pointer to a valid device context.
	//             rc          : Size of the area to draw.
	//             pTabManager : Pointer to a valid CXTPTabManager object.
    //-----------------------------------------------------------------------
	void FillHeader(CDC* pDC, CRect rc, CXTPTabManager* pTabManager);

	//-----------------------------------------------------------------------
	// Summary:    This member is called to fill a tab button.
	// Parameters: pDC    : Pointer to a valid device context.
	//             rcItem : Bounding rectangle of the tab button to fill.
	//             pItem  : Tab button to draw.
	// Returns:    COLORREF returned from CXTPTabManager::GetItemColor(pItem).
	// Remarks:    This member is used to fill all tab buttons except when the 
	//             XTPTabAppearanceStyle is xtpTabAppearanceStateButtons.  
	//             xtpTabAppearanceStateButtons require the FillStateButton member.
	//
	//             Tab buttons will use CXTPTabPaintManagerColorSet::GradientFill to
	//             fill the buttons.  The gradient will use the color of the tab item
	//             blended with RGB(255, 255, 255) to produce a "light color", the gradient will
	//             start with the "light color" and fill to the normal color of the tab item.
	// See Also:   FillStateButton
	//-----------------------------------------------------------------------
	virtual COLORREF FillPropertyButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);

protected:
	BOOL m_bGradientButton;    // 
	BOOL m_bTransparentButton; // 
	BOOL m_bHighlightSelected; // 
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETABCOLORSETVISUALSTUDIO2003_H__)
