#if !defined(AFX_BRUSH_COLOR_VIEW_H__CD375CBC_CB75_4973_B05B_1D158BFEA6BF__INCLUDED_)
#define AFX_BRUSH_COLOR_VIEW_H__CD375CBC_CB75_4973_B05B_1D158BFEA6BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBrushColorForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CBrushColorForm : public CDialog
{
public:
	CBrushColorForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrushColorForm)

// Form Data
public:
	//{{AFX_DATA(CBrushColorForm)
	enum { IDD = IDD_DLG_BRUSH_COLOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushColorForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	virtual void OnOK();
	virtual void OnCancel();

// Implementation
public:
	virtual ~CBrushColorForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBrushColorForm)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSH_COLOR_VIEW_H__CD375CBC_CB75_4973_B05B_1D158BFEA6BF__INCLUDED_)
