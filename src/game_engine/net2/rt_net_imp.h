#ifndef __RT_NET_IMP_H__
#define __RT_NET_IMP_H__
/*
Interface:
	NetDevice 
	NetServer 
	NetClient

Implement:
	NetDevice 
		NetDeviceImp 
	NetServer
		NetServerImp
			NetServerTcp
			NetServerUdp
	NetClient
		NetClientImp
			NetClientTcp
			NetClientUdp
*/
#include "net2/rt_net.h"

#define MAX_DEBUG_STR_LEN		128
#define	NET_LIB					"NetLib"
#define NET_LIB_VERSION         100

class CG_NetPacket;
class CG_NetClientImp;
class CG_NetServerImp;
class CG_NetDeviceImp;

class CG_NetClientTcp;
class CG_NetServerTcp;

class CG_NetClientImp : public CG_NetClient
{
public:
	CG_NetClientImp() 
	{
		m_bDebugInfo = true;
		m_bCompress  = true;
		m_bRemoteCompress = true;
	};
	virtual ~CG_NetClientImp() {};

	// net client interface
	void SetParam(DWORD param1, DWORD param2) { m_param1 = param1; m_param2 = param2; }
	void GetParam(DWORD &param1, DWORD &param2) { param1 = m_param1; param2 = m_param2; }
	DWORD m_param1, m_param2;

	void EnableCompress(bool local, bool remote) { m_bCompress = local; m_bRemoteCompress = remote; }
	bool              m_bDebugInfo;
	bool              m_bCompress;
	bool              m_bRemoteCompress;

	// private interface
	virtual void Process() = 0;
	virtual void Destroy() = 0;
	virtual bool CanClose()	= 0;
	virtual bool IsClosing() = 0;
	virtual CG_NetDeviceImp *GetDevice() = 0;
	virtual void SetDevice(CG_NetDeviceImp *dev) = 0;
};

class CG_NetServerImp : public CG_NetServer
{
public:
	CG_NetServerImp() 
	{
		m_bCompress  = true;
		m_bRemoteCompress = true;
		m_maxClientNum = -1;
	};
	virtual ~CG_NetServerImp() {};

	// net client interface
	void SetParam(DWORD param1, DWORD param2) { m_param1 = param1; m_param2 = param2; }
	void GetParam(DWORD &param1, DWORD &param2) { param1 = m_param1; param2 = m_param2; }
	DWORD m_param1, m_param2;

	void EnableDebugInfo(bool debug, const char *title) { m_bDebugInfo = debug; }
	void EnableCompress(bool local, bool remote) { m_bCompress = local; m_bRemoteCompress = remote; }
	void GetNetInfo(SNetInfo &info) {}
	bool              m_bDebugInfo;
	bool              m_bCompress;
	bool              m_bRemoteCompress;

	void AddAllowableIp(string from, string to) {}
	void SetMaxClient(int num) { m_maxClientNum = num; }
	int m_maxClientNum;
	vector<long> m_allowableIpList;

	// private interface
	virtual void Process() = 0;
	virtual void Destroy() = 0;
	virtual bool CanClose() = 0;
	virtual bool IsClosing() = 0;
	virtual CG_NetDeviceImp *GetDevice() = 0;
	virtual void SetDevice(CG_NetDeviceImp *dev) = 0;
};

class CG_NetDeviceImp : public CG_NetDevice
{
public:
	CG_NetDeviceImp();
	virtual ~CG_NetDeviceImp();

	bool Init();
	void Tick(long delta);
	void Exit();

	void GetDeviceInfo(SDeviceInfo &info) { info = m_deviceInfo; }
	CG_NetClient *CreateClient(int protocal);
	CG_NetServer *CreateServer(int protocal);

	CG_CmdPacket *GetSendCmdPacket() { return m_sendCmdPkt; }
	CG_CmdPacket *GetRecvCmdPacket() { return m_recvCmdPkt; }

	CG_NetPacket *GetSendNetPacket() { return m_sendNetPkt; }
	CG_NetPacket *GetRecvNetPacket() { return m_recvNetPkt; }

private:
	std::list<CG_NetClientImp*> m_clientList;
	std::list<CG_NetServerImp*> m_serverList;
	CG_CmdPacket  *m_sendCmdPkt;	
	CG_CmdPacket  *m_recvCmdPkt;
	CG_NetPacket  *m_sendNetPkt;	
	CG_NetPacket  *m_recvNetPkt;
	SDeviceInfo    m_deviceInfo;
};

/*
class CG_NetClientUdp : public CG_NetClient
{  
public:
	CG_NetClientUdp();
	virtual ~CG_NetClientUdp();

	virtual bool Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify);
	virtual bool Connect(const char *remoteIp, int remotePort);
	virtual void ReConnect(DWORD delay);
	virtual void Process();
	virtual void Close(DWORD delay);
	virtual bool CanClose();
	virtual bool IsClosing();
	virtual void SendPacket(CG_CmdPacket *packet);

	CG_UDPSession *m_session;

	string m_remoteIp, m_bindIp;
	int m_remotePort, m_bindPort;

	// used in client mode
	CG_NetClientNotify *m_notify;

	// used in server mode
	DWORD m_closeDelay;
};

class CG_NetServerUdp : public CG_NetServer
{
public:
	CG_NetServerUdp();
	virtual ~CG_NetServerUdp();

	virtual bool Init(const char *ip, int port, bool encrypt, CG_NetServerNotify *notify);
	virtual void Process();
	virtual void Close();  

	// virtual void CloseClient(CG_NetClient *client, DWORD delay); 
	CG_NetServerNotify *m_notify;
	CG_UDPSession  *m_session;
	
	list<CG_NetClientUdp*> m_clientList;
	CG_CmdPacket m_packet;
};
*/

#include "g_netsocket.h"
#include "g_netpacket.h"
#include "g_udp_imp.h"
#include "g_tcp_imp.h"

#endif
