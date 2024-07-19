inline RtThread::State RtThread::GetState()
{
	RtThreadLock::AutoLock lock(m_lock);

	return m_state;
}

inline bool RtThread::IsRunning()
{
    RtThreadLock::AutoLock lock(m_lock);

	return m_state == RUNNING;
}

#ifdef _WIN32
inline void RtThread::Join()
{
    if (m_handle)
    {
	    ::WaitForSingleObject(m_handle, INFINITE);
	    ::CloseHandle(m_handle);
    }
}

inline void RtThread::Sleep(int sec)
{
	::Sleep(sec * 1000);
}
#else
inline void RtThread::Join()
{
    if (m_handle)
    {
	    pthread_join(m_handle, NULL);
    }
}

inline void RtThread::Sleep(int sec)
{
	::sleep(sec);
}
#endif
