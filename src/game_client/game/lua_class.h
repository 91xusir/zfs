
#ifndef _LUA_CLASS_H
#define _LUA_CLASS_H

#include "lua_inc.h"

//class CLuaVar;

#define LUA_REG_FIXED(l_fn)	\
			lua_pushliteral(pl, l_fn);\
			lua_pushvalue(pl, methods);\
			lua_settable(pl, metatable);

template <typename T, typename MFP>
class CRegClassHelper
{
	typedef struct
	{
		T *pT;
	}UDT;

public:
	struct RegLib
	{
		MFP	mfunc;
	};
	static void Register(L_STATE *pl)
	{
		lua_newtable(pl);
		int methods = lua_gettop(pl);

		luaL_newmetatable(pl, T::ClassName());
		int metatable = lua_gettop(pl);

		lua_pushstring(pl, T::ClassName());
		lua_pushvalue(pl, methods);
		lua_settable(pl, LUA_GLOBALSINDEX);

		LUA_REG_FIXED("__metatable")
		LUA_REG_FIXED("__index")
		LUA_REG_FIXED("__tostring")
		LUA_REG_FIXED("__gc")

		/* set metatable table */
		lua_newtable(pl);
		int mt = lua_gettop(pl);
		lua_pushliteral(pl, "__call");
		lua_pushcfunction(pl, New);
		lua_pushliteral(pl, "new");
		lua_pushvalue(pl, -2);           // dup New function
		lua_settable(pl, methods);       // add New to method table
		lua_settable(pl, mt);            // mt.__call = New
		lua_setmetatable(pl, methods);

		lua_pop(pl, 2);  // drop metatable and method table
	}

	static void RegMemberFunc(L_STATE* pl, const char* name, MFP fn)
	{
		lua_getglobal(pl, T::ClassName());

		if(lua_isnil(pl, -1))
		{
			lua_pop(pl, 1);
			Register(pl);
			lua_getglobal(pl, T::ClassName());
		}

		int idx = lua_gettop(pl);

		RegLib	lib;
		lib.mfunc = fn;

		lua_pushstring(pl, name);
		RegLib* p = (RegLib*)lua_newuserdata(pl, sizeof(RegLib));
		memcpy(p, &lib, sizeof(RegLib));
		lua_pushcclosure(pl, Dispatcher, 1);
		lua_settable(pl, idx);

		lua_pop(pl, 1);
	}

	/* get userdata from Lua stack and return pointer to T object */
	static T *check(L_STATE *pl, int narg)
	{
		UDT *ud = (UDT*)(luaL_checkudata(pl, narg, T::ClassName()));

		if(!ud) luaL_typerror(pl, narg, T::ClassName());
		return ud->pT;
	}

private:
	CRegClassHelper();

	/************************************************************************ 
	* member function dispatcher
	************************************************************************/
	static int Dispatcher(L_STATE *pl)
	{
		T *obj = check(pl, 1);	/* userdata follow by args, so get 'this' pointer*/
		lua_remove(pl, 1);		/* remove obj so method args start at index 1 */

		/* get member function from upvalue */
		RegLib* p = (RegLib*)(lua_touserdata(pl, lua_upvalueindex(1)));

		return _Call(pl, obj, p->mfunc);
	}

	#define GET_PARATMP(cnt) \
		for(int i = 0; i < cnt; i++)GetStackDataByIndex(pl, i-cnt, &v[i]);
	template<typename RR>
	static int _Call(L_STATE *pl, T* obj, RR (T::*fn)())
	{
		RR ret = (obj->*(fn))();
		Push(pl, ret);
		return 1;
	}

	template<typename RR, typename P1>
	static int _Call(L_STATE *pl, T* obj, RR (T::*fn)(P1))
	{
		CLuaVar v[1];
		P1 arg;

		GET_PARATMP(1)

		arg << v[0];
		RR ret = (obj->*(fn))(arg);
		Push(pl, ret);
		return 1;
	}

	template<typename RR, typename P1, typename P2>
	static int _Call(L_STATE *pl, T* obj, RR (T::*fn)(P1, P2))
	{
		CLuaVar v[2];
		P1 arg1;
		P2 arg2;

		GET_PARATMP(2)

		arg1 << v[0];
		arg2 << v[1];
		RR ret = (obj->*(fn))(arg1, arg2);
		Push(pl, ret);
		return 1;
	}


	static int _Call(L_STATE *pl, T* obj, void (T::*fn)())
	{
		L_STATE* unused = pl; /* no warning */
		unused = NULL;

		(obj->*(fn))();
		return 0;
	}

	template<typename P1>
	static int _Call(L_STATE *pl, T* obj, void (T::*fn)(P1))
	{
		L_STATE* unused = pl; /* no warning */
		unused = NULL;
		
		CLuaVar v[1];
		P1 arg;

		GET_PARATMP(1)

		arg << v[0];
		(obj->*(fn))(arg);
		return 0;
	}

	template<typename P1, typename P2>
	static int _Call(L_STATE *pl, T* obj, void (T::*fn)(P1, P2))
	{
		L_STATE* unused = pl; /* no warning */
		unused = NULL;
		
		CLuaVar v[2];
		P1 arg1;
		P2 arg2;

		GET_PARATMP(2)

		arg1 << v[0];
		arg2 << v[1];
		(obj->*(fn))(arg1, arg2);

		return 0;
	}

	/************************************************************************* 
	 * create a new T object and
	 * push onto the Lua stack a userdata containing a pointer to T object
     ************************************************************************/
	static int New(L_STATE *pl)
	{
		lua_remove(pl, 1);   // use classname:new(), instead of classname.new()
		T *obj	= new T(); 
		UDT *ud	= (UDT*)(lua_newuserdata(pl, sizeof(UDT)));

		ud->pT = obj;
		luaL_getmetatable(pl, T::ClassName());
		lua_setmetatable(pl, -2);
		return 1;  // userdata containing pointer to T object
	}

	/************************************************************************* 
	* garbage collection metamethod
	************************************************************************/
	static int Gc(L_STATE *pl)
	{
		UDT *ud = static_cast<UDT*>(lua_touserdata(pl, 1));
		T *obj = ud->pT;
		delete obj;
		return 0;
	}

	static int ToString (L_STATE *pl)
	{
		char buff[32];
		UDT *ud = static_cast<UDT*>(lua_touserdata(pl, 1));
		T *obj = ud->pT;
		rt2_sprintf(buff, "%p", obj);
		lua_pushfstring(pl, "%s (%s)", T::ClassName(), buff);
		return 1;
	}
};

#endif //_LUA_CLASS_H


