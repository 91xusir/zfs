#include "rs_war_zhulu.h"

const int ZHULU_TIMER_TO_END		= OBJECT_C_TIMER_BASE + 1;
const int ZHULU_TIMER_UPDATE		= OBJECT_C_TIMER_BASE + 2;

const int ZHULU_REWARD_WIN			= 1311;
const int ZHULU_REWARD_LOST			= 1312;
const int ZHULU_REWARD_HIGHEST		= 1103;

CWarZhulu::CWarZhulu()
{
	m_bFixZhuluActive = false;
	m_bTmpZhuluActive = false;
	m_ZhuluBegTime = 0;
	m_ZhuluInterval = 0;
	m_ZhuluWinFaction = 2;
	m_ZhuluMap.push_back("scene01_118.rtb");
	m_ZhuluActiveUser.clear();
}

CWarZhulu::~CWarZhulu() {};

void CWarZhulu::OnTimer(ULONG id, DWORD dwParam[])
{
	//std::list<ULONG>::iterator it;
	EXT_SPACE::hash_map<ULONG,SZhuluUserInfo>::iterator it;
	switch(id)
	{
	case ZHULU_TIMER_TO_END:
	{
		CRegionUser *pUser;
		bool kick = false;
		int cnt = 0;
		for(it=m_ZhuluActiveUser.begin();it!=m_ZhuluActiveUser.end();++it)
		{
			//ULONG userId = *it;
			ULONG userId = it->first;
			if(g_region->m_userMap.find(userId) != g_region->m_userMap.end())
			{
				pUser = g_region->m_userMap[userId];
				if(IsInZhuluMap(pUser))
				{
					KickZhuluUser(pUser);
					kick = true;
					if(cnt++ >= 30) break;
				}
			}
		}
		if(!kick)
		{
			m_ZhuluActiveUser.clear();
			StopTimer(ZHULU_TIMER_TO_END);
		}
		break;
	}
	case ZHULU_TIMER_UPDATE:
	{
		if(IsZhuluActive(true))
		{
			if(!IsZhuluTime(true)) EndZhulu();
		}
		else
		{
			if(IsZhuluTime(true)) StartZhulu(true);
		}
		break;
	}
	}
}

bool CWarZhulu::IsZhuluActive(bool fix)
{
	if(fix)
		return m_bFixZhuluActive;
	else
		return m_bTmpZhuluActive;
}

void CWarZhulu::StartZhulu(bool fix)
{
	if(fix)
	{
		m_bFixZhuluActive = true;
	}
	else
	{
		m_bTmpZhuluActive = true;
		m_ZhuluBegTime = time(NULL); 
		m_ZhuluInterval = 2*3600;
	}

	m_ZhuluActiveUser.clear();
	m_ZhuluWinFaction = 2;
}

void CWarZhulu::OnEnterZhulu(CRegionUser *user)
{
	//std::list<ULONG>::iterator it = find(m_ZhuluActiveUser.begin(),m_ZhuluActiveUser.end(),user->m_userId);
	//EXT_SPACE::hash_map<ULONG,SZhuluUserInfo>::iterator it = 
	if(m_ZhuluActiveUser.find(user->m_userId) == m_ZhuluActiveUser.end())
	{
		SZhuluUserInfo sUserInfo(user->m_userId);
		m_ZhuluActiveUser[user->m_userId] = sUserInfo;
	}
}

void CWarZhulu::OnZhuluWin(char faction)
{
	if(m_ZhuluWinFaction != 2)
		return;
	m_ZhuluWinFaction = faction;

	CRegionUser *pUser;
	//std::list<ULONG>::iterator it = m_ZhuluActiveUser.begin();
	EXT_SPACE::hash_map<ULONG,SZhuluUserInfo>::iterator it = m_ZhuluActiveUser.begin();

	for(; it!=m_ZhuluActiveUser.end(); ++it)
	{

		ULONG id = it->first;
		SZhuluUserInfo& sUserInfo = it->second;
		
		if(sUserInfo.killNum < 5) //杀人数少于5个的直接干掉
			continue;
		
		if(g_region->m_userMap.find(id) != g_region->m_userMap.end())
		{
			pUser = g_region->m_userMap[id];
			if(pUser->m_dummy->m_core.Faction == faction)
			{
				SItemID item = g_region->m_pItemManager->CreateItem(ZHULU_REWARD_WIN); //定义常量
				if(ItemID_IsValid(item))
				{
					pUser->m_dummy->AddItem(item);
					pUser->m_dummy->SendSystemMessage("恭喜您获得涿鹿的奖励！"); //需定义话语，需变成GM公告
				}
			}
			else
			{
				SItemID item = g_region->m_pItemManager->CreateItem(ZHULU_REWARD_LOST); //定义常量
				if(ItemID_IsValid(item))
				{
					pUser->m_dummy->AddItem(item);
					pUser->m_dummy->SendSystemMessage("请再接再厉，争取下一次的胜利！"); //需定义话语，需变成GM公告
				}
			}
		}
	}
}

void CWarZhulu::EndZhulu()
{
	m_bFixZhuluActive = false;
	m_bTmpZhuluActive = false;
	SetTimer(ZHULU_TIMER_TO_END, 10*1000); 
	
	//取出杀人数最多的人并给予奖励
	EXT_SPACE::hash_map<ULONG,SZhuluUserInfo>::iterator it = m_ZhuluActiveUser.begin();
	if (it != m_ZhuluActiveUser.end())
	{
		int iMaxNum = it->second.killNum;
		ULONG maxId = it->first;
		CRegionUser *pUser;
		
		for(;it!=m_ZhuluActiveUser.end();++it)
		{
			if(it->second.killNum > iMaxNum)
			{
				iMaxNum = it->second.killNum;
				maxId = it->first;
			}
		}

		if(g_region->m_userMap.find(maxId) != g_region->m_userMap.end())
		{
			pUser = g_region->m_userMap[maxId];
			SItemID item = g_region->m_pItemManager->CreateItem(ZHULU_REWARD_HIGHEST); //定义常量
			if(ItemID_IsValid(item))
			{
				pUser->m_dummy->AddItem(item);
				pUser->m_dummy->SendSystemMessage("您杀敌数最高，获得奖励！"); //需定义话语，需变成GM公告
			}
			ERR("Best Killer is %s(%d)\n",pUser->m_dummy->m_core.Name.c_str(),iMaxNum);
		}
	}
}

bool CWarZhulu::IsInZhuluMap(CRegionUser *user)
{
	std::string map;
	RtsSceneBlockMap* bmap = g_region->m_defScene->FindBlockByPos(user->m_dummy->m_pos);
	if(bmap)
	{
		map = bmap->m_szFileName;
	}else
	{
		map = "scene01_001.rtb";
	}

	for(int i=0; i<(int)m_ZhuluMap.size(); i++)
	{
		if(map == m_ZhuluMap[i]) return true;
	}
	return false;
}

void CWarZhulu::KickZhuluUser(CRegionUser *user)
{
	int x,y;
	if(user->m_dummy->m_core.Faction == FACTION_ZHOU)
	{
		if(GetProb(0.3))
		{
			x = 3090;
			y = 2690;
		}
		else if(GetProb(0.3))
		{
			x = 4740;
			y = -3165;
		}
		else
		{
			x = 6080;
			y = -2540;
		}
	}
	else
	{
		if(GetProb(0.3))
		{
			x = 48648;
			y = -16648;
		}
		else if(GetProb(0.3))
		{
			x = 53945;
			y = -18640;
		}
		else
		{
			x = 49425;
			y = -13980;
		}
	}
	user->m_dummy->ForceSetPosition(x,y,0);
}

bool CWarZhulu::IsZhuluTime(bool fix)
{
	if(fix)
	{
		time_t tNow = time(NULL);
		struct tm* pTm = localtime(&tNow);
		if((pTm->tm_wday==2 || pTm->tm_wday == 4) && (pTm->tm_hour>=19 && pTm->tm_hour<21))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

void CWarZhulu::StartUp()
{
	SetTimer(ZHULU_TIMER_UPDATE,5*1000);
}

void CWarZhulu::OnZhuluPKOther(CRegionUser *user)
{
	ULONG id = user->m_userId;
	if(m_ZhuluActiveUser.find(id) != m_ZhuluActiveUser.end())
		m_ZhuluActiveUser[id].killNum++;
}
