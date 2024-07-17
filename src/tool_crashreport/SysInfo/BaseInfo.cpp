#include "BaseInfo.h"

namespace CrashRP
{
	CBaseInfo::CBaseInfo(void)
	{
		mInfoType[0] = 0;

		strcpy_s(mLine, "\r\n");
	}

	CBaseInfo::~CBaseInfo(void)
	{
	}

	bool CBaseInfo::Dump(HANDLE hDumpFile)
	{
		UL32 uLen = (UL32)strlen(mInfoType);
		WriteFile(hDumpFile, mInfoType, uLen, &uLen, NULL);

		Char_A *pLine = "\r\n"; uLen = (UL32)strlen(pLine);
		WriteFile(hDumpFile, pLine, uLen, &uLen, NULL);

		return DumpInfo(hDumpFile);
	}
}