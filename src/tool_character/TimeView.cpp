// TimeView.cpp : implementation file
//

#include "stdafx.h"
#include "tool_character.h"
#include "TimeView.h"
#include "MainFrm.h"
#include "tool_characterDoc.h"
#include "tool_characterView.h"
#include "EditorFrame.h"

#define SELECTED_COLOR RGB(0,128,0)
#define PIX_PER_FRAME 5
#define START_FRAME -5
// CTimeView
IMPLEMENT_DYNCREATE(CTimeView, CWnd)

CTimeView::CTimeView()
{
	m_delta = 0;
	m_bViewDragRect = false;
	m_oldViewPos.x = 0;
	m_oldViewPos.y = 0;
	m_viewDragStart.x = 0;
	m_viewDragStart.y = 0;
	m_viewDragEnd.x = 0;
	m_viewDragEnd.y = 0;

	m_pdcDisplayMemory = NULL;
	m_pBitmap = NULL;
	m_pSmallFont = m_pBigFont = NULL;

	m_gridScale = 2;
	m_width = 100;
	m_height = 100;
	m_pixPerFrame = PIX_PER_FRAME*m_gridScale;
	m_curFrame = 0;
	m_maxFrame = 2000;
	m_channelHeight = 30;
	m_validFrame = 0;

	m_channelNum = 10;
	m_channelStart = 0;
	m_curChannel = 0;
	m_host = NULL;

	m_barHeigh = 20;
	m_frameStart = START_FRAME;
	m_leftPos = 0;
	m_lastUpdate = rtMilliseconds();
}

CTimeView::~CTimeView()
{
	if ( m_pdcDisplayMemory ) delete m_pdcDisplayMemory;
	if ( m_pBitmap ) delete m_pBitmap;
	if ( m_pBigFont ) delete m_pBigFont;
	if ( m_pSmallFont ) delete m_pSmallFont;
}

void CTimeView::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTimeView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	// ON_COMMAND(ID_VIEW_UNIT_TIME, OnViewUnitTime)
	// ON_COMMAND(ID_LOCK_CHANNEL, OnLockChannel)
	ON_COMMAND(ID_CHANGE_GRIDSIZE,OnChangeGridSize)
	ON_COMMAND(ID_MODIFY_CHANNEL,OnModifyProp)
	ON_COMMAND(ID_MODIFY_KEY,OnModifyKey)
	ON_COMMAND(ID_ADD_KEY,OnAddKey)
	ON_COMMAND(ID_DEL_KEY,OnDelKey)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTimeView diagnostics
#ifdef _DEBUG
void CTimeView::AssertValid() const
{
	CWnd::AssertValid();
}

void CTimeView::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG


// CTimeView message handlers
void CTimeView::OnPaint() 
{
	if(m_curFrame != g_pAppCase->m_curFrame)
	{
		m_curFrame = g_pAppCase->m_curFrame;
		if(g_activeActor)
			m_barCenterPix = m_leftPos+m_curFrame*m_pixPerFrame;
		else
			m_barCenterPix = m_leftPos+m_curFrame*m_pixPerFrame;
	}

	// device context for painting
	if(!m_pdcDisplayMemory) return;

	CPaintDC dc(this); 

	CRect rect(CPoint(0,0), CSize(m_width, m_height));
	CBrush brush(GetSysColor(COLOR_BTNFACE));

	CPen pen1(PS_SOLID, 1, RGB(150, 150, 255));
	m_pdcDisplayMemory->SelectObject(pen1);
	m_pdcDisplayMemory->SelectObject(brush);
	m_pdcDisplayMemory->Rectangle(&rect);
	DrawGrid(m_pdcDisplayMemory);
	DrawChannel(m_pdcDisplayMemory);
	DrawTimeline(m_pdcDisplayMemory);
	DrawBar(m_pdcDisplayMemory);
	dc.BitBlt(0, 0, m_width, m_height, m_pdcDisplayMemory, 0, 0, SRCCOPY);
	// Do not call CWnd::OnPaint() for painting messages
}

void CTimeView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	m_width = cx;
	m_height = cy;

	if(m_pdcDisplayMemory)
	{
		delete m_pdcDisplayMemory;
		m_pdcDisplayMemory = NULL;
		delete m_pBitmap;
		m_pBitmap = NULL;
		delete m_pSmallFont;
		m_pSmallFont = NULL;
		delete m_pBigFont;
		m_pBigFont = NULL;
	}

	CClientDC dc(this);
	// CRect rect( CPoint(0,0), CSize(cx,cy));
	m_pdcDisplayMemory = new CDC;
	m_pBitmap = new CBitmap;
	m_pSmallFont = new CFont;
	m_pBigFont = new CFont;

	m_pdcDisplayMemory->CreateCompatibleDC(&dc);
	m_pdcDisplayMemory->SetMapMode(MM_TEXT);
	m_pBitmap->CreateCompatibleBitmap(&dc, cx, cy);
	m_pdcDisplayMemory->SelectObject(m_pBitmap);
	m_pBigFont->CreateFont( m_channelHeight*2/3, m_channelHeight/3, 
		0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	m_pSmallFont->CreateFont( m_channelHeight/3*1.5, m_channelHeight/6*1.5, 
		0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	// TODO: Add your message handler code here
}

void CTimeView::SetHost(CTimeViewHost *host)
{
	if(m_host != host)
	{
		ClearSelectedKey();
	}
	m_host = host;
	InvalidateRect(NULL,FALSE);

	// if(m_hWnd)
	//	RedrawWindow();
}

// frame num -> cur screen position
int CTimeView::FrameToScreenX(int frame)
{
	return (frame-m_frameStart)*m_pixPerFrame + m_leftPos;
}

void CTimeView::DrawGrid(CDC *dc)
{
	CString string;
	
	// draw draging key frame
	if(GetSelectedKeyNum()>0 && (GetAsyncKeyState(VK_LBUTTON)&0x8000) && m_ldSource==LD_SOURCE_VIEW)
	{
		CPen pen2(PS_DOT, 1, RGB(255, 128, 0));
		dc->SelectObject(pen2);
		int ybase = (m_curChannel-m_channelStart)*m_channelHeight;
		for(int i=0; i<m_selectedKeyVec.size(); i++)
		{
			int x = FrameToScreenX(m_selectedKeyVec[i] + (m_oldViewPos.x-m_viewDragStart.x)/m_pixPerFrame);
			if(x<0) x = 0;
			dc->Draw3dRect(x-m_pixPerFrame/2, ybase+m_barHeigh+20, m_pixPerFrame, m_channelHeight, RGB(255, 128, 0), RGB(255, 128, 0));
		}
	}

	int i,len;
	CPen pen1(PS_SOLID, 1, RGB(150, 150, 255));
	dc->SelectObject(pen1);
	dc->SelectObject(m_pSmallFont);
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(RGB(0,0,200));

	dc->MoveTo(0,m_barHeigh);
	dc->LineTo(0+m_width,m_barHeigh);
	for(i=m_frameStart; i<m_maxFrame; i++)
	{
		if( FrameToScreenX(i) > m_width) break;
		len = 10;
		if(i%10==0) len = 20;
		dc->MoveTo(FrameToScreenX(i), 0+m_barHeigh);
		dc->LineTo(FrameToScreenX(i), len+m_barHeigh);
		if(i%10==0)
		{
			string.Format("%2d", i);
			dc->TextOut(m_leftPos+(i-m_frameStart)*m_pixPerFrame+3,2+m_barHeigh,string);
		}
	}
	if(g_activeActor && g_activeActor->m_data)
	{
		int endbar = FrameToScreenX(g_activeActor->m_data->m_frameNum-1);
		CPen pen3(PS_DASH, 1, RGB(150, 150, 255));
		dc->SelectObject(pen3);
		dc->MoveTo(endbar, m_barHeigh);
		dc->LineTo(endbar, m_height);
	}
}

void CTimeView::DrawChannel(CDC *dc)
{
	CPen pen(PS_SOLID, 2, RGB(0,0,0));
	dc->SelectObject(pen);
	if(m_bViewDragRect)
	{
		dc->Draw3dRect(m_viewDragStart.x, m_viewDragStart.y, m_viewDragEnd.x-m_viewDragStart.x,
			m_viewDragEnd.y-m_viewDragStart.y, RGB(0,128,0), RGB(0,128,0));
	}
	if(!m_host) return;
	
	int i,j;
	int num = 1;
	for(j=1,i=m_channelStart+1; i<=num; i++,j++)
	{
		CPen pen1(PS_DOT, 1, RGB(150,150,255));
		dc->SelectObject(pen1);
		CPoint p0(0,j*m_channelHeight + m_barHeigh + 20);
		CPoint p1(m_width, j*m_channelHeight + m_barHeigh + 20);
		dc->MoveTo(p0);
		dc->LineTo(p1);
	}

	// draw name
	dc->SelectObject(m_pBigFont);
	dc->SetBkMode(TRANSPARENT);
	for( j=0,i=m_channelStart; i<num; i++,j++ )
	{
		int ybase = i*m_channelHeight;
		CString str = m_host->TVGetName();
		/*
		if(m_curChannel==i)
		{
			dc->SetTextColor(SELECTED_COLOR);
		}
		else
		{
		*/
			dc->SetTextColor(RGB(150,150,255));
		// }
		// str.Format("channel %2d", i);
		dc->TextOut(m_leftPos+5, ybase+10+m_barHeigh+20+30, str);
	
		int keyNum = m_host->TVGetKeyNum();
		for (int k=0; k<keyNum; k++)
		{
			int frame = m_host->TVGetKeyFrame(k);
			int x = FrameToScreenX(frame);
		 	//if (m_selectedKey == frame) 
			if(IsSelectedKey(frame))		
				dc->Draw3dRect(x-m_pixPerFrame/2, ybase+m_barHeigh+20, m_pixPerFrame, m_channelHeight, SELECTED_COLOR, SELECTED_COLOR);
			else
				dc->Draw3dRect(x-m_pixPerFrame/2, ybase+m_barHeigh+20, m_pixPerFrame, m_channelHeight, RGB(150,150,255), RGB(150,150,255));
		}
	}
}

void CTimeView::DrawTimeline(CDC *dc)
{
	// draw timeline
	CPen pen(PS_DASH, 1, RGB(0, 255, 0));
	dc->SelectObject(pen);
	dc->MoveTo(FrameToScreenX(m_curFrame), 0+m_barHeigh);
	dc->LineTo(FrameToScreenX(m_curFrame), m_height+m_barHeigh);
	// CRect rect(CPoint((m_curFrame-m_frameStart)*m_pixPerFrame,0),
	//		CSize(m_pixPerFrame, m_height));
	// CBrush brush(RGB(0, 0, 128));
	// dc->SelectObject(brush);
	// dc->SetBkMode(TRANSPARENT);
	// dc->Rectangle(&rect);
}

void CTimeView::DrawBar(CDC *dc)
{
	CString str;
	if(g_activeActor)
	{
		dc->SetTextColor(RGB(0,0,0));
		dc->SelectObject(m_pSmallFont);
		str.Format("%d / %d", m_curFrame, g_activeActor->GetFrameNum()-1);
		// dc->TextOut((m_curFrame-m_frameStart)*m_pixPerFrame-20, m_barHeigh/3, str);
		dc->TextOut(m_barCenterPix-m_frameStart*m_pixPerFrame-20, m_barHeigh/4, str);
	}
	// dc->Draw3dRect((m_curFrame-m_frameStart)*m_pixPerFrame-40, 0, 80, m_barHeigh, RGB(255,255,255), RGB(0,0,0));
	dc->Draw3dRect(m_barCenterPix-m_frameStart*m_pixPerFrame-50, 0, 100, m_barHeigh, RGB(255,255,255), RGB(0,0,0));
}

BOOL CTimeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	// cs.style |= ( WS_CHILD | WS_VISIBLE);
    return CWnd::PreCreateWindow(cs);
}

int CTimeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// SetCapture();
	// TODO: Add your specialized creation code here
	SetTimer(0,100,NULL);
	// SetScrollRange(SB_VERT, 0, m_channelNum-1);
	SetScrollRange(SB_HORZ, 0, m_maxFrame-1);
	return 0;
}

void CTimeView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int pos = GetScrollPos(SB_VERT);

	switch (nSBCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			pos = nPos;
			break;
		case SB_LINEDOWN:
			pos ++;
			break;
		case SB_LINEUP:
			pos --;
			break;
		case SB_PAGEDOWN:
			pos += 3;
			break;
		case SB_PAGEUP:
			pos -= 3;
			break;
	}
	if ( pos<0 ) pos = 0;
	if ( pos>m_channelNum - 1) pos = m_channelNum - 1;
	m_channelStart = pos;
	SetScrollPos(SB_VERT, pos);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int pos = GetScrollPos(SB_HORZ);

	switch (nSBCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			pos = nPos;
			break;
		case SB_LINEDOWN:
			pos ++;
			break;
		case SB_LINEUP:
			pos --;
			break;
		case SB_PAGEDOWN:
			pos += 10;
			break;
		case SB_PAGEUP:
			pos -= 10;
			break;
	}
	if ( pos>m_maxFrame-1 ) pos = m_maxFrame-1;
	if ( pos<0 ) pos = 0;
 
	SetScrollPos(SB_HORZ, pos);
	m_frameStart = pos+START_FRAME;
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnViewUnitTime() 
{
	// m_eChannelUnit = UNIT_TIME;
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnViewUnitFrame() 
{
	// m_eChannelUnit = UNIT_FRAME;	
    // RedrawWindow();
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnModifyProp()
{
	if(!m_host) return;
	m_host->TVOnNotify(OP_MODIFY_PROP,0,0);
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnAddKey()
{
	if(!m_host) return;
	m_host->TVOnNotify(OP_ADD_KEY,m_curFrame,0);
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnDelKey()
{
	if(!m_host) return;
	for(int i=0; i<m_selectedKeyVec.size(); i++)
        m_host->TVOnNotify(OP_DEL_KEY,m_selectedKeyVec[i],0);
	ClearSelectedKey();
	InvalidateRect(NULL,FALSE);
}

void CTimeView::OnModifyKey()
{
	if(!m_host) return;
	if(GetSelectedKeyNum() < 1) return;
	
	m_host->TVOnNotify(OP_MODIFY_KEY,m_selectedKeyVec[0],0);
	InvalidateRect(NULL,FALSE);
}

bool CTimeView::IsKeyFrame(int frame)
{
	if(!m_host) return false;
	int keyNum = m_host->TVGetKeyNum();
	for(int k=0; k<keyNum; k++)
	{
		if(frame == m_host->TVGetKeyFrame(k))
		{
			return true;
		}
	}
	return false;
}

void CTimeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_dragStart = point;
	if(IsInBarArea(point))
	{
		m_ldSource = LD_SOURCE_BAR;
		OnBarLButtonDown(nFlags,point);
	}
	else if(IsInViewArea(point))
	{
		m_ldSource = LD_SOURCE_VIEW;
		OnViewLButtonDown(nFlags,point);
	}
	
	SetCapture();
	if(GetAsyncKeyState(VK_CONTROL)&0x8000  && GetAsyncKeyState(VK_MENU)&0x8000)	
		return;

    // if(!m_lockChannel)
    //    m_curChannel = (float)point.y/m_channelHeight + m_channelStart;
	
	InvalidateRect(NULL,FALSE);
	CWnd::OnLButtonDown(nFlags, point);
}

void CTimeView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	/*
	if(GetAsyncKeyState(VK_CONTROL)&0x8000  && GetAsyncKeyState(VK_MENU)&0x8000)	
		return;

	if(!m_lockChannel)
        m_curChannel = (float)point.y/m_channelHeight + m_channelStart;
	
	if(IsInBarArea(point.y))
		m_curFrame = m_frameStart + (float)point.x/m_pixPerFrame;
	
	g_pAppCase->OnSelectFrame(m_curFrame);
	*/

	if(IsInBarArea(point))
	{
		OnBarLButtonDblClk(nFlags,point);
	}
	else if(IsInViewArea(point))
	{
		OnViewLButtonDblClk(nFlags,point);
	}
	
	InvalidateRect(NULL,FALSE);
	CWnd::OnLButtonDown(nFlags, point);
}

void CTimeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// check if change grid size
	if((GetAsyncKeyState(VK_LBUTTON)&0x8000)  && (GetAsyncKeyState(VK_CONTROL)&0x8000) 
		&& (GetAsyncKeyState(VK_MENU)&0x8000))	
	{
		if(m_oldMousePos.x - point.x > 0)
			m_delta -= 0.2;
		else if(m_oldMousePos.x - point.x < 0)
			m_delta += 0.2;

		if(Abs(m_delta) > 1)
		{
			m_pixPerFrame += m_delta;
			m_delta = 0;
			if(m_pixPerFrame < 5) m_pixPerFrame = 5; 
		}
	}
	else
	{
		if(m_ldSource == LD_SOURCE_NULL)
		{
			if(IsInViewArea(point))
				OnViewMouseMove(nFlags,point);
			else if(IsInBarArea(point))
				OnBarMouseMove(nFlags,point);
		}
		else if(m_ldSource == LD_SOURCE_BAR)
		{
			OnBarMouseDrag(nFlags,m_dragStart,point);
		}
		else if(m_ldSource == LD_SOURCE_VIEW)
		{
			OnViewMouseDrag(nFlags,m_dragStart,point);
		}
	}
	m_oldMousePos = point;

	if(rtMilliseconds()-m_lastUpdate > 20)
	{
		m_lastUpdate = rtMilliseconds();
		InvalidateRect(NULL,FALSE);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CTimeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTimeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ClientToScreen(&point);
	CMenu menu;
	menu.LoadMenu(IDR_MENU_POP);
	// if(m_lockChannel)
		menu.GetSubMenu(0)->CheckMenuItem(ID_LOCK_CHANNEL,MF_BYCOMMAND|MF_CHECKED);
	// else
	//	menu.GetSubMenu(0)->CheckMenuItem(ID_LOCK_CHANNEL,MF_BYCOMMAND|MF_UNCHECKED);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	CWnd::OnRButtonDown(nFlags, point);
}

void CTimeView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
	
	if(m_ldSource==LD_SOURCE_VIEW)
		OnViewLButtonUp(nFlags, point);
	else if(m_ldSource==LD_SOURCE_BAR)
		OnBarLButtonUp(nFlags, point);

	m_ldSource = LD_SOURCE_NULL;
}

int CTimeView::PointToFrame(int x)
{
	return m_frameStart + (float)(x+m_pixPerFrame/2)/m_pixPerFrame;
}

void CTimeView::OnChangeGridSize()
{
	m_gridScale = m_gridScale%5+1;
	m_pixPerFrame = PIX_PER_FRAME*m_gridScale;
}

void CTimeView::OnTimer(UINT nIDEvent)
{
	CWnd::OnTimer(nIDEvent);
}

void CTimeView::ReCheckSelectKeyVec(CPoint start, CPoint end)
{
	int min,max;
	if(start.x < end.x)
	{
		min = start.x;
		max = end.x;
	}
	else
	{
		max = start.x;
		min = end.x;
	}
	m_selectedKeyVec.clear();

	for(int i=min; i<max; i++)
	{
		if(IsKeyFrame(PointToFrame(i)))
			m_selectedKeyVec.push_back(PointToFrame(i));
	}
}

bool CTimeView::IsSelectedKey(int frame)
{
	for(int i=0; i<m_selectedKeyVec.size(); i++)
	{
		if(m_selectedKeyVec[i]==frame)
			return true;
	}
	return false;
}

//
// new interface
//
int CTimeView::PosToPix(int pos)
{
	return pos*m_gridScale;
}

int CTimeView::PixToPos(int pix)
{
	return pix/m_gridScale;
}


// bar area
void CTimeView::OnBarLButtonDown(UINT nFlags, CPoint point)
{
	m_barCenterPix = point.x + m_frameStart*m_pixPerFrame;
	m_curFrame = PointToFrame(point.x);
	if(m_curFrame < 0) 
	{
		m_curFrame = 0;
		m_barCenterPix = 0;
	}
	g_pAppCase->OnSelectFrame(m_curFrame);
}

void CTimeView::OnBarLButtonUp(UINT nFlags, CPoint point)
{

}

void CTimeView::OnBarLButtonDblClk(UINT nFlags, CPoint point)
{

}

void CTimeView::OnBarMouseMove(UINT nFlags, CPoint point)
{

}

void CTimeView::OnBarMouseDrag(UINT nFlags, CPoint start, CPoint end)
{
	m_barCenterPix = end.x + m_frameStart*m_pixPerFrame;
	m_curFrame = PointToFrame(end.x);
	if(m_curFrame < 0) 
	{
		m_curFrame = 0;
		m_barCenterPix = 0;
	}
	g_pAppCase->OnSelectFrame(m_curFrame);
}

// view area
void CTimeView::OnViewLButtonDown(UINT nFlags, CPoint point)
{
	m_oldViewPos = point;
	m_viewDragStart = point;
	if(GetSelectedKeyNum()>0)
	{
		if(IsSelectedKey(PointToFrame(point.x)))
		{

		}
		else if(IsKeyFrame(PointToFrame(point.x)))
		{
			ClearSelectedKey();
			m_selectedKeyVec.push_back(PointToFrame(point.x));
		}
		else
		{
			// m_bViewDrag = true;
			ClearSelectedKey();
		}
	}
	else
	{
		if(!IsKeyFrame(PointToFrame(point.x)))
		{
			// m_bViewDrag = true;
		}
		else
		{
			m_selectedKeyVec.push_back(PointToFrame(point.x));
		}
	}
}

void CTimeView::OnViewLButtonUp(UINT nFlags, CPoint point)
{
	bool moved = false;
	if(m_bViewDragRect)
		ReCheckSelectKeyVec(m_viewDragStart,m_viewDragEnd);
	else if(GetSelectedKeyNum()>0 && m_host)
	{
		vector<int> newKey;
		int i;
		for(i=0; i<m_selectedKeyVec.size(); i++)
		{
			int finiaKey = m_selectedKeyVec[i]+(point.x-m_viewDragStart.x)/m_pixPerFrame;
			if(finiaKey < 0) finiaKey = 0;
			if(finiaKey != m_selectedKeyVec[i])
			{
				moved = true;
				if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
				{
					m_host->TVOnNotify(OP_COPY_KEY,m_selectedKeyVec[i],finiaKey);
					// newKey.push_back(m_selectedKeyVec[i]);
					newKey.push_back(finiaKey);
				}
				else
				{
					newKey.push_back(finiaKey);
					m_host->TVOnNotify(OP_MOVE_KEY,m_selectedKeyVec[i],finiaKey);
				}
			}
		}
		if(moved)
		{
			m_selectedKeyVec.clear();
			for(i=0; i<newKey.size(); i++)
			{
				if(IsKeyFrame(newKey[i]))
					m_selectedKeyVec.push_back(newKey[i]);
			}
		}
	}

	m_bViewDragRect = false;
	m_viewDragStart.x = 0;
	m_viewDragStart.y = 0;
	m_viewDragEnd.x = 0;
	m_viewDragEnd.y = 0;
}

void CTimeView::OnViewLButtonDblClk(UINT nFlags, CPoint point)
{
	ClearSelectedKey();
	if(IsKeyFrame(PointToFrame(point.x)))
	{
		m_selectedKeyVec.push_back(PointToFrame(point.x));
	}
	OnModifyKey();
}

void CTimeView::OnViewMouseMove(UINT nFlags, CPoint point)
{
	m_oldViewPos = point;
	m_bViewDragRect = false;
}

void CTimeView::OnViewMouseDrag(UINT nFlags, CPoint start, CPoint end)
{
	m_oldViewPos = end;
	m_viewDragStart = start;
	m_viewDragEnd   = end;
	if(m_viewDragEnd.y < m_barHeigh)
		m_viewDragEnd.y = m_barHeigh;
	if(m_viewDragEnd.y > m_height)
		m_viewDragEnd.y = m_height;

	if(GetSelectedKeyNum()==0)
	{
		m_bViewDragRect = true;
	}
}
