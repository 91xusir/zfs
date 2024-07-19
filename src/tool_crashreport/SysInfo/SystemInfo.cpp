#include "SystemInfo.h"
#include "FileInfo.h"

#include "../GlobalSet.h"

#include "d3dx9.h"
#pragma comment(lib, "d3d9")

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

namespace CrashRP
{
	//////////////////////////////////////////////////////////////////////////
	// 
	#define VENDOR_STRING_LENGTH		(12 + 1)
	#define CHIPNAME_STRING_LENGTH		(48 + 1)
	#define SERIALNUMBER_STRING_LENGTH	(29 + 1)
	#define CLASSICAL_CPU_FREQ_LOOP		10000000
	#define RDTSC_INSTRUCTION			_asm _emit 0x0f _asm _emit 0x31
	#define	CPUSPEED_I32TO64(x, y)		(((I64) x << 32) + y)


	//////////////////////////////////////////////////////////////////////////
	// 
	class CSys_CpuSpeed
	{
		typedef	void (*DELAY_FUNC)(UI32 uMS);
	public:
		CSys_CpuSpeed()
		{
			I64 Total		= GetCyclesDifference(Delay, 50);
			I64 Overhead	= GetCyclesDifference(DelayOverhead, 50);

			Total -= Overhead;
			Total /= 50;
			Total /= 1000;

			mCPUSpeedInGHz = (Real32)Total * 0.001f;
		}
	public:
		inline Real32 GetSpeedInGHz()
		{
			return mCPUSpeedInGHz;
		}
	protected:
		I64 GetCyclesDifference(DELAY_FUNC DelayFunction, UI32 uParam)
		{
			UI32 edx1, eax1;
			UI32 edx2, eax2;

			__try
			{
				_asm
				{
					push uParam
					mov ebx, DelayFunction

					RDTSC_INSTRUCTION

					mov esi, eax			; esi = eax
					mov edi, edx			; edi = edx

					call ebx				; µ÷ÓÃ DelayFunction

					RDTSC_INSTRUCTION

					pop ebx

					mov edx2, edx			; edx2 = edx
					mov eax2, eax			; eax2 = eax

					mov edx1, edi			; edx2 = edi
					mov eax1, esi			; eax2 = esi
				}
			} __except (1)
			{
				return -1;
			}

			return (CPUSPEED_I32TO64(edx2, eax2) - CPUSPEED_I32TO64(edx1, eax1));
		}
	protected:
		static void Delay(UI32 uMS)
		{
			LARGE_INTEGER Frequency;
			if (QueryPerformanceFrequency(&Frequency))
			{
				I64 x = Frequency.QuadPart / 1000 * uMS;

				LARGE_INTEGER StartCounter, EndCounter;
				QueryPerformanceCounter(&StartCounter);

				do
				{
					QueryPerformanceCounter(&EndCounter);
				} while (EndCounter.QuadPart - StartCounter.QuadPart < x);
			}
		}

		static void DelayOverhead(UI32 uMS)
		{
			LARGE_INTEGER Frequency;
			if (QueryPerformanceFrequency(&Frequency))
			{
				I64 x = Frequency.QuadPart / 1000 * uMS;

				LARGE_INTEGER StartCounter, EndCounter;
				QueryPerformanceCounter(&StartCounter);

				do
				{
					QueryPerformanceCounter(&EndCounter);
				} while (EndCounter.QuadPart - StartCounter.QuadPart == x);
			}
		}
	protected:
		Real32 mCPUSpeedInGHz;
	};


	//////////////////////////////////////////////////////////////////////////
	// 
	class CSys_CpuInfo
	{
		enum CpuType
		{
			CpuT_AMD, CpuT_Intel, CpuT_NSC, CpuT_UMC, CpuT_Cyrix, CpuT_NexGen, CpuT_IDT, CpuT_Rise, CpuT_Transmeta, CpuT_Unknown
		};

		typedef struct tagCpuId
		{
			I32		_Type;
			I32		_Family;
			I32		_Model;
			I32		_Revision;
			I32		_ExtendedFamily;
			I32		_ExtendedModel;
			Char_A	_ProcessorName[CHIPNAME_STRING_LENGTH];
			Char_A	_Vendor[VENDOR_STRING_LENGTH];
			Char_A	_SerialNumber[SERIALNUMBER_STRING_LENGTH];
		} CpuId;
	public:
		CSys_CpuInfo()
			: mCpuType(CpuT_Unknown)
		{
			memset(&mCpuId, 0, sizeof(mCpuId));
			if (IsSupportCPUID())
			{
				RetrieveIdentity();
				RetrieveExtendedIdentity();
			}
		}
	public:
		bool IsSupportCPUID()
		{
			__try
			{
				_asm
				{
					push eax
					push ebx
					push ecx
					push edx

					mov eax, 0
					cpuid

					pop edx
					pop ecx
					pop ebx
					pop eax
				}
			} __except (1)
			{
				return false;
			}

			return true;
		}
	public:
		inline Char_A* GetProcessorName()
		{
			return mCpuId._ProcessorName;
		}
	protected:
		bool RetrieveIdentity()
		{
			I32 CPUVendor[3], CPUSignature;

			__try
			{
				_asm
				{
					push eax
					push ebx
					push ecx
					push edx

					mov eax, 0
					cpuid
					mov CPUVendor[0 * TYPE I32], ebx
					mov CPUVendor[1 * TYPE I32], edx
					mov CPUVendor[2 * TYPE I32], ecx

					mov eax,1
					cpuid
					mov CPUSignature, eax

					pop edx
					pop ecx
					pop ebx
					pop eax
				}
			} __except (1)
			{
				return false;
			}

			memcpy(mCpuId._Vendor,       &(CPUVendor[0]), sizeof(I32));
			memcpy(&(mCpuId._Vendor[4]), &(CPUVendor[1]), sizeof(I32));
			memcpy(&(mCpuId._Vendor[8]), &(CPUVendor[2]), sizeof(I32));
			mCpuId._Vendor[12] = '\0';

			if (strcmp(mCpuId._Vendor, "GenuineIntel") == 0)		mCpuType = CpuT_Intel;
			else if (strcmp (mCpuId._Vendor, "UMC UMC UMC ") == 0)	mCpuType = CpuT_UMC;
			else if (strcmp (mCpuId._Vendor, "AuthenticAMD") == 0)	mCpuType = CpuT_AMD;
			else if (strcmp (mCpuId._Vendor, "AMD ISBETTER") == 0)	mCpuType = CpuT_AMD;
			else if (strcmp (mCpuId._Vendor, "CyrixInstead") == 0)	mCpuType = CpuT_Cyrix;
			else if (strcmp (mCpuId._Vendor, "NexGenDriven") == 0)	mCpuType = CpuT_NexGen;
			else if (strcmp (mCpuId._Vendor, "CentaurHauls") == 0)	mCpuType = CpuT_IDT;
			else if (strcmp (mCpuId._Vendor, "RiseRiseRise") == 0)	mCpuType = CpuT_Rise;
			else if (strcmp (mCpuId._Vendor, "GenuineTMx86") == 0)	mCpuType = CpuT_Transmeta;
			else if (strcmp (mCpuId._Vendor, "TransmetaCPU") == 0)	mCpuType = CpuT_Transmeta;
			else if (strcmp (mCpuId._Vendor, "Geode By NSC") == 0)	mCpuType = CpuT_NSC;
			else													mCpuType = CpuT_Unknown;

			mCpuId._ExtendedFamily	= ((CPUSignature & 0x0FF00000) >> 20);
			mCpuId._ExtendedModel	= ((CPUSignature & 0x000F0000) >> 16);
			mCpuId._Type			= ((CPUSignature & 0x0000F000) >> 12);
			mCpuId._Family			= ((CPUSignature & 0x00000F00) >>  8);
			mCpuId._Model			= ((CPUSignature & 0x000000F0) >>  4);
			mCpuId._Revision		= ((CPUSignature & 0x0000000F) >>  0);

			return true;
		}

		bool RetrieveExtendedIdentity()
		{
			I32 ExtendedIdentity[12];

			if (!CheckExtendedLevelSupport(0X80000002)) return false;
			if (!CheckExtendedLevelSupport(0X80000003)) return false;
			if (!CheckExtendedLevelSupport(0X80000004)) return false;

			__try
			{
				_asm
				{
					push eax
					push ebx
					push ecx
					push edx

					mov eax,0x80000002
					cpuid
					mov ExtendedIdentity[0  * TYPE I32], eax
					mov ExtendedIdentity[1  * TYPE I32], ebx
					mov ExtendedIdentity[2  * TYPE I32], ecx
					mov ExtendedIdentity[3  * TYPE I32], edx

					mov eax,0x80000003
					cpuid
					mov ExtendedIdentity[4  * TYPE I32], eax
					mov ExtendedIdentity[5  * TYPE I32], ebx
					mov ExtendedIdentity[6  * TYPE I32], ecx
					mov ExtendedIdentity[7  * TYPE I32], edx

					mov eax,0x80000004
					cpuid
					mov ExtendedIdentity[8  * TYPE I32], eax
					mov ExtendedIdentity[9  * TYPE I32], ebx
					mov ExtendedIdentity[10 * TYPE I32], ecx
					mov ExtendedIdentity[11 * TYPE I32], edx

					pop edx
					pop ecx
					pop ebx
					pop eax
				}
			} __except (1)
			{
				return false;
			}

			memcpy (mCpuId._ProcessorName,        &(ExtendedIdentity[ 0]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[ 4]), &(ExtendedIdentity[ 1]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[ 8]), &(ExtendedIdentity[ 2]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[12]), &(ExtendedIdentity[ 3]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[16]), &(ExtendedIdentity[ 4]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[20]), &(ExtendedIdentity[ 5]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[24]), &(ExtendedIdentity[ 6]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[28]), &(ExtendedIdentity[ 7]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[32]), &(ExtendedIdentity[ 8]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[36]), &(ExtendedIdentity[ 9]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[40]), &(ExtendedIdentity[10]), sizeof(I32));
			memcpy (&(mCpuId._ProcessorName[44]), &(ExtendedIdentity[11]), sizeof(I32));
			mCpuId._ProcessorName[48] = '\0';

			if (mCpuType == CpuT_Intel)
			{
				I32 ProcessorNameStartPos = 0;

				for (I32 nCounter = 0; nCounter < CHIPNAME_STRING_LENGTH; nCounter ++)
				{
					if ((mCpuId._ProcessorName[nCounter] != '\0') && (mCpuId._ProcessorName[nCounter] != ' '))
					{
						ProcessorNameStartPos = nCounter;

						break;
					}
				}

				if (ProcessorNameStartPos == 0)
				{
					return true;
				}

				memmove(mCpuId._ProcessorName, &(mCpuId._ProcessorName[ProcessorNameStartPos]), (CHIPNAME_STRING_LENGTH - ProcessorNameStartPos));
			}

			return true;
		}

		bool CheckExtendedLevelSupport(I32 CPULevelToCheck)
		{
			// The extended CPUID is supported by various vendors starting with the following CPU models: 
			//
			//		Manufacturer & Chip Name			|		Family		 Model		Revision
			//
			//		AMD K6, K6-2						|		   5		   6			x		
			//		Cyrix GXm, Cyrix III "Joshua"		|		   5		   4			x
			//		IDT C6-2							|		   5		   8			x
			//		VIA Cyrix III						|		   6		   5			x
			//		Transmeta Crusoe					|		   5		   x			x
			//		Intel Pentium 4						|		   f		   x			x
			//

			if (mCpuType == CpuT_AMD)
			{
				if (mCpuId._Family < 5)								return false;
				if ((mCpuId._Family == 5) && (mCpuId._Model < 6))	return false;
			} else if (mCpuType == CpuT_Cyrix)
			{
				if (mCpuId._Family < 5)								return false;
				if ((mCpuId._Family == 5) && (mCpuId._Model < 4))	return false;
				if ((mCpuId._Family == 6) && (mCpuId._Model < 5))	return false;
			} else if (mCpuType == CpuT_IDT)
			{
				if (mCpuId._Family < 5)								return false;
				if ((mCpuId._Family == 5) && (mCpuId._Model < 8))	return false;
			} else if (mCpuType == CpuT_Transmeta)
			{
				if (mCpuId._Family < 5)								return false;
			} else if (mCpuType == CpuT_Intel)
			{
				if (mCpuId._Family < 0XF)							return false;
			}

			I32 MaxCPUExtendedLevel = 0;
			__try
			{
				_asm
				{
					push eax
					push ebx
					push ecx
					push edx
					mov eax,0x80000000
					cpuid
					mov MaxCPUExtendedLevel, eax

					pop edx
					pop ecx
					pop ebx
					pop eax
				}
			} __except (1)
			{
				return false;
			}

			I32 nLevelWanted = (CPULevelToCheck & 0X7FFFFFFF);
			I32 nLevelReturn = (MaxCPUExtendedLevel & 0X7FFFFFFF);

			if (nLevelWanted > nLevelReturn)
			{
				return false;
			}

			return true;
		}
	protected:
		CpuId	mCpuId;
		CpuType	mCpuType;

		PEXCEPTION_POINTERS
				mpExceptionInfo;
	};


	//////////////////////////////////////////////////////////////////////////
	// 
	class CSys_VideoInfo
	{
	public:
		CSys_VideoInfo()
			: mpD3D(NULL)
			, mD3DLibrary(NULL)
		{
			typedef IDirect3D9* (__stdcall *Direct3DCreate9Fn)(UINT);

			mD3DLibrary = LoadLibrary(__Text("d3d9.dll"));
			if (mD3DLibrary)
			{
				Direct3DCreate9Fn pDirect3DCreate9 = (Direct3DCreate9Fn)GetProcAddress(mD3DLibrary, "Direct3DCreate9");
				if (pDirect3DCreate9)
				{
					mpD3D = pDirect3DCreate9(D3D_SDK_VERSION);
				}
			}
		}

		virtual ~CSys_VideoInfo()
		{
			if (mD3DLibrary)
			{
				FreeLibrary(mD3DLibrary);
				mD3DLibrary = NULL;
			}
		}
	public:
		UI32 GetAdapterCount()
		{
			if (mpD3D)
			{
				return mpD3D->GetAdapterCount();
			}

			return 0;
		}

		bool GetVideoInfo(UI32 uAdapter, Char *pText, UI32 uTextLen)
		{
			if (mpD3D)
			{
				D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
				if (SUCCEEDED(mpD3D->GetAdapterIdentifier(uAdapter, 0, &AdapterIdentifier)))
				{
					CDataConvert::AsciiToString(AdapterIdentifier.Description, pText, uTextLen);

					return true;
				}
			}

			return false;
		}
	protected:
		IDirect3D9
				*mpD3D;
		HMODULE	mD3DLibrary;
	};


	//////////////////////////////////////////////////////////////////////////
	// 
	class CSys_MemoryInfo
	{
	public:
		CSys_MemoryInfo()
		{
			mLibrary = LoadLibrary(__Text("kernel32.dll"));
		}

		virtual ~CSys_MemoryInfo()
		{
			if (mLibrary)
			{
				FreeLibrary(mLibrary);
				mLibrary = NULL;
			}
		}
	public:
		bool GetMemoryInfo(Char *pText, UI32 uTextLen)
		{
			typedef BOOL (__stdcall * GlobalMemoryStatusExFn)(LPMEMORYSTATUSEX);

			if (mLibrary)
			{
				GlobalMemoryStatusExFn pGlobalMemoryStatusEx;
				pGlobalMemoryStatusEx = (GlobalMemoryStatusExFn)GetProcAddress(mLibrary, "GlobalMemoryStatusEx");

				if (pGlobalMemoryStatusEx)
				{
					MEMORYSTATUSEX Meminfo;
					memset(&Meminfo, 0, sizeof(Meminfo));
					Meminfo.dwLength = sizeof(Meminfo);

					pGlobalMemoryStatusEx(&Meminfo);

					Real64 MemSize = (double)Meminfo.ullTotalPhys;
					MemSize /= 1024.0;
					MemSize /= 1024.0;
					MemSize += 0.5;

					Real64 AMemSize = (double)Meminfo.ullAvailPhys;
					AMemSize /= 1024.0;
					AMemSize /= 1024.0;
					AMemSize += 0.5;

					_stprintf_s(pText, uTextLen, __Text("Total PhysMemory[%.0f(MB)], Valid PhysMemory[%.0f(MB)]"), MemSize, AMemSize);

					return true;
				}
			}

			return false;
		}
	protected:
		HMODULE	mLibrary;
	};


	//////////////////////////////////////////////////////////////////////////
	// 
	class CSys_NetInfo
	{
	public:
		bool GetNetworkIp(Char *pNetworkIp, UL32 uIpLen)
		{
			bool bResult = false;

			HINTERNET hSession = InternetOpen(_T("CrashReport"), 0, NULL, NULL, 0);
			if (hSession)
			{
				HINTERNET hConnection = InternetConnect(hSession, _T("www.ip.cn"), 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
				if (hConnection)
				{
					HINTERNET hFile = HttpOpenRequest(hConnection, _T("GET"), _T("getip.php?action=getip"), _T("HTTP/1.0"), NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT, 0);
					if (hFile)
					{
						Char szRequest[] = _T("Content-Type: application/x-www-form-urlencoded");
						if (HttpSendRequest(hFile, szRequest, (UL32)_tcslen(szRequest), NULL, 0))
						{
							Char_A ReadBuff[512]; UL32 uRead = 0;
							if (InternetReadFile(hFile, ReadBuff, 512, &uRead))
							{
								Char szTmp[512];
								CDataConvert::AsciiToString(ReadBuff, szTmp, 512);

								bool bFoundCh = false;

								UL32 uStartPos = 0;
								for (UL32 i = 0; i < uRead; i ++)
								{
									if (szTmp[i] == __Text('>'))
									{
										bFoundCh = true;

										uStartPos = i + 1;
									} else if (szTmp[i] == __Text('<') && bFoundCh)
									{
										_tcsncpy_s(pNetworkIp, uIpLen, &szTmp[uStartPos], i - uStartPos);
									}
								}
								bResult = true;
							}
						}

						InternetCloseHandle(hFile);
					}
					InternetCloseHandle(hConnection);
				}
				InternetCloseHandle(hSession);
			}

			return bResult;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// 
	CSystemInfo::CSystemInfo()
	{
		strcpy_s(mInfoType, "[System Info]");
	}

	CSystemInfo::~CSystemInfo()
	{
	}

	bool CSystemInfo::DumpInfo(HANDLE hDumpFile)
	{
		Char szText[256]; Char_A szTextA[512]; Char_A szTemp[512];
		UL32 uLen = 0;

		//////////////////////////////////////////////////////////////////////////
		// system info
		if (GetOSVersionStr(szText, 256))
		{
			CDataConvert::StringToAscii(szText, szTextA, 512);

			sprintf_s(szTemp, "SYSTEM: %s\r\n", szTextA);
			uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		//////////////////////////////////////////////////////////////////////////
		// cpu info
		CSys_CpuInfo CpuInfo;
		if (CpuInfo.IsSupportCPUID())
		{
			CSys_CpuSpeed Speed;

			sprintf_s(szTemp, "CPU:    %s[%G(GHz)]\r\n", CpuInfo.GetProcessorName(), Speed.GetSpeedInGHz());
			uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		//////////////////////////////////////////////////////////////////////////
		// video info
		CSys_VideoInfo VideoInfo;
		UI32 uAdapterCount = VideoInfo.GetAdapterCount();

		for (UI32 i = 0; i < uAdapterCount; i ++)
		{
			if (VideoInfo.GetVideoInfo(i, szText, 256))
			{
				CDataConvert::StringToAscii(szText, szTextA, 512);

				sprintf_s(szTemp, "Video:  Adapter(%d)-%s\r\n", i, szTextA);
				uLen = (UL32)strlen(szTemp);

				WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// memory info
		CSys_MemoryInfo MemoryInfo;
		if (MemoryInfo.GetMemoryInfo(szText, 256))
		{
			CDataConvert::StringToAscii(szText, szTextA, 512);

			sprintf_s(szTemp, "Memory: %s\r\n", szTextA);
			uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		//////////////////////////////////////////////////////////////////////////
		// network info
		CSys_NetInfo NetInfo;
		if (NetInfo.GetNetworkIp(szText, 256))
		{
			CGlobalSet::Instance()->Set_NetworkIp(szText);

			CDataConvert::StringToAscii(szText, szTextA, 512);

			sprintf_s(szTemp, "Net IP: %s\r\n", szTextA);
			uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		//////////////////////////////////////////////////////////////////////////
		// Client Version
		CGlobalSet::Instance()->Get_CheckFile(szText, 256);

		CFileInfo FInfo(szText);
		if (FInfo.GetFileVersion(szText, 256))
		{
			CDataConvert::StringToAscii(szText, szTextA, 512);

			sprintf_s(szTemp, "Client: %s\r\n", szTextA);
			uLen = (UL32)strlen(szTemp);

			WriteFile(hDumpFile, szTemp, uLen, &uLen, NULL);
		}

		uLen = (UL32)strlen(mLine);
		WriteFile(hDumpFile, mLine, uLen, &uLen, NULL);

		return true;
	}

	bool CSystemInfo::GetOSVersionStr(Char *pOSVer, UL32 uLen)
	{
		typedef void (WINAPI *GetSysInfoFn)(LPSYSTEM_INFO);

		OSVERSIONINFOEX OSVerInfo;
		SYSTEM_INFO		SysInfo;
		GetSysInfoFn	pGetSystemInfo;
		BOOL			bOsVersionInfoEx;

		Char szOSVer[256] = { 0 };

		Char szTemp[128] = { 0 };

		memset(&SysInfo, 0, sizeof(SYSTEM_INFO));
		memset(&OSVerInfo, 0, sizeof(OSVERSIONINFOEX));

		OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&OSVerInfo)))
		{
			OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if (!GetVersionEx((OSVERSIONINFO*)&OSVerInfo)) 
			{
				return false;
			}
		}

		pGetSystemInfo = (GetSysInfoFn)GetProcAddress(GetModuleHandle(__Text("kernel32.dll")), "GetNativeSystemInfo");
		if (pGetSystemInfo)
		{
			pGetSystemInfo(&SysInfo);
		} else 
		{
			GetSystemInfo(&SysInfo);
		}

		switch (OSVerInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			{
				if (OSVerInfo.dwMajorVersion == 6 && OSVerInfo.dwMinorVersion == 0)
				{
					if (OSVerInfo.wProductType == VER_NT_WORKSTATION)
					{
						_tcscpy_s(szOSVer, __Text("Windows Vista "));
					} else 
					{
						_tcscpy_s(szOSVer, __Text("Windows Server \"Longhorn\" "));
					}
				} else if (OSVerInfo.dwMajorVersion == 5 && OSVerInfo.dwMinorVersion == 2)
				{
					if (GetSystemMetrics(SM_SERVERR2))
					{
						_tcscpy_s(szOSVer, __Text("Microsoft Windows Server 2003 \"R2\" "));
					} else if (OSVerInfo.wProductType == VER_NT_WORKSTATION && SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					{
						_tcscpy_s(szOSVer, __Text("Microsoft Windows XP Professional x64 Edition "));
					} else 
					{
						_tcscpy_s(szOSVer, __Text("Microsoft Windows Server 2003 "));
					}
				} else if (OSVerInfo.dwMajorVersion == 5 && OSVerInfo.dwMinorVersion == 1)
				{
					_tcscpy_s(szOSVer, __Text("Microsoft Windows XP "));
				} else if (OSVerInfo.dwMajorVersion == 5 && OSVerInfo.dwMinorVersion == 0)
				{
					_tcscpy_s(szOSVer, __Text("Microsoft Windows 2000 "));
				} else if (OSVerInfo.dwMajorVersion <= 4)
				{
					_tcscpy_s(szOSVer, __Text("Microsoft Windows NT "));
				} else
				{
					_tcscpy_s(szOSVer, __Text("Unknown System Version")); break;
				}

				if (bOsVersionInfoEx)
				{
					if (OSVerInfo.wProductType == VER_NT_WORKSTATION && SysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64)
					{
						if (OSVerInfo.dwMajorVersion == 4)
						{
							_tcscat_s(szOSVer, __Text("Workstation 4.0 "));
						} else if (OSVerInfo.wSuiteMask & VER_SUITE_PERSONAL)
						{
							_tcscat_s(szOSVer, __Text("Home Edition "));
						} else 
						{
							_tcscat_s(szOSVer, __Text("Professional "));
						}
					} else if (OSVerInfo.wProductType == VER_NT_SERVER || OSVerInfo.wProductType == VER_NT_DOMAIN_CONTROLLER)
					{
						if (OSVerInfo.dwMajorVersion == 5 && OSVerInfo.dwMinorVersion == 2)
						{
							if (SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
							{
								if (OSVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
								{
									_tcscat_s(szOSVer, __Text("Datacenter Edition for Itanium-based Systems"));
								} else if (OSVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
								{
									_tcscat_s(szOSVer, __Text("Enterprise Edition for Itanium-based Systems"));
								}
							} else if (SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
							{
								if (OSVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
								{
									_tcscat_s(szOSVer, __Text("Datacenter x64 Edition "));
								} else if (OSVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
								{
									_tcscat_s(szOSVer, __Text("Enterprise x64 Edition "));
								} else 
								{
									_tcscat_s(szOSVer, __Text("Standard x64 Edition "));
								}
							} else 
							{
								if (OSVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
								{
									_tcscat_s(szOSVer, __Text("Datacenter Edition "));
								} else if (OSVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
								{
									_tcscat_s(szOSVer, __Text("Enterprise Edition "));
								} else if (OSVerInfo.wSuiteMask & VER_SUITE_BLADE)
								{
									_tcscat_s(szOSVer, __Text("Web Edition "));
								} else 
								{
									_tcscat_s(szOSVer, __Text("Standard Edition "));
								}
							}
						} else if (OSVerInfo.dwMajorVersion == 5 && OSVerInfo.dwMinorVersion == 0)
						{
							if (OSVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
							{
								_tcscat_s(szOSVer, __Text("Datacenter Server "));
							} else if (OSVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
							{
								_tcscat_s(szOSVer, __Text("Advanced Server "));
							} else
							{
								_tcscat_s(szOSVer, __Text("Server "));
							}
						} else
						{
							if (OSVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
							{
								_tcscat_s(szOSVer, __Text("Server 4.0, Enterprise Edition "));
							} else
							{
								_tcscat_s(szOSVer, __Text("Server 4.0 "));
							}
						}
					}
				} else
				{
					HKEY hKey = NULL;
					if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, __Text("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"), 0, KEY_QUERY_VALUE, &hKey))
					{
						break;
					}

					Char szProductType[128]; UL32 uBufLen = sizeof(szProductType);
					if (ERROR_SUCCESS == RegQueryValueEx(hKey, __Text("ProductType"), NULL, NULL, (Char_U*)szProductType, &uBufLen) || uBufLen > sizeof(szProductType))
					{
						RegCloseKey(hKey);

						break ;
					}

					RegCloseKey(hKey);

					if (_tcsicmp(__Text("WINNT"), szProductType) == 0)
					{
						_tcscat_s(szOSVer, __Text("Workstation "));
					} else if (_tcsicmp(__Text("LANMANNT"), szProductType) == 0)
					{
						_tcscat_s(szOSVer, __Text("Server "));
					} else if (_tcsicmp(__Text("SERVERNT"), szProductType) == 0)
					{
						_tcscat_s(szOSVer, __Text("Advanced Server "));
					}

					_stprintf_s(szTemp, __Text("%d.%d "), OSVerInfo.dwMajorVersion, OSVerInfo.dwMinorVersion);
					_tcscat_s(szOSVer, szTemp);
				}

				if (OSVerInfo.dwMajorVersion == 4 && 0 == _tcsicmp(OSVerInfo.szCSDVersion,__Text("Service Pack 6")))
				{
					HKEY hKey = NULL;
					L32 Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, __Text("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"), 0, KEY_QUERY_VALUE, &hKey);
					if (Result == ERROR_SUCCESS)
					{
						_stprintf_s(szTemp, __Text("Service Pack 6a (Build %d.%d.%d)"), 
							OSVerInfo.dwMajorVersion, OSVerInfo.dwMinorVersion, OSVerInfo.dwBuildNumber & 0XFFFF);

						_tcscat_s(szOSVer, szTemp);
					} else 
					{
						_stprintf_s(szTemp, __Text("%s (Build %d.%d.%d)"), 
							OSVerInfo.szCSDVersion, OSVerInfo.dwMajorVersion, OSVerInfo.dwMinorVersion, OSVerInfo.dwBuildNumber & 0XFFFF);

						_tcscat_s(szOSVer, szTemp);
					}

					RegCloseKey(hKey);
				} else 
				{
					_stprintf_s(szTemp, __Text("%s (Build %d.%d.%d)"), 
						OSVerInfo.szCSDVersion, OSVerInfo.dwMajorVersion, OSVerInfo.dwMinorVersion, OSVerInfo.dwBuildNumber & 0XFFFF);

					_tcscat_s(szOSVer, szTemp);
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			{
				if (OSVerInfo.dwMajorVersion == 4)
				{
					if (OSVerInfo.dwMinorVersion == 0)
					{
						if (OSVerInfo.szCSDVersion[1] == __Text('C') || OSVerInfo.szCSDVersion[1] == __Text('B'))
						{
							_tcscpy_s(szOSVer, __Text("Microsoft Windows 95 OSR2"));
						} else
						{
							_tcscpy_s(szOSVer, __Text("Microsoft Windows 95"));
						}
					} else if (OSVerInfo.dwMinorVersion == 10)
					{
						if (OSVerInfo.szCSDVersion[1] == __Text('A') || OSVerInfo.szCSDVersion[1] == __Text('B'))
						{
							_tcscpy_s(szOSVer, __Text("Microsoft Windows 98 SE"));
						} else
						{
							_tcscpy_s(szOSVer, __Text("Microsoft Windows 98"));
						}
					} else if (OSVerInfo.dwMinorVersion == 90)
					{
						_tcscpy_s(szOSVer, __Text("Microsoft Windows ME"));
					} else
					{
						_tcscpy_s(szOSVer, __Text("Unknown System Version"));
					}
				} else
				{
					_tcscpy_s(szOSVer, __Text("Unknown System Version"));
				}
			}
			break;
		case VER_PLATFORM_WIN32s:
			{
				_tcscpy_s(szOSVer, __Text("Microsoft Win32s"));
			}
			break;
		default:
			{
				_tcscpy_s(szOSVer, __Text("Unknown System Version"));
			}
		}

		if (uLen <= _tcslen(szOSVer))
		{
			return false;
		}

		_tcscpy_s(pOSVer, uLen, szOSVer);

		return true;
	}
}