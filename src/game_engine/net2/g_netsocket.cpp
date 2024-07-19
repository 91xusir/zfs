//
// g_netsocket.cpp: implementation of the CG_NetSocket class.
//
#include "rt_net_imp.h"
#include "core/rt2_core.h"

#ifdef WIN32
	#pragma comment(lib, "Ws2_32.lib")
#endif

CG_NetSocket::CG_NetSocket()
{
	CM_MEMPROTECTOR(m_debugStr, MAX_DEBUG_STR_LEN)

	m_bDebugInfo = true;
    rt2_strcpy(m_debugStr,NET_LIB);
	Reset();
}

CG_NetSocket::~CG_NetSocket()
{
	CM_MEMUNPROTECTOR(m_debugStr)
	Close();
}

bool CG_NetSocket::Initialize(int protocol)
{
    DWORD dwNow = rtMilliseconds();

	if (protocol==NETWORK_PROTOCOL_UDP)
	{
		m_socket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	}
	else 
	{
		m_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	}

	if(m_socket==INVALID_SOCKET) 
	{
		return false;
	}

	SetNonBlocking();
	
	/*
	// test code 
	long arg = 1;
	int ret = setsockopt(m_socket,IPPROTO_TCP,TCP_NODELAY,(char *)&arg,sizeof(arg));
	if(ret)
	{
		Sys_Log("socket: set tcp_nodelay failed");
	}
	*/
	return true;
}

bool CG_NetSocket::BindAddr(const char *ip,int port)
{
	SOCKADDR_IN addrLocal;
	addrLocal.sin_family=AF_INET;
	addrLocal.sin_port=htons(port);
	if(ip && strcmp(ip,"localhost")!=0)
	{
		addrLocal.sin_addr.s_addr=inet_addr(ip);
	}
	else
	{
		addrLocal.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	if(bind(m_socket,(SOCKADDR *)&addrLocal,sizeof(addrLocal))==SOCKET_ERROR)
	{
        RtCoreLog().Info("%s: bind socket error, IP=%s, Port=%d\n",m_debugStr, ip?ip:"none", port);
		return false;
	}
	return true;
}

bool CG_NetSocket::Listen()
{
	if(listen(m_socket,SOMAXCONN)==SOCKET_ERROR)
	{
		RtCoreLog().Info("%s: socket start listen error\n",m_debugStr);
		return false;
	}
	return true;
}

bool CG_NetSocket::Connect(const char *szAddr, int port, unsigned long ip)
{
	SOCKADDR_IN addrRemote;
	hostent *host=NULL;

	memset(&addrRemote,0,sizeof(addrRemote));
	addrRemote.sin_family=AF_INET;
	addrRemote.sin_port=htons(port);
	
	if(szAddr)
		addrRemote.sin_addr.s_addr = inet_addr(szAddr);
	else
		addrRemote.sin_addr.s_addr = ip;
	
	if(addrRemote.sin_addr.s_addr==INADDR_NONE)
	{
		if(!szAddr) return false;
		host=gethostbyname(szAddr);
		if(!host)
		{
#ifdef WIN32
			int iErrorCode = WSAGetLastError();
			switch (iErrorCode)
			{
            case WSANOTINITIALISED: RtCoreLog().Error("WSAError: A successful WSAStartup call must occur before using this function.\n"); break;
			case WSAENETDOWN:		RtCoreLog().Error("WSAError: The network subsystem has failed.\n"); break;
			case WSAHOST_NOT_FOUND: RtCoreLog().Error("WSAError: Authoritative answer host not found.\n"); break;
			case WSATRY_AGAIN:		RtCoreLog().Error("WSAError: Nonauthoritative host not found, or server failure.\n"); break;
			case WSANO_RECOVERY:	RtCoreLog().Error("WSAError: A nonrecoverable error occurred.\n"); break;
			case WSANO_DATA:		RtCoreLog().Error("WSAError: Valid name, no data record of requested type.\n"); break;
			case WSAEINPROGRESS:	RtCoreLog().Error("WSAError: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n"); break;
			case WSAEFAULT:			RtCoreLog().Error("WSAError: The name parameter is not a valid part of the user address space.\n"); break;
			case WSAEINTR:			RtCoreLog().Error("WSAError: A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n"); break;
			default:				RtCoreLog().Error("WSAError: Unknown socket error.\n"); break;
			}
#endif
			return false;
		}
		memcpy(&addrRemote.sin_addr,host->h_addr_list[0],host->h_length);
	}
	
	if(connect(m_socket,(SOCKADDR *)&addrRemote,sizeof(addrRemote))
		==SOCKET_ERROR)
	{
		int err = GETERROR;
		if (err != CONN_INPRROGRESS)
		{
			RtCoreLog().Info("%s: Socket connect error = %d\n",m_debugStr,err); 
			return false;
		}
	}
	return true;
}

//
// return value 
// =  0 send failed
// >  0	bytes send
// = -1 net dead
//
int CG_NetSocket::Send(char *buf, int len)
{
	if (!CanWrite()) return 0;
	
	//
	// in linux be careful of SIGPIPE
	//
	int ret;
	ret = send(m_socket,buf,len,0);

	if (ret==SOCKET_ERROR)
	{
		int err=GETERROR;
		if (err==WSAEWOULDBLOCK) 
			return 0;
		return -1;
	}
	
	return ret;
}

//
// return value 
// =  0 recv failed
// >  0	bytes recv
// = -1 net dead
//
int CG_NetSocket::Recv(char *buf, int len)
{
	if (!CanRead())	return 0;

	//
	// in linux be careful of SIGPIPE 
	//
	int ret;
	ret = recv(m_socket,buf,len,0);

	if (ret==0)
	{
		// remote closed 
		return -1;
	}

	if (ret==SOCKET_ERROR)
	{
		int err=GETERROR;
		if (err!=WSAEWOULDBLOCK)
		{
			return -1;
		}
	}

	return ret;
}

bool CG_NetSocket::CanRead()
{
#ifdef LINUX
	pollfd pfds;
	pfds.fd = m_socket;
	pfds.events = POLLIN;
	int ret = poll(&pfds,1,0);
	if(ret > 0 && (pfds.revents&POLLIN))
		return true;
	else
		return false;
#else
	fd_set readfds;
	timeval timeout;

	timeout.tv_sec  = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(m_socket,&readfds);
	int ret = select(m_socket+1,&readfds,NULL,NULL,&timeout);
	if(ret > 0 && FD_ISSET(m_socket,&readfds))
		return true;
	else 
		return false;
#endif
}

bool CG_NetSocket::CanWrite()
{
#ifdef LINUX
	pollfd pfds;
	pfds.fd = m_socket;
	pfds.events = POLLOUT;
	int ret = poll(&pfds,1,0);
	if(ret > 0 && (pfds.revents&POLLOUT))
		return true;
	else
		return false;
#else
	fd_set writefds;
	timeval timeout;

	timeout.tv_sec  = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&writefds);
	FD_SET(m_socket,&writefds);
	int ret = select(m_socket+1,NULL,&writefds,NULL,&timeout);
	if(ret > 0 && FD_ISSET(m_socket,&writefds))
		return true;
	else 
		return false;
#endif
}

bool CG_NetSocket::Close()
{
	if (m_socket == INVALID_SOCKET) return false;
	CLOSESOCKET(m_socket);
	Reset();
	return true;
}

SOCKET CG_NetSocket::Accept()
{
	SOCKADDR_IN addr;
	int len = sizeof(addr);
	SOCKET tmp;

	tmp = accept(m_socket,(SOCKADDR *)&addr,(socklen_t *)&len);

	if (tmp == INVALID_SOCKET || tmp == NULL)
	{
		RtCoreLog().Error("%s: accept RT_NEW socket error\n",m_debugStr);
		return NULL;
	}
	return tmp;	
}

bool CG_NetSocket::Attach(SOCKET socket)
{
	m_socket = socket;
	SetNonBlocking();
	return true;
}

int CG_NetSocket::SendTo(char *buf, int len, SOCKADDR_IN *addr)
{
	if (!CanWrite()) return 0;
	
	int ret;
	ret = sendto(m_socket,buf,len,0,(SOCKADDR *)addr,sizeof(SOCKADDR_IN));
	if (ret == SOCKET_ERROR)
	{
		int err=GETERROR;
		if (err!=WSAEWOULDBLOCK)
		{
			return -1;
		}
	}
	return ret;
}

int CG_NetSocket::RecvFrom(char *buf, int len, SOCKADDR_IN *addr ,int *addrlen)
{
	if (!CanRead()) return 0;

	int ret;
	ret = recvfrom(m_socket,buf,len,0,(SOCKADDR *)addr,(socklen_t *)addrlen);
	if (ret == SOCKET_ERROR)
	{
		int err=GETERROR;
		if (err!=WSAEWOULDBLOCK)
		{
			return -1;
		}
	}
	return ret;
}

bool CG_NetSocket::HasExcept()
{
#ifdef LINUX
	pollfd pfds;
	pfds.fd = m_socket;
	pfds.events = POLLIN;
	int ret = poll(&pfds,1,0);
	if(ret > 0 && (pfds.revents&POLLERR))
		return true;
	else
		return false;
#else
	fd_set exceptfds;
	timeval timeout;

	timeout.tv_sec=0;
	timeout.tv_usec=0;
	FD_ZERO(&exceptfds);
	FD_SET(m_socket,&exceptfds);
	int ret = select(m_socket+1,NULL,NULL,&exceptfds,&timeout);
	if(ret > 0 && FD_ISSET(m_socket,&exceptfds))
	{
		int err = GETERROR;
		RtCoreLog().Info("%s: Socket except error = %d\n",m_debugStr,err); 
		return true;
	}
	else
	{
		return false;
	}
#endif
}

void CG_NetSocket::Reset()
{
	m_socket = INVALID_SOCKET;
}

bool CG_NetSocket::SetNonBlocking()
{
	//
	// set to nonblocking socket mode 
	//
	u_long arg;
	arg = 1;
	int ret;
	ret = IOCTLSOCKET(m_socket,FIONBIO,&arg);
	if (ret == SOCKET_ERROR)
	{
		RtCoreLog().Error("%s: set nonblocking mode failed,IOCTLSOCKET = %d",m_debugStr,ret);
		return false;
	}
	else
	{
		return true;
	}
}

bool CG_NetSocket::SetBlocking()
{
	//
	// set to blocking socket mode 
	//
	u_long arg;
	arg = 0;
	int ret;
	ret = IOCTLSOCKET(m_socket,FIONBIO,&arg);
	if (ret == SOCKET_ERROR)
	{
		RtCoreLog().Error("%s: set blocking mode failed,IOCTLSOCKET = %d",m_debugStr,ret);
		return false;
	}
	else
	{
		return true;
	}
}

bool CG_NetSocket::SetSendBufferSize(int len)
{
	int ret;
	ret = setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&len,sizeof(len));
	if (ret == SOCKET_ERROR) return false;
	return true;
}

bool CG_NetSocket::SetRecvBufferSize(int len)
{
	int ret;
	ret = setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&len,sizeof(len));
	if (ret == SOCKET_ERROR) return false;
	return true;
}

int CG_NetSocket::GetSendBufferSize()
{
	int ret,data;
	int size = sizeof(data);
	ret = getsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&data,(socklen_t*)&size);
	if (ret == SOCKET_ERROR) return -1;
	return data;
}

int CG_NetSocket::GetRecvBufferSize()
{
	int ret,data;
	int size = sizeof(data);
	ret = getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&data,(socklen_t*)&size);
	if (ret == SOCKET_ERROR) return -1;
	return data;
}

// 
// get local address 
//
bool CG_NetSocket::GetLocalAddr(char *addr, short *port, unsigned long *ip)
{
	SOCKADDR_IN addrLocal;
	socklen_t len = sizeof(addrLocal);
	if(getsockname(m_socket,(SOCKADDR*)&addrLocal,&len)==SOCKET_ERROR)
		return false;
	
	char *tmp = inet_ntoa(addrLocal.sin_addr);
	if(!tmp) 
		return false;
	if(addr) 
		rt2_strcpy(addr,tmp);
	if(port)
		*port = ntohs(addrLocal.sin_port);
	if(ip) 
		*ip = addrLocal.sin_addr.s_addr;
	return true;
}

// 
// get remote address 
//
bool CG_NetSocket::GetRemoteAddr(SNetAddr &addr)
{
	sockaddr_in addrRemote;
	int len = sizeof(addrRemote);
	if(getpeername(m_socket,(sockaddr *)&addrRemote,(socklen_t *)&len)==SOCKET_ERROR)
		return false;
	
	char *tmp = inet_ntoa(addrRemote.sin_addr);
	if(!tmp) 
		return false;
	
	addr.ip = tmp;
	addr.port = ntohs(addrRemote.sin_port);
	
	// if(ip)
	//	*ip = addrRemote.sin_addr.s_addr; 
	return true;
}

bool CG_NetSocket::SetReuseAddr(bool reuse)
{
// #ifndef WIN32
	// only useful in linux 
	int opt = 0;
	unsigned int len = sizeof(opt);
	if(reuse) opt = 1;
	if(setsockopt(m_socket,SOL_SOCKET,SO_REUSEADDR,(const char*)&opt,len)==SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		return true;
	}
// #endif
	return true;
}

void CG_NetSocket::EnableDebugInfo(bool debug, const char *title) 
{
	m_bDebugInfo = debug;
	if(title)
	{	
		rt2_snprintf(m_debugStr, MAX_DEBUG_STR_LEN-1, "%s(%s)", m_debugStr,title);
		m_debugStr[MAX_DEBUG_STR_LEN-1] = '\0';
	}
}
