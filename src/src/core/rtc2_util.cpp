
#include "core/rt2_core.h"
#include "hashing/rt_md5.h"
#include "core/zlib.h"

namespace rt2_core {

/*-----------------------------------------------------------------------------
-   String functions.
-----------------------------------------------------------------------------*/

RT2_CORE_API char* rtFormatNumber(long lNumber)
{
    static char szBuf[40];
    sprintf(szBuf, "%d", lNumber);
    return szBuf;
}

RT2_CORE_API char* rtEncodeIntegerNumber(char* szBuf, int iBufSize, DWORD dwNumber)
{
    sprintf(szBuf, "%lu", dwNumber);
    int j = (int)strlen(szBuf);
    int i = j/3;
    if ((j%3)==0) i--;
    szBuf[j+i] = 0;
    for (j--; i>0; i--)
    {
        szBuf[j+i]   = szBuf[j];
        szBuf[j+i-1] = szBuf[j-1];
        szBuf[j+i-2] = szBuf[j-2];
        szBuf[j+i-3] = ',';
        j -= 3;
    }
    return szBuf;
}

RT2_CORE_API const char* rtStrRChr(const char* pStr, char cChr)
{
    CHECK(pStr!=NULL);
    if (pStr[0]==0) return NULL;
    const char* p = pStr;
    while (p[1])
    {
        p ++;
    }
    while (p>=pStr)
    {
        if (*p==cChr) return p;
        p --;
    }
    return NULL;
}

RT2_CORE_API const char* rtStrRChrs(const char* pStr, const char* pChrs)
{
    CHECK(pStr!=NULL);
    if (pStr[0]==0) return NULL;
    const char* p = pStr, *p2;
    while (p[1])
    {
        p ++;
    }
    while (p>=pStr)
    {
        p2 = pChrs;
        while (*p2)
        {
            if (*p==*p2) return p;
            p2 ++;
        }
        p --;
    }
    return NULL;
}

RT2_CORE_API int rtChrInStr(const char* pStr, char cChr)
{
    const char* p = pStr;
    while (*p)
    {
        if (*p==cChr) return p-pStr;
        p ++;
    }
    return -1;
}

RT2_CORE_API void rtTrimSpaces( char* pString)
{
    char *pS, *pD;
    pS = pD = pString;
    while (*pS)
    {
        if (*pS!=' ' && *pS!='\a' && *pS!='\t' && *pS!='\n')
        {
            if (pS!=pD) *pD = *pS;
            pD ++;
        }
        *pS ++;
    }
    if (pS!=pD) *pD = '\0';
}

/*-----------------------------------------------------------------------------
-   Memory functions.
-----------------------------------------------------------------------------*/


RT2_CORE_API BOOL rtMemIsZero( const void* pV, int iCount )
{
    char* pBuf = (char*)pV;
    while (iCount-->0)
    {
        if (*pBuf!=0)
            return FALSE;
        pBuf ++;
    }
    return TRUE;
}

RT2_CORE_API void rtMemswap( void* pBuf1, void* pBuf2, DWORD iSize )
{
    char *pC1=(char*)pBuf1, *pC2=(char*)pBuf2, c;
    while (iSize-->0)
    {
        c = *pC1;
        *pC1 = *pC2;
        *pC2 = c;
        pC1 ++;
        pC2 ++;
    }
}

/*-----------------------------------------------------------------------------
-   Lzw数据压缩
-----------------------------------------------------------------------------*/
#pragma warning( push )
#pragma warning( disable : 4311) /* 'type cast' : pointer truncation from 'unsigned char *' to 'unsigned int' */

unsigned _rtDoLzwCops(unsigned char *in,unsigned in_len,unsigned char *out,unsigned *out_len)
{
	static long c[16384L] ;
    register unsigned char *i;unsigned char ** j = ( unsigned char ** )c;
	unsigned char *a,*b,*n=in+in_len,*p=in+in_len-13;
    a=out,i=in,b=i;i+=4;
    for (;;) {
		register unsigned char *d;unsigned e,f,g;
		g=((0x21*(((((((unsigned)(i[3])<<6)^i[2])<<5)^i[1])<<5)^i[0]))>>5)&0x3fff;d=j[g];
		if(((unsigned)d<(unsigned)in)||(e=(unsigned)((unsigned)i-(unsigned)d))<=0||e>0xbfff)goto literal;
		if (e<=0x0800||d[3]==i[3])goto try_match;
		g=(g&0x7ff)^0x201f;d=j[g];
		if((unsigned)(d)<(unsigned)(in)||(e=(unsigned)((int)((unsigned)i-(unsigned)d)))<=0||e>0xbfff)goto literal;
		if(e<=0x0800||d[3]==i[3])goto try_match;
		goto literal;
try_match:
		if(*(unsigned short*)d==*(unsigned short*)i&&d[2]==i[2])goto match;
literal:
		j[g]=i++;
		if (i>=p)break;
		continue;
match:
		j[g]=i;
		if(i-b>0){
			register unsigned t=i-b;
			if(t<=3) a[-2]|=(unsigned char)t;
			else if(t<=18)	*a++=(unsigned char)(t-3);
			else{
				register unsigned tt=t-18;*a++=0;
				while(tt>255){tt-=255;*a++=0;}
				*a++=(unsigned char)tt;
			}
			do *a++=*b++; while(--t>0);
		}
		i += 3;
		if (d[3]!=*i++||d[4]!=*i++||d[5]!=*i++||d[6]!=*i++||d[7]!=*i++||d[8]!=*i++){
			--i;f=i-b;
			if(e<=0x0800){--e;*a++=(unsigned char)(((f-1)<<5)|((e&7)<<2));*a++=(unsigned char)(e>>3);}
			else if(e<=0x4000){--e;*a++=(unsigned char)(32|(f-2));goto m3_m4_offset;}
			else {e-=0x4000;*a++=(unsigned char)(16|((e&0x4000)>>11)|(f-2));goto m3_m4_offset;}
		}else{
			{unsigned char *end=n;unsigned char *m=d+9;while(i<end&&*m==*i)m++,i++;f=(i-b);}
			if(e<=0x4000){--e;if(f<=33)*a++=(unsigned char)(32|(f-2));else{f-=33;*a++=32;goto m3_m4_len;}}
			else{
				e-=0x4000;
				if(f<=9)*a++=(unsigned char)(16|((e&0x4000)>>11)|(f-2));
				else{f-=9;*a++=(unsigned char)(16|((e&0x4000)>>11));
m3_m4_len:
				while(f>255){f-=255;*a++=0;}
				*a++ = (unsigned char)f;
				}
			}
m3_m4_offset:
			*a++=(unsigned char)((e&63)<<2);*a++=(unsigned char)(e>>6);
		}
		b=i;
		if(i>=p)break;
    }
    *out_len=a-out;
    return (unsigned) (n - b);
}

int	rtLzwComp(void *in,unsigned in_len,void *out)
{
    unsigned char *a=(unsigned char*)out;unsigned t,out_len;
    if(in_len<=13)t = in_len;
    else{t=_rtDoLzwCops((unsigned char*)in,in_len,a,&out_len);a+=out_len;}
    if(t>0){
		unsigned char *b=(unsigned char*)in+in_len-t;
		if(a==(unsigned char*)out&&t<=238)*a++=(unsigned char)(17+t);
		else if(t<=3)a[-2]|=(unsigned char)t ;
		else if(t<=18)*a++=(unsigned char)(t-3);
		else {unsigned tt=t-18;*a++=0;while(tt>255){tt-=255;*a++=0;}*a++=(unsigned char)tt;}
		do *a++=*b++; while(--t>0);
    }
    *a++=17;*a++=0;*a++=0;
    return(a-(unsigned char*)out);
}

int	rtLzwUncomp(void *in,unsigned in_len,void *out)
{
	register unsigned char *a,*i,*d;
	register unsigned t;
	unsigned char *p=(unsigned char*)in+in_len;
	a=(unsigned char*)out;i=(unsigned char*)in;
	if(*i>17){
		t=*i++-17;
		if(t<4)goto match_next;
		do *a++=*i++; while(--t>0);
		goto first_literal_run;
	}
	while(1){
		t=*i++;
		if(t>=16)goto match;
		if(t==0){while(*i==0){t+=255;i++;}t+=15+*i++;}
		*(unsigned*)a=*(unsigned*)i;a+=4;i+=4;
		if(--t>0){
			if (t >= 4){do{*(unsigned*)a=*(unsigned*)i;a+=4;i+=4;t-=4;}while(t>=4);if(t>0)do*a++=*i++;while(--t>0);}
			else do*a++=*i++;while(--t>0);
		}
first_literal_run:
		t=*i++;
		if(t>=16)goto match;
		d=a-0x0801;d-=t>>2;d-=*i++<<2;*a++=*d++;*a++=*d++;*a++=*d;
		goto match_done;
		while (1){
match:
		if(t>=64){d=a-1;d-=(t>>2)&7;d-=*i++<<3;t=(t>>5)-1;goto copy_match;}
		else if(t>=32){
			t&=31;
			if(t==0){while(*i==0){t+=255;i++;}t+=31+*i++;}
			d=a-1;d-=(*(unsigned short*)i)>>2;i+=2;
		}
		else if(t>=16){
			d=a;d-=(t&8)<<11;t&=7;
			if(t==0){while(*i==0){t+=255;i++;}t+=7+*i++;}
			d-=(*(unsigned short*)i)>>2;i+=2;
			if(d==a)goto eof_found;
			d-=0x4000;
		}else{d=a-1;d-=t>>2;d-=*i++<<2;*a++=*d++;*a++=*d;goto match_done;}
		if (t >= 6 && (a - d) >= 4) {
			* (unsigned *) a = * ( unsigned *) d;
			a += 4; d += 4; t -= 2;
			do {*(unsigned*)a=*(unsigned*)d;a+=4;d+=4;t-=4;}while(t>=4);
			if(t>0)do*a++=*d++;while(--t>0);
		}else{
copy_match:
		*a++=*d++;*a++=*d++;
		do*a++=*d++;while(--t>0);
		}
match_done:
		t=i[-2]&3;
		if(t==0)break;
match_next:
		do*a++=*i++;while(--t>0);
		t=*i++;
		}
	}
eof_found:
	if(i!=p)return-1;
	return(a-(unsigned char*)out);
}

#pragma warning( pop )

int RtZCompress::Compress(void *in,unsigned int in_len,void *out)
{
    return rtLzwComp(in, in_len, out);
}

int RtZCompress::Decompression(void *in,unsigned int in_len,void *out)
{
    return rtLzwUncomp(in, in_len, out);
}

/*-----------------------------------------------------------------------------
-   ZLib数据压缩
-----------------------------------------------------------------------------*/

bool RtZLib::Compress(void *in,unsigned int in_len,void *out, unsigned int& out_len)
{
    return (compress((Bytef*)out,(uLongf*)&out_len,(const Bytef*)in,in_len)==Z_OK);
}

bool RtZLib::Decompression(void *in,unsigned int in_len,void *out, unsigned int& out_len)
{
    return (uncompress((Bytef*)out,(uLongf*)&out_len,(const Bytef*)in,in_len)==Z_OK);
}

/*-----------------------------------------------------------------------------
-   MD5编码
-----------------------------------------------------------------------------*/
namespace rtc_hashing {

RtMD5::RtMD5()
{
    MD5_Init((MD5_CTX*)&m_context);
}

void RtMD5::Update(unsigned char *pData, unsigned int dwLen)
{
    MD5_Update((MD5_CTX*)&m_context, pData, dwLen);
}

void RtMD5::Final()
{
    MD5_Final((MD5_CTX*)&m_context, m_pDigest);
}

unsigned char* RtMD5::GetMD5(unsigned char pDigest[16])
{
    if (pDigest)
    {
        memcpy(pDigest, m_pDigest, 16);
    }
    return m_pDigest;
}

void RtMD5::GetMD5String(char *szString)
{
    unsigned int i;
    for (i = 0; i < 16; i++)
    {
        sprintf (szString+i*2, "%02x", m_pDigest[i]);
    }
}

bool RtMD5::EncodeFile(char const*filename)
{
    return rtMD5_File(filename,(char*)m_pDigest);
    
}

void RtMD5::EncodeString(char const *str)
{
    rtMD5_String(str, (char*)m_pDigest);
}

void RtMD5::EncodeData(char const *data, int len)
{
    rtMD5_Data(data, len, (char*)m_pDigest);
}
} // namespace rtc_hashing

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
