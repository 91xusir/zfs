// XTPDockingPaneThemeOffice2003.h
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
#if !defined(__XTPDOCKINGPANETHEMEOFFICE2003_H__)
#define __XTPDOCKINGPANETHEMEOFFICE2003_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneOffice2003Theme is CXTPDockingPaneVisualStudio2003Theme derived class, represents
//     Office 2003 theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice2003); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneOffice2003Theme : public CXTPDockingPaneVisualStudio2003Theme
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneOffice2003Theme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneOffice2003Theme();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneOffice2003Theme object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPDockingPaneOffice2003Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the pane.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw a caption.
	// Parameters:
	//     pDC - Reference to the device context in which to draw.
	//     pPane - Pointer to the tabbed container.
	//     rc - Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawCaption(CDC* pDC, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw frame of floating window
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     pPane - Floating window pointer
	//     rc    - Client rectangle of floating frame
	//-----------------------------------------------------------------------
	virtual void DrawFloatingFrame(CDC* pDC, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw the side panel.
	// Parameters:
	//     pDC    - Reference to the device context in which to draw.
	//     pPane - Pointer to the side panel.
	//     rc    - Client rectangle of the side panel.
	//-----------------------------------------------------------------------
	virtual void DrawSidePanel(CDC* pDC, CXTPDockingPaneSidePanel* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get pane's caption rectangle.
	// Parameters:
	//     pPane - Side Panel container to get caption CRECT.
	// Returns:
	//     Bounding rectangle of the side panel's caption.
	//-----------------------------------------------------------------------
	virtual CRect GetPaneCaptionRect(const CXTPDockingPaneSidePanel* pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get caption gripper for tabbed container
	// Parameters:
	//     pPane - Tabbed container to test
	// Returns: Bounding rectangle of gripper to set size cursor
	//-----------------------------------------------------------------------
	virtual CRect GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* pPane);

protected:
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
	virtual void DrawCaptionPart(CDC* pDC, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

public:
	BOOL     m_bDrawGripper;            // TRUE to draw gripper
protected:
	BOOL     m_bRoundedCaption;         // TRUE to draw rounded caption
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEOFFICE2003_H__)
