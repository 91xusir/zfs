#include "gc_include.h"
#include "ui_layer.h"
#include "UIForm_Server.h"

CUIForm_Server::CUIForm_Server(void):
m_iSelect(-1)
{
	guard;
	m_pfrmServer					= RTW_WIDGET("fmserver");
	m_plstServerContent				= RTW_WIDGET_T(RtwListBox,"fmserver.fmxmm.lbxlogin");
	m_plbSelectServer				= RTW_WIDGET("fmserver.fmxmm.lbtext");
	m_pbtnOK						= RTW_WIDGET_T(RtwButton,"fmserver.fmxmm.btnok");

	m_plstServerContent->EvSelect	+= RTW_CALLBACK(this,CUIForm_Server,OnSelectServer);
	m_plstServerContent->EvLDClick  += RTW_CALLBACK(this,CUIForm_Server,OnBtnOKDown);
	m_pbtnOK->EvLClick				+= RTW_CALLBACK(this,CUIForm_Server,OnBtnOKDown);
	unguard;
}

CUIForm_Server::~CUIForm_Server(void)
{
	guard;
	unguard;
}

void CUIForm_Server::Show()
{
	guard;
	unguard;
}

void CUIForm_Server::Hide()
{
	guard;
	m_pfrmServer->Hide();
	unguard;
}

void CUIForm_Server::Refresh()
{
	guard;
	m_plstServerContent->RemoveAllItems();
	unguard;
}

void CUIForm_Server::IsVisible()
{
	guard;
	unguard;
}

void CUIForm_Server::OnSelectServer(RtwWidget *sender,void *e)
{
	guard;
	int i = m_plstServerContent->GetSelectedItem();
	switch(i)
	{
	case -1:
		break;
	default:
		m_iSelect = i;
	}
	m_plbSelectServer->SetText(m_plstServerContent->GetItemText(i,0));
	unguard;
}

void CUIForm_Server::OnInsertNewServer(const char* servername,int ping,char* serverstate, const RtwRect& imgRc)
{
	guard;
	static int Index = 0;

	m_plstServerContent->AddItem();//添加一个空行
	m_plstServerContent->SetItemText(Index,0,servername,RtwPixel(0xffffffff),SUiTextAlignment(alignCenter,alignCenter));
	char strtmp[5];
	memset(strtmp,0,5);
	/*if(ping >=0 && ping < 9999)
	{
		sprintf(strtmp,"%d",ping);
	}else
		memcpy(strtmp,"极高",sizeof("极高"));

	m_plstServerContent->SetItemText(Index,1,strtmp,RtwPixel(0xffffffff),SUiTextAlignment(alignCenter,alignCenter));*/
	m_plstServerContent->SetItemText(Index,2,serverstate);



	static RtwImage* pImage	= g_workspace.getImageFactory()->createImage("ui\\textures\\10.tga");
	RtwRect imgTmp		= imgRc;
	imgTmp.left			-= 16;
	imgTmp.top			-= 6;
	imgTmp.right		+= 11;
	imgTmp.bottom		+= 5;
	pImage->SetSourceRect(imgTmp);
	pImage->SetBlend(true);
	m_plstServerContent->SetLineItemImage(Index, 2, pImage);
	unguard;
}

void CUIForm_Server::OnBtnOKDown(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	// 保存选中的服务器
	RtIni iniUser;
	if (!iniUser.OpenFile(R(INI_USER), true))
	{
		return;
	}

	if (GetSelectItem() >= 0)
	{
		if (!iniUser.FindSection("server")) iniUser.AddSection("server");
		if (!iniUser.SetEntry("server", "select", (long)m_iSelect))
		{
			iniUser.AddEntry("server", "select", (long)m_iSelect);
		}
	}else
	{
		iniUser.DelEntry("server", "select");
	}

	iniUser.CloseFile();

	g_layerLogin->OnClicked_EnterLogin(sender, e);
	unguard;
}

void CUIForm_Server::SetSelectItemLast(int iLastSelect)
{
	guard;
	if(iLastSelect != -1 && iLastSelect <= m_plstServerContent->getLineCount())
		m_plstServerContent->SetSelect(iLastSelect);
	unguard;
}

RtwWidget*	CUIForm_Server::GetServerWidget()
{
	guard;
	return m_pfrmServer;
	unguard;
}
int CUIForm_Server::GetMaxLineNum()
{
// 	guard;
	return m_plstServerContent->getLineCount();
// 	unguard;
}

int CUIForm_Server::GetSelectItem()
{
	guard;
	return m_iSelect;
	unguard;
}
