// XTPCommandBarsOffice2013Theme.h : interface for the CXTPCommandBarsOffice2013Theme class.
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
#if !defined(__XTPCOMMANDBARSOFFICE2013THEME_H__)
#define __XTPCOMMANDBARSOFFICE2013THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

enum XTPBackImage
{
	XTPNone,
	XTPCalligraphy,
	XTPCirclesAndStripes,
	XTPCircuit,
	XTPClouds,
	XTPDoodleCircles,
	XTPDoodleDiamonds,
	XTPGeometry,
	XTPLunchBox,
	XTPSchoolSupplies,
	XTPSpring,
	XTPStars,
	XTPStraws,
	XTPTreeRings,
	XTPUnderWater,
};

//===========================================================================
// Summary:
//     The CXTPCommandBarsOffice2013Theme class is used to enable an Office 2013 style theme for Command Bars
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarsOffice2013Theme : public CXTPCommandBarsModernUITheme
{
	DECLARE_DYNAMIC(CXTPCommandBarsOffice2013Theme)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarsOffice2013Theme object.
	//-----------------------------------------------------------------------
	CXTPCommandBarsOffice2013Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBarsOffice2013Theme object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPCommandBarsOffice2013Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the base theme of the manager.
	// Returns:
	//     Returns the base theme.
	//-----------------------------------------------------------------------
	virtual XTPPaintTheme BaseTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the control's face
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object to draw.
	// Returns:
	//     TRUE if the rectange was drawn, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DrawRectangle(CDC* pDC, CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the command bar's face.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPCommandBar object to draw.
	//-----------------------------------------------------------------------
	virtual void FillCommandBarEntry(CDC *pDC, CXTPCommandBar *pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill a dockbar.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPDockBar object.
	//-----------------------------------------------------------------------
	virtual void FillDockBar(CDC *pDC, CXTPDockBar *pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a command bar's group separator.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     pBar     - Points to a CXTPCommandBar object
	//     pLeftControl - Points to a left CXTPControl object
	//     pRightControl - Points to a left CXTPControl object
	//     bDraw    - TRUE to draw; FALSE to retrieve the size of the separator.
	// Returns:
	//     This method is called to draw a command bar's separator.
	//-----------------------------------------------------------------------
	virtual void DrawCommandBarGroupSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pLeftControl, CXTPControl* pRightControl, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC   - Pointer to a valid device context
	//     bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	//     pEdit - Edit control to draw.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit* pEdit, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws workspace space
	// Parameters:
	//     pDC - Pointer to device context
	//     rc - Bounding rectangle
	//     rcExclude - Excluded rectangle
	//-----------------------------------------------------------------------
	virtual void FillWorkspace(CDC *pDC, CRect rc, CRect rcExclude);

	//-----------------------------------------------------------------------
	// Summary:
	//     Used by the paint manager to determine if the theme requires a 
	//     resource DLL to load image and color information.
	// Returns:
	//     Returns TRUE if the theme requires a resource DLL, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL RequiresResourceImages();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method returns the edit control back color to draw.
	// Parameters:
	//     pControl - Points to a CXTPControl object to get the back color from.
	// Returns: Edit control back color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetControlEditBackColor(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the background for a Gallery menu item.
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     rc          - Rectangle to draw.
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     barType     - Parent's bar type
	//-----------------------------------------------------------------------
	virtual void DrawGalleryItemBack(CDC* pDC, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked, XTPBarType barType);

	virtual BOOL SetOfficeBackImage(CXTPCommandBars* pCommandBars, XTPBackImage nBackImage = XTPNone);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Called to update the standard colors used by the framework.
	// See Also:
	//     CXTPPaintManager::GetXtremeColor(UINT nIndex)
	//-----------------------------------------------------------------------
	virtual void RefreshStandardColors();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves buttons text color
	// Parameters:
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	//-----------------------------------------------------------------------
	virtual COLORREF GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);
	virtual COLORREF GetControlTextColor(CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the control's face
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     rc          - Rectangle to draw.
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	//-----------------------------------------------------------------------
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition, int nID); // <combine CXTPCommandBarsOffice2013Theme::DrawRectangle@CDC*@CRect@BOOL@BOOL@BOOL@BOOL @BOOL@XTPBarType@XTPBarPosition>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill boreder the control's face
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     rc          - Rectangle to draw.
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	//-----------------------------------------------------------------------
	void DrawFrameBorder(CDC* pDC, CRect rc, int szBorderThickness, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw popup bar right gripper.
	// Parameters:
	//     pDC       - Points to a valid device context.
	//     xPos      - Specifies the logical x coordinate of the upper-left corner of the rectangle.
	//     yPos      - Specifies the logical y coordinate of the upper-left corner of the destination rectangle.
	//     cx        - Specifies the width of the rectangle.
	//     cy        - Specifies the height of the rectangle.
	//     bExpanded - TRUE if expanded.gripper.
	//-----------------------------------------------------------------------
	virtual void DrawPopupBarGripper(CDC* pDC, int xPos, int yPos, int cx, int cy, BOOL bExpanded = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the command bar's gripper.
	// Parameters:
	//     pDC   - Pointer to a valid device context
	//     pBar  - Points to a CXTPCommandBar object
	//     bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
	// Returns:
	//     Size of the gripper.
	//-----------------------------------------------------------------------
	virtual CSize DrawCommandBarGripper(CDC *pDC, CXTPCommandBar *pBar, BOOL bDraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     bDraw  - TRUE to draw; FALSE to retrieve the size of the control.
	//     pComboBox - CXTPControlComboBox pointer need to draw.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pComboBox, BOOL bDraw);

	virtual COLORREF GetGlyphColor(CXTPControl* pControl);

	virtual COLORREF GetSeparatorColor(CXTPControl* pControl, BOOL bDark);

	virtual void DrawControlPopupGlyph(CDC* pDC, CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw a split button expand symbol
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     pControl    - Points to a CXTPControl object
	//     pt          - Position of expand symbol
	//     bSelected   - TRUE if the control is selected.
	//     bPopuped    - TRUE if the control is popuped.
	//     bEnabled    - TRUE if the control is enabled.
	//     bVert       - TRUE if control is vertical
	//-----------------------------------------------------------------------
	virtual void DrawDropDownGlyph(CDC* pDC, CXTPControl* pControl, CPoint pt, BOOL bSelected, BOOL bPopuped, BOOL bEnabled, BOOL bVert);

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
	virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw expand or hidden glyphs for a button.
	// Parameters:
	//     pDC           - Output device context pointer.
	//     pButton       - Target button pointer.
	//     pt0, pt1, pt2 - Expand glyph triangle coordinates.
	//     clr           - Glyph color.
	//     bVertical     - TRUE if vertically positioned glyph is to be drawn.
	//----------------------------------------------------------------------
	virtual void DrawExpandGlyph(CDC* pDC, CXTPControl* pButton, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clr);
	virtual void DrawHiddenGlyph(CDC* pDC, CXTPControl* pButton, COLORREF clr, BOOL bVertical); // <combine CXTPPaintManager::DrawExpandGlyph@CDC*@CXTPControl*@CPoint@CPoint@CPoint@COLORREF>
	virtual void DrawExpandGlyph(CDC* pDC, CXTPControl* pButton, COLORREF clr, BOOL bVertical); // <combine CXTPPaintManager::DrawExpandGlyph@CDC*@CXTPControl*@CPoint@CPoint@CPoint@COLORREF>

	virtual void DrawCheckMark(CDC* pDC, CRect rcCheck, BOOL bIsRadio, BOOL bEnabled, BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw check box mark area
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pButton - Pointer to the checkbox control
	//     rc - Bounding rectangle of the check box itself (not control)
	//     bDraw - TRUE to draw; FALSE to find size
	// Returns: Size of check box mark
	//-----------------------------------------------------------------------
	virtual CSize DrawControlCheckBoxMark(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw check box mark area
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc - Bounding rectangle
	//     bDraw - TRUE to draw; FALSE to find size
	//     bSelected - TRUE if control is selected
	//     bPressed - TRUE if control is pressed
	//     bChecked - TRUE if control is checked
	//     bEnabled - TRUE if control is enabled
	// Returns: Size of check box mark
	//-----------------------------------------------------------------------
	virtual CSize DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw radio button mark area
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc - Bounding rectangle
	//     bDraw - TRUE to draw; FALSE to find size
	//     bSelected - TRUE if control is selected
	//     bPressed - TRUE if control is pressed
	//     bChecked - TRUE if control is checked
	//     bEnabled - TRUE if control is enabled
	// Returns: Size of radio button mark
	//-----------------------------------------------------------------------
	virtual CSize DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the frame for the SplitButton
	//     control
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object
	//     rc      - Bounding rectangle to draw
	//-----------------------------------------------------------------------
	virtual void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rc);

	CXTPColorSetStateButton* GetButtonColorSet(XTPBarType barType, BOOL bTextColor = FALSE);

	virtual void GetCheckMarkColors(COLORREF& clrMark, COLORREF& clrBack, COLORREF& clrBorder, BOOL bSelected, BOOL bPressed, BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw Menu MDI buttons
	// Parameters:
	//     pDC        - Pointer to a valid device context
	//     pButton    - Button to draw
	//-----------------------------------------------------------------------
	virtual void DrawControlMDIButton(CDC* pDC, CXTPControlButton* pButton);

	virtual void DrawDashedLine(CDC *pDC, int x, int y, int z, COLORREF clr, BOOL bHorzBar);

    BOOL                          m_bIntersect;        // TRUE to draw popup bar intersect rectangle.
    CXTPColorSetStateMenu         m_csMenu;            // State colors used by Menu items.
    CXTPColorSetStateRibbon       m_csRibbon;          // State colors used by Ribbon group buttons.
    CXTPColorSetStateToolbar      m_csToolbar;         // State colors used by Toolbar buttons.
	CXTPColorSetStateCheckMark    m_csCheckMark;       // State colors used by Menu Check Marks.
	CXTPColorSetStateComboBox     m_csComboBox;        // State colors used by ComboBox control.
	CXTPColorSetStateCheckBox     m_csCheckBox;        // State colors used by Check Box and Radio Button controls.
	CXTPPaintManagerColor         m_clrBack;           // CommandBars background color;
	CXTPPaintManagerColor         m_clrGlyph;          // Glyph Normal color.
	CXTPPaintManagerColor         m_clrGlyphHighlight; // Glyph Highlight color.
	CXTPPaintManagerColor         m_clrGlyphPushed;    // Glyph Pushed color.
	CXTPPaintManagerColor         m_clrGlyphDisabled;  // Glyph Disabled color.
    CXTPPaintManagerColor         m_clrMessageBarText; // Message bar text color.
	CXTPPaintManagerColor         m_clrBtnFace;        // Button background color.
	CXTPPaintManagerColor         m_clr3DShadow;       // Button shadow color.
	CXTPPaintManagerColor         m_clrHighlight;      // Button highlight color.
	CXTPPaintManagerColor         m_clrCaptionButtonFill;      // Text color for frame caption buttons.
	CXTPPaintManagerColorGradient m_clrWorkspace;      // Workspace area gradient color.

	COLORREF m_clrQuickAccess;
	COLORREF m_clrQuickAccessDisabled;
	COLORREF m_clrQuickAccessButtonHotBack;
	COLORREF m_clrQuickAccessButtonPressedBack;
};

_XTP_EXT_CLASS BOOL AFX_CDECL XTPSetOfficeSetBackImage(CXTPCommandBars* pCommandBars, XTPBackImage nBackImage = XTPNone);

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE XTPPaintTheme CXTPCommandBarsOffice2013Theme::BaseTheme() {
	return xtpThemeOffice2013;
}
AFX_INLINE BOOL CXTPCommandBarsOffice2013Theme::RequiresResourceImages() {
	return TRUE;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSOFFICE2013THEME_H__)
