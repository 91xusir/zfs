#ifndef _RTUI_THEME_H
#define _RTUI_THEME_H

namespace ui
{
	enum ESystemWidget
	{
		sw_SystemLayer              = 1,
		sw_DefaultHint,
		sw_DefaultHL,
		sw_AutoSbv,
		sw_AutoSbh,
		sw_PopupMenu,
		sw_MessageBox,
		sw_MessageBox_Ok,
		sw_MessageBox_Cancel,
		sw_MessageBox_Message,
		sw_InputBox,
		sw_InputBox_Ok,
		sw_InputBox_Cancel,
		sw_InputBox_Message,
		sw_InputBox_Edit,
		sw_InputBox_MessageName,
		sw_InputBox_MessageBoxTitle,
		sw_Count
	};

	extern std::string g_SystemWidgetName[];

	class CUiTheme : public RtObject
	{
		RT_DECLARE_DYNCREATE(CUiTheme, RtObject, 0, "ui")

	public:
		CUiTheme();
		virtual ~CUiTheme();

		void Reset();

		bool LoadFromFile(const std::string& FileName);

		bool AddTemplate(CUiTemplate* pTempl);
		bool RemoveTemplate(const std::string& TemplName);
		CUiTemplate* FindTemplate(const std::string& TemplName);

		std::string getName() { return m_Name; }

	private:
		bool _ProcessRoot(RtsXmlDoc::NodePtr* pRoot);

		bool _Process_Theme(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Template(RtsXmlDoc::NodePtr* pNode);
		bool _Process_SystemWidgets(RtsXmlDoc::NodePtr* pNode);

	private:
		std::string         m_Name;
		RtsXmlDoc*          m_pXmlDoc;
		UiTemplateList      m_Templates;
	};

	typedef std::list<CUiTheme*> UiThemeList;

	class CUiThemeManager
	{
	public:
		CUiThemeManager();
		~CUiThemeManager();

		bool Init();
		bool Close();

		bool AddTheme(CUiTheme* pTheme);
		bool RemoveTheme(const std::string& ThemeName);
		bool RemoveAllTheme();
		CUiTheme* FindTheme(const std::string& ThemeName);
		bool SelectTheme(const std::string& ThemeName);

		UiThemeList::iterator ThemeBegin();
		UiThemeList::iterator ThemeEnd();
		CUiTheme* getCurrentTheme() { return m_pCurrentTheme; }

	private:
		UiThemeList     m_Themes;
		CUiTheme*       m_pCurrentTheme;
	};

} // namespace ui


#endif // _RTUI_THEME_H
