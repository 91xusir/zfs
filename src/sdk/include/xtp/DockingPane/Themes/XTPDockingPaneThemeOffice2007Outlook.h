// XTPDockingPaneThemeOffice2007Outlook.h
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
#if !defined(__XTPDOCKINGPANETHEMEOFFICE2007OUTLOOK_H__)
#define __XTPDOCKINGPANETHEMEOFFICE2007OUTLOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneOffice2007OutlookTheme is CXTPDockingPaneOffice2007Theme derived class, represents
//     Outlook 2007 theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice2007Outlook); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneOffice2007OutlookTheme : public CXTPDockingPaneOffice2007Theme
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneOffice2007OutlookTheme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneOffice2007OutlookTheme();

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
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEOFFICE2007OUTLOOK_H__)
