#include "studio.h"
using namespace tinyxml2;

UiForm_WidgetTree::UiForm_WidgetTree()
{/*LOG("");*/
	m_pFrm_This		= NULL;
	m_pTre_Widgets	= NULL;
	m_pBtn_Delete	= NULL;
	m_pBtn_Refresh	= NULL;
}

UiForm_WidgetTree::~UiForm_WidgetTree()
{/*LOG("");*/
	RtwTree::Item* pTreeItem = m_pTre_Widgets->getFirstItem();
	while (pTreeItem)
	{
		SetTreeParam_Widget(pTreeItem, NULL);

		pTreeItem = m_pTre_Widgets->getNextItem(pTreeItem);
	}

	DROP_RTUI_OBJECT(m_pFrm_This);
	DROP_RTUI_OBJECT(m_pTre_Widgets);
	DROP_RTUI_OBJECT(m_pBtn_Delete);
	DROP_RTUI_OBJECT(m_pBtn_Refresh);
}

bool UiForm_WidgetTree::Init()
{/*LOG("");*/
	CHECK(g_workspace.FindWidget("layStudio.frmWidgetTree",				(RtwWidget**)&m_pFrm_This));
	CHECK(g_workspace.FindWidget("layStudio.frmWidgetTree.WidgetTree",	(RtwWidget**)&m_pTre_Widgets));
	CHECK(g_workspace.FindWidget("layStudio.frmWidgetTree.btnDel",		(RtwWidget**)&m_pBtn_Delete));
	CHECK(g_workspace.FindWidget("layStudio.frmWidgetTree.btnRefresh",	(RtwWidget**)&m_pBtn_Refresh));

	m_pTre_Widgets->EvMouseRDown	+= RTW_CALLBACK(this, UiForm_WidgetTree, OnRButtonDown_Tree);
	m_pTre_Widgets->EvSelect		+= RTW_CALLBACK(this, UiForm_WidgetTree, OnSelect_Tree);
	m_pBtn_Delete->EvLClick			+= RTW_CALLBACK(this, UiForm_WidgetTree, OnLClick_Delete);
	m_pBtn_Refresh->EvLClick		+= RTW_CALLBACK(this, UiForm_WidgetTree, OnLClick_Refresh);

	return true;
}

void UiForm_WidgetTree::Refresh()
{/*LOG("");*/
    RtwTree::Item* pTreeItem = m_pTre_Widgets->getFirstItem();
    while (pTreeItem)
    {
        SetTreeParam_Widget(pTreeItem, NULL);

        pTreeItem = m_pTre_Widgets->getNextItem(pTreeItem);
    }

    m_pTre_Widgets->DeleteAllItems();
    m_pTre_Widgets->getRootItem();
    InsertToTree_Element(m_pTre_Widgets->getRootItem(), g_pUiMain->m_pLay_Studio->m_UiDocument.getXmlDocument()->RootElement());

    m_pTre_Widgets->Refresh();
}

void UiForm_WidgetTree::Show()
{/*LOG("");*/
    m_pFrm_This->Show();
}

void UiForm_WidgetTree::Hide()
{/*LOG("");*/
	m_pFrm_This->Hide();
}

void UiForm_WidgetTree::Run(DWORD dwDelta)
{/*LOG("");*/
}

bool UiForm_WidgetTree::IsVisible()
{/*LOG("");*/
	return m_pFrm_This->getVisible();
}

void UiForm_WidgetTree::OnRButtonDown_Tree(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	RtwTree::Item* pSelectItem = m_pTre_Widgets->getSelectItem();
	UI_ENSURE(pSelectItem);
	RtwWidget* pOperateWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectItem);
	if (pOperateWidget && pOperateWidget->getWidgetType() != wtLayer)
		::getStudioProcess()->SetOperateWidget(pOperateWidget);
	std::vector<std::string> SupportElements;
	if (pOperateWidget)
	{
		g_pUiMain->m_pLay_Studio->m_UiDocument.getSupportElements(pOperateWidget->getWidgetType(), SupportElements);
	}
    else 
    {
        g_pUiMain->m_pLay_Studio->m_UiDocument.getSupportElements(GetWidgetTypeNameFromTreeItemText(pSelectItem->text), SupportElements);
    }

	RtwPopupMenu* pPopupMenu = g_workspace.getDefaultPopupMenu();
	pPopupMenu->RemoveAll();
	pPopupMenu->EvMenuSelect += RTW_CALLBACK(this, UiForm_WidgetTree, OnMenuSelect_Menu);
	foreach (std::vector<std::string>, iterElement, SupportElements)
	{
		pPopupMenu->AddMenuItem(*iterElement);
	}
	g_workspace.ShowPopupMenu();
}

void UiForm_WidgetTree::OnMenuSelect_Menu(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	RtwPopupMenu* pPopupMenu = (RtwPopupMenu*)pWidget;
	int index = pEvent->menuSelect.index;
	std::string& strOper = pPopupMenu->getMenuText(index);
	RtwTree::Item* pSelectTreeItem = m_pTre_Widgets->getSelectItem();
    g_pUiMain->m_pLay_Studio->m_UiDocument.AddNewElement(strOper, pSelectTreeItem);

	m_pTre_Widgets->Refresh();
}

void UiForm_WidgetTree::OnSelect_Tree(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	RtwTree::Item* pSelectTreeItem = m_pTre_Widgets->getSelectItem();
	RtwWidget* pSelectWidget = g_pUiMain->m_pFrm_WidgetTree->getTreeParam_Widget(pSelectTreeItem);

	if (pSelectWidget && pSelectWidget->getWidgetType() != wtLayer)
	{
		::getStudioProcess()->SetOperateWidget(pSelectWidget);
		g_workspace.SetFocusWidget(pSelectWidget);
	}

	g_pUiMain->m_pFrm_Attributes->Refresh();
}


RtwTree::Item* UiForm_WidgetTree::InsertToTree_Element(RtwTree::Item* pParentItem, const XMLElement* pElement)
{
	UI_ENSURE_P(pParentItem && pElement);

	// 插入新的树节点
	RtwTree::Item* pNewItem = m_pTre_Widgets->InsertItem(GetTreeItemTextFromElement(pElement), "", 0, pParentItem, 0, -1, 0, true, "完成", RtwPixel(0xff00ff00));
	UI_ENSURE_P(pNewItem);

	// 设置树节点的参数为元素指针
	SetTreeParam_Element(pNewItem, pElement);

	RtwWidget* pWidget = nullptr;
	// 如果元素有 "Name" 属性，则尝试获取相关的 Widget 名称
	const char* nameAttribute = pElement->Attribute("Name");
	if (nameAttribute)
	{
		std::string strWidgetFullName = g_pUiMain->m_pLay_Studio->m_UiDocument.getWidgetNameFromElement(pElement);
		if (!strWidgetFullName.empty() && g_workspace.FindWidget(strWidgetFullName, &pWidget))
		{
			g_pUiMain->m_pFrm_WidgetTree->SetTreeParam_Widget(pNewItem, pWidget);
			pWidget->drop(); // 减少 Widget 的引用计数
		}
	}

	// 遍历元素的子节点
	const XMLElement* pNode = pElement->FirstChildElement();
	while (pNode)
	{
		InsertToTree_Element(pNewItem, pNode); // 递归插入子节点
		pNode = pNode->NextSiblingElement();
	}

	return pNewItem;
}

bool UiForm_WidgetTree::SelectItemByWidget(RtwWidget* pWidget)
{/*LOG("");*/
	RtwTree::Item* pItem = m_pTre_Widgets->getFirstItem();
	while (pItem)
	{
		RtwWidget* pTmp = getTreeParam_Widget(pItem); 
		if (pTmp == pWidget)
		{
			m_pTre_Widgets->Select(pItem);
			// 展开
			RtwTree::Item* pTmp = pItem;
			while (pTmp)
			{
				pTmp->bExpanded = true;

				pTmp = pTmp->pParent;
			}
			m_pTre_Widgets->Refresh();
			return true;
		}

		pItem = m_pTre_Widgets->getNextItem(pItem);
	}

	return false;
}

void UiForm_WidgetTree::OnLClick_Delete(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	if (!getSelectItem())
		return;

	RtwMailBox* pMsgBox = g_workspace.getDefaultMessageBox();
	pMsgBox->EvOk += RTW_CALLBACK(this, UiForm_WidgetTree, OnMsgBoxOk_Delete);
	pMsgBox->Show("你确定要删除此节点吗？", "删除节点");
}

void UiForm_WidgetTree::OnLClick_Refresh(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	Refresh();
}

void UiForm_WidgetTree::OnMsgBoxOk_Delete(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
	RtwTree::Item* pSelectItem = getSelectItem();
	//xmlpp::Element* pElement = getTreeParam_Element(pSelectItem);
	XMLElement* pElement = getTreeParam_Element(pSelectItem);

	if (pSelectItem && pElement && std::string(pElement->Name()) != "Ui")
	{
		// 找到父亲Widget的TreeItem
		RtwTree::Item* pTreeItemWithWidget = pSelectItem->pParent;
		RtwWidget* pOperateWidget = getTreeParam_Widget(pTreeItemWithWidget);
		while (!pOperateWidget && pTreeItemWithWidget->pParent)
		{
			pTreeItemWithWidget = pTreeItemWithWidget->pParent;
			pOperateWidget = getTreeParam_Widget(pTreeItemWithWidget);
		}

		// 删除节点
		g_pUiMain->m_pLay_Studio->m_UiDocument.RemoveElement(&pSelectItem);

		// 重新载入
		if (pOperateWidget)
			g_pUiMain->m_pLay_Studio->m_UiDocument.ReloadElement(pTreeItemWithWidget);
	}
	g_pUiMain->m_pLay_Studio->m_UiDocument.SetUndoLevel();
}

void UiForm_WidgetTree::SetTreeParam_Widget(RtwTree::Item* pTreeItem, RtwWidget* pWidget)
{/*LOG("");*/
	CHECK(pTreeItem);

	RtwWidget* pOrigWidget = (RtwWidget*)pTreeItem->Param1;
	DROP_RTUI_OBJECT(pOrigWidget);
    pTreeItem->Param1 = NULL;

	if (pWidget)
	{
		pTreeItem->Param1 = (void*)pWidget;
		pWidget->grab();
	}
}

//void UiForm_WidgetTree::SetTreeParam_Element(RtwTree::Item* pTreeItem, const xmlpp::Element* pElement)
//{/*LOG("");*/
//	CHECK(pTreeItem);
//
//	pTreeItem->Param2 = (void*)pElement;
//}
void UiForm_WidgetTree::SetTreeParam_Element(RtwTree::Item* pTreeItem, const XMLElement* pElement)
{
	assert(pTreeItem);

	pTreeItem->Param2 = (void*)pElement;
}

RtwTree::Item* UiForm_WidgetTree::getElementTopItem()
{/*LOG("");*/
	RtwTree::Item* pItem = m_pTre_Widgets->getRootItem();
	while (pItem)
	{
		XMLElement* pElement = getTreeParam_Element(pItem);

		if (pElement && std::string(pElement->Name()) == "Ui")
			break;

		pItem = pItem->pFirstChild;
	}

	return pItem;
}

std::string UiForm_WidgetTree::GetWidgetTypeNameFromTreeItemText(const std::string& Text)
{/*LOG("");*/
	if (Text.empty())
		return "";

	size_t pos = Text.find_first_of('(');
	if (pos == std::string::npos)
		return Text;
	else 
		return Text.substr(0, pos);
}

//std::string UiForm_WidgetTree::GetTreeItemTextFromElement(const xmlpp::Element* pElement)
//{/*LOG("");*/
//	if (!pElement)
//		return "";
//
//	std::string Text = pElement->get_name();
//	xmlpp::Attribute* pAtt = pElement->get_attribute("Name");
//	if (pAtt)
//	{
//		Text += "(";
//		Text += pAtt->get_value();
//		Text += ")";
//	}
//
//	return Text;
//}
std::string UiForm_WidgetTree::GetTreeItemTextFromElement(const XMLElement* pElement)
{
	if (!pElement)
		return "";

	std::string Text = pElement->Name();

	const char* nameAttribute = pElement->Attribute("Name");
	if (nameAttribute)
	{
		Text += "(";
		Text += nameAttribute;
		Text += ")";
	}

	return Text;
}
