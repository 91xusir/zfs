#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.05.06 10:00
//	filename: 	rtw_listbox.cpp
//	author:		ldr123
//	purpose:	列表框
//  other:		局部重构添加注释
//********************************************************************

RT_IMPLEMENT_DYNAMIC(RtwListBox, RtwWidget, 0, "ui")

RtwListBox::RtwListBox():
							m_ScrollLineOffset(0), m_ScrollLineLast(0),
							m_bDrawScrollV(false), m_bScroll(true), m_nLineHeight(-1)
{
	m_pAutoScrollV = NULL;
}

RtwListBox::~RtwListBox(){}

void RtwListBox::OnBeforeDestroy()
{
	_ResetSelf();

	RemoveAllColumns(false);
	RemoveAllItems(false);
	DROP_RTUI_OBJECT(m_pAutoScrollV);

	RtwWidget::OnBeforeDestroy();
}

void RtwListBox::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwListBox::_ResetSelf()
{
	m_WidgetType		= wtListBox;
	m_Flag				= m_DefaultFlag;
	m_ScrollLineOffset	= 0;
	m_ScrollLineLast	= 0;
	m_bAutoResize		= false;
	m_bDrawHeader		= true;
	m_bDrawScrollV		= false;
	m_bMultiSelect		= false;
	m_bHoverSelect		= false;
	m_TextAlign			= SUiTextAlignment(alignNear, alignCenter);
	m_nLineHeight		= -1;

	_ClearSelectLines();
}

bool RtwListBox::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();
	
	// 创建自动滚动条
	RtwVScrollBar* pSbv = NULL;
	UI_ENSURE_B(g_workspace.FindWidget(g_SystemWidgetName[sw_AutoSbv], (RtwWidget**)&pSbv));

	m_pAutoScrollV = (RtwVScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarV);
	UI_ENSURE_B(m_pAutoScrollV);
	m_pAutoScrollV->SetName("$Sbv");
	AddChild(m_pAutoScrollV);
	UI_ENSURE_B(m_pAutoScrollV->CloneFrom(pSbv));
	m_pAutoScrollV->EvScroll += RTW_CALLBACK(this, RtwListBox, OnScroll_ScrollBarV);

	//EvMouseWheel,追加MouseWheel处理逻辑..Dongri Li 2009.12.1
	EvMouseWheel += RTW_CALLBACK(this, RtwListBox, OnMouseWheel_Item);
	if (m_bDrawScrollV)
		m_pAutoScrollV->Show();
	else 
		m_pAutoScrollV->Hide();

	DROP_RTUI_OBJECT(pSbv);

	//CalcClient();

	return true;
}

int RtwListBox::InsertColumn(const std::string& name, const int width, const RtwPixel color/* = SColor(0xffffffff)*/, int indexBefore/* = -1*/, bool bRefresh/* = true*/)
{
	SColumn col(name, width);
	// 创建Header
	RtwButton* pBtn = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	int iRet = -1;
	pBtn->SetUseSystemFont( false );
	UI_ENSURE_B(pBtn);
	pBtn->EvLClick += RTW_CALLBACK(this, RtwListBox, OnClick_Header);
	pBtn->SetText(name);
	pBtn->SetTextAlignment(SUiTextAlignment(alignCenter, alignCenter));
	pBtn->SetTextColor(color);
	col.pBtnHeader = pBtn;
	AddChild(col.pBtnHeader);
	pBtn->drop();
	// 插入到指定位置
	if (indexBefore < 0)
	{
		m_Columns.insert(m_Columns.end(), col);
		iRet = (int)m_Columns.size() - 1;
	}
	else if (indexBefore > (int)m_Columns.size())
		return -1;
	else 
	{
		m_Columns.insert(m_Columns.begin() + indexBefore, col);
		iRet = indexBefore;
	}
	// 填充每一行的Item
	foreach (std::vector<SLine>, iterLine, m_Lines)
	{
		SLine& line = *iterLine;

		SItem item;
		_CreateItem(item);

		if (indexBefore >= 0)
			line.items.insert(line.items.begin() + indexBefore, item);
		else 
			line.items.insert(line.items.end(), item);
	}
	// 刷新
	if (bRefresh)
		CalcClient();

	return iRet;
}

bool RtwListBox::InsertLine(void* param/* = NULL*/, int indexBefore/* = -1*/, bool bRefresh/* = true*/)
{
	SLine line;
	//Widen 4 pixel to line height in order to show fonts normally, update by chib at 2006/10/18 
	line.height = (m_nLineHeight == -1) ? (g_workspace.getFontManager()->getDefaultFont()->getFontSize() + 4) : m_nLineHeight; 
	line.param = param;

	// 创建Widget
	foreach (std::vector<SColumn>, iterCol, m_Columns)
	{
		SItem item;
		_CreateItem(item);
		line.items.push_back(item);
	}
	// 插入到指定位置
	if (indexBefore < 0)
		m_Lines.insert(m_Lines.end(), line);
	else if (indexBefore > (int)m_Columns.size())
		return false;
	else 
		m_Lines.insert(m_Lines.begin() + indexBefore, line);
	// 刷新
	if (bRefresh)
		CalcClient();

	return true;
}

bool RtwListBox::InsertLine(const std::string& text, const RtwPixel color/* = SColor(0xffffffff)*/, int indexBefore/* = -1*/, bool bRefresh/* = true*/,
							void* param/*= NULL*/)
{
	if (!InsertLine(param, indexBefore, false))
		return false;

	if (!m_Columns.empty())
	{
		SLine* pLine = NULL;
		if (indexBefore >= 0)
			pLine = _getLine(indexBefore);
		else 
			pLine = _getLine((int)m_Lines.size() - 1);

		UI_ENSURE_B(pLine);
		if (pLine->items.size() > 1)
		{
			pLine->items[2].pWidget->SetFontSize(m_FontSize);//tim.yang  功能:在UI编辑器中可以修改ListBox中字体的大小,只有12,14,16三种
			pLine->items[2].pWidget->SetText(text);
			pLine->items[2].pWidget->SetTextAlignment(m_TextAlign);
			pLine->items[2].pWidget->SetTextColor(color);
		}
		else if (pLine->items.size() >0)
		{
			pLine->items[0].pWidget->SetFontSize(m_FontSize);//tim.yang  功能:在UI编辑器中可以修改ListBox中字体的大小,只有12,14,16三种
			pLine->items[0].pWidget->SetText(text);
			pLine->items[0].pWidget->SetTextAlignment(m_TextAlign);
			pLine->items[0].pWidget->SetTextColor(color);

		}
	}

	if (bRefresh)
		CalcClient();

	return true;
}

void RtwListBox::SetLineParam(int index, void* param)
{
	SLine* pLine = NULL;
	if (index >= 0)
		pLine = _getLine(index);

	if (pLine)
	{
		pLine->param = param;
	}
}

void* RtwListBox::getLineParam(int index)
{
	SLine* pLine = NULL;
	if (index >= 0)
		pLine = _getLine(index);

	if (pLine)
		return pLine->param;
	else
		return NULL;
}

void RtwListBox::SetLineColor(int index /*= -1*/, const RtwPixel color/* = SColor(0xffffffff)*/)
{
	SLine* pLine = NULL;
	if (index >= 0)
		pLine = _getLine(index);
	else 
		pLine = _getLine((int)m_Lines.size() - 1);

	if (pLine)
	{
		for (int i = 0; i < (int)pLine->items.size(); ++i)
		{
			pLine->items[i].pWidget->SetTextColor(color);
		}
	}

}

bool RtwListBox::AddItem(int indexBefore/* = -1*/, bool bRefresh/* = true*/)
{
	if (indexBefore > (int)m_Columns.size())
		return false;

	SLine line;
	line.height = (m_nLineHeight == -1) ? (g_workspace.getFontManager()->getDefaultFont()->getFontSize() + 4) : m_nLineHeight;
	// 创建Widget
	foreach (std::vector<SColumn>, iterCol, m_Columns)
	{
		SItem item;
		_CreateItem(item);
		line.items.push_back(item);
	}
	// 插入到指定位置
	if (indexBefore < 0)
		m_Lines.insert(m_Lines.end(), line);
	else 
		m_Lines.insert(m_Lines.begin() + indexBefore, line);
	// 刷新
	if (bRefresh)
		CalcClient();

	return true;
}

bool RtwListBox::AddItem(const std::string& text, int indexBefore/* = -1*/, bool bRefresh/* = true*/)
{
	if (m_Columns.empty() && text != "")
	{
		InsertColumn("",GetFrameRect().getWidth());
		SetDrawHeader(false);
	}
	if (!AddItem(indexBefore, false))
		return false;

	if (!m_Columns.empty())
	{
		SLine* pLine = NULL;
		if (indexBefore >= 0)
			pLine = _getLine(indexBefore);
		else 
			pLine = _getLine((int)m_Lines.size() - 1);

		UI_ENSURE_B(pLine);
		pLine->items[0].pWidget->SetText(text);
	}

	if (bRefresh)
		CalcClient();

	return true;
}

bool RtwListBox::SetLineItem(int line, int col, const std::string& text, const RtwPixel color/* = SColor(0xffffffff)*/, SUiTextAlignment align)

{
	UI_ENSURE_B( (line >= 0 && line < (int)m_Lines.size()) && (col >= 0 && col < (int)m_Columns.size()) );
	SLine* pLine = _getLine(line);
	UI_ENSURE_B(pLine);

	RtwLabel* pLblItem = pLine->items[col].pWidget;
	UI_ENSURE_B(pLblItem);
	pLblItem->SetText(text);
	pLblItem->SetTextColor(color);
	pLblItem->SetUseSystemFont( false );
	pLblItem->SetTextAlignment(align);

	return true;
}

bool RtwListBox::SetLineItemImage(int line,int col,RtwImage* pImage)
{
	UI_ENSURE_B( (line >= 0 && line < (int)m_Lines.size()) && (col >= 0 && col < (int)m_Columns.size()) );
	SLine* pLine = _getLine(line);
	UI_ENSURE_B(pLine);

	RtwLabel* pLblItem = pLine->items[col].pWidget;
	UI_ENSURE_B(pLblItem);

	pLblItem->SetBackgroundImage(pImage);

	return true;

}

bool RtwListBox::RemoveLine(int line, bool bRefresh/* = true*/)
{
	SLine* pLine = _getLine(line);
	if (pLine)
	{
		foreach (std::vector<SItem>, iterItem, pLine->items)
		{
			SItem& item = *iterItem;
			_DestroyItem(item);
		}
		m_Lines.erase(m_Lines.begin() + line);

		/*
		2009.12.13 Dongri Li
		解决删除最后一行后,select的行号还是已经删除的那行,造成报错
		之所以用m_Lines.size()-1是为了解决一次删除N个元素的情况.
		*/
		m_SelectLines.clear();
		if ((int)m_Lines.size() <= line)
		{
			int result = (int)m_Lines.size()-1;
			if (result >= 0)
				_AddSelectLine(result);
		}
		else
		{
			_AddSelectLine(line);
		}
	}

	if (bRefresh)
		CalcClient();

	return true;
}

bool RtwListBox::RemoveCol(int col, bool bRefresh/* = true*/)
{
	if ((int)m_Lines.size() < 1)
	{
		return false;
	}

	SColumn* pCol = _getColumn(col);
	if (pCol)
	{
		RemoveChild(pCol->pBtnHeader->getId(), false);
 		DROP_RTUI_OBJECT(pCol->pBtnHeader);

		foreach (std::vector<SLine>, iterLine, m_Lines)
		{
			SLine& line = *iterLine;
			SItem& item = line.items[col];
			_DestroyItem(item);
			line.items.erase(line.items.begin() + col);
		}

		m_Columns.erase(m_Columns.begin() + col);
	}

	if (bRefresh)
		CalcClient();

	return true;
}

void RtwListBox::RemoveAllLines(bool bRefresh/* = true*/)
{
	m_ScrollLineOffset = 0;

	_ClearSelectLines( );
	for (int i = (int)m_Lines.size() - 1; i >= 0; i--)
		RemoveLine(i);

	if (bRefresh)
		CalcClient();
}

void RtwListBox::RemoveAllItems(bool bRefresh/* = true*/)
{
	for (int i = (int)m_Lines.size() - 1; i >= 0; i--)
		RemoveLine(i);
	m_SelectLines.clear();

	ResetScrollBar();
	if (bRefresh)
		CalcClient();
}

void RtwListBox::RemoveAllColumns(bool bRefresh/* = true*/)
{
	for (int i = (int)m_Columns.size() - 1; i >= 0; i--)
		RemoveCol(i);

	if (bRefresh)
		CalcClient();
}

void RtwListBox::SetSelect(int index/* = -1*/)
{
	if (index < 0 || index >= (int)m_Lines.size())
	{
		_ClearSelectLines();
		//EvSelect(this, etSelect);
		return;
	}
	if (!_FindInSelectLines(index))
	{
		if (!m_bMultiSelect)
			_ClearSelectLines();

		_AddSelectLine(index);
		EvSelect(this, etSelect);
	}
}

void RtwListBox::SetSelectedItem(int index/* = -1*/)
{
	if (index < 0 || index >= (int)m_Lines.size())
	{
		_ClearSelectLines();
		//EvSelect(this, etSelect);
		return;
	}
	if (!_FindInSelectLines(index))
	{
		_AddSelectLine(index);
		EvSelect(this, etSelect);
	}
}

void RtwListBox::SetDrawHeader(bool bDraw, bool bRefresh/* = true*/)
{
	if (m_bDrawHeader == bDraw)
		return;

	m_bDrawHeader = bDraw;

	if (bRefresh)
		CalcClient();
}

void RtwListBox::SetDrawScrollBarV(bool bDraw, bool bRefresh/* = true*/)
{
 	if (m_bDrawScrollV == bDraw)
 		return;

	m_bDrawScrollV = bDraw;
	if (bRefresh)
		CalcClient();
}

void RtwListBox::SetAutoResize(bool bAutoResize, bool bRefresh/* = true*/)
{
	if (m_bAutoResize == bAutoResize)
		return;

	m_bAutoResize = bAutoResize;
	if (bRefresh)
		CalcClient();
}

void RtwListBox::SetMultiSelect(bool bMultiSelect)
{
	if (m_bMultiSelect == bMultiSelect)
		return;

	m_bMultiSelect = bMultiSelect;
	_ClearSelectLines();
	//EvSelect(this, etSelect);
}

void RtwListBox::SetHoverSelect(bool bHoverSelect)
{
	m_bHoverSelect = bHoverSelect;
}

void RtwListBox::SetColumnWidth(int index, int width, bool bRefresh/* = true*/)
{
	SColumn* pCol = _getColumn(index);
	if (pCol)
	{
		pCol->width = width;
		if (bRefresh)
			CalcClient();
	}
}

int RtwListBox::GetItemHeight(int index)
{
	SLine* pLine = _getLine(index);
	if (!pLine)
		return 0;
	else 
		return pLine->height;
}

int RtwListBox::getLineHeight(int index)
{
	SLine* pLine = _getLine(index);
	if (!pLine)
		return 0;
	else 
		return pLine->height;
}

int RtwListBox::getColWidth(int index)
{
	SColumn* pCol = _getColumn(index);
	if (!pCol)
		return 0;
	else 
		return pCol->width;
}

std::string RtwListBox::getLineItemText(int line, int col)
{
	SLine* pLine = _getLine( line);
	if (!pLine)
		return "";
	if (col < 0 || col >= (int)pLine->items.size())
		return "";

	return pLine->items[col].pWidget->GetText();
}

int RtwListBox::getLineItemIndex(std::string mm)
{
	for(int i=0;i<getLineCount();++i)
	{
		SLine* pLine = _getLine(i);
		if(pLine->items[0].pWidget->getText()==mm)
			return i;
	}
	return 0;
}

RtwButton* RtwListBox::getHeaderButton(int index)
{
	SColumn* pCol = _getColumn(index);
	if (!pCol)
		return NULL;
	else 
		return pCol->pBtnHeader;
}

void RtwListBox::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	foreach (std::list<int>, iterSel, m_SelectLines)
	{
		int selIndex = *iterSel;
		if (m_Lines[selIndex].bDraw)
		{
			RtwRect selectrect = _GetLineRect(selIndex);
			selectrect.left += 1;
			selectrect.right -= g_workspace.getDefaultScrollVWidth()+3;
			selectrect.top += 1;
			selectrect.bottom -= 1;
			g_workspace.getRenderDevice()->DrawRectangle(selectrect, 0xb24060f0);
		}
	}

	RtwWidget::DrawForeground(pClipRect);
}

void RtwListBox::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget*  pkParent         = getParent();
	RtwWidget*  ppkParentPraent  = NULL;
	if(  pkParent )
	{
		ppkParentPraent = pkParent->getParent();

	}
	if( pkParent &&
		ppkParentPraent &&
		pkParent->getWidgetType()==wtComboBox )
	{
// 		RtwWidget::DrawBackground(&( ppkParentPraent->getFrameRect()));
		RtwWidget::DrawBackground(&m_rcFrame);
	}
	else
	{
		RtwWidget::DrawBackground(pClipRect);
	}
}

bool RtwListBox::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwListBox::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwListBox* pOther = (RtwListBox*)pWidget;

	m_bDrawHeader = pOther->m_bDrawHeader;
	m_bDrawScrollV = pOther->m_bDrawScrollV;
	m_SelectLines = pOther->m_SelectLines;
	m_ScrollLineOffset = pOther->m_ScrollLineOffset;
	m_ScrollLineLast = pOther->m_ScrollLineLast;

	m_bMultiSelect = pOther->m_bMultiSelect;
	m_bHoverSelect = pOther->m_bHoverSelect;
	m_bAutoResize = pOther->m_bAutoResize;

	RemoveAllItems(false);
	RemoveAllColumns(false);
	m_Lines = pOther->m_Lines;
	m_Columns = pOther->m_Columns;

	if (pOther->m_pAutoScrollV)
	{
		if (!pOther->m_pAutoScrollV)
		{
			m_pAutoScrollV = (RtwVScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarV);
			UI_ENSURE_B(m_pAutoScrollV);
		}
		m_pAutoScrollV->CloneFrom(pOther->m_pAutoScrollV);
	}
	else 
	{
		DROP_RTUI_OBJECT(pOther->m_pAutoScrollV);
	}

	m_rcSbv = pOther->m_rcSbv;

	return true;
}

void RtwListBox::CalcClient()
{
	// 如果是自动调整大小就计算rcClient和rcFrame
	if (m_bAutoResize)
	{
		int width = 0;
		for (int i = 0; i < (int)m_Columns.size(); ++i)
		{
			width += m_Columns[i].width;
		}
		int height = 0;
		for (int i = 0; i < (int)m_Lines.size(); ++i)
		{
			height += m_Lines[i].height;
		}
		m_rcClient = m_rcFrame;
		m_rcClient.right = width + m_rcClient.left;
		m_rcClient.bottom = height + m_rcClient.top;

		m_rcFrame = m_rcClient;
		if (m_bDrawScrollV)
			m_rcFrame.right += g_workspace.getDefaultScrollVWidth();
	}
	else 
	{
		m_rcClient = m_rcFrame;
		if (m_bDrawScrollV)
			m_rcClient.right -= g_workspace.getDefaultScrollVWidth();
	}


	// 计算每个item的位置
	CalcItemsPos();
	// 计算ScrollBarV的位置和数据
	if (m_pAutoScrollV)
	{
		int hight=0;
		if (((int)m_Lines.size())*(_GetLineRect(m_ScrollLineLast).bottom-_GetLineRect(m_ScrollLineLast).top) <=m_rcFrame.getHeight())
		{
			hight=0;
		}else
		{
			hight=10;
		}
		if (m_bDrawScrollV || _GetLineRect(m_ScrollLineLast).bottom >= getFrameRect().bottom-hight)
		{
			if(m_bDrawHeader)
				m_rcSbv = RtwRect(m_rcFrame.right-1 - g_workspace.getDefaultScrollVWidth(), m_rcFrame.top+m_HeaderHeight, m_rcFrame.right-1, m_rcFrame.bottom-1);
			else
				m_rcSbv = RtwRect(m_rcFrame.right-1 - g_workspace.getDefaultScrollVWidth(), m_rcFrame.top, m_rcFrame.right-1, m_rcFrame.bottom-1);
			m_rcSbv.Offset(SSize(-m_rcFrame.left, -m_rcFrame.top));
			m_pAutoScrollV->Move(m_rcSbv);
			if (m_bScroll)
			{
				m_pAutoScrollV->Show();
			}
			int sbv_min = 0;
			int sbv_max = 0;
			if (getFirstViewLine() != 0 || getLastViewLine() != (int)m_Lines.size()-1)
				sbv_max = (int)m_Lines.size() - (getLastViewLine() - getFirstViewLine() + 1);
			else
			sbv_max        = (int)m_Lines.size() - 2;
			int sbv_value = m_ScrollLineOffset;
			Real sbv_scale = m_Lines.empty() ? 1.0 : (getLastViewLine() - getFirstViewLine() + 1.0) / m_Lines.size();
			m_pAutoScrollV->SetData(sbv_min, sbv_max, sbv_value, sbv_scale);
		}
		else 
		{
			if ( ((int)m_Lines.size())*(_GetLineRect(m_ScrollLineLast).bottom-_GetLineRect(m_ScrollLineLast).top) <m_rcFrame.getHeight())
			{
				m_pAutoScrollV->Hide();
			}
		}
	}
}

void RtwListBox::OnLButtonDown_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	int line = _GetLineFromPosition(pEvent->mouse.y);
	int col = _GetColFromPosition(pEvent->mouse.x);
	UI_CHECK(line >= 0 && col >= 0);

	if (_FindInSelectLines(line))
	{
		if (m_bHoverSelect)
		{
			EvSelect(this, etSelect);
		}
		else 
		{
			if (m_bMultiSelect)
			{
				if (g_workspace.isKeyPressed(vkCtrl))
				{
					_RemoveSelectLine(line);
					EvSelect(this, etSelect);
				}
			}
		}
	}
	else 
	{
		if (!m_bMultiSelect)
			_ClearSelectLines();
		_AddSelectLine(line);
		EvSelect(this, etSelect);
	}

	EvMouseLDown(this, *pEvent);
}

void RtwListBox::OnRButtonDown_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{// [chib][add]
	int line = _GetLineFromPosition(pEvent->mouse.y);
	int col = _GetColFromPosition(pEvent->mouse.x);
	UI_CHECK(line >= 0 && col >= 0);
	if (_FindInSelectLines(line))
	{
		EvSelect(this, etSelect);
	}
	else 
	{
		if (!m_bMultiSelect)
			_ClearSelectLines();
		_AddSelectLine(line);
		EvSelect(this, etSelect);
	}

	EvRClickItem(this, *pEvent);
}

void RtwListBox::OnLDClick(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	int line = _GetLineFromPosition(pEvent->mouse.y);
	int col = _GetColFromPosition(pEvent->mouse.x);
	UI_CHECK(line >= 0 && col >= 0);
	if (_FindInSelectLines(line))
	{
		EvSelect(this, etSelect);
	}
	else 
	{
		if (!m_bMultiSelect)
			_ClearSelectLines();
		_AddSelectLine(line);
		EvSelect(this, etSelect);
	}
	EvLDClick(this, *pEvent);
}

void RtwListBox::OnHint(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	int line = _GetLineFromPosition(g_workspace.GetMousePosY());
	UI_ENSURE(line >= 0);
// 	if (_FindInSelectLines(line))
	{
		RtwEventDelegate redEvent;
		redEvent.menuSelect.index = line;

		EvHintItem(this, redEvent);//etHint
	}
// 	else 
// 	{
// 		if (!m_bMultiSelect)
// 			_ClearSelectLines();
// 		_AddSelectLine(line);
// 	}
}

void RtwListBox::OnMouseMove_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (!m_bHoverSelect)
		return;

	int line = _GetLineFromPosition(pEvent->mouse.y);
	int col = _GetColFromPosition(pEvent->mouse.x);
	UI_ENSURE(line >= 0 && col >= 0);
	if (_FindInSelectLines(line))
	{
		EvMoveItem(this, *pEvent);//etMove
	}
	else 
	{
		if (!m_bMultiSelect)
			_ClearSelectLines();
		_AddSelectLine(line);
	}
}

void RtwListBox::OnMouseWheel_Item(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if( pEvent )
	{
		if( pEvent->mouseWheel.delta < 0 )
		{
			m_ScrollLineOffset ++;
		}
		else if( pEvent->mouseWheel.delta > 0 && m_ScrollLineOffset>0)
		{
			m_ScrollLineOffset --;
		}
	}
	if (m_Lines.empty())
		m_ScrollLineOffset = 0;
	else if (m_ScrollLineOffset >= (int)m_Lines.size())
		m_ScrollLineOffset = (int)m_Lines.size() - 1;

	CalcClient();
}

void RtwListBox::OnClick_Header(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	EvLClickHeader(pWidget, weMouseClick);
}

void RtwListBox::DefaultKeyChar(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{

}
void RtwListBox::OnScroll_ScrollBarV(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
 	int value = m_pAutoScrollV->GetValue();
 
 	m_ScrollLineOffset = value;
 	if (m_Lines.empty())
 		m_ScrollLineOffset = 0;
 	else if (m_ScrollLineOffset >= (int)m_Lines.size())
 		m_ScrollLineOffset = (int)m_Lines.size() - 1;

	CalcClient();
}

void RtwListBox::CalcItemsPos()
{
	if (m_ScrollLineOffset > (int)m_Lines.size())
		return;

	// 计算列的位置和宽度,Header的位置
	int PosY = 0;
	std::vector<int> arrColPosX;
	std::vector<int> arrColWidth;
	int nTmp = 2;
	foreach (std::vector<SColumn>, iterCol, m_Columns)
	{
		SColumn& col = *iterCol;
		col.PosX = nTmp;
		if (m_bDrawHeader)
		{
			col.pBtnHeader->Show();
			col.pBtnHeader->Move(RtwRect(nTmp, PosY, nTmp + col.width, PosY + m_HeaderHeight));
		}
		else 
		{
			col.pBtnHeader->Hide();
		}
		arrColWidth.push_back(col.width);
		arrColPosX.push_back(nTmp);
		nTmp += col.width+1;
	}
	if (m_bDrawHeader)
		PosY += m_HeaderHeight;

	// 计算每个Widget的位置
	for (int i = 0; i < m_ScrollLineOffset; ++i)
	{
		SLine& line = m_Lines[i];
		line.bDraw = false;
		foreach (std::vector<SItem>, iterItem, line.items)
		{
			SItem& item = *iterItem;
			item.pWidget->Hide();
		}
	}
	for (int i = m_ScrollLineOffset; i < (int)m_Lines.size(); ++i)
	{
		SLine& line = m_Lines[i];
		if (PosY + line.height > m_rcClient.getHeight())
			break;
		line.bDraw = true;
		m_ScrollLineLast = i;
		line.PosY = PosY;
		int colIndex = 0;
		foreach (std::vector<SItem>, iterItem, line.items)
		{
			SItem& item = *iterItem;
			item.pWidget->Show();
			item.pWidget->Move(RtwRect(arrColPosX[colIndex], PosY, arrColPosX[colIndex] + arrColWidth[colIndex], PosY + line.height));
			colIndex ++;
		}
		PosY += line.height;
	}
	for (int i = m_ScrollLineLast + 1; i < (int)m_Lines.size(); ++i)
	{
		SLine& line = m_Lines[i];
		line.bDraw = false;
		foreach (std::vector<SItem>, iterItem, line.items)
		{
			SItem& item = *iterItem;
			item.pWidget->Hide();
		}
	}
}

bool RtwListBox::_CreateItem(SItem& item)
{
	UI_CHECK_B(!item.pWidget);

	RtwLabel* pLabel = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
	UI_ENSURE_B(pLabel);
	pLabel->ModifyFlags(wfGrabMouse | wfMouseMove | wfFocus|wfHint, 0);
	pLabel->SetTextAlignment(m_TextAlign);
	pLabel->SetTextAlignment(SUiTextAlignment(alignNear, alignCenter));
	pLabel->EvMouseLDown	+= RTW_CALLBACK(this, RtwListBox, OnLButtonDown_Item);
	pLabel->EvMouseRDown	+= RTW_CALLBACK(this, RtwListBox, OnRButtonDown_Item);
	pLabel->EvMouseMove		+= RTW_CALLBACK(this, RtwListBox, OnMouseMove_Item);
	pLabel->EvHint			+= RTW_CALLBACK(this, RtwListBox, OnHint);
	pLabel->EvMouseWheel	+= RTW_CALLBACK(this, RtwListBox, OnMouseWheel_Item);
	pLabel->EvLDClick		+= RTW_CALLBACK(this, RtwListBox, OnLDClick);

	item.pWidget = pLabel;
	AddChild(item.pWidget);
	pLabel->drop();

	return true;
}

bool RtwListBox::_DestroyItem(SItem& item)
{
	if (item.pWidget)
	{
		RemoveChild(item.pWidget->getId(), false);
		item.pWidget = NULL;
		//DROP_RTUI_OBJECT(item.pWidget);
	}

	return true;
}

int RtwListBox::_GetColFromPosition(int PointX)
{
	PointX -= m_rcClient.left;

	int index = 0;
	foreach(std::vector<SColumn>, iterCol, m_Columns)
	{
		SColumn& Col = *iterCol;
		if (Col.PosX + Col.width >= PointX)
		{
			return index;
		}
		index ++;
	}
	return -1;
}

RtwRect RtwListBox::_GetLineRect(int line)
{
	RtwRect rc(m_rcClient.left, 0, m_rcClient.right, 0);
	if (line < m_ScrollLineOffset || line >= (int)m_Lines.size())
		return rc;

	int PosY = m_bDrawHeader ? m_HeaderHeight : 0;
	for (int i = m_ScrollLineOffset; i < line; ++i)
	{
		PosY += m_Lines[line].height;
	}
	rc.top = PosY;
	rc.bottom = PosY + m_Lines[line].height;
	rc.Offset(SSize(0, m_rcClient.top));
	return rc;
}

int RtwListBox::_GetLineFromPosition(int PointY)
{
	PointY -= m_rcClient.top;

	int index = m_ScrollLineOffset;
	for (int i = m_ScrollLineOffset; i < (int)m_Lines.size(); ++i)
	{
		SLine& Line = m_Lines[i];
		if (Line.PosY + Line.height >= PointY)
		{
			return index;
		}
		index ++;
	}
	return -1;
}

void RtwListBox::_AddSelectLine(int index)
{
	if (index < 0 || index >= (int)m_Lines.size())
		return;
	if (_FindInSelectLines(index))
		return;
	m_SelectLines.push_back(index);
}

void RtwListBox::_RemoveSelectLine(int index)
{
	if (index < 0 || index >= (int)m_Lines.size())
		return;
	foreach (std::list<int>, iterSel, m_SelectLines)
	{
		int i = *iterSel;
		if (i == index)
		{
			m_SelectLines.erase(iterSel);
			return;
		}
	}
}

bool RtwListBox::_FindInSelectLines(int index)
{
	foreach (std::list<int>, iterSel, m_SelectLines)
	{
		int i = *iterSel;
		if (i == index)
		{
			return true;
		}
	}
	return false;
}

void RtwListBox::_ClearSelectLines()
{
	m_SelectLines.clear();
}

void RtwListBox::OnRemoveChild(RtwWidget* pWidget)
{
	if (pWidget == m_pAutoScrollV)
		DROP_RTUI_OBJECT(m_pAutoScrollV);
}
// add [3/17/2009 tooth.shi]

void RtwListBox::SetItemColor(int index,RtwPixel color)
{
//	SetSelectedItem(index);
	SetLineColor(index,color);
}

int RtwListBox::IndexFromPoint(int x,int y, bool bIndex)
{
	if (m_Lines.size()>0)
	{
		for (int i = 0; i < (int)m_Lines.size(); ++i)
		{
			SLine& line = m_Lines[i];
			if ( (y>=line.PosY) && (y <=line.PosY+line.height) )
			{
				return i;
			}
		}
		return -1;
	}else
	{
		return -1;
	}
	
	
}

void RtwListBox::SetItemData(int index, void* param)
{
	SLine* pLine = NULL;
	if (index >= 0)
		pLine = _getLine(index);

	if (pLine)
	{
		pLine->param = param;
	}
}

void RtwListBox::SetItemIcon(int index, RtwImage*)
{

}

void RtwListBox::SetFirstViewLine(int line)
{
	if (line < 0) line = 0;
	if (line > (int)m_Lines.size()) line = (int)m_Lines.size();
	m_ScrollLineOffset = line;
}

void RtwListBox::SetLastViewLine(int line)
{
	if (line < 0) line = 0;
	if (line > (int)m_Lines.size()) line = (int)m_Lines.size();
	m_ScrollLineLast = line;
}

bool RtwListBox::SetLineNameByParam(void *pParam,string name,int col/* = 0*/)
{
	if (!m_Lines.empty())
	{
		for (int i = 0; i < (int)m_Lines.size(); ++i)
		{
			if (m_Lines[i].param == pParam)
			{
				SetItemText(i,col,name);
				return true;
			}
		}
	}
	return false;
}

bool RtwListBox::RemoveLineByParam(void *pParam,bool refresh/* = true*/)
{
	if (!m_Lines.empty())
	{
		for (int i = 0; i < (int)m_Lines.size(); ++i)
		{
			if (m_Lines[i].param == pParam)
			{
				RemoveLine(i,refresh);
			}
		}
	}
	return NULL;
}
bool RtwListBox::SetSelectLineByParam(DWORD pParam)
{
	if (!m_Lines.empty())
	{
		for (int i = 0; i < (int)m_Lines.size(); ++i)
		{
			if (*(DWORD*)(m_Lines[i].param) == pParam)
			{
				SetSelectedItem(i);
				return true;
			}
		}
	}
	return false;
}