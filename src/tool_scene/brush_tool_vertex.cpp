#include <Windows.h>
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_vertex.h"

/*----------------------------------------------------------------------
    CBrusherVertex
----------------------------------------------------------------------*/

CBrusherVertex::CBrusherVertex(CBrushTool* pBrushTool) : CBrusher(pBrushTool)
{
    //guard;
    m_iBrushType = 0;
    m_iDragX = -1;
    m_iDragY = -1;
    m_eAdsorbType = AT_TILE;
    m_bEditTile = false;
    m_bEditTerrain = false;
    //unguard;
}

CBrusherVertex::~CBrusherVertex()
{
    //guard;
    //unguard;
}

void CBrusherVertex::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    //guard;
    switch (lParam1)
    {
    case 1:
        m_iBrushType = (int)lParam2;
        OnActive();
        break;
    case 2: // Edit Tile
        m_bEditTile = (lParam2!=0);
        break;
    case 3: // Edit Terrain
        m_bEditTerrain = (lParam2!=0);
        break;
    }
   // unguard;
}

void CBrusherVertex::OnActive()
{
   // guard;
    if (m_iBrushType==4)
    {
        m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_ROUND, TRUE);
    }else if (m_iBrushType==0)
    {
        m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE, TRUE);
    }else
    {
        m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_RECTANGLE);
    }
    //unguard;
}

BOOL CBrusherVertex::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    //guard;
    if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能开始刷
    {
        if (m_bEditTerrain==false && m_bEditTile==false) return FALSE;
        m_bBrushBegin = 1;
        m_pBrushTool->m_Drawer.SetShow(true);
        switch (m_iBrushType)
        {
        case 4: // 拉高
        case 0: // 拉高
            m_pBrushTool->m_Drawer.SetShow(false);
            break;
        case 1: // 拉平
            break;
        case 2: // 起伏
            break;
        case 3: // 抚平
            break;
        }
        return TRUE;
    }
    return FALSE;
    //unguard;
}

BOOL CBrusherVertex::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    int iTileX, iTileY;
    m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iTileX, iTileY, m_bEditTile?true:false);
    if (m_bEditTile==false)
    {
        iTileX /= 2;
        iTileY /= 2;
    }
    switch (m_iBrushType)
    {
    case 4: // 拉高
    case 0: // 拉高
        return OnBrushVertexHeight(iKey, iMouseX, iMouseY, iTileX, iTileY);
    case 1: // 拉平
        return OnBrushVertexDrag(iKey, iMouseX, iMouseY, iTileX, iTileY);
    case 2: // 起伏
        return OnBrushVertexNoise(iKey, iMouseX, iMouseY, iTileX, iTileY);
    case 3: // 抚平
        return OnBrushVertexSmooth(iKey, iMouseX, iMouseY, iTileX, iTileY);
    }
    CHECKEX("使用了未知的顶点刷子");
    return FALSE;
    //unguard;
}

void CBrusherVertex::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    m_pBrushTool->m_Drawer.SetShow(true);
    switch (m_iBrushType)
    {
    case 4: // 拉高
    case 0: // 拉高
        break;
    case 1: // 拉平
        break;
    case 2: // 起伏
        break;
    case 3: // 抚平
        break;
    }
    //unguard;
}

// 计算范围内的法向量 (m_iMinX, m_iMinY, m_iMaxX, m_iMaxY)
void CBrusherVertex::ReComputeNormal()
{
   // guard;
    int x, y;
    for (y=m_iMinY; y<=m_iMaxY; y++)
    {
        for (x=m_iMinX; x<=m_iMaxX; x++)
        {
            g_pScene->ComputeVertexNormal(x, y);
            //g_pScene->ComputeVertexColor(x, y, pVertex);
            g_pScene->ComputeVertexColor(x, y);
        }
    }
   // unguard;
}

BYTE CBrusherVertex::GetHeight(int iPosX, int iPosY)
{
    return g_pScene->GetVertexHeightLevel(iPosX, iPosY, m_bEditTile);
}

void CBrusherVertex::SetHeight(int iPosX, int iPosY, BYTE cHeight)
{
    g_pScene->SetVertexHeightLevel(iPosX, iPosY, m_bEditTerrain, m_bEditTile, cHeight);
}

// 拉高(低)
BOOL CBrusherVertex::OnBrushVertexHeight(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY)
{
    //guard;
    int x, y, iSize, iOffset;
    float c, *ph;
    iSize = m_pBrushTool->m_Drawer.m_iSizeMax;

    if (m_bBrushBegin==1)
    {
        m_bBrushBegin = 0;
        m_iDragX = iTileX;
        m_iDragY = iTileY;
        m_iDragMouseY = iMouseY;
        m_cLastHeight = GetHeight(iTileX, iTileY);
        m_iLastMouseY = m_iDragMouseY;

        for (y=-iSize; y<=iSize; y++)
        {
            for (x=-iSize; x<=iSize; x++)
            {
                m_pBrushTool->m_Drawer.m_fBuffer[y+CDrawerBrush::DB_MAX_SIZE][x+CDrawerBrush::DB_MAX_SIZE] = GetHeight(m_iDragX+x, m_iDragY+y);;
            }
        }
    }

    if (iKey&MK_CONTROL)
    {
        iOffset = -1;
    }else
    {
        iOffset = 1;
    }
    iOffset = m_iLastMouseY-iMouseY;
    m_iLastMouseY = iMouseY;

    for (y=-iSize; y<=iSize; y++)
    {
        for (x=-iSize; x<=iSize; x++)
        {
            c = m_pBrushTool->m_Drawer.m_fMask[y+CDrawerBrush::DB_MAX_SIZE][x+CDrawerBrush::DB_MAX_SIZE];
            if (c==0.f) continue; // 没有刷子
            ph = &(m_pBrushTool->m_Drawer.m_fBuffer[y+CDrawerBrush::DB_MAX_SIZE][x+CDrawerBrush::DB_MAX_SIZE]);
            *ph += (float)iOffset * c;
            if (*ph<0) *ph = 0;
            else if (*ph>255) *ph = 255;
            SetHeight(m_iDragX+x, m_iDragY+y, (BYTE)(*ph));
        }
    }

    // 设置初始值
    m_iMinX = m_iDragX-iSize;
    m_iMinY = m_iDragY-iSize;
    m_iMaxX = m_iDragX+iSize+1;
    m_iMaxY = m_iDragY+iSize+1;

    m_cLastHeight = GetHeight(m_iDragX, m_iDragY);

    // 从高度图更新数据
    if (m_bEditTile)    g_pScene->UpdateVertexPositionByHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);
    if (m_bEditTerrain) g_pScene->UpdateVertexPositionByTerrainHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // 重新计算这个区域的AABB
    g_pScene->UpdateGrid(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // 根据范围重新计算法向量
    ReComputeNormal();
    return TRUE;
    //unguard;
}

// 拉平
BOOL CBrusherVertex::OnBrushVertexDrag(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY)
{
    //guard;
    static int sTileX=-1, sTileY=-1;
    if (sTileX==iTileX && sTileY==iTileY) return TRUE;
    sTileX = iTileX;
    sTileY = iTileY;

    int x, y, iSize, px, py, iHeight;
    float c;
    iSize = m_pBrushTool->m_Drawer.m_iSizeMax;

    if (m_bBrushBegin==1)
    {
        m_bBrushBegin = 0;
        m_cLastHeight = GetHeight(iTileX, iTileY);
    }

    for (y=-iSize; y<=iSize; y++)
    {
        for (x=-iSize; x<iSize; x++)
        {
            c = m_pBrushTool->m_Drawer.m_fMask[y+CDrawerBrush::DB_MAX_SIZE][x+CDrawerBrush::DB_MAX_SIZE];
            if (c==0.f) continue; // 没有刷子
            px = iTileX + x;
            py = iTileY + y;
            iHeight = GetHeight(px, py);
            iHeight = m_cLastHeight*c + iHeight*(1.f-c);
            if (iHeight<0) iHeight = 0;
            else if (iHeight>255) iHeight = 255;
            SetHeight(px, py, iHeight);
        }
    }

    // 设置初始值
    m_iMinX = iTileX-iSize;
    m_iMinY = iTileY-iSize;
    m_iMaxX = iTileX+iSize+1;
    m_iMaxY = iTileY+iSize+1;

    // 从高度图更新数据
    if (m_bEditTile)    g_pScene->UpdateVertexPositionByHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);
    if (m_bEditTerrain) g_pScene->UpdateVertexPositionByTerrainHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // 重新计算这个区域的AABB
    g_pScene->UpdateGrid(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // 根据范围重新计算法向量
    ReComputeNormal();
    return TRUE;
    //unguard;
}

// 起伏
BOOL CBrusherVertex::OnBrushVertexNoise(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY)
{
    return TRUE;
}

// 抚平
BOOL CBrusherVertex::OnBrushVertexSmooth(int iKey, int iMouseX, int iMouseY, int iTileX, int iTileY)
{
    //guard;
    int x, y, iSize, px, py, iHeight, iVHeight;
    float c;
    iSize = m_pBrushTool->m_Drawer.m_iSizeMax;

    for (y=-iSize; y<=iSize; y++)
    {
        for (x=-iSize; x<iSize; x++)
        {
            c = m_pBrushTool->m_Drawer.m_fMask[y+CDrawerBrush::DB_MAX_SIZE][x+CDrawerBrush::DB_MAX_SIZE];
            if (c==0.f) continue; // 没有刷子
            px = iTileX + x;
            py = iTileY + y;
            iVHeight = (GetHeight(px-1, py) + GetHeight(px+1, py) + GetHeight(px, py-1) + GetHeight(px, py+1))/4;
            iHeight = GetHeight(px, py);
            iHeight = iVHeight*c + iHeight*(1.f-c);
            if (iHeight<0) iHeight = 0;
            else if (iHeight>255) iHeight = 255;
            SetHeight(px, py, iHeight);
        }
    }

    // 设置初始值
    m_iMinX = iTileX-iSize;
    m_iMinY = iTileY-iSize;
    m_iMaxX = iTileX+iSize+1;
    m_iMaxY = iTileY+iSize+1;

    // 从高度图更新数据
    if (m_bEditTile)    g_pScene->UpdateVertexPositionByHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);
    if (m_bEditTerrain) g_pScene->UpdateVertexPositionByTerrainHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // 重新计算这个区域的AABB
    g_pScene->UpdateGrid(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY);

    // 根据范围重新计算法向量
    ReComputeNormal();
    return TRUE;
   // unguard;
}

void CBrusherVertex::OnDrawText(RtgDevice* pDevice)
{
    char szOutputString[] = "Left Drag, ESC";
    pDevice->DrawString(
        (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
        pDevice->m_iWndHeight*3/4,
        0xFFFF0000,
        szOutputString
        );
}

bool CBrusherVertex::IsEditTerrain()
{
    return (!m_bEditTile);
}

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
