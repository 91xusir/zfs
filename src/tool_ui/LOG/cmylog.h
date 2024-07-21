#ifndef __MYLOG__
#define __MYLOG__

#include <vector>
#include <string>
#include <Windows.h>
using namespace std;
typedef enum
{ 
	LOGTOFILE = 1,
	LOGTOCON  = 2,
	LOGDEBUG
}LogType;

class CMyLog
{
protected:
	CMyLog(){}
public:
	static CMyLog *GetInstance(LogType lgType = LOGTOFILE, string strFileName = "");
	static void Release();
	void Log(const char *pInfo);
	void AppendToFile();

	~CMyLog(){}
protected:
	static CMyLog *m_instance;
private:
	void SetFileName(const string &strFileName);
	void Init(LogType lgType = LOGTOFILE, string strFileName = "");//³õÊ¼»¯ÄÚÈÝ

	vector<string>	vsLogList;
	string			m_strFileName;
	LogType         m_LogType;
	CRITICAL_SECTION m_cs;
};

#define LOGTYPE LOGTOFILE
#define LOG(X)  \
	do{ \
		char szBuf[4096] = "\0";  \
		SYSTEMTIME sysTime; \
		GetLocalTime(&sysTime); \
		char szDateTime[128] = "\0"; \
		sprintf_s(szDateTime, "%d-%d-%d %d:%d:%d.%d", \
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, \
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds); \
		sprintf_s(szBuf,  \
		"File:%s\tFunc:%s\tLine:%d\tDateTime:%s\t", \
		__FILE__, __FUNCTION__, __LINE__, szDateTime \
		); \
		string strX(X), strOut = szBuf; \
		if (strX != "" && !strX.empty()) \
		{ \
			strOut += "Info:"; \
			strOut += strX; \
		} \
		CMyLog *p = CMyLog::GetInstance(LOGTYPE, "log.txt"); \
		strOut += "\r\n"; \
		p->Log(strOut.c_str()); \
		p->AppendToFile(); \
	}while(0);
#endif