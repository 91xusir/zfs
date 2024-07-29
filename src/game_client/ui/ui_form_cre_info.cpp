#include "gc_include.h"
#include "ui_form_cre_info.h"

UIFormCreInfo::UIFormCreInfo()
{
	guard;

	m_pFrmThis			= LOAD_UI("fmhpcreature");
	m_pFace				= LOAD_UI("fmhpcreature.panbossface");
	m_pName				= LOAD_UI("fmhpcreature.panhpleft.lbbossname");
	m_pLevel			= LOAD_UI("fmhpcreature.lblvboss");
	m_pHPs				= LOAD_UI_T(RtwProgressBar, "fmhpcreature.barhp1");

	m_pFrmThis->Hide();

	m_iLevel = 0;
	m_nActorID = 0;
	m_nHpTotal = 0;
	m_nHpRemain = 0;
	unguard;
}

UIFormCreInfo::~UIFormCreInfo()
{
	guard;
	unguard;
}

void UIFormCreInfo::OnPreDraw()
{
	guard;
	m_pName->SetText(m_strName);
	RtwImage* pImageFace = g_workspace.getImageFactory()->createImage(m_strFaceFile.c_str());
	if (!pImageFace)
	{
		return;
	}
	char level[10];
	rt2_sprintf(level,"%d",m_iLevel);
	m_pLevel->SetText(level);
	pImageFace->SetBlend(true);
	m_pFace->SetBackgroundImage(pImageFace);
	m_pHPs->Show();
	m_pHPs = LOAD_UI_T(RtwProgressBar, "fmhpcreature.barhp1");
	m_pHPs->SetValue((float)m_nHpRemain/m_nHpTotal);
	unguard;
}

void UIFormCreInfo::Show()
{
	guard;
	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormCreInfo::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormCreInfo::Hide()
{
	guard;
	if (m_pFrmThis)
		m_pFrmThis->Hide();
	m_nActorID = 0;
	unguard;
}

void UIFormCreInfo::Refresh()
{
	guard;
	if ( IsVisible() )		Show();
	unguard;
}

void UIFormCreInfo::SetMonTextGolden()
{
	guard;
	m_pName->SetTextColor(RtwPixel(0xFFFFFF00));
	unguard;
}

void UIFormCreInfo::SetMonTextGray()
{
	guard;
	m_pName->SetTextColor(RtwPixel(0xFF888888));
	unguard;
}

UIFormMonInfo::UIFormMonInfo()
{
	guard;

	m_pFrmThis			= LOAD_UI("fmmonhp");
	m_pFace				= LOAD_UI("fmmonhp.monface");
	m_pName				= LOAD_UI("fmmonhp.monpk.lbname");
	m_pLevel			= LOAD_UI("fmmonhp.lbmonlv");
	m_pHPs				= LOAD_UI_T(RtwProgressBar, "fmmonhp.monhp");
	m_pFace->ModifyFlag(wfClick, 0);
	m_pFrmThis->EvRClick	+= RTW_CALLBACK(this,GcWorld,OnMouseRightDown);
	m_pFrmThis->Hide();

	m_iLevel = 0;
	m_nActorID = 0;
	m_nHpTotal = 0;
	m_nHpRemain = 0;

	unguard;
}

UIFormMonInfo::~UIFormMonInfo()
{
	guard;
	unguard;
}

void UIFormMonInfo::OnPreDraw()
{
	guard;
	if (m_nHpRemain==m_nHpTotal)
			m_pHPs->SetValue(1.0f);
	else
		m_pHPs->SetValue((float)m_nHpRemain/100);
	m_pName->SetText(m_strName);

	RtwImage* pImageFace = g_workspace.getImageFactory()->createImage(m_strFaceFile.c_str());
	if (!pImageFace)
	{
		return;
	}
	char level[10];
	rt2_sprintf(level,"%d",m_iLevel);
	m_pLevel->SetText(level);
	pImageFace->SetBlend(true);
	m_pFace->SetBackgroundImage(pImageFace);
	unguard;
}

void UIFormMonInfo::Show()
{
	guard;
	OnPreDraw();
	m_pFrmThis->Show();
	unguard;
}

bool UIFormMonInfo::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormMonInfo::Hide()
{
	guard;
	if (m_pFrmThis)
		m_pFrmThis->Hide();
	m_nActorID = 0;
	unguard;
}

void UIFormMonInfo::Refresh()
{
	guard;
	if ( IsVisible() )		Show();
	unguard;
}

void UIFormMonInfo::SetMonTextGray()
{
	guard;
	m_pName->SetTextColor(RtwPixel(0xFF888888));
	unguard;
}

void UIFormMonInfo::SetMonTextWhite()
{
	guard;
	m_pName->SetTextColor(RtwPixel(0xFFFFFFFF));
	unguard;
}

CUI_Form_Chant::CUI_Form_Chant()
{
	guard;
	m_pFrmThis = LOAD_UI("ChantForm"/*"fmprocess"*/);
	m_pName = LOAD_UI("ChantForm.SkillName");
	m_pChantBar = LOAD_UI_T(RtwProgressBar, "ChantForm.ChantBar"/*"fmprocess.EXPBar"*/);

	m_pFrmThis->Hide();
	unguard;
};

CUI_Form_Chant::~CUI_Form_Chant()
{
	guard;

	unguard;
}

void CUI_Form_Chant::Show()
{
	guard;
	m_pFrmThis->Show();
	unguard;
}

void CUI_Form_Chant::Hide()
{
	guard;
	m_pFrmThis->Hide();
	unguard;
}

bool CUI_Form_Chant::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void CUI_Form_Chant::Refresh(float fPercent)
{
	guard;
	m_pChantBar->SetValue(fPercent);
	m_pFrmThis->Show();
	unguard;
}

void CUI_Form_Chant::SetSkillName(const char* strName)
{
	guard;
	m_pName->SetText(strName);
	unguard;
}

CUIForm_Collect::CUIForm_Collect()
{
	guard;

	m_pFrmThis = LOAD_UI("ChantForm1");;
	m_pCollectBar = LOAD_UI_T(RtwProgressBar, "ChantForm1.ChantBar");
	m_pFrmThis->Hide();
	m_pCollectBar->SetValue(0.f);

	m_bStart = false;
	m_lCollectTime = 0;
	m_lFinishTime = 0;
	m_value = 0.f;

	unguard;
}

CUIForm_Collect::~CUIForm_Collect()
{

}

void CUIForm_Collect::Show()
{
	m_pFrmThis->Show();
}

void CUIForm_Collect::Hide()
{
	m_pFrmThis->Hide();
}

bool CUIForm_Collect::IsVisible()
{
	return m_pFrmThis->IsVisible();
}

void CUIForm_Collect::SetCollectTime(long millTime)
{
	m_lCollectTime = millTime;
	m_lFinishTime = rtGetMilliseconds() + m_lCollectTime;
	m_bStart = true;
}

void CUIForm_Collect::OnRun()
{
	if ( m_bStart )
	{
		int surplus = m_lFinishTime - rtGetMilliseconds();
		m_value = float(m_lCollectTime - surplus) / (float)m_lCollectTime;
		m_pCollectBar->SetValue(m_value);
		m_pFrmThis->Show();
		if ( m_value >= 1.f )
		{
			m_bStart = false;
		}
	}
}

void CUIForm_Collect::Revert()
{
	m_pCollectBar->SetValue(0.f);
	m_bStart = false;
}

bool CUIForm_Collect::IsFull()
{
	return m_value >= 1.f;
}