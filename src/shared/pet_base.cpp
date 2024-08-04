#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "string_mapping.h"
#include "pet_base.h"


long PT_GetRecoverGold(SItemID &item)
{
	return 50 + (item.params[PET_PARAM_LEV] * 10);
}

bool GetPetInfoFromItem(SItemID itemPet,float &FHp,float &FMp,float &FAttack,float &FDamage,float &FDodge,float &FArmor,float ElementArmorRate[3],float ElementAttackRate[3])
{
	if (!ItemIsPet(itemPet))
		return false;

	if (ItemIsPetNormal(itemPet))
	{
		//基准生命成长*(1+0.05*锻体等级)*(1+*0.15*融合等级);
		FHp		= (float)itemPet.params[PET_PARAM_GROWTH_HP] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FMp		= (float)itemPet.params[PET_PARAM_GROWTH_MP] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FAttack	= (float)itemPet.params[PET_PARAM_GROWTH_HIT] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FDamage	= (float)itemPet.params[PET_PARAM_GROWTH_ATT] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FDodge	= (float)itemPet.params[PET_PARAM_GROWTH_DODGE] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FArmor	= (float)itemPet.params[PET_PARAM_GROWTH_DEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);

		ElementArmorRate[ELEMENT_WATER] = (float)itemPet.params[PET_PARAM_GROWTH_WATERDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		ElementArmorRate[ELEMENT_FIRE] = (float)itemPet.params[PET_PARAM_GROWTH_FIREDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		ElementArmorRate[ELEMENT_POISON] = (float)itemPet.params[PET_PARAM_GROWTH_POSIONDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);

		ElementAttackRate[ELEMENT_WATER] = (float)itemPet.params[PET_PARAM_GROWTH_WATERDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		ElementAttackRate[ELEMENT_FIRE] = (float)itemPet.params[PET_PARAM_GROWTH_FIREDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		ElementAttackRate[ELEMENT_POISON] = (float)itemPet.params[PET_PARAM_GROWTH_POSIONDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
	}
	else if (ItemIsPetFairy(itemPet))
	{
		//基准生命成长*(1+0.05*锻体等级)*(1+*0.15*进化等级);
		FHp		= (float)itemPet.params[PET_PARAM_GROWTH_HP] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FMp		= (float)itemPet.params[PET_PARAM_GROWTH_MP] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FAttack	= (float)itemPet.params[PET_PARAM_GROWTH_HIT] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FDamage	= (float)itemPet.params[PET_PARAM_GROWTH_ATT] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FDodge	= (float)itemPet.params[PET_PARAM_GROWTH_DODGE] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);
		FArmor	= (float)itemPet.params[PET_PARAM_GROWTH_DEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_FUSE_LEV]);

		ElementArmorRate[ELEMENT_WATER] = (float)itemPet.params[PET_PARAM_GROWTH_WATERDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_EVOLUTION_LEV]);
		ElementArmorRate[ELEMENT_FIRE] = (float)itemPet.params[PET_PARAM_GROWTH_FIREDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_EVOLUTION_LEV]);
		ElementArmorRate[ELEMENT_POISON] = (float)itemPet.params[PET_PARAM_GROWTH_POSIONDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_EVOLUTION_LEV]);

		ElementAttackRate[ELEMENT_WATER] = (float)itemPet.params[PET_PARAM_GROWTH_WATERDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_EVOLUTION_LEV]);
		ElementAttackRate[ELEMENT_FIRE] = (float)itemPet.params[PET_PARAM_GROWTH_FIREDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_EVOLUTION_LEV]);
		ElementAttackRate[ELEMENT_POISON] = (float)itemPet.params[PET_PARAM_GROWTH_POSIONDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]) * (1.f + 0.15f * (float)itemPet.params[PET_PARAM_EVOLUTION_LEV]);
	}
	else if (ItemIsPetSpectre(itemPet))
	{
		//基准生命成长*(1+0.05*锻体等级)
		FHp		= (float)itemPet.params[PET_PARAM_GROWTH_HP] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		FMp		= (float)itemPet.params[PET_PARAM_GROWTH_MP] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		FAttack	= (float)itemPet.params[PET_PARAM_GROWTH_HIT] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		FDamage	= (float)itemPet.params[PET_PARAM_GROWTH_ATT] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		FDodge	= (float)itemPet.params[PET_PARAM_GROWTH_DODGE] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		FArmor	= (float)itemPet.params[PET_PARAM_GROWTH_DEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);

		ElementArmorRate[ELEMENT_WATER] = (float)itemPet.params[PET_PARAM_GROWTH_WATERDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		ElementArmorRate[ELEMENT_FIRE] = (float)itemPet.params[PET_PARAM_GROWTH_FIREDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		ElementArmorRate[ELEMENT_POISON] = (float)itemPet.params[PET_PARAM_GROWTH_POSIONDEF] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);

		ElementAttackRate[ELEMENT_WATER] = (float)itemPet.params[PET_PARAM_GROWTH_WATERDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		ElementAttackRate[ELEMENT_FIRE] = (float)itemPet.params[PET_PARAM_GROWTH_FIREDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
		ElementAttackRate[ELEMENT_POISON] = (float)itemPet.params[PET_PARAM_GROWTH_POSIONDA] / PET_PARAM_COMPUTE_GROWTH * (1.f + 0.05f * (float)itemPet.params[PET_PARAM_ABILITY_LEV]);
	}

	return true;
}

bool CanFusePet(CItemManager* m_pItemMgr,char fuseType,SItemID item1,SItemID item2,SItemID item3)
{
	switch(fuseType)
	{
	case PET_FUSE_TYPE_MERGE:
		{
			SItemID &pet1 = item1;
			SItemID &pet2 = item2;
			SItemID &item = item3;
			SSorption *pItemNeed = (SSorption*)m_pItemMgr->GetItem(item.type);
			if ( ItemIsPetNormal(pet1) && ItemIsPetNormal(pet2) && pItemNeed &&
				CItemManager::GetIndexFromType(item.type) == PET_MERGE_NEED_ITEM &&
				pet1.params[PET_PARAM_FUSE_LEV] == pet2.params[PET_PARAM_FUSE_LEV] &&
				pet1.params[PET_PARAM_LEV] >= 30 && pet2.params[PET_PARAM_LEV] >= 30 /*&&
				pItemNeed->param[0] >= pet1.params[PET_PARAM_FUSE_LEV]+1*/
				)
			{
				return true;
			}
		}
		break;
	case PET_FUSE_TYPE_ABILITY:
		{
			SItemID &pet = item1;
			SItemID &item1 = item2;
			SItemID &item2 = item3;
			if (ItemIsPet(pet) &&
				pet.params[PET_PARAM_ABILITY_LEV] < 10 &&
				CItemManager::GetIndexFromType(item1.type) == PET_ABILITY_NEED_ITEM1
				)
			{
				return true;
			}
		}
		break;
	case PET_FUSE_TYPE_TOBEADS:
		{
			SItemID &pet = item1;
			SItemID &item1 = item2;
			SItemID &item2 = item3;
			if (ItemIsPetSpectre(pet) && 
				pet.params[PET_PARAM_LEV] >= 30 &&
				CItemManager::GetIndexFromType(item1.type) == PET_TOBEADS_NEED_ITEM1
				)
			{
				return true;
			}
		}
		break;
	case PET_FUSE_TYPE_MERGEBEADS:
		{
			SItemID &pet = item1;
			SItemID &item = item2;
			if (ItemIsPetSpectre(pet) && ItemIsSorption(item))
			{
				return true;
			}
		}
		break;
	default:
		return false;
	}
	return false;
}

bool FusePetRate(char fuseType,SItemID item1,SItemID item2,float *fRate/* = 0.f*/)
{
	switch(fuseType)
	{
	case PET_FUSE_TYPE_MERGE:
		{
			SItemID &pet1 = item1;
			SItemID &pet2 = item2;

			int nSucceedRate = 10000.f * ((float)pet1.params[PET_PARAM_LEV] +
				(float)pet2.params[PET_PARAM_LEV]) / 20.f * 0.1f;

			if (fRate)
				*fRate = (float)nSucceedRate / 10000.f;

			if (GetRand(10000) < nSucceedRate)
				return true;
		}
		break;
	case PET_FUSE_TYPE_ABILITY:
		{
			SItemID &pet = item1;

			//max(0.2,1-宠物当前锻体等级*0.1)。
			float fSucceedRate = Max(0.2f, 1.f - (float)pet.params[PET_PARAM_ABILITY_LEV] * 0.1f);

			if (fRate)
				*fRate = fSucceedRate;

			if (GetRand(10000) < fSucceedRate * 10000.f )
				return true;
		}
		break;
	case PET_FUSE_TYPE_TOBEADS:
		{
			SItemID &pet = item1;

			float fSucceedRate = 0.5f;

			if (fRate)
				*fRate = fSucceedRate;

			if (GetRand(10000) < fSucceedRate * 10000.f )
				return true;
		}
		break;
	case PET_FUSE_TYPE_MERGEBEADS:
		{
			if (fRate)
				*fRate = 1.f;
			return true;
		}
		break;
	default:
		return false;
	}
	return false;
}

float GetToBeadsGrade(SItemID petItem,SPet *pPet)
{
	//round((1+int(宠物等级/10)*0.02)*(生命成长/表格生命成长系数+法力成长/表格法力成长系数+)/2,2)
	float grade = (1.f + ((float)petItem.params[PET_PARAM_LEV] / 10.f) * 0.25f) * (
		(float)petItem.params[PET_PARAM_GROWTH_HP] / (pPet->FHp * PET_PARAM_COMPUTE_GROWTH) + 
		(float)petItem.params[PET_PARAM_GROWTH_MP] / (pPet->FMp * PET_PARAM_COMPUTE_GROWTH) ) / 2.f;

	return grade;
}

DWORD GetFusePetMillseconds(char fuseType)
{
	switch(fuseType)
	{
	case PET_FUSE_TYPE_MERGE:
		{
			return 3000;
		}
		break;
	case PET_FUSE_TYPE_ABILITY:
		{
			return 3000;
		}
		break;
	case PET_FUSE_TYPE_TOBEADS:
		{
			return 3000;
		}
		break;
	case PET_FUSE_TYPE_MERGEBEADS:
		{
			return 3000;
		}
		break;
	default:
		return 0;
	}
	return 0;
}

void SetPetHpAndMp(SItemID &pet, SPet *pPet)
{
	float FHp = 0.f;
	float FMp = 0.f;
	float FAttack = 0.f;
	float FDamage = 0.f;
	float FDodge = 0.f;
	float FArmor = 0.f;
	float ElementArmorRate[3];
	float ElementAttackRate[3];

	if (!GetPetInfoFromItem(pet,FHp,FMp,FAttack,FDamage,FDodge,FArmor,ElementArmorRate,ElementAttackRate))
		return;

	SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(pet.params[PET_PARAM_LEV]));
	if(!base) return;

	CAttrValue MaxHp;
	CAttrValue MaxMp;
	MaxHp.Base     = base->FHp * FHp + pPet->AddedHp;
	MaxMp.Base     = base->FMp * FMp + pPet->AddedMp;

	pet.params[PET_PARAM_HP] = MaxHp.GetValue();
	pet.params[PET_PARAM_MP] = MaxMp.GetValue();
}
int GetMergeBeadsLev(SItemID petItem,SPet *pPet)
{

	//int((1号位阴魂珠品级+2号位阴魂珠品级+3号位阴魂珠品级+4号位阴魂珠品级)*
	//	(宠物等级*0.005)+(当前生命成长/表格生命成长+当前法力成长/表格法力成长
	int lev = 
	      (
			(float)petItem.params[PET_PARAM_BEADS_1] / PET_PARAM_COMPUTE_BEADS +
	   		(float)petItem.params[PET_PARAM_BEADS_2] / PET_PARAM_COMPUTE_BEADS +
	   		(float)petItem.params[PET_PARAM_BEADS_3] / PET_PARAM_COMPUTE_BEADS +
	   		(float)petItem.params[PET_PARAM_BEADS_4] / PET_PARAM_COMPUTE_BEADS 
		  ) *

		  ( (float)petItem.params[PET_PARAM_LEV]*0.005f ) +

		  (
			(float)petItem.params[PET_PARAM_GROWTH_HP] / PET_PARAM_COMPUTE_GROWTH / pPet->FHp +
			(float)petItem.params[PET_PARAM_GROWTH_MP] / PET_PARAM_COMPUTE_GROWTH / pPet->FMp /*+
			(float)petItem.params[PET_PARAM_GROWTH_ATT]			/ PET_PARAM_COMPUTE_GROWTH +
			(float)petItem.params[PET_PARAM_GROWTH_DEF]			/ PET_PARAM_COMPUTE_GROWTH +
			(float)petItem.params[PET_PARAM_GROWTH_FIREDA]		/ PET_PARAM_COMPUTE_GROWTH / 4.f +
			(float)petItem.params[PET_PARAM_GROWTH_WATERDA]		/ PET_PARAM_COMPUTE_GROWTH / 4.f +
			(float)petItem.params[PET_PARAM_GROWTH_POSIONDA]	/ PET_PARAM_COMPUTE_GROWTH / 4.f +
			(float)petItem.params[PET_PARAM_GROWTH_FIREDEF]		/ PET_PARAM_COMPUTE_GROWTH / 4.f +
			(float)petItem.params[PET_PARAM_GROWTH_WATERDEF]	/ PET_PARAM_COMPUTE_GROWTH / 4.f +
			(float)petItem.params[PET_PARAM_GROWTH_POSIONDEF]	/ PET_PARAM_COMPUTE_GROWTH / 4.f +
			(float)petItem.params[PET_PARAM_GROWTH_HIT]			/ PET_PARAM_COMPUTE_GROWTH +
			(float)petItem.params[PET_PARAM_GROWTH_DODGE]		/ PET_PARAM_COMPUTE_GROWTH*/
		  );
		  /// 2.f ) * (0.2f+);

	return lev;

	//int((1号位阴魂珠品级+2号位阴魂珠品级+3号位阴魂珠品级+4号位阴魂珠品级+
	//	(生命成长资质+法力成长资质+物伤成长资质+物防成长资质+火伤成长资质/4+
	//	水伤成长资质/4+毒伤成长资质/4+火防成长资质/4+水防成长资质/4+毒防成长资质/4+命中成长资质+闪避成长资质)/2)
	//	*(0.2+宠物等级*0.02))

}

SItemID CreatePetItem(SItemID item, CItemManager* pItemMgr)
{
	if (!pItemMgr)
		return ItemID_CreateInvalid();

	DWORD type = item.type;

	item.cLock = 0;

	item.paramCount = PET_PARAM_SIZE;

	int nItemIndex = CItemManager::GetIndexFromType(type);
	SItemPet *pItemPet = (SItemPet*)pItemMgr->GetItem(type);
	if(!pItemPet) return ItemID_CreateInvalid();
	SPet *pPet = g_TablePet.FindPet(pItemPet->petID);
	if(!pPet) return ItemID_CreateInvalid();
	item.params[PET_PARAM_LEV] = pPet->Lev;

	SBaseValue *base = g_TableBaseValue.Find(GetRealLevel(item.params[PET_PARAM_LEV]));
	if(!base) return ItemID_CreateInvalid();

	item.name = pPet->Name;
	item.params[PET_PARAM_EXP]	= 0;
	item.params[PET_PARAM_AI]	= 0;

	item.params[PET_PARAM_ABILITY_LEV]	= 0;

	item.params[PET_PARAM_FUSE_LEV] = 0;
	item.params[PET_PARAM_EVOLUTION_LEV] = 0;
	item.params[PET_PARAM_LINKUP_LEV] = 0;

	item.params[PET_PARAM_GROWTH_HP] = pPet->FHp * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_MP] = pPet->FMp * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_ATT] = pPet->FDamage * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_DEF] = pPet->FArmor * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_HIT] = pPet->FAttack * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_DODGE] = pPet->FDodge * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;

	item.params[PET_PARAM_GROWTH_WATERDA] = pPet->ElementAttackRate[0] * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_FIREDA] = pPet->ElementAttackRate[1] * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_POSIONDA] = pPet->ElementAttackRate[2] * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;

	item.params[PET_PARAM_GROWTH_WATERDEF] = pPet->ElementArmorRate[0] * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_FIREDEF] = pPet->ElementArmorRate[1] * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;
	item.params[PET_PARAM_GROWTH_POSIONDEF] = pPet->ElementArmorRate[2] * (float)GetRand(PET_CREATE_GROWTH_MAX,PET_CREATE_GROWTH_MIN)/ 100 * PET_PARAM_COMPUTE_GROWTH;

	SetPetHpAndMp(item,pPet);

	item.params[PET_PARAM_STATE] = 0;
	size_t count = 0;
    size_t skillCnt = PET_PARAM_PASSIVE_1;
	for (int i=PET_PARAM_PASSIVE_1;i<=PET_PARAM_PASSIVE_6;i++)
	{
		if (count < pPet->vPassive.size() && count < pPet->vPassiveRate.size()  && GetProb(pPet->vPassiveRate[count]))
		{
			item.params[skillCnt] = pPet->vPassive[count];
			skillCnt++;
		}
		else
		{
			item.params[i] = 0;
		}
		count++;
	}

	count = 0;
	skillCnt = PET_PARAM_SKILL_1;
	for (int i=PET_PARAM_SKILL_1;i<=PET_PARAM_SKILL_6;i++)
	{
		if (count < pPet->vSkill.size() && count < pPet->vSkillRate.size() && GetProb(pPet->vSkillRate[count]))
		{
			item.params[skillCnt] = pPet->vSkill[count];
			skillCnt++;
		}
		else
		{
			item.params[i] = 0;
		}
		count++;
	}

	for (int i=PET_PARAM_BEADS_1;i<=PET_PARAM_BEADS_4;i++)
	{
		item.params[i] = 0;
	}

	if (ItemIsPetSpectre(item))
	{
		item.params[PET_PARAM_LINKUP_LEV] = GetMergeBeadsLev(item,pPet);
	}

	return item;
}
