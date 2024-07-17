#pragma once

#ifndef _WINDOWS_
	#include <windows.h>
#endif

namespace CrashRP
{
	#ifdef CRASHRP_EXPORTS
		#define CrashAPI __declspec(dllexport)
	#else
		#define CrashAPI __declspec(dllimport)

	#   ifdef _DEBUG
	#       pragma comment(lib, "CrashReport_Debug.lib")
	#   else
	#       pragma comment(lib, "CrashReport.lib")
	#   endif
#endif

	extern "C"
	{
		// 启动
		CrashAPI bool Start();

		// 停止
		CrashAPI bool Stop();

		// 设置服务器信息
		CrashAPI bool SetServInfo(TCHAR *pServAddr, DWORD uServPort);
	}
}