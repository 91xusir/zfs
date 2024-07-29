


#include "ui_project_shell.h"
#include "ui_project.h"

const char* DEFAULT_PROJECT=UI_CHAT_ENTER_INPUT_PROJECT;
const char* DEFAULT_SHOW_PROJECT=UI_CHAT_NORMAL_SHOW_PROJECT;

CProject::CProject()
:mpInputProject(NULL),//输入模式
mpShowProject(NULL),//显示模式
m_eInputType(AllInput)
{
};

CProject::~CProject()
{
	DEL_ONE(mpInputProject);
	mpInputProject=NULL;
};

bool CProject::Init()
{
	RtString tmpStr;
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		tmpStr.Format("tbfeedback.fmchat%d.txtFeedback",i);
		CUIShowProject::mpBackGround[i]	=LOAD_UI(tmpStr.c_str())->GetBackground();
		CUIShowProject::mpBorder[i]		=LOAD_UI(tmpStr.c_str())->GetBorder();
	}

	SetInputProject();
	SetShowProject();
	return true;
}

bool CProject::Clear()
{
	RtString tmpStr;
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		LOAD_UI(tmpStr.c_str())->SetBackgroundImage(CUIShowProject::mpBackGround[i]);
		LOAD_UI(tmpStr.c_str())->SetBorder(CUIShowProject::mpBorder[i]);

		CUIShowProject::mpBackGround[i]=NULL;
		CUIShowProject::mpBorder[i]=NULL;
	}

	return true;
}

bool CProject::SetShowProject(string vShowProject)//选择显示一个配置
{
	bool Succend=true;
	//delete mpShowProject;
	//mpShowProject=NULL;

	if(vShowProject==UI_CHAT_NORMAL_SHOW_PROJECT)
	{
		if(mpShowProject)
			DEL_ONE(mpShowProject);
		mpShowProject=RT_NEW CUINormalShowProject;
	}
	else if(vShowProject==UI_CHAT_TRANSPARENT_SHOW_PROJECT)
	{
		if(mpShowProject)
			DEL_ONE(mpShowProject);
		mpShowProject=RT_NEW CUITransparentShowProject;
	}
	else if(vShowProject=="")
	{
		SetShowProject(DEFAULT_SHOW_PROJECT);
	}
	else
	{
		SetShowProject(DEFAULT_SHOW_PROJECT);
		Succend=false;
	}

	return Succend;
}



bool CProject::SetInputProject(string vInputProject)//选择一个配置
{
	bool Succend=true;
	//delete mpInputProject;
	//mpInputProject=NULL;


	//g_workspace.DelAllFocusWidget();
	if(vInputProject==UI_CHAT_DIRECT_INPUT_PROJECT)
	{
		if(mpInputProject)
			DEL_ONE(mpInputProject);
		mpInputProject=RT_NEW CUIProject1;
		m_eInputType = AllInput;
	}
	else if(vInputProject==UI_CHAT_ENTER_INPUT_PROJECT)
	{
		if(mpInputProject)
			DEL_ONE(mpInputProject);
		mpInputProject=RT_NEW CUIProject2;
		m_eInputType = EnterInput;
	}
	else if(vInputProject=="")
	{
		SetInputProject(DEFAULT_PROJECT);
	}
	else
	{
		SetInputProject(DEFAULT_PROJECT);
		Succend=false;
	}

	return Succend;
};

void CProject::KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift)
{
	if(mpInputProject!=NULL)
	{
		mpInputProject->KeyDown(vKey,vAlt,vCtrl,vShift);
	}
};

void CProject::KeyChar(int vChar)//字符信息
{
	if(mpInputProject!=NULL)
	{
		mpInputProject->KeyChar(vChar);
	}
}


