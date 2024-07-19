// SceneEditorView.h : CSceneEditorView ��Ľӿ�
//


#pragma once


class CSceneEditorView : public CView
{
protected: // �������л�����
	CSceneEditorView();
	DECLARE_DYNCREATE(CSceneEditorView)

// ����
public:
	CSceneEditorDoc* GetDocument() const;

public:
    HWND m_hWndRenderWindow;
    HWND m_hWndRenderFullScreen;
    CPoint m_ptMousePos;
    CPoint m_ptMouseCurPos;

// ����
public:
    void Pause();
    void Run();

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
//	virtual void OnInitialUpdate();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    void InitRtEngine();
    void ClearRtEngine();

// ʵ��
public:
	virtual ~CSceneEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
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
    afx_msg void OnMultiobjectSelect(UINT uCmdID);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnFileNew();
	afx_msg void OnFileBack();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // SceneEditorView.cpp �ĵ��԰汾
inline CSceneEditorDoc* CSceneEditorView::GetDocument() const
   { return reinterpret_cast<CSceneEditorDoc*>(m_pDocument); }
#endif

class CSceneMusicApp : public RtThread
{
public:
	CHAR m_szMusicName[40];
	bool m_bChangeMusic;
	bool m_bloop;
	CSceneMusicApp();
	void Play(const char* filename,bool loop = false);
protected:
	virtual int 		OnRun();
};

//class RtsSceneThread : public RtThread
//{
//public:
//	void Moveto(RtgVertex3 pos);
//
//protected:
//	RtgVertex3 m_pos;
//	bool       m_bMove;
//	virtual int OnRun();
//};
//class CEditorFrame;
//extern CEditorFrame* g_pAppCase;
extern CRtgAppFrame* g_pAppCase;
extern CSceneMusicApp* g_pMusicThread;
//extern RtsSceneThread* g_pSceneThread;
extern CHAR g_szMusicName[40];
extern bool g_bChangeMusic;