
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_scene_block_manager.h
//     Author : zxy
// CreateTime : 2010.05.01
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _Rts_SceneBlockManager_H_
#define _Rts_SceneBlockManager_H_

struct RtsSceneBlockRtb;

class RtsSceneBlockManager
{

public :
    bool Init();
    void FrameMove();

    bool CreateSceneBlock(RtsSceneBlockRtb* _block, 
                          const char szPath[], 
                          int blockPosx, 
                          int blockPosy, 
                          bool bForceLoad = false);
    bool InitSceneBlock(RtsSceneBlockRtb* _block, 
                        const char szPath[], 
                        int blockPosx,
                        int blockPosy);
    

    void ReleaseSceneBlock(RtsSceneBlockRtb* _block);
    void EvictResources(const Int2& _centerBlock);

    SceneBlockArray CreateSceneBlockArray(size_t _size);
    void DestroySceneBlockArray(SceneBlockArray& _blockArray);

    size_t LoadQueueSize();

    RtsSceneBlockManager();
    virtual ~RtsSceneBlockManager();

    void EanbleMutiThreadLoad(bool bEnable = true)
    {
        m_bMutiThreadLoad = bEnable;
    }

private :
    static void Load_Thread(void* _ptMgr);
    bool CreateSceneBlockImpl(RtsSceneBlockRtb* _block);
    void ReleaseSceneBlockImpl(RtsSceneBlockRtb* _block);

protected :
    list<RtsSceneBlockRtb*> m_loadQueue;//�洢���ڵȴ����صĳ�����
    set<RtsSceneBlockRtb*>  m_releaseQueue;//�洢���ͷŵ� RtsSceneBlockRtb ����
    list<RtsSceneBlockRtb*> m_creatQueue;//�洢������ɵ� RtsSceneBlockRtb ����
    set<Int2>               m_blockLoad;//�洢�Ѿ����صĿ��λ������
    vector<SceneBlockArray> m_DelectBlockArray;//�洢��� ��ɾ���� RtsSceneBlockArray ����

    thread_lock m_loadLock;
    thread_lock m_cratLock;
    bool m_bExit;
    bool m_bExited;
    bool m_bMutiThreadLoad;
};

#endif
