
#ifndef _SKILL_BASE_H_
#define _SKILL_BASE_H_

struct SCreature;
class CSkillManage;
class CActorSkill;
struct SSkillFilterMap;
struct SSkillEffectMap;
/* gao 2009.12.8
Ϊ��������ʹ�ã���ʵ�ַŵ�ͷ�ļ�������
*/
enum ESkillFilterType
{
	ESFT_NA,    // û�в���
	ESFT_OI,    // operate, int
	ESFT_OF,    // operate, float
	ESFT_CI,    // int
	ESFT_CII,   // int, int
};

enum ESkillEffectType
{
	ESET_NA,    // û�в���
	ESET_I,     // int
	ESET_F,     // float
	ESET_IF,    // int, float
	ESET_II,    // int, int
	ESET_FF,    // float, float
};

enum ESkillValueFun
{
	ESVF_NONE               = 0,    // û����
	ESVF_G_LEVEL            = 1,    // Get
	ESVF_G_HP,
	ESVF_G_MP,
	ESVF_G_RIDE,
	ESVF_G_WeaponType,
	ESVF_G_WeaponEquiped,
	ESVF_G_TrumpEquiped,
	ESVF_G_ModelID,
	ESVF_G_Cloth,
	ESVF_G_ClothLev,
	ESVF_G_ArmGuard,
	ESVF_G_ArmGuardLev,
	ESVF_G_Shoes,
	ESVF_G_ShoesLev,
	ESVF_G_Weapon,
	ESVF_G_WeaponLev,
	ESVF_G_MaxMP,
	ESVF_G_Attack,
	ESVF_G_Dodge,
	ESVF_G_DamageMin,
	ESVF_G_DamageMax,
	ESVF_G_Armor,
	ESVF_G_DamageRate,
	ESVF_G_Con,
	ESVF_G_Str,
	ESVF_G_Dex,
	ESVF_G_Int,
	ESVF_G_Hit,
	ESVF_G_AttSpeed,
	ESVF_G_Metier,
	ESVF_G_MetierLev,
	ESVF_G_Faction,
	ESVF_G_Exp,
	ESVF_G_AttrPoint,
	ESVF_G_SkillPoint,
	ESVF_G_SkillSoul,
	ESVF_G_AbsorbHp,
	ESVF_G_AbsorbMp,
	ESVF_G_RateGold,
	ESVF_G_RateExp,
	ESVF_G_NeedSkill,
	ESVF_G_NeedClassSP,
	ESVF_G_Critical,
	ESVF_G_MetierNeed,
	ESVF_G_ChangeBody,
	ESVF_G_haveAddSkillSeries,
	ESVF_G_IsHaveCallNpc,


	ESVF_S_HP               = 100, // Set
	ESVF_S_DAMAGE,
	ESVF_S_MP,
	ESVF_S_RANGE,
	ESVF_S_AddedCon,
	ESVF_S_AddedStr,
	ESVF_S_AddedDex,
	ESVF_S_AddedInt,
	ESVF_S_AddedHit,
	ESVF_S_Resist,
	ESVF_S_MaxHP,
	ESVF_S_MaxMP,
	ESVF_S_PhyAr,
	ESVF_S_PhyAb,
	ESVF_S_AbsorbHP,
	ESVF_S_AbsorbMP,
	ESVF_S_DamageMin,
	ESVF_S_DamageMax,
	ESVF_S_SpeedAttack,
	ESVF_S_SpeedMove,
	ESVF_S_RateGold,
	ESVF_S_RateExp,
	ESVF_S_WaterDm,
	ESVF_S_FireDm,
	ESVF_S_PoisonDm,
	ESVF_S_WaterAr,
	ESVF_S_FireAr,
	ESVF_S_PoisonAr,
	ESVF_S_WaterAb,
	ESVF_S_FireAb,
	ESVF_S_PoisonAb,
	ESVF_S_AttackRate,  // ����
	ESVF_S_Dodge,       // ���
	ESVF_S_AttackRange, // ��������
	ESVF_S_HPR,
	ESVF_S_MPR,
	ESVF_S_AddedConR,
	ESVF_S_AddedStrR,
	ESVF_S_AddedDexR,
	ESVF_S_AddedIntR,
	ESVF_S_AddedHitR,
	ESVF_S_ResistR,
	ESVF_S_MaxHPR,
	ESVF_S_MaxMPR,
	ESVF_S_PhyArR,
	ESVF_S_PhyAbR,
	ESVF_S_AbsorbHPR,
	ESVF_S_AbsorbMPR,
	ESVF_S_DamageMinR,
	ESVF_S_DamageMaxR,
	ESVF_S_SpeedAttackR,
	ESVF_S_SpeedMoveR,
	ESVF_S_RateGoldR,
	ESVF_S_RateExpR,
	ESVF_S_WaterDmR,
	ESVF_S_FireDmR,
	ESVF_S_PoisonDmR,
	ESVF_S_WaterArR,
	ESVF_S_FireArR,
	ESVF_S_PoisonArR,
	ESVF_S_WaterAbR,
	ESVF_S_FireAbR,
	ESVF_S_PoisonAbR,
	ESVF_S_AttackRateR,             // ����
	ESVF_S_DodgeR,                  // ���
	ESVF_S_AttackRangeR,            // ��������
	ESVF_S_Critical,                // ��ɱ��
	ESVF_S_CriticalR,               // ��ɱ��
	ESVF_S_StateMove,               // �Ƿ�����ƶ�
	ESVF_S_StateDodge,              // �Ƿ��������
	ESVF_S_StateUseSkill,           // �Ƿ����ʹ�ü���
	ESVF_S_StateNAttack,            // �Ƿ������ͨ����
	ESVF_S_StateResist,             // ����״̬�ֿ�
	ESVF_S_Hate,                    // ����ĳ�޶�
	ESVF_S_HateRate,                // ��ޱ���
	ESVF_S_AddHate,                 // ��ӹ���ĳ��
	ESVF_S_SkillCriRate,            // ���ܱ�����
	ESVF_S_SkillCriDamageR,          // ���ܱ����˺�
	ESVF_S_Resilience,				// ������
	ESVF_S_Rebirth,					// ����
	ESVF_S_ExtractHpR,
	ESVF_S_Immuno,                  //����  +1 �������� + 2�����к�  +3��������

	ESVF_S_AddAbsorptionDamage,     // ��������˺�����ֵ(Ѫ��)
	ESVF_S_Hiding,                  // ��������Ҫ����ս��״̬����չ���Ŀ��ͱ����˵��Ŀ��
	ESVF_S_ReboundHurtR,            // �����˺���

	ESVF_S_CriDamage,              // ���ܱ����˺��ӳ�
	ESVF_S_calDamage,              // ��ͨ���������˺�
	ESVF_S_calDamageR,             // ��ͨ�����������ʣ����Ǳ����˺�����
	ESVF_S_ShieldMpR,              // ����ʥ��ħ����������MP
	ESVF_S_AbsorptionHpR,
	ESVF_S_HPMPConversion,
	ESVF_S_AddCutMaxHP,            //���ӻ�������HP�İٷֱ�
    ESVF_S_AddCutMaxMP,            //��ȡѪֵ  �и������
	ESVF_O_ChangeWeapon,           //�л�����
	ESVF_O_BackHome,               //�س�
	ESVF_S_ChangeBody,             //����
	ESVF_S_MomentMove,             //˲���ƶ������֣�
	ESVF_S_DeBufWater,
	ESVF_S_DeBufFire,
	ESVF_S_DeBufPoison,

	ESVF_S_ResistZXC,              //�ֿ�����
	ESVF_S_ResistSwim,             //�ֿ�ѣ��
	ESVF_S_ResistInertia,          //�ֿ��ٶ�
	ESVF_S_ResistTrance,           //�ֿ���˯
	ESVF_S_ResistBleed,            //�ֿ���Ѫ

	ESVF_S_ReceiveDamageSkillR,    //���ռ����˺�����
	ESVF_S_TeamShareDamage,        //��������

	ESVF_S_MonomersCgroup,         //�����Ⱥ��
	ESVF_S_OrdinaryAddEffect,      //��ͨ����������Ч
	ESVF_S_Hprelay, 
	ESVF_S_Mprelay,
	ESVF_S_SeriesDamage,           //����ĳϵ�м��ܵ��˺�
	ESVF_S_BossConjure,            //BOSS����NPC���ٻ�
	ESVF_S_UltimateChange,         //�ռ�������
	//ESVF_S_ConsumeNpc,             //����NPC����
	ESVF_S_SkillhpRecover,          //����ÿ��ָ�Ѫ��
	ESVF_S_SkillmpRecover,         //����ÿ��ָ�����
	
	ESVF_O_RIDE             = 501,  //
	ESVF_O_UNRIDE,
	ESVF_O_FB_XIULIAN,              // �����������ͻ��˻�򿪷�����������
	ESVF_O_FB_ZHURU,                // ����ע�룬�ͻ��˻�򿪷���ע�����
	ESVF_O_AddEffect,
	ESVF_O_ClearUseful,
	ESVF_O_ClearBaleful,
	ESVF_O_ClearAllEffect,
	ESVF_O_ClearEffect,
	ESVF_O_Conjure,                 // �ٻ�
	ESVF_O_UI_Produce,              // �������ͻ��˻����������
	ESVF_O_UI_Syncretize,           // �ںϣ��ͻ��˻���ںϽ���
	ESVF_O_Item_Decompound,         // ���߷ֽ⣬�ͻ��˻����ɵ��߷ֽ�״̬
	ESVF_O_MoveParent,              //�ƶ�Ŀ��
};

struct SSkillFilterMap
{
	ESkillFilterType    eType;
	ESkillValueFun      eFun;
	char const*         szID;
	char const*         szMsg;
	char const*			szInfo;
};

struct SSkillEffectMap
{
	ESkillEffectType    eType;
	ESkillValueFun      eFun;
	char const*			szID;
};

struct SSkillContinueMask
{
	long wSkillClass;
	long useTime;
	long resumeTime;
};

/* end */
// ����Ŀ��
//     ָ������ܶ���ʲôĿ����Ч
#define SKILL_TARGET_TYPE_NONE      0       // û��Ŀ��(�κεط�ʹ��)
#define SKILL_TARGET_TYPE_ACTOR     1       // ����Ŀ��
#define SKILL_TARGET_TYPE_POSITION  2       // ��ǰ����
#define SKILL_TARGET_TYPE_BREAK     3       // ��ǰ����

#define SKILL_TARGET_ENEMY_ACTOR    0x00000001  // ����
#define SKILL_TARGET_ENEMY_PET      0x00000002  // ���˵���
#define SKILL_TARGET_PLAYER         0x00000004  // �Լ�
#define SKILL_TARGET_PET            0x00000008  // �Լ�����
#define SKILL_TARGET_FRIEND_ACTOR   0x00000010  // �Ѿ�
#define SKILL_TARGET_FRIEND_PET     0x00000020  // �Ѿ�����
#define SKILL_TARGET_NEUTRAL_ACTOR  0x00000040  // ����
#define SKILL_TARGET_NEUTRAL_PET    0x00000080  // ��������
#define SKILL_TARGET_CORPSE         0x00000100  // ʬ��
#define SKILL_TARGET_POSITION       0x00000200  // ��������
#define SKILL_TARGET_TEAM_MEMBER    0x00000400  // �����Ա
#define SKILL_TARGET_HERB           0x00001000  // ҩ��
#define SKILL_TARGET_MINE           0x00002000  // ��
#define SKILL_TARGET_ITEM_BAG       0x00004000  // ���ߴ��еĵ���
#define SKILL_TARGET_UNION          0x00010000  // ͬһ���
#define SKILL_TARGET_UNION_x        0x00020000  // �������
#define SKILL_TARGET_FACTION        0x00040000  // ͬһ��Ӫ
#define SKILL_TARGET_FACTION_x      0x00080000  // ������Ӫ

//����Ŀ��
#define SKILL_TARGET_MASTER         0x00100000  // �Լ�������
#define SKILL_TARGET_PETSELF        0x00200000  // �����Լ�


#define SKILL_PRODUCE_WEAPON        1       // ����
#define SKILL_PRODUCE_LEECHDOM      2       // ��ҩ
#define SKILL_PRODUCE_CHARM         3       // ��������
#define SKILL_PRODUCE_EQUIP         4       // ����
#define SKILL_DECOMPOUND_ITEM		5       // �ֽ�

//�ֿ�����
const short SKILL_STATE_XUANYUN            =1 ;      //ѣ��
const short SKILL_STATE_HUNSHUI            =2 ;      //��˯             
const short SKILL_STATE_LIUXUE             =3 ;       //��Ѫ
const short SKILL_STATE_CHIDUN             =4 ;      //�ٶ� 
// life skill
const short SKILL_DIG_ORZ_ID				= 1;
const short SKILL_DIG_DRUG_ID				= 16;
const short SKILL_DIG_SKIN_ID				= 31;
const short SKILL_MAKE_WEAPON_ID			= 46;
const short SKILL_MAKE_EQUIP_ID				= 91;
const short SKILL_MAKE_LEECHDOM_ID			= 61;

const short SKILL_DIG_ORZ_SUBID				= 581;
const short SKILL_DIG_DRUG_SUBID			= 582;
const short SKILL_DIG_SKIN_SUBID			= 654;
const short SKILL_MAKE_WEAPON_SUBID			= 583;
const short SKILL_MAKE_EQUIP_SUBID			= 588;
const short SKILL_MAKE_LEECHDOM_SUBID		= 584;

#define SKILL_HIDE_SUBID			94       // �������ͼ���
#define SKILL_LURKER_SUBID			64       // �ݵ����ͼ���

#define SKILL_TARGET_NONE           0  // û��Ŀ��(��ǰ����)
#define SKILL_TARGET_ENEMY          (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_ENEMY_PET) // ���еĵ���(��������)
#define SKILL_TARGET_FRIEND         (SKILL_TARGET_PLAYER|SKILL_TARGET_PET|SKILL_TARGET_FRIEND_ACTOR|SKILL_TARGET_FRIEND_PET) // ���е��Լ���(�����Լ����Ѿ�����)
#define SKILL_TARGET_NEUTRAL        (SKILL_TARGET_NEUTRAL_ACTOR|SKILL_TARGET_NEUTRAL_PET) // ���е�������
#define SKILL_TARGET_ALL_ACTOR      (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_PLAYER|SKILL_TARGET_FRIEND_ACTOR|SKILL_TARGET_NEUTRAL_ACTOR) // ���е���
#define SKILL_TARGET_ALL_PET        (SKILL_TARGET_ENEMY_PET|SKILL_TARGET_PET|SKILL_TARGET_FRIEND_PET|SKILL_TARGET_NEUTRAL_PET) // ���е���
#define SKILL_TARGET_OTHERS         (~(SKILL_TARGET_PLAYER|SKILL_TARGET_PET)) // �����Լ��ı���
#define SKILL_TARGET_ALL            (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_FRIEND_ACTOR)

// ��������
#define SKILL_TYPE_ACTIVE           0       // ��������
#define SKILL_TYPE_TRUMP            1       // ��������
#define SKILL_TYPE_PASSIVE          100     // ��������


//---------add by Tianh  10.03.01---------//
//����ϵ�У�
#define SKILL_TRAIN_WIZARD_FLYSWORD           1;    //1	��ɽ-�ɽ�
#define SKILL_TRAIN_WIZARD_FLYWHEEL           2;    //2	��ɽ-����
#define SKILL_TRAIN_WIZARD_PRIVATE            3;    //3	��ɽ-ר��
#define SKILL_TRAIN_HUNTER_CORESE             4;    //4	����-˿�
#define SKILL_TRAIN_HUNTER_DOUBLELOOP         5;    //5	����-˫��
#define SKILL_TRAIN_HUNTER_PRIVATE            6;    //6	����-ר��
#define SKILL_TRAIN_WARRIOR_HUGEAXE           7;    //7	��̨-�޸�
#define SKILL_TRAIN_WARRIOR_HUGEPESTLE        8;    //8	��̨-����
#define SKILL_TRAIN_WARRIOR_PRIVATE           9;    //9	��̨-ר��
#define SKILL_TRAIN_TAOIST_GEM                10;   //10�置-����
#define SKILL_TRAIN_TAOIST_STAFF              11;   //11�置-����
#define SKILL_TRAIN_TAOIST_PRIVATE            12;   //12�置-ר��
#define SKILL_TRAIN_WEAPON                    13;   //13����
#define SKILL_TRAIN_SEALEDBOOK                14;   //14����
#define SKILL_TRAIN_LIFE                      15;   //15����
#define SKILL_TRAIN_FACE                      16;   //16����
#define SKILL_TRAIN_BECURRENT                 17;   //17ͨ��
//---------add by Tianh  10.03.01---------//



/*----------------------------------------------------------------------------
- SSkill.
-   �������ܣ�������������浥����һЩ����
----------------------------------------------------------------------------*/
union SkillNumber
{
    int     i;
    float   f;
};

struct StrengthenSkill
{
	SkillNumber  Param1;
};

struct SSkillNodeFilter
{
    SSkillFilterMap*    pMap;
    SkillNumber         Param1;
    SkillNumber         Param2;
};

struct SSkillFilter // ��������
{
    int                 iCnt;
    SSkillNodeFilter*   pNodes;
    const char*         pFilterStr;
    int Analyse(const char* szStr, CSkillManage* pManage, int iOffset);
    const char* IsSuccess(CActorSkill *pCreature, bool bTest=false, bool bHtml=true, int* pLine=NULL); // ������ɹ��ͷ���NULL, ���򷵻س�����ʾ�ַ���
    void SaveToString(char* szString, int length);
};

struct SSkillActionNode
{
    SSkillEffectMap*    pMap;
    SkillNumber         Param1;
    SkillNumber         Param2;
};

struct SSkillAction
{
    int                 iCnt;
    SSkillActionNode*   pNodes;
    const char*         pEffectStr;
    int Analyse(const char* szStr, CSkillManage* pManage, int iOffset);
    void Apply(CActorSkill *pReleaser, CActorSkill *pTarget, DWORD dwUID, bool bReverse=false,int _DamageMode = 0);
};

struct SSkill
{
    unsigned short  wID             ; // ���
    const char*     szName          ; // ����

    // ��������
    unsigned short  wSubID          ; // ���ͱ�ţ��������ֲ�ͬ������
    unsigned short  dwType          ; // ��������
    unsigned short  wClass          ; // ����ϵ��
    unsigned short  wStrSkillSub    ; // ��ǿ���ܵ�����ID
    int             iLevel          ; // ���ܵȼ�
    int             iMaxLevel       ; // ��ߵȼ�
    int             iLevelUpExp     ; // ��������
    const char*     szSimpleDesc    ; // ��˵��
    const char*     szDescription   ; // ��ϸ˵��
    const char*     szIcon          ; // ͼ��

	int             iSP             ; // ����SkillPoint
	int             iSoul         ; // ���ĵ�����
	vector<int>    wManyStrSkillSub ; // ���ǿ���ܵ�����ID
	
    // ����
    SSkillFilter    filterSee       ; // ��ʼ��ѧϰ����ҵȼ�[>=����ȼ�����ҾͿ��Կ�����������Ƿ����ѧϰ]
    SSkillFilter    filterStudy     ; // ѧϰ����
    SSkillFilter    filterUse       ; // ʹ������
   
    ////////////////////////////////////////////////////
    // ��������

    // ʩ�� 
    int             iMP             ; // ����MP
	float           ConsumeAnger    ; // ����ŭ��
    unsigned long   dwRTarget       ; // ʩ��Ŀ��
    int             iRMinDest       ; // ʩ����С����
    int             iRMaxDest       ; // ʩ��������
    int             iRTime          ; // ʩ��ʱ��
    int             iRRTime         ; // ʩ�Żָ�ʱ��
    const char*     szRAction1      ; // ʩ�Ŷ��� ��������
	const char*		szRAction2      ; // ʩ�Ŷ��� ˫������
    const char*     szREvent        ; // ʩ�Ŵ����¼�
    const char*     szREffect1      ; // ʩ����Ч ��������
	const char*     szREffect2      ; // ʩ����Ч ˫������
    const char*     szREfLink       ; // ʩ����ЧLink
    const char*     szRSound        ; // ʩ����Ч
	const char*     szRSoundLoop    ; // ʩ�ŵ�ѭ����Ч

	const char*     szWayModel      ; // ����·��ģ��
	const char*     szWayName		; // ����������
	int				iAddModel		; // �����Ӽ���ģ����ʾ

    // ���� 
    unsigned char   bFly            ; // �Ƿ����
    unsigned char   cFlyType        ; // ���з�ʽ
    unsigned short  wFlySpeed       ; // �����ٶ�
    const char*     szFlyEffect     ; // ������Ч

    // ����
    unsigned long   dwATarget       ; // ����Ŀ��
    unsigned char   bAType          ; // �Ƿ�Χ����
    unsigned char   cAMaxObj        ; // ���Ŀ�����
    unsigned short  wARange         ; // ������÷�Χ
    unsigned char   bABad           ; // �Ƿ��к�
    unsigned char   bADamage        ; // �Ƿ��˺�

	//Tianh �˺��ļ��㷽ʽ0��2
	unsigned char   DamageMode      ; 
    unsigned char   bAlwaysUsable   ; // ��Զ�����ã����ǲ��ܲ���ʹ�ü��ܵ�Ӱ��
    unsigned long   dwAHitRate      ; // ������
    unsigned short  wHits;          ; // ����������Hit����

    // �ܻ� 
    const char*     szUEffect       ; // �ܻ���Ч
    const char*     szUEfLink       ; // �ܻ���ЧLink

    // ����
    SSkillAction    actSelfTemp     ; // �Լ�����ʱ����Ч��(�Լ�ǿ����Ҳ��Ч)
    SSkillAction    actTargetTemp   ; // Ŀ�����ʱ����Ч��(�Լ�ǿ����Ҳ��Ч)
    SSkillAction    actSelfUse      ; // ʹ��Ч��(�Լ�ǿ����Ҳ��Ч)
    SSkillAction    actTargetUse    ; // ʹ��Ч��(�Լ�ǿ����Ҳ��Ч)

    int             iStateRate      ; // ״̬���мӳ�
    int             iStateType      ; // ״̬����
    SSkillAction    actSelfState    ; // ״̬Ч��(��ǿ�ͼ��ܻ��滻)
    SSkillAction    actTargetState  ; // ״̬Ч��(��ǿ�ͼ��ܻ��滻)

    ////////////////////////////////////////////////////
    // ��������

    SSkillAction    actTemp         ; // ��������ʹ���������
    SSkillAction    actOpen         ; // ��Ч��
    SSkillAction    actClose        ; // �ر�Ч��

	int				iIsIntonate     ;//�Ƿ���Ҫ����
	int				iIntonateTime   ;//����ʱ��
	int				iBreakOdds		;//��ϼ���
	int				iBeBreakOdds	;//����ϼ���
	
	int             isFlashMove     ;  //Tianh �Ƿ�˲���ƶ�
	const char*     szChangModel    ;  // �����л�ģ��

	int             effectID        ; //���ܶ�Ӧ����Ч����ID
	
	int             IsAddHpSkill    ;  //��������Ƿ��Ǽ�Ѫ����  

	int             ResistState     ; // �ֿ���������ID

	int             iStrStateRate   ; // ��ǿ��״̬����
};


// ����Ч��
struct SSkillEffect
{
    WORD            wID             ; // Ч��ID
    BYTE            cLevel          ; // �ȼ�
    int             cClass          ; // Ч������
    const char*     szName          ; // ����
    const char*     szDescription   ; // Ч��˵��
    const char*     szIcon          ; // ͼ��
    SSkillFilter    filter          ; // ��������
    const char*     szEffect        ; // ��Ч
    const char*     szEffLink       ; // ��ЧLink
    BYTE            cTarget         ; // ����Ŀ��
    BYTE            cMaxObj         ; // ������
    WORD            wRange          ; // ������÷�Χ
    DWORD           dwATarget       ; // ����Ŀ��
    BYTE            cAType          ; // �������� 0����Ч��, 1����Ч��, 2�к�Ч��
    BYTE            cOverlay        ; // ͬ������ ���Ч����ʱ������Ѿ�������ͬ�ȼ������Ч��, 1���� 0������

    SSkillAction    actTemp         ; // ��ʱ����Ч��
    SSkillAction    actOpen         ; // ����Ч��
    SSkillAction    actClose        ; // �ر�Ч��
    SSkillAction    actCycle        ; // ����Ч��
    int             iDTimes         ; // ���ô���
    int             iCycle          ; // ��������
	int             isCleared       ; // ���������Ƿ�ɾ��Ч��

    struct SEffectMutex
    {
        unsigned short* listMutex;      // ���⣬����������б��е�����Ч�������Ч������
        unsigned short* listImmune;     // ���ߣ���ǰЧ�����������б��е�Ч��
        unsigned short* listDecline;    // �ܾ�����ǰ������ܱ��б��е����⼼������
    }mutex;
};


/*----------------------------------------------------------------------------
- CSkillInstance.
-   ����ʵ�������ڱ���һЩ����ʹ��ʱ�����ʱ����
-   �ͻ��˺ͷ���˷ֱ�̳��������������ͬ�ļ��㴦��
----------------------------------------------------------------------------*/
class CSkillInstance
{
public:
    virtual ~CSkillInstance();
    static CSkillInstance* GenerateOne(SSkill* pAttr);

    const SSkill*   GetAttr()                       { return m_pAttr; }
    const char*     GetErrInfo()                    { return m_pErrInfo?m_pErrInfo:""; }

    // ���򿪻���ʹ�õ�ʱ������������
    bool DoUse(CActorSkill* pReleaser, BYTE cTargetType, DWORD dwTargetID,const float* pPos);

	void ContinueMask( long ResumeTime, long UseTime );

    // �Ƿ��ܹ�ʹ�ã��������ʹ�ÿ�����GetErrInfoȡ����Ϣ
    bool CanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget,const float* pPos, bool bIgnoreDistance=false);

    // ���ػָ�ʱ��İٷֱ� ���磺1��ʾ��ȫû�ָ���0�Ѿ��ָ���0.5�ָ���һ��
    float GetResumeRate()
	{return (m_dwMaxResumeTime)?((float)m_dwResumeTime / m_dwMaxResumeTime):0.f;}

    void OnRun(unsigned long dwMillisecond,CActorSkill* pReleaser);

protected:
    CSkillInstance(unsigned short wSkillID);
    virtual void OnSkillRun(unsigned long dwMillisecond,CActorSkill* pReleaser){};
	virtual bool OnCanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget,const float* pPos, bool bIgnoreDistance){return true;}
public:
    virtual void OnUseBefore(CActorSkill* pReleaser){};
    virtual void OnUseAfter(CActorSkill* pReleaser){};

public:
    SSkill*                 m_pAttr;
    unsigned long           m_dwResumeTime; // ���ָܻ�ʱ�䣬�������ʱ��ָ���0��ʾ�Ѿ��ָ�
    unsigned long           m_dwUserTime;   // ���ָܻ�ʱ�䣬�������ʱ�����ʹ�ã�0��ʾ�Ѿ�����ʹ��
    unsigned long           m_dwMaxResumeTime;
    unsigned long           m_dwMaxUserTime;
    unsigned short          m_wExp;
    const char*             m_pErrInfo;
};


extern G_MEMDEF(g_szSkillErrMsg, 100)


#endif // _SKILL_BASE_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
