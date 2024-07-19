#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwProgressBar, RtwWidget, 0, "ui")

RtwProgressBar::RtwProgressBar():m_pImgBar(NULL), m_Direction(1){}

RtwProgressBar::~RtwProgressBar(){}

void RtwProgressBar::OnBeforeDestroy()
{
	_ResetSelf();
	RtwWidget::OnBeforeDestroy();
}

void RtwProgressBar::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

void RtwProgressBar::_ResetSelf()
{
	m_WidgetType	= wtProgressBar;
	m_Flag			= m_DefaultFlag;
	m_rcClient		= RtwRect(0, 0, 30, 10);
	m_rcFrame		= RtwRect(0, 0, 30, 10);
	m_TextAlign		= SUiTextAlignment(alignCenter, alignCenter);
	m_Value			= 1.0f;//1.0f;
	DROP_TRUI_IMAGE(m_pImgBar);
}

bool RtwProgressBar::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	CalcClient();

	return true;
}

void RtwProgressBar::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	CalcClient();
}


void RtwProgressBar::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	if (m_pImgBar)
	{
		m_pImgBar->getRenderImage()->SetCtrlTransparent(m_Transparency);
		g_workspace.getRenderDevice()->DrawImage(m_pImgBar->getRenderImage(), m_rcBar, SUiTextAlignment(alignNear, alignNear), pClipRect);
	}

	if (!m_Text.empty())
		g_workspace.getFontManager()->DrawString(m_Text, m_rcFrame, SETCOLORALPHA(m_TextColor, m_Transparency), true, m_TextAlign, false, pClipRect);

	RtwWidget::DrawForeground(pClipRect);
}

void RtwProgressBar::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool RtwProgressBar::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwProgressBar::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	RtwProgressBar* pOther = (RtwProgressBar*)pWidget;

	m_Value = pOther->m_Value;
	DROP_TRUI_IMAGE(m_pImgBar);
	if (pOther->m_pImgBar)
	{
		m_pImgBar = pOther->m_pImgBar;
		GRAB_TRUI_IMAGE(m_pImgBar);
	}
	m_rcBar = pOther->m_rcBar;

	return true;
}

void RtwProgressBar::SetValue(Real value)
{
	m_Value = value;
	if (m_Value < 0.0)
		m_Value = 0.0;
	else if (m_Value > 1.0)
		m_Value = 1.0;

	Refresh();
}

void RtwProgressBar::CalcClient()
{
	m_rcClient = m_rcFrame;

	m_rcBar = m_rcClient;
	if (m_Direction == 2)
	{
		m_rcBar.bottom = m_rcClient.top + m_Value * m_rcClient.getHeight();
	}
	else
		m_rcBar.right = m_rcClient.left + m_Value * m_rcClient.getWidth();
}

void RtwProgressBar::SetBarImage(RtwImage* pImage)
{
	DROP_TRUI_IMAGE(m_pImgBar);

	if (pImage)
	{
		m_pImgBar = pImage;
		GRAB_TRUI_IMAGE(m_pImgBar);
	}
}

void RtwProgressBar::Run(DWORD dwDelta)
{
	IMAGETICK(m_pImgBar, dwDelta);
	RtwWidget::Run(dwDelta);
}

void RtwProgressBar::OnRemoveChild(RtwWidget* pWidget){}
