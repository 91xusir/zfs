#include "gc_include.h"
#include "net_command_union.h"
#include "ui_form_feod_bank.h"

UIFormFeodBank::UIFormFeodBank()
{
	guard;

	m_pFrmThis = LOAD_UI("fmcitybank");
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_tbWithdraw = LOAD_UI("fmcitybank.txtgold");
	m_lbFeodBank = LOAD_UI("fmcitybank.lbbankgoldnum");

	m_pBtnOK = LOAD_UI_T(RtwButton, "fmcitybank.btnok");
	m_pBtnCancel = LOAD_UI_T(RtwButton, "fmcitybank.btncancel");

	// 注册本窗口中的事件
	m_pBtnOK->EvLClick += RTW_CALLBACK(this, UIFormFeodBank, OnBtnOK);
	m_pBtnCancel->EvLClick += RTW_CALLBACK(this, UIFormFeodBank, OnBtnCancel);

	m_iFeodBank = -1;
	m_pFrmThis->Hide();

	unguard;
}

UIFormFeodBank::~UIFormFeodBank()
{
	guard;
	unguard;
}

void UIFormFeodBank::OnPreDraw()
{
	guard;

	char cTmp32[32];
	if (m_iFeodBank >= 0)
	{
		rt2_snprintf(cTmp32, 32, "%d", m_iFeodBank);
        //cTmp32[31] = 0;
		m_lbFeodBank->SetText(cTmp32);

		m_pBtnOK->Enable();
	}
	else 
	{
		rt2_strncpy(cTmp32, R(MSG_ITEM_REFRESHING), 32);
        //cTmp32[31] = 0;
		m_lbFeodBank->SetText(cTmp32);

		m_pBtnOK->Disable();
	}

	unguard;
}

void UIFormFeodBank::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormFeodBank::Show(DWORD npcID)
{
	guard;

	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(npcID);
	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		return;
	m_npcID = pNpc->m_pNpc->Id;
	CUnionNetHelper::NetSend_c2r_feod_content_query(GetWorld()->m_dwInteractiveNPCID, (char*)pNpc->m_pNpc->Home.c_str());

	m_iFeodBank = -1;
	m_tbWithdraw->SetText("");
	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormFeodBank::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormFeodBank::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormFeodBank::OnBtnOK(RtwWidget* sender, void*)
{
	guard;

	const string& strWithdraw = m_tbWithdraw->GetText();

	if (strWithdraw.empty())
	{
		PlayerFailOperate(R(MSG_ITEM_WITHDRAWMONEY_COUNT));
		return;
	}

	int iWithdraw = atoi(strWithdraw.c_str());
	if (iWithdraw>m_iFeodBank)
	{
		PlayerFailOperate(R(MSG_ITEM_MONEY_INVALID));
		return;
	}
	else if (iWithdraw<=0)
	{
		PlayerFailOperate(R(MSG_ITEM_MONEY_INVALID));
		return;
	}

	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(GetWorld()->m_dwInteractiveNPCID);
	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		return;
	CUnionNetHelper::NetSend_c2r_feod_feodbank_withdraw(GetWorld()->m_dwInteractiveNPCID, (char*)pNpc->m_pNpc->Home.c_str(), iWithdraw);
	m_pBtnOK->Disable();

	unguard;
}

void UIFormFeodBank::OnBtnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormFeodBank::OnWithdrawSuccess(int newBank)
{
	guard;

	Hide();

	char tmp128[128];
	rt2_snprintf(tmp128, 128, R(MSG_UNION_WITHDRAW_SUCCESS));
    //tmp128[127] = 0;
	ShowSystemMessage(tmp128);

	unguard;
}