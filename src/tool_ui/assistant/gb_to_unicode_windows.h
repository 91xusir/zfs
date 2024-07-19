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

	//����ת�����ַ���//936�Ǽ�������
	int StrLen=MultiByteToWideChar (936, 0, vrGbString.c_str(),-1, NULL,0) ;

	//��Unicode�����ڴ�
	StringBuffer.Resize( StrLen * sizeof(wchar_t) );
	pUnicodeStr = (wchar_t*) (char*) StringBuffer;

	//ת���ַ�����Unicode�룬ʹ����API����MultiByteToWideChar
	MultiByteToWideChar (936, 0, vrGbString.c_str(), -1, pUnicodeStr,StrLen);

	return pUnicodeStr;
}






















#endif//GB_TO_UNICODE_WINDOWS_H