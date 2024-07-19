/**
* @file rtc2_safestr.h ��ȫ�ַ�������������Խ��ʱ��assert
* @author rendh
*/


namespace rt2_core {

RT2_CORE_API void rt2_ActiveSafestr();//����Safestrϵͳ����

RT2_CORE_API char* rt2_strncpy(char *strDest,
		const char *strSource,
		size_t count );//count Ϊdest���ȣ�����Ҫ�����ĳ���

RT2_CORE_API char* rt2_strncpy2(char *strDest,
							   const char *strSource,
							   size_t count );//count Ϊϣ���������ֽڳ��ȣ�����dest����

RT2_CORE_API char* rt2_strncat(char *strDest,
						 const char *strSource,
						 size_t count );//count ΪstrDest�ܳ��ȣ����Ǳ���Ҫ׷�ӵĳ���


RT2_CORE_API int rt2_snprintf(char *buffer,
						  size_t count,
						  const char *format, ...);//count Ϊbuffer�ܳ���

/************************************************************************/
/* ���º��������滻�ϰ治��ȫ���������´���������                       */
/************************************************************************/
RT2_CORE_API char* rt2_strcpy(char *strDest,
							   const char *strSource);


RT2_CORE_API char* rt2_strcat(char *strDest,
							   const char *strSource);


RT2_CORE_API int rt2_sprintf(char *buffer,
							  const char *format, ...);

} // namespace rt2_core
