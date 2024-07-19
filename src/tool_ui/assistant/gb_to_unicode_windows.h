#ifndef GB_TO_UNICODE_WINDOWS_H
#define GB_TO_UNICODE_WINDOWS_H

#include <string>
#include "Windows.h"
#include "linearity_buffer.h"

using std::string;

inline wchar_t* GbToUnicode(const string& vrGbString)
{/*LOG("");*/
	static CLinearityBuffer StringBuffer;

	wchar_t* pUnicodeStr=NULL; 

	//计算转换的字符数//936是简体中文
	int StrLen=MultiByteToWideChar (936, 0, vrGbString.c_str(),-1, NULL,0) ;

	//给Unicode分配内存
	StringBuffer.Resize( StrLen * sizeof(wchar_t) );
	pUnicodeStr = (wchar_t*) (char*) StringBuffer;

	//转换字符串到Unicode码，使用了API函数MultiByteToWideChar
	MultiByteToWideChar (936, 0, vrGbString.c_str(), -1, pUnicodeStr,StrLen);

	return pUnicodeStr;
}






















#endif//GB_TO_UNICODE_WINDOWS_H