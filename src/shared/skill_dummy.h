

class CDummyActorSkill
{
public:
    // Get
    virtual short GetMaxHP()=0;
    virtual int   GetLevel()=0;
    virtual int   GetHP()=0;
    virtual int   GetMP()=0;
	virtual float GetEnP()=0;
    virtual int   IsRide()=0; // 0���ɣ�1���ޣ�2����
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
    virtual int   GetAttack()=0;    // ȡ������
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
    virtual void AddPhyAr(short sV){};       // 15 ����������
    virtual void AddPhyAb(short sV){};       // 16 �����������
    virtual void AddAbsorbHP(short sV){};    // 17 �����Ѫ
    virtual void AddAbsorbMP(short sV){};    // 18 �����ħ
    virtual void AddDamageMin(short sV,int _DamageMode){};   // 19 �����С�˺�
    virtual void AddDamageMax(short sV,int _DamageMode){};   // 20 �������˺�
    virtual void AddSpeedAttack(short sV){}; // 21 ��ӹ����ٶ�
    virtual void AddSpeedMove(short sV){};   // 22 ����ƶ��ٶ�
    virtual void AddRateGold(short sV){};    // 23 ��ӻ�ý�Ǯ����
    virtual void AddRateExp(short sV){};     // 24 ��ӻ�þ��鱶��
    virtual void AddWaterAr(short sV){};     // 28 ���ˮ����
    virtual void AddFireAr(short sV){};      // 29 ��ӻ����
    virtual void AddPoisonAr(short sV){};    // 30 ��Ӷ�����
    virtual void AddWaterAb(short sV){};     // 31 ���ˮ����
    virtual void AddFireAb(short sV){};      // 32 ��ӻ�����
    virtual void AddPoisonAb(short sV){};    // 33 ��Ӷ�����
    virtual void AddAttackRate(short sV){};  // 34 �������
    virtual void AddDodge(short sV){};       // 35 ��Ӷ��
    virtual void AddAttackRange(short sV){}; // 36 ��ӹ�������
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
    virtual void AddPhyArR(float fV){}  // 15 ����������
    virtual void AddPhyAbR(float fV){}  // 16 �����������
    virtual void AddAbsorbHPR(float fV){}  // 17 �����Ѫ
    virtual void AddAbsorbMPR(float fV){}  // 18 �����ħ
    virtual void AddDamageMinR(float fV){}  // 19 �����С�˺�
    virtual void AddDamageMaxR(float fV){}  // 20 �������˺�
    virtual void AddSpeedAttackR(float fV){}  // 21 ��ӹ����ٶ�
    virtual void AddSpeedMoveR(float fV){}  // 22 ����ƶ��ٶ�
    virtual void AddRateGoldR(float fV){}  // 23 ��ӻ�ý�Ǯ����
    virtual void AddRateExpR(float fV){}  // 24 ��ӻ�þ��鱶��
    virtual void AddWaterDmR(float fV){}  // 25 ���ˮ�˺�
    virtual void AddFireDmR(float fV){}  // 26 ��ӻ��˺�
    virtual void AddPoisonDmR(float fV){}  // 27 ��Ӷ��˺�
    virtual void AddWaterArR(float fV){}  // 28 ���ˮ����
    virtual void AddFireArR(float fV){}  // 29 ��ӻ����
    virtual void AddPoisonArR(float fV){}  // 30 ��Ӷ�����
    virtual void AddWaterAbR(float fV){}  // 31 ���ˮ����
    virtual void AddFireAbR(float fV){}  // 32 ��ӻ�����
    virtual void AddPoisonAbR(float fV){}  // 33 ��Ӷ�����
    virtual void AddAttackRateR(float fV){} // 34 �������
    virtual void AddDodgeR(float fV){} // 35 ��Ӷ��
    virtual void AddAttackRangeR(float fV){} // 36 ��ӹ�������
    virtual void AddCriticalR(float fV){}
    virtual void AddWaterDm(short sV,int _DamageMode){}     // 25 ���ˮ�˺�
    virtual void AddFireDm(short sV,int _DamageMode){}      // 26 ��ӻ��˺�
    virtual void AddPoisonDm(short sV,int _DamageMode){}    // 27 ��Ӷ��˺�
    virtual void StateMove(short sV){}    // 78 �Ƿ�����ƶ�
    virtual void StateDodge(short sV){}    // 79 �Ƿ��������
    virtual void StateUseSkill(short sV){}    // 80 �Ƿ����ʹ�ü���
    virtual void StateNAttack(short sV){}    // 81 �Ƿ������ͨ����
    virtual void StateResist(short sV){}    // 82 ����״̬�ֿ�
    virtual void Hate(short sV){}    // 83 ����ĳ�޶�
    virtual void HateRate(short sV){}    // 84 ��ޱ���
    virtual void AddHate(short sV, CDummyActorSkill *pReleaser){}    // 85 ���ӹ�����
    virtual void AddSkillCriRate(short sV, CDummyActorSkill *pReleaser){}    // 86 ���ܱ�����
    virtual void AddSkillCriDamageR(short sV, CDummyActorSkill *pReleaser){}    // 87 ���ܱ����˺���
	virtual void AddResilience(float fV){} //90 ������
	virtual void Rebirth(int iHPrate,int iMPrate){} //91 ����
	virtual void AddAbsorptionMaxDamage(float sV){}//Tianh ���������˺����ֵ
	virtual void PlayerHiding(int aV){}
	virtual void AddSkillCriDamage(int sV){} // ���ܱ����˺��ӳ�
	virtual void AddCriticalDamage(int sV){}// ��ͨ���������˺�
	virtual void AddCriticalDamageR(int sV){}// ��ͨ�����������ʣ����Ǳ����˺�����
	virtual void AddShieldMpR(int sV){} // ����ʥ��ħ����������
	virtual void AddReboundHurtR(float sV,int Type){}//�����˺���
	virtual void AddAbsorptionHpR(int sV){} //�ܵ����˺�ת��ΪHP��
	virtual void HPMPConversion(float iHPrate,float iMPrate){} //102����HP ����MP  ��������HP ����MP
	virtual void AddCutMaxHP(float fV){}//���ӻ�������HP�İٷֱ�
	virtual void AddCutMaxMP(float fV){}//���ӻ�������MP�İٷֱ�
	virtual void ExtractHpR(int iHPrate,int maxHP){}//��ȡ�Է�HP�ٷֱ�  ���iMPrate
	virtual void OrdinaryAddEffect(int probability,int EffectID){}
	virtual void SeriesDamage(int Series,int damageR){}//���Ӽ���ϵ���˺�
	virtual void BossConjure(int NpcId,int Num){}
	virtual void PlayerImmuno(int sV){} //����  +1 �������� + 2�����к�  +3��������
	virtual void ChangeBody(int ModelID){} //����
	virtual void MomentMove(int sV){}  //˲���ƶ�
	virtual void AddResistZXC(int sV){} //�ֿ����� 
	virtual void AddResistSwim(int sV){} //�ֿ�ѣ�� 
	virtual void AddResistInertia(int sV){} //�ֿ��ٶ� 
	virtual void AddResistTrance(int sV){} //�ֿ���˯
	virtual void AddResistBleed(int sV){} //�ֿ���Ѫ
	virtual void AddReceiveDamageSkillR(float fV){}//���ӽ��ռ����˺�����
	virtual void TeamShareDamage(int sV){}//���ѷֵ��˺�
	virtual void MonomersCgroup(int sV){}//�����Ⱥ���չ� ���뷶Χ
	virtual void Hprelay(int sV,CActorSkill *pTarget){}//���Լ���ǰ��HP���ݸ�����
	virtual void Mprelay(int sV,CActorSkill *pTarget){}//���Լ���ǰ��MP���ݸ�����
	virtual void UltimateChange(int sV){}//�ռ�����
	virtual void SkillhpRecover(int sV){}       //����ÿ��ָ�Ѫ��
	virtual void SkillmpRecover(int sV){}    //����ÿ��ָ�����
	//virtual void ConsumeNpc(int sV){}  //�����ٻ�NPC�ĸ���

    // ֱ���˺���ֻ����actUse��Apply
    void SetImmDamage(short sV)         { m_sLastDamage = sV; }
    void SetImmDamageOffset(short sV)   { m_sLastDamageOffset = sV; }
    void AddWaterImmDm(short sV)        { m_sLastElmDamage[SKILL_ELEMENT_WATER] = sV; }     // 25 ���ˮ�˺�
    void AddFireImmDm(short sV)         { m_sLastElmDamage[SKILL_ELEMENT_FIRE] = sV; }      // 26 ��ӻ��˺�
    void AddPoisonImmDm(short sV)       { m_sLastElmDamage[SKILL_ELEMENT_POISON] = sV; }    // 27 ��Ӷ��˺�

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
	virtual void MoveParent(CActorSkill *pReleaser){}//��Ŀ��˲���ƶ����Լ��Ա�

public:
    short GetImmDamage()
    {
        short s = m_sLastDamage+m_sLastDamageOffset;
        m_sLastDamage = 0;
        m_sLastDamageOffset = 0;
        return s;
    }
    short GetImmWaterDm()  // ˮ�˺�
    {
        short s = m_sLastElmDamage[SKILL_ELEMENT_WATER];
        m_sLastElmDamage[SKILL_ELEMENT_WATER] = 0;
        return s;
    }
    short GetImmFireDm()   // ���˺�
    {
        short s = m_sLastElmDamage[SKILL_ELEMENT_FIRE];
        m_sLastElmDamage[SKILL_ELEMENT_FIRE] = 0;
        return s;
    }
    short GetImmPoisonDm() // ���˺�
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
