#include "gc_include.h"
#include "ui_form_bankpass.h"


UIFormBankPass::UIFormBankPass()
{
	guard;

	m_state = sAddPass;

	m_pFrmThis = RTW_WIDGET("fmbankpass");
	m_pTitle = RTW_WIDGET("fmbankpass.lbinfo");
	m_pText1 = RTW_WIDGET("fmbankpass.lbpass1");
	m_pEdit1 = RTW_WIDGET("fmbankpass.txtpass1");
	m_pText2 = RTW_WIDGET("fmbankpass.lbpass2");
	m_pEdit2 = RTW_WIDGET("fmbankpass.txtpass2");
	m_pText3 = RTW_WIDGET("fmbankpass.lbpass3");
	m_pEdit3 = RTW_WIDGET("fmbankpass.txtpass3");
	m_pBtnOK = RTW_WIDGET_T(RtwButton, "fmbankpass.btnok");
	m_pBtnCancel = RTW_WIDGET_T(RtwButton, "fmbankpass.btnback");

	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_pBtnOK->EvLClick += RTW_CALLBACK(this, UIFormBankPass, OnClicked_OK);
	m_pBtnCancel->EvLClick += RTW_CALLBACK(this, UIFormBankPass, OnClicked_Cancel);

	m_npcID = 0;
	m_pFrmThis->Hide();

	unguard;
}

UIFormBankPass::~UIFormBankPass()
{
	guard;

	unguard;
}

void UIFormBankPass::OnPreDraw()
{
	guard;

	m_pEdit1->SetText("");
	m_pEdit2->SetText("");
	m_pEdit3->SetText("");

	switch (m_state)
	{
	case sAddPass:
		m_pTitle->SetText(R(MSG_BANK_PASS_SET));
		m_pText1->SetText(R(MSG_BANK_INPUTPASS));
		m_pText2->SetText(R(MSG_BANK_REINPUTPASS));
		m_pText1->Show();
		m_pEdit1->Show();
		m_pText2->Show();
		m_pEdit2->Show();
		m_pText3->Hide();
		m_pEdit3->Hide();
		break;
	case sRemovePass:
		m_pTitle->SetText(R(MSG_BANK_CLEARPASS));
		m_pText1->SetText(R(MSG_BANK_INPUTPASS));
		m_pText1->Show();
		m_pEdit1->Show();
		m_pText2->Hide();
		m_pEdit2->Hide();
		m_pText3->Hide();
		m_pEdit3->Hide();
		break;
	case sChangePass:
		m_pTitle->SetText(R(MSG_BANK_MODIFYPASS));
		m_pText1->SetText(R(MSG_BANK_ORIGPASS));
		m_pText2->SetText(R(MSG_BANK_NEWPASS));
		m_pText3->SetText(R(MSG_BANK_REINPUTPASS));
		m_pText1->Show();
		m_pEdit1->Show();
		m_pText2->Show();
		m_pEdit2->Show();
		m_pText3->Show();
		m_pEdit3->Show();
		break;
	case sCheck:
		m_pTitle->SetText(R(MSG_BANK_PASS_QUERY));
		m_pText1->SetText(R(MSG_BANK_INPUTPASS));
		m_pText1->Show();
		m_pEdit1->Show();
		m_pText2->Hide();
		m_pEdit2->Hide();
		m_pText3->Hide();
		m_pEdit3->Hide();
		break;
	default:
		break;
	}

	unguard;
}

void UIFormBankPass::Show(DWORD npcID, EState state)
{
	guard;
	m_npcID = npcID;
	m_state = state;
	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormBankPass::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormBankPass::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormBankPass::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormBankPass::OnClicked_OK(RtwWidget* sender, void*)
{
	guard;
	
	string s1 = m_pEdit1->GetText();
	string s2 = m_pEdit2->GetText();
	string s3 = m_pEdit3->GetText();
	switch (m_state)
	{
	case sAddPass:
		if (s1!=s2)
		{
			PlayerFailOperate(R(MSG_BANK_PASS_NOTSAME));
			return;
		}
		CItemNetHelper::NetSend_c2r_bank_addpass(s1.c_str());
		break;
	case sChangePass:
		if (s2!=s3)
		{
			PlayerFailOperate(R(MSG_BANK_PASS_NOTSAME));
			return;
		}
		CItemNetHelper::NetSend_c2r_bank_changepass(s1.c_str(), s2.c_str());
		break;
	case sRemovePass:
		CItemNetHelper::NetSend_c2r_bank_removepass(s1.c_str());
		break;
	case sCheck:
		CItemNetHelper::NetSend_c2r_bank_query(m_npcID, s1.c_str());
		break;
	default:
		break;
	}

	Hide();

	unguard;
}

void UIFormBankPass::OnClicked_Cancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

