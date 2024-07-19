//
// s_object.cpp: implementation of the CRegionObject class.
//
#include "region.h"

// RT_IMPLEMENT_DYNCREATE(CRegionObject,rtObject,NULL,"")
// RT_IMPLEMENT_DYNCREATE(CRegionFactory,CRegionObject,NULL,"")

TObjectList CRegionObject::m_timerObjList;
ULONG CRegionObject::m_timerCalled = 0;

CRegionObject::CRegionObject()
{
	m_pModule   = NULL;
	m_pDict     = NULL;
	m_pFunc     = NULL;

	m_flag		= NULL;
	m_type		= OBJECT_TYPE_OBJECT;
	m_oId	    = NULL;

	m_nextObj   = NULL;
	m_prevObj   = NULL;
	m_gridX     = -1;
	m_gridY     = -1;

	for(int ii=0; ii<SCRIPT_STATE_VALUE_COUNT; ii++)
		m_lScriptState[ii] = 0;
}

CRegionObject::~CRegionObject()
{
	ClearTimer();
	Py_XDECREF(m_pModule);
}

bool CRegionObject::ReloadScript()
{
	if(m_scriptName.empty())
		return false;

	if(!m_pModule) 
		return LoadScript(m_scriptName.c_str());

	m_pModule = PyImport_ReloadModule(m_pModule);
	if(!m_pModule) 
		return false;

	m_pDict = PyModule_GetDict(m_pModule);
	if(!m_pDict)
		return false;
	
	return true;
}

bool CRegionObject::LoadScript(const char *name)
{
	// free old module
	Py_XDECREF(m_pModule);
	
	PyObject *pName;
	pName = PyString_FromString(name);
	m_pModule = PyImport_ImportModule((char*)name);
	if(!m_pModule) 
	{
		LOG1("CRegionObject: load script [%s] error,no module\n",name);
		PyErr_Print(); 
		return false;
	}

	m_pDict = PyModule_GetDict(m_pModule);
	if(!m_pDict)
	{
		LOG1("CRegionObject: load script [%s] error,no dict\n",name);
		return false;
	}

	// save scripts module name 
	m_scriptName = name;
	Py_XDECREF(pName);

	CallScript("OnLoad", false);

	return true;
}

PyObject* CRegionObject::FindFunc(char const*func, bool optional)
{
	if(!m_pDict) 
		return 0;

	PyObject* theFunction													= PyDict_GetItemString(m_pDict, func);
	if(!theFunction || !PyCallable_Check(theFunction)) 
	{
		if(!optional)
			LOG2("CRegionObject: can't find function [%s] in script [%s]\n",func,m_scriptName.c_str());
		
		return 0;
	}

	return theFunction;
}

long CRegionObject::ExecFunc(char const* func, PyObject* object, PyObject* args)
{
	DWORD begTime															= rtGetMilliseconds();
	PyObject* pRet															= PyObject_CallObject(object, args);
	m_scriptCpuTime															+= (rtGetMilliseconds() - begTime);

	if(!pRet)
	{
		LOG2("CRegionObject: call function [%s] in script [%s] failed",func,m_scriptName.c_str());
		PyErr_Print(); 
		return 0;
	}

	long lRet = PyInt_AsLong(pRet);
	Py_XDECREF(pRet);
	
	return lRet;
}

/*
//
// flag operate set,get,delete
//
void CRegionObject::SetFlag(bool tmp,const char *flag,const char *value)
{
	if(strlen(flag) == 0) return;
	if(!tmp)
		m_flagMap[0].Set(flag,value);
	else
		m_flagMap[1].Set(flag,value);
}

const char *CRegionObject::GetFlag(bool tmp,const char *flag)
{
	if(!tmp)
		return m_flagMap[0].Get(flag);
	else
		return m_flagMap[1].Get(flag);
}

void CRegionObject::DelFlag(bool tmp,const char *flag)
{
	if(!tmp)
		m_flagMap[0].Del(flag);
	else
		m_flagMap[1].Del(flag);
}
*/

//
// timer support 
//
void CRegionObject::OnTimer(ULONG id, DWORD dwParam[])
{
	// call script timer
	if(id > OBJECT_SCRIPT_TIMER_BASE && IsScriptLoaded())
	{
		//3.30，LeiJun修改为OnTimerX调用，X为计时器id
		char sFuncString[40] = {0};
		rt2_snprintf(sFuncString, 40, "OnTimer%d", id - OBJECT_SCRIPT_TIMER_BASE);
		if(dwParam)
			CallScript(sFuncString, false, dwParam[0], dwParam[1], dwParam[2], dwParam[3]);
		else
			CallScript(sFuncString, false, -1, -1, -1, -1);
	}
}

void CRegionObject::SetTimer(ULONG id,ULONG interval, DWORD dwParam[])
{
	if(interval == 0)
	{
		// LOG("CRegionObject: set timer interval = 0\n");
		StopTimer(id);
		return;
	}

	STimer *timer;
	timer = FindTimer(id);
	if(!timer)
	{
		timer = RT_NEW STimer;
		timer->id = id;
		timer->lastcall = rtGetMilliseconds();
		timer->valid = true;
		timer->active = true;
		m_timerList.push_back(timer);
	}
	timer->lastcall = rtGetMilliseconds();
	timer->valid = true;
	timer->interval = interval;

	//更新或设置计时器参数
	for(int ii=0; ii<4; ii++)
		timer->dwParam[ii] = -1;
	if(dwParam)
	{
		for(int ii=0; ii<4; ii++)
		{
			if(dwParam[ii] >= 0)
				timer->dwParam[ii] = dwParam[ii];
			else break;
		}
	}

	if(!(m_flag & OBJECT_FLAG_TIMER))
	{
		AddTimerObject(this);
	}
}

STimer *CRegionObject::FindTimer(ULONG id)
{
	list<STimer*>::iterator it;
	for(it=m_timerList.begin();it!=m_timerList.end();it++)
	{
		if((*it)->id == id) return (*it);
	}
	return NULL;
}

bool CRegionObject::StopTimer(ULONG id)
{
	STimer *timer = FindTimer(id);
	if(!timer) 
		return false;
	timer->valid = false;
	return true;
}

int CRegionObject::UpdateTimer(ULONG now)
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
		if(!timer->active) 
		{
			it++;
			continue;
		}
		if(!timer->valid) 
		{
			it = m_timerList.erase(it);
			DEL_ONE(timer);
			continue;
		}
		valid++;
		if(now - timer->lastcall > timer->interval) 
		{
			m_timerCalled++;
			OnTimer(timer->id, timer->dwParam);
			// some object will destory self in timer,so...
			// fix me 
			// if(m_flag & OBJECT_FLAG_DESTROY) return -1;
			timer->lastcall = now;
		}
		it++;
		continue;
	}
	return valid;
}

bool CRegionObject::ActiveTimer(ULONG id,bool active)
{
	STimer *timer = FindTimer(id);
	if(!timer) return false;
	timer->active = active;

	if(active && !(m_flag&OBJECT_FLAG_TIMER))
	{
		AddTimerObject(this);
	}
	return true;
}

void CRegionObject::ClearTimer()
{
	list<STimer*>::iterator it;
	for(it=m_timerList.begin();it!=m_timerList.end();it++)
	{
		DEL_ONE(*it);
	}
	m_timerList.clear();
}

void CRegionObject::AddTimerObject(CRegionObject *object)
{
	m_timerObjList.push_back(object);
	object->m_flag |= OBJECT_FLAG_TIMER;
}

void CRegionObject::RemoveTimerObject(CRegionObject *object)
{
	// if(object->m_flag & OBJECT_FLAG_TIMER)
	//{
	//	m_timerObjList.remove(object);
	//	object->m_flag &= ~OBJECT_FLAG_TIMER;
	//}
}

void CRegionObject::UpdateTimerObject()
{
	CRegionObject *ob;
    TObjectList::iterator it;
	ULONG now = rtGetMilliseconds();
	
	m_timerCalled = 0;
	for(it = m_timerObjList.begin();it != m_timerObjList.end();)
	{
		ob = *it;

		// if object is destory,remove it
		if(ob->m_flag & OBJECT_FLAG_DESTROY)
		{
			ob->m_flag &= ~OBJECT_FLAG_TIMER;
			it = m_timerObjList.erase(it);
			continue;
		}
		if(ob->UpdateTimer(now) <= 0)
		{
			ob->m_flag &= ~OBJECT_FLAG_TIMER;
			it = m_timerObjList.erase(it);
			continue;
		}
		it++;
		continue;
	}
}

// --------------------------------------------------------------------------
// object factory
// --------------------------------------------------------------------------
CRegionFactory::CRegionFactory()
{
	m_objectId = 1;
}

CRegionFactory::~CRegionFactory()
{
	// Only test here, destory all object Louis.Huang @ Mon 23 Feb 2009
	// TObjectHash::iterator it;
	// for(it = m_hashObject.begin();it != m_hashObject.end();++it)
	// {
		// CRegionObject* pOb = it->second;
		// m_destroyList.push_back(pOb);
	// }

	// m_hashObject.clear();
	// RealDestroyObject(true);
}

CRegionObject *CRegionFactory::CreateObject(unsigned char type,bool cheat)
{
	// use type instead of runtime class to save network bandwith
	if(type <= 0 || type >= OBJECT_TYPE_MAX) return NULL;

	CRegionObject *ob = CreateObjectByType(type);
	if(!ob) return NULL;

	if(!cheat && g_region->IsCheatId(PeekNextObjectId()))
		GetNextObjectId();
	
	ob->m_type  = type;
	ob->m_oId   = GetNextObjectId();
	m_hashObject[ob->m_oId] = ob;
	return ob;
}
//PZH
TObjectHash& CRegionFactory::GetTObjectHash()
{
	return m_hashObject;
}
//
#define CASE_CREATE_OBJECT(TYPE,CLASS) case TYPE:ob = RT_NEW CLASS;return ob;
CRegionObject *CRegionFactory::CreateObjectByType(unsigned char type)
{
	CRegionObject *ob;
	switch(type)
	{
		CASE_CREATE_OBJECT(OBJECT_TYPE_OBJECT,CRegionObject)
		CASE_CREATE_OBJECT(OBJECT_TYPE_CREATURE,CRegionCreature)
		CASE_CREATE_OBJECT(OBJECT_TYPE_ITEM,CRegionItem)
		CASE_CREATE_OBJECT(OBJECT_TYPE_NPC_SERVICE,CRegionNpcTask)
//---------------------------add start by tony 05.05.19----------------------//
		CASE_CREATE_OBJECT(OBJECT_TYPE_NPC_COMBATIVE,CRegionCreature)
//---------------------------add start by tony 05.05.19----------------------//
		CASE_CREATE_OBJECT(OBJECT_TYPE_SCENE,CRegionScene)
		CASE_CREATE_OBJECT(OBJECT_TYPE_SCENE_FB,CRegionSceneFB)
		CASE_CREATE_OBJECT(OBJECT_TYPE_SCENE_WARFIELD,CRsSceneWarfield)
		CASE_CREATE_OBJECT(OBJECT_TYPE_SPIRIT,CRegionCreature)
	}
	return NULL;
}

// destroy a object,remove from object hash table 
void CRegionFactory::DestroyObject(CRegionObject *object)
{
	if(!object)
	{
		LOG("CRegionFactory: destroy null object\n");
		return;
	}

	// check if object already destroyed
	if(object->m_flag & OBJECT_FLAG_DESTROY)
		return;
	
	object->m_flag |= OBJECT_FLAG_DESTROY;
	m_hashObject.erase(object->m_oId);
	m_destroyList.push_back(object);
}

CRegionObject *CRegionFactory::FindObject(ULONG oid)
{
	TObjectHash::iterator it;
	it = m_hashObject.find(oid);
	if (it == m_hashObject.end())
		return NULL;
	else
		return (*it).second;
}

// real delete a object,free memory 
void CRegionFactory::RealDestroyObject(bool bRemoveTimerObject /* = false*/)
{
	CRegionObject *ob;
	TObjectList::iterator it;
	for(it = m_destroyList.begin();it != m_destroyList.end();)
	{
		ob = *it;
		ob->ClearTimer();
		if((ob->m_flag & OBJECT_FLAG_TIMER) && !bRemoveTimerObject)
		{
			it++;
			continue;
		}
		DEL_ONE(*it);
		it = m_destroyList.erase(it);
	}
	// m_destroyList.clear();
}

ULONG CRegionFactory::GetNextObjectId()
{
	return m_objectId++;
}

ULONG CRegionFactory::PeekNextObjectId()
{
	return m_objectId;
}

ULONG CRegionFactory::GetObjectNum()
{
	return (int)m_hashObject.size();
}

void CRegionFactory::Exit()
{
	RealDestroyObject();
	int size = (int)m_hashObject.size();
	if(size > 0)
	{
		LOG1("CRegionFactory: [%d] object leak\n",size);
		TObjectHash::iterator it;
		for(it = m_hashObject.begin();it != m_hashObject.end();it++)
		{
			DEL_ONE((*it).second);
		}
	}
}

void CRegionFactory::ReloadScript()
{
	TObjectHash::iterator it;
	for(it=m_hashObject.begin(); it!=m_hashObject.end(); it++)
	{
		CRegionObject *ob = (*it).second;
		ob->ReloadScript();
	}
}

