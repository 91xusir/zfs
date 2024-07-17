#ifndef __RT_NET_H__
#define __RT_NET_H__

class CG_NetClient;
class CG_NetServer;
class CG_NetDevice;
class CG_NetServerNotify;
class CG_NetClientNotify;

#include "core/rt2_core.h"
using namespace rt2_core;
using namespace std;
#include "rt_net_extend.h"

// -----------------------------------------------------------------------------
// network device info
// -----------------------------------------------------------------------------
struct SDeviceInfo
{
	int Version;
	int FDSetSize;
	int SendBufferSize;
	int RecvBufferSize;
};

// -----------------------------------------------------------------------------
// network performance
// -----------------------------------------------------------------------------
struct SNetInfo
{
	SNetInfo() 
	{
		m_rtt                   = 0;
		m_sendPacketNum			= 0;
		m_recvPacketNum			= 0;
		m_totalByteSend		    = 0;
		m_totalByteRecv		    = 0;
		m_secByteSend           = 0;
		m_secByteRecv           = 0;
		m_lastSecByteSend	    = 0;
		m_lastSecByteRecv	    = 0;
		
		m_sendCompressRate      = 0;
		m_recvCompressRate      = 0;
	}
	DWORD  m_rtt;
	DWORD  m_sendPacketNum;
	DWORD  m_recvPacketNum;

	DWORD  m_totalByteSend;
	DWORD  m_totalByteRecv;

	DWORD  m_secByteSend;
	DWORD  m_secByteRecv;

	DWORD  m_lastSecByteSend;
	DWORD  m_lastSecByteRecv;

	float  m_sendCompressRate;
	float  m_recvCompressRate;
};

// -----------------------------------------------------------------------------
// network address
// -----------------------------------------------------------------------------
struct SNetAddr
{
	string ip;
	short  port;
};

// -----------------------------------------------------------------------------
// RT_NEW network interface
// -----------------------------------------------------------------------------
const int NET_ERR_NONE           = 0;
const int NET_ERR_UNKNOWN        = 1;
const int NET_ERR_VERSION_ERROR  = 2;
const int NET_ERR_TIMEOUT        = 3;
const int NET_ERR_REMOTE_CLOSE   = 4;

class CG_NetClientNotify
{
public:
	CG_NetClientNotify() {}
	virtual ~CG_NetClientNotify() {}

	virtual void OnConnect(CG_NetClient *client, int err) = 0;
	virtual void OnReceive(CG_NetClient *client, CG_CmdPacket *packet) = 0;
	virtual void OnDisconnect(CG_NetClient *client, int err) = 0;
};

class CG_NetServerNotify
{
public:
	CG_NetServerNotify() {}
	virtual ~CG_NetServerNotify() {}

	virtual void OnAccept(CG_NetServer *server, CG_NetClient *client) = 0;
	virtual void OnDisconnect(CG_NetServer *server, CG_NetClient *client, int err) = 0;
	virtual void OnReceive(CG_NetServer *server, CG_NetClient *client, CG_CmdPacket *packet) = 0;
};

class CG_NetClient
{
public:
	CG_NetClient() {}
	virtual ~CG_NetClient() {}

	virtual void SetParam(DWORD param1, DWORD param2) = 0; 
	virtual void GetParam(DWORD &param1, DWORD &param2) = 0;

	virtual bool Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify) = 0;
	virtual bool Connect(const char *remoteIp, int remotePort) = 0;
	virtual void ReConnect(DWORD delay) = 0;
	virtual void Close(DWORD delay) = 0;
	virtual bool SendPacket(CG_CmdPacket *packet) = 0;

	virtual void EnableDebugInfo(bool debug, const char *title) = 0;
	virtual void EnableCompress(bool local, bool remote) = 0;
	virtual void GetNetInfo(SNetInfo &info) = 0;
	virtual void SetBufferSize(int session, int net) = 0;
	
	virtual bool GetRemoteAddr(SNetAddr &addr) = 0;
};

class CG_NetServer
{
public:
	CG_NetServer() {}
	virtual ~CG_NetServer() {}

	virtual void SetParam(DWORD param1, DWORD param2) = 0;
	virtual void GetParam(DWORD &param1, DWORD &param2) = 0;

	virtual bool Init(const char *ip, int port, CG_NetServerNotify *notify) = 0;
	virtual void Close(DWORD delay) = 0;
	
	virtual void EnableDebugInfo(bool debug, const char *title) = 0;
	virtual void EnableCompress(bool local, bool remote) = 0;
	virtual void GetNetInfo(SNetInfo &info) = 0;
	virtual void AddAllowableIp(string from, string to) = 0;
	virtual void SetMaxClient(int num) = 0;
};

class CG_NetDevice
{
public:
	CG_NetDevice() {}
	virtual ~CG_NetDevice() {}

	virtual bool Init() = 0;
	virtual void Tick(long delta) = 0;
	virtual void Exit() = 0;

	virtual CG_NetClient *CreateClient(int protocal) = 0;
	virtual CG_NetServer *CreateServer(int protocal) = 0;
	
	virtual void GetDeviceInfo(SDeviceInfo &info) = 0;
};

const int NETWORK_PROTOCOL_UDP	=  1;
const int NETWORK_PROTOCOL_TCP	=  2;

CG_NetDevice* CreateNetDevice();
void DestroyNetDevice(CG_NetDevice *dev);

// -----------------------------------------------------------------------------
// RT_NEW network interface end
// -----------------------------------------------------------------------------

#endif
