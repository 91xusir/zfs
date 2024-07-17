#ifndef _INC_GW_SERVER_H_
#define _INC_GW_SERVER_H_


//#define ENABLE_OPERATOR_NEW


#include "core/rt2_core.h"
#include "rt1tort2.h"
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>
#include <vector>
#include <time.h>
#include "gw_config.h"
#include "net/rt_net.h"
#include "region_client.h"
#include "server_config.h"
#include "gw_team.h"
#include "lgs_gws.h"
#include "string_mapping.h"
#include "gw_dungeon.h"
#include "cardtrade.h"
#include "gw_master_list.h"
#include "gw_commandchannel.h"

using namespace std;

#ifndef snprintf
#	ifdef _WIN32
#		define snprintf _snprintf
#	endif // _WIN32
#endif // _WIN32

enum GWServerState
{
	ssRun,
	ssStartup,
	ssShutdown,
	ssReboot,
    ssExit
};

// extern "C" GWConfig g_config;
extern SConfigGameworldNet  g_cfgGW;
//extern SConfigConnect       g_cfgConnectLogin;
//extern SConfigGame          g_cfgGame;
extern CG_CmdPacket		   *g_sendCmd,*g_recvCmd;

#include "gw_protocol.h"
#include "gw_commandchannel.h"
#include "gw_dbasyncqueue.h"
#include "gw_database.h"
#include "gw_logdb.h"
#include "gw_loginsession.h"
#include "gw_sessionmanager.h"
#include "item_base.h"
#include "gw_union.h"
#include "gw_feod.h"
#include "gw_chattransmittal.h"
#include "gw_mailmanager.h"
#include "item_base.h"
#include "gw_item.h"
#include "gw_herolist.h"
#include "gwt.h"
//-----------add start by tony 05.06.16----------------//
#include "gw_castlewar.h"
//-----------add end   by tony 05.06.16----------------//
#include "auction.h"
#include "gw_gift.h"
#include "gw_LineMan.h"

#include "GW_ActDB.h"

struct GWRegion
{
	int sessionIndex;
	//  short port;
	//  string host;
	int lostTime;
	int seed;

    long                lRsID;

    std::string         szConnectGameworldIP;
    long                lConnectGameworldPort;

    std::string         szListenClientIP;
    long                lListenClientPort;

    SStatRegionServer   m_stat;

    // 版本检查
    bool                bChecked;
    std::string         szExeMD5;
    std::string         szDataMD5;
    std::string         szOsType;

    // 测试
    char                m_cTestState;    // 0 没有测试, 1 正在测试, 2 测试返回

    GWRegion()
    {
        m_cTestState = 0;
        sessionIndex = -1;
        lListenClientPort = 0;
        bChecked = false;
    }
};

struct GWRegionFilter
{
	string from;
	string to;
};

struct PyGmOP
{
	long			cmdIDinDB;
	std::string		gmcmd;
	long			gm_account;
	std::string		toUserName;
};

class GWServer
{
private:
	typedef vector<GWRegion>		GWRegionTable;
	typedef HASH_STR(string, int)	GWSceneTable;
	std::list<PyGmOP>				m_pygmop;

public:
    bool	Startup();
	void	Run();
	void	Exit();

	//
	// modify by carl
	//
	int m_linkSeed;
	GWUnloginClient *CreateUnloginClient(CG_TCPSession *netLink);
	bool    StartClientListen();
	void    RunClient();
	bool    UserLogin(GWUnloginClient *client,SAccountInfo *info);
	void    UserQuit(GWLoginClient *client);
	void    UserSelectActor(GWLoginClient *client);
	void    SaveUserData();
    void    SendKickAllUser(long lDelayTime);
	void	AddPyGMOP(long cmdid, std::string gmcommand, std::string toUser, long GMAccountid = 0);
	void    GetGMOP();
	void	DisPatchPyGMOP();

	CG_TCPListener                                      m_clientListen;
	list<GWUnloginClient*>								m_unloginClient;
	EXT_SPACE::hash_map<DWORD, GWLoginClient*>			m_loginClient;      // idx by account id
	HASH_STR(std::string, GWLoginClient*)	            m_tableByActorName; // idx by actor name    
	EXT_SPACE::hash_map<DWORD, GWLoginClient*>			m_tableByActorId;   // idx by actor id

	GWUnloginClient *m_curUnloginClient;
	GWLoginClient   *m_curLoginClient;
    CG_TCPBroadcastArchive* m_pNetLog;

	GWCommandChannel m_channelUnlogin;
	GWCommandChannel m_channelLogin;

	// 取得当前在线人数
	int GetCurrentUserCnt() { return (int)m_loginClient.size(); } 

	// 根据标识号码找到未登陆的客户端
	GWUnloginClient *FindUnloginClientBySeed(int seed);
	// 根据帐号或角色找到已登陆的客户端
	GWLoginClient *FindLoginClientByAccountId(int accountId);
	GWLoginClient *FindLoginClientByActorId(int actorId);

	// find actor by user id,if user is quiting return null,and if user is switch region return null.
	GWUser *FindActorByIdWithoutSwitchRegion(int actorId);
	// find actor by user id,if user is quiting return null
	GWUser *FindActorById(int actorId);
	// find actor by user name,if user is quiting return null
	GWUser *FindActorByName(const char *name);

	bool	KickAccount(DWORD accountId, char cCause, char const* szCause=NULL);
	void	LogoutUserByRegion(int regionID);
	bool	DistributeUser(GWLoginClient *lc,int seed,bool bLogin);

	void	BroadcastUpdateRegionList();
	void	BroadcastAddRegionList(long rgID);
	void	BroadcastRemoveRegionList(long rgID);
	void	SendRegionList(long session);
	void	SendRegionInit(long session);
	int		GetRegionSession(int id) { return id >= (int)m_regionTable.size() ? -1 : m_regionTable[id].sessionIndex; }
	void	UpdateUserState();
	bool	LoadSceneTable();	
	bool	PullUser(int target, int source);
	bool	PullUserAround(int target, int source, short nRange);
	bool	MoveUser(int userID, int x, int y);
    bool    MoveUserToDungeon(int userID, int iDungeonID, int x, int y, bool bBron);
	bool	MoveUserAround(int userID, int x, int y, short nRange);
	int		GetFreeRegion();
	bool    logOnline();
	bool    DeleteAllMail(long userID);
	bool	LogUserEvent(char type, long userID, const char* ip, long longParam1, const char* strParam1, long longParam2, const char* strParam2, long longParam3, const char* strParam3);
    void    SendMessageToAccount(DWORD dwAccountID, char cMsgType, const char* szMessage);
    void    SendSystemMessageToUser(GWUser* pUser, const char* szMessage, const char* szTitle=NULL);
    void    SendSystemMessageToUser(DWORD dwUserDBID, const char* szMessage, const char* szTitle=NULL);
    void    BroadcastBulletin(const char* szMessage, char bSysMsg=false, DWORD dwColor=0xFFFFFFFF, DWORD dwGMID=0);

    void    ChargeCard(long dwUserParam, long dwBuyerUserID,
        long lSalesmanAccountID, const char* szSalesmanAccountName,
        char cCardType, const char* szCardID, const char* szCardPwd, 
        const char* szUsername=0, short sLevel=0, char cMetier=0, char cMetierLev=0);
    void    OnChargeCardResult(long dwUserParam, long lUserID, const char* szCardID, char cSucceed, const char* szMsg);
    bool    AddMoneyToGift(long dwUserID, int iMoney, const char* szInfo);

    void    CheckCardCanTrade(long lSalesmanAccountID, const char* szSalesmanAccountName,
        long lUserID, char cCardType, const char* szCardID, const char* szCardPwd,
        long lUserParam, short sTimeOut);
    void    OnCheckCardCanTradeResult(long lUserID, const char* szCardID, char cCan, const char* szMsg, long lCardGroupType, long lUserParam);

	int		GetUserOnlineTime(DWORD userID);
	void	UpdateUserIsOnLine(int bOnLine,long lUserID=-1);//bOnLine为1在线， 为0不在线 ;  lUserID 为-1时不指定玩家， 对所有数据库角色生效

	bool	AddItemToGift(long dwUserID, SItemID& itemID, const char* szInfo);

	bool	MoveUserToWarfield(int userID, int x, int y, bool bBron,char faction);

    // --------------- Test server -------------------
    void TestServer();
    void SendTestResult();

	//tim.yang  VIP卡
	void AddAccountTime(long userID,long time);
	void AddAccountTimeRet(long lUserID,char isRight,std::string time = "");
	//end

public:
	void OnRealQuit(GWLoginClient *lc);
	void OnUserEnterWorld(GWLoginClient *lc);
	void OnUserLeaveWorld(GWLoginClient *lc);
	void OnUserSwitchRegion(int userID);
	void OnUserSwitchRegionSucceeded(GWLoginClient *lc);
	void OnUserDelete(int userID);
	void OnRegionRegister(int regionID);
    void OnRegionSessionChange();
    void OnAcceptLogin(bool bCanLogin);

public:
	GWServer()
	{
		m_state = ssStartup;
        m_cSubState = 0;
		m_linkSeed = 1;
        m_pNetLog = NULL;
	}

private:
	void CreateClientChannel();
	GWCommandChannel*	CreateRegionChannel();
	GWCommandChannel*	CreateGMChannel();

	static bool			OnClientLost(GWSessionManager* pseManager, GWSession& se);
    static bool         OnClientSessionDecline(GWSessionManager* pseManager, CG_TCPSession* session, char cCause);

    static bool			OnRegionLost(GWSessionManager* pseManager, GWSession& se);
    static bool			OnRegionAccept(GWSessionManager* pseManager, GWSession& se);

    static void			OnFinalSaveFinish(GWDBResult* result);

public:
	bool IsValidScene(const char *scene);
	void GetDefaultPosByFaction(int faction,string &scene,long &x,long &y);

	GWRegionTable		m_regionTable;
	GWSceneTable		m_sceneTable;
	GWDatabase			m_database;
	GWLogDb				m_LogDb;
	GW_ActDB			m_ActDB;

	GWSessionManager	m_region;
	GWSessionManager	m_gm;

	GWLoginSession		m_login;
	GWMailManager		m_mail;
	GwTeamManager		m_team;
	CMasterListManager  m_master_list_manager;
	GWChatTransmittal	m_chatTransmittal;
	time_t				m_lastUpdateRegionListTimeStamp;
	time_t				m_lastUpdateUserStateTimeStamp;
	time_t				m_lastLineCheckTimeStamp;
#ifdef ENABLE_OPERATOR_NEW
	time_t				m_lastMemCheckTimeStamp;
#endif
	int					m_lostRegionCount;
	GWServerState		m_state;
    char                m_cSubState;
	GWT					m_gwt;
    CGwDungeonMgr       m_dungeonMgr;
    GwCardTrade         m_cardTrade;
    GwAuction           m_gwAuction;
	gw_LineMan			m_LineMan;
    std::string         m_strLoginName;

	SStatGameWorldServer    m_stat;
    SStatGameMasterServer   m_gmStat;

	// 玩家惩罚列表
	struct SUserForbid
	{
		DWORD userID;
		ECreatureState Type;
		DWORD ForbidTime;
		DWORD EndTime;

		SUserForbid() { userID = 0; Type = CreState_Visible; ForbidTime = 0; EndTime = 0; }
	};
	std::list<SUserForbid> m_arrUserForidden;
	SUserForbid* FindUserForbidden(DWORD userID, ECreatureState forbidType);
	bool ForbidUser(DWORD userID, ECreatureState forbidType, DWORD forbidTime);
	bool UnForbidUser(DWORD userID, ECreatureState forbidType);
	void ProcessUserForbidden();
	void SetUserState(DWORD userID, DWORD state);
	void RemoveUserForbidden(DWORD userID, ECreatureState forbidType);

	// LogServerServer
	CG_UDPSession		m_LogServerSession;
	bool LogServerSender_Init();
	bool LogServerSender_Close();
	bool LogServerSender_Send(const char* title, const char* content);

private:
    time_t				m_lastTestServerTime;
    time_t				m_lastUpdateBusiness;
	time_t              m_lastOnlinetime; 
    int                 m_iAllUpdateBusiness;

public:
	//GWSessionManager	m_reboot;
	//GWCommandChannel*	CreateRebootChannel();
};

extern "C" GWServer g_server;
extern "C" CItemManager g_ItemMgr;

#endif // _INC_GW_SERVER_H_
