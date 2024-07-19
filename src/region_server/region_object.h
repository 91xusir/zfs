#ifndef __REGION_OBJECT_H__
#define __REGION_OBJECT_H__

#include "script_engine_py.h"

#define	SCRIPT_STATE_VALUE_COUNT	10

//
// Object flag 
//
const long OBJECT_FLAG_TIMER			= 1;
const long OBJECT_FLAG_DESTROY			= 2;

// Timer id define
const long OBJECT_C_TIMER_BASE			= 0;	// c timer
const long OBJECT_SCRIPT_TIMER_BASE		= 1000;	// script timer,only set in script

struct STimer
{
	bool   valid;
	bool   active;
	ULONG  id;
	ULONG  interval;
	ULONG  lastcall;
	DWORD  dwParam[4];

	STimer(void) : valid(false), active(false), id(0), interval(0), lastcall(0)
	{
		for(int ii=0; ii<4; ii++)
			dwParam[ii] = -1;
	}
	bool HaveParam(void)
	{
		for(int ii=0; ii<4; ii++)
		{
			if(dwParam[ii] >= 0)
				return true;
		}
		return false;
	}
};

// -----------------------------------------------------------------------
// Region base object
// -----------------------------------------------------------------------
class CRegionObject //: public RtObject  
{
public:
	// RT_DECLARE_DYNCREATE(CRegionObject,RtObject,NULL,"")
	
	CRegionObject();
	virtual ~CRegionObject();
	
	// set/get unique id
	ULONG GetObjectId() { return m_oId; } 
	void SetObjectId(ULONG id) { m_oId = id; }
	
	// multi timer support
	virtual void OnTimer(ULONG id, DWORD dwParam[]);
	//SetTimer可以设置计时器附带参数数组，值为正整数，最多4个值
	void SetTimer(ULONG id,ULONG interval, DWORD dwParam[] = 0);
	bool ActiveTimer(ULONG id,bool active);
	bool StopTimer(ULONG id);
	void ClearTimer();
	int  UpdateTimer(ULONG now);
	STimer *FindTimer(ULONG id);
	list<STimer*> m_timerList;

	// script support
	bool ReloadScript();
	bool LoadScript(const char *name);
	bool IsScriptLoaded() { return (m_pModule!=NULL); }
	const char *GetScriptFile() { return m_scriptName.c_str(); }
	PyObject* FindFunc(char const*func, bool optional);
	long ExecFunc(char const* func, PyObject* object, PyObject* args);
	

	// call python
	long CallScript(char const* functionName, bool optional)
	{
		//
		// get function
		//
		PyObject* object													= FindFunc(functionName, optional);
		if(!object)
			return 0;

		CScriptEngine* theEngine											= CScriptEngine::GetSingleton();
		PyObject* args														= theEngine->BuildCallArgs(1);
		theEngine->BuildArg(args, 0, this);
		long lRet = ExecFunc(functionName, object, args);
		Py_XDECREF(args);
		return lRet;
	}

	// call python
	template<typename T>
	long CallScript(char const* functionName, bool optional, T arg0)
	{
		//
		// get function
		//
		PyObject* object													= FindFunc(functionName, optional);
		if(!object)
			return 0;

		CScriptEngine* theEngine											= CScriptEngine::GetSingleton();
		PyObject* args														= theEngine->BuildCallArgs(2);
		theEngine->BuildArg(args, 0, this);
		theEngine->BuildArg(args, 1, arg0);
		long lRet = ExecFunc(functionName, object, args);
		Py_XDECREF(args);
		return lRet;
	}

	// call python
	template<typename T0, typename T1>
	long CallScript(char const* functionName, bool optional, T0 arg0, T1 arg1)
	{
		//
		// get function
		//
		PyObject* object													= FindFunc(functionName, optional);
		if(!object)
			return 0;

		CScriptEngine* theEngine											= CScriptEngine::GetSingleton();
		PyObject* args														= theEngine->BuildCallArgs(3);
		theEngine->BuildArg(args, 0, this);
		theEngine->BuildArg(args, 1, arg0);
		theEngine->BuildArg(args, 2, arg1);
		long lRet = ExecFunc(functionName, object, args);
		Py_XDECREF(args);
		return lRet;
	}

	// call python
	template<typename T0, typename T1, typename T2>
	long CallScript(char const* functionName, bool optional, T0 arg0, T1 arg1, T2 arg2)
	{
		//
		// get function
		//
		PyObject* object													= FindFunc(functionName, optional);
		if(!object)
			return 0;

		CScriptEngine* theEngine											= CScriptEngine::GetSingleton();
		PyObject* args														= theEngine->BuildCallArgs(4);
		theEngine->BuildArg(args, 0, this);
		theEngine->BuildArg(args, 1, arg0);
		theEngine->BuildArg(args, 2, arg1);
		theEngine->BuildArg(args, 3, arg2);
		long lRet = ExecFunc(functionName, object, args);
		Py_XDECREF(args);
		return lRet;
	}

	// call python
	template<typename T0, typename T1, typename T2, typename T3>
	long CallScript(char const* functionName, bool optional, T0 arg0, T1 arg1, T2 arg2, T3 arg3)
	{
		//
		// get function
		//
		PyObject* object													= FindFunc(functionName, optional);
		if(!object)
			return 0;

		CScriptEngine* theEngine											= CScriptEngine::GetSingleton();
		PyObject* args														= theEngine->BuildCallArgs(5);
		theEngine->BuildArg(args, 0, this);
		theEngine->BuildArg(args, 1, arg0);
		theEngine->BuildArg(args, 2, arg1);
		theEngine->BuildArg(args, 3, arg2);
		theEngine->BuildArg(args, 4, arg3);
		long lRet = ExecFunc(functionName, object, args);
		Py_XDECREF(args);
		return lRet;
	}

	// call python
	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	long CallScript(char const* functionName, bool optional, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		//
		// get function
		//
		PyObject* object													= FindFunc(functionName, optional);
		if(!object)
			return 0;

		CScriptEngine* theEngine											= CScriptEngine::GetSingleton();
		PyObject* args														= theEngine->BuildCallArgs(6);
		theEngine->BuildArg(args, 0, this);
		theEngine->BuildArg(args, 1, arg0);
		theEngine->BuildArg(args, 2, arg1);
		theEngine->BuildArg(args, 3, arg2);
		theEngine->BuildArg(args, 4, arg3);
		theEngine->BuildArg(args, 5, arg4);
		long lRet = ExecFunc(functionName, object, args);
		Py_XDECREF(args);
		return lRet;
	}

	// object info
	ULONG m_oId;
	char  m_type;
	long  m_flag;
	
	// flag support
	/*
	void  SetFlag(const char *key,const char *value);
	void  DelFlag(const char *key);
	const char *GetFlag(const char *key);
	*/

	// replication support
	virtual bool RepUpdate(char type,CG_CmdPacket *cmd) { return false; }
    
	// do timer call
	static void UpdateTimerObject();
	static ULONG m_timerCalled;

	CRegionObject *m_prevObj;
	CRegionObject *m_nextObj;
	long           m_gridX;
	long           m_gridY;

	long	GetScriptStateValue(int iIndex)
	{
		if(iIndex>0 && iIndex<SCRIPT_STATE_VALUE_COUNT)
			return m_lScriptState[iIndex-1];
		return 0;
	}
	void	SetScriptStateValue(int iIndex, long lValue)
	{
		if(iIndex>0 && iIndex<SCRIPT_STATE_VALUE_COUNT)
			m_lScriptState[iIndex-1] = lValue;
	}
	void	AddScriptStateValue(int iIndex, long lValue)
	{
		if(iIndex>0 && iIndex<SCRIPT_STATE_VALUE_COUNT)
			m_lScriptState[iIndex-1] += lValue;
	}

private:
	typedef HASH_STR(std::string,std::string) HDict;
	HDict	m_flagMap;

	//对象适时状态参数，脚本逻辑专用
	long	m_lScriptState[SCRIPT_STATE_VALUE_COUNT];
	
	// script
	DWORD     m_scriptCpuTime;	
	string    m_scriptName;
	PyObject *m_pModule,*m_pDict,*m_pFunc;
	
	// timer system support
	static TObjectList m_timerObjList;
	static void AddTimerObject(CRegionObject *object);
	static void	RemoveTimerObject(CRegionObject *object);
};

class CRegionFactory : public CRegionObject
{
public:
	// RT_DECLARE_DYNCREATE(CRegionFactory,CRegionObject,NULL,"")
	CRegionFactory();
	virtual ~CRegionFactory();

	CRegionObject *CreateObject(unsigned char type,bool cheat=false);
	CRegionObject *FindObject(ULONG oId);
	ULONG	   GetNextObjectId();
	ULONG      PeekNextObjectId();
	ULONG      GetObjectNum();
	void	   DestroyObject(CRegionObject *object);
	void	   RealDestroyObject(bool bRemoveTimerObject = false);
	void	   Exit();
	void       ReloadScript();
	TObjectHash& GetTObjectHash();//PZH
private:
	CRegionObject *CreateObjectByType(unsigned char type);
	TObjectHash m_hashObject;
	ULONG		m_objectId;
	TObjectList m_destroyList;
};

#endif 
