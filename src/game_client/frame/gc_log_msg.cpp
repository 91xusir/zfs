//#include "core/rt2_core.h"
#include "gc_log_msg.h"

const int BYTE_SIZE  = 1;
const int LONG_SIZE  = 4;
const int SHORT_SIZE = 2;
const int FLOAT_SIZE = 4;

CLogMsg::CLogMsg(void)
{
	CM_MEMPROTECTOR(m_szData, MAX_LOGMSG_DATA_SIZE)

	m_nDataSize = 0;
	m_bError = false;
	m_pReadData = m_szData;
	m_type = LMT_NULL;
}

CLogMsg::~CLogMsg(void)
{
	CM_MEMUNPROTECTOR(m_szData)
}

bool CLogMsg::WriteData(void *data, int len)
{
	if ((m_nDataSize + len) > MAX_LOGMSG_DATA_SIZE) 
	{
		m_bError = true;	
		return false;
	}
	memcpy(m_szData+m_nDataSize, data, len);
	m_nDataSize += len;
	return true;
}

bool CLogMsg::WriteByte(char c)
{
	return WriteData(&c,BYTE_SIZE);
}

bool CLogMsg::WriteBinary(char *data, int len)
{
	if (WriteShort(len)==false) 
		return false;
	return WriteData(data,len);
}

bool CLogMsg::ReadBinary(char **data, int *len)
{
	short dataLen;
	if (!ReadShort(&dataLen) || dataLen <=0 || ( abs(m_pReadData-m_szData)*sizeof(TCHAR) + dataLen) > MAX_LOGMSG_DATA_SIZE)
	{
		m_bError = true;
		return false;
	}
	
	*data = m_pReadData;
	*len = dataLen;
	m_pReadData += dataLen;
	return true;
}

bool CLogMsg::WriteShort(short s)
{
	return WriteData(&s,SHORT_SIZE);
}

bool CLogMsg::WriteLong(long l)
{
	return WriteData(&l,LONG_SIZE);
}

bool CLogMsg::WriteFloat(float f)
{
	return WriteData(&f,FLOAT_SIZE);
}

bool CLogMsg::ReadByte(char *c)
{
	return ReadData(c,BYTE_SIZE);
}

bool CLogMsg::ReadShort(short *s)
{
	return ReadData(s,SHORT_SIZE);
}

bool CLogMsg::ReadLong(long *l)
{
	return ReadData(l,LONG_SIZE);
}

bool CLogMsg::ReadFloat(float *f)
{
	return ReadData(f,FLOAT_SIZE);
}

bool CLogMsg::ReadString(char **str)
{
	short len;
	if (!ReadShort(&len) || len <= 0 || ( abs(m_pReadData-m_szData)*sizeof(TCHAR) + len) > MAX_LOGMSG_DATA_SIZE)
	{
		m_bError = true;
		return false;
	}
	
	// set end avoid overflow 
	*(m_pReadData+len-1) = '\0';
	*str = m_pReadData;
	m_pReadData += len;
	return true;
}

bool CLogMsg::WriteString(char *str)
{
	short len = (short)strlen(str) + 1;
	if (!WriteShort(len)) return false;
	return WriteData(str,len);
}

bool CLogMsg::ReadData(void *data, int len)
{
	if ( (abs(m_pReadData-m_szData)*sizeof(TCHAR) + len) >MAX_LOGMSG_DATA_SIZE) 
	{
		m_bError = true;
		return false;
	}
	memcpy(data, m_pReadData, len);
	m_pReadData += len;
	return true;
}