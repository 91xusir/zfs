
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
    list<RtsSceneBlockRtb*> m_loadQueue;//存储正在等待加载的场景块
    set<RtsSceneBlockRtb*>  m_releaseQueue;//存储待释放的 RtsSceneBlockRtb 对象
    list<RtsSceneBlockRtb*> m_creatQueue;//存储创建完成的 RtsSceneBlockRtb 对象
    set<Int2>               m_blockLoad;//存储已经加载的块的位置坐标
    vector<SceneBlockArray> m_DelectBlockArray;//存储多个 待删除的 RtsSceneBlockArray 对象

    thread_lock m_loadLock;
    thread_lock m_cratLock;
    bool m_bExit;
    bool m_bExited;
    bool m_bMutiThreadLoad;
};

#endif
