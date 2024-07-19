#pragma once
#include <Config.h>

namespace CrashRP
{
	class CReportHandle
	{
	public:
		CReportHandle();
		virtual ~CReportHandle();
	public:
		bool Open();
		bool Close();
	public:
		static CReportHandle* Instance();
	protected:
		long Report(PEXCEPTION_POINTERS pExceptionInfo);
	protected:
		static long __stdcall ExceptionHandle(PEXCEPTION_POINTERS pExceptionInfo);
	protected:
		LPTOP_LEVEL_EXCEPTION_FILTER
				mpFilter;
	};
}