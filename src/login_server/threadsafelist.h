#ifndef __THREAD_SAFE_LIST_H__
#define __THREAD_SAFE_LIST_H__

#include "core/rt2_core.h"
#include <list>
using namespace std;

template<class T> 
class CThreadSafeList  
{
public:
	CThreadSafeList();
	virtual ~CThreadSafeList();

	int  GetCmdNum();
	bool AddCmd(T *cmd);
	bool GetCmd(T *cmd);
	bool SetMaxCmdNum(int num);

private:
	RT_TYPE_CS		m_cs;
	list<T>		m_cmdList;
	int			m_maxCmd;
	int			m_cmdNum;
};

template<class T>
CThreadSafeList<T>::CThreadSafeList()
{
	m_cmdList.clear();
	INIT_CS(&m_cs);
	m_maxCmd = 1000;
	m_cmdNum = 0;
}

template<class T>
CThreadSafeList<T>::~CThreadSafeList()
{
	DELETE_CS(&m_cs);
	m_cmdList.clear();
}

template<class T>
bool CThreadSafeList<T>::SetMaxCmdNum(int num)
{
	m_maxCmd = num;
	return true;
}

template<class T>
bool CThreadSafeList<T>::GetCmd(T *cmd)
{
	LOCK_CS(&m_cs);
	if(m_cmdNum<=0)
	{
		UNLOCK_CS(&m_cs);
		return false;
	}
	*cmd = m_cmdList.front();
	m_cmdList.pop_front();
	m_cmdNum--;
	UNLOCK_CS(&m_cs);
	return true;
}

template<class T>
bool CThreadSafeList<T>::AddCmd(T *cmd)
{
	LOCK_CS(&m_cs);
	if(m_cmdNum >= m_maxCmd)
	{
		UNLOCK_CS(&m_cs);
		return false;
	}
	m_cmdList.push_back(*cmd);
	m_cmdNum++;
	UNLOCK_CS(&m_cs);
	return true;
}

template<class T>
int CThreadSafeList<T>::GetCmdNum()
{
	return m_cmdNum;
}

#endif 
