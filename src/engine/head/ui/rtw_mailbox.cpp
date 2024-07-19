#include "ui/rtw_ui.h"

RtwMailBox::RtwMailBox():
m_pFrm_This(NULL), m_pBtn_Ok(NULL), m_pBtn_Cancel(NULL), m_pLab_Message(NULL), 
m_pLab_MessageTitle(NULL), m_bEventOkSended(false){}

RtwMailBox::~RtwMailBox()
{
	DROP_RTUI_OBJECT(m_pFrm_This);
	DROP_RTUI_OBJECT(m_pLab_Message);
	DROP_RTUI_OBJECT(m_pBtn_Ok);
	DROP_RTUI_OBJECT(m_pBtn_Cancel);
	DROP_RTUI_OBJECT( m_pLab_MessageTitle );
}

bool RtwMailBox::Init()
{
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_MessageBox], (RtwWidget**)&m_pFrm_This));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_MessageBox_Ok], (RtwWidget**)&m_pBtn_Ok));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_MessageBox_Cancel], (RtwWidget**)&m_pBtn_Cancel));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_MessageBox_Message], (RtwWidget**)&m_pLab_Message));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_MessageBoxTitle], (RtwWidget**)&m_pLab_MessageTitle));
	g_workspace.AddTopRenderWidget(m_pFrm_This);

	m_pFrm_This->EvShow += RTW_CALLBACK(this, RtwMailBox, OnShow_This);
	m_pFrm_This->EvHide += RTW_CALLBACK(this, RtwMailBox, OnHide_This);
	m_pBtn_Ok->EvLClick += RTW_CALLBACK(this, RtwMailBox, OnLClick_Ok);
	m_pBtn_Cancel->EvLClick += RTW_CALLBACK(this, RtwMailBox, OnLClick_Cancel);

	return true;
}

void RtwMailBox::Refresh()
{
}

void RtwMailBox::Show(const std::string& Text, const std::string& TitleText/* = ""*/, bool bBtnOK/* = true*/, bool bBtnCancel/* = true*/)
{
	static SPoint _LeftPos, _RightPos, _CenterPos;
	_LeftPos.y = _RightPos.y = _CenterPos.y = m_pBtn_Ok->getFrameRect().top;
	_LeftPos.x = m_pFrm_This->getFrameRect().left + (m_pFrm_This->getFrameRect().getWidth()>>2) - (m_pBtn_Ok->getFrameRect().getWidth()>>1);
	_CenterPos.x = m_pFrm_This->getFrameRect().left + (m_pFrm_This->getFrameRect().getWidth()>>1) - (m_pBtn_Ok->getFrameRect().getWidth()>>1);
	_RightPos.x = m_pFrm_This->getFrameRect().right - (m_pFrm_This->getFrameRect().getWidth()>>2) - (m_pBtn_Ok->getFrameRect().getWidth()>>1);

	//m_pFrm_This->SetTitleText(TitleText);
	if( m_pLab_MessageTitle )
	{
		if( TitleText.size() ) 
		{
			m_pLab_MessageTitle->Show();
			m_pLab_MessageTitle->SetText( TitleText );
		}else
		{
			m_pLab_MessageTitle->Hide();
		}
	}
	m_pLab_Message->SetText(Text);
	if (bBtnOK)
		m_pBtn_Ok->Show();
	else 
		m_pBtn_Ok->Hide();
	if (bBtnCancel)
		m_pBtn_Cancel->Show();
	else 
		m_pBtn_Cancel->Hide();

	m_pFrm_This->ShowModal();

	if (m_pBtn_Ok->getVisible())
	{
		if (m_pBtn_Cancel->getVisible())
		{
			m_pBtn_Ok->Move(_LeftPos, false);
			m_pBtn_Cancel->Move(_RightPos, false);
		}
		else 
			m_pBtn_Ok->Move(_CenterPos, false);
	}
	else 
	{
		if (m_pBtn_Cancel->getVisible())
			m_pBtn_Cancel->Move(_CenterPos, false);
	}

	m_bEventOkSended = false;
}

void RtwMailBox::SetText(const std::string& strMsg)
{
	m_pLab_Message->SetText(strMsg);
	return;
}

std::string RtwMailBox::getText()
{
	return m_pLab_Message->getText();
}


void RtwMailBox::Hide()
{
	m_pFrm_This->Hide();
}

void RtwMailBox::OnLClick_Ok(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	m_bEventOkSended = true;
	EvOk(pWidget, etSelect);
	Hide();
}

void RtwMailBox::OnLClick_Cancel(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
//	EvCancel(pWidget, etSelect);
	Hide();
}

void RtwMailBox::OnShow_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	g_workspace.AddModalWidget(m_pFrm_This);
	g_workspace.SetFocusWidget(m_pFrm_This);
}

void RtwMailBox::OnHide_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!m_bEventOkSended)
		EvCancel(m_pBtn_Cancel, etSelect);
	EvOk.Reset();
	EvCancel.Reset();
	g_workspace.RemoveModalWidget(m_pFrm_This->getId());
}

void RtwMailBox::InsertImage(const char* key)
{

}

void RtwMailBox::SetReadOnly(bool bRead)
{

}