#pragma once
#include "ConfigHandle.h"

namespace CrashRP
{
	class CGlobalSet
		: public CConfigHandle
	{
	protected:
		CGlobalSet();
		virtual ~CGlobalSet();
	public:
		virtual bool Load(Char *pCfgFile);
	public:
		inline void
			Get_ServAddr(Char *pServAddr, UL32 uAddrLen)
		{
			_tcscpy_s(pServAddr, uAddrLen, mServAddr);
		}

		inline void
			Set_ServAddr(Char *pServAddr)
		{
			_tcscpy_s(mServAddr, 256, pServAddr);
		}

		inline void
			Get_ServPort(UL32 &uPort)
		{
			uPort = mServPort;
		}

		inline void
			Set_ServPort(UL32  uPort)
		{
			mServPort = uPort;
		}

		inline void
			Set_ServPath(Char *pServPath)
		{
			_tcscpy_s(mServPath, 256, pServPath);
		}

		inline void
			Get_ServPath(Char *pServPath, UL32 uPathLen)
		{
			_tcscpy_s(pServPath, uPathLen, mServPath);
		}

		inline void
			Get_CreateDmpFile(bool &bCreate)
		{
			bCreate = mCreateDmp;
		}

		inline void
			Set_CreateDmpFile(bool  bCreate)
		{
			mCreateDmp = bCreate;
		}

		inline void
			Get_DumpSys(bool &bDump)
		{
			bDump = mDumpSys;
		}

		inline void
			Set_DumpSys(bool  bDump)
		{
			mDumpSys = bDump;
		}

		inline void
			Get_DumpCpu(bool &bDump)
		{
			bDump = mDumpCpu;
		}

		inline void
			Set_DumpCpu(bool  bDump)
		{
			mDumpCpu = bDump;
		}

		inline void
			Get_DumpSta(bool &bDump)
		{
			bDump = mDumpSta;
		}

		inline void
			Set_DumpSta(bool  bDump)
		{
			mDumpSta = bDump;
		}

		inline void
			Get_DumpMod(bool &bDump)
		{
			bDump = mDumpMod;
		}

		inline void
			Set_DumpMod(bool  bDump)
		{
			mDumpMod = bDump;
		}

		inline void
			Get_NetworkIp(Char *pNetworkIp, UL32 uIpLen)
		{
			_tcscpy_s(pNetworkIp, uIpLen, mNetworkIp);
		}

		inline void
			Set_NetworkIp(Char *pNetworkIp)
		{
			_tcscpy_s(mNetworkIp, 256, pNetworkIp);
		}

		inline void
			Get_CheckFile(Char *pFileName, UL32 uIpLen)
		{
			_tcscpy_s(pFileName, uIpLen, mCheckFile);
		}

		inline void
			Set_CheckFile(Char *pFileName)
		{
			_tcscpy_s(mCheckFile, 256, pFileName);
		}
	public:
		static CGlobalSet* Instance();
	protected:
		Char mServAddr[256];
		Char mServPath[256];
		UL32 mServPort;

		bool mCreateDmp;
		bool mDumpSys;
		bool mDumpCpu;
		bool mDumpSta;
		bool mDumpMod;

		Char mNetworkIp[256];
		Char mCheckFile[256];
	};
}