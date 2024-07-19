#ifndef _RTUI_FORM_H
#define _RTUI_FORM_H

namespace ui 
{
	class RtwForm : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwForm, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;

	protected:
		RtwForm();
		virtual ~RtwForm();
	public:
		virtual void OnBeforeDestroy();
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnOffset(const SSize& size);

		virtual void Show();	// add by chib 2006-10-24  
		virtual void Hide();	// add by chib 2006-10-24

		void SetTitleText(const std::string& text);
		void SetTitleImage(RtwImage* pImage);
		void SetShowTitleBar(bool bShow, bool bAdjectOtherPosition = true);
		void SetShowCloseButton(bool bShow);
		void SetTitleBarHeight(int height);
		void SetMovable(bool bCanMove) { m_bCanMove = bCanMove; }
		void SetMode(bool bFlag = false){ m_bMode = bFlag; }				//设置对话框模式 [chib] [2006-10-24]

		const std::string& getTitleText() { return m_TitleText; }
		const RtwRect& getTitleRect() { return m_rcTitle; }
		bool getShowTitleBar() { return m_bShowTitleBar; }
		bool getShowCloseButton() { return m_pBtn_Close->getVisible(); }
		int getTitleBarHeight() { return m_TitleBarHeight; }
		RtwButton* getCloseButton() { return m_pBtn_Close; }
		void DoCalcClient(){CalcClient();}
		virtual void Run(DWORD dwDelta)
		{
			RtwWidget::Run(dwDelta);
			IMAGETICK(m_pTitleBarImage,		dwDelta);
		}

		void		SetEscapeHide(bool bEnable = false){m_bEnableEscapeHide = bEnable;}
		bool		GetEscapeHide(){return m_bEnableEscapeHide;}
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfMouseMove |wfDClick;
	protected:
		virtual void CalcClient();
	protected:
		bool            m_bShowTitleBar;
		int             m_TitleBarHeight;
		bool            m_bCanMove;
		bool            m_bCanResize;
		bool			m_bEnableEscapeHide;

		RtwRect         m_rcTitle;
		std::string     m_TitleText;
		RtwImage*       m_pTitleBarImage;       // 标题栏图片
		RtwButton*      m_pBtn_Close;           // 关闭按钮
		bool			m_bMode;				//是否为模式对话框	[chib][2006-10-24]
	private:
		void _ResetSelf();
		void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnClick_Close(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	public:
	};
} // namespace ui
#endif // _RTUI_FORM_H
