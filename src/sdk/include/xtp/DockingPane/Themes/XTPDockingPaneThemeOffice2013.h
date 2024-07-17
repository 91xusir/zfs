// XTPDockingPaneThemeOffice2013.h
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
#if !defined(__XTPDOCKINGPANETHEMEOFFICE2013_H__)
#define __XTPDOCKINGPANETHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: CXTPDockingPaneOffice2013Theme is used to define drawing routines
//          specific to an Office 2013 docking pane theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneOffice2013Theme : public CXTPDockingPaneVisualStudio2012Theme
{
public:
	//-------------------------------------------------------------------------
	// Summary: Constructs a CXTPDockingPaneOffice2013Theme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneOffice2013Theme();

	//-------------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneOffice2013Theme object, handles 
	//          cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPDockingPaneOffice2013Theme();

	//-----------------------------------------------------------------------
    // Summary: This member function is used to initialize default colors 
	//          for a theme from an INI resource.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:    Override this method to draw splitter.
	// Parameters: pDC       : Pointer to a valid device context
	//             pSplitter : Pointer to splitter window
	//-----------------------------------------------------------------------
	virtual void DrawSplitter(CDC *pDC, CXTPDockingPaneSplitterWnd *pSplitter);

	//-----------------------------------------------------------------------
	// Summary:    Override this member function to draw a caption.
	// Parameters: pDC   : Reference to the device context in which to draw.
	//             pPane : Pointer to the tabbed container.
	//             rc    : Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawCaption(CDC *pDC, CXTPDockingPaneTabbedContainer *pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:    Call this method to draw frame of floating window
	// Parameters: pDC   : Pointer to a valid device context
	//             pPane : Floating window pointer
	//             rc    : Client rectangle of floating frame
	//-----------------------------------------------------------------------
	virtual void DrawFloatingFrame(CDC *pDC, CXTPDockingPaneMiniWnd *pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:    Override this member function to draw a tabbed container.
	// Parameters: pDC   : Reference to the device context in which to draw.
	//             pPane : Pointer to the tabbed container.
	//             rc    : Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawPane(CDC *pDC, CXTPDockingPaneTabbedContainer *pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:    Override this member function to draw the side panel.
	// Parameters: pDC   : Reference to the device context in which to draw.
	//             pPane : Pointer to the side panel.
	//             rc    : Client rectangle of the side panel.
	//-----------------------------------------------------------------------
	virtual void DrawSidePanel(CDC *pDC, CXTPDockingPaneSidePanel *pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:    Override this member function to draw button of tabbed caption.
	// Parameters: pDC       : Pointer to a valid device context
	//             pButton   : Button pointer need to draw.
	//             clrButton : Color of button text.
	//-----------------------------------------------------------------------
	virtual void DrawCaptionButton(CDC *pDC, CXTPDockingPaneCaptionButton *pButton, COLORREF clrButton);

	//-----------------------------------------------------------------------
	// Summary:    This mmethod is called by the framework to draw the docking pane
	//             auto hide panel background and splitter.
	// Parameters: pDC       : Pointer to the auto hide panes device context.
	//             rc        : Size of the area to draw.
	//             direction : Docking direction either xtpPaneDockLeft, xtpPaneDockRight,
	//                         xtpPaneDockTop or xtpPaneDockBottom.
	//-----------------------------------------------------------------------
	virtual void DrawAutoHidePanel(CDC* pDC, CRect rc, XTPDockingPaneDirection direction);

protected:
    //-----------------------------------------------------------------------
    // Summary:    This member function is called to draw a titel bar for the
	//             dockign pane.
    // Parameters: pDC   : Pointer to a valid device context.
	//             pPane : Pointer to a valid CXTPDockingPaneBase object.
	//             rc    : Size of the title bar to draw.
	//             bVertical : TRUE it the title bar should be drawn vertical.
    //-----------------------------------------------------------------------
	virtual void DrawTitleBar(CDC *pDC, CXTPDockingPaneBase *pPane, CRect rc, BOOL bVertical);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEOFFICE2013_H__)
