//
// g_udpsession.h: interface for the CG_UdpSession class.
//
#ifndef __CG_UDP_IMP_H__
#define __CG_UDP_IMP_H__

class CG_NetAddress
{
public:
	CG_NetAddress()
	{ 
		memset(&m_addr,0,sizeof(m_addr));
		// SetRemote(NULL);
		m_remote = "";
		SetPort(0);
	};
	~CG_NetAddress(){};

	SOCKADDR_IN	*GetAddr() {return &m_addr;}
	bool BuildAddr()
	{
		hostent *host=NULL;
		memset(&m_addr,0,sizeof(m_addr));
		m_addr.sin_family=AF_INET;
		m_addr.sin_port=htons(m_port);
		if(!m_remote.empty())
			m_addr.sin_addr.s_addr = inet_addr(m_remote.c_str());
		if(m_addr.sin_addr.s_addr==INADDR_NONE)
		{
			if(m_remote.empty()) return false;
			host=gethostbyname(m_remote.c_str());
			if(!host) return false;
			memcpy(&m_addr.sin_addr,host->h_addr_list[0],host->h_length);
		}
		m_ip = inet_ntoa(m_addr.sin_addr);
		m_addrLen = sizeof(m_addr);
        return true;
	}
	void SetAddr(SOCKADDR_IN *addr,int len)
	{
		memcpy(&m_addr,addr,len);
		m_addrLen = len;
		m_ip = inet_ntoa(m_addr.sin_addr);
		m_port = ntohs(m_addr.sin_port);
	}
	void SetRemote(const char *remote) {m_remote = remote;}
	void SetPort(unsigned short port) {m_port = port;}
	const char *GetRemote() {return m_remote.c_str();}
	const char *GetIP() {return m_ip.c_str();}
	unsigned short GetPort() {return m_port;}
	
private:
	SOCKADDR_IN		m_addr;
	int				m_addrLen;
	unsigned short	m_port;
	std::string		m_remote;
	std::string		m_ip;
};

class CG_UDPSession  
{
public:
	CG_UDPSession();
	virtual ~CG_UDPSession();

	void Close();
	bool RecvFrom(CG_CmdPacket *cmd,CG_NetAddress *fromAddr);
	bool SendTo(CG_CmdPacket *cmd,CG_NetAddress *ToAddr);
	bool Create(const char *addr,short port);
	
private:
	//static char m_buffer[MAX_NET_PACKET_SIZE];
	S_MEMDEF(m_buffer, MAX_NET_PACKET_SIZE)
	CG_NetDeviceImp *m_dev;
	CG_NetSocket m_socket;
};

#endif
