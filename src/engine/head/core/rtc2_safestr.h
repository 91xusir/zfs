/**
* @file rtc2_safestr.h 安全字符串函数集，在越界时会assert
* @author rendh
*/


namespace rt2_core {

RT2_CORE_API void rt2_ActiveSafestr();//激活Safestr系统函数

RT2_CORE_API char* rt2_strncpy(char *strDest,
		const char *strSource,
		size_t count );//count 为dest长度，不是要拷贝的长度

RT2_CORE_API char* rt2_strncpy2(char *strDest,
							   const char *strSource,
							   size_t count );//count 为希望拷贝的字节长度，不是dest长度

RT2_CORE_API char* rt2_strncat(char *strDest,
						 const char *strSource,
						 size_t count );//count 为strDest总长度，不是本次要追加的长度


RT2_CORE_API int rt2_snprintf(char *buffer,
						  size_t count,
						  const char *format, ...);//count 为buffer总长度

/************************************************************************/
/* 以下函数用于替换老版不安全函数，在新代码中勿用                       */
/************************************************************************/
RT2_CORE_API char* rt2_strcpy(char *strDest,
							   const char *strSource);


RT2_CORE_API char* rt2_strcat(char *strDest,
							   const char *strSource);


RT2_CORE_API int rt2_sprintf(char *buffer,
							  const char *format, ...);

} // namespace rt2_core
