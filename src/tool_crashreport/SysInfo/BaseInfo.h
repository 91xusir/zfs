#pragma once

#include "../DataConvert.h"

namespace CrashRP
{
	class CBaseInfo
	{
	public:
		CBaseInfo();
		virtual ~CBaseInfo();
	public:
		bool Dump(HANDLE hDumpFile);
	protected:
		virtual bool DumpInfo(HANDLE hDumpFile) = 0;
	protected:
		Char_A mLine[3];
		Char_A mInfoType[64];
	};
}