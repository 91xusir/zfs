#pragma once
#include <map>
#include <list>
#include <vector>
#include <string>

#include <stdio.h>
#include <tchar.h>

#ifndef WINVER
	#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS
	#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
	#define _WIN32_IE 0x0600
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp")

namespace CrashRP
{
	typedef int					I32;
	typedef unsigned int		UI32;
	typedef __int64				I64;
	typedef unsigned __int64	UI64;
	typedef short				S32;
	typedef unsigned short		US32;
	typedef long				L32;
	typedef unsigned long		UL32;
	typedef long long			L64;
	typedef unsigned long long	UL64;
	typedef char				Char_A;
	typedef unsigned char		Char_U;
	typedef wchar_t				Char_W;

	typedef float				Real32;
	typedef double				Real64;

#ifdef _UNICODE
	typedef wchar_t				Char;
	#define	__Text(text)		L##text
#else
	typedef char				Char;
	#define	__Text(text)		text
#endif
}