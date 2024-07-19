#include <Windows.h>
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_single_vertex.h"

/*----------------------------------------------------------------------
    CBrusherSingleVertex
----------------------------------------------------------------------*/

CBrusherSingleVertex::CBrusherSingleVertex(CBrushTool* pBrushTool) : CBrusher(pBrushTool)
{
   // guard;
    m_eAdsorbType = AT_VERTEX;
    m_bEditTile = false;
    m_bEditTerrain = false;
    //unguard;
}

CBrusherSingleVertex::~CBrusherSingleVertex()
{
    //guard;
    //unguard;
}

void CBrusherSingleVertex::OnActive()
{
    //guard;
    m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_BOX, FALSE);
    //unguard;
}

BOOL CBrusherSingleVertex::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    //guard;
    if (iMouseBtn==1)   // ֻ�а�����������ʱ����ܿ�ʼˢ
    {
        if (m_bEditTerrain==false && m_bEditTile==false) return FALSE;
        m_bBrushBegin = 1;
        m_pBrushTool->m_Drawer.SetShow(false);
        return TRUE;
    }
    return FALSE;
    //unguard;
}

BOOL CBrusherSingleVertex::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
   // guard;
    int iTileX, iTileY, iHeight, iOffset;
    m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iTileX, iTileY, m_bEditTile?true:false, true);
    if (m_bBrushBegin==1)
    {
        m_bBrushBegin = 0;
        m_iDragX = iTileX;
        m_iDragY = iTileY;
        m_iDragMouseY = iMouseY;
        m_cLastHeight = GetHeight(iTileX, iTileY);
        m_iLastMouseY = m_iDragMouseY;
    }

    iOffset = (m_iLastMouseY-iMouseY)/3;
    if (iOffset==0) return TRUE;
    m_iLastMouseY = iMouseY;

    iHeight = GetHeight(m_iDragX, m_iDragY);
    iHeight += iOffset;
    if (iHeight<0) iHeight = 0;
    else if (iHeight>255) iHeight = 255;
    SetHeight(m_iDragX, m_iDragY, (BYTE)iHeight);
    m_cLastHeight = iHeight;

    // ���ó�ʼֵ
    m_iMinX = m_iDragX - 1;
    m_iMinY = m_iDragY - 1;
    m_iMaxX = m_iDragX + 1;
    m_iMaxY = m_iDragY + 1;

    // �Ӹ߶�ͼ��������
    if (m_bEditTerrain) g_pScene->UpdateVertexPositionByTerrainHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);
    if (m_bEditTile)    g_pScene->UpdateVertexPositionByHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // ���¼�����������AABB
    g_pScene->UpdateGrid(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // ���ݷ�Χ���¼��㷨����
    ReComputeNormal();
    return TRUE;
    //unguard;
}

void CBrusherSingleVertex::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
   // guard;
    m_pBrushTool->m_Drawer.SetShow(true);
   // unguard;
}

// ���㷶Χ�ڵķ����� (m_iMinX, m_iMinY, m_iMaxX, m_iMaxY)
void CBrusherSingleVertex::ReComputeNormal()
{
   // guard;
    if (m_bEditTile)
    {
        int x, y;
        for (y=m_iMinY; y<=m_iMaxY; y++)
        {
            for (x=m_iMinX; x<=m_iMaxX; x++)
            {
                g_pScene->ComputeVertexNormal(x, y);
                g_pScene->ComputeVertexColor(x, y);
            }
        }
    }
    //unguard;
}

void CBrusherSingleVertex::OnDrawText(RtgDevice* pDevice)
{
    char szOutputString[] = "Left Drag, ESC";
    pDevice->DrawString(
        (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
        pDevice->m_iWndHeight*3/4,
        0xFFFF0000,
        szOutputString
        );
}

bool CBrusherSingleVertex::IsEditTerrain()
{
    return (!m_bEditTile);
}

BYTE CBrusherSingleVertex::GetHeight(int iPosX, int iPosY)
{
    return g_pScene->GetVertexHeightLevel(iPosX, iPosY, m_bEditTile);
}

void CBrusherSingleVertex::SetHeight(int iPosX, int iPosY, BYTE cHeight)
{
    g_pScene->SetVertexHeightLevel(iPosX, iPosY, m_bEditTerrain, m_bEditTile, cHeight);
}

void CBrusherSingleVertex::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    //guard;
    switch (lParam1)
    {
    case 2: // Edit Tile
        m_bEditTile = (lParam2!=0);
        break;
    case 3: // Edit Terrain
        m_bEditTerrain = (lParam2!=0);
        break;
    }
    //unguard;
}

/*----------------------------------------------------------------------
CBrusherTileVertex
----------------------------------------------------------------------*/

CBrusherTileVertex::CBrusherTileVertex(CBrushTool* pBrushTool) : CBrusher(pBrushTool)
{
    m_eAdsorbType = AT_TILE;
    m_bEditTile = false;
    m_bEditTerrain = false;
}

CBrusherTileVertex::~CBrusherTileVertex()
{
}

void CBrusherTileVertex::OnActive()
{
    //guard;
    m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE, FALSE);
    //unguard;
}

void CBrusherTileVertex::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    //guard;
    switch (lParam1)
    {
    case 2: // Edit Tile
        m_bEditTile = (lParam2!=0);
        break;
    case 3: // Edit Terrain
        m_bEditTerrain = (lParam2!=0);
        break;
    }
    //unguard;
}

BOOL CBrusherTileVertex::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    funguard;
    if (iMouseBtn==1)   // ֻ�а�����������ʱ����ܿ�ʼˢ
        return TRUE;
    if (m_bEditTerrain==false && m_bEditTile==false) return FALSE;
    m_bBrushBegin = 1;
    return FALSE;
    fununguard;
}

int cbTileVertexTouch(int iLevel, int x, int y, float fDist, long p1, long p2)
{
    funguard;
    ((CBrusherTileVertex*)p1)->OnBrushTouch(p2, iLevel, fDist, x, y);
    return 0;
    fununguard;
}

BOOL CBrusherTileVertex::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    funguard;
    int iOutX, iOutY;
    this->m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iOutX, iOutY, true);
    if (m_bBrushBegin==1)
    {
        m_bBrushBegin = 0;
        m_iDragMouseY = iMouseY;
        m_iLastMouseY = m_iDragMouseY;

        // ���ó�ʼֵ
        m_iMinX = 1e10;
        m_iMinY = 1e10;
        m_iMaxX = -1;
        m_iMaxY = -1;

        m_iDragX = this->m_pBrushTool->m_iLastTileX;
        m_iDragY = this->m_pBrushTool->m_iLastTileY;
    }
    this->m_pBrushTool->m_iLastTileX = m_iDragX;
    this->m_pBrushTool->m_iLastTileY = m_iDragY;
    m_iOffset = (m_iLastMouseY-iMouseY)/3;
    if (m_iOffset==0) return TRUE;
    m_iLastMouseY = iMouseY;
    m_pBrushTool->CallbackTouch(cbTileVertexTouch, (long)this, iKey);

    m_iMinX --;
    m_iMinY --;
    m_iMaxX ++;
    m_iMaxY ++;

    // �Ӹ߶�ͼ��������
    if (m_bEditTerrain) g_pScene->UpdateVertexPositionByTerrainHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);
    if (m_bEditTile)    g_pScene->UpdateVertexPositionByHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // ���¼�����������AABB
    g_pScene->UpdateGrid(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // ���ݷ�Χ���¼��㷨����
    ReComputeNormal();
    return TRUE;
    fununguard;
}

void CBrusherTileVertex::OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY)
{
    //guard;
    BYTE h;
    h = GetHeight(  iTileX,   iTileY); SetHeight(  iTileX,   iTileY, (BYTE)Clamp(h, (BYTE)0, (BYTE)255));
    h = GetHeight(iTileX+1,   iTileY); SetHeight(iTileX+1,   iTileY, (BYTE)Clamp(h, (BYTE)0, (BYTE)255));
    h = GetHeight(  iTileX, iTileY+1); SetHeight(  iTileX, iTileY+1, (BYTE)Clamp(h, (BYTE)0, (BYTE)255));
    h = GetHeight(iTileX+1, iTileY+1); SetHeight(iTileX+1, iTileY+1, (BYTE)Clamp(h, (BYTE)0, (BYTE)255));
    //unguard;
}

void CBrusherTileVertex::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
}

// ���㷶Χ�ڵķ����� (m_iMinX, m_iMinY, m_iMaxX, m_iMaxY)
void CBrusherTileVertex::ReComputeNormal()
{
    //guard;
    if (m_bEditTile)
    {
        int x, y;
        for (y=m_iMinY; y<=m_iMaxY; y++)
        {
            for (x=m_iMinX; x<=m_iMaxX; x++)
            {
                g_pScene->ComputeVertexNormal(x, y);
                g_pScene->ComputeVertexColor(x, y);
            }
        }
    }
    //unguard;
}

BYTE CBrusherTileVertex::GetHeight(int iPosX, int iPosY)
{
    return g_pScene->GetVertexHeightLevel(iPosX, iPosY, m_bEditTile);
}

void CBrusherTileVertex::SetHeight(int iPosX, int iPosY, BYTE cHeight)
{
    g_pScene->SetVertexHeightLevel(iPosX, iPosY, m_bEditTerrain, m_bEditTile, cHeight);
}

void CBrusherTileVertex::OnDrawText(RtgDevice* pDevice)
{
    char szOutputString[] = "Left Drag, ESC";
    pDevice->DrawString(
        (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
        pDevice->m_iWndHeight*3/4,
        0xFFFF0000,
        szOutputString
        );
}


/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
