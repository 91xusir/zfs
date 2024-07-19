// XTPDockingPaneThemeVisualStudio2005Beta1.h
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
#if !defined(__XTPDOCKINGPANETHEMEVISUALSTUDIO2005BETA1_H__)
#define __XTPDOCKINGPANETHEMEVISUALSTUDIO2005BETA1_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneVisualStudio2005Beta1Theme is CXTPDockingPaneVisualStudio2003Theme derived class, represents
//     Visual Studio 2005 theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeVisualStudio2005Beta1); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneVisualStudio2005Beta1Theme : public CXTPDockingPaneVisualStudio2003Theme
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneVisualStudio2005Beta1Theme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneVisualStudio2005Beta1Theme();

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the pane.
	//-------------------------------------------------------------------------
	void RefreshMetrics();
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETHEMEVISUALSTUDIO2005BETA1_H__)
