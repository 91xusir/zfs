#include "core/rt2_core.h"

namespace rt2_core {

#ifdef _WIN32

struct RtCreateThread
{
	RtThread*       _this;
	RtThreadEvent   _event;		
};


RtThread::~RtThread()
{	
	if (IsRunning())
		Terminate(0);	
}

void RtThread::Terminate(int code)
{
	CHECK(m_handle != 0);
	CHECK(m_state != STOP && !IsThread());	

	LockThread();

	TerminateThread(m_handle, code);

	UnlockThread();

	Join();

	OnExitThread(code);

	m_state = STOP;
	m_handle = NULL;
}

void RtThread::Suspend()
{
	CHECK(m_handle != 0);
	CHECK(m_state = RtThread::RUNNING);	

	m_state = RtThread::SUSPEND;

	SuspendThread(m_handle);
}

void RtThread::Resume()
{
	CHECK(m_handle != 0);
	CHECK(m_state == SUSPEND);	

	m_state = RtThread::RUNNING;

	ResumeThread(m_handle);
}

bool RtThread::IsThread()
{	
	return m_id == GetCurrentThreadId();
}

/*
void RtThread::Exit(int code)
{
	if (IsThread())
	{
		if (m_isAutoDel)
			delete this;

		::ExitThread(code);
	}
}
*/


void RtThread::Start()
{
	CHECK(!IsRunning());

	// RtCreateThread* ct = (RtCreateThread*)RtCoreMem().Alloc(sizeof(RtCreateThread));
	RtCreateThread* ct = RT_NEW RtCreateThread;

	ct->_this = this;

	LockThread();

	HANDLE hThread = CreateThread(NULL, 0, RtThread::ThreadProc, ct, 0, &m_id);
	
	m_handle = hThread;

	UnlockThread();

	ct->_event.Wait();
	delete ct;
    //RtCoreMem().Free(ct);
}


#endif

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
