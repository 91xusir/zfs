#ifndef __REGION_AI_H__
#define __REGION_AI_H__


//-------------------------------------------------------
//- 无     无    可选    运动模式(互斥)
//-0000   0000   0000      0000
//-------------------------------------------------------
#define AI_NONE		0x0000		//无任何功能
#define AI_NORMAL	0xff00		//无特殊功能
#define AI_FIXED	0x0001		//固定不动
#define AI_IDLE		0x0002		//随机运动
#define AI_PATROL	0x0004		//巡逻
#define AI_PROTECT	0x0010		//有守护功能
#define AI_CONJURE	0x0020		//有召唤功能

#define MOVETYPE(flag)	((flag) & 0x000f)
#define PROTECT(flag)	((flag) & AI_PROTECT)
#define CONJURE(flag)	((flag) & AI_CONJURE)
//-------------------------------------------------------

//-------------------------------------------------------
#define SUCCOR_MODE_REWARD	0
#define SUCCOR_MODE_HARM	1
//-------------------------------------------------------

//-------------------------------------------------------
#define SELF_REWARD_NUM		3
#define CONJURE_NUM			3
#define CONJURE_SKILL_ID	1504
//-------------------------------------------------------

inline bool _need_debug(CRegionCreature* pMonster)
{
	//CRegionCreature* pTarget = pMonster->GetCurrentTarget();
	//if (pTarget)
	//	if (pTarget->m_core.Name == "1dsfaq")
	//		return true;

	return false;
}

//-----------------------------------------------------------------------------
//召唤AI
//-----------------------------------------------------------------------------
class CRegionConjureAI : public CRegionObject
{
public:
	CRegionConjureAI();
	CRegionConjureAI(CRegionCreature* p_cre, DWORD skill_id, short npc_id);
	~CRegionConjureAI(){};

public:
	bool CanUseSkill();

	void UseSkill();

	int GetCreatureNum() { return (int)m_ObjectID.size(); }

	void AddCreatureID(DWORD o_id) { m_ObjectID.push_back(o_id); }

	void DelCreatureID(DWORD o_id)
	{
		std::vector<DWORD>::iterator it = m_ObjectID.begin();

		for(; it != m_ObjectID.end(); it++)
		{
			if((*it) == o_id)
			{
				m_ObjectID.erase(it);
			}
		}
	}

public:
	void SetSkillID(DWORD id)		{ m_SkillID = id;	}

	DWORD GetSkillID()				{ return m_SkillID; }

	short GetNpcID()				{ return m_NpcID; }

	void SetConjureNum(int num)		{ m_ConjuredNum = num; }

	void SetConjureNpcID(short id)	{ m_NpcID = id; }

	void SetNpcCount(short num)		{ m_Count = num; }

	void DecNum()					{ m_ConjuredNum--; }

	void IncNum()					{ m_ConjuredNum++; }

	void ResetNum()					{ m_ConjuredNum = CONJURE_NUM; }

private:
	CRegionCreature*	m_creature;			//技能所属主体
	DWORD				m_SkillID;			//召唤技能ID
	short				m_NpcID;			//召唤的生物类型ID
	short				m_Count;			//召唤出的生物数量
	std::vector<DWORD>	m_ObjectID;			//召唤出的生物ID
	int					m_ConjuredNum;		//召唤技能使用次数
};
//-----------------------------------------------------------------------------

class CRegionAI : public CRegionObject
{
public:
	enum Type
	{
		TYPE_MONSTER,	// 怪物
		TYPE_PET,		// 宠物
		TYPE_GUARDER,	// 守卫
		TYPE_HERO,		// 神仙
		TYPE_TOUR,		// 旅行者
		TYPE_CALLNPC    // 召唤的NPC
	};

	enum State
	{
		STATE_MOVE,
		STATE_COMBAT,
		STATE_FOLLOW,
		STATE_RETURN,
		STATE_RUNAWAY,
		STATE_SUCCOR,
		STATE_ANIMATION
	};

public:
	/* 
	 * 该标志决定ai的具体运行模式
	 */
	DWORD	m_dwLevelFlag;
private:
	const Type m_type;

protected:
	CRegionAI(Type type);
	
public:
	virtual bool OnBorn(CRegionCreature* creature);

	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	virtual bool OnUnderlingBeAttacked(CRegionCreature* underling, CRegionCreature* target, int hate);

	virtual bool OnLostLeader(CRegionCreature* leader);

	virtual bool OnUnderlingBorn(CRegionCreature* underling);

	virtual bool OnUnderlingDead(CRegionCreature* underling);

	virtual bool OnLostTarget(CRegionCreature* target);

	virtual bool OnAttack(CRegionCreature* target);

	virtual bool OnAssist(CRegionCreature* target);

	virtual bool OnFollow(CRegionCreature* target);

	virtual bool OnDead();

	virtual bool OnThink();

	virtual bool OnSuccor(CRegionCreature* target);		//保护目标

	virtual bool OnArrivePoint();

	virtual void SetStartNode(long* pos);

	virtual void SetGuard(CRegionCreature* pGuard);

	virtual void CancelGuard(DWORD iUserID);

	CRegionCreature* GetCreature() { return m_creature; }

	void TalkSuccor(void);
	void TalkNormal(void);
	void TalkOver(void);

	bool IsValid()
	{
		if (m_freezeTimeStamp == 0)
		{
			return m_creature != NULL;
		}
		else
		{
			if (m_freezeTimeStamp <= (int)rtGetMilliseconds())
			{
				m_freezeTimeStamp = 0;

				return m_creature != NULL;
			}

			return false;
		}
	}	

	void SetToAnimationState(std::string* psAnimation, short iDurTime);

	Type GetType() { return m_type; }

	void SetState(State state) { m_state = state; }

	State GetState() { return m_state; }

	void DoAttack();

	void DoTrace(CRegionCreature* creature);

	bool IsOutOfRange(const long* p1, const long* p2, int range);

	bool CanAttack(CRegionCreature* creature);

	bool UpdateMovePath(int x, int y);

	bool ApplyMove();	

	CRegionCreature* GetLeader();

	CRegionSpawn* GetUnderling();

	void OnCallPathNextSuccessed(int tileX, int tileY, float &fx, float &fy);

	void SetLvFlag(DWORD flag) { m_dwLevelFlag = flag; }

	DWORD GetLvFlag() { return m_dwLevelFlag; }

	void SetActive(bool bActive);
	bool IsActive() { return m_bActive; }

public:
	CRegionCreature* m_creature;
	int m_moveFur;
	State m_state;
	long m_targetPos[3];
	int m_freezeTimeStamp;
	int m_freezeDuration;
	int m_lastAttackTime;

	short m_AnimDurativeTime;	//原地动画状态持续时间
	string m_PlayAnimName;		//播放原地动画的索引

	long m_nextNodePos[3];		// 下一个要走的格子的位置
	long m_currNodePos[3];		// 已经走过的那个格子的位置
	bool m_bHaveNextNodePos;	// m_nextNodePos和m_currNodePos是否已经被赋值了
	CRegionConjureAI* m_pConjureAI;
	bool m_isBeAttacked;
	bool	m_bTrace;			//是在在追踪
	DWORD m_dwForceReturnTime;

	bool m_bActive;

	DWORD	m_tTalkTime;	//最后一次喊话的时间
	const DWORD	c_tTalkCycleTime;	//喊话周期时间
};

class CRegionMonsterAI : public CRegionAI
{
public:
	virtual bool OnBorn(CRegionCreature* creature);

	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	virtual bool OnThink();

	virtual bool OnAssist(CRegionCreature* target);		//群体性ai

	virtual bool OnCombat(CRegionCreature* target);

	virtual bool OnAttack(CRegionCreature* target);	

	virtual bool OnLostLeader(CRegionCreature* leader);

	virtual bool OnUnderlingBorn(CRegionCreature* underling);

	virtual bool OnUnderlingDead(CRegionCreature* underling);

	void SeekHelp();	//

	virtual bool OnSuccor(CRegionCreature* target);		//保护目标

	virtual void SuccorMode(CRegionCreature* target);

public:
	CRegionCreature* DoScanTarget();

	bool BeginReturn();

	void DoReturn();

	void BeginRunaway();

	void DoRunaway();

	virtual void DoIdle();

	void DoPatrol();

	void DoMove();

	void KeepDistance(CRegionCreature* target);

	void ComputeRandomFaceTo(int fix);

	void SelfSuccor();

	void CryForHelp();

	//util
	//保存当前位置（离开当前巡逻路线之前）
	void SaveCurrPos()
	{
		//未曾离开过巡逻路线，则保存
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				(void *)m_creature->m_pos, 
				sizeof(m_TargetNode));

			m_Left = true;
		}
	}

public:
	CRegionMonsterAI();
	~CRegionMonsterAI()
	{
		if(CONJURE(m_dwLevelFlag) == AI_CONJURE)
		{
			if(m_pConjureAI)
				DEL_ONE(m_pConjureAI);
		}
	}

//private:
	protected:
	long							m_orignalPos[3];	//起始位置
	long							m_TargetNode[3];	//目标节点
	bool							m_Arrived;			//是否到达目标节点
	bool							m_Left;				//是否已离开当前巡逻路线
	std::list<SPathNode>			m_PatrolPath;		//巡逻路径
	std::list<SPathNode>::iterator	m_IterNode;			//指向下一巡逻节点
	
	int m_patrolInertia;
	int m_patrolSpeed;
	long m_face[2];
	float m_moveLengthPerStep;	

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

	DWORD	m_proTargetID;
	int		m_SuccorMode;
	int		m_SelfRewardNum;	//对自己施放有益魔法次数
	int		m_ConjuredNum;		//召唤技能使用次数
};

class CRegionGuarderAI : public CRegionAI
{

public:
	CRegionGuarderAI();

	//public:
	//产生时
	virtual bool OnBorn(CRegionCreature* creature);

	//被攻击时
	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	//思考时
	virtual bool OnThink();

	//接到援助信号时
	virtual bool OnAssist(CRegionCreature* target);

	//战斗进行时
	virtual bool OnCombat(CRegionCreature* target);

	//攻击敌人时（与OnBeAttacked对应）
	virtual bool OnAttack(CRegionCreature* target);	

	//队长死亡时
	virtual bool OnLostLeader(CRegionCreature* leader);
	
public:
	//寻找目标（红名玩家 对立阵营玩家）
	//true:找到目标 false:未找到
	bool DoScanTarget();

	//返回
	bool BeginReturn();
	void DoReturn();

	//继续运动
	void DoMove();
	void DoPatrol();
	void DoIdle();

	//寻求帮助
	void SeekHelp(CRegionCreature* target);

	//util
	//保存当前位置（离开当前巡逻路线之前）
	void SaveCurrPos()
	{
		//未曾离开过巡逻路线，则保存
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				   (void *)m_creature->m_pos, 
				   sizeof(m_TargetNode));

			m_Left = true;
		}
	}

private:
	long							m_StartNode[3];		//起始位置
	char							m_StartDir;			//起始方向
	long							m_TargetNode[3];	//目标节点
	bool							m_Arrived;			//是否到达目标节点
	bool							m_Left;				//是否已离开当前巡逻路线
	std::list<SPathNode>			m_PatrolPath;		//巡逻路径
	std::list<SPathNode>::iterator	m_IterNode;			//指向下一巡逻节点

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

};

//辅助npcAI
/*
 *	辅助玩家战斗，护送等
 */
class CRegionAssistantAI : public CRegionAI
{

public:
	CRegionAssistantAI();
	CRegionAssistantAI(Type type);

	//public:
	//产生时
	virtual bool OnBorn(CRegionCreature* creature);

	//被攻击时
	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	//思考时
	virtual bool OnThink();

	//接到援助信号时
	virtual bool OnAssist(CRegionCreature* target);

	//战斗进行时
	virtual bool OnCombat(CRegionCreature* target);

	//攻击敌人时（与OnBeAttacked对应）
	virtual bool OnAttack(CRegionCreature* target);	

    virtual bool OnArrivePoint();

	virtual void SetStartNode(long* pos);
	
public:
	//寻找目标
	bool DoScanTarget();

	//返回
	bool BeginReturn();
	void DoReturn();

	//继续运动
	void DoMove();
	void DoPatrol();
	void DoIdle();

	//消除移动限制(使npc可以移动)
	void BanishMoveLimit()
	{
		SetLvFlag(AI_PATROL);
	};

	//寻求帮助
	void SeekHelp(CRegionCreature* target);

	//util
	//保存当前位置（离开当前巡逻路线之前）
	void SaveCurrPos()
	{
		//未曾离开过巡逻路线，则保存
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				(void *)m_creature->m_pos, 
				sizeof(m_TargetNode));

			m_Left = true;
		}
	}

private:
	long							m_StartNode[3];		//起始位置
	char							m_StartDir;			//起始方向
	long							m_TargetNode[3];	//目标节点
	bool							m_Arrived;			//是否到达目标节点
	bool							m_Left;				//是否已离开当前巡逻路线
	std::list<SPathNode>			m_PatrolPath;		//巡逻路径
	std::list<SPathNode>::iterator	m_IterNode;			//指向下一巡逻节点

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

	DWORD	m_tBorn;		//出生时间
	DWORD	m_tDuration;	//持续时间

};

class CRegionBossAI  : public CRegionMonsterAI
{
public:
	virtual bool OnDead();

	virtual bool OnAssist(CRegionCreature* target);

	virtual bool OnUnderlingBeAttacked(CRegionCreature* underling, CRegionCreature* target, int hate);

	virtual bool OnBeAttacked(CRegionCreature* target, int hate);
};

//召唤NPCAI
class CRegionCallNpc : public CRegionAI
{
public:
	CRegionCallNpc();
	~CRegionCallNpc();
public:
	enum AttackState
	{
		AS_ATTACK,	 // 攻击
		AS_SYNERGY,	 // 协同
		AS_PASSIVE,	 // 被动
		AS_DEF		 // 防御
	};

	virtual bool	 OnAttack(CRegionCreature* target);
	virtual bool	 OnFollow(CRegionCreature* target);
	virtual bool	 OnThink();
	virtual bool     OnBorn(CRegionCreature* creature);
public:
	void			 BeginFollow(CRegionCreature* creature);
	void			 DoFollow(CRegionCreature* creature);
	CRegionCreature* DoScanTarget();
public:
	AttackState		 m_eAS;
};
//tim.yang  神灯召唤兽AI
class CGodLightMonsterAI  : public CRegionMonsterAI
{
private:
	long							m_orignalPos[3];	//起始位置

public:
	//DWORD m_TargetObjID;

	ULONG				m_TargetDBID;

	virtual bool OnBorn(CRegionCreature* creature);

	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	virtual bool OnThink();

public:
	CGodLightMonsterAI():m_TargetDBID(0){}
	~CGodLightMonsterAI(){}
};
//end

struct SPathPoint
{
	long x;
	long y;
	bool operator == (SPathPoint spp)
	{
		return (x == spp.x)&&(y == spp.y);
	}
};

//tim.yang  MC  AI
class CMCAI : public CRegionMonsterAI
{
public:
	CMCAI();
	~CMCAI();
	virtual bool OnBorn(CRegionCreature* creature);
	virtual void DoIdle();
	virtual bool OnThink();
protected:
private:
	CMonCastle *m_mc;
	std::vector<SPathPoint> _ChangeStringToSPointVector(const std::string str);
	double _GetDisBetweenTwoPoint(SPathPoint sp1,SPathPoint sp2);
	std::vector<SPathPoint> _GetPathPoint(byte country);
	SPathPoint _GetNearestDis(long *orignalPos, std::vector<SPathPoint> pathPoint);
	bool _ReCalCurTargetPos(long *curTargetPos);
	std::vector<SPathPoint> m_pathPoint;//自动寻路的路点
	long m_curTargetPos[3];//每次移动的目标点，并不全是路点列表中的点
	std::vector<SPathPoint>::iterator m_pathPointNext;//下一个路点
	ULONG m_attackTargetObj;
};
//endl

class CRegionPetAI : public CRegionAI
{
public:

	enum AttackState
	{
		AS_ATTACK,	 // 攻击
		AS_SYNERGY,	 // 协同
		AS_PASSIVE,	 // 被动
		AS_DEF		 // 防御
	};

	virtual bool	 OnAttack(CRegionCreature* target);
	virtual bool	 OnFollow(CRegionCreature* target);
	virtual bool	 OnThink();
	virtual void	 DoAttack();
	virtual bool	 OnBorn(CRegionCreature* creature);

public:
	void			 BeginFollow(CRegionCreature* creature);
	void			 DoFollow(CRegionCreature* creature);
	CRegionCreature* DoScanTarget();

	bool			 SetDefSkill(DWORD defSkill = 0);
	list<DWORD>		 GetPassiveList();
	list<DWORD>		 GetSkillList();

	PetType			 GetPetType();
	void			 UserCtrlPet(char command,DWORD dwParam1 = 0,DWORD dwParam2 = 0);
	bool			 DoUseSkill(DWORD skillID,DWORD dwTarget = 0);
	bool			 FindPetSkill(DWORD skillID);

	bool			 PetLevUp();

public:
	CRegionPetAI();
	~CRegionPetAI();

	AttackState		 m_eAS;
	SSkill			*m_pDefSkill;
	SSkill			*m_pCurSkill;
	SItemID			m_pItemPet;
};

class CRegionHeroAI : public CRegionAI
{
public:
	virtual bool	OnFollow(CRegionCreature* target);

	virtual bool	OnThink();

public:
	void			BeginFollow(CRegionCreature* creature);

	void			DoFollow(CRegionCreature* creature);

public:
	CRegionHeroAI();
};

struct	GuardData
{
	DWORD miUserID;
	DWORD miOutsideCount;

	GuardData(void) : miUserID(0), miOutsideCount(0)
	{
	}
	void Clear(void)
	{
		miUserID = 0;
		miOutsideCount = 0;
	}
	void SetUser(DWORD iID, DWORD iOut = 0)
	{
		miUserID = iID;
		miOutsideCount = iOut;
	}
	bool Empty(void)
	{
		return miUserID == 0;
	}
	bool operator== (DWORD iUserID)
	{
		if(miUserID == 0)
			return false;
		return miUserID == iUserID;
	}
};

class CTourAI : public CRegionAI
{
public:
	CTourAI();
	CTourAI(Type type);

	//public:
	//产生时
	virtual bool OnBorn(CRegionCreature* creature);

	//被攻击时
	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	//思考时
	virtual bool OnThink();

	//接到援助信号时
	virtual bool OnAssist(CRegionCreature* target);

	//战斗进行时
	virtual bool OnCombat(CRegionCreature* target);

	//攻击敌人时（与OnBeAttacked对应）
	virtual bool OnAttack(CRegionCreature* target);	

	virtual bool OnArrivePoint();

	virtual void SetStartNode(long* pos);

	virtual void SetGuard(CRegionCreature* pGuard);

	virtual void CancelGuard(DWORD iUserID);

public:
	//寻找目标
	bool DoScanTarget();

	//返回
	bool BeginReturn();
	void DoReturn();

	//继续运动
	void DoMove();
	void DoPatrol();
	void DoIdle();

	//消除移动限制(使npc可以移动)
	void BanishMoveLimit()
	{
		SetLvFlag(AI_PATROL);
	};

	//寻求帮助
	void SeekHelp(CRegionCreature* target);

	//判断护送者们距离自己超过距离就触发护送者脚本事件
	void RangeOutside(bool bDelOutside = false);

	//util
	//保存当前位置（离开当前巡逻路线之前）
	void SaveCurrPos()
	{
		//未曾离开过巡逻路线，则保存
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				(void *)m_creature->m_pos, 
				sizeof(m_TargetNode));

			m_Left = true;
		}
	}

private:
	long							m_StartNode[3];		//起始位置
	char							m_StartDir;			//起始方向
	long							m_TargetNode[3];	//目标节点
	bool							m_Arrived;			//是否到达目标节点
	bool							m_Left;				//是否已离开当前巡逻路线
	std::list<SPathNode>			m_PatrolPath;		//巡逻路径
	std::list<SPathNode>::iterator	m_IterNode;			//指向下一巡逻节点

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

	GuardData	m_vGroudID[8];	//护卫旅行者的人的ID
	DWORD	m_tBorn;		//出生时间
	DWORD	m_tDuration;	//持续时间

	DWORD	m_tBeAttTime;	//最后被攻击的时间

	const DWORD	c_tResumeTime; //从被攻击求助状态恢复的时间周期
	const DWORD	c_iGuardCount;	//最大护卫者总数
};

class CRegionAIMgr : public CRegionObject
{
	friend class CRegionAI;

public:
	CRegionAI*		CreateAI(const std::string& name, DWORD ai_level);

	void			CloseAI(CRegionAI* ai);

protected:
	virtual void	OnTimer(ULONG id, DWORD dwParam[]);

private:
	EXT_SPACE::hash_map<int, int> m_ai;
};

extern CRegionAIMgr g_aiMgr;

#endif // __REGION_CREATURE_H__
