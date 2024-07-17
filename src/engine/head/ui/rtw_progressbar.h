#ifndef _RTUI_PROGRESSBAR_H
#define _RTUI_PROGRESSBAR_H

namespace ui
{
	class RtwProgressBar : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwProgressBar, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwProgressBar();
		virtual ~RtwProgressBar();
	public:
		virtual void OnBeforeDestroy();

	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnOffset(const SSize& size);

		void SetValue(Real value);
		void SetDirection(char direction){m_Direction = direction;}
		void SetBarImage(RtwImage* pImage);

		Real GetValue() { return m_Value; }
		Real getValue() { return m_Value; }

		void Run(DWORD dwDelta);
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag;

	protected:
		virtual void CalcClient();

		Real            m_Value; 
		char			m_Direction;		//进度条的方向， 1为横向(default)，2为竖向
		RtwImage*       m_pImgBar;
		RtwRect         m_rcBar;

	private:
		void _ResetSelf();
	};
} 
#endif 
