#include "region_MonCastle.h"
//PZH
#include "PlayerLogic.h"
//
CMonCastle* CMonCastle::m_mcInstance = 0;
int CMonCastle::m_state = MC_STATE_DONOTHING;
CMonCastle::CMonCastle()
:m_MonsterDeadNumInFile(0),m_EasterDayInFile(0),m_startTimeHourInFile(0),m_endTimeHourInFile(0),m_startTimeMinInFile(0),
m_MonsterDeadNum(0),m_endTimeMinInFile(0),m_bfirstStartAct(false),m_bstartTask(false),zhaoge(0),xiqi(0),m_MonsterDeadNumMax(0),
m_EasterDay(0),m_startTimeHour(0),m_endTimeHour(0),m_startTimeMin(0),m_endTimeMin(0),m_startSec(0),m_endSec(0),m_isRightStartMC(0)
{
}
CMonCastle::~CMonCastle()
{
	if (xiqi) DEL_ONE(xiqi);
	if (zhaoge) DEL_ONE(zhaoge);
}

void CMonCastle::_ChangeStringToInt(const std::string str,int *arr)
{
	if (!str.empty())
	{
		int count = 0;
		int start = 0;
		int end = str.find(",",start);
		while (std::string::npos != end)
		{
			arr[count] = atoi(str.substr(start,end - start).c_str());
			count++;
			start = end+1;
			end = str.find(",",start);
		}
	}
}

int CMonCastle::_GetIsNotZeroNumFromArr(int *arr,int length)
{
	int num = 0;
	for (int i = 0; i<length; ++i)
	{
		if (0 != arr[i])
		{
			num++;
		}
	}
	return num;
}

void CMonCastle::EraseMonsterFromList(CRegionCreature *cre)
{
	if (cre->m_core.Faction == FACTION_SHANG)
	{
		if (zhaoge)
		{
			std::list<CRegionCreature*>::iterator it = find(zhaoge->m_mcMonster.begin(),zhaoge->m_mcMonster.end(),cre);
			if (it != zhaoge->m_mcMonster.end())
			{
				zhaoge->m_mcMonster.erase(it);
			}
		}
	}
	else if (cre->m_core.Faction == FACTION_ZHOU)
	{
		if (xiqi)
		{
			std::list<CRegionCreature*>::iterator it = find(xiqi->m_mcMonster.begin(),xiqi->m_mcMonster.end(),cre);
			if (it != xiqi->m_mcMonster.end())
			{
				xiqi->m_mcMonster.erase(it);
			}
		}
	}
}

bool CMonCastle::LoadMCConfig(const char* szPKConfigFile)
{
	RtIni ini;
	if (!ini.OpenFile(szPKConfigFile))
	{
		RtCoreLog().Info("Error: Can't open MonCastle Config File [%s].\n", szPKConfigFile);
		return false;
	}
	std::string sTempDay = ini.GetEntry("Day", "EasterDay");
	if (!sTempDay.empty())
	{
		int start = 0;
		int end = sTempDay.find(",",start);
		int i = 0;
		while (end != -1)
		{
			i = atoi(sTempDay.substr(start,end - start).c_str());
			m_EasterDayInFile |= 1<<(i-1);
			start = end+1;
			end = sTempDay.find(",",start);
		}
	}
	m_startTimeHourInFile		= (int)ini["Time"]["StartTimeHour"];
	m_endTimeHourInFile			= (int)ini["Time"]["EndTimeHour"];
	m_MonsterDeadNumInFile	= (int)ini["MonsterDeadNum"]["MonsterDeadNum"];
	m_startTimeMinInFile	= (int)ini["Time"]["StartTimeMin"];
	m_endTimeMinInFile		= (int)ini["Time"]["EndTimeMin"];
	//m_Tick					= (int)ini["Tick"]["Tick"];
	xiqi = _GreateCountyAtrr(ini,0);
	zhaoge = _GreateCountyAtrr(ini,1);
	if (!xiqi || !zhaoge)
	{
		m_isRightStartMC = 0;
		return false;
	}
	else
	{
		m_isRightStartMC = 1;
		WriteConfig(m_EasterDayInFile,m_startTimeHourInFile,m_endTimeHourInFile,m_startTimeMinInFile,m_endTimeMinInFile,m_MonsterDeadNumInFile);
	}
	return true;
}

void CMonCastle::WriteConfig(byte easterDay, int startTimeHour, int endTimeHour, int startTimeMin, 
							 int endTimeMin, int monserDeadNum)
{
	m_EasterDay = easterDay;//活动日
	m_startTimeHour = startTimeHour;//活动开始时间
	m_endTimeHour = endTimeHour;//活动结束时间
	m_startTimeMin = startTimeMin;
	m_endTimeMin = endTimeMin;//活动结束时间
	m_MonsterDeadNumMax = monserDeadNum;//怪物死亡的数量

	m_startSec = m_startTimeHour * 3600 + m_startTimeMin * 60;//将时间转化为秒数
	m_endSec = m_endTimeHour * 3600 + m_endTimeMin * 60;
}

SCountyAtrr* CMonCastle::_GreateCountyAtrr(RtIni& ini,int byte)
{
	SCountyAtrr* country = 0;
	int smgroup = 0;
	std::string smid = "";
	std::string smnum = "";
	std::string smbossid = "";
	std::string smbossnum = "";
	std::string smposX = "";
	std::string smposY = "";
	std::string smpathpoint = "";
	if (0 == byte)
	{
		smgroup = (int)ini["XiqiSMGroup"]["XiqiSMGroup"];
		smid = (string)ini["XiqiSMId"]["XiqiSMId"];
		smnum = (string)ini["XiqiSMNum"]["XiqiSMNum"];
		smbossid = (string)ini["XiqiSMBossId"]["XiqiSMBossId"];
		smbossnum = (string)ini["XiqiSMBossNum"]["XiqiSMBossNum"];
		smposX = (string)ini["XiqiSMPos"]["XiqiSMPosX"];
		smposY = (string)ini["XiqiSMPos"]["XiqiSMPosY"];
		smpathpoint = (string)ini["XiqiPP"]["Xiqipathpoint"];
	}
	else if (1 == byte)
	{
		smgroup = (int)ini["ZhaogeSMGroup"]["ZhaogeSMGroup"];
		smid = (string)ini["ZhaogeSMId"]["ZhaogeSMId"];
		smnum = (string)ini["ZhaogeSMNum"]["ZhaogeSMNum"];
		smbossid = (string)ini["ZhaogeSMBossId"]["ZhaogeSMBossId"];
		smbossnum = (string)ini["ZhaogeSMBossNum"]["ZhaogeSMBossNum"];
		smposX = (string)ini["ZhaogeSMPos"]["ZhaogeSMPosX"];
		smposY = (string)ini["ZhaogeSMPos"]["ZhaogeSMPosY"];
		smpathpoint = (string)ini["ZhaogePP"]["Zhaogepathpoint"];
	}
	else
		return 0;
	country = RT_NEW SCountyAtrr();
	country->m_country = byte;
	country->m_mcMonster.clear();
	if (smgroup<=0 || smgroup>10)
	{
		return 0;
	}
	country->m_monsterMaxGroup = smgroup;
	_ChangeStringToInt(smid,country->m_spMonsterId);
	_ChangeStringToInt(smnum,country->m_spMonsterNum);
	_ChangeStringToInt(smbossid,country->m_spBossId);
	_ChangeStringToInt(smbossnum,country->m_spBossNum);
	_ChangeStringToInt(smposX,country->m_spPosX);
	_ChangeStringToInt(smposY,country->m_spPosY);
	country->m_spPathPoint = smpathpoint;
	if (_GetIsNotZeroNumFromArr(country->m_spMonsterId,sizeof(country->m_spMonsterId)/sizeof(int)) > smgroup
		|| _GetIsNotZeroNumFromArr(country->m_spMonsterNum,sizeof(country->m_spMonsterNum)/sizeof(int)) > smgroup
		|| _GetIsNotZeroNumFromArr(country->m_spBossId,sizeof(country->m_spBossId)/sizeof(int)) > smgroup
		|| _GetIsNotZeroNumFromArr(country->m_spBossNum,sizeof(country->m_spBossNum)/sizeof(int)) > smgroup
		|| country->m_spPathPoint.empty())
	{
		return 0;
	}
	return country;
}

void CMonCastle::ChectState()
{
	time_t tmp = time(NULL);
	tm now = *(localtime(&tmp));
	int dayOfWeek = 0;/* days since Monday - [1,7] */
	if (0 == now.tm_wday)/* days since Sunday - [0,6] */
		dayOfWeek = 7;
	else
		dayOfWeek = now.tm_wday;
	if (m_EasterDay & 1<<(dayOfWeek-1))
	{
		if (0 == now.tm_hour)
		{
			now.tm_hour = 24;
		}
		int nowSec = now.tm_hour * 3600 + now.tm_min * 60;
		if( nowSec >= m_startSec && nowSec < m_endSec)
		{
			if (m_state == MC_STATE_DONOTHING)
			{
				m_bfirstStartAct = true;
				m_state = MC_STATE_ACTSTART;
			}
			if (m_state == MC_STATE_TASKSTART)
			{
				if ( xiqi->m_delTargetHp - xiqi->m_targetCre->m_core.GetHp() >100 )
				{
					SystemBroad(6, xiqi->m_country, 0,xiqi->m_targetCre->m_core.GetHp());
					xiqi->m_delTargetHp = xiqi->m_targetCre->m_core.GetHp();
				}
				if ( zhaoge->m_delTargetHp - zhaoge->m_targetCre->m_core.GetHp() >100 )
				{
					SystemBroad(6, zhaoge->m_country, 0,zhaoge->m_targetCre->m_core.GetHp());
					zhaoge->m_delTargetHp = zhaoge->m_targetCre->m_core.GetHp();
				}
				if ( /*PZH*//*xiqi->m_targetCre->m_bDead*/!xiqi->m_targetCre->GetPlayerLogic()->IsLive() && !xiqi->m_mcTaskFailed)
				{
					xiqi->m_mcTaskFailed = true;
					SystemBroad(4,xiqi->m_country);
					GameOver(xiqi->m_country);
					xiqi->m_targetCre->m_core.SetHp(xiqi->m_targetCre->m_core.GetMaxHp());
				}
				if (/*PZH*//*zhaoge->m_targetCre->m_bDead*/!zhaoge->m_targetCre->GetPlayerLogic()->IsLive() && !zhaoge->m_mcTaskFailed)
				{
					zhaoge->m_mcTaskFailed = true;
					SystemBroad(4,zhaoge->m_country);
					GameOver(zhaoge->m_country);
					zhaoge->m_targetCre->m_core.SetHp(zhaoge->m_targetCre->m_core.GetMaxHp());
				}
				if(xiqi->m_mcMonster.size() == 0 && xiqi->m_monsterGroup == xiqi->m_monsterMaxGroup)
				{
					if (!xiqi->m_bfinish && !xiqi->m_mcTaskFailed)
					{
						FinshMCTask(xiqi->m_country);
						SystemBroad(3,xiqi->m_country);
						GameOver(xiqi->m_country);
						xiqi->m_targetCre->m_core.SetHp(xiqi->m_targetCre->m_core.GetMaxHp());
						xiqi->m_bfinish = true;
						xiqi->m_mcTaskFailed = false;
						//PZH
						//xiqi->m_targetCre->m_bDead = false;
						xiqi->m_targetCre->GetPlayerLogic()->ReLive();
						//

						//xiqi->m_targetCre->m_core.SetHp(xiqi->m_targetCre->m_core.GetMaxHp());
					}
				}
				if (zhaoge->m_mcMonster.size() == 0 && zhaoge->m_monsterGroup == zhaoge->m_monsterMaxGroup)
				{
					if (!zhaoge->m_bfinish && !zhaoge->m_mcTaskFailed)
					{
						FinshMCTask(zhaoge->m_country);
						SystemBroad(3,zhaoge->m_country);
						GameOver(zhaoge->m_country);
						zhaoge->m_targetCre->m_core.SetHp(zhaoge->m_targetCre->m_core.GetMaxHp());
						zhaoge->m_bfinish = true;
						zhaoge->m_mcTaskFailed = false;
						//PZH
						//zhaoge->m_targetCre->m_bDead = false;
						zhaoge->m_targetCre->GetPlayerLogic()->ReLive();
						//
						//zhaoge->m_targetCre->m_core.SetHp(zhaoge->m_targetCre->m_core.GetMaxHp());
					}
				}
			}
		}
		else if (nowSec >= m_endSec && m_state == MC_STATE_TASKSTART)
		{
			if(!zhaoge->m_bfinish)
				SystemBroad(4,zhaoge->m_country);
			if (!xiqi->m_bfinish)
				SystemBroad(4,xiqi->m_country);
			GameOver(zhaoge->m_country);
			GameOver(xiqi->m_country);
			m_state = MC_STATE_DONOTHING;
			m_bstartTask = false;
			zhaoge->m_bfinish = false;
			zhaoge->m_monsterGroup = 0;
			xiqi->m_monsterGroup = 0;
			xiqi->m_bfinish = false;
			WriteConfig(m_EasterDayInFile,m_startTimeHourInFile,m_endTimeHourInFile,m_startTimeMinInFile,m_endTimeMinInFile,m_MonsterDeadNumInFile);
		}
		else if (nowSec >= m_endSec && m_state == MC_STATE_ACTSTART)
		{
			m_state = MC_STATE_DONOTHING;
			SystemBroad(9,xiqi->m_country);
			SystemBroad(9,zhaoge->m_country);
			WriteConfig(m_EasterDayInFile,m_startTimeHourInFile,m_endTimeHourInFile,m_startTimeMinInFile,m_endTimeMinInFile,m_MonsterDeadNumInFile);
		}
	}
}

void CMonCastle::GameOver(byte country)//只是把所属国家的任务和怪物全部移除
{
	if (0 == country)
	{
		RemoveMCTaskAllUsers(0);
		RemoveAllMCMonster(0);
	}
	else if (1 == country)
	{
		RemoveMCTaskAllUsers(1);
		RemoveAllMCMonster(1);
	}
}
void CMonCastle::CalMonsterNum()
{
	if (m_state != MC_STATE_ACTSTART)
	{
		return;
	}
	if (m_MonsterDeadNum<0)
		return;
	--m_MonsterDeadNum;
	if (m_MonsterDeadNum > 50)
	{
		if (0 == m_MonsterDeadNum % 100)
		{
			SystemBroad(2,-1,m_MonsterDeadNum);
		}
	}
	else if(m_MonsterDeadNum > 0)
	{
		SystemBroad(2,-1,m_MonsterDeadNum);
	}
	else if (m_MonsterDeadNum == 0)
	{
		SystemBroad(2,-1,m_MonsterDeadNum);
		m_bstartTask = true;//开始MC任务
		m_state = MC_STATE_TASKSTART;
		m_MonsterDeadNum = -1;
		if (zhaoge)
		{
			zhaoge->m_targetCre = g_region->m_defScene->FindCreature(zhaoge->m_targetObjId);
			zhaoge->m_monsterGroup = 0;
			zhaoge->m_delTargetHp =  zhaoge->m_targetCre->m_core.GetHp();

			//PZH
			//zhaoge->m_targetCre->m_bDead = false;
			zhaoge->m_targetCre->GetPlayerLogic()->ReLive();
			//
		}
		if (xiqi)
		{
			xiqi->m_targetCre = g_region->m_defScene->FindCreature(xiqi->m_targetObjId);
			xiqi->m_monsterGroup = 0;
			xiqi->m_delTargetHp =  xiqi->m_targetCre->m_core.GetHp();
			//PZH
			//xiqi->m_targetCre->m_bDead = false;
			xiqi->m_targetCre->GetPlayerLogic()->ReLive();
			//
		}
	}
}

void CMonCastle::SystemBroad(int state,byte country/* =-1 */,int monsterDeadNum/* =0 */,int targetHp /* = 0 */, int monsterGroup /* = 0 */)
{
	CRegionUser *pUser;
	EXT_SPACE::hash_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;
		if (!pUser)
		{
			return;
		}
		switch (state)
		{
		case 1://1.怪物开始起义
			pUser->m_dummy->SendSystemMessage(R(MSG_MC_KILLMONSTER));
			break;
		case 2://2.怪物到计数
			char buf[30];
			memset(buf,0,sizeof(buf));
			rt2_sprintf(buf,"怪物还剩余%d只",monsterDeadNum);
			pUser->m_dummy->SendSystemMessage(buf);
			break;
		case 3://任务成功
			if (pUser->m_dummy->m_core.Faction == country)
			{
				pUser->m_dummy->SendSystemMessage(R(MSG_MC_TASKSUCCEED));
			}
			break;
		case 4://任务失败
			if (pUser->m_dummy->m_core.Faction == country)
			{
				pUser->m_dummy->SendSystemMessage(R(MSG_MC_TASKFAILED));
			}
			break;
		case 5://任务开始
			if (!pUser->m_dummy->m_isAddMCTask)
			{
				pUser->m_dummy->SendSystemMessage(R(MSG_MC_TASKSTART));
				pUser->m_dummy->SendSystemMessage(R(MSG_MC_TASKSTARTADDITEM));
			}
			break;
		case 6://雕像的剩余HP
			if (pUser->m_dummy->m_core.Faction == country)
			{
				char buf[50];
				memset(buf,0,sizeof(buf));
				if( 0 == country)
				{
					rt2_sprintf(buf,"周凤雕像还剩余 %d HP",targetHp);
				}
				else if ( 1 == country)
				{
					rt2_sprintf(buf,"商龙雕像还剩余 %d HP",targetHp);
				}
				pUser->m_dummy->SendSystemMessage(buf);
			}
			break;
		case 7://第几组怪物的冲击
			if (pUser->m_dummy->m_core.Faction == country)
			{
				char buf[50];
				memset(buf,0,sizeof(buf));
				rt2_sprintf(buf,"第 %d 组怪物冲过来了，英雄们，勇敢的冲上去吧！",monsterGroup);
				pUser->m_dummy->SendSystemMessage(buf);
			}
			break;
		case 8://最后一组怪物的冲击
			if (pUser->m_dummy->m_core.Faction == country)
			{
				const char* buf = "最后一组怪物了，英雄们，坚持就是胜利!";
				pUser->m_dummy->SendSystemMessage(buf);
			}
			break;
		case 9://时间到，杀死怪物数量不足
			if (pUser->m_dummy->m_core.Faction == country)
			{
				const char* buf = "时间到，杀死怪物数量不足，没有引起怪物攻城！";
				pUser->m_dummy->SendSystemMessage(buf);
			}
			break;
		default:
			break;
		}
	}
}

void CMonCastle::AddMCTask()
{
	CRegionUser *pUser;
	EXT_SPACE::hash_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;
		if (pUser && !pUser->m_dummy->m_isAddMCTask)
		{
			if (pUser->m_dummy->m_core.Faction == FACTION_ZHOU && !xiqi->m_mcTaskFailed && xiqi->m_bfinish != true)
			{
				pUser->m_dummy->AcceptTask(9017,0);
				SystemBroad(5);
				pUser->m_dummy->m_isAddMCTask = true;
			}
			else if (pUser->m_dummy->m_core.Faction == FACTION_SHANG && !zhaoge->m_mcTaskFailed && zhaoge->m_bfinish != true)
			{
				pUser->m_dummy->AcceptTask(9117,0);
				SystemBroad(5);
				pUser->m_dummy->m_isAddMCTask = true;
			}
		}
	}
}
//活动时间到，任务失败,任务成功移除怪物攻城的任务
void CMonCastle::RemoveMCTaskAllUsers(byte country)
{
	CRegionUser *pUser;
	EXT_SPACE::hash_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;
		if(pUser && pUser->m_dummy->m_core.Faction == country)
		{
			pUser->m_dummy->RemoveMCTask();
			pUser->m_dummy->m_isAddMCTask = false;
		}
	}
}
void CMonCastle::RemoveAllMCMonster(byte country)
{
	if (0 == country)
	{
		if (xiqi && 0 != xiqi->m_mcMonster.size())
		{
			std::list<CRegionCreature*>::iterator it = xiqi->m_mcMonster.begin();
			for (;it!=xiqi->m_mcMonster.end();++it)
			{
				(*it)->Quit();
			}
			xiqi->m_mcMonster.clear();
		}
	}
	else if (1 == country)
	{
		if (zhaoge && 0 != zhaoge->m_mcMonster.size())
		{
			std::list<CRegionCreature*>::iterator it = zhaoge->m_mcMonster.begin();
			for (;it != zhaoge->m_mcMonster.end();++it)
			{
				(*it)->Quit();
			}
			zhaoge->m_mcMonster.clear();
		}
	}
}
void CMonCastle::MCTaskSpawnMonster(SCountyAtrr* county)
{
	if (!county)
	{
		return;
	}
	int monsterId = 0;
	int monstercount = 0;
	int bossId  = 0;
	int bossCount  = 0;
	if (county->m_monsterGroup <= county->m_monsterMaxGroup )
	{
		monsterId = county->m_spMonsterId[county->m_monsterGroup-1];
		monstercount = county->m_spMonsterNum[county->m_monsterGroup-1];
		bossId = county->m_spBossId[county->m_monsterGroup-1];
		bossCount = county->m_spBossNum[county->m_monsterGroup-1];
		if (county->m_monsterGroup < county->m_monsterMaxGroup)
		{
			SystemBroad(7,county->m_country,0,0,county->m_monsterGroup);//提示玩家是第几组怪物的冲击
		}
		else
		{
			SystemBroad(8,county->m_country,0,0,0);//提示玩家是最后一组怪物的冲击
		}
		MCTaskRealSpawnMonster(county,monsterId,monstercount,bossId,bossCount);
	}
}
void CMonCastle::MCTaskRealSpawnMonster(SCountyAtrr* county,int monsterId,int monstercount,int bossId,int bossCount)
{
	if (!county)
	{
		return;
	}
	int maxX = MAX(county->m_spPosX[0],county->m_spPosX[1]);
	int minX = MIN(county->m_spPosX[0],county->m_spPosX[1]);
	int maxY = MAX(county->m_spPosY[0],county->m_spPosY[1]);
	int minY = MIN(county->m_spPosY[0],county->m_spPosY[1]);
	for(int i=0;i<monstercount;++i)
	{
		CRegionCreature *monster = g_region->CreateNpc(monsterId);
		if(!monster)
		{
			return;
		}
		char dir = rand() % 255;
		long pos[3];
		pos[0] = rtRandom(maxX,minX) * 20;
		pos[1] = rtRandom(maxY,minY) * 20;
		pos[2] = 0;		
		monster->m_core.Faction = county->m_country;
		if(!g_region->m_defScene->AddCreature(monster, pos, dir))
		{		
			g_factory->DestroyObject(monster);
			return;
		}

		if(monster->IsScriptLoaded())
			monster->CallScript("OnCreate", false);
		county->m_mcMonster.push_back(monster);		
	}
	for(int i=0;i<bossCount;++i)
	{
		CRegionCreature *boss = g_region->CreateNpc(bossId);
		if(!boss)
		{
			return;
		}
		char dir = rand() % 255;
		long pos[3];
		pos[0] = rtRandom(maxX,minX) * 20;
		pos[1] = rtRandom(maxY,minY) * 20;
		pos[2] = 0;
		boss->m_core.Faction = county->m_country;
		if(!g_region->m_defScene->AddCreature(boss, pos, dir))
		{		
			g_factory->DestroyObject(boss);
			return;
		}
		if(boss->IsScriptLoaded())
			boss->CallScript("OnCreate", false);
		county->m_mcMonster.push_back(boss);			
	}
}

//任务完成
void CMonCastle::FinshMCTask(byte country)
{
	CRegionUser *pUser;
	EXT_SPACE::hash_map<ULONG,CRegionUser*>::iterator iter = g_region->m_userMap.begin();
	for (; iter!=g_region->m_userMap.end(); iter++)
	{
		pUser = (*iter).second;
		if(pUser && pUser->m_dummy->m_core.Faction == country)
		{
			pUser->m_dummy->FinishMCTask();
		}
	}
}

void CMonCastle::RestoreTargetObjId(ULONG oId,int NpcId)
{
	CRegionCreature* country = 0;
	if (722 == NpcId && zhaoge)
	{
		zhaoge->m_targetObjId = oId;
	}
	else if (723 == NpcId && xiqi)
	{
		xiqi->m_targetObjId = oId;
	}
}

void CMonCastle::Run()
{
	if (g_RegionCastleWarMgr.isStartWar)
	{
		return;
	}
	ChectState();
	switch (m_state)
	{
	case MC_STATE_DONOTHING:
		break;
	case MC_STATE_ACTSTART:
		if (m_bfirstStartAct)
		{		
			m_MonsterDeadNum = m_MonsterDeadNumMax;
			SystemBroad(1);
			m_bfirstStartAct = false;
		}
		break;
	case MC_STATE_TASKSTART:
		AddMCTask();//任务开始给所有玩家都自动加上怪物攻城的任务
		if (zhaoge && 0 == zhaoge->m_mcMonster.size() && !zhaoge->m_mcTaskFailed)
		{
			if (zhaoge->m_monsterGroup > zhaoge->m_monsterMaxGroup)
			{
				zhaoge->m_monsterGroup = zhaoge->m_monsterMaxGroup +1;
			}
			else
				zhaoge->m_monsterGroup++;
			if (zhaoge->m_monsterGroup <= zhaoge->m_monsterMaxGroup)
			{	

				MCTaskSpawnMonster(zhaoge);
			}
		}
		if (xiqi && 0 == xiqi->m_mcMonster.size() && !xiqi->m_mcTaskFailed)
		{
			if (xiqi->m_monsterGroup > xiqi->m_monsterMaxGroup)
			{
				xiqi->m_monsterGroup = xiqi->m_monsterMaxGroup +1;
			}
			else
				xiqi->m_monsterGroup++;
			if (xiqi->m_monsterGroup <= xiqi->m_monsterMaxGroup)
			{	
				MCTaskSpawnMonster(xiqi);
			}
		}
		break;
	default:
		break;
	}
}
