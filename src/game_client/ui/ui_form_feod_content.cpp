#include "gc_include.h"
#include "net_command_union.h"
#include "ui_form_feod_content.h"

CGcFeodMgr	g_GcFeodMgr;

UIFormFeodContent::UIFormFeodContent()
{
	guard;

	m_pFrmThis = LOAD_UI("fmcitylist");
	//m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

	m_lbTaxRate = LOAD_UI("fmcitylist.lbtaxnum");
	m_lbIncomeToday = LOAD_UI("fmcitylist.lbtotadygoldnum");
	m_lbBank = LOAD_UI("fmcitylist.lbcitygoldnum");

	m_pBtnOK = LOAD_UI_T(RtwButton, "fmcitylist.btnok");
	m_pBtnCancel = LOAD_UI_T(RtwButton, "fmcitylist.btncancel");

	// 注册本窗口中的事件
	m_pBtnOK->EvLClick += RTW_CALLBACK(this, UIFormFeodContent, OnBtnOK);
	m_pBtnCancel->EvLClick += RTW_CALLBACK(this, UIFormFeodContent, OnBtnCancel);

	m_IncomeToday = -1;
	m_FeodBank = -1;
	m_nTaxRate = -1;
	m_pFrmThis->Hide();

	unguard;
}

UIFormFeodContent::~UIFormFeodContent()
{
	guard;
	unguard;
}

void UIFormFeodContent::OnPreDraw()
{
	guard;
	
	char cTmp32[32];
	if (m_IncomeToday >= 0)
	{
		rt2_snprintf(cTmp32, 32, "%d", m_nTaxRate);
        //cTmp32[31] = 0;
		m_lbTaxRate->SetText(cTmp32);
		rt2_snprintf(cTmp32, 32, "%d", m_IncomeToday);
        //cTmp32[31] = 0;
		m_lbIncomeToday->SetText(cTmp32);
		rt2_snprintf(cTmp32, 32, "%d", m_FeodBank);
        //cTmp32[31] = 0;
		m_lbBank->SetText(cTmp32);
	}
	else 
	{
		rt2_strncpy(cTmp32, R(MSG_ITEM_REFRESHING), 32);
        //cTmp32[31] = 0;
		m_lbTaxRate->SetText(cTmp32);
		m_lbIncomeToday->SetText(cTmp32);
		m_lbBank->SetText(cTmp32);
	}

	unguard;
}

void UIFormFeodContent::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormFeodContent::Show(DWORD npcID)
{
	guard;

	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(npcID);
	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		return;
	m_npcID = pNpc->m_pNpc->Id;
	CUnionNetHelper::NetSend_c2r_feod_content_query(GetWorld()->m_dwInteractiveNPCID, (char*)pNpc->m_pNpc->Home.c_str());

	m_IncomeToday = -1;
	m_FeodBank = -1;
	m_nTaxRate = -1;
	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

bool UIFormFeodContent::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormFeodContent::Hide()
{
	guard;
	if (m_pFrmThis)		m_pFrmThis->Hide();
	unguard;
}

void UIFormFeodContent::OnBtnOK(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}

void UIFormFeodContent::OnBtnCancel(RtwWidget* sender, void*)
{
	guard;
	Hide();
	unguard;
}


CGcFeodMgr::CGcFeodMgr()
{
	Clear();
}

CGcFeodMgr::~CGcFeodMgr()
{
}

void CGcFeodMgr::Clear()
{
	m_arrFeods.clear();
}

SFeod* CGcFeodMgr::Find(string name)
{
	for (int i=0; i<(int)m_arrFeods.size(); i++)
	{
		if (m_arrFeods[i].name==name)
			return &(m_arrFeods[i]);
	}
	return NULL;
}

void CGcFeodMgr::GetFeodInfo(string feodName, DWORD &dwCurrentUnion, int &rate)
{
	dwCurrentUnion = 0;
	rate = 0;
	SFeod* pFeod = Find(feodName);
	if (!pFeod)
	{
		ERR1("未知的采邑地点：%s\n", feodName.c_str());
		return;
	}
	dwCurrentUnion = pFeod->unionID;
	rate = pFeod->taxRate;
	CHECK(rate>=UNION_TAXRATE_MIN && rate<=UNION_TAXRATE_MAX);
}

bool CGcFeodMgr::AddFeodInfo(SFeod& feod)
{
	m_arrFeods.push_back(feod);
	return true;
}