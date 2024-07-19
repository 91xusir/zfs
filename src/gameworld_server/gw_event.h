#ifndef _INC_GW_EVENT_H_
#define _INC_GW_EVENT_H_

#ifndef TIMEOUT
#	ifdef _WIN32
#		define INFTIMEOUT INFINITE
#	else
#		define INFTIMEOUT (-1)
#	endif
#endif

#ifdef _WIN32
#	define GWEventHandle HANDLE
#else
#	define GWEventHandle pthread_cond_t
#endif

/**
*/
class  GWEvent
{	
public:
	bool Wait(int timeout);

	void Wait();

	void Set();

	void Reset();

public:
	const GWEventHandle&	GetHandle();

public:
	GWEvent();

	~GWEvent();

private:
	GWEventHandle	m_handle;

	GWLock			m_lock;

	bool			m_signaled;
};


#ifdef _WIN32

inline GWEvent::GWEvent()
{
	m_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
}

inline GWEvent::~GWEvent()
{
	CloseHandle(m_handle);
}

inline bool GWEvent::Wait(int timeout)
{
	return WaitForSingleObject(m_handle, timeout) != WAIT_TIMEOUT;
}

inline void GWEvent::Wait()
{
	WaitForSingleObject(m_handle, INFINITE);
}

inline void GWEvent::Set()
{
	SetEvent(m_handle);
}

inline void GWEvent::Reset()
{
	ResetEvent(m_handle);
}

#else // _WIN32

inline GWEvent::GWEvent()
{
	m_signaled = false;
	pthread_cond_init(&m_handle, NULL);
}

inline GWEvent::~GWEvent()
{
	pthread_cond_destroy(&m_handle);
}

inline void GWEvent::Set()
{
	m_lock.Enter();
	m_signaled = true;
	pthread_cond_broadcast(&m_handle);
	m_lock.Leave();
}

inline void GWEvent::Reset()
{
	m_lock.Enter();
	m_signaled = false;
	m_lock.Leave();
}

inline void GWEvent::Wait()
{
	Wait(INFTIMEOUT);
}

inline bool GWEvent::Wait(int timeout)
{
	bool r = true;
	m_lock.Enter();
	if(!m_signaled)
	{
		if (timeout != INFTIMEOUT)
		{
			timeval now;
			gettimeofday(&now, NULL);

			timespec tspec;
			tspec.tv_sec = now.tv_sec + timeout / 1000;
			tspec.tv_nsec = (now.tv_usec + (timeout % 1000) * 1000) * 1000;

			//r = pthread_cond_timedwait(&m_handle, &m_lock, &tspec) != ETIMEDOUT;
            r = pthread_cond_timedwait(&m_handle, &m_lock, &tspec) == 0;
		}
		else
			pthread_cond_wait(&m_handle, &m_lock) == 0;
	}
	m_lock.Leave();	 
	return r;
}
#endif // _WIN32

#endif // _INC_GW_EVENT_H_
