// MainFrame.h : CMainFrame ��Ľӿ�
//

#pragma once
#include "BrushGrassForm.h"
class CMainFrame : public CXTPFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:
	CXTPDockingPaneManager  m_paneManager;
    CXTPDockingPaneLayout*  m_pPaneLayout;

	CWnd*                   m_pWndBrushMapView;
	CTreeCtrl               m_wndBrushObjectView;
	CXTPPropertyGrid        m_wndBrushPropertyGrid;
	CBrushVertexForm        m_wndBrushVertexView;
	CBrushColorForm         m_wndBrushColorView;
    CBrushGrassForm         m_wndBrushGrassView;
    CAreaListForm           m_wndAreaView;
	//tim.yang  ·���б�����
    CPathPointListView      m_wndPathPointView;

	CImageList              m_ilBrushObjectView;

    std::map<RtString, RtString>    m_mapBrushObject;

// ����
public:
	CWnd* CreateBrushMapView();
	CWnd* CreateBrushObjectView();
	CWnd* CreateBrushPropertyView();
	CWnd* CreateBrushVertexView();
    CWnd* CreateBrushGrassView();
	CWnd* CreateBrushColorView();
    CWnd* CreateAreaListView();
	//tim.yang  ����·���б���ͼ
    CWnd* CreatePathPointListView();

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CXTPStatusBar  m_wndStatusBar;

// ���ɵ���Ϣӳ�亯��  
//OnToolOutPutNpcInfo: jiang modify
protected:
    //{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnBuildSM();
    afx_msg void OnBuildSM2();
    afx_msg void OnShowNormal();
    afx_msg void OnEnablePE();
    afx_msg void OnSaveCurrBlock();
	afx_msg void OnShowView(UINT);
	afx_msg void OnUpdateUIBrushView(CCmdUI* pCmdUI);
	afx_msg void OnThemesOffice2000();
	afx_msg void OnUpdateThemesOffice2000(CCmdUI* pCmdUI);
	afx_msg void OnThemesOfficeVisio();
	afx_msg void OnUpdateThemesOfficeVisio(CCmdUI* pCmdUI);
	afx_msg void OnThemesOfficexp();
	afx_msg void OnUpdateThemesOfficexp(CCmdUI* pCmdUI);
	afx_msg void OnThemesOffice2003();
	afx_msg void OnUpdateThemesOffice2003(CCmdUI* pCmdUI);
	afx_msg void OnThemesNativexptheme();
	afx_msg void OnUpdateThemesNativexptheme(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLockGameView();
    afx_msg void OnHeightmapView();
    afx_msg void OnToolTgaReverse();
    afx_msg void OnToolRecomputeObjectLight();
    afx_msg void OnToolRecomputeObjectBelong();
    afx_msg void OnToolCheckSameObject();
    afx_msg void OnToolDeleteSameObject();
	afx_msg void OnViewCenter();
    afx_msg void OnToolSnapShotView();
    afx_msg void OnToolMoveObject();
    afx_msg void OnToolOriginallyObjectSize();
    afx_msg void OnToolAdjustObject();
    afx_msg void OnTreeCtrlSelectChanged(NMHDR * pNotifyStruct, LRESULT * result);
    afx_msg void OnToolLoadEnv();
    afx_msg void OnToolSaveEnv();
    afx_msg void OnToolAddAreaFromFile();
	afx_msg void OnToolOutPutNpcInfo();                                                         
    afx_msg void OnNewBlock();
    afx_msg void OnShowMask(UINT nID);
    afx_msg void OnUpdateShowMask(CCmdUI* pCmdUI);
    afx_msg void OnClose();
    //}}AFX_MSG  
    afx_msg int OnCreateCommandBar(LPCREATEBARSTRUCT lpCreatePopup);
    afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	DECLARE_MESSAGE_MAP()
};


