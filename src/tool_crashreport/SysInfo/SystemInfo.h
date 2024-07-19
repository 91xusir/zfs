#pragma once

#include "BaseInfo.h"

namespace CrashRP
{
	class CSystemInfo
		: public CBaseInfo
	{
	public:
		CSystemInfo();
		virtual ~CSystemInfo();
	protected:
		virtual bool DumpInfo(HANDLE hDumpFile);
	private:
		bool GetOSVersionStr(Char *pOSVer, UL32 uLen);
	};
}