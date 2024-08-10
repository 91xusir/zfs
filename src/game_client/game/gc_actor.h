
#ifndef GC_ACTOR_H
#define GC_ACTOR_H

class rt_graph::RtgMatrix16;
class GcActorGraph;
class GcPathNode;
class CComplexMissile;
class CUIForm_Pickup;

class GcActor
{
public:
    friend class GcBaseActor;
    enum ECommands
    {
        // Flag command
        ACMD_NONE               ,   // None

		ACMD_BORN               ,   // 出身
        // Base command
        ACMD_DIE                ,   // 死亡
        ACMD_TELEPORT           ,   // 传送 (x,y,z)
        ACMD_MOVETO             ,   // 移动到指定点 (x,y,z,distance)
        ACMD_FOLLOW             ,   // 移动到指定人一定距离内 (oid, distance)
		ACMD_ATTACK             ,   // 直接攻击 (oid)
		ACMD_USESKILL           ,   // 施放技能 (skill_id)
		ACMD_INTONATE			,	// 吟唱
		ACMD_PICKITEM			,	// 拾取
		ACMD_COLLECT			,	// 采集
		ACMD_PLAYTASKITEM       ,   // 三种任务触发通用命令
		ACMD_CATCHPET			,	// 捕捉宠物
        ACMD_UNDER_ATTACK       ,   // 受击
        ACMD_PLAY_POSE          ,   // 播放动作 dw[0]
        ACMD_CHANGE_ACTOR       ,   // 换模型 pStr[0]
        ACMD_UNEQUIP_ALL        ,   // 卸下装备
        ACMD_EQUIP_ALL          ,   // 装备
        ACMD_DEATH_ACT          ,   // 死亡 dw[0]半透明, dw[1]飞, dw[2]入地
        ACMD_DELETE_SELF        ,   // 把自己从人物列表里面删除

        // Advanced command
        ACMD_ATTACK_SOMEBODY    ,   // 普通攻击某人
        ACMD_SKILL_SOMEWHERE    ,   // 对定点施放技能
        ACMD_SKILL_SOMEBODY     ,   // 对某人施放技能
        ACMD_DEATH              ,   // 死亡
		ACMD_START_TRIGGER		,	// 启动机关

		ACMD_TALKNPC			,	// 跟NPC对话

		ACMD_SHAPESHIFT			,	// 变身

        ACMD_MAX                ,
    };

    enum ENpcType
    {
        ENT_UNKNOWN,		// 不认识，只用来初始化
        ENT_USER,			// 是一个玩家，包括自己和网络的其他用户
        ENT_PET,			// 是一个玩家的兽
        ENT_MONSTER,		// 是怪物
        ENT_NPC_SERVICE,	// 是一个服务NPC（包括任务，交易等）
		ENT_NPC_COMBATIVE,	// 是一个可战斗的NPC
		ENT_SPIRIT,			// 是一个元神
		ENT_CALLNPC			// 是一个召唤补助战斗型NPC
    };

    enum ERideState     // 骑兽状态
    {
        ERS_NONE,       // 空状态，就是自由状态
        ERS_ACTOR,      // 主人状态(骑东西的状态)
        ERS_PET,        // 兽状态(被骑的状态)
    };

    struct SCmd
    {
        ECommands   eCmd;           // 命令类型
        BYTE        cSubType;       // 0 单独的命令, 1 命令的子命令, 2 子命令的结束
        bool        bLoop;          // 这个命令是否循环
        BYTE        cTargetType;    // 目标类型，0没有目标，1人物目标，2地点目标
        union
        {
            float       f[3];
            DWORD       dw[3];
            int         i[3];
            const char  *pStr[3];
            const void  *p[3];
        };
        float           fDistance;
        unsigned short  wSkillID;   // 技能ID
        unsigned short  wAttackID;  // 攻击ID
        float           fRunTime;   // 这个命令已经执行了多少时间
        float           fTime;      // 服务器执行这个命令的时间(如果是玩家自己就为0)
        unsigned short  wCheck;     // 技能ID
        int             iNetCheck;
        SCmd()
        {
            eCmd = ACMD_NONE;
            cSubType = bLoop = false;
            cTargetType = SKILL_TARGET_TYPE_NONE;
            i[0] = i[1] = i[2] = i[3] = 0;
            wSkillID = 0;
            fRunTime = fTime = 0;
            wCheck = 0;
            iNetCheck = 0;
        }
    };

    struct SAttack
    {
        SSkill*     pSkill;         // 0 表示普通攻击
        WORD        wAttackID;      // 攻击ID, 每次攻击都会生成一个ID
        BYTE        cAnswer;        // 0事件存在但是没有播放动作, 1播放动作但是没有到hit点, 2已经到hit点,等待结束了
        BYTE        cTarget;        // 目标类型，0没有目标，1人物目标，2地点目标
        const char* pPoseName;
        SAttack*    pNext;
        float       fTime;          // 攻击时候的时间
        union
        {
            DWORD   dwTargetID;     // 目标ID
            float   f[3];
        };
        struct SDamage
        {
			int   ret;
			bool  criHit;
			char  eleSpe;
			bool  dead;
            DWORD dwActorID;
            short sDamage;
			short eleDmg[MAX_ELEMENT_ATTR];
			int   hits;
        };
        std::vector<SDamage> listDamage;
        void AddDamage(DWORD dwActorID, int ret,short sDamage,short eleDmg[MAX_ELEMENT_ATTR],
			bool criHit,char eleSpe,bool dead)
        {
            SDamage d;
			d.ret = ret;
            d.dwActorID = dwActorID;
            d.sDamage = sDamage;
			d.criHit  = criHit;
			d.eleSpe  = eleSpe;
			d.dead    = dead;
			d.hits    = 0;
			for(int i=0; i<MAX_ELEMENT_ATTR; i++)
				d.eleDmg[i] = eleDmg[i];
            listDamage.push_back(d);
        }
        SAttack()
        {
            Init();
        }
        void Init()
        {
            pSkill = 0;
            dwTargetID = 0;
            cAnswer = 0;
            f[0] = f[1] = f[2] = 0.f;
            pPoseName = NULL;
            fTime = 0.f;
			listDamage.clear();
        }
    };

    GcActor();
    virtual ~GcActor();
    void Destroy();
    bool CreateGraphData(short vModelID, short HeadModelID);            //创建图形
    bool DestroyGraphData();                                            //销毁图形
	virtual void Run(float fSecond);                                            //逻辑循环
    void Render(RtgDevice& inDevice,RTGRenderMask eMask);               //渲染
	void UpdateHUD();

    float Distance(GcActor* pActor);
	float Distance2(GcActor* pActor);

	float Distance(float x,float y);

    DWORD ID();
    void SetID(DWORD vID);

    // set a model id for this actor,rebuild graph resource
    virtual bool SetModel(short Id, short vHeadModelID);
    void DisplayActor(bool bDisplay);
	bool m_bDisplay;
    void SetPKState(char ePKState);
    char GetPKState()                   { return m_ePKState; }

    void                SetName(const char* szName);
    const char*         GetName();
    const RtgMatrix16*  GetMatrix()     { return mBaseActor.GetMatrix();}              //得到角色的位置矩阵
    void                SetMatrix(const RtgMatrix16& _m16) { mBaseActor.SetMatrix(_m16); }
    GcActorGraph*       GetGraph()      { return mBaseActor.GetGraph();}            //得到角色的图形
    RtgVertex3          GetHUDPos();
    ERideState          GetRideState()  { return m_eRideState; }
    DWORD               GetRidePetID()  { return m_pRidePet?m_pRidePet->ID():0; }
	GcActor*			GetRidePet()	{ return m_eRideState==ERS_ACTOR ? m_pRidePet:NULL; }
	GcActor*			GetMaster()		{ return m_eRideState==ERS_PET ? m_pMaster:NULL; }
	GcActor*			GetPetMaster()	{ return m_pMaster; }
    ENpcType            NpcType()           { return m_eNpcType; }
    void DelHUD();
    void SetActorType(ENpcType eNpcType)    { m_eNpcType = eNpcType; }
    bool IsValid()                          { return ((mBaseActor.GetGraph()->p()) != NULL); }
	bool IsNPC()							{ return m_eNpcType==ENT_NPC_SERVICE; }
	bool IsMonster()                        { return false; }
	int  GetMonsterLevel()					{ if(!m_pNpc) return 0; return m_pNpc->Level;}

	virtual bool CheckCanStartTrigger( DWORD triggerID, bool checkRemove = false );
	virtual bool ProcessKey(STaskKey &key);
	bool IsTrigger()					{ if(!m_pNpc) return false; return (m_pNpc->Type == 7 || m_pNpc->Type ==8);}

    bool IsAbiMove();
	bool IsAbiDodge();
	bool IsAbiUseSkill();
	bool IsAbiNormalAttack();
	int  GetAbiResist();
	int  GetAbiStateHit();
	int  IsAbiBeAttack();

    void MoveToNow(int iTileX, int iTileY);
    void MoveToNow(float fX, float fY);

    // fTime 为使用这个命令的时间
	void UseIntonate(WORD wAttackID, WORD wSkillID, float fX, float fY, float fZ, int iNetCheck, float fTime=-1);
	void UsePositionSkill(WORD wAttackID, WORD wSkillID, float fX, float fY, float fZ, int iNetCheck, float fTime=-1);
	void UseIntonate(WORD wAttackID, WORD wSkillID, DWORD dwTargetID, int iNetCheck, float fTime=-1);
    void UseTargetSkill(WORD wAttackID, WORD wSkillID, DWORD dwTargetID, int iNetCheck, float fTime=-1);
    void UseSelfSkill(WORD wAttackID, WORD wSkillID, int iNetCheck, float fTime=-1);
    void AttackSomebody(WORD wAttackID, DWORD dwTargetID, int iNetCheck, float fTime=-1);

    // 注意，所有的命令都有可能被舍弃。所以一定要执行的操作不适合放到命令中
    virtual void AddCommandMoveTo(float fX, float fY, float fZ, float fTime, float fDistance);
    void AddCommandTeleport(int iX, int iY, float fDistance);
    void AddCommand(ECommands eCommand);
    void ClearCommand();
    void RemoveCurCommand(); // 删除当前命令
    void SubmitCurMoveCommand();

    void DoAllCommandNow();

    virtual void OnDie(){}
    virtual void OnPlayFootfall(){}
    //virtual void OnBorn(){}
    virtual void OnPoseBegin(SRT_Pose *pPose); 
    virtual void OnPoseEnd(SRT_Pose *pPose) ;
    virtual void OnPoseEvent(SRT_Pose *pPose, SRT_PoseEvent *pEvent);
    virtual void OnPoseBreak(SRT_Pose *pOldPose,SRT_Pose *pNewPose);

    //调用网络上传攻击命令
    virtual void OnNetUploadAttack(){};

    //技能部分
	void ShowDamageNumber(int ret,short sDamage,short eleDmg[MAX_ELEMENT_ATTR],bool spec=false,char eleSpe=0);
	/*void ShowHeadNotify(char *type,bool spec = false);*/
	void ShowHeadNotify(const char *type, short sDamage, bool spec = false);
	void EquipTrump(bool Active,const char *Model, SItemID &item, STrump *pTrump=NULL);
	void EquipWeapon(bool Active,const char *Model, SWeapon *weapon, SItemID &item);

	void PickItemAdd(long pActorID);					// 添加一个拾取事件到列表中
	void CollectAdd(long pActorID);					// 添加一个采集事件到列表中
	void PlayCollectTask(long pActorID);                //播放采集任务
	void PlayActivityProgressBar(long lActorID, long lTime);		//活动进度条
    void AttackAdd(SAttack* pAttack);                   // 添加一个攻击事件到列表中
    bool AttackAnswer(unsigned short wAttackID, const char* pPoseName); // 响应一个攻击事件
    bool AttackBreak(unsigned short wAttackID);         // 中断一个攻击事件
    void AttackEnd(unsigned short wAttackID);
    SAttack* FindAttackByAttackID(unsigned short wAttackID);           // 追加一个攻击事件的伤害数值
    bool OnAttackArrive(WORD wAttackID, bool bSucceed); // 当应用攻击事件的时候
    void OnAttackArrive(SAttack* pAttack, bool bSucceed); // 当应用攻击事件的时候

	// 网络数据同步
	void UpdateFromServer(CG_CmdPacket *cmd);
	void UpdateFromServerDelta(CG_CmdPacket *cmd);
	void Serialize(char type,CG_CmdPacket *cmd);

//-------------add start by tony 05.06.13--------------------------------//
	//在城战的相关场境内
	bool InCastleWarArea();
//-------------add end   by tony 05.06.13--------------------------------//

	DWORD m_dwPetAbilityLev;
	DWORD m_dwPetFuseLev;
	DWORD m_dwPetMasterID;
	PetType  m_ePetType;
	void SetPetHUD();
#if DO_CONSOLE
    static void  Debug(bool vEnable){mDebug=vEnable;};
    static bool             mDebug;
    void DebugInfo();
#endif//DO_CONSOLE

    void Ride(GcActor* pPet,bool useUserPos = false);   // 骑兽
    void UnRide();				// 下兽

	DWORD GetUnionID()	{ return m_unionID; }
	void SetUnionID(DWORD unionID)	{ m_unionID = unionID; }
    virtual void OnPositionChanged(float fX, float fY);
    virtual void OnRemoveCreature();
    virtual void OnBlockChanged(RtsSceneBlockMap* pNewBlock, RtsSceneBlockMap* pOldBlock);
    virtual void OnAreaEnter(RtsSceneBlockMap::SArea* pArea, RtsSceneBlockMap* pBlock); // 当进入某个区域
    virtual void OnAreaExit(RtsSceneBlockMap::SArea* pArea, RtsSceneBlockMap* pBlock);  // 当离开某个区域
    virtual void UpdateAreaPKAttr();
    void OnMapChanged();

	bool IsIdel() { return m_bIsIdle; }
	bool IsAttack() { return m_bIsAttack; }
	void WantStopAttack() { m_bWantStopAttack =true; }

    bool CreateGraphData(SCreModel* vpModel, short HeadModelID);                        //创建图形

	SCreModel *GetModel() { return m_pModel; }
	virtual void DoCommand(SCmd* pCmd);
	void OnSetWeaponShow(bool bNeed);
   
protected:
    void CreateTrump();

    void Point2Tile(float vX,float vY,WORD& vrTileX,WORD& vrTileY);
    virtual void OnCreateGraph(){return;}; //CreateTrump();};
public:
	void AddCommand(SCmd& vCmd);
	void AddDoCommandPose(char command = 0);
protected:
    bool RunCommand(float fSecond);
    void RunIdle(float fSecond);
	void CommandFinished();         // 完成了当前的命令

	void RunBody(float fSecond);

    void OnAttackEvent(WORD wAttackID);
    void OnAttackEvent(SAttack* pAttack);

public:
	bool AutoEatMPBottle();

    // magic weapon interface
	void MagicWeaponBeginAddMp();
	void MagicWeaponEndAddMp();
	void MagicWeaponBeginAddEle();
	void MagicWeaponBeginAddEle(int type);
	void MagicWeaponEndAddEle();

	void SendTrumpRifine(char rifine, bool stop = false );
	void SendPickItemType( GcActor *pActor );
	void SendPickItemId(SItemID id,long isMouseRD = 0);

    void LivingSkillProduce(short v);
    void LivingSkillSyncretize();
    void LivingSkillItemDecompound();

	void SkillHideModel(char hide = 0);
	void ChangeActorToEffect(const char *effect);
	void ChangeEffectToActor();
	void ChangeCollectTool(bool totool = true);	// 5为采矿,4为采药,其它为扒皮,

	void SetWeaponTrumpVisible(bool weapon,bool trump);	// 设置武器和法宝是否显示

	void StartTriggerAdd(char close,bool final);
	void ShapeshiftAdd(long modelID = 0);
	void TalkNpcAdd(long pActorID);
	void CatchPetAdd(GcActor *pActor);
	void StopCatchPet(char cError);

	static RtwImage* GetHeadImage(char index);

	void DoTrumpSkill(SCmd cmd);

	void vUltimateChange(long sV);   //超级变身

	bool IsShapeshifting();			// 是否正在变身
	bool IsShapeshifted();			// 是否已经变身

	bool DoTeleportSkill();			//  瞬移技能

protected:
    static bool m_bLoadPRI;
    static int  m_PRIPlayer[ACMD_MAX][ACMD_MAX];
    static int  m_PRINetuser[ACMD_MAX][ACMD_MAX];
    const char* GetCmdName(ECommands eCmd);
    void LoadPRI();

public:
	bool State_IsVisible() { return (m_dwState&CreState_Visible) ? true : false; }
	bool State_CanMove() { return (m_dwState&CreState_CanMove) ? true : false; }
	bool State_CanSpeak() { return (m_dwState&CreState_CanSpeak) ? true : false; }
	bool State_CanPShop() { return (m_dwState&CreState_CanPShop) ? true : false; }

	void State_SetVisible(bool bVisible) { m_dwState &= CreState_Visible; }
	void State_SetMovable(bool bCanMove) { m_dwState &= CreState_CanMove; }
	void State_SetSpeakable(bool bSpeakable) { m_dwState &= CreState_CanSpeak; }
	void State_SetPShopable(bool bPShopable) { m_dwState &= CreState_CanPShop; }

	// Task
	void FbTaskState(int taskId, bool& bFinished, bool& bCanFinished);
	bool ProcessTaskKeyList(std::vector<STaskKey> &table, int op, long param);
	bool ProcessTaskKey(STaskKey &key, int op, long param);
	int GetDungeonMissionItemNum(DWORD typeId, int difficulty);

	//-----------add start by tony 05.09.13---------------//
	//void GetFixedReward(int taskId, int& goldRew, int& expRew, int& lev, int& percent);
	//bool GetRandomReward(int taskId);
	void ProcessFixedTaskReward(STaskKey& key, int& goldRew, int& expRew, int& lev, float& percent, int& itemId, int& itemNum, int& itemColor, int& skillID);
	void ProcessRandomTaskReward(STaskKey& key, int* id, int len);
	//-----------add end   by tony 05.09.13---------------//

	void GotoThere(int x, int y);//tim.yang   自动寻路到某地

	void GotoNpcFront(SNpc* pNpc);//tim.yang   自动寻路到某地
public:
	// task info
	CTaskInfo           m_task;

    // 诸侯国部分
	RtString			m_unionNickName;					// 别名
	RtString			m_unionIcon;						// 诸侯国标志
	RtString			m_PShopName;						// 摆摊名字

	std::string			m_feod_name;                        // for feod

	RtwHUD::Ptr			m_pHUD;                             // 头上显示//modify by fox for bug
    SCmd*               m_pCurCmd;                          // 当前命令
	SCmd				m_SCmd;
	SNpc*				m_pNpc;                             // NPC指针

    CItemContainer      mItem;                              // 装备
    GcActorSkill        m_Skill;                            // 技能
    SCreature           m_core;                             // 数据部分
    GcBaseActor         mBaseActor;                         // 基本的角色

	bool                m_bRenderSelect;					// 是否渲染选中圈
    bool                m_bDeleteSelf;                      // 是否删除自己
    bool                m_bShowHUD;                         // 是否显示HUD
	char				m_bPShop;							// 是否在摆摊
	BYTE				m_is_attacker;                      // for feod
    BYTE                mItemChangeCount;                   // 装备改变次数[用来同服务器数据同步]

	//<add by Tianh>   同过快照获得  添加用来显示目标
    BYTE                m_cHpRate;                          // 剩余HP的比率(值为0到100)
	BYTE                m_cMpRate;                          // 剩余MP的比率(值为0到100)
	BYTE                m_actorID;                          // 人物ID
	long                m_lLev;                             // 等级  
	long                m_HeadImageID;                      //人物头像
	//<add by Tianh>

	RtString			m_unionName;						// 诸侯国名字
    int m_iCurrentBlockX, m_iCurrentBlockY;
    std::list<RtsSceneBlockMap::SArea*> m_listArea;

    EPKAttr             m_eCurPosPKAttr;                    // 当前位置的PK属性
    EPKAttr             m_eBlockPosPKAttr;                  // 当前场景块的PK属性
	EPkMode				m_PkMode;							// PK模式

	long				m_lBossMaxHp;                       // Boss最大HP
	long				m_lBossHp;                          // Boss当前HP
	DWORD               m_dwState;                          // 
	int                 m_iCheckSeed;                       // 用来检查外挂
	bool				m_bTrace;
	unsigned long       m_dwIntonateTime;
	int					m_iNeedIntonateTime;
	bool				m_bIntonate;
	char				m_lLastDeadval;// 上次的生存信息
	char				m_bIsDead;
	char				m_cIsDeadNet;
	DWORD				m_dwDeadTime;
	time_t				m_iRebirthTime;
	char				m_cRebirthType;
	char				m_cInTeam;		// 0为无队伍,1为有队伍,2为有队伍并且是队长
	bool				m_bTeamOnOff;
	SItemID				m_RifineTrump;						// 正在修炼的法宝
	bool                m_bIsIdle;                          // 是否是IDLE状态
	bool                m_bIsCombat;                        // 是否在战斗状态
	bool                m_bIsCollect;                       // 是否在采集状态
	bool                bActivity;                          // 活动进度条
	long				m_lPlayIdleTime;
	long				m_lCollectTime;						// 采集时间
	long				m_lPickActorID;
	char				m_cPickType;						// 拾取方式  0为单件,1为全部拾取
	const char			*m_pChangeEffect;					// 是否切换模型为特效文件
	// 采集工具
	CM_MEMDEF(m_pCollectTool, 20)

	// 机关NPC专用
	bool				m_bTriggerOver;						// 机关动画播放完毕
	bool				m_bTriggerStart;					// 机关开始启动(全部触发)
	bool				m_bTriggerClose;					// 机关关闭(0为开,1为关,2为初使状态)
	long				m_lTriggerCloseNet;					// 机关关闭(同步用)
	int					triggerID;							// 所属机关ID
	int					triggerOpen;						// 是否已经触发
	int					triggerMain;						// 是否机关主要表现物件
	int					m_nFBprocess;						// 副本用的流程值
	bool				m_bInDungeon;
	long				m_dungeonUID;
	long				m_dungeonTID;

	bool				m_bInCallPet;						// 在正召唤宠物
	float				m_fVisible;

	char				m_cShapeshift;						// 0没变身,1正在变,2已经变了
	char				m_cShapeshiftNet;					// 0没变身,1正在变,2已经变了,同步用
	DWORD				m_dwShapeshiftModelID;				// 变身后的模型ID

	long                m_SkillSeries;                      //加强技能系列的ID
	long                m_CallNpcCount;                       //召唤的NPC的数量

	RtgVertex3			m_vTelePoint;
	bool				m_bTeleEvent;

	GcBaseActor::EPoses m_eIdlePose;

	map<unsigned short,DWORD> m_mapSetEffect;

	bool				m_bDoMoveCmd;

	DWORD				m_dwCatchMe;						// 宠物用,正在捕捉自己玩家.
	DWORD				m_dwCatchPet;						// 正在捕捉的宠物

private:
    DWORD               m_dwID;                             // 服务器唯一ID
    //std::string         m_strActorName;                     // 玩家的名字
    static EXT_SPACE::unordered_map<GcActor*, std::string> s_strActorNameMap;
    void                ResetHudName();

protected:
    SCreModel*          m_pModel;                           // 模型数据指针
    union
    {
        GcActor*        m_pRidePet;
        GcActor*        m_pMaster;
    };
	int                 m_curAttackEvent;
    CComplexMissile*    mpTrump;                            // 法宝

    std::string         m_curAttackPose;                    // 当前攻击动作

    std::list<SCmd>     m_listCmd;                          // 命令列表
    std::list<SAttack*> m_listAttack;                       // 攻击命令列表

    ENpcType            m_eNpcType;                         // 是否是NPC
    ERideState          m_eRideState;                       // 骑兽状态
    DWORD               m_dwDummyID;                        // 假的ID, 过一段时间后会修改这个ID

	bool				m_bIsAttack;						// 是否允许移动
    WORD                m_wAttackID;                        // 普通攻击
	bool				m_bHideHUD;							// 是否显示HUD

	int                 m_curAttackId;                      //
    char                m_ePKState;                         // PK状态
	bool				m_bWantStopAttack;					// 攻击时曾经试图停止攻击
    DWORD               m_dwDummyID2;                       // 服务器唯一ID
	DWORD				m_unionID;							// 诸侯国ID
	int					m_pLastTargetID;
	bool                bTaskCollect;                       //是否任务采集
	CM_MEMDEF(m_linkName, 20)
public:
	inline void ResetTargetID(){m_pLastTargetID=0;}
	void SetIsTaskCollect(bool bTask)
	{ 
		if (bTask)
		{
			bTaskCollect = true;
		} 
		else
		{
			bTaskCollect = false;
		}
	}
	bool GetIsTaskCollect(){ return bTaskCollect; }
	void SetInActivity(bool bActi)
	{
		bActivity = bActi;
	}
};


#endif  //GC_ACTOR_H

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
