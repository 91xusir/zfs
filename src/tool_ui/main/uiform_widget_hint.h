#pragma once

class UiForm_WidgetHint : public IUiEventReceiver, public IUiGameWidget
{
public:
	UiForm_WidgetHint();
	~UiForm_WidgetHint();

	void Run(DWORD dwDelta);
    virtual bool Init();
    virtual void Refresh();
    virtual void Show();
    virtual void Hide();
    virtual bool IsVisible();

private:
    void OnClassHint_WidgetHint(RtwWidget* pWidget, RtwEventDelegate* pEvent);

private:
    RtwForm*    m_pFrm_This;
    RtwLabel*   m_pLab_Type;
    RtwLabel*   m_pLab_ID;
    RtwLabel*   m_pLab_Name;
    RtwLabel*   m_pLab_RefCount;
};

