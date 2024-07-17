#include "gc_include.h"
#include "ui_form_pet.h"
#include "ui_form_pet_ctrl.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"
#include "gc_pet.h"
#include "ui_form_msg.h"
#include "ui_form_pet_aptitude.h"
#include "game_assistant.h"

UIFromPetCtrl::UIFromPetCtrl(void)
{
	m_pFormPetSkill	= RTW_WIDGET_T(RtwForm, "petskill");
	m_pFormPetCtrl	= RTW_WIDGET_T(RtwForm, "petcontrol");
	m_pBtnDefSkill	= RTW_WIDGET_T(RtwButton, "petskill.lbhotkey0");
	m_pBtnDefSkill->EvLClick		+= RTW_CALLBACK(this, UIFromPetCtrl, SetDefSkill);
	m_pBtnDefSkill->EvHint			+= RTW_CALLBACK(this, UIFromPetAptitude, GetPetSkillHint);
	m_pBtnDefSkill->ModifyFlags(wfHint, 0);
	char str[128];
	for (int i = 0;i<UIPET_SKILL_NUM;i++)
	{
		rt2_sprintf(str,"petskill.lbhotkey%d",i+1);
		m_pBtnSkill[i]					= RTW_WIDGET_T(RtwButton, str);
		m_pBtnSkill[i]->EvRClick		+= RTW_CALLBACK(this, UIFromPetCtrl, DoUseSkill);
		m_pBtnSkill[i]->EvLClick		+= RTW_CALLBACK(this, UIFromPetCtrl, SetDefSkill);
		m_pBtnSkill[i]->EvHint			+= RTW_CALLBACK(this, UIFromPetAptitude, GetPetSkillHint);
		m_pBtnSkill[i]->ModifyFlags(wfHint, 0);
		m_pBtnSkill[i]->SetParam1(&m_CurPetItem.params[PET_PARAM_SKILL_1+i]);
	}
	m_pBtnCtrl[0]		= RTW_WIDGET_T(CUiCheckButton, "petcontrol.withattackblt");

	m_pBtnCtrl[1]		= RTW_WIDGET_T(CUiCheckButton, "petcontrol.defattackblt");
	m_pBtnCtrl[2]		= RTW_WIDGET_T(CUiCheckButton, "petcontrol.pasattackblt");
	m_pBtnCtrl[3]		= RTW_WIDGET_T(CUiCheckButton, "petcontrol.attackblt");
	m_pBtnCallBack		= RTW_WIDGET_T(RtwButton, "petcontrol.callcancel");
	for (int i=0;i<UIPET_CTRL_BUTTON_NUM;i++)
	{
		m_pBtnCtrl[i]->EvLClick		+= RTW_CALLBACK(this, UIFromPetCtrl, OnChangeAttackType);
	}
	m_pBtnCallBack->EvLClick		+= RTW_CALLBACK(this, UIFromPetCtrl, OnChangeAttackType);
	m_pBtnCtrl[0]->SetParam1((void*)&PET_CTRL_SYNERGY);
	m_pBtnCtrl[1]->SetParam1((void*)&PET_CTRL_DEF);
	m_pBtnCtrl[2]->SetParam1((void*)&PET_CTRL_PASSIVE);
	m_pBtnCtrl[3]->SetParam1((void*)&PET_CTRL_ATTACK);
	m_pBtnCallBack->SetParam1((void*)&PET_CTRL_CALLBACK);
	Hide();
}

UIFromPetCtrl::~UIFromPetCtrl(void)
{

}
void UIFromPetCtrl::OnDragInSkill(RtwButton *sender,void *)
{

}
void UIFromPetCtrl::DoUseSkill(RtwButton *sender,void *)
{
	DWORD *dwSkill = (DWORD*)sender->getParam1();
	if (dwSkill)
	{
		if (*dwSkill == PET_SKILL_RIDE)
		{
			if (GetPlayerPet()->GetPetActor() && GetPlayerPet()->GetPetActor()->m_core.Lev < 35)
			{
				PlayerFailOperate(R(MSG_CANNOT_RIDEPET_LEV));
				return;
			}
			if (GetPlayer()->m_cShapeshiftNet != 0)
			{
				PlayerFailOperate(R(MSG_CANNOT_RIDEPET_SHAPESHIFT));
				return;
			}
		}
		//{
		//	if (m_Skill.UseSkill(pSkill->wID)) bCanUseSkill = true;

		//	if (m_Skill.UseSkill(pSkill->wID, pActor->ID())) bCanUseSkill = true;

		//	if (m_Skill.UseSkill(pSkill->wID, pCmd->f)) bCanUseSkill = true;

		//}
		//GetPlayerPet()->GetPetActor()->m_Skill.UseSkill(*dwSkill, GetPlayer()->OnGetTarget());
		SendCtrlToServer(PET_CTRL_USE_SKILL,*dwSkill,GetPlayer()->OnGetTarget());
	}
}
void UIFromPetCtrl::OnChangeAttackType(RtwButton *sender,void *)
{
	unsigned char *selectType = (unsigned char*)sender->getParam1();
	if (selectType)
	{
		DWORD dwTarget = 0;
		if (*selectType == PET_CTRL_ATTACK && GetPlayer()->OnGetTarget())
		{
			GcActor* pActor = FindActor(GetPlayer()->OnGetTarget());
			if (GetWorld()->CanAttack(pActor))
			{
				dwTarget = GetPlayer()->OnGetTarget();
			}
		}
		SendCtrlToServer(*selectType,dwTarget);
		if (*selectType != PET_CTRL_CALLBACK)
		{
			for (int i=0;i<UIPET_CTRL_BUTTON_NUM;i++)
			{
				if (m_pBtnCtrl[i] == sender)
				{
					m_pBtnCtrl[i]->SetChecked(true);
				}
				else
				{
					m_pBtnCtrl[i]->SetChecked(false);
				}
			}
		}
	}
}

void UIFromPetCtrl::SetDefSkillUI(DWORD skillid)
{
	SSkill *pSkill = Skill()->FindSkill(skillid);
	if (pSkill)
	{
		RtwImage *pImage = g_workspace.getImageFactory()->createImage(pSkill->szIcon);
		if (pImage)
			m_pBtnDefSkill->SetBackgroundImage(pImage);
		m_pBtnDefSkill->SetParam1(&pSkill->wID);
	}
	else
	{
		m_pBtnDefSkill->SetBackgroundImage((RtwImage*)NULL);
	}
}

void UIFromPetCtrl::SetDefSkill(RtwButton *sender,void *)
{
	DWORD *dwSkill = (DWORD*)sender->getParam1();
	if (dwSkill && *dwSkill)
	{
		if ( *dwSkill == PET_SKILL_DROPRIDE || *dwSkill == PET_SKILL_RIDE || *dwSkill == PET_SKILL_LINKUP )
		{
			PlayerFailOperate(R(MSG_CANNOT_SET_PET_AOTOSKILL));
			return;
		}
		SendCtrlToServer(PET_CTRL_SET_DEFSKILL,*dwSkill);
	}
}

void UIFromPetCtrl::OnClickCommand(RtwButton *sender,void *)
{

}

void UIFromPetCtrl::Refresh()
{

}
void UIFromPetCtrl::Show(SItemID pet)
{
	m_CurPetItem = pet;
	m_pFormPetSkill->Show();
	m_pFormPetCtrl->Show();
	OnPreDraw();
	m_pBtnCtrl[0]->SetChecked(true);
}
void UIFromPetCtrl::Hide()
{
	m_pFormPetSkill->Hide();
	m_pFormPetCtrl->Hide();
	if (GetPlayerPet() && GetPlayerPet()->GetPetItem().id)
		GetPlayerPet()->CallBackPet();

	for (int i=0;i<UIPET_CTRL_BUTTON_NUM;i++)
	{
		m_pBtnCtrl[i]->SetChecked(false);
	}
	m_pBtnDefSkill->SetParam1(NULL);
	m_pBtnDefSkill->SetBackgroundImage((RtwImage*)NULL);
}

void UIFromPetCtrl::Reset()
{
	for (int i=0;i<UIPET_CTRL_BUTTON_NUM;i++)
	{
		m_pBtnCtrl[i]->SetChecked(false);
	}
	m_pBtnDefSkill->SetParam1(NULL);
	m_pBtnDefSkill->SetBackgroundImage((RtwImage*)NULL);
}

void UIFromPetCtrl::OnPreDraw()
{
	int count = 0;
	if ( !GetWorld()->m_pPet )
		return;
	SItemID petItem = GetWorld()->m_pGcPet->GetPetItem();
	if ( !ItemIsPet(petItem) )
		return;
	//for (int i = PET_PARAM_SKILL_1;i<=PET_PARAM_SKILL_6;i++)
	//{
	//	SSkill *pSkill = Skill()->FindSkill(petItem.params[i]);
	//	if (pSkill)
	//	{
	//		RtwImage *pImage = g_workspace.getImageFactory()->createImage(pSkill->szIcon);
	//		if (pImage)
	//			m_pBtnSkill[count]->SetBackgroundImage(pImage);
	//		m_pBtnSkill[count]->SetParam1(&pSkill->wID);
	//	}
	//	else
	//	{
	//		m_pBtnSkill[count]->SetBackgroundImage((RtwImage*)NULL);
	//	}
	//	count++;
	//}
}

bool UIFromPetCtrl::IsVisible()
{
	return m_pFormPetSkill->IsVisible() || m_pFormPetCtrl->IsVisible();
}