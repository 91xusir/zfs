// DlgChannelInfo.cpp : implementation file
#include "stdafx.h"
#include "tool_character.h"
#include "DlgChannelInfo.h"

// CDlgChannelInfo dialog
IMPLEMENT_DYNAMIC(CDlgChannelInfo, CDialog)
CDlgChannelInfo::CDlgChannelInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChannelInfo::IDD, pParent)
{

}

CDlgChannelInfo::~CDlgChannelInfo()
{
}

void CDlgChannelInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChannelInfo, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgChannelInfo message handlers
BOOL CDlgChannelInfo::OnInitDialog()
{
	m_bRebuild = false;
	SetTimer(0,100,NULL);
	CDialog::OnInitDialog();
	return TRUE;
}

int CDlgChannelInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_grid.Create(CRect(0, 0, 100, 100), this, 0);
	m_grid.SetOwner(this);
	m_pRoot = m_grid.AddCategory(ID_CHANNEL_INFO);
	
	RtObject *ob = (RtObject*)m_object;
	if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material)))
	{
		((CRT_Material*)m_object)->AttachPropGrid(m_pRoot);
	}
	else if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect)))
	{
		((CRT_Effect*)m_object)->AttachPropGrid(m_pRoot);
	}
	return 0;
}

void CDlgChannelInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_grid.MoveWindow(0, 0, cx, cy);
	m_grid.Invalidate(FALSE);
}

void CDlgChannelInfo::Init(void *p)
{
	m_object = p;
}

LRESULT CDlgChannelInfo::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
    RtObject *ob = (RtObject*)m_object;
	if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material)))
	{
		CRT_Material *mtl = (CRT_Material*)m_object;
		if(mtl->OnPropGridNotify(wParam,lParam))
		{
			m_bRebuild = true;
		}
	}
	else if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect)))
	{
		CRT_Effect *eft = (CRT_Effect*)m_object;
		if(eft->OnPropGridNotify(wParam,lParam))
		{
			m_bRebuild = true;
		}
	}
	return 0;
}

void CDlgChannelInfo::OnTimer(UINT nIDEvent)
{
	RtObject *ob = (RtObject*)m_object;
	if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material)))
	{
		CRT_Material *mtl = (CRT_Material*)m_object;
		if(m_bRebuild)
		{
			m_grid.ResetContent();
			m_pRoot = m_grid.AddCategory(ID_CHANNEL_INFO);
			mtl->AttachPropGrid(m_pRoot);
			m_bRebuild = false;
		}
	}
	else if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect)))
	{
		CRT_Effect *eft = (CRT_Effect*)m_object;
		if(m_bRebuild)
		{
			SCROLLINFO info; 
			// m_grid.GetGridView().GetScrollInfo(SB_VERT,&info);
			m_grid.ResetContent();
			m_pRoot = m_grid.AddCategory(ID_CHANNEL_INFO);
			eft->AttachPropGrid(m_pRoot);
			// m_grid.GetGridView().SetScrollInfo(SB_VERT,&info);
			// m_grid.GetGridView().RedrawWindow;
			// m_grid.RedrawWindow();
			// m_grid.OnVScroll(SB_THUMBPOSITION,info.nPos,NULL);
			m_bRebuild = false;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
