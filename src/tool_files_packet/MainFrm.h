#pragma once
//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	MainFrm.h
//	author:		ldr123
//	purpose:	
//********************************************************************

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


