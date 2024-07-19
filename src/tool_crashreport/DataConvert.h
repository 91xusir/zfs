#pragma once
#include <Config.h>

namespace CrashRP
{
	class CDataConvert
	{
	public:
		static void
			AddressToString(UI64 Addr, Char *pStr, UI32 uStrLen, bool Add0X = false, UI32 uChars = 0);

		static void
			StringToAscii(Char *pSrc, Char_A *pDst, UI32 uDstLen);

		static void
			StringToUnicode(Char *pSrc, Char_W *pDst, UI32 uDstLen);

		static void
			AsciiToString(Char_A *pSrc, Char *pDst, UI32 uDstLen);

		static void
			UnicodeToString(Char_W *pSrc, Char *pDst, UI32 uDstLen);

		static void
			UnicodeToAscii(Char_W *pSrc, Char_A *pDst, UI32 uDstLen);

		static void
			AsciiToUnicode(Char_A *pSrc, Char_W *pDst, UI32 uDstLen);

		static UL32
			AsciiToHash(Char_A *pSrc);
	};
}