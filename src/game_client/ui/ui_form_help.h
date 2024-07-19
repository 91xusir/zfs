#ifndef __UI_FORM_HELP_H__
#define __UI_FORM_HELP_H__

class UIFormHelp
{
public:
	UIFormHelp();
	virtual ~UIFormHelp();

	bool IsVisible();
	void Hide(){guard;m_pFrmThis->Hide();unguard;}
	void Show(){guard;m_pFrmThis->Show();unguard;}
	void OnShow(RtwWidget* pSender, RtwEventDelegate* pEvent);
    void OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent);
    void OnUIHyperlink(RtwWidget* pSender, RtwEventDelegate* pEvent);

    void SetURL(const char* szURL);
    const char* GetURL();

private:
    RtwWidget*      m_pFrmThis;
    RtwHtmlView*    m_pHtmlView;
    RtwTree*        m_pTree;
    string          m_strURL;

};

#endif // __UI_FORM_HELP_H__
