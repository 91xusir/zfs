
#ifndef LUA_INC_H
#define LUA_INC_H

//#ifdef _DEBUG
//	#pragma comment(lib, "lua_mdd.lib")
//	#pragma comment(lib, "lualib_mdd.lib")
//#else
//	#pragma comment(lib, "lua_md.lib")
//	#pragma comment(lib, "lualib_md.lib")
//#endif

#include <string>
#include <iostream>
#include <stdarg.h>
#include <map>
#include <vector>
//#include <boost/type_traits.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

typedef enum E_LUATYPE
{
	L_NIL	= 0,	/* NIL		*/
	L_NUM	= 1,	/* NUMBER	*/
	L_BOOL	= 2,	/* BOOLEAN	*/
	L_STR	= 3,	/* STRING	*/	
	L_TABLE	= 4		/* TABLE	*/
}E_LUATYPE;

typedef enum E_LUAERROR
{
	LUA_GET_DATA_SUCESS		= 0,
	LUA_GET_DATA_ERROR		= 1,
	LUA_SET_DATA_SUCESS		= 2,
	LUA_SET_DATA_ERROR		= 3,

	LUA_STACK_IO_SUCCESS	= 4,
	LUA_STACK_OVERFLOW		= 5,
	LUA_STACK_IO_ERROR		= 6,

	LUA_ERROR_UNKNOWN		= 0xffff
}E_LUAERROR;

typedef lua_CFunction	C_FUNC;
typedef lua_State		L_STATE;

template <bool B>
struct bool2type
{
	enum { value = B };
};

typedef bool2type<true> PType;
typedef bool2type<false> VType;

class CLuaVar;
typedef std::map<CLuaVar, CLuaVar> LuaTable;

/* useful macro */
/******************************************************************************
* macro used to reg c function
******************************************************************************/
#define REG_CFUNC(lua, fn)							(lua).RegCFunc(#fn, fn);
#define REG_OVERLOADED_CFUNC(lua, l_fn, t, fn)		(lua).RegCFunc<t>(#l_fn, fn);

/******************************************************************************
* macro used to reg cpp class
******************************************************************************/
#define REG_CLASS_NAME(klass)	\
		public:\
		inline static const char* ClassName() { return klass; }
#define REG_CLASS_METHOD(lua, UDT, type, func)	\
	CRegClassHelper<UDT, type>::RegMemberFunc((lua).L(), #func, &UDT::func);
#define REG_CLASS_OVERLOADED_METHOD(lua, UDT, type, l_fn, func)	\
	CRegClassHelper<UDT, type>::RegMemberFunc((lua).L(), #l_fn, &UDT::func);

#endif //LUA_INC_H