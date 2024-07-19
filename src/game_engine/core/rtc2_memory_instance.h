/**
* @file 内存管理实例类
*     - RtNanoMemAllocatorInstance
*     - RtMemoryAllocatorInstance
* @author janhail
*/

namespace rt2_core {

class RtNanoMemAllocatorInstance : public RtNanoMemAllocator
{
public:
    enum
    {
        eChunkLimit   = 16384,
        eMaxChunkNode = 64,
        eAlignSize    = 8,
        eChunkNumber  = eChunkLimit/eAlignSize,
    };
public:
    RtNanoMemAllocatorInstance();
    virtual ~RtNanoMemAllocatorInstance();
    virtual void *Allocate(size_t size);
    virtual void Free(void *p, size_t size);

private:
    struct SMemoryNode
    {
        SMemoryNode* pNext;
    };
    struct SMemoryChunk
    {
        SMemoryChunk* pNext;
        SMemoryNode*  pData;
    };

private:
    SMemoryNode*    m_listFree[eChunkNumber];
    RtResLock       m_listGuard[eChunkNumber];
    SMemoryChunk*   m_listChunk;
    RtResLock       m_lockChunk;

private:
    static size_t GetChunkIndex(size_t iBytes)
    {
        size_t iIdx=(iBytes-1)/eAlignSize;
        CHECK(iIdx>=0 && iIdx<eChunkNumber);
        return iIdx;
    }
    SMemoryNode* AllocChunk(size_t iIdx);
};


} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
