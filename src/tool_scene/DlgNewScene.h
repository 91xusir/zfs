#pragma once


// CDlgNewScene dialog

class CDlgNewScene : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CDlgNewScene)

public:
	CDlgNewScene(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewScene();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_SCENE, IDH = IDR_HTML_DLGNEWSCENE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

private:
    int     m_iWidth;
    int     m_iHeight;

public:
    int GetWidth()  { return m_iWidth; }
    int GetHeight() { return m_iHeight; }

};
