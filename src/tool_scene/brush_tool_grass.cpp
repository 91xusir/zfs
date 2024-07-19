#include "stdafx.h"
#include "tool_scene.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_grass.h"
#include "BrushGrassForm.h"

CBrusherGrass::CBrusherGrass(CBrushTool* pBrushTool)
    : CBrusher(pBrushTool),
      m_pForm(NULL)
{

}

CBrusherGrass::~CBrusherGrass()
{

}

void CBrusherGrass::OnActive()
{
    m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE, FALSE);
    return CBrusher::OnActive();
}

int cbGrassTouch(int iLevel, int x, int y, float fDist, long p1, long p2)
{
    // guard;
    ((CBrusherGrass*)p1)->OnBrushTouch(p2, iLevel, fDist, x, y);
    return 0;
    //unguard;
}


BOOL CBrusherGrass::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    int iOutX, iOutY;
    this->m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iOutX, iOutY, true);
    m_pBrushTool->CallbackTouch(cbGrassTouch, (long)this, iKey);
    return TRUE;
    //unguard;
}

void CBrusherGrass::OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY)
{
    if (m_pForm)
    {
        RtgVertex2 szh;
        float szw;
        m_pForm->GetGrassSizeInfo(&szh, &szw);
        g_pScene->SetTileMapGrass(iTileX, iTileY, m_pForm->GetDensity(), m_pForm->GetTextId(), szh, szw);
    }
}
