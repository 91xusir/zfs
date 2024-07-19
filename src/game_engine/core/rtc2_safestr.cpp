#include "core/rt2_core.h"

#ifdef WIN32
#include <errno.h>
#endif


namespace rt2_core {

bool bSafestrActive = false;

void rt2_ActiveSafestr()
{
	bSafestrActive = true;
}
/*
* Copy src to string dst of size siz.  At most siz-1 characters
* will be copied.  Always NUL terminates (unless siz == 0).
* Returns strlen(src); if retval >= siz, truncation occurred.
*/
size_t strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

/*
* Appends src to string dst of size siz (unlike strncat, siz is the
* full size of dst, not space left).  At most siz-1 characters
* will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
* Returns strlen(src) + MIN(siz, strlen(initial dst)).
* If retval >= siz, truncation occurred.
*/
size_t strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}

char* rt2_strncpy(char *strDest,
		const char *strSource,
		size_t count )//count 为dest长度，不是要拷贝的长度
{
	if (!strDest || !strSource || !count)
	{
		RTASSERT(0);
		return NULL;
	}

	if (bSafestrActive)
	{
#ifdef WIN32
		errno_t err = strncpy_s(strDest, count, strSource, _TRUNCATE);
		if ( err == STRUNCATE )
		{
			RTASSERT(0);
			rtDebugBreak();
		}

#else
		int len = strlcpy(strDest, strSource, count);
		if (len >= count)
		{
			RTASSERT(0);
		}
#endif
	} 
	else
	{
		strncpy(strDest, strSource, count);
	}

	return strDest;
}

char* rt2_strncpy2(char *strDest,
				   const char *strSource,
				   size_t count )//count 为希望拷贝的字节长度，不是dest长度
{

	if (!strDest || !strSource)// || !count)
	{
		RTASSERT(0);
		return NULL;
	}

	strncpy(strDest, strSource, count);

	return strDest;

}

char* rt2_strncat(char *strDest,
				  const char *strSource,
				  size_t count )//count 为strDest总长度，不是本次要追加的长度
{
	if (!strDest || !strSource || !count)
	{
		RTASSERT(0);
		return NULL;
	}

	if (bSafestrActive)
	{
#ifdef WIN32
		errno_t err = strncat_s(strDest, count, strSource, _TRUNCATE);
		if ( err == STRUNCATE )
		{
			RTASSERT(0);
			rtDebugBreak();
		}
#else
		int len = strlcat(strDest, strSource, count);
		if (len >= count)
			RTASSERT(0);
#endif
	} 
	else
	{
		strncat(strDest, strSource, count);
	}

	return strDest;
}


int rt2_snprintf(char *buffer,
				 size_t count,
				 const char *format, ...)//count 为buffer总长度
{
	if (!buffer || !count || !format)
	{
		RTASSERT(0);
		return NULL;
	}
	int iRealWriteCount = -1;
	va_list argList;
	va_start(argList, format);

	if (bSafestrActive)
	{
#ifdef WIN32
		iRealWriteCount = vsnprintf_s(buffer, count, _TRUNCATE, format, argList);
		if ( iRealWriteCount == -1)
		{
			RTASSERT(0);
			rtDebugBreak();
		}
#else
		iRealWriteCount = vsnprintf(buffer, count, format, argList);
		if (iRealWriteCount >= count)
			RTASSERT(0);
#endif
	} 
	else
	{
		iRealWriteCount = vsnprintf(buffer, count, format, argList);
	}

	va_end(argList);

	return iRealWriteCount;
}



char* rt2_strcpy(char *strDest,
				 const char *strSource)
{
	if (!strDest || !strSource)
	{
		RTASSERT(0);
		return NULL;
	}

	if (bSafestrActive)
	{
		size_t iSize = RtMemoryAllocator::Instance().GetMemSize(strDest);

		if (iSize)
		{
			rt2_strncpy(strDest, strSource, iSize);
		}else
		{
			strcpy(strDest, strSource);
		}
	} 
	else
	{
		strcpy(strDest, strSource);
	}

	return strDest;
}

char* rt2_strcat(char *strDest,
				 const char *strSource)
{
	if (!strDest || !strSource)
	{
		RTASSERT(0);
		return NULL;
	}

	if (bSafestrActive)
	{
		size_t iSize = RtMemoryAllocator::Instance().GetMemSize(strDest);

		if (iSize)
		{
			rt2_strncat(strDest, strSource, iSize);
		}else
		{
			strcat(strDest, strSource);
		}

	} 
	else
	{
		strcat(strDest, strSource);
	}

	return strDest;
}

int rt2_sprintf(char *buffer,
				const char *format, ...)
{
	if (!buffer|| !format)
	{
		RTASSERT(0);
		return NULL;
	}


	int iRealWriteCount = -1;
	va_list argList;
	va_start(argList, format);

	if (bSafestrActive)
	{
		size_t iSize = RtMemoryAllocator::Instance().GetMemSize(buffer);

		if (iSize)
		{
#ifdef WIN32
			iRealWriteCount = vsnprintf_s(buffer, iSize, _TRUNCATE, format, argList);
			if ( iRealWriteCount == -1)
			{
				RTASSERT(0);
				rtDebugBreak();
			}
#else
			iRealWriteCount = vsnprintf(buffer, iSize, format, argList);
			if (iRealWriteCount >= iSize)
				RTASSERT(0);
#endif
		}else
		{
			iRealWriteCount = vsprintf(buffer, format, argList);
		}
	} 
	else
	{
		iRealWriteCount = vsprintf(buffer, format, argList);
	}

	va_end(argList);

	return iRealWriteCount;
}

} // namespace rt2_core
