#include "studio.h"


UiForm_WidgetHint::UiForm_WidgetHint()
{/*LOG("");*/
    m_pFrm_This = NULL;
    m_pLab_Type = NULL;
    m_pLab_ID = NULL;
    m_pLab_Name = NULL;
    m_pLab_RefCount = NULL;
}

UiForm_WidgetHint::~UiForm_WidgetHint()
{/*LOG("");*/
    DROP_RTUI_OBJECT(m_pFrm_This);
    DROP_RTUI_OBJECT(m_pLab_Type);
    DROP_RTUI_OBJECT(m_pLab_ID);
    DROP_RTUI_OBJECT(m_pLab_Name);
    DROP_RTUI_OBJECT(m_pLab_RefCount);
}

bool UiForm_WidgetHint::Init()
{/*LOG("");*/
    CHECK(g_workspace.FindWidget("frmWidgetHint", (RtwWidget**)&m_pFrm_This));
    CHECK(g_workspace.FindWidget("frmWidgetHint.labType", (RtwWidget**)&m_pLab_Type));
    CHECK(g_workspace.FindWidget("frmWidgetHint.labID", (RtwWidget**)&m_pLab_ID));
    CHECK(g_workspace.FindWidget("frmWidgetHint.labName", (RtwWidget**)&m_pLab_Name));
    CHECK(g_workspace.FindWidget("frmWidgetHint.labRefCount", (RtwWidget**)&m_pLab_RefCount));

    RtwWidget::EvClass_Hint += RTW_CALLBACK(this, UiForm_WidgetHint, OnClassHint_WidgetHint);

    return true;
}

void UiForm_WidgetHint::Refresh()
{/*LOG("");*/
}

void UiForm_WidgetHint::Show()
{/*LOG("");*/
    m_pFrm_This->Show();
}

void UiForm_WidgetHint::Hide()
{/*LOG("");*/
    m_pFrm_This->Hide();
}

void UiForm_WidgetHint::Run(DWORD dwDelta)
{/*LOG("");*/
}

bool UiForm_WidgetHint::IsVisible()
{/*LOG("");*/
    return m_pFrm_This->getVisible();
}

void UiForm_WidgetHint::OnClassHint_WidgetHint(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{/*LOG("");*/
    m_pLab_Type->SetText(g_WidgetTypeName[pWidget->getWidgetType()]);
    m_pLab_Name->SetText(pWidget->getName());
    m_pLab_ID->SetText(rtFormatNumber(pWidget->getId()));
    m_pLab_RefCount->SetText(rtFormatNumber(pWidget->getRefCount()));
}
