#include "gc_include.h"
#include "ui_form_char.h"
#include "ui_form_msg.h"
#include "ui_form_playeritems.h"
#include "ui_form_union.h"
#include "ui_form_item_update.h"
#include "ui_form_shop.h"
#include "ui_form_lookplayershop.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_compose.h"
#include "ui_form_fuse.h"
#include "UIForm_Gem.h"
#include "ui_form_decompound.h"
#include "ui_form_union_create.h"
#include "game_assistant.h"

// ----------------------------------------------------------------------------
// UI for char prop
// ----------------------------------------------------------------------------


// UI Task Sort
 enum ETaskSort
{
	TaskSort_Name,
	TaskSort_Level,
	TaskSort_Count,
	TaskSort_State
};

static ETaskSort taskSortPriority[4] = { TaskSort_State, TaskSort_Level, TaskSort_Count, TaskSort_Name };
static bool SortFunc_Task(const STaskInfo& i1, const STaskInfo& i2);

bool SortFunc_Task(const STaskInfo& i1, const STaskInfo& i2)
{
	guard;

	/*
	STaskDesc *d1 = NULL;
	STaskDesc *d2 = NULL;

	int depth = 0;
	while (depth<4)
	{
		if (taskSortPriority[depth]==TaskSort_Name)
		{
			if (!d1)		d1 = g_TableTask.FindTask(i1.Id);
			if (!d2)		d2 = g_TableTask.FindTask(i2.Id);
			if (d1->desc != d2->desc)			return (d1->desc > d2->desc);
		}
		else if (taskSortPriority[depth]==TaskSort_Level)
		{
			if (!d1)		d1 = g_TableTask.FindTask(i1.Id);
			if (!d2)		d2 = g_TableTask.FindTask(i2.Id);
			if (d1->levFrom != d2->levFrom)		return (d1->levFrom < d2->levFrom);
		}
		else if (taskSortPriority[depth]==TaskSort_Count)
		{
			if (i1.FinishNum != i2.FinishNum)	return (i1.FinishNum > i2.FinishNum);
		}
		else if (taskSortPriority[depth]==TaskSort_State)
		{
			if (i1.IsAccept != i2.IsAccept)		return (i1.IsAccept > i2.IsAccept);
		}
		depth++;
	}
	*/

	return true;

	unguard;
}

// UIFormCharProp
UIFormCharProp::UIFormCharProp()
{
	m_bValid = false;
	m_form = NULL;
}

UIFormCharProp::~UIFormCharProp()
{

}

bool UIFormCharProp::Init()
{
	m_pFormAll = LOAD_UI("fmcharall");
	//m_pFormAll->EvKey += RTW_CALLBACK(m_pFormAll, RtwWidget, DefaultKeyDown);

	m_form = LOAD_UI("fmcharall.tabcharall.fmchar");
	if(m_form)
        m_bValid = true;
	else
		m_bValid = false;

	LOAD_UI("fmcharall.tabcharall.fmchar.btnstradd")->EvLClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btnhitadd")->EvLClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btndexadd")->EvLClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btnwisadd")->EvLClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btnhealadd")->EvLClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);

	LOAD_UI("fmcharall.tabcharall.fmchar.btnstradd")->EvLDClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btnhitadd")->EvLDClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btndexadd")->EvLDClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btnwisadd")->EvLDClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);
	LOAD_UI("fmcharall.tabcharall.fmchar.btnhealadd")->EvLDClick += 
		RTW_CALLBACK(this, UIFormCharProp, OnClickAddAttr);

	LOAD_UI("fmcharall.tabcharall.fmchar")->EvShow += RTW_CALLBACK(this, UIFormCharProp, OnShow);
	LOAD_UI("fmcharall")->EvShow += RTW_CALLBACK(this, UIFormCharProp, OnShow);

	m_jobName = LOAD_UI("fmcharall.tabcharall.fmchar.lbjobname");
	m_faction = LOAD_UI("fmcharall.tabcharall.fmchar.lbsitename");
	m_name    = LOAD_UI("fmcharall.tabcharall.fmchar.lbname");
	m_exp     = LOAD_UI("fmcharall.tabcharall.fmchar.lbexpcount");
	m_lev     = LOAD_UI("fmcharall.tabcharall.fmchar.lblevnum");
	m_pk	  = LOAD_UI("fmcharall.tabcharall.fmchar.lbpkname");
	m_credit  = LOAD_UI("fmcharall.tabcharall.fmchar.lbprenum");
	m_exploit = LOAD_UI("fmcharall.tabcharall.fmchar.lbwarnum");

	m_attrPoint = LOAD_UI("fmcharall.tabcharall.fmchar.lbappoint");
	m_skillPoint = LOAD_UI("fmcharall.tabcharall.fmchar.lbsppoint");

	m_hp	= LOAD_UI("fmcharall.tabcharall.fmchar.lbhpcount");
	m_mp    = LOAD_UI("fmcharall.tabcharall.fmchar.lbmpcount");

	m_str   = LOAD_UI("fmcharall.tabcharall.fmchar.lbstrpoint");
	m_con   = LOAD_UI("fmcharall.tabcharall.fmchar.lbhealthpoint");
	m_dex   = LOAD_UI("fmcharall.tabcharall.fmchar.lbdexpoint");
	m_int   = LOAD_UI("fmcharall.tabcharall.fmchar.lbwispoint");
	m_hit   = LOAD_UI("fmcharall.tabcharall.fmchar.lbhitpoint");


	m_strBtn   = LOAD_UI("fmcharall.tabcharall.fmchar.lbstr");
	m_conBtn   = LOAD_UI("fmcharall.tabcharall.fmchar.lbhealth");
	m_dexBtn   = LOAD_UI("fmcharall.tabcharall.fmchar.lbdex");
	m_intBtn   = LOAD_UI("fmcharall.tabcharall.fmchar.lbwis");
	m_hitBtn   = LOAD_UI("fmcharall.tabcharall.fmchar.lbhit");

	m_strBtn->EvHint += RTW_CALLBACK(this, UIFormCharProp, OnSkillCriHint);
	m_dexBtn->EvHint += RTW_CALLBACK(this, UIFormCharProp, OnSkillCriHint);
	m_intBtn->EvHint += RTW_CALLBACK(this, UIFormCharProp, OnSkillCriHint);

	m_hitBtn->ModifyFlags(0,wfHint);
	m_conBtn->ModifyFlags(0,wfHint);

	m_strPower = LOAD_UI("fmcharall.tabcharall.fmchar.lbstrpowernum");
	m_conPower = LOAD_UI("fmcharall.tabcharall.fmchar.lbconpowernum");
	m_dexPower = LOAD_UI("fmcharall.tabcharall.fmchar.lbdexpowernum");
	m_intPower = LOAD_UI("fmcharall.tabcharall.fmchar.lbintpowernum");
	m_hitPower = LOAD_UI("fmcharall.tabcharall.fmchar.lbhitpowernum");

	m_attack = LOAD_UI("fmcharall.tabcharall.fmchar.lbattackpoint");
	m_damage = LOAD_UI("fmcharall.tabcharall.fmchar.lbdamagepoint");
	m_dodge  = LOAD_UI("fmcharall.tabcharall.fmchar.lbdoggepoint");
	m_armor  = LOAD_UI("fmcharall.tabcharall.fmchar.lbdefpoint");
	m_attSpeed = LOAD_UI("fmcharall.tabcharall.fmchar.lbspeedpoint");

	m_hpBar = LOAD_UI_T(RtwProgressBar,"fmcharall.tabcharall.fmchar.barhp");
	m_mpBar = LOAD_UI_T(RtwProgressBar,"fmcharall.tabcharall.fmchar.barmp");
	m_expBar = LOAD_UI_T(RtwProgressBar,"fmcharall.tabcharall.fmchar.barexp");

	m_fireArmor = LOAD_UI("fmcharall.tabcharall.fmchar.lbfirepoint");
	m_waterArmor = LOAD_UI("fmcharall.tabcharall.fmchar.lbwaterpoint");
	m_poisonArmor = LOAD_UI("fmcharall.tabcharall.fmchar.lbpoisonpoint");

	m_fireDamage = LOAD_UI("fmcharall.tabcharall.fmchar.lbfirepointA");
	m_waterDamage = LOAD_UI("fmcharall.tabcharall.fmchar.lbwaterpointA");
	m_poisonDamage = LOAD_UI("fmcharall.tabcharall.fmchar.lbpoisonpointA");
	return m_bValid;
}

RtwPixel UIFormCharProp::GetColor(float rate)
{
	if(rate-1 > 0.01)
	{
		return RtwPixel(0xFFFFFF00);
	}
	else if (rate-1 < -0.01)
	{
		return RtwPixel(0xFFFF0000);
	}
	else
	{
		return RtwPixel(0xFFFFFFFF);
	}
}

void UIFormCharProp::OnSkillCriHint(RtwWidget *sender,void *)
{
	if(!GetWorld()->m_pPlayer) return;
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;

	char tmp[128];
	rt2_snprintf(tmp,128,R(MSG_SKILL_CRITICAL),cre->Lev,cre->GetSkillCriRate(cre->Lev)*100);
	//tmp[127] = '\0';
	
	g_workspace.SetHintText(tmp);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
}

bool UIFormCharProp::Update()
{
	guard;

	if(!m_bValid) return false;
	if(!IsVisible()) return false;
	if(!GetWorld()->m_pPlayer) return false;
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;
	
	RtString tmp;
	m_strBtn->ModifyFlags(0,wfHint);
	m_dexBtn->ModifyFlags(0,wfHint);
	m_intBtn->ModifyFlags(0,wfHint);
	
	switch(cre->Metier)
	{
		case METIER_WARRIOR:
			m_strBtn->ModifyFlags(wfHint,0);
			break;

		case METIER_HUNTER:
			m_dexBtn->ModifyFlags(wfHint,0);
			break;

		case METIER_WIZARD:
			m_intBtn->ModifyFlags(wfHint,0);
			break;

		case METIER_TAOIST:
			m_intBtn->ModifyFlags(wfHint,0);
			break;
	}

	tmp.Format("%s",cre->Name.c_str());
	m_name->SetText((char*)tmp);
	// const char *jobName = g_TableUserActor.GetMetierName(cre->Metier,cre->MetierLev);
	SUserActor *actor = g_TableUserActor.FindUserActor(cre->Metier,cre->MetierLev);
	if(actor) m_jobName->SetText(actor->MetierName.c_str());

	/****lxJian*09.12.03****/
	if (m_faction) 
	{
		if (cre->Faction==FACTION_ZHOU)
		{
			m_faction->SetText(R(G_ZHOU));
		}
		else if (cre->Faction==FACTION_SHANG)
		{
			m_faction->SetText(R(G_SHANG));
		}
		else if (cre->Faction==FACTION_NEUTRAL)
		{
			m_faction->SetText(R(G_NEUTRAL));
		}
	}
	tmp.Format("%d",cre->Lev);
	m_lev->SetText((char*)tmp);

	tmp.Format("%d",cre->Credit);
	m_credit->SetText((char*)tmp);

	tmp.Format("%d",cre->Exploit);
	m_exploit->SetText((char*)tmp);

	if(cre->Pk == 0)
		tmp.Format("%d",cre->Pk);
	else
        tmp.Format("%d",cre->Pk / (1000*60) + 1);
	m_pk->SetText((char*)tmp);
	
	// 需要和服务器的定义同步
	if(cre->Pk == 0)
    {
		m_pk->SetTextColor(RtwPixel(0xFFFFFFFF));
    }else
    {
        if(cre->Pk >= 300 * 1000 * 60)
            m_pk->SetTextColor(RtwPixel(0xFFFF0000));
        else
            m_pk->SetTextColor(RtwPixel(0xFFFFFF00));
    }

	double expRate = (double)cre->Exp / g_TableExp.GetNextLevExp(cre->Lev);
	m_expBar->SetValue(expRate);
	m_hpBar->SetValue((float)cre->GetHp()/cre->GetMaxHp());	
	m_mpBar->SetValue((float)cre->GetMp()/cre->GetMaxMp());

	if(cre->Lev > 75)
	{
		tmp.Format("%.4f%%",expRate*100);
		// m_exp->SetTextColor(RtwPixel(0xFFFF0000));
	}
	else
	{
		// m_exp->SetTextColor(RtwPixel(0xFFFFFFFF));
		tmp.Format("%.2f%%",expRate*100);
	}
	m_exp->SetText((char*)tmp);
	tmp.Format("%d/%d",cre->GetHp(),cre->GetMaxHp());
	m_hp->SetText((char*)tmp);
	/*
	if (cre->MaxHp.ColorfulValue!=0)
		m_hp->SetTextColor(RtwPixel(0xFF8B8BE3));
	else 
		m_hp->SetTextColor(RtwPixel(0xFFFFFFFF));
	*/
	tmp.Format("%d/%d",cre->GetMp(),cre->GetMaxMp());
	m_mp->SetText((char*)tmp);
	/*
	if (cre->MaxMp.ColorfulValue!=0)
		m_mp->SetTextColor(RtwPixel(0xFF8B8BE3));
	else 
		m_mp->SetTextColor(RtwPixel(0xFFFFFFFF));
	*/
	tmp.Format("%d",cre->AttrPoint);
	m_attrPoint->SetText((char*)tmp);
	tmp.Format("%d",cre->SkillPoint);
	m_skillPoint->SetText((char*)tmp);
    
	tmp.Format("%d",cre->Str.GetValue());
	m_str->SetText((char*)tmp);
	if(cre->Str.Temp > 0)
		m_str->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_str->SetTextColor(RtwPixel(0xFFFFFFFF));

	tmp.Format("%d",cre->Con.GetValue());
	m_con->SetText((char*)tmp);
	if(cre->Con.Temp > 0)
		m_con->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_con->SetTextColor(RtwPixel(0xFFFFFFFF));
	
	tmp.Format("%d",cre->Dex.GetValue());
	m_dex->SetText((char*)tmp);
	if(cre->Dex.Temp > 0)
		m_dex->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_dex->SetTextColor(RtwPixel(0xFFFFFFFF));

	tmp.Format("%d",cre->Int.GetValue());
	m_int->SetText((char*)tmp);
	if(cre->Int.Temp > 0)
		m_int->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_int->SetTextColor(RtwPixel(0xFFFFFFFF));

	tmp.Format("%d",cre->Hit.GetValue());
	m_hit->SetText((char*)tmp);
	if(cre->Hit.Temp > 0)
		m_hit->SetTextColor(RtwPixel(0xFF00FF00));
	else
		m_hit->SetTextColor(RtwPixel(0xFFFFFFFF));

	tmp.Format("%d",cre->StrPower.GetValue());
	m_strPower->SetText((char*)tmp);
	tmp.Format("%d",cre->ConPower.GetValue());
	m_conPower->SetText((char*)tmp);
	tmp.Format("%d",cre->DexPower.GetValue());
	m_dexPower->SetText((char*)tmp);
	tmp.Format("%d",cre->IntPower.GetValue());
	m_intPower->SetText((char*)tmp);
	tmp.Format("%d",cre->HitPower.GetValue());
	m_hitPower->SetText((char*)tmp);

	// tmp.Format("%d",cre->Attack.GetValue());
	tmp.Format("%d",cre->GetAttack());
	m_attack->SetText((char*)tmp);
	m_attack->SetTextColor(GetColor(cre->Attack.Rate));
	/*
	if (cre->Attack.ColorfulValue!=0 || !IsZero(cre->Attack.Rate-1.0f) )
		m_attack->SetTextColor(RtwPixel(0xFF8B8BE3));
	else 
		m_attack->SetTextColor(RtwPixel(0xFFFFFFFF));
	*/

	tmp.Format("%d-%d",cre->GetDamageMin(),cre->GetDamageMax());
	m_damage->SetText((char*)tmp);
	m_damage->SetTextColor(GetColor(cre->DamageMin.Rate));
	/*
	if (cre->DamageMax.ColorfulValue!=0 || cre->DamageMin.ColorfulValue!=0 || !IsZero(cre->DamageMax.Rate-1.0f) || !IsZero(cre->DamageMin.Rate-1.0f) )
		m_damage->SetTextColor(RtwPixel(0xFF8B8BE3));
	else 
		m_damage->SetTextColor(RtwPixel(0xFFFFFFFF));
	*/

	// tmp.Format("%d",cre->Dodge.GetValue());
	tmp.Format("%d",cre->GetDodge());
	m_dodge->SetText((char*)tmp);
	m_dodge->SetTextColor(GetColor(cre->Dodge.Rate));
	/*
	if (cre->Dodge.ColorfulValue!=0 || !IsZero(cre->Dodge.Rate-1.0f) )
		m_dodge->SetTextColor(RtwPixel(0xFF8B8BE3));
	else 
		m_dodge->SetTextColor(RtwPixel(0xFFFFFFFF));
	*/
	tmp.Format("%d",cre->Armor.GetValue());
	m_armor->SetText((char*)tmp);
	m_armor->SetTextColor(GetColor(cre->Armor.Rate));
	/*
	if (cre->Armor.ColorfulValue!=0 || !IsZero(cre->Armor.Rate-1.0f) )
		m_armor->SetTextColor(RtwPixel(0xFF8B8BE3));
	else 
		m_armor->SetTextColor(RtwPixel(0xFFFFFFFF));
	*/
	tmp.Format("%d",cre->AttSpeed.GetValue());
	// tmp.Format("%d",cre->AttackRange.GetValue());
	m_attSpeed->SetText((char*)tmp);
	m_attSpeed->SetTextColor(GetColor(cre->AttSpeed.Rate));

	if(cre->AttrPoint > 0)
	{
		LOAD_UI("fmcharall.tabcharall.fmchar.btnstradd")->Show();
		LOAD_UI("fmcharall.tabcharall.fmchar.btndexadd")->Show();
		LOAD_UI("fmcharall.tabcharall.fmchar.btnwisadd")->Show();
		LOAD_UI("fmcharall.tabcharall.fmchar.btnhitadd")->Show();
		LOAD_UI("fmcharall.tabcharall.fmchar.btnhealadd")->Show();
	}
	else
	{
		LOAD_UI("fmcharall.tabcharall.fmchar.btnstradd")->Hide();
		LOAD_UI("fmcharall.tabcharall.fmchar.btndexadd")->Hide();
		LOAD_UI("fmcharall.tabcharall.fmchar.btnwisadd")->Hide();
		LOAD_UI("fmcharall.tabcharall.fmchar.btnhitadd")->Hide();
		LOAD_UI("fmcharall.tabcharall.fmchar.btnhealadd")->Hide();
	}

	tmp.Format("%d",cre->ElementDamage[ELEMENT_FIRE].GetValue());
	m_fireDamage->SetText((char*)tmp);
	tmp.Format("%d",cre->ElementDamage[ELEMENT_WATER].GetValue());
	m_waterDamage->SetText((char*)tmp);
	tmp.Format("%d",cre->ElementDamage[ELEMENT_POISON].GetValue());
	m_poisonDamage->SetText((char*)tmp);

	tmp.Format("%d",cre->ElementArmor[ELEMENT_FIRE].GetValue());
	m_fireArmor->SetText((char*)tmp);
	tmp.Format("%d",cre->ElementArmor[ELEMENT_WATER].GetValue());
	m_waterArmor->SetText((char*)tmp);
	tmp.Format("%d",cre->ElementArmor[ELEMENT_POISON].GetValue());
	m_poisonArmor->SetText((char*)tmp);

	if (g_layerMain->m_formUnion)
	{
		if (g_layerMain->m_formUnion->m_unionData.m_nID==0)
			LOAD_UI("fmcharall.tabcharall.fmchar.lbcountryname")->SetText(R(MSG_NO_UNION));
		else 
			LOAD_UI("fmcharall.tabcharall.fmchar.lbcountryname")->SetText(g_layerMain->m_formUnion->m_unionData.m_strName);
	}

	return true;

	unguard;
}

bool UIFormCharProp::Exit()
{
	m_bValid = false;
	return true;
}

void UIFormCharProp::OnClickAddAttr(RtwWidget *sender,void *)
{
	if(!m_bValid) return;
	if(!GetWorld()->m_pPlayer) return;
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;

	if(cre->AttrPoint > 0)
	{
		RtString tmp = sender->GetID().c_str();
        
		CG_CmdPacket *cmd = NetBeginWrite();
		cmd->WriteShort(c2r_add_attr_point);
		if(tmp == "btnstradd")
            cmd->WriteByte(ATTR_STR);
		else if(tmp == "btndexadd")
			cmd->WriteByte(ATTR_DEX);
		else if(tmp == "btnwisadd")
			cmd->WriteByte(ATTR_INT);
		else if(tmp == "btnhitadd")
			cmd->WriteByte(ATTR_HIT);
		else if(tmp == "btnhealadd")
			cmd->WriteByte(ATTR_CON);
		else
			return;
		NetSend(cmd);
	}
}

void UIFormCharProp::OnShow(void *,void *)
{
	Update();
}

bool UIFormCharProp::IsVisible()
{
	return LOAD_UI("fmcharall.tabcharall.fmchar")->IsVisible();
}

// ----------------------------------------------------------------------------
// UI for pet prop show   tim.yang   神兽系统
// ----------------------------------------------------------------------------
UIFormPetProp::UIFormPetProp()
{
	m_bValid = false;
	m_form = NULL;
}

bool UIFormPetProp::Init()
{
	m_pFormAll = LOAD_UI("fmcharall");
	//m_pFormAll->EvKey += RTW_CALLBACK(m_pFormAll, RtwWidget, DefaultKeyDown);

	m_form = LOAD_UI("fmcharall.tabcharall.fmpetsystem");
	if(m_form)
        m_bValid = true;
	else
		m_bValid = false;

	LOAD_UI("fmcharall.tabcharall.fmpetsystem")->EvShow += RTW_CALLBACK(this, UIFormPetProp, OnShow);
	LOAD_UI("fmcharall")->EvShow += RTW_CALLBACK(this, UIFormPetProp, OnShow);

	m_name1    = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbpetname");//名称
	m_name2    = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbpetsex");//名字
	m_fiveElements = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbelements");//五行
	m_expcount = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbexpcount");//经验值
	m_expbar     = LOAD_UI_T(RtwProgressBar,"fmcharall.tabcharall.fmpetsystem.barexp");//经验条
	m_lev     = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lblev");//等级
	m_growth     = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbgrowth");//成长度
	m_feed     = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbfeed");//养成度

	m_attack = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbattackpoint");//伤害
	m_damage = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbdamagepoint");//防御
	m_dodge  = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbdoggepoint");//命中
	m_armor  = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbdefpoint");//闪躲
	m_attSpeed = LOAD_UI("fmcharall.tabcharall.fmpetsystem.lbspeedpoint");//攻击速度

	m_skill1 = LOAD_UI("fmcharall.tabcharall.fmpetsystem.petskill1");//技能1
	m_skill2 = LOAD_UI("fmcharall.tabcharall.fmpetsystem.petskill2");//技能2
	m_skill3 = LOAD_UI("fmcharall.tabcharall.fmpetsystem.petskill3");//技能3
	m_skill4 = LOAD_UI("fmcharall.tabcharall.fmpetsystem.petskill4");//技能4
	m_skill5 = LOAD_UI("fmcharall.tabcharall.fmpetsystem.petskill5");//技能5

	Clear();
	return m_bValid;
}

void UIFormPetProp::OnShow(void *,void *)
{
	Update();
}

bool UIFormPetProp::Exit()
{
	m_bValid = false;
	return true;
}

bool UIFormPetProp::Update()
{
	guard;

	if(!m_bValid) return false;
	if(!IsVisible()) return false;
	if(!GetWorld()->m_pPet || !GetWorld()->m_pPet->m_pNpc) return false;
	SCreature *cre = &GetWorld()->m_pPet->m_core;

	RtString tmp;
	m_form->Show();
	////名称
	//tmp.Format("%s",GetWorld()->m_pPet->m_pNpc->Name.c_str());
	//m_name1->SetText((char*)tmp);
	//名字
// 	if (!GetWorld()->m_pPet->m_core.name1.empty())
// 	{
// 		tmp.Format("%s",GetWorld()->m_pPet->m_core.name1.c_str());
// 	}
	//m_name2->SetText((char*)tmp);
	//
	////五行
 //	switch(/*cre->fiveEle*/1)
 //	{
 //	case 1:
 //		tmp.Format("%s","金");
 //		break;
 //	case 2:
 //		tmp.Format("%s","木");
 //		break;
 //	case 4:
 //		tmp.Format("%s","水");
 //		break;
 //	case 8:
 //		tmp.Format("%s","火");
 //		break;
 //	case 16:
 //		tmp.Format("%s","土");
 //		break;
 //	default:
 //		tmp.Format("%s","");
 //		break;
 //	}
 //	if (/*cre->fiveEle*/1)
 //	{
 //		m_fiveElements->SetText((char*)tmp);
 //	}
 //	setprecision(2);
 //	//成长
 //	if (/*cre->growth*/1)
 //	{
 //		tmp.Format("%f",1.f/*cre->growth*//50.0f);
 //		m_growth->SetText((char*)tmp);
 //	}
 //	else
 //	{
 //		tmp.Format("%f",0.0);
 //		m_growth->SetText((char*)tmp);
 //	}
 //	//等级
 //	tmp.Format("%d",cre->Lev);
 //	m_lev->SetText((char*)tmp);
 //	//养成
 //	if (/*cre->feed*/1)
 //	{
 //		tmp.Format("%d%%",1/*cre->feed*/);
 //		m_feed->SetText((char*)tmp);
 //	}
 //	else
 //	{
 //		tmp.Format("%d%%",0);
 //		m_feed->SetText((char*)tmp);
 //	}
 //	//经验值  && 经验条
 //	double expRate = (double)cre->Exp / g_TableExp.GetNextLevExp(cre->Lev);
 //	m_expbar->SetValue(expRate);
 //	if(cre->Lev > 75)
 //	{
 //		tmp.Format("%.4f%%",expRate*100);
 //		// m_exp->SetTextColor(RtwPixel(0xFFFF0000));
 //	}
 //	else
 //	{
 //		// m_exp->SetTextColor(RtwPixel(0xFFFFFFFF));
 //		tmp.Format("%.2f%%",expRate*100);
 //	}
 //	m_expcount->SetText((char*)tmp);
 //	//属性
 //	tmp.Format("%d",cre->Attack.GetValue());
 //	m_attack->SetText((char*)tmp);
 //	m_attack->SetTextColor(RtwPixel(0xFFFF00FF));
 //
 //	tmp.Format("%d",cre->DamageMax.GetValue());
 //	m_damage->SetText((char*)tmp);
 //	m_damage->SetTextColor(RtwPixel(0xFFFFFF00));
 //
 //	tmp.Format("%d",cre->Dodge.GetValue());
 //	m_dodge->SetText((char*)tmp);
 //	m_dodge->SetTextColor(RtwPixel(0xFFFF0FF0));
 //
 //	tmp.Format("%d",cre->Armor.GetValue());
 //	m_armor->SetText((char*)tmp);
 //	m_armor->SetTextColor(RtwPixel(0xFF00F00F));
 //
 //	tmp.Format("%d",cre->AttSpeed.GetValue());
 //	m_attSpeed->SetText((char*)tmp);
 //	m_attSpeed->SetTextColor(RtwPixel(0xFF00F0FF));
 
 	////技能
 	//SetPetSkillIcon(m_skill1,cre->skill[0]);
 	//SetPetSkillIcon(m_skill2,cre->skill[1]);
 	//SetPetSkillIcon(m_skill3,cre->skill[2]);
 	//SetPetSkillIcon(m_skill4,cre->skill[3]);
 	//SetPetSkillIcon(m_skill5,cre->skill[4]);
	return true;

	unguard;
}

bool UIFormPetProp::SetPetSkillIcon(RtwWidget* skill,int skillid)
{
	SSkill* pAttr = 0;
	pAttr = Skill()->FindSkill(skillid);
	if (pAttr)
	{
		skill->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pAttr->szIcon));
		skill->ModifyFlags(wfHint,0);
		char buf[256];
		memset(buf,sizeof(buf),0);
		rt2_sprintf(buf,"技能名称:%s\r\n消耗MP:%d\r\n技能描述:%s",pAttr->szName,pAttr->iMP,pAttr->szDescription);
		skill->SetHintText(buf);
	}
	else
	{
		skill->SetBackgroundImage((RtwImage*)NULL);
		skill->SetHintText("");
	}
	return true;
}

bool UIFormPetProp::Clear()
{
	guard;

	//if(!m_bValid) return false;
	//if(!IsVisible()) return false;
	//if(!GetWorld()->m_pPet) return false;
	RtString tmp;
	//名称
	tmp.Format("%s","");
	m_name1->SetText((char*)tmp);
	//名字
	m_name2->SetText((char*)tmp);
	//五行
	m_fiveElements->SetText((char*)tmp);
	//成长
	m_growth->SetText((char*)tmp);
	//等级
	m_lev->SetText((char*)tmp);
	//养成
	m_feed->SetText((char*)tmp);
	//经验值  && 经验条
	double expRate = 0;
	m_expbar->SetValue(expRate);
	m_expcount->SetText((char*)tmp);
	//属性
	m_attack->SetText((char*)tmp);
	m_damage->SetText((char*)tmp);
	m_dodge->SetText((char*)tmp);
	m_armor->SetText((char*)tmp);
	m_attSpeed->SetText((char*)tmp);
	//技能
	m_skill1->SetBackgroundImage((RtwImage*)NULL);
	m_skill2->SetBackgroundImage((RtwImage*)NULL);
	m_skill3->SetBackgroundImage((RtwImage*)NULL);
	m_skill4->SetBackgroundImage((RtwImage*)NULL);
	m_skill5->SetBackgroundImage((RtwImage*)NULL);
	return true;

	unguard;
}

bool UIFormPetProp::IsVisible()
{
	return LOAD_UI("fmcharall.tabcharall.fmpetsystem")->IsVisible();
}
//end

// ----------------------------------------------------------------------------
// UI for pet simple show
// ----------------------------------------------------------------------------
UIFormPetSimple::UIFormPetSimple()
{
	m_form   = NULL;
	m_bValid = false;
	m_formSetb=false;
}

UIFormPetSimple::~UIFormPetSimple()
{

}

bool UIFormPetSimple::Init()
{
	m_form = LOAD_UI("fmhpc");
	m_formHp  = LOAD_UI("fmhpc.fmhpbar");
    
	if(m_form)
		m_bValid = true;
	else
		m_bValid = false;

	m_name    = LOAD_UI("fmhpc.lblvtc");
	m_lev     = LOAD_UI("fmhpc.lblvc");
	m_expRate = LOAD_UI("fmhpc.fmhpbar.lbexpcountc");
	m_pPetFace= LOAD_UI("fmhpc.lbpartface");
	m_hp	= LOAD_UI("fmhpc.fmhpbar.lbhpcountc");
	m_mp    = LOAD_UI("fmhpc.fmhpbar.lbmpcountc");
	m_hpBar  = LOAD_UI_T(RtwProgressBar,"fmhpc.fmhpbar.barhpc");
	m_mpBar  = LOAD_UI_T(RtwProgressBar,"fmhpc.fmhpbar.barmpc");
	m_expBar = LOAD_UI_T(RtwProgressBar,"fmhpc.fmhpbar.barexpc");

	m_form->EvLClick += RTW_CALLBACK(this, UIFormPetSimple, OnLClickUI);
	m_formHp->EvLClick += RTW_CALLBACK(this, UIFormPetSimple, OnLClickUI);
	m_pPetFace->EvLClick += RTW_CALLBACK(this, UIFormPetSimple, OnLClickUI);
	
	return m_bValid;
}

void UIFormPetSimple::OnSelectYes(void *,void *)
{
	//NotifyServer(); // gao
}

void UIFormPetSimple::OnSelectNo(void *,void *)
{
	//NotifyServer(); //gao
}

void UIFormPetSimple::OnClickAI(void *,void *)
{
}

void UIFormPetSimple::OnLClickUI(void *,void *)
{
	if ( !GetWorld()->m_pPet )
		return;

	GetPlayer()->OnSetTarget(GetWorld()->m_pPet->ID());
}

void UIFormPetSimple::NotifyServer()
{
	/*int idx = m_expSetting->GetSelectedItem();
	if(idx == -1)
		return;

	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_set_pet_info);
	switch(idx)
	{
		case 0: cmd->WriteLong(20); break;
		case 1: cmd->WriteLong(35); break;
		case 2: cmd->WriteLong(50); break;
		default: cmd->WriteLong(20); break;
	}

	if(m_ckYes->GetChecked())
		cmd->WriteLong(1);
	else
		cmd->WriteLong(0);

	NetSend(cmd);*/ // gao ai取消
}

void UIFormPetSimple::OnChangeExpSetting(void *, void *)
{
	NotifyServer();
}

bool UIFormPetSimple::Update()
{
	guard;
	if(GetWorld()->m_pPet==NULL || !GetWorld()->m_pPet->m_pNpc) 
	{
		m_form->Hide();
		return false;
	}
	else
	{
		//召唤出宠物的新手提示
		GetWorld()->ShowPetInfo();
	}
	m_formHp->Show();	
	SCreature *cre = &GetWorld()->m_pPet->m_core;
	
	RtString tmp;
	tmp.Format("%s",cre->Name.c_str());
	m_name->SetText((char*)tmp);
	tmp.Format("%d",cre->Lev);
	m_lev->SetText((char*)tmp);

	float expRate = (float)cre->Exp / ((float)g_TableExp.GetNextLevExp(cre->Lev) / 3.f);	
    m_expBar->SetValue(expRate);
	m_hpBar->SetValue((float)cre->GetHp()/cre->GetMaxHp());
	m_mpBar->SetValue((float)cre->GetMp()/cre->GetMaxMp());

	if(cre->Lev > 75)
	{
		tmp.Format("%.4f%%",expRate*100);
		//m_expRate->SetTextColor(RtwPixel(0xFFFF0000));
	}
	else
	{
		m_expRate->SetTextColor(RtwPixel(0xFFFFFFFF));
		tmp.Format("%.2f%%",expRate*100);
	}
	m_expRate->SetText((char*)tmp);
	
	tmp.Format("%d/%d",cre->GetHp(),cre->GetMaxHp());
	m_hp->SetText((char*)tmp);
	tmp.Format("%d/%d",cre->GetMp(),cre->GetMaxMp());
	m_mp->SetText((char*)tmp);
	m_form->Show();
	m_hp->Show();
	m_mp->Show();
	m_mpBar->Show();
	m_expBar->Show();
	m_expRate->Show();

	RtwImage* pImageFace = g_workspace.getImageFactory()->createImage(GetWorld()->m_pPet->m_pNpc->HeadPic);
	if (!pImageFace)
	{
		return true;
	}
	pImageFace->SetBlend(true);
	m_pPetFace->SetBackgroundImage(pImageFace);
	return true;
	unguard;
}

bool UIFormPetSimple::Exit()
{

	return true;
}

void UIFormPetSimple::SetVisible(bool visible)
{
	if(visible)
		m_form->Show();
	else
		m_form->Hide();
}

// ----------------------------------------------------------------------------
// UI for char simple show
// ----------------------------------------------------------------------------
UIFormCharSimple::UIFormCharSimple()
{
	m_form   = NULL;
	m_bValid = false;
	bShowScroll = true;
	m_bShowPower = false;
	bUpdateImage = true;
}

UIFormCharSimple::~UIFormCharSimple()
{

}

void UIFormCharSimple::OnTimerOK(void*, void*)
{
	m_ImageSequence->Pause();
	m_ImageCucurbit->SetCount(0);

	m_lblPowerCucurbit->Show();
	m_lblPowerSequence->Hide();
}


bool UIFormCharSimple::Init()
{
	m_form = LOAD_UI("fmhp");
	if(m_form)
		m_bValid = true;
	else
		m_bValid = false;

	m_name    = LOAD_UI("fmhp.lbname");
	m_lev     = LOAD_UI("fmhp.lblv");
	m_PlayerImage = LOAD_UI("fmhp.lbpicrole");
	m_expRate = LOAD_UI("fmhotkey.lbexpcount"); //change by 李泽华 2010.05.05
	m_hp	= LOAD_UI("fmhp.lbhpcount");
	m_mp    = LOAD_UI("fmhp.lbmpcount");
	m_ep    = LOAD_UI("fmhp.lbnpcount");

	m_hpBar  = LOAD_UI_T(RtwProgressBar,"fmhp.barhp");
	m_mpBar  = LOAD_UI_T(RtwProgressBar,"fmhp.barmp");
	m_expBar = LOAD_UI_T(RtwProgressBar,"fmhotkey.barexp");
	m_epBar  = LOAD_UI_T(RtwProgressBar,"fmhp.barnp");

// 	ldr123 能量条new
	m_fmPower		=	LOAD_UI("fmhp.fmPower");
	m_lblPowerSequence= LOAD_UI_T(RtwLabel, "fmhp.fmPower.lblPowerSequence"); 
	m_lblPowerCucurbit= LOAD_UI_T(RtwLabel, "fmhp.fmPower.lblPowerCucurbit"); 
	m_ImageSequence = g_workspace.getImageFactory()->createImageSequence();
	m_ImageCucurbit = g_workspace.getImageFactory()->createImageSequence();

	RtwImage *pImage;
	char szBufTemp[128];
	rt2_sprintf(szBufTemp, "ui\\x_textures\\power\\PowerEffect.dds");

	for (int i=0; i<10; ++i)
	{
		for (int j=0; j<5; ++j)
		{
			pImage = g_workspace.getImageFactory()->createImage();
			if(pImage->LoadFromFile(szBufTemp))
			{
				pImage->SetSourceRect(RtwRect(j*410, i*205, (j+1)*409, (i+1)*204));
				m_ImageSequence->AddImage(pImage, 16);
				DROP_TRUI_IMAGE(pImage);
			}
		}
	}

	m_ImageSequence->SetBlend(true);
   	m_ImageSequence->m_CountEvent[1] += RTW_CALLBACK(this, UIFormCharSimple, OnTimerOK);
	m_lblPowerSequence->SetBackgroundImage(m_ImageSequence);
	m_ImageSequence->SetCount();
	m_ImageSequence->Pause();
 	m_lblPowerSequence->Hide();

	rt2_sprintf(szBufTemp, "ui\\x_textures\\power\\Power.dds");

	for (int i=0; i<2; i++)
	{
		for (int j=0; j<8; j++)
		{
			pImage = g_workspace.getImageFactory()->createImage();
			if(pImage->LoadFromFile(szBufTemp))
			{
				pImage->SetSourceRect(RtwRect(j*64, i*128, (j+1)*64, (i+1)*128));
				m_ImageCucurbit->AddImage(pImage, 30);
				DROP_TRUI_IMAGE(pImage);
			}
		}
	}
	m_ImageCucurbit->SetBlend(true);

	m_lblPowerCucurbit->SetBackgroundImage(m_ImageCucurbit);
 	m_lblPowerCucurbit->Hide();
	m_fmPower->Show();
// 	end

	m_dynTaskTrack	= LOAD_UI_T(RtwHtmlView, "fmtasktrack.fmtrack.htmltrack");
	m_dynTaskTrack->ModifyFlags(0,wfInput);
	m_dynTaskTrack->ModifyFlags(wfGrabMouse,0);
	m_dynTaskTrack->SetEnableInput(false);
	m_dynTaskTrack->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlTask_HtmlHelpView);
	//任务追踪滚动条
	m_taskTrackScroll = LOAD_UI_T(RtwVScrollBar, "fmtasktrack.scrolltask");
	//m_taskTrackScroll->EvMouseMove += RTW_CALLBACK(this, UIFormCharSimple, OnScroll_MouseMove);
	m_taskTrackScroll->EvScroll += RTW_CALLBACK(this, UIFormCharSimple, OnScroll_TaskTrack);

	LOAD_UI("fmtasktrack.btntasktrack")->EvLClick += RTW_CALLBACK(this, UIFormCharSimple, OnClicked_HideTrack);
	LOAD_UI("fmtasktrack.btntasktrack2")->EvLClick += RTW_CALLBACK(this, UIFormCharSimple, OnClicked_ShowTrack);
	m_form->EvLClick								+=RTW_CALLBACK(this,UIFormCharSimple,OnSetTargetSelf);

	SetTrackVisible(true);
	bCheckTrack = true;

	return m_bValid;
}

void UIFormCharSimple::OnSetTargetSelf(void*,void*)
{
	guard;
	GetPlayer()->OnSetTarget(GetPlayer()->ID());
	unguard;
}

void UIFormCharSimple::SetPowerValue(float fVal)
{
   	if (fVal < 0.01)
   	{
   		fVal = 0.0;
   	}
   
   	if (fVal>0.99 && !m_bShowPower)
 	{
 		//能量值满
		m_ImageSequence->SetCount();
		m_ImageSequence->Run();
 		m_lblPowerSequence->Show();
		m_bShowPower = true;
 		return;
 	}

	if (fVal < 0.99f)
	{
 		if (m_lblPowerSequence->IsVisible())
 		{
			m_lblPowerSequence->Hide();
 		}

		if (m_lblPowerCucurbit->IsVisible())
		{
			m_lblPowerCucurbit->Hide();
		}

		m_bShowPower = false;
	}
}

void UIFormCharSimple::TrackTaskItem(DWORD id)
{
	if(!GetWorld()->m_pPlayer) return;
	CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;

	std::list<STaskInfo>::iterator it;
	for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
	{
		STaskDesc *desc = g_TableTask.FindTask((*it).Id);
		if(!desc) continue;

		if (id == desc->id)
		{
			DWORD mTrackId = (*it).Id;
			if (it->IsTrack())
			{
				it->CaneclTrack();
				CG_CmdPacket *cmd = NetBeginWrite();
				cmd->WriteShort(c2r_task_canecl_track);
				cmd->WriteLong((*it).Id);
				NetSend(cmd);
			} 
			else
			{
				it->SetTrack();
				CG_CmdPacket *cmd = NetBeginWrite();
				cmd->WriteShort(c2r_task_track);
				cmd->WriteLong((*it).Id);
				NetSend(cmd);
			}
		}
	}
}

void UIFormCharSimple::BuildTaskTrackList()
{
	if(!GetWorld()->m_pPlayer) return;
	CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;

	//输出任务追踪内容
	int nTrackNum = 0;
	std::string strTrack = "";
	std::string strContent = "";

	strTrack = "<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\"/>\n</head>\n<body>\n";

	std::list<STaskInfo>::iterator it;
	for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
	{
		if (it->IsTrack())
		{
			if (it->IsDoing == 3)
			{
				nTrackNum++;
				STaskDesc *desc = g_TableTask.FindTask((*it).Id);
				strContent += "<p><font color =\"#00AA00\">";
				strContent += desc->name;
				strContent += "</font></p>";
				for (size_t i=0; i< desc->dynamicText2.size(); ++i)
				{
					strContent += "<p>";
					strContent += "--";
					strContent += desc->dynamicText2[i].key;
					strContent += " ";
					for (size_t j=0; j<MAX_TASK_PARAM_SIZE; ++j)
					{
						strContent += desc->dynamicText2[i].param[j];
						if (!desc->dynamicText2[i].param[j].empty())
						{
							strContent += " ";
						}
					}

					strContent += "</p>";
				}
			} 
			else
			{
				nTrackNum++;
				STaskDesc *desc = g_TableTask.FindTask((*it).Id);
				strContent += "<p><font color =\"#00AA00\">";
				strContent += desc->name;
				strContent += "</font></p>";
				for (size_t i=0; i< desc->dynamicText.size(); ++i)
				{
					strContent += "<p>";
					strContent += "--";
					strContent += desc->dynamicText[i].key;
					strContent += " ";
					for (size_t j=0; j<MAX_TASK_PARAM_SIZE; ++j)
					{
						strContent += desc->dynamicText[i].param[j];
						if (!desc->dynamicText[i].param[j].empty())
						{
							strContent += " ";
						}
					}

					int num = 0;
					RtString tmp;
					if (desc->dynamicNum.empty())
					{
						if (desc->taskReq[i].key == "killmonster")
						{
							num = atoi(desc->taskReq[i].param[0].c_str());
							if(num != 0)
							{
								tmp.Format(":%d/%d",it->GetCurKillMonsterNum(),num);
							}
						}
						else if (desc->taskReq[i].key == "useitem")
						{
							num = atoi(desc->taskReq[i].param[0].c_str());
							if (it->IsUseItemByID(num))
							{
								tmp.Format(":1/1");
							} 
							else
							{
								tmp.Format(":0/1");
							}
						}
					} 
					else
					{
						strContent += ":";
						GetWorld()->m_pPlayer->mItem.m_MissionBag.FindType(atoi(desc->dynamicItem[i].key.c_str()),ItemColor_All,&num);
						if(num == 0) GetWorld()->m_pPlayer->mItem.m_Bag.FindType(atoi(desc->dynamicItem[i].key.c_str()),ItemColor_All,&num);
						tmp.Format("%d/%d",num,atoi(desc->dynamicNum[i].key.c_str()));
					}
					strContent += tmp;
					strContent += "</p>";
				}
			}
		}
	}

	if (bCheckTrack)
	{
		if (nTrackNum > 0)
		{
			SetTrackVisible(true);
		} 
		else if(nTrackNum == 0)
		{
			SetTrackVisible(false);
		}
	} 
	else
	{
		SetTrackVisible(false);
	}

	//滚动条是否显示
	int nLine = m_dynTaskTrack->getLineCount();
	if (nLine < 16)
	{
		m_taskTrackScroll->Hide();
	} 
	else
	{
		if (bShowScroll)
		{
			m_taskTrackScroll->Show();
		}

	}

	strTrack += strContent;
	strTrack += "</body>\n</html>\n";

	if (strLastTrack != strTrack)
	{
		if (!strContent.empty())
		{
			((RtwChatBox*)m_dynTaskTrack)->SetScrollPosition(0);
			m_dynTaskTrack->LoadFromString(strTrack);
			((RtwChatBox*)m_dynTaskTrack)->SetScrollPosition(m_taskTrackScroll->GetValue());
			//新手提示
			GetWorld()->ShowTaskGuide();
		}
	}

	//保存上一次追踪的内容
	strLastTrack.clear();
	strLastTrack = strTrack;
}

void UIFormCharSimple::OnScroll_TaskTrack(void*, void*)
{
	((RtwChatBox*)m_dynTaskTrack)->SetScrollPosition(m_taskTrackScroll->GetValue());
}

void UIFormCharSimple::OnScroll_MouseMove(void*, void*)
{
	//if (m_taskTrackScroll->IsVisible())
	//{
	//	m_taskTrackScroll->Show();
	//} 
	//else
	//{
	//	m_taskTrackScroll->Hide();
	//}
}

bool UIFormCharSimple::Update()
{
	guard;
	static int nUserLev = 0;

	if(!m_bValid) return false;
	if(!GetWorld()->m_pPlayer) return false;
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;

	RtString tmp;
	tmp.Format("%s",cre->Name.c_str());
	m_name->SetText((char*)tmp);

	if (nUserLev != cre->Lev)
	{
		nUserLev = cre->Lev;

		//新手升级提示
		if (GetWorld()->m_pPlayer->m_core.MetierLev <= 35)
		{
			//三十五级以下弹提示框
			g_layerMain->SetCountDownFormVisable(true);
		}
		g_layerMain->ShowCountDownForm();
	}

	tmp.Format("%d",cre->Lev);
	m_lev->SetText((char*)tmp);
	
	double expRate = (double)cre->Exp / g_TableExp.GetNextLevExp(cre->Lev);
	m_expBar->SetValue((float)expRate);
	m_hpBar->SetValue((float)cre->GetHp()/cre->GetMaxHp());
	m_mpBar->SetValue((float)cre->GetMp()/cre->GetMaxMp());
 	m_epBar->SetValue((float)cre->GetEnP()/cre->GetMaxEnP());
	//ldr123 怒气new
	SetPowerValue((float)cre->GetEnP()/cre->GetMaxEnP());

	//gao 2009.12.24 此处 暂时取消经验的百分比显示
	//if(cre->Lev > 75)
	//{
	//	tmp.Format("%.4f%%",expRate*100);
	//	//m_expRate->SetTextColor(RtwPixel(0xFFFF0000));
	//}
	//else
	//{
	//	m_expRate->SetTextColor(RtwPixel(0xFFFFFFFF));
	//	tmp.Format("%.2f%%",expRate*100);
	//}
	//m_expRate->SetText((char*)tmp);
	tmp.Format("%d/%d",cre->GetHp(),cre->GetMaxHp());
	m_hp->SetText((char*)tmp);
	tmp.Format("%d/%d",cre->GetMp(),cre->GetMaxMp());
	m_mp->SetText((char*)tmp);
	tmp.Format("%d/%d",cre->GetEnP(),cre->GetMaxEnP());
	m_ep->SetText((char*)tmp);
	//经验条文字显示 add by 李泽华 2010.05.05
	tmp.Format("%d/%d",cre->Exp,g_TableExp.GetNextLevExp(cre->Lev));
	m_expRate->SetText((char*)tmp);

	//人物头像
	vector<SCharImage> images;
	if (!g_TableCharImage.GetCharImage(cre->actorID, images))
	{
		RtCoreLog().Info("ActorID传错啦！\n");
		return false;
	}

	if(bUpdateImage)
	{
		std::string strImage = "ui/ui_texture/";
		strImage += images[cre->HeadImageID].imageGame;
		strImage += ".dds";

		RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
		if(pImage)
		{
			bUpdateImage = false;

			pImage->SetBlend(true);
			m_PlayerImage->SetBackgroundImage(pImage);
		}
		else
		{
			RtCoreLog().Info("找不到文件:%s\n", strImage);
		}
	}
	return true;

	unguard;
}

bool UIFormCharSimple::UpdateTaskTrackList()
{
	BuildTaskTrackList();
	return true;
}
bool UIFormCharSimple::Exit()
{
	return true;
}


void UIFormCharSimple::SetVisible(bool visible)
{
	if(visible)
		m_form->Show();
	else
		m_form->Hide();
}
 
bool UIFormCharSimple::IsTrackVisible()
{
	if (LOAD_UI("fmtasktrack")->IsVisible())
	{
		return true;
	} 
	else
	{
		return false;
	}
}

void UIFormCharSimple::SetTrackVisible(bool bVisible)
{
	if (bVisible)
	{
		LOAD_UI("fmtasktrack")->Show();
	} 
	else
	{
		LOAD_UI("fmtasktrack")->Hide();
	}
}

void UIFormCharSimple::OnClicked_HideTrack(void*, void*)
{
	bShowScroll = false;
	LOAD_UI("fmtasktrack.btntasktrack")->Hide();
	LOAD_UI("fmtasktrack.btntasktrack2")->Show();
	LOAD_UI("fmtasktrack.fmtrack")->Hide();
	if (m_taskTrackScroll->IsVisible())
	{
		m_taskTrackScroll->Hide();
	}

}
void UIFormCharSimple::OnClicked_ShowTrack(void*, void*)
{
	bShowScroll = true;
	LOAD_UI("fmtasktrack.btntasktrack")->Show();
	LOAD_UI("fmtasktrack.btntasktrack2")->Hide();
	LOAD_UI("fmtasktrack.fmtrack")->Show();
	//if (m_taskTrackScroll->IsVisible())
	//{
	//	m_taskTrackScroll->Show();
	//}

	//if (LOAD_UI("fmtasktrack.fmtrack")->IsVisible())
	//{
	//	LOAD_UI("fmtasktrack.fmtrack")->Hide();
	//} 
	//else
	//{
	//	LOAD_UI("fmtasktrack.fmtrack")->Show();
	//}
	//if (m_taskTrackScroll->IsVisible())
	//{
	//	m_taskTrackScroll->Hide();
	//}
	//else
	//{
	//	m_taskTrackScroll->Show();
	//}
}

void UIFormCharSimple::SetCheckTrack(bool bCheck)
{
	bCheckTrack = bCheck;
}
// ----------------------------------------------------------------------------
// UI for magic weapon add mp
// ----------------------------------------------------------------------------
UIFormMagicWeaponAddMp::UIFormMagicWeaponAddMp()
{
	m_form   = NULL;
	m_bValid = false;
}

UIFormMagicWeaponAddMp::~UIFormMagicWeaponAddMp()
{

}

bool UIFormMagicWeaponAddMp::Init()
{
	m_form = LOAD_UI("fmfabaomp");
	if(m_form)
		m_bValid = true;
	else
		m_bValid = false;

	m_form->EvHide += RTW_CALLBACK(this, UIFormMagicWeaponAddMp, OnHide);

	m_name    = LOAD_UI("fmfabaomp.panfabaomp.lbfabaoname");
	m_lev     = LOAD_UI("fmfabaomp.panfabaomp.lbfabaolev");
	m_mp      = LOAD_UI("fmfabaomp.panfabaomp.lbfabaomp");
	m_icon    = LOAD_UI_T(RtwButton,"fmfabaomp.panfabaomp.btnfabao");

	m_hostMpBarMsg = LOAD_UI("fmfabaomp.lbprimenum");
	m_mpBarMsg	   = LOAD_UI("fmfabaomp.lbmpnum");
		
	m_mpBar		  = LOAD_UI_T(RtwProgressBar,"fmfabaomp.barmp");
	m_hostMpBar   = LOAD_UI_T(RtwProgressBar,"fmfabaomp.barprime");

	m_mpBar->SetValue(0);
	m_hostMpBar->SetValue(0);
	return m_bValid;
}

bool UIFormMagicWeaponAddMp::Update()
{
	guard;

	if(!m_bValid) return false;
	if(!GetWorld()->m_pPlayer) 
	{
		m_form->Hide();
		return false;
	}
	// m_form->Show();
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;
	SItemID item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
	if(!ItemID_IsValid(item)) return false;
	if(!ItemIsWeapon_MagicWeapon(item.type)) return false;

	SWeapon *pWea = (SWeapon *)GetItemMgr()->GetItem(item.type);
	if(!pWea) return false;

	RtString tmp;
	tmp.Format("%s",pWea->name);
	m_name->SetText((char*)tmp);
	tmp.Format("%d",item.params[MW_PARAM_LEV]);
	m_lev->SetText((char*)tmp);
	
	int maxMp;
	MW_GetInfo(item,maxMp);
	m_hostMpBar->SetValue((float)cre->GetMp()/cre->GetMaxMp());
	m_mpBar->SetValue((float)item.params[MW_PARAM_MP]/maxMp);

	tmp.Format("%.2f%%",(float)item.params[MW_PARAM_MP]/maxMp*100);
	m_mpBarMsg->SetText((char*)tmp);

	tmp.Format("%d/%d",item.params[MW_PARAM_MP],maxMp);
	m_mp->SetText((char*)tmp);
	
	tmp.Format("%.2f%%",(float)cre->GetMp()/cre->GetMaxMp()*100);
	m_hostMpBarMsg->SetText((char*)tmp);

	if(m_iconName != pWea->icon)
	{
// change [3/20/2009 tooth.shi]        RtwImage* pImage1 = new RtwImage(pWea->icon);//imageimage
		RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pWea->icon);
		m_icon->SetBackgroundImage(pImage1);
		m_iconName = pWea->icon;
	}
	return true;

	unguard;
}

bool UIFormMagicWeaponAddMp::Exit()
{

	return true;
}

void UIFormMagicWeaponAddMp::SetVisible(bool visible)
{
	if(visible)
		m_form->Show();
	else
		m_form->Hide();
}

bool UIFormMagicWeaponAddMp::GetVisible()
{
	return m_form->IsVisible();
}

void UIFormMagicWeaponAddMp::OnHide(void *,void *)
{
	if(!GetWorld()) return;
	if(!GetWorld()->m_pPlayer) return;

	GetWorld()->m_pPlayer->MagicWeaponEndAddMp();
}

/*
void UIFormMagicWeaponAddMp::OnHide(void *,void *)
{
	if(!GetWorld()) return;
	if(!GetWorld()->m_pPlayer) return;

	GetWorld()->m_pPlayer->MagicWeaponBeginAddMp();
}
*/

// ----------------------------------------------------------------------------
// UI for magic weapon add element
// ----------------------------------------------------------------------------
UIFormMagicWeaponAddEle::UIFormMagicWeaponAddEle()
{
	m_form   = NULL;
	m_bValid = false;
}

UIFormMagicWeaponAddEle::~UIFormMagicWeaponAddEle()
{

}

bool UIFormMagicWeaponAddEle::Init()
{
	m_form = LOAD_UI("fmfabao");
	if(m_form)
		m_bValid = true;
	else
		m_bValid = false;

	LOAD_UI("fmfabao")->EvHide += RTW_CALLBACK(this, UIFormMagicWeaponAddEle, OnHide);
	LOAD_UI("fmfabao")->EvShow += RTW_CALLBACK(this, UIFormMagicWeaponAddEle, OnShow);
	LOAD_UI("fmfabao.btnwater")->EvLClick += RTW_CALLBACK(this, UIFormMagicWeaponAddEle, OnClickAddEle);
	LOAD_UI("fmfabao.btnfire")->EvLClick += RTW_CALLBACK(this, UIFormMagicWeaponAddEle, OnClickAddEle);
	LOAD_UI("fmfabao.btnposion")->EvLClick += RTW_CALLBACK(this, UIFormMagicWeaponAddEle, OnClickAddEle);

	m_name					= LOAD_UI("fmfabao.panfabao.lbfabaoname");
	m_icon					= LOAD_UI_T(RtwButton,"fmfabao.panfabao.btnfabao");
	m_lev					= LOAD_UI("fmfabao.panfabao.lbfabaolev");
	m_mp					= LOAD_UI("fmfabao.panfabao.lbfabaomp");
	m_rate[ELEMENT_WATER]   = LOAD_UI("fmfabao.lbsucwaternum");
	m_rate[ELEMENT_FIRE]    = LOAD_UI("fmfabao.lbsucfirenum");
	m_rate[ELEMENT_POISON]  = LOAD_UI("fmfabao.lbsucposionnum");

	m_value[ELEMENT_WATER]   = LOAD_UI("fmfabao.lbwaternownum");
	m_value[ELEMENT_FIRE]    = LOAD_UI("fmfabao.lbfirenownum");
	m_value[ELEMENT_POISON]  = LOAD_UI("fmfabao.lbposionnownum");
	
	m_bar[ELEMENT_WATER]  = LOAD_UI_T(RtwProgressBar,"fmfabao.barwater");
	m_bar[ELEMENT_FIRE]   = LOAD_UI_T(RtwProgressBar,"fmfabao.barfire");
	m_bar[ELEMENT_POISON] = LOAD_UI_T(RtwProgressBar,"fmfabao.barposion");

	m_cost[ELEMENT_WATER]  = LOAD_UI("fmfabao.lbwaterneednum");
	m_cost[ELEMENT_FIRE]   = LOAD_UI("fmfabao.lbfireneednum");
	m_cost[ELEMENT_POISON] = LOAD_UI("fmfabao.lbposionneednum");

	m_bar[ELEMENT_WATER]->SetValue(0); 
	m_bar[ELEMENT_FIRE]->SetValue(0);
	m_bar[ELEMENT_POISON]->SetValue(0);
	
	return m_bValid;
}

bool UIFormMagicWeaponAddEle::Update()
{
	guard;

	if(!m_bValid) return false;
	if(!GetWorld()->m_pPlayer) 
	{
		m_form->Hide();
		return false;
	}

	SItemID item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
	if(!ItemID_IsValid(item)) return false;
	if(!ItemIsWeapon_MagicWeapon(item.type)) return false;

	SWeapon *pWea = (SWeapon *)GetItemMgr()->GetItem(item.type);
	if(!pWea) return false;

	int maxMp;
	MW_GetInfo(item,maxMp);

	RtString tmp;
	float rate;
	long  time,mp;
	bool  lost;
	int   ele[MAX_ELEMENT_ATTR];
	MW_GetEleValue(item,ele);
	for(int i=0; i<MAX_ELEMENT_ATTR; ++i)
	{
		MW_GetAddEleInfo(item,i,rate,time,mp,lost);
		int dr = rate * 100;
		tmp.Format("%d%%",dr);
		m_rate[i]->SetText((char*)tmp);
		tmp.Format("%d",ele[i]);
		m_value[i]->SetText((char*)tmp);
		tmp.Format("%d",mp);
		m_cost[i]->SetText((char*)tmp);
	}
	
	tmp.Format("%s",pWea->name);
	m_name->SetText((char*)tmp);
	tmp.Format("%d",item.params[MW_PARAM_LEV]);
	m_lev->SetText((char*)tmp);
	tmp.Format("%d / %d",item.params[MW_PARAM_MP],maxMp);
    m_mp->SetText((char*)tmp);
	
	if(m_iconName != pWea->icon)
	{
// change [3/20/2009 tooth.shi]		RtwImage* pImage1 = new RtwImage(pWea->icon);//imageimage
		RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pWea->icon);
		m_icon->SetBackgroundImage(pImage1);
		m_iconName = pWea->icon;
	}
	return true;
	
	unguard;
}

bool UIFormMagicWeaponAddEle::Update(int type,float rate)
{
	guard;

	if(!m_bValid) return false;
	if(!GetWorld()->m_pPlayer) 
	{
		m_form->Hide();
		return false;
	}
	SCreature *cre = &GetWorld()->m_pPlayer->m_core;

	for(int i=0 ;i<MAX_ELEMENT_ATTR; ++i)
	{
		if(i == type)
		{
			m_bar[i]->SetValue(rate);
			m_bar[i]->Show();
		}
		else
		{
			m_bar[i]->SetValue(0);
			m_bar[i]->Hide();
		}
	}
	return true;
	unguard;
}

bool UIFormMagicWeaponAddEle::Exit()
{
	return true;
}

void UIFormMagicWeaponAddEle::SetVisible(bool visible)
{
	if(visible)
		m_form->Show();
	else
		m_form->Hide();
}

bool UIFormMagicWeaponAddEle::GetVisible()
{
	return m_form->IsVisible();
}

void UIFormMagicWeaponAddEle::OnHide(void *,void *)
{
	if(!GetWorld()) return;
	if(!GetWorld()->m_pPlayer) return;

	GetWorld()->m_pPlayer->MagicWeaponEndAddEle();
}

void UIFormMagicWeaponAddEle::OnMultiAddEle(void *ele,void *)
{
	if(!GetWorld()) return;
	if(!GetWorld()->m_pPlayer) return;

	GetWorld()->m_pPlayer->MagicWeaponBeginAddEle(m_ele);
}

void UIFormMagicWeaponAddEle::OnShow(void *,void *)
{
	Update();
	for(int i=0 ;i<MAX_ELEMENT_ATTR; ++i)
	{
		m_bar[i]->SetValue(0);
		m_bar[i]->Hide();
	}
}

void UIFormMagicWeaponAddEle::OnClickAddEle(RtwWidget *sender,void *)
{
	if(!m_bValid) return;
	if(!GetWorld()->m_pPlayer) return;
	
    GcActor *actor = GetWorld()->m_pPlayer;
	RtString tmp = sender->GetID().c_str();
	
	SItemID item = GetWorld()->m_pPlayer->mItem.GetEquipItem(CItemContainerBase::WEAPON_1, true);
	if(!ItemID_IsValid(item)) return;
	
	int ele = 0;
	if(tmp == "btnwater")
		ele = ELEMENT_WATER;
	else if(tmp == "btnfire")
		ele = ELEMENT_FIRE;
	else if(tmp == "btnposion")
		ele = ELEMENT_POISON;
	
	int now[MAX_ELEMENT_ATTR];
	MW_GetEleValue(item,now);
	
	int oldNum = MW_GetEleNum(now);
	now[ele]++;
	if(MW_GetEleNum(now) >= 3)
	{
		UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_MW_MULTI3_FAILED), UIFormMsg::TYPE_OK_CANCEL);
		return;
	}

	if(MW_GetEleNum(now) > oldNum && oldNum != 0)
	{
		m_ele = ele;
        UIFormMsg *pFrm = UIFormMsg::ShowStatic(R(MSG_MW_MAKESURE_MULTI), UIFormMsg::TYPE_OK_CANCEL);
		pFrm->EvOK = RTW_CALLBACK_1(this, UIFormMagicWeaponAddEle, OnMultiAddEle,pFrm);
		return;
	}

	if(tmp == "btnwater")
		actor->MagicWeaponBeginAddEle(ELEMENT_WATER);
	else if(tmp == "btnfire")
		actor->MagicWeaponBeginAddEle(ELEMENT_FIRE);
	else if(tmp == "btnposion")
		actor->MagicWeaponBeginAddEle(ELEMENT_POISON);
}

//
// recover pet ui
//
UIFormRecoverPet::UIFormRecoverPet()
{
	m_form = NULL;
	m_bValid = false;
	m_petItem = ItemID_CreateInvalid();

}

UIFormRecoverPet::~UIFormRecoverPet()
{

}

bool UIFormRecoverPet::Init()
{
	m_form = LOAD_UI("fmpet");
	if(m_form)
		m_bValid = true;
	else
		m_bValid = false;

	m_cost	= LOAD_UI("fmpet.panChange.lbmoney");
	m_yes   = LOAD_UI_T(RtwButton,"fmpet.btnok");
	m_no    = LOAD_UI_T(RtwButton,"fmpet.btncancel");
	m_icon  = LOAD_UI_T(RtwButton,"fmpet.aliasCombpet");
	m_ptrHoverImage	= g_workspace.getImageFactory()->createImage("ui/x_textures/highlight_button.tga");
	m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
	m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
	m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
	m_ptrHoverImage->getRenderImage()->SetBlend(true);

    m_yes->EvLClick	   		+= RTW_CALLBACK(this, UIFormRecoverPet, OnYes);
	m_no->EvLClick     		+= RTW_CALLBACK(this, UIFormRecoverPet, OnNo);
	m_form->EvShow     		+= RTW_CALLBACK(this, UIFormRecoverPet, OnShow);
	m_icon->EvDrag     		+= RTW_CALLBACK(this, UIFormRecoverPet, OnDragIn);
	m_icon->EvDragEnd		+= RTW_CALLBACK(this, UIFormRecoverPet, OnDragEndRecoverPet);
	m_icon->EvMouseRDown	+= RTW_CALLBACK(this, UIFormRecoverPet, OnMouseRDown);
	m_icon->EvHint			+= RTW_CALLBACK(this, UIFormRecoverPet, OnGetHintItemText);
	((RtwButton*)m_icon)->SetHoverImage(m_ptrHoverImage);

	m_cost->SetText("");
	m_icon->ModifyFlags(wfDragClient|wfDragOut|wfDragIn|wfHint|wfDClick,0);
	SetVisible(false);
	return m_bValid;
}

bool UIFormRecoverPet::Update()
{


	return true;
}

bool UIFormRecoverPet::Exit()
{
	return true;
}

void UIFormRecoverPet::SetVisible(bool visible)
{
	if(visible)
	{
		//if (g_layerMain->m_fromPlayerItems->IsShowing())	// 道具栏
		//	g_layerMain->m_fromPlayerItems->Hide();
		if (g_layerMain->m_formItemUpdate->IsVisible())		// 道具打造
			g_layerMain->m_formItemUpdate->Hide();
		if (g_layerMain->m_formGem->IsVisible())		// 宝石合成
			g_layerMain->m_formGem->Hide();
		if (g_layerMain->m_formShop->IsVisible())			// NPC商店
			g_layerMain->m_formShop->Hide();
		//if (g_layerMain->m_formPShop->IsVisible())			// 摆摊
		//	g_layerMain->m_formPShop->Hide();
		if (g_layerMain->m_formLookPlayerShop->IsVisible())		// 察看摆摊
			g_layerMain->m_formLookPlayerShop->Hide();
		if (g_layerMain->m_formTrade->IsShowing())			// 交易
			g_layerMain->m_formTrade->Hide();
		if (g_layerMain->m_formBank->IsVisible())			// 仓库
			g_layerMain->m_formBank->Hide();
		if (g_layerMain->m_formItemCompose->IsVisible())	// 配方合成
			g_layerMain->m_formItemCompose->Hide();
		if (g_layerMain->m_formItemFuse->IsVisible())		// 道具融合
			g_layerMain->m_formItemFuse->Hide();
		if (g_layerMain->m_formItemDecompound->IsVisible())
			g_layerMain->m_formItemDecompound->Hide();
		if (g_layerMain->m_formUnionCreate->IsVisible())	// 创建诸侯国
			g_layerMain->m_formUnionCreate->Hide();
		//if (g_layerMain->m_formRecoverPet->GetVisible())
		//	g_layerMain->m_formRecoverPet->SetVisible(false);
		m_form->Show();
	}
	else
		m_form->Hide();
}

bool UIFormRecoverPet::GetVisible()
{
	return m_form->IsVisible();
}

void UIFormRecoverPet::OnYes(void *,void *)
{
	// check pet item id and send it
	if(!ItemID_IsValid(m_petItem)) 
	{
		PlayerFailOperate(R(MSG_PET_PUTPETNEEDRECOVERFIRST));
		return;
	}
	if(GetWorld()->m_pPlayer->mItem.GetMoney() < m_costGold) 
	{
		PlayerFailOperate(R(MSG_PET_NOTENOUGHSILVERCOIN));
		return;
	}

	SetVisible(false);
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_recover_pet);
	cmd->WriteLong(m_petItem.id);
	//----------add start by tony 06.05.18------------------------------//
	//-reason: 没有传NPCID上去，外挂可以不经过NPC直接发送此命令复活宠物
	cmd->WriteLong((GetWorld()->m_dwInteractiveNPCID));
	//----------add end   by tony 06.05.18------------------------------//
	
	NetSend(cmd);
}

void UIFormRecoverPet::OnNo(void *,void *)
{
	SetVisible(false);
}

void UIFormRecoverPet::OnShow(void *,void *)
{
	m_icon->SetBackgroundImage((RtwImage*)NULL);
	m_cost->SetText("");
	m_petItem = ItemID_CreateInvalid();
	m_costGold = 0;
}

void UIFormRecoverPet::OnDragIn(RtwWidget* sender, void*)
{
	guard;

	RtwWidget* server = g_workspace.GetDragServer();
	if ( !server )// 把图标拖到空白处
	{
		/*
		m_ScrollID = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		Refresh();
		*/
	}
	else
	{
		int i,j,page;
		if (g_layerMain->m_fromPlayerItems->Is_Widget_Common(server->GetID(), i, j))
		{
			/*
			m_ScrollID = ItemID_CreateInvalid();
			sender->ModifyFlags(0, wfHint);
			Refresh();
			*/
			page = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
			SItemID item = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(page,i,j);
			if(!ItemIsPet(item.type)) return;
			if(item.params[PET_PARAM_HP] != 0) return;
			
			SItemBase *pWea = GetItemMgr()->GetItem(item.type);
			if(!pWea) return;
			
// change [3/20/2009 tooth.shi]			RtwImage* pImage1 = new RtwImage(pWea->icon);//imageimage
			RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pWea->icon);
			m_icon->SetBackgroundImage(pImage1);
			m_petItem = item;
			m_costGold = PT_GetRecoverGold(item);

			RtString tmp;
			tmp.Format("%d",m_costGold);
			m_cost->SetText((char*)tmp);
		}
		else
		{
			g_workspace.SetDragClient(NULL);
		}
	}

	unguard;
}
bool UIFormRecoverPet::OnBagToRecoverPet(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	int i,j;
	i = e->mouse.x;
	j = e->mouse.y;
	CItemContainer *pCon = &(GetWorld()->m_pPlayer->mItem);
	SItemID item = pCon->m_Bag.GetItem(i, j);

	if(!ItemIsPet(item.type)) return false;
	if(item.params[PET_PARAM_HP] != 0) return false;
	SItemBase *pWea = GetItemMgr()->GetItem(item.type);
	if(!pWea) return false;
	m_icon->ModifyFlags(wfHint, 0);
	m_iPage = g_layerMain->m_fromPlayerItems->OnGetCorrectPage();
	m_iISendItem = i;
	m_iJSendItem = j;
	m_petItem = item;

	RtwImage* pImage1 = g_workspace.getImageFactory()->createImage(pWea->icon);
	m_icon->SetBackgroundImage(pImage1);
	m_costGold = PT_GetRecoverGold(item);

	RtString tmp;
	tmp.Format("%d",m_costGold);
	m_cost->SetText((char*)tmp);
	g_workspace.ResetHintText();
	return true;
	unguard;
}
void UIFormRecoverPet::OnDragEndRecoverPet(RtwWidget* sender, void*)
{
	guard;
	if(!g_workspace.getMouseHover())
	{
		m_petItem = ItemID_CreateInvalid();
		sender->ModifyFlags(0, wfHint);
		sender->SetBackgroundImage((RtwImage*)NULL);
		m_iISendItem = -1;
		m_iJSendItem = -1;
		m_iPage = -1;
	}
	unguard;
}
void UIFormRecoverPet::OnMouseRDown(RtwWidget* sender, void*)
{
	guard;
	m_petItem = ItemID_CreateInvalid();
	sender->ModifyFlags(0, wfHint);
	sender->SetBackgroundImage((RtwImage*)NULL);
	m_iISendItem = -1;
	m_iJSendItem = -1;
	m_iPage = -1;
	unguard;
}
void UIFormRecoverPet::OnGetHintItemText(RtwWidget* sender, void*)
{
	guard;
	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, m_petItem, GetWorld()->m_pPlayer, &(GetWorld()->m_pPlayer->mItem.m_Bag), m_iPage, m_iISendItem, m_iJSendItem);
	g_workspace.SetHintText((char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
	unguard;
}

//
// clear skill
//
UIFormClearSkill::UIFormClearSkill()
{
	m_form = NULL;
	m_bValid = false;

	m_costGold = 0;
	m_spClearLeft = 0;
	m_spClearUsed = 0;
}

UIFormClearSkill::~UIFormClearSkill()
{

}

bool UIFormClearSkill::Init()
{
	m_form = LOAD_UI("fmclearsp");
	if(m_form)
		m_bValid = true;
	else
		m_bValid = false;

	m_cost	= LOAD_UI("fmclearsp.lbmoneynum");
	m_yes   = LOAD_UI_T(RtwButton,"fmclearsp.btnok");
	m_no    = LOAD_UI_T(RtwButton,"fmclearsp.btncancel");

	m_yes->EvLClick	   += RTW_CALLBACK(this, UIFormClearSkill, OnYes);
	m_no->EvLClick     += RTW_CALLBACK(this, UIFormClearSkill, OnNo);
	m_form->EvShow     += RTW_CALLBACK(this, UIFormClearSkill, OnShow);
	
	m_leftCnt	= LOAD_UI("fmclearsp.lbtimenum");
	m_usedCnt	= LOAD_UI("fmclearsp.lbchangednum");

	m_cost->SetText("");
	SetVisible(false);
	return m_bValid;
}

bool UIFormClearSkill::Update()
{
	RtString tmp;
	tmp.Format("%d",m_costGold);
	m_cost->SetText((char*)tmp);

	tmp.Format("%d",m_spClearUsed);
	m_usedCnt->SetText((char*)tmp);

	tmp.Format("%d",m_spClearLeft);
	m_leftCnt->SetText((char*)tmp);

	return true;
}

bool UIFormClearSkill::Exit()
{
	return true;
}

void UIFormClearSkill::SetVisible(bool visible)
{
	if(visible)
		m_form->Show();
	else
		m_form->Hide();
}

bool UIFormClearSkill::GetVisible()
{
	return m_form->IsVisible();
}

void UIFormClearSkill::OnYes(void *,void *)
{
	if(GetWorld()->m_pPlayer->mItem.GetMoney() < m_costGold) return;
	SetVisible(false);

	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_confirm_clear_skill);
	NetSend(cmd);
}

void UIFormClearSkill::OnNo(void *,void *)
{
	SetVisible(false);
}

void UIFormClearSkill::OnShow(void *,void *)
{

}


UIFormRebirth::UIFormRebirth()
{
	guard;
	m_pFrmThis	= LOAD_UI("fmrebirth");

	m_pLabelNotice  = LOAD_UI_T(RtwLabel, "fmrebirth.lbnotice");
	m_pLabelTimer	= LOAD_UI_T(RtwLabel, "fmrebirth.lbtime");
	m_pBtnPerfect	= LOAD_UI_T(RtwButton, "fmrebirth.btperfect");
	m_pBtnOrigin	= LOAD_UI_T(RtwButton, "fmrebirth.btorigin");
	m_pBtnBackCast	= LOAD_UI_T(RtwButton, "fmrebirth.btbackcast");

	//m_gScreenEffect = LOAD_UI("fmscreeneffect");
	//m_gScreenEffect->SetText("");
	//m_gScreenEffect->SetBackgroundColor(0xaf000000);
	//m_gScreenEffect->Hide();

	//RtwButton* pBtnBack = LOAD_UI_T(RtwButton, "fmcard.btnback");

	//m_pTextInput->SetAlphaNumberOnly(true);
	//m_pTextInput->SetCapsLock(1);

	m_pFrmThis->EvClose                             += RTW_CALLBACK(this, UIFormRebirth, OnClose);
	m_pBtnPerfect->EvLClick							+= RTW_CALLBACK(this, UIFormRebirth, OnLClick_Perfect);
	m_pBtnOrigin->EvLClick							+= RTW_CALLBACK(this, UIFormRebirth, OnLClick_Origin);
	m_pBtnBackCast->EvLClick						+= RTW_CALLBACK(this, UIFormRebirth, OnLClick_BackCast);

	m_cType = 3;
	m_pMsgBox = NULL;
	m_pFrmThis->Hide();
	unguard;
}
void UIFormRebirth::SetRebirthTime(int second)
{
	std::string sTime = std::string(rtFormatNumber(second));

	m_pLabelTimer->SetText(sTime);

	m_pLabelTimer->SetTextColor(0xA0FFFF00);

	m_pLabelTimer->Show();
}
void UIFormRebirth::Show(bool bModal)
{
	guard;

	if (!IsVisible())
	{
		if (GetPlayer()->m_dungeonTID)
		{
			m_pBtnPerfect->Disable();
			m_pBtnOrigin->Disable();
			m_pBtnBackCast->SetText("副本复活");
		}
		else
		{
			m_pBtnPerfect->Enable();
			m_pBtnOrigin->Enable();
			m_pBtnBackCast->SetText("回城复活");
		}
	}
	int count;
	GetPlayer()->mItem.m_Bag.FindType(PERFECT_REBIRTH_ITEM, ItemColor_All, &count);
	if (count < 1)
		m_pBtnPerfect->Disable();
	else
		m_pBtnPerfect->Enable();

	m_pFrmThis->SetFocus();
	((RtwForm*)LOAD_UI("fmrebirth"))->SetShowCloseButton(false);
 
 /*	RtwRect rc = g_workspace.getViewportRect();
 	m_gScreenEffect->Move(RtwRect(0, 0, rc.getWidth(), rc.getHeight()));
 	g_workspace.AddTopRenderWidget(m_gScreenEffect);
 	m_gScreenEffect->Show();*/

	if (!RtGetRender()->IsGray()/* && GetWorld()->m_pPlayer->m_bIsDead!=0*/)
	{
		RtGetRender()->Gray(true);
	}

	if (bModal)
		m_pFrmThis->ShowModal();
	else
		m_pFrmThis->Show();

	unguard;
}
void UIFormRebirth::Hide(bool bGray)
{
	guard;
	DROP_RTUI_OBJECT(m_pMsgBox)
	m_pFrmThis->Hide();

 	//m_gScreenEffect->Hide();
	if (bGray)
	{
		RtGetRender()->Gray(false);
	}
	unguard;
}
bool UIFormRebirth::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormRebirth::OnLClick_Perfect(RtwWidget* sender, void*)
{
	int count;
	GetPlayer()->mItem.m_Bag.FindType(PERFECT_REBIRTH_ITEM, ItemColor_All, &count);
	if (count < 1)
	{
		return;
	}

	//char cTmp[128];
	//snprintf(cTmp, 127, R(MSG_CLIENT_REBIRTH_PERFECT), itemName.c_str());

	Hide(false);
	Show(false);
	m_pMsgBox = UIFormMsg::ShowStatic(R(MSG_CLIENT_REBIRTH_PERFECT), UIFormMsg::TYPE_OK_CANCEL,true);
	m_pMsgBox->EvOK			= RTW_CALLBACK_1(this, UIFormRebirth, SendRebirthTypeMsg,m_pMsgBox);
	m_pMsgBox->EvCancel		= RTW_CALLBACK_1(this, UIFormRebirth, OnLClick_MsgClose,m_pMsgBox);
	m_cType = 1;
	//SendRebirthType(1);
}
void UIFormRebirth::OnLClick_Origin(RtwWidget* sender, void*)
{
	Hide(false);
	Show(false);
	static float DIE_BY_MONSTER_ADDED_LOST_EXP=0.0f;
	static CIniVariable Temp(GetGameRule(),"GameExpRule","DieByMonsterAddedLostExp",DIE_BY_MONSTER_ADDED_LOST_EXP);
	ULONG iLostExp = g_TableExp.GetNextLevExp(GetPlayer()->m_core.Lev)*DIE_BY_MONSTER_ADDED_LOST_EXP;
	ULONG iLostMoney = GetPlayer()->mItem.m_Bag.GetMoney() / 10;
	char str[128];
	rt2_sprintf(str,R(MSG_CLIENT_REBIRTH_ORIGIN),(int)iLostMoney,(int)iLostExp);
	m_pMsgBox = UIFormMsg::ShowStatic(str, UIFormMsg::TYPE_OK_CANCEL,true);
	m_pMsgBox->EvOK			= RTW_CALLBACK_1(this, UIFormRebirth, SendRebirthTypeMsg,m_pMsgBox);
	m_pMsgBox->EvCancel		= RTW_CALLBACK_1(this, UIFormRebirth, OnLClick_MsgClose,m_pMsgBox);
	m_cType = 2;
	//SendRebirthType(2);
}
void UIFormRebirth::OnLClick_BackCast(RtwWidget* sender, void*)
{
	SendRebirthType(3);
}
void UIFormRebirth::OnClose(void*, void*)
{
	guard;
	Hide();
	unguard;
}
UIFormRebirth::~UIFormRebirth()
{
	guard;
	DROP_RTUI_OBJECT(m_pMsgBox)
	unguard;
}

void UIFormRebirth::SendRebirthType(char type)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_user_rebirth);
	pPacket->WriteByte(type);
	NetSend(pPacket);
	Hide();
}

void UIFormRebirth::SendRebirthTypeMsg(void*,void*)
{
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_user_rebirth);
	pPacket->WriteByte(m_cType);
	NetSend(pPacket);
	Hide();
}

void UIFormRebirth::OnLClick_MsgClose(void*,void*)
{
	Hide(false);
	Show(true);
}
void UIFormRebirth::OnRebirthFromOther(long objectID,long HPrate,long MPrate)
{
	guard;
	m_lObjectID = objectID;
	m_lHPrate = HPrate;
	m_lMPrate = MPrate;
	GcActor *pActor = FindAllActor(objectID);
	char str[50];
	rt2_snprintf(str,50,R(MSG_CHARACTER_REVIVE),pActor->GetName());
	UIFormMsg* pConfirm=UIFormMsg::ShowStatic(str, UIFormMsg::TYPE_OK_CANCEL,true,"RebirthFromOther");
	pConfirm->EvOK		= RTW_CALLBACK_1(this,UIFormRebirth,OnEnsureRebirthFromOther,pConfirm);
	pConfirm->EvCancel	= RTW_CALLBACK_1(this,UIFormRebirth,OnCancelRebirthFromOther,pConfirm);
	unguard;
}
void UIFormRebirth::OnEnsureRebirthFromOther(void* vpSender,void* vpEvent)
{
	guard;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_rebirth_user);
	pPacket->WriteLong(m_lHPrate);
	pPacket->WriteLong(m_lMPrate);
	NetSend(pPacket);
	unguard;
}
void UIFormRebirth::OnCancelRebirthFromOther(void* vpSender,void* vpEvent)
{
	guard;
	unguard;
}
