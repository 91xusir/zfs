
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
    virtual int   GetAttack();      // ȡ������
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
    virtual void AddPhyAr(short sV);        // 15 ����������
    virtual void AddPhyAb(short sV);        // 16 �����������
    virtual void AddAbsorbHP(short sV);     // 17 �����Ѫ
    virtual void AddAbsorbMP(short sV);     // 18 �����ħ
    virtual void AddDamageMin(short sV,int _DamageMode);    // 19 �����С�˺�
    virtual void AddDamageMax(short sV,int _DamageMode);    // 20 �������˺�
    virtual void AddSpeedAttack(short sV);  // 21 ��ӹ����ٶ�
    virtual void AddSpeedMove(short sV);    // 22 ����ƶ��ٶ�
    virtual void AddRateGold(short sV);     // 23 ��ӻ�ý�Ǯ����
    virtual void AddRateExp(short sV);      // 24 ��ӻ�þ��鱶��
    virtual void AddWaterDm(short sV,int _DamageMode);      // 25 ���ˮ�˺�
    virtual void AddFireDm(short sV,int _DamageMode);       // 26 ��ӻ��˺�
    virtual void AddPoisonDm(short sV,int _DamageMode);     // 27 ��Ӷ��˺�
    virtual void AddWaterAr(short sV);      // 28 ���ˮ����
    virtual void AddFireAr(short sV);       // 29 ��ӻ����
    virtual void AddPoisonAr(short sV);     // 30 ��Ӷ�����
    virtual void AddWaterAb(short sV);      // 31 ���ˮ����
    virtual void AddFireAb(short sV);       // 32 ��ӻ�����
    virtual void AddPoisonAb(short sV);     // 33 ��Ӷ�����
    virtual void AddAttackRate(short sV);   // 34 �������
    virtual void AddDodge(short sV);        // 35 ��Ӷ��
    virtual void AddAttackRange(short sV);  // 36 ��ӹ�������
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
    virtual void AddPhyArR(float fV);  // 15 ����������
    virtual void AddPhyAbR(float fV);  // 16 �����������
    virtual void AddAbsorbHPR(float fV);  // 17 �����Ѫ
    virtual void AddAbsorbMPR(float fV);  // 18 �����ħ
    virtual void AddDamageMinR(float fV);  // 19 �����С�˺�
    virtual void AddDamageMaxR(float fV);  // 20 �������˺�
    virtual void AddSpeedAttackR(float fV);  // 21 ��ӹ����ٶ�
    virtual void AddSpeedMoveR(float fV);  // 22 ����ƶ��ٶ�
    virtual void AddRateGoldR(float fV);  // 23 ��ӻ�ý�Ǯ����
    virtual void AddRateExpR(float fV);  // 24 ��ӻ�þ��鱶��
    virtual void AddWaterDmR(float fV);  // 25 ���ˮ�˺�
    virtual void AddFireDmR(float fV);  // 26 ��ӻ��˺�
    virtual void AddPoisonDmR(float fV);  // 27 ��Ӷ��˺�
    virtual void AddWaterArR(float fV);  // 28 ���ˮ����
    virtual void AddFireArR(float fV);  // 29 ��ӻ����
    virtual void AddPoisonArR(float fV);  // 30 ��Ӷ�����
    virtual void AddWaterAbR(float fV);  // 31 ���ˮ����
    virtual void AddFireAbR(float fV);  // 32 ��ӻ�����
    virtual void AddPoisonAbR(float fV);  // 33 ��Ӷ�����
    virtual void AddAttackRateR(float fV); // 34 �������
    virtual void AddDodgeR(float fV); // 35 ��Ӷ��
    virtual void AddAttackRangeR(float fV); // 36 ��ӹ�������
    virtual void AddCriticalR(float fV);
    virtual void StateMove(short sV);    // 78 �Ƿ�����ƶ�
    virtual void StateDodge(short sV);    // 79 �Ƿ��������
    virtual void StateUseSkill(short sV);    // 80 �Ƿ����ʹ�ü���
    virtual void StateNAttack(short sV);    // 81 �Ƿ������ͨ����
    virtual void StateResist(short sV);    // 82 ����״̬�ֿ�
    virtual void Hate(short sV);    // 83 ����ĳ�޶�
    virtual void HateRate(short sV);    // 84 ��ޱ���
    virtual void AddHate(short sV, CDummyActorSkill *pReleaser);    // 85 ���ӹ�����
    virtual void AddSkillCriRate(short sV, CDummyActorSkill *pReleaser);    // 86 ���ܱ�����
    virtual void AddSkillCriDamageR(short sV, CDummyActorSkill *pReleaser);    // 87 ���ܱ����˺�
	virtual void AddResilience(float fV); //90 ������
	virtual void Rebirth(int iHPrate,int iMPrate); //91 ����
	virtual void AddAbsorptionMaxDamage(float sV);//Tianh ���������˺����ֵ
	virtual void PlayerHiding(int aV); //����
	virtual void AddSkillCriDamage(int sV); // ���ܱ����˺��ӳ�
	virtual void AddCriticalDamage(int sV);// ��ͨ���������˺�
	virtual void AddCriticalDamageR(int sV);// ��ͨ�����������ʣ����Ǳ����˺�����
	virtual void AddShieldMpR(int sV); // ����ʥ��ħ����������
	virtual void AddReboundHurtR(float sV,int Type);//�����˺���
	virtual void AddAbsorptionHpR(int sV); //�ܵ����˺�ת��ΪHP��
	virtual void HPMPConversion(float iHPrate,float iMPrate); //����HP ����MP  ��������HP ����MP��MP���� ���ٵ�HP*�ٷ��ʣ�
	virtual void AddCutMaxHP(float fV);//���ӻ�������HP�İٷֱ�
	virtual void AddCutMaxMP(float fV);//���ӻ�������MP�İٷֱ�
	virtual void ExtractHpR(int iHPrate,int maxHP);//��ȡ�Է�HP�ٷֱ�  ���iMPrate
	virtual void OrdinaryAddEffect(int probability,int EffectID);//�չ�������Ч
	virtual void SeriesDamage(int Series,int damageR);
	virtual void BossConjure(int NpcId,int Num);
	virtual void PlayerImmuno(int sV); //����  +1 �������� + 2�����к�  +3��������
	virtual void ChangeBody(int ModelID);  //����
	virtual void MomentMove(int sV);  //˲���ƶ�
	virtual void AddResistZXC(int sV); //�ֿ����� 
	virtual void AddResistSwim(int sV); //�ֿ�ѣ�� 
	virtual void AddResistInertia(int sV); //�ֿ��ٶ� 
	virtual void AddResistTrance(int sV); //�ֿ���˯
	virtual void AddResistBleed(int sV); //�ֿ���Ѫ
	virtual void AddReceiveDamageSkillR(float fV);//���ӽ��ռ����˺�����
	virtual void TeamShareDamage(int sV);//���ѷֵ��˺�
	virtual void MonomersCgroup(int sV);//�����Ⱥ���չ� ���뷶Χ
	virtual void Hprelay(int sV,CActorSkill *pTarget);//���Լ���ǰ��HP���ݸ�����
	virtual void Mprelay(int sV,CActorSkill *pTarget);//���Լ���ǰ��MP���ݸ�����
	virtual void UltimateChange(int sV);//�ռ�����
	virtual void SkillhpRecover(int sV);       //����ÿ��ָ�Ѫ��
	virtual void SkillmpRecover(int sV);     //����ÿ��ָ�����
	//virtual void ConsumeNpc(int sV);  //�����ٻ�NPC�ĸ���
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
	virtual void MoveParent(CActorSkill *pReleaser);//��Ŀ��˲���ƶ����Լ��Ա�

    bool CheckCanUse(SSkill* pSkillAttr, CActorSkill* pReleaser);

	virtual void OnSkillModify();
	virtual void OnEffectModify();

	// Ч��
	virtual void OnEffectOpen(SActorEffect* pEffect);
	virtual void OnEffectClose(SActorEffect* pEffect);
	virtual bool OnEffectCycle(SActorEffect* pEffect);

protected:

    virtual void OnUseRangeSkill(SSkill* pSkillAttr, const float* pPos);
	virtual void OnUseRangeSkillAddHp(SSkill* pSkillAttr, const float* pPos,CActorSkill* pReleaser);
    virtual void OnStudySkill(SSkill* pSkillAttr, bool bRefresh);
    virtual void OnClearSkill(bool bFigthSkill, int iRemoveSP);

	//virtual void OnResumeTimeModify(WORD wID,  WORD wResumeTime);

    // ��������
    virtual void OnSkillUse(SSkill* pSkill, CActorSkill* pReleaser);
	virtual void OnSkillUseCalculating(SSkill* pSkill, CActorSkill* pReleaser,int iDmgPhy,int iDmgElement[MAX_ELEMENT_ATTR],long bCriDamage,bool bResult);
	virtual void OnSkillAddHP(SSkill* pSkill, CActorSkill* pReleaser);

    // ��������
    virtual void OnPassiveOpen(SActorPassive* pPassive);
    virtual void OnPassiveClose(SActorPassive* pPassive);



private:
    float   m_pPos[3];

};

#endif // _REGION_SKILL_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
