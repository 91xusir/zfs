#ifndef _INC_UI_FORM_FEOD_BANK_H_
#define _INC_UI_FORM_FEOD_BANK_H_

class UIFormFeodBank
{
public:
	void Refresh();
	void Show(DWORD npcID);
	void Hide();
	bool IsVisible();

	int m_iFeodBank;

	void OnWithdrawSuccess(int newBank);

private:
	RtwWidget *m_pFrmThis;		// Õû¸ö´°¿Ú

	RtwButton *m_pBtnOK;
	RtwButton *m_pBtnCancel;

	RtwWidget *m_tbWithdraw;
	RtwWidget *m_lbFeodBank;

	void OnBtnOK(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);

private:
	void OnPreDraw();

	DWORD m_npcID;
public:
	UIFormFeodBank();
	~UIFormFeodBank();
};

#endif // _INC_UI_FORM_FEOD_BANK_H_
