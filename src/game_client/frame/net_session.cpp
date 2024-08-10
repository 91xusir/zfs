
#include "gc_include.h"
#include "region_client.h"
#include "gc_login_session.h"
#include "gc_constant.h"
#include "ui_form_msg.h"


short g_bUnWg = false;

/*------------------------------------------------------------------------
-   Class NetSession.
------------------------------------------------------------------------*/

NetSession::NetSession()
{
    m_iDisconnectCnt = 0;
    this->EnableCompress(false);
}

NetSession::~NetSession()
{
}

void NetSession::InitOnce()
{
    m_iDisconnectCnt = 0;
    net_register();
}

void NetSession::OnReceive(CG_CmdPacket *packet)
{
    guard;

	if(g_bUnWg)
	{
		int size = 1024*20;
		unsigned char buf[1024*20];
		char* pBuf;

		packet->ReadBinary(&pBuf, &size);
		if(size>1024*20-1)
		{
			//CHECK(0);
		}
		memcpy(buf, pBuf, size);

		if(NES_Dispatch_C(1, buf, size))
		{
			//CHECK(0);
		}

		packet->LoadFromBuffer((char*)buf, size);
	}

    short CommandID;
    if(!packet->ReadShort(&CommandID)) return;
    this->DoCmd(CommandID, packet);
    return;
    unguard;
}

void NetSession::OnConnect(bool bSucceed)
{
	guard;
    LOG2("网络:     连接游戏服务器 [%s] (%d)\n", bSucceed?R(G_SUCCEED):R(G_FAIL), m_iMaxReConnect);
    if (bSucceed)
    {
        m_iMaxReConnect = -1;
        GetWorld()->OnNetConnect(bSucceed);
    }else
    {
        if (m_iMaxReConnect==0)
        {
            GetWorld()->OnNetConnect(bSucceed);
        }
    }
	unguard;
}

int cos_table11(unsigned char* buf, int bufSize, int maxBuf, int code)
{
    char *pS, *pD;
    pS = ((char*)buf)+bufSize-1;
    pD = ((char*)buf)+bufSize+3;
    int i;
    for (i=0; i<bufSize; i++)
    {
        *pD = *pS;
        pS --;
        pD --;
    }
    pD = (char*)buf;
    pS = (char*)&code;
    for (i=0; i<4; i++)
    {
        *pD = *pS;
        pS ++;
        pD ++;
    }
    return bufSize+4;
}

bool wgCheckClient(unsigned char* buf, int& iDataSize, int iBufSize)
{
    /*
    void *ppEBP, *ppRet;
    int iLev, iMaxLev;
    unsigned long iCode = 0;
    iMaxLev = 7;

    FILE* fp = fopen("ttttt.txt", "wt");
    void** pStackBase;
    _asm { mov dword ptr [pStackBase],ebp }
    pStackBase = (void**)pStackBase[0];
    pStackBase = (void**)pStackBase[0];
    for (iLev=0; iLev<iMaxLev; iLev++)
    {
        iCode += (unsigned long)pStackBase[1];
        pStackBase = (void**)pStackBase[0];
        fprintf(fp, "%08X\n", pStackBase);
        fflush(fp);
        if (pStackBase==0)
        {
            break;
        }
    }
    fprintf(fp, "end\n", pStackBase);
    fclose(fp);
    iDataSize = cos_table11(buf, iDataSize, iBufSize, 0);
    */

    void** pStackBase;
    _asm { mov dword ptr [pStackBase],ebp }
    int iRetSize = 0;
    const char* szMsg = Lua()->CallLFunc("c", "iiii:i", buf, iDataSize, (int)pStackBase, iBufSize, &iRetSize);
    if (szMsg==NULL)
    {
        iDataSize = iRetSize;
    }

#ifdef _DEBUG
    if (szMsg)
    {
        LOG1("LuaError: %s\n", szMsg);
    }
#endif
    return true;
}

void NetSession::OnDisconnect()
{
	guard;
	if(g_bUnWg)
	{
		NES_Finalize_C();
		g_bUnWg = 0;
	}

    m_iDisconnectCnt ++;
    LOG2("网络:     游戏服务器连接中断 (%d,%d)\n", m_iMaxReConnect, m_iDisconnectCnt);
    if (m_iMaxReConnect==-1)
    {
        m_iMaxReConnect = 3;
    }
    if (m_iMaxReConnect==0 || m_iDisconnectCnt>5)
    {
        GetWorld()->OnNetDisconnect();
    }
	unguard;
}

void NetSession::ResetDisconnectCnt()
{
    m_iDisconnectCnt = 0;
}

void NetSession::Process(float fSecond)
{
    guard;
    int iNetState = this->GetState();
    if (iNetState==NET_STATE_ZERO || iNetState==NET_STATE_DEAD)
    {
        m_fNextConnectTime -= fSecond;
        if (m_fNextConnectTime>=0)
        {
            if (m_iMaxReConnect>0)
            {
                m_iMaxReConnect --;
                m_fNextConnectTime = 1.f; // 每秒只能连一次
                GetRegionConnectInfo();
                LOG3("网络: 连接游戏服务器 IP=[%s] Port=[%d] (%d)...\n", m_szHost, m_sPort, m_iMaxReConnect);
                this->EnableCompress(false);
                CG_TCPSession::Connect((char*)m_szHost, m_sPort);
            }
        }
    }else
    {
        CG_TCPSession::Process();
    }
    unguard;
}

void NetSession::SetConnectInfo(const char* szHostIPAddress, int iPort, int iMaxReConnect)
{
    m_sPort = iPort;
    m_szHost = szHostIPAddress;
    m_iMaxReConnect = iMaxReConnect;
}

void NetSession::GetRegionConnectInfo()
{
    long lRegionCharID;
    long lRegionSeed;
    char* szRegionHost;
    short sRegionPort;
    GetRegionServerInfo(lRegionCharID, lRegionSeed, &szRegionHost, sRegionPort);

    m_szHost = szRegionHost;
    m_sPort = sRegionPort;
}

bool NetSession::Connect()
{
	guard;
    int iNetState = this->GetState();

    if (iNetState==NET_STATE_ZERO || iNetState==NET_STATE_DEAD)
    {
        m_fNextConnectTime = 1.f; // 每秒只能连一次
        GetRegionConnectInfo();
        LOG3("网络: 连接游戏服务器 IP=[%s] Port=[%d] (%d)...\n", m_szHost, m_sPort, m_iMaxReConnect);
        this->EnableCompress(false);
        if(CG_TCPSession::Connect((char*)m_szHost, m_sPort))
        {
            return true;
        }else
        {
            m_iMaxReConnect = 0;
        }
    }else
    {
        LOG("警告: 连接已经建立或者正在建立, 目前不能接受Connect命令.\n");
    }
    return false;
	unguard;
}

CG_CmdPacket* NetSession::BeginWrite()
{
	guard;
    m_SendPacket.BeginWrite();
    return &m_SendPacket;
	unguard;
}

bool NetSession::Send(CG_CmdPacket* pPacket)
{
	guard;

	unsigned char buf[1024*20];
	int size = 1024*20;
	if(g_bUnWg)
	{
		pPacket->SaveToBuffer((char*)buf, size);

		size = sizeof(buf);
		if(NES_Dispatch_C(0, buf, size))
		{
			UIFormMsg::ShowStatic(R(MSG_CLIENT_SENTENCRYPTIONBAGFAIL), UIFormMsg::TYPE_OK_CANCEL);
			return false;
		}

		pPacket->BeginWrite();
		pPacket->WriteShort(c2r_wg_data);
		pPacket->WriteBinary((char*)buf, size);

		NES_Dispatch_C(2, buf, size);
	}
    else if (GetWorld()->m_cUserJhWg)
    {
		pPacket->SaveToBuffer((char*)buf, size);
        if (!wgCheckClient(buf, size, 1024*20))
            return false;
		pPacket->BeginWrite();
		pPacket->WriteShort(c2r_wg_data);
		pPacket->WriteBinary((char*)buf, size);
    }

    return this->SendPacket(pPacket);
	unguard;
}

bool NetSession::IsBind(short cmdID)
{
    return (m_Bind.find(cmdID)!=m_Bind.end());
}

void NetSession::BindCmd(short cmdID, NetCmdCB pFun)
{
	guard;
    m_Bind[cmdID] = pFun;
	unguard;
}

void NetSession::UnbindCmd(short cmdID)
{
	guard;
    unordered_map<short, NetCmdCB>::iterator it;
    it = m_Bind.find(cmdID);
    if (it!=m_Bind.end())
    {
        m_Bind.erase(it);
    }
	unguard;
}

bool NetSession::DoCmd(short cmdID, CG_CmdPacket *pPacket)
{
	guard;
    unordered_map<short, NetCmdCB>::iterator it;
    it = m_Bind.find(cmdID);
    if (it!=m_Bind.end())
    {
        return ((*it).second)(cmdID, pPacket);
    }
    return false;
	unguard;
}

/////////////////////////////////////////////////////////////////////////////////
// 网络

CG_CmdPacket* NetBeginWrite()
{
    return GetNet()->BeginWrite();
}

bool NetSend(CG_CmdPacket* pPacket)
{
    if (GetWorld()->m_bLogined)
    {
        return GetNet()->Send(pPacket);
    }
   //lyymark  warning C4715: “NetSend”: 不是所有的控件路径都返回值
    return false;
}

bool NetSendEx(CG_CmdPacket* pPacket)
{
    return GetNet()->Send(pPacket);
}

bool NetIsBind(short cmdID)
{
    return GetNet()->IsBind(cmdID);
}

void NetBindCmd(short cmdID, NetCmdCB pFun)
{
    GetNet()->BindCmd(cmdID, pFun);
}

void NetUnbindCmd(short cmdID)
{
    GetNet()->UnbindCmd(cmdID);
}

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
