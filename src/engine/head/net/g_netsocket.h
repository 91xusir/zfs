//
// NetSocket.h: interface for the CG_NetSocket class.
//
#ifndef __CG_NET_SOCKET_H__
#define __CG_NET_SOCKET_H__

#ifdef WIN32
	/*
	for windows
	*/
	#include <winsock.h>
	#define GETERROR			WSAGetLastError()
	#define CLOSESOCKET(s)		closesocket(s)
	#define IOCTLSOCKET(s,c,a)  ioctlsocket(s,c,a)
	#define CONN_INPRROGRESS	WSAEWOULDBLOCK
	#ifndef socklen_t			// python already define socklen_t...	
		typedef int socklen_t;
	#endif
#else
	/*
	for linux
	*/
	#include <sys/time.h>
	#include <stddef.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <sys/wait.h>
	#include <sys/poll.h>
	// typedef int            BOOL;
	// typedef unsigned char  BYTE;
	// typedef unsigned short WORD;
	// typedef unsigned int   DWORD;
	// #define TRUE  1
	// #define FALSE 0

	/*
	for socket
	*/
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <sys/ioctl.h>
    #undef __USE_GNU
	#include <netdb.h>
    #define __USE_GNU
	#include <sys/errno.h>
	#include <arpa/inet.h>

	typedef int SOCKET;
	typedef sockaddr_in			SOCKADDR_IN;
	typedef sockaddr			SOCKADDR;
	#define INVALID_SOCKET	    (-1)
	#define SOCKET_ERROR        (-1)
	#define GETERROR			errno
	#define WSAEWOULDBLOCK		EWOULDBLOCK
	#define CLOSESOCKET(s)		close(s)
	#define IOCTLSOCKET(s,c,a)  ioctl(s,c,a)
	#define CONN_INPRROGRESS	EINPROGRESS
#endif

const int NETWORK_PROTOCOL_UDP	=	1;
const int NETWORK_PROTOCOL_TCP	=	2;

class CG_NetSocket  
{
public:
	CG_NetSocket();
	CG_NetSocket(const CG_NetSocket &object);
	virtual ~CG_NetSocket();
			
	bool Attach(SOCKET socket);
	bool Close();
	bool Connect(const char *szAddr,int port,unsigned long ip = 0);
	bool Listen();
	bool Initialize(int protocol);
	
	int Recv(char *buf,int len);
	int Send(char *buf,int len);
	int RecvFrom(char *buf,int len,SOCKADDR_IN *addr,int *addrlen);
	int SendTo(char *buf,int len,SOCKADDR_IN *addr);

	bool CanWrite();
	bool CanRead();
	bool HasExcept();

	bool SetNonBlocking();
	bool SetBlocking();
	bool BindAddr(const char *ip,int port);

	bool SetSendBufferSize(int len);
	bool SetRecvBufferSize(int len);
	bool SetReuseAddr(bool reuse);

	bool GetLocalAddr (char *addr, short *port,unsigned long *ip = NULL);
	bool GetRemoteAddr(char *addr, short *port,unsigned long *ip = NULL);
	
	void	Reset();
	SOCKET	Accept();
	SOCKET	m_socket;

	SNetLayerInfo			m_netInfo;
	static SNetLayerInfo	m_globalNetInfo;
	void SetDebugStr(const char *str);
	
private:
	bool _NetStartUp(int VersionHigh,int VersionLow);
	bool _NetCleanUp();

	static int				m_nCount;
	//char					m_debugStr[MAX_DEBUG_STR_LEN];
	CM_MEMDEF(m_debugStr, MAX_DEBUG_STR_LEN)
};

#endif
