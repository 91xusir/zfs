#include "core/rt2_core.h"

namespace rt2_core {

#ifdef LINUX

struct RtCreateThread
{
  RtThread*     _this;
  RtThreadEvent _event;
};


RtThread::~RtThread()
{
}

void RtThread::Terminate(int code)
{
	if (m_state == RtThread::STOP || IsThread())
		return;

	CHECK(m_handle != 0);

	if (m_state == RtThread::SUSPEND)	
		Resume();
		
	pthread_cancel(m_handle);	
	pthread_join(m_handle, NULL);	
}

void RtThread::Suspend()
{
	if (m_state != RtThread::RUNNING)
		return;
	
	CHECK(m_handle != 0);
		
	m_state = RtThread::SUSPEND;		
	
	m_suspend.Wait();	
}
	
void RtThread::Resume()
{
	if (m_state != RtThread::SUSPEND)
		return;
		
	CHECK(m_handle != 0);
	
	m_suspend.Set();
		
	m_state = RtThread::RUNNING;
}

bool RtThread::IsThread()
{	
	return m_handle == pthread_self();
}

void RtThread::Start()
{
  CHECK(!IsRunning());

  RtCreateThread* cs = RT_NEW RtCreateThread;

  cs->_this = this;

  if (pthread_create(&m_handle, NULL, RtThread::ThreadProc,cs) != 0)
    return;

  // C Louis.Huang @ Tue 16 Dec 2008
  // This cs will be free mem when ThreadProc start up so comment below.
  //  delete cs;
}

#endif

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
