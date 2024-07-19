//
// g_NetPacket.h: interface for the CG_NetPacket class.
//
#ifndef __CG_NET_PACKET_H__
#define __CG_NET_PACKET_H__

const int NET_PACKET_HEAD_SIZE		 = 5 + 2;
const int NET_PACKET_ENCRYPT_OFFSET  = 3 + 2;
const int MAX_NET_PACKET_SIZE   	 = MAX_CMD_PACKET_SIZE + NET_PACKET_HEAD_SIZE + MAX_BIT_MSG_SIZE/8; 

/*
net packet head struct
{
	3  bit	010
	1  bit  udp
    1  bit  reliable
	1  bit  system 
	1  bit	compress 
	1  bit	encrypt
	16 bit  bit msg size
	16 bit  byte msg size
	16 bit  seq
}
*/
 
const int  PROTOCAL_FLAG_OFFSET = 5;
const char PROTOCOL_FLAG		= 2; // 010
const char FLAG_ENCRYPT			= 1;
const char FLAG_COMPRESS		= 2;
const char FLAG_SYSTEM			= 4;
const char FLAG_RELIABLE		= 8;
const char FLAG_UDP				= 16;

/* system net packet type define */
const short SYS_PACKET_SET_CONNECT_INFO = 1;
const short SYS_PACKET_KEEP_ALIVE		= 2;
const short SYS_PACKET_ALIVE_ACK		= 3;
const short SYS_PACKET_COM				= 104;

const unsigned short	MAX_NET_PACKET_SEQ	= 65000;
const int				COMPRESS_BUF_SIZE	= MAX_NET_PACKET_SIZE*3/2;

class CG_NetPacket  
{
public:
	CG_NetPacket();
	virtual ~CG_NetPacket();

	bool 	Uncompress();
	bool 	Compress();
	bool 	IsCompress();
	bool 	IsEncrypt();
	bool 	IsSysPacket();
	void 	SetSysPacket();
	void 	Decrypt(char *seed,int len);
	void 	Encrypt(char *seed,int len);
	void 	AddCmdPacket(CG_CmdPacket *packet);
	void 	AttachCmdPacket(CG_CmdPacket *packet);
	void 	SetSeq(unsigned short seq) { *m_pktSeq = seq; }

	int  	BeginRead(char *p,int len);
	int  	GetSeq()		{ return *m_pktSeq; }
	int  	GetHeadSize()  { return NET_PACKET_HEAD_SIZE; }
	int  	GetTotalSize() { return m_totalSize; }
	char   *GetBuffer()	{ return m_buffer; }

	//char			 m_buffer[MAX_NET_PACKET_SIZE];
	CM_MEMDEF(m_buffer, MAX_NET_PACKET_SIZE)
	int				 m_totalSize;
	char			*m_pktFlag;
	unsigned short  *m_pktSeq;
	unsigned short  *m_pktDataSize;
	unsigned short	*m_pktBitDataSize;
	//char			 m_comBuf[COMPRESS_BUF_SIZE];
	CM_MEMDEF(m_comBuf, COMPRESS_BUF_SIZE)
};

#endif
