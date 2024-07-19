#ifndef _RTUI_LABEL_H
#define _RTUI_LABEL_H

namespace ui 
{
	class RtwLabel : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwLabel, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	protected:
		RtwLabel();
		virtual ~RtwLabel();
	public:
		virtual void OnBeforeDestroy();
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);

		void SetHyperLink(SHyperLink* pHyperLink);

		SHyperLink* getHyperLink() { return (m_HyperLink.Type != hlinkInvalid) ? &m_HyperLink : NULL; }
	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag;
	protected:
		virtual void CalcClient();
		SHyperLink      m_HyperLink;
	private:
		void _ResetSelf();
		void OnLClick_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
	};
}
#endif