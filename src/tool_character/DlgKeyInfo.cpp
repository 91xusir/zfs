// DlgKeyInfo.cpp : implementation file
//

#include "stdafx.h"
#include "tool_character.h"
#include "DlgKeyInfo.h"
#include "character/actor.h"

// CDlgKeyInfo dialog

IMPLEMENT_DYNAMIC(CDlgKeyInfo, CDialog)
CDlgKeyInfo::CDlgKeyInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgKeyInfo::IDD, pParent)
{
}

CDlgKeyInfo::~CDlgKeyInfo()
{
}

void CDlgKeyInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgKeyInfo, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


// CDlgKeyInfo message handlers
BOOL CDlgKeyInfo::OnInitDialog()
{
	m_bRebuild = false;
	SetTimer(0,100,NULL);
	CDialog::OnInitDialog();
	return TRUE;
}

int CDlgKeyInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_grid.Create(CRect(0, 0, 100, 100), this, 0);
	m_grid.SetOwner(this);
	m_pRoot = m_grid.AddCategory(ID_KEY_INFO);
	
	RtObject *ob = (RtObject*)m_object;
	if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material)))
	{
		((CRT_Material*)m_object)->AttachKeyGrid(m_key,m_pRoot);
	}
	else if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect)))
	{
		((CRT_Effect*)m_object)->AttachKeyGrid(m_key,m_pRoot);
	}
	return 0;
}

void CDlgKeyInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_grid.MoveWindow(0, 0, cx, cy);
	m_grid.Invalidate(FALSE);
}

void CDlgKeyInfo::Init(int key,void *p)
{
	m_object = p;
	m_key = key;
}

LRESULT CDlgKeyInfo::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
    RtObject *ob = (RtObject*)m_object;
	if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material)))
	{
		CRT_Material *mtl = (CRT_Material*)m_object;
		if(mtl->OnKeyGridNotify(m_key,wParam,lParam))
		{
			m_bRebuild = true;
		}
	}
	else if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect)))
	{
		CRT_Effect *eft = (CRT_Effect*)m_object;
		if(eft->OnKeyGridNotify(m_key,wParam,lParam))
		{
			m_bRebuild = true;
		}
	}
	return 0;
}

void CDlgKeyInfo::OnTimer(UINT nIDEvent)
{
	RtObject *ob = (RtObject*)m_object;
	if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Material)))
	{
		CRT_Material *mtl = (CRT_Material*)m_object;
		if(m_bRebuild)
		{
			m_grid.ResetContent();
			m_pRoot = m_grid.AddCategory(ID_KEY_INFO);
			mtl->AttachKeyGrid(m_key,m_pRoot);
			m_bRebuild = false;
		}
	}
	else if(ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Effect)))
	{
		CRT_Effect *eft = (CRT_Effect*)m_object;
		if(m_bRebuild)
		{
			m_grid.ResetContent();
			m_pRoot = m_grid.AddCategory(ID_KEY_INFO);
			eft->AttachKeyGrid(m_key,m_pRoot);
			m_bRebuild = false;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
