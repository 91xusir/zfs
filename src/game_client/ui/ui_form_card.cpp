#include "gc_include.h"
#include "ui_form_card.h"


UIFormCard::UIFormCard()
{
	guard;
    m_pFrmThis = RTW_WIDGET("fmcard");

    m_pCaption   = RTW_WIDGET("fmcard.lbinfo");
    m_pTextInput = RTW_WIDGET_T(RtwTextBox, "fmcard.txtcard");
    m_pBtnOK     = RTW_WIDGET_T(RtwButton, "fmcard.btnok");

    RtwButton* pBtnBack = RTW_WIDGET_T(RtwButton, "fmcard.btnback");

    m_pTextInput->SetAlphaNumberOnly(true);
    m_pTextInput->SetCapsLock(1);

	m_pBtnOK->EvLClick                              += RTW_CALLBACK(this, UIFormCard, OnLClick_OK);
	m_pFrmThis->EvClose                             += RTW_CALLBACK(this, UIFormCard, OnClose);
	pBtnBack->EvLClick                              += RTW_CALLBACK(this, UIFormCard, OnLClick_Cancel);
//	(m_pFrmThis->GetChildWidget("$hide"))->EvLClick += RTW_CALLBACK(this, UIFormCard, OnLClick_Cancel);

	m_pFrmThis->Hide();
	unguard;
}

UIFormCard::~UIFormCard()
{
	guard;	
	unguard;
}

void UIFormCard::Show(const char* szTitle, const char* szContent, const char* szButton, bool bModal)
{
	guard;

    if (szTitle)    m_pCaption->SetText(szTitle);
    if (szContent)  m_pTextInput->SetText(szContent);
    if (szButton)   m_pBtnOK->SetText(szButton);
    m_pTextInput->SetFocus();

	if (bModal)
		m_pFrmThis->ShowModal();
	else
		m_pFrmThis->Show();

	m_pFrmThis->BringToTop();

	unguard;
}

void UIFormCard::SetCardID(const char* szContent)
{
	guard;
    if (szContent) m_pTextInput->SetText(szContent);
	unguard;
}

void UIFormCard::SetUserData(long lNpcID, long lType)
{
    m_lNpcID = lNpcID;
    m_lType = lType;
}

void UIFormCard::Hide()
{
	guard;
	m_pFrmThis->Hide();
	unguard;
}

bool UIFormCard::IsVisible()
{
	guard;
	return m_pFrmThis != NULL && m_pFrmThis->IsVisible();
	unguard;
}

void UIFormCard::OnLClick_OK(RtwWidget* sender, void*)
{
	guard;
	m_strCardID = m_pTextInput->GetText();
    GcWorld* pWorld = GetWorld();
    if (pWorld)
    {
        pWorld->OnInputCardID(m_lNpcID, m_lType, m_strCardID.c_str(), "");
    }
	m_pFrmThis->Hide();
//	EvOK(sender, weMouseClick);
	unguard;
}

void UIFormCard::OnLClick_Cancel(RtwWidget* sender, void*)
{
	guard;
	m_pFrmThis->Hide();
//	EvCancel(sender, weMouseClick);
	unguard;
}

void UIFormCard::OnClose(void*, void*)
{
}
