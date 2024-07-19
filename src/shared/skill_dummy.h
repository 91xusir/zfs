

class CDummyActorSkill
{
public:
    // Get
    virtual short GetMaxHP()=0;
    virtual int   GetLevel()=0;
    virtual int   GetHP()=0;
    virtual int   GetMP()=0;
	virtual float GetEnP()=0;
    virtual int   IsRide()=0; // 0自由，1骑兽，2被骑
	virtual int   IsChangeBody()=0;
	virtual int   IsHaveCallNpc()=0;
    virtual int   GetWeaponType()=0;
	virtual int   GetWeaponEquiped()=0;
	virtual int   GetTrumpEquiped()=0;
	virtual int   GetSkillSeries()=0;

    virtual float* GetSelfPos()=0;
    virtual DWORD  GetSelfID()=0;

    virtual short GetModelID()=0;
    virtual short GetCloth()=0;
    virtual char  GetClothLev()=0;
    virtual short GetArmGuard()=0;
    virtual char  GetArmGuardLev()=0;
    virtual short GetShoes()=0;
    virtual char  GetShoesLev()=0;
    virtual short GetWeapon()=0;
    virtual char  GetWeaponLev()=0;
    virtual short GetMaxMP()=0;
    virtual int   GetAttack()=0;    // 取得命中
    virtual int   GetDodge()=0;
    virtual int   GetDamageMin()=0;
    virtual int   GetDamageMax()=0;
    virtual int   GetArmor()=0;
    virtual float GetDamageRate()=0;
    virtual short GetCon()=0;
    virtual short GetStr()=0;
    virtual short GetDex()=0;
    virtual short GetInt()=0;
    virtual short GetHit()=0;
    virtual short GetAttSpeed()=0;
    virtual char  GetMetier()=0;
    virtual char  GetMetierLev()=0;
    virtual char  GetFaction()=0;
    virtual DWORD GetExp()=0;
    virtual long  GetAttrPoint()=0;
	virtual long  GetSkillPoint()=0;
	virtual long  GetSkillSoul()=0;
    virtual short GetAbsorbHp()=0;
    virtual short GetAbsorbMp()=0;
    virtual short GetRateGold()=0;
    virtual short GetRateExp()=0;
    virtual short GetCritical()=0;

    // Set
    virtual void AddHP(int iV,int IsDiS){};
    virtual void AddMP(int iV){};
	virtual void AddEnP(int iV){};
    virtual void AddMaxHP(int iV){};
    virtual void AddMaxMP(int iV){};
    virtual void AddPhyAr(short sV){};       // 15 添加物理防御
    virtual void AddPhyAb(short sV){};       // 16 添加物理吸收
    virtual void AddAbsorbHP(short sV){};    // 17 添加吸血
    virtual void AddAbsorbMP(short sV){};    // 18 添加吸魔
    virtual void AddDamageMin(short sV,int _DamageMode){};   // 19 添加最小伤害
    virtual void AddDamageMax(short sV,int _DamageMode){};   // 20 添加最大伤害
    virtual void AddSpeedAttack(short sV){}; // 21 添加攻击速度
    virtual void AddSpeedMove(short sV){};   // 22 添加移动速度
    virtual void AddRateGold(short sV){};    // 23 添加获得金钱倍率
    virtual void AddRateExp(short sV){};     // 24 添加获得经验倍率
    virtual void AddWaterAr(short sV){};     // 28 添加水防御
    virtual void AddFireAr(short sV){};      // 29 添加火防御
    virtual void AddPoisonAr(short sV){};    // 30 添加毒防御
    virtual void AddWaterAb(short sV){};     // 31 添加水吸收
    virtual void AddFireAb(short sV){};      // 32 添加火吸收
    virtual void AddPoisonAb(short sV){};    // 33 添加毒吸收
    virtual void AddAttackRate(short sV){};  // 34 添加命中
    virtual void AddDodge(short sV){};       // 35 添加躲避
    virtual void AddAttackRange(short sV){}; // 36 添加攻击距离
    virtual void AddCritical(short sV){};

    virtual void AddHPR(float fV){}
    virtual void AddMPR(float fV){}
    virtual void AddAddedConR(float fV){}
    virtual void AddAddedStrR(float fV){}
    virtual void AddAddedDexR(float fV){}
    virtual void AddAddedIntR(float fV){}
    virtual void AddAddedHitR(float fV){}
    virtual void AddResistR(float fV){}
    virtual void AddMaxHPR(float fV){}
    virtual void AddMaxMPR(float fV){}
    virtual void AddPhyArR(float fV){}  // 15 添加物理防御
    virtual void AddPhyAbR(float fV){}  // 16 添加物理吸收
    virtual void AddAbsorbHPR(float fV){}  // 17 添加吸血
    virtual void AddAbsorbMPR(float fV){}  // 18 添加吸魔
    virtual void AddDamageMinR(float fV){}  // 19 添加最小伤害
    virtual void AddDamageMaxR(float fV){}  // 20 添加最大伤害
    virtual void AddSpeedAttackR(float fV){}  // 21 添加攻击速度
    virtual void AddSpeedMoveR(float fV){}  // 22 添加移动速度
    virtual void AddRateGoldR(float fV){}  // 23 添加获得金钱倍率
    virtual void AddRateExpR(float fV){}  // 24 添加获得经验倍率
    virtual void AddWaterDmR(float fV){}  // 25 添加水伤害
    virtual void AddFireDmR(float fV){}  // 26 添加火伤害
    virtual void AddPoisonDmR(float fV){}  // 27 添加毒伤害
    virtual void AddWaterArR(float fV){}  // 28 添加水防御
    virtual void AddFireArR(float fV){}  // 29 添加火防御
    virtual void AddPoisonArR(float fV){}  // 30 添加毒防御
    virtual void AddWaterAbR(float fV){}  // 31 添加水吸收
    virtual void AddFireAbR(float fV){}  // 32 添加火吸收
    virtual void AddPoisonAbR(float fV){}  // 33 添加毒吸收
    virtual void AddAttackRateR(float fV){} // 34 添加命中
    virtual void AddDodgeR(float fV){} // 35 添加躲避
    virtual void AddAttackRangeR(float fV){} // 36 添加攻击距离
    virtual void AddCriticalR(float fV){}
    virtual void AddWaterDm(short sV,int _DamageMode){}     // 25 添加水伤害
    virtual void AddFireDm(short sV,int _DamageMode){}      // 26 添加火伤害
    virtual void AddPoisonDm(short sV,int _DamageMode){}    // 27 添加毒伤害
    virtual void StateMove(short sV){}    // 78 是否可以移动
    virtual void StateDodge(short sV){}    // 79 是否可以闪避
    virtual void StateUseSkill(short sV){}    // 80 是否可以使用技能
    virtual void StateNAttack(short sV){}    // 81 是否可以普通攻击
    virtual void StateResist(short sV){}    // 82 增加状态抵抗
    virtual void Hate(short sV){}    // 83 额外的仇恨度
    virtual void HateRate(short sV){}    // 84 仇恨比率
    virtual void AddHate(short sV, CDummyActorSkill *pReleaser){}    // 85 增加怪物仇恨
    virtual void AddSkillCriRate(short sV, CDummyActorSkill *pReleaser){}    // 86 技能爆击率
    virtual void AddSkillCriDamageR(short sV, CDummyActorSkill *pReleaser){}    // 87 技能爆击伤害率
	virtual void AddResilience(float fV){} //90 抗爆击
	virtual void Rebirth(int iHPrate,int iMPrate){} //91 复活
	virtual void AddAbsorptionMaxDamage(float sV){}//Tianh 增加吸收伤害最大值
	virtual void PlayerHiding(int aV){}
	virtual void AddSkillCriDamage(int sV){} // 技能爆击伤害加成
	virtual void AddCriticalDamage(int sV){}// 普通攻击爆击伤害
	virtual void AddCriticalDamageR(int sV){}// 普通攻击爆击倍率，就是爆击伤害倍数
	virtual void AddShieldMpR(int sV){} // 增加圣巫魔法盾吸收率
	virtual void AddReboundHurtR(float sV,int Type){}//反弹伤害率
	virtual void AddAbsorptionHpR(int sV){} //受到的伤害转化为HP率
	virtual void HPMPConversion(float iHPrate,float iMPrate){} //102减少HP 增加MP  或者增加HP 减少MP
	virtual void AddCutMaxHP(float fV){}//增加或减少最大HP的百分比
	virtual void AddCutMaxMP(float fV){}//增加或减少最大MP的百分比
	virtual void ExtractHpR(int iHPrate,int maxHP){}//抽取对方HP百分比  最大iMPrate
	virtual void OrdinaryAddEffect(int probability,int EffectID){}
	virtual void SeriesDamage(int Series,int damageR){}//增加技能系列伤害
	virtual void BossConjure(int NpcId,int Num){}
	virtual void PlayerImmuno(int sV){} //免疫  +1 免疫有益 + 2免疫有害  +3免疫所有
	virtual void ChangeBody(int ModelID){} //变身
	virtual void MomentMove(int sV){}  //瞬间移动
	virtual void AddResistZXC(int sV){} //抵抗爆击 
	virtual void AddResistSwim(int sV){} //抵抗眩晕 
	virtual void AddResistInertia(int sV){} //抵抗迟钝 
	virtual void AddResistTrance(int sV){} //抵抗昏睡
	virtual void AddResistBleed(int sV){} //抵抗流血
	virtual void AddReceiveDamageSkillR(float fV){}//增加接收技能伤害倍率
	virtual void TeamShareDamage(int sV){}//队友分担伤害
	virtual void MonomersCgroup(int sV){}//单体变群体普攻 传入范围
	virtual void Hprelay(int sV,CActorSkill *pTarget){}//把自己当前的HP传递给队友
	virtual void Mprelay(int sV,CActorSkill *pTarget){}//把自己当前的MP传递给队友
	virtual void UltimateChange(int sV){}//终极变身
	virtual void SkillhpRecover(int sV){}       //技能每秒恢复血量
	virtual void SkillmpRecover(int sV){}    //技能每秒恢复蓝量
	//virtual void ConsumeNpc(int sV){}  //消耗召唤NPC的个数

    // 直接伤害，只用在actUse的Apply
    void SetImmDamage(short sV)         { m_sLastDamage = sV; }
    void SetImmDamageOffset(short sV)   { m_sLastDamageOffset = sV; }
    void AddWaterImmDm(short sV)        { m_sLastElmDamage[SKILL_ELEMENT_WATER] = sV; }     // 25 添加水伤害
    void AddFireImmDm(short sV)         { m_sLastElmDamage[SKILL_ELEMENT_FIRE] = sV; }      // 26 添加火伤害
    void AddPoisonImmDm(short sV)       { m_sLastElmDamage[SKILL_ELEMENT_POISON] = sV; }    // 27 添加毒伤害

public:

    virtual void AddAddedStr(short v){}
    virtual void AddAddedCon(short v){}
    virtual void AddAddedDex(short v){}
    virtual void AddAddedInt(short v){}
    virtual void AddAddedHit(short v){}
    virtual void AddResist(short v){}

    // Op
    virtual void Ride(){}
	virtual void ChangeWeapon(){}
	virtual void BackHome(){}
    virtual void Unride(){}
    virtual void FbXiulian(){}
    virtual void FbZhuru(){}
    virtual void UIProduce(short v){}
    virtual void UISyncretize(){}
    virtual void ItemDecompound(){}
    virtual void AddEff(short sEffectID, DWORD dwUID){}
    virtual void ClearUseful(){}
	virtual void SpecialClearUseful(){}
	virtual void TopoClearUseful(){}
    virtual void ClearBaleful(){}
    virtual void ClearAllEff(){}
    virtual void ClearEff(short sEffectID){}
    virtual void Conjure(int NpcId,int Num){}
	virtual void MoveParent(CActorSkill *pReleaser){}//把目标瞬间移动到自己旁边

public:
    short GetImmDamage()
    {
        short s = m_sLastDamage+m_sLastDamageOffset;
        m_sLastDamage = 0;
        m_sLastDamageOffset = 0;
        return s;
    }
    short GetImmWaterDm()  // 水伤害
    {
        short s = m_sLastElmDamage[SKILL_ELEMENT_WATER];
        m_sLastElmDamage[SKILL_ELEMENT_WATER] = 0;
        return s;
    }
    short GetImmFireDm()   // 火伤害
    {
        short s = m_sLastElmDamage[SKILL_ELEMENT_FIRE];
        m_sLastElmDamage[SKILL_ELEMENT_FIRE] = 0;
        return s;
    }
    short GetImmPoisonDm() // 毒伤害
    {
        short s = m_sLastElmDamage[SKILL_ELEMENT_POISON];
        m_sLastElmDamage[SKILL_ELEMENT_POISON] = 0;
        return s;
    }

protected:
    short   m_sLastDamage;
    short   m_sLastDamageOffset;
    short   m_sLastElmDamage[SKILL_MAX_ELEMENT_ATTR];

};


/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
