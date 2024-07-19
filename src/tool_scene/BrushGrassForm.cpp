// BrushGrassForm.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "BrushGrassForm.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "tool_scene.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "BrushMapView.h"


// CBrushGrassForm dialog

IMPLEMENT_DYNAMIC(CBrushGrassForm, CDialog)

CBrushGrassForm::CBrushGrassForm(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushGrassForm::IDD, pParent)
{
    m_grassDensity = 1;
    m_grassHeightMax = 10;
    m_grassHeightMin = 10;
    m_grassWidthMin = 10;
    m_grassWidthMax = 10;
}

CBrushGrassForm::~CBrushGrassForm()
{
}

void CBrushGrassForm::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TEXT_WND, m_TextWnd);
    DDX_Control(pDX, IDC_COMBO1, m_ComboxText);
    DDX_Text(pDX, IDC_EDIT, m_grassDensity);
    DDX_Text(pDX, IDC_EDIT2, m_grassHeightMin);
    DDX_Text(pDX, IDC_EDIT4, m_grassHeightMax);
    DDX_Text(pDX, IDC_EDIT3, m_grassWidthMin);
    DDX_Text(pDX, IDC_EDIT5, m_grassWidthMax);
    DDX_Control(pDX, IDC_SLIDER1, m_BrushSize);
}


BEGIN_MESSAGE_MAP(CBrushGrassForm, CDialog)
    ON_BN_CLICKED(IDC_CHECK1, &CBrushGrassForm::OnBnClickedCheck1)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CBrushGrassForm::OnCbnSelchangeCombo1)
    ON_WM_HSCROLL()
    ON_EN_KILLFOCUS(IDC_EDIT, &CBrushGrassForm::OnEnKillfocusEdit)
    ON_EN_KILLFOCUS(IDC_EDIT2, &CBrushGrassForm::OnEnKillfocusEdit)
    ON_EN_KILLFOCUS(IDC_EDIT4, &CBrushGrassForm::OnEnKillfocusEdit)
    ON_EN_CHANGE(IDC_EDIT2, &CBrushGrassForm::OnEnChangeEdit2)
    ON_EN_CHANGE(IDC_EDIT4, &CBrushGrassForm::OnEnChangeEdit4)
END_MESSAGE_MAP()


// CBrushGrassForm message handlers

void CBrushGrassForm::OnBnClickedCheck1()
{
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    if (IsDlgButtonChecked(IDC_CHECK1))
    {
        pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_GRASS);
        pFrame->m_BrushTool.SetBrushData((long)(LONG_PTR)this);
    }
    else
        pFrame->m_BrushTool.SetBrushType(CBrushTool::BT_NONE);
}

void CBrushGrassForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    funguard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;
    if (pSliderCtrl==&m_BrushSize)
    {
        pEditor->m_BrushTool.m_Drawer.SetSize2(m_BrushSize.GetPos(), m_BrushSize.GetPos());
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
    fununguard;
}

BOOL CBrushGrassForm::OnInitDialog()
{
    CDialog::OnInitDialog();

    for (size_t i = 0; i < RtsGrassRender::Max_Grass_Text_Id; ++i)
    {
        RtsGrassRender::RtgGrassText* t = g_pScene->m_GrassRender.m_apText[i];

        if (t)
        {
            m_ComboxText.SetItemData(m_ComboxText.AddString(t->name.c_str()), i);
        }
    }

    if (m_ComboxText.GetCount() > 0)
    {
        m_ComboxText.SetCurSel(0);
        OnCbnSelchangeCombo1();
    }

    m_BrushSize.SetRange(1, 10);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CBrushGrassForm::OnCbnSelchangeCombo1()
{
    int sel = m_ComboxText.GetCurSel();

    if (sel == CB_ERR)
        return;

    RtsGrassRender::RtgGrassText* t = g_pScene->m_GrassRender.m_apText[(int)m_ComboxText.GetItemData(sel)];
    m_TextWnd.SetText(t->file.c_str(), t->sub_x, t->sub_y);


}

void CBrushGrassForm::OnEnKillfocusEdit()
{
    UpdateData();
}

void CBrushGrassForm::OnEnChangeEdit2()
{
    UpdateData();

    if (m_grassHeightMax < m_grassHeightMin)
        m_grassHeightMax = m_grassHeightMin;

    m_grassWidthMax = m_TextWnd.GetAspectW2H() * m_grassHeightMax;
    m_grassWidthMin = m_TextWnd.GetAspectW2H() * m_grassHeightMin;

    UpdateData(FALSE);
}

void CBrushGrassForm::OnEnChangeEdit4()
{
    UpdateData();

    if (m_grassHeightMin > m_grassHeightMax)
        m_grassHeightMin = m_grassHeightMax;

    m_grassWidthMax = m_TextWnd.GetAspectW2H() * m_grassHeightMax;
    m_grassWidthMin = m_TextWnd.GetAspectW2H() * m_grassHeightMin;

    UpdateData(FALSE);
}
