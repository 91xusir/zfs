#ifndef _INC_UI_FORM_CARD_H_
#define _INC_UI_FORM_CARD_H_

class UIFormCard
{
public:
	void Show(const char* szTitle, const char* szContent, const char* szButton, bool bModal = true);
	void Hide();
	bool IsVisible();

    void SetCardID(const char* szContent);
	const string GetCardID() { return m_strCardID; }

    void SetUserData(long lNpcID, long lType);

private:
	RtwWidget::Ptr  m_pFrmThis;		// Õû¸ö´°¿Ú
    RtwWidget*      m_pCaption;
    RtwTextBox*     m_pTextInput;
    RtwButton*      m_pBtnOK;

	void OnLClick_OK(RtwWidget* sender, void*);
	void OnLClick_Cancel(RtwWidget* sender, void*);
	void OnClose(void*, void*);

	string m_strCardID;
    long m_lNpcID;
    long m_lType;

//public:
//    RtwEventDispatcher EvOK;
//  RtwEventDispatcher EvCancel;

public:
	UIFormCard();
	~UIFormCard();
};

#endif // _INC_UI_FORM_CARD_H_
