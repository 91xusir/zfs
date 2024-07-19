#pragma once
#include <Config.h>

namespace CrashRP
{
	class CDumpInfo
	{
	public:
		CDumpInfo(Char *pFileName);
		virtual ~CDumpInfo();
	public:
		bool Dump(PEXCEPTION_POINTERS pExceptionInfo);
	private:
		HANDLE mhDumpFile;
	};
}