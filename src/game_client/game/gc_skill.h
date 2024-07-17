
#pragma once

/*----------------------------------------------------------------------------
- GcSkill.
----------------------------------------------------------------------------*/

class GcSkill : public CSkillInstance
{
public:
    GcSkill(unsigned short wSkillID);
    virtual ~GcSkill();

    virtual void OnSkillRun(unsigned long dwMillisecond,CActorSkill* pReleaser);
	virtual bool OnCanUse(CActorSkill* pReleaser, BYTE cTargetType, CActorSkill* pTarget, const float* pPos, bool bIgnoreDistance);

    void SetUIButton(RtwButton* pButton);

	bool AutoEatMPBottle(CActorSkill* pReleaser);
	void OnSetCanUse(bool bCanUse)
	{
		m_bCanUse= bCanUse;
	}
private:
    bool        m_bLastState;
	bool		m_bCanUse;
    RtwButton*  m_pButton;
	float m_curMaskValue;
};

/*----------------------------------------------------------------------------
- GcActorSkill.
----------------------------------------------------------------------------*/

class GcActorSkill : public CActorSkill
{
public:
    GcActorSkill();
    virtual ~GcActorSkill();

    GcActor* GetParentEx()          { return (GcActor*)m_pParent; }

    void UpdateUI();
    void UpdateEffectUI();
	void UpdateEffectUIForActor(int type);
	void ClearEffectUIForActor(int type);

    void UpdateSkillExp(CG_CmdPacket* pPacket);

    std::list<SSkillEffect*>    m_listSkillEffect;   // 技能效果列表

    void NetReadSkill(CG_CmdPacket* pPacket);   // 只用在客户端
    bool NetReadEffect(CG_CmdPacket* pPacket);  // 读取效果
    bool NetReadEffect(std::map<unsigned short,DWORD>& setEffect);  // 读取效果

    void OnGetEffectHint (RtwWidget* sender, void*);

    // 重载

    virtual void OnAddSkill(CSkillInstance* pSkill);
    virtual void OnDeleteSkill(CSkillInstance* pSkill);

    virtual void OnAddEffect(CSkillInstance* pEffect);
    virtual void OnDeleteEffect(CSkillInstance* pEffect);

    virtual void OnStudySkill(SSkill* pSkillAttr, bool bRefresh);

    // Get
    virtual short GetMaxHP();
    virtual int   GetLevel();
    virtual int   GetHP();
    virtual int   GetMP();
    virtual int   IsRide();
	virtual float GetEnP();
	virtual int   IsChangeBody();
	virtual int   IsHaveCallNpc();
	virtual int   GetSkillSeries();

	virtual int   GetWeaponType();    // 1鞭 2斧 3弓 4弩 5双鞭 6飞轮 7 法杖 8 飞剑 9 宝珠
    virtual int   GetWeaponEquiped();
	virtual int   GetTrumpEquiped();

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
    virtual int   GetAttack();
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

    // Op
    /*
    virtual void FbXiulian();
    virtual void FbZhuru();
    virtual void UIProduce(short v);
    virtual void UISyncretize();
    virtual void ItemDecompound();
    */
protected:
	std::map<unsigned short,DWORD> m_mapEffect;
	CRITICAL_SECTION	m_cs;
};

/*----------------------------------------------------------------------------
- End.
----------------------------------------------------------------------------*/
