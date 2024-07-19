//
// g_CmdPacket.h: interface for the CG_CmdPacket class.
//
#ifndef __CG_CMD_PACKET_H__
#define __CG_CMD_PACKET_H__

/* define default packet buffer size */
const int DEFAULT_CMD_PACKET_SIZE = 1024;
const int DEFAULT_BIT_MSG_SIZE    = 32*8;

/* define max packet buffer size */
const int MAX_CMD_PACKET_SIZE	  = 1024 * 16;
const int MAX_BIT_MSG_SIZE        = 255*8;  

/* define some length used in packet class */
const int BYTE_SIZE  = 1;
const int LONG_SIZE  = 4;
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

// look like Archive...
class CG_CmdPacket  
{
public:
	CG_CmdPacket();
	CG_CmdPacket(int byteBufLen,int bitBufLen);
	CG_CmdPacket(const CG_CmdPacket& packet);
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
	friend CG_CmdPacket& operator<<( CG_CmdPacket& Ar, std::string& str); 

	void Serialize(void *v,int size);
	void ByteOrderSerialize(void *v,int size);
	void ResetReadPos();

	bool SerializeShortEx(short &s);
	/*
	bool Serialize(std::string &str);
	bool Serialize(long *l);
	bool Serialize(char *c);
	bool Serialize(short *s);
	bool Serialize(float *f);
	*/
	bool ReadBinary(char **data,int *len);
	bool ReadString(char **str);
	bool ReadFloat(float *f);
	bool ReadLong(long *l);
	bool ReadShort(short *s);
	bool ReadByte(char *c);
	
	bool WriteBinary(char const*data,int len);
	bool WriteString(char const*str);
	bool WriteFloat(float f);
	bool WriteLong(long l);
	bool WriteShort(short s);
	bool WriteByte(char c);

	bool WritePacket(CG_CmdPacket* pPacket);

	bool ReadCmdOnly(short *s);
	
	char *GetByteData() { return m_pByteData; }
	int   GetByteDataSize() const
	{ 
		if(m_op==OP_READ)
			return m_maxByte;
		else
			return m_curByte; 
	}
	
	// bit msg
	char *GetBitData() { return m_pBitData; }
	int   GetBitDataSize() const
	{ 
		if(m_op==OP_READ)
			return m_maxBit;
		else
			return m_curBit; 
	}

	bool  SetSize(int byteBufLen,int bitBufLen=MAX_BIT_MSG_SIZE);
	bool  CloneFrom(CG_CmdPacket *packet);
	bool  CloneBitFrom(CG_CmdPacket *packet);
	int	  GetCurBytePos() { return m_curByte; };

	bool  IsReading() { return m_op == OP_READ; }
	bool  IsWritting() { return m_op == OP_WRITE; }
	bool  IsError() { return m_bError; }

	bool  SaveToBuffer(char *buf,int &size);
	bool  LoadFromBuffer(char *buf,int size);

	bool  ChangeString(char* szStr, int iDstBegin, size_t iOldStrLen);
	bool  ChangeShort(short i, int iDstBegin);
	bool  ChangeData(char *buf, size_t size, int iDstBegin, size_t iDstSize);
	
	bool  WriteData(void const*data,int len);
	bool  ReadData(void *data,int len);
private:
	bool  ReadDataOnly(void* data, int len);

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

// ÍøÂç¶ÁÈ¡ºê
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
			// LOG("read 1 bit\n");
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
			// LOG("write 1 bit\n");
		}
	}
}

#define SLZ_DELTA(v) ::SerializeDelta(cmd,v,old.v);
#endif
