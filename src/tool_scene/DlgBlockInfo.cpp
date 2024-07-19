// DlgBlockInfo.cpp : implementation file
//

#include "stdafx.h"
#include "tool_scene.h"
#include "DlgBlockInfo.h"
#include "scene/rt_scene.h"
#include ".\dlgblockinfo.h"
#include "GridItems.h"

//////////////////////////////////////////////////////////////////////////
// CDlgBlockInfo dialog

IMPLEMENT_DYNAMIC(CDlgBlockInfo, CDialog)
CDlgBlockInfo::CDlgBlockInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBlockInfo::IDD, pParent)
{
    m_pBlockMap = NULL;
}

CDlgBlockInfo::~CDlgBlockInfo()
{
}

void CDlgBlockInfo::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgBlockInfo, CDialog)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


// CDlgBlockInfo message handlers

int CDlgBlockInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;
    m_wndGrid.Create(CRect(0, 0, 0, 0), this, 0);
    m_wndGrid.SetTheme(xtpGridThemeCool);
    m_wndGrid.SetOwner(this);
    CXTPPropertyGridItem* pRoot;

    CXTPPropertyGridItem *pItem;
    pRoot = m_wndGrid.AddCategory("基本属性");
    pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BLOCK_FILENAME, m_pBlockMap->m_szFileName, 20)); pItem->SetReadOnly(TRUE);
    pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BLOCK_NAME, m_pBlockMap->m_szBlockName, 20));
	pItem = pRoot->AddChildItem(new CGridItemInt(ID_GRIDITEM_BLOCK_REGION_ID,&(m_pBlockMap->m_region_id)));
    pRoot->Expand();

    pRoot = m_wndGrid.AddCategory("环境");
    pRoot->SetDescription(_T("这里可以设置场景的灯光和雾等环境相关的属性"));
    pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_ENVIRONMENT, &(m_pBlockMap->m_dwEnvironmentLighting)));
    pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_ENV_SUN_COLOR, &(m_pBlockMap->m_dwSunLighting)));

    pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_FOG_COLOR, &m_pBlockMap->m_dwFogColor));
    pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_FOG_MIN, &(m_pBlockMap->m_fFogNear)));
    pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_FOG_MAX, &(m_pBlockMap->m_fFogFar)));
    pRoot->Expand();

    pRoot = m_wndGrid.AddCategory("背景音乐");
    pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BACKMUSIC, m_pBlockMap->m_szBackMusic, 20));
    pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_BACKMUSIC_START_TIME, &(m_pBlockMap->m_fBackMusicStartTime)));
    pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_BACKMUSIC_DIFF_TIME, &(m_pBlockMap->m_fBackMusicDifferenceTime)));
    pRoot->Expand();

    pRoot = m_wndGrid.AddCategory("其它");
    pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BLOCK_FLAG_TEXTURE, m_pBlockMap->m_szBlockFlagTexture, 20));
    pRoot->Expand();

    return 0;
}

void CDlgBlockInfo::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    m_wndGrid.MoveWindow(0, 0, cx, cy-50);
    m_wndGrid.Invalidate(FALSE);
}

void CDlgBlockInfo::Init(RtsSceneBlockMap* pBlockMap)
{
    CHECK(pBlockMap);
    m_pBlockMap = pBlockMap;
}

LRESULT CDlgBlockInfo::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CDlgBlockInfo::OnOK()
{
    CDialog::OnOK();
}
