#include "ui/rtw_ui.h"
//********************************************************************
//	created:	2010.05.04 10:00
//	filename: 	rtw_scrollbar.cpp
//	author:		ldr123
//	purpose:	滚动条
//  other:		局部重构添加注释
//********************************************************************

RT_IMPLEMENT_DYNAMIC(RtwScrollBar,  RtwWidget, 0, "ui")
RT_IMPLEMENT_DYNAMIC(RtwVScrollBar, RtwWidget, 0, "ui")
RT_IMPLEMENT_DYNAMIC(RtwHScrollBar, RtwWidget, 0, "ui")

RtwScrollBar::RtwScrollBar():m_pWgtScrollHandle(NULL){}

RtwScrollBar::~RtwScrollBar(){}

void RtwScrollBar::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pWgtScrollHandle);

	RtwWidget::OnBeforeDestroy();
}

void RtwScrollBar::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

bool RtwScrollBar::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();
	//滚动条条形控件
	m_pWgtScrollHandle = g_workspace.getWidgetFactory()->createWidget(wtWidget, "$WgtHandle");
	UI_ENSURE_B(m_pWgtScrollHandle);

	m_pWgtScrollHandle->ModifyFlags(wfGrabMouse | wfMouseMove, 0);
	AddChild(m_pWgtScrollHandle);

	m_pWgtScrollHandle->EvMouseMove += RTW_CALLBACK(this, RtwScrollBar, OnMouseMove_ScrollHandle);

	return true;
}

bool RtwScrollBar::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwScrollBar::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwScrollBar* pOther = (RtwScrollBar*)pWidget;
	m_Min	= pOther->m_Min;
	m_Max	= pOther->m_Max;
	m_Value = pOther->m_Value;
	m_Scale = pOther->m_Scale;

	if (pOther->m_pWgtScrollHandle)
	{
		if (!m_pWgtScrollHandle)
		{
			m_pWgtScrollHandle = g_workspace.getWidgetFactory()->createWidget(wtWidget);
			UI_ENSURE_B(m_pWgtScrollHandle);
		}
		m_pWgtScrollHandle->CloneFrom(pOther->m_pWgtScrollHandle);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pWgtScrollHandle);
	}

	m_ScrollHandleHeight	= pOther->m_ScrollHandleHeight;
	m_rcUpButton			= pOther->m_rcUpButton;
	m_rcDownButton			= pOther->m_rcDownButton;
	m_rcScrollSpace			= pOther->m_rcScrollSpace;
	m_rcScrollHandle		= pOther->m_rcScrollHandle;

	return true;
}

//设置滚动条数据
void RtwScrollBar::SetData(int min/* = -1*/, int max/* = -1*/, int value/* = -1*/, Real scale/* = -1.0*/)
{
	if (min >= 0)
	{
		m_Min = min;
	}

	if (max >= 0)
	{
		m_Max = max;
	}

	if (value >= 0)
	{
		m_Value = value;
	}

	if (scale >= 0.0f)
	{
		m_Scale = scale;
	}

	CalcClient();
}

void RtwScrollBar::SetMin(int min)
{
	m_Min = min;
	if (m_Max < m_Min)
	{
		m_Max = m_Min;
	}

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}

	CalcClient();
}

void RtwScrollBar::SetMax(int max)
{
	m_Max = max;
	if (m_Min > m_Max)
	{
		m_Min = m_Max;
	}

	if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();
}

void RtwScrollBar::SetValue(int value,bool b)
{
	m_Value = value;

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}
	else if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();
	EvScroll(this, etScroll);
}

void RtwScrollBar::SetScale(float scale)
{
	m_Scale = scale;

	if (m_Scale < 0.01f)
	{
		m_Scale = 0.01f;
	}
	else if (m_Scale > 100.0f)
	{
		m_Scale = 100.0f;
	}

	CalcClient();
}

void RtwScrollBar::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);
}

void RtwScrollBar::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

void RtwScrollBar::CalcClient()
{
	m_rcClient = m_rcFrame;

	int width = m_rcClient.right - m_rcClient.left;

	// "向上"按钮位置
	m_rcUpButton.left		= m_rcClient.left;
	m_rcUpButton.right		= m_rcClient.right;
	m_rcUpButton.top		= m_rcClient.top;
	m_rcUpButton.bottom		= m_rcClient.top + width;

	if (m_rcUpButton.bottom > m_rcClient.bottom)
	{
		m_rcUpButton.bottom = m_rcClient.bottom;
	}

	// "向下"按钮位置
	m_rcDownButton.left		= m_rcClient.left;
	m_rcDownButton.right	= m_rcClient.right;
	m_rcDownButton.top		= m_rcClient.bottom - width;
	m_rcDownButton.bottom	= m_rcClient.bottom;

	if (m_rcDownButton.top < m_rcClient.top)
	{
		m_rcDownButton.top = m_rcClient.top;
	}

	// "滚动空间"位置
	m_rcScrollSpace.left	= m_rcClient.left;
	m_rcScrollSpace.right	= m_rcClient.right;
	m_rcScrollSpace.top		= m_rcUpButton.bottom + 1;
	m_rcScrollSpace.bottom	= m_rcDownButton.top - 1;

	if (m_rcScrollSpace.bottom < m_rcScrollSpace.top)
	{
		m_rcScrollSpace.bottom = m_rcScrollSpace.top;
	}

	// "滚动条"高度
	int nHeight = m_rcScrollSpace.getHeight();

	m_ScrollHandleHeight = m_Scale * nHeight;
	if (m_ScrollHandleHeight < m_DefaultScrollHandleHeight)
	{
		m_ScrollHandleHeight = m_DefaultScrollHandleHeight;
	}
	else if (m_ScrollHandleHeight > nHeight)
	{
		m_ScrollHandleHeight = nHeight;
	}

	// "滚动条"位置
	int start = m_rcScrollSpace.top + (m_ScrollHandleHeight>>1);
	int delta = nHeight - m_ScrollHandleHeight;
	if (delta > 0)
	{
		int pos = start + delta * m_Value / (float)(m_Max - m_Min);
		m_rcScrollHandle.left = m_rcClient.left;
		m_rcScrollHandle.right = m_rcClient.right;
		m_rcScrollHandle.top = pos - (m_ScrollHandleHeight>>1);
		m_rcScrollHandle.bottom = pos + (m_ScrollHandleHeight>>1);
	}
	else 
	{
		m_rcScrollHandle = m_rcScrollSpace;
	}

	//滚动条条状控件移动
	if (getScrollHandle())
	{
		getScrollHandle()->Move(m_rcScrollHandle);
		getScrollHandle()->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
	}
}

int RtwScrollBar::ValueFromPoint(const SPoint& point)
{
	int top		= m_rcScrollSpace.top		+ (m_ScrollHandleHeight>>1);
	int bottom	= m_rcScrollSpace.bottom	- (m_ScrollHandleHeight>>1);

	if (point.y <= top)
	{
		return m_Min;
	}
	else if (point.y >= bottom)
	{
		return m_Max;
	}

	return (point.y - (Real)top) / ( ((Real)bottom - top) / (m_Max - m_Min + 1) ) + m_Min;
}

//鼠标拖动滚动条
void RtwScrollBar::OnMouseMove_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (g_workspace.getMouseCapture() == getScrollHandle())
	{
		int value = ValueFromPoint(g_workspace.getMousePos());
		if (value == m_Value)
			return;

		m_Value = value;
		CalcClient();
		EvScroll(this, etScroll);
	}
}

void RtwScrollBar::_ResetSelf()
{
	m_WidgetType			= wtScrollBarV;
	m_Flag					= m_DefaultFlag;
	m_ScrollHandleHeight	= m_DefaultScrollHandleHeight;
	m_Min					= 0;
	m_Max					= 100;
	m_Value					= m_Min;
	m_Scale					= 0.2f;
	m_rcClient				= RtwRect(0, 0, m_DefaultWidth, 30);
	m_rcFrame				= RtwRect(0, 0, m_DefaultWidth, 30);
}


RtwVScrollBar::RtwVScrollBar()
{
	m_pBtnStepUp				= NULL;
	m_pBtnStepDown				= NULL;
	m_pWgtScrollHandle			= NULL;
	m_bHasLeftAndRightButton	= true;
}

RtwVScrollBar::~RtwVScrollBar(){}

void RtwVScrollBar::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pBtnStepUp);
	DROP_RTUI_OBJECT(m_pBtnStepDown);
	DROP_RTUI_OBJECT(m_pWgtScrollHandle);

	RtwWidget::OnBeforeDestroy();
}

void RtwVScrollBar::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwVScrollBar::_ResetSelf()
{
	m_WidgetType			= wtScrollBarV;
	m_Flag					= m_DefaultFlag;
	m_ScrollHandleHeight	= m_DefaultScrollHandleHeight;
	m_Min					= 0;
	m_Max					= 100;
	m_Value					= m_Min;
	m_Scale					= 0.2f;
	m_rcClient				= RtwRect(0, 0, m_DefaultWidth, 30);
	m_rcFrame				= RtwRect(0, 0, m_DefaultWidth, 30);
}

bool RtwVScrollBar::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	m_pBtnStepUp		= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, "$BtnUp");
	m_pBtnStepDown		= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, "$BtnDown");
	m_pWgtScrollHandle	= g_workspace.getWidgetFactory()->createWidget(wtWidget, "$WgtHandle");
	UI_ENSURE_B(m_pBtnStepUp);
	UI_ENSURE_B(m_pBtnStepDown);
	UI_ENSURE_B(m_pWgtScrollHandle);

	AddChild(m_pBtnStepUp);
	m_pBtnStepUp->ModifyFlags(0, wfFocus);
	m_pBtnStepUp->EvLClick		 += RTW_CALLBACK(this, RtwVScrollBar, OnClick_StepUp);
	m_pBtnStepUp->EvMouseCapture += RTW_CALLBACK(this, RtwVScrollBar, OnClick_StepUp);

	AddChild(m_pBtnStepDown);
	m_pBtnStepDown->ModifyFlags(0, wfFocus);
	m_pBtnStepDown->EvLClick		+= RTW_CALLBACK(this, RtwVScrollBar, OnClick_StepDown);
	m_pBtnStepDown->EvMouseCapture	+= RTW_CALLBACK(this, RtwVScrollBar, OnClick_StepDown);

	m_pWgtScrollHandle->ModifyFlags(wfGrabMouse | wfMouseMove, 0);
	AddChild(m_pWgtScrollHandle);
	m_pWgtScrollHandle->EvMouseMove	 += RTW_CALLBACK(this, RtwVScrollBar, OnMouseMove_ScrollHandle);
	m_pWgtScrollHandle->EvMouseLDown += RTW_CALLBACK(this, RtwVScrollBar, OnMouseLDown_ScrollHandle);
	m_pWgtScrollHandle->EvMouseWheel += RTW_CALLBACK(this, RtwVScrollBar, OnMouseWheel_ScrollHandle);

	EvMouseLDown += RTW_CALLBACK(this, RtwVScrollBar, OnMouseLDown_This);

	return true;
}

bool RtwVScrollBar::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwVScrollBar::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwVScrollBar* pOther = (RtwVScrollBar*)pWidget;
	m_Min	= pOther->m_Min;
	m_Max	= pOther->m_Max;
	m_Value = pOther->m_Value;
	m_Scale = pOther->m_Scale;

	if (pOther->m_pBtnStepUp)
	{
		if (!m_pBtnStepUp)
		{
			m_pBtnStepUp = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			UI_ENSURE_B(m_pBtnStepUp);
		}
		m_pBtnStepUp->CloneFrom(pOther->m_pBtnStepUp);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pBtnStepUp);
	}

	if (pOther->m_pBtnStepDown)
	{
		if (!m_pBtnStepDown)
		{
			m_pBtnStepDown = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			UI_ENSURE_B(m_pBtnStepDown);
		}
		m_pBtnStepDown->CloneFrom(pOther->m_pBtnStepDown);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pBtnStepDown);
	}

	if (pOther->m_pWgtScrollHandle)
	{
		if (!m_pWgtScrollHandle)
		{
			m_pWgtScrollHandle = g_workspace.getWidgetFactory()->createWidget(wtWidget);
			UI_ENSURE_B(m_pWgtScrollHandle);
		}
		m_pWgtScrollHandle->CloneFrom(pOther->m_pWgtScrollHandle);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pWgtScrollHandle);
	}

	m_ScrollHandleHeight	= pOther->m_ScrollHandleHeight;
	m_rcUpButton			= pOther->m_rcUpButton;
	m_rcDownButton			= pOther->m_rcDownButton;
	m_rcScrollSpace			= pOther->m_rcScrollSpace;
	m_rcScrollHandle		= pOther->m_rcScrollHandle;

	return true;
}

void RtwVScrollBar::SetData(int min/* = -1*/, int max/* = -1*/, int value/* = -1*/, Real scale/* = -1.0*/)
{
	if (min >= 0)
	{
		m_Min = min;
	}

	if (max >= 0)
	{
		m_Max = max;
	}

	if (value >= 0)
	{
		m_Value = value;
	}

	if (scale >= 0.0f)
	{
		m_Scale = scale;
	}

	CalcClient();
}

void RtwVScrollBar::SetMin(int min)
{
	m_Min = min;
	if (m_Max < m_Min)
	{
		m_Max = m_Min;
	}

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}

	CalcClient();
}

void RtwVScrollBar::SetMax(int max)
{
	m_Max = max;
	if (m_Min > m_Max)
	{
		m_Min = m_Max;
	}

	if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();
}

void RtwVScrollBar::SetValue(int value)
{
	RtwEventDelegate e(etScroll);

	e.scroll.OrigValue = m_Value;
	m_Value = value;

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}
	else if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();
	EvScroll(this, etScroll);
}

void RtwVScrollBar::SetScale(float scale)
{
	m_Scale = scale;

	if (m_Scale < 0.01f)
	{
		m_Scale = 0.01f;
	}
	else if (m_Scale > 100.0f)
	{
		m_Scale = 100.0f;
	}

	CalcClient();
}

void RtwVScrollBar::ScrollToTop()
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;
	m_Value = m_Min;
	CalcClient();
	EvScroll(this, etScroll);
}

void RtwVScrollBar::ScrollToBottom()
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;
	m_Value = m_Max;
	CalcClient();
	EvScroll(this, etScroll);
}

void RtwVScrollBar::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);
}

void RtwVScrollBar::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

void RtwVScrollBar::CalcClient()
{
	m_rcClient = m_rcFrame;

	int width = m_rcClient.right - m_rcClient.left;

	RtwRect rcUpButton, rcDownButton, rcScrollSpace, rcScrollHandle;

	// "向上"按钮位置
	rcUpButton.left		= m_rcClient.left;
	rcUpButton.right	= m_rcClient.right;
	rcUpButton.top		= m_rcClient.top;
	rcUpButton.bottom	= m_rcClient.top + width;

	if (rcUpButton.bottom > m_rcClient.bottom)
	{
		rcUpButton.bottom = m_rcClient.bottom;
	}

	if (getStepUpButton())
	{
		getStepUpButton()->Move(rcUpButton);
		getStepUpButton()->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
	}

	// "向下"按钮位置
	rcDownButton.left	= m_rcClient.left;
	rcDownButton.right	= m_rcClient.right;
	rcDownButton.top	= m_rcClient.bottom - width;
	rcDownButton.bottom = m_rcClient.bottom;

	if (rcDownButton.top < m_rcClient.top)
	{
		rcDownButton.top = m_rcClient.top;
	}

	if (getStepDownButton())
	{
		getStepDownButton()->Move(rcDownButton);
		getStepDownButton()->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
	}

	// "滚动空间"位置
	rcScrollSpace = getScrollSpaceRect();

	// "滚动条"高度
	int nHeight = rcScrollSpace.getHeight();
	m_ScrollHandleHeight = m_Scale * nHeight;

	if (m_ScrollHandleHeight < m_DefaultScrollHandleHeight)
	{
		m_ScrollHandleHeight = m_DefaultScrollHandleHeight;
	}
	else if (m_ScrollHandleHeight > nHeight)
	{
		m_ScrollHandleHeight = nHeight;
	}

	// "滚动条"位置
	int start = rcScrollSpace.top + (m_ScrollHandleHeight>>1);
	int delta = nHeight - m_ScrollHandleHeight;

	RtwWidget *pScrollHandle = getScrollHandle();
	if (delta > 0 && m_Max != m_Min)
	{
		int pos = start + delta * m_Value / (float)(m_Max - m_Min);
		rcScrollHandle.left		= m_rcClient.left;
		rcScrollHandle.right	= m_rcClient.right;
		rcScrollHandle.top		= pos - (m_ScrollHandleHeight>>1);
		rcScrollHandle.bottom	= pos + (m_ScrollHandleHeight>>1);

		if (pScrollHandle)
		{
			pScrollHandle->Show();
			pScrollHandle->Move(rcScrollHandle);
			pScrollHandle->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
		}		
	}
	else 
	{
		rcScrollHandle = rcScrollSpace;
		if (pScrollHandle)
		{
			pScrollHandle->Hide();			
		}
	}
}

RtwRect RtwVScrollBar::getScrollSpaceRect()
{
	RtwRect rcScrollSpace;

	if( m_bHasLeftAndRightButton == false )
	{
		rcScrollSpace.left   = m_rcClient.left;
		rcScrollSpace.right  = m_rcClient.right;
		rcScrollSpace.top    = getFrameRect().top    - 5;
		rcScrollSpace.bottom = getFrameRect().bottom - 1;

		if (rcScrollSpace.bottom < rcScrollSpace.top)
		{
			rcScrollSpace.bottom = rcScrollSpace.top;
		}

	}else
	{
		rcScrollSpace.left	 = m_rcClient.left;
		rcScrollSpace.right  = m_rcClient.right;
		rcScrollSpace.top	 = m_pBtnStepUp->getFrameRect().bottom + 1;
		rcScrollSpace.bottom = m_pBtnStepDown->getFrameRect().top  - 1;

		if (rcScrollSpace.bottom < rcScrollSpace.top)
		{
			rcScrollSpace.bottom = rcScrollSpace.top;
		}
	}


	return rcScrollSpace;
}

void RtwVScrollBar::showHint(std::string m_mNums,RtwWidget* pWidget,RtwVScrollBar* pWidgetV)
{
	pWidget->SetText(m_mNums);
	if (g_workspace.getMouseCapture() == pWidgetV->getScrollHandle())
	{
		SPoint  mm2= g_workspace.getMousePos();
		if(mm2.x<=pWidgetV->getScrollSpaceRect().left)
		{
			mm2.x=pWidgetV->getScrollSpaceRect().left;
		}

		if(mm2.x>pWidgetV->getScrollSpaceRect().right)
		{
			mm2.x=pWidgetV->getScrollSpaceRect().right;
		}

		mm2.y=pWidgetV->getClientRect().top-(pWidget->getClientRect().bottom-pWidget->getClientRect().top);

		pWidget->Move(mm2);
	}
}

int RtwVScrollBar::ValueFromPoint(const SPoint& point)
{
	RtwRect rcScrollSpace = getScrollSpaceRect();
	int top		= rcScrollSpace.top		+ (m_ScrollHandleHeight>>1);
	int bottom	= rcScrollSpace.bottom	- (m_ScrollHandleHeight>>1);

	if (point.y <= top)
	{
		return m_Min;
	}
	else if (point.y >= bottom)
	{
		return m_Max;
	}

	return (point.y - (Real)top) / ( ((Real)bottom - top) / (m_Max - m_Min + 1) ) + m_Min;
}

void RtwVScrollBar::OnMouseMove_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (g_workspace.getMouseCapture() == getScrollHandle())
	{
		SPoint point = g_workspace.getMousePos();
		point.y -= m_OffsetDuringScroll;
		int value = ValueFromPoint(point);
		if (value == m_Value)
			return;

		RtwEventDelegate e(etScroll);
		e.scroll.OrigValue = m_Value;

		m_Value = value;
		CalcClient();
		EvScroll(this, e);
	}
}

void RtwVScrollBar::OnMouseLDown_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	m_OffsetDuringScroll = g_workspace.getMousePos().y - m_pWgtScrollHandle->getFrameRect().getCenter().y;
}

void RtwVScrollBar::OnClick_StepUp(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if (m_Value > m_Min)
	{
		m_Value -= 10;

		if (m_Value <= m_Min)
		{
			m_Value = m_Min;
		}
	}

	CalcClient();

	EvScroll(this, e);
}

void RtwVScrollBar::OnClick_StepDown(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if (m_Value < m_Max)
	{
		m_Value += 10;

		if (m_Value > m_Max)
		{
			m_Value = m_Max;
		}
	}

	CalcClient();

	EvScroll(this, e);
}

void RtwVScrollBar::OnMouseWheel_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if(pEvent)
	{
		if(pEvent->mouseWheel.delta > 0)
		{
			if (m_Value > m_Min)
			{
				m_Value--;
			}
		}
		else
		{
			if (m_Value < m_Max)
			{
				m_Value++;
			}
		}
	}

	CalcClient();
	EvScroll(this, e);
}

void RtwVScrollBar::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if (m_Scale == 1.0)
	{
		return;
	}

	int _Total			= m_Max - m_Min;
	int _ValuePerPage	= _Total * m_Scale;
	RtwRect rcHandle	= m_pWgtScrollHandle->getFrameRect();

	if (g_workspace.getMousePos().y >= rcHandle.bottom)
	{
		m_Value += _ValuePerPage;
	}
	else if (g_workspace.getMousePos().y <= rcHandle.top)
	{
		m_Value -= _ValuePerPage;
	}

	if (m_Value < m_Min)
		m_Value = m_Min;
	else if (m_Value > m_Max)
		m_Value = m_Max;

	CalcClient();

	EvScroll(this, e);
}

RtwHScrollBar::RtwHScrollBar()
{
	m_pBtnStepLeft = NULL;
	m_pBtnStepRight = NULL;
	m_pWgtScrollHandle = NULL;
	m_bHasLeftAndRightButton = true;
}

RtwHScrollBar::~RtwHScrollBar(){}

void RtwHScrollBar::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pBtnStepLeft);
	DROP_RTUI_OBJECT(m_pBtnStepRight);
	DROP_RTUI_OBJECT(m_pWgtScrollHandle);

	RtwWidget::OnBeforeDestroy();
}

void RtwHScrollBar::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwHScrollBar::_ResetSelf()
{
	m_ScrollHandleWidth = m_DefaultScrollHandleWidth;
	m_WidgetType	= wtScrollBarH;
	m_Flag			= m_DefaultFlag;
	m_Min			= 0;
	m_Max			= 100;
	m_Value			= m_Min;
	m_Scale			= 0.2f;
	m_rcClient		= RtwRect(0, 0, 30, m_DefaultHeight);
	m_rcFrame		= RtwRect(0, 0, 30, m_DefaultHeight);
}

bool RtwHScrollBar::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	// Modified by Wayne Wong 2010-12-20
	/*
	m_pBtnStepLeft		= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	m_pBtnStepRight		= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
	m_pWgtScrollHandle	= g_workspace.getWidgetFactory()->createWidget(wtWidget);
	UI_ENSURE_B(m_pBtnStepRight);
	UI_ENSURE_B(m_pBtnStepLeft);
	UI_ENSURE_B(m_pWgtScrollHandle);

	m_pBtnStepLeft->SetName("$BtnLeft");
	AddChild(m_pBtnStepLeft);
	m_pBtnStepLeft->ModifyFlags(0, wfFocus);
	m_pBtnStepLeft->EvLClick += RTW_CALLBACK(this, RtwHScrollBar, OnClick_StepLeft);

	m_pBtnStepRight->SetName("$BtnRight");
	AddChild(m_pBtnStepRight);
	m_pBtnStepRight->ModifyFlags(0, wfFocus);
	m_pBtnStepRight->EvLClick += RTW_CALLBACK(this, RtwHScrollBar, OnClick_StepRight);

	m_pWgtScrollHandle->SetName("$WgtHandle");
	m_pWgtScrollHandle->ModifyFlags(wfGrabMouse | wfMouseMove, 0);
	AddChild(m_pWgtScrollHandle);

	m_pWgtScrollHandle->EvMouseMove += RTW_CALLBACK(this, RtwHScrollBar, OnMouseMove_ScrollHandle);
	m_pWgtScrollHandle->EvMouseLDown += RTW_CALLBACK(this, RtwHScrollBar, OnMouseLDown_ScrollHandle);

	EvMouseLDown += RTW_CALLBACK(this, RtwHScrollBar, OnMouseLDown_This);
	*/

	m_pBtnStepLeft		= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, "$BtnLeft");
	m_pBtnStepRight		= (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, "$BtnRight");
	m_pWgtScrollHandle	= g_workspace.getWidgetFactory()->createWidget(wtWidget, "$WgtHandle");
	UI_ENSURE_B(m_pBtnStepRight);
	UI_ENSURE_B(m_pBtnStepLeft);
	UI_ENSURE_B(m_pWgtScrollHandle);

	//m_pBtnStepLeft->SetName("$BtnLeft");
	AddChild(m_pBtnStepLeft);
	m_pBtnStepLeft->ModifyFlags(0, wfFocus);
	m_pBtnStepLeft->EvLClick += RTW_CALLBACK(this, RtwHScrollBar, OnClick_StepLeft);
	m_pBtnStepLeft->EvMouseCapture += RTW_CALLBACK(this, RtwHScrollBar, OnClick_StepLeft);

	//m_pBtnStepRight->SetName("$BtnRight");
	AddChild(m_pBtnStepRight);
	m_pBtnStepRight->ModifyFlags(0, wfFocus);
	m_pBtnStepRight->EvLClick += RTW_CALLBACK(this, RtwHScrollBar, OnClick_StepRight);
	m_pBtnStepRight->EvMouseCapture += RTW_CALLBACK(this, RtwHScrollBar, OnClick_StepRight);

	//m_pWgtScrollHandle->SetName("$WgtHandle");
	m_pWgtScrollHandle->ModifyFlags(wfGrabMouse | wfMouseMove, 0);
	AddChild(m_pWgtScrollHandle);
	m_pWgtScrollHandle->EvMouseMove += RTW_CALLBACK(this, RtwHScrollBar, OnMouseMove_ScrollHandle);
	m_pWgtScrollHandle->EvMouseLDown += RTW_CALLBACK(this, RtwHScrollBar, OnMouseLDown_ScrollHandle);
	m_pWgtScrollHandle->EvMouseWheel += RTW_CALLBACK(this, RtwHScrollBar, OnMouseWheel_ScrollHandle);

	EvMouseLDown += RTW_CALLBACK(this, RtwHScrollBar, OnMouseLDown_This);

	return true;
}

bool RtwHScrollBar::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwHScrollBar::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwHScrollBar* pOther = (RtwHScrollBar*)pWidget;
	m_Min	= pOther->m_Min;
	m_Max	= pOther->m_Max;
	m_Value = pOther->m_Value;
	m_Scale = pOther->m_Scale;

	if (pOther->m_pBtnStepLeft)
	{
		if (!m_pBtnStepLeft)
		{
			m_pBtnStepLeft = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			UI_ENSURE_B(m_pBtnStepLeft);
		}

		m_pBtnStepLeft->CloneFrom(pOther->m_pBtnStepLeft);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pBtnStepLeft);
	}

	if (pOther->m_pBtnStepRight)
	{
		if (!m_pBtnStepRight)
		{
			m_pBtnStepRight = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			UI_ENSURE_B(m_pBtnStepRight);
		}

		m_pBtnStepRight->CloneFrom(pOther->m_pBtnStepRight);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pBtnStepRight);
	}

	if (pOther->m_pWgtScrollHandle)
	{
		if (!m_pWgtScrollHandle)
		{
			m_pWgtScrollHandle = g_workspace.getWidgetFactory()->createWidget(wtWidget);
			UI_ENSURE_B(m_pWgtScrollHandle);
		}
		m_pWgtScrollHandle->CloneFrom(pOther->m_pWgtScrollHandle);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pWgtScrollHandle);
	}

	m_ScrollHandleWidth	= pOther->m_ScrollHandleWidth;
	m_rcLeftButton		= pOther->m_rcLeftButton;
	m_rcRightButton		= pOther->m_rcRightButton;
	m_rcScrollSpace		= pOther->m_rcScrollSpace;
	m_rcScrollHandle	= pOther->m_rcScrollHandle;

	return true;
}

void RtwHScrollBar::SetData(int min/* = -1*/, int max/* = -1*/, int value/* = -1*/, Real scale/* = -1.0*/)
{
	if (min >= 0)
	{
		m_Min = min;
	}

	if (max >= 0)
	{
		m_Max = max;
	}

	if (value >= 0)
	{
		m_Value = value;
	}

	if (scale >= 0.0)
	{
		m_Scale = scale;
	}

	CalcClient();
}

void RtwHScrollBar::SetMin(int min)
{
	m_Min = min;
	if (m_Max < m_Min)
	{
		m_Max = m_Min;
	}

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}

	CalcClient();
}

void RtwHScrollBar::SetMax(int max)
{
	m_Max = max;
	if (m_Min > m_Max)
	{
		m_Min = m_Max;
	}

	if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();
}

void RtwHScrollBar::SetValue(int value)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;
	m_Value = value;

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}
	else if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();
	EvScroll(this, etScroll);
}

void RtwHScrollBar::SetScale(float scale)
{
	m_Scale = scale;

	if (m_Scale < 0.01f)
	{
		m_Scale = 0.01f;
	}
	else if (m_Scale > 100.0f)
	{
		m_Scale = 100.0f;
	}

	CalcClient();
}

void  RtwHScrollBar::showHint(std::string m_Nums,RtwWidget* pWidget,RtwHScrollBar* pWidgetV)
{
	pWidget->SetText(m_Nums);
	int nCurrentValue  = m_Value;
	int nTotal         = m_Max - m_Min;

	if( nTotal <= 0 ) 
	{
		nTotal = 1;
	}

	float fFactor = (float)nCurrentValue / nTotal;
	fFactor = fFactor < 0.0f ? 0.0f : fFactor;
	fFactor = fFactor > 1.0f ? 1.0f : fFactor;

	RtwRect rcScrollSpace = getScrollSpaceRect();	
	SPoint  mm2;
	mm2.x	= rcScrollSpace.left + fFactor * rcScrollSpace.getWidth();
	mm2.y	= pWidgetV->getClientRect().top-(pWidget->getClientRect().bottom-pWidget->getClientRect().top);
	pWidget->Move(mm2);
}

void RtwHScrollBar::ScrollToLeft()
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;
	m_Value = m_Min;
	CalcClient();
	EvScroll(this, etScroll);
}

void RtwHScrollBar::ScrollToRight()
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;
	m_Value = m_Max;
	CalcClient();
	EvScroll(this, etScroll);
}

void RtwHScrollBar::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);
}

void RtwHScrollBar::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

void RtwHScrollBar::CalcClient()
{
	m_rcClient = m_rcFrame;

	int height = m_rcClient.bottom - m_rcClient.top;

	RtwRect rcLeftButton, rcRightButton, rcScrollSpace, rcScrollHandle;

	// "向左"按钮位置
	rcLeftButton.left	= m_rcClient.left;
	rcLeftButton.right	= m_rcClient.left + height;
	rcLeftButton.top	= m_rcClient.top;
	rcLeftButton.bottom = m_rcClient.bottom;

	if (rcLeftButton.right > m_rcClient.right)
	{
		rcLeftButton.right = m_rcClient.right;
	}

	if (getStepLeftButton())
	{
		getStepLeftButton()->Move(rcLeftButton);
		getStepLeftButton()->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
	}

	// "向右"按钮位置
	rcRightButton.left		= m_rcClient.right - height;
	rcRightButton.right		= m_rcClient.right;
	rcRightButton.top		= m_rcClient.top;
	rcRightButton.bottom	= m_rcClient.bottom;

	if (rcRightButton.left < m_rcClient.left)
	{
		rcRightButton.left = m_rcClient.left;
	}

	if (getStepRightButton())
	{
		getStepRightButton()->Move(rcRightButton);
		getStepRightButton()->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
	}

	// "滚动空间"位置
	rcScrollSpace = getScrollSpaceRect();

	// "滚动条"宽度
	int nWidth = rcScrollSpace.getWidth();
	m_ScrollHandleWidth = m_Scale * nWidth;

	if (m_ScrollHandleWidth < m_DefaultScrollHandleWidth)
	{
		m_ScrollHandleWidth = m_DefaultScrollHandleWidth;
	}
	else if (m_ScrollHandleWidth > nWidth)
	{
		m_ScrollHandleWidth = nWidth;
	}

	// "滚动条"位置
	int start = rcScrollSpace.left + (m_ScrollHandleWidth>>1);
	int delta = nWidth - m_ScrollHandleWidth;

	RtwWidget *pScrollHandle = getScrollHandle();
	if (delta > 0 && m_Max != m_Min)
	{
		int pos = start + delta * m_Value / (float)(m_Max - m_Min);
		rcScrollHandle.left		= pos - (m_ScrollHandleWidth>>1);
		rcScrollHandle.right	= pos + (m_ScrollHandleWidth>>1);
		rcScrollHandle.top		= m_rcClient.top;
		rcScrollHandle.bottom	= m_rcClient.bottom;

		if (pScrollHandle)
		{
			pScrollHandle->Show();
			pScrollHandle->Move(rcScrollHandle);
			pScrollHandle->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
		}
	}
	else 
	{
		rcScrollHandle = rcScrollSpace;
		if (pScrollHandle)
		{
			pScrollHandle->Hide();			
		}
	}

	//if (getScrollHandle())
	//{
	//	getScrollHandle()->Move(rcScrollHandle);
	//	getScrollHandle()->Offset(SSize(-m_rcClient.left, -m_rcClient.top));
	//	getScrollHandle()->SetWidgetSize(SSize(6,12));
	//	getScrollHandle()->SetBorderColor(RtwPixel(0xccd6c384));
	//	getScrollHandle()->SetBorderSize(5);
	//}
}

RtwRect RtwHScrollBar::getScrollSpaceRect()
{
	RtwRect rcScrollSpace;

	if( false == m_bHasLeftAndRightButton )
	{
		// Modified by Wayne wong 2010-12-20
		//rcScrollSpace.left   = m_rcClient.left + 2 ;
		//rcScrollSpace.right  = m_rcClient.right + 18 ;
		//rcScrollSpace.top    = m_rcClient.top - 2;
		//rcScrollSpace.bottom = m_rcClient.bottom + 2;

		rcScrollSpace.left   = m_rcClient.left - 5;
		rcScrollSpace.right  = m_rcClient.right - 1;
		rcScrollSpace.top    = m_rcClient.top;
		rcScrollSpace.bottom = m_rcClient.bottom;

		if (rcScrollSpace.right < rcScrollSpace.left)
		{
			rcScrollSpace.right = rcScrollSpace.left;
		}
		m_pBtnStepLeft->Hide();
		m_pBtnStepRight->Hide();
	}
	else
	{
		rcScrollSpace.left	= m_pBtnStepLeft->getFrameRect().right + 1;
		rcScrollSpace.right = m_pBtnStepRight->getFrameRect().left - 1;
		rcScrollSpace.top	= m_rcClient.top;
		rcScrollSpace.bottom= m_rcClient.bottom;

		if (rcScrollSpace.right < rcScrollSpace.left)
		{
			rcScrollSpace.right = rcScrollSpace.left;
		}
		m_pBtnStepLeft->Show();
		m_pBtnStepRight->Show();
	}


	return rcScrollSpace;
}

int RtwHScrollBar::ValueFromPoint(const SPoint& point)
{
	RtwRect rcScrollSpace = getScrollSpaceRect();
	int left	= rcScrollSpace.left  + (m_ScrollHandleWidth>>1);
	int right	= rcScrollSpace.right - (m_ScrollHandleWidth>>1);

	if (point.x <= left)
	{
		return m_Min;
	}
	else if (point.x >= right)
	{
		return m_Max;
	}

	return (point.x - (Real)left) / ( ((Real)right - left) / (m_Max - m_Min + 1) ) + m_Min;
}

void RtwHScrollBar::OnMouseMove_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (g_workspace.getMouseCapture() == getScrollHandle())
	{
		SPoint point = g_workspace.getMousePos();
		point.x	-= m_OffsetDuringScroll;
		int value  = ValueFromPoint(point);
		if (value == m_Value)
			return;

		RtwEventDelegate e(etScroll);
		e.scroll.OrigValue = m_Value;

		m_Value = value;
		CalcClient();
		EvScroll(this, e);
	}
}

void RtwHScrollBar::OnMouseLDown_ScrollHandle(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	m_OffsetDuringScroll = g_workspace.getMousePos().x - m_pWgtScrollHandle->getFrameRect().getCenter().x;
}

void RtwHScrollBar::OnClick_StepLeft(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if (m_Value > m_Min)
	{
		// Modified by Wayne Wong 2010-12-20
		//m_Value--;
		m_Value -= 10;
		if (m_Value <= m_Min)
		{
			m_Value = m_Min;
		}
	}

	CalcClient();

	EvScroll(this, etScroll);
}

void RtwHScrollBar::OnClick_StepRight(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if (m_Value < m_Max)
	{
		// Modified by Wayne Wong 2010-12-20
		//m_Value++;
		m_Value += 10;

		if (m_Value > m_Max)
		{
			m_Value = m_Max;
		}
	}

	CalcClient();

	EvScroll(this, etScroll);
}

void RtwHScrollBar::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if (m_Scale == 1.0)
	{
		return;
	}

	int		_Total			= m_Max - m_Min;
	int		_ValuePerPage	= _Total * m_Scale;
	RtwRect	rcHandle		= m_pWgtScrollHandle->getFrameRect();

	if (g_workspace.getMousePos().x >= rcHandle.right)
	{
		m_Value += _ValuePerPage;
	}
	else if (g_workspace.getMousePos().x <= rcHandle.left)
	{
		m_Value -= _ValuePerPage;
	}

	if (m_Value < m_Min)
	{
		m_Value = m_Min;
	}
	else if (m_Value > m_Max)
	{
		m_Value = m_Max;
	}

	CalcClient();

	EvScroll(this, e);
}

void ui::RtwHScrollBar::OnMouseWheel_ScrollHandle( RtwWidget* pWidget, RtwEventDelegate* pEvent )
{
	RtwEventDelegate e(etScroll);
	e.scroll.OrigValue = m_Value;

	if(pEvent)
	{
		if(pEvent->mouseWheel.delta > 0)
		{
			if (m_Value > m_Min)
			{
				m_Value--;
			}
		}
		else
		{
			if (m_Value < m_Max)
			{
				m_Value++;
			}
		}
	}

	CalcClient();
	EvScroll(this, e);
}
