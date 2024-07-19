#include "rt_net_imp.h"
#include "core/rt2_core.h"

CG_NetDevice* CreateNetDevice()
{
	return RT_NEW CG_NetDeviceImp;
}

void DestroyNetDevice(CG_NetDevice *dev)
{
	//delete dev;
	DEL_ONE( dev );
}

// ------------------------------------------------------------------------------
// net device
// ------------------------------------------------------------------------------
CG_NetDeviceImp::CG_NetDeviceImp()
{
	m_sendCmdPkt = NULL;
	m_recvCmdPkt = NULL;
	m_sendNetPkt = NULL;
	m_recvNetPkt = NULL;
}

CG_NetDeviceImp::~CG_NetDeviceImp()
{
	DEL_ONE( m_sendCmdPkt );
	DEL_ONE( m_recvCmdPkt );
	DEL_ONE( m_sendNetPkt );
	DEL_ONE( m_recvNetPkt );
}

bool CG_NetDeviceImp::Init()
{
	/*
	if(m_globalNetInfo.m_startTime == 0)
	{
		DWORD dwNow = rtGetMilliseconds();
		m_globalNetInfo.m_startTime     = dwNow;
		m_globalNetInfo.m_secStartTime  = dwNow;
	}
	*/
#ifdef WIN32
	// startup socket 1.1
	int VersionHigh = 1;
	int VersionLow  = 1;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(VersionHigh,VersionLow);
	err=WSAStartup(wVersionRequested, &wsaData);

	// startup failed 
	if (err!=0)									
	{
		RtCoreLog().Error("WSAStartup error\n");
		WSACleanup();
		return false;
	}

	// version error 
	if (LOBYTE(wsaData.wVersion)!= VersionLow ||
		HIBYTE(wsaData.wVersion)!= VersionHigh ) 
	{
		RtCoreLog().Error("WSAStartup version error\n");
		WSACleanup();
		return false;
	}
#endif

	m_sendCmdPkt = RT_NEW CG_CmdPacket;
	m_sendCmdPkt->SetSize(MAX_CMD_PACKET_SIZE,MAX_BIT_MSG_SIZE);

	m_recvCmdPkt = RT_NEW CG_CmdPacket;
	m_recvCmdPkt->SetSize(MAX_CMD_PACKET_SIZE,MAX_BIT_MSG_SIZE);

	m_sendNetPkt = RT_NEW CG_NetPacket;
	m_recvNetPkt = RT_NEW CG_NetPacket;

	CG_NetSocket skt;
	if(!skt.Initialize(NETWORK_PROTOCOL_TCP))
		return false;

	m_deviceInfo.FDSetSize = FD_SETSIZE;
	m_deviceInfo.Version   = NET_LIB_VERSION;
	m_deviceInfo.SendBufferSize = skt.GetSendBufferSize();
	m_deviceInfo.RecvBufferSize = skt.GetRecvBufferSize();
    	
	RtCoreLog().Info("net device startup ok\n");
	RtCoreLog().Info("	FDSetSize       = %d\n", m_deviceInfo.FDSetSize);
	RtCoreLog().Info("	Version         = %d\n", m_deviceInfo.Version);
	RtCoreLog().Info("	SendBufferSize  = %d\n", m_deviceInfo.SendBufferSize);
	RtCoreLog().Info("	RecvBufferSize  = %d\n", m_deviceInfo.RecvBufferSize);

	return true;
}

void CG_NetDeviceImp::Tick(long delta)
{
	// process net client
	std::list<CG_NetClientImp*>::iterator itClient;
	for(itClient=m_clientList.begin(); itClient!=m_clientList.end();)
	{
		CG_NetClientImp *cl = *itClient;
		if(cl->CanClose())
		{
			cl->Destroy();
			/*delete cl;*/
			DEL_ONE( cl );
			itClient = m_clientList.erase(itClient);
			continue;
		}
		if(!cl->IsClosing())
		{
			cl->Process();
		}
		itClient++;
		continue;
	}

	// process net server
	std::list<CG_NetServerImp*>::iterator itServer;
	for(itServer=m_serverList.begin(); itServer!=m_serverList.end();)
	{
		CG_NetServerImp *sv = *itServer;
		if(sv->CanClose())
		{
			sv->Destroy();
			/*delete sv;*/
			DEL_ONE( sv );
			itServer = m_serverList.erase(itServer);
			continue;
		}
		if(!sv->IsClosing())
		{
			sv->Process();
		}
		itServer++;
		continue;
	}
}

void CG_NetDeviceImp::Exit()
{
#ifdef WIN32	
	WSACleanup();
#endif
	/*delete m_sendCmdPkt;
	delete m_recvCmdPkt;
	delete m_sendNetPkt;
	delete m_recvNetPkt;*/
	DEL_ONE( m_sendCmdPkt );
	DEL_ONE( m_recvCmdPkt );
	DEL_ONE( m_sendNetPkt );
	DEL_ONE( m_recvNetPkt );

	std::list<CG_NetClientImp*>::iterator itClient;
	for(itClient=m_clientList.begin(); itClient!=m_clientList.end(); itClient ++ )
	{
		CG_NetClientImp *cl = *itClient;		
		DEL_ONE( cl );
	}
	m_clientList.clear();
	// process net server
	std::list<CG_NetServerImp*>::iterator itServer;
	for(itServer=m_serverList.begin(); itServer!=m_serverList.end(); itServer ++)
	{
		CG_NetServerImp *sv = *itServer;		
		DEL_ONE( sv );
	}
	m_serverList.clear();
	RtCoreLog().Info("net device exit ok\n");
}

CG_NetClient *CG_NetDeviceImp::CreateClient(int protocal)
{
	CG_NetClientImp *client = NULL;
	if(protocal == NETWORK_PROTOCOL_TCP)
	{
		client = RT_NEW CG_NetClientTcp;
	}
	else if(protocal == NETWORK_PROTOCOL_UDP)
	{
		client = NULL;
	}

	if(client)
	{
		client->SetDevice(this);
		m_clientList.push_back(client);
	}
	return client;
}

CG_NetServer *CG_NetDeviceImp::CreateServer(int protocal)
{
	CG_NetServerImp *server = NULL;
	if(protocal == NETWORK_PROTOCOL_TCP)
	{
		server = RT_NEW CG_NetServerTcp;
	}
	else if(protocal == NETWORK_PROTOCOL_UDP)
	{
		server = NULL;
	}

	if(server)
	{
		server->SetDevice(this);
		m_serverList.push_back(server);
	}
	return server;
}


