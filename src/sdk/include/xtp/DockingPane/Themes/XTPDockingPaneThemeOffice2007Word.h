// XTPDockingPaneThemeOffice2007Word.h
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
#if !defined(__XTPDOCKINGPANETHEMEOFFICE2007WORD_H__)
#define __XTPDOCKINGPANETHEMEOFFICE2007WORD_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneOffice2007WordTheme is CXTPDockingPaneOffice2003Theme derived class, represents
//     Office Word 2007 theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice2007Word); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneOffice2007WordTheme : public CXTPDockingPaneOffice2003Theme
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneOffice2007WordTheme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneOffice2007WordTheme();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the pane.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw a caption.
	// Parameters:
	//     pDC - Reference to the device context in which to draw.
	//     pPane - Pointer to the tabbed container.
	//     rc - Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	void DrawCaption(CDC *pDC, CXTPDockingPaneTabbedContainer *pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw common part of tabbed and floating caption.
	// Parameters:
	//     pDC        - Reference to the device context in which to draw.
	//     pPane     - Container which caption need to draw.
	//     rcCaption - Caption bounding rectangle
	//     strTitle  - Caption text.
	//     bActive   - TRUE if caption is active.
	//     bVertical - TRUE to draw caption vertically
	//-----------------------------------------------------------------------
	void DrawCaptionPart(CDC* pDC, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member to change the client rectangle of the child
	//     docking pane.
	// Parameters:
	//     pPane  - Pointer to the tabbed container.
	//     rect   - Client rectangle to be changed.
	//     bApply - TRUE to update tabs inside client area.
	//-----------------------------------------------------------------------
	void AdjustClientRect(CXTPDockingPaneTabbedContainer* pPane, CRect& rect, BOOL bApply);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw a tabbed container.
	// Parameters:
	//     pDC    - Reference to the device context in which to draw.
	//     pPane - Pointer to the tabbed container.
	//     rc    - Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	void DrawPane(CDC* pDC, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a pane has borders (if title is visible).
	// Parameters:
	//     pPane - Pointer to the tabbed container.
	// Returns:
	//      TRUE if the pane has borders (title visible), FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL PaneHasBorders(CXTPDockingPaneTabbedContainer* pPane) const;

protected:
	BOOL m_bOffice2010Style;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEOFFICE2007WORD_H__)
