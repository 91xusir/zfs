#ifndef __CUNREDOINTERFACE__
#define __CUNREDOINTERFACE__

#include <deque>

/*
����ӿڣ�����ʵ�ֳ������ָ�
IsCanDo����ʵ��
���ܴ��ڻ�����(��Ϊ�п����Ǽ��صĶ�������),
����������֤һ�º����Ƿ����������.
*/
class CCommand
{
public:
	virtual ~CCommand(){};
	virtual bool operator()(void) = 0;
	virtual bool IsCanDo() const = 0; 
};
class CURDoInterface
{
public:
	CURDoInterface (int nUndoLevel = 200) : m_nUndoLevel(nUndoLevel){}

	//���ó�������
	void SetUndoLevel(int nUndoLevel = 200)
	{
		m_nUndoLevel = nUndoLevel;
	}

	//����б�
	virtual ~CURDoInterface()
	{
		ClearRedoList();
		ClearUndoList();
	}

	//�������
	bool AddCommand (CCommand* pCmd)
	{
		if ((m_nUndoLevel <= 0) || !pCmd)
		{
			return false;
		}

		ClearRedoList();

		if ((int)m_UndoList.size() >= m_nUndoLevel) // never '>'
		{
			CCommand* p = m_UndoList.front() ;
			m_UndoList.pop_front() ;
			delete p ;
		}
		m_UndoList.push_back (pCmd) ;
		return true ;
	}

	//����
	bool Undo()
	{
		return _undo();
	}

	//����
	bool Redo()
	{
		return _redo();
	}
protected:
	//����ʵ��
	virtual bool _undo()
	{
		bool bResult = false;
		if (!m_UndoList.empty())
		{
			CCommand* p = m_UndoList.back() ; // get the latest undo-command
			(*p)();
			m_UndoList.pop_back() ;
			m_RedoList.push_front(p) ;
			bResult = true;
		}
		return bResult;
	}

	//����ʵ��
	virtual bool _redo()
	{
		bool bResult = false;
		if (!m_RedoList.empty())
		{
			CCommand* p = m_RedoList.front() ; // get the first redo-command
			(*p)();
			m_RedoList.pop_front() ;
			m_UndoList.push_back(p) ;
			bResult = true;
		}
		return bResult;
	}

	void ClearRedoList()
	{
		for (sdci=m_RedoList.begin(); sdci!=m_RedoList.end(); ++sdci)
		{
			delete (*sdci);
		}
		m_RedoList.clear();
	}

	void ClearUndoList()
	{
		for (sdci=m_UndoList.begin(); sdci!=m_UndoList.end(); ++sdci)
		{
			delete (*sdci);
		}
		m_UndoList.clear();
	}
private:
	std::deque<CCommand*>	m_UndoList;		//��������
	std::deque<CCommand*>	m_RedoList;		//�����б�
	std::deque<CCommand*>::iterator sdci;
	int						m_nUndoLevel ;	//���ɳ����Ĵ���
};
#endif
