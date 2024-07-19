//
// g_udpsession.cpp: implementation of the CG_UdpSession class.
//
#include "rt_net_imp.h"

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
	m_dev->GetSendNetPacket()->AddCmdPacket(cmd);
	// m_dev->GetSendNetPacket()->Compress();
	
	int ret;
	ret = m_socket.SendTo(m_dev->GetSendNetPacket()->m_buffer,m_dev->GetSendNetPacket()->m_totalSize,ToAddr->GetAddr());
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
		if(m_dev->GetRecvNetPacket()->BeginRead(m_buffer,ret) <= 0)
			return false;
        if(m_dev->GetRecvNetPacket()->IsCompress())
		{
			return false;
			// m_dev->GetRecvNetPacket()->Uncompress();
		}
		m_dev->GetRecvNetPacket()->AttachCmdPacket(cmd);
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

