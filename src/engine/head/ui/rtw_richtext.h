//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_RICHTEXT_H
#define _RTUI_RICHTEXT_H


namespace ui {

	class CUiRichText
	{
	public:
		CUiRichText();
		virtual ~CUiRichText();

	public:
		bool Reset();
		bool LoadFromFile(const std::string& FileName);
		bool LoadFromString(const std::string& strXml);

		RtsXmlDoc* getXmlDoc() { return m_pXmlDoc; }

		std::list<RtwTextBox::SLine> m_Lines;

	protected:
		RtsXmlDoc* m_pXmlDoc;

		RtwTextBox::SLine m_LineTmp;
		RtwTextBox::SItem m_ItemTmp;

	private:
		bool _ProcessNode(RtsXmlDoc::NodePtr* pNode);

		bool _Process_UiRichText(RtsXmlDoc::NodePtr* pNode);
		bool _Process_Lines(RtsXmlDoc::NodePtr* pNode);
		RtwTextBox::SLine* _Process_Line(RtsXmlDoc::NodePtr* pNode);
		RtwTextBox::SItem* _Process_Item(RtsXmlDoc::NodePtr* pNode);

		bool _Process_Image(RtwTextBox::SItem* pItem, RtsXmlDoc::NodePtr* pNode);
	};

} // namespace ui


#endif // _RTUI_RICHTEXT_H
