#pragma once
#include "afxwin.h"
//#include "rts_scene_block.h"
class CDlgNewBlock;

class CNewBlockView : public CWnd
{
public:
	friend class RtSceneBlock;
	friend struct RtsSceneBlockMap;
    CNewBlockView();
    virtual ~CNewBlockView();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnPaint();

public:
    bool GetPosition(int iX, int iY, int& iPosX, int& iPosY, bool& bDummy,int& iScene);
    void GetRect(int iPosX, int iPosY, CRect& rect,int s);

private:
    int m_iDrawStartX, m_iDrawStartY, m_iDrawStartY1;
    int m_iGridStartX, m_iGridStartY;
    int m_iGridWidth;                   // 每一个格子的大小
    int m_iGridSpaceBetween;            // 格子之间的间距
    int m_iSelectGridX, m_iSelectGridY, m_iSelectGridIsDummy;

    int m_iLastMenuX, m_iLastMenuY;
	int m_iScene;
	const char* m_cblock;
	RtSceneBlock* m_block;
	RtsSceneBlockMap* m_pBlockMap;
	
	bool m_bDrag;

public:
    CDlgNewBlock* m_pDlgNewBlock;

    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMenuBlockOpMoveTo(void);
    afx_msg void OnMenuBlockOpMiniMap(void);
    afx_msg void OnMenuBlockOpProperty(void);
    afx_msg void OnMenuBlockOpDelete(void);
	afx_msg void OnMenuBlockOpCopy(void);
	afx_msg void OnMenuBlockOpPost(void);
//	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

// CDlgNewBlock dialog

class CDlgNewBlock : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewBlock)

public:
	CDlgNewBlock(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewBlock();

// Dialog Data
	enum { IDD = IDD_DLG_NEW_BLOCK };
    virtual BOOL OnInitDialog();

protected:
	UINT nCurHPos;
	UINT nCurVPos;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CNewBlockView*  m_pView;
    CStatic         m_Attrib;
    afx_msg void OnBnClickedButtonGenerateAllMinimap();
	afx_msg void OnFileOpen();
	CScrollBar m_vscroll;
	CScrollBar m_hscroll;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
};
