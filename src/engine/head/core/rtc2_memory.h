/**
* @file rtc2_memory.h 内存管理.
* @author janhail
*/

namespace rt2_core {
////////////////////////////////////////////////////////////////////////

#define PADING_SIZE 32

#ifdef ENABLE_MEMORY_ALLOCATOR_DEBUG
	struct SMemBlock
	{
		char		strNameBuffer[32];
		SMemBlock*  pPriv;
		SMemBlock*  pNext;
		int         iMUID;
		size_t      iSize;
		const char* pFile;
		int         iLine;
#ifdef _DEBUG
		char*       pkPadding;
		int         nPadingSize;
#endif
	};
#else
	struct SMemBlock
	{
		size_t      iSize;
	};
#endif


/** 静态内存保护器.
*    - 对静态、全局、类成员buf进行登记
*    - 在这些内存的尾部放置雷区
*/
class RtStaticMemProtector
{
protected:
	RtStaticMemProtector();
	static void CreateInstance();
	static RtStaticMemProtector*   s_pInstance;
	static RtResLock            s_lockSingleton;
	static bool                 s_bSingletonDestroyed;
public:
	/// 创建单键
	static RtStaticMemProtector& Instance()
	{
		if (!s_pInstance)
		{
			CreateInstance();
		}
		return *s_pInstance;
	}

	~RtStaticMemProtector();


	size_t GetMemBlockSize(void* p);
	bool AddMemBlock(void* p, size_t iSize, const char* szFileName, int nLine);
	bool RemoveMemBlock(void* p);
	void CheckAll();
private:
	void OnAddBlock(SMemBlock* pBlock, const char* szFileName, int nLine);

	std::map<void*, SMemBlock> m_AddrToMemBlock;
	RtResLock   m_lock;

#ifdef _DEBUG
	char        m_akBoundCheak[ PADING_SIZE ];	// Use for memory bounding check.
#endif

};

/** 小内存分配器.
*    - 大量的小内存会导致系统产生大量的内存碎片和占用大量的内存分配时间和空间
*    - 这里使用小内存块的方法来解决这个问题，一次会申请多个内存块，类似大小的小内存会聚集在一起。
*      如果释放内存则会回收，有新的分配就只从缓冲区中取出
*    - 如果要分配内存，请不要直接使用这个类，如果要求的内存块过大，这个类可能返回失败，请使用RtMemoryAllocator分配内存
*/
class RtNanoMemAllocator
{
protected:
    RtNanoMemAllocator();
    static void CreateInstance();
    static RtNanoMemAllocator*  s_pInstance;
    static RtResLock            s_lockSingleton;
    static bool                 s_bSingletonDestroyed;
public:
    /// 创建单键
    static RtNanoMemAllocator& Instance()
    {
        if (!s_pInstance)
        {
            CreateInstance();
        }
        return *s_pInstance;
    }
    virtual ~RtNanoMemAllocator(){}
    virtual void *Allocate(size_t iSize) = 0;
    virtual void Free(void *p, size_t iSize) = 0;

public:
    int iTotalAllocate;
    int iTotalFree;
    int iCurrentAllocate;
    int iTotalBlock;
};

////////////////////////////////////////////////////////////////////////

/* for copy
inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
*/

/** 内存分配器.
*    - 比较小的内存在内部会使用缓冲区进行管理，方法请看RtNanoMemAllocator的描述
*    - 如果存在宏 ENABLE_MEMORY_ALLOCATOR_DEBUG，就会为每个分配的内存块标记一个属性，用来检查内存泄露
*    - 建议在一些非常常用的类中重载new和delete，并且使用这个类来管理内存，使用方法如下<BR>
*      @code
*       class CYourClass
*       {
*           ...
*       public:
*           inline void* operator new(size_t iSize)
*           {
*               return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
*           }
*           inline void operator delete(void *p)
*           {
*               return RtMemoryAllocator::Instance().Free(p);
*           }
*           inline void* operator new[](size_t iSize)
*           {
*               return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
*           }
*           inline void operator delete[](void* p)
*           {
*               return RtMemoryAllocator::Instance().Free(p);
*           }
*           ...
*       };
*      @endcode
*/
class RtMemoryAllocator
{
protected:
    friend class RtCore;
    RtMemoryAllocator();
    static void CreateInstance();
    static RtMemoryAllocator*   s_pInstance;
    static RtResLock            s_lockSingleton;
    static bool                 s_bSingletonDestroyed;
public:
    /// 创建单键
    static RtMemoryAllocator& Instance()
    {
        if (!s_pInstance)
        {
            CreateInstance();
        }
        return *s_pInstance;
    }
    ~RtMemoryAllocator();

    /// 分配内存
    void* Alloc(size_t iSize);
    void* Alloc(size_t iSize, const char* szFileName, int nLine);

    /// 释放内存
    void Free(void* p);
    void Free(void* p, const char* szFileName, int nLine);

    void SetDumpMemLeak(bool bEnable);

    void SetLeakBreak(bool bEnable);

    /// 设置断点，当应用分配到第iMUID块内存时会跳到断点
    void SetBreak(int iMUID);

    /// 检查内存泄露、越界
    void CheckAll();
	size_t GetMemSize(void* p);//取得单个内存块大小

private:
#ifdef ENABLE_MEMORY_ALLOCATOR_DEBUG
    int         m_iMUID;
    RtResLock   m_lock;
    int         m_iBreakMUID;
    SMemBlock*  m_pList;
    bool        m_bDumpLeakInfo;
    bool        m_bBreakLeak;

	DWORD		m_dwCount;
	DWORD		m_dwTotalCheckTime;
	DWORD		m_dwCheckedTime;
	SMemBlock*  m_pLastCheckBlock;

#ifdef _DEBUG
	char        m_akBoundCheak[ PADING_SIZE ];	// Use for memory bounding check.
#endif
#endif

private:
    void OnMallocBlock(SMemBlock* pBlock, const char* szFileName, int nLine);
    void OnFreeBlock(SMemBlock* pBlock);
	bool IsStackBuf(void* p);
};


} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
