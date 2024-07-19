// XTPStatusBarThemeOffice2013.h
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
#if !defined(__XTPSTATUSBARTHEMEOFFICE2013_H__)
#define __XTPSTATUSBARTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     The CXTPStatusBarThemeVisualStudio2015 class is used to enable the 
//     Visual Studio 2015 style theme for the status bar.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBarThemeOffice2013 : public CXTPStatusBarPaintManager
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarThemeOffice2013 object.
	// Parameters:
	//     pPaintManager - Points to the parent CXTPPaintManager object.
	//-------------------------------------------------------------------------
	CXTPStatusBarThemeOffice2013(CXTPPaintManager *pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill a status bar.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPStatusBar object
	//-----------------------------------------------------------------------
	virtual void FillStatusBar(CDC *pDC, CXTPStatusBar *pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the status bar's gripper.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     rcClient - Client rectangle of the status bar.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarGripper(CDC *pDC, CRect rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw frame of single cell of status bar.
	// Parameters:
	//     pDC - Points to a valid device context.
	//     rc - CRect object specifying size of area.
	//     pPane - The status bar pane need to draw
	//     bGripperPane - TRUE if pane is last cell of status bar
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarPaneBorder(CDC *pDC, CRect rc, CXTPStatusBarPane *pPane, BOOL bGripperPane);

	//-----------------------------------------------------------------------
	// Input:   pDC - Pointer to a valid device context.
	//          rc - CRect object specifying size of area.
	//          pPane - The status bar pane need to draw.
	// Summary: This method is called to draw a status bar switch pane separator.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarPaneSeparator(CDC *pDC, CRect rc, CXTPStatusBarPane *pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a single pane text of the status bar.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     pPane - The status bar pane need to draw.
	//     rcItem - Item rectangle.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarPaneEntry(CDC *pDC, CRect rcItem, CXTPStatusBarPane *pPane);

	//-----------------------------------------------------------------------
	// Input:   pDC - Pointer to a valid device context.
	//          rcItem - Item rectangle.
	//          pPane - The status bar pane need to draw.
	// Summary: This method is called to draw a status bar button.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarButtonFace(CDC *pDC, CRect rcItem, CXTPStatusBarPane *pPane);

	//-----------------------------------------------------------------------
	// Input:   pDC - Pointer to a valid device context.
	//          rcItem - Item rectangle.
	//          pPane - The status bar pane need to draw.
	// Summary: This method is called to draw a status bar switch pane.
	// Returns: Returns width of switch pane
	//-----------------------------------------------------------------------
	virtual int DrawStatusBarSwitchPane(CDC *pDC, CRect rcItem, CXTPStatusBarSwitchPane *pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh colors and other theme specific 
	//     metrics for the paint manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve the text color for the pane 
	//     specified by pPane.
	// Input: pPane - Points to the CXTPStatusBarPane object to retrieve
	//                color for.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(CXTPStatusBarPane* pPane);

protected:

	CXTPPaintManagerColor m_clrButtonBack;          // Status bar button background color.
	CXTPPaintManagerColor m_clrButtonBackChecked;   // Status bar button checked background color.
	CXTPPaintManagerColor m_clrButtonBackHighlight; // Status bar button highlight background color.
	CXTPPaintManagerColor m_clrButtonBackPushed;    // Status bar button pushed background color.
	CXTPPaintManagerColor m_clrButtonTextChecked;   // Status bar button checked text color.
	CXTPPaintManagerColor m_clrButtonTextHighlight; // Status bar button highlight text color.
	CXTPPaintManagerColor m_clrButtonTextPushed;    // Status bar button pushed text color.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSTATUSBARTHEMEOFFICE2013_H__)
