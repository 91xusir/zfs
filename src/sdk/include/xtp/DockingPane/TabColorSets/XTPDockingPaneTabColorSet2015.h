// XTPDockingPaneTabColorSet2015
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
#if !defined(__XTPDOCKINGPANETABCOLORSET2015_H__)
#define __XTPDOCKINGPANETABCOLORSET2015_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Helper class used to initialize default colors for a theme from 
//          an INI resource and handle drawing routines.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneTabColorSet2015 : public CXTPDockingPaneTabColorSet2012
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default class constructor.
    //-----------------------------------------------------------------------
	CXTPDockingPaneTabColorSet2015();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to fill the header of the TabClient of the tab manager.
	// Parameters:
	//     pDC         - Pointer to a valid device context.
	//     rc          - Bounding rectangle of the  tab header.
	//     pTabManager - Fill the header of this CXTPTabManager object.
	// Remarks:
	//     The TabClient header is the bounding rectangle with tab tab buttons.
	//     The bounding rectangle can be retrieved with the CXTPTabPaintManagerTheme::GetHeaderRect,
	//     and CXTPTabManager::GetHeaderRect members.
	// See Also: CXTPTabPaintManagerTheme::GetHeaderRect, CXTPTabManager::GetHeaderRect
	//-----------------------------------------------------------------------
	virtual void FillHeader(CDC* pDC, CRect rc, CXTPTabManager* pTabManager);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANETABCOLORSET2015_H__)
