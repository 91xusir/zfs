
#ifndef __UI_FORM_PET_MERGEBEADS_H_
#define __UI_FORM_PET_MERGEBEADS_H_

#include "ui/rtw_ui.h"

class UIFormPetMergeBeads
{
public:
	UIFormPetMergeBeads();
	~UIFormPetMergeBeads();

	void Refresh();
	void Show();
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();

	RtwWidget* OnGetThisForm(){return m_pFmWindow;}

	void OnHide(RtwWidget *sender,void *);

	void OnFinishMergeBeads(bool succeed);

	void DragOutItem(RtwButton *sender,void *);
	void SetItem(SItemID item);

	RtwButton	*m_pBtnItem;

private:
	SItemID m_pet;
	SItemID m_item;
	list<SItemID> m_listPet;
	DWORD *m_pdwSelectPet;

	void OnSelectPet(RtwComboBox *sender,void *);
	void OnClickMergeBeads(RtwButton *sender,void *);
	void SetMergeBeadsButton();

private:
	RtwForm		*m_pFmWindow;
	Rtw3DView		*m_pFmPetView;
	RtwComboBox *m_pCbPetList;
	RtwButton	*m_pBtnMergeBeads;
	RtwLabel	*m_pLbRate;
	RtwLabel	*m_pLbBeads[4];
};

#endif