#pragma once

enum eAlignType
{
	LEFT,RIGHT,BOTTOM, TOP, SAMEWIDTH, SAMEHEIGHT
};
class UiForm_ToolBar : public IUiEventReceiver, public IUiGameWidget
{
public:
	UiForm_ToolBar();
	~UiForm_ToolBar();

	void Run(DWORD dwDelta);
    virtual bool Init();
    virtual void Refresh();
    virtual void Show();
    virtual void Hide();
    virtual bool IsVisible();

private:
    void OnLClick_Cut(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_Copy(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_Paste(RtwWidget* pWidget, RtwEventDelegate* pEvent);

	void OnLClick_Left(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_Top(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_Bottom(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_Right(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_MoveUp(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_MoveDown(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_SameWidth(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_SameHeight(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	void OnLClick_AllSame(RtwWidget* pWidget, RtwEventDelegate* pEvent);

	void AlignWidget(eAlignType eType);
public:
    RtwForm*    m_pFrm_This;
    RtwButton*  m_pBtn_ShowWidgetsLine;
    RtwButton*  m_pBtn_Cut;
    RtwButton*  m_pBtn_Copy;
    RtwButton*  m_pBtn_Paste;
    RtwButton*  m_pBtn_AlignTop;
    RtwButton*  m_pBtn_AlignBottom;
    RtwButton*  m_pBtn_AlignLeft;
    RtwButton*  m_pBtn_AlignRight;
	RtwButton*  m_pBtn_MoveUp;
	RtwButton*  m_pBtn_MoveDown;
	RtwButton*  m_pBtn_SameWidth;
	RtwButton*  m_pBtn_SameHeight;
	RtwButton*  m_pBtn_SameAll;
// 	RtwHtmlView* m_HtmlView;
};

