//
// g_CmdPacket.cpp: implementation of the CG_CmdPacket class.
//
#include "rt_net_imp.h"
#include "core/rt2_core.h"

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

	SetSize(byteBufLen,bitBufLen);
}

CG_CmdPacket::~CG_CmdPacket()
{
	/*delete []m_pByteData;
	delete []m_pBitData;*/
	DEL_ARRAY( m_pByteData );
	DEL_ARRAY( m_pBitData );	
}

bool CG_CmdPacket::SetSize(int byteBufLen,int bitBufLen)
{
	if(byteBufLen > MAX_CMD_PACKET_SIZE) return false;

	if(m_byteBufLen < byteBufLen)
	{
		DEL_ARRAY( m_pByteData );
		//delete []m_pByteData;
		m_pByteData = RT_NEW char[byteBufLen];
		m_byteBufLen = byteBufLen;
		m_maxByte    = m_byteBufLen;
	}

	if(bitBufLen > MAX_BIT_MSG_SIZE) return false;
	
	if(m_bitBufLen < bitBufLen)
	{
		DEL_ARRAY( m_pBitData );
		//delete []m_pBitData;
		m_pBitData = RT_NEW char[BitToByte(bitBufLen)];
		m_bitBufLen = bitBufLen;
		m_maxBit    = m_bitBufLen;
	}
	return true;
}

void CG_CmdPacket::Serialize(void *v,int size,ReadWriteMethod pMethod)
{
	if(m_op==OP_READ) 
		ReadData(v,size,pMethod);
	else
		WriteData(v,size,pMethod);
}

void CG_CmdPacket::ByteOrderSerialize(void *v,int size,ReadWriteMethod pMethod)
{
	if(m_op==OP_READ) 
		ReadData(v,size,pMethod);
	else
		WriteData(v,size,pMethod);
}

CG_CmdPacket& operator<<(CG_CmdPacket& Ar, std::string& str)
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

//
// try write data to packet 
//


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

bool CG_CmdPacket::WriteBinary(char *data, int len,ReadWriteMethod pWriteMethod)
{
	if (WriteInt(len,pWriteMethod)==false) 
		return false;
	return WriteData(data,len,pWriteMethod);
}
bool CG_CmdPacket::WriteByte(char c,ReadWriteMethod pWriteMethod)
{
	return WriteData(&c,BYTE_SIZE,pWriteMethod);
}
bool CG_CmdPacket::WriteUByte(unsigned char c,ReadWriteMethod pWriteMethod)
{
	return WriteData(&c,BYTE_SIZE,pWriteMethod);
}
bool CG_CmdPacket::WriteBool(bool b,ReadWriteMethod pWriteMethod)
{
	return WriteData(&b,(sizeof(bool)),pWriteMethod);
}

bool CG_CmdPacket::WriteShort(short s,ReadWriteMethod pWriteMethod)
{
	return WriteData(&s,SHORT_SIZE,pWriteMethod);
}
bool CG_CmdPacket::WriteUShort(unsigned short us,ReadWriteMethod pWriteMethod)
{
	return WriteData(&us,SHORT_SIZE,pWriteMethod);
}

bool CG_CmdPacket::WriteInt(int i,ReadWriteMethod pWriteMethod)
{
	return WriteData(&i,(sizeof(int)),pWriteMethod);
}
bool CG_CmdPacket::WriteLong(long l,ReadWriteMethod pWriteMethod)
{
	return WriteData(&l,LONG_SIZE,pWriteMethod);
}
bool CG_CmdPacket::WriteQWORD(QWORD q,ReadWriteMethod pWriteMethod)
{
	return WriteData(&q, QWORD_SIZE, pWriteMethod);
}
bool CG_CmdPacket::WriteULong(unsigned long ul,ReadWriteMethod pWriteMethod)
{
	return WriteData(&ul,LONG_SIZE,pWriteMethod);
}

bool CG_CmdPacket::WriteFloat(float f,ReadWriteMethod pWriteMethod)
{
	return WriteData(&f,FLOAT_SIZE,pWriteMethod);
}
bool CG_CmdPacket::WriteString(char *str,ReadWriteMethod pWriteMethod)
{
	short len = (short)strlen(str) + 1;
	if (!WriteShort(len,pWriteMethod)) return false;
	return WriteData(str,len,pWriteMethod);
}
bool CG_CmdPacket::WriteData(void *data, int len,ReadWriteMethod pWriteMethod)
{
	if ((m_curByte + len) > m_maxByte) 
	{
		m_bError = true;	
		return false;
	}

	if(len != 0)
	{
		if(pWriteMethod)
		{			
			pWriteMethod(m_pByteData+m_curByte,data,len);
			m_curByte += len;
		}
		else
		{
			memcpy(m_pByteData+m_curByte,data,len);
			m_curByte += len;
		}

	}
	return true;
}

bool CG_CmdPacket::ReadBinary(char **data, int *len,ReadWriteMethod pReadMethod)
{
	if (!ReadInt(len,pReadMethod) || (m_curByte + *len) > m_maxByte)
	{
		m_bError = true;
		return false;
	}

	*data = m_pByteData + m_curByte;
	m_curByte += *len;
	return true;
}
bool CG_CmdPacket::ReadByte(char *c,ReadWriteMethod pReadMethod)
{
	return ReadData(c,BYTE_SIZE,pReadMethod);
}
bool CG_CmdPacket::ReadUByte(unsigned char *c,ReadWriteMethod pReadMethod)
{
	return ReadData(c,BYTE_SIZE,pReadMethod);
}
bool CG_CmdPacket::ReadBool(bool * b,ReadWriteMethod pReadMethod)
{
	return ReadData(b,(sizeof(bool)),pReadMethod);
}
bool CG_CmdPacket::ReadShort(short *s,ReadWriteMethod pReadMethod)
{
	return ReadData(s,SHORT_SIZE,pReadMethod);
}
bool CG_CmdPacket::ReadUShort(unsigned short *us,ReadWriteMethod pReadMethod)
{
	return ReadData(us,SHORT_SIZE,pReadMethod);
}
bool CG_CmdPacket::ReadInt(int * i,ReadWriteMethod pReadMethod)
{
	return ReadData(i,(sizeof(int)),pReadMethod);
}
bool CG_CmdPacket::ReadLong(long *l,ReadWriteMethod pReadMethod)
{
	return ReadData(l,LONG_SIZE,pReadMethod);
}
bool CG_CmdPacket::ReadQWORD(QWORD *q,ReadWriteMethod pReadMethod)
{
	return ReadData(q, QWORD_SIZE, pReadMethod);
}
bool CG_CmdPacket::ReadULong(unsigned long *ul,ReadWriteMethod pReadMethod)
{
	return ReadData(ul,LONG_SIZE,pReadMethod);
}
bool CG_CmdPacket::ReadFloat(float *f,ReadWriteMethod pReadMethod)
{
	return ReadData(f,FLOAT_SIZE,pReadMethod);
}

bool CG_CmdPacket::ReadString(char **str,ReadWriteMethod pReadMethod)
{
	short len;
	if (!ReadShort(&len,pReadMethod) || len <= 0 || (m_curByte + len) > m_maxByte)
	{
		m_bError = true;
		return false;
	}
	
	// set end avoid overflow 

	if(pReadMethod)
	{
		char * szTemp = RT_NEW char[len];
		pReadMethod(m_pByteData + m_curByte,szTemp,len);
		memcpy(m_pByteData + m_curByte,szTemp,len);
		DEL_ARRAY(szTemp);	
	}

	*(m_pByteData + m_curByte + len - 1) = '\0';
	*str = m_pByteData + m_curByte;
	m_curByte += len;	


	return true;
}

bool CG_CmdPacket::ReadData(void *data, int len,ReadWriteMethod pReadMethod)
{
	if ((m_curByte + len) > m_maxByte) 
	{
		m_bError = true;
		return false;
	}

	if(len != 0)
	{
		if(pReadMethod)
		{
			pReadMethod(m_pByteData+m_curByte,data,len);
			m_curByte += len;
		}
		else
		{
			memcpy(data,m_pByteData+m_curByte,len);
			m_curByte += len;
		}
	}
	return true;
}
bool CG_CmdPacket::WritePacket (CG_CmdPacket* pPacket)
{
	int newByteSize = GetByteDataSize() + pPacket->GetByteDataSize();
	if (newByteSize > m_maxByte)
		SetSize(newByteSize);

	return WriteData(pPacket->GetByteData(), pPacket->GetByteDataSize());
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

bool CG_CmdPacket::CloneFromEx(CG_CmdPacket *packet)
{
	if(!CloneFrom(packet)) return false;
	
	m_bError = packet->m_bError;
	m_op = packet->m_op;
	m_curByte = packet->m_curByte;

	return true;
}

bool CG_CmdPacket::CloneBitFromEx(CG_CmdPacket *packet)
{
	if(!CloneBitFrom(packet)) return false;
	
	m_bError = packet->m_bError;
	m_op = packet->m_op;
	m_curBit = packet->m_curBit;
	
	return true;
}

bool CG_CmdPacket::IsByteStreamEnd()
{
	if(IsReading())
	{
		if(m_curByte == m_maxByte) return false;
	}
	return false;
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
	len = BitToByte(GetBitDataSize());
	memcpy(p,&len,sizeof(short));
	p += sizeof(short);
	memcpy(p,GetBitData(),len);
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
	
	if(size != byteDataSize + bitDataSize + 4) 
		return false; 

	byteBuf = buf + 2;
	bitBuf  = buf + 4 + byteDataSize;

	return BeginRead(byteBuf,byteDataSize,bitBuf,bitDataSize);
}

void CG_CmdPacket::GetCurPos(int &curByte, int &curBit)
{
	curBit = m_curBit;
	curByte = m_curByte;
}

void CG_CmdPacket::SetCurPos(int curByte, int curBit)
{
	m_curBit = curBit;
	m_curByte = curByte;
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


RtKeyValue::RtKeyValue() 
{ 
	m_dict = NULL;
}

RtKeyValue::~RtKeyValue() 
{ 
	//if(m_dict) delete m_dict;
	DEL_ONE( m_dict );
}

void RtKeyValue::SetDict(RtDict *dict) 
{
	//if(m_dict) delete m_dict;
	DEL_ONE( m_dict );
	m_dict = RT_NEW RtDict;
	*m_dict = *dict;
}

bool RtKeyValue::operator==(const RtKeyValue &kv) 
{
	if(m_kt != kv.m_kt) return false;
	switch(m_kt) 
	{
		case KT_FLOAT:  return Abs(m_float - kv.m_float) < 0.01;
		case KT_NUMBER: return m_long == kv.m_long;
		case KT_STRING: return m_string == kv.m_string;
		case KT_DICT:   return *m_dict == *kv.m_dict;
		case KT_BLOCK:  return m_block == kv.m_block;
		default:        return false;
	}
}

CG_CmdPacket& operator<<(CG_CmdPacket &packet, RtKeyValue &kv)
{
	char type;
	if(packet.IsReading()) 
	{
		packet << type;
		kv.m_kt = (ERtKeyType)type;
	}
	else
	{
		type = kv.m_kt;
		packet << type;
	}

	switch(kv.m_kt) 
	{
		case KT_FLOAT:  packet << kv.m_float;	break;
		case KT_NUMBER: packet << kv.m_long;	break;
		case KT_STRING: packet << kv.m_string;  break;
		case KT_BLOCK:  packet << kv.m_block;	break;
		case KT_DICT:   packet << *kv.m_dict;	break;
		default: return packet;
	}
	return packet;
}

RtArchive& operator<<(RtArchive &ar, RtKeyValue &kv)
{
	char type;
	if(ar.IsLoading()) 
	{
		ar << type;
		kv.m_kt = (ERtKeyType)type;
	}
	else
	{
		type = kv.m_kt;
		ar << type;
	}

	switch(kv.m_kt) 
	{
		case KT_FLOAT:  ar << kv.m_float;	break;
		case KT_NUMBER: ar << kv.m_long;	break;
		case KT_STRING: ar << kv.m_string;  break;
		// case KT_BLOCK:  ar << kv.m_block;	break;
		case KT_DICT:   ar << *kv.m_dict;	break;
		default: return ar;
	}
	return ar;
}

// ----------------------------------------------------------------------------------
// dict
// ----------------------------------------------------------------------------------
RtDict::RtDict()
{

}

RtDict::~RtDict() 
{

}

bool RtDict::CheckProp(const char *key)
{
	HASH_STR(std::string, RtKeyValue)::iterator it;
	it = m_map.find(key);
	if(it != m_map.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool RtDict::DeleteProp(const char *key)
{
	HASH_STR(std::string, RtKeyValue)::iterator it;
	it = m_map.find(key);
	if(it != m_map.end())
	{
		m_map.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

void RtDict::Clear()
{
	m_map.clear();
}

int RtDict::GetSize()
{
	return (int)m_map.size();
}

void RtDict::SetPropLong(const char *key, long &value)
{
	RtKeyValue v;
	v = value;
	SetProp(key, v);
}

void RtDict::SetPropFloat(const char *key, float &value)
{
	RtKeyValue v;
	v = value;
	SetProp(key, v);
}

void RtDict::SetPropString(const char *key, string &value)
{
	RtKeyValue v;
	v = value;
	SetProp(key, v);
}

bool RtDict::GetPropLong(const char *key, long &value)
{
	RtKeyValue v;
	if(!GetProp(key,v))
		return false;
	if(v.GetType() != KT_NUMBER)
		return false;

	value = v;
	return true;
}

bool RtDict::GetPropFloat(const char *key, float &value)
{
	RtKeyValue v;
	if(!GetProp(key,v))
		return false;
	if(v.GetType() != KT_FLOAT)
		return false;

	value = v;
	return true;
}

bool RtDict::GetPropString(const char *key, string &value)
{
	RtKeyValue v;
	if(!GetProp(key,v))
		return false;
	if(v.GetType() != KT_STRING)
		return false;

	value = v.GetString();
	return true;
}

bool RtDict::GetProp(const char *key, RtKeyValue &value)
{
	HASH_STR(std::string, RtKeyValue)::iterator it;
	it = m_map.find(key);
	if(it != m_map.end())
	{
		value = (*it).second;
		return true;
	}
	else
	{
		return false;
	}
}

bool RtDict::SetProp(const char *key, RtKeyValue &value)
{
    m_map[key] = value;
	return true;
}

void RtDict::SerializeDelta(CG_CmdPacket &packet,RtDict &old)
{

}

RtArchive& operator<<(RtArchive &ar, RtDict &dict)
{
	int		size;
	string	idx;

	if(ar.IsLoading())
	{
		ar << size;
		for(int i=0; i<size; i++)
		{
			RtKeyValue kv;
			ar << idx;
			ar << kv;
			dict.m_map.insert(HASH_STR(std::string,RtKeyValue)::value_type(idx,kv));
		}
	}
	else 
	{
		size = (int)dict.m_map.size();
		ar << size;
		HASH_STR(std::string, RtKeyValue)::iterator it;
		for(it=dict.m_map.begin(); it!=dict.m_map.end(); it++)
		{
			idx = (*it).first;
			ar << idx;
			// ar.WriteString((char*)((*it).first.c_str()));
			ar << (*it).second;
		}
	}	
	return ar;
}

CG_CmdPacket& operator<<(CG_CmdPacket &packet, RtDict &dict) 
{
	int		size;
	string	idx;

	if(packet.IsReading())
	{
		packet << size;
		for(int i=0; i<size; i++)
		{
			RtKeyValue kv;
			packet << idx;
			packet << kv;
			dict.m_map.insert(HASH_STR(std::string,RtKeyValue)::value_type(idx,kv));
		}
	}
	else 
	{
		size = (int)dict.m_map.size();
		packet << size;
		HASH_STR(std::string, RtKeyValue)::iterator it;
		for(it=dict.m_map.begin(); it!=dict.m_map.end(); it++)
		{
			packet.WriteString((char*)((*it).first.c_str()));
			packet << (*it).second;
		}
	}	
	return packet;
}
