// XTPCommandBarsVisualStudio2010Theme.h
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
#if !defined(__XTPCOMMANDBARSVISUALSTUDIO2010THEME_H__)
#define __XTPCOMMANDBARSVISUALSTUDIO2010THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     The CXTPVisualStudio2008Theme class is used to enable a Visual Studio 2008 style theme for Command Bars
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPVisualStudio2010Theme : public CXTPCommandBarsOffice2003Theme
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPVisualStudio2008Theme object.
	//-----------------------------------------------------------------------
	CXTPVisualStudio2010Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPVisualStudio2008Theme object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPVisualStudio2010Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve the command bar's borders.
	// Parameters:
	//     pBar - Points to a CXTPCommandBar object that the borders need to get.
	// Returns:
	//     Borders of the command bar.
	//-----------------------------------------------------------------------
	virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw popup bar right gripper.
	// Parameters:
	//     pDC       - Points to a valid device context.
	//     xPos         - Specifies the logical x coordinate of the upper-left corner of the rectangle.
	//     yPos         - Specifies the logical y coordinate of the upper-left corner of the destination rectangle.
	//     cx        - Specifies the width of the rectangle.
	//     cy        - Specifies the height of the rectangle.
	//     bExpanded - TRUE if expanded.gripper.
	//-----------------------------------------------------------------------
	virtual void DrawPopupBarGripper(CDC* pDC, int xPos, int yPos, int cx, int cy, BOOL bExpanded);

	//-----------------------------------------------------------------------
	// Summary: This method is called to fill the control's face.
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     rc          - Rectangle to draw.
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type.
	//     barPosition - Parent's bar position.
	//     nID         - Bar's control ID.
	//-----------------------------------------------------------------------
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition, int nID); // <combine CXTPVisualStudio2010Theme::DrawRectangle@CDC*@CRect@BOOL@BOOL@BOOL@BOOL @BOOL@XTPBarType@XTPBarPosition>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the control's face
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPControl object to draw.
	// Returns:
	//     TRUE if the rectange was drawn, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DrawRectangle(CDC* pDC, CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves buttons text color.
	// Parameters:
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	// Returns:
	//     Buttons text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine offset of popuped bar.
	// Parameters:
	//     rc        - Control's bounding rectangle.
	//     pControl  - Points to a CXTPControl object
	//     bVertical - TRUE if control docked vertically.
	//-----------------------------------------------------------------------
	virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a command bar's separator.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     pBar     - Points to a CXTPCommandBar object
	//     pControl - Points to a CXTPControl object
	//     bDraw    - TRUE to draw; FALSE to retrieve the size of the separator.
	// Returns:
	//     This method is called to draw a command bar's separator.
	//-----------------------------------------------------------------------
	virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw);

	virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar);

protected:

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the base theme of the manager.
	// Returns:
	//     Returns the base theme.
	//-----------------------------------------------------------------------
	virtual XTPPaintTheme BaseTheme() { return xtpThemeVisualStudio2010; }


public:
	CXTPPaintManagerColor m_clrMenuSeparator;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSVISUALSTUDIO2010THEME_H__)
