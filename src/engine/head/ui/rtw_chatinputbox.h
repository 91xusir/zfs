#ifndef _RTUI_INPUTBOX_H
#define _RTUI_INPUTBOX_H

namespace ui
{
	class RtwChatInputBox : public RtwTextBox
	{
	public:
		RtwChatInputBox();
		virtual ~RtwChatInputBox();
	public:
		virtual bool Init();
		virtual void Refresh();
		virtual void Show(const std::string& Text, const std::string& TitleText = "", const std::string& InputText = "", bool bBtnOK = true, bool bBtnCancel = true);
		virtual void Hide();

		std::string getInputString() { return m_pEdt_Edit->GetText(); }
		void SetPassword(bool bPassword,char ShowChar='*'){m_pEdt_Edit->SetPassword(bPassword);m_pEdt_Edit->SetPasswordChar(ShowChar);}
		void SetInputTitle( std::string sString ) { m_pEdt_Title->SetText( sString ) ;}
		void SetNumeric(bool bFlag) { return m_pEdt_Edit->SetNumeric(bFlag);}
	private:
		RtwForm*        m_pFrm_This;
		RtwButton*      m_pBtn_Ok;
		RtwButton*      m_pBtn_Cancel;
		RtwLabel*       m_pLab_Message;
		RtwTextBox*     m_pEdt_Edit;
		RtwLabel*       m_pEdt_Title;
		void OnLClick_Ok(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnLClick_Cancel(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnShow_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnHide_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		bool            m_bEventOkSended;   // 是否已经发送了EvOk消息
	public:
		RtwEventDispatcher  EvOk;
		RtwEventDispatcher  EvCancel;
	};
} 
#endif // _RTUI_INPUTBOX_H
