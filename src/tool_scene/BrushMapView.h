#if !defined(AFX_TERRAINVIEW_H__6FD5D13E_6D06_4391_A898_5B2302A5B4A7__INCLUDED_)
#define AFX_TERRAINVIEW_H__6FD5D13E_6D06_4391_A898_5B2302A5B4A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainBrushView.h : header file
//

#include "NoFlickerWnd.h"
#include "ListCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CBrushMapForm view

class CBrushMapForm : public CWnd
{
public:
	CBrushMapForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrushMapForm)

// Attributes
public:

	CListCtrlEx  m_listCtrl;
	CImageList m_imageList;

	CXTPToolBar m_wndToolBar;
    CSliderCtrl m_wndSlider;

	int m_nFilter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushMapForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBrushMapForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//afx_msg void OnFilterColor(UINT nID);
	//afx_msg void OnUpdateFilterColor(CCmdUI* pCmd);

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrushMapForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINVIEW_H__6FD5D13E_6D06_4391_A898_5B2302A5B4A7__INCLUDED_)
