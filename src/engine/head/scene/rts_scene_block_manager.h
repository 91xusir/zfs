
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

struct RtsSceneBlockMap;

class RtsSceneBlockManager
{

public :
    bool Init();
    void FrameMove();

    bool CreateSceneBlock(RtsSceneBlockMap* _block, 
                          const char szPath[], 
                          int blockPosx, 
                          int blockPosy, 
                          bool bForceLoad = false);
    bool InitSceneBlock(RtsSceneBlockMap* _block, 
                        const char szPath[], 
                        int blockPosx,
                        int blockPosy);
    

    void ReleaseSceneBlock(RtsSceneBlockMap* _block);
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
    bool CreateSceneBlockImpl(RtsSceneBlockMap* _block);
    void ReleaseSceneBlockImpl(RtsSceneBlockMap* _block);

protected :
    list<RtsSceneBlockMap*> m_loadQueue;
    set<RtsSceneBlockMap*>  m_releaseQueue;
    list<RtsSceneBlockMap*> m_creatQueue;
    set<Int2>               m_blockLoad;
    vector<SceneBlockArray> m_DelectBlockArray;

    thread_lock m_loadLock;
    thread_lock m_cratLock;
    bool m_bExit;
    bool m_bExited;
    bool m_bMutiThreadLoad;
};

#endif
