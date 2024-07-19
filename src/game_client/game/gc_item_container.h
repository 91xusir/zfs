
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
		WEAPON_1=0,	//������λ1
		WEAPON_2,	//������λ2
		HEAD,		//ͷ
		HAND,		//��
		BODY,		//����
		FOOT		//��
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
	GcActor*		mpMaster;//����Ʒ������

	//���ϵ���Ʒ
	SItem*			mpItem[MAX_EQUIP_PARTS];		//����
	byte			mItemLevel[MAX_EQUIP_PARTS];	//���ߵȼ�
	const char*		mpDefaultItem[MAX_EQUIP_PARTS];	//Ĭ��װ���ı�ʶ����
	GcActorGraph	mGraph[MAX_EQUIP_PARTS];		//ͼ��
};

#endif//GC_ITEM_CONTAINER_H
