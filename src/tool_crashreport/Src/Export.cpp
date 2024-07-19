#include "../Include/CrashReport/Export.h"
#include "../ReportHandle.h"

namespace CrashRP
{
	// 启动
	bool Start()
	{
		return CReportHandle::Instance()->Open();
	}

	// 停止
	bool Stop()
	{
		return CReportHandle::Instance()->Close();
	}

	// 设置服务器信息
	bool SetServInfo(TCHAR *pServAddr, DWORD uServPort)
	{
		return false;
	}
}