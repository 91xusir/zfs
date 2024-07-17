
#ifndef _REGION_SKILL_H_
#define _REGION_SKILL_H_

#include "region.h"
#include "skill_manage.h"

/*----------------------------------------------------------------------------
-   class CSvSkill.
----------------------------------------------------------------------------*/

class CSvSkill : public CSkillInstance
{
public:
    CSvSkill(unsigned short wSkillID);
    virtual ~CSvSkill();

    virtual void OnUseBefore(CActorSkill* pReleaser);
    virtual void OnUseAfter(CActorSkill* pReleaser);
	virtual bool OnCanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget, const float* pPos, bool bIgnoreDistance);

};

/*----------------------------------------------------------------------------
-   class CSvSkillCreature.
----------------------------------------------------------------------------*/

class CSvSkillCreature : public CActorSkill
{
public:
    void SkillWriteDamage(ULONG dwTargetID, bool bMiss, int &phyDmg, int eleDmg[MAX_ELEMENT_ATTR], long dead, long bCriDamage);
    void SkillWriteDamageBuffer(CG_CmdPacket* pPacket);
    int m_bSkillWriteDamage;
    unsigned long m_dwLastRunSkill;

    struct SSkillDamage
    {
        ULONG dwTargetID;
        bool  bMiss;
        //bool  bDead;
		char  cDead;
        bool  bCriDamage;
        short phyDmg;
        short eleDmg[MAX_ELEMENT_ATTR];
    };
    std::list<SSkillDamage> m_listSkillDamage;

    /*
    struct SSkillAttackAttr
    {
        DWORD   dwAttackerID;
        short   sAttackSeed;
        short   sAttackSkillID;
        char    cTargetType;
        DWORD   dwEnemyId;
        long    lPos[3];
    }
    SSkillAttackAttr        m_attackAttr;
    //int m_iSkillWriteDamageCnt;
    static CG_CmdPacket s_skillSendCmd;
    */

public:
    CSvSkillCreature()                        { m_bSkillWriteDamage=0; m_dwLastRunSkill=0; }
    virtual ~CSvSkillCreature()               {}

    CRegionCreature* GetParentEx()            { return (CRegionCreature*)m_pParent; }
	void SetParentEx(CRegionCreature* Parent) { m_pParent = Parent;}
    CActorSkill* FindActorSkill (DWORD dwCreatureOID);

    // Get
    virtual short GetMaxHP();
    virtual int   GetLevel();
    virtual int   GetHP();
    virtual int   GetMP();
    virtual float GetEnP();
    virtual int   IsRide();

	virtual int   IsChangeBody();
	virtual int   IsHaveCallNpc();
    virtual int   GetWeaponType();
	virtual int   GetWeaponEquiped();
	virtual int   GetTrumpEquiped();
	virtual int   GetSkillSeries();

    virtual float* GetSelfPos();
    virtual DWORD  GetSelfID();

    virtual short GetModelID();
    virtual short GetCloth();
    virtual char  GetClothLev();
    virtual short GetArmGuard();
    virtual char  GetArmGuardLev();
    virtual short GetShoes();
    virtual char  GetShoesLev();
    virtual short GetWeapon();
    virtual char  GetWeaponLev();
    virtual short GetMaxMP();
    virtual int   GetAttack();      // 取得命中
    virtual int   GetDodge();
    virtual int   GetDamageMin();
    virtual int   GetDamageMax();
    virtual int   GetArmor();
    virtual float GetDamageRate();
    virtual short GetCon();
    virtual short GetStr();
    virtual short GetDex();
    virtual short GetInt();
    virtual short GetHit();
    virtual short GetAttSpeed();
    virtual char  GetMetier();
    virtual char  GetMetierLev();
    virtual char  GetFaction();
    virtual DWORD GetExp();
    virtual long  GetAttrPoint();
	virtual long  GetSkillPoint();
	virtual long  GetSkillSoul();
    virtual short GetAbsorbHp();
    virtual short GetAbsorbMp();
    virtual short GetRateGold();
    virtual short GetRateExp();
    virtual short GetCritical();

    // Set
    virtual void AddHP(int iV,int IsDiS);
    virtual void AddMP(int iV);
	virtual void AddEnP(int iV);
    virtual void AddMaxHP(int iV);
    virtual void AddMaxMP(int iV);
    virtual void AddAddedStr(short v);
    virtual void AddAddedCon(short v);
    virtual void AddAddedDex(short v);
    virtual void AddAddedInt(short v);
    virtual void AddAddedHit(short v);
    virtual void AddResist(short v);
    virtual void AddPhyAr(short sV);        // 15 添加物理防御
    virtual void AddPhyAb(short sV);        // 16 添加物理吸收
    virtual void AddAbsorbHP(short sV);     // 17 添加吸血
    virtual void AddAbsorbMP(short sV);     // 18 添加吸魔
    virtual void AddDamageMin(short sV,int _DamageMode);    // 19 添加最小伤害
    virtual void AddDamageMax(short sV,int _DamageMode);    // 20 添加最大伤害
    virtual void AddSpeedAttack(short sV);  // 21 添加攻击速度
    virtual void AddSpeedMove(short sV);    // 22 添加移动速度
    virtual void AddRateGold(short sV);     // 23 添加获得金钱倍率
    virtual void AddRateExp(short sV);      // 24 添加获得经验倍率
    virtual void AddWaterDm(short sV,int _DamageMode);      // 25 添加水伤害
    virtual void AddFireDm(short sV,int _DamageMode);       // 26 添加火伤害
    virtual void AddPoisonDm(short sV,int _DamageMode);     // 27 添加毒伤害
    virtual void AddWaterAr(short sV);      // 28 添加水防御
    virtual void AddFireAr(short sV);       // 29 添加火防御
    virtual void AddPoisonAr(short sV);     // 30 添加毒防御
    virtual void AddWaterAb(short sV);      // 31 添加水吸收
    virtual void AddFireAb(short sV);       // 32 添加火吸收
    virtual void AddPoisonAb(short sV);     // 33 添加毒吸收
    virtual void AddAttackRate(short sV);   // 34 添加命中
    virtual void AddDodge(short sV);        // 35 添加躲避
    virtual void AddAttackRange(short sV);  // 36 添加攻击距离
    virtual void AddCritical(short sV);

    virtual void AddHPR(float fV);
    virtual void AddMPR(float fV);
    virtual void AddAddedConR(float fV);
    virtual void AddAddedStrR(float fV);
    virtual void AddAddedDexR(float fV);
    virtual void AddAddedIntR(float fV);
    virtual void AddAddedHitR(float fV);
    virtual void AddResistR(float fV);
    virtual void AddMaxHPR(float fV);
    virtual void AddMaxMPR(float fV);
    virtual void AddPhyArR(float fV);  // 15 添加物理防御
    virtual void AddPhyAbR(float fV);  // 16 添加物理吸收
    virtual void AddAbsorbHPR(float fV);  // 17 添加吸血
    virtual void AddAbsorbMPR(float fV);  // 18 添加吸魔
    virtual void AddDamageMinR(float fV);  // 19 添加最小伤害
    virtual void AddDamageMaxR(float fV);  // 20 添加最大伤害
    virtual void AddSpeedAttackR(float fV);  // 21 添加攻击速度
    virtual void AddSpeedMoveR(float fV);  // 22 添加移动速度
    virtual void AddRateGoldR(float fV);  // 23 添加获得金钱倍率
    virtual void AddRateExpR(float fV);  // 24 添加获得经验倍率
    virtual void AddWaterDmR(float fV);  // 25 添加水伤害
    virtual void AddFireDmR(float fV);  // 26 添加火伤害
    virtual void AddPoisonDmR(float fV);  // 27 添加毒伤害
    virtual void AddWaterArR(float fV);  // 28 添加水防御
    virtual void AddFireArR(float fV);  // 29 添加火防御
    virtual void AddPoisonArR(float fV);  // 30 添加毒防御
    virtual void AddWaterAbR(float fV);  // 31 添加水吸收
    virtual void AddFireAbR(float fV);  // 32 添加火吸收
    virtual void AddPoisonAbR(float fV);  // 33 添加毒吸收
    virtual void AddAttackRateR(float fV); // 34 添加命中
    virtual void AddDodgeR(float fV); // 35 添加躲避
    virtual void AddAttackRangeR(float fV); // 36 添加攻击距离
    virtual void AddCriticalR(float fV);
    virtual void StateMove(short sV);    // 78 是否可以移动
    virtual void StateDodge(short sV);    // 79 是否可以闪避
    virtual void StateUseSkill(short sV);    // 80 是否可以使用技能
    virtual void StateNAttack(short sV);    // 81 是否可以普通攻击
    virtual void StateResist(short sV);    // 82 增加状态抵抗
    virtual void Hate(short sV);    // 83 额外的仇恨度
    virtual void HateRate(short sV);    // 84 仇恨比率
    virtual void AddHate(short sV, CDummyActorSkill *pReleaser);    // 85 增加怪物仇恨
    virtual void AddSkillCriRate(short sV, CDummyActorSkill *pReleaser);    // 86 技能爆击率
    virtual void AddSkillCriDamageR(short sV, CDummyActorSkill *pReleaser);    // 87 技能爆击伤害
	virtual void AddResilience(float fV); //90 抗爆击
	virtual void Rebirth(int iHPrate,int iMPrate); //91 复活
	virtual void AddAbsorptionMaxDamage(float sV);//Tianh 增加吸收伤害最大值
	virtual void PlayerHiding(int aV); //隐身
	virtual void AddSkillCriDamage(int sV); // 技能爆击伤害加成
	virtual void AddCriticalDamage(int sV);// 普通攻击爆击伤害
	virtual void AddCriticalDamageR(int sV);// 普通攻击爆击倍率，就是爆击伤害倍数
	virtual void AddShieldMpR(int sV); // 增加圣巫魔法盾吸收率
	virtual void AddReboundHurtR(float sV,int Type);//反弹伤害率
	virtual void AddAbsorptionHpR(int sV); //受到的伤害转化为HP率
	virtual void HPMPConversion(float iHPrate,float iMPrate); //减少HP 增加MP  或者增加HP 减少MP（MP增加 减少的HP*百分率）
	virtual void AddCutMaxHP(float fV);//增加或减少最大HP的百分比
	virtual void AddCutMaxMP(float fV);//增加或减少最大MP的百分比
	virtual void ExtractHpR(int iHPrate,int maxHP);//抽取对方HP百分比  最大iMPrate
	virtual void OrdinaryAddEffect(int probability,int EffectID);//普攻触发特效
	virtual void SeriesDamage(int Series,int damageR);
	virtual void BossConjure(int NpcId,int Num);
	virtual void PlayerImmuno(int sV); //免疫  +1 免疫有益 + 2免疫有害  +3免疫所有
	virtual void ChangeBody(int ModelID);  //变身
	virtual void MomentMove(int sV);  //瞬间移动
	virtual void AddResistZXC(int sV); //抵抗爆击 
	virtual void AddResistSwim(int sV); //抵抗眩晕 
	virtual void AddResistInertia(int sV); //抵抗迟钝 
	virtual void AddResistTrance(int sV); //抵抗昏睡
	virtual void AddResistBleed(int sV); //抵抗流血
	virtual void AddReceiveDamageSkillR(float fV);//增加接收技能伤害倍率
	virtual void TeamShareDamage(int sV);//队友分担伤害
	virtual void MonomersCgroup(int sV);//单体变群体普攻 传入范围
	virtual void Hprelay(int sV,CActorSkill *pTarget);//把自己当前的HP传递给队友
	virtual void Mprelay(int sV,CActorSkill *pTarget);//把自己当前的MP传递给队友
	virtual void UltimateChange(int sV);//终极变身
	virtual void SkillhpRecover(int sV);       //技能每秒恢复血量
	virtual void SkillmpRecover(int sV);     //技能每秒恢复蓝量
	//virtual void ConsumeNpc(int sV);  //消耗召唤NPC的个数
	// Op
    virtual void Ride();
    virtual void Unride();
    virtual void AddEff(short sEffectID, DWORD dwUID);
    virtual void ClearUseful();
	virtual void ChangeWeapon();
	virtual void BackHome();	
	virtual void SpecialClearUseful();
	virtual void TopoClearUseful();
    virtual void ClearBaleful();
    virtual void ClearAllEff();
    virtual void ClearEff(short sEffectID);
	virtual void ClearEffByClass(BYTE cClass);
    virtual void Conjure(int NpcId,int Num);
	virtual void MoveParent(CActorSkill *pReleaser);//把目标瞬间移动到自己旁边

    bool CheckCanUse(SSkill* pSkillAttr, CActorSkill* pReleaser);

	virtual void OnSkillModify();
	virtual void OnEffectModify();

	// 效果
	virtual void OnEffectOpen(SActorEffect* pEffect);
	virtual void OnEffectClose(SActorEffect* pEffect);
	virtual bool OnEffectCycle(SActorEffect* pEffect);

protected:

    virtual void OnUseRangeSkill(SSkill* pSkillAttr, const float* pPos);
	virtual void OnUseRangeSkillAddHp(SSkill* pSkillAttr, const float* pPos,CActorSkill* pReleaser);
    virtual void OnStudySkill(SSkill* pSkillAttr, bool bRefresh);
    virtual void OnClearSkill(bool bFigthSkill, int iRemoveSP);

	//virtual void OnResumeTimeModify(WORD wID,  WORD wResumeTime);

    // 主动技能
    virtual void OnSkillUse(SSkill* pSkill, CActorSkill* pReleaser);
	virtual void OnSkillUseCalculating(SSkill* pSkill, CActorSkill* pReleaser,int iDmgPhy,int iDmgElement[MAX_ELEMENT_ATTR],long bCriDamage,bool bResult);
	virtual void OnSkillAddHP(SSkill* pSkill, CActorSkill* pReleaser);

    // 被动技能
    virtual void OnPassiveOpen(SActorPassive* pPassive);
    virtual void OnPassiveClose(SActorPassive* pPassive);



private:
    float   m_pPos[3];

};

#endif // _REGION_SKILL_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
