
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

		ACMD_BORN               ,   // ����
        // Base command
        ACMD_DIE                ,   // ����
        ACMD_TELEPORT           ,   // ���� (x,y,z)
        ACMD_MOVETO             ,   // �ƶ���ָ���� (x,y,z,distance)
        ACMD_FOLLOW             ,   // �ƶ���ָ����һ�������� (oid, distance)
		ACMD_ATTACK             ,   // ֱ�ӹ��� (oid)
		ACMD_USESKILL           ,   // ʩ�ż��� (skill_id)
		ACMD_INTONATE			,	// ����
		ACMD_PICKITEM			,	// ʰȡ
		ACMD_COLLECT			,	// �ɼ�
		ACMD_PLAYTASKITEM       ,   // �������񴥷�ͨ������
		ACMD_CATCHPET			,	// ��׽����
        ACMD_UNDER_ATTACK       ,   // �ܻ�
        ACMD_PLAY_POSE          ,   // ���Ŷ��� dw[0]
        ACMD_CHANGE_ACTOR       ,   // ��ģ�� pStr[0]
        ACMD_UNEQUIP_ALL        ,   // ж��װ��
        ACMD_EQUIP_ALL          ,   // װ��
        ACMD_DEATH_ACT          ,   // ���� dw[0]��͸��, dw[1]��, dw[2]���
        ACMD_DELETE_SELF        ,   // ���Լ��������б�����ɾ��

        // Advanced command
        ACMD_ATTACK_SOMEBODY    ,   // ��ͨ����ĳ��
        ACMD_SKILL_SOMEWHERE    ,   // �Զ���ʩ�ż���
        ACMD_SKILL_SOMEBODY     ,   // ��ĳ��ʩ�ż���
        ACMD_DEATH              ,   // ����
		ACMD_START_TRIGGER		,	// ��������

		ACMD_TALKNPC			,	// ��NPC�Ի�

		ACMD_SHAPESHIFT			,	// ����

        ACMD_MAX                ,
    };

    enum ENpcType
    {
        ENT_UNKNOWN,		// ����ʶ��ֻ������ʼ��
        ENT_USER,			// ��һ����ң������Լ�������������û�
        ENT_PET,			// ��һ����ҵ���
        ENT_MONSTER,		// �ǹ���
        ENT_NPC_SERVICE,	// ��һ������NPC���������񣬽��׵ȣ�
		ENT_NPC_COMBATIVE,	// ��һ����ս����NPC
		ENT_SPIRIT,			// ��һ��Ԫ��
		ENT_CALLNPC			// ��һ���ٻ�����ս����NPC
    };

    enum ERideState     // ����״̬
    {
        ERS_NONE,       // ��״̬����������״̬
        ERS_ACTOR,      // ����״̬(�ﶫ����״̬)
        ERS_PET,        // ��״̬(�����״̬)
    };

    struct SCmd
    {
        ECommands   eCmd;           // ��������
        BYTE        cSubType;       // 0 ����������, 1 �����������, 2 ������Ľ���
        bool        bLoop;          // ��������Ƿ�ѭ��
        BYTE        cTargetType;    // Ŀ�����ͣ�0û��Ŀ�꣬1����Ŀ�꣬2�ص�Ŀ��
        union
        {
            float       f[3];
            DWORD       dw[3];
            int         i[3];
            const char  *pStr[3];
            const void  *p[3];
        };
        float           fDistance;
        unsigned short  wSkillID;   // ����ID
        unsigned short  wAttackID;  // ����ID
        float           fRunTime;   // ��������Ѿ�ִ���˶���ʱ��
        float           fTime;      // ������ִ����������ʱ��(���������Լ���Ϊ0)
        unsigned short  wCheck;     // ����ID
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
        SSkill*     pSkill;         // 0 ��ʾ��ͨ����
        WORD        wAttackID;      // ����ID, ÿ�ι�����������һ��ID
        BYTE        cAnswer;        // 0�¼����ڵ���û�в��Ŷ���, 1���Ŷ�������û�е�hit��, 2�Ѿ���hit��,�ȴ�������
        BYTE        cTarget;        // Ŀ�����ͣ�0û��Ŀ�꣬1����Ŀ�꣬2�ص�Ŀ��
        const char* pPoseName;
        SAttack*    pNext;
        float       fTime;          // ����ʱ���ʱ��
        union
        {
            DWORD   dwTargetID;     // Ŀ��ID
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
    bool CreateGraphData(short vModelID, short HeadModelID);            //����ͼ��
    bool DestroyGraphData();                                            //����ͼ��
	virtual void Run(float fSecond);                                            //�߼�ѭ��
    void Render(RtgDevice& inDevice,RTGRenderMask eMask);               //��Ⱦ
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
    const RtgMatrix16*  GetMatrix()     { return mBaseActor.GetMatrix();}              //�õ���ɫ��λ�þ���
    void                SetMatrix(const RtgMatrix16& _m16) { mBaseActor.SetMatrix(_m16); }
    GcActorGraph*       GetGraph()      { return mBaseActor.GetGraph();}            //�õ���ɫ��ͼ��
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

    // fTime Ϊʹ����������ʱ��
	void UseIntonate(WORD wAttackID, WORD wSkillID, float fX, float fY, float fZ, int iNetCheck, float fTime=-1);
	void UsePositionSkill(WORD wAttackID, WORD wSkillID, float fX, float fY, float fZ, int iNetCheck, float fTime=-1);
	void UseIntonate(WORD wAttackID, WORD wSkillID, DWORD dwTargetID, int iNetCheck, float fTime=-1);
    void UseTargetSkill(WORD wAttackID, WORD wSkillID, DWORD dwTargetID, int iNetCheck, float fTime=-1);
    void UseSelfSkill(WORD wAttackID, WORD wSkillID, int iNetCheck, float fTime=-1);
    void AttackSomebody(WORD wAttackID, DWORD dwTargetID, int iNetCheck, float fTime=-1);

    // ע�⣬���е�����п��ܱ�����������һ��Ҫִ�еĲ������ʺϷŵ�������
    virtual void AddCommandMoveTo(float fX, float fY, float fZ, float fTime, float fDistance);
    void AddCommandTeleport(int iX, int iY, float fDistance);
    void AddCommand(ECommands eCommand);
    void ClearCommand();
    void RemoveCurCommand(); // ɾ����ǰ����
    void SubmitCurMoveCommand();

    void DoAllCommandNow();

    virtual void OnDie(){}
    virtual void OnPlayFootfall(){}
    //virtual void OnBorn(){}
    virtual void OnPoseBegin(SRT_Pose *pPose); 
    virtual void OnPoseEnd(SRT_Pose *pPose) ;
    virtual void OnPoseEvent(SRT_Pose *pPose, SRT_PoseEvent *pEvent);
    virtual void OnPoseBreak(SRT_Pose *pOldPose,SRT_Pose *pNewPose);

    //���������ϴ���������
    virtual void OnNetUploadAttack(){};

    //���ܲ���
	void ShowDamageNumber(int ret,short sDamage,short eleDmg[MAX_ELEMENT_ATTR],bool spec=false,char eleSpe=0);
	/*void ShowHeadNotify(char *type,bool spec = false);*/
	void ShowHeadNotify(const char *type, short sDamage, bool spec = false);
	void EquipTrump(bool Active,const char *Model, SItemID &item, STrump *pTrump=NULL);
	void EquipWeapon(bool Active,const char *Model, SWeapon *weapon, SItemID &item);

	void PickItemAdd(long pActorID);					// ���һ��ʰȡ�¼����б���
	void CollectAdd(long pActorID);					// ���һ���ɼ��¼����б���
	void PlayCollectTask(long pActorID);                //���Ųɼ�����
	void PlayActivityProgressBar(long lActorID, long lTime);		//�������
    void AttackAdd(SAttack* pAttack);                   // ���һ�������¼����б���
    bool AttackAnswer(unsigned short wAttackID, const char* pPoseName); // ��Ӧһ�������¼�
    bool AttackBreak(unsigned short wAttackID);         // �ж�һ�������¼�
    void AttackEnd(unsigned short wAttackID);
    SAttack* FindAttackByAttackID(unsigned short wAttackID);           // ׷��һ�������¼����˺���ֵ
    bool OnAttackArrive(WORD wAttackID, bool bSucceed); // ��Ӧ�ù����¼���ʱ��
    void OnAttackArrive(SAttack* pAttack, bool bSucceed); // ��Ӧ�ù����¼���ʱ��

	// ��������ͬ��
	void UpdateFromServer(CG_CmdPacket *cmd);
	void UpdateFromServerDelta(CG_CmdPacket *cmd);
	void Serialize(char type,CG_CmdPacket *cmd);

//-------------add start by tony 05.06.13--------------------------------//
	//�ڳ�ս����س�����
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

    void Ride(GcActor* pPet,bool useUserPos = false);   // ����
    void UnRide();				// ����

	DWORD GetUnionID()	{ return m_unionID; }
	void SetUnionID(DWORD unionID)	{ m_unionID = unionID; }
    virtual void OnPositionChanged(float fX, float fY);
    virtual void OnRemoveCreature();
    virtual void OnBlockChanged(RtsSceneBlockMap* pNewBlock, RtsSceneBlockMap* pOldBlock);
    virtual void OnAreaEnter(RtsSceneBlockMap::SArea* pArea, RtsSceneBlockMap* pBlock); // ������ĳ������
    virtual void OnAreaExit(RtsSceneBlockMap::SArea* pArea, RtsSceneBlockMap* pBlock);  // ���뿪ĳ������
    virtual void UpdateAreaPKAttr();
    void OnMapChanged();

	bool IsIdel() { return m_bIsIdle; }
	bool IsAttack() { return m_bIsAttack; }
	void WantStopAttack() { m_bWantStopAttack =true; }

    bool CreateGraphData(SCreModel* vpModel, short HeadModelID);                        //����ͼ��

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
	void CommandFinished();         // ����˵�ǰ������

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
	void ChangeCollectTool(bool totool = true);	// 5Ϊ�ɿ�,4Ϊ��ҩ,����Ϊ��Ƥ,

	void SetWeaponTrumpVisible(bool weapon,bool trump);	// ���������ͷ����Ƿ���ʾ

	void StartTriggerAdd(char close,bool final);
	void ShapeshiftAdd(long modelID = 0);
	void TalkNpcAdd(long pActorID);
	void CatchPetAdd(GcActor *pActor);
	void StopCatchPet(char cError);

	static RtwImage* GetHeadImage(char index);

	void DoTrumpSkill(SCmd cmd);

	void vUltimateChange(long sV);   //��������

	bool IsShapeshifting();			// �Ƿ����ڱ���
	bool IsShapeshifted();			// �Ƿ��Ѿ�����

	bool DoTeleportSkill();			//  ˲�Ƽ���

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

	void GotoThere(int x, int y);//tim.yang   �Զ�Ѱ·��ĳ��

	void GotoNpcFront(SNpc* pNpc);//tim.yang   �Զ�Ѱ·��ĳ��
public:
	// task info
	CTaskInfo           m_task;

    // ��������
	RtString			m_unionNickName;					// ����
	RtString			m_unionIcon;						// ������־
	RtString			m_PShopName;						// ��̯����

	std::string			m_feod_name;                        // for feod

	RtwHUD::Ptr			m_pHUD;                             // ͷ����ʾ//modify by fox for bug
    SCmd*               m_pCurCmd;                          // ��ǰ����
	SCmd				m_SCmd;
	SNpc*				m_pNpc;                             // NPCָ��

    CItemContainer      mItem;                              // װ��
    GcActorSkill        m_Skill;                            // ����
    SCreature           m_core;                             // ���ݲ���
    GcBaseActor         mBaseActor;                         // �����Ľ�ɫ

	bool                m_bRenderSelect;					// �Ƿ���Ⱦѡ��Ȧ
    bool                m_bDeleteSelf;                      // �Ƿ�ɾ���Լ�
    bool                m_bShowHUD;                         // �Ƿ���ʾHUD
	char				m_bPShop;							// �Ƿ��ڰ�̯
	BYTE				m_is_attacker;                      // for feod
    BYTE                mItemChangeCount;                   // װ���ı����[����ͬ����������ͬ��]

	//<add by Tianh>   ͬ�����ջ��  ���������ʾĿ��
    BYTE                m_cHpRate;                          // ʣ��HP�ı���(ֵΪ0��100)
	BYTE                m_cMpRate;                          // ʣ��MP�ı���(ֵΪ0��100)
	BYTE                m_actorID;                          // ����ID
	long                m_lLev;                             // �ȼ�  
	long                m_HeadImageID;                      //����ͷ��
	//<add by Tianh>

	RtString			m_unionName;						// ��������
    int m_iCurrentBlockX, m_iCurrentBlockY;
    std::list<RtsSceneBlockMap::SArea*> m_listArea;

    EPKAttr             m_eCurPosPKAttr;                    // ��ǰλ�õ�PK����
    EPKAttr             m_eBlockPosPKAttr;                  // ��ǰ�������PK����
	EPkMode				m_PkMode;							// PKģʽ

	long				m_lBossMaxHp;                       // Boss���HP
	long				m_lBossHp;                          // Boss��ǰHP
	DWORD               m_dwState;                          // 
	int                 m_iCheckSeed;                       // ����������
	bool				m_bTrace;
	unsigned long       m_dwIntonateTime;
	int					m_iNeedIntonateTime;
	bool				m_bIntonate;
	char				m_lLastDeadval;// �ϴε�������Ϣ
	char				m_bIsDead;
	char				m_cIsDeadNet;
	DWORD				m_dwDeadTime;
	time_t				m_iRebirthTime;
	char				m_cRebirthType;
	char				m_cInTeam;		// 0Ϊ�޶���,1Ϊ�ж���,2Ϊ�ж��鲢���Ƕӳ�
	bool				m_bTeamOnOff;
	SItemID				m_RifineTrump;						// ���������ķ���
	bool                m_bIsIdle;                          // �Ƿ���IDLE״̬
	bool                m_bIsCombat;                        // �Ƿ���ս��״̬
	bool                m_bIsCollect;                       // �Ƿ��ڲɼ�״̬
	bool                bActivity;                          // �������
	long				m_lPlayIdleTime;
	long				m_lCollectTime;						// �ɼ�ʱ��
	long				m_lPickActorID;
	char				m_cPickType;						// ʰȡ��ʽ  0Ϊ����,1Ϊȫ��ʰȡ
	const char			*m_pChangeEffect;					// �Ƿ��л�ģ��Ϊ��Ч�ļ�
	// �ɼ�����
	CM_MEMDEF(m_pCollectTool, 20)

	// ����NPCר��
	bool				m_bTriggerOver;						// ���ض����������
	bool				m_bTriggerStart;					// ���ؿ�ʼ����(ȫ������)
	bool				m_bTriggerClose;					// ���عر�(0Ϊ��,1Ϊ��,2Ϊ��ʹ״̬)
	long				m_lTriggerCloseNet;					// ���عر�(ͬ����)
	int					triggerID;							// ��������ID
	int					triggerOpen;						// �Ƿ��Ѿ�����
	int					triggerMain;						// �Ƿ������Ҫ�������
	int					m_nFBprocess;						// �����õ�����ֵ
	bool				m_bInDungeon;
	long				m_dungeonUID;
	long				m_dungeonTID;

	bool				m_bInCallPet;						// �����ٻ�����
	float				m_fVisible;

	char				m_cShapeshift;						// 0û����,1���ڱ�,2�Ѿ�����
	char				m_cShapeshiftNet;					// 0û����,1���ڱ�,2�Ѿ�����,ͬ����
	DWORD				m_dwShapeshiftModelID;				// ������ģ��ID

	long                m_SkillSeries;                      //��ǿ����ϵ�е�ID
	long                m_CallNpcCount;                       //�ٻ���NPC������

	RtgVertex3			m_vTelePoint;
	bool				m_bTeleEvent;

	GcBaseActor::EPoses m_eIdlePose;

	map<unsigned short,DWORD> m_mapSetEffect;

	bool				m_bDoMoveCmd;

	DWORD				m_dwCatchMe;						// ������,���ڲ�׽�Լ����.
	DWORD				m_dwCatchPet;						// ���ڲ�׽�ĳ���

private:
    DWORD               m_dwID;                             // ������ΨһID
    //std::string         m_strActorName;                     // ��ҵ�����
    static EXT_SPACE::unordered_map<GcActor*, std::string> s_strActorNameMap;
    void                ResetHudName();

protected:
    SCreModel*          m_pModel;                           // ģ������ָ��
    union
    {
        GcActor*        m_pRidePet;
        GcActor*        m_pMaster;
    };
	int                 m_curAttackEvent;
    CComplexMissile*    mpTrump;                            // ����

    std::string         m_curAttackPose;                    // ��ǰ��������

    std::list<SCmd>     m_listCmd;                          // �����б�
    std::list<SAttack*> m_listAttack;                       // ���������б�

    ENpcType            m_eNpcType;                         // �Ƿ���NPC
    ERideState          m_eRideState;                       // ����״̬
    DWORD               m_dwDummyID;                        // �ٵ�ID, ��һ��ʱ�����޸����ID

	bool				m_bIsAttack;						// �Ƿ������ƶ�
    WORD                m_wAttackID;                        // ��ͨ����
	bool				m_bHideHUD;							// �Ƿ���ʾHUD

	int                 m_curAttackId;                      //
    char                m_ePKState;                         // PK״̬
	bool				m_bWantStopAttack;					// ����ʱ������ͼֹͣ����
    DWORD               m_dwDummyID2;                       // ������ΨһID
	DWORD				m_unionID;							// ����ID
	int					m_pLastTargetID;
	bool                bTaskCollect;                       //�Ƿ�����ɼ�
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
