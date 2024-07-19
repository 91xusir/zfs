// DlgNewShader.cpp : implementation file
//
#include "stdafx.h"
#include "tool_character.h"
#include "DlgNewShader.h"
#include "mainfrm.h"
#include "character/actor.h"
#include ".\dlgnewshader.h"

// CDlgNewShader dialog
IMPLEMENT_DYNAMIC(CDlgNewShader, CDialog)
CDlgNewShader::CDlgNewShader(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewShader::IDD, pParent)
	, m_mtlName(_T(""))
	, m_combo(_T(""))
{
	
}

CDlgNewShader::~CDlgNewShader()
{

}

void CDlgNewShader::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATH, m_mtlName);
	DDX_CBString(pDX, IDC_COMBO1, m_combo);
	DDX_Control(pDX, IDC_COMBO1, m_comboCon);
}


BEGIN_MESSAGE_MAP(CDlgNewShader, CDialog)
	ON_BN_CLICKED(IDC_CREATE, OnBnClickedCreate)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgNewShader::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()
// CDlgNewShader message handlers

BOOL CDlgNewShader::OnInitDialog()
{
	m_firstActive = true;
	return CDialog::OnInitDialog();
}

void CDlgNewShader::OnBnClickedCreate()
{
	UpdateData(TRUE);
	if(m_mtlName.GetLength() == 0) return;

	if(g_activeActor)
	{
		CMainFrame *main;
		main = (CMainFrame*)AfxGetMainWnd();
		CRT_MaterialLib *lib = main->GetSelectedMtlLib();
		if(!lib) return;

		CRT_Material *mtl = NULL;
		if(m_combo == "Standard")
            mtl = new CRT_MtlStandard;
		else if(m_combo == "Mu")
			mtl = new CRT_MtlMu;
		else if(m_combo == "Blend")
			mtl = new CRT_MtlBlend;

		if(!mtl) return;
		mtl->m_poRef = 1;
		string name = m_mtlName;
		mtl->SetName(name);
		if(!lib->AddMaterial(mtl))
		{
			MessageBox("名字重复");
			delete mtl;
			return; 
		}
	}
	OnOK();
}

int CDlgNewShader::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CDlgNewShader::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if(m_firstActive)
	{
		int idx = 0;
		m_comboCon.InsertString(idx++,"Standard");
		m_comboCon.InsertString(idx++,"Mu");
		m_comboCon.InsertString(idx++,"Blend");
		m_firstActive = false;
	}
}


void CDlgNewShader::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}
