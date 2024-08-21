#ifndef __REGION_CREATURE_H__
#define __REGION_CREATURE_H__


const int DEFAULT_CHAT_RANGE			= 400;
const int DEFAULT_CHAT_NEWS             = 1024;
//const int CHEATER_CHECK_INTERVAL		= 5000;
const int CHECK_CARD_INTERVAL           = 24000;

const int CRE_TIMER_HEARTBEAT			= OBJECT_C_TIMER_BASE + 1;
const int CRE_TIMER_AI					= OBJECT_C_TIMER_BASE + 2;
const int CRE_TIMER_CHEATER_CHECK		= OBJECT_C_TIMER_BASE + 3;
const int CRE_TIMER_MONITOR				= OBJECT_C_TIMER_BASE + 4;
const int CRE_TIMER_PRENTICE			= OBJECT_C_TIMER_BASE + 5;
const int CRE_TIMER_CHECK_CARD          = OBJECT_C_TIMER_BASE + 6;
const int CRE_TIMER_KILL_SELF           = OBJECT_C_TIMER_BASE + 7;
const int CRE_TIMER_CHECK_ITEM_VALID    = OBJECT_C_TIMER_BASE + 8;
const int CRE_TIMER_HPANDMP             = OBJECT_C_TIMER_BASE + 9;//自动回血和蓝
const int CRE_TIMER_ENP                 = OBJECT_C_TIMER_BASE +10;//自动增加能量
const int CRE_TIMER_TIME                = OBJECT_C_TIMER_BASE +11;//从开始上线增加在线时间
const int CRE_TIMER_TIME2               = OBJECT_C_TIMER_BASE +12;//从开始上线增加在线时间 用于上线保护
const int CRE_TIMER_DIE_SELF            = OBJECT_C_TIMER_BASE + 13;
const int CRE_TIMER_STGTTIME            = OBJECT_C_TIMER_BASE + 14;

const int REBIRTH_PROTECT_MILLSECONDS			= 10000;
const int COMBAT_KEEP_MILLSECONDS				= 30000;
const int LEAVE_COMBAT_MILLSECONDS				= 6000;

#define GODLIGHT_MONSTER_COUNT  2//tim.yang 摩擦神灯产生最多的怪物数量
struct SAttackParam
{
	SAttackParam():crcCheck(0), Seed(0), Skill(0), TargetType(0), EnemyId(0), LuaCheck(0) {}
    unsigned short  crcCheck;
	short	        Seed;
	short	        Skill;
	char	        TargetType;
	DWORD	        EnemyId;
	long	        Pos[3];
	long            LuaCheck;

	/*
	short   Damage;
	short   EleDmg[MAX_ELEMENT_ATTR];
	*/
    bool Serialize(CG_CmdPacket& Cmd);
	//friend	CG_CmdPacket& operator<<( CG_CmdPacket& Cmd, SAttackParam &p); 
};

struct SKillReward
{
	SKillReward() 
	{
		Exp = 0;
        lSP = 0;
		Gold = 0;
		GoldRate = 0.0f;
	}

	long Exp;
    long lSP;
	long Gold;
	float GoldRate;
	// item
	// ...
};



struct STarget
{
	int Id;
	int Hate;
	int Damage;
};

typedef list<STarget> STargetList;

struct SMoveSpeedCheck
{
	SMoveSpeedCheck()
	{
		cur = 0;
		max = 0;
		speed = 1;
		ignore_time = 0;
	}

	void Init(int dis)
	{
		cur = 0;
		max = dis;
//		speed = 0;//不应该清零
//		ignore_time = 0;//不应该清零
	}

	void Init(int dis,int vSpeed)//add by fox //当速度突变时会有问题//要抛弃这次检查
	{
		cur = 0;
		max = dis;
		speed = vSpeed;
		//ignore_time = 0;//不应该清零
	}

	bool Valid(int dis)
	{
		if(ignore_time > 0 )
		{//要忽略
			ignore_time--;
			return true;
		}

		if(cur + dis > max) return false;
		cur += dis;
		return true;
	}

	bool Valid(int dis,int vSpeed,bool &IsDoTeleport)//add by fox //当速度突变时会有问题//要抛弃这次检查
	{
		if (IsDoTeleport)  //如果在瞬间移动中   抛弃检查
		{
			IsDoTeleport = false;
			return true;
		}

		if(ignore_time > 0 )
		{//要忽略
			ignore_time--;
			return true;
		}

		if(speed == vSpeed)
		{
			if(cur + dis > max) return false;
			cur += dis;
			return true;
		}
		else
		{//速度不同，要将最大值按速度比率变化，并清空cur//其实就是放弃当前检查
			cur = 0;
			if (speed != 0)	// Add this condition for crash Louis @ Tue 05 May 2009.
				max = max * vSpeed / speed ;
			speed = vSpeed;
			return true;
		}
		return false;
	}

	void Ignore(int vIgnoreTime)
	{
		ignore_time = vIgnoreTime; 
	}
	
	int cur;
	int max;

	int speed;
	int ignore_time;
};

const long TASK_STATE_NONE       = 0;
const long TASK_STATE_CAN_ACCEPT = 1;
// const long TASK_STATE_CAN_SELECT = 2;
const long TASK_STATE_CAN_FINISH = 3;
const long TASK_STATE_DOING      = 4;

const unsigned char CRE_FLAG_TALK   = 1;
const unsigned char CRE_FLAG_ATTACK = 1;

class CPlayerLogic;
class CRegionCreature : public CRegionObject
{
public:
	//PZH
	friend class CPlayerLogic;
	//

	enum CombatRule
	{
		COMBAT_RULE_AGGRESSIVE,
		COMBAT_RULE_DISCREET,
		COMBAT_RULE_GUARD
	};
	struct SSelfDef {
		string enemyName;
		DWORD enemyId;
		DWORD beginTime;
	};

public:
	CRegionCreature();
	virtual ~CRegionCreature();

	// invoke event command
	void InvokeEventCommand(const char *event);
	/*
	bool m_canBeAttack;
	bool m_canBeTalk;
	char m_flag;
	*/
	// cheater check
	void ResetCheaterCheck();
	SMoveSpeedCheck m_mvChk;
	SMoveSpeedCheck m_atChk;
    
	/* CRegionObject interface */
	virtual void OnTimer(ULONG id, DWORD dwParam[]);

	/* CRegionCreature interface */
	virtual bool Init(const char *param) { return true; }					// Initialize a creature with a param
	virtual void Interactive(CRegionCreature *target,CG_CmdPacket *cmd);	// Interactive with a cmd

	/* CRegionCreature event */
	void OnBeAttacked(CRegionCreature *enemy, int damage);
	long OnDead(CRegionCreature *killer);
	void OnPkOther(CRegionCreature *other);
	void OnBePked(CRegionCreature *killer);

	void OnTreasureDiscover(CRegionCreature *discover);

	void OnAddAccountTime(long lUserID, char* pAccountTime,char cSucceed);		//ac.ma
	void WriteAccountTime(int index);											//ac.ma
	
	void AddHp(int add,int IsFullHpShow = 0,bool Hit = false);
	//Tianh
	void BackHome();//回程
	void AddPlayerOnlineTime(long add);//用来计算在线累积时间 5小时清除一次 5小时加一次修为
	/* sync function */
	void RecvCmd(CG_CmdPacket *cmd);
	void Serialize(char type,CG_CmdPacket *cmd);
	void UpdateToClient(char type);
	void UpdateToClientDelta();

	void SendProblemToClient(long lProblemID);

	bool WriteSnapshot(CG_CmdPacket *cmd,CRegionCreature *target);
	bool WriteModelInfo(CG_CmdPacket *cmd);
	bool WriteCreateInfo(CG_CmdPacket *cmd);
	
	void AddCulExp(int Cul); //Tianh 增加修为
    void AddExp(int Exp, int SP);
	bool AddCardMinute(int minute);
	void Levelup(int iUpCount);
	void LevelDown(int iDownCount);
	long GetGold();
	void AddGold(int Gold);
	bool RemoveGold(int remove);
	void AddCredit(int add);
	void RemoveCredit(int remove) {
		m_core.Credit -= remove;
		if(m_core.Credit < 0) m_core.Credit = 0;
	}
	long GetCredit() {
		return m_core.Credit;
	}
    void AddExploit(int add);
    void RemoveExploit(int remove) {
        m_core.Exploit -= remove;
        if (m_core.Exploit < 0) m_core.Exploit = 0;
    }
    long GetExploit() { return m_core.Exploit; }

	bool IsUser() { return m_userInfo != NULL; }
	bool IsUserPet() { return (m_master && m_master->IsUser() && m_master->m_pet == this); }
	void SetVip(char vip) { m_isVip = vip; }
	char IsVip(){return m_isVip;}

//------------------------del start by tony 05.05.18-------------------------//
#if 0
@	bool IsMonster() { return !IsUser() && !IsUserPet(); }
#else
	bool IsMonster() 
	{ 
		return (!IsUser()   && 
			   !IsUserPet() &&
			   (m_type == OBJECT_TYPE_CREATURE) && !IsCallNpc());
	}
	bool IsSpirit() 
	{ 
		return (!IsUser()   && 
			!IsUserPet() &&
			(m_type == OBJECT_TYPE_SPIRIT));
	}
#endif
//------------------------del end   by tony 05.05.18-------------------------//
//------------------------add start by tony 05.05.18-------------------------//
	bool IsGuarder() { return (m_type == OBJECT_TYPE_NPC_COMBATIVE); }
//------------------------add end   by tony 05.05.18-------------------------//

	CRegionUser  *m_userInfo;
	CRegionScene *m_scene;
    long          m_dungeonUID;     // 副本唯一ID
    long          m_dungeonTID;     // 副本类型ID
    long          m_dungeonLastQuit;// 最后一次退出副本的时间
	DWORD		  m_dwGroupID;
	DWORD		  m_dwLineID;
	SCreature     m_core;
	SCreature     m_oldCore;
	long		  m_pos[3];
	char          m_posFlag;
	char		  m_dir;
	unsigned char m_modelRef;
    //bool          m_bStarted; // 必须收到客户端的start命令才会设置这个变量为true
	DWORD         m_lastEmoteTime;
	bool          m_bFunKick;
	bool		  m_bIsInWarfield;
	long		  m_lWarfieldLastQuit;
	char		  m_isVip;
	long		  m_lIsCombat;			// 玩家是否处于战斗状态 0否,1是.
	DWORD		  m_dwCombatEndTime;	// 玩家结束战斗时间.
	long          m_IsHiding;           //Tianh 是否隐身  有值就是在隐身  0为不隐身了

	bool          m_bTime;   //是否关闭上线保护

	long		  m_lTriggerTime;
	int			  m_nFBprocess;

	int           CulAwardCount;   //奖励修为值的次数
	int           localTime;  //保存当前的日子
 
	int           ExtractHpR;    //百分比抽取目标血量，但最多不能多于一个固定值
	int           ExtractMaxHp;  //抽取目标血量最大固定值

	int           m_ChangeModelID;  //保存变身后的模型ID //不变身为0
	bool          IsChengBody();
	
	int           m_ResistSwim; //抵抗眩晕 
	int           m_ResistLethargy;//抵抗昏睡
	int           m_ResistBleed;//抵抗流血
	int           m_ResistBluntness;//抵抗迟钝

	short         m_ReceiveDamageSkillR; //接收技能伤害倍率
	int           m_TeamShareDamage;  //队友分担伤害
	int           m_ScopeoDinaryInjury;//是否是范围普通伤害--溅射的伤害百分比(m_ScopeoDinaryInjury/100)

	int           m_EffectProbability;  //普攻触发特效的概率
	int           m_PTEffectID;         //触发特效的ID
	int           m_SkillSeries;        //要增加伤害的技能系列
	int           m_SkilldamageR;       //要增加伤害

	long          m_UltimateChange;   //是否是超级变身

	long          ReboundHurtR;    //反弹伤害
	long          ReboundHurtRType; //反弹伤害类型

	bool          IsDoTeleport;    //是否在瞬间移动中
	        
	// 机关NPC专用
	int		triggerID;			// 所属机关ID
	int		triggerOpen;		// 是否已经触发
	int		triggerMain;		// 是否机关主要表现物件
	long	triggerIsClose;		// 是否机关主要表现物件

	short   m_IsFirstLogin;     //是否是第一次登陆
	short   m_IsDead;           //是否死亡  死亡为1

	int	     tempItemID;		//ac.ma
	short    m_IsInvincible;    //是否无敌

	void  Setrefusesetting();

	float TemporaryElementArmor[MAX_ELEMENT_ATTR];
	float TemporaryElementArmor1[MAX_ELEMENT_ATTR];

	float ElementFactor[MAX_ELEMENT_ATTR];
	float ElementFactor1[MAX_ELEMENT_ATTR];

	void AddSkillSeries();

	void EnterCombat();
	bool IsInCombat();
	void LeaveCombat();
	void EnterDungeon();
	void LeaveDungeon();
	void StartTrigger(CRegionCreature *npc, bool AotoOpen = false);
	void UpdateTrigger();
	/* pet function */
	void PetSetInfo(int rate,bool counterAttack);
	bool RidePet();
	bool RideOffPet(bool bResetState = true);
	bool IsRided() { return m_master && m_master->m_bRidePet; }
	bool IsRidePet() { return m_pet && m_bRidePet; }
	bool AddPet(CRegionCreature *pet)
	{
		if(m_pet) return false;
		m_pet = pet;
		pet->m_master = this;
		return true;
	}
	bool AddHero(CRegionCreature *hero)
	{
		if(m_hero) return false;
		m_hero = hero;
		hero->m_master = this;
		return true;
	}
	bool ReleasePet(SItemID &item);
	bool CallbackPet(bool bSaveCallPet = false, bool bResetState = true);
	bool ReleaseHero(SItemID &item);
	bool CallbackHero();
	void UpdatePetDataByItem(SItemID& item);
	void SavePetDataToItem(CRegionCreature* pMaster);
	void UpdateEquipTrump(SItemID& newItem);

	PetType GetPetType();
	int	m_iGuardID;
	CRegionCreature *m_WardNPC;
	bool SetWarder(CRegionCreature *pWarder);
	bool RemoveWardNPC();
	CRegionCreature *m_master;
	CRegionCreature *m_pet;
	CRegionCreature *m_hero;
	bool			 m_bRidePet;
	long             m_petExpRate;
	bool             m_petCounterAttack;

	/* combat function */
	CRegionCreature *GetEntity();
	CRegionCreature *GetControl();
	bool IsInSafeArea();
	bool TryNormalAttack(CRegionCreature *target,int attRange);
	bool TryHit(CRegionCreature *target);
	void GetDamage(int &phyDmg,int eleDmg[MAX_ELEMENT_ATTR],float ScopeoDinaryInjuryR=1);
	bool TryStateHit(int param,CRegionCreature *target,SSkill* pSkillAttr);
	bool IsSkillCritical(int param, CRegionCreature *target);
	void GetSkillCriDamage(int &phyDmg,int eleDmg[MAX_ELEMENT_ATTR],CRegionCreature *target);

	void EquipCuff();

	void UnEquipCuff();

	std::vector<int> vSuitSkill; //天书和套装技能ID

	//<add by Tianh> 取消遁地或者隐身
	void ClearHidingAndTopo(); 
	bool ChangeWeapon();

	void CMoveParent(float *npos);

	void ChangeBoby(int ModelID);
	void SendChangeBosyToC(int ChangeModelID);
	long m_cuffDamageMin;
	long m_cuffDamageMax;
	long m_cuffAttRange;
	long m_cuffAttSpeed;
	//bool m_bDead;

	bool DoTeleportSkill(DWORD dwDistance);
	bool m_bBroadcastTele;

	bool CallSpirit(long dwSpiritID = 0);
	bool AddCatchSpiritState(CRegionCreature *cre = NULL);
	bool StopCatchSpirit(char cInfo = 0,bool broadcast = true);
	bool AddPetToBag(long dwPetID);
	bool AddPetToBagByItem(long itemID);
	long m_dwCatchMe;	// 宠物用,正在捕捉自己玩家.
	long m_dwCatchPet;	// 正在捕捉的宠物
	bool DropUnRide(float fRate = 1.f);
	bool LinkUpWithPet(long petItemID = 0,bool bResetState = true);
	long m_dwLinkUpPet;	// 合体的宠物
	bool IsLinkUpWithPet();
	void SendPetInfo();

    bool IsTeamFriend(CRegionCreature *cre); //Tianh 是否是自己的队友

	void Rebirth();
	void Rebirth(char type, int iHPrate = 0, int iMPrate = 0);
	
	bool AddPopularity(long sV,int Popularity);
	bool ToPopularity(int Popularity);
	bool LoadToPopularity(const char* szString);
	std::vector<long> vPopularity;
	string  m_popularity; //声望
	
	void UltimateChange(int sV);
    //Tianh 隐身
	void Hiding(int sV);
	void PlayerLurk(int sV);
	void AddSoul(int MonsterLev);
	void AddSelfDef(CRegionCreature *enemy);
	void RemoveTimeoutSelfDef();
	bool IsSelfDef(CRegionCreature *enemy);
	list<SSelfDef> m_selfDef;
	//<add by Tian> 删除生活技能
	void DeleteLifeSkill(unsigned short wID);

	//4.8 LeiJun
	void AttChangeMessageToClient(const char *sMessageType, int iAttValue);

    DWORD GetTargetParty(CRegionCreature* pTarget); // 取得敌人的类型，可能是友军，敌军，宏在skill_base.h中

    void DoAttack(long enemyId,long seed);
    bool CanAttack(CRegionCreature *enemy, SSkill* pSkill);
	void DoAttack(CRegionCreature *enemy,long seed);
	void RealAttack(CRegionCreature *enemy,int phyDmg,int eleDmg[MAX_ELEMENT_ATTR],long seed,float ScopeoDinaryInjuryR = 1);
	void RealAttackcalculating(CRegionCreature *enemy,int phyDmg,int eleDmg[MAX_ELEMENT_ATTR],int ret,long seed);

	// void DoAttack(SAttackParam &param);
    // void ApplyDamage(short& sDamage, float fAtt,CRegionCreature* pTarget);
    bool CanUseSkill(short sSkillID);
	void DeleteSkill(unsigned short wID);
	void UseSkill(unsigned short wID);  
    bool UseSkill(SAttackParam& param);
    bool UseSkill(SSkill* pSkill, SAttackParam& param);  // 使用技能，内部会调用CanUseSkill检查是否可以使用，如果不可使用则返回false
    long RecvDamage(CRegionCreature *enemy,int &phyDmg,int eleDmg[MAX_ELEMENT_ATTR],int AttackType = 0);
	//Tianh      isSkill---是否是技能攻击
	void ApplyDamage(CRegionCreature *enemy,int &phyDmg,int eleDmg[MAX_ELEMENT_ATTR],bool isSkill,SSkill* pSkillAttr = NULL);
	void RefreshEquipWear();

	bool CheckAttackSpeed(long interval,bool test); //技能检查
	bool CheckCommonAttackSpeed(long interval,bool test);//普通攻击检查
	bool CheckUseExpItem();
	long Die(CRegionCreature *killer);
	void MeDie(CRegionCreature *killer);
	long GetAttackBusy();
    void AddInvincibility();            // 增加无敌状态(黄名和红名不能无敌)，Add和Remove的调用次数要相同
    void RemoveInvincibility();         // 减少无敌状态(黄名和红名不能无敌)，Add和Remove的调用次数要相同
	CSvSkillCreature    m_Skill;
	bool HaveEffectByID(unsigned short wID);
	bool HaveEffectByClass(unsigned char cID);

	/* move function */
	int  CheckCanMove(int page, int gridI, int gridJ,SItemID item);
	bool SetPosition(long x,long y,long z);
    bool ForceSetPosition(long x,long y,long z,bool bBroadcast = false);
	bool TestSetPosition(long x,long y,long z);
	bool CanMove();
    int m_iCurrentBlockX, m_iCurrentBlockY;
    std::list<RtsSceneBlockRtb::SArea*> m_listArea;
    void OnPositionChanged(float fX, float fY);
    void OnRemoveCreature();
    void OnBlockChanged(RtsSceneBlockRtb* pNewBlock, RtsSceneBlockRtb* pOldBlock);
    void OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock); // 当进入某个区域
    void OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock);  // 当离开某个区域
    void UpdateAreaPKAttr();
    char GetPKState();

	EPkMode		m_ePkMode;

	//4.7 LeiJun
	bool AtAreaByName(const char *sAreaName);

    /* pk */
    EPKAttr     m_eBlockPosPKAttr;      // 当前场景块的PK属性
    EPKAttr     m_eCurPosPKAttr;        // 当前位置的PK属性
    // int         m_iPKStateChangeTime;   // 改变名字状态(白黄红名)的剩余时间(ms), 就是说剩下多少时间会改变状态

    /* */
	RtsPathPath *m_pathNode;
	CRegionAI *m_ai;

	bool SetModel(short Id);
	bool AddWeapon(short Id);
	long Distance(long *pos);
	SCreModel *m_model;

	bool m_bStatic;
	SMonsterLair *m_lair;
	SUserActor *m_actorInfo;

	/* AI function */
	void RecvMsg(const char *msg,const char *param);
	void ActiveAI();
	void DeleteAI();
	void SetAI(const char *ai);

	// r2c系统公告接口    Tianh
	void Systembroadcast(const char *Playername,const char *Name,const char *BossName,const char *SceneName,long type,const char *ItemName);
	//r2c系统公告接口
	void ChatNewS(const char *fromName,const char *Name,const char *BossName,const char *SceneName,long type,const char *ItemName);

	//r2c聊天接口
	void Chat(char channel,const char *msg,const int Range=DEFAULT_CHAT_RANGE, char faction=FACTION_UNKNOWN);
	void Chat(char channel,const char *msg,CRegionCreature *receive_cre,ULONG from_id,const char * from_name);//单对单聊天
	void Chat(char channel,const char *msg,CRegionCreature *receive_cre,CRegionCreature *from_cre=NULL,bool self = false);

    void SendSystemMessage(const char* szSysMsg, CRegionCreature *pFromCre=NULL);
	void SendBattleInfoMessage(const char* szSysMsg, CRegionCreature *pFromCre= NULL);
	
	//Tian 10.04.18 解析聊天中的超连接
	bool  ResolvingLinkChat(const char* msg);


	//void LineMoveto(int _lineGroup,int _fRebirthX,int _fRebirthY); //分线传送
	/*std::list<SItemID>    m_listChatItemID;*/

	/*void GetItemInfo(long PlayerID,long ID);*/

	//c2r聊天接口
	void PublicChat(const char *msg);// world chat
	void PartyChat(const char *msg);//同盟聊天
	void TeamChat(const char *msg);//队伍聊天
	void PrivateChat(const char *target,const char *msg);
	void GmChat(const char *msg, const char *title="");
	void CountryChat(const char *msg);//国家聊天
	void CommonChat(char channel, char const* msg, char const* target = "", char const* title = "");

	DWORD m_WorldChatTime;
	DWORD m_UseItemEggTime;

	long m_moveSpeed;
	long m_attackRange;

	//5.1 LeiJun NPC名字颜色以及颜色所属客户端
	DWORD m_NameColor;		//颜色格式：0xAARRGGBB
	//此颜色名字显示在某个客户端(用户Creature objectID)，其它客户端显示白色，如为-1，则显示于全部客户端
	std::list<ULONG> m_vResideObjectID;
	void ClearResideForNameColor(void);
	void DeleteResideForNameColor(ULONG lObjectID);
	void AddResideForNameColor(ULONG lObjectID);
	void SetColorToNPCName(DWORD dwColor = -1);
	void UpdataNPCNameColorToClient(void);
	void UpdataNPCNameColorToClient(ULONG lObjectID);

	void UpdataAnimationToClient(const char *sAnimName);

	/* npc */
	//char  m_skillAttackRate[NPC_MAX_SKILL];
	CM_MEMDEF(m_skillAttackRate, NPC_MAX_SKILL)
	DWORD m_enemy;
	short m_npcId;
	SNpc *m_npcInfo;
	CRegionSpawn*	m_spawn;
	SKillReward		m_killReward;
	CBag*			m_pSellBag;			                // npc出售的可以无限买的道具
    std::vector<SNpcSellRareItem> m_NpcSellRareItem;    // npc出售的过一定时间才刷新的道具
    int FindInSellRareItem(DWORD itemId);             // 在npc出售列表中找一个道具
    bool StartRareItemSell();
    bool StopRareItemSell();

	CombatRule m_combatRule;
	short m_patrolRange;
	short m_patrolRate;
	short m_viewRange;
	short m_traceRange;
	short m_assistRange;
	int	  m_faction;
	char  m_unitedPoint;
	char  m_loyalPoint;
	char  m_bravePoint;
	short m_angryPointLevel1;
	short m_angryPointLevel2;
	DWORD m_ai_level;	
	long OriginalLevel;
	long OriginalCulLevel;
	//DWORD m_proTarget; //保护目标ID //此ID为m_npcInfo->ID

	/* user */
	void Quit();
	CServerTeam*	m_pTeam;	// 队伍
	CServerMail*	m_pMail;	// 邮件
	CServerFriend*  m_pFriend;	// 好友列表
	
	/* item function */
	bool UseItem(DWORD Id);						// 使用道具
	bool UseItem(DWORD typeId, EItemColor color);
	bool UseItem(int page, int gridI, int gridJ, CItemContainerBase::EEquipParts part = CItemContainerBase::EQUIP_DEFAULT_PART, bool bBagItem = true);
	void AddPoint(short sPoint);				//添加点数
	bool AddItem(SItemID item);					// 根据物品类型添加道具
	bool AddItem(DWORD typeId,EItemColor color);
	int  GetItemNum(DWORD typeId, EItemColor color = ItemColor_All);
	int  GetPetNum(DWORD petId);
	bool RemovePetFromBag(DWORD petId, int num = 1);
	bool AddPetByItem(SItemID item);
	bool RemoveItem(DWORD typeId,int num, EItemColor color = ItemColor_All); // 删除一定数量一定类型的道具（针对数量，而不是格子）
	bool RemoveItem(DWORD Id, char bMissionBag = 0, int reason = 0); // 删除某一个id的道具（针对道具格子）
	int GetDungeonMissionItemNum(DWORD typeId, int difficulty);
	bool AddDungeonMissionItem(DWORD typeId, int difficulty);
	bool RemoveDungeonMissionItem(DWORD typeId, int num, int difficulty);
	SItemID CheckEquipment(DWORD typeId, EItemColor color, CItemContainerBase::EEquipParts* part = 0);
	SItemID CheckEquipment(DWORD Id, CItemContainerBase::EEquipParts* part = 0);

	void ClientDoCommand(char command,long finished,bool sendself = false);

    void QueryAccountPoing();

	bool StopPShop();
	bool StopPlayerTrade();

	DWORD m_dwPetItemOID;		// 如果是宠物，此值为道具的ID
	DWORD m_dwPetSave;
    
	CRegionItemContainer *m_pItemContainer;
	CTrade				*m_pTrade;	
	CPrivateShop		*m_pPShop;				// 摆摊商店
	char				 m_bPShop;				// 是否摆摊
	CBag				*m_pBank;
	CBag				*m_pBankPet;
    DWORD               m_dwLastPickItemTime;


	bool                m_bRefuseTeam ;       //设置是否拒绝组队
	bool                m_bRefusetrade;       //设置是否拒绝交易
	bool                m_bRefuseMail;        //设置是否拒绝邮件
	bool                m_bRefuseDuel;        //设置是否拒绝决斗
	bool                m_bRefuseFriend;      //设置是否拒绝好友
	bool                m_bRefuseMonster;     //设置是否拒绝师徒

 	/* 诸侯国 */
	DWORD m_unionID;							// 所属诸侯国ID
	string m_unionName;							// 所属诸侯国名字
	string m_unionNickName;						// 称号
	string m_unionIcon;							// 诸侯国logo文件
	bool IsUnionCaption();						// 是不是诸侯国的诸侯

	//城战相关
	string	m_feod_name;
	BYTE	m_is_attacker;

	int		m_hpRemain;
	int		m_mpRemain;
	int		m_EnpRemain;
	int		m_hpRecover;
	int		m_mpRecover;
	int		m_EnpRecover;
	
	int     m_SkillhpRecover;//技能每秒恢复血量
	int     m_SkillmpRecover;//技能每秒恢复蓝量

	float	m_hpAutoRecover;
	float	m_mpAutoRecover;
	float	m_hpAutoRecoverRate;
	float	m_mpAutoRecoverRate;
	
	DWORD m_CcombatBusy; //普攻
	DWORD m_combatBusy;
	DWORD m_usePetBusy;

	SItemID m_RifineTrump;
	bool	m_bIsRining;
	bool IsRifining(){ return m_bIsRining; }
	DWORD m_dwLastModelID;

	std::vector<SItemID>	m_vPkGetItemList;
	DWORD					m_dwPkGetItemTime;
	void SendPkGetItemList(std::vector<SItemID> itemList);

	DWORD m_dwProtectTime;
	bool IsInProtect();
	void SetProtectTime(DWORD millseconds, bool add = false);

	//CRegionCreature* m_pHuiYuanHoptoad;//tim.yang   会员蟾蜍精
	//int m_HuiYuanHoptoadCount;//会员蟾蜍精的数量，一次交易只允许一只出现
	//void RemoveHuiYuanHoptoad();//移除会员蟾蜍精
	ULONG m_curHuiYuanHoptoadOid;//当前召唤蟾蜍精的ObjectID;
	bool IsHuiYuanTimeOver(string huiyuantime);//根据会员时间判断是否添加会员道具
	DWORD  m_huiyuanTime;
	//bool m_isHuiYuan;
	bool IsHuiYuan();
	void CreateDropItem(const DWORD itemId,EItemDropStyle dropStyle);//创建掉落道具，属性不全，待添加。。。
	void CalcExploit(CRegionCreature* Killer);
	//神灯随机生成0，1，2三种事件
	void GodLightCreateMonster(CRegionCreature *target);//0
	void GodLightDropItem();//1
	void GodLightCreateLight();//2
	CRegionCreature* m_GodLightMonster[GODLIGHT_MONSTER_COUNT];
	byte m_monsterCount;//创建怪物的数量  最多127只 ，初始化是0只
	byte m_IsAddMonster;//判断玩家是否使用神灯变成的monster
	bool m_bIsDead;//判断玩家是否被杀死
	bool m_bIsOutOfLine;//判断玩家是否使用神灯召唤出来怪物后下线
	long m_FBID;     // 副本唯一ID
	bool m_bIsFBReborn;
	bool GodLightCreateMonster_AddMonster(CRegionCreature *target,byte monsterCount);
	void GodLightCreateMonster_DelAllMonster();

	//tim.yang  日常任务
	void JudgeDayTimeTaskIsAnotherDay();

	//tim.yang  检查传送蟾蜍精是否合法  true--合法传送   false--不合法不传送
	bool isteleport(int npcId);

	CMonCastle *m_mc;//tim.yang  MC
	bool m_isAddMCTask;
	void FinishMCTask();
	void RemoveMCTask();

	int m_iKillMonsterNum;			//save role killmonster number ,sever for rige
	int m_iRigelevel;				//save current rige level
	void SetRageLevel(int lev) {m_iRigelevel = lev;}

	//tim.yang  写使用道具的LOG
	void WriteUseitemLog(int index);
	//end

	//ac.ma			for防沉迷
	unsigned long m_playerLastLogoutTime;		//记录玩家下线时间				
	unsigned long m_playerNowLoginTime;			//记录玩家本次上线时间	
	int m_iIncomeState;					        //收益状态 100 or 50 or 0
	unsigned long m_lTotalOnline;				//Tianh 在线时间累计
	unsigned long m_lTotalOffline;				//下线时间累计
	unsigned long m_ltempOnline;
	bool m_bforGetTime;
	unsigned long m_lLastTotalOnline;
	void SetIncomeState(long lplayerNowLoginTime);

	// task system	
	//---------chg start by tony 06.05.09-------------------------------------------//
#if 0
@	bool CanFinishTask(STaskDesc *task,int idx);
@	long CheckTaskFinishState(STaskDesc *task);
@	bool CanAcceptTask(int TaskId);
@	bool CanAcceptTask(STaskDesc *task,bool showErr);
@	long CheckTaskAcceptState(STaskDesc *task);
@	bool TaskProcess(STaskDesc *task,const char *str,char **link);
@	bool AcceptTask(int TaskId);
@	bool AcceptTask(STaskDesc *task);
@	bool FinishTask(STaskDesc *task,int idx);
@	bool IsDoingTask(STaskDesc *task);
#else
	bool CanAcceptTask(int TaskId, CRegionCreature* npc=0);
	bool CanAcceptTask(STaskDesc *task,bool showErr, CRegionCreature* npc=0);
	bool CanFinishTask(STaskDesc *task,int idx, CRegionCreature* npc=0);
	long CheckTaskFinishState(STaskDesc *task, CRegionCreature* npc=0, bool bFinish = true, long nIndex = 0);
	long CheckTaskAcceptState(STaskDesc *task, CRegionCreature* npc=0);
	bool TaskProcess(STaskDesc *task,const char *str,char **link, CRegionCreature* npc=0);
	bool AcceptTask(int TaskId, CRegionCreature* npc=0);
	bool AcceptTask(STaskDesc *task, CRegionCreature* npc=0);
	bool FinishTask(STaskDesc *task,int idx, CRegionCreature* npc=0);
	bool IsDoingTask(STaskDesc *task, CRegionCreature* npc=0);
	void OnBuyTaskItem(int taskType); //- taskType: 任务类型 0-跑环
#endif
	//---------chg end  by tony 06.05.09--------------------------------------------//
	////---------add start by tony 06.05.09-------------------------//
	////-reason: 新添家跑环任务, 由circleTask!=0确定
	////-circleTask==1: 寻物
	////-circleTask==2: 找人
	////-circleTask==3: 战斗
	////-以下仅对跑环任务有效
	//int m_currCircle;						// 当前环数, 50环为1轮结束
	//int m_currMonsterNum;					// 当前已杀死怪物数量--仅对战斗任务有效
	//int m_finditemID;						// 寻找道具ID--仅对寻物任务有效
	//int m_findnpcID;						// 寻找npcID--仅对找人任务有效
	//int m_srcNpcID;							// 授予任务npcID
	//int m_killmonsterID;					// 杀死怪物ID--仅对战斗任务有效	

	int m_tmpcurrCircle;						// 当前环数, 50环为1轮结束
	int m_tmpcurrMonsterNum;					// 当前已杀死怪物数量--仅对战斗任务有效
	int m_tmpfinditemID;						// 寻找道具ID--仅对寻物任务有效
	int m_tmpfindnpcID;						// 寻找npcID--仅对找人任务有效
	int m_tmpsrcNpcID;							// 授予任务npcID
	int m_tmpkillmonsterID;					// 杀死怪物ID--仅对战斗任务有效	
	////---------add end   by tony 06.05.09-------------------------//

	//---------add start by tony 06.05.16---------------//
	//-换装备1秒内不能攻击
	DWORD m_lastEquipItemTime;
	//---------add end   by tony 06.05.16---------------//
	
	bool CancelTask(DWORD TaskId,bool busy);	
	void RemoveTaskItem(STaskDesc *task);

	void NetSend_r2c_task_event(char result);
	ETaskOverHead GetTaskOverHead(DWORD npcOID);
	
	bool ProcessTaskKeyList(vector<STaskKey> &table,int op,vector<long> *param, STaskDesc *pDesc = NULL);
	bool ProcessTaskKey(STaskKey &key,int op,vector<long> *param, STaskDesc *pDesc = NULL);
	
	CTaskInfo m_task;
	bool m_valid;

	// fb task system
	bool FbCanFinishTask(int taskId,int sreward);
	bool FbFinishTask(SFbTask *task,SFbTaskInfo *info,int sreward);
	bool FbFinishTask(int taskId,int sreward);
	void FbCheckTask(bool notifyClient);
	void FbCheckAllTask(bool notifyClient);
	
	bool AddReward(int rewardId);
	
	string m_shortcut;
	string m_configData;

	STargetList m_targets;
	STargetList m_enemyList;
	bool m_lockTarget;

	
	std::list<CRegionCreature*> m_protectorList;
	void				AddProtector();
	void				SetProtector(CRegionCreature* pro);

	//--------add end by Tianh--10.05.28---------------//
	//计算创建NPC出生点
	bool				CreateAllUnderlingNpc(short npc_id,char Type,int num = 5,long* pos = NULL);
	//创建召唤NPC
	bool				CreateUnderlingNpc(short npc_id, float pos[3]);
	//创建BOSS召唤NPC
	bool				CreateUnderlingNpcByBoss(short npc_id, float pos[3]);
	//删除创建NPC
	void                DeleteAddUnderlingNpc();
	void                DeleteCallNpc(int sV);
	//存放召唤NPC
	std::vector<CRegionCreature*> CallNpc;

	void                SendMagCallNpc();

	bool IsCallNpc() {/*Tianh 是否是召唤的NPC*/
		if (!m_master)
		{
			return false;
		}
		bool IsHave = false;
		std::vector<CRegionCreature*>::iterator it = m_master->CallNpc.begin();
		for (;it != m_master->CallNpc.end();it++)
		{
			if ((*it) == this)
			{
				IsHave = true;
			}
		}
		return (m_master->IsUser() && IsHave); 
	}
	//--------add end by Tianh--10.05.28---------------//

	//--------add end   by tony 05.08.09 ai modify-----------------//

	CRegionCreature*	GetCurrentTarget();
	void				AddEnemy(CRegionCreature* enemy, int damage);
	void				AddTarget(CRegionCreature* target, int hate, int damage);
	void				SetTarget(CRegionCreature* target);
	// STargetList::iterator	FindTargetByCreature(CRegionCreature* cre);

	void				RemoveAllTargets();
	void				RemoveTargetByCreature(CRegionCreature* cre); 

	bool				IsOutOfRange(const long* pos, int range);
	void                RecomputeAttr();
	void				OnLevelUp();
	void				OnLevelDown();
	bool				SetLev(int lev);

	// magic weapon
	void                MagicWeaponBeginAddMp();
	bool                MagicWeaponEndAddMp();

	void                MagicWeaponBeginAddEle(int type);
	bool                MagicWeaponEndAddEle();

	void                TickTask(unsigned long delta);
	// creature state
	list<CCreatureState*> m_stateList;
	void                TickState(unsigned long delta);
	bool                AddState(CCreatureState* state,SCreStateParam *param);
	CCreatureState*     FindStateByType(const ECreatureStateType type);
	bool				FindStateCannotMove();

	//4.13 LeiJun
	void				AddBattleState(CRegionCreature *pEnemy);
	bool				OnlyBattleState(void);
	bool				HaveBattleState(void);
	void				BeginBattleState(void);
	void				EndBattleState(void);

	vector<STaskDesc*> m_acceptTask;
	vector<STaskDesc*> m_finishTask;
	long m_func;

	void ToCreatureInfo(SCreatureInfo* pInfo);
	bool ShouldKickCheat() { 
		return (m_cheatKick!=0 && (rtGetMilliseconds()>m_cheatKick));
	}
	DWORD m_cheatKick;
	bool  m_cheatDummy;

	DWORD				m_dwState;	// ECreatureState
	void				NetSend_State();
	bool				State_IsVisible() { return (m_dwState&CreState_Visible)?true:false; }
	bool				State_CanMove() { return (m_dwState&CreState_CanMove)?true:false; }
	bool				State_CanSpeak() { return (m_dwState&CreState_CanSpeak)?true:false; }
	bool				State_CanPShop() { return (m_dwState&CreState_CanPShop)?true:false; }

	void				State_SetVisible(bool bVisible);
	void				State_SetMovable(bool bCanMove);
	void				State_SetSpeakable(bool bSpeakable);
	void				State_SetPShopable(bool bPShopable);

	void				ReComputePetBagSize();

	// 统计信息
#define	PLAYER_MONITOR_CYCLE		1800 // seconds (30*60)
	int					m_nGetMoneyFromMonster;
	int					m_nGetMoneyFromTask;
	int					m_nMonsterCount;
	int					m_nGetItemCount;
	int					m_nGetExp;
    int                 m_MonitorLastTime;

    int                 m_iUserMoveFailedCnt;   // 检查到非法移动的次数
    long                m_lLastCheckProcessTime;// 最后一次检查进程的时间

	long                m_lAbsClientTime;
    long                m_lLastClientTime;
    long                m_lPosCmdCnt;
    long                m_lFirstPosCmdTime;
    char                m_crcCheckFlag;         // =0的时候会进行CRC检查，并且设置下一次检测间隔
    char                m_wgChecked;
    long                m_wgCheckedTime;        // 检查到外挂的时间
    char                m_wgMaybeChecked;
    long                m_wgMaybeCheckedTime;   // 检查到外挂的时间
    time_t              m_tNextCheckProcess;    // 下一次检查进程的时间, 0不要检查
    void WgChecked(int iBaseTime=120, int iRandTime=600, char *szCheckInfo = ""); // 检查到使用外挂
    void PossiblyUsingWG(int iCnt=1, int iBaseTime=60, int iRandTime=200); // 检查到可能使用外挂
    void SendWgCheckedToClient();

	// new pet function
	int m_petAbsorbMp;
	int m_petAbsorbHp;

	// 催钱任务相关数据
	long  m_mtBusy;			 // 放弃催粮任务后的延迟时间
	short m_mtFinish[3];	 // 当天已完成的催粮任务数，分三个难度
	short m_mtClearDay;		 // 最后一次清除任务次数的日期
	bool  IsDoingMoneyTask();

	// 文官任务
	short m_wenTaskFinish;
	short m_wenTaskAccept;
	short m_wenClearDay;

	// 门贡任务
	short m_gongTaskFinish;
	short m_gongTaskAccept;
	short m_gongClearDay;

	short m_UnionTask;//帮派任务
	short m_UnionClearDay;

	short m_MouTaskFinish;//谋生任务
	short m_MouTaskAccept;
	short m_MouClearDay;

	bool SaveExtraData(RtMemoryBlock &block);
	bool LoadExtraData(RtMemoryBlock &block);

	bool Save(SUserData &data);
	bool Load(SUserData &data);

	float GetExpRate();
	long  m_cardTime;
	long  m_usingCard;
	long  m_cardLastTime; // 最后一次增加打卡时间的日期
	long  m_LastDieTime;	// 最后一次死亡的时间
	
	void StartUseCard();
	void EndUseCard();
	void ShowCardInfo();
	void ComputeCardTime(long lastTime,bool newBie);

    // 生活技能的配方相关
    void UpdateMatchToClient();
    void WriteMatchToNet(CG_CmdPacket *cmd);
    void AddMatch(short sMatchID);
    void DelMatch(short sMatchID);
    std::list<short>    m_listMatchID;

private:
    bool m_bNpcSellRareItemStart; // 是否启动刷新道具
    bool m_bScapegoatRemovedDuringDead; // 死亡过程中是否已经删除掉替身娃娃了

private:
	//  [8/20/2009 tim.yang] 双倍经验瓶使用限制
	DWORD m_lastuseCardMinuteBottleTime;//记录上次使用双倍经验瓶的时间
	int m_useCardMinuteBottleNum;//记录双倍经验瓶在一天内使用的次数
	bool CanUseCardMinBottle();
	void AddUseCardMinBottleNum();
	void InitCardMinBottleParam();
	bool IsUseCardMinBottleOverday();
	bool isSameDay(long nowtime, long lastTime);
public:
	CPlayerLogic* GetPlayerLogic();
private:
	CPlayerLogic* m_pPlayerLogic;
};

// -------------------------------------------------------------------------
// User infomation 
// -------------------------------------------------------------------------

class CRegionUser
{
public:
	CRegionUser() 
	{ 
		CM_MEMPROTECTOR(m_szPointMsg, 100)
		m_dummy				= NULL;
		m_session			= NULL;
		m_userId			= 0;
		m_regionId			= 0;
		m_updateTimeStamp	= rtGetMilliseconds() / 1000;
        m_active            = true;
		m_bSaveData         = true;
		m_bLogout           = true;
        m_lastKeepOnlineTime= 0;
		m_bankPass			= "";
		m_dwPrenSysLockTick	= 0;
        m_switchRegionSeed = 0;
        m_bSwitchRegionState = false;

        m_lPoint            = 0;
        m_tPointExpire      = 0;
        m_cVIP              = 0;

		m_ArSendTick		= 0;
		m_ArErrorCount		= 0;
		m_ArStatus			= 0;
		m_ArResendCount		= 0;
		m_ArSendCharacters	= "";

        m_lClientLastCheckedTime = 0;
        m_iJhWgCheckedCnt = 0;
		for(int ii=0; ii<SCRIPT_FUNC_STATE_COUNT; ii++)
			m_lFunctionState[ii] = 0;
	}

	virtual ~CRegionUser()
	{
		CM_MEMUNPROTECTOR(m_szPointMsg)
		SaveAllActivityInfo(true);
	}

	bool LinkDummy(CRegionCreature *cre) 
	{	
		if(cre->m_userInfo) return false;
		m_dummy = cre;
		cre->m_userInfo = this;
		// notify client,initialize control and ui
		// ...
		return true;
	}
    int Prededuct(short sPoint,long event,long lAuctionId = 0); // 预扣点数, 用来做用点数向NPC买道具, 不能为负数,event是事件 1——竞标
	void Deduct(short sPoint,long orderid,DeductType type,void* v); // 扣点数, 用来做用点数向NPC买道具, 不能为负数
    void OnPrededuct(long lDeductOrderID, char cResult, short sPoint);
    void OnDeduct(long lDeductOrderID, char cResult, short sPoint);
    // sLockFlag 0为解锁, 1为锁定
    // lLockSecond为锁定的秒数
    //        1800 为  30 分钟
    //       10800 为   3 小时
    //      259200 为   3 天
    //      432000 为   5 天
    //      604800 为   7 天
    //      864000 为  10 天
    //     2592000 为  30 天
    //    25920000 为 300 天 (大约1年)
    //   259200000 为 3千 天 (大约10年)
    //   315360000 为   十年 (十年可以认为是永远)
    void LockAccount(short sLockFlag, long lLockSecond);

    bool CheckBankPass(const std::string& strPass);

    // eType 可以为
	//      CreState_CanSpeak
	//      CreState_CanPShop
	//      CreState_CanMove
    // lTimeSecond 单位为秒
    bool ForbidUser(bool bBan, ECreatureState eType, long lTimeSecond);

	//5.26 LeiJun 活动相关用户数据    Start
	typedef	std::map<int, bool>	ACTJOINS;
	typedef	std::map<int, UserActivityInfo*>	ACTINFOS;
	ACTJOINS			m_ActJoins;
	ACTINFOS			m_ActInfos;
	void				UpdataActivityJoin(const char *sJoins)
	{
		if(!sJoins)
			return;
		const char *seq = ".";
		char *pStr = strtok((char*)sJoins, seq);
		while(pStr && pStr[0] != 0)
		{
			int iIndex = atoi(pStr);
			pStr = strtok(NULL, seq);
			if(pStr && pStr[0] != 0)
			{
				m_ActJoins[iIndex] = (atoi(pStr) != 0) ? true : false;
				pStr = strtok(NULL, seq);
			}
			else break;
		}
	}
	void				UpdataActivityJoin(bool bJoin[], int iCount)
	{
		for(int iType=0; iType<iCount; iType++)
			m_ActJoins[(ACTIVITY_TYPE)iType] = bJoin[iType];
	}
	void				SetActivityJoin(int iID, bool bJoin)
	{
		ACTJOINS::iterator itJoin = m_ActJoins.find(iID);
		if(itJoin != m_ActJoins.end())
			itJoin->second = bJoin;
	}
	bool				GetActivityJoin(int iID)
	{
		ACTJOINS::iterator itJoin = m_ActJoins.find(iID);
		if(itJoin != m_ActJoins.end())
			return itJoin->second;
		return false;
	}
	bool				SetActivityInfo(int iID, UserActivityInfo *pInfo)
	{
		if(iID <= 0 || !pInfo)
			return false;
		if(m_ActJoins.find(iID) == m_ActJoins.end())
			return false;
		ACTINFOS::iterator itInfo = m_ActInfos.find(iID);
		if(itInfo != m_ActInfos.end())
		{
			itInfo->second->UpdataInfo(pInfo);
			return false;
		}
		else
		{
			m_ActInfos[iID] = pInfo;
			return true;
		}
		return false;
	}
	UserActivityInfo*	GetActivityInfo(int iID)
	{
		ACTINFOS::iterator itInfo = m_ActInfos.find(iID);
		if(itInfo != m_ActInfos.end())
			return itInfo->second;
		return NULL;
	}
	void				SaveAllActivityInfo(bool bDelete = false);
	//5.26 LeiJun 活动相关用户数据    End

	//LeiJun 脚本功能状态值
	long m_lFunctionState[SCRIPT_FUNC_STATE_COUNT];

	char				m_lanCode;
	ULONG				m_userInfo;
	ULONG				m_userId;
	ULONG				m_regionId;
	ULONG				m_connectTime;
	ULONG				m_netDeadTime;
	time_t              m_lastKeepOnlineTime; // 最后一次发送信息给Gameworld表示说玩家在线的信息
	std::string			m_username;
	std::string			m_accountName;
	CRegionCreature    *m_dummy;
	long                m_switchRegionSeed;
	int					m_updateTimeStamp;
    bool                m_active;               // 是否激活，默认激活，在切换服务器或者未创建的时候不是激活的，不激活状态不接受命令
    bool                m_bSwitchRegionState;   // 是否处于切换Region状态
    long                privileges;         // 权限 (enum EUserPrivileges [gw_protocol.h])
	bool                m_bSaveData;
	bool                m_bLogout;
	std::string			m_userIpAddress;
	std::string			m_bankPass;
	DWORD				m_dwPrenSysLockTick;	// 师徒操作的lock时间
	// long                m_onlineTime;

    // 点数信息
    long                m_lPoint, m_tPointExpire; // 点数和点数过期时间
    char                m_cVIP;                   // VIP信息，0不是VIP
    //char                m_szPointMsg[100];
	CM_MEMDEF(m_szPointMsg, 100)

	// 防外挂变量
	long				m_ArStatus;
	long				m_ArSendTick;
	short				m_ArResendCount;
	short				m_ArErrorCount;
    std::string         m_ArSendCharacters;

    unsigned long       m_lClientLastCheckedTime;
    int                 m_iJhWgCheckedCnt;
    long                m_lClientLUASeed;
    bool wgCheckServer(unsigned char* buf, int& iDataSize, int iBufSize);

	string              m_luaPrefix;
	string              m_luaEndfix;

	void RecvCmd(CG_CmdPacket *cmd);
	void KeepAlive(long now)	{if (m_session) m_session->KeepAlive(now);}
	void SetSession(CG_TCPSession* p) {m_session = p;}
	CG_TCPSession* GetSession()	{ return m_session; }
	void ClearSession()	{ DEL_ONE(m_session); m_session = 0;}
	bool Session()		{ return m_session!=0;}
private:
	CG_TCPSession	   *m_session;
};

// -------------------------------------------------------------------------
// Task npc
// -------------------------------------------------------------------------

class CNpcTaskLogicExt;//PZH
class CRegionNpcTask : public CRegionCreature
{
public:
	friend class CNpcTaskLogicExt;//PZH
	CRegionNpcTask();
	virtual ~CRegionNpcTask();

	/* CRegionCreature Interface */
	virtual bool Init(const char *param) { return true; } 
	virtual void Interactive(CRegionCreature *target,CG_CmdPacket *cmd);
	void DisposeTaskInterface(CRegionCreature *pTarget, char *pInfo);

	bool GetNpcFunction(long func);
	void SetNpcFunction(long func);
	//-----------add start by tony 06.05.09-------------------------------------//
	//-reason:新添加跑环任务,与以前任务系统有很大差别,所以在npc上加以区别
	bool IsCircleTaskNpc();
	//-----------add end   by tony 06.05.09-------------------------------------//

	bool ProcessAcceptTaskList(CRegionCreature *target);
	bool ProcessFinishTaskList(CRegionCreature *target);
	void EntTask(CRegionCreature *target);
	bool ProcessTask(CRegionCreature *target,char *str);
	void ShowHtmlDialog(CRegionCreature *target,const char *link);

	//PZH
	CNpcTaskLogicExt* GetCNpcTaskLogicExt();
private:
	CNpcTaskLogicExt* m_pCNpcTaskLogicExt;
	//
};
#endif
