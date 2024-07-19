// XTPGalleryListBox.h
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
#if !defined(__XTPGALLERYLISTBOX_H__)
#define __XTPGALLERYLISTBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPRecentFileListItem;

class _XTP_EXT_CLASS CXTPGalleryListBoxSeparatorItem : public CXTPControlGalleryItem
{
	DECLARE_DYNAMIC(CXTPGalleryListBoxSeparatorItem);

public:
	CXTPGalleryListBoxSeparatorItem();

public:
	virtual CSize GetSize();
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);

protected:
};


class _XTP_EXT_CLASS CXTPRecentFileListBoxItem : public CXTPControlGalleryItem
{
	DECLARE_DYNAMIC(CXTPRecentFileListBoxItem);

public:
	CXTPRecentFileListBoxItem(CXTPRecentFileListItem* pRecentItem);

	BOOL GetUseShellIcon() const;
	void SetUseShellIcon(BOOL bUseShellIcon);

public:
	virtual CSize GetSize();
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);

	CXTPRecentFileListItem* GetRecentItem() const;

	virtual int GetImageIndex();
	virtual void SetImageIndex(int nImageIndex);
	
protected:
	CXTPRecentFileListItem* m_pRecentItem;
	BOOL m_bUseShellIcon; // If TRUE the icon is extracted for file extension, otherwise 
	                      // it makes an attempt to extract embedded file icon first. FALSE by default. 
};

AFX_INLINE CXTPRecentFileListItem* CXTPRecentFileListBoxItem::GetRecentItem() const {
	return m_pRecentItem;
}

AFX_INLINE BOOL CXTPRecentFileListBoxItem::GetUseShellIcon() const {
	return m_bUseShellIcon;
}

AFX_INLINE void CXTPRecentFileListBoxItem::SetUseShellIcon(BOOL bUseShellIcon) {
	m_bUseShellIcon = bUseShellIcon;
}

class _XTP_EXT_CLASS CXTPRecentFileListBoxPinItem : public CXTPControlGalleryItem
{
	DECLARE_DYNAMIC(CXTPRecentFileListBoxPinItem);

public:
	CXTPRecentFileListBoxPinItem(CXTPRecentFileListItem* pRecentItem);

public:
	virtual CSize GetSize();
	virtual BOOL OnClick();
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);

protected:
	CXTPRecentFileListItem* m_pRecentItem;

};

class _XTP_EXT_CLASS CXTPGalleryListBox : public CXTPCommandBar
{

// Construction
public:
	CXTPGalleryListBox();
	virtual ~CXTPGalleryListBox();

public:
	void RecalcLayout();
	BOOL IsVisible() const;

// Operations
public:

	CXTPControlGallery* GetGallery() const;
	CXTPControlGalleryItems* GetItems() const;

	CXTPToolTipContext* GetToolTipContext() const;

// Implementation
public:
	virtual void OnExecute();

protected:
	void FillCommandBarEntry(CDC* pDC);

	//{{AFX_VIRTUAL(CXTPGalleryListBox)
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPGalleryListBox)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CXTPControlGallery* m_pGallery;
	CXTPControlGalleryItems* m_pItems;
	CPoint m_ptLastMousePos;
};


class _XTP_EXT_CLASS CXTPRecentFileListBox : public CXTPGalleryListBox
{
public:
	CXTPRecentFileListBox();

public:
	void BuildItems();
	void BuildItems(CXTPRecentFileList* pRecentFileList);

	BOOL GetUseShellIcon() const;
	void SetUseShellIcon(BOOL bUseShellIcon);

	CRecentFileList* GetRecentFileList() const;

protected:
	BOOL m_bUseShellIcon;
};

AFX_INLINE BOOL CXTPRecentFileListBox::GetUseShellIcon() const {
	return m_bUseShellIcon;
}

AFX_INLINE CXTPControlGalleryItems* CXTPGalleryListBox::GetItems() const {
	return m_pItems;
}

AFX_INLINE CXTPControlGallery* CXTPGalleryListBox::GetGallery() const {
	return m_pGallery;
}
AFX_INLINE BOOL CXTPGalleryListBox::IsVisible() const {
	return m_hWnd && IsWindowVisible();
}
AFX_INLINE CXTPToolTipContext* CXTPGalleryListBox::GetToolTipContext() const {
	return m_pToolTipContext;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPGALLERYLISTBOX_H__)
