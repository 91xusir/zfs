// XTPColorSelectorCtrl.h : interface for the CXTPColorSelectorCtrl class.
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
#if !defined(__XTPCOLORSELECTORCTRL_H__)
#define __XTPCOLORSELECTORCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPColorDialog;
class CXTPColorSelectorCtrlTheme;

//===========================================================================
// Summary: XTP_PICK_BUTTON structure is used by the CXTPColorSelectorCtrl
//          class to support a user defined color list.
// See Also: CXTPColorSelectorCtrl
//===========================================================================
struct _XTP_EXT_CLASS XTP_PICK_BUTTON
{
	COLORREF clrButton;  // RGB value representing the button color.
	UINT     toolTipID;  // Resource ID for the tooltip for the color button.
	TCHAR    szTip[256]; // Tooltip string for the color button, if defined toolTipID must be 0.
};

//===========================================================================
// Summary: CXTPPickBtnArray is used to manage a collection of XTP_PICK_BUTTON
//          objects.
//===========================================================================
typedef CArray<XTP_PICK_BUTTON, XTP_PICK_BUTTON&> CXTPPickBtnArray;

//===========================================================================
// Summary: CXTPColorSelectorCtrl is a CWnd derived class. It is used to create
//          a CXTPColorSelectorCtrl control that will allow a user to select colors.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrl : public CWnd
{
public:

	// ----------------------------------------------------------------------
	// Summary:  COLOR_CELL structure is used by the CXTPColorSelectorCtrl
	//           class to maintain information about a particular color cell.
	// See Also: CXTPColorSelectorCtrl
	// ----------------------------------------------------------------------
	struct _XTP_EXT_CLASS COLOR_CELL
	{
		UINT     nID;         // Command ID of the color cell.
		UINT     nIndex;      // Index of the color cell.
		bool     bChecked;    // true if the cell is checked.
		CRect    rect;        // Size of the color cell.
		CRect    rcBorder;    // Determines what borders are drawn.
		DWORD    dwStyle;     // Color picker style for the cell.
		TCHAR    szText[256]; // Tooltip text displayed for the color cell.
		COLORREF clr;         // An RGB value that represents the color of the cell.
	};

protected:

	// ----------------------------------------------------------------------
	// Summary:  List for maintaining COLOR_CELL structures.
	// Remarks:  CList definition used by the CXTPColorSelectorCtrl class to maintain
	//           a list of COLOR_CELL structures representing each color item in
	//           the color selector control.
	// See Also: CXTPColorSelectorCtrl, COLOR_CELL
	// ----------------------------------------------------------------------
	typedef CList<COLOR_CELL*, COLOR_CELL*> CColorCellList;

	// ---------------------------------------------------------------
	// Summary:  Array of listener windows to send notifications to.
	// Remarks:  CArray definition used by the CXTPColorSelectorCtrl class to
	//           maintain a list of window handles that receive notification
	//           messages that are sent when an event occurs in the control.
	// See Also: CXTPColorSelectorCtrl
	// ---------------------------------------------------------------
	typedef CArray<HWND, HWND> CListenerArray;


public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPColorSelectorCtrl object
	//-----------------------------------------------------------------------
	CXTPColorSelectorCtrl();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPColorSelectorCtrl object, handles cleanup and
	//          deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPColorSelectorCtrl();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to initialize the recently used
	//             color list for the color selector.
	// Parameters: arRecentColors - Reference to a CUIntArray object that
	//                              contains an array of COLORREF values.
	//                  lpszTitle - Title used for recent colors caption.
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetRecentColors(const CUIntArray& arRecentColors, LPCTSTR lpszTitle = NULL);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return a reference to the recently
	//          used color array.
	// Returns: A reference to a CUIintArray object that represents an array
	//          of recently used COLORREF values.
	//-----------------------------------------------------------------------
	static CUIntArray& AFX_CDECL GetRecentColors();

	//-----------------------------------------------------------------------
	// Summary:    This member function will add a color to the user defined
	//             color list.
	// Parameters: clrColor - An RGB value that represents the recently used 
	//                        color to be added to the recent color list.
	//-----------------------------------------------------------------------
	static void AFX_CDECL AddRecentColor(COLORREF clrRecent);

	//-----------------------------------------------------------------------
	// Summary: Resets the recently used color list.
	// Remarks: Call this member function to remove all colors from the 
	//          recently used color list.
	//-----------------------------------------------------------------------
	static void AFX_CDECL ResetRecentColors();

//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATE("The function is no longer supported. Use SetRecentColors instead.")
	static void AFX_CDECL SetUserColors(const CUIntArray& arUserDefColors) { SetRecentColors(arUserDefColors); }
	_XTP_DEPRECATE("The function is no longer supported. Use GetRecentColors instead.")
	static CUIntArray& AFX_CDECL GetUserColors() { return GetRecentColors(); }
//}}AFX_CODEJOCK_PRIVATE

    //-----------------------------------------------------------------------
    // Summary:    Called to create a tooltip for a theme color using the
	//             specified string resource ID and shading that were originally
	//             passed into SetThemeColors.
    // Parameters: lpszColor - Name of the color.
	//             lpszDesc  - Description of color.
	//             nShade    - Percentage the color has been lightened or darkened.
    // Returns:    A string representing the tooltip for the theme color.
    //-----------------------------------------------------------------------
	CString GetThemeTip(LPCTSTR lpszColor, LPCTSTR lpszDesc, int nShade);

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
	//             nCols         - Number of columns displayed.
	//             arColors      - Array of colors used to initialize the theme
	//                             colors.
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
	BOOL SetThemeColors(CXTPPickBtnArray& arColors, int nCols = 10, LPCTSTR lpszTitle = NULL);

    //-----------------------------------------------------------------------
    // Summary:    Called to initialize the standard colors for the color 
	//             selector.
    // Parameters: pColors   - Array of colors and tooltips.
	//             nCount    - Number of colors in the array.
	//             lpszTitle - Title used for theme colors caption.
	//             nCols     - Number of columns to show
	//             arColors  - Array of colors used to initialize the standard
	//                         colors.
    // Returns:    TRUE if successful, otherwise returns FALSE.
	// Example:    Here is an example of how use SetThemeColors.
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
	BOOL SetStandardColors(CXTPPickBtnArray& arColors, int nCols = 10, LPCTSTR lpszTitle = NULL);
	
	//-----------------------------------------------------------------------
	// Summary:    Call this method to set custom colors for color picker
	// Parameters: pColors   - Static array of colors and tooltips
	//             nCount    - Number of colors
	//             nCols     - Number of columns to show
	//             lpszTitle - Custom title for colors displayed.
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
	// Summary:    This member function will return the index of the currently selected
	//             color and will initialize 'pColorCell' struct.
	// Parameters: pColorCell - Receives a pointer to the currently selected button.
	// Returns:    The zero (0) based index of the currently selected button.
	//-----------------------------------------------------------------------
	int GetCurSel(COLOR_CELL* pColorCell);

	//-----------------------------------------------------------------------
	// Summary:    This member function will select a button based upon its index.
	// Parameters: nIndex - An integer value that represents the zero (0) based
	//             index of the button to be selected.
	//-----------------------------------------------------------------------
	void SetCurSel(int nIndex);

	// ----------------------------------------------------------------------------
	// Summary:    This member function handles the creation of the color popup
	//             window.
	// Parameters: rect       - A reference to a <i>CRect</i> object that represents the
	//                          size of the color popup window.
	//             pParentWnd - Points to the parent window for the color popup.
	//             dwPopup    - Style for the popup window. See the Remarks section for a
	//                          complete list of available styles.
	//             clrColor   - An <i>RGB</i> value that represents the currently selected
	//                          color for the popup window.
	//             clrDefault - Specifies the default color for the color popup. If the
	//                          current style includes CPS_NOFILL this parameter is
	//                          ignored.
	// Remarks:    Styles to be added or removed can be combined by using the bitwise
	//             OR (|) operator. It can be one or more of the following:
	//             * <b>CPS_XTP_NOFILL</b> Displays a "No Fill" button rather than the default Automatic Color button.
	//             * <b>CPS_XTP_EXTENDED</b> Displays 40 extended colors rather than the default 16 colors.
	//             * <b>CPS_XTP_MORECOLORS</b> Displays a "More Colors" button which will display a CXTPColorDialog.
	//             * <b>CPS_XTP_NOAUTOMATIC</b> Do not display an "Automatic Color" button.
	//             * <b>CPS_XTP_RECENTCOLORS</b> Displays a recently used color list for the control.
	//             * <b>CPS_XTP_RIGHTALIGN</b> The popup window is displayed right aligned.
	//             * <b>CPS_XTP_COLORBORDERS</b> The popop window displays color boxes with color borders.
	// Returns:    TRUE if successful, otherwise returns FALSE.
	// See Also:   CXTPColorPicker::ModifyCPStyle, CXTPColorPopup::Create
	// ----------------------------------------------------------------------------
	BOOL Create(CRect rect, CWnd* pParentWnd, DWORD dwPopup, COLORREF clrColor, COLORREF clrDefault = CLR_DEFAULT);

	//-----------------------------------------------------------------------
	// Summary:    This member function adds a window to send color picker notifications to.
	// Parameters: hwndListener - A handle to the listener window.  Messages will
	//                            be sent to it.
	//-----------------------------------------------------------------------
	void AddListener(HWND hwndListener);

	//-----------------------------------------------------------------------
	// Summary:    This member function removes a window from notification list.
	// Parameters: hwndListener - A handle to the listener window to remove.
	//-----------------------------------------------------------------------
	void RemoveListener(HWND hwndListener);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to select a color cell.
	// Parameters: pColorCell - Points to an COLOR_CELL object.
	//-----------------------------------------------------------------------
	virtual void SelectColorCell(COLOR_CELL* pColorCell);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to select a color cell.
	// Parameters: clr - Color of selected cell.
	//-----------------------------------------------------------------------
	void SelectColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to return an COLOR_CELL struct
	//             from the color cell array.
	// Parameters: iIndex - Index into the color cell array.
	// Returns:    An COLOR_CELL object.
	//-----------------------------------------------------------------------
	COLOR_CELL* GetCellFromIndex(int iIndex);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to set the size of the borders
	//             for the control.
	// Parameters: l - Specifies the left position.
	//             t - Specifies the top.
	//             r - Specifies the right.
	//             b - Specifies the bottom.
	//-----------------------------------------------------------------------
	void SetBorders(int l = 0, int t = 0, int r = 0, int b = 0);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the color selection dialog
	//          is visible.
	// Returns: True if the color selection dialog is open, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsColorDlgVisible();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to determine if the ColorSelector control is
	//             used in a dialog or in a toolbar.
	// Parameters: pDC - Device context of the control
	// Returns:    TRUE if the ColorSelector control is in a dialog and FALSE if used in a toolbar.
	//-----------------------------------------------------------------------
	BOOL IsColorStatic(CDC* pDC) const;

	//-----------------------------------------------------------------------
	// Summary:    Call this member to switch the visual theme of the control.
	// Parameters: eTheme - New window theme. Can be any of the values listed in the Remarks section.
	//             pTheme - Pointer to the new control theme.
	// Remarks:    eTheme can be one of the following values:
	//             * <b>xtpControlThemeDefault</b> Use default theme.
	//             * <b>xtpControlThemeOfficeXP</b> Use Office XP theme.
	//             * <b>xtpControlThemeOffice2003</b> Use Office 2003 theme.
	//             * <b>xtpControlThemeResource</b> Use Office 2007 theme.
	//-----------------------------------------------------------------------
	BOOL SetTheme(XTPControlTheme eTheme);
	BOOL SetTheme(CXTPColorSelectorCtrlTheme* pTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get a pointer to the currently selected theme.
	// Returns: A pointer to a CXTPColorSelectorCtrlTheme object representing the currently selected theme.
	//-----------------------------------------------------------------------
	CXTPColorSelectorCtrlTheme* GetTheme();

    //-----------------------------------------------------------------------
    // Summary: Call to retrieve the size of a single color button.
    // Returns: A CSize object representing the size of a single color button.
    //-----------------------------------------------------------------------
	CSize GetButtonSize() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retrieve the number of columns displayed.
    // Returns: An integer value representing the number of columns.
    //-----------------------------------------------------------------------
	int GetNumCols() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retrieve the size of the control border.
    // Returns: A CRect representing the size of the control border.
    //-----------------------------------------------------------------------
	CRect GetBorderRect() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retrieve the popup style for the color selector
	//          control.
    // Returns: A DWORD value representing the popup style set for the
	//          control.  Can be one or more of the following values:
	//          * <b>CPS_XTP_NOFILL</b> Displays a "No Fill" button rather than the default Automatic Color button.
	//          * <b>CPS_XTP_EXTENDED</b> Displays 40 extended colors rather than the default 16 colors.
	//          * <b>CPS_XTP_MORECOLORS</b> Displays a "More Colors" button which will display a CXTPColorDialog.
	//          * <b>CPS_XTP_NOAUTOMATIC</b> Do not display an "Automatic Color" button.
	//          * <b>CPS_XTP_RECENTCOLORS</b> Displays a recently used color list for the control.
	//          * <b>CPS_XTP_RIGHTALIGN</b> The popup window is displayed right aligned.
	//          * <b>CPS_XTP_COLORBORDERS</b> The popop window displays color boxes with color borders.
    //-----------------------------------------------------------------------
	DWORD GetPopupStyle() const;

    //-----------------------------------------------------------------------
    // Summary:    Call to modify the color selector style. Styles to be added 
	//             or removed can be combined by using the bitwise OR (|) operator.
    //             Can be one or more of the following values:
	//             * <b>CPS_XTP_NOFILL</b> Displays a "No Fill" button rather than the default Automatic Color button.
	//             * <b>CPS_XTP_EXTENDED</b> Displays 40 extended colors rather than the default 16 colors.
	//             * <b>CPS_XTP_MORECOLORS</b> Displays a "More Colors" button which will display a CXTPColorDialog.
	//             * <b>CPS_XTP_NOAUTOMATIC</b> Do not display an "Automatic Color" button.
	//             * <b>CPS_XTP_RECENTCOLORS</b> Displays a recently used color list for the control.
	//             * <b>CPS_XTP_RIGHTALIGN</b> The popup window is displayed right aligned.
	//             * <b>CPS_XTP_COLORBORDERS</b> The popop window displays color boxes with color borders.
    // Parameters: dwRemove - Specifies styles to be removed during style modification.
	//             dwAdd    - Specifies styles to be added during style modification.
    // Returns:    Nonzero if style was successfully modified; otherwise, 0.
    //-----------------------------------------------------------------------
	BOOL ModifyPopupStyle(DWORD dwRemove, DWORD dwAdd);

    //-----------------------------------------------------------------------
    // Summary: Call to retrieve the array of custom colors defined for the
	//          color selecor control.
    // Returns: A pointer to a XTP_PICK_BUTTON* array representing custom
	//          colors defined for the control.
    //-----------------------------------------------------------------------
	XTP_PICK_BUTTON* GetExtendedColors();

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the number of items in the custom color array.
    // Returns: An integer representing the number of items in the color array.
    //-----------------------------------------------------------------------
	int GetExtendedCount() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the index of the currently selected color.
    // Returns: An integer representing the currently selected color.
    //-----------------------------------------------------------------------
	int GetCurSel() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the the index of the currently pressed color.
    // Returns: An integer representing the currently pressed color.
    //-----------------------------------------------------------------------
	int GetPressed() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the default color defined for the control.
    // Returns: An RGB value representing the default color.
    //-----------------------------------------------------------------------
	COLORREF GetDefColor() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the currently selected color for the control.
    // Returns: An RGB value representing the currently selected color.
    //-----------------------------------------------------------------------
	COLORREF GetCurColor() const;

    //-----------------------------------------------------------------------
    // Summary: Call to retreive a reference to the tooltip for the control.
    // Returns: A reference to a CToolTipCtl obbject.
    //-----------------------------------------------------------------------
	CToolTipCtrl& GetToolTipCtrl();

    //-----------------------------------------------------------------------
    // Summary: Call to retreive a reference to the cell list array for the control.
    // Returns: A reference to a CColorCellList object.
    //-----------------------------------------------------------------------
	CColorCellList& GetColorCellList();

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the string set for the theme caption.
    // Returns: A string representing the theme caption.
    //-----------------------------------------------------------------------
	CString GetThemeTitle();

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the string set for the standard caption.
    // Returns: A string representing the standard caption.
    //-----------------------------------------------------------------------
	CString GetStandardTitle();

    //-----------------------------------------------------------------------
    // Summary: Call to retreive the string set for the custom caption.
    // Returns: A string representing the custom caption.
    //-----------------------------------------------------------------------
	CString GetCustomTitle();

    //-----------------------------------------------------------------------
    // Summary: Call to determine if captions are used for the color selector.
    // Returns: TRUE if themes are used, otherwise FALSE.
    //-----------------------------------------------------------------------
	BOOL HasCaptions() const;

    //-----------------------------------------------------------------------
    // Summary:    Call to hide or show captions for the color selector.
    // Parameters: bShow - TRUE to show captions, FALSE to hide.
    //-----------------------------------------------------------------------
	void ShowCaptions(BOOL bShow);

protected:

    //-----------------------------------------------------------------------
    // Summary:    Called to initialize the object specified by arColors using
	//             the data contained in the array specified by pColors.
    // Parameters: arColors - Reference to an array to be initalized.
	//             pColors  - Array of XTP_PICK_BUTTON structures.
	//             nCount   - Number of items contained in pColors.
    // Returns:    TRUE if the array specified by arColors has been successfully
	//             initialized, otherwise returns FALSE.
    //-----------------------------------------------------------------------
	BOOL SetColors(CXTPPickBtnArray& arColors, const XTP_PICK_BUTTON* pColors, const int nCount);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the color selector.
	// Parameters: pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawColorSelector(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:    This member function will finish the selection process for the color
	//             box or button in the color popup window.
	// Parameters: nCurSel - Current index of the selected color box or button
	//                       in the color popup window.
	//-----------------------------------------------------------------------
	virtual void EndSelection(int nCurSel);

	// ----------------------------------------------------------------------
	// Summary:    This member function is called to determine if the specified color
	//             is already in the ColorDialog's MRU list or in the standard colors
	//             list.
	// Parameters: crFind -  A const COLORREF value.
	// Return:     TRUE if the color is found otherwise returns FALSE.
	// ----------------------------------------------------------------------
	BOOL LookUpColor(const COLORREF crFind);

	// ---------------------------------------------------------------------------
	// Summary:    This member function is called to finish the selection process for
	//             the color box or button in the color popup window.
	// Parameters: nCurSel      - Current index of the selected color box or button in the
	//                            color popup window.
	//             callerParam -  User defined value to be passed to listeners.
	// Remarks:    Override this method to pass user defined data to the listener
	//             windows that receive notification messages.
	// ---------------------------------------------------------------------------
	virtual void OnEndSelection(int nCurSel, LPARAM callerParam);

	//-----------------------------------------------------------------------
	// Summary: This method is called to calculate size of Selector
	// Returns: Size of Selector to fit all buttons.
	//-----------------------------------------------------------------------
	virtual CSize CalcSize();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to refresh theme colors and redraw the control.
	// Parameters: pHeader - Points to the CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

//{{AFX_CODEJOCK_PRIVATE
protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorSelectorCtrl)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorSelectorCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCS);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnSetTheme(WPARAM wParam, LPARAM lParam);

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	BOOL SetThemeColors(XTP_PICK_BUTTON* pColors, int nCount, LPCTSTR lpszTitle, CStringArray& arStrings, int ppShadeMatrix[6][10]);
	friend class CXTPopupColorTearOff;
//}}AFX_CODEJOCK_PRIVATE

public:
	static CUIntArray m_arRecentColors; // Array of recently used colors.
	static CString    m_strTitleRecent; // Represents the title for recent colors caption.

protected:
	int                         m_nCols;            // Number of columns in the color popup window.
	int                         m_nCurSel;          // Currently selected index.
	int                         m_nPressed;         // Pressed button.
	BOOL                        m_bCaptions;        // TRUE to display color captions.
	CWnd*                       m_pParentWnd;       // Points to the parent window for the popup window.
	CRect                       m_rcWnd;            // Rect for the popup window.
	CRect                       m_rcBorders;        // Control borders
	DWORD                       m_dwPopup;          // Color popup window style.
	CPoint                      m_point;            // Last known cursor position.
	CString                     m_strTitleTheme;    // Represents the title for theme colors caption.
	CString                     m_strTitleStandard; // Represents the title for standard colors caption.
	CString                     m_strTitleCustom;   // Custom colors title.
	COLORREF                    m_clrColor;         // An RGB value that represents the currently selected color.
	COLORREF                    m_clrDefault;       // An RGB value that represents the default color for the popup window.
	CToolTipCtrl                m_tooltip;          // Tooltip control.
	CListenerArray              m_listeners;        // Array of listener windows to be sent notifications.
	CColorCellList              m_arCells;          // Array of color items.
	CXTPColorDialog*            m_pColorDlg;        // Points to the color selection dialog when visible.
	CXTPColorSelectorCtrlTheme* m_pTheme;           // Pointer to the current theme.
	XTP_PICK_BUTTON*            m_pExtendedColors;  // Custom colors
	int                         m_nExtendedColors;  // Custom colors count
	CXTPPickBtnArray            m_arThemeColors;    // Theme color array.
	CXTPPickBtnArray            m_arStandardColors; // Standard color array.
};

//===========================================================================
// Summary: CXTPColorSelectorCtrlTheme is derived from CXTPControlThemeManagerStyle to
//          handle theme specific visualizations for the CXTPColorSelectorCtrl
//          class.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrlTheme : public CXTPControlTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPColorSelectorCtrlTheme object.
	//-----------------------------------------------------------------------
	CXTPColorSelectorCtrlTheme();

	//-----------------------------------------------------------------------
	// Summary:    This member function is called by the theme manager to refresh
	//             the visual styles used by each components theme.
	// Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPColorSelectorCtrl* pOwner);

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to draw the background of the
	//             ColorSelector control.
	// Parameters: pDC    - A pointer to a valid device context.
	//             pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//             rect   - A  CRect object that contains the dimensions of the area to fill.
	//-----------------------------------------------------------------------
	virtual void FillBackground(CDC* pDC, CXTPColorSelectorCtrl* pOwner, const CRect& rect);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the cell specified
	//             by 'pColorCell'.
	// Parameters: pDC        - A CDC pointer that represents the current device context.
	//             pOwner     - A pointer to the parent CXTPColorSelectorCtrl object.
	//             pColorCell - A COLOR_CELL object.
	//             bHilite    - TRUE if mouse over cell
	//             bPressed   - TRUE if cell pressed
	//-----------------------------------------------------------------------
	virtual void DrawColorCell(CDC* pDC, CXTPColorSelectorCtrl* pOwner, CXTPColorSelectorCtrl::COLOR_CELL* pColorCell, BOOL bHilite, BOOL bPressed);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the color selector.
	// Parameters: pDC    - A CDC pointer that represents the current device context.
	//             pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void DrawColorSelector(CDC* pDC, CXTPColorSelectorCtrl* pOwner);

    //-----------------------------------------------------------------------
    // Summary:    Called to determine the size of the color selector window.
    // Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
    // Returns:    A CSize object containing the width and height for the control.
    //-----------------------------------------------------------------------
	virtual CSize CalcSize(CXTPColorSelectorCtrl* pOwner);

    //-----------------------------------------------------------------------
    // Summary:    Called to add the 'Automatic Color' button to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if the 'Automatic Color' button was successfully created, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateAutoButton(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add default color buttons to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateExtendedButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add theme and standard buttons to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateThemeButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to determine if theme or default color buttons should be added.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateColorButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add recent colors to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateRecentColorButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add a 'More Colors' button to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateMoreButton(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary: Call to retrieve the size of a single color button.
    // Returns: A CSize object representing the size of a single color button.
    //-----------------------------------------------------------------------
	virtual CSize GetButtonSize() const;

public:
	int      m_nAutoButton;     // Height of the Automatic / More Colors buttons.
	int      m_nCaption;        // Height of a caption that is displayed in the control.
	int      m_nMargin;         // Size for default margin spacing.
	CRect    m_rcThemeText;     // Size and location of the 'Theme Colors' caption area text.
	CRect    m_rcRecentText;    // Size and location of the 'Recent Colors' caption area text.
	CRect    m_rcStandardText;  // Size and location of the 'Standard Colors' caption area text.
	CSize    m_sizeButton;      // cx and cy size for a color picker button.
	BOOL     m_bThinBorder;     // TRUE to use thin borders.
	COLORREF m_crBorder;        // RGB value representing the border color.
	COLORREF m_crBorderHilight; // RGB value for border highlight color.
	COLORREF m_crBorderPressed; // RGB value for border pressed color.
	COLORREF m_crBack;          // RGB value for background color.
	COLORREF m_crBackPressed;   // RGB value for pressed background color.
	COLORREF m_crBackHilight;   // RGB value for hilight background color.
	COLORREF m_crBackChecked;   // RGB value for checked background color.
	COLORREF m_crText;          // RGB value for text color.
	COLORREF m_crTextPressed;   // RGB value for text pressed color.
	COLORREF m_crPopupBorder;   // RGB value for the popup border.
};

//===========================================================================
// Summary: CXTPColorSelectorCtrlThemeOfficeXP is derived from CXTPColorSelectorCtrlTheme to
//          handle theme specific visualizations for the CXTPColorSelectorCtrl
//          class.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrlThemeOfficeXP : public CXTPColorSelectorCtrlTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPColorSelectorCtrlThemeOfficeXP object.
	//-----------------------------------------------------------------------
	CXTPColorSelectorCtrlThemeOfficeXP();

	//-----------------------------------------------------------------------
	// Summary:    This member function is called by the theme manager to refresh
	//             the visual styles used by each components theme.
	// Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPColorSelectorCtrl* pOwner);
};

//===========================================================================
// Summary: CXTPColorSelectorCtrlThemeOffice2003 is derived from CXTPColorSelectorCtrlThemeOfficeXP to
//          handle theme specific visualizations for the CXTPColorSelectorCtrl
//          class.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrlThemeOffice2003 : public CXTPColorSelectorCtrlThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPColorSelectorCtrlThemeOffice2003 object.
	//-----------------------------------------------------------------------
	CXTPColorSelectorCtrlThemeOffice2003();

	//-----------------------------------------------------------------------
	// Summary:    This member function is called by the theme manager to refresh
	//             the visual styles used by each components theme.
	// Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPColorSelectorCtrl* pOwner);
};

//===========================================================================
// Summary: CXTPColorSelectorCtrlThemeResource is derived from CXTPColorSelectorCtrlThemeOfficeXP to
//          handle theme specific visualizations for the CXTPColorSelectorCtrl
//          class.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrlThemeResource : public CXTPColorSelectorCtrlThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPColorSelectorCtrlThemeResource object.
	//-----------------------------------------------------------------------
	CXTPColorSelectorCtrlThemeResource();

	//-----------------------------------------------------------------------
	// Summary:    This member function is called by the theme manager to refresh
	//             the visual styles used by each components theme.
	// Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPColorSelectorCtrl* pOwner);
};

//===========================================================================
// Summary: CXTPColorSelectorCtrlThemeOffice2013 is derived from CXTPColorSelectorCtrlTheme
//          to handle theme specific visualizations for the CXTPColorSelectorCtrl class.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrlThemeOffice2013 : public CXTPColorSelectorCtrlTheme
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default constructor.
    //-----------------------------------------------------------------------
	CXTPColorSelectorCtrlThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary:    Called by the theme manager to refresh the visual styles 
	//             used by each components theme.
	// Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPColorSelectorCtrl* pOwner);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the cell specified
	//             by 'pColorCell'.
	// Parameters: pDC      - A CDC pointer that represents the current device context.
	//             rcItem   - Specifies the size of the border to draw.
	//             rcBorder - Specifies left, top, right and bottom borders. 1 = draw border, 0 = do not draw border.
	//             crBorder - RGB value specifying the border color to draw.
	//-----------------------------------------------------------------------
	virtual void DrawBorder(CDC* pDC, CRect rcItem, CRect rcBorder, COLORREF crBorder);

    //-----------------------------------------------------------------------
    // Summary:    Called to draw a color cell for the color selector control.
    // Parameters: pDC        - A CDC pointer that represents the current device context.
	//             pOwner     - A pointer to the parent CXTPColorSelectorCtrl object.
	//             pColorCell - A COLOR_CELL object.
	//             rcItem     - Size and location of the color cell to draw.
	//             bHilite    - TRUE if the color should be drawn highlighted.
	//             bChecked   - TRUE if the color should be drawn checked.
	//             bPressed   - TRUE if the color should be drawn pressed.
    //-----------------------------------------------------------------------
	virtual void DrawColor(CDC* pDC, CXTPColorSelectorCtrl* pOwner, CXTPColorSelectorCtrl::COLOR_CELL* pColorCell, CRect rcItem, BOOL bChecked, BOOL bHilite, BOOL bPressed);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the cell specified
	//             by 'pColorCell'.
	// Parameters: pDC        - A CDC pointer that represents the current device context.
	//             pOwner     - A pointer to the parent CXTPColorSelectorCtrl object.
	//             pColorCell - A COLOR_CELL object.
	//             bHilite    - TRUE if mouse over cell
	//             bPressed   - TRUE if cell pressed
	//-----------------------------------------------------------------------
	virtual void DrawColorCell(CDC* pDC, CXTPColorSelectorCtrl* pOwner, CXTPColorSelectorCtrl::COLOR_CELL* pColorCell, BOOL bHilite, BOOL bPressed);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the color selector.
	// Parameters: pDC    - A CDC pointer that represents the current device context.
	//             pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void DrawColorSelector(CDC* pDC, CXTPColorSelectorCtrl* pOwner);

    //-----------------------------------------------------------------------
    // Summary:    Called to determine the size of the color selector window.
    // Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
    // Returns:    A CSize object containing the width and height for the control.
    //-----------------------------------------------------------------------
	virtual CSize CalcSize(CXTPColorSelectorCtrl* pOwner);

    //-----------------------------------------------------------------------
    // Summary:    Called to add the 'Automatic Color' button to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if the 'Automatic Color' button was successfully created, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateAutoButton(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add default color buttons to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateExtendedButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add theme and standard buttons to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateThemeButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add recent colors to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateRecentColorButtons(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

    //-----------------------------------------------------------------------
    // Summary:    Called to add a 'More Colors' button to the color selector control.
    // Parameters: pOwner   - A pointer to the parent CXTPColorSelectorCtrl object.
	//             nIndex   - Reference to an integer representing the next available index in the color cell array.
	//             x        - Specifies starting coordinates for the left edge.
	//             y        - Specifies starting coordinates for the top edge.
	//             cx       - Specifies the width of the client area.
	//             bChecked - TRUE when a color button has been checked.
    // Returns:    TRUE if successful, otherwise FALSE.
    //-----------------------------------------------------------------------
	virtual BOOL CreateMoreButton(CXTPColorSelectorCtrl* pOwner, int& nIndex, const int x, int& y, const int cx, bool& bChecked);

protected:
	COLORREF m_crTitleText;    // RGB value representing the title text color.
	COLORREF m_crTitleBack;    // RGB value representing the title background color.
	COLORREF m_crBorderInside; // RGB value representing the inside border highlight color.
};

//===========================================================================
// Summary: CXTPColorSelectorCtrlThemeVisualStudio2015 is derived from CXTPColorSelectorCtrlThemeOffice2013
//          to handle theme specific visualizations for the CXTPColorSelectorCtrl class.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSelectorCtrlThemeVisualStudio2015 : public CXTPColorSelectorCtrlThemeOffice2013
{
public:
    //-----------------------------------------------------------------------
    // Summary: Default constructor.
    //-----------------------------------------------------------------------
	CXTPColorSelectorCtrlThemeVisualStudio2015();

	//-----------------------------------------------------------------------
	// Summary:    Called by the theme manager to refresh the visual styles 
	//             used by each components theme.
	// Parameters: pOwner - A pointer to the parent CXTPColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPColorSelectorCtrl* pOwner);

	//-----------------------------------------------------------------------
	// Summary:    This member function is called to draw the cell specified
	//             by 'pColorCell'.
	// Parameters: pDC        - A CDC pointer that represents the current device context.
	//             pOwner     - A pointer to the parent CXTPColorSelectorCtrl object.
	//             pColorCell - A COLOR_CELL object.
	//             bHilite    - TRUE if mouse over cell
	//             bPressed   - TRUE if cell pressed
	//-----------------------------------------------------------------------
	virtual void DrawColorCell(CDC* pDC, CXTPColorSelectorCtrl* pOwner, CXTPColorSelectorCtrl::COLOR_CELL* pColorCell, BOOL bHilite, BOOL bPressed);
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTPColorSelectorCtrl::SetBorders(int l, int t, int r, int b) {
	m_rcBorders.left = l; m_rcBorders.top = t; m_rcBorders.right = r; m_rcBorders.bottom = b;
}
AFX_INLINE BOOL CXTPColorSelectorCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE void CXTPColorSelectorCtrl::SetColors(XTP_PICK_BUTTON* pColors, int nCount, int nCols /*=8*/, LPCTSTR lpszTitle /*=NULL*/) {
	m_pExtendedColors = pColors; m_nExtendedColors = nCount; m_nCols = nCols; m_strTitleCustom = lpszTitle;
}
AFX_INLINE CXTPColorSelectorCtrlTheme* CXTPColorSelectorCtrl::GetTheme() {
	return m_pTheme;
}
AFX_INLINE  void CXTPColorSelectorCtrl::AddRecentColor(COLORREF clrRecent) {
	m_arRecentColors.Add(clrRecent);
}
AFX_INLINE void CXTPColorSelectorCtrl::ResetRecentColors() {
	m_arRecentColors.RemoveAll();
}
AFX_INLINE CUIntArray& CXTPColorSelectorCtrl::GetRecentColors() {
	return m_arRecentColors;
}
AFX_INLINE CXTPPickBtnArray& CXTPColorSelectorCtrl::GetThemeColors() {
	return m_arThemeColors;
}
AFX_INLINE CXTPPickBtnArray& CXTPColorSelectorCtrl::GetStandardColors() {
	return m_arStandardColors;
}
AFX_INLINE CSize CXTPColorSelectorCtrl::GetButtonSize() const {
	ASSERT(m_pTheme != NULL); return m_pTheme->GetButtonSize();
}
AFX_INLINE int CXTPColorSelectorCtrl::GetNumCols() const {
	return m_nCols;
}
AFX_INLINE CRect CXTPColorSelectorCtrl::GetBorderRect() const {
	return m_rcBorders;
}
AFX_INLINE DWORD CXTPColorSelectorCtrl::GetPopupStyle() const {
	return m_dwPopup;
}
AFX_INLINE XTP_PICK_BUTTON* CXTPColorSelectorCtrl::GetExtendedColors() {
	return m_pExtendedColors;
}
AFX_INLINE int CXTPColorSelectorCtrl::GetExtendedCount() const {
	return m_nExtendedColors;
}
AFX_INLINE int CXTPColorSelectorCtrl::GetCurSel() const {
	return m_nCurSel;
}
AFX_INLINE int CXTPColorSelectorCtrl::GetPressed() const {
	return m_nPressed;
}
AFX_INLINE COLORREF CXTPColorSelectorCtrl::GetDefColor() const {
	return m_clrDefault;
}
AFX_INLINE COLORREF CXTPColorSelectorCtrl::GetCurColor() const {
	return m_clrColor;
}
AFX_INLINE CToolTipCtrl& CXTPColorSelectorCtrl::GetToolTipCtrl() {
	return m_tooltip;
}
AFX_INLINE CXTPColorSelectorCtrl::CColorCellList& CXTPColorSelectorCtrl::GetColorCellList() {
	return m_arCells;
}
AFX_INLINE CString CXTPColorSelectorCtrl::GetThemeTitle() {
	return m_strTitleTheme;
}
AFX_INLINE CString CXTPColorSelectorCtrl::GetStandardTitle() {
	return m_strTitleStandard;
}
AFX_INLINE CString CXTPColorSelectorCtrl::GetCustomTitle() {
	return m_strTitleCustom;
}
AFX_INLINE BOOL CXTPColorSelectorCtrl::HasCaptions() const {
	return m_bCaptions;
}
AFX_INLINE void CXTPColorSelectorCtrl::ShowCaptions(BOOL bShow) {
	m_bCaptions = bShow;
}

/////////////////////////////////////////////////////////////////////////////


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //__XTPCOLORSELECTORCTRL_H__
