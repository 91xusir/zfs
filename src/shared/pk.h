
#ifndef _PK_H_
#define _PK_H_

const DWORD PK_TEMP_TIME_MILLSECOND = 60000;
const DWORD PK_DROP_TIME_MILLSECOND = 60000;

struct SPkTempInfo
{
	SPkTempInfo(DWORD id,DWORD time = 0)
	{
		if (time == 0)
			time = rtGetMilliseconds() + PK_TEMP_TIME_MILLSECOND;
		dwTempPkID = id;
		dwTempPkTime = time;
	}
	DWORD dwTempPkID;
	DWORD dwTempPkTime;
};
enum EPKAttr
{
    PKA_NONE,       // 没有设置
    PKA_NORMAL,     // 普通区域(有红名惩罚)
    PKA_UNSAFE,     // 不安全区域(无红名惩罚)
    PKA_SAFE,       // 安全区(除了国战状态都是不允许PK的)
    PKA_SAFEST,     // 绝对安全区(新手区)
    PKA_PKUNABLE,   // 不能PK(太虚幻境)
};

enum EPkMode
{
	PK_Monster = 0,
	PK_Union = 1,
	PK_Feod = 2,
	PK_Faction = 3,
	PK_Goodevil = 4,
	PK_Free = 5
};

struct SPKLost
{
    // money
    long        iMoney;         // 掉钱的几率
    long        iMoneyMinRate;  // 可能掉了多少钱
    long        iMoneyMaxRate;  // 可能掉了多少钱

    long        iItem;          // 掉道具的几率
    long        iItemMinPiece;  // 最小几件道具
    long        iItemMaxPiece;  // 最大几件道具

    long        iEquip;         // 掉装备的几率
    long        iEquipMinPiece; // 最小几件装备
    long        iEquipMaxPiece; // 最大几件装备

	int			iOn_Off;		// 是否开启战功
	int			iBeginTime;		// 开启时间
	int			iEndTime;		// 关闭时间
	int			iDie_Interval;	// 死亡间隔（天）
	int			iRandom;		// 掉器官的概率（1/random）
};

struct SPKConfig
{
    float       fSafeRebirthZouX;
    float       fSafeRebirthZouY;
    float       fSafeRebirthShangX;
    float       fSafeRebirthShangY;
	float		fSafeRebirthZhouOtherX;
	float		fSafeRebirthZhouOtherY;
	float		fSafeRebirthShangOtherX;
	float		fSafeRebirthShangOtherY;

    SPKLost     lostYellow;
    SPKLost     lostRed;
	SPKLost		lostWhite;//tim.yang  白名掉落道具
	SPKLost     exploitManager;//战功管理

    long        iStateChangeTimeYellowName;
    long        iStateChangeTimeRedName;
    long        iStateChangeTimeIncrease;
};

extern SPKConfig *g_pkConfig;

void LoadPKConfig(const char* szPKConfigFile);

#endif // _PK_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
