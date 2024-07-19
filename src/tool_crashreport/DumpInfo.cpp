#include "DumpInfo.h"
#include "GlobalSet.h"

#include "SysInfo/ModuleList.h"

#include "SysInfo/CpuInfo.h"
#include "SysInfo/StackInfo.h"
#include "SysInfo/ModuleInfo.h"
#include "SysInfo/SystemInfo.h"

namespace CrashRP
{
	CDumpInfo::CDumpInfo(Char *pFileName)
	{
		mhDumpFile = CreateFile(pFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	CDumpInfo::~CDumpInfo()
	{
		if (mhDumpFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(mhDumpFile); mhDumpFile = INVALID_HANDLE_VALUE;
		}
	}

	bool CDumpInfo::Dump(PEXCEPTION_POINTERS pExceptionInfo)
	{
		if (mhDumpFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		CModuleList::Instance()->Update();

		bool bIsDump = false;

		CGlobalSet::Instance()->Get_DumpSys(bIsDump);
		if (bIsDump)
		{
			CSystemInfo SysInfo;
			SysInfo.Dump(mhDumpFile);
		}

		CGlobalSet::Instance()->Get_DumpCpu(bIsDump);
		if (bIsDump)
		{
			CCpuInfo CPUInfo(pExceptionInfo);
			CPUInfo.Dump(mhDumpFile);
		}

		CGlobalSet::Instance()->Get_DumpSta(bIsDump);
		if (bIsDump)
		{
			CStackInfo StackInfo(pExceptionInfo);
			StackInfo.Dump(mhDumpFile);
		}

		CGlobalSet::Instance()->Get_DumpMod(bIsDump);
		if (bIsDump)
		{
			CModuleInfo ModuleInfo;
			ModuleInfo.Dump(mhDumpFile);
		}

		return true;
	}
}