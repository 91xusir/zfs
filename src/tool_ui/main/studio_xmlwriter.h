#pragma once

//#include <libxml++/libxml++.h>
#include "tinyxml2.h"
#include "CURDoInterface.h"

class CUiDocument : public CURDoInterface
{
public:
	CUiDocument();
	~CUiDocument();

public:
	bool Init();

	bool IsXmlFile(const string& strFileName)
	{
		if (strFileName.empty())
		{
			return false;
		}
		FILE* fp;
		fp = fopen(strFileName.c_str(), "r+");
		char buffer[1024] = "\0";
		fread(buffer, 1024, 1, fp);
		fclose(fp);
		if (string(buffer).find("<?xml version=\"1.0\"") != string::npos)
			return true;
		return false;
	}

	bool LoadFromFile(const std::string& filename); // Without Create Widgets
	bool AddFromFile(const std::string& filename); // Without Create Widgets
	bool SaveToFile(const std::string& filename);

	bool getSupportAttribs(EWidgetType wt, std::vector<std::string>& attribs);
	bool getSupportAttribs(const std::string& str, std::vector<std::string>& attribs);
	bool getSupportElements(EWidgetType wt, std::vector<std::string>& elements);
	bool getSupportElements(const std::string& str, std::vector<std::string>& elements);
	bool FindInSupportAttribs(const std::string& NodeName, const std::string& AttribName);
	bool FindInSupportElements(const std::string& NodeName, const std::string& ElementName);

	bool OnCreateElement(const std::string& ElementName, RtwTree::Item* pTreeItem, RtwWidget* pWidget = NULL);

	// ����widget�����뵽UIϵͳ����ӵ�XmlDoc����ӵ�UiTree
	tinyxml2::XMLElement* AddNewElement(
		const std::string& NodeName,
		RtwTree::Item* pTreeParent,
		RtwTree::Item** ppNewTreeItem = NULL
	);

	void RemoveElement(RtwTree::Item** ppTreeItem);
	bool ReloadElement(RtwTree::Item* pTreeItem);
	void RemoveAllElements();

	// attrib
	bool  SetAttribute(
		const std::string& AttribName,
		const std::string& AttribValue,
		RtwTree::Item* pTreeItem,
		bool IsUndoRedo = true
	);
	void RemoveAttribute(
		const std::string& AttribName,
		RtwTree::Item* pTreeItem,
		RtwWidget* pWidget = NULL,
		bool IsUndoRedo = true
	);

	// ��RtXml֮���ת��
	bool LoadFromRtXmlDoc(RtsXmlDoc* pRtXmlDoc);
	bool SaveToRtXmlDoc(RtsXmlDoc* pRtXmlDoc);

	// ����ճ��
	bool Copy(RtwTree::Item* pTreeItem);
	bool Cut(RtwTree::Item* pTreeItem);
	bool Paste(RtwTree::Item* pTreeItem);
	bool MoveUp(RtwTree::Item* pTreeItem);
	bool MoveDown(RtwTree::Item* pTreeItem);

	std::string getWidgetNameFromElement(const tinyxml2::XMLElement* pElement);
private:
	bool _OnCreateElement_Widget(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_Layer(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_Label(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_Button(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_Form(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_EditBox(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_ScrollBarV(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_ScrollBarH(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_ListBox(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_ProgressBar(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_ComboBox(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_TabCtrl(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_CheckButton(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_Tree(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_PopupMenu(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_MediaFrame(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_HyperLink(RtwWidget* pWidget, RtwTree::Item* pTreeItem);
	bool _OnCreateElement_TabItem(RtwTree::Item* pTreeItem);
	bool _OnCreateElement_HtmlView(RtwWidget* pWidget, RtwTree::Item* pTreeItem);

	bool _TranslateFromRtXmlToLibXml(tinyxml2::XMLElement* pTinyXmlNode, const RtsXmlDoc::NodePtr* pRtXmlNode, tinyxml2::XMLElement* pTinyXmlParentNode);
	bool _TranslateFromLibXmlToRtXml(RtsXmlDoc::NodePtr* pRtXmlNode, tinyxml2::XMLElement* pTinyXmlNode);



	bool _Paste(RtwTree::Item* pTreeItemTo, RtwTree::Item* pTreeItemFrom);

	std::string getDefaultName(RtwWidget* pWidget);
	std::string CUiDocument::GetElementPath(const tinyxml2::XMLElement* element);

public:
	const tinyxml2::XMLDocument* getXmlDocument() const { /*LOG("");*/ return m_pXmlDoc; }
private:
	tinyxml2::XMLDocument* m_pXmlDoc;
	tinyxml2::XMLDocument* m_pXmlDomParser;

	bool                m_bCut;                         // �Ƿ�Ϊ����
	std::string         m_strCopyElementPath;           // ����ָ���Element
	RtwTree::Item* m_pCopyTreeItem;                // ���Ƶ�TreeItem(ֻ��Element��Чʱ�ſ���ʹ�ô�ָ�� )
};

