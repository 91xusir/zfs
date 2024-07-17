//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(CUiMediaFrame, RtwWidget, 0, "ui")

CUiMediaFrame::CUiMediaFrame()
{
}

CUiMediaFrame::~CUiMediaFrame()
{
}

void CUiMediaFrame::OnBeforeDestroy()
{
	_ResetSelf();
	RtwWidget::OnBeforeDestroy();
}

void CUiMediaFrame::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

bool CUiMediaFrame::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();

	return true;
}


void CUiMediaFrame::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawForeground(pClipRect);
}

void CUiMediaFrame::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool CUiMediaFrame::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(CUiMediaFrame::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	return true;
}

void CUiMediaFrame::CalcClient()
{
	m_rcClient = m_rcFrame;
	m_rcClient.Inflate(-2);
}

void CUiMediaFrame::_ResetSelf()
{
	m_WidgetType = wtMediaFrame;
	m_Flag = m_DefaultFlag;
	m_TextAlign = SUiTextAlignment(alignNear, alignNear);
	m_NextHyperLink.Reset();
}

void CUiMediaFrame::SetNextHyperLink(SHyperLink* pHLink)
{
	if (!pHLink)
	{
		m_NextHyperLink.Type = hlinkInvalid;
	}
	else 
	{
		m_NextHyperLink = *pHLink;
		g_workspace.SetRefreshMediaFrame(this);
	}
}

void CUiMediaFrame::ProcessNextHyperLink()
{
	switch (m_NextHyperLink.Type)
	{
	case hlinkFile:
		{
			for (UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin(); iterChild != m_pChildren->m_Widgets.end(); ++iterChild)
			{
				RtwWidget* pWidget = *iterChild;
				if (pWidget == m_pAutoScrollV || pWidget == m_pAutoScrollH)
					continue;

				pWidget->Hide();
			}
			RemoveAllChildren();

			if (g_workspace.getXmlLoader()->LoadFromFile(m_NextHyperLink.Text, this))
			{
			}
		}
		break;
	case hlinkWidget:
		{
			RtwWidget* pWidget = NULL;
			if (g_workspace.FindWidget(m_NextHyperLink.Text, &pWidget))
			{
				for (UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin(); iterChild != m_pChildren->m_Widgets.end(); ++iterChild)
				{
					RtwWidget* pWidget = *iterChild;
					if (pWidget == m_pAutoScrollV || pWidget == m_pAutoScrollH)
						continue;

					pWidget->Hide();
				}
				RemoveAllChildren();

				AddChild(pWidget->getChildren());

				DROP_RTUI_OBJECT(pWidget);
			}
		}
		break;
	case hlinkString:
		{
			RtwEventDelegate e(etHyperLink);
			e.hyperLink.pHLink = &m_NextHyperLink;
			EvHyperLink(this, e);
		}
		break;
	case hlinkURL:
		{
			RtwEventDelegate e(etHyperLink);
			e.hyperLink.pHLink = &m_NextHyperLink;
			string url = "http://";
			url += e.hyperLink.pHLink->Text;
			ShellExecute(NULL, "open", "iexplore.exe", url.c_str(), NULL, SW_SHOWNORMAL);
		}
		break;
	default:
		break;
	}

	m_NextHyperLink.Reset();
}

void CUiMediaFrame::OnRemoveChild(RtwWidget* pWidget)
{
}

