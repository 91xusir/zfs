#pragma once


class UiForm_WidgetTree : public IUiEventReceiver, public IUiGameWidget
{
public:
	UiForm_WidgetTree();
	~UiForm_WidgetTree();

	void Run(DWORD dwDelta);
    virtual bool Init();
    virtual void Refresh();
    virtual void Show();
    virtual void Hide();
    virtual bool IsVisible();

    RtwTree::Item* InsertToTree_Element(RtwTree::Item* pParentItem, const xmlpp::Element* pElement);
    bool SelectItemByWidget(RtwWidget* pWidget);

    void SetTreeParam_Widget(RtwTree::Item* pTreeItem, RtwWidget* pWidget);
    void SetTreeParam_Element(RtwTree::Item* pTreeItem, const xmlpp::Element* pElement);

    RtwWidget* getTreeParam_Widget(RtwTree::Item* pTreeItem) {/*LOG("");*/ return (RtwWidget*)pTreeItem->Param1; }
    xmlpp::Element* getTreeParam_Element(RtwTree::Item* pTreeItem) {/*LOG("");*/ return (xmlpp::Element*)pTreeItem->Param2; }

    RtwTree::Item* getSelectItem() {/*LOG("");*/ return m_pTre_Widgets->getSelectItem(); }
    RtwTree::Item* getElementTopItem();
	RtwWidget* GetSelectItemWidget()
	{/*LOG("");*/
		return getTreeParam_Widget(getSelectItem());
	}

    std::string GetTreeItemTextFromElement(const xmlpp::Element* pElement);
    std::string GetWidgetTypeNameFromTreeItemText(const std::string& Text);

public:
    RtwForm*        m_pFrm_This;
    RtwTree*        m_pTre_Widgets;
    RtwButton*      m_pBtn_Delete;
    RtwButton*      m_pBtn_Refresh;

private:
    void OnRButtonDown_Tree(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnMenuSelect_Menu(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnSelect_Tree(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_Delete(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnMsgBoxOk_Delete(RtwWidget* pWidget, RtwEventDelegate* pEvent);
    void OnLClick_Refresh(RtwWidget* pWidget, RtwEventDelegate* pEvent);

};

