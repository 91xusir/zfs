#include "core/rt2_core.h"

namespace rt2_core {

RtThread::RtThread()
: m_handle(0)
, m_state(RtThread::STOP)
, m_isAutoDel(false)
{
}
struct RtCreateThread
{
	RtThread*       _this;
	RtThreadEvent   _event;
};

#ifdef _WIN32
DWORD WINAPI RtThread::ThreadProc(void * args)
#else
void* RtThread::ThreadProc(void * args)
#endif
{
	CHECK(args != NULL);

	RtCreateThread* cs = (RtCreateThread*)args;

	RtThread * _this = cs->_this;
	// RtCoreMem().Free(cs);	
	// delete cs;
	/// Set the thread state
    _this->LockThread();
	_this->m_state = RtThread::RUNNING;
    _this->UnlockThread();

	cs->_event.Set();

	/// Initalize the thread
	int code = _this->OnInitThread();
	
	if (code != 0)
#ifdef _WIN32
		return (DWORD)code;
#else
		return (void*)code;
#endif

	/// Main loop
	code = _this->OnRun();

	/// RtThread exit, unitalize the thread
	code = _this->OnExitThread(code);

	/// Set the thread state
    _this->LockThread();

#ifdef _WIN32 // 这句可能有错误 error
	CloseHandle(_this->m_handle);
#endif

	_this->m_handle = 0;
	_this->m_state = RtThread::STOP;    
	
	if (_this->m_isAutoDel)
	{
		_this->UnlockThread();

		delete _this;
	}
	else
	{
		_this->UnlockThread();	
	}
		
#ifdef _WIN32
	return (DWORD)0;
#else
	return (void*)code;
#endif
}

int RtThread::OnInitThread()
{
	return 0;
}

int RtThread::OnExitThread(int code)
{
	return code;
}

int RtThread::OnRun()
{
	return 0;
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
