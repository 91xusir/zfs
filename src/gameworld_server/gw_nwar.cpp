
#include "gw_server.h"
#include "gw_database.h"
#include "gw_nwar.h"
#include <functional>

#define EMAX(lev)	((lev)/10 * 5)

//static const char*	s_szUserLimit = "玩家数量已达到上限";
//static const char*	s_szWarfieldStop = "战场还未开启，排队人数已满";
//static const char*	s_szWarfieldApp = "战场还未开启，您已加入排队队列";
//static const char*	s_szWarfieldIn = "已经在战场内";
#define s_szUserLimit		R(MSG_NATIONAL_WAR_USER_LIMIT)
#define	s_szWarfieldStop	R(MSG_NATIONAL_WAR_FIELD_NOT_START)
#define	s_szWarfieldApp		R(MSG_NATIONAL_WAR_ADD_LIST)
#define	s_szWarfieldIn		R(MSG_NATIONAL_WAR_IN_FIELD)

const int iExploit[3] = {2, 5, 30};

CGwNationalWarMgr::CGwNationalWarMgr()
{
	m_bCanStart = false;
	m_bHad		= false;
	m_bFlagDestroyedShang	= false;
	m_bFlagDestroyedZhou	= false;
	m_Winner	= FACTION_UNKNOWN;
}

CGwNationalWarMgr::~CGwNationalWarMgr()
{
}

bool CGwNationalWarMgr::Init()
{	
	if (Load(R(RES_WARFIELD)))
	{
		GetStartTime(m_stWarField.tStartTime);
		m_stWarField.tEndTime = m_stWarField.tStartTime + m_stWarField.iDuration * 60 * 60;
		return true;
	}
	return false;
}

void CGwNationalWarMgr::AddHeroItem(char faction)
{
	if(faction != FACTION_SHANG && faction != FACTION_ZHOU) return;

	TLSExploit	lsTmp;

	TLSExploit::iterator it = m_stWarField.listExploit.begin();
	for(; it != m_stWarField.listExploit.end(); ++it)
	{
		if(faction == it->faction)
		{
			lsTmp.push_back((*it));
		}
	}

	lsTmp.sort(std::greater<SExploit>());

	int cnt = 3;
	long prev = 0;
	it = lsTmp.begin();
	for(; (it!=lsTmp.end() && cnt>0); it++)
	{
		if(it == lsTmp.begin())
		{
			prev = it->lExploit;
			AddHeroItem(faction, it->dwUnionID, cnt);
			continue;
		}

		if(prev == it->lExploit)
		{
			AddHeroItem(faction, it->dwUnionID, cnt);
		}else if(prev > it->lExploit)
		{
			cnt--;
			if(cnt<=0) return;
			AddHeroItem(faction, it->dwUnionID, cnt);
			prev = it->lExploit;
		}else
		{
			LOG("Warfield Reward: Sort Error\n");
		}
	}
}
void CGwNationalWarMgr::AddHeroItem(char faction, DWORD dwUnionID, int lev)
{
	if(lev<=0 || lev>3) return;
	if(faction != FACTION_SHANG && faction != FACTION_ZHOU) return;
	CUnion* pUnion = g_UnionMgr.FindUnion(dwUnionID);
	if(!pUnion)
	{
		LOG1("Warfield: Cannot find union [%d]\n", dwUnionID);
		return;
	}

	const int iIdx[2][3] =
	{
		{1102, 1103, 1104},
		{1105 ,1106 ,1107}
	};

	GetStartTime(m_stWarField.tStartTime);
	m_stWarField.tEndTime = m_stWarField.tStartTime + m_stWarField.iDuration * 60 * 60;

	DWORD userID = pUnion->GetCaption();

	std::vector<SCreateItem> arrCi;
	SCreateItem ci;
	ci.index = iIdx[faction][lev-1];
	ci.level = 0;
	ci.cBind = IB_Binded;
	ci.color = ItemColor_Golden;
	arrCi.push_back(ci);
	std::string strItems;
	g_ItemMgr.GenerateCreateString(arrCi, strItems);
	g_GiftMgr.AddGift(userID, 0, GT_Items, (char*)strItems.c_str(), 0, m_stWarField.tStartTime, R(MSG_NATIONAL_WAR_HORTATION));
	g_server.LogUserEvent(PLT_WarfieldToke, userID, "", dwUnionID, "", lev, "", 0, "");
}

bool CGwNationalWarMgr::Stop()
{
	LOG("Warfield Wait Stopping......\n");
	LOG2("Warfield Compare: [zhou][%d], [shang][%d]\n", (int)m_stWarField.listUserZhou.size(), (int)m_stWarField.listUserShang.size());

	if(m_bFlagDestroyedZhou) m_Winner = FACTION_SHANG;
	else if(m_bFlagDestroyedShang) m_Winner = FACTION_ZHOU;
	else if(m_stWarField.lScoreShang > m_stWarField.lScoreZhou) m_Winner = FACTION_SHANG;
	else if(m_stWarField.lScoreShang < m_stWarField.lScoreZhou) m_Winner = FACTION_ZHOU;
	else m_Winner = FACTION_UNKNOWN;

	std::string strWinner = "";
	if(m_Winner == FACTION_ZHOU) strWinner = "Zhou";
	else if(m_Winner == FACTION_SHANG) strWinner = "Shang";
	else if(m_Winner == FACTION_UNKNOWN) strWinner = "None";
	LOG1("Warfield Winner is [%s]\n", (char*)strWinner.c_str());
	g_server.LogUserEvent(PLT_WarfieldResult, 0, "", 0, (char*)strWinner.c_str(), 0, "", 0, "");

	TLSUser		lsTmp;
	if(m_Winner == 0) lsTmp = m_stWarField.listUserZhou;
	else if(m_Winner ==1) lsTmp = m_stWarField.listUserShang;
	else lsTmp.clear();

	stlforeach(TLSUser, it, lsTmp)
	{
		Log(it->dwUserID);
	}

	//添加封神令牌
	AddHeroItem(m_Winner);

	m_stWarField.eState = CGwNationalWarMgr::eWaitStopping;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_stop);
	g_server.m_region.SendToPeer(g_server.GetRegionSession(m_stWarField.lRsidx), &cmd);

	return true;
}

int	CGwNationalWarMgr::GetUserNum(int faction)
{
	if(m_stWarField.eState == eStarting)
	{
		if(faction == FACTION_ZHOU)
		{
			return (int)m_stWarField.listUserZhou.size();
		}else
		{
			return (int)m_stWarField.listUserShang.size();
		}
	}else
	{
		return 0;
	}
}

bool CGwNationalWarMgr::Log(DWORD dwUserID)
{
	CGwNationalWarMgr::SUser* pWUser = FindUser(dwUserID);
	if(!pWUser)
	{
		LOG1("Warfield [Log]: User [%d] did not join in warfield\n",dwUserID);
		return false;
	}

	long lExp = 0;
	DWORD dwLev = pWUser->dwLevel;

	if(dwLev >= 20 && dwLev < 40)
	{
		lExp = WF_EXP_20_40(dwLev);
	}else if(dwLev >= 40 && dwLev < 60)
	{
		lExp = WF_EXP_40_60;
	}else if(dwLev >= 60 && dwLev < 80)
	{
		lExp = WF_EXP_60_80;
	}else if(dwLev >= 80 && dwLev < 100)
	{
		lExp = WF_EXP_80_100;
	}

	if(!g_server.LogUserEvent(PLT_Warfield, dwUserID, "", lExp, "", 0, "", 0, ""))
	{
		LOG("Log Warfield Info Error\n");
		return false;
	}
	return true;
}

void CGwNationalWarMgr::Run()
{
#ifndef _WARFIELD_
	return;
#endif
	time_t now = time(NULL);
	static bool bSendApp = false;
	static bool bSendAll = false;

	struct tm*	pTime = localtime(&now);

	//发送倒计时消息，只发送一次
	if(m_stWarField.eState == CGwNationalWarMgr::eStop && !bSendApp)
	{
		if( (now+300 >= m_stWarField.tStartTime) && !m_bHad )
		{
			BroadcastWarfieldMsg(WF_APP, msg_warfield_prepare);
			bSendApp = true;
		}
	}

	if(m_stWarField.eState == CGwNationalWarMgr::eStarting && !bSendAll)
	{
		if( now+300 >= m_stWarField.tEndTime )
		{
			BroadcastWarfieldMsg(WF_ALL, msg_warfield_will_stop);
			bSendAll = true;
		}
	}

	if(now >= m_stWarField.tEndTime)
	{
		//如果到时间关闭了，看看战场是否正在开放
		if(m_stWarField.eState == CGwNationalWarMgr::eStarting)
		{
			Stop();
			m_stWarField.eState = CGwNationalWarMgr::eWaitStopping;
		}
	}
	else if( (now >= m_stWarField.tStartTime) && !m_bHad && m_stWarField.eState != CGwNationalWarMgr::eWaitStopping)
	{
		if(m_stWarField.eState == CGwNationalWarMgr::eStop)
		{
			//如果战场开启时间到了，看看战场是否关闭状态
			if(!CanStart())
			{
				LOG("Region Disconnected, Cannot Start WarField\n");
				return;
			}

			if(Start())
			{
				m_stWarField.eState = CGwNationalWarMgr::eWaitStarting;
			}
		}else if(m_stWarField.eState == CGwNationalWarMgr::eStarting)
		{
			//如果战场已经开放，看看还有没有在排队的加进去
			AddAppUserToWarfield();

			static time_t tUpdate = 0;
			if(now - tUpdate > 60)
			{
				CalcRate();
				UpdateRateToRs();
				tUpdate = now;
			}
		}
	}

	if(m_bHad && !(now >= m_stWarField.tStartTime && now <= m_stWarField.tEndTime))
	{
		m_bHad = false;
		bSendApp = false;
		bSendAll = false;
		m_stWarField.eState = CGwNationalWarMgr::eStop;
		GetStartTime(m_stWarField.tStartTime);
		m_stWarField.tEndTime = m_stWarField.tStartTime + m_stWarField.iDuration * 60 * 60;
		m_bFlagDestroyedZhou = false;
		m_bFlagDestroyedShang = false;
	}

	//如果战场是关闭的，看看排队的时间，排队时间长的踢出去
	if(m_stWarField.eState == CGwNationalWarMgr::eStop)
	{
		for(TLSApp::iterator it = m_stWarField.listAppZhou.begin(); it != m_stWarField.listAppZhou.end();)
		{
			if(it->tStartTime + 30 * 60 < now)
			{
				it = m_stWarField.listAppZhou.erase(it);
				SendWarfieldMsg(it->dwUserID, msg_warfield_kick_timeout);
			}else
			{
				++it;
			}
		}

		for(TLSApp::iterator it = m_stWarField.listAppShang.begin(); it != m_stWarField.listAppShang.end();)
		{
			if(it->tStartTime + 30 * 60 < now)
			{
				it = m_stWarField.listAppShang.erase(it);
				SendWarfieldMsg(it->dwUserID, msg_warfield_kick_timeout);
			}else
			{
				++it;
			}
		}
	}

	//掉线超过5分钟的剔出战场
	if(m_stWarField.eState == CGwNationalWarMgr::eStarting)
	{
		for(TLSUser::iterator it = m_stWarField.listUserZhou.begin(); it != m_stWarField.listUserZhou.end();)
		{
			if(it->dwTimeout == 0)
			{
				++it;
				continue;
			}

			if(it->dwTimeout > 5 * 60)
			{
				LOG1("Warfield: user [%d][zhou] is kicked out because of timeout\n", it->dwUserID);
				it = m_stWarField.listUserZhou.erase(it);				
			}else
			{
				it->dwTimeout += 5;
				++it;				
			}
		}

		for(TLSUser::iterator it = m_stWarField.listUserShang.begin(); it != m_stWarField.listUserShang.end();)
		{
			if(it->dwTimeout == 0) 
			{
				++it;
				continue;
			}

			if(it->dwTimeout > 5 * 60)
			{
				LOG1("Warfield: user [%d][shang] is kicked out because of timeout\n", it->dwUserID);
				it = m_stWarField.listUserShang.erase(it);				
			}else
			{
				it->dwTimeout += 5;
				++it;
			}
		}
	}
}

void CGwNationalWarMgr::AddAppUserToWarfield()
{
	//如果战场已经开放，看看还有没有在排队的加进去
	if(!m_stWarField.listAppZhou.empty())
	{
		if((int)m_stWarField.listAppZhou.size() < WF_NUM_LIMIT)
		{
			int cnt_total = WF_NUM_LIMIT - (int)m_stWarField.listUserZhou.size();
			int cnt_join  = 0;
			TLSApp::iterator it = m_stWarField.listAppZhou.begin();
			for(; it != m_stWarField.listAppZhou.end() && (cnt_total > 0) && (cnt_join < WF_JOIN_NUM); cnt_total--, cnt_join++)
			{
				EnterWarfield(it->dwUserID);
				it = m_stWarField.listAppZhou.erase(it);
			}
		}
	}

	if(!m_stWarField.listAppShang.empty())
	{
		if((int)m_stWarField.listUserShang.size() < WF_NUM_LIMIT)
		{
			int cnt_total = WF_NUM_LIMIT - (int)m_stWarField.listUserShang.size();
			int cnt_join  = 0;
			TLSApp::iterator it = m_stWarField.listAppShang.begin();
			for(; it != m_stWarField.listAppShang.end() && (cnt_total > 0) && (cnt_join < WF_JOIN_NUM); cnt_total--)
			{
				EnterWarfield(it->dwUserID);
				it = m_stWarField.listAppShang.erase(it);
			}
		}
	}
}

void CGwNationalWarMgr::CalcRate()
{
	long lEffectZhou	= 1;
	long lEffectShang	= 1;

	m_stWarField.fRateZhou = 1.0;

	stlforeach(TLSUser, it1, m_stWarField.listUserZhou)
	{
		lEffectZhou += it1->dwLevel;
	}

	stlforeach(TLSUser, it2, m_stWarField.listUserShang)
	{
		lEffectShang += it2->dwLevel;
	}

	float f = m_stWarField.fRateZhou * lEffectZhou/lEffectShang;
	if(f > 2.0) f = 2.0;
	if(f < 0.5) f = 0.5;

	m_stWarField.fRateShang = f;

	LOG2("Warfield CalcRate: [zhou][%.1f], [shang][%.1f]\n", m_stWarField.fRateZhou, m_stWarField.fRateShang);
}

void CGwNationalWarMgr::UpdateRateToRs()
{
	int session = g_server.GetRegionSession(m_stWarField.lRsidx);
	if(session == -1) return;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_update_rate);
	cmd.WriteFloat(m_stWarField.fRateZhou);
	cmd.WriteFloat(m_stWarField.fRateShang);
	g_server.m_region.SendToPeer(session);
}

void CGwNationalWarMgr::SendWarfieldMsg(long dwUserID, int iMsgID)
{
	GWLoginClient* gc = g_server.FindLoginClientByActorId(dwUserID);

	if(!gc) return;

	int session = g_server.GetRegionSession(gc->m_regionId);

	if(session == -1) return;

	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_msg);

	cmd.WriteLong(dwUserID);
	cmd.WriteLong(iMsgID);

	g_server.m_region.SendToPeer(session);
}

void CGwNationalWarMgr::BroadcastWarfieldMsg(int toWho, int iMsgID)
{
	if(toWho == WF_APP || toWho == WF_ALL)
	{	
		TLSApp::iterator it = m_stWarField.listAppZhou.begin();

		for(; it != m_stWarField.listAppZhou.end(); ++it)
		{
			GWLoginClient* gc = g_server.FindLoginClientByActorId(it->dwUserID);

			if(!gc) return;

			int session = g_server.GetRegionSession(gc->m_regionId);

			if(session == -1) continue;

			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_msg);

			cmd.WriteLong(it->dwUserID);
			cmd.WriteLong(iMsgID);

			g_server.m_region.SendToPeer(session);
		}

		it = m_stWarField.listAppShang.begin();

		for(; it != m_stWarField.listAppShang.end(); ++it)
		{
			GWLoginClient* gc = g_server.FindLoginClientByActorId(it->dwUserID);

			if(!gc) return;

			int session = g_server.GetRegionSession(gc->m_regionId);

			if(session == -1) continue;

			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_msg);

			cmd.WriteLong(it->dwUserID);
			cmd.WriteLong(iMsgID);

			g_server.m_region.SendToPeer(session);
		}
	}

	if(toWho == WF_IN || toWho == WF_ALL)
	{	
		TLSUser::iterator it = m_stWarField.listUserZhou.begin();

		for(; it != m_stWarField.listUserZhou.end(); ++it)
		{
			//GWLoginClient* gc = g_server.FindLoginClientByActorId(it->dwUserID);

			//if(!gc) return;

			int session = g_server.GetRegionSession(m_stWarField.lRsidx);

			if(session == -1) continue;

			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_msg);

			cmd.WriteLong(it->dwUserID);
			cmd.WriteLong(iMsgID);

			g_server.m_region.SendToPeer(session);
		}

		it = m_stWarField.listUserShang.begin();

		for(; it != m_stWarField.listUserShang.end(); ++it)
		{
			//GWLoginClient* gc = g_server.FindLoginClientByActorId(it->dwUserID);

			//if(!gc) return;

			int session = g_server.GetRegionSession(m_stWarField.lRsidx);

			if(session == -1) continue;

			CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_msg);

			cmd.WriteLong(it->dwUserID);
			cmd.WriteLong(iMsgID);

			g_server.m_region.SendToPeer(session);
		}
	}
}

void CGwNationalWarMgr::BroadcastSystemMsg(const char* s_szMsg)
{
	TLSUser::iterator it = m_stWarField.listUserZhou.begin();

	for(; it != m_stWarField.listUserZhou.end(); ++it)
	{
		GWLoginClient* gc = g_server.FindLoginClientByActorId(it->dwUserID);

		if(gc)
		{
			g_server.m_chatTransmittal.Chat(g_server.GetRegionSession(gc->m_regionId), it->dwUserID, 
				R(MSG_NATIONAL_WAR_FIELD), CHAT_CHANNEL_SYSTEM, s_szMsg);
		}
	}

	it = m_stWarField.listUserShang.begin();

	for(; it != m_stWarField.listUserShang.end(); ++it)
	{
		GWLoginClient* gc = g_server.FindLoginClientByActorId(it->dwUserID);

		if(gc)
		{
			g_server.m_chatTransmittal.Chat(g_server.GetRegionSession(gc->m_regionId), it->dwUserID, 
				R(MSG_NATIONAL_WAR_FIELD), CHAT_CHANNEL_SYSTEM, s_szMsg);
		}
	}
}

int CGwNationalWarMgr::EnterWarfield(DWORD dwUserID)
{
	GWUser*		pUser = g_server.FindActorById(dwUserID);

	char nfaction = pUser->attr.faction;

	if(!pUser)
	{
		m_szErrorInfo = R(MSG_NATIONAL_WAR_USER_INFO_ERR);	
		return 0;
	}

	if(m_stWarField.eState == CGwNationalWarMgr::eWaitStopping)
	{
		m_szErrorInfo = R(MSG_NATIONAL_WAR_FIELD_CLOSE);	
		return 0;
	}

	//如果战场还没有开放，添加到等待队列中
	if(m_stWarField.eState != CGwNationalWarMgr::eStarting)
	{
		if(pUser->attr.faction == FACTION_ZHOU)
		{
			if((int)m_stWarField.listAppZhou.size() > 300)
			{
				m_szErrorInfo = s_szWarfieldStop;
			}else
			{
				m_szErrorInfo = s_szWarfieldApp;

				time_t now = time(NULL);
				CGwNationalWarMgr::SAppUser	tmp;
				tmp.dwUserID = dwUserID;
				tmp.tStartTime = now;

				if(!FindUserInApp(dwUserID))
				{
					m_stWarField.listAppZhou.push_back(tmp);
				}
			}
		}else 
		{
			if((int)m_stWarField.listAppShang.size() > 300)
			{
				m_szErrorInfo = s_szWarfieldStop;
			}else
			{
				m_szErrorInfo = s_szWarfieldApp;

				time_t now = time(NULL);
				CGwNationalWarMgr::SAppUser	tmp;
				tmp.dwUserID = dwUserID;
				tmp.tStartTime = now;

				if(!FindUserInApp(dwUserID))
				{
					m_stWarField.listAppShang.push_back(tmp);
				}
			}
		}

		return 0;
	}

	//已经开放但是人满了
	if(GetUserNum(pUser->attr.faction) >= WF_NUM_LIMIT && !FindUser(pUser->userID))
	{
		m_szErrorInfo = s_szUserLimit;
		return 0;
	}

	//可以加入战场
	CGwNationalWarMgr::SUser	tmp;
	long	pos[3];
	tmp.bInNWar = true;
	tmp.dwUserID = dwUserID;
	tmp.dwLevel = pUser->attr.level;

	if(pUser->attr.faction == FACTION_ZHOU)
	{
		TLSUser::iterator it = m_stWarField.listUserZhou.begin();
		for(; it != m_stWarField.listUserZhou.end(); ++it)
		{
			if(it->dwUserID == dwUserID)
			{
				break;
			}
		}

		if(it == m_stWarField.listUserZhou.end())
		{
			m_stWarField.listUserZhou.push_back(tmp);
			memcpy((void*)pos, m_stWarField.stInfo.bornPosZhou, sizeof(long) * 3);
		}else
		{
			LOG1("user [%d] exist in warfield, but request enter again\n", dwUserID);
			memcpy((void*)pos, m_stWarField.stInfo.bornPosZhou, sizeof(long) * 3);
		}
	}else
	{
		TLSUser::iterator it = m_stWarField.listUserShang.begin();
		for(; it != m_stWarField.listUserShang.end(); ++it)
		{
			if(it->dwUserID == dwUserID)
			{
				break;
			}
		}

		if(it == m_stWarField.listUserShang.end())
		{
			m_stWarField.listUserShang.push_back(tmp);
			memcpy((void*)pos, m_stWarField.stInfo.bornPosShang, sizeof(long) * 3);
		}else
		{
			LOG1("user [%d] exist in warfield, but request enter again\n", dwUserID);
			memcpy((void*)pos, m_stWarField.stInfo.bornPosShang, sizeof(long) * 3);
		}
	}

	g_server.MoveUserToWarfield(dwUserID, pos[0], pos[1], true,nfaction);

	SendWarfieldMsg(dwUserID, msg_warfield_enter);

	time_t t = time(NULL);
	LOG2("User [%d] enter warfield at [%s]\n", dwUserID, ctime(&t));

	return 1;
}

bool CGwNationalWarMgr::RemoveUserInApp(DWORD dwUserID)
{
	stlforeach(TLSApp, it1, m_stWarField.listAppZhou)
	{
		if(it1->dwUserID == dwUserID)
		{
			it1 = m_stWarField.listAppZhou.erase(it1);
			return true;
		}
	}

	stlforeach(TLSApp, it2, m_stWarField.listAppShang)
	{
		if(it2->dwUserID == dwUserID)
		{
			it2 = m_stWarField.listAppShang.erase(it2);
			return true;
		}
	}

	return false;
}

bool CGwNationalWarMgr::Load(const char* file)
{
	SWarfieldInfo tmp;
	int j;
	RtCsv csv;

	if(!csv.LoadTableFile(file))
		return false;

	j = csv.FindLineByString("ID");
	if(j < 0) return false;

	//要拼接路径//add by fox
	static const char* pTaskPath=NULL;
	static CIniVariable Temp(GetConfigBoot(),"ConfigInfo","TaskDirectory",&pTaskPath);
	static string TaskPath(pTaskPath);

	tmp.html        = TaskPath + csv[j+1]["描述"].Str();
	tmp.script      = csv[j+1]["脚本"].Str();
	tmp.sceneFile   = csv[j+1]["场景文件"].Str();
	tmp.lev			= atol(csv[j+1]["等级限制"].Str());

	m_default_weekday = csv[j+1]["星期"].Int();
	m_default_hour = csv[j+1]["时"].Int();
	m_default_min = csv[j+1]["分"].Int();
	m_default_duration = csv[j+1]["持续时间(小时)"].Int();

	char* pos = (char*)csv[j+1]["周出生点"].Str();
	char  delim[] = ";";
	char* k;

	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.bornPosZhou[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.bornPosZhou[2] = 0;
	}

	pos = (char*)csv[j+1]["周退出点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.exitPosZhou[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.exitPosZhou[2] = 0;
	}

	pos = (char*)csv[j+1]["商出生点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.bornPosShang[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.bornPosShang[2] = 0;
	}

	pos = (char*)csv[j+1]["商退出点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.exitPosShang[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.exitPosShang[2] = 0;
	}

	tmp.exitSceneBlockZhou = csv[j+1]["周退出场景"].Str();
	tmp.exitSceneBlockShang = csv[j+1]["商退出场景"].Str();

	pos = (char*)csv[j+1]["周最终复活点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.posZhou.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.posZhou.pos[2] = 0;
		tmp.posZhou.faction = FACTION_ZHOU;
	}

	pos = (char*)csv[j+1]["商最终复活点"].Str();
	if(pos)
	{
		k = strtok(pos, delim);

		if(k && strlen(k)>0)
		{
			for(int i = 0; k && (i < 2); i++)
			{
				tmp.posShang.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}
		}

		tmp.posShang.pos[2] = 0;
		tmp.posShang.faction = FACTION_SHANG;
	}

	pos = (char*)csv[j+1]["周控制复活点"].Str();
	if(pos)
	{
		for(k = strtok(pos, delim); k && strlen(k)>0; )
		{
			SRebirthPos tmpPos;
			tmpPos.npcID = atol(k);

			k = strtok(NULL, delim);

			//读取可选npc
			for(int i = 0; k && (i < 2); ++i)
			{
				for(int j = 0; k && (j < 2); ++j)
				{
					tmpPos.optionalNpcID[i][j] = atol(k);
					k = strtok(NULL, delim);
				}
			}

			//读取坐标
			for(int i = 0; k && (i < 2); i++)
			{
				tmpPos.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}

			tmpPos.pos[2] = 0;
			tmpPos.faction = FACTION_ZHOU;
			tmp.posTmp.push_back(tmpPos);
		}		
	}

	pos = (char*)csv[j+1]["商控制复活点"].Str();
	if(pos)
	{
		for(k = strtok(pos, delim); k && strlen(k)>0; )
		{
			SRebirthPos tmpPos;
			tmpPos.npcID = atol(k);

			k = strtok(NULL, delim);

			//读取可选npc
			for(int i = 0; k && (i < 2); ++i)
			{
				for(int j = 0; k && (j < 2); ++j)
				{
					tmpPos.optionalNpcID[i][j] = atol(k);
					k = strtok(NULL, delim);
				}
			}

			for(int i = 0; k && (i < 2); i++)
			{
				tmpPos.pos[i] = atol(k);
				k = strtok(NULL, delim);
			}

			tmpPos.pos[2] = 0;
			tmpPos.faction = FACTION_SHANG;
			tmp.posTmp.push_back(tmpPos);
		}
	}

	m_stWarField.stInfo = tmp;

	return true;
}

bool CGwNationalWarMgr::CanStart()
{
	return true;
}

void CGwNationalWarMgr::OnRecvExploit(char faction, long lUnionID, long lExploit)
{
	if(CGwNationalWarMgr::eStarting != m_stWarField.eState) return;
	if(FACTION_UNKNOWN == faction) return;
	if(lUnionID == 0)	return;

	std::list<SExploit>::iterator it = m_stWarField.listExploit.begin();
	for(; it != m_stWarField.listExploit.end(); ++it)
	{
		if(it->dwUnionID != lUnionID) continue;

		it->lExploit = lExploit;
		break;
	}

	if(it == m_stWarField.listExploit.end())
	{
		SExploit tmp;

		tmp.dwUnionID = lUnionID;
		tmp.faction = faction;
		tmp.lScore = 0;
		tmp.lExploit = lExploit;

		m_stWarField.listExploit.push_back(tmp);
	}
}

void CGwNationalWarMgr::BroadcastUpdateData()
{
	time_t now = time(NULL);
	long remain = (long)(m_stWarField.iDuration * 60 * 60 - (now - m_stWarField.tStartTime));

	for (long id_cnt = 0; id_cnt < (long)g_server.m_regionTable.size(); id_cnt++)
	{
		GWRegion& rg = g_server.m_regionTable[id_cnt];

		if (rg.sessionIndex == -1)
			continue;

		//if (rg.sessionIndex == g_server.GetRegionSession(m_stWarField.lRsidx))
		//	continue;

		CG_CmdPacket& cmd = g_server.m_region.GetSendCmd();
		cmd.BeginWrite();
		cmd.WriteShort(g2r_warfield_update_data);
		cmd.WriteLong(remain);
		cmd.WriteLong((long)m_stWarField.listUserZhou.size());
		for(std::list<SUser>::iterator it1 = m_stWarField.listUserZhou.begin(); it1 != m_stWarField.listUserZhou.end(); it1++)
		{
			cmd.WriteLong((long)it1->dwUserID);
		}
		cmd.WriteFloat(m_stWarField.lScoreZhou);
		cmd.WriteLong((long)m_stWarField.listUserShang.size());
		for(std::list<SUser>::iterator it2 = m_stWarField.listUserShang.begin(); it2 != m_stWarField.listUserShang.end(); it2++)
		{
			cmd.WriteLong((long)it2->dwUserID);
		}
		cmd.WriteFloat(m_stWarField.lScoreShang);

		cmd.WriteFloat(m_stWarField.lPriHero[0]);
		cmd.WriteFloat(m_stWarField.lPriHero[1]);
		cmd.WriteFloat(m_stWarField.lPriHero[2]);
		cmd.WriteFloat(m_stWarField.lInterHero[0]);
		cmd.WriteFloat(m_stWarField.lInterHero[1]);
		cmd.WriteFloat(m_stWarField.lInterHero[2]);
		cmd.WriteFloat(m_stWarField.lSeniorHero[0]);
		cmd.WriteFloat(m_stWarField.lSeniorHero[1]);
		cmd.WriteFloat(m_stWarField.lSeniorHero[2]);

		cmd.WriteLong((long)m_stWarField.listExploit.size());
		std::list<SExploit>::iterator it = m_stWarField.listExploit.begin();
		for(; it != m_stWarField.listExploit.end(); ++it)
		{
			it->Serialize(&cmd);
		}

		cmd.WriteLong((long)m_stWarField.listPersonExploit.size());
		std::list<SPerExploit>::iterator it3 = m_stWarField.listPersonExploit.begin();
		for(; it3 != m_stWarField.listPersonExploit.end(); ++it3)
		{
			cmd.WriteLong((long)it3->dwUserID);
			cmd.WriteLong((long)it3->dwExploit);
		}

		g_server.m_region.SendToPeer(rg.sessionIndex);
	}
}

void CGwNationalWarMgr::OnAddScore(char faction, float score)
{
	if(FACTION_UNKNOWN == faction) return;

	if(score == WF_SCORE_MAX)
	{
		if(faction == FACTION_SHANG)
		{
			m_bFlagDestroyedZhou = true;
			m_bFlagDestroyedShang = false;
		}else
		{
			m_bFlagDestroyedShang = true;
			m_bFlagDestroyedZhou = false;
		}

		Stop();
	}else
	{	
		if(faction == FACTION_SHANG)
		{
			m_stWarField.lScoreShang += score;
		}else
		{
			m_stWarField.lScoreZhou += score;
		}
	}
}

void CGwNationalWarMgr::OnRecvUpdateData()
{
}

void CGwNationalWarMgr::GetStartTime(time_t& ret)
{
	struct tm*	p_local_time;	//当前时间
	DWORD		interval;		//时间间隔(second)

	//取得当前时间：年月日分秒
	time_t tmp = 0;	
	time(&tmp);
	p_local_time = localtime(&tmp);

	//国战一周一次
	if((p_local_time->tm_wday < m_default_weekday)  ||
		((p_local_time->tm_wday == m_default_weekday) && (p_local_time->tm_hour < m_default_hour)) ||
		((p_local_time->tm_wday == m_default_weekday) && (p_local_time->tm_hour == m_default_hour) && 
		(p_local_time->tm_min < m_default_min)))
	{
		interval = ((m_default_weekday - p_local_time->tm_wday) * 24 * 60 * 60) +   //天数差
			((m_default_hour - p_local_time->tm_hour) * 60 * 60) +				//小时差
			((m_default_min - p_local_time->tm_min) * 60) -						//分钟差
			(p_local_time->tm_sec);									//秒差
	}else
	{
		interval = ((7 - (p_local_time->tm_wday - m_default_weekday)) * 24 * 60 * 60) -     //天数差
			((p_local_time->tm_hour - m_default_hour) * 60 * 60) -						//小时差
			((p_local_time->tm_min - m_default_min) * 60) -								//分钟差
			(p_local_time->tm_sec);											//秒差
	}

	ret = tmp + interval;

	return;
}

//void CGwNationalWarMgr::ClearDB()
//{
//	GWDBCommand cmd;
//	MYSQL_RES* res;
//
//	rt2_sprintf(cmd.command, "delete * from T_warfield");
//
//	if (!g_server.m_database.ExecCommand(cmd.command, &res))
//	{
//		LOG1("SQL Command Error : [%s]\n", cmd.command);
//		return;
//	}
//}

bool CGwNationalWarMgr::Start()
{	
	if(!m_bCanStart)
	{
		return false;
	}

	int se = g_server.GetRegionSession(m_stWarField.lRsidx);

	if(se == -1)
	{
		return false;
	}

	LOG("Warfield Wait for Starting......\n");
	CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_start);
	g_server.m_region.SendToPeer(se, &cmd);

	return true;
}

void CGwNationalWarMgr::OnRsStartRep(bool bResult)
{
	if(bResult)
	{
		time_t tmp = time(NULL);
		LOG1("Warfield started at %s\n", ctime(&tmp));
		m_stWarField.eState = CGwNationalWarMgr::eStarting;
		BroadcastWarfieldMsg(WF_ALL, msg_warfield_start);
		BroadcastUpdateData();
	}
}

void CGwNationalWarMgr::OnRecvConjureProgress(char faction, float progress, long lev)
{
	if(FACTION_UNKNOWN == faction) return;

	float* lHero = NULL;

	if(lev == 1) lHero = (float*)m_stWarField.lPriHero;
	else if(lev == 2) lHero = (float*)m_stWarField.lInterHero;
	else if(lev == 3) lHero = (float*)m_stWarField.lSeniorHero;
	else return;

	lHero[(int)faction] = (progress >= lHero[2]) ? lHero[2]-1 : progress;

	return;
}

void CGwNationalWarMgr::OnGMStart()
{
	if(m_stWarField.eState == CGwNationalWarMgr::eStop)
	{
		m_stWarField.tStartTime = time(NULL);
		m_stWarField.tEndTime = m_stWarField.tStartTime + 3 * 60 * 60;

		LOG1("GM Started Warfield at %s\n", ctime(&m_stWarField.tStartTime));
	}else
	{
		LOG("GM Started Warfield so frequently\n");
	}
}

void CGwNationalWarMgr::OnGMStop()
{
	if(m_stWarField.eState == CGwNationalWarMgr::eStarting)
	{
		m_stWarField.tEndTime = time(NULL);
	}

	LOG1("GM Stoped Warfield at %s\n", ctime(&m_stWarField.tStartTime));
}

void CGwNationalWarMgr::OnRegionRegister(int regionID)
{
	if(regionID == m_stWarField.lRsidx)
	{
		m_bCanStart = true;
	}
}

void CGwNationalWarMgr::OnRegionDisconnected(int regionID)
{
	if(m_stWarField.eState == CGwNationalWarMgr::eStop)
	{
		return;
	}

	if(m_stWarField.eState == CGwNationalWarMgr::eWaitStopping)
	{
		m_bHad = true;
		return;
	}

	if(regionID == m_stWarField.lRsidx)
	{
		m_bCanStart = false;
		m_stWarField.eState = CGwNationalWarMgr::eStop;

		time_t tmp = time(NULL);
		LOG1("Because of Region Disconnect at %s, Reset\n", ctime(&tmp));
	}
}

void CGwNationalWarMgr::OnRsStopRep(bool bResult)
{
	if(bResult)
	{
		time_t tmp = time(NULL);
		LOG1("Warfield stop at %s\n", ctime(&tmp));

		m_bHad = true;

		//region关闭战场成功， 清空队列
		m_stWarField.Reset();
		m_stWarField.eState = CGwNationalWarMgr::eStop;
		m_bFlagDestroyedZhou = false;
		m_bFlagDestroyedShang = false;
		m_Winner = FACTION_UNKNOWN;

		GetStartTime(m_stWarField.tStartTime);
		m_stWarField.tEndTime = m_stWarField.tStartTime + m_stWarField.iDuration * 60 * 60;

		BroadcastWarfieldMsg(WF_ALL, msg_warfield_stop);
	}else
	{
		LOG("UnKnown Reason : Warfield cannot stop\n");
	}
}

void CGwNationalWarMgr::OnAddPersonExploit(DWORD dwUserID, long lExploit)
{
	//CGwNationalWarMgr::SUser* pWUser = FindUser(dwUserID);
	//if(!pWUser)
	//{
	//	LOG1("Warfield [OnAddPersonExploit]: User [%d] did not join in warfield\n",dwUserID);
	//	return;
	//}

	std::string strExploit = rtFormatNumber(lExploit);

	g_GiftMgr.AddGift(dwUserID, 0, GT_Exploit, strExploit.c_str(), 0, 0, R(MSG_NATIONAL_WAR_HORTATION));
}

void CGwNationalWarMgr::OnAddExp(DWORD dwUserID, char faction)
{
	if(m_Winner == FACTION_UNKNOWN) return;

	CGwNationalWarMgr::SUser* pWUser = FindUser(dwUserID);
	if(!pWUser)
	{
		LOG1("Warfield [OnAddExp]: User [%d] did not join in warfield\n",dwUserID);
		return;
	}

	long lExp = 0;
	DWORD dwLev = pWUser->dwLevel;

	if(dwLev >= 20 && dwLev < 40)
	{
		//lExp = g_TableExp.GetNextLevExp(dwLev) * 20 / 100;
		lExp = WF_EXP_20_40(dwLev);
	}else if(dwLev >= 40 && dwLev < 60)
	{
		lExp = WF_EXP_40_60;
	}else if(dwLev >= 60 && dwLev < 80)
	{
		lExp = WF_EXP_60_80;
	}else if(dwLev >= 80 && dwLev < 100)
	{
		lExp = WF_EXP_80_100;
	}

	if(m_Winner != faction)
	{
		lExp /= 2;
	}

	std::string strExp = rtFormatNumber(lExp);

	g_GiftMgr.AddGift(dwUserID, 0, GT_Exp, strExp.c_str(), 0, 0, R(MSG_NATIONAL_WAR_HORTATION));

	LOG2("[Warfield Exp]: [%d]---[%s]\n", dwUserID, (char*)strExp.c_str());
}

void CGwNationalWarMgr::OnUserOnline(DWORD dwUserID)
{
	GWUser* pUser = g_server.FindActorById(dwUserID);
	if(!pUser) return;

	CGwNationalWarMgr::SUser* pWUser = FindUser(dwUserID);

	if(pWUser)
	{
		pWUser->dwTimeout = 0;
		if(pUser->IsInWarfield && m_stWarField.eState == CGwNationalWarMgr::eStarting)
		{
			SendWarfieldMsg(dwUserID, msg_warfield_start);
			SendWarfieldMsg(dwUserID, msg_warfield_enter);
		}else
		{
		}
	}else
	{
		pUser->IsInWarfield = false;
	}
}

bool CGwNationalWarMgr::FindUserInApp(DWORD dwUserID)
{
	stlforeach(TLSApp, it1, m_stWarField.listAppZhou)
	{
		if(it1->dwUserID == dwUserID)
		{
			return true;
		}
	}

	stlforeach(TLSApp, it2, m_stWarField.listAppShang)
	{
		if(it2->dwUserID == dwUserID)
		{
			return true;
		}
	}

	return false;
}

CGwNationalWarMgr::SUser* CGwNationalWarMgr::FindUser(DWORD dwUserID)
{
	stlforeach(TLSUser, it1, m_stWarField.listUserZhou)
	{
		if(it1->dwUserID == dwUserID)
		{
			return (CGwNationalWarMgr::SUser*)&*it1;
		}
	}

	stlforeach(TLSUser, it2, m_stWarField.listUserShang)
	{
		if(it2->dwUserID == dwUserID)
		{
			return (CGwNationalWarMgr::SUser*)&*it2;
		}
	}

	return NULL;
}

void CGwNationalWarMgr::OnUserOffline(DWORD dwUserID)
{
	if(m_stWarField.eState != CGwNationalWarMgr::eStarting)	return;
	if(!m_bCanStart) return;

	CGwNationalWarMgr::SUser* pUser = FindUser(dwUserID);
	if(!pUser) return;

	pUser->dwTimeout = 1;
	return;
}

//void CGwNationalWarMgr::OnCreateArms(DWORD dwUserID, long dwLev, long lType)
//{
//	if(lType > 2)
//	{
//		LOG1("Npc Type Invalid [%d]\n", lType);
//		return;
//	}
//	stlforeach(TLSUser, it1, m_stWarField.listUserZhou)
//	{
//		if(it1->dwUserID == dwUserID)
//		{
//			it1->dwExploit += iExploit[lType];
//			it1->dwExploit = (it1->dwExploit > EMAX(dwLev)) ? EMAX(dwLev) : it1->dwExploit;
//			return;
//		}
//	}
//
//	stlforeach(TLSUser, it2, m_stWarField.listUserShang)
//	{
//		if(it2->dwUserID == dwUserID)
//		{
//			it2->dwExploit += iExploit[lType];
//			it2->dwExploit = (it1->dwExploit > EMAX(dwLev)) ? EMAX(dwLev) : it2->dwExploit;
//			return;
//		}
//	}
//
//	LOG2("User [%d] create npc [%d], but cannot find the user\n ", dwUserID, lType);
//}

void CGwNationalWarMgr::OnRecvPerExploit(DWORD dwUserID, DWORD dwExploit)
{
	stlforeach(std::list<SPerExploit>, it, m_stWarField.listPersonExploit)
	{
		if(it->dwUserID == dwUserID)
		{
			it->dwExploit = dwExploit;
			break;
		}
	}

	if(it == m_stWarField.listPersonExploit.end())
	{
		SPerExploit tmp;
		tmp.dwUserID = dwUserID;
		tmp.dwExploit = dwExploit;

		m_stWarField.listPersonExploit.push_back(tmp);
	}
}

void CGwNationalWarMgr::OnUserLeave(DWORD dwUserID)
{
	GWUser* pUser = g_server.FindActorById(dwUserID);

	if(!pUser)
	{
		return;
	}

	bool bSuccess = false;

	if(pUser->attr.faction == FACTION_ZHOU)
	{
		stlforeach(TLSUser, it, m_stWarField.listUserZhou)
		{
			if(it->dwUserID == dwUserID)
			{
				m_stWarField.listUserZhou.erase(it);
				bSuccess = true;
				break;
			}
		}
	}else
	{
		stlforeach(TLSUser, it, m_stWarField.listUserShang)
		{
			if(it->dwUserID == dwUserID)
			{
				m_stWarField.listUserShang.erase(it);
				bSuccess = true;
				break;
			}
		}
	}

	if(bSuccess)
	{
		time_t t = time(NULL);
		LOG2("user [%d] leave warfield at [%s]\n", dwUserID, ctime(&t));
		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_leave_rep);
		cmd.WriteLong(dwUserID);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(m_stWarField.lRsidx), &cmd);

		SendWarfieldMsg(dwUserID, msg_warfield_leave);
	}else
	{
		LOG1("user [%d] is not in warfield\n", dwUserID);
		CG_CmdPacket& cmd = g_server.m_region.BeginCmd(g2r_warfield_leave_rep);
		cmd.WriteLong(dwUserID);
		g_server.m_region.SendToPeer(g_server.GetRegionSession(m_stWarField.lRsidx), &cmd);

		SendWarfieldMsg(dwUserID, msg_warfield_leave);
	}
	/*add by Tianh 退出国战 阵营为中立 09.12.22*/
	pUser->attr.faction = FACTION_NEUTRAL;
}

