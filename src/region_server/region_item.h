#ifndef REGION_ITEM_H
#define REGION_ITEM_H

class CRegionItemContainer : public CItemContainerBase
{
public:
	CRegionItemContainer(CItemManager *pManager, CRegionCreature* pMaster);
	virtual ~CRegionItemContainer();
	virtual bool	CheckEquip(EEquipParts vPart, SItemID& item, SCreature &cre);// 测试人物装备起物品的条件是否成立
    virtual bool	CheckUse(SItemID& item, SCreature &cre) const;				// 检查使用道具的需求是否满足

	virtual void	OnAfterEquip(SItemID& item, EEquipParts vPart);
	virtual void	OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld);

	bool			ItemCanChangeAttrib(SItemID &item);
	bool			ItemHaveToRecomputeAttrib(SItemID &item);

    void            OnCreatureAttribChanged(EEquipParts part);
	void			OnCreatureAttribChanged();

	void			ExchangeHandWeapon();				// 主要用于：只有左手拿武器（而且不是盾牌）时，把它换到右手
	void			UpdateItemToClient(EItemContainer container, DWORD itemID);	// 把一个道具的数据更新到客户端
	void			UpdateItemToClient(EItemContainer container, int page, int gridI, int gridJ);
	void			UpdateItemToClient(CItemContainerBase::EEquipParts part);

	// 掉落装备
	void			DropEquipItem(CRegionCreature *pCre, int count, vector<SItemID>& itemsDrop);
	void			DropBagItem(CRegionCreature *pCre, int count, vector<SItemID>& itemsDrop);
	int				DropMoney(CRegionCreature *pCre, float fRate);

    void            GetSkillAddtionRate(float& fDamageWea1, float& fDamageWea2, float& fAttackWea1, float& fAttackWea2);

	void			LockOperate(bool bLock);
	bool			IsOperateLocked();

	ETradableResult	CanItemTrade(char bCommonBag, int page, int i, int j);
	ETradableResult	CanItemTrade(DWORD itemID);

protected:
	void			AttribToCreature(SItemID& item, SAttribAddtion& att, BYTE attribRate, bool bAdd, bool bItemRelated = true);		// 把追加属性更新到人物上(bItemRelated表示是否要乘上装备的等级)
	void			AttribToCreature(SItemID& item,DWORD attribID, BYTE attribRate, bool bAdd, bool bItemRelated = true);			// 把追加属性更新到人物上(bItemRelated表示是否要乘上装备的等级)
	virtual void	_UpdateCreature(bool bEquip, SItemID& item, EEquipParts part);	// Equip和UnEquip时调用，调整人物属性

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

#define ITEMSALESTAT_LOG_INTERVAL			(4*3600*1000)		// 每隔多少时间写入数据库
#define ITEMSALESTAT_RUN_INTERVAL			(300*1000)			// 每隔多少时间run一次
class CRegionItemSaleStat
{
public:
	struct SItemSaleStat
	{
		std::string ItemName;	// 道具名字
		int Count;				// 统计个数
		DWORD StartTick;		// 开始统计时间
	};

	CRegionItemSaleStat();
	~CRegionItemSaleStat();

	void Run();

	void Add(DWORD ItemIdx, int Count);

	void LogToDb(DWORD ItemIdx, const std::string& ItemName, int Count, DWORD IntervalInMinites);

public:
	EXT_SPACE::unordered_map<DWORD, SItemSaleStat> m_Stat; // 道具类型对统计结构的映射
};

extern CRegionItemSaleStat g_ItemSaleStat;

#endif // REGION_ITEM_H
