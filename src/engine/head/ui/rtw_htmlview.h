#ifndef _HTML_VIEW_H
#define _HTML_VIEW_H
namespace ui
{


	class RtwHtmlView : public RtwTextBox
	{
		RT_DECLARE_DYNCREATE(RtwHtmlView, RtwWidget, 0, "ui")
	public:
		RtwHtmlView();
		~RtwHtmlView();
		typedef bool (CBTranslate) (string&, const char*, void*);
	public:
		void SetUserData(int data){ m_userdata = data;}
		int	 GetUserData(){return m_userdata;}
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);
		// begin rtui_htmltext.h [3/17/2009 tooth.shi]
	public:
		void Reset();
		bool LoadFromFile(const std::string& FileName);
		bool LoadFromString(const std::string& strHtml);
		RtsXmlDoc* getXmlDoc() {return m_pXmlDoc;}
//		std::list<RtwTextBox::SLine> m_Lines;

		// add [3/20/2009 tooth.shi]
		void SetText(const std::string text);
		std::string GetPath(){ return m_path; }
		void AddPath(char* path);
		void ClearPath(){	m_path = "";	}

	protected:
		RtsXmlDoc* m_pXmlDoc;
		RtwTextBox::SItem m_Itemtmp;
		RtwTextBox::SLine m_Linetmp;
	private:
		bool _ProcessNode(RtsXmlDoc::NodePtr* pNode);
		bool _Process_UiHtmlText(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Lines(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Image(RtsXmlDoc::NodePtr* pNode);
		RtwTextBox::SItem* _Process_Item(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SItem* _Process_ChildItem(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SItem* _Process_ImageItem(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SLine* _Process_Line(RtsXmlDoc::NodePtr *pNode);
		void ReplaceString(std::string& replace);
		std::string ReadFileToString(const std::string& FileName);
		// end rtui_htmltext.h [3/17/2009 tooth.shi]
	protected:
	private:
		int m_userdata;
		std::string m_path;
		std::string m_ImageItempath;
	public:
		static		void SetTranslateText(CBTranslate* cb);
		static		CBTranslate* ms_cbTranslateFunc;

		RtwEventDispatcher EvAction;

	};
}

#endif