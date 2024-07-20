
#include "login.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CChargeSession::CChargeSession()
{
    m_iCurSn = 1;
    m_bLogined = false;
    this->SetDebugStr("ChargeSession");
}

CChargeSession::~CChargeSession()
{
}

bool CChargeSession::ConnectServer(const char* szRemoteIP, int iRemotePort, const char* szUsername, const char* szPassword, const char* szBindIP, int iBindPort)
{
    m_szRemoteIP = szRemoteIP;
    m_iRemotePort = iRemotePort;
    m_szBindIP = szBindIP;
    m_iBindPort = iBindPort;
    m_tDisconnect = time(NULL);
    m_szUsername = szUsername;
    m_szPassword = szPassword;
    const char* bindip = m_szBindIP.empty()?NULL:m_szBindIP.c_str();
    return Connect((char*)m_szRemoteIP.c_str(), m_iRemotePort, (char*)bindip, m_iBindPort);
}

void CChargeSession::ProcessNet()
{
    if (!m_szRemoteIP.empty())
    {
        time_t tNow = time(NULL);
        Process();
        if (IsClosed())
        {
            if ((tNow-m_tDisconnect) > 5) // ÿ5������һ��
            {
                m_tDisconnect = tNow;
                const char* bindip = m_szBindIP.empty()?NULL:m_szBindIP.c_str();
                Connect((char*)m_szRemoteIP.c_str(), m_iRemotePort, (char*)bindip, m_iBindPort);
            }
        }
    }
}

void CChargeSession::OnConnect(bool ret)
{
    if (ret)
    {
        ERR1("Connect billing server SUCCEEDED at %d\n", rtTimestamp());
#ifdef WIN32
        LOG(""); // У��Windows���������ɫ
#endif
        m_bLogined = false;
        LoginChargeServer();
    }else
    {
        MSG("Connect billing server FAILED\n");
    }
}

void CChargeSession::OnDisconnect()
{
    ERR1("Disconnect from billing server at %d\n", rtTimestamp());
    m_bLogined = false;
}

void CChargeSession::OnReceive(CG_CmdPacket* packet)
{
    char cmdID;
    char cResult;
    char cType, cVIP;
    long lMsgID;
    long lSn;
    char *pUsername, *pPassword;
    long lPoint;
    long lExpireTime, lNextCheckTime;
    if (packet->ReadByte((char*)&cmdID))
    {
        switch (cmdID)
        {
        case 101: ////////////////////////////////////////// linkresult
            {
                // ����
                if (!packet->ReadByte(&cResult))
                {
                    break; // ����
                }

                // ����
                if (cResult==1) // ��½�ɹ�
                {
                    m_bLogined = true;
                }else
                {
                    Close();
                }
            }
            break;

        case 102: ////////////////////////////////////////// pass
            {
                // ����
                packet->ReadLong(&lSn);
                packet->ReadByte(&cType);
                packet->ReadLong(&lMsgID);
                if (cType==1)
                {
                    packet->ReadLong(&lPoint);
                }else
                {
                    packet->ReadLong(&lExpireTime);
                }
                if (!packet->ReadLong(&lNextCheckTime)) // ע�⣬�⴫�������ǲ�ֵ
                {
                    break; // ����
                }
                lNextCheckTime += (long)time(NULL);

                cVIP = 0; // Ĭ�ϲ���VIP
                packet->ReadByte(&cVIP); // ��ȡ�Ƿ���VIP

                // ����
                SUserInfo* pUserInfo = g_lgs->OnChargePass(lSn, cType, ((cType==1)?lPoint:lExpireTime), cVIP);
                if (pUserInfo)
                {
                    pUserInfo->bReceivePointInfo = true;
                    if (cType==1)
                    {
                        pUserInfo->iChargePoint = lPoint;
                        pUserInfo->tChargeExpire = 0;
                    }else
                    {
                        pUserInfo->iChargePoint = 0;
                        pUserInfo->tChargeExpire = lExpireTime;
                    }
                    pUserInfo->cVIP = cVIP;
                    strncpy(pUserInfo->szChargeMsg, Message(lMsgID), 99);
                    pUserInfo->szChargeMsg[99] = 0;
                    pUserInfo->tChargeNextCheck = lNextCheckTime;
                }
            }
            break;

        case 103: ////////////////////////////////////////// fail
            {
                // ����
                packet->ReadLong(&lSn);
                if (!packet->ReadLong(&lMsgID))
                {
                    break; // ����
                }

                // ����
                g_lgs->OnChargeFail(lSn, Message(lMsgID));
            }
            break;

        case 104: ////////////////////////////////////////// kick
            {
                // ����
                packet->ReadString(&pUsername);
                if (!packet->ReadLong(&lMsgID))
                {
                    break; // ����
                }

                // ����
                g_lgs->KickUserByName(pUsername, KICK_USER_USERDEFINE, (char*)Message(lMsgID));
            }
            break;

        case 105: ////////////////////////////////////////// ������ʾ��Ϣ��
            {
                long lMsgID;
                char* szStr;
                packet->ReadLong(&lMsgID);
                if (!packet->ReadString(&szStr))
                {
                    break; // ����
                }

                m_mapMsgTable[lMsgID] = szStr;
            }
            break;

        case 106: ////////////////////////////////////////// ��ʾ��Ϣ�����
            {
                // ����
                char cMsgType;
                char *szMsgStr;
                packet->ReadString(&pUsername);
                packet->ReadByte(&cMsgType);
                if (!packet->ReadString(&szMsgStr))
                {
                    break; // ����
                }

                // ����
                g_lgs->SendMsgToUser(pUsername, cMsgType, szMsgStr);
            }
            break;

        case 107: ////////////////////////////////////////// �ǳ���Ϣ
            {
                // ����
                long lAccountID, lLogoutOrderID;
                short sServer;
                float fPoint;
                packet->ReadShort(&sServer);
                packet->ReadLong(&lAccountID);
                packet->ReadLong(&lLogoutOrderID);
                if (!packet->ReadFloat(&fPoint))
                {
                    break; // ����
                }

                // ����
                g_lgs->LogLogoutInfo(sServer, lAccountID, fPoint, lLogoutOrderID);
            }
            break;

        case 111: ////////////////////////////////////////// ����ķ���
            {
                char cCmdResult; // 0 ʧ��, 1 ��Ա, 2 ���ֿ� , 3 �������
                long lCmdOrderID;
                packet->ReadByte(&cCmdResult);
                packet->ReadLong(&lCmdOrderID);
                if (cCmdResult==1) // ����ǻ�Ա���򷵻ػ�Ա��Ϣ
                {
                    long  lUserPriv;
                    short sLocked;
                    long  tLockBegin;
                    long  tLockEnd;
                    packet->ReadString(&pUsername);
                    packet->ReadString(&pPassword);
                    packet->ReadLong(&lUserPriv);
                    packet->ReadShort(&sLocked);
                    packet->ReadLong(&tLockBegin);
                    if (!packet->ReadLong(&tLockEnd))
                    {
                        break; // ����
                    }

                    g_lgs->CreateAccount(lCmdOrderID, pUsername, pPassword, lUserPriv, sLocked, tLockBegin, tLockEnd);
                }else if (cCmdResult==3) // �������������򷵻�������Ϣ�������޸�����
                {
                    packet->ReadString(&pUsername);
                    if (packet->ReadString(&pPassword)) // �����Ѿ���MD5�������
                    {
                        g_lgs->SetAccountPassword(NULL, 0, pUsername, pPassword, lCmdOrderID);
                    }
                }
                g_lgs->OnActiveResult(cCmdResult, lCmdOrderID);
            }
            break;

        case 112: ////////////////////////////////////////// �޸�����
            {
                // ����
                packet->ReadString(&pUsername);
                if (!packet->ReadString(&pPassword)) // �����Ѿ���MD5�������
                {
                    break; // ����
                }

                // ����
                g_lgs->SetAccountPassword(NULL, 0, pUsername, pPassword);
            }
            break;

        case 121: ////////////////////////////////////////// ��ֵ�ķ���
            {
                // ����
                long lUserID;
                short sServer;
                char cSucceeded;
                char* szCardID;

                packet->ReadLong(&lSn);
                packet->ReadLong(&lUserID);
                packet->ReadShort(&sServer);
                packet->ReadString(&szCardID);
                packet->ReadByte(&cSucceeded); // 1 Ϊ�ɹ�, 0 Ϊʧ��
                if (!packet->ReadLong(&lMsgID))
                {
                    break; // ����
                }

                // ����
                g_lgs->OnChargeCard(lSn, sServer, lUserID, szCardID, cSucceeded, (char*)Message(lMsgID));
            }
            break;

        case 122: ////////////////////////////////////////// �������
            {
                // ����
                short sServer;
                long lUserID;
                short sLevel;
                short sType;
                char* szGiftString;
                long tLimitTime;
                char cFromCardID;
                char* szCardID;

                packet->ReadLong(&lSn);
                packet->ReadString(&pUsername);
                packet->ReadShort(&sServer);
                packet->ReadLong(&lUserID);
                packet->ReadShort(&sLevel);
                packet->ReadShort(&sType);
                packet->ReadString(&szGiftString);
                packet->ReadLong(&tLimitTime); // ����ʱ��, 0 Ϊ�������� [ע: ���ڻ�û����]
                packet->ReadByte(&cFromCardID);
                if (!packet->ReadString(&szCardID))
                {
                    break; // ����
                }

                // ����
                g_lgs->AddGift(lSn, pUsername, sServer, lUserID, sLevel, sType, szGiftString, tLimitTime, cFromCardID, szCardID);
            }
            break;

        case 123: ////////////////////////////////////////// �������
            {
                // ����
                short sServer;
                long lUserID;
                packet->ReadString(&pUsername);
                packet->ReadShort(&sServer);
                if (!packet->ReadLong(&lUserID))
                {
                    break; // ����
                }

                // ����
                g_lgs->ClearGift(pUsername, sServer, lUserID);
            }
            break;

        case 124: ////////////////////////////////////////// ���ص㿨�Ƿ���Խ���
            {
                // ����
                long lUserID;
                short sServer;
                char cSucceeded;
                long lCardGroupType;    // ��������ͣ�����˵ ������Բ�����Ż�200�㿨 �ȵ�
                char* szCardID;

                packet->ReadLong(&lSn);
                packet->ReadLong(&lUserID);
                packet->ReadShort(&sServer);
                packet->ReadString(&szCardID);
                packet->ReadByte(&cSucceeded); // 1 Ϊ���Խ���, 0 Ϊ�����Խ���
                packet->ReadLong(&lMsgID);     // �����Խ��׵�ԭ��, û��ԭ��д0
                if (!packet->ReadLong(&lCardGroupType))
                {
                    break; // ����
                }

                // ����
                g_lgs->OnCardCanTradeResult(lSn, sServer, lUserID, szCardID, cSucceeded, (char*)Message(lMsgID), lCardGroupType);
            }
            break;

        case 125: ////////////////////////////////////////// �㿨�Ѿ���ֵ
            {
                // ����
                long lCardGroupType;    // ��������ͣ�����˵ ������Բ�����Ż�200�㿨 �ȵ�
                char* szCardID;

                packet->ReadString(&szCardID);
                if (!packet->ReadLong(&lCardGroupType))
                {
                    break; // ����
                }

                // ����
                g_lgs->OnCardCharged(szCardID, lCardGroupType);
            }
            break;

        case 126: ////////////////////////////////////////// ����Ԥ���Ƿ�ɹ�
            {
                // ����
                short sServer;
                short sPoint;
                long lUserID;
                long lAccountID;
                long lDeductOrderID;

                packet->ReadLong(&lSn);
                packet->ReadShort(&sServer);
                packet->ReadLong(&lAccountID);
                packet->ReadLong(&lUserID);
                packet->ReadShort(&sPoint);
                packet->ReadLong(&lDeductOrderID);
                if (packet->ReadByte(&cResult)) // 1 �ɹ�, 0 ʧ��,��ȫ���ܿ۵�, 2 ʧ��,���Կ۳����ֵ���,����������sPoint��
                {
                    break; // ����
                }

                // ����
            }
            break;

        case 127: ////////////////////////////////////////// ���ؿ۵��Ƿ�ɹ�
            {
                // ����
                short sServer;
                short sPoint;
                long lUserID;
                long lAccountID;
                long lDeductOrderID;

                packet->ReadLong(&lSn);
                packet->ReadShort(&sServer);
                packet->ReadLong(&lAccountID);
                packet->ReadLong(&lUserID);
                packet->ReadShort(&sPoint);
                packet->ReadLong(&lDeductOrderID);
                if (!packet->ReadByte(&cResult)) // 1 �ɹ�, 0 ʧ��,û�п��κε�, 2 ʧ��,�۳����ֵ���,����������sPoint��
                {
                    break; // ����
                }

                // ����
            }
            break;

        }
    }
}

void CChargeSession::SendCmd()
{
    if (IsConnected() && m_bLogined)
    {
        while (m_listCmd.size())
        {
            _SendCmd(m_listCmd.front());
            m_listCmd.pop_front();
        }
    }
}

void CChargeSession::SendCmd(SSendCmd& sCmd)
{
    if (IsConnected() && m_bLogined)
    {
        _SendCmd(sCmd);
    }else
    {
        m_listCmd.push_back(sCmd);
    }
}

void CChargeSession::_SendCmd(SSendCmd& sCmd)
{
    CG_CmdPacket& p = BeginSend(sCmd.cCmdID);
    switch (sCmd.cCmdID)
    {
    case 1: // link ������֤
        p.WriteString(sCmd.data.cmdLink.szUsername);
        p.WriteString(sCmd.data.cmdLink.szPassword);
        break;
    case 2: // login ������Ϸ
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdLogin.sServerID);
        p.WriteString(sCmd.data.cmdLogin.szUsername);
        p.WriteLong(sCmd.data.cmdLogin.lRemotehost);
        break;
    case 3: // enter ������Ϸ
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdEnter.sServerID);
        p.WriteString(sCmd.data.cmdEnter.szUsername);
        p.WriteShort(sCmd.data.cmdEnter.sLevel);
        break;
    case 4: // check ״̬��֤
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdCheck.sServerID);
        p.WriteString(sCmd.data.cmdCheck.szUsername);
        p.WriteShort(sCmd.data.cmdCheck.sLevel);
        break;
    case 5: // logout �û��ǳ�
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdLogout.sServerID);
        p.WriteString(sCmd.data.cmdLogout.szUsername);
        p.WriteLong(sCmd.data.cmdLogout.lAccountID);
        p.WriteLong(sCmd.data.cmdLogout.lLogoutOrderID);
        break;
    case 6: // reset ״̬����
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdReset.sServerID);
        break;
    case 11: // active �����ʺ�
        // p.WriteLong(sCmd.lSN);
        p.WriteLong(sCmd.data.cmdActive.lCmdOrderID);
        p.WriteString(sCmd.data.cmdActive.szUsername);
        p.WriteString(sCmd.data.cmdActive.szPassword);
        p.WriteByte(sCmd.data.cmdActive.cType);      // 0 �����ʺ�û���ҵ�����Ҫ����, 1 ��������Ƚϴ���
        break;
    case 21: // Charge Card ��ֵ��
        p.WriteLong(sCmd.data.cmdChargeCard.lUserParam);
        p.WriteShort(sCmd.data.cmdChargeCard.sServerID);
        p.WriteLong(sCmd.data.cmdChargeCard.lBuyerAccountID);
        p.WriteString(sCmd.data.cmdChargeCard.szBuyerAccountName);
        p.WriteLong(sCmd.data.cmdChargeCard.lSalesmanAccountID);
        p.WriteString(sCmd.data.cmdChargeCard.szSalesmanAccountName);
        p.WriteLong(sCmd.data.cmdChargeCard.lBuyerUserID);
        p.WriteString(sCmd.data.cmdChargeCard.szBuyerCharName);
        p.WriteShort(sCmd.data.cmdChargeCard.sLevel);
        p.WriteByte(sCmd.data.cmdChargeCard.cMetier);
        p.WriteByte(sCmd.data.cmdChargeCard.cMetierLev);
        p.WriteByte(sCmd.data.cmdChargeCard.cCardType);
        p.WriteString(sCmd.data.cmdChargeCard.szCardID);   // ����
        p.WriteString(sCmd.data.cmdChargeCard.szCardPwd);  // ������
        break;
    case 22: // ���㿨�Ƿ���Խ���
        p.WriteLong(sCmd.data.cmdCanTrade.lUserParam);
        p.WriteShort(sCmd.data.cmdCanTrade.sServerID);
        p.WriteLong(sCmd.data.cmdCanTrade.lSalesmanAccountID);
        p.WriteString(sCmd.data.cmdCanTrade.szSalesmanAccountName);
        p.WriteLong(sCmd.data.cmdCanTrade.lUserID);
        p.WriteByte(sCmd.data.cmdCanTrade.cCardType);
        p.WriteString(sCmd.data.cmdCanTrade.szCardID);   // ����
        p.WriteString(sCmd.data.cmdCanTrade.szCardPwd);  // ������
        p.WriteShort(sCmd.data.cmdCanTrade.sTime);       // ���׵�ʱ��(��λΪ����, Ϊ0��ʾû����Ҫ��¼��ʱ, ���������Ϊ�˷�ֹ�в��ֵĳ�ֵ����ʱ�����ƣ���Щ���ڽӽ����ڵ�ʱ����м�������ƭ������ҡ�)
        break;
    case 31: // Ԥ�۵���(ֻ�Ǽ���Ƿ����㹻�ĵ�������������ʵ�ʵĿ۵����)
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdPreDeduct.sServerID);
        p.WriteLong(sCmd.data.cmdPreDeduct.lAccountID);
        p.WriteLong(sCmd.data.cmdPreDeduct.lUserID);
        p.WriteShort(sCmd.data.cmdPreDeduct.sPoint);
        p.WriteLong(sCmd.data.cmdPreDeduct.lDeductOrderID);
        break;
    case 32: // �۳�����(��ʹ��������ҲҪ�����۳������˶��ٵ��ڷ��ش�����)
        p.WriteLong(sCmd.lSN);
        p.WriteShort(sCmd.data.cmdDeduct.sServerID);
        p.WriteLong(sCmd.data.cmdDeduct.lAccountID);
        p.WriteLong(sCmd.data.cmdDeduct.lUserID);
        p.WriteShort(sCmd.data.cmdDeduct.sPoint);
        p.WriteLong(sCmd.data.cmdDeduct.lDeductOrderID);
        break;
    case 41: // �����鷢���仯
        p.WriteString(sCmd.data.cmdBlockChanged.szAccountName);
        p.WriteLong(sCmd.data.cmdBlockChanged.lAccountID);
        p.WriteLong(sCmd.data.cmdBlockChanged.lUserID);
        p.WriteString(sCmd.data.cmdBlockChanged.szOldBlockName);
        p.WriteString(sCmd.data.cmdBlockChanged.szNewBlockName);
        break;
    }
    EndSend();
}

void CChargeSession::LoginChargeServer()
{
    SSendCmd cmd;
    cmd.cCmdID = 1;
    strncpy(cmd.data.cmdLink.szUsername, m_szUsername.c_str(), m_iUsernameLen); cmd.data.cmdLink.szUsername[m_iUsernameLen] = 0;
    strncpy(cmd.data.cmdLink.szPassword, m_szPassword.c_str(), m_iUsernameLen); cmd.data.cmdLink.szPassword[m_iUsernameLen] = 0;
    _SendCmd(cmd); // ֱ�ӷ��͵�½����
}

int CChargeSession::Login(short sServer, const char* szUsername, const char* szRemotehost)
{
    SSendCmd cmd;
    cmd.cCmdID = 2;
    cmd.lSN = (++m_iCurSn);
    cmd.data.cmdLogin.sServerID = sServer;
    strncpy(cmd.data.cmdLogin.szUsername, szUsername, m_iUsernameLen); cmd.data.cmdLogin.szUsername[m_iUsernameLen] = 0;
    cmd.data.cmdLogin.lRemotehost = StrToIP(szRemotehost);
    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::Enter(short sServer, const char* szUsername, short sLevel)
{
    SSendCmd cmd;
    cmd.cCmdID = 3;
    cmd.lSN = (++m_iCurSn);
    cmd.data.cmdEnter.sServerID = sServer;
    strncpy(cmd.data.cmdEnter.szUsername, szUsername, m_iUsernameLen); cmd.data.cmdEnter.szUsername[m_iUsernameLen] = 0;
    cmd.data.cmdEnter.sLevel = sLevel;
    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::Check(short sServer, const char* szUsername, short sLevel)
{
    SSendCmd cmd;
    cmd.cCmdID = 4;
    cmd.lSN = (++m_iCurSn);
    cmd.data.cmdCheck.sServerID = sServer;
    strncpy(cmd.data.cmdCheck.szUsername, szUsername, m_iUsernameLen); cmd.data.cmdCheck.szUsername[m_iUsernameLen] = 0;
    cmd.data.cmdCheck.sLevel = sLevel;
    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::Logout(short sServer, const char* szUsername, long lAccountID, long lLougoutOrderID)
{
    SSendCmd cmd;
    cmd.cCmdID = 5;
    cmd.lSN = (++m_iCurSn);
    cmd.data.cmdLogout.sServerID = sServer;
    cmd.data.cmdLogout.lAccountID = lAccountID;
    cmd.data.cmdLogout.lLogoutOrderID = lLougoutOrderID;
    strncpy(cmd.data.cmdLogout.szUsername, szUsername, m_iUsernameLen); cmd.data.cmdLogout.szUsername[m_iUsernameLen] = 0;
    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::Reset(short sServer)
{
    SSendCmd cmd;
    cmd.cCmdID = 6;
    cmd.lSN = (++m_iCurSn);
    cmd.data.cmdReset.sServerID = sServer;
    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::AccountActive(long lCmdOrderID, const char* szUsername, const char* szPassword, char cType)
{
    SSendCmd cmd;
    cmd.cCmdID = 11;
    cmd.lSN = (++m_iCurSn);
    strncpy(cmd.data.cmdActive.szUsername, szUsername, m_iUsernameLen); cmd.data.cmdActive.szUsername[m_iUsernameLen] = 0;
    strncpy(cmd.data.cmdActive.szPassword, szPassword, m_iUsernameLen); cmd.data.cmdActive.szPassword[m_iUsernameLen] = 0;
    cmd.data.cmdActive.lCmdOrderID = lCmdOrderID;
    cmd.data.cmdActive.cType = cType;
    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::ChargeCard(long lBuyerAccountID, const char* szBuyerAccountName,
            long lSalesmanAccountID, const char* szSalesmanAccountName,
            short sServer, long lCharID, const char* szCharName, short sLevel, char cMetier, char cMetierLev,
            char cCardType, const char* szCardID, const char* szCardPwd, long lUserParam)
{
    SSendCmd cmd;
    cmd.cCmdID = 21;
    cmd.lSN = (++m_iCurSn);

    cmd.data.cmdChargeCard.sServerID  = sServer;
    cmd.data.cmdChargeCard.lBuyerAccountID  = lBuyerAccountID;
    strncpy(cmd.data.cmdChargeCard.szBuyerAccountName, szBuyerAccountName, m_iUsernameLen); cmd.data.cmdChargeCard.szBuyerAccountName[m_iUsernameLen] = 0;
    cmd.data.cmdChargeCard.lSalesmanAccountID  = lSalesmanAccountID;
    strncpy(cmd.data.cmdChargeCard.szSalesmanAccountName, szSalesmanAccountName, m_iUsernameLen); cmd.data.cmdChargeCard.szSalesmanAccountName[m_iUsernameLen] = 0;
    cmd.data.cmdChargeCard.lBuyerUserID = lCharID;
    strncpy(cmd.data.cmdChargeCard.szBuyerCharName, szCharName, m_iUsernameLen); cmd.data.cmdChargeCard.szBuyerCharName[m_iUsernameLen] = 0;
    cmd.data.cmdChargeCard.sLevel     = sLevel;
    cmd.data.cmdChargeCard.cMetier    = cMetier;
    cmd.data.cmdChargeCard.cMetierLev = cMetierLev;
    cmd.data.cmdChargeCard.cCardType  = cCardType;
    cmd.data.cmdChargeCard.lUserParam = lUserParam;
    strncpy(cmd.data.cmdChargeCard.szCardID, szCardID, m_iUsernameLen); cmd.data.cmdChargeCard.szCardID[m_iUsernameLen] = 0;
    strncpy(cmd.data.cmdChargeCard.szCardPwd, szCardPwd, m_iUsernameLen); cmd.data.cmdChargeCard.szCardPwd[m_iUsernameLen] = 0;

    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::CardCanTrade(long lSalesmanAccountID, const char* szSalesmanAccountName,
            short sServer, long lCharID, char cCardType, const char* szCardID,
            const char* szCardPwd, long lUserParam, short sTime)
{
    SSendCmd cmd;
    cmd.cCmdID = 22;
    cmd.lSN = (++m_iCurSn);

    cmd.data.cmdCanTrade.lSalesmanAccountID  = lSalesmanAccountID;
    strncpy(cmd.data.cmdCanTrade.szSalesmanAccountName, szSalesmanAccountName, m_iUsernameLen); cmd.data.cmdCanTrade.szSalesmanAccountName[m_iUsernameLen] = 0;
    cmd.data.cmdCanTrade.sServerID  = sServer;
    cmd.data.cmdCanTrade.sTime      = sTime;
    cmd.data.cmdCanTrade.lUserID    = lCharID;
    cmd.data.cmdCanTrade.cCardType  = cCardType;
    cmd.data.cmdCanTrade.lUserParam = lUserParam;
    strncpy(cmd.data.cmdCanTrade.szCardID, szCardID, m_iUsernameLen); cmd.data.cmdCanTrade.szCardID[m_iUsernameLen] = 0;
    strncpy(cmd.data.cmdCanTrade.szCardPwd, szCardPwd, m_iUsernameLen); cmd.data.cmdCanTrade.szCardPwd[m_iUsernameLen] = 0;

    SendCmd(cmd);
    return cmd.lSN;
}

const char* CChargeSession::Message(long lMsgID)
{
    EXT_SPACE::unordered_map<long, std::string>::iterator it = m_mapMsgTable.find(lMsgID);
    if (it!=m_mapMsgTable.end())
    {
        return (*it).second.c_str();
    }
    return "δ֪����";
}

int CChargeSession::PreDeduct(short sServer, long lAccountID, long lCharID, short sPoint, long lDeductOrderID)
{
    SSendCmd cmd;
    cmd.cCmdID = 31;
    cmd.lSN = (++m_iCurSn);

    cmd.data.cmdPreDeduct.lAccountID     = lAccountID;
    cmd.data.cmdPreDeduct.lUserID        = lCharID;
    cmd.data.cmdPreDeduct.sServerID      = sServer;
    cmd.data.cmdPreDeduct.sPoint         = sPoint;
    cmd.data.cmdPreDeduct.lDeductOrderID = lDeductOrderID;

    SendCmd(cmd);
    return cmd.lSN;
}

int CChargeSession::Deduct(short sServer, long lAccountID, long lCharID, short sPoint, long lDeductOrderID)
{
    SSendCmd cmd;
    cmd.cCmdID = 32;
    cmd.lSN = (++m_iCurSn);

    cmd.data.cmdDeduct.lAccountID        = lAccountID;
    cmd.data.cmdDeduct.lUserID           = lCharID;
    cmd.data.cmdDeduct.sServerID         = sServer;
    cmd.data.cmdDeduct.sPoint            = sPoint;
    cmd.data.cmdDeduct.lDeductOrderID    = lDeductOrderID;

    SendCmd(cmd);
    return cmd.lSN;
}

void CChargeSession::OnUserBlockChanged(char* szAccountName, long lAccountId, long lUserId, char* oldBlockFileName, char* newBlockFileName)
{
    SSendCmd cmd;
    cmd.cCmdID = 41;

    strncpy(cmd.data.cmdBlockChanged.szAccountName, szAccountName, 35); cmd.data.cmdBlockChanged.szOldBlockName[35] = 0;
    cmd.data.cmdBlockChanged.lAccountID        = lAccountId;
    cmd.data.cmdBlockChanged.lUserID           = lUserId;
    strncpy(cmd.data.cmdBlockChanged.szOldBlockName, oldBlockFileName, 40); cmd.data.cmdBlockChanged.szOldBlockName[39] = 0;
    strncpy(cmd.data.cmdBlockChanged.szNewBlockName, newBlockFileName, 40); cmd.data.cmdBlockChanged.szNewBlockName[39] = 0;

    SendCmd(cmd);
}
