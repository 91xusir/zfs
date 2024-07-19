#ifndef _ITEM_BASE_H
#define _ITEM_BASE_H

#include "item_base.h"
#include "gc_actor_graph.h"




class CPlayerItemContainerView;
class GcActor;
class GcActorGraph;

enum EItemOperate
{
	IO_Throw,
	IO_Delete,
	IO_Move,
	IO_Equip,
	IO_UnEquip,
	IO_Eat,
	IO_UsePet,
	IO_UseScroll,
	IO_UseOther,
	IO_EnchantItem,		// 附魔
	IO_Decompose,		// 分解
    IO_Lock
};

class CItemContainer : public CItemContainerBase
{
public:
	CItemContainer(CItemManager *pManager, SCreature*pCre, GcActor* pActor);
	virtual ~CItemContainer();
	GcActorGraph*		GetGraph(EEquipParts vPart);
	virtual bool		CheckEquip(EEquipParts vPart, SItemID& item, SCreature &cre);	// 测试人物装备起物品的条件是否成立
	virtual void		_Equip(SItemID& id, EEquipParts vPart);
	virtual bool		_UnEquip(EEquipParts vPart, bool bAddToBag = false);
	virtual bool		UnEquipAll(bool bAddToBag = false);
    virtual bool        CheckUse(SItemID& item, SCreature &cre) const;
	void				UpdateGraph();			// 如果人物身上有道具但是没有刷新图形时使用
	void				Run(unsigned long dwMillisecond);
    void	            EquipDefaultItem(EEquipParts vPart, const char vpIDName[]);

	virtual void		OnAfterEquip(SItemID& item, EEquipParts vPart);
	virtual void		OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld);

	void				ExchangeHandWeapon();
	void				SetEquipItemValid(EEquipParts part, bool bValid);

	static int			CalcHintLineCount(const char* str);
	static RtString 	GenerateAttribString(SItemBase* pItemClass, SAttribAddtion &attrib, BYTE rate, bool bItemRelated = true);

	// 针对：删除，移动，装备，卸下，扔，拣，喝药瓶，神兽，卷轴，其他道具的使用
	bool				OperateItem_CheckUIContent(SItemID &item, EItemOperate op);

	void				OnResetGraph();

    void                SetEquipData(SItemID &item, EEquipParts part);
    void                ChangeModel(SItemID &itemOrig, SItemID &itemNew, EEquipParts part);

	ETradableResult		CanItemTrade(char bCommonBag, int page, int i, int j);
	ETradableResult		CanItemTrade(DWORD itemID);

	char				SwitchSpareWeapon();

private:
    const char*		    mpDefaultItem[MAX_EQUIP_PARTS];		// 默认装备的名称
	void				_UpdateGraph(EEquipParts part);		// 更新某个部位的图像
	GcActor*			m_pMaster;

	void				WeaponPlayPose(const char* szLinkName);

	// 用于保存最后一次的装备(Graph总是表现此变量)，主要用于通过网络刷新过来的装备数据如果与当前不符，会根据此变量来卸装
	SItemID				m_nLastItemID[MAX_EQUIP_PARTS];
};

#define ITEM_HINT_BLANK_HEIGHT		10
#define ITEM_HINT_LINE_HEIGHT		16
#define ITEM_HINT_WIDTH				160

RtString GenerateItemHtmlHint(int &outLineCount, SItemID &itemID, GcActor *pPlayer, CBag *pBag = NULL, int page = -1, int i = -1, int j = -1,  float fPriceRate = -1.0f, UINT price = 0, bool bCountAll = false, float fTaxRate = -1.0f, int tTimeLimit=0,int OtherPrice = 0);

CItemManager* GetItemMgr();
DWORD GetNextItemID();


#endif
