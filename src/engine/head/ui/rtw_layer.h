#ifndef _RTUI_LAYER_H
#define _RTUI_LAYER_H

namespace ui
{

	class CUiLayer;
	class CUiDesktop;

	typedef std::list<CUiLayer*> UiLayerList;


	// CUiLayer ÊÇÒ»¸öWidgetÈÝÆ÷
	class CUiLayer : public RtwWidget
	{
		friend class CUiWidgetFactory;

	protected:
		CUiLayer();
		virtual ~CUiLayer();

		virtual void OnBeforeDestroy();

	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove | wfDClick | wfKeyboardEvent | wfEnable;

		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void Run(DWORD dwDelta);

		void Render(CUiWidgetContainer* pExcludeWidgets = NULL);
		void PrintToConsole(const std::string& prefix = "");

		bool PickWidget(const SPoint& Position, RtwWidget** ppWidget = NULL, DWORD FlagMask = 0x00000000, CUiWidgetContainer* pExcludeWidgets = NULL);

		bool AddWidget(RtwWidget* pWidget);
		bool RemoveWidget(DWORD WidgetID, bool bRecursive = false);
		bool RemoveWidget(const std::string& WidgetName, bool bRecursive = false);
		bool FindWidget(DWORD WidgetID, RtwWidget** ppWidget = NULL, bool bRecursive = false);
		bool FindWidget(const std::string& WidgetName, RtwWidget** ppWidget = NULL, bool bRecursive = false);
		bool BringWidgetToTop(DWORD WidgetID);
		bool BringWidgetToBottom(DWORD WidgetID);

		UiWidgetList::iterator WidgetBegin();
		UiWidgetList::iterator WidgetEnd();
		UiWidgetList::reverse_iterator WidgetBegin_Reverse();
		UiWidgetList::reverse_iterator WidgetEnd_Reverse();

		int getWidgetCount() { 		if (m_pChildren) return m_pChildren->getWidgetCount(); return 0;}

	private:
		void _ResetSelf();

	protected:
	};


} // namespace ui





#endif // _RTUI_LAYER_H
