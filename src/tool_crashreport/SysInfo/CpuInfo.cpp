#include "CpuInfo.h"

#include <AtlConv.h>

namespace CrashRP
{
	CCpuInfo::CCpuInfo(PEXCEPTION_POINTERS pExceptionInfo)
		: mpExceptionInfo(pExceptionInfo)
	{
		strcpy_s(mInfoType, "[CPU(REGISTER) Info]");
	}

	CCpuInfo::~CCpuInfo(void)
	{
	}

	bool CCpuInfo::DumpInfo(HANDLE hDumpFile)
	{
		CONTEXT *pContext = mpExceptionInfo->ContextRecord;

		Char szEAX[32];		CDataConvert::AddressToString(pContext->Eax,    szEAX,    32, true, 8);
		Char szEBX[32];		CDataConvert::AddressToString(pContext->Ebx,    szEBX,    32, true, 8);
		Char szECX[32];		CDataConvert::AddressToString(pContext->Ecx,    szECX,    32, true, 8);
		Char szEDX[32];		CDataConvert::AddressToString(pContext->Edx,    szEDX,    32, true, 8);
		Char szEDI[32];		CDataConvert::AddressToString(pContext->Edi,    szEDI,    32, true, 8);
		Char szESI[32];		CDataConvert::AddressToString(pContext->Esi,    szESI,    32, true, 8);
		Char szEBP[32];		CDataConvert::AddressToString(pContext->Ebp,    szEBP,    32, true, 8);
		Char szESP[32];		CDataConvert::AddressToString(pContext->Esp,    szESP,    32, true, 8);
		Char szEIP[32];		CDataConvert::AddressToString(pContext->Eip,    szEIP,    32, true, 8);
		Char szEFlags[32];	CDataConvert::AddressToString(pContext->EFlags, szEFlags, 32, true, 8);
		Char szCS[32];		CDataConvert::AddressToString(pContext->SegCs,  szCS,     32, true, 8);
		Char szDS[32];		CDataConvert::AddressToString(pContext->SegDs,  szDS,     32, true, 8);
		Char szES[32];		CDataConvert::AddressToString(pContext->SegEs,  szES,     32, true, 8);
		Char szFS[32];		CDataConvert::AddressToString(pContext->SegFs,  szFS,     32, true, 8);
		Char szGS[32];		CDataConvert::AddressToString(pContext->SegGs,  szGS,     32, true, 8);
		Char szSS[32];		CDataConvert::AddressToString(pContext->SegSs,  szSS,     32, true, 8);

		Char szDumpInfo[512];
		_stprintf_s(szDumpInfo, 512, __Text("EAX = %s , EBX = %s\r\nECX = %s , EDX = %s\r\nEDI = %s , ESI = %s\r\nEBP = %s , ESP = %s\r\nEIP = %s , EFlags = %s\r\nCS  = %s , DS  = %s\r\nES  = %s , FS  = %s\r\nGS  = %s , SS  = %s\r\n\r\n"),
			szEAX, szEBX, szECX, szEDX, szEDI, szESI, szEBP, szESP, szEIP, szEFlags, szCS, szDS, szES, szFS, szGS, szSS);

		Char_A szText[512]; CDataConvert::StringToAscii(szDumpInfo, szText, 512);
		UL32 uLen = (UL32)strlen(szText);

		WriteFile(hDumpFile, szText, uLen, &uLen, NULL);

		return true;
	}
}