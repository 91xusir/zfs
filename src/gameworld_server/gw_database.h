#ifndef _INC_GW_DATABASE_H_
#define _INC_GW_DATABASE_H_
//PZH
#include<string>
//
class  CTeamData;
struct GWDBResult;
struct GWUser;

// ------------------------------------------------------------------------------------
// 未经过login server认证的client
// 三次登录失败自动断开连接，每次登录不得小于1秒
// ------------------------------------------------------------------------------------
const int MAX_LOGIN_FAILED_CNT = 4;//PZH
const int MIN_LOGIN_INTERVAL   = 1000;

enum GWUnloginClientState
{
	UCS_IDLE,
	UCS_CHECKING_ACCOUNT,
};

class GWUnloginClient
{
public:
	GWUnloginClient()
	{
		loginFailedCnt	= 0;
		lastLoginTime	= 0;
		seed			= 0;
		connectTime		= 0;
		netLink			= NULL;
	};
	virtual ~GWUnloginClient()
	{

	}
	void SendPacket(CG_CmdPacket *packet)
	{
		if(netLink) netLink->SendPacket(packet);
	}

	int						loginFailedCnt;
	int						lastLoginTime;
	int                     seed;
	int                     connectTime;
	bool                    valid;
	CG_TCPSession          *netLink;
	GWUnloginClientState    state;
//PZH  为了实现锁定账号一段时间的功能，以下添加了账号名称的字段,还增加了是否强制登陆的功能
public:
	void SetAccountName(const char* szName)
	{
		m_strAccountName = szName;
	}
	const char* GetAccountName()
	{
		return m_strAccountName.c_str();
	}
	bool IsPowerLogin()
	{
		return bPowerLogin;
	}
	void SetPowerLogin(bool bPower)
	{
		bPowerLogin = bPower;
	}
private:
	std::string m_strAccountName;
	bool bPowerLogin;
	//
};

// ------------------------------------------------------------------------------------
// 已经认证通过的client,有两种状态，选人状态或进入游戏
// ------------------------------------------------------------------------------------
enum GWLoginClientState
{
	LCS_INGAME,
	LCS_SELECT_CHAR,
	LCS_SWITCH_REGION,
	LCS_QUITING
};

class GWLoginClient
{
public:
	GWLoginClient();
	virtual ~GWLoginClient();

	int						actorCnt;
	int                     kickCnt;
	CG_TCPSession          *netLink;
	bool                    valid;
    bool                    m_bKickInSwitchRegion;
    char                    m_cKickCause;
    //char                    m_szKickInfo[50];
	CM_MEMDEF(m_szKickInfo, 50)
	int                     m_switchRegionSeed;
	int                     m_switchRegionTime; 
	int                     m_switchRegionCnt;
	int						m_regionId;
	int                     m_lastSaveTime;
	
	struct UserInfo
	{
		int				userID;			
		string			name;
		string			deleteDate;
		SUserAttr		attr;
		string			items;
		short			headModelID;
		//PZH
		bool			bHasPassword;
		bool			bDelete;//是否冻结状态
		//
	} users[GW_MAX_USER_PER_ACCOUNT];

	SAccountInfo			 info;
	GWUser					*actorData;
	GWLoginClientState       state;
	//PZH
	void SetPower(unsigned long dwSeed/*GWUnloginClient *ulc*/);
	bool IsPower();
	void PowerLogin();
	//GWUnloginClient *ulc;
	unsigned long dwulcSeed;
	long lPoint;
    char cPointType; // 1 点数, 2 时间
    char cVIP;
	//

	bool IsFirstSwitchRegion() { return m_switchRegionCnt == 1; }
	bool SendPacket(CG_CmdPacket *packet);
	GWLoginClientState GetState();
	void SetState(GWLoginClientState st);
	void OnSwitchRegion();
};

/*
enum GWUserState
{
	usIdle,	
	usLogin,
	usSwitchRegion,
	usBusy
};
*/

// gameworld上的角色信息，包括该角色的帐号信息
struct GWUser : public SUserData
{
	GWUser();
	~GWUser(){};

	// SUserData           data;
	// int				   userID;
	string              accountName;
	int                 accountId;
	int					regionID;
	long                enterTime;
	string				uname;
	CTeamData*			team;
	long				dwLastDelUnion;
	long				dwLastRemoveMPTime;
	long				dwWithdrawMPGift;
	vector<SGift>		giftsWithdrawed;			// 已经领取的礼品列表
	long				dwWithdrawGiftLockTick;		// 领取礼物lock的时间
	int					isModified;
	int					updateTimeStamp;
	string				lastMailDate;
	int					switchRegionTime;
	bool                bankModified;
	bool                m_bRefuseTeam ;       //设置是否拒绝组队
	bool                m_bRefusetrade;       //设置是否拒绝交易
	bool                m_bRefuseMail;        //设置是否拒绝邮件
	bool                m_bRefuseDuel;        //设置是否拒绝决斗
	bool                m_bRefuseFriend;      //设置是否拒绝好友
	bool                m_bRefuseMonster;     //设置是否拒绝师徒
	
	// string				scene;
	/*
	// int				seed;

	string				teams;

	long				dwState;
	SMemoryBlock        MyData;
	*/
};

struct GWActor
{
	SUserAttr			attr;
	string				metierName;	
	string				skills;
	string				items;
	string              BodyItem;
	string				zhouScene;
	long                zhouCreate;
	long                shangCreate;  
	long				zhouPosition[2];
	string				shangScene;
	long				shangPosition[2];
	/*Tianh*09.12.03*/
	string              neutralscene;  //开始创建人物为中立
	long                neutralCreate;
	long                neutralPosition[2];
};

//struct GWGuild
//{
//	int				guildID;
//	string			guildName;
//	list<int>		members;
//};

enum GWDBCommandType
{
	dbCmdQuery,
	dbCmdUpdate,
	dbCmdInsert,
	dbCmdDelete
};

typedef void (*GWDBCallback)(GWDBResult* data);
struct GWDBCommand
{
	GWDBCommandType	type;
	GWDBCallback	succeeded;
	GWDBCallback	failed;
	void*			data;
	//char			command[GW_MAX_DB_COMMAND_LENGTH];
	CM_MEMDEF(command, GW_MAX_DB_COMMAND_LENGTH)

	GWDBCommand()
	{
		CM_MEMPROTECTOR(command, GW_MAX_DB_COMMAND_LENGTH)

		succeeded = NULL;
		failed = NULL;
		data = NULL;
		command[0] = '\0';
	}
	GWDBCommand(const GWDBCommand &object)
	{
		memcpy(this, &object, sizeof(GWDBCommand));
		CM_MEMPROTECTOR(command, GW_MAX_DB_COMMAND_LENGTH)
	}
	~GWDBCommand()
	{
		CM_MEMUNPROTECTOR(command)
	}
};

struct GWDBResult
{
	bool			okay;
	GWDBCallback	succeeded;
	GWDBCallback	failed;
	void*			data;
	MYSQL_RES*		res;
	int				affectRows;
};

typedef GWDBAsyncQueue<GWDBCommand>	GWDBCommandQueue;
typedef GWDBAsyncQueue<GWDBResult>	GWDBResultQueue;

class GWDatabase : public RtThread
{
	friend class GWServer;

public:
	typedef EXT_SPACE::unordered_map<int, GWUser>	GWUserTable;
	typedef vector<GWActor>						GWActorTable;

	bool			Initalize();
	void			Close();
	bool			IsValid() { RtThreadLock::AutoLock lock(m_cmdLock); return m_mysql != NULL && m_connectTime != 0; }
    bool			ExecCommand(const char* command, MYSQL_RES** res, bool bLogErr=true);
    bool			ExecCommandRow(const char* command, MYSQL_RES** res, int* iAffectedRows);
	bool			ExecCommandEx(const char* command, MYSQL_RES** res, int* iAffectedRows, bool bLogErr);
	void			Stop();
	bool			PushCommand(GWDBCommand& cmd) { return m_commandQueue.Push(cmd); }
	GWUser*			LoadUserData(DWORD accountID, int userID);
	GWActor*		LookupActor(int actorID);
	unsigned long GetInsertId();
protected:
	virtual int		OnRun();

public:
	void				ProcessQueryResult();
	bool				QueryActorList(GWLoginClient& client);
	bool				SaveUserData(GWUser* user, bool sync, int param, GWDBCallback callback);
	GWCreateUserResult	CreateUserData(GWUser* user);
	GWCreateUserResult	CreateUser(DWORD accountID, const string& name, int actorID, short headModelID, int factor, std::string password, GWUser* outputData,int Sex,short headImageID,short iAnimal);
	bool				DeleteUser(int index);
	bool				RestoreUser(int userID);	
	void				LoadActorTable();
	/*void                SendPlayerInfo();*/
	bool				IsValidName(const string& name);
	void				EscapeString(string& result, const string& str);
	void				EscapeString(char* result, const char* str);
	std::string			EscapeString(const char *str, int size); 

	static void			OnSaveUserData(GWDBResult* result);
	bool				Connect();
	bool				SaveCharPassword();

public:
	GWDatabase();
	virtual ~GWDatabase();

	GWDBCommandQueue		m_commandQueue;
	GWDBResultQueue			m_resultQueue;	

private:
	MYSQL*					m_mysql;	
	GWActorTable			m_actors;	
	RtThreadLock			m_cmdLock;

	/*
	int						m_saveUserDataInterval;
	int						m_saveUserDataIterator;
	int						m_saveUserDataTime;
	*/
	bool					m_shutdown;
	int						m_connectTime;

public:
	// GWUserTable				m_users;
	long GetDeleteLevel();
	void SetDeleteLevel(long dwDeleteLevel);
private:
	long m_dwDeleteLevel;
};

#endif // _INC_GW_DATABASE_H_
