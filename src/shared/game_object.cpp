#include "gc_include.h"

DWORD CGameObject::m_timerCalled = 0;

CGameObject::CGameObject():m_oId(0),m_flag(0)
{
//	sprintf(szStructHead,"COB(%d)",GetObjectId());
}

CGameObject::~CGameObject()
{
//	sprintf(szStructHead,"DOB(%d)",GetObjectId());
	ClearTimer();
}

void CGameObject::OnTimer(DWORD id)
{

}

void CGameObject::SetTimer(DWORD id,DWORD interval)
{
	if(interval == 0)
	{
		StopTimer(id);
		return;
	}

	STimer *timer= FindTimer(id);
	if(!timer)
	{
		timer = RT_NEW STimer;
		timer->id = id;
		timer->active = true;
		m_timerList.push_back(timer);
	}	
	timer->valid = true;
	timer->delay = interval;
	timer->interval = interval;
// 	if(!CheckBit(m_flag,OBJECT_FLAG_TIMER))
// 	{
		AddTimerObject(this);
//	}
}

void CGameObject::RemoveTimer(DWORD id)
{
	STimer * pTimer=NULL;
	list<STimer*>::iterator it;
	for(it=m_timerList.begin();it!=m_timerList.end();)
	{
		pTimer=(*it);
		if(pTimer && pTimer->id == id)
		{
			DEL_ONE(*it)
				it=m_timerList.erase(it);
		}
		else
		{
			it++;
		}
	}

	if(!m_timerList.size()/* && CheckBit(m_flag,OBJECT_FLAG_TIMER)*/)
	{
		RemoveTimerObject(this);
	}

	return;
}

STimer *CGameObject::FindTimer(DWORD id)
{
	list<STimer*>::iterator it;
	for(it=m_timerList.begin();it!=m_timerList.end();it++)
	{
		if((*it)->id == id) return (*it);
	}
	return NULL;
}

bool CGameObject::StopTimer(DWORD id)
{
	STimer *timer = FindTimer(id);
	if(!timer) 
		return false;
	timer->valid = false;
	return true;
}

int CGameObject::UpdateTimer(DWORD delta)
{
	list<STimer*>::iterator it;
	STimer *timer;
	int valid = 0;

	if(m_timerList.empty()) 
	{
		return 0;
	}
	for(it=m_timerList.begin();it!=m_timerList.end();)
	{
		timer = *it; 
		if(timer && timer->valid)
		{
			if(!timer->active) 
			{
				it++;
				continue;
			}

			valid++;

			timer->delay -= delta;
			if(timer->delay <= 0)
			{
				timer->delay += timer->interval;
				m_timerCalled++;

				OnTimer(timer->id);

				// some object will destory self in timer,so...
				// fix me 
				// if(CheckBit(m_flag,OBJECT_FLAG_DESTROY)) return -1;
			}

			it++;
			continue;
		}
		else
		{
			it = m_timerList.erase(it);
			DEL_ONE(timer)
				continue;
		}

	}
	return valid;
}

bool CGameObject::ActiveTimer(DWORD id,bool active)
{
	STimer *timer = FindTimer(id);
	if(!timer) return false;
	timer->active = active;

	if(active/* && !CheckBit(m_flag,OBJECT_FLAG_TIMER)*/)
	{
		AddTimerObject(this);
	}
	return true;
}

void CGameObject::ClearTimer()
{
	list<STimer*>::iterator it;
	for(it=m_timerList.begin();it!=m_timerList.end();it++)
	{
		DEL_ONE(*it)
	}
	m_timerList.clear();

	RemoveTimerObject(this);
}

void CGameObject::AddTimerObject(CGameObject *object)
{
/*
	m_timerObjList.push_back(object);
	AddBit(m_flag,OBJECT_FLAG_TIMER);
*/
}

void CGameObject::RemoveTimerObject(CGameObject *object)
{
/*
	if(CheckBit(object->m_flag,OBJECT_FLAG_TIMER))
	{
		CGameObject * pObj=NULL;
		list<CGameObject*>::iterator it;
		for(it=m_timerObjList.begin(); it!=m_timerObjList.end();++it)
		{
			pObj=*it;
			if(pObj==object)
			{
				RemoveBit(object->m_flag,OBJECT_FLAG_TIMER);				
				m_timerObjList.erase(it);
				return;
			}
		}
	}
*/
}

void CGameObject::UpdateTimerObject(DWORD delta)
{
/*
	CGameObject *ob;
	list<CGameObject*>::iterator it;

	m_timerCalled = 0;
	for(it=m_timerObjList.begin(); it!=m_timerObjList.end();)
	{
		ob = *it;

		// if object is destory,remove it
		if(CheckBit(ob->m_flag,OBJECT_FLAG_DESTROY))
		{
			//RemoveBit(ob->m_flag,OBJECT_FLAG_TIMER);
			//it = m_timerObjList.erase(it);
			it++;
			continue;
		}
		if(ob->UpdateTimer(delta) <= 0)
		{
			RemoveBit(ob->m_flag,OBJECT_FLAG_TIMER);
			it = m_timerObjList.erase(it);
			continue;
		}
		it++;
		continue;
	}
*/
}
