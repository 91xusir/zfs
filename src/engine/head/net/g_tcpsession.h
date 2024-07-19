//
// g_tcpsession.h: interface for the CG_TCPSession class.
//
#ifndef __CG_TCP_SESSION_H__
#define __CG_TCP_SESSION_H__

#include "g_netpacket.h"
#include "g_netsocket.h"

const int MAX_BUFFER_SIZE		= MAX_NET_PACKET_SIZE;
const int MAX_NET_SEED_SIZE		= 16;

enum ENetState
{
	NET_STATE_ZERO			= 0,
	NET_STATE_CONNECTING	= 1,
	NET_STATE_WAITING_SEED	= 2,
	NET_STATE_CONNECTED		= 3,
	NET_STATE_DEAD			= 4,
};

const int NET_CONNECT_TIMEOUT		= 30*1000;
const int NET_KEEP_ALIVE_INTERVAL	= 30*1000;
#ifdef _DEBUG
	const int NET_TIMEOUT			= 900*1000;
#else
	const int NET_TIMEOUT			= 90*1000;
#endif

class CG_TCPSession  
{
public:
	CG_TCPSession();
	CG_TCPSession(const CG_TCPSession &object);
	virtual ~CG_TCPSession();
	
	CG_NetClient *m_client;
	CG_NetClientNotify *m_notify;
	void SetNotify(CG_NetClient *client, CG_NetClientNotify *notify)
	{
		m_client = client;
		m_notify = notify;
	}
	void Attach(SOCKET socket);
	bool GetPacket(CG_CmdPacket *packet);
	bool IsDead();
	bool Connect(const char *remote,int port,char *bindIp=0,int bindPort=0);
	bool SendPacket(CG_CmdPacket *packet,bool flush=true,bool sys=false);
	
	int  GetRtt();
	int  GetState();
	int  GetBytesOut();
	int  GetBytesIn();
	int  GetAvgFluxOut();
	int  GetAvgFluxIn();
	
	float GetRecvCompressRate()
	{
		if(m_rowRecvBytes == 0) return 0;
		return (m_rowRecvBytes - GetBytesIn()) / ((float)m_rowRecvBytes);
	}
	float GetSendCompressRate()
	{
		if(m_rowSendBytes == 0) return 0;
		return (m_rowSendBytes - GetBytesOut()) / ((float)m_rowSendBytes);
	}
	
	void DefProc();
	void Reset();
	void Close();
	void Process();
	void SetBufSize(int kb);
	void SendSeed(long seed);
	
	virtual void OnReceive(CG_CmdPacket *packet);
	virtual void OnDisconnect();
	virtual void OnConnect(bool ret);

	void KeepAlive(long now);

	CG_NetSocket& GetSocket() { return m_socket; }
	void EnableCompress(bool compress) { m_bCompress = compress; }
	void EnableRemoteCompress(bool compress) { m_bRemoteCompress = compress; }
	void SetDebugStr(const char *str);

	// for run old code
	// void Replace(CG_TCPSession *old);

private:
	int  HandleSysPacket(CG_CmdPacket *packet);
	bool RecvPacket(CG_NetPacket *packet);
	bool ReadData();
	bool PushData(char *buf,int len);
	bool ResizeBuf(int min);
	void SetSeed(long seed);
	void FlushData();

public:
	CG_NetSocket  m_socket;
	/* shared packet */
	static CG_CmdPacket  m_sendCmdPkt;	
	static CG_CmdPacket  m_recvCmdPkt;
	static CG_NetPacket  m_sendNetPkt;	
	static CG_NetPacket  m_recvNetPkt;
	
	ENetState	  m_state;
	unsigned long m_startTime;
	unsigned long m_lastRecvTime;
	unsigned long m_lastSendTime;
	unsigned long m_connTime;
	unsigned long m_waitSeedTime;
	
	char *m_readBuf;
	int	  m_readBufSize;
	int   m_readPos;
	int   m_readLen;
		
	char *m_writeBuf;
	int   m_writeBufSize;
	int	  m_writeLen;
	int   m_writeBeg;
	int	  m_writeEnd;
	
	//char m_seed[MAX_NET_SEED_SIZE];
	CM_MEMDEF(m_seed, MAX_NET_SEED_SIZE)
	int  m_seedLen;
	int  m_maxBufSize;
	
	bool m_bEncrypt;
	bool m_bCompress;
	bool m_bRemoteCompress;
	unsigned short m_sendSeq;
	unsigned short m_recvSeq;

	int   m_rowSendBytes;
	int   m_rowRecvBytes;
	int   m_sendBytes;
	int   m_recvBytes;
	int   m_rtt;

	//char  m_debugStr[MAX_DEBUG_STR_LEN];
	CM_MEMDEF(m_debugStr, MAX_DEBUG_STR_LEN)
	int   m_comDelay;
};

class CG_NetClientTcp : public CG_NetClient
{
public:
	CG_NetClientTcp();
	virtual ~CG_NetClientTcp();

	virtual bool Init(const char *bindIp, int bindPort, CG_NetClientNotify *notify);
	virtual bool Connect(const char *remoteIp, int remotePort);
	virtual void ReConnect(DWORD delay);
	virtual void Process();
	virtual void Close(DWORD delay);
	virtual bool CanClose();
	virtual bool IsClosing();
	virtual void SendPacket(CG_CmdPacket *packet);

    void SetUserData1(DWORD dwUserData) { m_dwUserData1 = dwUserData; }
    void SetUserData2(DWORD dwUserData) { m_dwUserData2 = dwUserData; }
    DWORD GetUserData1()                { return m_dwUserData1; }
    DWORD GetUserData2()                { return m_dwUserData2; }

	CG_TCPSession *m_session;

	std::string m_remoteIp, m_bindIp;
	int m_remotePort, m_bindPort;

	// used in client mode
	DWORD m_reConnTime;
	CG_NetClientNotify *m_notify;

	// used in server mode
	DWORD m_closeDelay;
    DWORD m_dwUserData1;
    DWORD m_dwUserData2;
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
*/

class CG_TCPListener;
class CG_NetServerTcp : public CG_NetServer
{
public:
	CG_NetServerTcp();
	virtual ~CG_NetServerTcp();

	virtual bool Init(const char *ip, int port, bool encrypt, CG_NetServerNotify *notify);
	virtual void Process();
	virtual void Close();  

	// virtual void CloseClient(CG_NetClient *client, DWORD delay); 
	CG_NetServerNotify *m_notify;
	CG_TCPListener *m_listener;
	std::list<CG_NetClientTcp*> m_clientList;
	CG_CmdPacket m_packet;
};

/*
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

#endif
