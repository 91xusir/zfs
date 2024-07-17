#pragma once

#include "BaseInfo.h"

namespace CrashRP
{
	class CStackInfo
		: public CBaseInfo
	{
	public:
		CStackInfo(PEXCEPTION_POINTERS pExceptionInfo);
		virtual ~CStackInfo();
	protected:
		virtual bool DumpInfo(HANDLE hDumpFile);
	protected:
		US32 GetMachineType();
	private:
		PEXCEPTION_POINTERS mpExceptionInfo;
	};
}