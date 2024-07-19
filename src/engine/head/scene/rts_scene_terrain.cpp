  
#include "core/rt2_core.h"
#include "rts_path_find.h"
#include "rts_scene_terrain.h"

RtsPathFind*        g_pPathFind = NULL;

RtSceneBlockTerrain::RtSceneBlockTerrain()
{
    m_bModifyFlag = false;
    m_dwVersion = 0;        // 场景版本
    m_iBlockPosX = m_iBlockPosY = 0;
}

RtSceneBlockTerrain::~RtSceneBlockTerrain()
{
    ClearPathFindInfo();
}

bool RtSceneBlockTerrain::LoadBlock(const char* blockFileName)
{
    DWORD _BlockType  = 0;
    DWORD _BlockSize  = 0;
    DWORD _FileHead   = 0;
    int   _CurFilePos = 0;

    RtArchive* _ar = RtCoreFile().CreateFileReader(blockFileName);
    
    if (!_ar)
    {
        RtCoreLog().Error("loading scene block : %s failed\n", blockFileName);
        return false;
    }

    (*_ar)<<_FileHead;

    if (_FileHead != RT_SCENE_BLOCK_HEADER)
    {
        RtCoreFile().CloseFile(_ar);
        RtCoreLog().Error("scene block : %s format error\n", blockFileName);
        return false;
    }

    (*_ar)<<m_dwVersion;  

    while (true)
    {
        (*_ar)<<_BlockType;
        
        if (_BlockType == 0) 
            break;
        
        (*_ar)<<_BlockSize;

        _CurFilePos = _ar->Tell();
        
        if (!OnSerializeChunk(_ar, _BlockType, _BlockSize))
        {
            RtCoreFile().CloseFile(_ar);
            return false;
        }
        _ar->Seek(_CurFilePos + _BlockSize, RtArchive::SeekBegin);
    }

    (*_ar)<<_FileHead;
    
    if (_FileHead != RT_SCENE_BLOCK_HEADER)
    {
        RtCoreFile().CloseFile(_ar);
        RtCoreLog().Error("scene block : %s format error\n", blockFileName);
        return false;
    }

    RtCoreFile().CloseFile(_ar);

    return true;
}

bool RtSceneBlockTerrain::OnSerializeChunk(RtArchive* pFile, DWORD dwType, DWORD dwSize)
{
    switch (dwType)
    {
    case 0x01: // Block Information
        if (!OnSerializeBlockInfo(pFile))       return false;
        if (!OnResourceAllocate())              return false;
        break;
    case 0x02: // Block Terrain
        if (!OnSerializeBlockTerrain(pFile))    return false;
        break;
    default:
#ifndef RT_SCENE_ONLY_LOAD_TERRAIN
        RtCoreLog().Error("RtSceneLoad: Skip unkonwn Block content[%d]\n", dwType);
#endif
        break;
    }
    return true;
}

bool RtSceneBlockTerrain::OnSerializeBlockInfo(RtArchive* pFile)
{
    return true;
}

bool RtSceneBlockTerrain::OnResourceAllocate()
{
    memset(m_pTileAttr, 0, sizeof(STileAttr)*g_iSceneTerrainCntX*g_iSceneTerrainCntY );
    if ( m_pTileAttr==0 )
    {
        RtCoreLog().Error("ERROR: Can't create resource.\n");
        return false;
    }
    return true;
}

bool RtSceneBlockTerrain::OnResourceFree()
{
    return true;
}

bool RtSceneBlockTerrain::OnSerializeBlockTerrain(RtArchive* pFile)
{
    DWORD dwType = 0x0000;
    if (pFile->IsStoring())
    {   // Storing
        dwType = 0x0001;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0002;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0003;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0004;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0005;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0006;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        /*dwType = 0x0008;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;*/

        dwType = 0x0009;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0010;
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
        if (!OnSerializeBlockTerrain(pFile, dwType)) return false;

        dwType = 0x0000; // 结束标记
        if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
    }else
    {   // Loading
        while (true)
        {
            if (pFile->Serialize(&dwType, sizeof(DWORD))!=sizeof(DWORD)) return false;
            if (dwType==0x0000)
            {
                break;
            }
            if (!OnSerializeBlockTerrain(pFile, dwType)) return false;
        }
    }
    return true;
}

bool RtSceneBlockTerrain::OnSerializeBlockTerrain(RtArchive* pFile, DWORD dwType)
{
    DWORD dwSize;
    switch (dwType)
    {
    case 0x0006: // 地表属性
        dwSize = sizeof(STileAttr)*g_iSceneTerrainCntX*g_iSceneTerrainCntY;
        pFile->Serialize(&dwSize, sizeof(DWORD));
        pFile->Serialize(m_pTileAttr, sizeof(STileAttr)*g_iSceneTerrainCntX*g_iSceneTerrainCntY);
        break;
    default:
        CHECK(pFile->IsLoading());
        pFile->Serialize(&dwSize, sizeof(DWORD));
        pFile->Seek(dwSize);
#ifndef RT_SCENE_ONLY_LOAD_TERRAIN
        RtCoreLog().Error("RtSceneLoad: Skip unkonown Terrain type[%d]\n", dwType);
#endif
        break;
    }
    return true;
}

bool RtSceneBlockTerrain::SetTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask)
{
    STileAttr* pAttr = m_pTileAttr + (iTerrainY*g_iSceneTerrainCntY + iTerrainX);
    if (pAttr && (pAttr->cAttr&cAttrMask)!=cAttrMask)
    {
        pAttr->cAttr |= cAttrMask;
        SetModifyFlag(true);
		return true;
    }
	return false;
}

bool RtSceneBlockTerrain::RemoveTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask)
{
    STileAttr* pAttr = m_pTileAttr + (iTerrainY*g_iSceneTerrainCntY + iTerrainX);
    if (pAttr && (pAttr->cAttr&cAttrMask)!=0)
    {
        pAttr->cAttr &= ~cAttrMask;
		SetModifyFlag(true);
		return true;
	}
	return false;
}

void RtSceneBlockTerrain::ClearPathFindInfo(bool bDelete)
{
    if (g_pPathFind==NULL) return;
    // 取得地表信息
    int x, y;
    STileAttr* pAttr = m_pTileAttr;
    RtsPathPos *pNode, *ppNode;
    for (y=0; y<g_iSceneTerrainCntY; y++)
    {
        for (x=0; x<g_iSceneTerrainCntX; x++)
        {
            if (pAttr->pContainer)
            {
                pNode = pAttr->pContainer->pNodes;
                while (pNode)
                {
                    ppNode = pNode->pNext;
                    pNode->pNext = NULL;
                    if (pNode->cType&PT_PATH)
                    {
                        if (bDelete)
                        {
							((RtsPathPath*)pNode)->ClearPath();
							g_pPathFind->m_poolPath.DestroyObj((RtsPathPath*)pNode);
                        }
                    }else
                    {
                        if (bDelete)
						{
                            g_pPathFind->m_poolPos.DestroyObj(pNode);
                        }
                    }
                    pNode = ppNode;
				}
                if (bDelete)
				{
                    g_pPathFind->m_poolContainer.DestroyObj(pAttr->pContainer);
                }
                pAttr->pContainer = NULL;
            }
            pAttr ++;
        }
    }
}

void RtSceneBlockTerrain::ClearPathFindIncreaseFlag()
{
    int x, y;
    STileAttr* pAttr = m_pTileAttr;
    for (y=0; y<g_iSceneTerrainCntY; y++)
    {
        for (x=0; x<g_iSceneTerrainCntX; x++)
        {
            pAttr->dis.wDepth = 0;
            pAttr->dis.wFlag = 0;
            pAttr ++;
        }
    }
}

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
