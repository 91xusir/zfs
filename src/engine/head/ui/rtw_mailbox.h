//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_MESSAGEBOX_H
#define _RTUI_MESSAGEBOX_H



namespace ui {

	class RtwMailBox : public RtwWidget
	{
	public:
		RtwMailBox();
		virtual ~RtwMailBox();

	public:
		virtual bool Init();
		virtual void Refresh();
		virtual void Show(const std::string& Text, const std::string& TitleText = "", bool bBtnOK = true, bool bBtnCancel = true);
		virtual void Hide();
		// add [3/17/2009 tooth.shi]
		void InsertImage(const char* key);
		void SetReadOnly(bool bRead);
		RtwButton* getOkButton(){return m_pBtn_Ok;};
		RtwButton* getCacelButton(){return m_pBtn_Cancel;};
		void SetText(const std::string& strMsg);
		std::string getText();

	private:
		RtwForm*        m_pFrm_This;
		RtwButton*      m_pBtn_Ok;
		RtwButton*      m_pBtn_Cancel;
		RtwLabel*       m_pLab_Message;
		RtwLabel*       m_pLab_MessageTitle;

		void OnLClick_Ok(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnLClick_Cancel(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnShow_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnHide_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		bool            m_bEventOkSended;   // 是否已经发送了EvOk消息

	public:
		RtwEventDispatcher EvOk;
		RtwEventDispatcher EvCancel;
	};

} // namespace ui


#endif // _RTUI_MESSAGEBOX_H
