#pragma once 
#include "DataConvert.h"

namespace CrashRP
{
	class CConfigHandle
	{
		typedef std::map<UL32, Char_A*> KeyMap;
		typedef std::map<UL32, KeyMap*> GroupMap;
	protected:
		CConfigHandle();
		virtual ~CConfigHandle();
	public:
		virtual bool Load(Char *pCfgFile);
		virtual void Free();
	public:
		virtual bool ReadI32(Char *pGropuName, Char *pKeyName, I32 &Value);
		virtual bool WriteI32(Char *pGropuName, Char *pKeyName, I32 Value);

		virtual bool ReadUI32(Char *pGropuName, Char *pKeyName, UI32 &Value);
		virtual bool WriteUI32(Char *pGropuName, Char *pKeyName, UI32 Value);

		virtual bool ReadI64(Char *pGropuName, Char *pKeyName, I64 &Value);
		virtual bool WriteI64(Char *pGropuName, Char *pKeyName, I64 Value);

		virtual bool ReadUI64(Char *pGropuName, Char *pKeyName, UI64 &Value);
		virtual bool WriteUI64(Char *pGropuName, Char *pKeyName, UI64 Value);

		virtual bool ReadL32(Char *pGropuName, Char *pKeyName, L32 &Value);
		virtual bool WriteL32(Char *pGropuName, Char *pKeyName, L32 Value);

		virtual bool ReadUL32(Char *pGropuName, Char *pKeyName, UL32 &Value);
		virtual bool WriteUL32(Char *pGropuName, Char *pKeyName, UL32 Value);

		virtual bool ReadBool(Char *pGropuName, Char *pKeyName, bool &Value);
		virtual bool WriteBool(Char *pGropuName, Char *pKeyName, bool Value);

		virtual bool ReadReal32(Char *pGropuName, Char *pKeyName, Real32 &Value);
		virtual bool WriteReal32(Char *pGropuName, Char *pKeyName, Real32 Value);

		virtual bool ReadReal64(Char *pGropuName, Char *pKeyName, Real64 &Value);
		virtual bool WriteReal64(Char *pGropuName, Char *pKeyName, Real64 Value);

		virtual bool ReadStr(Char *pGropuName, Char *pKeyName, Char *pValue, UL32 uValueLen);
		virtual bool WriteStr(Char *pGropuName, Char *pKeyName, Char *pValue, UL32 uValueLen);
	public:
		inline bool IsLoaded() const
		{
			return mIsLoaded;
		}
	protected:
		bool	mIsLoaded;
		GroupMap
				mGroupMap;
	};
}