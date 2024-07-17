#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwComboBox, RtwWidget, 0, "ui")

RtwComboBox::RtwComboBox():m_pEdtText(NULL), m_pBtnArrow(NULL), m_pLstDropDown(NULL), m_NorImage(NULL){}

RtwComboBox::~RtwComboBox(){}

void RtwComboBox::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pEdtText);
	DROP_RTUI_OBJECT(m_pBtnArrow);
	DROP_RTUI_OBJECT(m_pLstDropDown);

	RtwWidget::OnBeforeDestroy();
}

void RtwComboBox::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwComboBox::_ResetSelf()
{
	m_WidgetType = wtComboBox;
	m_Flag = m_DefaultFlag;
	m_rcClient = RtwRect(0, 0, 30, 10);
	m_rcFrame = RtwRect(0, 0, 30, 10);
	m_TextAlign = SUiTextAlignment(alignCenter, alignCenter);
	m_bDrop = false;
	m_bEnableInput = false;
}

void RtwComboBox::HideDrop()
{
	m_bDrop = true;
	SwapDropDownState();
}

void RtwComboBox::SwapDropDownState()
{
	m_bDrop = !m_bDrop;

	int ListHeight = 0;

	if (m_bDrop)
	{
		g_workspace.AddTopRenderWidget(m_pLstDropDown);
	}
	else 
	{
		g_workspace.RemoveTopRenderWidget(m_pLstDropDown->getId());
	}
	CalcClient();
	SetAutoScrollH( m_bAutoScrollH );
	SetAutoScrollV( m_bAutoScrollV );
}

void RtwComboBox::OnClick_Arrow(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	SwapDropDownState();

	if (m_bDrop)
		g_workspace.SetFocusWidget(m_pLstDropDown);

	if (m_bDrop)
	{
		EvDropDown(this, etDropDown);
	}
}

void RtwComboBox::OnMouseLDown_List(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	int line = m_pLstDropDown->GetLineFromPosition(pEvent->mouse.y);
	if (line >= 0)
	{
		SetText(m_pLstDropDown->getLineItemText(line, 0));
		if (m_bDrop)
		{
			SwapDropDownState();
		}
		m_pLstDropDown->SetSelectedItem(line);
		EvUpdateText(this, weUpdateText);
		EvSelect(this, etSelect);
	}
}

bool RtwComboBox::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	m_pEdtText = (RtwTextBox*)g_workspace.getWidgetFactory()->createWidget(wtEditBox);
	UI_ENSURE_B(m_pEdtText);
	m_pEdtText->SetMultiLine(false);
	m_bEnableInput ? m_pEdtText->Enable() : m_pEdtText->Disable();
	AddChild(m_pEdtText);

	m_pBtnArrow = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	UI_ENSURE_B(m_pBtnArrow);
	m_pBtnArrow->EvLClick += RTW_CALLBACK(this, RtwComboBox, OnClick_Arrow);
	AddChild(m_pBtnArrow);

	m_pLstDropDown = (RtwListBox*)g_workspace.getWidgetFactory()->createWidget(wtListBox);
	UI_ENSURE_B(m_pLstDropDown);
	m_pLstDropDown->ModifyFlags(wfGrabMouse|wfMouseMove|wfPick, 0);
	m_pLstDropDown->SetBackgroundColor(RtwPixel(0xaa000000));
	m_pLstDropDown->SetBorderColor(RtwPixel(0xff94825a));
	m_pLstDropDown->SetBorderSize(3);
	m_pLstDropDown->SetDrawHeader(false, false);
	m_pLstDropDown->SetDrawScrollBarV(false, false);
	m_pLstDropDown->SetAutoResize(true, false);
	m_pLstDropDown->SetHoverSelect(true);
	m_pLstDropDown->InsertColumn("DropText", 1);
	m_pLstDropDown->EvMouseLDown += RTW_CALLBACK(this, RtwComboBox, OnMouseLDown_List);
	AddChild(m_pLstDropDown);

	m_pLstDropDown->EvUnFocus += RTW_CALLBACK(this, RtwComboBox, OnUnFocus_List);
	return true;
}

void RtwComboBox::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	if (!m_Text.empty())
		g_workspace.getFontManager()->DrawString(m_Text, m_rcClient,SETCOLORALPHA(m_TextColor, m_Transparency), false,
		m_TextAlign, false, pClipRect);

	RtwWidget::DrawForeground(pClipRect);
}

void RtwComboBox::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool RtwComboBox::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwComboBox::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwComboBox* pOther = (RtwComboBox*)pWidget;

	m_bEnableInput = pOther->m_bEnableInput;
	m_bDrop = pOther->m_bDrop;
	m_pEdtText->CloneFrom(pOther->m_pEdtText);
	m_pBtnArrow->CloneFrom(pOther->m_pBtnArrow);
	m_pLstDropDown->CloneFrom(pOther->m_pLstDropDown);
	m_rcText = pOther->m_rcText;
	m_rcArrow = pOther->m_rcArrow;
	m_rcList = pOther->m_rcList;

	return true;
}

void RtwComboBox::SetText(const std::string& text)
{
	if (m_pEdtText)
		m_pEdtText->SetText(text);
}

void RtwComboBox::SetEnableInput(bool bEnableInput)
{
	m_bEnableInput = bEnableInput;
	if (m_bEnableInput)
	{
		m_pEdtText->Enable();
		m_pEdtText->SetEnableInput(true);
	}
	else 
	{
		m_pEdtText->Disable();
		m_pEdtText->SetEnableInput(false);
	}
}

void RtwComboBox::AddItem(const std::string& text)
{
	m_pLstDropDown->InsertLine(text);
	CalcClient();
}

void RtwComboBox::RemoveAllItems()
{
	m_pLstDropDown->RemoveAllLines();
}

std::string RtwComboBox::GetText() const 
{
	return m_pEdtText ? m_pEdtText->GetText() : "";
}

std::string RtwComboBox::getText() const 
{
	return m_pEdtText ? m_pEdtText->GetText() : "";
}


void RtwComboBox::CalcClient()
{
	if (!m_pLstDropDown)
		return;

	int ListHeight = 0;
	for (int i=0; i<(int)m_pLstDropDown->getLineCount(); ++i)
	{
		ListHeight += m_pLstDropDown->getLineHeight(i);
	}

	if (m_bDrop)
	{
	    m_rcClient = m_rcFrame;
	}
	else 
	{
		m_rcClient = m_rcFrame;
	}

	if (m_pEdtText)
	{
		m_rcText.left = 0;
		m_rcText.top = 0;
		m_rcText.right = m_rcClient.right - m_rcClient.left - m_rcClient.getHeight();
		m_rcText.bottom = m_rcClient.bottom - m_rcClient.top;
		m_pEdtText->Move(m_rcText);
	}

	if (m_pBtnArrow)
	{
		m_rcArrow.left = m_rcText.right + 2;
		m_rcArrow.top = m_rcText.top;
		m_rcArrow.right = m_rcClient.right - m_rcClient.left - 4;
		m_rcArrow.bottom = m_rcText.bottom;
		m_pBtnArrow->Move(m_rcArrow);
	}

	if (m_pLstDropDown)
	{
		if( m_pAutoScrollV )
		{
			m_pLstDropDown->SetDrawScrollBarV( true );
		}
		if (m_bDrop)
		{
			m_pLstDropDown->Show();

			m_rcList.left = m_rcText.left;
			m_rcList.right = m_rcArrow.right;
			m_rcList.top = m_rcText.bottom/*m_rcClient.bottom - m_rcClient.top + 1*/;
			m_rcList.bottom = m_rcText.bottom + 40/*+ ListHeightm_rcFrame.bottom - m_rcFrame.top*/;
			m_pLstDropDown->Move(m_rcList);
			m_pLstDropDown->SetColumnWidth(0, m_rcList.getWidth());
		}
		else 
		{
			m_pLstDropDown->Hide();
		}
	}
	if(m_NorImage)
		m_pBtnArrow->SetNormalImage(m_NorImage);
}

void RtwComboBox::OnUnFocus_List(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (m_bDrop)
		SwapDropDownState();
}
void RtwComboBox::OnRemoveChild(RtwWidget* pWidget)
{
	if (pWidget == m_pEdtText)
		DROP_RTUI_OBJECT(m_pEdtText)
	else if (pWidget == m_pBtnArrow)
	DROP_RTUI_OBJECT(m_pBtnArrow)
	else if (pWidget == m_pLstDropDown)
	DROP_RTUI_OBJECT(m_pLstDropDown)
}

int RtwComboBox::GetSelectedItem()
{
	return getDropList()->getLineItemIndex(m_pEdtText->getText());
}

void RtwComboBox::SetSelectedItem(int index)
{
	m_pLstDropDown->ClearSelectLine();
	m_pLstDropDown->SetSelectedItem(index);
	SetText(m_pLstDropDown->getLineItemText(index, 0));
	EvSelect(this, etSelect);
}

void RtwComboBox::SetLineParam(int index, void* param)
{
	m_pLstDropDown->SetLineParam(index,param);
}
void* RtwComboBox::GetLineParam(int index)
{
	return m_pLstDropDown->getLineParam(index);
}
RtwListBox::SLine* RtwComboBox::GetSelectLine()
{
	return m_pLstDropDown->getSelectLineItem();
}
bool RtwComboBox::AddListItem(const std::string& text)
{
	return m_pLstDropDown->AddItem(text);
}
bool RtwComboBox::SetLineNameByParam(void *pParam,string name,int col/* = 0*/)
{
	return m_pLstDropDown->SetLineNameByParam(pParam,name,col);
}

bool RtwComboBox::RemoveLineByParam(void *pParam,bool refresh/* = true*/)
{
	return m_pLstDropDown->RemoveLineByParam(pParam,refresh);
}

bool RtwComboBox::SetSelectLineByParam(DWORD pParam)
{
	if (m_pLstDropDown->SetSelectLineByParam(pParam))
	{
		int index = m_pLstDropDown->getSelectLine();
		SetText(m_pLstDropDown->getLineItemText(index, 0));
		return true;
	}
	return false;
}