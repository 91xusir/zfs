//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "gc_include.h"
#include "net_command_castlewar.h"
#include "game_string_assistant.h"
#include "ui_form_castlewar.h"
#include "ui_form_union.h"
#include "net_command_union.h"

static bool SortDefender(const STDefenderInfo& p1, const STDefenderInfo& p2);


UIFormCastleWar::UIFormCastleWar()
{
	guard;

	m_pFrmThis = LOAD_UI("fmbattle");

	m_pTabGuild			= LOAD_UI_T(RtwTab, "fmbattle.fmcity.tabguild");
	m_pBtnAttacker		= LOAD_UI_T(RtwButton, "fmbattle.fmcity.tabguild.btnattacklist");
	m_pBtnDefender		= LOAD_UI_T(RtwButton, "fmbattle.fmcity.tabguild.btndeflist");
	m_pBtnAllow			= LOAD_UI_T(RtwButton, "fmbattle.fmcity.btnagree");
	m_pBtnApply			= LOAD_UI_T(RtwButton, "fmbattle.fmcity.btnjoin");
	m_pBtnCancelApply	= LOAD_UI_T(RtwButton, "fmbattle.fmcity.btnleave");

	m_pListAttacker		= LOAD_UI_T(RtwListBox, "fmbattle.fmcity.tabguild.fmattacklist.listattack");
	m_pListDefender		= LOAD_UI_T(RtwListBox, "fmbattle.fmcity.tabguild.fmdeflist.listdef");

	m_pLabelCastleName	= LOAD_UI_T(RtwLabel, "fmbattle.fmcity.lbcity");
	m_pLabelUnionName	= LOAD_UI_T(RtwLabel, "fmbattle.fmcity.lbguildname");
	m_pLabelUnionLogo	= LOAD_UI_T(RtwLabel, "fmbattle.fmcity.lblv");
	m_pLabelStartTime	= LOAD_UI_T(RtwLabel, "fmbattle.fmcity.lbattacktime");

	// 注册本窗口中的事件
	m_pBtnAttacker->EvLClick	+= RTW_CALLBACK(this, UIFormCastleWar, OnBtnAttacker);
	m_pBtnDefender->EvLClick	+= RTW_CALLBACK(this, UIFormCastleWar, OnBtnDefender);
	m_pBtnAllow->EvLClick		+= RTW_CALLBACK(this, UIFormCastleWar, OnBtnAllow);
	m_pBtnApply->EvLClick		+= RTW_CALLBACK(this, UIFormCastleWar, OnBtnApply);
	m_pBtnCancelApply->EvLClick	+= RTW_CALLBACK(this, UIFormCastleWar, OnBtnCancelApply);

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

	m_pTabGuild->SetSelectedItem(0);
	m_pFrmThis->Hide();

	unguard;
}

UIFormCastleWar::~UIFormCastleWar()
{
	guard;

	unguard;
}

static bool SortDefender(const STDefenderInfo& p1, const STDefenderInfo& p2)
{
	guard;	

	if (p1.is_allowed != p2.is_allowed)
	{
		return (p1.is_allowed == true);
	}

	//CHECK(0);		// playerID是不可能相同的

	return true;

	unguard;
}

void UIFormCastleWar::Hide()
{
	guard;

	if (m_pFrmThis)
	{
		m_pFrmThis->Hide();
	}

	unguard;
}
void UIFormCastleWar::Refresh()
{
	guard;
	
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}

	unguard;
}

bool UIFormCastleWar::IsVisible()
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

bool UIFormCastleWar::IsInAttackerList()
{
	guard;

	return (m_pTabGuild->GetSelectedItem() == 0);

	unguard;
}

void UIFormCastleWar::Show(DWORD npc_id)
{
	guard;
	
	m_npc_id = npc_id;

	CCastleWarNetHelper::NetSend_c2r_castlewar_view_info(m_npc_id);
	
	OnPreDraw();

	m_pFrmThis->Show();

	unguard;
}

void UIFormCastleWar::OnPreDraw()
{
	guard;

	//-----------------------
	//通用信息显示
	//-----------------------
	std::string feod_show = "";

	for(int cnt = 0; cnt < FEOD_COUNT; cnt++)
	{
		if(m_war_info.feod_name == FeodNameInDB[cnt])
		{
			feod_show = CastleWarAreaName[cnt];

			/*if(GetWorld()->IsCastleWarTime(m_war_info.feod_name) == true)
			{
				feod_show += STRING_FIGHTING;
			}*/

			m_pLabelCastleName->SetText(feod_show);
		}
	}	
	
	if(m_war_info.master_union_id == 0)
	{
		m_pLabelUnionName->SetText(STRING_NPC_OCCUPY);
		m_pLabelUnionLogo->SetBackgroundImage((RtwImage*)NULL);
	}else
	{
		m_pLabelUnionName->SetText(m_war_info.master_union_name);

		if ( strcmp(m_war_info.master_union_logo.c_str(), UNION_DEFAULT_ICON_NAME)!=0 )
		{
			CUnionIconMgr::SUnionIconData *pIconData = g_UnionIconMgr.Find((char*)m_war_info.master_union_logo.c_str());
			if (pIconData)
			{
// 				RtwImage *pImage = new RtwImage();
// 				pImage->SetImage(pIconData->data, pIconData->dataLen);// change [3/17/2009 tooth.shi]
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(pIconData->data, pIconData->dataLen);
				m_pLabelUnionLogo->SetBackgroundImage(pImage);
			}
			else 
			{
				m_pLabelUnionLogo->SetBackgroundImage((RtwImage*)NULL);
				CUnionNetHelper::NetSend_c2r_union_query_logo_data((char*)m_war_info.master_union_logo.c_str());
			}
		}
		else 
			m_pLabelUnionLogo->SetBackgroundImage((RtwImage*)NULL);
	}

	//将DWORD格式时间转换为字符串格式

	TIME		tmp = m_war_info.start_time;
	ST_TIME*	p_time;

	p_time = localtime(&tmp);

	string time_str = fox::TimeConvert(m_war_info.start_time);

#ifdef _DEBUG
	LOG1("Next Castle War Start Time is : [%d]\n", m_war_info.start_time);
	LOG5("Start Time: [%d]-[%d]-[%d]-[%d]-[%d]\n", 
		p_time->tm_year + 1900,
		p_time->tm_mon + 1,
		p_time->tm_mday,
		p_time->tm_hour,
		p_time->tm_min);
	LOG1("Next Castle War Start Time is : [%s]", time_str.c_str());
#endif //_DEBUG

	m_pLabelStartTime->SetText(time_str);
	
	if(IsInAttackerList())
	{
		m_pBtnAllow->Hide();

		m_pListAttacker->RemoveAllItems();

		int att_num = m_war_info.v_attacker_union_info.size();

		for(int i = 0; i < att_num ; i++)
		{
			int id = m_pListAttacker->AddItem("");
			m_pListAttacker->SetItemText(i, 0, m_war_info.v_attacker_union_info[i].union_name);
			m_pListAttacker->SetItemText(i, 1, m_war_info.v_attacker_union_info[i].vassal_name);
			m_pListAttacker->SetItemText(i, 2, STRING_REGISTERED);
		}

	}else
	{
		// 排序
		//std::sort(m_war_info.v_defender_union_info.begin(), m_war_info.v_defender_union_info.end(), SortDefender);
		
		m_pBtnAllow->Show();

		m_pListDefender->RemoveAllItems();

		//m_pListDefender->SetHintText(m_pListDefender->GetSelectedItem())

		//首先添加采邑主人
		if(m_war_info.master_union_id != 0)
		{
			m_pListDefender->AddItem("");
			m_pListDefender->SetItemText(0, 0, m_war_info.master_union_name);
			m_pListDefender->SetItemText(0, 1, m_war_info.vassal_name);
			m_pListDefender->SetItemText(0, 2, STRING_FEOD_MASTER);
		}

		int def_num = m_war_info.v_defender_union_info.size();

		for(int i = 0; i < def_num; i++)
		{
			int idx = m_pListDefender->AddItem("");
			m_pListDefender->SetItemText(idx, 0, m_war_info.v_defender_union_info[i].union_name);
			m_pListDefender->SetItemText(idx, 1, m_war_info.v_defender_union_info[i].vassal_name);
			
			if(m_war_info.v_defender_union_info[i].is_allowed == true)
			{
				m_pListDefender->SetItemText(idx, 2, STRING_IS_ALLOWED);

				m_pListDefender->SetItemColor(idx, RtwPixel(0xFFFFFF00));
			}else
			{
				m_pListDefender->SetItemText(idx, 2, STRING_WAIT_FOR_ALLOWED);

				m_pListDefender->SetItemColor(idx, RtwPixel(0xFFFF0000));
			}
		}
	}

	unguard;
}

void UIFormCastleWar::OnBtnAllow(RtwWidget* sender, void*)
{
	guard;

	if(m_b_lock == true)
	{
		return;
	}

	DWORD	union_id = 0;
	int		index	 = m_pListDefender->GetSelectedItem();
	std::string union_name;

	if(index == -1) return;

	union_name = m_pListDefender->GetItemText(index, 0);

	int i = 0;
	for(i = 0; i < m_war_info.v_defender_union_info.size(); i++)
	{
		if(m_war_info.v_defender_union_info[i].union_name == union_name)
		{
			union_id = m_war_info.v_defender_union_info[i].union_id;
			break;
		}
	}

	if(i >= m_war_info.v_defender_union_info.size()) return;
	
	CCastleWarNetHelper::NetSend_c2r_castlewar_allow_bund(m_npc_id, union_id, ALLOWED);
	
	Lock();

	unguard;
}

void UIFormCastleWar::OnBtnApply(RtwWidget* sender, void*)
{
	guard;
	
	if(m_b_lock == true)
	{
		return;
	}

	if(IsInAttackerList())
	{				
		CCastleWarNetHelper::NetSend_c2r_castlewar_reg_attack(m_npc_id);
	}else
	{
		CCastleWarNetHelper::NetSend_c2r_castlewar_reg_defend(m_npc_id);
	}

	Lock();

	unguard;
}

void UIFormCastleWar::OnBtnAttacker(RtwWidget* sender, void*)
{
	guard;

	Refresh();

	unguard;
}

void UIFormCastleWar::OnBtnCancelApply(RtwWidget* sender, void*)
{
	guard;

	if(m_b_lock == true)
	{
		return;
	}

	if(IsInAttackerList())
	{		
		CCastleWarNetHelper::NetSend_c2r_castlewar_reg_attack_cancel(m_npc_id);
	}else
	{
		CCastleWarNetHelper::NetSend_c2r_castlewar_reg_defend_cancel(m_npc_id);
	}

	Lock();

	unguard;
}

void UIFormCastleWar::OnBtnDefender(RtwWidget* sender, void*)
{
	guard;

	Refresh();

	unguard;
}
