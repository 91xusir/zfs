//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_MEDIAFRAME_H
#define _RTUI_MEDIAFRAME_H


namespace ui {

	class CUiMediaFrame : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(CUiMediaFrame, RtwWidget, 0, "ui")

			friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;

	protected:
		CUiMediaFrame();
		virtual ~CUiMediaFrame();

		virtual void OnBeforeDestroy();

	public:
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);

		void SetNextHyperLink(SHyperLink* pHLink);
		void ProcessNextHyperLink();

	public:
		SHyperLink* getNextHyperLink() { return (m_NextHyperLink.Type != hlinkInvalid) ? &m_NextHyperLink : NULL; }

	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfFocus | wfInput | wfMouseMove;

	protected:
		virtual void CalcClient();

		SHyperLink      m_NextHyperLink;

	private:
		void _ResetSelf();

	public:
		RtwEventDispatcher  EvHyperLink;
	};


} // namespace ui



#endif // _RTUI_MEDIAFRAME_H