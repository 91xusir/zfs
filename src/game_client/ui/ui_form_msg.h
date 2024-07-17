#ifndef _INC_UI_FORM_CONFIRM_H_
#define _INC_UI_FORM_CONFIRM_H_

class UIFormMsg:public RtwRefObject
{
public:
	friend class RtwRefPtr<UIFormMsg>;
	enum FormType
	{
		TYPE_OK,
		TYPE_OK_CANCEL
	};

public:
	void Show(const RtString& text, bool modal = true);
	void Hide();
	//void Close();//rem by fox for bug
	bool IsVisible();
	void SetUserData(void* data);
	void* GetUserData();
	RtwButton* GetButtonOK(FormType t);

public:
	//<modify by fox for bug>
	static UIFormMsg* ShowStatic(const RtString& text, FormType t = TYPE_OK, 
		bool modal = true, const string& id = "",bool vDefaultLayer=true);//vDefaultLayer=true 就会被清除，在退到主界面的时候
	static void Clear(bool vAll)
	{
		if(vAll)
		{//表示统统清除
			s_forms.clear();
			return ;
		}

		unordered_map<string, RtwRefPtr<UIFormMsg> >::iterator It;
		
		for(It=s_forms.begin();It!=s_forms.end();)
		{
			if(It->second->mDefaultLayer)
			{//是默认层，会被清除掉
				It=s_forms.erase(It);
			}
			else
			{
				++It;
			}
		}
	};
	//</modify by fox for bug>

private:
	RtwWidget::Ptr m_pFrmThis;		// 整个窗口
	bool m_autoDelete;
	FormType m_formType;

	void OnLClick_OK(RtwWidget* sender, void*);
	void OnLClick_Cancel(RtwWidget* sender, void*);
	//void OnClose(void*, void*);//rem by fox for bug
	void OnKeyDown(RtwWidget* sender, RtwEventDelegate* e);

	static unordered_map<string, RtwRefPtr<UIFormMsg> > s_forms;//modify by fox for bug


public:
	RtwEventDispatcher EvOK;
	RtwEventDispatcher EvCancel;

//public://rem by fox for bug
private:
	//UIFormMsg(FormType type = TYPE_OK, bool autoDelete = false, int layer = -1);//rem by fox for bug
	UIFormMsg(FormType type = TYPE_OK,int layer = -1);//modify by fox for bug
	~UIFormMsg();

	string m_id;
	bool	mDefaultLayer;//是否使用默认层
};


#endif // _INC_UI_FORM_CONFIRM_H_
