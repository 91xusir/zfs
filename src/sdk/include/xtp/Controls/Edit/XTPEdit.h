// XTPEdit.h interface for the CXTPEdit class.
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
#if !defined(__XTPEDIT_H__)
#define __XTPEDIT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPRegExp;
class CXTPWinThemeWrapper;
class CXTPEditTheme;

//===========================================================================
// Summary:
//     CXTPEdit is a CEdit derived class. The CXTPEdit class provides the functionality
//     of a Windows edit control. An edit control is a rectangular child window
//     in which the user can enter text.
// Remarks:
//     You can create an edit control either from a dialog template or directly
//     in your code. In both cases, first call the constructor CXTPEdit to construct
//     the CXTPEdit object, then call the Create member function to create the
//     Windows edit control and attach it to the CXTPEdit object.
//
//     Construction can be a one-step process in a class derived from CXTPEdit.
//     Write a constructor for the derived class and call Create from within
//     the constructor.
//
//     CXTPEdit inherits significant functionality from CWnd. To set and retrieve
//     text from a CXTPEdit object, use the CWnd member functions SetWindowText
//     and GetWindowText, which either set or get the entire contents of an edit control
//     even if it is a multiline control. Also, if an edit control is multiline,
//     get and set part of the control's text by calling the CXTPEdit member
//     functions GetLine, SetSel, GetSel, and ReplaceSel.
//
//     If you want to handle Windows notification messages sent by an edit
//     control to its parent (usually a class derived from CDialog), add a
//     message-map entry and message-handler member function to the parent
//     class for each message.
//
//     Each message-map entry takes the following form:
//
// <code>
// ON_Notification(id, memberFxn)
// </code>
//
//          where 'id' specifies the child window ID of the edit control sending the
//          notification, and 'memberFxn' is the name of the parent member function
//          you have written to handle the notification.
//
//          The parent's function prototype is as follows:
//
// <code>
// afx_msg void memberFxn();
// </code>
//
//          Following is a list of potential message-map entries and a description
//          of the cases in which they would be sent to the parent:
//
//        * <b>ON_EN_CHANGE</b> The user has taken an action that may have altered
//          text in an edit control. Unlike the EN_UPDATE notification message,
//          this notification message is sent after Windows updates the display.
//        * <b>ON_EN_ERRSPACE</b> The edit control cannot allocate enough memory
//          to meet a specific request.
//        * <b>ON_EN_HSCROLL</b> The user clicks an edit control's horizontal scroll
//          bar. The parent window is notified before the screen is updated.
//        * <b>ON_EN_KILLFOCUS</b> The edit control loses the input focus.
//        * <b>ON_EN_MAXTEXT</b> The current insertion has exceeded the specified
//          number of characters for the edit control and has been truncated.
//          Also sent when an edit control does not have the ES_AUTOHSCROLL
//          style and the number of characters to be inserted would exceed the
//          width of the edit control. Also sent when an edit control does not
//          have the ES_AUTOVSCROLL style and the total number of lines resulting
//          from a text insertion would exceed the height of the edit control.
//        * <b>ON_EN_SETFOCUS</b> Sent when an edit control receives the input focus.
//        * <b>ON_EN_UPDATE</b> The edit control is about to display altered text.
//          Sent after the control has formatted the text but before it screens
//          the text so that the window size can be altered, if necessary.
//        * <b>ON_EN_VSCROLL</b> The user clicks an edit control's vertical scroll
//          bar. The parent window is notified before the screen is updated.
//
//     If you create a CXTPEdit object within a dialog box, the CXTPEdit object
//     is automatically destroyed when the user closes the dialog box.
//
//     If you create a CXTPEdit object from a dialog resource using the dialog
//     editor, the CXTPEdit object is automatically destroyed when the user
//     closes the dialog box.
//
//     If you create a CXTPEdit object within a window, you may also need to
//     destroy it. If you create the CXTPEdit object on the stack, it is destroyed
//     automatically. If you create the CXTPEdit object on the heap by using
//     the new function, you must call delete on the object to destroy it when
//     the user terminates the Windows edit control. If you allocate any memory
//     in the CXTPEdit object, override the CXTPEdit destructor to dispose of
//     the allocations.
//===========================================================================
class _XTP_EXT_CLASS CXTPEdit : public CXTPMaskEditT<CEdit>
{
	DECLARE_DYNAMIC(CXTPEdit)

	typedef CXTPMaskEditT<CEdit> TBase;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPEdit object
	//-----------------------------------------------------------------------
	CXTPEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPEdit object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will set the mask for the edit control.
	// Parameters:
	//     lpszMask    - The format for the mask field. For example, if
	//                   you wanted to set the mask for a phone number,
	//                   and you only wanted digits to be entered, your
	//                   mask might look like this; _T("(000) 000-0000").
	//     lpszLiteral - The literal format is entered here. Wherever you
	//                   place an underscore ('_') is where the user will
	//                   be allowed to enter data only. Using the phone
	//                   number example; _T("(___) ___-____").
	//     lpszDefault - Text that is to be displayed when the control
	//                   is initialized. For example; _T("(800) 555-1212").
	//                   If NULL, 'lpszLiteral' is used to initialize the
	//                   edit text.
	// Remarks:
	//     The values that can be set are:
	//     <TABLE>
	//          <b>Mask Character</b>  <b>Description</b>
	//          ---------------------  ------------------------
	//          0                      Numeric (0-9)
	//          9                      Numeric (0-9) or space (' ')
	//          #                      Numeric (0-9) or space (' ') or ('+') or ('-')
	//          L                      Alpha (a-Z)
	//          ?                      Alpha (a-Z) or space (' ')
	//          A                      Alpha numeric (0-9 and a-Z)
	//          a                      Alpha numeric (0-9 and a-Z) or space (' ')
	//          &                      All print character only
	//          H                      Hex digit (0-9 and A-F)
	//          X                      Hex digit (0-9 and A-F) and space (' ')
	//          >                      Forces characters to upper case (A-Z)
	//          <                      Forces characters to lower case (a-z)
	//     </TABLE>
	//-----------------------------------------------------------------------
	BOOL SetEditMask(LPCTSTR lpszMask, LPCTSTR lpszLiteral, LPCTSTR lpszDefault = NULL);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the pattern for the edit mask.
	// Input:   lpszPattern - text pattern to set.
	//-----------------------------------------------------------------------
	void SetPattern(LPCTSTR lpszPattern);

	// ------------------------------------------------------------------------
	// Summary:
	//     Determines if the clipboard contains valid information.
	// Returns:
	//     TRUE if the Clipboard contains data in a format which this edit view
	//     can accept, otherwise it returns FALSE.
	// Remarks:
	//     Call this member function to determine if the Clipboard contains
	//     information that can be pasted into this edit view.
	// ------------------------------------------------------------------------
	virtual BOOL CanPaste();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if a selection has been made.
	// Returns:
	//     TRUE if a selection has been made, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL HasSelection() const;

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to adjust the size of the area to draw
	//          the buddy control borders on.
	// Input:   pBuddy     - Points to the buddy control associated with the edit.
	//          rcUpDown   - Size of the buddy border area to be drawn.
	//          nAlignment - Side of the edit box the buddy is attached to, 
	//                       either UDS_ALIGNRIGHT or UDS_ALIGNLEFT.
	// Returns: TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	void AdjustBuddyRect(CWnd* pBuddy, CRect& rcUpDown, int nAlignemnt);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to draw the borders for the associated
	//          buddy window.
	// Input:   pBuddy     - Points to the buddy control associated with the edit.
	//          pDC        - Points to a valid device context.
	//          rc         - Size of the buddy border area to be drawn.
	//          nAlignment - Side of the edit box the buddy is attached to, 
	//                       either UDS_ALIGNRIGHT or UDS_ALIGNLEFT.
	//-----------------------------------------------------------------------
	void DrawBuddyBorders(CWnd* pBuddy, CDC* pDC, CRect rc, int nAlignemnt);

	// -----------------------------------------------------------------
	// Summary:
	//     This member is called to update color, text and other visual elements
	//     of the control.
	// -----------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the visual theme of the control.
	// Parameters:
	//     nTheme - New visual theme. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     nStyle can be one of the following:
	//     * <b>xtpControlThemeDefault</b> Standard appearance style.
	//     * <b>xtpControlThemeFlat</b> Flat appearance style.
	//     * <b>xtpControlThemeUltraFlat</b> Ultra flat appearance style.
	//     * <b>xtpControlThemeOffice2000</b> Office 2000 appearance style.
	//     * <b>xtpControlThemeOfficeXP</b> Office XP appearance style.
	//     * <b>xtpControlThemeOffice2003</b> Office 2003 appearance style.
	//     * <b>xtpControlThemeResource</b> Office 2007 appearance style.
	//     * <b>xtpControlThemeVisualStudio2012Light</b> VS 2012 Light style theme.
	//     * <b>xtpControlThemeVisualStudio2012Dark</b> VS 2012 Dark style theme.
	//-----------------------------------------------------------------------
	virtual void SetTheme(XTPControlTheme nTheme);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the style for the theme.
	// Returns: A XTPControlTheme enumerator index value representing the 
	//          theme style.
	//-----------------------------------------------------------------------
	XTPControlTheme GetThemeID();

	//-----------------------------------------------------------------------
	// Summary: Call this member to retrieve a pointer to the current theme.
	// Returns: A CXTPEditTheme object that represents the current theme.
	//-----------------------------------------------------------------------
	CXTPEditTheme* GetTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the flat style appearance of the button control.
	// Remarks:
	//     The control will appear flat until the mouse pointer moves over it,
	//     at which point it appears three-dimensional.
	// Returns:
	//     TRUE if the flat style is used, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetFlatStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the flat style appearance of the button control.
	// Parameters:
	//     bFlatStyle - TRUE to use the flat style, FALSE otherwise.
	// Remarks:
	//     The control will appear flat until the mouse pointer moves over it,
	//     at which point it appears three-dimensional.
	//-----------------------------------------------------------------------
	void SetFlatStyle(BOOL bFlatStyle = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the control is drawn using visual styles\SkinFramework, if supported.
	// Remarks:
	//     This method enables Windows XP visual styles for the control. This
	//     Control will draw with visual styles if the control and the operating
	//     system supports it.
	//     Visual styles and SkinFramwork skins are specifications for the
	//     appearance of controls.  GetUseVisualStyle determines whether to
	//     use the currently set Windows XP visual style.  If using SkinFramwork,
	//     it determines whether to use the currently set style of the SkinFramework.
	// Returns:
	//     TRUE if the currently set Windows XP visual style will be used to
	//     theme the control.  If using SkinFramwork, the currently set style
	//     of the SkinFramework will be used to skin the control.
	//
	//     FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	BOOL GetUseVisualStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables Windows XP visual styles for the control.
	// Remarks:
	//     This method enables Windows XP visual styles for the control. This
	//     Control will draw with visual styles if the control and the operating
	//     system supports it.
	//     Visual styles and SkinFramwork skins are specifications for the
	//     appearance of controls.  GetUseVisualStyle determines whether to
	//     use the currently set Windows XP visual style.  If using SkinFramwork,
	//     it determines whether to use the currently set style of the SkinFramework.
	// Parameters:
	//     bUseVisualStyle - TRUE if the currently set Windows XP visual style will be used to
	//                       theme the control.  If using SkinFramwork, the currently set style
	//                       of the SkinFramework will be used to skin the control.
	//
	//                       FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	void SetUseVisualStyle(BOOL bUseVisualStyle = TRUE);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to determine if the pattern set using
	//          SetPattern() is valid.
	// Returns: TRUE if the previously set pattern is valid.
	//-----------------------------------------------------------------------
	BOOL IsPatternValid();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to enable / disable theme border
	//          display:
	// Input:   bShowThemedBorder - TRUE to display themed borders.
	//-----------------------------------------------------------------------
	void ShowThemedBorder(BOOL bShowThemedBorder);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the edit state
	//          is highilghted, meaning the mouse cursor is currently hovering
	//          the control.
	// Returns: TRUE if the edit state is highlighted, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsHighlighted() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the edit-box has
	//          input focus.
	// Returns: TRUE if the edit-box has input focus, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary: Gets the flat style appearance of the edit control.
	// Remarks: The control will appear flat until the mouse pointer moves 
	//          over it, at which point it appears three-dimensional.
	// Returns: TRUE if the flat style is used, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsFlatStyle() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if non client borders are displayed.
	// Returns: TRUE if non-client borders are to be drawn.
	//-----------------------------------------------------------------------
	BOOL ShowBorder() const;

//{{AFX_CODEJOCK_PRIVATE
	virtual bool Initialize(CWnd* pParentWnd); // Obsolete
//}}AFX_CODEJOCK_PRIVATE

protected:
	
	//-----------------------------------------------------------------------
	// Summary: Retreives a handle to the client background brush.
	// Input:   pDC - Points to a valid device context.
	// Returns: A HBRUSH handle representing the client background brush.
	//-----------------------------------------------------------------------
	HBRUSH GetClientBrush(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to redraw the edit-control.
	//-----------------------------------------------------------------------
	void RedrawEdit();

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to redraw the focus rectangle for the
	//          edit control.
	//-----------------------------------------------------------------------
	void RedrawFocusedFrame();

	CXTPEditTheme* m_pTheme; // Points to the current edit theme.

//{{AFX_CODEJOCK_PRIVATE

	BOOL PreCreateWindow(CREATESTRUCT& cs);
	void PreSubclassWindow();
	void Init();

	//{{AFX_MSG(CXTPEdit)
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnChange();
	afx_msg void OnSysColorChange();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnSetTheme(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

	BOOL m_bPreSubclassInit; // TRUE if the control has been subclassed.
	BOOL m_bShowBorder;      // TRUE if non client borders are displayed.
	BOOL m_bHighlighted;     // TRUE when the mouse is overing over the control.
	BOOL m_bFocused;         // TRUE when the control has input focus.
	CXTPRegExp* m_pRegExp;   // Points to the CXTPRegExp object for this control.
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPEdit::IsHighlighted() const {
	return m_bHighlighted;
}
AFX_INLINE BOOL CXTPEdit::IsFocused() const {
	return m_bFocused;
}
AFX_INLINE BOOL CXTPEdit::GetFlatStyle() const {
	return IsFlatStyle();
}
AFX_INLINE BOOL CXTPEdit::ShowBorder() const {
	return m_bShowBorder;
}
AFX_INLINE void CXTPEdit::ShowThemedBorder(BOOL bShowThemedBorder) {
	m_bShowBorder = bShowThemedBorder;
}


class _XTP_EXT_CLASS CXTPEditor : public CXTPEdit
{
	DECLARE_DYNAMIC(CXTPEditor)

public:
	// -----------------------------------------------------------------
	// Summary:
	//     This member is called to update color, text and other visual elements
	//     of the control.
	// -----------------------------------------------------------------
	virtual void RefreshMetrics();
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPEDIT_H__)
