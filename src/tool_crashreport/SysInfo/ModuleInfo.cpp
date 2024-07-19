#include "ModuleInfo.h"
#include "ModuleList.h"
#include "FileInfo.h"

namespace CrashRP
{
	CModuleInfo::CModuleInfo()
	{
		strcpy_s(mInfoType, "[Module Info]");
	}

	CModuleInfo::~CModuleInfo()
	{
	}

	bool CModuleInfo::DumpInfo(HANDLE hDumpFile)
	{
		ModuleInfoList *pModuleList = CModuleList::Instance()->GetModuleList();

		Char szText[512];
		for (ModuleInfoIter it = pModuleList->begin(); it != pModuleList->end(); it ++)
		{
			Char szAddr[32];
			CDataConvert::AddressToString((UI64)((*it)->modBaseAddr), szAddr, 32, true, 8);

			CFileInfo FileInfo((*it)->szExePath);

			Char szFileInfo[64];
			if (!FileInfo.GetProductVersion(szFileInfo, 64))
			{
				_stprintf_s(szText, __Text("[%s] %s\r\n"), szAddr, (*it)->szExePath);
			} else
			{
				_stprintf_s(szText, __Text("[%s] %s (%s)\r\n"), szAddr, (*it)->szExePath, szFileInfo);
			}

			Char_A szTemp[128]; CDataConvert::StringToAscii(szText, szTemp, 128);
			UL32 uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		UL32 uLen = (UL32)strlen(mLine);
		WriteFile(hDumpFile, mLine, uLen, &uLen, NULL);

		return true;
	}
}