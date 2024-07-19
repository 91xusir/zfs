// XTPDockingPaneThemeVisualStudio2003.h
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
#if !defined(__XTPDOCKINGPANETHEMEVISUALSTUDIO2003_H__)
#define __XTPDOCKINGPANETHEMEVISUALSTUDIO2003_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneVisualStudio2003Theme is CXTPDockingPaneGripperedTheme derived class, represents
//     Office XP theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeVisualStudio2003); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneVisualStudio2003Theme : public CXTPDockingPaneGripperedTheme
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneVisualStudio2003Theme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneVisualStudio2003Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the pane.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

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

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the background of the pane caption.
	// Parameters:
	//     pDC        - Reference to the device context in which to draw.
	//     pPane     - Container of the caption needs to draw the background.
	//     rcCaption - Caption bounding rectangle
	//     strTitle  - Caption text.
	//     bActive   - TRUE if caption is active.
	//     bVertical - TRUE to draw caption vertically
	//-----------------------------------------------------------------------
	virtual COLORREF FillCaptionPart(CDC* pDC, CXTPDockingPaneBase* pPane, CRect rcCaption, BOOL bActive, BOOL bVertical);

protected:
	BOOL m_bGradientCaption;                            // TRUE to draw gradient caption.
	int m_nCaptionButtonSize;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEVISUALSTUDIO2003_H__)
