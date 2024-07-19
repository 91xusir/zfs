#include "ui/rtw_ui.h"

CUiRichText::CUiRichText()
{
	m_pXmlDoc = RT_NEW RtsXmlDoc();
}

CUiRichText::~CUiRichText()
{
	DEL_ONE(m_pXmlDoc);
}

bool CUiRichText::Reset()
{
	if (m_pXmlDoc)
		m_pXmlDoc->Cleanup();
	return true;
}

bool CUiRichText::LoadFromFile(const std::string& FileName)
{
	Reset();

	if (!m_pXmlDoc->ParserFile(FileName.c_str()) == 0)
	{
		RtCoreLog().Error("[CUiRichText::LoadFromFile] Parse file \'%s\'error\n", FileName.c_str());
		return false;
	}
	if (!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
	{
		RtCoreLog().Error("[CUiRichText::LoadFromFile] Process file \'%s\'error\n", FileName.c_str());
		return false;
	}

	return true;
}

bool CUiRichText::LoadFromString(const std::string& strXml)
{
	Reset();

	if (m_pXmlDoc->ParserString(strXml.c_str()))
	{
		return false;
	}

	if (!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
		return false;

	Reset();

	return true;
}

bool CUiRichText::_ProcessNode(RtsXmlDoc::NodePtr* pNode)
{
	if (!pNode)
		return true;

	const std::string& TagName = pNode->strName;
	if (TagName == "UiRichText")
	{
		UI_ENSURE_B(_Process_UiRichText(pNode))
	}
	else
	{
	}

	return true;
}

bool CUiRichText::_Process_UiRichText(RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Lines")
			UI_ENSURE_B(_Process_Lines(pChildNode))

			pChildNode = pChildNode->pNext;
	}

	return true;
}

bool CUiRichText::_Process_Lines(RtsXmlDoc::NodePtr* pNode)
{
	m_Lines.clear();

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Line")
		{
			RtwTextBox::SLine* pLine = _Process_Line(pChildNode);
			if (pLine)
			{
				m_Lines.push_back(*pLine);
			}
		}

		pChildNode = pChildNode->pNext;
	}

	return true;
}

RtwTextBox::SLine* CUiRichText::_Process_Line(RtsXmlDoc::NodePtr* pNode)
{
	m_LineTmp.Reset();

	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Item")
		{
			RtwTextBox::SItem* pItem = _Process_Item(pChildNode);
			if (pItem)
			{
				m_LineTmp.lineitems.push_back(*pItem);
			}
		}

		pChildNode = pChildNode->pNext;
	}

	return &m_LineTmp;
}

RtwTextBox::SItem* CUiRichText::_Process_Item(RtsXmlDoc::NodePtr* pNode)
{
	m_ItemTmp.Reset();

	std::string strType = pNode->GetProp_s("type");
	if (strType == "TEXT")
	{
		std::string strText = pNode->GetProp_s("text");
		std::string strColor = pNode->GetProp_s("color");
		std::string strUnderline = pNode->GetProp_s("underline");
		std::string strHLink = pNode->GetProp_s("hlink");
		m_ItemTmp.type = RtwTextBox::eit_Text;
		m_ItemTmp.text = strText;
		if (strColor.size() > 1)
			m_ItemTmp.color = strtoul(strColor.c_str() + 1, NULL, 16);
		if (!strUnderline.empty())
		{
			if (strUnderline == "true")
				m_ItemTmp.bUnderLine = true;
			else if (strUnderline == "false")
				m_ItemTmp.bUnderLine = false;
		}
		if (!strHLink.empty())
		{
			m_ItemTmp.hyperLink.FromString(strHLink);
		}
	}
	else if (strType == "IMAGE")
	{
		RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
		while (pChildNode)
		{
			if (pChildNode->strName == "Image")
			{
				UI_ENSURE_P(_Process_Image(&m_ItemTmp, pChildNode))
			}

			pChildNode = pChildNode->pNext;
		}
	}
	else 
	{
	}

	return &m_ItemTmp;
}

bool CUiRichText::_Process_Image(RtwTextBox::SItem* pItem, RtsXmlDoc::NodePtr* pNode)
{
	std::string strFile = pNode->GetProp_s("File");
	std::string strSource = pNode->GetProp_s("Source");
	std::string strBlend = pNode->GetProp_s("Blend");
	std::string strTransparency = pNode->GetProp_s("Transparency");

	bool bBlend = false;
	if (strBlend == "true")
		bBlend = true;
	else if (strBlend == "false")
		bBlend = false;

	Real transparency = 0;
	if (!strTransparency.empty())
		transparency = atof(strTransparency.c_str());

	RtwImage* pImage = g_workspace.getImageFactory()->createImage(strFile);
	if (pImage)
	{
		RtwRect rcSource;
		if (!strSource.empty())
		{
			if (rcSource.FromString(strSource))
			{
				pImage->SetSourceRect(rcSource);//modify by fox
			}
		}
		pImage->SetBlend(bBlend);
		pImage->SetTransparency(transparency);

		pItem->type = RtwTextBox::eit_Image;
		pItem->SetImage(pImage);
		DROP_TRUI_IMAGE(pImage);
	}

	return true;
}