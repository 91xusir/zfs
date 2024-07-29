//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "gc_include.h"
#include "net_command_castlewar.h"
#include "game_string_assistant.h"
#include "ui_form_castlewar_settime.h"

UIFormCastleWarTimeSet::UIFormCastleWarTimeSet()
{
	guard;

	m_pFrmThis = LOAD_UI("fmcitytime");

	m_pBtnOk	 = LOAD_UI_T(RtwButton, "fmcitytime.btnok");
	m_pBtnCancel = LOAD_UI_T(RtwButton, "fmcitytime.btncancel");

	m_pLabelDefaultTime	= LOAD_UI_T(RtwLabel, "fmcitytime.lbdate");
	m_pTimeSetBox		= LOAD_UI_T(RtwComboBox, "fmcitytime.cbtime");

	// 注册本窗口中的事件
	m_pBtnOk->EvLClick		+= RTW_CALLBACK(this, UIFormCastleWarTimeSet, OnBtnOk);
	m_pBtnCancel->EvLClick	+= RTW_CALLBACK(this, UIFormCastleWarTimeSet, OnBtnCancel);

	m_pTimeSetBox->SetSelectedItem(2);	

	m_npc_id = 0;
	m_b_lock = false;

	m_war_info.feod_name			= "";
	m_war_info.master_union_id		= 0;
	m_war_info.master_union_name	= "";
	m_war_info.master_union_logo	= "";
	m_war_info.vassal_name			= "";
	m_war_info.state				= NPC_OCCUPY_STATE;
	m_war_info.start_time			= 0;
	m_war_info.end_time				= 0;
	m_war_info.last_end_time		= 0;
	m_war_info.v_attacker_union_info.clear();
	m_war_info.v_defender_union_info.clear();

	m_pFrmThis->Hide();

	unguard;
}

UIFormCastleWarTimeSet::~UIFormCastleWarTimeSet()
{
	guard;

	unguard;
}

void UIFormCastleWarTimeSet::Hide()
{
	guard;

	if (m_pFrmThis)
	{
		m_pFrmThis->Hide();
	}

	unguard;
}
void UIFormCastleWarTimeSet::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}

	unguard;
}

bool UIFormCastleWarTimeSet::IsVisible()
{
	guard;

	if(m_pFrmThis && (m_pFrmThis->GetFlags(wfVisible)))
	{
		return true;
	}else
	{
		return false;
	}

	unguard;
}


void UIFormCastleWarTimeSet::Show(DWORD npc_id)
{
	guard;

	if((GetWorld()->m_pPlayer->GetUnionID() != m_war_info.master_union_id) ||
	   (strcmp(GetWorld()->m_pPlayer->GetName(), m_war_info.vassal_name.c_str()) != 0))
	{
		ShowSystemMessage(STR_NOT_VASSAL_MSG);

		return;
	}

	m_npc_id = npc_id;

	CCastleWarNetHelper::NetSend_c2r_castlewar_view_info(m_npc_id);

	OnPreDraw();

	m_pFrmThis->Show();

	unguard;
}

void UIFormCastleWarTimeSet::OnPreDraw()
{
	guard;

	//-----------------------
	//通用信息显示
	//-----------------------

	string time_str = fox::TimeConvert(m_war_info.start_time);

	m_pLabelDefaultTime->SetText(time_str);
	
	unguard;
}

void UIFormCastleWarTimeSet::OnBtnOk(RtwWidget* sender, void*)
{
	guard;

	if(m_b_lock == true)
	{
		return;
	}

	short index = m_pTimeSetBox->GetSelectedItem();

	if(index == -1) return;

	//程序中默认周日为一周的第一天

	CCastleWarNetHelper::NetSend_c2r_castlewar_select_time(m_npc_id, (index + 1) % 7);

	Lock();

	unguard;
}

void UIFormCastleWarTimeSet::OnBtnCancel(RtwWidget* sender, void*)
{
	guard;

	Hide();

	unguard;
}