#ifndef UI_FORM_PET_ABILITY_H_
#define UI_FORM_PET_ABILITY_H_

#include "ui/rtw_ui.h"

class UIFormPetAbility
{
public:
	UIFormPetAbility();
	~UIFormPetAbility();

	void Refresh();
	void Show();
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();

	void OnHide(RtwWidget *sender,void *);

	void OnFinishAbility(bool succeed);

	void SetItem1(SItemID item1);
	void SetItem2(SItemID item2);
	void DragOutItem1(RtwButton *sender,void *);
	void DragOutItem2(RtwButton *sender,void *);
	void OnGetHintItemText(RtwButton *sender,void *);

	RtwWidget* OnGetThisForm(){return m_pFmWindow;}

	RtwButton	*m_pBtnItem1;
	RtwButton	*m_pBtnItem2;

private:
	SItemID m_pet;
	SItemID m_item1;
	SItemID m_item2;
	list<SItemID> m_listPet;
	DWORD *m_pdwSelectPet;

	void OnSelectPet(RtwComboBox *sender,void *);
	void OnClickAbility(RtwButton *sender,void *);
	void SetAbilityButton();

private:
	RtwForm		*m_pFmWindow;
	Rtw3DView	*m_pFmPetView;
	RtwComboBox *m_pCbPetList;
	RtwButton	*m_pBtnAbility;
	RtwLabel	*m_pLbRate;
	RtwLabel	*m_pLbLev;
};

#endif