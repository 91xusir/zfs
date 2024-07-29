#include "gc_include.h"
#include "ui_form_pet.h"
#include "ui_form_pet_aptitude.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"
#include "ui_form_msg.h"
#include "ui_form_pet_ctrl.h"

UIFromPetAptitude::UIFromPetAptitude(void)
{
	m_pFormMain					= LOAD_UI_T(RtwForm, "fmconjurezz");
	m_pFormView					= LOAD_UI_T(RtwForm, "fmconjurezz");

	m_pLabelGrowthHp			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lblifegrowthcount");
	m_pLabelGrowthMp			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lbmagicgrowthcount");
	m_pLabelGrowthAtt			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lbpydamgrowthcount");
	m_pLabelGrowthDef			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lbpydefgrowthcount");
	m_pLabelGrowthHit			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lbhitgrowthcount");
	m_pLabelGrowthDodge			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lbdodgegrowthcount");
	m_pLabelFuseName			= LOAD_UI_T(RtwLabel, "fmconjurezz.conjureform01.lbcomplevtext");

	for (int i = 0;i<MAX_ELEMENT_ATTR;i++)
	{
		char str[128];
		rt2_snprintf(str,128,"fmconjurezz.fmbackimage.lbatt%d",i+1);
		m_pLabelElementAtt[i]	= LOAD_UI_T(RtwLabel, str);
		rt2_snprintf(str,128,"fmconjurezz.fmbackimage.lbdef%d",i+1);
		m_pLabelElementDef[i]	= LOAD_UI_T(RtwLabel, str);
	}

	char str[128];
	for (int i = 0;i<UIPET_SKILL_NUM;i++)
	{
		if (i < UIPET_SAMELEV_IMAGE_NUM)
		{
			rt2_sprintf(str,"fmconjurezz.conjureform01.lbstrlev0%d",i+1);
			m_pLabelAbilityLev[i]		= LOAD_UI_T(RtwLabel, str);
			m_pLabelAbilityLev[i]->ModifyFlags(wfGrabMouse|wfMouseMove|wfHint, 0);
			m_pLabelAbilityLev[i]->EvHint += RTW_CALLBACK(this, UIFromPetAptitude, GetLevHint);
			m_pLabelAbilityLev[i]->SetParam1(&m_CurPetItem.params[PET_PARAM_ABILITY_LEV]);

			rt2_sprintf(str,"fmconjurezz.conjureform01.lbcomplev0%d",i+1);
			m_pLabelFuseLev[i]			= LOAD_UI_T(RtwLabel, str);
			m_pLabelFuseLev[i]->ModifyFlags(wfGrabMouse|wfMouseMove|wfHint, 0);
			m_pLabelFuseLev[i]->EvHint += RTW_CALLBACK(this, UIFromPetAptitude, GetLevHint);
			m_pLabelFuseLev[i]->SetParam1(&m_CurPetItem.params[PET_PARAM_FUSE_LEV]);

			//sprintf(str,"fmconjurezz.conjureform04.lbsoullev0%d",i+1);
			//m_pLabelLinkupLev[i]		= LOAD_UI_T(RtwLabel, str);

			rt2_sprintf(str,"fmconjurezz.conjureform04.soulkey.lbhotkey%d",i+1);
			m_pBtnLinkupBeads[i]		= LOAD_UI_T(RtwButton, str);
			m_pBtnLinkupBeads[i]->EvDragIn		+= RTW_CALLBACK(this, UIFromPetAptitude, DragInBeads);
			m_pBtnLinkupBeads[i]->ModifyFlags(wfHint, 0);
			m_pBtnLinkupBeads[i]->EvHint += RTW_CALLBACK(this, UIFromPetAptitude, GetBeadsGrade);
			m_pBtnLinkupBeads[i]->SetParam1(&m_CurPetItem.params[PET_PARAM_BEADS_1+i]);
		}
		rt2_sprintf(str,"fmconjurezz.conjureform05.passive.lbhotkey%d",i+1);
		m_pBtnPassive[i]				= LOAD_UI_T(RtwButton, str);
		m_pBtnPassive[i]->EvHint		+= RTW_CALLBACK(this, UIFromPetAptitude, GetPetSkillHint);
		m_pBtnPassive[i]->ModifyFlags(wfHint, 0);
		m_pBtnPassive[i]->SetParam1(&m_CurPetItem.params[PET_PARAM_PASSIVE_1+i]);
		rt2_sprintf(str,"fmconjurezz.conjureform06.activekey.lbhotkey%d",i+1);
		m_pBtnSkill[i]					= LOAD_UI_T(RtwButton, str);
		m_pBtnSkill[i]->EvHint			+= RTW_CALLBACK(this, UIFromPetAptitude, GetPetSkillHint);
		m_pBtnSkill[i]->EvRClick		+= RTW_CALLBACK(this, UIFromPetCtrl, DoUseSkill);
		m_pBtnSkill[i]->ModifyFlags(wfHint, 0);
		m_pBtnSkill[i]->SetParam1(&m_CurPetItem.params[PET_PARAM_SKILL_1+i]);
	}
	m_pFormMain->Hide();
}

UIFromPetAptitude::~UIFromPetAptitude(void)
{
}

void UIFromPetAptitude::DragOutSkill(void *,void *)
{

}

void UIFromPetAptitude::DragInBeads(void *,void *)
{

}
void UIFromPetAptitude::Refresh()
{

}

void UIFromPetAptitude::GetBeadsGrade(RtwWidget *sender,void *)
{
	int *id = (int*)sender->getParam1();
	if (id && ItemIsPet(m_CurPetItem) && *id)
	{
		char tmp[64];
		rt2_snprintf(tmp,64,"品级 %.2f",(float)(*id) / PET_PARAM_COMPUTE_BEADS);

		g_workspace.SetHintText(tmp, true);
		//g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}
	else
	{
		g_workspace.SetHintText("");
		g_workspace.AutoHintSize();
	}
}
char *UIFromPetAptitude::GetPetBeadsImage(DWORD dwGrade)
{
	if (dwGrade <= 150)
	{
		return PET_BEADS_IMAGE_1;
	}
	else if (dwGrade <= 200)
	{
		return PET_BEADS_IMAGE_2;
	}
	else if (dwGrade <= 250)
	{
		return PET_BEADS_IMAGE_3;
	}
	else if (dwGrade <= 300)
	{
		return PET_BEADS_IMAGE_4;
	}
	else
	{
		return PET_BEADS_IMAGE_4;
	}
}

void UIFromPetAptitude::GetLevHint(RtwWidget *sender,void *)
{
	int *id = (int*)sender->getParam1();
	if (id && ItemIsPet(m_CurPetItem))
	{
		//string MakeHint(string vString,string vShortcutChar);
		char tmp[64];
		rt2_snprintf(tmp,64,"%d级",*id);
		g_workspace.SetHintText(tmp, true);
		//g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
		g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
	}
	else
	{
		g_workspace.SetHintText("");
		g_workspace.AutoHintSize();
	}
}

void UIFromPetAptitude::GetPetSkillHint(RtwWidget *sender,void *)
{
	int *id = (int*)sender->getParam1();
	if (id)
	{
		int iLine=0;
		unsigned short wSkillID = *id;
		SSkill* pSkill = Skill()->FindSkill(wSkillID);
		const char* pTxt;
		std::string spTxt;
		if (pSkill)
		{
			pTxt = NULL;
			bool bStudied = true;
			int iState = 0;

			//g_workspace.ResetHintText();
			pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, true, false, iLine, NULL);
			if (iLine)
			{
				g_workspace.SetHintSize(180, 100);
				g_workspace.SetHintText(pTxt, true);
				//g_workspace.AutoHintSize();
				g_workspace.AutoHintPosition();
			}else
			{
				g_workspace.SetHintText("");
				g_workspace.AutoHintSize();
			}
			//sender->SetHint(g_workspace.getDefaultHint());
		}
		else
		{
			g_workspace.SetHintText("");
			g_workspace.AutoHintSize();
		}
	}
	else
	{
		g_workspace.SetHintText("");
		g_workspace.AutoHintSize();
	}
}

void UIFromPetAptitude::Reset()
{
	m_pLabelGrowthHp->SetText("");
	m_pLabelGrowthMp->SetText("");
	m_pLabelGrowthAtt->SetText("");
	m_pLabelGrowthDef->SetText("");
	m_pLabelGrowthHit->SetText("");
	m_pLabelGrowthDodge->SetText("");
	m_pLabelFuseName->SetText("");

	for (int i = 0;i<UIPET_SKILL_NUM;i++)
	{
		if (i < MAX_ELEMENT_ATTR)
		{
			m_pLabelElementAtt[i]->SetText("");
			m_pLabelElementDef[i]->SetText("");
		}
		if (i < UIPET_SAMELEV_IMAGE_NUM)
		{
			m_pLabelAbilityLev[i]->SetBackgroundImage((RtwImage*)NULL);
			m_pLabelFuseLev[i]->SetBackgroundImage((RtwImage*)NULL);
			//m_pLabelLinkupLev[i]->SetBackgroundImage((RtwImage*)NULL);
			m_pBtnLinkupBeads[i]->SetBackgroundImage((RtwImage*)NULL);
		}
		m_pBtnPassive[i]->SetBackgroundImage((RtwImage*)NULL);
		m_pBtnSkill[i]->SetBackgroundImage((RtwImage*)NULL);
	}
	ItemID_SetInvalid(m_CurPetItem);
}
void UIFromPetAptitude::Show(SItemID itemPet)
{
	m_CurPetItem = itemPet;
	if (ItemIsPet(m_CurPetItem))
		OnPreDraw();
}

void UIFromPetAptitude::Hide()
{
	m_pFormMain->Hide();
	Reset();
}

bool UIFromPetAptitude::IsVisible()
{
	return m_pFormMain->IsVisible();
}

void UIFromPetAptitude::OnPreDraw()
{
	if (!ItemIsPet(m_CurPetItem)) return;

	SItemPet *pItemPet = (SItemPet*)GetWorld()->m_pItemManager->GetItem(m_CurPetItem.type);
	if (!pItemPet) return;

	SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
	if (!pPet) return;

	float FHp = 0.f;
	float FMp = 0.f;
	float FAttack = 0.f;
	float FDamage = 0.f;
	float FDodge = 0.f;
	float FArmor = 0.f;
	float ElementArmorRate[3];
	float ElementAttackRate[3];

	//FHp		= (float)m_CurPetItem.params[PET_PARAM_GROWTH_HP] / PET_PARAM_COMPUTE_GROWTH;
	//FMp		= (float)m_CurPetItem.params[PET_PARAM_GROWTH_MP] / PET_PARAM_COMPUTE_GROWTH;
	//FAttack	= (float)m_CurPetItem.params[PET_PARAM_GROWTH_HIT] / PET_PARAM_COMPUTE_GROWTH;
	//FDamage	= (float)m_CurPetItem.params[PET_PARAM_GROWTH_ATT] / PET_PARAM_COMPUTE_GROWTH;
	//FDodge	= (float)m_CurPetItem.params[PET_PARAM_GROWTH_DODGE] / PET_PARAM_COMPUTE_GROWTH;
	//FArmor	= (float)m_CurPetItem.params[PET_PARAM_GROWTH_DEF] / PET_PARAM_COMPUTE_GROWTH;

	//ElementArmorRate[ELEMENT_WATER] = m_CurPetItem.params[PET_PARAM_GROWTH_WATERDEF] / PET_PARAM_COMPUTE_GROWTH;
	//ElementArmorRate[ELEMENT_FIRE] = m_CurPetItem.params[PET_PARAM_GROWTH_FIREDEF] / PET_PARAM_COMPUTE_GROWTH;
	//ElementArmorRate[ELEMENT_POISON] = m_CurPetItem.params[PET_PARAM_GROWTH_POSIONDEF] / PET_PARAM_COMPUTE_GROWTH;

	//ElementAttackRate[ELEMENT_WATER] = m_CurPetItem.params[PET_PARAM_GROWTH_WATERDA] / PET_PARAM_COMPUTE_GROWTH;
	//ElementAttackRate[ELEMENT_FIRE] = m_CurPetItem.params[PET_PARAM_GROWTH_FIREDA] / PET_PARAM_COMPUTE_GROWTH;
	//ElementAttackRate[ELEMENT_POISON] = m_CurPetItem.params[PET_PARAM_GROWTH_WATERDA] / PET_PARAM_COMPUTE_GROWTH;

	if (!GetPetInfoFromItem(m_CurPetItem,FHp,FMp,FAttack,FDamage,FDodge,FArmor,ElementArmorRate,ElementAttackRate))
		return;

	m_pFormMain->Show();

	char str[128];
	rt2_sprintf(str,"%d",int(FHp * 1000.f * 6.f));
	m_pLabelGrowthHp->SetText(str);
	rt2_sprintf(str,"%d",int(FMp * 1000.f));
	m_pLabelGrowthMp->SetText(str);
	rt2_sprintf(str,"%d",int(FDamage * 1000.f));
	m_pLabelGrowthAtt->SetText(str);
	rt2_sprintf(str,"%d",int(FArmor * 1000.f));
	m_pLabelGrowthDef->SetText(str);
	rt2_sprintf(str,"%d",int(FAttack * 1000.f));
	m_pLabelGrowthHit->SetText(str);
	rt2_sprintf(str,"%d",int(FDodge * 1000.f));
	m_pLabelGrowthDodge->SetText(str);
	for (int i=0;i<MAX_ELEMENT_ATTR;i++)
	{
		rt2_sprintf(str,"%d",int(ElementAttackRate[i] * 250.f));
		m_pLabelElementAtt[i]->SetText(str);
		rt2_sprintf(str,"%d",int(ElementArmorRate[i] * 250.f));
		m_pLabelElementDef[i]->SetText(str);
	}

	if (ItemIsPetNormal(m_CurPetItem))
	{
		m_pLabelFuseName->SetText("融合等级");
	}
	else if (ItemIsPetFairy(m_CurPetItem))
	{
		m_pLabelFuseName->SetText("进化等级");
	}
	else if (ItemIsPetSpectre(m_CurPetItem))
	{
		m_pLabelFuseName->SetText("炼阴等级");
	}

	for (int i = 0;i<UIPET_SKILL_NUM;i++)
	{
		if (i < UIPET_SAMELEV_IMAGE_NUM)
		{
			// 锻体等级
			if (i == 0 && m_CurPetItem.params[PET_PARAM_ABILITY_LEV] && m_CurPetItem.params[PET_PARAM_ABILITY_LEV] < 3)
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_SOMELEV_IMAGE_1);
				if (pImage)
					m_pLabelAbilityLev[i]->SetBackgroundImage(pImage);
			}
			else if(m_CurPetItem.params[PET_PARAM_ABILITY_LEV] >= 3 && (i * 2 + 3) <= m_CurPetItem.params[PET_PARAM_ABILITY_LEV])
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_SOMELEV_IMAGE_2);
				if (pImage)
					m_pLabelAbilityLev[i]->SetBackgroundImage(pImage);
			}
			else
			{
				m_pLabelAbilityLev[i]->SetBackgroundImage((RtwImage*)NULL);
			}

			if (ItemIsPetSpectre(m_CurPetItem))
			{
				if ((m_CurPetItem.params[PET_PARAM_LINKUP_LEV] + 4) / ((i+1) * 5) > 0)
				{
					RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_SOMELEV_IMAGE_2);
					if (pImage)
						m_pLabelFuseLev[i]->SetBackgroundImage(pImage);
				}
				else
				{
					m_pLabelFuseLev[i]->SetBackgroundImage((RtwImage*)NULL);
				}
			}
			else
			{
				if (i < m_CurPetItem.params[PET_PARAM_FUSE_LEV])
				{
					RtwImage *pImage = g_workspace.getImageFactory()->createImage(PET_SOMELEV_IMAGE_2);
					if (pImage)
						m_pLabelFuseLev[i]->SetBackgroundImage(pImage);
				}
				else
				{
					m_pLabelFuseLev[i]->SetBackgroundImage((RtwImage*)NULL);
				}
			}

			if (m_CurPetItem.params[PET_PARAM_BEADS_1+i])
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(GetPetBeadsImage(m_CurPetItem.params[PET_PARAM_BEADS_1+i]));
				if (pImage)
					m_pBtnLinkupBeads[i]->SetBackgroundImage(pImage);
			}
			else
			{
				m_pBtnLinkupBeads[i]->SetBackgroundImage((RtwImage*)NULL);
			}
		}
		if (m_CurPetItem.params[PET_PARAM_PASSIVE_1+i])
		{
			SSkill *pSkill = Skill()->FindSkill(m_CurPetItem.params[PET_PARAM_PASSIVE_1+i]);
			if (pSkill)
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(pSkill->szIcon);
				if (pImage)
					m_pBtnPassive[i]->SetBackgroundImage(pImage);
			}
			else
			{
				m_pBtnPassive[i]->SetBackgroundImage((RtwImage*)NULL);
			}
		}
		else
		{
			m_pBtnPassive[i]->SetBackgroundImage((RtwImage*)NULL);
		}
		if (m_CurPetItem.params[PET_PARAM_SKILL_1+i])
		{
			SSkill *pSkill = Skill()->FindSkill(m_CurPetItem.params[PET_PARAM_SKILL_1+i]);
			if (pSkill)
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(pSkill->szIcon);
				if (pImage)
					m_pBtnSkill[i]->SetBackgroundImage(pImage);
			}
			else
			{
				m_pBtnSkill[i]->SetBackgroundImage((RtwImage*)NULL);
			}
		}
		else
		{
			m_pBtnSkill[i]->SetBackgroundImage((RtwImage*)NULL);
		}
	}
}