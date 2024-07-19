// DlgNpc.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "DlgNpc.h"
#include ".\dlgnpc.h"


// CDlgNpc dialog

IMPLEMENT_DYNAMIC(CDlgNpc, CDialog)
CDlgNpc::CDlgNpc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNpc::IDD, pParent)
	, m_SerialNum(_T(""))
	, m_NpcName(_T(""))
	, m_NpcFuncName(_T(""))
	, m_szModelId(_T(""))
	, m_szPosx(_T(""))
	, m_Posy(_T(""))
	, m_Dirx(_T(""))
	, m_Diry(_T(""))
	, m_szScript(_T(""))
	, m_szDialog(_T(""))
	, m_szAtten(_T(""))
	, m_szVertion(_T(""))
{
    //CDialog(CDlgNpc::IDD, pParent);
	//int p = 0;

}

CDlgNpc::~CDlgNpc()
{
}

void CDlgNpc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	/*
	DDX_Control(pDX, IDC_EDIT1, m_SerialNum);
	DDX_Control(pDX, IDC_EDIT2, m_NpcName);
	DDX_Control(pDX, IDC_EDIT3, m_NpcFuncName);
	DDX_Control(pDX, IDC_EDIT4, m_ModelId);
	DDX_Control(pDX, IDC_EDIT6, m_Posx);
	DDX_Control(pDX, IDC_EDIT7, m_PosY);
	//DDX_Control(pDX, IDC_EDIT8, m_PosZ);
	DDX_Control(pDX, IDC_EDIT9, m_Dirx);
	DDX_Control(pDX, IDC_EDIT10, m_Diry);
	//DDX_Control(pDX, IDC_EDIT11, m_Dirz);
	DDX_Control(pDX, IDC_EDIT12, m_ScriptEdit);
	DDX_Control(pDX, IDC_EDIT13, m_DialogEdit);
	DDX_Control(pDX, IDC_EDIT14, m_AttenEdit);
	DDX_Control(pDX, IDC_EDIT15, m_VertionEdit);
	*/
	DDX_Text(pDX, IDC_EDIT1, m_SerialNum);
	DDX_Text(pDX, IDC_EDIT2, m_NpcName);
	DDX_Text(pDX, IDC_EDIT3, m_NpcFuncName);
	DDX_Text(pDX, IDC_EDIT4, m_szModelId);
	DDX_Text(pDX, IDC_EDIT6, m_szPosx);
	DDX_Text(pDX, IDC_EDIT7, m_Posy);
	DDX_Text(pDX, IDC_EDIT9, m_Dirx);
	DDX_Text(pDX, IDC_EDIT10, m_Diry);
	DDX_Text(pDX, IDC_EDIT12, m_szScript);
	DDX_Text(pDX, IDC_EDIT13, m_szDialog);
	DDX_Text(pDX, IDC_EDIT14, m_szAtten);
	DDX_Text(pDX, IDC_EDIT15, m_szVertion);
}


BEGIN_MESSAGE_MAP(CDlgNpc, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNpc message handlers

void CDlgNpc::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
	//保存每个NPC的信息

}
