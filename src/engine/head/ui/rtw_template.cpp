#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(CUiTemplate, RtObject, 0, "ui")


CUiTemplate::CUiTemplate():m_pNode(NULL)
{
	m_WidgetType = wtUnknown;
}

CUiTemplate::~CUiTemplate(){}

void CUiTemplate::SetNode(RtsXmlDoc::NodePtr* pNode)
{
	m_pNode = pNode;
	m_WidgetType = g_workspace.getWidgetTypeFromTypeName(pNode->strName);
}

bool CUiTemplate::IsCompatibleWith(EWidgetType WidgetType)
{
	switch (m_WidgetType)
	{
	case wtWidget:
		return true;
	default:
		if (WidgetType == m_WidgetType)
			return true;
		else 
			return false;
	}
	return true;
}
