//
// g_CmdPacket.cpp: implementation of the CG_CmdPacket class.
//
#include "net/rt_net.h"


CG_CmdPacket::CG_CmdPacket()
{
	m_op = OP_NONE;
	m_bError = false;

	// normal msg
	m_pByteData	 = NULL;
	m_curByte    = 0;
	m_maxByte    = 0;
	m_byteBufLen = 0;

	// bit msg
	m_pBitData  = NULL;
	m_curBit    = 0;
	m_maxBit    = 0;
	m_bitBufLen = 0;
	
	// set buffer to default size 
	SetSize(DEFAULT_CMD_PACKET_SIZE,DEFAULT_BIT_MSG_SIZE);
}

CG_CmdPacket::CG_CmdPacket(int byteBufLen,int bitBufLen)
{
	CG_CmdPacket();
	SetSize(byteBufLen,bitBufLen);
}

CG_CmdPacket::CG_CmdPacket(const CG_CmdPacket& packet)
{
	m_op			= packet.m_op;
	m_bError		= packet.m_bError;

	m_curByte		= packet.m_curByte;
	m_maxByte		= packet.m_maxByte;
	m_byteBufLen	= packet.m_byteBufLen;

	m_curBit		= packet.m_curBit;
	m_maxBit		= packet.m_maxBit;
	m_bitBufLen		= packet.m_bitBufLen;

	m_pByteData		= RT_NEW char[m_byteBufLen];
	m_pBitData		= RT_NEW char[BitToByte(m_bitBufLen)];

	memcpy(m_pByteData, packet.m_pByteData, m_byteBufLen);
	memcpy(m_pBitData, packet.m_pBitData, BitToByte(m_bitBufLen));
}

CG_CmdPacket::~CG_CmdPacket()
{
	DEL_ARRAY(m_pByteData);
	DEL_ARRAY(m_pBitData);
}

bool CG_CmdPacket::SetSize(int byteBufLen,int bitBufLen)
{
	if(byteBufLen > MAX_CMD_PACKET_SIZE) return false;

	if(m_byteBufLen < byteBufLen)
	{
		DEL_ARRAY(m_pByteData);
		m_pByteData = RT_NEW char[byteBufLen];
		m_byteBufLen = byteBufLen;
		m_maxByte    = m_byteBufLen;
	}

	if(bitBufLen > MAX_BIT_MSG_SIZE) return false;
	
	if(m_bitBufLen < bitBufLen)
	{
		DEL_ARRAY(m_pBitData);
		m_pBitData = RT_NEW char[BitToByte(bitBufLen)];
		m_bitBufLen = bitBufLen;
		m_maxBit    = m_bitBufLen;
	}
	return true;
}

void CG_CmdPacket::Serialize(void *v,int size)
{
	if(m_op==OP_READ) 
		ReadData(v,size);
	else
		WriteData(v,size);
}

void CG_CmdPacket::ByteOrderSerialize(void *v,int size)
{
	if(m_op==OP_READ) 
		ReadData(v,size);
	else
		WriteData(v,size);
}

CG_CmdPacket& operator<<( CG_CmdPacket& Ar, std::string& str)
{
	if(Ar.IsReading())
	{
		char *tmp;
		if(!Ar.ReadString(&tmp)) return Ar;
		str = tmp;
		return Ar;
	}
	else
	{
		Ar.WriteString((char*)str.c_str());
		return Ar;
	}
}

bool CG_CmdPacket::ChangeData(char *buf, size_t size, int iDstBegin, size_t iDstSize)
{
	if (buf == NULL) return false;
	if (iDstBegin > m_maxByte || (iDstBegin + (int)iDstSize) > m_maxByte)
		return false;

	if (size == iDstSize)
	{
		memcpy(m_pByteData + iDstBegin, buf, size);
		return true;
	}

	if (size < iDstSize)
	{
		memcpy(m_pByteData + iDstBegin, buf, size);
		memmove(m_pByteData + iDstBegin + size,
			m_pByteData + iDstBegin + iDstSize,
			m_maxByte - iDstBegin - iDstSize);
		m_maxByte = m_maxByte - (int)(iDstSize - size);
		return true;
	}

	if (size > iDstSize)
	{
		int iNewSize = m_maxByte + (int)(size - iDstSize);
		if (iNewSize > MAX_BUFFER_SIZE)
			return false;
		char *pNewBuf = RT_NEW char[iNewSize];
		memcpy(pNewBuf, m_pByteData, iDstBegin);
		memcpy(pNewBuf + iDstBegin, buf, size);
		memcpy(pNewBuf + iDstBegin + size, 
			m_pByteData + iDstBegin + iDstSize,
			m_maxByte - iDstBegin - iDstSize);
		m_maxByte = iNewSize;
		delete[] m_pByteData;
		m_pByteData = pNewBuf;
		return true;
	}

	return false;
}

bool CG_CmdPacket::WritePacket(CG_CmdPacket* pPacket)
{
	if (!pPacket) return false;

	int newByteSize = GetByteDataSize() + pPacket->GetByteDataSize();
	if (newByteSize > m_maxByte)
		SetSize(newByteSize);

	return WriteData(pPacket->GetByteData(), pPacket->GetByteDataSize());
}

//
// try write data to packet 
//
bool CG_CmdPacket::WriteData(void const*data, int len)
{
	if ((m_curByte + len) > m_maxByte) 
	{
		m_bError = true;	
		return false;
	}
	
	if(len != 0)
	{
		memcpy(m_pByteData+m_curByte,data,len);
		m_curByte += len;
	}
	return true;
}

bool CG_CmdPacket::WriteByte(char c)
{
	return WriteData(&c,BYTE_SIZE);
}

bool CG_CmdPacket::BeginRead(char *byteBuf,int byteBufLen,char *bitBuf,int bitBufLen)
{
	if(!SetSize(byteBufLen,bitBufLen)) return false;

	m_bError = false;
	m_op = OP_READ;

	m_curByte = 0;
	m_maxByte = byteBufLen;
	memcpy(m_pByteData,byteBuf,byteBufLen);

	m_curBit  = 0;
	m_maxBit  = bitBufLen;
	memcpy(m_pBitData,bitBuf,BitToByte(bitBufLen));

	return true;
}

void CG_CmdPacket::BeginRead()
{
	// BeginRead(m_pData,m_nLen,(m_maxBitNum+7)/8);
	m_bError = false;
	m_op = OP_READ;

	m_curByte = 0;
	m_curBit  = 0;
}

void CG_CmdPacket::BeginWrite()
{
	m_bError = false;
	m_op = OP_WRITE;

	m_curByte   = 0;
	m_curBit    = 0;
	memset(m_pBitData,0,BitToByte(m_bitBufLen));
}

bool CG_CmdPacket::WriteBinary(char const* data, int len)
{
	if (WriteShort(len)==false) 
		return false;
	return WriteData(data,len);
}

bool CG_CmdPacket::ReadBinary(char **data, int *len)
{
	short dataLen;
	if (!ReadShort(&dataLen) || dataLen <0 || (m_curByte + dataLen) > m_maxByte)
	{
		m_bError = true;
		return false;
	}
	
	*data = m_pByteData + m_curByte;
	*len = dataLen;
	m_curByte += dataLen;
	return true;
}

bool CG_CmdPacket::WriteShort(short s)
{
	return WriteData(&s,SHORT_SIZE);
}

bool CG_CmdPacket::WriteLong(long l)
{
	return WriteData(&l,LONG_SIZE);
}

bool CG_CmdPacket::WriteFloat(float f)
{
	return WriteData(&f,FLOAT_SIZE);
}

bool CG_CmdPacket::ReadByte(char *c)
{
	return ReadData(c,BYTE_SIZE);
}

bool CG_CmdPacket::ReadShort(short *s)
{
	return ReadData(s,SHORT_SIZE);
}

bool CG_CmdPacket::ChangeShort(short i, int iDstBegin)
{
	return ChangeData((char *)&i, SHORT_SIZE, iDstBegin, SHORT_SIZE);
}

bool CG_CmdPacket::ReadCmdOnly(short *s)
{
	return ReadDataOnly(s,SHORT_SIZE);
}

bool CG_CmdPacket::ReadLong(long *l)
{
	return ReadData(l,LONG_SIZE);
}

bool CG_CmdPacket::ReadFloat(float *f)
{
	return ReadData(f,FLOAT_SIZE);
}

bool CG_CmdPacket::ReadString(char **str)
{
	short len;
	if (!ReadShort(&len) || len <= 0 || (m_curByte + len) > m_maxByte)
	{
		m_bError = true;
		return false;
	}
	
	// set end avoid overflow 
	*(m_pByteData + m_curByte + len - 1) = '\0';
	*str = m_pByteData + m_curByte;
	m_curByte += len;
	return true;
}

bool CG_CmdPacket::WriteString(char const* str)
{
	short len = (short)strlen(str) + 1;
	if (!WriteShort(len)) return false;
	return WriteData(str,len);
}

bool CG_CmdPacket::ChangeString(char* szStr, int iDstBegin, size_t iOldStrLen)
{
	short len = (short)strlen(szStr) + 1;
	if (ChangeShort(len, iDstBegin))
		return ChangeData(szStr, len, iDstBegin + SHORT_SIZE, iOldStrLen + 1);
	return false;
}

void CG_CmdPacket::ResetReadPos()
{
	m_curByte = 0;
	m_curBit  = 0;
}

bool CG_CmdPacket::ReadData(void *data, int len)
{
	if ((m_curByte + len) > m_maxByte) 
	{
		m_bError = true;
		return false;
	}

	if(len != 0)
	{
		memcpy(data,m_pByteData+m_curByte,len);
		m_curByte += len;
	}
	return true;
}

bool CG_CmdPacket::ReadDataOnly(void *data, int len)
{
	if ((m_curByte + len) > m_maxByte) 
	{
		m_bError = true;
		return false;
	}

	if(len != 0)
	{
		memcpy(data,m_pByteData+m_curByte,len);
	}
	return true;
}

bool CG_CmdPacket::CloneFrom(CG_CmdPacket *packet)
{
	if(!SetSize(packet->GetByteDataSize(),packet->GetBitDataSize())) 
		return false;

	m_maxByte = packet->GetByteDataSize();
	m_maxBit  = packet->GetBitDataSize();
	memcpy(m_pByteData,packet->GetByteData(),packet->GetByteDataSize());
	memcpy(m_pBitData,packet->GetBitData(),BitToByte(packet->GetBitDataSize()));
	return true;
}

bool CG_CmdPacket::CloneBitFrom(CG_CmdPacket *packet)
{
	if(!SetSize(0,packet->GetBitDataSize())) 
		return false;
	
	// m_maxBit  = packet->GetBitDataSize();
    m_curBit  = packet->GetBitDataSize();
	memcpy(m_pBitData,packet->GetBitData(),BitToByte(packet->GetBitDataSize()));
	return true;
}

bool CG_CmdPacket::SerializeShortEx(short &s)
{
	long flag;
	if(m_op == OP_WRITE)
	{
		if(s > 255 || s < -255) {
			flag = 1;
			SerializeBit(flag,1);
			return WriteShort(s);
		}
		else
		{
			flag = 0;
			SerializeBit(flag,1);
			if(s<0) {
				SerializeBit(flag,1);
				return WriteByte(-s);
			}
			else {
				flag = 1;
				SerializeBit(flag,1);
				return WriteByte(s);
			}
		}
	}
	else
	{
		SerializeBit(flag,1);
		if(flag == 1)
			return ReadShort(&s);
		
		SerializeBit(flag,1);
		unsigned char tmp;
		ReadByte((char*)&tmp);
		if(flag==1)
			s = tmp;
		else
			s = -tmp;
	}
	return true;
}

void CG_CmdPacket::SerializeBit(long &value,long num)
{
	if(m_curBit >= m_maxBit) 
	{
		m_bError = true;
		return;
	}

	if(m_op == OP_READ)
	{
		value = 0;
		long tmp;
		for(int i=0; i<num; i++)
		{
			tmp = m_pBitData[(m_curBit)/8] & (1 << (m_curBit%8));
			tmp = (tmp >> (m_curBit%8)) << i;
			value |= tmp;
			m_curBit++;
			if(m_curBit > m_maxBit) 
			{
				m_bError = true;
				return;
			}
		}
	}
	else
	{
		for(int i=0; i<num; i++)
		{
			char tmp = ((value >> i) & 1) << (m_curBit%8);
			m_pBitData[(m_curBit)/8] |= tmp;
			m_curBit++;
			if(m_curBit > m_maxBit) 
			{
				m_bError = true;
				return;
			}
		}
	}
}

bool CG_CmdPacket::SaveToBuffer(char *buf, int &size)
{
	// byte date size [short]
	// byte data
	// bit date size [short]
	// bit data
	short total = GetByteDataSize() + BitToByte(GetBitDataSize()) + 4;
	if(size < total) return false;

	char *p;
	short len;
	p = buf;
	
	// write byte data
	len = GetByteDataSize();
	memcpy(p,&len,sizeof(short));
	p += sizeof(short);
	memcpy(p,GetByteData(),len);
	p += len;

	// write bit data
	len = GetBitDataSize();
	memcpy(p,&len,sizeof(short));
	p += sizeof(short);
	memcpy(p,GetBitData(),BitToByte(len));
	p += len;

	size = total;
	return true;
}

bool CG_CmdPacket::LoadFromBuffer(char *buf,int size)
{
	char *byteBuf,*bitBuf;
	short byteDataSize,bitDataSize;

	memcpy(&byteDataSize,buf,sizeof(short));
	memcpy(&bitDataSize,buf+2+byteDataSize,sizeof(short));
	
	if(size != byteDataSize + BitToByte(bitDataSize) + 4) 
		return false; 

	byteBuf = buf + 2;
	bitBuf  = buf + 4 + byteDataSize;

	return BeginRead(byteBuf,byteDataSize,bitBuf,bitDataSize);
}

/*
bool CG_CmdPacket::Serialize(std::string &str)
{
	if(m_op == OP_READ)
	{
		char *tmp;
		if(!ReadString(&tmp)) return false;
		str = tmp;
		return true;
	}
	else
	{
		return WriteString((char*)str.c_str());
	}
}

#define IO_DATA(p,size) if(m_op==OP_READ) { return ReadData(p,size);}\
						else { return WriteData(p,size); }
bool CG_CmdPacket::Serialize(long *l)
{
	IO_DATA(l,LONG_SIZE);
}

bool CG_CmdPacket::Serialize(char *c)
{
	IO_DATA(c,BYTE_SIZE);
}

bool CG_CmdPacket::Serialize(short *s)
{
	IO_DATA(s,SHORT_SIZE);
}

bool CG_CmdPacket::Serialize(float *f)
{
	IO_DATA(f,FLOAT_SIZE);
}
*/

