#ifndef REGION_ITEM_H
#define REGION_ITEM_H

class CRegionItemContainer : public CItemContainerBase
{
public:
	CRegionItemContainer(CItemManager *pManager, CRegionCreature* pMaster);
	virtual ~CRegionItemContainer();
	virtual bool	CheckEquip(EEquipParts vPart, SItemID& item, SCreature &cre);// ��������װ������Ʒ�������Ƿ����
    virtual bool	CheckUse(SItemID& item, SCreature &cre) const;				// ���ʹ�õ��ߵ������Ƿ�����

	virtual void	OnAfterEquip(SItemID& item, EEquipParts vPart);
	virtual void	OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld);

	bool			ItemCanChangeAttrib(SItemID &item);
	bool			ItemHaveToRecomputeAttrib(SItemID &item);

    void            OnCreatureAttribChanged(EEquipParts part);
	void			OnCreatureAttribChanged();

	void			ExchangeHandWeapon();				// ��Ҫ���ڣ�ֻ�����������������Ҳ��Ƕ��ƣ�ʱ��������������
	void			UpdateItemToClient(EItemContainer container, DWORD itemID);	// ��һ�����ߵ����ݸ��µ��ͻ���
	void			UpdateItemToClient(EItemContainer container, int page, int gridI, int gridJ);
	void			UpdateItemToClient(CItemContainerBase::EEquipParts part);

	// ����װ��
	void			DropEquipItem(CRegionCreature *pCre, int count, vector<SItemID>& itemsDrop);
	void			DropBagItem(CRegionCreature *pCre, int count, vector<SItemID>& itemsDrop);
	int				DropMoney(CRegionCreature *pCre, float fRate);

    void            GetSkillAddtionRate(float& fDamageWea1, float& fDamageWea2, float& fAttackWea1, float& fAttackWea2);

	void			LockOperate(bool bLock);
	bool			IsOperateLocked();

	ETradableResult	CanItemTrade(char bCommonBag, int page, int i, int j);
	ETradableResult	CanItemTrade(DWORD itemID);

protected:
	void			AttribToCreature(SItemID& item, SAttribAddtion& att, BYTE attribRate, bool bAdd, bool bItemRelated = true);		// ��׷�����Ը��µ�������(bItemRelated��ʾ�Ƿ�Ҫ����װ���ĵȼ�)
	void			AttribToCreature(SItemID& item,DWORD attribID, BYTE attribRate, bool bAdd, bool bItemRelated = true);			// ��׷�����Ը��µ�������(bItemRelated��ʾ�Ƿ�Ҫ����װ���ĵȼ�)
	virtual void	_UpdateCreature(bool bEquip, SItemID& item, EEquipParts part);	// Equip��UnEquipʱ���ã�������������

private:
	bool			m_bLocked;
	DWORD			m_dwLastLockTick;

	CRegionCreature *m_pMaster;
    float           m_lastTwoBianRate_Damage_Wea1;
    float           m_lastTwoBianRate_Damage_Wea2;
    float           m_lastTwoBianRate_Attack_Wea1;
    float           m_lastTwoBianRate_Attack_Wea2;

public:
	EXT_SPACE::unordered_map<DWORD, BYTE> m_ReleaseEffectsToEnemy;	// skillID, skillRate
	EXT_SPACE::unordered_map<DWORD, BYTE> m_ReleaseEffectsToSelf;	// skillID, skillRate
};


class CRegionItemCompose : public CCreatureState
{
public:
	CRegionItemCompose();
	~CRegionItemCompose();

	void Clear();
	void SetComposeID(DWORD ComposeID) { m_ComposeID = ComposeID; }

	virtual bool Begin(CRegionCreature *host,SCreStateParam *param);
	virtual bool Tick(unsigned long delta);
	virtual void End();

private:
	CRegionCreature* m_Host;

	DWORD m_ComposeID;
	DWORD m_EndTime;
};

#define ITEMSALESTAT_LOG_INTERVAL			(4*3600*1000)		// ÿ������ʱ��д�����ݿ�
#define ITEMSALESTAT_RUN_INTERVAL			(300*1000)			// ÿ������ʱ��runһ��
class CRegionItemSaleStat
{
public:
	struct SItemSaleStat
	{
		std::string ItemName;	// ��������
		int Count;				// ͳ�Ƹ���
		DWORD StartTick;		// ��ʼͳ��ʱ��
	};

	CRegionItemSaleStat();
	~CRegionItemSaleStat();

	void Run();

	void Add(DWORD ItemIdx, int Count);

	void LogToDb(DWORD ItemIdx, const std::string& ItemName, int Count, DWORD IntervalInMinites);

public:
	EXT_SPACE::unordered_map<DWORD, SItemSaleStat> m_Stat; // �������Ͷ�ͳ�ƽṹ��ӳ��
};

extern CRegionItemSaleStat g_ItemSaleStat;

#endif // REGION_ITEM_H
