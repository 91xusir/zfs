
#ifndef __UI_FORM_PET_APITITUDE_H__
#define __UI_FORM_PET_APITITUDE_H__

#include "ui/rtw_ui.h"

class UIFromPetAptitude
{
public:
	UIFromPetAptitude(void);
	~UIFromPetAptitude(void);

	void Refresh();
	void Show(SItemID itemPet);
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();

	void GetLevHint(RtwWidget *sender,void *);
	void GetPetSkillHint(RtwWidget *sender,void *);
	void GetBeadsGrade(RtwWidget *sender,void *);

	char *GetPetBeadsImage(DWORD dwGrade);

	SItemID m_CurPetItem;

private:
	void DragOutSkill(void *,void *);
	void DragInBeads(void *,void *);

private:
	RtwForm			*m_pFormMain;
	RtwForm			*m_pFormView;

	RtwLabel		*m_pLabelGrowthHp;
	RtwLabel		*m_pLabelGrowthMp;
	RtwLabel		*m_pLabelGrowthAtt;
	RtwLabel		*m_pLabelGrowthDef;
	RtwLabel		*m_pLabelGrowthHit;
	RtwLabel		*m_pLabelGrowthDodge;

	RtwLabel		*m_pLabelElementAtt[MAX_ELEMENT_ATTR];
	RtwLabel		*m_pLabelElementDef[MAX_ELEMENT_ATTR];

	RtwLabel		*m_pLabelFuseName;		// 融合等级,进化等级名字

	RtwLabel		*m_pLabelAbilityLev[UIPET_SAMELEV_IMAGE_NUM];
	RtwLabel		*m_pLabelFuseLev[UIPET_SAMELEV_IMAGE_NUM];
	RtwLabel		*m_pLabelLinkupLev[UIPET_SAMELEV_IMAGE_NUM];

	RtwButton		*m_pBtnLinkupBeads[UIPET_LINKUP_BEADS_NUM];
	RtwButton		*m_pBtnPassive[UIPET_SKILL_NUM];
	RtwButton		*m_pBtnSkill[UIPET_SKILL_NUM];
};

#endif