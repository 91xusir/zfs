#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__

const int MAX_SQL_SIZE = 1024 * 16;

class CLoginLogicExt;  //PZH

class CLoginServer {
   public:
    friend class CLoginLogicExt;  //PZH
    CLoginServer(void);
    virtual ~CLoginServer(void);

    // --------------- Server -------------------
    bool Startup();
    bool Run();
    bool Shutdown(int iDelayTime = 30, int iForceTime = 30);  // 默认强制关闭时间为 30 秒
    void ShutdownNow();
    void Exit();
    bool Tick(long delta);

    void SetServerStatus(long lState) { m_lStatus = lState; }

    // --------------- Config -------------------
    bool LoadConfigFile(const char* file);
    // --------------- Game world -------------------
    CGameWorldStub* FindGws(long lIdx);
    CGameWorldStub* FindGwsByGWSID(long lGameworldID);
    CGameWorldStub* FindGwsByName(const char* szName);
    bool            TryAddGameWorld(CG_TCPSession* link);
    void            GameWorldNetDead(CGameWorldStub* stub);
    //------------------datebase-----------------------
    bool m_binitdb;
    bool Initdb();
    // --------------- User -------------------
    void       AddUser(SUserInfo* user, CGameWorldStub* stub);
    SUserInfo* FindUser(DWORD id);
    SUserInfo* FindUserByName(const char* szAccountName);
    void       UserQuit(DWORD dwAccountID);
    SUserInfo* TryAddUser(SListCmd* cmd, char cPointType, long lPoint,
                          char cVIP);  // cPointType 1点数, 2时间, cVIP 0不是VIP, 1是VIP
    void       KickUser(SUserInfo* user, char cReason, char* szReason = NULL);
    void       RemoveAllUser(CGameWorldStub* stub);
    void       KickUserByName(const char* szAccountName, char cReason, char* szReason = NULL);
    void       FlushMsg();
    void       OnGamigoAdbLoginCmd(int iParam, const char** pParam);
    void       OnActiveResult(char cResult, long lOrderID);
    void       OnCreateAccountResult(SListCmd* pResultCmd);
    void       UpdatePointInfo(SUserInfo* pUserInfo, char cPointType, long lPoint, char cVIP);
    void       OnUserEnterGame(DWORD dwAccountID, short sLevel);
    SUserInfo* OnChargePass(long lSn, char cPointType, long lPoint,
                            char cVIP);  // cPointType 1点数, 2时间, cVIP 0不是VIP, 1是VIP
    void       OnChargeFail(long lSn, const char* pReason);
    void       SendMsgToUser(const char* szUsername, char cMsgType, const char* szMessage);
    void       LogLogoutInfo(short sServer, long lAccountID, float fPoint, long lLogoutOrderID);
    // --------------- LogConcurrent -------------------
    void LogConcurrent();
    // --------------- Bulletin -------------------
    void SendBulletinToAllGWS(const char* content, char cStyle);
    void SendBulletinToGWServer(CGameWorldStub* pGW, std::string content, char cStyle);
    void StartAutoBulletin(long lDelay, const char* szContent);
    // --------------- Version -------------------
    bool CheckServerVersion(CG_CmdPacket* pPacket);
    bool CheckGameworldVersion(CGameWorldStub* pCheckGws, CG_CmdPacket* pPacket);
    // --------------- Guide server -------------------
    void SendUpdateServerList(GSClientSessionManager* pClient);
    void SendServerInfoToMoniter(GSClientSessionManager* pClientSessionManager);
    // --------------- Test server -------------------
    void TestServer(GSClientSessionManager* pClientSessionManager, int iSessionIdx);
    void SendTestResult();
    // --------------- Gws list -------------------
    void SendGWServerList(GSClientSessionManager* pClient,
                          int                     iCount = 0);  // If count == 0, send all servers
    void RegisterGws(long lIdx, const char* szName, const char* szIP, short sPort, long lGwID,
                     SAllowIP& allowIP);
    void UnregisterGws(long lIdx);
    // --------------- Account -------------------
    void CreateAccount(long lOrderID, const char* szUsername, const char* szPassword, long lPriv,
                       short sLocked, long tLockBegin, long tLockEnd);
    void LockAccount(CGameWorldStub* pStub, long lResultAccountID, const char* szUsername,
                     short sLocked, long tLockBegin, long tLockEnd);
    void LockAccountByID(CGameWorldStub* pStub, long lResultAccountID, long lAccountID,
                         short sLocked, long tLockBegin, long tLockEnd);
    void SetAccountPriv(CGameWorldStub* pStub, long lResultAccountID, const char* szUsername,
                        long lPriv);
    void SetAccountPrivByID(CGameWorldStub* pStub, long lResultAccountID, long lAccountID,
                            long lPriv);
    void SetAccountPassword(CGameWorldStub* pStub, long lResultAccountID, const char* szUsername,
                            const char* szPassword, long lLoginOrderID = 0);
    void SetAccountPasswordByID(CGameWorldStub* pStub, long lResultAccountID, long lAccountID,
                                const char* szPassword);
    void DeleteAccount(CGameWorldStub* pStub, long lResultAccountID, const char* szUsername);
    void DeleteAccountByID(CGameWorldStub* pStub, long lResultAccountID, long lAccountID);
    // --------------- Point operation -------------------
    void AddPointRet(short sServer, long lAccountID, long lUserID, short sPoint, char* cResult,
                     char cSucceed);
    bool Deduct(short sServer, long lAccountID, long lUserID, short sPoint, long lDeductOrderID);
    void PreDeduct(short sServer, long lAccountID, long lUserID, short sPoint, long lDeductOrderID);
    void OnDeduct(long lSn, short sServer, long lAccountID, long lUserID, short sPoint,
                  short sCurPoint, char* cResult, long lDeductOrderID);
    void OnPreDeduct(short sServer, long lAccountID, long lUserID, short sPoint, short curPoint,
                     char* cResult, char cSucceed, long lDeductOrderID, long event,
                     long lAuctionId);
    // --------------- Gift -------------------
    void ChargeCard(long lBuyerAccountID, const char* szBuyerAccountName, long lSalesmanAccountID,
                    const char* szSalesmanAccountName, short sServer, long lUserID,
                    const char* szUserName, short sLevel, char cMetier, char cMetierLev,
                    char cCardType, const char* szCardID, const char* szCardPwd, long lUserParam);
    void OnChargeCard(long lSn, short sServer, long lUserID, const char* szCardID, char cSucceeded,
                      const char* szMsg);
    void GiftErrorRet(short sServer, long lUserID, char* szCardID, char* msg, long lUserParam,
                      char szSucceed = 0);
    void AddGift(long lSnCard, const char* szUsername, short sServer, long lUserID, short sLevel,
                 short sType, const char* szGiftString, long tLimitTime, char cFromCardID,
                 char* szCardID);
    void ClearGift(const char* szUsername, short sServer, long lUserID);
    void CardCanTrade(long lSalesmanAccountID, const char* szSalesmanAccountName, short sServer,
                      long lCharID, char cCardType, const char* szCardID, const char* szCardPwd,
                      long lUserParam, short sTime);
    void OnCardCanTradeResult(long lSn, short sServer, long lUserID, const char* szCardID,
                              char cSucceeded, const char* szMsg, long lCardGroupType);
    void OnCardCharged(const char* szCardID, long lCardGroupType);
    //tim.yang   VIP卡
    void AddCardTimeRet(short gwsid, long lUserid, char isRight, std::string time);
    //end
    // --------------- Util -------------------
    void DoGameMasterCommand(CGameWorldStub* pStub, long lGMAccountID, const char* szCommand);
    void BroadcastCmdToGameworld(const char* szCommand);
    void BroadcastCmdToAllRegion(const char* szCommand);
    void LogChargeCard(long lSn, SCmdCardChargeLog::ECCOP eOP, char cCardType, long lAccountID,
                       short sServer, long lUserID, const char* szCardID);
    void OnUserBlockChanged(char* szAccountName, long lAccountId, long lUserId,
                            char* oldBlockFileName, char* newBlockFileName);

   private:
    void AddChargeCmd(long lSn, DWORD dwAccountID, SUserInfo* pUserInfo, time_t tNow);

   public:
    // 设置状态
    //     0x0001 普通登陆
    //     0x0002 GM登陆
    //     0x0004 更新
    long        m_lStatus;
    time_t      m_tShutdownTime;
    time_t      m_tShutdownDelay;
    long        m_lShutdownForceTime;
    char        m_bNeedSendShutdown;
    std::string m_strLoginName;
    //数据库
    CDBMySql m_db;
    //char		m_sql[MAX_SQL_SIZE];
    CM_MEMDEF(m_sql, MAX_SQL_SIZE)

    GSClientSessionManager m_client80;
    GSClientSessionManager m_client6620;
    GSUpdateSessionManager m_update;
    GSClientSessionManager m_monitor;

   private:
    typedef std::unordered_map<long, CGameWorldStub*> TGWSMap;
    typedef std::unordered_map<DWORD, SUserInfo*>     TUserMap;
    typedef std::unordered_map<long, SListCmd*>       TNetCmdMap;
    typedef std::unordered_map<long, SChargeCard*>    TChargeCardMap;

    TUserMap       m_userMap;
    DWORD          m_dwLogTime;
    CG_TCPListener m_gwsListen;

    long    m_lLastGwsIdx;  // 最后一个gws的索引
    TGWSMap m_gwsMap;       // 所有gws

    bool m_bListenGameWorld;
    bool m_bListenUpdateServer;
    bool m_bListenClientGuide;
    bool m_bListenMonitor;
    bool m_bConnectDatabase;
    bool m_bConnectChargeServer;
    bool m_bUseCharge;
    bool m_bLicenseCheck;

    SStatLoginServer        m_lsStat;
    CG_TCPBroadcastArchive* m_pNetLog;

    //************************************************* 公告系统
    DWORD       m_dwBulletinTick;             // 剩余的更新时间
    DWORD       m_dwBulletinLastTime;         // 最后一次发出公告的时间
    std::string m_strBulletinContent;         // 公告的内容
    DWORD       m_dwNextUpdateBulletinCycle;  // 下一次更新的更新周期
    const DWORD m_dwBulletinInterval;         // 更新的大周期
    void        ResetBulletin();
    void        RunBulletin();

    //************************************************* 计费检查命令  废弃
    struct SChargeCmd {
        long   lSn;
        DWORD  dwUserID;
        time_t tTimeout;
    };

    struct SChargeLoginCmd {
        long     lSn;
        SListCmd cmd;
        time_t   tTimeout;
    };

    CGamigoADBSession          m_gamigoADBSession;
    time_t                     m_tLastCheckTime;
    CChargeSession             m_chargeSession;
    std::list<SChargeCmd>      m_listChargeCmd;
    std::list<SChargeLoginCmd> m_listChargeLoginCmd;
    TNetCmdMap                 m_cmdAccountActiving;
    TNetCmdMap                 m_cmdAccountCreating;
    std::list<SListCmd*>       m_cmdFree;

    //************************************************* 检查卡是否可以交易
    struct SCmdCardCanTrade {
        long  lSn;
        short sServer;
        long  lUserID;
        //char    szCardID[40];
        CM_MEMDEF(szCardID, 40)
        time_t tTimeout;

        SCmdCardCanTrade() { CM_MEMPROTECTOR(szCardID, 40) }

        ~SCmdCardCanTrade() { CM_MEMUNPROTECTOR(szCardID) }
    };

    std::list<SCmdCardCanTrade*> m_cmdCardCanTradeFree;
    std::list<SCmdCardCanTrade*> m_cmdCardCanTradeList;
    SCmdCardCanTrade*            CmdCardCanTradeNew();
    bool                         CmdCardCanTradeRemove(long lSn);
    bool                         CmdCardCanTradeExist(long lSn);
    void                         CmdCardCanTradeRun();
    void                         CmdCardCanTradeDelete();

    //************************************************* 卡号充值
    struct SCmdChargeCard {
        long  lSn;
        short sServer;
        char  cCardType;
        long  lAccountID;
        long  lUserID;
        //char    szCa rdID[40];
        CM_MEMDEF(szCardID, 40)
        time_t tTimeout;

        SCmdChargeCard() { CM_MEMPROTECTOR(szCardID, 40) }

        ~SCmdChargeCard() { CM_MEMUNPROTECTOR(szCardID) }
    };

    std::list<SCmdChargeCard*> m_cmdChargeCardFree;
    std::list<SCmdChargeCard*> m_cmdChargeCardList;
    SCmdChargeCard*            CmdChargeCardNew();
    bool                       CmdChargeCardRemove(long lSn);
    bool                       CmdChargeCardExist(long lSn);
    void                       CmdChargeCardRun();
    void                       CmdChargeCardDelete();

    //************************************************* 登出命令
    struct SLogoutMsg {
        long  lLogoutOrder;
        DWORD dwAccountID;
        DWORD dwLoginIP;
        DWORD dwEnterTime;
        DWORD dwLeaveTime;
        float fPoint;
        short sServer;
    };

    long                   m_lastLogoutOrder;
    std::list<SLogoutMsg*> m_listLogoutFree;
    std::list<SLogoutMsg*> m_listLogoutCmd;
    void                   RunLogout();

    //************************************************* 版本信息
    struct SVersionData {
        std::string szExeMD5;
        std::string szDataMD5;
        std::string szOSType;
    };

    std::map<std::string, SVersionData> m_mapVersion;

    //************************************************* 服务器测试
    struct STester {
        GSClientSessionManager* pClientSessionManager;
        int                     iSessionIdx;
    };

    time_t             m_lastTestTime;
    std::list<STester> m_listTest;

   private:
    //************************************************* 游戏世界列表
    struct SGwsList {
        long        lIdx;
        long        lGameworldID;
        std::string szName;
        std::string szIP;
        short       sPort;
        short       bHaveAllow;
        SAllowIP    allowDisplayIP;
    };

    std::list<SGwsList> m_listGws;
};

#endif
