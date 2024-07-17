#pragma once
#include <config.h>

#include <Tlhelp32.h>

namespace CrashRP
{
	typedef std::list<PMODULEENTRY32>			ModuleInfoList;
	typedef std::list<PMODULEENTRY32>::iterator	ModuleInfoIter;

	class CModuleList
	{
	private:
		CModuleList();
	public:
		virtual ~CModuleList();
	public:
		bool	Update();

		PMODULEENTRY32
				QueryModuleInfoByAddress(UI64 uAddr);

		ModuleInfoList*
				GetModuleList();
	public:
		static CModuleList*
				Instance();
	private:
		void	Clear();
	protected:
		ModuleInfoList mModuleList;
	};
}