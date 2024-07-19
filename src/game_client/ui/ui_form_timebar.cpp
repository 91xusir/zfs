#include "gc_include.h"
#include "ui_form_timebar.h"
#include "ui_form_playeritems.h"


UIFormTimeBar::UIFormTimeBar()
{
	guard;

	m_pFrmThis = RTW_WIDGET("fmprocess");
	m_pProgressBar = RTW_WIDGET_T(RtwProgressBar, "fmprocess.EXPBar");

	m_pFrmThis->Hide();

	unguard;
}

UIFormTimeBar::~UIFormTimeBar()
{
	guard;

	unguard;
}

void UIFormTimeBar::ClearData()
{
	m_pProgressBar->SetValue(0.0f);
	m_UseType = (ETimeBarUseType)0;
	m_BeginTime = 0;
	m_ProgressTime = 0;
}

void UIFormTimeBar::OnPreDraw()
{
	guard;

	unguard;
}

void UIFormTimeBar::Begin(ETimeBarUseType UseType, DWORD TimeProgress)
{
	guard;

	ClearData();

	m_UseType = UseType;
	m_BeginTime = rtGetMilliseconds();
	m_ProgressTime = TimeProgress;

	m_pProgressBar->SetValue(0.0f);
	OnPreDraw();
	m_pFrmThis->Show();

	unguard;
}

void UIFormTimeBar::End(bool bNotify)
{
	guard;
	Hide(bNotify);
	unguard;
}

bool UIFormTimeBar::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	return true;
	unguard;
}

void UIFormTimeBar::Hide(bool bNotify)
{
	guard;

	m_pFrmThis->Hide();
	ClearData();
	if (bNotify)
		EvTimeOver(m_pProgressBar, weNone);

	unguard;
}

void UIFormTimeBar::Refresh()
{
	guard;
	if ( IsVisible() )
	{
		OnPreDraw();
		m_pFrmThis->Show();
	}
	unguard;
}

void UIFormTimeBar::Tick(DWORD Delta)
{
	guard;

	if (!IsVisible() || m_ProgressTime==0)
		return;

	DWORD dwNow = rtGetMilliseconds();
	int tmp = dwNow - m_BeginTime;
	float value = tmp*1.0f/m_ProgressTime;
	m_pProgressBar->SetValue(value);
	if (value >= 1.0f)
	{
		EvTimeOver(m_pProgressBar, weNone);
		EvTimeOver.Reset();
	}else if ( m_UseType == tbut_CallPet && GetPlayer()->m_bInCallPet == false)
	{
		g_layerMain->m_fromPlayerItems->m_pet = g_layerMain->m_fromPlayerItems->m_LastPetItemID;
		End(false);
		EvTimeOver.Reset();
		return;
	}

	unguard;
}
