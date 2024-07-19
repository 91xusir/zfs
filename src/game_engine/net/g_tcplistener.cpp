//
// g_tcplistener.cpp: implementation of the CG_TCPListener class.
//
#include "net/rt_net.h"

CG_TCPListener::CG_TCPListener()
{

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

CG_TCPSession *CG_TCPListener::AcceptNewSession()
{
	if(!m_socket.CanRead()) return NULL;
	SOCKET socket = m_socket.Accept();
	if(!socket) return NULL;
	
	CG_TCPSession *tmp = RT_NEW CG_TCPSession;
	tmp->Attach(socket);
	
	// set encrypt seed 
	tmp->SendSeed(ComputeSeed());
	return tmp;
}

bool CG_TCPListener::AcceptNewSession(CG_TCPSession *session)
{
	if(!m_socket.CanRead()) return false;
	SOCKET socket = m_socket.Accept();
	if(!socket) return false;

	session->Attach(socket);

	// set encrypt seed 
	session->SendSeed(ComputeSeed());
	return true;
}

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
