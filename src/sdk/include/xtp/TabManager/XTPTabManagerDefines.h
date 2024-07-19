// XTPTabManagerDefines.h
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
#if !defined(__XTPTABMANAGERDEFINES_H__)
#define __XTPTABMANAGERDEFINES_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//     XTPTabNavigateButtonFlags is an enumeration used to indicate
//     when a navigate button will be displayed.  This style is
//     applied to one button.  Navigate buttons include the left,
//     right, and close buttons that appear in the tab header area.
//
// Remarks:
//     If automatic is selected, then the button will appear only
//     when needed.  I.e. When the XTPTabLayoutStyle is set to
//     xtpTabLayoutAutoSize, all tab might not fit in the tab header
//     area.  When there are more tabs than can fit in the header, the
//     button will automatically be displayed.
//
// Example:
//     This example code illustrates how to specify when the tab navigation
//     buttons are displayed.
// <code>
// //Gets a reference to the tab manager
// CXTPTabManager* pManager = GetManager();
// if (pManager)
// {
//     //Finds the left navigation button and specifies that it is always displayed
//     pManager->FindNavigateButton(xtpTabNavigateButtonLeft)->SetFlags(xtpTabNavigateButtonAlways);
//     //Finds the right navigation button and specifies that it is never displayed
//     pManager->FindNavigateButton(xtpTabNavigateButtonRight)->SetFlags(xtpTabNavigateButtonNone);
//     //Finds the close navigation button and specifies that it is always displayed
//     pManager->FindNavigateButton(xtpTabNavigateButtonClose)->SetFlags(xtpTabNavigateButtonAlways);
// }
// //Called to recalculate tab area and reposition components
// pManager->Reposition();
// </code>
// See Also: CXTPTabManagerNavigateButton, CXTPTabManagerNavigateButton::SetFlags,
//           CXTPTabManagerNavigateButton::GetFlags
//
// <KEYWORDS xtpTabNavigateButtonNone, xtpTabNavigateButtonAutomatic, xtpTabNavigateButtonAlways>
//-----------------------------------------------------------------------
enum XTPTabNavigateButtonFlags
{
	xtpTabNavigateButtonNone,       // Never display the navigate button.
	xtpTabNavigateButtonAutomatic,  // Automatically display the navigate button.
	xtpTabNavigateButtonAlways      // Always display the navigate button.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabAppearanceStyle is an enumeration used to set the appearance of tabs.
// Example:
//     <code>m_wndTabControl.SetAppearance(xtpTabAppearancePropertyPage2003);</code>
// See Also: CXTPTabManager::SetAppearance, CXTPTabPaintManager::SetAppearance
//-----------------------------------------------------------------------
enum XTPTabAppearanceStyle
{
    xtpTabAppearancePropertyPage           =  0, // Gives your tabs an Office 2000 appearance.
    xtpTabAppearancePropertyPageSelected   =  1, // Gives your tabs an Office 2000 selected appearance.
    xtpTabAppearancePropertyPageFlat       =  2, // Gives your tabs an Office 2000 Flat appearance.
    xtpTabAppearancePropertyPage2003       =  3, // Gives your tabs an Office 2003 appearance.
    xtpTabAppearanceStateButtons           =  4, // Gives your tabs a State Button appearance.
    xtpTabAppearanceFlat                   =  6, // Gives your tabs Flat appearance.
    xtpTabAppearanceExcel2003              =  7, // Gives your tabs an Excel 2003 appearance.
    xtpTabAppearanceVisio                  =  8, // Gives your tabs a Visio appearance.
    xtpTabAppearancePropertyPage2007       = 10, // Gives your tabs an Office 2007 appearance.
    xtpTabAppearancePropertyPageAccess2007 = 11, // Gives your tabs an Office 2007 appearance.

    xtpTabAppearanceVisualStudio2003       =  5, // Gives your tabs a Visual Studio 2003 appearance.
    xtpTabAppearanceVisualStudio2005       =  9, // Gives your tabs a Visual Studio 2005 appearance.
    xtpTabAppearanceVisualStudio2010       = 12, // Gives your tabs a Visual Studio 2010 appearance.
    xtpTabAppearanceVisualStudio2012       = 13, // Gives your tabs a Visual Studio 2012 appearance.
    xtpTabAppearanceVisualStudio2015       = 15, // Gives your tabs a Visual Studio 2015 appearance.

    xtpTabAppearanceOffice2013             = 14, // Office 2013 appearance
    xtpTabAppearanceOffice2016             = xtpTabAppearanceOffice2013, // Office 2016 appearance
};

//{{AFX_CODEJOCK_PRIVATE
#define xtpTabThemeExcel2003         xtpTabAppearanceExcel2003 // Old names
#define xtpTabThemeVisualStudio2003  xtpTabAppearanceVisualStudio2003
#define xtpTabThemeVisualStudio2005  xtpTabAppearanceVisualStudio2005
#define xtpTabThemeVisualStudio2010  xtpTabAppearanceVisualStudio2010
#define xtpTabThemeVisualStudio2012  xtpTabAppearanceVisualStudio2012
#define xtpTabThemeVisualStudio2015  xtpTabAppearanceVisualStudio2015
#define xtpTabThemeOffice2013        xtpTabAppearanceOffice2013
#define xtpTabAppearanceVisualStudio xtpTabAppearanceVisualStudio2003
#define xtpTabAppearanceExcel        xtpTabAppearanceExcel2003
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------
// Summary:
//     XTPTabColorStyle is an enumeration used to set the color style of tabs.
// Example:
//     <code>m_wndTabControl.SetColor(xtpTabColorVisualStudio2005);</code>
// See Also:
//     CXTPTabManager::SetColor, CXTPTabPaintManager::SetColor,
//     CXTPTabManager::GetColor, CXTPTabPaintManager::GetColor
//
// <KEYWORDS xtpTabColorDefault, xtpTabColorVisualStudio2003, xtpTabColorOffice2003, xtpTabColorWinNative, xtpTabColorVisualStudio2005>
//-----------------------------------------------------------------------
enum XTPTabColorStyle
{
	xtpTabColorDefault              = 0x0001, // Tabs will use the default color for the currently set Appearance.
	xtpTabColorOffice2003           = 0x0004, // Tabs will use the Office 2003 color style for the currently set Appearance.
	xtpTabColorWinNative            = 0x0008, // Tabs will use the Windows XP color style for the currently set Appearance.
	xtpTabColorResource             = 0x0020, // Tabs will use the Office 2007 color style for the currently set Appearance.
	xtpTabColorOffice2007Access     = 0x0080, // Tabs will use the Office 2007 Access color style for the currently set Appearance.
	xtpTabColorVisualStudio2003     = 0x0002, // Tabs will use the Visual Studio 2003 color style for the currently set Appearance.
	xtpTabColorVisualStudio2005     = 0x0010, // Tabs will use the Visual Studio 2005 color style for the currently set Appearance.
	xtpTabColorVisualStudio2008     = 0x0040, // Tabs will use the Visual Studio 2008 color style for the currently set Appearance.
	xtpTabColorVisualStudio2010     = 0x0100, // Tabs will use the Visual Studio 2010 color style for the currently set Appearance.
	xtpTabColorVisualStudio2012     = 0x0200, // Tabs will use the Visual Studio 2012 color style for the currently set Appearance.
	xtpTabColorVisualStudio2012Dark = 0x0400, // Tabs will use the Visual Studio 2012 Dark color style for the currently set Appearance.
	xtpTabColorOffice2013           = 0x0800, // Tabs will use the Office 2013 style for the currently set Appearance.
	xtpTabColorOffice2016           = xtpTabColorOffice2013, // Tabs will use the Office 2016 style for the currently set Appearance.
	xtpTabColorVisualStudio2015     = 0x1000, // Tabs will use the Visual Studio 2015 color style for the currently set Appearance.
	xtpTabColorAll                  = 0xFFFF,
};

//{{AFX_CODEJOCK_PRIVATE
#define xtpTabColorVisualStudio xtpTabColorVisualStudio2003 // obsolete names, for backward compatibility only.
#define xtpTabColorWinXP        xtpTabColorWinNative
#define xtpTabColorWhidbey      xtpTabColorVisualStudio2005
#define xtpTabColorAccess2007   xtpTabColorOffice2007Access
#define xtpTabColorOffice2007   xtpTabColorResource
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------
// Summary:
//     XTPTabPosition is an enumeration used to set the position of tabs.
// Example:
//     <code>m_wndTabControl.SetPosition(xtpTabPositionBottom);</code>
// See Also: CXTPTabManager::SetPosition, CXTPTabManager::GetPosition
//
// <KEYWORDS xtpTabPositionTop, xtpTabPositionLeft, xtpTabPositionBottom, xtpTabPositionRight>
//-----------------------------------------------------------------------
enum XTPTabPosition
{
	xtpTabPositionTop,          // Tabs will be drawn on the Top.
	xtpTabPositionLeft,         // Tabs will be drawn on the Left.
	xtpTabPositionBottom,       // Tabs will be drawn on the Bottom.
	xtpTabPositionRight         // Tabs will be drawn on the Right.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabLayoutStyle is an enumeration used to set the layout of tabs.
// Example:
//     <code>m_wndTabControl.SetLayoutStyle(xtpTabLayoutCompressed);</code>
// See Also: CXTPTabManager::SetLayoutStyle, CXTPTabManager::GetLayout
//
// <KEYWORDS xtpTabLayoutAutoSize, xtpTabLayoutSizeToFit, xtpTabLayoutFixed, xtpTabLayoutCompressed, xtpTabLayoutMultiRow>
//-----------------------------------------------------------------------
enum XTPTabLayoutStyle
{
	xtpTabLayoutAutoSize,       // Tabs will be automatically sized based on the caption and image size.  With this flag set, tabs will appear in their normal size.
	xtpTabLayoutSizeToFit,      // Tabs are sized to fit within the tab panel.  All tabs will be compressed and forced to fit into the tab panel.
	xtpTabLayoutFixed,          // All tabs will be set to a fixed size within the tab panel.
	xtpTabLayoutCompressed,     // Tabs will be compressed within the tab panel.  This will compress the size of the tabs, but all tabs will not be forced into the tab panel.
	xtpTabLayoutMultiRow,       // Causes a tab control to display multiple rows of tabs
	xtpTabLayoutRotated         // Rotated tab layout - all tabs rotated to 90 degree.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabToolTipBehaviour is an enumeration used to set tooltip behaviour for tabs
// Example:
//     <code>m_wndTabControl.GetPaintManager()->EnableToolTips(xtpTabToolTipAlways);</code>
// See Also: CXTPTabPaintManager::EnableToolTips, XTPTabLayoutStyle
//
// <KEYWORDS xtpTabToolTipNever, xtpTabToolTipAlways, xtpTabToolTipShrinkedOnly>
//-----------------------------------------------------------------------
enum XTPTabToolTipBehaviour
{
	xtpTabToolTipNever,         // Doesn't show tooltips for tabs
	xtpTabToolTipAlways,        // Show tooltips for tabs always
	xtpTabToolTipShrinkedOnly   // Show tooltips only if tab was shrinked (see xtpTabLayoutSizeToFit layout)
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabOneNoteColor is an enumeration used to indicate which OneNote
//     COLORREF value to retrieve.
// Remarks:
//     This enumerator is used by CXTPTabPaintManager::GetOneNoteColor to
//     retrieve a specified OneNote RGB color value.
// Example:
//      See WM_XTP_GETTABCOLOR for an example.
// See Also:
//     CXTPTabPaintManager::GetOneNoteColor, CXTPTabManager::GetItemColor,
//     CXTPDockingPane::GetItemColor
//
// <KEYWORDS xtpTabColorBlue, xtpTabColorYellow, xtpTabColorGreen, xtpTabColorRed, xtpTabColorPurple, xtpTabColorCyan, xtpTabColorOrange, xtpTabColorMagenta>
//-----------------------------------------------------------------------
enum XTPTabOneNoteColor
{
	xtpTabColorBlue    = 0x1000000, // Blue tab color used when OneNote colors enabled.
	xtpTabColorYellow  = 0x1000001, // Yellow tab color used when OneNote colors enabled.
	xtpTabColorGreen   = 0x1000002, // Green tab color used when OneNote colors enabled.
	xtpTabColorRed     = 0x1000003, // Red tab color used when OneNote colors enabled.
	xtpTabColorPurple  = 0x1000004, // Purple tab color used when OneNote colors enabled.
	xtpTabColorCyan    = 0x1000005, // Cyan tab color used when OneNote colors enabled.
	xtpTabColorOrange  = 0x1000006, // Orange tab color used when OneNote colors enabled.
	xtpTabColorMagenta = 0x1000007  // Magenta tab color used when OneNote colors enabled.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabWorkspacePart is an enumeration used to indicate which
//     part of the MDI TabWorkspace needs to be redrawn.
// Remarks:
//     The enumerators are used by CXTPTabPaintManagerTheme::DrawWorkspacePart
//     when redrawing the MDITabClient TabWorkspace when grouping is enabled.
// See Also: CXTPTabPaintManagerTheme::DrawWorkspacePart
//
// <KEYWORDS xtpTabWorkspacePartBorder, xtpTabWorkspacePartWidth, xtpTabWorkspacePartVSplitter, xtpTabWorkspacePartHSplitter>
//-----------------------------------------------------------------------
enum XTPTabWorkspacePart
{
	xtpTabWorkspacePartBorder,      // Indicates the border of the MDITabClient TabWorkspace should be drawn.
	xtpTabWorkspacePartWidth,       // Indicates the border width of the MDITabClient TabWorkspace should be recalculated.  I.e. When adding or removing a static frame.
	xtpTabWorkspacePartVSplitter,   // Indicates the vertical splitter of the MDITabClient TabWorkspace should be drawn.
	xtpTabWorkspacePartHSplitter,   // Indicates the horizontal splitter of the MDITabClient TabWorkspace should be drawn.
	xtpTabWorkspacePartBack         // Indicates the background of the MDITabClient TabWorkspace should be drawn.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabClientFrame is an enumeration used to set the client frame style drawn around the client of tabs.
// Example:
//     <code>m_wndTabControl.GetPaintManager()->m_clientFrame = xtpTabFrameBorder;</code>
// See Also: CXTPTabManager, CXTPTabPaintManager
//
// <KEYWORDS xtpTabFrameBorder, xtpTabFrameSingleLine, xtpTabFrameNone>
//-----------------------------------------------------------------------
enum XTPTabClientFrame
{
	xtpTabFrameBorder,      // With this flag set, a border will appear around the client area.
	xtpTabFrameSingleLine,  // With this flag set, only a single line is used as a border around the client area.
	xtpTabFrameNone         // With this flag set, no border will appear around the client area.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPTabNavigateButton is an enumeration used to identify navigation
//     buttons within a tab control.
// Remarks:
//     This enumerator is used by the CXTPTabManagerNavigateButton class to
//     identify navigation buttons.
// Example:
//      See XTPTabNavigateButtonFlags for an example.
// See Also:
//     XTPTabNavigateButtonFlags, CXTPTabManager, CXTPTabManagerNavigateButton
//
// <KEYWORDS xtpTabNavigateButtonLeft, xtpTabNavigateButtonRight, xtpTabNavigateButtonClose>
//-----------------------------------------------------------------------
enum XTPTabNavigateButton
{
	xtpTabNavigateButtonLeft,   // Left tab navigation button.
	xtpTabNavigateButtonRight,  // Right tab navigation button.
	xtpTabNavigateButtonClose,   // Close tab navigation button.
	xtpTabNavigateButtonActiveFiles // Active Files button (Used in XTPTabClientWnd only).
};


//-----------------------------------------------------------------------
// Summary:
//     Indicates that a transparent color should be used.
// Remarks:
//     If an objects COLORREF value is XTP_TABMANAGER_COLOR_NONE it
//     is used to indicate that the object should not be drawn.  This
//     is used in the tab appearance and color functions to indicate
//     when tab borders, highlighting, shadowing, etc should be drawn.
//
//      Setting a member of CXTPTabPaintManagerColorSet to
//      XTP_TABMANAGER_COLOR_NONE will indicate that the member should
//      not be drawn.
// See Also: CXTPTabPaintManagerColorSet
//-----------------------------------------------------------------------
const COLORREF XTP_TABMANAGER_COLOR_NONE = (COLORREF)(-1);

//-----------------------------------------------------------------------
// Summary:
//     Indicates a special "shaded" fill type.
// Remarks:
//     To get the "shaded" effect, the object will be filled with
//     one pixel COLOR_3DFACE, one pixel COLOR_3DHIGHLIGHT, one
//     pixel COLOR_3DFACE....
//
//           Setting the clrLight parameter to XTP_TABMANAGER_COLOR_SHADED
//           in the used in CXTPTabPaintManagerColorSet::GradientFill
//           member will cause the object to be filled with the special shaded fill.
//
// NOTE:
//     This is used in xtpTabAppearanceStateButtons to colorize
//     the button face.  The special fill was used to color of the currently
//     selected tab.  The special fill can be applied to any
//     color member.
// See Also: CXTPTabPaintManagerColorSet::GradientFill
//-----------------------------------------------------------------------
const COLORREF XTP_TABMANAGER_COLOR_SHADED = (COLORREF)(-2);


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPTABMANAGERDEFINES_H__)
