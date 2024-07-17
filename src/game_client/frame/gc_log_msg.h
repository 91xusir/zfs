#ifndef _RT_LOG_MSG_H
#define _RT_LOG_MSG_H

#include "core/rt2_core.h"

/// LogMsg struct ///////////////////////////
/// |----------------------------------------
/// |	msgType	|	dataSize	|	data	|
/// |	(DWORD)	|	(DWORD)		|	...		|
/// |----------------------------------------
/////////////////////////////////////////////

#define MAX_LOGMSG_DATA_SIZE		2048


enum ELogMsgType
{
	LMT_NULL = -1,
	LMT_SYSINFO,
	LMT_ERROR_REPORT,
	LMT_STRING,
	LMT_COUNT
};

class CLogMsg
{
public:
	CLogMsg(void);
	~CLogMsg(void);
	bool SetType(ELogMsgType type)
	{ 
		if (type<=LMT_NULL || type>=LMT_COUNT)
			return false;
		m_type = type;
		return true;
	}
	ELogMsgType GetType()	{ return m_type; }
	char* GetData()	{ return m_szData;	}
	int GetDataSize() { return m_nDataSize; }
	bool WriteData(void *data, int len);		
	bool WriteByte(char c);
	bool WriteBinary(char *data, int len);
	bool ReadBinary(char **data, int *len);
	bool WriteShort(short s);
	bool WriteLong(long l);
	bool WriteFloat(float f);
	bool ReadByte(char *c);
	bool ReadShort(short *s);
	bool ReadLong(long *l);
	bool ReadFloat(float *f);
	bool ReadString(char **str);
	bool WriteString(char *str);
	bool ReadData(void *data, int len);

private:
	bool			m_bError;
	char			*m_pReadData;
	ELogMsgType		m_type;
	unsigned int	m_nDataSize;

	CM_MEMDEF(m_szData, MAX_LOGMSG_DATA_SIZE)
};

#endif
