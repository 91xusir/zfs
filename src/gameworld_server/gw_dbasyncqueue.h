#ifndef _INC_GW_DB_ASYNC_QUEUE_H_
#define _INC_GW_DB_ASYNC_QUEUE_H_

template<class T>
class GWDBAsyncQueue
{
private:
	vector<T>				m_queue;
	int						m_begin;
	int						m_end;
	int						m_size;	
	RtThreadLock			m_lock;
	string					m_name;
	bool					m_shutdown;

public:
	GWDBAsyncQueue()
		: m_shutdown(false)
	{
	}

	~GWDBAsyncQueue()
	{
	}

	void Create(int capicity, const string& name)
	{
		m_name = name;
		m_queue.resize(capicity);
		
		m_size  = 0;
		m_begin = 0;
		m_end   = 0;
	}

	bool Push(const T& queuer)
	{
		m_lock.Enter();
		if(m_size == (int)m_queue.size())
		{
			m_lock.Leave();
			return false;
		}

		m_queue[m_end] = queuer;
		m_end = (m_end + 1) % (int)m_queue.size();
		m_size++;

		m_lock.Leave();
		return true;
	}


	bool Pop(T& queuer)
	{
		m_lock.Enter();
		if(m_size == 0)
		{
			m_lock.Leave();
			return false;
		}

		queuer = m_queue[m_begin];
		m_begin = (m_begin + 1) % (int)m_queue.size();
		m_size--;

		m_lock.Leave();	
		return true;
	}

	int GetSize()
	{
		RtThreadLock::AutoLock lock(m_lock);
		return m_size;
	}

	void Stop()
	{
		RtThreadLock::AutoLock lock(m_lock);
		m_shutdown = true;
	}

	bool IsFull(int reservation)
	{
		RtThreadLock::AutoLock lock(m_lock);
		return reservation >= (int)m_queue.size() - m_size;
	}
	
	int GetFreeSize()
	{
		RtThreadLock::AutoLock lock(m_lock);
		return (int)m_queue.size() - m_size;
	}
};

#endif // _INC_GW_DB_ASYNC_QUEUE_H_
