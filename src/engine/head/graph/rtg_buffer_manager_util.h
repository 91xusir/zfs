
namespace rt_graph {

/*----------------------------------------------------------------------------
-   class RtgDynamicStreamManager.
----------------------------------------------------------------------------*/

class GRAPH_API RtgDynamicStreamManager
{
public:
    friend class RtgRenderStreamManager;
    struct SDynamicStream
    {
        DWORD       dwTotalSize;    // 这个Buffer的内存大小
        DWORD       dwFreeSize;     // 剩下没有使用的空间的大小
        DWORD       dwActiveEntry;  // 在这个空间中有多少块 (只是统计用)
#ifndef _DEBUG
        inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
        inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
    };

public:

    SDynamicStream*     m_pBuffers;         // 缓冲区

    DWORD               m_dwTotalStream;    // 总共的缓冲区数
    DWORD               m_dwTotalMemory;
    DWORD               m_dwUsedMemory;

    DWORD               m_dwActiveEntry;
    DWORD               m_dwLastestStream;

public:

    RtgDynamicStreamManager();
    ~RtgDynamicStreamManager();

    void InitStreamManager(DWORD dwTotalStream, DWORD* pdwSize);

    BOOL QuerySpace(DWORD dwInSize, DWORD& dwOutBufferID, DWORD& dwOutOffset);
    DWORD ClearOneBuffer();
    DWORD ClearSizeBuffer(DWORD dwMinSize);
    void ClearAllBuffer();
    SDynamicStream* GetBuffer(DWORD dwBufferID);

    // 返回False表示没有加进去, 否则是成功
    BOOL AddEntry(DWORD dwBufferID, DWORD dwSize, DWORD& dwInOutOffset, DWORD dwStride);

    DWORD GetTotalBuffer()            { return m_dwTotalStream;    }
    DWORD GetActiveEntry()            { return m_dwActiveEntry;    }

};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
