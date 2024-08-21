
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
    m_bExit = false;  // �� m_bExit ��־��Ϊ false����ʾ��ǰ������δ�˳�

    // ����һ�����̣߳�ִ�� Load_Thread ����
    // _beginthread �������ڴ���������һ���߳�
    // �̵߳���ں����� Load_Thread
    // �̵߳Ķ�ջ��С��Ϊ 0��ʹ��Ĭ�ϴ�С��
    // ���ݵĲ����ǵ�ǰ�����ָ�� (this)
    uintptr_t res = _beginthread(Load_Thread, 0, (void*)this);
    RTASSERT(res != 1L);
    return true;
}

void RtsSceneBlockManager::FrameMove() {

    RtsSceneBlockRtb* _blockMap = nullptr;
    // �����������������е�Ԫ��
    m_cratLock.lock();
    if (!m_creatQueue.empty()) {
        _blockMap = m_creatQueue.front();  // ��ȡ�������еĵ�һ��Ԫ��
        m_creatQueue.pop_front();          // ����һ��Ԫ�شӴ����������Ƴ�
    }
    m_cratLock.unlock();
    // ������ͷŶ����еĶ���
    if (_blockMap) {
        const auto _it = m_releaseQueue.find(_blockMap);  // ���ͷŶ����в��Ҹö����ָ��
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

    // ����ɾ���������е�Ԫ��
    auto _it = m_DelectBlockArray.begin();
    while (_it != m_DelectBlockArray.end()) {
        SceneBlockArray& _ba = (*_it);
        size_t           j   = 0;

        while (j < _ba.size())  // ������ǰ�������е����п�
        {
            if (_ba[j].GetState() != Object_State_New)  // �����ǰ���״̬�����´���״̬
                break;                                  // �˳�ѭ��
            ++j;                                        // ������������һ����
        }

        if (j == _ba.size())  // ������п��״̬�����´���״̬
        {
            //lyytodo ������ͷŴ�����  ��ע��
            //DEL_ARRAY(_ba._blockArray);
            _ba._size = 0;                        // �������С��Ϊ 0
            _it = m_DelectBlockArray.erase(_it);  // ��ɾ�����������Ƴ���ǰԪ�أ������µ�����

        } else {
            ++_it;  // ���δɾ����ǰԪ�أ����ƶ�����һ��Ԫ��
        }
    }
}

void RtsSceneBlockManager::EvictResources(const Int2& _centerBlock) {
    // ����Ѽ��صĿ鼯�ϲ�Ϊ�գ���ʼ����
    if (m_blockLoad.size() > 0) {
        set<Int2>::iterator i = m_blockLoad.begin();  // ����һ�����������ӵ�һ�����صĿ鿪ʼ����

        // �����Ѽ��صĿ鼯��
        while (i != m_blockLoad.end()) {
            // ���ݵ�ǰ��������ȡ������ָ��
            RtsSceneBlockRtb* _blockMap = g_pScene->GetBlockMap(i->x, i->y);

            // �����ȡ�ĳ����鲻���ڻ�״̬���� "��׼����"�������˿�
            if (!_blockMap || _blockMap->GetState() != Object_State_Ready) {
                ++i;  // ������һ����
                continue;
            }

            // �жϵ�ǰ�������Ŀ�ľ��룬��������Χ����Ҫ�ͷ�
            if (abs(_blockMap->m_blockPosx - _centerBlock.x) > 1 ||
                abs(_blockMap->m_blockPosy - _centerBlock.y) > 1) {
                // �ͷų�������Դ
                ReleaseSceneBlockImpl(_blockMap);
                // ���Ѽ��ؼ������Ƴ��ÿ飬����¼��־
                i = m_blockLoad.erase(i);
                RtCoreLog().Info("auto release scene block : %s\n", _blockMap->m_szFileName);
            } else {
                // ������ڷ�Χ�ڣ����������һ����
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
    // �����ݵ� void* ָ��ת��Ϊ RtsSceneBlockManager* ����
    RtsSceneBlockManager* ptMgr = (RtsSceneBlockManager*)_ptMgr;
    // ��ʼ���̵߳��˳���־Ϊ false
    ptMgr->m_bExited = false;
    // �̵߳���ѭ����ֱ�� m_bExit ��־������Ϊ true
    while (!ptMgr->m_bExit) {
        // ������ض���Ϊ��
        if (ptMgr->m_loadQueue.empty()) {
            Sleep(10);
            continue;  // ����ѭ�����ȴ���һ������
        }

        // �Ӽ��ض����л�ȡ��һ�� RtsSceneBlockRtb ����
        RtsSceneBlockRtb* _block = ptMgr->m_loadQueue.front();
        // ��������
        if (ptMgr->CreateSceneBlockImpl(_block)) {
            _block->Loack_SetState(Object_State_Ready);  // �������ɹ��������״̬����Ϊ Ready

            g_pScene->OnBlockLoadFinish(_block);  // ֪ͨ���������������

            ptMgr->m_cratLock.lock();
            ptMgr->m_creatQueue.push_back(_block);  // ��������ɵĿ������ӵ�����������
            ptMgr->m_cratLock.unlock();
        }

        ptMgr->m_loadLock.lock();
        ptMgr->m_loadQueue.pop_front();  // �Ӽ��ض������Ƴ��Ѿ�����Ŀ����
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
