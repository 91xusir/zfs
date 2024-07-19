// XTPComboBoxTheme.h: interface for the CXTPComboBoxTheme class.
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
#if !defined(__XTPCOMBOBOXTHEME_H__)
#define __XTPCOMBOBOXTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPWinThemeWrapper;
class CXTPComboBox;

//===========================================================================
// Summary: CXTPComboBoxTheme is a class used to perform Button Theme
//          drawing tasks.  This is the base class for all button themes.
//===========================================================================
class _XTP_EXT_CLASS CXTPComboBoxTheme : public CXTPControlTheme
{
public:

	//-----------------------------------------------------------------------
	// Summary: Default constructor
	//-----------------------------------------------------------------------
	CXTPComboBoxTheme();

	//-----------------------------------------------------------------------
	// Summary: Destructor handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPComboBoxTheme();

	//-----------------------------------------------------------------------
	// Summary: Called by the combo-box to fill a rectangle using the brush
	//          specifed by hBrush.
	// Input:   hdc    - Handle to a valid device context.
	//          x      - Left side of the rectangle to draw.
	//          y      - Top side of the rectangle to draw.
	//          cx     - Width of the rectangle to draw.
	//          cy     - Height of the rectangle to draw.
	//          hBrush - Handle to a HBRUSH object representing the background
	//                   fill color.
	//-----------------------------------------------------------------------
	virtual void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, HBRUSH hBrush);

	//-----------------------------------------------------------------------
	// Summary: Called by the combo-box to draw a frame rectangle using the
	//          brush specified by hBrush.
	// Input:   hdc    - Handle to a valid device context.
	//          lprc   - Pointer to RECT structure representing the size of
	//                   rectangle to draw.
	//          nSize  - Width in pixels of the rectangle borders.
	//          hBrush - Handle to a HBRUSH object representing the border
	//                   fill color.
	//-----------------------------------------------------------------------
	virtual void DrawFrame(HDC hdc, LPRECT lprc, int nSize, HBRUSH hBrush);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to draw the combo-box control.
	// Input:   pDC       - Points to a valid device context.
	//          pComboBox - Points to the CXTPComboBox object to be drawn.
	//-----------------------------------------------------------------------
	virtual BOOL DrawComboBox(CDC* pDC, CXTPComboBox* pComboBox);

	//-----------------------------------------------------------------------
	// Summary: This member function is called to draw the combo-box background.
	// Input:   pDC       : Points to a valid device context.
	//          pComboBox : Points to the combo-box to be drawn.
	//-----------------------------------------------------------------------
	virtual void DrawBackground(CDC *pDC, CXTPComboBox* pComboBox);

	//-----------------------------------------------------------------------
	// Summary: Called by the paint manager to draw the button portion of the
	//          combo-box control.
	// Input:   pDC       : Points to the device context for the combo-box.
	//          pComboBox : Points to a CXTPComboBox object.
	//          rcBtn     : Size of the button to draw.
	//-----------------------------------------------------------------------
	virtual void DrawButton(CDC *pDC, CXTPComboBox* pComboBox, CRect rcBtn);

	//-----------------------------------------------------------------------
	// Summary: Called by the paint manager to draw the drop down arrow for
	//          the combo-box drop button.
	// Input:   pDC       : Points to the device context for the combo-box.
	//          pComboBox : Points to a CXTPComboBox object.
	//          rcBtn     : Size of the button to draw.
	//-----------------------------------------------------------------------
	virtual void DrawButtonTriangle(CDC *pDC, CXTPComboBox* pComboBox, CRect rcBtn);

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to refresh
	//          the visual styles used by each components theme.
	// Input:   pComboBox - Points to a CXTPComboBox object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPComboBox* pComboBox);

	//-----------------------------------------------------------------------
	// Summary:  Call this member function to retrieve the background color
	//           for the CXTPComboBox.
	// Returns:  An RGB color value representing the background color for
	//           the CXTPComboBox.
	// See Also: SetTextColor, ResetTextColor, SetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const;

	//-----------------------------------------------------------------------
	// Summary:  Call this member function to set the background color
	//           for the CXTPComboBox.
	// Returns:  An RGB color value representing the old background color
	//           value for the CXTPComboBox.
	// See Also: SetTextColor, ResetTextColor, GetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	COLORREF SetBackColor(COLORREF clrBackground);

	//-----------------------------------------------------------------------
	// Summary:  Call this member function to reset the background color
	//           for the CXTPComboBox.
	// See Also: GetTextColor, SetTextColor, GetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	void ResetBackColor();

	//-----------------------------------------------------------------------
	// Summary:  Call this member function to retrieve the text color
	//           for the CXTPComboBox.
	// Returns:  An RGB color value representing the text color for
	//           the CXTPComboBox.
	// See Also: SetTextColor, ResetTextColor, SetBackColor, GetBackColor
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;

	//-----------------------------------------------------------------------
	// Summary:  Call this member function to set the text color
	//           for the CXTPComboBox.
	// Returns:  An RGB color value representing the old text color for
	//           the CXTPComboBox.
	// See Also: GetTextColor, ResetTextColor, SetBackColor, GetBackColor
	//-----------------------------------------------------------------------
	COLORREF SetTextColor(COLORREF clrText);

	//-----------------------------------------------------------------------
	// Summary:   Call this member function to reset the text color
	//            for the CXTPComboBox.
	// See Also : GetTextColor, SetTextColor, SetBackColor, GetBackColor
	//-----------------------------------------------------------------------
	void ResetTextColor();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve a reference to a CBrush
	//          object used to draw the combo-box background.
	// Returns: A reference to a CBrush object used to draw the combo-box
	//          background.
	//-----------------------------------------------------------------------
	const CBrush &GetBackgroundBrush() const;

protected:

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to update the color value used by
	//          the combo-box background brush.
	//-----------------------------------------------------------------------
	void UpdateBackgroundBrush();

	int                           m_nThumbWidth;           // Size in pixels of the drop button width.
	CXTPBrush                     m_xtpBrushBackground;    // Brush used by the combo-box to fill the background color.
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CBrush, m_brBackground, m_xtpBrushBackground, GetBackgroundBrushHandle);
	CXTPWinThemeWrapper*          m_themeComboBox;         // Windows API wrapper used to draw windows theme.

public:
	
	CXTPPaintManagerColor         m_clrBorderDisabled;     // RGB value used to draw the disabled border color.
	CXTPPaintManagerColor         m_clrBorderHot;          // RGB value used to draw the hilighted border color.
	CXTPPaintManagerColor         m_clrBorderNormal;       // RGB value used to draw the normal border color.  
	CXTPPaintManagerColor         m_clrButtonPressed;      // RGB value used to draw the pressed button color.
	CXTPPaintManagerColor         m_clrButtonHot;          // RGB value used to draw the highlighted button color.
	CXTPPaintManagerColor         m_clrButtonNormal;       // RGB value used to draw the normal button color.
	CXTPPaintManagerColor         m_clrButtonArrowPressed; // RGB value used to draw the pressed button arrow color.
	CXTPPaintManagerColor         m_clrButtonArrowHot;     // RGB value used to draw the highlighted button arrow color.
	CXTPPaintManagerColor         m_clrButtonArrowNormal;  // RGB value used to draw the normal button arrow color.
	CXTPPaintManagerColor         m_clrBackground;         // RGB color value representing background color.
	CXTPPaintManagerColor         m_clrBackDisabled;       // RGB color value representing disabled background color.
	CXTPPaintManagerColor         m_clrBackHighlight;      // RGB color value representing highlight background color.
	CXTPPaintManagerColor         m_clrText;               // RGB color value representing text color.
	CXTPPaintManagerColor         m_clrTextDisabled;       // RGB color value representing disabled text color.
	CXTPPaintManagerColor         m_clrTextHighlight;      // RGB color value representing highlight text color.
	CXTPPaintManagerColorGradient m_grcButton;             // RGB value used to draw the button gradient color.
	CXTPPaintManagerColorGradient m_grcButtonHilite;       // RGB value used to draw the button highlighted gradient color.
	CXTPPaintManagerColorGradient m_grcButtonPushed;       // RGB value used to draw the button pushed gradient color.

private:

	friend class CXTPComboBox;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CXTPComboBoxTheme::GetBackColor() const {
	return m_clrBackground;
}
AFX_INLINE COLORREF CXTPComboBoxTheme::SetBackColor(COLORREF clrBackground) {
	COLORREF clrOldBackground = m_clrBackground; m_clrBackground.SetCustomValue(clrBackground); return clrOldBackground;
}
AFX_INLINE void CXTPComboBoxTheme::ResetBackColor() {
	m_clrBackground.SetDefaultValue();
}
AFX_INLINE COLORREF CXTPComboBoxTheme::GetTextColor() const {
	return m_clrText;
}
AFX_INLINE COLORREF CXTPComboBoxTheme::SetTextColor(COLORREF clrText) {
	COLORREF clrOldText = m_clrText; m_clrText.SetCustomValue(clrText); return clrOldText;
}
AFX_INLINE void CXTPComboBoxTheme::ResetTextColor() {
	m_clrText.SetDefaultValue();
}
AFX_INLINE const CBrush & CXTPComboBoxTheme::GetBackgroundBrush() const {
	return m_xtpBrushBackground;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // __XTPCOMBOBOXTHEME_H__
