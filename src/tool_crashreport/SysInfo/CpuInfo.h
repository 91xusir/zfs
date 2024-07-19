#pragma once

#include "BaseInfo.h"

namespace CrashRP
{
	class CCpuInfo
		: public CBaseInfo
	{
	public:
		CCpuInfo(PEXCEPTION_POINTERS pExceptionInfo);
		virtual ~CCpuInfo();
	protected:
		virtual bool DumpInfo(HANDLE hDumpFile);
		PEXCEPTION_POINTERS
				mpExceptionInfo;
	};
}