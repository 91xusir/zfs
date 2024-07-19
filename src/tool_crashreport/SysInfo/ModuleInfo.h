#pragma once

#include "BaseInfo.h"

namespace CrashRP
{
	class CModuleInfo
		: public CBaseInfo
	{
	public:
		CModuleInfo();
		virtual ~CModuleInfo();
	protected:
		virtual bool DumpInfo(HANDLE hDumpFile);
	};
}