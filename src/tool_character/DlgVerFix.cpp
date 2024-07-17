// DlgVerFix.cpp : implementation file
//
#include "stdafx.h"
using namespace std;
#include "character/actor.h"
#include "tool_character.h"
#include "DlgVerFix.h"
#include ".\dlgverfix.h"


// CDlgVerFix dialog
void CDlgVerFix::PtWavConvert(const char *name)
{
	m_msg.Format("fix %s ...",name);
	UpdateData(FALSE);
	RedrawWindow();
	FILE *file = fopen(name,"rb");
	if(!file) return;
	
	string tmp = name;
	tmp[tmp.length()-4] = '@';
	tmp[tmp.length()-3] = '.';
	tmp[tmp.length()-2] = 'b';
	tmp[tmp.length()-1] = 'm';
	tmp.push_back('p');

	FILE *tar = fopen(tmp.c_str(),"w+b");
	if(!tar) return;

	char tag[] = "bm";
	long size = 0;
	long reserved = 0;
	long offset = 54;
	
	// header
	fwrite("BM",1,2,tar);
    
	// size
	fseek(file, 0, SEEK_END);
	long iFileLength = ftell(file);
	fwrite(&iFileLength,1,4,tar);

	// reserved
	fwrite(&reserved,1,4,tar);

	// offset
	fwrite(&offset,1,4,tar);

	fseek(file,14,SEEK_SET);
	char buf[1024];
	while(1)
	{
		int cnt  = fread(buf,1,1024,file);
		// LOG1("read %d bytes\n",cnt);
		int ret = fwrite(buf,1,cnt,tar);
		// LOG1("write %d bytes\n",ret);
		if(cnt != 1024)
            break;
	}
	fclose(file);
	fclose(tar);

	// delete old file
	DeleteFile(name);
}


IMPLEMENT_DYNAMIC(CDlgVerFix, CDialog)
CDlgVerFix::CDlgVerFix(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVerFix::IDD, pParent)
	, m_msg(_T(""))
{

}

CDlgVerFix::~CDlgVerFix()
{
}

void CDlgVerFix::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MSG, m_msg);
}


BEGIN_MESSAGE_MAP(CDlgVerFix, CDialog)
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
END_MESSAGE_MAP()


// CDlgVerFix message handlers
void CDlgVerFix::SearchDirectory(string base,string sub)
{
	WIN32_FIND_DATA data;
	HANDLE h;

	string tmp;
	string full = base + "\\" + sub + "\\*.*";
	RtCoreLog().Info("SearchDirectory: dir = %s\n",full.c_str());
	h = FindFirstFile(full.c_str(),&data);
	if(h == INVALID_HANDLE_VALUE)
	{
		return;
	}

	while(FindNextFile(h,&data))
	{
		if(strcmp(data.cFileName,".")==0 || strcmp(data.cFileName,"..")==0)
			continue;

		full = base + "\\" + sub + "\\" + data.cFileName;
		if(sub.empty())
			tmp = data.cFileName;
		else
			tmp  = sub + "\\" + data.cFileName;
		if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			SearchDirectory(base,tmp.c_str());
			continue;
		}

		int len = strlen(data.cFileName);
		if(data.cFileName[len-5] == '@') continue;
		if(!(data.cFileName[len-1] == 'p' && data.cFileName[len-2] == 'm'
			&& data.cFileName[len-3] == 'b') &&
		   !(data.cFileName[len-1] == 'P' && data.cFileName[len-2] == 'M'
			&& data.cFileName[len-3] == 'B'))
		{
			continue;
		}
		PtWavConvert(full.c_str());
		/*
		if(!(data.cFileName[len-1] == 't' && data.cFileName[len-2] == 'c'
			&& data.cFileName[len-3] == 'a'))
		{
			continue;
		}
		LOG1("SearchDirectory: get file %s\n",data.cFileName);
		
		// do version fix
		VerFix(data.cFileName);
		// pkt.AddFile(full.c_str(),tmp.c_str());
		*/
	}
}

void CDlgVerFix::VerFix(const char *name)
{
	/*
	m_msg.Format("fix %s ...",name);
	UpdateData(FALSE);
	RedrawWindow();
	CRT_ActorInstance *actor = LoadActorInstance(name);
	if(actor)
	{
		actor->Create(g_pDevice);
		actor->Save(NULL);
	}
	*/
}

void CDlgVerFix::OnBnClickedStart()
{
	m_totalNum = 0;
	m_fixOk = 0;
	m_fixFailed = 0;
	SearchDirectory("actor","");
	MessageBox("Fix ok");
}

