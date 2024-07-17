#ifndef __RT_NET_H__
#define __RT_NET_H__

#include "core/rt2_core.h"

class CG_NetClient;
class CG_NetServer;
class CG_NetServerNotify;
class CG_NetClientNotify;
class CG_CmdPacket;

// -----------------------------------------------------------------------------
// network performance
// -----------------------------------------------------------------------------
struct SNetLayerInfo
{
	SNetLayerInfo() {
		m_startTime			    = 0;
		m_totalByteSend		    = 0;
		m_totalByteRecv		    = 0;
		
		m_secByteSend           = 0;
		m_secByteRecv           = 0;
		m_secStartTime          = 0;

		m_lastSecByteSend	    = 0;
		m_lastSecByteRecv	    = 0;
		m_lastSecTime           = 0;
	}
	DWORD  m_startTime;
	DWORD  m_totalByteSend;
	DWORD  m_totalByteRecv;
	
	DWORD  m_secByteSend;
	DWORD  m_secByteRecv;
	DWORD  m_secStartTime;

	DWORD  m_lastSecByteSend;
	DWORD  m_lastSecByteRecv;
	DWORD  m_lastSecTime;

	int GetAvgFluxOut() {
		DWORD dwDiff = rtMilliseconds()-m_startTime;
		if (dwDiff==0) return 0;
		return (m_totalByteSend * 1000 / dwDiff);
	}
	int GetAvgFluxIn() {
		DWORD dwDiff = rtMilliseconds()-m_startTime;
		if (dwDiff==0) return 0;
		return (m_totalByteRecv * 1000 / dwDiff);
	}
	int GetLastSecAvgFluxOut() {
		if (m_lastSecTime==0) return 0;
		return m_lastSecByteSend * 1000 / m_lastSecTime;
	}
	int GetLastSecAvgFluxIn() {
		if (m_lastSecTime==0) return 0;
		return m_lastSecByteRecv * 1000 / m_lastSecTime;
	}
	void UpdateTime(DWORD dwTime) {
		if (dwTime-m_secStartTime > 5000) {
			m_lastSecTime = dwTime - m_secStartTime;
			m_lastSecByteSend = m_secByteSend;
			m_lastSecByteRecv = m_secByteRecv;

			m_secStartTime = dwTime;
			m_secByteSend  = 0;
			m_secByteRecv  = 0;
		}
	}
};

// -----------------------------------------------------------------------------
// new network interface
// -----------------------------------------------------------------------------
class CG_NetClientNotify
{
public:
	CG_NetClientNotify() {}
	virtual ~CG_NetClientNotify() {}

	virtual void OnConnect(CG_NetClient *client, bool ret) {}
	virtual void OnReceive(CG_NetClient *client, CG_CmdPacket *packet) {}
	virtual void OnDisconnect(CG_NetClient *client, int reason) {}
};

class CG_NetServerNotify
{
public:
	CG_NetServerNotify() {}
	virtual ~CG_NetServerNotify() {}

	virtual void OnAccept(CG_NetServer *server, CG_NetClient *client) {}
	virtual void OnDisconnect(CG_NetServer *server, CG_NetClient *client, int reason) {}
	virtual void OnReceive(CG_NetServer *server, CG_NetClient *client, CG_CmdPacket *packet) {}
};

class CG_NetClient
{
public:
	CG_NetClient() {}
	virtual ~CG_NetClient() {}

	void SetName(const char *name) { m_name = name; }
	std::string &GetName() { return m_name; }
	void SetParam(DWORD param1, DWORD param2) { m_param1 = param1; m_param2 = param2; }
	void GetParam(DWORD &param1, DWORD &param2) { param1 = m_param1; param2 = m_param2; }

private:
	std::string m_name;
	DWORD m_param1, m_param2;

public:
	virtual bool Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify) = 0;
	virtual bool Connect(const char *remoteIp, int remotePort) = 0;
	virtual void ReConnect(DWORD delay) = 0;
	virtual void Process() = 0;
	virtual void Close(DWORD delay) = 0;
	virtual bool IsClosing() = 0;
	virtual bool CanClose() = 0;
	virtual void SendPacket(CG_CmdPacket *packet) = 0;

	// enable/disable debug info
	// enable/disable compress
	// enable/disable encrypt
	// set net buffer size
	// get session layer info
	// get net layer info
};

class CG_NetServer
{
public:
	CG_NetServer() {}
	virtual ~CG_NetServer() {}

	void SetName(const char *name) { m_name = name; }
	std::string &GetName() { return m_name; }
	void SetParam(DWORD param1, DWORD param2) { m_param1 = param1; m_param2 = param2; }
	void GetParam(DWORD &param1, DWORD &param2) { param1 = m_param1; param2 = m_param2; }

private:
	std::string m_name;
	DWORD m_param1, m_param2;

public:
	virtual bool Init(const char *ip, int port, bool encrypt, CG_NetServerNotify *notify) = 0;
	virtual void Process() = 0;
	virtual void Close() = 0;  

	// set max client,max accept per cycle
	// ip access list
	// 
};

class CG_NetDevice
{
public:
	CG_NetDevice();
	virtual ~CG_NetDevice();

    static CG_NetDevice *GetInstance();
	bool Init();
	void Tick(long delta);
	void Exit();

	SNetLayerInfo *GetNetLayerInfo();
	CG_NetClient *CreateClient(int protocal);
	CG_NetServer *CreateServer(int protocal);
	void DestroyClient(CG_NetClient *client);
	void DestroyServer(CG_NetServer *server);

private:
	static CG_NetDevice *m_device;
};
// -----------------------------------------------------------------------------
// new network interface end
// -----------------------------------------------------------------------------

const int	MAX_DEBUG_STR_LEN = 128;
#define		NET_LIB "NetLib"

#include "g_netsocket.h"
#include "g_cmdpacket.h"
#include "g_netpacket.h"
#include "g_udpsession.h"
#include "g_tcpsession.h"
#include "g_tcplistener.h"

#endif
