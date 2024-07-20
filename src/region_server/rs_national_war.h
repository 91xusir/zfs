#ifndef _INC_RS_NATIONALWAR_H_
#define _INC_RS_NATIONALWAR_H_

// 国战系统
/*****************************************************************************/
#define WARFLAG_ZHOU		1247
#define WARFLAG_SHANG		1246
#define WF_SCORE_MAX		0xffff
#define WF_ONEHOUR			60 * 60
/*****************************************************************************/
#define WF_HSCORE_P			1000*0.6		//初级神仙分数
#define WF_HSCORE_I			2000*0.6		//中级神仙分数
#define WF_HSCORE_S			3000*0.6		//高级神仙分数
/*****************************************************************************/

class CRegionSpawnMgr;

enum EConjureRet
{
	eSuccess		= 0,	// 已经将资源用于召唤
	eFailed			= 1,	// 失败, 不知道为什么
	eHaveConjured	= 2,	// 失败, 早就召唤出来了(指神仙)
	eTimeLimit		= 3,	// 失败, 时间未到
	eOverFlow		= 4,	// 失败, 数量满了
	eNoMoney		= 5,	// 失败, 金钱不足
};

enum EArmsKind
{
	eBuBing		= 0,	//步兵
	eGongjian	= 1,	//弓箭手
	eJiangjun	= 2,	//将军
	eNone
};

const int iArmsPrice[3] = {250000, 500000, 3000000};	//价格
const int iArmsRate[3]	= {50, 30, 3};					//每小时多少个

struct SExploit
{
	DWORD	dwUnionID;
	char	faction;	//阵营
	long	lScore;		//积分
	long	lExploit;	//战功

	SExploit()
	{
		dwUnionID = 0;
		faction = FACTION_UNKNOWN;
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

	bool SerializeToGC(CG_CmdPacket *cmd)
	{
		EXT_SPACE::unordered_map<ULONG, SUnionData>::iterator iter_union;
		iter_union = g_region->m_unionMap.find(dwUnionID);
		if(iter_union == g_region->m_unionMap.end())
		{
			return false;
		}

		*cmd << iter_union->second.m_strName;
		//*cmd << dwUnionID;
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

struct SArms
{
	EArmsKind	kind;
	char		faction;
	time_t		tStartTime;
	long		lCurrNum;

	SArms()
	{
		kind = eBuBing;
		faction = FACTION_UNKNOWN;
		tStartTime = 0;
		lCurrNum = 0;
	}
};

struct SRsNWar
{
	CRsSceneWarfield*	pScene;
	SWarfieldInfo		stInfo;
	std::list<DWORD>			listUserZhou;		// 周朝玩家
	std::list<DWORD>			listUserShang;		// 商朝玩家
	std::list<SExploit>			listExploit;		// 战功列表
	std::list<SPerExploit>		listPersonExploit;	// 个人战功列表
	time_t				tEndTime;			// 结束时间(60秒缓冲)
	time_t				tRemainTime;		// 剩余时间长度（秒）
	float				lScoreZhou;			// 周朝得分
	float				lScoreShang;		// 商朝得分
	float				lPriHero[3];		// 初级神仙获得的资源
	float				lInterHero[3];		// 中级神仙获得的资源
	float				lSeniorHero[3];		// 高级神仙获得的资源
	std::list<SArms>	listArms;			// 双方召唤的Npc
	float				fRZhou;				// 周比率
	float				fRShang;			// 商比率

	SRsNWar()
	{
		pScene = NULL;
		listUserZhou.clear();
		listUserShang.clear();
		listExploit.clear();
		listArms.clear();
		listPersonExploit.clear();
		tEndTime		= 0;
		lScoreZhou		= 0.f;
		lScoreShang		= 0.f;
		tRemainTime		= 0;
		fRZhou			= 1.0;
		fRShang			= 1.0;
		lPriHero[0]		= lPriHero[1]	 = 0.f;
		lInterHero[0]	= lInterHero[1]  = 0.f;
		lSeniorHero[0]	= lSeniorHero[1] = 0.f;
		lPriHero[2]		= WF_HSCORE_P;
		lInterHero[2]	= WF_HSCORE_I;
		lSeniorHero[2]	= WF_HSCORE_S;
	}

	~SRsNWar()
	{
		//if(pScene)
	}

	void Reset()
	{
		listUserZhou.clear();
		listUserShang.clear();
		listExploit.clear();
		listArms.clear();
		listPersonExploit.clear();
		tEndTime		= 0;
		lScoreZhou		= 0.f;
		lScoreShang		= 0.f;
		tRemainTime		= 0;
		fRZhou			= 1.0;
		fRShang			= 1.0;
		lPriHero[0]		= lPriHero[1]	 = 0.f;
		lInterHero[0]	= lInterHero[1]  = 0.f;
		lSeniorHero[0]	= lSeniorHero[1] = 0.f;
		lPriHero[2]		= WF_HSCORE_P;
		lInterHero[2]	= WF_HSCORE_I;
		lSeniorHero[2]	= WF_HSCORE_S;		
	}

	bool Serialize(CG_CmdPacket *cmd);
};

class CRsNwarMgr : public CRegionObject
{
public:
    CRsNwarMgr();
    virtual ~CRsNwarMgr();

public:
	bool Load(const char* file);
	bool Exit();
    
	//处理在region上发生的事件
	void Run();

    bool CreateWarfield();
	void Stop();
    void DestroyWarfield();


    // 添加玩家
	bool AddUserToNWar(DWORD userId, char cBorn, long x, long y);
	bool _AddUserToNWar(CRegionUser *user, char cBorn, long x, long y);
	
	//寻找离自己最近的己方复活点   
	void FindRebirthPos(long* pos, CRegionCreature* cre);
    //移除玩家
	CRegionUser* RemoveUser(CRegionUser *user);
	//请求进入	
	const char* RequestEnter(CRegionUser *user,char Faction);
	//请求离开
	bool RequestLeave(CRegionUser *user);
	
	//奖励
	void Reward();	
	//添加经验
	void AddExp(DWORD dwUserID, char faction);	
	//添加个人战功保存至gw
	void AddPersonExploit(DWORD dwUserID, DWORD dwExploit);	
	//添加诸侯国战功
	void AddExploit(DWORD dwUnionID, char faction, long lExploit);	
	//临时添加个人战功
	void AddPerTmpExploit(DWORD dwUserID, DWORD dwExploit, bool IsWinnerReward = false);
	//临时添加诸侯国战功
	void AddTmpExploit(DWORD dwUnionID, char faction, DWORD dwExploit);
	//添加个人积分(会累加到本阵营积分中)，同时会添加相应战功
	void AddUserScore(DWORD dwUserID, long res);
	//添加阵营积分
	void AddScore(char faction, float score);
	//更新比率
	void UpdateRate(float fRZhou, float fRShang);

	void TransmitMsg(long dwUserID, long iMsgID);
	long Distance(long *pos, CRegionCreature* cre);	
	bool IsInWarfield(CRegionCreature* p_cre);		//角色在战场内
	bool CreateRebirthNpc(SRebirthPos* pos);
	bool RemoveNpc(long lObjID);
	bool RemoveNpc(CRegionCreature* cre);
	bool AddNpc(SRebirthPos* pos);
	void TransUser();

	//玩家申请更新
	void UpdateClientData(CRegionCreature* CmdGiver);
	//服务器主动更新
	void SendUpdateData();
	//战场结束时发送战报
	void SendReport();

	//诸侯国战功存贮
	void SaveDataToGW();

	EConjureRet	 ConjureHero(long dwUserID, long dwResource, int lev);
	EConjureRet	 ConjureHero(char faction, long dwResource, int lev);
	EConjureRet	 ConjureArms(long dwUserID, EArmsKind kind);
	
	//check
	bool	CanConjureHero(char faction, int lev);		//可否召唤神仙
	bool	IsRebirthNpc(short npcID);
	bool	IsFlagNpc(short npcID);
	EArmsKind	IsArmsNpc(short npcID);

private:
	//添加诸侯国积分(会累加到本阵营积分中)
	//void AddUnionScore(DWORD dwUnionID, char faction, long score);
	void Reward(char faction);

public:
    // 
    bool OnNWarUserLeave(CRegionUser *user);

	void OnFlagIsDestroy(long flag_id);

	void OnUserDead(CRegionCreature* cre);

	void OnRebirthPosKilled(CRegionCreature* cre, CRegionCreature* killer);

	void OnRebirthPosBeAttacked(CRegionCreature* cre);

	void OnFlagBeAttacked(long flag_id, CRegionCreature* cre);

	void OnConjureHero(char faction, int lev);	//可以召唤出神仙

	void OnConjureArms(CRegionCreature* pUser, char faction, EArmsKind kind);	//召唤出普通防守npc

	void OnRecvAddScore(float lScoreZhou, float lScoreShang);

    void BroadcastSystemMessage(char* szMessage, char faction = FACTION_ALL);

	void BroadcastMessage(char* szMessage, BYTE channel, long from_id, const char* from_name, char faction = FACTION_ALL);

	void OnRecvUpdateData(  long lRemain, long numZhou, std::list<DWORD>* pUserZhou, float lScoreZhou, long numShang, std::list<DWORD>* pUserShang, float lScoreSHang,
							float* lPriHero, float* lInterHero, float* lSeniorHero,
							std::list<SExploit>* pExploit, std::list<SPerExploit>* pPerExploit);

public:
    static int s_iMaxLastQuitTime;

public:
    //char			m_szMsg[256];
	CM_MEMDEF(m_szMsg, 256)
	SRsNWar*		m_pWarfield;
	bool			m_bFlagDestroyedZhou;
	bool			m_bFlagDestroyedShang;
	bool			m_bIsOpen;				//战场逻辑上是否开启
	bool			m_bIsWaitingStop;		/* 战场实际上是否开启
											 * 如果m_bIsOpen==false不代表真正关闭
											 * 此变量为true时表示正在进行战场结束预处理
											 */
	CRegionSpawnMgr* m_pSpawnMgr;
	bool			m_bCanTrans;

	//..................
	int	m_iNumZhou;
	int m_iNumShang;
 };

extern CRsNwarMgr	g_WarfieldMgr;

#endif // _INC_RS_NATIONALWAR_H_
