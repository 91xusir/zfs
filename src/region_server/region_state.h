
enum ECreatureStateType
{
	CRE_STATE_MAGIC_WEAPON_ADD_MP,
	CRE_STATE_MAGIC_WEAPON_ADD_ELE, 
	CRE_STATE_DEAD,
	CRE_STATE_COMPOSE_ITEM,
	CRE_STATE_PROCESS_BATTLE,
	CRE_STATE_FUSE_PET,
	CRE_STATE_CATCH_PET,
};

struct SCreStateParam
{
	SCreStateParam(){p1=p2=p3=p4=p5=0;}
	long p1;
	long p2;
	long p3;
	long p4;
	long p5;
};

class CCreatureState
{
public:
	CCreatureState() { m_bEnd = false; m_bCanMove = false; }
	virtual ~CCreatureState() {}
	
	void SetName(const char *name) {
		m_name = name;
	}
	void SetType(const ECreatureStateType type) {
		m_type = type;
	}
	const char* GetName() {
		return m_name.c_str();
	}
	const ECreatureStateType GetType() {
		return m_type;
	}
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param) {
		m_bEnd = false;
		return true;
	}
	virtual bool Tick(unsigned long delta) { 
		if(m_bEnd) return false;
		return true;
	}
	virtual void End() {
		m_bEnd = true;
	}
	bool GetCanMove() {return m_bCanMove;}
	void SetCanMove(bool move) {m_bCanMove = move;}

private:
	string m_name;
	ECreatureStateType m_type;
	bool   m_bEnd;
	bool   m_bCanMove;
};

class CCreState_MagWeaAddMp : public CCreatureState
{
public:
	CCreState_MagWeaAddMp();
	virtual ~CCreState_MagWeaAddMp();
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End();

	CRegionCreature *m_host;

	long m_lTime;
};

class CCreState_MagWeaAddEle : public CCreatureState
{
public:
	CCreState_MagWeaAddEle();
	virtual ~CCreState_MagWeaAddEle();
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End();

	CRegionCreature *m_host;
	int				m_itemId;
	int				m_eleType;
	unsigned long	m_endTime;
	float			m_interval;

	bool			m_bCanFull;
	SItemID			m_itemID;
	int 			m_iRifine;
	int 			m_iTrumpParam;
	float			m_noFullValue;
	float			m_ActorSoulGap;
	bool			m_bStop;

	float			m_fSpeed;
	DWORD			m_dwNeedTime;
	DWORD			m_dwNeedSoul;
	DWORD			m_dwLastTime;
};

const unsigned char PET_FUSE_MAX_ITEM = 3;
class CCreState_FusePet : public CCreatureState
{
public:
	CCreState_FusePet();
	virtual ~CCreState_FusePet();
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End();

	bool SetFuseItem(unsigned char fuseType, SItemID item[PET_FUSE_MAX_ITEM]);

private:
	SItemID	m_item[PET_FUSE_MAX_ITEM];
	CRegionCreature *m_host;
	DWORD m_dwEndTime;
	unsigned char m_cFuseType;
	int m_nItemCount;
	bool m_bIsFuse;
};

class CCreState_CatchPet : public CCreatureState
{
public:
	CCreState_CatchPet();
	virtual ~CCreState_CatchPet();
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End();

	bool SetSpirit(CRegionCreature *spirit);

	CRegionCreature *m_spirit;
private:
	CRegionCreature *m_host;
	DWORD m_dwEndTime;
};

class CCreState_Dead : public CCreatureState
{
public:
	CCreState_Dead();
	virtual ~CCreState_Dead();
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End();

	CRegionCreature *m_host;
	DWORD            m_deadTime;
};

class CCreState_Battle : public CCreatureState
{
public:
	CCreState_Battle(void);
	virtual ~CCreState_Battle(void);
	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End(void);

private:
	const DWORD		c_cycleTime;
	CRegionCreature	*m_host;
	DWORD			m_startTime;
};
