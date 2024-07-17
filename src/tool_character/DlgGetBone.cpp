// DlgGetBone.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "tool_character.h"
#include "DlgGetBone.h"
#include "MainFrm.h"

// CDlgGetBone �Ի���

IMPLEMENT_DYNAMIC(CDlgGetBone, CDialog)
CDlgGetBone::CDlgGetBone(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGetBone::IDD, pParent)
	, m_selectedBone(_T(""))
{

}

CDlgGetBone::~CDlgGetBone()
{

}

void CDlgGetBone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_LBString(pDX, IDC_BONE_LIST, m_selectedBone);
	DDX_Control(pDX, IDC_BONE_LIST, m_boneList);
}

BEGIN_MESSAGE_MAP(CDlgGetBone, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDlgGetBone::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(!g_activeActor || !g_activeActor->m_data) return TRUE;
	CRT_Actor *cha = g_activeActor->m_data;
	for(int i=0;i<cha->m_bones.size();i++)
	{
		SRT_Bone *bone = &cha->m_bones[i];
		m_boneList.InsertString(i,bone->Name.c_str());
	}
	return TRUE;
}

// CDlgGetBone ��Ϣ�������
void CDlgGetBone::OnBnClickedOk()
{
	UpdateData(TRUE);
	OnOK();
}
