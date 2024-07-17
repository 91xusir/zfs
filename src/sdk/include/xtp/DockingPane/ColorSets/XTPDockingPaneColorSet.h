// XTPDockingPaneColorSet.h
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
#if !defined(__XTPDOCKINGPANECOLORSET_H__)
#define __XTPDOCKINGPANECOLORSET_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Helper class used to initialize default colors for a theme from 
//          an INI resource
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneColorSet
{
public:
    //-----------------------------------------------------------------------
    // Summary:    This member function is used to initialize default colors 
	//             for a theme from an INI resource.
    // Parameters: strSection : Location of color settings in theme INI file.
    //-----------------------------------------------------------------------
	virtual void RefreshMetrics(CString strSection) = 0;

    //-----------------------------------------------------------------------
    // Summary:    Call this member function to retrieve the state color set 
	//             for the caption button specified by pButton.
	// Parameters: pButton : Pointer to a CXTPDockingPaneCaptionButton object to
    //                       used to determine the correct color set to return.
    // Returns:    A reference to a CXTPColorSetCaptionButton object representing
	//             the caption button state colors.
    //-----------------------------------------------------------------------
	virtual CXTPColorSetCaptionButton& GetButtonColorSet(CXTPDockingPaneCaptionButton *pButton);

	CXTPPaintManagerColor m_clrBorder;     // Border color
	CXTPPaintManagerColor m_clrBackground; // Background color
	CXTPPaintManagerColor m_clrCaption;    // Caption color
	CXTPPaintManagerColor m_clrText;       // Text color
	CXTPPaintManagerColor m_clrGripper;    // Gripper color
	CXTPPaintManagerColor m_clrSplitter;   // Splitter color

protected:
	CXTPColorSetCaptionButton m_csCaptionButtonHot;     // Selected state caption button colors.
	CXTPColorSetCaptionButton m_csCaptionButtonNormal;  // Normal state caption button colors.
	CXTPColorSetCaptionButton m_csCaptionButtonPressed; // Pressed state caption button colors.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANECOLORSET_H__)
