#ifndef _RTUI_HTMLTEXT_H
#define _RTUI_HTMLTEXT_H


namespace ui 
{
	class CUiHtmlText
	{
	public:
		CUiHtmlText();
		virtual ~CUiHtmlText();

	public:
		bool Reset();
		bool LoadFromFile(const std::string& FileName);
		bool LoadFromString(const std::string& strHtml);
		RtsXmlDoc* getXmlDoc() {return m_pXmlDoc;}
		std::list<RtwTextBox::SLine> m_Lines;
	protected:
		RtsXmlDoc* m_pXmlDoc;
		RtwTextBox::SItem m_Itemtmp;
		RtwTextBox::SLine m_Linetmp;
	private:
		bool _ProcessNode(RtsXmlDoc::NodePtr* pNode);
		bool _Process_UiHtmlText(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Lines(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Image(RtsXmlDoc::NodePtr* pNode);
		RtwTextBox::SItem* _Process_ImageItem(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SItem* _Process_Item(RtsXmlDoc::NodePtr *pNode);
		RtwTextBox::SLine* _Process_Line(RtsXmlDoc::NodePtr *pNode);
	};
}//namespace ui

#endif // _RTUI_HIMLTEXT_H