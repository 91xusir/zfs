#ifndef _INC_UI_FORM_FEOD_CONTENT_H_
#define _INC_UI_FORM_FEOD_CONTENT_H_

#include "union_base.h"

class UIFormFeodContent
{
public:
	void Refresh();
	void Show(DWORD npcID);
	void Hide();
	bool IsVisible();

	int m_IncomeToday;
	int m_FeodBank;
	int m_nTaxRate;

private:
	RtwWidget *m_pFrmThis;		// Õû¸ö´°¿Ú

	RtwButton *m_pBtnOK;
	RtwButton *m_pBtnCancel;

	RtwWidget *m_lbIncomeToday;
	RtwWidget *m_lbBank;
	RtwWidget *m_lbTaxRate;

	void OnBtnOK(RtwWidget* sender, void*);
	void OnBtnCancel(RtwWidget* sender, void*);
private:
	void OnPreDraw();

	DWORD m_npcID;

public:
	UIFormFeodContent();
	~UIFormFeodContent();
};

class CGcFeodMgr
{
public:
	vector<SFeod> m_arrFeods;
	SFeod *Find(string name);

public:
	CGcFeodMgr();
	~CGcFeodMgr();
	void Clear();
	bool AddFeodInfo(SFeod& feod);
	void GetFeodInfo(string feodName, DWORD &dwCurrentUnion, int &rate);
};

extern CGcFeodMgr	g_GcFeodMgr;

#endif // _INC_UI_FORM_FEOD_CONTENT_H_
