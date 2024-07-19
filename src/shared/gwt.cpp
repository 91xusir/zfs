#include <time.h>
#include "gwt.h"
#include "core/rt2_core.h"

GWT::GWT()
{
	Initialize(0, 1);
}

unsigned GWT::GetTrueTime()
{
	time_t curTime  = (unsigned)time(NULL);
	RtLocateTime curTrueTime(curTime);
	return curTrueTime.iHour;
}

void GWT::Update()
{
	m_ticks = (unsigned)(time(NULL) - m_originSysTicks);
	m_ticks = (unsigned)(m_ticks * m_factor) + m_originTicks;
}

void GWT::Initialize(unsigned originTicks, float scaling)
{	
	m_factor = scaling / 60.0f; // 除以60，用于将秒转换成分

	Synchronize(originTicks);
}

void GWT::Synchronize(unsigned originTicks)
{
	m_originTicks = originTicks;
	m_originSysTicks = (unsigned)time(NULL);
	m_ticks = m_originTicks;
}
