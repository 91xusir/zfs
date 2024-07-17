
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
    PKA_NONE,       // û������
    PKA_NORMAL,     // ��ͨ����(�к����ͷ�)
    PKA_UNSAFE,     // ����ȫ����(�޺����ͷ�)
    PKA_SAFE,       // ��ȫ��(���˹�ս״̬���ǲ�����PK��)
    PKA_SAFEST,     // ���԰�ȫ��(������)
    PKA_PKUNABLE,   // ����PK(̫��þ�)
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
    long        iMoney;         // ��Ǯ�ļ���
    long        iMoneyMinRate;  // ���ܵ��˶���Ǯ
    long        iMoneyMaxRate;  // ���ܵ��˶���Ǯ

    long        iItem;          // �����ߵļ���
    long        iItemMinPiece;  // ��С��������
    long        iItemMaxPiece;  // ��󼸼�����

    long        iEquip;         // ��װ���ļ���
    long        iEquipMinPiece; // ��С����װ��
    long        iEquipMaxPiece; // ��󼸼�װ��

	int			iOn_Off;		// �Ƿ���ս��
	int			iBeginTime;		// ����ʱ��
	int			iEndTime;		// �ر�ʱ��
	int			iDie_Interval;	// ����������죩
	int			iRandom;		// �����ٵĸ��ʣ�1/random��
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
	SPKLost		lostWhite;//tim.yang  �����������
	SPKLost     exploitManager;//ս������

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
