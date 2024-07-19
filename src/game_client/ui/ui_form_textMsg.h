#ifndef _INC_UI_FORM_TEXTMSG_H_
#define _INC_UI_FORM_TEXTMSG_H_

class UIFormTextMsg:public RtwRefObject
{
public:
	friend class RtwRefPtr<UIFormTextMsg>;
	enum FormType
	{
		TYPE_OK,
		TYPE_OK_CANCEL
	};

public:
	void Show(const RtString& title, const RtString& content, bool modal = true);
	void Hide();
	bool IsVisible();
    void SetString(string& str);
	RtString GetString() { return m_string; }
	void SetNumberOnly(bool bNumberOnly);
    void SetPassword(bool bPassword);
	void SetTxtLength(int num);
	void SetMaxNum(int num);

public:
	//<modify by fox for bug>
	static UIFormTextMsg* ShowStatic(const RtString& title, const RtString& content, FormType t = TYPE_OK_CANCEL, bool modal = true,const string& id="");
	void static Clear()
	{
		s_forms.clear();
	};
	//</modify by fox for bug>

private:
	RtwWidget::Ptr m_pFrmThis;		// 整个窗口


	bool m_autoDelete;

	void OnInputChar(RtwWidget* sender, void*);
	void OnUpdateText(RtwWidget* sender, void*);
	void OnLClick_OK(RtwWidget* sender, void*);
	void OnLClick_Cancel(RtwWidget* sender, void*);
	//void OnClose(void*, void*);//rem by fox for bug

	static unordered_map<string, RtwRefPtr<UIFormTextMsg> > s_forms;//add by fox for bug

	RtString m_string;

public:
	RtwEventDispatcher EvOK;
	RtwEventDispatcher EvCancel;

//public:
private:
	//UIFormTextMsg(FormType type = TYPE_OK_CANCEL, bool autoDelete = false);//rem by fox for bug
	UIFormTextMsg(FormType type = TYPE_OK_CANCEL);//modify by fox for bug
	~UIFormTextMsg();

	string				m_id;
	int					m_maxNum;
	int					m_iLength;
	bool				m_bNumOnly;
	
};


class CUIFormInputBox:public RtwRefObject
{
public:
	friend class RtwRefPtr<UIFormTextMsg>;

public:
	void Show(const RtString& title, const RtString& content, bool modal = true);
	void Hide();
	bool IsVisible();
	void SetString(string& str);
	RtString GetString() { return m_string; }
	int GetNumFromInputBox();
	void SetNumberOnly(bool bNumberOnly);
	void SetPassword(bool bPassword);
	void SetTxtLength(int num);
	void SetMaxNum(int num);
	void OnMoveToXY(int x,int y);
	//<modify by fox for bug>
	static CUIFormInputBox* ShowStatic(const RtString& title, const RtString& content, bool modal = true,const string& id="");
	void static Clear()
	{
		input_forms.clear();
	};
	//</modify by fox for bug>
	CUIFormInputBox();//modify by fox for bug
	~CUIFormInputBox();

private:

	void OnUpdateText(RtwWidget* sender, void*);
	void OnInputChar(RtwWidget* sender, void*);
	void OnLClick_OK(RtwWidget* sender, void*);
	void OnLClick_Cancel(RtwWidget* sender, void*);
	void OnAddNum(RtwWidget* sender, void*);
	void OnSubNum(RtwWidget* sender, void*);
	void OnSetMaxNum(RtwWidget* sender, void*);

public:
	RtwEventDispatcher EvOK;
	RtwEventDispatcher EvCancel;

private:
	RtString			m_string;
	int					m_maxNum;
	int					m_iLength;
	RtwWidget::Ptr		m_pFrmThis;		// 整个窗口

	bool				m_autoDelete;
	string				m_id;
	static unordered_map<string, RtwRefPtr<CUIFormInputBox> > input_forms;//add by fox for bug
};

#endif // _INC_UI_FORM_TEXTMSG_H_
