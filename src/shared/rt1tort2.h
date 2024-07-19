
#ifndef rt1tort2_h__
#define rt1tort2_h__

inline DWORD rtGetMilliseconds() 
{
	return rtMilliseconds();
}

inline bool rtMD5FileString(char const* szSource, char* szMD5)
{
	RtMD5 rtMD5;
	rtMD5.EncodeFile(szSource);
	char buff[33];
	memset(buff, 0, 33);
	rtMD5.GetMD5String(buff);
	memcpy(szMD5, buff, 33);
	return true;
}

inline bool rtMD5DataString(char const* szData, DWORD iLen, char* szMD5)
{
	RtMD5 rtMD5;
	rtMD5.EncodeData(szData, iLen);
	char buff[33];
	memset(buff, 0, 33);
	rtMD5.GetMD5String(buff);
	memcpy(szMD5, buff, 33);
	return true;
}

inline bool rtMD5StringString(char const* szString, char* szMD5)
{
	RtMD5 rtMD5;
	rtMD5.EncodeData(szString, (int)strlen(szString));
	char buff[33];
	memset(buff, 0, 33);
	rtMD5.GetMD5String(buff);
	memcpy(szMD5, buff, 33);
	return true;
}

inline void rtMD5PrintString(char const * buf, unsigned char*)
{

}

 #define ERR5 RtCoreLog().Error
 #define ERR4 RtCoreLog().Error
 #define ERR3 RtCoreLog().Error
 #define ERR2 RtCoreLog().Error
 #define ERR1  RtCoreLog().Error
 #define ERR  RtCoreLog().Error

 #define NOTICE  RtCoreLog().Notice

// #define LOG5  RtCoreLog().Info
// #define LOG4  RtCoreLog().Info
// #define LOG3  RtCoreLog().Info
// #define LOG2  RtCoreLog().Info
// #define LOG1  RtCoreLog().Info
// #define LOG  RtCoreLog().Info
// #define MSG4  RtCoreLog().Info
// #define MSG3  RtCoreLog().Info
// #define MSG2  RtCoreLog().Info
// #define MSG1  RtCoreLog().Info
// #define MSG  RtCoreLog().Info
#define guard funguard
#define unguard fununguard
#define finalunguard finalfununguard
#define guard_name(name) funguard_name(name)

inline void UselessFun(char const* pFmt, ...)
{
	return;
}

//#define ERR5 UselessFun
//#define ERR4 UselessFun
//#define ERR3 UselessFun
//#define ERR2 UselessFun
//#define ERR1 UselessFun
//#define ERR  UselessFun
#define LOG5 UselessFun
#define LOG4 UselessFun
#define LOG3 UselessFun
#define LOG2 UselessFun
#define LOG1 UselessFun
#define LOG  UselessFun
#define MSG4 UselessFun
#define MSG3 UselessFun
#define MSG2 UselessFun
#define MSG1 UselessFun
#define MSG  UselessFun

#define INIT_CS	RT_INIT_CS
#define DELETE_CS RT_DELETE_CS
#define LOCK_CS RT_LOCK_CS
#define UNLOCK_CS RT_UNLOCK_CS

//#define guard_name(name)

// A{{{ [11/3/2008 Louis.Huang]
struct SSystemInfo
{
	char*   sComputerName;
	char*   sCpuName;
	int     nCpuSpeed;
	char*   sDisplayParams;
	char*   sIpAddr;
	char*   sIpMask;
	char*   sLanguage;
	char*   sLoginUserName;
	//char*   sMacAddr[6];
	CM_MEMDEF(sMacAddr, 6)
	char*   sOSType;
	char*   sVideoChip;
	char*   sVideoDriver;
	int     nDriveCount;
	int     nMemTotal;
	int     nMemAvailable;
	int     nVideoMemFree;
	int     nVideoMemTotal;

	SSystemInfo()
	{
		CM_MEMPROTECTOR(sMacAddr, 6)
	}
	SSystemInfo(const SSystemInfo &object)
	{
		memcpy(this, &object, sizeof(SSystemInfo));
		CM_MEMPROTECTOR(sMacAddr, 6)
	}
	~SSystemInfo()
	{
		CM_MEMUNPROTECTOR(sMacAddr)
	}
};

// }}} [11/3/2008 Louis.Huang]
extern char* g_strStaticBuffer;
extern char* g_strStaticBuffer1024;
inline char* rtStaticString1024()
{
	return g_strStaticBuffer1024;
}



#endif // rt1tort2_h__

