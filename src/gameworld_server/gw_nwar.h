#ifndef _INC_GW_NATIONALWAR_H_
#define _INC_GW_NATIONALWAR_H_

/*****************************************************************************/
#define WF_T					3			//国战持续时间（小时）
#define WF_NUM_LIMIT			200			//国战人数上限
#define WF_END_TIMELIMIT		0			//时间到，国战结束
#define WF_END_FLAGDESTROYED	1			//令旗被摧毁，国战结束
#define WF_SCORE_MAX			0xffff		//极大分数
/*****************************************************************************/
#define WF_APP					0			//操作：对申请列表
#define WF_IN					1			//操作：对参战列表
#define WF_ALL					2			//操作：对所有国战相关玩家
/*****************************************************************************/
#define WF_JOIN_NUM				5			//每次传送人数
/*****************************************************************************/
#define WF_HSCORE_P				1000*0.6	//初级神仙分数
#define WF_HSCORE_I				2000*0.6	//中级神仙分数
#define WF_HSCORE_S				3000*0.6	//高级神仙分数
/*****************************************************************************/
#define WF_EXP_20_40(lev)	g_TableExp.GetNextLevExp((lev)) * 20 / 100  //20-40
#define WF_EXP_40_60		2000000										//40-60
#define WF_EXP_60_80		5000000										//60-80
#define WF_EXP_80_100		40000000									//80-100
/*****************************************************************************/
struct SExploit
{
	DWORD	dwUnionID;
	char	faction;	//阵营
	long	lScore;		//积分
	long	lExploit;	//战功

	SExploit()
	{
		dwUnionID = 0;
		faction = FACTION_NEUTRAL;
		lScore = 0;
		lExploit = 0;
	}

	bool Serialize(CG_CmdPacket *cmd)
	{
		*cmd << dwUnionID;
		*cmd << faction;
		*cmd << lExploit;

		if (cmd->IsError())
			return false;
		else 
			return true;
	}
};

inline bool operator> (const SExploit& lhs, const SExploit& rhs)
{
	return (lhs.lExploit > rhs.lExploit);
}

struct SPerExploit
{
	DWORD	dwUserID;
	DWORD	dwExploit;

	SPerExploit()
	{
		dwExploit = 0;
		dwUserID = 0;
	}
};

class CGwNationalWarMgr
{
public:
	enum EState
	{
		eStarting		= 0,
		eWaitStarting	= 1,
		eStop			= 2,
		eWaitStopping	= 3
	};
private:
	struct SUser
	{
		DWORD dwUserID;                // 用户的ID
		bool  bInNWar;                 // 是否在国战场景
		DWORD dwTimeout;			   // 断线超时多少
		DWORD dwLevel;				   // 玩家等级

		SUser()
		{
			dwUserID = 0;
			bInNWar = false;
			dwTimeout = 0;
			dwLevel = 0;
		}
	};

	struct SAppUser
	{
		DWORD	dwUserID;               // 用户的ID
		time_t	tStartTime;				// 开始排队时间
	};

	typedef std::list<SAppUser>		TLSApp;
	typedef std::list<SUser>		TLSUser;
	typedef std::list<SExploit>		TLSExploit;
	typedef std::list<SPerExploit>	TLSPerExploit;

	struct SWarField
	{
		long				lRsidx;				// 战场所在region服务器
		EState				eState;				// 开始否
		TLSApp				listAppZhou;		// 申请列表周
		TLSApp				listAppShang;		// 申请列表商
		TLSUser				listUserZhou;		// 参战列表周
		TLSUser				listUserShang;		// 参战列表商
		TLSExploit			listExploit;		// 诸侯国战功列表
		TLSPerExploit		listPersonExploit;	// 个人战功列表
		time_t				tStartTime;			// 开始时间
		int					iDuration;			// 极限时间（小时）
		time_t				tEndTime;			// 结束时间
		float				lScoreZhou;			// 周分数
		float				lScoreShang;		// 商分数
		float				lPriHero[3];		// 初级神仙获得的资源
		float				lInterHero[3];		// 中级神仙获得的资源
		float				lSeniorHero[3];		// 高级神仙获得的资源
		SWarfieldInfo		stInfo;				// 战场基本信息
		float				fRateZhou;			// 周战场比率
		float				fRateShang;			// 商战场比率

		SWarField()
		{
			lRsidx			= 0;
			eState			= eStop;
			tStartTime		= 0;
			iDuration		= WF_T;
			tEndTime		= 0;
			lScoreShang		= 0.f;
			lScoreZhou		= 0.f;
			fRateZhou		= 1.0;
			fRateShang		= 1.0;
			lPriHero[0]		= lPriHero[1]	 = 0.f;
			lInterHero[0]	= lInterHero[1]	 = 0.f;
			lSeniorHero[0]	= lSeniorHero[1] = 0.f;

			lPriHero[2]		= WF_HSCORE_P;
			lInterHero[2]	= WF_HSCORE_I;
			lSeniorHero[2]	= WF_HSCORE_S;

			listAppZhou.clear();
			listAppShang.clear();
			listUserZhou.clear();
			listUserShang.clear();
			listExploit.clear();
			listPersonExploit.clear();
		}

		void Reset()
		{
			eState			= eStop;
			iDuration		= WF_T;
			lScoreShang		= 0.f;
			lScoreZhou		= 0.f;
			fRateZhou		= 1.0f;
			fRateShang		= 1.0f;
			lPriHero[0]		= lPriHero[1]	 = 0.f;
			lInterHero[0]	= lInterHero[1]	 = 0.f;
			lSeniorHero[0]	= lSeniorHero[1] = 0.f;
			lPriHero[2]		= WF_HSCORE_P;
			lInterHero[2]	= WF_HSCORE_I;
			lSeniorHero[2]	= WF_HSCORE_S;

			listAppZhou.clear();
			listAppShang.clear();
			listUserZhou.clear();
			listUserShang.clear();
			listExploit.clear();
			listPersonExploit.clear();
		}
	};

public:
	CGwNationalWarMgr();
	~CGwNationalWarMgr();

public:
	bool	Init();
	bool	Load(const char* file);
	bool	Start();
	bool	Stop();
	void	Run();
	bool	CanStart();
	int		EnterWarfield(DWORD dwUserID);
	void	GetStartTime(time_t& ret);
	void	BroadcastSystemMsg(const char* s_szMsg);		//给玩家
	void	BroadcastWarfieldMsg(int toWho, int iMsgID);	//给玩家
	void	SendWarfieldMsg(long dwUserID, int iMsgID);		//给玩家
	int		GetUserNum(int faction);
	void	BroadcastCmd();
	void	BroadcastUpdateData();	//给region
	bool	Log(DWORD dwUserID);
	SUser*	FindUser(DWORD dwUserID);
	bool	FindUserInApp(DWORD dwUserID);
	bool	RemoveUserInApp(DWORD dwUserID);
	void	CalcRate();
	void	UpdateRateToRs();
	void	AddAppUserToWarfield();
	//void	ClearDB();
	void	AddHeroItem(char faction);
	void	AddHeroItem(char faction, DWORD dwUnionID, int lev);

public:
	void	OnRsStartRep(bool bResult);
	void	OnUserLeave(DWORD dwUserID);
	void	OnRsStopRep(bool bResult);
	void	OnRegionRegister(int regionID);
	void	OnRegionDisconnected(int regionID);
	void	OnUserOffline(DWORD dwUserID);
	void	OnAddScore(char faction, float score);
	void	OnUserOnline(DWORD dwUserID);
	void	OnRecvConjureProgress(char faction, float progress, long lev);
	void	OnRecvExploit(char faction, long lUnionID, long lExploit);
	void	OnRecvPerExploit(DWORD dwUserID, DWORD dwExploit);
	void	OnRecvUpdateData();
	void	OnAddExp(DWORD dwUserID, char faction);
	void	OnAddPersonExploit(DWORD dwUserID, long lExploit);
	//void	OnCreateArms(DWORD dwUserID, long dwLev, long lType);

	void	OnGMStart();
	void	OnGMStop();

public:
	SWarField		m_stWarField;
	const char*		m_szErrorInfo;  
	bool			m_bCanStart;
	bool			m_bHad;			//这个礼拜是否已经打过
	bool			m_bFlagDestroyedZhou;
	bool			m_bFlagDestroyedShang;
	char			m_Winner;

	//-----------------------------
	//默认国战时间
	//-----------------------------
	int m_default_weekday;

	int m_default_hour;

	int m_default_min;

	int m_default_duration;
};

extern CGwNationalWarMgr g_GWWarfieldMgr;

#endif //_INC_GW_NATIONALWAR_H_
