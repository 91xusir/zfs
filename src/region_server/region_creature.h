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
const int CRE_TIMER_HPANDMP             = OBJECT_C_TIMER_BASE + 9;//�Զ���Ѫ����
const int CRE_TIMER_ENP                 = OBJECT_C_TIMER_BASE +10;//�Զ���������
const int CRE_TIMER_TIME                = OBJECT_C_TIMER_BASE +11;//�ӿ�ʼ������������ʱ��
const int CRE_TIMER_TIME2               = OBJECT_C_TIMER_BASE +12;//�ӿ�ʼ������������ʱ�� �������߱���
const int CRE_TIMER_DIE_SELF            = OBJECT_C_TIMER_BASE + 13;
const int CRE_TIMER_STGTTIME            = OBJECT_C_TIMER_BASE + 14;

const int REBIRTH_PROTECT_MILLSECONDS			= 10000;
const int COMBAT_KEEP_MILLSECONDS				= 30000;
const int LEAVE_COMBAT_MILLSECONDS				= 6000;

#define GODLIGHT_MONSTER_COUNT  2//tim.yang Ħ����Ʋ������Ĺ�������
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
//		speed = 0;//��Ӧ������
//		ignore_time = 0;//��Ӧ������
	}

	void Init(int dis,int vSpeed)//add by fox //���ٶ�ͻ��ʱ��������//Ҫ������μ��
	{
		cur = 0;
		max = dis;
		speed = vSpeed;
		//ignore_time = 0;//��Ӧ������
	}

	bool Valid(int dis)
	{
		if(ignore_time > 0 )
		{//Ҫ����
			ignore_time--;
			return true;
		}

		if(cur + dis > max) return false;
		cur += dis;
		return true;
	}

	bool Valid(int dis,int vSpeed,bool &IsDoTeleport)//add by fox //���ٶ�ͻ��ʱ��������//Ҫ������μ��
	{
		if (IsDoTeleport)  //�����˲���ƶ���   �������
		{
			IsDoTeleport = false;
			return true;
		}

		if(ignore_time > 0 )
		{//Ҫ����
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
		{//�ٶȲ�ͬ��Ҫ�����ֵ���ٶȱ��ʱ仯�������cur//��ʵ���Ƿ�����ǰ���
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
	void BackHome();//�س�
	void AddPlayerOnlineTime(long add);//�������������ۻ�ʱ�� 5Сʱ���һ�� 5Сʱ��һ����Ϊ
	/* sync function */
	void RecvCmd(CG_CmdPacket *cmd);
	void Serialize(char type,CG_CmdPacket *cmd);
	void UpdateToClient(char type);
	void UpdateToClientDelta();

	void SendProblemToClient(long lProblemID);

	bool WriteSnapshot(CG_CmdPacket *cmd,CRegionCreature *target);
	bool WriteModelInfo(CG_CmdPacket *cmd);
	bool WriteCreateInfo(CG_CmdPacket *cmd);
	
	void AddCulExp(int Cul); //Tianh ������Ϊ
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
    long          m_dungeonUID;     // ����ΨһID
    long          m_dungeonTID;     // ��������ID
    long          m_dungeonLastQuit;// ���һ���˳�������ʱ��
	DWORD		  m_dwGroupID;
	DWORD		  m_dwLineID;
	SCreature     m_core;
	SCreature     m_oldCore;
	long		  m_pos[3];
	char          m_posFlag;
	char		  m_dir;
	unsigned char m_modelRef;
    //bool          m_bStarted; // �����յ��ͻ��˵�start����Ż������������Ϊtrue
	DWORD         m_lastEmoteTime;
	bool          m_bFunKick;
	bool		  m_bIsInWarfield;
	long		  m_lWarfieldLastQuit;
	char		  m_isVip;
	long		  m_lIsCombat;			// ����Ƿ���ս��״̬ 0��,1��.
	DWORD		  m_dwCombatEndTime;	// ��ҽ���ս��ʱ��.
	long          m_IsHiding;           //Tianh �Ƿ�����  ��ֵ����������  0Ϊ��������

	bool          m_bTime;   //�Ƿ�ر����߱���

	long		  m_lTriggerTime;
	int			  m_nFBprocess;

	int           CulAwardCount;   //������Ϊֵ�Ĵ���
	int           localTime;  //���浱ǰ������
 
	int           ExtractHpR;    //�ٷֱȳ�ȡĿ��Ѫ��������಻�ܶ���һ���̶�ֵ
	int           ExtractMaxHp;  //��ȡĿ��Ѫ�����̶�ֵ

	int           m_ChangeModelID;  //���������ģ��ID //������Ϊ0
	bool          IsChengBody();
	
	int           m_ResistSwim; //�ֿ�ѣ�� 
	int           m_ResistLethargy;//�ֿ���˯
	int           m_ResistBleed;//�ֿ���Ѫ
	int           m_ResistBluntness;//�ֿ��ٶ�

	short         m_ReceiveDamageSkillR; //���ռ����˺�����
	int           m_TeamShareDamage;  //���ѷֵ��˺�
	int           m_ScopeoDinaryInjury;//�Ƿ��Ƿ�Χ��ͨ�˺�--������˺��ٷֱ�(m_ScopeoDinaryInjury/100)

	int           m_EffectProbability;  //�չ�������Ч�ĸ���
	int           m_PTEffectID;         //������Ч��ID
	int           m_SkillSeries;        //Ҫ�����˺��ļ���ϵ��
	int           m_SkilldamageR;       //Ҫ�����˺�

	long          m_UltimateChange;   //�Ƿ��ǳ�������

	long          ReboundHurtR;    //�����˺�
	long          ReboundHurtRType; //�����˺�����

	bool          IsDoTeleport;    //�Ƿ���˲���ƶ���
	        
	// ����NPCר��
	int		triggerID;			// ��������ID
	int		triggerOpen;		// �Ƿ��Ѿ�����
	int		triggerMain;		// �Ƿ������Ҫ�������
	long	triggerIsClose;		// �Ƿ������Ҫ�������

	short   m_IsFirstLogin;     //�Ƿ��ǵ�һ�ε�½
	short   m_IsDead;           //�Ƿ�����  ����Ϊ1

	int	     tempItemID;		//ac.ma
	short    m_IsInvincible;    //�Ƿ��޵�

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

	std::vector<int> vSuitSkill; //�������װ����ID

	//<add by Tianh> ȡ���ݵػ�������
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
	long m_dwCatchMe;	// ������,���ڲ�׽�Լ����.
	long m_dwCatchPet;	// ���ڲ�׽�ĳ���
	bool DropUnRide(float fRate = 1.f);
	bool LinkUpWithPet(long petItemID = 0,bool bResetState = true);
	long m_dwLinkUpPet;	// ����ĳ���
	bool IsLinkUpWithPet();
	void SendPetInfo();

    bool IsTeamFriend(CRegionCreature *cre); //Tianh �Ƿ����Լ��Ķ���

	void Rebirth();
	void Rebirth(char type, int iHPrate = 0, int iMPrate = 0);
	
	bool AddPopularity(long sV,int Popularity);
	bool ToPopularity(int Popularity);
	bool LoadToPopularity(const char* szString);
	std::vector<long> vPopularity;
	string  m_popularity; //����
	
	void UltimateChange(int sV);
    //Tianh ����
	void Hiding(int sV);
	void PlayerLurk(int sV);
	void AddSoul(int MonsterLev);
	void AddSelfDef(CRegionCreature *enemy);
	void RemoveTimeoutSelfDef();
	bool IsSelfDef(CRegionCreature *enemy);
	list<SSelfDef> m_selfDef;
	//<add by Tian> ɾ�������
	void DeleteLifeSkill(unsigned short wID);

	//4.8 LeiJun
	void AttChangeMessageToClient(const char *sMessageType, int iAttValue);

    DWORD GetTargetParty(CRegionCreature* pTarget); // ȡ�õ��˵����ͣ��������Ѿ����о�������skill_base.h��

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
    bool UseSkill(SSkill* pSkill, SAttackParam& param);  // ʹ�ü��ܣ��ڲ������CanUseSkill����Ƿ����ʹ�ã��������ʹ���򷵻�false
    long RecvDamage(CRegionCreature *enemy,int &phyDmg,int eleDmg[MAX_ELEMENT_ATTR],int AttackType = 0);
	//Tianh      isSkill---�Ƿ��Ǽ��ܹ���
	void ApplyDamage(CRegionCreature *enemy,int &phyDmg,int eleDmg[MAX_ELEMENT_ATTR],bool isSkill,SSkill* pSkillAttr = NULL);
	void RefreshEquipWear();

	bool CheckAttackSpeed(long interval,bool test); //���ܼ��
	bool CheckCommonAttackSpeed(long interval,bool test);//��ͨ�������
	bool CheckUseExpItem();
	long Die(CRegionCreature *killer);
	void MeDie(CRegionCreature *killer);
	long GetAttackBusy();
    void AddInvincibility();            // �����޵�״̬(�����ͺ��������޵�)��Add��Remove�ĵ��ô���Ҫ��ͬ
    void RemoveInvincibility();         // �����޵�״̬(�����ͺ��������޵�)��Add��Remove�ĵ��ô���Ҫ��ͬ
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
    void OnAreaEnter(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock); // ������ĳ������
    void OnAreaExit(RtsSceneBlockRtb::SArea* pArea, RtsSceneBlockRtb* pBlock);  // ���뿪ĳ������
    void UpdateAreaPKAttr();
    char GetPKState();

	EPkMode		m_ePkMode;

	//4.7 LeiJun
	bool AtAreaByName(const char *sAreaName);

    /* pk */
    EPKAttr     m_eBlockPosPKAttr;      // ��ǰ�������PK����
    EPKAttr     m_eCurPosPKAttr;        // ��ǰλ�õ�PK����
    // int         m_iPKStateChangeTime;   // �ı�����״̬(�׻ƺ���)��ʣ��ʱ��(ms), ����˵ʣ�¶���ʱ���ı�״̬

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

	// r2cϵͳ����ӿ�    Tianh
	void Systembroadcast(const char *Playername,const char *Name,const char *BossName,const char *SceneName,long type,const char *ItemName);
	//r2cϵͳ����ӿ�
	void ChatNewS(const char *fromName,const char *Name,const char *BossName,const char *SceneName,long type,const char *ItemName);

	//r2c����ӿ�
	void Chat(char channel,const char *msg,const int Range=DEFAULT_CHAT_RANGE, char faction=FACTION_UNKNOWN);
	void Chat(char channel,const char *msg,CRegionCreature *receive_cre,ULONG from_id,const char * from_name);//���Ե�����
	void Chat(char channel,const char *msg,CRegionCreature *receive_cre,CRegionCreature *from_cre=NULL,bool self = false);

    void SendSystemMessage(const char* szSysMsg, CRegionCreature *pFromCre=NULL);
	void SendBattleInfoMessage(const char* szSysMsg, CRegionCreature *pFromCre= NULL);
	
	//Tian 10.04.18 ���������еĳ�����
	bool  ResolvingLinkChat(const char* msg);


	//void LineMoveto(int _lineGroup,int _fRebirthX,int _fRebirthY); //���ߴ���
	/*std::list<SItemID>    m_listChatItemID;*/

	/*void GetItemInfo(long PlayerID,long ID);*/

	//c2r����ӿ�
	void PublicChat(const char *msg);// world chat
	void PartyChat(const char *msg);//ͬ������
	void TeamChat(const char *msg);//��������
	void PrivateChat(const char *target,const char *msg);
	void GmChat(const char *msg, const char *title="");
	void CountryChat(const char *msg);//��������
	void CommonChat(char channel, char const* msg, char const* target = "", char const* title = "");

	DWORD m_WorldChatTime;
	DWORD m_UseItemEggTime;

	long m_moveSpeed;
	long m_attackRange;

	//5.1 LeiJun NPC������ɫ�Լ���ɫ�����ͻ���
	DWORD m_NameColor;		//��ɫ��ʽ��0xAARRGGBB
	//����ɫ������ʾ��ĳ���ͻ���(�û�Creature objectID)�������ͻ�����ʾ��ɫ����Ϊ-1������ʾ��ȫ���ͻ���
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
	CBag*			m_pSellBag;			                // npc���۵Ŀ���������ĵ���
    std::vector<SNpcSellRareItem> m_NpcSellRareItem;    // npc���۵Ĺ�һ��ʱ���ˢ�µĵ���
    int FindInSellRareItem(DWORD itemId);             // ��npc�����б�����һ������
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
	//DWORD m_proTarget; //����Ŀ��ID //��IDΪm_npcInfo->ID

	/* user */
	void Quit();
	CServerTeam*	m_pTeam;	// ����
	CServerMail*	m_pMail;	// �ʼ�
	CServerFriend*  m_pFriend;	// �����б�
	
	/* item function */
	bool UseItem(DWORD Id);						// ʹ�õ���
	bool UseItem(DWORD typeId, EItemColor color);
	bool UseItem(int page, int gridI, int gridJ, CItemContainerBase::EEquipParts part = CItemContainerBase::EQUIP_DEFAULT_PART, bool bBagItem = true);
	void AddPoint(short sPoint);				//��ӵ���
	bool AddItem(SItemID item);					// ������Ʒ������ӵ���
	bool AddItem(DWORD typeId,EItemColor color);
	int  GetItemNum(DWORD typeId, EItemColor color = ItemColor_All);
	int  GetPetNum(DWORD petId);
	bool RemovePetFromBag(DWORD petId, int num = 1);
	bool AddPetByItem(SItemID item);
	bool RemoveItem(DWORD typeId,int num, EItemColor color = ItemColor_All); // ɾ��һ������һ�����͵ĵ��ߣ���������������Ǹ��ӣ�
	bool RemoveItem(DWORD Id, char bMissionBag = 0, int reason = 0); // ɾ��ĳһ��id�ĵ��ߣ���Ե��߸��ӣ�
	int GetDungeonMissionItemNum(DWORD typeId, int difficulty);
	bool AddDungeonMissionItem(DWORD typeId, int difficulty);
	bool RemoveDungeonMissionItem(DWORD typeId, int num, int difficulty);
	SItemID CheckEquipment(DWORD typeId, EItemColor color, CItemContainerBase::EEquipParts* part = 0);
	SItemID CheckEquipment(DWORD Id, CItemContainerBase::EEquipParts* part = 0);

	void ClientDoCommand(char command,long finished,bool sendself = false);

    void QueryAccountPoing();

	bool StopPShop();
	bool StopPlayerTrade();

	DWORD m_dwPetItemOID;		// ����ǳ����ֵΪ���ߵ�ID
	DWORD m_dwPetSave;
    
	CRegionItemContainer *m_pItemContainer;
	CTrade				*m_pTrade;	
	CPrivateShop		*m_pPShop;				// ��̯�̵�
	char				 m_bPShop;				// �Ƿ��̯
	CBag				*m_pBank;
	CBag				*m_pBankPet;
    DWORD               m_dwLastPickItemTime;


	bool                m_bRefuseTeam ;       //�����Ƿ�ܾ����
	bool                m_bRefusetrade;       //�����Ƿ�ܾ�����
	bool                m_bRefuseMail;        //�����Ƿ�ܾ��ʼ�
	bool                m_bRefuseDuel;        //�����Ƿ�ܾ�����
	bool                m_bRefuseFriend;      //�����Ƿ�ܾ�����
	bool                m_bRefuseMonster;     //�����Ƿ�ܾ�ʦͽ

 	/* ���� */
	DWORD m_unionID;							// ��������ID
	string m_unionName;							// ������������
	string m_unionNickName;						// �ƺ�
	string m_unionIcon;							// ����logo�ļ�
	bool IsUnionCaption();						// �ǲ������������

	//��ս���
	string	m_feod_name;
	BYTE	m_is_attacker;

	int		m_hpRemain;
	int		m_mpRemain;
	int		m_EnpRemain;
	int		m_hpRecover;
	int		m_mpRecover;
	int		m_EnpRecover;
	
	int     m_SkillhpRecover;//����ÿ��ָ�Ѫ��
	int     m_SkillmpRecover;//����ÿ��ָ�����

	float	m_hpAutoRecover;
	float	m_mpAutoRecover;
	float	m_hpAutoRecoverRate;
	float	m_mpAutoRecoverRate;
	
	DWORD m_CcombatBusy; //�չ�
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

	//CRegionCreature* m_pHuiYuanHoptoad;//tim.yang   ��Ա��ܾ�
	//int m_HuiYuanHoptoadCount;//��Ա��ܾ���������һ�ν���ֻ����һֻ����
	//void RemoveHuiYuanHoptoad();//�Ƴ���Ա��ܾ�
	ULONG m_curHuiYuanHoptoadOid;//��ǰ�ٻ���ܾ���ObjectID;
	bool IsHuiYuanTimeOver(string huiyuantime);//���ݻ�Աʱ���ж��Ƿ���ӻ�Ա����
	DWORD  m_huiyuanTime;
	//bool m_isHuiYuan;
	bool IsHuiYuan();
	void CreateDropItem(const DWORD itemId,EItemDropStyle dropStyle);//����������ߣ����Բ�ȫ������ӡ�����
	void CalcExploit(CRegionCreature* Killer);
	//����������0��1��2�����¼�
	void GodLightCreateMonster(CRegionCreature *target);//0
	void GodLightDropItem();//1
	void GodLightCreateLight();//2
	CRegionCreature* m_GodLightMonster[GODLIGHT_MONSTER_COUNT];
	byte m_monsterCount;//�������������  ���127ֻ ����ʼ����0ֻ
	byte m_IsAddMonster;//�ж�����Ƿ�ʹ����Ʊ�ɵ�monster
	bool m_bIsDead;//�ж�����Ƿ�ɱ��
	bool m_bIsOutOfLine;//�ж�����Ƿ�ʹ������ٻ��������������
	long m_FBID;     // ����ΨһID
	bool m_bIsFBReborn;
	bool GodLightCreateMonster_AddMonster(CRegionCreature *target,byte monsterCount);
	void GodLightCreateMonster_DelAllMonster();

	//tim.yang  �ճ�����
	void JudgeDayTimeTaskIsAnotherDay();

	//tim.yang  ��鴫����ܾ��Ƿ�Ϸ�  true--�Ϸ�����   false--���Ϸ�������
	bool isteleport(int npcId);

	CMonCastle *m_mc;//tim.yang  MC
	bool m_isAddMCTask;
	void FinishMCTask();
	void RemoveMCTask();

	int m_iKillMonsterNum;			//save role killmonster number ,sever for rige
	int m_iRigelevel;				//save current rige level
	void SetRageLevel(int lev) {m_iRigelevel = lev;}

	//tim.yang  дʹ�õ��ߵ�LOG
	void WriteUseitemLog(int index);
	//end

	//ac.ma			for������
	unsigned long m_playerLastLogoutTime;		//��¼�������ʱ��				
	unsigned long m_playerNowLoginTime;			//��¼��ұ�������ʱ��	
	int m_iIncomeState;					        //����״̬ 100 or 50 or 0
	unsigned long m_lTotalOnline;				//Tianh ����ʱ���ۼ�
	unsigned long m_lTotalOffline;				//����ʱ���ۼ�
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
	void OnBuyTaskItem(int taskType); //- taskType: �������� 0-�ܻ�
#endif
	//---------chg end  by tony 06.05.09--------------------------------------------//
	////---------add start by tony 06.05.09-------------------------//
	////-reason: ������ܻ�����, ��circleTask!=0ȷ��
	////-circleTask==1: Ѱ��
	////-circleTask==2: ����
	////-circleTask==3: ս��
	////-���½����ܻ�������Ч
	//int m_currCircle;						// ��ǰ����, 50��Ϊ1�ֽ���
	//int m_currMonsterNum;					// ��ǰ��ɱ����������--����ս��������Ч
	//int m_finditemID;						// Ѱ�ҵ���ID--����Ѱ��������Ч
	//int m_findnpcID;						// Ѱ��npcID--��������������Ч
	//int m_srcNpcID;							// ��������npcID
	//int m_killmonsterID;					// ɱ������ID--����ս��������Ч	

	int m_tmpcurrCircle;						// ��ǰ����, 50��Ϊ1�ֽ���
	int m_tmpcurrMonsterNum;					// ��ǰ��ɱ����������--����ս��������Ч
	int m_tmpfinditemID;						// Ѱ�ҵ���ID--����Ѱ��������Ч
	int m_tmpfindnpcID;						// Ѱ��npcID--��������������Ч
	int m_tmpsrcNpcID;							// ��������npcID
	int m_tmpkillmonsterID;					// ɱ������ID--����ս��������Ч	
	////---------add end   by tony 06.05.09-------------------------//

	//---------add start by tony 06.05.16---------------//
	//-��װ��1���ڲ��ܹ���
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
	//���㴴��NPC������
	bool				CreateAllUnderlingNpc(short npc_id,char Type,int num = 5,long* pos = NULL);
	//�����ٻ�NPC
	bool				CreateUnderlingNpc(short npc_id, float pos[3]);
	//����BOSS�ٻ�NPC
	bool				CreateUnderlingNpcByBoss(short npc_id, float pos[3]);
	//ɾ������NPC
	void                DeleteAddUnderlingNpc();
	void                DeleteCallNpc(int sV);
	//����ٻ�NPC
	std::vector<CRegionCreature*> CallNpc;

	void                SendMagCallNpc();

	bool IsCallNpc() {/*Tianh �Ƿ����ٻ���NPC*/
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

	// ͳ����Ϣ
#define	PLAYER_MONITOR_CYCLE		1800 // seconds (30*60)
	int					m_nGetMoneyFromMonster;
	int					m_nGetMoneyFromTask;
	int					m_nMonsterCount;
	int					m_nGetItemCount;
	int					m_nGetExp;
    int                 m_MonitorLastTime;

    int                 m_iUserMoveFailedCnt;   // ��鵽�Ƿ��ƶ��Ĵ���
    long                m_lLastCheckProcessTime;// ���һ�μ����̵�ʱ��

	long                m_lAbsClientTime;
    long                m_lLastClientTime;
    long                m_lPosCmdCnt;
    long                m_lFirstPosCmdTime;
    char                m_crcCheckFlag;         // =0��ʱ������CRC��飬����������һ�μ����
    char                m_wgChecked;
    long                m_wgCheckedTime;        // ��鵽��ҵ�ʱ��
    char                m_wgMaybeChecked;
    long                m_wgMaybeCheckedTime;   // ��鵽��ҵ�ʱ��
    time_t              m_tNextCheckProcess;    // ��һ�μ����̵�ʱ��, 0��Ҫ���
    void WgChecked(int iBaseTime=120, int iRandTime=600, char *szCheckInfo = ""); // ��鵽ʹ�����
    void PossiblyUsingWG(int iCnt=1, int iBaseTime=60, int iRandTime=200); // ��鵽����ʹ�����
    void SendWgCheckedToClient();

	// new pet function
	int m_petAbsorbMp;
	int m_petAbsorbHp;

	// ��Ǯ�����������
	long  m_mtBusy;			 // ���������������ӳ�ʱ��
	short m_mtFinish[3];	 // ��������ɵĴ������������������Ѷ�
	short m_mtClearDay;		 // ���һ������������������
	bool  IsDoingMoneyTask();

	// �Ĺ�����
	short m_wenTaskFinish;
	short m_wenTaskAccept;
	short m_wenClearDay;

	// �Ź�����
	short m_gongTaskFinish;
	short m_gongTaskAccept;
	short m_gongClearDay;

	short m_UnionTask;//��������
	short m_UnionClearDay;

	short m_MouTaskFinish;//ı������
	short m_MouTaskAccept;
	short m_MouClearDay;

	bool SaveExtraData(RtMemoryBlock &block);
	bool LoadExtraData(RtMemoryBlock &block);

	bool Save(SUserData &data);
	bool Load(SUserData &data);

	float GetExpRate();
	long  m_cardTime;
	long  m_usingCard;
	long  m_cardLastTime; // ���һ�����Ӵ�ʱ�������
	long  m_LastDieTime;	// ���һ��������ʱ��
	
	void StartUseCard();
	void EndUseCard();
	void ShowCardInfo();
	void ComputeCardTime(long lastTime,bool newBie);

    // ����ܵ��䷽���
    void UpdateMatchToClient();
    void WriteMatchToNet(CG_CmdPacket *cmd);
    void AddMatch(short sMatchID);
    void DelMatch(short sMatchID);
    std::list<short>    m_listMatchID;

private:
    bool m_bNpcSellRareItemStart; // �Ƿ�����ˢ�µ���
    bool m_bScapegoatRemovedDuringDead; // �����������Ƿ��Ѿ�ɾ��������������

private:
	//  [8/20/2009 tim.yang] ˫������ƿʹ������
	DWORD m_lastuseCardMinuteBottleTime;//��¼�ϴ�ʹ��˫������ƿ��ʱ��
	int m_useCardMinuteBottleNum;//��¼˫������ƿ��һ����ʹ�õĴ���
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
    int Prededuct(short sPoint,long event,long lAuctionId = 0); // Ԥ�۵���, �������õ�����NPC�����, ����Ϊ����,event���¼� 1��������
	void Deduct(short sPoint,long orderid,DeductType type,void* v); // �۵���, �������õ�����NPC�����, ����Ϊ����
    void OnPrededuct(long lDeductOrderID, char cResult, short sPoint);
    void OnDeduct(long lDeductOrderID, char cResult, short sPoint);
    // sLockFlag 0Ϊ����, 1Ϊ����
    // lLockSecondΪ����������
    //        1800 Ϊ  30 ����
    //       10800 Ϊ   3 Сʱ
    //      259200 Ϊ   3 ��
    //      432000 Ϊ   5 ��
    //      604800 Ϊ   7 ��
    //      864000 Ϊ  10 ��
    //     2592000 Ϊ  30 ��
    //    25920000 Ϊ 300 �� (��Լ1��)
    //   259200000 Ϊ 3ǧ �� (��Լ10��)
    //   315360000 Ϊ   ʮ�� (ʮ�������Ϊ����Զ)
    void LockAccount(short sLockFlag, long lLockSecond);

    bool CheckBankPass(const std::string& strPass);

    // eType ����Ϊ
	//      CreState_CanSpeak
	//      CreState_CanPShop
	//      CreState_CanMove
    // lTimeSecond ��λΪ��
    bool ForbidUser(bool bBan, ECreatureState eType, long lTimeSecond);

	//5.26 LeiJun �����û�����    Start
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
	//5.26 LeiJun �����û�����    End

	//LeiJun �ű�����״ֵ̬
	long m_lFunctionState[SCRIPT_FUNC_STATE_COUNT];

	char				m_lanCode;
	ULONG				m_userInfo;
	ULONG				m_userId;
	ULONG				m_regionId;
	ULONG				m_connectTime;
	ULONG				m_netDeadTime;
	time_t              m_lastKeepOnlineTime; // ���һ�η�����Ϣ��Gameworld��ʾ˵������ߵ���Ϣ
	std::string			m_username;
	std::string			m_accountName;
	CRegionCreature    *m_dummy;
	long                m_switchRegionSeed;
	int					m_updateTimeStamp;
    bool                m_active;               // �Ƿ񼤻Ĭ�ϼ�����л�����������δ������ʱ���Ǽ���ģ�������״̬����������
    bool                m_bSwitchRegionState;   // �Ƿ����л�Region״̬
    long                privileges;         // Ȩ�� (enum EUserPrivileges [gw_protocol.h])
	bool                m_bSaveData;
	bool                m_bLogout;
	std::string			m_userIpAddress;
	std::string			m_bankPass;
	DWORD				m_dwPrenSysLockTick;	// ʦͽ������lockʱ��
	// long                m_onlineTime;

    // ������Ϣ
    long                m_lPoint, m_tPointExpire; // �����͵�������ʱ��
    char                m_cVIP;                   // VIP��Ϣ��0����VIP
    //char                m_szPointMsg[100];
	CM_MEMDEF(m_szPointMsg, 100)

	// ����ұ���
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
	//-reason:������ܻ�����,����ǰ����ϵͳ�кܴ���,������npc�ϼ�������
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
