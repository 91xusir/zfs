//
// g_tcpsession.cpp: implementation of the CG_TCPSession class.
//
#include "net/rt_net.h"

CG_CmdPacket  CG_TCPSession::m_sendCmdPkt(MAX_CMD_PACKET_SIZE,MAX_BIT_MSG_SIZE);
CG_CmdPacket  CG_TCPSession::m_recvCmdPkt(MAX_CMD_PACKET_SIZE,MAX_BIT_MSG_SIZE);
CG_NetPacket  CG_TCPSession::m_sendNetPkt;
CG_NetPacket  CG_TCPSession::m_recvNetPkt;

CG_TCPSession::CG_TCPSession()
{
	CM_MEMPROTECTOR(m_seed, MAX_NET_SEED_SIZE)
	CM_MEMPROTECTOR(m_debugStr, MAX_DEBUG_STR_LEN)

	// m_sendCmdPkt.SetSize(MAX_CMD_PACKET_SIZE,MAX_BIT_MSG_SIZE);
	// m_recvCmdPkt.SetSize(MAX_CMD_PACKET_SIZE,MAX_BIT_MSG_SIZE);
	m_bEncrypt  = false;
	m_bCompress = true;
	m_bRemoteCompress = true;
	m_readBuf = m_writeBuf = NULL;
	SetBufSize(3*MAX_BUFFER_SIZE/1024+1);
	rt2_strcpy(m_debugStr,NET_LIB);
	m_comDelay = 3;
	m_notify = NULL;
	m_client = NULL;
	
	Reset();
}

CG_TCPSession::CG_TCPSession(const CG_TCPSession &object)
{
	memcpy(this, &object, sizeof(CG_TCPSession));
	CM_MEMPROTECTOR(m_seed, MAX_NET_SEED_SIZE)
	CM_MEMPROTECTOR(m_debugStr, MAX_DEBUG_STR_LEN)
}

CG_TCPSession::~CG_TCPSession()
{
	CM_MEMUNPROTECTOR(m_seed)
	CM_MEMUNPROTECTOR(m_debugStr)

	DEL_ARRAY(m_readBuf);
	DEL_ARRAY(m_writeBuf);
}

void CG_TCPSession::OnConnect(bool ret)
{

}

void CG_TCPSession::OnDisconnect()
{

}

void CG_TCPSession::OnReceive(CG_CmdPacket *packet)
{

}

void CG_TCPSession::Process()
{
	DefProc();
	bool ret = false;
	
	switch (m_state)
	{
		case NET_STATE_CONNECTED:
			while(GetPacket(&m_recvCmdPkt))
			{
				OnReceive(&m_recvCmdPkt);
				if(m_notify)
					m_notify->OnReceive(m_client,&m_recvCmdPkt);
    		}
			break;

		case NET_STATE_DEAD:
			Close();
			OnDisconnect();
			if(m_notify)
				m_notify->OnDisconnect(m_client,0);
			break;

		case NET_STATE_WAITING_SEED:
			// check waiting seed timeout
			if(rtMilliseconds() - m_waitSeedTime > NET_CONNECT_TIMEOUT)
			{
				RtCoreLog().Info("%s: waiting seed timeout\n",m_debugStr);
				Close();
				OnConnect(false);
				if(m_notify)
					m_notify->OnConnect(m_client,false);
			}
			else
			{
				// try get encrypt seed 
				if(RecvPacket(&m_recvNetPkt))
				{
					m_recvNetPkt.AttachCmdPacket(&m_recvCmdPkt);
					if(m_recvNetPkt.IsSysPacket())
					{
						HandleSysPacket(&m_recvCmdPkt);
					}
				}
			}
			break;
			
		case NET_STATE_CONNECTING:
			if(m_socket.HasExcept())
			{
				RtCoreLog().Info("%s: connecting failed\n",m_debugStr);
				Close();
				OnConnect(false);
				if(m_notify)
					m_notify->OnConnect(m_client,false);
				break;
			}
			else if( rtMilliseconds() - m_connTime > NET_CONNECT_TIMEOUT)
			{
				RtCoreLog().Info("%s: connecting timeout\n",m_debugStr);
				Close();
				OnConnect(false);
				if(m_notify)
					m_notify->OnConnect(m_client,false);
				break;
			}

			if(m_socket.CanWrite()) 
			{
				ret = true;
			}
			else
			{
				break;
			}
#ifdef LINUX 
			// in linux should check again 
			int err,len;
			len = sizeof(err);
			if(getsockopt(m_socket.m_socket,SOL_SOCKET,SO_ERROR,(void *)&err,(socklen_t*)&len) != SOCKET_ERROR) 
			{
				if(err != 0) ret = false;
			}
			else
			{
				ret = false;
			}
#endif		
			if(ret)
			{
				m_state = NET_STATE_WAITING_SEED;
				m_startTime = rtMilliseconds();
			    m_lastSendTime = m_startTime;
				m_lastRecvTime = m_startTime;
				m_waitSeedTime = m_startTime;
			}
			else
			{
				Close();
				OnConnect(false);
				if(m_notify)
					m_notify->OnConnect(m_client,false);
			}
 			break;
	}
}

bool CG_TCPSession::Connect(const char *remote, int port, char *bindIp,int bindPort)
{
	if(!m_socket.Initialize(NETWORK_PROTOCOL_TCP))
		return false;

	m_socket.SetReuseAddr(true);
	if(!m_socket.BindAddr(bindIp,bindPort))
		return false;
		
	if(m_socket.Connect(remote,port))
	{
		m_state = NET_STATE_CONNECTING;
		m_connTime = rtMilliseconds();
		return true;
	}
	return false;
}

void CG_TCPSession::Close()
{
	Reset();
}

void CG_TCPSession::Reset()
{
	m_rtt = 0;

	DEL_ARRAY(m_readBuf);
	DEL_ARRAY(m_writeBuf);

	m_readBuf = RT_NEW char[MAX_BUFFER_SIZE];
	m_readBufSize = MAX_BUFFER_SIZE;

	m_writeBuf = RT_NEW char[MAX_BUFFER_SIZE];
	m_writeBufSize = MAX_BUFFER_SIZE;
	// m_maxBufSize   = MAX_BUFFER_SIZE;

	m_socket.Close();
	m_socket.Reset();
	
	m_seedLen = 0;
	m_sendSeq = 0;
	m_recvSeq = 0;
	m_sendBytes = 0;
	m_recvBytes = 0;
	m_rowSendBytes = 0;
	m_rowRecvBytes = 0;
	m_state = NET_STATE_ZERO;
	m_writeLen = m_writeBeg = m_writeEnd = 0;
	m_readLen = m_readPos = 0;
}

void CG_TCPSession::Attach(SOCKET socket)
{
	m_socket.Attach(socket);
	m_state = NET_STATE_CONNECTED;
	m_startTime = rtMilliseconds();
	m_lastRecvTime = m_startTime;
	m_lastSendTime = m_startTime;
}

//#include "profile.h"

bool CG_TCPSession::SendPacket(CG_CmdPacket *packet,bool flush,bool sys)
{
	//PROFILE("CG_TCPSession_SendPacket");
	if (m_state != NET_STATE_CONNECTED) 
		return false;

	m_sendNetPkt.AddCmdPacket(packet);
	m_sendNetPkt.SetSeq(m_sendSeq);
	
	const int row = m_sendNetPkt.GetTotalSize();

	// dont's encrypt sys packet 
	if(sys)
	{
		m_sendNetPkt.SetSysPacket();
	}
	else
	{
		// check if need encrypt and compress 
		if(m_bCompress) m_sendNetPkt.Compress();
		if(m_bEncrypt) m_sendNetPkt.Encrypt(m_seed,m_seedLen);
	}

	// add to send buffer 
	// if push data failed,don't set state to NET_STATE_DEAD
	// the app layer decide if should close net,not net layer 
	if(!PushData(m_sendNetPkt.GetBuffer(),m_sendNetPkt.GetTotalSize())) 
	{
		// new added,need test
		// ...
		// m_state = NET_STATE_DEAD;
		return false;
	}

	m_rowSendBytes += row;
	// compute next seq number 
	m_sendSeq++;
	if(m_sendSeq > MAX_NET_PACKET_SEQ) m_sendSeq = 1;

	// flush data 
	if(flush) FlushData();
	return true;
}

/*
#define SEND_PACKET(packet,flush,sys,ret)\
do\
{\
	if(packet->m_state != NET_STATE_CONNECTED)\
	{\
		ret = false;\
		break;\
	}\
	packet->m_sendNetPkt.AddCmdPacket(packet);\
	packet->m_sendNetPkt.SetSeq(packet->m_sendSeq);\
	int row;\
	row = packet->m_sendNetPkt.GetTotalSize();\
	if(sys)\
	{\
		packet->m_sendNetPkt.SetSysPacket();\
	}\
	else\
	{\
		if(packet->m_bCompress) packet->m_sendNetPkt.Compress();\
		if(packet->m_bEncrypt) packet->m_sendNetPkt.Encrypt(m_seed,m_seedLen);\
	}\
	if(!packet->PushData(packet->m_sendNetPkt.GetBuffer(),packet->m_sendNetPkt.GetTotalSize()))\
	{\
		ret = false;\
		break;\
	}\
	packet->m_rowSendBytes += row;\
	packet->m_sendSeq++;\
	if(packet->m_sendSeq > MAX_NET_PACKET_SEQ) packet->m_sendSeq = 1;\
	if(flush) packet->FlushData();\
	ret = true;\
}while(0);
*/

int CG_TCPSession::GetAvgFluxIn()
{
	int time = (rtMilliseconds() - m_startTime)/1000;
	if(time == 0) time = 1;
	return GetBytesIn()/time;
}

int CG_TCPSession::GetAvgFluxOut()
{
	int time = (rtMilliseconds() - m_startTime)/1000;
	if(time == 0) time = 1;
	return (GetBytesOut()/time);
}

int CG_TCPSession::GetBytesIn()
{
	return m_recvBytes;
}

int CG_TCPSession::GetBytesOut()
{
	return m_sendBytes;
}

bool CG_TCPSession::GetPacket(CG_CmdPacket *packet)
{
	while(RecvPacket(&m_recvNetPkt))
	{
		// attach a cmd packet for read 
		m_recvNetPkt.AttachCmdPacket(packet);
		if(m_recvNetPkt.IsSysPacket())
		{
			// if a system msg packet,handle it 
			HandleSysPacket(packet);
		    continue;
		}
		else
		{
			// get a user data packet,return 
			return true;
		}
	}
	return false;
}

bool CG_TCPSession::ReadData()
{
	if (m_readLen == 0)	
	{
		m_readPos = 0;
	}
	else if (m_readLen > 0 && m_readPos != 0)
	{
		memmove(m_readBuf,m_readBuf+m_readPos,m_readLen);
		m_readPos = 0;
	}
	
	// m_readPos always = 0 
	int ret;
	ret = m_socket.Recv(m_readBuf+m_readLen,m_readBufSize-m_readLen);
	if (ret > 0)
	{
		m_readLen += ret;
		m_recvBytes += ret;
		// reset recv time 
		m_lastRecvTime = rtMilliseconds();
		return true;
	}
	else if(ret == 0)
	{
		return false;
	}
	else
	{
		m_state = NET_STATE_DEAD;
	}
	return false;
}

void CG_TCPSession::FlushData()
{
	//PROFILE("CG_TCPSession_FlushData");
	// flush data in buffer 
	int ret;
	if (m_writeLen == 0) return;

	// don't reset last send time,so the keep alive packet can be send 
	// m_lastSendTime = rtMilliseconds();
	if (m_writeEnd > m_writeBeg)
	{
		ret = m_socket.Send(m_writeBuf+m_writeBeg,m_writeLen);
		if (ret >= 0)
		{
			m_sendBytes += ret;
			m_writeLen -= ret;
			m_writeBeg = (m_writeBeg+ret)%m_writeBufSize;
		}
		else if (ret == -1)
		{
			m_state = NET_STATE_DEAD;
			return;
		}
	}
	else  // end < begin 
	{
		int reqLen = m_writeBufSize-m_writeBeg;
		ret = m_socket.Send(m_writeBuf+m_writeBeg,reqLen);
		if (ret >= 0)
		{
			m_sendBytes += ret;
			m_writeLen -= ret;
			m_writeBeg = (m_writeBeg+ret)%m_writeBufSize;
			if(m_writeBeg!=0) return;
		}
		else if (ret == -1)
		{
			m_state = NET_STATE_DEAD;
			return;
		}
		if(ret != reqLen)
		{
			RtCoreLog().Error("%s: flush data not complete,ret=%d,req=%d\n",m_debugStr,ret,reqLen);
			return;
		}
		
		ret = m_socket.Send(m_writeBuf,m_writeEnd);
		if (ret >= 0)
		{
			m_sendBytes += ret;
			m_writeLen -= ret;
			m_writeBeg = (m_writeBeg+ret)%m_writeBufSize;
		}
		else if (ret == -1)
		{
			m_state = NET_STATE_DEAD;
			return;
		}
	}
}

bool CG_TCPSession::PushData(char *buf, int len)
{
	//PROFILE("CG_TCPSession_PushData");

	int tlen = len;
	
	// first check if can hold data 
	if(len >= m_writeBufSize - m_writeLen) 
	{
		if (!ResizeBuf(m_writeLen + len +1)) 
		{
			return false;
		}
	}
	m_writeLen+=len;
	while(len--)
	{
		m_writeBuf[m_writeEnd++] = *buf++;
		if (m_writeEnd == m_writeBufSize)
			m_writeEnd = 0;
	}
	return true;
}

void CG_TCPSession::SetBufSize(int kb)
{
	m_maxBufSize = kb*1000;
}

bool CG_TCPSession::ResizeBuf(int min)
{
	int newSize = min*2;
	if (min >= m_maxBufSize) return false;
	if (newSize>m_maxBufSize)
		newSize = m_maxBufSize;
	RtCoreLog().Error("%s: Resize socket buf to %d\n",m_debugStr,newSize);
	char *buf = RT_NEW char[newSize];

	// copy src data 
	if (m_writeLen > 0)
	{
		if(m_writeEnd > m_writeBeg)
			memcpy(buf,m_writeBuf+m_writeBeg,m_writeLen);
		else
		{
			int nLen = m_writeBufSize - m_writeBeg;
			memcpy(buf,m_writeBuf+m_writeBeg,nLen);
			memcpy(buf+nLen,m_writeBuf,m_writeLen - nLen);
		}
	}
	DEL_ARRAY(m_writeBuf);
	m_writeBuf = buf;
	m_writeBeg = 0;
	m_writeEnd = m_writeLen;
	m_writeBufSize = newSize;
	return true;
}

int CG_TCPSession::GetState()
{
	return m_state;
}

bool CG_TCPSession::IsDead()
{
	return (m_state == NET_STATE_DEAD);
}

void CG_TCPSession::DefProc()
{
	if(m_state != NET_STATE_CONNECTED && m_state != NET_STATE_WAITING_SEED) return;
	
	if(m_state == NET_STATE_CONNECTED)
	{
		unsigned long now = rtMilliseconds();
		if(now - m_lastSendTime > NET_KEEP_ALIVE_INTERVAL)
		{
			KeepAlive(now);
			m_lastSendTime = now;
		}
		if(now - m_lastRecvTime > NET_TIMEOUT)
		{
			if (now < m_lastRecvTime)
			{
				//有时会出现now < m_lastRecvTime的情况，原因暂不明，先这样处理之
				RtCoreLog().Error("%s: Tcp session dead reason = timeout now = %d m_lastRecvTime = %d, m_startTime = %d\n",m_debugStr, now, m_lastRecvTime, m_startTime);
			}else
			{
				m_state = NET_STATE_DEAD;
				return;
			}
		}
		FlushData();
	}
	ReadData();
}

bool CG_TCPSession::RecvPacket(CG_NetPacket *packet)
{
	int ret;
	ret = packet->BeginRead(m_readBuf+m_readPos,m_readLen);
	if(ret > 0)
	{
		m_readLen -= ret;
		m_readPos += ret;
		if(packet->IsEncrypt())  
		{
			packet->Decrypt(m_seed,m_seedLen);
		}
		if(m_recvSeq != packet->GetSeq())
		{
			RtCoreLog().Error("%s: seq error packet seq = %d,expect seq = %d,close session\n",
				m_debugStr,packet->GetSeq(),m_recvSeq); 
			m_state = NET_STATE_DEAD;
			return false;
		}
		m_recvSeq++;
		if(m_recvSeq > MAX_NET_PACKET_SEQ) m_recvSeq = 1;
		
		if(packet->IsCompress()) 
		{
			if(!m_bRemoteCompress) 
			{
#ifdef _DEBUG
				RtCoreLog().Info("%s: remote compress not allowed,ignore\n",m_debugStr);
#endif
				m_rowRecvBytes += packet->GetTotalSize();
				return false;
			}
			if(!packet->Uncompress()) return false;
		}
		m_rowRecvBytes += packet->GetTotalSize();
		return true;
	}
	else if(ret == 0)
	{
		return false;
	}
	else 
	{
		// the packet flag error,not belong to this session 
		RtCoreLog().Error("%s: (RecvPacket) begin read packet failed (%d)\n",m_debugStr,ret); 
		m_state = NET_STATE_DEAD;
		return false;
	}
	return false;
}

int CG_TCPSession::HandleSysPacket(CG_CmdPacket *packet)
{
	short type;
	long  param;

	if(!packet->ReadShort(&type)) return NULL;
	switch(type)
	{
		case SYS_PACKET_KEEP_ALIVE:
			if(!packet->ReadLong(&param)) return NULL;
			// Sys_Log("recv keep alive msg");
			m_sendCmdPkt.BeginWrite();
			m_sendCmdPkt.WriteShort(SYS_PACKET_ALIVE_ACK);
			m_sendCmdPkt.WriteLong(param);
			SendPacket(&m_sendCmdPkt,true,true);
			break;

		case SYS_PACKET_ALIVE_ACK:
			if(!packet->ReadLong(&param)) return NULL;
			// Sys_Log("recv alive ack msg");
			m_rtt = rtMilliseconds() - param;
			break;

		case SYS_PACKET_SET_SEED:
			if(!packet->ReadLong(&param)) return NULL;
			SetSeed(param);
			m_state = NET_STATE_CONNECTED;
			OnConnect(true);
			if(m_notify)
				m_notify->OnConnect(m_client,true);
	    	break;

		case SYS_PACKET_COM:
			m_comDelay--;
			if(m_comDelay == 0) m_bRemoteCompress = true;
			break;

		default :
			return NULL;
	}
	return type;
}

void CG_TCPSession::SetSeed(long seed)
{
	// Sys_Log("net: set seed = %d",seed);
	if(seed == 0)
	{
		m_bEncrypt = false;
		return;
	}

	int i;
	for(i=0;i<MAX_NET_SEED_SIZE;i++)
	{
		m_seed[i] = (seed % 255)+1;
		seed /= 19;
	}
	m_seedLen = i;
	m_bEncrypt = true;
}

void CG_TCPSession::SendSeed(long seed)
{
	m_sendCmdPkt.BeginWrite();
	m_sendCmdPkt.WriteShort(SYS_PACKET_SET_SEED);
	m_sendCmdPkt.WriteLong(seed);
	SendPacket(&m_sendCmdPkt,true,true);
	SetSeed(seed);
}

void CG_TCPSession::KeepAlive(long now)
{
	m_sendCmdPkt.BeginWrite();
	m_sendCmdPkt.WriteShort(SYS_PACKET_KEEP_ALIVE);
	m_sendCmdPkt.WriteLong(now);
	SendPacket(&m_sendCmdPkt,true,true);
}

int CG_TCPSession::GetRtt()
{
	return m_rtt;
}

void CG_TCPSession::SetDebugStr(const char *str)
{
	//snprintf(m_debugStr, MAX_DEBUG_STR_LEN-1, "%s(%s)", m_debugStr,str);
	rt2_snprintf(m_debugStr, MAX_DEBUG_STR_LEN-1, "%s(%s)", m_debugStr,str);
	m_debugStr[MAX_DEBUG_STR_LEN-1] = '\0';
	m_socket.SetDebugStr(str);
}

/*
void CG_TCPSession::Replace(CG_TCPSession *old)
{
	Attach(old->GetSocket().m_socket);
	old->GetSocket().m_socket = INVALID_SOCKET;

	m_sendSeq = old->m_sendSeq;
	m_recvSeq = old->m_recvSeq;
}
*/

//
// new network interface
//

// ------------------------------------------------------------------------------
// tcp client
// ------------------------------------------------------------------------------
CG_NetClientTcp::CG_NetClientTcp()
{
	m_notify = NULL;
	m_session = NULL;
	m_reConnTime = 0;
	m_closeDelay = 0;
}

CG_NetClientTcp::~CG_NetClientTcp()
{

}

bool CG_NetClientTcp::Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify)
{
	m_session = RT_NEW CG_TCPSession;
	m_notify = notify;
	if(bindIp)
        m_bindIp = bindIp;
	m_bindPort = bindPort;
	m_session->SetNotify(this,m_notify);
	return true;
}

bool CG_NetClientTcp::Connect(const char *remoteIp, int remotePort)
{
	m_remoteIp = remoteIp;
	m_remotePort = remotePort;
	return m_session->Connect((char*)remoteIp,remotePort);
}

void CG_NetClientTcp::ReConnect(DWORD delay)
{
	m_reConnTime = rtMilliseconds() + delay;
}

void CG_NetClientTcp::Process()
{
	m_session->Process();
}

void CG_NetClientTcp::Close(DWORD delay)
{
	if(delay == 0)
	{
		m_session->Close();
		DEL_ONE(m_session);
		m_session = NULL;
	}
	else
	{
		m_closeDelay = delay + rtMilliseconds();
	}
}

bool CG_NetClientTcp::IsClosing()
{
	return m_closeDelay != 0;
}

bool CG_NetClientTcp::CanClose()
{
	if(m_closeDelay == 0) return false;
	return m_closeDelay < rtMilliseconds();
}

void CG_NetClientTcp::SendPacket(CG_CmdPacket *packet)
{
	m_session->SendPacket(packet);
}

// ------------------------------------------------------------------------------
// tcp server
// ------------------------------------------------------------------------------
CG_NetServerTcp::CG_NetServerTcp()
{
	m_notify = NULL;
	m_listener = NULL;
}

CG_NetServerTcp::~CG_NetServerTcp()
{

}

bool CG_NetServerTcp::Init(const char *bindIp, int bindPort, bool encrypt, CG_NetServerNotify *notify)
{
	m_notify = notify;
	m_listener = RT_NEW CG_TCPListener;
	return m_listener->StartListen(bindIp,bindPort,encrypt);
}

void CG_NetServerTcp::Close()
{
	std::list<CG_NetClientTcp*>::iterator it;
	for(it=m_clientList.begin(); it!=m_clientList.end(); it++)
	{
		CG_NetClientTcp *tcp = *it;
		tcp->Close(0);
		DEL_ONE(tcp);
	}

	m_listener->Close();
	DEL_ONE(m_listener);
}

void CG_NetServerTcp::Process()
{
	// try accept new client
	CG_TCPSession *tmp = NULL;
	int cnt = 0;
	while(cnt++ < 20)
	{
		tmp = m_listener->AcceptNewSession();
		if(!tmp) break;

		CG_NetClientTcp *tcp = RT_NEW CG_NetClientTcp;
		tcp->m_session = tmp;
		m_clientList.push_back(tcp);
		m_notify->OnAccept(this,tcp);
	}

	// process current client
	std::list<CG_NetClientTcp*>::iterator it;
	for(it=m_clientList.begin(); it!=m_clientList.end();)
	{
		CG_NetClientTcp *tcp = *it;
		if(tcp->CanClose())
		{
			tcp->Close(0);
			DEL_ONE(tcp);
			it = m_clientList.erase(it);
			continue;
		}

		tcp->m_session->DefProc();
		if(tcp->IsClosing())
		{
			it++;
			continue;
		}

		if(tcp->m_session->IsDead())
		{
			m_notify->OnDisconnect(this,tcp,0);
			tcp->Close(0);
			DEL_ONE(tcp);
			it = m_clientList.erase(it);
			continue;
		}

		while(tcp->m_session->GetPacket(&m_packet))
		{
			m_notify->OnReceive(this,tcp,&m_packet);
		}
		it++;
	}
}

//
// net device
//
CG_NetDevice *CG_NetDevice::m_device;
CG_NetDevice::CG_NetDevice()
{
	m_device = NULL;
}

CG_NetDevice::~CG_NetDevice()
{

}

CG_NetDevice *CG_NetDevice::GetInstance()
{
	if(m_device) 
		return m_device;
	
	m_device = RT_NEW CG_NetDevice;
	return m_device;
}

bool CG_NetDevice::Init()
{
	return true;
}

void CG_NetDevice::Tick(long delta)
{

}

void CG_NetDevice::Exit()
{

}

SNetLayerInfo *CG_NetDevice::GetNetLayerInfo()
{
	return &CG_NetSocket::m_globalNetInfo;
}

CG_NetClient *CG_NetDevice::CreateClient(int protocal)
{
	if(protocal == NETWORK_PROTOCOL_TCP)
		return RT_NEW CG_NetClientTcp;
	else if(protocal == NETWORK_PROTOCOL_UDP)
		return NULL;
	else
		return NULL;
}

CG_NetServer *CG_NetDevice::CreateServer(int protocal)
{
	if(protocal == NETWORK_PROTOCOL_TCP)
		return RT_NEW CG_NetServerTcp;
	else if(protocal == NETWORK_PROTOCOL_UDP)
		return NULL;
	else
		return NULL;
}

void CG_NetDevice::DestroyClient(CG_NetClient *client)
{
	DEL_ONE(client);
}

void CG_NetDevice::DestroyServer(CG_NetServer *server)
{
	DEL_ONE(server);
}
