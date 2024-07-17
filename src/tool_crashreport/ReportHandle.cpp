#include "ReportHandle.h"
#include "DumpInfo.h"

#include "GlobalSet.h"
#include "ConfigHandle.h"
#include "Net_Ftp_Handle.h"

#include <time.h>

#include <XZip/XZip.h>

namespace CrashRP
{
	CReportHandle::CReportHandle()
		: mpFilter(NULL)
	{
	}

	CReportHandle::~CReportHandle()
	{
	}

	bool CReportHandle::Open()
	{
		if (CGlobalSet::Instance()->Load(__Text(".\\CrashReport.cfg")))
		{
			mpFilter = SetUnhandledExceptionFilter(ExceptionHandle);
		}

		return true;
	}

	bool CReportHandle::Close()
	{
		CGlobalSet::Instance()->Free();

		if (mpFilter)
		{
			mpFilter = SetUnhandledExceptionFilter(mpFilter);
		}

		return true;
	}

	long CReportHandle::Report(PEXCEPTION_POINTERS pExceptionInfo)
	{
		bool bIsCreateDmp = false;
		CGlobalSet::Instance()->Get_CreateDmpFile(bIsCreateDmp);

		Char szNetIp[256] = { 0 };
		CGlobalSet::Instance()->Get_NetworkIp(szNetIp, 256);

		// get local time fmt(年月日时分秒)
		Char szTime[256] = { 0 };
		{
			__time64_t t64 = _time64(NULL);

			struct tm tmT;
			errno_t err = _localtime64_s(&tmT, &t64);
			if (err != 0 || !_tcsftime(szTime, 256, __Text("%Y%m%d%H%M%S"), &tmT))
			{
				szTime[0] = '\0';
			}
		}

		if (bIsCreateDmp)
		{
			HANDLE hDumpFile = CreateFile(__Text("CrashReport.DMP"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
			if (hDumpFile)
			{
				MINIDUMP_EXCEPTION_INFORMATION ExpParam;
				ExpParam.ThreadId			= GetCurrentThreadId();
				ExpParam.ClientPointers		= TRUE;
				ExpParam.ExceptionPointers	= pExceptionInfo;

				MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

				CloseHandle(hDumpFile);
			}
		}

		// force load ~CDumpInfo() to close file
		{
			CDumpInfo DumpInfo(__Text("CrashReport.txt"));
			DumpInfo.Dump(pExceptionInfo);
		}

		// Comment by Wayne wong 2010-11-30
		/*
		Char szRptName[256];
		_stprintf_s(szRptName, 256, __Text("CrashReport[%s-%s].zip"), szNetIp, szTime);

		HZIP hZip = CreateZip(szRptName, 0, ZIP_FILENAME);
		if (hZip)
		{
			ZipAdd(hZip, __Text("CrashReport.txt"), __Text("CrashReport.txt"), 0, ZIP_FILENAME);
			ZipAdd(hZip, __Text("CrashReport.DMP"), __Text("CrashReport.DMP"), 0, ZIP_FILENAME);

			CloseZip(hZip);

			Char szServPath[256] = { 0 };
			CGlobalSet::Instance()->Get_ServPath(szServPath, 256);

			if (CNet_Ftp_Handle::Instance()->SendFile(__Text("xian.update"), __Text("oiwjoier@!#WERwer#$%#$eeewer"), szServPath, szRptName))
			{
				DeleteFile(szRptName);
			}
		}

		DeleteFile(__Text("CrashReport.txt"));
		DeleteFile(__Text("CrashReport.DMP"));
		*/

		return 1;
	}

	long __stdcall CReportHandle::ExceptionHandle(PEXCEPTION_POINTERS pExceptionInfo)
	{
		return CReportHandle::Instance()->Report(pExceptionInfo);
	}

	CReportHandle* CReportHandle::Instance()
	{
		static CReportHandle Inst;

		return &Inst;
	}
}