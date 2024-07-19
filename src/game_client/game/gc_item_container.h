
#include "gc_actor_graph.h"


#ifndef	GC_ITEM_CONTAINER_H 
#define GC_ITEM_CONTAINER_H

class GcActor;
struct SItem;

class CItemContainer
{
public:
	enum EItemTypes
	{
		UNKNOW_TYPE=0,
		WEAPON_TYPE,
		ARMOR_TYPE,
	};

	enum EEquipParts
	{
		WEAPON_1=0,	//武器部位1
		WEAPON_2,	//武器部位2
		HEAD,		//头
		HAND,		//手
		BODY,		//身体
		FOOT		//脚
	};

	enum
	{
		MAX_EQUIP_PARTS=6,
	};


	CItemContainer(GcActor* vpMaster);
	virtual ~CItemContainer();

	void			Reset();
	void			EquipDefaultItem(EEquipParts vPart,const char vpIDName[]);
	bool			Equip(short int vItemID,byte vLevel);
	bool			UnEquip(EEquipParts vPart);
	void			Display(EEquipParts vPart);
	bool			IsEquip(EEquipParts vPart);

	bool GetItem(EEquipParts vPart,SItem*& vrpItem,byte& vrLevel);
	GcActorGraph*	GetGraph(EEquipParts vPart);

private:
	GcActor*		mpMaster;//该物品的宿主

	//身上的物品
	SItem*			mpItem[MAX_EQUIP_PARTS];		//道具
	byte			mItemLevel[MAX_EQUIP_PARTS];	//道具等级
	const char*		mpDefaultItem[MAX_EQUIP_PARTS];	//默认装备的标识名称
	GcActorGraph	mGraph[MAX_EQUIP_PARTS];		//图象
};

#endif//GC_ITEM_CONTAINER_H
