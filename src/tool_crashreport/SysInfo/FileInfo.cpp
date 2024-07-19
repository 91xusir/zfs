#include "FileInfo.h"

#pragma comment(lib, "Version")

namespace CrashRP
{
	CFileInfo::CFileInfo(Char *pFileName)
		: mpDataBuff(NULL)
	{
		if (pFileName)
		{
			_tcscpy_s(mFileName, pFileName);
		} else
		{
			mFileName[0] = 0;
		}

		mbInit = Init();
	}

	CFileInfo::~CFileInfo()
	{
		if (mpDataBuff)
		{
			delete[] mpDataBuff; mpDataBuff = NULL;
		}
	}

	bool CFileInfo::Init()
	{
		UL32 uVersionLen = GetFileVersionInfoSize(mFileName, NULL);
		if (uVersionLen <= 0)   
		{
			return false;
		}

		mpDataBuff = new Char_U[uVersionLen];
		if (!mpDataBuff)
		{
			return false;
		}

		if (!GetFileVersionInfo(mFileName, NULL, uVersionLen, mpDataBuff))
		{
			delete[] mpDataBuff; mpDataBuff = NULL;

			return false;
		}  

		UI32 uTranslateBytes; UL32 *pDataBuff = NULL;
		if (!VerQueryValue(mpDataBuff, _T("\\VarFileInfo\\Translation"), (void**)&pDataBuff, &uTranslateBytes) || !pDataBuff)
		{
			delete[] mpDataBuff; mpDataBuff = NULL;

			return false;
		}

		mLanguage = (US32)(((*pDataBuff) >>  0) & 0xFFFF);
		mCodePage = (US32)(((*pDataBuff) >> 16) & 0xFFFF);

		return true;
	}

	bool CFileInfo::GetData(Char *pName, UL32 uLen, Char *pStr)
	{
		if (!mbInit)
		{
			return false;
		}

		Char szSubBlock[256];
		_stprintf_s(szSubBlock, _T("\\StringFileInfo\\%04x%04x\\%s"), mLanguage, mCodePage, pStr);

		Char *pBuff = NULL; UI32 uBuffLen = 0;
		if (!VerQueryValue(mpDataBuff, szSubBlock, (void**)&pBuff, &uBuffLen) || !pBuff)
		{
			return false;
		}

		if (uLen <= _tcslen(pBuff))
		{
			return false;
		}

		_tcscpy_s(pName, uLen, pBuff);

		return true;
	}

	bool CFileInfo::GetComments(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("Comments"));
	}

	bool CFileInfo::GetInternalName(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("InternalName"));
	}

	bool CFileInfo::GetProductName(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("ProductName"));
	}

	bool CFileInfo::GetCompanyName(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("CompanyName"));
	}

	bool CFileInfo::GetLegalCopyright(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("LegalCopyright"));
	}

	bool CFileInfo::GetProductVersion(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("ProductVersion"));
	}

	bool CFileInfo::GetFileDescription(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("FileDescription"));
	}

	bool CFileInfo::GetLegalTrademarks(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("LegalTrademarks"));
	}

	bool CFileInfo::GetPrivateBuild(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("PrivateBuild"));
	}

	bool CFileInfo::GetFileVersion(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("FileVersion"));
	}

	bool CFileInfo::GetOriginalFilename(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("OriginalFilename"));
	}

	bool CFileInfo::GetSpecialBuild(Char *pName, UL32 uLen)
	{
		return GetData(pName, uLen, __Text("SpecialBuild"));
	}

}
