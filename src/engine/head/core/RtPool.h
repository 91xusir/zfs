/** �����.
*    - ����һ��ģ�棬�������������û��Լ��Ļ����
*    - iGrowΪ����سɳ��������ǵ��������û�п��õ��ڴ��ʱ��һ�����������ڴ���������ڴ�������
*    - ע�⣺
*        - ���ڴ󲿷ֵĶ����Ǵӻ�����е��������Զ���Ĺ��������ֻ���ڷ����ʱ����ã�Ϊ�˱���ȡ��
*          �����ʱ���ܳ�ʼ��������Ҫ��ʹ�������Ķ���һ��Ҫ��������OnCreate��OnDestroy����������
*          ����������������
*        - ����һ�εĵ���DestroyObj�ᵼ�������������false��Ϊ�˱�֤������ȶ��ԣ����ڵ��õĵط���������
*        - �벻Ҫ�Լ�����delete��ɾ������
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
	/// ����һ������
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

	/// ����һ����������ö����Ѿ������ٹ����ͻ᷵��false�����򷵻�true
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

	/// ��ǰ���ٶ�����ʹ��
	int UsedCount()     { return m_iUsedCount; }
	/// ��ǰ������л��ж��ٶ���
	int UnusedCount()   { return m_iUnsedCount; }
	/// �ܵķ������ĸ���
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