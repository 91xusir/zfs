
#ifndef __UI_FORM_PET_MERGE_H_
#define __UI_FORM_PET_MERGE_H_

#include "ui/rtw_ui.h"

class UIFormPetMerge
{
public:
	UIFormPetMerge();
	~UIFormPetMerge();

	void Refresh();
	void Show();
	void Hide();
	void OnPreDraw();
	bool IsVisible();
	void Reset();

	RtwWidget* OnGetThisForm(){return m_pFmWindow;}

	void OnHide(RtwWidget *sender,void *);

	void OnFinishMerge(bool succeed);

	void SetItem(SItemID item);
	void DragOutItem(RtwButton *sender,void *);

	RtwButton	*m_pBtnItem;

private:
	SItemID m_pet1;
	SItemID m_pet2;
	SItemID m_item;
	list<SItemID> m_listPet;
	DWORD *m_pdwSelectPet1;
	DWORD *m_pdwSelectPet2;

	void OnSelectPet1(RtwComboBox *sender,void *);
	void OnSelectPet2(RtwComboBox *sender,void *);

	void OnClickMerge(RtwButton *sender,void *);
	void SetMergeButton();

private:
	RtwForm		*m_pFmWindow;
	Rtw3DView	*m_pFmPetView1;
	Rtw3DView	*m_pFmPetView2;
	RtwComboBox *m_pCbPetList1;
	RtwComboBox *m_pCbPetList2;
	RtwButton	*m_pBtnMerge;
	RtwLabel	*m_pLbRate;
};

#endif