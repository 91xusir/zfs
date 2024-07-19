
#include "region.h"
#include "jhwg_session.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CJhWgSession::CJhWgSession()
{
    m_iCurSn = 1;
    m_bLogined = false;
    m_tDisconnect = 0;
    this->SetDebugStr("JhWgSession");
}

CJhWgSession::~CJhWgSession()
{
}

bool CJhWgSession::SetConnectServer(const char* szRemoteIP, int iRemotePort,
                                 const char* szLoginName, const char* szGameworldName, short sRegionIdx,
                                 const char* szBindIP, int iBindPort)
{
    m_szRemoteIP = szRemoteIP;
    m_iRemotePort = iRemotePort;
    m_szBindIP = szBindIP;
    m_iBindPort = iBindPort;
    m_tDisconnect = 0;

    m_szLoginName = szLoginName;
    m_szGameworldName = szGameworldName;
    m_sRegionIdx = sRegionIdx;
    return true;
}

void CJhWgSession::ProcessNet()
{
    if (!m_szRemoteIP.empty())
    {
        time_t tNow = time(NULL);
        Process();
        if (IsClosed())
        {
#ifdef _WIN32
            if ((tNow-m_tDisconnect) > 5) // Windows, 每5秒重连一次
#else
            if ((tNow-m_tDisconnect) > 60) // Linux,  每60秒重连一次
#endif
            {
                m_tDisconnect = tNow;
                const char* bindip = m_szBindIP.empty()?NULL:m_szBindIP.c_str();
                Connect((char*)m_szRemoteIP.c_str(), m_iRemotePort, (char*)bindip, m_iBindPort);
            }
        }
    }
}

void CJhWgSession::OnConnect(bool ret)
{
    if (ret)
    {
        ERR1("Connect jhwg server SUCCEEDED at %d\n", rtTimestamp());
#ifdef WIN32
        LOG(""); // 校正Windows下输出的颜色
#endif
        m_bLogined = false;
        LoginToServer(m_szLoginName.c_str(), m_szGameworldName.c_str(), m_sRegionIdx);
    }else
    {
        MSG("Connect jhwg server FAILED\n");
    }
}

void CJhWgSession::OnDisconnect()
{
    ERR1("Disconnect from jhwg server at %d\n", rtTimestamp());
    m_bLogined = false;
}

void CJhWgSession::OnReceive(CG_CmdPacket* packet)
{
    short cmdID;
    short sProgramID;
    long  lActorID;
    long  lSecond;
    char* pMsg;

    if (packet->ReadShort(&cmdID))
    {
        switch (cmdID)
        {
        case 201: // 登录返回
            {
                char cResult;
                if (!packet->ReadByte(&cResult))
                {
                    break; // 错误
                }

                if (cResult==1) // 登陆成功
                {
                    m_bLogined = true;

                    long lCode;
                    short i, sCount;
                    if (packet->ReadShort(&sCount))
                    {
                        g_region->m_iClientCheckCodeCnt = 0;
                        for (i=0; i<sCount; i++)
                        {
                            if (!packet->ReadLong(&lCode))
                            {
                                break;
                            }
                            g_region->m_dwClientCheckCode[g_region->m_iClientCheckCodeCnt] = lCode;
                            g_region->m_iClientCheckCodeCnt ++;
                        }
                    }
                }else
                {
                    Close();
                }
            }
            break;

        case 202: // 处理用户
            {
                packet->ReadLong(&lActorID);
                packet->ReadShort(&sProgramID);
                packet->ReadLong(&lSecond);
                packet->ReadString(&pMsg);
                OnUserProgram(lActorID, sProgramID, lSecond, pMsg);
            }
            break;
        }
    }
}

bool CJhWgSession::EndSend()
{
    if (IsConnected() && m_bLogined)
    {
        return SendPacket(&m_sendCmd);
    }
    return false;
}

void CJhWgSession::LoginToServer(const char* szLoginName, const char* szGameworldName, short sRegionIdx)
{
    CG_CmdPacket& p = BeginSend(101);
    p.WriteString((char*)szLoginName);
    p.WriteString((char*)szGameworldName);
    p.WriteShort(sRegionIdx);
    EndSend();
}

void CJhWgSession::CheckedUser(const char* szAccountName, const char* szActorName, long lActorID, long lCheckCnt)
{
    CG_CmdPacket& p = BeginSend(102);
    p.WriteString((char*)szAccountName);
    p.WriteString((char*)szActorName);
    p.WriteLong(lActorID);
    p.WriteLong(lCheckCnt);
    EndSend();
}

void CJhWgSession::UpdateStatus(long lTotalUser, long lCheckedUser)
{
    CG_CmdPacket& p = BeginSend(103);
    p.WriteLong(lTotalUser);
    p.WriteLong(lCheckedUser);
    EndSend();
}

void CJhWgSession::OnUserProgram(long lActorID, short sProgramID, long lSecond, const char* szMsg)
{
    g_region->OnJhWgServerUserProgram(lActorID, sProgramID, lSecond, szMsg);
}
