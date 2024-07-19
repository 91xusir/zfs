
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
		mpItem[i]=NULL;						//����
		mItemLevel[i]=0;					//���ߵȼ�
		mpDefaultItem[i]=NULL;				//Ĭ��װ��
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
	LOG2("װ��[%d]  �ȼ�[%d]\n",vItemID,vLevel);
	if(vItemID==0)
	{
		return false;
	}

	//������Ƿ����
	SItem* pItem = RS_FindItem(vItemID);
	if (pItem==NULL)
	{
		return false;
	}


	//��������װ��
	switch (pItem->Type)
	{
		case ITEM_TYPE_WEAPON:
			{
				//��ԭ��������ж��
				if(mpItem[WEAPON_1]!=NULL)
				{
					if(mpItem[WEAPON_1]->Id==vItemID && mItemLevel[WEAPON_1]==vLevel)
					{
						return true;
					}
					UnEquip(WEAPON_1);
				}
				

				//���ݸ���
				mpItem[WEAPON_1]= pItem;
				mItemLevel[WEAPON_1]=vLevel;
				mpItem[WEAPON_2]= pItem;
				mItemLevel[WEAPON_2]=vLevel;

				mpMaster->mpAI->SetAttackRange(pItem->Wea.AttackRange);//�ı乥������

				//ͼ�����
				SItemWeapon& rWeapon = pItem->Wea;
				if (rWeapon.Act[vLevel][0]!='\0')
				{//�ǿ��ַ���
					mGraph[WEAPON_1]=GcActorGraph(rWeapon.Act[vLevel]);
					mpMaster->GetGraph()->LinkSon(mGraph[WEAPON_1],rWeapon.CombatLinkSlot);
				}

				if (rWeapon.Act1[vLevel][0]!='\0')
				{//�ǿ��ַ���
					mGraph[WEAPON_2]=GcActorGraph(rWeapon.Act1[vLevel]);
					mpMaster->GetGraph()->LinkSon(mGraph[WEAPON_2],rWeapon.CombatLinkSlot1);
				}
			}
			break;
		case ITEM_TYPE_ARMOR:
			{
				//ͼ�����
				//ԭ����ж��
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
					mpMaster->GetGraph()->UnloadSkin(mpItem[Part]->Armor.Skin[mItemLevel[Part]]);//��ԭ���Ļ���
				}
				else
				{
					if(mpDefaultItem[Part]!=NULL)
					{
						mpMaster->GetGraph()->UnloadSkin(mpDefaultItem[Part]);//��Ĭ�ϵ�װ��ж��
					}
				}
				mpMaster->GetGraph()->LoadSkin(pItem->Armor.Skin[vLevel]);//�����µ�skin
				//���ݸ���
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
				//����Ʒ�����������£�������
				if(!(mGraph[WEAPON_1]==(GcActorGraph*)NULL))
				{//�ò�λ��ͼ��
					mpMaster->GetGraph()->UnLinkSon(mGraph[WEAPON_1]);//������
					mGraph[WEAPON_1]=NULL;//����
				}

				//����Ʒ�����������£�������
				if(!(mGraph[WEAPON_2]==(GcActorGraph*)NULL))
				{//�ò�λ��ͼ��
					mpMaster->GetGraph()->UnLinkSon(mGraph[WEAPON_2]);//������
					mGraph[WEAPON_2]=NULL;//����
				}
				
				//����������
				mpItem[WEAPON_1]=NULL;
				mItemLevel[WEAPON_1]=0;
				mpItem[WEAPON_2]=NULL;
				mItemLevel[WEAPON_2]=0;
			}
		}
		break;
	case HEAD:		//ͷ
	case HAND:		//��
	case BODY:		//����
	case FOOT:		//��
		{
			if(mpItem[vPart]!=NULL)
			{
				//��װ������
				//����Ĭ�ϵ�skin
				//ͼ�����
				mpMaster->GetGraph()->UnloadSkin(mpItem[vPart]->Armor.Skin[mItemLevel[vPart]]);
				if(mpDefaultItem[vPart]!=NULL)
				{
					mpMaster->GetGraph()->LoadSkin(mpDefaultItem[vPart]);
				}
				//���ݸ���
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



