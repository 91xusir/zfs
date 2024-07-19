// DlgPoseConn.cpp : 实现文件
//

#include "stdafx.h"
#include "tool_character.h"
#include "DlgPoseConn.h"
#include ".\dlgposeconn.h"
#include "MainFrm.h"

// CDlgPoseConn 对话框

IMPLEMENT_DYNAMIC(CDlgPoseConn, CDialog)
CDlgPoseConn::CDlgPoseConn(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPoseConn::IDD, pParent)
{
}

CDlgPoseConn::~CDlgPoseConn()
{
}

void CDlgPoseConn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_POSE1, m_pose1);
	DDX_Control(pDX, IDC_LIST_POSE2, m_pose2);
}


BEGIN_MESSAGE_MAP(CDlgPoseConn, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgPoseConn 消息处理程序

BOOL CDlgPoseConn::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(!g_activeActor || !g_activeActor->m_data) return TRUE;
	CRT_Actor *cha = g_activeActor->m_data;

	// get pose info
	TPoseMap::iterator it;
	int i=0;
	for(it=cha->m_poseMap.begin();it!=cha->m_poseMap.end();it++)
	{
		SRT_Pose *pose = &(*it).second;
        m_pose1.InsertString(i,pose->Name.c_str());
        m_pose2.InsertString(i++,pose->Name.c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgPoseConn::OnBnClickedOk()
{
	if(!g_activeActor) return;

	CString tmp;
	int i;
	i = m_pose1.GetCurSel();
	if(i != LB_ERR)
	{
		m_pose1.GetText(i,tmp);
		g_activeActor->m_pose1 = tmp;	
	}

	i = m_pose2.GetCurSel();
	if(i != LB_ERR)
	{
		m_pose2.GetText(i,tmp);
		g_activeActor->m_pose2 = tmp;	
	}
	OnOK();
}
