#include "rt_net_2_1.h"

CG_TCPSession::CG_TCPSession()
{

}

CG_TCPSession::~CG_TCPSession()
{

}

void CG_TCPSession::Process()
{

}

void CG_TCPSession::EnableCompress(bool bCom)
{
	m_bCompress = bCom;
}

bool CG_TCPSession::Connect(const char* m_szHost, DWORD m_sPort)
{
	return true;
}

bool CG_TCPSession::SendPacket(CG_CmdPacket* pPacket)
{
	return true;
}

int CG_TCPSession::GetState()
{
	return m_iState;
}

int CG_TCPSession::GetAvgFluxIn()
{
	return 0;
}

int CG_TCPSession::GetAvgFluxOut()
{
	return 0;
}

int CG_TCPSession::GetBytesIn()
{
	return 0;
}

int CG_TCPSession::GetBytesOut()
{
	return 0;
}

int CG_TCPSession::GetSendCompressRate()
{
	return 0;
}

int CG_TCPSession::GetRecvCompressRate()
{
	return 0;
}

void CG_TCPSession::Close(DWORD dwTime /* = 500 */)
{
	return;
}
