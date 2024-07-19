//--------------------add start by Tim 09.01.13-----------------------------//
//怪物攻城，每周一、三、五晚上19:00至就21:00的活动
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
	std::list<CRegionCreature*> m_mcMonster;//保存怪物的list
	int  m_monsterGroup;//刷怪批次
	int  m_monsterMaxGroup;//最大刷怪批次
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
	void CalMonsterNum();//世界中怪物每死亡一个，数量减一
	bool LoadMCConfig(const char* szPKConfigFile);
	int  GetMcState(){return m_state;}
	void SystemBroad(int state,byte country=-1,int monsterDeadNum=0,int targetHp = 0, int monsterGroup = 0);//系统公告  1.怪物开始起义. 2.怪物到计数 3.任务成功 4.任务失败
	void ChectState();//根据日期和时间判断是否开始活动
	void Run();
	void AddMCTask();//添加怪物攻城的任务
	void RemoveMCTaskAllUsers(byte country);//活动时间到，任务失败移除怪物攻城的任务(玩家下线删除任务在region_creature中完成)
	void FinshMCTask(byte country);//任务成功，给与奖励
	void MCTaskRealSpawnMonster(SCountyAtrr* county,int monsterId,int monstercount,int bossId,int bossCount);
	void MCTaskSpawnMonster(SCountyAtrr* county);//任务刷怪
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
	bool m_bstartTask;//开始任务
	void WriteConfig(byte easterDay, int startTimeHour, int endTimeHour, int startTimeMin, int endTimeMin, int monserDeadNum);
protected:
private:		
	CMonCastle();
	int   m_MonsterDeadNumInFile;//文件中填写的怪物死亡的数量
	byte  m_EasterDayInFile;//活动日
	int   m_startTimeHourInFile;//活动开始时间
	int   m_endTimeHourInFile;//活动结束时间	
	int   m_startTimeMinInFile;
	int   m_endTimeMinInFile;//活动结束时间

	int   m_MonsterDeadNumMax;//怪物死亡的数量
	byte  m_EasterDay;//活动日
	int   m_startTimeHour;//活动开始时间
	int   m_endTimeHour;//活动结束时间	
	int   m_startTimeMin;
	int   m_endTimeMin;//活动结束时间
	int   m_startSec;
	int   m_endSec;

	//int   m_Tick;
	int   m_MonsterDeadNum;//任务开始前要求怪物死亡的数量
	bool  m_bfirstStartAct;//活动第一次开始
	
	SCountyAtrr* _GreateCountyAtrr(RtIni& ini,int byte);//0--xiqi 1--zhaoge
	void _ChangeStringToInt(const std::string str,int* arr);
	int _GetIsNotZeroNumFromArr(int *arr,int length);
	static CMonCastle *m_mcInstance;
	static int m_state;
};
#endif
