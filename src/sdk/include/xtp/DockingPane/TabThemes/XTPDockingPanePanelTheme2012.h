// XTPDockingPanePanelTheme2012.h
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
#if !defined(__XTPDOCKINGPANEPANELTHEME2012_H__)
#define __XTPDOCKINGPANEPANELTHEME2012_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Helper class used to initialize default colors for a theme from 
//          an INI resource and handle drawing routines.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPanePanelTheme2012 : public CXTPTabThemeFlat
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default class constructor.
    //-----------------------------------------------------------------------
	CXTPDockingPanePanelTheme2012();

    //-----------------------------------------------------------------------
    // Summary: This member function is used to initialize default colors 
	//          for a theme from an INI resource.
    //-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:    This method is called to draw a single tab button in the tab
	//             client header area.
	// Parameters: pDC   : Pointer to a valid device context.
	//             pItem : Tab button to draw.
	// Remarks:    This method will draw a single tab button.  This method only
	//             draws the button, no text or icon is added.
	//
	//             DrawSingleButton calls CXTPTabPaintManager::DrawSingleButtonIconAndText
	//             after the button is drawn.  This is called to draw the button's
	//             icon and text.
	// See Also:   CXTPTabPaintManager::DrawTabControlEx, CXTPTabPaintManager::DrawSingleButtonIconAndText
	//-----------------------------------------------------------------------
	virtual void DrawSingleButton(CDC *pDC, CXTPTabManagerItem *pItem);

protected:

	int m_nGripperWidth; // Size of the docking pane gripper.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANEPANELTHEME2012_H__)
