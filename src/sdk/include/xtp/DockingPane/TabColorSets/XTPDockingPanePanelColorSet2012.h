// XTPDockingPanePanelColorSet2012.h
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
#if !defined(__XTPDOCKINGPANEPANELCOLORSET2012_H__)
#define __XTPDOCKINGPANEPANELCOLORSET2012_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Helper class used to initialize default colors for a theme from 
//          an INI resource and handle drawing routines.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPanePanelColorSet2012 : public CXTPTabPaintManagerColorSet
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default class constructor.
    //-----------------------------------------------------------------------
	CXTPDockingPanePanelColorSet2012();

    //-----------------------------------------------------------------------
    // Summary: This member function is used to initialize default colors 
	//          for a theme from an INI resource.
    //-----------------------------------------------------------------------
	virtual void RefreshMetrics();

    //-----------------------------------------------------------------------
    // Summary:    Call this member funciton to retrieve the color for the
	//             tab specified by pItem.
    // Parameters: pItem : Points to a valid CXTPTabManagerItem object.
    // Returns:    An RGB color value representing the specified tab color.
    //-----------------------------------------------------------------------
	virtual COLORREF GetItemColor(CXTPTabManagerItem *pItem);

    //-----------------------------------------------------------------------
	// Summary:    This member is called to fill the tab navigation buttons.
	// Parameters: pDC     : Pointer to a valid device context.
	//             pButton : Tab navigation button to fill.
	//             rc      : Bounding rectangle of the tab navigation button.
	// Remarks:    This member must be overridden and takes care of filling 
	//             the tab navigation buttons that are in the header of the 
	//             TabClient.  The XTPTabColorStyle CXTPTabPaintManagerColorSet 
	//             classes override this to perform actions such as painting 
	//             the highlighting, pressed, and normal versions of the tab 
	//             navigation buttons.
	// See Also:   CXTPTabManager::GetNavigateButton, CXTPTabManagerNavigateButton
    //-----------------------------------------------------------------------
	virtual void FillNavigateButton(CDC *pDC, CXTPTabManagerNavigateButton *pButton, CRect &rc);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANEPANELCOLORSET2012_H__)
