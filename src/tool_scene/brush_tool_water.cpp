#include <windows.h>
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_water.h"
#include <stack>

const BYTE cSeaLevelHeight = 127;

/*----------------------------------------------------------------------
    CBrusherWater
----------------------------------------------------------------------*/

CBrusherWater::CBrusherWater(CBrushTool* pBrushTool) : CBrusher(pBrushTool)
{
    //guard;
    m_eAdsorbType = AT_VERTEX;
    m_cLastSelectWaterType = 1;
    m_cWaterHeight = cSeaLevelHeight;
    //unguard;
}

CBrusherWater::~CBrusherWater()
{
    //guard;
    //unguard;
}

void CBrusherWater::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    m_cLastSelectWaterType = (BYTE)lParam1;
}

void CBrusherWater::OnActive()
{
    //guard;
    m_pBrushTool->m_Drawer.SetType(CDrawerBrush::DB_BOX, FALSE);
    //unguard;
}

BOOL CBrusherWater::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    //guard;
    int iTileX, iTileY;
    m_pBrushTool->GetTerrainPos(iMouseX, iMouseY, iTileX, iTileY, true);
    if (iMouseBtn==1)   // ֻ�а�����������ʱ����ܿ�ʼˢ
    {
        if (g_pScene->GetVertexValue(iTileX, iTileY, false)->cWaterHMap)
        {
            ClearWater(iTileX, iTileY, g_pScene->GetVertexValue(iTileX, iTileY, false)->cWaterHMap);
            UpdateVertexPositionByWaterHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY, 0);
        }else
        {
            BYTE cHeight = m_cWaterHeight; //g_pScene->GetVertexValue(iTileX, iTileY, false)->cHeightMap;
            OnFillWater(iTileX, iTileY, cHeight, m_cLastSelectWaterType);
            UpdateVertexPositionByWaterHeightmap(m_iMinX, m_iMinY, m_iMaxX, m_iMaxY, cHeight);
        }
        return TRUE;
    }
    return FALSE;
    //unguard;
}

BOOL CBrusherWater::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    return TRUE;
    //unguard;
}

void CBrusherWater::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
}

void CBrusherWater::UpdateVertexPositionByWaterHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY, BYTE cHeight)
{
    int x, y, i;
    RtsSVertexValue *pV, *pV2;
    RtsSSceneWater *pW;
    int a[8][2] =
    {
        {-1, -1},
        {-1,  0},
        { 0, -1},
        {-1,  1},
        { 1, -1},
        { 0,  1},
        { 1,  0},
        { 1,  1}
    };
    if (cHeight>0) // ��������ˮ���������ˮ�Ļ��Ͳ��ұ�Ե
    {
        for (y=iMinY; y<iMaxY; y++)
        {
            for (x=iMinX; x<iMaxX; x++)
            {
                pV = g_pScene->GetVertexValue(x, y, false);
                if (pV && pV->cWaterHMap==255)
                {
                    for (i=0; i<8; i++)
                    {
                        pV2 = g_pScene->GetVertexValue(x+a[i][0], y+a[i][1], true);
                        if (pV2 && pV2->cWaterHMap!=255)
                        {
                            pV2->cWaterHMap = 253;
                            if (i<3)
                            {
                                g_pScene->SetTileWaterType(x+a[i][0], y+a[i][1], m_cLastSelectWaterType);
                            }
                        }
                    }
                }
            }
        }
    }
    for (y=iMinY-1; y<=iMaxY; y++)
    {
        for (x=iMinX-1; x<=iMaxX; x++)
        {
            pV = g_pScene->GetVertexValue(x, y, true);
            if (pV)
            {
                if (pV->cWaterHMap==255 || pV->cWaterHMap==253)
                {
                    pV->cWaterHMap = cHeight;
                    pV->fWaterHeight = pV->cWaterHMap * g_fSceneHeightStep;

                    // ����͸����
                    i = pV->cWaterHMap - pV->cHeightMap;
                    pW = g_pScene->m_Waters + (m_cLastSelectWaterType-1);
                    if (i>pW->iDepth)
                    {
                        pV->cWaterAlphaMap = (BYTE)pW->iAlpha;
                    }else if (i<0)
                    {
                        pV->cWaterAlphaMap = 0x00;
                    }else
                    {
                        pV->cWaterAlphaMap = i*pW->iAlpha/pW->iDepth;
                    }
                }

                if (pV->cWaterHMap==254)
                {
                   // LOG("Error254\n");
                }
            }
        }
    }
}

bool CBrusherWater::ClearLine(int iLine, BYTE cHeight)
{
    //guard;
    int i, j;
    RtsSVertexValue* pV;
    bool bClear = false;

    // ����ȡ�����ĵ�����ߺ��ұ߱�����ֱ��ȡ�����еĵ�
    for (i=m_iMinX; i<=m_iMaxX; i++)
    {
        pV = g_pScene->GetVertexValue(i, iLine, true);
        if (pV && pV->cWaterHMap==254)
        {
            for (j=i-1; j>=m_iAbMinX; j--)
            {
                if (j<m_iMinX) m_iMinX = j;
                pV = g_pScene->GetVertexValue(j, iLine, true);
                if (pV && pV->cWaterHMap==cHeight)
                {
                    bClear = true;
                    pV->cWaterHMap = 255;
                    g_pScene->SetTileWaterType(j, iLine, 0);
                    pV = g_pScene->GetVertexValue(j, iLine-1, true);
                    if (pV && pV->cWaterHMap<254 && pV->cWaterHMap==cHeight) pV->cWaterHMap = 254;
                    pV = g_pScene->GetVertexValue(j, iLine+1, true);
                    if (pV && pV->cWaterHMap<254 && pV->cWaterHMap==cHeight) pV->cWaterHMap = 254;
                }else
                {
                    break;
                }
            }
            for (j=i+1; j<m_iAbMaxX; j++)
            {
                if (j>=m_iMaxX) m_iMaxX = j+1;
                pV = g_pScene->GetVertexValue(j, iLine, true);
                if (pV && pV->cWaterHMap==cHeight)
                {
                    bClear = true;
                    pV->cWaterHMap = 255;
                    g_pScene->SetTileWaterType(j, iLine, 0);
                    pV = g_pScene->GetVertexValue(j, iLine-1, true);
                    if (pV && pV->cWaterHMap<254 && pV->cWaterHMap==cHeight) pV->cWaterHMap = 254;
                    pV = g_pScene->GetVertexValue(j, iLine+1, true);
                    if (pV && pV->cWaterHMap<254 && pV->cWaterHMap==cHeight) pV->cWaterHMap = 254;
                }else
                {
                    break;
                }
            }
            bClear = true;
            g_pScene->GetVertexValue(i, iLine, true)->cWaterHMap = 255;
            g_pScene->SetTileWaterType(i, iLine, 0);
            pV = g_pScene->GetVertexValue(i, iLine-1, true);
            if (pV && pV->cWaterHMap<254 && pV->cWaterHMap==cHeight) pV->cWaterHMap = 254;
            pV = g_pScene->GetVertexValue(i, iLine+1, true);
            if (pV && pV->cWaterHMap<254 && pV->cWaterHMap==cHeight) pV->cWaterHMap = 254;
        }
    }
    if (iLine<m_iMinY) m_iMinY = iLine;
    if (iLine>=m_iMaxY) m_iMaxY = iLine+1;
    return bClear;
    //unguard;
}

void CBrusherWater::ClearWater(int iTileX, int iTileY, BYTE cHeight)
{
    //guard;
    int iCurLine, iTmpLine;
    std::stack<int> stackLine;

    m_iAbMinX = 0 - g_pScene->GetCenterX() * g_iSceneTileCntX;
    m_iAbMinY = 0 - g_pScene->GetCenterY() * g_iSceneTileCntY;
    m_iAbMaxX = g_pScene->GetBlockCntX() * g_iSceneTileCntX + m_iAbMinX;
    m_iAbMaxY = g_pScene->GetBlockCntY() * g_iSceneTileCntY + m_iAbMinY;

    // ����һ��, ����ȡ�����ĵ�����ߺ��ұ߱�����ֱ��ȡ�����еĵ�
    RtsSVertexValue* pV = g_pScene->GetVertexValue(iTileX, iTileY, true);
    if (pV==NULL) return;
    pV->cWaterHMap = 254; // ���÷��ʱ��
    m_iMinX = iTileX;
    m_iMinY = iTileY;
    m_iMaxX = m_iMinX + 1;
    m_iMaxY = m_iMinY + 1;
    ClearLine(iTileY, cHeight);
    stackLine.push(iTileY);
    iCurLine = iTileY;

    // �ж϶�ջ�Ƿ������ݣ����û�о��˳�
    while (stackLine.size())
    {
        // ȡ����ջָ�����У�ͬʱ������ջ
        iCurLine = stackLine.top();
        stackLine.pop();

        // ѭ�������������
        iTmpLine = iCurLine-1;
        while (ClearLine(iTmpLine, cHeight))
        {
            stackLine.push(iTmpLine);
            iTmpLine -= 1;
        }

        // ѭ�������������
        iTmpLine = iCurLine+1;
        while (ClearLine(iTmpLine, cHeight))
        {
            stackLine.push(iTmpLine);
            iTmpLine += 1;
        }
    }
    //unguard;
}

bool CBrusherWater::FillLine(int iLine, BYTE cHeight, BYTE cWaterType)
{
    //guard;
    int i, j;
    bool bFill = false;
    RtsSVertexValue* pV, *pV2;

    // ����ȡ�����ĵ�����ߺ��ұ߱�����ֱ��ȡ�����еĵ�
    for (i=m_iMinX; i<m_iMaxX; i++)
    {
        pV = g_pScene->GetVertexValue(i, iLine, true);
        if (pV && pV->cWaterHMap==254)
        {
            for (j=i-1; j>=m_iAbMinX; j--)
            {
                if (j<m_iMinX) m_iMinX = j;
                pV = g_pScene->GetVertexValue(j, iLine, true);
                if (pV==NULL) break;
                if (pV->cWaterHMap==255) break;
                if (pV->cHeightMap<cHeight)
                {
                    bFill = true;
                    pV->cWaterHMap = 255;
                    g_pScene->SetTileWaterType(j, iLine, cWaterType); // Ӧ��
                    pV2 = g_pScene->GetVertexValue(j, iLine-1, true);
                    if (pV2 && pV2->cWaterHMap<254 && pV2->cHeightMap<cHeight) pV2->cWaterHMap = 254;
                    pV2 = g_pScene->GetVertexValue(j, iLine+1, true);
                    if (pV2 && pV2->cWaterHMap<254 && pV2->cHeightMap<cHeight) pV2->cWaterHMap = 254;
                }else
                {
                    //pV->cWaterHMap = 255;
                    //g_pScene->SetTileWaterType(j, iLine, cWaterType); // �����չ
                    break;
                }
            }
            for (j=i+1; j<m_iAbMaxX; j++)
            {
                if (j>=m_iMaxX) m_iMaxX = j+1;
                pV = g_pScene->GetVertexValue(j, iLine, true);
                if (pV==NULL) break;
                if (pV->cWaterHMap==255) break;
                if (pV->cHeightMap<cHeight)
                {
                    bFill = true;
                    pV->cWaterHMap = 255;
                    g_pScene->SetTileWaterType(j, iLine, cWaterType); // Ӧ��
                    pV2 = g_pScene->GetVertexValue(j, iLine-1, true);
                    if (pV2 && pV2->cWaterHMap<254 && pV2->cHeightMap<cHeight) pV2->cWaterHMap = 254;
                    pV2 = g_pScene->GetVertexValue(j, iLine+1, true);
                    if (pV2 && pV2->cWaterHMap<254 && pV2->cHeightMap<cHeight) pV2->cWaterHMap = 254;
                }else
                {
                    //pV->cWaterHMap = 255;
                    //g_pScene->SetTileWaterType(j, iLine, cWaterType); // �ұ���չ
                    break;
                }
            }
            pV = g_pScene->GetVertexValue(i, iLine, true);
            if (pV->cHeightMap<cHeight)
            {
                bFill = true;
                pV->cWaterHMap = 255;
                g_pScene->SetTileWaterType(i, iLine, cWaterType); // Ӧ��
                pV2 = g_pScene->GetVertexValue(i, iLine-1, true);
                if (pV2 && pV2->cWaterHMap<254 && pV2->cHeightMap<cHeight) pV2->cWaterHMap = 254;
                pV = g_pScene->GetVertexValue(i, iLine+1, true);
                if (pV2 && pV2->cWaterHMap<254 && pV2->cHeightMap<cHeight) pV2->cWaterHMap = 254;
            }else
            {
                //pV->cWaterHMap = 255;
                //g_pScene->SetTileWaterType(i, iLine, cWaterType); // �ϻ�����չ
            }
        }
    }
    if (iLine<m_iMinY) m_iMinY = iLine;
    if (iLine>=m_iMaxY) m_iMaxY = iLine+1;
    return bFill;
    //unguard;
}

void CBrusherWater::OnFillWater(int iTileX, int iTileY, BYTE cHeight, BYTE cWaterType)
{
    //guard;
    int iCurLine, iTmpLine;
    std::stack<int> stackLine;

    m_iAbMinX = 0 - g_pScene->GetCenterX() * g_iSceneTileCntX;
    m_iAbMinY = 0 - g_pScene->GetCenterY() * g_iSceneTileCntY;
    m_iAbMaxX = g_pScene->GetBlockCntX() * g_iSceneTileCntX + m_iAbMinX;
    m_iAbMaxY = g_pScene->GetBlockCntY() * g_iSceneTileCntY + m_iAbMinY;

    // ����һ��, ����ȡ�����ĵ�����ߺ��ұ߱�����ֱ��ȡ�����еĵ�
    RtsSVertexValue* pV = g_pScene->GetVertexValue(iTileX, iTileY, true);
    if (pV==NULL || pV->cHeightMap>=cHeight) return;
    pV->cWaterHMap = 254;
    m_iMinX = iTileX;
    m_iMinY = iTileY;
    m_iMaxX = m_iMinX + 1;
    m_iMaxY = m_iMinY + 1;
    FillLine(iTileY, cHeight, cWaterType);
    stackLine.push(iTileY);
    iCurLine = iTileY;

    // �ж϶�ջ�Ƿ������ݣ����û�о��˳�
    while (stackLine.size())
    {
        // ȡ����ջָ�����У�ͬʱ������ջ
        iTmpLine = iCurLine = stackLine.top();
        stackLine.pop();

        // ѭ�������������
        iTmpLine = iCurLine-1;
        while (FillLine(iTmpLine, cHeight, cWaterType))
        {
            stackLine.push(iTmpLine);
            iTmpLine -= 1;
        }

        // ѭ�������������
        iTmpLine = iCurLine+1;
        while (FillLine(iTmpLine, cHeight, cWaterType))
        {
            stackLine.push(iTmpLine);
            iTmpLine += 1;
        }
    }
    //unguard;
}

void CBrusherWater::OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    DrawLineBrush(pDevice, vScene, vPos, iGX, iGY);

    DWORD dwLineColor = 0xFF00FF00;
    RtgVertex3 vTopPos;
    vTopPos = vPos;
    vTopPos.z = m_cWaterHeight * g_fSceneHeightStep;
    if (vTopPos.z<vPos.z)
    {
        dwLineColor = 0xFFFF0000;
    }
    pDevice->DrawLine(vPos, vTopPos, &dwLineColor);
}

void CBrusherWater::OnDrawText(RtgDevice* pDevice)
{
    char szOutputString[] = "Left Click, +, -, ESC";
    pDevice->DrawString(
        (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
        pDevice->m_iWndHeight*3/4,
        0xFFFF0000,
        szOutputString
        );
}

void CBrusherWater::OnCommand(int iCmdID, long lParam1, long lParam2)
{
    switch (iCmdID)
    {
    case 2: // Move
        switch (lParam1)
        {
        case VK_PRIOR:      // �����ƶ�
        case VK_OEM_PLUS:   // �Ŵ�
            if (m_cWaterHeight>=0 && m_cWaterHeight<cSeaLevelHeight)
            {
                m_cWaterHeight ++;
            }
            break;
        case VK_NEXT:       // �����ƶ�
        case VK_OEM_MINUS:  // ��С
            if (m_cWaterHeight>0 && m_cWaterHeight<=cSeaLevelHeight)
            {
                m_cWaterHeight --;
            }
            break;
        }
        break;
    }
}

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
