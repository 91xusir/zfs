#include "gc_include.h"
#include "ui_form_skill_study.h"

UIFormSkillStudy::UIFormSkillStudy()
{
    guard;

    m_pFrmThis = LOAD_UI("fmlearn");
	m_btnClose = LOAD_UI_T(RtwButton, "fmlearn.btnclose");
	m_btnClose->EvLClick += RTW_CALLBACK(this, UIFormSkillStudy, OnLClickClose);
	m_pStudyBtn[0] = LOAD_UI_T(RtwButton, "fmlearn.fmcontent.fmzoning1.btulearn1");
	m_pStudyBtn[1] = LOAD_UI_T(RtwButton, "fmlearn.fmcontent.fmzoning1.btulearn2");
	m_pStudyBtn[2] = LOAD_UI_T(RtwButton, "fmlearn.fmcontent.fmzoning1.btulearn3");
	m_pStudyBtn[3] = LOAD_UI_T(RtwButton, "fmlearn.fmcontent.fmzoning2.btulearn1");
	m_pStudyBtn[4] = LOAD_UI_T(RtwButton, "fmlearn.fmcontent.fmzoning2.btulearn2");
	m_pStudyBtn[5] = LOAD_UI_T(RtwButton, "fmlearn.fmcontent.fmzoning2.btulearn3");

	for(int i=0;i<6;i++)
	{
		m_pStudyBtn[i]->EvLClick += RTW_CALLBACK(this, UIFormSkillStudy, OnLClickStudy);
	}

    unguard;
}

UIFormSkillStudy::~UIFormSkillStudy()
{
    guard;
    unguard;
}

void UIFormSkillStudy::Show()
{
    guard;
	GetWorld()->HideGuideForm();
	GetWorld()->ShowStudySkill();
    Refresh();
    m_pFrmThis->Show();
    unguard;
}

bool UIFormSkillStudy::IsShowing()
{
    guard;
    if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )    return true;
    else                                                    return false;
    unguard;
}

void UIFormSkillStudy::Hide()
{
    guard;
	if (g_layerMain->m_formGuide3->IsVisible())
	{
		g_layerMain->m_formGuide3->Hide();
	}
    if (m_pFrmThis) m_pFrmThis->Hide();
    unguard;
}

void UIFormSkillStudy::OnLClickClose(RtwWidget* sender, void*)
{
    Hide();
}

void UIFormSkillStudy::Refresh()
{
	guard;

	for(int i=0;i<6;i++)
	{
		m_pStudyBtn[i]->Enable();
	}

	if ( GetPlayer()->m_Skill.HaveCollectSkill() )
	{
		for(int i=0;i<3;i++)
		{
			m_pStudyBtn[i]->Disable();
		}
	}
	if ( GetPlayer()->m_Skill.HaveProtectSkill() )
	{
		for(int i=3;i<6;i++)
		{
			m_pStudyBtn[i]->Disable();
		}
	}

    unguard;
}


void UIFormSkillStudy::OnLClickStudy(RtwWidget* sender, void*)
{
    guard;
	if (g_layerMain->m_formGuide3->IsVisible())
	{
		g_layerMain->m_formGuide3->Hide();
	}
	//if (GetPlayer()->mItem.GetMoney() < 500)
	//{
	//	PlayerFailOperate("½ðÇ®²»×ã");
	//	return;
	//}
	for (int i=0;i<6;i++)
	{
		if (m_pStudyBtn[i] == sender)
		{
			if ((i < 3 && !GetPlayer()->m_Skill.HaveCollectSkill())		||
				(i > 2 && i < 6 && !GetPlayer()->m_Skill.HaveProtectSkill()) )
			{
				if (i<3)
				{
					if (GetPlayer()->mItem.GetMoney() < 50)
					{
						PlayerFailOperate(R(MSG_ITEM_NOMONEY));
						return;
					}
				}
				else
				{
					if (GetPlayer()->mItem.GetMoney() < 500)
					{
						PlayerFailOperate(R(MSG_ITEM_NOMONEY));
						return;
					}
				}
				short skill = ConvertIndexToID(i);
				if (!skill) return;
				CG_CmdPacket* pPacket = NetBeginWrite();
				pPacket->WriteShort(c2r_study_life_skill);
				pPacket->WriteShort(skill);
				SSkill* pSkill = Skill()->FindSkill(skill);
				char lifeSkill[128];
				rt2_sprintf(lifeSkill,R(MSG_SKILL_LEARNNEWSKILL),pSkill->szName);
				PlayerFailOperate(lifeSkill);
				NetSend(pPacket);
			}
		}
	}
    unguard;
}

short UIFormSkillStudy::ConvertIndexToID(int index)
{
	int wID = 0;
	switch (index)
	{
	case 0:
		wID = SKILL_DIG_ORZ_ID;
		break;
	case 1:
		wID = SKILL_DIG_DRUG_ID;
		break;
	case 2:
		wID = SKILL_DIG_SKIN_ID;
		break;
	case 3:
		wID = SKILL_MAKE_WEAPON_ID;
		break;
	case 4:
		wID = SKILL_MAKE_EQUIP_ID;
		break;
	case 5:
		wID = SKILL_MAKE_LEECHDOM_ID;
		break;
	}
	return wID;
}
