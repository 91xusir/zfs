
#include "login.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CGamigoADBSession::CGamigoADBSession()
{
    m_pCmdBuf = NULL;
    m_iCmdBufSize = 0;
    m_ppCmdParameter = NULL;
    m_iCmdParameterSize = 0;
    m_iCmdParameterCnt = 0;
    this->SetDebugStr("GamigoADBSession");
}

CGamigoADBSession::~CGamigoADBSession()
{
    if (m_pCmdBuf)
    {
        delete[] m_pCmdBuf;
    }
    if (m_ppCmdParameter)
    {
        delete[] m_ppCmdParameter;
    }
}

bool CGamigoADBSession::ConnectServer(const char* szRemoteIP, int iRemotePort, const char* szBindIP, int iBindPort)
{
    m_szRemoteIP = szRemoteIP;
    m_iRemotePort = iRemotePort;
    m_szBindIP = szBindIP;
    m_iBindPort = iBindPort;
    m_tDisconnect = time(NULL);
    return Connect((char*)m_szRemoteIP.c_str(), m_iRemotePort, (char*)m_szBindIP.c_str(), m_iBindPort);
}

void CGamigoADBSession::ProcessNet()
{
    if (!m_szRemoteIP.empty())
    {
        time_t tNow = time(NULL);
        Process();
        if (IsClosed())
        {
            if ((tNow-m_tDisconnect) > 5) // 每5秒重连一次
            {
                m_tDisconnect = tNow;
                Connect((char*)m_szRemoteIP.c_str(), m_iRemotePort, (char*)m_szBindIP.c_str(), m_iBindPort);
            }
        }
    }
}

void CGamigoADBSession::OnConnect(bool ret)
{
    if (ret)
    {
        ERR1("Connect gamigo adb agent SUCCEEDED at %d\n", rtTimestamp());
#ifdef WIN32
        LOG(""); // 校正Windows下输出的颜色
#endif
    }else
    {
        MSG("Connect gamigo adb agent FAILED\n");
    }
}

void CGamigoADBSession::OnDisconnect()
{
    ERR1("Disconnect from gamigo adb agent at %d\n", rtTimestamp());
}

void CGamigoADBSession::OnReceive(CG_CmdPacket* packet)
{
    short cmdID;
    if (packet->ReadShort(&cmdID))
    {
        if (cmdID==2001)
        {
            char* pBuf;
            char seps[] = " \t\n";
            char *pToken;

            packet->ReadString(&pBuf);
#ifdef _DEBUG
            MSG1("From gamigo: %s\n", pBuf);
#endif

            if ((int)strlen(pBuf)+1>m_iCmdBufSize)
            {
                delete[] m_pCmdBuf;
                m_iCmdBufSize = (int)strlen(pBuf)+1;
                m_pCmdBuf = new char[m_iCmdBufSize];
            }
            strcpy(m_pCmdBuf, pBuf);

            m_iCmdParameterCnt = 0;
            pToken = strtok(m_pCmdBuf, seps);
            while( pToken != NULL )
            {
                if (m_iCmdParameterCnt>=m_iCmdParameterSize)
                {
                    char** pp = new char*[m_iCmdParameterSize+10];
                    memcpy(pp, m_ppCmdParameter, sizeof(char*)*m_iCmdParameterSize);
                    m_iCmdParameterSize += 10;
                    delete[] m_ppCmdParameter;
                    m_ppCmdParameter = pp;
                }
                m_ppCmdParameter[m_iCmdParameterCnt++] = pToken;
                pToken = strtok( NULL, seps );
            }
            if (m_iCmdParameterCnt>0)
            {
                if (strcmp(m_ppCmdParameter[0], "Login")==0)
                {
                    g_lgs->OnGamigoAdbLoginCmd(m_iCmdParameterCnt-1, (const char**)(m_ppCmdParameter+1));
                }
            }
        }
    }
}

void CGamigoADBSession::SendCmd(const char* szCmd)
{
#ifdef _DEBUG
    MSG1("To gamigo: %s\n", szCmd);
#endif
    CG_CmdPacket& p = BeginSend();
    p.WriteString((char*)szCmd);
    EndSend();
}

void CGamigoADBSession::SendCmdLogin(const char* szUsername, const char* szPassword, long dwOrderID)
{
    snprintf(g_strStaticBuffer, 1023, "Login %s %s %d", szUsername, szPassword, dwOrderID);
    g_strStaticBuffer[1023] = 0;
    SendCmd(g_strStaticBuffer);
}
