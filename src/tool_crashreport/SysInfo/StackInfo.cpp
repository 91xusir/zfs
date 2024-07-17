#include "StackInfo.h"
#include "ModuleList.h"

namespace CrashRP
{
	CStackInfo::CStackInfo(PEXCEPTION_POINTERS pExceptionInfo)
		: mpExceptionInfo(pExceptionInfo)
	{
		strcpy_s(mInfoType, "[Stack Info]");
	}

	CStackInfo::~CStackInfo()
	{
	}

	bool CStackInfo::DumpInfo(HANDLE hDumpFile)
	{
		CONTEXT *pContext = mpExceptionInfo->ContextRecord;

		HANDLE hCurrentProcess = ::GetCurrentProcess();
		HANDLE hCurrentThread  = ::GetCurrentThread();

		STACKFRAME64 StackFrame;
		memset(&StackFrame, 0, sizeof(STACKFRAME64));

		StackFrame.AddrPC.Offset	= pContext->Eip;
		StackFrame.AddrPC.Mode		= AddrModeFlat;
		StackFrame.AddrStack.Offset	= pContext->Esp;
		StackFrame.AddrStack.Mode	= AddrModeFlat;
		StackFrame.AddrFrame.Offset	= pContext->Ebp;
		StackFrame.AddrFrame.Mode	= AddrModeFlat;

		Char szBuffer[1024];

		SymInitialize(hCurrentProcess, NULL, TRUE);

		Char_A *pComment = "[地址][偏移] 进程 函数\r\n"; UL32 uLen = (UL32)strlen(pComment);

		WriteFile(hDumpFile, pComment, uLen, &uLen, NULL);

		// IMAGE_FILE_MACHINE_IA64, IMAGE_FILE_MACHINE_AMD64, IMAGE_FILE_MACHINE_I386
		UL32 uMachineType = GetMachineType();

		while (StackWalk64(uMachineType, hCurrentProcess, hCurrentThread, &StackFrame, pContext, 0, 0, 0, 0))
		{
			if (StackFrame.AddrFrame.Offset == 0)
			{
				break;
			}

			Char_U SymBuffer[sizeof(SYMBOL_INFO) + 1024];
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)SymBuffer;

			pSymbol->SizeOfStruct = sizeof(SymBuffer);
			pSymbol->MaxNameLen = 512;

			if (!SymFromAddr(hCurrentProcess, StackFrame.AddrPC.Offset, 0, pSymbol))
			{
				pSymbol->Name[0] = 0;
			}

			PMODULEENTRY32 pModuleInfo = CModuleList::Instance()->QueryModuleInfoByAddress(StackFrame.AddrPC.Offset);
			if (!pModuleInfo)
			{
				Char szAddr[32];
				CDataConvert::AddressToString(StackFrame.AddrPC.Offset, szAddr, 32, true, 8);

				Char szTemp[128]; CDataConvert::AsciiToString(pSymbol->Name, szTemp, 128);

				_stprintf_s(szBuffer, __Text("[%s]  UnkownModule  %s\r\n"), szAddr, szTemp);
			} else
			{
				Char szBaseAddr[32];
				CDataConvert::AddressToString((UI64)(pModuleInfo->modBaseAddr), szBaseAddr, 32, true, 8);

				Char szOffsetAddr[32];
				CDataConvert::AddressToString((UI64)(StackFrame.AddrPC.Offset - (UL64)(pModuleInfo->modBaseAddr)), szOffsetAddr, 32, true, 8);

				Char szTemp[128]; CDataConvert::AsciiToString(pSymbol->Name, szTemp, 128);

				_stprintf_s(szBuffer, __Text("[%s][%s]  %s  %s\r\n"), szBaseAddr, szOffsetAddr, pModuleInfo->szModule, szTemp);
			}

			Char_A szTemp[1024 + 512]; CDataConvert::StringToAscii(szBuffer, szTemp, 1024 + 512);
			uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		SymCleanup(GetCurrentProcess());

		uLen = (UL32)strlen(mLine);

		WriteFile(hDumpFile, mLine, uLen, &uLen, NULL);

		return true;
	}

	US32 CStackInfo::GetMachineType()
	{
		US32 uType = IMAGE_FILE_MACHINE_I386;

		HINSTANCE hInstLib = LoadLibraryEx(__Text("user32.dll"), NULL, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
		if (hInstLib)
		{
			PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hInstLib;
			PIMAGE_NT_HEADERS pNTHeader  = (PIMAGE_NT_HEADERS)(pDOSHeader->e_lfanew + (DWORD_PTR)pDOSHeader);

			uType = pNTHeader->FileHeader.Machine;

			FreeLibrary(hInstLib);
		}

		return uType;
	}
}