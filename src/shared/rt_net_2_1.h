#ifndef rt_net_2_1_h__
#define rt_net_2_1_h__

/************************************************************************/
/* This class use to adope rt_net_1 to rt_net_2 interface by louis     */
/************************************************************************/

#include "rt_net.h"

enum NetState
{
	NET_STATE_ZERO,
	NET_STATE_DEAD
};

class CG_TCPSession
{
public:
	CG_TCPSession();
	~CG_TCPSession();

	//////////////////////////////////////////////////////////////////////////
	// Interfaces
	static void Process();

	void EnableCompress(bool bCom);
	bool Connect(const char* m_szHost, DWORD m_sPort);
	bool SendPacket(CG_CmdPacket* pPacket);
	int  GetState();

	// Interfaces for logging
	int	 GetAvgFluxIn();
	int  GetAvgFluxOut();
	int  GetBytesIn();
	int  GetBytesOut();
	int  GetSendCompressRate();
	int  GetRecvCompressRate();
	void Close(DWORD dwTime = 500);

	

	//////////////////////////////////////////////////////////////////////////
	// Callback events
	virtual void OnConnect(bool bSucceed) = 0;
	virtual void OnDisconnect() = 0;

private:
	CG_NetClient*			m_pNetClient;
	CG_NetServer*			m_pNetServer;

	CG_NetClientNotify*		m_pClientNotify;
	CG_NetServerNotify*		m_pServerNotify;

	bool		m_bCompress;
	int			m_iState;
};

#endif // rt_net_2_1_h__