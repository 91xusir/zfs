#pragma once


class UiLayerStudio : public IUiEventReceiver, public IUiGameWidget
{
public:
	UiLayerStudio();
	~UiLayerStudio();

	void Run(DWORD dwDelta);
    virtual bool Init();
    virtual void Refresh();
    virtual void Show();
    virtual void Hide();
    virtual bool IsVisible();

    CUiDocument     m_UiDocument;
    CUiLayer*       m_pLay_This;             // ±à¼­Æ÷²ã
    std::string     m_FileName;
private:
    RtwLabel*   m_pLbl_OperateState;
    RtwLabel*   m_pLbl_MouseX;
    RtwLabel*   m_pLbl_MouseY;
    RtwLabel*   m_pLbl_Fps;

    void Refresh_Attributes();

	void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnKeyChar_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
};
