#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwForm, RtwWidget, 0, "ui")

RtwForm::RtwForm():m_pTitleBarImage(NULL), m_pBtn_Close(NULL), m_bMode(false),m_bEnableEscapeHide(false)
{
	m_Flag			 |= wfDClick;
}

RtwForm::~RtwForm(){}

void RtwForm::OnBeforeDestroy()
{
	_ResetSelf();

	DROP_RTUI_OBJECT(m_pBtn_Close);

	RtwWidget::OnBeforeDestroy();
}

void RtwForm::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwForm::_ResetSelf()
{
	m_WidgetType = wtForm;
	m_Flag = m_DefaultFlag;
	m_bShowTitleBar = true;
	m_TitleBarHeight = 0;
	m_bCanMove = true;
	m_bCanResize = false;
	m_bEnableEscapeHide = false;

	DROP_TRUI_IMAGE(m_pTitleBarImage);
}

bool RtwForm::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	m_pBtn_Close = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, "$Close");
	UI_ENSURE_B(m_pBtn_Close);
	AddChild(m_pBtn_Close);
	m_pBtn_Close->Show();
	m_pBtn_Close->SetJoinParentAutoScroll(false);
 	m_pBtn_Close->EvLClick += RTW_CALLBACK(this, RtwForm, OnClick_Close);
 	EvMouseMove += RTW_CALLBACK(this, RtwForm, OnMouseMove_This);

	return true;
}

void RtwForm::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	CalcClient();
}


bool RtwForm::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwForm::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwForm* pOther = (RtwForm*)pWidget;

	m_bShowTitleBar = pOther->m_bShowTitleBar;
	m_TitleBarHeight = pOther->m_TitleBarHeight;
	m_bCanMove = pOther->m_bCanMove;
	m_bCanResize = pOther->m_bCanResize;

	m_rcTitle = pOther->m_rcTitle;
	m_TitleText = pOther->m_TitleText;

	DROP_TRUI_IMAGE(m_pTitleBarImage);
	if (pOther->m_pTitleBarImage)
	{
		m_pTitleBarImage = pOther->m_pTitleBarImage;
		GRAB_TRUI_IMAGE(m_pTitleBarImage);
	}

	if (pOther->m_pBtn_Close)
	{
		if (!m_pBtn_Close)
		{
			m_pBtn_Close = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton);
			UI_ENSURE_B(m_pBtn_Close);
		}
		m_pBtn_Close->CloneFrom(pOther->m_pBtn_Close);
	}
	else
	{
		DROP_RTUI_OBJECT(m_pBtn_Close);
	}

	return true;
}

void RtwForm::SetTitleText(const std::string& text)
{
	m_TitleText = text;
}

void RtwForm::SetTitleImage(RtwImage* pImage)
{
	DROP_TRUI_IMAGE(m_pTitleBarImage);
	m_pTitleBarImage = pImage;
	if (pImage)
	{
		GRAB_TRUI_IMAGE(pImage);
	}
}

void RtwForm::SetShowTitleBar(bool bShow, bool bAdjectOtherPosition/* = true*/)
{
	if (m_bShowTitleBar == bShow)
		return;

	m_bShowTitleBar = bShow;

	CalcClient();

	if (bAdjectOtherPosition)
	{
		SSize OffsetSize;
		if (m_bShowTitleBar)
		{
			OffsetSize.width = 0;
			OffsetSize.height = 10;
		}
		else 
		{
			OffsetSize.width = 0;
			OffsetSize.height = -10;
		}
		
		if (m_pChildren)
		{
			for (UiWidgetList::iterator iterWidget = m_pChildren->m_Widgets.begin(); iterWidget != m_pChildren->m_Widgets.end(); ++iterWidget)
			{
				RtwWidget* pWidget = *iterWidget;
				pWidget->Offset(OffsetSize);
			}
		}
	}

	if (!m_bShowTitleBar)
	{
		m_pBtn_Close->Hide();
	}
}

void RtwForm::SetShowCloseButton(bool bShow)
{
	if (!m_pBtn_Close || m_pBtn_Close->getVisible() == bShow)
		return;

	if (bShow)
		m_pBtn_Close->Show();
	else 
		m_pBtn_Close->Hide();
}

void RtwForm::SetTitleBarHeight(int height)
{
	int delta = height - m_TitleBarHeight;
	if (delta == 0)
		return;

	m_TitleBarHeight = height;

	CalcClient();

	SSize OffsetSize(0, delta);

	if (m_pChildren)
	{
		for (UiWidgetList::iterator iterWidget = m_pChildren->m_Widgets.begin(); iterWidget != m_pChildren->m_Widgets.end(); ++iterWidget)
		{
			RtwWidget* pWidget = *iterWidget;
			pWidget->Offset(OffsetSize);
		}
	}
	//m_rcFrame.bottom += 24;//ͼƬԭ��
}

void RtwForm::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);
}

void RtwForm::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwRect bgRect;

	//����form����ɫ�Ҵ������⡣��� 2010.01.04

	//if (pClipRect)
	//{
	//	bgRect.left = pClipRect->left + (m_BorderSize + m_padding.left);
	//	bgRect.right = pClipRect->right - (m_BorderSize + m_padding.right);
	//	bgRect.bottom = pClipRect->bottom - (m_BorderSize + m_padding.bottom);
	//	bgRect.top = pClipRect->top + (m_BorderSize);
	//}
	//else
	{
		bgRect.bottom = m_rcFrame.bottom - (m_BorderSize + m_padding.bottom);
		bgRect.top = m_rcFrame.top + (m_BorderSize);
		bgRect.left = m_rcFrame.left + (m_BorderSize + m_padding.left);
		bgRect.right = m_rcFrame.right - (m_BorderSize + m_padding.right);
		if (pClipRect) bgRect.ClipBy(*pClipRect);
	}
	if (m_BkColor != 0)
	{
		g_workspace.getRenderDevice()->DrawRectangle(RtwRect(bgRect.left,bgRect.top+m_padding.top,bgRect.right,bgRect.bottom), SETCOLORALPHA(m_BkColor, m_Transparency));
	}


// 	if (m_bShowTitleBar)
// 		bgRect.top += m_TitleBarHeight;
	RtwWidget::DrawBackground(&bgRect);

	// Draw TitleBar
	if (m_bShowTitleBar)
	{
		if (m_bShowTitleBar)
		{
			if (m_pTitleBarImage)
			{
				RtwImage *pImage = m_pTitleBarImage->getRenderImage();
				if (pImage)
				{
					pImage->SetCtrlTransparent(m_Transparency);
					g_workspace.getRenderDevice()->DrawImage(pImage, m_rcTitle, SUiTextAlignment(alignNear, alignNear), &bgRect);
				}
			}
// 			if (!m_TitleText.empty())
// 			{
// 				RtwRect rcTitleDraw(m_rcTitle.left + 8, m_rcTitle.top, m_rcTitle.right, m_rcTitle.bottom);
// 				g_workspace.getFontManager()->DrawString(m_TitleText, rcTitleDraw, RtwPixel(0xffffffff), false, SUiTextAlignment(alignCenter, alignCenter), false, &bgRect);
// 			}
		}
	}
}

void RtwForm::CalcClient()
{
	m_rcTitle.left   = m_rcFrame.left	 + (m_BorderSize);
	m_rcTitle.right  = m_rcFrame.right	 - (m_BorderSize);
	m_rcTitle.top    = m_rcFrame.top	 + (m_BorderSize);
	m_rcTitle.bottom = m_rcFrame.bottom - (m_BorderSize);
	if (m_bShowTitleBar)
		m_rcTitle.bottom = m_rcFrame.top + m_TitleBarHeight;
	else 
		m_rcTitle.bottom = m_rcFrame.top;

	m_rcClient.left   = m_rcFrame.left	 + (m_BorderSize + m_padding.left);
	m_rcClient.right  = m_rcFrame.right	 - (m_BorderSize + m_padding.right);
	m_rcClient.top    = m_rcFrame.top	 + (m_BorderSize + m_padding.top);
	m_rcClient.bottom = m_rcFrame.bottom - (m_BorderSize + m_padding.bottom);
	if (m_rcClient.bottom < m_rcClient.top)
		m_rcClient.bottom = m_rcClient.top;

	if (m_pBtn_Close && m_bShowTitleBar)
	{
		RtwRect& rcCloseBtnFrame = m_pBtn_Close->GetFrameRect();
		RtwRect rcCloseBtn;
		SMargin rcBtn =m_pBtn_Close->GetPadding();
		rcCloseBtn.right = m_rcTitle.right - rcBtn.right;
		rcCloseBtn.left = rcCloseBtn.right - rcCloseBtnFrame.getWidth();
		rcCloseBtn.top = m_rcTitle.top + rcBtn.top;
		rcCloseBtn.bottom = rcCloseBtn.top + rcCloseBtnFrame.getHeight();

		m_pBtn_Close->Move(rcCloseBtn, true);
	}
	if (m_bAutoScrollV || m_bAutoScrollH)
		CalcAutoScroll();
}

void RtwForm::OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	// ����Ƿ������ƶ���������������˳�����    ע�Ͳ��ֿ����Ƿ���ʾ������������
	if (!m_bCanMove/* || !m_bShowTitleBar*/)
		return;
	//�����ǰ��������겶���ߣ��������������ƶ���
	if (g_workspace.getMouseCapture() == this)
	{
		// ��ȡ����ƶ�������
		const SSize& MouseDelta = g_workspace.getMouseDelta();
		// ��ȡ��ǰ����λ��
		const SPoint& MousePoint = g_workspace.getMousePos();
		// ��������ƶ�ǰ��λ��
		SPoint MouseLastPoint;
		MouseLastPoint.x = MousePoint.x - MouseDelta.width;
		MouseLastPoint.y = MousePoint.y - MouseDelta.height;
		// �����Ҫ�������������������һ��λ���Ƿ��ڱ�������
// 		if (m_rcTitle.IsContain(MouseLastPoint))
		{
			// ���� Offset ��������������λ�ã���������ƶ������������ƶ�
			Offset(MouseDelta);
		}

		// ��ֹ�ƶ�����Ļ����
 		SSize AdjectSize;

		// ��ȡ�ӿڵľ�������
 		RtwRect rcViewport = g_workspace.getViewportRect();
		// ��鴰���ұ��Ƿ񳬳��ӿڵ���߽磬����������������Ҫ�����Ŀ��
 		if (m_rcFrame.right < 10)
 		{
 			AdjectSize.width = 10 - m_rcFrame.right;
 		}
		// ��鴰������Ƿ񳬳��ӿڵ��ұ߽磬����������������Ҫ�����Ŀ��
 		else if (m_rcFrame.left > rcViewport.right - 10)
 		{
 			AdjectSize.width = rcViewport.right - 10 - m_rcFrame.left;
 		}
		// ��鴰�ڶ����Ƿ񳬳��ӿڵĵײ��߽磬����������������Ҫ�����ĸ߶�
 		if (m_rcFrame.top < -5)
 		{
 			AdjectSize.height = -5 - m_rcFrame.top;
 		}
		// ��鴰�ڵײ��Ƿ񳬳��ӿڵĶ����߽磬����������������Ҫ�����ĸ߶�
 		else if (m_rcFrame.top > rcViewport.bottom - 10)
 		{
 			AdjectSize.height = rcViewport.bottom - 10 - m_rcFrame.top;
 		}
		// �����Ҫ�����Ŀ�Ȼ�߶Ȳ�Ϊ�㣬����� Offset �������е���
 		if (AdjectSize.width != 0 || AdjectSize.height != 0)
 			Offset(AdjectSize);
	}
}

void RtwForm::OnClick_Close(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	if (getCloseButton()->getFlag(wfVisible,false))
	{
		Hide(/*esTrans|esZoom*/);
	}
}

void RtwForm::OnRemoveChild(RtwWidget* pWidget)
{
	if (pWidget == m_pBtn_Close)
		DROP_RTUI_OBJECT(m_pBtn_Close);
}

void RtwForm::Show()
{
	g_workspace.InsertFocusWidget(this);
	RtwWidget::Show();

// 	SSize AdjectSize;
// 	RtwRect rcViewport = g_workspace.getViewportRect();
// 	if (m_rcFrame.right < 10)
// 	{
// 		AdjectSize.width = 10 - m_rcFrame.right;
// 	}
// 	else if (m_rcFrame.left > rcViewport.right - 10)
// 	{
// 		AdjectSize.width = rcViewport.right - 10 - m_rcFrame.left;
// 	}
// 	if (m_rcFrame.top < -5)
// 	{
// 		AdjectSize.height = -5 - m_rcFrame.top;
// 	}
// 	else if (m_rcFrame.top > rcViewport.bottom - 10)
// 	{
// 		AdjectSize.height = rcViewport.bottom - 10 - m_rcFrame.top;
// 	}
// 
// 	if (AdjectSize.width != 0 || AdjectSize.height != 0)
// 		Offset(AdjectSize);
}	

void RtwForm::Hide()
{
	g_workspace.DeleteFocusWidget(this);
	RtwWidget::Hide();
}
