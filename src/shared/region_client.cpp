#include "region_client.h"
#include "string_mapping.h"
#include "linux_util.h"

#ifdef GAME_CLIENT
#include "gc_include.h"
#endif

 
char var[100] = {
	1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,
	51,52,53,54,55,56,57,58,59,60,
	61,62,63,64,65,66,67,68,69,70,
	71,72,73,74,75,76,77,78,79,80,
	81,82,83,84,85,86,87,88,89,90,
	91,92,93,94,95,96,97,98,99,100,
};

//------------add start by tony 05.06.16-------------------------//
//-�ܻ�������
std::vector<long> g_tcexpRate;
std::vector<long> g_tcrndNpcID;
std::vector<long> g_tcrndItemID;
std::vector<long> g_tcrndMonsterID;
int g_tcTotalCircle;
//------------add end   by tony 05.06.16-------------------------//

// ---------------------------------------------------------------------------
// rand and probability
// ---------------------------------------------------------------------------
static long HoldRand = 1L;
void rtSrand(unsigned int seed)
{
	HoldRand = (long)seed;
}

// return 0 through 32767.
int rtRand()
{
	return(((HoldRand = HoldRand * 214013L + 2531011L) >> 16) & 0x7fff);
}

// min is 1/1,000,000
const int PROB_MIN = 1000000;
bool GetProb(double probability)
{
	if(probability < 1.0/PROB_MIN) return false;
	// should find a better way
	long ran = (rtRand() << 16) | rtRand();
	return ran%PROB_MIN < probability*PROB_MIN;
}

int GetRand(int max, int min)
{
//	if(max == min) return 0;
//	return rtRand()%(max-min) + min;
	if(max < min) return 0;
	return rtRand()%(max+1-min) + min;
}

SCreature::SCreature()
{
	ModelId = 0;
	HeadModelId = 0;
	ComplexModel = false;
	Cloth = 0;
	ClothLev = 0;
	ArmGuard = 0;
	ArmGuardLev = 0;
	Weapon = 0;
	WeaponLev = 0;
	Shoes = 0;
	ShoesLev = 0;
	actorID = 0;
	
	IsShieldMpR = false;

	Xianyu = 0;
	Guanfu = 0;
	Manzu = 0;
	Yishi = 0;
	Yijun = 0;
	Lianmeng = 0;

	XianyuLev = 0;
	GuanfuLev = 0;
	ManzuLev = 0;
	YishiLev = 0;
	YijunLev = 0;
	LianmengLev = 0;

	//-----Tianh----2009.11.20--
	Cul = 0;  
	CulExp = 0;
	Soul = 0;           
	Contribute = 0;     
	Point = 0;          
	Record = 0;         
	UnionRecord = 0;    
	JobRecord = 0;     
	BornTag = 0;
	Sex = 0;
	Vigour = 0;
	VigourPoint = 0;
	PlayerOnlineTime = 0;
	GradeLev = 0;
	HeadImageID = 0; //����ͷ��
	WarScores = 0;

	//------------------------ --

	Lev = 0;
	Exp = 0;
	Hp  = 0;
	Mp = 0;
	EnP = 0;
	AttrPoint  = 0;
	SkillPoint = 0;
	SkillClearCnt = 0;

	RateExp.Base = 100;
	RateGold.Base = 1.0;

    Metier    = 0;
	MetierLev = 0;
    Faction = 3;
	DamageRate = 1.0;
    Exploit = 0;
	Pk = 0;
	Credit = 0;

	AbiMove = 0;
	AbiDodge = 0;
	AbiUseSkill = 0;
	AbiNormalAttack = 0;
	AbiStateResist = 0;
	AbiStateHit = 0;
    AbiBeAttack = 0;

	Hate = 0;
	HateRate = 100;
	PhyDmgAbsorbRate = 0;
	bAdult = false;
	RelivePostion = 0;

	ShieldHp = 0;
	ShieldMpR = 0;

	CriticalDamage = 0;     // ��ͨ���������˺�
	CriticalDamageR = 0;    // ��ͨ������������,ע���ʼ����2��
	SkillCriRate = 0;       // ���ܻ���������
	SkillCriDamage = 0;     // �����˺��ӳ�
	SkillCriDamageRate = 0; // ���ܱ����˺����� 

	AbsorptionHpR = 0;

	nLoginNowTime = 0;  //���߿�ʼ��ʱ
	m_IHpMpChange = NULL;
};

void SCreature::SerializeToDB(long reason,CG_CmdPacket &cmd)
{
	cmd << Name << Metier << MetierLev << Lev << Faction <<Sex;
	cmd << Exp << Cul << GradeLev <<AttrPoint << SkillPoint << VigourPoint << Pk << Credit;
	cmd << Con << Str << Dex << Int << Hit;
	cmd << Contribute<< Point<< Record<< UnionRecord<< JobRecord<< BornTag<< Sex<< Exploit<<actorID;

}

void SCreature::Serialize(char type,CG_CmdPacket *cmd)
{
    int i;
	switch(type)
	{
	case NET_UPDATE_ALL:
	case NET_CRE_UPDATE_ATTR_ALL:
		// model info
		// *cmd << 
		*cmd << Lev << Exp << Name << Metier << MetierLev << Faction << AttrPoint << SkillPoint << VigourPoint << Cul << GradeLev;
		*cmd << Con << Str << Dex << Int << Hit;
		*cmd << Hp << MaxHp << Mp << MaxMp << EnP << MaxEnP;
		*cmd << Attack << DamageMin << DamageMax << Dodge << Armor << ReCriticalHit << ReSkillCritical << ReFaint << ReParry;
		*cmd << DamageRate << AttackRange << AttSpeed << MovSpeed;
		*cmd << Exploit << Pk << Credit << HeadImageID<<actorID;
		*cmd << Soul << Sex << RelivePostion<< Exploit;
		*cmd << Contribute<< Point<< Record<< UnionRecord<< JobRecord<< BornTag;

        for (i=0; i<MAX_ELEMENT_ATTR; i++)
        {
			*cmd << ElementDamage[i];
            *cmd << ElementAbsorb[i];
			*cmd << ElementArmor[i];
        }
		break;

	case NET_CRE_UPDATE_MODEL:
		*cmd << ModelId;
		if(ComplexModel)
		{
			*cmd << HeadModelId;
			*cmd << Cloth << ClothLev << ArmGuard << ArmGuardLev;
			*cmd << Shoes << ShoesLev << Weapon << WeaponLev;
		}
		break;

	case NET_CRE_UPDATE_ATTR_SIMPLE:
		*cmd << Hp << MaxHp << Mp << MaxMp;
		*cmd << Lev << Exp << AttrPoint << SkillPoint << VigourPoint << MovSpeed << Cul << GradeLev << Pk; 
		*cmd << AbiMove << AbiNormalAttack;
		break;
    
	case NET_CRE_UPDATE_ATTR_ELEMENT_ARMOR:
        for (i=0; i<MAX_ELEMENT_ATTR; i++)
        {
            *cmd << ElementAbsorb[i];
        }
        break;
	case NET_CRE_UPDATE_PET_INFO:

		*cmd << Lev << Exp << MovSpeed << AttSpeed << Name; 
		*cmd << Hp << MaxHp << Mp << MaxMp;
		*cmd << Attack << DamageMin << DamageMax << Dodge << Armor;
		*cmd << AbiMove << AbiNormalAttack;

		for (i=0; i<MAX_ELEMENT_ATTR; i++)
		{
			*cmd << ElementDamage[i];
			*cmd << ElementArmor[i];
		}
		break;
	}
}

bool SCreature::AddBaseStr(long v,bool ChangeBody)
{ 
	long point = v;
    if(AttrPoint <= 0) return false;
    Str.Base += point;
    AttrPoint -= point;
    RecomputeAttr(ChangeBody);
    return true;
}
bool SCreature::AddBaseCon(long v,bool ChangeBody)
{ 
	long point = v;
    if(AttrPoint <= 0) return false;
    Con.Base += point;
    AttrPoint-= point;
    RecomputeAttr(ChangeBody);
    return true;
}
bool SCreature::AddBaseDex(long v,bool ChangeBody)
{ 
	long point = v;
    if(AttrPoint <= 0) return false;
    Dex.Base += point;
    AttrPoint -= point;
    RecomputeAttr(ChangeBody);
    return true;
}
bool SCreature::AddBaseInt(long v,bool ChangeBody)
{ 
	long point = v;
    if(AttrPoint <= 0) return false;
    Int.Base += point;
    AttrPoint -= point;
    RecomputeAttr(ChangeBody);
    return true;
}
bool SCreature::AddBaseHit(long v,bool ChangeBody)
{ 
	long point = v;
    if(AttrPoint <= 0) return false;
    Hit.Base += point;
    AttrPoint -= point;
    RecomputeAttr(ChangeBody);
    return true;
}

void SCreature::AddAddedStr(short v)
{
	Str.Temp += v;
    RecomputeAttr();
}

void SCreature::AddAddedCon(short v)
{
    Con.Temp += v;
    RecomputeAttr();
}

void SCreature::AddAddedDex(short v)
{
    Dex.Temp += v;
    RecomputeAttr();
}

void SCreature::AddAddedInt(short v)
{
    Int.Temp += v;
    RecomputeAttr();
}

void SCreature::AddAddedHit(short v)
{
    Hit.Temp += v;
    RecomputeAttr();
}

void SCreature::AddAddedStrR(float v)
{
    Str.Rate += v;
    RecomputeAttr();
}

void SCreature::AddAddedConR(float v)
{
    Con.Rate += v;
    RecomputeAttr();
}

void SCreature::AddAddedDexR(float v)
{
    Dex.Rate += v;
    RecomputeAttr();
}

void SCreature::AddAddedIntR(float v)
{
    Int.Rate += v;
    RecomputeAttr();
}

void SCreature::AddAddedHitR(float v)
{
    Hit.Rate += v;
    RecomputeAttr();
}


/*Tianh 09.12.15*��ʽ�䶯���������ֶ�������*/
void SCreature::RecomputeAttr(bool ChangeBody)
{
	int realLev = GetRealLevel(Lev);

	//if(Lev > 10)
	//{
	//	ElementArmor[ELEMENT_WATER].Base  = (Lev - 10) * 0.1 * ELE_ARMOR_SCALE;
	//	ElementArmor[ELEMENT_FIRE].Base   = (Lev - 10) * 0.1 * ELE_ARMOR_SCALE;
	//	ElementArmor[ELEMENT_POISON].Base = (Lev - 10) * 0.1 * ELE_ARMOR_SCALE;
	//}	
	switch(Metier)
	{
    default:
	case METIER_WARRIOR:  //����//��̨
		DamageRate		    = 1+StrPower.GetValue()/1000.0f;        //�˺�
		MaxHp.Base		    = Con.GetValue()*25.00+Str.GetValue()*2;//HP
		MaxMp.Base		    = Int.GetValue()*8.00;                 //MP
		Attack.Base		    = Hit.GetValue()*2.00;		           //����
		Dodge.Base		    = Dex.GetValue()*2.00;                 //����
		DamageMin.Base	    = (Str.GetValue()*1)*0.9;          //��С�˺�
	    DamageMax.Base	    = (Str.GetValue()*1)*1.1;          //����˺�
		Armor.Base          = Con.GetValue()*8.00;                 //����   
		MaxEnP.Base         = (realLev-16)*2+100;                  //����ֵ//ŭ��
		RestoreHp.Base      = Con.GetValue()*0.2;                  //Ѫ���ָ�
		RestoreMp.Base      = Int.GetValue()*0.2;                  //ħ���ָ�
		RestoreEnP.Base     = (realLev-15)*0.02+1;                  //�����ָ�
		MovSpeed.Base		= BASE_MOVE_SPEED;                     //�ƶ��ٶ�

		ElementArmor[ELEMENT_FIRE].Base = Int.GetValue()*2;
		ElementArmor[ELEMENT_WATER].Base = Int.GetValue()*2;
		ElementArmor[ELEMENT_POISON].Base = Int.GetValue()*2;
		
		break;

	case METIER_HUNTER:  //����
		DamageRate		    = 1 + DexPower.GetValue()/1000.0f; 
		MaxHp.Base		    = Con.GetValue()*24.00+Str.GetValue()*2;
		MaxMp.Base		    = Int.GetValue()*12.00;
		Attack.Base		    = Hit.GetValue()*2.00;
		Dodge.Base		    = Dex.GetValue()*2.50;
		DamageMin.Base	    = (Dex.GetValue()*1)*0.7;
		DamageMax.Base	    = (Dex.GetValue()*1)*1.3;
		Armor.Base          = Con.GetValue()*2.50;
		MaxEnP.Base         = (realLev-16)*2+100;                  //����ֵ
		RestoreHp.Base      = Con.GetValue()*0.2;                  //Ѫ���ָ�
		RestoreMp.Base      = Int.GetValue()*0.2;                  //ħ���ָ�
		RestoreEnP.Base     = (realLev-15)*0.02+1;                 //�����ָ�
		MovSpeed.Base		= BASE_MOVE_SPEED;

		ElementArmor[ELEMENT_FIRE].Base = Int.GetValue()*2;
		ElementArmor[ELEMENT_WATER].Base = Int.GetValue()*2;
		ElementArmor[ELEMENT_POISON].Base = Int.GetValue()*2;

		break;

	case METIER_TAOIST: //ʥ��  //�置
		if (ChangeBody)//�ڱ��ǵ�״̬��
		{
			DamageRate		    = 1 + IntPower.GetValue()/1000.0f; 
			MaxHp.Base		    = Con.GetValue()*25.00+Str.GetValue()*2;
			MaxMp.Base		    = Int.GetValue()*18.00;
			Attack.Base		    = Hit.GetValue()*2.00;
			Dodge.Base		    = Dex.GetValue()*2.00;
			DamageMin.Base	    = (Int.GetValue()*1)*0.95;
			DamageMax.Base	    = (Int.GetValue()*1)*1.05;
			Armor.Base          = Con.GetValue()*8.50;
			MaxEnP.Base         = (realLev-16)*2+100;                  //����ֵ
			RestoreHp.Base      = Con.GetValue()*0.2;                  //Ѫ���ָ�
			RestoreMp.Base      = Int.GetValue()*0.2;                  //ħ���ָ�
			RestoreEnP.Base     = (realLev-15)*0.02+1;                 //�����ָ�
			MovSpeed.Base		= BASE_MOVE_SPEED+20;


			ElementArmor[ELEMENT_FIRE].Base = Int.GetValue()*3;
			ElementArmor[ELEMENT_WATER].Base = Int.GetValue()*3;
			ElementArmor[ELEMENT_POISON].Base = Int.GetValue()*3;
			break;

		}
		else
		{
			DamageRate		    = 1 + IntPower.GetValue()/1000.0f; 
			MaxHp.Base		    = Con.GetValue()*24.00+Str.GetValue()*2;
			MaxMp.Base		    = Int.GetValue()*18.00;
			Attack.Base		    = Hit.GetValue()*2.00;
			Dodge.Base		    = Dex.GetValue()*2.00;
			DamageMin.Base	    = (Int.GetValue()*0.6)*0.95;
			DamageMax.Base	    = (Int.GetValue()*0.6)*1.05;
			Armor.Base          = Con.GetValue()*5.50;
			MaxEnP.Base         = (realLev-16)*2+100;                  //����ֵ
			RestoreHp.Base      = Con.GetValue()*0.2;                  //Ѫ���ָ�
			RestoreMp.Base      = Int.GetValue()*0.2;                  //ħ���ָ�
			RestoreEnP.Base     = (realLev-15)*0.02+1;                 //�����ָ�
			MovSpeed.Base		= BASE_MOVE_SPEED;


			ElementArmor[ELEMENT_FIRE].Base = Int.GetValue()*3;
			ElementArmor[ELEMENT_WATER].Base = Int.GetValue()*3;
			ElementArmor[ELEMENT_POISON].Base = Int.GetValue()*3;
			break;
		}


	case METIER_WIZARD: //��ɽ
		DamageRate		    = 1 + IntPower.GetValue()/1000.0f; 
		MaxHp.Base		    = Con.GetValue()*21.00+Str.GetValue()*2;
		MaxMp.Base		    = Int.GetValue()*25.00;
		Attack.Base		    = Hit.GetValue()*2.00;
		Dodge.Base		    = Dex.GetValue()*2.00;
		DamageMin.Base	    = (Int.GetValue()*0.6)*0.95;
		DamageMax.Base	    = (Int.GetValue()*0.6)*1.05;
		Armor.Base          = Con.GetValue()*2.00;
		MaxEnP.Base         = (realLev-16)*2+100;                  //����ֵ
		RestoreHp.Base      = Con.GetValue()*0.2;                  //Ѫ���ָ�
		RestoreMp.Base      = Int.GetValue()*0.2;                  //ħ���ָ�
		RestoreEnP.Base     = (realLev-15)*0.02+1;                 //�����ָ�
		MovSpeed.Base		= BASE_MOVE_SPEED;
		
		
		ElementArmor[ELEMENT_FIRE].Base = Int.GetValue()*3;
		ElementArmor[ELEMENT_WATER].Base = Int.GetValue()*3;
		ElementArmor[ELEMENT_POISON].Base = Int.GetValue()*3;
	
		break;
	}


	AtrribPoint * info = g_TableAtrribPoint.FindPet(Metier);
	if(info)
	{
		float fOld = WarScores;
		//((1+��ҵȼ�*��ҵȼ���ֵ)*(����*���˷�ֵ*���*�����ֵ+����*���˷�ֵ*���*�����ֵ+ˮ��*ˮ�˷�ֵ*ˮ��*ˮ����ֵ+����*���˷�ֵ*��������*������ֵ)
		//*(5/12+����*���з�ֵ))
		//*(1+����һ����*����һ����ֵ+����һ����*����һ����ֵ)*(�����ٶ�*�����ٶȷ�ֵ)*(1+��Ѫ*��Ѫ��ֵ)+����/(24/0.7)*������ֵ*(1+��*�񵲷�ֵ)
		//*(1+�ֿ�����һ����*�ֿ�����һ����ֵ+�ֿ�����һ����*����һ����ֵ)
		//+��Ѫ*��Ѫ��ֵ
		WarScores = ((1+Lev*info->PlayerLev)*((DamageMin.GetValue() + DamageMax.GetValue())/2*(info->PhysicalDamage)*Armor.GetValue()*(info->PhysicalDefense)
			+ElementDamage[ELEMENT_FIRE].GetValue()*(info->FireDamage)*ElementArmor[MAX_ELEMENT_ATTR].GetValue()*(info->FireDefense)+ElementDamage[ELEMENT_WATER].GetValue()*(info->WaterDamage)*ElementArmor[ELEMENT_WATER].GetValue()*(info->WaterDefense)+ElementDamage[ELEMENT_POISON].GetValue()*(info->PoisonDamage)*ElementArmor[ELEMENT_POISON].GetValue()*(info->PoisonDefense))
			*(5/12+Attack.GetValue()*(info->Attack)))
			*(1+CriticalHit.GetValue()*(info->SlayR))+SkillCriRate*(info->SkillCriRate)*(AttSpeed.GetValue()*(info->DamagesSpeed))*(1+AbsorbHp.GetValue()*(info->AbsorbHp))
			+GetMaxHp()/(24/0.7)*(info->PlayerHP)*(1+ReParry.GetValue()*(info->ReParry))*(1+ReCriticalHit.GetValue()*(info->ReCriticalHit)+ReSkillCritical.GetValue()*(info->ReSkillCritical))
			+RestoreHp.GetValue()*(info->RestoreHp);

		if (NULL != m_IHpMpChange)
		{
			m_IHpMpChange->OnWarScoresChange(fOld, WarScores);
		}
	}
}

void SCreature::AddResist(short v)
{
    int i;
    for (i=0; i<MAX_ELEMENT_ATTR; i++)
    {
        ElementArmor[i].Temp += v;
    }
}

void SCreature::AddResistR(float v)
{
    int i;
    for (i=0; i<MAX_ELEMENT_ATTR; i++)
    {
        ElementArmor[i].Rate += v;
    }
}

float SCreature::GetSkillCriRate(int enemyLev,float ReSkillCr/*=0.0*/)
{
	if(enemyLev == 0) return 0.0;
	
	float rate = 0;
	int m1=0,m2=0;
	switch(Metier)
	{
		//case METIER_WARRIOR:
		//	m1 = Str.GetValue();
		//	m2 = StrPower.GetValue();
		//	break;

		//case METIER_HUNTER:
		//	m1 = Dex.GetValue();
		//	m2 = DexPower.GetValue();
		//	break;

		//case METIER_WIZARD:
		//	m1 = Int.GetValue();
		//	m2 = IntPower.GetValue();
		//	break;

		//case METIER_TAOIST:
		//	m1 = Int.GetValue();
		//	m2 = IntPower.GetValue();
		//	break;

		//Tianh ȥ�����������ԶԱ����ʵ�Ӱ��
		case METIER_WARRIOR:
			m1 = 0;
			m2 = 0;
			break;

		case METIER_HUNTER:
			m1 = 0;
			m2 = 0;
			break;

		case METIER_WIZARD:
			m1 = 0;
			m2 = 0;
			break;

		case METIER_TAOIST:
			m1 = 0;
			m2 = 0;
			break;

		default:
			return 0.0;
	}
	//Tianh ���ﱣ�ֲ���ֻ�ǰ����������ȫ��Ϊ0ֵ
    if(((m1/(m1+enemyLev*2.0+30))*0.1+(m2/(m2+enemyLev*10.0))*0.2+m1/2000 + SkillCriRate/100.0-ReSkillCr/100.0)<=0)
         return 0.0;
	else
	     return (m1/(m1+enemyLev*2.0+30))*0.1+(m2/(m2+enemyLev*10.0))*0.2+m1/2000 + SkillCriRate/100.0-ReSkillCr/100.0;
}


// ---------------------------------------------------------------------------------
// Table Pet 
// ---------------------------------------------------------------------------------
CRS_Pet::CRS_Pet() {}

CRS_Pet::~CRS_Pet() {}

bool CRS_Pet::Load(const char *File)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	char *pStrTmp = NULL;
	char *sep = ";";
	char *k = NULL;
	for(j++; j<Total; j++)
	{
		SPet tmp;
		tmp.Name		= csv[j]["������"].Str();
		tmp.Id			= csv[j]["ID"].Long();
		tmp.Type		= csv[j]["����"].Short();
		tmp.dwItemID	= csv[j]["��Ӧ����"].Long();

		tmp.ModelId		= csv[j]["ģ��ID"].Short();
		tmp.AttackSpeed = csv[j]["�����ٶ�"].Float();
		tmp.MoveSpeed   = csv[j]["�ƶ��ٶ�"].Float();
		tmp.AttackRange = csv[j]["��������"].Float();
		tmp.Lev	        = csv[j]["LEVEL"].Int();

		tmp.FHp          = csv[j]["HPϵ��"].Float();
		tmp.FMp			 = csv[j]["MPϵ��"].Float();
		tmp.FAttack		 = csv[j]["����ϵ��"].Float();
		tmp.FDamage		 = csv[j]["�˺�ϵ��"].Float();
		tmp.FDodge       = csv[j]["����ϵ��"].Float();
		tmp.FArmor       = csv[j]["����ϵ��"].Float();

		tmp.AddedHp      = csv[j]["HP����"].Float();
		tmp.AddedMp		 = csv[j]["MP����"].Float();
		tmp.AddedAttack	 = csv[j]["��������"].Float();
		tmp.AddedDamage  = csv[j]["�˺�����"].Float();
		tmp.AddedDodge   = csv[j]["���㸽��"].Float();
		tmp.AddedArmor   = csv[j]["��������"].Float();

		tmp.AttackRange  = csv[j]["��������"].Float();
		tmp.MoveSpeed    = csv[j]["�ƶ��ٶ�"].Float();
		tmp.AttackSpeed  = csv[j]["�����ٶ�"].Float();
		tmp.DamageFloat  = csv[j]["�˺�����"].Float();

		tmp.isride       = csv[j]["�Ƿ����"].Bool();
		tmp.dwCatchMillseconds = csv[j]["��׽ʱ��"].Long();
		tmp.NeedCulLev = csv[j]["��Ϊ����ȼ�"].Short();

		sscanf(csv[j]["����׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[0],&tmp.ModelChange[0],&tmp.AttackRangeChange[0]);
		sscanf(csv[j]["��һ�׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[1],&tmp.ModelChange[1],&tmp.AttackRangeChange[1]);
		sscanf(csv[j]["�ڶ��׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[2],&tmp.ModelChange[2],&tmp.AttackRangeChange[2]);
		sscanf(csv[j]["�����׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[3],&tmp.ModelChange[3],&tmp.AttackRangeChange[3]);
		sscanf(csv[j]["���Ľ׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[4],&tmp.ModelChange[4],&tmp.AttackRangeChange[4]);
		sscanf(csv[j]["����׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[5],&tmp.ModelChange[5],&tmp.AttackRangeChange[5]);
		sscanf(csv[j]["�����׶�"].Str(),"%d,%d,%f",&tmp.ModelLevel[6],&tmp.ModelChange[6],&tmp.AttackRangeChange[6]);

		if(sscanf(csv[j]["������ǿ"].Str(),"%f,%f,%f",&tmp.ElementArmorRate[0],
			&tmp.ElementArmorRate[1],&tmp.ElementArmorRate[2]) != 3)
		{
			RtCoreLog().Info("CRS_Pet::Load: parse ������ǿ error.\n");
			// return false;
		}

		if (sscanf(csv[j]["���Թ����˺�"].Str(),"%f,%f,%f",&tmp.ElementAttackRate[0],
			&tmp.ElementAttackRate[1],&tmp.ElementAttackRate[2]) != 3)
		{
			RtCoreLog().Info("CRS_Pet::Load: parse ���Թ����˺� error.\n");
		}

		pStrTmp			= (char*)csv[j]["��������"].Str();
		if (pStrTmp)
		{
			k = strtok(pStrTmp, sep);
			while (k && strlen(k)>0)
			{
				tmp.vSkill.push_back(atoi(k));
				k = strtok(NULL, sep);
			}
		}
		pStrTmp			= (char*)csv[j]["��������"].Str();
		if (pStrTmp)
		{
			k = strtok(pStrTmp, sep);
			while (k && strlen(k)>0)
			{
				tmp.vPassive.push_back(atoi(k));
				k = strtok(NULL, sep);
			}
		}

		pStrTmp			= (char*)csv[j]["�������ܸ���"].Str();
		if (pStrTmp)
		{
			k = strtok(pStrTmp, sep);
			while (k && strlen(k)>0)
			{
				tmp.vSkillRate.push_back(atof(k));
				k = strtok(NULL, sep);
			}
		}
		pStrTmp			= (char*)csv[j]["�������ܸ���"].Str();
		if (pStrTmp)
		{
			k = strtok(pStrTmp, sep);
			while (k && strlen(k)>0)
			{
				tmp.vPassiveRate.push_back(atof(k));
				k = strtok(NULL, sep);
			}
		}

		pStrTmp			= (char*)csv[j]["����1"].Str();
		if (pStrTmp)
		{
			k = strtok(pStrTmp, sep);
			while (k && strlen(k)>0)
			{
				tmp.vParams1.push_back(atol(k));
				k = strtok(NULL, sep);
			}
		}

		m_table.push_back(tmp);
	}
	return true;
}

SPet *CRS_Pet::FindPet(int Id)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].Id == Id) return &m_table[i];
	}
	return NULL;
}
// ---------------------------------------------------------------------------------
// global define
// ---------------------------------------------------------------------------------
CRS_Popularity  g_TablePopularity;
CRS_Exp			g_TableExp;
CRS_UserActor	g_TableUserActor;
CRS_CreModel    g_TableCreModel;
CRS_Npc			g_TableNpc;
CRS_Pet			g_TablePet;
CRS_SceneInfo   g_TableScene;
CRS_Task		g_TableTask;
CRS_HeadModel	g_TableHeadModel;
CRS_CharImage   g_TableCharImage;
CRS_BaseValue   g_TableBaseValue;
CRS_FuBen       g_TableFB;
CRS_FBReward    g_TableFBReword;
CRS_LineSys		g_TableLineSys;
CRS_MainStory   g_TableMainStory;
CRS_TaskGroup   g_TaskGroup;
CRS_NPCTalkText	g_TableNpcRandomTalk;				//NPC���˵������
CRS_Problem		g_TableProblem;						//�ش����������
CRS_Activity	g_TableActivity;					//�������
CRS_AtrribPoint g_TableAtrribPoint;
CRS_UnionIcon   g_TableUnionIcon;
CRS_ShopCenter  g_TableShopCenter;

bool RS_Load()
{
	if(!g_TableExp.Load(R(RES_TABLE_EXP)))
	{
		RtCoreLog().Info("startup: load exp table failed!");
		return false;
	}
	if (!g_TablePopularity.Load(R(RES_TABLE_HONORS)))
	{
		RtCoreLog().Info("startup: load Honors table failed!");
		return false;
	}
	if(!g_TableUserActor.Load(R(RES_TABLE_USER)))
	{
		RtCoreLog().Info("startup: load user actor table failed!");
		return false;
	}
	if(!g_TableCreModel.Load(R(RES_TABLE_MODEL)))
	{
		RtCoreLog().Info("startup: load creature model table failed!");
		return false;
	}
	if(!g_TableHeadModel.Load(R(RES_TABLE_HEAD_MODEL)))
	{
		RtCoreLog().Info("startup: load creature head model table failed!");
		return false;
	}
	if(!g_TableCharImage.Load(R(RES_TABLE_HEAD_IMAGE)))
	{
		RtCoreLog().Info("startup: load creature 2D Image table failed!");
		return false;
	}
	if(!g_TableNpc.Load(R(RES_TABLE_NPC)))
	{
		RtCoreLog().Info("startup: load npc table failed!");
		return false;
	}
	if(!g_TablePet.Load(R(RES_TABLE_PET)))
	{
		RtCoreLog().Info("startup: load pet table failed!");
		return false;
	}
	if(!g_TableBaseValue.Load(R(RES_TABLE_BASE)))
	{
		RtCoreLog().Info("startup: load base table failed!");
		return false;
	}

	if(!g_TableTask.Load(R(RES_TABLE_TASK)))
	{
		RtCoreLog().Info("startup: load task table failed!");
		return false;
	}
	if(!g_TableTask.LoadFbTask(R(RES_TABLE_FBTASK)))
	{
		RtCoreLog().Info("startup: load fb task table failed!");
		return false;
	}
    if(!g_TableScene.Load(R(RES_TABLE_SCENE)))
    {
        RtCoreLog().Info("startup: load scene information table failed!");
        return false;
    }
	if(!g_TableFB.Load(R(RES_TABLE_FBLIST)))
	{
		RtCoreLog().Info("startup: load fb information table failed!");
		return false;
	}
	if(!g_TableFB.LoadTrigger(R(RES_TABLE_TRIGGER)))
	{
	RtCoreLog().Info("startup: load trigger information table failed!");
	return false;
	}
	if(!g_TableFBReword.Load(R(RES_TABLE_FBITEM)))
	{
		RtCoreLog().Info("startup: load fb item table failed!");
		return false;
	}
	if (!g_TableLineSys.Load(R(RES_TABLE_LINESYS)))
	{
		RtCoreLog().Info("startup: load linesys table failed!");
		return false;
	}
	if (!g_TableNpcRandomTalk.Load(R(RES_TABLE_NPC_TALK)))
	{
		RtCoreLog().Info("startup: load NPC Talk table failed!");
		return true;
	}

	if (!g_TableMainStory.Load(R(RES_TABLE_MAINSTORY)))
	{
		RtCoreLog().Info("startup: load main story table failed!");
		return true;
	}

	if (!g_TaskGroup.Load(R(RES_TABLE_TASKGROUP)))
	{
		RtCoreLog().Info("startup: load task group table failed!");
		return true;
	}
	
	if (!g_TableAtrribPoint.Load(R(RES_TABLE_ATPOINT)))
	{
		RtCoreLog().Info("startup: load AtrribPoint failed!");
		return true;
	} 

	if (!g_TableProblem.Load(R(RES_TABLE_PROBLEM)))
	{
		RtCoreLog().Info("startup: load problem table failed!");
		return true;
	}
	if (!g_TableActivity.Load(R(RES_TABLE_ACTIVITY)))
	{
		RtCoreLog().Info("startup: load activity table failed!");
		return true;
	}

	if (!g_TableUnionIcon.Load(R(RES_TABLE_GUILD_PIC)))
	{
		RtCoreLog().Info("startup: load UnionIcon table failed!");
		return true;
	}

	if (!g_TableShopCenter.Load(R(RES_TABLE_SHOP_CENTER)))
	{
		RtCoreLog().Info("startup: load ShopCenter table failed!");
		return true;
	}

	return true;
}

void RS_Clear()
{
}

// ---------------------------------------------------------------------------------
// Table Exp    Tianh
// ---------------------------------------------------------------------------------
CRS_Exp::CRS_Exp() {}

CRS_Exp::~CRS_Exp() {}

bool CRS_Exp::Load(const char *file)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);
	
	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		m_expTable.push_back(csv[j]["�������辭��ֵ"].DWord());
		m_CulexpTable.push_back(csv[j]["��Ϊ�ȼ��������辭��ֵ"].DWord());
		m_CulTable.push_back(csv[j]["���������Ϊ"].DWord());	
	}
	return true;
}


DWORD CRS_Exp::GetNextLevExp(long lev)
{
	if(lev <=0 || lev > MAX_LEVEL) return 0xffffffff;
	return m_expTable[lev-1];
}

DWORD CRS_Exp::GetNextLevCulExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_CulexpTable[lev];
}

DWORD CRS_Exp::GetLevCulTable(long lev)
{
	if(lev <=0 || lev > MAX_LEVEL) return 0xffffffff;
	return m_CulTable[lev-1];
}

//--------------------------------------------------------------------------
//
//----------------------add by Tianh ����-----------------------------------

CRS_Popularity::CRS_Popularity() {}

CRS_Popularity::~CRS_Popularity() {}

bool CRS_Popularity::Load(const char *file)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();

	for(j++; j<Total; j++)
	{
		m_Xainyu.push_back(csv[j]["����"].Long());;
		m_Guanfu.push_back(csv[j]["�񿤹ٸ�"].Long());;
		m_Manzu.push_back(csv[j]["��Ǽ����"].Long());;
		m_YiShi.push_back(csv[j]["������ʿ"].Long());;
		m_Yijun.push_back(csv[j]["�������"].Long());;
		m_Lianmeng.push_back(csv[j]["��ħ����"].Long());;
	}
	return true;
}

DWORD CRS_Popularity::GetNextXainyuExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_Xainyu[lev];
}

DWORD CRS_Popularity::GetNextGuanfuExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_Guanfu[lev];
}

DWORD CRS_Popularity::GetNextManzuExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_Manzu[lev];
}

DWORD CRS_Popularity::GetNextYiShiExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_YiShi[lev];
}

DWORD CRS_Popularity::GetNextYijunExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_Yijun[lev];
}

DWORD CRS_Popularity::GetNextLianmengExp(long lev)
{
	if(lev <0 || lev >= MAX_LEVEL) return 0xffffffff;
	return m_Lianmeng[lev];
}

// ---------------------------------------------------------------------------------
// Table FB Reward
// ---------------------------------------------------------------------------------
bool CRS_FBReward::Load(const char *file)
{
	SFBReward tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		tmp.id			= csv[j]["ID"].Long();
		tmp.exp			= csv[j]["����ֵ"].Long();
		tmp.gold        = csv[j]["��Ǯ"].Long();
		tmp.itemId      = csv[j]["����ID"].Long();
		tmp.bindType    = csv[j]["�󶨷�ʽ"].Long();
		tmp.color       = csv[j]["Ʒ��"].Str();
		tmp.colorRate   = csv[j]["Ʒ�ʱ���"].Long();
		tmp.colorParam1 = csv[j]["Ʒ�ʲ���1"].Long();
		tmp.colorParam2 = csv[j]["Ʒ�ʲ���2"].Long();
		
		m_table.push_back(tmp);
	}
	return true;
}


// ---------------------------------------------------------------------------------
// Table User Actor
// ---------------------------------------------------------------------------------
CRS_UserActor::CRS_UserActor() {}

CRS_UserActor::~CRS_UserActor() {}

bool CRS_UserActor::Load(const char *file)
{
	SUserActor tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		int i = 0;
		tmp.Id			= csv[j]["ID"].Long();
		tmp.ModelId		= csv[j]["ģ��ID"].Short();
		tmp.Metier		= csv[j]["ְҵ"].Long();
		tmp.MetierLev	= csv[j]["תְ����"].Long();
		tmp.MetierName	= csv[j]["ְҵ����"].Str();
		tmp.Sex			= csv[j]["�Ա�"].Long();
		tmp.Age			= csv[j]["����"].Long();
		tmp.Desc		= csv[j]["˵��"].Str();
		// tmp.bCreate     = csv[j]["�ɴ���"].Long()!=0;
		tmp.bZCreate    = csv[j]["�ܿɴ���"].Long()!=0;
		tmp.bSCreate    = csv[j]["�̿ɴ���"].Long()!=0;
		tmp.bZlCreate   = csv[j]["�����ɴ���"].Long()!=0;
		tmp.Level       = csv[j]["�ȼ�"].Long();
		tmp.BaseStr     = csv[j]["����"].Short();
		tmp.BaseDex     = csv[j]["����"].Short();
		tmp.BaseHit     = csv[j]["��׼"].Short();
		tmp.BaseCon     = csv[j]["����"].Short();
		tmp.BaseInt     = csv[j]["����"].Short();
		

		for(int i=0; i<MAX_CREATE_ITEM_NUM; i++)
			tmp.Item[i] = 0;
		if(sscanf(csv[j]["Я������"].Str(),"%d,%d,%d,%d,%d,%d,%d,%d",
			&tmp.Item[0],&tmp.Item[1],&tmp.Item[2],&tmp.Item[3],
			&tmp.Item[4],&tmp.Item[5],&tmp.Item[6],&tmp.Item[7]) != 8)
		{
			// RtCoreLog().Info("CRS_UserActor::Load: parse Я������ error.\n");
			// return false;
		}

		for(int i=0; i<MAX_CREATE_BODY_NUM; i++)
			tmp.BodyItem[i] = 0;
		if(sscanf(csv[j]["װ������"].Str(),"%d,%d,%d,%d,%d,%d,%d,%d",
			&tmp.BodyItem[0],&tmp.BodyItem[1],&tmp.BodyItem[2],&tmp.BodyItem[3],
			&tmp.BodyItem[4],&tmp.BodyItem[5],&tmp.BodyItem[6],&tmp.BodyItem[7]) != 8)
		{
			// RtCoreLog().Info("CRS_UserActor::Load: parse װ������ error.\n");
			// return false;
		}


		for(int i=0; i<MAX_CREATE_SKILL_NUM; i++)
			tmp.Skill[i] = 0;
		if(sscanf(csv[j]["Я������"].Str(),"%d,%d,%d,%d,%d,%d,%d,%d",
			&tmp.Skill[0],&tmp.Skill[1],&tmp.Skill[2],&tmp.Skill[3],
			&tmp.Skill[4],&tmp.Skill[5],&tmp.Skill[6],&tmp.Skill[7]) != 8)
		{
			// RtCoreLog().Info("CRS_UserActor::Load: parse Я������ error.\n");
			// return false;
		}
		m_table.push_back(tmp);
	}
	return true;
}

SUserActor *CRS_UserActor::FindUserActor(char Metier,char MetierLev)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].Metier == Metier && m_table[i].MetierLev == MetierLev)
			return &m_table[i];
	}
	return NULL;
}

SUserActor *CRS_UserActor::FindUserActor(int Id)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].Id == Id)
			return &m_table[i];
	}
	return NULL;
}

// ---------------------------------------------------------------------------------
// Table fu list
// ---------------------------------------------------------------------------------

bool SPeriodicity::NowIsTime()
{
    time_t tNow = time(NULL);
    struct tm* t = localtime(&tNow);

    if (iStartYear!=0)
    {
        if ((t->tm_year+1970)<iStartYear) return false;
        if ((t->tm_mon+1)<iStartMonth) return false;
        if ((t->tm_mday)<iStartDay) return false;
    }

    if (iEndYear!=0)
    {
        if ((t->tm_year+1970)>iEndYear) return false;
        if ((t->tm_mon+1)>iEndMonth) return false;
        if ((t->tm_mday)>iEndDay) return false;
    }

    switch (cycMode)
    {
    case 0: // �κ�ʱ��
    case 1: // ÿ��
        break;
    case 2: // ÿ��
        if (cycWeek[t->tm_wday]==0) return false;
        break;
    case 3: // ÿ��
        if (cycMonth[0]<(t->tm_mon+1) || cycMonth[1]>(t->tm_mon+1)) return false;
        break;
    }

    int iSecond = t->tm_hour*3600 + t->tm_min*60 + t->tm_sec;
    if (iStartSecond)
    {
        if (iStartSecond<iSecond) return false;
    }

    if (iEndSecond)
    {
        if (iEndSecond>iSecond) return false;
    }

    return true;
}
bool CRS_FuBen::GetTriggerKey(const char *str,vector<STaskKey> &key)
{
	const char *sepLine = "\x0a\x0d";
	// const char *sep = ",;";

	char *pStrTmp,*k;
	pStrTmp = (char *)str;
	k = strtok(pStrTmp, sepLine);
	if(k && strlen(k)>0)
	{
		while (k)
		{
			char buf[10][128];
			memset(buf,0,10*128);
			sscanf(k,"%s %s %s %s %s %s %s %s %s %s",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9]);

			STaskKey tkey;
			tkey.key = buf[0];
			tkey.param[0] = buf[1];
			tkey.param[1] = buf[2];
			tkey.param[2] = buf[3];
			tkey.param[3] = buf[4];
			tkey.param[4] = buf[5];
			tkey.param[5] = buf[6];
			tkey.param[6] = buf[7];
			tkey.param[7] = buf[8];
			tkey.param[8] = buf[9];

			key.push_back(tkey);
			k = strtok(NULL, sepLine);
		}
	}	
	return true;
}
bool CRS_FuBen::LoadTrigger(const char *File)
{
	STrigger tmp;
	int j;
	RtCsv csv;
	char *pStrTmp = NULL;
	char delim[] = ";";	
	int counter = 0;
	char *k = NULL;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		tmp.id				= csv[j]["ID"].Int();
		tmp.active			= csv[j]["�Ƿ񼤻�"].Int();
		tmp.fbID			= csv[j]["��������ID"].Int();
		tmp.name			= csv[j]["����"].Str();
		tmp.process			= csv[j]["������Ľ���ֵ"].Int();
		tmp.type			= csv[j]["����"].Int();
		tmp.opendoorMain	= csv[j]["��Ҫ�������"].Int();	
		tmp.close			= csv[j]["��ʹ�ر�"].Int();	
		tmp.maxUseNum		= csv[j]["���ʹ�ô���(0����)"].Int();		
		tmp.UseTime			= csv[j]["�ٴδ������ʱ��"].Int();		
		tmp.StartTime		= csv[j]["����ʱ��"].Int();	
		tmp.startKey.clear();
		GetTriggerKey(csv[j]["�����������"].Str(),tmp.startKey);
		tmp.startKeyRemove.clear();
		GetTriggerKey(csv[j]["��������ɾ��"].Str(),tmp.startKeyRemove);
		tmp.startKeyAdd.clear();
		GetTriggerKey(csv[j]["������������"].Str(),tmp.startKeyAdd);

		if (tmp.type == 1 || tmp.type == 2)
		{
			for (int i = 0;i<MAX_TRIGGER_OBJ;i++)
			{
				tmp.block[i]	= csv[j]["�赲���"].Int();
			}
		}else if (tmp.type == 3)
		{
			counter = 0;
			pStrTmp = (char *)csv[j]["�赲���"].Str();
			k = strtok(pStrTmp, delim);
			if (k && strlen(k)>0)
			{
				while (k && counter<MAX_TRIGGER_OBJ)
				{
					tmp.block[counter] = atoi(k);
					counter++;
					k = strtok(NULL, delim);
				}
				while( counter < MAX_TRIGGER_OBJ )
				{
					tmp.block[counter] = 0;
					counter++;
				}
			}
		}

		counter = 0;
		pStrTmp = (char *)csv[j]["�赲�ص�"].Str();
		if(pStrTmp)
		{		
			k = strtok(pStrTmp, delim);
			if( (k != NULL) && (strlen(k) > 0) )
			{
				while(k != NULL)
				{
					if(sscanf(k, "%d,%d,%d", &tmp.blockPos[counter].x, &tmp.blockPos[counter].y,&tmp.blockDir[counter]) != 3)
					{
						RtCoreLog().Info("�����赲�ص������ʽ����\n");
						return false;
					}
					k = strtok(NULL, delim);
					counter++;
				}
			}
		}
		tmp.blockCount = counter;

		counter = 0;
		pStrTmp = (char *)csv[j]["Ҫ�رյĻ���"].Str();
		k = strtok(pStrTmp, delim);
		if (k && strlen(k)>0)
		{
			while (k && counter<MAX_TRIGGER_OBJ)
			{
				tmp.closeTrigger[counter] = atoi(k);
				counter++;
				k = strtok(NULL, delim);
			}
			tmp.closeTriggerNum = counter;
			while( counter < MAX_TRIGGER_OBJ )
			{
				tmp.closeTrigger[counter] = 0;
				counter++;
			}
		}
		if ( counter == 0 )
			tmp.closeTriggerNum = 0;

		counter = 0;
		pStrTmp = (char *)csv[j]["�������"].Str();
		k = strtok(pStrTmp, delim);
		if (k && strlen(k)>0)
		{
			while (k && counter<MAX_TRIGGER_OBJ)
			{
				tmp.trigger[counter] = atoi(k);
				counter++;
				k = strtok(NULL, delim);
			}
			tmp.triggerNum = counter;
			while( counter < MAX_TRIGGER_OBJ )
			{
				tmp.trigger[counter] = 0;
				counter++;
			}
		}
		if ( counter == 0 )
			tmp.triggerNum = 0;

		counter = 0;
		pStrTmp = (char *)csv[j]["���������λ�úͷ���"].Str();
		if(pStrTmp)
		{		
			k = strtok(pStrTmp, delim);
			if( (k != NULL) && (strlen(k) > 0) )
			{
				while(k != NULL)
				{
					if(sscanf(k, "%d,%d,%d", &tmp.triggerPos[counter].x, &tmp.triggerPos[counter].y, &tmp.triggerDir[counter]) != 3)
					{
						RtCoreLog().Info("���������λ�������ʽ����\n");
						return false;
					}
					k = strtok(NULL, delim);
					counter++;
				}
			}
		}
		counter = 0;
		pStrTmp = (char *)csv[j]["�������"].Str();
		k = strtok(pStrTmp, delim);
		if (k && strlen(k)>0)
		{
			while (k && counter<MAX_TRIGGER_OBJ)
			{
				tmp.opendoor[counter] = atoi(k);
				counter++;
				k = strtok(NULL, delim);
			}
			tmp.opendoorNum = counter;
			while( counter < MAX_TRIGGER_OBJ )
			{
				tmp.opendoor[counter] = 0;
				counter++;
			}
		}
		if ( counter == 0 )
			tmp.opendoorNum = 0;

		counter = 0;
		pStrTmp = (char *)csv[j]["���������λ�úͷ���"].Str();
		if(pStrTmp)
		{		
			k = strtok(pStrTmp, delim);
			if( (k != NULL) && (strlen(k) > 0) )
			{
				while(k != NULL)
				{
					if(sscanf(k, "%d,%d,%d", &tmp.opendoorPos[counter].x, &tmp.opendoorPos[counter].y, &tmp.opendoorDir[counter]) != 3)
					{
						RtCoreLog().Info("���������λ�������ʽ����\n");
						return false;
					}
					k = strtok(NULL, delim);
					counter++;
				}
			}
		}
		m_trigger[tmp.id] = tmp;
	}
	return true;
}
bool CRS_FuBen::Load(const char *file)
{
	SFuBen tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{

		tmp.id			= csv[j]["ID"].Long();
		

		//Ҫƴ��·��//add by fox
		static const char* pTaskPath=NULL;
		static CIniVariable Temp(GetConfigBoot(),"ConfigInfo","TaskDirectory",&pTaskPath);
		static string TaskPath(pTaskPath);
		tmp.html        = TaskPath + csv[j]["����"].Str();
		tmp.name        = csv[j]["����"].Str();
		tmp.script      = csv[j]["�ű�"].Str();
		tmp.sceneFile   = csv[j]["�����ļ�"].Str();
		tmp.faction     = csv[j]["��Ӫ"].Long();

		if(sscanf(csv[j]["�ȼ�Ҫ��"].Str(),"%d,%d,%d,%d,%d",
			&tmp.reqLev[0],&tmp.reqLev[1],&tmp.reqLev[2],&tmp.reqLev[3], &tmp.reqLev[4]) != 5)
		{
			RtCoreLog().Info("CRS_FuBen::Load: parse �ȼ�Ҫ��[%s] error.\n",csv[j]["�ȼ�Ҫ��"].Str());
			return false;
		}
		if(sscanf(csv[j]["������"].Str(),"%d,%d,%d",&tmp.bornPos[0],&tmp.bornPos[1], &tmp.bornPos[2]) != 3)
		{
			RtCoreLog().Info("CRS_FuBen::Load: parse ������ error.\n");
			return false;
		}
		if(sscanf(csv[j]["�˳���"].Str(),"%d,%d,%d",&tmp.exitPos[0],&tmp.exitPos[1], &tmp.exitPos[2]) != 3)
		{
			RtCoreLog().Info("CRS_FuBen::Load: parse �˳��� error.\n");
			return false;
		}
		tmp.bornSceneBlock      = csv[j]["�˳�����"].Str();

		tmp.enterType			= csv[j]["����ģʽ"].Long();
		tmp.enterNum			= csv[j]["��������"].Long();
        if (tmp.enterNum>1)
        {
		    if(sscanf(csv[j]["�ȴ���"].Str(),"%d,%d,%d",&tmp.waitPos[0],&tmp.waitPos[1], &tmp.waitPos[2]) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse �ȴ��� error.\n");
			    return false;
		    }
        }

		tmp.reqLeaderItem       = csv[j]["�ӳ�����Ҫ��"].Long();
		tmp.reqMemberItem       = csv[j]["��Ա����Ҫ��"].Long();

        tmp.timeLimitSecond		= csv[j]["�ʱ��"].Long() * 60;
        tmp.timeOutSecond		= csv[j]["��תʱ��"].Long() * 60;
        tmp.minChr			    = csv[j]["��������"].Long();
        tmp.maxChr			    = csv[j]["�������"].Long();

        // ������
        tmp.cyc.iStartSecond    = csv[j]["��ʼʱ��"].Long();
        tmp.cyc.iEndSecond      = csv[j]["����ʱ��"].Long();

        tmp.cyc.cycMode         = csv[j]["������"].Long();
        if (tmp.cyc.cycMode==2)
        {
		    if(sscanf(csv[j]["����ģʽ"].Str(),"%d,%d,%d,%d,%d,%d,%d",
                &tmp.cyc.cycWeek[0],&tmp.cyc.cycWeek[1],&tmp.cyc.cycWeek[2],&tmp.cyc.cycWeek[3],&tmp.cyc.cycWeek[4],&tmp.cyc.cycWeek[5],&tmp.cyc.cycWeek[6]) != 7)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ����ģʽ error.\n");
			    return false;
		    }
        }else if (tmp.cyc.cycMode==3)
        {
		    if(sscanf(csv[j]["����ģʽ"].Str(),"%d,%d",&tmp.cyc.cycMonth[0],&tmp.cyc.cycMonth[1]) != 2)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ����ģʽ error.\n");
			    return false;
		    }
        }else
		{
			tmp.cyc.cycMonth[0] = 0;
			tmp.cyc.cycMonth[1] = 0;
		}

        // �ظ���Χ
        if (strlen(csv[j]["��ʼ����"].Str()))
        {
		    if(sscanf(csv[j]["��ʼ����"].Str(),"%d-%d-%d",&tmp.cyc.iStartYear,&tmp.cyc.iStartMonth, &tmp.cyc.iStartDay) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ��ʼ���� error.\n");
			    return false;
		    }
        }else
		{
			tmp.cyc.iStartYear = 0;
		}

        if (strlen(csv[j]["��������"].Str()))
        {
		    if(sscanf(csv[j]["��������"].Str(),"%d-%d-%d",&tmp.cyc.iEndYear,&tmp.cyc.iEndMonth, &tmp.cyc.iEndDay) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse �������� error.\n");
			    return false;
		    }
        }else
		{
			tmp.cyc.iEndYear = 0;
		}

        // ��
        if (strlen(csv[j]["��1"].Str()))
        {
    	    if(sscanf(csv[j]["��1"].Str(),"%d,%d,%d",&tmp.pos1[0],&tmp.pos1[1], &tmp.pos1[2]) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ��1 error.\n");
			    return false;
		    }
        }else
		{
			tmp.pos1[0] = tmp.pos1[1] = tmp.pos1[2] = 0;
		}
        if (strlen(csv[j]["��2"].Str()))
        {
    	    if(sscanf(csv[j]["��2"].Str(),"%d,%d,%d",&tmp.pos2[0],&tmp.pos2[1], &tmp.pos2[2]) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ��1 error.\n");
			    return false;
		    }
        }else
		{
			tmp.pos2[0] = tmp.pos2[1] = tmp.pos2[2] = 0;
		}
        if (strlen(csv[j]["��3"].Str()))
        {
    	    if(sscanf(csv[j]["��3"].Str(),"%d,%d,%d",&tmp.pos3[0],&tmp.pos3[1], &tmp.pos3[2]) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ��1 error.\n");
			    return false;
		    }
        }else
		{
			tmp.pos3[0] = tmp.pos3[1] = tmp.pos3[2] = 0;
		}
        if (strlen(csv[j]["��4"].Str()))
        {
    	    if(sscanf(csv[j]["��4"].Str(),"%d,%d,%d",&tmp.pos4[0],&tmp.pos4[1], &tmp.pos4[2]) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ��1 error.\n");
			    return false;
		    }
        }else
		{
			tmp.pos4[0] = tmp.pos4[1] = tmp.pos4[2] = 0;
		}
        if (strlen(csv[j]["��5"].Str()))
        {
    	    if(sscanf(csv[j]["��5"].Str(),"%d,%d,%d",&tmp.pos5[0],&tmp.pos5[1], &tmp.pos5[2]) != 3)
		    {
			    RtCoreLog().Info("CRS_FuBen::Load: parse ��1 error.\n");
			    return false;
		    }
        }else
		{
			tmp.pos5[0] = tmp.pos5[1] = tmp.pos5[2] = 0;
		}

		m_table.push_back(tmp);
	}
	return true;
}


bool CRS_LineSys::Load(const char *file)
{
	SLineSys tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();

	m_dwGroupCount = Total - 2;

	for(j++; j<Total; j++)
	{

		tmp.id			= csv[j]["ID"].Long();

		static const char* pTaskPath=NULL;
		static CIniVariable Temp(GetConfigBoot(),"ConfigInfo","TaskDirectory",&pTaskPath);
		static string TaskPath(pTaskPath);
		tmp.html        = TaskPath + csv[j]["����"].Str();
		tmp.name        = csv[j]["����"].Str();
		tmp.script      = csv[j]["�ű�"].Str();
		tmp.sceneFile   = csv[j]["�����ļ�"].Str();


		if(sscanf(csv[j]["������"].Str(),"%d,%d,%d",&tmp.bornPos[0],&tmp.bornPos[1], &tmp.bornPos[2]) != 3)
		{
			RtCoreLog().Info("CRS_FuBen::Load: parse ������ error.\n");
			return false;
		}

		tmp.enterType			= csv[j]["����ģʽ"].Long();

		tmp.maxChr			    = csv[j]["�������"].Long();

		tmp.iPercnet			= csv[j]["�л��ٷֱ�"].Long();

		tmp.iLineCount			= csv[j]["����"].Long();

		m_table.push_back(tmp);
	}
	return true;
}

// ---------------------------------------------------------------------------------
// Table Creature Model
// ---------------------------------------------------------------------------------
CRS_CreModel::CRS_CreModel() {}

CRS_CreModel::~CRS_CreModel() {}

bool CRS_CreModel::Load(const char *file)
{
 	SCreModel tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		int i = 0;
		tmp.Id			= csv[j]["ID"].Long();
		tmp.bComplex    = csv[j]["�Ƿ���ģ��"].Long()!=0;
		tmp.Act			= csv[j]["ģ���ļ�"].Str();
		tmp.HeadSkin    = csv[j]["ͷ"].Str();
		tmp.BodySkin    = csv[j]["����"].Str();
		tmp.HandSkin    = csv[j]["��"].Str();
		tmp.FootSkin    = csv[j]["��"].Str();
		tmp.CaestusSkin = csv[j]["��"].Str();
		tmp.ReplaceMtl  = csv[j]["������"].Str();
		if(sscanf(csv[j]["ģ������"].Str(),"%f,%f,%f",&tmp.ModelScale[0],
			&tmp.ModelScale[1],&tmp.ModelScale[2]) != 3)
		{
			// RtCoreLog().Info("CRS_CreModel::Load: parse ģ������ error.\n");
			// return false;
			tmp.ModelScale[0] = 1;
			tmp.ModelScale[1] = 1;
			tmp.ModelScale[2] = 1;
		}

		tmp.AttackRange = csv[j]["��������"].Float();
		tmp.MoveSpeed   = csv[j]["�ƶ��ٶ�"].Float();
		tmp.AttackSpeed = csv[j]["�����ٶ�"].Float();
		tmp.DeadType	= csv[j]["������ʽ"].Long();

		tmp.DeadAct		= csv[j]["����ģ���ļ�"].Str();
		tmp.BornAct		= csv[j]["����ģ���ļ�"].Str();

		tmp.LinkEffect  = csv[j]["������Ч�ļ�"].Str();
		tmp.LinkPoint   = csv[j]["link����"].Str();
		tmp.ReplaceMtl  = csv[j]["������"].Str();
		tmp.ModelRadius = csv[j]["�뾶"].Float();

		tmp.AddedSkin   = csv[j]["����Ƥ��"].Str();
		tmp.RidePoseFix = csv[j]["��˶���β׺"].Str();
		m_table.push_back(tmp);
	}
	return true;
}

SCreModel *CRS_CreModel::FindCreModel(int Id)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].Id == Id) return &m_table[i];
	}
	return NULL;
}


// ---------------------------------------------------------------------------------
// Table Actor Head Model
// ---------------------------------------------------------------------------------
CRS_HeadModel::CRS_HeadModel() {}

CRS_HeadModel::~CRS_HeadModel() {}

bool CRS_HeadModel::Load(const char *file)
{
	SHeadModel tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		int i = 0;
		tmp.id				= csv[j]["ID"].Short();
		tmp.name			= csv[j]["����"].Str();
		tmp.skin			= csv[j]["Ƥ��"].Str();
		tmp.actorModelID	= csv[j]["����ģ��"].Short();

		m_table.push_back(tmp);
	}
	return true;
}

bool CRS_HeadModel::GetHeadModel(short ModelID, vector<SHeadModel>& models)
{
	models.clear();

	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].actorModelID == ModelID)
			models.push_back(m_table[i]);
	}

	return !(models.empty());
}

bool CRS_HeadModel::GetHeadModelByID(short id, SHeadModel& model)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].id == id)
		{
			model = m_table[i];
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------------
// Table Actor 2D Image
// ---------------------------------------------------------------------------------
CRS_CharImage::CRS_CharImage() {}

CRS_CharImage::~CRS_CharImage() {}

bool CRS_CharImage::Load(const char *file)
{
	SCharImage tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		int i = 0;
		tmp.id				= csv[j]["ID"].Short();
		tmp.imageCreate		= csv[j]["����ͼ��"].Str();
		tmp.imageGame		= csv[j]["��Ϸͼ��"].Str();
		tmp.imageTeam		= csv[j]["���ͼ��"].Str();
		tmp.bValid			= csv[j]["�Ƿ���Ч"].Bool();
		tmp.actorModelID	= csv[j]["����ģ��"].Short();

		m_table.push_back(tmp);
	}
	return true;
}

bool CRS_CharImage::GetCharImage(short ModelID, vector<SCharImage>& images)
{
	images.clear();

	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].actorModelID == ModelID)
			images.push_back(m_table[i]);
	}

	return !(images.empty());
}


// ---------------------------------------------------------------------------------
// Table Creature Model
// ---------------------------------------------------------------------------------
CRS_Npc::CRS_Npc() {}

CRS_Npc::~CRS_Npc() {}

bool CRS_Npc::GetEventCommand(const char *str,vector<SEventCommand> &cmdList)
{
	const char *sepLine = "\x0a\x0d";

	char *pStrTmp,*k;
	pStrTmp = (char *)str;
	k = strtok(pStrTmp, sepLine);
	if(k && strlen(k)>0)
	{
		while (k)
		{
			char eve[128];
			char buf[6][128];
			memset(buf,0,6*128);
			sscanf(k,"%s %s %s %s %s %s %s",eve,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

			SEventCommand cmd;
			cmd.event = eve;
			cmd.command.key = buf[0];
			cmd.command.param[0] = buf[1];
			cmd.command.param[1] = buf[2];
			cmd.command.param[2] = buf[3];
			cmd.command.param[3] = buf[4];
			cmd.command.param[4] = buf[5];

			cmdList.push_back(cmd);
			k = strtok(NULL, sepLine);
		}
	}	
	return true;
}

bool CRS_Npc::Load(const char *File)
{
	int i, j, counter;
    char* k;
    char* pStrTmp;
    const char sep[] = ",;";	// �ָ���
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		SNpc tmp;
		tmp.Name		 = csv[j]["����"].Str();
        tmp.Id			 = csv[j]["ID"].Long();
		tmp.TypeId       = csv[j]["����id"].Str();
        tmp.Type		 = csv[j]["����"].Long();
		// tmp.TypeParam = csv[j]["���Ͳ���"].Str();
		tmp.Script       = csv[j]["�ű�"].Str();
		tmp.mOverHead    = csv[j]["����ͷ����ʾ"].Str();

		tmp.PatrolRange  = csv[j]["Ѳ�߾���"].Float();
		tmp.ViewRange	 = csv[j]["��Ұ��Χ"].Float();
		tmp.ValidRange   = csv[j]["�������"].Float();
		tmp.AssistRange  = csv[j]["Э����Χ"].Float();
		tmp.CombatType   = csv[j]["ս����ʽ"].Long();	
		tmp.PartyMonster = csv[j]["�����"].Long();
	
		tmp.BoundType   = csv[j]["��ײ����"].Long();
        for(i=0; i<NPC_MAX_SKILL; i++) tmp.sSkills[i] = 0;
        pStrTmp = (char *)csv[j]["����"].Str();
        counter = 0;
        k = strtok(pStrTmp, sep);
        if (k && strlen(k)>0)
        {
            while (k && counter<NPC_MAX_SKILL)
            {
                tmp.sSkills[counter] = atoi(k);
                counter++;
                k = strtok(NULL, sep);
            }
        }

		for(i=0; i<NPC_MAX_SKILL; i++) tmp.SkillAttackRate[i] = 0;
		pStrTmp = (char *)csv[j]["����ʹ�ø���"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter<NPC_MAX_SKILL)
			{
				tmp.SkillAttackRate[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		//tmp.SkillAttackRate = csv[j]["����ʹ�ø���"].Float();

		tmp.ModelId		= csv[j]["ģ��ID"].Short();
		tmp.AttackSpeed = csv[j]["�����ٶ�"].Float();
		tmp.MoveSpeed   = csv[j]["�ƶ��ٶ�"].Float();
		tmp.AttackRange = csv[j]["��������"].Float();
		tmp.Level       = csv[j]["LEVEL"].Int();
		tmp.Hp          = csv[j]["HP"].Long();
		tmp.Mp			= csv[j]["MP"].Long();
		tmp.Attack		= csv[j]["Attack"].Long();
		tmp.Damage      = csv[j]["Damage"].Long();
		tmp.Dodge       = csv[j]["Dodge"].Long();
		tmp.Armor       = csv[j]["Armor"].Long();
		
		tmp.Exp			= csv[j]["Exp"].Long();
        tmp.lSP			= csv[j]["SP"].Long();
		tmp.Credit		= csv[j]["����"].Long();
		tmp.Gold		= csv[j]["Gold"].Long();
		tmp.GoldRate	= csv[j]["��Ǯ���伸��"].Float();
        tmp.SellItems	= csv[j]["������Ʒ"].Str();
        tmp.SellRareItems = csv[j]["����ϡ����Ʒ"].Str();
		tmp.AtMiddleMap = csv[j]["�е�ͼ��"].Str();						// NPC�����е�ͼ����
		if(sscanf(csv[j]["����"].Str(),"%d,%d,%d",&tmp.Pos[0],			// NPC�����е�ͼ����
			&tmp.Pos[1],&tmp.Pos[2]) != 3)
		{
			RtCoreLog().Info("CRS_Npc::Load: [%d] parse NPC������� error.\n",tmp.Id);
			// return false;
		}
        if (!tmp.SellRareItems.empty()) // Ԥ����
        {
            char *pStrRareItems = RT_NEW char[tmp.SellRareItems.size() +1];
            rt2_strncpy(pStrRareItems, tmp.SellRareItems.c_str(), tmp.SellRareItems.size() +1);

            const char sep1[] = ";";
            std::vector<std::string> arrStringRare;
            char* k = strtok(pStrRareItems, sep1);
            while (k)
            {
                arrStringRare.push_back(k);
                k = strtok(NULL ,sep1);
            }
			DEL_ARRAY(pStrRareItems);
            int tmpIdx = 0;
            for (tmpIdx=0; tmpIdx<(int)arrStringRare.size(); tmpIdx++)
            {
                SNpcSellRareItem sri;
                if (!sri.Load(arrStringRare[tmpIdx]))
                {
                    RtCoreLog().Info("[CRS_Npc::Load] ����ϡ����Ʒ ���� (line=%d)\n", j);
                    return false;
                }
            }
        }

		//Ҫƴ��·��//add by fox
		static const char* pTaskPath=NULL;
		static CIniVariable Temp(GetConfigBoot(),"ConfigInfo","TaskDirectory",&pTaskPath);
		static string TaskPath(pTaskPath);

		tmp.DefaultTalk = TaskPath + csv[j]["Ĭ�϶Ի�"].Str();

		tmp.Dif         = csv[j]["�Ѷ�"].Long();
		tmp.Boss        = csv[j]["Boss"].Long();
		tmp.HeadPic     = csv[j]["ͷ��"].Str();
		tmp.taskPic     = csv[j]["������"].Str();


	    RtString trim = csv[j]["��Ӫ"].Str();
		trim.TrimLeft();
		trim.TrimRight();
		if(trim == "zhou")
		{
			tmp.Faction = FACTION_ZHOU;
		}
		else if(trim == "shang")
		{
			tmp.Faction = FACTION_SHANG;
		}
		else if (trim == "zhongli")
		{
			tmp.Faction = FACTION_NEUTRAL;
		}
		else
		{
            tmp.Faction = FACTION_UNKNOWN;
		}

		for(i=0; i<MAX_ELEMENT_ATTR; i++)
		{
			tmp.ElementDamage[i] = 0;
			tmp.ElementArmor[i] = 0;
		}

		for(i=0; i<MAX_NPC_DROP_ITEM; i++)
			tmp.ItemDropRate[i] = 0;

		for(i=0; i<MAX_NPC_DROP_ITEM; i++)
			tmp.Item[i] = 0;


		pStrTmp = (char *)csv[j]["�������"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter<MAX_NPC_DROP_ITEM)
			{
				tmp.Item[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		tmp.ItemNum = counter;

		/*for(i=0; i<MAX_NPC_DROP_ITEM; i++)
			tmp.ItemDropRate[i] = 0;*/

		pStrTmp = (char *)csv[j]["������߸���"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter < MAX_NPC_DROP_ITEM)
			{
				tmp.ItemDropRate[counter] = atof(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		if(tmp.ItemNum != counter)
		{
			RtCoreLog().Info("CRS_Npc::Load: %d itemNum[%d] != dropNum[%d]\n",tmp.Id,tmp.ItemNum,counter);
		}

		for(i=0; i<NPC_MAX_CALL_SPIRIT; i++)
		{
			tmp.CallSpiritID[i] = 0;
			tmp.CallSpiritRate[i] = 0.f;
		}

		pStrTmp = (char *)csv[j]["����Ԫ��"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter<MAX_NPC_DROP_ITEM)
			{
				tmp.CallSpiritID[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		tmp.CallSpiritNum = counter;

		/*for(i=0; i<MAX_NPC_DROP_ITEM; i++)
			tmp.ItemDropRate[i] = 0;*/

		pStrTmp = (char *)csv[j]["����Ԫ�����"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter < MAX_NPC_DROP_ITEM)
			{
				tmp.CallSpiritRate[counter] = atof(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		if(tmp.CallSpiritNum != counter)
		{
			RtCoreLog().Error("CRS_Npc::Load: %d CallSpiritRateNum[%d] != CallSpiritIDNum[%d]\n",tmp.Id,tmp.CallSpiritNum,counter);
		}

		tmp.ItemGreenRate = csv[j]["��ɫ����"].Double();
		tmp.ItemBlueRate = csv[j]["��ɫ����"].Double();
		tmp.ItemGoldRate = csv[j]["��ɫ����"].Double();
		tmp.ItemBindType = csv[j]["�󶨷�ʽ"].Long();
		tmp.isPrompt     = csv[j]["�����Ƿ���ʾ"].Bool();
		// random item 
		for(i=0; i<MAX_NPC_DROP_ITEM; i++)
			tmp.RandItem[i] = 0;

		pStrTmp = (char *)csv[j]["�������"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter<MAX_NPC_DROP_ITEM)
			{
				tmp.RandItem[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		tmp.RandItemNum = counter;

		for(i=0; i<MAX_NPC_DROP_ITEM; i++)
			tmp.RandItemDropRate[i] = 0;

		pStrTmp = (char *)csv[j]["����������"].Str();
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k)>0)
		{
			while (k && counter < MAX_NPC_DROP_ITEM)
			{
				tmp.RandItemDropRate[counter] = atof(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		if(tmp.RandItemNum != counter)
		{
			RtCoreLog().Info("CRS_Npc::Load: %d itemNum[%d] != dropNum[%d]\n",tmp.Id,tmp.RandItemNum,counter);
		}


		if(sscanf(csv[j]["���Թ����˺�"].Str(),"%d,%d,%d",&tmp.ElementDamage[0],
			&tmp.ElementDamage[1],&tmp.ElementDamage[2]) != 3)
		{
			RtCoreLog().Info("CRS_Npc::Load: [%d] parse ���Թ����˺� error.\n",tmp.Id);
			// return false;
		}
		if(sscanf(csv[j]["����"].Str(),"%d,%d,%d",&tmp.ElementArmor[0],
			&tmp.ElementArmor[1],&tmp.ElementArmor[2]) != 3)
		{
			RtCoreLog().Info("CRS_Npc::Load: [%d] parse ���� error.\n",tmp.Id);
			// return false;
		}

		GetEventCommand(csv[j]["�¼�����"].Str(),tmp.EventCmd);

		tmp.FbTask			 = csv[j]["�Ƿ񸱱�����"].Long();

		char* pStrTask = (char *)csv[j]["�������"].Str();
		k = strtok(pStrTask, sep);
		counter = 0;
		if (k && strlen(k)>0)
		{
			while (k && counter < NPC_MAX_TASK_ITEM)
			{
				tmp.TaskId[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		tmp.nTaskNum = counter;
		pStrTask = (char *)csv[j]["�������"].Str();
		k = strtok(pStrTask, sep);
		counter = 0;
		if (k && strlen(k)>0)
		{
			while (k && counter < NPC_MAX_TASK_ITEM)
			{
				tmp.TaskItemId[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
			//tmp.nTaskNum = counter;
		}
		pStrTask = (char *)csv[j]["������߸���"].Str();
		k = strtok(pStrTask, sep);
		counter = 0;
		if (k && strlen(k)>0)
		{
			while (k && counter < NPC_MAX_TASK_ITEM)
			{
				tmp.TaskItemDropRate[counter] = atof(k);
				counter++;
				k = strtok(NULL, sep);
			}
			//tmp.nTaskNum = counter;
		}

		pStrTask = (char *)csv[j]["��������"].Str();
		k = strtok(pStrTask, sep);
		counter = 0;
		if (k && strlen(k)>0)
		{
			while (k && counter < NPC_MAX_TASK_ITEM)
			{
				tmp.TaskItemLimit[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
			//tmp.nTaskNum = counter;
		}

		//tmp.TaskId			 = csv[j]["�������"].Long();
		//tmp.TaskItemId		 = csv[j]["�������"].Long();
		//tmp.TaskItemLimit    = csv[j]["��������"].Long();
		//tmp.TaskItemDropRate = csv[j]["������߸���"].Float();
		tmp.AIType			 = csv[j]["AI����"].Str();
		tmp.Home			 = csv[j]["��ס��"].Str();
//-----------------------add start by tony 05.05.23--------------------------//
		tmp.Interval		 = csv[j]["��������"].Long();
//-----------------------add end by tony 05.05.23----------------------------//

//-----------------------add start by tony 05.05.08--------------------------//
		//��ȡѲ��·�������ʽΪ1,1;2,2
		char delim[] = ";";								//�ָ���

		SPathNode node = {0, 0};						//·���ڵ�

		pStrTmp = (char *)csv[j]["Ѳ��·��"].Str();		//��ȡ�ַ���

		if(pStrTmp)
		{		
			k = strtok(pStrTmp, delim);					//ȡ�õ�һ������

			if( (k != NULL) && (strlen(k) > 0) )
			{
				//ѭ����ȡ��������
				while(k != NULL)
				{
					//�����ȡ����
					if(sscanf(k, "%d,%d", &node.x, &node.y) != 2)
					{
						RtCoreLog().Info("�ļ�npc.csv�����ʽ����\n");
						return false;
					}

					tmp.PatrolPath.push_back(node);

					k = strtok(NULL, delim);			//ȡ�ú�������
				}//end of while

			}//end of if

		}//end of if

		tmp.AILevel = csv[j]["AI����"].DWord();
		tmp.ConjureTarID = csv[j]["�ٻ�Ŀ��ID"].DWord();
		tmp.ProtecterID = csv[j]["������ID"].DWord();

//-------------------------add end by tony 05.05.08--------------------------//
		char sep1[] = ";";
		pStrTmp = (char *)csv[j]["�ɽ�������"].Str();
		k = strtok(pStrTmp, sep1);
		if(k && strlen(k)>0)
		{
			while (k)
			{
				tmp.GetTaskId.push_back(atol(k));
				k = strtok(NULL, sep1);
			}
		}

		pStrTmp = (char *)csv[j]["���������"].Str();
		k = strtok(pStrTmp, sep1);
		if(k && strlen(k)>0)
		{
			while (k)
			{
				tmp.FinishTaskId.push_back(atol(k));
				k = strtok(NULL, sep1);
			}
		}

		tmp.RandomTask = csv[j]["�Ƿ��������"].Long();
		pStrTmp = (char *)csv[j]["�����������"].Str();
		k = strtok(pStrTmp, sep1);
		if(k && strlen(k)>0)
		{
			while (k)
			{
				tmp.RandomTaskRate.push_back(atol(k));
				k = strtok(NULL, sep1);
			}
		}

		pStrTmp = (char *)csv[j]["����"].Str();
		k = strtok(pStrTmp, sep1);
		if(k && strlen(k)>0)
		{
			while (k)
			{
				tmp.Function.push_back(atol(k));
				k = strtok(NULL, sep1);
			}
		}
		//PZH
		tmp.cTaskType = (char)(csv[j]["�����������"].Long());
		tmp.dwSkinID = csv[j]["Ƥ��ID"].Long();
		tmp.dwDiwKeetTime = csv[j]["ʬ�����ʱ����"].Long();
		tmp.dwGetSkinTime = csv[j]["��Ƥʱ����"].Long();
		//
		tmp.szDeadLink = csv[j]["������ЧLINK��"].Str();
		tmp.szDeadEffect = csv[j]["������Ч"].Str();
		m_table.push_back(tmp);
	}
	return true;
}

SNpc *CRS_Npc::FindNpc(int Id)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].Id == Id) return &m_table[i];
	}
	return NULL;
}
void CRS_Npc::GetNpcForMiddleMap(vector<vector<SNpc*>*>& vectorSNpc,map<string,int>& stringToInt)
{
	guard;
	map<string,int>::iterator it;
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		it = stringToInt.find(m_table[i].AtMiddleMap);
		if(it != stringToInt.end())
		{
			vectorSNpc[it->second]->push_back(&m_table[i]);
		}
	}
	unguard;
}

//-----------------------Add LeiJun NPC Random Talk----------------------------//
CRS_NPCTalkText::CRS_NPCTalkText(void)
{

}

CRS_NPCTalkText::~CRS_NPCTalkText(void)
{

}

bool	CRS_NPCTalkText::Load(const char *File)
{
	const char *sDelim = ";";
	RtCsv csv;
	if(!csv.LoadTableFile(File))
		return false;
	int jj = csv.FindLineByString("ID");
	if(jj < 0) return false;
	csv.SetTitleLine(jj);

	int Total = csv.GetLineCount();
	for(jj++; jj<Total; jj++)
	{
		std::vector<std::string> vTexts;
		char *pTemp = 0;
		long lID = csv[jj]["ID"].Long();
		char *sTalkText = (char*)csv[jj]["һ�㺰��"].Str();
		if(!sTalkText)	return false;
		pTemp = strtok(sTalkText, sDelim);
		if (pTemp && strlen(pTemp) > 0)
		{
			while(pTemp)
			{
				vTexts.push_back(pTemp);
				pTemp = strtok(NULL, sDelim);
			}
		}
		m_TalkTexts[lID] = vTexts;
		vTexts.clear();
		pTemp = 0;

		char *sSuccorText = (char*)csv[jj]["��������"].Str();
		if(!sSuccorText)	return false;
		pTemp = strtok(sSuccorText, sDelim);
		if (pTemp && strlen(pTemp) > 0)
		{
			while(pTemp)
			{
				vTexts.push_back(pTemp);
				pTemp = strtok(NULL, sDelim);
			}
		}
		m_SuccorTexts[lID] = vTexts;
		vTexts.clear();
		pTemp = 0;

		char *sOverText = (char*)csv[jj]["��������"].Str();
		if(!sOverText)	return false;
		pTemp = strtok(sOverText, sDelim);
		if (pTemp && strlen(pTemp) > 0)
		{
			while(pTemp)
			{
				vTexts.push_back(pTemp);
				pTemp = strtok(NULL, sDelim);
			}
		}
		m_OverTexts[lID] = vTexts;
		vTexts.clear();
		pTemp = 0;
	}

	return true;
}

const char*	CRS_NPCTalkText::FindRandomTalkTextByID(int iID)
{
	if(iID <= 0)
		return 0;

	RANDTEXT::iterator itTexts = m_TalkTexts.find(iID);
	if(itTexts != m_TalkTexts.end() && !itTexts->second.empty())
	{
		int iIndex = rand() % itTexts->second.size();
		return itTexts->second[iIndex].c_str();
	}

	return 0;
}

const char*	CRS_NPCTalkText::FindRandomSuccorTextByID(int iID)
{
	if(iID <= 0)
		return 0;

	RANDTEXT::iterator itTexts = m_SuccorTexts.find(iID);
	if(itTexts != m_SuccorTexts.end() && !itTexts->second.empty())
	{
		int iIndex = rand() % itTexts->second.size();
		return itTexts->second[iIndex].c_str();
	}

	return 0;
}

const char*	CRS_NPCTalkText::FindRandomOverTextByID(int iID)
{
	if(iID <= 0)
		return 0;

	RANDTEXT::iterator itTexts = m_OverTexts.find(iID);
	if(itTexts != m_OverTexts.end() && !itTexts->second.empty())
	{
		int iIndex = rand() % itTexts->second.size();
		return itTexts->second[iIndex].c_str();
	}

	return 0;
}
//-----------------------End LeiJun NPC Random Talk----------------------------//
CRS_Problem::CRS_Problem()
{

}
CRS_Problem::~CRS_Problem()
{

}

bool CRS_Problem::Load(const char *File)
{
	RtCsv csv;
	if(!csv.LoadTableFile(File))
		return false;
	int jj = csv.FindLineByString("ID");
	if(jj < 0) return false;
	csv.SetTitleLine(jj);
	int Total = csv.GetLineCount();

	for(jj++; jj<Total; jj++)
	{
		long lID = csv[jj]["ID"].Long();
		ProblemData oData;
		oData.sTitle = "<p>";
		oData.sTitle += (char*)csv[jj]["�������"].Str();
		oData.sTitle += "</p>";
		oData.sDescription = "<p>";
		oData.sDescription += (char*)csv[jj]["��������"].Str();
		oData.sDescription += "</p>";
		for(int loop=0; loop<ProblemData::OPTION_COUNT; loop++)
		{
			char head[30] = {0};
			rt2_snprintf(head, 30, "����ѡ��%d", loop+1);
			char *option = (char*)csv[jj][head].Str();
			if(!option || option[0] == 0)
				break;
			rt2_snprintf(head, 30, "ѡ��%d����", loop+1);
			char *command = (char*)csv[jj][head].Str();
			oData.sOption[loop] += "<p><a href=\"";
			oData.sOption[loop] += command;
			oData.sOption[loop] += "\">";
			oData.sOption[loop] += option;
			oData.sOption[loop] += "</a></p>";
		}
		vProblems[lID] = oData;
	}
	return true;
}

ProblemData* CRS_Problem::FindProblemByID(long lProblemID)
{
	if(lProblemID <= 0)
		return 0;
	PROBLEMLIST::iterator itProblem = vProblems.find(lProblemID);
	if(itProblem != vProblems.end())
	{
		return &(itProblem->second);
	}
	return 0;
}
//-----------------------Add LeiJun Problem----------------------------//

//-----------------------End LeiJun Problem----------------------------//

////------------------------------------------------------
////  SSQLPet   Tianh 10.03.09
////-------------------------------------------------------
//
//SSQLPet::SSQLPet()
//{
//	//�ӱ��ж����ĳ����˺�   �����FRDDamage.
//	FRDDamage = ;
//}

// ---------------------------------------------------------------------------------
// Table Base Value 
// ---------------------------------------------------------------------------------
CRS_BaseValue::CRS_BaseValue() {}

CRS_BaseValue::~CRS_BaseValue() {}

bool CRS_BaseValue::Load(const char *File)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("�ȼ�");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		SBaseValue tmp;
		tmp.FHp          = csv[j]["HP"].Float();
		tmp.FMp			 = csv[j]["MP"].Float();
		tmp.FAttack		 = csv[j]["����"].Float();
		tmp.FDamage      = csv[j]["�˺�"].Float();
		tmp.FDodge       = csv[j]["����"].Float();
		tmp.FArmor       = csv[j]["����"].Float();

		m_table.push_back(tmp);
	}
	return true;
}

SBaseValue *CRS_BaseValue::Find(int lev)
{
	return &m_table[lev-1]; 
}

// ---------------------------------------------------------------------------------
// Table Task 
// ---------------------------------------------------------------------------------
CRS_Task::CRS_Task() {}

CRS_Task::~CRS_Task() {}

bool CRS_Task::GetTaskKey(const char *str,vector<STaskKey> &key)
{
	const char *sepLine = "\x0a\x0d";
	// const char *sep = ",;";

	char *pStrTmp,*k;
	pStrTmp = (char *)str;
	k = strtok(pStrTmp, sepLine);
	if(k && strlen(k)>0)
	{
		while (k)
		{
			char buf[20][128];
			memset(buf,0,20*128);
			sscanf(k,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],
				   buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15],buf[16],buf[17],buf[18],buf[19]);
					
			STaskKey tkey;
			tkey.key = buf[0];
			tkey.param[0] = buf[1];
			tkey.param[1] = buf[2];
			tkey.param[2] = buf[3];
			tkey.param[3] = buf[4];
			tkey.param[4] = buf[5];
            tkey.param[5] = buf[6];
            tkey.param[6] = buf[7];
			tkey.param[7] = buf[8];
			tkey.param[8] = buf[9];
			tkey.param[9] = buf[10];
			tkey.param[10] = buf[11];
			tkey.param[11] = buf[12];
			tkey.param[12] = buf[13];
			tkey.param[13] = buf[14];
			tkey.param[14] = buf[15];
			tkey.param[15] = buf[16];
			tkey.param[16] = buf[17];
			tkey.param[17] = buf[18];
			tkey.param[18] = buf[19];
			key.push_back(tkey);
			k = strtok(NULL, sepLine);
		}
	}	
	return true;
}

bool CRS_Task::Load(const char *File)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);
    
    int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		STaskDesc tmp;
		tmp.id			= csv[j]["ID"].Long();
		tmp.group       = csv[j]["����ID"].Long();
		tmp.name		= csv[j]["��������"].Str();
		tmp.desc		= csv[j]["��������"].Str();
		if(csv[j]["����ֱ�ӽ���"].Long() == 0)
		{
			tmp.access = false;
		}
		else
		{
			tmp.access = true;
		}

		GetTaskKey(csv[j]["�����������"].Str(),tmp.acceptReq);
		GetTaskKey(csv[j]["�����������"].Str(),tmp.taskReq);
		tmp.reqMinLev   = csv[j]["��͵ȼ�"].Long();
		tmp.reqMaxLev   = csv[j]["��ߵȼ�"].Long();
		tmp.repeat      = csv[j]["�Ƿ���ظ�"].Long();
		tmp.canCancel	= csv[j]["�Ƿ��ȡ��"].Long();
		tmp.metierLimit = csv[j]["ְҵ����"].Long();  
		tmp.countryLimit = csv[j]["��������"].Long();
		tmp.getTimeLimit = csv[j]["����ʱ������"].Long();
		tmp.finishTimeLimit = csv[j]["���ʱ������"].Long();
		tmp.reqUnion = csv[j]["��ְҪ��"].Long();
		
		GetTaskKey(csv[j]["�����ǵ���"].Str(),tmp.flagItem);
		tmp.moneyTask  = csv[j]["��������"].Long();
		tmp.wenTask  = csv[j]["�Ĺ�����"].Long();
		tmp.wenTaskCnt  = csv[j]["�Ĺ��������"].Long();

		//-------add start by tony 06.05.08-------------------------------//
		//- reason: ����ܻ������޸��˱�task.csv,����һ�ֶ�
		tmp.circleTask = csv[j]["�ܻ�����"].Long();
		//-------add end   by tony 06.05.08-------------------------------//

		tmp.acceptFlag = csv[j]["����������"].Str();
		tmp.finishType = csv[j]["��ɷ�ʽ"].Long();


		//Ҫƴ��·��//add by fox
		static const char* pTaskPath=NULL;
		static CIniVariable Temp(GetConfigBoot(),"ConfigInfo","TaskDirectory",&pTaskPath);
		static string TaskPath(pTaskPath);

		tmp.npc1PreAcceptTalk	= TaskPath + csv[j]["npc1������Ի�"].Str();
		tmp.npc1AfterAcceptTalk	= TaskPath + csv[j]["npc1�������Ի�"].Str();
		tmp.npc1InTaskTalk		= TaskPath + csv[j]["npc1�����жԻ�"].Str();
		tmp.npc2InTaskTalk		= TaskPath + csv[j]["npc2�����жԻ�"].Str();
		tmp.npc2AfterFinishTalk = TaskPath + csv[j]["npc2��������Ի�"].Str();
		tmp.npc2SelectFailedTalk= TaskPath + csv[j]["npc2ѡ��ʧ�ܶԻ�"].Str();

		GetTaskKey(csv[j]["�̶�����"].Str(),tmp.fixReward);
		GetTaskKey(csv[j]["��ѡ����"].Str(),tmp.selectReward);
		GetTaskKey(csv[j]["ְҵ����"].Str(),tmp.jobReward);
		tmp.randomRewardRate	= csv[j]["�����������"].Float();
		GetTaskKey(csv[j]["�������"].Str(),tmp.randomReward);
		
		tmp.specRewardRate = csv[j]["���⽱������"].Float();
		for(int i=0; i<MAX_TASK_SELECT_NUM; i++)
		{
			char buf[128];
			rt2_sprintf(buf,"ѡ����%d",i+1);
			tmp.specFlag[i] = csv[j][buf].Str();

			rt2_sprintf(buf,"����Ҫ��%d",i+1);
			GetTaskKey(csv[j][buf].Str(),tmp.specReq[i]);

			rt2_sprintf(buf,"���⽱��%d",i+1);
			GetTaskKey(csv[j][buf].Str(),tmp.specReward[i]);
		}
		
		GetTaskKey(csv[j]["��̬��ʾ"].Str(),tmp.dynamicText);
		GetTaskKey(csv[j]["��̬��ʾ2"].Str(),tmp.dynamicText2);
		GetTaskKey(csv[j]["��ʾ����"].Str(),tmp.dynamicItem);
		GetTaskKey(csv[j]["��ʾ����"].Str(),tmp.dynamicNum);
// 
// 		tmp.dynamicText		= csv[j]["��̬��ʾ"].Str();
// 		tmp.dynamicItem     = csv[j]["��ʾ����"].Long();
// 		tmp.dynamicNum		= csv[j]["��ʾ����"].Long();

		//PZH
		tmp.m_TaskSencenID = csv[j]["��������"].Str();//��������
		tmp.m_TaskGroupID = csv[j]["�����������"].Long();//����������� 
		tmp.m_bHasMax = csv[j]["�Ƿ������޸�������"].Bool();//�Ƿ������޸�������
		tmp.m_bShare = csv[j]["�Ƿ�ɹ���"].Bool();//�Ƿ�ɹ���
		tmp.mAcceptDesc = csv[j]["���������Ϣ"].Str();
		// gao
		if(sscanf(csv[j]["����NPC����"].Str(),"%d,%d",&tmp.m_posTaskBegin[0],			// NPC�����е�ͼ����
			&tmp.m_posTaskBegin[1]) != 2)
		{
			RtCoreLog().Info("CRS_Npc::Load: [%d] parse ����NPC������� error.\n",tmp.id);
			// return false;
		}
		if(sscanf(csv[j]["���NPC����"].Str(),"%d,%d",&tmp.m_posTaskEnd[0],			// NPC�����е�ͼ����
			&tmp.m_posTaskEnd[1]) != 2)
		{
			RtCoreLog().Info("CRS_Npc::Load: [%d] parse ���NPC������� error.\n",tmp.id);
			// return false;
		}
		
		m_table.push_back(tmp);
	}

	//----------add start by tony 06.05.16------------------------------//
	RtCsv csv1;
	if(!csv1.LoadTableFile(R(RES_TABLE_CIRCLE_ITEM)))
		return false;
	j = csv1.FindLineByString("ID");
	if(j<0) return false;
	csv1.SetTitleLine(j);
	Total = csv1.GetLineCount();
	for(j++; j<Total; j++)
	{
		long tmpItemID;
		tmpItemID = csv1[j]["�ܻ������õ���ID"].Long();
		g_tcrndItemID.push_back(tmpItemID);
	}

	RtCsv csv2;
	if(!csv2.LoadTableFile(R(RES_TABLE_CIRCLE_NPC)))
		return false;
	j = csv2.FindLineByString("ID");
	if(j<0) return false;
	csv2.SetTitleLine(j);
	Total = csv2.GetLineCount();
	for(j++; j<Total; j++)
	{
		long tmpNpcID;
		tmpNpcID = csv2[j]["�ܻ�����npcID"].Long();
		g_tcrndNpcID.push_back(tmpNpcID);
	}

	RtCsv csv3;
	if(!csv3.LoadTableFile(R(RES_TABLE_CIRCLE_MONSTER)))
		return false;
	j = csv3.FindLineByString("ID");
	if(j<0) return false;
	csv3.SetTitleLine(j);
	Total = csv3.GetLineCount();
	for(j++; j<Total; j++)
	{
		long tmpMonsterID;
		tmpMonsterID = csv3[j]["�ܻ��������ID"].Long();
		g_tcrndMonsterID.push_back(tmpMonsterID);
	}

	RtCsv csv4;
	if(!csv4.LoadTableFile(R(RES_TABLE_CIRCLE_EXPRATE)))
		return false;
	j = csv4.FindLineByString("ID");
	g_tcTotalCircle = csv4[j+1]["�ܻ���"].Long();
	if(j<0) return false;
	csv4.SetTitleLine(j);
	Total = csv4.GetLineCount();
	for(j++; j<Total; j++)
	{
		long tmpRate;		
		tmpRate = csv4[j]["��������(���֮һ)"].Long();
		g_tcexpRate.push_back(tmpRate);
	}
	//----------add end  by tony 06.05.16--------------------------------//
	return true;
}

STaskDesc *CRS_Task::FindTask(int id)
{
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		if(m_table[i].id == id) return &m_table[i];
	}
	return NULL;
}


bool CRS_Task::LoadFbTask(const char *file)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		SFbTask tmp;
		tmp.id			= csv[j]["ID"].Long();
		tmp.fbId        = csv[j]["��������"].Long();
		tmp.name		= csv[j]["����"].Str();
		tmp.visDif      = csv[j]["�����Ѷ�"].Long();
		tmp.taskDif     = csv[j]["�����Ѷ�"].Long();

		for(int i=0; i<MAX_FB_LEVEL; i++)
		{
			char buf[128];
			rt2_sprintf(buf,"����%d",i+1);



			//Ҫƴ��·��//add by fox
			static const char* pTaskPath=NULL;
			static CIniVariable Temp(GetConfigBoot(),"ConfigInfo","TaskDirectory",&pTaskPath);
			static string TaskPath(pTaskPath);
			tmp.taskDesc[i] = TaskPath + csv[j][buf].Str();

			rt2_sprintf(buf,"���Ҫ��%d",i+1);
			GetTaskKey(csv[j][buf].Str(),tmp.taskFinReq[i]);

			rt2_sprintf(buf,"����%d",i+1);
			GetTaskKey(csv[j][buf].Str(),tmp.taskReward[i]);
		}

		m_fbTable.push_back(tmp);
	}
	return true;
}

SFbTask *CRS_Task::FindFbTask(int id)
{
	for(unsigned int i=0; i<m_fbTable.size(); i++)
	{
		if(m_fbTable[i].id == id) return &m_fbTable[i];
	}
	return NULL;
}

void CRS_Task::GetTaskByFb(int fbId,int dif,vector<long> &taskId)
{
	for(unsigned int i=0; i<m_fbTable.size(); i++)
	{
		if(m_fbTable[i].fbId==fbId && m_fbTable[i].visDif <= dif) taskId.push_back(m_fbTable[i].id);
	}
}
void CRS_Task::GetTaskDescForMiddleMap(vector<vector<STaskDesc*>*>& vectorTaskDesc,map<string,int>& stringToInt)
{
	guard;
	map<string,int>::iterator it;
	for(unsigned int i=0; i<m_table.size(); i++)
	{
		it = stringToInt.find(m_table[i].m_TaskSencenID);
		if(it != stringToInt.end())
		{
			vectorTaskDesc[it->second]->push_back(&m_table[i]);
		}
	}
	unguard;
}

//
bool SFbTaskInfo::IsTaskFinished(int taskId)
{
	for(int i=0; i<(int)finTask.size(); i++)
		if(finTask[i]==taskId) return true;
	return false;
}

bool SFbTaskInfo::FinishTask(int taskId) 
{
	finTask.push_back(taskId);
	if(CanLevelUp())
		LevelUp();
	return true;
}

bool SFbTaskInfo::CanLevelUp() 
{
	if(curDif >= MAX_FB_LEVEL-1) return false;
	vector<long> taskId;
	g_TableTask.GetTaskByFb(fbId,curDif,taskId);
	
	if(finTask.size() >= taskId.size())
		return true;
	else
		return false;
}

void SFbTaskInfo::LevelUp() 
{
	curDif++;
	finTask.clear();
}

// ----------------------------------------------------------------------------------
// scene info 
// ----------------------------------------------------------------------------------

CRS_SceneInfo::CRS_SceneInfo()
{
}

CRS_SceneInfo::~CRS_SceneInfo()
{
}

bool CRS_SceneInfo::Load(const char *File)
{
    SSceneInfo tmp;
    int j;
    RtCsv csv;
    const char* pStr;

    if(!csv.LoadTableFile(File))
        return false;

    j = csv.FindLineByString("�����ļ�");
    if(j<0) return false;
    csv.SetTitleLine(j);

    int Total = csv.GetLineCount();
    for(j++; j<Total; j++)
    {
		tmp.szSceneFileName = csv[j]["�����ļ�"].Str();
		tmp.szSceneCombatMusic = csv[j]["ս������"].Str();
        pStr = csv[j]["����������1"].Str();     sscanf(pStr, "%f;%f", &(tmp.fRebirth1X), &(tmp.fRebirth1Y));
        pStr = csv[j]["����������2"].Str();     sscanf(pStr, "%f;%f", &(tmp.fRebirth2X), &(tmp.fRebirth2Y));
        pStr = csv[j]["����������3"].Str();     sscanf(pStr, "%f;%f", &(tmp.fRebirth3X), &(tmp.fRebirth3Y));
		pStr = csv[j]["�Է�������1"].Str();     sscanf(pStr, "%f;%f", &(tmp.fX_Oppose1), &(tmp.fY_Oppose1));
		pStr = csv[j]["�Է�������2"].Str();     sscanf(pStr, "%f;%f", &(tmp.fX_Oppose2), &(tmp.fY_Oppose2));
		pStr = csv[j]["�Է�������3"].Str();     sscanf(pStr, "%f;%f", &(tmp.fX_Oppose3), &(tmp.fY_Oppose3));
        pStr = csv[j]["����������"].Str();  sscanf(pStr, "%f;%f", &(tmp.fRebirthRedX1), &(tmp.fRebirthRedY1));
		pStr = csv[j]["����������2"].Str();  sscanf(pStr, "%f;%f", &(tmp.fRebirthRedX2), &(tmp.fRebirthRedY2));
		pStr = csv[j]["����������3"].Str();  sscanf(pStr, "%f;%f", &(tmp.fRebirthRedX3), &(tmp.fRebirthRedY3));
		pStr = csv[j]["����������4"].Str();  sscanf(pStr, "%f;%f", &(tmp.fRebirthRedX4), &(tmp.fRebirthRedY4));
		pStr = csv[j]["����������5"].Str();  sscanf(pStr, "%f;%f", &(tmp.fRebirthRedX5), &(tmp.fRebirthRedY5));
		tmp.iPKAttr = csv[j]["PK����"].Int();
		int iFaction = csv[j]["��Ӫ"].Int();
		if (iFaction==0)		tmp.sceneFaction = FACTION_ZHOU;
		else if (iFaction==1)	tmp.sceneFaction = FACTION_SHANG;
		else if (iFaction==3)   tmp.sceneFaction = FACTION_NEUTRAL;
		//else					tmp.sceneFaction = FACTION_ALL;
		/* gao 2010.1.20
		��ȡ������ͼ�������
		*/
		tmp.szSceneMiddleMapFileName	= csv[j]["������ͼ"].Str();
		tmp.szMiddleMapName				= csv[j]["������"].Str();
		tmp.iMiddleMapHeight			= csv[j]["�����߶�"].Int();
		tmp.iMiddleMapWidth				= csv[j]["�������"].Int();
		tmp.iMiddleMapHeightOffset		= csv[j]["BLOCKƫ��Y"].Int();
		tmp.iMiddleMapWidthOffset		= csv[j]["BLOCKƫ��X"].Int();
		tmp.iMiddleMapGroup				= csv[j]["Ѱ·���ͷ���"].Int();
		/* end */
		RtCoreLog().Info("SceneFile (%s,%d,%d)\n", tmp.szSceneFileName.c_str(), iFaction, tmp.sceneFaction);
        m_table.insert(EXT_SPACE::unordered_map<std::string, SSceneInfo>::value_type(tmp.szSceneFileName, tmp));
    }
    return true;
}

SSceneInfo* CRS_SceneInfo::FindScene(std::string& szBlockName)
{
    HASH_MULTISTR(std::string, SSceneInfo)::iterator it;
    it = m_table.find(szBlockName);
    if (it!=m_table.end())
    {
        return &((*it).second);
    }
    return NULL;
}
void CRS_SceneInfo::GetAllMiddleMapName(map<string,SSceneInfo>& lstName)
{
	HASH_MULTISTR(std::string, SSceneInfo)::iterator it;
	map<string,SSceneInfo>::iterator mapit;
	for(it = m_table.begin();it != m_table.end();++it)
	{
		mapit = lstName.find((*it).second.szMiddleMapName);
		if(mapit == lstName.end() && (*it).second.szMiddleMapName != "")
		{
			SSceneInfo tmp = (*it).second;
			string sMiddleMapName = (*it).second.szMiddleMapName;

			lstName.insert(make_pair(sMiddleMapName,tmp));
		}
	}
}

// ----------------------------------------------------------------------------------
// task info 
// ----------------------------------------------------------------------------------
CTaskInfo::CTaskInfo() 
{
	m_currCircle=0; 
	m_isActive=1; 
	m_bisDayTimeTaskFinish = false; 
	m_nMainTaskData = -1;
	int i;
	for (i = 0; i < c_g_nMaxGuideTafNum; i++)
	{
		m_nGuideTag[i] = 0;
	}
}

CTaskInfo::~CTaskInfo() {}

STaskInfo *CTaskInfo::FindTask(DWORD TaskId)
{
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		STaskInfo &info = *it;
		if(info.Id == TaskId) 
		{
			return &info;
		}
	}
	return NULL;
}

bool CTaskInfo::RemoveTask(DWORD TaskId)
{
	bool bRet = false;
	do 
	{
		list<STaskInfo>::iterator it;
		for(it = m_taskList.begin(); it != m_taskList.end(); it++)
		{
			if((*it).Id == TaskId)
			{
				m_taskList.erase(it);
				bRet = true;
				break;
			}
		}
	} while (false);
	
	return bRet;
}

bool CTaskInfo::AddTask(DWORD TaskId)
{
	if(FindTask(TaskId))
		return false;

	STaskInfo task;
	task.BeginTime = 0;
	task.FinishNum = 0;
	task.LastFinishTime = 0;
	task.Id        = TaskId;
	task.FSetDoing();//task.IsDoing   = 0;
	task.DayTimeTaskFinishTime = 0;
	STaskDesc *desc = g_TableTask.FindTask((int)(TaskId));
	task.Desc = desc;
	m_taskList.push_back(task);	
	return true;
}

bool CTaskInfo::AcceptTask(DWORD TaskId)
{
	STaskInfo *task = FindTask(TaskId);

	//tim.yang  �ճ�����
	if (task && task->DayTimeTaskFinishTime)
	{
		time_t tmpTime = time(NULL);
		tm now = *(localtime(&tmpTime));

		long lastTime = task->DayTimeTaskFinishTime;
		tm *last = localtime((time_t*)&lastTime);
		if(!last) return false;

		int day = 0;
		day = (now.tm_year - last->tm_year)*365 + (now.tm_yday - last->tm_yday);
		if (0 == day)
		{
			m_bisDayTimeTaskFinish = true;
			return false;
		}
		else
		{
			task->DayTimeTaskFinishTime = 0;
		}
	}
	//end

	if(!task)
		AddTask(TaskId);

	task = FindTask(TaskId);
	if(!task) return false;
	
	//if(/*PZH*//*task->IsDoing == 1*/task->FIsDoing()) 
		//return false; 

	task->BeginTime = time(NULL);
	task->FSetDoing();//task->IsDoing  = 1;
	task->dwUseItem = 0;
	//task->Desc = g_TableTask.FindTask(task->Id);
	return true;
}

bool CTaskInfo::FinishTask(STaskDesc *task)
{
	STaskInfo *info = FindTask(task->id);
	if(!info)
		return false;

	info->FinishNum++;
	//info->FSetDone(); //info->IsDoing = 0;//PZH
	info->LastFinishTime = time(NULL);

	//tim.yang  �ճ�����
	if (task->group == -1 && info->DayTimeTaskFinishTime == 0)
	{
		info->DayTimeTaskFinishTime = time(NULL);
	}
	//end

	//// ֻ������ʱ����Ҫ��Ͳ����ظ���������
	//if(!task->repeat || task->getTimeLimit > 0)
	//{
	//	
	//	
	//}
	//else
	//{
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if((*it).Id == task->id)
		{
			m_taskList.erase(it);
			break;
		}
	}
	//}
	if(!task->repeat)
	{
		DoneTaskSet& ds = GetDoneTaskSet();
		ds.insert(task->id);
	}
	return true;
}

bool CTaskInfo::CancelTask(DWORD TaskId)
{
	STaskInfo *task = FindTask(TaskId);
	if(!task)
		return false;

	if(/*PZH*//*task->IsDoing == 0*/!task->FIsDoing())
		return false;
	STaskDesc* pDesc = task->GetDesc();
	if (NULL == pDesc)
	{
		return false;
	}
	if (0 == pDesc->canCancel)
	{
		return false;
	}

	//task->FSetCanecl();//task->IsDoing = 0;//PZH
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if((*it).Id == TaskId)
		{
			m_taskList.erase(it);
			break;
		}
	}
	return true;
}

bool CTaskInfo::IsTaskAccept(DWORD TaskId)
{
	STaskInfo *task = FindTask(TaskId);
	if(!task)
		return false;

	return task->FIsDoing();//task->IsDoing == 1;
}

bool CTaskInfo::IsTaskGroupAccept(DWORD group)
{
	if(group == 0 || group == -1) return false;//tim.yang  �ճ�����
	
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(/*PZH*//*it->IsDoing != 1*/!it->FIsDoing()) continue;
	    STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(desc)
		{
			if(desc->group == group) return true;
		}
	}
	return false;
}

int CTaskInfo::IsDoingMoneyTask()
{
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(/*PZH*//*it->IsDoing != 1*/!it->FIsDoing()) continue;
		STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(desc)
		{
			if(desc->moneyTask) return it->Id;
		}
	}
	return 0;
}

int CTaskInfo::IsDoingWenTask()
{
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(/*PZH*//*it->IsDoing != 1*/!it->FIsDoing()) continue;
		STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(desc)
		{
			if(desc->IsWenTask()/*wenTask*/) return it->Id;
		}
	}
	return 0;
}

STaskInfo* CTaskInfo::FindCircleTask()
{
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(/*PZH*//*it->IsDoing != 1*/!it->FIsDoing()) continue;
		STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(desc)
		{
			if(desc->circleTask) return &(*it);
		}
	}
	return NULL;
}

int  CTaskInfo::GetTaskFinishNum(DWORD TaskId)
{
	STaskInfo *task = FindTask(TaskId);
	if(!task)
		return 0;

	return task->FinishNum;
}

int	 CTaskInfo::GetTaskAcceptNum()
{
	int num = 0;
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(/*PZH*//*(*it).IsDoing*/it->FIsDoing()) num++;
	}
	return num;
}

int CTaskInfo::FindTimeoutTask()
{
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(/*PZH*//*!it->IsDoing*/!it->FIsDoing()) continue;
		if(!it->Desc)
		{
			it->Desc = g_TableTask.FindTask(it->Id);
		}
		if(!it->Desc) continue;
		if(it->Desc->finishTimeLimit == -1) continue;
		if(it->Desc->circleTask) 
		{
			if(m_isActive==0)	continue;
		}
		if((DWORD)time(NULL) > it->BeginTime + it->Desc->finishTimeLimit)
			return it->Id;
	}
	return 0;
}


/*
STaskInfo *CTaskInfo::GetTaskByIndex(DWORD idx)
{
	int num = 0;
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		if(num == idx) return &(*it);
		num++;
	}
	return NULL;
}
*/

void CTaskInfo::SerializeFbTask(CG_CmdPacket *cmd)
{
	long num;
	SFbTaskInfo info;
	if(cmd->IsReading())
	{
		m_fbTaskList.clear();
		cmd->ReadLong(&num);
		for(int i=0; i<num; i++)
		{
			*cmd << info;
			if(cmd->IsError()) return; 
			m_fbTaskList.push_back(info);
		}
	}
	else
	{
		num = (long)m_fbTaskList.size();
		cmd->WriteLong(num);
		for(int i=0; i<num; i++)
			*cmd << m_fbTaskList[i];
	}
}

void CTaskInfo::SerializeTask(CG_CmdPacket *cmd)
{
	long num=0;
	if(cmd->IsReading())
	{
		m_taskList.clear();
		STaskInfo task;
		cmd->ReadLong(&num);
		for(int i=0; i<num; i++) 
		{
			cmd->ReadLong((long*)&task.Id);
			cmd->ReadLong((long*)&task.FinishNum);
			cmd->ReadLong((long*)&task.IsDoing);
			cmd->ReadLong((long*)&task.BeginTime);
			//--------add start by tony 06.05.11-----------------//
			cmd->ReadLong((long*)&task.m_currMonsterNum);
			cmd->ReadLong((long*)&task.m_finditemID);
			cmd->ReadLong((long*)&task.m_findnpcID);
			cmd->ReadLong((long*)&task.m_killmonsterID);
			cmd->ReadLong((long*)&task.m_srcNpcID);
			cmd->ReadLong((long*)&task.dwUseItem);

			//--------add end   by tony 06.05.11-----------------//
			if(cmd->IsError()) return;
			m_taskList.push_back(task);
		}
		//--------add start by tony 06.05.11-----------------//
		cmd->ReadLong((long*)&m_currCircle);
		cmd->ReadByte((char*)&m_isActive);
		//--------add end   by tony 06.05.11-----------------//
		cmd->ReadLong((long*)&m_nMainTaskData);
		int k;
		for (k = 0; k < c_g_nMaxGuideTafNum; k++)
		{
			cmd->ReadLong((long*)&(m_nGuideTag[k]));
		}
		DoneTaskSet& ds = GetDoneTaskSet();
		ds.clear();
		long nSize;
		cmd->ReadLong(&nSize);
		for (k = 0; k < nSize; k++)
		{
			short nTemp;
			cmd->ReadShort(&nTemp);
			ds.insert(nTemp);
		}
	}
	else
	{
		num = (long)m_taskList.size();
		cmd->WriteLong(num);

		list<STaskInfo>::iterator it;
		for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
		{
			//if((*it).IsDoing == 0) continue;
			cmd->WriteLong((*it).Id);
			cmd->WriteLong((*it).FinishNum);
			cmd->WriteLong((*it).IsDoing);
			cmd->WriteLong((*it).BeginTime);
			//--------add start by tony 06.05.11-----------------//
			cmd->WriteLong((*it).m_currMonsterNum);
			cmd->WriteLong((*it).m_finditemID);
			cmd->WriteLong((*it).m_findnpcID);
			cmd->WriteLong((*it).m_killmonsterID);
			cmd->WriteLong((*it).m_srcNpcID);
			cmd->WriteLong((*it).dwUseItem);
			//--------add end   by tony 06.05.11-----------------//
		}
		//--------add start by tony 06.05.11-----------------//
		cmd->WriteLong(m_currCircle);
		cmd->WriteByte(m_isActive);
		//--------add end   by tony 06.05.11-----------------//
		cmd->WriteLong(m_nMainTaskData);
		int k;
		for (k = 0; k < c_g_nMaxGuideTafNum; k++)
		{
			cmd->WriteLong(m_nGuideTag[k]);
		}
		DoneTaskSet& ds = GetDoneTaskSet();
		long nSize = (long)(ds.size());
		cmd->WriteLong(nSize);
		for(DoneTaskSet::iterator iter = ds.begin(); ds.end() != iter; iter++)
		{
			short nTemp = *iter;
			cmd->WriteShort(nTemp);
		}
	}
}

void CTaskInfo::CleanTask()
{
	// �������ʱ�����Ƶ�����
	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end();)
	{
		if(/*PZH*//*it->IsDoing*/it->FIsDoing()) 
		{
			it++;
			continue;
		}
		STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(!desc)
		{
			it = m_taskList.erase(it);
			continue;
		}
	    if(desc->repeat && (time(NULL) - (DWORD)it->LastFinishTime > (DWORD)desc->getTimeLimit) && desc->group!=-1)
		{
			it = m_taskList.erase(it);
			continue;
		}
		it++;
	}
}
//tim.yang  MC  �Ƴ����﹥������
void CTaskInfo::RemoveMCTask()
{
	std::list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end();)
	{
		STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(!desc)
		{
			++it;
			continue;
		}
		if (desc->id == 9017 || desc->id == 9117)//tim.yang  MC  ��������������
		{
			(*it).FSetDoing();//(*it).IsDoing = 0;//PZH
			it = m_taskList.erase(it);
			break;
		}
                ++it;
	}
}
//end

int CTaskInfo::GetTaskInfosForID(std::vector<int> &oInfos)
{
	int iHasTaskCount = 0;
	oInfos.clear();
	std::list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); ++it)
	{
		oInfos.push_back((int)(*it).Id);
		++iHasTaskCount;
	}

	return iHasTaskCount;
}

bool CTaskInfo::SaveToString(string& str)
{
	CleanTask();
	RemoveMCTask();//tim.yang MC
	str = "";
	// add version info
	//--------chg start by tony 06.05.11--------------------//
#if 0
	@	#if 0
		@	@	int ver = 1;
	@	#else
		@		int ver = 2;
	@	#endif
#else
	int ver = 6;
#endif
	//--------chg end   by tony 06.05.11--------------------//
	rt2_sprintf(g_strStaticBuffer, "%d,", ver);
	str += g_strStaticBuffer;

	DoneTaskSet& ds = GetDoneTaskSet();
	int nSize = (int)(ds.size());
	rt2_sprintf(g_strStaticBuffer, "%d,", nSize);
	str += g_strStaticBuffer;
	DoneTaskSet::iterator iter;
	for (iter = ds.begin(); ds.end() != iter; iter++)
	{
		rt2_sprintf(g_strStaticBuffer, "%d,", *iter);
		str += g_strStaticBuffer;
	}
	int k;
	for (k = 0; k < c_g_nMaxGuideTafNum; k++)
	{
		rt2_sprintf(g_strStaticBuffer, "%d,", m_nGuideTag[k]);
		str += g_strStaticBuffer;
	}

	rt2_sprintf(g_strStaticBuffer, "%d,", m_nMainTaskData);
	str += g_strStaticBuffer;
	//--------add start by tony 06.05.11--------------------//
	//-ver==2
	rt2_sprintf(g_strStaticBuffer, "%d,", m_currCircle);
	str += g_strStaticBuffer;
	//--------add end   by tony 06.05.11--------------------//

	//--------add start by tony 06.06.20--------------------//
	//-ver==3
	rt2_sprintf(g_strStaticBuffer, "%d,", (int)m_isActive);
	str += g_strStaticBuffer;
	rt2_sprintf(g_strStaticBuffer, "%d,", (int)m_passTime);
	str += g_strStaticBuffer;
	//--------add end   by tony 06.06.20--------------------//

	int num = (int)m_taskList.size();
	rt2_sprintf(g_strStaticBuffer, "%d", num);
	str += g_strStaticBuffer;

	list<STaskInfo>::iterator it;
	for(it=m_taskList.begin(); it!=m_taskList.end(); it++)
	{
		STaskDesc *desc = g_TableTask.FindTask(it->Id);
		if(NULL == desc)
		{
			continue;
		}
		if ((*it).FIsShared())
		{
			continue;
		}
		if (desc->group == -1)
		{
			//--------chg start by tony 06.05.11--------------------//
#if 0
			@		rt2_sprintf(g_strStaticBuffer, ",%d,%d,%d,%d,%d", (*it).Id, (*it).FinishNum, (*it).IsDoing, (*it).BeginTime, (*it).DayTimeTaskFinishTime);
			@		str += g_strStaticBuffer;
#else
			rt2_sprintf(g_strStaticBuffer, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", (*it).Id, (*it).FinishNum, (*it).IsDoing, (*it).BeginTime, (*it).DayTimeTaskFinishTime,
				(*it).m_currMonsterNum, (*it).m_finditemID, (*it).m_findnpcID, (*it).m_srcNpcID,
				(*it).m_killmonsterID, (*it).dwUseItem);
			str += g_strStaticBuffer;
#endif
			//--------chg end   by tony 06.05.11--------------------//
		}
		else
		{
			//--------chg start by tony 06.05.11--------------------//
#if 0
			@		rt2_sprintf(g_strStaticBuffer, ",%d,%d,%d,%d", (*it).Id, (*it).FinishNum, (*it).IsDoing, (*it).BeginTime);
			@		str += g_strStaticBuffer;
#else
			rt2_sprintf(g_strStaticBuffer, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", (*it).Id, (*it).FinishNum, (*it).IsDoing, (*it).BeginTime,
				(*it).m_currMonsterNum, (*it).m_finditemID, (*it).m_findnpcID, (*it).m_srcNpcID,
				(*it).m_killmonsterID, (*it).dwUseItem);
			str += g_strStaticBuffer;
#endif
			//--------chg end   by tony 06.05.11--------------------//

		}
	}

	return true;
}

bool CTaskInfo::LoadFromString(const string& str)
{
	if (str=="")	return true;

	string strClone = str; // "strtok" will destroy string, so clone a new one and opeate the new
	char* pReadPtr = (char *)strClone.c_str();

	char sep[] = ",";
	DWORD taskID = 0;
	STaskInfo *pTask = NULL;
	char *k = strtok(pReadPtr, sep);
	BOOL_ENSURE(k)

		int ver = atol(k);
	if(ver == 5)
	{
		int kk;
		for (kk = 0; kk < c_g_nMaxGuideTafNum; kk++)
		{
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				m_nGuideTag[kk] = atoi(k);
		}

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_nMainTaskData = atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_currCircle = atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_isActive = (BYTE)atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_passTime = atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			int num = atoi(k);

		for (int i=0; i<num; i++)
		{
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				taskID = atoi(k);
			BOOL_ENSURE(AddTask(taskID))
				pTask = FindTask(taskID);
			BOOL_ENSURE(pTask);

			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->FinishNum = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->IsDoing = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->BeginTime = atoi(k);


			STaskDesc *desc = g_TableTask.FindTask(taskID);
			if (desc && desc->group == -1)
			{			
				k = strtok(NULL, sep);
				BOOL_ENSURE(k)
					pTask->DayTimeTaskFinishTime = atoi(k);
			}


			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_currMonsterNum = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_finditemID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_findnpcID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_srcNpcID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_killmonsterID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->dwUseItem = atoi(k);
		}
	}
	else if (6 == ver)
	{
		DoneTaskSet& ds = GetDoneTaskSet();
		ds.clear();
		int nSize = 0;

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			nSize = atoi(k);
		int kk;
		for (kk = 0; kk < nSize; kk++)
		{
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				int nID = atoi(k);
			ds.insert(nID);
		}

		for (kk = 0; kk < c_g_nMaxGuideTafNum; kk++)
		{
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				m_nGuideTag[kk] = atoi(k);
		}

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_nMainTaskData = atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_currCircle = atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_isActive = (BYTE)atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			m_passTime = atoi(k);

		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
			int num = atoi(k);

		for (int i=0; i<num; i++)
		{
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				taskID = atoi(k);
			BOOL_ENSURE(AddTask(taskID))
				pTask = FindTask(taskID);
			BOOL_ENSURE(pTask);

			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->FinishNum = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->IsDoing = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->BeginTime = atoi(k);


			STaskDesc *desc = g_TableTask.FindTask(taskID);
			if (desc && desc->group == -1)
			{			
				k = strtok(NULL, sep);
				BOOL_ENSURE(k)
					pTask->DayTimeTaskFinishTime = atoi(k);
			}


			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_currMonsterNum = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_finditemID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_findnpcID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_srcNpcID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->m_killmonsterID = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				pTask->dwUseItem = atoi(k);
		}
	}
	//------add end   by tony 06.06.20---------------------//
	else
	{
		RtCoreLog().Info("unknow task version\n");
		return false;
	}

	return true;
}

bool CTaskInfo::IsDoneGuild(GuideType type)
{
	bool bRet = true;
	do 
	{
		int nOnceSize = sizeof(m_nGuideTag[0]) * 8;
		int n = type / nOnceSize;
		int k = type % nOnceSize;
		unsigned long dwMark = 1 << k;
		unsigned long dwRe = dwMark & m_nGuideTag[n];
		bRet = dwRe != 0;
	} while (false);
	return bRet;
}

void CTaskInfo::SetDoneGuild(GuideType type, bool bDone)
{
	do 
	{
		int nOnceSize = sizeof(m_nGuideTag[0]) * 8;
		int n = type / nOnceSize;
		int k = type % nOnceSize;
		unsigned long dwMark;
		if (bDone)
		{
			dwMark = 1 << k;
			m_nGuideTag[n] = m_nGuideTag[n] | dwMark;
		}
		else
		{
			dwMark = 1 << k;
			dwMark = ~ dwMark;
			m_nGuideTag[n] = m_nGuideTag[n] & dwMark;
		}
	} while (false);
}


bool CTaskInfo::IsSendNewPlayerMail(int type)
{
	bool bRet = true;
	do 
	{
		int n =2;
		int k = type;
		unsigned long dwMark = 1 << k;
		unsigned long dwRe = dwMark & m_nGuideTag[n];
		bRet = dwRe != 0;
	} while (false);
	return bRet;
}

void CTaskInfo::SetSendNewPlayerMail(int type, bool bDone)
{
	do 
	{
		int n =2;
		int k = type;
		unsigned long dwMark;
		if (bDone)
		{
			dwMark = 1 << k;
			m_nGuideTag[n] = m_nGuideTag[n] | dwMark;
		}
		else
		{
			dwMark = 1 << k;
			dwMark = ~ dwMark;
			m_nGuideTag[n] = m_nGuideTag[n] & dwMark;
		}
	} while (false);
}

//
// fb system
//
// fb task info
bool CTaskInfo::SaveFbTask(std::string &str)
{
	str = "";
	
	// add version info
	int ver = 1;
	rt2_sprintf(g_strStaticBuffer, "%d,", ver);
	str += g_strStaticBuffer;

	int num = (int)m_fbTaskList.size();
	rt2_sprintf(g_strStaticBuffer, "%d,", num);
	str += g_strStaticBuffer;

	vector<SFbTaskInfo>::iterator it;
	SFbTaskInfo *tmp;
	for(int j=0; j<num; j++)
	{
		tmp = &m_fbTaskList[j];
		
		long num = (long)(tmp->finTask.size());
		if(tmp->curDif<0 || tmp->curDif>=MAX_FB_LEVEL)
		{
			RtCoreLog().Info("save user fb dif err [%d]\n",tmp->curDif);
			tmp->curDif = 0;
		}

		rt2_sprintf(g_strStaticBuffer, "%d,%d,%d,",tmp->fbId, tmp->curDif, num);
		str += g_strStaticBuffer;
		for(int i=0; i<num; i++)
		{
			rt2_sprintf(g_strStaticBuffer, "%d,", tmp->finTask[i]);
			str += g_strStaticBuffer;
		}
	}
	return true;
}

bool CTaskInfo::LoadFbTask(std::string &str)
{
	if (str=="")	return true;

	string strClone = str; // "strtok" will destroy string, so clone a new one and opeate the new
	char* pReadPtr = (char *)strClone.c_str();

	char sep[] = ",";
	char *k = strtok(pReadPtr, sep);
	BOOL_ENSURE(k)

	int ver = atol(k);
	if(ver == 1)
	{
		k = strtok(NULL, sep);
		BOOL_ENSURE(k)
		int num = atoi(k);

		for (int i=0; i<num; i++)
		{
			SFbTaskInfo tmp;
			long finnum = 0;
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				tmp.fbId = atoi(k);
			k = strtok(NULL, sep);
			BOOL_ENSURE(k)
				tmp.curDif = atoi(k);
			
			if(tmp.curDif<0 || tmp.curDif>=MAX_FB_LEVEL)
			{
				RtCoreLog().Info("load user fb dif err [%d]\n",tmp.curDif);
				tmp.curDif = 0;
			}

			k = strtok(NULL, sep);
			BOOL_ENSURE(k);
				finnum = atoi(k);
			for(int j=0; j<finnum; j++)
			{
				k = strtok(NULL, sep);
				BOOL_ENSURE(k)
					tmp.finTask.push_back(atoi(k));
			}
			m_fbTaskList.push_back(tmp);			
		}
	}
	else
	{
		RtCoreLog().Info("unknow fb task version\n");
		return false;
	}

	return true;
}

bool CTaskInfo::IsFbTaskAccept(long taskId,long dif)
{
	SFbTask *task = g_TableTask.FindFbTask(taskId);
	if(!task) return false;
	
	if(task->visDif > dif) return false;

	SFbTaskInfo *info = GetFbTaskInfo(task->fbId);
	if(!info) return false;

	if(info->IsTaskFinished(taskId))
		return false;

	if(info->curDif != dif) return false;
	return true;
}

bool CTaskInfo::CheckAllFbTask()
{
	bool changed = false;
	for(int i=0; i<(int)m_fbTaskList.size(); i++)
	{
		if(m_fbTaskList[i].CanLevelUp())
		{
			m_fbTaskList[i].LevelUp();
			changed = true;
		}
	}
	return changed;
}

SFbTaskInfo *CTaskInfo::GetFbTaskInfo(int fbTid)
{
	for(int i=0; i<(int)m_fbTaskList.size(); i++)
		if(m_fbTaskList[i].fbId == fbTid)
			return &m_fbTaskList[i];

	return NULL;
}

void CTaskInfo::AddFb(int fbTid)
{
	if(GetFbTaskInfo(fbTid))
		return;

	SFbTaskInfo info;
	info.fbId = fbTid;
	info.curDif = 0;
	m_fbTaskList.push_back(info);
}


CTaskInfo::DoneTaskSet& CTaskInfo:: GetDoneTaskSet()
{
	return m_DoneTaskSet;
}

bool CTaskInfo::IsDoneTask(unsigned long TaskId)
{
	bool bRet = false;
	do 
	{
		unsigned short sID = (unsigned short)(TaskId);
		DoneTaskSet& ds = GetDoneTaskSet();
		DoneTaskSet::iterator iter = ds.find(sID);
		bRet = ds.end() != iter;
	} while (false);
	return bRet;
}
//
// for magic weapon 
//
void GetTrumpElement(SItemID &item,int Damage[MAX_ELEMENT_ATTR],int Armor[MAX_ELEMENT_ATTR])
{
	Damage[ELEMENT_WATER]	= item.params[TRUMP_WATER_ATT];
	Damage[ELEMENT_FIRE]	= item.params[TRUMP_FIRE_ATT];
	Damage[ELEMENT_POISON]	= item.params[TRUMP_POISON_ATT];

	Armor[ELEMENT_WATER]	= item.params[TRUMP_WATER_DEF];
	Armor[ELEMENT_FIRE]		= item.params[TRUMP_FIRE_DEF];
	Armor[ELEMENT_POISON]	= item.params[TRUMP_POISON_DEF];
}

void MW_GetEleValue(SItemID &item,int ele[MAX_ELEMENT_ATTR])
{
	ele[ELEMENT_WATER]  = item.params[DEF_POISON];
	ele[ELEMENT_FIRE]   = item.params[MW_PARAM_ELE_FIRE];
	ele[ELEMENT_POISON] = item.params[MW_PARAM_ELE_POISON];

	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
	{
		if(ele[i] < 0) ele[i] = 0;
		if(ele[i] > 10000) ele[i] = 0;
	}
}

void MW_SetEleValue(SItemID &item,int ele[MAX_ELEMENT_ATTR])
{
	long tmp = 0;
	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
	{
		if(ele[i] < 0) ele[i] = 0;
		if(ele[i] > 10000) ele[i] = 0;
	}
	
	item.params[MW_PARAM_ELE_WATOR]  = ele[ELEMENT_WATER];  
	item.params[MW_PARAM_ELE_FIRE]	 = ele[ELEMENT_FIRE];   
	item.params[MW_PARAM_ELE_POISON] = ele[ELEMENT_POISON]; 
}

int MW_GetEleNum(int ele[MAX_ELEMENT_ATTR])
{
	int num = 0;
	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
	{
		if(ele[i] > 0) num++;
	}
	return num;
}

bool MW_GetMainEle(int ele[MAX_ELEMENT_ATTR],int &main)
{
	if(ele[ELEMENT_WATER]==0 && ele[ELEMENT_FIRE]==0 && ele[ELEMENT_POISON]==0)
	{
		return false;
	}

	if(ele[ELEMENT_WATER] >= ele[ELEMENT_FIRE])
		main = ELEMENT_WATER;
	else
		main = ELEMENT_FIRE;
	if(ele[main] < ele[ELEMENT_POISON])
		main = ELEMENT_POISON;

	return true;
}

struct CompEle
{
	int ele;
	int value;
};

void MW_ComputeAttenuation(int ele[MAX_ELEMENT_ATTR],int val[MAX_ELEMENT_ATTR])
{
	if(ele[ELEMENT_WATER]==0 && ele[ELEMENT_FIRE]==0 && ele[ELEMENT_POISON]==0)
	{
		//val[ELEMENT_WATER] *= 0.5;
		//val[ELEMENT_FIRE] *= 0.5;
		//val[ELEMENT_POISON] *= 0.5;
		return;
	}

	CompEle com[MAX_ELEMENT_ATTR],tmp;
    com[0].ele = ELEMENT_WATER;
	com[0].value = ele[ELEMENT_WATER];

	com[1].ele = ELEMENT_FIRE;
	com[1].value = ele[ELEMENT_FIRE];

	com[2].ele = ELEMENT_POISON;
	com[2].value = ele[ELEMENT_POISON];

	for(int i=0; i<MAX_ELEMENT_ATTR; i++)
	{
		for(int j=i; j<MAX_ELEMENT_ATTR; j++)
		{
			if(com[j].value > com[i].value)
			{
				tmp = com[j];
				com[j] = com[i];
				com[i] = tmp;
			}
		}
	}
	val[com[0].ele] *= 1.0;
	val[com[1].ele] *= 0.5;
	val[com[2].ele] *= 0.25;
}

//
// rate = 50 - 100
//  
//
void MW_GetAddEleInfo(SItemID &item,int element,float &rate,long &time,long &mp,bool &lost)
{
	int ele[MAX_ELEMENT_ATTR];
	MW_GetEleValue(item,ele);

	float num = MW_GetEleNum(ele);
	if(num == 0) num = 1;

	int limit = item.params[MW_PARAM_LEV] * item.params[MW_PARAM_LEV] / 5;
	float next = ele[element] + 5;//tim.yang  ����ע���5
	
	lost = false;

	// �չ˵͵ȼ����
	/*
	if(next <= 80)
	{
		rate = 1.0;
	}
	else
	{	
	*/

	if(next <= limit/2)
	{
		rate = 1.0;
	}
	else if(next <= limit)
	{
		rate = 0.5+((float)limit-next)/(limit/2)/2;
	}
	else if(next <= limit * 1.5)
	{
		rate = 0.5;
		lost = true;
	}
	else
	{
		rate = 0.1;
		lost = true;
	}
	mp = 200 * (1+(num-1)*0.5) * MW_MP_SCALE;
	
	if(rate > 1) rate = 1;
	if(rate < 0.5) rate = 0.5;
}

void MW_GetInfo(SItemID &item,int &maxMp)
{
	maxMp = (item.params[MW_PARAM_LEV]/5.0f) * 200 * 10 * MW_MP_SCALE;
}
const int RifineToTrumpParam(int iRifine)
{
	switch(iRifine)
	{
	case MP_TRUMP:
		return TRUMP_LEV;
	case ATTACK_WATER:
		return TRUMP_WATER_ATT;
	case DEF_WATER:
		return TRUMP_WATER_DEF;
	case ATTACK_FIRE:
		return TRUMP_FIRE_ATT;
	case DEF_FIRE:
		return TRUMP_FIRE_DEF;
	case ATTACK_POISON:
		return TRUMP_POISON_ATT;
	case DEF_POISON:
		return TRUMP_POISON_DEF;
	default:
		return TRUMP_LEV;
	}
}
const int RifineToTrumpParamExp(int iRifine)
{
	switch(iRifine)
	{
	case MP_TRUMP:
		return TRUMP_MP;
	case ATTACK_WATER:
		return TRUMP_WATER_ATT_EXP;
	case DEF_WATER:
		return TRUMP_WATER_DEF_EXP;
	case ATTACK_FIRE:
		return TRUMP_FIRE_ATT_EXP;
	case DEF_FIRE:
		return TRUMP_FIRE_DEF_EXP;
	case ATTACK_POISON:
		return TRUMP_POISON_ATT_EXP;
	case DEF_POISON:
		return TRUMP_POISON_DEF_EXP;
	default:
		return TRUMP_MP;
	}
}

int TRUMP_ActorSoulSec(SItemID &itemID, int rifine, STrump *pTrump)
{
	if ( rifine == PNEUMA_TRUMP )
	{
		return 200;
	}
	if (ItemID_IsValid(itemID) && ItemIsTrump(itemID))
	{
		//if (!pTrump)
		//{
		//	return 100*1*(int(itemID.params[TRUMP_LEV]/20)+1)*itemID.params[TRUMP_LEV]/(4+itemID.params[TRUMP_LEV]);
		//}
		if (rifine < 6)
		{
			return 100*1*(int(itemID.params[TRUMP_LEV]/20)+1)*itemID.params[TRUMP_LEV]/(4+itemID.params[TRUMP_LEV]) / 10.f;
		}
		return 100*1*(int(itemID.params[TRUMP_LEV]/20)+1)*itemID.params[TRUMP_LEV]/(4+itemID.params[TRUMP_LEV]);
	}else{
		return 0;
	}
}
float	TRUMP_CentiSoul( SItemID &itemID, int rifine )
{
	if ( rifine == PNEUMA_TRUMP )
	{
		return 60.f;
	}
	if (ItemID_IsValid(itemID) && ItemIsTrump(itemID))
	{
		switch (rifine)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			return 0.1f * (float(itemID.params[TRUMP_LEV]/20)+1) * float(itemID.params[TRUMP_LEV]);
			break;
		case 6:
			return 1.f * (float(itemID.params[TRUMP_LEV]/20)+1) * float(itemID.params[TRUMP_LEV]);
			break;
		case 7:
			return 60.f;
			break;
		}
	}
	return 0.f;
}

#ifdef REGION_SERVER
void  MW_RecomputAttr(SItem &item)
{
	
}

#endif


// help function
bool TestAttackRange(long attRange,long srcPos[3],SCreModel *srcModel,long tarPos[3],SCreModel *tarModel)
{
	float disx = srcPos[0] - tarPos[0];
	float disy = srcPos[1] - tarPos[1];

	float dis = sqrt(disx*disx + disy*disy);
	return attRange > dis - tarModel->ModelRadius;
}

bool TestCollision(long srcPos[3],SCreModel *srcModel,long tarPos[3],SCreModel *tarModel)
{
	float disx = srcPos[0] - tarPos[0];
	float disy = srcPos[1] - tarPos[1];

	float dis = sqrt(disx*disx + disy*disy);
	return dis < srcModel->ModelRadius + tarModel->ModelRadius;
}

bool NpcHaveFunction(SNpc* pNpcInfo, ENpcFunction func)
{
	if (!pNpcInfo)		return false;

    int i;
	vector<long>& functions = pNpcInfo->Function;
	for (i=0; i<(int)functions.size(); i++)
	{
		if (functions[i]==func)
			break;
	}

	if (i>=(int)functions.size())
		return false;
	else 
		return true;
}

void SUserDataAssert_CreateMD5String12(SUserDataAssert& uda, std::string& outString)
{
	char data[33];
	rtMD5DataString((char*)(&uda), sizeof(SUserDataAssert), data);

	CHECK(SUserDataAssert_Version<100 && SUserDataAssert_Version>0);
	char tmp64[64];
	rt2_sprintf(tmp64, "%02d", SUserDataAssert_Version);
	outString = tmp64;
	memcpy(tmp64, data, 10);
	tmp64[10] = '\0';
	outString += tmp64;

	if (outString.size()!=12)
	{
		RtCoreLog().Info("[SUserDataAssert_CreateMD5String12] Create MD5 Error(size=%d)\n", (int)outString.size());
	}
}

// hash 
/*
** A simple and fast generic string hasher based on Peter K. Pearson's
** article in CACM 33-6, pp. 677.
*/
static int T[] =
{
	1, 87, 49, 12, 176, 178, 102, 166, 121, 193, 6, 84, 249, 230, 44, 163,
	14, 197, 213, 181, 161, 85, 218, 80, 64, 239, 24, 226, 236, 142, 38, 200,
	110, 177, 104, 103, 141, 253, 255, 50, 77, 101, 81, 18, 45, 96, 31, 222,
	25, 107, 190, 70, 86, 237, 240, 34, 72, 242, 20, 214, 244, 227, 149, 235,
	97, 234, 57, 22, 60, 250, 82, 175, 208, 5, 127, 199, 111, 62, 135, 248,
	174, 169, 211, 58, 66, 154, 106, 195, 245, 171, 17, 187, 182, 179, 0, 243,
	132, 56, 148, 75, 128, 133, 158, 100, 130, 126, 91, 13, 153, 246, 216, 219,
	119, 68, 223, 78, 83, 88, 201, 99, 122, 11, 92, 32, 136, 114, 52, 10,
	138, 30, 48, 183, 156, 35, 61, 26, 143, 74, 251, 94, 129, 162, 63, 152,
	170, 7, 115, 167, 241, 206, 3, 150, 55, 59, 151, 220, 90, 53, 23, 131,
	125, 173, 15, 238, 79, 95, 89, 16, 105, 137, 225, 224, 217, 160, 37, 123,
	118, 73, 2, 157, 46, 116, 9, 145, 134, 228, 207, 212, 202, 215, 69, 229,
	27, 188, 67, 124, 168, 252, 42, 4, 29, 108, 21, 247, 19, 205, 39, 203,
	233, 40, 186, 147, 198, 192, 155, 33, 164, 191, 98, 204, 165, 180, 117, 76,
	140, 36, 210, 172, 41, 54, 159, 8, 185, 232, 113, 196, 231, 47, 146, 120,
	51, 65, 28, 144, 254, 221, 93, 189, 194, 139, 112, 43, 71, 109, 184, 209,
};

int ha_P3(char const*s, int maxn, int hashs)
{
	 unsigned int h;
	 unsigned char *p;

	h = (unsigned char) *s;
	if (h) {
		if (hashs > 256) {
			 int oh = T[(unsigned char) *s];

			for (p = (unsigned char *) s + 1; *p && p <= (unsigned char *) s + maxn; p++) {
				h = T[h ^ *p];
				oh = T[oh ^ *p];
			}
			h |= (oh << 8);
		} else
			for (p = (unsigned char *) s + 1; *p && p <= (unsigned char *) s + maxn; p++)
				h = T[h ^ *p];
	}
	return (int) (h % hashs);
}


/*
* whashstr is faster than hashstr, but requires an even power-of-2 table size
* Taken from Amylaars driver.
*/

int ha_P2(char const*s, int maxn)
{
	 unsigned char oh, h;
	 unsigned char *p;
	 int i;

	if (!*s)
		return 0;
	p = (unsigned char *) s;
	oh = T[*p];
	h = (*(p++) + 1) & 0xff;
	for (i = maxn - 1; *p && --i >= 0; ) {
		oh = T[oh ^ *p];
		h = T[h ^ *(p++)];
	}

	return (oh << 8) + h;
}


bool SNpcSellRareItem::Load(const std::string& str)
{
    char cTmp1024[1024];
    if (str.size()>=1024)
    {
        RtCoreLog().Info("[SNpcSellRareItem::Load] Clause is too long!(MaxSize=%d)\n", 1024);
        return false;
    }
    rt2_strncpy(cTmp1024, str.c_str(), 1024);

    char sep[] = "&";

    char* p = strtok(cTmp1024, sep);
    if (!p)		return false;
    itemType = atoi(p);
    p = strtok(NULL, sep);
    if (!p)		return false;
    maxCount = atoi(p);
    p = strtok(NULL, sep);
    if (!p)		return false;
    interval = atoi(p) * 60;

    return true;
}

bool SNpcSellRareItem::Create(CItemManager* pItemMgr)
{
    if (itemType==0)
        return false;

    item = pItemMgr->CreateItem(itemType);
    if (!ItemID_IsValid(item))
        return false;

    return true;
}


// These two points never been deleted louis [8/4/2009 Louis.Huang]
RtIni* gConfigBoot;
RtIni* gGameRule;

bool LoadConfig(std::string& vrError)
{
	/*RtIni VersionConfig;
	if(VersionConfig.OpenFile("version_config.ini"))
	{
		VersionConfig.CloseFile();
	}
	else
	{
		vrError = "Error: Cannot open game setting file [version_config.ini]\n Please check if the directory is correct or re-install the game";
		return false;
	}

	const char* pConfigBootFileName;
	pConfigBootFileName = VersionConfig.GetEntry("VersionConfig","ConfigBootFile");
	if(pConfigBootFileName == NULL)
	{
		vrError = "Error: \"ConfigBootFile\" is no exit in [version_config.ini]!";
		return false;
	}*/
	// Convert path to right slash.HACK,HACK,HACK
	//GetRightPath((char*)pConfigBootFileName);
	gConfigBoot = RT_NEW RtIni();
    if (gConfigBoot->OpenFile("config_boot.ini"))
	{
		gConfigBoot->CloseFile();
	}
	else
	{
		vrError = "Error: [";
        vrError += "config_boot.ini";
		vrError += "] is no find!";
		return false;
	}

	//����game_rule.ini�ļ�
	const char* pGameRuleFileName;
	pGameRuleFileName = gConfigBoot->GetEntry("ConfigInfo","GameRulePath");
	if(pGameRuleFileName == NULL)
	{
		vrError = "Error: \"GameRuleFile\" is no exit in [game_rule.ini]!";
		return false;
	}

	// Convert path to right slash.HACK,HACK,HACK
	GetRightPath((char*)pGameRuleFileName);
	gGameRule = RT_NEW RtIni();
	if(gGameRule->OpenFile(pGameRuleFileName))
	{
		gGameRule->CloseFile();
	}
	else
	{
		vrError = "Error: [";
		vrError += pGameRuleFileName;
		vrError += "] is no find!";
		return false;
	}


	return true;
};

RtIni* GetConfigBoot()
{
	return gConfigBoot;
};


RtIni* GetGameRule()
{
	return gGameRule;
};

CRS_MainStory::CRS_MainStory()
{

}

CRS_MainStory::~CRS_MainStory()
{

}

CRS_MainStory::MainStoryDataV& CRS_MainStory::GetMainStoryDataV()
{
	return m_MainStoryDataV;
}
bool CRS_MainStory::Load(const char *File)
{
	int j, counter;
	char* k;
	char* pStrTmp;
	const char sep[] = ",;";	// �ָ���
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		MainStoryData tmp;
		tmp.nDataID			 = csv[j]["ID"].Long();
		tmp.strDataName		 = csv[j]["������"].Str();
		tmp.nJob			 = csv[j]["ְҵ"].Long();//ְҵ
		tmp.nGroupID		 = csv[j]["����������"].Long();//����������
		
		tmp.strImage1		 = csv[j]["��������ͼƬ1"].Str();//��������ͼƬ1
		tmp.strImage2		 = csv[j]["��������ͼƬ2"].Str();//��������ͼƬ2
		
		pStrTmp = (char*)(csv[j]["�������ID"].Str());
		counter = 0;
		k = strtok(pStrTmp, sep);
		if (k && strlen(k) > 0)
		{
			while (k && counter < c_nMaxData)
			{
				tmp.nTaskID[counter] = atoi(k);
				counter++;
				k = strtok(NULL, sep);
			}
		}
		tmp.nTaskNum = counter;
		m_MainStoryDataV.push_back(tmp);
	}
	return true;
}

MainStoryData* CRS_MainStory::FindMainStory(int Id)
{
	MainStoryData* pRet = NULL;
	do 
	{
		MainStoryDataV& v = GetMainStoryDataV();
		MainStoryDataV::iterator iter;
		for(iter = v.begin(); v.end() != iter; iter++)
		{
			MainStoryData& T = *iter;
			
			if (T.nDataID == Id)
			{
				pRet = &T;
				break;
			}
		}
	} while (false);
	return pRet;
}

CRS_TaskGroup::CRS_TaskGroup()
{

}

CRS_TaskGroup::~CRS_TaskGroup()
{

}

bool CRS_TaskGroup::Load(const char *File)
{
	int j;
	//char* k;
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		TaskGroupData tmp;
		tmp.nGroupID			 = csv[j]["ID"].Long();
		tmp.strGroupName		 = csv[j]["��������"].Str();
		tmp.strImage1			 = csv[j]["ͼƬ1"].Str();//ͼƬ1
		tmp.strImage2			 = csv[j]["ͼƬ2"].Str();//ͼƬ2
		tmp.strImage3			 = csv[j]["ͼƬ3"].Str();//ͼƬ3
		tmp.strChapter1          = csv[j]["��������1"].Str();     //�µ�����
		tmp.strChapter2          = csv[j]["��������2"].Str();     //�µ�����
		m_TaskGroupDataV.push_back(tmp);
	}
	return true;
}

CRS_UnionIcon::CRS_UnionIcon()
{
	
}

CRS_UnionIcon::~CRS_UnionIcon()
{

}


bool CRS_UnionIcon::Load(const char *File)
{
	int j;
	//char* k;
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ID");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		SUnionIcon tmp;
		tmp.nIconID			 = csv[j]["ID"].Long();
		tmp.strIconName		 = csv[j]["����"].Str();
		tmp.strDesc			 = csv[j]["����"].Str();
		tmp.nMoney			 = csv[j]["�ۼ�"].Long();
		
		m_UnionIconV.push_back(tmp);
	}
	return true;
}

SUnionIcon* CRS_UnionIcon::FindUnionIcon(int Id)
{
	SUnionIcon* pRet = NULL;
	do 
	{
		UnionIconV& v = GetUnionIconV();
		UnionIconV::iterator iter;
		for(iter = v.begin(); v.end() != iter; iter++)
		{
			SUnionIcon& T = *iter;

			if (T.nIconID == Id)
			{
				pRet = &T;
				break;
			}
		}
	} while (false);
	return pRet;
}

CRS_UnionIcon::UnionIconV& CRS_UnionIcon::GetUnionIconV()
{
	return m_UnionIconV;
}

//�̳Ǳ�
CRS_ShopCenter::CRS_ShopCenter()
{

}

CRS_ShopCenter::~CRS_ShopCenter()
{

}

bool CRS_ShopCenter::Load(const char* fileName)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(fileName))
		return false;

	j = csv.FindLineByString("id");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();
	for(j++; j<Total; j++)
	{
		SShopCenter tmp;
		tmp.id			         = csv[j]["id"].Long();
		tmp.itemName		     = csv[j]["����"].Str();
		tmp.itemId			     = csv[j]["����id"].Long();
		tmp.type			     = csv[j]["����"].Short();
		tmp.subType			     = csv[j]["������"].Short();
		tmp.commendType          = csv[j]["�Ƽ�����"].Short();    
		tmp.bHotSale             = csv[j]["�Ƿ�����"].Bool();   
		tmp.fSubPriceRate        = csv[j]["���۶��"].Float();
		tmp.bPresent             = csv[j]["�Ƿ���Ʒ"].Bool();
		tmp.presentType          = csv[j]["��Ʒ����"].Short();
		tmp.timeLimitType        = csv[j]["ʱ����������"].Short();
		tmp.bSetNumber           = csv[j]["�Ƿ�����ø���"].Bool();
		tmp.goldPrice            = csv[j]["�����۸�"].Long();
		tmp.pointPrice           = csv[j]["����۸�"].Long();
		tmp.texName              = csv[j]["ʱװ��ͼ"].Str();
		tmp.bValid               = csv[j]["�Ƿ���Ч"].Bool();
		m_ShopCenterV.push_back(tmp);
	}
	return true;
}

SShopCenter* CRS_ShopCenter::FindShopCenter(int ID)
{
	SShopCenter* pRet = NULL;
	do 
	{
		ShopCenterV& v = GetShopCenterV();
		ShopCenterV::iterator iter;
		for(iter = v.begin(); v.end() != iter; iter++)
		{
			SShopCenter& T = *iter;

			if (T.id == ID)
			{
				pRet = &T;
				break;
			}
		}
	} while (false);
	return pRet;
}

CRS_ShopCenter::ShopCenterV& CRS_ShopCenter::GetShopCenterV()
{
	return m_ShopCenterV;
}

TaskGroupData* CRS_TaskGroup::FindTaskGroup(int Id)
{
	TaskGroupData* pRet = NULL;
	do 
	{
		TaskGroupDataV& v = GetMainStoryDataV();
		TaskGroupDataV::iterator iter;
		for(iter = v.begin(); v.end() != iter; iter++)
		{
			TaskGroupData &T = *iter;
			
			if (T.nGroupID == Id)
			{
				pRet = &T;
				break;
			}
		}
	} while (false);
	return pRet;
}

bool  CRS_TaskGroup::GetMainLineID(vector<int>& taskId)
{
	taskId.clear();

	for(unsigned int i=0; i<m_TaskGroupDataV.size(); i++)
	{
		if(m_TaskGroupDataV[i].nGroupID >= 0 && m_TaskGroupDataV[i].nGroupID < 40)
		{
			if (!m_TaskGroupDataV[i].strGroupName.empty())
			{
				taskId.push_back(m_TaskGroupDataV[i].nGroupID);
			}
		}
	}
	return !(taskId.empty());
}

CRS_TaskGroup::TaskGroupDataV& CRS_TaskGroup::GetMainStoryDataV()
{
	return m_TaskGroupDataV;
}


/////////CRS_AtrribPoint/////////
/////////////////////////////////

CRS_AtrribPoint::CRS_AtrribPoint()
{

}
CRS_AtrribPoint::~CRS_AtrribPoint()
{

}

bool CRS_AtrribPoint::Load(const char *File)
{
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(File))
		return false;

	j = csv.FindLineByString("ְҵ��ֵ");
	if(j<0) return false;
	csv.SetTitleLine(j);

	int Total = csv.GetLineCount();

	for(j++; j<Total; j++)
	{
		AtrribPoint AtPoint;
		AtPoint.Metier = csv[j]["ְҵ��ֵ"].Int();
		AtPoint.PlayerLev = csv[j]["��ҵȼ�"].Float();      //����ȼ�
		AtPoint.GradeLev = csv[j]["��Ϊ�ȼ�"].Float();       //��Ϊ�ȼ�
		AtPoint.PlayerHP = csv[j]["����"].Float();       //���Ѫֵ
		AtPoint.PlayerMp = csv[j]["����"].Float();       //���ħ��ֵ
		AtPoint.PhysicalDamage = csv[j]["����"].Float(); //�����˺�
		AtPoint.PhysicalDefense = csv[j]["���"].Float();//�������
		AtPoint.FireDamage = csv[j]["����"].Float();     //���˺�
		AtPoint.FireDefense = csv[j]["���"].Float();    //�����
		AtPoint.WaterDamage = csv[j]["ˮ��"].Float();    //ˮ�˺�
		AtPoint.WaterDefense = csv[j]["ˮ��"].Float();   //ˮ����
		AtPoint.PoisonDamage = csv[j]["����"].Float();   //���˺�
		AtPoint.PoisonDefense = csv[j]["����"].Float();  //������
		AtPoint.Attack = csv[j]["����"].Float();         //����
		AtPoint.Dodge = csv[j]["����"].Float();          //����
		AtPoint.DamagesSpeed = csv[j]["�����ٶ�"].Float();   //�����ٶ�
		AtPoint.MoveSpeed = csv[j]["�ƶ��ٶ�"].Float();      //�ƶ��ٶ�
		AtPoint.DamageDistance = csv[j]["��������"].Float(); //��������
		AtPoint.SlayR = csv[j]["����һ����"].Float();          //����һ����
		AtPoint.SkillCriRate = csv[j]["����һ����"].Float();   //����һ����
		AtPoint.SkillCriDamage = csv[j]["����һ���˺��ӳ�"].Float(); //����һ���˺��ӳ�
		AtPoint.ReCriticalHit = csv[j]["�ֿ�����һ����"].Float();  //�ֿ�����һ����
		AtPoint.ReSkillCritical = csv[j]["�ֿ�����һ����"].Float();//�ֿ�����һ����
		AtPoint.ReParry = csv[j]["��"].Float();        //��
		AtPoint.AbsorbHp = csv[j]["��Ѫ"].Float();       //��Ѫ
		AtPoint.AbsorbMp = csv[j]["��ħ"].Float();       //����
		AtPoint.RestoreHp = csv[j]["��Ѫ"].Float();      //ÿ3��ظ���Ѫ��
		AtPoint.RestoreMp = csv[j]["��ħ"].Float();      //ÿ3��ظ���ħ����

		m_AtPtable.push_back(AtPoint);
	}
	return true;
}

AtrribPoint* CRS_AtrribPoint::FindPet(int Metier)
{
	for(unsigned int i=0; i<m_AtPtable.size(); i++)
	{
		if(m_AtPtable[i].Metier == Metier) return &m_AtPtable[i];
	}
	return NULL;
}

bool STaskInfo::IsUseItemByID(int nID)
{
	bool bRet = false;
	do 
	{
		STaskDesc* pDesc = g_TableTask.FindTask(this->Id);
		if (NULL == pDesc)
		{
			break;
		}
		vector<STaskKey>::iterator itKey;
		unsigned char cIndex = 0;
		bool bFind = false;
		for(itKey = pDesc->taskReq.begin(); pDesc->taskReq.end() != itKey; itKey++)
		{
			STaskKey &key = *itKey;
			if(key.key == "useitem")
			{
				int nItemID = atoi(key.param[0].c_str());
				if (nItemID == nID)
				{
					bFind = true;
					break;
				}
				cIndex++;
			}
		}
		if (!bFind || cIndex > 4)
		{
			break;
		}
		if (!IsUseItem(cIndex))
		{
			break;
		}
		bRet = true;
	} while (false);
	return bRet;
}
