
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
    m_bExit       = false;
    uintptr_t res = _beginthread(Load_Thread, 0, (void*)this);
    RTASSERT(res != 1L);
    return true;
}

void RtsSceneBlockManager::FrameMove() {
    RtsSceneBlockMap* _blockMap = NULL;  // ����һ��ָ�� RtsSceneBlockMap �����ָ�룬��ʼΪ NULL

    m_cratLock.lock();          // ���������� m_creatQueue ���е��̰߳�ȫ����
    if (!m_creatQueue.empty())  // ��鴴�������Ƿ�ǿ�
    {
        _blockMap = m_creatQueue.front();  // ��ȡ�������еĵ�һ��Ԫ��
        m_creatQueue.pop_front();          // ����һ��Ԫ�شӴ����������Ƴ�
    }
    m_cratLock.unlock();  // ���������������̷߳��� m_creatQueue

    if (_blockMap)  // ����ɹ���ȡ��һ�� RtsSceneBlockMap ����
    {
        set<RtsSceneBlockMap*>::iterator _it =
            m_releaseQueue.find(_blockMap);  // ���ͷŶ����в��Ҹö����ָ��

        if (_it != m_releaseQueue.end())  // ������ͷŶ������ҵ��˸ö���
        {
            (*_it)->Destroy();  // ���ö���� Destroy() ������ִ�ж����������
            m_releaseQueue.erase(_it);  // ���ͷŶ������Ƴ��ö���
        } else                          // ����ö������ͷŶ�����
        {
#if RTS_DRAW_HELPER
            _blockMap->CreateHelper();  // ��������� RTS_DRAW_HELPER������� CreateHelper() ����
#endif
        }
    }

    vector<SceneBlockArray>::iterator _it = m_DelectBlockArray.begin();  // ��ȡɾ��������ĵ�����

    while (_it != m_DelectBlockArray.end())  // ����ɾ���������е�����Ԫ��
    {
        SceneBlockArray& _ba = (*_it);  // ��ȡ��ǰ�� SceneBlockArray ����
        size_t           j   = 0;       // ��ʼ������ j

        while (j < _ba.size())  // ������ǰ�������е����п�
        {
            if (_ba[j].GetState() != Object_State_New)  // �����ǰ���״̬�����´���״̬
                break;                                  // �˳�ѭ��
            ++j;                                        // ������������һ����
        }

        if (j == _ba.size())  // ������п��״̬�����´���״̬
        {
            DEL_ARRAY(_ba._blockArray);  // ɾ�������飬�ͷ��ڴ�
            _ba._size = 0;               // �������С��Ϊ 0
            _it = m_DelectBlockArray.erase(_it);  // ��ɾ�����������Ƴ���ǰԪ�أ������µ�����
        } else {
            ++_it;  // ���δɾ����ǰԪ�أ����ƶ�����һ��Ԫ��
        }
    }
}

void RtsSceneBlockManager::EvictResources(const Int2& _centerBlock) {
    if (m_blockLoad.size() > 0) {
        set<Int2>::iterator i = m_blockLoad.begin();

        while (i != m_blockLoad.end()) {
            RtsSceneBlockMap* _blockMap = g_pScene->GetBlockMap(i->x, i->y);

            if (!_blockMap || _blockMap->GetState() != Object_State_Ready) {
                ++i;
                continue;
            }

            if (abs(_blockMap->m_blockPosx - _centerBlock.x) > 1 ||
                abs(_blockMap->m_blockPosy - _centerBlock.y) > 1) {
                ReleaseSceneBlockImpl(_blockMap);
                i = m_blockLoad.erase(i);
                RtCoreLog().Info("auto release scene block : %s\n", _blockMap->m_szFileName);
            } else {
                ++i;
            }
        }
    }
}

SceneBlockArray RtsSceneBlockManager::CreateSceneBlockArray(size_t _size) {
    RTASSERT(_size);
    return SceneBlockArray(RT_NEW RtsSceneBlockMap[_size], _size);
}

void RtsSceneBlockManager::DestroySceneBlockArray(SceneBlockArray& _blockArray) {
    for (size_t i = 0; i < m_DelectBlockArray.size(); ++i)
        if (m_DelectBlockArray[i] == _blockArray)
            return;
    m_DelectBlockArray.push_back(_blockArray);
}

bool RtsSceneBlockManager::CreateSceneBlock(RtsSceneBlockMap* _block, const char szPath[],
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

bool RtsSceneBlockManager::InitSceneBlock(RtsSceneBlockMap* _block, const char szPath[],
                                          int blockPosx, int blockPosy) {
    RTASSERT(_block);
    _block->New(szPath, blockPosx, blockPosy);
    m_blockLoad.insert(Int2(_block->m_blockPosx, _block->m_blockPosy));
    return true;
}

void RtsSceneBlockManager::ReleaseSceneBlock(RtsSceneBlockMap* _block) {
    if (!_block)
        return;

    set<Int2>::iterator it = m_blockLoad.find(Int2(_block->m_blockPosx, _block->m_blockPosy));

    if (it != m_blockLoad.end())
        m_blockLoad.erase(it);

    ReleaseSceneBlockImpl(_block);
}

bool RtsSceneBlockManager::CreateSceneBlockImpl(RtsSceneBlockMap* _block) {
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

void RtsSceneBlockManager::ReleaseSceneBlockImpl(RtsSceneBlockMap* _block) {
    RTASSERT(_block);

    if (find(m_loadQueue.begin(), m_loadQueue.end(), _block) != m_loadQueue.end())
        m_releaseQueue.insert(_block);
    else {
        m_cratLock.lock();
        list<RtsSceneBlockMap*>::iterator _it =
            find(m_creatQueue.begin(), m_creatQueue.end(), _block);
        if (_it != m_creatQueue.end())
            m_creatQueue.erase(_it);
        m_cratLock.unlock();
        _block->Destroy();
    }
}

void RtsSceneBlockManager::Load_Thread(void* _ptMgr) {
    RtsSceneBlockManager* ptMgr = (RtsSceneBlockManager*)_ptMgr;
    ptMgr->m_bExited            = false;

    while (!ptMgr->m_bExit) {
        if (ptMgr->m_loadQueue.empty()) {
            Sleep(10);
            continue;
        }

        RtsSceneBlockMap* _block = ptMgr->m_loadQueue.front();

        if (ptMgr->CreateSceneBlockImpl(_block)) {
            _block->Loack_SetState(Object_State_Ready);
            g_pScene->OnBlockLoadFinish(_block);

            ptMgr->m_cratLock.lock();
            ptMgr->m_creatQueue.push_back(_block);
            ptMgr->m_cratLock.unlock();
        }

        ptMgr->m_loadLock.lock();
        ptMgr->m_loadQueue.pop_front();
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
