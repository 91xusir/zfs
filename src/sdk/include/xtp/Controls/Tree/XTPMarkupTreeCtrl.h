// XTPMarkupTreeCtrl.h
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
#if !defined(__MARKUPTREECTRL_H__) && defined(_XTP_INCLUDE_MARKUP)
#define __MARKUPTREECTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

// ----------------------------------------------------------------------
// Summary: Tree control class that allows adding markup items.
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPMarkupTreeCtrl : public CXTPTreeCtrl, public CXTPMarkupContext
{
public:
	// ----------------------------------------------------------------------
	// Summary: Constructs markup tree control.
	// ----------------------------------------------------------------------
	CXTPMarkupTreeCtrl();

	// ----------------------------------------------------------------------
	// Summary: Handles markup tree control deallocation.
	// ----------------------------------------------------------------------
	virtual ~CXTPMarkupTreeCtrl();

public:

	// ----------------------------------------------------------------------
	// Summary: Obtains markup context associated with makrup tree.
	// Return: Markup context pointer associated with makrup tree.
	// ----------------------------------------------------------------------
	CXTPMarkupContext* GetMarkupContext();

	// ----------------------------------------------------------------------
	// Summary: Adds a new markup item.
	// Parameters:
	//  lpszItem - Pointer to a NULL terminated string that contains either
	//             plain item text or markup text to display.
	//  nImage - Zero-based normal image index.
	//  nSelectedImage - Zero-based selected image index.
	//  hParent - Parent item handle.
	//  hInsertAfter - Item handle to insert a new item after.
	// Returns: A newly inserted item handle or NULL if failed.
	// ----------------------------------------------------------------------
	HTREEITEM AddItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);

	//{{AFX_VIRTUAL(CXTPMarkupTreeCtrl)
	virtual void PreSubclassWindow();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
	//}}AFX_VIRTUAL

protected:
	// ----------------------------------------------------------------------
	// Summary: Draws markup item.
	// Paramters: lpLVCD - Pointer to NMTVCUSTOMDRAW structure.
	// ----------------------------------------------------------------------
	void DrawMarkupItem(NMTVCUSTOMDRAW* lpLVCD);

	// ----------------------------------------------------------------------
	// Summary: Obtains plain markup item text, i.e. text without markup elements.
	// Parameters: pUIElement - Pointer to markup element for which to
	//                          obtain plain text.
	// Returns: Plain markup text.
	// ----------------------------------------------------------------------
	CString GetPlainMarkupItemText(CXTPMarkupUIElement* pUIElement) const;

	// ----------------------------------------------------------------------
	// Summary: Obtains item text extent. Takes into account control's font.
	// Parameters: 
	//  pDC - Control's device context pointer.
	//  lpszText - Pointer to NULL terminated string for which to compute size information.
	//  nCount - Number of characters in a string pointed by lpszText.
	// Returns: Size of text in device units.
	// ----------------------------------------------------------------------
	CSize GetItemTextExtent(CDC* pDC, LPCTSTR lpszText, int nCount) const;

	// ----------------------------------------------------------------------
	// Summary: Peforms item hit test.
	// Parameters: point - Point in client coordinates where to perform hit test.
	// Returns: Item handle that is located at the pointed coordinates or NULL if no item found.
	// ----------------------------------------------------------------------
	HTREEITEM HitTestMarkupItem(CPoint point);

	//{{AFX_MSG(CXTPMarkupTreeCtrl)
	afx_msg void OnDeleteItem (NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetInfoTip (NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomDraw (NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

	CXTPToolTipContext::CMarkupToolTip *m_pToolTip;

	mutable CXTPFont m_xtpFontDefaultGui;
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntDefaultGui, m_xtpFontDefaultGui, GetDefaultGuiFontHandle);
};


AFX_INLINE CXTPMarkupContext* CXTPMarkupTreeCtrl::GetMarkupContext() {
	return this;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__MARKUPTREECTRL_H__)
