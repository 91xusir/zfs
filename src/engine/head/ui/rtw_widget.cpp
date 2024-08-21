#include "ui/rtw_ui.h"
//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_widget.cpp
//	author:		ldr123
//	purpose:	������༰���������
//  other:		�ֲ��ع����ע��
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
    //ժ���븸�ڵ�Ĺ�ϵ
    _SetParent(NULL);
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            //�ӿؼ�Ҳժ��
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

    //ж�������¼�
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
    m_pParent = pParent;  //���ø����

    if (m_pParent) {
        std::string prefix = m_pParent->getName();
        prefix += ".";
        //���ñ����������ǰ׺
        _AddNamePrefix(prefix);
    }
}

//�����������ǰ׺
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

//ɾ������ǰ׺
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

    //�����
    m_pChildren = RT_NEW CUiWidgetContainer();
    m_pChildren->grab();

    return true;
}

void RtwWidget::Run(DWORD dwDelta) {
    //ͼƬ�����л�ͼƬ
    if (m_pImageSequence)
        m_pImageSequence->Tick(dwDelta);

    //����ͼ�л�
    if (m_pBackGround)
        m_pBackGround->Tick(dwDelta);

    //�������run
    if (m_pChildren)
        m_pChildren->Run(dwDelta);

    if (m_bShowing || m_bHiding) {
        //���ڴ� or ���ڹر�
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

        //���뵭��
        TransparentByPos(fPos);

        //����
        MoveResizeByPos(fPos);

        if (fStartingTime > m_nShowTime) {
            //��ʾ����
            m_bShowing = false;
            fStartingTime = 0.0;
        }

        if (fHidingTime > m_nShowTime) {
            //�رս���
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

//����
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

//����͸����
void RtwWidget::TransparentByPos(float fPos) {
    if (m_bTrans) {
        SetTransparency(fPos);
    }
}

//����ǰ�������Ϊ����
void RtwWidget::Enable() {
    if (!getFlag(wfEnable, false)) {
        ModifyFlags(wfEnable, 0);
    }

    //���ӿؼ�ȫ������
    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->Enable();
        }
    }
}

//����
void RtwWidget::Disable() {
    if (getFlag(wfEnable, false)) {
        ModifyFlags(0, wfEnable);
    }

    //�����ӿؼ�
    if (m_pChildren) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->Disable();
        }
    }
}

//������ʾ�ı�
void RtwWidget::SetText(const std::string& text) {
    m_Text = text;
}

void RtwWidget::SetName(const std::string& name, const std::string& replacedName /* = ""*/) {
    /////////////////////
    m_base_name = name;
    /////////////////////
    // ֻ������һ�Σ������Ըı�
    // [����������.]�Լ�����/

    std::string _Replaced;
    std::string OldName = m_Name;

    // �滻(Ӱ���Լ������ֻ���Ӱ�츸�׵�����)������������
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

    // ���������Ӵ�������
    if (m_pChildren && !_Replaced.empty()) {
        UiWidgetList::iterator iter = m_pChildren->m_Widgets.begin();
        for (; iter != m_pChildren->m_Widgets.end(); ++iter) {
            RtwWidget* pWidget = *iter;
            pWidget->SetName(name, _Replaced);
        }
    }
}

//�����ı����뷽ʽ
void RtwWidget::SetTextAlignment(SUiTextAlignment& align) {
    m_TextAlign = align;
}

//���������С
void RtwWidget::SetWidgetSize(SSize& size) {
    m_rcFrame.right = m_rcFrame.left + size.width;
    m_rcFrame.bottom = m_rcFrame.top + size.height;

    m_rcClient.right = m_rcClient.left + size.width;
    m_rcClient.bottom = m_rcClient.top + size.height;

    m_WidgetSize = size;

    Refresh();
}

//���ñ���
void RtwWidget::SetCaption(const std::string& caption) {
    m_Caption = caption;
}

//���ñ�־λ
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
    // �ɼ�              ����������״̬�Ƿ��ǵ�ǰ״̬���Ӽ�        ���������������Ƿ��ڿؼ���
    if (getVisible() && ((GetFlags() & FlagMask) == FlagMask) && m_rcFrame.IsContained(Position)) {
        if (ppWidget)  //���ppWidget����,���Լ�������
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

        //�ؼ����Ͳ��ǲ㲢��(�ؼ�������Ϊ�ջ��ҵ���������һ���ؼ�)
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

    //�ӿؼ��ĸ��ڵ����Լ�
    pWidget->_SetParent(this);

    m_bNeedSortForRender = true;

    // �����Child�ĳ�ʼ��Ϣ
    pWidget->Move(SPoint(pWidget->getClientRect().left, pWidget->getClientRect().top));

    return true;
}

bool RtwWidget::AddChild(CUiWidgetContainer* pWidgetContainer) {
    if (m_pChildren)
        return m_pChildren->AddWidget(pWidgetContainer);

    return false;
}

//���ø����
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

        // ������������ط����ã�Ϊ�˱����widget�������״̬���Ͱ������뵽GarbageContianer��
        if (pWidget->getRefCount() > 1) {
            g_workspace.AddGarbageWidget(pWidget);
        }

        DROP_RTUI_OBJECT(pWidget);
    }

    return true;
}

//��������ɾ�������
bool RtwWidget::RemoveChild(const std::string& WidgetName, bool bRecursive /* = false*/) {
    RtwWidget* pWidget = NULL;
    if (!m_pChildren && !m_pChildren->FindWidget(WidgetName, &pWidget, bRecursive)) {
        return true;
    }

    m_pChildren->RemoveWidget(WidgetName, bRecursive);

    OnRemoveChild(pWidget);

    pWidget->_SetParent(NULL);

    // ������������ط����ã�Ϊ�˱����widget�������״̬���Ͱ������뵽GarbageContianer��
    if (pWidget->getRefCount() > 1) {
        g_workspace.AddGarbageWidget(pWidget);
    }

    DROP_RTUI_OBJECT(pWidget);

    return true;
}

//ɾ���������
bool RtwWidget::RemoveAllChildren(CUiWidgetContainer* pExcludeWidgets /* = NULL*/) {
    std::list<DWORD> WidgetsIdToRemove;
    if (m_pChildren) {
        for (UiWidgetList::iterator iterWidget = m_pChildren->m_Widgets.begin();
             iterWidget != m_pChildren->m_Widgets.end(); ++iterWidget) {
            RtwWidget* pWidget = *iterWidget;
            if (pWidget == m_pAutoScrollV || pWidget == m_pAutoScrollH)  // ��ɾ���Զ�������
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

//����id���������
bool RtwWidget::FindChild(DWORD WidgetID, RtwWidget** ppWidget /* = NULL*/,
                          bool bRecursive /* = false*/) {
    if (m_pChildren)
        return m_pChildren->FindWidget(WidgetID, ppWidget, bRecursive);
    return false;
}

//������������������
bool RtwWidget::FindChild(const std::string& WidgetName, RtwWidget** ppWidget /* = NULL*/,
                          bool bRecursive /* = false*/) {
    if (m_pChildren)
        return m_pChildren->FindWidget(WidgetName, ppWidget, bRecursive);

    return false;
}

//������������������
RtwWidget* RtwWidget::GetChildWidget(const std::string& WidgetName) {
    RtwWidget* pRetWg = NULL;
    if (FindChild(WidgetName, &pRetWg, true))
        return pRetWg;

    return pRetWg;
}

//���ÿ�����Ч(��ʾ,�ر�)
void RtwWidget::SetWidgetEffect(int nShowEffect, int nHideEffect, int nDelay) {
    m_nShowEffectLst = nShowEffect;
    m_nHideEffectLst = nHideEffect;
    m_nShowEffectLstBack = m_nShowEffectLst;
    m_nHideEffectLstBack = nHideEffect;
    m_nShowTime = nDelay;
}

//��ʾ
void RtwWidget::Show() {
    if (!getFlag(wfVisible, false)) {
        ModifyFlag(wfVisible, 0);

        // ����Ǵ���������Form,��Widget�ᵽ��ǰ����ʾ
        if (getWidgetType() == wtForm) {
            RtwForm* pForm = (RtwForm*)this;

            //��ʾform��titlebar?
            if (pForm->getShowTitleBar()) {
                //���û�и����
                if (!pForm->getParent()) {
                    g_workspace.BringWidgetToTop(this);
                }
            }

            //��Ч����
            if (m_nShowEffectLst != esNormal) {
                m_bShowing = true;
                //͸����Ч
                if ((m_nShowEffectLst & esTrans) == esTrans) {
                    m_bTrans = true;
                }

                //������Ч
                if ((m_nShowEffectLst & esZoom) == esZoom) {
                    m_bZoom = true;
                }
            }
        }

        m_backRc = m_rcFrame;
        OnShow();
    }
}

//��ʾ�����ý���
void RtwWidget::ShowAndFocus() {
    Show();
    g_workspace.SetFocusWidget(this);
}

//���ش���
void RtwWidget::Hide() {
    //����Ч�ر�
    m_bHiding = m_bTrans = m_bZoom = false;

    //������?
    if (GetFlags(wfVisible)) {
        static bool bRecorded = false;

        //��Ч����
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
            //�޸���Ч״̬
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

//���ù�����
void RtwWidget::ResetScrollBar() {
    //�Զ����������
    RtwVScrollBar* pAutoScrollV = getAutoScrollV();
    if (pAutoScrollV) {
        //����
        pAutoScrollV->ScrollToTop();
    }

    //�����ȫ������
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            RtwWidget* pWidget = *iterChild;
            pWidget->ResetScrollBar();
        }
    }
}

//ģ̬�Ի�����ʾ
void RtwWidget::ShowModal() {
    if (!getFlag(wfVisible, false)) {
        ModifyFlags(wfVisible, 0);

        g_workspace.AddModalWidget(this);
        g_workspace.SetFocusWidget(this);

        OnShow();

        //��Ч
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

//ˢ��
void RtwWidget::Refresh() {
    //������ʾ����
    CalcClient();

    //���������
    CalcAutoScroll();
}

//�ر�
void RtwWidget::Close() {
    OnClose();
}

//�ƶ�����(���ݸ����Ĵ�С�ƶ�λ��)
//being
void RtwWidget::Offset(const SSize& size, bool bTriggerEvent /* = true*/,
                       bool bAutoScrollParent /* = true*/) {
    //����x,y����
    if (size.width == 0 && size.height == 0)
        return;

    m_rcFrame.Offset(size.width, size.height);
    m_rcClient.Offset(size.width, size.height);

    //����������
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            RtwWidget* pWidget = *iterChild;

            if (pWidget->getAnchorsCount() == 0)  // ���û��ê����Ϣ
            {
                pWidget->Offset(size, bTriggerEvent, bAutoScrollParent);
            }
        }
    }

    OnOffset(size);

    // �׳��¼�
    if (bTriggerEvent) {
        RtwEventDelegate e(etOffset);
        e.offset.width = size.width;
        e.offset.height = size.height;

        EvOffset(this, e);
    }

    // ����������Զ�����,�͸��¸�����
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
    // �����Сδ�����ı䣬�͵���Offset
    if (rect.getWidth() == m_rcFrame.getWidth() && rect.getHeight() == m_rcFrame.getHeight()) {
        SSize sizeOffset(rect.left - m_rcFrame.left, rect.top - m_rcFrame.top);
        if (!bAbs && getParent()) {
            sizeOffset.width += getParent()->GetFrameRect().left;
            sizeOffset.height += getParent()->GetFrameRect().top;
        }

        Offset(sizeOffset, bTriggerEvent, bAutoScrollParent);
        return;
    }

    // Widget������������
    m_rcFrame = rect;
    if (!bAbs && GetParent()) {
        m_rcFrame.Offset(GetParent()->GetFrameRect().getLeftTop());
    }

    RtwRect rcNow = m_rcFrame;
    SSize   OffsetSize(rcNow.left - rcOrig.left, rcNow.top - rcOrig.top);

    // ��Widgetλ��
    if (m_pChildren) {
        UiWidgetList::iterator iterChild = m_pChildren->m_Widgets.begin();
        for (; iterChild != m_pChildren->m_Widgets.end(); ++iterChild) {
            RtwWidget* pWidget = *iterChild;
            if (pWidget->getAnchorsCount() == 0)
                pWidget->Offset(OffsetSize);
        }
    }

    OnMove(rcOrig);

    // �׳��¼�
    if (bTriggerEvent) {
        RtwEventDelegate e(etMove);
        e.move.OrigFrame_Left = rcOrig.left;
        e.move.OrigFrame_Right = rcOrig.right;
        e.move.OrigFrame_Top = rcOrig.top;
        e.move.OrigFrame_Bottom = rcOrig.bottom;
        EvMove(this, e);
    }

    // ����������Զ�����,�͸��¸�����
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

//������ʾ
void RtwWidget::CenterScreen(int nOffsetX /*= 0*/, int nOffsetY /* = 0 */) {
    int w = g_workspace.getRenderDevice()->getViewportRect().getWidth();
    int h = g_workspace.getRenderDevice()->getViewportRect().getHeight();

    int x = ((w - m_rcFrame.getWidth()) >> 1) + nOffsetX;
    int y = ((h - m_rcFrame.getHeight()) >> 1) + nOffsetY;

    Move(SPoint(x, y));
}

//���ñ���ͼ
//begin
void RtwWidget::SetBackgroundImage(RtwImage* pImage) {
    DROP_TRUI_IMAGE(m_pBackGround);
    m_pBackGround = pImage;
    if (m_pBackGround)
        m_pBackGround->border = pImage->border;
    GRAB_TRUI_IMAGE(m_pBackGround);
}

//end

//���ñ���ɫ
void RtwWidget::SetBackgroundColor(RtwPixel color) {
    m_BkColor = color;
}

//����ͼƬ����
void RtwWidget::SetImageSequence(RtwComplexImage* pImageSequence) {
    DROP_TRUI_IMAGE(m_pImageSequence);

    m_pImageSequence = pImageSequence;
    GRAB_TRUI_IMAGE(m_pImageSequence);
}

//��ĳ���������Ϊ��ʾ������
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
//û��ʹ��
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

//������ʾ���ı�
void RtwWidget::SetHintText(const std::string& HintText) {
    if (m_HintText == HintText)
        return;
    m_HintText = HintText;
    if (m_HintText.empty() || m_pHint == 0) {
        SetHint(g_workspace.getDefaultHint());
    }

    m_pHint->SetUseSystemFont(false);

    // ��ֵTargetWidget��ˢ��
    g_workspace.ResetHintTargetWidget();
}

//��̬������ʾ��λ��
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

//�Զ���hint��С(editbox)
void RtwWidget::AutoHintSize() {
    //lyytodo
    return;  //Yֱ��return????
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

//����������ʾ���Զ���С
//begin
void RtwWidget::AutoHtmlHintSize() {
    return;  //ֱ��return

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

//����¡���ӹ�ϵ������¡�¼����� (Ӧ��ɾ�����ж��ӣ����ҿ�¡����)
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

//����Ŀ����Ⱦ���
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
    //ֻ��Ϊ����ʾ
    if (INVALIDE_RENDERTARGET != mnRenderTargetIndex) {
        g_workspace.getRenderDevice()->SetTextureRenderTarget(
            mnRenderTargetIndex, mbClearRenderTaget, mlBackBufferDiffuse);
        if (mbOnlyRenderToShow) {
            Show();
        }
    }

    //���ɼ�
    if (!getVisible(false)) {
        if (mnRenderTargetIndex != INVALIDE_RENDERTARGET) {
            g_workspace.getRenderDevice()->RestorRenderTarget();
            if (mbOnlyRenderToShow)
                Hide();
        }
        return;
    }

    OnDraw();

    //�Ȼ���̨
    DrawBackground(pClipRect);

    //ǰ̨����
    DrawForeground(pClipRect);

    //������Ⱦ
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

//lmk ����ṹ��ӡ
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

//ǰ����ʾ
void RtwWidget::DrawForeground(const RtwRect* pClipRect /* = NULL*/) {
    //lyytodo ����������������ʾ���Ʒ��㿪��ʹ�� ����ɾ��

    if (isDrawUIName && g_workspace.getMouseHover() == this) {
        //RtwRect temp;
        //temp.left = m_rcFrame.left;
        //temp.right = m_rcFrame.right;
        //temp.top = m_rcFrame.top -
        //           20;  // �����εĶ���λ������Ϊ����Ķ�����ȥƫ������20 ��ʾ��ֵ��������Ҫ������
        //temp.bottom = m_rcFrame.top;  // ���εĵײ��붥������
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

    //��ʾ�ı�
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

    //ͼƬ����
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

    //����, ���,�߿�Ϊ��
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

//������
void RtwWidget::DrawBackground(const RtwRect* pClipRect /* = NULL*/) {
    RtwRect rcRender = m_rcFrame;
    if (pClipRect)
        rcRender.ClipBy(*pClipRect);

    //������ɫ
    if (m_BkColor != 0 && m_WidgetType != wtForm) {
        g_workspace.getRenderDevice()->DrawRectangle(rcRender,
                                                     SETCOLORALPHA(m_BkColor, m_Transparency));
    }

    if (g_workspace.getDebugMode()) {
        g_workspace.getRenderDevice()->DrawRectangleBorder(
            m_rcFrame, SETCOLORALPHA(RtwPixel(0xffef4fff), m_Transparency));
    }

    //������ͼ
    if (m_pBackGround) {
        //����ͼ
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

        //�����߿�
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

//��껬��ʱ��ͼƬ
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

//Frame�仯��Сʱ����Ӧ����
void RtwWidget::OnFrameSizeChanged() {
    CalcClient();
}

void RtwWidget::OnNameChanged(const std::string& OldFullName) {}

//������μ���
void RtwWidget::CalcClient() {
    m_rcClient.left = m_rcFrame.left + (m_BorderSize + m_padding.left);
    m_rcClient.right = m_rcFrame.right - (m_BorderSize + m_padding.right);
    m_rcClient.top = m_rcFrame.top + (m_BorderSize + m_padding.top);
    m_rcClient.bottom = m_rcFrame.bottom - (m_BorderSize + m_padding.bottom);
    CalcAutoScroll();
}

//����������
void RtwWidget::CalcAutoScroll() {
    //��ˢ�¹�����
    if (!m_bCalcAutoScrollFlag)
        return;

    if (m_pAutoScrollV || m_pAutoScrollH) {
        //�õ��ӿؼ�����
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

        //�����Զ�������
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

        //�����Զ�������
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

//�ƶ��������¼���
void RtwWidget::OnMove(const RtwRect& OrigFrameRect) {
    CalcClient();
}

void RtwWidget::OnOffset(const SSize& offset) {}

//������ʾ��
void RtwWidget::ResetHint(bool bShow) {
    if (bShow) {
        OnHint();
    } else {
        if (m_pHint) {
            m_pHint->Hide();
        }
    }
}

//lmk hint�¼� html xml����
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
        //Ĭ��hint
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

//�����hint
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

//�ַ�����
void RtwWidget::OnKeyChar(const RtwEventDelegate& e) {
    if (getEnable()) {
        EvKeyChar(this, e);
        //�߻��ı�����
        // 		// esc����, �����Ҵ���������Form������ҵ���Hide��Form
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

//�����Լ�Ϊ����
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

//ê�㴦��
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

        SPoint pointAnchor;  // ê����λ��
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

//���ê��
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

//ò��û��ʵ��������
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

//�¼�ע��
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

//ɾ�������¼�
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

//���÷�
bool RtwWidget::getEnable() {
    return GetFlags(wfEnable);
}

//�ɼ�?
bool RtwWidget::getVisible(bool bParentRelated /*= true */) {
    return getFlag(wfVisible, bParentRelated);
}

//��ȡ���λ
bool RtwWidget::GetFlags(int flag) {
    RtwWidget* pTmp = this;
    while (pTmp) {
        if ((pTmp->m_Flag & flag) != flag)
            return false;
        pTmp = pTmp->GetParent();
    }
    return true;
}

//�ݹ����ϻ�ȡ���λ
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

//�������������
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

//�����Զ����������
void RtwWidget::SetAutoScrollV(bool bAutoScrollV) {
    if (m_bAutoScrollV == bAutoScrollV)
        return;

    m_bAutoScrollV = bAutoScrollV;
    if (m_bAutoScrollV) {
        if (!m_pAutoScrollV) {
            //���û��,���ϵͳ�в���
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

//�����Զ����������
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

//�Զ�����������¼�
void RtwWidget::OnAutoScrollV(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    m_AutoScrollOffset.height += pEvent->scroll.OrigValue - m_pAutoScrollV->getValue();

    _DoAutoScrollV(pEvent->scroll.OrigValue);
}

//�Զ�����������¼�
void RtwWidget::OnAutoScrollH(RtwWidget* pWidget, RtwEventDelegate* pEvent) {
    m_AutoScrollOffset.width += pEvent->scroll.OrigValue - m_pAutoScrollH->getValue();

    _DoAutoScrollH(pEvent->scroll.OrigValue);
}

//ɾ��ָ���������(ò��ֻ�ǹ�����)
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

    // Ҫȷ��Offset�����е���CalcAutoScroll��Ч
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

    // Ҫȷ��Offset�����е���CalcAutoScroll��Ч
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

//�ƶ����ı��С
//begin
void RtwWidget::MoveResize(const RtwRect& rect, bool bAbs /* = false */) {
    Move(rect, bAbs);
    SetWidgetSize(SSize(rect.getWidth(), rect.getHeight()));
}
// ���ϽǺ����½�����
void RtwWidget::MoveResize(int x, int y, int height, int bottom, bool bAbs /* = false */) {
    Move(RtwRect(x, y, height, bottom), bAbs);
    SetWidgetSize(SSize(height - x, bottom - y));
}

//end

//����͸����
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

//�������
CUiWidgetFactory::CUiWidgetFactory() : m_NextId(1) {}

CUiWidgetFactory::~CUiWidgetFactory() {
    //�ͷ�
    if (!m_LiveWidgets.empty()) {
        //static char strOutput[10240];
        S_MEMDEF(strOutput, 10240)
        S_MEMPROTECTOR(strOutput, 10240, bMP)
        rt2_sprintf(strOutput,
                    "[CUiWidgetFactory::~CUiWidgetFactory] һЩ���ڲ���û�б�����(��%d��): \n",
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

//�������id
DWORD CUiWidgetFactory::grabNextId() {
    return (m_NextId++);
}

//����
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
            UI_CHECK_P(0);  //"��ͼ�����Ƿ����͵Ĵ���С����"
            break;
    }

    //�����ɹ�
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

//����id�������
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

//ɾ�����
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

//ɾ���������
void CUiWidgetContainer::ClearWidgets() {
    while (!m_Widgets.empty()) {
        RtwWidget* pWidget = m_Widgets.front();
        DROP_RTUI_OBJECT(pWidget);
        m_Widgets.pop_front();
    }
}

//���һ�����
bool CUiWidgetContainer::AddWidget(RtwWidget* pWidget) {
    //����Ѿ�����,���˳�
    if (FindWidget(pWidget->getId()))
        return false;

    pWidget->grab();
    m_Widgets.push_back(pWidget);
    return true;
}

//������Ⱦ
bool CUiWidgetContainer::InsertTopWidget(RtwWidget* pWidget) {
    if (FindWidget(pWidget->getId()))
        return false;

    pWidget->grab();
    m_Widgets.push_front(pWidget);
    return true;
}

//�����һ������������
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

//ɾ�����(Ĭ�ϲ��ݹ�,����ò�Ʋ���, ���ݹ�Ļ�,�����զ����)
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

//������ɾ�����
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

//���ݵ�����ɾ��
bool CUiWidgetContainer::RemoveWidget(UiWidgetList::iterator& iter) {
    RtwWidget* pWidget = *iter;
    DROP_RTUI_OBJECT(pWidget);
    m_Widgets.erase(iter);
    return true;
}

//�����������id �������
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

//�������,���ݵ�����
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

//�������,��������
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

//��indexǰ�������
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

//��������ʼ
UiWidgetList::iterator CUiWidgetContainer::Begin() {
    return m_Widgets.begin();
}

//����������
UiWidgetList::iterator CUiWidgetContainer::End() {
    return m_Widgets.end();
}

//�����������ʼ
UiWidgetList::reverse_iterator CUiWidgetContainer::Begin_Reverse() {
    return m_Widgets.rbegin();
}

//�������������
UiWidgetList::reverse_iterator CUiWidgetContainer::End_Reverse() {
    return m_Widgets.rend();
}

//������ʽ�������
RtwWidget* CUiWidgetContainer::operator[](int index) {
    if (index < 0 || index >= (int)m_Widgets.size())
        return NULL;

    UiWidgetList::iterator iter = m_Widgets.begin();
    for (int i = 0; i < index; ++i) {
        iter++;
    }
    return *iter;
}

//��ָ��id��������ڶ���
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

//��ָ��id��������ڵײ�
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

//����
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

//��Ⱦ
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

//��ӡ����ṹ
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

//��ȡ
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
