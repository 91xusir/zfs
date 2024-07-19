
#ifndef __UI_FORM_PET_CTRL_H__
#define __UI_FORM_PET_CTRL_H__

#include "ui/rtw_ui.h"

class UIFromPetCtrl
{
public:
	UIFromPetCtrl(void);
	~UIFromPetCtrl(void);

	void Refresh();
	void Show(SItemID pet);
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();

	void OnClickCommand(RtwButton *sender,void *);

	void OnChangeAttackType(RtwButton *sender,void *);

	void OnDragInSkill(RtwButton *sender,void *);

	void DoUseSkill(RtwButton *sender,void *);

	void SetDefSkill(RtwButton *sender,void *);

	void SetDefSkillUI(DWORD skillid);

	unsigned char m_cAttackType;

	RtwButton		*m_pBtnDefSkill;
	RtwButton		*m_pBtnSkill[UIPET_SKILL_NUM];

	SItemID	m_CurPetItem;


private:
	RtwForm			*m_pFormPetSkill;
	RtwForm			*m_pFormPetCtrl;

	CUiCheckButton	*m_pBtnCtrl[UIPET_CTRL_BUTTON_NUM];
	RtwButton		*m_pBtnCallBack;
};

#endif