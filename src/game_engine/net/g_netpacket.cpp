//
// g_NetPacket.cpp: implementation of the CG_NetPacket class.
//
#include "net/rt_net.h"
//#include "profile.h"

CG_NetPacket::CG_NetPacket()
{
	CM_MEMPROTECTOR(m_buffer, MAX_NET_PACKET_SIZE)
	CM_MEMPROTECTOR(m_comBuf, COMPRESS_BUF_SIZE)

	m_pktFlag			= &m_buffer[0];
	m_pktSeq			= (unsigned short*)&m_buffer[3+2];
	m_pktBitDataSize	= (unsigned short* )&m_buffer[1];
	m_pktDataSize		= (unsigned short*)&m_buffer[1+2];
}

CG_NetPacket::CG_NetPacket(const CG_NetPacket &object)
{
	memcpy(this, &object, sizeof(CG_NetPacket));
	CM_MEMPROTECTOR(m_buffer, MAX_NET_PACKET_SIZE)
	CM_MEMPROTECTOR(m_comBuf, COMPRESS_BUF_SIZE)
}

CG_NetPacket::~CG_NetPacket()
{
	CM_MEMUNPROTECTOR(m_buffer)
	CM_MEMUNPROTECTOR(m_comBuf)
}

void CG_NetPacket::AddCmdPacket(CG_CmdPacket *packet)
{
//	PROFILE("CG_NetPacket_AddCmdPacket");
	unsigned short dataSize = packet->GetByteDataSize();
	unsigned short bitDataSize = packet->GetBitDataSize();
	*m_pktFlag = PROTOCOL_FLAG<<PROTOCAL_FLAG_OFFSET;
	*m_pktDataSize = dataSize + BitToByte(bitDataSize);
	*m_pktBitDataSize = bitDataSize;

	if(bitDataSize > 0)
		memcpy(m_buffer+NET_PACKET_HEAD_SIZE,packet->GetBitData(),BitToByte(bitDataSize));

	memcpy(m_buffer+NET_PACKET_HEAD_SIZE+BitToByte(bitDataSize),packet->GetByteData(),dataSize);
	m_totalSize = NET_PACKET_HEAD_SIZE + dataSize + BitToByte(bitDataSize);
}

void CG_NetPacket::AttachCmdPacket(CG_CmdPacket *packet)
{
	packet->BeginRead(m_buffer+NET_PACKET_HEAD_SIZE+BitToByte(*m_pktBitDataSize),*m_pktDataSize-BitToByte(*m_pktBitDataSize),
		m_buffer+NET_PACKET_HEAD_SIZE,*m_pktBitDataSize);
}

// -1	Error
//  0	Process Next Time
// >0	Len
int CG_NetPacket::BeginRead(char *p,int len)
{
	if (len < NET_PACKET_HEAD_SIZE) return 0;
	m_pktFlag = &p[0];
	m_pktSeq  = (unsigned short*)&p[3+2];
	m_pktDataSize = (unsigned short*)&p[1+2];
	m_pktBitDataSize = (unsigned short*)&p[1];
	
	if (*m_pktFlag>>PROTOCAL_FLAG_OFFSET != PROTOCOL_FLAG)
	{
		RtCoreLog().Error("%s: net packet flag error\n",NET_LIB);
		return -1;
	}

	if (*m_pktBitDataSize > MAX_BIT_MSG_SIZE) 
	{
		RtCoreLog().Info("%s: net packet bit size error (%d)\n",NET_LIB,*m_pktBitDataSize);
		return -1;
	}

	if (*m_pktDataSize <= 0 || *m_pktDataSize > MAX_CMD_PACKET_SIZE + BitToByte(MAX_BIT_MSG_SIZE)) 
	{
		RtCoreLog().Info("%s: net packet size error (%d)\n",NET_LIB,*m_pktDataSize);
		return -1;
	}

	int total = *m_pktDataSize + NET_PACKET_HEAD_SIZE;

	if (len < total) 
	{
		// Sys_Log("NetPacket Error 2: len = %d,s = %d",len,s);
		return 0;
	}
	// copy to self buffer 
	memcpy(m_buffer,p,total);
	m_pktFlag = &m_buffer[0];
	m_pktSeq  = (unsigned short*)&m_buffer[3+2];
	m_pktDataSize = (unsigned short*)&m_buffer[1+2];
	m_pktBitDataSize = (unsigned short*)&m_buffer[1];
	m_totalSize = total;
	return total;
}

void CG_NetPacket::Encrypt(char *seed,int len)
{
//	PROFILE("CG_NetPacket_Encrypt");
	// encrypt here 
	int i,k;
	for(i=NET_PACKET_ENCRYPT_OFFSET,k=0;i<m_totalSize;i++,k++)
	{
		if(k==len) k=0;
		m_buffer[i]^=seed[k];
	}
	*m_pktFlag |= FLAG_ENCRYPT;
}

void CG_NetPacket::Decrypt(char *seed,int len)
{
	// decrypt here 
	int i,k;
	for(i=NET_PACKET_ENCRYPT_OFFSET,k=0;i<m_totalSize;i++,k++)
	{
		if(k==len) k=0;
		m_buffer[i]^=seed[k];
	}
	*m_pktFlag &= (~FLAG_ENCRYPT);
}

bool CG_NetPacket::IsEncrypt()
{
	return (*m_pktFlag & FLAG_ENCRYPT);
}

bool CG_NetPacket::Compress()
{
//	PROFILE("CG_NetPacket_Compress");
	// compress here 
	unsigned long destLen = COMPRESS_BUF_SIZE;
	destLen = RtZCompress::Compress(m_buffer+NET_PACKET_HEAD_SIZE,*m_pktDataSize,m_comBuf);
	if(destLen >= *m_pktDataSize) 
	{
		return false;
	}
	// Sys_Log("compress src = %d, dest = %d",*m_pktDataSize,destLen);
	memcpy(m_buffer+NET_PACKET_HEAD_SIZE,m_comBuf,destLen);
	*m_pktDataSize = (short)destLen;
	m_totalSize = destLen+NET_PACKET_HEAD_SIZE;
	
	*m_pktFlag |= FLAG_COMPRESS;
	return true;
}

bool CG_NetPacket::Uncompress()
{
	// uncompress here 
	unsigned long destLen = COMPRESS_BUF_SIZE;
	destLen = RtZCompress::Decompression(m_buffer+NET_PACKET_HEAD_SIZE,*m_pktDataSize,m_comBuf);
	if(destLen >= MAX_NET_PACKET_SIZE || destLen <= 0)
	{
		return false;
	}
	memcpy(m_buffer+NET_PACKET_HEAD_SIZE,m_comBuf,destLen);
	*m_pktDataSize = (short)destLen;
	m_totalSize = destLen + NET_PACKET_HEAD_SIZE;

	*m_pktFlag &= (~FLAG_COMPRESS);
	return true;
}

bool CG_NetPacket::IsCompress()
{
	if(*m_pktFlag & FLAG_COMPRESS) return true;
	return false;
}

void CG_NetPacket::SetSysPacket()
{
	*m_pktFlag |= FLAG_SYSTEM;
}

bool CG_NetPacket::IsSysPacket()
{
	if(*m_pktFlag & FLAG_SYSTEM) return true;
	return false;
}


