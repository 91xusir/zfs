
#pragma once

#include "NoFlickerWnd.h"
#include "scene/rts_path_find.h"
class RtSceneTool;

class CPathPointListView : public CWnd 
{
public:
	CPathPointListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPathPointListView)
	virtual ~CPathPointListView();
	void SetScene(RtSceneTool* pScene)  { m_pSceneTool=pScene; }
	void Reposition(int cx, int cy);//设置左边list窗口
	
	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

	void LoadWaypointFile(LPCTSTR szFileName);
	void SaveWaypointFile(LPCTSTR szFileName);
	void OnMouseLDown(int x, int y);
	void RenderLinks(RtgCamera& inCamera, RtgDevice& inDevice);
	void RenderTips( RtgDevice& inDevice);
	struct WAY_EDGE
	{
		CPoint	m_ptStartTile;
		CPoint  m_ptEndTile;
	};
private:
	void AddOrRemoveLink(int x0, int y0, int x1, int y1);
	void AddLink(int x0, int y0, int x1, int y1);
	void RemoveLink(int x, int y);
	void AddWayPoint(int x, int y);
	// Attributes
public:
	CNoFlickerWnd<CListBox>     m_listBox;
	CNoFlickerWnd<CEdit>        m_wndNameEdit;
	CNoFlickerWnd<CEdit>        m_wndDataEdit;
	CNoFlickerWnd<CButton>      m_btnUpdate;
	CXTPToolBar                 m_wndToolBar;
	//jin.wang 生成路点
	RtsPathFind m_PathFind;
	// Operations
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaListForm)
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
	HCURSOR     m_hCursor;
	COLORREF    m_clrFace;
	COLORREF    m_clrShadow;
	COLORREF    m_clrHelpBack;
	COLORREF    m_clrHelpFore;
	int         m_nHelpHeight;
	CRect		m_rectTracker;
	RtSceneTool*m_pSceneTool;

	//
	// way point
	//
	BOOL		m_bStartPoint;
	INT			m_nLastTileX;
	INT			m_nLastTileY;
	std::vector<WAY_EDGE> m_vtEdges;
	// Generated message map functions
protected:
	//{{AFX_MSG(CAreaListForm)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToobarEnable();
	afx_msg void OnToobarSaveFile();
	afx_msg void OnToobarLoadFile();
	afx_msg void OnUpdateUIToobarAreaOperate(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
