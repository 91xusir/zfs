
#ifndef __UI_FORM_PET_TOBEADS_H_
#define __UI_FORM_PET_TOBEADS_H_

#include "ui/rtw_ui.h"

class UIFormPetToBeads
{
public:
	UIFormPetToBeads();
	~UIFormPetToBeads();

	void Refresh();
	void Show();
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();

	RtwWidget* OnGetThisForm(){return m_pFmWindow;}

	void OnHide(RtwWidget *sender,void *);

	void OnFinishToBeads(bool succeed);

	void SetItem1(SItemID item1);
	void SetItem2(SItemID item2);
	void DragOutItem1(RtwButton *sender,void *);
	void DragOutItem2(RtwButton *sender,void *);

	RtwButton	*m_pBtnItem1;
	RtwButton	*m_pBtnItem2;

private:
	SItemID m_pet;
	SItemID m_item1;
	SItemID m_item2;
	list<SItemID> m_listPet;
	DWORD *m_pdwSelectPet;

	void OnSelectPet(RtwComboBox *sender,void *);
	void OnClickToBeads(RtwButton *sender,void *);
	void SetToBeadsButton();

private:
	RtwForm		*m_pFmWindow;
	Rtw3DView		*m_pFmPetView;
	RtwComboBox *m_pCbPetList;
	RtwButton	*m_pBtnToBeads;
	RtwLabel	*m_pLbRate;
	RtwLabel	*m_pLbGrade;
};

#endif