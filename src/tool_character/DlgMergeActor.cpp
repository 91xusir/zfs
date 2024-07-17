// DlgMergeActor.cpp : implementation file
//

#include "stdafx.h"
#include "tool_character.h"
#include "DlgMergeActor.h"
#include "MainFrm.h"

// CDlgMergeActor dialog
IMPLEMENT_DYNAMIC(CDlgMergeActor, CDialog)
CDlgMergeActor::CDlgMergeActor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMergeActor::IDD, pParent)
	, m_actor1Path(_T(""))
	//, m_actor2Path(_T(""))
	, m_bReplace(FALSE)
{

}

CDlgMergeActor::~CDlgMergeActor()
{

}

void CDlgMergeActor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ACTOR1, m_actor1Path);
	DDX_Text(pDX, IDC_ACTOR2, m_actor2Path);
	DDX_Check(pDX, IDC_REPLACE, m_bReplace);
}

BEGIN_MESSAGE_MAP(CDlgMergeActor, CDialog)
	ON_BN_CLICKED(IDC_GET_ACTOR1, OnBnClickedGetActor1)
	ON_BN_CLICKED(IDC_GET_ACTOR2, OnBnClickedGetActor2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgMergeActor message handlers
void CDlgMergeActor::OnBnClickedGetActor1()
{
	string tmp,path;
	if(GetActorFile(tmp,path))
	{
		m_actor1Path = path.c_str();
		UpdateData(FALSE);
	}
}

void CDlgMergeActor::OnBnClickedGetActor2()
{
	string tmp,path;
	if(GetMultiActorFile(m_mulitActor2Path))
	{
		m_actor2Path = m_mulitActor2Path[0];
		UpdateData(FALSE);
	}
}

bool CDlgMergeActor::MergeFile(CRT_Actor *a1, CRT_Actor *a2, bool eventOnly)
{
	if(eventOnly)
	{
		return a1->MergeEvent(a2);
	}
	else
	{
		//
		// 将第二个文件整体合并入第一个文件
		//
		// merge frame
		return a1->Merge(a2);
	}
	return false;
}

void CDlgMergeActor::OnBnClickedOk()
{
	UpdateData(TRUE);

	RtObject *ob;
	CRT_Actor *a1,*a2;

	RtArchive *ar = RtCoreFile().CreateFileReader(m_actor1Path);
	if(!ar) return;
	ob = ar->ReadObject(NULL);
	if(!ob || !ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Actor)))
	{
		MessageBox("读取人物文件出错");
		return;
	}
	a1 = (CRT_Actor*)ob;

	for(int i=0; i<m_mulitActor2Path.size(); i++)
	{
		ar = RtCoreFile().CreateFileReader(m_mulitActor2Path[i]);
		if(!ar) 
		{
			MessageBox("读取人物文件出错");
			return;
		}
		ob = ar->ReadObject(NULL);
		if(!ob || !ob->IsKindOf(RT_RUNTIME_CLASS(CRT_Actor)))
		{
			MessageBox("读取人物文件出错");
			return;
		}
		a2 = (CRT_Actor*)ob;

		if(!MergeFile(a1,a2,m_bReplace))
		{
			MessageBox("合并失败");
			delete a1;
			delete a2;
			return;
		}
	}
    	
	ar = RtCoreFile().CreateFileWriter(NULL,m_actor1Path);
	if(!ar) return;
	CRT_Actor *final;
	final = a1;
	if(!ar->WriteObject(final))
	{
		MessageBox("合并失败");
	}
	else
	{
		MessageBox("合并成功");
	}
	ar->Close();
	delete ar;
	
	delete a1;
	delete a2;
}
