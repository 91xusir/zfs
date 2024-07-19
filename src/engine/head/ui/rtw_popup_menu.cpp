#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwPopupMenu, RtwWidget, 0, "ui")

RtwPopupMenu::RtwPopupMenu():m_pList(NULL){}

RtwPopupMenu::~RtwPopupMenu(){}

void RtwPopupMenu::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pList);

	RtwWidget::OnBeforeDestroy();
}

void RtwPopupMenu::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwPopupMenu::_ResetSelf()
{
	m_lstDisableList.clear();
	m_WidgetType	= wtPopupMenu;
	m_Flag			= m_DefaultFlag;
	m_TextAlign		= SUiTextAlignment(alignNear, alignNear);
	m_BorderSize	= 1;
	m_BorderColor	= RtwPixel(0xff99aa33);
	m_MenuTextColIndex = 0;
}

bool RtwPopupMenu::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	m_pList = (RtwListBox*)g_workspace.getWidgetFactory()->createWidget(wtListBox);
	UI_ENSURE_B(m_pList);
	AddChild(m_pList);
	m_pList->SetAutoResize(false, false);
	m_pList->SetDrawHeader(false, false);
	m_pList->SetDrawScrollBarV(false, false);
	m_pList->SetHoverSelect(true);
	m_pList->InsertColumn("MenuText", m_DefaultWidth);
	EvHide += RTW_CALLBACK(this, RtwPopupMenu, OnHide_This);
	m_pList->EvSelect += RTW_CALLBACK(this, RtwPopupMenu, OnLClick_List);
	m_pList->SetbScroll(false);

	return true;
}

void RtwPopupMenu::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	for (std::list<int>::iterator slii = m_lstDisableList.begin(); slii!=m_lstDisableList.end(); ++slii)
	{
		RtwListBox::SLine *pLine = m_pList->GetLine(*slii);
		pLine->bDraw = false;
	}
	RtwWidget::DrawForeground(pClipRect);
}

void RtwPopupMenu::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

void RtwPopupMenu::EnableIndex(const std::string &itemName, bool bEnable /* = true */)
{
	EnableIndex(FindMenuItemIndex(itemName), bEnable);
}

void RtwPopupMenu::EnableIndex(int index, bool bEnable)
{
	UI_CHECK(m_pList->getLineCount() == (int)m_lstEnable.size());
	UI_CHECK(m_pList->getLineCount() >= index);

	RtwListBox::SLine *pLine = m_pList->GetLine(index);
	if (!pLine)
	{
		return;
	}

	RtwPixel popColor = RtwPixel(0xffffffff);

	
	std::list<int>::iterator sli = find(m_lstDisableList.begin(), m_lstDisableList.end(), index);

	if (sli != m_lstDisableList.end())
	{
		m_lstDisableList.erase(sli);
	}
	m_lstDisableList.swap(m_lstDisableList);

	if (!bEnable)
	{
		popColor = RtwPixel(0xffababab);
		m_lstDisableList.push_back(index);
	}

	if (pLine->items.size() > 1)
	{
		pLine->items[2].pWidget->SetTextColor(popColor);
	}
	else if (pLine->items.size() >0)
	{
		pLine->items[0].pWidget->SetTextColor(popColor);

	}
}

bool RtwPopupMenu::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwPopupMenu::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	return true;
}

bool RtwPopupMenu::AddMenuItem(const std::string& text, bool bEnable/* = true*/, void* param/* = NULL*/, const std::string& type /*= ""*/)
{
	UI_CHECK_B(m_pList->getLineCount() == (int)m_lstEnable.size());

	m_pList->InsertLine(text, RtwPixel(0xffffffff), -1, true, param);
	m_lstEnable.push_back(bEnable);
	m_itemString[m_pList->getLineCount()-1]=type;

	EnableIndex(GetMenuItemsCount()-1, bEnable);

	CalcClient();

	return true;
}

int RtwPopupMenu::FindMenuItemIndex(const std::string& text)
{
	UI_CHECK_B(m_pList->getLineCount() == (int)m_lstEnable.size());

	for (int i = 0; i < m_pList->getLineCount(); ++i)
	{
		if (m_pList->getLineItemText(i, m_MenuTextColIndex) == text)
			return i;
	}

	return -1;
}

bool RtwPopupMenu::RemoveMenuItem(const std::string& text)
{
	UI_CHECK_B(m_pList->getLineCount() == (int)m_lstEnable.size());

	return RemoveMenuItem(FindMenuItemIndex(text));
}

bool RtwPopupMenu::RemoveMenuItem(int index)
{
	if (index < 0 || index >= m_pList->getLineCount())
		return false;

	m_pList->RemoveLine(index);
	m_itemString.erase(index);

	std::list<bool>::iterator iter = m_lstEnable.begin();
	for (int i = 0; i < index; ++i)
		iter ++;
	m_lstEnable.erase(iter);

	return true;
}

void RtwPopupMenu::RemoveAllItems()
{
	m_pList->RemoveAllLines(false);
	m_itemString.clear();
	m_lstEnable.clear();
}
void RtwPopupMenu::RemoveAll()
{
	m_pList->RemoveAllLines(false);
	m_lstEnable.clear();
	EvMenuSelect.Reset();
	m_itemString.clear();

	Refresh();
}

void RtwPopupMenu::CalcClient()
{
	int height = 8 /*4 * 2*/;
	if (m_pList->getLineCount() != 0)
	{
		height += m_pList->getLineCount() * m_pList->GetItemHeight(0);
	}
	int width = 100;
	m_rcFrame.right = m_rcFrame.left + width;
	m_rcFrame.bottom = m_rcFrame.top + height;
	m_rcClient = m_rcFrame;

	m_pList->Move(m_rcClient, true);
}

void RtwPopupMenu::OnHide_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
}

void RtwPopupMenu::OnLClick_List(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	int index = m_pList->GetSelectedItem();

	//±»½ûÓÃ
	if (find(m_lstDisableList.begin(), m_lstDisableList.end(), index) != m_lstDisableList.end())
	{
		return;
	}
	if (index >= 0)
	{
		RtwEventDelegate e(etMenuSelect);
		e.menuSelect.index = index;
		EvMenuSelect(this, e);
	}
	Hide();
}

std::string RtwPopupMenu::getMenuText(int index)
{
	return m_pList->getLineItemText(index, m_MenuTextColIndex);
}

void RtwPopupMenu::OnRemoveChild(RtwWidget* pWidget)
{
	if (pWidget == m_pList)
		DROP_RTUI_OBJECT(m_pList);
}

// add [3/17/2009 tooth.shi]

void RtwPopupMenu::AddItem(const std::string& text,void* lparam /* = NULL */, void* param /* = NULL */, const std::string& type/* "" */)
{
	AddMenuItem(text,true,lparam,type);
}

int RtwPopupMenu::GetLastSelectItem()
{
	return m_pList->getSelectLine();
}

int RtwPopupMenu::GetItemData(int select)
{
	return select + 1;
}

const string INVITE_COMMAND("invite");
const string LEAVE_COMMAND("leave");
const string BANISH_COMMAND("divid");
const string DISMISS_COMMAND("dismiss");

std::string RtwPopupMenu::GetItemUserString(int select)
{
	return m_itemString[select];
}