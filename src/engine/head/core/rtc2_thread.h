/**
* @file rtc2_thread.h 线程对象
* @author janhail
*/

namespace rt2_core {

class RtThread
{
public:	
	/// RtThread state flags	
	enum State
	{		
		RUNNING,		
		STOP,		
		SUSPEND
	};

private:
#ifdef _WIN32	
	static DWORD WINAPI	ThreadProc(void* args);
#else
	static void*		ThreadProc(void* args);
#endif

public:
	/// Working in current thread	
	bool                IsThread();

	/// Current thread is running	
	bool                IsRunning();

	/// Get the m_isAutoDel field	
	bool                IsAutoDelete();

	/// Set the m_isAutoDel field	
	void                SetAutoDelete(bool autoDel);

	/// Exit the thread, this method is be called in the thread	
	//void                Exit(int code);

	///
	void				Join();

	/// Get current thread state	
	State 				GetState();

	/// Get current thread handle
	const RtThreadHandle&	GetHandle() { return m_handle; }

	static void			Sleep(int sec);	

public:
	/// Start a thread	
	virtual void 		Start();

	/// Stop a thread
	virtual void 		Terminate(int code);

	/// Suspend a thread
	virtual void 		Suspend();

	/// Resume a thread	
	virtual void 		Resume();

public:
    void				LockThread() { m_lock.Lock(); }

    void				UnlockThread() { m_lock.Unlock(); }

protected:
	/// Initalize the thread when a thread is first created	
	virtual int			OnInitThread();

	/// Uninitalize the thread when a thread will exit	
	virtual int			OnExitThread(int code);

	/// This method must be overridden to privode
	/// the thread main loop	
	virtual int 		OnRun();	
	

public:
	/// Consructor, initalize fields
	RtThread();

	/// Destructor, close thread if the thread is running
	virtual ~RtThread();

private:
#ifdef _WIN32
	/// Win32 thread id
	DWORD				m_id;
#endif

	/// RtThread handle
	RtThreadHandle		m_handle;	

	/// Current thread state
	State				m_state;

	/// Delete the instance automatic after the thread exit
	bool                m_isAutoDel;

	/// RtThread state lock
	RtThreadLock        m_lock;

#ifndef _WIN32
	///	
	RtThreadEvent		m_suspend;
#endif // _WIN32	
};

#include "rtc2_thread.inl"

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
