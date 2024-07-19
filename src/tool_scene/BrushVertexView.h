#if !defined(AFX_BRUSH_VERTEX_VIEW_H__CD375CBC_CB75_4973_B05B_1D158BFEA6BF__INCLUDED_)
#define AFX_BRUSH_VERTEX_VIEW_H__CD375CBC_CB75_4973_B05B_1D158BFEA6BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBrushVertexForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "afxcmn.h"
#include "afxwin.h"

class RtsObject;

class CBrushVertexForm : public CDialog
{
public:
	CBrushVertexForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrushVertexForm)

// Form Data
public:
	//{{AFX_DATA(CBrushVertexForm)
	enum { IDD = IDD_DLG_BRUSH_VERTEX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	void    SetFadeOutFadeIn( bool );
	void    SetOperatorObject( RtsObject* );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushVertexForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	virtual void OnOK();
	virtual void OnCancel();
	
// Implementation
public:
	virtual ~CBrushVertexForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBrushVertexForm)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
private:
    // 内圈画刷大小
    CSliderCtrl m_minBrush;
    // 外圈刷子大小
    CSliderCtrl m_maxBrush;
    // 刷子的形状(方形或者圆形)
    int m_iBrushShape;
public:
    afx_msg void OnBnClickedRadioRectangle();
    afx_msg void OnBnClickedRadioRounded();
    afx_msg void OnBnClickedRadioBrushVertexDrag();
    afx_msg void OnBnClickedRadioBrushVertexBrush();
    afx_msg void OnBnClickedRadioBrushVertexNoise();
    afx_msg void OnBnClickedRadioBrushVertexSmooth();
//    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedRadioBrushTerrainVertex();
    afx_msg void OnBnClickedRadioBrushTerrainWalk();
    afx_msg void OnBnClickedRadioBrushTerrainFly();
    afx_msg void OnBnClickedRadioBrushTerrainDig();
    afx_msg void OnBnClickedCheckSnapToGrid();
    BOOL m_bCheckSnapToGrid;
    afx_msg void OnBnClickedRadioBrushTerrainWater();
    afx_msg void OnBnClickedRadioBrushPointTile();
    afx_msg void OnBnClickedCheckTile();
    afx_msg void OnBnClickedCheckTerrain();
    BOOL m_bEditTile;
    BOOL m_bEditTerrain;
    int m_iSelectWater;
    CComboBox m_SelectWaterCtrl;
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedRadioBrushTileVertex();
    afx_msg void OnBnClickedRadioBrushVertexDrag2();
	BOOL       m_bFadeInFadeOut;
	RtsObject* m_pkObject;
	afx_msg void OnBnClickedCheckFadeoutin();
    
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSH_VERTEX_VIEW_H__CD375CBC_CB75_4973_B05B_1D158BFEA6BF__INCLUDED_)
