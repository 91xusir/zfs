// XTPCommandBarsColorSetOffice2013.h : interface for the CXTPCommandBarsColorSetOffice2013 class.
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
#if !defined(__XTPCOMMANDBARSCOLORSETOFFICE2013_H__)
#define __XTPCOMMANDBARSCOLORSETOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Base class used to define various state colors used by command bar
//          buttons text, background and border.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateButton : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPColorSetStateButton)

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors() = 0;

	CXTPPaintManagerColor clrText;                   // Text Normal color.
	CXTPPaintManagerColor clrTextHighlight;          // Text Highlight color.
	CXTPPaintManagerColor clrTextPushed;             // Text Pushed color.
	CXTPPaintManagerColor clrTextDisabled;           // Text Disabled color.
	CXTPPaintManagerColor clrTextChecked;            // Text Checked color.
	CXTPPaintManagerColor clrTextCheckedHighlight;   // Text Checked highlight color.

	CXTPPaintManagerColor clrBack;                   // Background Normal color.
	CXTPPaintManagerColor clrBackHighlight;          // Background Highlight color.
	CXTPPaintManagerColor clrBackPushed;             // Background Pushed color.
	CXTPPaintManagerColor clrBackDisabled;           // Background Disabled color.
	CXTPPaintManagerColor clrBackChecked;            // Background Checked color.
	CXTPPaintManagerColor clrBackCheckedHighlight;   // Background Checked highlight color.

	CXTPPaintManagerColor clrBorder;                 // Border Normal color.
	CXTPPaintManagerColor clrBorderHighlight;        // Border Highlight color.
	CXTPPaintManagerColor clrBorderPushed;           // Border Pushed color.
	CXTPPaintManagerColor clrBorderDisabled;         // Border Disabled color.
	CXTPPaintManagerColor clrBorderChecked;          // Border Checked color.
	CXTPPaintManagerColor clrBorderCheckedHighlight; // Border Checked highlight color.
};

//===========================================================================
// Summary: Color set class used to define various state colors used by 
//          command bar menus.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateMenu : public CXTPColorSetStateButton
{
	DECLARE_DYNAMIC(CXTPColorSetStateMenu)

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

    CXTPPaintManagerColor         clrMenuBack;   // Menu background color.
    CXTPPaintManagerColor         clrMenuBorder; // Menu border color.
    CXTPPaintManagerColor         clrGripper;    // Menu item icon strip background color.
    CXTPPaintManagerColorGradient clrSeparator;  // Separator dark and light colors.
};

//===========================================================================
// Summary: Color set class used to define various state colors used by 
//          command bar ribbon.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateRibbon : public CXTPColorSetStateButton
{
	DECLARE_DYNAMIC(CXTPColorSetStateRibbon)

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

    CXTPPaintManagerColor clrTabTextInactive; // Ribbon tab inactive text color;
    CXTPPaintManagerColor clrTabBackInactive; // Ribbon tab inactive background color;
};

//===========================================================================
// Summary: Color set class used to define various state colors used by 
//          command bar toolbars.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateToolbar : public CXTPColorSetStateButton
{
	DECLARE_DYNAMIC(CXTPColorSetStateToolbar)

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

    CXTPPaintManagerColor         clrToolbarBack;   // Toolbar background color.
    CXTPPaintManagerColor         clrToolbarBorder; // Toolbar border color.
    CXTPPaintManagerColor         clrEditBorder;    // Toolbar edit control border color.
    CXTPPaintManagerColor         clrGripper;       // Toolbar gripper color.
    CXTPPaintManagerColor         clrDisabledIcon;  // Toolbar disabled icon color.
    CXTPPaintManagerColorGradient clrSeparator;     // Separator dark and light colors.
};

//===========================================================================
// Summary:
//     Helper class used to define various state colors used by command bar
//     check marks foreground and background.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateCheckMark
{

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

    CXTPPaintManagerColor clrCheck;          // Foreground Normal color.
    CXTPPaintManagerColor clrCheckHighlight; // Foreground Highlight color.
    CXTPPaintManagerColor clrCheckPushed;    // Foreground Pushed color.
    CXTPPaintManagerColor clrCheckDisabled;  // Foreground Disabled color.

    CXTPPaintManagerColor clrBack;           // Background Normal color.
    CXTPPaintManagerColor clrBackHighlight;  // Background Highlight color.
    CXTPPaintManagerColor clrBackPushed;     // Background Pushed color.
    CXTPPaintManagerColor clrBackDisabled;   // Background Disabled color.
};

//===========================================================================
// Summary:
//     Helper class used to define various state colors used by command bar
//     glyphs.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateComboBox
{

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

    CXTPPaintManagerColor clrBack;                    // ComboBox normal background color.
	CXTPPaintManagerColor clrBackHighlight;           // ComboBox highlight background color.
    CXTPPaintManagerColor clrBackDropped;             // ComboBox dropped background color.
    CXTPPaintManagerColor clrBackDisabled;            // ComboBox disabled background color.

    CXTPPaintManagerColor clrBorder;                  // ComboBox normal border color.
	CXTPPaintManagerColor clrBorderHighlight;         // ComboBox highlight border color.
    CXTPPaintManagerColor clrBorderDropped;           // ComboBox dropped border color.
    CXTPPaintManagerColor clrBorderDisabled;          // ComboBox disabled border color.

    CXTPPaintManagerColor clrDropDownBack;            // Drop down ComboBox normal background color.
	CXTPPaintManagerColor clrDropDownBackHighlight;   // Drop down ComboBox highlight background color.
    CXTPPaintManagerColor clrDropDownBackDropped;     // Drop down ComboBox dropped background color.
    CXTPPaintManagerColor clrDropDownBackDisabled;    // Drop down ComboBox disabled background color.

    CXTPPaintManagerColor clrDropDownBorder;          // Drop down ComboBox normal border color.
	CXTPPaintManagerColor clrDropDownBorderHighlight; // Drop down ComboBox highlight border color.
    CXTPPaintManagerColor clrDropDownBorderDropped;   // Drop down ComboBox dropped border color.
    CXTPPaintManagerColor clrDropDownBorderDisabled;  // Drop down ComboBox disabled border color.

    CXTPPaintManagerColor clrDropDownMarker;          // Drop down ComboBox normal marker color.
	CXTPPaintManagerColor clrDropDownMarkerHighlight; // Drop down ComboBox highlight marker color.
    CXTPPaintManagerColor clrDropDownMarkerDropped;   // Drop down ComboBox dropped marker color.
    CXTPPaintManagerColor clrDropDownMarkerDisabled;  // Drop down ComboBox disabled marker color.
};

//===========================================================================
// Summary:
//     Helper class used to define various state colors used by check box and
//     radio button controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetStateCheckBox
{

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the colors from predefined
	//          Section and Key values located in the theme INI file for loaded 
	//          resource DLL.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

    CXTPPaintManagerColor clrMark;
    CXTPPaintManagerColor clrMarkPushed;
    CXTPPaintManagerColor clrMarkHighlight;
    CXTPPaintManagerColor clrMarkDisabled;
    CXTPPaintManagerColor clrBack;
    CXTPPaintManagerColor clrBackHighlight;
    CXTPPaintManagerColor clrBackPushed;
    CXTPPaintManagerColor clrBackDisabled;
    CXTPPaintManagerColor clrBorder;
    CXTPPaintManagerColor clrBorderPushed;
    CXTPPaintManagerColor clrBorderHighlight;
    CXTPPaintManagerColor clrBorderDisabled;
};

/////////////////////////////////////////////////////////////////////////////


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSCOLORSETOFFICE2013_H__)
