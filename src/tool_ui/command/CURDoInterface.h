#ifndef __CUNREDOINTERFACE__
#define __CUNREDOINTERFACE__

#include <deque>

/*
命令接口，用于实现撤销，恢复
IsCanDo函数实现
可能存在坏数据(因为有可能是加载的多余数据),
本函数则验证一下函数是否有逆操作等.
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

	//设置撤销步数
	void SetUndoLevel(int nUndoLevel = 200)
	{
		m_nUndoLevel = nUndoLevel;
	}

	//清空列表
	virtual ~CURDoInterface()
	{
		ClearRedoList();
		ClearUndoList();
	}

	//命令添加
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

	//撤销
	bool Undo()
	{
		return _undo();
	}

	//重做
	bool Redo()
	{
		return _redo();
	}
protected:
	//撤销实做
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

	//重做实做
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
	std::deque<CCommand*>	m_UndoList;		//撤销队列
	std::deque<CCommand*>	m_RedoList;		//重做列表
	std::deque<CCommand*>::iterator sdci;
	int						m_nUndoLevel ;	//最大可撤销的次数
};
#endif
