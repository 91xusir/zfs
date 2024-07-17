#ifndef _RTUI_SCROLLBAR_H
#define _RTUI_SCROLLBAR_H
//********************************************************************
//	created:	2010.05.04 10:00
//	filename: 	rtw_scrollbar.h
//	author:		ldr123
//	purpose:	滚动条
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
	class RtwButton;

	class RtwScrollBar : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwScrollBar, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwScrollBar();
		virtual ~RtwScrollBar();
	public:
		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);

		void SetData(int min = -1, int max = -1, int value = -1, Real scale = -1.0);
		void SetMin(int min);
		void SetMax(int max);
		void SetRange(int min, int max) { SetMin(min); SetMax(max); }
		void SetValue(int value,bool b = false);// add `bool b = false` [3/20/2009 tooth.shi]
		void SetScale(float scale);
		void SetSValue(int svalue){ m_Value-=svalue;}

		int getMin() { return m_Min; }
		int getMax() { return m_Max; }
		int GetValue() { return m_Value; }
		int getValue() { return m_Value; }
		int getScale() { return m_Scale; }
		void SetMaximum(int min) { SetMin(min);	}

		RtwWidget* getScrollHandle() { return m_pWgtScrollHandle; }
		int getScrollHandleHeight() { return m_ScrollHandleHeight; }
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove;
		static const int m_DefaultWidth = 10;
		static const int m_DefaultScrollHandleHeight = 20;
	protected:
		virtual void CalcClient();
		int ValueFromPoint(const SPoint& point);
		void OnMouseMove_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		int m_Min;
		int m_Max;
		int m_Value;
		float m_Scale;                        // 滚动条大小与滚动长度的比例

		RtwWidget* m_pWgtScrollHandle;

		int m_ScrollHandleHeight;           // "滚动条"的高度
		RtwRect m_rcUpButton;
		RtwRect m_rcDownButton;
		RtwRect m_rcScrollSpace;
		RtwRect m_rcScrollHandle;
	private:
		void _ResetSelf();
	public:
		RtwEventDispatcher EvScroll;
	};

	class RtwVScrollBar : public RtwScrollBar
	{
		RT_DECLARE_DYNCREATE(RtwVScrollBar, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwVScrollBar();
		virtual ~RtwVScrollBar();
	public:
		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);

		void SetData(int min = -1, int max = -1, int value = -1, Real scale = -1.0);
		void SetMin(int min);
		void SetMax(int max);
		void SetRange(int min, int max) { SetMin(min); SetMax(max); }
		void SetValue(int value);
		void SetScale(float scale);
		void SetSValue(int svalue){ m_Value-=svalue;}

		void ScrollToTop();
		void ScrollToBottom();

		int getMin() { return m_Min; }
		int getMax() { return m_Max; }
		int GetValue() { return m_Value; }
		int getValue() { return m_Value; }
		int getScale() { return m_Scale; }

		RtwButton* getStepUpButton() { return m_pBtnStepUp; }
		RtwButton* getStepDownButton() { return m_pBtnStepDown; }
		RtwWidget* getScrollHandle() { return m_pWgtScrollHandle; }
		int getScrollHandleHeight() { return m_ScrollHandleHeight; }
		RtwRect getScrollSpaceRect();
		void showHint(std::string m_Nums,RtwWidget* pWidget,RtwVScrollBar* pWidgetV);//by steven 2007.1.11

		void  SetUseLeftRightButton( bool bUse )  { m_bHasLeftAndRightButton = bUse; }
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove;
		static const int m_DefaultWidth = 10;
		static const int m_DefaultScrollHandleHeight = 20;
	protected:
		virtual void CalcClient();
		int ValueFromPoint(const SPoint& point);

		void OnMouseMove_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseWheel_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseLDown_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnClick_StepUp(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnClick_StepDown(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		int m_Min;
		int m_Max;
		int m_Value;
		float m_Scale;                        // 滚动条大小与滚动长度的比例

		RtwButton* m_pBtnStepUp;
		RtwButton* m_pBtnStepDown;
		RtwWidget* m_pWgtScrollHandle;
		bool m_bHasLeftAndRightButton;

		int m_ScrollHandleHeight;           // "滚动条"的高度
		int m_OffsetDuringScroll;           // 拖动滚动条时的距离滚动条中心的偏移量
		RtwRect m_rcUpButton;
		RtwRect m_rcDownButton;
		RtwRect m_rcScrollSpace;
		RtwRect m_rcScrollHandle;
	private:
		void _ResetSelf();
	public:
		RtwEventDispatcher EvScroll;
	};

	class RtwHScrollBar : public RtwScrollBar
	{
		RT_DECLARE_DYNCREATE(RtwHScrollBar, RtwWidget, 0, "ui")
		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwHScrollBar();
		virtual ~RtwHScrollBar();
	public:
		virtual void OnBeforeDestroy();
	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);

		void SetData(int min = -1, int max = -1, int value = -1, Real scale = -1.0);
		void SetMin(int min);
		void SetMax(int max);
		void SetValue(int value);
		void SetScale(float scale);

		void ScrollToLeft();
		void ScrollToRight();

		int getMin() { return m_Min; }
		int getMax() { return m_Max; }
		int GetValue() { return m_Value; }
		int getValue() { return m_Value; }
		int getScale() { return m_Scale; }

		RtwButton* getStepLeftButton() { return m_pBtnStepLeft; }
		RtwButton* getStepRightButton() { return m_pBtnStepRight; }
		RtwWidget* getScrollHandle() { return m_pWgtScrollHandle; }
		int getScrollHandleWidth() { return m_ScrollHandleWidth; }
		RtwRect getScrollSpaceRect();
		void  showHint(std::string m_Nums,RtwWidget* pWidget,RtwHScrollBar* pWidgetV);
		void  SetUseLeftRightButton( bool bUse )  { m_bHasLeftAndRightButton = bUse; }
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove;
		static const int m_DefaultHeight = 10;
		static const int m_DefaultScrollHandleWidth = 20;
	protected:
		virtual void CalcClient();
		int ValueFromPoint(const SPoint& point);

		void OnMouseMove_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseLDown_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		void OnMouseWheel_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent); // Added by Wayne Wong 2010-12-20

		void OnClick_StepLeft(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnClick_StepRight(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		int m_Min;
		int m_Max;
		int m_Value;
		float m_Scale;                        // 滚动条大小与滚动长度的比例

		RtwButton* m_pBtnStepLeft;
		RtwButton* m_pBtnStepRight;
		RtwWidget* m_pWgtScrollHandle;

		int m_ScrollHandleWidth;           // "滚动条"的宽度
		int m_OffsetDuringScroll;           // 拖动滚动条时的距离滚动条中心的偏移量

		bool m_bHasLeftAndRightButton;
		RtwRect m_rcLeftButton;
		RtwRect m_rcRightButton;
		RtwRect m_rcScrollSpace;
		RtwRect m_rcScrollHandle;
	private:
		void _ResetSelf();
	public:
		RtwEventDispatcher EvScroll;
	};
} 
#endif
