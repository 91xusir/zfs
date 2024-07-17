#include "ui/rtw_ui.h"

RtwChatInputBox::RtwChatInputBox():m_pFrm_This(NULL), m_pBtn_Ok(NULL), m_pBtn_Cancel(NULL), 
m_pLab_Message(NULL), m_pEdt_Edit(NULL), m_bEventOkSended(false), m_pEdt_Title(NULL)
{
	m_WidgetType = wtChatInputBox;
}

RtwChatInputBox::~RtwChatInputBox()
{
	DROP_RTUI_OBJECT(m_pFrm_This);
	DROP_RTUI_OBJECT(m_pBtn_Ok);
	DROP_RTUI_OBJECT(m_pBtn_Cancel);
	DROP_RTUI_OBJECT(m_pLab_Message);
	DROP_RTUI_OBJECT(m_pEdt_Edit);
	DROP_RTUI_OBJECT(m_pEdt_Title);
}

bool RtwChatInputBox::Init()
{
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox], (RtwWidget**)&m_pFrm_This));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_Ok], (RtwWidget**)&m_pBtn_Ok));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_Cancel], (RtwWidget**)&m_pBtn_Cancel));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_Message], (RtwWidget**)&m_pLab_Message));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_Edit], (RtwWidget**)&m_pEdt_Edit));
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_InputBox_MessageName], (RtwWidget**)&m_pEdt_Title));
	g_workspace.AddTopRenderWidget(m_pFrm_This);

	m_pFrm_This->EvShow += RTW_CALLBACK(this, RtwChatInputBox, OnShow_This);
	m_pFrm_This->EvHide += RTW_CALLBACK(this, RtwChatInputBox, OnHide_This);
	m_pBtn_Ok->EvLClick += RTW_CALLBACK(this, RtwChatInputBox, OnLClick_Ok);
	m_pBtn_Cancel->EvLClick += RTW_CALLBACK(this, RtwChatInputBox, OnLClick_Cancel);

	return true;
}

void RtwChatInputBox::Refresh()
{
}

void RtwChatInputBox::Show(const std::string& Text, const std::string& TitleText/* = ""*/,
						   const std::string& InputText/* = ""*/, bool bBtnOK/* = true*/, bool bBtnCancel/* = true*/)
{
	static SPoint _LeftPos, _RightPos, _CenterPos;
	_LeftPos.y = _RightPos.y = _CenterPos.y = m_pBtn_Ok->getFrameRect().top;
	_LeftPos.x = m_pFrm_This->getFrameRect().left + 
		(m_pFrm_This->getFrameRect().getWidth()>>2) - (m_pBtn_Ok->getFrameRect().getWidth()>>1);
	_CenterPos.x = m_pFrm_This->getFrameRect().left +
		(m_pFrm_This->getFrameRect().getWidth()>>1) - (m_pBtn_Ok->getFrameRect().getWidth()>>1);
	_RightPos.x = m_pFrm_This->getFrameRect().right-(m_pFrm_This->getFrameRect().getWidth()>>2) - (m_pBtn_Ok->getFrameRect().getWidth()>>1);

	m_pFrm_This->SetTitleText(TitleText);
	m_pLab_Message->SetText(Text);
	m_pEdt_Edit->SetText(InputText);
	if (bBtnOK)
		m_pBtn_Ok->Show();
	else 
		m_pBtn_Ok->Hide();
	if (bBtnCancel)
		m_pBtn_Cancel->Show();
	else 
		m_pBtn_Cancel->Hide();

	m_pFrm_This->Show();

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

void RtwChatInputBox::Hide()
{
	m_pFrm_This->Hide();
}

void RtwChatInputBox::OnLClick_Ok(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	m_bEventOkSended = true;
	EvOk(pWidget, etSelect);
	Hide();
}

void RtwChatInputBox::OnLClick_Cancel(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	Hide();
}

void RtwChatInputBox::OnShow_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	g_workspace.AddModalWidget(m_pFrm_This);
	g_workspace.SetFocusWidget(m_pFrm_This);
}

void RtwChatInputBox::OnHide_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!m_bEventOkSended)
		EvCancel(pWidget, etSelect);

	EvOk.Reset();
	EvCancel.Reset();
	m_pEdt_Edit->SetPassword(false);
	g_workspace.RemoveModalWidget(m_pFrm_This->getId());
}