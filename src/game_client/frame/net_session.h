
#include "net/g_tcpsession.h"
#ifndef NET_SESSION_H
#define NET_SESSION_H

extern short g_bUnWg;


/*------------------------------------------------------------------------
-   Net Command Callback.
------------------------------------------------------------------------*/

typedef bool (*NetCmdCB)(short cmdID, CG_CmdPacket *pPacket);

/*------------------------------------------------------------------------
-   Class NetSession.
------------------------------------------------------------------------*/

class NetSession : public CG_TCPSession  
{
public:
    NetSession();
    virtual ~NetSession();

    void InitOnce();

public:
    void Process(float fSecond);
    void SetConnectInfo(const char* szHostIPAddress, int iPort, int iMaxReConnect=3);
    void GetRegionConnectInfo();
    bool Connect();

    CG_CmdPacket* BeginWrite();
    bool Send(CG_CmdPacket* pPacket);

    bool IsBind(short cmdID);
    void BindCmd(short cmdID, NetCmdCB pFun);
    void UnbindCmd(short cmdID);

    void ResetDisconnectCnt();

protected:
    virtual void OnReceive(CG_CmdPacket *packet);
    virtual void OnDisconnect();
    virtual void OnConnect(bool bSucceed);
    bool DoCmd(short cmdID, CG_CmdPacket *pPacket);

protected:
    RtString                        m_szUsername;   // 登录用户名
    CG_CmdPacket                    m_SendPacket;   // 要发送的消息
    EXT_SPACE::unordered_map<short, NetCmdCB>  m_Bind;         // 绑定命令

    RtString                        m_szHost;
    int                             m_sPort;
    int                             m_iDisconnectCnt;
    int                             m_iMaxReConnect;
    float                           m_fNextConnectTime;

};

/////////////////////////////////////////////////////////////////////////////////
// 网络

CG_CmdPacket* NetBeginWrite();
bool NetSend(CG_CmdPacket* pPacket);
bool NetSendEx(CG_CmdPacket* pPacket);

bool NetIsBind(short cmdID);
void NetBindCmd(short cmdID, NetCmdCB pFun);
void NetUnbindCmd(short cmdID);


#endif//NET_SESSION_H
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
