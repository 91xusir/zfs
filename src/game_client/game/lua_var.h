/****************************************************************************
 * Lua Variant
 ****************************************************************************/
#ifndef LUA_VAR_H
#define LUA_VAR_H

#include "lua_inc.h"

using namespace std;

/******************************************************
* struct LVar: member of CLuaVar
* represent variable used in lua
* number/boolean/string/nil/table
******************************************************/
typedef struct
{
	LUA_NUMBER		ival;
	bool			bval;
	std::string		str;
	LuaTable		tbl;
}LVar;

/******************************************************
* CLuaVar: used to access lua variable
* number/boolean/string/nil/table
******************************************************/
class CLuaVar
{
public:
	CLuaVar();
	CLuaVar(E_LUATYPE type);
	CLuaVar(LUA_NUMBER num);
	CLuaVar(bool bl);
	CLuaVar(const char* str);
	CLuaVar(LuaTable tbl);

public:
	const E_LUATYPE Type() const		{ return m_eType;					}
	const LVar* Data() const			{ return &m_lData;					}
	bool  SetType(E_LUATYPE t)			{ m_eType = t; return true;			}
	bool  SetNum(LUA_NUMBER num)		{ m_lData.ival = num; return true;	}
	bool  SetBool(bool bv)				{ m_lData.bval = bv; return true;	}
	bool  SetStr(const char* str)		{ m_lData.str = str; return true;	}
	bool  SetTable(const LuaTable& tbl)	{ m_lData.tbl = tbl; return true;	}

	void  SetValue(CLuaVar& val);

	const double	GetNum()	const	{ if(m_eType == L_NUM) return m_lData.ival; else return 0; }
	const bool		GetBool()	const	{ if(m_eType == L_BOOL) return m_lData.bval; else return false; }
	const char*		GetStr()	const	{ if(m_eType == L_STR) return m_lData.str.c_str(); else return NULL; }
	const LuaTable&	GetTable()	const	
	{ 
		if(m_eType == L_TABLE) return m_lData.tbl; 

		static LuaTable nulltbl;

		return nulltbl;
	}

	const std::string&	GetCMStr()const	{ if(m_eType == L_STR) return m_lData.str; }

public:
	/******************************************************************************
	* operater = overloaded start¡¡¡ý
	******************************************************************************/
	const CLuaVar& operator = (const CLuaVar& rhs)
	{
		m_lData = rhs.m_lData;
		m_eType = rhs.m_eType;

		return *this;
	}

	const CLuaVar& operator = (int num)
	{		
		m_lData.ival = (LUA_NUMBER)num;
		m_eType = L_NUM;

		m_lData.bval = false;
		m_lData.str.clear();
		m_lData.tbl.clear();

		return *this;
	}

	const CLuaVar& operator = (double num)
	{		
		m_lData.ival = (LUA_NUMBER)num;
		m_eType = L_NUM;

		m_lData.bval = false;
		m_lData.str.clear();
		m_lData.tbl.clear();

		return *this;
	}

	const CLuaVar& operator = (const char* str)
	{		
		m_lData.str = str;
		m_eType = L_STR;

		m_lData.ival = 0;
		m_lData.bval = false;
		m_lData.tbl.clear();

		return *this;
	}

	const CLuaVar& operator = (bool bl)
	{
		m_lData.bval = bl;
		m_eType = L_BOOL;

		m_lData.ival = 0;
		m_lData.str.clear();
		m_lData.tbl.clear();

		return *this;
	}

	const CLuaVar& operator = (LuaTable tbl)
	{
		m_lData.tbl = tbl;
		m_eType = L_TABLE;

		m_lData.ival = 0;
		m_lData.bval = false;
		m_lData.str.clear();

		return *this;
	}
	/******************************************************************************
	* operater = overloaded end¡¡¡ü
	******************************************************************************/

	/******************************************************************************
	* operater += overloaded start¡¡¡ý
	******************************************************************************/
	template<typename T>
	friend T& operator += (T& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_NUM)
		{
			lhs += (T)rhs.GetNum();
		}
		return lhs;
	}
	/******************************************************************************
	* operater += overloaded end¡¡¡ü
	******************************************************************************/

	/******************************************************************************
	* operater << overloaded start¡¡¡ý
	******************************************************************************/
	friend ostream& operator << (ostream& lhs, const CLuaVar& rhs)
	{
		switch(rhs.Type())
		{
		case L_NUM:
			lhs<<rhs.GetNum();
			break;
		case L_STR:
			lhs<<rhs.GetStr();
			break;
		case L_BOOL:
			lhs<<rhs.GetBool();
			break;
		case L_TABLE:
			{
				lhs<<endl;
				LuaTable tbl;
				tbl << rhs.GetTable();

				LuaTable::iterator it = tbl.begin();

				for(; it != tbl.end(); it++)
				{					
					lhs<<"	"<<it->first<<" : "<<it->second<<endl;
				}
			}
		default:
			break;
		}

		return lhs;
	}

	friend CLuaVar& operator << (CLuaVar& lhs, const CLuaVar& rhs)
	{
		lhs.m_lData = rhs.m_lData;
		lhs.m_eType = rhs.m_eType;

		return lhs;
	}

	friend int&		operator << (int& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_NUM)	lhs = (int)rhs.GetNum();
		return lhs;
	}

	friend short&	operator << (short& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_NUM)	lhs = (short)rhs.GetNum();
		return lhs;
	}

	friend float&	operator << (float& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_NUM)	lhs = (float)rhs.GetNum();
		return lhs;
	}

	friend double&	operator << (double& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_NUM)	lhs = rhs.GetNum();
		return lhs;
	}

	friend bool&	operator << (bool& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_BOOL) lhs = rhs.GetBool();
		return lhs;
	}

	friend std::string&		operator << (std::string& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_STR)	lhs = rhs.GetStr();
		return lhs;
	}

	friend char*&		operator << (char*& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_STR)	lhs = (char*)rhs.GetStr();
		return lhs;
	}

	friend char const*&	operator << (char const*& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_STR)	lhs = (char*)rhs.GetStr();
		return lhs;
	}

	friend char**		operator << (char** lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_STR)	(*lhs) = (char*)rhs.GetStr();
		return lhs;
	}

	friend LuaTable&		operator << (LuaTable& lhs, const CLuaVar& rhs)
	{
		if(rhs.Type() == L_TABLE) lhs = rhs.GetTable();
		return lhs;
	}
	friend void*			operator << (void* lhs, const CLuaVar& rhs)
	{
		if(!lhs) return NULL;
		
		switch(rhs.Type())
		{
		case L_NIL:
			return NULL;
			break;
		case L_NUM:
			*(double*)lhs = rhs.GetNum();
			break;
		case L_BOOL:
			*(bool*)lhs = rhs.GetBool();
			break;
		case L_STR:
			lhs = (char*)rhs.GetStr();
			break;
		case L_TABLE:
			break;
		default:
			break;
		}

		return lhs;
	}
	/**************************************************************************
	* operater << overloaded end¡¡¡ü
	**************************************************************************/

private:
	LVar		m_lData;
	E_LUATYPE	m_eType;
};

/******************************************************************************
* used to sort
******************************************************************************/
inline bool operator < (const CLuaVar& l, const CLuaVar& r)
{
	switch(l.Type())
	{
	case L_NUM:
		return l.GetNum() < r.GetNum();
		break;
	case L_BOOL:
		return l.GetBool() == r.GetBool();
		break;
	case L_STR:
		return strcmp(l.GetStr(), r.GetStr()) < 0;
		//return l.GetCMStr() < r.GetCMStr() < 0;
		break;
	default:
		break;
	}

	return strcmp(l.GetStr(), r.GetStr()) < 0;
	//return l.GetCMStr() < r.GetCMStr() < 0;
}

#endif //LUA_VAR_H