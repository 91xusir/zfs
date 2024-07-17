
namespace rt_graph {

/*----------------------------------------------------------------------------
-   class RtgRenderStreamManager.
----------------------------------------------------------------------------*/

class GRAPH_API RtgRenderStreamManager
{
public:
    RtgRenderStreamManager(RtgDevice* pDevice);
    virtual ~RtgRenderStreamManager();
#ifndef _DEBUG
    inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
    inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif

    // 初始化
    BOOL  InitStreamManager(DWORD nTotalDynamicStream, DWORD* nDynamicSize, DWORD nTotalStaticStream, DWORD* nStaticSize, DWORD nTotalDynamicEntry, DWORD nTotalStaticEntry);

    // 开始上传数据
    //      dwResID     为资源的ID
    //      dwSize      为资源的大小(单位是字节)
    //      bResident   是否驻留显存, 如果是则会返回dwResID, 如果不驻留则后面调用Bind的时候传入(0, 0)就可以
    //      dwStride    每个顶点(索引)的大小(单位是字节)
    //      返回一个指针，你可以对这个指针写入数据
    void* BeginUpload(DWORD& dwResID, DWORD dwSize, BOOL bResident, DWORD dwStride);
    BOOL  EndUpload(DWORD& dwResID);
    BOOL  Upload(DWORD& dwResID, BYTE* pResource, DWORD dwSize, BOOL bResident, DWORD dwStride);

    void  Clear(DWORD dwResID);
    void  ClearAll();

    void  OnBeginRender();
    void  OnEndRender();

    // 把资源绑定给设备，对于VB和ID，这两个参数有不同的解释
    //  VertexBuffer
    //      dwChannel   数据流编号，通常传0
    //      dwResID     资源编号
    //  IndexBuffer
    //      dwChannel   VertexBuffer的资源编号
    //      dwResID     资源编号
    // 返回值表示是否成功
    // 如果前面Upload的是动态缓冲(bResident=FALSE)，那么这里可以都传入0 (即 Bind(0, 0))
    virtual BOOL Bind(DWORD dwChannel, DWORD dwResID) = 0;

    virtual BOOL OnCreateStreamManager()  = 0;
    virtual BOOL OnReleaseStreamManager() = 0;
    virtual BOOL OnResizeStream(DWORD dwResID, DWORD dwSize) = 0;

    DWORD GetLastUpdateOffset()     { return m_dwLastUpdateOffset;}
    DWORD GetLastUpdateStride()     { return m_dwLastUpdateStride;}

protected:

    virtual void* LockStream(DWORD dwBufferID, DWORD dwOffset, DWORD dwSize, DWORD bDiscard)=0;
    virtual BOOL  UnlockStream(DWORD dwBufferID)=0;

protected:

    RtgDevice*                  m_pDevice;
    RtgDynamicStreamManager     m_streamMgr;
    RtgDynamicStreamManager*    m_pStreamMgr;
    DWORD                       m_dwLastUpdateBuffferID;
    DWORD                       m_dwLastUpdateStride;
    DWORD                       m_dwLastUpdateOffset;

public:
    // for STAT.

    DWORD           m_dwStatDynamicSend;
    DWORD           m_dwStatDynamicClear;
    DWORD           m_dwStatDynamicSwap;

    DWORD           GetTotalActiveEntry();

    DWORD           GetTotalStaticMemory()      { return 0; }
    DWORD           GetTotalDynamicMemory()     { return m_pStreamMgr->m_dwTotalMemory; }
    DWORD           GetUsedStaticMemory()       { return 0; }
    DWORD           GetUsedDynamicMemory()      { return m_pStreamMgr->m_dwUsedMemory; }

};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
