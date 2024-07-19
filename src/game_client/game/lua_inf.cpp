/******************************************************************************
 *-Lua Interface CPP
 *****************************************************************************/
#include "lua_inf.h"

#define CHECK_NULL(p)	{ if(!(p)) return LUA_ERROR_UNKNOWN; }

CILua::CILua()
{
	m_pL = lua_open();

	luaopen_base(m_pL);
	luaopen_io(m_pL);
	luaopen_string(m_pL);
	luaopen_math(m_pL);
	luaopen_table(m_pL);

#ifdef _DEBUG
	luaopen_debug(m_pL);
#endif
}

int CILua::GetTableFromStack(int index, CLuaVar* v)
{
	CHECK_NULL(v)
	
	LuaTable	tbl;

	if(!lua_istable(m_pL, index))
	{			
		return LUA_STACK_IO_ERROR;
	}

	/* notice: here pushing a value to the stack */
	lua_pushnil(m_pL);

	if(index < 0)
	{
		index -= 1;
	}

	for(; lua_next(m_pL, index) != 0;)
	{
		CLuaVar		imap;
		CLuaVar		vmap;

		switch(lua_type(m_pL, -2))
		{
		case LUA_TNIL:
			break;
		case LUA_TNUMBER:
			imap = (LUA_NUMBER)lua_tonumber(m_pL, -2);
			break;
		case LUA_TBOOLEAN:
			imap = lua_toboolean(m_pL, -2) ? true : false;
			break;
		case LUA_TSTRING:
			imap = (char*)lua_tostring(m_pL, -2);
			break;
		default:
			break;
		}

		switch(lua_type(m_pL, -1))
		{
		case LUA_TNIL:
			break;
		case LUA_TNUMBER:
			vmap = (LUA_NUMBER)lua_tonumber(m_pL, -1);
			break;
		case LUA_TBOOLEAN:
			vmap = lua_toboolean(m_pL, -1) ? true : false;
			break;
		case LUA_TSTRING:
			vmap = (char*)lua_tostring(m_pL, -1);
			break;
		case LUA_TTABLE:
			if(GetTableFromStack(lua_gettop(m_pL), &vmap) != LUA_STACK_IO_SUCCESS)
			{
				return LUA_STACK_IO_ERROR;
			}
			vmap.SetType(L_TABLE);
			break;
		default:
			break;
		}

		tbl[imap] = vmap;

		lua_pop(m_pL, 1);
	}

	*v = tbl;

	return LUA_STACK_IO_SUCCESS;
}

int CILua::GetStackDataByIndex(int index, CLuaVar* v)
{
	CHECK_NULL(v)
	//CHECK_INDEX(index)

	int n = GetStackTopIndex();
							
	if( (index > n)		||
		(index < -n)	||
		(index == 0))
	{
		return LUA_STACK_OVERFLOW;
	}

	switch(lua_type(m_pL, index))
	{
	case LUA_TNIL:
		break;
	case LUA_TNUMBER:
		*v = (LUA_NUMBER)lua_tonumber(m_pL, index);
		break;
	case LUA_TBOOLEAN:
		*v = lua_toboolean(m_pL, index) ? true : false;
		break;
	case LUA_TSTRING:
		*v = (char*)lua_tostring(m_pL, index);
		break;
	case LUA_TTABLE:
		if(GetTableFromStack(index, v) != LUA_STACK_IO_SUCCESS)
		{
			return LUA_STACK_OVERFLOW;
		}
		break;
	default:
		break;
	}
	
	return LUA_STACK_IO_SUCCESS;
}

int CILua::Get(const char* name, CLuaVar* val)
{
	CHECK_NULL(name)
	CHECK_NULL(val)

	int ret = LUA_GET_DATA_SUCESS;

	lua_getglobal(m_pL, name);

	switch(lua_type(m_pL, -1))
	{
	case LUA_TNIL:
		break;
	case LUA_TNUMBER:
		*val = (LUA_NUMBER)lua_tonumber(m_pL, -1);
		break;
	case LUA_TBOOLEAN:
		*val = lua_toboolean(m_pL, -1) ? true : false;
		break;
	case LUA_TSTRING:
		*val = (char*)lua_tostring(m_pL, -1);
		break;
	case LUA_TTABLE:
		if(GetTableFromStack(-1, val) != LUA_STACK_IO_SUCCESS)
		{
			ret = LUA_GET_DATA_ERROR;
			break;
		}
		break;
	default:
		ret = LUA_GET_DATA_ERROR;
		break;
	}

	lua_pop(m_pL, 1);

	return ret;
}

const char* CILua::CallLFunc(const char *func, const char *sig, ...)
{
	int		narg = 0;		/* numbers of arguments */
	int		nres = 0;		/* numbers of results */
	bool	barg = false;	/* list of arg is end */
	int		index = 0;		/* index */

	va_list vl;
	const char* sresult = NULL;	/* return value */

	va_start(vl, sig);

	lua_getglobal(m_pL, func);

	while(*sig)
	{
		switch(*sig)
		{
		case 'd':	/* double arguments */
			lua_pushnumber(m_pL, va_arg(vl, double));
			break;
		case 'i':	/* int arguments */
			lua_pushnumber(m_pL, va_arg(vl, int));
			break;
		case 'b':	/* bool arguments */
			lua_pushboolean(m_pL, va_arg(vl, int));
			break;
		case 's':	/* string arguments */
			lua_pushstring(m_pL, va_arg(vl, char*));
			break;
		case '>':	/* arguments list end */
		case ':':	/* arguments list end */
			barg = true;
			break;
		default:
			break;
		}

		sig++;
		luaL_checkstack(m_pL, 1, "too many arguments");

		if(barg) break;
		narg++;
	}

	nres = (int)strlen(sig);		/* numbers of expected results */

	if(lua_pcall(m_pL, narg, nres, 0) != 0)
	{
		sresult = lua_tostring(m_pL, -1);
		nres = 1;
	}else
	{
		index = -nres;
		//for(; *sig; sig++)
		while(*sig)
		{
			switch(*sig)
			{
			case 'd':
				if(!lua_isnumber(m_pL, index))
				{
					//
				}

				*va_arg(vl, double*) = (double)lua_tonumber(m_pL, index);
				break;
			case 'i':
				if(!lua_isnumber(m_pL, index))
				{
					//
				}

				*va_arg(vl, int*) = (int)lua_tonumber(m_pL, index);
				break;
			case 'b':
				if(!lua_isboolean(m_pL, index))
				{
					//
				}

				*va_arg(vl, int*) = (int)lua_tonumber(m_pL, index);
				break;
			case 's':
				if(!lua_isstring(m_pL, index))
				{
					//
				}

				*va_arg(vl, const char**) = (char*)lua_tostring(m_pL, index);
				//strcpy((char*)(*va_arg(vl, const char**)), (char*)lua_tostring(m_pL, index));
				break;
			default:
				break;
			}

			index++;
			sig++;
		}
	}

	va_end(vl);

	lua_pop(m_pL, nres);

	return sresult;
}

/******************************************************
* global stack operate function(include para L_STATE*)
******************************************************/
int GetStackTopIndex(L_STATE* pl)
{
	return lua_gettop(pl);
}

int GetTableFromStack(L_STATE* pl, int index, CLuaVar* v)
{
	CHECK_NULL(v)

	LuaTable	tbl;

	if(!lua_istable(pl, index))
	{			
		return LUA_STACK_IO_ERROR;
	}

	lua_pushnil(pl);

	if(index < 0)
	{
		index -= 1;
	}

	for(; lua_next(pl, index) != 0;)
	{
		CLuaVar		imap;
		CLuaVar		vmap;

		switch(lua_type(pl, -2))
		{
		case LUA_TNIL:
			break;
		case LUA_TNUMBER:
			imap = (double)lua_tonumber(pl, -2);
			break;
		case LUA_TBOOLEAN:
			imap = lua_toboolean(pl, -2) ? true : false;
			break;
		case LUA_TSTRING:
			imap = (char*)lua_tostring(pl, -2);
			break;
		default:
			break;
		}

		switch(lua_type(pl, -1))
		{
		case LUA_TNIL:
			break;
		case LUA_TNUMBER:
			vmap = (double)lua_tonumber(pl, -1);
			break;
		case LUA_TBOOLEAN:
			vmap = lua_toboolean(pl, -1) ? true : false;
			break;
		case LUA_TSTRING:
			vmap = (char*)lua_tostring(pl, -1);
			break;
		case LUA_TTABLE:
			if(GetTableFromStack(pl, index, &vmap) != LUA_STACK_IO_SUCCESS)
			{
				return LUA_STACK_IO_ERROR;
			}
			break;
		default:
			break;
		}

		tbl[imap] = vmap;

		lua_pop(pl, 1);
	}

	*v = tbl;

	return LUA_STACK_IO_SUCCESS;
}

int GetStackDataByIndex(L_STATE* pl, int index, CLuaVar* v)
{
	CHECK_NULL(v)
	//CHECK_INDEX(index)

	int n = GetStackTopIndex(pl);

	if( (index > n)		||
		(index < -n)	||
		(index == 0))
	{
		return LUA_STACK_OVERFLOW;
	}

	switch(lua_type(pl, index))
	{
	case LUA_TNIL:
		break;
	case LUA_TNUMBER:
		*v = (LUA_NUMBER)lua_tonumber(pl, index);
		break;
	case LUA_TBOOLEAN:
		*v = lua_toboolean(pl, index) ? true : false;
		break;
	case LUA_TSTRING:
		*v = (char*)lua_tostring(pl, index);
		break;
	case LUA_TTABLE:
		if(GetTableFromStack(pl, index, v) != LUA_STACK_IO_SUCCESS)
		{
			return LUA_STACK_OVERFLOW;
		}
		break;
	default:
		break;
	}

	return LUA_STACK_IO_SUCCESS;
}
