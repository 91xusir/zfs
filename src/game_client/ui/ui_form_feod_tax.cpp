#include "gc_include.h"
#include "net_command_union.h"
#include "ui_form_feod_tax.h"
#include "union_base.h"

UIFormFeodTax::UIFormFeodTax()
{
	guard;

	m_pFrmThis = LOAD_UI("fmtax");
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_tbTaxSet = LOAD_UI("fmtax.txttax");
	m_lbIncomeToday = LOAD_UI("fmtax.lbtotadygoldnum");

	m_pBtnOK = LOAD_UI_T(RtwButton, "fmtax.btnok");
	m_pBtnCancel = LOAD_UI_T(RtwButton, "fmtax.btncancel");

	// 注册本窗口中的事件
	m_pBtnOK->EvLClick += RTW_CALLBACK(this, UIFormFeodTax, OnBtnOK);
	m_pBtnCancel->EvLClick += RTW_CALLBACK(this, UIFormFeodTax, OnBtnCancel);

	m_nTaxRate = -1;
	m_pFrmThis->Hide();

	unguard;
}

UIFormFeodTax::~UIFormFeodTax()
{
	guard;
	unguard;
}

void UIFormFeodTax::OnPreDraw()
{
	guard;
	
	char cTmp32[32];
	if (m_nTaxRate >= 0)
	{
		rt2_snprintf(cTmp32, 32, "%d", m_nTaxRate);
        //cTmp32[31] = 0;
		m_tbTaxSet->SetText(cTmp32);

		rt2_snprintf(cTmp32, 32, "%d", m_iIncomeToday);
        //cTmp32[31] = 0;
		m_lbIncomeToday->SetText(cTmp32);

		m_pBtnOK->Enable();
	}
	else 
	{
		m_tbTaxSet->SetText("0");

		rt2_strncpy(cTmp32, R(MSG_ITEM_REFRESHING), 32);
        //cTmp32[31] = 0;
		m_lbIncomeToday->SetText(cTmp32);

		m_pBtnOK->Disable();
	}

	unguard;
}

void UIFormFeodTax::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormFeodTax::Show(DWORD npcID)
{
	guard;

	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(npcID);
	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		return;
	m_npcID = pNpc->m_pNpc->Id;
	CUnionNetHelper::NetSend_c2r_feod_content_query(GetWorld()->m_dwInteractiveNPCID, (char*)pNpc->m_pNpc->Home.c_str());

	m_nTaxRate = -1;
	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormFeodTax::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormFeodTax::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormFeodTax::OnBtnOK(RtwWidget* sender, void*)
{
	guard;

	const string& strTaxSet = m_tbTaxSet->GetText();

	if (strTaxSet.empty())
	{
		PlayerFailOperate(R(MSG_UNION_SET_TAX));
		return;
	}

	int nTaxRate = atoi(strTaxSet.c_str());
	if (nTaxRate>UNION_TAXRATE_MAX || nTaxRate<UNION_TAXRATE_MIN)
	{
		PlayerFailOperate(R(MSG_UNION_TAX_INVALID));
		return;
	}

	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(GetWorld()->m_dwInteractiveNPCID);
	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		return;
	CUnionNetHelper::NetSend_c2r_feod_taxrate(GetWorld()->m_dwInteractiveNPCID, (char*)pNpc->m_pNpc->Home.c_str(), nTaxRate);

	unguard;
}

void UIFormFeodTax::OnBtnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormFeodTax::OnModifySuccess(int newTaxRate)
{
	guard;

	char tmp128[128];
	rt2_snprintf(tmp128, 128, R(MSG_UNION_TAX_SUCCESS));
    //tmp128[127] = 0;
	ShowSystemMessage(tmp128);

	Hide();

	unguard;
}