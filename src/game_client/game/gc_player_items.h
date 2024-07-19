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
	IO_EnchantItem,		// ��ħ
	IO_Decompose,		// �ֽ�
    IO_Lock
};

class CItemContainer : public CItemContainerBase
{
public:
	CItemContainer(CItemManager *pManager, SCreature*pCre, GcActor* pActor);
	virtual ~CItemContainer();
	GcActorGraph*		GetGraph(EEquipParts vPart);
	virtual bool		CheckEquip(EEquipParts vPart, SItemID& item, SCreature &cre);	// ��������װ������Ʒ�������Ƿ����
	virtual void		_Equip(SItemID& id, EEquipParts vPart);
	virtual bool		_UnEquip(EEquipParts vPart, bool bAddToBag = false);
	virtual bool		UnEquipAll(bool bAddToBag = false);
    virtual bool        CheckUse(SItemID& item, SCreature &cre) const;
	void				UpdateGraph();			// ������������е��ߵ���û��ˢ��ͼ��ʱʹ��
	void				Run(unsigned long dwMillisecond);
    void	            EquipDefaultItem(EEquipParts vPart, const char vpIDName[]);

	virtual void		OnAfterEquip(SItemID& item, EEquipParts vPart);
	virtual void		OnAfterUnEquip(EEquipParts vPart, SItemID& itemOld);

	void				ExchangeHandWeapon();
	void				SetEquipItemValid(EEquipParts part, bool bValid);

	static int			CalcHintLineCount(const char* str);
	static RtString 	GenerateAttribString(SItemBase* pItemClass, SAttribAddtion &attrib, BYTE rate, bool bItemRelated = true);

	// ��ԣ�ɾ�����ƶ���װ����ж�£��ӣ��𣬺�ҩƿ�����ޣ����ᣬ�������ߵ�ʹ��
	bool				OperateItem_CheckUIContent(SItemID &item, EItemOperate op);

	void				OnResetGraph();

    void                SetEquipData(SItemID &item, EEquipParts part);
    void                ChangeModel(SItemID &itemOrig, SItemID &itemNew, EEquipParts part);

	ETradableResult		CanItemTrade(char bCommonBag, int page, int i, int j);
	ETradableResult		CanItemTrade(DWORD itemID);

	char				SwitchSpareWeapon();

private:
    const char*		    mpDefaultItem[MAX_EQUIP_PARTS];		// Ĭ��װ��������
	void				_UpdateGraph(EEquipParts part);		// ����ĳ����λ��ͼ��
	GcActor*			m_pMaster;

	void				WeaponPlayPose(const char* szLinkName);

	// ���ڱ������һ�ε�װ��(Graph���Ǳ��ִ˱���)����Ҫ����ͨ������ˢ�¹�����װ����������뵱ǰ����������ݴ˱�����жװ
	SItemID				m_nLastItemID[MAX_EQUIP_PARTS];
};

#define ITEM_HINT_BLANK_HEIGHT		10
#define ITEM_HINT_LINE_HEIGHT		16
#define ITEM_HINT_WIDTH				160

RtString GenerateItemHtmlHint(int &outLineCount, SItemID &itemID, GcActor *pPlayer, CBag *pBag = NULL, int page = -1, int i = -1, int j = -1,  float fPriceRate = -1.0f, UINT price = 0, bool bCountAll = false, float fTaxRate = -1.0f, int tTimeLimit=0,int OtherPrice = 0);

CItemManager* GetItemMgr();
DWORD GetNextItemID();


#endif
