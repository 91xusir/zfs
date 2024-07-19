//
// g_udpsession.cpp: implementation of the CG_UdpSession class.
//
#include "net/rt_net.h"
CG_CmdPacket  CG_UDPSession::m_sendCmdPkt;
CG_CmdPacket  CG_UDPSession::m_recvCmdPkt;
CG_NetPacket  CG_UDPSession::m_sendNetPkt;
CG_NetPacket  CG_UDPSession::m_recvNetPkt;
//char CG_UDPSession::m_buffer[MAX_NET_PACKET_SIZE];
G_MEMDEF(CG_UDPSession::m_buffer, MAX_NET_PACKET_SIZE)

CG_UDPSession::CG_UDPSession() 
{
	S_MEMPROTECTOR(m_buffer, MAX_NET_PACKET_SIZE, bMP)
}

CG_UDPSession::~CG_UDPSession()
{

}

bool CG_UDPSession::Create(const char *ip,short port)
{
	m_socket.Initialize(NETWORK_PROTOCOL_UDP);
	m_socket.SetReuseAddr(true);
	return m_socket.BindAddr(ip,port);
}

bool CG_UDPSession::SendTo(CG_CmdPacket *cmd,CG_NetAddress *ToAddr)
{
	m_sendNetPkt.AddCmdPacket(cmd);
	// m_sendNetPkt.Compress();
	
	int ret;
	ret = m_socket.SendTo(m_sendNetPkt.m_buffer,m_sendNetPkt.m_totalSize,ToAddr->GetAddr());
	if(ret > 0) 
		return true;
	else
		return false;
}

bool CG_UDPSession::RecvFrom(CG_CmdPacket *cmd,CG_NetAddress *fromAddr)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);
	int ret;
	ret = m_socket.RecvFrom(m_buffer,MAX_NET_PACKET_SIZE,&addr,&addrLen);
	if(ret > 0)
	{
		if(m_recvNetPkt.BeginRead(m_buffer,ret) <= 0)
			return false;
        if(m_recvNetPkt.IsCompress())
		{
			return false;
			// m_recvNetPkt.Uncompress();
		}
		m_recvNetPkt.AttachCmdPacket(cmd);
		fromAddr->SetAddr(&addr,addrLen);
		return true;
	}
	else
	{
		return false;
	}
}

void CG_UDPSession::Close()
{
	m_socket.Close();
}

RtKeyValue::RtKeyValue() 
{ 
	m_dict = NULL;
}

RtKeyValue::~RtKeyValue() 
{ 
	if(m_dict) DEL_ONE(m_dict);
}

void RtKeyValue::SetDict(RtDict *dict) 
{
	if(m_dict) DEL_ONE(m_dict);
	m_dict = RT_NEW RtDict;
	*m_dict = *dict;
}

bool RtKeyValue::operator==(const RtKeyValue &kv) 
{
	if(m_kt != kv.m_kt) return false;
	switch(m_kt) 
	{
		case KT_NUMBER: return Abs(m_number-kv.m_number) < 0.01;
		case KT_STRING: return m_string == kv.m_string;
		case KT_DICT:   return *m_dict == *kv.m_dict;
		case KT_BLOCK:  return m_block == kv.m_block;
		default:        return false;
	}
}

bool RtDict::GetValue(const char *key, RtKeyValue &value)
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

void RtDict::SetValue(const char *key, RtKeyValue &value)
{
	m_map[key] = value;
}

/*
RtKeyValue& RtDict::operator[](const char *key)
{
	EXT_SPACE::unordered_map<std::string,RtKeyValue>::iterator it;
	it = m_map.find(key);
	if(it != m_map.end())
	{
		return ((*it).second);
	}
	else
	{
		m_map[key] = "";
		it = m_map.find(key);
		return ((*it).second);
	}
}
*/
