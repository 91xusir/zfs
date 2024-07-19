// DlgCreateForceField.cpp : implementation file
//

#include "stdafx.h"
#include "tool_character.h"
#include "mainfrm.h"
#include "DlgCreateForceField.h"


// CDlgCreateForceField dialog

IMPLEMENT_DYNAMIC(CDlgCreateForceField, CDialog)
CDlgCreateForceField::CDlgCreateForceField(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCreateForceField::IDD, pParent)
	, m_name(_T(""))
	, m_combo(_T(""))
{
//	m_result = NULL;
	m_firstActive = TRUE;
}

CDlgCreateForceField::~CDlgCreateForceField()
{

}

void CDlgCreateForceField::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_LIST, m_comboCon);
	DDX_CBString(pDX, IDC_LIST, m_combo);
}


BEGIN_MESSAGE_MAP(CDlgCreateForceField, CDialog)
	ON_BN_CLICKED(IDC_CREATE, OnBnClickedCreate)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CDlgCreateForceField message handlers

void CDlgCreateForceField::OnBnClickedCreate()
{
	UpdateData(TRUE);
	// if(m_name.GetLength() == 0) return;

/*
	if(g_activeActor)
	{
		CRT_ForceField *base = NULL;
		if(m_combo == "Vortex")
			base = new CRT_FFVortex;
	
		if(!base) return;
		base->m_ffName = m_name;
		m_result = base;
	}*/

	OnOK();
}

void CDlgCreateForceField::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
/*
	if(m_firstActive)
	{
		m_firstActive = false;
		int idx = 0;
		m_comboCon.InsertString(idx++,"Vortex");
		m_firstActive = false;
	}*/

}

