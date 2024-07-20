#ifndef _CHARGE_SESSION_H_
#define _CHARGE_SESSION_H_


class CChargeSession : protected CG_TCPSession
{
public:
    CChargeSession();
    virtual ~CChargeSession();

    enum { m_iUsernameLen = 35 };
    struct SSendCmd
    {
        char        cCmdID;
        long        lSN;
        struct UData
        {
            struct SLink
            {
                //char    szUsername[m_iUsernameLen+1];
                //char    szPassword[m_iUsernameLen+1];
				CM_MEMDEF(szUsername, m_iUsernameLen+1)
				CM_MEMDEF(szPassword, m_iUsernameLen+1)

				SLink()
				{
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szPassword, m_iUsernameLen+1)
				}
				SLink(const SLink &object)
				{
					memcpy(this, &object, sizeof(SLink));
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szPassword, m_iUsernameLen+1)
				}
				~SLink()
				{
					CM_MEMUNPROTECTOR(szUsername)
					CM_MEMUNPROTECTOR(szPassword)
				}
            }cmdLink;

            struct SLogin
            {
                short   sServerID;
                //char    szUsername[m_iUsernameLen+1];
				CM_MEMDEF(szUsername, m_iUsernameLen+1)
				long    lRemotehost;

				SLogin()
				{
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
				}
				SLogin(const SLogin &object)
				{
					memcpy(this, &object, sizeof(SLogin));
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
				}
				~SLogin()
				{
					CM_MEMUNPROTECTOR(szUsername)
				}
            }cmdLogin;

            struct SEnter
            {
                //char    szUsername[m_iUsernameLen+1];
				CM_MEMDEF(szUsername, m_iUsernameLen+1)
                short   sServerID;
                short   sLevel;

				SEnter()
				{
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
				}
				SEnter(const SEnter &object)
				{
					memcpy(this, &object, sizeof(SEnter));
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
				}
				~SEnter()
				{
					CM_MEMUNPROTECTOR(szUsername)
				}
			};
            SEnter  cmdEnter;
            SEnter  cmdCheck;

            struct SLogout
            {
                short   sServerID;
                //char    szUsername[m_iUsernameLen+1];
				CM_MEMDEF(szUsername, m_iUsernameLen+1)
                long    lAccountID;
                long    lLogoutOrderID;

				SLogout()
				{
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
				}
				SLogout(const SLogout &object)
				{
					memcpy(this, &object, sizeof(SLogout));
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
				}
				~SLogout()
				{
					CM_MEMUNPROTECTOR(szUsername)
				}
			}cmdLogout;

            struct SReset
            {
                short   sServerID;
            }cmdReset;

            struct SActive
            {
                long    lCmdOrderID;
                //char    szUsername[m_iUsernameLen+1];
                //char    szPassword[m_iUsernameLen+1];
				CM_MEMDEF(szUsername, m_iUsernameLen+1)
				CM_MEMDEF(szPassword, m_iUsernameLen+1)
                char    cType;      // 0 �����ʺ�û���ҵ�����Ҫ����, 1 ��������Ƚϴ���

				SActive()
				{
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szPassword, m_iUsernameLen+1)
				}
				SActive(const SActive &object)
				{
					memcpy(this, &object, sizeof(SActive));
					CM_MEMPROTECTOR(szUsername, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szPassword, m_iUsernameLen+1)
				}
				~SActive()
				{
					CM_MEMUNPROTECTOR(szUsername)
					CM_MEMUNPROTECTOR(szPassword)
				}
            }cmdActive;

            struct SChargeCard
            {
                long    lBuyerAccountID;
                //char    szBuyerAccountName[m_iUsernameLen+1];
				CM_MEMDEF(szBuyerAccountName, m_iUsernameLen+1)
               long    lSalesmanAccountID;
                //char    szSalesmanAccountName[m_iUsernameLen+1];
				CM_MEMDEF(szSalesmanAccountName, m_iUsernameLen+1)
               long    lBuyerUserID;
                //char    szBuyerCharName[m_iUsernameLen+1];
				CM_MEMDEF(szBuyerCharName, m_iUsernameLen+1)
                short   sServerID;
                short   sLevel;
                char    cMetier;
                char    cMetierLev;
                char    cCardType;      // 1 ���߿�(û������), 2 �㿨(������)
                long    lUserParam;
                //char    szCardID[m_iUsernameLen+1];
				CM_MEMDEF(szCardID, m_iUsernameLen+1)
                //char    szCardPwd[m_iUsernameLen+1];
				CM_MEMDEF(szCardPwd, m_iUsernameLen+1)

				SChargeCard()
				{
					CM_MEMPROTECTOR(szBuyerAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szSalesmanAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szBuyerCharName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardID, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardPwd, m_iUsernameLen+1)
				}
				SChargeCard(const SChargeCard &object)
				{
					memcpy(this, &object, sizeof(SChargeCard));
					CM_MEMPROTECTOR(szBuyerAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szSalesmanAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szBuyerCharName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardID, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardPwd, m_iUsernameLen+1)
				}
				~SChargeCard()
				{
					CM_MEMUNPROTECTOR(szBuyerAccountName)
					CM_MEMUNPROTECTOR(szSalesmanAccountName)
					CM_MEMUNPROTECTOR(szBuyerCharName)
					CM_MEMUNPROTECTOR(szCardID)
					CM_MEMUNPROTECTOR(szCardPwd)
				}
            }cmdChargeCard;

            struct SCanTrade
            {
                long    lSalesmanAccountID;
                //char    szSalesmanAccountName[m_iUsernameLen+1];
				CM_MEMDEF(szSalesmanAccountName, m_iUsernameLen+1)
                short   sServerID;
                short   sTime;     // ���׵�ʱ��(��λΪ����, Ϊ0��ʾû����Ҫ��¼��ʱ)
                long    lUserID;
                char    cCardType; // 1 ���߿�(û������), 2 �㿨(������)
                //char    szCardID[m_iUsernameLen+1];
				CM_MEMDEF(szCardID, m_iUsernameLen+1)
                //char    szCardPwd[m_iUsernameLen+1];
				CM_MEMDEF(szCardPwd, m_iUsernameLen+1)
                long    lUserParam;

				SCanTrade()
				{
					CM_MEMPROTECTOR(szSalesmanAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardID, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardPwd, m_iUsernameLen+1)
				}
				SCanTrade(const SCanTrade &object)
				{
					memcpy(this, &object, sizeof(SCanTrade));
					CM_MEMPROTECTOR(szSalesmanAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardID, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szCardPwd, m_iUsernameLen+1)
				}
				~SCanTrade()
				{
					CM_MEMUNPROTECTOR(szSalesmanAccountName)
					CM_MEMUNPROTECTOR(szCardID)
					CM_MEMUNPROTECTOR(szCardPwd)
				}
            }cmdCanTrade;

            struct SDeduct
            {
                long    lAccountID;
                long    lUserID;
                short   sServerID;
                short   sPoint;
                long    lDeductOrderID;
            };
            SDeduct cmdDeduct;
            SDeduct cmdPreDeduct;

            struct SBlockChanged
            {
                //char    szAccountName[m_iUsernameLen+1];
				CM_MEMDEF(szAccountName, m_iUsernameLen+1)
                long    lAccountID;
                long    lUserID;
                //char    szOldBlockName[40];
				CM_MEMDEF(szOldBlockName, 40)
                //char    szNewBlockName[40];
				CM_MEMDEF(szNewBlockName, 40)

				SBlockChanged()
				{
					CM_MEMPROTECTOR(szAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szOldBlockName, 40)
					CM_MEMPROTECTOR(szNewBlockName, 40)
				}
				SBlockChanged(const SBlockChanged &object)
				{
					memcpy(this, &object, sizeof(SBlockChanged));
					CM_MEMPROTECTOR(szAccountName, m_iUsernameLen+1)
					CM_MEMPROTECTOR(szOldBlockName, 40)
					CM_MEMPROTECTOR(szNewBlockName, 40)
				}
				~SBlockChanged()
				{
					CM_MEMUNPROTECTOR(szAccountName)
					CM_MEMUNPROTECTOR(szOldBlockName)
					CM_MEMUNPROTECTOR(szNewBlockName)
				}
            }cmdBlockChanged;
        }data;

		SSendCmd() : cCmdID(0), lSN(0)
		{
		}
		~SSendCmd()
		{
		}
    };

protected:
    virtual void    OnConnect(bool ret);
    virtual void    OnDisconnect();
    virtual void    OnReceive(CG_CmdPacket* packet);

public:
    bool ConnectServer(const char* szRemoteIP, int iRemotePort,
        const char* szUsername, const char* szPassword, const char* szBindIP=0, int iBindPort=0);
    void ProcessNet();

    bool IsConnected()  { return GetState()==NET_STATE_CONNECTED; }
    bool IsClosed()     { return GetState()==NET_STATE_ZERO || GetState()==NET_STATE_DEAD; }

    void SendCmd();
    void SendCmd(SSendCmd& sCmd);

    // for �Ʒ�ϵͳ
    int  Login(short sServer, const char* szUsername, const char* szRemotehost);
    int  Enter(short sServer, const char* szUsername, short sLevel);
    int  Check(short sServer, const char* szUsername, short sLevel);
    int  Logout(short sServer, const char* szUsername, long lAccountID, long lLougoutOrderID);
    int  Reset(short sServer);

    // for ��������
    int  PreDeduct(short sServer, long lAccountID, long lCharID, short sPoint, long lDeductOrderID);
    int  Deduct(short sServer, long lAccountID, long lCharID, short sPoint, long lDeductOrderID);

    // for ����ϵͳ
    int  AccountActive(long lCmdOrderID, const char* szUsername, const char* szPassword, char cType);

    // for ���ų�ֵ(�����㿨�����߿��͸��ֵ��߱��)
    int  ChargeCard(long lBuyerAccountID, const char* szBuyerAccountName,
        long lSalesmanAccountID, const char* szSalesmanAccountName,
        short sServer, long lCharID, const char* szCharName, short sLevel, char cMetier, char cMetierLev,
        char cCardType, const char* szCardID, const char* szCardPwd, long lUserParam);
    int  CardCanTrade(
        long lSalesmanAccountID, const char* szSalesmanAccountName,
        short sServer, long lCharID, char cCardType, const char* szCardID,
        const char* szCardPwd, long lUserParam, short sTime);

    // for ��ͼ�仯
    void OnUserBlockChanged(char* szAccountName, long lAccountId, long lUserId, char* oldBlockFileName, char* newBlockFileName);

private:
    CG_CmdPacket&   BeginSend(char cCmdID) { m_sendCmd.BeginWrite(); m_sendCmd.WriteByte(cCmdID); return m_sendCmd; }
    bool            EndSend() { return SendPacket(&m_sendCmd); }
    void            _SendCmd(SSendCmd& sCmd);
    void            LoginChargeServer();
    const char*     Message(long lMsgID);

    CG_CmdPacket    m_sendCmd;

    std::string     m_szRemoteIP;
    int             m_iRemotePort;
    std::string     m_szBindIP;
    int             m_iBindPort;
    std::string     m_szUsername;
    std::string     m_szPassword;
    bool            m_bLogined;

    EXT_SPACE::unordered_map<long, std::string> m_mapMsgTable;

    time_t          m_tDisconnect;
    int             m_iCurSn;

    std::list<SSendCmd>  m_listCmd;

};


#endif // _CHARGE_SESSION_H_
