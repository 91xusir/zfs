#ifndef _INC_UI_FORM_FEOD_TAX_H_
#define _INC_UI_FORM_FEOD_TAX_H_

class UIFormFeodTax
{
public:
	void Refresh();
	void Show(DWORD npcID);
	void Hide();
	bool IsVisible();

	int m_nTaxRate;
	int m_iIncomeToday;

	void OnModifySuccess(int newTaxRate);

private:
	RtwWidget *m_pFrmThis;		// Õû¸ö´°¿Ú

	RtwButton *m_pBtnOK;
	RtwButton *m_pBtnCancel;

	RtwWidget *m_tbTaxSet;
	RtwWidget *m_lbIncomeToday;

	void OnBtnOK(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);
private:
	void OnPreDraw();

	DWORD m_npcID;

public:
	UIFormFeodTax();
	~UIFormFeodTax();
};

#endif // _INC_UI_FORM_FEOD_TAX_H_
