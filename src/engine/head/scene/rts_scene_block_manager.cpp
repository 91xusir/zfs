
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
    : m_bExit(true), m_bExited(true), m_bMutiThreadLoad(true) {}

RtsSceneBlockManager::~RtsSceneBlockManager() {
    m_bExit = true;
    while (!m_bExited)
        Sleep(10);
}

bool RtsSceneBlockManager::Init() {
    m_bExit = false;  // 将 m_bExit 标志设为 false，表示当前对象尚未退出

    // 启动一个新线程，执行 Load_Thread 函数
    // _beginthread 函数用于创建和启动一个线程
    // 线程的入口函数是 Load_Thread
    // 线程的堆栈大小设为 0（使用默认大小）
    // 传递的参数是当前对象的指针 (this)
    uintptr_t res = _beginthread(Load_Thread, 0, (void*)this);
    RTASSERT(res != 1L);
    return true;
}

void RtsSceneBlockManager::FrameMove() {

    RtsSceneBlockRtb* _blockMap = nullptr;
    // 加锁并处理创建队列中的元素
    m_cratLock.lock();
    if (!m_creatQueue.empty()) {
        _blockMap = m_creatQueue.front();  // 获取创建队列的第一个元素
        m_creatQueue.pop_front();          // 将第一个元素从创建队列中移除
    }
    m_cratLock.unlock();
    // 处理从释放队列中的对象
    if (_blockMap) {
        const auto _it = m_releaseQueue.find(_blockMap);  // 在释放队列中查找该对象的指针
        if (_it != m_releaseQueue.end())  // 如果在释放队列中找到了该对象
        {
            (*_it)->Destroy();  // 调用对象的 Destroy() 方法，执行对象的清理工作
            m_releaseQueue.erase(_it);  // 从释放队列中移除该对象
        } else                          // 如果该对象不在释放队列中
        {
#if RTS_DRAW_HELPER
            _blockMap->CreateHelper();  // 如果定义了 RTS_DRAW_HELPER，则调用 CreateHelper() 方法
#endif
        }
    }

    // 处理删除块数组中的元素
    auto _it = m_DelectBlockArray.begin();
    while (_it != m_DelectBlockArray.end()) {
        SceneBlockArray& _ba = (*_it);
        size_t           j   = 0;

        while (j < _ba.size())  // 遍历当前块数组中的所有块
        {
            if (_ba[j].GetState() != Object_State_New)  // 如果当前块的状态不是新创建状态
                break;                                  // 退出循环
            ++j;                                        // 否则继续检查下一个块
        }

        if (j == _ba.size())  // 如果所有块的状态都是新创建状态
        {
            //lyytodo 这里会释放错误奔溃  先注释
            //DEL_ARRAY(_ba._blockArray);
            _ba._size = 0;                        // 将数组大小设为 0
            _it = m_DelectBlockArray.erase(_it);  // 从删除块数组中移除当前元素，并更新迭代器

        } else {
            ++_it;  // 如果未删除当前元素，则移动到下一个元素
        }
    }
}

void RtsSceneBlockManager::EvictResources(const Int2& _centerBlock) {
    // 如果已加载的块集合不为空，开始处理
    if (m_blockLoad.size() > 0) {
        set<Int2>::iterator i = m_blockLoad.begin();  // 定义一个迭代器，从第一个加载的块开始遍历

        // 遍历已加载的块集合
        while (i != m_blockLoad.end()) {
            // 根据当前块的坐标获取场景块指针
            RtsSceneBlockRtb* _blockMap = g_pScene->GetBlockMap(i->x, i->y);

            // 如果获取的场景块不存在或状态不是 "已准备好"，跳过此块
            if (!_blockMap || _blockMap->GetState() != Object_State_Ready) {
                ++i;  // 继续下一个块
                continue;
            }

            // 判断当前块与中心块的距离，若超出范围则需要释放
            if (abs(_blockMap->m_blockPosx - _centerBlock.x) > 1 ||
                abs(_blockMap->m_blockPosy - _centerBlock.y) > 1) {
                // 释放场景块资源
                ReleaseSceneBlockImpl(_blockMap);
                // 从已加载集合中移除该块，并记录日志
                i = m_blockLoad.erase(i);
                RtCoreLog().Info("auto release scene block : %s\n", _blockMap->m_szFileName);
            } else {
                // 如果块在范围内，继续检查下一个块
                ++i;
            }
        }
    }
}

SceneBlockArray RtsSceneBlockManager::CreateSceneBlockArray(size_t _size) {
    RTASSERT(_size);
    return SceneBlockArray(RT_NEW RtsSceneBlockRtb[_size], _size);
}

void RtsSceneBlockManager::DestroySceneBlockArray(SceneBlockArray& _blockArray) {
    for (size_t i = 0; i < m_DelectBlockArray.size(); ++i)
        if (m_DelectBlockArray[i] == _blockArray)
            return;
    m_DelectBlockArray.push_back(_blockArray);
}

bool RtsSceneBlockManager::CreateSceneBlock(RtsSceneBlockRtb* _block, const char szPath[],
                                            int blockPosx, int blockPosy,
                                            bool bForceLoad /* = false */
) {
    RTASSERT(_block);
    _block->SetLoadInfo(szPath, blockPosx, blockPosy);

    if (!bForceLoad && m_bMutiThreadLoad) {
        _block->Loack_SetState(Object_State_Loading);
        m_loadLock.lock();
        m_loadQueue.push_back(_block);
        m_loadLock.unlock();
    } else {
        CreateSceneBlockImpl(_block);
        _block->Loack_SetState(Object_State_Ready);
        g_pScene->OnBlockLoadFinish(_block);
#if RTS_DRAW_HELPER
        _block->CreateHelper();
#endif
    }

    m_blockLoad.insert(Int2(_block->m_blockPosx, _block->m_blockPosy));

    return true;
}

bool RtsSceneBlockManager::InitSceneBlock(RtsSceneBlockRtb* _block, const char szPath[],
                                          int blockPosx, int blockPosy) {
    RTASSERT(_block);
    _block->New(szPath, blockPosx, blockPosy);
    m_blockLoad.insert(Int2(_block->m_blockPosx, _block->m_blockPosy));
    return true;
}

void RtsSceneBlockManager::ReleaseSceneBlock(RtsSceneBlockRtb* _block) {
    if (!_block)
        return;

    set<Int2>::iterator it = m_blockLoad.find(Int2(_block->m_blockPosx, _block->m_blockPosy));

    if (it != m_blockLoad.end())
        m_blockLoad.erase(it);

    ReleaseSceneBlockImpl(_block);
}

bool RtsSceneBlockManager::CreateSceneBlockImpl(RtsSceneBlockRtb* _block) {
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

void RtsSceneBlockManager::ReleaseSceneBlockImpl(RtsSceneBlockRtb* _block) {
    RTASSERT(_block);

    if (find(m_loadQueue.begin(), m_loadQueue.end(), _block) != m_loadQueue.end())
        m_releaseQueue.insert(_block);
    else {
        m_cratLock.lock();
        list<RtsSceneBlockRtb*>::iterator _it =
            find(m_creatQueue.begin(), m_creatQueue.end(), _block);
        if (_it != m_creatQueue.end())
            m_creatQueue.erase(_it);
        m_cratLock.unlock();
        _block->Destroy();
    }
}

void RtsSceneBlockManager::Load_Thread(void* _ptMgr) {
    // 将传递的 void* 指针转换为 RtsSceneBlockManager* 类型
    RtsSceneBlockManager* ptMgr = (RtsSceneBlockManager*)_ptMgr;
    // 初始化线程的退出标志为 false
    ptMgr->m_bExited = false;
    // 线程的主循环，直到 m_bExit 标志被设置为 true
    while (!ptMgr->m_bExit) {
        // 如果加载队列为空
        if (ptMgr->m_loadQueue.empty()) {
            Sleep(10);
            continue;  // 继续循环，等待下一个任务
        }

        // 从加载队列中获取第一个 RtsSceneBlockRtb 对象
        RtsSceneBlockRtb* _block = ptMgr->m_loadQueue.front();
        // 处理块对象
        if (ptMgr->CreateSceneBlockImpl(_block)) {
            _block->Loack_SetState(Object_State_Ready);  // 如果处理成功，将块的状态设置为 Ready

            g_pScene->OnBlockLoadFinish(_block);  // 通知场景对象块加载完成

            ptMgr->m_cratLock.lock();
            ptMgr->m_creatQueue.push_back(_block);  // 将处理完成的块对象添加到创建队列中
            ptMgr->m_cratLock.unlock();
        }

        ptMgr->m_loadLock.lock();
        ptMgr->m_loadQueue.pop_front();  // 从加载队列中移除已经处理的块对象
        ptMgr->m_loadLock.unlock();
    }

    ptMgr->m_bExited = true;
}

size_t RtsSceneBlockManager::LoadQueueSize() {
    size_t _size = 0;
    m_loadLock.lock();
    _size = m_loadQueue.size();
    m_loadLock.unlock();
    return _size;
}
