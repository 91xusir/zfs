//
// g_tcpsession.cpp: implementation of the CG_NetClientTcp class.
//
#include "rt_net_imp.h"
#include "core/rt2_core.h"

CG_NetClientTcp::CG_NetClientTcp()
{
	CM_MEMPROTECTOR(m_seed, MAX_NET_SEED_SIZE)
	CM_MEMPROTECTOR(m_debugStr, MAX_DEBUG_STR_LEN)

	m_bEncrypt  = true;
	m_bCompress = true;
	m_bRemoteCompress = true;
	m_readBuf  = NULL; 
	m_writeBuf = NULL;
	SetBufferSize(1.5*MAX_BUFFER_SIZE/1024,0);
	rt2_strcpy(m_debugStr,NET_LIB);
	m_comDelay = 3;
	
	m_notify = NULL;
	m_reConnTime = 0;
	m_closeDelay = 0;
	m_dev = NULL;
	m_bDebugInfo = true;

	Reset();
}

CG_NetClientTcp::~CG_NetClientTcp()
{
	CM_MEMUNPROTECTOR(m_seed)
	CM_MEMUNPROTECTOR(m_debugStr)
	/*if( m_notify )
	{
		delete m_notify;
		m_notify = NULL;
	}*/
	//DEL_ONE( m_notify );
	//if(m_readBuf) delete []m_readBuf;  m_readBuf = NULL;
	//if(m_writeBuf)delete []m_writeBuf; m_writeBuf= NULL; 
	DEL_ARRAY( m_readBuf ); 
	DEL_ARRAY( m_writeBuf ); 
}

// -------------------------------------------------------------------------------------
// CG_NetClient interface begin
// -------------------------------------------------------------------------------------
bool CG_NetClientTcp::Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify)
{
	if(bindIp)
		m_bindIp = bindIp;
	m_bindPort = bindPort;
	/*if( m_notify )
	{
		delete m_notify;
		m_notify = NULL;
	}*/
	DEL_ONE( m_notify );
	m_notify = notify;

	if(!m_socket.Initialize(NETWORK_PROTOCOL_TCP))
		return false;

	m_socket.SetReuseAddr(true);
	if(!m_socket.BindAddr(bindIp,bindPort))
		return false;

	return true;
}

bool CG_NetClientTcp::Connect(const char *remoteIp, int remotePort)
{
	if(remoteIp)
		m_remoteIp = remoteIp;
	m_remotePort = remotePort;
	
	if(!m_socket.Connect(remoteIp,remotePort))
		return false;

	m_state = NET_STATE_CONNECTING;
	m_connTime = rtMilliseconds();
	return true;
}

void CG_NetClientTcp::ReConnect(DWORD delay)
{
	m_state = NET_STATE_RECONNECT;
	m_reConnTime = rtMilliseconds() + delay;
}

void CG_NetClientTcp::Destroy()
{
	
}

void CG_NetClientTcp::Close(DWORD delay)
{
	m_closeDelay = delay + rtMilliseconds();
}

void CG_NetClientTcp::GetNetInfo(SNetInfo &info)
{
	info.m_rtt           = GetRtt();
	info.m_sendPacketNum = m_sendPktNum;
	info.m_recvPacketNum = m_recvPktNum;
	info.m_totalByteSend = GetTotleBytesOut();
	info.m_totalByteRecv = GetTotleBytesIn();
	info.m_sendCompressRate = GetSendCompressRate();
	info.m_recvCompressRate = GetRecvCompressRate();
	info.m_secByteSend = GetAvgFluxOut(); 
	info.m_secByteRecv = GetAvgFluxIn();
	info.m_lastSecByteSend = m_lastSecSendBytes;
	info.m_lastSecByteRecv = m_lastSecRecvBytes;
}

bool CG_NetClientTcp::SendPacket(CG_CmdPacket *packet)
{
	return SendPacket(packet,true,false);	
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

void CG_NetClientTcp::SetBufferSize(int client, int net)
{
	if(client > 0)
	{
		m_maxBufSize = client*1024;
	}

	if(net > 0)
	{
		m_socket.SetSendBufferSize(net*1024);
		// m_socket.SetRecvBufferSize(net*1024);
	}
}

void CG_NetClientTcp::Process()
{
	DefProc();
	bool ret = false;
	
	switch (m_state)
	{
		case NET_STATE_CONNECTED:
			do
			{
				while(GetPacket(m_dev->GetRecvCmdPacket()))
				{
					if(m_notify)
						m_notify->OnReceive(this,m_dev->GetRecvCmdPacket());
    			}
			}	while(ReadData());

			DWORD interval,now;
			now = rtMilliseconds();
			interval = now - m_lastStatTime;
			if(interval > NET_STAT_INTERVAL)
			{
				m_lastSecSendBytes = m_statSendBytes*(double)1000/interval;
				m_lastSecRecvBytes = m_statRecvBytes*(double)1000/interval;
				m_lastStatTime = now;
				m_statSendBytes = 0;
				m_statRecvBytes = 0;
				// RtCoreLog().Info("lastSecSendBytes = %d\n",m_lastSecSendBytes);
				// RtCoreLog().Info("lastSecRecvBytes = %d\n",m_lastSecRecvBytes);
			}
			break;

		case NET_STATE_DEAD:
			Close(0);
			if(m_notify)
				m_notify->OnDisconnect(this,NET_ERR_UNKNOWN);
			break;

		case NET_STATE_RECONNECT:
			if(m_reConnTime > rtMilliseconds())
			{
				Init(m_bindIp.c_str(), m_bindPort, m_notify);
				Connect(m_remoteIp.c_str(),m_remotePort);
			}
			break;

		case NET_STATE_WAITING_SEED:
			// check waiting seed timeout
			if(rtMilliseconds() - m_waitSeedTime > NET_CONNECT_TIMEOUT)
			{
				RtCoreLog().Info("%s: waiting seed timeout\n",m_debugStr);
				Close(0);
				if(m_notify)
					m_notify->OnConnect(this,NET_ERR_TIMEOUT);
			}
			else
			{
				// try get encrypt seed 
				if(RecvPacket(m_dev->GetRecvNetPacket()))
				{
					m_dev->GetRecvNetPacket()->AttachCmdPacket(m_dev->GetRecvCmdPacket());
					if(m_dev->GetRecvNetPacket()->IsSysPacket())
					{
						HandleSysPacket(m_dev->GetRecvCmdPacket());
					}
				}
			}
			break;
			
		case NET_STATE_CONNECTING:
			if(m_socket.HasExcept())
			{
				RtCoreLog().Info("%s: connecting failed\n",m_debugStr);
				Close(0);
				if(m_notify)
					m_notify->OnConnect(this,NET_ERR_UNKNOWN);
				break;
			}
			else if( rtMilliseconds() - m_connTime > NET_CONNECT_TIMEOUT)
			{
				RtCoreLog().Info("%s: connecting timeout\n",m_debugStr);
				Close(0);
				if(m_notify)
					m_notify->OnConnect(this,NET_ERR_TIMEOUT);
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
				m_lastStatTime = m_startTime;
			    m_lastSendTime = m_startTime;
				m_lastRecvTime = m_startTime;
				m_waitSeedTime = m_startTime;
			}
			else
			{
				Close(0);
				if(m_notify)
					m_notify->OnConnect(this,NET_ERR_UNKNOWN);
			}
 			break;
	}
}

bool CG_NetClientTcp::GetRemoteAddr(SNetAddr &addr)
{
	return m_socket.GetRemoteAddr(addr);
}
// -------------------------------------------------------------------------------------
// CG_NetClient interface end
// -------------------------------------------------------------------------------------

void CG_NetClientTcp::Reset()
{
	m_rtt = 0;

	//delete []m_readBuf;
	//delete []m_writeBuf;
	DEL_ARRAY( m_readBuf );
	DEL_ARRAY( m_writeBuf );

	m_readBuf = RT_NEW char[MAX_BUFFER_SIZE];
	m_readBufSize = MAX_BUFFER_SIZE;

	m_writeBuf = RT_NEW char[MAX_BUFFER_SIZE];
	m_writeBufSize = MAX_BUFFER_SIZE;
	
	m_socket.Close();
	m_socket.Reset();
	
	m_seedLen = 0;
	m_sendSeq = 0;
	m_recvSeq = 0;

	m_lastStatTime  = 0;
	m_statSendBytes = 0;
	m_statRecvBytes = 0;

	m_sendPktNum	= 0;
	m_recvPktNum	= 0;
	m_rowSendBytes	= 0;
	m_rowRecvBytes	= 0;
	m_sendBytes	= 0;
	m_recvBytes	= 0;
	m_lastSecSendBytes	= 0;
	m_lastSecRecvBytes	= 0;

	m_state = NET_STATE_ZERO;
	m_writeLen = m_writeBeg = m_writeEnd = 0;
	m_readLen = m_readPos = 0;
}

void CG_NetClientTcp::Attach(SOCKET socket)
{
	m_socket.Attach(socket);
	m_state = NET_STATE_CONNECTED;
	m_startTime = rtMilliseconds();
	m_lastStatTime = m_startTime;
	m_lastRecvTime = m_startTime;
	m_lastSendTime = m_startTime;
}

//#include "profile.h"

bool CG_NetClientTcp::SendPacket(CG_CmdPacket *packet,bool flush,bool sys)
{
	// PROFILE("CG_NetClientTcp_SendPacket");
	if (m_state != NET_STATE_CONNECTED) 
		return false;

	m_dev->GetSendNetPacket()->AddCmdPacket(packet);
	m_dev->GetSendNetPacket()->SetSeq(m_sendSeq);
	
	int row = m_dev->GetSendNetPacket()->GetTotalSize();

	// dont's encrypt sys packet 
	if(sys)
	{
		m_dev->GetSendNetPacket()->SetSysPacket();
	}
	else
	{
		// check if need encrypt and compress 
		if(m_bCompress) m_dev->GetSendNetPacket()->Compress();
		if(m_bEncrypt) m_dev->GetSendNetPacket()->Encrypt(m_seed,m_seedLen);
	}

	// add to send buffer 
	// if push data failed,don't set state to NET_STATE_DEAD
	// the app layer decide if should close net,not net layer 
	if(!PushData(m_dev->GetSendNetPacket()->GetBuffer(),m_dev->GetSendNetPacket()->GetTotalSize())) 
	{
		// RT_NEW added,need test
		// ...
		// m_state = NET_STATE_DEAD;
		return false;
	}

	m_rowSendBytes += row;
	// compute next seq number 
	m_sendSeq++;
	m_sendPktNum++;
	if(m_sendSeq > MAX_NET_PACKET_SEQ) m_sendSeq = 1;

	// flush data  
	if(flush) FlushData();
	return true;
}

int CG_NetClientTcp::GetAvgFluxIn()
{
	int time = (rtMilliseconds() - m_startTime)/1000;
	if(time == 0) time = 1;
	return GetTotleBytesIn()/time;
}

int CG_NetClientTcp::GetAvgFluxOut()
{
	int time = (rtMilliseconds() - m_startTime)/1000;
	if(time == 0) time = 1;
	return (GetTotleBytesOut()/time);
}

int CG_NetClientTcp::GetTotleBytesIn()
{
	return m_recvBytes;
}

int CG_NetClientTcp::GetTotleBytesOut()
{
	return m_sendBytes;
}

bool CG_NetClientTcp::GetPacket(CG_CmdPacket *packet)
{
	while(RecvPacket(m_dev->GetRecvNetPacket()))
	{
		// attach a cmd packet for read 
		m_dev->GetRecvNetPacket()->AttachCmdPacket(packet);
		if(m_dev->GetRecvNetPacket()->IsSysPacket())
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

bool CG_NetClientTcp::ReadData()
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
		m_statRecvBytes += ret;
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

void CG_NetClientTcp::FlushData()
{
	//PROFILE("CG_NetClientTcp_FlushData");
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
			m_statSendBytes += ret;
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
			m_statSendBytes += ret;
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
			m_statSendBytes += ret;
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

bool CG_NetClientTcp::PushData(char *buf, int len)
{
	//PROFILE("CG_NetClientTcp_PushData");

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

bool CG_NetClientTcp::ResizeBuf(int min)
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
	DEL_ARRAY( m_writeBuf );
	//delete []m_writeBuf;
	m_writeBuf = buf;
	m_writeBeg = 0;
	m_writeEnd = m_writeLen;
	m_writeBufSize = newSize;
	return true;
}

int CG_NetClientTcp::GetState()
{
	return m_state;
}

bool CG_NetClientTcp::IsDead()
{
	return (m_state == NET_STATE_DEAD);
}

// long startTime = 0;
void CG_NetClientTcp::DefProc()
{
	if(m_state != NET_STATE_CONNECTED && m_state != NET_STATE_WAITING_SEED) return;
	
	if(m_state == NET_STATE_CONNECTED)
	{
		DWORD now = rtMilliseconds();
		if(now - m_lastSendTime > NET_KEEP_ALIVE_INTERVAL)
		{
			// startTime = 0;
			// rtClock(startTime);
			// KeepAlive(startTime);
			KeepAlive(now);
			m_lastSendTime = now;
		}
		if(now - m_lastRecvTime > NET_TIMEOUT)
		{
			RtCoreLog().Error("%s: Tcp session dead reason = timeout\n",m_debugStr);
			m_state = NET_STATE_DEAD;
			return;
		}
		FlushData();
	}
	ReadData();
}

bool CG_NetClientTcp::RecvPacket(CG_NetPacket *packet)
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
		m_recvPktNum++;
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

int CG_NetClientTcp::HandleSysPacket(CG_CmdPacket *packet)
{
	short type;
	long  param;

	if(!packet->ReadShort(&type)) return NULL;
	switch(type)
	{
		case SYS_PACKET_KEEP_ALIVE:
			if(!packet->ReadLong(&param)) return NULL;
			// LOG("recv keep alive msg\n");
			m_dev->GetSendCmdPacket()->BeginWrite();
			m_dev->GetSendCmdPacket()->WriteShort(SYS_PACKET_ALIVE_ACK);
			m_dev->GetSendCmdPacket()->WriteLong(param);
			SendPacket(m_dev->GetSendCmdPacket(),true,true);
			break;

		case SYS_PACKET_ALIVE_ACK:
			if(!packet->ReadLong(&param)) return NULL;
			// LOG("recv alive ack msg\n");
			m_rtt = rtMilliseconds() - param;
			/*
			rtUnclock(startTime);
			m_rtt = startTime*g_SecondsPerCycle*1000;
			RtCoreLog().Info("recv alive ack msg [%d]\n",m_rtt);
			*/
			break;

		case SYS_PACKET_SET_CONNECT_INFO:
			long ver;
			if(!packet->ReadLong(&ver)) return NULL;
			if(ver != NET_LIB_VERSION)
			{
				if(m_notify)
					m_notify->OnConnect(this,NET_ERR_VERSION_ERROR);
				Close(0);
				return NULL;
			}
			if(!packet->ReadLong(&param)) return NULL;
			SetSeed(param);
			m_state = NET_STATE_CONNECTED;
			if(m_notify)
				m_notify->OnConnect(this,NET_ERR_NONE);
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

void CG_NetClientTcp::SetSeed(long seed)
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

void CG_NetClientTcp::SendConnectInfo(long seed)
{
	m_dev->GetSendCmdPacket()->BeginWrite();
	m_dev->GetSendCmdPacket()->WriteShort(SYS_PACKET_SET_CONNECT_INFO);
	m_dev->GetSendCmdPacket()->WriteLong(NET_LIB_VERSION);
	m_dev->GetSendCmdPacket()->WriteLong(seed);
	SendPacket(m_dev->GetSendCmdPacket(),true,true);
	SetSeed(seed);
}

void CG_NetClientTcp::KeepAlive(long now)
{
	m_dev->GetSendCmdPacket()->BeginWrite();
	m_dev->GetSendCmdPacket()->WriteShort(SYS_PACKET_KEEP_ALIVE);
	m_dev->GetSendCmdPacket()->WriteLong(now);
	SendPacket(m_dev->GetSendCmdPacket(),true,true);
}

int CG_NetClientTcp::GetRtt()
{
	return m_rtt;
}

void CG_NetClientTcp::EnableDebugInfo(bool debug, const char *title) 
{
	m_bDebugInfo = debug;
	if(title)
	{
		rt2_snprintf(m_debugStr, MAX_DEBUG_STR_LEN-1, "%s(%s)", m_debugStr,title);
		m_debugStr[MAX_DEBUG_STR_LEN-1] = '\0';
	}
	m_socket.EnableDebugInfo(debug, title);
}

// -------------------------------------------------------------------------------------
// CG_TCPListener 
// -------------------------------------------------------------------------------------
CG_TCPListener::CG_TCPListener()
{
	m_dev = NULL;
}

CG_TCPListener::~CG_TCPListener()
{

}

bool CG_TCPListener::StartListen(const char *ip, int port,bool encrypt)
{
	if(!m_socket.Initialize(NETWORK_PROTOCOL_TCP))
		return false;

	m_socket.SetReuseAddr(true);
	if(!m_socket.BindAddr(ip,port)) return false;
	
	// initialize random seed 
	srand(rtMilliseconds());
	m_bEncrypt = encrypt;
	return m_socket.Listen();
}

CG_NetClientTcp *CG_TCPListener::AcceptNewClient()
{
	if(!m_socket.CanRead()) return NULL;
	SOCKET socket = m_socket.Accept();
	if(!socket) return NULL;
	
	CG_NetClientTcp *tmp = RT_NEW CG_NetClientTcp;
	tmp->SetDevice(m_dev);
	tmp->Attach(socket);
	
	// set encrypt seed
	tmp->SendConnectInfo(ComputeSeed());
	return tmp;
}

/*
bool CG_TCPListener::AcceptNewClient(CG_NetClientTcp *client)
{
	if(!m_socket.CanRead()) return false;
	SOCKET socket = m_socket.Accept();
	if(!socket) return false;

	session->Attach(socket);

	// set encrypt seed 
	session->SendSeed(ComputeSeed());
	return true;
}
*/

void CG_TCPListener::Close()
{
	m_socket.Close();
}

long CG_TCPListener::ComputeSeed()
{
	long seed = 0;
	if(!m_bEncrypt) return seed;
	seed |= rand()<<16;
	seed |= rand();
	return seed;
}

// ------------------------------------------------------------------------------
// tcp server
// ------------------------------------------------------------------------------
CG_NetServerTcp::CG_NetServerTcp()
{
	m_notify	 = NULL;
	m_listener   = NULL;
	m_closeDelay = 0;
	m_dev        = NULL;
}

CG_NetServerTcp::~CG_NetServerTcp()
{
    DEL_ONE( m_listener );
}

bool CG_NetServerTcp::Init(const char *bindIp, int bindPort, CG_NetServerNotify *notify)
{
	m_notify = notify;
	m_listener = RT_NEW CG_TCPListener;
	m_listener->m_dev = m_dev;
	return m_listener->StartListen(bindIp,bindPort,true);
}

void CG_NetServerTcp::Destroy()
{
	m_listener->Close();
	//delete m_listener;
	DEL_ONE( m_listener );

	std::list<CG_NetClientTcp*>::iterator it;
	for(it=m_clientList.begin(); it!=m_clientList.end(); it++)
	{
		CG_NetClientTcp *tcp = *it;
		tcp->Destroy();
		/*delete tcp;*/
		DEL_ONE( tcp );
	}
}

void CG_NetServerTcp::Close(DWORD delay)
{
	m_closeDelay = delay + rtMilliseconds();
}

bool CG_NetServerTcp::IsClosing()
{
	return m_closeDelay != 0;
}

bool CG_NetServerTcp::CanClose()
{
	if(m_closeDelay == 0) return false;
	return m_closeDelay < rtMilliseconds();
}

void CG_NetServerTcp::Process()
{
	// try accept RT_NEW client
	CG_NetClientTcp *tmp = NULL;
	int cnt = 0;

	if(!m_listener) return;

	while(cnt++ < 20)
	{
		tmp = m_listener->AcceptNewClient();
		if(!tmp) break;
		m_clientList.push_back(tmp);
		m_notify->OnAccept(this,tmp);
	}

	// process current client
	std::list<CG_NetClientTcp*>::iterator it;
	for(it=m_clientList.begin(); it!=m_clientList.end();)
	{
		CG_NetClientTcp *tcp = *it;
		if(tcp->CanClose())
		{
			tcp->Destroy();
			/*delete tcp;*/
			DEL_ONE( tcp );
			it = m_clientList.erase(it);
			continue;
		}

		tcp->DefProc();
		if(tcp->IsClosing())
		{
			it++;
			continue;
		}

		if(tcp->IsDead())
		{
			m_notify->OnDisconnect(this,tcp,0);
			tcp->Destroy();
			/*delete tcp;*/
			DEL_ONE( tcp );
			it = m_clientList.erase(it);
			continue;
		}

		while(tcp->GetPacket(&m_packet))
		{
			m_notify->OnReceive(this,tcp,&m_packet);
		}
		it++;
	}
}
