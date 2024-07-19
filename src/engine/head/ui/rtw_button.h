#ifndef _RTUI_BUTTON_H
#define _RTUI_BUTTON_H
//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_button.h.
//	author:		ldr123
//	purpose:	按钮
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
	class RtwButton : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwButton, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
		typedef RtwRefPtr<RtwButton> Ptr;
	public:
		RtwButton();
		virtual ~RtwButton();
	public:
		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);
		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		void SetButtonStateImages(RtwImage* pNormal, RtwImage* Highlight, RtwImage* pPushed, RtwImage* pDisabled);
		void SetNormalImage(RtwImage* pImg);
		void SetHighlightImage(RtwImage* pImg);
		void SetPushedImage(RtwImage* pImg);
		void SetDisabledImage(RtwImage* pImg);
		void SetMaskImage( RtwImage* pImg );
		void SetSelected(bool bSelected);
		void SetBlink(bool bBlink);
		void SetBlinkInterval(short sBlinkInterval);
		void SetPushed(bool bPushed);
		void EnableAlphaEffect(bool bAlpha);
		void SetAlphaRef(float fAlpha){ m_nBackGroundAlpha = fAlpha ;}
		void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void SetHoverImage(RtwImage* pImg);

		RtwImage* getNormalImage()      { return m_pImgNormal; }
		RtwImage* getHighlightImage()   { return m_pImgHighlight; }
		RtwImage* getPushedImage()      { return m_pImgPushed; }
		RtwImage* getDisabledImage()    { return m_pImgDisabled; }
		void  SetForceDrawImage( int nForceImage )      { m_nForceDrawBackImage = nForceImage; }
		virtual void Run(DWORD dwDelta);
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfGrabMouse | wfMouseMove;
		enum
		{
			NO_FORCE                  = 0,
			FORCE_DRAW_NORMALIMAGE    = 1 ,
			FORCE_DRAW_HIGHLIGHTIMAGE = 2 ,
			FORCE_DRAW_PUSHEDIMAGE    = 3 ,
			FORCE_DRAW_DISABLEIMAGE   = 4 
		};
	protected:
		RtwImage* m_pImgNormal;
		RtwImage* m_pImgHighlight;
		RtwImage* m_pImgPushed;
		RtwImage* m_pImgDisabled;
		RtwImage* m_pImgMask;

		bool      m_bCanMove;
		bool      m_bCanResize;
		bool	  m_bPushed;
		bool	  m_bSelected;
		bool	  m_bNeedSelect;
		bool	  m_bBlink;
		short	  m_sBlinkInterval;
		int       m_nForceDrawBackImage;
		virtual void CalcClient();
	private:
		void _ResetSelf();
	public:
		void SetHoverImage(RtwComplexImage cimage);
		void SetNormalImage(RtwComplexImage pImg){}
		void SetPressedImage(RtwComplexImage pImg){}
		void SetFocusImage(RtwComplexImage pImg){}
		void SetMaskImage(RtwComplexImage pImg){}
		void LoadAttributesFromTemplate(std::string namge);
		void SetChecked(bool push) { m_bPushed = push;}
		void SetCheckedImage(RtwComplexImage cimage) {}
		void Resize(int height,int widget){}
	};

	class CUiCheckButton : public RtwButton
	{
		RT_DECLARE_DYNCREATE(CUiCheckButton, RtwButton, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		CUiCheckButton();
		virtual ~CUiCheckButton();
	public:
		virtual void OnBeforeDestroy();
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);
		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnOffset(const SSize& size);
		void SetCheckedImage(RtwComplexImage* pImg);
		void SetHighlightImage(RtwComplexImage* pImg);
		void SetChecked(bool bChecked);
		void SetCheckStyleLayout(bool bCheckStyle);
		bool GetChecked() { return m_bChecked; }
		bool getCheckStyleLayout() { return m_bCheckStyleLayout; }
		void setRadioID(string strRadioID);

		virtual void Run(DWORD dwDelta);
		void UpdateRadioButton();
	protected:
		virtual void CalcClient();
	public:
		virtual void OnMouseClick(const RtwEventDelegate& e);
	protected:
		bool m_bChecked;
		bool m_bCheckStyleLayout;           // 是使用Checkbox(图片在左边，文字在右边)的布局，还是使用普通button的布局

		RtwComplexImage* m_pImgChecked;
		RtwComplexImage* m_pImgCheckHighlight;
		RtwRect m_rcCheckImage;
		RtwRect m_rcText;
		RtwImage* m_NormalImage;
		RtwImage* m_HightImage;
		RtwImage* m_PushImage;
		string	  m_strRadioID;
	private:
		void _ResetSelf();
	};
	
	class RtwAliasButton : public RtwButton
	{
		RT_DECLARE_DYNCREATE(RtwAliasButton, RtwButton, 0, "ui")
		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwAliasButton();
		virtual ~RtwAliasButton();
	public:
		virtual void OnBeforeDestory();

		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnOffset(const SSize& size);

		void CommandButton();
		void OnDragIn_Aliasbtn(RtwWidget* sender, RtwEventDelegate* e);
		void OnDragOut_Aliasbtn(RtwWidget* sender, RtwEventDelegate* e);
		void SetButton(RtwButton* btn);
		void SetButton(const std::string strBtnName);
		RtwButton* GetButton() { return m_pRealBtn; };
	private:
		RtwButton*          m_pRealBtn;
		void _ResetSelf();
	};
	typedef std::map< std::string, std::vector<int> >::iterator iterMapStrVec;
} 
#endif
