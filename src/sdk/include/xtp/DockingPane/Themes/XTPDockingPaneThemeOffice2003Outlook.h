// XTPDockingPaneThemeOffice2003Outlook.h
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
#if !defined(__XTPDOCKINGPANETHEMEOFFICE2003OUTLOOK_H__)
#define __XTPDOCKINGPANETHEMEOFFICE2003OUTLOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneOffice2003OutlookTheme is CXTPDockingPaneOffice2003Theme derived class, represents
//     Office 2003 theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice2003Outlook); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneOffice2003OutlookTheme : public CXTPDockingPaneOffice2003Theme
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneOffice2003OutlookTheme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneOffice2003OutlookTheme();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneOffice2003OutlookTheme object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPDockingPaneOffice2003OutlookTheme();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the pane.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get caption gripper for tabbed container
	// Parameters:
	//     pPane - Tabbed container to test
	// Returns: Bounding rectangle of gripper to set size cursor
	//-----------------------------------------------------------------------
	virtual CRect GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* pPane);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEOFFICE2003OUTLOOK_H__)
