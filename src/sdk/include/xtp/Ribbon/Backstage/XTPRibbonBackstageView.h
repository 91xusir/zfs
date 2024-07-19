// XTPRibbonBackstageView.h: interface for the CXTPRibbonBackstageView class.
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
#if !defined(__XTPRIBBONBACKSTAGEVIEW_H__)
#define __XTPRIBBONBACKSTAGEVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPRibbonControlSystemButton;
class CXTPRibbonBackstageCommand;
class CXTPRibbonBackstageTab;
class CXTPRibbonBackstageMenuSeparator;
class CXTPRibbonBackstagePaintManager;
class CXTPCommandBarScrollBarCtrl;

const UINT XTP_BS_TABCHANGED = 0x1100;

// The style of backstage
enum XTPRibbonBackstageStyle
{
	xtpRibbonBackstageOffice2010Style, // Office 2010 like style
	xtpRibbonBackstageOffice2013Style, // backstage style like in Office 2013 and Office 2016
	xtpRibbonBackstageOffice2016Style = xtpRibbonBackstageOffice2013Style
};

// This enum defines what should be done for certain pressed key and the current tip level
enum XTPRibbonBackstageKeyboardTipResult
{
	xtpNoActionForThisLevel = -3, // nothing to do for the current level
	xtpExecuteCommand = -2, // pressed key corresponds to a command and tips of first level are shown
	xtpHideTips = -1, // hide all tips
	xtpShowSecondLevel = 2, // show tips with two letters with the same first letter
	xtpSelectTab = 3 // pressed key corresponds to a tab and tips of first level are shown
};

// Defines the current keyboard tip show level
enum XTPRibbonBackstageKeyboardTipLevel
{
	xtpNoTips = -1, // tips are not shown
	xtpUserHoldAltKey = 0, // user pressed and hold ALT key and previous level was xtpNoTips
	xtpFirstLevel = 1, // Show tips of first level (commands and tabs)
	xtpSecondLevel = 2, // show tips with two letters with the same first letter (commands and tabs)
	xtpTipsOnTabPane = 3 // Show tips for buttons on the currently active tab
};

//===========================================================================
// Summary:
//     CXTPRibbonBackstageView is CDialog derived class used for Backstage parent frame
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonBackstageView : public CXTPCommandBar
{
	DECLARE_XTP_COMMANDBAR(CXTPRibbonBackstageView);

protected:
	CXTPRibbonBackstageView();
	~CXTPRibbonBackstageView();

public:
	static CXTPRibbonBackstageView* AFX_CDECL CreateBackstageView(CXTPCommandBars* pCommandBars);



public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the currently active tab.
	//-------------------------------------------------------------------------
	CXTPRibbonBackstageTab* GetActiveTab() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the active tab.
	//-------------------------------------------------------------------------
	void SetActiveTab(CXTPRibbonBackstageTab *pTab);

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the currently focused control
	//-------------------------------------------------------------------------
	CXTPControl* GetFocusedControl() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the focused control
	//-------------------------------------------------------------------------
	void SetFocusedControl(CXTPControl *pFocusedControl);

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the currently highlighted control
	//-------------------------------------------------------------------------
	CXTPControl* GetHighlightedControl() const;


	//-------------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if pControl is a Backstage Command
	//-------------------------------------------------------------------------
	BOOL IsCommand(CXTPControl* pControl) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if pControl is a Backstage Separator
	//-------------------------------------------------------------------------
	BOOL IsMenuSeparator(CXTPControl* pControl) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if pControl is the active Tab of the Backstage view
	//-------------------------------------------------------------------------
	BOOL IsActiveTab(CXTPControl *pControl) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if pControl is the control with the focus input
	//-------------------------------------------------------------------------
	BOOL IsFocusedControl(CXTPControl *pControl) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if the mouse hovers pControl
	//-------------------------------------------------------------------------
	BOOL IsHighlightedControl(CXTPControl *pControl) const;


	CXTPImageManager* GetImageManager() const;


	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the control which contains the point
	//-------------------------------------------------------------------------
	CXTPControl* HitTest(CPoint point) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the index of pControl in the array of control in Backstage view
	//-------------------------------------------------------------------------
	int IndexOf(CXTPControl* pControl) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Executes the action difined by pControl
	//-------------------------------------------------------------------------
	void OnExecute(CXTPControl* pControl);


	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the pointer to the pane window of the currently active Tab
	//-------------------------------------------------------------------------
	CWnd* GetActiveControlPane() const;


	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the width of the menu bar of the Backstage View
	//-------------------------------------------------------------------------
	int GetMenuWidth() const;


	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the theme of the backstage view
	//-------------------------------------------------------------------------
	void SetTheme(XTPPaintTheme theme);

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the style of the backstage view.
	//		See defenition of XTPRibbonBackstageStyle
	//-------------------------------------------------------------------------

	void SetBackstageStyle(XTPRibbonBackstageStyle style = xtpRibbonBackstageOffice2016Style);
	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the style of the backstage view.
	//		See defenition of XTPRibbonBackstageStyle
	//-------------------------------------------------------------------------
	XTPRibbonBackstageStyle GetBackstageStyle() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the color of the rectangle in which the keyboard tip symbol(s) is(are) drawn
	//-------------------------------------------------------------------------
	COLORREF GetKeyboardTipsBackColor();

	//-------------------------------------------------------------------------
	// Summary:
	//     Returns the font for drawing keyboard tips
	//-------------------------------------------------------------------------
	CFont* GetKeyboardTipsFont();

	//-------------------------------------------------------------------------
	// Summary:
	//		Defines what should be done for the current keyboard input depending on the current state of
	//		keyboard tips engine.
	// Parameters:
	//		keyMessage	- the code of the Windows keyboard message
	//		altPressed	- TRUE if the key Alt was rassed
	//		key			- virtual code of the pressed key
	//		repeat		- the count of repeating key pressings
	//	Returns TRUE if keyboard input does not require further processing
	//-------------------------------------------------------------------------
	BOOL ProcessKeyboardInput(UINT keyMessage, BOOL altPressed, UINT key, int repeat); // It defines whether keyboard tip engine should be activated and what should be done for pressed keys

	BOOL IsDPIIconsScallingEnabled();

	//-------------------------------------------------------------------------
	// Summary:
	//     Overrides CWnd::GetScrollBarCtrl virtual method
	//-------------------------------------------------------------------------
	virtual CScrollBar* GetScrollBarCtrl(int nBar);

public:
	CXTPRibbonBackstageCommand* AddCommand(UINT nID);
	CXTPRibbonBackstageTab* AddTab(CWnd* pWnd, UINT nID);
	CXTPRibbonBackstageMenuSeparator* AddMenuSeparator(UINT nID = -1);

public:
	virtual void OnCancel();

protected:
	virtual void RecalcLayout();

	BOOL Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst = FALSE);
	virtual BOOL SetTrackingMode(int bMode, BOOL bSelectFirst = TRUE, BOOL bKeyboard = FALSE);

	void ShowBackstageView();

	//-----------------------------------------------------------------------
	// Summary:
	//      Gurantees that keyboard tips are set for all menu items on backstage.
	//      It also checks that keyboard tips do not conflict each other.
	//-----------------------------------------------------------------------
	void CheckKeyboardTips();

	//-----------------------------------------------------------------------
	// Summary:
	//      Gurantees that keyboard tips are set for all buttons in all tab 
	//      panes in the baclstage view.
	//-----------------------------------------------------------------------
	void CheckTabsKeyboardTips();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines which control in the backstage corresponds 
	//     to pressed keyboard key (according to keyboard key) and which action
	//     corresponds to this key and the current SHOW KEYBOARD TIPS level
	// Parameters:
	//     key           - virtual code of pressed key
	//     nControlIndex - returning value. The index of the control corresponding
	//                     pressed key in the array of controls in backstage.
	// Returns:
	//     The enum value defining what should be done for pressed key and current
	//     SHOW KEYBOARD TIPS level
	// See Also: XTPRibbonBackstageKeyboardTipResult and XTPRibbonBackstageKeyboardTipLevel
	//-----------------------------------------------------------------------
	XTPRibbonBackstageKeyboardTipResult TranslateKeyboardTip(UINT key, int &nControlIndex);


protected:
	DECLARE_MESSAGE_MAP();

	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnMouseLeave();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

protected:
#ifdef _XTP_ACTIVEX
//{{AFX_CODEJOCK_PRIVATE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPRibbonBackstageView);

	afx_msg LPDISPATCH OleAddTab(long nId, LPCTSTR lpszCaption, OLE_HANDLE hWnd);
	afx_msg LPDISPATCH OleAddCommand(long nId, LPCTSTR lpszCaption);
	afx_msg LPDISPATCH OleAddMenuSeparator();
	afx_msg int OleGetStyle();
	afx_msg void OleSetStyle(int NewValue);
	afx_msg void OleClose();
	afx_msg void OleSetTheme(long nTheme);
	afx_msg COLORREF OleGetColorForeground() const;
	afx_msg COLORREF OleGetColorBackground() const;

//}}AFX_CODEJOCK_PRIVATE
#endif
protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Updates scrollbar information
	//-------------------------------------------------------------------------
	void UpdateScrollInfo();

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets new position of the vertical scrollbar
	//-------------------------------------------------------------------------
	void SetScrollPos(int nScrollPos);
	void RepositionControls(int dy);
public:
	int m_nMenuWidth;

	CXTPFont m_xtpFontCommand;
	CXTPFont m_xtpFontTab;

	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntCommand, m_xtpFontCommand, GetCommandFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntTab, m_xtpFontTab, GetTabFontHandle);

	COLORREF m_clrText;
	COLORREF m_clrBack;
	XTPPaintTheme m_themeCurrent;
	CXTPCommandBarScrollBarCtrl m_wndScrollBar;

	CXTPRibbonBackstagePaintManager *m_pPaintManager;

	CXTPRibbonControlSystemButton *m_pSystemButton;
	CXTPRibbonBackstageTab *m_pActiveTab;
	CXTPControl *m_pHighlightedControl;
	CXTPControl *m_pFocusedControl;
	CArray<HWND, HWND> m_arrChildWindows;
	CList<int, int> m_listControlsToBeVisible;
protected:
	int m_nScrollPos;
	int m_nPageSize;
	int m_nMax;
	int m_ScrolLine;
	BOOL m_bIsScrollBarVisible;
	CRect m_rcOldPaneRect;
	XTPRibbonBackstageStyle m_backStageStyle;
	BOOL m_bRestoreQuickAccess;
	CRect m_rcOldSysButtonRect;
	XTPRibbonBackstageKeyboardTipLevel m_nKeyboardTipsLevel; // See XTPRibbonBackstageKeyboardTipLevel definition
	TCHAR m_chSecondLevelCurrentChar; // It is used when keyboard tips with 2 letters are shown (m_nKeyboardTipsLevel == xtpSecondLevel). It defines first character in such tips
};

AFX_INLINE CXTPImageManager* CXTPRibbonBackstageView::GetImageManager() const
{
	return CXTPCommandBar::GetImageManager();
}

AFX_INLINE int CXTPRibbonBackstageView::GetMenuWidth() const
{
	return m_nMenuWidth;
}

AFX_INLINE XTPRibbonBackstageStyle CXTPRibbonBackstageView::GetBackstageStyle() const
{
	return m_backStageStyle;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPRIBBONBACKSTAGEVIEW_H__)
