// XTPDockingPanePanelColorSet2015.h
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
#if !defined(__XTPDOCKINGPANEPANELCOLORSET2015_H__)
#define __XTPDOCKINGPANEPANELCOLORSET2015_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Helper class used to initialize default colors for a theme from 
//          an INI resource and handle drawing routines.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPanePanelColorSet2015 : public CXTPDockingPanePanelColorSet2012
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default class constructor.
    //-----------------------------------------------------------------------
	CXTPDockingPanePanelColorSet2015();

    //-----------------------------------------------------------------------
    // Summary: This member function is used to initialize default colors 
	//          for a theme from an INI resource.
    //-----------------------------------------------------------------------
	virtual void RefreshMetrics();
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANEPANELCOLORSET2015_H__)
