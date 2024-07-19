#include "GlobalSet.h"

namespace CrashRP
{
	CGlobalSet::CGlobalSet()
		: mServPort(-1)

		, mCreateDmp(false)
		, mDumpSys(false)
		, mDumpCpu(false)
		, mDumpSta(false)
		, mDumpMod(false)
	{
		mServAddr[0]	= NULL;
		mNetworkIp[0]	= NULL;
		mCheckFile[0]	= NULL;
	}

	CGlobalSet::~CGlobalSet()
	{
	}

	bool CGlobalSet::Load(Char *pCfgFile)
	{
		if (!CConfigHandle::Load(pCfgFile))
		{
			return false;
		}

		bool bTmp = false; UL32 uTmp = 0;
		Char szTmp[256] = { 0 };

		ReadStr(__Text("ServInfo"), __Text("ServAddr"), szTmp, 256);
		Set_ServAddr(szTmp);

		ReadUL32(__Text("ServInfo"), __Text("ServPort"), uTmp);
		Set_ServPort(uTmp);

		ReadStr(__Text("ServInfo"), __Text("ServPath"), szTmp, 256);
		Set_ServPath(szTmp);

		ReadBool(__Text("DumpInfo"), __Text("CreateDmp"), bTmp);
		Set_CreateDmpFile(bTmp);

		ReadBool(__Text("DumpInfo"), __Text("DumpSys"), bTmp);
		Set_DumpSys(bTmp);

		ReadBool(__Text("DumpInfo"), __Text("DumpCpu"), bTmp);
		Set_DumpCpu(bTmp);

		ReadBool(__Text("DumpInfo"), __Text("DumpSta"), bTmp);
		Set_DumpSta(bTmp);

		ReadBool(__Text("DumpInfo"), __Text("DumpMod"), bTmp);
		Set_DumpMod(bTmp);

		ReadStr(__Text("SysInfo"), __Text("CheckFile"), szTmp, 256);
		Set_CheckFile(szTmp);

		return true;
	}

	CGlobalSet* CGlobalSet::Instance()
	{
		static CGlobalSet Inst;

		return &Inst;
	}
}