
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

    // ��ʼ��
    BOOL  InitStreamManager(DWORD nTotalDynamicStream, DWORD* nDynamicSize, DWORD nTotalStaticStream, DWORD* nStaticSize, DWORD nTotalDynamicEntry, DWORD nTotalStaticEntry);

    // ��ʼ�ϴ�����
    //      dwResID     Ϊ��Դ��ID
    //      dwSize      Ϊ��Դ�Ĵ�С(��λ���ֽ�)
    //      bResident   �Ƿ�פ���Դ�, �������᷵��dwResID, �����פ����������Bind��ʱ����(0, 0)�Ϳ���
    //      dwStride    ÿ������(����)�Ĵ�С(��λ���ֽ�)
    //      ����һ��ָ�룬����Զ����ָ��д������
    void* BeginUpload(DWORD& dwResID, DWORD dwSize, BOOL bResident, DWORD dwStride);
    BOOL  EndUpload(DWORD& dwResID);
    BOOL  Upload(DWORD& dwResID, BYTE* pResource, DWORD dwSize, BOOL bResident, DWORD dwStride);

    void  Clear(DWORD dwResID);
    void  ClearAll();

    void  OnBeginRender();
    void  OnEndRender();

    // ����Դ�󶨸��豸������VB��ID�������������в�ͬ�Ľ���
    //  VertexBuffer
    //      dwChannel   ��������ţ�ͨ����0
    //      dwResID     ��Դ���
    //  IndexBuffer
    //      dwChannel   VertexBuffer����Դ���
    //      dwResID     ��Դ���
    // ����ֵ��ʾ�Ƿ�ɹ�
    // ���ǰ��Upload���Ƕ�̬����(bResident=FALSE)����ô������Զ�����0 (�� Bind(0, 0))
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
