// XTPColorPicker.h : interface for the CXTPColorPicker class.
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
#if !defined(__XTPCOLORPICKER_H__)
#define __XTPCOLORPICKER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

// class forwards
class CXTPColorPopup;
struct XTP_PICK_BUTTON;

//===========================================================================
// Summary:
//     CXTPColorPicker is a CXTPButton derived class. It displays a color picker
//     popup window (CXTPColorPopup) for Office(tm) style color selection.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorPicker : public CXTPButton
{
	DECLARE_DYNAMIC(CXTPColorPicker)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorPicker object
	//-----------------------------------------------------------------------
	CXTPColorPicker();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorPicker object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPColorPicker();

public:
    //-----------------------------------------------------------------------
    // Summary:    Called to initialize the theme colors for the color 
	//             selector.  The colors passed in must be an array of 10
	//             colors.  Additional values can be passed in such as an array
	//             of 10 string resource IDs and a 6 x 10 shading matrix
	//             used to adjust each of the 10 theme colors passed in.
    // Parameters: pColors       - Array of colors and tooltips.
	//             nCount        - Number of colors in the array.
	//             lpszTitle     - Title used for theme colors caption.
	//             pStringIDs    - Array of resource IDs representing each color's 
	//                             description, if NULL default values are used. Array
	//                             size must match nCount.
	//             ppszStrings   - Array of strings representing each color's 
	//                             description, values cannot be NULL. Array
	//                             size must match nCount.
	//             ppShadeMatrix - 6 x 10 matrix array that specifies 6 rows of
	//                             shading by 10 colums.  If NULL default values
	//                             are used.
    // Returns: TRUE if successful, otherwise returns FALSE.
	// Example: Here is an example of how use SetThemeColors.
	// <code>
	// XTP_PICK_BUTTON _ThemeColors[] =
	// {
	//     { RGB(255, 255, 255), XTP_IDS_CLR_WHITE    },
	//     { RGB(  0,   0,   0), XTP_IDS_CLR_BLACK    },
	//     { RGB(231, 230, 230), XTP_IDS_CLR_GRAY25   },
	//     { RGB( 68,  84, 106), XTP_IDS_CLR_BLUEGRAY },
	//     { RGB( 91, 155, 213), XTP_IDS_CLR_BLUE     },
	//     { RGB(237, 125,  49), XTP_IDS_CLR_ORANGE   },
	//     { RGB(165, 165, 165), XTP_IDS_CLR_GRAY50   },
	//     { RGB(255, 192,   0), XTP_IDS_CLR_GOLD     },
	//     { RGB( 68, 114, 196), XTP_IDS_CLR_BLUE     },
	//     { RGB(112, 173,  71), XTP_IDS_CLR_GREEN    },
	// };
    //
    // UINT _StringIDs[10] = 
    // {
    //     XTP_IDS_BACKGROUND1,
    //     XTP_IDS_TEXT1,
    //     XTP_IDS_BACKGROUND2,
    //     XTP_IDS_TEXT2,
    //     XTP_IDS_ACCENT1, 
    //     XTP_IDS_ACCENT2,
    //     XTP_IDS_ACCENT3,
    //     XTP_IDS_ACCENT4,
    //     XTP_IDS_ACCENT5,
    //     XTP_IDS_ACCENT6,
    // };
    //
    // int _Shade[6][10] = 
    // {
    //     {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    //     { - 5, +50, -10, +80, +80, +80, +80, +80, +80, +80 },
    //     { -15, +35, -25, +60, +60, +60, +60, +60, +60, +60 },
    //     { -25, +25, -50, +40, +40, +40, +40, +40, +40, +40 },
    //     { -35, +15, -75, -25, -25, -25, -25, -25, -25, -25 },
    //     { -50, + 5, -90, -50, -50, -50, -50, -50, -50, -50 },
    // };
    //
	// SetThemeColors(_ThemeColors, _countof(_ThemeColors), _StringIDs, _Shade);
	// </code>
    //-----------------------------------------------------------------------
	BOOL SetThemeColors(XTP_PICK_BUTTON* pColors = NULL, int nCount = 10, LPCTSTR lpszTitle = NULL, UINT* pStringIDs = NULL, int ppShadeMatrix[6][10] = NULL);
	BOOL SetThemeColors(XTP_PICK_BUTTON* pColors, int nCount, LPCTSTR lpszTitle, LPCTSTR* ppszStrings, int ppShadeMatrix[6][10]);

    //-----------------------------------------------------------------------
    // Summary:    Called to initialize the standard colors for the color 
	//             selector.  The colors passed in must be an array of 10
	//             colors.
    // Parameters: pColors   - Array of colors and tooltips.
	//             nCount    - Number of colors in the array.
	//             nCols     - Number of columns to display.
	//             lpszTitle - Title used for theme colors caption.
    // Returns: TRUE if successful, otherwise returns FALSE.
	// Example: Here is an example of how use SetThemeColors.
	// <code>
	// XTP_PICK_BUTTON _StdColors[] =
	// {
	//     { RGB(192,   0,   0), XTP_IDS_CLR_DARK_RED    },
	//     { RGB(255,   0,   0), XTP_IDS_CLR_RED         },
	//     { RGB(255, 192,   0), XTP_IDS_CLR_ORANGE      },
	//     { RGB(255, 255,   0), XTP_IDS_CLR_YELLOW      },
	//     { RGB(146, 208,  80), XTP_IDS_CLR_LIGHT_GREEN },
	//     { RGB(  0, 176,  80), XTP_IDS_CLR_GREEN       },
	//     { RGB(  0, 176, 240), XTP_IDS_CLR_LIGHT_BLUE  },
	//     { RGB(  0, 112, 192), XTP_IDS_CLR_BLUE        },
	//     { RGB(  0,  32,  96), XTP_IDS_CLR_DARK_BLUE   },
	//     { RGB(112,  48, 160), XTP_IDS_CLR_PURPLE      },
	// };
	//
	// SetStandardColors(_StdColors, _countof(_StdColors));
	// </code>
    //-----------------------------------------------------------------------
	BOOL SetStandardColors(XTP_PICK_BUTTON* pColors = NULL, int nCount = 10, int nCols = 10, LPCTSTR lpszTitle = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will set the color value for the color
	//     picker.
	// Parameters:
	//     clr - An RGB value that represents the new RGB color for the
	//           control.
	//-----------------------------------------------------------------------
	virtual void SetColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will return the currently selected color.
	// Returns:
	//     An RGB color value that represents the currently selected color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetColor() const;

	// -------------------------------------------------------------------
	// Summary:
	//     This member function will display any text associated with the
	//     color picker button control. If the button has text, it will be
	//     displayed.
	// Parameters:
	//     bShowText -  TRUE to display button text.
	// -------------------------------------------------------------------
	virtual void ShowText(BOOL bShowText);

	// --------------------------------------------------------------------
	// Summary:
	//     This member function will determine if text is displayed for the
	//     color picker button control.
	// Returns:
	//     TRUE if the color picker displays text, otherwise returns FALSE.
	// --------------------------------------------------------------------
	virtual BOOL IsTextShown() const;

	// ------------------------------------------------------------------
	// Summary:
	//     This member function will display any text associated with the
	//     color picker button control in color.
	// Parameters:
	//     bColorText -  TRUE to display the button text in color.
	// ------------------------------------------------------------------
	virtual void ShowTextInColor(BOOL bColorText);

	// -----------------------------------------------------------------
	// Summary:
	//     This member function will determine if the text for the color
	//     picker button control is displayed in color.
	// Returns:
	//     TRUE if the button text is in color, otherwise returns FALSE.
	// -----------------------------------------------------------------
	virtual BOOL IsTextInColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will set the default RGB color value for
	//     the color picker.
	// Parameters:
	//     clrDefault - An RGB value that represents the default color
	//     for the color picker.
	//-----------------------------------------------------------------------
	virtual void SetDefaultColor(COLORREF clrDefault);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will return the default RGB color value for
	//     the color picker.
	// Returns:
	//     An RGB value that represents the default color value for the
	//     color picker.
	//-----------------------------------------------------------------------
	virtual COLORREF GetDefaultColor() const;

	// ------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to modify the color picker display
	//     style.
	// Parameters:
	//     dwRemove -  Specifies the window styles to be removed during style
	//                 modification.
	//     dwAdd -     Specifies the window styles to be added during style modification.
	// Remarks:
	//     Styles to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:
	//     * <b>CPS_XTP_NOFILL</b> Displays a "No Fill" button rather than the default Automatic Color button.
	//     * <b>CPS_XTP_EXTENDED</b> Displays 40 extended colors rather than the default 16 colors.
	//     * <b>CPS_XTP_MORECOLORS</b> Displays a "More Colors" button which will display a CXTPColorDialog.
	//     * <b>CPS_XTP_NOAUTOMATIC</b> Do not display an "Automatic Color" button.
	//     * <b>CPS_XTP_RECENTCOLORS</b> Displays a recently used color list for the control.
	//     * <b>CPS_XTP_RIGHTALIGN</b> The popup window is displayed right aligned.
	//     * <b>CPS_XTP_COLORBORDERS</b> The popop window displays color boxes with color borders.
	//     * <b>CPS_XTP_SHOW3DSELECTION</b> Displays the color selection box with a 3D raised border in CXTPColorDialog.
	//     * <b>CPS_XTP_SHOWHEXVALUE</b> Displays the hex equivalent of the selected color.
	//     * <b>CPS_XTP_SHOWEYEDROPPER</b> Displays the eye dropper box to allow selecting any color on the screen.
	// See Also:
	//     CXTPColorPopup::Create, CXTPColorSelectorCtrl::Create
	// ------------------------------------------------------------------------------
	virtual void ModifyCPStyle(DWORD dwRemove, DWORD dwAdd);

	// -------------------------------------------------------------------
	// Summary:
	//     This member function will activate the color picker and display
	//     the color popup window.
	// -------------------------------------------------------------------
	virtual void ShowPopupWindow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom colors for color picker
	// Parameters:
	//     pColors   - Static array of colors and tooltips
	//     nCount    - Number of colors
	//     nCols     - Number of columns to show
	//     lpszTitle - Custom title for colors displayed.
	//-----------------------------------------------------------------------
	void SetColors(XTP_PICK_BUTTON* pColors, int nCount, int nCols = 8, LPCTSTR lpszTitle = NULL);

    //-----------------------------------------------------------------------
    // Summary: Called to retrieve a reference to the theme color button array.
    // Returns: Reference to a CXTPPickBtnArray object representing the theme
	//          color array for the color selector control.
    //-----------------------------------------------------------------------
	CXTPPickBtnArray& GetThemeColors();

    //-----------------------------------------------------------------------
    // Summary: Called to retrieve a reference to the standard color button array.
    // Returns: Reference to a CXTPPickBtnArray object representing the standard
	//          color array for the color selector control.
    //-----------------------------------------------------------------------
	CXTPPickBtnArray& GetStandardColors();

    //-----------------------------------------------------------------------
    // Summary:    Call to hide or show captions for the color selector.
    // Parameters: bShow - TRUE to show captions, FALSE to hide.
    //-----------------------------------------------------------------------
	void ShowCaptions(BOOL bShow);

protected:

	// ------------------------------------------------------------------------
	// Summary:
	//     Retrieves the background contrast color for the CXTPColorPicker.
	// Returns:
	//     An RGB value either BLACK or WHITE.
	// Remarks:
	//     This member function is called to return the contrast color for the
	//     color picker button. The contrast color is used when text is
	//     displayed to contrast with the currently selected color. For
	//     \example, if the currently selected color is WHITE, then BLACK would
	//     be returned so the text is legible.
	// ------------------------------------------------------------------------
	virtual COLORREF GetContrastColor() const;

protected:
	virtual void DoDraw(CDC* pDC);
	virtual void DrawColorPicker(CDC* pDC);

public:
	virtual BOOL IsPushed();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorPicker)
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorPicker)
	afx_msg LRESULT OnXtpSelChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXtpDropDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXtpCloseUp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXtpSelEndOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXtpSelEndCancel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXtpSelNoFill(WPARAM wParam, LPARAM lParam);
	afx_msg void OnXtpShowPopup();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL     m_bShowText;        // TRUE if the color picker displays text.
	BOOL     m_bColorText;       // TRUE if the color picker text is colorized; only valid if m_bShowText is TRUE.
	BOOL     m_bPopupActive;     // TRUE if the popup window is currently active.
	DWORD    m_dwPopup;          // Represents the style for the color picker.
	COLORREF m_clrSelected;      // An RGB value that represents the color for the display color.
	COLORREF m_clrDefault;       // An RGB value that represents the color for the default color.
	CString  m_strTitleTheme;    // Represents the title for theme colors caption.
	CString  m_strTitleStandard; // Represents the title for standard colors caption.
	CString  m_strTitleCustom;   // Custom colors title.

	BOOL              m_bCaptions;        // TRUE to display color captions.
	CXTPPickBtnArray  m_arThemeColors;    // Theme color array.
	CXTPPickBtnArray  m_arStandardColors; // Standard color array.
	XTP_PICK_BUTTON*  m_pExtendedColors;  // Custom colors
	int               m_nExtendedColors;  // Custom colors count
	int               m_nCols;            // Number of columns in the color popup window.

	friend class CXTPColorPickerTheme;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTPColorPicker::SetColors(XTP_PICK_BUTTON* pColors, int nCount, int nCols, LPCTSTR lpszTitle /*=NULL*/) {
	m_pExtendedColors = pColors; m_nExtendedColors = nCount; m_nCols = nCols; m_strTitleCustom = lpszTitle;
}
AFX_INLINE COLORREF CXTPColorPicker::GetColor() const {
	return m_clrSelected;
}
AFX_INLINE void CXTPColorPicker::ShowText(BOOL bShowText) {
	m_bShowText = bShowText; RedrawWindow();
}
AFX_INLINE BOOL CXTPColorPicker::IsTextShown() const {
	return m_bShowText;
}
AFX_INLINE void CXTPColorPicker::ShowTextInColor(BOOL bColorText) {
	m_bColorText = bColorText; RedrawWindow();
}
AFX_INLINE BOOL CXTPColorPicker::IsTextInColor() const {
	return m_bColorText;
}
AFX_INLINE void CXTPColorPicker::SetDefaultColor(COLORREF clrDefault) {
	m_clrDefault = clrDefault; RedrawWindow();
}
AFX_INLINE COLORREF CXTPColorPicker::GetDefaultColor() const {
	return m_clrDefault;
}
AFX_INLINE void CXTPColorPicker::ModifyCPStyle(DWORD dwRemove, DWORD dwAdd) {
	m_dwPopup = (m_dwPopup & ~dwRemove) | dwAdd; if (::IsWindow(m_hWnd)) RedrawWindow();
}
AFX_INLINE CXTPPickBtnArray& CXTPColorPicker::GetThemeColors() {
	return m_arThemeColors;
}
AFX_INLINE CXTPPickBtnArray& CXTPColorPicker::GetStandardColors() {
	return m_arStandardColors;
}
AFX_INLINE void CXTPColorPicker::ShowCaptions(BOOL bShow) {
	m_bCaptions = bShow;
}

// ------------------------------------------------------------------------
// Summary:
//     The DDX_XTPColorPicker function manages the transfer of
//     integer data between a color picker control in a dialog box, form
//     view, or control view object and a COLORREF data member of the
//     dialog box, form view, or control view object.
//
//     When DDX_XTPColorPicker is called, <i>'value'</i> is set to
//     the current state of the color picker control.
// Parameters:
//     pDX -    A pointer to a CDataExchange object. The framework supplies
//              this object to establish the context of the data exchange,
//              including its direction.
//     nIDC -   The resource ID of the color picker control associated with
//              the control property.
//     value -  A reference to a member variable of the dialog box, form view,
//              or control view object with which data is exchanged.
// See Also:
//     CXTPColorPicker
// ------------------------------------------------------------------------
_XTP_EXT_CLASS void AFXAPI DDX_XTPColorPicker(CDataExchange* pDX, int nIDC, COLORREF& value);


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPCOLORPICKER_H__)
