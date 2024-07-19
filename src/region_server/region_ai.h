#ifndef __REGION_AI_H__
#define __REGION_AI_H__


//-------------------------------------------------------
//- ��     ��    ��ѡ    �˶�ģʽ(����)
//-0000   0000   0000      0000
//-------------------------------------------------------
#define AI_NONE		0x0000		//���κι���
#define AI_NORMAL	0xff00		//�����⹦��
#define AI_FIXED	0x0001		//�̶�����
#define AI_IDLE		0x0002		//����˶�
#define AI_PATROL	0x0004		//Ѳ��
#define AI_PROTECT	0x0010		//���ػ�����
#define AI_CONJURE	0x0020		//���ٻ�����

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
//�ٻ�AI
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
	CRegionCreature*	m_creature;			//������������
	DWORD				m_SkillID;			//�ٻ�����ID
	short				m_NpcID;			//�ٻ�����������ID
	short				m_Count;			//�ٻ�������������
	std::vector<DWORD>	m_ObjectID;			//�ٻ���������ID
	int					m_ConjuredNum;		//�ٻ�����ʹ�ô���
};
//-----------------------------------------------------------------------------

class CRegionAI : public CRegionObject
{
public:
	enum Type
	{
		TYPE_MONSTER,	// ����
		TYPE_PET,		// ����
		TYPE_GUARDER,	// ����
		TYPE_HERO,		// ����
		TYPE_TOUR,		// ������
		TYPE_CALLNPC    // �ٻ���NPC
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
	 * �ñ�־����ai�ľ�������ģʽ
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

	virtual bool OnSuccor(CRegionCreature* target);		//����Ŀ��

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

	short m_AnimDurativeTime;	//ԭ�ض���״̬����ʱ��
	string m_PlayAnimName;		//����ԭ�ض���������

	long m_nextNodePos[3];		// ��һ��Ҫ�ߵĸ��ӵ�λ��
	long m_currNodePos[3];		// �Ѿ��߹����Ǹ����ӵ�λ��
	bool m_bHaveNextNodePos;	// m_nextNodePos��m_currNodePos�Ƿ��Ѿ�����ֵ��
	CRegionConjureAI* m_pConjureAI;
	bool m_isBeAttacked;
	bool	m_bTrace;			//������׷��
	DWORD m_dwForceReturnTime;

	bool m_bActive;

	DWORD	m_tTalkTime;	//���һ�κ�����ʱ��
	const DWORD	c_tTalkCycleTime;	//��������ʱ��
};

class CRegionMonsterAI : public CRegionAI
{
public:
	virtual bool OnBorn(CRegionCreature* creature);

	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	virtual bool OnThink();

	virtual bool OnAssist(CRegionCreature* target);		//Ⱥ����ai

	virtual bool OnCombat(CRegionCreature* target);

	virtual bool OnAttack(CRegionCreature* target);	

	virtual bool OnLostLeader(CRegionCreature* leader);

	virtual bool OnUnderlingBorn(CRegionCreature* underling);

	virtual bool OnUnderlingDead(CRegionCreature* underling);

	void SeekHelp();	//

	virtual bool OnSuccor(CRegionCreature* target);		//����Ŀ��

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
	//���浱ǰλ�ã��뿪��ǰѲ��·��֮ǰ��
	void SaveCurrPos()
	{
		//δ���뿪��Ѳ��·�ߣ��򱣴�
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
	long							m_orignalPos[3];	//��ʼλ��
	long							m_TargetNode[3];	//Ŀ��ڵ�
	bool							m_Arrived;			//�Ƿ񵽴�Ŀ��ڵ�
	bool							m_Left;				//�Ƿ����뿪��ǰѲ��·��
	std::list<SPathNode>			m_PatrolPath;		//Ѳ��·��
	std::list<SPathNode>::iterator	m_IterNode;			//ָ����һѲ�߽ڵ�
	
	int m_patrolInertia;
	int m_patrolSpeed;
	long m_face[2];
	float m_moveLengthPerStep;	

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

	DWORD	m_proTargetID;
	int		m_SuccorMode;
	int		m_SelfRewardNum;	//���Լ�ʩ������ħ������
	int		m_ConjuredNum;		//�ٻ�����ʹ�ô���
};

class CRegionGuarderAI : public CRegionAI
{

public:
	CRegionGuarderAI();

	//public:
	//����ʱ
	virtual bool OnBorn(CRegionCreature* creature);

	//������ʱ
	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	//˼��ʱ
	virtual bool OnThink();

	//�ӵ�Ԯ���ź�ʱ
	virtual bool OnAssist(CRegionCreature* target);

	//ս������ʱ
	virtual bool OnCombat(CRegionCreature* target);

	//��������ʱ����OnBeAttacked��Ӧ��
	virtual bool OnAttack(CRegionCreature* target);	

	//�ӳ�����ʱ
	virtual bool OnLostLeader(CRegionCreature* leader);
	
public:
	//Ѱ��Ŀ�꣨������� ������Ӫ��ң�
	//true:�ҵ�Ŀ�� false:δ�ҵ�
	bool DoScanTarget();

	//����
	bool BeginReturn();
	void DoReturn();

	//�����˶�
	void DoMove();
	void DoPatrol();
	void DoIdle();

	//Ѱ�����
	void SeekHelp(CRegionCreature* target);

	//util
	//���浱ǰλ�ã��뿪��ǰѲ��·��֮ǰ��
	void SaveCurrPos()
	{
		//δ���뿪��Ѳ��·�ߣ��򱣴�
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				   (void *)m_creature->m_pos, 
				   sizeof(m_TargetNode));

			m_Left = true;
		}
	}

private:
	long							m_StartNode[3];		//��ʼλ��
	char							m_StartDir;			//��ʼ����
	long							m_TargetNode[3];	//Ŀ��ڵ�
	bool							m_Arrived;			//�Ƿ񵽴�Ŀ��ڵ�
	bool							m_Left;				//�Ƿ����뿪��ǰѲ��·��
	std::list<SPathNode>			m_PatrolPath;		//Ѳ��·��
	std::list<SPathNode>::iterator	m_IterNode;			//ָ����һѲ�߽ڵ�

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

};

//����npcAI
/*
 *	�������ս�������͵�
 */
class CRegionAssistantAI : public CRegionAI
{

public:
	CRegionAssistantAI();
	CRegionAssistantAI(Type type);

	//public:
	//����ʱ
	virtual bool OnBorn(CRegionCreature* creature);

	//������ʱ
	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	//˼��ʱ
	virtual bool OnThink();

	//�ӵ�Ԯ���ź�ʱ
	virtual bool OnAssist(CRegionCreature* target);

	//ս������ʱ
	virtual bool OnCombat(CRegionCreature* target);

	//��������ʱ����OnBeAttacked��Ӧ��
	virtual bool OnAttack(CRegionCreature* target);	

    virtual bool OnArrivePoint();

	virtual void SetStartNode(long* pos);
	
public:
	//Ѱ��Ŀ��
	bool DoScanTarget();

	//����
	bool BeginReturn();
	void DoReturn();

	//�����˶�
	void DoMove();
	void DoPatrol();
	void DoIdle();

	//�����ƶ�����(ʹnpc�����ƶ�)
	void BanishMoveLimit()
	{
		SetLvFlag(AI_PATROL);
	};

	//Ѱ�����
	void SeekHelp(CRegionCreature* target);

	//util
	//���浱ǰλ�ã��뿪��ǰѲ��·��֮ǰ��
	void SaveCurrPos()
	{
		//δ���뿪��Ѳ��·�ߣ��򱣴�
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				(void *)m_creature->m_pos, 
				sizeof(m_TargetNode));

			m_Left = true;
		}
	}

private:
	long							m_StartNode[3];		//��ʼλ��
	char							m_StartDir;			//��ʼ����
	long							m_TargetNode[3];	//Ŀ��ڵ�
	bool							m_Arrived;			//�Ƿ񵽴�Ŀ��ڵ�
	bool							m_Left;				//�Ƿ����뿪��ǰѲ��·��
	std::list<SPathNode>			m_PatrolPath;		//Ѳ��·��
	std::list<SPathNode>::iterator	m_IterNode;			//ָ����һѲ�߽ڵ�

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

	DWORD	m_tBorn;		//����ʱ��
	DWORD	m_tDuration;	//����ʱ��

};

class CRegionBossAI  : public CRegionMonsterAI
{
public:
	virtual bool OnDead();

	virtual bool OnAssist(CRegionCreature* target);

	virtual bool OnUnderlingBeAttacked(CRegionCreature* underling, CRegionCreature* target, int hate);

	virtual bool OnBeAttacked(CRegionCreature* target, int hate);
};

//�ٻ�NPCAI
class CRegionCallNpc : public CRegionAI
{
public:
	CRegionCallNpc();
	~CRegionCallNpc();
public:
	enum AttackState
	{
		AS_ATTACK,	 // ����
		AS_SYNERGY,	 // Эͬ
		AS_PASSIVE,	 // ����
		AS_DEF		 // ����
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
//tim.yang  ����ٻ���AI
class CGodLightMonsterAI  : public CRegionMonsterAI
{
private:
	long							m_orignalPos[3];	//��ʼλ��

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
	std::vector<SPathPoint> m_pathPoint;//�Զ�Ѱ·��·��
	long m_curTargetPos[3];//ÿ���ƶ���Ŀ��㣬����ȫ��·���б��еĵ�
	std::vector<SPathPoint>::iterator m_pathPointNext;//��һ��·��
	ULONG m_attackTargetObj;
};
//endl

class CRegionPetAI : public CRegionAI
{
public:

	enum AttackState
	{
		AS_ATTACK,	 // ����
		AS_SYNERGY,	 // Эͬ
		AS_PASSIVE,	 // ����
		AS_DEF		 // ����
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
	//����ʱ
	virtual bool OnBorn(CRegionCreature* creature);

	//������ʱ
	virtual bool OnBeAttacked(CRegionCreature* target, int hate);

	//˼��ʱ
	virtual bool OnThink();

	//�ӵ�Ԯ���ź�ʱ
	virtual bool OnAssist(CRegionCreature* target);

	//ս������ʱ
	virtual bool OnCombat(CRegionCreature* target);

	//��������ʱ����OnBeAttacked��Ӧ��
	virtual bool OnAttack(CRegionCreature* target);	

	virtual bool OnArrivePoint();

	virtual void SetStartNode(long* pos);

	virtual void SetGuard(CRegionCreature* pGuard);

	virtual void CancelGuard(DWORD iUserID);

public:
	//Ѱ��Ŀ��
	bool DoScanTarget();

	//����
	bool BeginReturn();
	void DoReturn();

	//�����˶�
	void DoMove();
	void DoPatrol();
	void DoIdle();

	//�����ƶ�����(ʹnpc�����ƶ�)
	void BanishMoveLimit()
	{
		SetLvFlag(AI_PATROL);
	};

	//Ѱ�����
	void SeekHelp(CRegionCreature* target);

	//�жϻ������Ǿ����Լ���������ʹ��������߽ű��¼�
	void RangeOutside(bool bDelOutside = false);

	//util
	//���浱ǰλ�ã��뿪��ǰѲ��·��֮ǰ��
	void SaveCurrPos()
	{
		//δ���뿪��Ѳ��·�ߣ��򱣴�
		if(!m_Left)
		{
			memcpy((void *)m_TargetNode, 
				(void *)m_creature->m_pos, 
				sizeof(m_TargetNode));

			m_Left = true;
		}
	}

private:
	long							m_StartNode[3];		//��ʼλ��
	char							m_StartDir;			//��ʼ����
	long							m_TargetNode[3];	//Ŀ��ڵ�
	bool							m_Arrived;			//�Ƿ񵽴�Ŀ��ڵ�
	bool							m_Left;				//�Ƿ����뿪��ǰѲ��·��
	std::list<SPathNode>			m_PatrolPath;		//Ѳ��·��
	std::list<SPathNode>::iterator	m_IterNode;			//ָ����һѲ�߽ڵ�

	bool m_bHaveIdlePos;
	long m_idleToPos[3];

	GuardData	m_vGroudID[8];	//���������ߵ��˵�ID
	DWORD	m_tBorn;		//����ʱ��
	DWORD	m_tDuration;	//����ʱ��

	DWORD	m_tBeAttTime;	//��󱻹�����ʱ��

	const DWORD	c_tResumeTime; //�ӱ���������״̬�ָ���ʱ������
	const DWORD	c_iGuardCount;	//�����������
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
