#ifndef _LINUX_UTIL_H_
#define _LINUX_UTIL_H_

#ifdef LINUX
extern int GetIPMacAddr(unsigned char macAddr[], char ip[]);
extern void GetRightPath(char* szPath);
#else
inline void GetRightPath(const char* szPath) {};
#endif //LINUX

#endif //_LINUX_UTIL_H_
