#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwLabel, RtwWidget, 0, "ui")

RtwLabel::RtwLabel() {
    m_bIsUseSystemFont = true;
}

RtwLabel::~RtwLabel() {}

void RtwLabel::OnBeforeDestroy() {
    _ResetSelf();
    RtwWidget::OnBeforeDestroy();
}

void RtwLabel::Reset() {
    RtwWidget::Reset();
    _ResetSelf();
}

bool RtwLabel::Create(DWORD Id) {
    if (!RtwWidget::Create(Id))
        return false;

    _ResetSelf();

    EvLClick += RTW_CALLBACK(this, RtwLabel, OnLClick_This);

    return true;
}

void RtwLabel::DrawForeground(const RtwRect* pClipRect /* = NULL*/) {
    if (!m_Text.empty()) {
        RtwPixel RenderColor = m_TextColor;
        bool     bRenderUnderline = m_bTextUnderline;
        if (getHyperLink()) {
            if (g_workspace.getMouseHover() == this)
                RenderColor = g_workspace.getHyperLinkDefaultStyle().HighlightColor;
            else {
                if (m_TextColor != 0xffffffff)
                    RenderColor = g_workspace.getHyperLinkDefaultStyle().NormalColor;
            }
            bRenderUnderline = g_workspace.getHyperLinkDefaultStyle().bUnderLine;
        }
        if (m_bIsUseSystemFont && m_FontType.empty()) {
            //tim.yang 修改Label字体的大小
            RtwFont* pkFont = g_workspace.getFontManager()->getDefaultFont();
            ;
            /*RtwFont*  pkFont = NULL;
			switch( m_FontSize ) 
			{
			default:
 			case 12:
				{
					pkFont = g_workspace.getFontManager()->getDefaultFont();
				}
				break;
 			case 14:
 				{
 					pkFont = g_workspace.getFontManager()->getUISystemFont14();
 				}
 				break;
 			case 16:
 				{
 					pkFont = g_workspace.getFontManager()->getUISystemFont16();
 				}
 				break;
			}*/
            g_workspace.getFontManager()->DrawString(m_Text, m_rcClient,
                                                     SETCOLORALPHA(RenderColor, m_Transparency), pkFont, true,
                m_TextAlign, bRenderUnderline, pClipRect);
            //end
        } else {
            g_workspace.getFontManager()->DrawString(
                m_Text, m_rcClient, SETCOLORALPHA(RenderColor, m_Transparency), true, m_TextAlign,
                bRenderUnderline, pClipRect, m_FontType, m_FontSize);
        }

        if (this->getParent())
            if (this->getParent()->getParent()) {
                if (this->getParent()->getParent()->getWidgetType() == wtComboBox) {
                    g_workspace.getFontManager()->DrawString(
                        m_Text, m_rcClient, SETCOLORALPHA(RenderColor, m_Transparency), true,
                        m_TextAlign, bRenderUnderline, &(this->getParent()->getFrameRect()));
                }
            }
    }

    RtwWidget::DrawForeground(pClipRect);
}

void RtwLabel::DrawBackground(const RtwRect* pClipRect /* = NULL*/) {

    RtwWidget::DrawBackground(pClipRect);
}

bool RtwLabel::CloneFrom(const RtwWidget* pWidget) {
    UI_ENSURE_B(pWidget->IsKindOf(RtwLabel::GetRuntimeClass()));

    UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

    return true;
}

void RtwLabel::CalcClient() {
    m_rcClient = m_rcFrame;
    m_rcClient.Inflate(-2);
}

void RtwLabel::_ResetSelf() {
    m_WidgetType = wtLabel;
    m_Flag = m_DefaultFlag;
    m_TextAlign = SUiTextAlignment(alignNear, alignNear);
    RemoveAllChildren();
}

void RtwLabel::SetHyperLink(SHyperLink* pHyperLink) {
    if (!pHyperLink) {
        m_HyperLink.Type = hlinkInvalid;
        ModifyFlag(0, wfGrabMouse | wfMouseMove);
    } else {
        m_HyperLink = *pHyperLink;
        ModifyFlag(wfGrabMouse | wfMouseMove, 0);
    }
}

void RtwLabel::OnLClick_This(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    if (getHyperLink()) {
        CUiMediaFrame* pFrame = static_cast<CUiMediaFrame*>(g_workspace.getWidgetFactory()->createWidget(wtMediaFrame));
        if (pFrame) {
            pFrame->SetNextHyperLink(&m_HyperLink);
        }
    }
}

void RtwLabel::OnRemoveChild(RtwWidget* pWidget) {}
