#ifndef _INC_UI_FORM_CONFIRM_H_
#define _INC_UI_FORM_CONFIRM_H_

class UIFormMsg
{
public:
	enum FormType
	{
		TYPE_OK,
		TYPE_OK_CANCEL
	};

public:
	void Show(const RtString& text, bool modal = true);
	void Hide();
	bool IsVisible();

public:
	static UIFormMsg* ShowStatic(const RtString& text, FormType t = TYPE_OK, bool modal = true);

private:
	RtwWidget::Ptr m_pFrmThis;		// Õû¸ö´°¿Ú

	RtwButton *m_pBtnOk;
	RtwButton *m_pBtnCancel;
	bool m_autoDelete;

	void OnLClick_OK(RtwWidget* sender, void*);
	void OnLClick_Cancel(RtwWidget* sender, void*);
	void OnClose(void*, void*);


public:
	RtwEventBroadcast EvOK;
	RtwEventBroadcast EvCancel;

public:
	UIFormMsg(FormType type = TYPE_OK, bool autoDelete = false);
	~UIFormMsg();
};

#endif // _INC_UI_FORM_CONFIRM_H_
