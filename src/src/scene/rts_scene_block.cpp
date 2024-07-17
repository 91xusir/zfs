  
#include "scene/rt_scene.h"
#include "rts_scene_xml_loader.h"

RtSceneBlock::RtSceneBlock()
    : m_cLastOpFlag(0),
      m_fOffsetX(0.f),
      m_fOffsetY(0.f),
      m_iGridSize(g_iSceneGridCntX * g_iSceneGridCntY),
      m_texItemSM(0)
      
{
	CM_MEMPROTECTOR(m_szBlockName, 128)
    (*m_szBlockName) = 0;
    memset(m_texMinmap, 0, sizeof(m_texMinmap));
}

RtSceneBlock::~RtSceneBlock()
{
	CM_MEMUNPROTECTOR(m_szBlockName)
    Clear();
}

bool RtSceneBlock::Init()
{
    Clear();
    return true;
}

void RtSceneBlock::Clear()
{
    OnResourceFree();
}

void RtSceneBlock::SaveSM2File()
{
    if (!m_texItemSM)
        return;

    char szPathSM[MAX_PATH];
    rt2_strcpy(szPathSM, "scene\\texture\\sm\\");
    rt2_strcat(szPathSM, m_szBlockName);
    rt2_strcat(szPathSM, ".dds");
    RtGetRender()->GetTextMgr()->Save(m_texItemSM, szPathSM);
}

void RtSceneBlock::GetRenderGrid(gridlist_t* pgrid, RtgCamera& _cam)
{
    RTASSERT(pgrid);

    for (int i = 0; i < m_iGridSize; ++i)
    {
        RtsSGrid* pGrid = m_pGrid + i;
        if (_cam.CheckVisable(pGrid->vMin, pGrid->vMax))
            pgrid->push_back(pGrid);
    }
}

BYTE RtSceneBlock::IncreaseOpFlag()
{
    if (m_cLastOpFlag==0xFF)
    {
        ClearOpFlag();
    }
    m_cLastOpFlag ++;
    return m_cLastOpFlag;
}

BYTE RtSceneBlock::GetLastOpFlag()
{
    return m_cLastOpFlag;
}

void RtSceneBlock::ClearOpFlag()
{
    m_cLastOpFlag = 0;
    memset(m_pOpFlag, 0, sizeof(BYTE)*g_iSceneTileCntX*g_iSceneTileCntY);
}

void RtSceneBlock::UpdateGrid(int iMinX, int iMinY, int iMaxX, int iMaxY)
{
    int x, y, gx, gy, iGMinX, iGMinY, iGMaxX, iGMaxY;

    if (iMinX<0) iMinX = 0;
    if (iMaxX>g_iSceneTileCntX) iMaxX = g_iSceneTileCntX;
    if (iMinX<0) iMaxX = 0;
    if (iMaxY>g_iSceneTileCntY) iMaxY = g_iSceneTileCntY;

    iGMinX = (iMinX/RTS_BLOCK_GRID_SIZE);
    iGMinY = (iMinY/RTS_BLOCK_GRID_SIZE);
    iGMaxX = ((iMaxX-1)/RTS_BLOCK_GRID_SIZE)+1;
    iGMaxY = ((iMaxY-1)/RTS_BLOCK_GRID_SIZE)+1;

    RtsSGrid* pGrid;
    RtsSVertex* pVertex;
    for (gy=iGMinY; gy<iGMaxY; gy++)
    {
        for (gx=iGMinX; gx<iGMaxX; gx++)
        {
            pGrid = m_pGrid+(gy*g_iSceneGridCntX+gx);
            pGrid->vMin.z = 1e10;
            pGrid->vMax.z = -1e10;
            for (y=pGrid->y; y<=pGrid->y+RTS_BLOCK_GRID_SIZE; y++)
            {
                for (x=pGrid->x; x<=pGrid->x+RTS_BLOCK_GRID_SIZE; x++)
                {
                    if (x>=g_iSceneTileCntX) continue;
                    if (y>=g_iSceneTileCntY) continue;
                    pVertex = m_pVertices + (y*g_iSceneTileCntX + x)*4;
                    if (pVertex->pPos->v.z<pGrid->vMin.z) pGrid->vMin.z = pVertex->pPos->v.z;
                    if (pVertex->pPos->v.z>pGrid->vMax.z) pGrid->vMax.z = pVertex->pPos->v.z;
                    if (pVertex->pPos->fWaterHeight>pGrid->vMax.z) pGrid->vMax.z = pVertex->pPos->fWaterHeight;
                }
            }
        }
    }

    m_vMin.z = 1e10;
    m_vMax.z = -1e10;
    for (x=0; x<m_iGridSize; x++)
    {
        if (m_pGrid[x].vMin.z<m_vMin.z) m_vMin.z=m_pGrid[x].vMin.z;
        if (m_pGrid[x].vMax.z>m_vMax.z) m_vMax.z=m_pGrid[x].vMax.z;
    }

}

RtsObject* RtSceneBlock::PickRay(const RtgVertex3 &v, const RtgVertex3 &dir, float& fOutDist, int& iOutX, int& iOutY, DWORD dwType)
{
    RtgVertex2 c, f, o;
    int x, y, ox, oy, bx=0, by=0;
    float d1, d2, fDist, fMinDistObj = 1e10, fTerrainDist = 1e10;
    RtsSGrid *pGrid = NULL, *pG;
    RtsSIndex* pFace;
    RtsObject* pObject, *pSelObject=NULL, *pMinDistObj=NULL;
    RtgVertex3 v0, v1, v2, vDist, vd, vLine0, vLine1, vMin, vMax;
    RtgMatrix16 mI16;

    // 先取目标点，用于和物件的碰撞，距离不能设置太小，以免近的物体没有被检测
    vd = v + dir*10000.f;

    // 取出方向
    ox = (dir.x>0.f)?1:0;
    oy = (dir.y>0.f)?1:0;

    if (!rtg2DRectangleCutLine(v, vd, m_vMin, m_vMax, v0, v1))
    {
        return NULL;
    }

    // 取出原点坐标
    c.x = v0.x;
    c.y = v0.y;

    // 取出位置
    x = (c.x - m_fOffsetX)/g_fSceneTileWidth;
    y = (c.y - m_fOffsetY)/g_fSceneTileWidth;

    // 设置Test值
    float fTestLenght=-1.f, fTestLenCnt=0.f;
    int iTestCount=0;
#if DO_CONSOLE
    RtgVertex3 l1, l2=v;
#endif

    d1 = Abs(dir.x/dir.y);

    // 开始
    if (x==g_iSceneTileCntX) x = g_iSceneTileCntX-1;
    if (y==g_iSceneTileCntY) y = g_iSceneTileCntY-1;
    while (x>=0 && y>=0 && x<g_iSceneTileCntX && y<g_iSceneTileCntY)
    {
        f.x = (x+ox)*g_fSceneTileWidth + m_fOffsetX;
        f.y = (y+oy)*g_fSceneTileWidth + m_fOffsetY;

        // x 象项 和 y 象项
        o = f - c;

        d2 = Abs(o.x/o.y);

        if (d1 < d2)
        {
            bx = 0;
            by = oy?1:-1;
        }else if (d1 > d2)
        {
            bx = ox?1:-1;
            by = 0;
        }else
        {
            bx = ox?1:0;
            by = oy?1:0;
            if (bx==0 && by==0)
            {
                bx = by = -1;
            }
        }

        {   // 碰撞
            pG = m_pGrid + ((y/RTS_BLOCK_GRID_SIZE)*g_iSceneGridCntX+(x/RTS_BLOCK_GRID_SIZE)); // 取得格子

            if (pGrid!=pG) // 如果和上一次不在同一个格子里面
            {
                if (rtgIsLineIntersectAABB(v, vd, pG->vMin, pG->vMax))
                { // 比较格子内的模型
                    pGrid = pG;
                    if (dwType&(PICK_OBJECT|PICK_HELPER)) // 物件
                    {
                        for (int iObj=0; iObj<pGrid->aryObject.size(); iObj++)
                        {
                            pObject = RtObjectManager()->GetObject(pGrid->aryObject[iObj]);
                            if (pObject)
                            {
                                if (pObject->GetLastRenderFrame() >= RtGetRender()->GetRenderFrame())
                                    continue;
                                pObject->SetLastRenderFrame(RtGetRender()->GetRenderFrame());


                                if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
                                {
                                    if ((dwType&PICK_OBJECT)==0)
                                        continue;
                                }else
                                {
                                    if ((dwType&PICK_HELPER)==0)
                                        continue;
                                }
                                mI16 = pObject->GetMatrix().Inverse();
                                vLine0 = v  * mI16;
                                vLine1 = vd * mI16;
                                pObject->Get_AABBox()->GetMin(&vMin);
                                pObject->Get_AABBox()->GetMax(&vMax);
                             //   vMin = pObject->m_AABB.vPos - pObject->m_AABB.vExt;
                            //    vMax = pObject->m_AABB.vPos + pObject->m_AABB.vExt;
                                if (rtgIsLineIntersectAABB(vLine0, vLine1, vMin, vMax, &fDist, NULL))
                                {
                                    fDist = vLine0.Distance(vLine1) * fDist;
									if(pObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
										
#if DO_CONSOLE
                                    if (g_pScene->m_bEnablePickRayRecord) g_pScene->AddPickRayPoint(fDist);
#endif
                                    if (fDist<fMinDistObj)
                                    {
                                        fMinDistObj = fDist;
                                        pMinDistObj = pObject;
                                        // 距离最小的放到最前面
                                        pObject->m_pObjectLink = pSelObject;
                                        pSelObject = pObject;
                                    }else
                                    {
                                        // 否则放到第二个位置
                                        pObject->m_pObjectLink = pSelObject->m_pObjectLink;
                                        pSelObject->m_pObjectLink = pObject;
                                    }
                                }
                            }
                        }
                    }else if (dwType&PICK_NPC) // NPC
                    {
                        (dwType&PICK_TRIANGLE);
                    }
                }
            }

            // 和地表做碰撞
            {
                pFace = m_pIndices + (y*g_iSceneTileCntX+x)*2*3;
                if (dwType&PICK_VTERRAIN)
                {
                    if (rtgIsRayHitTriangle(v, dir, m_pVertices[pFace[0].wIdx].pPos->v, m_pVertices[pFace[1].wIdx].pPos->v, m_pVertices[pFace[2].wIdx].pPos->v, &fDist))
                    {
                        if (fDist>=0.f)
                        {
                            iOutX = x;
                            iOutY = y;
                            fTerrainDist = fDist;
                            break;
                        }
                    }else if (rtgIsRayHitTriangle(v, dir, m_pVertices[pFace[3].wIdx].pPos->v, m_pVertices[pFace[4].wIdx].pPos->v, m_pVertices[pFace[5].wIdx].pPos->v, &fDist))
                    {
                        if (fDist>=0.f)
                        {
                            iOutX = x;
                            iOutY = y;
                            fTerrainDist = fDist;
                            break;
                        }
                    }
                }else if (dwType&PICK_TERRAIN)
                {
                    v0 = m_pVertices[pFace[0].wIdx].pPos->v;
                    v0.z = m_pVertices[pFace[0].wIdx].pPos->fTerrainHeight;
                    v1 = m_pVertices[pFace[1].wIdx].pPos->v;
                    v1.z = m_pVertices[pFace[1].wIdx].pPos->fTerrainHeight;
                    v2 = m_pVertices[pFace[2].wIdx].pPos->v;
                    v2.z = m_pVertices[pFace[2].wIdx].pPos->fTerrainHeight;
                    if (rtgIsRayHitTriangle(v, dir, v0, v1, v2, &fDist))
                    {
                        if (fDist>=0.f)
                        {
                            vDist = v + dir*fDist;
                            iOutX = (vDist.x-m_fOffsetX)/g_fSceneTerrainTileWidth;
                            iOutY = (vDist.y-m_fOffsetY)/g_fSceneTerrainTileWidth;
                            fTerrainDist = fDist;
                            break;
                        }
                    }else
                    {
                        v0 = m_pVertices[pFace[3].wIdx].pPos->v;
                        v0.z = m_pVertices[pFace[3].wIdx].pPos->fTerrainHeight;
                        v1 = m_pVertices[pFace[4].wIdx].pPos->v;
                        v1.z = m_pVertices[pFace[4].wIdx].pPos->fTerrainHeight;
                        v2 = m_pVertices[pFace[5].wIdx].pPos->v;
                        v2.z = m_pVertices[pFace[5].wIdx].pPos->fTerrainHeight;
                        if (rtgIsRayHitTriangle(v, dir, v0, v1, v2, &fDist))
                        {
                            if (fDist>=0.f)
                            {
                                vDist = v + dir*fDist;
                                iOutX = (vDist.x-m_fOffsetX)/g_fSceneTerrainTileWidth;
                                iOutY = (vDist.y-m_fOffsetY)/g_fSceneTerrainTileWidth;
                                fTerrainDist = fDist;
                                break;
                            }
                        }
                    }
                }
            }
        }
        x += bx;
        y += by;
    }
#if DO_CONSOLE
    if (fTerrainDist<1e10)
    {
        if (g_pScene->m_bEnablePickRayRecord) g_pScene->AddPickRayPoint(fTerrainDist);
    }
#endif
    if (pMinDistObj && fMinDistObj<fTerrainDist)
    {
        fOutDist = fMinDistObj;
        return pSelObject; // 物件
    }else
    {
        if (fTerrainDist<1e10)
        {
            fOutDist = fTerrainDist;
            return (RtsObject*)-1; // 找到地表
        }
    }
    return NULL;
}

// 返回的X和Y都是行走网格的XY
RtsObject* RtSceneBlock::PickLine(const RtgVertex3 &v0, const RtgVertex3 &v1, float& fOutDist, int& iOutX, int& iOutY, DWORD dwType)
{
    RtgVertex3 dir = v1-v0;
    float fLen2;
    RtsObject *pObj;
    fLen2 = dir.Length2();
    dir.Normalize();
    if (pObj=PickRay(v0, dir, fOutDist, iOutX, iOutY, dwType))
    {
        if (fOutDist*fOutDist<fLen2)
            return pObj;
    }
    return NULL;
}

bool RtSceneBlock::GetHeight(const RtgVertex3 &v, float& fOutDist, int& iOutX, int& iOutY, DWORD dwType)
{
    RtsSIndex* pFace;
    RtgVertex3 v0, v1, v2;
    RtgVertex3 dir(0, 0, -1);

    iOutX = (v.x - m_fOffsetX)/g_fSceneTileWidth;
    iOutY = (v.y - m_fOffsetY)/g_fSceneTileWidth;

    if (iOutX<0 || iOutX>=g_iSceneTileCntX || iOutY<0 || iOutY>=g_iSceneTileCntY)
        return false;

    pFace = m_pIndices + (iOutY*g_iSceneTileCntX+iOutX)*2*3;
    if (dwType&PICK_VTERRAIN)
    {
        if (rtgIsRayHitTriangle(v, dir, m_pVertices[pFace[0].wIdx].pPos->v, m_pVertices[pFace[1].wIdx].pPos->v, m_pVertices[pFace[2].wIdx].pPos->v, &fOutDist))
        {
            return true; // 找到地表
        }else if (rtgIsRayHitTriangle(v, dir, m_pVertices[pFace[3].wIdx].pPos->v, m_pVertices[pFace[4].wIdx].pPos->v, m_pVertices[pFace[5].wIdx].pPos->v, &fOutDist))
        {
            return true; // 找到地表
        }
    }else if (dwType&PICK_TERRAIN)
    {
        v0 = m_pVertices[pFace[0].wIdx].pPos->v;
        v0.z = m_pVertices[pFace[0].wIdx].pPos->fTerrainHeight;
        v1 = m_pVertices[pFace[1].wIdx].pPos->v;
        v1.z = m_pVertices[pFace[1].wIdx].pPos->fTerrainHeight;
        v2 = m_pVertices[pFace[2].wIdx].pPos->v;
        v2.z = m_pVertices[pFace[2].wIdx].pPos->fTerrainHeight;
        if (rtgIsRayHitTriangle(v, dir, v0, v1, v2, &fOutDist))
        {
            return true; // 找到地表
        }else
        {
            v0 = m_pVertices[pFace[3].wIdx].pPos->v;
            v0.z = m_pVertices[pFace[3].wIdx].pPos->fTerrainHeight;
            v1 = m_pVertices[pFace[4].wIdx].pPos->v;
            v1.z = m_pVertices[pFace[4].wIdx].pPos->fTerrainHeight;
            v2 = m_pVertices[pFace[5].wIdx].pPos->v;
            v2.z = m_pVertices[pFace[5].wIdx].pPos->fTerrainHeight;
            if (rtgIsRayHitTriangle(v, dir, v0, v1, v2, &fOutDist))
            {
                return true; // 找到地表
            }
        }
    }
    return false;
}

bool RtSceneBlock::GetTerrainHeight(float fX, float fY, float& outZ)
{
    RtgVertex3 v(fX, fY, 0.f);
    int iOutX, iOutY;
    bool bResult = GetHeight(v, outZ, iOutX, iOutY, PICK_TERRAIN);
    if (bResult)
    {
        outZ = 0.f - outZ;
    }
    return bResult;
}

RtsModel* RtSceneBlock::FindFirstModel(const char* szFilename)
{
    int i, j;
    RtsSGrid* pGrid;
    RtsObject* pObj;
    for (i=0; i<m_iGridSize; i++)
    {
        pGrid = m_pGrid+i;
        for (j=0; j<pGrid->aryObject.size(); j++)
        {
            pObj = RtObjectManager()->GetObject(pGrid->aryObject[j]);
            if (pObj && pObj->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
            {
				if (stricmp(((RtsActor*)pObj)->GetActor()->GetCore()->m_poTag.c_str(), szFilename)==0)
                {
                    return (RtsModel*)pObj;
                }
            }
        }
    }

    return NULL;
}

// 环境光 = 光的强度
// 平行光 = (光线方向*点的法向量)*光的强度
// 点光源 = (光线方向*点的法向量)*光的强度*衰减系数
// 衰减系数 = min(1, 1/(a0+a1*距离+a2*距离*距离)
// 总的亮度 = 各个光源相加
void RtSceneBlock::ComputeVertexColor(int iX, int iY, RtsSVertex* pVertex)
{

    static RtgVertex3 vDif(1.f, 1.f, 1.f);
    static RtgVertex3 vEmi(0.f, 0.f, 0.f);
    static RtgVertex3 vAmb(1.f, 1.f, 1.f);

    /*
    BOOL bEdge = (iX==0 || iY==0 || iX==(g_iSceneTileCntX-1) || iY==(g_iSceneTileCntY-1));
    if (bEdge)
    {
        pVertex->pPos->dwTerrainColor = pVertex->pPos->c = 0x00000000;
        return;
    }
    */

    // 生成影响顶点的灯光列表
    // 然后计算灯光
    RtsLight* pLights = g_pScene->GetRangeLights(pVertex->pPos->v, 200.f, 0x01);
    if (pLights==NULL)
    {
        pVertex->pPos->c = 0x00000000;
    }else
    {
        pVertex->pPos->c = RtgComputeVertexColor(pVertex->pPos->v, pVertex->pPos->n, vAmb, vDif, vEmi, g_pScene->m_lightTerrainAmbient, pLights);
    }

    RtgVertex3 vTP(pVertex->pPos->v);
    vTP.z = pVertex->pPos->fTerrainHeight;
    pLights = g_pScene->GetRangeLights(vTP, 200.f, 0x01);
    if (pLights==NULL)
    {
        pVertex->pPos->dwTerrainColor = 0x00000000;
    }else
    {
        pVertex->pPos->dwTerrainColor = RtgComputeVertexColor(vTP, pVertex->pPos->n, vAmb, vDif, vEmi, g_pScene->m_lightTerrainAmbient, pLights);
    }
}

/*
void RtSceneBlock::ComputeVertexNormal(RtsSVertex* pVertex)
{
    RtgVertex3 l1, l2, l3, l4, l5, l6;
    RtgVertex3 n1, n2, n3, n4, n5, n6;
    RtsSVertex* pV;

    pV = pVertex-(g_iSceneTileCntX+1)*4;
    l1 = pV[3].pPos->v - pV[2].pPos->v;
    l2 = pV[3].pPos->v - pV[1].pPos->v;
    pV = pVertex-(g_iSceneTileCntX+0)*4;
    l3 = pV[2].pPos->v - pV[1].pPos->v;
    l4 = pV[2].pPos->v - pV[3].pPos->v;
    pV = pVertex-4;
    l5 = pV[1].pPos->v - pV[3].pPos->v;
    l6 = pV[1].pPos->v - pV[2].pPos->v;

    n1 = l1.Cross(l2);
    n2 = l2.Cross(l3);
    n3 = l3.Cross(l4);
    n4 = l4.Cross(l5);
    n5 = l5.Cross(l6);
    n6 = l6.Cross(l1);

    n1 = (n1 + n2 + n3 + n4 + n5 + n6)/6.f;
    n1.Normalize();
    pVertex->pPos->n = n1;
}

void RtSceneBlock::ComputeVertexNormal(int iX, int iY)
{
    if (iX<=0 || iY<=0 || iX>g_iSceneTileCntX || iY>g_iSceneTileCntY) return;
    ComputeVertexNormal(m_pVertices+(iY*g_iSceneTileCntX+iX)*4);
}
*/

void RtSceneBlock::ComputeVertexColor(int iX, int iY)
{
    if (iX<0 || iY<0 || iX>=g_iSceneTileCntX || iY>=g_iSceneTileCntY) return;
    ComputeVertexColor(iX, iY, m_pVertices+(iY*g_iSceneTileCntX+iX)*4);
}

void RtSceneBlock::UpdateVertexPositionByHeightmap()
{
    RtsSVertexValue* pPos = m_pPositions;
    int x, y;
    for (y=0; y<=g_iSceneTileCntY; y++)
    {
        for (x=0; x<=g_iSceneTileCntX; x++)
        {
            pPos->v.x = x*g_fSceneTileWidth + m_fOffsetX;
            pPos->v.y = y*g_fSceneTileWidth + m_fOffsetY;
            pPos->v.z = pPos->cHeightMap * g_fSceneHeightStep;
            pPos ++;
        }
    }
}

void RtSceneBlock::UpdateVertexPositionByTerrainHeightmap()
{
    RtsSVertexValue* pPos = m_pPositions;
    int x, y;
    for (y=0; y<=g_iSceneTileCntY; y++)
    {
        for (x=0; x<=g_iSceneTileCntX; x++)
        {
            pPos->fTerrainHeight = pPos->cTerrainHMap * g_fSceneHeightStep;
            pPos ++;
        }
    }
}

void RtSceneBlock::UpdateVertexPositionByWaterHeightmap()
{
    const BYTE cAlphaHeight = 20;
    const BYTE cMaxAlpha = 0xB0;
    const float fTmp = cMaxAlpha/cAlphaHeight;
    int iWaterIdx = 0;

    RtsSVertexValue* pPos = m_pPositions;
    RtsSTileMap* pTile = m_pTileMapIdx;
    RtsSSceneWater* pWater;
    int x, y, i;
    for (y=0; y<=g_iSceneTileCntY; y++)
    {
        if (y==g_iSceneTileCntY)
        {
            pTile -= g_iSceneTileCntX;
        }
        for (x=0; x<=g_iSceneTileCntX; x++)
        {
            if (pTile->cWaterType)
            {
                pPos->fWaterHeight = pPos->cWaterHMap * g_fSceneHeightStep;
                i = pPos->cWaterHMap - pPos->cHeightMap;
                //if (x==0 || y==0 || x==g_iSceneTileCntX || y==g_iSceneTileCntY)
                //{
                //    pPos->cWaterAlphaMap = 0x00;
                //}else
                {
                    iWaterIdx = pTile->cWaterType-1;
                    if (iWaterIdx<0 || iWaterIdx>=RtScene::MAX_WATER_COUNT)
                    {
                        iWaterIdx = 0;
                    }
                    pWater = g_pScene->m_Waters + iWaterIdx;
                    if (i>pWater->iDepth)
                    {
                        pPos->cWaterAlphaMap = (BYTE)pWater->iAlpha;
                    }else if (i<0)
                    {
                        pPos->cWaterAlphaMap = 0x00;
                    }else
                    {
                        if (pWater->iDepth==0)
                        {
                            pPos->cWaterAlphaMap = 0;
                        }else
                        {
                            pPos->cWaterAlphaMap = i*pWater->iAlpha/pWater->iDepth;
                        }
                    }
                }
            }else
            {
                pPos->fWaterHeight = 0.f;
            }
            pPos ++;
            if (x!=g_iSceneTileCntX)
            {
                pTile ++;
            }
        }
    }
}

void RtSceneBlock::RecomputeAllLight(RtgDevice& inDevice)
{
    for (int y = 0; y <= g_iSceneTileCntY; ++y)
        for (int x = 0; x <= g_iSceneTileCntX; x++)
            ComputeVertexColor(x, y);

    RtgVertex3 _lightOffset = RtgVertex3(0.f, 0.f, 0.f);

    for (int i = 0; i < m_iGridSize; ++i)
    {
        RtsSGrid* _grid = m_pGrid + i;

        for (int j = 0; j < _grid->aryObject.size(); ++j)
        {
            RtsObject* _obj = RtObjectManager()->GetObject(_grid->aryObject[j]);

            if (_obj && _obj->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
            {
                RtsModel* _model = (RtsModel*)_obj;

                if (!_model->GetActor())
                {
                    inDevice.UpdateAppTime();
                    _model->RecreateResource(_grid);
                }

                if (!_model->GetActor())
                    continue;

                RtgVertex3 _pos = _model->Get_AABBox()->vPos * _model->GetMatrix();
                RtgLightItem* _light = g_pScene->GetRangeLights(_pos, 200.f, 0x01);
                _model->m_aryVc.clear();
                _model->GetActor()->ComputeVC(_model->GetMatrix(), _model->m_aryVc, _lightOffset, _light);
                _model->GetActor()->SetVC(_model->m_aryVc);
            }
        }
    }

    inDevice.UpdateAppTime();
}

int RtSceneBlock::GetTerrainMapIdx(float fX, float fY)
{
    int iX, iY;
    iX = (fX-m_fOffsetX)/g_fSceneTileWidth;
    iY = (fY-m_fOffsetY)/g_fSceneTileWidth;
    CHECK(iX>=0 && iY>=0 && iX<=g_iSceneTileCntX && iY<=g_iSceneTileCntY);
    return (m_pTileMapIdx[iY*g_iSceneTileCntX+iX].dwSortMapIdx&0x0FF);
}

DWORD RtSceneBlock::GetTerrainColor(float fX, float fY)
{
    int iX, iY;
    iX = (fX-m_fOffsetX)/g_fSceneTileWidth;
    iY = (fY-m_fOffsetY)/g_fSceneTileWidth;
    if (!(iX>=0 && iY>=0 && iX<=g_iSceneTileCntX && iY<=g_iSceneTileCntY))
    {
        // CHECKEX("取颜色的位置错误");
        RtCoreLog().Error("RtSceneBlock: 取颜色的位置[%.2f,%.2f]出错, 坐标为[%d,%d].\n", fX, fY, iX, iY);
        return 0xFFFF0000;
    }

    int iPos = iY*(g_iSceneTileCntX+1)+iX;
    RtgVertex3 col[4];
    col[0].x = (m_pPositions[iPos].dwTerrainColor&0x00FF0000)>>16;
    col[0].y = (m_pPositions[iPos].dwTerrainColor&0x0000FF00)>>8;
    col[0].z = (m_pPositions[iPos].dwTerrainColor&0x000000FF);

    col[1].x = (m_pPositions[iPos+1].dwTerrainColor&0x00FF0000)>>16;
    col[1].y = (m_pPositions[iPos+1].dwTerrainColor&0x0000FF00)>>8;
    col[1].z = (m_pPositions[iPos+1].dwTerrainColor&0x000000FF);

    col[2].x = (m_pPositions[iPos+(g_iSceneTileCntX+1)].dwTerrainColor&0x00FF0000)>>16;
    col[2].y = (m_pPositions[iPos+(g_iSceneTileCntX+1)].dwTerrainColor&0x0000FF00)>>8;
    col[2].z = (m_pPositions[iPos+(g_iSceneTileCntX+1)].dwTerrainColor&0x000000FF);

    col[3].x = (m_pPositions[iPos+(g_iSceneTileCntX+2)].dwTerrainColor&0x00FF0000)>>16;
    col[3].y = (m_pPositions[iPos+(g_iSceneTileCntX+2)].dwTerrainColor&0x0000FF00)>>8;
    col[3].z = (m_pPositions[iPos+(g_iSceneTileCntX+2)].dwTerrainColor&0x000000FF);

    float b = (fX - iX * g_fSceneTileWidth - m_fOffsetX) / g_fSceneTileWidth;
    col[0] = col[0] + b*(col[1] - col[0]);
    col[2] = col[2] + b*(col[3] - col[2]);

    b = (fY - iY * g_fSceneTileWidth - m_fOffsetY) / g_fSceneTileWidth;
    col[0] = col[0] + b*(col[2] - col[0]);

    col[0] /= 255.0f;
    return RtgVectorToColor(col[0],1.0f);
    // return m_pPositions[iY*(g_iSceneTileCntX+1)+iX].c;
}

void RtSceneBlock::DrawNormal(RtgCamera& inCamera, RtgDevice& inDevice, DWORD dwColor, float fLength)
{
#ifndef RENDER_MULTI_DRAW
    int iMapLevel;
    RtsSVertex* pVertex;
    RtgShader nullShader;
    inDevice.SetShader(&nullShader);
    for (iMapLevel=0; iMapLevel<4; iMapLevel++)
    {
        for (pVertex=m_pRenderStartVertex[iMapLevel].pHead; pVertex!=0; pVertex=pVertex->pNext)
        {
            inDevice.DrawLine(pVertex->pPos->v, pVertex->pPos->v + pVertex->pPos->n * fLength, &dwColor);
        }
    }
    inDevice.RestoreShader(&nullShader);
#endif
}

void GettTerrainVertex(RtSceneBlock* pScene, int iIdx, int x, int y, RtgVertex3& v)
{
    RtsSVertex* pV = pScene->m_pVertices + (y*g_iSceneTileCntX+x)*4;
    bool bTwoValue = true;
    RtgVertex3 v1, v2;
    switch (iIdx)
    {
    case 0:
        v1 = pV[0].pPos->v;   v1.z = pV[0].pPos->fTerrainHeight;
        bTwoValue = false;
        break;
    case 1:
        v1 = pV[0].pPos->v;   v1.z = pV[0].pPos->fTerrainHeight;
        v2 = pV[1].pPos->v;   v2.z = pV[1].pPos->fTerrainHeight;
        break;
    case 2:
        v1 = pV[1].pPos->v;   v1.z = pV[1].pPos->fTerrainHeight;
        bTwoValue = false;
        break;
    case 3:
        v1 = pV[0].pPos->v;   v1.z = pV[0].pPos->fTerrainHeight;
        v2 = pV[2].pPos->v;   v2.z = pV[2].pPos->fTerrainHeight;
        break;
    case 4:
        v1 = pV[1].pPos->v;   v1.z = pV[1].pPos->fTerrainHeight;
        v2 = pV[2].pPos->v;   v2.z = pV[2].pPos->fTerrainHeight;
        break;
    case 5:
        v1 = pV[1].pPos->v;   v1.z = pV[1].pPos->fTerrainHeight;
        v2 = pV[3].pPos->v;   v2.z = pV[3].pPos->fTerrainHeight;
        break;
    case 6:
        v1 = pV[2].pPos->v;   v1.z = pV[2].pPos->fTerrainHeight;
        bTwoValue = false;
        break;
    case 7:
        v1 = pV[2].pPos->v;   v1.z = pV[2].pPos->fTerrainHeight;
        v2 = pV[3].pPos->v;   v2.z = pV[3].pPos->fTerrainHeight;
        break;
    case 8:
        v1 = pV[3].pPos->v;   v1.z = pV[3].pPos->fTerrainHeight;
        bTwoValue = false;
        break;
    }
    if (bTwoValue)
    {
        v = (v1+v2)/2.f;
    }else
    {
        v = v1;
    }
}

void RtSceneBlock::DrawTerrain(RtgCamera& inCamera, RtgDevice& inDevice, BYTE cAttrMask, DWORD dwNormalColor, DWORD dwHitColor, float fOffset)
{
    int x, y, i, j, g, iIdx;
    RtsSGrid* pGrid;
    DWORD dwVertexCount;
    RtgShader terrainShader;
    RtgShader terrainLineShader;
    RtgVertexVC pVertices[RTS_BLOCK_GRID_SIZE*RTS_BLOCK_GRID_SIZE*24];
    RtgVertexVC* pV;
    RtgVertex3 v[9];
    DWORD dwColor;
    int iTriangleIdx[24] = 
    {
        0, 1, 3,
        1, 4, 3,
        1, 2, 4,
        2, 5, 4,
        3, 4, 6,
        4, 7, 6,
        4, 5, 7,
        5, 8, 7
    };
    CHECK(g_iSceneTileRate==2); // 必须为2，否则这个函数的内容要改

    terrainShader.dwTextureFactor = 0x80FFFFFF;
    terrainShader.iNumStage = 1;
    terrainShader.bEnableBlendFunc = true;
    terrainShader.eBlendSrcFunc = RTGBLEND_SRCALPHA;
    terrainShader.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
    terrainShader.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
    terrainShader.Shaders[0].eColorArg1 = RTGTA_DIFFUSE;
    terrainShader.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
    terrainShader.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;

    terrainLineShader.iNumStage = 0;

    inDevice.SetRenderState(RTGRS_Z_WRITE, FALSE);
    inDevice.SetVertexFormat(RTG_VC);
    for (g=0; g<m_iGridSize; g++)
    {
        pGrid = m_pGrid+g;
        if (inCamera.CheckVisable(pGrid->vMin, pGrid->vMax))
        {
            // 开始组织三角形
            dwVertexCount = 0;
            pV = pVertices;
            for (y=0; y<RTS_BLOCK_GRID_SIZE; y++)
            {
                for (x=0; x<RTS_BLOCK_GRID_SIZE; x++)
                {
                    if ((x+pGrid->x)>=g_iSceneTileCntX) continue;
                    if ((y+pGrid->y)>=g_iSceneTileCntY) continue;
                    iIdx = (y+pGrid->y)*g_iSceneTileCntX*g_iSceneTileRate*g_iSceneTileRate+(x+pGrid->x)*g_iSceneTileRate;

                    for (i=0; i<24; i++)
                    {
                        GettTerrainVertex(this, iTriangleIdx[i], x+pGrid->x, y+pGrid->y, pV[i].v);
                        pV[i].v.z += fOffset;
                    }
                    for (i=0; i<4; i++)
                    {
                        dwColor = dwNormalColor;
                        switch (i)
                        {
                        case 0:
                            if (m_pTileAttr[iIdx+0].cAttr&cAttrMask)
                                dwColor = dwHitColor;
                            break;
                        case 1:
                            if (m_pTileAttr[iIdx+1].cAttr&cAttrMask)
                                dwColor = dwHitColor;
                            break;
                        case 2:
                            if (m_pTileAttr[iIdx+0+g_iSceneTileCntX*g_iSceneTileRate].cAttr&cAttrMask)
                                dwColor = dwHitColor;
                            break;
                        case 3:
                            if (m_pTileAttr[iIdx+1+g_iSceneTileCntX*g_iSceneTileRate].cAttr&cAttrMask)
                                dwColor = dwHitColor;
                            break;
                        }
                        for (j=0; j<6; j++)
                        {
                            pV->c = dwColor;
                            pV ++;
                        }
                    }
                    dwVertexCount += 24;
                    inDevice.SetRenderState(RTGRS_Z_WRITE, FALSE);
                }
            }
            // 开始画图
            inDevice.SetShader(&terrainShader);
            inDevice.DrawPrimitiveUP(D3DPT_TRIANGLELIST, pVertices, sizeof(RtgVertexVC), dwVertexCount/3);
            inDevice.RestoreShader(&terrainShader);

            inDevice.SetRenderState(RTGRS_FILL_MODE, RTG_FILLWIREFRAME);
            inDevice.SetShader(&terrainLineShader);
            inDevice.DrawPrimitiveUP(D3DPT_TRIANGLELIST, pVertices, sizeof(RtgVertexVC), dwVertexCount/3);

            inDevice.RestoreShader(&terrainLineShader);
            inDevice.SetRenderState(RTGRS_FILL_MODE, RTG_FILLSOLID);
        }
    }

    inDevice.SetRenderState(RTGRS_Z_WRITE, TRUE);
    //inDevice.SetRenderState(RTGRS_Z_TEST, TRUE);
}

bool RtSceneBlock::TakeGeometry(const RtgAABB& inAABB, RtgVertex3* pVertices, int& iVertexNum, short* pIndices, int& iIndexNum)
{
    CHECK( pVertices!=NULL );
    CHECK( pIndices!=NULL );
    int iMinX, iMinY, iMaxX, iMaxY;
    int i, j, iVB, iIB;
    short pIdx[6] = {0, 1, 2, 1, 3, 2};

    iMinX = (int)(((inAABB.vPos.x-inAABB.vExt.x)-m_fOffsetX)/g_fSceneTileWidth);
    iMinY = (int)(((inAABB.vPos.y-inAABB.vExt.y)-m_fOffsetY)/g_fSceneTileWidth);
    iMaxX = (int)(((inAABB.vPos.x+inAABB.vExt.x)-m_fOffsetX)/g_fSceneTileWidth);
    iMaxY = (int)(((inAABB.vPos.y+inAABB.vExt.y)-m_fOffsetY)/g_fSceneTileWidth);

    if (iMinX<0) iMinX = 0;
    if (iMinY<0) iMinY = 0;
    if (iMaxX>=g_iSceneTileCntX) iMaxX = g_iSceneTileCntX-1;
    if (iMaxY>=g_iSceneTileCntY) iMaxY = g_iSceneTileCntY-1;

    CHECK(iMinX<=iMaxX);
    CHECK(iMinY<=iMaxY);

    i = (iMaxX-iMinX+1)*(iMaxY-iMinY+1);
    iVB = i * 4;
    iIB = i * 6;

    if (iVertexNum<iVB || iIndexNum<iIB)
    {
        iVertexNum = iVB;
        iIndexNum  = iIB;
        return false;
    }
    iVertexNum = iVB;
    iIndexNum  = iIB;

    i = 0;
    RtsSVertex* pPos;
    for (; iMinY<=iMaxY; iMinY++)
    {
        for (j=iMinX; j<=iMaxX; j++)
        {
            pPos = m_pVertices + (iMinY*g_iSceneTileCntX + j)*4;

            pVertices[0].x = pPos[0].pPos->v.x;
            pVertices[0].y = pPos[0].pPos->v.y;
            pVertices[0].z = pPos[0].pPos->fTerrainHeight;
            pVertices[1].x = pPos[1].pPos->v.x;
            pVertices[1].y = pPos[1].pPos->v.y;
            pVertices[1].z = pPos[1].pPos->fTerrainHeight;
            pVertices[2].x = pPos[2].pPos->v.x;
            pVertices[2].y = pPos[2].pPos->v.y;
            pVertices[2].z = pPos[2].pPos->fTerrainHeight;
            pVertices[3].x = pPos[3].pPos->v.x;
            pVertices[3].y = pPos[3].pPos->v.y;
            pVertices[3].z = pPos[3].pPos->fTerrainHeight;

            pIndices[0] = i + pIdx[0];
            pIndices[1] = i + pIdx[1];
            pIndices[2] = i + pIdx[2];
            pIndices[3] = i + pIdx[3];
            pIndices[4] = i + pIdx[4];
            pIndices[5] = i + pIdx[5];

            i += 4;
            pVertices += 4;
            pIndices += 6;
        }
    }
    return true;
}

//void RtSceneBlock::ClearObjectLink()
//{
//    // 创建Grid
//    int x, y;
//    RtsSGrid* pGrid;
//    for (y=0; y<g_iSceneGridCntY; y++)
//    {
//        for (x=0; x<g_iSceneGridCntX; x++)
//        {
//            pGrid = m_pGrid+(y*g_iSceneGridCntX+x);
//            DEL_ARRAY(pGrid->ppObject);
//            pGrid->iObjectNum = 0;
//        }
//    }
//}

void RtSceneBlock::ReIndexObjectList()
{
}


bool RtSceneBlock::OnResourceAllocate()
{
    RtSceneBlockTerrain::OnResourceAllocate();
    ClearOpFlag();
    memset(m_pTileMapIdx, 0, sizeof(RtsSTileMap)*g_iSceneTileCntX*g_iSceneTileCntY);
    memset(m_pPositions, 0, sizeof(RtsSVertexValue)*(g_iSceneTileCntX+1)*(g_iSceneTileCntY+1));
    memset(m_pVertices, 0, sizeof(RtsSVertex)*g_iSceneTileCntX*g_iSceneTileCntY*4 );

    if ( m_pVertices==0 || m_pIndices==0 )
    {
        RtCoreLog().Error("ERROR: 不能创建资源.\n");
        return false;
    }
    return true;
}

bool RtSceneBlock::OnResourceFree()
{
    if (m_pGrid)
        for (int i = 0; i < m_iGridSize; ++i)
            for (size_t j = 0; j < m_pGrid[i].aryObject.size(); ++j)
                RtObjectManager()->DelObject(m_pGrid[i].aryObject[j]);

    RtSceneBlockTerrain::OnResourceFree();
    OnChangeBorderline(false);

    for (int i = 0; i < g_iSceneTileCntX * g_iSceneTileCntY; ++i)
    {
        if (m_pTileMapIdx[i].grassVertex)
            DEL_ARRAY(m_pTileMapIdx[i].grassVertex);
    }

    if (m_texItemSM)
        RtGetRender()->GetTextMgr()->ReleaseTexture(m_texItemSM);

    for (int i = 0; i < 4; ++i)
        if (m_texMinmap[i])
            RtGetRender()->GetTextMgr()->ReleaseTexture(m_texMinmap[i]);

    return true;
}

bool RtSceneBlock::NewBlock(const char* szBlockFileName, float fOffsetX, float fOffsetY)
{
    m_vMin.Set(1e10);
    m_vMax.Set(-1e10);

    m_fOffsetX = fOffsetX;
    m_fOffsetY = fOffsetY;
    m_dwVersion = 0x0101;

    // Geometry

    //m_szFileName = szBlockFileName;
    if (!OnResourceAllocate())
    {
        return false;
    }

    int x, y, iIdx;
    for (y=0; y<=g_iSceneTileCntY; y++)
    {
        for (x=0; x<=g_iSceneTileCntX; x++)
        {
            m_pPositions[(g_iSceneTileCntX+1)*y+x].cHeightMap = 128;
            m_pPositions[(g_iSceneTileCntX+1)*y+x].cTerrainHMap = 128;
        }
    }

    for (y=0; y<g_iSceneTileCntY; y++)
    {
        for (x=0; x<g_iSceneTileCntX; x++)
        {
            iIdx = y*g_iSceneTileCntX+x;
            m_pTileMapIdx[iIdx].mtl[0] = 0;
            m_pTileMapIdx[iIdx].mtl[1] = 0;
            m_pTileMapIdx[iIdx].mtl[2] = 0;
            m_pTileMapIdx[iIdx].mtl[3] = 0;
            g_pScene->GetSortTileMapIdx(m_pTileMapIdx[iIdx]);
        }
    }

    if (!OnLoadSceneComplete())
    {
        return false;
    }
    for (y=0; y<g_iSceneTileCntY; y++)
    {
        for (x=0; x<g_iSceneTileCntX; x++)
        {
            ComputeVertexColor(x, y);
        }
    }

    OnChangeBorderline(true);
    SetModifyFlag(true);

    char* t = (char*)strrchr(szBlockFileName, '\\');
    if (!t)
        t = (char*)szBlockFileName;
    rt2_strncpy(m_szBlockName, t, sizeof(m_szBlockName));
    t = strrchr(m_szBlockName, '.');
    if (t)
        *t = 0;

    return true;
}

bool RtSceneBlock::OnSerializeChunk(RtArchive* pFile, DWORD dwType, DWORD dwSize)
{
    if (dwType==0x03) // 物件
    {
        return OnSerializeBlockObject(pFile);
    }
    if (RtSceneBlockTerrain::OnSerializeChunk(pFile, dwType, dwSize))
    {
        switch (dwType)
        {
        case 0x02: // Block Terrain
            OnLoadSceneComplete();
            break;
        }
        return true;
    }
    return false;
}


bool RtSceneBlock::OnLoadSceneComplete()
{
    int x, y, v1, v2, v3, v4, iIdx;
    RtsSVertex* pVertex;

    UpdateVertexPositionByHeightmap();
    UpdateVertexPositionByTerrainHeightmap();
    UpdateVertexPositionByWaterHeightmap();

    pVertex = m_pVertices;
    for (y=0; y<g_iSceneTileCntY; y++)
    {
        for (x=0; x<g_iSceneTileCntX; x++)
        {
            iIdx = (y*(g_iSceneTileCntX+1))+x;

            pVertex[0].pPos = m_pPositions+iIdx;
            pVertex[1].pPos = m_pPositions+iIdx+1;
            pVertex[2].pPos = m_pPositions+iIdx+(g_iSceneTileCntX+1);
            pVertex[3].pPos = m_pPositions+iIdx+(g_iSceneTileCntX+1)+1;
            g_pScene->GetSortTileMapIdx(m_pTileMapIdx[y*g_iSceneTileCntX+x]);

            pVertex += 4;
        }
    }

    iIdx = 0;
    for (y=0; y<g_iSceneTileCntY; y++)
    {
        for (x=0; x<g_iSceneTileCntX; x++)
        {
            v1 = (y*g_iSceneTileCntX+x)*4;
            v2 = v1+1;
            v3 = v2+1;
            v4 = v3+1;
            // face 0
            m_pIndices[iIdx++].wIdx = v1; // 1
            m_pIndices[iIdx++].wIdx = v2; // 2
            m_pIndices[iIdx++].wIdx = v3; // 3
            // face 1
            m_pIndices[iIdx++].wIdx = v2; // 2
            m_pIndices[iIdx++].wIdx = v4; // 4
            m_pIndices[iIdx++].wIdx = v3; // 3
        }
    }

    // 创建Grid
    RtsSGrid* pGrid;
    for (v2=0; v2<g_iSceneGridCntY; v2++)
    {
        for (v1=0; v1<g_iSceneGridCntX; v1++)
        {
            pGrid = m_pGrid+(v2*g_iSceneGridCntX+v1);
            pGrid->pBlock = this;
            pGrid->x = v1*RTS_BLOCK_GRID_SIZE;
            pGrid->y = v2*RTS_BLOCK_GRID_SIZE;
            //pGrid->iObjectNum = 0;
            //pGrid->ppObject = NULL;
            pGrid->vMin.x = m_fOffsetX + pGrid->x*g_fSceneTileWidth;
            pGrid->vMin.y = m_fOffsetY + pGrid->y*g_fSceneTileWidth;
            pGrid->vMin.z = 0*g_fSceneHeightStep;
            pGrid->vMax.x = m_fOffsetX + (pGrid->x+RTS_BLOCK_GRID_SIZE)*g_fSceneTileWidth;
            pGrid->vMax.y = m_fOffsetY + (pGrid->y+RTS_BLOCK_GRID_SIZE)*g_fSceneTileWidth;
            pGrid->vMax.z = 255*g_fSceneHeightStep;
            pGrid->bObjLoad = false;
        }
    }

    m_vMin.x = m_fOffsetX;
    m_vMin.y = m_fOffsetY;
    m_vMin.z = 0*g_fSceneHeightStep;
    m_vMax.x = m_fOffsetX + g_iSceneTileCntX*g_fSceneTileWidth;
    m_vMax.y = m_fOffsetY + g_iSceneTileCntY*g_fSceneTileWidth;
    m_vMax.z = 255*g_fSceneHeightStep;

    // 更新网格
    UpdateGrid(0, 0, g_iSceneTileCntX, g_iSceneTileCntY);

    return true;
}

bool RtSceneBlock::OnSerializeBlockObject(RtArchive* pfile)
{
    RTASSERT(pfile && pfile->IsLoading());
    RtArchive& _file = (*pfile);

    RtsObject* _obj = NULL;
    int objNum = 0;
    _file<<objNum;

    for (int i = 0; i < objNum; ++i)
    {
        _obj = (RtsObject*)_file.ReadObject(NULL);

        if (!_obj->DataVaild())
        {
            RtCoreLog().Warn("block : %s, has null object\n", pfile->m_sPathFileName);
            continue;
        }

        if (RtObjectManager()->SetObject(_obj))
            _obj->ResetPosition();
        else
            DEL_ONE(_obj);
    }

    RtsSGrid* _gridEnd = m_pGrid + m_iGridSize;
    objectId _objId;
    size_t _realNum = 0;

    for (RtsSGrid* _grid = m_pGrid; _grid < _gridEnd; ++_grid)
    {
        _file<<objNum;

        if (objNum <= 0)
            continue;

        _grid->aryObject.alloc(objNum);
        _realNum = 0;

        for (int i = 0; i < objNum; ++i)
        {
            Compatible_Serialize_ObjId(_file, _objId);
            if (RtObjectManager()->GetObject(_objId) && RtObjectManager()->GetObject(_objId)->DataVaild())
                _grid->aryObject[_realNum++] = _objId;
        }

        if (_realNum != objNum)
            _grid->aryObject.resize(_realNum);
    }

    SetModifyFlag(true);
    
    return true;
}

bool RtSceneBlock::LoadBlock(const char* szBlockFileName)
{
    m_fOffsetX = m_iBlockPosX * g_fSceneTileWidth * g_iSceneTileCntX;
    m_fOffsetY = m_iBlockPosY * g_fSceneTileWidth * g_iSceneTileCntY;
 
    if (!RtSceneBlockTerrain::LoadBlock(szBlockFileName))
        return false;

    char szPathSM[MAX_PATH];
    char* t = (char*)strrchr(szBlockFileName, '\\');
    if (!t)
        t = (char*)strrchr(szBlockFileName, '/');
    if (!t)
        t = (char*)szBlockFileName;
    else
        ++t;
    
    rt2_strcpy(szPathSM, t);
    t = strrchr(szPathSM, '.');
    if (t)
        *t = 0;

    RTASSERT(m_texItemSM == 0);
    m_texItemSM = RtGetRender()->GetTextMgr()->CreateTextureFromFile(
        szPathSM,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        1);

    int _size = (int)strlen(szBlockFileName);
    t = (char*)szBlockFileName + _size - 1;
    
    while (true)
    {
        if ((*t) == '\\' || (*t) == '/')
        {
            ++t;
            break;
        }

        if (--t <= szBlockFileName)
            break;
    }

    rt2_strncpy(m_szBlockName, t, sizeof(m_szBlockName));
    t = strrchr(m_szBlockName, '.');
    if (t)
        *t = 0;

    char minmapName[MAX_PATH];

    for (int i = 0; i < 4; ++i)
    {
        rt2_sprintf(minmapName, "scene\\minimap\\%s%d.dds", m_szBlockName, i);
        m_texMinmap[i] = RtGetRender()->GetTextMgr()->CreateTextureFromFile(
            minmapName,
            D3DX_DEFAULT,
            D3DX_DEFAULT,
            1);
    }

    return true;
}

void RtSceneBlock::FixGridObjectNum()
{
    // done
    // RTASSERT(false);
   /* int i, j, iNull, s, d;
    RtsSGrid* pGrid;
    pGrid = m_pGrid;
    iNull = 0;
    for (i=0; i<m_iGridSize; i++, pGrid++)
    {
        s = d = 0;
        for (j=0; j<pGrid->iObjectNum; j++, s++)
        {
            if (!pGrid->ppObject[j])
            {
                iNull ++;
            }else
            {
                if (d!=s)
                {
                    pGrid->ppObject[d] = pGrid->ppObject[s];
                }
                d ++;
            }
        }
        if (pGrid->iObjectNum!=d)
        {
            for (j=d; j<pGrid->iObjectNum; j++)
            {
                if (pGrid->ppObject[j])
                {
                    pGrid->ppObject[j].Unref();
                }
            }
        }
        pGrid->iObjectNum = d;
    }
    if (iNull) RtCoreLog().Info("RtSceneBlock::FixGridObjectNum %d\n", iNull);*/
}

bool RtSceneBlock::SaveBlock(const char* szBlockFileName)
{
    RtArchive* _pfile = RtCoreFile().CreateFileWriter(NULL, szBlockFileName);

    if (!_pfile)
    {
        RtCoreLog().Error("RtSceneBlock: 保存场景块[%s]失败.\n", szBlockFileName);
        return false;
    }

    RtArchive& _file = *_pfile;

    DWORD dwFileHeader = RT_SCENE_BLOCK_HEADER;
    _file<<dwFileHeader;
    _file<<m_dwVersion;        

    DWORD dwBlockType = 0;
    DWORD dwBlockSize = 0;
    DWORD dwPos = 0;
    DWORD dwSizePos = 0;

    dwBlockType = 0x01;
    _file<<dwBlockType;
    dwSizePos = _pfile->Tell();
    _file<<dwBlockSize;
    OnSerializeBlockInfo(_pfile);

    dwPos = _file.Tell();
    dwBlockSize = dwPos - dwSizePos - 4;
    _file.Seek(dwSizePos, RtArchive::SeekBegin);
    _file.Serialize(&dwBlockSize, sizeof(DWORD));
    _file.Seek(dwPos, RtArchive::SeekBegin);

    dwBlockType = 0x02;
    _file<<dwBlockType;
    dwSizePos = _pfile->Tell();
    _file<<dwBlockSize;
    RtSceneBlockTerrain::OnSerializeBlockTerrain(_pfile);
    dwPos = _file.Tell();
    dwBlockSize = dwPos - dwSizePos - 4;
    _file.Seek(dwSizePos, RtArchive::SeekBegin);
    _file.Serialize(&dwBlockSize, sizeof(DWORD));
    _file.Seek(dwPos, RtArchive::SeekBegin);

    dwBlockType = 0x03;
    _file<<dwBlockType;
    dwSizePos = _pfile->Tell();
    _file<<dwBlockSize;

    FixGridObjectNum(); 

    RtsSGrid* _gridEnd = m_pGrid + m_iGridSize;
    RtsObject* _obj = NULL;
    set<RtsObject*> setObj;

    for (RtsSGrid* _grid = m_pGrid; _grid < _gridEnd; ++_grid)
    {
        for (size_t i = 0; i < _grid->aryObject.size(); ++i)
        {
            _obj = RtObjectManager()->GetObject(_grid->aryObject[i]);
            
            if (_obj)
                setObj.insert(_obj);
        }
    }

    size_t objNum = setObj.size();
    _file<<objNum;

    for (set<RtsObject*>::iterator i = setObj.begin(); i != setObj.end(); ++i)
    {
        _obj = (*i);
        _file.WriteObject(_obj);
    }

    for (RtsSGrid* _grid = m_pGrid; _grid < _gridEnd; ++_grid)
    {
        size_t objNum = _grid->aryObject.size();

        _file<<objNum;

        for (int i = 0; i < _grid->aryObject.size(); ++i)
            Compatible_Serialize_ObjId(_file, _grid->aryObject[i]);
    }

    dwPos = _file.Tell();
    dwBlockSize = dwPos - dwSizePos - 4;
    _file.Seek(dwSizePos, RtArchive::SeekBegin);
    _file.Serialize(&dwBlockSize, sizeof(DWORD));
    _file.Seek(dwPos, RtArchive::SeekBegin);

    dwBlockType = 0x00;
    _file<<dwBlockType;
    _file<<dwFileHeader;

    RtCoreFile().CloseFile(_pfile);
    SetModifyFlag(false);

    return true;
}

void RtSceneBlock::RecomputeObjectId()
{
    map<objectId, objectId> remapId;

    for (RtsSGrid* _grid = m_pGrid; _grid < m_pGrid + m_iGridSize; ++_grid)
    {
        for (size_t i = 0; i < _grid->aryObject.size(); ++i)
        {
            if (remapId.find(_grid->aryObject[i]) == remapId.end())
            {
                RtsObject* _obj = RtObjectManager()->RecomputeObjectId(_grid->aryObject[i]);

                if (_obj)
                    remapId[_grid->aryObject[i]] = _obj->Id();
            }
        }
    }

    for (RtsSGrid* _grid = m_pGrid; _grid < m_pGrid + m_iGridSize; ++_grid)
    {
        for (size_t i = 0; i < _grid->aryObject.size(); ++i)
        {
            map<objectId, objectId>::iterator _it = remapId.find(_grid->aryObject[i]);

            if (_it != remapId.end())
                _grid->aryObject[i] = _it->second;
        }
    }
}


bool RtSceneBlock::OnSerializeBlockTerrain(RtArchive* pFile, DWORD dwType)
{
    int x, y;
    DWORD dwSize;
    switch (dwType)
    {
    case 0x0001:
        // 高度图, 行走高度图, 水面高度图, 水面Alpha值
        dwSize = (g_iSceneTileCntX+1)*(g_iSceneTileCntY+1)*sizeof(BYTE)*4;
        pFile->Serialize(&dwSize, sizeof(DWORD));
        for (y=0; y<=g_iSceneTileCntY; y++)
        {
            for (x=0; x<=g_iSceneTileCntX; x++)
            {
                pFile->Serialize(&(m_pPositions[y*(g_iSceneTileCntX+1)+x].cHeightMap), sizeof(BYTE)*4);
            }
        }
        break;

    case 0x0002:
        // 场景顶点色
        dwSize = g_iSceneTileCntX*g_iSceneTileCntY*sizeof(DWORD);
        pFile->Serialize(&dwSize, sizeof(DWORD));
        for (y=0; y<g_iSceneTileCntY; y++)
        {
            for (x=0; x<g_iSceneTileCntX; x++)
            {
                pFile->Serialize(&(m_pPositions[y*(g_iSceneTileCntX+1)+x].c), sizeof(DWORD));
            }
        }
        break;

    case 0x0003:
        // 场景行走地表颜色
        dwSize = g_iSceneTileCntX*g_iSceneTileCntY*sizeof(DWORD);
        pFile->Serialize(&dwSize, sizeof(DWORD));
        for (y=0; y<g_iSceneTileCntY; y++)
        {
            for (x=0; x<g_iSceneTileCntX; x++)
            {
                pFile->Serialize(&(m_pPositions[y*(g_iSceneTileCntX+1)+x].dwTerrainColor), sizeof(DWORD));
            }
        }
        break;

    case 0x0004:
        // 场景贴图
        dwSize = g_iSceneTileCntX*g_iSceneTileCntY*sizeof(BYTE)*4;
        pFile->Serialize(&dwSize, sizeof(DWORD));
        for (y=0; y<g_iSceneTileCntY; y++)
        {
            for (x=0; x<g_iSceneTileCntX; x++)
            {
                pFile->Serialize(m_pTileMapIdx[y*g_iSceneTileCntX+x].mtl, sizeof(BYTE)*4);
            }
        }
        break;

    case 0x0005:
        // 场景水面的属性
        dwSize = g_iSceneTileCntX*g_iSceneTileCntY*sizeof(BYTE);
        pFile->Serialize(&dwSize, sizeof(DWORD));
        for (y=0; y<g_iSceneTileCntY; y++)
        {
            for (x=0; x<g_iSceneTileCntX; x++)
            {
                pFile->Serialize(&(m_pTileMapIdx[y*g_iSceneTileCntX+x].cWaterType), sizeof(BYTE));
            }
        }
        break;

        // grass
    case 0x0008 :
        {  
            RTASSERT(pFile->IsLoading());

            dwSize = g_iSceneTileCntX*g_iSceneTileCntY*sizeof(BYTE)*20;
            pFile->Serialize(&dwSize, sizeof(DWORD));

            for (y=0; y<g_iSceneTileCntY; y++)
            {
                for (x=0; x<g_iSceneTileCntX; x++)
                {
                    RtsSTileMap* tile = &m_pTileMapIdx[y * g_iSceneTileCntX + x];

                    pFile->Serialize(&tile->grassId, sizeof(WORD));
                    pFile->Serialize(&tile->minH, sizeof(float));
                    pFile->Serialize(&tile->maxH, sizeof(float));
                    pFile->Serialize(&tile->aspectW, sizeof(float));
                    pFile->Serialize(&tile->grassDensity, sizeof(WORD));

                }
            }

            break;
        }

        // grass new
    case 0x0010 :
        {  
            dwSize = g_iSceneTileCntX*g_iSceneTileCntY*sizeof(BYTE)*20;
            int pos = pFile->Tell();
            pFile->Serialize(&dwSize, sizeof(DWORD));

            for (y=0; y<g_iSceneTileCntY; y++)
            {
                for (x=0; x<g_iSceneTileCntX; x++)
                {
                    RtsSTileMap* tile = &m_pTileMapIdx[y * g_iSceneTileCntX + x];

                    pFile->Serialize(&tile->grassId,      sizeof(WORD));
                    pFile->Serialize(&tile->minH,         sizeof(float));
                    pFile->Serialize(&tile->maxH,         sizeof(float));
                    pFile->Serialize(&tile->aspectW,      sizeof(float));
                    pFile->Serialize(&tile->grassDensity, sizeof(WORD));

                    if (tile->grassDensity > 0)
                    {
                        if (pFile->IsLoading())
                        {
                            RTASSERT(tile->grassVertex == NULL);
                            DWORD _Size = 0;
                            pFile->Serialize(&_Size, sizeof(DWORD));
                            if (_Size == tile->grassDensity * sizeof(RtgVertexGrass) * 8)
                                tile->grassVertex = RT_NEW RtgVertexGrass[tile->grassDensity * 8]; 
                        }
                        else
                        {
                            DWORD _Size = 0;
                            if (tile->grassVertex)
                                _Size = tile->grassDensity * sizeof(RtgVertexGrass) * 8;
                            pFile->Serialize(&_Size, sizeof(DWORD));
                        }

                        if (tile->grassVertex)
                            pFile->Serialize(tile->grassVertex, tile->grassDensity * sizeof(RtgVertexGrass) * 8);
                    }
                }
            }

            if (pFile->IsStoring())
            {
                int pos2 = pFile->Tell();
                dwSize = pos2 - pos - sizeof(DWORD);
                pFile->Seek(pos, RtArchive::SeekBegin);
                pFile->Serialize(&dwSize, sizeof(DWORD));
                pFile->Seek(pos2, RtArchive::SeekBegin);
            }


            break;
        }

    case 0x0009 :
        dwSize = g_iSceneTileCntX*g_iSceneTileCntY*4*sizeof(RtgVertex2);
        pFile->Serialize(&dwSize, sizeof(DWORD));
        for (int i = 0; i < g_iSceneTileCntX * g_iSceneTileCntY * 4; ++i)
            pFile->Serialize(&m_pVertices[i].uvSM, sizeof(RtgVertex2));
        break;

    default:
        return RtSceneBlockTerrain::OnSerializeBlockTerrain(pFile, dwType);
    }
    return true;
}

// bAdd 为 True 的时候为添加，否则为删除
void RtSceneBlock::OnChangeBorderline(bool bAdd)
{
    if (bAdd)
    {
        // 要把左边和上面的指向自己
        // 并且把自己左边和下边的指向别人

        int i, iT1, iT2, iS1, iS2;
        RtSceneBlock* pBlock;

        // 自己(右边)
        pBlock = g_pScene->GetBlock(m_iBlockPosX+1, m_iBlockPosY);
        if (pBlock)
        {
            for (i=0; i<g_iSceneTileCntY; i++)
            {
                iT1 = (i*g_iSceneTileCntX+g_iSceneTileCntX-1)*4+1;
                iS1 = (i*g_iSceneTileCntX)*4+0;
                iT2 = (i*g_iSceneTileCntX+g_iSceneTileCntX-1)*4+3;
                iS2 = (i*g_iSceneTileCntX)*4+2;
                *(m_pVertices[iT1].pPos) = *(pBlock->m_pVertices[iS1].pPos);
                *(m_pVertices[iT2].pPos) = *(pBlock->m_pVertices[iS2].pPos);
                m_pVertices[iT1].pPos = pBlock->m_pVertices[iS1].pPos;
                m_pVertices[iT2].pPos = pBlock->m_pVertices[iS2].pPos;
            }
        }

        // 自己(下边)
        pBlock = g_pScene->GetBlock(m_iBlockPosX, m_iBlockPosY+1);
        if (pBlock)
        {
            for (i=0; i<g_iSceneTileCntX; i++)
            {
                iT1 = ((g_iSceneTileCntY-1)*g_iSceneTileCntX+i)*4+2;
                iS1 = i*4+0;
                iT2 = ((g_iSceneTileCntY-1)*g_iSceneTileCntX+i)*4+3;
                iS2 = i*4+1;
                *(m_pVertices[iT1].pPos) = *(pBlock->m_pVertices[iS1].pPos);
                *(m_pVertices[iT2].pPos) = *(pBlock->m_pVertices[iS2].pPos);
                m_pVertices[iT1].pPos = pBlock->m_pVertices[iS1].pPos;
                m_pVertices[iT2].pPos = pBlock->m_pVertices[iS2].pPos;
            }
        }

        // 自己(右下)
        pBlock = g_pScene->GetBlock(m_iBlockPosX+1, m_iBlockPosY+1);
        if (pBlock)
        {
            iT1 = (g_iSceneTileCntY*g_iSceneTileCntX-1)*4+3;
            iS1 = 0;
            *(m_pVertices[iT1].pPos) = *(pBlock->m_pVertices[iS1].pPos);
            m_pVertices[iT1].pPos = pBlock->m_pVertices[iS1].pPos;
        }

        // 左边
        pBlock = g_pScene->GetBlock(m_iBlockPosX-1, m_iBlockPosY);
        if (pBlock)
        {
            for (i=0; i<g_iSceneTileCntY; i++)
            {
                iT1 = (i*g_iSceneTileCntX+g_iSceneTileCntX-1)*4+1;
                iS1 = i*g_iSceneTileCntX*4+0;
                iT2 = (i*g_iSceneTileCntX+g_iSceneTileCntX-1)*4+3;
                iS2 = i*g_iSceneTileCntX*4+2;
                *(pBlock->m_pVertices[iT1].pPos) = *(m_pVertices[iS1].pPos);
                *(pBlock->m_pVertices[iT2].pPos) = *(m_pVertices[iS2].pPos);
                pBlock->m_pVertices[iT1].pPos = m_pVertices[iS1].pPos;
                pBlock->m_pVertices[iT2].pPos = m_pVertices[iS2].pPos;
            }
        }

        // 上边
        pBlock = g_pScene->GetBlock(m_iBlockPosX, m_iBlockPosY-1);
        if (pBlock)
        {
            for (i=0; i<g_iSceneTileCntX; i++)
            {
                iT1 = ((g_iSceneTileCntY-1)*g_iSceneTileCntX+i)*4+2;
                iS1 = i*4+0;
                iT2 = ((g_iSceneTileCntY-1)*g_iSceneTileCntX+i)*4+3;
                iS2 = i*4+1;
                *(pBlock->m_pVertices[iT1].pPos) = *(m_pVertices[iS1].pPos);
                *(pBlock->m_pVertices[iT2].pPos) = *(m_pVertices[iS2].pPos);
                pBlock->m_pVertices[iT1].pPos = m_pVertices[iS1].pPos;
                pBlock->m_pVertices[iT2].pPos = m_pVertices[iS2].pPos;
            }
        }

        // 左上
        pBlock = g_pScene->GetBlock(m_iBlockPosX-1, m_iBlockPosY-1);
        if (pBlock)
        {
            iT1 = (g_iSceneTileCntY*g_iSceneTileCntX-1)*4+3;
            iS1 = 0;
            *(pBlock->m_pVertices[iT1].pPos) = *(m_pVertices[iS1].pPos);
            pBlock->m_pVertices[iT1].pPos = m_pVertices[iS1].pPos;
        }

        int x, y;
        for (y=-1; y<=g_iSceneTileCntY; y++)
        {
            for (x=-1; x<=g_iSceneTileCntX; x++)
            {
                g_pScene->ComputeVertexNormal(m_iBlockPosX*g_iSceneTileCntX+x, m_iBlockPosY*g_iSceneTileCntY+y);
            }
        }
    }else
    {
        // 要把左边和上面指向自己的变成指向他们自己

        int i;
        RtSceneBlock* pBlock;

        // 左边
        pBlock = g_pScene->GetBlock(m_iBlockPosX-1, m_iBlockPosY);
        if (pBlock)
        {
            for (i=0; i<g_iSceneTileCntY; i++)
            {
                pBlock->m_pVertices[(i*g_iSceneTileCntX+g_iSceneTileCntX-1)*4+1].pPos = pBlock->m_pPositions+(i*(g_iSceneTileCntX+1))+g_iSceneTileCntY;
                pBlock->m_pVertices[(i*g_iSceneTileCntX+g_iSceneTileCntX-1)*4+3].pPos = pBlock->m_pPositions+((i+1)*(g_iSceneTileCntX+1))+g_iSceneTileCntY;
            }
        }

        // 上边
        pBlock = g_pScene->GetBlock(m_iBlockPosX, m_iBlockPosY-1);
        if (pBlock)
        {
            for (i=0; i<g_iSceneTileCntX; i++)
            {
                pBlock->m_pVertices[((g_iSceneTileCntY-1)*g_iSceneTileCntX+i)*4+2].pPos = pBlock->m_pPositions+(g_iSceneTileCntY*(g_iSceneTileCntX+1))+i;
                pBlock->m_pVertices[((g_iSceneTileCntY-1)*g_iSceneTileCntX+i)*4+3].pPos = pBlock->m_pPositions+(g_iSceneTileCntY*(g_iSceneTileCntX+1))+i+1;
            }
        }

        // 左上
        pBlock = g_pScene->GetBlock(m_iBlockPosX-1, m_iBlockPosY-1);
        if (pBlock)
        {
            pBlock->m_pVertices[(g_iSceneTileCntY*g_iSceneTileCntX-1)*4+3].pPos = pBlock->m_pPositions+((g_iSceneTileCntY+1)*(g_iSceneTileCntX+1))-1;
        }
    }
}

/*------------------------------------------------------------------------
The End.
------------------------------------------------------------------------*/
