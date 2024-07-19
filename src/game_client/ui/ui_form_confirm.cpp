#include "rtw_form.h"
#include "gc_include.h"
#include "ui_form_confirm.h"

UIFormMsg::UIFormMsg(FormType t, bool autoDelete)
: m_autoDelete(autoDelete)
{
	guard;

	m_pFrmThis = RTW_WIDGET_MANAGER()->Load("ui/ui_form_confirm_layer.xml", true);

	m_pFrmThis->EvClose += RTW_EVENT(this, UIFormMsg, OnClose);

	m_pBtnOk	 = (RtwButton*)(m_pFrmThis->GetChildWidget("btnconfirm"));

	m_pBtnOk->EvLClick += RTW_EVENT(this, UIFormMsg, OnLClick_OK);

	m_pBtnCancel = (RtwButton*)m_pFrmThis->GetChildWidget("btncancel");

	m_pBtnCancel->EvLClick += RTW_EVENT(this, UIFormMsg, OnLClick_Cancel);

	unguard;
}

UIFormMsg::~UIFormMsg()
{
	guard;	

	if (m_pFrmThis)
	{
		m_pFrmThis->Close();
	}

	unguard;
}

UIFormMsg* UIFormMsg::ShowStatic(const RtString& text, FormType t, bool modal)
{
	UIFormMsg* form = new UIFormMsg(t, true);

	form->Show(text, modal);

	return form;
}

void UIFormMsg::Show(const RtString& text, bool modal)
{
	guard;

	m_pFrmThis->GetChildWidget("lbinfoexit")->SetText(text);

	if (modal)
		m_pFrmThis->ShowModal();
	else
		m_pFrmThis->Show();

	m_pFrmThis->BringToTop();

	unguard;
}

void UIFormMsg::Hide()
{
	guard;

	m_pFrmThis->Hide();

	unguard;
}

bool UIFormMsg::IsVisible()
{
	guard;

	return m_pFrmThis != NULL && m_pFrmThis->GetVisible();

	unguard;
}

void UIFormMsg::OnLClick_OK(RtwWidget* sender, void*)
{
	EvOK(this, sender);

	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
}

void UIFormMsg::OnLClick_Cancel(RtwWidget* sender, void*)
{
	EvCancel(this, sender);

	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
}

void UIFormMsg::OnClose(void*, void*)
{
	if (m_autoDelete)
		delete this;
}
