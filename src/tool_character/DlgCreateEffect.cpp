// DlgCreateEffect.cpp : implementation file
//
#include "stdafx.h"
#include "tool_character.h"
#include "DlgCreateEffect.h"
#include "mainfrm.h"
#include "dlgcreateeffect.h"
// CDlgCreateEffect dialog

IMPLEMENT_DYNAMIC(CDlgCreateEffect, CDialog)
CDlgCreateEffect::CDlgCreateEffect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCreateEffect::IDD, pParent)
	, m_combo(_T(""))
	, m_eftName(_T(""))
{

}

CDlgCreateEffect::~CDlgCreateEffect()
{

}

void CDlgCreateEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboCon);
	DDX_CBString(pDX, IDC_COMBO1, m_combo);
	DDX_Text(pDX, IDC_NAME, m_eftName);
}

BEGIN_MESSAGE_MAP(CDlgCreateEffect, CDialog)
	ON_BN_CLICKED(IDC_CREATE, OnBnClickedCreate)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()
// CDlgCreateEffect message handlers

BOOL CDlgCreateEffect::OnInitDialog()
{
	m_firstActive = true;
	return CDialog::OnInitDialog();
}

void CDlgCreateEffect::OnBnClickedCreate()
{
	UpdateData(TRUE);
	// if(m_eftName.GetLength() == 0) return;

	if(g_activeActor)
	{
		
		CRT_Effect *base = NULL;
		if(m_combo == "Emitter")
			base = new CRT_EffectEmitter;
		else if(m_combo == "Ribbon")
			base = new CRT_EffectRibbon;
        else if(m_combo == "Ribbon2")
            base = new CRT_EffectRibbon2;
        else if(m_combo == "Ribbon Dynamic")
            base = new CRT_Effect_Dynamic_Ribbon;
		else if(m_combo == "Blur")
			base = new CRT_EffectBlur;
/*
		else if(m_combo == "Projector")
			base = new CRT_EffectProjector;*/

		/*
		else if(m_combo == "Light")
			base = new CRT_EffectLight;
		else if(m_combo == "Ribbon2")
			base = new CRT_EffectRibbon2;
		*/
		if(!base) return;
		// base->m_poRef = 1;
		base->SetName(m_eftName);

		// Modified by Wayne Wong 2010-11-09
		//base->Init(RtGetRender(),g_activeActor->m_inst);
		base->Create();
		base->LinkActor(g_activeActor->m_inst);

		//g_activeActor->m_data->m_eftList.push_back(base);
		g_activeActor->m_inst->m_effectList.push_back(base);
		// tmp->Init(g_pDevice,g_activeActor->m_cha);
		// CMainFrame *main;
		// main = (CMainFrame*)AfxGetMainWnd();
		// main->CreateGridActor(true);
	}
	OnOK();
}

void CDlgCreateEffect::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if(m_firstActive)
	{
		m_firstActive = false;
		int idx = 0;
		m_comboCon.InsertString(idx++,"Emitter");
		m_comboCon.InsertString(idx++,"Ribbon");
        m_comboCon.InsertString(idx++,"Ribbon2");
        m_comboCon.InsertString(idx++,"Ribbon Dynamic");
		// m_comboCon.InsertString(idx++,"Glow");
		m_comboCon.InsertString(idx++,"Blur");
		//m_comboCon.InsertString(idx++,"Projector");
		// m_comboCon.InsertString(idx++,"Light");
		m_firstActive = false;
	}
}
