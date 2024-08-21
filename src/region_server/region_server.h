//
// s_regionserver.h: interface for the CRegionServer class.
//
#ifndef __CS_REGION_SERVER_H__
#define __CS_REGION_SERVER_H__
#include "region_master_list.h"

class RtRandomImage;
class CRegionLsSession;
class CJhWgSession;

//PZH 
class CRSLogicExt;
//

static char * WG_INFO_SPEEDUP_TIME = "check user error move speed";
static char * WG_INFO_SPEEDUP_DOUBLE = "check user double move speed";
static char * WG_INFO_ATTACK_NETCHECK = "check user attack info error";
static char * WG_INFO_CLIENT_CONFIG = "client checked command error";
static char * WG_INFO_CLIENT_MODULE = "client checked illegal module";
static char * WG_INFO_ROBOT_KICK = "robot kick";
static char * WG_INFO_KILL_CHEAT = "kill cheat";
static char * WG_INFO_SHOULD_KICK_CHEATER = "should kick cheater";
static char * WG_INFO_CHECK_COMMAND = "check receive command error";

class CRegionServer : public CRegionObject 
{
public:
	friend class CRSLogicExt;//PZH
    CRegionServer();
	virtual ~CRegionServer();

    enum ServerState
    {
        ssRun,
        ssStartup,
        ssShutdown,
        ssReboot,
        ssExit
    }	m_eState;

	// temp net link,used before user login
	struct SNetLink 
	{
		bool   Valid;
		ULONG  ConnectTime;
		ULONG  Id;
		ULONG  Seed;
		string Username;
		string Pwd;
		char   Code;
		DWORD  UserId;
		CG_TCPSession *Session;
	};

	virtual void OnTimer(ULONG id, DWORD dwParam[]);
    bool RegionWaittingForGWS();
	
	bool StartUp();						// server startup
	void Run();							// server main cycle
	void Exit();						// server exit,clean all object here

	void SyncToClient();	
	void InitChannel();
	void Process();						// run one frame
	void ProcessNewLink();				// try accept new connection
	void ProcessNetLink();				// process unlogin connection
	void ProcessUserLink();				// process in game connection
	void RepCreatureToClient(CRegionCreature *cre,char type);
	void ShowPingTime();
	bool IsCheatId(DWORD id);
	
	bool CreateUser(SNetLink* linkId, SUserData &data, CG_CmdPacket *cmd);
	CRegionCreature *CreatePet(short Id,int lev);
	CRegionCreature *CreateNpc(short Id,bool cheat=false);
	CRegionCreature *CreateSpirit(short Id,bool cheat=false);
	CRegionUser *FindUser(ULONG UserId, bool forceAll=false);
	CRegionUser *FindUser(const char *Name, bool forceAll=false);
	bool KickUser(DWORD dbId, char cCause, bool saveData,bool logout, char const* szCause=NULL);
	int  GetCurrentUserCnt() { return (int)m_userMap.size(); }
	void BroadcastShowTimerByScene(CRegionScene *pScene, const char *pText, long lTime);
	void BroadcastSceneDataByScene(CRegionScene *pScene, int iCount, const char **pInfoName, long *plInfovalue);
	void StartSchedule(CRegionCreature *pCreature, long lSchedule, const char *pRetScript);
	void BroadcastCmd(CG_CmdPacket *cmd,CRegionCreature *exclude);
	void BroadcastCmd(CG_CmdPacket *cmd,CRegionCreature *exclude,char faction);

	// event manage 
	// temp code,need move to other class
	bool BuildEventDead(CG_CmdPacket *cmd,CRegionCreature *cre);
	bool BuildEventCulLevelUp(CG_CmdPacket *cmd,CRegionCreature *cre);//Tianh  发送修为升级消息
	bool BuildEventBorn(CG_CmdPacket *cmd,CRegionCreature *cre);
	bool BuildEventLevelUp(CG_CmdPacket *cmd,CRegionCreature *cre);
	bool BuildEventStaticDialog(CG_CmdPacket *cmd,char *name);
	bool BuildEventSystemMsg(CG_CmdPacket *cmd, long msgID);
	bool BuildEventPlayPose(CG_CmdPacket *cmd, CRegionCreature *cre, short poseId, char dir);
	void ShowMessageBox(CRegionCreature *pUser, const char *pMessage, const char *pCommandStr, long iButton);
	void ShowHtmlDialog(CRegionCreature *src,CRegionCreature *tar,const char *link, const char* szName=NULL, 
		const char* szValue=NULL, char cListType = 0, short nTaskID = 0, char cPangeType = 0, long lShowTime = 0); //PZH

	void Shutdown();
	void Reboot();
    void KickAllUser(long lDelayTime);

	bool ProcessLogin(SNetLink* link);
	void ProcessLoginUser(int user);

	void StartListen();
    bool UpdateEudemonData();

	SUnionData* FindUnion(DWORD unionID);
	SUnionData* FindUnion(const char* unionName);

	// 活动
	bool IsFunActive(bool fix);
	bool IsFunTime(bool fix);
	void StartFun(bool fix);
	bool IsInFunMap(CRegionUser *user);
	void EndFun();
	void KickFunUser(CRegionUser *user);
	
	bool  m_bFixFunActive;
	bool  m_bTmpFunActive;
	long  m_funBegTime;
	long  m_funInterval;
	vector<string> m_funMap;

	//逐鹿
// 	bool IsZhuluActive(bool fix);
// 	bool IsZhuluTime(bool fix);
// 	void StartZhulu(bool fix);
// 	bool IsInZhuluMap(CRegionUser *user);
// 	void EndZhulu();
// 	void KickZhuluUser(CRegionUser *user);
// 	void OnEnterZhulu(CRegionUser *user);
// 	void OnZhuluWin(char faction);
// 
// 	bool  m_bFixZhuluActive;
// 	bool  m_bTmpZhuluActive;
// 	long  m_ZhuluBegTime;
// 	long  m_ZhuluInterval;
// 	char  m_ZhuluWinFaction;
// 	vector<string> m_ZhuluMap;
// 	std::list<ULONG> m_ZhuluActiveUser;

    struct SRandomImage
    {
        int     iDataSize;
        //char    szStrBuf[4];
        //unsigned char szDataBuf[32*32*4];
		CM_MEMDEF(szStrBuf, 4)
		CM_MEMDEF(szDataBuf, 32*32*4)

		SRandomImage(void)
		{
			CM_MEMPROTECTOR(szStrBuf, 4)
			CM_MEMPROTECTOR(szDataBuf, 32*32*4)
		}
		~SRandomImage(void)
		{
			CM_MEMUNPROTECTOR(szStrBuf)
			CM_MEMUNPROTECTOR(szDataBuf)
		}
    };
    SRandomImage* CreateRandomImage();

public:
	CRegionCreature    *m_cmdGiver;
	CG_CmdPacket	   *m_activeCmd;
	SNetLink		   *m_activeLink;
	CRegionScene       *m_defScene;
	// vector<CRegionScene*> m_fbList;
	ULONG				m_time;
	ULONG				m_sec;
	bool				m_canAcceptNewLink;
    SStatRegionServer   m_stat;

	// 战场
	CWarZhulu			*m_pWarZhulu;

	// 道具 
	CItemManager		*m_pItemManager;
    RsCardTrade         m_rsCardTrade;
    RsAuction           m_rsAuction;
	rs_LineMan			m_rsLineMan;

    list<DWORD>         m_serviceNpcList;   // 请注意：这个列表只用于显示，不做其他用途，也不安全

	ULONG m_shopnpcoid;
private:
    CG_UDPSession       m_udpEudemonSession;
    CG_NetAddress       m_eudemonToAddress;
    CG_CmdPacket        m_cmdEudemon;
    CG_TCPBroadcastArchive* m_pNetLog;
    RtRandomImage*      m_pRandomImgFactory;
    SRandomImage        m_randomImageData;

    list<SNetLink*>		m_unloginLink;			// unlogin user	
	CG_TCPListener		m_clientListen;			// listen socket for user

	// channel with user 
	CCmdChannel			m_channelUnlogin;
	CCmdChannel			m_channelLogin;

	// id for temp net link 
	ULONG				m_linkId;

    long                m_lKickAllUserTime;
    DWORD               m_dwKickAllUserLastTickTime;

	float               m_expRate;

public:
	CRsDungeonMgr      *m_fbMgr;
	CRegionGWSession	m_gws;
	GWT					m_gwt;
	SHeroList			m_HeroList;
	CRegionMasterListManager   m_MasterList;
    long                m_lCurRegionID;
	EXT_SPACE::unordered_map<ULONG, SUnionData>		m_unionMap;
	EXT_SPACE::unordered_map<ULONG, CRegionUser*>    m_userMap;      // login user
	EXT_SPACE::unordered_map<ULONG, long>            m_regionMap;    // EXT_SPACE::unordered_map<RtsSceneBlockRtb*, long>
	float GetExpRate() { return m_expRate; }

	int  m_yDay;
	void OnDayChange();
	void AddCheater(CRegionUser *user);
	EXT_SPACE::unordered_map<ULONG, ULONG> m_cheaterMap;
	bool m_bKickCheater;
    int  m_iCheaterValve;
    int  m_iCheaterCnt;
	/*
	int  m_maxCheatNpc;
	int  m_curCheatNpc;
	int  CanSpawnCheatNpc() {
		return (m_curCheatNpc<m_maxCheatNpc*0.1f);
	}
	*/

	void OnUserEnter(CRegionUser *user);
	void OnUserQuit(CRegionUser *user);
	bool UserUploadString_IsValid(const char* str); // 判断玩家上传的字符串是否合法


    // 外挂进程检查
    bool                        m_bAutoProcessCheck;    // 是否自动进行进程检查
    int                         m_iMinCheckTime;        // 最小检查间隔
    int                         m_iMaxCheckTime;        // 最大检查间隔
    std::vector<std::string>    m_listWgProcesses;      // 使用外挂的进程

    int     m_iWgCheckedTotal;
    int GetCurrentCheckedWgUser();

    // 返回false表示找到外挂
    void ConnectJhWgServer();
    bool JhWgCheck(int iCheckData, long lClientSeed, short cmdID);
    void ReadJhWgCheckConfig(const char* szFilename);
    void OnJhWgServerUserProgram(long lActorID, short sProgramID, long lSecond, const char* szMsg);
    bool                        m_bJhWgCheck;
    bool                        m_bJhWgCollect;
    bool                        m_bJhWgServer;
    int                         m_iJhWgPunish;
    int                         m_iJhWgInterval;
    std::string                 m_strJhWgServerHost;
    std::string                 m_strJhWgBindIP;
    std::string                 m_strLoginName;
    std::string                 m_strGameworldName;
    unsigned long               m_dwClientCheckCode[100];
    int                         m_iClientCheckCodeCnt;
    CJhWgSession*               m_pJhWgServer;

	// lua check
	bool IsHashError(int id,CRegionUser *user,int hash);

	bool SendUserLogEvent(char type, long userID, const char* ip, long longParam1, const char* strParam1, long longParam2, const char* strParam2, long longParam3, const char* strParam3);

	// LogServerServer
	CG_UDPSession m_LogServerSession;
	bool LogServerSender_Init();
	bool LogServerSender_Close();
	bool LogServerSender_Send(CRegionUser* pUser, const char* szTitle, const char* szContent);

	//--------------add start by tony 06.06.02--------------------------//
	//- 防私服
	CRegionLsSession  m_AuthSession;
	void GetCheckCode(const char* szCode, char* szCheckCode);
	bool AuthorizationCheck(const char* szListenIP, const char* szConnectIP="licence.zfs.gamigo.com.cn", int iConnectPort=9000); // szListenIP为监听地址
	void Disturb(char* data, char a, char r, char g, char b);
	void UnDisturb(char* data);
	//--------------add end   by tony 06.06.02--------------------------//

	public:
		CMonCastle *m_mc;//tim.yang  MC
		void ReadUseCardMinBottleMaxNum();//从配置文件读取最大使用次数
		bool WriteUseCardMinBottleMaxNum(int num);//将最大使用次数写入配置文件
		void SetUseCardMinBottleMaxNum(int num);//设置最大使用次数
		int GetUseCardMinBottleMaxNum();//得到最大使用次数
	private:
		int m_useCardMinuteBottleMaxNum;//双倍经验瓶一天内使用的最大次数
};

//typedef void (*FP_GM_PUNISHCALLBACK)(unsigned int userID, int action, int value);

extern void GMPunishWGCallback(unsigned int userID, int action, int value);
extern std::string	g_strConnectLicenseIP;
extern long			g_strConnectLicensePort;

#endif
