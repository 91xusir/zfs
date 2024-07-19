
#pragma once

#include "NoFlickerWnd.h"

class RtSceneTool;
struct RtsAreaHelper;

class CAreaListForm : public CWnd
{
public:
	CAreaListForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAreaListForm)
    virtual ~CAreaListForm();

    void SetScene(RtSceneTool* pScene)  { m_pSceneTool=pScene; }

    void UpdateAreaList();

    void AddArea(RtsAreaHelper* pArea);
    void RemoveArea(RtsAreaHelper* pArea);
    void SelectArea(RtsAreaHelper* pArea);

// Attributes
public:

	CNoFlickerWnd<CListBox>     m_listBox;

    CNoFlickerWnd<CEdit>        m_wndNameEdit;
    CNoFlickerWnd<CEdit>        m_wndDataEdit;

    CNoFlickerWnd<CButton>      m_btnUpdate;

	CXTPToolBar                 m_wndToolBar;

// Operations

public:
    BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaListForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    bool HitTestSplitter(CPoint pt);
    void OnInvertTracker(CRect rc);
    void Reposition(int cx, int cy);

    HCURSOR     m_hCursor;
    COLORREF    m_clrFace;
    COLORREF    m_clrShadow;
    COLORREF    m_clrHelpBack;
    COLORREF    m_clrHelpFore;
    int         m_nHelpHeight;
    CRect		m_rectTracker;
    RtSceneTool*m_pSceneTool;


	// Generated message map functions
protected:
	//{{AFX_MSG(CAreaListForm)
    afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnListBoxDoubleClick();
    afx_msg void OnListBoxSelectChange();
    afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
    afx_msg void OnEditNameKillFocus();
    afx_msg void OnEditDataKillFocus();
    afx_msg void OnToobarEnable();
    afx_msg void OnUpdateUIToobarEnable(CCmdUI* pCmdUI);
    afx_msg void OnToobarSaveFile();
    afx_msg void OnToobarLoadFile();
    afx_msg void OnUpdateUIToobarAreaOperate(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
