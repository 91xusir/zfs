#include "Net_Ftp_Handle.h"
#include "GlobalSet.h"

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

namespace CrashRP
{
	CNet_Ftp_Handle::CNet_Ftp_Handle()
	{
	}

	CNet_Ftp_Handle::~CNet_Ftp_Handle()
	{
	}

	bool CNet_Ftp_Handle::SendFile(Char *pUser, Char *pPass, Char *pRemotePath, Char *pFileName)
	{
		bool bResult = false;

		Char szServAddr[256];
		CGlobalSet::Instance()->Get_ServAddr(szServAddr, 256);

		UL32 uServPort;
		CGlobalSet::Instance()->Get_ServPort(uServPort);

		HINTERNET hSession = InternetOpen(_T("CrashReport"), 0, NULL, NULL, 0);
		if (hSession)
		{
			HINTERNET hConnection = InternetConnect(hSession, szServAddr, (INTERNET_PORT)uServPort, pUser, pPass, INTERNET_SERVICE_FTP, 0, 0);
			if (hConnection)
			{
				if (FtpSetCurrentDirectory(hConnection, pRemotePath))
				{
					if (FtpPutFile(hConnection, pFileName, pFileName, FTP_TRANSFER_TYPE_BINARY, 0))
					{
						bResult = true;
					}
				}

				InternetCloseHandle(hConnection);
			}
			InternetCloseHandle(hSession);
		}

		return bResult;
	}

	CNet_Ftp_Handle* CNet_Ftp_Handle::Instance()
	{
		static CNet_Ftp_Handle Inst;

		return &Inst;
	}
}