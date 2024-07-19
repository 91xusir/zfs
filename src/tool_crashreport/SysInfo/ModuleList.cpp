#include "ModuleList.h"

namespace CrashRP
{
	CModuleList::CModuleList()
	{
	}

	CModuleList::~CModuleList()
	{
	}

	bool CModuleList::Update()
	{
		Clear();

		HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

		MODULEENTRY32 me32;
		me32.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hModuleSnap, &me32)) 
		{
			do
			{
				PMODULEENTRY32 pInfo = new MODULEENTRY32;
				memcpy(pInfo, &me32, sizeof(MODULEENTRY32));

				mModuleList.push_back(pInfo);
			} while(Module32Next(hModuleSnap, &me32));
		}

		CloseHandle(hModuleSnap);

		return true;
	}

	PMODULEENTRY32 CModuleList::QueryModuleInfoByAddress(UI64 uAddr)
	{
		for (ModuleInfoIter it = mModuleList.begin(); it != mModuleList.end(); it ++)
		{
			UI64 uBaseAddr = (UI64)((*it)->modBaseAddr);
			UI64 uEndAddr  = (UI64)((*it)->modBaseSize) + uBaseAddr;
			if (uAddr >= uBaseAddr && uAddr < uEndAddr)
			{
				return (*it);
			}
		}

		return NULL;
	}

	ModuleInfoList* CModuleList::GetModuleList()
	{
		return &mModuleList;
	}

	void CModuleList::Clear()
	{
		for (ModuleInfoIter it = mModuleList.begin(); it != mModuleList.end(); it ++)
		{
			delete (*it);
		}
		mModuleList.clear();
	}

	CModuleList* CModuleList::Instance()
	{
		static CModuleList Inst;

		return &Inst;
	}
}