#pragma once

class UiForm_MainMenu : public IUiEventReceiver, public IUiGameWidget
{
public:
	UiForm_MainMenu();
	~UiForm_MainMenu();

	void Run(DWORD dwDelta);
    virtual bool Init();
    virtual void Refresh();
    virtual void Show();
    virtual void Hide();
    virtual bool IsVisible();

private:
    RtwForm*        m_pFrm_This;
    RtwButton*      m_pBtn_Load;
    RtwButton*      m_pBtn_LoadAndAdd;
    RtwButton*      m_pBtn_Save;
    RtwButton*      m_pBtn_SaveAs;
	RtwButton*      m_pBtn_CtrlUIShow;   // Add by Peter 2010.12.31

    void OnLClick_Load(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_LoadAndAdd(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_Save(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_SaveAs(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	// Add by Peter 2010.12.31
	void OnLClick_CtrlUIShow(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnCtrlUIShow(bool bShouUI = true);
	bool UIIsVisible();

    void OnSave(RtwWidget* pWidget, RtwEventDelegate* pEvent);

	//add by lidongri (2009.11.17)
	RtwButton*	   m_pBtn_Config;
	void OnLClick_Config(RtwWidget* pWidget, RtwEventDelegate* pEvent);

	RtwButton*	   m_pBtn_Exit;
	void OnLClick_Exit(RtwWidget* pWidget, RtwEventDelegate* pEvent);

};

