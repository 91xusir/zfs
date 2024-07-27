#include "ui/rtw_ui.h"

CUiLayer::CUiLayer() {}

CUiLayer::~CUiLayer() {}

void CUiLayer::OnBeforeDestroy() {
    _ResetSelf();
    RtwWidget::OnBeforeDestroy();
}

void CUiLayer::Reset() {
    RtwWidget::Reset();
    _ResetSelf();
}

bool CUiLayer::Create(DWORD Id) {
    if (!RtwWidget::Create(Id))
        return false;

    _ResetSelf();

    return true;
}

void CUiLayer::Run(DWORD dwDelta) {
    if (!getVisible())
        return;

    if (m_pChildren) {
        for (UiWidgetList::iterator iterWidget = m_pChildren->m_Widgets.begin();
             iterWidget != m_pChildren->m_Widgets.end(); ++iterWidget) {
            RtwWidget* pWidget = *iterWidget;
            pWidget->Run(dwDelta);
        }
    }
}

void CUiLayer::Render(CUiWidgetContainer* pExcludeWidgets /* = NULL*/) {
    if (!getVisible())
        return;

    _SortChildrenForRender();

    if (m_pChildren) {
        for (UiWidgetList::reverse_iterator iterWidget = m_pChildren->m_Widgets.rbegin();
             iterWidget != m_pChildren->m_Widgets.rend(); ++iterWidget) {
            RtwWidget* pWidget = *iterWidget;

            if (pExcludeWidgets && pExcludeWidgets->FindWidget(pWidget->getId()))
                continue;

            pWidget->Render();
        }
    }
}

void CUiLayer::PrintToConsole(const std::string& prefix /* = ""*/) {
    RtCoreLog().Info("[Layer %s]\n", getName().c_str());
    std::string            prefixWidget = prefix + "    ";
    UiWidgetList::iterator iter = WidgetBegin();
    for (; iter != WidgetEnd(); ++iter) {
        RtwWidget* pWidget = *iter;
        pWidget->PrintToConsole(prefixWidget);
    }
    RtCoreLog().Info("\n");
}

bool CUiLayer::PickWidget(const SPoint& Position, RtwWidget** ppWidget /* = NULL*/, DWORD FlagMask /* = 0x00000000*/,
                          CUiWidgetContainer* pExcludeWidgets /* = NULL*/) {
    if (ppWidget) {
        if (RtwWidget::PickWidget(Position, ppWidget, FlagMask, pExcludeWidgets)) {
            if (*ppWidget == this) {
                DROP_RTUI_OBJECT(*ppWidget);
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    } else {
        RtwWidget* pWidget = NULL;
        if (RtwWidget::PickWidget(Position, &pWidget, FlagMask, pExcludeWidgets)) {
            pWidget->drop();
            if (pWidget == this)
                return false;
            else
                return true;
        } else {
            return false;
        }
    }
}

bool CUiLayer::AddWidget(RtwWidget* pWidget) {
    return AddChild(pWidget);
}

bool CUiLayer::RemoveWidget(DWORD WidgetID, bool bRecursive /* = false*/) {
    return RemoveChild(WidgetID, bRecursive);
}

bool CUiLayer::RemoveWidget(const std::string& WidgetName, bool bRecursive /* = false*/) {
    return RemoveChild(WidgetName, bRecursive);
}

bool CUiLayer::FindWidget(DWORD WidgetID, RtwWidget** ppWidget /* = NULL*/, bool bRecursive /* = false*/) {
    return FindChild(WidgetID, ppWidget, bRecursive);
}

bool CUiLayer::FindWidget(const std::string& WidgetName, RtwWidget** ppWidget /* = NULL*/,
                          bool bRecursive /* = false*/) {
    return FindChild(WidgetName, ppWidget, bRecursive);
}

UiWidgetList::iterator CUiLayer::WidgetBegin() {
    return m_pChildren->m_Widgets.begin();
}

UiWidgetList::iterator CUiLayer::WidgetEnd() {
    return m_pChildren->m_Widgets.end();
}

UiWidgetList::reverse_iterator CUiLayer::WidgetBegin_Reverse() {
    return m_pChildren->Begin_Reverse();
}

UiWidgetList::reverse_iterator CUiLayer::WidgetEnd_Reverse() {
    return m_pChildren->End_Reverse();
}

void CUiLayer::_ResetSelf() {
    m_WidgetType = wtLayer;
    m_rcFrame = RtwRect(0, 0, 10000, 10000);
    m_rcClient = RtwRect(0, 0, 10000, 10000);
    m_Flag = m_DefaultFlag;
}

bool CUiLayer::BringWidgetToTop(DWORD WidgetID) {
    if (m_pChildren)
        return m_pChildren->BringWidgetToTop(WidgetID);
    return false;
}

bool CUiLayer::BringWidgetToBottom(DWORD WidgetID) {
    if (m_pChildren)
        return m_pChildren->BringWidgetToBottom(WidgetID);
    return false;
}
