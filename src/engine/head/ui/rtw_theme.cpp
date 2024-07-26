//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(CUiTheme, RtObject, 0, "ui")


std::string ui::g_SystemWidgetName[] = 
{
	"Null",
	"$laySystem",
	"$laySystem.$DefaultHint",
	"$laySystem.$DefaultHL",
	"$laySystem.$AutoSbv",
	"$laySystem.$AutoSbh",
	"$laySystem.$PopupMenu",
	"$laySystem.$MessageBox",
	"$laySystem.$MessageBox.$Ok",
	"$laySystem.$MessageBox.$Cancel",
	"$laySystem.$MessageBox.$Message",
	"$laySystem.$InputBox",
	"$laySystem.$InputBox.$Ok",
	"$laySystem.$InputBox.$Cancel",
	"$laySystem.$InputBox.$Message",
	"$laySystem.$InputBox.$Edit",
	"$laySystem.$InputBox.$MessageName",
	"$laySystem.$MessageBox.$MessageTitle",
};


CUiTheme::CUiTheme()
{
	m_pXmlDoc = RT_NEW RtsXmlDoc();
}

CUiTheme::~CUiTheme()
{
	DEL_ONE(m_pXmlDoc);
	foreach(UiTemplateList, iterTempl, m_Templates)
	{
		CUiTemplate* pTempl = *iterTempl;
		DEL_ONE(pTempl);
	}
	m_Templates.clear();
}

void CUiTheme::Reset()
{
	m_Name = "";
	m_Templates.clear();
}

bool CUiTheme::LoadFromFile(const std::string& FileName)
{
	Reset();

	UI_ENSURE_B(m_pXmlDoc->ParserFile(FileName.c_str()) == 0);
	UI_ENSURE_B(_ProcessRoot((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()));

	return true;
}
// 添加主题
bool CUiTheme::AddTemplate(CUiTemplate* pTempl)
{
	//<ProgressBar  Name="HPBar">   ->  "HPBar"  一个HPBar是一个模板
	if (FindTemplate(pTempl->getWidgetName()))
		return false;

	m_Templates.push_back(pTempl);
	return true;
}

bool CUiTheme::RemoveTemplate(const std::string& TemplName)
{
	foreach(UiTemplateList, iterTempl, m_Templates)
	{
		CUiTemplate* pTempl = *iterTempl;
		if (pTempl->getWidgetName() == TemplName)
		{
			m_Templates.erase(iterTempl);
			return true;
		}
	}
	return false;
}

CUiTemplate* CUiTheme::FindTemplate(const std::string& TemplName)
{
	foreach(UiTemplateList, iterTempl, m_Templates)
	{
		CUiTemplate* pTempl = *iterTempl;
		if (pTempl->getWidgetName() == TemplName)
		{
			return pTempl;
		}
	}
	return NULL;
}

bool CUiTheme::_ProcessRoot(RtsXmlDoc::NodePtr* pRoot)
{
	if (!pRoot)
		return true;

	const std::string& TagName = pRoot->strName;
	if (TagName == "Ui")
	{
		RtsXmlDoc::NodePtr* pChildNode = pRoot->pChildren;
		while (pChildNode)
		{
			if (pChildNode->strName == "Theme")
				UI_ENSURE_B(_Process_Theme(pChildNode))
			else if (pChildNode->strName == "")
			UI_ENSURE_B(1)

			pChildNode = pChildNode->pNext;
		}
	}

	return true;
}

bool CUiTheme::_Process_Theme(RtsXmlDoc::NodePtr* pNode)
{
	if (!pNode)
		return true;
	//<Theme Name="ZfsLike">
	m_Name = pNode->GetProp_s("Name");

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{//<Theme Name="ZfsLike">-><Templates Description="?Widget">
		const std::string& TagName = pChildNode->strName;
		if (TagName == "Templates")
		{
			RtsXmlDoc::NodePtr* pTemplNode = pChildNode->pChildren;
			while (pTemplNode)
			{
				//<Theme Name = "ZfsLike">-><Templates Description="?Widget">-><ProgressBar Name="HPBar">
				UI_ENSURE_B(_Process_Template(pTemplNode));
				pTemplNode = pTemplNode->pNext;
			}
		}
		else if (TagName == "WidgetsDefaultAttrib")
		{
		}
		else if (TagName == "SystemWidgets")
		{
			UI_ENSURE_B(g_workspace.getXmlLoader()->_ProcessNode(pChildNode));
			//UI_ENSURE_B(_Process_SystemWidgets(pChildNode));
		}
		else
		{
			RtCoreLog().Error("Invalid Tag Type \'%s\'\n", TagName.c_str());
			//return false;
		}

		pChildNode = pChildNode->pNext;
	}

	return true;
}
// 处理_Process_Template的子元素
bool CUiTheme::_Process_Template(RtsXmlDoc::NodePtr* pNode)
{
	// <ProgressBar  Name="HPBar">
	CUiTemplate* pTempl = RT_NEW CUiTemplate();
	pTempl->SetNode(pNode);
	AddTemplate(pTempl);
	return true;
}

bool CUiTheme::_Process_SystemWidgets(RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		UI_ENSURE_B(g_workspace.getXmlLoader()->_ProcessNode(pChildNode));

		pChildNode = pChildNode->pNext;
	}

	return true;
}



CUiThemeManager::CUiThemeManager()
{
	m_pCurrentTheme = NULL;
}

CUiThemeManager::~CUiThemeManager()
{
}
//lyymark 1.UI.Theme 初始化主题 也就是组件默认材质
bool CUiThemeManager::Init()
{
	CUiTheme* pTheme = RT_NEW CUiTheme();
	// @@@@@
	m_pCurrentTheme = pTheme;
	if (pTheme->LoadFromFile("ui/theme.xml"))
	{
		UI_ENSURE_B(AddTheme(pTheme));
	}
	else 
	{
		RtCoreLog().Error("Invalid theme\n");
	}

	return true;
}

bool CUiThemeManager::Close()
{
	foreach(UiThemeList, iterTheme, m_Themes)
	{
		CUiTheme* pTheme = *iterTheme;
		DEL_ONE(pTheme);
	}
	m_Themes.clear();

	return true;
}

bool CUiThemeManager::AddTheme(CUiTheme* pTheme)
{
	if (FindTheme(pTheme->getName()))
		return false;

	m_Themes.push_back(pTheme);
	return true;
}

bool CUiThemeManager::RemoveTheme(const std::string& ThemeName)
{
	foreach(UiThemeList, iterTheme, m_Themes)
	{
		CUiTheme* pTheme = *iterTheme;
		if (pTheme->getName() == ThemeName)
		{
			m_Themes.erase(iterTheme);
			return true;
		}
	}
	return false;
}

bool CUiThemeManager::RemoveAllTheme()
{
	m_Themes.clear();
	return true;
}

CUiTheme* CUiThemeManager::FindTheme(const std::string& ThemeName)
{
	foreach(UiThemeList, iterTheme, m_Themes)
	{
		CUiTheme* pTheme = *iterTheme;
		if (pTheme->getName() == ThemeName)
		{
			return pTheme;
		}
	}
	return NULL;
}

bool CUiThemeManager::SelectTheme(const std::string& ThemeName)
{
	CUiTheme* pTheme = FindTheme(ThemeName);
	if (pTheme)
	{
		m_pCurrentTheme = pTheme;
		return true;
	}
	else 
	{
		return false;
	}
}

UiThemeList::iterator CUiThemeManager::ThemeBegin()
{
	return m_Themes.begin();
}

UiThemeList::iterator CUiThemeManager::ThemeEnd()
{
	return m_Themes.end();
}


