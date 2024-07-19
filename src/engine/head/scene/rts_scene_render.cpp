
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_scene_render.cpp
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

class CRT_ActorInstance;

#include "graph/rt_graph.h"
#include "rt_scene.h"

RtsSceneRender::RtsSceneRender()
    : m_tileNodePool(RT_NEW RtsTileNode[2000]),
      m_tnPoolSize(2000),
      m_freeNodeNext(0),
      m_tileNumOfNodeMax(0),
      m_bUseSM(true),
      m_nProcessLevel(0),
      m_ib(NULL)
{
}

RtsSceneRender::~RtsSceneRender()
{
    if (m_tileNodePool)
        DEL_ARRAY(m_tileNodePool);
    Reset();
    RtgReleaseBuffer(m_ib);
}

bool RtsSceneRender::Init()
{
    RTASSERT(!m_ib);

    RtgSetCreateBufferDebugInfo();
    m_ib = RtGetRender()->GetBuffMgr()->CreateBuffer(
        Init_ib_Size * sizeof(WORD),
        D3DFMT_INDEX16,
        0,
        D3DPOOL_MANAGED,
        NULL);

    if (!m_ib)
        return false;

    WORD* ib = (WORD*)RtGetRender()->GetBuffMgr()->Lock(m_ib);

    if (ib)
    {
        for (int i = 0; i < Init_ib_Size / 6; ++i)
        {
            (*ib++) = i * 4 + 0;
            (*ib++) = i * 4 + 1;
            (*ib++) = i * 4 + 2;
            (*ib++) = i * 4 + 1;
            (*ib++) = i * 4 + 3;
            (*ib++) = i * 4 + 2;
        }

        RtGetRender()->GetBuffMgr()->Unlock(m_ib);
    }

    return true;

}

void RtsSceneRender::Reset()
{
    m_mapRenderBlock.clear();
    m_freeNodeNext = 0;
    m_tileNumOfNodeMax = 0;
    RtGetPref()->RenderTile_Num = 0;
}

WORD RtsSceneRender::RequireTileNode()
{
    if (m_freeNodeNext >= m_tnPoolSize)
        m_tileNodePool = (RtsTileNode*)realloc(m_tileNodePool, (m_tnPoolSize += 32) * sizeof(RtsTileNode));
    m_tileNodePool[m_freeNodeNext].Reset();
    return m_freeNodeNext++;
}

void RtsSceneRender::Insert(RtSceneBlock* pBlock, int tileId, RtgTextItem* texMap[])
{
    RenderBlockMap::iterator it = m_mapRenderBlock.find(pBlock);
    WORD tileRoot = _ErrorTileNode;

    if (it != m_mapRenderBlock.end())
    {
        tileRoot = it->second;
    }
    else
    {
        tileRoot = RequireTileNode();
        m_mapRenderBlock[pBlock] = tileRoot;
    }

    WORD tileCurr = tileRoot;

    RtsTileNode* nodeCurr = GetTileNode(tileCurr);
    RtsTileNode* nodeInst = NULL;

    for (int i = 0; i < 4 && texMap[i]; ++i)
    {
        RtgTextItem* texItem = texMap[i];

        if (nodeCurr->Leaf())
        {
            WORD newId = RequireTileNode();
            RtsTileNode* nodeNew = GetTileNode(newId);

            nodeNew->m_texItem  = texItem;
            nodeCurr->m_wChild = newId;
            nodeCurr = nodeNew;
        }
        else
        {
            WORD nodeId = nodeCurr->m_wChild;
            RtsTileNode* nodeParent = nodeCurr;
            RtsTileNode* nodePrev = NULL;

            while (nodeCurr = GetTileNode(nodeId))
            {
                if (nodeCurr->m_texItem == texItem)
                    break;
                nodePrev = nodeCurr;
                nodeId = nodeCurr->m_wNext;
            }

            if (!nodeCurr)
            {
                WORD newId = RequireTileNode();
                RtsTileNode* nodeNew = GetTileNode(newId);

                nodeNew->m_texItem = texItem;
                nodePrev->m_wNext = newId;
                nodeCurr = nodeNew;
            }
        }
    }

    pBlock->m_pTileMapIdx[tileId].wNext = nodeCurr->m_wTile;
    nodeCurr->m_wTile = tileId;
    ++nodeCurr->m_wTileNum;

    if (nodeCurr->m_wTileNum > m_tileNumOfNodeMax)
        m_tileNumOfNodeMax = nodeCurr->m_wTileNum;

    ++RtGetPref()->RenderTile_Num;
}

namespace {

    __forceinline void SetUV(BYTE* vb, RtScene* scene, RtSceneBlock* block, int tileId, int uvId, int vsiz)
    {
        if (uvId == 0xff)
        {
            float dx = 1.f / g_iSceneTileCntX;
            float dy = 1.f / g_iSceneTileCntY;

            float x = (float)(tileId % g_iSceneTileCntX) / (float)g_iSceneTileCntX;
            float y = (float)(g_iSceneTileCntY - tileId / g_iSceneTileCntX) / (float)g_iSceneTileCntY;

            ((RtgVertex2*)vb)->Set(x, y);
            vb += vsiz;
            ((RtgVertex2*)vb)->Set(x + dx, y);
            vb += vsiz;
            ((RtgVertex2*)vb)->Set(x, y - dy);
            vb += vsiz;
            ((RtgVertex2*)vb)->Set(x + dx, y - dy);
            vb += vsiz;

            return;
        }


		int iuv = 0;
		if ((uvId == 0 || block->m_pTileMapIdx[tileId].mapCnt == 1) && block->m_pTileMapIdx[tileId].uv2)
			iuv = 16;

		switch (uvId)
		{
		case 0:
			iuv += block->m_pTileMapIdx[tileId].uv1;
			break;
		case 1:
			iuv += block->m_pTileMapIdx[tileId].uv2;
			break;
		case 2:
			iuv += block->m_pTileMapIdx[tileId].uv3;
			break;
		case 3:
			iuv += block->m_pTileMapIdx[tileId].uv4;
			break;
		default :
			RTASSERT(false);
			break;
		}

        RtgVertex2* uv = scene->m_TexUVPattern[iuv].pUV;

        ((RtgVertex2*)vb)->Set(*uv++);
        vb += vsiz;
        ((RtgVertex2*)vb)->Set(*uv++);
        vb += vsiz;
        ((RtgVertex2*)vb)->Set(*uv++);
        vb += vsiz;
        ((RtgVertex2*)vb)->Set(*uv++);
    }
}

void RtsSceneRender::RenderNode_FFP(RtScene* scene, RtSceneBlock* block, RtsTileNode* node, int level, RtgShader* sh)
{
    RTASSERT(block);
    RTASSERT(node);
    RTASSERT(sh);
    RTASSERT(scene);

    if (node->m_wTileNum <= 0)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    RTGTextureOp opSave = sh->Shaders[sh->iNumStage].eColorOp;
    RTGTexArg argSave1 = sh->Shaders[sh->iNumStage].eColorArg1;
    RTGTexArg argSave2 = sh->Shaders[sh->iNumStage].eColorArg2;
    RTGTextureOp opSave2 = sh->Shaders[level].eColorOp;

    DWORD vSiz = sizeof(RtgTileVertexVNC) + sizeof(RtgVertex2) * (level + 1);
    DWORD vbId = 0; 
    RTGVertexFormat vFmt = (RTGVertexFormat)(RTGVF_V | RTGVF_N | RTGVF_C | (RTGVF_T1 << level));

    WORD uvMap[] = {0, 1, 2, 3};
    if (block->HasSM())
    {
        uvMap[level] = 0xff;
        sh->Shaders[level].eColorOp = RTGTOP_MODULATE;
    }

    RtgShareBufferItem vbItem;
    BYTE* vb = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(node->m_wTileNum * vSiz * 4, &vbItem);

    for (int i = 0, tileId = node->m_wTile; i < node->m_wTileNum; ++i)
    {
        RtsSVertex* tv = block->m_pVertices + tileId * 4;

        for (int j = 0; j <= level; ++j)
            SetUV(vb + sizeof(RtgTileVertexVNC) + sizeof(RtgVertex2) * j, scene, block, tileId, uvMap[j], vSiz);

        for (int j = 0; j < 4; ++j)
        {
            memcpy(vb, tv[j].pPos, sizeof(RtgTileVertexVNC));
            vb += vSiz;
        }

        tileId = block->m_pTileMapIdx[tileId].wNext;
    }

    dev->GetBuffMgr()->Unlock(&vbItem);
    dev->SetStream_vb(0, &vbItem, vSiz);

    sh->Shaders[sh->iNumStage].eColorOp = RTGTOP_MODULATE;
    sh->Shaders[sh->iNumStage].eColorArg1 = RTGTA_CURRENT;
    sh->Shaders[sh->iNumStage].eColorArg2 = RTGTA_DIFFUSE;
    sh->iNumStage++; 

    dev->SetVertexFormat(vFmt);
    dev->SetShader(sh);
    dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, node->m_wTileNum * 4, 0, node->m_wTileNum * 2);
    dev->RestoreShader(sh);

    sh->Shaders[level].eColorOp = opSave2;
    sh->iNumStage--; 
    sh->Shaders[sh->iNumStage].eColorOp = opSave;
    sh->Shaders[sh->iNumStage].eColorArg1 = argSave1;
    sh->Shaders[sh->iNumStage].eColorArg2 = argSave2;

}

void RtsSceneRender::RenderNode_PP(RtScene* scene, RtSceneBlock* block, RtsTileNode* node, int level, ID3DXEffect* eff)
{
    RTASSERT(block);
    RTASSERT(node);
    RTASSERT(scene);
    RTASSERT(eff);

    if (node->m_wTileNum <= 0)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    DWORD vSiz = sizeof(RtgTileVertexVNC) + sizeof(RtgVertex2) * (level + 1);
    DWORD vbId = 0; 
    RTGVertexFormat vFmt = (RTGVertexFormat)(RTGVF_V | RTGVF_N | RTGVF_C | (RTGVF_T1 << level));
    
    if (m_bUseSM != block->HasSM() || m_nProcessLevel != level)
    {
        m_bUseSM = block->HasSM();
        m_nProcessLevel = level;
        eff->SetBool("bUseSM", m_bUseSM);
        eff->SetInt("texMax", m_nProcessLevel);
        eff->CommitChanges();
    }

    WORD  uvMapSM[] = {0xff, 0, 1, 2, 3, 4};
    WORD  uvMapNM[] = {0, 1, 2, 3, 4};
    WORD* uvMap = uvMapNM;

    if (block->HasSM())
        uvMap = uvMapSM;

    RtgShareBufferItem vbItem;
    BYTE* vb = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(node->m_wTileNum * vSiz * 4, &vbItem);
        
    for (int i = 0, tileId = node->m_wTile; i < node->m_wTileNum; ++i)
    {
        RtsSVertex* tv = block->m_pVertices + tileId * 4;

        for (int j = 0; j <= level; ++j)
            SetUV(vb + sizeof(RtgTileVertexVNC) + sizeof(RtgVertex2) * j, scene, block, tileId, uvMap[j], vSiz);

        for (int j = 0; j < 4; ++j)
        {
            memcpy(vb, tv[j].pPos, sizeof(RtgTileVertexVNC));
            vb += vSiz;
        }

        tileId = block->m_pTileMapIdx[tileId].wNext;
    }

    dev->GetBuffMgr()->Unlock(&vbItem);
    dev->SetStream_vb(0, &vbItem, vSiz);
    dev->SetVertexFormat(vFmt);
    dev->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        0,
        0,
        node->m_wTileNum * 4,
        0,
        node->m_wTileNum * 2);
}

void RtsSceneRender::Prepare_ib(size_t _Size)
{
    RTASSERT(m_ib);

    if ((DWORD)_Size * sizeof(WORD) > m_ib->dwSize)
    {
        RtGetRender()->GetBuffMgr()->Resize(m_ib, (DWORD)_Size * sizeof(WORD));
    
        WORD* ib = (WORD*)RtGetRender()->GetBuffMgr()->Lock(m_ib);

        if (ib)
        {
            for (int i = 0; i < _Size / 6; ++i)
            {
                (*ib++) = i * 4 + 0;
                (*ib++) = i * 4 + 1;
                (*ib++) = i * 4 + 2;
                (*ib++) = i * 4 + 1;
                (*ib++) = i * 4 + 3;
                (*ib++) = i * 4 + 2;
            }

            RtGetRender()->GetBuffMgr()->Unlock(m_ib);
        }
    }
}

void RtsSceneRender::Render(RtScene* pScene)
{
	pScene->GetDevice()->ResetTextMat();

    if (pScene->SupportTerrainRender_PP())
        Render_PP(pScene);
	else
		Render_FFP(pScene);
}

void RtsSceneRender::Render_FFP(RtScene* pScene)
{
    RTASSERT(pScene);
    if (m_tileNumOfNodeMax <= 0)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    RtgShader sh;
    Prepare_ib(m_tileNumOfNodeMax * 6);
    dev->SetStream_ib(m_ib);

    sh.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
    sh.Shaders[0].eColorArg1 = RTGTA_TEXTURE;

    for (int i = 1; i < _MaxTileMapNum; ++i)
    {
        sh.Shaders[i].eColorOp = RTGTOP_BLENDTEXTUREALPHA;
        sh.Shaders[i].eColorArg1 = RTGTA_TEXTURE;
        sh.Shaders[i].eColorArg2 = RTGTA_CURRENT;
        sh.Shaders[i].eAlphaOp = RTGTOP_SELECTARG1;
        sh.Shaders[i].eAlphaArg1 = RTGTA_TEXTURE;
        sh.Shaders[i].eAlphaArg2 = RTGTA_CURRENT;
    }

    RtgMatrix16 matWorld;
    matWorld.Unit();
    dev->SetMatrix(RTGTS_WORLD, &matWorld);
    dev->SetDefaultMaterial();
    dev->SetRenderState(RTGRS_LIGHTING, TRUE);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_COLOR1);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, RTGMCS_MATERIAL);
    dev->SetTextureAddress(RTGTADD_CLAMP);

    for (RenderBlockMap::iterator it = m_mapRenderBlock.begin(); it != m_mapRenderBlock.end(); ++it)
    {
        RtSceneBlock* block = it->first;
        WORD nodeId = it->second;

        RtsTileNode* levelNode[] = { GetTileNode(GetTileNode(nodeId)->m_wChild), NULL, NULL, NULL, NULL, NULL };
        RtsTileNode* node = NULL;
        int _Level = 0;

        while (_Level >= 0)
        {
            if (node = levelNode[_Level])
            {
                sh.iNumStage = _Level + 1;
                sh.Shaders[_Level].SetTexture(node->m_texItem);
                RenderNode_FFP(pScene, block, node, _Level, &sh);
                levelNode[_Level++] = GetTileNode(node->m_wNext);
                levelNode[_Level] = GetTileNode(node->m_wChild);
            }
            else
            {
                --_Level;
            }
        } 
    }

    dev->SetTextureAddress(RTGTADD_WRAP);
}

void RtsSceneRender::Render_PP(RtScene* pScene)
{
    RTASSERT(pScene);
    if (m_tileNumOfNodeMax <= 0)
        return;

    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    ID3DXEffect* eff = dev->GetEffect(Rtg_Effect_Terrain);
    RtgShader sh;

    if (!eff)
        return;

    Prepare_ib(m_tileNumOfNodeMax * 6);
    dev->SetStream_ib(m_ib);

    RtgMatrix16 matWorld;
    matWorld.Unit();
    dev->SetMatrix(RTGTS_WORLD, &matWorld);
    dev->SetDefaultMaterial();
    dev->SetRenderState(RTGRS_LIGHTING, TRUE);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_COLOR1);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, RTGMCS_MATERIAL);
    dev->SetTextureAddress(RTGTADD_CLAMP);
    dev->SetShader(&sh);

    for (int i = 0; i < dev->m_Ability.iMaxMultiTextures; ++i)
        dev->SetTextureCoord(i, i);

    if (FAILED(eff->SetTechnique("tec")))
        return;

    UINT uPass = 0;
    m_bUseSM = true;
    m_nProcessLevel = 0;

    if (SUCCEEDED(eff->Begin(&uPass, 0)))
    {
        if (SUCCEEDED(eff->BeginPass(0)))
        {
            for (RenderBlockMap::iterator it = m_mapRenderBlock.begin(); it != m_mapRenderBlock.end(); ++it)
            {
                RtSceneBlock* block = it->first;
                WORD nodeId = it->second;

                RtsTileNode* levelNode[] = { GetTileNode(GetTileNode(nodeId)->m_wChild), NULL, NULL, NULL, NULL, NULL };
                RtsTileNode* node = NULL;
                int _Level = 0;

                while (_Level >= 0)
                {
                    if (node = levelNode[_Level])
                    {
                        dev->UseTexture(_Level, node->m_texItem);
                        RenderNode_PP(pScene, block, node, _Level, eff);
                        levelNode[_Level++] = GetTileNode(node->m_wNext);
                        levelNode[_Level] = GetTileNode(node->m_wChild);
                    }
                    else
                    {
                        --_Level;
                    }
                } 
            }
            eff->EndPass();
        }
        eff->End();
    }

    dev->SetTextureAddress(RTGTADD_WRAP);
    dev->RestoreShader(&sh);
}

void RtsSceneRender::RenderNormal(RtScene* pScene)
{
    RTASSERT(pScene);
    
    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    UINT tileNum = 0;

    for (RenderBlockMap::iterator it = m_mapRenderBlock.begin(); it != m_mapRenderBlock.end(); ++it)
    {
        RtSceneBlock* block = it->first;
        WORD nodeId = it->second;

        RtsTileNode* levelNode[] = { GetTileNode(GetTileNode(nodeId)->m_wChild), NULL, NULL, NULL, NULL, NULL };
        RtsTileNode* node = NULL;
        int _Level = 0;

        while (_Level >= 0)
        {
            if (node = levelNode[_Level])
            {
                tileNum += node->m_wTileNum; 
                levelNode[_Level++] = GetTileNode(node->m_wNext);
                levelNode[_Level] = GetTileNode(node->m_wChild);
            }
            else
                --_Level;
        }
    }

    if (!tileNum)
        return;

    RtgShareBufferItem vbItem;
    RtgVertex3* vb = (RtgVertex3*)dev->GetBuffMgr()->Lock_Shared_vb(tileNum * 4 * 2 * sizeof(RtgVertex3), &vbItem);

    for (RenderBlockMap::iterator it = m_mapRenderBlock.begin(); it != m_mapRenderBlock.end(); ++it)
    {
        RtSceneBlock* block = it->first;
        WORD nodeId = it->second;
        WORD tileId = 0;
        RtsTileNode* levelNode[] = { GetTileNode(GetTileNode(nodeId)->m_wChild), NULL, NULL, NULL, NULL, NULL };
        RtsTileNode* node = NULL;
        int _Level = 0;

        while (_Level >= 0)
        {
            if (node = levelNode[_Level])
            {
                for (int i = 0, tileId = node->m_wTile; i < node->m_wTileNum; ++i)
                {
                    RtsSVertex* tv = block->m_pVertices + tileId * 4;

                    for (int j = 0; j < 4; ++j)
                    {
                        vb->Set(tv[j].pPos->v);
                        ++vb;
                        vb->Set(tv[j].pPos->v + tv[j].pPos->n * 10.f);
                        ++vb;
                    }

                    tileId = block->m_pTileMapIdx[tileId].wNext;
                }

                levelNode[_Level++] = GetTileNode(node->m_wNext);
                levelNode[_Level] = GetTileNode(node->m_wChild);
            }
            else
                --_Level;
        }
    }

    dev->GetBuffMgr()->Unlock(&vbItem);
    
    DWORD blight = dev->GetRenderState(RTGRS_LIGHTING);

    dev->SetRenderState(RTGRS_LIGHTING, FALSE);
    dev->SetDefaultMaterial(1.f, 1.f, 1.f, 1.f);
    dev->SetShader(&dev->m_nullShader);
    dev->SetStream_vb(0, &vbItem, sizeof(RtgVertex3), D3DFVF_XYZ);
    dev->DrawPrimitive(
        D3DPT_LINELIST,
        0,
        tileNum * 4);
    dev->SetRenderState(RTGRS_LIGHTING, blight);
}