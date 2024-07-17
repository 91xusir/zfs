#ifndef _INC_UI_FORM_BANKPASS_H_
#define _INC_UI_FORM_BANKPASS_H_

class UIFormBankPass
{
public:
	enum EState
	{
		sAddPass,
		sChangePass,
		sRemovePass,
		sCheck
	};

	void Show(DWORD npcID, EState state);
	void Hide();
	bool IsVisible();										// 是否已经显示
	void Refresh();

private:
	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwWidget *m_pTitle;
	RtwWidget *m_pText1;
	RtwWidget *m_pEdit1;
	RtwWidget *m_pText2;
	RtwWidget *m_pEdit2;
	RtwWidget *m_pText3;
	RtwWidget *m_pEdit3;
	RtwButton *m_pBtnOK;
	RtwButton *m_pBtnCancel;

private:
	void OnPreDraw();
	void OnClicked_OK(RtwWidget* sender, void*);
	void OnClicked_Cancel(RtwWidget* sender, void*);

	EState m_state;
	DWORD m_npcID;

public:
	UIFormBankPass();
	~UIFormBankPass();
};

#endif // _INC_UI_FORM_BANKPASS_H_
