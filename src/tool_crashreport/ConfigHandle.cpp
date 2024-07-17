#include "ConfigHandle.h"

namespace CrashRP
{
	CConfigHandle::CConfigHandle()
		: mIsLoaded(false)
	{
	}

	CConfigHandle::~CConfigHandle()
	{
		Free();
	}

	bool CConfigHandle::Load(Char *pCfgFile)
	{
		Free();

		FILE *fp = NULL; _tfopen_s(&fp, pCfgFile, __Text("rt"));
		if (fp)
		{
			KeyMap *pKey = NULL;

			do
			{
				Char_A szToken[256];
				fscanf_s(fp, "%s", szToken, 256);

				if (szToken[0] == '[')
				{
					pKey = new KeyMap();

					Char_A szGroupName[256];
					strncpy_s(szGroupName, 256, &szToken[1], strlen(szToken) - 2);

					mGroupMap.insert(GroupMap::value_type(CDataConvert::AsciiToHash(szGroupName) , pKey));
				} else if (szToken[0] == ';' || szToken[0] == ' ' || szToken[0] == '\\' || szToken[0] == '/')	// ×¢ÊÍ
				{
					do
					{
						Char_A ch = fgetc(fp);

						if (ch == '\n')
						{
							break;
						}
					} while (!feof(fp));
				} else
				{
					if (pKey)
					{
						Char_A szKey[256];
						strcpy_s(szKey, 256, szToken);

						fscanf_s(fp, "%s", szToken, 256);	// =
						fscanf_s(fp, "%s", szToken, 256);	// Value

						Char_A *pValue = new Char_A[256];
						strcpy_s(pValue, 256, szToken);

						pKey->insert(KeyMap::value_type(CDataConvert::AsciiToHash(szKey), pValue));
					}
				}
			} while (!feof(fp));

			fclose(fp);

			mIsLoaded = true;

			return true;
		}

		return false;
	}

	void CConfigHandle::Free()
	{
		if (mIsLoaded)
		{
			mIsLoaded = false;

			for (GroupMap::iterator itA = mGroupMap.begin(); itA != mGroupMap.end(); itA ++)
			{
				KeyMap *pKey = (*itA).second;
				if (pKey)
				{
					for (KeyMap::iterator itB = pKey->begin(); itB != pKey->end(); itB ++)
					{
						delete[] (*itB).second;
					}

					delete pKey;
				}
			}
			mGroupMap.clear();
		}
	}

	bool CConfigHandle::ReadI32(Char *pGropuName, Char *pKeyName, I32 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = _ttoi(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteI32(Char *pGropuName, Char *pKeyName, I32 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadUI32(Char *pGropuName, Char *pKeyName, UI32 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = (UI32)_ttoi(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteUI32(Char *pGropuName, Char *pKeyName, UI32 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadI64(Char *pGropuName, Char *pKeyName, I64 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = _ttoi64(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteI64(Char *pGropuName, Char *pKeyName, I64 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadUI64(Char *pGropuName, Char *pKeyName, UI64 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = (UI64)_ttoi64(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteUI64(Char *pGropuName, Char *pKeyName, UI64 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadL32(Char *pGropuName, Char *pKeyName, L32 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = _ttol(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteL32(Char *pGropuName, Char *pKeyName, L32 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadUL32(Char *pGropuName, Char *pKeyName, UL32 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = (UL32)_ttol(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteUL32(Char *pGropuName, Char *pKeyName, UL32 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadBool(Char *pGropuName, Char *pKeyName, bool &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			_tcsupr_s(szValue);

			if (_tcscmp(szValue, __Text("TRUE")) == 0)
			{
				Value = true;
			} else
			{
				Value = false;
			}

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteBool(Char *pGropuName, Char *pKeyName, bool Value)
	{
		return true;
	}

	bool CConfigHandle::ReadReal32(Char *pGropuName, Char *pKeyName, Real32 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = (Real32)_tstof(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteReal32(Char *pGropuName, Char *pKeyName, Real32 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadReal64(Char *pGropuName, Char *pKeyName, Real64 &Value)
	{
		Char szValue[256];
		if (ReadStr(pGropuName, pKeyName, szValue, 256))
		{
			Value = _tstof(szValue);

			return true;
		}

		return false;
	}

	bool CConfigHandle::WriteReal64(Char *pGropuName, Char *pKeyName, Real64 Value)
	{
		return true;
	}

	bool CConfigHandle::ReadStr(Char *pGropuName, Char *pKeyName, Char *pValue, UL32 uValueLen)
	{
		if (mIsLoaded)
		{
			Char_A szGroupName[256];
			CDataConvert::StringToAscii(pGropuName, szGroupName, 256);

			GroupMap::iterator itA = mGroupMap.find(CDataConvert::AsciiToHash(szGroupName));
			if (itA != mGroupMap.end())
			{
				KeyMap *pKey = (*itA).second;
				if (pKey)
				{
					Char_A szKeyName[256];
					CDataConvert::StringToAscii(pKeyName, szKeyName, 256);

					KeyMap::iterator itB = pKey->find(CDataConvert::AsciiToHash(szKeyName));
					if (itB != pKey->end())
					{
						CDataConvert::AsciiToString((*itB).second, pValue, uValueLen);

						return true;
					}
				}
			}
		}

		return false;
	}

	bool CConfigHandle::WriteStr(Char *pGropuName, Char *pKeyName, Char *pValue, UL32 uValueLen)
	{
		return true;
	}
}