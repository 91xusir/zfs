/** 缓冲池.
*    - 这是一个模版，可以用来生成用户自己的缓冲池
*    - iGrow为缓冲池成长量，就是当缓冲池中没有可用的内存的时候一次性申请多个内存块来减少内存分配次数
*    - 注意：
*        - 由于大部分的对象都是从缓冲池中调出，所以对象的构造和析构只有在分配的时候调用，为了避免取得
*          对象的时候不能初始化，这里要求使用这个类的对象一定要包含函数OnCreate和OnDestroy，用他们来
*          替代构造和析构函数
*        - 超过一次的调用DestroyObj会导致这个函数返回false，为了保证程序的稳定性，请在调用的地方做这个检查
*        - 请不要自己调用delete来删除对象
*/ 


namespace rt2_core {


template<class T, int iGrow=10>
class RtPool
{
private:
	struct SItem
	{
		SItem*  pNext;
		int     bUsed;
		T       item;
	};
	struct SItemArray
	{
		SItem       items[iGrow];
		SItemArray* pNextArray;
	};

	SItemArray* m_pArrays;
	SItem*      m_pUnused;
	int         m_iUsedCount;
	int         m_iUnsedCount;

public:
	/// 创建一个对象
	T* CreateObj()
	{
		if (m_pUnused==0)
		{
			SItemArray* pNewArray = RT_NEW SItemArray;
			pNewArray->pNextArray = m_pArrays;
			m_pArrays = pNewArray;
			for (int i=1; i<iGrow; i++)
			{
				pNewArray->items[i-1].pNext = pNewArray->items+i;
				pNewArray->items[i-1].bUsed = FALSE;
			}
			pNewArray->items[iGrow-1].bUsed = FALSE;
			pNewArray->items[iGrow-1].pNext = 0;
			m_pUnused = pNewArray->items;
			m_iUnsedCount += iGrow;
		}
		SItem* pItem;
		pItem = m_pUnused;
		CHECK(pItem->bUsed==0);
		m_pUnused = m_pUnused->pNext;
		pItem->bUsed = TRUE;
		m_iUsedCount ++;
		m_iUnsedCount --;
		pItem->item.OnCreate();
		return &(pItem->item);
	}

	/// 销毁一个对象，如果该对象已经被销毁过，就会返回false，否则返回true
	bool DestroyObj(T* p)
	{
		SItem* pItem = (SItem*)(((int*)p)-2);
		if (pItem->bUsed==FALSE)
			return false;
		m_iUsedCount --;
		m_iUnsedCount ++;
		p->OnDestroy();
		pItem->pNext = m_pUnused;
		m_pUnused = pItem;
		pItem->bUsed = FALSE;
		return true;
	}

	/// 当前多少对象在使用
	int UsedCount()     { return m_iUsedCount; }
	/// 当前缓冲池中还有多少对象
	int UnusedCount()   { return m_iUnsedCount; }
	/// 总的分配对象的个数
	int NodeCount()     { return m_iUnsedCount+m_iUsedCount; }

	RtPool()
	{
		m_pArrays = 0;
		m_pUnused = 0;
		m_iUsedCount = 0;
		m_iUnsedCount = 0;
	}

	~RtPool()
	{
		RTASSERT(m_iUsedCount==0);

		// Can't log here, maybe rtcore is destory now. Louis @ Mon 31 Aug 2009
		// RtCoreLog().Error("RtPool:~RtPool leak pool object[%d]\n", m_iUsedCount);

		SItemArray* pDel;
		while (m_pArrays)
		{
			pDel = m_pArrays;
			m_pArrays = m_pArrays->pNextArray;
			delete pDel;
		}
		m_pArrays = NULL;
		m_pUnused = NULL;
		m_iUsedCount = 0;
		m_iUnsedCount = 0;
	}
};
} // namespace rt2_core