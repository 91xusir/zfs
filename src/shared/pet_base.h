#ifndef _PET_BASE
#define _PET_BASE

struct SPet;
#include "region_client.h"
#include "item_base.h"

enum PetType
{
	PET_TYPE_NULL = 0,
	PET_TYPE_NORMAL,
	PET_TYPE_FAIRY,
	PET_TYPE_SPECTRE
};

const unsigned char PET_CTRL_CALLBACK		= 1;
const unsigned char PET_CTRL_ATTACK			= 2;
const unsigned char PET_CTRL_SYNERGY		= 3;
const unsigned char PET_CTRL_PASSIVE		= 4;
const unsigned char PET_CTRL_DEF			= 5;
const unsigned char PET_CTRL_USE_SKILL		= 6;
const unsigned char PET_CTRL_SET_DEFSKILL	= 7;
const unsigned char PET_CTRL_CALLOUT		= 8;
const unsigned char PET_CTRL_CHANGE_NAME	= 9;
const unsigned char PET_CTRL_DELETE_PET		= 10;

const unsigned char PET_FUSE_TYPE_MERGE			= 1;
const unsigned char PET_FUSE_TYPE_ABILITY		= 2;
const unsigned char PET_FUSE_TYPE_TOBEADS		= 3;
const unsigned char PET_FUSE_TYPE_MERGEBEADS	= 4;

const unsigned int PET_MERGE_NEED_ITEM		= 6221;		//6221	融合石	
const unsigned int PET_ABILITY_NEED_ITEM1	= 6222;		//6222	灵兽果	
const unsigned int PET_ABILITY_NEED_ITEM2	= 6223;		//6223	固灵丹	
const unsigned int PET_TOBEADS_NEED_ITEM1	= 6224;		//6224	聚灵水
const unsigned int PET_TOBEADS_NEED_ITEM2	= 6225;		//6225	固阴丹	
const unsigned int PET_BEADS				= 6226;		//6226	阴魂珠	

const unsigned char PET_CATCH_ERROR_START				= 0;
const unsigned char PET_CATCH_ERROR_SUCCEED				= 1;
const unsigned char PET_CATCH_ERROR_LIFETIME			= 2;
const unsigned char PET_CATCH_ERROR_BECATCHED			= 3;
const unsigned char PET_CATCH_ERROR_BEATTACKED			= 4;
const unsigned char PET_CATCH_ERROR_DIE					= 5;
const unsigned char PET_CATCH_ERROR_CULLEV				= 6;
const unsigned char PET_CATCH_ERROR_CANCEL				= 7;

// pet data in item
const int PET_PARAM_SIZE	= 36;
const int PET_PARAM_LEV		= 0;
const int PET_PARAM_EXP		= 1;
const int PET_PARAM_HP		= 2;
const int PET_PARAM_MP		= 3;
const int PET_PARAM_AI		= 4;

const int PET_PARAM_ABILITY_LEV			= 5;	// 锻体等级

const int PET_PARAM_FUSE_LEV			= 6;	// 融合等级
const int PET_PARAM_EVOLUTION_LEV		= 6;	// 进化等级
const int PET_PARAM_LINKUP_LEV			= 6;	// 融阴等级

const int PET_PARAM_GROWTH_HP			= 7;	// 宠物血量上限的系数调节
const int PET_PARAM_GROWTH_MP			= 8;	// 宠物法力上限的系数调节
const int PET_PARAM_GROWTH_ATT			= 9;	// 宠物攻击值的系数调节
const int PET_PARAM_GROWTH_DEF			= 10;	// 宠物防御值的系数调节
const int PET_PARAM_GROWTH_HIT			= 11;	// 宠物命中值的系数调节
const int PET_PARAM_GROWTH_DODGE		= 12;	// 宠物闪避值的系数调节
const int PET_PARAM_GROWTH_WATERDA		= 13;	// 宠物水伤值的系数调节
const int PET_PARAM_GROWTH_FIREDA		= 14;	// 宠物火伤值的系数调节
const int PET_PARAM_GROWTH_POSIONDA		= 15;	// 宠物毒伤值的系数调节
const int PET_PARAM_GROWTH_WATERDEF		= 16;	// 宠物水防值的系数调节
const int PET_PARAM_GROWTH_FIREDEF		= 17;	// 宠物火防值的系数调节
const int PET_PARAM_GROWTH_POSIONDEF	= 18;	// 宠物毒防值的系数调节

const int PET_PARAM_PASSIVE_1			= 19;	// 被动技能
const int PET_PARAM_PASSIVE_2			= 20;	
const int PET_PARAM_PASSIVE_3			= 21;	
const int PET_PARAM_PASSIVE_4			= 22;	
const int PET_PARAM_PASSIVE_5			= 23;	
const int PET_PARAM_PASSIVE_6			= 24;	

const int PET_PARAM_SKILL_1				= 25;	// 主动技能
const int PET_PARAM_SKILL_2				= 26;		
const int PET_PARAM_SKILL_3				= 27;		
const int PET_PARAM_SKILL_4				= 28;		
const int PET_PARAM_SKILL_5				= 29;		
const int PET_PARAM_SKILL_6				= 30;		

const int PET_PARAM_BEADS_1				= 31;		
const int PET_PARAM_BEADS_2				= 32;		
const int PET_PARAM_BEADS_3				= 33;		
const int PET_PARAM_BEADS_4				= 34;
const int PET_PARAM_STATE				= 35;	// 宠物的状态,0为标准状态,1为被骑或合体状态

const float PET_PARAM_COMPUTE_GROWTH	= 10000.f;
const float PET_PARAM_COMPUTE_BEADS		= 100.f;

const int	PET_CREATE_GROWTH_MIN		= 85;
const int	PET_CREATE_GROWTH_MAX		= 115;

const int USE_PET_BUSY		= 1000*3;

const int MAX_PET_MODEL_CHANGE = 7;

const unsigned short PET_SKILL_DROPRIDE	= 6501;
const unsigned short PET_SKILL_RIDE		= 6502;
const unsigned short PET_SKILL_LINKUP	= 6503;

const unsigned short CATCH_PET_SKILL	= 114;

const unsigned short PET_NAME_CHAR_NUM_MAX	= 16;
const unsigned short PET_NAME_CHAR_NUM_MIN	= 2;

const unsigned char PET_FUSE_FINISH_NONE	= 0;
const unsigned char PET_FUSE_FINISH_DELETE	= 1;



SItemID CreatePetItem(SItemID item, CItemManager* pItemMgr);
bool	GetPetInfoFromItem(SItemID itemPet,float &FHp,float &FMp,float &FAttack,float &FDamage,float &FDodge,float &FArmor,float ElementArmorRate[3],float ElementAttackRate[3]);
bool	CanFusePet(CItemManager* m_pItemMgr,char fuseType,SItemID item1,SItemID item2,SItemID item3);
bool	FusePetRate(char fuseType,SItemID item1,SItemID item2,float *fRate = NULL);
float	GetToBeadsGrade(SItemID petItem,SPet *pPet);
int		GetMergeBeadsLev(SItemID petItem,SPet *pPet);
DWORD	GetFusePetMillseconds(char fuseType);
long	PT_GetRecoverGold(SItemID &item);
void	SetPetHpAndMp(SItemID &pet, SPet *pPet);

struct SPet
{
	short  Id;
	short  Type;
	float  AttackRange;
	float  AttackSpeed;
	float  MoveSpeed;

	std::string Name;
	short ModelId;
	char  Lev;
	float FHp;
	float FMp;
	float FAttack;
	float FDamage;
	float FRDDamage;  
	float FDodge;
	float FArmor;
	short NeedCulLev;

	float AddedHp;
	float AddedMp;
	float AddedAttack;
	float AddedDamage;
	float AddedDodge;
	float AddedArmor;
	float DamageFloat;

	bool  isride;

	vector<DWORD>  vSkill;	
	vector<float>  vSkillRate;	
	vector<DWORD>  vPassive;
	vector<float>  vPassiveRate;	
	DWORD dwItemID;
	DWORD dwCatchMillseconds;

	vector<long>  vParams1;

	long  ModelLevel[MAX_PET_MODEL_CHANGE];
	long  ModelChange[MAX_PET_MODEL_CHANGE];
	float AttackRangeChange[MAX_PET_MODEL_CHANGE];
	float ElementArmorRate[MAX_ELEMENT_ATTR];
	float ElementAttackRate[MAX_ELEMENT_ATTR];
};


#endif