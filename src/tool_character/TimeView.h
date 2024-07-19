#ifndef __CTIME_VIEW_H__
#define __CTIME_VIEW_H__

enum ETV_OP
{
	OP_MODIFY_PROP,
	OP_SELECT_KEY,
	OP_MODIFY_KEY,
	OP_ADD_KEY,
	OP_DEL_KEY,
	OP_MOVE_KEY,
	OP_COPY_KEY
};

class CTimeViewHost
{
public:
	CTimeViewHost() {}
	virtual ~CTimeViewHost() {}

	virtual const char *TVGetName() = 0;
	virtual void TVOnNotify(ETV_OP op,long param1,long param2) = 0;
	virtual int TVGetKeyNum() = 0;
	virtual int TVGetKeyFrame(int key) = 0;
};

const int LD_SOURCE_NULL = 0;
const int LD_SOURCE_BAR  = 1;
const int LD_SOURCE_VIEW = 2;
// CTimeView form view
class CTimeView : public CWnd
{
	DECLARE_DYNCREATE(CTimeView)

public:
	CTimeView();           // protected constructor used by dynamic creation
	virtual ~CTimeView();

public:
	enum CHANNEL_UNIT { UNIT_TIME, UNIT_FRAME};

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewUnitTime();
	afx_msg void OnViewUnitFrame();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	void DrawBar(CDC *dc);
	void DrawGrid(CDC *dc);
	void DrawChannel(CDC *dc);
	void DrawTimeline(CDC *dc);
	void OnChangeGridSize();
	
	void OnModifyProp();
	void OnAddKey();
	void OnModifyKey();
	void OnDelKey();

	bool IsKeyFrame(int frame);
	void SetHost(CTimeViewHost *hostanage);
	CTimeViewHost *m_host;

	CDC *m_pdcDisplayMemory;
	CBitmap *m_pBitmap;
	CFont *m_pSmallFont;
	CFont *m_pBigFont;

	int m_gridScale;
	int m_channelNum,m_channelStart,m_channelHeight,m_curChannel;
	int m_width,m_height;				// current window size
	int m_frameStart;
	int m_curFrame;
	int m_lastFrame;
	int m_pixPerFrame;
	int m_maxFrame;
	int m_validFrame;
	int m_leftPos;
    
	afx_msg void OnTimer(UINT nIDEvent);

	bool IsInViewArea(CPoint p) { return p.y>m_barHeigh && p.y<m_height; }
	bool IsInBarArea(CPoint p) { return p.y>=0 && p.y<=m_barHeigh; }
	int PointToFrame(int x);
	int FrameToScreenX(int frame);

	int m_barHeigh;
	int m_ldSource;
	float m_delta;
	CPoint m_oldViewPos;

	vector<int> m_selectedKeyVec;
	bool   m_bViewDragRect;
	CPoint m_viewDragStart;
	CPoint m_viewDragEnd;

	void ReCheckSelectKeyVec(CPoint start, CPoint end);
	bool IsSelectedKey(int frame);
	int  GetSelectedKeyNum() { return (int)m_selectedKeyVec.size(); }
	void ClearSelectedKey() { m_selectedKeyVec.clear(); }

	int PosToPix(int pos);
	int PixToPos(int pix);
	
	CPoint m_dragStart;
	CPoint m_oldMousePos;

	DWORD m_lastUpdate;
	// bar area
	void OnBarLButtonDown(UINT nFlags, CPoint point);
	void OnBarLButtonUp(UINT nFlags, CPoint point);
	void OnBarLButtonDblClk(UINT nFlags, CPoint point);
	void OnBarMouseMove(UINT nFlags, CPoint point);
	void OnBarMouseDrag(UINT nFlags, CPoint start, CPoint end);
	float m_barCenterPix;

	// view area
	void OnViewLButtonDown(UINT nFlags, CPoint point);
	void OnViewLButtonUp(UINT nFlags, CPoint point);
	void OnViewLButtonDblClk(UINT nFlags, CPoint point);
	void OnViewMouseMove(UINT nFlags, CPoint point);
	void OnViewMouseDrag(UINT nFlags, CPoint start, CPoint end);
};

#endif
