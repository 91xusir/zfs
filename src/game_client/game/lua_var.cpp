//-----------------------------------------------------------------------------
//-CLuaVar Implemention
//-----------------------------------------------------------------------------
#include "lua_var.h"

CLuaVar::CLuaVar()
{
	m_eType			= L_NIL;
	m_lData.ival	= 0;
}

CLuaVar::CLuaVar(E_LUATYPE type)
{
	m_eType	= type;

	switch(type)
	{
	case L_NIL:
		m_lData.ival = 0;
		break;
	case L_NUM:
		m_lData.ival = 0;
		break;
	case L_BOOL:
		m_lData.bval = false;
		break;
	case L_STR:
		m_lData.str  = "";
		break;
	case L_TABLE:
		m_lData.tbl.clear();
		break;
	}
}

CLuaVar::CLuaVar(LUA_NUMBER num)
{
	m_eType	= L_NUM;

	m_lData.ival = num;
}

CLuaVar::CLuaVar(bool bl)
{
	m_eType	= L_BOOL;

	m_lData.bval = bl;
}

CLuaVar::CLuaVar(const char* str)
{
	if(!str) return;

	m_eType	= L_STR;

	m_lData.str = str;
}

CLuaVar::CLuaVar(LuaTable tbl)
{
	m_eType	= L_TABLE;

	m_lData.tbl = tbl;
}

void CLuaVar::SetValue(CLuaVar& val)
{
	switch(val.Type())
	{
	case L_NIL:
		break;
	case L_NUM:
		SetNum(val.Data()->ival);
		break;
	case L_BOOL:
		SetBool(val.Data()->bval);
		break;
	case L_STR:
		SetStr(val.Data()->str.c_str());
		break;
	case L_TABLE:
		SetTable(val.Data()->tbl);
		break;
	default:
		break;
	}
}

