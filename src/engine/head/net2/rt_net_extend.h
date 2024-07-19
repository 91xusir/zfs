#ifndef __RT_NET_EXTEND_H__
#define __RT_NET_EXTEND_H__

#include "core/rt2_core.h"

/* define default packet buffer size */
const int DEFAULT_CMD_PACKET_SIZE = 1024;
const int DEFAULT_BIT_MSG_SIZE    = 32*8;

/* define max packet buffer size */
const int MAX_CMD_PACKET_SIZE	  = 1000 * 64;
const int MAX_BIT_MSG_SIZE        = 255*8; 

/* define some length used in packet class */
const int BYTE_SIZE  = 1;
const int LONG_SIZE  = 4;
const int QWORD_SIZE = 8;
const int SHORT_SIZE = 2;
const int FLOAT_SIZE = 4;

const int OP_NONE  = 0;
const int OP_READ  = 1;
const int OP_WRITE = 2;

inline int ByteToBit(int byte)
{
	return byte*8;
}

inline int BitToByte(int bit)
{
	return (bit+7)/8;
}

typedef void (*ReadWriteMethod)(char * pSourceData,void * pTargetData,int iReadLen);

// look like Archive...
class CG_CmdPacket  
{
public:
	CG_CmdPacket();
	CG_CmdPacket(int byteBufLen,int bitBufLen);
	virtual ~CG_CmdPacket();

	void BeginRead();
	void BeginWrite();
	bool BeginRead(char *byteBuf,int byteBufLen,char *bitBuf,int bitBufLen);
	
	// bit msg function
	void SerializeBit(long &value,long num);
	
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, BYTE& B )  { Ar.Serialize( &B, 1 );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, SBYTE& B ) { Ar.Serialize( &B, 1 );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, WORD& W )  { Ar.ByteOrderSerialize( &W, sizeof(W) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, SWORD& S ) { Ar.ByteOrderSerialize( &S, sizeof(S) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, DWORD& D ) { Ar.ByteOrderSerialize( &D, sizeof(D) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, INT& I )   { Ar.ByteOrderSerialize( &I, sizeof(I) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, FLOAT& F ) { Ar.ByteOrderSerialize( &F, sizeof(F) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, QWORD& Q ) { Ar.ByteOrderSerialize( &Q, sizeof(Q) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, SQWORD& S) { Ar.ByteOrderSerialize( &S, sizeof(S) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, RtTime& F) { Ar.ByteOrderSerialize( &F, sizeof(F) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, long &L)   { Ar.ByteOrderSerialize( &L, sizeof(L) );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, char &C )  { Ar.Serialize( &C, 1 );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, bool &B )  { Ar.Serialize( &B, 1 );return Ar;}
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, std::string& str ); 
	
	void Serialize(void *v,int size,ReadWriteMethod pMethod=NULL);
	void ByteOrderSerialize(void *v,int size,ReadWriteMethod pMethod=NULL);

	bool SerializeShortEx(short &s);
	/*
	bool Serialize(std::string &str);
	bool Serialize(long *l);
	bool Serialize(char *c);
	bool Serialize(short *s);
	bool Serialize(float *f);
	*/	
	
	bool ReadData(void *data,int len,ReadWriteMethod pReadMethod=NULL);
	bool ReadBinary(char **data,int *len,ReadWriteMethod pReadMethod=NULL);	
	bool ReadByte(char *c,ReadWriteMethod pReadMethod=NULL);
	bool ReadUByte(unsigned char *c,ReadWriteMethod pReadMethod=NULL);
	bool ReadBool(bool *b,ReadWriteMethod pReadMethod=NULL);
	bool ReadFloat(float *f,ReadWriteMethod pReadMethod=NULL);
	bool ReadInt(int *i,ReadWriteMethod pReadMethod=NULL);
	bool ReadLong(long *l,ReadWriteMethod pReadMethod=NULL);
	bool ReadQWORD(QWORD *q,ReadWriteMethod pReadMethod=NULL);
	bool ReadULong(unsigned long *ul,ReadWriteMethod pReadMethod=NULL);
	bool ReadShort(short *s,ReadWriteMethod pReadMethod=NULL);
	bool ReadUShort(unsigned short *us,ReadWriteMethod pReadMethod=NULL);
	bool ReadString(char **str,ReadWriteMethod pReadMethod=NULL);
	
	bool WriteData(void *data,int len,ReadWriteMethod pWriteMethod=NULL);
	bool WriteBinary(char *data,int len,ReadWriteMethod pWriteMethod=NULL);
	bool WriteByte(char c,ReadWriteMethod pWriteMethod=NULL);
	bool WriteUByte(unsigned char c,ReadWriteMethod pWriteMethod=NULL);
	bool WriteBool(bool b,ReadWriteMethod pWriteMethod=NULL);
	bool WriteFloat(float f,ReadWriteMethod pWriteMethod=NULL);
	bool WriteInt(int i,ReadWriteMethod pWriteMethod=NULL);
	bool WriteLong(long l,ReadWriteMethod pWriteMethod=NULL);
	bool WriteQWORD(QWORD q,ReadWriteMethod pWriteMethod=NULL);
	bool WriteULong(unsigned long ul,ReadWriteMethod pWriteMethod=NULL);
	bool WriteShort(short s,ReadWriteMethod pWriteMethod=NULL);
	bool WriteUShort(unsigned short us,ReadWriteMethod pWriteMethod=NULL);
	bool WriteString(char *str,ReadWriteMethod pWriteMethod=NULL);

	bool WritePacket (CG_CmdPacket* pPacket);

	char *GetByteData() { return m_pByteData; }
	int   GetByteDataSize()
	{ 
		if(m_op==OP_READ)
			return m_maxByte;
		else
			return m_curByte; 
	}
	
	// bit msg
	char *GetBitData() { return m_pBitData; }
	int   GetBitDataSize() 
	{ 
		if(m_op==OP_READ)
			return m_maxBit;
		else
			return m_curBit; 
	}

	bool  SetSize(int byteBufLen,int bitBufLen=MAX_BIT_MSG_SIZE);
	bool  CloneFrom(CG_CmdPacket *packet);
	bool  CloneBitFrom(CG_CmdPacket *packet);

	bool  CloneFromEx(CG_CmdPacket *packet);
	bool  CloneBitFromEx(CG_CmdPacket *packet);

	bool  IsReading() { return m_op == OP_READ; }
	bool  IsWritting() { return m_op == OP_WRITE; }
	bool  IsError() { return m_bError; }
	bool  IsByteStreamEnd();

	bool  SaveToBuffer(char *buf,int &size);
	bool  LoadFromBuffer(char *buf,int size);
	
	void  GetCurPos(int &curByte, int &curBit);
	void  SetCurPos(int curByte, int curBit);


private:
	
	

	int   m_op;
	bool  m_bError;
	
	// byte msg
	char *m_pByteData;
	int   m_curByte;
	int   m_maxByte;
	int   m_byteBufLen;
	
	// bit msg
	char *m_pBitData;
	int   m_curBit;
	int   m_maxBit;
	int   m_bitBufLen;
};

#define NET_READ_ERROR          CHECKEX("Net read error")
#define NET_READ_CMD(d,cmd)     if (!pPacket->cmd(d)) {NET_READ_ERROR;return;}
#define NET_READ_CMDb(d,cmd)    if (!pPacket->cmd(d)) {NET_READ_ERROR;return false;}

#define READ_char(d)            NET_READ_CMD(&d, ReadByte)
#define READ_byte(d)            NET_READ_CMD((char*)&d, ReadByte)
#define READ_string(d)          NET_READ_CMD(&d, ReadString)
#define READ_short(d)           NET_READ_CMD(&d, ReadShort)
#define READ_ulong(d)           NET_READ_CMD((long*)&d, ReadLong)
#define READ_long(d)            NET_READ_CMD(&d, ReadLong)
#define READ_float(d)           NET_READ_CMD(&d, ReadFloat)

#define READb_char(d)           NET_READ_CMDb(&d, ReadByte)
#define READb_byte(d)           NET_READ_CMDb((char*)&d, ReadByte)
#define READb_string(d)         NET_READ_CMDb(&d, ReadString)
#define READb_short(d)          NET_READ_CMDb(&d, ReadShort)
#define READb_ulong(d)          NET_READ_CMDb((long*)&d, ReadLong)
#define READb_long(d)           NET_READ_CMDb(&d, ReadLong)
#define READb_float(d)          NET_READ_CMDb(&d, ReadFloat)

// serialize delta msg
template<class T>
inline void SerializeDelta(CG_CmdPacket &cmd,T &cur,T &old)
{
	long flag,tmp;
	if(cmd.IsReading())
	{
		cmd.SerializeBit(flag,1);
		if(flag == 1) 
		{
			// RtCoreLog().Info("read 1 bit\n");
			cmd << cur;
		}
		else
			cur = old;
	}
	else
	{
		if(cur == old)
			tmp = 0;
		else
			tmp = 1;
		cmd.SerializeBit(tmp,1);
		if(tmp == 1)
		{
			cmd << cur;
			// RtCoreLog().Info("write 1 bit\n");
		}
	}
}

#define SLZ_DELTA(v) ::SerializeDelta(cmd,v,old.v);

class RtKeyValue;
class RtDict;

struct RtMemoryBlock
{
	RtMemoryBlock() { 
		data = NULL; 
		size = 0;
	}
	~RtMemoryBlock() {
		//if(data) delete []data;
		DEL_ARRAY( data );
	}
	void SetData(void *buf,int len)
	{
		//if(data) delete []data;
		DEL_ARRAY( data );
		data = NULL;
		size = len;
		if(size != 0)
		{
			data = RT_NEW char[size];
			memcpy(data,buf,size);
		}
	}
	void *data;
	int   size;

	const RtMemoryBlock& operator=(const RtMemoryBlock &block) {
		SetData(block.data, block.size);
		return block;
	}
	bool operator==(const RtMemoryBlock &block) {
		return false;
	}
	friend CG_CmdPacket& operator<<(CG_CmdPacket& cmd, RtMemoryBlock &block) {
		if(cmd.IsReading()) {
			char *tmp;
			int size;
			cmd.ReadBinary(&tmp,&size);
			block.SetData(tmp,size);
		}
		else {
			cmd.WriteBinary((char*)block.data,block.size);
		}	
		return cmd;
	}
};

enum ERtKeyType 
{
	KT_STRING	= 1,
	KT_DICT		= 2,
	KT_NUMBER	= 3,
	KT_FLOAT	= 4,
	KT_BLOCK	= 5,
};

class RtKeyValue
{
public:
	RtKeyValue();
	virtual ~RtKeyValue();

	ERtKeyType GetType() { return m_kt;	}
	const char *GetString() { return m_string.c_str(); }
	RtDict *GetDict() { return m_dict; }

	operator float()		{ return (float)m_float; }
	operator double()		{ return (double)m_float; }
	operator int()			{ return (int)m_long; }
	operator unsigned int()	{ return (unsigned int)m_long; }

	operator char()		{ return (char)m_long; }
	operator short()	{ return (short)m_long; }
	operator long()		{ return (long)m_long; }
	operator unsigned char()	{ return (unsigned char)m_long; }
	operator unsigned short()	{ return (unsigned short)m_long; }
	operator unsigned long()	{ return (unsigned long)m_long; }

	operator RtDict&()	{ return *m_dict; }
	operator std::string&()  { return m_string; }
	operator const char*() { return m_string.c_str(); }

	const std::string&  	operator=(const std::string &str)	{ m_kt = KT_STRING; m_string = str; return str; };
	const char *			operator=(const char *str)			{ m_kt = KT_STRING; m_string = str; return str; };
	const float             operator=(const float &v)			{ m_kt = KT_FLOAT;  m_float = v; return v; }
	const double            operator=(const double &v)			{ m_kt = KT_FLOAT;  m_float = v; return v; }
	const int				operator=(const int &v)				{ m_kt = KT_NUMBER; m_long = v; return v; }
	const unsigned int		operator=(const unsigned int &v)    { m_kt = KT_NUMBER; m_long = v; return v; }
	const char              operator=(const char &v)			{ m_kt = KT_NUMBER; m_long = v; return v; }
	const short             operator=(const short &v)			{ m_kt = KT_NUMBER; m_long = v; return v; }
	const long              operator=(const long &v)			{ m_kt = KT_NUMBER; m_long = v; return v; }
	const unsigned char     operator=(const unsigned char &v)   { m_kt = KT_NUMBER; m_long = v; return v; }
	const unsigned short    operator=(const unsigned short &v)  { m_kt = KT_NUMBER; m_long = v; return v; }
	const unsigned long     operator=(const unsigned long &v)   { m_kt = KT_NUMBER; m_long = v; return v; }

	const RtMemoryBlock&    operator=(const RtMemoryBlock &block) { m_kt = KT_BLOCK; m_block = block; return block; };
	const RtDict&     		operator=(const RtDict &dict)	{ 
		m_kt = KT_DICT; SetDict((RtDict*)&dict); return dict; 
	};
	const RtKeyValue		operator=(const RtKeyValue &kv) { 
		m_kt = kv.m_kt; 
		switch(m_kt) {
			case KT_NUMBER:	m_long   = kv.m_long;	break;
			case KT_FLOAT:  m_float  = kv.m_float;	break;
			case KT_STRING: m_string = kv.m_string; break;
			case KT_DICT:   SetDict(kv.m_dict);		break;
			case KT_BLOCK:  m_block  = kv.m_block;  break;
		}
		return kv;
	}
	friend CG_CmdPacket& operator<<(CG_CmdPacket &packet, RtKeyValue &kv);
	friend RtArchive& operator<<(RtArchive &ar, RtKeyValue &kv);

	bool operator==(const RtKeyValue &kv);

private:
	void SetDict(RtDict *dict);

	ERtKeyType	   m_kt;
	long           m_long;
	float          m_float;
	std::string    m_string;
	RtDict        *m_dict;
	RtMemoryBlock  m_block;
};

class RtDict
{
public:
	RtDict();
	virtual ~RtDict();

	void SetPropLong(const char *key, long &value);
	void SetPropFloat(const char *key, float &value);
	void SetPropString(const char *key, string &value);

	bool GetPropLong(const char *key, long &value);
	bool GetPropFloat(const char *key, float &value);
	bool GetPropString(const char *key, string &value);

	bool CheckProp(const char *key);
	bool DeleteProp(const char *key);
	void Clear();
	int  GetSize();

	const RtDict& operator=(const RtDict& dict)
	{
		m_map = dict.m_map;
		return dict;
	}
	bool operator==(const RtDict& dict)
	{
		return false;
	}

	friend CG_CmdPacket& operator<<(CG_CmdPacket &packet, RtDict &dict); 
	friend RtArchive& operator<<(RtArchive &ar, RtDict &dict); 
	void SerializeDelta(CG_CmdPacket &packet,RtDict &old);
	
private:
	bool GetProp(const char *key, RtKeyValue &value);
	bool SetProp(const char *key, RtKeyValue &value);
	HASH_STR(std::string, RtKeyValue) m_map;
};

#endif
