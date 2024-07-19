#include "RegionActManager.h"

extern CRS_Activity	g_TableActivity;
//////////////////////////////////////////////////////////////////////////

CRegionActManager::CRegionActManager(void) :
ciDelListmaxCount(100)
{
}

CRegionActManager::~CRegionActManager(void)
{
}

void	CRegionActManager::CreateActivity(CG_CmdPacket *pCommand)
{
	long iID = -1;
	pCommand->ReadLong(&iID);
	if(iID <= 0)
		return;
	short iType = -1;
	pCommand->ReadShort(&iType);
	if(iType < 0 || iType >= ACTIVITY_TYPE_OVER)
		return;
	char *sActivity = 0;
	pCommand->ReadString(&sActivity);
	if(!sActivity)
		return;

	CRegionActivityBase *pActivity = GetActivity(iID);
	if(pActivity)
	{
		pActivity->UpdataInfo(sActivity, true);
	}
	else
	{
		switch(iType)
		{
		case ACTIVITY_TELPHER:
			pActivity = RT_NEW ActivityTelpher(iID);
			pActivity->UpdataInfo(sActivity, true);
			break;
		case ACTIVITY_TREASURE:
			pActivity = RT_NEW ActivityTreasure(iID);
			pActivity->UpdataInfo(sActivity, true);
			break;
		case ACTIVITY_CACHOT:
			pActivity = RT_NEW ActivityCachotClass(iID);
			pActivity->UpdataInfo(sActivity, true);
			break;
		}

		if(pActivity)
			mapActivitys[iID] = pActivity;
	}
}

void	CRegionActManager::RemoveActivity(int iID)
{
	ACTIVITYS::iterator itAct = mapActivitys.find(iID);
	if(itAct != mapActivitys.end())
	{
		DEL_ONE(itAct->second);
		mapActivitys.erase(itAct);
	}
}

void	CRegionActManager::SetUserEnterActivity(CG_CmdPacket *pCommand)
{
	char *sUserName = 0;
	pCommand->ReadString(&sUserName);
	char *sBuffer = 0;
	pCommand->ReadString(&sBuffer);
	CRegionUser *pUser = CRegionActivityBase::GetCreatureByName(sUserName);
	if(pUser)
		pUser->UpdataActivityJoin(sBuffer);
}

bool	CRegionActManager::CanExecute(int iID, CRegionUser *pUser)
{
	if(!pUser)
		return false;
	CRegionActivityBase *pActivity = GetActivity(iID);
	if(pActivity)
	{
		return pActivity->CanExecute(pUser);
	}
	return false;
}

bool	CRegionActManager::Execute(int iID, CRegionUser *pUser, long *plParam)
{
	if(!pUser)
		return false;
	CRegionActivityBase *pActivity = GetActivity(iID);
	if(pActivity)
	{
		return pActivity->Execute(pUser, plParam);
	}
	return false;
}

bool	CRegionActManager::Complate(int iID, CRegionUser *pUser)
{
	CRegionActivityBase *pActivity = GetActivity(iID);
	if(pActivity)
	{
		return pActivity->Complate(pUser);
	}
	return false;
}

void	CRegionActManager::OnEvent(CG_CmdPacket *pCommand)
{
	if(!pCommand)
		return;

	short iMsg = -1;
	pCommand->ReadShort(&iMsg);
	if(iMsg < 0 || iMsg >= ACT_MESSAGE_OVER)
		return;

	CRegionActivityBase *pActivity = 0;
	long iID = 0;
	char *pBroadcastInfo = 0;
	switch(iMsg)
	{
	case ACTMSG_CREATE:
		CreateActivity(pCommand);
		break;
	case ACTMSG_DESTORY:
		pCommand->ReadLong(&iID);
		if(iID <= 0)
			break;
		RemoveActivity(iID);
		break;
	case ACTMSG_BROADCAST:
		{
			pCommand->ReadString(&pBroadcastInfo);
			CG_CmdPacket& cmd = g_region->m_gws.BeginSend();
			cmd.WriteShort(r2g_broadcast_bulletin);
			cmd.WriteLong(0);
			cmd.WriteString(pBroadcastInfo);
			cmd.WriteLong(-1);
			cmd.WriteByte(0);
			g_region->m_gws.EndSend();
		}

		//pCommand->ReadString(&pBroadcastInfo);
		//g_sendCmd->BeginWrite();
		//g_sendCmd->WriteShort(r2c_chat);
		//g_sendCmd->WriteByte(CHAT_CHANNEL_SYSTEM);
		//g_sendCmd->WriteLong(0);
		//g_sendCmd->WriteString((char*)R(MSG_ACTIVITY_BROADCAST));
		//g_sendCmd->WriteString(pBroadcastInfo);
		//g_region->BroadcastCmd(g_sendCmd, NULL);
		break;
	case ACTMSG_UPDATAUSERJOIN:
		SetUserEnterActivity(pCommand);
		break;
	default:
		pCommand->ReadLong(&iID);
		if(iID <= 0)
			break;
		pActivity = GetActivity(iID);
		if(pActivity)
			pActivity->OnEvent((ACTIVITY_MESSAGE)iMsg, pCommand);
		break;
	}
}

void	CRegionActManager::SaveUserInfo(int iID, CRegionUser *pUser, bool bEnterActive, UserActivityInfo *pInfo)
{
	if(!pInfo || !pUser)
		return;

	std::string sBuffer;
	pInfo->SerializeToString(&sBuffer);
	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_activity_system);
	cmd.WriteShort(ACTMSG_SAVEUSERINFO);
	cmd.WriteLong(iID);
	cmd.WriteString(pUser->m_username.c_str());
	cmd.WriteShort(bEnterActive ? 1 : 0);
	cmd.WriteString(sBuffer.c_str());
	g_region->m_gws.EndSend();
}

void	CRegionActManager::Shutdown(void)
{
	for(ACTIVITYS::iterator itAct = mapActivitys.begin();
		itAct != mapActivitys.end();
		++itAct)
		DEL_ONE(itAct->second);
	mapActivitys.clear();
}

CRegionActivityBase*	CRegionActManager::GetActivity(int iID)
{
	ACTIVITYS::iterator itAct = mapActivitys.find(iID);
	if(itAct != mapActivitys.end())
	{
		return itAct->second;
	}
	return NULL;
}

UserActivityInfo*	CRegionActManager::GetNewUserActivityInfo(ACTIVITY_TYPE eType)
{
	UserActivityInfo *pInfo = NULL;
	switch(eType)
	{
	case ACTIVITY_BASE:
		//pInfo = RT_NEW UserActivityInfo();
		break;
	case ACTIVITY_TELPHER:
	case ACTIVITY_TREASURE:
		pInfo = RT_NEW UserActivityInfo();
		break;
	case ACTIVITY_CACHOT:
		pInfo = RT_NEW UserCachotInfo();
		break;
	}

	if(pInfo)
	{
		pInfo->eActivityType = eType;
		//CRegionActManager::GetSingle()->listDelInfo.push_back(pInfo);

		//if(CRegionActManager::GetSingle()->listDelInfo.size() >= CRegionActManager::GetSingle()->ciDelListmaxCount)
		//{
		//	DELETE_LIST::iterator it = CRegionActManager::GetSingle()->listDelInfo.begin();
		//	DEL_ONE(*it);
		//	CRegionActManager::GetSingle()->listDelInfo.erase(it);
		//}
	}
	return pInfo;
}

//////////////////////////////////////////////////////////////////////////

CRegionUser*	CRegionActivityBase::GetCreatureByID(long lID)
{
	CRegionUser *pUser = g_region->FindUser(lID);
	return pUser;
}
CRegionUser*	CRegionActivityBase::GetCreatureByName(const char *sName)
{
	if(!sName)
		return NULL;
	CRegionUser *pUser = g_region->FindUser(sName, true);
	return pUser;
}

CRegionActivityBase::CRegionActivityBase(int iID) :
m_iID(iID < 0 ? 0 : iID),
m_eType(ACTIVITY_BASE),
m_pActivityInfo(0)
{
	m_type = OBJECT_TYPE_ACTIVITY;
}

CRegionActivityBase::~CRegionActivityBase(void)
{
	m_listNPC.clear();
	DEL_ONE(m_pActivityInfo);
}

void	CRegionActivityBase::AddLinkNPC(int iNPC_ID)
{
	for(LINK_NPC::iterator it = m_listNPC.begin();
		it != m_listNPC.end();
		it++)
	{
		if((*it) == iNPC_ID)
			return;
	}
	m_listNPC.push_back(iNPC_ID);
}

CRegionCreature*	CRegionActivityBase::GetLinkNPCByIndex(int iIndex)
{
	int ii=0;
	for(LINK_NPC::iterator it = m_listNPC.begin();
		it != m_listNPC.end();
		it++)
	{
		if(ii == iIndex)
		{
			CRegionCreature *pCreature = g_region->m_defScene->FindCreature(*it);
			if(pCreature && (pCreature->IsUser() || pCreature->IsUserPet()))
			{
				m_listNPC.erase(it);
				return NULL;
			}
			return pCreature;
		}
		++ii;
	}

	return NULL;
}

void	CRegionActivityBase::GetAllLinkNPC(std::list<CRegionCreature*> &listCreature)
{
	listCreature.clear();
	for(LINK_NPC::iterator it = m_listNPC.begin();
		it != m_listNPC.end();
		it++)
	{
		CRegionCreature *pCreature = g_region->m_defScene->FindCreature(*it);
		if(pCreature && (pCreature->IsUser() || pCreature->IsUserPet()))
		{
			m_listNPC.erase(it);
			continue;
		}
		if(pCreature)
			listCreature.push_back(pCreature);
	}
}

void	CRegionActivityBase::ClearLinkNPCByIndex(int iIndex)
{
	if(iIndex < 0)
		m_listNPC.clear();

	int ii=0;
	for(LINK_NPC::iterator it = m_listNPC.begin();
		it != m_listNPC.end();
		it++)
	{
		if(ii == iIndex)
		{
			m_listNPC.erase(it);
			break;
		}
		++ii;
	}
}

void	CRegionActivityBase::ClearLinkNPCByID(int iID)
{
	if(iID < 0)
		m_listNPC.clear();

	for(LINK_NPC::iterator it = m_listNPC.begin();
		it != m_listNPC.end();
		it++)
	{
		if(*it == iID)
		{
			m_listNPC.erase(it);
			break;
		}
	}
}

void	CRegionActivityBase::StartUp(long lIndex)
{
	int iTemp = 0;
	if(m_pActivityInfo)
	{
		m_pActivityInfo->eState = AS_START;
		int iTime[Activity::ciActTimeCount] = {0};
		rtSystemTime(iTime[0], iTime[1],
			iTemp, iTime[2], iTime[3],
			iTime[4], iTemp, iTemp);
		for(int ii=0; ii<Activity::ciActTimeCount; ii++)
			m_pActivityInfo->iUpdataTime[ii] = (short)iTime[ii];
		if(IsScriptLoaded())
			CallScript("OnStart", false, lIndex);
	}
}

void	CRegionActivityBase::Shutdown(long lIndex)
{
	int iTemp = 0;
	if(m_pActivityInfo)
	{
		m_pActivityInfo->eState = AS_OVER;
		int iTime[Activity::ciActTimeCount] = {0};
		rtSystemTime(iTime[0], iTime[1],
			iTemp, iTime[2], iTime[3],
			iTime[4], iTemp, iTemp);
		for(int ii=0; ii<Activity::ciActTimeCount; ii++)
			m_pActivityInfo->iUpdataTime[ii] = (short)iTime[ii];
		if(IsScriptLoaded())
			CallScript("OnShutdown", false, lIndex);
	}
}

//执行活动，返回FALSE表示活动没有开启或错误，返回TRUE表示成功或需要等待处理或已经参加了
bool	CRegionActivityBase::Execute(CRegionUser *pUser, long *plParam)
{
	if(!pUser || !m_pActivityInfo)
		return false;
	if(m_pActivityInfo->eState != AS_START)
	{
		pUser->m_dummy->SendSystemMessage(R(MSG_ACTIVITY_NOT_START));
		return false;
	}

	if(pUser->GetActivityJoin(m_eType))
	{
		ExecuteFailed(pUser);
		return true;
	}
	else
	{
		UserActivityInfo *pInfo = pUser->GetActivityInfo(m_eType);
		if(pInfo)
		{
			if(Compera(pInfo, pUser))
			{
				SetUserInfo(AS_START, pUser, pInfo, false, plParam);
				return true;
			}
			else
			{
				ExecuteFailed(pUser);
				return false;
			}
		}
		else
		{
			GetUserInfo(pUser, AS_START, plParam);
			return true;
		}
	}
	return false;
}

//完成活动，返回FALSE表示活动没有参加或错误，返回TRUE表示成功或需要等待处理
bool	CRegionActivityBase::Complate(CRegionUser *pUser)
{
	if(!pUser || !pUser->GetActivityJoin(m_eType))
		return false;

	UserActivityInfo *pInfo = pUser->GetActivityInfo(m_eType);
	if(pInfo)
	{
		if(pInfo->eState != AS_START)
			return false;
		SetUserInfo(AS_OVER, pUser, pInfo, false, 0);
		return true;
	}
	else
	{
		long temp[5] = {0};
		GetUserInfo(pUser, AS_OVER, temp);
		return true;
	}
	return false;
}

bool	CRegionActivityBase::CanExecute(CRegionUser *pUser)
{
	if(!pUser || !m_pActivityInfo)
		return false;
	if(m_pActivityInfo->eState != AS_START)
	{
		if(IsScriptLoaded())
			CallScript("NotJoinActivity", false, pUser->m_dummy);
		return true;
	}

	if(pUser->GetActivityJoin(m_eType))
	{
		if(IsScriptLoaded())
			CallScript("NotJoinActivity", false, pUser->m_dummy);
		return true;
	}
	else
	{
		UserActivityInfo *pInfo = pUser->GetActivityInfo(m_eType);
		if(pInfo)
		{
			if(Compera(pInfo, pUser))
			{
				if(IsScriptLoaded())
					CallScript("CanJoinActivity", false, pUser->m_dummy);
				return true;
			}
			else
			{
				if(IsScriptLoaded())
					CallScript("NotJoinActivity", false, pUser->m_dummy);
				return false;
			}
		}
		else
		{
			long plParam[5] = {0};
			GetUserInfo(pUser, AS_CAN, plParam);
			return true;
		}
	}

	return false;
}

void	CRegionActivityBase::OnEvent(ACTIVITY_MESSAGE eMsgType, CG_CmdPacket *pCommand)
{
	if(!pCommand)
		return;

	bool bHasNewInfo = false;
	long lUserID = 0;
	char *sActivity = 0;
	CRegionUser *pUser = 0;
	long lIndex = -1;
	long lState = -1;
	long lJoinParam[5] = {0};
	switch(eMsgType)
	{
	case ACTMSG_START:
		pCommand->ReadLong(&lIndex);
		StartUp(lIndex);
		break;
	case ACTMSG_OVER:
		pCommand->ReadLong(&lIndex);
		Shutdown(lIndex);
		break;
	case ACTMSG_CYCLE:
		if(IsScriptLoaded())
			CallScript("OnEvent", false);
		break;
	case ACTMSG_GETUSERINFO:
		pCommand->ReadLong(&lUserID);
		pCommand->ReadLong(&lState);
		pCommand->ReadString(&sActivity);
		if(!lUserID || !sActivity)
			break;
		for(int ii=0; ii<5; ii++)
			pCommand->ReadLong(&(lJoinParam[ii]));
		pUser = GetCreatureByID(lUserID);
		UserActivityInfo *pInfo = NULL;
		if(pUser)
			pInfo = pUser->GetActivityInfo(m_eType);
		if(!pInfo)
		{
			pInfo = CRegionActManager::GetNewUserActivityInfo(m_eType);
			bHasNewInfo = true;
		}
		if(pInfo)
		{
			pInfo->SerializeToThis(&std::string(sActivity));
			if(lState == AS_START)
			{
				if(Compera(pInfo, pUser))
					SetUserInfo((ACTIVE_STATE)lState, pUser, pInfo, bHasNewInfo, lJoinParam);
				else
				{
					ExecuteFailed(pUser);
					if(!pUser->SetActivityInfo(m_eType, pInfo) && bHasNewInfo)
						DEL_ONE(pInfo);
				}
			}
			else if(lState == AS_CAN)
			{
				if(Compera(pInfo, pUser))
				{
					if(IsScriptLoaded())
						CallScript("CanJoinActivity", false, pUser->m_dummy);
				}
				else if(IsScriptLoaded())
				{
					CallScript("NotJoinActivity", false, pUser->m_dummy);
				}
				if(bHasNewInfo)
					DEL_ONE(pInfo);
			}
			else
			{
				if(pInfo->eState == AS_START)
					SetUserInfo((ACTIVE_STATE)lState, pUser, pInfo, bHasNewInfo, lJoinParam);
				else if(bHasNewInfo)
					DEL_ONE(pInfo);
			}
		}
		break;
	}
}

void	CRegionActivityBase::UpdataInfo(std::string sString, bool bRealTime)
{
	if(!sString.empty() && m_pActivityInfo)
	{
		m_pActivityInfo->SerializeToThis(&sString);
		if(bRealTime)
		{
			int iTemp = 0;
			int iTime[Activity::ciActTimeCount] = {0};
			rtSystemTime(iTime[0], iTime[1],
				iTemp, iTime[2], iTime[3],
				iTime[4], iTemp, iTemp);
			for(int ii=0; ii<Activity::ciActTimeCount; ii++)
				m_pActivityInfo->iUpdataTime[ii] = (short)iTime[ii];
		}
	}
}

bool	CRegionActivityBase::Compera(UserActivityInfo *pInfo, CRegionUser *pUser)
{
	if(!m_pActivityInfo || !pInfo || !pUser || !pUser->m_dummy)
		return false;

	return pInfo->TimeLessEqual(m_pActivityInfo->iUpdataTime) && m_pActivityInfo->CanJoinActivity(pUser->m_dummy->m_core.Lev, pInfo);
	//return m_pActivityInfo->TimeLessEqual(pInfo->iUpdataTime);
}

void	CRegionActivityBase::GetUserInfo(CRegionUser *pUser, ACTIVE_STATE eState, long *plParam)
{
	if(!pUser || !plParam)
		return;

	CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
	cmd.WriteShort(r2g_activity_system);
	cmd.WriteShort(ACTMSG_GETUSERINFO);
	cmd.WriteLong(m_eType);//m_iID);
	cmd.WriteShort(m_eType);
	cmd.WriteShort(eState);
	cmd.WriteString(pUser->m_username.c_str());
	cmd.WriteLong(pUser->m_userId);
	for(int ii = 0; ii < 5; ii ++)
		cmd.WriteLong(plParam[ii]);
	g_region->m_gws.EndSend();
}

void	CRegionActivityBase::SetUserInfo(ACTIVE_STATE eState, CRegionUser *pUser, UserActivityInfo *pInfo, bool bIsNewInfo, long *plParam)
{
	if(!pUser || !pInfo)
		return;

	pInfo->eState = eState;
	int iTime[Activity::ciActTimeCount] = {0};
	int iTemp = 0;
	rtSystemTime(iTime[0], iTime[1],
		iTemp, iTime[2], iTime[3],
		iTime[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
		pInfo->iUpdataTime[ii] = (short)iTime[ii];

	pUser->SetActivityJoin(m_eType, eState == AS_START ? true : false);
	if(!pUser->SetActivityInfo(m_eType, pInfo) && bIsNewInfo)
		DEL_ONE(pInfo);

	if(eState == AS_START)
	{
		ExecuteSuccessful(pUser, plParam);
	}
	if(eState == AS_OVER && IsScriptLoaded())
	{
		CallScript("Complate", false, pUser->m_dummy);
	}
}

void	CRegionActivityBase::ExecuteSuccessful(CRegionUser *pUser, long *plParam)
{
	if(IsScriptLoaded() && pUser)
	{
		PyObject *pArray = PyTuple_New(5);
		for(long i = 0; i < 5; i ++)
			PyTuple_SetItem( pArray, i, Py_BuildValue("l", plParam ? plParam[i] : 0) );
		CallScript("ExecuteSuccessful", false, pUser->m_dummy, pArray);
	}
}

void	CRegionActivityBase::ExecuteFailed(CRegionUser *pUser)
{
	if(IsScriptLoaded() && pUser)
		CallScript("ExecuteFailed", false, pUser->m_dummy);
}

//////////////////////////////////////////////////////////////////////////

ActivityTelpher::ActivityTelpher(int iID) :
CRegionActivityBase(iID)
{
	ActivityRes *pRes = g_TableActivity.FindActivity(iID);
	m_eType = ACTIVITY_TELPHER;
	DEL_ONE(m_pActivityInfo);
	if(pRes)
	{
		m_sActName = pRes->sActName;
		m_pActivityInfo = RT_NEW ActivityInfo(pRes->iLevel);
		if(!pRes->sScript.empty())
		{
			LoadScript(pRes->sScript.c_str());
			CallScript("OnCreate", false);
		}
	}
}

ActivityTelpher::~ActivityTelpher(void)
{
	//CRegionActivityBase::~CRegionActivityBase();
}

//////////////////////////////////////////////////////////////////////////

ActivityTreasure::ActivityTreasure(int iID) :
CRegionActivityBase(iID)
{
	ActivityRes *pRes = g_TableActivity.FindActivity(iID);
	m_eType = ACTIVITY_TREASURE;
	DEL_ONE(m_pActivityInfo);
	if(pRes)
	{
		m_sActName = pRes->sActName;
		m_pActivityInfo = RT_NEW ActivityInfo(pRes->iLevel);
		if(!pRes->sScript.empty())
		{
			LoadScript(pRes->sScript.c_str());
			CallScript("OnCreate", false);
		}
	}
}

ActivityTreasure::~ActivityTreasure(void)
{

}

//////////////////////////////////////////////////////////////////////////

ActivityCachotClass::ActivityCachotClass(int iID) :
CRegionActivityBase(iID)
{
	ActivityRes *pRes = g_TableActivity.FindActivity(iID);
	m_eType = ACTIVITY_CACHOT;
	DEL_ONE(m_pActivityInfo);
	if(pRes)
	{
		m_sActName = pRes->sActName;
		m_pActivityInfo = RT_NEW ActivityCachot(pRes->iLevel, pRes->iDegree);
		if(!pRes->sScript.empty())
		{
			LoadScript(pRes->sScript.c_str());
			CallScript("OnCreate", false);
		}
	}
}

ActivityCachotClass::~ActivityCachotClass(void)
{

}

bool	ActivityCachotClass::Execute(CRegionUser *pUser, long *plParam)
{
	return CRegionActivityBase::Execute(pUser, plParam);
	//if(!pUser || !m_pActivityInfo)
	//	return false;
	//if(m_pActivityInfo->eState != AS_START)
	//{
	//	pUser->m_dummy->SendSystemMessage(R(MSG_ACTIVITY_NOT_START));
	//	return false;
	//}

	//UserActivityInfo *pInfo = pUser->GetActivityInfo(m_eType);
	//if(pInfo)
	//{
	//	if(Compera(pInfo, pUser))
	//	{
	//		SetUserInfo(AS_START, pUser, pInfo, false, plParam);
	//		return true;
	//	}
	//	else
	//	{
	//		ExecuteFailed(pUser);
	//		return false;
	//	}
	//}
	//else
	//{
	//	GetUserInfo(pUser, AS_START, plParam);
	//	return true;
	//}
	//return false;
}

bool	ActivityCachotClass::Compera(UserActivityInfo *pInfo, CRegionUser *pUser)
{
	if(!m_pActivityInfo || !pInfo || !pUser || !pUser->m_dummy)
		return false;

	UserCachotInfo *pAInfo = dynamic_cast<UserCachotInfo*>(pInfo);
	bool bLess = pInfo->TimeLessEqual(m_pActivityInfo->iUpdataTime);
	if(bLess)
	{
		pAInfo->iDegree = 0;
	}
	else if(pAInfo->iDegree > 0)
		bLess = true;
	return bLess && m_pActivityInfo->CanJoinActivity(pUser->m_dummy->m_core.Lev, pInfo);
}

void	ActivityCachotClass::SetUserInfo(ACTIVE_STATE eState, CRegionUser *pUser, UserActivityInfo *pInfo, bool bIsNewInfo, long *plParam)
{
	if(!pUser || !pInfo)
		return;
	UserCachotInfo *pCachot = dynamic_cast<UserCachotInfo*>(pInfo);
	ActivityCachot *pAInfo = dynamic_cast<ActivityCachot*>(m_pActivityInfo);
	if(!pCachot || !pAInfo)
		return;

	pInfo->eState = eState;
	if(eState == AS_OVER)
		pCachot->iDegree++;
	int iTime[Activity::ciActTimeCount] = {0};
	int iTemp = 0;
	rtSystemTime(iTime[0], iTime[1],
		iTemp, iTime[2], iTime[3],
		iTime[4], iTemp, iTemp);
	for(int ii=0; ii<Activity::ciActTimeCount; ii++)
		pInfo->iUpdataTime[ii] = (short)iTime[ii];

	pUser->SetActivityJoin(m_eType, eState == AS_START ? true : false);
	if(!pUser->SetActivityInfo(m_eType, pInfo) && bIsNewInfo)
		DEL_ONE(pInfo);

	if(eState == AS_START)
	{
		if(pCachot->iDegree == pAInfo->iDegree)
			pCachot->iDegree = 0;
		ExecuteSuccessful(pUser, plParam);
	}
	if(eState == AS_OVER)
	{
		if(IsScriptLoaded())
			CallScript("Complate", false, pUser->m_dummy);
	}
}

//////////////////////////////////////////////////////////////////////////
