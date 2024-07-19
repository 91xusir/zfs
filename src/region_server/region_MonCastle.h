//--------------------add start by Tim 09.01.13-----------------------------//
//���﹥�ǣ�ÿ��һ������������19:00����21:00�Ļ
#ifndef _MON_CASTLE_
#define _MON_CASTLE_
#pragma once
#include "region.h"
#define MAX(a,b) (a)>(b)?(a):(b);
#define MIN(a,b) (a)<(b)?(a):(b);
enum MCState
{
	MC_STATE_DONOTHING					= 0,
	MC_STATE_ACTSTART					= 1,
	MC_STATE_ACTEND						= 2,	
	MC_STATE_TASKSTART					= 3,
	MC_STATE_TASKSUCCEED				= 4,
	MC_STATE_TASKFAILED_TIMEOVER		= 5,
	MC_STATE_TASKFAILED_NPCDEAD			= 6,
};

struct SCountyAtrr
{
	SCountyAtrr()
	{
		m_country = -1;
		m_monsterGroup = 0;
		m_monsterMaxGroup = 0;
		memset(m_spMonsterNum,0,sizeof(m_spMonsterNum));
		memset(m_spMonsterId,0,sizeof(m_spMonsterId));
		memset(m_spBossNum,0,sizeof(m_spBossNum));
		memset(m_spBossId,0,sizeof(m_spBossId));
		memset(m_spPosX,0,sizeof(m_spPosX));
		memset(m_spPosY,0,sizeof(m_spPosY));
		m_spPathPoint = "";
		m_targetCre = 0;
		m_mcTaskFailed = 0;
		m_delTargetHp = false;
		m_bfinish = false;
	}
	~SCountyAtrr(){}
	byte m_country;
	std::list<CRegionCreature*> m_mcMonster;//��������list
	int  m_monsterGroup;//ˢ������
	int  m_monsterMaxGroup;//���ˢ������
	int  m_spMonsterNum[10];//
	int  m_spMonsterId[10];
	int  m_spBossNum[10];
	int  m_spBossId[10];
	int  m_spPosX[2];
	int  m_spPosY[2];
	std::string m_spPathPoint;
	ULONG m_targetObjId;
	CRegionCreature* m_targetCre;
	bool m_mcTaskFailed;
	int  m_delTargetHp;
	bool  m_bfinish;
};
class CMonCastle
{
public:
	inline DWORD GetDeadMonsterNum()  {return m_MonsterDeadNum;}
	inline byte  GetState()  {return m_state;}
	inline std::string GetPosVector(byte country) 
	{
		if (1 == country)
		{
			return zhaoge->m_spPathPoint;
		}
		if (0 == country)
		{
			return xiqi->m_spPathPoint;
		}
	}
	inline ULONG GetTargetObjId(byte country)
	{
		if (1 == country)
		{
			return zhaoge->m_targetObjId;
		}
		if (0 == country)
		{
			return xiqi->m_targetObjId;
		}
	}
	//inline int GetTick()  {return m_Tick;}

	void RestoreTargetObjId(ULONG oId,int NpcId);
	void CalMonsterNum();//�����й���ÿ����һ����������һ
	bool LoadMCConfig(const char* szPKConfigFile);
	int  GetMcState(){return m_state;}
	void SystemBroad(int state,byte country=-1,int monsterDeadNum=0,int targetHp = 0, int monsterGroup = 0);//ϵͳ����  1.���￪ʼ����. 2.���ﵽ���� 3.����ɹ� 4.����ʧ��
	void ChectState();//�������ں�ʱ���ж��Ƿ�ʼ�
	void Run();
	void AddMCTask();//��ӹ��﹥�ǵ�����
	void RemoveMCTaskAllUsers(byte country);//�ʱ�䵽������ʧ���Ƴ����﹥�ǵ�����(�������ɾ��������region_creature�����)
	void FinshMCTask(byte country);//����ɹ������뽱��
	void MCTaskRealSpawnMonster(SCountyAtrr* county,int monsterId,int monstercount,int bossId,int bossCount);
	void MCTaskSpawnMonster(SCountyAtrr* county);//����ˢ��
	void RemoveAllMCMonster(byte country);
	void GameOver(byte country);
	void EraseMonsterFromList(CRegionCreature *cre);
	inline static CMonCastle* GetMCInstance()
	{
		if (m_mcInstance == 0) 
		{ 
			m_mcInstance = RT_NEW CMonCastle(); 
		} 
		return m_mcInstance;
	}

	~CMonCastle();
	SCountyAtrr *zhaoge;
	SCountyAtrr *xiqi;
	bool m_isRightStartMC;
	bool m_bstartTask;//��ʼ����
	void WriteConfig(byte easterDay, int startTimeHour, int endTimeHour, int startTimeMin, int endTimeMin, int monserDeadNum);
protected:
private:		
	CMonCastle();
	int   m_MonsterDeadNumInFile;//�ļ�����д�Ĺ�������������
	byte  m_EasterDayInFile;//���
	int   m_startTimeHourInFile;//���ʼʱ��
	int   m_endTimeHourInFile;//�����ʱ��	
	int   m_startTimeMinInFile;
	int   m_endTimeMinInFile;//�����ʱ��

	int   m_MonsterDeadNumMax;//��������������
	byte  m_EasterDay;//���
	int   m_startTimeHour;//���ʼʱ��
	int   m_endTimeHour;//�����ʱ��	
	int   m_startTimeMin;
	int   m_endTimeMin;//�����ʱ��
	int   m_startSec;
	int   m_endSec;

	//int   m_Tick;
	int   m_MonsterDeadNum;//����ʼǰҪ���������������
	bool  m_bfirstStartAct;//���һ�ο�ʼ
	
	SCountyAtrr* _GreateCountyAtrr(RtIni& ini,int byte);//0--xiqi 1--zhaoge
	void _ChangeStringToInt(const std::string str,int* arr);
	int _GetIsNotZeroNumFromArr(int *arr,int length);
	static CMonCastle *m_mcInstance;
	static int m_state;
};
#endif
