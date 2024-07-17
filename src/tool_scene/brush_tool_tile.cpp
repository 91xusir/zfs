#include <windows.h>
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_tile.h"

/*----------------------------------------------------------------------
    CBrusherTile
----------------------------------------------------------------------*/

CBrusherTile::CBrusherTile(CBrushTool* pBrushTool) : CBrusher(pBrushTool)
{
    m_eAdsorbType = AT_TILE;
}

CBrusherTile::~CBrusherTile()
{
}

void CBrusherTile::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    //guard;
    m_iFillTerrain = (int)lParam1;
    //unguard;
}

void CBrusherTile::OnActive()
{
    //guard;
    m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE, FALSE);
    //unguard;
}

BOOL CBrusherTile::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    //guard;
    if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能开始刷
        return TRUE;
    return FALSE;
    //unguard;
}

int cbTileTouch(int iLevel, int x, int y, float fDist, long p1, long p2)
{
   // guard;
    ((CBrusherTile*)p1)->OnBrushTouch(p2, iLevel, fDist, x, y);
    return 0;
    //unguard;
}

BOOL CBrusherTile::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    int iOutX, iOutY;
    this->m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iOutX, iOutY, true);
    m_pBrushTool->CallbackTouch(cbTileTouch, (long)this, iKey);
    return TRUE;
    //unguard;
}

void CBrusherTile::OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY)
{
    //guard;
    g_pScene->SetTileMapMtl(iTileX  , iTileY  , m_iFillTerrain, 0x0F); // center
    g_pScene->SetTileMapMtl(iTileX-1, iTileY-1, m_iFillTerrain, 0x08); // left top
    g_pScene->SetTileMapMtl(iTileX  , iTileY-1, m_iFillTerrain, 0x0C); // top
    g_pScene->SetTileMapMtl(iTileX+1, iTileY-1, m_iFillTerrain, 0x04); // right top
    g_pScene->SetTileMapMtl(iTileX-1, iTileY+1, m_iFillTerrain, 0x02); // left bottom
    g_pScene->SetTileMapMtl(iTileX  , iTileY+1, m_iFillTerrain, 0x03); // bottom
    g_pScene->SetTileMapMtl(iTileX+1, iTileY+1, m_iFillTerrain, 0x01); // right bottom
    g_pScene->SetTileMapMtl(iTileX-1, iTileY  , m_iFillTerrain, 0x0A); // left
    g_pScene->SetTileMapMtl(iTileX+1, iTileY  , m_iFillTerrain, 0x05); // right
    //unguard;
}

void CBrusherTile::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
}

void CBrusherTile::OnDrawText(RtgDevice* pDevice)
{
    char szOutputString[] = "Left Click, Left Drag, ESC";
    pDevice->DrawString(
        (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
        pDevice->m_iWndHeight*3/4,
        0xFFFF0000,
        szOutputString
        );
}

/*----------------------------------------------------------------------
    CBrusherAttribute
----------------------------------------------------------------------*/

CBrusherAttribute::CBrusherAttribute(CBrushTool* pBrushTool, BYTE cAttrMask) : CBrusher(pBrushTool)
{
    //guard;
    m_cAttrMask = cAttrMask;
    m_eAdsorbType = AT_TERRAIN;
    //unguard;
}

CBrusherAttribute::~CBrusherAttribute()
{
}

bool CBrusherAttribute::IsEditTerrain()
{
    return true;
}

void CBrusherAttribute::OnActive()
{
   // guard;
    m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE, FALSE);
   // unguard;
}

BOOL CBrusherAttribute::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
   // guard;
    if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能开始刷
    {
        OnBrushMove(iKey, iMouseX, iMouseY);
        return TRUE;
    }
    return FALSE;
   // unguard;
}

int cbAttributeTouch(int x, int y, int iIdx, float fDist, long p1, long p2)
{
    //guard;
    ((CBrusherAttribute*)p1)->OnBrushTouch(p2, iIdx, fDist, x, y);
    return 0;
   // unguard;
}

BOOL CBrusherAttribute::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
   // guard;
    int iOutX, iOutY;
    this->m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iOutX, iOutY, false);
    m_pBrushTool->CallbackSmallTouch(cbAttributeTouch, (long)this, iKey);
    return TRUE;
    //unguard;
}

void CBrusherAttribute::OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY)
{
   // guard;
    if (!(iKey&MK_CONTROL))
    {
        g_pScene->SetTerrainAttr(iTileX*g_iSceneTileRate+iLevel%2, iTileY*g_iSceneTileRate+iLevel/2, m_cAttrMask);
    }else
    {
        g_pScene->RemoveTerrainAttr(iTileX*g_iSceneTileRate+iLevel%2, iTileY*g_iSceneTileRate+iLevel/2, m_cAttrMask);
    }
   // unguard;
}

void CBrusherAttribute::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
}

void CBrusherAttribute::OnDrawText(RtgDevice* pDevice)
{
    char szOutputString[] = "Left Click, Left Drag, ESC";
    pDevice->DrawString(
        (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
        pDevice->m_iWndHeight*3/4,
        0xFFFF0000,
        szOutputString
        );
}

void CBrusherAttribute::OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    iGX = (iGX+g_pScene->GetCenterX()*g_iSceneTerrainCntX)/2-g_pScene->GetCenterX()*g_iSceneTileCntX;
    iGY = (iGY+g_pScene->GetCenterY()*g_iSceneTerrainCntY)/2-g_pScene->GetCenterY()*g_iSceneTileCntY;
    DrawLineBrush(pDevice, vScene, vPos, iGX, iGY);
}

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
