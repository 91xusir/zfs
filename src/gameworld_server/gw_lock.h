#ifndef _INC_GW_LOCK_H_
#define _INC_GW_LOCK_H_

#ifdef _WIN32
#define GWLockHandle CRITICAL_SECTION
#else
#define GWLockHandle pthread_mutex_t
#endif

class  GWLock
{
public:
	/// Lock the thread	
	void                Enter();

	/// Unlock thread thread	
	void                Leave();

	/// Get the current lock handle
	const GWLockHandle&	GetHandle() const;

	GWLockHandle&		GetHandle();

	const GWLockHandle*	operator & () const;

	GWLockHandle*		operator & ();

public:
	/// Constructor, create the lock handle object	
	GWLock();

	/// Destructor, destroy the lock handle object	
	~GWLock();

public:
	/**
	The automatic GWLock/Unlock class. 
	Constructor to lock and destruct to unlock
	*/	
	class AutoLock
	{
	public:
		/// Constructor, entry the mutex		
		AutoLock(GWLock & lock);

		/// Destructor, leave the mutex		
		~AutoLock();

	private:
		///	
		GWLock &        m_lock;
	}; // class GWLock::AutoLock

private:
	///	
	GWLockHandle		m_handle;
}; // class GWLock

inline const GWLockHandle& GWLock::GetHandle() const
{
	return m_handle;
}

inline GWLockHandle& GWLock::GetHandle()
{
	return m_handle;
}

inline const GWLockHandle* GWLock::operator&() const
{
	return &m_handle;
}

inline GWLockHandle* GWLock::operator& ()
{
	return &m_handle;
}

inline GWLock::AutoLock::AutoLock(GWLock& lock)
: m_lock(lock)
{
	lock.Enter();
}

inline GWLock::AutoLock::~AutoLock()
{
	m_lock.Leave();
}


#ifdef _WIN32

inline GWLock::GWLock()
{
	InitializeCriticalSection(&m_handle);
}

inline GWLock::~GWLock()
{
	DeleteCriticalSection(&m_handle);
}

inline void GWLock::Enter()
{
	EnterCriticalSection(&m_handle);
}

inline void GWLock::Leave()
{
	LeaveCriticalSection(&m_handle);
}

#else // _WIN32

inline GWLock::GWLock()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);	
	pthread_mutex_init(&m_handle, &attr);
	pthread_mutexattr_destroy(&attr);
}

inline GWLock::~GWLock()
{
	pthread_mutex_destroy(&m_handle);
}

inline void GWLock::Enter()
{
	pthread_mutex_lock(&m_handle);
}

inline void GWLock::Leave()
{
	pthread_mutex_unlock(&m_handle);
}

#endif // _WIN32


#endif // _INC_GW_LOCK_H_
