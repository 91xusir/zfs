// XTPDockingPaneCaptionButton.h
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
#if !defined(__XTPDOCKINGPANECAPTIONBUTTON_H__)
#define __XTPDOCKINGPANECAPTIONBUTTON_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneCaptionButton is a stand alone internal class.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneCaptionButton
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneCaptionButton object
	// Parameters:
	//     nID   - If of the caption button.
	//     pPane - Pane that the caption button controls.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton(int nID, CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the button's rectangle.
	// Parameters:
	//     rc - Button's rectangle.
	//-----------------------------------------------------------------------
	void SetRect (CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw the Pin button.
	// Parameters:
	//     pDC     - Pointer to the device context in which to draw.
	//     pt      - Center point.
	//     rc      - Button rectangle.
	//     bPinned - TRUE if the button is pinned.
	//     clr     - Button color. COLORREF_NULL by default, in this case
	//               current text color is used.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawPinnButton(CDC* pDC, CPoint pt, BOOL bPinned, COLORREF clr = COLORREF_NULL);
	static void AFX_CDECL DrawPinnButton(CDC* pDC, CRect rc, BOOL bPinned, COLORREF clr = COLORREF_NULL); // <combine CXTPDockingPaneCaptionButton::DrawPinnButton@CDC*@CPoint@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw the Maximize\Restore button.
	// Parameters:
	//     pDC     - Pointer to the device context in which to draw.
	//     pt      - Center point.
	//     rc      - Button rectangle.
	//     bMaximize - TRUE if the button is to be drawn maximized, FALSE to
	//                 draw restore button.
	//     clr     - Button color. COLORREF_NULL by default, in this case
	//               current text color is used.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawMaximizeRestoreButton(CDC* pDC, CPoint pt, BOOL bMaximize, COLORREF clr = COLORREF_NULL);
	static void AFX_CDECL DrawMaximizeRestoreButton(CDC* pDC, CRect rc, BOOL bMaximize, COLORREF clr = COLORREF_NULL); // <combine CXTPDockingPaneCaptionButton::DrawMaximizeRestoreButton@CDC*@CPoint@BOOL@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw the Close button.
	// Parameters:
	//     pDC     - Pointer to the device context in which to draw.
	//     rc      - Button rectangle.
	//     clr     - Button color. COLORREF_NULL by default, in this case
	//               current text color is used.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawCloseButton(CDC* pDC, CRect rc, COLORREF clr = COLORREF_NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw the Menu button.
	// Parameters:
	//     pDC     - Pointer to the device context in which to draw.
	//     rc      - Button rectangle.
	//     clr     - Button color. COLORREF_NULL by default, in this case
	//               current text color is used.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawMenuButton(CDC* pDC, CRect rc, COLORREF clr = COLORREF_NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to click button.
	// Parameters:
	//     pWnd    - Parent window of button.
	//     pt      - Initial mouse position.
	//     bClient - TRUE if <i>pt</i> belongs client area of window.
	//-----------------------------------------------------------------------
	DWORD Click (CWnd* pWnd, CPoint pt, BOOL bClient = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the bounding rectangle
	// Returns:
	//     A CRect object
	//-----------------------------------------------------------------------
	CRect GetRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine the state of the caption button.
	//     This will tell you if the pin caption button is visible or
	//     if it is pushed.
	// Returns:
	//     1 if the the button is visible, 2 if it is pushed, 3 if it
	//     is both visible and pushed.
	// See Also: XTPDockingPanePinState
	//-----------------------------------------------------------------------
	DWORD GetState() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the state of the pin caption button.
	// Parameters:
	//     dwState - XTPDockingPanePinState of pin button.
	// See Also: XTPDockingPanePinState
	//-----------------------------------------------------------------------
	void SetState(DWORD dwState);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves button identifier
	// Returns:
	//     Button identifier. Standard values are
	//     * <b>XTP_IDS_DOCKINGPANE_CLOSE</b> Close button
	//     * <b>XTP_IDS_DOCKINGPANE_AUTOHIDE</b> Auto-hide button
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the pane that the pin button controls.
	// Returns:
	//     Pane that the pin button controls.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the point belongs to button.
	// Parameters:
	//     pt - Point to test.
	// Returns:
	//     TRUE if point belongs to the button; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL PtInRect(POINT pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to refresh styles of the button.
	// Parameters:
	//     pt - Mouse cursor pointer.
	//-----------------------------------------------------------------------
	BOOL CheckForMouseOver (CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to redraw button
	//-----------------------------------------------------------------------
	void InvalidateRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the button is pushed.
	// Returns:
	//     TRUE is the button is pushed, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsPressed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the button mouse hover the button
	// Returns:
	//     TRUE if mouse hover the button.
	//-----------------------------------------------------------------------
	BOOL IsSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the pin caption button is visible.
	// Returns:
	//     TRUE if the pin button is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if button is enabled
	// Returns:
	//     TRUE if button is enabled
	//-----------------------------------------------------------------------
	BOOL IsEnabled() const;

protected:
	CRect m_rcButton;             // Bounding rectangle of the button
	CXTPDockingPaneBase* m_pPane; // Parent pane
	int m_nID;                    // Identifier of the button.
	BOOL m_bPressed;              // TRUE if button is pressed by user.
	BOOL m_bSelected;             // TRUE if user move mouse cursor inside button rectangle
	DWORD m_dwState;              // XTPDockingPanePinState of caption button.

	friend class CXTPDockingPaneTabbedContainer;
	friend class CXTPDockingPaneMiniWnd;

private:
	static COLORREF AFX_CDECL NormalizeButtonColor(CDC* pDC, COLORREF clr);
};

AFX_INLINE CRect CXTPDockingPaneCaptionButton::GetRect() const {
	return m_rcButton;
}

AFX_INLINE DWORD CXTPDockingPaneCaptionButton::GetState() const {
	return m_dwState;
}

AFX_INLINE void CXTPDockingPaneCaptionButton::SetState(DWORD dwState) {
	m_dwState = dwState;
}

AFX_INLINE int CXTPDockingPaneCaptionButton::GetID() const {
	return m_nID;
}

AFX_INLINE CXTPDockingPaneBase* CXTPDockingPaneCaptionButton::GetPane() const {
	return m_pPane;
}

AFX_INLINE BOOL CXTPDockingPaneCaptionButton::PtInRect(POINT pt) const {
	return m_rcButton.PtInRect(pt) != 0;
}

AFX_INLINE BOOL CXTPDockingPaneCaptionButton::IsPressed() const {
	return m_bPressed;
}

AFX_INLINE BOOL CXTPDockingPaneCaptionButton::IsSelected() const {
	return m_bSelected;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPDOCKINGPANECAPTIONBUTTON_H__)
