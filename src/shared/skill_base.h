
#ifndef _SKILL_BASE_H_
#define _SKILL_BASE_H_

struct SCreature;
class CSkillManage;
class CActorSkill;
struct SSkillFilterMap;
struct SSkillEffectMap;
/* gao 2009.12.8
为方便后面的使用，将实现放到头文件中来了
*/
enum ESkillFilterType
{
	ESFT_NA,    // 没有参数
	ESFT_OI,    // operate, int
	ESFT_OF,    // operate, float
	ESFT_CI,    // int
	ESFT_CII,   // int, int
};

enum ESkillEffectType
{
	ESET_NA,    // 没有参数
	ESET_I,     // int
	ESET_F,     // float
	ESET_IF,    // int, float
	ESET_II,    // int, int
	ESET_FF,    // float, float
};

enum ESkillValueFun
{
	ESVF_NONE               = 0,    // 没有用
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
	ESVF_S_AttackRate,  // 命中
	ESVF_S_Dodge,       // 躲避
	ESVF_S_AttackRange, // 攻击距离
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
	ESVF_S_AttackRateR,             // 命中
	ESVF_S_DodgeR,                  // 躲避
	ESVF_S_AttackRangeR,            // 攻击距离
	ESVF_S_Critical,                // 必杀率
	ESVF_S_CriticalR,               // 必杀率
	ESVF_S_StateMove,               // 是否可以移动
	ESVF_S_StateDodge,              // 是否可以闪避
	ESVF_S_StateUseSkill,           // 是否可以使用技能
	ESVF_S_StateNAttack,            // 是否可以普通攻击
	ESVF_S_StateResist,             // 增加状态抵抗
	ESVF_S_Hate,                    // 额外的仇恨度
	ESVF_S_HateRate,                // 仇恨比率
	ESVF_S_AddHate,                 // 添加怪物的仇恨
	ESVF_S_SkillCriRate,            // 技能爆击率
	ESVF_S_SkillCriDamageR,          // 技能爆击伤害
	ESVF_S_Resilience,				// 抗暴击
	ESVF_S_Rebirth,					// 复活
	ESVF_S_ExtractHpR,
	ESVF_S_Immuno,                  //免疫  +1 免疫有益 + 2免疫有害  +3免疫所有

	ESVF_S_AddAbsorptionDamage,     // 增加最大伤害吸收值(血盾)
	ESVF_S_Hiding,                  // 隐身，这里要脱离战斗状态，清空怪物目标和被别人点击目标
	ESVF_S_ReboundHurtR,            // 反弹伤害率

	ESVF_S_CriDamage,              // 技能爆击伤害加成
	ESVF_S_calDamage,              // 普通攻击爆击伤害
	ESVF_S_calDamageR,             // 普通攻击爆击倍率，就是爆击伤害倍数
	ESVF_S_ShieldMpR,              // 增加圣巫魔法盾吸收率MP
	ESVF_S_AbsorptionHpR,
	ESVF_S_HPMPConversion,
	ESVF_S_AddCutMaxHP,            //增加或减少最大HP的百分比
    ESVF_S_AddCutMaxMP,            //抽取血值  有个最大量
	ESVF_O_ChangeWeapon,           //切换武器
	ESVF_O_BackHome,               //回程
	ESVF_S_ChangeBody,             //变身
	ESVF_S_MomentMove,             //瞬间移动（闪现）
	ESVF_S_DeBufWater,
	ESVF_S_DeBufFire,
	ESVF_S_DeBufPoison,

	ESVF_S_ResistZXC,              //抵抗爆击
	ESVF_S_ResistSwim,             //抵抗眩晕
	ESVF_S_ResistInertia,          //抵抗迟钝
	ESVF_S_ResistTrance,           //抵抗昏睡
	ESVF_S_ResistBleed,            //抵抗流血

	ESVF_S_ReceiveDamageSkillR,    //接收技能伤害倍率
	ESVF_S_TeamShareDamage,        //生命锁链

	ESVF_S_MonomersCgroup,         //单体变群攻
	ESVF_S_OrdinaryAddEffect,      //普通攻击触发特效
	ESVF_S_Hprelay, 
	ESVF_S_Mprelay,
	ESVF_S_SeriesDamage,           //增加某系列技能的伤害
	ESVF_S_BossConjure,            //BOSS或者NPC的召唤
	ESVF_S_UltimateChange,         //终极变身技能
	//ESVF_S_ConsumeNpc,             //消耗NPC个数
	ESVF_S_SkillhpRecover,          //技能每秒恢复血量
	ESVF_S_SkillmpRecover,         //技能每秒恢复蓝量
	
	ESVF_O_RIDE             = 501,  //
	ESVF_O_UNRIDE,
	ESVF_O_FB_XIULIAN,              // 法宝修炼，客户端会打开法宝修炼界面
	ESVF_O_FB_ZHURU,                // 法宝注入，客户端会打开法宝注入界面
	ESVF_O_AddEffect,
	ESVF_O_ClearUseful,
	ESVF_O_ClearBaleful,
	ESVF_O_ClearAllEffect,
	ESVF_O_ClearEffect,
	ESVF_O_Conjure,                 // 召唤
	ESVF_O_UI_Produce,              // 制作，客户端会打开制作界面
	ESVF_O_UI_Syncretize,           // 融合，客户端会打开融合界面
	ESVF_O_Item_Decompound,         // 道具分解，客户端会进入成道具分解状态
	ESVF_O_MoveParent,              //移动目标
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
// 技能目标
//     指这个技能对于什么目标有效
#define SKILL_TARGET_TYPE_NONE      0       // 没有目标(任何地方使用)
#define SKILL_TARGET_TYPE_ACTOR     1       // 人物目标
#define SKILL_TARGET_TYPE_POSITION  2       // 当前地面
#define SKILL_TARGET_TYPE_BREAK     3       // 当前地面

#define SKILL_TARGET_ENEMY_ACTOR    0x00000001  // 敌人
#define SKILL_TARGET_ENEMY_PET      0x00000002  // 敌人的兽
#define SKILL_TARGET_PLAYER         0x00000004  // 自己
#define SKILL_TARGET_PET            0x00000008  // 自己的兽
#define SKILL_TARGET_FRIEND_ACTOR   0x00000010  // 友军
#define SKILL_TARGET_FRIEND_PET     0x00000020  // 友军的兽
#define SKILL_TARGET_NEUTRAL_ACTOR  0x00000040  // 中立
#define SKILL_TARGET_NEUTRAL_PET    0x00000080  // 中立的兽
#define SKILL_TARGET_CORPSE         0x00000100  // 尸体
#define SKILL_TARGET_POSITION       0x00000200  // 地面坐标
#define SKILL_TARGET_TEAM_MEMBER    0x00000400  // 队伍成员
#define SKILL_TARGET_HERB           0x00001000  // 药草
#define SKILL_TARGET_MINE           0x00002000  // 矿
#define SKILL_TARGET_ITEM_BAG       0x00004000  // 道具袋中的道具
#define SKILL_TARGET_UNION          0x00010000  // 同一诸侯
#define SKILL_TARGET_UNION_x        0x00020000  // 其他诸侯
#define SKILL_TARGET_FACTION        0x00040000  // 同一阵营
#define SKILL_TARGET_FACTION_x      0x00080000  // 其他阵营

//宠物目标
#define SKILL_TARGET_MASTER         0x00100000  // 自己的主人
#define SKILL_TARGET_PETSELF        0x00200000  // 宠物自己


#define SKILL_PRODUCE_WEAPON        1       // 铁匠
#define SKILL_PRODUCE_LEECHDOM      2       // 制药
#define SKILL_PRODUCE_CHARM         3       // 符咒制作
#define SKILL_PRODUCE_EQUIP         4       // 铸甲
#define SKILL_DECOMPOUND_ITEM		5       // 分解

//抵抗技能
const short SKILL_STATE_XUANYUN            =1 ;      //眩晕
const short SKILL_STATE_HUNSHUI            =2 ;      //昏睡             
const short SKILL_STATE_LIUXUE             =3 ;       //流血
const short SKILL_STATE_CHIDUN             =4 ;      //迟钝 
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

#define SKILL_HIDE_SUBID			94       // 隐身类型技能
#define SKILL_LURKER_SUBID			64       // 遁地类型技能

#define SKILL_TARGET_NONE           0  // 没有目标(当前地面)
#define SKILL_TARGET_ENEMY          (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_ENEMY_PET) // 所有的敌人(包括人兽)
#define SKILL_TARGET_FRIEND         (SKILL_TARGET_PLAYER|SKILL_TARGET_PET|SKILL_TARGET_FRIEND_ACTOR|SKILL_TARGET_FRIEND_PET) // 所有的自己人(包括自己，友军和兽)
#define SKILL_TARGET_NEUTRAL        (SKILL_TARGET_NEUTRAL_ACTOR|SKILL_TARGET_NEUTRAL_PET) // 所有的中立者
#define SKILL_TARGET_ALL_ACTOR      (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_PLAYER|SKILL_TARGET_FRIEND_ACTOR|SKILL_TARGET_NEUTRAL_ACTOR) // 所有的人
#define SKILL_TARGET_ALL_PET        (SKILL_TARGET_ENEMY_PET|SKILL_TARGET_PET|SKILL_TARGET_FRIEND_PET|SKILL_TARGET_NEUTRAL_PET) // 所有的兽
#define SKILL_TARGET_OTHERS         (~(SKILL_TARGET_PLAYER|SKILL_TARGET_PET)) // 除了自己的别人
#define SKILL_TARGET_ALL            (SKILL_TARGET_ENEMY_ACTOR|SKILL_TARGET_FRIEND_ACTOR)

// 技能类型
#define SKILL_TYPE_ACTIVE           0       // 主动技能
#define SKILL_TYPE_TRUMP            1       // 法宝技能
#define SKILL_TYPE_PASSIVE          100     // 被动技能


//---------add by Tianh  10.03.01---------//
//技能系列：
#define SKILL_TRAIN_WIZARD_FLYSWORD           1;    //1	蜀山-飞剑
#define SKILL_TRAIN_WIZARD_FLYWHEEL           2;    //2	蜀山-飞轮
#define SKILL_TRAIN_WIZARD_PRIVATE            3;    //3	蜀山-专用
#define SKILL_TRAIN_HUNTER_CORESE             4;    //4	花间-丝绫
#define SKILL_TRAIN_HUNTER_DOUBLELOOP         5;    //5	花间-双环
#define SKILL_TRAIN_HUNTER_PRIVATE            6;    //6	花间-专用
#define SKILL_TRAIN_WARRIOR_HUGEAXE           7;    //7	五台-巨斧
#define SKILL_TRAIN_WARRIOR_HUGEPESTLE        8;    //8	五台-巨杵
#define SKILL_TRAIN_WARRIOR_PRIVATE           9;    //9	五台-专用
#define SKILL_TRAIN_TAOIST_GEM                10;   //10苗疆-法珠
#define SKILL_TRAIN_TAOIST_STAFF              11;   //11苗疆-法杖
#define SKILL_TRAIN_TAOIST_PRIVATE            12;   //12苗疆-专用
#define SKILL_TRAIN_WEAPON                    13;   //13法宝
#define SKILL_TRAIN_SEALEDBOOK                14;   //14天书
#define SKILL_TRAIN_LIFE                      15;   //15生活
#define SKILL_TRAIN_FACE                      16;   //16表情
#define SKILL_TRAIN_BECURRENT                 17;   //17通用
//---------add by Tianh  10.03.01---------//



/*----------------------------------------------------------------------------
- SSkill.
-   单个技能，这个类用来保存单个的一些技能
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

struct SSkillFilter // 技能条件
{
    int                 iCnt;
    SSkillNodeFilter*   pNodes;
    const char*         pFilterStr;
    int Analyse(const char* szStr, CSkillManage* pManage, int iOffset);
    const char* IsSuccess(CActorSkill *pCreature, bool bTest=false, bool bHtml=true, int* pLine=NULL); // 如果检查成功就返回NULL, 否则返回出错提示字符串
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
    unsigned short  wID             ; // 编号
    const char*     szName          ; // 名字

    // 基本属性
    unsigned short  wSubID          ; // 类型编号，用来区分不同的类型
    unsigned short  dwType          ; // 技能类型
    unsigned short  wClass          ; // 技能系列
    unsigned short  wStrSkillSub    ; // 加强技能的类型ID
    int             iLevel          ; // 技能等级
    int             iMaxLevel       ; // 最高等级
    int             iLevelUpExp     ; // 升级经验
    const char*     szSimpleDesc    ; // 简单说明
    const char*     szDescription   ; // 详细说明
    const char*     szIcon          ; // 图标

	int             iSP             ; // 消耗SkillPoint
	int             iSoul         ; // 消耗的灵力
	vector<int>    wManyStrSkillSub ; // 多加强技能的类型ID
	
    // 条件
    SSkillFilter    filterSee       ; // 开始能学习的玩家等级[>=这个等级的玩家就可以看到这个技能是否可以学习]
    SSkillFilter    filterStudy     ; // 学习条件
    SSkillFilter    filterUse       ; // 使用条件
   
    ////////////////////////////////////////////////////
    // 主动技能

    // 施放 
    int             iMP             ; // 消耗MP
	float           ConsumeAnger    ; // 消耗怒气
    unsigned long   dwRTarget       ; // 施放目标
    int             iRMinDest       ; // 施放最小距离
    int             iRMaxDest       ; // 施放最大距离
    int             iRTime          ; // 施放时间
    int             iRRTime         ; // 施放恢复时间
    const char*     szRAction1      ; // 施放动作 单手武器
	const char*		szRAction2      ; // 施放动作 双手武器
    const char*     szREvent        ; // 施放触发事件
    const char*     szREffect1      ; // 施放特效 单手武器
	const char*     szREffect2      ; // 施放特效 双手武器
    const char*     szREfLink       ; // 施放特效Link
    const char*     szRSound        ; // 施放音效
	const char*     szRSoundLoop    ; // 施放的循环音效

	const char*     szWayModel      ; // 武器路径模型
	const char*     szWayName		; // 武器动画名
	int				iAddModel		; // 需增加几个模型显示

    // 飞行 
    unsigned char   bFly            ; // 是否飞行
    unsigned char   cFlyType        ; // 飞行方式
    unsigned short  wFlySpeed       ; // 飞行速度
    const char*     szFlyEffect     ; // 飞行特效

    // 攻击
    unsigned long   dwATarget       ; // 攻击目标
    unsigned char   bAType          ; // 是否范围攻击
    unsigned char   cAMaxObj        ; // 最大目标个数
    unsigned short  wARange         ; // 最大作用范围
    unsigned char   bABad           ; // 是否有害
    unsigned char   bADamage        ; // 是否伤害

	//Tianh 伤害的计算方式0到2
	unsigned char   DamageMode      ; 
    unsigned char   bAlwaysUsable   ; // 永远都可用，就是不受不能使用技能的影响
    unsigned long   dwAHitRate      ; // 爆击率
    unsigned short  wHits;          ; // 工作动作的Hit点数

    // 受击 
    const char*     szUEffect       ; // 受击特效
    const char*     szUEfLink       ; // 受击特效Link

    // 动作
    SSkillAction    actSelfTemp     ; // 自己的临时叠加效果(对加强技能也有效)
    SSkillAction    actTargetTemp   ; // 目标的临时叠加效果(对加强技能也有效)
    SSkillAction    actSelfUse      ; // 使用效果(对加强技能也有效)
    SSkillAction    actTargetUse    ; // 使用效果(对加强技能也有效)

    int             iStateRate      ; // 状态命中加成
    int             iStateType      ; // 状态类型
    SSkillAction    actSelfState    ; // 状态效果(加强型技能会替换)
    SSkillAction    actTargetState  ; // 状态效果(加强型技能会替换)

    ////////////////////////////////////////////////////
    // 被动技能

    SSkillAction    actTemp         ; // 被动技能使用这个变量
    SSkillAction    actOpen         ; // 打开效果
    SSkillAction    actClose        ; // 关闭效果

	int				iIsIntonate     ;//是否需要吟唱
	int				iIntonateTime   ;//吟唱时间
	int				iBreakOdds		;//打断几率
	int				iBeBreakOdds	;//被打断几率
	
	int             isFlashMove     ;  //Tianh 是否瞬间移动
	const char*     szChangModel    ;  // 人物切换模型

	int             effectID        ; //技能对应持续效果的ID
	
	int             IsAddHpSkill    ;  //这个技能是否是加血技能  

	int             ResistState     ; // 抵抗技能类型ID

	int             iStrStateRate   ; // 加强型状态命中
};


// 技能效果
struct SSkillEffect
{
    WORD            wID             ; // 效果ID
    BYTE            cLevel          ; // 等级
    int             cClass          ; // 效果类型
    const char*     szName          ; // 名字
    const char*     szDescription   ; // 效果说明
    const char*     szIcon          ; // 图标
    SSkillFilter    filter          ; // 作用条件
    const char*     szEffect        ; // 特效
    const char*     szEffLink       ; // 特效Link
    BYTE            cTarget         ; // 作用目标
    BYTE            cMaxObj         ; // 最大个数
    WORD            wRange          ; // 最大作用范围
    DWORD           dwATarget       ; // 攻击目标
    BYTE            cAType          ; // 作用类型 0中立效果, 1有益效果, 2有害效果
    BYTE            cOverlay        ; // 同级覆盖 添加效果的时候如果已经存在相同等级的这个效果, 1覆盖 0不覆盖

    SSkillAction    actTemp         ; // 临时叠加效果
    SSkillAction    actOpen         ; // 作用效果
    SSkillAction    actClose        ; // 关闭效果
    SSkillAction    actCycle        ; // 周期效果
    int             iDTimes         ; // 作用次数
    int             iCycle          ; // 作用周期
	int             isCleared       ; // 更换武器是否删除效果

    struct SEffectMutex
    {
        unsigned short* listMutex;      // 互斥，在这个互斥列表中的任意效果和这个效果互斥
        unsigned short* listImmune;     // 免疫，当前效果免疫所有列表中的效果
        unsigned short* listDecline;    // 拒绝，当前这个技能被列表中的任意技能免疫
    }mutex;
};


/*----------------------------------------------------------------------------
- CSkillInstance.
-   技能实例，用于保存一些技能使用时候的临时变量
-   客户端和服务端分别继承这个类用来做不同的计算处理
----------------------------------------------------------------------------*/
class CSkillInstance
{
public:
    virtual ~CSkillInstance();
    static CSkillInstance* GenerateOne(SSkill* pAttr);

    const SSkill*   GetAttr()                       { return m_pAttr; }
    const char*     GetErrInfo()                    { return m_pErrInfo?m_pErrInfo:""; }

    // 当打开或者使用的时候调用这个函数
    bool DoUse(CActorSkill* pReleaser, BYTE cTargetType, DWORD dwTargetID,const float* pPos);

	void ContinueMask( long ResumeTime, long UseTime );

    // 是否能够使用，如果不能使用可以用GetErrInfo取得信息
    bool CanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget,const float* pPos, bool bIgnoreDistance=false);

    // 返回恢复时间的百分比 比如：1表示完全没恢复，0已经恢复，0.5恢复到一半
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
    unsigned long           m_dwResumeTime; // 技能恢复时间，还差多少时间恢复，0表示已经恢复
    unsigned long           m_dwUserTime;   // 技能恢复时间，还差多少时间可以使用，0表示已经可以使用
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
