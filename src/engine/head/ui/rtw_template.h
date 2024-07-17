//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_TEMPLATE_H
#define _RTUI_TEMPLATE_H


namespace ui {

	class CUiTemplate : public RtObject
	{
		RT_DECLARE_DYNCREATE(CUiTemplate, RtObject, 0, "ui")

	public:
		CUiTemplate();
		virtual ~CUiTemplate();

		void SetNode(RtsXmlDoc::NodePtr* pNode);
		bool IsCompatibleWith(EWidgetType WidgetType);

		RtsXmlDoc::NodePtr* getNode() { return m_pNode; }
		EWidgetType getWidgetType() { return m_WidgetType; }
		std::string getWidgetName() { return m_pNode ? m_pNode->GetProp_s("Name") : ""; }

	private:
		EWidgetType         m_WidgetType;
		RtsXmlDoc::NodePtr* m_pNode;
	};

	typedef std::list<CUiTemplate*> UiTemplateList;

} // namespace ui


#endif // _RTUI_TEMPLATE_H
