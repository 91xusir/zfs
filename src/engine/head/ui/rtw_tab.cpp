#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwTab, RtwWidget, 0, "ui")

RtwTab::RtwTab(){}

RtwTab::~RtwTab(){}

void RtwTab::OnBeforeDestroy()
{
	_ResetSelf();

	foreach(std::vector<SItem>, iterTabItem, m_Items)
	{
		SItem& item = *iterTabItem;
		item.pBtn->EvLClick -= RTW_CALLBACK(this, RtwTab, OnLClick_Button);
	}
	m_Items.clear();

	RtwWidget::OnBeforeDestroy();
}

void RtwTab::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwTab::_ResetSelf()
{
	m_WidgetType = wtTabCtrl;
	m_Flag = m_DefaultFlag;
	m_rcClient = RtwRect(0, 0, 30, 10);
	m_rcFrame = RtwRect(0, 0, 30, 10);
	m_TextAlign = SUiTextAlignment(alignCenter, alignCenter);

	m_CurrentSelectItem = -1;
}

bool RtwTab::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	return true;
}

void RtwTab::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);

}

void RtwTab::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool RtwTab::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwTab::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	return true;
}

bool RtwTab::AddTabItem(CUiCheckButton* pBtn, RtwForm* pForm)
{
	UI_ENSURE_B(pBtn && pForm);

	SItem TabItem;
	TabItem.SetButton(pBtn);
	TabItem.SetForm(pForm);

	TabItem.getButton()->SetCheckStyleLayout(false);
	TabItem.getButton()->EvLClick += RTW_CALLBACK(this, RtwTab, OnLClick_Button);
	TabItem.getForm()->Hide();
	TabItem.getForm()->ModifyFlags(0, wfVisible);

	if (!FindChild(TabItem.getButton()->getId()))
		AddChild(TabItem.getButton());
	if (!FindChild(TabItem.getForm()->getId()))
		AddChild(TabItem.getForm());
	m_Items.push_back(TabItem);

	if (m_Items.size() == 1)
	{
		SetCurrentSelect(0);

		RtwEventDelegate Event;
		Event.type = 0;
		EvChangeSelected(pBtn, Event);
	}

	return true;
}

bool RtwTab::RemoveTabItem(int index)
{
	if (index < 0 || index >=(int)m_Items.size())
		return false;

	int counter = 0;
	for (std::vector<SItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		if (counter == index)
		{
			m_Items.erase(iter);

			if (m_CurrentSelectItem == index && m_CurrentSelectItem >= (int)m_Items.size())
			{
				SetCurrentSelect(0);
			}

			return true;
		}
		counter++;
	}

	return false;
}

void RtwTab::SetCurrentSelect(int index)
{
	if (index < 0 || index >=(int)m_Items.size())
		return;

	m_CurrentSelectItem = index;

	for (int i=0; i<(int)m_Items.size(); ++i)
	{
		if (i == m_CurrentSelectItem)
		{
			m_Items[i].getButton()->SetChecked(true);
			m_Items[i].getForm()->Show();
		}
		else 
		{
			m_Items[i].getButton()->SetChecked(false);
			m_Items[i].getForm()->Hide();
		}
	}
}

void RtwTab::SetSelectedItem(int index)
{
	if (index < 0 || index >=(int)m_Items.size())
		return;

	m_CurrentSelectItem = index;

	for (int i=0; i<(int)m_Items.size(); ++i)
	{
		if (i == m_CurrentSelectItem)
		{
			m_Items[i].pBtn->SetChecked(true);
			m_Items[i].pForm->Show();
			RtwEventDelegate e;
			e.type			= weMouseClick;
			e.mouse.button	= buttonLeft;
			EvChangeSelected(m_Items[i].pBtn, e);
		}
		else 
		{
			m_Items[i].pBtn->SetChecked(false);
			m_Items[i].pForm->Hide();
		}
	}
}

const RtwForm* RtwTab::getTabItemForm(int index)
{
	if (index < 0 || index >=(int)m_Items.size())
		return NULL;

	return m_Items[index].getForm();
}

const RtwButton* RtwTab::getTabItemButton(int index)
{
	if (index < 0 || index >=(int)m_Items.size())
		return NULL;

	return m_Items[index].getButton();
}

void RtwTab::OnLClick_Button(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	int i;
	for (i = 0; i < (int)m_Items.size(); ++i)
	{
		if (pWidget == m_Items[i].pBtn)
			break;
	}

	SetCurrentSelect(i);
	pEvent->type = i;
	EvChangeSelected(pWidget, *pEvent);
}

void RtwTab::CalcClient()
{
	m_rcClient = m_rcFrame;
}
void RtwTab::OnRemoveChild(RtwWidget* pWidget)
{
	int counter = 0;
	for (std::vector<SItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		SItem& item = *iter;
		if (item.getButton() == pWidget || item.getForm() == pWidget)
		{
			RemoveTabItem(counter);
			return;
		}

		counter++;
	}
}
