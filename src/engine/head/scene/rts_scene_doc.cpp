
#ifdef RT_SCENE_ONLY_LOAD_TERRAIN
#   include "core/rt2_core.h"
#   include "rts_path_find.h"
#   include "rts_scene_terrain.h"
#   include "rts_scene_doc.h"
#else
#   include "rt_scene.h"
#endif

#define _Scene_Block_Version_0 0x0100
#define _Scene_Block_Version_1 0x0101
#define _Scene_Block_Version_2 0x0102
#define _Scene_Block_Version_3 0x0103
#define _Scene_Block_Version_4 0x0104
#define _Scene_Block_Version_5 0x0105
#define _Scene_Block_Version _Scene_Block_Version_5

RtsSceneBlockMap& SceneBlockArray::operator [](size_t _index)
{
    RTASSERT(_index < _size);
    return _blockArray[_index];
}


RtsSceneDoc::RtsSceneDoc()
{
	CM_MEMPROTECTOR(m_szFilePath, _MAX_PATH)
	CM_MEMPROTECTOR(m_szMapFileName, _MAX_PATH)
	CM_MEMPROTECTOR(m_szMapSetName, 20)

	m_dwMapVersion = _Scene_Block_Version;     // 场景版本
    m_iBlockXCnt = 0;       // 场景块的X方向的个数
    m_iBlockYCnt = 0;       // 场景块的Y方向的个数
	m_iOldBlockXCnt = 0;
	m_iOldBlockYCnt = 0;
	m_bMergeBlock = false;
    m_iBlockCnt = 0;        // 场景块的个数
    m_iCenterPosX = -g_iSceneBlockOffsetX;
    m_iCenterPosY = -g_iSceneBlockOffsetY;
    m_szMapFileName[0] = 0;
    m_szMapSetName[0] = 0;
    m_szFilePath[0] = 0;
    m_iAreaRef = -1;
    m_pAreaDoc = NULL;
    m_pAreas = NULL;
    m_iAreaCount = 0;
    m_ppAreaStringBuffer = NULL;
    m_iAreaStringBufferCount = 0;
}

RtsSceneDoc::~RtsSceneDoc()
{
	CM_MEMUNPROTECTOR(m_szFilePath)
	CM_MEMUNPROTECTOR(m_szMapFileName)
	CM_MEMUNPROTECTOR(m_szMapSetName)

	DEL_ARRAY(m_pAreas);
    m_iAreaCount = 0;

    int i;
    for (i=0; i<m_iAreaStringBufferCount; i++)
    {
        DEL_ARRAY(m_ppAreaStringBuffer[i]);
    }
    DEL_ARRAY(m_ppAreaStringBuffer);
    m_iAreaStringBufferCount = 0;

#ifndef REGION_SERVER
    m_SceneBlockManager.DestroySceneBlockArray(m_blockMapArray);
#else
    DEL_ARRAY(m_blockMapArray._blockArray);
#endif
}

bool RtsSceneDoc::NewMap(const char* szMapFileName)
{
	m_dwMapVersion = _Scene_Block_Version;
    m_iBlockXCnt = 1;
    m_iBlockYCnt = 1;
    m_iBlockCnt = 1;
#ifndef REGION_SERVER
    m_blockMapArray = m_SceneBlockManager.CreateSceneBlockArray(1);
#else
    m_blockMapArray._blockArray = RT_NEW RtsSceneBlockMap[1];
    m_blockMapArray._size = 1;
#endif
    SetMapFileName(szMapFileName);
    char szName[20];
    rt2_sprintf(szName, "%s_000.rtb", m_szMapSetName);
    m_blockMapArray[0].SetFileName(szName);
    m_iCenterPosX = -g_iSceneBlockOffsetX;
    m_iCenterPosY = -g_iSceneBlockOffsetY;
#ifndef REGION_SERVER
    m_SceneBlockManager.InitSceneBlock(&m_blockMapArray[0], m_szFilePath, g_iSceneBlockOffsetX, g_iSceneBlockOffsetY);
#else
    m_blockMapArray[0].New(m_szFilePath, g_iSceneBlockOffsetX, g_iSceneBlockOffsetY);
#endif
    return true;
}

void RtsSceneDoc::SetMapFileName(const char* szMapFileName)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char szOldMapSet[20];
    rt2_strcpy(szOldMapSet, m_szMapSetName);
    rt2_strcpy(m_szMapFileName, szMapFileName);
    rtfSplitpath(szMapFileName, drive, dir, fname, ext);
    rtfMakepath(m_szFilePath, drive, dir, "", "");
    CHECK(strlen(fname)<20);
    rt2_strncpy(m_szMapSetName, fname, 19);
    strlwr(m_szMapSetName);

    if (strcmp(szOldMapSet,m_szMapSetName)!=0)
    {
        int il = (int)strlen(szOldMapSet);
        RtString szName;
        RtsSceneBlockMap* pBlockMap;
        for (int iy=0; iy<m_iBlockYCnt; iy++)
        {
            for (int ix=0; ix<m_iBlockXCnt; ix++)
            {
                pBlockMap = &m_blockMapArray[iy*m_iBlockXCnt+ix];
                if (pBlockMap->GetFileName()[0])
                {
                    rt2_strcpy(fname, m_szMapSetName);
                    rt2_strcat(fname, pBlockMap->GetFileName()+il);
                    pBlockMap->SetFileName(fname);
                    if (pBlockMap->m_pBlock)
                    {
                        pBlockMap->m_pBlock->SetModifyFlag(true);
                    }
                }
            }
        }
        /*
        RtSceneBlockTerrain* pBlock = GetNextBlockTerrain(NULL);
        while (pBlock)
        {
            pBlock->m_szFileName.Replace(szMapSet, m_szMapSetName);
            pBlock = GetNextBlockTerrain(pBlock);
        }
        */
    }
}
bool RtsSceneDoc::AddMapBlock(int iPosX, int iPosY,RtsSceneBlockMap* pMap)
{
	int x, y, xOffset, yOffset;
    SceneBlockArray _blockArray;
    if (iPosX<-1 || iPosY<-1 || iPosX>m_iBlockXCnt || iPosY>m_iBlockYCnt) return false;
    if (iPosX==-1 || iPosY==-1 || iPosX==m_iBlockXCnt || iPosY==m_iBlockYCnt)
    { // 要修改大小，地图被扩大了
        int iNewSizeX=m_iBlockXCnt, iNewSizeY=m_iBlockYCnt;
        if (iPosX==-1 || iPosX==m_iBlockXCnt) iNewSizeX ++;
        if (iPosY==-1 || iPosY==m_iBlockYCnt) iNewSizeY ++;
        if (iPosX==-1) xOffset = 1;
        else           xOffset = 0;
        if (iPosY==-1) yOffset = 1;
        else           yOffset = 0;
        m_iCenterPosX += xOffset;
        m_iCenterPosY += yOffset;

#ifndef REGION_SERVER
        _blockArray = m_SceneBlockManager.CreateSceneBlockArray(iNewSizeX*iNewSizeY);
#else
        _blockArray._blockArray = RT_NEW RtsSceneBlockMap[iNewSizeX*iNewSizeY];
        _blockArray._size = iNewSizeX*iNewSizeY;
#endif
        for (y=0; y<iNewSizeY; y++)
        {
            if (y<yOffset) continue;
            if ((y-yOffset)>=m_iBlockYCnt) continue;
            for (x=0; x<iNewSizeX; x++)
            {
                if (x<xOffset) continue;
                if ((x-xOffset)>=m_iBlockXCnt) continue;
                _blockArray[y*iNewSizeX+x] = m_blockMapArray[(y-yOffset)*m_iBlockXCnt+(x-xOffset)];
                m_blockMapArray[((y-yOffset)*m_iBlockXCnt+(x-xOffset))].m_pBlock = NULL;
            }
        }
        
#ifndef REGION_SERVER
        m_SceneBlockManager.DestroySceneBlockArray(m_blockMapArray);
#else
        DEL_ARRAY(m_blockMapArray._blockArray);
#endif
        m_blockMapArray = _blockArray;
        m_iBlockXCnt = iNewSizeX;
        m_iBlockYCnt = iNewSizeY;
        iPosX += xOffset;
        iPosY += yOffset;
    }

	m_iBlockCnt ++;
	m_blockMapArray[iPosY*m_iBlockXCnt+iPosX] = *pMap;
	m_iOldBlockXCnt = iPosX - m_iCenterPosX - m_iOldBlockXCnt;
	m_iOldBlockYCnt = iPosY - m_iCenterPosY - m_iOldBlockYCnt;

#ifndef REGION_SERVER
    m_SceneBlockManager.CreateSceneBlock(&m_blockMapArray[iPosY*m_iBlockXCnt+iPosX],
        m_szFilePath, iPosX - m_iCenterPosX , iPosY - m_iCenterPosY, true);
#else
    RtsSceneBlockMap* _block = &m_blockMapArray[iPosY*m_iBlockXCnt+iPosX];
    _block->SetLoadInfo(m_szFilePath, iPosX - m_iCenterPosX , iPosY - m_iCenterPosY);
    _block->Load(m_szFilePath, iPosX, m_iCenterPosX, iPosY, m_iCenterPosY);
#endif

    //获取新SCENE下的BLOCK名
	char szName[40];
	rt2_sprintf(szName, "%s_%03d.rtb", m_szMapSetName, m_iBlockCnt);
	int iCurCnt = 0, iSameName;
	do
	{
		iCurCnt ++;
		rt2_sprintf(szName, "%s_%03d.rtb", m_szMapSetName, iCurCnt);
		iSameName = FALSE;
		for (y=0; y<m_iBlockYCnt; y++)
		{
			for (x=0; x<m_iBlockXCnt; x++)
			{
				RtsSceneBlockMap* pBlock = &m_blockMapArray[y*m_iBlockXCnt+x];
				if (pBlock->GetFileName()[0]!=0)
				{
					if (strcmp(pBlock->GetFileName(), szName)==0)
					{
						iSameName = TRUE;
					}
				}
			}
		}
	}while (iSameName);
	m_blockMapArray[iPosY*m_iBlockXCnt+iPosX].SetFileName(szName);

    return true;

}
bool RtsSceneDoc::AddMapBlock(int iPosX, int iPosY)
{
    int x, y, xOffset, yOffset;
    SceneBlockArray _blockArray;
    if (iPosX<-1 || iPosY<-1 || iPosX>m_iBlockXCnt || iPosY>m_iBlockYCnt) return false;
    if (iPosX==-1 || iPosY==-1 || iPosX==m_iBlockXCnt || iPosY==m_iBlockYCnt)
    { // 要修改大小，地图被扩大了
        int iNewSizeX=m_iBlockXCnt, iNewSizeY=m_iBlockYCnt;
        if (iPosX==-1 || iPosX==m_iBlockXCnt) iNewSizeX ++;
        if (iPosY==-1 || iPosY==m_iBlockYCnt) iNewSizeY ++;
        if (iPosX==-1) xOffset = 1;
        else           xOffset = 0;
        if (iPosY==-1) yOffset = 1;
        else           yOffset = 0;
        m_iCenterPosX += xOffset;
        m_iCenterPosY += yOffset;

#ifndef REGION_SERVER
        _blockArray = m_SceneBlockManager.CreateSceneBlockArray(iNewSizeX*iNewSizeY);
#else
        _blockArray._blockArray =  RT_NEW RtsSceneBlockMap[iNewSizeX*iNewSizeY];
        _blockArray._size = iNewSizeX*iNewSizeY;
#endif
        for (y=0; y<iNewSizeY; y++)
        {
            if (y<yOffset) continue;
            if ((y-yOffset)>=m_iBlockYCnt) continue;
            for (x=0; x<iNewSizeX; x++)
            {
                if (x<xOffset) continue;
                if ((x-xOffset)>=m_iBlockXCnt) continue;
                _blockArray[y*iNewSizeX+x] = m_blockMapArray[(y-yOffset)*m_iBlockXCnt+(x-xOffset)];
                m_blockMapArray[((y-yOffset)*m_iBlockXCnt+(x-xOffset))].m_pBlock = NULL;
            }
        }

#ifndef REGION_SERVER
        m_SceneBlockManager.DestroySceneBlockArray(m_blockMapArray);
#else
        DEL_ARRAY(m_blockMapArray._blockArray);
#endif
        m_blockMapArray = _blockArray;
        m_iBlockXCnt = iNewSizeX;
        m_iBlockYCnt = iNewSizeY;
        iPosX += xOffset;
        iPosY += yOffset;
    }
    if (*(m_blockMapArray[iPosY*m_iBlockXCnt+iPosX].m_szFileName))
    {
        return false;
    }
   /* if (szBlockFileName)
    {
        m_pBlockMap[iPosY*m_iBlockXCnt+iPosX].SetFileName(szBlockFileName);
    }else
    {*/
        // 查找名字
    char szName[40];
    rt2_sprintf(szName, "%s_%03d.rtb", m_szMapSetName, m_iBlockCnt);
    int iCurCnt = 0, iSameName;
    do
    {
        iCurCnt ++;
        rt2_sprintf(szName, "%s_%03d.rtb", m_szMapSetName, iCurCnt);
        iSameName = FALSE;
        for (y=0; y<m_iBlockYCnt; y++)
        {
            for (x=0; x<m_iBlockXCnt; x++)
            {
                RtsSceneBlockMap* pBlock = &m_blockMapArray[y*m_iBlockXCnt+x];
                if (pBlock->GetFileName()[0]!=0)
                {
                    if (strcmp(pBlock->GetFileName(), szName)==0)
                    {
                        iSameName = TRUE;
                    }
                }
            }
        }
    }while (iSameName);
    m_blockMapArray[iPosY*m_iBlockXCnt+iPosX].SetFileName(szName);
    /*}*/
    m_iBlockCnt ++;

#ifndef REGION_SERVER
    m_SceneBlockManager.InitSceneBlock(&m_blockMapArray[iPosY*m_iBlockXCnt+iPosX], m_szFilePath, iPosX-m_iCenterPosX, iPosY-m_iCenterPosY);
#else
    m_blockMapArray[iPosY*m_iBlockXCnt+iPosX].New(m_szFilePath, iPosX-m_iCenterPosX, iPosY-m_iCenterPosY);
#endif

    return true;
}

// 根据文件名载入地图, bLoadAll为true的时候就把全部载入内存
bool RtsSceneDoc::LoadMap(const char* szMapFileName, bool bLoadAll)
{
    int i, j;
    // 打开文件
    DWORD dwFileHeader;
    SetMapFileName(szMapFileName);
    RtArchive* pFile = RtCoreFile().CreateFileReader(m_szMapFileName);
    if (pFile==NULL)
    {
        return false;
    }

    // 检查文件头
    *pFile << dwFileHeader;
    if (dwFileHeader!=RT_MAP_FILE_HEADER)
    {
        RtCoreFile().CloseFile(pFile);
        CHECK(dwFileHeader==RT_MAP_FILE_HEADER);
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // 文件基本信息
    if (!OnSerializeMapInfo(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    j = m_iBlockXCnt*m_iBlockYCnt;

#ifndef REGION_SERVER
    m_blockMapArray = m_SceneBlockManager.CreateSceneBlockArray(j);
#else
    m_blockMapArray._blockArray = RT_NEW RtsSceneBlockMap[j];
    m_blockMapArray._size = j;
#endif
    for (i=0; i<j; i++)
    {
        m_blockMapArray[i].m_pBlock = NULL;
    }

    if (!OnSerializeObjects(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    if (!OnSerializeBlockMapping(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    // 文件结束
    *pFile << dwFileHeader;
    if (dwFileHeader!=RT_MAP_FILE_HEADER)
    {
        CHECK(dwFileHeader==RT_MAP_FILE_HEADER);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    // 关闭文件
    RtCoreFile().CloseFile(pFile);

    m_dwMapVersion = _Scene_Block_Version;

    if (bLoadAll)
        LoadAllBlock();

    OnLoadMap(szMapFileName, bLoadAll);

    return true;
}

void RtsSceneDoc::LoadAllBlock()
{
    for (int y = 0; y < m_iBlockYCnt; ++y)
    {
        for (int x = 0; x < m_iBlockXCnt; ++x)
        {
            RtsSceneBlockMap* _block =  &m_blockMapArray[y * m_iBlockXCnt + x];

            if (_block->Check_CanLoad())
            {
#ifndef REGION_SERVER
                m_SceneBlockManager.CreateSceneBlock(
                    _block, m_szFilePath, x - GetCenterX(), y - GetCenterY(), true);
#else
                _block->SetLoadInfo(m_szFilePath, x - m_iCenterPosX , y - m_iCenterPosY);
                _block->Load(m_szFilePath, x, y, GetCenterX(), GetCenterY());
#endif
                RtCoreLog().Info("load scene block : %s\n", _block->m_szFileName);
            }
        }
    }

}


short RtsSceneDoc::GetRegionIDByBlock(int bX, int bY)
{
	RtsSceneBlockMap* pBlock = &m_blockMapArray[bY * m_iBlockXCnt + bX];
	if ( pBlock->m_pBlock == NULL)
		return 0;
	else
		return pBlock->GetRegionID();
}

#ifndef REGION_SERVER
void RtsSceneDoc::RemapObject()
{
    if (m_SceneBlockManager.LoadQueueSize() > 0)
        Sleep(100);

    vector<objectId> newMap;

    RtsSceneBlockMap* _blockMap = NULL;
    bool bNeedRelease = false;

    for (int y = 0; y < m_iBlockYCnt; ++y)
    {
        for (int x = 0; x < m_iBlockXCnt; ++x)
        {
            _blockMap = &m_blockMapArray[y * m_iBlockXCnt + x];
            
            bNeedRelease = false;

            if (_blockMap->Check_CanLoad())
            {
                m_SceneBlockManager.CreateSceneBlock(
                    _blockMap, 
                    m_szFilePath,
                    x - m_iCenterPosX,
                    y - m_iCenterPosY,
                    true);
                bNeedRelease = true;
            }

            RtSceneBlock* _block = ((RtSceneBlock*)_blockMap->m_pBlock);
            RtsSGrid* _gridEnd = _block->m_pGrid + _block->m_iGridSize;

            for (RtsSGrid* _grid = _block->m_pGrid; _grid < _gridEnd; ++_grid)
            {
                for (size_t i = 0; i < _grid->aryObject.size(); ++i)
                    newMap.push_back(_grid->aryObject[i]);
            }

            if (bNeedRelease)
                m_SceneBlockManager.ReleaseSceneBlock(_blockMap);
        }
    }


}

#endif

bool RtsSceneDoc::SaveMap(const char* szMapFileName)
{
    DWORD dwFileHeader = RT_MAP_FILE_HEADER;
    SetMapFileName(szMapFileName);

    RtArchive* pFile = RtCoreFile().CreateFileWriter(NULL, m_szMapFileName);
    
    if (!pFile)
        return false;
   
    (*pFile)<<dwFileHeader;

    if (!OnSerializeMapInfo(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    if (!OnSerializeObjects(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    if (!OnSerializeBlockMapping(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

	(*pFile)<<dwFileHeader; 
    RtCoreFile().CloseFile(pFile);

    RtsSceneBlockMap* pBlock = NULL;

    for (int j = 0; j < m_iBlockYCnt; ++j)
    {
        for (int i = 0; i < m_iBlockXCnt; ++i)
        {
            pBlock = &m_blockMapArray[j * m_iBlockXCnt + i];

            if (pBlock->GetFileName()[0]!=0 && pBlock->m_pBlock)
            {
                if (pBlock->m_pBlock->GetModifyFlag())
                {
                    if (!pBlock->Save(m_szFilePath))
                    {
                        RtCoreLog().Warn("RtsSceneDoc: Save map block [%s] error!\n", pBlock->GetFileName());
                    }
                }
            }
        }
    }

    return true;
}

bool RtsSceneDoc::SaveAllMap(const char* szMapFileName)
{
    DWORD dwFileHeader = RT_MAP_FILE_HEADER;
    SetMapFileName(szMapFileName);

    RtArchive* pFile = RtCoreFile().CreateFileWriter(NULL, m_szMapFileName);

    if (!pFile)
        return false;

    (*pFile)<<dwFileHeader;

    if (!OnSerializeMapInfo(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    if (!OnSerializeObjects(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    if (!OnSerializeBlockMapping(pFile))
    {
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    (*pFile)<<dwFileHeader; 
    RtCoreFile().CloseFile(pFile);

    RtsSceneBlockMap* pBlock = NULL;

    for (int j = 0; j < m_iBlockYCnt; ++j)
    {
        for (int i = 0; i < m_iBlockXCnt; ++i)
        {
            pBlock = &m_blockMapArray[j * m_iBlockXCnt + i];

            if (pBlock->GetFileName()[0]!=0 && pBlock->m_pBlock)
            {
                if (pBlock->m_pBlock->GetModifyFlag())
                {
                    if (!pBlock->Save(m_szFilePath))
                    {
                        RtCoreLog().Warn("RtsSceneDoc: Save map block [%s] error!\n", pBlock->GetFileName());
                    }
                }
            }
        }
    }

    return true;
}

void RtsSceneDoc::InitMap()
{
    m_szFilePath[0] = 0;
    m_szMapFileName[0] = 0; // 地图文件名
    m_szMapSetName[0] = 0;
    m_dwMapVersion = _Scene_Block_Version;     // 场景版本
    m_iBlockXCnt = 0;       // 场景块的X方向的个数
    m_iBlockYCnt = 0;       // 场景块的Y方向的个数
    m_iBlockCnt = 0;        // 场景块的个数
    m_iCenterPosX = -g_iSceneBlockOffsetX;
    m_iCenterPosY = -g_iSceneBlockOffsetY;
}

// 清除地图
void RtsSceneDoc::ClearMap()
{
    int i, j;
    RtsSceneBlockMap* pBlock;

    if (m_pAreaDoc)
    {
        m_pAreaDoc->m_iAreaRef --;
        m_pAreaDoc->m_listAreaUser.remove(this);
    }else
    {
        if (m_iAreaRef>1)
        {
            m_listAreaUser.remove(this);
            std::list<RtsSceneDoc*>::iterator itArea;
            for (itArea=m_listAreaUser.begin(); itArea!=m_listAreaUser.end(); itArea++)
            {
                (*itArea)->m_iAreaRef = -1;
                (*itArea)->m_pAreaDoc = m_listAreaUser.front();
            }
            m_listAreaUser.front()->m_iAreaRef = m_iAreaRef-1;
            m_listAreaUser.front()->m_pAreaDoc = NULL;
            m_listAreaUser.front()->m_listAreaUser = m_listAreaUser;
        }else
        {
            DEL_ARRAY(m_pAreas);
            m_iAreaCount = 0;
            for (i=0; i<m_iAreaStringBufferCount; i++)
            {
                DEL_ARRAY(m_ppAreaStringBuffer[i]);
            }
            DEL_ARRAY(m_ppAreaStringBuffer);
            m_iAreaStringBufferCount = 0;
        }
    }


    for (j=0; j<m_iBlockYCnt; j++)
    {
        for (i=0; i<m_iBlockXCnt; i++)
        {
            pBlock = &m_blockMapArray[j*m_iBlockXCnt+i];
            if (pBlock->m_pBlock)
            {
#ifndef REGION_SERVER
                m_SceneBlockManager.ReleaseSceneBlock(pBlock);
#else
                pBlock->Destroy();
#endif
            }
        }
    }
    

#ifndef REGION_SERVER
    m_SceneBlockManager.DestroySceneBlockArray(m_blockMapArray);
#else
    DEL_ARRAY(m_blockMapArray._blockArray);
#endif
    InitMap();
}

bool RtsSceneDoc::OnSerializeMapInfo(RtArchive* pFile)
{
    float fOffsetX=0.f, fOffsetY=0.f;
    *pFile << m_dwMapVersion;       // 文件版本
    *pFile << m_iBlockXCnt;         // 
    *pFile << m_iBlockYCnt;         // 
    *pFile << m_iBlockCnt;          // 
    *pFile << fOffsetX;             // 场景偏移
    *pFile << fOffsetY;             // 场景偏移
    *pFile << m_iCenterPosX;        //
    *pFile << m_iCenterPosY;        //

    RTASSERT(m_dwMapVersion <= _Scene_Block_Version);

    return true;
}

bool RtsSceneDoc::OnSerializeObjects(RtArchive* pFile)
{
    size_t _objNum = 0;
    *pFile<<_objNum;    
    return true;
}

bool RtsSceneDoc::OnSerializeBlockMapping(RtArchive* pFile)
{
    int i, j, c;
    RtsSceneBlockMap* pBlock;

    if (pFile->IsStoring())
    {
        c = 0;
        for (j=0; j<m_iBlockYCnt; j++)
        {
            for (i=0; i<m_iBlockXCnt; i++)
            {
                pBlock = &m_blockMapArray[j*m_iBlockXCnt+i];
                if (pBlock->m_szFileName[0])
                {
                    *pFile << i;
                    *pFile << j;
                    pBlock->Serialize(m_dwMapVersion, pFile);
                    c ++;
                }
            }
        }
        CHECK(c==m_iBlockCnt);
    }else
    {
        for (c=0; c<m_iBlockCnt; c++)
        {
            *pFile << i;
            *pFile << j;
            pBlock = &m_blockMapArray[j*m_iBlockXCnt+i];
            pBlock->Serialize(m_dwMapVersion, pFile);
        }
    }
    return true;
}



RtSceneBlockTerrain* RtsSceneDoc::GetTerrainBlock(int iTerrainX, int iTerrainY, int& iBx, int& iBy)
{
    OffsetTerrainToMap(iTerrainX, iTerrainY);
    if (iTerrainX<0 || iTerrainY<0) return NULL;
    iBx = iTerrainX/g_iSceneTerrainCntX;
    iBy = iTerrainY/g_iSceneTerrainCntY;
    if (iBx>=m_iBlockXCnt || iBy>=m_iBlockYCnt) return NULL;
    return m_blockMapArray[iBy*m_iBlockXCnt+iBx].m_pBlock;
}

const RtSceneBlockTerrain::STileAttr* RtsSceneDoc::GetTerrainAttr(int iTerrainX, int iTerrainY)
{
    return GetTerrainPathAttr(iTerrainX, iTerrainY);
}

bool RtsSceneDoc::SetTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask)
{
    int iBx, iBy;
    RtSceneBlockTerrain* pBlock = GetTerrainBlock(iTerrainX, iTerrainY, iBx, iBy);
    if (pBlock==NULL) return false;
    return pBlock->SetTerrainAttr(iTerrainX-(iBx-m_iCenterPosX)*g_iSceneTerrainCntX, iTerrainY-(iBy-m_iCenterPosY)*g_iSceneTerrainCntY, cAttrMask);
}

bool RtsSceneDoc::GetCanWalk(int iTerrainX, int iTerrainY)
{
	int iBx, iBy;
	RtSceneBlockTerrain* pBlock = GetTerrainBlock(iTerrainX, iTerrainY, iBx, iBy);
	if (pBlock==NULL) return false;
	return pBlock->CanWalk(iTerrainX-(iBx-m_iCenterPosX)*g_iSceneTerrainCntX, iTerrainY-(iBy-m_iCenterPosY)*g_iSceneTerrainCntY);

}

bool RtsSceneDoc::RemoveTerrainAttr(int iTerrainX, int iTerrainY, BYTE cAttrMask)
{
    int iBx, iBy;
    RtSceneBlockTerrain* pBlock = GetTerrainBlock(iTerrainX, iTerrainY, iBx, iBy);
    if (pBlock==NULL) return false;
    return pBlock->RemoveTerrainAttr(iTerrainX-(iBx-m_iCenterPosX)*g_iSceneTerrainCntX, iTerrainY-(iBy-m_iCenterPosY)*g_iSceneTerrainCntY, cAttrMask);
}

RtSceneBlockTerrain* RtsSceneDoc::GetNextBlockTerrain(RtSceneBlockTerrain* pCur)
{
    static int ix, iy;
    if (pCur==NULL)
    {
        ix = 0;
        iy = 0;
    }
    for (; iy<m_iBlockYCnt; iy++)
    {
        for (; ix<m_iBlockXCnt; ix++)
        {
            if (m_blockMapArray[iy*m_iBlockXCnt+ix].GetState() == Object_State_Ready)
            {
                return m_blockMapArray[iy*m_iBlockXCnt+(ix++)].m_pBlock;
            }
        }
        ix = 0;
    }
    return NULL;
}

void RtsSceneDoc::ClearPathFindInfo()
{
    int ix, iy;
    for (iy=0; iy<m_iBlockYCnt; iy++)
    {
        for (ix=0; ix<m_iBlockXCnt; ix++)
        {
            if (m_blockMapArray[iy*m_iBlockXCnt+ix].m_pBlock)
            {
                m_blockMapArray[iy*m_iBlockXCnt+ix].m_pBlock->ClearPathFindInfo();
            }
        }
    }
}

void RtsSceneDoc::ClearPathFindIncreaseFlag()
{
    int ix, iy;
    for (iy=0; iy<m_iBlockYCnt; iy++)
    {
        for (ix=0; ix<m_iBlockXCnt; ix++)
        {
            if (m_blockMapArray[iy*m_iBlockXCnt+ix].m_pBlock)
            {
                m_blockMapArray[iy*m_iBlockXCnt+ix].m_pBlock->ClearPathFindIncreaseFlag();
            }
        }
    }
}

RtSceneBlockTerrain::STileAttr* RtsSceneDoc::GetTerrainPathAttr(int iTerrainX, int iTerrainY)
{
    int iBx, iBy;
    OffsetTerrainToMap(iTerrainX, iTerrainY);
    if (iTerrainX<0 || iTerrainY<0) return NULL;
    iBx = iTerrainX/g_iSceneTerrainCntX;
    iBy = iTerrainY/g_iSceneTerrainCntY;
    if (iBx>=m_iBlockXCnt || iBy>=m_iBlockYCnt) return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy*m_iBlockXCnt+iBx];
    if (pBlockMap->m_pBlock==NULL) return NULL;
    return pBlockMap->m_pBlock->m_pTileAttr+ ((iTerrainY-iBy*g_iSceneTerrainCntY)*g_iSceneTerrainCntY + (iTerrainX-iBx*g_iSceneTerrainCntX));
}

void RtsSceneDoc::GetTileByPosFast(float fPosX, float fPosY, int& iTx, int& iTy)
{
    OffsetToMap(fPosX, fPosY);
    iTx = (int)(fPosX / g_fSceneTileWidth);
    iTy = (int)(fPosY / g_fSceneTileWidth);
    OffsetToWorld(iTx, iTy);
}

void RtsSceneDoc::GetPosByTileXY(int iTileX, int iTileY, float& fX, float& fY)
{
    OffsetToMap(iTileX, iTileY);
    fX = (iTileX+0.5) * g_fSceneTileWidth;
    fY = (iTileY+0.5) * g_fSceneTileWidth;
    OffsetToWorld(fX, fY);
}

void RtsSceneDoc::GetTerrainByPosFast(float fPosX, float fPosY, int& iTx, int& iTy)
{
    OffsetToMap(fPosX, fPosY);
    iTx = (int)(fPosX / g_fSceneTerrainTileWidth);
    iTy = (int)(fPosY / g_fSceneTerrainTileWidth);
    OffsetTerrainToWorld(iTx, iTy);
}

void RtsSceneDoc::GetTerrainByPosFast(float fPos[], float fDir[],short sRad[],int& iMinX, int& iMinY, int& iMaxX, int& iMaxY)
{
	float fRtX1,fRtY1,fRbX1,fRbY1,fLtX1,fLtY1,fLbX1,fLbY1;
	float fRtX,fRtY,fRbX,fRbY,fLtX,fLtY,fLbX,fLbY,x,y;
	x=fDir[0]-fPos[0];
	y=fDir[1]-fPos[1];

	float rad = acos(x/sqrt(x*x + y*y))+PI/2; 

	fLbX1=fLtX1=-sRad[0];
	fLbY1=fRbY1=-sRad[1];
	fRtX1=fRbX1=sRad[0];
	fRtY1=fLtY1=sRad[1];

	fLbX=fLbX1*cos(rad)-fLbY1*sin(rad);
	fLbY=fLbX1*sin(rad)+fLbY1*cos(rad);
	fLtX=fLtX1*cos(rad)-fLtY1*sin(rad);
	fLtY=fLtX1*sin(rad)+fLtY1*cos(rad);
	fRbX=fRbX1*cos(rad)-fRbY1*sin(rad);
	fRbY=fRbX1*sin(rad)+fRbY1*cos(rad);
	fRtX=fRtX1*cos(rad)-fRtY1*sin(rad);
	fRtY=fRtX1*sin(rad)+fRtY1*cos(rad);

	fLbX=fLbX<fLtX?fLbX:fLtX;
	fLbX=fLbX<fRtX?fLbX:fRtX;
	fLbX=fLbX<fRbX?fLbX:fRbX;
	fLbY=fLbY<fLtY?fLbY:fLtY;
	fLbY=fLbY<fRtY?fLbY:fRtY;
	fLbY=fLbY<fRbY?fLbY:fRbY;

	fRtX=fRtX>fLtX?fRtX:fLtX;
	fRtX=fRtX>fLbX?fRtX:fLbX;
	fRtX=fRtX>fRbX?fRtX:fRbX;
	fRtY=fRtY>fLtY?fRtY:fLtY;
	fRtY=fRtY>fLbY?fRtY:fLbY;
	fRtY=fRtY>fRbY?fRtY:fRbY;

	fLbX+=fPos[0];
	fLbY+=fPos[1];
	fRtX+=fPos[0];
	fRtY+=fPos[1];

	OffsetToMap(fLbX, fLbY);
	OffsetToMap(fRtX, fRtY);

	iMinX = (int)(fLbX / g_fSceneTerrainTileWidth);
	iMinY = (int)(fLbY / g_fSceneTerrainTileWidth);
	iMaxX = (int)(fRtX / g_fSceneTerrainTileWidth);
	iMaxY = (int)(fRtY / g_fSceneTerrainTileWidth);

	OffsetTerrainToWorld(iMinX, iMinY);
	OffsetTerrainToWorld(iMaxX, iMaxY);
}

void RtsSceneDoc::GetTerrainByPosFast(long lPos[], long lDir[],short sRad[],int& iMinX, int& iMinY, int& iMaxX, int& iMaxY)
{
	float fRtX1,fRtY1,fRbX1,fRbY1,fLtX1,fLtY1,fLbX1,fLbY1;
	float fRtX,fRtY,fRbX,fRbY,fLtX,fLtY,fLbX,fLbY,x,y;
	x=lDir[0]-lPos[0];
	y=lDir[1]-lPos[1];

	float rad = acos(x/sqrt(x*x + y*y))+PI/2; 

	fLbX1=fLtX1=-sRad[0];
	fLbY1=fRbY1=-sRad[1];
	fRtX1=fRbX1=sRad[0];
	fRtY1=fLtY1=sRad[1];

	fLbX=fLbX1*cos(rad)-fLbY1*sin(rad);
	fLbY=fLbX1*sin(rad)+fLbY1*cos(rad);
	fLtX=fLtX1*cos(rad)-fLtY1*sin(rad);
	fLtY=fLtX1*sin(rad)+fLtY1*cos(rad);
	fRbX=fRbX1*cos(rad)-fRbY1*sin(rad);
	fRbY=fRbX1*sin(rad)+fRbY1*cos(rad);
	fRtX=fRtX1*cos(rad)-fRtY1*sin(rad);
	fRtY=fRtX1*sin(rad)+fRtY1*cos(rad);

	fLbX=fLbX<fLtX?fLbX:fLtX;
	fLbX=fLbX<fRtX?fLbX:fRtX;
	fLbX=fLbX<fRbX?fLbX:fRbX;
	fLbY=fLbY<fLtY?fLbY:fLtY;
	fLbY=fLbY<fRtY?fLbY:fRtY;
	fLbY=fLbY<fRbY?fLbY:fRbY;

	fRtX=fRtX>fLtX?fRtX:fLtX;
	fRtX=fRtX>fLbX?fRtX:fLbX;
	fRtX=fRtX>fRbX?fRtX:fRbX;
	fRtY=fRtY>fLtY?fRtY:fLtY;
	fRtY=fRtY>fLbY?fRtY:fLbY;
	fRtY=fRtY>fRbY?fRtY:fRbY;

	fLbX+=lPos[0];
	fLbY+=lPos[1];
	fRtX+=lPos[0];
	fRtY+=lPos[1];

	OffsetToMap(fLbX, fLbY);
	OffsetToMap(fRtX, fRtY);

	iMinX = (int)(fLbX / g_fSceneTerrainTileWidth);
	iMinY = (int)(fLbY / g_fSceneTerrainTileWidth);
	iMaxX = (int)(fRtX / g_fSceneTerrainTileWidth);
	iMaxY = (int)(fRtY / g_fSceneTerrainTileWidth);

	OffsetTerrainToWorld(iMinX, iMinY);
	OffsetTerrainToWorld(iMaxX, iMaxY);
}

void RtsSceneDoc::GetPosByTerrainXY(int iTerrainX, int iTerrainY, float& fX, float& fY)
{
    OffsetTerrainToMap(iTerrainX, iTerrainY);
    fX = (iTerrainX+0.5) * g_fSceneTerrainTileWidth;
    fY = (iTerrainY+0.5) * g_fSceneTerrainTileWidth;
    OffsetToWorld(fX, fY);
}

bool RtsSceneDoc::AddAreaInfo(const char* szAreaFileName)
{
    RtArchive* pFile = RtCoreFile().CreateFileReader(szAreaFileName);
    if (pFile==NULL)
    {
        RtCoreLog().Error("RtsSceneArea: Load area information file [%s] failed.\n", szAreaFileName);
        return false;
    }

    // 文件信息
    DWORD dwRead;
    DWORD dwFileFlag = 'A'<<24 | 'r'<<16 | 'e'<<8 | 'a';
    *pFile << dwRead;
    if (dwRead!=dwFileFlag)
    {
        RtCoreLog().Error("RtsSceneDoc::AddAreaInfo: Opened file [%s] Not area information file.\n", szAreaFileName);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    // 版本号
    DWORD dwFileVersion = 1;
    *pFile << dwRead;
    if (dwRead!=dwFileVersion)
    {
        RtCoreLog().Error("RtsSceneDoc::AddAreaInfo: Opened area information [%s] Version [%d] unknown.\n", szAreaFileName, dwRead);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    int i, iSize, iCount;

    // 文件信息
    *pFile << iCount;
    *pFile << iSize;

    if (iCount==0)
    {
        RtCoreFile().CloseFile(pFile);
        return true;
    }

    m_iAreaRef = 1;
    RtsSceneBlockMap::SArea* pArea = RT_NEW RtsSceneBlockMap::SArea[iCount+m_iAreaCount];
    memcpy(m_pAreas, pArea, sizeof(RtsSceneBlockMap::SArea)*m_iAreaCount);
    DEL_ARRAY(m_pAreas);
    m_pAreas = pArea;

    char** ppBuf = RT_NEW char*[m_iAreaStringBufferCount+1];
    memcpy(ppBuf, m_ppAreaStringBuffer, sizeof(char*)*m_iAreaStringBufferCount);
    DEL_ARRAY(m_ppAreaStringBuffer);
    m_ppAreaStringBuffer = ppBuf;
    char* pBuf = RT_NEW char[iSize];
    m_ppAreaStringBuffer[m_iAreaStringBufferCount] = pBuf;
    m_iAreaStringBufferCount ++;

    RtString szName;
    RtString szData;
    for (i=0; i<iCount; i++)
    {
        pArea = m_pAreas + m_iAreaCount + i;
        *pFile << szName;
        *pFile << pArea->lMinX;
        *pFile << pArea->lMinY;
        *pFile << pArea->lMaxX;
        *pFile << pArea->lMaxY;
        *pFile << szData;

        rt2_strncpy2(pBuf, szName, strlen(szName));
		(*(pBuf + strlen(szName))) = 0;
        pArea->szName = pBuf;
        pBuf += szName.GetLength()+1;

        rt2_strncpy2(pBuf, szData, strlen(szData));
		(*(pBuf + strlen(szData))) = 0;
        pArea->szData = pBuf;
        pBuf += szData.GetLength()+1;
    }
    m_iAreaCount += iCount;

    // 结束标记
    *pFile << dwRead;
    if (dwRead!=dwFileFlag)
    {
        RtCoreLog().Error("RtsSceneDoc::AddAreaInfo: Opened area information file [%s] format error.\n", szAreaFileName);
        RtCoreFile().CloseFile(pFile);
        return false;
    }
    RtCoreFile().CloseFile(pFile);


    // 更新各个场景块的范围选择
    int x, y, z;
    long fMinX, fMinY, fMaxX, fMaxY;
    RtsSceneBlockMap* pMap;
    for (z=0; z<2; z++)
    {
        for (y=0; y<m_iBlockYCnt; y++)
        {
            for (x=0; x<m_iBlockXCnt; x++)
            {
                pMap = &m_blockMapArray[y*m_iBlockXCnt + x];
                fMinX = (x-m_iCenterPosX)*g_fSceneBlockWidth;
                fMinY = (y-m_iCenterPosY)*g_fSceneBlockHeight;
                fMaxX = fMinX+g_fSceneBlockWidth;
                fMaxY = fMinY+g_fSceneBlockHeight;
                if (pMap->m_szFileName[0])
                {
                    if (z)
                    {
                        if (pMap->m_iMapAreaCount)
                        {
                            pMap->m_ppAreas = RT_NEW RtsSceneBlockMap::SArea*[pMap->m_iMapAreaCount];
                            pMap->m_iMapAreaCount = 0;
                        }else
                        {
                            pMap->m_ppAreas = NULL;
                        }
                    }else
                    {
                        DEL_ARRAY(pMap->m_ppAreas);
                        pMap->m_iMapAreaCount = 0;
                    }
                    for (i=0; i<m_iAreaCount; i++)
                    {
                        pArea = m_pAreas + i;
                        if (!(fMinX>=pArea->lMaxX || fMinY>=pArea->lMaxY || fMaxX<pArea->lMinX || fMaxY<pArea->lMinY))
                        {
                            if (z)
                            {
                                pMap->m_ppAreas[pMap->m_iMapAreaCount] = m_pAreas+i;
                            }
                            pMap->m_iMapAreaCount ++;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool RtsSceneDoc::PointInArea(float fPosX, float fPosY, const char* szAreaName)
{
    int x, y, i;
    GetBlockByPos(fPosX, fPosY, x, y);
    if (x>=0 && x<m_iBlockXCnt && y>=0 && y<m_iBlockYCnt)
    {
        RtsSceneBlockMap* pBlockMap = &m_blockMapArray[y*m_iBlockXCnt+x];
        if (pBlockMap->m_szFileName[0]==0) return false;
        for (i=0; i<pBlockMap->m_iMapAreaCount; i++)
        {
            if (stricmp(pBlockMap->m_ppAreas[i]->szName, szAreaName)==0)
            {
				if(pBlockMap->m_ppAreas[i]->bActivation)
					return true;
            }
        }
    }
    return false;
}

RtsSceneBlockMap* s_pFindAreaBlockMap = NULL;
int s_iFindAreaIndex = 0;
float s_fFindAreaPosX, s_fFindAreaPosY;

RtsSceneBlockMap::SArea* RtsSceneDoc::FindFirstArea(float fPosX, float fPosY)
{
    int x, y;
    GetBlockByPos(fPosX, fPosY, x, y);
    if (x>=0 && x<m_iBlockXCnt && y>=0 && y<m_iBlockYCnt)
    {
        s_pFindAreaBlockMap = &m_blockMapArray[y*m_iBlockXCnt+x];
        if (s_pFindAreaBlockMap->m_szFileName[0])
        {
            s_iFindAreaIndex = 0;
            s_fFindAreaPosX = fPosX;
            s_fFindAreaPosY = fPosY;
            return FindNextArea();
        }
    }
    s_pFindAreaBlockMap = NULL;
    return NULL;
}

RtsSceneBlockMap::SArea* RtsSceneDoc::FindNextArea()
{
    if (s_pFindAreaBlockMap)
    {
        while (s_iFindAreaIndex<s_pFindAreaBlockMap->m_iMapAreaCount)
        {
            RtsSceneBlockMap::SArea* pAreas = s_pFindAreaBlockMap->m_ppAreas[s_iFindAreaIndex++];
            if (s_fFindAreaPosX>=pAreas->lMinX && s_fFindAreaPosX<pAreas->lMaxX && s_fFindAreaPosY>=pAreas->lMinY && s_fFindAreaPosY<pAreas->lMaxY)
            {
				if(pAreas->bActivation)
					return pAreas;
            }
        }
        s_pFindAreaBlockMap = NULL;
    }
    return NULL;
}

RtsSceneBlockMap* RtsSceneDoc::FindSceneBlockMapByAreaName(const char *sAreaName)
{
	if(!sAreaName || sAreaName[0] == 0)
		return NULL;
	for(int ii=0; ii<m_iBlockXCnt * m_iBlockYCnt; ii++)
	{
		RtsSceneBlockMap* pBlockMap = &m_blockMapArray[ii];
		if(!pBlockMap)
			continue;
		for(int jj=0; jj<pBlockMap->m_iMapAreaCount; jj++)
		{
			RtsSceneBlockMap::SArea* pAreas = pBlockMap->m_ppAreas[jj];
			if(pAreas && strncmp(pAreas->szName, sAreaName, strlen(pAreas->szName)) == 0)
				return pBlockMap;
		}
	}
	return NULL;
}


RtsSceneBlockMap::RtsSceneBlockMap()
    : m_pBlock(NULL),
      m_region_id(0),
      m_dwEnvironmentLighting(0),
      m_dwSunLighting(0),
      m_bPostProcessEffect(0),
      m_bPostProcessObjectHL(0),
      m_fPostProcessEffectContrast(0),
      m_fPostProcessEffectIntensity(0),
      m_fPostProcessObjectHLScale(0),
      m_dwPostProcessObjectHLColorEmi(0),
      m_dwPostProcessObjectHLColorDif(0),
      m_dwPostProcessObjectHLColorSpe(0),
      m_fPostProcessObjectHLColorSpePower(0),
      m_dwFogColor(0),
      m_fFogNear(0.f),
      m_fFogFar(0.f),
      m_fBackMusicStartTime(0.f),
      m_fBackMusicDifferenceTime(0.f),
      m_ppAreas(NULL),
      m_iMapAreaCount(0),
      m_lLastUseTime(0),
      m_blockPosx(0),
      m_blockPosy(0),
      m_state(Object_State_New)
{
	CM_MEMPROTECTOR(m_loadFileName, _MAX_PATH)
	CM_MEMPROTECTOR(m_szFileName, 20)
	CM_MEMPROTECTOR(m_szBlockName, 20)
	CM_MEMPROTECTOR(m_szBlockFlagTexture, 20)
	CM_MEMPROTECTOR(m_szBackMusic, 20)
	CM_MEMPROTECTOR2(m_szMiniMapFile, 4, 20)

	(*m_loadFileName) = 0;
    (*m_szFileName) = 0;
    (*m_szBlockName) = 0;
    (*m_szBlockFlagTexture) = 0;
    (*m_szBackMusic) = 0;
    memset(m_szMiniMapFile, 0, sizeof(m_szMiniMapFile));

    SetSunDir(0.f, 0.f);
}


RtsSceneBlockMap::~RtsSceneBlockMap()  
{ 
	CM_MEMUNPROTECTOR(m_loadFileName)
	CM_MEMUNPROTECTOR(m_szFileName)
	CM_MEMUNPROTECTOR(m_szBlockName)
	CM_MEMUNPROTECTOR(m_szBlockFlagTexture)
	CM_MEMUNPROTECTOR(m_szBackMusic)
	CM_MEMUNPROTECTOR2(m_szMiniMapFile, 4, 20)

	/*delete m_pBlock;*/ 
	DEL_ONE( m_pBlock ) ; 
	DEL_ARRAY(m_ppAreas);
}

bool RtsSceneBlockMap::New(const char* szPath, int iOffsetX, int iOffsetY)
{
    CHECK(m_pBlock==NULL);
#ifdef RT_SCENE_ONLY_LOAD_TERRAIN
    CHECKEX("Error");
    return false;
#else
    m_fFogNear = 300.f;
    m_fFogFar = 1000.f;
    m_dwEnvironmentLighting = 0xFF555555;
    m_dwSunLighting = 0xFF555555;
    m_dwFogColor = 0xFF555555;

    m_bPostProcessEffect = FALSE;
    m_bPostProcessObjectHL = FALSE;
    m_fPostProcessEffectContrast = 0;
    m_fPostProcessEffectIntensity = 0.f;
    m_fPostProcessObjectHLScale = 1.f;
    m_dwPostProcessObjectHLColorEmi = 0x00222222;
    m_dwPostProcessObjectHLColorDif = 0x00555555;
    m_dwPostProcessObjectHLColorSpe = 0x00FFFFFF;
    m_fPostProcessObjectHLColorSpePower = 8.f;
    m_blockPosx = iOffsetX;
    m_blockPosy = iOffsetY;

    m_pBlock = RT_NEW RtSceneBlock;
    m_pBlock->m_iBlockPosX = iOffsetX;
    m_pBlock->m_iBlockPosY = iOffsetY;
    char szPathName[_MAX_PATH];
    rt2_strcpy(szPathName, szPath);
    rt2_strcat(szPathName, m_szFileName);
    SetState(Object_State_Ready);
    return ((RtSceneBlock*)m_pBlock)->NewBlock(szPathName, m_pBlock->m_iBlockPosX*g_fSceneTileWidth*g_iSceneTileCntX, m_pBlock->m_iBlockPosY*g_fSceneTileWidth*g_iSceneTileCntY);
#endif
}

bool RtsSceneBlockMap::Load(const char* szPath, int iOffsetX, int iOffsetY, int iCenterX, int iCenterY)
{
    CHECK(m_pBlock==NULL);
#ifdef RT_SCENE_ONLY_LOAD_TERRAIN
    m_pBlock = RT_NEW RtSceneBlockTerrain;
#else
    m_pBlock = RT_NEW RtSceneBlock;
#endif
    char szPathName[_MAX_PATH];
    rt2_strcpy(szPathName, szPath);
    rt2_strcat(szPathName, m_szFileName);
    m_pBlock->m_iBlockPosX = iOffsetX-iCenterX;
    m_pBlock->m_iBlockPosY = iOffsetY-iCenterY;

    /*
    m_bPostProcessEffect = FALSE;
    m_bPostProcessObjectHL = FALSE;
    m_fPostProcessEffectContrast = 0;
    m_fPostProcessEffectIntensity = 0.f;
    m_fPostProcessObjectHLScale = 1.f;
    m_dwPostProcessObjectHLColorEmi = 0x00222222;
    m_dwPostProcessObjectHLColorDif = 0x00555555;
    m_dwPostProcessObjectHLColorSpe = 0x00FFFFFF;
    m_fPostProcessObjectHLColorSpePower = 8.f;
    */

    if (!m_pBlock->LoadBlock(szPathName))
        return false;

    return true;
}

bool RtsSceneBlockMap::Save(const char* szPath)
{
    CHECK(m_pBlock!=NULL);
#ifdef RT_SCENE_ONLY_LOAD_TERRAIN
    return false;
#else
    char szPathName[_MAX_PATH];
    rt2_strcpy(szPathName, szPath);
    rt2_strcat(szPathName, m_szFileName);
    return ((RtSceneBlock*)m_pBlock)->SaveBlock(szPathName);
#endif
}

bool RtsSceneBlockMap::Destroy()
{
    SetState(Object_State_New);
    DEL_ONE(m_pBlock);
    DEL_ARRAY(m_ppAreas);
    m_iMapAreaCount = 0;

    return true;
}

void RtsSceneBlockMap::SetSunDir(float inclination, float rotation)
{
#ifndef REGION_SERVER
    RtgMatrix16 _mat;
    RtgVertex3 _dir;
    _mat.Unit();
    _mat.RotateLZ(DegreeToRadian(m_fSunRotation = rotation));
    _mat.RotateLX(DegreeToRadian((m_fSunInclination = inclination) + 180.f));
    _mat.GetRow(2, _dir);
    _dir.Normalize();
    m_sunLightDir.x = _dir.x;
    m_sunLightDir.y = _dir.y;
    m_sunLightDir.z = _dir.z;
#endif
}

void RtsSceneBlockMap::Serialize(DWORD dwMapVersion, RtArchive* ar)
{
    ar->Serialize(m_szBlockName, 20);
    ar->Serialize(m_szFileName, 20);

    if (dwMapVersion >= _Scene_Block_Version_2)
    {
        *ar << m_dwEnvironmentLighting;
        *ar << m_dwSunLighting;
        *ar << m_dwFogColor;
        *ar << m_fFogNear;
        *ar << m_fFogFar;

        if (dwMapVersion >= _Scene_Block_Version_3)
        {
            *ar << m_bPostProcessEffect;               // 是否打开后期处理
            *ar << m_bPostProcessObjectHL;             // 是否打开物件高光
            *ar << m_fPostProcessEffectContrast;       // 后期处理对比度
            *ar << m_fPostProcessEffectIntensity;      // 后期处理强度
            *ar << m_fPostProcessObjectHLScale;        // 物件缩放
            *ar << m_dwPostProcessObjectHLColorEmi;    // 自发光
            *ar << m_dwPostProcessObjectHLColorDif;    // 漫反射
            *ar << m_dwPostProcessObjectHLColorSpe;    // 高光
            *ar << m_fPostProcessObjectHLColorSpePower;// 高光强度
        }

		if (dwMapVersion >= _Scene_Block_Version_4)
		{
            *ar <<  m_region_id;                       // REGION服务器分布
		}

        if (dwMapVersion >= _Scene_Block_Version_5)
        {
            *ar << m_sunLightDir;
            *ar << m_fSunInclination;
            *ar << m_fSunRotation;
        }

        ar->Serialize(m_szBlockFlagTexture, 20);

        *ar << m_fBackMusicStartTime;
        *ar << m_fBackMusicDifferenceTime;
        ar->Serialize(m_szBackMusic, 20);

        ar->Serialize(m_szMiniMapFile[0], 20);
        ar->Serialize(m_szMiniMapFile[1], 20);
        ar->Serialize(m_szMiniMapFile[2], 20);
        ar->Serialize(m_szMiniMapFile[3], 20);
    }
	
}


bool RtsSceneBlockMap::ActivationBlockArea(const char *sName, bool bAct)
{
	if(!sName || sName[0] == 0 || !m_ppAreas)
		return false;

	for(int ii=0; ii<m_iMapAreaCount; ii++)
	{
		RtsSceneBlockMap::SArea* pAreas = m_ppAreas[ii];
		if(pAreas && strncmp(pAreas->szName, sName, strlen(pAreas->szName)) == 0)
		{
			pAreas->bActivation = bAct;
			return true;
		}
	}

	return false;
}

#if RTS_DRAW_HELPER

void RtsSceneBlockMap::CreateHelper()
{
    RtSceneBlock* _block = (RtSceneBlock*)m_pBlock;
    RTASSERT(_block);

    for (int i = 0; i < _block->m_iGridSize; ++i)
    {
        RtsSGrid* pGrid = &(_block->m_pGrid[i]);

        for (int j = 0; j < pGrid->aryObject.size(); ++j)
        {
            RtsObject* obj = RtObjectManager()->GetObject(pGrid->aryObject[j]);

            if (!obj)
                continue;

            if (obj->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)) ||
                obj->IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
            {
                obj->SetShowType(RtGetRender(), RtsObject::SO_SPRITE);
            }
        }
    }
}

#endif