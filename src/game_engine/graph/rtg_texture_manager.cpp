
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_texture_manager.cpp
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph\rtg_texture_manager.h"
#include "graph\rtg_image.h"
#include "graph\rtg_texture_util.h"
#include "graph\rtg_graph.h"
#include "process.h"

namespace rt_graph {

    RtgTextureManager::RtgTextureManager(IDirect3DDevice9* pd3dDevice)
        : m_pDevice(pd3dDevice),
          m_texChessboard(NULL),
          m_bExit(true),
          m_bExited(true),
          m_bMutiThreadLoad(true)
    {
    }

    RtgTextureManager::~RtgTextureManager()
    {
        m_bExit = true;
        while (!m_bExited)
            Sleep(10);

        ReleaseTexture(m_texChessboard);

        for (list<RtgTextItem*>::iterator i = m_listCatch.begin();
             i != m_listCatch.end(); ++i)
        {
            DeleteTexture(*i);
        }

#ifdef _DEBUG
		// Comment by Wayne Wong 2010-11-11 
        //RTASSERT(m_mapText.empty());
        RTASSERT(m_setText.empty());
#else
        for (map<string, RtgTextItem*>::iterator i = m_mapText.begin();
             i != m_mapText.end(); ++i)
        {
            RtgTextItem* texItem = i->second;
            RtCoreLog().Error("texture unrelease(ref = %d) : %s\n", texItem->Ref, texItem->fileName);
        }

        for (set<RtgTextItem*>::iterator i = m_setText.begin();
            i != m_setText.end(); ++i)
        {
            RtgTextItem* texItem = *i;
            RtCoreLog().Error("texture unrelease(ref = %d) : %s\n", texItem->Ref, texItem->fileName);
        }
#endif
    }

    bool RtgTextureManager::Init()
    {
        m_bExit = false;
        uintptr_t res = _beginthread(Load_Thread, 0, (void*)this);
        RTASSERT(res != 1L);
        m_vecPath.push_back(".");
        m_texChessboard = TextUtil_CreateChessboard();
        return true;
    }

    bool RtgTextureManager::FrameMove()
    {
        begin_log_tick(0)

        RtgTextItem* texItem = NULL;

        m_cratLock.lock();
        if (!m_cratQueue.empty())
        {
            texItem = m_cratQueue.front();
            m_cratQueue.pop_front();
        }
        m_cratLock.unlock();
        
        if (texItem)
        {
            RTASSERT(texItem->state == Object_State_Creating);
            RTASSERT(texItem->pData);

            if (SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(
                m_pDevice,
                texItem->pData,
                texItem->dataSize,
                texItem->width,
                texItem->height,
                texItem->mipLevels,
                texItem->Usage,
                texItem->format,
                texItem->poolType,
                D3DX_DEFAULT,
                D3DX_DEFAULT,
                0,
                NULL,
                NULL,
                &texItem->pText
                )))
            {
                D3DSURFACE_DESC _Desc;
                texItem->pText->GetLevelDesc(0, &_Desc);
                texItem->width     = _Desc.Width;
                texItem->height    = _Desc.Height;
                texItem->poolType  = _Desc.Pool;
                texItem->Usage     = _Desc.Usage;
                texItem->state     = Object_State_Ready;
                texItem->mipLevels = texItem->pText->GetLevelCount(); 
                texItem->dataSize = 0;
                DEL_ARRAY(texItem->pData);
            }
            else
                RtCoreLog().Error("create texture : %s faild\n", texItem->fileName);
        }

        if (texItem)
            end_log_tick2(0, "create texture", texItem->fileName);

        return true;
    }

    IDirect3DTexture9* RtgTextureManager::GetD3dTexture(RtgTextItem* texItem)
    {
        if (!texItem)
            return NULL;

        if (texItem->textSrc == Text_Src_Sequence)
        {
            RtgSeqTextItem* seqItem = (RtgSeqTextItem*)texItem;

            if (seqItem->frameNum <= 0)
                return NULL;

            float curTime = RtGetRender()->GetAppTime();

            if (seqItem->lastUpdateTime < 0.f)
            {
                seqItem->curFrame = 0;
                seqItem->lastUpdateTime = curTime;
            }
            else
            {
                float difTime = curTime - seqItem->lastUpdateTime;

                if (difTime > seqItem->interFrame[seqItem->curFrame])
                {
                    ++seqItem->curFrame;
                    seqItem->lastUpdateTime = curTime;
                }

                if (seqItem->curFrame >= seqItem->frameNum)
                    seqItem->curFrame = 0;

            }
            
            texItem->pText = seqItem->apText[seqItem->curFrame]->pText;
        }

        return texItem->pText;
    }

    IDirect3DSurface9* RtgTextureManager::GetFrameTargt(RtgTextItem* texItem)
    {
        RTASSERT(texItem->state == Object_State_Ready);
        IDirect3DSurface9* frame = NULL;
        IDirect3DTexture9* text = NULL;

        if (texItem && 
            (texItem->Usage & D3DUSAGE_RENDERTARGET) &&
            (texItem->poolType == D3DPOOL_DEFAULT))
            text = GetD3dTexture(texItem);
        
        if (!text)
            return NULL;

        if (FAILED(text->GetSurfaceLevel(0, &frame)))
            return NULL;

        frame->Release();
        return frame;
    }

    IDirect3DSurface9* RtgTextureManager::GetDepthTargt(RtgTextItem* texItem)
    {
        RTASSERT(texItem->state == Object_State_Ready);
        IDirect3DSurface9* depth = NULL;
        IDirect3DTexture9* text = NULL;

        if (texItem && 
            (texItem->format == D3DFMT_D24S8 || texItem->format == D3DFMT_D24X8 || texItem->format == D3DFMT_D16 || D3DFMT_D32) &&
            (texItem->Usage & D3DUSAGE_DEPTHSTENCIL) &&
            (texItem->poolType == D3DPOOL_DEFAULT))
            text = GetD3dTexture(texItem);

        if (!text)
            return NULL;

        if (FAILED(text->GetSurfaceLevel(0, &depth)))
            return NULL;

        depth->Release();
        return depth;
    }

    void RtgTextureManager::AddTextSearchPath(const char szPath[])
    {
        m_vecPath.push_back(szPath);
    }

    bool RtgTextureManager::GetTextFilePath(char textPath[], const char textName[])
    {
        RTASSERT(textPath);
        RTASSERT(textName);

        const char* textExt[] = 
        {
            ".dds",
            ".tga",
            ".ifl"
        };

        const int extNum = sizeof(textExt) / sizeof(const char*);
        
        if (strrchr(textName, '.'))
        {
            for (size_t i = 0; i < m_vecPath.size(); ++i)
            {
                rt2_sprintf(textPath, "%s\\%s", m_vecPath[i].c_str(), textName);
                if (RtCoreFile().FileExist(textPath))
                    return true;
            }
        }
        else
        {
            for (size_t i = 0; i < m_vecPath.size(); ++i)
            {
                for (size_t j = 0; j < extNum; ++j)
                {
                    rt2_sprintf(textPath, "%s\\%s%s", m_vecPath[i].c_str(), textName, textExt[j]);
                    if (RtCoreFile().FileExist(textPath))
                        return true;
                }
            }
        }

        return false;
    }

    UINT RtgTextureManager::QueryFreeTextMemory()
    {
        return m_pDevice->GetAvailableTextureMem();
    }

    bool RtgTextureManager::IsDynamicTexture(RtgTextItem* texItem)
    {
        if (!texItem)
            return false;

        return ((texItem->poolType == D3DPOOL_DEFAULT) && 
                (texItem->Usage & D3DUSAGE_DYNAMIC));
    }

    bool RtgTextureManager::NativeLockable(RtgTextItem* texItem)
    {
        if (!texItem)
            return false;

        switch (texItem->poolType)
        {
        case D3DPOOL_DEFAULT :
            {
                if (texItem->Usage & D3DUSAGE_DYNAMIC)
                    return true;
                else
                    return false;
            }
            break;
        default :
            return true;
        }

        return true;
    }

    void RtgTextureManager::DumpTextInfo(const char fileName[])
    {
        RtCoreLog().Error("DumpTextInfo Unfulfilment\n");
    }

    bool RtgTextureManager::Save(RtgTextItem* texItem, const char* fileName)
    {
        RTASSERT(texItem);

        if (!texItem->pText)
            return false;

        if (!fileName)
            fileName = texItem->fileName;

        if (!*fileName)
            return false;
        
        if (FAILED(D3DXSaveTextureToFile(
                fileName, 
                GetTextFileFmt(fileName), 
                texItem->pText, 
                NULL)))
            return false;

        return true;
        
    }

    bool RtgTextureManager::Lock(RtgTextLock* texLock, RtgTextItem* texItem)
    {
        RTASSERT(texItem->state == Object_State_Ready);
        RTASSERT(texLock);
        RTASSERT(texItem);

        if (texItem->bLock || texItem->state < Object_State_Ready)
            return false;

        bool bCreateSM = false;
        DWORD lockFlag = D3DLOCK_NOSYSLOCK;
        D3DLOCKED_RECT d3dLockRect;

        if (NativeLockable(texItem))
        {
            texLock->texLock = GetD3dTexture(texItem);
            if (IsDynamicTexture(texItem))
                lockFlag = D3DLOCK_DISCARD;

        }
        else
        {
            D3DXCreateTexture(
                m_pDevice,
                texItem->width,
                texItem->height,
                1,
                0,
                texItem->format,
                D3DPOOL_SYSTEMMEM,
                &texLock->texLock
                );
            bCreateSM = true;

            if (texLock->flag & Text_Lock_Read)
            {
                IDirect3DSurface9* src = GetFrameTargt(texItem);
                IDirect3DSurface9* dst = NULL;

                if (SUCCEEDED(texLock->texLock->GetSurfaceLevel(0, &dst)))
                {
                    m_pDevice->GetRenderTargetData(src, dst);
                    dst->Release();
                }
            }

        }

        if (!texLock->texLock)
            return false;

        if (!(texLock->flag & Text_Lock_Write))
            lockFlag = D3DLOCK_READONLY;
        
        if (FAILED(texLock->texLock->LockRect(0, &d3dLockRect, NULL, lockFlag)))
        {
            if (bCreateSM)
                SafeRelease(texLock->texLock);
            return false;
        }

        texItem->bLock = true;
        texLock->texItem = texItem;
        texLock->width = texItem->width;
        texLock->height = texItem->height;
        texLock->pitch = d3dLockRect.Pitch;
        texLock->data = d3dLockRect.pBits;

        return true;
    }

    bool RtgTextureManager::Unlock(RtgTextLock* texLock)
    {
        RTASSERT(texLock);
        if (!texLock->texItem->bLock)
            return false;

        RTASSERT(texLock->texLock);
        texLock->texLock->UnlockRect(0);

        if (!NativeLockable(texLock->texItem))
        {
            if (texLock->flag & Text_Lock_Write)
            {
                IDirect3DTexture9* text = GetD3dTexture(texLock->texItem);
                RTASSERT(text != texLock->texLock);

                if (text == texLock->texLock)
                    return false;

                m_pDevice->UpdateTexture(texLock->texLock, text);
            }

            SafeRelease(texLock->texLock);
        }

        texLock->texItem->bLock = false;

        return true;
    }

    bool RtgTextureManager::UpdateTexture(RtgTextItem* texItem, void* data)
    {
        if (!texItem || !data)
            return false;

        RtgTextFill fillFunc = GetTextFill(texItem);

        if (!fillFunc)
            return false;

        RtgTextLock texLock;
        texLock.flag = Text_Lock_Write;

        if (!Lock(&texLock, texItem))
            return false;
        fillFunc(texLock.width, texLock.height, texLock.data, texLock.pitch, data, texLock.width * 4);
        Unlock(&texLock);

        return true;
    }

    bool RtgTextureManager::IsSequenceTextFile(const char fileName[])
    {
        const char* ext = strrchr(fileName, '.');
        if (ext && !stricmp(ext, ".ifl"))
            return true;
        return false;
    }

    RtgTextItem* RtgTextureManager::CreateTextureFromFile_Direct(const char filePath[], 
                                                                 UINT width /* = D3DX_DEFAULT */, 
                                                                 UINT height /* = D3DX_DEFAULT */, 
                                                                 UINT mipLevels /* = 0 */, 
                                                                 D3DFORMAT format /* = D3DFMT_FROM_FILE */, 
                                                                 D3DPOOL poolType /* = D3DPOOL_MANAGED */, 
                                                                 DWORD Usage /* = 0 */ ,
                                                                 BOOL bForceLoad
        )
    {
        RTASSERT(m_pDevice);

        RtgTextItem* texItem = NULL;
        map<string, RtgTextItem*>::iterator ifind = m_mapText.find(string(filePath));

        if (ifind != m_mapText.end())
        {
            texItem = ifind->second;
            if (texItem->Ref <= 0)
                m_listCatch.remove(texItem);
            ++texItem->Ref;
            return texItem;
        }

        if (IsSequenceTextFile(filePath))
        {
            RtgSeqTextItem* seqItem = RT_NEW RtgSeqTextItem();

            seqItem->textSrc = Text_Src_Sequence;
            rt2_strncpy(seqItem->fileName, filePath, MAX_PATH);
            seqItem->width = width;
            seqItem->height = height;
            seqItem->mipLevels = mipLevels;
            seqItem->format = format;
            seqItem->poolType = poolType;
            seqItem->Usage = Usage;
            seqItem->Ref = 1;
            seqItem->dataSize = 0;
            seqItem->pData = NULL;
            seqItem->pText = NULL;
            seqItem->bLock = false;
            seqItem->state = Object_State_New;
            seqItem->frameNum = 0;
            seqItem->curFrame = 0;
            seqItem->lastUpdateTime = -1.f;
            texItem = (RtgTextItem*)seqItem;

            RtArchive* pFile = RtCoreFile().CreateFileReader(seqItem->fileName, 0);

            if (!pFile)
            {
                DEL_ONE(texItem);
                return NULL;
            }

            int _Size = pFile->TotalSize();
            char* pBuffer = RT_NEW char[_Size + 1];

            pFile->Serialize(pBuffer, _Size);
            pBuffer[_Size] = 0;
            pFile->Close();
            DEL_ONE(pFile);

            char  step[] = "\n\r\0";
            char* str = strtok(pBuffer, step);
            char  mid[128];
            char  path[MAX_PATH];

            char* tmp = strrchr(seqItem->fileName, '/');
            if (!tmp)
                tmp = strrchr(seqItem->fileName, '\\');
            if (tmp)
            {
                int cnt =  tmp - seqItem->fileName;
                rt2_strncpy2(mid, seqItem->fileName, cnt);
                mid[cnt] = 0;
            }
            else
            {
                *mid = 0;
            }

            while(str && seqItem->frameNum < RtgSeqTextItem::SetText_Max_Text_Num)
            {
                if (*mid)
                    rt2_sprintf(path, "%s\\%s", mid, str);
                else
                    rt2_strcpy(path, str);
                seqItem->apText[seqItem->frameNum] = CreateTextureFromFile(
                    path,
                    width,
                    height,
                    mipLevels,
                    format,
                    poolType,
                    Usage,
                    bForceLoad
                    );
                seqItem->interFrame[seqItem->frameNum] = 0.033f;
                ++seqItem->frameNum;
                str = strtok(NULL, step);
            } 
        }
        else
        {
            texItem = RT_NEW RtgTextItem;
            texItem->textSrc = Text_Src_File;
            rt2_strncpy(texItem->fileName, filePath, MAX_PATH);
            texItem->width = width;
            texItem->height = height;
            texItem->mipLevels = mipLevels;
            texItem->format = format;
            texItem->poolType = poolType;
            texItem->Usage = Usage;
            texItem->Ref = 1;
            texItem->dataSize = 0;
            texItem->pData = NULL;
            texItem->pText = NULL;
            texItem->bLock = false;

			// Added by Wayne Wong 2011-01-04
			CheckMipLevels(texItem);

			if (!bForceLoad && m_bMutiThreadLoad)
            {
                texItem->state = Object_State_Loading;
                m_mapText[string(filePath)] = texItem;
                m_loadLock.lock();
                m_loadQueue.push_back(texItem);
                m_loadLock.unlock();
            }
            else
            {
                begin_log_tick(0);
                texItem->state = Object_State_New;
                if (CreateTexture(texItem))
                    m_mapText[string(filePath)] = texItem;
                else
                    DEL_ONE(texItem);
                if (texItem)
                    end_log_tick2(0, "force create texture", texItem->fileName);
            }

        }

        return texItem;
    }


    RtgTextItem* RtgTextureManager::CreateTextureFromFile(const char fileName[], 
                                                          UINT width /* = D3DX_DEFAULT */, 
                                                          UINT height /* = D3DX_DEFAULT */, 
                                                          UINT mipLevels /* = 0 */, 
                                                          D3DFORMAT format /* = D3DFMT_FROM_FILE */, 
                                                          D3DPOOL poolType /* = D3DPOOL_MANAGED */, 
                                                          DWORD Usage /* = 0 */ ,
                                                          BOOL bForceLoad
                                                          )
    {
        RTASSERT(m_pDevice);

        char filePath[MAX_PATH];
        if (!GetTextFilePath(filePath, fileName))
        {
            RtCoreLog().Error_ResMissing("can not find texture : %s\n", fileName);
            return NULL;
        }
        
        return CreateTextureFromFile_Direct(filePath,
            width,
            height,
            mipLevels,
            format,
            poolType,
            Usage,
            bForceLoad);
    }

    RtgTextItem* RtgTextureManager::CreateTexture(UINT width, 
                                                  UINT height, 
                                                  UINT mipLevels /* = 1 */, 
                                                  D3DFORMAT format /* = D3DFMT_A8R8G8B8 */, 
                                                  D3DPOOL poolType /* = D3DPOOL_MANAGED */, 
                                                  DWORD Usage /* = 0 */
                                                  )
    {
        RTASSERT(width > 0);
        RTASSERT(height > 0);
        RTASSERT(m_pDevice);

        RtgTextItem* texItem = RT_NEW RtgTextItem();

        texItem->textSrc = Text_Src_Dynamic;
        texItem->fileName[0] = 0;
        texItem->width = width;
        texItem->height = height;
        texItem->mipLevels = mipLevels;
        texItem->format = format;
        texItem->poolType = poolType;
        texItem->Usage = Usage;
        texItem->Ref = 1;
        texItem->dataSize = 0;
        texItem->pData = NULL;
        texItem->pText = NULL;
        texItem->bLock = false;
        texItem->state = Object_State_New;
        
        if (CreateTexture(texItem))
            m_setText.insert(texItem);
        else
            DEL_ONE(texItem);

        return texItem;
    }

    RtgTextItem* RtgTextureManager::CreateTextureFromFileMemery(PVOID pData, 
                                                                UINT dataSize, 
                                                                UINT width /* = D3DX_DEFAULT */, 
                                                                UINT height /* = D3DX_DEFAULT */, 
                                                                UINT mipLevels /* = D3DX_DEFAULT */, 
                                                                D3DFORMAT format /* = D3DFMT_FROM_FILE */, 
                                                                D3DPOOL poolType /* = D3DPOOL_MANAGED */, 
                                                                DWORD Usage /* = 0 */ 
                                                                )
    {
        RTASSERT(m_pDevice);

        RtgTextItem* texItem = RT_NEW RtgTextItem();

        texItem->textSrc = Text_Src_File_Memery;
        texItem->fileName[0] = 0;
        texItem->width = width;
        texItem->height = height;
        texItem->mipLevels = mipLevels;
        texItem->format = format;
        texItem->poolType = poolType;
        texItem->Usage = Usage;
        texItem->Ref = 1;
        texItem->dataSize = dataSize;
        texItem->pData = pData;
        texItem->pText = NULL;
        texItem->bLock = false;
        texItem->state = Object_State_New;

		// Added by Wayne Wong 2011-01-04
		CheckMipLevels(texItem);

        if (CreateTexture(texItem))
            m_setText.insert(texItem);
        else
            DEL_ONE(texItem);

        return texItem;
    }

    RtgTextItem* RtgTextureManager::CreateTextureFromArchive(RtArchive* pArchive, 
                                                             UINT width /* = D3DX_DEFAULT */, 
                                                             UINT height /* = D3DX_DEFAULT */, 
                                                             UINT mipLevels /* = D3DX_DEFAULT */, 
                                                             D3DFORMAT format /* = D3DFMT_FROM_FILE */, 
                                                             D3DPOOL poolType /* = D3DPOOL_MANAGED */, 
                                                             DWORD Usage /* = 0 */ 
                                                             )
    {
        RTASSERT(pArchive);
        RTASSERT(m_pDevice);

        void* pData = RT_NEW char[pArchive->TotalSize()];
        pArchive->Serialize(pData, pArchive->TotalSize());

        return CreateTextureFromFileMemery(
            pData,
            pArchive->TotalSize(),
            width,
            height,
            mipLevels,
            format,
            poolType,
            Usage);
    }


    RtgTextItem* RtgTextureManager::CreateTextureFromARGBData(PVOID pData, 
                                                              UINT dataSize, 
                                                              UINT width, 
                                                              UINT height, 
                                                              UINT mipLevels /* = 1 */, 
                                                              D3DFORMAT format /* = D3DFMT_A8R8G8B8 */, 
                                                              D3DPOOL poolType /* = D3DPOOL_MANAGED */, 
                                                              DWORD Usage /* = 0 */ 
                                                              )
    {
        RTASSERT(width > 0);
        RTASSERT(height > 0);
        RTASSERT(m_pDevice);

        RtgTextItem* texItem = RT_NEW RtgTextItem();

        texItem->textSrc = Text_Src_ARGB_Data;
        texItem->fileName[0] = 0;
        texItem->width = width;
        texItem->height = height;
        texItem->mipLevels = mipLevels;
        texItem->format = format;
        texItem->poolType = poolType;
        texItem->Usage = Usage;
        texItem->Ref = 1;
        texItem->dataSize = dataSize;
        texItem->pData = pData;
        texItem->pText = NULL;
        texItem->bLock = false;
        texItem->state = Object_State_New;

        if (CreateTexture(texItem))
            m_setText.insert(texItem);
        else
            DEL_ONE(texItem);

        return texItem;
    }

    RtgTextItem* RtgTextureManager::CreateDynamicTexture(UINT width, 
                                                         UINT height,
                                                         D3DFORMAT format/* = D3DFMT_A8R8G8B8*/
                                                         )
    {
        return CreateTexture(
            width,
            height,
            1,
            format,
            D3DPOOL_DEFAULT,
            D3DUSAGE_DYNAMIC
            );
    }

    RtgTextItem* RtgTextureManager::CreateFrameBufferTexture(UINT width, 
                                                             UINT height, 
                                                             D3DFORMAT format /* = D3DFMT_A8R8G8B8 */
                                                             )
    {
        return CreateTexture(
            width,
            height,
            1,
            format,
            D3DPOOL_DEFAULT,
            D3DUSAGE_RENDERTARGET
            );
    }

    RtgTextItem* RtgTextureManager::CreateDepthBufferTexture(UINT width, 
                                                             UINT height, 
                                                             D3DFORMAT format /* = D3DFMT_D24S8 */
                                                             )
    {
        return CreateTexture(
            width,
            height,
            1,
            format,
            D3DPOOL_DEFAULT,
            D3DUSAGE_DEPTHSTENCIL
            );
    }

    RtgTextItem* RtgTextureManager::CreateUiTexture(const char fileName[], 
                                                    UINT width /* = D3DX_DEFAULT */, 
                                                    UINT height /* = D3DX_DEFAULT */ 
                                                    )
    {
        return CreateTextureFromFile(
            fileName,
            width,
            height,
            D3DX_UI_MIPLEVEL,
            D3DFMT_FROM_FILE,
            D3DPOOL_MANAGED,
            0,
            true);
    }

    bool RtgTextureManager::CreateTexture(RtgTextItem* textItem)
    {
        RTASSERT(textItem);
        RTASSERT(m_pDevice);
        RTASSERT((textItem->textSrc == Text_Src_Sequence) || !textItem->pText);

        switch (textItem->textSrc)
        {
        case Text_Src_File :
            {
                RtArchive* ar = RtCoreFile().CreateFileReader(textItem->fileName, 0);

                if (!ar)
                    return false;

                textItem->dataSize = ar->TotalSize();
                textItem->pData = RT_NEW char[textItem->dataSize];
                ar->Serialize(textItem->pData, textItem->dataSize);

                ar->Close();
                DEL_ONE(ar);

                if (FAILED(D3DXCreateTextureFromFileInMemoryEx(
                    m_pDevice,
                    textItem->pData,
                    textItem->dataSize,
                    textItem->width,
                    textItem->height,
                    textItem->mipLevels,
                    textItem->Usage,
                    textItem->format,
                    textItem->poolType,
                    D3DX_DEFAULT,
                    D3DX_DEFAULT,
                    0,
                    NULL,
                    NULL,
                    &textItem->pText
                    )))
                {
                    DEL_ARRAY(textItem->pData);
                    return false;
                }

                DEL_ARRAY(textItem->pData);
            }
            break;

        case Text_Src_File_Memery :
            {
                if (!textItem->pData)
                    return false;

                if (FAILED(D3DXCreateTextureFromFileInMemoryEx(
                    m_pDevice,
                    textItem->pData,
                    textItem->dataSize,
                    textItem->width,
                    textItem->height,
                    textItem->mipLevels,
                    textItem->Usage,
                    textItem->format,
                    textItem->poolType,
                    D3DX_DEFAULT,
                    D3DX_DEFAULT,
                    0,
                    NULL,
                    NULL,
                    &textItem->pText
                    )))
                {
                    return false;
                }

                if (textItem->poolType == D3DPOOL_DEFAULT)
                {
                    void* data = RT_NEW char[textItem->dataSize];
                    memcpy(data, textItem->pData, textItem->dataSize);
                    textItem->pData = data;
                }
                else
                {
                    textItem->pData = NULL;
                }
            }
            break;

        case Text_Src_ARGB_Data :
            {
                if (!textItem->pData)
                    return false;

                if (FAILED(D3DXCreateTexture(
                    m_pDevice,
                    textItem->width,
                    textItem->height,
                    textItem->mipLevels,
                    textItem->Usage,
                    textItem->format,
                    textItem->poolType,
                    &textItem->pText
                    )))
                {
                    return false;
                }

                IDirect3DTexture9* textFill = textItem->pText;

                if (textItem->poolType == D3DPOOL_DEFAULT)
                {
                    if (FAILED(D3DXCreateTexture(
                        m_pDevice,
                        textItem->width,
                        textItem->height,
                        1,
                        0,
                        textItem->format,
                        D3DPOOL_SYSTEMMEM,
                        &textFill 
                        )))
                    {   
                        SafeRelease(textItem->pText);
                        return false;
                    }
                }

                D3DLOCKED_RECT lockRect;

                if (SUCCEEDED(textFill->LockRect(0, &lockRect, NULL, 0)))
                {
                    memcpy(lockRect.pBits, textItem->pData, lockRect.Pitch * textItem->height);
                    textFill->UnlockRect(0);
                }

                if (textItem->poolType == D3DPOOL_DEFAULT)
                {
                    m_pDevice->UpdateTexture(textFill, textItem->pText);
                    SafeRelease(textFill);

                    void* data = RT_NEW char[textItem->dataSize];
                    memcpy(data, textItem->pData, textItem->dataSize);
                    textItem->pData = data;
                }
                else
                {
                    textItem->pData = NULL;
                }
            }
            break;

        case Text_Src_Dynamic :
            {
                if (FAILED(D3DXCreateTexture(
                    m_pDevice,
                    textItem->width,
                    textItem->height,
                    textItem->mipLevels,
                    textItem->Usage,
                    textItem->format,
                    textItem->poolType,
                    &textItem->pText
                    )))
                {   
                    return false;
                }
            }
            break;

        case Text_Src_Sequence :
            {
                RTASSERT(false);
            } 
            
            break;

        default :
            return false;
            break;
        }

        IDirect3DTexture9* pd3dText = GetD3dTexture(textItem);

        if (pd3dText)
        {
            D3DSURFACE_DESC _Desc;
            pd3dText->GetLevelDesc(0, &_Desc);
            textItem->width     = _Desc.Width;
            textItem->height    = _Desc.Height;
            textItem->poolType  = _Desc.Pool;
            textItem->Usage     = _Desc.Usage;
            textItem->state     = Object_State_Ready;
            textItem->mipLevels = pd3dText->GetLevelCount(); 
        }

        return true;
    }

    void RtgTextureManager::ReleaseTexture(RtgTextItem* texItem)
    {
        if (!texItem)
            return;

        if (texItem->textSrc == Text_Src_Sequence)
        {
            RtgSeqTextItem* _seqText = (RtgSeqTextItem*)texItem;

            while (--_seqText->frameNum >= 0)
                ReleaseTexture(_seqText->apText[_seqText->frameNum]);

            DEL_ONE(texItem);

            return;
        }

        if (--texItem->Ref <= 0)
        {
            if (texItem->textSrc == Text_Src_File)
            {
                if (m_listCatch.size() >= Text_Catch_Size)
                {
                    DeleteTexture(m_listCatch.front());
                    m_listCatch.pop_front();
                    RTASSERT(find(m_listCatch.begin(), m_listCatch.end(), texItem) == m_listCatch.end());
                }
                m_listCatch.push_back(texItem);
            }
            else
            {
                DeleteTexture(texItem);
            }
        }
    }

    void RtgTextureManager::DeleteTexture(RtgTextItem* texItem)
    {
        RTASSERT(texItem);
        RTASSERT(texItem->Ref <= 0);

        if (texItem->textSrc == Text_Src_File)
        {
            switch (texItem->state)
            {
            case Object_State_Loading :
                {
                    m_loadLock.lock();
                    m_loadQueue.remove(texItem);
                    m_loadLock.unlock();

                }
                break;
            case Object_State_Creating :
                {
                    m_cratLock.lock();
                    m_cratQueue.remove(texItem);
                    m_cratLock.unlock();
                }
                break;
            default :
                __assume(0);
                break;
            }
        }

        switch (texItem->textSrc)
        {
        case Text_Src_File :
            {
                SafeRelease(texItem->pText);
                m_mapText.erase(texItem->fileName);
                DEL_ONE(texItem);
            }
            break;

        case Text_Src_File_Memery :
        case Text_Src_ARGB_Data   :
        case Text_Src_Dynamic     :
            {
                SafeRelease(texItem->pText);
                DEL_ARRAY(texItem->pData);
                m_setText.erase(texItem);
                DEL_ONE(texItem);
            }
            break;

        case Text_Src_Sequence :
            {
                RTASSERT(false);
            }
            break;

        default :
            RTASSERT(false);
            break;
        }
    }

    void RtgTextureManager::OnLostDevice()
    {
        RTASSERT(m_vecResetText.empty());

        for (map<string, RtgTextItem*>::iterator i = m_mapText.begin();
             i != m_mapText.end(); ++i)
        {
            RtgTextItem* textItem = i->second;

            if (textItem->poolType == D3DPOOL_DEFAULT)
            {
                SafeRelease(textItem->pText);
                textItem->state = Object_State_New;
                m_vecResetText.push_back(textItem);
            }
        }

        for (set<RtgTextItem*>::iterator i = m_setText.begin();
            i != m_setText.end(); ++i)
        {
            RtgTextItem* textItem = (*i);

            if (textItem->poolType == D3DPOOL_DEFAULT)
            {
                SafeRelease(textItem->pText);
                textItem->state = Object_State_New;
                m_vecResetText.push_back(textItem);
            }
        }
    }

    void RtgTextureManager::OnResetDevice()
    {
        for (size_t i = 0; i < m_vecResetText.size(); ++i)
        {
            RtgTextItem* textItem = m_vecResetText[i];
            CreateTexture(textItem);
        }
        m_vecResetText.clear();
    }

    bool RtgTextureManager::SetTextureQuality(int _Quality)
    {
        return false;
    }

    int RtgTextureManager::GetTextureQuality()
    {
        return 0;
    }

    D3DXIMAGE_FILEFORMAT RtgTextureManager::GetTextFileFmt(const char fileName[])
    {
        const char* _Ext = strrchr(fileName, '.');

        if (!_Ext)
            return D3DXIFF_DDS;

        ++_Ext;

        if (!stricmp(_Ext, "dds"))
            return D3DXIFF_DDS;
        else if (!stricmp(_Ext, "tga"))
            return D3DXIFF_TGA;
        else if (!stricmp(_Ext, "bmp"))
            return D3DXIFF_BMP;
        else if (!stricmp(_Ext, "jpg"))
            return D3DXIFF_JPG;
        else if (!stricmp(_Ext, "png"))
            return D3DXIFF_PNG;
        else 
            return D3DXIFF_DDS;
    }

    RtgTextFill RtgTextureManager::GetTextFill(RtgTextItem* texItem)
    {
        RTASSERT(texItem);

        switch (texItem->format)
        {
        case D3DFMT_A8R8G8B8 :
            return Rtgimg_Fill_A8R8G8B8;
            break;
        case D3DFMT_X8R8G8B8 :
            return Rtgimg_Fill_X8R8G8B8;
            break;
        case D3DFMT_R8G8B8 :
            return Rtgimg_Fill_R8G8B8;
            break;
        case D3DFMT_A4R4G4B4 :
            return Rtgimg_Fill_A4R4G4B4;
            break;
        case D3DFMT_A1R5G5B5 :
            return Rtgimg_Fill_A1R5G5B5;
            break;
        case D3DFMT_R5G6B5 :
            return Rtgimg_Fill_R5G6B5;
            break;
        case D3DFMT_X1R5G5B5 :
            return Rtgimg_Fill_X1R5G5B5;
            break;
        case D3DFMT_X4R4G4B4 :
            return Rtgimg_Fill_X4R4G4B4;
            break;
        default :
            break;
        }

        return NULL;
    }

    RtgTextItem* RtgTextureManager::GetChessboardText()
    {
        return m_texChessboard;
    }

    void RtgTextureManager::Load_Thread(void* _ptMgr)
    {
        RtgTextureManager* ptMgr = (RtgTextureManager*)_ptMgr;
        ptMgr->m_bExited = false;

        while (!ptMgr->m_bExit)
        {
            if (ptMgr->m_loadQueue.empty())
            {
                Sleep(10);
                continue;
            }

            ptMgr->m_loadLock.lock();
            RtgTextItem* texItem = ptMgr->m_loadQueue.front();
            ptMgr->m_loadQueue.pop_front();
            ptMgr->m_loadLock.unlock();

            RtArchive* ar = RtCoreFile().CreateFileReader(texItem->fileName, 0);

            if (!ar)
            {
                RtCoreLog().Error("create file reader %s fail\n", texItem->fileName);
                continue;
            }
            
            texItem->dataSize = ar->TotalSize();
            texItem->pData = RT_NEW char[texItem->dataSize];
            ar->Serialize(texItem->pData, texItem->dataSize);

            ar->Close();
            DEL_ONE(ar);

            ptMgr->m_cratLock.lock();
            ptMgr->m_cratQueue.push_back(texItem);
            ptMgr->m_cratLock.unlock();
            InterlockedExchange(&texItem->state, Object_State_Creating);
        }

        ptMgr->m_bExited = true;
    }

	// Added by Wayne Wong 2011-01-04
	void RtgTextureManager::CheckMipLevels( RtgTextItem* texItem )
	{
		if (texItem->mipLevels == 0 || texItem->mipLevels == D3DX_DEFAULT)
		{
			D3DXIMAGE_INFO info;
			if (texItem->textSrc == Text_Src_File)
			{
				if (D3D_OK != D3DXGetImageInfoFromFile(texItem->fileName, &info))
					return;
			}
			else if (texItem->textSrc == Text_Src_File_Memery)
			{
				if (D3D_OK != D3DXGetImageInfoFromFileInMemory(texItem->pData, texItem->dataSize, &info))
					return;
			}
			else
			{
				return;
			}

			if ((info.MipLevels > 0) && (info.ImageFileFormat == D3DXIFF_DDS))
				texItem->mipLevels = D3DX_FROM_FILE;
		}
	}

}
