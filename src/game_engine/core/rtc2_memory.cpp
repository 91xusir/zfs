#include "core/rt2_core.h"
#include "rtc2_memory_instance.h"

namespace rt2_core {

	/*-----------------------------------------------------------------------------
	-   RtNanoMemAllocator
	-----------------------------------------------------------------------------*/

	RtNanoMemAllocator* RtNanoMemAllocator::s_pInstance = NULL;
	RtResLock            RtNanoMemAllocator::s_lockSingleton;
	bool                 RtNanoMemAllocator::s_bSingletonDestroyed = false;;

	static bool gbStaticInit = false;

	RtNanoMemAllocator::RtNanoMemAllocator() :
		iTotalAllocate(0),
		iTotalFree(0),
		iCurrentAllocate(0),
		iTotalBlock(0)
	{
	}

	void RtNanoMemAllocator::CreateInstance()
	{
		RtResLock::AutoLock lock(s_lockSingleton);
		if (s_pInstance)
			return;
		RTASSERT(s_bSingletonDestroyed == false); // 这里不能用CHECK，因为CHECK会有信息输出，而信息输出又依赖于Core，Core在这时候可能还没有初始化
		static RtNanoMemAllocatorInstance   allocatorNanoMem;
		s_pInstance = &allocatorNanoMem;
	}

	/*-----------------------------------------------------------------------------
	-   RtNanoMemAllocatorInstance
	-----------------------------------------------------------------------------*/

	RtNanoMemAllocatorInstance::RtNanoMemAllocatorInstance()
	{
		m_listChunk = 0;
		::memset(m_listFree, 0, sizeof(m_listFree));
		//::memset(m_listGuard, 0, sizeof(m_listGuard));
	}

	RtNanoMemAllocatorInstance::~RtNanoMemAllocatorInstance()
	{
		int s = 0;
		SMemoryChunk* temp = m_listChunk;
		while (temp)
		{
			++s;
			temp = temp->pNext;
		}

		void** chunk = reinterpret_cast<void**>(malloc(s * sizeof(void*)));
		temp = m_listChunk;
		int i = 0;
		while (temp)
		{
			chunk[i] = temp->pData;
			++i;
			temp = temp->pNext;
		}
		for (i = 0; i < s; i++)
		{
			::free(chunk[i]);
		}
		::free(chunk);

		iCurrentAllocate = 0;
		iTotalBlock = 0;
	}

	RtNanoMemAllocatorInstance::SMemoryNode* RtNanoMemAllocatorInstance::AllocChunk(size_t iIdx)
	{
		const size_t iNodeSize = (iIdx + 1) * eAlignSize;
		const size_t iChunkSize = min(eChunkLimit / iNodeSize * iNodeSize, iNodeSize * eMaxChunkNode);
		RtResLock::AutoLock autoLock(m_lockChunk);
		SMemoryNode*& pCurrentList = m_listFree[iIdx];
		if (pCurrentList)
			return pCurrentList;
		iTotalBlock++;
		SMemoryNode* ret = pCurrentList = reinterpret_cast<SMemoryNode*>(::malloc(iChunkSize));
		SMemoryNode* iter = ret;
		for (size_t i = 0; i + iNodeSize < iChunkSize; i += iNodeSize)
		{
			iter->pNext = (SMemoryNode*)(((unsigned char*)iter) + iNodeSize);
			iter = iter->pNext;
		}
		iter->pNext = 0;
		return ret;
	}

	void* RtNanoMemAllocatorInstance::Allocate(size_t iSize)
	{
		size_t iIdx = GetChunkIndex(iSize);
		CHECK(iIdx < eChunkNumber);
		RtResLock::AutoLock autoLock(m_listGuard[iIdx]);
		SMemoryNode*& temp = m_listFree[iIdx];
		if (!temp)
		{
			SMemoryNode* pNewChunk = AllocChunk(iIdx);
			SMemoryChunk* pChunkNode;
			if (GetChunkIndex(sizeof(SMemoryChunk)) == iIdx)
			{
				pChunkNode = reinterpret_cast<SMemoryChunk*>(temp);
				temp = temp->pNext;
			}
			else
			{
				pChunkNode = reinterpret_cast<SMemoryChunk*>(Allocate(sizeof(SMemoryChunk)));
			}
			RtResLock::AutoLock autoLock(m_lockChunk);
			pChunkNode->pNext = m_listChunk;
			pChunkNode->pData = pNewChunk;
			m_listChunk = pChunkNode;
		}
		iTotalAllocate++;
		iCurrentAllocate++;
		void* ret = temp;
		temp = temp->pNext;
		return ret;
	}

	void RtNanoMemAllocatorInstance::Free(void* p, size_t iSize)
	{
		if (iSize == 0)
			return;
		size_t iIdx = GetChunkIndex(iSize);
		CHECK(iIdx < eChunkNumber);
		SMemoryNode* pFreeBlock = reinterpret_cast<SMemoryNode*>(p);
		RtResLock::AutoLock autoLock(m_listGuard[iIdx]);
		SMemoryNode*& temp = m_listFree[iIdx];
		pFreeBlock->pNext = temp;
		temp = pFreeBlock;
		iTotalFree++;
		iCurrentAllocate--;
	}


	////////////////////////////////////////////////////////////////////////

	RtMemoryAllocator* RtMemoryAllocator::s_pInstance = 0;
	RtResLock            RtMemoryAllocator::s_lockSingleton;
	bool                 RtMemoryAllocator::s_bSingletonDestroyed = false;


	void RtMemoryAllocator::CreateInstance()
	{
		RtResLock::AutoLock lock(s_lockSingleton);
		if (s_pInstance)
			return;
		RTASSERT(s_bSingletonDestroyed == false); // 这里不能用CHECK，因为CHECK会有信息输出，而信息输出又依赖于Core
		static RtMemoryAllocator            allocatorMemory;
		s_pInstance = &allocatorMemory;
		gbStaticInit = true;
	}

#ifdef ENABLE_MEMORY_ALLOCATOR_DEBUG
	RtMemoryAllocator::RtMemoryAllocator()
	{
		m_iMUID = 1;
		m_iBreakMUID = 0;
#ifdef _DEBUG
		m_bDumpLeakInfo = true;
		m_bBreakLeak = true;
#else
		m_bDumpLeakInfo = false;
		m_bBreakLeak = false;
#endif // _DEBUG
		m_pList = reinterpret_cast<SMemBlock*>(RtCoreNanoMem().Allocate(sizeof(SMemBlock)));
		m_pList->iMUID = 0;
		m_pList->iSize = 0;

		SMemBlock* pTemp = reinterpret_cast<SMemBlock*>(RtCoreNanoMem().Allocate(sizeof(SMemBlock)));
		m_pList->pNext = m_pList->pPriv = pTemp;
		pTemp->iMUID = 0;
		pTemp->iSize = 0;
		pTemp->pNext = pTemp->pPriv = m_pList;
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		memset(m_akBoundCheak, NULL, PADING_SIZE);
#endif
		m_dwCheckedTime = 0;
		m_dwTotalCheckTime = 10;
		m_dwCount = 0;
		m_pLastCheckBlock = NULL;
	}

	RtMemoryAllocator::~RtMemoryAllocator()
	{
		/*
		if (m_pList->pNext==m_pList->pPriv)
		{
			RgCoreLog().Logf("No memory leak\n");
		}else
		{
			RgCoreLog().Logf("Memory leak:\n");
			SMemBlock* pBlock = m_pList->pNext;
			while (pBlock!=m_pList->pPriv)
			{
				RgCoreLog().Logf("MUID=%d, size=%d\n", pBlock->iMUID, pBlock->iSize);
				pBlock = pBlock->pNext;
			}
		}
		*/
		if (m_bDumpLeakInfo)
		{
			// lyymark 由于Log已经Shutdown，所以只能用ANSI文件来记录
			FILE* fp = fopen("memory_leak.txt", "wt");
			if (fp)
			{
				if (m_pList->pNext == m_pList->pPriv)
				{
					fprintf(fp, "No memory leak\n");
				}
				else
				{
					fprintf(fp, "Memory leak:\n");
					SMemBlock* pBlock = m_pList->pNext;
					while (pBlock != m_pList->pPriv)
					{

#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
						int bResult = memcmp(m_akBoundCheak, pBlock->pkPadding, PADING_SIZE);
						if (bResult != 0)
						{
							fprintf(fp, "****THIS MEMORY OUT OF BOUND***   ");
						}
#endif
						fprintf(fp, "MUID=%d, size=%d, File='%s':%d\n", pBlock->iMUID, pBlock->iSize, pBlock->pFile, pBlock->iLine);
						pBlock = pBlock->pNext;
					}
				}
				fclose(fp);
			}
		}

		// 如果有内存泄露，就中断出来。。
		if (m_bBreakLeak)
		{
			if (m_pList->pNext != m_pList->pPriv)
			{
				//rtDebugBreak();
				int ii = 0; // 内存泄露
			}
		}

		RtCoreNanoMem().Free(m_pList, sizeof(SMemBlock));
		RtCoreNanoMem().Free(m_pList->pNext, sizeof(SMemBlock));
	}

	void RtMemoryAllocator::OnMallocBlock(SMemBlock* pBlock, const char* szFileName, int nLine)
	{
		{
			RtResLock::AutoLock autoLock(m_lock);
			rt2_strncpy(pBlock->strNameBuffer, "RtMemoryAllocator-HeapMemBlock", 32);
			pBlock->iMUID = m_iMUID++;
			pBlock->pPriv = m_pList;
			pBlock->pNext = m_pList->pNext;
			if (szFileName)
			{
				pBlock->pFile = szFileName;
			}
			else
			{
				pBlock->pFile = "";
			}
			pBlock->iLine = nLine;
			m_pList->pNext->pPriv = pBlock;
			m_pList->pNext = pBlock;
		}
		CHECK(pBlock->iMUID != m_iBreakMUID);
		m_dwCount++;
	}

	void RtMemoryAllocator::OnFreeBlock(SMemBlock* pBlock)
	{
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		int bResult = memcmp(m_akBoundCheak, pBlock->pkPadding, PADING_SIZE);
		if (bResult != 0)
		{
			RtCoreLog().Error("****THIS MEMORY OUT OF BOUND***\n");
			RtCoreLog().Error("MUID=%d, size=%d, File='%s':%d\n", pBlock->iMUID, pBlock->iSize, pBlock->pFile, pBlock->iLine);
			//rtDebugBreak( );
		}
#endif
		RtResLock::AutoLock autoLock(m_lock);

		if (!pBlock->iMUID) // 如果等于0就说明该内存被释放过了
		{
			RtCoreLog().Error("Block ReFree MUID=%d, size=%d, File='%s':%d\n", pBlock->iMUID, pBlock->iSize, pBlock->pFile, pBlock->iLine);
		}

#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		if (pBlock == m_pLastCheckBlock)
		{
			m_pLastCheckBlock = pBlock->pNext;
		}
#endif
		pBlock->pNext->pPriv = pBlock->pPriv;
		pBlock->pPriv->pNext = pBlock->pNext;
		pBlock->iMUID = 0;
		// 这里没有重新设置iSize的值，如果第二次释放会在前面的CHECK中检查出来，并且可以从iSize看到大小

		if (m_dwCount)//避免重复释放减出一个超大数来
		{
			m_dwCount--;
		}
	}

	void RtMemoryAllocator::SetDumpMemLeak(bool bEnable)
	{
		m_bDumpLeakInfo = bEnable;
	}

	void RtMemoryAllocator::SetLeakBreak(bool bEnable)
	{
		m_bBreakLeak = bEnable;
	}

	void RtMemoryAllocator::SetBreak(int iMUID)
	{
		m_iBreakMUID = iMUID;
	}

	void RtMemoryAllocator::CheckAll()
	{
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		m_dwCheckedTime++;

		SMemBlock* pBlock = NULL;

		if (m_pLastCheckBlock)
		{
			pBlock = m_pLastCheckBlock;
		}
		else
		{
			pBlock = m_pList->pNext;
		}

		DWORD dwCheckCountThisTime = m_dwCount / m_dwTotalCheckTime;
		bool bLastTime = m_dwCheckedTime == m_dwTotalCheckTime;

		while ((pBlock != m_pList->pPriv) && (bLastTime ? 1 : dwCheckCountThisTime)/*如果是最后一次，就不管检查量，一口气到底*/)
		{
			//CHECK(pBlock->iMUID>0 && pBlock->iMUID<m_iMUID);

			int bResult = memcmp(m_akBoundCheak, pBlock->pkPadding, PADING_SIZE);
			if (bResult != 0)
			{
				RtCoreLog().Error("****THIS MEMORY OUT OF BOUND***\n");
				RtCoreLog().Error("MUID=%d, size=%d, File='%s':%d\n", pBlock->iMUID, pBlock->iSize, pBlock->pFile, pBlock->iLine);
				//rtDebugBreak( );
			}

			m_pLastCheckBlock = pBlock = pBlock->pNext;
			dwCheckCountThisTime--;

		}

		//	RtCoreLog().Debug("MEMORY BOUND check %d time!\n", m_dwCheckedTime);

		if (pBlock == m_pList->pPriv)
		{
			//如果到底了，就重新初始化
			m_pLastCheckBlock = NULL;
			m_dwCheckedTime = 0;
			//		RtCoreLog().Debug("MEMORY BOUND check Reach the End!\n");
					//检查静态内存
			RtStaticMemProtector::Instance().CheckAll();
		}
#endif
	}

#else

	RtMemoryAllocator::RtMemoryAllocator()
	{
	}

	RtMemoryAllocator::~RtMemoryAllocator()
	{
	}

	void RtMemoryAllocator::OnMallocBlock(SMemBlock* pBlock, const char* szFileName, int nLine)
	{
	}

	void RtMemoryAllocator::OnFreeBlock(SMemBlock* pBlock)
	{
	}
	void RtMemoryAllocator::SetDumpMemLeak(bool bEnable)
	{
	}
	void RtMemoryAllocator::SetLeakBreak(bool bEnable)
	{
	}
	void RtMemoryAllocator::SetBreak(int iMUID)
	{
	}

	void RtMemoryAllocator::CheckAll()
	{
	}
#endif

	void* RtMemoryAllocator::Alloc(size_t iSize)
	{
		if (iSize == 0)
		{
			return NULL;
		}

#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		size_t     nRealSize = iSize;
#endif
		SMemBlock* pBlock;
		iSize += sizeof(SMemBlock);
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		iSize += PADING_SIZE;
#endif
		if (iSize > 16384)
		{
			pBlock = reinterpret_cast<SMemBlock*>(::malloc(iSize));
		}
		else
		{
			pBlock = reinterpret_cast<SMemBlock*>(RtCoreNanoMem().Allocate(iSize));
		}
		pBlock->iSize = iSize;
		OnMallocBlock(pBlock, NULL, 0);

#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		pBlock->nPadingSize = PADING_SIZE;
		char* pkRealBuffer = (char*)pBlock;
		pBlock->pkPadding = pkRealBuffer + nRealSize + sizeof(SMemBlock);
		memset(pBlock->pkPadding, NULL, PADING_SIZE);
#endif
		return pBlock + 1;
	}

	void* RtMemoryAllocator::Alloc(size_t iSize, const char* szFileName, int nLine)
	{
		if (iSize == 0)
		{
			return NULL;
		}
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		size_t     nRealSize = iSize;
#endif
		SMemBlock* pBlock;
		iSize += sizeof(SMemBlock);
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		iSize += PADING_SIZE;
#endif

		if (iSize > 16384)
		{
			pBlock = reinterpret_cast<SMemBlock*>(::malloc(iSize));
		}
		else
		{
			pBlock = reinterpret_cast<SMemBlock*>(RtCoreNanoMem().Allocate(iSize));
		}
		pBlock->iSize = iSize;
		OnMallocBlock(pBlock, szFileName, nLine);

#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		pBlock->nPadingSize = PADING_SIZE;
		char* pkRealBuffer = (char*)pBlock;
		pBlock->pkPadding = pkRealBuffer + nRealSize + sizeof(SMemBlock);
		memset(pBlock->pkPadding, NULL, PADING_SIZE);
#endif

		return pBlock + 1;
	}

	void RtMemoryAllocator::Free(void* p)
	{
		if (p == 0)
		{
			return;
		}

		SMemBlock* pBlock = (reinterpret_cast<SMemBlock*>(p)) - 1;
		OnFreeBlock(pBlock);

		if (pBlock->iSize > 16384)
		{
			::free(pBlock);
		}
		else
		{
			RtCoreNanoMem().Free(pBlock, pBlock->iSize);
		}
	}

	void RtMemoryAllocator::Free(void* p, const char* szFileName, int nLine)
	{
		if (p == 0)
		{
			return;
		}

		SMemBlock* pBlock = (reinterpret_cast<SMemBlock*>(p)) - 1;
		OnFreeBlock(pBlock);

		if (pBlock->iSize > 16384)
		{
			::free(pBlock);
		}
		else
		{
			RtCoreNanoMem().Free(pBlock, pBlock->iSize);
		}
	}

	size_t RtMemoryAllocator::GetMemSize(void* p)
	{
		if (p == 0)
		{
			return 0;
		}

		SMemBlock* pBlock = (reinterpret_cast<SMemBlock*>(p)) - 1;

		if (!strncmp(pBlock->strNameBuffer, "RtMemoryAllocator-HeapMemBlock", strlen("RtMemoryAllocator-HeapMemBlock")))
		{
			//堆内存
			return (pBlock->iSize - sizeof(SMemBlock) - PADING_SIZE);
		}
		else if (IsStackBuf(p))
		{
			//栈内存
			return 0;
		}
		else
		{
			size_t iSize = RtStaticMemProtector::Instance().GetMemBlockSize(p);
			//静态内存
			if (iSize)
				return iSize;
		}

		RtCoreLog().Error("****You Have user a UnProtect Mem!!***\n");

		RTASSERT(0);
		rtDebugBreak();

		return 0;
	}
	/************************************************************************/
	/* linux与win的堆栈都是逆向生长的，所以判断buf是否在栈上分配的
	只要判断其地址与当前帧的局部变量地址谁大就可以了。这个方法只对本堆栈有效
	如果是主线程堆栈判断其他线程上的buf，可能会出现误判，不过，在线程间传递局部
	buf，这个程序员也太SB了。BTW，win下的堆栈是从很低的地址开始生长的，所以要
	判断是否超出栈底（win默认栈大小1M），linux下从3G的位置开始生长，其他变量
	都比栈变量小*/
	/************************************************************************/
	bool RtMemoryAllocator::IsStackBuf(void* p)
	{
		char* sp;
		char buf[8];


		sp = buf;
#ifdef WIN32
		if (sp > p || sp + 0x100000 < p)
			return false;
#else
		if (sp > p)
			return false;
#endif


		return true;
	}

	/*-----------------------------------------------------------------------------
	-   RtStaticMemProtector
	-----------------------------------------------------------------------------*/


	RtStaticMemProtector* RtStaticMemProtector::s_pInstance = 0;
	RtResLock            RtStaticMemProtector::s_lockSingleton;
	bool                 RtStaticMemProtector::s_bSingletonDestroyed = false;


	void RtStaticMemProtector::CreateInstance()
	{
		RtResLock::AutoLock lock(s_lockSingleton);
		if (s_pInstance)
			return;
		RTASSERT(s_bSingletonDestroyed == false); // 这里不能用CHECK，因为CHECK会有信息输出，而信息输出又依赖于Core
		static RtStaticMemProtector            StaticMemProtector;
		s_pInstance = &StaticMemProtector;
	}
	RtStaticMemProtector::RtStaticMemProtector()
	{
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		memset(m_akBoundCheak, NULL, PADING_SIZE);
#endif
	}

	RtStaticMemProtector::~RtStaticMemProtector()
	{

	}

	size_t RtStaticMemProtector::GetMemBlockSize(void* p)
	{
		RtResLock::AutoLock autoLock(m_lock);
		std::map<void*, SMemBlock>::iterator it = m_AddrToMemBlock.find(p);

		if (it == m_AddrToMemBlock.end())
		{
			return 0;
		}


		return (it->second.iSize - PADING_SIZE);
	}

	bool RtStaticMemProtector::AddMemBlock(void* p, size_t iSize, const char* szFileName, int nLine)
	{
		RtResLock::AutoLock autoLock(m_lock);

		std::map<void*, SMemBlock>::iterator it = m_AddrToMemBlock.find(p);

		if (it != m_AddrToMemBlock.end())
		{
			RTASSERT(0);
			return false;
		}

		SMemBlock MemBlock;
		MemBlock.iSize = iSize + PADING_SIZE;

		OnAddBlock(&MemBlock, szFileName, nLine);


#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		MemBlock.nPadingSize = PADING_SIZE;
		char* pkRealBuffer = (char*)p;
		MemBlock.pkPadding = pkRealBuffer + MemBlock.iSize - PADING_SIZE;
		memset(MemBlock.pkPadding, NULL, PADING_SIZE);
#endif

		m_AddrToMemBlock[p] = MemBlock;
		return true;
	}

	bool RtStaticMemProtector::RemoveMemBlock(void* p)
	{
		RtResLock::AutoLock autoLock(m_lock);

		std::map<void*, SMemBlock>::iterator it = m_AddrToMemBlock.find(p);

		if (it == m_AddrToMemBlock.end())
		{
			RTASSERT(0);
			return false;
		}

		m_AddrToMemBlock.erase(it);

		return true;
	}

	void RtStaticMemProtector::CheckAll()
	{
#if defined( _DEBUG ) && defined ( ENABLE_MEMORY_ALLOCATOR_DEBUG )
		RtResLock::AutoLock autoLock(m_lock);

		std::map<void*, SMemBlock>::iterator it = m_AddrToMemBlock.begin();

		int bResult = 0;
		for (; it != m_AddrToMemBlock.end(); it++)
		{
			bResult = memcmp(m_akBoundCheak, it->second.pkPadding, PADING_SIZE);
			if (bResult != 0)
			{
				RtCoreLog().Error("****THIS MEMORY OUT OF BOUND***\n");
				RtCoreLog().Error("size=%d, File='%s':%d\n", it->second.iSize, it->second.pFile, it->second.iLine);
				//rtDebugBreak( );
			}
		}
#endif
	}

	void RtStaticMemProtector::OnAddBlock(SMemBlock* pBlock, const char* szFileName, int nLine)
	{

		rt2_strncpy(pBlock->strNameBuffer, "RtStaticMemProtector-MemBlock", 32);

		if (szFileName)
		{
			pBlock->pFile = szFileName;
		}
		else
		{
			pBlock->pFile = "";
		}

		pBlock->iLine = nLine;

	}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
