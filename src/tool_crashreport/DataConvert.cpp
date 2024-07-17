#include "DataConvert.h"

namespace CrashRP
{
	void CDataConvert::AddressToString(UI64 Addr, Char *pStr, UI32 uStrLen, bool Add0X, UI32 uChars)
	{
		Char szAddr[32] = { 0 };
		_ui64tot_s(Addr, szAddr, 32, 16);

		I32 i = 0;

		if (Add0X)
		{
			pStr[i ++] = __Text('0');
			pStr[i ++] = __Text('X');

			UI32 uLen = (UI32)_tcslen(szAddr);
			while (uLen ++ < uChars)
			{
				pStr[i ++] = __Text('0');
			}
		}

		pStr[i] = 0;

		_tcscat_s(pStr, uStrLen - i, szAddr);
	}

	void CDataConvert::StringToAscii(Char *pSrc, Char_A *pDst, UI32 uDstLen)
	{
#ifdef _UNICODE
		UnicodeToAscii(pSrc, pDst, uDstLen);
#else
		strcpy_s(pDst, uDstLen, pSrc);
#endif
	}

	void CDataConvert::StringToUnicode(Char *pSrc, Char_W *pDst, UI32 uDstLen)
	{
#ifdef _UNICODE
		wcscpy_s(pDst, uDstLen, pSrc);
#else
		AsciiToUnicode(pSrc, pDst, uDstLen);
#endif
	}

	void CDataConvert::AsciiToString(Char_A *pSrc, Char *pDst, UI32 uDstLen)
	{
#ifdef _UNICODE
		AsciiToUnicode(pSrc, pDst, uDstLen);
#else
		strcpy_s(pDst, uDstLen, pSrc);
#endif
	}

	void CDataConvert::UnicodeToString(Char_W *pSrc, Char *pDst, UI32 uDstLen)
	{
#ifdef _UNICODE
		wcscpy_s(pDst, uDstLen, pSrc);
#else
		UnicodeToAscii(pSrc, pDst, uDstLen);
#endif
	}

	void CDataConvert::UnicodeToAscii(Char_W *pSrc, Char_A *pDst, UI32 uDstLen)
	{
		UL32 uNum = WideCharToMultiByte(CP_OEMCP, 0, pSrc, -1, NULL, 0, NULL, FALSE);
		if (uNum < uDstLen)
		{
			WideCharToMultiByte(CP_OEMCP, 0, pSrc, -1, pDst, uNum, NULL, FALSE);
		}
	}

	void CDataConvert::AsciiToUnicode(Char_A *pSrc, Char_W *pDst, UI32 uDstLen)
	{
		UL32 uNum = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
		if (uNum < uDstLen)
		{
			MultiByteToWideChar(CP_ACP, 0, pSrc, -1, pDst, uNum);
		}
	}

	UL32 CDataConvert::AsciiToHash(Char_A *pSrc)
	{
		UL32 uHash = 0;

		Char_A *pStart	= pSrc;
		Char_A *pEnd	= pSrc + strlen(pSrc);

		while (pStart < pEnd)
		{
			uHash = (uHash << 4) + *pStart ++;

			UL32 uTmp = (uHash & 0XF0000000);
			if (uTmp)
			{
				uHash = uHash ^ (uTmp >> 24);
				uHash = uHash ^ uTmp;
			}
		}

		return uHash;
	}
}