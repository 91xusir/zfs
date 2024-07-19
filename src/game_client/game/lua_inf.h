/******************************************************************************
 *-Lua Interface Header
 *****************************************************************************/
#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

#include "lua_var.h"
#include "lua_class.h"

using namespace std;

/******************************************************************************
* CILua : Interface with Lua
*		  including get lua variable/call lua function
*		  cFunction called by lua
******************************************************************************/
#define GET_PARA_TYPE(p)	\
			if(boost::is_integral<p>::value){sig += "i";}\
			else if(boost::is_floating_point<p>::value){sig += "d";}\
			else if(boost::is_pointer<p>::value){sig += "s";}

class CILua
{
public:
	CILua();
		
	~CILua()
	{
		if(m_pL)
		{
			lua_close(m_pL);
		}
	}

public:
	L_STATE* L() { return m_pL; }

/******************************************************************************
* stack operate function
******************************************************************************/
private:
	int GetTableFromStack(int index, CLuaVar* v);
public:
	void Push()							{ lua_pushnil(m_pL);				}
	void Push(const char* pstr)			{ lua_pushstring(m_pL, pstr);		}
	void Push(const std::string& str)	{ lua_pushlstring(m_pL, str.c_str(), 
														  str.size());		}
	void Push(int num)					{ lua_pushnumber(m_pL, (LUA_NUMBER)num);		}
	void Push(float num)				{ lua_pushnumber(m_pL, (LUA_NUMBER)num);		}
	void Push(double num)				{ lua_pushnumber(m_pL, (LUA_NUMBER)num);		}
	void Push(bool bl)					{ lua_pushboolean(m_pL, bl);		}
	void Push(C_FUNC  cf)				{ lua_pushcfunction(m_pL, cf);		}

	/**********************************************************
	 * return numbers of params
	 *********************************************************/
	int GetStackTopIndex()				{ return lua_gettop(m_pL);}

	int GetStackDataByIndex(int index, CLuaVar* v);

/******************************************************************************
* lua script operate function
******************************************************************************/
private:
	/******************************************************
	* only called by GetLuaData
	******************************************************/
	int Get(const char* name, CLuaVar* val);
public:		
	/******************************************************
	 * file_name : script file name							
	 ******************************************************/
	bool LoadScript(const char* file_name)
	{
		return lua_dofile(m_pL, file_name) == 0;
	}

	/******************************************************
	* buffer : lua stream							
	******************************************************/
	bool LoadScriptByString(const char* buffer)
	{
		return lua_dostring(m_pL, buffer) == 0;
	}

	/******************************************************
	* name : var name in lua
	* v    : return value
	* desc : used to get lua var's value from script
	******************************************************/
	template<typename T>
	int GetLuaData(const char* name, T* v)
	{
		if(!name || !v) return LUA_ERROR_UNKNOWN;

		CLuaVar l_var;

		if(Get(name, &l_var) != LUA_GET_DATA_SUCESS)
		{
			return LUA_GET_DATA_ERROR;
		}

		(*v) << l_var;

		return LUA_GET_DATA_SUCESS;
	}

	template<typename T>
	int SetLuaData(const char* name, T v)
	{
		if(!name) return LUA_ERROR_UNKNOWN;

		Push(v);

		lua_setglobal(m_pL, name);

		return LUA_SET_DATA_SUCESS;
	}

	/********************************************************
	*l_func: function name in lua
	*c_func: function name in c
	*desc	: c_func is registerd in lua as l_func by calling
	********************************************************/
	template<typename T>
	void RegCFunc(const char* l_func, T my_func)
	{
		Push(l_func);
		mylua_pushcfunc<T>(m_pL, my_func, 0);
		lua_settable(m_pL, LUA_GLOBALSINDEX);
	}

	/********************************************************
	 *func: function in lua used to be called 
	 *sig : const string like "did>sd" or "bids:ds"
	 *		b:bool d:double i:int s:string
	 *... : variable arguments according to 'sig' content
	 *desc	: c_func is registerd in lua as l_func by calling
	 *return: if calling correct, return null
	 *		  else return the error info
	 ********************************************************/
	const char* CallLFunc(const char *func, const char *sig = "", ...);

	/******************************************************************************
	* used to call lua function which hasnot return value
	******************************************************************************/
	const char* Call(const char* func)
	{
		return CallLFunc(func, "");
	}

	template<typename P1>
	const char* Call(const char* func, P1 arg1)
	{
		std::string sig = "";

		GET_PARA_TYPE(P1)

		return CallLFunc(func, sig.c_str(), arg1);
	}

	template<typename P1, typename P2>
	const char* Call(const char* func, P1 arg1, P2 arg2)
	{
		std::string sig = "";

		GET_PARA_TYPE(P1)
		GET_PARA_TYPE(P2)

		return CallLFunc(func, sig.c_str(), arg1, arg2);
	}

	template<typename P1, typename P2, typename P3>
	const char* Call(const char* func, P1 arg1, P2 arg2, P3 arg3)
	{
		std::string sig = "";

		GET_PARA_TYPE(P1)
		GET_PARA_TYPE(P2)
		GET_PARA_TYPE(P3)

		return CallLFunc(func, sig.c_str(), arg1, arg2, arg3);
	}

	template<typename P1, typename P2, typename P3, typename P4>
	const char* Call(const char* func, P1 arg1, P2 arg2, P3 arg3, P4 arg4)
	{
		std::string sig = "";

		GET_PARA_TYPE(P1)
		GET_PARA_TYPE(P2)
		GET_PARA_TYPE(P3)
		GET_PARA_TYPE(P4)

		return CallLFunc(func, sig.c_str(), arg1, arg2, arg3, arg4);
	}

	template<typename P1, typename P2, typename P3, typename P4, typename P5>
	const char* Call(const char* func, P1 arg1, P2 arg2, P3 arg3, P4 arg4, P5 arg5)
	{
		std::string sig = "";

		GET_PARA_TYPE(P1)
		GET_PARA_TYPE(P2)
		GET_PARA_TYPE(P3)
		GET_PARA_TYPE(P4)
		GET_PARA_TYPE(P5)

		return CallLFunc(func, sig.c_str(), arg1, arg2, arg3, arg4, arg5);
	}

	template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	const char* Call(const char* func, P1 arg1, P2 arg2, P3 arg3, P4 arg4, P5 arg5, P6 arg6)
	{
		std::string sig = "";

		GET_PARA_TYPE(P1)
		GET_PARA_TYPE(P2)
		GET_PARA_TYPE(P3)
		GET_PARA_TYPE(P4)
		GET_PARA_TYPE(P5)
		GET_PARA_TYPE(P6)

		return CallLFunc(func, sig.c_str(), arg1, arg2, arg3, arg4, arg5, arg6);
	}


	
public:
	template<typename UDT>
	void RegClass(const char* lib_name)
	{
		m_regLib.SetMyLibM();
		//m_regLib.SetTypeName(l_name);
		m_regLib.LuaOpenLibMember<UDT>(m_pL);
		m_regLib.LuaOpenLib<UDT>(m_pL, lib_name);
	}
private:
	L_STATE*	m_pL;
	//CRegFuncLib	m_regLib;
};

/******************************************************************************
* global stack operate function(include para L_STATE*)
******************************************************************************/
extern int GetStackTopIndex(L_STATE* pl);
extern int GetStackDataByIndex(L_STATE* pl, int index, CLuaVar* v);
extern int GetTableFromStack(L_STATE* pl, int index, CLuaVar* v);

extern inline void Push(L_STATE* pl)							{ lua_pushnil(pl);				}
extern inline void Push(L_STATE* pl, const char* pstr)			{ lua_pushstring(pl, pstr);		}
extern inline void Push(L_STATE* pl, const std::string& str)	{ lua_pushlstring(pl, str.c_str(), str.size());}
extern inline void Push(L_STATE* pl, int num)					{ lua_pushnumber(pl, (LUA_NUMBER)num);		}
extern inline void Push(L_STATE* pl, float num)					{ lua_pushnumber(pl, (LUA_NUMBER)num);		}
extern inline void Push(L_STATE* pl, double num)				{ lua_pushnumber(pl, (LUA_NUMBER)num);		}
extern inline void Push(L_STATE* pl, bool bl)					{ lua_pushboolean(pl, bl);		}
extern inline void Push(L_STATE* pl, C_FUNC  cf)				{ lua_pushcfunction(pl, cf);	}

/******************************************************************************
* register function proxy
* this is the function that really called in lua 
/*****************************************************************************/
template<typename F>
int GlobalRegCFunc(L_STATE* pl)
{
	/* pfunc is pointer to user's c function */
	void** pfunc = (void**)lua_touserdata(pl, lua_upvalueindex(1));	
	return Call(pl, (F)(*pfunc));
}

template<typename F>
void mylua_pushcfunc(L_STATE* pl, F fn, int n)
{
	unsigned char* pfunc = (unsigned char*)lua_newuserdata(pl, sizeof(F));
	memcpy(pfunc, &fn, sizeof(F));
	lua_pushcclosure(pl, GlobalRegCFunc<F>, n + 1);
}

/********************************************************
* According to numbers of para, which function called
* is decided
********************************************************/
#define GET_PARA2TMP(cnt)	for(int i = 0; i < cnt; i++)GetStackDataByIndex(pl, i-cnt, &v[i]);
template <typename RR>
int Call(L_STATE* pl, RR (*fn)())
{
	RR ret = (*fn)();
	Push(pl, ret);
	return 1;
}

template <typename RR, typename P1>
int Call(L_STATE* pl, RR (*fn)(P1))
{
	CLuaVar v[1];
	P1 arg;

	GET_PARA2TMP(1)

	arg << v[0];
	RR ret = (*fn)(arg);
	Push(pl, ret);
	return 1;
}

template <typename RR, typename P1, typename P2>
int Call(L_STATE* pl, RR (*fn)(P1, P2))
{
	CLuaVar v[2];
	P1 arg1;
	P2 arg2;

	GET_PARA2TMP(2)

	arg1 << v[0];
	arg2 << v[1];

	RR ret = (*fn)(arg1, arg2);
	Push(pl, ret);
	return 1;
}

template <typename RR, typename P1, typename P2, typename P3>
int Call(L_STATE* pl, RR (*fn)(P1, P2, P3))
{
	CLuaVar v[3];
	P1 arg1;
	P2 arg2;
	P3 arg3;

	GET_PARA2TMP(3)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];

	RR ret = (*fn)(arg1, arg2, arg3);
	Push(pl, ret);
	return 1;
}

template <typename RR, typename P1, typename P2, typename P3, typename P4>
int Call(L_STATE* pl, RR (*fn)(P1, P2, P3, P4))
{
	CLuaVar v[4];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;

	GET_PARA2TMP(4)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];

	RR ret = (*fn)(arg1, arg2, arg3, arg4);
	Push(pl, ret);
	return 1;
}

template <typename RR, typename P1, typename P2, typename P3, typename P4, typename P5>
int Call(L_STATE* pl, RR (*fn)(P1, P2, P3, P4, P5))
{
	CLuaVar v[5];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;
	P5 arg5;

	GET_PARA2TMP(5)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];
	arg5 << v[4];

	RR ret = (*fn)(arg1, arg2, arg3, arg4, arg5);
	Push(pl, ret);
	return 1;
}

template <typename RR, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
int Call(L_STATE* pl, RR (*fn)(P1, P2, P3, P4, P5, P6))
{
	CLuaVar v[6];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;
	P5 arg5;
	P6 arg6;

	GET_PARA2TMP(6)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];
	arg5 << v[4];
	arg6 << v[5];

	RR ret = (*fn)(arg1, arg2, arg3, arg4, arg5, arg6);
	Push(pl, ret);
	return 1;
}


inline int Call(L_STATE* pl, void (*fn)())
{
	L_STATE* unused = pl;	/* no warning */
	unused = NULL;

	(*fn)();
	return 0;
}

template<typename P1>
int Call(L_STATE* pl, void (*fn)(P1))
{
	CLuaVar v[1];
	P1 arg;

	GET_PARA2TMP(1)

	arg << v[0];
	(*fn)(arg);
	return 0;
}

template <typename P1, typename P2>
int Call(L_STATE* pl, void (*fn)(P1, P2))
{
	CLuaVar v[2];
	P1 arg1;
	P2 arg2;

	GET_PARA2TMP(2)

	arg1 << v[0];
	arg2 << v[1];

	(*fn)(arg1, arg2);
	return 0;
}

template <typename P1, typename P2, typename P3>
int Call(L_STATE* pl, void (*fn)(P1, P2, P3))
{
	CLuaVar v[3];
	P1 arg1;
	P2 arg2;
	P3 arg3;

	GET_PARA2TMP(3)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];

	(*fn)(arg1, arg2, arg3);
	return 0;
}

template <typename P1, typename P2, typename P3, typename P4>
int Call(L_STATE* pl, void (*fn)(P1, P2, P3, P4))
{
	CLuaVar v[4];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;

	GET_PARA2TMP(4)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];

	(*fn)(arg1, arg2, arg3, arg4);
	return 0;
}

template <typename P1, typename P2, typename P3, typename P4, typename P5>
int Call(L_STATE* pl, void (*fn)(P1, P2, P3, P4, P5))
{
	CLuaVar v[5];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;
	P5 arg5;

	GET_PARA2TMP(5)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];
	arg5 << v[4];

	(*fn)(arg1, arg2, arg3, arg4, arg5);
	return 0;
}

template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
int Call(L_STATE* pl, void (*fn)(P1, P2, P3, P4, P5, P6))
{
	CLuaVar v[6];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;
	P5 arg5;
	P6 arg6;

	GET_PARA2TMP(6)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];
	arg5 << v[4];
	arg6 << v[5];

	(*fn)(arg1, arg2, arg3, arg4, arg5, arg6);
	return 0;
}

/******************************************************************************
 *如果想返回多个值到lua，自己压栈，并且c函数必须返回int为返回值个数
 *****************************************************************************/

template <typename RR>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl))
{
	return (*fn)(pl);
}

template <typename RR, typename P1>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl, P1))
{
	CLuaVar v[1];
	P1 arg;

	GET_PARA2TMP(1)

	arg << v[0];
	return (*fn)(pl, arg);
}

template <typename RR, typename P1, typename P2>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl, P1, P2))
{
	CLuaVar v[2];
	P1 arg1;
	P2 arg2;

	GET_PARA2TMP(2)

	arg1 << v[0];
	arg2 << v[1];

	return (*fn)(pl, arg1, arg2);
}

template <typename RR, typename P1, typename P2, typename P3>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl, P1, P2, P3))
{
	CLuaVar v[3];
	P1 arg1;
	P2 arg2;
	P3 arg3;

	GET_PARA2TMP(3)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];

	return (*fn)(pl, arg1, arg2, arg3);
}

template <typename RR, typename P1, typename P2, typename P3, typename P4>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl, P1, P2, P3, P4))
{
	CLuaVar v[4];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;

	GET_PARA2TMP(4)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];

	return (*fn)(pl, arg1, arg2, arg3, arg4);
}

template <typename RR, typename P1, typename P2, typename P3, typename P4, typename P5>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl, P1, P2, P3, P4, P5))
{
	CLuaVar v[5];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;
	P5 arg5;

	GET_PARA2TMP(5)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];
	arg5 << v[4];

	return (*fn)(pl, arg1, arg2, arg3, arg4, arg5);
}

template <typename RR, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
int Call(L_STATE* pl, RR (*fn)(L_STATE* pl, P1, P2, P3, P4, P5, P6))
{
	CLuaVar v[6];
	P1 arg1;
	P2 arg2;
	P3 arg3;
	P4 arg4;
	P5 arg5;
	P6 arg6;

	GET_PARA2TMP(6)

	arg1 << v[0];
	arg2 << v[1];
	arg3 << v[2];
	arg4 << v[3];
	arg5 << v[4];
	arg6 << v[5];

	return (*fn)(pl, arg1, arg2, arg3, arg4, arg5, arg6);
}

#endif //LUA_INTERFACE_H