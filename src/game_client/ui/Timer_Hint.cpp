#include "gc_include.h"
#include "Timer_Hint.h"
#include "ui_form_skill.h"
#include "ui_form_playeritems.h"

CTimer_Hint::CTimer_Hint(void)
{
	guard;
	GetTimer()->AddTimer(this,0.5);
	m_eNowHintFor = WIDGET_NULL;
	m_pActorSkill = NULL;
	unguard;
}

CTimer_Hint::~CTimer_Hint(void)
{
	guard;
	GetTimer()->RemoveObject(this);//将时间关联取消
	unguard;
}

void CTimer_Hint::OnTimerEvent(DWORD vID)
{
	guard;
	if(GetWorld()->GetHintWidget()->IsVisible() && m_eNowHintFor)
	{
		switch(m_eNowHintFor)
		{
		case WIDGET_SKILL_TREE:
			g_layerMain->m_fromSkill->OnGetTreeItemSkillHint(m_wdtHintFor,NULL);
			break;
		case WIDGET_SKILL_BUFF:
			if(m_pActorSkill)
				m_pActorSkill->OnGetEffectHint(m_wdtHintFor,NULL);
			break;
		case WIDGET_SKILL_SHORTCUT:
			g_layerMain->m_fromSkill->OnGetHintTextCommonButton(m_wdtHintFor,NULL);
			break;
		case WIDGET_ITEAM_SHOW:
			g_layerMain->m_fromPlayerItems->OnGetHintText_CommonItem(m_wdtHintFor,NULL);
			break;
		}
	}else
	{
		m_wdtHintFor = NULL;
		m_eNowHintFor = WIDGET_NULL;
		m_pActorSkill = NULL;
	}
	unguard;
}