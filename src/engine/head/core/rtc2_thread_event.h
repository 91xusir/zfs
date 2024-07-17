/**
* @file rtc2_thread_event.h 线程事件
* @author janhail
*/

namespace rt2_core {

#ifndef TIMEOUT
#	ifdef _WIN32
#		define INFTIMEOUT INFINITE
#	else
#		define INFTIMEOUT (-1)
#	endif
#endif

#ifdef _WIN32
#	define RtThreadEventHandle HANDLE
#else
#	define RtThreadEventHandle pthread_cond_t
#endif

/**
*/
class  RtThreadEvent
{	
public:
	bool Wait(int timeout);

	void Wait();

	void Set();

	void Reset();

public:
	const RtThreadEventHandle&	GetHandle();

public:
	RtThreadEvent();

	~RtThreadEvent();

private:
	RtThreadEventHandle	m_handle;

	RtThreadLock		m_lock;

	bool			    m_signaled;
};


#ifdef _WIN32

inline RtThreadEvent::RtThreadEvent()
{
	m_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
}

inline RtThreadEvent::~RtThreadEvent()
{
	CloseHandle(m_handle);
}

inline bool RtThreadEvent::Wait(int timeout)
{
	return WaitForSingleObject(m_handle, timeout) != WAIT_TIMEOUT;
}

inline void RtThreadEvent::Wait()
{
	WaitForSingleObject(m_handle, INFINITE);
}

inline void RtThreadEvent::Set()
{
	SetEvent(m_handle);
}

inline void RtThreadEvent::Reset()
{
	ResetEvent(m_handle);
}

#else // _WIN32

inline RtThreadEvent::RtThreadEvent()
{
	m_signaled = false;
	pthread_cond_init(&m_handle, NULL);
}

inline RtThreadEvent::~RtThreadEvent()
{
	pthread_cond_destroy(&m_handle);
}

inline void RtThreadEvent::Set()
{
	m_lock.Enter();
	m_signaled = true;
	m_lock.Leave();
	// pthread_cond_broadcast(&m_handle);
}

inline void RtThreadEvent::Reset()
{
	m_lock.Enter();
	m_signaled = false;
	m_lock.Leave();
}

inline void RtThreadEvent::Wait()
{
	Wait(INFTIMEOUT);
}

inline bool RtThreadEvent::Wait(int timeout)
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
} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
