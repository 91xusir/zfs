#include "cmylog.h"
#include <iostream>
#include <Windows.h>
CMyLog *CMyLog::m_instance = NULL;

CMyLog *CMyLog::GetInstance(LogType lgType, string strFileName)
{
	if (m_instance == NULL)
	{
		m_instance = new CMyLog;
		m_instance->Init(lgType, strFileName);
		InitializeCriticalSection(&(m_instance->m_cs));
	}

	return m_instance;
}

void CMyLog::Release()
{
	if (m_instance != NULL)
	{
		DeleteCriticalSection(&(m_instance->m_cs));
		delete m_instance;
		m_instance = NULL;
	}
}

void CMyLog::Log(const char *pInfo)
{
	switch (m_LogType)
	{
	case LOGTOFILE:
		vsLogList.push_back(pInfo);
		break;
	case LOGTOCON:
		cout<<pInfo<<endl;
		break;
	case LOGDEBUG:
		OutputDebugString(pInfo);
	default:
		break;
	}
}

void CMyLog::AppendToFile()
{
	EnterCriticalSection(&m_cs);
	if (vsLogList.empty())
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	if (m_strFileName.empty())
	{
		LeaveCriticalSection(&m_cs);
		return;
	}
	if (m_LogType != LOGTOFILE)
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	FILE *fp = NULL;
	fopen_s(&fp, m_strFileName.c_str(), "a+");
	if (fp == NULL)
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	vector<string>::iterator vsi = vsLogList.begin();
	for (; vsi!=vsLogList.end(); vsi++)
	{
		fwrite(vsi->c_str(), vsi->length(), 1, fp);
	}
	fclose(fp);
	vsLogList.clear();
	LeaveCriticalSection(&m_cs);
}

void CMyLog::SetFileName(const string &strFileName){m_strFileName = strFileName;}

void CMyLog::Init(LogType lgType, string strFileName)//³õÊ¼»¯ÄÚÈÝ
{
	vsLogList.clear();
	vsLogList.swap(vsLogList);

	m_LogType = lgType;
	if (strFileName == "" || strFileName.empty())
	{
		m_strFileName = "log.log";
	}
	m_strFileName = strFileName;
}
