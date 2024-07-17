
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_scene_block_manager.h
//     Author : zxy
// CreateTime : 2010.05.01
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "scene\rt_scene.h"
#include "process.h"

RtsSceneBlockManager::RtsSceneBlockManager()
    : m_bExit(true),
      m_bExited(true)
{
}

RtsSceneBlockManager::~RtsSceneBlockManager()
{
    m_bExit = true;
    while (!m_bExited)
        Sleep(10);
}

bool RtsSceneBlockManager::Init()
{
    uintptr_t res = _beginthread(Load_Thread, 0, (void*)this);
    RTASSERT(res != 1L);
    m_bExit = false;
    return true;
}

void RtsSceneBlockManager::FrameMove()
{
    RtsSceneBlockMap* _blockMap = NULL;

    m_cratLock.lock();
    if (!m_creatQueue.empty())
    {
        _blockMap = m_creatQueue.front();
        m_creatQueue.pop_front();
    }
    m_cratLock.unlock();

    if (_blockMap)
        _blockMap->OnCreateFinish();
}

void RtsSceneBlockManager::EvictResources(const Int2& _centerBlock)
{
    if (m_blockLoad.size() > 0)
    {
        set<Int2>::iterator i = m_blockLoad.begin();

        while (i != m_blockLoad.end())
        {
            RtsSceneBlockMap* _blockMap = RtsGetScene()->GetBlockMap(i->x, i->y);
            
            if (!_blockMap || _blockMap->GetState() != Object_State_Ready)
            {
                ++i;
                continue;
            }

            if (abs(_blockMap->m_blockPosx - _centerBlock.x) > 1 || 
                abs(_blockMap->m_blockPosy - _centerBlock.y) > 1)
            {
/*
#ifdef SCENE_EDITOR
                if (_blockMap->m_pBlock->GetModifyFlag())
                {
                    ((RtSceneBlock*)_blockMap->m_pBlock)->SaveBlock(_blockMap->m_loadFileName);
                }
#endif */
                ReleaseSceneBlockImpl(_blockMap);
                i = m_blockLoad.erase(i);
                RtCoreLog().Info("auto release scene block : %s\n", _blockMap->m_szFileName);
            }           
            else
            {
                ++i;
            }
        }
    }
}

bool RtsSceneBlockManager::CreateSceneBlock(RtsSceneBlockMap* _block, 
                                            const char szPath[], 
                                            int blockPosx, 
                                            int blockPosy, 
                                            bool bForceLoad /* = false */
                                            )
{
    RTASSERT(_block);
    _block->SetLoadInfo(szPath, blockPosx, blockPosy);

    if (bForceLoad)
    {
        CreateSceneBlockImpl(_block);
        _block->Loack_SetState(Object_State_Ready);
        _block->OnLoadFinish();
    }
    else
    {
        _block->Loack_SetState(Object_State_Loading);
        m_loadLock.lock();
        m_loadQueue.push_back(_block);
        m_loadLock.unlock();
    }


    m_blockLoad.insert(Int2(_block->m_blockPosx, _block->m_blockPosy));

    return true;
}

bool RtsSceneBlockManager::NewSceneBlock(RtsSceneBlockMap* _block, 
                                         const char szPath[], 
                                         int blockPosx, 
                                         int blockPosy)
{
    RTASSERT(_block);
    _block->New(szPath, blockPosx, blockPosy);
    m_blockLoad.insert(Int2(_block->m_blockPosx, _block->m_blockPosy));
    return true;
}

void RtsSceneBlockManager::ReleaseSceneBlock(RtsSceneBlockMap* _block)
{
    if (!_block)
        return;

    set<Int2>::iterator it = m_blockLoad.find(Int2(_block->m_blockPosx, _block->m_blockPosy));

    if (it != m_blockLoad.end())
        m_blockLoad.erase(it);

    ReleaseSceneBlockImpl(_block);
}

bool RtsSceneBlockManager::CreateSceneBlockImpl(RtsSceneBlockMap* _block)
{
    RTASSERT(_block && !_block->m_pBlock);

#ifdef RT_SCENE_ONLY_LOAD_TERRAIN
    _block->m_pBlock = RT_NEW RtSceneBlockTerrain;
#else
    _block->m_pBlock = RT_NEW RtSceneBlock;
#endif

    _block->m_pBlock->m_iBlockPosX = _block->m_blockPosx;
    _block->m_pBlock->m_iBlockPosY = _block->m_blockPosy;

    if (!_block->m_pBlock->LoadBlock(_block->m_loadFileName))
        return false;
    return true;
}


void RtsSceneBlockManager::ReleaseSceneBlockImpl(RtsSceneBlockMap* _block)
{
    RTASSERT(_block);
    _block->Destroy();
}

void RtsSceneBlockManager::Load_Thread(void* _ptMgr)
{
    RtsSceneBlockManager* ptMgr = (RtsSceneBlockManager*)_ptMgr;
    ptMgr->m_bExited = false;

    while (!ptMgr->m_bExit)
    {
        ptMgr->m_loadLock.lock();
        if (ptMgr->m_loadQueue.empty())
        {
            ptMgr->m_loadLock.unlock();
            Sleep(10);
            continue;
        }

        RtsSceneBlockMap* _block = ptMgr->m_loadQueue.front();
        ptMgr->m_loadQueue.pop_front();
        ptMgr->m_loadLock.unlock();

        if (ptMgr->CreateSceneBlockImpl(_block))
        {
            ptMgr->m_cratLock.lock();
            ptMgr->m_creatQueue.push_back(_block);
            ptMgr->m_cratLock.unlock();
            _block->Loack_SetState(Object_State_Ready);
        }
    }

    ptMgr->m_bExited = true;
}

size_t RtsSceneBlockManager::LoadQueueSize() 
{
    size_t _size = 0;
    m_loadLock.lock();
    _size = m_loadQueue.size();
    m_loadLock.unlock();
    return _size;
}