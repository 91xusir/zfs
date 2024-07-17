#pragma once
#include <Config.h>

namespace CrashRP
{
	class CNet_Ftp_Handle
	{
	protected:
		CNet_Ftp_Handle();
		virtual ~CNet_Ftp_Handle();
	public:
		bool SendFile(Char *pUser, Char *pPass, Char *pRemotePath, Char *pFileName);
	public:
		static CNet_Ftp_Handle*
				Instance();
	};
}