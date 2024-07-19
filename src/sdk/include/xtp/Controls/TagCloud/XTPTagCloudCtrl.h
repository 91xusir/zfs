// XTPTagCloudCtrl.h : header file
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
#if !defined(__XTPTAGCLOUDCTRL_H__)
#define __XTPTAGCLOUDCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: CXTPTagCloudStyle is a helper class used by CXTPTagCloudStyleManager
//          to manage styles for the CXTPTagCloudCtrl class.
//===========================================================================
class _XTP_EXT_CLASS CXTPTagCloudStyle
{
public:
	//-----------------------------------------------------------------------
	// Summary:    Default constructor.
	// Parameters: nScore       - Unique ID for the style.
	//             clrText      - RGB value representing the style text color.
	//             clrTextHover - RGB value representing the style text hover
	//                            color.
	//             clrBorder    - RGB value representing the style border 
	//                            color, setting to COLORREF_NULL (-1) 
	//                            indicates no border.
	//                    lf    - Reference to a LOGFONT struct used to create
	//                            the font used by the style.
	//-----------------------------------------------------------------------
	CXTPTagCloudStyle();
	CXTPTagCloudStyle(const CXTPTagCloudStyle& style);
	CXTPTagCloudStyle(int nScore, COLORREF clrText, COLORREF clrTextHover, COLORREF clrBorder, LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Summary:    Sets the font for the style using the LOGFONT struct 
	//             specified by lf.
	// Parameters: lf - Reference to a LOGFONT struct used to create the font
	//                  used by the style.
	//-----------------------------------------------------------------------
	void SetFont(LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Summary:    Overloaded operator initializes a CXTPTagCloudStyle
	//             class using the object specified by style.
	// Parameters: style - Reference to a CXTPTagCloudStyle object used to
	//                     initialize this class.
	// Returns:    A reference to the CXTPTagCloudStyle object.
	//-----------------------------------------------------------------------
	CXTPTagCloudStyle& operator=(const CXTPTagCloudStyle& style);

	//-----------------------------------------------------------------------
	// Summary:    Copies the contents from one CXTPTagCloudStyle object to
	//             another.
	// Parameters: pSource - Points to the source CXTPTagCloudStyle object.
	//             pDest   - Points to the destination CXTPTagCloudStyle object.
	//-----------------------------------------------------------------------
	static void AFX_CDECL Copy(const CXTPTagCloudStyle* pSource, CXTPTagCloudStyle* pDest);

	int      m_nScore;       // Unique identifier used to specify the score or weight of the style.

	CXTPFont m_xtpFont;      // Font used by the style.
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_font, m_xtpFont, GetFontHandle);

	COLORREF m_clrText;      // RGB value representing the style text color.
	COLORREF m_clrTextHover; // RGB value representing the style text hover color.
	COLORREF m_clrBorder;    // RGB value representing the style border color.
};

//===========================================================================
// Summary: CArray based object used for storing CXTPTagCloudStyle structs.
//===========================================================================
typedef CArray<CXTPTagCloudStyle*, CXTPTagCloudStyle*> CXTPTagCloudStyleArray;

//===========================================================================
// Summary: CXTPTagCloudStyleManager is a singleton class used to manage 
//          CXTPTagCloudStyle structs that identify each tag style used by 
//          CXTPTagCloudCtrl.
//===========================================================================
class _XTP_EXT_CLASS CXTPTagCloudStyleManager : public CXTPCmdTarget
{
	CXTPTagCloudStyleManager();

public:
	//-----------------------------------------------------------------------
	// Summary: Destructor used to handle clean-up and de-allocation.
	//-----------------------------------------------------------------------
	~CXTPTagCloudStyleManager();

	//-----------------------------------------------------------------------
	// Summary: Called by the CXTPTagCloudCtrl to initialize a default array 
	//          of CXTPTagCloudStyle structs representing unique tag styles.
	//-----------------------------------------------------------------------
	void InitDefaultStyles();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to add a new style to the 
	//             style array.
	// Parameters: nScore       - Unique ID for the style.
	//             clrText      - RGB value representing the style text color.
	//             clrTextHover - RGB value representing the style text hover
	//                            color.
	//             clrBorder    - RGB value representing the style border 
	//                            color, setting to COLORREF_NULL (-1) 
	//                            indicates no border.
	//                    lf    - Reference to a LOGFONT struct used to create
	//                            the font used by the style.
	//                pStyle    - Pointer to a valid CXTPTagCloudStyle object.
	// Returns:    The index of the added element.
	//-----------------------------------------------------------------------
	INT_PTR AddStyle(int nScore, COLORREF clrText, COLORREF clrTextHover, COLORREF clrBorder, LOGFONT& lf);
	INT_PTR AddStyle(CXTPTagCloudStyle* pStyle);

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to remove an existing style from
	//             the style array.
	// Parameters: nScore - Identifier of the style to remove.
	// Returns:    TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL RemoveStyle(int nScore);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to remove all tag styles from the
	//          style array and free any allocated memory.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to retrieve the style associated
	//             with the identifier specified by nScore.
	// Parameters: nScore - Unique identifier for the style to find.
	// Returns:    A pointer to a CXTPTagCloudStyle object if successful, 
	//             otherwise NULL.
	//-----------------------------------------------------------------------
	CXTPTagCloudStyle* GetStyle(int nScore);

	//-----------------------------------------------------------------------
	// Summary:    Sets the color values for the style specified by nScore.
	// Parameters: nScore       - ID of the style to update.
	//             clrText      - RGB value representing the style text color,
	//                            if COLORREF_NULL this value is ignored.
	//             clrTextHover - RGB value representing the style text highlight
	//                            color, if COLORREF_NULL this value is ignored
	//             clrBorder    - RGB value representing the style border color,
	//                            if COLORREF_NULL no border will be drawn.
	// Returns:    TRUE if the style was found and updated, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetColors(int nScore, COLORREF clrText, COLORREF clrTextHover, COLORREF clrBorder = COLORREF_NULL);

	//-----------------------------------------------------------------------
	// Summary:    Sets the font for the style specified by nScore.
	// Parameters: nScore - ID of the style to update.
	//                 lf - Reference to a LOGFONT struct used to create
	//                      the font used by the style.
	// Returns:    TRUE if the style was found and updated, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetFont(int nScore, LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve a reference to the
	//          CXTPTagCloudStyleArray managed by CXTPTagCloudStyleManager.
	// Returns: A reference to a CXTPTagCloudStyleArray object.
	//-----------------------------------------------------------------------
	CXTPTagCloudStyleArray& GetCloudStyleArray();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to determine if the score
	//             specified by nScore already exists in the array.
	// Parameters: nScore - Score index to check.
	// Returns:    TRUE if the score already exists, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL ScoreExists(int nScore);

protected:
	CXTPTagCloudStyleArray m_arrStyles; // Array of CXTPTagCloudStyle structs managed by CXTPTagCloudStyleManager.

private:
	friend class CXTPSingleton<CXTPTagCloudStyleManager>;
	friend _XTP_EXT_CLASS CXTPTagCloudStyleManager* AFX_CDECL XTPTabCloudStyleManager();
#ifdef _XTP_ACTIVEX
//{{AFX_CODEJOCK_PRIVATE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPTagCloudStyleManager);

	void OleInitDefaultStyles();
	void OleAdd(LPDISPATCH pStyle);
	void OleRemove(long Score);
	void OleRemoveAll();
	LPDISPATCH OleGet(long Score);
	void OleSetStyle(LPDISPATCH pStyle);
	BOOL OleScoreExists(long Score);
	LPDISPATCH OleCreateStyle();
	virtual void OnFinalRelease();
	afx_msg long OleGetItemCount();
	afx_msg LPDISPATCH OleGetItem(long Index);

	DECLARE_ENUM_VARIANT(CXTPTagCloudStyleManager);

//}}AFX_CODEJOCK_PRIVATE
#endif
};

AFX_INLINE CXTPTagCloudStyleArray& CXTPTagCloudStyleManager::GetCloudStyleArray() {
	return m_arrStyles;
}

//===========================================================================
// Summary: Use the XTPTabCloudStyleManager() function for access to the 
//          singleton class CXTPTagCloudStyleManager.
//===========================================================================
_XTP_EXT_CLASS CXTPTagCloudStyleManager* AFX_CDECL XTPTabCloudStyleManager();

//===========================================================================
// Summary: Manages the attributes for each tag element such as weight, font,
//          background and foreground color.
//===========================================================================
struct XTP_TAGCLOUDTAG
{
	//-----------------------------------------------------------------------
	// Summary:    Default constructor initializes members.
	// Parameters: lpszItem - Name of the tag cloud item.
	//             nScore   - Score of the tag cloud item.
	//-----------------------------------------------------------------------
	XTP_TAGCLOUDTAG(LPCTSTR lpszItem, int nScore) : strItem(lpszItem), nScore(nScore) { }

	int           nScore;  // Style for this tag.
	CString       strItem; // String value representing the tag text.
	CXTPEmptyRect rcItem;  // Size and location of the tag.
};

//===========================================================================
// Summary: XTPTagCloudSort is an enumeration used by the CXTPTagCloudTagArray
//          class to perform sorting operations.
//===========================================================================
enum XTPTagCloudSort
{
	xtpTagCloudNone,    // No sorting
	xtpTagCloudAlpha,   // Sort by tag name
	xtpTagCloudNumeric, // Sort by tag score
};

//===========================================================================
// Summary: CXTPTagCloudTagArray is a CArray derived class used to manage
//          XTP_TAGCLOUDTAG objects for the CXTPTagCloudCtrl class.
//===========================================================================
class _XTP_EXT_CLASS CXTPTagCloudTagArray : public CArray<XTP_TAGCLOUDTAG*, XTP_TAGCLOUDTAG*>
{
public:
	//-----------------------------------------------------------------------
	// Summary:    Call this member function to sort the tag item array by
	//             style specified by nSort.
	// Parameters: nSort - Specifies the sorting style can be one of the
	//                     following values: xtpTagCloudNone, xtpTagCloudAlpha 
	//                     or xtpTagCloudNumeric.
	//             bAsc  - TRUE to sort ascending, FALSE to sort descending.
	//-----------------------------------------------------------------------
	void Sort(XTPTagCloudSort nSort, BOOL bAsc);

private:

	//-----------------------------------------------------------------------
	// Private use: CXTPTagCloudCtrl::RemoveAll() instead.
	//-----------------------------------------------------------------------
	void RemoveAll();
	//-----------------------------------------------------------------------
	// Private use: CXTPTagCloudCtrl::RemoveTag(XTP_TAGCLOUDTAG* pTag) instead.
	//-----------------------------------------------------------------------
	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);

	static int AFX_CDECL CompareName(XTP_TAGCLOUDTAG* pTagA, XTP_TAGCLOUDTAG* pTagB, BOOL bAsc);
	static int AFX_CDECL CompareScore(XTP_TAGCLOUDTAG* pTagA, XTP_TAGCLOUDTAG* pTagB, BOOL bAsc);
	static int AFX_CDECL CompareNameAsc(LPCVOID a, LPCVOID b);
	static int AFX_CDECL CompareNameDesc(LPCVOID a, LPCVOID b);
	static int AFX_CDECL CompareScoreAsc(LPCVOID a, LPCVOID b);
	static int AFX_CDECL CompareScoreDesc(LPCVOID a, LPCVOID b);

	friend class CXTPTagCloudCtrl;
};

AFX_INLINE int AFX_CDECL CXTPTagCloudTagArray::CompareNameAsc(LPCVOID a, LPCVOID b) {
	return CompareName(*(XTP_TAGCLOUDTAG**)a, *(XTP_TAGCLOUDTAG**)b, TRUE);
}
AFX_INLINE int AFX_CDECL CXTPTagCloudTagArray::CompareNameDesc(LPCVOID a, LPCVOID b) {
	return CompareName(*(XTP_TAGCLOUDTAG**)a, *(XTP_TAGCLOUDTAG**)b, FALSE);
}
AFX_INLINE int AFX_CDECL CXTPTagCloudTagArray::CompareScoreAsc(LPCVOID a, LPCVOID b) {
	return CompareScore(*(XTP_TAGCLOUDTAG**)a, *(XTP_TAGCLOUDTAG**)b, TRUE);
}
AFX_INLINE int AFX_CDECL CXTPTagCloudTagArray::CompareScoreDesc(LPCVOID a, LPCVOID b) {
	return CompareScore(*(XTP_TAGCLOUDTAG**)a, *(XTP_TAGCLOUDTAG**)b, FALSE);
}

//===========================================================================
// Summary: The CXTPTagCloudCtrl class is used to create a tag cloud control
//          representing a visual depiction of generated tags identified by 
//          color and font size to represent the prominence or frequency of
//          the tags depicted.
//===========================================================================
class _XTP_EXT_CLASS CXTPTagCloudCtrl : public CWnd
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default constructor.
	//-----------------------------------------------------------------------
	CXTPTagCloudCtrl();

	//-----------------------------------------------------------------------
	// Summary: Destructor used to handle clean-up and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPTagCloudCtrl();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to create the CXTPTagCloudCtrl
	//             window.
	// Parameters: dwStyle    - Specifies the window style.
	//             rect       - Size of the tag cloud control.
	//             pParentWnd - Parent window.
	//             nID        - Identifier for the tag cloud control.
	// Returns:    
	//-----------------------------------------------------------------------
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	//-----------------------------------------------------------------------
	// Summary:    Adds a new tag to the tag cloud control.
	// Parameters: pTag - Points to a valid XTP_TAGCLOUDTAG object to add.
	//-----------------------------------------------------------------------
	void AddTag(XTP_TAGCLOUDTAG* pTag);

	//-----------------------------------------------------------------------
	// Summary:    Removes a tag from the tag cloud control.
	// Parameters: pTag - Points to a valid XTP_TAGCLOUDTAG object to remove.
	//-----------------------------------------------------------------------
	void RemoveTag(XTP_TAGCLOUDTAG* pTag);

	//-----------------------------------------------------------------------
	// Summary: Removes all tags from the XTP_TAGCLOUDTAG window.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:    Retrieves the tag from the cursor position specified by point.
	// Parameters: point - cursor position in client coordinates.
	// Returns:    A pointer to a XTP_TAGCLOUDTAG if successful, otherwise NULL.
	//-----------------------------------------------------------------------
	XTP_TAGCLOUDTAG* HitTest(const CPoint& point);

	//-----------------------------------------------------------------------
	// Summary: Recalculates the size and position for all tags associated 
	//          with the tag cloud control.
	//-----------------------------------------------------------------------
	virtual void RecalcLayout();

	//-----------------------------------------------------------------------
	// Summary:    Sets the margin for the client area of the 
	//             tag cloud control.
	// Parameters: nMargin - Size in pixels of the margin to set.
	//-----------------------------------------------------------------------
	void SetMargin(int nMargin);

	//-----------------------------------------------------------------------
	// Summary: Retrieves the client area margin for the tag cloud control.
	// Returns: An integer representing the client area margin.
	//-----------------------------------------------------------------------
	int GetMargin() const;

	//-----------------------------------------------------------------------
	// Summary:    Sets the padding for each tag associated with the 
	//             tag cloud control.
	// Parameters: nPadding - Size in pixels of the padding to set.
	//-----------------------------------------------------------------------
	void SetPadding(int nPadding);

	//-----------------------------------------------------------------------
	// Summary: Retrieves the padding for each tag associated with the 
	//          tag cloud control.
	// Returns: An integer representing the padding for each tag.
	//-----------------------------------------------------------------------
	int GetPadding() const;

	//-----------------------------------------------------------------------
	// Summary:    Enables / disables tooltips for the tag cloud control.
	// Parameters: bTipEnabled - TRUE to enable tooltips, FALSE to disable.
	//-----------------------------------------------------------------------
	void EnableTooltip(BOOL bTipEnabled);

	//-----------------------------------------------------------------------
	// Summary: Called to determine if tooltips are enabled for the tab cloud
	//          control.
	// Returns: TRUE if tooltips are enabled for the tag cloud control.
	//-----------------------------------------------------------------------
	BOOL IsTipEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:    Enables / disables tag item focus rectangle display.
	// Parameters: bFocusRect - TRUE to enable tag item focus display.
	//-----------------------------------------------------------------------
	void EnableFocusRect(BOOL bFocusRect);

	//-----------------------------------------------------------------------
	// Summary: Called to determine if tag item focus display is enabled for
	//          the tag cloud control.
	// Returns: TRUE if tag item focus has been enabled, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsFocusEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:    Sets the sort order for all tags associated with the tag
	//             cloud control.
	// Parameters: nSortOrder - Specifies the sort order for the tag items, 
	//                          can be one of the following values: 
	//                          xtpTagCloudNone, xtpTagCloudAlpha or 
	//                          xtpTagCloudNumeric.
	//             bAsc       - TRUE to sort ascending, FALSE to sort 
	//                          descending.
	//-----------------------------------------------------------------------
	void SetSortOrder(XTPTagCloudSort nSortOrder, BOOL bAsc);

	//-----------------------------------------------------------------------
	// Summary: Called to determine the sort order for the tag cloud control,
	//          can be one of the following values: xtpTagCloudNone, 
	//          xtpTagCloudAlpha or xtpTagCloudNumeric.
	// Returns: A XTPTagCloudSort enumeration value representing the tag cloud
	//          control sort order.
	//-----------------------------------------------------------------------
	XTPTagCloudSort GetSortOrder() const;

	//-----------------------------------------------------------------------
	// Summary: Retrieves a pointer to the currently selected tag if any.
	// Returns: A pointer to the currently selected XTP_TAGCLOUDTAG object
	//          if a tag has been selected, otherwise NULL.
	//-----------------------------------------------------------------------
	XTP_TAGCLOUDTAG* GetCurSel();

	//-----------------------------------------------------------------------
	// Summary:    Sets the current selection for the tag cloud control
	//             and sends event notification to the owner window.
	// Parameters: pCurSel - A pointer to a valid XTP_TAGCLOUDTAG tag.
	//-----------------------------------------------------------------------
	void SetCurSel(XTP_TAGCLOUDTAG* pCurSel);

	//-----------------------------------------------------------------------
	// Summary:    Sets the background color for the tag cloud control.
	// Parameters: clrBack - A CXTPPaintManagerColorGradient representing the
	//                       background color for the tag cloud control.  If
	//                       dark and light colors are specified the background
	//                       will be drawn gradient, otherwise drawn solid.
	//             bHorz   - TRUE if the gradient should be drawn horizontal,
	//                       FALSE to draw vertical.
	//-----------------------------------------------------------------------
	void SetBackColor(CXTPPaintManagerColorGradient clrBack, BOOL bHorz = FALSE);

	//-----------------------------------------------------------------------
	// Summary: Retrieves the background color for the tag cloud control.
	// Returns: A CXTPPaintManagerColorGradient representing the background
	//          color for the tag cloud control.
	//-----------------------------------------------------------------------
	CXTPPaintManagerColorGradient GetBackColor() const;

	//-----------------------------------------------------------------------
	// Summary:    Sets the border color for the tag cloud control.
	// Parameters: clrBorder - Specifies the new border color.
	//-----------------------------------------------------------------------
	void SetBorderColor(CXTPPaintManagerColor clrBorder);

	//-----------------------------------------------------------------------
	// Summary: Retrieves the border color for the tag cloud control.
	// Returns: A CXTPPaintManagerColor object representing the border color
	//          for the tag cloud control.
	//-----------------------------------------------------------------------
	CXTPPaintManagerColor GetBorderColor() const;

	//-----------------------------------------------------------------------
	// Summary: Determines if the background gradient fill style is horizontal
	//          or vertical.
	// Returns: TRUE if the fill style is horizontal, FALSE if vertical.
	//-----------------------------------------------------------------------
	BOOL IsFillHorz() const;

	//-----------------------------------------------------------------------
	// Summary: Called to restore all default values for the tag cloud control.
	//-----------------------------------------------------------------------
	void RestoreDefaults();

	//{{AFX_VIRTUAL(CXTPTagCloudCtrl)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:

	//-----------------------------------------------------------------------
	// Summary: Retrieves a reference to the CXTPTagCloudTagArray that contains
	//          all of the tags associated with the tag cloud control.
	// Returns: A reference to a CXTPTagCloudTagArray object.
	//-----------------------------------------------------------------------
	virtual CXTPTagCloudTagArray& GetTagArray();

	//-----------------------------------------------------------------------
	// Summary:    Called by the tag cloud control to register the window
	//             class used to create the control.
	// Parameters: hInstance - Handle to the instance that contains the window 
	//                         procedure for the class.
	// Returns:    TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

	//-----------------------------------------------------------------------
	// Summary:    Called by the tag cloud control to draw the client area of
	//             the control.
	// Parameters: pDC      - Points to a valid device context.
	//             rcClient - Size of the client area to draw.
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC, const CRect& rcClient);

	//-----------------------------------------------------------------------
	// Summary:    Called by the OnDraw function to calculate the size and 
	//             position for tag in the row specified by arrRow.
	// Parameters: rcClient - Specifies the client area of the tag cloud 
	//                        minus margins.
	//             arrRow   - Array of tags representing a single row in the
	//                        tag cloud control.
	// Returns:    A CSize object representing the max width and height for 
	//             each tag in the row specified by arrRow.
	//-----------------------------------------------------------------------
	virtual CSize CalcRowLayout(const CRect& rcClient, const CXTPTagCloudTagArray& arrRow);

	//-----------------------------------------------------------------------
	// Summary: Retrieves the cursor position in window coordinates and
	//          determines if the cursor is hovering over the tag cloud window.
	// Returns: TRUE if the cursor is hovering over the tag cloud control, 
	//          otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL PointInRect();

	//-----------------------------------------------------------------------
	// Summary:    Called during mouse move events to set the active / inactive
	//             state for the tag cloud control.
	// Parameters: bActive - TRUE if the tag cloud control is active, FALSE
	//                       if inactive.
	//-----------------------------------------------------------------------
	virtual void SetActive(BOOL bActive);

	//-----------------------------------------------------------------------
	// Summary:    Called by tag cloud control to send notification messages
	//             to the owner window when an event specified by nEventCode
	//             occurs.
	// Parameters: pTag - Points to a XTP_TAGCLOUDTAG struct. 
	// Returns:    TRUE if notification was sent to the owner window, otherwise
	//             returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL OnNotify(XTP_TAGCLOUDTAG* pTag, UINT nEventCode);

	//-----------------------------------------------------------------------
	// Summary: Removes all tags from the XTP_TAGCLOUDTAG window.
	//-----------------------------------------------------------------------
	void RemoveAllInternal();

	int                           m_nPadding;        // Size in pixels of the padding around each tag item.
	int                           m_nMargin;         // Size in pixels of the margin for the client area.
	BOOL                          m_bActive;         // TRUE if the tag cloud control is active.
	BOOL                          m_bTipEnabled;     // Set to FALSE to disable tooltips.
	BOOL                          m_bFocusRect;      // Set to FALSE to disable focus rect.
	BOOL                          m_bHorz;           // TRUE if the background gradient colors should be drawn horizontal, FALSE if vertical.
	BOOL                          m_bAsc;            // TRUE if sorting ascending, FALSE if sorting descending.
	HCURSOR                       m_hCursor;         // Handle to the mouse hover cursor.
	CToolTipCtrl                  m_wndToolTip;      // Tooltip to be displayed for the control.
	XTPTagCloudSort               m_nSortOrder;      // Specifies the sort order for the tag cloud control. 
	XTP_TAGCLOUDTAG*              m_pHover;          // Points to the tag item that the mouse cursor is currently hovering over.
	XTP_TAGCLOUDTAG*              m_pCurSel;         // Points to the tag item that is currently selected.
	CXTPTagCloudTagArray          m_arrTags;         // Array of all tags associated with the tag cloud control.
	CXTPTagCloudTagArray          m_arrTagsSorted;   // Array of sorted tags associated with the tag cloud control.
	CXTPPaintManagerColor         m_clrBorder;       // Tag cloud control border color.
	CXTPPaintManagerColorGradient m_clrBack;         // Tag cloud control background colors.
	CXTPWinThemeWrapper           m_themeWrapper;    // Determines if tag cloud control should use Windows themes.

	//{{AFX_MSG(CXTPTagCloudCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//}}AFX_MSG

	//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnMouseLeave();
	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE void CXTPTagCloudCtrl::SetMargin(int nMargin) {
	m_nMargin = nMargin;
}
AFX_INLINE int CXTPTagCloudCtrl::GetMargin() const {
	return m_nMargin;
}
AFX_INLINE void CXTPTagCloudCtrl::SetPadding(int nPadding) {
	m_nPadding = nPadding;
}
AFX_INLINE int CXTPTagCloudCtrl::GetPadding() const {
	return m_nPadding;
}
AFX_INLINE BOOL CXTPTagCloudCtrl::IsTipEnabled() const {
	return m_bTipEnabled;
}
AFX_INLINE void CXTPTagCloudCtrl::EnableFocusRect(BOOL bFocusRect) {
	m_bFocusRect = bFocusRect;
	if (GetSafeHwnd())
		RedrawWindow();
}
AFX_INLINE BOOL CXTPTagCloudCtrl::IsFocusEnabled() const {
	return m_bFocusRect;
}
AFX_INLINE XTPTagCloudSort CXTPTagCloudCtrl::GetSortOrder() const {
	return m_nSortOrder;
}
AFX_INLINE XTP_TAGCLOUDTAG* CXTPTagCloudCtrl::GetCurSel() {
	return m_pCurSel;
}
AFX_INLINE void CXTPTagCloudCtrl::SetBackColor(CXTPPaintManagerColorGradient clrBack, BOOL bHorz) {
	m_clrBack = clrBack; m_bHorz = bHorz;
}
AFX_INLINE CXTPPaintManagerColorGradient CXTPTagCloudCtrl::GetBackColor() const {
	return m_clrBack;
}
AFX_INLINE void CXTPTagCloudCtrl::SetBorderColor(CXTPPaintManagerColor clrBorder) {
	m_clrBorder = clrBorder;
}
AFX_INLINE CXTPPaintManagerColor CXTPTagCloudCtrl::GetBorderColor() const {
	return m_clrBorder;
}
AFX_INLINE BOOL CXTPTagCloudCtrl::IsFillHorz() const {
	return m_bHorz;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTAGCLOUDCTRL_H__)
