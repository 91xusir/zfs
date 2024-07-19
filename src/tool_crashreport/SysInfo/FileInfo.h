#pragma once
#include <config.h>

namespace CrashRP
{
	class CFileInfo
	{
	public:
		CFileInfo(Char *pFileName);
		virtual ~CFileInfo();
	public:
		bool GetComments(Char *pName, UL32 uLen);
		bool GetInternalName(Char *pName, UL32 uLen);
		bool GetProductName(Char *pName, UL32 uLen);
		bool GetCompanyName(Char *pName, UL32 uLen);
		bool GetLegalCopyright(Char *pName, UL32 uLen);
		bool GetProductVersion(Char *pName, UL32 uLen);
		bool GetFileDescription(Char *pName, UL32 uLen);
		bool GetLegalTrademarks(Char *pName, UL32 uLen);
		bool GetPrivateBuild(Char *pName, UL32 uLen);
		bool GetFileVersion(Char *pName, UL32 uLen);
		bool GetOriginalFilename(Char *pName, UL32 uLen);
		bool GetSpecialBuild(Char *pName, UL32 uLen);
	private:
		bool Init();

		bool GetData(Char *pName, UL32 uLen, Char *pStr);
	private:
		Char	mFileName[512];
		bool	mbInit;

		Char_U	*mpDataBuff;

		US32	mLanguage;
		US32	mCodePage;
	};
}