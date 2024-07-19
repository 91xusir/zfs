/**
* @file rtc2_memory.h �ڴ����.
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


/** ��̬�ڴ汣����.
*    - �Ծ�̬��ȫ�֡����Աbuf���еǼ�
*    - ����Щ�ڴ��β����������
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
	/// ��������
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

/** С�ڴ������.
*    - ������С�ڴ�ᵼ��ϵͳ�����������ڴ���Ƭ��ռ�ô������ڴ����ʱ��Ϳռ�
*    - ����ʹ��С�ڴ��ķ��������������⣬һ�λ��������ڴ�飬���ƴ�С��С�ڴ��ۼ���һ��
*      ����ͷ��ڴ������գ����µķ����ֻ�ӻ�������ȡ��
*    - ���Ҫ�����ڴ棬�벻Ҫֱ��ʹ������࣬���Ҫ����ڴ������������ܷ���ʧ�ܣ���ʹ��RtMemoryAllocator�����ڴ�
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
    /// ��������
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

/** �ڴ������.
*    - �Ƚ�С���ڴ����ڲ���ʹ�û��������й��������뿴RtNanoMemAllocator������
*    - ������ں� ENABLE_MEMORY_ALLOCATOR_DEBUG���ͻ�Ϊÿ��������ڴ����һ�����ԣ���������ڴ�й¶
*    - ������һЩ�ǳ����õ���������new��delete������ʹ��������������ڴ棬ʹ�÷�������<BR>
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
    /// ��������
    static RtMemoryAllocator& Instance()
    {
        if (!s_pInstance)
        {
            CreateInstance();
        }
        return *s_pInstance;
    }
    ~RtMemoryAllocator();

    /// �����ڴ�
    void* Alloc(size_t iSize);
    void* Alloc(size_t iSize, const char* szFileName, int nLine);

    /// �ͷ��ڴ�
    void Free(void* p);
    void Free(void* p, const char* szFileName, int nLine);

    void SetDumpMemLeak(bool bEnable);

    void SetLeakBreak(bool bEnable);

    /// ���öϵ㣬��Ӧ�÷��䵽��iMUID���ڴ�ʱ�������ϵ�
    void SetBreak(int iMUID);

    /// ����ڴ�й¶��Խ��
    void CheckAll();
	size_t GetMemSize(void* p);//ȡ�õ����ڴ���С

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
