#include "ui/rtw_ui.h"
//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_widget.cpp
//	author:		ldr123
//	purpose:	组件基类及组件管理器
//  other:		局部重构添加注释
//********************************************************************

RT_IMPLEMENT_DYNAMIC(RtwWidget, RtwRefObject, 0, "ui")

RtwEventDispatcher RtwWidget::EvClass_Hint;

bool RtwWidget::isDrawUIName = false;

void ui::RtwWidget::ChangeDrawUIName() {
    isDrawUIName = !isDrawUIName;
}

RtwWidget::RtwWidget()
    : mnRenderTargetIndex(INVALIDE_RENDERTARGET), mbClearRenderTaget(false),
      mlBackBufferDiffuse(0x000000), m_bIsUseSystemFont(false), mbNoClipRect(true),
      mbOnlyRenderToShow(false), m_FontSize(12), m_nBackGroundAlpha(255), mbHighLight(false),
      m_Transparency(1.0), m_Id(0), m_WidgetType(wtUnknown), m_pParent(NULL), m_pHint(NULL),
      m_pChildren(NULL), m_MouseLeave(false), m_pBackGround(NULL), m_pImageSequence(NULL),
      m_Borderpadding(true), m_bAutoScrollH(false), m_pAutoScrollH(NULL), m_bAutoScrollV(false),
      m_pAutoScrollV(NULL), m_light(32), m_bHint(false), m_bShowing(false), m_bHiding(false),
      m_bTrans(false), m_bZoom(false), m_pHoverBorderImage(NULL), m_padding(SMargin(0, 0, 0, 0))

{
    m_Flag |= wfPick | wfDClick;

    SetWidgetEffect();
}

RtwWidget::~RtwWidget() {}

void RtwWidget::OnBeforeDestroy() {
    //摘除与父节点的关系
    _SetParent(NULL);
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            //子控件也摘除
            (*iterChild)->_SetParent(NULL);
        }
        DROP_RTUI_OBJECT(m_pChildren);
    }

    DROP_RTUI_OBJECT(m_pHint);
    DROP_RTUI_OBJECT(m_pAutoScrollV);
    DROP_RTUI_OBJECT(m_pAutoScrollH);

    DROP_TRUI_IMAGE(m_pHoverBorderImage);
    DROP_TRUI_IMAGE(m_pBackGround);

    _ResetSelf();

    //卸载所有事件
    _UnregisterAllEvents();
    RtwRefObject::OnBeforeDestroy();
}

void RtwWidget::_ResetSelf() {
    m_WidgetType = wtWidget;
    if (m_pBackGround) {
        // 		if(m_pBackGround->border) m_pBackGround->border->drop();
        if (m_pBackGround)
            DROP_TRUI_IMAGE(m_pBackGround);
    }

    DROP_TRUI_IMAGE(m_pBackGround);

    DROP_TRUI_IMAGE(m_pImageSequence);

    m_Anchors.clear();
    m_AutoScrollOffset.Reset();

    m_Flag = m_DefaultFlag;
    m_Text = "";
    m_Param1 = NULL;
    m_Param2 = NULL;
    m_BkColor = RtwPixel((DWORD)0x00000000);
    m_Caption = "";
    m_rcFrame = RtwRect(0, 0, 0, 0);
    m_rcClient = RtwRect(0, 0, 0, 0);
    m_HintText = "";
    m_FontType = "";
    m_FontSize = 12;
    mbHighLight = false;
    m_MaskColor = RtwPixel(0x66666666);
    m_MaskValue = 0;
    m_TextAlign = SUiTextAlignment(alignCenter, alignCenter);
    m_TextColor = RtwPixel(0xffffffff);
    m_MouseLeave = false;
    m_WidgetSize = SSize(0, 0);
    m_BorderSize = 0;
    m_BorderColor = (DWORD)0x00000000;
    m_bIsHintWidget = false;
    m_bTextUnderline = false;
    m_nBackGroundAlpha = 255;
    m_bCalcAutoScrollFlag = true;
    m_bJoinParentAutoScroll = true;
    m_bShowing = false;
    m_bHiding = false;
    m_bTrans = false;
    m_bZoom = false;
    m_pHoverBorderImage = NULL;

    SetWidgetEffect();
}

void RtwWidget::Reset() {
    _ResetSelf();
}

void RtwWidget::_SetParent(RtwWidget* pParent) {
    m_pParent = pParent;  //设置父组件

    if (m_pParent) {
        std::string prefix = m_pParent->getName();
        prefix += ".";
        //设置本组件的名字前缀
        _AddNamePrefix(prefix);
    }
}

//设置组件名字前缀
void RtwWidget::_AddNamePrefix(const std::string& prefix) {
    std::string OldName = m_Name;
    m_Name.insert(0, prefix);
    OnNameChanged(OldName);

    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->_AddNamePrefix(prefix);
        }
    }
}

//删除名字前缀
void RtwWidget::_RemoveNamePrefix(const std::string& prefix) {
    int pos = (int)m_Name.find(prefix);
    UI_ENSURE(pos != std::string::npos);

    m_Name.replace(0, pos + (int)prefix.size() + 1, "");

    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->_RemoveNamePrefix(prefix);
        }
    }
}

bool RtwWidget::Create(DWORD Id) {
    _ResetSelf();

    m_Id = Id;
    m_Name += rtFormatNumber(Id);
    OnNameChanged("");

    //子组件
    m_pChildren = RT_NEW CUiWidgetContainer();
    m_pChildren->grab();

    return true;
}

void RtwWidget::Run(DWORD dwDelta) {
    //图片序列切换图片
    if (m_pImageSequence)
        m_pImageSequence->Tick(dwDelta);

    //背景图切换
    if (m_pBackGround)
        m_pBackGround->Tick(dwDelta);

    //子组件的run
    if (m_pChildren)
        m_pChildren->Run(dwDelta);

    if (m_bShowing || m_bHiding) {
        //正在打开 or 正在关闭
        float        fPos = 0.0;
        static float fStartingTime = 0.0;
        static float fHidingTime = 0.0;

        if (m_bShowing) {
            fStartingTime += ((float)dwDelta - fStartingTime / 20);
            fPos = fStartingTime / m_nShowTime;
        } else if (m_bHiding) {
            fHidingTime += ((float)dwDelta + fHidingTime / 20);
            fPos = 1.0 - fHidingTime / m_nShowTime;
        }

        //淡入淡出
        TransparentByPos(fPos);

        //缩放
        MoveResizeByPos(fPos);

        if (fStartingTime > m_nShowTime) {
            //显示结束
            m_bShowing = false;
            fStartingTime = 0.0;
        }

        if (fHidingTime > m_nShowTime) {
            //关闭结束
            m_bHiding = false;
            fHidingTime = 0.0;

            m_nHideEffectLst = esNormal;

            Hide();

            m_nHideEffectLst = m_nHideEffectLstBack;
        }

        if (!m_bShowing && !m_bHiding) {
            m_bTrans = false;
            m_bZoom = false;
        }
    }
}

//缩放
void RtwWidget::MoveResizeByPos(float fPos) {
    if (m_bZoom) {
        RtwRect rc = m_backRc;
        float   HALFACTIVETIME = m_nShowTime / 2;
        int     x = rc.left;
        int     y = rc.top;
        int     width = rc.getWidth();
        int     height = rc.getHeight();
        int     endX = width + x;
        int     endY = height + y;
        int     centerX = (x + endX) >> 1;
        int     centerY = (y + endY) >> 1;

        float fStepX = (float)width / HALFACTIVETIME;
        float fStepY = (float)height / HALFACTIVETIME;

        float fPassed = fPos * HALFACTIVETIME / 2;

        int resultX = centerX - fPassed * fStepX;
        int resultY = centerY - fPassed * fStepY;

        MoveResize(resultX, resultY, width * fPos + resultX, height * fPos + resultY);
    }
}

//设置透明度
void RtwWidget::TransparentByPos(float fPos) {
    if (m_bTrans) {
        SetTransparency(fPos);
    }
}

//将当前组件设置为可用
void RtwWidget::Enable() {
    if (!getFlag(wfEnable, false)) {
        ModifyFlags(wfEnable, 0);
    }

    //将子控件全部可用
    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->Enable();
        }
    }
}

//禁用
void RtwWidget::Disable() {
    if (getFlag(wfEnable, false)) {
        ModifyFlags(0, wfEnable);
    }

    //禁用子控件
    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->Disable();
        }
    }
}

//设置显示文本
void RtwWidget::SetText(const std::string& text) {
    m_Text = text;
}

void RtwWidget::SetName(const std::string& name, const std::string& replacedName /* = ""*/) {
    /////////////////////
    m_base_name = name;
    /////////////////////
    // 只能设置一次，不可以改变
    // [父窗口名字.]自己名字/

    std::string _Replaced;
    std::string OldName = m_Name;

    // 替换(影响自己的名字或者影响父亲的名字)或者重设名字
    if (!replacedName.empty()) {
        int pos = (int)m_Name.find(replacedName);
        if (pos != std::string::npos) {
            _Replaced = replacedName;
            m_Name.replace(pos, replacedName.size(), name);
        }
    } else {
        int posDot = (int)m_Name.find_last_of('.');
        if (posDot != std::string::npos) {
            _Replaced = m_Name.substr(posDot + 1);
            m_Name.replace(posDot + 1, _Replaced.size(), name);
        } else {
            _Replaced = m_Name;
            m_Name = name;
        }
    }
    OnNameChanged(OldName);

    // 设置所有子窗口名字
    if (m_pChildren && !_Replaced.empty()) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->SetName(name, _Replaced);
        }
    }
}

//设置文本对齐方式
void RtwWidget::SetTextAlignment(SUiTextAlignment& align) {
    m_TextAlign = align;
}

//设置组件大小
void RtwWidget::SetWidgetSize(SSize& size) {
    m_rcFrame.right = m_rcFrame.left + size.width;
    m_rcFrame.bottom = m_rcFrame.top + size.height;

    m_rcClient.right = m_rcClient.left + size.width;
    m_rcClient.bottom = m_rcClient.top + size.height;

    m_WidgetSize = size;

    Refresh();
}

//设置标题
void RtwWidget::SetCaption(const std::string& caption) {
    m_Caption = caption;
}

//设置标志位
//begin
void RtwWidget::ModifyFlags(int add, int remove) {
    m_Flag |= add;
    m_Flag &= ~remove;

    if (m_pParent) {
        if ((add & wfGrabMouse) || (remove & wfGrabMouse))
            m_pParent->m_bNeedSortForRender = true;
    }
}

void RtwWidget::ModifyFlag(int add, int remove) {
    ModifyFlags(add, remove);
}

//end

bool RtwWidget::PickWidget(const SPoint& Position, RtwWidget** ppWidget, DWORD FlagMask,
                           CUiWidgetContainer* pExcludeWidgets) {
    // 可见              参数给出的状态是否是当前状态的子集        参数给出的坐标是否在控件内
    if (getVisible() && ((GetFlags() & FlagMask) == FlagMask) && m_rcFrame.IsContained(Position)) {
        if (ppWidget)  //如果ppWidget存在,则将自己赋给他
        {
            grab();
            *ppWidget = this;
        }

        if (m_pChildren) {
            UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
            for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
                RtwWidget* pWidget = *iter;
                if (pWidget->PickWidget(Position, ppWidget, FlagMask, pExcludeWidgets)) {
                    if (pWidget->m_pParent)
                        pWidget->m_pParent->drop();
                    break;
                }
            }
        }

        //控件类型不是层并且(控件容器不为空或找到存在这样一个控件)
        if ((*ppWidget)->getWidgetType() != wtLayer &&
            (!pExcludeWidgets || pExcludeWidgets->FindWidget((*ppWidget)->getId()))) {
            return true;
        } else {
            DROP_RTUI_OBJECT(*ppWidget);
            return false;
        }
    } else {
        return false;
    }
}

bool RtwWidget::AddChild(RtwWidget* pWidget) {
    if (!pWidget)
        return false;

    if (m_pChildren) {
        if (!m_pChildren->AddWidget(pWidget))
            return false;
    }

    //子控件的父节点是自己
    pWidget->_SetParent(this);

    m_bNeedSortForRender = true;

    // 计算此Child的初始信息
    pWidget->Move(SPoint(pWidget->getClientRect().left, pWidget->getClientRect().top));

    return true;
}

bool RtwWidget::AddChild(CUiWidgetContainer* pWidgetContainer) {
    if (m_pChildren)
        return m_pChildren->AddWidget(pWidgetContainer);

    return false;
}

//设置父组件
void RtwWidget::SetParent(RtwWidget* pWidget) {
    _SetParent(pWidget);
}

bool RtwWidget::RemoveChild(DWORD WidgetID, bool bRecursive /* = false*/) {
    RtwWidget* pWidget = NULL;

    if (!m_pChildren) {
        return false;
    }

    if (!m_pChildren->FindWidget(WidgetID, &pWidget, bRecursive)) {
        return false;
    }

    m_pChildren->RemoveWidget(WidgetID, bRecursive);

    OnRemoveChild(pWidget);

    if (pWidget) {
        pWidget->_SetParent(NULL);

        // 如果还有其他地方引用，为了避免此widget处于真空状态，就把它加入到GarbageContianer中
        if (pWidget->getRefCount() > 1) {
            g_workspace.AddGarbageWidget(pWidget);
        }

        DROP_RTUI_OBJECT(pWidget);
    }

    return true;
}

//根据名字删除子组件
bool RtwWidget::RemoveChild(const std::string& WidgetName, bool bRecursive /* = false*/) {
    RtwWidget* pWidget = NULL;
    if (!m_pChildren && !m_pChildren->FindWidget(WidgetName, &pWidget, bRecursive)) {
        return true;
    }

    m_pChildren->RemoveWidget(WidgetName, bRecursive);

    OnRemoveChild(pWidget);

    pWidget->_SetParent(NULL);

    // 如果还有其他地方引用，为了避免此widget处于真空状态，就把它加入到GarbageContianer中
    if (pWidget->getRefCount() > 1) {
        g_workspace.AddGarbageWidget(pWidget);
    }

    DROP_RTUI_OBJECT(pWidget);

    return true;
}

//删除所有组件
bool RtwWidget::RemoveAllChildren(CUiWidgetContainer* pExcludeWidgets /* = NULL*/) {
    std::list<DWORD> WidgetsIdToRemove;
    if (m_pChildren) {
        for (UiWidgetList::iterator iterWidget = m_pChildren->m_Widgets.begin();
             iterWidget != m_pChildren->m_Widgets.end(); ++iterWidget) {
            RtwWidget* pWidget = *iterWidget;
            if (pWidget == m_pAutoScrollV || pWidget == m_pAutoScrollH)  // 不删除自动滚动条
                continue;
            if (pExcludeWidgets && pExcludeWidgets->FindWidget(pWidget->getId()))
                continue;

            WidgetsIdToRemove.push_back(pWidget->getId());
        }
    }

    foreach (std::list<DWORD>, iterRemove, WidgetsIdToRemove) {
        DWORD WidgetId = *iterRemove;
        RemoveChild(WidgetId);
    }

    return true;
}

//根据id查找子组件
bool RtwWidget::FindChild(DWORD WidgetID, RtwWidget** ppWidget /* = NULL*/,
                          bool bRecursive /* = false*/) {
    if (m_pChildren)
        return m_pChildren->FindWidget(WidgetID, ppWidget, bRecursive);
    return false;
}

//根据组件名查找子组件
bool RtwWidget::FindChild(const std::string& WidgetName, RtwWidget** ppWidget /* = NULL*/,
                          bool bRecursive /* = false*/) {
    if (m_pChildren)
        return m_pChildren->FindWidget(WidgetName, ppWidget, bRecursive);

    return false;
}

//根据组件名查找子组件
RtwWidget* RtwWidget::GetChildWidget(const std::string& WidgetName) {
    RtwWidget* pRetWg = NULL;
    if (FindChild(WidgetName, &pRetWg, true))
        return pRetWg;

    return pRetWg;
}

//设置开关特效(显示,关闭)
void RtwWidget::SetWidgetEffect(int nShowEffect, int nHideEffect, int nDelay) {
    m_nShowEffectLst = nShowEffect;
    m_nHideEffectLst = nHideEffect;
    m_nShowEffectLstBack = m_nShowEffectLst;
    m_nHideEffectLstBack = nHideEffect;
    m_nShowTime = nDelay;
}

//显示
void RtwWidget::Show() {
    if (!getFlag(wfVisible, false)) {
        ModifyFlag(wfVisible, 0);

        // 如果是带标题栏的Form,把Widget提到最前面显示
        if (getWidgetType() == wtForm) {
            RtwForm* pForm = (RtwForm*)this;

            //显示form的titlebar?
            if (pForm->getShowTitleBar()) {
                //如果没有父组件
                if (!pForm->getParent()) {
                    g_workspace.BringWidgetToTop(this);
                }
            }

            //特效处理
            if (m_nShowEffectLst != esNormal) {
                m_bShowing = true;
                //透明特效
                if ((m_nShowEffectLst & esTrans) == esTrans) {
                    m_bTrans = true;
                }

                //缩放特效
                if ((m_nShowEffectLst & esZoom) == esZoom) {
                    m_bZoom = true;
                }
            }
        }

        m_backRc = m_rcFrame;
        OnShow();
    }
}

//显示并设置焦点
void RtwWidget::ShowAndFocus() {
    Show();
    g_workspace.SetFocusWidget(this);
}

//隐藏窗体
void RtwWidget::Hide() {
    //将特效关闭
    m_bHiding = m_bTrans = m_bZoom = false;

    //可隐藏?
    if (GetFlags(wfVisible)) {
        static bool bRecorded = false;

        //特效备份
        if (m_nHideEffectLst == esNormal) {
            ModifyFlags(0, wfVisible);

            if (g_workspace.GetFocusWidget()) {
                if (getWidgetType() == wtLayer) {
                    g_workspace.SetFocusWidget(NULL);
                } else {
                    RtwWidget* pTmp = g_workspace.GetFocusWidget();
                    while (pTmp) {
                        if (pTmp == this) {
                            g_workspace.SetFocusWidget(NULL);
                            break;
                        }
                        pTmp = pTmp->GetParent();
                    }
                }
            }

            g_workspace.RemoveModalWidget(getId());
            OnHide();
            // 			ResetScrollBar();
            bRecorded = false;
        } else {
            //修改特效状态
            m_bHiding = true;
            if ((m_nHideEffectLst & esTrans) == esTrans) {
                m_bTrans = true;
            }

            if ((m_nHideEffectLst & esZoom) == esZoom) {
                m_bZoom = true;
            }
        }
    }
}

//重置滚动条
void RtwWidget::ResetScrollBar() {
    //自动纵向滚动条
    RtwVScrollBar* pAutoScrollV = getAutoScrollV();
    if (pAutoScrollV) {
        //重置
        pAutoScrollV->ScrollToTop();
    }

    //子组件全部重置
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            RtwWidget* pWidget = *iterChild;
            pWidget->ResetScrollBar();
        }
    }
}

//模态对话框显示
void RtwWidget::ShowModal() {
    if (!getFlag(wfVisible, false)) {
        ModifyFlags(wfVisible, 0);

        g_workspace.AddModalWidget(this);
        g_workspace.SetFocusWidget(this);

        OnShow();

        //特效
        if (m_nShowEffectLst != esNormal) {
            m_bShowing = true;
            if ((m_nShowEffectLst & esTrans) == esTrans) {
                m_bTrans = true;
            }

            if ((m_nShowEffectLst & esZoom) == esZoom) {
                m_bZoom = true;
            }
        }
        m_backRc = m_rcFrame;
    }
}

//刷新
void RtwWidget::Refresh() {
    //计算显示区域
    CalcClient();

    //计算滚动条
    CalcAutoScroll();
}

//关闭
void RtwWidget::Close() {
    OnClose();
}

//移动函数(根据给出的大小移动位置)
//being
void RtwWidget::Offset(const SSize& size, bool bTriggerEvent /* = true*/,
                       bool bAutoScrollParent /* = true*/) {
    //当成x,y处理
    if (size.width == 0 && size.height == 0)
        return;

    m_rcFrame.Offset(size.width, size.height);
    m_rcClient.Offset(size.width, size.height);

    //如果有子组件
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            RtwWidget* pWidget = *iterChild;

            if (pWidget->getAnchorsCount() == 0)  // 如果没有锚点信息
            {
                pWidget->Offset(size, bTriggerEvent, bAutoScrollParent);
            }
        }
    }

    OnOffset(size);

    // 抛出事件
    if (bTriggerEvent) {
        RtwEventDelegate e(etOffset);
        e.offset.width = size.width;
        e.offset.height = size.height;

        EvOffset(this, e);
    }

    // 如果父窗口自动滚动,就更新父窗口
    if (bAutoScrollParent && m_bJoinParentAutoScroll) {
        RtwWidget* pParent = getParent();
        if (pParent) {
            if (pParent->getAutoScrollV() || pParent->getAutoScrollH()) {
                pParent->CalcAutoScroll();
            }
        }
    }
}

void RtwWidget::Move(const SPoint& pos, bool bTriggerEvent /* = true*/,
                     bool bAutoScrollParent /* = true*/) {
    SSize OffsetSize(pos.x - m_rcFrame.left, pos.y - m_rcFrame.top);
    Offset(OffsetSize);
}

void RtwWidget::Move(const RtwRect& rect, bool bAbs /* = false*/, bool bTriggerEvent /* = true*/,
                     bool bAutoScrollParent /* = true*/) {
    RtwRect rcOrig = m_rcFrame;
    // 如果大小未发生改变，就调用Offset
    if (rect.getWidth() == m_rcFrame.getWidth() && rect.getHeight() == m_rcFrame.getHeight()) {
        SSize sizeOffset(rect.left - m_rcFrame.left, rect.top - m_rcFrame.top);
        if (!bAbs && getParent()) {
            sizeOffset.width += getParent()->GetFrameRect().left;
            sizeOffset.height += getParent()->GetFrameRect().top;
        }

        Offset(sizeOffset, bTriggerEvent, bAutoScrollParent);
        return;
    }

    // Widget重新设置区域
    m_rcFrame = rect;
    if (!bAbs && GetParent()) {
        m_rcFrame.Offset(GetParent()->GetFrameRect().getLeftTop());
    }

    RtwRect rcNow = m_rcFrame;
    SSize   OffsetSize(rcNow.left - rcOrig.left, rcNow.top - rcOrig.top);

    // 子Widget位移
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            RtwWidget* pWidget = *iterChild;
            if (pWidget->getAnchorsCount() == 0)
                pWidget->Offset(OffsetSize);
        }
    }

    OnMove(rcOrig);

    // 抛出事件
    if (bTriggerEvent) {
        RtwEventDelegate e(etMove);
        e.move.OrigFrame_Left = rcOrig.left;
        e.move.OrigFrame_Right = rcOrig.right;
        e.move.OrigFrame_Top = rcOrig.top;
        e.move.OrigFrame_Bottom = rcOrig.bottom;
        EvMove(this, e);
    }

    // 如果父窗口自动滚动,就更新父窗口
    if (bAutoScrollParent && m_bJoinParentAutoScroll) {
        RtwWidget* pParent = getParent();
        if (pParent) {
            if (pParent->getAutoScrollV() || pParent->getAutoScrollH()) {
                pParent->CalcAutoScroll();
            }
        }
    }
}

//end

//居中显示
void RtwWidget::CenterScreen(int nOffsetX /*= 0*/, int nOffsetY /* = 0 */) {
    int w = g_workspace.getRenderDevice()->getViewportRect().getWidth();
    int h = g_workspace.getRenderDevice()->getViewportRect().getHeight();

    int x = ((w - m_rcFrame.getWidth()) >> 1) + nOffsetX;
    int y = ((h - m_rcFrame.getHeight()) >> 1) + nOffsetY;

    Move(SPoint(x, y));
}

//设置背景图
//begin
void RtwWidget::SetBackgroundImage(RtwImage* pImage) {
    DROP_TRUI_IMAGE(m_pBackGround);
    m_pBackGround = pImage;
    if (m_pBackGround)
        m_pBackGround->border = pImage->border;
    GRAB_TRUI_IMAGE(m_pBackGround);
}

//end

//设置背景色
void RtwWidget::SetBackgroundColor(RtwPixel color) {
    m_BkColor = color;
}

//设置图片序列
void RtwWidget::SetImageSequence(RtwComplexImage* pImageSequence) {
    DROP_TRUI_IMAGE(m_pImageSequence);

    m_pImageSequence = pImageSequence;
    GRAB_TRUI_IMAGE(m_pImageSequence);
}

//以某具体组件作为提示框内容
void RtwWidget::SetHint(RtwWidget* pHint) {
    DROP_RTUI_OBJECT(m_pHint);

    m_pHint = pHint;
    if (m_pHint) {
        m_pHint->grab();
        g_workspace.AddTopRenderWidget(m_pHint);
        m_pHint->m_bIsHintWidget = true;
        m_pHint->ModifyFlag(0, wfGrabMouse | wfMouseMove);
    }
}
//没有使用
void RtwWidget::SetOwnHint(RtwWidget* pHint)  // add by chib 2006-11-14 pHint must be editbox point
{
    if (NULL == pHint) {
        return;
    }
    SetHint(pHint);

    m_bHint = true;
    AutoHintSize();
    AutoHintPosition();
    m_pHint->Hide();
}

//设置提示框文本
void RtwWidget::SetHintText(const std::string& HintText) {
    if (m_HintText == HintText)
        return;
    m_HintText = HintText;
    if (m_HintText.empty() || m_pHint == 0) {
        SetHint(g_workspace.getDefaultHint());
    }

    m_pHint->SetUseSystemFont(false);

    // 重值TargetWidget来刷新
    g_workspace.ResetHintTargetWidget();
}

//动态设置提示框位置
void RtwWidget::AutoHintPosition() {
    if (m_pHint) {
        RtwRect rc;
        RtwRect rcHint = m_pHint->GetFrameRect();

        rc.left = m_rcFrame.left;

        if (rc.left < 0) {
            rc.left = 0;
        }

        rc.right = rc.left + rcHint.getWidth();
        rc.top = m_rcFrame.top - 10 - rcHint.getHeight();

        if (rc.top < 0) {
            rc.top = 0;
        }

        rc.bottom = rc.top + rcHint.getHeight() - 1;
        if (rc.right > g_workspace.getViewportRect().right) {
            rc.Offset(g_workspace.getViewportRect().right - rc.right - 2, 0);
        }
        if (rc.bottom > g_workspace.getViewportRect().bottom) {
            rc.Offset(0, g_workspace.getViewportRect().bottom - rc.bottom - 2);
        }

        m_pHint->Move(rc);
    }
}

//自动的hint大小(editbox)
void RtwWidget::AutoHintSize() {
    //lyytodo
    return;  //Y直接return????
    if (m_pHint) {
        int nLength = g_workspace.getFontManager()->GetStringWidth(m_pHint->getText());
        int nLineCount = nLength / Max_Width + 1;

        if (m_bHint)  // add by chib
        {
            nLineCount = ((RtwTextBox*)m_pHint)->getLineCount();
        }

        RtwRect rc;
        rc.left = 0;
        rc.top = 0;
        if (nLineCount > 1) {
            rc.right = Max_Width - 1;
        } else {
            rc.right = nLength + (RtwTextBox::m_LeftSpace << 1) + 3;
        }
        rc.bottom = nLineCount * g_workspace.getFontManager()->getFontSize() +
                    RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;
        m_pHint->Move(rc);
    }
}

//其他类型提示框自动大小
//begin
void RtwWidget::AutoHtmlHintSize() {
    return;  //直接return

    int nLineCount = ((RtwTextBox*)m_pHint)->getLineCount();

    RtwRect rc;
    rc.left = 0;
    rc.right = 200;
    rc.top = 0;
    rc.bottom = nLineCount * g_workspace.getFontManager()->getFontSize() + 20;
    m_pHint->Move(rc);
}

void RtwWidget::AutoSizeByFont() {

    int nLength = g_workspace.getFontManager()->GetStringWidth(m_Text);
    int nLineCount = nLength / Max_Width + 1;

    RtwRect rc;
    rc.left = 0;
    rc.top = 0;
    if (nLineCount > 1) {
        rc.right = Max_Width - 1;
    } else {
        rc.right = nLength + (RtwTextBox::m_LeftSpace << 1) + 3;
    }
    rc.bottom = nLineCount * g_workspace.getFontManager()->getFontSize() +
                RtwTextBox::m_LineSpace * (nLineCount + 1) - 1;

    Move(rc);
}

//end

void RtwWidget::AutoClampScreen() {
    RtwRect rc;
    rc.left = m_rcFrame.left;
    if (rc.left < 0)
        rc.left = 0;
    rc.right = rc.left + m_rcFrame.getWidth() - 1;
    rc.top = m_rcFrame.top - 10 - m_rcFrame.getHeight();
    if (rc.top < 0)
        rc.top = 0;
    rc.bottom = rc.top + m_rcFrame.getHeight() - 1;

    if (rc.right > g_workspace.getViewportRect().right) {
        rc.Offset(g_workspace.getViewportRect().right - rc.right - 2, 0);
    }
    if (rc.bottom > g_workspace.getViewportRect().bottom) {
        rc.Offset(0, g_workspace.getViewportRect().bottom - rc.bottom - 2);
    }
    Move(rc);
}

//不克隆父子关系，不克隆事件处理 (应该删掉所有儿子，并且克隆儿子)
bool RtwWidget::CloneFrom(const RtwWidget* pWidget) {
    UI_ENSURE_B(pWidget->IsKindOf(RtwWidget::GetRuntimeClass()));

    m_pHint = pWidget->m_pHint;
    if (m_pHint)
        m_pHint->grab();

    m_Flag = pWidget->GetFlags();
    m_Caption = pWidget->getCaption();
    m_Text = pWidget->GetText();
    m_FontType = pWidget->getFont();
    m_TextAlign = pWidget->getTextAlignment();

    m_pBackGround = pWidget->m_pBackGround;
    if (m_pBackGround) {
        GRAB_TRUI_IMAGE(m_pBackGround);
        if (m_pBackGround->border) {
            m_pBackGround->border->grab();
        }
    }
    m_pImageSequence = pWidget->m_pImageSequence;
    if (m_pImageSequence)
        GRAB_TRUI_IMAGE(m_pImageSequence);

    m_TextColor = pWidget->getTextColor();
    m_BkColor = pWidget->getBackgroundColor();
    m_rcClient = pWidget->getClientRect();
    m_rcFrame = pWidget->GetFrameRect();
    m_HintText = pWidget->getHintText();
    m_BorderColor = pWidget->getBorderColor();
    m_BorderSize = pWidget->getBorderSize();
    m_strSoundFilename = pWidget->getSoundFile();
    m_WidgetSize = pWidget->getWidgetSize();
    m_bAutoScrollV = pWidget->m_bAutoScrollV;
    m_bAutoScrollH = pWidget->m_bAutoScrollH;
    m_pAutoScrollV = pWidget->m_pAutoScrollV;

    m_bJoinParentAutoScroll = pWidget->getJoinParentAutoScroll();
    if (m_pAutoScrollV)
        m_pAutoScrollV->grab();

    m_pAutoScrollH = pWidget->m_pAutoScrollH;
    if (m_pAutoScrollH)
        m_pAutoScrollH->grab();

    m_bCalcAutoScrollFlag = pWidget->m_bCalcAutoScrollFlag;

    return true;
}

//设置目标渲染组件
void RtwWidget::SetWidgetRenderTarget(int nTargetIndex) {
    mnRenderTargetIndex = nTargetIndex;
    SetClearRenderTarget(false, 0);

    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->SetWidgetRenderTarget(nTargetIndex);
        }
    }
}

void RtwWidget::Render(const RtwRect* pClipRect /* = NULL*/, bool bAlwaysRender /* = false*/) {
    //只是为了显示
    if (INVALIDE_RENDERTARGET != mnRenderTargetIndex) {
        g_workspace.getRenderDevice()->SetTextureRenderTarget(
            mnRenderTargetIndex, mbClearRenderTaget, mlBackBufferDiffuse);
        if (mbOnlyRenderToShow) {
            Show();
        }
    }

    //不可见
    if (!getVisible(false)) {
        if (mnRenderTargetIndex != INVALIDE_RENDERTARGET) {
            g_workspace.getRenderDevice()->RestorRenderTarget();
            if (mbOnlyRenderToShow)
                Hide();
        }
        return;
    }

    OnDraw();

    //先画后台
    DrawBackground(pClipRect);

    //前台绘制
    DrawForeground(pClipRect);

    //字体渲染
    g_workspace.getFontManager()->Render();

    // DrawChildren
    _SortChildrenForRender();

    RtwRect rcClip = m_rcFrame;
    if (pClipRect) {
        rcClip.ClipBy(*pClipRect);
    }

    if (m_pChildren) {
        UiWidgetList::reverse_iterator iterChild = m_pChildren->Begin_Reverse();
        for (; iterChild != m_pChildren->End_Reverse(); ++iterChild) {
            RtwWidget* pChild = *iterChild;
            int        nRenderTarget = pChild->mnRenderTargetIndex;
            pChild->mnRenderTargetIndex = INVALIDE_RENDERTARGET;
            if (pChild->mbNoClipRect) {
                pChild->Render(&rcClip);
            } else {
                pChild->Render(NULL, true);
            }
            pChild->mnRenderTargetIndex = nRenderTarget;
        }
    }

    if (mnRenderTargetIndex != INVALIDE_RENDERTARGET) {
        g_workspace.getRenderDevice()->RestorRenderTarget();
        if (mbOnlyRenderToShow)
            Hide();
    }

    EvPostDraw(this, etPostDraw);
}

//lmk 组件结构打印
void RtwWidget::PrintToConsole(const std::string& prefix /* = ""*/) {
    RtCoreLog().Info("%s[%s] %s(0x%x)\n", prefix.c_str(),
                     g_WidgetTypeShortName[m_WidgetType].c_str(), getName().c_str(), GetFlags());

    if (m_pChildren) {
        std::string            prefixChild = prefix + "    ";
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->PrintToConsole(prefixChild);
        }
    }
}

//前端显示
void RtwWidget::DrawForeground(const RtwRect* pClipRect /* = NULL*/) {
    //lyytodo 这里添加所有组件显示名称方便开发使用 后续删掉

    if (isDrawUIName && g_workspace.getMouseHover() == this) {
        //RtwRect temp;
        //temp.left = m_rcFrame.left;
        //temp.right = m_rcFrame.right;
        //temp.top = m_rcFrame.top -
        //           20;  // 将矩形的顶部位置设置为组件的顶部减去偏移量（20 是示例值，根据需要调整）
        //temp.bottom = m_rcFrame.top;  // 矩形的底部与顶部对齐
        g_workspace.getFontManager()->DrawString(m_Name, m_rcFrame, 0xffff0000, true,
                                                 SUiTextAlignment(alignFar, alignFar), true,
                                                 pClipRect, "", 12);
    }

    //
    RtwRect rcRender = m_rcFrame;
    if (pClipRect)
        rcRender.ClipBy(*pClipRect);

    RtwRect UltraRect;
    UltraRect.bottom = m_rcFrame.bottom;
    UltraRect.left = m_rcFrame.left;
    UltraRect.top = m_rcFrame.top;
    UltraRect.right = m_rcFrame.right - 2;

    //显示文本
    //begin
    if (!m_UltraText1.Text.empty()) {
        if (m_UltraText1.FontSize > 0)
            g_workspace.getFontManager()->DrawString(
                m_UltraText1.Text, UltraRect, SETCOLORALPHA(m_UltraText1.TextColor, m_Transparency),
                false, m_UltraText1.Align, false, pClipRect, "", m_UltraText1.FontSize);
        else
            g_workspace.getFontManager()->DrawString(
                m_UltraText1.Text, UltraRect, SETCOLORALPHA(m_UltraText1.TextColor, m_Transparency),
                g_workspace.getFontManager()->getDefaultFont(), true, m_UltraText1.Align, false,
                pClipRect);
    }

    if (!m_UltraText2.Text.empty()) {
        if (m_UltraText2.FontSize > 0)
            g_workspace.getFontManager()->DrawString(
                m_UltraText2.Text, UltraRect, SETCOLORALPHA(m_UltraText2.TextColor, m_Transparency),
                false, m_UltraText2.Align, false, pClipRect, "", m_UltraText2.FontSize);
        else
            g_workspace.getFontManager()->DrawString(
                m_UltraText2.Text, UltraRect, SETCOLORALPHA(m_UltraText2.TextColor, m_Transparency),
                g_workspace.getFontManager()->getDefaultFont(), true, m_UltraText2.Align, false,
                pClipRect);
    }
    //end

    //图片序列
    if (m_pImageSequence) {
        RtwImage* pImage = m_pImageSequence->getCurrentImage();
        pImage->SetCtrlTransparent(m_Transparency);
        if (pImage)
            g_workspace.getRenderDevice()->DrawImage(pImage->getRenderImage(), m_rcFrame,
                                                     SUiTextAlignment(alignNear, alignNear),
                                                     pClipRect);
    }

    //m_MaskValue > 0
    if (!REAL_EQUAL(m_MaskValue, 0) && m_MaskValue > 0) {
        SPoint p1, p2, p3;
        p1 = m_rcFrame.getCenter();
        int  Width = m_rcFrame.getWidth();
        int  Height = m_rcFrame.getHeight();
        Real _Factor = atan2((Real)Width, (Real)Height) / 2 / UI_PI;

        if (m_MaskValue > 0 && m_MaskValue <= _Factor) {
            p2.x = p1.x;
            p2.y = m_rcFrame.top;
            p3.x = p2.x + (Real)tan((Real)(2 * UI_PI * m_MaskValue)) * (Height >> 1);
            p3.y = m_rcFrame.top;

            g_workspace.getRenderDevice()->DrawTriangle(p1, p3, p2,
                                                        SETCOLORALPHA(m_MaskColor, m_Transparency));
        } else {
            p2.x = p1.x;
            p2.y = m_rcFrame.top;
            p3.x = m_rcFrame.right;
            p3.y = m_rcFrame.top;
            g_workspace.getRenderDevice()->DrawTriangle(p1, p3, p2,
                                                        SETCOLORALPHA(m_MaskColor, m_Transparency));

            if (m_MaskValue <= 0.5 - _Factor) {
                p2.x = m_rcFrame.right;
                p2.y = m_rcFrame.top;
                p3.x = m_rcFrame.right;
                if (m_MaskValue <= 0.25) {
                    p3.y =
                        p1.y - (Real)tan((Real)(2 * UI_PI * (0.25 - m_MaskValue))) * (Width >> 1);
                } else {
                    p3.y =
                        p1.y + (Real)tan((Real)(2 * UI_PI * (m_MaskValue - 0.25))) * (Width >> 1);
                }

                g_workspace.getRenderDevice()->DrawTriangle(
                    p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));
            } else {
                p2.x = m_rcFrame.right;
                p2.y = m_rcFrame.top;
                p3.x = m_rcFrame.right;
                p3.y = m_rcFrame.bottom;
                g_workspace.getRenderDevice()->DrawTriangle(
                    p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));

                if (m_MaskValue <= 0.5 + _Factor) {
                    p2.x = m_rcFrame.right;
                    p2.y = m_rcFrame.bottom;
                    p3.y = m_rcFrame.bottom;
                    if (m_MaskValue <= 0.5) {
                        p3.x = p1.x +
                               (Real)tan((Real)(2 * UI_PI * (0.5 - m_MaskValue))) * (Height >> 1);
                    } else {
                        p3.x = p1.x -
                               (Real)tan((Real)(2 * UI_PI * (m_MaskValue - 0.5))) * (Height >> 1);
                    }

                    g_workspace.getRenderDevice()->DrawTriangle(
                        p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));
                } else {
                    p2.x = m_rcFrame.right;
                    p2.y = m_rcFrame.bottom;
                    p3.x = m_rcFrame.left;
                    p3.y = m_rcFrame.bottom;
                    g_workspace.getRenderDevice()->DrawTriangle(
                        p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));

                    if (m_MaskValue <= 1 - _Factor) {
                        p2.x = m_rcFrame.left;
                        p2.y = m_rcFrame.bottom;
                        p3.x = m_rcFrame.left;
                        if (m_MaskValue <= 0.75) {
                            p3.y = p1.y + (Real)tan((Real)(2 * UI_PI * (0.75 - m_MaskValue))) *
                                              (Width >> 1);
                        } else {
                            p3.y = p1.y - (Real)tan((Real)(2 * UI_PI * (m_MaskValue - 0.75))) *
                                              (Width >> 1);
                        }

                        g_workspace.getRenderDevice()->DrawTriangle(
                            p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));
                    } else {
                        p2.x = m_rcFrame.left;
                        p2.y = m_rcFrame.bottom;
                        p3.x = m_rcFrame.left;
                        p3.y = m_rcFrame.top;
                        g_workspace.getRenderDevice()->DrawTriangle(
                            p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));

                        if (m_MaskValue <= 1) {
                            p2.x = m_rcFrame.left;
                            p2.y = m_rcFrame.top;
                            p3.y = m_rcFrame.top;
                            p3.x = p1.x -
                                   (Real)tan((Real)(2 * UI_PI * (1 - m_MaskValue))) * (Height >> 1);

                            g_workspace.getRenderDevice()->DrawTriangle(
                                p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));
                        } else {
                            p2.x = m_rcFrame.left;
                            p2.y = m_rcFrame.top;
                            p3.x = p1.x;
                            p3.y = m_rcFrame.top;
                            g_workspace.getRenderDevice()->DrawTriangle(
                                p1, p3, p2, SETCOLORALPHA(m_MaskColor, m_Transparency));
                        }
                    }
                }
            }
        }
    }

    //可用, 鼠标,边框不为空
    if (getEnable() && g_workspace.getMouseHover() == this && !m_BorderMargin.IsEmpty()) {
        if (m_pHoverBorderImage) {
            RtwRect rc(m_rcFrame);
            rc.left -= m_BorderMargin.left;
            rc.top -= m_BorderMargin.top;
            rc.right += m_BorderMargin.right;
            rc.bottom += m_BorderMargin.bottom;
            g_workspace.getRenderDevice()->DrawBorderImage(m_pHoverBorderImage, rc, m_BorderMargin);
        }
    }
}

//画背景
void RtwWidget::DrawBackground(const RtwRect* pClipRect /* = NULL*/) {
    RtwRect rcRender = m_rcFrame;
    if (pClipRect)
        rcRender.ClipBy(*pClipRect);

    //画背景色
    if (m_BkColor != 0 && m_WidgetType != wtForm) {
        g_workspace.getRenderDevice()->DrawRectangle(rcRender,
                                                     SETCOLORALPHA(m_BkColor, m_Transparency));
    }

    if (g_workspace.getDebugMode()) {
        g_workspace.getRenderDevice()->DrawRectangleBorder(
            m_rcFrame, SETCOLORALPHA(RtwPixel(0xffef4fff), m_Transparency));
    }

    //画背景图
    if (m_pBackGround) {
        //背景图
        RtwImage* pBackgroundImg = m_pBackGround->getRenderImage();
        if (pBackgroundImg) {
            RtwPixel kBackColor = pBackgroundImg->m_Color;
            pBackgroundImg->m_Color.a = m_nBackGroundAlpha;
            pBackgroundImg->m_Color.r = m_nBackGroundAlpha;
            pBackgroundImg->m_Color.g = m_nBackGroundAlpha;
            pBackgroundImg->m_Color.b = m_nBackGroundAlpha;
            if (mbHighLight) {
                pBackgroundImg->m_bHighLight = true;
            }

            RtwRect rect = m_rcFrame;
            rect.top += m_pBackGround->borderSize.top;
            rect.left += m_pBackGround->borderSize.left;
            rect.bottom -= m_pBackGround->borderSize.bottom;
            rect.right -= m_pBackGround->borderSize.right;

            pBackgroundImg->SetCtrlTransparent(m_Transparency);
            g_workspace.getRenderDevice()->DrawImage(
                pBackgroundImg, rect, SUiTextAlignment(alignNear, alignNear), &rcRender);
            if (mbHighLight) {
                pBackgroundImg->m_bHighLight = false;
            }
            pBackgroundImg->m_Color = kBackColor;
        }

        //背景边框
        if (m_pBackGround->border && m_pBackGround->border->getRenderImage()) {
            m_pBackGround->border->SetCtrlTransparent(m_Transparency);
            if (pClipRect) {
                if (m_Borderpadding) {
                    g_workspace.getRenderDevice()->DrawBorderImage(
                        m_pBackGround->border,
                        RtwRect(m_rcFrame.left, m_rcFrame.top + m_padding.top, m_rcFrame.right,
                                m_rcFrame.bottom),
                        m_pBackGround->borderSize, SUiTextAlignment(alignNear, alignNear),
                        &rcRender);
                } else {
                    g_workspace.getRenderDevice()->DrawBorderImage(
                        m_pBackGround->border,
                        RtwRect(m_rcFrame.left, m_rcFrame.top, m_rcFrame.right, m_rcFrame.bottom),
                        m_pBackGround->borderSize, SUiTextAlignment(alignNear, alignNear),
                        &rcRender);
                }

            } else
                g_workspace.getRenderDevice()->DrawBorderImage(
                    m_pBackGround->border, m_rcFrame, m_pBackGround->borderSize,
                    SUiTextAlignment(alignNear, alignNear), &rcRender);
        }
    }
}

//鼠标滑过时的图片
void RtwWidget::SetHoverBoderImage(const string& strBoderFile, RtwRect srcRc,
                                   SMargin BorderMargin) {
    if (strBoderFile.empty()) {
        return;
    }

    if (strBoderFile == m_strHoverBorderFile) {
        return;
    }

    if (m_pHoverBorderImage) {
        DROP_RTUI_OBJECT(m_pHoverBorderImage);
    }

    m_pHoverBorderImage = g_workspace.getImageFactory()->createImage(strBoderFile.c_str());
    if (m_pHoverBorderImage) {
        if (srcRc.isValid()) {
            m_pHoverBorderImage->SetSourceRect(srcRc);
        }
        m_pHoverBorderImage->SetBlend(true);
        SetHoverBoderSize(BorderMargin);
    }
}

//Frame变化大小时的响应函数
void RtwWidget::OnFrameSizeChanged() {
    CalcClient();
}

void RtwWidget::OnNameChanged(const std::string& OldFullName) {}

//组件矩形计算
void RtwWidget::CalcClient() {
    m_rcClient.left = m_rcFrame.left + (m_BorderSize + m_padding.left);
    m_rcClient.right = m_rcFrame.right - (m_BorderSize + m_padding.right);
    m_rcClient.top = m_rcFrame.top + (m_BorderSize + m_padding.top);
    m_rcClient.bottom = m_rcFrame.bottom - (m_BorderSize + m_padding.bottom);
    CalcAutoScroll();
}

//滚动条计算
void RtwWidget::CalcAutoScroll() {
    //不刷新滚动条
    if (!m_bCalcAutoScrollFlag)
        return;

    if (m_pAutoScrollV || m_pAutoScrollH) {
        //得到子控件矩形
        RtwRect rcChildren = CalcChildrenRect();

        // 		bool bScrollH, bScrollV;
        // 		bScrollV = bScrollH = false;
        if (rcChildren.top > m_rcFrame.top) {
            // 			bScrollV = true;
            int nTmpHeight = rcChildren.getHeight();
            rcChildren.top = m_rcFrame.top;
            rcChildren.bottom = rcChildren.top + nTmpHeight;
        }

        if (rcChildren.left > m_rcFrame.left) {
            // 			bScrollH = true;
            int nTmpWidth = rcChildren.getWidth();
            rcChildren.left = m_rcFrame.left;
            rcChildren.right = rcChildren.left + nTmpWidth + 1;
        }

        rcChildren.Offset(-m_rcFrame.getLeftTop());
        rcChildren.Offset(-rcChildren.getLeftTop());

        rcChildren.right += g_workspace.getDefaultScrollVWidth() - 1;
        rcChildren.bottom += g_workspace.getDefaultScrollHHeight() - 1;

        Real _rate = (Real)m_rcFrame.getHeight() / rcChildren.getHeight();

        //纵向自动滚动条
        if (m_pAutoScrollV) {
            int _min = rcChildren.top;
            int _max = rcChildren.bottom - m_rcFrame.getHeight();
            if (_max < _min) {
                _max = _min;
            }

            int _value = m_pAutoScrollV->getValue();
            int _minValue = m_pAutoScrollV->getMin();
            int _stepValue = _value - _minValue;

            if (_stepValue < 0) {
                _stepValue = 0;
            }

            _value = _min + _stepValue;
            if (_value > _max) {
                _value = _max;
            } else if (_value < _min) {
                _value = _min;
            }

            m_pAutoScrollV->SetData(_min, _max, _value, _rate);
        }

        //横向自动滚动条
        if (m_pAutoScrollH) {
            // Modified by Wayne Wong 2010-12-20
            /*
			int _min = rcChildren.left;
			int _max = m_rcFrame.getWidth() - rcChildren.right;

			if (_max < _min)
			{
				_max = _min;
			}

			int _value = m_pAutoScrollH->getValue();
			if (_value > _max)
			{
				_value = _max;
			}
			else if (_value < _min)
			{
				_value = _min;
			}

			m_pAutoScrollH->SetData(_min, _max, _value, _rate);
			*/

            int _min = rcChildren.left;
            int _max = rcChildren.right - m_rcFrame.getWidth();
            //int _max = m_rcFrame.getWidth() - rcChildren.right;

            if (_max < _min) {
                _max = _min;
            }

            int _value = m_pAutoScrollH->getValue();
            int _minValue = m_pAutoScrollV->getMin();
            int _stepValue = _value - _minValue;
            if (_stepValue < 0) {
                _stepValue = 0;
            }

            _value = _min + _stepValue;
            if (_value > _max) {
                _value = _max;
            } else if (_value < _min) {
                _value = _min;
            }

            Real _rate = (Real)m_rcFrame.getWidth() / rcChildren.getWidth();
            m_pAutoScrollH->SetData(_min, _max, _value, _rate);
        }
        //
        // 		if (bScrollH)
        // 		{
        // 			rcChildren.right--;
        // 		}
        //
        // 		if (bScrollV)
        // 		{
        // 			rcChildren.bottom--;
        // 		}
    }
}

void RtwWidget::OnCreate() {
    RtwEventDelegate e(etCreate);
    EvCreate(this, e);

    OnMove(RtwRect(0, 0, -1, -1));
}

void RtwWidget::OnClose() {
    RtwEventDelegate e(etClose);
    EvClose(this, e);
}

void RtwWidget::OnShow() {
    RtwEventDelegate e(etShow);
    EvShow(this, e);
}

void RtwWidget::OnHide() {
    RtwEventDelegate e(etHide);
    EvHide(this, e);
}

//移动后需重新计算
void RtwWidget::OnMove(const RtwRect& OrigFrameRect) {
    CalcClient();
}

void RtwWidget::OnOffset(const SSize& offset) {}

//重置提示框
void RtwWidget::ResetHint(bool bShow) {
    if (bShow) {
        OnHint();
    } else {
        if (m_pHint) {
            m_pHint->Hide();
        }
    }
}

//lmk hint事件 html xml解析
void RtwWidget::OnHint() {
    RtwEventDelegate e(etHint);
    EvHint(this, e);

    RtwWidget* pHint = g_workspace.getDefaultHint();
    if (!m_pHint || (m_pHint == g_workspace.getDefaultHint() && m_HintText.empty()))
        return;

    m_MouseLeave = true;
    m_pHint->SetWidgetRenderTarget(mnRenderTargetIndex);
    m_pHint->Show();
    g_workspace.SetHintWidget(m_pHint);

    if (m_pHint == g_workspace.getDefaultHint()) {
        //默认hint
        if (m_pHint->getWidgetType() == wtEditBox &&
            !strcmp(m_HintText.substr(0, 6).c_str(), "<html>")) {
            //html
            ((RtwTextBox*)m_pHint)->SetAutoSizeH(false);
            ((RtwTextBox*)m_pHint)->SetAutoSizeV(true);
            ((RtwTextBox*)m_pHint)->SetHtmlText(m_HintText);
        } else if (m_pHint->getWidgetType() == wtEditBox &&
                   !strcmp(m_HintText.substr(0, 5).c_str(), "<xml>")) {
            //xml
            ((RtwTextBox*)m_pHint)->SetAutoSizeH(false);
            ((RtwTextBox*)m_pHint)->SetAutoSizeV(true);
            ((RtwTextBox*)m_pHint)->SetRichText(m_HintText);
        } else {
            ((RtwTextBox*)m_pHint)->SetAutoSizeH(true);
            ((RtwTextBox*)m_pHint)->SetAutoSizeV(true);
            ((RtwTextBox*)m_pHint)->SetWidgetSize(SSize(400, 400));
            m_pHint->SetText(m_HintText);
        }
    }

    AutoHintPosition();
}

//输入框hint
void RtwWidget::OnEditBoxHint() {

    if (!m_pHint || (m_pHint == g_workspace.getDefaultHint() && m_HintText.empty()))
        return;

    m_MouseLeave = true;

    m_pHint->Show();
    g_workspace.BringWidgetToTop(m_pHint);
    RtwEventDelegate e(etHint);
    EvHint(this, e);
}

void RtwWidget::OnMouseMove(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvMouseMove(this, e);
    }
}

void RtwWidget::OnMouseWheel(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvMouseWheel(this, e);
    }
}

void RtwWidget::OnMouseEnter(const RtwEventDelegate& e) {}

void RtwWidget::OnMouseLeave(const RtwEventDelegate& e) {}

void RtwWidget::OnMouseCapture(const RtwEventDelegate& e) {
    if (getEnable()) {
        if (e.type == etButtonPressed) {
            EvMouseCapture(this, e);
        }
    }
}

void RtwWidget::OnMouseDown(const RtwEventDelegate& e) {
    if (getEnable()) {
        if (e.mouse.button == buttonLeft)
            EvMouseLDown(this, e);
        else if (e.mouse.button == buttonRight)
            EvMouseRDown(this, e);
        else
            return;
    }
}

void RtwWidget::OnMouseUp(const RtwEventDelegate& e) {
    if (getEnable()) {
        if (e.mouse.button == buttonLeft)
            EvMouseLUp(this, e);
        else if (e.mouse.button == buttonRight)
            EvMouseRUp(this, e);
        else
            return;
    }
}

void RtwWidget::OnMouseClick(const RtwEventDelegate& e) {
    if (getEnable()) {
        if (e.mouse.button == buttonLeft)
            EvLClick(this, e);
        else if (e.mouse.button == buttonRight)
            EvRClick(this, e);
        else
            return;
    }
}

void RtwWidget::OnMouseDClick(const RtwEventDelegate& e) {
    if (getEnable()) {
        if (e.mouse.button == buttonLeft)
            EvLDClick(this, e);
        else if (e.mouse.button == buttonRight)
            return;
        else
            return;
    }
}

void RtwWidget::OnDraw() {
    RtwEventDelegate e(etDraw);
    EvDraw(this, e);
}

void RtwWidget::OnKeyDown(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvKey(this, e);
    }
}

void RtwWidget::OnKeyUp(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvKeyUp(this, e);
    }
}

//字符输入
void RtwWidget::OnKeyChar(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvKeyChar(this, e);
        //策划改变需求
        // 		// esc键后, 向上找带标题栏的Form，如果找到就Hide此Form
        // 		if (e.key.code == vkEscape)
        // 		{
        // 			RtwWidget* pWidget = this;
        // 			while (pWidget)
        // 			{
        // 				if (pWidget->getWidgetType() == wtForm)
        // 				{
        // 					RtwForm* pForm = (RtwForm*)pWidget;
        // 					if (pForm->getShowTitleBar() && pForm->GetEscapeHide())
        // 					{
        // 						pForm->Hide();
        // 						break;
        // 					}
        // 				}
        // 				pWidget = pWidget->getParent();
        // 			}
        // 		}
    }
}

void RtwWidget::OnDragIn(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvDrag(this, e);
    }
}

void RtwWidget::OnDragOut(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvDragOut(this, e);
    }
}

void RtwWidget::OnDragNull(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvDragEnd(this, e);
    }
}

const std::string RtwWidget::getShortName() const {
    size_t pos = m_Name.find_last_of('.');
    if (pos != std::string::npos) {
        std::string strShortName = m_Name.substr(pos + 1);
        return strShortName;
    } else {
        return m_Name;
    }
}

const std::string RtwWidget::GetID() const {
    size_t pos = m_Name.find_last_of('.');
    if (pos != std::string::npos) {
        std::string strShortName = m_Name.substr(pos + 1);
        return strShortName;
    } else {
        return m_Name;
    }
}

//设置自己为焦点
void RtwWidget::SetFocus() {
    g_workspace.SetFocusWidget(this);
}

CUiLayer* RtwWidget::getLayer() {
    RtwWidget* pParent = m_pParent;
    while (pParent) {
        if (pParent->getWidgetType() == wtLayer)
            return (CUiLayer*)pParent;

        pParent = pParent->m_pParent;
    }

    return NULL;
}

CUiMediaFrame* RtwWidget::getMediaFrame() {
    RtwWidget* pParent = m_pParent;
    while (pParent) {
        if (pParent->getWidgetType() == wtMediaFrame)
            return (CUiMediaFrame*)pParent;

        pParent = pParent->m_pParent;
    }

    return NULL;
}

//锚点处理
void RtwWidget::OnMove_AnchorWidget(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    RtwRect rcFrame(0, 0, m_WidgetSize.width, m_WidgetSize.height);

    foreach (std::list<SUiAnchor>, iterAnchor, m_Anchors) {
        SUiAnchor& Anchor = *iterAnchor;
        RtwWidget* pRelativeWidget = NULL;

        bool bNeedDrop = false;

        if (!Anchor.RelativeWidgetName.empty()) {
            if (pWidget && Anchor.RelativeWidgetName == pWidget->getName())
                pRelativeWidget = pWidget;
            else
                bNeedDrop = g_workspace.FindWidget(Anchor.RelativeWidgetName, &pRelativeWidget);
        }
        RtwRect rcAnchor =
            pRelativeWidget ? pRelativeWidget->GetFrameRect() : g_workspace.getViewportRect();
        if (bNeedDrop)
            DROP_RTUI_OBJECT(pRelativeWidget);

        SPoint pointAnchor;  // 锚定的位置
        switch (Anchor.RelativePoint) {
            case anchorLeft:
                pointAnchor.x = rcAnchor.left;
                break;
            case anchorRight:
                pointAnchor.x = rcAnchor.right;
                break;
            case anchorTop:
                pointAnchor.y = rcAnchor.top;
                break;
            case anchorBottom:
                pointAnchor.y = rcAnchor.bottom;
                break;
            case anchorCenterV:
                pointAnchor.x = rcAnchor.getCenter().x;
                break;
            case anchorCenterH:
                pointAnchor.y = rcAnchor.getCenter().y;
                break;
            default:
                break;
        }
        pointAnchor += Anchor.Offset;

        switch (Anchor.Point) {
            case anchorLeft:
                rcFrame.left = pointAnchor.x;
                if (m_WidgetSize.width)
                    rcFrame.right = rcFrame.left + m_WidgetSize.width - 1;
                if (pointAnchor.y != 0) {
                    rcFrame.top = pointAnchor.y;
                    rcFrame.bottom = rcFrame.top + m_WidgetSize.height - 1;
                }
                break;
            case anchorRight:
                rcFrame.right = pointAnchor.x;
                if (m_WidgetSize.width)
                    rcFrame.left = rcFrame.right - m_WidgetSize.width + 1;
                if (pointAnchor.y != 0) {
                    rcFrame.top = pointAnchor.y;
                    rcFrame.bottom = rcFrame.top + m_WidgetSize.height - 1;
                }
                break;
            case anchorTop:
                rcFrame.top = pointAnchor.y;
                if (m_WidgetSize.height)
                    rcFrame.bottom = rcFrame.top + m_WidgetSize.height - 1;
                if (pointAnchor.x != 0) {
                    rcFrame.left = pointAnchor.x;
                    rcFrame.right = rcFrame.left + m_WidgetSize.width - 1;
                }
                break;
            case anchorBottom:
                rcFrame.bottom = pointAnchor.y;
                if (m_WidgetSize.height)
                    rcFrame.top = rcFrame.bottom - m_WidgetSize.height + 1;
                if (pointAnchor.x != 0) {
                    rcFrame.left = pointAnchor.x;
                    rcFrame.right = rcFrame.left + m_WidgetSize.width - 1;
                }
                break;
            case anchorCenterV:
                rcFrame.left = pointAnchor.x - (m_WidgetSize.width >> 1);
                if (m_WidgetSize.width)
                    rcFrame.right = rcFrame.left + m_WidgetSize.width - 1;
                if (pointAnchor.y != 0) {
                    rcFrame.top = pointAnchor.y;
                    rcFrame.bottom = rcFrame.top + m_WidgetSize.height - 1;
                }
                break;
            case anchorCenterH:
                rcFrame.top = pointAnchor.y - (m_WidgetSize.height >> 1);
                if (m_WidgetSize.height)
                    rcFrame.bottom = rcFrame.top + m_WidgetSize.height - 1;
                if (pointAnchor.x != 0) {
                    rcFrame.left = pointAnchor.x;
                    rcFrame.right = rcFrame.left + m_WidgetSize.width - 1;
                }
                break;
            default:
                break;
        }
    }

    Move(rcFrame, true);
}

//添加锚点
void RtwWidget::AddAnchor(SUiAnchor& Anchor, RtwWidget* pRelativeWidget) {
    m_Anchors.push_back(Anchor);

    RtwEventDelegate e(etMove);
    OnMove_AnchorWidget(pRelativeWidget, &e);

    if (pRelativeWidget) {
        pRelativeWidget->EvMove += RTW_CALLBACK(this, RtwWidget, OnMove_AnchorWidget);
        pRelativeWidget->EvOffset += RTW_CALLBACK(this, RtwWidget, OnMove_AnchorWidget);

        _RegisterEvent(pRelativeWidget, &(pRelativeWidget->EvMove),
                       RTW_CALLBACK(this, RtwWidget, OnMove_AnchorWidget));
        _RegisterEvent(pRelativeWidget, &(pRelativeWidget->EvOffset),
                       RTW_CALLBACK(this, RtwWidget, OnMove_AnchorWidget));
    } else {
        g_workspace.EvMove += RTW_CALLBACK(this, RtwWidget, OnMove_AnchorWidget);
    }
}

//貌似没有实质性作用
//begin
bool SortFunc_Render(RtwWidget* pWidget1, RtwWidget* pWidget2) {
    bool bMouseEvent1 = pWidget1->GetFlags(wfGrabMouse);
    bool bMouseEvent2 = pWidget2->GetFlags(wfGrabMouse);

    if (bMouseEvent1 && !bMouseEvent2)
        return true;
    else
        return false;
}

void RtwWidget::_SortChildrenForRender() {
    if (m_bNeedSortForRender) {
        if (m_pChildren) {
            m_pChildren->getWidgetList()->sort(SortFunc_Render);
            m_bNeedSortForRender = false;
        }
    }
}

//end

bool RtwWidget::isChildOf(RtwWidget* pWidget) {
    RtwWidget* pParent = m_pParent;
    while (pParent && pParent->getWidgetType() != wtLayer) {
        if (pParent == pWidget)
            return true;

        pParent = pParent->m_pParent;
    }

    return false;
}

//事件注册
void RtwWidget::_RegisterEvent(RtwWidget* pWidget, RtwEventDispatcher* _pDispatcher,
                               CUiDelegate& _Delegate) {
    DWORD WidgetId = pWidget ? pWidget->getId() : 0;

    std::list<SRegisteredEvent>::iterator iterRE = m_RegisteredEvents.begin();
    for (; iterRE != m_RegisteredEvents.end(); ++iterRE) {
        SRegisteredEvent& re = *iterRE;
        if (re.WidgetId == WidgetId && re.pDispatcher == _pDispatcher &&
            re.Delegate_.IsEqual(_Delegate))
            return;
    }

    SRegisteredEvent re;
    re.WidgetId = WidgetId;
    re.pDispatcher = _pDispatcher;
    re.Delegate_ = _Delegate;
    m_RegisteredEvents.push_back(re);
}

//删除所有事件
void RtwWidget::_UnregisterAllEvents() {
    std::list<SRegisteredEvent>::iterator iterRE = m_RegisteredEvents.begin();
    for (; iterRE != m_RegisteredEvents.end(); ++iterRE) {
        SRegisteredEvent& re = *iterRE;
        if (g_workspace.getWidgetFactory()->FindWidget(re.WidgetId)) {
            if (re.pDispatcher)
                *(re.pDispatcher) -= re.Delegate_;
        }
    }

    m_RegisteredEvents.clear();
}

//可用否
bool RtwWidget::getEnable() {
    return GetFlags(wfEnable);
}

//可见?
bool RtwWidget::getVisible(bool bParentRelated /*= true */) {
    return getFlag(wfVisible, bParentRelated);
}

//获取标记位
bool RtwWidget::GetFlags(int flag) {
    RtwWidget* pTmp = this;
    while (pTmp) {
        if ((pTmp->m_Flag & flag) != flag)
            return false;
        pTmp = pTmp->GetParent();
    }
    return true;
}

//递归往上获取标记位
bool RtwWidget::getFlag(int flag, bool bParentRelated /* = true*/) {
    if (!bParentRelated) {
        return (m_Flag & flag) == flag;
    } else {
        if (flag == wfHint || flag == wfDragOut || flag == wfDragIn || flag == wfDClick) {
            return (m_Flag & flag) == flag;
        } else {
            RtwWidget* pTmp = this;
            while (pTmp) {
                if ((pTmp->m_Flag & flag) != flag)
                    return false;
                pTmp = pTmp->getParent();
            }
            return true;
        }
    }
}

//计算子组件矩形
RtwRect RtwWidget::CalcChildrenRect() {
    bool    bSet = false;
    RtwRect rcMax = m_rcFrame;

    if (!m_pChildren || m_pChildren->getWidgetCount() == 0)
        return rcMax;

    for (UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
         iter != m_pChildren->m_Widgets.end(); ++iter) {
        RtwWidget* pWidget = *iter;

        //if (!pWidget->getJoinParentAutoScroll())
        //	continue;

        if (!pWidget->getVisible())
            continue;

        RtwRect& rcFrame = pWidget->GetFrameRect();
        if (!rcFrame.isValid())
            continue;

        rcMax.Expand(pWidget->CalcChildrenRect());
        rcMax.Expand(rcFrame);

        //if (!bSet)
        //{
        //	rcMax = rcFrame;
        //	bSet = true;
        //}
        //else
        //{
        //	if (rcFrame.right	> rcMax.right)
        //		rcMax.right		= rcFrame.right;

        //	if (rcFrame.bottom	> rcMax.bottom)
        //		rcMax.bottom	= rcFrame.bottom;

        //	if (rcFrame.left	< rcMax.left)
        //		rcMax.left		= rcFrame.left;

        //	if (rcFrame.top		< rcMax.top)
        //		rcMax.top		= rcFrame.top;
        //}
    }

    return rcMax;
}

//创建自动纵向滚动条
void RtwWidget::SetAutoScrollV(bool bAutoScrollV) {
    if (m_bAutoScrollV == bAutoScrollV)
        return;

    m_bAutoScrollV = bAutoScrollV;
    if (m_bAutoScrollV) {
        if (!m_pAutoScrollV) {
            //如果没有,则从系统中查找
            RtwVScrollBar* pSbv = NULL;
            UI_ENSURE(g_workspace.FindWidget(g_SystemWidgetName[sw_AutoSbv], (RtwWidget**)&pSbv));

            m_pAutoScrollV =
                (RtwVScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarV, "$Sbv");
            UI_ENSURE(m_pAutoScrollV);
            UI_ENSURE(m_pAutoScrollV->CloneFrom(pSbv));
            DROP_RTUI_OBJECT(pSbv);
            m_pAutoScrollV->Show();
            m_pAutoScrollV->SetData(0, 0, 0);
            m_pAutoScrollV->SetJoinParentAutoScroll(false);
            AddChild(m_pAutoScrollV);

            m_pAutoScrollV->EvScroll += RTW_CALLBACK(this, RtwWidget, OnAutoScrollV);

            m_pAutoScrollV->SetWidgetSize(SSize(g_workspace.getDefaultScrollVWidth(), 0));

            SUiAnchor anchor;
            anchor.Point = anchorTop;
            anchor.RelativePoint = anchorTop;
            anchor.RelativeWidgetName = getName();
            m_pAutoScrollV->AddAnchor(anchor, this);

            anchor.Reset();
            anchor.Point = anchorRight;
            anchor.RelativePoint = anchorRight;
            anchor.RelativeWidgetName = getName();
            m_pAutoScrollV->AddAnchor(anchor, this);

            anchor.Reset();
            anchor.Point = anchorBottom;
            anchor.RelativePoint = anchorBottom;
            anchor.RelativeWidgetName = getName();

            if (m_pAutoScrollH) {
                anchor.Offset.height = -m_pAutoScrollH->GetFrameRect().getHeight();
            }
            m_pAutoScrollV->AddAnchor(anchor, this);
        }
    } else {
        if (m_pAutoScrollV) {
            RemoveChild(m_pAutoScrollV->getId());
        }
        DROP_RTUI_OBJECT(m_pAutoScrollV);
    }
}

//创建自动横向滚动条
void RtwWidget::SetAutoScrollH(bool bAutoScrollH) {
    if (m_bAutoScrollH == bAutoScrollH)
        return;

    m_bAutoScrollH = bAutoScrollH;
    if (m_bAutoScrollH) {
        if (!m_pAutoScrollH) {
            RtwHScrollBar* pSbh = NULL;
            UI_ENSURE(g_workspace.FindWidget(g_SystemWidgetName[sw_AutoSbh], (RtwWidget**)&pSbh));

            m_pAutoScrollH =
                (RtwHScrollBar*)g_workspace.getWidgetFactory()->createWidget(wtScrollBarH, "$Sbh");
            UI_ENSURE(m_pAutoScrollH);
            UI_ENSURE(m_pAutoScrollH->CloneFrom(pSbh));
            DROP_RTUI_OBJECT(pSbh);
            m_pAutoScrollH->Show();
            m_pAutoScrollH->SetData(0, 0, 0);
            m_pAutoScrollH->SetJoinParentAutoScroll(false);
            AddChild(m_pAutoScrollH);

            m_pAutoScrollH->EvScroll += RTW_CALLBACK(this, RtwWidget, OnAutoScrollH);

            m_pAutoScrollH->SetWidgetSize(SSize(0, g_workspace.getDefaultScrollHHeight()));

            SUiAnchor anchor;
            anchor.Point = anchorLeft;
            anchor.RelativePoint = anchorLeft;
            anchor.RelativeWidgetName = getName();
            m_pAutoScrollH->AddAnchor(anchor, this);

            anchor.Reset();
            anchor.Point = anchorRight;
            anchor.RelativePoint = anchorRight;
            anchor.RelativeWidgetName = getName();
            if (m_pAutoScrollV) {
                anchor.Offset.width = -m_pAutoScrollV->GetFrameRect().getWidth();
            }
            m_pAutoScrollH->AddAnchor(anchor, this);

            anchor.Reset();
            anchor.Point = anchorBottom;
            anchor.RelativePoint = anchorBottom;
            anchor.RelativeWidgetName = getName();
            m_pAutoScrollH->AddAnchor(anchor, this);
        }
    } else {
        if (m_pAutoScrollH) {
            RemoveChild(m_pAutoScrollH->getId());
        }
        DROP_RTUI_OBJECT(m_pAutoScrollH);
    }
}

//自动纵向滚动条事件
void RtwWidget::OnAutoScrollV(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    m_AutoScrollOffset.height += pEvent->scroll.OrigValue - m_pAutoScrollV->getValue();

    _DoAutoScrollV(pEvent->scroll.OrigValue);
}

//自动横向滚动条事件
void RtwWidget::OnAutoScrollH(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    m_AutoScrollOffset.width += pEvent->scroll.OrigValue - m_pAutoScrollH->getValue();

    _DoAutoScrollH(pEvent->scroll.OrigValue);
}

//删除指定的子组件(貌似只是滚动条)
void RtwWidget::OnRemoveChild(RtwWidget* pWidget) {
    if (m_pAutoScrollV == pWidget) {
        DROP_RTUI_OBJECT(m_pAutoScrollV);
        m_bAutoScrollV = false;
    } else if (m_pAutoScrollH == pWidget) {
        DROP_RTUI_OBJECT(m_pAutoScrollH);
        m_bAutoScrollH = false;
    }
}

void RtwWidget::_DoAutoScrollV(int OriginalValue) {
    SSize _OffsetSize;
    _OffsetSize.height = OriginalValue - m_pAutoScrollV->getValue();

    // 要确保Offset过程中调用CalcAutoScroll无效
    bool _bBak = m_bCalcAutoScrollFlag;
    m_bCalcAutoScrollFlag = false;

    if (m_pChildren) {
        for (UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
             iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* _pChild = *iter;
            if (_pChild == m_pAutoScrollV)
                continue;
            if (!_pChild->getJoinParentAutoScroll())
                continue;

            _pChild->Offset(_OffsetSize, false, false);
        }
    }
    m_bCalcAutoScrollFlag = _bBak;
}

void RtwWidget::_DoAutoScrollH(int OriginalValue) {
    SSize _OffsetSize;
    _OffsetSize.width = OriginalValue - m_pAutoScrollH->getValue();

    // 要确保Offset过程中调用CalcAutoScroll无效
    bool _bBak = m_bCalcAutoScrollFlag;
    m_bCalcAutoScrollFlag = false;

    if (m_pChildren) {
        for (UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
             iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* _pChild = *iter;
            if (_pChild == m_pAutoScrollH)
                continue;
            if (!_pChild->getJoinParentAutoScroll())
                continue;

            _pChild->Offset(_OffsetSize, false, false);
        }
    }
    m_bCalcAutoScrollFlag = _bBak;
}

void RtwWidget::SetUltraText1(const std::string& text /* = ""*/,
                              SUiTextAlignment&  align /* = SUiTextAlignment(alignFar, alignFar)*/,
                              int fontSize /* = 0*/, RtwPixel textColor /* = RtwPixel(0xffffffff)*/
) {
    m_UltraText1.Text = text;
    m_UltraText1.Align = align;
    m_UltraText1.FontSize = fontSize;
    m_UltraText1.TextColor = textColor;
}

void RtwWidget::SetUltraText2(const std::string& text /* = ""*/,
                              SUiTextAlignment& align /* = SUiTextAlignment(alignNear, alignNear)*/,
                              int fontSize /* = 0*/, RtwPixel textColor /* = RtwPixel(0xffffffff)*/
) {
    m_UltraText2.Text = text;
    m_UltraText2.Align = align;
    m_UltraText2.FontSize = fontSize;
    m_UltraText2.TextColor = textColor;
}

//移动并改变大小
//begin
void RtwWidget::MoveResize(const RtwRect& rect, bool bAbs /* = false */) {
    Move(rect, bAbs);
    SetWidgetSize(SSize(rect.getWidth(), rect.getHeight()));
}
// 左上角和右下角坐标
void RtwWidget::MoveResize(int x, int y, int height, int bottom, bool bAbs /* = false */) {
    Move(RtwRect(x, y, height, bottom), bAbs);
    SetWidgetSize(SSize(height - x, bottom - y));
}

//end

//设置透明度
void RtwWidget::SetTransparency(const Real& Transparency) {
    m_Transparency = Transparency;
    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->SetTransparency(Transparency);
        }
    }
}

//组件工厂
CUiWidgetFactory::CUiWidgetFactory() : m_NextId(1) {}

CUiWidgetFactory::~CUiWidgetFactory() {
    //释放
    if (!m_LiveWidgets.empty()) {
        //static char strOutput[10240];
        S_MEMDEF(strOutput, 10240)
        S_MEMPROTECTOR(strOutput, 10240, bMP)
        rt2_sprintf(strOutput,
                    "[CUiWidgetFactory::~CUiWidgetFactory] 一些窗口部件没有被销毁(共%d个): \n",
                    (int)m_LiveWidgets.size());
        int counter = 10;
        for (EXT_SPACE::unordered_map<DWORD, RtwWidget*>::iterator iterWidget =
                 m_LiveWidgets.begin();
             iterWidget != m_LiveWidgets.end(); ++iterWidget) {
            RtwWidget* pWidget = iterWidget->second;
            rt2_strcat(strOutput, pWidget->getName().c_str());
            if (iterWidget != m_LiveWidgets.end() && counter > 0)
                rt2_strcat(strOutput, ",");
            if (counter-- <= 0)
                break;
        }
        UI_CHECKEX(strOutput);
    }
}

//生成组件id
DWORD CUiWidgetFactory::grabNextId() {
    return (m_NextId++);
}

//创建
RtwWidget* CUiWidgetFactory::createWidget(EWidgetType type, const std::string& name /* = ""*/) {
    RtwWidget* pWidget = NULL;

    switch (type) {
        case wtWidget:
            pWidget = RT_NEW RtwWidget();
            break;
        case wtLayer:
            pWidget = RT_NEW CUiLayer();
            break;
        case wtForm:
            pWidget = RT_NEW RtwForm();
            break;
        case wtLabel:
            pWidget = RT_NEW RtwLabel();
            break;
        case wtButton:
            pWidget = RT_NEW RtwButton();
            break;
        case wtEditBox:
            pWidget = RT_NEW RtwTextBox();
            break;
        case wtScrollBarV:
            pWidget = RT_NEW RtwVScrollBar();
            break;
        case wtScrollBarH:
            pWidget = RT_NEW RtwHScrollBar();
            break;
        case wtListBox:
            pWidget = RT_NEW RtwListBox();
            break;
        case wtProgressBar:
            pWidget = RT_NEW RtwProgressBar();
            break;
        case wtComboBox:
            pWidget = RT_NEW RtwComboBox();
            break;
        case wtTabCtrl:
            pWidget = RT_NEW RtwTab();
            break;
        case wtCheckButton:
            pWidget = RT_NEW CUiCheckButton();
            break;
        case wtTree:
            pWidget = RT_NEW RtwTree();
            break;
        case wtPopupMenu:
            pWidget = RT_NEW RtwPopupMenu();
            break;
        case wtMediaFrame:
            pWidget = RT_NEW CUiMediaFrame();
            break;
        case wtAliasButton:
            pWidget = RT_NEW RtwAliasButton();
            break;
        case wtHtmlView:
            pWidget = RT_NEW RtwHtmlView();
            break;
        case wtChatBox:
            pWidget = RT_NEW RtwChatBox();
            break;
        case wtChatInputBox:
            pWidget = RT_NEW RtwChatInputBox();
            break;
        case wtHud:
            pWidget = RT_NEW RtwHUD();
            break;
        case wt3dview:
            pWidget = RT_NEW Rtw3DView();
            break;
        default:
            UI_CHECK_P(0);  //"试图创建非法类型的窗口小部件"
            break;
    }

    //创建成功
    if (pWidget){
        pWidget->grab();
    }

    if (pWidget) {
        if (!pWidget->Create(grabNextId())) {
            DEL_ONE(pWidget);
            return NULL;
        }

        if (!name.empty())
            pWidget->SetName(name);

        pWidget->OnCreate();
    }

    if (pWidget) {
        EXT_SPACE::unordered_map<DWORD, RtwWidget*>::iterator iterFinder =
            m_LiveWidgets.find(pWidget->getId());
        UI_CHECK_P(iterFinder == m_LiveWidgets.end());
        m_LiveWidgets[pWidget->getId()] = pWidget;
    }

    return pWidget;
}

//根据id查找组件
bool CUiWidgetFactory::FindWidget(DWORD Id, RtwWidget** ppWidget /* = NULL*/) {
    if (ppWidget)
        *ppWidget = NULL;

    EXT_SPACE::unordered_map<DWORD, RtwWidget*>::iterator iter = m_LiveWidgets.find(Id);
    UI_ENSURE_B(iter != m_LiveWidgets.end());
    if (ppWidget) {
        *ppWidget = iter->second;
        (*ppWidget)->grab();
    }
    return true;
}

//删除组件
void CUiWidgetFactory::OnDeleteWidget(RtwWidget* pWidget) {
    EXT_SPACE::unordered_map<DWORD, RtwWidget*>::iterator iterFinder =
        m_LiveWidgets.find(pWidget->getId());
    UI_CHECK(iterFinder != m_LiveWidgets.end());
    m_LiveWidgets.erase(iterFinder);
}

CUiWidgetContainer::CUiWidgetContainer() {}

CUiWidgetContainer::~CUiWidgetContainer() {}

void CUiWidgetContainer::OnBeforeDestroy() {
    ClearWidgets();

    RtwRefObject::OnBeforeDestroy();
}

//删除所有组件
void CUiWidgetContainer::ClearWidgets() {
    while (!m_Widgets.empty()) {
        RtwWidget* pWidget = m_Widgets.front();
        DROP_RTUI_OBJECT(pWidget);
        m_Widgets.pop_front();
    }
}

//添加一个组件
bool CUiWidgetContainer::AddWidget(RtwWidget* pWidget) {
    //如果已经存在,则退出
    if (FindWidget(pWidget->getId()))
        return false;

    pWidget->grab();
    m_Widgets.push_back(pWidget);
    return true;
}

//顶层渲染
bool CUiWidgetContainer::InsertTopWidget(RtwWidget* pWidget) {
    if (FindWidget(pWidget->getId()))
        return false;

    pWidget->grab();
    m_Widgets.push_front(pWidget);
    return true;
}

//添加另一个容器里的组件
bool CUiWidgetContainer::AddWidget(CUiWidgetContainer* pWidgetContainer) {
    for (UiWidgetList::iterator iter = pWidgetContainer->m_Widgets.begin();
         iter != pWidgetContainer->m_Widgets.end(); ++iter) {
        RtwWidget* pWidget = *iter;
        if (pWidget) {
            AddWidget(pWidget);
        }
    }
    return true;
}

//删除组件(默认不递归,但是貌似不对, 不递归的话,子组件咋办呢)
bool CUiWidgetContainer::RemoveWidget(DWORD WidgetID, bool bRecursive /* = false*/) {
    for (UiWidgetList::iterator iter = m_Widgets.begin(); iter != m_Widgets.end(); ++iter) {
        RtwWidget* pWidget = *iter;
        if (pWidget->getId() == WidgetID) {
            return RemoveWidget(iter);
        } else {
            if (bRecursive) {
                if (pWidget->RemoveChild(WidgetID, true))
                    return true;
            }
        }
    }

    return false;
}

//按名字删除组件
bool CUiWidgetContainer::RemoveWidget(const std::string& WidgetName, bool bRecursive /* = false*/) {
    for (UiWidgetList::iterator iter = m_Widgets.begin(); iter != m_Widgets.end(); ++iter) {
        RtwWidget* pWidget = *iter;
        if (pWidget->getName() == WidgetName) {
            return RemoveWidget(iter);
        } else {
            if (bRecursive) {
                if (pWidget->RemoveChild(WidgetName, true))
                    return true;
            }
        }
    }

    return false;
}

//根据迭代器删除
bool CUiWidgetContainer::RemoveWidget(UiWidgetList::iterator& iter) {
    RtwWidget* pWidget = *iter;
    DROP_RTUI_OBJECT(pWidget);
    m_Widgets.erase(iter);
    return true;
}

//在容器里根据id 查找组件
bool CUiWidgetContainer::FindWidget(DWORD WidgetID, RtwWidget** ppWidget /* = NULL*/,
                                    bool bRecursive /* = false*/) {
    if (ppWidget)
        *ppWidget = NULL;

    for (UiWidgetList::iterator iterFinder = m_Widgets.begin(); iterFinder != m_Widgets.end();
         ++iterFinder) {
        RtwWidget* pWidget = *iterFinder;
        if (pWidget->getId() == WidgetID) {
            if (ppWidget) {
                RtwWidget* pWidget = *iterFinder;
                pWidget->grab();
                *ppWidget = pWidget;
            }
            return true;
        } else {
            if (bRecursive) {
                if (pWidget->FindChild(WidgetID, ppWidget, true)) {
                    return true;
                }
            }
        }
    }

    return false;
}

//查找组件,根据迭代器
bool CUiWidgetContainer::FindWidget(DWORD WidgetID, UiWidgetList::iterator& iter) {
    for (UiWidgetList::iterator iterFinder = m_Widgets.begin(); iterFinder != m_Widgets.end();
         ++iterFinder) {
        RtwWidget* pWidget = *iterFinder;
        if (pWidget->getId() == WidgetID) {
            iter = iterFinder;
            return true;
        }
    }

    return false;
}

//查找组件,根据名字
bool CUiWidgetContainer::FindWidget(const std::string& WidgetName, RtwWidget** ppWidget /* = NULL*/,
                                    bool bRecursive /* = false*/) {
    if (ppWidget)
        *ppWidget = NULL;

    for (UiWidgetList::iterator iterFinder = m_Widgets.begin(); iterFinder != m_Widgets.end();
         ++iterFinder) {
        RtwWidget* pWidget = *iterFinder;
        if (pWidget->getShortName() == WidgetName || pWidget->getName() == WidgetName) {
            if (ppWidget) {
                RtwWidget* pWidget = *iterFinder;
                pWidget->grab();
                *ppWidget = pWidget;
            }
            return true;
        } else {
            if (bRecursive) {
                if (pWidget->FindChild(WidgetName, ppWidget, true)) {
                    return true;
                }
            }
        }
    }

    return false;
}

//在index前插入组件
bool CUiWidgetContainer::InsertWidget(RtwWidget* pWidget, int indexBefore) {
    if (indexBefore < 0 || indexBefore > (int)m_Widgets.size())
        return false;

    UiWidgetList::iterator iter = m_Widgets.begin();
    for (int i = 0; i < indexBefore; ++i) {
        iter++;
    }
    m_Widgets.insert(iter, pWidget);
    pWidget->grab();
    return true;
}

//迭代器开始
UiWidgetList::iterator CUiWidgetContainer::Begin() {
    return m_Widgets.begin();
}

//迭代器结束
UiWidgetList::iterator CUiWidgetContainer::End() {
    return m_Widgets.end();
}

//反向迭代器开始
UiWidgetList::reverse_iterator CUiWidgetContainer::Begin_Reverse() {
    return m_Widgets.rbegin();
}

//反向迭代器结束
UiWidgetList::reverse_iterator CUiWidgetContainer::End_Reverse() {
    return m_Widgets.rend();
}

//数组形式返回组件
RtwWidget* CUiWidgetContainer::operator[](int index) {
    if (index < 0 || index >= (int)m_Widgets.size())
        return NULL;

    UiWidgetList::iterator iter = m_Widgets.begin();
    for (int i = 0; i < index; ++i) {
        iter++;
    }
    return *iter;
}

//将指定id的组件置于顶层
bool CUiWidgetContainer::BringWidgetToTop(DWORD WidgetID) {
    UiWidgetList::iterator iter;
    UI_ENSURE_B(FindWidget(WidgetID, iter));

    RtwWidget* pWidget = *iter;
    pWidget->grab();

    UI_CHECK_B(RemoveWidget(iter));
    UI_CHECK_B(InsertWidget(pWidget, 0));

    DROP_RTUI_OBJECT(pWidget);

    return true;
}

//将指定id的组件置于底层
bool CUiWidgetContainer::BringWidgetToBottom(DWORD WidgetID) {
    UiWidgetList::iterator iter;
    UI_ENSURE_B(FindWidget(WidgetID, iter));

    RtwWidget* pWidget = *iter;
    pWidget->grab();

    UI_CHECK_B(RemoveWidget(iter));
    UI_CHECK_B(AddWidget(pWidget));

    DROP_RTUI_OBJECT(pWidget);

    return true;
}

//运行
void CUiWidgetContainer::Run(DWORD dwDelta) {
    if ((int)m_Widgets.size() > 0) {
        for (UiWidgetList::iterator iterWidget = m_Widgets.begin(); iterWidget != m_Widgets.end();
             ++iterWidget) {
            RtwWidget* pWidget = *iterWidget;
            if (pWidget) {
                pWidget->Run(dwDelta);
            }
        }
    }
}

//渲染
void CUiWidgetContainer::Render(CUiWidgetContainer* pExcludeWidgets /* = NULL*/) {
    if ((int)m_Widgets.size() > 0) {
        for (UiWidgetList::reverse_iterator iterWidget = m_Widgets.rbegin();
             iterWidget != m_Widgets.rend(); ++iterWidget) {
            RtwWidget* pWidget = *iterWidget;
            if (pExcludeWidgets && pExcludeWidgets->FindWidget(pWidget->getId()))
                continue;

            if (pWidget) {
                pWidget->Render();
            }
        }
    }
}

//打印组件结构
void CUiWidgetContainer::PrintToConsole(const std::string& name,
                                        const std::string& prefix /* = ""*/) {
    RtCoreLog().Info("[WidgetContainer %s]\n", name.c_str());
    std::string            prefixWidget = prefix + "    ";
    UiWidgetList::iterator iter = Begin();
    for (; iter != End(); ++iter) {
        RtwWidget* pWidget = *iter;
        pWidget->PrintToConsole(prefixWidget);
    }
    RtCoreLog().Info("\n");
}

//捡取
bool CUiWidgetContainer::PickWidget(const SPoint& Position, RtwWidget** ppWidget /* = NULL*/,
                                    DWORD               FlagMask /* = 0x00000000*/,
                                    CUiWidgetContainer* pExcludeWidgets /* = NULL*/
) {
    if (ppWidget)
        *ppWidget = NULL;

    for (UiWidgetList::iterator iterWidget = Begin(); iterWidget != End(); ++iterWidget) {
        RtwWidget* pWidget = *iterWidget;

        if (!pWidget->getVisible())
            continue;
        if ((pWidget->GetFlags() & FlagMask) != FlagMask)
            continue;

        if (pWidget->PickWidget(Position, ppWidget, FlagMask, pExcludeWidgets))
            return true;
    }

    return false;
}
