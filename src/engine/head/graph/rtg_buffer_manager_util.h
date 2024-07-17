
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
        DWORD       dwTotalSize;    // ���Buffer���ڴ��С
        DWORD       dwFreeSize;     // ʣ��û��ʹ�õĿռ�Ĵ�С
        DWORD       dwActiveEntry;  // ������ռ����ж��ٿ� (ֻ��ͳ����)
#ifndef _DEBUG
        inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
        inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
    };

public:

    SDynamicStream*     m_pBuffers;         // ������

    DWORD               m_dwTotalStream;    // �ܹ��Ļ�������
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

    // ����False��ʾû�мӽ�ȥ, �����ǳɹ�
    BOOL AddEntry(DWORD dwBufferID, DWORD dwSize, DWORD& dwInOutOffset, DWORD dwStride);

    DWORD GetTotalBuffer()            { return m_dwTotalStream;    }
    DWORD GetActiveEntry()            { return m_dwActiveEntry;    }

};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
