// DlgSliderView.cpp : implementation file
//
#include "stdafx.h"
#include "tool_character.h"
#include "DlgSliderView.h"
#include "tool_characterDoc.h"
#include "tool_characterView.h"
#include "EditorFrame.h"
#include "dlgsliderview.h"
#include "MainFrm.h"
#include "DlgPoseConn.h"

IMPLEMENT_DYNAMIC(CDlgSliderView, CXTResizeDialog)
CDlgSliderView::CDlgSliderView(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSliderView::IDD, pParent)
	, m_frameInfo(_T(""))
{
}

CDlgSliderView::~CDlgSliderView()
{
}

void CDlgSliderView::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FRAME_INFO, m_frameInfo);
	DDX_Control(pDX, IDC_ANIM_SLIDER, m_animSlider);
	// DDX_Control(pDX, IDC_VIEW, m_view);
}


BEGIN_MESSAGE_MAP(CDlgSliderView, CXTResizeDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ANIM_PLAY, OnBnClickedAnimPlay)
	ON_BN_CLICKED(IDC_ANIM_STOP, OnBnClickedAnimStop)
	ON_BN_CLICKED(IDC_ANIM_LOOP, OnBnClickedAnimLoop)
	ON_BN_CLICKED(IDC_POSE_CONN, OnBnClickedPoseConn)
END_MESSAGE_MAP()


// CDlgSliderView message handlers
BOOL CDlgSliderView::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetResize(IDC_ANIM_SLIDER,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_ANIM_STOP,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	SetResize(IDC_ANIM_PLAY,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	SetResize(IDC_ANIM_LOOP,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	SetResize(IDC_FRAME_INFO,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_POSE_CONN,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	// SetResize(IDC_VIEW,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	
	m_frameInfo = "NULL";
	UpdateData(FALSE);
	return TRUE;
}

void CDlgSliderView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos); 
	// maxlimit = pScrollBar->GetScrollLimit();

	// Get the current position of scroll box.
	int curpos = pScrollBar->GetScrollPos();

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
		case SB_LEFT:			// Scroll to far left.
			curpos = minpos;
			break;

		case SB_RIGHT:			// Scroll to far right.
			curpos = maxpos;
			break;

		case SB_ENDSCROLL:		// End scroll.
			break;

		case SB_LINELEFT:		// Scroll left.
			if (curpos > minpos)
				curpos--;
			break;

		case SB_LINERIGHT:		// Scroll right.
			if (curpos < maxpos)
				curpos++;
			break;

		case SB_PAGELEFT:		// Scroll one page left.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos > minpos)
					curpos = max(minpos, curpos - (int) info.nPage);
			}
			break;

		case SB_PAGERIGHT:      // Scroll one page right.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos < maxpos)
					curpos = min(maxpos, curpos + (int) info.nPage);
			}
			break;

		case SB_THUMBPOSITION:	// Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;

		case SB_THUMBTRACK:		// Drag scroll box to specified position. nPos is the
			curpos = nPos;		// position that the scroll box has been dragged to.
			break;
	}

	// Set the new position of the thumb (scroll box).
	pScrollBar->SetScrollPos(curpos);
	if(g_pAppCase) 
		g_pAppCase->OnSelectFrame(curpos);
	
	m_frameInfo.Format("%d / %d",curpos,maxpos);
	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgSliderView::SetAnimRange(int min,int max,int pos)
{
	m_animSlider.SetScrollRange(min,max);
	m_animSlider.SetScrollPos(pos);
	m_frameInfo.Format("%d / %d",pos,max);
	UpdateData(FALSE);

}

void CDlgSliderView::SetCurFrame(int frame)
{
	int minpos,maxpos;
	m_animSlider.GetScrollRange(&minpos, &maxpos); 
	m_animSlider.SetScrollPos(frame);
	m_frameInfo.Format("%d / %d",frame,maxpos);
	UpdateData(FALSE);
}

void CDlgSliderView::OnBnClickedAnimPlay()
{
	if(g_activeActor)
	{
		CMainFrame *main;
		main = (CMainFrame*)AfxGetMainWnd();
		string pose = main->GetListSelectedPose();
		g_activeActor->PlayPose(pose);
	}
}

void CDlgSliderView::OnBnClickedAnimLoop()
{
	if(g_activeActor)
	{
		CMainFrame *main;
		main = (CMainFrame*)AfxGetMainWnd();
		string pose = main->GetListSelectedPose();
		g_activeActor->PlayPose(pose,true);
	}
}

void CDlgSliderView::OnBnClickedAnimStop()
{
	if(g_activeActor)
		g_activeActor->StopPose();
}

void CDlgSliderView::OnBnClickedPoseConn()
{
	CDlgPoseConn dlg;
	dlg.DoModal();
}
