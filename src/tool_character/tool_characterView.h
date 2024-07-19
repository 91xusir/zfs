//
// tool_characterView.h : Ctool_characterView ��Ľӿ�
//
#pragma once
class Ctool_characterView : public CView
{
protected: // �������л�����
	Ctool_characterView();
	DECLARE_DYNCREATE(Ctool_characterView)

// ����
public:
	Ctool_characterDoc* GetDocument() const;

public:
    HWND m_hWndRenderWindow;
    HWND m_hWndRenderFullScreen;
    CPoint m_ptMousePos;
    CPoint m_ptMouseCurPos;
    bool   m_bShowBone;

// ����
public:
    void Pause();
    void Run();

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:

// ʵ��
public:
	virtual ~Ctool_characterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual void OnInitialUpdate();
public:
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	afx_msg void OnNewShader();
	afx_msg void OnOpenActor();
	afx_msg void OnLink();
	afx_msg void OnSaveAsShaderBase();
	afx_msg void OnSaveAsEffectShader();
	afx_msg void OnSaveAsActor();
	afx_msg void OnSaveAsSkin();
	afx_msg void OnSaveAll();
	afx_msg void OnMergeActor();
	afx_msg void OnNewEffect();
	afx_msg void OnVerFix();
    afx_msg void OnDestroy();
	afx_msg void OnSaveAsEffect();
	afx_msg void OnSaveAsMtllib();
	afx_msg void OnBnClickedAnimPlay();
	afx_msg void OnBnClickedAnimStop();
	afx_msg void OnBnClickedAnimLoop();
	afx_msg void OnBnClickedPoseConn();
	afx_msg void OnBnClickedPoseList();
    afx_msg void OnBnClickedShowLink();
	afx_msg void OnBnClickedMeshView();
	afx_msg void OnBnClickedParameter();
	afx_msg void OnOpenRelated();

	void OpenActor(bool related);
	afx_msg void OnModifyRelatedActor();
	afx_msg void OnGetRelatedRes();
	afx_msg void OnCopyEffect();
};

class CEditorFrame;
extern CEditorFrame* g_pAppCase;

#ifndef _DEBUG  // tool_characterView.cpp �ĵ��԰汾
inline Ctool_characterDoc* Ctool_characterView::GetDocument() const
   { return reinterpret_cast<Ctool_characterDoc*>(m_pDocument); }
#endif

