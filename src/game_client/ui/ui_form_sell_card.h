#ifndef _INC_UI_FORM_SELL_CARD_H_
#define _INC_UI_FORM_SELL_CARD_H_

class UIFormSellCard
{
public:
	void Show(bool bModal = true);
	void Hide();
	bool IsVisible();

    void SetCardID(const char* szContent);
	const string GetCardID() { return m_strCardID; }

    void SetUserData(long lNpcID, long lType);

private:
	RtwWidget::Ptr  m_pFrmThis;		// Õû¸ö´°¿Ú
    RtwTextBox*     m_pTextCardID;
    RtwTextBox*     m_pTextCardPwd;
    RtwTextBox*     m_pTextPrice;
    RtwButton*      m_pBtnOK;

	void OnLClick_OK(RtwWidget* sender, void*);
	void OnLClick_Cancel(RtwWidget* sender, void*);
	void OnClose(void*, void*);

	string m_strCardID;
    string m_strCardPwdMD5;
    long m_lPrice;
    long m_lNpcID;
    long m_lType;

public:
	UIFormSellCard();
	~UIFormSellCard();
};

#endif // _INC_UI_FORM_SELL_CARD_H_
