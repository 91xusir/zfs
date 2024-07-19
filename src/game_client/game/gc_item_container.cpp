
#include "gc_item_container.h"
#include "region_client.h"
#include "gc_ai_processor.h"
#include "gc_actor.h"

CItemContainer::CItemContainer(GcActor* vpMaster)
:mpMaster(vpMaster)
{
	Reset();
}

void CItemContainer::Reset()
{
	for(int i=0;i<MAX_EQUIP_PARTS;i++)
	{
		mpItem[i]=NULL;						//数据
		mItemLevel[i]=0;					//道具等级
		mpDefaultItem[i]=NULL;				//默认装备
	}
}

CItemContainer::~CItemContainer()
{
}

void CItemContainer::EquipDefaultItem(EEquipParts vPart,const char vpIDName[])
{
	if(vPart<0 ||  vPart>=MAX_EQUIP_PARTS)
	{
		return ;
	}

	mpDefaultItem[vPart]=vpIDName;
}

bool CItemContainer::Equip(short int vItemID,byte vLevel)
{
	LOG2("装备[%d]  等级[%d]\n",vItemID,vLevel);
	if(vItemID==0)
	{
		return false;
	}

	//该物件是否存在
	SItem* pItem = RS_FindItem(vItemID);
	if (pItem==NULL)
	{
		return false;
	}


	//根据类型装备
	switch (pItem->Type)
	{
		case ITEM_TYPE_WEAPON:
			{
				//将原来的武器卸下
				if(mpItem[WEAPON_1]!=NULL)
				{
					if(mpItem[WEAPON_1]->Id==vItemID && mItemLevel[WEAPON_1]==vLevel)
					{
						return true;
					}
					UnEquip(WEAPON_1);
				}
				

				//数据更新
				mpItem[WEAPON_1]= pItem;
				mItemLevel[WEAPON_1]=vLevel;
				mpItem[WEAPON_2]= pItem;
				mItemLevel[WEAPON_2]=vLevel;

				mpMaster->mpAI->SetAttackRange(pItem->Wea.AttackRange);//改变攻击距离

				//图象更新
				SItemWeapon& rWeapon = pItem->Wea;
				if (rWeapon.Act[vLevel][0]!='\0')
				{//非空字符串
					mGraph[WEAPON_1]=GcActorGraph(rWeapon.Act[vLevel]);
					mpMaster->GetGraph()->LinkSon(mGraph[WEAPON_1],rWeapon.CombatLinkSlot);
				}

				if (rWeapon.Act1[vLevel][0]!='\0')
				{//非空字符串
					mGraph[WEAPON_2]=GcActorGraph(rWeapon.Act1[vLevel]);
					mpMaster->GetGraph()->LinkSon(mGraph[WEAPON_2],rWeapon.CombatLinkSlot1);
				}
			}
			break;
		case ITEM_TYPE_ARMOR:
			{
				//图象更新
				//原来的卸下
				EEquipParts Part;
				switch(pItem->Armor.ArmorPart)
				{
				case PART_HEAD:
					Part=HEAD;
					break;
				case PART_BODY:
					Part=BODY;
					break;
				case PART_HAND:
					Part=HAND;
					break;
				case PART_FOOT:
					Part=FOOT;
					break;
				default:
					return false;
					break;
				} 

				if(mpItem[Part]!=NULL)
				{
					if(mpItem[Part]->Id==vItemID && mItemLevel[Part]==vLevel)
					{
						return true;
					}
					mpMaster->GetGraph()->UnloadSkin(mpItem[Part]->Armor.Skin[mItemLevel[Part]]);//将原来的换下
				}
				else
				{
					if(mpDefaultItem[Part]!=NULL)
					{
						mpMaster->GetGraph()->UnloadSkin(mpDefaultItem[Part]);//将默认的装备卸下
					}
				}
				mpMaster->GetGraph()->LoadSkin(pItem->Armor.Skin[vLevel]);//换上新的skin
				//数据更新
				mpItem[Part]=pItem;
				mItemLevel[Part]=vLevel;
			}
			break;
		default:
			return false;
			break;
	}

	return true;
}





bool CItemContainer::UnEquip(EEquipParts vPart)
{
	if(vPart<0 ||  vPart>=MAX_EQUIP_PARTS)
	{
		return false;
	}

	switch(vPart)
	{
	case WEAPON_1:
	case WEAPON_2:
		{
			if(mpItem[WEAPON_1]!=NULL)
			{
				//将物品从人物上拿下，并销毁
				if(!(mGraph[WEAPON_1]==(GcActorGraph*)NULL))
				{//该部位有图象
					mpMaster->GetGraph()->UnLinkSon(mGraph[WEAPON_1]);//拿下来
					mGraph[WEAPON_1]=NULL;//销毁
				}

				//将物品从人物上拿下，并销毁
				if(!(mGraph[WEAPON_2]==(GcActorGraph*)NULL))
				{//该部位有图象
					mpMaster->GetGraph()->UnLinkSon(mGraph[WEAPON_2]);//拿下来
					mGraph[WEAPON_2]=NULL;//销毁
				}
				
				//将数据销毁
				mpItem[WEAPON_1]=NULL;
				mItemLevel[WEAPON_1]=0;
				mpItem[WEAPON_2]=NULL;
				mItemLevel[WEAPON_2]=0;
			}
		}
		break;
	case HEAD:		//头
	case HAND:		//手
	case BODY:		//身体
	case FOOT:		//脚
		{
			if(mpItem[vPart]!=NULL)
			{
				//将装备换下
				//换上默认的skin
				//图象更新
				mpMaster->GetGraph()->UnloadSkin(mpItem[vPart]->Armor.Skin[mItemLevel[vPart]]);
				if(mpDefaultItem[vPart]!=NULL)
				{
					mpMaster->GetGraph()->LoadSkin(mpDefaultItem[vPart]);
				}
				//数据更新
				mpItem[vPart]=NULL;
				mItemLevel[vPart]=0;
			}
		}
		break;
	default:
		return false;
		break;
	}
	return true;
}


bool CItemContainer::GetItem(EEquipParts vPart,SItem*& vrpItem,byte& vrLevel)
{
	if(vPart<0 ||  vPart>=MAX_EQUIP_PARTS)
	{
		return false;
	}
	vrpItem=mpItem[vPart];
	vrLevel=mItemLevel[vPart];
	return true;
}


bool			CItemContainer::IsEquip(EEquipParts vPart)
{
	if(vPart<0 ||  vPart>=MAX_EQUIP_PARTS)
	{
		return false;
	}

	if(mpItem[vPart]==NULL)
	{
		return false;
	}

	return true;
}

GcActorGraph*	CItemContainer::GetGraph(EEquipParts vPart)
{
	if(vPart<0 ||  vPart>=MAX_EQUIP_PARTS)
	{
		return NULL;
	}
	return &(mGraph[vPart]);
}



