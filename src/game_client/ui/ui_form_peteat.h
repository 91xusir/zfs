#ifndef _INC_UI_FORM_PETEAT_H_
#define _INC_UI_FORM_PETEAT_H_


class UIFormPetEat
{
public:
	UIFormPetEat();
	~UIFormPetEat();

	void Show();
	void Hide();
	bool IsVisible();
	void Refresh();

private:
	RtwWidget *m_pFrmThis;
	RtwButton *m_pBtn_PetFrom;
    RtwButton *m_pBtn_PetTo;
    RtwButton *m_pBtn_Ok;
    RtwButton *m_pBtn_Cancel;

    SItemID m_PetFrom;
    SItemID m_PetTo;

private:
	void OnPreDraw(bool bComposeList = true, bool bComposingDetail = true);
	void OnClicked_Ok(RtwWidget* sender, void*);
	void OnClicked_Cancel(RtwWidget* sender, void*);

    void OnDragTo_PetFrom(RtwWidget* sender, void*);
    void OnDragTo_PetTo(RtwWidget* sender, void*);
    void OnDragEnd_PetFrom(RtwWidget* sender, void*);
    void OnDragEnd_PetTo(RtwWidget* sender, void*);
    void OnGetHintText(RtwWidget* sender, void*);
};

#endif // _INC_UI_FORM_PETEAT_H_
