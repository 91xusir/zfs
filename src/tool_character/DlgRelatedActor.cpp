// DlgRelatedActor.cpp : 实现文件
//

#include "stdafx.h"
#include "tool_character.h"
#include "DlgRelatedActor.h"

// CDlgRelatedActor 对话框

IMPLEMENT_DYNAMIC(CDlgRelatedActor, CDialog)
CDlgRelatedActor::CDlgRelatedActor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRelatedActor::IDD, pParent)
{
}

CDlgRelatedActor::~CDlgRelatedActor()
{
}

void CDlgRelatedActor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_listBox);
}


BEGIN_MESSAGE_MAP(CDlgRelatedActor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_READD, OnBnClickedButtonReadd)
END_MESSAGE_MAP()


// CDlgRelatedActor 消息处理程序

void CDlgRelatedActor::OnBnClickedButtonReadd()
{
/*
	vector<CString> path;
	if(GetMultiActorFile(path))
	{
		int i;
		// convert path to filename
		for(i=0; i<path.size(); i++)
		{
			char buf[512],name[128],ext[128];
			strcpy(buf,path[i]);
			_splitpath(buf, NULL, NULL, name, ext);
			path[i] = name;
			path[i] += ext;
		}

		if(g_activeActor)
		{
			CRT_Actor *actor = g_activeActor->m_data;
			actor->m_relatedActFile.clear();
			for(i=0; i<path.size(); i++)
				actor->m_relatedActFile.push_back((const char*)path[i]);

			m_listBox.ResetContent();
			for(int i=0; i<actor->m_relatedActFile.size(); i++)
			{
				m_listBox.InsertString(i,actor->m_relatedActFile[i].c_str());
			}
			UpdateData(FALSE);
		}
	}
*/
}

BOOL CDlgRelatedActor::OnInitDialog()
{
	CDialog::OnInitDialog();
/*
	if(g_activeActor)
	{
		CRT_Actor *actor = g_activeActor->m_data;
		for(int i=0; i<actor->m_relatedActFile.size(); i++)
		{
			m_listBox.InsertString(i,actor->m_relatedActFile[i].c_str());
		}
	}
*/

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
