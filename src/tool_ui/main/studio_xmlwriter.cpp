#include "studio.h"

/* attrib
xmlpp::Attribute* SetAttribute(const std::string& AttribName, const std::string& AttribValue, RtwTree::Item* pTreeItem);
void RemoveAttribute(const std::string& AttribName, RtwTree::Item* pTreeItem, RtwWidget* pWidget = NULL);
*/
class SetAttribCmd : public CCommand
{
public:
	explicit SetAttribCmd(
							const std::string&	AttribName,
							const std::string&	AttribValue,
							RtwTree::Item		*pTreeItem
							):m_AttribValue(AttribValue),
							m_AttribName(AttribName), m_pTreeItem(pTreeItem){}
	bool operator()(void)
	{
		if (!IsCanDo())
			return false;

		CUiDocument *uiPtr = &g_pUiMain->m_pLay_Studio->m_UiDocument;
		return (uiPtr->SetAttribute(m_AttribName, m_AttribValue, m_pTreeItem, false) != NULL);
	}

	bool IsCanDo() const 
	{
		return ((m_pTreeItem != NULL) && (!m_AttribName.empty()));
	}
private:
	std::string		m_AttribName;
	std::string		m_AttribValue;
	RtwTree::Item	*m_pTreeItem;
};

CUiDocument::CUiDocument()
{
	//LOG("");
    m_pXmlDoc = NULL;
    m_pXmlDomParser = NULL;
    m_pCopyTreeItem = NULL;
}

CUiDocument::~CUiDocument()
{
	//LOG("");
    DEL_ONE(m_pXmlDomParser);
}

bool CUiDocument::Init()
{
	//LOG("");
    UI_ENSURE_B(!m_pXmlDoc && !m_pXmlDomParser);

    std::string strOrigXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Ui></Ui>";
    m_pXmlDomParser = new xmlpp::DomParser();
    m_pXmlDomParser->parse_memory(strOrigXml);
    m_pXmlDoc = (xmlpp::Document*)m_pXmlDomParser->get_document();

    getUiMain()->m_pFrm_WidgetTree->Refresh();
    getUiMain()->m_pFrm_Attributes->Refresh();

    return true;
}

bool CUiDocument::SaveToFile(const std::string& filename)
{
	//LOG("");
    try
    {
        m_pXmlDoc->write_to_file_formatted(filename);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool CUiDocument::getSupportAttribs(EWidgetType wt, std::vector<std::string>& attribs)
{
	//LOG("");
    UI_ENSURE_B(wt > wtUnknown && wt < wtCount);
    return getSupportAttribs(g_WidgetTypeName[wt], attribs);
}

bool CUiDocument::getSupportElements(EWidgetType wt, std::vector<std::string>& elements)
{
	//LOG("");
    UI_ENSURE_B(wt > wtUnknown && wt < wtCount);
    return getSupportElements(g_WidgetTypeName[wt], elements);
}

bool CUiDocument::getSupportAttribs(const std::string& str, std::vector<std::string>& attribs)
{
	//LOG("");
    static std::vector<std::string> s_arrWidgetAttrib;
    static std::vector<std::string> s_arrImageAttrib;
    if (s_arrWidgetAttrib.empty())
    {
        s_arrWidgetAttrib.push_back("Name");
        s_arrWidgetAttrib.push_back("Inherit");
        s_arrWidgetAttrib.push_back("Location");
        s_arrWidgetAttrib.push_back("Size");
        s_arrWidgetAttrib.push_back("Show");
        s_arrWidgetAttrib.push_back("Move");
        s_arrWidgetAttrib.push_back("Text");
		s_arrWidgetAttrib.push_back("TextFont");
		s_arrWidgetAttrib.push_back("FontSize");
        s_arrWidgetAttrib.push_back("TextColor");
        s_arrWidgetAttrib.push_back("AlignCol");
        s_arrWidgetAttrib.push_back("AlignLine");
        s_arrWidgetAttrib.push_back("Active");
        s_arrWidgetAttrib.push_back("MouseEvent");
        s_arrWidgetAttrib.push_back("MouseMove");
        s_arrWidgetAttrib.push_back("BackgroundColor");
        s_arrWidgetAttrib.push_back("DragOut");
        s_arrWidgetAttrib.push_back("DragIn");
        s_arrWidgetAttrib.push_back("Enable");
		s_arrWidgetAttrib.push_back("BorderCorlor");
        s_arrWidgetAttrib.push_back("BorderSize");

		s_arrWidgetAttrib.push_back("FadeIn");
		s_arrWidgetAttrib.push_back("Disable");
//		    s_arrWidgetAttrib.push_back("Column");
		s_arrWidgetAttrib.push_back("Alignment");
		s_arrWidgetAttrib.push_back("BackgroundTrans");
		s_arrWidgetAttrib.push_back("Anchor");
		s_arrWidgetAttrib.push_back("StartPosition1");
		s_arrWidgetAttrib.push_back("GrabKeyboard");
//		    s_arrWidgetAttrib.push_back("Multiline");
		s_arrWidgetAttrib.push_back("ActivateParent");
		s_arrWidgetAttrib.push_back("PushLike");
		s_arrWidgetAttrib.push_back("TargetID");
		s_arrWidgetAttrib.push_back("Hit");
		s_arrWidgetAttrib.push_back("HitText");
		s_arrWidgetAttrib.push_back("Dock");
		s_arrWidgetAttrib.push_back("MouseHoverImage");
		s_arrWidgetAttrib.push_back("MouseHoverSize");
		s_arrWidgetAttrib.push_back("MouseHoverImgRect");
		s_arrWidgetAttrib.push_back("Penetrate");
//		    s_arrWidgetAttrib.push_back("PushedImage");
//		    s_arrWidgetAttrib.push_back("FaceImage");

		sort(s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    if (s_arrImageAttrib.empty())
    {
        s_arrImageAttrib.push_back("File");
        s_arrImageAttrib.push_back("Source");
        s_arrImageAttrib.push_back("Blend");
        s_arrImageAttrib.push_back("Transparency");
		s_arrImageAttrib.push_back("Border");
		s_arrImageAttrib.push_back("BorderFile");
		s_arrImageAttrib.push_back("BorderSource");
		s_arrImageAttrib.push_back("BorderBlend");
		s_arrImageAttrib.push_back("BorderTransparency");

		sort(s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }

    attribs.clear();

    if (str == "Ui")
    {
    }
    else if (str == "Layer")
    {
        attribs.push_back("Name");
        attribs.push_back("Show");
    }
    else if (str == "Widget")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "Label")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("HyperLink");
    }
    else if (str == "Button")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("Sound");
    }
    else if (str == "Form")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("Movable");
        attribs.push_back("AutoScrollV");
        attribs.push_back("AutoScrollH");
        attribs.push_back("EscapeHide");
    }
    else if (str == "EditBox")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("MultiLine");
        attribs.push_back("WordWrap");
        attribs.push_back("AutoScrollV");
        attribs.push_back("EnableInput");
        attribs.push_back("Password");
        attribs.push_back("PasswordChar");
        attribs.push_back("AutoSizeV");
        attribs.push_back("AutoSizeH");
        attribs.push_back("RichText");
    }
    else if (str == "ScrollBarV")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "ScrollBarH")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "ListBox")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "ProgressBar")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "ComboBox")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("EnableInput");
    }
    else if (str == "TabCtrl")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("SelectTab");
    }
    else if (str == "CheckButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("Sound");
		attribs.push_back("RadioGroupID");
    }
    else if (str == "Tree")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("AutoScrollV");
        attribs.push_back("AutoScrollH");
    }
    else if (str == "PopupMenu")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
		attribs.push_back("Item");
    }
    else if (str == "MediaFrame")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
        attribs.push_back("AutoScrollV");
        attribs.push_back("AutoScrollH");
    }
    else if (str == "HyperLink")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
		attribs.push_back("Link");
    }

    else if (str == "BackgroundImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "Hint")
    {
        attribs.push_back("Form");
        attribs.push_back("Text");
    }
    else if (str == "Image")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "NormalImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "PushedImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "DisabledImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "HighlightImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "CheckedImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "ProgressBarImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "TabItem")
    {
    }
    else if (str == "TitleBar")
    {
        attribs.push_back("Text");
        attribs.push_back("Height");
        attribs.push_back("Show");
    }
    else if (str == "TitleImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "CloseButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "DropdownButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "Editor")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "HandleImage")
    {
        attribs.insert(attribs.end(), s_arrImageAttrib.begin(), s_arrImageAttrib.end());
    }
    else if (str == "TopButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "BottomButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "LeftButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "RightButton")
    {
        attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
    }
    else if (str == "Anchor")
    {
        attribs.push_back("point");
        attribs.push_back("relativeTo");
        attribs.push_back("relativePoint");
        attribs.push_back("offset");
    }
	else if (str == "Column")
	{
		attribs.push_back("Inherit");
		attribs.push_back("Text");
		attribs.push_back("Width");
	}
    else if (str == "Border")
    {
        attribs.push_back("size");
        attribs.push_back("color");
    }
	else if (str == "Item")
	{
		attribs.push_back("Text");
	}
	else if (str == "HtmlView")
	{
		attribs.insert(attribs.end(), s_arrWidgetAttrib.begin(), s_arrWidgetAttrib.end());
	}
	
	sort(attribs.begin(), attribs.end());
	
	// Added by Wayne Wong 2010-12-20
	attribs.erase(std::unique(attribs.begin(), attribs.end()), attribs.end());


    return true;
}

bool CUiDocument::getSupportElements(const std::string& str, std::vector<std::string>& elements)
{
	//LOG("");
    static std::vector<std::string> s_arrWidgetElement;
    if (s_arrWidgetElement.empty())
    {
        s_arrWidgetElement.push_back("Widget");
        s_arrWidgetElement.push_back("Label");
        s_arrWidgetElement.push_back("Button");
        s_arrWidgetElement.push_back("Form");
        s_arrWidgetElement.push_back("EditBox");
        s_arrWidgetElement.push_back("ScrollBarV");
        s_arrWidgetElement.push_back("ScrollBarH");
        s_arrWidgetElement.push_back("ListBox");
        s_arrWidgetElement.push_back("ProgressBar");
        s_arrWidgetElement.push_back("ComboBox");
        s_arrWidgetElement.push_back("TabCtrl");
        s_arrWidgetElement.push_back("CheckButton");
        s_arrWidgetElement.push_back("Tree");
        s_arrWidgetElement.push_back("PopupMenu");
        s_arrWidgetElement.push_back("MediaFrame");
        s_arrWidgetElement.push_back("HyperLink");
        s_arrWidgetElement.push_back("BackgroundImage");
        s_arrWidgetElement.push_back("ImageSequence");
        s_arrWidgetElement.push_back("Hint");
        s_arrWidgetElement.push_back("Anchors");
        s_arrWidgetElement.push_back("Border");
		s_arrWidgetElement.push_back("Column");
		s_arrWidgetElement.push_back("Multiline");
		s_arrWidgetElement.push_back("PushedImage");
		s_arrWidgetElement.push_back("FaceImage");
		s_arrWidgetElement.push_back("HtmlView");
    }
	sort(s_arrWidgetElement.begin(), s_arrWidgetElement.end());

    elements.clear();

    if (str == "Ui")
    {
        elements.push_back("Widget");
        elements.push_back("Label");
        elements.push_back("Button");
        elements.push_back("Form");
        elements.push_back("EditBox");
        elements.push_back("ScrollBarV");
        elements.push_back("ScrollBarH");
        elements.push_back("ListBox");
        elements.push_back("ProgressBar");
        elements.push_back("ComboBox");
        elements.push_back("TabCtrl");
        elements.push_back("CheckButton");
        elements.push_back("Tree");
        elements.push_back("PopupMenu");
        elements.push_back("MediaFrame");
        elements.push_back("HyperLink");
        elements.push_back("Layer");
		elements.push_back("HtmlView");
    }
    else if (str == "Layer")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "Widget")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "Label")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "Button")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "Form")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("TitleBar");
        elements.push_back("CloseButton");
    }
    else if (str == "EditBox")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "ScrollBarV")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("HandleImage");
        elements.push_back("TopButton");
        elements.push_back("BottomButton");
    }
    else if (str == "ScrollBarH")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("HandleImage");
        elements.push_back("LeftButton");
        elements.push_back("RightButton");
    }
    else if (str == "ListBox")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "ProgressBar")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("ProgressBarImage");
    }
    else if (str == "ComboBox")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("DropdownButton");
        elements.push_back("Editor");
    }
    else if (str == "TabCtrl")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("TabItem");
    }
    else if (str == "CheckButton")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
        elements.push_back("CheckedImage");
    }
    else if (str == "Tree")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "PopupMenu")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
		elements.push_back("Item");
    }
    else if (str == "MediaFrame")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }
    else if (str == "HyperLink")
    {
        elements.insert(elements.end(), s_arrWidgetElement.begin(), s_arrWidgetElement.end());
    }

    else if (str == "ImageSequence")
    {
        elements.push_back("Image");
    }
    else if (str == "BackgroundImage")
    {
    }
    else if (str == "Hint")
    {
    }
    else if (str == "NormalImage")
    {
    }
    else if (str == "PushedImage")
    {
    }
    else if (str == "DisabledImage")
    {
    }
    else if (str == "HighlightImage")
    {
    }
    else if (str == "CheckedImage")
    {
    }
    else if (str == "TabItem")
    {
        elements.push_back("CheckButton");
        elements.push_back("Form");
    }
    else if (str == "TitleBar")
    {
        elements.push_back("TitleImage");
    }
    else if (str == "TitleImage")
    {
    }
    else if (str == "CloseButton")
    {
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "DropdownButton")
    {
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "Editor")
    {
        s_arrWidgetElement.push_back("BackgroundImage");
        s_arrWidgetElement.push_back("Border");
    }
    else if (str == "HandleImage")
    {
    }
    else if (str == "TopButton")
    {
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "BottomButton")
    {
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "LeftButton")
    {
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "RightButton")
    {
        elements.push_back("NormalImage");
        elements.push_back("HighlightImage");
        elements.push_back("PushedImage");
        elements.push_back("DisabledImage");
    }
    else if (str == "Anchors")
    {
        elements.push_back("Anchor");
    }

    else 
    {
    }
	sort(elements.begin(), elements.end());

	// Added by Wayne Wong 2010-12-20
	elements.erase(std::unique(elements.begin(), elements.end()), elements.end());

    return true;
}

bool CUiDocument::FindInSupportAttribs(const std::string& NodeName, const std::string& AttribName)
{
	//LOG("");
	std::vector<std::string> arrAttribs;
    getSupportAttribs(NodeName, arrAttribs);
	std::vector<std::string>::iterator iterAttrib;
    for(iterAttrib = arrAttribs.begin(); iterAttrib != arrAttribs.end(); ++iterAttrib)
    {
        if (*iterAttrib == AttribName)
            break;
    }

    return (iterAttrib != arrAttribs.end());
}

bool CUiDocument::FindInSupportElements(const std::string& NodeName, const std::string& ElementName)
{
	//LOG("");
    std::vector<std::string> arrElements;
    getSupportElements(NodeName, arrElements);
	std::vector<std::string>::iterator iterElement;
    for(iterElement = arrElements.begin();iterElement!= arrElements.end(); ++iterElement)
    {
        if (*iterElement == ElementName)
            break;
    }

    return (iterElement != arrElements.end());
}

xmlpp::Element* CUiDocument::AddNewElement(const std::string& NodeName, RtwTree::Item* pTreeParent, RtwTree::Item** ppNewTreeItem/* = NULL*/)
{
	//LOG("");
    RtwWidget* pNewWidget = NULL;

    xmlpp::Element* pParentElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeParent);
    RtwWidget* pParentWidget = NULL;
    CUiLayer* pLayer = NULL;
    {
    RtwTree::Item* pTmpTreeItem = pTreeParent;
    while (pTmpTreeItem)
    {
        RtwWidget* pTmpWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTmpTreeItem);
        if (pTmpWidget)
        {
            if (pTmpWidget->getWidgetType() == wtLayer)
            {
                if (!pLayer)
                    pLayer = (CUiLayer*)pTmpWidget;
            }
            else
            {
                if (!pParentWidget)
                    pParentWidget = pTmpWidget;
            }
        }
        pTmpTreeItem = pTmpTreeItem->pParent;
        }
    }

    EWidgetType wt = g_workspace.getWidgetTypeFromTypeName(NodeName);
    if (wt > wtUnknown)
        pNewWidget = g_workspace.getWidgetFactory()->createWidget(wt);

    if (pNewWidget)
    {
        if (pNewWidget->getWidgetType() == wtLayer)
        {
            g_workspace.AddLayer((CUiLayer*)pNewWidget);
        }
        else
        {
            if (pParentWidget)
            {
                pParentWidget->AddChild(pNewWidget);
            }
            else if (pLayer)
            {
                pLayer->AddWidget(pNewWidget);
            }
            else 
            {
                g_workspace.AddWidget(pNewWidget);
            }
        }
    }

    xmlpp::Element* pNewElement = pParentElement->add_child(NodeName);

    if (pNewWidget)
        pNewElement->set_attribute("Name", pNewWidget->getShortName());

    RtwTree::Item* pNewTreeItem = g_pUiMain->m_pFrm_WidgetTree->InsertToTree_Element(pTreeParent, pNewElement);
    if (ppNewTreeItem)
        *ppNewTreeItem = pNewTreeItem;

    pTreeParent->bExpanded = true;
    pNewTreeItem->bExpanded = true;

    OnCreateElement(NodeName, pNewTreeItem, pNewWidget);

    DROP_RTUI_OBJECT(pNewWidget);

    return pNewElement;
}

void CUiDocument::RemoveElement(RtwTree::Item** ppTreeItem)
{
	//LOG("");
	SetUndoLevel();
    RtwTree::Item* pChildItem = (*ppTreeItem)->pFirstChild;
    while (pChildItem)
    {
        RtwTree::Item* pNextChild = pChildItem->pNext;
        RemoveElement(&pChildItem);
        pChildItem = pNextChild;
    }

    RtwWidget* pWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(*ppTreeItem);
    if (pWidget)
    {
    CHECK(g_workspace.RemoveWidget(pWidget->getId()));
    g_pUiMain->m_pFrm_WidgetTree->SetTreeParam_Widget(*ppTreeItem, NULL);
    }

    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(*ppTreeItem); 
	CHECK(pElement);
    RtwTree::Item* pParentItem = (*ppTreeItem)->pParent;
    CHECK(pParentItem);
    xmlpp::Element* pParentElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pParentItem);
	CHECK(pParentElement);
    pParentElement->remove_child(pElement);

    g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->DeleteItem(ppTreeItem);

    g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->Refresh();
}

xmlpp::Attribute* CUiDocument::SetAttribute(
													const std::string& AttribName, 
													const std::string& AttribValue, 
													RtwTree::Item* pTreeItem,
													bool IsUndoRedo
											)
{
	//LOG("");
    // 检查有效性
    if (AttribName == "Name")
    {
        UI_ENSURE_P(!AttribValue.empty());
    }
    else if (AttribName == "Location")
    {
        RtwRect rc;
        UI_ENSURE_P(rc.FromString(AttribValue));
    }

    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem);
	CHECK(pElement);

	if (IsUndoRedo && pElement)
	{
		xmlpp::Attribute *pAttrib = pElement->get_attribute(AttribName);
		string strAttribVal = "";
		if (pAttrib != NULL)
		{
			strAttribVal = pAttrib->get_value();
		}

		if (strAttribVal != AttribValue)
		{
				AddCommand(new SetAttribCmd(AttribName, strAttribVal, pTreeItem));
		}
	}

    xmlpp::Attribute* pNewAttrib = pElement->set_attribute(AttribName, AttribValue);

    // 找到修改属性的Widget
    RtwTree::Item* pTreeItemWithWidget = pTreeItem;
    RtwWidget* pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItemWithWidget);
    while (!pOperateWidget && pTreeItemWithWidget->pParent)
    {
        pTreeItemWithWidget = pTreeItemWithWidget->pParent;
        pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItemWithWidget);
    }
 	if (
			pOperateWidget 
		&&	pOperateWidget->GetParent()
		&&	pOperateWidget->GetParent()->getWidgetType() == wtForm
		&&	((RtwForm*)pOperateWidget->getParent())->getTitleBarHeight() > 0
	)
 	{
 		pTreeItemWithWidget = pTreeItemWithWidget->pParent;
 		pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItemWithWidget);
 	}
    CHECK(pOperateWidget);
	ReloadElement(pTreeItemWithWidget);

    return pNewAttrib;
}

void CUiDocument::RemoveAttribute(
										const std::string& AttribName,
										RtwTree::Item* pTreeItem,
										RtwWidget* pWidget,
										bool IsUndoRedo
								)
{
	//LOG("");
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem); //(xmlpp::Element*)pTreeItem->Param2;
    if (pElement && !AttribName.empty())
	{
		if (IsUndoRedo)
		{
			std::string strAttribVal = "";
			xmlpp::Attribute *pAttrib = pElement->get_attribute(AttribName);
			if (pAttrib != NULL)
			{
				strAttribVal = pAttrib->get_value();
			}
			AddCommand(new SetAttribCmd(AttribName, strAttribVal, pTreeItem));
		}
    if (pElement)
        pElement->remove_attribute(AttribName);
	}
}

bool CUiDocument::ReloadElement(RtwTree::Item* pTreeItem)
{
	//LOG("");
    // 分Widget相关的TreeItem和不与Widget相关的TreeItem两种情况
    RtwWidget* pWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItem);
    if (!pWidget)
    {
        return false;
    }
    else 
    {
        // 把数据复制到RtXmlNode，借助RtXmlNode重新Load
        RtsXmlDoc::NodePtr* pNodeTmp = NULL;
        g_workspace.getXmlLoader()->_CreateRtXmlNode(&pNodeTmp);
        xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem);
        UI_ENSURE_P(_TranslateFromLibXmlToRtXml(pNodeTmp, pElement));

        // 删除所有儿子孙子Widget，并删除Param1参数
        RtwTree::Item* pTmpTreeItem = pTreeItem->pFirstChild;
        //const RtwTree::Item* pLastItem = pTreeItem->pNext;
        while (pTmpTreeItem && pTmpTreeItem->IsChildOf(pTreeItem))
        {
            RtwWidget* pTmpWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTmpTreeItem);
            if (pTmpWidget)
            {
                /*CHECK(*/g_workspace.RemoveWidget(pTmpWidget->getId())/*)*/;
                g_pUiMain->m_pFrm_WidgetTree->SetTreeParam_Widget(pTmpTreeItem, NULL);
            }

            pTmpTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getNextItem(pTmpTreeItem);
        }

        // 重新载入
        g_workspace.getXmlLoader()->_ProcessAttrib(pWidget, pNodeTmp);
        g_workspace.getXmlLoader()->_ProcessChild(pWidget, NULL, pNodeTmp); // "Inherit"等需要重新LoadChild
        g_workspace.getXmlLoader()->_DeleteRtXmlNode(&pNodeTmp);

        // 存储Param1参数,刷新TreeItem的名字
        pTreeItem->text = g_pUiMain->m_pFrm_WidgetTree->GetTreeItemTextFromElement(pElement);
        pTmpTreeItem = pTreeItem->pFirstChild;
        while (pTmpTreeItem && pTmpTreeItem->IsChildOf(pTreeItem))
        {
            RtwWidget* pTmpWidget = NULL;
            xmlpp::Element* pTmpElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTmpTreeItem);
            std::string strName = getWidgetNameFromElement(pTmpElement);
            if (!strName.empty())
            {
                CHECK(g_workspace.FindWidget(strName, &pTmpWidget));
                g_pUiMain->m_pFrm_WidgetTree->SetTreeParam_Widget(pTmpTreeItem, pTmpWidget);
                pTmpTreeItem->text = g_pUiMain->m_pFrm_WidgetTree->GetTreeItemTextFromElement(pTmpElement);
                DROP_RTUI_OBJECT(pTmpWidget);
            }

            pTmpTreeItem = g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->getNextItem(pTmpTreeItem);
        }
        g_pUiMain->m_pFrm_Attributes->Refresh();

        return true;
    }
}

bool CUiDocument::_TranslateFromRtXmlToLibXml(xmlpp::Element* pLibXmlNode, const RtsXmlDoc::NodePtr* pRtXmlNode, xmlpp::Element* pLibXmlParentNode)
{
	//LOG("");
    UI_ENSURE_B(pLibXmlNode && pRtXmlNode);

    pLibXmlNode->set_name(pRtXmlNode->strName);

    std::map<std::string, std::string>::const_iterator iterAttrib = pRtXmlNode->mapAttribute.begin();
    for (; iterAttrib != pRtXmlNode->mapAttribute.end(); iterAttrib++)
    {
		pLibXmlNode->set_attribute(iterAttrib->first, iterAttrib->second);
    }

    if (pRtXmlNode->pChildren)
    {
        xmlpp::Element* pChildElement = pLibXmlNode->add_child("");
        UI_ENSURE_B(_TranslateFromRtXmlToLibXml(pChildElement, pRtXmlNode->pChildren, pLibXmlNode));
    }
    if (pRtXmlNode->pNext)
    {
        UI_ENSURE_B(pLibXmlParentNode);
        xmlpp::Element* pNextElement = pLibXmlParentNode->add_child("");
        UI_ENSURE_B(_TranslateFromRtXmlToLibXml(pNextElement, pRtXmlNode->pNext, pLibXmlParentNode));
    }

    return true;
}

bool CUiDocument::_TranslateFromLibXmlToRtXml(RtsXmlDoc::NodePtr* pRtXmlNode, xmlpp::Element* pLibXmlNode)
{
	//LOG("");
    UI_ENSURE_B(pLibXmlNode && pRtXmlNode);

    pRtXmlNode->strName = pLibXmlNode->get_name();

    const xmlpp::Element::AttributeList attribList = pLibXmlNode->get_attributes();
    xmlpp::Element::AttributeList::const_iterator iterAttrib = attribList.begin();
    for (; iterAttrib != attribList.end(); iterAttrib++)
    {
        xmlpp::Attribute* pAttrib = *iterAttrib;
        pRtXmlNode->mapAttribute[pAttrib->get_name()] = pAttrib->get_value();
    }

    const xmlpp::Element::NodeList childrenList = pLibXmlNode->get_children();
    xmlpp::Element::NodeList::const_iterator iterChild = childrenList.begin();
    for (; iterChild != childrenList.end(); iterChild++)
    {
        xmlpp::Element* pLibXmlChildNode = (xmlpp::Element*)*iterChild;

        RtsXmlDoc::NodePtr* pRtChildNode = NULL;
        if (!pRtXmlNode->pChildren)
        {
            g_workspace.getXmlLoader()->_CreateRtXmlNode(&pRtChildNode);
            pRtXmlNode->pChildren = pRtChildNode;
        }
        else 
        {
            RtsXmlDoc::NodePtr* pTmp = NULL;
            pTmp = pRtXmlNode->pChildren;
            while (pTmp->pNext)
                pTmp = pTmp->pNext;
            g_workspace.getXmlLoader()->_CreateRtXmlNode(&pRtChildNode);
            pRtChildNode->pParent = pRtXmlNode;
            pTmp->pNext = pRtChildNode;
        }
        UI_ENSURE_B(_TranslateFromLibXmlToRtXml(pRtChildNode, pLibXmlChildNode));
    }

    return true;
}

void CUiDocument::RemoveAllElements()
{
	//LOG("");
    // 删除所有节点
    RtwTree::Item* pTopTreeItem = g_pUiMain->m_pFrm_WidgetTree->getElementTopItem();
    if (pTopTreeItem)
    {
        RtwTree::Item* pChildItem = pTopTreeItem->pFirstChild;
        while (pChildItem)
        {
            RtwTree::Item* pNextChild = pChildItem->pNext;
            RemoveElement(&pChildItem);
            pChildItem = pNextChild;
        }
    }
}

bool CUiDocument::LoadFromFile(const std::string& filename)
{
	//LOG("");
	RemoveAllElements();

    UI_ENSURE_B(AddFromFile(filename));
	SetUndoLevel();

    return true;
}

bool CUiDocument::AddFromFile(const std::string& filename)
{
	//LOG("");
    RtsXmlDoc rtXml;
    UI_ENSURE_B(rtXml.ParserFile(filename.c_str()) == 0);
    UI_ENSURE_B(LoadFromRtXmlDoc(&rtXml));

    return true;
}

bool CUiDocument::LoadFromRtXmlDoc(RtsXmlDoc* pRtXmlDoc)
{
	//LOG("");
    xmlpp::Element* pLibXmlRoot = NULL;

    pLibXmlRoot = m_pXmlDoc->get_root_node();
    UI_ENSURE_B(pLibXmlRoot);

    const RtsXmlDoc::NodePtr* pRtXmlRoot = pRtXmlDoc->GetRoot();
    if (pRtXmlRoot)
    {
        UI_ENSURE_B(_TranslateFromRtXmlToLibXml(pLibXmlRoot, pRtXmlRoot, NULL));
    }

    return true;
}

bool CUiDocument::SaveToRtXmlDoc(RtsXmlDoc* pRtXmlDoc)
{
	//LOG("");
    return true;
}

bool CUiDocument::OnCreateElement(const std::string& ElementName, RtwTree::Item* pTreeItem, RtwWidget* pWidget/* = NULL*/)
{
	//LOG("");
	if (pWidget)
    {
        switch(pWidget->getWidgetType())
        {
        case wtWidget:
            UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));
            break;
        case wtLayer:
            UI_ENSURE_B(_OnCreateElement_Layer(pWidget, pTreeItem));
            break;
        case wtLabel:
            UI_ENSURE_B(_OnCreateElement_Label(pWidget, pTreeItem));
            break;
        case wtButton:
            UI_ENSURE_B(_OnCreateElement_Button(pWidget, pTreeItem));
            break;
        case wtForm:
            UI_ENSURE_B(_OnCreateElement_Form(pWidget, pTreeItem));
            break;
        case wtEditBox:
            UI_ENSURE_B(_OnCreateElement_EditBox(pWidget, pTreeItem));
            break;
        case wtScrollBarV:
            UI_ENSURE_B(_OnCreateElement_ScrollBarV(pWidget, pTreeItem));
            break;
        case wtScrollBarH:
            UI_ENSURE_B(_OnCreateElement_ScrollBarH(pWidget, pTreeItem));
            break;
        case wtListBox:
            UI_ENSURE_B(_OnCreateElement_ListBox(pWidget, pTreeItem));
            break;
        case wtProgressBar:
            UI_ENSURE_B(_OnCreateElement_ProgressBar(pWidget, pTreeItem));
            break;
        case wtComboBox:
            UI_ENSURE_B(_OnCreateElement_ComboBox(pWidget, pTreeItem));
            break;
        case wtTabCtrl:
            UI_ENSURE_B(_OnCreateElement_TabCtrl(pWidget, pTreeItem));
            break;
        case wtCheckButton:
            UI_ENSURE_B(_OnCreateElement_CheckButton(pWidget, pTreeItem));
            break;
        case wtTree:
            UI_ENSURE_B(_OnCreateElement_Tree(pWidget, pTreeItem));
            break;
        case wtPopupMenu:
            UI_ENSURE_B(_OnCreateElement_PopupMenu(pWidget, pTreeItem));
            break;
        case wtMediaFrame:
            UI_ENSURE_B(_OnCreateElement_MediaFrame(pWidget, pTreeItem));
            break;
		case wtHtmlView:
			UI_ENSURE_B(_OnCreateElement_HtmlView(pWidget, pTreeItem));
        default:
            break;
        }
    }
    else
    {
        if (ElementName == "TabItem")
        {
            UI_ENSURE_B(_OnCreateElement_TabItem(pTreeItem));
        }
        else 
        {
        }
    }

    return true;
}

bool CUiDocument::_OnCreateElement_Widget(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    SetAttribute("Name", getDefaultName(pWidget), pTreeItem, false);
    SetAttribute("Location", "100,100,200,200", pTreeItem, false);
    SetAttribute("Inherit", "", pTreeItem, false);
    SetAttribute("Show", "true", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_Layer(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    SetAttribute("Name", getDefaultName(pWidget), pTreeItem, false);
    SetAttribute("Show", "true", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_Label(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,70,18", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_Button(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,100,24", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_Form(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));
	//LOG("");

    SetAttribute("Location", "10,10,300,500", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_EditBox(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,70,18", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_ScrollBarV(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,100,20", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_ScrollBarH(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,10,100", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_ListBox(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,300,500", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_ProgressBar(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
   UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,200,20", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_ComboBox(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,130,18", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_TabCtrl(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,300,500", pTreeItem, false);

    AddNewElement("TabItem", pTreeItem);

    return true;
}

bool CUiDocument::_OnCreateElement_CheckButton(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
   UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,200,20", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_Tree(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,300,500", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_PopupMenu(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,200,300", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_MediaFrame(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,200,300", pTreeItem, false);

    return true;
}

// 			UI_ENSURE_B(_OnCreateElement_HtmlView(pWidget, pTreeItem));
bool CUiDocument::_OnCreateElement_HtmlView(RtwWidget *pWidget, RtwTree::Item *pTreeItem)
{
	UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));
	return true;
}

bool CUiDocument::_OnCreateElement_HyperLink(RtwWidget* pWidget, RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(_OnCreateElement_Widget(pWidget, pTreeItem));

    SetAttribute("Location", "10,10,100,20", pTreeItem, false);

    return true;
}

bool CUiDocument::_OnCreateElement_TabItem(RtwTree::Item* pTreeItem)
{
	//LOG("");
    AddNewElement("CheckButton", pTreeItem);
    AddNewElement("Form", pTreeItem);

    return true;
}

std::string CUiDocument::getDefaultName(RtwWidget* pWidget)
{
	//LOG("");
   return rtFormatNumber(pWidget->getId());
}

std::string CUiDocument::getWidgetNameFromElement(const xmlpp::Element* pElement)
{
	//LOG("");
    if (g_workspace.getWidgetTypeFromTypeName(pElement->get_name()) <= wtUnknown)
        return "";

    // 拆分Path名字
    std::string strXmlPath = pElement->get_path();
    std::vector<std::string> arrPathNames;
    int offset = 1;
    int pos = (int)strXmlPath.find_first_of('/', offset);
    while (pos != std::string::npos)
    {
        std::string str;
        if (!arrPathNames.empty())
        {
            str += arrPathNames.back();
        }
        str += "/";
        str += strXmlPath.substr(offset, pos-offset);
        arrPathNames.push_back(str);
        offset = pos + 1;
        pos = (int)strXmlPath.find_first_of('/', offset);
    }
    if (offset < (int)strXmlPath.size())
    {
        arrPathNames.push_back(strXmlPath);
    }
    // 生成Widget名字
    std::string strWidgetFullName = "";
    xmlpp::Element* pRootElement = g_pUiMain->m_pLay_Studio->m_UiDocument.getXmlDocument()->get_root_node();
    std::vector<std::string>::iterator iterPath = arrPathNames.begin();
    for (; iterPath != arrPathNames.end(); iterPath++)
    {
        std::string strPath = *iterPath;
        xmlpp::NodeSet nodes = pRootElement->find(strPath);
        xmlpp::Element* pNode = (xmlpp::Element*)nodes[0];
        EWidgetType wt = g_workspace.getWidgetTypeFromTypeName(pNode->get_name());
        if (wt > wtUnknown)
        {
            if (!strWidgetFullName.empty())
                strWidgetFullName += ".";
			if (pNode->get_attribute("Name"))
			{
				strWidgetFullName += pNode->get_attribute("Name")->get_value();
			}
        }
    }

    return strWidgetFullName;
}

bool CUiDocument::Copy(RtwTree::Item* pTreeItem)
{
	//LOG("");
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem);
    if (pElement)
    {
        m_strCopyElementPath = pElement->get_path();
        m_pCopyTreeItem = pTreeItem;
        m_bCut = false;
        return true;
    }
    return false;
}

bool CUiDocument::Cut(RtwTree::Item* pTreeItem)
{
	//LOG("");
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem);
    if (pElement)
    {
        m_strCopyElementPath = pElement->get_path();
        m_pCopyTreeItem = pTreeItem;
        m_bCut = true;
        return true;
    }
    return false;
}

bool CUiDocument::Paste(RtwTree::Item* pTreeItem)
{
	//LOG("");
    UI_ENSURE_B(!m_strCopyElementPath.empty());

    xmlpp::NodeSet nodes = m_pXmlDoc->get_root_node()->find(m_strCopyElementPath);
    UI_ENSURE_B(!nodes.empty());

    RtwTree::Item* pSelectTreeItem = g_pUiMain->m_pFrm_WidgetTree->getSelectItem();
    UI_ENSURE_B(pSelectTreeItem);

    xmlpp::Element* pSelectTreeElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pSelectTreeItem);
    UI_ENSURE_B(pSelectTreeElement);
    
    xmlpp::Element* pElement = (xmlpp::Element*)nodes[0];
    // 确保pSelectTreeElement不是pElement，并且不是它的儿子
    std::string strPath1 = pSelectTreeElement->get_path();
    std::string strPath2 = pElement->get_path();
    UI_ENSURE_B(strPath1 != strPath2 && strPath1.find(strPath2) == std::string::npos);

    _Paste(pTreeItem, m_pCopyTreeItem);

    if (m_bCut)
    {
        RemoveElement(&m_pCopyTreeItem);
        m_pCopyTreeItem = NULL;
        m_strCopyElementPath = "";
    }

    // 找到父亲Widget的TreeItem
    RtwTree::Item* pTreeItemWithWidget = pTreeItem->pParent;
    RtwWidget* pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItemWithWidget);
    while (!pOperateWidget && pTreeItemWithWidget->pParent)
    {
        pTreeItemWithWidget = pTreeItemWithWidget->pParent;
        pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItemWithWidget);
    }
    // 重新载入
    if (pOperateWidget)
        ReloadElement(pTreeItemWithWidget);

    g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->Refresh();

    return true;
}

bool CUiDocument::_Paste(RtwTree::Item* pTreeItemTo, RtwTree::Item* pTreeItemFrom)
{
	//LOG("");
    // 检查目录下是否有同名的Widget
    bool bNameExist = false;
    RtwWidget* pWidgetFrom = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pTreeItemFrom);
    xmlpp::Element* pElementFrom = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItemFrom);
    xmlpp::Attribute* pAttribTmp = pElementFrom->get_attribute("Name");
    if (pWidgetFrom && pAttribTmp)
    {
        std::string strNameTmp = pAttribTmp->get_value();
        RtwTree::Item* pChildItemTmp = pTreeItemTo->pFirstChild;
        while (pChildItemTmp)
        {
            RtwWidget* pWidgetTmp = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pChildItemTmp);
            if (pWidgetTmp)
            {
                if (pWidgetTmp->getShortName() == strNameTmp)
                {
                    bNameExist = true;
                    break;
                }
            }
            pChildItemTmp = pChildItemTmp->pNext;
        }
    }

    RtwTree::Item* pNewTreeItem = NULL;
    xmlpp::Element* pNewElement = NULL;
    UI_ENSURE_B(pNewElement = AddNewElement(pElementFrom->get_name(), pTreeItemTo, &pNewTreeItem));

    xmlpp::Element::AttributeList attribList = pElementFrom->get_attributes();
    foreach (xmlpp::Element::AttributeList, iterAttrib, attribList)
    {
        xmlpp::Attribute* pAttrib = *iterAttrib;
        if (bNameExist && pAttrib->get_name() == "Name") // 复制时不复制名字
            continue;

        SetAttribute(pAttrib->get_name(), pAttrib->get_value(), pNewTreeItem);
    }

    //xmlpp::Element::NodeList childrenList = pElementFrom->get_children();
    //foreach (xmlpp::Element::NodeList, iterChild , childrenList)
    //{
    //    xmlpp::Element* pEle = (xmlpp::Element*)(*iterChild);
    //    UI_ENSURE_B(_Paste(pNewTreeItem, pEle));
    //}
    RtwTree::Item* pTreeItemChild = pTreeItemFrom->pFirstChild;
    while (pTreeItemChild)
    {
        UI_ENSURE_B(_Paste(pNewTreeItem, pTreeItemChild));
        pTreeItemChild = pTreeItemChild->pNext;
    }

    return true;
}

bool CUiDocument::MoveUp(RtwTree::Item* pTreeItem)
{
	//LOG("");
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem);
    UI_ENSURE_B(pElement);

    RtwTree::Item* pParentTreeItem = pTreeItem->pParent;
    UI_ENSURE_B(pParentTreeItem);

    xmlpp::Element* pParentElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pParentTreeItem);
    UI_ENSURE_B(pParentElement);

    if (pParentElement->movetoprevious_child(pElement))
    {
        g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->MoveItemToPrevious(pTreeItem);
    }

    return true;
}

bool CUiDocument::MoveDown(RtwTree::Item* pTreeItem)
{
	//LOG("");
    xmlpp::Element* pElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pTreeItem);
    UI_ENSURE_B(pElement);

    RtwTree::Item* pParentTreeItem = pTreeItem->pParent;
    UI_ENSURE_B(pParentTreeItem);

    xmlpp::Element* pParentElement = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Element(pParentTreeItem);
    UI_ENSURE_B(pParentElement);

    if (pParentElement->movetonext_child(pElement))
    {
        g_pUiMain->m_pFrm_WidgetTree->m_pTre_Widgets->MoveItemToNext(pTreeItem);
    }

    return true;
}

