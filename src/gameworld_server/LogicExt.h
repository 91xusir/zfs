/////////////////////////////////
//Filename:   LogicExt.h
//Author:     PZH
//CreateTime: 2009-12-22
//Desc:		  一些逻辑功能的扩展，一般是新加的功能
////////////////////////////////

#ifndef _LOGIC_EXT_H__
#define _LOGIC_EXT_H__

#include <map>
#include <string>

class GWUnloginClient;
typedef std::map<std::string, unsigned long> NextLoginTimeMap;
class CLogicExt
{
public:
	bool IsLockName(const char* szUserName);
	int GetErrNum(const char* szUserName);
	void DelInNumMap(const char* szUserName);
	void SetErrNum(const char* szUserName, int nNum);
	void AddLock(const char* szUserName);
	void OnRun(unsigned long dwTime);
	int DeleteOldLogin(unsigned long nID, GWUnloginClient *ulc, long lPoint, char cPointType, char cVIP);
	static CLogicExt& GetCLogicExt();
private:
	NextLoginTimeMap& GetNextLoginTimeMap();
	NextLoginTimeMap& GetNumMap();
private:
	NextLoginTimeMap m_NextLoginTimeMap;
	NextLoginTimeMap m_NumMap;
};
#endif//_LOGIC_EXT_H__

